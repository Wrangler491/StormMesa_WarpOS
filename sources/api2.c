/* $Id: api2.c,v 3.6 1998/08/21 02:43:52 brianp Exp $ */

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
 * $Log: api2.c,v $
 * Revision 3.6  1998/08/21 02:43:52  brianp
 * implemented true packing/unpacking of polygon stipples
 *
 * Revision 3.5  1998/07/26 02:32:43  brianp
 * removed unused variable
 *
 * Revision 3.4  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.3  1998/03/10 01:26:57  brianp
 * updated for David's v0.23 fxmesa driver
 *
 * Revision 3.2  1998/02/20 04:49:19  brianp
 * move extension functions into apiext.c
 *
 * Revision 3.1  1998/02/06 01:57:42  brianp
 * added GL 1.2 3-D texture enums and functions
 *
 * Revision 3.0  1998/01/31 20:43:12  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include "api.h"
#include "context.h"
#include "image.h"
#include "macros.h"
#include "matrix.h"
#include "teximage.h"
#include "types.h"
#include "vb.h"
#endif


/*
 * Part 2 of API functions
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
 
		 
void APIENTRY aglOrtho(void* cc,GLdouble left, GLdouble right,		
							  GLdouble bottom, GLdouble top,		
							  GLdouble nearval, GLdouble farval )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Ortho)	(*CC->API.Ortho)(CC, left, right, bottom, top, nearval, farval);		
}		
		
		
void APIENTRY aglPassThrough(void* cc,GLfloat token )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PassThrough)	(*CC->API.PassThrough)(CC, token);		
}		
		
		
void APIENTRY aglPixelMapfv(void* cc,GLenum map, GLint mapsize, const GLfloat *values )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelMapfv)	(*CC->API.PixelMapfv)(CC, map, mapsize, values );		
}		
		
		
void APIENTRY aglPixelMapuiv(void* cc,GLenum map, GLint mapsize, const GLuint *values )		
{		
	GLfloat fvalues[MAX_PIXEL_MAP_TABLE];		
	GLint i;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
		
	if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {		
		for (i=0;i<mapsize;i++) {		
			fvalues[i] = (GLfloat) values[i];		
		}		
	}		
	else {		
		for (i=0;i<mapsize;i++) {		
			fvalues[i] = UINT_TO_FLOAT(values[i] );		
		}		
	}		
	SFUNCTION(PixelMapfv)	(*CC->API.PixelMapfv)(CC, map, mapsize, fvalues );		
}		
		
		
		
void APIENTRY aglPixelMapusv(void* cc,GLenum map, GLint mapsize, const GLushort *values )		
{		
	GLfloat fvalues[MAX_PIXEL_MAP_TABLE];		
	GLint i;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
		
	if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {		
		for (i=0;i<mapsize;i++) {		
			fvalues[i] = (GLfloat) values[i];		
		}		
	}		
	else {		
		for (i=0;i<mapsize;i++) {		
			fvalues[i] = USHORT_TO_FLOAT(values[i] );		
		}		
	}		
	SFUNCTION(PixelMapfv)	(*CC->API.PixelMapfv)(CC, map, mapsize, fvalues );		
}		
		
		
void APIENTRY aglPixelStoref(void* cc,GLenum pname, GLfloat param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelStorei)	(*CC->API.PixelStorei)(CC, pname, (GLint) param );		
}		
		
		
void APIENTRY aglPixelStorei(void* cc,GLenum pname, GLint param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelStorei)	(*CC->API.PixelStorei)(CC, pname, param );		
}		
		
		
void APIENTRY aglPixelTransferf(void* cc,GLenum pname, GLfloat param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelTransferf)	(*CC->API.PixelTransferf)(CC, pname, param);		
}		
		
		
void APIENTRY aglPixelTransferi(void* cc,GLenum pname, GLint param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelTransferf)	(*CC->API.PixelTransferf)(CC, pname, (GLfloat) param);		
}		
		
		
void APIENTRY aglPixelZoom(void* cc,GLfloat xfactor, GLfloat yfactor )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PixelZoom)	(*CC->API.PixelZoom)(CC, xfactor, yfactor);		
}		
		
		
void APIENTRY aglPointSize(void* cc,GLfloat size )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PointSize)	(*CC->API.PointSize)(CC, size);		
}		
		
		
void APIENTRY aglPolygonMode(void* cc,GLenum face, GLenum mode )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PolygonMode)	(*CC->API.PolygonMode)(CC, face, mode);		
}		
		
		
void APIENTRY aglPolygonOffset(void* cc,GLfloat factor, GLfloat units )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PolygonOffset)	(*CC->API.PolygonOffset)(CC, factor, units );		
}		
		
		
/* GL_EXT_polygon_offset */		
void APIENTRY aglPolygonOffsetEXT(void* cc,GLfloat factor, GLfloat bias )		
{		
	aglPolygonOffset(cc, factor, bias * DEPTH_SCALE );		
}		
		
		
void APIENTRY aglPolygonStipple(void* cc,const GLubyte *pattern )		
{		
	GLuint unpackedPattern[32];		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	gl_unpack_polygon_stipple(CC, pattern, unpackedPattern );		
	SFUNCTION(PolygonStipple)	(*CC->API.PolygonStipple)(CC, unpackedPattern);		
}		
		
		
void APIENTRY aglPopAttrib(void* cc )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PopAttrib)	(*CC->API.PopAttrib)(CC);		
}		
		
		
void APIENTRY aglPopClientAttrib(void* cc )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PopClientAttrib)	(*CC->API.PopClientAttrib)(CC);		
}		
		
		
void APIENTRY aglPopMatrix(void* cc )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PopMatrix)	(*CC->API.PopMatrix)(CC );		
}		
		
		
void APIENTRY aglPopName(void* cc )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PopName)	(*CC->API.PopName)(CC);		
}		
		
		
void APIENTRY aglPrioritizeTextures(void* cc,GLsizei n, const GLuint *textures,		
												const GLclampf *priorities )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PrioritizeTextures)	(*CC->API.PrioritizeTextures)(CC, n, textures, priorities);		
}		
		
		
void APIENTRY aglPushMatrix(void* cc )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PushMatrix)	(*CC->API.PushMatrix)(CC );		
}		
		
		
void APIENTRY aglRasterPos2d(void* cc,GLdouble x, GLdouble y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2f(void* cc,GLfloat x, GLfloat y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2i(void* cc,GLint x, GLint y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2s(void* cc,GLshort x, GLshort y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3d(void* cc,GLdouble x, GLdouble y, GLdouble z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3f(void* cc,GLfloat x, GLfloat y, GLfloat z )		 
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3i(void* cc,GLint x, GLint y, GLint z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3s(void* cc,GLshort x, GLshort y, GLshort z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos4d(void* cc,GLdouble x, GLdouble y, GLdouble z, GLdouble w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y,		
								(GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglRasterPos4f(void* cc,GLfloat x, GLfloat y, GLfloat z, GLfloat w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, x, y, z, w );		
}		
		
		
void APIENTRY aglRasterPos4i(void* cc,GLint x, GLint y, GLint z, GLint w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y,		
									(GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglRasterPos4s(void* cc,GLshort x, GLshort y, GLshort z, GLshort w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) x, (GLfloat) y,		
									(GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglRasterPos2dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );		
}		
		
		
void APIENTRY aglRasterPos2sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );		
}		
		
		
/*** 3 element vector ***/		
		
void APIENTRY aglRasterPos3dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], 1.0F );		
}		
		
		
void APIENTRY aglRasterPos3sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], 1.0F );		
}		
		
		
void APIENTRY aglRasterPos4dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglRasterPos4fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, v[0], v[1], v[2], v[3] );		
}		
		
		
void APIENTRY aglRasterPos4iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglRasterPos4sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(RasterPos4f)	(*CC->API.RasterPos4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									(GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglReadBuffer(void* cc,GLenum mode )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(ReadBuffer)	(*CC->API.ReadBuffer)(CC, mode );		
}		
		
		
void APIENTRY aglReadPixels(void* cc,GLint x, GLint y, GLsizei width, GLsizei height,		
			GLenum format, GLenum type, GLvoid *pixels )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(ReadPixels)	(*CC->API.ReadPixels)(CC, x, y, width, height, format, type, pixels );		
}		
		
		
void APIENTRY aglRectd(void* cc,GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) x1, (GLfloat) y1,		
							(GLfloat) x2, (GLfloat) y2 );		
}		
		
		
void APIENTRY aglRectf(void* cc,GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, x1, y1, x2, y2 );		
}		
		
		
void APIENTRY aglRecti(void* cc,GLint x1, GLint y1, GLint x2, GLint y2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) x1, (GLfloat) y1,		
								 (GLfloat) x2, (GLfloat) y2 );		
}		
		
		
void APIENTRY aglRects(void* cc,GLshort x1, GLshort y1, GLshort x2, GLshort y2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) x1, (GLfloat) y1,		
							(GLfloat) x2, (GLfloat) y2 );		
}		
		
		
void APIENTRY aglRectdv(void* cc,const GLdouble *v1, const GLdouble *v2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],		
						  (GLfloat) v2[0], (GLfloat) v2[1]);		
}		
		
		
void APIENTRY aglRectfv(void* cc,const GLfloat *v1, const GLfloat *v2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, v1[0], v1[1], v2[0], v2[1]);		
}		
		
		
void APIENTRY aglRectiv(void* cc,const GLint *v1, const GLint *v2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],		
							(GLfloat) v2[0], (GLfloat) v2[1] );		
}		
		
		
void APIENTRY aglRectsv(void* cc,const GLshort *v1, const GLshort *v2 )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rectf)	(*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],		
		  (GLfloat) v2[0], (GLfloat) v2[1]);		
}		
		
		
void APIENTRY aglScissor(void* cc,GLint x, GLint y, GLsizei width, GLsizei height)		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Scissor)	(*CC->API.Scissor)(CC, x, y, width, height);		
}		
		
		
GLboolean APIENTRY aglIsEnabled(void* cc,GLenum cap )		
{		
	GET_CONTEXT; CHECK_CONTEXT_RETURN(GL_FALSE);		
	SFUNCTION(IsEnabled)	return (*CC->API.IsEnabled)(CC, cap );		
}		
		
		
		
void APIENTRY aglPushAttrib(void* cc,GLbitfield mask )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PushAttrib)	(*CC->API.PushAttrib)(CC, mask);		
}		
		
		
void APIENTRY aglPushClientAttrib(void* cc,GLbitfield mask )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PushClientAttrib)	(*CC->API.PushClientAttrib)(CC, mask);		
}		
		
		
void APIENTRY aglPushName(void* cc,GLuint name )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(PushName)	(*CC->API.PushName)(CC, name);		
}		
		
		
GLint APIENTRY aglRenderMode(void* cc,GLenum mode )		
{		
	GET_CONTEXT; CHECK_CONTEXT_RETURN(0);		
	SFUNCTION(RenderMode)	return (*CC->API.RenderMode)(CC, mode);		
}		
		
		
void APIENTRY aglRotated(void* cc,GLdouble angle, GLdouble x, GLdouble y, GLdouble z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rotatef)	(*CC->API.Rotatef)(CC, (GLfloat) angle,		
							  (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglRotatef(void* cc,GLfloat angle, GLfloat x, GLfloat y, GLfloat z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Rotatef)	(*CC->API.Rotatef)(CC, angle, x, y, z );		
}		
		
		
void APIENTRY aglSelectBuffer(void* cc,GLsizei size, GLuint *buffer )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(SelectBuffer)	(*CC->API.SelectBuffer)(CC, size, buffer);		
}		
		
		
void APIENTRY aglScaled(void* cc,GLdouble x, GLdouble y, GLdouble z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Scalef)	(*CC->API.Scalef)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglScalef(void* cc,GLfloat x, GLfloat y, GLfloat z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Scalef)	(*CC->API.Scalef)(CC, x, y, z );		
}		
		
		
void APIENTRY aglShadeModel(void* cc,GLenum mode )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(ShadeModel)	(*CC->API.ShadeModel)(CC, mode);		
}		
		
		
void APIENTRY aglStencilFunc(void* cc,GLenum func, GLint ref, GLuint mask )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(StencilFunc)	(*CC->API.StencilFunc)(CC, func, ref, mask);		
}		
		
		
void APIENTRY aglStencilMask(void* cc,GLuint mask )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(StencilMask)	(*CC->API.StencilMask)(CC, mask);		
}		
		
		
void APIENTRY aglStencilOp(void* cc,GLenum fail, GLenum zfail, GLenum zpass )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(StencilOp)	(*CC->API.StencilOp)(CC, fail, zfail, zpass);		
}		
		
		
void APIENTRY aglTexCoord1d(void* cc,GLdouble s )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1f(void* cc,GLfloat s )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, s, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1i(void* cc,GLint s )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1s(void* cc,GLshort s )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord2d(void* cc,GLdouble s, GLdouble t )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) s, (GLfloat) t );		
}		
		
		
void APIENTRY aglTexCoord2f(void* cc,GLfloat s, GLfloat t )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, s, t );		
}		
		
		
void APIENTRY aglTexCoord2i(void* cc,GLint s, GLint t )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) s, (GLfloat) t );		
}		
		
		
void APIENTRY aglTexCoord2s(void* cc,GLshort s, GLshort t )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) s, (GLfloat) t );		
}		
		
		
void APIENTRY aglTexCoord3d(void* cc,GLdouble s, GLdouble t, GLdouble r )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t, (GLfloat) r, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3f(void* cc,GLfloat s, GLfloat t, GLfloat r )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, s, t, r, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3i(void* cc,GLint s, GLint t, GLint r )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t,		
										 (GLfloat) r, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3s(void* cc,GLshort s, GLshort t, GLshort r )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t,		
										 (GLfloat) r, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord4d(void* cc,GLdouble s, GLdouble t, GLdouble r, GLdouble q )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t,		
										 (GLfloat) r, (GLfloat) q );		
}		
		
		
void APIENTRY aglTexCoord4f(void* cc,GLfloat s, GLfloat t, GLfloat r, GLfloat q )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, s, t, r, q );		
}		
		
		
void APIENTRY aglTexCoord4i(void* cc,GLint s, GLint t, GLint r, GLint q )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t,		
										 (GLfloat) r, (GLfloat) q );		
}		
		
		
void APIENTRY aglTexCoord4s(void* cc,GLshort s, GLshort t, GLshort r, GLshort q )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) s, (GLfloat) t,		
										 (GLfloat) r, (GLfloat) q );		
}		
		
		
void APIENTRY aglTexCoord1dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) *v, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, *v, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, *v, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord1sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) *v, 0.0, 0.0, 1.0 );		
}		
		
		
void APIENTRY aglTexCoord2dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		 
}		
		
		
void APIENTRY aglTexCoord2fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, v[0], v[1] );		
}		
		
		
void APIENTRY aglTexCoord2iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		
}		
		
		
void APIENTRY aglTexCoord2sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord2f)	(*CC->API.TexCoord2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		
}		
		
		
void APIENTRY aglTexCoord3dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, v[0], v[1], v[2], 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
								  (GLfloat) v[2], 1.0 );		
}		
		
		
void APIENTRY aglTexCoord3sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], 1.0 );		
}		
		
		
void APIENTRY aglTexCoord4dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglTexCoord4fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, v[0], v[1], v[2], v[3] );		
}		
		
		
void APIENTRY aglTexCoord4iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglTexCoord4sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(TexCoord4f)	(*CC->API.TexCoord4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
										 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglTexCoordPointer(void* cc,GLint size, GLenum type, GLsizei stride,		
											const GLvoid *ptr )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexCoordPointer)	(*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);		
}		
		
		
void APIENTRY aglTexGend(void* cc,GLenum coord, GLenum pname, GLdouble param )		
{		
	GLfloat p = (GLfloat) param;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, &p );		
}		
		
		
void APIENTRY aglTexGenf(void* cc,GLenum coord, GLenum pname, GLfloat param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, &param );		
}		
		
		
void APIENTRY aglTexGeni(void* cc,GLenum coord, GLenum pname, GLint param )		
{		
	GLfloat p = (GLfloat) param;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, &p );		
}		
		
		
void APIENTRY aglTexGendv(void* cc,GLenum coord, GLenum pname, const GLdouble *params )		
{		
	GLfloat p[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	p[0] = params[0];		
	p[1] = params[1];		
	p[2] = params[2];		
	p[3] = params[3];		
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, p );		
}		
		
		
void APIENTRY aglTexGeniv(void* cc,GLenum coord, GLenum pname, const GLint *params )		
{		
	GLfloat p[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	p[0] = params[0];		
	p[1] = params[1];		
	p[2] = params[2];		
	p[3] = params[3];		
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, p );		
}		
		
		
void APIENTRY aglTexGenfv(void* cc,GLenum coord, GLenum pname, const GLfloat *params )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexGenfv)	(*CC->API.TexGenfv)(CC, coord, pname, params );		
}		
		
		
		
		
void APIENTRY aglTexEnvf(void* cc,GLenum target, GLenum pname, GLfloat param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexEnvfv)	(*CC->API.TexEnvfv)(CC, target, pname, &param );		
}		
		
		
		
void APIENTRY aglTexEnvi(void* cc,GLenum target, GLenum pname, GLint param )		
{		
	GLfloat p[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
	p[0] = (GLfloat) param;		
	p[1] = p[2] = p[3] = 0.0;		
			
	SFUNCTION(TexEnvfv)	(*CC->API.TexEnvfv)(CC, target, pname, p );		
}		
		
		
		
void APIENTRY aglTexEnvfv(void* cc,GLenum target, GLenum pname, const GLfloat *param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexEnvfv)	(*CC->API.TexEnvfv)(CC, target, pname, param );		
}		
		
		
		
void APIENTRY aglTexEnviv(void* cc,GLenum target, GLenum pname, const GLint *param )		
{		
	GLfloat p[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
	p[0] = INT_TO_FLOAT(param[0] );		
	p[1] = INT_TO_FLOAT(param[1] );		
	p[2] = INT_TO_FLOAT(param[2] );		
	p[3] = INT_TO_FLOAT(param[3] );		
			
	SFUNCTION(TexEnvfv)	(*CC->API.TexEnvfv)(CC, target, pname, p );		
}		
		
		
void APIENTRY aglTexImage1D(void* cc,GLenum target, GLint level, GLint internalformat,		
									 GLsizei width, GLint border,		
									 GLenum format, GLenum type, const GLvoid *pixels )		
{		
	struct gl_image *teximage;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	teximage = gl_unpack_image(CC, width, 1, format, type, pixels );		
	SFUNCTION(TexImage1D)	(*CC->API.TexImage1D)(CC, target, level, internalformat,		
								  width, border, format, type, teximage );		
}		
		
		
		
void APIENTRY aglTexImage2D(void* cc,GLenum target, GLint level, GLint internalformat,		
									 GLsizei width, GLsizei height, GLint border,		
									 GLenum format, GLenum type, const GLvoid *pixels )		
{		
  struct gl_image *teximage;		
#if defined(FX) && defined(__WIN32__)		
  GLvoid *newpixels=NULL;		
  GLsizei newwidth,newheight;		
  GLint x,y;		
  static GLint leveldif=0;		
  static GLuint lasttexobj=0;		
#endif		
  GET_CONTEXT; CHECK_CONTEXT;		
  		
		
#if defined(FX) && defined(__WIN32__)		
  newpixels=NULL;		
  		
  /* AN HACK for WinGLQuake*/		
  		
  if (CC->Texture.Set[0].Current2D->Name!=lasttexobj) {		
	 lasttexobj=CC->Texture.Set[0].Current2D->Name;		
	 leveldif=0;		
  }		
  		
  if ((format==GL_COLOR_INDEX) && (internalformat==1))		
	 internalformat=GL_COLOR_INDEX8_EXT;		
  		
  if (width>256 || height>256) {		
	 newpixels=malloc((width+4)*height*4);		
		
	 while (width>256 || height>256) {		
		newwidth=width/2;		
		newheight=height/2;		
		leveldif++;		
				
		/*LibPrintf*/ printf("Scaling: (%d) %dx%d -> %dx%d\n",internalformat,width,height,newwidth,newheight);		
				
		for(y=0;y<newheight;y++)		
	for(x=0;x<newwidth;x++) {		
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+0]=((GLubyte *)pixels)[(x*2+y*width*2)*4+0];		
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+1]=((GLubyte *)pixels)[(x*2+y*width*2)*4+1];		
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+2]=((GLubyte *)pixels)[(x*2+y*width*2)*4+2];		
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+3]=((GLubyte *)pixels)[(x*2+y*width*2)*4+3];		
	}		
		
		pixels=newpixels;		
		width=newwidth;		
		height=newheight;		
	 }		
	 		
	 level=0;		
  } else		
	 level-=leveldif;		
#endif		
  teximage = gl_unpack_image(CC, width, height, format, type, pixels );		
	SFUNCTION(TexImage2D)	(*CC->API.TexImage2D)(CC, target, level, internalformat, width, height, border, format, type, teximage );		
#if defined(FX) && defined(__WIN32__)		
  if(newpixels)		
	 free(newpixels);		
#endif		
}		
		
		
void APIENTRY aglTexImage3D(void* cc,GLenum target, GLint level, GLenum internalformat,		
									 GLsizei width, GLsizei height, GLsizei depth,		
									 GLint border, GLenum format, GLenum type,		
									 const GLvoid *pixels )		
{		
	struct gl_image *teximage;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	teximage = gl_unpack_image3D(CC, width, height, depth, format, type, pixels);		
	SFUNCTION(TexImage3DEXT)	(*CC->API.TexImage3DEXT)(CC, target, level, internalformat,		
									  width, height, depth, border, format, type, 		
									  teximage );		
}		
		
		
void APIENTRY aglTexParameterf(void* cc,GLenum target, GLenum pname, GLfloat param )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexParameterfv)	(*CC->API.TexParameterfv)(CC, target, pname, &param );		
}		
		
		
void APIENTRY aglTexParameteri(void* cc,GLenum target, GLenum pname, GLint param )		
{		
	GLfloat fparam[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
	fparam[0] = (GLfloat) param;		
	fparam[1] = fparam[2] = fparam[3] = 0.0;		
			
	SFUNCTION(TexParameterfv)	(*CC->API.TexParameterfv)(CC, target, pname, fparam );		
}		
		
		
void APIENTRY aglTexParameterfv(void* cc,GLenum target, GLenum pname, const GLfloat *params )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(TexParameterfv)	(*CC->API.TexParameterfv)(CC, target, pname, params );		
}		
		
		
void APIENTRY aglTexParameteriv(void* cc,GLenum target, GLenum pname, const GLint *params )		
{		
	GLfloat p[4];		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	if (pname==GL_TEXTURE_BORDER_COLOR) {		
		p[0] = INT_TO_FLOAT(params[0] );		
		p[1] = INT_TO_FLOAT(params[1] );		
		p[2] = INT_TO_FLOAT(params[2] );		
		p[3] = INT_TO_FLOAT(params[3] );		
	}		
	else {		
		p[0] = (GLfloat) params[0];		
		p[1] = (GLfloat) params[1];		
		p[2] = (GLfloat) params[2];		
		p[3] = (GLfloat) params[3];		
	}		
	SFUNCTION(TexParameterfv)	(*CC->API.TexParameterfv)(CC, target, pname, p );		
}		
		
		
void APIENTRY aglTexSubImage1D(void* cc,GLenum target, GLint level, GLint xoffset,		
										 GLsizei width, GLenum format,		
										 GLenum type, const GLvoid *pixels )		
{		
	struct gl_image *image;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	image = gl_unpack_texsubimage(CC, width, 1, format, type, pixels );		
	SFUNCTION(TexSubImage1D)	(*CC->API.TexSubImage1D)(CC, target, level, xoffset, width,		
									  format, type, image );		
}		
		
		
void APIENTRY aglTexSubImage2D(void* cc,GLenum target, GLint level,		
										 GLint xoffset, GLint yoffset,		
										 GLsizei width, GLsizei height,		
										 GLenum format, GLenum type,		
										 const GLvoid *pixels )		
{		
	struct gl_image *image;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	image = gl_unpack_texsubimage(CC, width, height, format, type, pixels );		
	SFUNCTION(TexSubImage2D)	(*CC->API.TexSubImage2D)(CC, target, level, xoffset, yoffset,		
									  width, height, format, type, image );		
}		
		
		
void APIENTRY aglTexSubImage3D(void* cc,GLenum target, GLint level, GLint xoffset,		
										 GLint yoffset, GLint zoffset, GLsizei width,		
										 GLsizei height, GLsizei depth, GLenum format,		
										 GLenum type, const GLvoid *pixels )		
{		
	struct gl_image *image;		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	image = gl_unpack_texsubimage3D(CC, width, height, depth, format, type,		
												pixels );		
	SFUNCTION(TexSubImage3DEXT)	(*CC->API.TexSubImage3DEXT)(CC, target, level, xoffset, yoffset, zoffset,		
										  width, height, depth, format, type, image );		
}		
		
		
void APIENTRY aglTranslated(void* cc,GLdouble x, GLdouble y, GLdouble z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Translatef)	(*CC->API.Translatef)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglTranslatef(void* cc,GLfloat x, GLfloat y, GLfloat z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
			
	SFUNCTION(Translatef)	(*CC->API.Translatef)(CC, x, y, z );		
}		
		
		
void APIENTRY aglVertex2d(void* cc,GLdouble x, GLdouble y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) x, (GLfloat) y );		
}		
		
		
void APIENTRY aglVertex2f(void* cc,GLfloat x, GLfloat y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, x, y );		
}		
		
		
void APIENTRY aglVertex2i(void* cc,GLint x, GLint y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) x, (GLfloat) y );		
}		
		
		
void APIENTRY aglVertex2s(void* cc,GLshort x, GLshort y )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) x, (GLfloat) y );		
}		
		
		
void APIENTRY aglVertex3d(void* cc,GLdouble x, GLdouble y, GLdouble z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglVertex3f(void* cc,GLfloat x, GLfloat y, GLfloat z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, x, y, z );		
}		
		
		
void APIENTRY aglVertex3i(void* cc,GLint x, GLint y, GLint z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglVertex3s(void* cc,GLshort x, GLshort y, GLshort z )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );		
}		
		
		
void APIENTRY aglVertex4d(void* cc,GLdouble x, GLdouble y, GLdouble z, GLdouble w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) x, (GLfloat) y,		
									 (GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglVertex4f(void* cc,GLfloat x, GLfloat y, GLfloat z, GLfloat w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, x, y, z, w );		
}		
		
		
void APIENTRY aglVertex4i(void* cc,GLint x, GLint y, GLint z, GLint w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) x, (GLfloat) y,		
									 (GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglVertex4s(void* cc,GLshort x, GLshort y, GLshort z, GLshort w )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) x, (GLfloat) y,		
									 (GLfloat) z, (GLfloat) w );		
}		
		
		
void APIENTRY aglVertex2dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		
}		
		
		
void APIENTRY aglVertex2fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, v[0], v[1] );		
}		
		
		
void APIENTRY aglVertex2iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		
}		
		
		
void APIENTRY aglVertex2sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex2f)	(*CC->API.Vertex2f)(CC, (GLfloat) v[0], (GLfloat) v[1] );		
}		
		
		
void APIENTRY aglVertex3dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );		
}		
		
		
void APIENTRY aglVertex3fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3fv)	(*CC->API.Vertex3fv)(CC, v );		
}		
		
		
void APIENTRY aglVertex3iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );		
}		
		
		
void APIENTRY aglVertex3sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex3f)	(*CC->API.Vertex3f)(CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );		
}		
		
		
void APIENTRY aglVertex4dv(void* cc,const GLdouble *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglVertex4fv(void* cc,const GLfloat *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, v[0], v[1], v[2], v[3] );		
}		
		
		
void APIENTRY aglVertex4iv(void* cc,const GLint *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglVertex4sv(void* cc,const GLshort *v )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Vertex4f)	(*CC->API.Vertex4f)(CC, (GLfloat) v[0], (GLfloat) v[1],		
									 (GLfloat) v[2], (GLfloat) v[3] );		
}		
		
		
void APIENTRY aglVertexPointer(void* cc,GLint size, GLenum type, GLsizei stride,		
										 const GLvoid *ptr )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(VertexPointer)	(*CC->API.VertexPointer)(CC, size, type, stride, ptr);		
}		
		
		
void APIENTRY aglViewport(void* cc,GLint x, GLint y, GLsizei width, GLsizei height )		
{		
	GET_CONTEXT; CHECK_CONTEXT;		
	SFUNCTION(Viewport)	(*CC->API.Viewport)(CC, x, y, width, height );		
}		
	 
