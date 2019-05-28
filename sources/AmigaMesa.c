/* $Id: AmigaMesa.c 1.16 1997/06/25 19:16:56 StefanZ Exp StefanZ $ */

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
//#define STATIC_ 1

#include "AmigaIncludes.h"
#include "AmigaPrefs.h"
extern struct STORMMESA_parameters StormMesa;

#include <stdlib.h>
#include <gl/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"
#include "misc.h"
#include "types.h"

#include "AmigaMesa.h"
#include "ADisp_Cyb.h"
#include "ADisp_SW.h"
#include "ADisp_SWFS.h"
#include "ADisp_HW.h"
#include "htnew_color.h"

//#define DEBUGPRINT

#ifdef DEBUGPRINT
#define DEBUGOUT(x) printf(x);	//LibPrintf(x);
#else
#define DEBUGOUT(x) /*LibPrintf(x);*/
#endif

#ifndef __PPC__
#define FREEVEC(mem) FreeVec(mem)
#else
#define FREEVEC(mem) FreeVecPPC(mem)
#endif

#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

#ifdef WARPUP
#pragma pack(push,2)
struct Library* P96APIBase;
#pragma pack(pop)
#endif

#define MAXTAGS 100
long tagStore[MAXTAGS*2]={0};

/**********************************************************************/
/*****                  Amiga/Mesa API Functions                  *****/
/**********************************************************************/
/*
 * Implement the client-visible Amiga/Mesa interface functions defined
 * in Mesa/include/GL/Amigamesa.h
 *
 **********************************************************************/
/*
 * Implement the public Amiga/Mesa interface functions defined
 * in Mesa/include/GL/AmigaMesa.h
 */
 
struct amigamesa_visual *
AmigaMesaCreateVisualTags(long Tag1, ...)
{
	return AmigaMesaCreateVisual((struct TagItem *)&Tag1);
}

#ifdef __GNUC__
#ifdef WARPUP
struct amigamesa_visual * 
AmigaMesaCreateVisual(struct TagItem *tagList)
#else
struct amigamesa_visual * 
AmigaMesaCreateVisual(struct TagItem *tagList __asm("a0"))
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY struct amigamesa_visual *
AmigaMesaCreateVisual(register __a0 struct TagItem *tagList)
#else
#ifndef __PPC__
struct amigamesa_visual * APIENTRY
AmigaMesaCreateVisual(register __a0 struct TagItem *tagList)
#else
struct amigamesa_visual * APIENTRY
AmigaMesaCreateVisual(struct TagItem *tagList)
#endif
#endif
#endif
{
   struct amigamesa_visual *v;
   int  index_bits;
   GLint redbits, greenbits, bluebits, alphabits;

	REM(AmigaMesaCreateVisual);


#ifndef __PPC__
   v = (struct amigamesa_visual *)AllocVec(sizeof(struct amigamesa_visual),MEMF_PUBLIC|MEMF_CLEAR);
#else
   v = (struct amigamesa_visual *)AllocVecPPC(sizeof(struct amigamesa_visual),MEMF_PUBLIC|MEMF_CLEAR,0);
#endif
   if (!v) {
	  return NULL;
   }

printf("AMCV, visual located at: 0x%08x\n",v);
	v->rgb_flag=GetTagData(AMA_RGBMode,GL_TRUE,tagList);
	v->db_flag=GetTagData(AMA_DoubleBuf,GL_FALSE,tagList);
	v->alpha_flag=GetTagData(AMA_AlphaFlag,GL_FALSE,tagList);


   if (v->rgb_flag) {
	  /* RGB(A) mode */
	  index_bits = 0;
	  redbits = greenbits = bluebits = 8;
	  alphabits = 8;
   }
   else {
	  /* color index mode */
	  index_bits = 8;           /* @@@ TODO */
	  redbits = greenbits = bluebits = alphabits = 0;
   }
   v->depth = 16;
   if (GetTagData(AMA_NoDepth,GL_FALSE,tagList))
	v->depth = 0;
   v->stencil = 8;
   if (GetTagData(AMA_NoStencil,GL_FALSE,tagList))
	v->stencil = 0;
   v->accum = 16;
   if (GetTagData(AMA_NoAccum,GL_FALSE,tagList))
	v->accum = 0;
printf("AMCV RGBMode: %d, DB: %d, alpha: %d\n",v->rgb_flag,v->db_flag,v->alpha_flag);

   /* Create core visual */
   v->gl_visual = (void *)gl_create_visual( v->rgb_flag,
				    v->alpha_flag,
				    v->db_flag,
				    GL_FALSE,
				    v->depth,        /* depth_size */
				    v->stencil,         /* stencil_size */
				    v->accum,        /* accum_size */
				    index_bits,
				    redbits, greenbits,
				    bluebits, alphabits);

   return v;
}

#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaDestroyVisual(register struct amigamesa_visual *v)
#else
void AmigaMesaDestroyVisual(register struct amigamesa_visual *v __asm("a0"))
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaDestroyVisual(register __a0 struct amigamesa_visual *v )
#else
#ifndef __PPC__
void APIENTRY AmigaMesaDestroyVisual(register __a0 struct amigamesa_visual *v )
#else
void APIENTRY AmigaMesaDestroyVisual(struct amigamesa_visual *v )
#endif
#endif
#endif
{
	REM(AmigaMesaDestroyVisual);

   gl_destroy_visual( (GLvisual *)v->gl_visual );
   FREEVEC(v);
}




struct amigamesa_buffer *AmigaMesaCreateBuffer( struct amigamesa_visual *visual,int windowid)
{
   struct amigamesa_buffer *b;

	REM(AmigaMesaCreateBuffer);


#ifndef __PPC__
   b = (struct amigamesa_buffer *) AllocVec(sizeof(struct amigamesa_buffer),MEMF_PUBLIC|MEMF_CLEAR);
#else
   b = (struct amigamesa_buffer *) AllocVecPPC(sizeof(struct amigamesa_buffer),MEMF_PUBLIC|MEMF_CLEAR,0);
#endif
   if (!b) {
	  return NULL;
   }

printf("AMCB, buffer located at: 0x%08x\n",b);
   b->gl_buffer = (void *)gl_create_framebuffer( (GLvisual *)visual->gl_visual);

   /* other stuff */

   return b;
}



void AmigaMesaDestroyBuffer( struct amigamesa_buffer *b )
{
	REM(AmigaMesaDestroyBuffer);

	gl_destroy_framebuffer( (GLframebuffer *)b->gl_buffer );
   FREEVEC(b);

}




/* this small function checks if P96 is running */

/* currently not used

int DetectP96(void)
{
	struct Node *current;

	Forbid();
	current = ((struct ExecBase *)SysBase)->LibList.lh_Head;
	while (current->ln_Succ != 0)
	{
		if (!(strcmp("rtg.library",current->ln_Name)))
		{
			Permit();
			return(TRUE);
		}
		current = current->ln_Succ;
	}
	Permit();
	return(FALSE);
}

*/

/*wawa
#ifndef __PPC__
ULONG SetFPU(void);
void RestoreFPU(ULONG);
#endif
wawa*/

struct amigamesa_context *AmigaMesaCreateContextTags(long Tag1, ...)
{
	va_list tagList;
	int n;
	ULONG val1, val2;
	
	va_start (tagList, Tag1);
	val1 = Tag1;
	tagStore[0] = val1;

	for(n=1;n<MAXTAGS*2;n+=2) {
		if(val1==TAG_END)
			break;
		val2 = va_arg(tagList, long);
		val1 = va_arg(tagList, long);
		tagStore[n] = val2;
		tagStore[n+1] = val1;
	}

	va_end(tagList);
	if(val1!=TAG_END) {
		printf("Max Tags exceeded!\n");
	}

	return AmigaMesaCreateContext((struct TagItem *)tagStore);
	//return AmigaMesaCreateContext((struct TagItem *)&Tag1);
}
 
#ifdef __GNUC__
struct amigamesa_context *
#ifdef WARPUP
	AmigaMesaCreateContext(struct TagItem *tagList)
#else
#ifdef STATIC_
	AmigaMesaCreateContext(struct TagItem *tagList)
#else
	AmigaMesaCreateContext(register struct TagItem *tagList __asm("a0"))
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY struct amigamesa_context *
AmigaMesaCreateContext(register __a0 struct TagItem *tagList)
#else
#ifndef __PPC__
struct amigamesa_context * APIENTRY
AmigaMesaCreateContext(register __a0 struct TagItem *tagList)
#else
struct amigamesa_context * APIENTRY
AmigaMesaCreateContext(struct TagItem *tagList)
#endif
#endif
#endif
{
	struct amigamesa_context *c;
#ifndef __PPC__
	ULONG oldrnd;
#endif
	char varbuffer[256];


	if(tagList==NULL)			/* It mean use backdoor to StormMesa-Prefs */
		{ STORMMESA_Prefs(); return(NULL); }

	REM(AmigaMesaCreateContext);

    SysBase = OpenLibrary("exec.library",0);
    UtilityBase = OpenLibrary("utility.library",0);
    GfxBase = OpenLibrary("graphics.library",0);
#ifndef __PPC__
	//wawa oldrnd = SetFPU();       //set rounding mode to 'round to zero'
#endif
	/* try to open cybergraphics.library */
	if (CyberGfxBase==NULL)
		{
			CyberGfxBase = OpenLibrary((UBYTE*)"cybergraphics.library",0);
		}

		/* allocate amigamesa_context struct initialized to zeros */
#ifndef __PPC__
	c = (struct amigamesa_context *) AllocVec(sizeof(struct amigamesa_context),MEMF_PUBLIC|MEMF_CLEAR);
#else
	c = (struct amigamesa_context *) AllocVecPPC(sizeof(struct amigamesa_context),MEMF_PUBLIC|MEMF_CLEAR,0);
#endif
	if (!c)
		{
		return(NULL);
		}
	printf("AMCC context located at: 0x%08x ptr to: 0x%08x\n",c,*c);
#ifndef __PPC__
	c->oldFPU = oldrnd;
#endif
	c->window=(struct Window *)GetTagData(AMA_Window,0,tagList);

/* this was moved from the driver to this place here */
	if (!c->window)
	    {
	    c->rp=(struct RastPort *)GetTagData(AMA_RastPort,0,tagList);
	    if (!c->rp)
		{
		printf("No rastport!\n");
		return(FALSE);
		}
	    c->Screen=(struct Screen *)GetTagData(AMA_Screen,0,tagList);
	    if (!c->Screen)
		{
		printf("No screen!\n");
		return(FALSE);
		}
	    }
	else
	{
	    c->rp =c->window->RPort;
	    c->Screen=c->window->WScreen;
	}
if(c->window)
	printf("Window ptr found: 0x%08x, ptr to: 0x%08x\n",c->window,*c->window);
printf("Screen ptr found: 0x%08x, ptr to: 0x%08x\n",c->Screen,*c->Screen);
printf("Rastport ptr found: 0x%08x, ptr to: 0x%08x\n",c->rp,*c->rp);

	if (GetTagData(AMA_Fullscreen,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_FULLSCREEN;
	if (GetTagData(AMA_DirectRender,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_DIRECTRENDER;
	if (GetTagData(AMA_TwoBuffers,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_TWOBUFFERS;
	if (GetTagData(AMA_Forbid3DHW,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_FORBID3DHW;
	if (GetTagData(AMA_Fast,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_FAST;
	if (GetTagData(AMA_VeryFast,GL_FALSE,tagList) == GL_TRUE)
		c->flags = c->flags | FLAG_VERYFAST;

	if (StormMesa.DIRECT.ON)
		c->flags |= FLAG_DIRECTRENDER;
	if (StormMesa.NOHW.ON)
		c->flags |= FLAG_FORBID3DHW;
	if (StormMesa.FAST.ON)
		c->flags |= FLAG_FAST;
	if (StormMesa.VERYFAST.ON)
		c->flags |= FLAG_VERYFAST;
	if (StormMesa.SYNC.ON)
		c->flags |= FLAG_SYNC;
	if (StormMesa.TRIPLE.ON)
		c->flags |= FLAG_TRIPLE;

	c->visual=(struct amigamesa_visual *)GetTagData(AMA_Visual,NULL,tagList);
	c->buffer=(struct amigamesa_buffer *)GetTagData(AMA_Buffer,NULL,tagList);
printf("AMCC part 1\n");

	if(!c->visual)
		{
		if (!(c->visual=AmigaMesaCreateVisual(tagList)))
			{
			return NULL;
			}
		c->flags=c->flags|FLAG_OWNVISUAL;
		}

	if(!c->buffer)
		{
		if(!(c->buffer=AmigaMesaCreateBuffer( c->visual,GetTagData(AMA_WindowID,1,tagList))))
			{
			return NULL;
			}
		c->flags=c->flags|FLAG_OWNBUFFER;
		}

	c->share = NULL;        /* we misuse this field */
	c->gl_ctx = (void *)gl_create_context(  ((GLvisual *)c->visual->gl_visual),NULL,(void *) c,NULL  );
	c->specialalloc = 0;
	if (c->flags & FLAG_FULLSCREEN)
		c->flags &= ~FLAG_DIRECTRENDER;
	if (c->visual->rgb_flag)
		c->flags |= FLAG_RGBA;

printf("AMCC part 2\n");
printf("AMCC hardware forbidden flag: %d\n",(c->flags & FLAG_FORBID3DHW));






/*
 *
 *      Launch gfx drivers
 *
 */

/* First check if 3D hardware support is possible */

/* the following requirements must be satisfied for 3D hardware
   support:
   - availability of cybergraphics.library (CGX or P96)
   - direct rendering or fullscreen mode
   - 3DHW support not forbidden
   - Warp3D available
   - Warp3D hardware driver available resp. supported 3D hardware
     available
   - W3D driver must not be a software driver (might be changed later)
   - compatible destination format
*/

	if ((c->flags & (FLAG_FULLSCREEN|FLAG_DIRECTRENDER)) && (!(c->flags & (FLAG_FORBID3DHW))))
	{

	/* work around for the ramlib stack problem. This can be
	   removed as soon as powerpc.library >13.6 is available,
	   which fixes the problem
	   NOTE: if this is removed, the close code must also be
	   removed */
#ifdef __PPC__
	   struct Library* rtgBase;
	   if (rtgBase = OpenLibrary("rtg.library",0))
	   {
		P96APIBase = OpenLibrary("Picasso96API.library",0);
		CloseLibrary(rtgBase);
	   }
#endif
printf("AMCC part 3\n");

#ifndef __PPC__
		if (Warp3DBase = OpenLibrary("Warp3D.library",2))
#else
		if (Warp3DPPCBase = OpenLibrary("Warp3DPPC.library",2))
#endif
		{

			/* now launch Mesa hardware driver */

				if (HWDriver_init(c,tagList))
				{
					return c;
				}
				else
				{
#ifndef __PPC__
					if (Warp3DBase)		CloseLibrary(Warp3DBase);
#else
					if (Warp3DPPCBase)	CloseLibrary(Warp3DPPCBase);
#endif
					gl_destroy_context( (GLcontext *)c->gl_ctx );
					FREEVEC( c );
					return NULL;
				}
			    
		}
		
	}
printf("AMCC part 4\n");


/* First check if cybergraphics.library available */

	if (CyberGfxBase)
	{
		if ( ((c->flags & FLAG_DIRECTRENDER) && (IsCyberModeID(GetVPModeID(&c->Screen->ViewPort))))
		    || (c->flags & FLAG_FULLSCREEN) )
		{

/* if direct rendering to gfx RAM or fullscreen mode is enabled,
   always call the full screen SW/HW driver */
printf("AMCC part 5: gfx RAM or FS\n");


			if (SWFSDriver_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}

		if(c->visual->db_flag==GL_TRUE)
		{
/* if double buffered, than launch the new gfxboard SW driver (or
   HW driver in future */
printf("AMCC part 6: DB\n");

			if (SWDriver_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}
		else
		{
/* if not double buffered, launch the old gfx board driver */
printf("AMCC part 7: old gfx driver\n");


			if (Cyb_Standard_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}
	}

/* if cybergraphics.library not available, check for further drivers
   insert here alternative drivers (for example EGS).

   if no custom driver found, launch standard AGA compatible driver
   (works also for CGFX LUT 8)
*/
	else
	{

		if (c->flags & FLAG_FULLSCREEN)
		{
printf("AMCC part 8: CGFX 8/AGA driver\n");


/* if full screen mode is enabled, call the full screen SW/HW driver */

			if (SWFSDriver_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}

		if(c->visual->db_flag==GL_TRUE)
		{
/* if double buffered, launch the new AGA compatible SW driver */
printf("AMCC part 9: DB AGA\n");


			if (SWDriver_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}
		else
		{
/* if not double-buffered, launch the old AGA driver */
printf("AMCC part 10: old AGA\n");


			if (Amiga_Standard_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( (GLcontext *)c->gl_ctx );
				FREEVEC(c);
				return NULL;
				}
		}
	}
printf("AMCC end\n");
}


#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaDestroyContext(register struct amigamesa_context *c )
#else
#ifdef STATIC_
void AmigaMesaDestroyContext(register struct amigamesa_context *c )
#else
void AmigaMesaDestroyContext(register struct amigamesa_context *c __asm("a0"))
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaDestroyContext(register __a0 struct amigamesa_context *c )
#else
#ifndef __PPC__
void APIENTRY AmigaMesaDestroyContext(register __a0 struct amigamesa_context *c )
#else
void APIENTRY AmigaMesaDestroyContext(struct amigamesa_context *c )
#endif
#endif
#endif

{

	REM(AmigaMesaDestroyContext...);


#ifdef __PPC__
	/* work around */
	if (P96APIBase)
		CloseLibrary(P96APIBase);
#endif

	/* destroy a Amiga/Mesa context */

	if (c->specialalloc)
		ReleasePen(c->Screen->ViewPort.ColorMap, c->clearpixel);
/*
	if (c==amesa)
		amesa=NULL;
*/

REM(will Dispose);
	(*c->Dispose)( c );

REM(will AmigaMesaDestroyVisual);
	if(c->flags&FLAG_OWNVISUAL)
		AmigaMesaDestroyVisual(c->visual);
REM(will AmigaMesaDestroyBuffer);
	if(c->flags&FLAG_OWNBUFFER)
		AmigaMesaDestroyBuffer(c->buffer);

REM(gl_destroy_context);
	gl_destroy_context( (GLcontext *)c->gl_ctx );
#ifndef __PPC__
	//wawa RestoreFPU(c->oldFPU);           //restore FPU rounding mode
#endif

	FREEVEC(c);
#ifndef __PPC__
	if (Warp3DBase)
		CloseLibrary(Warp3DBase);
#else
	if (Warp3DPPCBase)
		CloseLibrary(Warp3DPPCBase);
#endif
	if (CyberGfxBase)
		CloseLibrary(CyberGfxBase);

	REM(AmigaMesaDestroyContextOK);

}


#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaMakeCurrent(register struct amigamesa_context *amesa,register struct amigamesa_buffer *b )
#else
#ifdef STATIC_
void AmigaMesaMakeCurrent(register struct amigamesa_context *amesa,register struct amigamesa_buffer *b )
#else
void AmigaMesaMakeCurrent(register struct amigamesa_context *amesa __asm("a0"),register struct amigamesa_buffer *b __asm("a1"))
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaMakeCurrent(register __a0 struct amigamesa_context *amesa,register __a1    struct amigamesa_buffer *b )
#else
#ifndef __PPC__
void APIENTRY AmigaMesaMakeCurrent(register __a0 struct amigamesa_context *amesa,register __a1    struct amigamesa_buffer *b )
#else
void APIENTRY AmigaMesaMakeCurrent(struct amigamesa_context *amesa,struct amigamesa_buffer *b )
#endif
#endif
#endif
{
	/* Make the specified context the current one */
	/* the order of operations here is very important! */

	REM(AmigaMesaMakeCurrent);
   if (amesa && b) {
	REM(AMMC: have ctx and buffer);
	    if (amesa->gl_ctx == (void *)gl_get_current_context())
		{
	REM(AMMC: no gl_ctx);
		return;
		}
	    if (amesa->share == NULL)
	      if (!(amesa->visual->rgb_flag))
	      {
		AmigaMesaSetOneColor(amesa,0,0.0,0.0,0.0);
		AmigaMesaSetOneColor(amesa,1,1.0,0.0,0.0);
		AmigaMesaSetOneColor(amesa,2,0.0,1.0,0.0);
		AmigaMesaSetOneColor(amesa,3,1.0,1.0,0.0);
		AmigaMesaSetOneColor(amesa,4,0.0,0.0,1.0);
		AmigaMesaSetOneColor(amesa,5,1.0,0.0,1.0);
		AmigaMesaSetOneColor(amesa,6,0.0,1.0,1.0);
		AmigaMesaSetOneColor(amesa,7,1.0,1.0,1.0);
	     }
	REM(AMMC: call driver init)
	   (*amesa->InitDD)((GLcontext *)amesa->gl_ctx);                            /* Call Driver_init_rutine */
	REM(AMMC: call gl_make_current)
	   gl_make_current( (GLcontext *)amesa->gl_ctx,(GLframebuffer *)b->gl_buffer );
	   if (((GLcontext *)amesa->gl_ctx)->Viewport.Width==0) {
		aglViewport(amesa->gl_ctx, 0, 0, amesa->width, amesa->height );
	   }
	   if (amesa->share == NULL)
	   {
	     if (!(amesa->visual->rgb_flag))
		aglClearIndex(amesa->gl_ctx,0.0);
	     else
		aglClearColor(amesa->gl_ctx,0.0,0.0,0.0,0.0);
	   }
	   amesa->share = (struct amigamesa_context *)1;  /* misuse */
   }
   else
   {
	gl_make_current( NULL,NULL);
   }
	REM(end AmigaMesaMakeCurrent);
}



#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaSwapBuffers(register struct amigamesa_context *amesa)
#else
#ifdef STATIC_
void AmigaMesaSwapBuffers(register struct amigamesa_context *amesa)
#else
void AmigaMesaSwapBuffers(register struct amigamesa_context *amesa __asm("a0"))
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaSwapBuffers(register __a0 struct amigamesa_context *amesa)
#else
#ifndef __PPC__
void APIENTRY AmigaMesaSwapBuffers(register __a0 struct amigamesa_context *amesa)
#else
void APIENTRY AmigaMesaSwapBuffers(struct amigamesa_context *amesa)
#endif
#endif
#endif
	{                /* copy/swap back buffer to front if applicable */
	//REM(AmigaMesaSwapBuffers);

	(*amesa->SwapBuffer)( amesa );
	}


/**********************************************************************/
/***** AmigaMesaSetOneColor                                       *****/
/**********************************************************************/
#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaSetOneColor(struct amigamesa_context *c,int index, float r, float g, float b)
#else
#ifdef STATIC_
void AmigaMesaSetOneColor(struct amigamesa_context *c,int index, float r, float g, float b)
#else
void AmigaMesaSetOneColor(register struct amigamesa_context *c __asm("a0"),register int index __asm("d0"), register float r __asm("fp0"), register float g  __asm("fp1"), register float b  __asm("fp2"))

#endif
#endif //WARPUP

#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaSetOneColor(register __a0 struct amigamesa_context *c,register __d0 int index, register __fp0 float r, register __fp1 float g, register __fp2 float b)
#else
#ifndef __PPC__
void APIENTRY AmigaMesaSetOneColor(register __a0 struct amigamesa_context *c,register __d0 int index, register __fp0 float r, register __fp1 float g, register __fp2 float b)
#else
void APIENTRY AmigaMesaSetOneColor(struct amigamesa_context *c,int index,float r,float g,float b)
#endif
#endif
#endif
{
    unsigned char color;
    unsigned char red = (int)(r*255);
    unsigned char green = (int)(g*255);
    unsigned char blue = (int)(b*255);
	REM(AmigaMesaSetOneColor);

    if (c->visual->rgb_flag)
	return;

    if (c->visual->db_flag)
    {
	if (c->depth <= 8)
	{
	    color = (unsigned char)ObtainBestPen(c->Screen->ViewPort.ColorMap,red<<24,green<<24,blue<<24,OBP_Precision,PRECISION_EXACT,TAG_DONE);
	    (c->ColorTable2[color])++;
	    c->penconv[index] = color;
	    c->penconvinv[color] = index;
	}
	else
	{
	    if ((c->flags & FLAG_FULLSCREEN) || (c->flags & FLAG_DIRECTRENDER))
	    {
		if (c->fmt == PIXFMT_ARGB32)
		    c->ColorTable2[index] = (red << 16) | (green << 8) | blue;
		if (c->fmt == PIXFMT_BGRA32)
		    c->ColorTable2[index] = (blue << 24) | (green << 16) | (red << 8);
		if (c->fmt == PIXFMT_RGBA32)
		    c->ColorTable2[index] = (red << 24) | (green << 16) | (blue << 8);
		if (c->fmt == PIXFMT_RGB24)
		    c->ColorTable2[index] = (red << 16) | (green << 8) | blue;
		if (c->fmt == PIXFMT_BGR24)
		    c->ColorTable2[index] = (blue << 16) | (green << 8) | red;
		if (c->fmt == PIXFMT_RGB16)
		    c->ColorTable2[index] = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
		if (c->fmt == PIXFMT_RGB16PC)
		    c->ColorTable2[index] = ((red >> 3) << 3) | (green >> 5) | ((green >> 2) << 13) | ((blue >> 3) << 8);
		if (c->fmt == PIXFMT_RGB15)
		    c->ColorTable2[index] = ((red >> 3) << 10) | ((green >> 3) << 5) | (blue >> 3);
		if (c->fmt == PIXFMT_RGB15PC)
		    c->ColorTable2[index] = ((red >> 3) << 2) | (green >> 6) | ((green >> 3) << 13) | ((blue >> 3) << 8);
		if (c->fmt == PIXFMT_BGR16PC)
		    c->ColorTable2[index] = ((blue >> 3) << 3) | (green >> 5) | ((green >> 2) << 13) | ((red >> 3) << 8);
		if (c->fmt == PIXFMT_BGR15PC)
		    c->ColorTable2[index] = ((blue >> 3) << 2) | (green >> 6) | ((green >> 3) << 13) | ((red >> 3) << 8);
	    }
	    else
		c->ColorTable2[index] = (red << 16) | (green << 8) | blue;
	}
    }
    else
    {
	if (c->depth>8) {
	    c->ColorTable2[index] = (red << 16) | (green << 8) | blue;
	    ((unsigned long*)c->penconv)[index] = c->ColorTable2[index];
	} else {
	    color = (unsigned char)ObtainBestPen(c->Screen->ViewPort.ColorMap,red<<24,green<<24,blue<<24,OBP_Precision,PRECISION_EXACT,TAG_DONE);
	    (c->ColorTable2[color])++;
	    c->penconv[index] = color;
	    c->penconvinv[color] = index;
	}
    }
}

void AmigaMesaSetRastTags(struct amigamesa_context *c, long Tag1, ...)
{
	AmigaMesaSetRast(c,(struct TagItem *)&Tag1);
}

/**********************************************************************/
/***** AmigaMesaSetRast                                           *****/
/**********************************************************************/
#ifdef __GNUC__
#ifdef WARPUP
void AmigaMesaSetRast(register struct amigamesa_context *c, register struct TagItem* tagList)
#else
#ifdef STATIC_
void AmigaMesaSetRast(register struct amigamesa_context *c, register struct TagItem* tagList)
#else
void AmigaMesaSetRast(register struct amigamesa_context *c __asm("a0"), register struct TagItem* tagList __asm("a1"))
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaSetRast(register __a0 struct amigamesa_context *c, register __a1 struct TagItem* tagList)
#else
#ifndef __PPC__
void APIENTRY AmigaMesaSetRast(register __a0 struct amigamesa_context *c, register __a1 struct TagItem* tagList)
#else
void APIENTRY AmigaMesaSetRast(struct amigamesa_context *c, struct TagItem* tagList)
#endif
#endif
#endif
{
	struct Window* win;
	struct RastPort* rp;

	REM(AmigaMesaSetRast);

	if (c->flags & FLAG_FULLSCREEN)
	{
		return;
	}
	win=(struct Window *)GetTagData(AMA_Window,0,tagList);
	rp=(struct RastPort *)GetTagData(AMA_RastPort,0,tagList);

	if (!win)
	{
	    if (!rp)
	    {
		c->rp = NULL;
		c->front_rp = rp;
	    }
	    else
	    {
		c->rp = rp;
		c->front_rp = rp;
	    }
	}
	else
	{
	    c->window = win;
	    c->rp = win->RPort;
	    c->front_rp = c->rp;
	}
}


/**********************************************************************/
/***** AmigaMesaGetConfig                                           *****/
/**********************************************************************/
#ifdef __GNUC__
#ifdef WARPUP
void APIENTRY AmigaMesaGetConfig(register struct amigamesa_visual *v, register GLenum pname, register GLint* params )
#else
#ifdef STATIC_
void APIENTRY AmigaMesaGetConfig(register struct amigamesa_visual *v, register GLenum pname, register GLint* params )
#else
void APIENTRY AmigaMesaGetConfig(register struct amigamesa_visual *v __asm("a0"), register GLenum pname __asm("d0"), register GLint* params __asm("a1") )
#endif
#endif //WARPUP
#else
#ifndef __STORM__
__asm APIENTRY void AmigaMesaGetConfig(register __a0 struct amigamesa_visual *v, register __d0 GLenum pname, register __a1 GLint* params )
#else
#ifndef __PPC__
void APIENTRY AmigaMesaGetConfig(register __a0 struct amigamesa_visual *v, register __d0 GLenum pname, register __a1 GLint* params )
#else
void APIENTRY AmigaMesaGetConfig(struct amigamesa_visual *v, GLenum pname, GLint* params )
#endif
#endif
#endif
{

	REM(AmigaMesaGetConfig);

	switch (pname)
	{
		case GL_STENCIL_BITS:
			*params = v->stencil;
			break;
		case GL_DEPTH_BITS:
			*params = v->depth;
			break;
		case GL_RED_BITS:
		case GL_GREEN_BITS:
		case GL_BLUE_BITS:
		case GL_ALPHA_BITS:
		case GL_INDEX_BITS:
			*params = 8;
			break;
		case GL_ACCUM_RED_BITS:
		case GL_ACCUM_GREEN_BITS:
		case GL_ACCUM_BLUE_BITS:
		case GL_ACCUM_ALPHA_BITS:
			*params = v->accum;
			break;
		case GL_DOUBLEBUFFER:
			*params = (GLint) v->db_flag;
			break;
		case GL_RGBA_MODE:
			*params = (GLint) v->rgb_flag;
			break;
		case GL_STEREO:
			*params = (GLint) GL_FALSE;
			break;
	}
}

#undef DEBUGPRINT
