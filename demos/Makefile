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

CC = gcc
CFLAGS = --specs=warpup -woslibs -g -O2 #-save-temps


##### MACROS #####

INCDIR = ..
LIBDIR = ../lib

GL_LIBS = -L$(LIBDIR) -laglut -laglu -lagl -lm $(XLIBS)

LIB_DEP = $(LIBDIR)/$(GL_LIB) $(LIBDIR)/$(GLU_LIB) $(LIBDIR)/$(GLUT_LIB)

PROGS = gears-minigl\
	bounce clearspd drawpix gamma gears glinfo glutfx isosurf\
	morph3d multiext multitex paltex pointblast reflect \
	renormal spectex stex3d tessdemo texcyl texobj trispd winpos
#	osdemo

##### RULES #####

.SUFFIXES:
.SUFFIXES: .c

.c: $(LIB_DEP)
	$(CC) -I$(INCDIR) $(CFLAGS) $< $(GL_LIBS) -o $@



##### TARGETS #####

#default:
#	@echo "Specify a target configuration"

targets: $(PROGS)

clean:
	-rm *.o *~

realclean:
	-rm $(PROGS)
	-rm *.o *~

# execute all programs
exec: $(PROGS)
	@for prog in $(PROGS) ;			\
	do					\
		echo -n "Running $$prog ..." ;	\
		$$prog ;			\
		echo ;				\
	done


#include ../Make-config

