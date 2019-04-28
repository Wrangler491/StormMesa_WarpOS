/*==================================================================================*/
struct button3D{
	BOOL ON;
	char name[30];
	};
/*==================================================================================*/
struct STORMMESA_parameters{
struct button3D NOHW;
struct button3D DIRECT;
struct button3D FAST;
struct button3D VERYFAST;
struct button3D SYNC;
struct button3D TRIPLE;
struct button3D STATS;
struct button3D NOCLAMP;
struct button3D NICETEX;
struct button3D NICEFOG;
struct button3D NOHWLINES;
struct button3D LOCKMODE1;
struct button3D LOCKMODE2;
struct button3D LOCKMODE3;
struct button3D NOZB;
struct button3D NOZBU;
struct button3D LIN;
struct button3D PROFILE;
struct button3D NORASTER;
struct button3D NODITHER;
struct button3D DebugStormMesa;
struct button3D DebugFunction;	
struct button3D StepFunction;		
struct button3D UseDrawArray;
struct button3D ChangePrimitiveFuncs;
};
/*==================================================================================*/
void STORMMESA_Prefs(void);
