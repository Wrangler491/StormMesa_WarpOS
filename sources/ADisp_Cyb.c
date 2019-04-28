/* $Id: $ */

/*
 * Mesa 3-D graphics library
 * Copyright (C) 1995  Brian Paul  (brianp@ssec.wisc.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Note that you'll usually have to flip Y coordinates since Mesa's
 * window coordinates start at the bottom and increase upward.  Most
 * window system's Y-axis increases downward
 *
 * See dd.h for more device driver info.
 * See the other device driver implementations for ideas.
 *
 */

#include "AmigaIncludes.h"
#include <stdlib.h>
#include "GL/gl.h"

#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"
#include "types.h"

#include "AmigaMesa.h"
#include "ADisp_Cyb.h"
#include "ADisp_AGA.c"
#include "ADisp_SW.h"

#include "htnew_color.h"


#define MAX_POLYGON 300

#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

//#define DEBUGPRINT

#ifdef DEBUGPRINT
#define DEBUGOUT(x) /*LibPrintf*/ printf(x);
#else
#define DEBUGOUT(x) /*LibPrintf(x);*/
#endif

extern void amiga_standard_DD_pointers( GLcontext *ctx );

static void Cyb_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);
extern UBYTE* alloc_penbackarray( int width, int height, int bytes);
extern void destroy_penbackarray(UBYTE *buf);
extern struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
extern void destroy_rastport( struct RastPort *rp );
extern void AllocOneLine(struct amigamesa_context *AmigaMesaCreateContext);
extern void FreeOneLine(struct amigamesa_context *AmigaMesaCreateContext);
//#define RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

/**********************************************************************/
/*****                Miscellaneous device driver funcs                 *****/
/**********************************************************************/

static void cyb_finish( void )
{
    /* implements glFinish if possible */
}

void Dummy( void )
{
   //do nothing now
}

static void cyb_flush( void )
{
    /* implements glFlush if possible */
}


static void cyb_clear_index( GLcontext *ctx, GLuint index )
{
   /* implement glClearIndex */
   /* usually just save the value in the context struct */
   /*LibPrintf("aclear_index=glClearIndex=%d\n",index);*/
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel = ((unsigned long*)amesa->penconv)[index];
}


static void cyb_clear_color( GLcontext *ctx,
			  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   /* implement glClearColor */
   /* color components are floats in [0,1] */
   /* usually just save the value in the context struct */
   /*LibPrintf("aclear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
   /* @@@ TODO FREE COLOR IF NOT USED */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel=TC_RGBA(r,g,b,a);
}




static GLbitfield cyb_clear( GLcontext *ctx, GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

   if ((mask & GL_COLOR_BUFFER_BIT) && (!(ctx->Color.SWmasking)) )
   {

    if(all)
	{
	FillPixelArray (amesa->rp,FIXx(ctx->Viewport.X),
		FIXy(ctx->Viewport.Y)-ctx->Viewport.Height+1,/*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width/*-1*/,ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/,amesa->clearpixel);
	}
    else
	{
	if(amesa->rp!=0)
	    {
	    FillPixelArray (amesa->rp,FIXx(x),FIXy(y)-height+1,width,height,amesa->clearpixel);
	    }
	else
	    /*LibPrintf*/ printf("Serius error amesa->rp=0 detected in cyb_clear() in file cybmesa.c\n");
	}
	mask &= (~GL_COLOR_BUFFER_BIT);
   }
   return(mask);
}

static void cyb_set_index( GLcontext *ctx,GLuint index )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    /* Set the amesa color index. */
/*LibPrintf("aset_index(%d)\n",index);*/
    amesa->pixel = ((unsigned long*)amesa->penconv)[index];
}



static void cyb_set_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

/*LibPrintf("cyb_set_color(%d,%d,%d,%d)\n",r,g,b,a);*/

	/* Set the current RGBA color. */
	/* r is in 0..255.RedScale */
	/* g is in 0..255.GreenScale */
	/* b is in 0..255.BlueScale */
	/* a is in 0..255.AlphaScale */
    amesa->pixel = TC_RGBA(r,g,b,a);
}


/**********************************************************************/
/*****              Write spans of pixels                                   *****/
/**********************************************************************/

static void cyb_write_index_span32(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLuint index[],
					const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;
    unsigned long * penconv=(unsigned long*)amesa->penconv;
    register struct RastPort * rp=amesa->rp;

	REM(cyb_write_index_span32);
    y=FIXy(y);
    x=FIXx(x);
    if(dp = (ULONG*)amesa->imageline)
    {                     /* if imageline have been
			allocated then use fastversion */

	ant=0;
	    for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
		if (mask[i]) {
		    ant++;
		*dp = penconv[index[i]];dp++;
	    } else {
		if(ant)
		{
		    STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
		dp=(ULONG*)amesa->imageline;
		x += ant;
		ant=0;
		}
		x++;
	    }
	}
	if(ant)
	    STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);

    } else {            /* Slower */
	    for (i=0;i<n;i++,x++) {
		if (mask[i]) {
		 /* draw pixel (x[i],y[i]) using index[i] */
		WriteRGBPixel(rp,x,y,penconv[index[i]]);
	    }
	}
    }
}

static void cyb_write_index_span(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte index[],
					const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;
    unsigned long * penconv=(unsigned long*)amesa->penconv;
    register struct RastPort * rp=amesa->rp;
	REM(cyb_write_index_span);

    y=FIXy(y);
    x=FIXx(x);
    if(dp = (ULONG*)amesa->imageline)
    {                     /* if imageline have been
			allocated then use fastversion */

	ant=0;
	    for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
		if (mask[i]) {
		    ant++;
		*dp = penconv[index[i]];dp++;
	    } else {
		if(ant)
		    STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
		dp=(ULONG*)amesa->imageline;
		ant=0;
		x++;
	    }
	}
	if(ant)
	    STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);

    } else {            /* Slower */
	    for (i=0;i<n;i++,x++) {
		if (mask[i]) {
		 /* draw pixel (x[i],y[i]) using index[i] */
		WriteRGBPixel(rp,x,y,penconv[index[i]]);
	    }
	}
    }
}



static void cyb_write_color_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				CONST GLubyte rgba[][4],const GLubyte mask[] )

{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;

    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_color_span);
    y=FIXy(y);
    x=FIXx(x);

    if(dp = (ULONG*)amesa->imageline)
    {           /* if imageline allocated then use fastversion */
	if (mask)
	    {
	    ant=0;
	    for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
		{
		if (mask[i])
		    {
		    ant++;
		    *dp++ = ((GLint *)rgba)[i];
		    }
		else
		    {
		    if(ant)
			{
			STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_RGBA);
			dp=(ULONG*)amesa->imageline;
			x=x+ant;
			ant=0;
			}
		    x++;
		    }
		}

	    if(ant)
		{
		STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_RGBA);
		}
	    }
	else
	    {
	    for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
		{
		*dp++ = ((GLint *)rgba)[i];
		}
	    STACKWritePixelArray(amesa->imageline,0,0,4*n,rp,x,y,n,1,RECTFMT_RGBA);
	    }
	
	}
    else
	{    /* Slower version */
	if (mask)
	    {
	    /* draw some pixels */
	    for (i=0; i<n; i++, x++)
		{
		if (mask[i])
		    {
		    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
		    WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],rgba[i][3]));
		    }
		}
	    }
	else
	    {
	    /* draw all pixels */
	    for (i=0; i<n; i++, x++)
		{
		/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
		WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],rgba[i][3]));
		}
	    }
	}
}

static void cyb_write_color_span3( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				CONST GLubyte rgba[][3],const GLubyte mask[] )

{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;

    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_color_span3);

    y=FIXy(y);
    x=FIXx(x);

    if(dp = (ULONG*)amesa->imageline)
    {           /* if imageline allocated then use fastversion */
	if (mask)
	    {
	    ant=0;
	    for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
		{
		if (mask[i])
		    {
		    ant++;
		    *dp++ = TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],0);
		    }
		else
		    {
		    if(ant)
			{
			STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
			dp=(ULONG*)amesa->imageline;
			x=x+ant;
			ant=0;
			}
		    x++;
		    }
		}

	    if(ant)
		{
		STACKWritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
		}
	    }
	else
	    {
	    for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
		{
		*dp++ = TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],0);
		}
	    STACKWritePixelArray(amesa->imageline,0,0,4*n,rp,x,y,n,1,RECTFMT_ARGB);
	    }
	
	}
    else
	{    /* Slower version */
	if (mask)
	    {
	    /* draw some pixels */
	    for (i=0; i<n; i++, x++)
		{
		if (mask[i])
		    {
		    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
		    WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],0));
		    }
		}
	    }
	else
	    {
	    /* draw all pixels */
	    for (i=0; i<n; i++, x++)
		{
		/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
		WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],0));
		}
	    }
	}
}




static void cyb_write_monoindex_span(const GLcontext *ctx,GLuint n,GLint x,GLint y,const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,j;
    y=FIXy(y);
    x=FIXx(x);
	REM(cyb_write_monoindex_span);

    i=0;
    while(i<n)
	{
	while(!mask[i] && i<n)
	    {
	    i++;x++;
	    }

	if(i<n)
	    {
	    j=0;
	    while(mask[i] && i<n)
		{
		i++;j++;
		}
	    FillPixelArray (amesa->rp,x,y,j,1,amesa->pixel);
	    x+=j;
	    }
	}
}

static void cyb_write_monocolor_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
					     const GLubyte mask[])
{
    AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,j;
    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_monocolor_span);

    y=FIXy(y);
    x=FIXx(x);
    i=0;
    while(i<n)
	{
	while(!mask[i] && i<n)
	    {
	    i++;
	    x++;
	    }

	if(i<n)
	    {
	    j = 0;
	    while(mask[i] && i<n)
		{
		i++;
		j++;
		}
	    FillPixelArray(rp,x,y,j,1,amesa->pixel);
	    x += j;
	    }
	}
}


/**********************************************************************/
/*****                    Read spans of pixels                              *****/
/**********************************************************************/

/* Here we should check if the size of the colortable is <256 */

static void cyb_read_index_span( const GLcontext *ctx,GLuint n, GLint x, GLint y, GLuint index[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    int i;
    unsigned long color;


    y=FIXy(y);
    x=FIXx(x);

    if(amesa->imageline) {
	STACKReadPixelArray(amesa->imageline,0,0,4*n,amesa->rp,x,y,n,1,RECTFMT_ARGB);
	for(i=0; i<n; i++)
	{
	    color=((ULONG*)amesa->imageline)[i];
	    index[i]= FindNearestPen(amesa,
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)(color & 0xff),
		      (unsigned long *)amesa->ColorTable2);
	}
    } else {
	for (i=0; i<n; i++,x++)
	{
	    color = ReadRGBPixel(amesa->rp,x,y);
	    index[i]= FindNearestPen(amesa,
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)(color & 0xff),
		      (unsigned long *)amesa->ColorTable2);
	}
    }
}


static void cyb_read_color_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				      GLubyte rgba[][4])
{
    AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    ULONG col;


    y=FIXy(y);
    x=FIXx(x);

    if(amesa->imageline) {
	STACKReadPixelArray(amesa->imageline,0,0,n*4,amesa->rp,x,y,n,1,RECTFMT_RGBA);
	for(i=0; i<n; i++) {
	    ((GLint *)rgba)[i] = ((ULONG*)amesa->imageline)[i];
	}
    } else
	for (i=0; i<n; i++, x++) {
	    col=ReadRGBPixel(amesa->rp,x,y);
	    rgba[i][0] = (col & 0xff0000)>>16;
	    rgba[i][1] = (col & 0xff00)>>8;
	    rgba[i][2] = col & 0xff;
	    rgba[i][3] = (col & 0xff000000)>>24;
	    }
}




/**********************************************************************/
/*****                Write arrays of pixels                      *****/
/**********************************************************************/

static void cyb_write_index_pixels(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					  const GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_index_pixels);

    for (i=0; i<n; i++)
	{
	if (mask[i])
	    {
	    WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),((unsigned long*)amesa->penconv)[index[i]]);
	    }
	}
}


static void cyb_write_monoindex_pixels(const GLcontext *ctx, GLuint n,
						const GLint x[], const GLint y[],
						const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_monoindex_pixels);
	
    for (i=0; i<n; i++) {
	if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
	WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);

	}
    }
}



static void cyb_write_color_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					  CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_color_pixels);
printf("Writing %d pixels!\n",n);

    for (i=0; i<n; i++) {
	if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
	WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),TC_RGBA(rgba[i][0],rgba[i][1],rgba[i][2],rgba[i][3]));
	}
    }
}



static void cyb_write_monocolor_pixels(const GLcontext *ctx, GLuint n,
						const GLint x[], const GLint y[],
						const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
	REM(cyb_write_monocolor_pixels);

    for (i=0; i<n; i++) {
	if (mask[i]) {
/*        write pixel x[i], y[i] using current color*/
	WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);
	}
    }
}




/**********************************************************************/
/*****                       Read arrays of pixels                          *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void cyb_read_index_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    unsigned long color;
    register struct RastPort * rp = amesa->rp;


  for (i=0; i<n; i++) {
      if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
	color = ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i]));
	index[i]= FindNearestPen(amesa,
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)(color & 0xff),
		      (unsigned long *)amesa->ColorTable2);
      }
  }
}

static void cyb_read_color_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLubyte rgba[][4], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,col;
    register struct RastPort * rp = amesa->rp;


    for (i=0; i<n; i++)
	{
	if (mask[i])
	    {
	    col=ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i]));

	    rgba[i][0]  = (col&0xff0000)>>16;
	    rgba[i][1] = (col&0xff00)>>8;
	    rgba[i][2] = col&0xff;
	    rgba[i][3] = (col&0xff000000)>>24;
	}
    }
}


/**********************************************************************/
/**********************************************************************/

static GLboolean cyb_set_buffer( GLcontext *ctx,GLenum mode )
{
    /* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
    /* for success/failure */
    
/*   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx; */


/* TODO implemed a set of buffers */
    if (mode==GL_FRONT)
	{
	return(GL_TRUE);
	}
    else if (mode==GL_BACK)
	{
	return(GL_TRUE);
	}
    else
	{
	return(GL_FALSE);
	}
}



/* ctx->Driver.RendererString() */
const char *cyb_rendererstring(void)
{
	return("AMIGA_ARGB_SB");
}

/**********************************************************************/
/**********************************************************************/
    /* Initialize all the pointers in the DD struct.  Do this whenever   */
    /* a new context is made current or we change buffers via set_buffer! */
void cyb_standard_DD_pointers( GLcontext *ctx )
{
	REM(cyb_standard_DD_pointers);
   ctx->Driver.RendererString = cyb_rendererstring;

   ctx->Driver.UpdateState = cyb_standard_DD_pointers;
   ctx->Driver.ClearIndex = cyb_clear_index;
   ctx->Driver.ClearColor = cyb_clear_color;
   ctx->Driver.Clear = cyb_clear;

   ctx->Driver.Index = cyb_set_index;
   ctx->Driver.Color = cyb_set_color;

   ctx->Driver.SetBuffer = cyb_set_buffer;
   ctx->Driver.GetBufferSize = Cyb_Standard_resize;

   /* Pixel/span writing functions: */
   ctx->Driver.WriteRGBASpan       = cyb_write_color_span;
   ctx->Driver.WriteRGBSpan        = cyb_write_color_span3;
   ctx->Driver.WriteMonoRGBASpan   = cyb_write_monocolor_span;
   ctx->Driver.WriteRGBAPixels     = cyb_write_color_pixels;
   ctx->Driver.WriteMonoRGBAPixels = cyb_write_monocolor_pixels;
   ctx->Driver.WriteCI32Span       = cyb_write_index_span32;
   ctx->Driver.WriteCI8Span        = cyb_write_index_span;
   ctx->Driver.WriteMonoCISpan     = cyb_write_monoindex_span;
   ctx->Driver.WriteCI32Pixels     = cyb_write_index_pixels;
   ctx->Driver.WriteMonoCIPixels   = cyb_write_monoindex_pixels;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadCI32Span        = cyb_read_index_span;
   ctx->Driver.ReadRGBASpan        = cyb_read_color_span;
   ctx->Driver.ReadCI32Pixels      = cyb_read_index_pixels;
   ctx->Driver.ReadRGBAPixels      = cyb_read_color_pixels;
}

/**********************************************************************/
/*****                  Amiga/Mesa private misc procedures        *****/
/**********************************************************************/




static void
Cyb_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
    {
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    *width=amesa->width;
    *height=amesa->height;
	if (amesa->rp == NULL)
		return;

    if(!((  amesa->width  == (amesa->front_rp->Layer->bounds.MaxX-amesa->front_rp->Layer->bounds.MinX-amesa->left-amesa->right) )
    &&  ( amesa->height == (amesa->front_rp->Layer->bounds.MaxY-amesa->front_rp->Layer->bounds.MinY-amesa->bottom-amesa->top)  )))
	{
	FreeOneLine(amesa);

	amesa->FixedWidth =amesa->RealWidth =amesa->front_rp->Layer->bounds.MaxX-amesa->front_rp->Layer->bounds.MinX;
	amesa->FixedHeight=amesa->RealHeight=amesa->front_rp->Layer->bounds.MaxY-amesa->front_rp->Layer->bounds.MinY;

	*width=amesa->width = amesa->RealWidth-amesa->left-amesa->right;
	*height=amesa->height = amesa->RealHeight-amesa->bottom-amesa->top;
	amesa->depth = GetCyberMapAttr(amesa->front_rp->BitMap,CYBRMATTR_DEPTH);

	if (amesa->visual->db_flag) {
	    if (amesa->back_rp) {   /* Free double buffer */
		destroy_rastport(amesa->back_rp);
	    }
	    if((amesa->back_rp = make_rastport(amesa->RealWidth,amesa->RealHeight,amesa->depth,amesa->rp->BitMap))==NULL) {
		amesa->rp = amesa->front_rp;
		/*LibPrintf*/ printf("To little mem free. Couldn't allocate Dubblebuffer in this size.\n");
	    } else {
		amesa->rp=amesa->back_rp;
	    }
	}

	AllocOneLine(amesa);
	}
    }

/**********************************************************************/
/*****                  Amiga/Mesa private init/despose/resize       *****/
/**********************************************************************/


BOOL
Cyb_Standard_init(struct amigamesa_context *c,struct TagItem *tagList)
    {
/*	Wrangler 27/4/19 MaxX and MinX return 0 if no window! */
	c->left = GetTagData(AMA_Left,0,tagList);
	c->bottom= GetTagData(AMA_Bottom,0,tagList);
    c->front_rp =c->rp;
    c->back_rp=NULL;

	if(c->window) {
	    c->FixedWidth =c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX;
	    c->FixedHeight=c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY;
	    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
	    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);

	    c->right = c->RealWidth - c->width - c->left;   /* 3.1.1998 added by Sam Jordan */
	    c->top = c->RealHeight - c->height - c->bottom;
	} else {
		c->right = 0;
		c->top = 0;
	    c->width = GetTagData(AMA_Width,0,tagList);
	    c->height= GetTagData(AMA_Height,0,tagList);
		if(!c->width||!c->height) return FALSE;
		c->RealWidth = c->FixedWidth = c->width + c->right + c->left;
		c->RealHeight = c->FixedHeight = c->height + c->top + c->bottom;
	}

printf("FWxFH: %dx%d, RWxRH: %dx%d, WxH: %dx%d, top: %d, bottom %d, l %d, r %d\n", \
	c->FixedWidth, c->FixedHeight, c->RealWidth, c->RealHeight, c->width, c->height, \
	c->top, c->bottom, c->left, c->right);

    if (CyberGfxBase) {
		if(IsCyberModeID(GetVPModeID(&c->Screen->ViewPort)))
		{
printf("Cyber screen found\n");
		c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
		} 
    	else
		{
printf("Cyber screen not found\n");
		c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
		}
	} else {
		c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
	}
printf("Depth: %d\n",c->depth);
if(!c->depth) return NULL;
    c->pixel = 0;   /* current drawing pen */

printf("CSI: phase 1\n");
    if (c->depth<=8)
    {
	if (c->visual->rgb_flag)
	{
	    if (!(CreateTransTable(c)))
		return(FALSE);
	}
	else
	{
	    if (!(InitTransTable(c)))
		return(FALSE);
	}
    }
printf("CSI: phase 2\n");

    if (c->visual->db_flag==GL_TRUE)
	{
	if((c->back_rp = make_rastport(c->RealWidth,c->RealHeight,c->depth, c->rp->BitMap))!=NULL)
	    {
	    ((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_BACK;
	    c->rp = c->back_rp;
	    }
	else
	    {
	    /*LibPrintf*/ printf("make_rastport Faild\n");
	    ((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_FRONT;
	    }
	}
    else
	{
	((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_FRONT;
	}
    AllocOneLine(c); /* A linebuffer for WritePixelLine */
printf("CSI: phase 3\n");

    if (c->depth<=8) {
	/* JAM: Added alloc_temp_rastport */
	alloc_temp_rastport(c);
    }
printf("CSI: phase 4\n");

    c->InitDD=c->depth<=8 ? (void(*)(void *))amiga_standard_DD_pointers : (void(*)(void *))cyb_standard_DD_pointers;  /*standard drawing*/
    c->Dispose=Amiga_Standard_Dispose;
    c->SwapBuffer=Amiga_Standard_SwapBuffer;

    if (c->depth > 8)
    {
      if (!(c->visual->rgb_flag))
      {
	int i;

#ifndef __PPC__
	if (!(c->ColorTable2 = AllocVec(256*sizeof(long),MEMF_PUBLIC|MEMF_CLEAR)))
#else
	if (!(c->ColorTable2 = AllocVecPPC(256*sizeof(long),MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
		return(0);
	for (i=0;i<=256;i++)
	    c->ColorTable2[i] = -1;
      }
    }
printf("CSI: phase 5\n");

    return(TRUE);
    }


#undef DEBUGPRINT

