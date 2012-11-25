######################################################################
##
## Copyright (C) 2004,  Blekinge Institute of Technology
##
## Filename:      Rules.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   SDL Makefile rules. Requires GNU make.
##
## $Id: Rules_SDL.mk,v 1.1 2004/06/24 14:58:54 ska Exp $
##
######################################################################

# --- Variables, these can be owerridden. ---
# programs
cc        ?= gcc
CC        ?= g++
ld        ?= gcc
sed       ?= sed
tags	  ?= ctags
make      ?= make
rm        ?= rm

# Options
LDLIBS    ?=
INCLUDES  ?=
DEFINES   ?=-DTARGET_ANSIC

CFLAGS    ?=-Wall -g -std=c99
LDFLAGS   ?=

TAGSOPTS  ?= -e -R

# Rules
all: $(TARGET) $(SUBDIRS)

%.o: %.c
	$(cc) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $<

%.o: %.cc
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $<

# Targets
$(TARGET): $(OBJS)
	$(ld) $(LDFLAGS) -o $@ $+ $(LDLIBS)

# Rule for making subdirectories
$(SUBDIRS): FORCE
	$(make) -C $@


tags: FORCE
	$(tags) $(TAGSOPTS) $(SUBDIRS)

clean:
	for dir in $(SUBDIRS); do \
		$(make) clean -C $$dir; \
        done
	$(rm) -f *~
	$(rm) -f *.o
	rm -f $(TARGET)
	rm -f TAGS

# Dummy rule to force some operations
FORCE:
