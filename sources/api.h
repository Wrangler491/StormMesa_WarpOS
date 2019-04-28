/* Thellier should not put the define here but in the makefile */
#define AMIGA 1
#define NOASM_68K 1
#define NOASM_PPC 1
#define DRIVEREXT 1


/* $Id: api.h,v 3.3 1998/02/20 04:48:57 brianp Exp $ */

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
 * $Log: api.h,v $
 * Revision 3.3  1998/02/20 04:48:57  brianp
 * updated comments
 *
 * Revision 3.2  1998/02/04 00:44:02  brianp
 * WIN32 patch from Oleg Letsinsky
 *
 * Revision 3.1  1998/02/04 00:14:18  brianp
 * updated for Cygnus (Stephane Rehel)
 *
 * Revision 3.0  1998/01/31 20:41:55  brianp
 * initial rev
 *
 */


/*
 * This header contains stuff only included by api1.c, api2.c and apiext.c
 */


#ifndef API_H
#define API_H


/*
 * Single/multiple thread context selection.
 */
#ifdef THREADS
/* Get the context associated with the calling thread */
#define GET_CONTEXT     GLcontext *CC = gl_get_thread_context()
#else
/* CC is a global pointer for all threads in the address space */

#ifdef WARPUP
void LibPrintf(const char *string, ...);
#define GET_CONTEXT
#else
#ifdef AMIGA

void LibPrintf(const char *string, ...);    
#define GET_CONTEXT GLcontext *CC=cc;  /*  LibPrintf("GET_CONTEXT: CC %ld \n",CC); */


#else
#define GET_CONTEXT
#endif
#endif	//WARPUP
#endif /* THREADS */

#ifndef AMIGA
#define CHECK_CONTEXT if(!CC){return;}
#define CHECK_CONTEXT_RETURN(R) if(!CC){return(R);}
#else
#define CHECK_CONTEXT if(!CC) {/*LibPrintf*/ printf("GLcontext *CC is lost !!!\n"); return;}
#define CHECK_CONTEXT_RETURN(R) if(!CC){/*LibPrintf*/ printf("The GLcontext CC is lost !!!!!!\n");return(R);}
#endif

/*
 * An optimization in a few performance-critical functions.
 */
#define SHORTCUT


/*
 * Windows 95/NT DLL stuff.
 */
#if !defined(WIN32) && !defined(WINDOWS_NT) && !defined(__CYGWIN32__)
#ifndef APIENTRY
#define APIENTRY
#endif
#endif


#endif
