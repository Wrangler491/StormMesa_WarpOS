/*
 * Amiga GLUT graphics library toolkit
 * Version:  1.1
 * Copyright (C) 1998 Jarno van der Linden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * glutCreateWindow.c
 *
 * Version 1.0  27 Jun 1998
 * by Jarno van der Linden
 * jarno@kcbbs.gen.nz
 *
 * Version 1.1  02 Aug 1998
 * by Jarno van der Linden
 * jarno@kcbbs.gen.nz
 *
 * - Quantizer plugin support added
 *
 */

#include <stdlib.h>
#include <stdio.h>
#ifndef WARPUP
#include <inline/intuition.h>
#include <inline/gadtools.h>
#include <inline/layers.h>
#include <libraries/gadtools.h>
#else
#pragma pack(push,2)
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/layers.h>
#include <libraries/gadtools.h>
#pragma pack(pop)
#endif
#include "glutstuff.h"
#include <GL/Amigamesa.h>

#include "../sources/AmigaIncludes.h"
//#define LibPrintf(t) SPrintF(t,NULL)

extern VOID CloseWindowSafely(struct Window *win);

#ifdef WARPUP
#pragma pack(push,2)
#endif
struct NewMenu defmenu[] =
{
  {NM_TITLE, "Left Menu", 0, 0, 0, 0,},
  {NM_TITLE, "Middle Menu", 0, 0, 0, 0,},
  {NM_TITLE, "Right Menu", 0, 0, 0, 0,},
  {NM_END, NULL, 0, 0, 0, 0,}
};
#ifdef WARPUP
#pragma pack(pop)
#endif

int glutCreateSubWindow(int win, int x, int y, int width, int height) {
  struct GlutWindow *gw;
  int newWinID = -1;

          DEBUGOUT(11, "glutCreateSubWindow\n");
  IGNORE_IN_GAME_MODE(newWinID);

  if((gw = stuffGetWin(win))) {
    glutInitWindowPosition(gw->wincurx + x, gw->wincury + y);
    glutInitWindowSize(width, height);

    if ((newWinID = glutCreateWindow("subwindow")))
      stuffLinkInSubWin(gw, stuffGetWin(newWinID));
    else {
      DEBUGOUT(1, "failed to open sub-window\n");
      newWinID = -1;
    }
    
    DEBUGOUT(2, "%d = glutCreateSubWindow(%d, %d, %d, %d, %d)\n", newWinID, win, x, y, width, height);
  }
  else
    DEBUGOUT(1, "cannot subwindows parent\n");

  return newWinID;
}

int glutCreateWindow(const char *title)
{
  struct GlutWindow *gw;

          DEBUGOUT(11, "glutCreateWindow\n");
#ifndef WARPUP
  if ((gw = AllocVecPooled(glutPool, sizeof(struct GlutWindow)))) {
#else
  if ((gw = AllocVecPPC(sizeof(struct GlutWindow),MEMF_PUBLIC|MEMF_CLEAR,0))) {
#endif
    struct Window *win;
    struct Screen *screen;
    WORD zoom[4];

    screen = LockPubScreen(glutstuff.pubscreenname);
    zoom[0] = glutstuff.initposx;
    zoom[1] = glutstuff.initposy;
    zoom[2] = glutstuff.initwidth;
    zoom[3] = glutstuff.initheight;

    win = gw->window = OpenWindowTags(NULL,
				WA_Title, (ULONG)title,
				WA_PubScreen, (ULONG)screen,
				WA_Left, glutstuff.initposx,
				WA_Top, glutstuff.initposy,
				WA_InnerWidth, glutstuff.initwidth,
				WA_InnerHeight, glutstuff.initheight,
				WA_MinWidth, 32,
				WA_MinHeight, 32,
				WA_MaxWidth, ~0,
				WA_MaxHeight, ~0,
				WA_Flags, WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_NOCAREREFRESH | WFLG_CLOSEGADGET | WFLG_DEPTHGADGET | WFLG_SIZEBBOTTOM | ((glutstuff.doublemode) ? WFLG_SIMPLE_REFRESH : WFLG_NOCAREREFRESH),
				WA_Activate, TRUE,		//Changed by Wrangler		/* only by makecurrent */
				WA_Zoom, (ULONG)zoom,
				TAG_END);
    UnlockPubScreen(NULL, screen);

    if (win) {
      win->UserPort = glutstuff.msgport;

#ifdef USE_CLIP_LAYER_
      gw->clipreg = clipWindowToBorders(win);
#endif

      if ((gw->context = AmigaMesaCreateContextTags(
							//AMA_Window, win,
						    AMA_RGBMode, (glutstuff.rgbamode ? (glutstuff.rgbamode < 0 ? GL_FALSE : GL_TRUE) : glutstuff.rgba),
						    AMA_AlphaFlag, glutstuff.alpha,
						    //AMA_Stereo, (glutstuff.stereomode ? (glutstuff.stereomode < 0 ? GL_FALSE : GL_TRUE) : glutstuff.stereo),
						    AMA_DoubleBuf, (glutstuff.doublemode ? (glutstuff.doublemode < 0 ? GL_FALSE : GL_TRUE) : glutstuff.db),
							AMA_NoDepth, (glutstuff.depth ? GL_FALSE : GL_TRUE),
							AMA_NoStencil, (glutstuff.stencil ? GL_FALSE : GL_TRUE),
							AMA_NoAccum, (glutstuff.accum ? GL_FALSE : GL_TRUE),
						    //AMA_PaletteMode, glutstuff.palmode,
						    AMA_RastPort, win->RPort,
						    AMA_Screen, win->WScreen,
						    AMA_Left, win->BorderLeft,
						    AMA_Bottom, win->BorderBottom,
						    AMA_Width, glutstuff.initwidth,
						    AMA_Height, glutstuff.initheight,
							AMA_DirectRender, GL_TRUE,
						    TAG_END))) {
	if (ModifyIDCMP(win, IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MENUPICK | IDCMP_MOUSEBUTTONS | IDCMP_INTUITICKS | IDCMP_CHANGEWINDOW)) {
	  DEBUGOUT(11,"CreateWindow step 1\n");

LibPrintf("** Created amigamesa_context with addr:\n"); // 0x%08x\n",gw->context);
	  nNewList(&gw->SubWindows);
	  nNewList(&gw->WindowTimers);

	  win->UserData = (APTR)gw;
	  gw->vi = GetVisualInfo(win->WScreen, TAG_END);
	  DEBUGOUT(11,"CreateWindow step 2\n");

	  if ((gw->menu = CreateMenus(defmenu, TAG_END))) {
	    if(LayoutMenus(gw->menu, gw->vi, TAG_END)) {
	    SetMenuStrip(win, gw->menu);
		} else {
			DEBUGOUT(1,"create menu failed\n");
		}
	  }
	  else
	    DEBUGOUT(1, "failed to create menu\n");

	  gw->cursor = GLUT_CURSOR_INHERIT;
	  gw->mousex = -1;
	  gw->mousey = -1;

	  gw->winx = gw->wincurx = (uint16_t)win->LeftEdge;
	  gw->winy = gw->wincury = (uint16_t)win->TopEdge;
LibPrintf("InnerWidth: \n"); //%d, width: %d, left: %d, right: %d \n",InnerWidth(win), win->Width, win->BorderLeft, win->BorderRight);
LibPrintf("InnerHeight: \n"); //%d, height: %d, top: %d, bottom: %d \n",InnerHeight(win), win->Height, win->BorderTop, win->BorderBottom);
	  gw->winwidth = InnerWidth(win);
	  gw->winheight =  InnerHeight(win);
	  gw->wincurwidth = gw->winwidth;
	  gw->wincurheight = gw->winheight;
LibPrintf("Winwidthxheight: \n"); //%d x %d, curwidthxcurheight: %d x %d\n",gw->winwidth,gw->winheight,gw->wincurwidth,gw->wincurheight);

	  gw->needreshapegui = TRUE;
	  gw->needpositiongui = TRUE;
	  gw->needredisplay = TRUE;
	  gw->needvisibility = TRUE;
	  gw->visible = TRUE;
	  gw->needleftmenu = TRUE;
	  gw->needmiddlemenu = TRUE;
	  gw->needrightmenu = TRUE;
	  DEBUGOUT(11,"CreateWindow step 3\n");

		LibPrintf("Linking in gw: \n"); //0x%08x\n",gw);
	  stuffLinkInWin(gw);
	  DEBUGOUT(10, "glutCreateWindow about to call glEnable\n");

	//glClear(  GL_COLOR_BUFFER_BIT );
	  if (glutstuff.depth) //{
	    glEnable(GL_DEPTH_TEST);
	//	  glClear( GL_DEPTH_BUFFER_BIT ); }

	//ActivateWindow(win);
	//glutstuff.activeWindow = gw;
	  DEBUGOUT(2, "%d = glutCreateWindow(`%s')\n", gw->WinID, title);
LibPrintf("Winwidth as we leave CreateWindow: \n"); //%d x %d\n",gw->winwidth,gw->winheight);
LibPrintf("First lw in gw: \n"); // 0x%08x\n",*gw);

	  return gw->WinID;
	}

	DEBUGOUT(1, "failed to modify internal window-state\n");
	AmigaMesaDestroyContext(gw->context);
      }
      else
        DEBUGOUT(1, "failed to create context\n");

      CloseWindowSafely(win);
    }
    else
      DEBUGOUT(1, "failed to open window\n");

#ifndef WARPUP
    FreeVecPooled(glutPool, (ULONG *)gw);
#else
    FreeVecPPC((ULONG *)gw);
#endif
  }
  else
    DEBUGOUT(1, "failed to allocate window\n");

  return 0;
}
