/* $Id: api1.c,v 3.4 1998/03/27 03:30:36 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log: api1.c,v $
 * Revision 3.4  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.3  1998/02/20 04:49:19  brianp
 * move extension functions into apiext.c
 *
 * Revision 3.2  1998/02/08 20:23:49  brianp
 * lots of bitmap rendering changes
 *
 * Revision 3.1  1998/02/01 20:05:10  brianp
 * added aglDrawRangeElements()
 *
 * Revision 3.0  1998/01/31 20:42:56  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include "api.h"
#include "bitmap.h"
#include "context.h"
#include "drawpix.h"
#include "eval.h"
#include "image.h"
#include "macros.h"
#include "matrix.h"
#include "teximage.h"
#include "types.h"
#include "vb.h"
#endif


/*
 * Part 1 of API functions
 */

#define STORMMESADEBUG 1
/*==================================================================================*/
#ifdef STORMMESADEBUG
void LibPrintf(const char *string, ...);
void STORMMESA_Function(const char *func);
#define VAR(var)	 /*LibPrintf*/ printf(#var ": %d \n",var);
#define SFUNCTION(func) STORMMESA_Function(#func);
/*==================================================================================*/
#else
#define VAR(var) ;
#define SFUNCTION(func) ;
#endif
/*==================================================================================*/


void APIENTRY aglAccum(void *cc,GLenum op, GLfloat value )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Accum)	(*CC->API.Accum)(CC, op, value);
}


void APIENTRY aglAlphaFunc(void *cc,GLenum func, GLclampf ref )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(AlphaFunc)	(*CC->API.AlphaFunc)(CC, func, ref);
}


GLboolean APIENTRY aglAreTexturesResident(void *cc,GLsizei n, const GLuint *textures,
											GLboolean *residences )
{
	GET_CONTEXT; CHECK_CONTEXT_RETURN(GL_FALSE);
	SFUNCTION(AreTexturesResident)	return (*CC->API.AreTexturesResident)(CC, n, textures, residences);
}


void APIENTRY aglArrayElement(void *cc,GLint i )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ArrayElement)	(*CC->API.ArrayElement)(CC, i);
}


void APIENTRY aglBegin(void *cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Begin)	(*CC->API.Begin)(CC, mode );
}


void APIENTRY aglBindTexture(void *cc,GLenum target, GLuint texture )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(BindTexture)	(*CC->API.BindTexture)(CC, target, texture);
}


void APIENTRY aglBitmap(void *cc,GLsizei width, GLsizei height,
								GLfloat xorig, GLfloat yorig,
								GLfloat xmove, GLfloat ymove,
								const GLubyte *bitmap )
{
	GET_CONTEXT; CHECK_CONTEXT;

	if (!CC->DirectContext || CC->CompileFlag
		 || !gl_direct_bitmap(CC, width, height, xorig, yorig,
									  xmove, ymove, bitmap)) {
		struct gl_image *image;
		image = gl_unpack_bitmap(CC, width, height, bitmap );
	SFUNCTION(Bitmap)	(*CC->API.Bitmap)(CC, width, height, xorig, yorig,
								 xmove, ymove, image );
		if (image && image->RefCount==0) {
			gl_free_image( image );
		}
	}
}


void APIENTRY aglBlendFunc(void *cc,GLenum sfactor, GLenum dfactor )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(BlendFunc)	(*CC->API.BlendFunc)(CC, sfactor, dfactor);
}


void APIENTRY aglCallList(void *cc,GLuint list )
{
	GET_CONTEXT; CHECK_CONTEXT;
	/*LibPrintf*/ printf("aglCallList(cc:%d,list:%d\n",cc,list);
	SFUNCTION(CallList)	(*CC->API.CallList)(CC, list);
}


void APIENTRY aglCallLists(void *cc,GLsizei n, GLenum type, const GLvoid *lists )
{
	GET_CONTEXT; CHECK_CONTEXT;
	/*LibPrintf*/ printf("aglCallLists(cc:%d,n:%d,type:%d,lists:%d\n",cc,n,type,lists);
	SFUNCTION(CallLists)
	(*CC->API.CallLists)(CC, n, type, lists);
}


void APIENTRY aglClear(void *cc,GLbitfield mask )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Clear)	(*CC->API.Clear)(CC, mask);
}


void APIENTRY aglClearAccum(void *cc,GLfloat red, GLfloat green,
				  GLfloat blue, GLfloat alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ClearAccum)	(*CC->API.ClearAccum)(CC, red, green, blue, alpha);
}



void APIENTRY aglClearIndex(void *cc,GLfloat c )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ClearIndex)	(*CC->API.ClearIndex)(CC, c);
}


void APIENTRY aglClearColor(void *cc,GLclampf red,
				  GLclampf green,
				  GLclampf blue,
				  GLclampf alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ClearColor)	(*CC->API.ClearColor)(CC, red, green, blue, alpha);
}


void APIENTRY aglClearDepth(void *cc,GLclampd depth )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ClearDepth)	(*CC->API.ClearDepth)(CC, depth );
}


void APIENTRY aglClearStencil(void *cc,GLint s )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ClearStencil)	(*CC->API.ClearStencil)(CC, s);
}


void APIENTRY aglClipPlane(void *cc,GLenum plane, const GLdouble *equation )
{
	GLfloat eq[4];
	GET_CONTEXT; CHECK_CONTEXT;

	eq[0] = (GLfloat) equation[0];
	eq[1] = (GLfloat) equation[1];
	eq[2] = (GLfloat) equation[2];
	eq[3] = (GLfloat) equation[3];
	SFUNCTION(ClipPlane)	(*CC->API.ClipPlane)(CC, plane, eq );
}


void APIENTRY aglColor3b(void *cc,GLbyte red, GLbyte green, GLbyte blue )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, BYTE_TO_FLOAT(red), BYTE_TO_FLOAT(green),
							  BYTE_TO_FLOAT(blue) );
}


void APIENTRY aglColor3d(void *cc,GLdouble red, GLdouble green, GLdouble blue )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, (GLfloat) red, (GLfloat) green, (GLfloat) blue );
}


void APIENTRY aglColor3f(void *cc,GLfloat red, GLfloat green, GLfloat blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, red, green, blue );
}


void APIENTRY aglColor3i(void *cc,GLint red, GLint green, GLint blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, INT_TO_FLOAT(red), INT_TO_FLOAT(green),
							  INT_TO_FLOAT(blue) );
}


void APIENTRY aglColor3s(void *cc,GLshort red, GLshort green, GLshort blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, SHORT_TO_FLOAT(red), SHORT_TO_FLOAT(green),
							  SHORT_TO_FLOAT(blue) );
}


void APIENTRY aglColor3ub(void *cc,GLubyte red, GLubyte green, GLubyte blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4ub)	(*CC->API.Color4ub)(CC, red, green, blue, 255 );
}


void APIENTRY aglColor3ui(void *cc,GLuint red, GLuint green, GLuint blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, UINT_TO_FLOAT(red), UINT_TO_FLOAT(green),
							  UINT_TO_FLOAT(blue) );
}


void APIENTRY aglColor3us(void *cc,GLushort red, GLushort green, GLushort blue )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, USHORT_TO_FLOAT(red), USHORT_TO_FLOAT(green),
							  USHORT_TO_FLOAT(blue) );
}


void APIENTRY aglColor4b(void *cc,GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, BYTE_TO_FLOAT(red), BYTE_TO_FLOAT(green),
							  BYTE_TO_FLOAT(blue), BYTE_TO_FLOAT(alpha) );
}


void APIENTRY aglColor4d(void *cc,GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, (GLfloat) red, (GLfloat) green,
							  (GLfloat) blue, (GLfloat) alpha );
}


void APIENTRY aglColor4f(void *cc,GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, red, green, blue, alpha );
}

void APIENTRY aglColor4i(void *cc,GLint red, GLint green, GLint blue, GLint alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, INT_TO_FLOAT(red), INT_TO_FLOAT(green),
							  INT_TO_FLOAT(blue), INT_TO_FLOAT(alpha) );
}


void APIENTRY aglColor4s(void *cc,GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, SHORT_TO_FLOAT(red), SHORT_TO_FLOAT(green),
							  SHORT_TO_FLOAT(blue), SHORT_TO_FLOAT(alpha) );
}

void APIENTRY aglColor4ub(void *cc,GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4ub)	(*CC->API.Color4ub)(CC, red, green, blue, alpha );
}

void APIENTRY aglColor4ui(void *cc,GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, UINT_TO_FLOAT(red), UINT_TO_FLOAT(green),
							  UINT_TO_FLOAT(blue), UINT_TO_FLOAT(alpha) );
}

void APIENTRY aglColor4us(void *cc,GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, USHORT_TO_FLOAT(red), USHORT_TO_FLOAT(green),
							  USHORT_TO_FLOAT(blue), USHORT_TO_FLOAT(alpha) );
}


void APIENTRY aglColor3bv(void *cc,const GLbyte *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, BYTE_TO_FLOAT(v[0]), BYTE_TO_FLOAT(v[1]),
							  BYTE_TO_FLOAT(v[2]) );
}


void APIENTRY aglColor3dv(void *cc,const GLdouble *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, (GLdouble) v[0], (GLdouble) v[1], (GLdouble) v[2] );
}


void APIENTRY aglColor3fv(void *cc,const GLfloat *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3fv)	(*CC->API.Color3fv)(CC, v );
}


void APIENTRY aglColor3iv(void *cc,const GLint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, INT_TO_FLOAT(v[0]), INT_TO_FLOAT(v[1]),
							  INT_TO_FLOAT(v[2]) );
}


void APIENTRY aglColor3sv(void *cc,const GLshort *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, SHORT_TO_FLOAT(v[0]), SHORT_TO_FLOAT(v[1]),
							  SHORT_TO_FLOAT(v[2]) );
}


void APIENTRY aglColor3ubv(void *cc,const GLubyte *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4ub)	(*CC->API.Color4ub)(CC, v[0], v[1], v[2], 255 );
}


void APIENTRY aglColor3uiv(void *cc,const GLuint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, UINT_TO_FLOAT(v[0]), UINT_TO_FLOAT(v[1]),
							  UINT_TO_FLOAT(v[2]) );
}


void APIENTRY aglColor3usv(void *cc,const GLushort *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color3f)	(*CC->API.Color3f)(CC, USHORT_TO_FLOAT(v[0]), USHORT_TO_FLOAT(v[1]),
							  USHORT_TO_FLOAT(v[2]) );

}


void APIENTRY aglColor4bv(void *cc,const GLbyte *v )
{

	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, BYTE_TO_FLOAT(v[0]), BYTE_TO_FLOAT(v[1]),
							  BYTE_TO_FLOAT(v[2]), BYTE_TO_FLOAT(v[3]) );
}


void APIENTRY aglColor4dv(void *cc,const GLdouble *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, (GLdouble) v[0], (GLdouble) v[1],
							  (GLdouble) v[2], (GLdouble) v[3] );
}


void APIENTRY aglColor4fv(void *cc,const GLfloat *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, v[0], v[1], v[2], v[3] );
}


void APIENTRY aglColor4iv(void *cc,const GLint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, INT_TO_FLOAT(v[0]), INT_TO_FLOAT(v[1]),
							  INT_TO_FLOAT(v[2]), INT_TO_FLOAT(v[3]) );
}


void APIENTRY aglColor4sv(void *cc,const GLshort *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, SHORT_TO_FLOAT(v[0]), SHORT_TO_FLOAT(v[1]),
							  SHORT_TO_FLOAT(v[2]), SHORT_TO_FLOAT(v[3]) );
}


void APIENTRY aglColor4ubv(void *cc,const GLubyte *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4ubv)
	(*CC->API.Color4ubv)(CC, v );
}


void APIENTRY aglColor4uiv(void *cc,const GLuint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, UINT_TO_FLOAT(v[0]), UINT_TO_FLOAT(v[1]),
							  UINT_TO_FLOAT(v[2]), UINT_TO_FLOAT(v[3]) );
}


void APIENTRY aglColor4usv(void *cc,const GLushort *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Color4f)	(*CC->API.Color4f)(CC, USHORT_TO_FLOAT(v[0]), USHORT_TO_FLOAT(v[1]),
							  USHORT_TO_FLOAT(v[2]), USHORT_TO_FLOAT(v[3]) );
}


void APIENTRY aglColorMask(void *cc,GLboolean red, GLboolean green,
				 GLboolean blue, GLboolean alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ColorMask)	(*CC->API.ColorMask)(CC, red, green, blue, alpha);
}


void APIENTRY aglColorMaterial(void *cc,GLenum face, GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ColorMaterial)	(*CC->API.ColorMaterial)(CC, face, mode);
}


void APIENTRY aglColorPointer(void *cc,GLint size, GLenum type, GLsizei stride,
							const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(ColorPointer)	(*CC->API.ColorPointer)(CC, size, type, stride, ptr);
}


void APIENTRY aglColorTableEXT(void *cc,GLenum target, GLenum internalFormat,
										 GLsizei width, GLenum format, GLenum type,
										 const GLvoid *table )
{
	struct gl_image *image;
	GET_CONTEXT; CHECK_CONTEXT;

	image = gl_unpack_image(CC, width, 1, format, type, table );
	SFUNCTION(ColorTable)	(*CC->API.ColorTable)(CC, target, internalFormat, image );
	if (image->RefCount == 0)
		gl_free_image(image);
}


void APIENTRY aglColorSubTableEXT(void *cc,GLenum target, GLsizei start, GLsizei count,
											 GLenum format, GLenum type,
											 const GLvoid *data )
{
	struct gl_image *image;
	GET_CONTEXT; CHECK_CONTEXT;

	image = gl_unpack_image(CC, count, 1, format, type, data );
	SFUNCTION(ColorSubTable)	(*CC->API.ColorSubTable)(CC, target, start, image );
	if (image->RefCount == 0)
		gl_free_image(image);
}



void APIENTRY aglCopyPixels(void *cc,GLint x, GLint y, GLsizei width, GLsizei height,
				  GLenum type )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyPixels)	(*CC->API.CopyPixels)(CC, x, y, width, height, type);
}


void APIENTRY aglCopyTexImage1D(void *cc,GLenum target, GLint level,
										  GLenum internalformat,
										  GLint x, GLint y,
										  GLsizei width, GLint border )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyTexImage1D)	(*CC->API.CopyTexImage1D)(CC, target, level, internalformat,
											x, y, width, border );
}


void APIENTRY aglCopyTexImage2D(void *cc,GLenum target, GLint level,
										  GLenum internalformat,
										  GLint x, GLint y,
										  GLsizei width, GLsizei height, GLint border )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyTexImage2D)	(*CC->API.CopyTexImage2D)(CC, target, level, internalformat,
										x, y, width, height, border );
}


void APIENTRY aglCopyTexSubImage1D(void *cc,GLenum target, GLint level,
											  GLint xoffset, GLint x, GLint y,
											  GLsizei width )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyTexSubImage1D)	(*CC->API.CopyTexSubImage1D)(CC, target, level, xoffset, x, y, width );
}


void APIENTRY aglCopyTexSubImage2D(void *cc,GLenum target, GLint level,
											  GLint xoffset, GLint yoffset,
											  GLint x, GLint y,
											  GLsizei width, GLsizei height )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyTexSubImage2D)	(*CC->API.CopyTexSubImage2D)(CC, target, level, xoffset, yoffset,
											x, y, width, height );
}


/* 1.2 */
void APIENTRY aglCopyTexSubImage3D(void *cc,GLenum target, GLint level, GLint xoffset,
											  GLint yoffset, GLint zoffset,
											  GLint x, GLint y, GLsizei width,
											  GLsizei height )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CopyTexSubImage3DEXT)	(*CC->API.CopyTexSubImage3DEXT)(CC, target, level, xoffset, yoffset,
												zoffset, x, y, width, height );
}



void APIENTRY aglCullFace(void *cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(CullFace)	(*CC->API.CullFace)(CC, mode);
}


void APIENTRY aglDepthFunc(void *cc,GLenum func )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(DepthFunc)	(*CC->API.DepthFunc)(CC, func );

}


void APIENTRY aglDepthMask(void *cc,GLboolean flag )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(DepthMask)	(*CC->API.DepthMask)(CC, flag );
}


void APIENTRY aglDepthRange(void *cc,GLclampd near_val, GLclampd far_val )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DepthRange)	(*CC->API.DepthRange)(CC, near_val, far_val );
}


void APIENTRY aglDeleteLists(void *cc,GLuint list, GLsizei range )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DeleteLists)	(*CC->API.DeleteLists)(CC, list, range);
}


void APIENTRY aglDeleteTextures(void *cc,GLsizei n, const GLuint *textures)
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DeleteTextures)	(*CC->API.DeleteTextures)(CC, n, textures);
}


void APIENTRY aglDisable(void *cc,GLenum cap )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Disable)	(*CC->API.Disable)(CC, cap );
}


void APIENTRY aglDisableClientState(void *cc,GLenum cap )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DisableClientState)	(*CC->API.DisableClientState)(CC, cap );
}


void APIENTRY aglDrawArrays(void *cc,GLenum mode, GLint first, GLsizei count )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DrawArrays)	(*CC->API.DrawArrays)(CC, mode, first, count);
}


void APIENTRY aglDrawBuffer(void *cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DrawBuffer)	(*CC->API.DrawBuffer)(CC, mode);
}


void APIENTRY aglDrawElements(void *cc,GLenum mode, GLsizei count,
										GLenum type, const GLvoid *indices )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DrawElements)	(*CC->API.DrawElements)(CC, mode, count, type, indices );
}


void APIENTRY aglDrawPixels(void *cc,GLsizei width, GLsizei height,
									 GLenum format, GLenum type, const GLvoid *pixels )
{
	GET_CONTEXT; CHECK_CONTEXT;

	if (!CC->DirectContext || CC->CompileFlag
		 || !gl_direct_DrawPixels(CC, &CC->Unpack, width, height,
										  format, type, pixels)) {
		struct gl_image *image;
		image = gl_unpack_image(CC, width, height, format, type, pixels );
		(*CC->API.DrawPixels)(CC, image );
		if (image->RefCount==0) {
			/* image not in display list */
			gl_free_image( image );
		}
	}
}


/* GL_VERSION_1_2 */
void APIENTRY aglDrawRangeElements(void *cc,GLenum mode, GLuint start, GLuint end,
								 GLsizei count, GLenum type, const GLvoid *indices )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(DrawRangeElements)	(*CC->API.DrawRangeElements)(CC, mode, start, end, count, type, indices );
}


void APIENTRY aglEnable(void *cc,GLenum cap )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Enable)	(*CC->API.Enable)(CC, cap );
}


void APIENTRY aglEnableClientState(void *cc,GLenum cap )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(EnableClientState)	(*CC->API.EnableClientState)(CC, cap );
}


void APIENTRY aglEnd(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(End)	(*CC->API.End)(CC );
}


void APIENTRY aglEndList(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EndList)	(*CC->API.EndList)(CC);
}




void APIENTRY aglEvalCoord1d(void *cc,GLdouble u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord1f)	(*CC->API.EvalCoord1f)(CC, (GLfloat) u );
}


void APIENTRY aglEvalCoord1f(void *cc,GLfloat u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord1f)	(*CC->API.EvalCoord1f)(CC, u );
}


void APIENTRY aglEvalCoord1dv(void *cc,const GLdouble *u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord1f)	(*CC->API.EvalCoord1f)(CC, (GLfloat) *u );
}


void APIENTRY aglEvalCoord1fv(void *cc,const GLfloat *u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord1f)	(*CC->API.EvalCoord1f)(CC, (GLfloat) *u );
}


void APIENTRY aglEvalCoord2d(void *cc,GLdouble u, GLdouble v )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord2f)	(*CC->API.EvalCoord2f)(CC, (GLfloat) u, (GLfloat) v );
}


void APIENTRY aglEvalCoord2f(void *cc,GLfloat u, GLfloat v )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord2f)	(*CC->API.EvalCoord2f)(CC, u, v );
}


void APIENTRY aglEvalCoord2dv(void *cc,const GLdouble *u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord2f)	(*CC->API.EvalCoord2f)(CC, (GLfloat) u[0], (GLfloat) u[1] );
}


void APIENTRY aglEvalCoord2fv(void *cc,const GLfloat *u )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalCoord2f)	(*CC->API.EvalCoord2f)(CC, u[0], u[1] );
}


void APIENTRY aglEvalPoint1(void *cc,GLint i )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalPoint1)	(*CC->API.EvalPoint1)(CC, i );
}


void APIENTRY aglEvalPoint2(void *cc,GLint i, GLint j )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalPoint2)	(*CC->API.EvalPoint2)(CC, i, j );
}


void APIENTRY aglEvalMesh1(void *cc,GLenum mode, GLint i1, GLint i2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalMesh1)	(*CC->API.EvalMesh1)(CC, mode, i1, i2 );
}


void APIENTRY aglEdgeFlag(void *cc,GLboolean flag )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EdgeFlag)	(*CC->API.EdgeFlag)(CC, flag);
}


void APIENTRY aglEdgeFlagv(void *cc,const GLboolean *flag )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EdgeFlag)	(*CC->API.EdgeFlag)(CC, *flag);
}


void APIENTRY aglEdgeFlagPointer(void *cc,GLsizei stride, const GLboolean *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EdgeFlagPointer)	(*CC->API.EdgeFlagPointer)(CC, stride, ptr);
}


void APIENTRY aglEvalMesh2(void *cc,GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(EvalMesh2)	(*CC->API.EvalMesh2)(CC, mode, i1, i2, j1, j2 );
}


void APIENTRY aglFeedbackBuffer(void *cc,GLsizei size, GLenum type, GLfloat *buffer )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(FeedbackBuffer)	(*CC->API.FeedbackBuffer)(CC, size, type, buffer);
}


void APIENTRY aglFinish(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Finish)	(*CC->API.Finish)(CC);
}


void APIENTRY aglFlush(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Flush)	(*CC->API.Flush)(CC);
}


void APIENTRY aglFogf(void *cc,GLenum pname, GLfloat param )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Fogfv)	(*CC->API.Fogfv)(CC, pname, &param);
}


void APIENTRY aglFogi(void *cc,GLenum pname, GLint param )
{
	GLfloat fparam = (GLfloat) param;
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Fogfv)	(*CC->API.Fogfv)(CC, pname, &fparam);
}


void APIENTRY aglFogfv(void *cc,GLenum pname, const GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Fogfv)	(*CC->API.Fogfv)(CC, pname, params);
}


void APIENTRY aglFogiv(void *cc,GLenum pname, const GLint *params )
{
	GLfloat p[4];
	GET_CONTEXT; CHECK_CONTEXT;


	switch (pname) {
		case GL_FOG_MODE:
		case GL_FOG_DENSITY:
		case GL_FOG_START:
		case GL_FOG_END:
		case GL_FOG_INDEX:
	  p[0] = (GLfloat) *params;
	  break;
		case GL_FOG_COLOR:
	  p[0] = INT_TO_FLOAT( params[0] );
	  p[1] = INT_TO_FLOAT( params[1] );
	  p[2] = INT_TO_FLOAT( params[2] );
	  p[3] = INT_TO_FLOAT( params[3] );
	  break;
		default:
			/* Error will be caught later in gl_Fogfv */
			;
	}
	SFUNCTION(Fogfv)	(*CC->API.Fogfv)(CC, pname, p );
}



void APIENTRY aglFrontFace(void *cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(FrontFace)	(*CC->API.FrontFace)(CC, mode);
}


void APIENTRY aglFrustum(void *cc,GLdouble left, GLdouble right,
								 GLdouble bottom, GLdouble top,
								 GLdouble nearval, GLdouble farval )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Frustum)	(*CC->API.Frustum)(CC, left, right, bottom, top, nearval, farval);
}


GLuint APIENTRY aglGenLists(void *cc,GLsizei range )
{
	GET_CONTEXT; CHECK_CONTEXT_RETURN(0);
	SFUNCTION(GenLists)	return (*CC->API.GenLists)(CC, range);
}


void APIENTRY aglGenTextures(void *cc,GLsizei n, GLuint *textures )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GenTextures)	(*CC->API.GenTextures)(CC, n, textures);
}


void APIENTRY aglGetBooleanv(void *cc,GLenum pname, GLboolean *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetBooleanv)	(*CC->API.GetBooleanv)(CC, pname, params);
}


void APIENTRY aglGetClipPlane(void *cc,GLenum plane, GLdouble *equation )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetClipPlane)	(*CC->API.GetClipPlane)(CC, plane, equation);
}


void APIENTRY aglGetColorTableEXT(void *cc,GLenum target, GLenum format,
											 GLenum type, GLvoid *table )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetColorTable)	(*CC->API.GetColorTable)(CC, target, format, type, table);
}


void APIENTRY aglGetColorTableParameterivEXT(void *cc,GLenum target, GLenum pname,
															GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetColorTableParameteriv)	(*CC->API.GetColorTableParameteriv)(CC, target, pname, params);
}


void APIENTRY aglGetColorTableParameterfvEXT(void *cc,GLenum target, GLenum pname,
															GLfloat *params )
{
	GLint iparams;
	aglGetColorTableParameterivEXT(cc,target, pname, &iparams );
	*params = (GLfloat) iparams;
}


void APIENTRY aglGetDoublev(void *cc,GLenum pname, GLdouble *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetDoublev)	(*CC->API.GetDoublev)(CC, pname, params);
}


GLenum APIENTRY aglGetError(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;
	if (!CC) {
		/* No current context */
		return (GLenum) GL_NO_ERROR;
	}
	SFUNCTION(GetError)	return (*CC->API.GetError)(CC);
}


void APIENTRY aglGetFloatv(void *cc,GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetFloatv)	(*CC->API.GetFloatv)(CC, pname, params);
}


void APIENTRY aglGetIntegerv(void *cc,GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetIntegerv)	(*CC->API.GetIntegerv)(CC, pname, params);
}


void APIENTRY aglGetLightfv(void *cc,GLenum light, GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetLightfv)	(*CC->API.GetLightfv)(CC, light, pname, params);
}


void APIENTRY aglGetLightiv(void *cc,GLenum light, GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetLightiv)	(*CC->API.GetLightiv)(CC, light, pname, params);
}


void APIENTRY aglGetMapdv(void *cc,GLenum target, GLenum query, GLdouble *v )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetMapdv)	(*CC->API.GetMapdv)(CC, target, query, v );
}


void APIENTRY aglGetMapfv(void *cc,GLenum target, GLenum query, GLfloat *v )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetMapfv)	(*CC->API.GetMapfv)(CC, target, query, v );
}


void APIENTRY aglGetMapiv(void *cc,GLenum target, GLenum query, GLint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetMapiv)	(*CC->API.GetMapiv)(CC, target, query, v );
}


void APIENTRY aglGetMaterialfv(void *cc,GLenum face, GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetMaterialfv)	(*CC->API.GetMaterialfv)(CC, face, pname, params);
}


void APIENTRY aglGetMaterialiv(void *cc,GLenum face, GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetMaterialiv)	(*CC->API.GetMaterialiv)(CC, face, pname, params);
}


void APIENTRY aglGetPixelMapfv(void *cc,GLenum map, GLfloat *values )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetPixelMapfv)	(*CC->API.GetPixelMapfv)(CC, map, values);
}


void APIENTRY aglGetPixelMapuiv(void *cc,GLenum map, GLuint *values )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetPixelMapuiv)	(*CC->API.GetPixelMapuiv)(CC, map, values);
}


void APIENTRY aglGetPixelMapusv(void *cc,GLenum map, GLushort *values )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetPixelMapusv)	(*CC->API.GetPixelMapusv)(CC, map, values);
}


void APIENTRY aglGetPointerv(void *cc,GLenum pname, GLvoid **params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetPointerv)	(*CC->API.GetPointerv)(CC, pname, params);
}


void APIENTRY aglGetPolygonStipple(void *cc,GLubyte *mask )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetPolygonStipple)	(*CC->API.GetPolygonStipple)(CC, mask);
}


const GLubyte * APIENTRY aglGetString(void *cc,GLenum name )
{
	GET_CONTEXT; CHECK_CONTEXT_RETURN(NULL);
	SFUNCTION(GetString)	return (*CC->API.GetString)(CC, name);
}



void APIENTRY aglGetTexEnvfv(void *cc,GLenum target, GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexEnvfv)	(*CC->API.GetTexEnvfv)(CC, target, pname, params);
}


void APIENTRY aglGetTexEnviv(void *cc,GLenum target, GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexEnviv)	(*CC->API.GetTexEnviv)(CC, target, pname, params);
}


void APIENTRY aglGetTexGeniv(void *cc,GLenum coord, GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexGeniv)	(*CC->API.GetTexGeniv)(CC, coord, pname, params);
}


void APIENTRY aglGetTexGendv(void *cc,GLenum coord, GLenum pname, GLdouble *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexGendv)	(*CC->API.GetTexGendv)(CC, coord, pname, params);
}


void APIENTRY aglGetTexGenfv(void *cc,GLenum coord, GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexGenfv)	(*CC->API.GetTexGenfv)(CC, coord, pname, params);
}



void APIENTRY aglGetTexImage(void *cc,GLenum target, GLint level, GLenum format,
									  GLenum type, GLvoid *pixels )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexImage)	(*CC->API.GetTexImage)(CC, target, level, format, type, pixels);
}


void APIENTRY aglGetTexLevelParameterfv(void *cc,GLenum target, GLint level,
													 GLenum pname, GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexLevelParameterfv)	(*CC->API.GetTexLevelParameterfv)(CC, target, level, pname, params);
}


void APIENTRY aglGetTexLevelParameteriv(void *cc,GLenum target, GLint level,
													 GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexLevelParameteriv)	(*CC->API.GetTexLevelParameteriv)(CC, target, level, pname, params);
}




void APIENTRY aglGetTexParameterfv(void *cc,GLenum target, GLenum pname, GLfloat *params)
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexParameterfv)	(*CC->API.GetTexParameterfv)(CC, target, pname, params);
}


void APIENTRY aglGetTexParameteriv(void *cc,GLenum target, GLenum pname, GLint *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(GetTexParameteriv)	(*CC->API.GetTexParameteriv)(CC, target, pname, params);
}


void APIENTRY aglHint(void *cc,GLenum target, GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Hint)	(*CC->API.Hint)(CC, target, mode);
}


void APIENTRY aglIndexd(void *cc,GLdouble c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexf)	(*CC->API.Indexf)(CC, (GLfloat) c );
}


void APIENTRY aglIndexf(void *cc,GLfloat c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexf)	(*CC->API.Indexf)(CC, c );
}


void APIENTRY aglIndexi(void *cc,GLint c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, c );
}


void APIENTRY aglIndexs(void *cc,GLshort c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, (GLint) c );
}


/* GL_VERSION_1_1 */
void APIENTRY aglIndexub(void *cc,GLubyte c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, (GLint) c );
}


void APIENTRY aglIndexdv(void *cc,const GLdouble *c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexf)	(*CC->API.Indexf)(CC, (GLfloat) *c );
}


void APIENTRY aglIndexfv(void *cc,const GLfloat *c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexf)	(*CC->API.Indexf)(CC, *c );
}


void APIENTRY aglIndexiv(void *cc,const GLint *c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, *c );
}


void APIENTRY aglIndexsv(void *cc,const GLshort *c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, (GLint) *c );
}


/* GL_VERSION_1_1 */
void APIENTRY aglIndexubv(void *cc,const GLubyte *c )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Indexi)	(*CC->API.Indexi)(CC, (GLint) *c );
}


void APIENTRY aglIndexMask(void *cc,GLuint mask )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(IndexMask)	(*CC->API.IndexMask)(CC, mask);
}


void APIENTRY aglIndexPointer(void *cc,GLenum type, GLsizei stride, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(IndexPointer)	(*CC->API.IndexPointer)(CC, type, stride, ptr);
}


void APIENTRY aglInterleavedArrays(void *cc,GLenum format, GLsizei stride,
											  const GLvoid *pointer )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(InterleavedArrays)	(*CC->API.InterleavedArrays)(CC, format, stride, pointer );
}


void APIENTRY aglInitNames(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(InitNames)	(*CC->API.InitNames)(CC);
}


GLboolean APIENTRY aglIsList(void *cc,GLuint list )
{
	GET_CONTEXT; CHECK_CONTEXT_RETURN(GL_FALSE);
	SFUNCTION(IsList)	return (*CC->API.IsList)(CC, list);
}


GLboolean APIENTRY aglIsTexture(void *cc,GLuint texture )
{
	GET_CONTEXT; CHECK_CONTEXT_RETURN(GL_FALSE);
	SFUNCTION(IsTexture)	return (*CC->API.IsTexture)(CC, texture);
}


void APIENTRY aglLightf(void *cc,GLenum light, GLenum pname, GLfloat param )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Lightfv)	(*CC->API.Lightfv)(CC, light, pname, &param, 1 );
}



void APIENTRY aglLighti(void *cc,GLenum light, GLenum pname, GLint param )
{
	GLfloat fparam = (GLfloat) param;
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Lightfv)	(*CC->API.Lightfv)(CC, light, pname, &fparam, 1 );
}



void APIENTRY aglLightfv(void *cc,GLenum light, GLenum pname, const GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Lightfv)	(*CC->API.Lightfv)(CC, light, pname, params, 4 );
}



void APIENTRY aglLightiv(void *cc,GLenum light, GLenum pname, const GLint *params )
{
	GLfloat fparam[4];
	GET_CONTEXT; CHECK_CONTEXT;


	switch (pname) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
			fparam[0] = INT_TO_FLOAT( params[0] );
			fparam[1] = INT_TO_FLOAT( params[1] );
			fparam[2] = INT_TO_FLOAT( params[2] );
			fparam[3] = INT_TO_FLOAT( params[3] );
			break;
		case GL_POSITION:
			fparam[0] = (GLfloat) params[0];
			fparam[1] = (GLfloat) params[1];
			fparam[2] = (GLfloat) params[2];
			fparam[3] = (GLfloat) params[3];
			break;
		case GL_SPOT_DIRECTION:
			fparam[0] = (GLfloat) params[0];
			fparam[1] = (GLfloat) params[1];
			fparam[2] = (GLfloat) params[2];
			break;
		case GL_SPOT_EXPONENT:
		case GL_SPOT_CUTOFF:
		case GL_CONSTANT_ATTENUATION:
		case GL_LINEAR_ATTENUATION:
		case GL_QUADRATIC_ATTENUATION:
			fparam[0] = (GLfloat) params[0];
			break;
		default:
			/* error will be caught later in gl_Lightfv */
			;
	}
	SFUNCTION(Lightfv)	(*CC->API.Lightfv)(CC, light, pname, fparam, 4 );
}



void APIENTRY aglLightModelf(void *cc,GLenum pname, GLfloat param )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LightModelfv)	(*CC->API.LightModelfv)(CC, pname, &param );
}


void APIENTRY aglLightModeli(void *cc,GLenum pname, GLint param )
{
	GLfloat fparam[4];
	GET_CONTEXT; CHECK_CONTEXT;

	fparam[0] = (GLfloat) param;
	SFUNCTION(LightModelfv)	(*CC->API.LightModelfv)(CC, pname, fparam );
}


void APIENTRY aglLightModelfv(void *cc,GLenum pname, const GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LightModelfv)	(*CC->API.LightModelfv)(CC, pname, params );
}


void APIENTRY aglLightModeliv(void *cc,GLenum pname, const GLint *params )
{
	GLfloat fparam[4];
	GET_CONTEXT; CHECK_CONTEXT;


	switch (pname) {
		case GL_LIGHT_MODEL_AMBIENT:
			fparam[0] = INT_TO_FLOAT( params[0] );
			fparam[1] = INT_TO_FLOAT( params[1] );
			fparam[2] = INT_TO_FLOAT( params[2] );
			fparam[3] = INT_TO_FLOAT( params[3] );
			break;
		case GL_LIGHT_MODEL_LOCAL_VIEWER:
		case GL_LIGHT_MODEL_TWO_SIDE:
			fparam[0] = (GLfloat) params[0];
			break;
		default:
			/* Error will be caught later in gl_LightModelfv */
			;
	}
	SFUNCTION(LightModelfv)	(*CC->API.LightModelfv)(CC, pname, fparam );
}


void APIENTRY aglLineWidth(void *cc,GLfloat width )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LineWidth)	(*CC->API.LineWidth)(CC, width);
}


void APIENTRY aglLineStipple(void *cc,GLint factor, GLushort pattern )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LineStipple)	(*CC->API.LineStipple)(CC, factor, pattern);
}


void APIENTRY aglListBase(void *cc,GLuint base )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(ListBase)	(*CC->API.ListBase)(CC, base);
}


void APIENTRY aglLoadIdentity(void *cc )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LoadIdentity)	(*CC->API.LoadIdentity)(CC );
}


void APIENTRY aglLoadMatrixd(void *cc,const GLdouble *m )
{
	GLfloat fm[16];
	GLuint i;
	GET_CONTEXT; CHECK_CONTEXT;


	for (i=0;i<16;i++) {
		fm[i] = (GLfloat) m[i];
	}

	SFUNCTION(LoadMatrixf)	(*CC->API.LoadMatrixf)(CC, fm );
}


void APIENTRY aglLoadMatrixf(void *cc,const GLfloat *m )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LoadMatrixf)	(*CC->API.LoadMatrixf)(CC, m );
}


void APIENTRY aglLoadName(void *cc,GLuint name )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LoadName)	(*CC->API.LoadName)(CC, name);
}


void APIENTRY aglLogicOp(void *cc,GLenum opcode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(LogicOp)	(*CC->API.LogicOp)(CC, opcode);
}



void APIENTRY aglMap1d(void *cc,GLenum target, GLdouble u1, GLdouble u2, GLint stride,
							  GLint order, const GLdouble *points )
{
	GLfloat *pnts;
	GLboolean retain;
	GET_CONTEXT; CHECK_CONTEXT;


	pnts = gl_copy_map_points1d( target, stride, order, points );
	retain = CC->CompileFlag;
	SFUNCTION(Map1f)	(*CC->API.Map1f)(CC, target, u1, u2, stride, order, pnts, retain );
}


void APIENTRY aglMap1f(void *cc,GLenum target, GLfloat u1, GLfloat u2, GLint stride,
							  GLint order, const GLfloat *points )
{
	GLfloat *pnts;
	GLboolean retain;
	GET_CONTEXT; CHECK_CONTEXT;


	pnts = gl_copy_map_points1f( target, stride, order, points );
	retain = CC->CompileFlag;
	SFUNCTION(Map1f)	(*CC->API.Map1f)(CC, target, u1, u2, stride, order, pnts, retain );
}


void APIENTRY aglMap2d(void *cc,GLenum target,
							  GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
							  GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
							  const GLdouble *points )
{
	GLfloat *pnts;
	GLboolean retain;
	GET_CONTEXT; CHECK_CONTEXT;


	pnts = gl_copy_map_points2d( target, ustride, uorder,
										  vstride, vorder, points );
	retain = CC->CompileFlag;
	SFUNCTION(Map2f)	(*CC->API.Map2f)(CC, target, u1, u2, ustride, uorder,
							v1, v2, vstride, vorder, pnts, retain );
}


void APIENTRY aglMap2f(void *cc,GLenum target,
							  GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
							  GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
							  const GLfloat *points )
{
	GLfloat *pnts;
	GLboolean retain;
	GET_CONTEXT; CHECK_CONTEXT;


	pnts = gl_copy_map_points2f( target, ustride, uorder,
										  vstride, vorder, points );
	retain = CC->CompileFlag;
	SFUNCTION(Map2f)	(*CC->API.Map2f)(CC, target, u1, u2, ustride, uorder,
							v1, v2, vstride, vorder, pnts, retain );
}


void APIENTRY aglMapGrid1d(void *cc,GLint un, GLdouble u1, GLdouble u2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MapGrid1f)	(*CC->API.MapGrid1f)(CC, un, (GLfloat) u1, (GLfloat) u2 );
}


void APIENTRY aglMapGrid1f(void *cc,GLint un, GLfloat u1, GLfloat u2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MapGrid1f)	(*CC->API.MapGrid1f)(CC, un, u1, u2 );
}


void APIENTRY aglMapGrid2d(void *cc,GLint un, GLdouble u1, GLdouble u2,
									GLint vn, GLdouble v1, GLdouble v2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MapGrid2f)	(*CC->API.MapGrid2f)(CC, un, (GLfloat) u1, (GLfloat) u2,
								 vn, (GLfloat) v1, (GLfloat) v2 );
}


void APIENTRY aglMapGrid2f(void *cc,GLint un, GLfloat u1, GLfloat u2,
									GLint vn, GLfloat v1, GLfloat v2 )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MapGrid2f)	(*CC->API.MapGrid2f)(CC, un, u1, u2, vn, v1, v2 );
}


void APIENTRY aglMaterialf(void *cc,GLenum face, GLenum pname, GLfloat param )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Materialfv)	(*CC->API.Materialfv)(CC, face, pname, &param );
}



void APIENTRY aglMateriali(void *cc,GLenum face, GLenum pname, GLint param )
{
	GLfloat fparam[4];
	GET_CONTEXT; CHECK_CONTEXT;

	fparam[0] = (GLfloat) param;
	SFUNCTION(Materialfv)	(*CC->API.Materialfv)(CC, face, pname, fparam );
}


void APIENTRY aglMaterialfv(void *cc,GLenum face, GLenum pname, const GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(Materialfv)	(*CC->API.Materialfv)(CC, face, pname, params );
}


void APIENTRY aglMaterialiv(void *cc,GLenum face, GLenum pname, const GLint *params )
{
	GLfloat fparam[4];
	GET_CONTEXT; CHECK_CONTEXT;

	switch (pname) {
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
		case GL_AMBIENT_AND_DIFFUSE:
			fparam[0] = INT_TO_FLOAT( params[0] );
			fparam[1] = INT_TO_FLOAT( params[1] );
			fparam[2] = INT_TO_FLOAT( params[2] );
			fparam[3] = INT_TO_FLOAT( params[3] );
			break;
		case GL_SHININESS:
			fparam[0] = (GLfloat) params[0];
			break;
		case GL_COLOR_INDEXES:
			fparam[0] = (GLfloat) params[0];
			fparam[1] = (GLfloat) params[1];
			fparam[2] = (GLfloat) params[2];
			break;
		default:
			/* Error will be caught later in gl_Materialfv */
			;
	}
	SFUNCTION(Materialfv)	(*CC->API.Materialfv)(CC, face, pname, fparam );
}


void APIENTRY aglMatrixMode(void *cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MatrixMode)	(*CC->API.MatrixMode)(CC, mode );
}


void APIENTRY aglMultMatrixd(void *cc,const GLdouble *m )
{
	GLfloat fm[16];
	GLuint i;
	GET_CONTEXT; CHECK_CONTEXT;


	for (i=0;i<16;i++) {
		fm[i] = (GLfloat) m[i];
	}

	SFUNCTION(MultMatrixf)	(*CC->API.MultMatrixf)(CC, fm );
}


void APIENTRY aglMultMatrixf(void *cc,const GLfloat *m )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(MultMatrixf)	(*CC->API.MultMatrixf)(CC, m );
}


void APIENTRY aglNewList(void *cc,GLuint list, GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;

	SFUNCTION(NewList)	(*CC->API.NewList)(CC, list, mode);
}

void APIENTRY aglNormal3b(void *cc,GLbyte nx, GLbyte ny, GLbyte nz )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, BYTE_TO_FLOAT(nx),
								BYTE_TO_FLOAT(ny), BYTE_TO_FLOAT(nz) );
}


void APIENTRY aglNormal3d(void *cc,GLdouble nx, GLdouble ny, GLdouble nz )
{
	GLfloat fx, fy, fz;
	GET_CONTEXT; CHECK_CONTEXT;
	if (ABSD(nx)<0.00001)	fx = 0.0F;	else  fx = nx;
	if (ABSD(ny)<0.00001)	fy = 0.0F;	else  fy = ny;
	if (ABSD(nz)<0.00001)	fz = 0.0F;	else  fz = nz;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, fx, fy, fz );
}


void APIENTRY aglNormal3f(void *cc,GLfloat nx, GLfloat ny, GLfloat nz )
{
	GET_CONTEXT; CHECK_CONTEXT;
#ifdef SHORTCUT
	if (CC->CompileFlag) {
		(*CC->Save.Normal3f)(CC, nx, ny, nz );
	}
	else {
		/* Execute */
		CC->Current.Normal[0] = nx;
		CC->Current.Normal[1] = ny;
		CC->Current.Normal[2] = nz;
		CC->VB->MonoNormal = GL_FALSE;
	}
#else
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, nx, ny, nz );
#endif
}


void APIENTRY aglNormal3i(void *cc,GLint nx, GLint ny, GLint nz )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, INT_TO_FLOAT(nx),INT_TO_FLOAT(ny), INT_TO_FLOAT(nz) );
}


void APIENTRY aglNormal3s(void *cc,GLshort nx, GLshort ny, GLshort nz )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, SHORT_TO_FLOAT(nx),SHORT_TO_FLOAT(ny), SHORT_TO_FLOAT(nz) );
}


void APIENTRY aglNormal3bv(void *cc,const GLbyte *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, BYTE_TO_FLOAT(v[0]),BYTE_TO_FLOAT(v[1]), BYTE_TO_FLOAT(v[2]) );
}


void APIENTRY aglNormal3dv(void *cc,const GLdouble *v )
{
	GLfloat fx, fy, fz;
	GET_CONTEXT; CHECK_CONTEXT;
	if (ABSD(v[0])<0.00001)	fx = 0.0F;	else  fx = v[0];
	if (ABSD(v[1])<0.00001)	fy = 0.0F;	else  fy = v[1];
	if (ABSD(v[2])<0.00001)	fz = 0.0F;	else  fz = v[2];
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, fx, fy, fz );
}


void APIENTRY aglNormal3fv(void *cc,const GLfloat *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
#ifdef SHORTCUT
	if (CC->CompileFlag) {
		(*CC->Save.Normal3fv)(CC, v );
	}
	else {
		/* Execute */
		GLfloat *n = CC->Current.Normal;
		n[0] = v[0];
		n[1] = v[1];
		n[2] = v[2];
		CC->VB->MonoNormal = GL_FALSE;
	}
#else
	SFUNCTION(Normal3fv)	(*CC->API.Normal3fv)(CC, v );
#endif
}


void APIENTRY aglNormal3iv(void *cc,const GLint *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, INT_TO_FLOAT(v[0]),
								INT_TO_FLOAT(v[1]), INT_TO_FLOAT(v[2]) );
}


void APIENTRY aglNormal3sv(void *cc,const GLshort *v )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(Normal3f)	(*CC->API.Normal3f)(CC, SHORT_TO_FLOAT(v[0]),SHORT_TO_FLOAT(v[1]), SHORT_TO_FLOAT(v[2]) );
}


void APIENTRY aglNormalPointer(void *cc,GLenum type, GLsizei stride, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
	SFUNCTION(NormalPointer)	(*CC->API.NormalPointer)(CC, type, stride, ptr);
}



