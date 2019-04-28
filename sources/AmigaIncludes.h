
#ifdef WARPUP
#pragma pack(push,2)
#endif

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/timer.h>
#include <proto/asl.h>
#include <proto/cybergraphics.h>
#ifdef WARPUP
#include <Warp3D/Warp3D.h>
#include <Warp3D/Warp3D_protos.h>
#else
#include <proto/Warp3D.h>
#endif
#include <cybergraphics/cybergraphics.h>

/* all the "xxxxBases" are in the LIBRARY_amigaos_inits.c */

extern struct ExecBase *SysBase;
extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
extern struct Library *GadToolsBase;
extern struct Library *CyberGfxBase;

#ifndef __PPC__
extern struct Library *Warp3DBase; 
extern struct Device *TimerBase;
#else
extern struct Library *Warp3DPPCBase; 
extern struct Library *TimerBase;
#endif

#ifdef WARPUP
#pragma pack(pop)
#endif

/* stack functions calls only for GCC to avoid the error "fixed or forbidden register was spilled" */
ULONG STACKW3D_UpdateTexSubImage(W3D_Context *context, W3D_Texture *texture, void *teximage,ULONG level, ULONG *palette, W3D_Scissor* scissor, ULONG srcbpr);
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat );
ULONG STACKReadPixelArray(APTR  destRect, UWORD  destX,UWORD  destY, UWORD  destMod, struct RastPort *  RastPort, UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat );

#define STORMMESADEBUG 1

void LibPrintf(const char *string, ...);
/*==================================================================================*/
#ifdef STORMMESADEBUG
#define REM(message) printf(#message"\n");	//was LibPrintf
#define SFUNCTION(n) STORMMESA_Function(n);
/*==================================================================================*/
#else
#define REM(message) ;
#define SFUNCTION(n) ;
#endif  
/*==================================================================================*/