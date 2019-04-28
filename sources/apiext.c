/* $Id: apiext.c,v 3.2 1998/06/07 22:18:52 brianp Exp $ */

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
 * $Log: apiext.c,v $
 * Revision 3.2  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.1  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.0  1998/02/20 04:45:50  brianp
 * implemented GL_SGIS_multitexture
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include "api.h"
#include "context.h"
#include "types.h"
#endif



/*
 * Extension API functions
 */



/*
 * GL_EXT_blend_minmax
 */

void APIENTRY aglBlendEquationEXT(void* cc,GLenum mode )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.BlendEquation)(CC, mode);
}




/*
 * GL_EXT_blend_color
 */

void APIENTRY aglBlendColorEXT(void* cc,GLclampf red, GLclampf green,
										 GLclampf blue, GLclampf alpha )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.BlendColor)(CC, red, green, blue, alpha);
}




/*
 * GL_EXT_vertex_array
 */

void APIENTRY aglVertexPointerEXT(void* cc,GLint size, GLenum type, GLsizei stride,
											 GLsizei count, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.VertexPointer)(CC, size, type, stride, ptr);
	(void) count;
}


void APIENTRY aglNormalPointerEXT(void* cc,GLenum type, GLsizei stride, GLsizei count,
											 const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.NormalPointer)(CC, type, stride, ptr);
	(void) count;
}


void APIENTRY aglColorPointerEXT(void* cc,GLint size, GLenum type, GLsizei stride,
											GLsizei count, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.ColorPointer)(CC, size, type, stride, ptr);
	(void) count;
}


void APIENTRY aglIndexPointerEXT(void* cc,GLenum type, GLsizei stride,
											GLsizei count, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.IndexPointer)(CC, type, stride, ptr);
	(void) count;
}


void APIENTRY aglTexCoordPointerEXT(void* cc,GLint size, GLenum type, GLsizei stride,
												GLsizei count, const GLvoid *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);
	(void) count;
}


void APIENTRY aglEdgeFlagPointerEXT(void* cc,GLsizei stride, GLsizei count,
												const GLboolean *ptr )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.EdgeFlagPointer)(CC, stride, ptr);
	(void) count;
}


void APIENTRY aglGetPointervEXT(void* cc,GLenum pname, GLvoid **params )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.GetPointerv)(CC, pname, params);
}


void APIENTRY aglArrayElementEXT(void* cc,GLint i )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.ArrayElement)(CC, i);
}


void APIENTRY aglDrawArraysEXT(void* cc,GLenum mode, GLint first, GLsizei count )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.DrawArrays)(CC, mode, first, count);
}




/*
 * GL_EXT_texture_object
 */

GLboolean APIENTRY aglAreTexturesResidentEXT(void* cc,GLsizei n, const GLuint *textures,
															GLboolean *residences )
{
	return aglAreTexturesResident(cc, n, textures, residences );
}


void APIENTRY aglBindTextureEXT(void* cc,GLenum target, GLuint texture )
{
	aglBindTexture(cc, target, texture );
}


void APIENTRY aglDeleteTexturesEXT(void* cc,GLsizei n, const GLuint *textures)
{
	aglDeleteTextures(cc, n, textures );
}


void APIENTRY aglGenTexturesEXT(void* cc,GLsizei n, GLuint *textures )
{
	aglGenTextures(cc, n, textures );
}


GLboolean APIENTRY aglIsTextureEXT(void* cc,GLuint texture )
{
	return aglIsTexture(cc, texture );
}


void APIENTRY aglPrioritizeTexturesEXT(void* cc,GLsizei n, const GLuint *textures,
													const GLclampf *priorities )
{
	aglPrioritizeTextures(cc, n, textures, priorities );
}




/*
 * GL_EXT_texture3D
 */

void APIENTRY aglCopyTexSubImage3DEXT(void* cc,GLenum target, GLint level, GLint xoffset,
												  GLint yoffset, GLint zoffset,
												  GLint x, GLint y, GLsizei width,
												  GLsizei height )
{
	aglCopyTexSubImage3D(cc,target, level, xoffset, yoffset, zoffset,
							  x, y, width, height);
}



void APIENTRY aglTexImage3DEXT(void* cc,GLenum target, GLint level, GLenum internalformat,
										 GLsizei width, GLsizei height, GLsizei depth,
										 GLint border, GLenum format, GLenum type,
										 const GLvoid *pixels )
{
	aglTexImage3D(cc,target, level, internalformat, width, height, depth,
					 border, format, type, pixels);
}


void APIENTRY aglTexSubImage3DEXT(void* cc,GLenum target, GLint level, GLint xoffset,
											 GLint yoffset, GLint zoffset, GLsizei width,
											 GLsizei height, GLsizei depth, GLenum format,
											 GLenum type, const GLvoid *pixels )
{
	aglTexSubImage3D(cc,target, level, xoffset, yoffset, zoffset,
						 width, height, depth, format, type, pixels);
}




/*
 * GL_EXT_point_parameters
 */

void APIENTRY aglPointParameterfEXT(void* cc,GLenum pname, GLfloat param )
{
	GLfloat params[3];
	GET_CONTEXT; CHECK_CONTEXT;
 
	params[0] = param;
	params[1] = 0.0;
	params[2] = 0.0;
	(*CC->API.PointParameterfvEXT)(CC, pname, params);
}


void APIENTRY aglPointParameterfvEXT(void* cc,GLenum pname, const GLfloat *params )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.PointParameterfvEXT)(CC, pname, params);
}




#ifdef GL_MESA_window_pos
/*
 * Mesa implementation of aglWindowPos*MESA()
 */
void APIENTRY aglWindowPos4fMESA(void* cc,GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.WindowPos4fMESA)(CC, x, y, z, w );
}
#else
/* Implementation in winpos.c is used */
#endif


void APIENTRY aglWindowPos2iMESA(void* cc,GLint x, GLint y )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2sMESA(void* cc,GLshort x, GLshort y )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2fMESA(void* cc,GLfloat x, GLfloat y )
{
	aglWindowPos4fMESA(cc,x, y, 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2dMESA(void* cc,GLdouble x, GLdouble y )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2ivMESA(void* cc,const GLint *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2svMESA(void* cc,const GLshort *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2fvMESA(void* cc,const GLfloat *p )
{
	aglWindowPos4fMESA(cc,p[0], p[1], 0.0F, 1.0F );
}

void APIENTRY aglWindowPos2dvMESA(void* cc,const GLdouble *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY aglWindowPos3iMESA(void* cc,GLint x, GLint y, GLint z )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY aglWindowPos3sMESA(void* cc,GLshort x, GLshort y, GLshort z )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY aglWindowPos3fMESA(void* cc,GLfloat x, GLfloat y, GLfloat z )
{
	aglWindowPos4fMESA(cc,x, y, z, 1.0F );
}

void APIENTRY aglWindowPos3dMESA(void* cc,GLdouble x, GLdouble y, GLdouble z )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY aglWindowPos3ivMESA(void* cc,const GLint *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY aglWindowPos3svMESA(void* cc,const GLshort *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY aglWindowPos3fvMESA(void* cc,const GLfloat *p )
{
	aglWindowPos4fMESA(cc,p[0], p[1], p[2], 1.0F );
}

void APIENTRY aglWindowPos3dvMESA(void* cc,const GLdouble *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY aglWindowPos4iMESA(void* cc,GLint x, GLint y, GLint z, GLint w )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void APIENTRY aglWindowPos4sMESA(void* cc,GLshort x, GLshort y, GLshort z, GLshort w )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void APIENTRY aglWindowPos4dMESA(void* cc,GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
	aglWindowPos4fMESA(cc,(GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void APIENTRY aglWindowPos4ivMESA(void* cc,const GLint *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1],
							 (GLfloat) p[2], (GLfloat) p[3] );
}

void APIENTRY aglWindowPos4svMESA(void* cc,const GLshort *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1],
							 (GLfloat) p[2], (GLfloat) p[3] );
}

void APIENTRY aglWindowPos4fvMESA(void* cc,const GLfloat *p )
{
	aglWindowPos4fMESA(cc,p[0], p[1], p[2], p[3] );
}

void APIENTRY aglWindowPos4dvMESA(void* cc,const GLdouble *p )
{
	aglWindowPos4fMESA(cc,(GLfloat) p[0], (GLfloat) p[1],
							 (GLfloat) p[2], (GLfloat) p[3] );
}




/*
 * GL_MESA_resize_buffers
 */

/*
 * Called by user application when window has been resized.
 */
void APIENTRY aglResizeBuffersMESA(void* cc)
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.ResizeBuffersMESA)(CC );
}



/*
 * GL_SGIS_multitexture
 */

void APIENTRY aglMultiTexCoord1dSGIS(void *cc,GLenum target, GLdouble s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1dvSGIS(void *cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1fSGIS(void *cc,GLenum target, GLfloat s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1fvSGIS(void *cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1iSGIS(void *cc,GLenum target, GLint s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1ivSGIS(void *cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1sSGIS(void *cc,GLenum target, GLshort s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1svSGIS(void *cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2dSGIS(void *cc,GLenum target, GLdouble s, GLdouble t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2dvSGIS(void *cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2fSGIS(void *cc,GLenum target, GLfloat s, GLfloat t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2fvSGIS(void *cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2iSGIS(void *cc,GLenum target, GLint s, GLint t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2ivSGIS(void *cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2sSGIS(void *cc,GLenum target, GLshort s, GLshort t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2svSGIS(void *cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord3dSGIS(void *cc,GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3dvSGIS(void *cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3fSGIS(void *cc,GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3fvSGIS(void *cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3iSGIS(void *cc,GLenum target, GLint s, GLint t, GLint r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3ivSGIS(void *cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3sSGIS(void *cc,GLenum target, GLshort s, GLshort t, GLshort r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3svSGIS(void *cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord4dSGIS(void *cc,GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4dvSGIS(void *cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4fSGIS(void *cc,GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4fvSGIS(void *cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4iSGIS(void *cc,GLenum target, GLint s, GLint t, GLint r, GLint q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4ivSGIS(void *cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4sSGIS(void *cc,GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4svSGIS(void *cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}



void APIENTRY aglMultiTexCoordPointerSGIS(void *cc,GLenum target, GLint size, GLenum type,
													  GLsizei stride, const GLvoid *ptr)
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.MultiTexCoordPointer)(CC, target, size, type, stride, ptr);
}



void APIENTRY aglSelectTextureSGIS(void *cc,GLenum target)
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.SelectTextureSGIS)(CC, target);
}



void APIENTRY aglSelectTextureCoordSetSGIS(void *cc,GLenum target)
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.SelectTextureCoordSet)(CC, target);
}




/*
 * GL_EXT_multitexture
 */

void APIENTRY aglMultiTexCoord1dEXT(void* cc,GLenum target, GLdouble s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1dvEXT(void* cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1fEXT(void* cc,GLenum target, GLfloat s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1fvEXT(void* cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1iEXT(void* cc,GLenum target, GLint s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1ivEXT(void* cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1sEXT(void* cc,GLenum target, GLshort s)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord1svEXT(void* cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2dEXT(void* cc,GLenum target, GLdouble s, GLdouble t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2dvEXT(void* cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2fEXT(void* cc,GLenum target, GLfloat s, GLfloat t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2fvEXT(void* cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2iEXT(void* cc,GLenum target, GLint s, GLint t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2ivEXT(void* cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2sEXT(void* cc,GLenum target, GLshort s, GLshort t)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord2svEXT(void* cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY aglMultiTexCoord3dEXT(void* cc,GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3dvEXT(void* cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3fEXT(void* cc,GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3fvEXT(void* cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3iEXT(void* cc,GLenum target, GLint s, GLint t, GLint r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3ivEXT(void* cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord3sEXT(void* cc,GLenum target, GLshort s, GLshort t, GLshort r)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, 1.0 );
}

void APIENTRY aglMultiTexCoord3svEXT(void* cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY aglMultiTexCoord4dEXT(void* cc,GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4dvEXT(void* cc,GLenum target, const GLdouble *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4fEXT(void* cc,GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4fvEXT(void* cc,GLenum target, const GLfloat *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4iEXT(void* cc,GLenum target, GLint s, GLint t, GLint r, GLint q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4ivEXT(void* cc,GLenum target, const GLint *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY aglMultiTexCoord4sEXT(void* cc,GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, s, t, r, q );
}

void APIENTRY aglMultiTexCoord4svEXT(void* cc,GLenum target, const GLshort *v)
{
	GET_CONTEXT; CHECK_CONTEXT;
	(*CC->API.MultiTexCoord4f)(CC, target, v[0], v[1], v[2], v[3] );
}



void APIENTRY aglInterleavedTextureCoordSetsEXT(void* cc, GLint factor )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.InterleavedTextureCoordSets)(CC, factor );
}



void APIENTRY aglSelectTextureTransformEXT(void* cc,GLenum target )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.SelectTextureTransform)(CC, target );
}



void APIENTRY aglSelectTextureEXT(void* cc, GLenum target )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.SelectTexture)(CC, target );
}



void APIENTRY aglSelectTextureCoordSetEXT(void* cc,GLenum target )
{
	GET_CONTEXT; CHECK_CONTEXT;
 
	(*CC->API.SelectTextureCoordSet)(CC, target );
}

