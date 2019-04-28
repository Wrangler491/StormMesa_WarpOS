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

/* New software driver for AGA and gfxboards (compatible to
   cybergraphics.library). Based on ADisp_SW.c. Uses some functions
   of ADisp_SW.c.

   Uses functions of ADisp_SW.c (and ADispSW.c uses functions
   of this source here). Could be merged into one source, but,
   well, it works, so why change?

   Only for window mode/direct rendering resp. full screen mode
   Could be even more faster, if the algorithms would take care
   that as many longword accesses to gfx RAM space as possible
   are longword-aligned. The assembler implementation does this
   indeed.

   17.1.1998 - start coding
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

#include "ADisp_SW.h"
#include "ADisp_SWFS.h"
#include "htnew_color.h"

#ifdef WARPUP
#pragma pack(push,2)
#include <powerpc/powerpc.h>
#pragma pack(pop)
#endif

//#define DBG

#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)
#define TC_BGRA(r,g,b,a) ((((((b<<8)|g)<<8)|r)<<8)|a)
#define TC_RGB15(r,g,b) ((b >> 3) | ((g >> 3)<<5) | ((r >> 3)<<10))
#define TC_RGB15PC(r,g,b) (((b >> 3) << 8) | (g >> 6) | ((g >> 3) << 13) | ((r >> 3)<<2))
#define TC_RGB16(r,g,b) ((b >> 3) | ((g >> 2)<<5) | ((r >> 3)<<11))
#define TC_RGB16PC(r,g,b) (((b >> 3) << 8) | (g >> 5) | ((g >> 2) << 13) | ((r >> 3)<<3))
#define TC_BGR15PC(r,g,b) (((r >> 3) << 8) | (g >> 6) | ((g >> 3) << 13) | ((b >> 3)<<2))
#define TC_BGR16PC(r,g,b) (((r >> 3) << 8) | (g >> 5) | ((g >> 2) << 13) | ((b >> 3)<<3))

void SWFSDriver_SwapBuffer_AGA(struct amigamesa_context *c);
void SWFSDriver_SwapBuffer_DR(struct amigamesa_context *c);
void SWFSDriver_SwapBuffer_FS(struct amigamesa_context *c);

#ifdef WARPUP
unsigned long getLibBase()
{
	return NULL;
}
#endif

// misc support functions
;// SetBuffer
void SetBuffer(AmigaMesaContext c)
{
    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (c->backarrayflag)
	{
		if (!c->drawbufferflag)
		{
			c->DrawBuffer = c->FrontArray;
			c->draw_rp = c->back2_rp;
		}
		else
		{
			c->DrawBuffer = c->BackArray;
			c->draw_rp = c->back_rp;
		}
		if (!c->readbufferflag)
			c->ReadBuffer = c->FrontArray;
		else
			c->ReadBuffer = c->BackArray;
	}
	else
	{
		if (c->drawbufferflag)
		{
			c->DrawBuffer = c->FrontArray;
			c->draw_rp = c->back2_rp;
		}
		else
		{
			c->DrawBuffer = c->BackArray;
			c->draw_rp = c->back_rp;
		}
		if (c->readbufferflag)
			c->ReadBuffer = c->FrontArray;
		else
			c->ReadBuffer = c->BackArray;
	}
    }
}
;;//

// misc driver functions which work independent of the colormodel
;// SWFSDriver_set_buffer
GLboolean SWFSDriver_set_buffer( GLcontext *ctx,GLenum mode )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
#ifdef DBG
    /*LibPrintf*/ printf("SWFSDriver_set_buffer\n");
#endif

    if (c->flags & FLAG_TWOBUFFERS)
    {
	switch (mode)
	{
		case GL_FRONT:
			c->drawbufferflag = 1;
			c->readbufferflag = 1;
			break;
		case GL_BACK:
			c->drawbufferflag = 0;
			c->readbufferflag = 0;
			break;
	}
	SetBuffer(c);
    }
    return(GL_TRUE);
}
;;//
;// SWFSDriver_resize
void SWFSDriver_resize( GLcontext *ctx,GLuint *width, GLuint *height)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    *width=c->width;
    *height=c->height;
    if (c->rp == NULL)
	return;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSDriver_resize: %d %d\n",c->width,c->height);
#endif

    if (c->flags & FLAG_DIRECTRENDER)
    {

    if(!((c->width  == (c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX-c->left-c->right+1))
    &&  (c->height == (c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY-c->bottom-c->top+1))))
	{
	    c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
	    c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;
	    *width=c->width = c->RealWidth-c->left-c->right;
	    *height=c->height = c->RealHeight-c->bottom-c->top;
	    FreeBitMap(c->back_rp->BitMap);
	    if (!(c->back_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
	    {
		/*LibPrintf*/ printf("Panic! Not enough memory for resizing!\n");
		exit(0);
	    }
	    SetAPen(c->back_rp,0);
	    RectFill(c->back_rp,0,0,c->width-1,c->height-1);
	    if (c->flags & FLAG_TWOBUFFERS)
	    {
		FreeBitMap(c->back2_rp->BitMap);
		if (!(c->back2_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
		{
			/*LibPrintf*/ printf("Panic! Not enough memory for resizing!\n");
			exit(0);
		}
		SetAPen(c->back2_rp,0);
		RectFill(c->back2_rp,0,0,c->width-1,c->height-1);
	    }
	    else
		c->back2_rp->BitMap = c->back_rp->BitMap;
	    c->bprow = GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_XMOD);
	    c->BackArray = (unsigned char *)GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_DISPADR);
	    c->FrontArray = c->BackArray;
	    if (c->flags & FLAG_TWOBUFFERS)
		c->FrontArray = (unsigned char *)GetCyberMapAttr(c->back2_rp->BitMap,CYBRMATTR_DISPADR);
	    c->DrawBuffer = c->BackArray;
	    c->ReadBuffer = c->BackArray;
	    c->draw_rp = c->back_rp;
	    SetBuffer(c);
	}

    }

}
;;//

// clear index color
;// SWFSD_clear_index_LUT8
void SWFSD_clear_index_LUT8(GLcontext *ctx, GLuint index)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    if (c->specialalloc)
	ReleasePen(c->Screen->ViewPort.ColorMap, c->clearpixel);
    c->specialalloc = 0;
    c->clearpixel = c->penconv[index];

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_index_LUT8 %ld %ld\n",c->clearpixel,index);
#endif

}
;;//
;// SWFSD_clear_index_RGB
void SWFSD_clear_index_RGB(GLcontext *ctx, GLuint index)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_index_RGB\n");
#endif
    c->clearpixel = c->ColorTable2[index];
}
;;//

// set index color
;// SWFSD_set_index_LUT8
void SWFSD_set_index_LUT8(GLcontext *ctx, GLuint index)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    c->pixel = c->penconv[index];

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_index_LUT8 %ld\n",c->pixel);
#endif

}
;;//
;// SWFSD_set_index_RGB
void SWFSD_set_index_RGB(GLcontext *ctx, GLuint index)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_index_RGB\n");
#endif

    c->pixel = c->ColorTable2[index];
}
;;//

// clear RGBA color
;// SWFSD_clear_color_LUT8
void SWFSD_clear_color_LUT8(GLcontext *ctx,
			    GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
/*    c->clearpixel = EvalPen(c,r,g,b); */
#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_LUT8: %ld\n",c->clearpixel);
#endif
    if (c->specialalloc)
	ReleasePen(c->Screen->ViewPort.ColorMap, c->clearpixel);
    c->clearpixel = (unsigned long)ObtainBestPen(c->Screen->ViewPort.ColorMap,
				      r << 24, g << 24, b << 24,
				      OBP_Precision, PRECISION_EXACT,
				      TAG_DONE);
    c->specialalloc = 1;
}
;;//
;// SWFSD_clear_color_RGB15
void SWFSD_clear_color_RGB15(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGB15\n");
#endif

    c->clearpixel = TC_RGB15(r,g,b);
}
;;//
;// SWFSD_clear_color_RGB15PC
void SWFSD_clear_color_RGB15PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGB15PC\n");
#endif

    c->clearpixel = TC_RGB15PC(r,g,b);
}
;;//
;// SWFSD_clear_color_RGB16
void SWFSD_clear_color_RGB16(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGB16\n");
#endif

    c->clearpixel = TC_RGB16(r,g,b);
}
;;//
;// SWFSD_clear_color_RGB16PC
void SWFSD_clear_color_RGB16PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGB16PC\n");
#endif

    c->clearpixel = TC_RGB16PC(r,g,b);
}
;;//
;// SWFSD_clear_color_BGR15PC
void SWFSD_clear_color_BGR15PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_BGR15PC\n");
#endif

    c->clearpixel = TC_BGR15PC(r,g,b);
}
;;//
;// SWFSD_clear_color_BGR16PC
void SWFSD_clear_color_BGR16PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_BGR16PC\n");
#endif

    c->clearpixel = TC_BGR16PC(r,g,b);
}
;;//
;// SWFSD_clear_color_RGB
void SWFSD_clear_color_RGB(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGB\n");
#endif

    c->clearpixel = b | (g << 8) | (r << 16);
}
;;//
;// SWFSD_clear_color_BGR
void SWFSD_clear_color_BGR(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_BGR\n");
#endif

    c->clearpixel = r | (g << 8) | (b << 16);
}
;;//
;// SWFSD_clear_color_ARGB
void SWFSD_clear_color_ARGB(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_ARGB\n");
#endif

    c->clearpixel = b | (g << 8) | (r << 16) + (a << 24);
}
;;//
;// SWFSD_clear_color_BGRA
void SWFSD_clear_color_BGRA(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_BGRA\n");
#endif

    c->clearpixel = a | (r << 8) | (g << 16) + (b << 24);
}
;;//
;// SWFSD_clear_color_RGBA
void SWFSD_clear_color_RGBA(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_color_RGBA\n");
#endif

    c->clearpixel = a | (b << 8) | (g << 16) + (r << 24);
}
;;//

// set RGBA color
;// SWFSD_set_color_LUT8
void SWFSD_set_color_LUT8(GLcontext *ctx,
			    GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_LUT8: %d %d %d %d\n",r,g,b,a);
#endif
    c->pixel = EvalPen(c,r,g,b);
    c->pixelargb[0] = r;
    c->pixelargb[1] = g;
    c->pixelargb[2] = b;
    c->pixelargb[3] = a;
#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_LUT8: %ld\n",c->pixel);
#endif
}
;;//
;// SWFSD_set_color_RGB15
void SWFSD_set_color_RGB15(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGB15\n");
#endif

    c->pixel = TC_RGB15(r,g,b);
}
;;//
;// SWFSD_set_color_RGB15PC
void SWFSD_set_color_RGB15PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGB15PC\n");
#endif

    c->pixel = TC_RGB15PC(r,g,b);
}
;;//
;// SWFSD_set_color_RGB16
void SWFSD_set_color_RGB16(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGB16\n");
#endif

    c->pixel = TC_RGB16(r,g,b);
}
;;//
;// SWFSD_set_color_RGB16PC
void SWFSD_set_color_RGB16PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGB16PC\n");
#endif

    c->pixel = TC_RGB16PC(r,g,b);
}
;;//
;// SWFSD_set_color_BGR15PC
void SWFSD_set_color_BGR15PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_BGR15PC\n");
#endif

    c->pixel = TC_BGR15PC(r,g,b);
}
;;//
;// SWFSD_set_color_BGR16PC
void SWFSD_set_color_BGR16PC(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_BGR16PC\n");
#endif

    c->pixel = TC_BGR16PC(r,g,b);
}
;;//
;// SWFSD_set_color_RGB
void SWFSD_set_color_RGB(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGB\n");
#endif

    c->pixel = b | (g << 8) | (r << 16);
}
;;//
;// SWFSD_set_color_BGR
void SWFSD_set_color_BGR(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_BGR\n");
#endif

    c->pixel = r | (g << 8) | (b << 16);
}
;;//
;// SWFSD_set_color_ARGB
void SWFSD_set_color_ARGB(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_ARGB\n");
#endif

    c->pixel = b | (g << 8) | (r << 16) + (a << 24);
}
;;//
;// SWFSD_set_color_BGRA
void SWFSD_set_color_BGRA(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_BGRA\n");
#endif

    c->pixel = a | (r << 8) | (g << 16) + (b << 24);
}
;;//
;// SWFSD_set_color_RGBA
void SWFSD_set_color_RGBA(GLcontext *ctx,
				  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_set_color_RGBA\n");
#endif

    c->pixel = a | (b << 8) | (g << 16) + (r << 24);
}
;;//

// clear buffer
;// SWFSD_clear_LUT8
GLbitfield SWFSD_clear_LUT8(GLcontext *ctx, GLbitfield mask,
		      GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_LUT8 %ld\n",c->clearpixel);
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {

/* first check if AGA mode (clear backbuffer) */

    if (c->SwapBuffer == SWFSDriver_SwapBuffer_AGA)
    {
	if (all)
	{
	    long* buffer = (long *)c->DrawBuffer;
	    int x,max,rest;
	    unsigned long col = color | (color << 8) | (color << 16) | (color << 24);

	    rest = (c->height * c->width) & 0x03;
	    max = (c->height * c->width) >> 2;
	    for (x=0;x<max;x++)
		*buffer++ = col;
	    for (x=0;x<rest;x++) {
			*((char *)buffer) = color;
			(char *)buffer++;
		}
	}
	else
	{
	    long* buffer = (long *)(c->DrawBuffer +
		(c->height-y-height) * c->width + x);
	    int y,ymax,x,xmax,xrest;
	    unsigned long col = color | (color << 8) | (color << 16) | (color << 24);
	    int mod = c->width - width;

	    ymax = height;
	    xrest = width & 0x03;
	    xmax = width >> 2;
	    for (y=0;y<ymax;y++)
	    {
		for (x=0;x<xmax;x++)
		    *buffer++ = col;
		for (x=0;x<xrest;x++) {
		    *((char *)buffer) = color;
			(char *)buffer++;
		}
		buffer = (char *)buffer + mod;
	    }
	}
    }

/*  otherwise clear back bitmap using RectFill */

    else
    {
	if (all)
	{
	    SetAPen(c->draw_rp,c->clearpixel);
	    RectFill(c->draw_rp,0,0,c->width-1,c->height-1);
	}
	else
	{
	    SetAPen(c->draw_rp,c->clearpixel);
	    RectFill(c->draw_rp,x,c->height-y-height,width-1,c->height-y-1);
	}
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGB15
GLbitfield SWFSD_clear_RGB15(GLcontext *ctx, GLbitfield mask,
			GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;
#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGB15\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }


  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0x7c00) << 9) |
			((c->clearpixel & 0x03e0) << 6) |
			((c->clearpixel & 0x001f) << 3) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0x7c00) << 9) |
			((c->clearpixel & 0x03e0) << 6) |
			((c->clearpixel & 0x001f) << 3) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGB15PC
GLbitfield SWFSD_clear_RGB15PC(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGB15PC %08lx\n",c->clearpixel);
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0x007c) << 17) |
			((c->clearpixel & 0x0003) << 14) |
			((c->clearpixel & 0xe000) >> 2) |
			((c->clearpixel & 0x1f00) >> 5) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0x007c) << 17) |
			((c->clearpixel & 0x0003) << 14) |
			((c->clearpixel & 0xe000) >> 2) |
			((c->clearpixel & 0x1f00) >> 5) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGB16
GLbitfield SWFSD_clear_RGB16(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGB16\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0xf800) << 8) |
			((c->clearpixel & 0x07e0) << 5) |
			((c->clearpixel & 0x001f) << 3) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0xf800) << 8) |
			((c->clearpixel & 0x07e0) << 5) |
			((c->clearpixel & 0x001f) << 3) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGB16PC
GLbitfield SWFSD_clear_RGB16PC(GLcontext *ctx, GLbitfield mask,
			GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGB16PC %08lx\n",c->clearpixel);
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0x00f8) << 16) |
			((c->clearpixel & 0x0007) << 13) |
			((c->clearpixel & 0xe000) >> 3) |
			((c->clearpixel & 0x1f00) >> 5) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0x00f8) << 16) |
			((c->clearpixel & 0x0007) << 13) |
			((c->clearpixel & 0xe000) >> 3) |
			((c->clearpixel & 0x1f00) >> 5) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_BGR15PC
GLbitfield SWFSD_clear_BGR15PC(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_BGR15PC %08lx\n",c->clearpixel);
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0x007c) << 1) |
			((c->clearpixel & 0x0003) << 14) |
			((c->clearpixel & 0xe000) >> 2) |
			((c->clearpixel & 0x1f00) << 11) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0x007c) << 1) |
			((c->clearpixel & 0x0003) << 14) |
			((c->clearpixel & 0xe000) >> 2) |
			((c->clearpixel & 0x1f00) << 11) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_BGR16PC
GLbitfield SWFSD_clear_BGR16PC(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_BGR16PC %08lx\n",c->clearpixel);
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel & 0x00f8)) |
			((c->clearpixel & 0x0007) << 13) |
			((c->clearpixel & 0xe000) >> 3) |
			((c->clearpixel & 0x1f00) << 11) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel & 0x00f8)) |
			((c->clearpixel & 0x0007) << 13) |
			((c->clearpixel & 0xe000) >> 3) |
			((c->clearpixel & 0x1f00) << 11) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGB
GLbitfield SWFSD_clear_RGB(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGB\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,c->clearpixel);
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		       c->clearpixel);
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_BGR
GLbitfield SWFSD_clear_BGR(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_BGR\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel >> 16) & 0xff) |
			(c->clearpixel & 0xff00) |
			((c->clearpixel << 16) & 0xff0000)));
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel >> 16) & 0xff) |
			(c->clearpixel & 0xff00) |
			((c->clearpixel << 16) & 0xff0000)));
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_ARGB
GLbitfield SWFSD_clear_ARGB(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_ARGB\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,c->clearpixel);
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		       c->clearpixel);
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_BGRA
GLbitfield SWFSD_clear_BGRA(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_BGRA\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel >> 24) & 0xff) |
			((c->clearpixel >> 8) & 0xff00) |
			((c->clearpixel << 8) & 0xff0000) |
			(c->clearpixel << 24) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel >> 24) & 0xff) |
			((c->clearpixel >> 8) & 0xff00) |
			((c->clearpixel << 8) & 0xff0000) |
			(c->clearpixel << 24) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//
;// SWFSD_clear_RGBA
GLbitfield SWFSD_clear_RGBA(GLcontext *ctx, GLbitfield mask,
		GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;
    GLboolean masking = GL_FALSE;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_clear_RGBA\n");
#endif

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
	(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
       ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
	masking = GL_TRUE;
   }

  if ((mask & GL_COLOR_BUFFER_BIT) && (!masking) )
  {
    if (all)
    {
	FillPixelArray(c->draw_rp,0,0,c->width,c->height,
		      ( ((c->clearpixel >> 8) & 0xff0000) |
			((c->clearpixel >> 8) & 0xff00) |
			((c->clearpixel >> 8) & 0xff) |
			(c->clearpixel << 24) ) );
    }
    else
    {
	FillPixelArray(c->draw_rp,x,c->height-y-height,width,height,
		      ( ((c->clearpixel >> 8) & 0xff0000) |
			((c->clearpixel >> 8) & 0xff00) |
			((c->clearpixel >> 8) & 0xff) |
			(c->clearpixel << 24) ) );
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
;;//

// write color span
;// SWFSD_write_color_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_LUT8\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		*buffer = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    }
	    x++;
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	char tempbuffer[4];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+1,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+2,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+3,y);
	    rgba++;
	    x+=4;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    rgba++;
	    x++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span_RGB15
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x *2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGB15\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span_RGB15PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGB15PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x *2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGB16\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span_RGB16PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGB16PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span_BGR15PC
void SWFSD_write_color_span_BGR15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_BGR15PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span_BGR16PC
void SWFSD_write_color_span_BGR16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_BGR16PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span_RGB
void SWFSD_write_color_span_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		buffer[0] = (*rgba)[0];
		buffer[1] = (*rgba)[1];
		buffer[2] = (*rgba)[2];
	    }
	    buffer+=3;
	    rgba++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = (*rgba)[0];
	    *buffer++ = (*rgba)[1];
	    *buffer++ = (*rgba)[2];
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span_BGR
void SWFSD_write_color_span_BGR(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_BGR\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		buffer[0] = (*rgba)[2];
		buffer[1] = (*rgba)[1];
		buffer[2] = (*rgba)[0];
	    }
	    buffer+=3;
	    rgba++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = (*rgba)[2];
	    *buffer++ = (*rgba)[1];
	    *buffer++ = (*rgba)[0];
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_ARGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGBA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	    buffer++;
	    rgba++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = TC_RGBA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	    rgba++;
	}
    }

}
#endif
;;//
;// SWFSD_write_color_span_BGRA
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_BGRA\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGRA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = TC_BGRA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	    rgba++;
	}
    }

}
#endif
;;//
;// SWFSD_write_color_span_RGBA
void SWFSD_write_color_span_RGBA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span_RGBA\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = *(long*)rgba;
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = *(long*)rgba++;
	}
    }

}
;;//
;// SWFSD_write_color_span3_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_LUT8\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    x++;
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	char tempbuffer[4];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+1,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+2,y);
	    rgba++;
	    *ptr++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x+3,y);
	    rgba++;
	    x+=4;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],x,y);
	    rgba++;
	    x++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span3_RGB15
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x *2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGB15\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span3_RGB15PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGB15PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span3_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x *2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGB16\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span3_RGB16PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGB16PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer = TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
#endif
;;//
;// SWFSD_write_color_span3_BGR15PC
void SWFSD_write_color_span3_BGR15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_BGR15PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span3_BGR16PC
void SWFSD_write_color_span3_BGR16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_BGR16PC\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    *ptr++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span3_RGB
void SWFSD_write_color_span3_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		buffer[0] = (*rgba)[0];
		buffer[1] = (*rgba)[1];
		buffer[2] = (*rgba)[2];
	    }
	    buffer+=3;
	    rgba++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    *ptr++ = (*rgba)[0];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[2];
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = (*rgba)[0];
	    *buffer++ = (*rgba)[1];
	    *buffer++ = (*rgba)[2];
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span3_BGR
void SWFSD_write_color_span3_BGR(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_BGR\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		buffer[0] = (*rgba)[2];
		buffer[1] = (*rgba)[1];
		buffer[2] = (*rgba)[0];
	    }
	    buffer+=3;
	    rgba++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    *ptr++ = (*rgba)[2];
	    *ptr++ = (*rgba)[1];
	    *ptr++ = (*rgba)[0];
	    rgba++;
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = (*rgba)[2];
	    *buffer++ = (*rgba)[1];
	    *buffer++ = (*rgba)[0];
	    rgba++;
	}
    }
}
;;//
;// SWFSD_write_color_span3_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_ARGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_RGBA((*rgba)[0],(*rgba)[1],(*rgba)[2],0);
	    buffer++;
	    rgba++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = TC_RGBA((*rgba)[0],(*rgba)[1],(*rgba)[2],0);
	    rgba++;
	}
    }

}
#endif
;;//
;// SWFSD_write_color_span3_BGRA
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_span3_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_BGRA\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = TC_BGRA((*rgba)[0],(*rgba)[1],(*rgba)[2],0);
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = TC_BGRA((*rgba)[0],(*rgba)[1],(*rgba)[2],0);
	    rgba++;
	}
    }

}
#endif
;;//
;// SWFSD_write_color_span3_RGBA
void SWFSD_write_color_span3_RGBA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte rgba[][3], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_span3_RGBA\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = *(long*)rgba;
	    rgba++;
	    buffer++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = *(long*)rgba++;
	}
    }

}
;;//

// write mono color/index span
;// SWFSD_write_monorgba_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_monorgba_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned char buf[8];
    unsigned char* bptr = buf;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_monorgba_span_LUT8\n");
#endif

    GetPensDithered((int *)c->ColorTable,bptr,c->pixelargb[0],c->pixelargb[1],c->pixelargb[2],x,y);
    if (mask)
    {
	unsigned char* buffer = (char *)(c->DrawBuffer +
		(c->height-y-1) * c->bprow + x);
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = bptr[i & 0x7];
	    buffer++;
	}
    }
    else
    {
	long* buffer = (long *)(c->DrawBuffer +
		(c->height-y-1) * c->bprow + x);
	int xmax,xrest;

	xmax = n >> 3;
	xrest = n & 0x7;
	for (i=0;i<xmax;i++)
	{
	    *buffer++ = ((long *)bptr)[0];
	    *buffer++ = ((long *)bptr)[1];
	}
	for (i=0;i<xrest;i++)
	{
	    *((char *)buffer) = bptr[i & 0x7];
		(char *)buffer++;
	}
    }
}
#endif
;;//
;// SWFSD_write_monoci_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_monoci_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char color = (char)c->pixel;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_monoci_span_LUT8 %ld\n",c->pixel);
#endif

    if (mask)
    {
	char* buffer = (char *)(c->DrawBuffer +
		(c->height-y-1) * c->bprow + x);
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = color;
	    buffer++;
	}
    }
    else
    {
	long* buffer = (long *)(c->DrawBuffer +
		(c->height-y-1) * c->bprow + x);
	int xmax,xrest;
	long col = color | (color << 8) | (color << 16) | (color << 24);

	xmax = n >> 2;
	xrest = n & 0x3;
	for (i=0;i<xmax;i++)
	{
	    *buffer++ = col;
	}
	for (i=0;i<xrest;i++)
	{
	    *((char *)buffer) = color;
		(char *)buffer++;
	}
    }
}
#endif
;;//
;// SWFSD_write_mono_span_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_mono_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    short color = (short)c->pixel;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_span_RGB16 %ld\n",c->pixel);
#endif

    if (mask)
    {
	short* buffer = (short *)(c->DrawBuffer +
		(c->height-y-1) * c->bprow + x * 2);

	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = color;
	    buffer++;
	}
    }
    else
    {
	long* buffer = (long *)(c->BackArray +
		(c->height-y-1) * c->bprow + x * 2);
	int xmax,xrest;
	long col = color | (color << 8);

	xmax = n >> 1;
	xrest = n & 0x1;
	for (i=0;i<xmax;i++)
	{
	    *buffer++ = col;
	}
	for (i=0;i<xrest;i++)
	{
	    *((short *)buffer) = color;
		(short *)buffer++;
	}
    }
}
#endif
;;//
;// SWFSD_write_mono_span_RGB
void SWFSD_write_mono_span_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    unsigned long color = c->pixel;
    char* colptr = (char *)&color;
    int i;
    colptr++;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_span_RGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
	    {
		buffer[0] = colptr[0];
		buffer[1] = colptr[1];
		buffer[2] = colptr[2];
	    }
	    buffer+=3;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (i=0;i<n1;i++)
	{
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (i=0;i<n2;i++)
	{
	    *buffer++ = colptr[0];
	    *buffer++ = colptr[1];
	    *buffer++ = colptr[2];
	}
    }
}
;;//
;// SWFSD_write_mono_span_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_mono_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    unsigned long color = c->pixel;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_span_ARGB\n");
#endif

    if (mask)
    {
	for (i=0;i<n;i++)
	{
	    if (*mask++)
		*buffer = color;
	    buffer++;
	}
    }
    else
    {
	for (i=0;i<n;i++)
	{
	    *buffer++ = color;
	}
    }
}
#endif
;;//

// write color pixels
;// SWFSD_write_color_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_LUT8\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		EvalPen_D(c,(*rgba)[0],(*rgba)[1],(*rgba)[2],*x,*y);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_RGB15
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_RGB15(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGB15\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_RGB15((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_RGB15PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_RGB15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGB15PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_RGB15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGB16\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_RGB16((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_RGB16PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_RGB16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGB16PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_RGB16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_BGR15PC
void SWFSD_write_color_pixels_BGR15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_BGR15PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_BGR15PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_write_color_pixels_BGR16PC
void SWFSD_write_color_pixels_BGR16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_BGR16PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_BGR16PC((*rgba)[0],(*rgba)[1],(*rgba)[2]);
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_write_color_pixels_RGB
void SWFSD_write_color_pixels_RGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    int index = (h-(*y)) * w + 3*(*x);
	    buffer[index++] = (*rgba)[0];
	    buffer[index++] = (*rgba)[1];
	    buffer[index++] = (*rgba)[2];
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_write_color_pixels_BGR
void SWFSD_write_color_pixels_BGR(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_BGR\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    int index = (h-(*y)) * w + 3*(*x);
	    buffer[index++] = (*rgba)[2];
	    buffer[index++] = (*rgba)[1];
	    buffer[index++] = (*rgba)[0];
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_write_color_pixels_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_ARGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_RGBA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_BGRA
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_color_pixels_BGRA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_BGRA\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] =
		TC_BGRA((*rgba)[0],(*rgba)[1],(*rgba)[2],(*rgba)[3]);
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_write_color_pixels_RGBA
void SWFSD_write_color_pixels_RGBA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte rgba[][4], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_color_pixels_RGBA\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = *(long*)rgba;
	x++;
	y++;
	rgba++;
    }
}
;;//

// write mono color/index pixels
;// SWFSD_write_monorgba_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_monorgba_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    unsigned char buf[8];
    unsigned char* bptr = buf;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_monorgba_pixels_LUT8\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = EvalPen_D(c,c->pixelargb[0],c->pixelargb[1],c->pixelargb[2],*x,*y);
	x++;
	y++;
    }
}
#endif
;;//
;// SWFSD_write_monoci_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_monoci_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char color = (char)c->pixel;
    char* buffer = (char *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_monoci_pixels_LUT8: %ld\n",(unsigned long)color);
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = color;
	x++;
	y++;
    }
}
#endif
;;//
;// SWFSD_write_mono_pixels_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_mono_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short color = (short)c->pixel;
    short* buffer = (short *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_pixels_RGB16\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = color;
	x++;
	y++;
    }
}
#endif
;;//
;// SWFSD_write_mono_pixels_RGB
void SWFSD_write_mono_pixels_RGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->pixel;
    char* buffer = (char *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow;
    char* colptr = (char *)&color;
    int i;
    colptr++;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_pixels_RGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    int index = (h-(*y)) * w + 3*(*x);
	    buffer[index++] = colptr[0];
	    buffer[index++] = colptr[1];
	    buffer[index++] = colptr[2];
	}
	x++;
	y++;
    }
}
;;//
;// SWFSD_write_mono_pixels_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_mono_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->pixel;
    long* buffer = (long *)c->DrawBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_mono_pixels_ARGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = color;
	x++;
	y++;
    }
}
#endif
;;//

// write index span
;// SWFSD_write_index_span32_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span32_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLuint index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x);
    int z;
    unsigned char* pc = c->penconv;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span32_LUT8\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = pc[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	char tempbuffer[4];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (z=0;z<n1;z++)
	{
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (z=0;z<n2;z++)
	{
	    *buffer++ = pc[*index++];
	}
    }
}
#endif
;;//
;// SWFSD_write_index_span32_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span32_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLuint index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int z;
    long* coltab = c->ColorTable2;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span32_RGB16\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = (short)coltab[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (z=0;z<n1;z++)
	{
	    *ptr++ = (short)coltab[*index++];
	    *ptr++ = (short)coltab[*index++];
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (z=0;z<n2;z++)
	{
	    *buffer++ = (short)coltab[*index++];
	}
    }
}
#endif
;;//
;// SWFSD_write_index_span32_RGB
void SWFSD_write_index_span32_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLuint index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int z;
    long* coltab = c->ColorTable2;
    long col;
    char *colptr = (char *)&col;
    colptr++;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span32_RGB\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
	    {
		col = (long)coltab[*index];
		buffer[0] = colptr[0];
		buffer[1] = colptr[1];
		buffer[2] = colptr[2];
	    }
	    buffer+=3;
	    index++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (z=0;z<n1;z++)
	{
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (z=0;z<n2;z++)
	{
	    col = (long)coltab[*index++];
	    *buffer++ = colptr[0];
	    *buffer++ = colptr[1];
	    *buffer++ = colptr[2];
	}
    }
}
;;//
;// SWFSD_write_index_span32_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span32_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLuint index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int z;
    long* coltab = c->ColorTable2;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span32_ARGB\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = coltab[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	for (z=0;z<n;z++)
	{
	    *buffer++ = coltab[*index++];
	}
    }
}
#endif
;;//
;// SWFSD_write_index_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x);
    int z;
    unsigned char* pc = c->penconv;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span_LUT8\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = pc[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	char tempbuffer[4];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (z=0;z<n1;z++)
	{
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    *ptr++ = pc[*index++];
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (z=0;z<n2;z++)
	{
	    *buffer++ = pc[*index++];
	}
    }
}
#endif
;;//
;// SWFSD_write_index_span_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int z;
    long* coltab = c->ColorTable2;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span_RGB16\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = (short)coltab[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	short tempbuffer[2];
	short *ptr = tempbuffer;
	int n1 = (n >> 1);
	int n2 = (n & 0x01);

	for (z=0;z<n1;z++)
	{
	    *ptr++ = (short)coltab[*index++];
	    *ptr++ = (short)coltab[*index++];
	    ptr = tempbuffer;
	    *((long *)buffer) = *(long *)ptr;
		(long *)buffer++;
	}
	for (z=0;z<n2;z++)
	{
	    *buffer++ = (short)coltab[*index++];
	}
    }
}
#endif
;;//
;// SWFSD_write_index_span_RGB
void SWFSD_write_index_span_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int z;
    long* coltab = c->ColorTable2;
    long col;
    char *colptr = (char *)&col;
    colptr++;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span_RGB\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
	    {
		col = (long)coltab[*index];
		buffer[0] = colptr[0];
		buffer[1] = colptr[1];
		buffer[2] = colptr[2];
	    }
	    buffer+=3;
	    index++;
	}
    }
    else
    {
	char tempbuffer[12];
	char *ptr = tempbuffer;
	int n1 = (n >> 2);
	int n2 = (n & 0x03);

	for (z=0;z<n1;z++)
	{
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    col = (long)coltab[*index++];
	    *ptr++ = colptr[0];
	    *ptr++ = colptr[1];
	    *ptr++ = colptr[2];
	    ptr = tempbuffer;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    *((long *)buffer) = *((long *)ptr);
		(long *)buffer++;
		(long *)ptr++;
	    ptr = tempbuffer;
	}
	for (z=0;z<n2;z++)
	{
	    col = (long)coltab[*index++];
	    *buffer++ = colptr[0];
	    *buffer++ = colptr[1];
	    *buffer++ = colptr[2];
	}
    }
}
;;//
;// SWFSD_write_index_span_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					const GLubyte index[],
					const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->DrawBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int z;
    long* coltab = c->ColorTable2;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_span_ARGB\n");
#endif

    if (mask)
    {
	for (z=0;z<n;z++)
	{
	    if (*mask++)
		*buffer = coltab[*index];
	    buffer++;
	    index++;
	}
    }
    else
    {
	for (z=0;z<n;z++)
	{
	    *buffer++ = coltab[*index++];
	}
    }
}
#endif
;;//

// write index pixels
;// SWFSD_write_index_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLuint index[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    unsigned char* pc = c->penconv;
    int h = c->height-1;
    int w = c->bprow;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_pixels_LUT8\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = pc[*index];
	x++;
	y++;
	index++;
    }
}
#endif
;;//
;// SWFSD_write_index_pixels_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLuint index[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->DrawBuffer;
    long* coltab = c->ColorTable2;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_pixels_RGB16\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = (short)coltab[*index];
	x++;
	y++;
	index++;
    }
}
#endif
;;//
;// SWFSD_write_index_pixels_RGB
void SWFSD_write_index_pixels_RGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLuint index[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->DrawBuffer;
    long* coltab = c->ColorTable2;
    int h = c->height-1;
    int w = c->bprow;
    int z;
    long col;
    char *colptr = (char *)&col;
    colptr++;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_pixels_RGB16\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    int index2 = (h-(*y)) * w + 3*(*x);
	    col = (long)coltab[*index];
	    buffer[index2++] = colptr[0];
	    buffer[index2++] = colptr[1];
	    buffer[index2++] = colptr[2];
	}
	x++;
	y++;
	index++;
    }
}
;;//
;// SWFSD_write_index_pixels_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_write_index_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					      const GLuint index[],
					      const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->DrawBuffer;
    long* coltab = c->ColorTable2;
    int h = c->height-1;
    int w = c->bprow>>2;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_write_index_pixels_ARGB\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	    buffer[(h-(*y)) * w + (*x)] = coltab[*index];
	x++;
	y++;
	index++;
    }
}
#endif
;;//

// read color span
;// SWFSD_read_color_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned char* buffer = (unsigned char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x);
    long* coltab = c->ColorTable2;
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_LUT8\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = coltab[(temp>>24) & 0xff];
	*(unsigned long*)rgba++ = (color << 8) | 0xff;
	color = coltab[(temp>>16) & 0xff];
	*(unsigned long*)rgba++ = (color << 8) | 0xff;
	color = coltab[(temp>>8) & 0xff];
	*(unsigned long*)rgba++ = (color << 8) | 0xff;
	color = coltab[temp & 0xff];
	*(unsigned long*)rgba++ = (color << 8) | 0xff;
    }
    for (i=0;i<n2;i++)
    {
	color = coltab[*buffer++];
	*(unsigned long*)rgba++ = (color << 8) | 0xff;
    }
}
#endif
;;//
;// SWFSD_read_color_span_RGB15
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGB15\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[0] = (GLubyte)((color >> 7) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 2) & 0xf8);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[0] = (GLubyte)((color >> 7) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 2) & 0xf8);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[0] = (GLubyte)((color >> 7) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 2) & 0xf8);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_RGB15PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGB15PC\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[0] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[0] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[0] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGB16\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[0] = (GLubyte)((color >> 8) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 3) & 0xfc);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[0] = (GLubyte)((color >> 8) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 3) & 0xfc);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[0] = (GLubyte)((color >> 8) & 0xf8);
	(*rgba)[1] = (GLubyte)((color >> 3) & 0xfc);
	(*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_RGB16PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGB16PC\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[0] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[0] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[0] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_BGR15PC
void SWFSD_read_color_span_BGR15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_BGR15PC\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[2] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[2] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[2] = (GLubyte)((color << 1) & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_span_BGR16PC
void SWFSD_read_color_span_BGR16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short *buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    int i;
    unsigned long color;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_BGR16PC\n");
#endif

    for (i=0;i<n1;i++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	(*rgba)[2] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
	color = temp & 0xffff;
	(*rgba)[2] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
    for (i=0;i<n2;i++)
    {
	color = *buffer++;
	(*rgba)[2] = (GLubyte)(color & 0xf8);
	(*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	(*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_span_RGB
void SWFSD_read_color_span_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGB\n");
#endif

    char tempbuffer[12];
    char *ptr = tempbuffer;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

    for (i=0;i<n1;i++)
    {
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	ptr = tempbuffer;
	(*rgba)[0] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[2] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[0] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[2] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[0] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[2] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[0] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[2] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	ptr = tempbuffer;
    }
    for (i=0;i<n2;i++)
    {
	(*rgba)[0] = *buffer++;
	(*rgba)[1] = *buffer++;
	(*rgba)[2] = *buffer++;
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_span_BGR
void SWFSD_read_color_span_BGR(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_BGR\n");
#endif

    char tempbuffer[12];
    char *ptr = tempbuffer;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

    for (i=0;i<n1;i++)
    {
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	ptr = tempbuffer;
	(*rgba)[2] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[0] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[2] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[0] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[2] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[0] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	(*rgba)[2] = *ptr++;
	(*rgba)[1] = *ptr++;
	(*rgba)[0] = *ptr++;
	(*rgba)[3] = 0xff;
	rgba++;
	ptr = tempbuffer;
    }
    for (i=0;i<n2;i++)
    {
	(*rgba)[2] = *buffer++;
	(*rgba)[1] = *buffer++;
	(*rgba)[0] = *buffer++;
	(*rgba)[3] = 0xff;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_span_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_ARGB\n");
#endif

    for (i=0;i<n;i++)
    {
	color = *buffer++;
	(*rgba)[3] = (GLubyte)(color >> 24);
	(*rgba)[0] = (GLubyte)((color >> 16) & 0xff);
	(*rgba)[1] = (GLubyte)((color >> 8) & 0xff);
	(*rgba)[2] = (GLubyte)(color & 0xff);
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_BGRA
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_BGRA\n");
#endif

    for (i=0;i<n;i++)
    {
	color = *buffer++;
	(*rgba)[2] = (GLubyte)(color >> 24);
	(*rgba)[1] = (GLubyte)((color >> 16) & 0xff);
	(*rgba)[0] = (GLubyte)((color >> 8) & 0xff);
	(*rgba)[3] = (GLubyte)(color & 0xff);
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_span_RGBA
void SWFSD_read_color_span_RGBA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLubyte rgba[][4])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_span_RGBA\n");
#endif

    for (i=0;i<n;i++)
    {
	*(long *)rgba++ = *buffer++;
    }
}
;;//

// read index span
;// SWFSD_read_index_span_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_index_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned char* buffer = (unsigned char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x);
    unsigned char* pc = c->penconvinv;
    int z;
    unsigned long temp;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_LUT8\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	*index++ = pc[(temp >> 24) & 0xff];
	*index++ = pc[(temp >> 16) & 0xff];
	*index++ = pc[(temp >> 8) & 0xff];
	*index++ = pc[temp & 0xff];
    }
    for (z=0;z<n2;z++)
    {
	*index++ = pc[*buffer++];
    }
}
#endif
;;//
;// SWFSD_read_index_span_RGB15
void SWFSD_read_index_span_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGB15\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 7) & 0xf8),
		      (GLubyte)((color >> 2) & 0xf8),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 7) & 0xf8),
		      (GLubyte)((color >> 2) & 0xf8),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 7) & 0xf8),
		      (GLubyte)((color >> 2) & 0xf8),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_RGB15PC
void SWFSD_read_index_span_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGB15PC\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color << 1) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color << 1) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color << 1) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_RGB16
void SWFSD_read_index_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGB16\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xf8),
		      (GLubyte)((color >> 3) & 0xfc),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xf8),
		      (GLubyte)((color >> 3) & 0xfc),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xf8),
		      (GLubyte)((color >> 3) & 0xfc),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_RGB16PC
void SWFSD_read_index_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGB16PC\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)(color & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)(color & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)(color & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_BGR15PC
void SWFSD_read_index_span_BGR15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_BGR15PC\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color << 1) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color << 1) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color << 1) & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_BGR16PC
void SWFSD_read_index_span_BGR16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 2);
    unsigned long color;
    int z;
    unsigned long temp;
    int n1 = (n >> 1);
    int n2 = (n & 0x01);

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_BGR16PC\n");
#endif

    for (z=0;z<n1;z++)
    {
	temp = *((long *)buffer);
	(long *)buffer++;
	color = temp >> 16;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)(color & 0xf8),
		      (unsigned long *)c->ColorTable2);
	color = temp & 0xffff;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)(color & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
    for (z=0;z<n2;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)(color & 0xf8),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_RGB
void SWFSD_read_index_span_RGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGB\n");
#endif

    char tempbuffer[12];
    char *ptr = tempbuffer;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

    for (z=0;z<n1;z++)
    {
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	ptr = tempbuffer;

	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[0]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[2]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[0]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[2]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[0]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[2]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[0]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[2]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	ptr = tempbuffer;
    }
    for (z=0;z<n2;z++)
    {
	*index++ = FindNearestPen(c,
		      (GLubyte)(buffer[0]),
		      (GLubyte)(buffer[1]),
		      (GLubyte)(buffer[2]),
		      (unsigned long *)c->ColorTable2);
	buffer+=3;
    }
}
;;//
;// SWFSD_read_index_span_BGR
void SWFSD_read_index_span_BGR(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 3);
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_BGR\n");
#endif

    char tempbuffer[12];
    char *ptr = tempbuffer;
    int n1 = (n >> 2);
    int n2 = (n & 0x03);

    for (z=0;z<n1;z++)
    {
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	*((long *)ptr) = *((long *)buffer);
	(long *)buffer++;
	(long *)ptr++;
	ptr = tempbuffer;

	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[2]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[0]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[2]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[0]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[2]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[0]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	*index++ = FindNearestPen(c,
		      (GLubyte)(ptr[2]),
		      (GLubyte)(ptr[1]),
		      (GLubyte)(ptr[0]),
		      (unsigned long *)c->ColorTable2);
	ptr+=3;
	ptr = tempbuffer;
    }
    for (z=0;z<n2;z++)
    {
	*index++ = FindNearestPen(c,
		      (GLubyte)(buffer[2]),
		      (GLubyte)(buffer[1]),
		      (GLubyte)(buffer[0]),
		      (unsigned long *)c->ColorTable2);
	buffer+=3;
    }
}
;;//
;// SWFSD_read_index_span_ARGB
void SWFSD_read_index_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    unsigned long color;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_ARGB\n");
#endif

    for (z=0;z<n;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)(color & 0xff),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_BGRA
void SWFSD_read_index_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    unsigned long color;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_ARGB\n");
#endif

    for (z=0;z<n;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 24) & 0xff),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//
;// SWFSD_read_index_span_RGBA
void SWFSD_read_index_span_RGBA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
					GLuint index[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)(c->ReadBuffer +
	    (c->height-y-1) * c->bprow + x * 4);
    unsigned long color;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_span_RGBA\n");
#endif

    for (z=0;z<n;z++)
    {
	color = *buffer++;
	*index++ = FindNearestPen(c,
		      (GLubyte)((color >> 24) & 0xff),
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (unsigned long *)c->ColorTable2);
    }
}
;;//

// read color pixels
;// SWFSD_read_color_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned char* buffer = (unsigned char *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow;
    long* coltab = c->ColorTable2;
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_LUT8\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = coltab[buffer[(h-(*y)) * w + (*x)]];
	    *(unsigned long*)rgba = (color << 8) | 0xff;
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_RGB15
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_RGB15(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGB15\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[0] = (GLubyte)((color >> 7) & 0xf8);
	    (*rgba)[1] = (GLubyte)((color >> 2) & 0xf8);
	    (*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_RGB15PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_RGB15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGB15PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[0] = (GLubyte)((color << 1)& 0xf8);
	    (*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	    (*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_RGB16
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGB16\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[0] = (GLubyte)((color >> 8) & 0xf8);
	    (*rgba)[1] = (GLubyte)((color >> 3) & 0xfc);
	    (*rgba)[2] = (GLubyte)((color << 3) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_RGB16PC
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_RGB16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGB16PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[0] = (GLubyte)(color & 0xf8);
	    (*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	    (*rgba)[2] = (GLubyte)((color >> 5) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_BGR15PC
void SWFSD_read_color_pixels_BGR15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_BGR15PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[2] = (GLubyte)((color << 1)& 0xf8);
	    (*rgba)[1] = (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) );
	    (*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_pixels_BGR16PC
void SWFSD_read_color_pixels_BGR16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int i;
    unsigned short color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_BGR16PC\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)0xff;
	    (*rgba)[2] = (GLubyte)(color & 0xf8);
	    (*rgba)[1] = (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) );
	    (*rgba)[0] = (GLubyte)((color >> 5) & 0xf8);
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_pixels_RGB
void SWFSD_read_color_pixels_RGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    int index = (h-(*y)) * w + 3*(*x);
	    (*rgba)[0] = buffer[index+0];
	    (*rgba)[1] = buffer[index+1];
	    (*rgba)[2] = buffer[index+2];
	    (*rgba)[3] = 0xff;
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_pixels_BGR
void SWFSD_read_color_pixels_BGR(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int i;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_BGR\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    int index = (h-(*y)) * w + 3*(*x);
	    (*rgba)[0] = buffer[index+2];
	    (*rgba)[1] = buffer[index+1];
	    (*rgba)[2] = buffer[index+0];
	    (*rgba)[3] = 0xff;
	}
	x++;
	y++;
	rgba++;
    }
}
;;//
;// SWFSD_read_color_pixels_ARGB
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_ARGB\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)(color >> 24);
	    (*rgba)[0] = (GLubyte)((color >> 16) & 0xff);
	    (*rgba)[1] = (GLubyte)((color >> 8) & 0xff);
	    (*rgba)[2] = (GLubyte)(color & 0xff);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_BGRA
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_color_pixels_BGRA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_BGRA\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    (*rgba)[3] = (GLubyte)(color & 0xff);
	    (*rgba)[0] = (GLubyte)((color >> 8) & 0xff);
	    (*rgba)[1] = (GLubyte)((color >> 16) & 0xff);
	    (*rgba)[2] = (GLubyte)((color >> 24) & 0xff);
	}
	x++;
	y++;
	rgba++;
    }
}
#endif
;;//
;// SWFSD_read_color_pixels_RGBA
void SWFSD_read_color_pixels_RGBA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
					GLubyte rgba[][4], const GLubyte mask[])
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int i;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_color_pixels_RGBA\n");
#endif

    for (i=0;i<n;i++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *(long *)rgba++ = color;
	}
	x++;
	y++;
	rgba++;
    }
}
;;//

// read index pixels
;// SWFSD_read_index_pixels_LUT8
#if defined(NOASM_68K) || defined(NOASM_PPC)
void SWFSD_read_index_pixels_LUT8(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned char* buffer = (unsigned char *)c->ReadBuffer;
    const GLubyte* m = mask;
    unsigned char* pc = c->penconvinv;
    int h = c->height-1;
    int w = c->bprow;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_LUT8\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	    *index = pc[buffer[(h-(*y)) * w + (*x)]];
	index++;
	x++;
	y++;
    }
}
#endif
;;//
;// SWFSD_read_index_pixels_RGB15
void SWFSD_read_index_pixels_RGB15(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGB15\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 7) & 0xf8),
		      (GLubyte)((color >> 2) & 0xf8),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_RGB15PC
void SWFSD_read_index_pixels_RGB15PC(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGB15PC\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color << 1)& 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_RGB16
void SWFSD_read_index_pixels_RGB16(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGB16\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xf8),
		      (GLubyte)((color >> 3) & 0xfc),
		      (GLubyte)((color << 3) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_RGB16PC
void SWFSD_read_index_pixels_RGB16PC(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGB16PC\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)(color & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)((color >> 5) & 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_BGR15PC
void SWFSD_read_index_pixels_BGR15PC(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_BGR15PC\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 6) & 0xc0) | ((color >> 10) & 0x38) ),
		      (GLubyte)((color << 1)& 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_BGR16PC
void SWFSD_read_index_pixels_BGR16PC(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    short* buffer = (short *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>1;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_BGR16PC\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 5) & 0xf8),
		      (GLubyte)( ((color << 5) & 0xe0) | ((color >> 11) & 0x1c) ),
		      (GLubyte)(color & 0xf8),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_RGB
void SWFSD_read_index_pixels_RGB(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGB\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    int index2 = (h-(*y)) * w + 3*(*x);
	    *index = FindNearestPen(c,
		      (GLubyte)(buffer[index2+0]),
		      (GLubyte)(buffer[index2+1]),
		      (GLubyte)(buffer[index2+2]),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_BGR
void SWFSD_read_index_pixels_BGR(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    char* buffer = (char *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow;
    int z;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_BGR\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    int index2 = (h-(*y)) * w + 3*(*x);
	    *index = FindNearestPen(c,
		      (GLubyte)(buffer[index2+2]),
		      (GLubyte)(buffer[index2+1]),
		      (GLubyte)(buffer[index2+0]),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_ARGB
void SWFSD_read_index_pixels_ARGB(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_ARGB\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)(color & 0xff),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_BGRA
void SWFSD_read_index_pixels_BGRA(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_BGRA\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 8) & 0xff),
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 24) & 0xff),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//
;// SWFSD_read_index_pixels_RGBA
void SWFSD_read_index_pixels_RGBA(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
					 GLuint index[], const GLubyte mask[] )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    long* buffer = (long *)c->ReadBuffer;
    int h = c->height-1;
    int w = c->bprow>>2;
    int z;
    unsigned long color;

#ifdef DBG
    /*LibPrintf*/ printf("SWFSD_read_index_pixels_RGBA\n");
#endif

    for (z=0;z<n;z++)
    {
	if (*mask++)
	{
	    color = buffer[(h-(*y)) * w + (*x)];
	    *index = FindNearestPen(c,
		      (GLubyte)((color >> 24) & 0xff),
		      (GLubyte)((color >> 16) & 0xff),
		      (GLubyte)((color >> 8) & 0xff),
		      (unsigned long *)c->ColorTable2);
	}
	index++;
	x++;
	y++;
    }
}
;;//

// renderer string
;// SWFSDriver_rendererstring
const char *SWFSDriver_rendererstring(void)
{
	return("AMIGA_DB_DR");
}
;;//

// flush
;// SWFSDriver_flush
void SWFSDriver_flush( GLcontext *ctx )
{
    struct BitMap* bm;
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (c->SwapBuffer == SWFSDriver_SwapBuffer_AGA)
	{
		ChunkyToPlanar(c->FrontArray,c->sbuf1->sb_BitMap,c->width,c->height);
	}
	else if (c->SwapBuffer == SWFSDriver_SwapBuffer_DR)
	{

		bm = c->backarrayflag ? c->back_rp->BitMap : c->back2_rp->BitMap;
		BltBitMapRastPort(bm,
				0,
				0,
				c->front_rp,
				c->left,
				c->top,
				c->width,
				c->height,
				0xc0);

	}
    }
}
;;//

// driver setup / dispose
;// Common_DD_Pointers
/*
 *
 * Driver functions setup
 *
 */


void Common_DD_pointers(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

   switch (c->fmt)
   {
      case PIXFMT_LUT8:
	ctx->Driver.ClearIndex = SWFSD_clear_index_LUT8;
	ctx->Driver.ClearColor = SWFSD_clear_color_LUT8;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_LUT8;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_LUT8;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_monorgba_span_LUT8;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_LUT8;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_monorgba_pixels_LUT8;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_LUT8;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_LUT8;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_monoci_span_LUT8;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_LUT8;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_monoci_pixels_LUT8;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_LUT8;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_LUT8;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_LUT8;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_LUT8;
	break;
      case PIXFMT_RGB15:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGB15;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGB15;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGB15;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGB15;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGB15;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGB15;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGB15;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGB15;
	break;
      case PIXFMT_RGB15PC:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGB15PC;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGB15PC;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGB15PC;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGB15PC;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGB15PC;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGB15PC;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGB15PC;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGB15PC;
	break;
      case PIXFMT_RGB16:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGB16;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGB16;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGB16;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGB16;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGB16;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGB16;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGB16;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGB16;
	break;
      case PIXFMT_RGB16PC:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGB16PC;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGB16PC;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGB16PC;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGB16PC;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGB16PC;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGB16PC;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGB16PC;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGB16PC;
	break;
      case PIXFMT_BGR15PC:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_BGR15PC;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_BGR15PC;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_BGR15PC;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_BGR15PC;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_BGR15PC;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_BGR15PC;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_BGR15PC;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_BGR15PC;
	break;
      case PIXFMT_BGR16PC:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_BGR16PC;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_BGR16PC;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_BGR16PC;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_BGR16PC;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB16;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB16;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB16;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB16;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB16;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_BGR16PC;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_BGR16PC;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_BGR16PC;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_BGR16PC;
	break;
      case PIXFMT_RGB24:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGB;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGB;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGB;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGB;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGB;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGB;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGB;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGB;
	break;
      case PIXFMT_BGR24:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_BGR;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_BGR;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_BGR;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_RGB;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_BGR;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_RGB;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_RGB;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_RGB;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_RGB;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_RGB;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_RGB;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_BGR;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_BGR;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_BGR;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_BGR;
	break;
      case PIXFMT_ARGB32:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_ARGB;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_ARGB;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_ARGB;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_ARGB;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_ARGB;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_ARGB;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_ARGB;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_ARGB;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_ARGB;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_ARGB;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_ARGB;
	break;
      case PIXFMT_BGRA32:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_BGRA;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_BGRA;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_BGRA;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_BGRA;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_ARGB;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_ARGB;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_ARGB;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_BGRA;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_BGRA;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_BGRA;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_BGRA;
	break;
      case PIXFMT_RGBA32:
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_RGBA;
	ctx->Driver.WriteRGBASpan = SWFSD_write_color_span_RGBA;
	ctx->Driver.WriteRGBSpan = SWFSD_write_color_span3_RGBA;
	ctx->Driver.WriteMonoRGBASpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteRGBAPixels = SWFSD_write_color_pixels_RGBA;
	ctx->Driver.WriteMonoRGBAPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.WriteCI32Span = SWFSD_write_index_span32_ARGB;
	ctx->Driver.WriteCI8Span = SWFSD_write_index_span_ARGB;
	ctx->Driver.WriteMonoCISpan = SWFSD_write_mono_span_ARGB;
	ctx->Driver.WriteCI32Pixels = SWFSD_write_index_pixels_ARGB;
	ctx->Driver.WriteMonoCIPixels = SWFSD_write_mono_pixels_ARGB;
	ctx->Driver.ReadCI32Span = SWFSD_read_index_span_RGBA;
	ctx->Driver.ReadRGBASpan = SWFSD_read_color_span_RGBA;
	ctx->Driver.ReadCI32Pixels = SWFSD_read_index_pixels_RGBA;
	ctx->Driver.ReadRGBAPixels = SWFSD_read_color_pixels_RGBA;
	break;
   }
}
;;//
;// SWFSDriver_DD_Pointers
/*
 *
 * Driver functions setup
 *
 */


void SWFSDriver_DD_pointers( GLcontext *ctx )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
   /*LibPrintf*/ printf("SWFSDriver_DD_pointers\n");
#endif

   ctx->Driver.RendererString = SWFSDriver_rendererstring;
   ctx->Driver.Flush = SWFSDriver_flush;
   ctx->Driver.UpdateState = SWFSDriver_DD_pointers;
   ctx->Driver.SetBuffer = SWFSDriver_set_buffer;
   ctx->Driver.GetBufferSize = SWFSDriver_resize;
   switch (c->fmt)
   {
      case PIXFMT_LUT8:
	ctx->Driver.Index = SWFSD_set_index_LUT8;
	ctx->Driver.Color = SWFSD_set_color_LUT8;
	ctx->Driver.Clear = SWFSD_clear_LUT8;
	break;
      case PIXFMT_RGB15:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGB15;
	ctx->Driver.Clear = SWFSD_clear_RGB15;
	break;
      case PIXFMT_RGB15PC:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGB15PC;
	ctx->Driver.Clear = SWFSD_clear_RGB15PC;
	break;
      case PIXFMT_RGB16:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGB16;
	ctx->Driver.Clear = SWFSD_clear_RGB16;
	break;
      case PIXFMT_RGB16PC:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGB16PC;
	ctx->Driver.Clear = SWFSD_clear_RGB16PC;
	break;
      case PIXFMT_BGR15PC:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_BGR15PC;
	ctx->Driver.Clear = SWFSD_clear_BGR15PC;
	break;
      case PIXFMT_BGR16PC:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_BGR16PC;
	ctx->Driver.Clear = SWFSD_clear_BGR16PC;
	break;
      case PIXFMT_RGB24:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGB;
	ctx->Driver.Clear = SWFSD_clear_RGB;
	break;
      case PIXFMT_BGR24:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_BGR;
	ctx->Driver.Clear = SWFSD_clear_BGR;
	break;
      case PIXFMT_ARGB32:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_ARGB;
	ctx->Driver.Clear = SWFSD_clear_ARGB;
	break;
      case PIXFMT_BGRA32:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_BGRA;
	ctx->Driver.Clear = SWFSD_clear_BGRA;
	break;
      case PIXFMT_RGBA32:
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_RGBA;
	ctx->Driver.Clear = SWFSD_clear_RGBA;
	break;
   }
   Common_DD_pointers(ctx);
}
;;//
;// SWFSDriver_SwapBuffer_FS
/*
 *
 * SWFSDriver_SwapBuffer_FS
 * swaps front and hidden bitmap in full screen mode
 *
 */

void SWFSDriver_SwapBuffer_FS(struct amigamesa_context *c)
{
#ifndef __PPC__
	struct ScreenBuffer* temp;
#else
	extern void SwapBuffer_FS(void);
	struct PPCArgs CallArgs;
#endif

#ifndef __PPC__
	c->sbuf2->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = c->dbport;
	while (!(ChangeScreenBuffer(c->Screen,c->sbuf2)))
		WaitTOF();
	temp = c->sbuf1;
	c->sbuf1 = c->sbuf2;
	c->sbuf2 = temp;
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
	c->back2_rp->BitMap = c->sbuf1->sb_BitMap;
	c->BackArray = (unsigned char *)GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_DISPADR);
	c->FrontArray = (unsigned char *)GetCyberMapAttr(c->back2_rp->BitMap,CYBRMATTR_DISPADR);
	WaitBOVP(&c->Screen->ViewPort);
	while (!GetMsg(c->dbport))
		Wait(1L<<(c->dbport->mp_SigBit));
	c->SwapCounter ^= 1;
#else
	CallArgs.PP_Code = (void *)SwapBuffer_FS;
	CallArgs.PP_Offset = 0;
	CallArgs.PP_Flags = 0;
	CallArgs.PP_Stack = 0;
	CallArgs.PP_StackSize = 0;
	CallArgs.PP_Regs[8] = (unsigned long)c;
	CallArgs.PP_Regs[12] = (unsigned long)getLibBase();
	Run68K(&CallArgs);
#endif
	SetBuffer(c);
}
;;//
;// SWFSDriver_SwapBuffer_FS3
/*
 *
 * SWFSDriver_SwapBuffer_FS
 * swaps front and hidden bitmap in full screen mode
 * uses triple buffering
 *
 */

void SWFSDriver_SwapBuffer_FS3(struct amigamesa_context *c)
{
#ifndef __PPC__
	struct ScreenBuffer* temp;
#else
	extern void SwapBuffer_FS3(void);
	struct PPCArgs CallArgs;
#endif

#ifndef __PPC__
	while (!(ChangeScreenBuffer(c->Screen,c->sbuf2)))
		WaitTOF();
	temp = c->sbuf1;
	c->sbuf1 = c->sbuf2;
	c->sbuf2 = c->sbuf3;
	c->sbuf3 = temp;
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
	c->back2_rp->BitMap = c->sbuf1->sb_BitMap;
	c->BackArray = (unsigned char *)GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_DISPADR);
	c->FrontArray = (unsigned char *)GetCyberMapAttr(c->back2_rp->BitMap,CYBRMATTR_DISPADR);
	c->SwapCounter = (c->SwapCounter+1) % 3;
#else
	CallArgs.PP_Code = (void *)SwapBuffer_FS3;
	CallArgs.PP_Offset = 0;
	CallArgs.PP_Flags = 0;
	CallArgs.PP_Stack = 0;
	CallArgs.PP_StackSize = 0;
	CallArgs.PP_Regs[8] = (unsigned long)c;
	CallArgs.PP_Regs[12] = (unsigned long)getLibBase();
	Run68K(&CallArgs);
#endif
	SetBuffer(c);
}
;;//
;// SWFSDriver_SwapBuffer_DR
/*
 *
 * SWFSDriver_SwapBuffer_DR
 * copies the hidden frame to the current rastport using
 * graphics/BltBitMapRastPort
 *
 */

void SWFSDriver_SwapBuffer_DR(struct amigamesa_context *c)
{
	struct BitMap* bm;

	c->backarrayflag ^= 1;
	SetBuffer(c);
	if (c->rp == NULL)
		return;
	if (c->flags & FLAG_SYNC)
		WaitBOVP(&c->Screen->ViewPort);
	bm = c->backarrayflag ? c->back_rp->BitMap : c->back2_rp->BitMap;
	BltBitMapRastPort(bm,
			  0,
			  0,
			  c->front_rp,
			  c->left,
			  c->top,
			  c->width,
			  c->height,
			  0xc0);
}
;;//
;// SWFSDriver_SwapBuffer_AGA
/*
 *
 * SWFSDriver_SwapBuffer_AGA
 * converts the hidden frame to planar format into the hidden bitmap
 *
 */

void SWFSDriver_SwapBuffer_AGA(struct amigamesa_context *c)
{
#ifndef __PPC__
	struct ScreenBuffer* temp;
	UBYTE* temp2;
#else
	extern void SwapBuffer_AGA(void);
	struct PPCArgs CallArgs;
#endif

	ChunkyToPlanar(c->BackArray,c->back_rp->BitMap,c->width,c->height);
#ifndef __PPC__
	c->sbuf2->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = c->dbport;
	while (!(ChangeScreenBuffer(c->Screen,c->sbuf2)))
		WaitTOF();
	temp = c->sbuf1;
	c->sbuf1 = c->sbuf2;
	c->sbuf2 = temp;
	temp2 = c->BackArray;
	c->BackArray = c->FrontArray;
	c->FrontArray = temp2;
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
	WaitBOVP(&c->Screen->ViewPort);
	while (!GetMsg(c->dbport))
		Wait(1L<<(c->dbport->mp_SigBit));
	c->SwapCounter ^= 1;
#else
	CallArgs.PP_Code = (void *)SwapBuffer_AGA;
	CallArgs.PP_Offset = 0;
	CallArgs.PP_Flags = 0;
	CallArgs.PP_Stack = 0;
	CallArgs.PP_StackSize = 0;
	CallArgs.PP_Regs[8] = (unsigned long)c;
	CallArgs.PP_Regs[12] = (unsigned long)getLibBase();
	Run68K(&CallArgs);
#endif
	SetBuffer(c);
}
;;//
;// SWFSDriver_Dispose
/*
 *
 *   Driver shutdown  - handles AGA and gfxboard driver
 *
 */

void SWFSDriver_Dispose(struct amigamesa_context *c)
{
    if (c->depth <= 8)
    {
	if (c->flags & FLAG_RGBA)
	{
	    DisposeTransTable(c);
	}
	else
	{
	    FreeTransTable(c);
	}
    }
    else
    {
	if (!(c->flags & FLAG_RGBA))
	{
	    FreeTransTable(c);
	}
    }

    if (c->flags & FLAG_FULLSCREEN)
    {
	if (c->SwapCounter != 0)
	{
#ifdef __PPC__
	    extern void FinalSwap(void);
	    struct PPCArgs CallArgs;
#endif
#ifndef __PPC__
	    c->sbuf_initial->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
	    while (!(ChangeScreenBuffer(c->Screen,c->sbuf_initial)))
		WaitTOF();
#else
	    CallArgs.PP_Code = (void *)FinalSwap;
	    CallArgs.PP_Offset = 0;
	    CallArgs.PP_Flags = 0;
	    CallArgs.PP_Stack = 0;
	    CallArgs.PP_StackSize = 0;
	    CallArgs.PP_Regs[8] = (unsigned long)c;
	    CallArgs.PP_Regs[12] = (unsigned long)getLibBase();
	    Run68K(&CallArgs);
#endif
	}
	FreeScreenBuffer(c->Screen,c->sbuf1);
	FreeScreenBuffer(c->Screen,c->sbuf2);
	if (c->flags & FLAG_TRIPLE)
		FreeScreenBuffer(c->Screen,c->sbuf3);
	DeleteMsgPort(c->dbport);
    }
    else
    {
	FreeBitMap(c->back_rp->BitMap);
	if (c->flags & FLAG_TWOBUFFERS)
		FreeBitMap(c->back2_rp->BitMap);
    }
#ifndef __PPC__
    FreeVec(c->back_rp);
    if (c->SwapBuffer != SWFSDriver_SwapBuffer_AGA)
	FreeVec(c->back2_rp);
#else
    FreeVecPPC(c->back_rp);
    if (c->SwapBuffer != SWFSDriver_SwapBuffer_AGA)
	FreeVecPPC(c->back2_rp);
#endif
    if (c->SwapBuffer == SWFSDriver_SwapBuffer_AGA)
    {
#ifndef __PPC__
	FreeVec(c->BackArray);
	if (c->flags & FLAG_TWOBUFFERS)
		FreeVec(c->FrontArray);
#else
	FreeVecPPC(c->BackArray);
	if (c->flags & FLAG_TWOBUFFERS)
		FreeVecPPC(c->FrontArray);
#endif
    }
}
;;//
;// LaunchGfxboardDriver

/*
 *
 *   Gfxboard driver startup (called from the initial startup function)
 *
 */

static BOOL LaunchGfxboardDriver(struct amigamesa_context *c,struct TagItem *tagList)
{
    if (c->flags & FLAG_DIRECTRENDER)
    {
	c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
	c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
	c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;
	c->left = GetTagData(AMA_Left,0,tagList);
	c->bottom = GetTagData(AMA_Bottom,0,tagList);
	c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
	c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);
    }
    else
    {
	c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
	c->RealWidth = c->Screen->Width;
	c->RealHeight= c->Screen->Height;
	c->width = c->RealWidth;
	c->height = c->RealHeight;
	c->left = 0;
	c->bottom = 0;
	c->flags |= FLAG_TWOBUFFERS;
    }
    c->right = c->RealWidth - c->width - c->left;
    c->top = c->RealHeight - c->height - c->bottom;
#ifndef __PPC__
    if (!(c->back_rp = AllocVec(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR)))
	return(FALSE);
    InitRastPort(c->back_rp);
    if (!(c->back2_rp = AllocVec(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR)))
	return(FALSE);
    InitRastPort(c->back2_rp);
#else
    if (!(c->back_rp = AllocVecPPC(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR,0)))
	return(FALSE);
    InitRastPort(c->back_rp);
    if (!(c->back2_rp = AllocVecPPC(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR,0)))
	return(FALSE);
    InitRastPort(c->back2_rp);
#endif
    c->draw_rp = c->back_rp;
    if (c->flags & FLAG_FULLSCREEN)
    {
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
	c->back2_rp->BitMap = c->sbuf1->sb_BitMap;
    }
    else
    {
	if (!(c->back_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
		return(FALSE);
	c->back2_rp->BitMap = c->back_rp->BitMap;
	if (c->flags & FLAG_TWOBUFFERS)
	{
		if (!(c->back2_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
			return(FALSE);
	}
    }
    c->BackArray = (unsigned char *)GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_DISPADR);
    c->FrontArray = c->BackArray;
    SetAPen(c->back_rp,0);
    RectFill(c->back_rp,0,0,c->width-1,c->height-1);
    if (c->flags & FLAG_FULLSCREEN)
    {
	c->back_rp->BitMap = c->sbuf1->sb_BitMap;
	RectFill(c->back_rp,0,0,c->width-1,c->height-1);
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
	c->FrontArray = (unsigned char *)GetCyberMapAttr(c->back2_rp->BitMap,CYBRMATTR_DISPADR);
    }
    else if (c->flags & FLAG_TWOBUFFERS)
    {
	SetAPen(c->back2_rp,0);
	RectFill(c->back2_rp,0,0,c->width-1,c->height-1);
	c->FrontArray = (unsigned char *)GetCyberMapAttr(c->back2_rp->BitMap,CYBRMATTR_DISPADR);
    }
    c->DrawBuffer = c->BackArray;
    c->ReadBuffer = c->BackArray;
    c->drawbufferflag = 0; /* back buffer */
    c->readbufferflag = 0; /* back buffer */
    c->backarrayflag = 0; /* back array really points to back buffer */
    c->bprow = GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_XMOD);
    c->bppix = GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_BPPIX);
    c->fmt = GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_PIXFMT);

    if (c->depth <= 8)
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
    else
    {
	if (!(c->visual->rgb_flag))
	{
	    if (!(InitTransTable(c)))
		return(FALSE);
	}
    }
    c->InitDD = (void(*)(void*))SWFSDriver_DD_pointers;
    c->Dispose = SWFSDriver_Dispose;
    if (c->flags & FLAG_FULLSCREEN)
    {
	if (c->flags & FLAG_TRIPLE)
		c->SwapBuffer = SWFSDriver_SwapBuffer_FS3;
	else
		c->SwapBuffer = SWFSDriver_SwapBuffer_FS;
    }
    else
	c->SwapBuffer = SWFSDriver_SwapBuffer_DR;
    return(TRUE);
}
;;//
;// LaunchAGADriver
/*
 *
 *   AGA driver startup (called from the initial startup function)
 *
 */

static BOOL LaunchAGADriver(struct amigamesa_context *c,struct TagItem *tagList)
{
    if ((c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH)) < 8)
	return(FALSE);
    c->RealWidth = c->Screen->Width;
    c->RealHeight= c->Screen->Height;
    c->width = c->RealWidth;
    c->height = c->RealHeight;
    c->left = 0;
    c->bottom = 0;
    c->right = c->RealWidth - c->width - c->left;
    c->top = c->RealHeight - c->height - c->bottom;
    c->FrontArray = 0;
#ifndef __PPC__
    if (!(c->BackArray = AllocVec(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
	return(FALSE);
    c->FrontArray = c->BackArray;
    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (!(c->FrontArray = AllocVec(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
		return(FALSE);
    }
#else
    if (!(c->BackArray = AllocVecPPC(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
	return(FALSE);
    c->FrontArray = c->BackArray;
    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (!(c->FrontArray = AllocVecPPC(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
		return(FALSE);
    }
#endif

    c->DrawBuffer = c->BackArray;
    c->ReadBuffer = c->BackArray;
    c->drawbufferflag = 0; /* back buffer */
    c->readbufferflag = 0; /* back buffer */
    c->backarrayflag = 0; /* back array really points to back buffer */
#ifndef __PPC__
    if (!(c->back_rp = AllocVec(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR)))
	    return(FALSE);
#else
    if (!(c->back_rp = AllocVecPPC(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR,0)))
	    return(FALSE);
#endif
    InitRastPort(c->back_rp);
    c->draw_rp = c->back_rp;
    c->back_rp->BitMap = c->sbuf2->sb_BitMap;
    SetAPen(c->back_rp,0);
    RectFill(c->back_rp,0,0,c->width-1,c->height-1);
    if (c->flags & FLAG_FULLSCREEN)
    {
	c->back_rp->BitMap = c->sbuf1->sb_BitMap;
	RectFill(c->back_rp,0,0,c->width-1,c->height-1);
	c->back_rp->BitMap = c->sbuf2->sb_BitMap;
    }
    c->bprow = c->width;
    c->bppix = 1;
    c->fmt = PIXFMT_LUT8;
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
    c->InitDD = (void(*)(void *))SWFSDriver_DD_pointers;
    c->Dispose = SWFSDriver_Dispose;
    c->SwapBuffer = SWFSDriver_SwapBuffer_AGA;
    return(TRUE);
}
;;//
;// SWFSDriver_init

/*
 *
 *   Driver startup  -  called for AGA and gfxboard drivers
 *
 */


BOOL SWFSDriver_init(struct amigamesa_context *c,struct TagItem *tagList)
    {
    c->front_rp = c->rp;
    if (c->flags & FLAG_FULLSCREEN)
    {
	if (CyberGfxBase)
	{
		if (!(IsCyberModeID(GetVPModeID(&c->Screen->ViewPort))))
			c->flags &= (~FLAG_TRIPLE);
	}
	if (!(c->sbuf1 = AllocScreenBuffer(c->Screen,NULL,SB_SCREEN_BITMAP)))
	    return(FALSE);
	c->sbuf_initial = c->sbuf1;
	if (!(c->sbuf2 = AllocScreenBuffer(c->Screen,NULL,0)))
	{
	    FreeScreenBuffer(c->Screen,c->sbuf1);
	    c->sbuf1 = NULL;
	    return(FALSE);
	}
	if (c->flags & FLAG_TRIPLE)
	{
	    if (!(c->sbuf3 = AllocScreenBuffer(c->Screen,NULL,0)))
	    {
		FreeScreenBuffer(c->Screen,c->sbuf1);
		FreeScreenBuffer(c->Screen,c->sbuf2);
		c->sbuf1 = NULL;
		c->sbuf2 = NULL;
		return(FALSE);
	    }
	}
	c->front_rp = c->rp = &c->Screen->RastPort;
	if (!(c->dbport = CreateMsgPort()))
	{
		FreeScreenBuffer(c->Screen,c->sbuf1);
		FreeScreenBuffer(c->Screen,c->sbuf2);
		if (c->flags & FLAG_TRIPLE)
			FreeScreenBuffer(c->Screen,c->sbuf3);
		return(FALSE);
	}
	c->SwapCounter = 0;
    }
    if (CyberGfxBase)
	if (IsCyberModeID(GetVPModeID(&c->Screen->ViewPort)))
	    return(LaunchGfxboardDriver(c,tagList));
    return(LaunchAGADriver(c,tagList));
}
;;//
//wawa>
void ChunkyToPlanar(unsigned char* src,struct BitMap *dest,int width,int
height)
{return;}
//<wawa

