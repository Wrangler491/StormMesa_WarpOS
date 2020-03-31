/* $Id: $*/

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
#include "ADisp_AGA.h"
#include "ADisp_SW.h"
#include "htnew_color.h"

#ifdef AGA
#define c8to32(x) ((x)<<24)
#else
#define c8to32(x) (((((((x)<<8)|(x))<<8)|(x))<<8)|(x))
#endif

#define MAX_POLYGON 300
//#define AGA 1
// TrueColor-RGBA
#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

#define DEBUGPRINT

#ifdef DEBUGPRINT
#define DEBUGOUT(x) LibPrintf(x);
#else
#define DEBUGOUT(x) /*LibPrintf(x);*/
#endif

static void Amiga_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void destroy_rastport( struct RastPort *rp );
BOOL make_temp_raster( struct RastPort *rp );
void destroy_temp_raster( struct RastPort *rp );
void AllocOneLine(struct amigamesa_context *AmigaMesaCreateContext);
void FreeOneLine(struct amigamesa_context *AmigaMesaCreateContext);

BOOL alloc_temp_rastport(struct amigamesa_context * c);
void free_temp_rastport(struct amigamesa_context * c);


#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight-amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth*FIXy(y)+FIXx(x)))


/**********************************************************************/
/*****                Miscellaneous device driver funcs           *****/
/**********************************************************************/

static void afinish( void )
{
	/* implements glFinish if possible */
}


static void aflush( void )
{
	/* implements glFlush if possible */
}


static void aclear_index( GLcontext *ctx, GLuint index )
{
   /* implement glClearIndex */
   /* usually just save the value in the context struct */
   /*LibPrintf("aclear_index=glClearIndex=%d\n",index);*/
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

   if (amesa->specialalloc)
	ReleasePen(amesa->Screen->ViewPort.ColorMap, amesa->clearpixel);
   amesa->specialalloc = 0;
   amesa->clearpixel = amesa->penconv[index];
}


static void aclear_color( GLcontext *ctx,
						  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   /* implement glClearColor */
   /* color components are floats in [0,1] */
   /* usually just save the value in the context struct */
   /*LibPrintf("aclear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
   /* @@@ TODO FREE COLOR IF NOT USED */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
/*   amesa->clearpixel=EvalPen(amesa,r,g,b); */
/*  amesa->clearpixel=RGBA(r,g,b,a);  Orginal */  
    if (amesa->specialalloc)
	ReleasePen(amesa->Screen->ViewPort.ColorMap, amesa->clearpixel);
    amesa->clearpixel = (unsigned long)ObtainBestPen(amesa->Screen->ViewPort.ColorMap,
				      r << 24, g << 24, b << 24,
				      OBP_Precision, PRECISION_EXACT,
				      TAG_DONE);
    amesa->specialalloc = 1;
}




static GLbitfield aclear( GLcontext *ctx,GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    if ((mask & GL_COLOR_BUFFER_BIT) && (!(ctx->Color.SWmasking)) )
    {

	SetAPen(amesa->rp,amesa->clearpixel);
	if(all)
		{

		RectFill(amesa->rp,FIXx(ctx->Viewport.X),FIXy(ctx->Viewport.Y)-ctx->Viewport.Height+1,FIXx(ctx->Viewport.X)+ctx->Viewport.Width-1,FIXy(ctx->Viewport.Y));
		}
	else
		{
		if(amesa->rp!=0)
			{
			RectFill(amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y));
			}
		else
			LibPrintf("Serius error amesa->rp=0 detected in aclear() in file amigamesa.c\n");
		}
	
	mask &= (~GL_COLOR_BUFFER_BIT);
    }
    return(mask);
}

static void aset_index( GLcontext *ctx,GLuint index )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	/* Set the amesa color index. */
	amesa->pixel = amesa->penconv[index];
}



static void aset_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;


		/* Set the current RGBA color. */
		/* r is in 0..255.RedScale */
		/* g is in 0..255.GreenScale */
		/* b is in 0..255.BlueScale */
		/* a is in 0..255.AlphaScale */
	amesa->pixel = EvalPen(amesa,r,g,b);
	amesa->pixelargb[0] = r;
	amesa->pixelargb[1] = g;
	amesa->pixelargb[2] = b;
	amesa->pixelargb[3] = a;
}



/**********************************************************************/
/*****              Write spans of pixels                                   *****/
/**********************************************************************/
static void awrite_index_span(const GLcontext *ctx, GLuint n, GLint x, GLint y,
			      const GLubyte index[],
			      const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
//        unsigned long * penconv=(unsigned long *)amesa->penconv;
	UBYTE *penconv = amesa->penconv;
	register struct RastPort * rp=amesa->rp;


	y=FIXy(y);
	x=FIXx(x);
	/* JAM: Added correct checking */
	/* if((dp = amesa->imageline) && amesa->tmpras) */
	if((dp = amesa->imageline) && amesa->temprp)
	{                     /* if imageline and temporary rastport have been
						allocated then use fastversion */

		ant=0;
			for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
				if (mask[i]) {
					ant++;
//              x++;
				*dp = penconv[index[i]];
				dp++;
			} else {
				if(ant)
				{
					/* JAM: Replaced by correct call */
					/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
					WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				dp=amesa->imageline;
				x = x + ant;
				ant=0;
				}
			    x++;
			}
		}
		if(ant)
			/* JAM: Replaced by correct call */
			/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
			WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);

	} else {            /* Slower */
			for (i=0;i<n;i++,x++) {
				if (mask[i]) {
				 /* draw pixel (x[i],y[i]) using index[i] */
				SetAPen(rp,penconv[index[i]]);
				WritePixel(rp,x,y);
			}
		}
	}
}

static void awrite_index_span32(const GLcontext *ctx, GLuint n, GLint x, GLint y,
			      const GLuint index[],
			      const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
//        unsigned long * penconv=(unsigned long *)amesa->penconv;
	UBYTE *penconv = amesa->penconv;
	register struct RastPort * rp=amesa->rp;


	y=FIXy(y);
	x=FIXx(x);
	/* JAM: Added correct checking */
	/* if((dp = amesa->imageline) && amesa->tmpras) */
	if((dp = amesa->imageline) && amesa->temprp)
	{                     /* if imageline and temporary rastport have been
						allocated then use fastversion */

		ant=0;
			for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
				if (mask[i]) {
					ant++;
//              x++;
				*dp = penconv[index[i]];
				dp++;
			} else {
				if(ant)
				{
					/* JAM: Replaced by correct call */
					/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
					WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				dp=amesa->imageline;
				x = x + ant;
				ant=0;
				}
			    x++;
			}
		}
		if(ant)
			/* JAM: Replaced by correct call */
			/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
			WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);

	} else {            /* Slower */
			for (i=0;i<n;i++,x++) {
				if (mask[i]) {
				 /* draw pixel (x[i],y[i]) using index[i] */
				SetAPen(rp,penconv[index[i]]);
				WritePixel(rp,x,y);
			}
		}
	}
}

static void awrite_color_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				CONST GLubyte rgba[][4],const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
	int xd = x;
	int yd = y;
	register struct RastPort * rp = amesa->rp;


	y=FIXy(y);
	x=FIXx(x);

	if((dp = amesa->imageline) && amesa->temprp)
	{           /* if imageline allocated then use fastversion */
		if (mask)
			{
			ant=0;
			for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
				{
				xd++;
				if (mask[i])
					{
					ant++;
					*dp = EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd);
					dp++;
					}
				else
					{
					if(ant)
						{
						WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
						dp=amesa->imageline;
						x=x+ant;
						ant=0;
						}
					x++;
					}
				}

			if(ant)
				{
				WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				}
			}
		else
			{
			for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
				{
				xd++;
				*dp = EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd);
				dp++;
				}
			WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
			}
		
		}
	else
		{    /* Slower version */
		if (mask)
			{
			/* draw some pixels */
			for (i=0; i<n; i++, x++)
				{
				xd++;
				if (mask[i])
					{
					/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
					SetAPen(rp,EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd));
					WritePixel(rp,x,y);
					}
				}
			}
		else
			{
			/* draw all pixels */
			for (i=0; i<n; i++, x++)
				{
				/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
				xd++;
				SetAPen(rp,EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd));
				WritePixel(rp,x,y);
				}
			}
		}
}

static void awrite_color_span3( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				CONST GLubyte rgba[][3],const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
	int xd = x;
	int yd = y;
	register struct RastPort * rp = amesa->rp;


	y=FIXy(y);
	x=FIXx(x);

	if((dp = amesa->imageline) && amesa->temprp)
	{           /* if imageline allocated then use fastversion */
		if (mask)
			{
			ant=0;
			for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
				{
				xd++;
				if (mask[i])
					{
					ant++;
					*dp = EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd);
					dp++;
					}
				else
					{
					if(ant)
						{
						WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
						dp=amesa->imageline;
						x=x+ant;
						ant=0;
						}
					x++;
					}
				}

			if(ant)
				{
				WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				}
			}
		else
			{
			for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
				{
				xd++;
				*dp = EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd);
				dp++;
				}
			WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
			}
		
		}
	else
		{    /* Slower version */
		if (mask)
			{
			/* draw some pixels */
			for (i=0; i<n; i++, x++)
				{
				xd++;
				if (mask[i])
					{
					/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
					SetAPen(rp,EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd));
					WritePixel(rp,x,y);
					}
				}
			}
		else
			{
			/* draw all pixels */
			for (i=0; i<n; i++, x++)
				{
				/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
				xd++;
				SetAPen(rp,EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd));
				WritePixel(rp,x,y);
				}
			}
		}
}



static void awrite_monoindex_span(const GLcontext *ctx,GLuint n,GLint x,GLint y,const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	y=FIXy(y);
	x=FIXx(x);
	SetAPen(amesa->rp,amesa->pixel);

	i=0;
	while(i<n)
		{
		while(!mask[i] && i<n)
			{
			i++;x++;
			}

		if(i<n)
			{
			Move(amesa->rp,x,y);
			while(mask[i] && i<n)
				{
				i++;x++;
				}
			Draw(amesa->rp,x-1,y);
			}
		}
}

static void awrite_monocolor_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
							const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
	UBYTE r,g,b;
	int xd = x;
	int yd = y;
	register struct RastPort * rp = amesa->rp;

	r = amesa->pixelargb[0];
	g = amesa->pixelargb[1];
	b = amesa->pixelargb[2];
	y=FIXy(y);
	x=FIXx(x);

	if((dp = amesa->imageline) && amesa->temprp)
	{           /* if imageline allocated then use fastversion */
		if (mask)
			{
			ant=0;
			for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
				{
				xd++;
				if (mask[i])
					{
					ant++;
					*dp = EvalPen_D(amesa,r,g,b,xd,yd);
					dp++;
					}
				else
					{
					if(ant)
						{
						WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
						dp=amesa->imageline;
						x=x+ant;
						ant=0;
						}
					x++;
					}
				}

			if(ant)
				{
				WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				}
			}
		else
			{
			for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
				{
				xd++;
				*dp = EvalPen_D(amesa,r,g,b,xd,yd);
				dp++;
				}
			WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
			}
		
		}
	else
		{    /* Slower version */
		if (mask)
			{
			/* draw some pixels */
			for (i=0; i<n; i++, x++)
				{
				xd++;
				if (mask[i])
					{
					/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
					SetAPen(rp,EvalPen_D(amesa,r,g,b,xd,yd));
					WritePixel(rp,x,y);
					}
				}
			}
		else
			{
			/* draw all pixels */
			for (i=0; i<n; i++, x++)
				{
				/* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
				xd++;
				SetAPen(rp,EvalPen_D(amesa,r,g,b,xd,yd));
				WritePixel(rp,x,y);
				}
			}
		}
}


/**********************************************************************/
/*****                    Read spans of pixels                              *****/
/**********************************************************************/

static void aread_index_span( const GLcontext *ctx,GLuint n, GLint x, GLint y, GLuint index[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	int i;


	y=FIXy(y);
	x=FIXx(x);
	if(amesa->temprp && amesa->imageline) {
		ReadPixelLine8(amesa->rp,x,y,n,amesa->imageline,amesa->temprp);
		for(i=0; i<n; i++)
			index[i]=amesa->penconvinv[amesa->imageline[i]];
	} else {
		for (i=0; i<n; i++,x++)
			index[i] = amesa->penconvinv[ReadPixel(amesa->rp,x,y)];
	}
}

static void aread_color_span( const GLcontext *ctx,GLuint n, GLint x, GLint y,
				GLubyte rgba[][4])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,col;
	ULONG ColTab[3];
	struct ColorMap * cm=amesa->Screen->ViewPort.ColorMap;


	y=FIXy(y);
	x=FIXx(x);

	if(amesa->imageline && amesa->temprp) {
		ReadPixelLine8(amesa->rp,x,y,n,amesa->imageline,amesa->temprp);
		for(i=0; i<n; i++) {
			GetRGB32(cm,amesa->imageline[i],1,ColTab);
			rgba[i][0]  = ColTab[0]>>24;
			rgba[i][1] = ColTab[1]>>24;
			rgba[i][2] = ColTab[2]>>24;
			rgba[i][3] =255;
		}
	} else
		for (i=0; i<n; i++, x++) {
			col=ReadPixel(amesa->rp,x,y);
			GetRGB32(cm,col,1,ColTab);

			rgba[i][0]  = ColTab[0]>>24;
			rgba[i][1] = ColTab[1]>>24;
			rgba[i][2] = ColTab[2]>>24;
			rgba[i][3] =255;
			}
}




/**********************************************************************/
/*****                Write arrays of pixels                                *****/
/**********************************************************************/
static void awrite_index_pixels(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
				const GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			SetAPen(rp,amesa->penconv[index[i]]);
			WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
			}
		}
}


static void awrite_monoindex_pixels(const GLcontext *ctx, GLuint n,
												const GLint x[], const GLint y[],
												const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

		SetAPen(rp,amesa->pixel);

	for (i=0; i<n; i++) {
		if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));

		}
	}
}



static void awrite_color_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
				CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;
	int xd = (int)x;
	int yd = (int)y;


	for (i=0; i<n; i++) {
		xd++;
		if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
		SetAPen(rp,EvalPen_D(amesa,rgba[i][0],rgba[i][1],rgba[i][2],xd,yd));
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
		}
	}
}



static void awrite_monocolor_pixels(const GLcontext *ctx, GLuint n,
				    const GLint x[], const GLint y[],
				    const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	UBYTE r,g,b;
	register struct RastPort * rp = amesa->rp;

	r = amesa->pixelargb[0];
	g = amesa->pixelargb[1];
	b = amesa->pixelargb[2];

		SetAPen(rp,amesa->pixel);

	for (i=0; i<n; i++) {
		if (mask[i]) {
/*        write pixel x[i], y[i] using current color*/
		SetAPen(rp,EvalPen_D(amesa,r,g,b,x[i],y[i]));
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
		}
	}
}




/**********************************************************************/
/*****                       Read arrays of pixels                          *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void aread_index_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
										 GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;


  for (i=0; i<n; i++) {
	  if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
		index[i] = amesa->penconvinv[ReadPixel(rp,FIXx(x[i]),FIXy(y[i]))];
	  }
  }
}

static void aread_color_pixels( const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
				GLubyte rgba[][4], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,col;
	register struct RastPort * rp = amesa->rp;

	ULONG ColTab[3];


	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			col=ReadPixel(rp,FIXx(x[i]),FIXy(y[i]));

			GetRGB32(amesa->Screen->ViewPort.ColorMap,col,1,ColTab);

			rgba[i][0]  = ColTab[0]>>24;
			rgba[i][1] = ColTab[1]>>24;
			rgba[i][2] = ColTab[2]>>24;
			rgba[i][3] =255;
		}
	}
}



static GLboolean aset_buffer( GLcontext *ctx,GLenum mode )
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
const char *amiga_rendererstring(void)
{
	return("AMIGA_LUT8_SB");
}

/**********************************************************************/
/**********************************************************************/
	/* Initialize all the pointers in the DD struct.  Do this whenever   */
	/* a new context is made current or we change buffers via set_buffer! */
void amiga_standard_DD_pointers( GLcontext *ctx )
{
   ctx->Driver.RendererString = amiga_rendererstring;
   ctx->Driver.UpdateState = amiga_standard_DD_pointers;
   ctx->Driver.ClearIndex = aclear_index;
   ctx->Driver.ClearColor = aclear_color;
   ctx->Driver.Clear = aclear;

   ctx->Driver.Index = aset_index;
   ctx->Driver.Color = aset_color;

   ctx->Driver.SetBuffer = aset_buffer;
   ctx->Driver.GetBufferSize = Amiga_Standard_resize;

   /* Pixel/span writing functions: */
   ctx->Driver.WriteRGBASpan       = awrite_color_span;
   ctx->Driver.WriteRGBSpan        = awrite_color_span3;
   ctx->Driver.WriteMonoRGBASpan   = awrite_monocolor_span;
   ctx->Driver.WriteRGBAPixels     = awrite_color_pixels;
   ctx->Driver.WriteMonoRGBAPixels = awrite_monocolor_pixels;
   ctx->Driver.WriteCI32Span       = awrite_index_span32;
   ctx->Driver.WriteCI8Span        = awrite_index_span;
   ctx->Driver.WriteMonoCISpan     = awrite_monoindex_span;
   ctx->Driver.WriteCI32Pixels     = awrite_index_pixels;
   ctx->Driver.WriteMonoCIPixels   = awrite_monoindex_pixels;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadCI32Span        = aread_index_span;
   ctx->Driver.ReadRGBASpan        = aread_color_span;
   ctx->Driver.ReadCI32Pixels      = aread_index_pixels;
   ctx->Driver.ReadRGBAPixels      = aread_color_pixels;
}

/**********************************************************************/
/*****                  Amiga/Mesa Private Functions                        *****/
/**********************************************************************/
/* JAM: temporary rastport allocation and freeing */

BOOL alloc_temp_rastport(struct amigamesa_context * c) {
	struct RastPort * temprp;

#ifndef __PPC__
	if(temprp= AllocVec(sizeof(struct RastPort), 0)) {
#else
	if(temprp= AllocVecPPC(sizeof(struct RastPort), 0,0)) {
#endif
		CopyMem(c->rp, temprp, sizeof(struct RastPort));
		temprp->Layer = NULL;

		if(temprp->BitMap= AllocBitMap(c->FixedWidth, 1,
										 c->rp->BitMap->Depth,0,c->rp->BitMap)) {
			c->temprp=temprp;
			return TRUE;
		}
		FreeVec(temprp);
	}
	LibPrintf("Error allocating temporary rastport");
	return FALSE;
}

 void free_temp_rastport(struct amigamesa_context * c) {
	if(c->temprp) {
		FreeBitMap(c->temprp->BitMap);
#ifndef __PPC__
		FreeVec(c->temprp);
#else
		FreeVecPPC(c->temprp);
#endif
	}
	c->temprp=NULL;
}

/*
 * Create a new rastport to use as a back buffer.
 * Input:  width, height - size in pixels
 *        depth - number of bitplanes
 */

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm )
	{
	struct RastPort *rp;
	struct BitMap *bm;

	if (bm=AllocBitMap(width,height,depth,BMF_CLEAR|BMF_INTERLEAVED,friendbm))
		{
		if (rp = (struct RastPort *) malloc( sizeof(struct RastPort)))
			{
			InitRastPort( rp );
			rp->BitMap = bm;
			return rp;
			}
		else
			{
			FreeBitMap(bm);
			return 0;
			}
		}
	else
		return 0;
}


/*
 * Deallocate a rastport.
 */

void destroy_rastport( struct RastPort *rp )
	{
	WaitBlit();
	FreeBitMap( rp->BitMap );
	free( rp );
	}

	/* 
	** Color_buf is a array of pens equals the drawing area
	** it's for faster dubbelbuffer rendering
	** Whent it's time for bufferswitch just use c2p and copy.
	*/

UBYTE* alloc_penbackarray( int width, int height, int bytes)
	{
#ifndef __PPC__
	return(AllocVec(width*height*bytes, MEMF_PUBLIC|MEMF_CLEAR));
#else
	return(AllocVecPPC(width*height*bytes, MEMF_PUBLIC|MEMF_CLEAR,0));
#endif
	}

void destroy_penbackarray(UBYTE *buf)
	{
#ifndef __PPC__
	FreeVec(buf);
#else
	FreeVecPPC(buf);
#endif
	}



/*
 * Construct a temporary raster for use by the given rasterport.
 * Temp rasters are used for polygon drawing.
 *
 * Not used at the moment
 */

BOOL make_temp_raster( struct RastPort *rp ) {
	BOOL    OK=TRUE;
	unsigned long width, height;
	PLANEPTR p;
	struct TmpRas *tmpras;

	if(rp==0) {
		LibPrintf("Zero rp\n");
		return(FALSE);
	}
		width = rp->BitMap->BytesPerRow*8;
		height = rp->BitMap->Rows;

	/* allocate structures */
		if(p = AllocRaster( width, height )) {
			if(tmpras = (struct TmpRas *) AllocVec( sizeof(struct TmpRas),
													MEMF_ANY)) {
			if(InitTmpRas( tmpras, p, ((width+15)>>4)*height )) {
				rp->TmpRas = tmpras;
			} else
					OK=FALSE;
			} else
			OK=FALSE;
	} else
		return(FALSE);

	if (OK)
		return(TRUE);
	else {
		LibPrintf("Error when allocationg TmpRas\n");
		if (tmpras)
			FreeVec(tmpras);
		if (p)
			FreeRaster(p,width, height);
		return(FALSE);
		}
}

/* not used now */

static BOOL allocarea(struct RastPort *rp ) {
	BOOL    OK=TRUE;
	struct AreaInfo *areainfo;
	UWORD *pattern;
	APTR vbuffer;

	areainfo = (struct AreaInfo *) AllocVec( sizeof(struct AreaInfo),MEMF_ANY );
	if(areainfo!=0) {
		pattern = (UWORD *) AllocVec( sizeof(UWORD),MEMF_ANY);
		if(pattern!=0) {
			*pattern = 0xffff;      /*@@@ org: 0xffffffff*/
			vbuffer = (APTR) AllocVec( MAX_POLYGON * 5 * sizeof(WORD),MEMF_ANY);
			if(vbuffer!=0) {
				/* initialize */
				InitArea( areainfo, vbuffer, MAX_POLYGON );
				/* bind to rastport */
				rp->AreaPtrn = pattern;
				rp->AreaInfo = areainfo;
				rp->AreaPtSz = 0;
			} else
				OK=FALSE;
		} else
				OK=FALSE;
	} else
			OK=FALSE;

	if (OK)
		return (OK);
	else {
		LibPrintf("Error when allocationg AreaBuffers\n");
		if (vbuffer)
			FreeVec(vbuffer);
		if (pattern)
			FreeVec(pattern);
		if (areainfo)
			FreeVec(areainfo);
		return(OK);
		}
}

/* not used now */

void freearea(struct RastPort *rp) {
	if (rp->AreaInfo) {
		if (rp->AreaInfo->VctrTbl)
			FreeVec(rp->AreaInfo->VctrTbl);
		if (rp->AreaPtrn) {
			FreeVec(rp->AreaPtrn);
			rp->AreaPtrn=NULL;
		}
		FreeVec(rp->AreaInfo);
		rp->AreaInfo=NULL;
	}
}

/*
 * Destroy a temp raster.
 */


void destroy_temp_raster( struct RastPort *rp )
	{
	/* bitmap */

	unsigned long width, height;

	width = rp->BitMap->BytesPerRow*8;
	height = rp->BitMap->Rows;

	if (rp->TmpRas)
		{
		if(rp->TmpRas->RasPtr)
			FreeRaster( rp->TmpRas->RasPtr,width,height );
		FreeVec( rp->TmpRas );
		rp->TmpRas=NULL;

		}
}


void AllocOneLine( struct amigamesa_context *c) {
	if(c->imageline)
		FreeVec(c->imageline);
	if (c->depth<=8) {

#ifndef __PPC__
		c->imageline = AllocVec((c->width+15) & 0xfffffff0,MEMF_ANY);   /* One Line */
#else
		c->imageline = AllocVecPPC((c->width+15) & 0xfffffff0,MEMF_ANY,0);   /* One Line */
#endif
	} else {
#ifndef __PPC__
		c->imageline = AllocVec(((c->width+3) & 0xfffffffc)*4,MEMF_ANY);   /* One Line */
#else
		c->imageline = AllocVecPPC(((c->width+3) & 0xfffffffc)*4,MEMF_ANY,0);   /* One Line */
#endif
	}
}

void FreeOneLine( struct amigamesa_context *c) {
	if(c->imageline) {
#ifndef __PPC__
		FreeVec(c->imageline);
#else
		FreeVecPPC(c->imageline);
#endif
		c->imageline=NULL;
	}
}
/**********************************************************************/
/*****                  Amiga/Mesa private init/despose/resize       *****/
/**********************************************************************/

static void
Amiga_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
	{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

DEBUGOUT("Amiga_Standard_resize\n")
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
		amesa->depth = GetBitMapAttr(amesa->front_rp->BitMap,BMA_DEPTH);

		/* JAM: Added free_temp_rastport() */
		free_temp_rastport(amesa);

		if (amesa->visual->db_flag) {
			if (amesa->back_rp) {   /* Free double buffer */
				destroy_rastport(amesa->back_rp);
			}
			if((amesa->back_rp = make_rastport(amesa->RealWidth,amesa->RealHeight,amesa->depth,amesa->rp->BitMap))==NULL) {
				amesa->rp = amesa->front_rp;
				LibPrintf("To little mem free. Couldn't allocate Dubblebuffer in this size.\n");
			} else {
				amesa->rp=amesa->back_rp;
			}
		}

		/* JAM: added alloc_temp_rastport */
		alloc_temp_rastport(amesa);

	
		AllocOneLine(amesa);
		}
	}

void
Amiga_Standard_Dispose(struct amigamesa_context *c)
	{
DEBUGOUT("Amiga_Standard_Dispose\n")
	FreeOneLine(c);
	if (c->depth<=8) {
		if (c->visual->rgb_flag)
		{
			DisposeTransTable(c);
		}
		else
		{
			FreeTransTable(c);
		}
		/* JAM: Added free_temp_rastport() */
		free_temp_rastport(c);
	}

	if (c->visual->rgb_flag)
		{
		if (c->rgb_buffer)
			{
			LibPrintf("free(c->rgb_buffer)\n");
			free( c->rgb_buffer );
			}
		}
	if (c->back_rp)
		{
		destroy_rastport( c->back_rp );
		c->back_rp=NULL;
		}

    if ((c->depth > 8) && (!(c->visual->rgb_flag)) && (c->visual->db_flag==GL_FALSE))
    {
	if (c->ColorTable2)
#ifndef __PPC__
		FreeVec(c->ColorTable2);
#else
		FreeVecPPC(c->ColorTable2);
#endif
    }
	}

void
Amiga_Standard_SwapBuffer(struct amigamesa_context *amesa)
{
DEBUGOUT("Amiga_Standard_SwapBuffer\n")
	if (amesa->rp == NULL)
		return;
		if (amesa->back_rp)
			{
			UBYTE minterm = 0xc0;

			ClipBlit( amesa->back_rp, FIXx(((GLcontext *)amesa->gl_ctx)->Viewport.X), FIXy(((GLcontext *)amesa->gl_ctx)->Viewport.Y)-((GLcontext *)amesa->gl_ctx)->Viewport.Height+1,    /* from */
						 amesa->front_rp, FIXx(((GLcontext *)amesa->gl_ctx)->Viewport.X), FIXy(((GLcontext *)amesa->gl_ctx)->Viewport.Y)-((GLcontext *)amesa->gl_ctx)->Viewport.Height+1,  /* to */
						 ((GLcontext *)amesa->gl_ctx)->Viewport.Width, ((GLcontext *)amesa->gl_ctx)->Viewport.Height,  /* size */
						 minterm );
	/* TODO Use these cordinates insted more efficent if you only use part of screen
			RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/
			}
}

BOOL
Amiga_Standard_init(struct amigamesa_context *c,struct TagItem *tagList)
	{

	c->FixedWidth =c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX;
	c->FixedHeight=c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY;

	c->left = GetTagData(AMA_Left,0,tagList);
	c->bottom= GetTagData(AMA_Bottom,0,tagList);

	c->front_rp =c->rp;
	c->back_rp=NULL;


	c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
	c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);
	c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
	c->right = c->RealWidth - c->width - c->left;
	c->top = c->RealHeight - c->height - c->bottom;

	c->pixel = 0;   /* current drawing pen */

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

	if (c->visual->db_flag==GL_TRUE)
		{
		if((c->back_rp = make_rastport(c->RealWidth,c->RealHeight,c->depth,c->rp->BitMap))!=NULL)
			{
			((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_BACK;
			c->rp = c->back_rp;
			}
		else
			{
			LibPrintf("make_rastport Faild\n");
			((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_FRONT;
			}
		}
	else
		{
		((GLcontext *)c->gl_ctx)->Color.DrawBuffer = GL_FRONT;
		}
	AllocOneLine(c); /* A linebuffer for WritePixelLine */


	alloc_temp_rastport(c);

	c->InitDD=(void(*)(void*))amiga_standard_DD_pointers;  /*standard drawing*/
	c->Dispose=Amiga_Standard_Dispose;
	c->SwapBuffer=Amiga_Standard_SwapBuffer;

	return(TRUE);
	}
