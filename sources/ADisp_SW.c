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
   cybergraphics.library). Based on ADisp_Cyb.c of the MESA 2.2
   archive.
   Uses functions of ADisp_SWFS.c (and ADispSWFS.c uses functions
   of this source here). Could be merged into one source, but,
   well, it works, so why change?

   Only double-buffering version

   4.1.1998 - start coding
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

#define DBG

#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

extern UBYTE DivMod51[];
extern UBYTE Halftone8x8[];

void SWDriver_SwapBuffer_LUT8(struct amigamesa_context *c);
void SWDriver_SwapBuffer_AGA(struct amigamesa_context *c);
void SWDriver_SwapBuffer_ARGB(struct amigamesa_context *c);

// misc driver functions which work independent of the colormodel
;// SWDriver_resize
static void SWDriver_resize( GLcontext *ctx,GLuint *width, GLuint *height)
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    *width=c->width;
    *height=c->height;
    if (c->rp == NULL)
      return;
#ifdef DBG
    LibPrintf("SWDriver_resize:\n"); // %d %d\n",c->width,c->height);
#endif

    if(!((c->width  == (c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX-c->left-c->right+1))
    &&  (c->height == (c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY-c->bottom-c->top+1))))
	{
	    c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
	    c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;

	    if (c->depth <= 8)
	    {
		FreeBitMap(c->temprp->BitMap);
		*width=c->width = c->RealWidth-c->left-c->right;
		*height=c->height = c->RealHeight-c->bottom-c->top;
#if 1
		c->bprow = c->width;
#ifndef __PPC__
		FreeVec(c->BackArray);
		if (!(c->BackArray = AllocVec(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
#else
		FreeVecPPC(c->BackArray);
		if (!(c->BackArray = AllocVecPPC(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
#else
		c->bprow = (c->width+15)&0xfffffff0;
#ifndef __PPC__
		FreeVec(c->BackArray);
		if (!(c->BackArray = AllocVec(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
#else
		FreeVecPPC(c->BackArray);
		if (!(c->BackArray = AllocVecPPC(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
#endif
		{
		    LibPrintf("Panic! Not enough memory for resizing!\n");
		    exit(0);
		}
		if (c->flags & FLAG_TWOBUFFERS)
		{
#if 1
#ifndef __PPC__
			FreeVec(c->FrontArray);
			if (!(c->FrontArray = AllocVec(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
#else
			FreeVecPPC(c->FrontArray);
			if (!(c->FrontArray = AllocVecPPC(c->width*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
#else
#ifndef __PPC__
			FreeVec(c->FrontArray);
			if (!(c->FrontArray = AllocVec(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
#else
			FreeVecPPC(c->FrontArray);
			if (!(c->FrontArray = AllocVecPPC(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
#endif
			{
				LibPrintf("Panic! Not enough memory for resizing!\n");
				exit(0);
			}
		}
		else
			c->FrontArray = c->BackArray;
		if (!(c->temprp->BitMap = AllocBitMap((((c->width+15)>>4)<<4),1,c->depth,0,c->rp->BitMap)))
		{
		    LibPrintf("Panic! Not enough memory for resizing!\n");
		    exit(0);
		}
	    }
	    else
	    {
		*width=c->width = c->RealWidth-c->left-c->right;
		*height=c->height = c->RealHeight-c->bottom-c->top;
		c->bprow = c->width * 4;
#ifndef __PPC__
		FreeVec(c->BackArray);
		if (!(c->BackArray = AllocVec(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR)))
#else
		FreeVecPPC(c->BackArray);
		if (!(c->BackArray = AllocVecPPC(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
		{
		    LibPrintf("Panic! Not enough memory for resizing!\n");
		    exit(0);
		}
		if (c->flags & FLAG_TWOBUFFERS)
		{
#ifndef __PPC__
			FreeVec(c->FrontArray);
			if (!(c->FrontArray = AllocVec(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR)))
#else
			FreeVecPPC(c->FrontArray);
			if (!(c->FrontArray = AllocVecPPC(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
			{
				LibPrintf("Panic! Not enough memory for resizing!\n");
				exit(0);
			}
		}
		else
			c->FrontArray = c->BackArray;
	    }
	    c->DrawBuffer = c->BackArray;
	    c->ReadBuffer = c->BackArray;
	    SetBuffer(c);
	}
}
;;//

// clear buffer
;// SWD_clear_LUT8
//#if defined(NOASM_68K) || defined(NOASM_PPC)
GLbitfield SWD_clear_LUT8(GLcontext *ctx, GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long color = c->clearpixel;

#ifdef DBG
    LibPrintf("SWD_clear_LUT8 \n"); //%ld\n",c->clearpixel);
#endif

  if ((mask & GL_COLOR_BUFFER_BIT) && (!(ctx->Color.SWmasking)) )
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
	    buffer = (char *)buffer + mod;	//was (char *)buffer += mod;
	}
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
//#endif
;;//
;// SWD_clear_ARGB
//#if defined(NOASM_68K) || defined(NOASM_PPC)
GLbitfield SWD_clear_ARGB(GLcontext *ctx, GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
    unsigned long col = c->clearpixel;
/*    unsigned long col = color | (color << 8) | (color << 16) | (color << 24); */


#ifdef DBG
    LibPrintf("SWD_clear_ARGB\n");
#endif

  if (mask & GL_COLOR_BUFFER_BIT)
  {
    if (all)
    {
	long* buffer = (long *)c->DrawBuffer;
	int x,max;
	max = (c->height * c->width);
	for (x=0;x<max;x++)
	    *buffer++ = col;
    }
    else
    {
	long* buffer = (long *)c->DrawBuffer +
	    (c->height-y-height) * c->width + x;
	int y,ymax,x,xmax;
	int mod = c->width - width;

	ymax = height;
	xmax = width;
	for (y=0;y<ymax;y++)
	{
	    for (x=0;x<xmax;x++)
		*buffer++ = col;
	    buffer += mod;
	}
    }
    mask &= (~GL_COLOR_BUFFER_BIT);
  }
  return(mask);
}
//#endif
;;//

// renderer string
;// SWDriver_rendererstring
const char *SWDriver_rendererstring(void)
{
	return("AMIGA_DB_IR");
}
;;//

// flush
;// SWDriver_flush
void SWDriver_flush( GLcontext *ctx )
{
    UBYTE* array;
    AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;


#ifdef DBG
    LibPrintf("SWDriver_flush\n");
#endif

    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (c->SwapBuffer == SWDriver_SwapBuffer_LUT8)
	{
		array = c->backarrayflag ? c->BackArray : c->FrontArray;
		STACKWritePixelArray(array,
				0,
				0,
				c->width,
				c->rp,
				c->left,
				c->top,
				c->width,
				c->height,
				RECTFMT_LUT8);
	}
	else if (c->SwapBuffer == SWDriver_SwapBuffer_ARGB)
	{

		array = c->backarrayflag ? c->BackArray : c->FrontArray;
		STACKWritePixelArray(array,
				0,
				0,
				c->width*4,
				c->rp,
				c->left,
				c->top,
				c->width,
				c->height,
				RECTFMT_ARGB);
	}
	else if (c->SwapBuffer == SWDriver_SwapBuffer_AGA)
	{
		array = c->backarrayflag ? c->BackArray : c->FrontArray;
		if (((struct Library *)GfxBase)->lib_Version >= 40)
		{
			WriteChunkyPixels(c->rp,
				 c->left,
				 c->top,
				 c->left+c->width-1,
				 c->top+c->height-1,
				 array,
				 c->width);
		}
		else
		{
			WritePixelArray8(c->rp,
				 c->left,
				 c->top,
				 c->left+c->width-1,
				 c->top+c->height-1,
				 array,
				 c->temprp);
		}
	}
    }
}
;;//

// driver setup / dispose
;// SWDriver_DD_Pointers
/*
 *
 * Driver functions setup
 *
 */


void SWDriver_DD_pointers( GLcontext *ctx )
{
   AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DBG
    LibPrintf("SWDdriver_DD_pointers\n");
#endif


   ctx->Driver.RendererString = SWDriver_rendererstring;
   ctx->Driver.Flush = SWDriver_flush;
   ctx->Driver.UpdateState = SWDriver_DD_pointers;
   ctx->Driver.SetBuffer = SWFSDriver_set_buffer;
   ctx->Driver.GetBufferSize = SWDriver_resize;
   if (c->depth <= 8)
   {
	ctx->Driver.ClearIndex = SWFSD_clear_index_LUT8;
	ctx->Driver.ClearColor = SWFSD_clear_color_LUT8;
	ctx->Driver.Index = SWFSD_set_index_LUT8;
	ctx->Driver.Color = SWFSD_set_color_LUT8;
	ctx->Driver.Clear = SWD_clear_LUT8;
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
   }
   else
   {
	ctx->Driver.ClearIndex = SWFSD_clear_index_RGB;
	ctx->Driver.ClearColor = SWFSD_clear_color_ARGB;
	ctx->Driver.Index = SWFSD_set_index_RGB;
	ctx->Driver.Color = SWFSD_set_color_ARGB;
	ctx->Driver.Clear = SWD_clear_ARGB;
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
   }
}
;;//
;// SWDriver_SwapBuffer_LUT8
/*
 *
 * SWDriver_SwapBuffer_LUT8
 * copies the hidden frame to the current rastport using
 * cybergraphics/WritePixelArray
 *
 */

void SWDriver_SwapBuffer_LUT8(struct amigamesa_context *c)
{
	UBYTE* array;

#ifdef DBG
    LibPrintf("SWDriver_SwapBuffer_LUT8\n");
#endif

	c->backarrayflag ^= 1;
	SetBuffer(c);
	if (c->rp == NULL)
		return;
	WaitBOVP(&c->Screen->ViewPort);
	array = c->backarrayflag ? c->BackArray : c->FrontArray;
	STACKWritePixelArray(array,
			0,
			0,
			c->width,
			c->rp,
			c->left,
			c->top,
			c->width,
			c->height,
			RECTFMT_LUT8);
}
;;//
;// SWDriver_SwapBuffer_AGA
/*
 *
 * SWDriver_SwapBuffer_AGA
 * copies the hidden frame to the current rastport using
 * graphics/WriteChunkyPixels
 *
 */

void SWDriver_SwapBuffer_AGA(struct amigamesa_context *c)
{
	UBYTE* array;

#ifdef DBG
    LibPrintf("SWDricer_SwapBuffer_AGA\n");
#endif

	c->backarrayflag ^= 1;
	SetBuffer(c);
	if (c->rp == NULL)
		return;

	WaitBOVP(&c->Screen->ViewPort);
	array = c->backarrayflag ? c->BackArray : c->FrontArray;
	if (((struct Library *)GfxBase)->lib_Version >= 40)
	{
		WriteChunkyPixels(c->rp,
			 c->left,
			 c->top,
			 c->left+c->width-1,
			 c->top+c->height-1,
			 array,
			 c->width);
	}
	else
	{
		WritePixelArray8(c->rp,
			 c->left,
			 c->top,
			 c->left+c->width-1,
			 c->top+c->height-1,
			 array,
			 c->temprp);
	}
}
;;//
;// SWDriver_SwapBuffer_ARGB
/*
 *
 * SWDriver_SwapBuffer_ARGB
 * copies the hidden frame to the current rastport using
 * cybergraphics/WritePixelArray
 *
 */

void SWDriver_SwapBuffer_ARGB(struct amigamesa_context *c)
{
	UBYTE* array;

#ifdef DBG
    LibPrintf("SWDriver_SwapBuffer_ARGB\n");
#endif

	c->backarrayflag ^= 1;
	SetBuffer(c);
	if (c->rp == NULL)
		return;
	WaitBOVP(&c->Screen->ViewPort);
	array = c->backarrayflag ? c->BackArray : c->FrontArray;
	STACKWritePixelArray(array,
			0,
			0,
			c->width*4,
			c->rp,
			c->left,
			c->top,
			c->width,
			c->height,
			RECTFMT_ARGB);
}
;;//
;// EvalPen
/*
 *
 * Evaluate real(LUT8)/GL(ARGB) pen index for given ARGB values
 *
 */
#ifndef __PPC__
long EvalPen(struct amigamesa_context *c,unsigned char r, unsigned char g,unsigned char b)
#else
long EvalPen(struct amigamesa_context *c, unsigned char r, unsigned char g, unsigned char b)
#endif
{
	return (GetPen((int *)c->ColorTable,r,g,b));
}
;;//
;// EvalPen_D
/*
 *
 * Evaluate real(LUT8)/GL(ARGB) pen index for given ARGB values
 * Uses dithering algorithm
 *
 */
#ifndef __PPC__
long EvalPen_D(struct amigamesa_context *c,unsigned char r, unsigned char g,unsigned char b,int x, int y)
#else
long EvalPen_D(struct amigamesa_context *c, unsigned char r, unsigned char g, unsigned char b, int x, int y)
#endif
{

#ifdef DBG
    LibPrintf("EvalPen_D\n");
#endif

	return (GetPenDithered((int *)c->ColorTable,r,g,b,x,y));
}
;;//
;// FindNearestPen
/*
 *
 * Evaluate the best pen in a pen table to match given ARGB values
 * 'tab' points to an array of unsigned longs
 * containing the allocated pens
 * Based on 'GetPen' of ht_colors.c
 */
long FindNearestPen(struct amigamesa_context *c, unsigned long r, unsigned long g, unsigned long b, unsigned long* tab)
{
	long i;
	int x;
	int cn = -1;
	long dist=0x7ffffff;

	unsigned long col,pr,pg,pb;

#ifdef DBG
    LibPrintf("FindNearestPen\n");
#endif

	for (i=0;i<=256;i++)
	{
	    col = tab[i];
	    if (col != 0xffffffff)
	    {
		pr = (col >> 16) & 0xff;
		pg = (col >> 8) & 0xff;
		pb = col & 0xff;
		if(!(x = (pr-r)*(pr-r)+(pg-g)*(pg-g)+(pb-b)*(pb-b)))
		{
			cn = i;
			break;
		}
		if(x < dist)
		{
			dist = x;
			cn = i;
		}
	    }
	}
//        LibPrintf("Returned pen no: \n"); //%ld Col r: %ld g: %ld b: %ld | dist: %ld\n",cn,r,g,b,dist);
	return cn;
}
;;//
;// CreateTransTable
/*
 *
 * CreateTransTable: creates the color translation table
 * (real LUT8 pen <-> RGBA color)
 * only called in RGBA mode
 *
 *
 */

int CreateTransTable(struct amigamesa_context* c)
{
	int i,j;
REM(CreateTransTable);

	c->ColorTable = (long *)&(c->ColTable);
	c->ColorTable2 = (long *)&(c->ColTable2);
	AllocColors((int *)c->ColorTable,(int *)c->ColorTable2,c->Screen);
	for (i=0;i<512;i++)
		c->dtable[i] = DivMod51[i];
	for (i=0;i<8;i++)
	{
		for (j=0;j<8;j++)
		{
			c->dmatrix[i*16+j] = Halftone8x8[i*8+j];
			c->dmatrix[i*16+8+j] = Halftone8x8[i*8+j];
		}
	}
	return(TRUE);
}
;;//
;// DisposeTransTable
/*
 *
 * DisposeTransTable: releases all pens allocated by CreateTransTable
 * and frees all memory
 *
 */

void DisposeTransTable(struct amigamesa_context* c)
{
#ifdef DBG
    LibPrintf("DisposeTransTable\n");
#endif
	FreeColors((int *)c->ColorTable,c->Screen);
}
;;//
;// InitTransTable
/*
 *
 * InitTransTable: allocates memory for the (GL index pen <-> RGBA
 * conversion). The table is filled by AmigaMesaSetOneColor.
 * only called in LUT8 mode
 *
 */

int InitTransTable(struct amigamesa_context* c)
{
	int i;
REM(InitTransTable);

#ifndef __PPC__
	if (!(c->ColorTable2 = AllocVec(256*sizeof(long),MEMF_PUBLIC|MEMF_CLEAR)))
#else
	if (!(c->ColorTable2 = AllocVecPPC(256*sizeof(long),MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
		return(0);
	for (i=0;i<256;i++)
	    c->ColorTable2[i] = -1;
	return(1);
}
;;//
;// FreeTransTable
/*
 *
 * FreeTransTable: frees all memory allocated by InitTransTable
 *
 */

void FreeTransTable(struct amigamesa_context* c)
{
#ifdef DBG
	LibPrintf("FreeTransTable\n");
#endif
	if ( (!(c->visual->rgb_flag)) && (c->depth <= 8) )
	{
	    int i,j;

	    for (i=0;i<256;i++)
	    {
		if (c->ColorTable2[i] >= 0)
		{
		    for (j=0;j<=c->ColorTable2[i];j++)
			ReleasePen(c->Screen->ViewPort.ColorMap,i);
		}
	    }
	}
#ifndef __PPC__
	FreeVec(c->ColorTable2);
#else
	FreeVecPPC(c->ColorTable2);
#endif
}
;;//
;// SWDriver_Dispose
/*
 *
 *   Driver shutdown  - handles AGA and gfxboard driver
 *
 */

void SWDriver_Dispose(struct amigamesa_context *c)
{
#ifdef DBG
    LibPrintf("SWDriver_Dispose\n");
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
	FreeBitMap(c->temprp->BitMap);
#ifndef __PPC__
	FreeVec(c->temprp);
#else
	FreeVecPPC(c->temprp);
#endif
    }
    else
    {
	if (!(c->flags & FLAG_RGBA))
	{
	    FreeTransTable(c);
	}
    }
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
;;//
;// LaunchGfxboardDriver

/*
 *
 *   Gfxboard driver startup (called from the initial startup function)
 *
 */

static BOOL LaunchGfxboardDriver(struct amigamesa_context *c,struct TagItem *tagList)
{
#ifdef DBG
    LibPrintf("LaunchGfxboardDriver\n");
#endif

    c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
    c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
    c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;
#ifdef DBG
    LibPrintf("realwidth, realheight \n"); //%d %d\n",c->RealWidth,c->RealHeight);
#endif
#ifdef DBG
    LibPrintf("MaxX, MinX, MaxY, MinY \n"); /*%d %d %d %d\n",
	    c->rp->Layer->bounds.MaxX,
	    c->rp->Layer->bounds.MinX,
	    c->rp->Layer->bounds.MaxY,
	    c->rp->Layer->bounds.MinY); */
#endif
    c->left = GetTagData(AMA_Left,0,tagList);
    c->bottom = GetTagData(AMA_Bottom,0,tagList);
    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);
    c->right = c->RealWidth - c->width - c->left;
    c->top = c->RealHeight - c->height - c->bottom;
    if (c->depth <= 8)
    {
	c->bprow = c->width;
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
#ifndef __PPC__
	if (!(c->temprp = AllocVec(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR)))
#else
	if (!(c->temprp = AllocVecPPC(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
		return(FALSE);
	InitRastPort(c->temprp);
	if (!(c->temprp->BitMap = AllocBitMap((((c->width+15)>>4)<<4),1,c->depth,0,c->rp->BitMap)))
		return(FALSE);
    }
    else
    {
	c->bprow = c->width*4;
#ifndef __PPC__
	if (!(c->BackArray = AllocVec(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR)))
		return(FALSE);
	c->FrontArray = c->BackArray;
	if (c->flags & FLAG_TWOBUFFERS)
	{
		if (!(c->FrontArray = AllocVec(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR)))
			return(FALSE);
	}
#else
	if (!(c->BackArray = AllocVecPPC(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR,0)))
		return(FALSE);
	c->FrontArray = c->BackArray;
	if (c->flags & FLAG_TWOBUFFERS)
	{
		if (!(c->FrontArray = AllocVecPPC(c->width*c->height*4,MEMF_PUBLIC|MEMF_CLEAR,0)))
			return(FALSE);
	}
#endif
	if (!(c->visual->rgb_flag))
	{
	    if (!(InitTransTable(c)))
		return(FALSE);
	}
    }
    c->DrawBuffer = c->BackArray;
    c->ReadBuffer = c->BackArray;
    c->drawbufferflag = 0; /* back buffer */
    c->readbufferflag = 0; /* back buffer */
    c->backarrayflag = 0; /* back array really points to back buffer */
    c->InitDD = (void(*)(void*))SWDriver_DD_pointers;
    c->Dispose = SWDriver_Dispose;
    if (c->depth <= 8)
	c->SwapBuffer = SWDriver_SwapBuffer_LUT8;
    else
	c->SwapBuffer = SWDriver_SwapBuffer_ARGB;
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
#ifdef DBG
    LibPrintf("LaunchAGADriver\n");
#endif

    c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
    c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX+1;
    c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY+1;
    c->left = GetTagData(AMA_Left,0,tagList);
    c->bottom = GetTagData(AMA_Bottom,0,tagList);
    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
#if 1
    c->bprow = c->width;
#else
    c->bprow = (c->width+15)&0xfffffff0;
#endif
    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);
    c->right = c->RealWidth - c->width - c->left;
    c->top = c->RealHeight - c->height - c->bottom;
#if 1
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
#else
#ifndef __PPC__
    if (!(c->BackArray = AllocVec(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
	return(FALSE);
    c->FrontArray = c->BackArray;
    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (!(c->FrontArray = AllocVec(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR)))
		return(FALSE);
    }
#else
    if (!(c->BackArray = AllocVecPPC(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
	return(FALSE);
    c->FrontArray = c->BackArray;
    if (c->flags & FLAG_TWOBUFFERS)
    {
	if (!(c->FrontArray = AllocVecPPC(((c->width+15)&0xfffffff0)*c->height,MEMF_PUBLIC|MEMF_CLEAR,0)))
		return(FALSE);
    }
#endif
#endif
    c->DrawBuffer = c->BackArray;
    c->ReadBuffer = c->BackArray;
    c->drawbufferflag = 0; /* back buffer */
    c->readbufferflag = 0; /* back buffer */
    c->backarrayflag = 0; /* back array really points to back buffer */
#ifndef __PPC__
    if (!(c->temprp = AllocVec(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR)))
#else
    if (!(c->temprp = AllocVecPPC(sizeof(struct RastPort),MEMF_PUBLIC|MEMF_CLEAR,0)))
#endif
	return(FALSE);
    InitRastPort(c->temprp);
    if (!(c->temprp->BitMap = AllocBitMap((((c->width+15)>>4)<<4),1,c->depth,0,c->rp->BitMap)))
	return(FALSE);
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
    c->InitDD = (void(*)(void*))SWDriver_DD_pointers;
    c->Dispose = SWDriver_Dispose;
    c->SwapBuffer = SWDriver_SwapBuffer_AGA;
    return(TRUE);
}
;;//
;// SWDriver_init

/*
 *
 *   Driver startup  -  called for AGA and gfxboard drivers
 *
 */


BOOL SWDriver_init(struct amigamesa_context *c,struct TagItem *tagList)
    {
#ifdef DBG
    LibPrintf("SWDriver_init\n");
#endif
    if (CyberGfxBase)
	if (IsCyberModeID(GetVPModeID(&c->Screen->ViewPort)))
	    return(LaunchGfxboardDriver(c,tagList));
    return(LaunchAGADriver(c,tagList));
}
;;//



