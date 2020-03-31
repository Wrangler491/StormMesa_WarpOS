# $Id: Makefile,v 1.3 1998/06/17 01:12:48 brianp Exp $

# Makefile for GLUT
#
# NOTICE:  The OpenGL Utility Toolkit (GLUT) distribution contains source
# code published in a book titled "Programming OpenGL for the X Window
# System" (ISBN: 0-201-48359-9) published by Addison-Wesley.  The
# programs and associated files contained in the distribution were
# developed by Mark J. Kilgard and are Copyright 1994, 1995, 1996 by Mark
# J. Kilgard (unless otherwise noted).  The programs are not in the
# public domain, but they are freely distributable without licensing
# fees.  These programs are provided without guarantee or warrantee
# expressed or implied.
#
# GLUT source included with Mesa with permission from Mark Kilgard.


# $Log: Makefile,v $
# Revision 1.3  1998/06/17 01:12:48  brianp
# added glut_menu2.c to source list
#
# Revision 1.2  1998/06/14 15:12:48  brianp
# updated for GLUT 3.7
#
# Revision 1.1  1997/12/09 01:51:55  brianp
# Initial revision
#

CC = gcc
CFLAGS = --specs=warpup -woslibs -O0 -w #-save-temps

##### MACROS #####

GLUT_MAJOR = 3
GLUT_MINOR = 7

VPATH = RCS

INCDIR = ..
LIBDIR = ../lib

SOURCES = glutAddMenuEntry.c glutAddSubMenu.c glutAttachDetachMenu.c \
	glutChangeToMenuSubEntry.c glutCreateMenu.c glutCreateWindow.c \
	glutDestroyMenu.c glutDestroyWindow.c glutGameMode.c glutGet.c \
	glutGetModifiers.c glutGetSetColor.c glutGetSetDevice.c \
	glutGetSetMenu.c glutGetSetWindow.c glutIconifying.c glutInit.c \
	glutInitDisplayMode.c glutInitWindowPosition.c glutInitWindowSize.c \
	glutLayers.c glutMainLoop.c glutPostRedisplay.c glutRemoveMenuItem.c \
	glutReshapePositionWindow.c glutSwapBuffers.c glutTeapot.c glutVideo.c \
	glutXFunc.c glutstuff.c \
	\
	glutBitmap.c glutBitmapWidth.c glutExtensionSupported.c glutShapes.c \
	glutStroke.c glutStrokeWidth.c \
	glutBitmap8x13.c glutBitmap9x15.c glutBitmaphel10.c glutBitmaphel12.c \
	glutBitmaphel18.c glutBitmaptr10.c glutBitmaptr24.c glutStrokemroman.c \
	glutStrokeroman.c


OBJECTS = $(SOURCES:.c=.o)



##### RULES #####

.c.o:
	$(CC) -c -I. -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

#default:
#	@echo "Specify a target configuration"


targets: libaglut.a
#$(LIBDIR)/$(GLUT_LIB)

# Make the library
libaglut.a: $(OBJECTS)
	../mklib libaglut.a $^
	#$(MAKELIB) $(GLUT_LIB) $(GLUT_MAJOR) $(GLUT_MINOR) $(OBJECTS)
	mv libaglut.a $(LIBDIR)

#include ../Make-config

include depend

clean:
	-rm *.o


#
# Run 'make dep' to update the dependencies if you change what's included
# by any source file.
# 
dep: $(SOURCES)
	makedepend -fdepend -Y -I../include $(SOURCES)
