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

/* New driver supporting 3D hardware through Warp3D. Based on
   ADisp_SWFS.c. These two files interact with eachother, which
   allows to combine software rendering and hardware rendering to
   achieve the optimal result. If high speed is required, then
   the 3D hardware does most of the job, but maybe not very
   accurate with respect to the OpenGL requirements, if high
   quality is desired, then the software engine will do more,
   but speed will go down

   2.10.1998 - start coding
*/

#include "AmigaIncludes.h"
#include "AmigaPrefs.h"
extern struct STORMMESA_parameters StormMesa;

/* A value of 0 means, that all mipmaps are provided by the application */
/* And this value means that all mipmaps will be created with Warp3D    */
#define W3D_REDOALLMIPMAPS 0xffffffff
#define W3D_HAVEALLMIPMAPS 0

#include <stdlib.h>
#include <gl/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"
#include "types.h"
#include "depth.h"

#include "AmigaMesa.h"
 
#include "ADisp_SW.h"
#include "ADisp_SWFS.h"
#include "ADisp_HW.h"

#ifdef WARPUP
#pragma pack(push,2)
#include <powerpc/powerpc.h>
#pragma pack(pop)
#endif

void HW_DrawQuad_smooth(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint v4, GLuint pv);
void HW_DrawQuad_flat(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint v4, GLuint pv);
void HW_DrawTriangle_smooth(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint pv);
void HW_DrawTriangle_flat(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint pv);
void HW_DrawLine_smooth(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv);
void HW_DrawLine_flat(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv);
void HW_DrawPoint_smooth(GLcontext *ctx, GLuint first, GLuint last);
void HW_DrawPoint_flat(GLcontext *ctx, GLuint first, GLuint last);

void HW_DrawQuad_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint v4, GLuint pv);
void HW_DrawTriangle_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2,
						 GLuint v3, GLuint pv);
void HW_DrawLine_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv);
void HW_DrawPoint_smoothTwo(GLcontext *ctx, GLuint first, GLuint last);
void HW_DrawLineStrip(GLcontext *ctx, GLuint first, GLuint last);
void HW_DrawTriStrip(GLcontext *ctx, GLuint first, GLuint last);
void HW_DrawTriFan(GLcontext *ctx, GLuint zero, GLuint first, GLuint last);


void HW_SetupFlat( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupFlatZ( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraud( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraudZ( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupFlatFog( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupFlatFogZ( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraudFog( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraudFogZ( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupFlatTex( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupFlatTexZ( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraudTex( GLcontext *ctx, GLuint start, GLuint end );
void HW_SetupGouraudTexZ( GLcontext *ctx, GLuint start, GLuint end );

void HW_CopyFlat(W3D_Vertex*, W3D_Vertex*);
void HW_CopyFlatZ(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraud(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraudZ(W3D_Vertex*, W3D_Vertex*);
void HW_CopyFlatFog(W3D_Vertex*, W3D_Vertex*);
void HW_CopyFlatFogZ(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraudFog(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraudFogZ(W3D_Vertex*, W3D_Vertex*);
void HW_CopyFlatTex(W3D_Vertex*, W3D_Vertex*);
void HW_CopyFlatTexZ(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraudTex(W3D_Vertex*, W3D_Vertex*);
void HW_CopyGouraudTexZ(W3D_Vertex*, W3D_Vertex*);

void HWDriver_Lock3(struct amigamesa_context* c);
void HWDriver_UnLock3(struct amigamesa_context* c);

#ifndef __PPC__
#define INTERVAL 40000
#else
#define INTERVAL 50000
#endif
struct timeval tval1,tval2;

#define STORMMESADEBUG 1
#ifdef STORMMESADEBUG
void LibPrintf(const char *string, ...);
#define VAR(var)	 /*LibPrintf*/ printf(#var ": %d \n",var);
#else
#define VAR(var) ;
#endif


;// texts
const char * checkstrings[] = {
		"3D hardware not accessible (bitmap might not be in VRAM)",
		"not enough video memory for Z buffer",
		"no hardware acceleration possible for the selected display",
		"draw buffer disabled",
		"texture mapping mode not supported by Warp3D",
		"texture image structure is NULL",
		"texture mapping not supported by hardware",
		"wrapping not supported for linear texture mapping",
		"clamping not supported for linear texture mapping",
		"texture too large for linear texture mapping",
		"perspective texture mapping not supported by hardware",
		"wrapping not supported for perspective texture mapping",
		"clamping not supported for perspective texture mapping",
		"texture too large for perspective texture mapping",
		"bilinear filtering not supported by hardware",
		"mipmapping not supported by hardware",
		"depth filter (mipmap blending) not supported by hardware",
		"texture environment GL_REPLACE not supported",
		"texture environment GL_DECAL not supported",
		"texture environment GL_MODULATE not supported",
		"texture environment GL_BLEND not supported",
		"rectangular textures not supported by hardware",
		"no texture defined as current texture",
		"texture image structure is NULL",
		"texture format is not supported by hardware",
		"mipmaps have different texture formats",
		"no W3D texture object available",
		"smooth shading not supported by hardware",
		"flat shading not supported by hardware",
		"z buffering not supported by hardware",
		"z compare modes not supported by hardware",
		"z buffer update not supported by hardware",
		"alpha test not supported by hardware",
		"alpha test modes not supported by hardware",
		"alpha blending not supported by hardware",
		"alpha blending factors not supported by hardware",
		"fogging not supported by hardware",
		"linear fogging not supported by hardware",
		"exponential fogging not supported by hardware",
		"square exponential fogging not supported by hardware",
		"point antialiasing not supported by hardware",
		"line antialiasing not supported by hardware",
		"polygon antialiasing not supported by hardware",
		"dithering not supported by hardware",
		"scissoring not supported by hardware",
		"logic ops not supported by hardware",
		"color/index masking not supported by hardware",
		"stencil buffering not supported by hardware",
		"stencil function not supported by hardware",
		"stencil mask not supported by hardware",
		"stencil operation (sfail) not supported by hardware",
		"stencil operation (dpfail) not supported by hardware",
		"stencil operation (dppass) not supported by hardware",
		"stencil write protection not supported by hardware",
		"hardware acceleration not supported for color index mode",
		"assymmetric wrapping not supported",
		"GL_DECAL / alpha blending / fog combination not supported",
		"no hardware acceleration available for 8 bit screens",
		"specular highlighting not supported",
		"not enough video memory for textures"
		};
;;//

//#define DBG 1

// misc support
;// logbase2
int logbase2(int n)
{
   GLint i = 1;
   GLint log2 = 0;

   if (n<0) {
	  return -1;
   }

   while ( n > i ) {
	  i *= 2;
	  log2++;
   }
   if (i != n) {
	  return -1;
   }
   else {
	  return log2;
   }
}
;;//
;// HWDriver_set_buffer
GLboolean HWDriver_set_buffer( GLcontext *ctx,GLenum mode )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;

#ifdef DBG
	/*LibPrintf*/ printf("HWDriver_set_buffer\n");
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

		hwcontext->currentbm = c->draw_rp->BitMap;
		W3D_SetDrawRegion(context,hwcontext->currentbm,0,&hwcontext->scissor);
	}
	return(GL_TRUE);
}
;;//
;// HWDriver_resize
void HWDriver_resize( GLcontext *ctx,GLuint *width, GLuint *height)
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;

	*width=c->width;
	*height=c->height;
	if (c->rp == NULL)
		return;

#ifdef DBG
	/*LibPrintf*/ printf("HWDriver_resize: %d %d\n",c->width,c->height);
#endif

	if (c->flags & FLAG_DIRECTRENDER)
	{
	printf("Phase 1\n");

	if(!((c->width  == (c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX-c->left-c->right+1))
	&&  (c->height == (c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY-c->bottom-c->top+1))))
		{
	printf("Width: %d, MaxX: %d, MinX: %d, left: %d, right: %d\n",c->width,c->rp->Layer->bounds.MaxX,c->rp->Layer->bounds.MinX,c->left,c->right);
	printf("Height: %d, MaxY: %d, MinY: %d, bottom: %d, top: %d\n",c->height,c->rp->Layer->bounds.MaxY,c->rp->Layer->bounds.MinY,c->bottom,c->top);
			c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
			c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;
	printf("RealWidth: %d\n",c->RealWidth);
	printf("RealHeight: %d\n",c->RealHeight);
			*width=c->width = c->RealWidth-c->left-c->right;
			*height=c->height = c->RealHeight-c->bottom-c->top;
			FreeBitMap(c->back_rp->BitMap);
#if 0
			if (!(c->back_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
#else
			if (!(c->back_rp->BitMap = AllocBitMap(((c->width+63)&0xffffffc0),c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
#endif
			{
				/*LibPrintf*/ printf("Panic! Not enough memory for resizing!\n");
				exit(0);
			}
			SetAPen(c->back_rp,0);
			RectFill(c->back_rp,0,0,c->width-1,c->height-1);
			if (c->flags & FLAG_TWOBUFFERS)
			{
				FreeBitMap(c->back2_rp->BitMap);
#if 0
				if (!(c->back2_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
#else
				if (!(c->back2_rp->BitMap = AllocBitMap(((c->width+63)&0xffffffc0),c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
#endif
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

			hwcontext->currentbm = c->draw_rp->BitMap;
			W3D_SetDrawRegion(context,hwcontext->currentbm,0,&hwcontext->scissor);
			hwcontext->flags &= (~HWFLAG_NOVMEM);
			//hwcontext->flags |= HWFLAG_SCISSOR;

			if (hwcontext->nohw)
			{
				if (W3D_AllocZBuffer(context) == W3D_SUCCESS)
				{
						W3D_Double clearvalue = (W3D_Double)ctx->Depth.Clear;
						hwcontext->nohw = FALSE;
						hwcontext->flags |= HWFLAG_ZBUFFER;
						W3D_ClearZBuffer(context,&clearvalue);
				}
			}
			else if (hwcontext->flags & HWFLAG_ZBUFFER)
			{
				W3D_FreeZBuffer(context);
				if (W3D_AllocZBuffer(context) != W3D_SUCCESS)
				{
						hwcontext->nohw = TRUE;
						hwcontext->flags &= (~HWFLAG_ZBUFFER);
				}
			}
		}
	}

}
;;//

// set RGBA color
;// HW_set_color_LUT8
void HW_set_color_LUT8(GLcontext *ctx,
							GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;
#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_LUT8: %d %d %d %d\n",r,g,b,a);
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_LUT8(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGB15
void HW_set_color_RGB15(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGB15\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGB15(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGB15PC
void HW_set_color_RGB15PC(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGB15PC\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGB15PC(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGB16
void HW_set_color_RGB16(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGB16\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGB16(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGB16PC
void HW_set_color_RGB16PC(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGB16PC\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGB16PC(ctx,r,g,b,a);
}
;;//
;// HW_set_color_BGR15PC
void HW_set_color_BGR15PC(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_BGR15PC\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_BGR15PC(ctx,r,g,b,a);
}
;;//
;// HW_set_color_BGR16PC
void HW_set_color_BGR16PC(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_BGR16PC\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_BGR16PC(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGB
void HW_set_color_RGB(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGB\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGB(ctx,r,g,b,a);
}
;;//
;// HW_set_color_BGR
void HW_set_color_BGR(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_BGR\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_BGR(ctx,r,g,b,a);
}
;;//
;// HW_set_color_ARGB
void HW_set_color_ARGB(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_ARGB\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_ARGB(ctx,r,g,b,a);
}
;;//
;// HW_set_color_BGRA
void HW_set_color_BGRA(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_BGRA\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_BGRA(ctx,r,g,b,a);
}
;;//
;// HW_set_color_RGBA
void HW_set_color_RGBA(GLcontext *ctx,
								  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Float* colf = hwcontext->colconv;
	W3D_Color co;

#ifdef DBG
	/*LibPrintf*/ printf("HW_set_color_RGBA\n");
#endif
	co.r = colf[r];
	co.g = colf[g];
	co.b = colf[b];
	co.a = colf[a];
	W3D_SetCurrentColor(context,&co);
	SWFSD_set_color_RGBA(ctx,r,g,b,a);
}
;;//

// clear buffer
;// HW_clear_generic
GLbitfield HW_clear_generic(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
  AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
  W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
  W3D_Context *context = hwcontext->context;

  if ((hwcontext->flags & HWFLAG_ACTIVE) &&
	  (hwcontext->flags & (HWFLAG_ZBUFFER|HWFLAG_SBUFFER)))
  {
	  if (hwcontext->lockmode >= 2)
		HWDriver_Lock3(c);
	  if ((hwcontext->flags & HWFLAG_ZBUFFER) && (mask & GL_DEPTH_BUFFER_BIT))
	  {
		  if (!(ctx->Visual->DepthBits==0 || !ctx->Depth.Mask))
		  {
			  W3D_Double clearvalue = (W3D_Double)ctx->Depth.Clear;
			  W3D_ClearZBuffer(context,&clearvalue);
			  hwcontext->flags |= HWFLAG_DIRTY;
			  mask &= (~GL_DEPTH_BUFFER_BIT);
		  }
	  }
/* TODO: enable this as soon as Mesa supports accelerated stencil buffering */
#if 0
	  if ((hwcontext->flags & HWFLAG_SBUFFER) && (mask & GL_STENCIL_BUFFER_BIT))
	  {
		  if (!(ctx->Visual->StencilBits==0))
		  {
			  W3D_ClearStencilBuffer(context,&ctx->Stencil.Clear);
			  hwcontext->flags |= HWFLAG_DIRTY;
			  mask &= (~GL_STENCIL_BUFFER_BIT);
		  }
	  }
#endif
  }
  else {
		W3D_Flush(context);
	}
  return(mask);
}
;;//
;// HW_clear_LUT8
GLbitfield HW_clear_LUT8(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_LUT8 %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_LUT8(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGB15
GLbitfield HW_clear_RGB15(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGB15 %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGB15(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGB15PC
GLbitfield HW_clear_RGB15PC(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGB15PC %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGB15PC(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGB16
GLbitfield HW_clear_RGB16(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGB16 %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGB16(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGB16PC
GLbitfield HW_clear_RGB16PC(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGB16PC %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGB16PC(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_BGR15PC
GLbitfield HW_clear_BGR15PC(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_BGR15PC %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_BGR15PC(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_BGR16PC
GLbitfield HW_clear_BGR16PC(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_BGR16PC %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_BGR16PC(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGB24
GLbitfield HW_clear_RGB24(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGB24 %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGB(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_BGR24
GLbitfield HW_clear_BGR24(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_BGR24 %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_BGR(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_ARGB
GLbitfield HW_clear_ARGB(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_ARGB %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_ARGB(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_BGRA
GLbitfield HW_clear_BGRA(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_BGRA %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_BGRA(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//
;// HW_clear_RGBA
GLbitfield HW_clear_RGBA(GLcontext *ctx, GLbitfield mask,
					  GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	GLbitfield m;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

#ifdef DBG
	/*LibPrintf*/ printf("HWD_clear_RGBA %ld\n",c->clearpixel);
#endif
	if (hwcontext->lockmode >= 2)
		HWDriver_UnLock3(c);
	m = SWFSD_clear_RGBA(ctx,mask,all,x,y,width,height);
	return(HW_clear_generic(ctx,m,all,x,y,width,height));
}
;;//

// renderer string
;// HWDriver_rendererstring
const char *HWDriver_rendererstring(void)
{
		return("AMIGA_WARP3D");
}
;;//

// stats
;// HW_PrintStats
/*
 *
 * prints rasterization statistics
 *
 */
void HW_PrintStats(W3Ddriver* hwcontext)
{
	int i;

	/*LibPrintf*/ printf("\n");
	/*LibPrintf*/ printf("3D hardware operation statistics\n");
	/*LibPrintf*/ printf("--------------------------------\n");
	/*LibPrintf*/ printf("\n");
	if (hwcontext->failed)
	{
		/*LibPrintf*/ printf("Rasterization compatibility check failed due to the\n");
		/*LibPrintf*/ printf("following reasons:\n");

		for (i=0;i<CHECK_MAX;i++)
		{
			if (hwcontext->statarray[i])
				/*LibPrintf*/ printf("%5d: %s\n",hwcontext->statarray[i],checkstrings[i]);
		}

		/*LibPrintf*/ printf("\n");
	}

	/*LibPrintf*/ printf("Rasterization statistics\n");
	/*LibPrintf*/ printf("\n");

	for (i=0;i<DRAW_MAX;i++)
		/*LibPrintf*/ printf("%s %d\n",drawstrings[i],hwcontext->statarray2[i]);

	/*LibPrintf*/ printf("\n");
}
;;//

// Z buffer
;// HW_AllocDepthBuffer
/*
 *
 * Z buffer resize notification
 *
 */
void HW_AllocDepthBuffer( GLcontext *ctx )
{
}
;;//
;// HW_DepthTestSpan
/*
 *
 * Z test
 *
 */
GLuint HW_DepthTestSpan( GLcontext *ctx,
							GLuint n, GLint x, GLint y, const GLdepth z[],
							GLubyte mask[] )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3D_Double zf[MAX_WIDTH];
   W3D_Double *zptr = zf;

   GLubyte *m = mask;
   GLuint i;
   GLuint passed = 0;

   if (hwcontext->flags & HWFLAG_DIRTY)
   {
		W3D_Flush(context);
		hwcontext->flags &= (~HWFLAG_DIRTY);
   }

/* these depth tests are really slow (VRAM accesses) therefore
   GL applications, which can't be handled by the installed
   3D hardware should be run with hardware acceleration disabled */

   /* switch cases ordered from most frequent to less frequent */
   switch (ctx->Depth.Func) {
	  case GL_LESS:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0; i<n; i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] < (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 /* fail */
					 *m = 0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0; i<n; i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] < (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_LEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] <= (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] <= (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_GEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] >= (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] >= (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_GREATER:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] > (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] > (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_NOTEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] != (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] != (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_EQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] == (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
					 passed++;
				  }
				  else {
					 *m =0;
				  }
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  if (z[i] == (GLdepth)(*zptr*MAX_DEPTH+0.5)) {
					 /* pass */
					 passed++;
				  }
				  else {
					 *m =0;
				  }
			   }
			}
		 }
		 break;
	  case GL_ALWAYS:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			W3D_ReadZSpan(context,x,c->height-y-1,n,zf);
			for (i=0;i<n;i++,zptr++,m++) {
			   if (*m) {
				  *zptr = ((W3D_Float)z[i]+0.5)/MAX_DEPTH;
				  passed++;
			   }
			}
			W3D_WriteZSpan(context,x,c->height-y-1,n,zf,(UBYTE *)mask);
		 }
		 else {
			/* Don't update Z buffer or mask */
			passed = n;
		 }
		 break;
	  case GL_NEVER:
		 for (i=0;i<n;i++) {
			mask[i] = 0;
		 }
		 break;
   } /*switch*/
   return passed;

}
;;//
;// HW_DepthTestPixels
/*
 *
 * Z test
 *
 */
void HW_DepthTestPixels( GLcontext *ctx,
							GLuint n, const GLint x[], const GLint y[],
							const GLdepth z[], GLubyte mask[] )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3D_Double f;

   register GLdepth *zptr;
   register GLuint i;

   if (hwcontext->flags & HWFLAG_DIRTY)
   {
		W3D_Flush(context);
		hwcontext->flags &= (~HWFLAG_DIRTY);
   }

/* these depth tests are really slow (VRAM accesses) therefore
   GL applications, which can't be handled by the installed
   3D hardware should be run with hardware acceleration disabled */

   /* switch cases ordered from most frequent to less frequent */
   switch (ctx->Depth.Func) {
	  case GL_LESS:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] < (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] < (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_LEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] <= (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] <= (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_GEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] >= (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] >= (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_GREATER:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] > (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] > (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_NOTEQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] != (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] != (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_EQUAL:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] == (GLdepth)(f*MAX_DEPTH+0.5)) {
						f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
						W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 else {
			/* Don't update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  W3D_ReadZPixel(context,x[i],c->height-y[i]-1,&f);
				  if (z[i] == (GLdepth)(f*MAX_DEPTH+0.5)) {
					 /* pass */
				  }
				  else {
					 mask[i] = 0;
				  }
			   }
			}
		 }
		 break;
	  case GL_ALWAYS:
		 if (ctx->Depth.Mask) {
			/* Update Z buffer */
			for (i=0; i<n; i++) {
			   if (mask[i]) {
				  f = ((W3D_Float)z[i]+0.5) / MAX_DEPTH;
				  W3D_WriteZPixel(context,x[i],c->height-y[i]-1,&f);
			   }
			}
		 }
		 else {
			/* Don't update Z buffer or mask */
		 }
		 break;
	  case GL_NEVER:
		 /* depth test never passes */
		 for (i=0;i<n;i++) {
			mask[i] = 0;
		 }
		 break;
   } /*switch*/
}
;;//
;// HW_ReadDepthSpanFloat
/*
 *
 * Z buffer read accesses
 *
 */
void HW_ReadDepthSpanFloat( GLcontext *ctx,
							   GLuint n, GLint x, GLint y, GLfloat depth[])
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Double z[MAX_WIDTH];
	int i;

	if (hwcontext->flags & HWFLAG_DIRTY)
	{
		W3D_Flush(context);
		hwcontext->flags &= (~HWFLAG_DIRTY);
	}
	W3D_ReadZSpan(context,x,c->height-y-1,n,z);
	for (i=0;i<n;i++)
		depth[i] = (float)z[i];

}
;;//
;// HW_ReadDepthSpanInt
/*
 *
 * Z buffer read accesses
 *
 */
void HW_ReadDepthSpanInt( GLcontext *ctx,
							 GLuint n, GLint x, GLint y, GLdepth depth[] )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;
	W3D_Double z[MAX_WIDTH];
	int i;

	if (hwcontext->flags & HWFLAG_DIRTY)
	{
		W3D_Flush(context);
		hwcontext->flags &= (~HWFLAG_DIRTY);
	}

	/* implicitly assumes, that GLfloat == float! */
	W3D_ReadZSpan(context,x,c->height-y-1,n,z);
	for (i=0;i<n;i++)
		depth[i] = (GLdepth)(z[i]*MAX_DEPTH+0.5);
}
;;//

// driver setup
;// HW_SetupFog
/*
 *
 * Fog initialisation
 *
 */
void HW_SetupFog( GLcontext *ctx )
{
	AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	W3D_Context *context = hwcontext->context;

	W3D_Fog fog;
	ULONG fogmode;
	W3D_Float wscale = hwcontext->wscale;

	if ((ctx->Fog.Mode == GL_LINEAR) || (!(c->flags & (FLAG_FAST|FLAG_VERYFAST))))
	{
		fog.fog_start = (1/ctx->Fog.Start)*wscale;
		fog.fog_end = (1/ctx->Fog.End)*wscale;
	}
	else
	{
		fog.fog_start = 1.0;
		fog.fog_end = (1/hwcontext->far)*wscale;
	}
	fog.fog_density = ctx->Fog.Density / wscale;

	if (ctx->Visual->RGBAflag)
	{
			fog.fog_color.r = ctx->Fog.Color[0];
			fog.fog_color.g = ctx->Fog.Color[1];
			fog.fog_color.b = ctx->Fog.Color[2];
	}
	else
	{
			ULONG index = ctx->Fog.Index;
			fog.fog_color.r = (W3D_Float)((((UBYTE *)c->ColorTable2)[index*4+1])/255.0);
			fog.fog_color.g = (W3D_Float)((((UBYTE *)c->ColorTable2)[index*4+2])/255.0);
			fog.fog_color.b = (W3D_Float)((((UBYTE *)c->ColorTable2)[index*4+3])/255.0);
	}
	switch (ctx->Fog.Mode)
	{
			case GL_LINEAR:
					fogmode = W3D_FOG_LINEAR;
					if (QUERY3D(W3D_Q_LINEAR) == W3D_NOT_SUPPORTED)
					{
						fogmode = W3D_FOG_INTERPOLATED;
					}
					break;
			case GL_EXP:
					if (c->flags & (FLAG_FAST|FLAG_VERYFAST))
							fogmode = W3D_FOG_INTERPOLATED;
					else
							fogmode = W3D_FOG_EXP;
					break;
			case GL_EXP2:
					if (c->flags & (FLAG_FAST|FLAG_VERYFAST))
							fogmode = W3D_FOG_INTERPOLATED;
					else
							fogmode = W3D_FOG_EXP_2;
					break;
	}
	W3D_SetFogParams(context,&fog,fogmode);
}
;;//
;// HW_DeleteTexture
/*
 *
 * deletes a texture
 *
 */
void HW_DeleteTexture( GLcontext *ctx, struct gl_texture_object *tObj )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3Dtexobj* to = (W3Dtexobj *)tObj->DriverData;
   W3Dteximg* ti;
   int i;

   if (to && to->texture)
   {
		if(to->texture!=NULL) W3D_FreeTexObj(context,to->texture);
   }
   for (i=0;i<MAX_TEXTURE_LEVELS;i++)
   {
		if (tObj->Image[i])
		{
				ti = ((W3Dteximg *)(tObj->Image[i]->DriverData));
				if (ti && ti->conv_image)
				{
#ifndef __PPC__
						FreeVec(ti->conv_image);
#else
						FreeVecPPC(ti->conv_image);
#endif
				}
				if (ti)
				{
#ifndef __PPC__
						FreeVec(ti);
#else
						FreeVecPPC(ti);
#endif
				}
				tObj->Image[i]->DriverData = NULL;
		}
   }
   if (to)
   {
#ifndef __PPC__
				FreeVec(to);
#else
				FreeVecPPC(to);
#endif
				tObj->DriverData = NULL;
   }
}
;;//
;// HW_BindTexture
/*
 *
 * defines the current texture
 *
 */

void HW_BindTexture( GLcontext *ctx, GLenum target,
						struct gl_texture_object *tObj )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct gl_texture_image *timg,*timg0;
   W3Dtexobj* to = (W3Dtexobj *)tObj->DriverData;
   W3Dteximg *ti,*ti0;
   int i,width,height;
   int level = 0;
   void* teximage;
   ULONG mask = W3D_REDOALLMIPMAPS;  
   void *mipmapptrs[MAX_TEXTURE_LEVELS]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   void **mptrs = mipmapptrs;

REM(HW_BindTexture);

   hwcontext->currenttex = NULL;
   if (!to)
   {
REM(no to);

#ifndef __PPC__
		if (!(to = (W3Dtexobj *)tObj->DriverData = AllocVec(sizeof(W3Dtexobj),MEMF_CLEAR)))
#else
		if (!(to = tObj->DriverData = AllocVecPPC(sizeof(W3Dtexobj),MEMF_CLEAR,0)))
#endif
			 return;	//was commented out??
		to->hasMipmaps = FALSE;
		to->minfilter = W3D_NEAREST;
		to->magfilter = W3D_LINEAR;
		to->wrap_s = W3D_REPEAT;
		to->wrap_t = W3D_REPEAT;
		to->MipMapFilter = FALSE;
		to->dirtymm = TRUE;
   }
   if (!to->texture)
   {
REM(no to->texture);

		if (!(timg0 = tObj->Image[0]))
				return;
		if (!(ti0 = (W3Dteximg *)(timg0->DriverData)))
				return;
		ti0->isW3DMap = TRUE;
		ti0->W3DMipmaplevel = 0;
		if (to->hasMipmaps)
		{
				width = timg0->Width;
				height = timg0->Height;
				if ( (logbase2(width) < 0) || (logbase2(height) < 0) )
						return;
				if ((width != height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
						{REM(rectangular texture); return;}
				while ((width>>1) && (height>>1))
				{
						width>>=1;
						height>>=1;
						level++;
						for (i=0;i<MAX_TEXTURE_LEVELS;i++)
						if(tObj->Image[i]!=NULL)
						{
								timg = tObj->Image[i];
								ti = (W3Dteximg *)(timg->DriverData);

								if ((timg->Width == width) && (timg->Height == height))
								{
										ti->isW3DMap = TRUE;
										ti->W3DMipmaplevel = level;
/* Thellier: Using user mipmaps cause problems with Warp3D so we disabled them
										mask &= (~(1<<(level-1)));
										if (to->converted)
												teximage = (void *)ti->conv_image;
										else
												teximage = (void *)timg->Data;
										*mptrs++ = teximage;
*/
										break;
								}
						}
				}
		}
		*mptrs++ = NULL;
		if (to->converted)
				teximage = (void *)ti0->conv_image;
		else
				teximage = (void *)timg0->Data;
		if (to->MipMapFilter || to->hasMipmaps)
		{
		REM(w3d_alloctex with mipmaps);
		VAR(mask);
				if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
							W3D_ATO_IMAGE,teximage,
							W3D_ATO_FORMAT,to->W3Dformat,
							W3D_ATO_WIDTH,timg0->Width,
							W3D_ATO_HEIGHT,timg0->Height,
							W3D_ATO_MIPMAP,W3D_HAVEALLMIPMAPS,
							W3D_ATO_MIPMAPPTRS,mipmapptrs,
							TAG_DONE)))
				{
						return;
				}
				to->MipMapTexture = TRUE;
		}
		else
		{
		REM(w3d_alloctex);
				if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
							W3D_ATO_IMAGE,teximage,
							W3D_ATO_FORMAT,to->W3Dformat,
							W3D_ATO_WIDTH,timg0->Width,
							W3D_ATO_HEIGHT,timg0->Height,
							TAG_DONE)))
				{
						return;
				}
				to->MipMapTexture = FALSE;
		}
		to->teximage = teximage;
		to->mmask = mask;
		to->width = timg0->Width;
		to->height = timg0->Height;
		W3D_SetFilter(context,to->texture,to->minfilter,to->magfilter);
		W3D_SetWrapMode(context,
		   to->texture,
		   to->wrap_s,
		   to->wrap_t,
		   &hwcontext->bordercolor);
   }
   hwcontext->currenttex = tObj;
   hwcontext->currentw3dtex = to->texture;
   hwcontext->texwidth = (W3D_Float)to->width;
   hwcontext->texheight = (W3D_Float)to->height;
}
;;//
;// HW_TexParameter
/*
 *
 * called fog glTexParameter
 *
 */

void HW_TexParameter( GLcontext *ctx, GLenum target,
						 struct gl_texture_object *tObj,
						 GLenum pname, const GLfloat *params )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3Dtexobj* to;
   GLenum param=(GLenum)(GLint)params[0];
   GLboolean newFilter = GL_FALSE;
   GLboolean newWrap = GL_FALSE;


   if (target != GL_TEXTURE_2D)
		return;

   if (!(to = (W3Dtexobj *)tObj->DriverData))
   {
#ifndef __PPC__
		if (!(to = (W3Dtexobj *)tObj->DriverData = AllocVec(sizeof(W3Dtexobj),MEMF_CLEAR)))
#else
		if (!(to = tObj->DriverData = AllocVecPPC(sizeof(W3Dtexobj),MEMF_CLEAR,0)))
#endif
				return;
		to->hasMipmaps = FALSE;
		to->minfilter = W3D_NEAREST;
		to->magfilter = W3D_LINEAR;
		to->wrap_s = W3D_REPEAT;
		to->wrap_t = W3D_REPEAT;
		to->MipMapFilter = FALSE;
		to->dirtymm = TRUE;
   }
   switch (pname)
   {
		case GL_TEXTURE_MIN_FILTER:
			newFilter = GL_TRUE;
			to->dirtymm = FALSE;
			switch(param)
			{
				case GL_NEAREST:
						to->MipMapFilter = FALSE;
						to->minfilter = W3D_NEAREST;
						break;
				case GL_LINEAR:
						to->MipMapFilter = FALSE;
						to->minfilter = W3D_LINEAR;
						break;
				case GL_NEAREST_MIPMAP_NEAREST:
						to->MipMapFilter = TRUE;
						to->minfilter = W3D_NEAREST_MIP_NEAREST;
						break;
				case GL_LINEAR_MIPMAP_NEAREST:
						to->MipMapFilter = TRUE;
						to->minfilter = W3D_LINEAR_MIP_NEAREST;
						break;
				case GL_NEAREST_MIPMAP_LINEAR:
						to->MipMapFilter = TRUE;
						to->minfilter = W3D_NEAREST_MIP_LINEAR;
						break;
				case GL_LINEAR_MIPMAP_LINEAR:
						to->MipMapFilter = TRUE;
						to->minfilter = W3D_LINEAR_MIP_LINEAR;
						break;
			}
			break;

		case GL_TEXTURE_MAG_FILTER:
			newFilter = GL_TRUE;
			to->dirtymm = FALSE;
			switch(param)
			{
				case GL_NEAREST:
						to->magfilter = W3D_NEAREST;
						break;
				case GL_LINEAR:
						to->magfilter = W3D_LINEAR;
						break;
			}
			break;

		case GL_TEXTURE_WRAP_S:
			newWrap = GL_TRUE;
			switch(param)
			{
				case GL_CLAMP:
						to->wrap_s = W3D_CLAMP;
						break;
				case GL_REPEAT:
						to->wrap_s = W3D_REPEAT;
						break;
			}
			break;
		case GL_TEXTURE_WRAP_T:
			newWrap = GL_TRUE;
			switch(param)
			{
				case GL_CLAMP:
						to->wrap_t = W3D_CLAMP;
						break;
				case GL_REPEAT:
						to->wrap_t = W3D_REPEAT;
						break;
			}
			break;
   }
   if (to->texture)
   {
		if (newFilter)
		{
				if ((!(to->MipMapTexture)) && (to->MipMapFilter))
				{
						W3D_Texture* wtobj = to->texture;

						if(to->texture!=NULL) W3D_FreeTexObj(context,to->texture);
						if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
									W3D_ATO_IMAGE,to->teximage,
									W3D_ATO_FORMAT,to->W3Dformat,
									W3D_ATO_WIDTH,to->width,
									W3D_ATO_HEIGHT,to->height,
									W3D_ATO_MIPMAP,W3D_REDOALLMIPMAPS,
									W3D_ATO_MIPMAPPTRS,NULL,
									TAG_DONE)))
						{
								return;
						}
						to->MipMapTexture = TRUE;
						newWrap = TRUE;
						if (wtobj == hwcontext->currentw3dtex)
								hwcontext->currentw3dtex = to->texture;
				}
				W3D_SetFilter(context,to->texture,to->minfilter,to->magfilter);
		}
		if (newWrap)
				W3D_SetWrapMode(context,to->texture,to->wrap_s,to->wrap_t,&hwcontext->bordercolor);
   }

}
;;//
;// HW_ConvertTexture
/*
 *
 * texture conversion from GL to W3D
 *
 */

GLboolean HW_ConvertTexture(W3D_Context* context, W3Dtexobj *to,
							W3Dteximg *ti,
							const struct gl_texture_image* image,
							GLint internalformat,GLint xoffset,GLint yoffset,
							GLsizei width,GLsizei height,int* bppix)
{
   switch (internalformat)
   {
		case GL_ALPHA:
		case GL_ALPHA4:
		case GL_ALPHA8:
		case GL_ALPHA12:
		case GL_ALPHA16:
				if (W3D_GetTexFmtInfo(context,W3D_A8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_A8;
				to->converted = FALSE;
				*bppix = 1;
				return GL_TRUE;
		case 1:
		case GL_LUMINANCE:
		case GL_LUMINANCE4:
		case GL_LUMINANCE8:
		case GL_LUMINANCE12:
		case GL_LUMINANCE16:
				if (W3D_GetTexFmtInfo(context,W3D_L8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_L8;
				to->converted = FALSE;
				*bppix = 1;
				return GL_TRUE;
		case 2:
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE4_ALPHA4:
		case GL_LUMINANCE6_ALPHA2:
		case GL_LUMINANCE8_ALPHA8:
		case GL_LUMINANCE12_ALPHA4:
		case GL_LUMINANCE12_ALPHA12:
		case GL_LUMINANCE16_ALPHA16:
				if (W3D_GetTexFmtInfo(context,W3D_L8A8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_L8A8;
				to->converted = FALSE;
				*bppix = 2;
				return GL_TRUE;
		case GL_INTENSITY:
		case GL_INTENSITY4:
		case GL_INTENSITY8:
		case GL_INTENSITY12:
		case GL_INTENSITY16:
				if (W3D_GetTexFmtInfo(context,W3D_I8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_I8;
				to->converted = FALSE;
				*bppix = 1;
				return GL_TRUE;
		case 3:
		case GL_RGB:
		case GL_R3_G3_B2:
		case GL_RGB4:
		case GL_RGB5:
		case GL_RGB8:
		case GL_RGB10:
		case GL_RGB12:
		case GL_RGB16:
				if (W3D_GetTexFmtInfo(context,W3D_R8G8B8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_R8G8B8;
				to->converted = FALSE;
				*bppix = 3;
				return GL_TRUE;
		case 4:
		case GL_RGBA:
		case GL_RGBA2:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGBA8:
		case GL_RGB10_A2:
		case GL_RGBA12:
		case GL_RGBA16:
				if (W3D_GetTexFmtInfo(context,W3D_R8G8B8A8,0) & W3D_TEXFMT_UNSUPPORTED)
						return GL_FALSE;
				to->W3Dformat = W3D_R8G8B8A8;
				to->converted = FALSE;
				*bppix = 4;
				return GL_TRUE;
		case GL_COLOR_INDEX1_EXT:
		case GL_COLOR_INDEX2_EXT:
		case GL_COLOR_INDEX4_EXT:
		case GL_COLOR_INDEX8_EXT:
		case GL_COLOR_INDEX12_EXT:
		case GL_COLOR_INDEX16_EXT:
		default:
				return GL_FALSE;
   }
}
;;//
;// HW_TexImage_generic
/*
 *
 * generic TexImage
 *
 */
void HW_TexImage_generic( GLcontext *ctx, GLenum target,
						struct gl_texture_object *tObj, GLint level,
						GLint xoffset, GLint yoffset,
						GLsizei width, GLsizei height,
						GLint internalFormat,
						struct gl_texture_image *image,
						GLboolean fullUpdate)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3Dtexobj* to;
   W3Dteximg* ti;
   ULONG wlevel = 0;
   int i,bppix;

   if(target!=GL_TEXTURE_2D)
		return;
/* Currently I don't want to write an emulation of rectangular textures,
   and they aren't anyway used much */

   if ((image->Width != image->Height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
		return;

/* this palette stuff annoys me... but actually almost no demos use
   the paletted textures extension */

   if (image->Format == GL_COLOR_INDEX)
		return;
   if (!(to = (W3Dtexobj *)tObj->DriverData))
   {
#ifndef __PPC__
		if (!(to = (W3Dtexobj *)tObj->DriverData = AllocVec(sizeof(W3Dtexobj),MEMF_CLEAR)))
#else
		if (!(to = tObj->DriverData = AllocVecPPC(sizeof(W3Dtexobj),MEMF_CLEAR,0)))
#endif
				return;
		to->hasMipmaps = FALSE;
		to->minfilter = W3D_NEAREST;
		to->magfilter = W3D_LINEAR;
		to->wrap_s = W3D_REPEAT;
		to->wrap_t = W3D_REPEAT;
		to->MipMapFilter = FALSE;
		to->dirtymm = TRUE;
   }
   if (level)
		to->hasMipmaps = TRUE;
   if (!(ti = (W3Dteximg *)image->DriverData))
   {

/* TODO: this structure is not freed, if several images are assigned
   to the same texture object */
   //image->DriverData = AllocVec(sizeof(W3Dteximg),MEMF_CLEAR);
   //if (!image->DriverData)return;

#ifndef __PPC__
		if (!(ti = (W3Dteximg *)image->DriverData = AllocVec(sizeof(W3Dteximg),MEMF_CLEAR)))
#else
		if (!(ti = image->DriverData = AllocVecPPC(sizeof(W3Dteximg),MEMF_CLEAR,0)))
#endif
		//		return;
		if (level)
				ti->isW3DMap = FALSE;
		else
				ti->isW3DMap = TRUE;
		while (level)
		{
				int lw,lh,tolw,tolh;

				if (!(to->texture))
						break;
				if ( ((lw = logbase2(image->Width)) < 0) || ((lh = logbase2(image->Height)) < 0) )
						break;
				if ((image->Width != image->Height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
						break;
				tolw = logbase2(to->width);
				tolh = logbase2(to->height);
				if ((tolw-lw) != (tolh-lh))
						break;
				if (!((to->mmask) & (1<<((tolw-lw)-1)))) /* urgs */
						break;
				/* What the hell is all this? Well, I'd like to now, if this
				   image can serve as W3D mipmap level, in which case it
				   replaces the generated one by W3D */
				ti->isW3DMap = TRUE;
				wlevel = ti->W3DMipmaplevel = tolw-lw;
				break;
		}
   }
/* if no filter was set up to now and a mipmap level was given, then set
   the filter mode to the correct initial state */

   if (level && ti->isW3DMap && to->dirtymm)
		to->minfilter = W3D_NEAREST_MIP_LINEAR;
   if (!HW_ConvertTexture(context,to,ti,image,internalFormat,xoffset,yoffset,width,height,&bppix))
		return; /* unsupported texture format */

   if (to->texture && ti->isW3DMap)
   {
		void *img;
		if (to->converted)
				img = (void *)ti->conv_image;
		else
				img = (void *)image->Data;
		if (!level)
				to->teximage = img;

/* case 1: texture was not a MipMap-Texture and a mipmap level was
   created. reallocate texture as MipMap-Texture */

		if ((!(to->MipMapTexture)) && level)
		{
				W3D_Texture* wtobj = to->texture;

				if(to->texture!=NULL) W3D_FreeTexObj(context,to->texture);
				if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
									W3D_ATO_IMAGE,to->teximage,
									W3D_ATO_FORMAT,to->W3Dformat,
									W3D_ATO_WIDTH,to->width,
									W3D_ATO_HEIGHT,to->height,
									W3D_ATO_MIPMAP,W3D_REDOALLMIPMAPS,
									W3D_ATO_MIPMAPPTRS,NULL,
									TAG_DONE)))
				{
						return;
				}
				to->mmask = ~0;
				to->MipMapTexture = TRUE;
				W3D_SetFilter(context,to->texture,to->minfilter,to->magfilter);
				W3D_SetWrapMode(context,to->texture,to->wrap_s,to->wrap_t,&hwcontext->bordercolor);
				if (wtobj == hwcontext->currentw3dtex)
				{
						hwcontext->currentw3dtex = to->texture;
						hwcontext->texwidth = (W3D_Float)to->width;
						hwcontext->texheight = (W3D_Float)to->height;
				}
		}

		if (!level && ((image->Width != to->width) || (image->Height != to->height)))
		{
				W3D_Texture* wtobj = to->texture;

				if(to->texture!=NULL) W3D_FreeTexObj(context,to->texture);
				if (!(to->MipMapTexture))
				{

/* case 2: texture dimension has changed and texture is not a mipmap texture.
		just reallocate the texture */

						if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
									W3D_ATO_IMAGE,to->teximage,
									W3D_ATO_FORMAT,to->W3Dformat,
									W3D_ATO_WIDTH,image->Width,
									W3D_ATO_HEIGHT,image->Height,
									TAG_DONE)))
						{
								return;
						}
						to->width = image->Width;
						to->height = image->Height;
				}
				else
				{
					   

/* case 3: texture dimension has changed and texture was a mipmap texture.
		reallocate the texture and regenerate all mipmaps */

						if (!(to->texture = W3D_AllocTexObjTags(context,&hwcontext->error,
									W3D_ATO_IMAGE,to->teximage,
									W3D_ATO_FORMAT,to->W3Dformat,
									W3D_ATO_WIDTH,image->Width,
									W3D_ATO_HEIGHT,image->Height,
									W3D_ATO_MIPMAP,W3D_REDOALLMIPMAPS,
									W3D_ATO_MIPMAPPTRS,NULL,
									TAG_DONE)))
						{
								return;
						}
						to->width = image->Width;
						to->height = image->Height;
						to->mmask = ~0;
				}
				W3D_SetFilter(context,to->texture,to->minfilter,to->magfilter);
				W3D_SetWrapMode(context,to->texture,to->wrap_s,to->wrap_t,&hwcontext->bordercolor);
				if (wtobj == hwcontext->currentw3dtex)
				{
						hwcontext->currentw3dtex = to->texture;
						hwcontext->texwidth = (W3D_Float)image->Width;
						hwcontext->texheight = (W3D_Float)image->Height;
				}
		}

		if (fullUpdate)
		{
				W3D_UpdateTexImage(context, to->texture,img,ti->W3DMipmaplevel,NULL);
		}
		else
		{
				ULONG bprow;
				void *imgptr;
				W3D_Scissor sc;

				imgptr = (void *)(((char *)img) + (yoffset*image->Width+xoffset)*bppix);
				sc.left = xoffset;
				sc.top = yoffset;
				sc.width = width;
				sc.height = height;
				bprow = image->Width*bppix;
				STACKW3D_UpdateTexSubImage(context, to->texture,imgptr,ti->W3DMipmaplevel,NULL,&sc,bprow);
		}
   }
   if (!(hwcontext->currenttex))
		HW_BindTexture(ctx,GL_TEXTURE_2D,tObj);
}
;;//
;// HW_TexSubImage
/*
 *
 * called for glTexSubImagexxx
 *
 */
void HW_TexSubImage( GLcontext *ctx, GLenum target,
						struct gl_texture_object *tObj, GLint level,
						GLint xoffset, GLint yoffset,
						GLsizei width, GLsizei height,
						GLint internalFormat,
						const struct gl_texture_image *image )
{
   HW_TexImage_generic(ctx,target,tObj,level,xoffset,yoffset,width,height,
				  internalFormat,image,GL_FALSE);

}
;;//
;// HW_TexImage
/*
 *
 * called for glTexImagexxx
 *
 */
void HW_TexImage( GLcontext *ctx, GLenum target,
					 struct gl_texture_object *tObj, GLint level,
					 GLint internalFormat,
					 const struct gl_texture_image *image )
{
   HW_TexImage_generic(ctx,target,tObj,level,0,0,image->Width,image->Height,
				  internalFormat,image,GL_TRUE);
}
;;//
;// HW_TexEnv
/*
 *
 * called for glTexEnv
 *
 */
void HW_TexEnv( GLcontext *ctx, GLenum pname, const GLfloat *param )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   GLenum mode;

   switch (pname)
   {
		case GL_TEXTURE_ENV_MODE:
				mode = (GLenum) (GLint) *param;
				switch (mode)
				{
						case GL_MODULATE:
								hwcontext->envmode = W3D_MODULATE;
								break;
						case GL_BLEND:
								hwcontext->envmode = W3D_BLEND;
								break;
						case GL_DECAL:
								hwcontext->envmode = W3D_DECAL;
								break;
						case GL_REPLACE:
								hwcontext->envmode = W3D_REPLACE;
								break;
				}
				break;
		case GL_TEXTURE_ENV_COLOR:
				hwcontext->envcolor.r = param[0];
				hwcontext->envcolor.g = param[1];
				hwcontext->envcolor.b = param[2];
				hwcontext->envcolor.a = param[3];
				break;
   }
   W3D_SetTexEnv(context,hwcontext->currentw3dtex,hwcontext->envmode,&hwcontext->envcolor);
}
;;//
;// HW_GetParameteri
/*
 *
 * tells Mesa some device driver parameter
 *
 */
GLint HW_GetParameteri( const GLcontext *ctx, GLint param )
{
   switch (param)
   {
		case DD_MAX_TEXTURE_SIZE:
				return MAX_TEXTURE_SIZE;
		case DD_MAX_TEXTURES:
				return MAX_TEX_SETS;
		case DD_MAX_TEXTURE_COORD_SETS:
				return MAX_TEX_COORD_SETS;
		case DD_HAVE_HARDWARE_FOG:
				return 1;
   }
}
;;//
;// SW_GetParameteri
/*
 *
 * tells Mesa some device driver parameter
 *
 */
GLint SW_GetParameteri( const GLcontext *ctx, GLint param )
{
   switch (param)
   {
		case DD_MAX_TEXTURE_SIZE:
				return MAX_TEXTURE_SIZE;
		case DD_MAX_TEXTURES:
				return MAX_TEX_SETS;
		case DD_MAX_TEXTURE_COORD_SETS:
				return MAX_TEX_COORD_SETS;
		case DD_HAVE_HARDWARE_FOG:
				return 0;
   }
}
;;//
;// HW_Dither
/*
 *
 * Enables/Disable dithering
 *
 */
void HW_Dither(GLcontext *ctx, GLboolean enable )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (enable)
		W3D_SetState(context,W3D_DITHERING,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_DITHERING,W3D_DISABLE);
}
;;//
;// HW_NearFar
/*
 *
 * handles z values of near/far clipping plane
 *
 */

void HW_NearFar( GLcontext *ctx, GLfloat nearVal, GLfloat farVal )
{
		AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;

		hwcontext->near = nearVal;
		hwcontext->far = farVal;
		if (nearVal < 1)
				hwcontext->wscale = 1.0;
		else
				hwcontext->wscale = nearVal;
		HW_SetupFog(ctx);
}
;;//
;// HW_SetupDriver
/*
 *
 * Driver state initialisation
 *
 */
void HW_SetupDriver( GLcontext *ctx )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

	REM(HW_SetupDriver);
printf("GLcontext: 0x%08x, AM context: 0x%08x, HW context: 0x%08x, context: 0x%08x\n",ctx,c,hwcontext,context);
/* texmapping setup */
/* TODO: support 3d textures */

   //W3D_WaitIdle(context);
   if (ctx->Texture.Enabled)
   {
		W3D_SetState(context,W3D_TEXMAPPING,W3D_ENABLE);
		if (ctx->Hint.PerspectiveCorrection==GL_FASTEST)
				W3D_SetState(context,W3D_PERSPECTIVE,W3D_DISABLE);
		else
		{
		if (!(hwcontext->flags & 0x20000000))
				W3D_SetState(context,W3D_PERSPECTIVE,W3D_ENABLE);
		else
				W3D_SetState(context,W3D_PERSPECTIVE,W3D_DISABLE);
		}
   }
   else
		W3D_SetState(context,W3D_TEXMAPPING,W3D_DISABLE);

/* gouraud / flat shading setup */

   if (ctx->Light.ShadeModel==GL_SMOOTH)
		W3D_SetState(context,W3D_GOURAUD,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_GOURAUD,W3D_DISABLE);

/* depth buffer setup */

   if (ctx->Depth.Test)
   {
		ULONG zmode;

		if (!(hwcontext->flags & 0x80000000))
		{
				W3D_SetState(context,W3D_ZBUFFER,W3D_ENABLE);
		}
		if (!(hwcontext->flags & 0x40000000))
		{
		if (ctx->Depth.Mask)
				W3D_SetState(context,W3D_ZBUFFERUPDATE,W3D_ENABLE);
		else
				W3D_SetState(context,W3D_ZBUFFERUPDATE,W3D_DISABLE);
		}
		switch (ctx->Depth.Func)
		{
				case GL_NEVER:	zmode = W3D_Z_NEVER; break;
				case GL_LESS:	 zmode = W3D_Z_LESS; break;
				case GL_GEQUAL:   zmode = W3D_Z_GEQUAL; break;
				case GL_LEQUAL:   zmode = W3D_Z_LEQUAL; break;
				case GL_GREATER:  zmode = W3D_Z_GREATER; break;
				case GL_NOTEQUAL: zmode = W3D_Z_NOTEQUAL; break;
				case GL_EQUAL:	zmode = W3D_Z_EQUAL; break;
				case GL_ALWAYS:   zmode = W3D_Z_ALWAYS; break;
		}
		W3D_SetZCompareMode(context,zmode);
   }
   else
   {
		W3D_SetState(context,W3D_ZBUFFER,W3D_DISABLE);
		W3D_SetState(context,W3D_ZBUFFERUPDATE,W3D_DISABLE);
   }

/* alpha test setup */

   if (ctx->Color.AlphaEnabled)
   {
		W3D_Float ref;
		ULONG amode;

		W3D_SetState(context,W3D_ALPHATEST,W3D_ENABLE);
		ref = (((GLfloat)ctx->Color.AlphaRef)-0.5)/255.0;
		switch (ctx->Color.AlphaFunc)
		{
				case GL_NEVER:	amode = W3D_A_NEVER; break;
				case GL_LESS:	 amode = W3D_A_LESS; break;
				case GL_GEQUAL:   amode = W3D_A_GEQUAL; break;
				case GL_LEQUAL:   amode = W3D_A_LEQUAL; break;
				case GL_GREATER:  amode = W3D_A_GREATER; break;
				case GL_NOTEQUAL: amode = W3D_A_NOTEQUAL; break;
				case GL_EQUAL:	amode = W3D_A_EQUAL; break;
				case GL_ALWAYS:   amode = W3D_A_ALWAYS; break;
		}
		W3D_SetAlphaMode(context,amode,&ref);
   }
   else
		W3D_SetState(context,W3D_ALPHATEST,W3D_DISABLE);

/* blending check */

   if (ctx->Color.BlendEnabled)
   {
		ULONG srcfactor, destfactor;

		W3D_SetState(context,W3D_BLENDING,W3D_ENABLE);
		switch (ctx->Color.BlendSrc)
		{
				case GL_ZERO:					 srcfactor = W3D_ZERO; break;
				case GL_ONE:					  srcfactor = W3D_ONE; break;
				case GL_DST_COLOR:				srcfactor = W3D_DST_COLOR; break;
				case GL_ONE_MINUS_DST_COLOR:	  srcfactor = W3D_ONE_MINUS_DST_COLOR; break;
				case GL_SRC_ALPHA:				srcfactor = W3D_SRC_ALPHA; break;
				case GL_ONE_MINUS_SRC_ALPHA:	  srcfactor = W3D_ONE_MINUS_SRC_ALPHA; break;
				case GL_DST_ALPHA:				srcfactor = W3D_DST_ALPHA; break;
				case GL_ONE_MINUS_DST_ALPHA:	  srcfactor = W3D_ONE_MINUS_DST_ALPHA; break;
				case GL_SRC_ALPHA_SATURATE:	   srcfactor = W3D_SRC_ALPHA_SATURATE; break;
				case GL_CONSTANT_COLOR:		   srcfactor = W3D_CONSTANT_COLOR; break;
				case GL_ONE_MINUS_CONSTANT_COLOR: srcfactor = W3D_ONE_MINUS_CONSTANT_COLOR; break;
				case GL_CONSTANT_ALPHA:		   srcfactor = W3D_CONSTANT_ALPHA; break;
				case GL_ONE_MINUS_CONSTANT_ALPHA: srcfactor = W3D_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		switch (ctx->Color.BlendDst)
		{
				case GL_ZERO:					 destfactor = W3D_ZERO; break;
				case GL_ONE:					  destfactor = W3D_ONE; break;
				case GL_SRC_COLOR:				destfactor = W3D_SRC_COLOR; break;
				case GL_ONE_MINUS_SRC_COLOR:	  destfactor = W3D_ONE_MINUS_SRC_COLOR; break;
				case GL_SRC_ALPHA:				destfactor = W3D_SRC_ALPHA; break;
				case GL_ONE_MINUS_SRC_ALPHA:	  destfactor = W3D_ONE_MINUS_SRC_ALPHA; break;
				case GL_DST_ALPHA:				destfactor = W3D_DST_ALPHA; break;
				case GL_ONE_MINUS_DST_ALPHA:	  destfactor = W3D_ONE_MINUS_DST_ALPHA; break;
				case GL_CONSTANT_COLOR:		   destfactor = W3D_CONSTANT_COLOR; break;
				case GL_ONE_MINUS_CONSTANT_COLOR: destfactor = W3D_ONE_MINUS_CONSTANT_COLOR; break;
				case GL_CONSTANT_ALPHA:		   destfactor = W3D_CONSTANT_ALPHA; break;
				case GL_ONE_MINUS_CONSTANT_ALPHA: destfactor = W3D_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		W3D_SetBlendMode(context,srcfactor,destfactor);
   }
   else
		W3D_SetState(context,W3D_BLENDING,W3D_DISABLE);

/* fogging check */

   if (ctx->Fog.Enabled)
   {
		W3D_SetState(context,W3D_FOGGING,W3D_ENABLE);
		HW_SetupFog(ctx);
   }
   else
		W3D_SetState(context,W3D_FOGGING,W3D_DISABLE);

/* antialiasing setup */

   if (ctx->Point.SmoothFlag)
		W3D_SetState(context,W3D_ANTI_POINT,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_ANTI_POINT,W3D_DISABLE);
   if (ctx->Line.SmoothFlag)
		W3D_SetState(context,W3D_ANTI_LINE,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_ANTI_LINE,W3D_DISABLE);
   if (ctx->Polygon.SmoothFlag)
		W3D_SetState(context,W3D_ANTI_POLYGON,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_ANTI_POLYGON,W3D_DISABLE);

/* dithering setup */

   if (!(ctx->NoDither) && (ctx->Color.DitherFlag))
		W3D_SetState(context,W3D_DITHERING,W3D_ENABLE);
   else
		W3D_SetState(context,W3D_DITHERING,W3D_DISABLE);

/* scissor setup */

   if (ctx->Scissor.Enabled)
   {
		hwcontext->scissor.left = ctx->Scissor.X;
		hwcontext->scissor.top = c->height-ctx->Scissor.Y-ctx->Scissor.Height;
		hwcontext->scissor.width = ctx->Scissor.Width;
		hwcontext->scissor.height = ctx->Scissor.Height;
		W3D_SetState(context,W3D_SCISSOR,W3D_ENABLE);
		W3D_SetScissor(context,&hwcontext->scissor);
   }
   else
   {
		hwcontext->scissor.left = 0;
		hwcontext->scissor.top = 0;
		hwcontext->scissor.width = c->width;
		hwcontext->scissor.height = c->height;
		W3D_SetState(context,W3D_SCISSOR,W3D_DISABLE);
		/* small workaround to fix W3DV2 Prototype problem */
		W3D_SetScissor(context,&hwcontext->scissor);
   }

/* logic op setup */

   if ((ctx->Color.IndexLogicOpEnabled) || (ctx->Color.ColorLogicOpEnabled))
   {
		ULONG operation;

		W3D_SetState(context,W3D_LOGICOP,W3D_ENABLE);
		switch (ctx->Color.LogicOp)
		{
				case GL_CLEAR:		  operation = W3D_LO_CLEAR; break;
				case GL_SET:			operation = W3D_LO_SET; break;
				case GL_COPY:		   operation = W3D_LO_COPY; break;
				case GL_COPY_INVERTED:  operation = W3D_LO_COPY_INVERTED; break;
				case GL_NOOP:		   operation = W3D_LO_NOOP; break;
				case GL_INVERT:		 operation = W3D_LO_INVERT; break;
				case GL_AND:			operation = W3D_LO_AND; break;
				case GL_NAND:		   operation = W3D_LO_NAND; break;
				case GL_OR:			 operation = W3D_LO_OR; break;
				case GL_NOR:			operation = W3D_LO_NOR; break;
				case GL_XOR:			operation = W3D_LO_XOR; break;
				case GL_EQUIV:		  operation = W3D_LO_EQUIV; break;
				case GL_AND_REVERSE:	operation = W3D_LO_AND_REVERSE; break;
				case GL_AND_INVERTED:   operation = W3D_LO_AND_INVERTED; break;
				case GL_OR_REVERSE:	 operation = W3D_LO_OR_REVERSE; break;
				case GL_OR_INVERTED:	operation = W3D_LO_OR_INVERTED; break;
		}
		W3D_SetLogicOp(context,operation);
   }
   else
		W3D_SetState(context,W3D_LOGICOP,W3D_DISABLE);

/* masking setup */

   if (ctx->Visual->RGBAflag)
   {
		W3D_Bool r,g,b,a;

		r = (ctx->Color.ColorMask[RCOMP] == 0xff);
		g = (ctx->Color.ColorMask[GCOMP] == 0xff);
		b = (ctx->Color.ColorMask[BCOMP] == 0xff);
		a = (ctx->Color.ColorMask[ACOMP] == 0xff);
		W3D_SetColorMask(context,r,g,b,a);
   }
   else
   {
		W3D_SetPenMask(context,(ULONG)ctx->Color.IndexMask);
   }

/* stencil setup */

/* TODO: enable this as soon as Mesa supports hardware accelerated
   stenciling

   if (ctx->Stencil.Enabled)
   {
		ULONG stfunc, stwmask, stref, stmask;
		ULONG stop_fail, stop_zfail, stop_zpass;

		W3D_SetState(context,W3D_STENCILBUFFER,W3D_ENABLE);
		stref = ctx->Stencil.Ref;
		stmask = ctx->Stencil.ValueMask;
		stwmask = ctx->Stencil.WriteMask;
		switch (ctx->Stencil.Function)
		{
				case GL_NEVER:		  stfunc = W3D_ST_NEVER; break;
				case GL_LESS:		   stfunc = W3D_ST_LESS; break;
				case GL_LEQUAL:		 stfunc = W3D_ST_LEQUAL; break;
				case GL_GREATER:		stfunc = W3D_ST_GREATER; break;
				case GL_GEQUAL:		 stfunc = W3D_ST_GEQUAL; break;
				case GL_EQUAL:		  stfunc = W3D_ST_EQUAL; break;
				case GL_NOTEQUAL:	   stfunc = W3D_ST_NOTEQUAL; break;
				case GL_ALWAYS:		 stfunc = W3D_ST_ALWAYS; break;
		}
		switch (ctx->Stencil.FailFunc)
		{
				case GL_KEEP:		   stop_fail = W3D_ST_KEEP; break;
				case GL_ZERO:		   stop_fail = W3D_ST_ZERO; break;
				case GL_REPLACE:		stop_fail = W3D_ST_REPLACE; break;
				case GL_INCR:		   stop_fail = W3D_ST_INCR; break;
				case GL_DECR:		   stop_fail = W3D_ST_DECR; break;
				case GL_INVERT:		 stop_fail = W3D_ST_INVERT; break;
		}
		switch (ctx->Stencil.ZFailFunc)
		{
				case GL_KEEP:		   stop_zfail = W3D_ST_KEEP; break;
				case GL_ZERO:		   stop_zfail = W3D_ST_ZERO; break;
				case GL_REPLACE:		stop_zfail = W3D_ST_REPLACE; break;
				case GL_INCR:		   stop_zfail = W3D_ST_INCR; break;
				case GL_DECR:		   stop_zfail = W3D_ST_DECR; break;
				case GL_INVERT:		 stop_zfail = W3D_ST_INVERT; break;
		}
		switch (ctx->Stencil.ZPassFunc)
		{
				case GL_KEEP:		   stop_zpass = W3D_ST_KEEP; break;
				case GL_ZERO:		   stop_zpass = W3D_ST_ZERO; break;
				case GL_REPLACE:		stop_zpass = W3D_ST_REPLACE; break;
				case GL_INCR:		   stop_zpass = W3D_ST_INCR; break;
				case GL_DECR:		   stop_zpass = W3D_ST_DECR; break;
				case GL_INVERT:		 stop_zpass = W3D_ST_INVERT; break;
		}
		W3D_SetStencilFunc(context,stfunc,stref,stmask);
		W3D_SetStencilOp(context,stop_fail,stop_zfail,stop_zpass);
		W3D_SetWriteMask(context,stwmask);
   }
   else
		W3D_SetState(context,W3D_STENCILBUFFER,W3D_DISABLE);
*/

/* TODO: support specular highlighting as soon as supported by W3D */

}
;;//

// rendering setup
;// HW_ChoosePoint_VeryFast
/*
 *
 * Choose the appropriate point rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
points_func HW_ChoosePoint_VeryFast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_POINT) == W3D_NOT_SUPPORTED)
		return NULL;
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawPoint_smoothTwo;
		return HW_DrawPoint_smooth;
   }
   else
		return HW_DrawPoint_flat;
}
;;//
;// HW_ChoosePoint_Fast
/*
 *
 * Choose the appropriate point rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
points_func HW_ChoosePoint_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_POINT) == W3D_NOT_SUPPORTED)
		return NULL;

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_POINT_FX) == W3D_NOT_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_POINT_TEX) == W3D_NOT_SUPPORTED)
				return NULL;
   }
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawPoint_smoothTwo;
		return HW_DrawPoint_smooth;
   }
   else
		return HW_DrawPoint_flat;
}
;;//
;// HW_ChoosePoint
/*
 *
 * Choose the appropriate point rendering function or returns NULL
 * High quality desired
 *
 */
points_func HW_ChoosePoint(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_POINT) != W3D_FULLY_SUPPORTED)
		return NULL;

   if (ctx->Point.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_POINT) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_POINT_FX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_POINT_TEX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if (!(ctx->Point.Params[0]==1.0 && ctx->Point.Params[1]==0.0 &&
		 ctx->Point.Params[2]==0.0))
		return NULL;

   if (ctx->Point.Size != 1.0f)
   {
		if (QUERY3D(W3D_Q_DRAW_POINT_X) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawPoint_smoothTwo;
		return HW_DrawPoint_smooth;
   }
   else
		return HW_DrawPoint_flat;
}
;;//
;// HW_ChooseLine_VeryFast
/*
 *
 * Choose the appropriate line rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
line_func HW_ChooseLine_VeryFast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) == W3D_NOT_SUPPORTED)
		return NULL;

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawLine_smoothTwo;
		return HW_DrawLine_smooth;
   }
   else
		return HW_DrawLine_flat;
}
;;//
;// HW_ChooseLine_Fast
/*
 *
 * Choose the appropriate line rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
line_func HW_ChooseLine_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) == W3D_NOT_SUPPORTED)
		return NULL;

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_FX) == W3D_NOT_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_TEX) == W3D_NOT_SUPPORTED)
				return NULL;
   }

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawLine_smoothTwo;
		return HW_DrawLine_smooth;
   }
   else
		return HW_DrawLine_flat;
}
;;//
;// HW_ChooseLine
/*
 *
 * Choose the appropriate line rendering function or returns NULL
 * High quality desired
 *
 */
line_func HW_ChooseLine(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) != W3D_FULLY_SUPPORTED)
		return NULL;

   if (ctx->Line.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_LINE) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_FX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_TEX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if (ctx->Line.Width != 1.0f)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_X) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Line.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawLine_smoothTwo;
		return HW_DrawLine_smooth;
   }
   else
		return HW_DrawLine_flat;
}
;;//
;// HW_ChooseTriangle_Fast
/*
 *
 * Choose the appropriate triangle rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
triangle_func HW_ChooseTriangle_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOT_SUPPORTED)
		return NULL;
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawTriangle_smoothTwo;
		return HW_DrawTriangle_smooth;
   }
   else
		return HW_DrawTriangle_flat;
}
;;//
;// HW_ChooseTriangle
/*
 *
 * Choose the appropriate triangle rendering function or returns NULL
 * High quality desired
 *
 */
triangle_func HW_ChooseTriangle(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) != W3D_FULLY_SUPPORTED)
		return NULL;
   if ((ctx->Polygon.OffsetAny) || (ctx->RasterMask & FRONT_AND_BACK_BIT))
		return NULL;

/* TODO: enable this, as soon as Mesa supports polygon antialiasing
   if (ctx->Polygon.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_POLYGON) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
*/

   if (ctx->Polygon.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_POLY_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawTriangle_smoothTwo;
		return HW_DrawTriangle_smooth;
   }
   else
		return HW_DrawTriangle_flat;
}
;;//
;// HW_ChooseQuad_Fast
/*
 *
 * Choose the appropriate quad rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
quad_func HW_ChooseQuad_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOT_SUPPORTED)
		return NULL;

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawQuad_smoothTwo;
		return HW_DrawQuad_smooth;
   }
   else
		return HW_DrawQuad_flat;
}
;;//
;// HW_ChooseQuad
/*
 *
 * Choose the appropriate quad rendering function or returns NULL
 * High quality desired
 *
 */
quad_func HW_ChooseQuad(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) != W3D_FULLY_SUPPORTED)
		return NULL;

   if ((ctx->Polygon.OffsetAny) || (ctx->RasterMask & FRONT_AND_BACK_BIT))
		return NULL;

/* TODO: enable this, as soon as Mesa supports polygon antialiasing
   if (ctx->Polygon.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_POLYGON) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
*/

   if (ctx->Polygon.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_POLY_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if(ctx->Light.Model.TwoSide)
				return HW_DrawQuad_smoothTwo;
		return HW_DrawQuad_smooth;
   }
   else
		return HW_DrawQuad_flat;
}
;;//
;// HW_ChooseLineStrip_VeryFast
/*
 *
 * Choose the appropriate line rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
linestrip_func HW_ChooseLineStrip_VeryFast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) == W3D_NOT_SUPPORTED)
		return NULL;

   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawLineStrip;
}
;;//
;// HW_ChooseLineStrip_Fast
/*
 *
 * Choose the appropriate linestrip rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
linestrip_func HW_ChooseLineStrip_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) == W3D_NOT_SUPPORTED)
		return NULL;

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_FX) == W3D_NOT_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_TEX) == W3D_NOT_SUPPORTED)
				return NULL;
   }
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawLineStrip;
}
;;//
;// HW_ChooseLineStrip
/*
 *
 * Choose the appropriate linestrip rendering function or returns NULL
 * High quality desired
 *
 */
linestrip_func HW_ChooseLineStrip(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (hwcontext->flags & HWFLAG_NOHWLINES)
		return NULL;

   if (QUERY3D(W3D_Q_DRAW_LINE) != W3D_FULLY_SUPPORTED)
		return NULL;

   if (ctx->Line.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_LINE) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if ((ctx->Color.AlphaEnabled) || (ctx->Color.BlendEnabled) ||
	   (ctx->Fog.Enabled) || (ctx->Color.IndexLogicOpEnabled) ||
	   (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_FX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Texture.Enabled)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_TEX) != W3D_FULLY_SUPPORTED)
				return NULL;
   }

   if (ctx->Line.Width != 1.0f)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_X) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if (ctx->Line.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_LINE_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawLineStrip;
}
;;//
;// HW_ChooseTriStrip_Fast
/*
 *
 * Choose the appropriate tristrip rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
tristrip_func HW_ChooseTriStrip_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOT_SUPPORTED)
		return NULL;
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawTriStrip;
}
;;//
;// HW_ChooseTriStrip
/*
 *
 * Choose the appropriate tristrip rendering function or returns NULL
 * High quality desired
 *
 */
tristrip_func HW_ChooseTriStrip(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) != W3D_FULLY_SUPPORTED)
		return NULL;
   if ((ctx->Polygon.OffsetAny) || (ctx->RasterMask & FRONT_AND_BACK_BIT))
		return NULL;

/* TODO: enable this, as soon as Mesa supports polygon antialiasing
   if (ctx->Polygon.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_POLYGON) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
*/

   if (ctx->Polygon.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_POLY_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawTriStrip;
}
;;//
;// HW_ChooseTriFan_Fast
/*
 *
 * Choose the appropriate trifan rendering function or returns NULL
 * Medium quality / higher speed desired
 *
 */
trifan_func HW_ChooseTriFan_Fast(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOT_SUPPORTED)
		return NULL;
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawTriFan;
}
;;//
;// HW_ChooseTriFan
/*
 *
 * Choose the appropriate trifan rendering function or returns NULL
 * High quality desired
 *
 */
trifan_func HW_ChooseTriFan(GLcontext *ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) != W3D_FULLY_SUPPORTED)
		return NULL;
   if ((ctx->Polygon.OffsetAny) || (ctx->RasterMask & FRONT_AND_BACK_BIT))
		return NULL;

/* TODO: enable this, as soon as Mesa supports polygon antialiasing
   if (ctx->Polygon.SmoothFlag)
   {
		if (QUERY3D(W3D_Q_ANTI_POLYGON) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
*/

   if (ctx->Polygon.StippleFlag)
   {
		if (QUERY3D(W3D_Q_DRAW_POLY_ST) != W3D_FULLY_SUPPORTED)
				return NULL;
   }
   if(ctx->Light.Model.TwoSide)
		return NULL;
   return HW_DrawTriFan;
}
;;//
;// HW_ChooseRasterSetup
/*
 *
 * Choose a raster setup function
 *
 */
typedef void(*rastersetupfunc)(GLcontext *,GLuint,GLuint);
rastersetupfunc HW_ChooseRasterSetup(GLcontext *ctx,W3Ddriver* hwcontext)
{
   if (ctx->Depth.Test)
   {
		if (ctx->Light.ShadeModel==GL_SMOOTH)
		{
				if (ctx->Texture.Enabled)
				{
						if(ctx->Light.Model.TwoSide)
						{
								hwcontext->CopyVertex = HW_CopyFlatTexZ;
								return (HW_SetupFlatTexZ);
						}
						hwcontext->CopyVertex = HW_CopyGouraudTexZ;
						return (HW_SetupGouraudTexZ);
				}
				else
				{
						if (ctx->Fog.Enabled)
						{
								if(ctx->Light.Model.TwoSide)
								{
										hwcontext->CopyVertex = HW_CopyFlatFogZ;
										return (HW_SetupFlatFogZ);
								}
								hwcontext->CopyVertex = HW_CopyGouraudFogZ;
								return (HW_SetupGouraudFogZ);
						}
						else
						{
								if(ctx->Light.Model.TwoSide)
								{
										hwcontext->CopyVertex = HW_CopyFlatZ;
										return (HW_SetupFlatZ);
								}
								hwcontext->CopyVertex = HW_CopyGouraudZ;
								return (HW_SetupGouraudZ);
						}
				}
		}
		else
		{
				if (ctx->Texture.Enabled)
				{
						hwcontext->CopyVertex = HW_CopyFlatTexZ;
						return (HW_SetupFlatTexZ);
				}
				else
				{
						if (ctx->Fog.Enabled)
						{
								hwcontext->CopyVertex = HW_CopyFlatFogZ;
								return (HW_SetupFlatFogZ);
						}
						else
						{
								hwcontext->CopyVertex = HW_CopyFlatZ;
								return (HW_SetupFlatZ);
						}
				}
		}
   }
   else
   {
		if (ctx->Light.ShadeModel==GL_SMOOTH)
		{
				if (ctx->Texture.Enabled)
				{
						if(ctx->Light.Model.TwoSide)
						{
								hwcontext->CopyVertex = HW_CopyFlatTex;
								return (HW_SetupFlatTex);
						}
						hwcontext->CopyVertex = HW_CopyGouraudTex;
						return (HW_SetupGouraudTex);
				}
				else
				{
						if (ctx->Fog.Enabled)
						{
								if(ctx->Light.Model.TwoSide)
								{
										hwcontext->CopyVertex = HW_CopyFlatFog;
										return (HW_SetupFlatFog);
								}
								hwcontext->CopyVertex = HW_CopyGouraudFog;
								return (HW_SetupGouraudFog);
						}
						else
						{
								if(ctx->Light.Model.TwoSide)
								{
										hwcontext->CopyVertex = HW_CopyFlat;
										return (HW_SetupFlat);
								}
								hwcontext->CopyVertex = HW_CopyGouraud;
								return (HW_SetupGouraud);
						}
				}
		}
		else
		{
				if (ctx->Texture.Enabled)
				{
						hwcontext->CopyVertex = HW_CopyFlatTex;
						return (HW_SetupFlatTex);
				}
				else
				{
						if (ctx->Fog.Enabled)
						{
								hwcontext->CopyVertex = HW_CopyFlatFog;
								return (HW_SetupFlatFog);
						}
						else
						{
								hwcontext->CopyVertex = HW_CopyFlat;
								return (HW_SetupFlat);
						}
				}
		}
   }
}
;;//
;// HW_ContextSupported_VeryFast
/*
 *
 * Compatibility check
 * Determines if the current context is supported by the 3D hardware
 * Low quality / highest speed desired
 *
 */
int HW_ContextSupported_VeryFast(GLcontext* ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (!(ctx->NewState & (~NEW_LIGHTING)))
		return CHECK_SUCCESS;

/* TODO: remove, as soon as flat shading works correctly */
   if (c->depth <= 8)
		return CHECK_NO8BIT;

/* color index mode currently not supported (would need some
   adaptions to the color conversion tables) */

   if (!(c->visual->rgb_flag))
		return CHECK_NOCI;

/* depth buffer allocation */

   if (ctx->Depth.Test && hwcontext->nohw)
		return CHECK_NOZMEM;

/* general check: HW ready? */

   if (!(hwcontext->flags & HWFLAG_ACTIVE))
		return CHECK_INACTIVE;

/* TODO: implement general check, as soon as available */

#if 0
   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOTHING_SUPPORTED)
		return CHECK_NOHW;
#endif
   if (ctx->Color.DrawBuffer==GL_NONE)
		return CHECK_NODRAWBUF;

/* texture mapping check */
/* TODO: support 3d textures */

   if (ctx->Texture.Enabled && ctx->Texture.Enabled != TEXTURE0_2D)
		return CHECK_TEXMODE;
   if (ctx->Texture.Enabled)
   {
		struct gl_texture_object *ct = ctx->Texture.Set[0].Current2D;
		GLenum envmode = ctx->Texture.Set[0].EnvMode;

		if (hwcontext->flags & HWFLAG_NOVMEM)
				return CHECK_NOVMEM;
		if (!ct->Image[0])
				return CHECK_NOIMAGE;
		if (QUERY3D(W3D_Q_TEXMAPPING) == W3D_NOT_SUPPORTED)
				return CHECK_NOTEXMAPPING;
		if (ctx->Hint.PerspectiveCorrection==GL_FASTEST)
		{
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH))
				return CHECK_TEXOV_LIN;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT))
				return CHECK_TEXOV_LIN;
		}
		else
		{
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH_P))
				return CHECK_TEXOV_P;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT_P))
				return CHECK_TEXOV_P;
		}
		if ((ct->Image[0]->Width != ct->Image[0]->Height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
				return CHECK_NORECTTEX;
		if (!hwcontext->currenttex)
				return CHECK_NOTEXOBJ;
		else
		{
				struct gl_texture_image* ti;
				struct gl_texture_object* to = hwcontext->currenttex;
				ULONG w3dformat = 0;
				ULONG destformat = 0;
				GLenum format;
				int i;

				if (!(ti = to->Image[0]))
						return CHECK_NOTEXIMAGE;
				switch (format = ti->Format)
				{
						case GL_ALPHA:
								w3dformat = W3D_A8;
								break;
						case GL_LUMINANCE:
								w3dformat = W3D_L8;
								break;
						case GL_LUMINANCE_ALPHA:
								w3dformat = W3D_L8A8;
								break;
						case GL_INTENSITY:
								w3dformat = W3D_I8;
								break;
						case GL_COLOR_INDEX:
								return CHECK_FORMATERR;
								break;
						case GL_RGB:
								w3dformat = W3D_R8G8B8;
								break;
						case GL_RGBA:
								w3dformat = W3D_A8R8G8B8;
								break;
				}
				if (c->depth <= 8)
						destformat = W3D_CHUNKY;
				if (W3D_GetTexFmtInfo(context,w3dformat,destformat) & W3D_TEXFMT_UNSUPPORTED)
						return CHECK_FORMATERR;

				for (i=0;i<MAX_TEXTURE_LEVELS;i++)
				{
						/* all mipmap levels must have the same format */
						if (to->Image[i] && (to->Image[i]->Format != format))
								return CHECK_FORMATERR2;
				}
				if (!(((W3Dtexobj *)(to->DriverData))->texture))
						return CHECK_NOW3DTEX; /* no W3D texture object exists */
		}
   }

/* gouraud / flat shading check */

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if (QUERY3D(W3D_Q_GOURAUDSHADING) == W3D_NOT_SUPPORTED)
				return CHECK_NOSMOOTH;
   }
   else
   {
		if (QUERY3D(W3D_Q_FLATSHADING) == W3D_NOT_SUPPORTED)
				return CHECK_NOFLAT;
   }

/* depth buffer check */

   if (ctx->Depth.Test)
   {
		if (QUERY3D(W3D_Q_ZBUFFER) == W3D_NOT_SUPPORTED)
				return CHECK_NOZBUFFER;
		if (ctx->Depth.Mask)
		{
				if (QUERY3D(W3D_Q_ZBUFFERUPDATE) == W3D_NOT_SUPPORTED)
						return CHECK_NOZUPDATE;
		}
   }



/* alpha test check */

/* blending check */

/* fogging check */

/* antialiasing check */

/* dithering check */

/* scissor check */

/* logic op check */

/* masking check */

/* stencil check */

   return CHECK_SUCCESS;
}
;;//
;// HW_ContextSupported_Fast
/*
 *
 * Compatibility check
 * Determines if the current context is supported by the 3D hardware
 * Medium quality / higher speed desired
 *
 */
int HW_ContextSupported_Fast(GLcontext* ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (!(ctx->NewState & (~NEW_LIGHTING)))
		return CHECK_SUCCESS;

/* TODO: remove, as soon as flat shading works correctly */
   if (c->depth <= 8)
		return CHECK_NO8BIT;

/* color index mode currently not supported (would need some
   adaptions to the color conversion tables) */

   if (!(c->visual->rgb_flag))
		return CHECK_NOCI;

/* depth buffer allocation */

   if (ctx->Depth.Test && hwcontext->nohw)
		return CHECK_NOZMEM;

/* general check: HW ready? */

   if (!(hwcontext->flags & HWFLAG_ACTIVE))
		return CHECK_INACTIVE;

/* TODO: implement general check, as soon as available */
#if 0
   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOTHING_SUPPORTED)
		return CHECK_NOHW;
#endif
   if (ctx->Color.DrawBuffer==GL_NONE)
		return CHECK_NODRAWBUF;

/* texture mapping check */
/* TODO: support 3d textures */

   if (ctx->Texture.Enabled && ctx->Texture.Enabled != TEXTURE0_2D)
		return CHECK_TEXMODE;
   if (ctx->Texture.Enabled)
   {
		struct gl_texture_object *ct = ctx->Texture.Set[0].Current2D;
		GLenum envmode = ctx->Texture.Set[0].EnvMode;

		if (hwcontext->flags & HWFLAG_NOVMEM)
				return CHECK_NOVMEM;
		if (!ct->Image[0])
				return CHECK_NOIMAGE;
		if (QUERY3D(W3D_Q_TEXMAPPING) == W3D_NOT_SUPPORTED)
				return CHECK_NOTEXMAPPING;
		if (ctx->Hint.PerspectiveCorrection==GL_FASTEST)
		{
			 if (((ct->WrapS==GL_REPEAT) || (ct->WrapT==GL_REPEAT)) &&
						(QUERY3D(W3D_Q_LINEAR_REPEAT) == W3D_NOT_SUPPORTED))
				return CHECK_NOREPEAT_LIN;
			 if (!(hwcontext->flags & HWFLAG_NOCLAMP))
			 {
			   if (((ct->WrapS!=GL_REPEAT) || (ct->WrapT!=GL_REPEAT)) &&
						  (QUERY3D(W3D_Q_LINEAR_CLAMP) != W3D_FULLY_SUPPORTED))
				  return CHECK_NOCLAMP_LIN;
			 }
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH))
				return CHECK_TEXOV_LIN;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT))
				return CHECK_TEXOV_LIN;
		}
		else
		{
			 if (QUERY3D(W3D_Q_PERSPECTIVE) == W3D_NOT_SUPPORTED)
				return CHECK_NOPERSP;
			 if (((ct->WrapS==GL_REPEAT) || (ct->WrapT==GL_REPEAT)) &&
						(QUERY3D(W3D_Q_PERSP_REPEAT) == W3D_NOT_SUPPORTED))
				return CHECK_NOREPEAT_P;
			 if (!(hwcontext->flags & HWFLAG_NOCLAMP))
			 {
			   if (((ct->WrapS!=GL_REPEAT) || (ct->WrapT!=GL_REPEAT)) &&
						  (QUERY3D(W3D_Q_PERSP_CLAMP) != W3D_FULLY_SUPPORTED))
				  return CHECK_NOCLAMP_P;
			 }
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH_P))
				return CHECK_TEXOV_P;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT_P))
				return CHECK_TEXOV_P;
		}
		if (((ct->MinFilter == GL_LINEAR) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_NEAREST) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_LINEAR) ||
		   (ct->MagFilter == GL_LINEAR)) &&
		   (QUERY3D(W3D_Q_BILINEARFILTER) == W3D_NOT_SUPPORTED))
				return CHECK_NOBILINEAR;
		if ((envmode == GL_REPLACE) &&
		   (QUERY3D(W3D_Q_ENV_REPLACE) == W3D_NOT_SUPPORTED))
				return CHECK_NOREPLACE;
		if ((envmode == GL_DECAL) &&
		   (QUERY3D(W3D_Q_ENV_DECAL) == W3D_NOT_SUPPORTED))
				return CHECK_NODECAL;
		if ((envmode == GL_MODULATE) &&
		   (QUERY3D(W3D_Q_ENV_MODULATE) == W3D_NOT_SUPPORTED))
				return CHECK_NOMODULATE;
		if ((envmode == GL_BLEND) &&
		   (QUERY3D(W3D_Q_ENV_BLEND) == W3D_NOT_SUPPORTED))
				return CHECK_NOBLEND;
		if ((ct->Image[0]->Width != ct->Image[0]->Height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
				return CHECK_NORECTTEX;
		if (!hwcontext->currenttex)
				return CHECK_NOTEXOBJ;
		else
		{
				struct gl_texture_image* ti;
				struct gl_texture_object* to = hwcontext->currenttex;
				ULONG w3dformat = 0;
				ULONG destformat = 0;
				GLenum format;
				int i;

				if (!(ti = to->Image[0]))
						return CHECK_NOTEXIMAGE;
				switch (format = ti->Format)
				{
						case GL_ALPHA:
								w3dformat = W3D_A8;
								break;
						case GL_LUMINANCE:
								w3dformat = W3D_L8;
								break;
						case GL_LUMINANCE_ALPHA:
								w3dformat = W3D_L8A8;
								break;
						case GL_INTENSITY:
								w3dformat = W3D_I8;
								break;
						case GL_COLOR_INDEX:
								return CHECK_FORMATERR;
								break;
						case GL_RGB:
								w3dformat = W3D_R8G8B8;
								break;
						case GL_RGBA:
								w3dformat = W3D_A8R8G8B8;
								break;
				}
				if (c->depth <= 8)
						destformat = W3D_CHUNKY;
				if (W3D_GetTexFmtInfo(context,w3dformat,destformat) & W3D_TEXFMT_UNSUPPORTED)
						return CHECK_FORMATERR;

				for (i=0;i<MAX_TEXTURE_LEVELS;i++)
				{
						/* all mipmap levels must have the same format */
						if (to->Image[i] && (to->Image[i]->Format != format))
								return CHECK_FORMATERR2;
				}
				if (!(((W3Dtexobj *)(to->DriverData))->texture))
						return CHECK_NOW3DTEX; /* no W3D texture object exists */
		}
   }

/* gouraud / flat shading check */

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if (QUERY3D(W3D_Q_GOURAUDSHADING) == W3D_NOT_SUPPORTED)
				return CHECK_NOSMOOTH;
   }
   else
   {
		if (QUERY3D(W3D_Q_FLATSHADING) == W3D_NOT_SUPPORTED)
				return CHECK_NOFLAT;
   }

/* depth buffer check */

   if (ctx->Depth.Test)
   {
		if (QUERY3D(W3D_Q_ZBUFFER) == W3D_NOT_SUPPORTED)
				return CHECK_NOZBUFFER;
		if (ctx->Depth.Mask)
		{
				if (QUERY3D(W3D_Q_ZBUFFERUPDATE) == W3D_NOT_SUPPORTED)
						return CHECK_NOZUPDATE;
		}
   }


/* alpha test check */

   if (ctx->Color.AlphaEnabled)
   {
		if (QUERY3D(W3D_Q_ALPHATEST) == W3D_NOT_SUPPORTED)
				return CHECK_NOALPHATEST;
   }

/* blending check */

   if (ctx->Color.BlendEnabled)
   {
		ULONG srcfactor, destfactor;

		if (QUERY3D(W3D_Q_BLENDING) == W3D_NOT_SUPPORTED)
				return CHECK_NOBLENDING;
		if (QUERY3D(W3D_Q_BLEND_DECAL_FOG) != W3D_FULLY_SUPPORTED)
		{
				GLenum envmode = ctx->Texture.Set[0].EnvMode;
				if (ctx->Texture.Enabled && ctx->Fog.Enabled && (envmode == GL_DECAL))
						return CHECK_DECAL_BLEND;
		}
   }

/* fogging check */

   if (ctx->Fog.Enabled)
   {
		if (QUERY3D(W3D_Q_FOGGING) == W3D_NOT_SUPPORTED)
				return CHECK_NOFOGGING;
   }

/* antialiasing check */

/* dithering check */

/* scissor check */

   if (ctx->Scissor.Enabled)
   {
		if (QUERY3D(W3D_Q_SCISSOR) == W3D_NOT_SUPPORTED)
				return CHECK_NOSCISSOR;
   }

/* logic op check */

   if ((ctx->Color.IndexLogicOpEnabled) || (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_LOGICOP) == W3D_NOT_SUPPORTED)
				return CHECK_NOLOGICOP;
   }

/* masking check */

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
		(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
	   ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
		if (QUERY3D(W3D_Q_MASKING) == W3D_NOT_SUPPORTED)
				return CHECK_NOMASKING;
   }

/* stencil check */


   if (ctx->Stencil.Enabled)
   {

/* TO DO: change this as soon as Mesa supports hardware accelerated
   stenciling */

#if 0
		if (QUERY3D(W3D_Q_STENCILBUFFER) == W3D_NOT_SUPPORTED)
#endif
				return CHECK_NOSTENCIL;

   }

/* TODO: support specular highlighting completely */

   if (ctx->Light.Model.ColorControl == GL_SEPARATE_SPECULAR_COLOR)
		return CHECK_NOSPECULAR;

   return CHECK_SUCCESS;
}
;;//
;// HW_ContextSupported
/*
 *
 * Compatibility check
 * Determines if the current context is supported by the 3D hardware
 * High quality desired
 *
 */
int HW_ContextSupported(GLcontext* ctx)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;

   if (!(ctx->NewState & (~NEW_LIGHTING)))
		return CHECK_SUCCESS;

/* TODO: remove, as soon as flat shading works correctly */
   if (c->depth <= 8)
		return CHECK_NO8BIT;

/* color index mode currently not supported (would need some
   adaptions to the color conversion tables) */

   if (!(c->visual->rgb_flag))
		return CHECK_NOCI;

/* depth buffer allocation */

   if (ctx->Depth.Test && hwcontext->nohw)
		return CHECK_NOZMEM;

/* general check: HW ready? */

   if (!(hwcontext->flags & HWFLAG_ACTIVE))
		return CHECK_INACTIVE;

/* TODO: implement general check, as soon as available */
#if 0
   if (QUERY3D(W3D_Q_DRAW_TRIANGLE) == W3D_NOTHING_SUPPORTED)
		return CHECK_NOHW;
#endif
   if (ctx->Color.DrawBuffer==GL_NONE)
		return CHECK_NODRAWBUF;

/* texture mapping check */

/* TODO: support 3d textures */
   if (ctx->Texture.Enabled && ctx->Texture.Enabled != TEXTURE0_2D)
		return CHECK_TEXMODE;
   if (ctx->Texture.Enabled)
   {
		struct gl_texture_object *ct = ctx->Texture.Set[0].Current2D;
		GLenum envmode = ctx->Texture.Set[0].EnvMode;

		if (hwcontext->flags & HWFLAG_NOVMEM)
				return CHECK_NOVMEM;
		if (!ct->Image[0])
				return CHECK_NOIMAGE;
		if (QUERY3D(W3D_Q_TEXMAPPING) != W3D_FULLY_SUPPORTED)
				return CHECK_NOTEXMAPPING;
		if (ctx->Hint.PerspectiveCorrection==GL_FASTEST)
		{
			 if (((ct->WrapS==GL_REPEAT) || (ct->WrapT==GL_REPEAT)) &&
						(QUERY3D(W3D_Q_LINEAR_REPEAT) != W3D_FULLY_SUPPORTED))
				return CHECK_NOREPEAT_LIN;
			 if (!(hwcontext->flags & HWFLAG_NOCLAMP))
			 {
			   if (((ct->WrapS!=GL_REPEAT) || (ct->WrapT!=GL_REPEAT)) &&
						  (QUERY3D(W3D_Q_LINEAR_CLAMP) != W3D_FULLY_SUPPORTED))
				  return CHECK_NOCLAMP_LIN;
			 }
			 if ((ct->WrapS != ct->WrapT) &&
				(QUERY3D(W3D_Q_WRAP_ASYM) != W3D_FULLY_SUPPORTED))
						return CHECK_NOASYM;
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH))
				return CHECK_TEXOV_LIN;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT))
				return CHECK_TEXOV_LIN;
		}
		else
		{
			 if (QUERY3D(W3D_Q_PERSPECTIVE) != W3D_FULLY_SUPPORTED)
				return CHECK_NOPERSP;
			 if (((ct->WrapS==GL_REPEAT) || (ct->WrapT==GL_REPEAT)) &&
						(QUERY3D(W3D_Q_PERSP_REPEAT) != W3D_FULLY_SUPPORTED))
				return CHECK_NOREPEAT_P;
			 if (!(hwcontext->flags & HWFLAG_NOCLAMP))
			 {
			   if (((ct->WrapS!=GL_REPEAT) || (ct->WrapT!=GL_REPEAT)) &&
						  (QUERY3D(W3D_Q_PERSP_CLAMP) != W3D_FULLY_SUPPORTED))
				  return CHECK_NOCLAMP_P;
			 }
			 if ((ct->WrapS != ct->WrapT) &&
				(QUERY3D(W3D_Q_WRAP_ASYM) != W3D_FULLY_SUPPORTED))
						return CHECK_NOASYM;
			 if (ct->Image[0]->Width > QUERY3D(W3D_Q_MAXTEXWIDTH_P))
				return CHECK_TEXOV_P;
			 if (ct->Image[0]->Height > QUERY3D(W3D_Q_MAXTEXHEIGHT_P))
				return CHECK_TEXOV_P;
		}
		if (((ct->MinFilter == GL_LINEAR) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_NEAREST) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_LINEAR) ||
		   (ct->MagFilter == GL_LINEAR)) &&
		   (QUERY3D(W3D_Q_BILINEARFILTER) != W3D_FULLY_SUPPORTED))
				return CHECK_NOBILINEAR;
		if (((ct->MinFilter == GL_NEAREST_MIPMAP_NEAREST) ||
		   (ct->MinFilter == GL_NEAREST_MIPMAP_LINEAR) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_NEAREST) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_LINEAR)) &&
		   (QUERY3D(W3D_Q_MIPMAPPING) != W3D_FULLY_SUPPORTED))
				return CHECK_NOMIPMAP;
		if (((ct->MinFilter == GL_NEAREST_MIPMAP_LINEAR) ||
		   (ct->MinFilter == GL_LINEAR_MIPMAP_LINEAR)) &&
		   (QUERY3D(W3D_Q_MMFILTER) != W3D_FULLY_SUPPORTED))
				return CHECK_NODEPTHFILTER;
		if ((envmode == GL_REPLACE) &&
		   (QUERY3D(W3D_Q_ENV_REPLACE) != W3D_FULLY_SUPPORTED))
				return CHECK_NOREPLACE;
		if ((envmode == GL_DECAL) &&
		   (QUERY3D(W3D_Q_ENV_DECAL) != W3D_FULLY_SUPPORTED))
				return CHECK_NODECAL;
		if ((envmode == GL_MODULATE) &&
		   (QUERY3D(W3D_Q_ENV_MODULATE) != W3D_FULLY_SUPPORTED))
				return CHECK_NOMODULATE;
		if ((envmode == GL_BLEND) &&
		   (QUERY3D(W3D_Q_ENV_BLEND) != W3D_FULLY_SUPPORTED))
				return CHECK_NOBLEND;
		if ((ct->Image[0]->Width != ct->Image[0]->Height) && (!(hwcontext->flags & HWFLAG_RECTTEX)))
				return CHECK_NORECTTEX;

		if (!hwcontext->currenttex)
				return CHECK_NOTEXOBJ;
		else
		{
				struct gl_texture_image* ti;
				struct gl_texture_object* to = hwcontext->currenttex;
				ULONG w3dformat = 0;
				ULONG destformat = 0;
				GLenum format;
				int i;

				if (!(ti = to->Image[0]))
						return CHECK_NOTEXIMAGE;
				switch (format = ti->Format)
				{
						case GL_ALPHA:
								w3dformat = W3D_A8;
								break;
						case GL_LUMINANCE:
								w3dformat = W3D_L8;
								break;
						case GL_LUMINANCE_ALPHA:
								w3dformat = W3D_L8A8;
								break;
						case GL_INTENSITY:
								w3dformat = W3D_I8;
								break;
						case GL_COLOR_INDEX:
								return CHECK_FORMATERR;
								break;
						case GL_RGB:
								w3dformat = W3D_R8G8B8;
								break;
						case GL_RGBA:
								w3dformat = W3D_A8R8G8B8;
								break;
				}
				if (c->depth <= 8)
						destformat = W3D_CHUNKY;
				if (W3D_GetTexFmtInfo(context,w3dformat,destformat) & W3D_TEXFMT_UNSUPPORTED)
						return CHECK_FORMATERR;

				for (i=0;i<MAX_TEXTURE_LEVELS;i++)
				{
						/* all mipmap levels must have the same format */
						if (to->Image[i] && (to->Image[i]->Format != format))
								return CHECK_FORMATERR2;
				}
				if (!(((W3Dtexobj *)(to->DriverData))->texture))
						return CHECK_NOW3DTEX; /* no W3D texture object exists */
		}
   }

/* gouraud / flat shading check */

   if (ctx->Light.ShadeModel==GL_SMOOTH)
   {
		if (QUERY3D(W3D_Q_GOURAUDSHADING) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSMOOTH;
   }
   else
   {
		if (QUERY3D(W3D_Q_FLATSHADING) != W3D_FULLY_SUPPORTED)
				return CHECK_NOFLAT;
   }

/* depth buffer check */

   if (ctx->Depth.Test)
   {
		if (QUERY3D(W3D_Q_ZBUFFER) != W3D_FULLY_SUPPORTED)
				return CHECK_NOZBUFFER;
		if (QUERY3D(W3D_Q_ZCOMPAREMODES) != W3D_FULLY_SUPPORTED)
				return CHECK_NOZMODES;
		if (ctx->Depth.Mask)
		{
				if (QUERY3D(W3D_Q_ZBUFFERUPDATE) != W3D_FULLY_SUPPORTED)
						return CHECK_NOZUPDATE;
		}
   }

/* alpha test check */

   if (ctx->Color.AlphaEnabled)
   {
		if (QUERY3D(W3D_Q_ALPHATEST) != W3D_FULLY_SUPPORTED)
				return CHECK_NOALPHATEST;
		if (QUERY3D(W3D_Q_ALPHATESTMODES) != W3D_FULLY_SUPPORTED)
				return CHECK_NOAMODES;
   }

/* blending check */

   if (ctx->Color.BlendEnabled)
   {
		ULONG srcfactor, destfactor;

		if (QUERY3D(W3D_Q_BLENDING) == W3D_NOT_SUPPORTED)
				return CHECK_NOBLENDING;
		switch (ctx->Color.BlendSrc)
		{
				case GL_ZERO:					 srcfactor = W3D_ZERO; break;
				case GL_ONE:					  srcfactor = W3D_ONE; break;
				case GL_DST_COLOR:				srcfactor = W3D_DST_COLOR; break;
				case GL_ONE_MINUS_DST_COLOR:	  srcfactor = W3D_ONE_MINUS_DST_COLOR; break;
				case GL_SRC_ALPHA:				srcfactor = W3D_SRC_ALPHA; break;
				case GL_ONE_MINUS_SRC_ALPHA:	  srcfactor = W3D_ONE_MINUS_SRC_ALPHA; break;
				case GL_DST_ALPHA:				srcfactor = W3D_DST_ALPHA; break;
				case GL_ONE_MINUS_DST_ALPHA:	  srcfactor = W3D_ONE_MINUS_DST_ALPHA; break;
				case GL_SRC_ALPHA_SATURATE:	   srcfactor = W3D_SRC_ALPHA_SATURATE; break;
				case GL_CONSTANT_COLOR:		   srcfactor = W3D_CONSTANT_COLOR; break;
				case GL_ONE_MINUS_CONSTANT_COLOR: srcfactor = W3D_ONE_MINUS_CONSTANT_COLOR; break;
				case GL_CONSTANT_ALPHA:		   srcfactor = W3D_CONSTANT_ALPHA; break;
				case GL_ONE_MINUS_CONSTANT_ALPHA: srcfactor = W3D_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		switch (ctx->Color.BlendDst)
		{
				case GL_ZERO:					 destfactor = W3D_ZERO; break;
				case GL_ONE:					  destfactor = W3D_ONE; break;
				case GL_SRC_COLOR:				destfactor = W3D_SRC_COLOR; break;
				case GL_ONE_MINUS_SRC_COLOR:	  destfactor = W3D_ONE_MINUS_SRC_COLOR; break;
				case GL_SRC_ALPHA:				destfactor = W3D_SRC_ALPHA; break;
				case GL_ONE_MINUS_SRC_ALPHA:	  destfactor = W3D_ONE_MINUS_SRC_ALPHA; break;
				case GL_DST_ALPHA:				destfactor = W3D_DST_ALPHA; break;
				case GL_ONE_MINUS_DST_ALPHA:	  destfactor = W3D_ONE_MINUS_DST_ALPHA; break;
				case GL_CONSTANT_COLOR:		   destfactor = W3D_CONSTANT_COLOR; break;
				case GL_ONE_MINUS_CONSTANT_COLOR: destfactor = W3D_ONE_MINUS_CONSTANT_COLOR; break;
				case GL_CONSTANT_ALPHA:		   destfactor = W3D_CONSTANT_ALPHA; break;
				case GL_ONE_MINUS_CONSTANT_ALPHA: destfactor = W3D_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		if (W3D_SetBlendMode(context,srcfactor,destfactor) != W3D_SUCCESS)
				return CHECK_NOBLENDFACTORS;
		if (QUERY3D(W3D_Q_BLEND_DECAL_FOG) != W3D_FULLY_SUPPORTED)
		{
				GLenum envmode = ctx->Texture.Set[0].EnvMode;
				if (ctx->Texture.Enabled && ctx->Fog.Enabled && (envmode == GL_DECAL))
						return CHECK_DECAL_BLEND;
		}
   }

/* fogging check */

   if (ctx->Fog.Enabled)
   {
		if (QUERY3D(W3D_Q_FOGGING) != W3D_FULLY_SUPPORTED)
				return CHECK_NOFOGGING;
		switch (ctx->Fog.Mode)
		{
				case GL_LINEAR:
						if (QUERY3D(W3D_Q_LINEAR) != W3D_FULLY_SUPPORTED)
						{
							if (hwcontext->flags & HWFLAG_NICEFOG)
								return CHECK_FOGLINEAR;
							else
							{
								if (QUERY3D(W3D_Q_INTERPOLATED) != W3D_FULLY_SUPPORTED)
										return CHECK_FOGLINEAR;
							}
						}
						break;
				case GL_EXP:
						if (QUERY3D(W3D_Q_EXPONENTIAL) != W3D_FULLY_SUPPORTED)
								return CHECK_FOGEXP;
						break;
				case GL_EXP2:
						if (QUERY3D(W3D_Q_S_EXPONENTIAL) != W3D_FULLY_SUPPORTED)
								return CHECK_FOGEXP2;
						break;
		}
   }

/* scissor check */

   if (ctx->Scissor.Enabled)
   {
		if (QUERY3D(W3D_Q_SCISSOR) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSCISSOR;
   }

/* logic op check */

   if ((ctx->Color.IndexLogicOpEnabled) || (ctx->Color.ColorLogicOpEnabled))
   {
		if (QUERY3D(W3D_Q_LOGICOP) != W3D_FULLY_SUPPORTED)
				return CHECK_NOLOGICOP;
   }

/* masking check */

   if (!(((*(GLuint *)ctx->Color.ColorMask == 0xffffffff) ||
		(*(GLuint *)ctx->Color.ColorMask == 0xffffff00)) &&
	   ((ctx->Color.IndexMask & 0xff) == 0xff)))
   {
		if (QUERY3D(W3D_Q_MASKING) != W3D_FULLY_SUPPORTED)
				return CHECK_NOMASKING;
   }

/* stencil check */

   if (ctx->Stencil.Enabled)
   {

/* TODO: change this as soon as Mesa supports hardware accelerated
   stenciling */

#if 0
		if (QUERY3D(W3D_Q_STENCILBUFFER) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCIL;
		if (QUERY3D(W3D_Q_STENCIL_FUNC) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCILFUNC;
		if (QUERY3D(W3D_Q_STENCIL_MASK) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCILMASK;
		if (QUERY3D(W3D_Q_STENCIL_SFAIL) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCILOP1;
		if (QUERY3D(W3D_Q_STENCIL_DPFAIL) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCILOP2;
		if (QUERY3D(W3D_Q_STENCIL_DPPASS) != W3D_FULLY_SUPPORTED)
				return CHECK_NOSTENCILOP3;
		if (ctx->Stencil.WriteMask != ((1<<STENCIL_BITS)-1))
		{
				if (QUERY3D(W3D_Q_STENCIL_WRMASK) != W3D_FULLY_SUPPORTED)
						return CHECK_NOSTENCILWMASK;
		}
#else
		return CHECK_NOSTENCIL;
#endif
   }

/* TODO: support specular highlighting completely */

   if (ctx->Light.Model.ColorControl == GL_SEPARATE_SPECULAR_COLOR)
		return CHECK_NOSPECULAR;


/* UFF! */

   return CHECK_SUCCESS;
}
;;//
;// HW_DD_Pointers
/*
 *
 * Driver functions setup
 *
 */


void HW_DD_pointers( GLcontext *ctx )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   int rc;
   BOOL ChangePrimitiveFuncs;

/* Thellier: I suppose hardware use allways the same functions (?) whatever the state */
/* So if the functions are setted dont set again them for each draw */
	ChangePrimitiveFuncs=(ctx->Driver.TriangleFunc==NULL);		/* Allways change if functions not setted yet */
	if (StormMesa.ChangePrimitiveFuncs.ON)				/* Change each time if StormMesa-Prefs want that*/
		ChangePrimitiveFuncs=TRUE;

#ifdef DBG
   /*LibPrintf*/ printf("HW_DD_pointers\n");
#endif
	REM(HW_DD_pointers);

   /* if only lighting state changes, then it shouldn't have any
	  effect on the driver state */
	rc = ((*hwcontext->CompCheck)(ctx));
   if (rc == CHECK_SUCCESS)
   {
	if(ChangePrimitiveFuncs)
	{
		ctx->Driver.PointsFunc = (*hwcontext->ChPoint)(ctx);
		ctx->Driver.LineFunc = (*hwcontext->ChLine)(ctx);
		ctx->Driver.TriangleFunc = (*hwcontext->ChTriangle)(ctx);
		ctx->Driver.QuadFunc = (*hwcontext->ChQuad)(ctx);
		ctx->Driver.LineStripFunc = (*hwcontext->ChLineStrip)(ctx);
		ctx->Driver.TriStripFunc = (*hwcontext->ChTriStrip)(ctx);
		ctx->Driver.TriFanFunc = (*hwcontext->ChTriFan)(ctx);
	}
		ctx->Driver.GetParameteri = HW_GetParameteri;
		ctx->Driver.RasterSetup = HW_ChooseRasterSetup(ctx,hwcontext);
		if (ctx->NewState & (~NEW_LIGHTING))
				HW_SetupDriver(ctx);
		hwcontext->height = (W3D_Float)c->height;
   }
   else
   {
		ctx->Driver.GetParameteri = SW_GetParameteri;
		ctx->Driver.RasterSetup = NULL;
   }
   if (!(hwcontext->nohw))
   {
	   ctx->Driver.AllocDepthBuffer = HW_AllocDepthBuffer;
	   ctx->Driver.DepthTestSpan = HW_DepthTestSpan;
	   ctx->Driver.DepthTestPixels = HW_DepthTestPixels;
	   ctx->Driver.ReadDepthSpanFloat = HW_ReadDepthSpanFloat;
	   ctx->Driver.ReadDepthSpanInt = HW_ReadDepthSpanInt;
   }
   else
   {
	   if (ctx->Buffer)
	   {
		   if (!ctx->Buffer->Depth)
		   {
				gl_alloc_depth_buffer(ctx);
				gl_clear_depth_buffer(ctx);
		   }
	   }
   }
   if (hwcontext->flags & HWFLAG_STATS)
   {
		if (rc > 0)
		{
				(hwcontext->statarray[rc-1])++;
				hwcontext->failed++;
		}
   }
   W3D_Flush(hwcontext->context);
}
;;//

// general setup / dispose
;// HWDriver_Lock2
/*
 *
 * Enter locked state
 * case: partial locking
 *
 */
void HWDriver_Lock2(GLcontext* ctx)
{
printf("HWDriver_Lock2\n");
		AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;

		if (hwcontext->lockmode != 1)
				return;
		if (!(hwcontext->flags & HWFLAG_LOCKED))
		{
				W3D_LockHardware(context);
				hwcontext->flags |= HWFLAG_LOCKED;
		}
printf("HWDriver_Lock2 exit\n");
}
;;//
;// HWDriver_UnLock2
/*
 *
 * Enter unlocked state
 * case: partial locking
 *
 */
void HWDriver_UnLock2(GLcontext* ctx)
{
printf("HWDriver_UnLock2\n");
		AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;

		if (hwcontext->lockmode != 1)
				return;
		if (hwcontext->flags & HWFLAG_LOCKED)
		{
				W3D_UnLockHardware(context);
				hwcontext->flags &= (~HWFLAG_LOCKED);
		}
printf("HWDriver_UnLock2 exit\n");
}
;;//
;// HWDriver_Lock3
/*
 *
 * Enter locked state
 * case: automatic full locking
 *
 */
void HWDriver_Lock3(struct amigamesa_context *c)
{
printf("HWDriver_Lock3\n");
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;

		if (hwcontext->lockmode < 2)
				return;
		if (!(hwcontext->flags & HWFLAG_LOCKED))
		{
				if (hwcontext->lockmode == 3)
				{
#ifdef __PPC__
						GetSysTimePPC(&tval1);
#else
						GetSysTime(&tval1);
#endif
				}
//				W3D_SetState(context,W3D_INDIRECT,W3D_DISABLE);
//				W3D_SetState(context,W3D_FAST,W3D_DISABLE);
printf("HWDriver_Lock3 locking hardware\n");
				//W3D_LockHardware(context);
printf("HWDriver_Lock3 locked hardware\n");
				//hwcontext->flags |= HWFLAG_LOCKED;
		}
printf("HWDriver_Lock3 exit\n");
}
;;//
;// HWDriver_UnLock3
/*
 *
 * Enter unlocked state
 * case: automatic full locking
 *
 */
void HWDriver_UnLock3(struct amigamesa_context *c)
{
printf("HWDriver_UnLock3\n");
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;

		if (hwcontext->lockmode < 2)
				return;
		if (hwcontext->flags & HWFLAG_LOCKED)
		{
printf("HWDriver_UnLock3 unlocking\n");
				W3D_UnLockHardware(context);
//				W3D_SetState(context,W3D_FAST,W3D_ENABLE);
//				W3D_SetState(context,W3D_INDIRECT,W3D_ENABLE);
				hwcontext->flags &= (~HWFLAG_LOCKED);
		}
printf("HWDriver_UnLock3 exit\n");
}
;;//
;// HWDriver_UnLock4
/*
 *
 * Enter unlocked state
 * case: automatic intelligent locking
 *
 */
void HWDriver_UnLock4(GLcontext* ctx)
{
printf("HWDriver_UnLock4\n");
		AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;

		if (hwcontext->lockmode != 3)
				return;
#ifdef __PPC__
		GetSysTimePPC(&tval2);
		SubTimePPC(&tval2,&tval1);
#else
		GetSysTime(&tval2);
		SubTime(&tval2,&tval1);
#endif
		if ((tval2.tv_secs*1000000+tval2.tv_micro) > INTERVAL)
		{
printf("HWDriver_UnLock4 unlocking\n");
				HWDriver_UnLock3(c);
		}
printf("HWDriver_UnLock4 exit\n");
}
;;//
;// HWDriver_Flush
/*
 *
 * HWDriver_Flush
 *
 */

void HWDriver_Flush(GLcontext* ctx)
{
		AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;
		ULONG result;

		W3D_FlushFrame(context);
		HWDriver_UnLock3(c);

		result = W3D_Flush(context);
		if (result == W3D_NOGFXMEM)
				hwcontext->flags |= HWFLAG_NOVMEM;
		if (result == W3D_NOTVISIBLE)
				hwcontext->flags &= (~HWFLAG_ACTIVE);
		SWFSDriver_flush(ctx);
}
;;//
;// HW_SetupDD_Pointers
/*
 *
 * Driver functions setup
 *
 */


void HW_SetupDD_pointers( GLcontext *ctx )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   ULONG quality;

#ifdef DBG
   /*LibPrintf*/ printf("HW_SetupDD_pointers\n");
#endif
	REM(HW_SetupDD_pointers);

   ctx->Driver.RendererString = HWDriver_rendererstring;
   ctx->Driver.SetBuffer = HWDriver_set_buffer;
   ctx->Driver.GetBufferSize = HWDriver_resize;
   ctx->Driver.UpdateState = HW_DD_pointers;
   if (c->flags & FLAG_FAST)
		quality = W3D_H_AVERAGE;
   else if (c->flags & FLAG_VERYFAST)
		quality = W3D_H_FAST;
   else
		quality = W3D_H_NICE;
   switch (c->fmt)
   {
	  case PIXFMT_LUT8:
		ctx->Driver.Index = SWFSD_set_index_LUT8;
		ctx->Driver.Color = HW_set_color_LUT8;
		ctx->Driver.Clear = HW_clear_LUT8;
		break;
	  case PIXFMT_RGB15:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGB15;
		ctx->Driver.Clear = HW_clear_RGB15;
		break;
	  case PIXFMT_RGB15PC:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGB15PC;
		ctx->Driver.Clear = HW_clear_RGB15PC;
		break;
	  case PIXFMT_RGB16:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGB16;
		ctx->Driver.Clear = HW_clear_RGB16;
		break;
	  case PIXFMT_RGB16PC:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGB16PC;
		ctx->Driver.Clear = HW_clear_RGB16PC;
		break;
	  case PIXFMT_BGR15PC:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_BGR15PC;
		ctx->Driver.Clear = HW_clear_BGR15PC;
		break;
	  case PIXFMT_BGR16PC:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_BGR16PC;
		ctx->Driver.Clear = HW_clear_BGR16PC;
		break;
	  case PIXFMT_RGB24:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGB;
		ctx->Driver.Clear = HW_clear_RGB24;
		break;
	  case PIXFMT_BGR24:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_BGR;
		ctx->Driver.Clear = HW_clear_BGR24;
		break;
	  case PIXFMT_ARGB32:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_ARGB;
		ctx->Driver.Clear = HW_clear_ARGB;
		break;
	  case PIXFMT_BGRA32:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_BGRA;
		ctx->Driver.Clear = HW_clear_BGRA;
		break;
	  case PIXFMT_RGBA32:
		ctx->Driver.Index = SWFSD_set_index_RGB;
		ctx->Driver.Color = HW_set_color_RGBA;
		ctx->Driver.Clear = HW_clear_RGBA;
		break;
   }
   Common_DD_pointers(ctx);
   if (hwcontext->flags & HWFLAG_NICETEX)
   {
		W3D_Hint(context,W3D_H_TEXMAPPING,W3D_H_NICE);
		W3D_Hint(context,W3D_H_MIPMAPPING,W3D_H_NICE);
		W3D_Hint(context,W3D_H_BILINEARFILTER,W3D_H_NICE);
		W3D_Hint(context,W3D_H_MMFILTER,W3D_H_NICE);
		W3D_Hint(context,W3D_H_PERSPECTIVE,W3D_H_NICE);
   }
   else
   {
		W3D_Hint(context,W3D_H_TEXMAPPING,quality);
		W3D_Hint(context,W3D_H_MIPMAPPING,quality);
		W3D_Hint(context,W3D_H_BILINEARFILTER,quality);
		W3D_Hint(context,W3D_H_MMFILTER,quality);
		W3D_Hint(context,W3D_H_PERSPECTIVE,quality);
   }
   W3D_Hint(context,W3D_H_BLENDING,quality);
   W3D_Hint(context,W3D_H_FOGGING,quality);
   W3D_Hint(context,W3D_H_ANTIALIASING,quality);
   W3D_Hint(context,W3D_H_DITHERING,quality);
   W3D_Hint(context,W3D_H_ZBUFFER,quality);
   ctx->Driver.Flush = HWDriver_Flush;
   ctx->Driver.Finish = HWDriver_Flush;
   ctx->Driver.Dither = HW_Dither;
   ctx->Driver.NearFar = HW_NearFar;
   ctx->Driver.TexEnv = HW_TexEnv;
   ctx->Driver.TexImage = HW_TexImage;
   ctx->Driver.TexSubImage = HW_TexSubImage;
   ctx->Driver.TexParameter = HW_TexParameter;
   ctx->Driver.BindTexture = HW_BindTexture;
   ctx->Driver.DeleteTexture = HW_DeleteTexture;

   if (c->flags & FLAG_VERYFAST)
   {
		hwcontext->CompCheck = HW_ContextSupported_VeryFast;
		hwcontext->ChPoint = HW_ChoosePoint_VeryFast;
		hwcontext->ChLine = HW_ChooseLine_VeryFast;
		hwcontext->ChTriangle = HW_ChooseTriangle_Fast;
		hwcontext->ChQuad = HW_ChooseQuad_Fast;
		hwcontext->ChLineStrip = HW_ChooseLineStrip_VeryFast;
		hwcontext->ChTriStrip = HW_ChooseTriStrip_Fast;
		hwcontext->ChTriFan = HW_ChooseTriFan_Fast;
   }
   else if (c->flags & FLAG_FAST)
   {
		hwcontext->CompCheck = HW_ContextSupported_Fast;
		hwcontext->ChPoint = HW_ChoosePoint_Fast;
		hwcontext->ChLine = HW_ChooseLine_Fast;
		hwcontext->ChTriangle = HW_ChooseTriangle_Fast;
		hwcontext->ChQuad = HW_ChooseQuad_Fast;
		hwcontext->ChLineStrip = HW_ChooseLineStrip_Fast;
		hwcontext->ChTriStrip = HW_ChooseTriStrip_Fast;
		hwcontext->ChTriFan = HW_ChooseTriFan_Fast;
   }
   else
   {
		hwcontext->CompCheck = HW_ContextSupported;
		hwcontext->ChPoint = HW_ChoosePoint;
		hwcontext->ChLine = HW_ChooseLine;
		hwcontext->ChTriangle = HW_ChooseTriangle;
		hwcontext->ChQuad = HW_ChooseQuad;
		hwcontext->ChLineStrip = HW_ChooseLineStrip;
		hwcontext->ChTriStrip = HW_ChooseTriStrip;
		hwcontext->ChTriFan = HW_ChooseTriFan;
   }
   if (hwcontext->lockmode == 1)
   {
		W3D_SetState(context,W3D_INDIRECT,W3D_DISABLE);
		W3D_SetState(context,W3D_FAST,W3D_DISABLE);
		ctx->Driver.Lock = HWDriver_Lock2;
		ctx->Driver.UnLock = HWDriver_UnLock2;
   }
   if (hwcontext->lockmode == 3)
   {
		ctx->Driver.UnLock = HWDriver_UnLock4;
   }
   if (ctx->Visual->DepthBits)
   {
		if (W3D_AllocZBuffer(context) != W3D_SUCCESS)
				hwcontext->nohw = TRUE;
		else
				hwcontext->flags |= HWFLAG_ZBUFFER;
   }

   (*ctx->Driver.UpdateState)(ctx);
}
;;//
;// HWDriver_SwapBuffer_FS
/*
 *
 * HWDriver_SwapBuffer_FS
 * swaps front and hidden bitmap in full screen mode
 *
 */

void HWDriver_SwapBuffer_FS(struct amigamesa_context *c)
{
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;
		ULONG result;
#ifdef DBG
   /*LibPrintf*/ printf("HWDriver_SwapBuffer_FS\n");
#endif

		result = W3D_Flush(context);
		if (result == W3D_NOGFXMEM)
				hwcontext->flags |= HWFLAG_NOVMEM;
		if (result == W3D_NOTVISIBLE)
				hwcontext->flags &= (~HWFLAG_ACTIVE);

		W3D_FlushFrame(context);
		HWDriver_UnLock3(c);
		if (!(hwcontext->flags & HWFLAG_ACTIVE))
		{
				if (W3D_GetDriverState(hwcontext->context) == W3D_SUCCESS)
				{
						hwcontext->flags |= HWFLAG_ACTIVE;
						((GLcontext *)c->gl_ctx)->NewState |= NEW_DRVSTATE0;
				}
		}
		if (c->flags & FLAG_TRIPLE)
				SWFSDriver_SwapBuffer_FS3(c);
		else
				SWFSDriver_SwapBuffer_FS(c);
		hwcontext->currentbm = c->draw_rp->BitMap;
		W3D_SetDrawRegion(context,hwcontext->currentbm,0,&hwcontext->scissor);
}
;;//
;// HWDriver_SwapBuffer_DR
/*
 *
 * HWDriver_SwapBuffer_DR
 * copies the hidden frame to the current rastport using
 * graphics/BltBitMapRastPort
 *
 */

void HWDriver_SwapBuffer_DR(struct amigamesa_context *c)
{
		W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
		W3D_Context *context = hwcontext->context;
		ULONG result;
#ifdef DBG
   /*LibPrintf*/ printf("HWDriver_SwapBuffer_DR\n");
#endif

		result = W3D_Flush(context);
		if (result == W3D_NOGFXMEM)
				hwcontext->flags |= HWFLAG_NOVMEM;
		if (result == W3D_NOTVISIBLE)
				hwcontext->flags &= (~HWFLAG_ACTIVE);

		W3D_FlushFrame(context);

		HWDriver_UnLock3(c);
		if (!(hwcontext->flags & HWFLAG_ACTIVE))
		{
				if (W3D_GetDriverState(hwcontext->context) == W3D_SUCCESS)
				{
						hwcontext->flags |= HWFLAG_ACTIVE;
						((GLcontext *)c->gl_ctx)->NewState |= NEW_DRVSTATE0;
				}
		}
		SWFSDriver_SwapBuffer_DR(c);

		hwcontext->currentbm = c->draw_rp->BitMap;
		W3D_SetDrawRegion(context,hwcontext->currentbm,0,&hwcontext->scissor);
}
;;//
;// HWDriver_Dispose
/*
 *
 *   Driver shutdown  - handles AGA and gfxboard driver
 *
 */

void HWDriver_Dispose(struct amigamesa_context *c)
{
	W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
	struct gl_texture_object* to = ((GLcontext *)c->gl_ctx)->Shared->TexObjectList;

REM(HWDriver_Dispose);
	W3D_Flush(hwcontext->context);
	while (to)
	{
		HW_DeleteTexture(c->gl_ctx,to);
		to = to->Next;
	}
	if (hwcontext && hwcontext->flags & HWFLAG_STATS)
		HW_PrintStats(hwcontext);
	if (hwcontext && (hwcontext->flags & HWFLAG_ZBUFFER))
		W3D_FreeZBuffer(hwcontext->context);
	if (hwcontext && hwcontext->context)
		W3D_DestroyContext(hwcontext->context);
	if (hwcontext)
#ifndef __PPC__
		FreeVec(hwcontext);
#else
		FreeVecPPC(hwcontext);
#endif

#ifndef __PPC__
  if (TimerBase) {
REM(....will CheckIO);
	/*LibPrintf*/ printf("TimerBase %ld timerio %ld\n",TimerBase,c->timerio);

	if (!CheckIO((struct IORequest *)c->timerio)) {
	  AbortIO((struct IORequest *)c->timerio);
	  WaitIO((struct IORequest *)c->timerio);
	}
	CloseDevice((struct IORequest *)c->timerio);
	DeleteMsgPort(c->timerport);
	DeleteIORequest(c->timerio);
  }
#endif

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
	FreeVec(c->back2_rp);
#else
	FreeVecPPC(c->back_rp);
	FreeVecPPC(c->back2_rp);
#endif
}
;;//
;// LaunchHWDriver

/*
 *
 *   hardware driver startup (called from the initial startup function)
 *
 */

static BOOL LaunchHWDriver(struct amigamesa_context *c,struct TagItem *tagList)
{
	W3Ddriver* hwcontext;
	char varbuffer[256];
	W3D_Driver **drivers;	
	int i;
	BOOL contextindirect;

REM(LaunchHWDriver);
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
printf("FWxFH: %dx%d, RWxRH: %dx%d, WxH: %dx%d, top: %d, bottom %d, l %d, r %d\n", \
	c->FixedWidth, c->FixedHeight, c->RealWidth, c->RealHeight, c->width, c->height, \
	c->top, c->bottom, c->left, c->right);
printf("Depth: %d\n",c->depth);

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
	printf("LHWD part 1\n");
	c->draw_rp = c->back_rp;
	if (c->flags & FLAG_FULLSCREEN)
	{
		c->back_rp->BitMap = c->sbuf2->sb_BitMap;
		c->back2_rp->BitMap = c->sbuf1->sb_BitMap;
	}
	else
	{
#if 0
		if (!(c->back_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
				return(FALSE);
		c->back2_rp->BitMap = c->back_rp->BitMap;
		if (c->flags & FLAG_TWOBUFFERS)
		{
				if (!(c->back2_rp->BitMap = AllocBitMap(c->width,c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
						return(FALSE);
		}
#else
		if (!(c->back_rp->BitMap = AllocBitMap(((c->width+63)&0xffffffc0),c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
				return(FALSE);
		c->back2_rp->BitMap = c->back_rp->BitMap;
		if (c->flags & FLAG_TWOBUFFERS)
		{
				if (!(c->back2_rp->BitMap = AllocBitMap(((c->width+63)&0xffffffc0),c->height,c->depth,BMF_MINPLANES|BMF_DISPLAYABLE,c->rp->BitMap)))
						return(FALSE);
		}
#endif
	}
	c->BackArray = (unsigned char *)GetCyberMapAttr(c->back_rp->BitMap,CYBRMATTR_DISPADR);
	c->FrontArray = c->BackArray;
	SetAPen(c->back_rp,0);
	RectFill(c->back_rp,0,0,c->width-1,c->height-1);
	printf("LHWD part 2\n");
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
	printf("LHWD part 3\n");

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
#ifndef __PPC__
	if (!(c->hwdriver = (void *)AllocVec((sizeof(W3Ddriver)),MEMF_CLEAR)))
#else
	if (!(c->hwdriver = (void *)AllocVecPPC((sizeof(W3Ddriver)),MEMF_CLEAR,0)))
#endif
		return(FALSE);
	hwcontext = (W3Ddriver *)c->hwdriver;
	printf("LHWD part 4\n");


/* Thellier: Warp3D can install several drivers but we cant select especially one 	*/
/* can only choose the first hardware one with W3D_CC_DRIVERTYPE,W3D_DRIVER_3DHW 	*/
	drivers = W3D_GetDrivers();
	while (drivers[0]) 
	{
/* recover current driver so can do QUERY3D on current driver/destfmt */
	hwcontext->driver=drivers[0];
	/*LibPrintf*/ printf("W3D_Driver <%s> soft:%ld ChipIP:%ld formats:%ld \n",hwcontext->driver->name,hwcontext->driver->swdriver,hwcontext->driver->ChipID,hwcontext->driver->formats);
	if (!hwcontext->driver->swdriver)		/* Use first hardware driver found. If no hardware driver then use last driver */
		break;
	drivers++;
	}

	contextindirect=FALSE;
	if (!StormMesa.LOCKMODE1.ON)
	if (!StormMesa.LOCKMODE2.ON)
	if (!StormMesa.LOCKMODE3.ON)
				contextindirect=TRUE;

	if (!(c->flags & FLAG_FULLSCREEN))
	{
		if (!(hwcontext->context = W3D_CreateContextTags(&hwcontext->error,
								W3D_CC_BITMAP,c->back_rp->BitMap,
								W3D_CC_YOFFSET,0,
								W3D_CC_DRIVERTYPE,W3D_DRIVER_3DHW,
								W3D_CC_GLOBALTEXENV,TRUE,
								W3D_CC_INDIRECT,contextindirect,
								W3D_CC_FAST,TRUE,
								TAG_DONE,0)))
		{
#ifndef __PPC__
				FreeVec(c->hwdriver);
#else
				FreeVecPPC(c->hwdriver);
#endif
				return(FALSE);
		}
	}
	else
	{
		ULONG modeid = GetVPModeID(&(c->Screen->ViewPort));
		if (!(hwcontext->context = W3D_CreateContextTags(&hwcontext->error,
								W3D_CC_MODEID,modeid,
								W3D_CC_BITMAP,c->back_rp->BitMap,
								W3D_CC_YOFFSET,0,
								W3D_CC_DRIVERTYPE,W3D_DRIVER_3DHW,
								W3D_CC_GLOBALTEXENV,TRUE,
								W3D_CC_INDIRECT,contextindirect,
								W3D_CC_FAST,TRUE,
								TAG_DONE,0)))
		{
#ifndef __PPC__
				FreeVec(c->hwdriver);
#else
				FreeVecPPC(c->hwdriver);
#endif
				return(FALSE);
		}
	}

/* recover current bitmap's destfmt so can do QUERY3D on current driver/destfmt */
	hwcontext->destfmt=GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_PIXFMT);
	if(hwcontext->destfmt==PIXFMT_LUT8)		{hwcontext->destfmt=W3D_FMT_CLUT; 		REM(destfmt=W3D_FMT_CLUT);}   	   
	if(hwcontext->destfmt==PIXFMT_RGB15)	{hwcontext->destfmt=W3D_FMT_R5G5B5; 	REM(destfmt=W3D_FMT_R5G5B5);}   	   
	if(hwcontext->destfmt==PIXFMT_BGR15)	{hwcontext->destfmt=W3D_FMT_B5G5R5; 	REM(destfmt=W3D_FMT_B5G5R5);}  	   
	if(hwcontext->destfmt==PIXFMT_RGB15PC)	{hwcontext->destfmt=W3D_FMT_R5G5B5PC; 	REM(destfmt=W3D_FMT_R5G5B5PC);}  	   
	if(hwcontext->destfmt==PIXFMT_BGR15PC)	{hwcontext->destfmt=W3D_FMT_B5G5R5PC; 	REM(destfmt=W3D_FMT_B5G5R5PC);}   	   
	if(hwcontext->destfmt==PIXFMT_RGB16)	{hwcontext->destfmt=W3D_FMT_R5G6B5; 	REM(destfmt=W3D_FMT_R5G6B5);}   	   
	if(hwcontext->destfmt==PIXFMT_BGR16)	{hwcontext->destfmt=W3D_FMT_B5G6R5; 	REM(destfmt=W3D_FMT_B5G6R5);}   	   
	if(hwcontext->destfmt==PIXFMT_RGB16PC)	{hwcontext->destfmt=W3D_FMT_R5G6B5PC; 	REM(destfmt=W3D_FMT_R5G6B5PC);}   	   
	if(hwcontext->destfmt==PIXFMT_BGR16PC)	{hwcontext->destfmt=W3D_FMT_B5G6R5PC; 	REM(destfmt=W3D_FMT_B5G6R5PC);}  	   
	if(hwcontext->destfmt==PIXFMT_RGB24)	{hwcontext->destfmt=W3D_FMT_R8G8B8; 	REM(destfmt=W3D_FMT_R8G8B8);}   	   
	if(hwcontext->destfmt==PIXFMT_BGR24)	{hwcontext->destfmt=W3D_FMT_B8G8R8; 	REM(destfmt=W3D_FMT_B8G8R8);}   	   
	if(hwcontext->destfmt==PIXFMT_ARGB32)	{hwcontext->destfmt=W3D_FMT_A8R8G8B8; 	REM(destfmt=W3D_FMT_A8R8G8B8);}   	   
	if(hwcontext->destfmt==PIXFMT_BGRA32)	{hwcontext->destfmt=W3D_FMT_B8G8R8A8; 	REM(destfmt=W3D_FMT_B8G8R8A8);}   	   
	if(hwcontext->destfmt==PIXFMT_RGBA32)	{hwcontext->destfmt=W3D_FMT_R8G8B8A8; 	REM(destfmt=W3D_FMT_R8G8B8A8);} 

	if (QUERY3D(W3D_Q_RECTTEXTURES) == W3D_FULLY_SUPPORTED)
		hwcontext->flags |= HWFLAG_RECTTEX;

	for (i=0;i<256;i++)
		hwcontext->colconv[i] = ((W3D_Float)i)/255.0;

	hwcontext->currentbm = c->back_rp->BitMap;
	hwcontext->envmode = W3D_MODULATE;
	hwcontext->wscale = 1.0;
	hwcontext->far = 1.0;
	W3D_SetTexEnv(hwcontext->context,NULL,hwcontext->envmode,&hwcontext->envcolor);

	if (W3D_GetDriverState(hwcontext->context) == W3D_SUCCESS)
		hwcontext->flags |= HWFLAG_ACTIVE;
	c->InitDD = (void(*)(void*))HW_SetupDD_pointers;
	c->Dispose = HWDriver_Dispose;
	if (c->flags & FLAG_FULLSCREEN)
		c->SwapBuffer = HWDriver_SwapBuffer_FS;
	else
		c->SwapBuffer = HWDriver_SwapBuffer_DR;

	if (StormMesa.STATS.ON)
			hwcontext->flags |= HWFLAG_STATS;
	if (StormMesa.NOCLAMP.ON)
			hwcontext->flags |= HWFLAG_NOCLAMP;
	if (StormMesa.NICETEX.ON)
			hwcontext->flags |= HWFLAG_NICETEX;
	if (StormMesa.NICEFOG.ON)
			hwcontext->flags |= HWFLAG_NICEFOG;
	if (StormMesa.NOHWLINES.ON)
			hwcontext->flags |= HWFLAG_NOHWLINES;

		hwcontext->lockmode = 0;
	if (StormMesa.LOCKMODE1.ON)
				hwcontext->lockmode = 1;
	if (StormMesa.LOCKMODE2.ON)
				hwcontext->lockmode = 2;
	if (StormMesa.LOCKMODE3.ON)
				hwcontext->lockmode = 3;

	if (hwcontext->lockmode >= 2)
	{
		if(W3D_SetState(hwcontext->context,W3D_INDIRECT,W3D_DISABLE) == W3D_SUCCESS) {
			printf("W3D_INDIRECT disabled\n");
		} else {
			printf("W3D_INCIRECT failed!!\n");
		}
		if(W3D_SetState(hwcontext->context,W3D_FAST,W3D_DISABLE) == W3D_SUCCESS) {
			printf("W3D_FAST disabled\n");
		} else {
			printf("W3D_FAST failed!!\n");
		}
	}

#ifndef __PPC__
/* open timer.device if intelligent locking is required */
  if (c->timerport = CreateMsgPort()) {
	if (c->timerio = (struct timerequest *)CreateIORequest(c->timerport,sizeof(struct timerequest))) {
	  if (OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)c->timerio,0) == 0) {
		TimerBase = (struct Library *)c->timerio->tr_node.io_Device;
	  }
	  else {
		DeleteIORequest(c->timerio);
		DeleteMsgPort(c->timerport);
	  }
	}
	else
	  DeleteMsgPort(c->timerport);
  }
  if (!TimerBase)
	hwcontext->lockmode = 2;			/* fall back to full locking */
#endif

	printf("LWHD: lockmode in use is %d\n",hwcontext->lockmode);
/* temporary variables */

	if (StormMesa.NOZB.ON)
			hwcontext->flags |= 0x80000000;
	if (StormMesa.NOZBU.ON)
			hwcontext->flags |= 0x40000000;
	if (StormMesa.LIN.ON)
			hwcontext->flags |= 0x20000000;

	return(TRUE);
}
;;//
;// HWDriver_init

/*
 *
 *   Driver startup - hardware driver
 *
 */


BOOL HWDriver_init(struct amigamesa_context *c,struct TagItem *tagList)
	{

REM(HWDriver_init);
	c->front_rp = c->rp;
	printf("HWDI: rastport pointer 0x%08x\n",c->rp);
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
	printf("HWDI: rastport pointer 0x%08x\n",c->rp);

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
#ifndef __PPC__
  /* open timer.device */
  if (c->timerport = CreateMsgPort()) {
	if (c->timerio = (struct timerequest *)CreateIORequest(c->timerport,sizeof(struct timerequest))) {
	  if (OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)c->timerio,0) == 0) {
		TimerBase = (struct Library *)c->timerio->tr_node.io_Device;
	  }
	  else {
		DeleteIORequest(c->timerio);
		DeleteMsgPort(c->timerport);
	  }
	}
	else
	  DeleteMsgPort(c->timerport);
  }
#endif

	return(LaunchHWDriver(c,tagList));
}
;;//

