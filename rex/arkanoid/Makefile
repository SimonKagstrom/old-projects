# Makefile for the arkanoid project.

# debugging options
USE_PROFILING 	= y
USE_GPROF     	= n # depends on USE_PROFILING

cc            	= gcc
ld            	= gcc
zcc             = zcc

INCLUDES        = -Isysdeps/include/

# SDL config.
SDL_CFLAGS   	:= $(shell sdl-config --cflags)
SDL_LDFLAGS  	:= $(shell sdl-config --libs)

CFLAGS        	= -ansi -Wall $(SDL_CFLAGS) -DTARGET_SDL $(INCLUDES)
LDFLAGS         = $(SDL_LDFLAGS)
LDLIBS  	= -lSDL_ttf

# For REX6000 binary generation
REX_CFLAGS      = +rex -vn -O3 -create-app -D_NOFLOAT_ -DTARGET_REX $(INCLUDES) $(DEFINES) -I$(Z80_OZFILES)../include

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

TARGETS       	= arkanoid.SDL
REX_TARGETS     = arkanoid.bin

OBJECTS 	= arkanoid.o draw.o sysdeps/sdl_rex_sysdeps.o

all:	$(TARGETS) \
	$(REX_TARGETS)

.c.o: %.o:%.c
	$(cc) -c $(CFLAGS) $<

$(OBJECTS):	%.o:%.c
	$(cc) $(CFLAGS) -c $< -o $@

$(TARGETS): $(OBJECTS)
	$(ld) $(LDFLAGS) $(LDLIBS) -o $@ $+

$(REX_TARGETS): arkanoid.c draw.c sysdeps/rex_sysdeps.c
	$(zcc) $(REX_CFLAGS) -o $@ $+

.SUFFIXES: .cpp .cxx .cc .C .c

clean:
	find . -name "*~"  | xargs rm -f
	find . -name "*.o" | xargs rm -f
	rm -f $(TARGETS) $(REX_TARGETS)
	rm -f core *.gcov *.da *.bb *.bbg bb.out gmon.out zcc_opt.def
