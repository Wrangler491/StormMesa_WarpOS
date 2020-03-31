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


#include "AmigaIncludes.h"
#include <stdlib.h>
#include "GL/gl.h"

#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"
#include "types.h"
#include "depth.h"
#include "AmigaMesa.h"
#include "ADisp_HW.h"

void HWDriver_Lock3(struct amigamesa_context* c);

#if defined(__PPC__) && !defined(NOASM_PPC)
void asm_HW_SetupFlat(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float swap);
void asm_HW_SetupFlatZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float swap,W3D_Float invdepth);
void asm_HW_SetupGouraud(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte *col,
		      W3D_Float swap);
void asm_HW_SetupGouraudZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte *col,
		      W3D_Float swap, W3D_Float invdepth);
void asm_HW_SetupFlatFog(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* clip,
		      W3D_Float swap, W3D_Float wscale);
void asm_HW_SetupFlatFogZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* clip,
		      W3D_Float swap,W3D_Float invdepth,W3D_Float wscale);
void asm_HW_SetupGouraudFog(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte *col,
		      W3D_Float* clip, W3D_Float swap, W3D_Float wscale);
void asm_HW_SetupGouraudFogZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte *col,
		      W3D_Float* clip, W3D_Float swap,
		      W3D_Float invdepth, W3D_Float wscale);
void asm_HW_SetupFlatTex(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* clip,W3D_Float* texcoord,
		      W3D_Float swap, W3D_Float wscale,
		      W3D_Float texwidth, W3D_Float texheight);
void asm_HW_SetupFlatTexZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* clip,W3D_Float* texcoord,
		      W3D_Float swap, W3D_Float invdepth, W3D_Float wscale,
		      W3D_Float texwidth, W3D_Float texheight);
void asm_HW_SetupGouraudTex(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte* col,
		      W3D_Float* clip,W3D_Float* texcoord,
		      W3D_Float swap, W3D_Float wscale,
		      W3D_Float texwidth, W3D_Float texheight);
void asm_HW_SetupGouraudTexZ(int start,int end,W3D_Vertex* vptr,
		      W3D_Float* win,W3D_Float* colf,GLubyte* col,
		      W3D_Float* clip,W3D_Float* texcoord,
		      W3D_Float swap, W3D_Float invdepth, W3D_Float wscale,
		      W3D_Float texwidth, W3D_Float texheight);
#endif


// rasterization
;// HW_DrawPoint_flat
/*
 *
 * draw point
 *
 */
void HW_DrawPoint_flat(GLcontext *ctx, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3D_Point po;
   int i;
   struct vertex_buffer *VB = ctx->VB;
   GLubyte* col = VB->Color[first];
   W3D_Float* colf = hwcontext->colconv;
   GLboolean mono = VB->MonoColor;
   W3D_Color co;

   	REM(HW_DrawPoint_flat);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_POINT])+= (last-first+1);

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);
   po.tex = hwcontext->currentw3dtex;
   po.pointsize = (W3D_Float)ctx->Point.Size;
   for (i=first;i<=last;i++)
   {
	if (!mono)
	{
		co.r = colf[*col++];
		co.g = colf[*col++];
		co.b = colf[*col++];
		co.a = colf[*col++];
		W3D_SetCurrentColor(context,&co);
	}
	hwcontext->CopyVertex(&po.v1,&hwcontext->vbuffer[i]);
	if (W3D_DrawPoint(context,&po) != W3D_SUCCESS)
		hwcontext->flags &= (~HWFLAG_ACTIVE);
   }
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawPoint_smooth
/*
 *
 * draw point
 *
 */
void HW_DrawPoint_smooth(GLcontext *ctx, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   W3D_Point po;
   int i;

   	REM(HW_DrawPoint_smooth);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_POINT])+= (last-first+1);

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);
   po.tex = hwcontext->currentw3dtex;
   po.pointsize = (W3D_Float)ctx->Point.Size;
   for (i=first;i<=last;i++)
   {
	hwcontext->CopyVertex(&po.v1,&hwcontext->vbuffer[i]);
	if (W3D_DrawPoint(context,&po) != W3D_SUCCESS)
		hwcontext->flags &= (~HWFLAG_ACTIVE);
   }
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawLine_flat
/*
 *
 * draw line
 *
 */
void HW_DrawLine_flat(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Line line;
   W3D_Color co;
   GLubyte* col = VB->Color[pv];
   W3D_Float* colf = hwcontext->colconv;
   GLboolean mono = VB->MonoColor;

   	REM(HW_DrawLine_flat);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_LINE])++;

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   co.r = colf[*col++];
   co.g = colf[*col++];
   co.b = colf[*col++];
   co.a = colf[*col++];

   if (v2-1 == v1)
   {

/* optimization, very ugly style */

       W3D_Line* lineptr = (W3D_Line *)(&hwcontext->vbuffer[v1]);
       W3D_Texture* texsave = lineptr->tex;
       W3D_Float widthsave = lineptr->linewidth;
       W3D_Bool patternenable = lineptr->st_enable;
       unsigned short patternsave = lineptr->st_pattern;
       int factorsave = lineptr->st_factor;

       lineptr->tex = hwcontext->currentw3dtex;
       if (!ctx->Line.StippleFlag)
       {
	    lineptr->st_enable = TRUE;
	    lineptr->st_pattern = (unsigned short)(ctx->Line.StipplePattern);
	    lineptr->st_factor = (int)(ctx->Line.StippleFactor);
       }
       else
	    lineptr->st_enable = FALSE;
       lineptr->linewidth = (W3D_Float)ctx->Line.Width;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       if (W3D_DrawLine(context,lineptr) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       lineptr->tex = texsave;
       lineptr->linewidth = widthsave;
       lineptr->st_enable = patternenable;
       lineptr->st_pattern = patternsave;
       lineptr->st_factor = factorsave;
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {
       line.tex = hwcontext->currentw3dtex;
       if (!ctx->Line.StippleFlag)
       {
	    line.st_enable = TRUE;
	    line.st_pattern = (unsigned short)(ctx->Line.StipplePattern);
	    line.st_factor = (int)(ctx->Line.StippleFactor);
       }
       else
	    line.st_enable = FALSE;
       line.linewidth = (W3D_Float)ctx->Line.Width;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       hwcontext->CopyVertex(&line.v1,&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&line.v2,&hwcontext->vbuffer[v2]);
       if (W3D_DrawLine(context,&line) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawLine_smooth
/*
 *
 * draw line
 *
 */
void HW_DrawLine_smooth(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Line line;

   	REM(HW_DrawLine_smooth);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_LINE])++;

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if (v2-1 == v1)
   {

/* optimization, very ugly style */

       W3D_Line* lineptr = (W3D_Line *)(&hwcontext->vbuffer[v1]);
       W3D_Texture* texsave = lineptr->tex;
       W3D_Float widthsave = lineptr->linewidth;
       W3D_Bool patternenable = lineptr->st_enable;
       unsigned short patternsave = lineptr->st_pattern;
       int factorsave = lineptr->st_factor;

       lineptr->tex = hwcontext->currentw3dtex;
       if (!ctx->Line.StippleFlag)
       {
	    lineptr->st_enable = TRUE;
	    lineptr->st_pattern = (unsigned short)(ctx->Line.StipplePattern);
	    lineptr->st_factor = (int)(ctx->Line.StippleFactor);
       }
       else
	    lineptr->st_enable = FALSE;
       lineptr->linewidth = (W3D_Float)ctx->Line.Width;
       if (W3D_DrawLine(context,lineptr) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       lineptr->tex = texsave;
       lineptr->linewidth = widthsave;
       lineptr->st_enable = patternenable;
       lineptr->st_pattern = patternsave;
       lineptr->st_factor = factorsave;
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {
       line.tex = hwcontext->currentw3dtex;
       if (!ctx->Line.StippleFlag)
       {
	    line.st_enable = TRUE;
	    line.st_pattern = (unsigned short)(ctx->Line.StipplePattern);
	    line.st_factor = (int)(ctx->Line.StippleFactor);
       }
       else
	    line.st_enable = FALSE;
       line.linewidth = (W3D_Float)ctx->Line.Width;
       hwcontext->CopyVertex(&line.v1,&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&line.v2,&hwcontext->vbuffer[v2]);
       if (W3D_DrawLine(context,&line) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawTriangle_flat
/*
 *
 * draw triangles
 *
 */
void HW_DrawTriangle_flat(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangle tri;
   W3D_Color co;
   GLubyte* col = VB->Color[pv];
   W3D_Float* colf = hwcontext->colconv;
   GLboolean mono = VB->MonoColor;


   	REM(HW_DrawTriangle_flat);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_TRIANGLE])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   co.r = colf[*col++];
   co.g = colf[*col++];
   co.b = colf[*col++];
   co.a = colf[*col++];

   if (((v2-1 == v1) && (v3-2 == v1)) || ((v2-1 == v3) && (v1-2 == v3)))
   {

/* optimization, very ugly style */

       W3D_Triangle* triptr = (W3D_Triangle *)(&hwcontext->vbuffer[v2-1]);
       W3D_Texture* texsave = triptr->tex;
       unsigned char* patternsave = triptr->st_pattern;

       triptr->tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    triptr->st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    triptr->st_pattern = NULL;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       if (W3D_DrawTriangle(context,triptr) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       triptr->tex = texsave;
       triptr->st_pattern = patternsave;
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {

       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       hwcontext->CopyVertex(&tri.v1,&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&tri.v2,&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&tri.v3,&hwcontext->vbuffer[v3]);
       if (W3D_DrawTriangle(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
    }
}
;;//
;// HW_DrawTriangle_smooth
/*
 *
 * draw triangles
 *
 */
void HW_DrawTriangle_smooth(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangle tri;

   	REM(HW_DrawTriangle_smooth);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_TRIANGLE])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if (((v2-1 == v1) && (v3-2 == v1)) || ((v2-1 == v3) && (v1-2 == v3)))
   {

/* optimization, very ugly style */

       W3D_Triangle* triptr = (W3D_Triangle *)(&hwcontext->vbuffer[v2-1]);
       W3D_Texture* texsave = triptr->tex;
       unsigned char* patternsave = triptr->st_pattern;

       triptr->tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    triptr->st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    triptr->st_pattern = NULL;
       if (W3D_DrawTriangle(context,triptr) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       triptr->tex = texsave;
       triptr->st_pattern = patternsave;
       hwcontext->flags |= HWFLAG_DIRTY;

   }
   else
   {
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       hwcontext->CopyVertex(&tri.v1,&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&tri.v2,&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&tri.v3,&hwcontext->vbuffer[v3]);
       if (W3D_DrawTriangle(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
    }
}
;;//
;// HW_DrawQuad_flat
/*
 *
 * draw quadliterals
 *
 */
void HW_DrawQuad_flat(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint v4, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangles tri;
   W3D_Vertex w3dv[4];
   W3D_Color co;
   GLubyte* col = VB->Color[pv];
   W3D_Float* colf = hwcontext->colconv;
   GLboolean mono = VB->MonoColor;

   	REM(HW_DrawQuad_flat);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_QUAD])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   co.r = colf[*col++];
   co.g = colf[*col++];
   co.b = colf[*col++];
   co.a = colf[*col++];

   if ((v2-1 == v1) && (v3-3 == v1) && (v4-2 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else if ((v2-1 == v1) && (v3-2 == v1) && (v4-3 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       if (W3D_DrawTriFan(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {
       tri.vertexcount = 4;
       tri.v = w3dv;
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (!mono)
       {
	W3D_SetCurrentColor(context,&co);
       }
       hwcontext->CopyVertex(&w3dv[0],&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&w3dv[1],&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&w3dv[2],&hwcontext->vbuffer[v4]);
       hwcontext->CopyVertex(&w3dv[3],&hwcontext->vbuffer[v3]);
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawQuad_smooth
/*
 *
 * draw quadliterals
 *
 */
void HW_DrawQuad_smooth(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint v4, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangles tri;
   W3D_Vertex w3dv[4];


   	REM(HW_DrawQuad_smooth);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_QUAD])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if ((v2-1 == v1) && (v3-3 == v1) && (v4-2 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else if ((v2-1 == v1) && (v3-2 == v1) && (v4-3 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       if (W3D_DrawTriFan(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {
       tri.vertexcount = 4;
       tri.v = w3dv;
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       hwcontext->CopyVertex(&w3dv[0],&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&w3dv[1],&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&w3dv[2],&hwcontext->vbuffer[v4]);
       hwcontext->CopyVertex(&w3dv[3],&hwcontext->vbuffer[v3]);
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawPoint_smoothTwo
/*
 *
 * draw point
 *
 */
void HW_DrawPoint_smoothTwo(GLcontext *ctx, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Point po;
   int i;
   GLubyte* col;
   W3D_Float* colf = hwcontext->colconv;

   	REM(HW_DrawPoint_smoothTwo);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_POINT])+= (last-first+1);

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);
   po.tex = hwcontext->currentw3dtex;
   po.pointsize = (W3D_Float)ctx->Point.Size;
   for (i=first;i<=last;i++)
   {
	hwcontext->CopyVertex(&po.v1,&hwcontext->vbuffer[i]);
	col = VB->Color[i];
	po.v1.color.r = colf[*col++];
	po.v1.color.g = colf[*col++];
	po.v1.color.b = colf[*col++];
	po.v1.color.a = colf[*col++];
	if (W3D_DrawPoint(context,&po) != W3D_SUCCESS)
		hwcontext->flags &= (~HWFLAG_ACTIVE);
   }
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawLine_smoothTwo
/*
 *
 * draw line
 *
 */
void HW_DrawLine_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Line line;
   GLubyte* col;
   W3D_Float* colf = hwcontext->colconv;

   	REM(HW_DrawLine_smoothTwo);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_LINE])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   line.tex = hwcontext->currentw3dtex;
   if (!ctx->Line.StippleFlag)
   {
	line.st_enable = TRUE;
	line.st_pattern = (unsigned short)(ctx->Line.StipplePattern);
	line.st_factor = (int)(ctx->Line.StippleFactor);
   }
   else
	line.st_enable = FALSE;
   line.linewidth = (W3D_Float)ctx->Line.Width;
   hwcontext->CopyVertex(&line.v1,&hwcontext->vbuffer[v1]);
   hwcontext->CopyVertex(&line.v2,&hwcontext->vbuffer[v2]);
   col = VB->Color[v1];
   line.v1.color.r = colf[*col++];
   line.v1.color.g = colf[*col++];
   line.v1.color.b = colf[*col++];
   line.v1.color.a = colf[*col++];
   col = VB->Color[v2];
   line.v2.color.r = colf[*col++];
   line.v2.color.g = colf[*col++];
   line.v2.color.b = colf[*col++];
   line.v2.color.a = colf[*col++];
   if (W3D_DrawLine(context,&line) != W3D_SUCCESS)
	hwcontext->flags &= (~HWFLAG_ACTIVE);
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawTriangle_smoothTwo
/*
 *
 * draw triangles
 *
 */
void HW_DrawTriangle_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangle tri;
   GLubyte* col;
   W3D_Float* colf = hwcontext->colconv;

   	REM(HW_DrawTriangle_smoothTwo);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_TRIANGLE])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if (((v2-1 == v1) && (v3-2 == v1)) || ((v2-1 == v3) && (v1-2 == v3)))
   {

/* optimization, very ugly style */

       W3D_Triangle* triptr = (W3D_Triangle *)(&hwcontext->vbuffer[v2-1]);
       W3D_Texture* texsave = triptr->tex;
       unsigned char* patternsave = triptr->st_pattern;

       triptr->tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    triptr->st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    triptr->st_pattern = NULL;
       col = VB->Color[v1];
       hwcontext->vbuffer[v1].color.r = colf[*col++];
       hwcontext->vbuffer[v1].color.g = colf[*col++];
       hwcontext->vbuffer[v1].color.b = colf[*col++];
       hwcontext->vbuffer[v1].color.a = colf[*col++];
       col = VB->Color[v2];
       hwcontext->vbuffer[v2].color.r = colf[*col++];
       hwcontext->vbuffer[v2].color.g = colf[*col++];
       hwcontext->vbuffer[v2].color.b = colf[*col++];
       hwcontext->vbuffer[v2].color.a = colf[*col++];
       col = VB->Color[v3];
       hwcontext->vbuffer[v3].color.r = colf[*col++];
       hwcontext->vbuffer[v3].color.g = colf[*col++];
       hwcontext->vbuffer[v3].color.b = colf[*col++];
       hwcontext->vbuffer[v3].color.a = colf[*col++];
       if (W3D_DrawTriangle(context,triptr) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       triptr->tex = texsave;
       triptr->st_pattern = patternsave;
       hwcontext->flags |= HWFLAG_DIRTY;

   }
   else
   {
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       hwcontext->CopyVertex(&tri.v1,&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&tri.v2,&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&tri.v3,&hwcontext->vbuffer[v3]);
       col = VB->Color[v1];
       tri.v1.color.r = colf[*col++];
       tri.v1.color.g = colf[*col++];
       tri.v1.color.b = colf[*col++];
       tri.v1.color.a = colf[*col++];
       col = VB->Color[v2];
       tri.v2.color.r = colf[*col++];
       tri.v2.color.g = colf[*col++];
       tri.v2.color.b = colf[*col++];
       tri.v2.color.a = colf[*col++];
       col = VB->Color[v3];
       tri.v3.color.r = colf[*col++];
       tri.v3.color.g = colf[*col++];
       tri.v3.color.b = colf[*col++];
       tri.v3.color.a = colf[*col++];

       if (W3D_DrawTriangle(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawQuad_smoothTwo
/*
 *
 * draw quadliterals
 *
 */
void HW_DrawQuad_smoothTwo(GLcontext *ctx, GLuint v1, GLuint v2,
			 GLuint v3, GLuint v4, GLuint pv)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangles tri;
   W3D_Vertex w3dv[4];
   GLubyte* col;
   W3D_Float* colf = hwcontext->colconv;

   	REM(HW_DrawQuad_smoothTwo);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_QUAD])++;
   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if ((v2-1 == v1) && (v3-3 == v1) && (v4-2 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       col = VB->Color[v1];
       hwcontext->vbuffer[v1].color.r = colf[*col++];
       hwcontext->vbuffer[v1].color.g = colf[*col++];
       hwcontext->vbuffer[v1].color.b = colf[*col++];
       hwcontext->vbuffer[v1].color.a = colf[*col++];
       col = VB->Color[v2];
       hwcontext->vbuffer[v2].color.r = colf[*col++];
       hwcontext->vbuffer[v2].color.g = colf[*col++];
       hwcontext->vbuffer[v2].color.b = colf[*col++];
       hwcontext->vbuffer[v2].color.a = colf[*col++];
       col = VB->Color[v3];
       hwcontext->vbuffer[v3].color.r = colf[*col++];
       hwcontext->vbuffer[v3].color.g = colf[*col++];
       hwcontext->vbuffer[v3].color.b = colf[*col++];
       hwcontext->vbuffer[v3].color.a = colf[*col++];
       col = VB->Color[v4];
       hwcontext->vbuffer[v4].color.r = colf[*col++];
       hwcontext->vbuffer[v4].color.g = colf[*col++];
       hwcontext->vbuffer[v4].color.b = colf[*col++];
       hwcontext->vbuffer[v4].color.a = colf[*col++];
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else if ((v2-1 == v1) && (v3-2 == v1) && (v4-3 == v1))
   {
       tri.vertexcount = 4;
       tri.v = (&hwcontext->vbuffer[v1]);
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       col = VB->Color[v1];
       hwcontext->vbuffer[v1].color.r = colf[*col++];
       hwcontext->vbuffer[v1].color.g = colf[*col++];
       hwcontext->vbuffer[v1].color.b = colf[*col++];
       hwcontext->vbuffer[v1].color.a = colf[*col++];
       col = VB->Color[v2];
       hwcontext->vbuffer[v2].color.r = colf[*col++];
       hwcontext->vbuffer[v2].color.g = colf[*col++];
       hwcontext->vbuffer[v2].color.b = colf[*col++];
       hwcontext->vbuffer[v2].color.a = colf[*col++];
       col = VB->Color[v3];
       hwcontext->vbuffer[v3].color.r = colf[*col++];
       hwcontext->vbuffer[v3].color.g = colf[*col++];
       hwcontext->vbuffer[v3].color.b = colf[*col++];
       hwcontext->vbuffer[v3].color.a = colf[*col++];
       col = VB->Color[v4];
       hwcontext->vbuffer[v4].color.r = colf[*col++];
       hwcontext->vbuffer[v4].color.g = colf[*col++];
       hwcontext->vbuffer[v4].color.b = colf[*col++];
       hwcontext->vbuffer[v4].color.a = colf[*col++];
       if (W3D_DrawTriFan(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
   else
   {
       tri.vertexcount = 4;
       tri.v = w3dv;
       tri.tex = hwcontext->currentw3dtex;
       if (!ctx->Polygon.StippleFlag)
	    tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
       else
	    tri.st_pattern = NULL;
       hwcontext->CopyVertex(&w3dv[0],&hwcontext->vbuffer[v1]);
       hwcontext->CopyVertex(&w3dv[1],&hwcontext->vbuffer[v2]);
       hwcontext->CopyVertex(&w3dv[2],&hwcontext->vbuffer[v4]);
       hwcontext->CopyVertex(&w3dv[3],&hwcontext->vbuffer[v3]);
       col = VB->Color[v1];
       w3dv[0].color.r = colf[*col++];
       w3dv[0].color.g = colf[*col++];
       w3dv[0].color.b = colf[*col++];
       w3dv[0].color.a = colf[*col++];
       col = VB->Color[v2];
       w3dv[1].color.r = colf[*col++];
       w3dv[1].color.g = colf[*col++];
       w3dv[1].color.b = colf[*col++];
       w3dv[1].color.a = colf[*col++];
       col = VB->Color[v4];
       w3dv[2].color.r = colf[*col++];
       w3dv[2].color.g = colf[*col++];
       w3dv[2].color.b = colf[*col++];
       w3dv[2].color.a = colf[*col++];
       col = VB->Color[v3];
       w3dv[3].color.r = colf[*col++];
       w3dv[3].color.g = colf[*col++];
       w3dv[3].color.b = colf[*col++];
       w3dv[3].color.a = colf[*col++];
       if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
	    hwcontext->flags &= (~HWFLAG_ACTIVE);
       hwcontext->flags |= HWFLAG_DIRTY;
   }
}
;;//
;// HW_DrawLineStrip
/*
 *
 * draw line strips
 * this is never called, when two-sided lighting is enabled
 * this is not called when flat shading is enabled, except
 * when texture mapping is enabled and the env variable NOFLAT
 * is set (which results in wrong behaviour, if the constant
 * color is not always white and GL_DECAL/GL_MODULATE are used)
 *
 */
void HW_DrawLineStrip(GLcontext *ctx, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Lines li;
   W3D_Color co;

   	REM(HW_DrawLineStrip);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_LINE]) += (last-first);

   if (last-first < 1)
	return;

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   li.vertexcount = last-first+1;
   li.v = (&hwcontext->vbuffer[first]);
   li.tex = hwcontext->currentw3dtex;
   if (!ctx->Line.StippleFlag)
   {
       li.st_enable = TRUE;
       li.st_pattern = (unsigned short)(ctx->Line.StipplePattern);
       li.st_factor = (int)(ctx->Line.StippleFactor);
   }
   else
	li.st_enable = FALSE;
   if (W3D_DrawLineStrip(context,&li) != W3D_SUCCESS)
       hwcontext->flags &= (~HWFLAG_ACTIVE);
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawTriStrip
/*
 *
 * draw triangle strips
 * this is never called, when two-sided lighting is enabled
 * this is not called when flat shading is enabled, except
 * when texture mapping is enabled and the env variable NOFLAT
 * is set (which results in wrong behaviour, if the constant
 * color is not always white and GL_DECAL/GL_MODULATE are used)
 *
 */
void HW_DrawTriStrip(GLcontext *ctx, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangles tri;
   W3D_Color co;

   	//REM(HW_DrawTriStrip);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_TRIANGLE]) += (last-first-1);

   if (last-first < 2)
	return;

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   tri.vertexcount = last-first+1;
   tri.v = (&hwcontext->vbuffer[first]);
   tri.tex = hwcontext->currentw3dtex;
   if (!ctx->Polygon.StippleFlag)
       tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
   else
       tri.st_pattern = NULL;

	//LibPrintf("GLctx: 0x%08x, AMctx: 0x%08x, hwctx: 0x%08x, W3Dcontext: 0x%08x, tri: 0x%08x\n",ctx,c,hwcontext,context,tri);
   if (W3D_DrawTriStrip(context,&tri) != W3D_SUCCESS)
       hwcontext->flags &= (~HWFLAG_ACTIVE);
   hwcontext->flags |= HWFLAG_DIRTY;
}
;;//
;// HW_DrawTriFan
/*
 *
 * draw triangle fans
 * this is never called, when two-sided lighting is enabled
 * this is not called when flat shading is enabled, except
 * when texture mapping is enabled and the env variable NOFLAT
 * is set (which results in wrong behaviour, if the constant
 * color is not always white and GL_DECAL/GL_MODULATE are used)
 *
 */
void HW_DrawTriFan(GLcontext *ctx, GLuint zero, GLuint first, GLuint last)
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   W3D_Context *context = hwcontext->context;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Triangles tri;
   W3D_Color co;
   W3D_Vertex vtemp;

   	REM(HW_DrawTriFan);

   if (hwcontext->flags & HWFLAG_STATS)
	(hwcontext->statarray2[DRAW_TRIANGLE]) += (last-first);

   if (last-first < 1)
	return;

   if (hwcontext->lockmode >= 2)
	HWDriver_Lock3(c);

   if (first-zero != 1)
   {
	hwcontext->CopyVertex(&vtemp,&hwcontext->vbuffer[first-1]);
	hwcontext->CopyVertex(&hwcontext->vbuffer[first-1],&hwcontext->vbuffer[zero]);
   }
   tri.vertexcount = last-first+2;
   tri.v = (&hwcontext->vbuffer[first-1]);
   tri.tex = hwcontext->currentw3dtex;
   if (!ctx->Polygon.StippleFlag)
       tri.st_pattern = (unsigned char *)ctx->PolygonStipple;
   else
       tri.st_pattern = NULL;
   if (W3D_DrawTriFan(context,&tri) != W3D_SUCCESS)
       hwcontext->flags &= (~HWFLAG_ACTIVE);
   hwcontext->flags |= HWFLAG_DIRTY;
   if (first-zero != 1)
	hwcontext->CopyVertex(&hwcontext->vbuffer[first-1],&vtemp);
}
;;//

// raster setup
;// HW_SetupFlat
/*
 *
 * raster setup function
 * flat shading, no texture mapping, no fogging, no ZBuffering
 *
 */
void HW_SetupFlat( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlat(start,end,vptr,win,swap);
#else
   for (i=start;i<end;i++,vptr++)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	win++;
   }
#endif
}
;;//
;// HW_SetupFlatZ
/*
 *
 * raster setup function
 * flat shading, no texture mapping, no fogging, ZBuffering
 *
 */
void HW_SetupFlatZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlatZ(start,end,vptr,win,swap,invdepth);
#else
   for (i=start;i<end;i++,vptr++)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
   }
#endif
}
;;//
;// HW_SetupGouraud
/*
 *
 * raster setup function
 * gouraud shading, no texture mapping, no fogging, no ZBuffering
 *
 */
void HW_SetupGouraud( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraud(start,end,vptr,win,colf,col,swap);
#else
   for (i=start;i<end;i++,vptr++)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
	win++;
   }
#endif
}
;;//
;// HW_SetupGouraudZ
/*
 *
 * raster setup function
 * gouraud shading, no texture mapping, no fogging, ZBuffering
 *
 */
void HW_SetupGouraudZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraudZ(start,end,vptr,win,colf,col,swap,invdepth);
#else
   for (i=start;i<end;i++,vptr++)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
   }
#endif
}
;;//
;// HW_SetupFlatFog
/*
 *
 * raster setup function
 * flat shading, no texture mapping, fogging, no ZBuffering
 *
 */
void HW_SetupFlatFog( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlatFog(start,end,vptr,win,clip,swap,wscale);
#else
   for (i=start;i<end;i++,vptr++,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->w = (W3D_Float)(wscale/clip[3]);
	win++;
   }
#endif
}
;;//
;// HW_SetupFlatFogZ
/*
 *
 * raster setup function
 * flat shading, no texture mapping, fogging, ZBuffering
 *
 */
void HW_SetupFlatFogZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlatFogZ(start,end,vptr,win,clip,swap,invdepth,wscale);
#else
   for (i=start;i<end;i++,vptr++,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
	vptr->w = (W3D_Float)(wscale/clip[3]);
   }
#endif
}
;;//
;// HW_SetupGouraudFog
/*
 *
 * raster setup function
 * gouraud shading, no texture mapping, fogging, no ZBuffering
 *
 */
void HW_SetupGouraudFog( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraudFog(start,end,vptr,win,colf,col,clip,swap,wscale);
#else
   for (i=start;i<end;i++,vptr++,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->w = (W3D_Float)(wscale/clip[3]);
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
	win++;
   }
#endif
}
;;//
;// HW_SetupGouraudFogZ
/*
 *
 * raster setup function
 * gouraud shading, no texture mapping, fogging, ZBuffering
 *
 */
void HW_SetupGouraudFogZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraudFogZ(start,end,vptr,win,colf,col,clip,swap,invdepth,wscale);
#else
   for (i=start;i<end;i++,vptr++,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
	vptr->w = (W3D_Float)(wscale/clip[3]);
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
   }
#endif
}
;;//
;// HW_SetupFlatTex
/*
 *
 * raster setup function
 * flat shading, texture mapping, no ZBuffering
 *
 */
void HW_SetupFlatTex( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* texcoord = VB->MultiTexCoord[0][start];
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;
   W3D_Float tc3,invq;
   W3D_Float one = 1.0;
   W3D_Float texwidth = hwcontext->texwidth;
   W3D_Float texheight = hwcontext->texheight;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlatTex(start,end,vptr,win,clip,texcoord,swap,wscale,texwidth,texheight);
#else
   for (i=start;i<end;i++,vptr++,texcoord+=4,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	tc3 = texcoord[3];
	if (tc3 == one)
	{
		vptr->u = (W3D_Float)(texwidth * texcoord[0]);
		vptr->v = (W3D_Float)(texheight * texcoord[1]);
		vptr->w = (W3D_Float)(wscale/clip[3]);
	}
	else
	{
		invq = one/tc3;
		vptr->u = (W3D_Float)(texwidth * texcoord[0] * invq);
		vptr->v = (W3D_Float)(texheight * texcoord[1] * invq);
		vptr->w = (W3D_Float)(tc3*wscale/clip[3]);
	}
	win++;
   }
#endif
}
;;//
;// HW_SetupFlatTexZ
/*
 *
 * raster setup function
 * flat shading, texture mapping, ZBuffering
 *
 */
void HW_SetupFlatTexZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* texcoord = VB->MultiTexCoord[0][start];
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;
   W3D_Float tc3,invq;
   W3D_Float one = 1.0;
   W3D_Float texwidth = hwcontext->texwidth;
   W3D_Float texheight = hwcontext->texheight;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupFlatTexZ(start,end,vptr,win,clip,texcoord,swap,invdepth,wscale,texwidth,texheight);
#else
   for (i=start;i<end;i++,vptr++,texcoord+=4,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
	tc3 = texcoord[3];
	if (tc3 == one)
	{
		vptr->u = (W3D_Float)(texwidth * texcoord[0]);
		vptr->v = (W3D_Float)(texheight * texcoord[1]);
		vptr->w = (W3D_Float)(wscale/clip[3]);
	}
	else
	{
		invq = one/tc3;
		vptr->u = (W3D_Float)(texwidth * texcoord[0] * invq);
		vptr->v = (W3D_Float)(texheight * texcoord[1] * invq);
		vptr->w = (W3D_Float)(tc3*wscale/clip[3]);
	}
   }
#endif
}
;;//
;// HW_SetupGouraudTex
/*
 *
 * raster setup function
 * gouraud shading, texture mapping, no ZBuffering
 *
 */
void HW_SetupGouraudTex( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* texcoord = VB->MultiTexCoord[0][start];
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;
   W3D_Float tc3,invq;
   W3D_Float one = 1.0;
   W3D_Float texwidth = hwcontext->texwidth;
   W3D_Float texheight = hwcontext->texheight;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraudTex(start,end,vptr,win,colf,col,clip,texcoord,swap,wscale,texwidth,texheight);
#else
   for (i=start;i<end;i++,vptr++,texcoord+=4,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	tc3 = texcoord[3];
	if (tc3 == one)
	{
		vptr->u = (W3D_Float)(texwidth * texcoord[0]);
		vptr->v = (W3D_Float)(texheight * texcoord[1]);
		vptr->w = (W3D_Float)(wscale/clip[3]);
	}
	else
	{
		invq = one/tc3;
		vptr->u = (W3D_Float)(texwidth * texcoord[0] * invq);
		vptr->v = (W3D_Float)(texheight * texcoord[1] * invq);
		vptr->w = (W3D_Float)(tc3*wscale/clip[3]);
	}
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
	win++;
   }
#endif
}
;;//
;// HW_SetupGouraudTexZ
/*
 *
 * raster setup function
 * gouraud shading, texture mapping, ZBuffering
 *
 */
void HW_SetupGouraudTexZ( GLcontext *ctx, GLuint start, GLuint end )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
   W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
   struct vertex_buffer *VB = ctx->VB;
   W3D_Vertex* vptr = &(hwcontext->vbuffer[start]);
   GLubyte* col = VB->Color[start];
   W3D_Float* colf = hwcontext->colconv;
   W3D_Float* win = (W3D_Float *)VB->Win[start];
   W3D_Float swap = hwcontext->height;
   int i;
   W3D_Float invdepth = 1.0/DEPTH_SCALE;
   W3D_Float* texcoord = VB->MultiTexCoord[0][start];
   W3D_Float* clip = VB->Clip[start];
   W3D_Float wscale = hwcontext->wscale;
   W3D_Float tc3,invq;
   W3D_Float one = 1.0;
   W3D_Float texwidth = hwcontext->texwidth;
   W3D_Float texheight = hwcontext->texheight;

#if defined(__PPC__) && !defined(NOASM_PPC)
   asm_HW_SetupGouraudTexZ(start,end,vptr,win,colf,col,clip,texcoord,swap,invdepth,wscale,texwidth,texheight);
#else
   for (i=start;i<end;i++,vptr++,texcoord+=4,clip+=4)
   {
	vptr->x = (W3D_Float)*win++;
	vptr->y = (W3D_Float)(swap-*win++);
	vptr->z = (W3D_Float)(*win++*invdepth);
	tc3 = texcoord[3];
	if (tc3 == one)
	{
		vptr->u = (W3D_Float)(texwidth * texcoord[0]);
		vptr->v = (W3D_Float)(texheight * texcoord[1]);
		vptr->w = (W3D_Float)(wscale/clip[3]);
	}
	else
	{
		invq = one/tc3;
		vptr->u = (W3D_Float)(texwidth * texcoord[0] * invq);
		vptr->v = (W3D_Float)(texheight * texcoord[1] * invq);
		vptr->w = (W3D_Float)(tc3*wscale/clip[3]);
	}
	vptr->color.r = colf[*col++];
	vptr->color.g = colf[*col++];
	vptr->color.b = colf[*col++];
	vptr->color.a = colf[*col++];
   }
#endif
}
;;//

// vertex copy functions
;// HW_CopyFlat
/*
 *
 * vertex copy function
 * flat shading, no texture mapping, no fogging, no ZBuffering
 *
 */
void HW_CopyFlat(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
}
;;//
;// HW_CopyFlatZ
/*
 *
 * vertex copy function
 * flat shading, no texture mapping, no fogging, ZBuffering
 *
 */
void HW_CopyFlatZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
}
;;//
;// HW_CopyGouraud
/*
 *
 * vertex copy function
 * gouraud shading, no texture mapping, no fogging, no ZBuffering
 *
 */
void HW_CopyGouraud(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//
;// HW_CopyGouraudZ
/*
 *
 * vertex copy function
 * gouraud shading, no texture mapping, no fogging, ZBuffering
 *
 */
void HW_CopyGouraudZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//
;// HW_CopyFlatFog
/*
 *
 * vertex copy function
 * flat shading, no texture mapping, fogging, no ZBuffering
 *
 */
void HW_CopyFlatFog(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->w = source->w;
}
;;//
;// HW_CopyFlatFogZ
/*
 *
 * vertex copy function
 * flat shading, no texture mapping, fogging, ZBuffering
 *
 */
void HW_CopyFlatFogZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
   dest->w = source->w;
}
;;//
;// HW_CopyGouraudFog
/*
 *
 * vertex copy function
 * gouraud shading, no texture mapping, fogging, no ZBuffering
 *
 */
void HW_CopyGouraudFog(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->w = source->w;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//
;// HW_CopyGouraudFogZ
/*
 *
 * vertex copy function
 * gouraud shading, no texture mapping, fogging, ZBuffering
 *
 */
void HW_CopyGouraudFogZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
   dest->w = source->w;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//
;// HW_CopyFlatTex
/*
 *
 * vertex copy function
 * flat shading, texture mapping, no ZBuffering
 *
 */
void HW_CopyFlatTex(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->u = source->u;
   dest->v = source->v;
   dest->w = source->w;
}
;;//
;// HW_CopyFlatTexZ
/*
 *
 * vertex copy function
 * flat shading, texture mapping, ZBuffering
 *
 */
void HW_CopyFlatTexZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
   dest->u = source->u;
   dest->v = source->v;
   dest->w = source->w;
}
;;//
;// HW_CopyGouraudTex
/*
 *
 * vertex copy function
 * gouraud shading, texture mapping, no ZBuffering
 *
 */
void HW_CopyGouraudTex(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->u = source->u;
   dest->v = source->v;
   dest->w = source->w;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//
;// HW_CopyGouraudTexZ
/*
 *
 * vertex copy function
 * gouraud shading, texture mapping, ZBuffering
 *
 */
void HW_CopyGouraudTexZ(W3D_Vertex* dest, W3D_Vertex* source)
{
   dest->x = source->x;
   dest->y = source->y;
   dest->z = source->z;
   dest->u = source->u;
   dest->v = source->v;
   dest->w = source->w;
   dest->color.r = source->color.r;
   dest->color.g = source->color.g;
   dest->color.b = source->color.b;
   dest->color.a = source->color.a;
}
;;//

#include "HW_DrawTriangles.c"

