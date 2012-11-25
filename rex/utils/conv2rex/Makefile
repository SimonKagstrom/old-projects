######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 6264 2006-01-21 09:46:06Z ska $
##
######################################################################
SUBDIRS =
OBJS    = conv2rex.o
INCLUDES=-Isysdeps/include/
LDLIBS  =-lSDL_image
CFLAGS  =-D_GNU_SOURCE
TARGET  = conv2rex

include ~/scripts/build/Rules-sdl.mk

install: $(TARGET)
	cp $< ~/bin/
