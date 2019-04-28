/* Alain Thellier - 2010							*/
/* For GL_TRIANGLES draw fastly the vbuffer with W3D_DrawArray()	*/

#define STORMMESADEBUG 1

#ifdef STORMMESADEBUG
void LibPrintf(const char *string, ...);
#endif

/*==================================================================*/
#ifdef REMINDER
typedef struct
{
    W3D_Float x, y;             /* window coordinates */
    W3D_Double z;               /* z koordinate. Double precision for 32bit zbuffers */
    W3D_Float w;                /* w koordinate. */
    W3D_Float u, v, tex3d;      /* u/v koordinates, and an additional for 3d textures */
    W3D_Color color;            /* vertex color */
    W3D_ColorRGB spec;          /* specular lighting color */
    W3D_Float l;                /* Lambda value for Mipmapping. Private right  now */
}
W3D_Vertex;
#endif
/*==================================================================*/
void HW_SetArray(W3D_Context* context,W3D_Vertex *V,W3D_Texture *tex)
{
/*Set-up for W3D_DrawArray() */
void *VertexPointer;
void *TexCoordPointer;
void *ColorPointer;
int stride=sizeof(W3D_Vertex);
int off_v,off_w;

	VertexPointer=	(void *)&(V->x);
	TexCoordPointer=(void *)&(V->u);
	ColorPointer=	(void *)&(V->color);
	off_v=(UWORD)( (ULONG)&(V->v) - (ULONG)&(V->u));
	off_w=(UWORD)( (ULONG)&(V->w) - (ULONG)&(V->u));

	W3D_VertexPointer(context,VertexPointer,stride,W3D_VERTEX_F_F_D, 0);
	W3D_TexCoordPointer(context,TexCoordPointer,stride,0, off_v, off_w,0);
	W3D_ColorPointer(context,ColorPointer,stride,W3D_COLOR_FLOAT ,W3D_CMODE_RGBA,0);

	W3D_BindTexture(context,0,tex);
}
/*==================================================================*/
void HW_DrawTriangles(GLcontext *ctx, GLuint count)
{
/* Implement GL_TRIANGLES fastly */

AmigaMesaContext c = (AmigaMesaContext) ctx->DriverCtx;
W3Ddriver *hwcontext = (W3Ddriver *)c->hwdriver;
W3D_Context *context = hwcontext->context;
struct vertex_buffer *VB = ctx->VB;
W3D_Triangle tri;
GLuint pv;
GLubyte* RGBA8;
W3D_Color currentcolor;
W3D_Float* colf = hwcontext->colconv;
char mode;
BOOL UseDrawArray;
ULONG i;

/*LibPrintf*/printf("HW_DrawTriangles %ld\n",count);
/*LibPrintf*/printf("Func%ld flat %ld smooth %ld smoothTwo %ld \n",ctx->Driver.TriangleFunc,HW_DrawTriangle_flat,HW_DrawTriangle_smooth,HW_DrawTriangle_smoothTwo);
	if(ctx->Driver.TriangleFunc==HW_DrawTriangle_flat)		{mode='f';/*LibPrintf*/printf("flat \n");}
	if(ctx->Driver.TriangleFunc==HW_DrawTriangle_smooth)		{mode='s';/*LibPrintf*/ printf("smooth \n");}
	if(ctx->Driver.TriangleFunc==HW_DrawTriangle_smoothTwo)	{mode='2';/*LibPrintf*/ printf("smoothTwo \n");}

	if (hwcontext->flags & HWFLAG_STATS)
		(hwcontext->statarray2[DRAW_TRIANGLE])+=count/3;
	if (hwcontext->lockmode >= 2)
		HWDriver_Lock3(c);

	UseDrawArray=TRUE;
/*LibPrintf*/ printf("StippleFlag    %ld\n",ctx->Polygon.StippleFlag);
/*LibPrintf*/ printf("MonoColor      %ld\n",VB->MonoColor);
/*LibPrintf*/ printf("PolygonStipple %ld\n",ctx->PolygonStipple);

	if (ctx->Polygon.StippleFlag)
		UseDrawArray=FALSE;

	if(mode=='f')
	if(!VB->MonoColor)
		UseDrawArray=FALSE;
/*LibPrintf*/ printf("UseDrawArray %ld\n",UseDrawArray);

/* If cant use W3D_DrawArray() then loop with W3D_DrawTriangle () */
	if(!UseDrawArray)
	{
	tri.tex = hwcontext->currentw3dtex;
	if (!ctx->Polygon.StippleFlag)	/* ???? but was in original stormmesa sources */
	{tri.st_pattern = (unsigned char *)ctx->PolygonStipple;}
	else
	{tri.st_pattern = NULL;}

      for (i=0;i<count;i+=3)
	{
	hwcontext->CopyVertex(&tri.v1,&hwcontext->vbuffer[i+0]);
	hwcontext->CopyVertex(&tri.v2,&hwcontext->vbuffer[i+1]);
	hwcontext->CopyVertex(&tri.v3,&hwcontext->vbuffer[i+2]);

		if(mode=='f')	/* HW_DrawTriangles_Flat */
		if(!VB->MonoColor)
		{
		pv=i+2;
		RGBA8 = VB->Color[pv];
		currentcolor.r = colf[*RGBA8++];
		currentcolor.g = colf[*RGBA8++];
		currentcolor.b = colf[*RGBA8++];
		currentcolor.a = colf[*RGBA8++];
		W3D_SetCurrentColor(context,&currentcolor);
		}

		if(mode=='2')	/* HW_DrawTriangles_SmoothTwo */
		{
		 RGBA8 = VB->Color[i+0];
		 tri.v1.color.r = colf[*RGBA8++];
		 tri.v1.color.g = colf[*RGBA8++];
		 tri.v1.color.b = colf[*RGBA8++];
		 tri.v1.color.a = colf[*RGBA8++];
		 RGBA8 = VB->Color[i+1];
		 tri.v2.color.r = colf[*RGBA8++];
		 tri.v2.color.g = colf[*RGBA8++];
		 tri.v2.color.b = colf[*RGBA8++];
		 tri.v2.color.a = colf[*RGBA8++];
		 RGBA8 = VB->Color[i+2];
		 tri.v3.color.r = colf[*RGBA8++];
		 tri.v3.color.g = colf[*RGBA8++];
		 tri.v3.color.b = colf[*RGBA8++];
		 tri.v3.color.a = colf[*RGBA8++];
		}
	if (W3D_DrawTriangle(context,&tri) != W3D_SUCCESS)
		hwcontext->flags &= (~HWFLAG_ACTIVE);
	}

	}
/* If can use W3D_DrawArray() then draw in a single pass */
	else
	{
	/* use hardware culling so dont have to check each triangle */
	if(ctx->Polygon.FrontFace==GL_CW ) W3D_SetFrontFace(context,W3D_CW );
	if(ctx->Polygon.FrontFace==GL_CCW) W3D_SetFrontFace(context,W3D_CCW);
	W3D_SetState(context,W3D_CULLFACE,W3D_ENABLE);

	/* for xx_Smooth ==> use the vbuffer as array*/
	HW_SetArray(context,hwcontext->vbuffer,hwcontext->currentw3dtex);

	/* for xx_SmoothTwo ==> use the other color values as UBYTEs */
	if(mode=='2')
		W3D_ColorPointer(context,VB->Color,4,W3D_COLOR_UBYTE ,W3D_CMODE_RGBA,0);

	/* for xx_Flat + MonoColor ==> dont use the color values */
	if(mode=='f')
	if(VB->MonoColor)
		W3D_ColorPointer(context,NULL,4,W3D_COLOR_UBYTE ,W3D_CMODE_RGBA,0);

	/* Use fast W3D_DrawArray() function = draw all triangleS with one W3D call */
	if (W3D_DrawArray(context,W3D_PRIMITIVE_TRIANGLES,0,count) != W3D_SUCCESS)
		hwcontext->flags &= (~HWFLAG_ACTIVE);

	/* go back to software culling */
	W3D_SetState(context,W3D_CULLFACE,W3D_DISABLE);
	}

	hwcontext->flags |= HWFLAG_DIRTY;
}
/*==================================================================*/




