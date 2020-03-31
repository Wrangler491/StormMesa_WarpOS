#define NAMETXT	 	"agl"
#define VERSION	 	2		
#define REVISION		5
#define DATETXT		"22.06.2012"
#define VERSTXT		"2.5"
#define DEBUGLIBRARY 	1 
/*======================================================================================*/
#if defined(__GNUC__) && defined(__mc68000__)
asm ("jra _ReturnError");
#endif
#ifdef WARPUP
#pragma pack(push,2)
#include <sys/types.h>
#else
#include <machine/types.h>
#endif
#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <devices/timer.h>
#include <intuition/intuition.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <stdarg.h> /* for varargs */
#ifdef WARPUP
#pragma pack(pop)
#endif
/*======================================================================================*/
BOOL OpenAmigaLibraries();
void CloseAmigaLibraries();
void STORMMESA_Init();
void STORMMESA_Close();
void STORMMESA_Prefs();
void LibAlert(UBYTE *t);
void LibsPrintf(UBYTE *buffer,UBYTE *string, ...);
//void LibPrintf(const char *string); //, ...);
#define DeleteLibrary(BASE) FreeMem((APTR)((char*)(BASE)-((BASE)->lib_NegSize)),(BASE)->lib_NegSize+(BASE)->lib_PosSize)
#define REM(t) LibPrintf(#t); //LibAlert(#t);
//#define VAR(var) { LibPrintf(" " #var "="); LibPrintf("%ld\n",var);}
#define LibPrintf(t) ; //printf(t); //SPrintF(t,NULL)

#ifndef WARPUP
/*======================================================================================*/
/*	Name:			SDI_compiler.h
	Versionstring:	$VER: SDI_compiler.h 1.4 (30.03.2000)
	Author:		SDI
	Distribution:	PD
	Description:	defines to hide compiler stuff

 1.1	 25.06.98 : created from data made by Gunter Nikl
 1.2	 17.11.99 : added VBCC
 1.3	 29.02.00 : fixed VBCC REG define
 1.4	 30.03.00 : fixed SAVEDS for VBCC
*/
/*======================================================================================*/
/* first "exceptions" */

#if defined(__MAXON__)
	#define STDARGS
	#define REGARGS
	#define SAVEDS
	#define INLINE inline
#elif defined(__VBCC__)
	#define STDARGS
	#define REGARGS
	#define INLINE
	#define REG(reg,arg) __reg(#reg) arg
#elif defined(__STORM__)
	#define STDARGS
	#define REGARGS
	#define INLINE inline
#elif defined(__SASC)
	#define ASM(arg) arg __asm
#elif defined(__GNUC__)
	#define REG(reg,arg) arg __asm(#reg)
	#define LREG(reg,arg) register REG(reg,arg)
#endif

/* then "common" ones */

#if !defined(ASM)
	#define ASM(arg) arg
#endif
#if !defined(REG)
	#define REG(reg,arg) register __##reg arg
#endif
#if !defined(LREG)
	#define LREG(reg,arg) register arg
#endif
#if !defined(CONST)
	#define CONST const
#endif
#if !defined(SAVEDS)
	#define SAVEDS __saveds
#endif
#if !defined(INLINE)
	#define INLINE __inline
#endif
#if !defined(REGARGS)
	#define REGARGS __regargs
#endif
#if !defined(STDARGS)
	#define STDARGS __stdargs
#endif

#define D0(arg)	REG(d0,arg)
#define D1(arg)	REG(d1,arg)
#define D2(arg)	REG(d2,arg)
#define D3(arg)	REG(d3,arg)
#define D4(arg)	REG(d4,arg)
#define D5(arg)	REG(d5,arg)
#define D6(arg)	REG(d6,arg)
#define D7(arg)	REG(d7,arg)

#define A0(arg)	REG(a0,arg)
#define A1(arg)	REG(a1,arg)
#define A2(arg)	REG(a2,arg)
#define A3(arg)	REG(a3,arg)
#define A4(arg)	REG(a4,arg)
#define A5(arg)	REG(a5,arg)
#define A6(arg)	REG(a6,arg)
#define A7(arg)	REG(a7,arg)
/*======================================================================================*/
#if defined(_M68060) || defined(__M68060) || defined(__mc68060)
	#define CPUTXT	"060"
	#define CPUMSK	AFF_68060
#elif defined(_M68040) || defined(__M68040) || defined(__mc68040)
	#define CPUTXT	"040"
	#define CPUMSK	AFF_68040
#elif defined(_M68030) || defined(__M68030) || defined(__mc68030)
	#define CPUTXT	"030"
	#define CPUMSK	AFF_68030
#elif defined(_M68020) || defined(__M68020) || defined(__mc68020)
	#define CPUTXT	"020"
	#define CPUMSK	AFF_68020
#else
	#define CPUTXT	""
	#define CPUMSK	0
#endif
/*======================================================================================*/
#define LIBNAME	 NAMETXT ".library"
#define IDSTRING NAMETXT " " VERSTXT " (" DATETXT ") " CPUTXT "\r\n"
#define FULLNAME LIBNAME " " VERSTXT " (" DATETXT ")680" CPUTXT
/*======================================================================================*/
/*	SegList pointer definition	*/

#if defined(_AROS)
	typedef struct SegList * SEGLISTPTR;
#elif defined(__VBCC__)
	typedef APTR SEGLISTPTR;
#else
	typedef BPTR SEGLISTPTR;
#endif
/*======================================================================================*/
/*	library base private structure. The official one does not contain all the private fields! */
struct ExampleBase
	{
	struct Library		LibNode;
	UBYTE				Pad[2];	/* also serve to check if LibBase pointer is truly a struct ExampleBase */
	ULONG				NumCalls;
	struct ExecBase *		SysBase;
	SEGLISTPTR			SegList;
	struct Task *Task;
	void *CurrentContext;		/* appli data = contain current Mesa's context */
	void *ChildBase;
	};
void *FirstBase=NULL;

#endif	//WARPUP
/*======================================================================================*/
struct ExecBase *SysBase=NULL;
struct IntuitionBase *IntuitionBase;	//=NULL; for SDL
struct DosLibrary *DOSBase=NULL;
struct GfxBase *GfxBase;	//=NULL; for SDL
struct Library *CyberGfxBase;	//=NULL; for SDL
//struct Library *MathIeeeSingBasBase=NULL;
//struct Library *MathIeeeDoubBasBase=NULL;
//struct Library *__MathIeeeDoubTransBase=NULL;
//struct Library *__MathIeeeDoubBasBase=NULL;
struct UtilityBase *UtilityBase=NULL; 
#ifndef __PPC__
struct Library *Warp3DBase=NULL; 
struct Device *TimerBase=NULL;
#else
struct Library *Warp3DPPCBase=NULL; 
struct Library *TimerBase=NULL;
#endif
static struct timerequest tr;
ULONG StartTime=0;
BOOL LibDebug=FALSE;
BOOL CanPrint=FALSE;

#ifndef WARPUP
/*======================================================================================*/
/* First executable routine of this library; must return an error to the unsuspecting caller */
LONG ReturnError(void)
{
	if(OpenAmigaLibraries())
	{
		LibAlert(LIBNAME " is not a program! Copy it to libs:");
		CloseAmigaLibraries();
	}
	return -1;
}
/*======================================================================================*/
extern const ULONG LibInitTable[4]; /* the prototype */
/* The library loader looks for this marker in the memory the library code
	 and data will occupy. It is responsible setting up the Library base data structure.	*/
/*======================================================================================*/
const struct Resident RomTag = {
	RTC_MATCHWORD,				/* Marker value. */
	(struct Resident *)&RomTag,		/* This points back to itself. */
	(struct Resident *)LibInitTable,	 /* This points somewhere behind this marker. */
	RTF_AUTOINIT,				/* The Library should be set up according to the given table. */
	VERSION,					/* The version of this Library. */
	NT_LIBRARY,					/* This defines this module as a Library. */
	0,						/* Initialization priority of this Library; unused. */
	LIBNAME,					/* Points to the name of the Library. */
	IDSTRING,					/* The identification string of this Library. */
	(APTR)&LibInitTable			/* This table is for initializing the Library. */
};
#else
/*======================================================================================*/
/*	your own library's accessables functions	*/
#include "types.h"
extern GLcontext *CC;
void *AmigaGetGLcontext(void *LibBase)
{
	return CC;	//only one context in current implementation
}
#endif
#include "AmigaMesa.c"
#include "gllib.c"
#include "glapi.c" 
#ifndef WARPUP
/*======================================================================================*/
/* The mandatory reserved library function */
ULONG LibReserved(void)
{ 
	return 0;
}
/*======================================================================================*/
/* For current Task give current Library Base */
void *GetTaskLibBase(void)
{
struct ExampleBase *ThisBase;
UWORD BaseNum=0;
struct Task *MyTask;

	MyTask = FindTask(NULL);
	LibPrintf("GetTaskLibBase \n"); //MyTask%ld\n",MyTask);
	ThisBase=FirstBase;
	while(ThisBase!=NULL)
	{
	//LibPrintf("[%ld]LibBase: %ld Task: %ld ctx: %ld\n",BaseNum,ThisBase,ThisBase->Task,ThisBase->CurrentContext);
	if(ThisBase->Task==MyTask)
		{
		return(ThisBase);
		}
	ThisBase=ThisBase->ChildBase;
	BaseNum++;
	}
	return(NULL);
}
/*======================================================================================*/
/* Open the library, as called via OpenLibrary() */
ASM(struct Library *) LibOpen(REG(a6, struct ExampleBase * ExampleBase))
{
UBYTE *NewBasePtr;
struct ExampleBase *ThisBase;
ULONG	LibSize;
UBYTE *LibPtr;


	LibPrintf("LibOpen\n");
	/* Prevent delayed expunge and increment opencnt */
	ThisBase=FirstBase;
	ThisBase->LibNode.lib_Flags &= ~LIBF_DELEXP;
	ThisBase->LibNode.lib_OpenCnt++;

	LibPrintf("Exists a LibBase for this Task ?\n");
	ThisBase=GetTaskLibBase();
	if(ThisBase!=NULL)
		{
		LibPrintf("Already a LibBase exists:OK\n");
		return(&ThisBase->LibNode);
		}

/*	duplicate the first Library Base */
	LibPrintf("...Create a new LibBase for this Task\n");
	ThisBase=FirstBase;
	LibSize = ThisBase->LibNode.lib_NegSize + ThisBase->LibNode.lib_PosSize;
	LibPtr	= ((UBYTE *)ThisBase) - ThisBase->LibNode.lib_NegSize;
	if ((NewBasePtr = AllocMem(LibSize,MEMF_ANY)) != NULL)
	{
		CopyMem(LibPtr, NewBasePtr, LibSize);
		CacheClearE(NewBasePtr, ThisBase->LibNode.lib_NegSize, CACRF_ClearI | CACRF_ClearD); /* In MorphOS we dont need this if using SysV ABI */	
		NewBasePtr = NewBasePtr + ThisBase->LibNode.lib_NegSize;

		ThisBase->ChildBase = (struct ExampleBase *)NewBasePtr; 	/* the new Library Base is added in the list*/
		ThisBase=ThisBase->ChildBase; 					/* now use the new Library Base */

		ThisBase->Pad[0]='O';							/* reset the new base */
		ThisBase->Pad[1]='K';
		ThisBase->Task=FindTask(NULL);
		ThisBase->ChildBase= NULL;
		ThisBase->CurrentContext= NULL;
	}
	LibPrintf("New LibBase created:OK\n");
	return(&ThisBase->LibNode);
}
/*======================================================================================*/
/* Expunge the library, remove it from memory */
ASM(SEGLISTPTR) LibExpunge(REG(a6, struct ExampleBase * ExampleBase))
{
	ExampleBase=FirstBase;
	FirstBase=NULL;
	if(!ExampleBase->LibNode.lib_OpenCnt)
		{
		SEGLISTPTR SegList;
		SegList = ExampleBase->SegList;
		STORMMESA_Close(); 
		CloseAmigaLibraries();

		/* Remove the library from the public list */
		Remove((struct Node *) ExampleBase);

		/* Free the vector table and the library data */
		DeleteLibrary(&ExampleBase->LibNode);

		return SegList;
		}
	else
		ExampleBase->LibNode.lib_Flags |= LIBF_DELEXP;

	/* Return the segment pointer, if any */
	return 0;
}
/*======================================================================================*/
ASM(SEGLISTPTR) LibClose(REG(a6, struct ExampleBase * ExampleBase))
{
/* Close the library, as called by CloseLibrary() */
struct ExampleBase *ThisBase;

	STORMMESA_Close(); 

	if(!(--ExampleBase->LibNode.lib_OpenCnt))
		{
		if(ExampleBase->LibNode.lib_Flags & LIBF_DELEXP)
			return LibExpunge(ExampleBase);
		}

	ThisBase=FirstBase;
	while(ThisBase!=NULL)
	{
	if(ThisBase->ChildBase==ExampleBase)
		{
		ThisBase->ChildBase=ExampleBase->ChildBase;
		DeleteLibrary(&ExampleBase->LibNode);
		break;
		}
	ThisBase=ThisBase->ChildBase;
	}

	return 0;
}
/*======================================================================================*/
ASM(struct Library *) LibInit(REG(a0, SEGLISTPTR SegList),REG(d0, struct ExampleBase * ExampleBase), REG(a6, struct ExecBase *SysBaseLocale))
{
/* Initialize library */
	SysBase=SysBaseLocale;

	//if (CPUMSK && (SysBase->AttnFlags & CPUMSK) == 0)
	//	return 0;

	/* Remember stuff */
	ExampleBase->SegList	= SegList;
	ExampleBase->SysBase	= SysBase;

	ExampleBase->Pad[0]='O';							/* reset the (given) new base */
	ExampleBase->Pad[1]='K';
	ExampleBase->Task=FindTask(NULL);
	ExampleBase->ChildBase= NULL;
	ExampleBase->CurrentContext= NULL;

	FirstBase			= ExampleBase;

	if(OpenAmigaLibraries())
		{
		LibAlert(FULLNAME);
		STORMMESA_Init(); 
		return &ExampleBase->LibNode;
		}

	/* Free the vector table and the library data */
	DeleteLibrary(&ExampleBase->LibNode);
	return 0;
}
/*==================================================================================*/
void *Libmalloc(ULONG size )
{
void *pt;

	pt=AllocVec(size,MEMF_FAST | MEMF_CLEAR);
	if(pt==NULL)
		return(AllocVec(size,MEMF_ANY | MEMF_CLEAR));
	else
		return(pt); 
}
/*==================================================================================*/
void Libfree(void *p)
{
	FreeVec(p);
}
/*==================================================================================*/
#endif
void *Libmemcpy(void *s1,const void *s2,LONG n)
{
	CopyMem((APTR)s2,(APTR)s1,n);
	return s1;
}
/*==================================================================================*/
UBYTE *Libstrcpy(UBYTE *s1,UBYTE *s2)
{ UBYTE *s=s1;
	do
	*s++=*s2;
	while(*s2++!='\0');
	return s1;
}
/*==================================================================================*/
UBYTE *Libstrcat(UBYTE *s1,UBYTE *s2)
{
	UBYTE *s=s1;

	while(*s++)
	;
	--s;
	while((*s++=*s2++))
	;
	return s1;
}
/*==================================================================================*/
ULONG Libstrlen(UBYTE *string)
{
UBYTE *s=string;

	while(*s++)
	;
	return ~(string-s);
}
/*==================================================================================*/
void LibAlert(UBYTE *t)
{
#ifdef WARPUP
	LibPrintf(t);
#else

void *Data=&t+1L;
struct EasyStruct EasyStruct;
ULONG IDCMPFlags;

	if(!LibDebug) return;
	if(!CanPrint) return;
	EasyStruct.es_StructSize=sizeof(struct EasyStruct);
	EasyStruct.es_Flags=0L;
	EasyStruct.es_Title="Message:";
	EasyStruct.es_TextFormat=t;
	EasyStruct.es_GadgetFormat="OK";

	IDCMPFlags=0L;
	(void)EasyRequestArgs(NULL,&EasyStruct,&IDCMPFlags,Data);
	return;
#endif
}
#ifndef WARPUP
/*==================================================================================*/
static const UWORD CpyChr[] =
{
 0x16C0, 	/* move.b d0,(a3)+	*/
 0x4E75, 	/* rts			*/
};

/*==================================================================================*/
void LibsPrintf(UBYTE *buffer,UBYTE *string, ...)
{
va_list args;

	if(!LibDebug) return;
	if(!CanPrint) return;
	va_start(args, string);
	RawDoFmt (string, args, (void (*)(void))CpyChr, buffer);
	va_end(args);
}
/*==================================================================================*/
void LibPrintf(const char *string, ...)
{
UBYTE buffer[256];
va_list args;

	if(!LibDebug) return;
	if(!CanPrint) return;
	va_start(args, string);
	RawDoFmt (string, args, (void (*)(void))CpyChr, buffer);
	va_end(args);
	Write(Output(), buffer, Libstrlen(buffer));
}
/*==================================================================================*/
void Libsavefile(UBYTE *filename,void *pt,ULONG size)
{
BPTR file;

	if ((file = Open(filename,MODE_NEWFILE)))
	{ 
	Write(file,pt,size);
	Close(file); 
	}
}
/*==================================================================================*/
void Libloadfile(UBYTE *filename,void *pt,ULONG size)
{
BPTR file;

	if ((file = Open(filename,MODE_OLDFILE)))
	{ 
	Read(file,pt,size);
	Close(file); 
	}
}
/*==================================================================================*/
ULONG LibMilliTimer(void)
{
struct timeval tv;
ULONG	MilliFrequence1=1000;
ULONG	MilliFrequence2=1000000/MilliFrequence1;
ULONG MilliTime;

	GetSysTime(&tv);
	if(StartTime==0)	StartTime=tv.tv_secs;
	MilliTime	= (tv.tv_secs-StartTime) *	MilliFrequence1 + tv.tv_micro/MilliFrequence2;
	return(MilliTime);
}
/*==================================================================================*/
void *AmigaGetGLcontext(void *LibBase)
{
struct ExampleBase * ExampleBase=LibBase;

	if(ExampleBase)
	if(ExampleBase->Pad[0]=='O')
	if(ExampleBase->Pad[1]=='K')
		return(ExampleBase->CurrentContext);

	LibPrintf("ERROR: Unvalid LibBase in A6!\n");
	ExampleBase=GetTaskLibBase();		/* if LibBase is lost then try to find it with with the task */

	if(ExampleBase)
	if(ExampleBase->Pad[0]=='O')
	if(ExampleBase->Pad[1]=='K')
		return(ExampleBase->CurrentContext);

	return(NULL);	/* a6 has lost the LibBase pointer so we cant recover the GL CurrentContext */
}
/*==================================================================================*/
void	AmigaSetGLcontext(void *LibBase,void *GLcontext)
{
struct ExampleBase * ExampleBase=LibBase;

	ExampleBase->CurrentContext=GLcontext;
}
#endif
/*==================================================================================*/
BOOL OpenAmigaLibraries()
{
/* Initialize the standards libraries We assume that ROM libraries open OK */

	DOSBase=(struct DosLibrary *)	OpenLibrary("dos.library",36L); 
	if(IntuitionBase==NULL) IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L);
	UtilityBase=(struct Library *)OpenLibrary("utility.library",36L); 

#ifndef WARPUP
	if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)&tr, 0L) != 0)
		{return(FALSE);}
#endif

	CanPrint=TRUE;
	return(TRUE);
}
/*======================================================================================*/
void CloseAmigaLibraries()
{
#define LIBCLOSE(Lbase)	 if(Lbase!=NULL)	{CloseLibrary( ((struct Library *)Lbase) );Lbase=NULL;}

	LIBCLOSE(UtilityBase) 
	LIBCLOSE(IntuitionBase)
	LIBCLOSE(DOSBase)
	CanPrint=FALSE;	/* with OS libraries closed cant print anymore */
}
#ifndef WARPUP
/*======================================================================================*/
/* This is the table of functions that make up the library. The first
	 four are mandatory, everything following it are user callable
	 routines. The table is terminated by the value -1. */

static const APTR LibVectors[] = {
	(APTR) LibOpen,
	(APTR) LibClose,
	(APTR) LibExpunge,
	(APTR) LibReserved,

#include "Amiga_functions_names.h"

	(APTR) -1
};
/*======================================================================================*/
struct LibInitData {
 UBYTE i_Type;	 UBYTE o_Type;	 UBYTE	d_Type;	UBYTE p_Type;
 UBYTE i_Name;	 UBYTE o_Name;	 STRPTR d_Name;
 UBYTE i_Flags;	UBYTE o_Flags;	UBYTE	d_Flags;	UBYTE p_Flags;
 UBYTE i_Version;	UBYTE o_Version;	UWORD	d_Version;
 UBYTE i_Revision; UBYTE o_Revision; UWORD	d_Revision;
 UBYTE i_IdString; UBYTE o_IdString; STRPTR d_IdString;
 ULONG endmark;
};
/*======================================================================================*/
static const struct LibInitData LibInitData = {
#ifdef __VBCC__		/* VBCC does not like OFFSET macro */
 0xA0,	8, NT_LIBRARY,				0,
 0x80, 10, LIBNAME,
 0xA0, 14, LIBF_SUMUSED|LIBF_CHANGED, 0,
 0x90, 20, VERSION,
 0x90, 22, REVISION,
 0x80, 24, IDSTRING,
#else
 0xA0, (UBYTE) OFFSET(Node,	ln_Type),		NT_LIBRARY,		 0,
 0x80, (UBYTE) OFFSET(Node,	ln_Name),		LIBNAME,
 0xA0, (UBYTE) OFFSET(Library, lib_Flags),	LIBF_SUMUSED|LIBF_CHANGED, 0,
 0x90, (UBYTE) OFFSET(Library, lib_Version),	VERSION,
 0x90, (UBYTE) OFFSET(Library, lib_Revision), REVISION,
 0x80, (UBYTE) OFFSET(Library, lib_IdString), IDSTRING,
#endif
 0
};
/*======================================================================================*/
/* The following data structures and data are responsible for*/
/*	 setting up the Library base data structure and the library*/
/*	 function vector.*/
/*======================================================================================*/
const ULONG LibInitTable[4] = {
	(ULONG)sizeof(struct ExampleBase), /* Size of the base data structure */
	(ULONG)LibVectors,			 /* Points to the function vector */
	(ULONG)&LibInitData,			 /* Library base data structure setup table */
	(ULONG)LibInit				 /* The address of the routine to do the setup */
};
/*======================================================================================*/
#else

void LibsPrintf(UBYTE *buffer, UBYTE *string, ...)
{
	LibPrintf(string);
	return;
}


void glConstructor(void)
{
  char *Flag;
	LibPrintf("Welcome to StormMesa for WarpOS\n");
  if(!OpenAmigaLibraries()) {
	LibPrintf("Major fault starting StormMesa - quitting\n");
	exit(10);
  }
	LibAlert("Opened lib\n"); //FULLNAME
	STORMMESA_Init(); 
}

void glDestructor(void)
{
	STORMMESA_Close();
	CloseAmigaLibraries();
}

  __asm__ (".section .init \n .long glConstructor, 340 \n .section .text\n");
  __asm__ (".section .fini \n .long glDestructor, 340 \n .section .text\n");


#endif	//WARPUP
