/* This file has been automatically generated - Thellier 2010 */

/*
* Mesa 3-D graphics library)
* Version:2.2)
* Copyright (C) 1995-1997Brian Paul)
*
* This library is free software you can redistribute it and/or)
* modify it under the terms of the GNU Library General Public)
* License as published by the Free Software Foundation either)
* version 2 of the License, or (at your option) any later version.
*
* but WITHOUT ANY WARRANTY without even the implied warranty of)
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU)
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public)
* License along with this library if not, write to the Free)
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <GL/gl.h>

/*======================================================================================*/
/*  Name:			SDI_compiler.h
		$VER: SDI_compiler.h 1.4 (30.03.2000)
			SDI
		PD
		defines to hide compiler stuff

 1.1   25.06.98 : created from data made by Gunter Nikl
 1.2   17.11.99 : added VBCC
 1.3   29.02.00 : fixed VBCC REG define
 1.4   30.03.00 : fixed SAVEDS for VBCC
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
	#ifndef WARPUP
	  #define REG(reg,arg) arg __asm(#reg)
	  #define LREG(reg,arg) register REG(reg,arg)
	#else
	  #define REG(reg,arg) arg
	  #define LREG(reg,arg) arg
	#endif
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

#define D0(arg)  REG(d0,arg)
#define D1(arg)  REG(d1,arg)
#define D2(arg)  REG(d2,arg)
#define D3(arg)  REG(d3,arg)
#define D4(arg)  REG(d4,arg)
#define D5(arg)  REG(d5,arg)
#define D6(arg)  REG(d6,arg)
#define D7(arg)  REG(d7,arg)

#define A0(arg)  REG(a0,arg)
#define A1(arg)  REG(a1,arg)
#define A2(arg)  REG(a2,arg)
#define A3(arg)  REG(a3,arg)
#define A4(arg)  REG(a4,arg)
#define A5(arg)  REG(a5,arg)
#define A6(arg)  REG(a6,arg)
#define A7(arg)  REG(a7,arg)

#define FP0(arg)  REG(fp0,arg)
#define FP1(arg)  REG(fp1,arg)
#define FP2(arg)  REG(fp2,arg)
#define FP3(arg)  REG(fp3,arg)
#define FP4(arg)  REG(fp4,arg)
#define FP5(arg)  REG(fp5,arg)
#define FP6(arg)  REG(fp6,arg)
#define FP7(arg)  REG(fp7,arg)

#include "api1.c"
#include "api2.c"
#include "apiext.c"

/* we are inside a library call so register a6 contain the library base		*/
/* The current GL context is stored in this task's library-base			*/
/* so AmigaGetGLcontext(libbase) obtain the current GL context from a6		*/

void APIENTRY smglClearIndex(A6(void* libbase),FP0(GLfloat c))	{aglClearIndex(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglClearColor(A6(void* libbase),FP0(GLclampf red),FP1(GLclampf green),FP2(GLclampf blue),FP3(GLclampf alpha))	{aglClearColor(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglClear(A6(void* libbase),D0(GLbitfield mask))	{aglClear(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglIndexMask(A6(void* libbase),D0(GLuint mask))	{aglIndexMask(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglColorMask(A6(void* libbase),D0(GLboolean red),D1(GLboolean green),D2(GLboolean blue),D3(GLboolean alpha))	{aglColorMask(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglAlphaFunc(A6(void* libbase),D0(GLenum func),FP0(GLclampf ref))	{aglAlphaFunc(AmigaGetGLcontext(libbase),func,ref);}
void APIENTRY smglBlendFunc(A6(void* libbase),D0(GLenum sfactor),D1(GLenum dfactor))	{aglBlendFunc(AmigaGetGLcontext(libbase),sfactor,dfactor);}
void APIENTRY smglLogicOp(A6(void* libbase),D0(GLenum opcode))	{aglLogicOp(AmigaGetGLcontext(libbase),opcode);}
void APIENTRY smglCullFace(A6(void* libbase),D0(GLenum mode))	{aglCullFace(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglFrontFace(A6(void* libbase),D0(GLenum mode))	{aglFrontFace(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglPointSize(A6(void* libbase),FP0(GLfloat size))	{aglPointSize(AmigaGetGLcontext(libbase),size);}
void APIENTRY smglLineWidth(A6(void* libbase),FP0(GLfloat width))	{aglLineWidth(AmigaGetGLcontext(libbase),width);}
void APIENTRY smglLineStipple(A6(void* libbase),D0(GLint factor),D1(GLushort pattern))	{aglLineStipple(AmigaGetGLcontext(libbase),factor,pattern);}
void APIENTRY smglPolygonMode(A6(void* libbase),D0(GLenum face),D1(GLenum mode))	{aglPolygonMode(AmigaGetGLcontext(libbase),face,mode);}
void APIENTRY smglPolygonOffset(A6(void* libbase),FP0(GLfloat factor),FP1(GLfloat units))	{aglPolygonOffset(AmigaGetGLcontext(libbase),factor,units);}
void APIENTRY smglPolygonStipple(A6(void* libbase),A0(const GLubyte* mask))	{aglPolygonStipple(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglGetPolygonStipple(A6(void* libbase),A0(GLubyte* mask))	{aglGetPolygonStipple(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglEdgeFlag(A6(void* libbase),D0(GLboolean flag))	{aglEdgeFlag(AmigaGetGLcontext(libbase),flag);}
void APIENTRY smglEdgeFlagv(A6(void* libbase),A0(const GLboolean* flag))	{aglEdgeFlagv(AmigaGetGLcontext(libbase),flag);}
void APIENTRY smglScissor(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLsizei width),D3(GLsizei height))	{aglScissor(AmigaGetGLcontext(libbase),x,y,width,height);}
void APIENTRY smglClipPlane(A6(void* libbase),D0(GLenum plane),A0(const GLdouble* equation))	{aglClipPlane(AmigaGetGLcontext(libbase),plane,equation);}
void APIENTRY smglGetClipPlane(A6(void* libbase),D0(GLenum plane),A0(GLdouble* equation))	{aglGetClipPlane(AmigaGetGLcontext(libbase),plane,equation);}
void APIENTRY smglDrawBuffer(A6(void* libbase),D0(GLenum mode))	{aglDrawBuffer(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglReadBuffer(A6(void* libbase),D0(GLenum mode))	{aglReadBuffer(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglEnable(A6(void* libbase),D0(GLenum cap))	{aglEnable(AmigaGetGLcontext(libbase),cap);}
void APIENTRY smglDisable(A6(void* libbase),D0(GLenum cap))	{aglDisable(AmigaGetGLcontext(libbase),cap);}
void APIENTRY smglEnableClientState(A6(void* libbase),D0(GLenum cap))	{aglEnableClientState(AmigaGetGLcontext(libbase),cap);}
void APIENTRY smglDisableClientState(A6(void* libbase),D0(GLenum cap))	{aglDisableClientState(AmigaGetGLcontext(libbase),cap);}
void APIENTRY smglGetBooleanv(A6(void* libbase),D0(GLenum pname),A0(GLboolean* params))	{aglGetBooleanv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglGetDoublev(A6(void* libbase),D0(GLenum pname),A0(GLdouble* params))	{aglGetDoublev(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglGetFloatv(A6(void* libbase),D0(GLenum pname),A0(GLfloat* params))	{aglGetFloatv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglGetIntegerv(A6(void* libbase),D0(GLenum pname),A0(GLint* params))	{aglGetIntegerv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglPushAttrib(A6(void* libbase),D0(GLbitfield mask))	{aglPushAttrib(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglPopAttrib(A6(void* libbase))	{aglPopAttrib(AmigaGetGLcontext(libbase));}
void APIENTRY smglPushClientAttrib(A6(void* libbase),D0(GLbitfield mask))	{aglPushClientAttrib(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglPopClientAttrib(A6(void* libbase))	{aglPopClientAttrib(AmigaGetGLcontext(libbase));}
void APIENTRY smglFinish(A6(void* libbase))	{aglFinish(AmigaGetGLcontext(libbase));}
void APIENTRY smglFlush(A6(void* libbase))	{aglFlush(AmigaGetGLcontext(libbase));}
void APIENTRY smglHint(A6(void* libbase),D0(GLenum target),D1(GLenum mode))	{aglHint(AmigaGetGLcontext(libbase),target,mode);}
void APIENTRY smglClearDepth(A6(void* libbase),FP0(GLclampd depth))	{aglClearDepth(AmigaGetGLcontext(libbase),depth);}
void APIENTRY smglDepthFunc(A6(void* libbase),D0(GLenum func))	{aglDepthFunc(AmigaGetGLcontext(libbase),func);}
void APIENTRY smglDepthMask(A6(void* libbase),D0(GLboolean flag))	{aglDepthMask(AmigaGetGLcontext(libbase),flag);}
void APIENTRY smglDepthRange(A6(void* libbase),FP0(GLclampd near_val),FP1(GLclampd far_val))	{aglDepthRange(AmigaGetGLcontext(libbase),near_val,far_val);}
void APIENTRY smglClearAccum(A6(void* libbase),FP0(GLfloat red),FP1(GLfloat green),FP2(GLfloat blue),FP3(GLfloat alpha))	{aglClearAccum(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglAccum(A6(void* libbase),D0(GLenum op),FP0(GLfloat value))	{aglAccum(AmigaGetGLcontext(libbase),op,value);}
void APIENTRY smglMatrixMode(A6(void* libbase),D0(GLenum mode))	{aglMatrixMode(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglOrtho(A6(void* libbase),FP0(GLdouble left),FP1(GLdouble right),FP2(GLdouble bottom),FP3(GLdouble top),FP4(GLdouble nearval),FP5(GLdouble farval))	{aglOrtho(AmigaGetGLcontext(libbase),left,right,bottom,top,nearval,farval);}
void APIENTRY smglFrustum(A6(void* libbase),FP0(GLdouble left),FP1(GLdouble right),FP2(GLdouble bottom),FP3(GLdouble top),FP4(GLdouble nearval),FP5(GLdouble farval))	{aglFrustum(AmigaGetGLcontext(libbase),left,right,bottom,top,nearval,farval);}
void APIENTRY smglViewport(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLsizei width),D3(GLsizei height))	{aglViewport(AmigaGetGLcontext(libbase),x,y,width,height);}
void APIENTRY smglPushMatrix(A6(void* libbase))	{aglPushMatrix(AmigaGetGLcontext(libbase));}
void APIENTRY smglPopMatrix(A6(void* libbase))	{aglPopMatrix(AmigaGetGLcontext(libbase));}
void APIENTRY smglLoadIdentity(A6(void* libbase))	{aglLoadIdentity(AmigaGetGLcontext(libbase));}
void APIENTRY smglLoadMatrixd(A6(void* libbase),A0(const GLdouble* m))	{aglLoadMatrixd(AmigaGetGLcontext(libbase),m);}
void APIENTRY smglLoadMatrixf(A6(void* libbase),A0(const GLfloat* m))	{aglLoadMatrixf(AmigaGetGLcontext(libbase),m);}
void APIENTRY smglMultMatrixd(A6(void* libbase),A0(const GLdouble* m))	{aglMultMatrixd(AmigaGetGLcontext(libbase),m);}
void APIENTRY smglMultMatrixf(A6(void* libbase),A0(const GLfloat* m))	{aglMultMatrixf(AmigaGetGLcontext(libbase),m);}
void APIENTRY smglRotated(A6(void* libbase),FP0(GLdouble angle),FP1(GLdouble x),FP2(GLdouble y),FP3(GLdouble z))	{aglRotated(AmigaGetGLcontext(libbase),angle,x,y,z);}
void APIENTRY smglRotatef(A6(void* libbase),FP0(GLfloat angle),FP1(GLfloat x),FP2(GLfloat y),FP3(GLfloat z))	{aglRotatef(AmigaGetGLcontext(libbase),angle,x,y,z);}
void APIENTRY smglScaled(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z))	{aglScaled(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglScalef(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z))	{aglScalef(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglTranslated(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z))	{aglTranslated(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglTranslatef(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z))	{aglTranslatef(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglDeleteLists(A6(void* libbase),D0(GLuint list),D1(GLsizei range))	{aglDeleteLists(AmigaGetGLcontext(libbase),list,range);}
void APIENTRY smglNewList(A6(void* libbase),D0(GLuint list),D1(GLenum mode))	{aglNewList(AmigaGetGLcontext(libbase),list,mode);}
void APIENTRY smglEndList(A6(void* libbase))	{aglEndList(AmigaGetGLcontext(libbase));}
void APIENTRY smglCallList(A6(void* libbase),D0(GLuint list))	{aglCallList(AmigaGetGLcontext(libbase),list);}
void APIENTRY smglCallLists(A6(void* libbase),D0(GLsizei n),D1(GLenum type),A0(const GLvoid* lists))	{aglCallLists(AmigaGetGLcontext(libbase),n,type,lists);}
void APIENTRY smglListBase(A6(void* libbase),D0(GLuint base))	{aglListBase(AmigaGetGLcontext(libbase),base);}
void APIENTRY smglBegin(A6(void* libbase),D0(GLenum mode))	{aglBegin(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglEnd(A6(void* libbase))	{aglEnd(AmigaGetGLcontext(libbase));}
void APIENTRY smglVertex2d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y))	{aglVertex2d(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglVertex2f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y))	{aglVertex2f(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglVertex2i(A6(void* libbase),D0(GLint x),D1(GLint y))	{aglVertex2i(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglVertex2s(A6(void* libbase),D0(GLshort x),D1(GLshort y))	{aglVertex2s(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglVertex3d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z))	{aglVertex3d(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglVertex3f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z))	{aglVertex3f(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglVertex3i(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z))	{aglVertex3i(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglVertex3s(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z))	{aglVertex3s(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglVertex4d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z),FP3(GLdouble w))	{aglVertex4d(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglVertex4f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z),FP3(GLfloat w))	{aglVertex4f(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglVertex4i(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z),D3(GLint w))	{aglVertex4i(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglVertex4s(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z),D3(GLshort w))	{aglVertex4s(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglVertex2dv(A6(void* libbase),A0(const GLdouble* v))	{aglVertex2dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex2fv(A6(void* libbase),A0(const GLfloat* v))	{aglVertex2fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex2iv(A6(void* libbase),A0(const GLint* v))	{aglVertex2iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex2sv(A6(void* libbase),A0(const GLshort* v))	{aglVertex2sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex3dv(A6(void* libbase),A0(const GLdouble* v))	{aglVertex3dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex3fv(A6(void* libbase),A0(const GLfloat* v))	{aglVertex3fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex3iv(A6(void* libbase),A0(const GLint* v))	{aglVertex3iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex3sv(A6(void* libbase),A0(const GLshort* v))	{aglVertex3sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex4dv(A6(void* libbase),A0(const GLdouble* v))	{aglVertex4dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex4fv(A6(void* libbase),A0(const GLfloat* v))	{aglVertex4fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex4iv(A6(void* libbase),A0(const GLint* v))	{aglVertex4iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglVertex4sv(A6(void* libbase),A0(const GLshort* v))	{aglVertex4sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglNormal3b(A6(void* libbase),D0(GLbyte nx),D1(GLbyte ny),D2(GLbyte nz))	{aglNormal3b(AmigaGetGLcontext(libbase),nx,ny,nz);}
void APIENTRY smglNormal3d(A6(void* libbase),FP0(GLdouble nx),FP1(GLdouble ny),FP2(GLdouble nz))	{aglNormal3d(AmigaGetGLcontext(libbase),nx,ny,nz);}
void APIENTRY smglNormal3f(A6(void* libbase),FP0(GLfloat nx),FP1(GLfloat ny),FP2(GLfloat nz))	{aglNormal3f(AmigaGetGLcontext(libbase),nx,ny,nz);}
void APIENTRY smglNormal3i(A6(void* libbase),D0(GLint nx),D1(GLint ny),D2(GLint nz))	{aglNormal3i(AmigaGetGLcontext(libbase),nx,ny,nz);}
void APIENTRY smglNormal3s(A6(void* libbase),D0(GLshort nx),D1(GLshort ny),D2(GLshort nz))	{aglNormal3s(AmigaGetGLcontext(libbase),nx,ny,nz);}
void APIENTRY smglNormal3bv(A6(void* libbase),A0(const GLbyte* v))	{aglNormal3bv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglNormal3dv(A6(void* libbase),A0(const GLdouble* v))	{aglNormal3dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglNormal3fv(A6(void* libbase),A0(const GLfloat* v))	{aglNormal3fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglNormal3iv(A6(void* libbase),A0(const GLint* v))	{aglNormal3iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglNormal3sv(A6(void* libbase),A0(const GLshort* v))	{aglNormal3sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglIndexd(A6(void* libbase),FP0(GLdouble c))	{aglIndexd(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexf(A6(void* libbase),FP0(GLfloat c))	{aglIndexf(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexi(A6(void* libbase),D0(GLint c))	{aglIndexi(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexs(A6(void* libbase),D0(GLshort c))	{aglIndexs(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexub(A6(void* libbase),D0(GLshort c))	{aglIndexub(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexdv(A6(void* libbase),A0(const GLdouble* c))	{aglIndexdv(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexfv(A6(void* libbase),A0(const GLfloat* c))	{aglIndexfv(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexiv(A6(void* libbase),A0(const GLint* c))	{aglIndexiv(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexsv(A6(void* libbase),A0(const GLshort* c))	{aglIndexsv(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglIndexubv(A6(void* libbase),A0(const GLubyte* c))	{aglIndexubv(AmigaGetGLcontext(libbase),c);}
void APIENTRY smglColor3b(A6(void* libbase),D0(GLbyte red),D1(GLbyte green),D2(GLbyte blue))	{aglColor3b(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3d(A6(void* libbase),FP0(GLdouble red),FP1(GLdouble green),FP2(GLdouble blue))	{aglColor3d(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3f(A6(void* libbase),FP0(GLfloat red),FP1(GLfloat green),FP2(GLfloat blue))	{aglColor3f(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3i(A6(void* libbase),D0(GLint red),D1(GLint green),D2(GLint blue))	{aglColor3i(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3s(A6(void* libbase),D0(GLshort red),D1(GLshort green),D2(GLshort blue))	{aglColor3s(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3ub(A6(void* libbase),D0(GLubyte red),D1(GLubyte green),D2(GLubyte blue))	{aglColor3ub(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3ui(A6(void* libbase),D0(GLuint red),D1(GLuint green),D2(GLuint blue))	{aglColor3ui(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor3us(A6(void* libbase),D0(GLushort red),D1(GLushort green),D2(GLushort blue))	{aglColor3us(AmigaGetGLcontext(libbase),red,green,blue);}
void APIENTRY smglColor4b(A6(void* libbase),D0(GLbyte red),D1(GLbyte green),D2(GLbyte blue),D3(GLbyte alpha))	{aglColor4b(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4d(A6(void* libbase),FP0(GLdouble red),FP1(GLdouble green),FP2(GLdouble blue),FP3(GLdouble alpha))	{aglColor4d(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4f(A6(void* libbase),FP0(GLfloat red),FP1(GLfloat green),FP2(GLfloat blue),FP3(GLfloat alpha))	{aglColor4f(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4i(A6(void* libbase),D0(GLint red),D1(GLint green),D2(GLint blue),D3(GLint alpha))	{aglColor4i(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4s(A6(void* libbase),D0(GLshort red),D1(GLshort green),D2(GLshort blue),D3(GLshort alpha))	{aglColor4s(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4ub(A6(void* libbase),D0(GLubyte red),D1(GLubyte green),D2(GLubyte blue),D3(GLubyte alpha))	{aglColor4ub(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4ui(A6(void* libbase),D0(GLuint red),D1(GLuint green),D2(GLuint blue),D3(GLuint alpha))	{aglColor4ui(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor4us(A6(void* libbase),D0(GLushort red),D1(GLushort green),D2(GLushort blue),D3(GLushort alpha))	{aglColor4us(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglColor3bv(A6(void* libbase),A0(const GLbyte* v))	{aglColor3bv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3dv(A6(void* libbase),A0(const GLdouble* v))	{aglColor3dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3fv(A6(void* libbase),A0(const GLfloat* v))	{aglColor3fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3iv(A6(void* libbase),A0(const GLint* v))	{aglColor3iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3sv(A6(void* libbase),A0(const GLshort* v))	{aglColor3sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3ubv(A6(void* libbase),A0(const GLubyte* v))	{aglColor3ubv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3uiv(A6(void* libbase),A0(const GLuint* v))	{aglColor3uiv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor3usv(A6(void* libbase),A0(const GLushort* v))	{aglColor3usv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4bv(A6(void* libbase),A0(const GLbyte* v))	{aglColor4bv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4dv(A6(void* libbase),A0(const GLdouble* v))	{aglColor4dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4fv(A6(void* libbase),A0(const GLfloat* v))	{aglColor4fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4iv(A6(void* libbase),A0(const GLint* v))	{aglColor4iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4sv(A6(void* libbase),A0(const GLshort* v))	{aglColor4sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4ubv(A6(void* libbase),A0(const GLubyte* v))	{aglColor4ubv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4uiv(A6(void* libbase),A0(const GLuint* v))	{aglColor4uiv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglColor4usv(A6(void* libbase),A0(const GLushort* v))	{aglColor4usv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord1d(A6(void* libbase),FP0(GLdouble s))	{aglTexCoord1d(AmigaGetGLcontext(libbase),s);}
void APIENTRY smglTexCoord1f(A6(void* libbase),FP0(GLfloat s))	{aglTexCoord1f(AmigaGetGLcontext(libbase),s);}
void APIENTRY smglTexCoord1i(A6(void* libbase),D0(GLint s))	{aglTexCoord1i(AmigaGetGLcontext(libbase),s);}
void APIENTRY smglTexCoord1s(A6(void* libbase),D0(GLshort s))	{aglTexCoord1s(AmigaGetGLcontext(libbase),s);}
void APIENTRY smglTexCoord2d(A6(void* libbase),FP0(GLdouble s),FP1(GLdouble t))	{aglTexCoord2d(AmigaGetGLcontext(libbase),s,t);}
void APIENTRY smglTexCoord2f(A6(void* libbase),FP0(GLfloat s),FP1(GLfloat t))	{aglTexCoord2f(AmigaGetGLcontext(libbase),s,t);}
void APIENTRY smglTexCoord2i(A6(void* libbase),D0(GLint s),D1(GLint t))	{aglTexCoord2i(AmigaGetGLcontext(libbase),s,t);}
void APIENTRY smglTexCoord2s(A6(void* libbase),D0(GLshort s),D1(GLshort t))	{aglTexCoord2s(AmigaGetGLcontext(libbase),s,t);}
void APIENTRY smglTexCoord3d(A6(void* libbase),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r))	{aglTexCoord3d(AmigaGetGLcontext(libbase),s,t,r);}
void APIENTRY smglTexCoord3f(A6(void* libbase),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r))	{aglTexCoord3f(AmigaGetGLcontext(libbase),s,t,r);}
void APIENTRY smglTexCoord3i(A6(void* libbase),D0(GLint s),D1(GLint t),D2(GLint r))	{aglTexCoord3i(AmigaGetGLcontext(libbase),s,t,r);}
void APIENTRY smglTexCoord3s(A6(void* libbase),D0(GLshort s),D1(GLshort t),D2(GLshort r))	{aglTexCoord3s(AmigaGetGLcontext(libbase),s,t,r);}
void APIENTRY smglTexCoord4d(A6(void* libbase),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r),FP3(GLdouble q))	{aglTexCoord4d(AmigaGetGLcontext(libbase),s,t,r,q);}
void APIENTRY smglTexCoord4f(A6(void* libbase),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r),FP3(GLfloat q))	{aglTexCoord4f(AmigaGetGLcontext(libbase),s,t,r,q);}
void APIENTRY smglTexCoord4i(A6(void* libbase),D0(GLint s),D1(GLint t),D2(GLint r),D3(GLint q))	{aglTexCoord4i(AmigaGetGLcontext(libbase),s,t,r,q);}
void APIENTRY smglTexCoord4s(A6(void* libbase),D0(GLshort s),D1(GLshort t),D2(GLshort r),D3(GLshort q))	{aglTexCoord4s(AmigaGetGLcontext(libbase),s,t,r,q);}
void APIENTRY smglTexCoord1dv(A6(void* libbase),A0(const GLdouble* v))	{aglTexCoord1dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord1fv(A6(void* libbase),A0(const GLfloat* v))	{aglTexCoord1fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord1iv(A6(void* libbase),A0(const GLint* v))	{aglTexCoord1iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord1sv(A6(void* libbase),A0(const GLshort* v))	{aglTexCoord1sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord2dv(A6(void* libbase),A0(const GLdouble* v))	{aglTexCoord2dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord2fv(A6(void* libbase),A0(const GLfloat* v))	{aglTexCoord2fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord2iv(A6(void* libbase),A0(const GLint* v))	{aglTexCoord2iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord2sv(A6(void* libbase),A0(const GLshort* v))	{aglTexCoord2sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord3dv(A6(void* libbase),A0(const GLdouble* v))	{aglTexCoord3dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord3fv(A6(void* libbase),A0(const GLfloat* v))	{aglTexCoord3fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord3iv(A6(void* libbase),A0(const GLint* v))	{aglTexCoord3iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord3sv(A6(void* libbase),A0(const GLshort* v))	{aglTexCoord3sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord4dv(A6(void* libbase),A0(const GLdouble* v))	{aglTexCoord4dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord4fv(A6(void* libbase),A0(const GLfloat* v))	{aglTexCoord4fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord4iv(A6(void* libbase),A0(const GLint* v))	{aglTexCoord4iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglTexCoord4sv(A6(void* libbase),A0(const GLshort* v))	{aglTexCoord4sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos2d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y))	{aglRasterPos2d(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglRasterPos2f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y))	{aglRasterPos2f(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglRasterPos2i(A6(void* libbase),D0(GLint x),D1(GLint y))	{aglRasterPos2i(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglRasterPos2s(A6(void* libbase),D0(GLshort x),D1(GLshort y))	{aglRasterPos2s(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglRasterPos3d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z))	{aglRasterPos3d(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglRasterPos3f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z))	{aglRasterPos3f(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglRasterPos3i(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z))	{aglRasterPos3i(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglRasterPos3s(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z))	{aglRasterPos3s(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglRasterPos4d(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z),FP3(GLdouble w))	{aglRasterPos4d(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglRasterPos4f(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z),FP3(GLfloat w))	{aglRasterPos4f(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglRasterPos4i(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z),D3(GLint w))	{aglRasterPos4i(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglRasterPos4s(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z),D3(GLshort w))	{aglRasterPos4s(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglRasterPos2dv(A6(void* libbase),A0(const GLdouble* v))	{aglRasterPos2dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos2fv(A6(void* libbase),A0(const GLfloat* v))	{aglRasterPos2fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos2iv(A6(void* libbase),A0(const GLint* v))	{aglRasterPos2iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos2sv(A6(void* libbase),A0(const GLshort* v))	{aglRasterPos2sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos3dv(A6(void* libbase),A0(const GLdouble* v))	{aglRasterPos3dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos3fv(A6(void* libbase),A0(const GLfloat* v))	{aglRasterPos3fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos3iv(A6(void* libbase),A0(const GLint* v))	{aglRasterPos3iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos3sv(A6(void* libbase),A0(const GLshort* v))	{aglRasterPos3sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos4dv(A6(void* libbase),A0(const GLdouble* v))	{aglRasterPos4dv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos4fv(A6(void* libbase),A0(const GLfloat* v))	{aglRasterPos4fv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos4iv(A6(void* libbase),A0(const GLint* v))	{aglRasterPos4iv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRasterPos4sv(A6(void* libbase),A0(const GLshort* v))	{aglRasterPos4sv(AmigaGetGLcontext(libbase),v);}
void APIENTRY smglRectd(A6(void* libbase),FP0(GLdouble x1),FP1(GLdouble y1),FP2(GLdouble x2),FP3(GLdouble y2))	{aglRectd(AmigaGetGLcontext(libbase),x1,y1,x2,y2);}
void APIENTRY smglRectf(A6(void* libbase),FP0(GLfloat x1),FP1(GLfloat y1),FP2(GLfloat x2),FP3(GLfloat y2))	{aglRectf(AmigaGetGLcontext(libbase),x1,y1,x2,y2);}
void APIENTRY smglRecti(A6(void* libbase),D0(GLint x1),D1(GLint y1),D2(GLint x2),D3(GLint y2))	{aglRecti(AmigaGetGLcontext(libbase),x1,y1,x2,y2);}
void APIENTRY smglRects(A6(void* libbase),D0(GLshort x1),D1(GLshort y1),D2(GLshort x2),D3(GLshort y2))	{aglRects(AmigaGetGLcontext(libbase),x1,y1,x2,y2);}
void APIENTRY smglRectdv(A6(void* libbase),A0(const GLdouble* v1),A1(const GLdouble* v2))	{aglRectdv(AmigaGetGLcontext(libbase),v1,v2);}
void APIENTRY smglRectfv(A6(void* libbase),A0(const GLfloat* v1),A1(const GLfloat* v2))	{aglRectfv(AmigaGetGLcontext(libbase),v1,v2);}
void APIENTRY smglRectiv(A6(void* libbase),A0(const GLint* v1),A1(const GLint* v2))	{aglRectiv(AmigaGetGLcontext(libbase),v1,v2);}
void APIENTRY smglRectsv(A6(void* libbase),A0(const GLshort* v1),A1(const GLshort* v2))	{aglRectsv(AmigaGetGLcontext(libbase),v1,v2);}
void APIENTRY smglVertexPointer(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),A0(const GLvoid* ptr))	{aglVertexPointer(AmigaGetGLcontext(libbase),size,type,stride,ptr);}
void APIENTRY smglNormalPointer(A6(void* libbase),D0(GLenum type),D1(GLsizei stride),A0(const GLvoid* ptr))	{aglNormalPointer(AmigaGetGLcontext(libbase),type,stride,ptr);}
void APIENTRY smglColorPointer(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),A0(const GLvoid* ptr))	{aglColorPointer(AmigaGetGLcontext(libbase),size,type,stride,ptr);}
void APIENTRY smglIndexPointer(A6(void* libbase),D0(GLenum type),D1(GLsizei stride),A0(const GLvoid* ptr))	{aglIndexPointer(AmigaGetGLcontext(libbase),type,stride,ptr);}
void APIENTRY smglTexCoordPointer(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),A0(const GLvoid* ptr))	{aglTexCoordPointer(AmigaGetGLcontext(libbase),size,type,stride,ptr);}
void APIENTRY smglEdgeFlagPointer(A6(void* libbase),D0(GLsizei stride),A0(const GLboolean* ptr))	{aglEdgeFlagPointer(AmigaGetGLcontext(libbase),stride,ptr);}
void APIENTRY smglGetPointerv(A6(void* libbase),D0(GLenum pname),A0(GLvoid* *params))	{aglGetPointerv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglArrayElement(A6(void* libbase),D0(GLint i))	{aglArrayElement(AmigaGetGLcontext(libbase),i);}
void APIENTRY smglDrawArrays(A6(void* libbase),D0(GLenum mode),D1(GLint first),D2(GLsizei count))	{aglDrawArrays(AmigaGetGLcontext(libbase),mode,first,count);}
void APIENTRY smglDrawElements(A6(void* libbase),D0(GLenum mode),D1(GLsizei count),D2(GLenum type),A0(const GLvoid* indices))	{aglDrawElements(AmigaGetGLcontext(libbase),mode,count,type,indices);}
void APIENTRY smglInterleavedArrays(A6(void* libbase),D0(GLenum format),D1(GLsizei stride),A0(const GLvoid* pointer))	{aglInterleavedArrays(AmigaGetGLcontext(libbase),format,stride,pointer);}
void APIENTRY smglShadeModel(A6(void* libbase),D0(GLenum mode))	{aglShadeModel(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglLightf(A6(void* libbase),D0(GLenum light),D1(GLenum pname),FP0(GLfloat param))	{aglLightf(AmigaGetGLcontext(libbase),light,pname,param);}
void APIENTRY smglLighti(A6(void* libbase),D0(GLenum light),D1(GLenum pname),D2(GLint param))	{aglLighti(AmigaGetGLcontext(libbase),light,pname,param);}
void APIENTRY smglLightfv(A6(void* libbase),D0(GLenum light),D1(GLenum pname),A0(const GLfloat* params))	{aglLightfv(AmigaGetGLcontext(libbase),light,pname,params);}
void APIENTRY smglLightiv(A6(void* libbase),D0(GLenum light),D1(GLenum pname),A0(const GLint* params))	{aglLightiv(AmigaGetGLcontext(libbase),light,pname,params);}
void APIENTRY smglGetLightfv(A6(void* libbase),D0(GLenum light),D1(GLenum pname),A0(GLfloat* params))	{aglGetLightfv(AmigaGetGLcontext(libbase),light,pname,params);}
void APIENTRY smglGetLightiv(A6(void* libbase),D0(GLenum light),D1(GLenum pname),A0(GLint* params))	{aglGetLightiv(AmigaGetGLcontext(libbase),light,pname,params);}
void APIENTRY smglLightModelf(A6(void* libbase),D0(GLenum pname),FP0(GLfloat param))	{aglLightModelf(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglLightModeli(A6(void* libbase),D0(GLenum pname),D1(GLint param))	{aglLightModeli(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglLightModelfv(A6(void* libbase),D0(GLenum pname),A0(const GLfloat* params))	{aglLightModelfv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglLightModeliv(A6(void* libbase),D0(GLenum pname),A0(const GLint* params))	{aglLightModeliv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglMaterialf(A6(void* libbase),D0(GLenum face),D1(GLenum pname),FP0(GLfloat param))	{aglMaterialf(AmigaGetGLcontext(libbase),face,pname,param);}
void APIENTRY smglMateriali(A6(void* libbase),D0(GLenum face),D1(GLenum pname),D2(GLint param))	{aglMateriali(AmigaGetGLcontext(libbase),face,pname,param);}
void APIENTRY smglMaterialfv(A6(void* libbase),D0(GLenum face),D1(GLenum pname),A0(const GLfloat* params))	{aglMaterialfv(AmigaGetGLcontext(libbase),face,pname,params);}
void APIENTRY smglMaterialiv(A6(void* libbase),D0(GLenum face),D1(GLenum pname),A0(const GLint* params))	{aglMaterialiv(AmigaGetGLcontext(libbase),face,pname,params);}
void APIENTRY smglGetMaterialfv(A6(void* libbase),D0(GLenum face),D1(GLenum pname),A0(GLfloat* params))	{aglGetMaterialfv(AmigaGetGLcontext(libbase),face,pname,params);}
void APIENTRY smglGetMaterialiv(A6(void* libbase),D0(GLenum face),D1(GLenum pname),A0(GLint* params))	{aglGetMaterialiv(AmigaGetGLcontext(libbase),face,pname,params);}
void APIENTRY smglColorMaterial(A6(void* libbase),D0(GLenum face),D1(GLenum mode))	{aglColorMaterial(AmigaGetGLcontext(libbase),face,mode);}
void APIENTRY smglPixelZoom(A6(void* libbase),FP0(GLfloat xfactor),FP1(GLfloat yfactor))	{aglPixelZoom(AmigaGetGLcontext(libbase),xfactor,yfactor);}
void APIENTRY smglPixelStoref(A6(void* libbase),D0(GLenum pname),FP0(GLfloat param))	{aglPixelStoref(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglPixelStorei(A6(void* libbase),D0(GLenum pname),D1(GLint param))	{aglPixelStorei(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglPixelTransferf(A6(void* libbase),D0(GLenum pname),FP0(GLfloat param))	{aglPixelTransferf(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglPixelTransferi(A6(void* libbase),D0(GLenum pname),D1(GLint param))	{aglPixelTransferi(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglPixelMapfv(A6(void* libbase),D0(GLenum map),D1(GLint mapsize),A0(const GLfloat* values))	{aglPixelMapfv(AmigaGetGLcontext(libbase),map,mapsize,values);}
void APIENTRY smglPixelMapuiv(A6(void* libbase),D0(GLenum map),D1(GLint mapsize),A0(const GLuint* values))	{aglPixelMapuiv(AmigaGetGLcontext(libbase),map,mapsize,values);}
void APIENTRY smglPixelMapusv(A6(void* libbase),D0(GLenum map),D1(GLint mapsize),A0(const GLushort* values))	{aglPixelMapusv(AmigaGetGLcontext(libbase),map,mapsize,values);}
void APIENTRY smglGetPixelMapfv(A6(void* libbase),D0(GLenum map),A0(GLfloat* values))	{aglGetPixelMapfv(AmigaGetGLcontext(libbase),map,values);}
void APIENTRY smglGetPixelMapuiv(A6(void* libbase),D0(GLenum map),A0(GLuint* values))	{aglGetPixelMapuiv(AmigaGetGLcontext(libbase),map,values);}
void APIENTRY smglGetPixelMapusv(A6(void* libbase),D0(GLenum map),A0(GLushort* values))	{aglGetPixelMapusv(AmigaGetGLcontext(libbase),map,values);}
void APIENTRY smglBitmap(A6(void* libbase),D0(GLsizei width),D1(GLsizei height),FP0(GLfloat xorig),FP1(GLfloat yorig),FP2(GLfloat xmove),FP3(GLfloat ymove),A0(const GLubyte* bitmap))	{aglBitmap(AmigaGetGLcontext(libbase),width,height,xorig,yorig,xmove,ymove,bitmap);}
void APIENTRY smglReadPixels(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLsizei width),D3(GLsizei height),D4(GLenum format),D5(GLenum type),A0(GLvoid* pixels))	{aglReadPixels(AmigaGetGLcontext(libbase),x,y,width,height,format,type,pixels);}
void APIENTRY smglDrawPixels(A6(void* libbase),D0(GLsizei width),D1(GLsizei height),D2(GLenum format),D3(GLenum type),A0(const GLvoid* pixels))	{aglDrawPixels(AmigaGetGLcontext(libbase),width,height,format,type,pixels);}
void APIENTRY smglCopyPixels(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLsizei width),D3(GLsizei height),D4(GLenum type))	{aglCopyPixels(AmigaGetGLcontext(libbase),x,y,width,height,type);}
void APIENTRY smglStencilFunc(A6(void* libbase),D0(GLenum func),D1(GLint ref),D2(GLuint mask))	{aglStencilFunc(AmigaGetGLcontext(libbase),func,ref,mask);}
void APIENTRY smglStencilMask(A6(void* libbase),D0(GLuint mask))	{aglStencilMask(AmigaGetGLcontext(libbase),mask);}
void APIENTRY smglStencilOp(A6(void* libbase),D0(GLenum fail),D1(GLenum zfail),D2(GLenum zpass))	{aglStencilOp(AmigaGetGLcontext(libbase),fail,zfail,zpass);}
void APIENTRY smglClearStencil(A6(void* libbase),D0(GLint s))	{aglClearStencil(AmigaGetGLcontext(libbase),s);}
void APIENTRY smglTexGend(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),FP0(GLdouble param))	{aglTexGend(AmigaGetGLcontext(libbase),coord,pname,param);}
void APIENTRY smglTexGenf(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),FP0(GLfloat param))	{aglTexGenf(AmigaGetGLcontext(libbase),coord,pname,param);}
void APIENTRY smglTexGeni(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),D2(GLint param))	{aglTexGeni(AmigaGetGLcontext(libbase),coord,pname,param);}
void APIENTRY smglTexGendv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(const GLdouble* params))	{aglTexGendv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglTexGenfv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(const GLfloat* params))	{aglTexGenfv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglTexGeniv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(const GLint* params))	{aglTexGeniv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglGetTexGendv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(GLdouble* params))	{aglGetTexGendv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglGetTexGenfv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(GLfloat* params))	{aglGetTexGenfv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglGetTexGeniv(A6(void* libbase),D0(GLenum coord),D1(GLenum pname),A0(GLint* params))	{aglGetTexGeniv(AmigaGetGLcontext(libbase),coord,pname,params);}
void APIENTRY smglTexEnvf(A6(void* libbase),D0(GLenum target),D1(GLenum pname),FP0(GLfloat param))	{aglTexEnvf(AmigaGetGLcontext(libbase),target,pname,param);}
void APIENTRY smglTexEnvi(A6(void* libbase),D0(GLenum target),D1(GLenum pname),D2(GLint param))	{aglTexEnvi(AmigaGetGLcontext(libbase),target,pname,param);}
void APIENTRY smglTexEnvfv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(const GLfloat* params))	{aglTexEnvfv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglTexEnviv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(const GLint* params))	{aglTexEnviv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetTexEnvfv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLfloat* params))	{aglGetTexEnvfv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetTexEnviv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLint* params))	{aglGetTexEnviv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglTexParameterf(A6(void* libbase),D0(GLenum target),D1(GLenum pname),FP0(GLfloat param))	{aglTexParameterf(AmigaGetGLcontext(libbase),target,pname,param);}
void APIENTRY smglTexParameteri(A6(void* libbase),D0(GLenum target),D1(GLenum pname),D2(GLint param))	{aglTexParameteri(AmigaGetGLcontext(libbase),target,pname,param);}
void APIENTRY smglTexParameterfv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(const GLfloat* params))	{aglTexParameterfv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglTexParameteriv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(const GLint* params))	{aglTexParameteriv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetTexParameterfv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLfloat* params))	{aglGetTexParameterfv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetTexParameteriv(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLint* params))	{aglGetTexParameteriv(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetTexLevelParameterfv(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum pname),A0(GLfloat* params))	{aglGetTexLevelParameterfv(AmigaGetGLcontext(libbase),target,level,pname,params);}
void APIENTRY smglGetTexLevelParameteriv(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum pname),A0(GLint* params))	{aglGetTexLevelParameteriv(AmigaGetGLcontext(libbase),target,level,pname,params);}
void APIENTRY smglTexImage1D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint components),D3(GLsizei width),D4(GLint border),D5(GLenum format),D6(GLenum type),A0(const GLvoid* pixels))	{aglTexImage1D(AmigaGetGLcontext(libbase),target,level,components,width,border,format,type,pixels);}
void APIENTRY smglTexImage2D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint components),D3(GLsizei width),D4(GLsizei height),D5(GLint border),D6(GLenum format),D7(GLenum type),A0(const GLvoid *pixels))	{aglTexImage2D(AmigaGetGLcontext(libbase),target,level,components,width,height,border,format,type,pixels);}
void APIENTRY smglGetTexImage(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum format),D3(GLenum type),A0(GLvoid* pixels))	{aglGetTexImage(AmigaGetGLcontext(libbase),target,level,format,type,pixels);}
void APIENTRY smglGenTextures(A6(void* libbase),D0(GLsizei n),A0(GLuint* textures))	{aglGenTextures(AmigaGetGLcontext(libbase),n,textures);}
void APIENTRY smglDeleteTextures(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures))	{aglDeleteTextures(AmigaGetGLcontext(libbase),n,textures);}
void APIENTRY smglBindTexture(A6(void* libbase),D0(GLenum target),D1(GLuint texture))	{aglBindTexture(AmigaGetGLcontext(libbase),target,texture);}
void APIENTRY smglPrioritizeTextures(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures),A1(const GLclampf* priorities))	{aglPrioritizeTextures(AmigaGetGLcontext(libbase),n,textures,priorities);}
void APIENTRY smglTexSubImage1D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLsizei width),D4(GLenum format),D5(GLenum type),A0(const GLvoid* pixels))	{aglTexSubImage1D(AmigaGetGLcontext(libbase),target,level,xoffset,width,format,type,pixels);}
void APIENTRY smglTexSubImage2D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLsizei width),D5(GLsizei height),D6(GLenum format),D7(GLenum type),A0(GLvoid *pixels))	{aglTexSubImage2D(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,width,height,format,type,pixels);}
void APIENTRY smglCopyTexImage1D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum internalformat),D3(GLint x),D4(GLint y),D5(GLsizei width),D6(GLint border))	{aglCopyTexImage1D(AmigaGetGLcontext(libbase),target,level,internalformat,x,y,width,border);}
void APIENTRY smglCopyTexImage2D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum internalformat),D3(GLint x),D4(GLint y),D5(GLsizei width),D6(GLsizei height),D7(GLint border))	{aglCopyTexImage2D(AmigaGetGLcontext(libbase),target,level,internalformat,x,y,width,height,border);}
void APIENTRY smglCopyTexSubImage1D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint x),D4(GLint y),D5(GLsizei width))	{aglCopyTexSubImage1D(AmigaGetGLcontext(libbase),target,level,xoffset,x,y,width);}
void APIENTRY smglCopyTexSubImage2D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLint x),D5(GLint y),D6(GLsizei width),D7(GLsizei height))	{aglCopyTexSubImage2D(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,x,y,width,height);}
void APIENTRY smglMap1d(A6(void* libbase),D0(GLenum target),FP0(GLdouble u1),FP1(GLdouble u2),D1(GLint stride),D2(GLint order),A0(const GLdouble* points))	{aglMap1d(AmigaGetGLcontext(libbase),target,u1,u2,stride,order,points);}
void APIENTRY smglMap1f(A6(void* libbase),D0(GLenum target),FP0(GLfloat u1),FP1(GLfloat u2),D1(GLint stride),D2(GLint order),A0(const GLfloat* points))	{aglMap1f(AmigaGetGLcontext(libbase),target,u1,u2,stride,order,points);}
void APIENTRY smglMap2d(A6(void* libbase),D0(GLenum target),FP0(GLdouble u1),FP1(GLdouble u2),D1(GLint ustride),D2(GLint uorder),FP2(GLdouble v1),FP3(GLdouble v2),D3(GLint vstride),D4(GLint vorder),A0(const GLdouble* points))	{aglMap2d(AmigaGetGLcontext(libbase),target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);}
void APIENTRY smglMap2f(A6(void* libbase),D0(GLenum target),FP0(GLfloat u1),FP1(GLfloat u2),D1(GLint ustride),D2(GLint uorder),FP2(GLfloat v1),FP3(GLfloat v2),D3(GLint vstride),D4(GLint vorder),A0(GLfloat* points))	{aglMap2f(AmigaGetGLcontext(libbase),target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);}
void APIENTRY smglGetMapdv(A6(void* libbase),D0(GLenum target),D1(GLenum query),A0(GLdouble* v))	{aglGetMapdv(AmigaGetGLcontext(libbase),target,query,v);}
void APIENTRY smglGetMapfv(A6(void* libbase),D0(GLenum target),D1(GLenum query),A0(GLfloat* v))	{aglGetMapfv(AmigaGetGLcontext(libbase),target,query,v);}
void APIENTRY smglGetMapiv(A6(void* libbase),D0(GLenum target),D1(GLenum query),A0(GLint* v))	{aglGetMapiv(AmigaGetGLcontext(libbase),target,query,v);}
void APIENTRY smglEvalCoord1d(A6(void* libbase),FP0(GLdouble u))	{aglEvalCoord1d(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglEvalCoord1f(A6(void* libbase),FP0(GLfloat u))	{aglEvalCoord1f(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglEvalCoord1dv(A6(void* libbase),A0(const GLdouble* u))	{aglEvalCoord1dv(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglEvalCoord1fv(A6(void* libbase),A0(const GLfloat* u))	{aglEvalCoord1fv(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglEvalCoord2d(A6(void* libbase),FP0(GLdouble u),FP1(GLdouble v))	{aglEvalCoord2d(AmigaGetGLcontext(libbase),u,v);}
void APIENTRY smglEvalCoord2f(A6(void* libbase),FP0(GLfloat u),FP1(GLfloat v))	{aglEvalCoord2f(AmigaGetGLcontext(libbase),u,v);}
void APIENTRY smglEvalCoord2dv(A6(void* libbase),A0(const GLdouble* u))	{aglEvalCoord2dv(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglEvalCoord2fv(A6(void* libbase),A0(const GLfloat* u))	{aglEvalCoord2fv(AmigaGetGLcontext(libbase),u);}
void APIENTRY smglMapGrid1d(A6(void* libbase),D0(GLint un),FP0(GLdouble u1),FP1(GLdouble u2))	{aglMapGrid1d(AmigaGetGLcontext(libbase),un,u1,u2);}
void APIENTRY smglMapGrid1f(A6(void* libbase),D0(GLint un),FP0(GLfloat u1),FP1(GLfloat u2))	{aglMapGrid1f(AmigaGetGLcontext(libbase),un,u1,u2);}
void APIENTRY smglMapGrid2d(A6(void* libbase),D0(GLint un),FP0(GLdouble u1),FP1(GLdouble u2),D1(GLint vn),FP2(GLdouble v1),FP3(GLdouble v2))	{aglMapGrid2d(AmigaGetGLcontext(libbase),un,u1,u2,vn,v1,v2);}
void APIENTRY smglMapGrid2f(A6(void* libbase),D0(GLint un),FP0(GLfloat u1),FP1(GLfloat u2),D1(GLint vn),FP2(GLfloat v1),FP3(GLfloat v2))	{aglMapGrid2f(AmigaGetGLcontext(libbase),un,u1,u2,vn,v1,v2);}
void APIENTRY smglEvalPoint1(A6(void* libbase),D0(GLint i))	{aglEvalPoint1(AmigaGetGLcontext(libbase),i);}
void APIENTRY smglEvalPoint2(A6(void* libbase),D0(GLint i),D1(GLint j))	{aglEvalPoint2(AmigaGetGLcontext(libbase),i,j);}
void APIENTRY smglEvalMesh1(A6(void* libbase),D0(GLenum mode),D1(GLint i1),D2(GLint i2))	{aglEvalMesh1(AmigaGetGLcontext(libbase),mode,i1,i2);}
void APIENTRY smglEvalMesh2(A6(void* libbase),D0(GLenum mode),D1(GLint i1),D2(GLint i2),D3(GLint j1),D4(GLint j2))	{aglEvalMesh2(AmigaGetGLcontext(libbase),mode,i1,i2,j1,j2);}
void APIENTRY smglFogf(A6(void* libbase),D0(GLenum pname),FP0(GLfloat param))	{aglFogf(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglFogi(A6(void* libbase),D0(GLenum pname),D1(GLint param))	{aglFogi(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglFogfv(A6(void* libbase),D0(GLenum pname),A0(const GLfloat* params))	{aglFogfv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglFogiv(A6(void* libbase),D0(GLenum pname),A0(const GLint* params))	{aglFogiv(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglFeedbackBuffer(A6(void* libbase),D0(GLsizei size),D1(GLenum type),A0(GLfloat* buffer))	{aglFeedbackBuffer(AmigaGetGLcontext(libbase),size,type,buffer);}
void APIENTRY smglPassThrough(A6(void* libbase),FP0(GLfloat token))	{aglPassThrough(AmigaGetGLcontext(libbase),token);}
void APIENTRY smglSelectBuffer(A6(void* libbase),D0(GLsizei size),A0(GLuint* buffer))	{aglSelectBuffer(AmigaGetGLcontext(libbase),size,buffer);}
void APIENTRY smglInitNames(A6(void* libbase))	{aglInitNames(AmigaGetGLcontext(libbase));}
void APIENTRY smglLoadName(A6(void* libbase),D0(GLuint name))	{aglLoadName(AmigaGetGLcontext(libbase),name);}
void APIENTRY smglPushName(A6(void* libbase),D0(GLuint name))	{aglPushName(AmigaGetGLcontext(libbase),name);}
void APIENTRY smglPopName(A6(void* libbase))	{aglPopName(AmigaGetGLcontext(libbase));}
void APIENTRY smglBlendEquationEXT(A6(void* libbase),D0(GLenum mode))	{aglBlendEquationEXT(AmigaGetGLcontext(libbase),mode);}
void APIENTRY smglBlendColorEXT(A6(void* libbase),FP0(GLclampf red),FP1(GLclampf green),FP2(GLclampf blue),FP3(GLclampf alpha))	{aglBlendColorEXT(AmigaGetGLcontext(libbase),red,green,blue,alpha);}
void APIENTRY smglPolygonOffsetEXT(A6(void* libbase),FP0(GLfloat factor),FP1(GLfloat bias))	{aglPolygonOffsetEXT(AmigaGetGLcontext(libbase),factor,bias);}
void APIENTRY smglVertexPointerEXT(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),D3(GLsizei count),A0(const GLvoid* ptr))	{aglVertexPointerEXT(AmigaGetGLcontext(libbase),size,type,stride,count,ptr);}
void APIENTRY smglNormalPointerEXT(A6(void* libbase),D0(GLenum type),D1(GLsizei stride),D2(GLsizei count),A0(const GLvoid* ptr))	{aglNormalPointerEXT(AmigaGetGLcontext(libbase),type,stride,count,ptr);}
void APIENTRY smglColorPointerEXT(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),D3(GLsizei count),A0(const GLvoid* ptr))	{aglColorPointerEXT(AmigaGetGLcontext(libbase),size,type,stride,count,ptr);}
void APIENTRY smglIndexPointerEXT(A6(void* libbase),D0(GLenum type),D1(GLsizei stride),D2(GLsizei count),A0(const GLvoid* ptr))	{aglIndexPointerEXT(AmigaGetGLcontext(libbase),type,stride,count,ptr);}
void APIENTRY smglTexCoordPointerEXT(A6(void* libbase),D0(GLint size),D1(GLenum type),D2(GLsizei stride),D3(GLsizei count),A0(const GLvoid* ptr))	{aglTexCoordPointerEXT(AmigaGetGLcontext(libbase),size,type,stride,count,ptr);}
void APIENTRY smglEdgeFlagPointerEXT(A6(void* libbase),D0(GLsizei stride),D1(GLsizei count),A0(const GLboolean* ptr))	{aglEdgeFlagPointerEXT(AmigaGetGLcontext(libbase),stride,count,ptr);}
void APIENTRY smglGetPointervEXT(A6(void* libbase),D0(GLenum pname),A0(void* *params))	{aglGetPointervEXT(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglArrayElementEXT(A6(void* libbase),D0(GLint i))	{aglArrayElementEXT(AmigaGetGLcontext(libbase),i);}
void APIENTRY smglDrawArraysEXT(A6(void* libbase),D0(GLenum mode),D1(GLint first),D2(GLsizei count))	{aglDrawArraysEXT(AmigaGetGLcontext(libbase),mode,first,count);}
void APIENTRY smglGenTexturesEXT(A6(void* libbase),D0(GLsizei n),A0(GLuint* textures))	{aglGenTexturesEXT(AmigaGetGLcontext(libbase),n,textures);}
void APIENTRY smglDeleteTexturesEXT(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures))	{aglDeleteTexturesEXT(AmigaGetGLcontext(libbase),n,textures);}
void APIENTRY smglBindTextureEXT(A6(void* libbase),D0(GLenum target),D1(GLuint texture))	{aglBindTextureEXT(AmigaGetGLcontext(libbase),target,texture);}
void APIENTRY smglPrioritizeTexturesEXT(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures),A1(const GLclampf* priorities))	{aglPrioritizeTexturesEXT(AmigaGetGLcontext(libbase),n,textures,priorities);}
void APIENTRY smglTexImage3DEXT(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum internalformat),D3(GLsizei width),D4(GLsizei height),D5(GLsizei depth),D6(GLint border),D7(GLenum format),A0(GLenum type),A1(const GLvoid *pixels))	{aglTexImage3DEXT(AmigaGetGLcontext(libbase),target,level,internalformat,width,height,depth,border,format,type,pixels);}
void APIENTRY smglTexSubImage3DEXT(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLint zoffset),D5(GLsizei width),D6(GLsizei height),D7(GLsizei depth),A0(GLenum format),A1(GLenum type),A2(const GLvoid *pixels))	{aglTexSubImage3DEXT(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);}
void APIENTRY smglCopyTexSubImage3DEXT(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLint zoffset),D5(GLint x),D6(GLint y),D7(GLsizei width),A0(GLsizei height))	{aglCopyTexSubImage3DEXT(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,zoffset,x,y,width,height);}
void APIENTRY smglColorTableEXT(A6(void* libbase),D0(GLenum target),D1(GLenum internalformat),D2(GLsizei width),D3(GLenum format),D4(GLenum type),A0(const GLvoid* table))	{aglColorTableEXT(AmigaGetGLcontext(libbase),target,internalformat,width,format,type,table);}
void APIENTRY smglColorSubTableEXT(A6(void* libbase),D0(GLenum target),D1(GLsizei start),D2(GLsizei count),D3(GLenum format),D4(GLenum type),A0(const GLvoid* data))	{aglColorSubTableEXT(AmigaGetGLcontext(libbase),target,start,count,format,type,data);}
void APIENTRY smglGetColorTableEXT(A6(void* libbase),D0(GLenum target),D1(GLenum format),D2(GLenum type),A0(GLvoid* table))	{aglGetColorTableEXT(AmigaGetGLcontext(libbase),target,format,type,table);}
void APIENTRY smglGetColorTableParameterfvEXT(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLfloat* params))	{aglGetColorTableParameterfvEXT(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglGetColorTableParameterivEXT(A6(void* libbase),D0(GLenum target),D1(GLenum pname),A0(GLint* params))	{aglGetColorTableParameterivEXT(AmigaGetGLcontext(libbase),target,pname,params);}
void APIENTRY smglMultiTexCoord1dSGIS(A6(void* libbase),D0(GLenum target),FP0(GLdouble s))	{aglMultiTexCoord1dSGIS(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1dvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord1dvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1fSGIS(A6(void* libbase),D0(GLenum target),FP0(GLfloat s))	{aglMultiTexCoord1fSGIS(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1fvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord1fvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1iSGIS(A6(void* libbase),D0(GLenum target),D1(GLint s))	{aglMultiTexCoord1iSGIS(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1ivSGIS(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord1ivSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1sSGIS(A6(void* libbase),D0(GLenum target),D1(GLshort s))	{aglMultiTexCoord1sSGIS(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1svSGIS(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord1svSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2dSGIS(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t))	{aglMultiTexCoord2dSGIS(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2dvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord2dvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2fSGIS(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t))	{aglMultiTexCoord2fSGIS(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2fvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord2fvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2iSGIS(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t))	{aglMultiTexCoord2iSGIS(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2ivSGIS(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord2ivSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2sSGIS(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t))	{aglMultiTexCoord2sSGIS(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2svSGIS(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord2svSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3dSGIS(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r))	{aglMultiTexCoord3dSGIS(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3dvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord3dvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3fSGIS(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r))	{aglMultiTexCoord3fSGIS(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3fvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord3fvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3iSGIS(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t),D3(GLint r))	{aglMultiTexCoord3iSGIS(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3ivSGIS(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord3ivSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3sSGIS(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t),D3(GLshort r))	{aglMultiTexCoord3sSGIS(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3svSGIS(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord3svSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4dSGIS(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r),FP3(GLdouble q))	{aglMultiTexCoord4dSGIS(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4dvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord4dvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4fSGIS(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r),FP3(GLfloat q))	{aglMultiTexCoord4fSGIS(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4fvSGIS(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord4fvSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4iSGIS(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t),D3(GLint r),D4(GLint q))	{aglMultiTexCoord4iSGIS(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4ivSGIS(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord4ivSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4sSGIS(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t),D3(GLshort r),D4(GLshort q))	{aglMultiTexCoord4sSGIS(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4svSGIS(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord4svSGIS(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoordPointerSGIS(A6(void* libbase),D0(GLenum target),D1(GLint size),D2(GLenum type),D3(GLsizei stride),A0(const GLvoid* pointer))	{aglMultiTexCoordPointerSGIS(AmigaGetGLcontext(libbase),target,size,type,stride,pointer);}
void APIENTRY smglSelectTextureSGIS(A6(void* libbase),D0(GLenum target))	{aglSelectTextureSGIS(AmigaGetGLcontext(libbase),target);}
void APIENTRY smglSelectTextureCoordSetSGIS(A6(void* libbase),D0(GLenum target))	{aglSelectTextureCoordSetSGIS(AmigaGetGLcontext(libbase),target);}
void APIENTRY smglMultiTexCoord1dEXT(A6(void* libbase),D0(GLenum target),FP0(GLdouble s))	{aglMultiTexCoord1dEXT(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1dvEXT(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord1dvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1fEXT(A6(void* libbase),D0(GLenum target),FP0(GLfloat s))	{aglMultiTexCoord1fEXT(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1fvEXT(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord1fvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1iEXT(A6(void* libbase),D0(GLenum target),D1(GLint s))	{aglMultiTexCoord1iEXT(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1ivEXT(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord1ivEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord1sEXT(A6(void* libbase),D0(GLenum target),D1(GLshort s))	{aglMultiTexCoord1sEXT(AmigaGetGLcontext(libbase),target,s);}
void APIENTRY smglMultiTexCoord1svEXT(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord1svEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2dEXT(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t))	{aglMultiTexCoord2dEXT(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2dvEXT(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord2dvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2fEXT(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t))	{aglMultiTexCoord2fEXT(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2fvEXT(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord2fvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2iEXT(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t))	{aglMultiTexCoord2iEXT(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2ivEXT(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord2ivEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord2sEXT(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t))	{aglMultiTexCoord2sEXT(AmigaGetGLcontext(libbase),target,s,t);}
void APIENTRY smglMultiTexCoord2svEXT(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord2svEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3dEXT(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r))	{aglMultiTexCoord3dEXT(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3dvEXT(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord3dvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3fEXT(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r))	{aglMultiTexCoord3fEXT(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3fvEXT(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord3fvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3iEXT(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t),D3(GLint r))	{aglMultiTexCoord3iEXT(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3ivEXT(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord3ivEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord3sEXT(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t),D3(GLshort r))	{aglMultiTexCoord3sEXT(AmigaGetGLcontext(libbase),target,s,t,r);}
void APIENTRY smglMultiTexCoord3svEXT(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord3svEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4dEXT(A6(void* libbase),D0(GLenum target),FP0(GLdouble s),FP1(GLdouble t),FP2(GLdouble r),FP3(GLdouble q))	{aglMultiTexCoord4dEXT(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4dvEXT(A6(void* libbase),D0(GLenum target),A0(const GLdouble* v))	{aglMultiTexCoord4dvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4fEXT(A6(void* libbase),D0(GLenum target),FP0(GLfloat s),FP1(GLfloat t),FP2(GLfloat r),FP3(GLfloat q))	{aglMultiTexCoord4fEXT(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4fvEXT(A6(void* libbase),D0(GLenum target),A0(const GLfloat* v))	{aglMultiTexCoord4fvEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4iEXT(A6(void* libbase),D0(GLenum target),D1(GLint s),D2(GLint t),D3(GLint r),D4(GLint q))	{aglMultiTexCoord4iEXT(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4ivEXT(A6(void* libbase),D0(GLenum target),A0(const GLint* v))	{aglMultiTexCoord4ivEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglMultiTexCoord4sEXT(A6(void* libbase),D0(GLenum target),D1(GLshort s),D2(GLshort t),D3(GLshort r),D4(GLshort q))	{aglMultiTexCoord4sEXT(AmigaGetGLcontext(libbase),target,s,t,r,q);}
void APIENTRY smglMultiTexCoord4svEXT(A6(void* libbase),D0(GLenum target),A0(const GLshort* v))	{aglMultiTexCoord4svEXT(AmigaGetGLcontext(libbase),target,v);}
void APIENTRY smglInterleavedTextureCoordSetsEXT(A6(void* libbase),D0(GLint factor))	{aglInterleavedTextureCoordSetsEXT(AmigaGetGLcontext(libbase),factor);}
void APIENTRY smglSelectTextureEXT(A6(void* libbase),D0(GLenum target))	{aglSelectTextureEXT(AmigaGetGLcontext(libbase),target);}
void APIENTRY smglSelectTextureCoordSetEXT(A6(void* libbase),D0(GLenum target))	{aglSelectTextureCoordSetEXT(AmigaGetGLcontext(libbase),target);}
void APIENTRY smglSelectTextureTransformEXT(A6(void* libbase),D0(GLenum target))	{aglSelectTextureTransformEXT(AmigaGetGLcontext(libbase),target);}
void APIENTRY smglPointParameterfEXT(A6(void* libbase),D0(GLenum pname),FP0(GLfloat param))	{aglPointParameterfEXT(AmigaGetGLcontext(libbase),pname,param);}
void APIENTRY smglPointParameterfvEXT(A6(void* libbase),D0(GLenum pname),A0(const GLfloat* params))	{aglPointParameterfvEXT(AmigaGetGLcontext(libbase),pname,params);}
void APIENTRY smglWindowPos2iMESA(A6(void* libbase),D0(GLint x),D1(GLint y))	{aglWindowPos2iMESA(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglWindowPos2sMESA(A6(void* libbase),D0(GLshort x),D1(GLshort y))	{aglWindowPos2sMESA(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglWindowPos2fMESA(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y))	{aglWindowPos2fMESA(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglWindowPos2dMESA(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y))	{aglWindowPos2dMESA(AmigaGetGLcontext(libbase),x,y);}
void APIENTRY smglWindowPos2ivMESA(A6(void* libbase),A0(const GLint* p))	{aglWindowPos2ivMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos2svMESA(A6(void* libbase),A0(const GLshort* p))	{aglWindowPos2svMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos2fvMESA(A6(void* libbase),A0(const GLfloat* p))	{aglWindowPos2fvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos2dvMESA(A6(void* libbase),A0(const GLdouble* p))	{aglWindowPos2dvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos3iMESA(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z))	{aglWindowPos3iMESA(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglWindowPos3sMESA(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z))	{aglWindowPos3sMESA(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglWindowPos3fMESA(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z))	{aglWindowPos3fMESA(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglWindowPos3dMESA(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z))	{aglWindowPos3dMESA(AmigaGetGLcontext(libbase),x,y,z);}
void APIENTRY smglWindowPos3ivMESA(A6(void* libbase),A0(const GLint* p))	{aglWindowPos3ivMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos3svMESA(A6(void* libbase),A0(const GLshort* p))	{aglWindowPos3svMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos3fvMESA(A6(void* libbase),A0(const GLfloat* p))	{aglWindowPos3fvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos3dvMESA(A6(void* libbase),A0(const GLdouble* p))	{aglWindowPos3dvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos4iMESA(A6(void* libbase),D0(GLint x),D1(GLint y),D2(GLint z),D3(GLint w))	{aglWindowPos4iMESA(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglWindowPos4sMESA(A6(void* libbase),D0(GLshort x),D1(GLshort y),D2(GLshort z),D3(GLshort w))	{aglWindowPos4sMESA(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglWindowPos4fMESA(A6(void* libbase),FP0(GLfloat x),FP1(GLfloat y),FP2(GLfloat z),FP3(GLfloat w))	{aglWindowPos4fMESA(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglWindowPos4dMESA(A6(void* libbase),FP0(GLdouble x),FP1(GLdouble y),FP2(GLdouble z),FP3(GLdouble w))	{aglWindowPos4dMESA(AmigaGetGLcontext(libbase),x,y,z,w);}
void APIENTRY smglWindowPos4ivMESA(A6(void* libbase),A0(const GLint* p))	{aglWindowPos4ivMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos4svMESA(A6(void* libbase),A0(const GLshort* p))	{aglWindowPos4svMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos4fvMESA(A6(void* libbase),A0(const GLfloat* p))	{aglWindowPos4fvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglWindowPos4dvMESA(A6(void* libbase),A0(const GLdouble* p))	{aglWindowPos4dvMESA(AmigaGetGLcontext(libbase),p);}
void APIENTRY smglResizeBuffersMESA(A6(void* libbase))	{aglResizeBuffersMESA(AmigaGetGLcontext(libbase));}
void APIENTRY smglDrawRangeElements(A6(void* libbase),D0(GLenum mode),D1(GLuint start),D2(GLuint end),D3(GLsizei count),D4(GLenum type),A0(const GLvoid* indices))	{aglDrawRangeElements(AmigaGetGLcontext(libbase),mode,start,end,count,type,indices);}
void APIENTRY smglTexImage3D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLenum internalFormat),D3(GLsizei width),D4(GLsizei height),D5(GLsizei depth),D6(GLint border),D7(GLenum format),A0(GLenum type),A1(const GLvoid *pixels))	{aglTexImage3D(AmigaGetGLcontext(libbase),target,level,internalFormat,width,height,depth,border,format,type,pixels);}
void APIENTRY smglTexSubImage3D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLint zoffset),D5(GLsizei width),D6(GLsizei height),D7(GLsizei depth),A0(GLenum format),A1(GLenum type),A2(const GLvoid *pixels))	{aglTexSubImage3D(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);}
void APIENTRY smglCopyTexSubImage3D(A6(void* libbase),D0(GLenum target),D1(GLint level),D2(GLint xoffset),D3(GLint yoffset),D4(GLint zoffset),D5(GLint x),D6(GLint y),D7(GLsizei width),A0(GLsizei height))	{aglCopyTexSubImage3D(AmigaGetGLcontext(libbase),target,level,xoffset,yoffset,zoffset,x,y,width,height);}
GLint APIENTRY smglRenderMode(A6(void* libbase),D0(GLenum mode))	{return(aglRenderMode(AmigaGetGLcontext(libbase),mode));}
GLenum APIENTRY smglGetError(A6(void* libbase))	{return(aglGetError(AmigaGetGLcontext(libbase)));}
GLuint APIENTRY smglGenLists(A6(void* libbase),D0(GLsizei range))	{return(aglGenLists(AmigaGetGLcontext(libbase),range));}
GLboolean APIENTRY smglIsEnabled(A6(void* libbase),D0(GLenum cap))	{return(aglIsEnabled(AmigaGetGLcontext(libbase),cap));}
GLboolean APIENTRY smglIsList(A6(void* libbase),D0(GLuint list))	{return(aglIsList(AmigaGetGLcontext(libbase),list));}
GLboolean APIENTRY smglAreTexturesResident(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures),A1(GLboolean* residences))	{return(aglAreTexturesResident(AmigaGetGLcontext(libbase),n,textures,residences));}
GLboolean APIENTRY smglIsTexture(A6(void* libbase),D0(GLuint texture))	{return(aglIsTexture(AmigaGetGLcontext(libbase),texture));}
GLboolean APIENTRY smglAreTexturesResidentEXT(A6(void* libbase),D0(GLsizei n),A0(const GLuint* textures),A1(GLboolean* residences))	{return(aglAreTexturesResidentEXT(AmigaGetGLcontext(libbase),n,textures,residences));}
GLboolean APIENTRY smglIsTextureEXT(A6(void* libbase),D0(GLuint texture))	{return(FALSE);}
const GLubyte* APIENTRY smglGetString(A6(void* libbase),D0(GLenum name))	{return(aglGetString(AmigaGetGLcontext(libbase),name));}
