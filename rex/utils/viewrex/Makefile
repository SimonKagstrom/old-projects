# Makefile for the viewrex project.

# debugging options
USE_PROFILING 	= y
USE_GPROF     	= n # depends on USE_PROFILING

cc            	= gcc
ld            	= gcc
zcc             = zcc

# SDL config.
SDL_CFLAGS   	:= $(shell sdl-config --cflags)
SDL_LDFLAGS  	:= $(shell sdl-config --libs)

CFLAGS        	= -Wall $(SDL_CFLAGS) $(INCDIR) -DTARGET_SDL
LDFLAGS      	= $(SDL_LDFLAGS)
LDLIBS  	= -lSDL_image

ifeq ($(USE_PROFILING), y)
 CFLAGS  	+= -g -pedantic -DUSE_PROFILING
 LDFLAGS 	+=
ifeq ($(USE_GPROF), y) # this is VERY slow. Use with care
 CFLAGS  	+= -ftest-coverage -fprofile-arcs -pg -a 
 LDFLAGS 	+= -pg -a
endif # USE_GPROF
else
 CFLAGS  	+= -O6 -DNDEBUG $(INCDIR)
endif # USE_PROFILING

TARGETS       	= viewrex

OBJECTS 	= 

all:	$(TARGETS)

$(TARGETS): %:%.o $(OBJECTS)
	$(ld) $(LDFLAGS) -o $@ $<  $(OBJECTS) $(LDLIBS)

.SUFFIXES: .cpp .cxx .cc .C .c

.c.o: %.o:%.c
	$(cc) -c $(CFLAGS) $<

$(OBJECTS):	%.o:%.c
	$(cc) $(CFLAGS) -c $< 	

clean:
	@rm -f *.o $(TARGETS) $(REX_TARGETS) \
	@rm -f *~ core *.gcov *.da *.bb *.bbg *.log *.aux bb.out gmon.out