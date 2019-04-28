#include <exec/types.h>
#include <intuition/screens.h>


extern int Colors[];

#define RGBA(amesa, r, g, b, a) GetPen(Colors, r, g, b)
#define RGBA_Dithered(amesa, r, g, b, a, x, y) GetPenDithered(Colors, r, g, b, x, y)

BOOL AllocColors(int *colors, int *invcolors, struct Screen *Screen);
void FreeColors(int *colors, struct Screen *Screen);
/*
#ifndef __PPC__
ULONG GetPen(register __a0 int *colors, register __d0 long r, register __d1 long g, register __d2 long b);
ULONG GetPenDithered(register __a0 int *colors, register __d0 long r, register __d1 long g, register __d2 long b, register __d3 int x, register __d4 int y);
void GetPensDithered(register __a0 int *colors, register __a1 unsigned char* buf, register __d0 long r, register __d1 long g, register __d2 long b, register __d3 int x, register __d4 int y);
#else
*/
ULONG GetPen(int *colors, long r, long g, long b);
ULONG GetPenDithered(int *colors, long r, long g, long b, int x, int y);
void GetPensDithered(int *colors, unsigned char* buf, long r, long g, long b, int x, int y);
//#endif

