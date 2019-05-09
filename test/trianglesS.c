/*
 * triangles.c
 *
 * Modified  27 Jun 1998
 * by Jarno van der Linden
 * jarno@kcbbs.gen.nz
 *
 * Based on triangles.c from CyberGL
 * Changes to work with AmigaMesaRTL
 *
 * Original copyright notice follows:
 */

/*
 *   $VER: triangles.c 1.0 (20.03.1997)
 *
 *   This is an example program for CyberGL
 *
 *      Written by Frank Gerberding
 *
 *   Copyright © 1996-1997 by phase5 digital products
 *      All Rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <sys/time.h>
#ifdef WARPUP
#pragma pack(push,2)
#endif
#include <intuition/intuition.h>

#ifndef WARPUP
#include <inline/exec.h>
#include <inline/graphics.h>
#include <inline/intuition.h>
#else
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <cybergraphics/cybergraphics.h>
#include <proto/cybergraphics.h>
#endif

extern struct ExecBase *SysBase;
extern struct DOSBase *DOSBase;
#ifndef WARPUP
struct Library *IntuitionBase;
#else
struct IntuitionBase *IntuitionBase;
struct Library *CyberGFxBase;
#endif
struct Screen *screen;
struct RastPort *rpptr;

#ifdef WARPUP
#pragma pack(pop)
#endif

#include <GL/Amigamesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
AmigaMesaContext context;

#define WIDTH    320
#define HEIGHT   240

void drawTriangles(AmigaMesaContext context, int num)
{
  struct amigamesa_buffer *buffer = NULL;
  int count;
  struct timeval startTime, stopTime;
  double secs;

  /* we have changed the context (maybe the buffer too, so
   * make it the current again
   */
  //AmigaMesaGetContextTags(context, AMA_Buffer, &buffer, TAG_DONE);
	buffer = context->buffer;
  if (buffer)
    AmigaMesaMakeCurrent(context, buffer);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DITHER);
  glShadeModel(GL_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-400.0, 400.0, -300.0, 300.0, 500.0, -500.0);

  srand(42);

  GetSysTimePPC(&startTime);	
  //gettimeofday(&startTime, NULL);
  for (count = 0; count < num; count++) {
    glBegin(GL_TRIANGLES);
    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    glEnd();
  }
  glFlush();
	AmigaMesaSwapBuffers(context);
  GetSysTimePPC(&stopTime);
  //gettimeofday(&stopTime, NULL);
  
  secs  = (double)stopTime.tv_micro  / 1000000 + stopTime.tv_secs ;
  secs -= (double)startTime.tv_micro / 1000000 + startTime.tv_secs;
  if (secs == 0.0)
    secs = 0.1;

  printf("%g triangles/s (%g secs)\n", (double)num / secs, secs);
}
/*
void drawTrianglesIndexModes2(AmigaMesaContext context, int num) {
  AmigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_GREY_MATCH, TAG_DONE);
  printf("   PaletteMode : grey\n");
  drawTriangles(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_TRUECOLOR_MATCH, TAG_DONE);
  printf("   PaletteMode : color match\n");
  drawTriangles(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_WEIGHTED_MATCH, TAG_DONE);
  printf("   PaletteMode : weighted color\n");
  drawTriangles(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_HPCR_MATCH, TAG_DONE);
  printf("   PaletteMode : hpcr color\n");
  drawTriangles(context, num);
}

void drawTrianglesIndexModes1(AmigaMesaContext context, int num) {
  AmigaMesaChangeContextTags(context, AMA_PaletteCache, GL_FALSE, TAG_DONE);
  printf("  PaletteCache : off\n");
  drawTrianglesIndexModes2(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteCache, GL_TRUE, TAG_DONE);
  printf("  PaletteCache : on\n");
  drawTrianglesIndexModes2(context, num);
}

void drawTrianglesIndexModes0(AmigaMesaContext context, int num) {
  AmigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_TRUECOLOR_SHIFT, TAG_DONE);
  printf(" PaletteDither : off\n");
  printf("  PaletteCache : off\n");
  printf("   PaletteMode : color shift\n");
  drawTriangles(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteDither, GL_FALSE, TAG_DONE);
  printf(" PaletteDither : off\n");
  drawTrianglesIndexModes1(context, num);

  AmigaMesaChangeContextTags(context, AMA_PaletteDither, GL_TRUE, TAG_DONE);
  printf(" PaletteDither : on\n");
  drawTrianglesIndexModes1(context, num);
}*/

void drawTrianglesRGBAModes(AmigaMesaContext context, int num) {
  printf("   PaletteMode : off\n");
  drawTriangles(context, num);
}

void exitT(void) {
}

int main(int argc, char **argv)
{
  //atexit(exitT);
  uint16_t depth;
	ULONG modeID = INVALID_ID;


  if ((IntuitionBase = OpenLibrary("intuition.library", 1))) {
		printf("Opened intuition.library\n");
	  if((CyberGfxBase=OpenLibrary("cybergraphics.library",0))) {
		printf("Opened cybergraphics.library\n");	
	   modeID = BestCModeIDTags(CYBRBIDTG_NominalWidth, WIDTH,
				CYBRBIDTG_NominalHeight, HEIGHT,
				CYBRBIDTG_Depth, 16,
				TAG_END);
		printf("ModeID: 0x%08x\n",modeID);
		if(modeID == INVALID_ID) {
			printf("Couldn't find mode\n");
			CloseLibrary(IntuitionBase);
			return 0;
		}
		//modeID = 0x50001100;
		if ((screen = OpenScreenTags(NULL,
					 SA_Width, WIDTH,
					 SA_Height, HEIGHT,
					 SA_Depth, 16,
					 SA_SysFont, 1,
					 //SA_Type, PUBLICSCREEN,
					 //SA_ShowTitle, TRUE,
					 //SA_PubName, "Triangles",
					 //SA_Title, "Triangles",
					 SA_ShowTitle,FALSE,
					 SA_Quiet,TRUE,
					 SA_DisplayID, modeID,
					 TAG_END))) {
				printf("Opened screen\n");
				printf("Is Cybergfx mode? %d\n",IsCyberModeID(GetVPModeID(&screen->ViewPort)));
				rpptr = &screen->RastPort;
				printf("Triangles found screen ptr: 0x%08x\n",screen);
				printf("Triangles found rastport ptr: 0x%08x\n",rpptr);
				printf("Screen depth is: %d\n",GetCyberMapAttr(screen->RastPort.BitMap,CYBRMATTR_DEPTH));
				printf("Should equal: %d\n",GetCyberMapAttr(rpptr->BitMap,CYBRMATTR_DEPTH));

				if ((context = AmigaMesaCreateContextTags(AMA_RastPort, rpptr,
								AMA_Screen, (ULONG)screen,
								AMA_Width, WIDTH,
								AMA_Height, HEIGHT,
								AMA_RGBMode, GL_TRUE,
								AMA_Fullscreen, GL_TRUE,
								TAG_END))) {
					depth = GetBitMapAttr (screen->RastPort.BitMap, BMA_DEPTH);
					printf("Screen depth is: %d\n",depth);
					//if (window->WScreen->RastPort.BitMap->Depth <= 8)
					//if(depth<=8)
					//  drawTrianglesIndexModes0(context, argc == 2 ? atoi(argv[1]) : 500);
					//else
						drawTrianglesRGBAModes(context, argc == 2 ? atoi(argv[1]) : 500);
				 } else {
					printf("Cannot open context\n");
				 }
			} else {
			  printf("Cannot open screen\n");
			}
	  } else {
		  printf("No Cybergraphx!\n");
	  }
  }
    if (context) {
	printf("Closing context\n");
    AmigaMesaDestroyContext(context);
	}
  if (screen) {
	printf("Closing screen\n");
    CloseScreen(screen);
	}
  if (CyberGfxBase) {
	printf("Closing cgx library\n");
	CloseLibrary(CyberGfxBase);
	}
  if (IntuitionBase) {
	printf("Closing library\n");
    CloseLibrary(IntuitionBase);
	}
  return 0;
}
