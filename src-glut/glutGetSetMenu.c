/*
 * glutGetSetMenu.c
 *
 * Version 1.0  27 Jun 1998
 * by Jarno van der Linden
 * jarno@kcbbs.gen.nz
 *
 */

#include "glutstuff.h"

void glutSetMenu(int menu)
{
  struct GlutMenu *gm;
        DEBUGOUT(11, "glutSetMenu\n");

  if((gm = stuffGetMenu(menu)))
    stuffMakeCurrentMenu(gm);
  else
    DEBUGOUT(1, "somethings wrong in glutSetMenu(%d)\n", menu);
}

int glutGetMenu(void)
{
	        DEBUGOUT(11, "glutGetMenu\n");
  return glutstuff.curmenu ? glutstuff.curmenu->MenuID : 0;
}
