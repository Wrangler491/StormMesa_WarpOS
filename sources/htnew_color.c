/* based on ht_color.s from Szczepan Kuzniarz */
/* modified, bugfixed and accelerated by Sam Jordan */

#include <stdlib.h>
#include <string.h>

#include <intuition/screens.h>
#include "htnew_color.h"
#ifdef WARPUP
#include <proto/intuition.h>
#include <proto/graphics.h>
#else
#ifndef __PPC__
#include <proto/intuition.h>
#include <proto/graphics.h>
#else
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#endif
#endif 

UBYTE DividedBy51Rounded[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

UBYTE DivMod51[512] =
{
  0,  0, 0,  1, 0,  2, 0,  3, 0,  4, 0,  5, 0,  6, 0,  7, 0,  8, 0,  9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15,
  0, 16, 0, 17, 0, 18, 0, 19, 0, 20, 0, 21, 0, 22, 0, 23, 0, 24, 0, 25, 0, 26, 0, 27, 0, 28, 0, 29, 0, 30, 0, 31,
  0, 32, 0, 33, 0, 34, 0, 35, 0, 36, 0, 37, 0, 38, 0, 39, 0, 40, 0, 41, 0, 42, 0, 43, 0, 44, 0, 45, 0, 46, 0, 47,
  0, 48, 0, 49, 0, 50, 1,  0, 1,  1, 1,  2, 1,  3, 1,  4, 1,  5, 1,  6, 1,  7, 1,  8, 1,  9, 1, 10, 1, 11, 1, 12,
  1, 13, 1, 14, 1, 15, 1, 16, 1, 17, 1, 18, 1, 19, 1, 20, 1, 21, 1, 22, 1, 23, 1, 24, 1, 25, 1, 26, 1, 27, 1, 28,
  1, 29, 1, 30, 1, 31, 1, 32, 1, 33, 1, 34, 1, 35, 1, 36, 1, 37, 1, 38, 1, 39, 1, 40, 1, 41, 1, 42, 1, 43, 1, 44,
  1, 45, 1, 46, 1, 47, 1, 48, 1, 49, 1, 50, 2,  0, 2,  1, 2,  2, 2,  3, 2,  4, 2,  5, 2,  6, 2,  7, 2,  8, 2,  9,
  2, 10, 2, 11, 2, 12, 2, 13, 2, 14, 2, 15, 2, 16, 2, 17, 2, 18, 2, 19, 2, 20, 2, 21, 2, 22, 2, 23, 2, 24, 2, 25,
  2, 26, 2, 27, 2, 28, 2, 29, 2, 30, 2, 31, 2, 32, 2, 33, 2, 34, 2, 35, 2, 36, 2, 37, 2, 38, 2, 39, 2, 40, 2, 41,
  2, 42, 2, 43, 2, 44, 2, 45, 2, 46, 2, 47, 2, 48, 2, 49, 2, 50, 3,  0, 3,  1, 3,  2, 3,  3, 3,  4, 3,  5, 3,  6,
  3,  7, 3,  8, 3,  9, 3, 10, 3, 11, 3, 12, 3, 13, 3, 14, 3, 15, 3, 16, 3, 17, 3, 18, 3, 19, 3, 20, 3, 21, 3, 22,
  3, 23, 3, 24, 3, 25, 3, 26, 3, 27, 3, 28, 3, 29, 3, 30, 3, 31, 3, 32, 3, 33, 3, 34, 3, 35, 3, 36, 3, 37, 3, 38,
  3, 39, 3, 40, 3, 41, 3, 42, 3, 43, 3, 44, 3, 45, 3, 46, 3, 47, 3, 48, 3, 49, 3, 50, 4,  0, 4,  1, 4,  2, 4,  3,
  4,  4, 4,  5, 4,  6, 4,  7, 4,  8, 4,  9, 4, 10, 4, 11, 4, 12, 4, 13, 4, 14, 4, 15, 4, 16, 4, 17, 4, 18, 4, 19,
  4, 20, 4, 21, 4, 22, 4, 23, 4, 24, 4, 25, 4, 26, 4, 27, 4, 28, 4, 29, 4, 30, 4, 31, 4, 32, 4, 33, 4, 34, 4, 35,
  4, 36, 4, 37, 4, 38, 4, 39, 4, 40, 4, 41, 4, 42, 4, 43, 4, 44, 4, 45, 4, 46, 4, 47, 4, 48, 4, 49, 4, 50, 5,  0
};

UBYTE Halftone8x8[64] =
{
   0, 25,  6, 31,  1, 27,  7, 33,
  38, 12, 44, 19, 39, 14, 46, 20,
   9, 35,  3, 28, 11, 36,  4, 30,
  47, 22, 41, 25, 49, 23, 43, 17,
   2, 27,  8, 34,  0, 26,  7, 32,
  40, 15, 47, 21, 39, 13, 45, 19,
  11, 37,  5, 31, 10, 35,  3, 29,
  50, 24, 43, 18, 48, 23, 42, 16
};

BOOL AllocColors(int *colors, int *invcolors, struct Screen *Screen)
{
  BOOL fail = FALSE;
  int r, g, b,pen;

  for(r = 0 ; r < 6 ; r+=2)
    for(g = 0 ; g < 6 ; g+=2)
      for(b = 0 ; b < 6 ; b+=2)
      {
	if((pen = ObtainBestPen(Screen->ViewPort.ColorMap,
				      (r * 51)<<24, (g * 51)<<24, (b * 51)<<24,
				      OBP_Precision, PRECISION_EXACT,
				      TAG_DONE)) == -1)
	  fail = TRUE;
	else
	{
	  colors[r*36+g*6+b] = pen;
	  invcolors[pen] = ( ((r * 51) << 16 ) | ((g * 51) << 8) | (b * 51) );
	}
      };

  for(r = 1 ; r < 6 ; r+=2)
    for(g = 1 ; g < 6 ; g+=2)
      for(b = 1 ; b < 6 ; b+=2)
      {
	if((pen = ObtainBestPen(Screen->ViewPort.ColorMap,
				      (r * 51)<<24, (g * 51)<<24, (b * 51)<<24,
				      OBP_Precision, PRECISION_EXACT,
				      TAG_DONE)) == -1)
	  fail = TRUE;
	else
	{
	  colors[r*36+g*6+b] = pen;
	  invcolors[pen] = ( ((r * 51) << 16 ) | ((g * 51) << 8) | (b * 51) );
	}
      };

  for(r = 0 ; r < 6 ; r++)
    for(g = 0 ; g < 6 ; g++)
      for(b = 0 ; b < 6 ; b++)
      {
       if (!( ((r&1)&&(g&1)&&(b&1)) || (((r+1)&1)&&((g+1)&1)&&((b+1)&1)) ))
       {
	if((pen = ObtainBestPen(Screen->ViewPort.ColorMap,
				      (r * 51)<<24, (g * 51)<<24, (b * 51)<<24,
				      OBP_Precision, PRECISION_EXACT,
				      TAG_DONE)) == -1)
	  fail = TRUE;
	else
	{
	  colors[r*36+g*6+b] = pen;
	  invcolors[pen] = ( ((r * 51) << 16 ) | ((g * 51) << 8) | (b * 51) );
	}
       }
      };

  return fail;
}


void FreeColors(int *colors, struct Screen *Screen)
{
  int i;
  for(i = 0 ; i < 216 ; i++)
    if(colors[i] != -1)
    {
      ReleasePen(Screen->ViewPort.ColorMap, colors[i]);
      colors[i] = -1;
    }
}

//#ifndef __PPC__
//ULONG GetPen(register __a0 int *colors, register __d0 long r, register __d1 long g, register __d2 long b)
//#else
ULONG GetPen(int *colors, long r, long g, long b)
//#endif
{
  r = DividedBy51Rounded[r];
  g = DividedBy51Rounded[g];
  b = DividedBy51Rounded[b];
  return (ULONG)colors[r*36 + g*6 + b];
}


//#ifndef __PPC__
//ULONG GetPenDithered(register __a0 int *colors, register __d0 long r, register __d1 long g, register __d2 long b, register __d3 int x, register __d4 int y)
//#else
ULONG GetPenDithered(int *colors, long r, long g, long b, int x, int y)
//#endif
{
  UWORD offset = Halftone8x8[((y & 7) << 3) + (x & 7)];

  r = DivMod51[r*2] + (DivMod51[r*2+1] > offset);
  g = DivMod51[g*2] + (DivMod51[g*2+1] > offset);
  b = DivMod51[b*2] + (DivMod51[b*2+1] > offset);
  return (ULONG)colors[r*36 + g*6 + b];
}


//#ifndef __PPC__
//void GetPensDithered(register __a0 int *colors, register __a1 unsigned char* buf, register __d0 long r, register __d1 long g, register __d2 long b, register __d3 int x, register __d4 int y)
//#else
void GetPensDithered(int *colors, unsigned char* buf, long r, long g, long b, int x, int y)
//#endif
{
  UBYTE *ptr,*start;
  int i,offset;
  long rmod,gmod,bmod,r2,g2,b2;


  ptr = &(Halftone8x8[((y & 7) << 3) + (x & 7)]);
  start = &(Halftone8x8[(y & 7) << 3]);
  rmod = DivMod51[r*2+1];
  r = DivMod51[r*2];
  gmod = DivMod51[g*2+1];
  g = DivMod51[g*2];
  bmod = DivMod51[b*2+1];
  b = DivMod51[b*2];
  for (i=0;i<8;i++)
  {
	offset = *ptr++;
	r2 = rmod > offset ? r+1 : r;
	g2 = gmod > offset ? g+1 : g;
	b2 = bmod > offset ? b+1 : b;
	*buf++ = (unsigned char)(colors[r2*36+g2*6+b2]);
	if ((ptr-start) == 8)
		ptr = start;
  }
}

