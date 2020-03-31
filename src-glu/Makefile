# $Id: Makefile,v 1.3 1997/10/21 23:58:26 brianp Exp $

# Mesa 3-D graphics library
# Version:  2.5
# Copyright (C) 1995-1997  Brian Paul
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# $Log: Makefile,v $
# Revision 1.3  1997/10/21 23:58:26  brianp
# now pass version numbers to library maker
#
# Revision 1.2  1997/08/13 01:26:22  brianp
# updated for 2.4
#
# Revision 1.1  1996/09/27 01:22:36  brianp
# Initial revision
#

CC = gcc
CFLAGS = --specs=warpup -woslibs -O3 -w #-save-temps


##### MACROS #####

VPATH = RCS

INCDIR = ..
LIBDIR = ../lib

SOURCES = glu.c mipmap.c nurbs.c nurbscrv.c nurbssrf.c nurbsutl.c	\
	project.c quadric.c tess.c tess_clip.c tess_fist.c	\
	tess_hash.c tess_heap.c tess_winding.c

#glu.c mipmap.c nurbs.c nurbscrv.c nurbssrf.c nurbsutl.c \
#	project.c quadric.c tess.c tesselat.c polytest.c

OBJECTS = $(SOURCES:.c=.o)



##### RULES #####

.c.o:
	$(CC) -c -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

#default:
#	@echo "Specify a target configuration"

targets: libaglu.a
#$(LIBDIR)/$(GLU_LIB)

# Make the library:
libaglu.a: $(OBJECTS)
	../mklib libaglu.a $^
	#$(MAKELIB) $(GLU_LIB) $(MAJOR) $(MINOR) $(OBJECTS)
	mv libaglu.a $(LIBDIR)

#include ../Make-config

include depend

clean:
	-rm *.o 



#
# Run 'make depend' to update the dependencies if you change what's included
# by any source file.
# 
dep: $(SOURCES)
	makedepend -fdepend -Y -I. $(SOURCES)
