#include "AmigaIncludes.h"
   
/* stack functions calls only for GCC to avoid the error "fixed or forbidden register was spilled" */
/* having those functioons in a separate file prevent gcc to inline them */
   
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat )        
{
APTR    a=image;		/* explanation: 'image' is in a data register but 'a' is on the stack  */
UWORD   b=SrcX;
UWORD   c=SrcY;
UWORD   d=SrcMod;
struct RastPort *e=RastPort;
UWORD   f=DestX;
UWORD   g=DestY;
UWORD   h=SizeX;
UWORD   i=SizeY;
UBYTE   j=SrcFormat;        

	WritePixelArray(a,b,c,d,e,f,g,h,i,j);

} 

ULONG STACKW3D_UpdateTexSubImage(W3D_Context *context, W3D_Texture *texture, void *teximage,ULONG level, ULONG *palette, W3D_Scissor* scissor, ULONG srcbpr)
{
W3D_Context *a=context;
W3D_Texture *b=texture;
void 		*c=teximage;
ULONG 	 d=level;
ULONG 	*e=palette;
W3D_Scissor *f=scissor;
ULONG 	 g=srcbpr;

return(W3D_UpdateTexSubImage(a,b,c,d,e,f,g));
}

ULONG STACKReadPixelArray(APTR  destRect, UWORD  destX,UWORD  destY, UWORD  destMod, struct RastPort *  RastPort, UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat )
{
APTR   a=destRect;
UWORD  b=destX;
UWORD  c=destY;
UWORD  d=destMod;
struct RastPort *  e=RastPort;
UWORD  f=SrcX;
UWORD  g=SrcY;
UWORD  h=SizeX;
UWORD  i=SizeY;
UBYTE  j=DestFormat;

	return(ReadPixelArray(a,b,c,d,e,f,g,h,i,j));

}
