
#include "AmigaIncludes.h"
#include "AmigaPrefs.h"
#ifdef WARPUP
#pragma pack(push,2)
#endif
#include <proto/gadtools.h>
#ifdef WARPUP
#pragma pack(pop)
#endif
extern BOOL LibDebug;				/* Enable Library Debugger (global)		*/

#if (!defined(__AROS__) && !defined(__MORPHOS__)) || defined(WARPUP)
typedef ULONG IPTR; 
#endif
/*=================================================================*/
UBYTE StormMesaPrefsName[] = {"StormMesa2010 Prefs(v2.5 June 2012)"};
IPTR StormMesaPrefsNameTag=(IPTR) StormMesaPrefsName;
struct STORMMESA_parameters StormMesa;
struct Library *GadToolsBase;
static struct TextAttr MyFont = { "topaz.font", 8, 0, 0, };
#define LIBCLOSE(Lbase)	 if(Lbase!=NULL)	{CloseLibrary( ((struct Library *)Lbase) );Lbase=NULL;}
#define NLOOP(nbre) for(n=0;n<nbre;n++)
//#define STORMMESADEBUG 1
/*=================================================================*/
void STORMMESA_Function(const char *func)
{
#ifdef STORMMESADEBUG
	if(StormMesa.DebugFunction.ON)
		//LibPrintf("AGL: %s\n" ,func);
	if(StormMesa.StepFunction.ON)
		LibAlert(func);
#endif
}
/*=================================================================*/
static void process_window_events(struct Window *mywin,struct button3D *ButtonList)
{
struct IntuiMessage *imsg;
struct Gadget *gad;
BOOL  terminated = FALSE;

while (!terminated)
    {
    Wait (1 << mywin->UserPort->mp_SigBit);

    /* Use GT_GetIMsg() and GT_ReplyIMsg() for handling */
    /* IntuiMessages with GadTools gadgets.             */
    while ((!terminated) && (imsg = GT_GetIMsg(mywin->UserPort)))
        {
        /* GT_ReplyIMsg() at end of loop */

        switch (imsg->Class)
            {
            case IDCMP_GADGETUP:       /* Buttons only report GADGETUP */
                gad = (struct Gadget *)imsg->IAddress;
            ButtonList[gad->GadgetID].ON=!ButtonList[gad->GadgetID].ON;
                break;
            case IDCMP_CLOSEWINDOW:
                terminated = TRUE;
                break;
            case IDCMP_REFRESHWINDOW:
                /* This handling is REQUIRED with GadTools. */
                GT_BeginRefresh(mywin);
                GT_EndRefresh(mywin, TRUE);
                break;
            }
        /* Use the toolkit message-replying function here... */
        GT_ReplyIMsg(imsg);
        }
    }
}
/*=================================================================*/
void LibSettings(struct button3D *ButtonList,WORD ButtonCount)
{
struct Screen    *mysc;
struct Window    *mywin;
struct Gadget    *glist, *gad;
struct NewGadget ng;
void             *vi;
WORD n,x,y;
glist = NULL;


	GadToolsBase =			 OpenLibrary("gadtools.library", 37);
	if(GadToolsBase==NULL) return;

if ( (mysc = LockPubScreen(NULL)) != NULL )
    {
    if ( (vi = GetVisualInfo(mysc, TAG_END)) != NULL )
        {
        /* GadTools gadgets require this step to be taken */
        gad = CreateContext(&glist);

NLOOP(ButtonCount)
{
	x=220 + mysc->WBorLeft;
	y=12*n + 6 + mysc->WBorTop + (mysc->Font->ta_YSize + 1);
	if((ButtonCount/2) < n)
		{x=x+220; y=y-ButtonCount/2*12;}

        /* create a button gadget centered below the window title */
        ng.ng_TextAttr   = &MyFont;
        ng.ng_VisualInfo = vi;
        ng.ng_LeftEdge   = x;
        ng.ng_TopEdge    = y;
        ng.ng_Width      = 24;
        ng.ng_Height     = 12;
        ng.ng_GadgetText = ButtonList[n].name;
        ng.ng_GadgetID   = n;
        ng.ng_Flags      = 0;
	gad = CreateGadget(CHECKBOX_KIND, gad, &ng, GT_Underscore, '_',  GTCB_Scaled, TRUE,GTCB_Checked,ButtonList[n].ON,TAG_DONE);
}

        if (gad != NULL)
            {
            if ( (mywin = OpenWindowTags(NULL,
                    WA_Title,  StormMesaPrefsNameTag ,
                    WA_Gadgets, (ULONG)  glist,      WA_AutoAdjust,    TRUE,
                    WA_InnerWidth,       260+220,      WA_InnerHeight,    20+12*ButtonCount/2,
                    WA_DragBar,    TRUE,      WA_DepthGadget,   TRUE,
                    WA_Activate,   TRUE,      WA_CloseGadget,   TRUE,
                    WA_IDCMP, IDCMP_CLOSEWINDOW |IDCMP_REFRESHWINDOW | BUTTONIDCMP,
                    WA_PubScreen,  (ULONG) mysc,
                    TAG_END)) != NULL )
                {
                GT_RefreshWindow(mywin, NULL);

                process_window_events(mywin,ButtonList);

                CloseWindow(mywin);
                }
            }
        /* FreeGadgets() must be called after the context has been
        ** created.  It does nothing if glist is NULL
        */
        FreeGadgets(glist);
        FreeVisualInfo(vi);
        }

    UnlockPubScreen(NULL, mysc);
    }

	CloseLibrary( ((struct Library *)GadToolsBase) );
}
/*=================================================================*/
void STORMMESA_Prefs()
{
WORD ButtonCount;

#ifdef STORMMESADEBUG
	ButtonCount=((LONG)&StormMesa.ChangePrimitiveFuncs  - (LONG)&StormMesa.NOHW)/sizeof(struct button3D)+1;
#else
	ButtonCount=((LONG)&StormMesa.DebugStormMesa  - (LONG)&StormMesa.NOHW)/sizeof(struct button3D)+1;
#endif
	StormMesa.DebugStormMesa.ON=LibDebug;
	LibSettings(&StormMesa.NOHW,ButtonCount);
	LibDebug=StormMesa.DebugStormMesa.ON;
}
/*==================================================================================*/
void STORMMESA_Init()
{
	REM(STORMMESA_Init)
	Libstrcpy(StormMesa.NOHW.name,"NOHW");
	Libstrcpy(StormMesa.DIRECT.name,"DIRECT");
	Libstrcpy(StormMesa.FAST.name,"FAST");
	Libstrcpy(StormMesa.VERYFAST.name,"VERYFAST");
	Libstrcpy(StormMesa.SYNC.name,"SYNC");
	Libstrcpy(StormMesa.TRIPLE.name,"TRIPLE");
	Libstrcpy(StormMesa.STATS.name,"STATS");
	Libstrcpy(StormMesa.NOCLAMP.name,"NOCLAMP");
	Libstrcpy(StormMesa.NICETEX.name,"NICETEX");
	Libstrcpy(StormMesa.NICEFOG.name,"NICEFOG");
	Libstrcpy(StormMesa.NOHWLINES.name,"NOHWLINES");
	Libstrcpy(StormMesa.LOCKMODE1.name,"LOCKMODE1");
	Libstrcpy(StormMesa.LOCKMODE2.name,"LOCKMODE2");
	Libstrcpy(StormMesa.LOCKMODE3.name,"LOCKMODE3");
	Libstrcpy(StormMesa.PROFILE.name,"PROFILE");
	Libstrcpy(StormMesa.NORASTER.name,"NORASTER");
	Libstrcpy(StormMesa.NODITHER.name,"NODITHER");
	Libstrcpy(StormMesa.NOZB.name,"NOZB");
	Libstrcpy(StormMesa.NOZBU.name,"NOZBU");
	Libstrcpy(StormMesa.LIN.name,"LIN");
	Libstrcpy(StormMesa.DebugStormMesa.name,">>> Enable Debugger >>>");
	Libstrcpy(StormMesa.DebugFunction.name,"Debug Function");	
	Libstrcpy(StormMesa.StepFunction.name,"Step Function");		
	Libstrcpy(StormMesa.UseDrawArray.name,"Use DrawArray");
	Libstrcpy(StormMesa.ChangePrimitiveFuncs.name,"Change Primitive Funcs");

	StormMesa.LOCKMODE3.ON=TRUE;
	//StormMesa.VERYFAST.ON=TRUE;
	//StormMesa.NOHW.ON=FALSE;
	//StormMesa.UseDrawArray.ON=TRUE;
	//StormMesa.STATS.ON=TRUE;

}
/*==================================================================================*/
void STORMMESA_Close()
{
	return;
}
/*==================================================================================*/
