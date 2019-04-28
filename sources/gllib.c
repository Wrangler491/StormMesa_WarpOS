#include <exec/libraries.h>
#include <clib/exec_protos.h>
#include "glsm.h"

struct glBase
{
        struct Library base;
};

#pragma libbase glBase

#define STORMMESADEBUG 1

void LibPrintf(const char *string, ...);
/*==================================================================================*/
#ifdef STORMMESADEBUG
#define REM(message) printf(#message"\n");	//LibPrintf(#message"\n");
#define SFUNCTION(n) STORMMESA_Function(n);
/*==================================================================================*/
#else
#define REM(message) ;
#define SFUNCTION(n) ;
#endif  
/*==================================================================================*/

void (*f_exit)(int);
#if defined(STATIC_) | defined(WARPUP)
void registerGL(register struct glreg* ptr)
#else
void registerGL(register struct glreg* ptr __asm("a0"))
#endif
{
/* REM(registerGL) */
        f_exit = ptr->func_exit;
}

void exit(int rc)
{
        (*f_exit)(rc);
}


void Dummy_at_90(void){}
void Dummy_at_96(void){}
void Dummy_at_102(void){}
void Dummy_at_108(void){}
void Dummy_at_114(void){}
void Dummy_at_120(void){}
void Dummy_at_126(void){}
void Dummy_at_132(void){}
void Dummy_at_138(void){}
void Dummy_at_144(void){}
void Dummy_at_150(void){}
void Dummy_at_156(void){}
void Dummy_at_162(void){}
void Dummy_at_168(void){}
void Dummy_at_174(void){}
