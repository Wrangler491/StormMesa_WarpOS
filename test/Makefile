# $Id: Makefile,v 3.3 1998/06/10 02:52:20 brianp Exp $

# Mesa 3-D graphics library
# Version:  3.0
# Copyright (C) 1995-1998  Brian Paul
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

# Makefile for GLUT-based demo programs for Unix/X11

# $Log: Makefile,v $
# Revision 3.3  1998/06/10 02:52:20  brianp
# added multiext example
#
# Revision 3.2  1998/04/21 23:58:41  brianp
# added LIBDIR variable
#
# Revision 3.1  1998/02/21 01:31:56  brianp
# added multitex and renormal demos
#
# Revision 3.0  1998/02/14 18:30:39  brianp
# initial rev
#

ROOT = ..

CC = gcc

CFLAGS = --specs=warpup -woslibs -DWARPUP -w -O0

##### MACROS #####

## include-directory (replace by ${srcdir}/include later)
INC_DIR = $(ROOT)
#$(ROOT)/include

## lib-directory (replace by ${srcdir}/lib later)
LIB_DIR = $(ROOT)/lib

## library dependencies
LIB_DEP = 

## no libraries to build
LIBS = -L$(LIB_DIR) -lagl -L../sources -L/gg/ppc-warpos/newlib/lib -L/gg/ppc-warpos/lib 

## no dirs to recurse
DIRS = glut mui

## sources to process
SRCS = test.c triangles.c trianglesDB.c trianglesS.c 
##accum.c

## programs to create
PRGS = test triangles trianglesDR trianglesS 
##accum

# MASTER-CONFIG #

#include $(ROOT)/Make-config.AmigaOS

##### RULES #####

## implicit rule to make object-files
#%.o: %.c
#	$(CC) -I$(INC_DIR) $(CFLAGS) $< -s -c -o $@

#$(PRGS):  
#	$(CC) -I. -I$(INC_DIR) $(CFLAGS) $< $(LIBS) -s -o $@

.SUFFIXES:
.SUFFIXES: .c

.c: $(LIB_DEP)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< $(LIBS) -o $@

targets: $(PRGS)



##accum: accum$(O) $(PRG_DEP)
##	$(CC) -I. -I$(INC_DIR) $(CFLAGS) $< -lMesaGLtk $(LIBS) -s -o $@
#triangles: triangles.o $(PRG_DEP)
#	$(CC) -I.. -I$(INC_DIR) $(CFLAGS) $< $(LIBS) -s -o $@
#trianglesDR: trianglesDR.o $(PRG_DEP)
#	$(CC) -I.. -I$(INC_DIR) $(CFLAGS) $< $(LIBS) -s -o $@
##trianglesDB: trianglesDB$(O) $(PRG_DEP)
##	$(CC) -I. -I$(INC_DIR) $(CFLAGS) $< $(LIBS) -s -o $@
#trianglesS: trianglesS.o $(PRG_DEP)
#	$(CC) -I.. -I$(INC_DIR) $(CFLAGS) $< $(LIBS) -s -o $@
