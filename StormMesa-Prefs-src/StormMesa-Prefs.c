#ifdef WARPUP
#pragma pack(push,2)
#endif
#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>

#ifndef WARPUP
struct Library *glBase;

#include <inline/macros.h>
#define AmigaMesaCreateContext(tagList)  LP1(36,ULONG,AmigaMesaCreateContext,void*,tagList,a0,,glBase)

void main(int argc, char **argv)
{
    glBase = OpenLibrary("agl.library", 2L);
    if (glBase)
    {
    AmigaMesaCreateContext(NULL); /* It mean use backdoor to StormMesa-Prefs */
    CloseLibrary(glBase);
    }
    exit(0);
}
#else
#pragma pack(pop)
#include "GL/gl.h"
#include "AmigaMesa.h"

//WARPUP static library version
//void AmigaMesaCreateContext(tagList);

void main(int argc, char **argv)
{
    AmigaMesaCreateContext(NULL); /* It mean use backdoor to StormMesa-Prefs */
    exit(0);
}
#endif