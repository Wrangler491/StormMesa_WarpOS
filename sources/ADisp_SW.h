#ifndef _ADISP_SW_H
#define _ADISP_SW_H

BOOL SWDriver_init(struct amigamesa_context *c,struct TagItem *tagList);
//#ifndef __PPC__
//long EvalPen(register __a0 struct amigamesa_context *c, register __d0 unsigned char r, register __d1 unsigned char g, register __d2 unsigned char b);
//long EvalPen_D(register __a0 struct amigamesa_context *c, register __d0 unsigned char r, register __d1 unsigned char g, register __d2 unsigned char b, register __d3 int x, register __d4 int y);
//#else
long EvalPen(struct amigamesa_context *c, unsigned char r, unsigned char g, unsigned char b);
long EvalPen_D(struct amigamesa_context *c, unsigned char r, unsigned char g, unsigned char b, int x, int y);
//#endif
long FindNearestPen(struct amigamesa_context *c, unsigned long r, unsigned long g, unsigned long b, unsigned long* tab);
int CreateTransTable(struct amigamesa_context* c);
void DisposeTransTable(struct amigamesa_context* c);
int InitTransTable(struct amigamesa_context* c);
void FreeTransTable(struct amigamesa_context* c);

GLbitfield SWD_clear_LUT8(GLcontext *ctx, GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWD_clear_ARGB(GLcontext *ctx, GLbitfield mask, GLboolean all, GLint x, GLint y, GLint width, GLint height );

#endif
