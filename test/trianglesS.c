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
#endif

extern struct ExecBase *SysBase;
extern struct DOSBase *DOSBase;
#ifndef WARPUP
struct Library *IntuitionBase;
#else
struct IntuitionBase *IntuitionBase;
#endif
struct Screen *screen;


#ifdef WARPUP
#pragma pack(pop)
#endif

#include <GL/Amigamesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
AmigaMesaContext context;

#define WIDTH    300
#define HEIGHT   200

void drawTriangles(AmigaMesaContext context, int num)
{
  struct amigamesa_buffer *buffer = NULL;
  int count;
  struct timeval startTime, stopTime;
  double secs;

  /* we have changed the context (maybe the buffer too, so
   * make it the current again
   */
  //amigaMesaGetContextTags(context, AMA_Buffer, &buffer, TAG_DONE);
	buffer = context->buffer;
  if (buffer)
    AmigaMesaMakeCurrent(context, buffer);

  smglEnable(GL_DEPTH_TEST);
  smglEnable(GL_DITHER);
  smglShadeModel(GL_SMOOTH);
  smglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  smglMatrixMode(GL_PROJECTION);
  smglLoadIdentity();

  smglOrtho(-400.0, 400.0, -300.0, 300.0, 500.0, -500.0);

  srand(42);

  gettimeofday(&startTime, NULL);
  for (count = 0; count < num; count++) {
    smglBegin(GL_TRIANGLES);
    smglColor3ub(rand() % 256, rand() % 256, rand() % 256);
    smglVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    smglColor3ub(rand() % 256, rand() % 256, rand() % 256);
    smglVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    smglColor3ub(rand() % 256, rand() % 256, rand() % 256);
    smglVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
    smglEnd();
  }
  smglFlush();
  gettimeofday(&stopTime, NULL);
  
  secs  = (double)stopTime.tv_micro  / 1000000 + stopTime.tv_secs ;
  secs -= (double)startTime.tv_micro / 1000000 + startTime.tv_secs;
  if (secs == 0.0)
    secs = 0.1;

  printf("%g triangles/s (%g secs)\n", (double)num / secs, secs);
}

/*
void drawTrianglesIndexModes2(AmigaMesaContext context, int num) {
  amigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_GREY_MATCH, TAG_DONE);
  printf("   PaletteMode : grey\n");
  drawTriangles(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_TRUECOLOR_MATCH, TAG_DONE);
  printf("   PaletteMode : color match\n");
  drawTriangles(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_WEIGHTED_MATCH, TAG_DONE);
  printf("   PaletteMode : weighted color\n");
  drawTriangles(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_HPCR_MATCH, TAG_DONE);
  printf("   PaletteMode : hpcr color\n");
  drawTriangles(context, num);
}

void drawTrianglesIndexModes1(AmigaMesaContext context, int num) {
  amigaMesaChangeContextTags(context, AMA_PaletteCache, GL_FALSE, TAG_DONE);
  printf("  PaletteCache : off\n");
  drawTrianglesIndexModes2(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteCache, GL_TRUE, TAG_DONE);
  printf("  PaletteCache : on\n");
  drawTrianglesIndexModes2(context, num);
}

void drawTrianglesIndexModes0(AmigaMesaContext context, int num) {
  amigaMesaChangeContextTags(context, AMA_PaletteMode, AMESA_TRUECOLOR_SHIFT, TAG_DONE);
  printf(" PaletteDither : off\n");
  printf("  PaletteCache : off\n");
  printf("   PaletteMode : color shift\n");
  drawTriangles(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteDither, GL_FALSE, TAG_DONE);
  printf(" PaletteDither : off\n");
  drawTrianglesIndexModes1(context, num);

  amigaMesaChangeContextTags(context, AMA_PaletteDither, GL_TRUE, TAG_DONE);
  printf(" PaletteDither : on\n");
  drawTrianglesIndexModes1(context, num);
}
*/
void drawTrianglesRGBAModes(AmigaMesaContext context, int num) {
  printf("   PaletteMode : off\n");
  drawTriangles(context, num);
}

void exitT(void) {
  if (context) {
	printf("Closing context\n");
    AmigaMesaDestroyContext(context);
	}
  if (screen) {
	printf("Closing screen\n");
    CloseScreen(screen);
	}
  if (IntuitionBase) {
	printf("Closing library\n");
    CloseLibrary(IntuitionBase);
	}
}

int main(int argc, char **argv)
{
  atexit(exitT);
  uint16_t depth;

/*	static ULONG colourtable[] = {256l<<16+0,r1,g1,b1,r2,g2,b2,.....0}; 
	struct ColorMap *cm;
	uint8_t r, g, b, n;
	n=0;
	for(r=0; r<4; r++) {
		for(g=0; g<8; g++) {
			for(b=0; b<8; b++) {
				colourtable[n++] = (r<<6) + (g<<3) + b;
			}
		}
	}
*/

  if ((IntuitionBase = OpenLibrary("intuition.library", 1))) {
    if ((screen = OpenScreenTags(NULL,
				 SA_Width, WIDTH,
				 SA_Height, HEIGHT,
				 SA_Depth, 16,
				 SA_SysFont, 1,
				 SA_Type, PUBLICSCREEN,
				 SA_ShowTitle, TRUE,
				 SA_PubName, "Triangles",
				 SA_Title, "Triangles",
				 TAG_END))) {
			printf("Triangles found screen ptr: 0x%08x\n",screen);
			printf("Triangles found rastport ptr: 0x%08x\n",screen->RastPort);
      if ((context = AmigaMesaCreateContextTags(AMA_RastPort, (ULONG)&screen->RastPort,
						AMA_Screen, (ULONG)screen,
						/*AMA_Top, screen->BarHeight + 1,*/
						AMA_Width, WIDTH,
						AMA_Height, HEIGHT,
						AMA_RGBMode, GL_TRUE,
						TAG_END))) {
	depth = GetBitMapAttr (screen->RastPort.BitMap, BMA_DEPTH);
	printf("Screen depth is: %d\n",depth);
	//if (window->WScreen->RastPort.BitMap->Depth <= 8)
	//if(depth<=8)
	  //drawTrianglesIndexModes0(context, argc == 2 ? atoi(argv[1]) : 500);
	//else
	  drawTrianglesRGBAModes(context, argc == 2 ? atoi(argv[1]) : 500);
      }
    }
  }
  
  return 0;
}
