#ifdef WARPUP
#pragma pack(push,2)
#endif
#include <exec/libraries.h>
#ifdef WARPUP
#pragma pack(pop)
#endif

struct glreg
{
	int size;
	void (*func_exit)(int);
	void *base; /* PPC only */
};

#ifndef __PPC__
#ifdef STATIC_
void registerGL(register struct glreg* ptr);
#else
void registerGL(register struct glreg* ptr __asm("a0"));
#endif
#else
void registerGL(struct glreg* ptr);
#endif
extern void exit(int);
