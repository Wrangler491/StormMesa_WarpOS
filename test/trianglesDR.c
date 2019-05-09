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
struct Window *window;


#ifdef WARPUP
#pragma pack(pop)
#endif

#include <GL/Amigamesa.h>
#include <GL/gl.h>
//#include <GL/glu.h>

AmigaMesaContext context;
#define WIDTH    300
#define HEIGHT   200

void handle_window_events(struct Window *window)
{
  struct IntuiMessage *msg;
  int done = 0;

  while (!done) {
    Wait(1L << window->UserPort->mp_SigBit);
    while ((!done) && (msg = (struct IntuiMessage *)GetMsg(window->UserPort))) {
      switch (msg->Class) {
	case IDCMP_CLOSEWINDOW:
	  done = 1;
	default:
	  break;
      }
      ReplyMsg((struct Message *)msg);
    }
  }
}

void drawTriangles(AmigaMesaContext context, int num)
{
  struct amigamesa_buffer *buffer = NULL;
  int count;
  struct timeval startTime, stopTime, midTime1, midTime2;
  double secs, totBegin, totColor, totVertex, totEnd;

  /* we have changed the context (maybe the buffer too, so
   * make it the current again
   */
  //amigaMesaGetContextTags(context, AMA_Buffer, &buffer, TAG_DONE);
	buffer = context->buffer;
  if (buffer)
    AmigaMesaMakeCurrent(context, buffer);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DITHER);
  glShadeModel(GL_SMOOTH);
  //glClearColor( 0, .250, 0, 1 );  // (In fact, this is the default.)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  smglOrtho(-400.0, 400.0, -300.0, 300.0, 500.0, -500.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-400.0, 400.0, -300.0, 300.0, 500.0, -500.0);

  srand(42);
  totBegin=totColor=totVertex=totEnd=0;

  GetSysTimePPC(&startTime);
  //gettimeofday(&startTime, NULL);
  midTime1 = startTime;
  for (count = 0; count < num; count++) {
    glBegin(GL_TRIANGLES);
    	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totBegin += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totBegin -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
    	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totColor += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totColor -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totVertex += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totVertex -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totColor += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totColor -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totVertex += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totVertex -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totColor += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totColor -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glVertex3i(rand() % 800 - 400, rand() % 600 - 300, rand() % 1000 - 500);
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totVertex += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totVertex -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;

    glEnd();
	GetSysTimePPC(&midTime2);
	//gettimeofday(&midTime2, NULL);
	totEnd += (double)midTime2.tv_micro / 1000000 + midTime2.tv_secs;
	totEnd -= (double)midTime1.tv_micro / 1000000 + midTime1.tv_secs;
	midTime1 = midTime2;
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
  printf("glBegin() took %g secs\n",totBegin);
  printf("glColor() took %g secs\n",totColor);
  printf("glVertex3i() took %g secs\n", totVertex);
  printf("glEnd() took %g secs\n",totEnd);
  printf("which sums to %g secs\n",totBegin+totColor+totVertex+totEnd);
  
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
}
*/
void drawTrianglesRGBAModes(AmigaMesaContext context, int num) {
  printf("   PaletteMode : off\n");
  drawTriangles(context, num);
}

void exitT(void) {

}

int main(int argc, char **argv)
{
  //texit(exitT);
  uint16_t depth;

  if ((IntuitionBase = OpenLibrary("intuition.library", 1))) {
    if (!(screen = LockPubScreen("Mesa")))
      screen = LockPubScreen(NULL);
    if ((window = OpenWindowTags(NULL,
				 WA_InnerWidth, WIDTH,
				 WA_InnerHeight, HEIGHT,
				 WA_Title, "Triangles",
				 WA_PubScreen, screen,
				 //WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY,
				 WA_CloseGadget, TRUE,
				 WA_DepthGadget, TRUE,
				 WA_DragBar, TRUE,
				 WA_Activate, TRUE,
				 TAG_END))) {
			printf("Triangles found window ptr: 0x%08x\n",window);
			printf("Triangles found screen ptr: 0x%08x\n",window->WScreen);
      if ((context = AmigaMesaCreateContextTags( AMA_Window, window,
						AMA_RastPort, (unsigned long)window->RPort,
						AMA_Screen, (unsigned long)window->WScreen,
						AMA_Left, window->BorderLeft,
						AMA_Bottom, window->BorderBottom,
						AMA_Width, WIDTH,
						AMA_Height, HEIGHT,
						AMA_RGBMode, GL_TRUE,
						AMA_DirectRender, GL_TRUE,
						TAG_END))) {
	printf("AMCCT returned\n");
	depth = GetBitMapAttr (window->WScreen->RastPort.BitMap, BMA_DEPTH);
	printf("Screen depth is: %d\n",depth);
	//if (window->WScreen->RastPort.BitMap->Depth <= 8)
	/*if(depth<=8)
	  drawTrianglesIndexModes0(context, argc == 2 ? atoi(argv[1]) : 500);
	else*/
	  drawTrianglesRGBAModes(context, argc == 2 ? atoi(argv[1]) : 500);

printf("Done\n");
	//handle_window_events(window);
printf("Clicked close\n");
      }
printf("Over\n");
    }
  }
   if (context) {
	printf("Closing context\n");
    AmigaMesaDestroyContext(context);
	}
  if (window) {
	printf("Closing window\n");
    CloseWindow(window);
	}
  if (screen) {
	printf("Unlocking screen\n");
    UnlockPubScreen(NULL, screen);
	}
  if (IntuitionBase) {
	printf("Closing library\n");
    CloseLibrary(IntuitionBase);
	} 
  return 0;
}
