# Makefile for the advlib project.

# debugging options
USE_DEBUGGING	= y
USE_GPROF	= n # depends on USE_PROFILING
REX_COMPILER    = zcc

ADVG_SIG        = ADV
EMULATOR_PATH   = /home/simon/emulator/rexemu

# gcc stuff
cc	        = gcc
ld	        = gcc

# zcc stuff
zcc             = zcc
mklib           = mklib
mkemu           = mkemu

# sdcc stuff
sdcc            = sdcc
linkrx          = link-rx
rxbuild         = rxbuild

# tools
conv2rex        = conv2rex
cat             = cat
echo            = echo
cd              = cd
rm              = rm
cp              = cp
mv              = mv

# tool options
RMOPTS          = -f

# SDL config.
SDL_CFLAGS	:= $(shell sdl-config --cflags) -DADVG_DEBUG
SDL_LDFLAGS	:= $(shell sdl-config --libs)

INCLUDES        = -I. -Iinclude/ -Iadvglib/include/ -Iadvlib/include
DEFINES         =
CFLAGS	        = -ansi -Wall $(SDL_CFLAGS) -DTARGET_SDL $(INCLUDES) $(DEFINES)
LDFLAGS         = $(SDL_LDFLAGS)
LDLIBS          = -lSDL_ttf

SDCC_CFLAGS     += -mz80 -DTARGET_REX -I/home/simon/sdcc/rxsdk/include $(INCLUDES) $(DEFINES)
SDCC_LIB_CFLAGS += -mz80
SDCC_LDFLAGS    += -- -mux -z -k /home/simon/sdcc/rxsdk/lib -l z80 -l librex -b _CODE=0x8000 -b _DATA=0xf000
SDCC_ADDINFLAGS  = -c

ZCC_CFLAGS      = +rex -O1 -vn -create-app -D_NOFLOAT_ -DTARGET_REX $(INCLUDES) $(DEFINES)
ZCCLIB_CFLAGS   += +rexlib -O1 -vn -create-app -D_NOFLOAT_ -DTARGET_REX $(INCLUDES) $(DEFINES)


# For REX6000 binary generation
REX_CFLAGS      = -DTARGET_REX $(INCLUDES)
REXLIB_CFLAGS   = -DTARGET_REX $(INCLUDES)

ifeq ($(USE_DEBUGGING), y)
 CFLAGS	        += -g -pedantic -DUSE_PROFILING
 LDFLAGS	+=
ifeq ($(USE_GPROF), y) # this is VERY slow. Use with care
 CFLAGS	        += -ftest-coverage -fprofile-arcs -pg -a
 LDFLAGS	+= -pg -a
endif # USE_GPROF
else
 CFLAGS         += -O6 -DNDEBUG $(INCLUDES)
endif # USE_DEBUGGING


# Targets to build
TARGETS	     = examples/example_game/example_game     \
               examples/null_game/null_game           \
               examples/advlib_test/advlib_test       \
               examples/sysdeps_test/sysdeps_test
REX_TARGETS  = examples/example_game/example_game.bin \
               examples/null_game/null_game.bin       \
               examples/sysdeps_test/sysdeps_test.bin \
               examples/advlib_test/advlib_test.bin   \
               advglib/lib/advglib.bin

IMAGES       = mountain_mleft16x16.png road_htov_right16x16.png coins.png          \
               mountain_mmiddle16x16.png road_left16x16.png                        \
               forest16x16.png mountain_mright16x16.png road_middle16x16.png       \
               fruit.png mountain_uleft16x16.png road_right16x16.png               \
               grass16x16.png mountain_umiddle16x16.png road_vtoh_down16x16.png    \
               house_left16x16.png mountain_uright16x16.png road_vtoh_up16x16.png  \
               house_right16x16.png player.png rock1_16x16.png house_up_left16x16.png \
               player_mask.png rope.png house_up_right16x16.png road_bendright_up16x16.png \
               tree16x16.png mountain_dleft16x16.png road_horiz_down16x16.png      \
               troll8x8.png mountain_dmiddle16x16.png road_horiz_up16x16.png       \
               mountain_dright16x16.png road_htov_left16x16.png floor_dright16x16.png \
               floor_dleft16x16.png floor_dmid16x16.png floor_umid16x16.png floor_uleft16x16.png \
               floor_uright16x16.png floor_mmid16x16.png floor_mleft16x16.png floor_mright16x16.png


# All that's built
all: examples/example_game/bitmaps.h $(TARGETS) $(REX_TARGETS)

# Generic rule to make the example game bitmaps
examples/example_game/bitmaps.h: $(patsubst %.png, examples/example_game/data/%.png, $(IMAGES)) Makefile
	$(cat) buildsystem/bitmap_hdr.h > $@
	$(echo) "#define NR_TILES          37 /* The number of tiles*/" >> $@
	$(echo) "#define NR_OBJECTS         5 /* The total number of objects */" >> $@
	$(echo) "const uint8_t p_background_data[] =" >> $@
	$(echo) "{" >> $@

	$(echo) "  /* Grass (A) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/grass16x16.png >> $@
	$(echo) "  /* Road left (B) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_left16x16.png >> $@
	$(echo) "  /* Road middle (C) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_middle16x16.png >> $@
	$(echo) "  /* Road right (D) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_right16x16.png >> $@
	$(echo) "  /* Horizontal road up (E) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_horiz_up16x16.png >> $@
	$(echo) "  /* Horizontal road up (F) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_horiz_down16x16.png >> $@
	$(echo) "  /* Road vertical to horisontal up (G) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_vtoh_up16x16.png >> $@
	$(echo) "  /* Road vertical to horisontal down (H) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_vtoh_down16x16.png >> $@
	$(echo) "  /* Road horisontal to vertical left (I) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_htov_left16x16.png >> $@
	$(echo) "  /* Road horisontal to vertical right (J) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/road_htov_right16x16.png >> $@
	$(echo) "  /* House floor mid mid (K) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_mmid16x16.png >> $@
	$(echo) "  /* Rock (L) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/rock1_16x16.png >> $@
	$(echo) "  /* Tree (M) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/tree16x16.png >> $@
	$(echo) "  /* House down left (N) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/house_left16x16.png >> $@
	$(echo) "  /* House down right (O) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/house_right16x16.png >> $@
	$(echo) "  /* House up left (P) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/house_up_left16x16.png >> $@
	$(echo) "  /* House up right (Q) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/house_up_right16x16.png >> $@
	$(echo) "  /* Mountain up left (R) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_uleft16x16.png >> $@
	$(echo) "  /* Mountain up middle (S) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_umiddle16x16.png >> $@
	$(echo) "  /* Mountain up right (T) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_uright16x16.png >> $@
	$(echo) "  /* Mountain middle left (U) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_mleft16x16.png >> $@
	$(echo) "  /* Mountain middle middle (V) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_mmiddle16x16.png >> $@
	$(echo) "  /* Mountain middle right (W) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_mright16x16.png >> $@
	$(echo) "  /* Mountain down left (X) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_dleft16x16.png >> $@
	$(echo) "  /* Mountain down middle (Y) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_dmiddle16x16.png >> $@
	$(echo) "  /* Mountain down right (Z) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/mountain_dright16x16.png >> $@
	$(echo) "  /* House floor up left ([) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_uleft16x16.png >> $@
	$(echo) "  /* House floor up mid (\) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_umid16x16.png >> $@
	$(echo) "  /* House floor up right (]) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_uright16x16.png >> $@
	$(echo) "  /* House floor mid left (^) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_mleft16x16.png >> $@
	$(echo) "  /* House floor mid right (_) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_mright16x16.png >> $@
	$(echo) "  /* House floor down left (\`) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_dleft16x16.png >> $@
	$(echo) "  /* House floor down mid (a) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_dmid16x16.png >> $@
	$(echo) "  /* House floor down right (b) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/floor_dright16x16.png >> $@
	$(echo) "  /* Table (c) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/table16x16.png >> $@
	$(echo) "  /* Vertical able (d) */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/vtable16x16.png >> $@
	$(echo) "};" >> $@

	$(echo) "" >> $@
	$(conv2rex) -u -n p_player_bitmap -c examples/example_game/data/player.png >> $@
	$(echo) "" >> $@
	$(conv2rex) -u -n p_player_mask_bitmap -c examples/example_game/data/player_mask.png >> $@
	$(echo) "" >> $@

	$(conv2rex) -u -n p_troll_bitmap -c examples/example_game/data/troll8x8.png >> $@
	$(echo) "" >> $@


	$(echo) "uint8_t p_objects_bitmap[] =" >> $@
	$(echo) "{" >> $@
	$(echo) "  /* Rope */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/rope.png >> $@
	$(echo) "  /* Fruit */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/fruit.png >> $@
	$(echo) "  /* Coins */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/coins.png >> $@
	$(echo) "  /* Bottle */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/bottle.png >> $@
	$(echo) "  /* Shining coins */" >> $@
	$(conv2rex) -c -d -w examples/example_game/data/shining_coins.png >> $@
	$(echo) "};" >> $@
	$(cat) buildsystem/bitmap_tail.h >> $@


# SDL targets
examples/sysdeps_test/sysdeps_test: sysdeps/sdl_rex_sysdeps.o examples/sysdeps_test/sysdeps_test.o
	$(ld) $(LDFLAGS) $(LDLIBS) -o $@ $+

examples/advlib_test/advlib_test: sysdeps/sdl_rex_sysdeps.o advlib/advlib.o examples/advlib_test/advlib_test.o
	$(ld) $(LDFLAGS) $(LDLIBS) -o $@ $+

examples/null_game/null_game: sysdeps/sdl_rex_sysdeps.o advlib/advlib.o advglib/advgame.o advglib/game_loop.o \
                              examples/null_game/null_game.o
	$(ld) $(LDFLAGS) $(LDLIBS) -o $@ $+

examples/example_game/example_game: sysdeps/sdl_rex_sysdeps.o advlib/advlib.o advglib/advgame.o advglib/game_loop.o \
                                 examples/example_game/example_game.o
	$(ld) $(LDFLAGS) $(LDLIBS) -o $@ $+



# REX targets (zcc)
advglib/lib/advglib.bin: sysdeps/rex_sysdeps.c advlib/advlib.c advglib/advg_rex_lib.c advglib/advgame.c
	$(zcc) $(ZCCLIB_CFLAGS) -o $@ sysdeps/rex_sysdeps.c advlib/advlib.c advglib/advgame.c advglib/advg_rex_lib.c
	$(mklib) $@ $(ADVG_SIG)
	$(mkemu) advglib/lib/advglib.b2 $(EMULATOR_PATH)/ 4
	$(mv) -f $(EMULATOR_PATH)/rom1.d2 $(EMULATOR_PATH)/rom1.dat

examples/sysdeps_test/sysdeps_test.bin: sysdeps/rex_sysdeps.c examples/sysdeps_test/sysdeps_test.c
	$(zcc) $(ZCC_CFLAGS) -o $@ sysdeps/rex_sysdeps.c examples/sysdeps_test/sysdeps_test.c

examples/advlib_test/advlib_test.bin: sysdeps/rex_sysdeps.c advglib/advg_client_lib.c examples/advlib_test/advlib_test.c
	$(zcc) $(ZCC_CFLAGS) -o $@ -DADVLIB_ONLY sysdeps/rex_sysdeps.c advglib/advg_client_lib.c examples/advlib_test/advlib_test.c

examples/null_game/null_game.bin: sysdeps/rex_sysdeps.c advglib/advg_client_lib.c \
                                  advglib/game_loop.c examples/null_game/null_game.c
	$(zcc) $(ZCC_CFLAGS) -o $@ sysdeps/rex_sysdeps.c advglib/advg_client_lib.c \
                advglib/game_loop.c examples/null_game/null_game.c
# The null_game, linked directly against the library.
examples/null_game/null_game_nolib.bin: sysdeps/rex_sysdeps.c advlib/advlib.c advglib/advgame.c advglib/game_loop.c \
				        examples/null_game/null_game.c
	$(zcc) $(ZCC_CFLAGS) -o $@ sysdeps/rex_sysdeps.c advglib/advgame.c advlib/advlib.c advglib/game_loop.c \
				   examples/null_game/null_game.c

examples/example_game/example_game.bin: sysdeps/rex_sysdeps.c advglib/advg_client_lib.c advglib/game_loop.c \
                                        examples/example_game/example_game.c examples/example_game/bitmaps.h
	$(zcc) $(ZCC_CFLAGS) -o $@ sysdeps/rex_sysdeps.c advglib/advg_client_lib.c \
               advglib/game_loop.c examples/example_game/example_game.c



# REX targets (sdcc)
examples/sysdeps_test/sysdeps_test.rex: sysdeps/rex_sysdeps.c examples/sysdeps_test/sysdeps_test.c
	$(sdcc) $(SDCC_CFLAGS) -c sysdeps/rex_sysdeps.c
	$(sdcc) $(SDCC_CFLAGS) -c examples/sysdeps_test/sysdeps_test.c
	cd examples/sysdeps_test/ && $(linkrx) -f sysdeps_test.lrx
	$(rxbuild) $(SDCC_ADDINFLAGS) examples/sysdeps_test/sysdeps_test.rel -o $@



# Rule for building objects
.c.o: %.o:%.c
	$(cc) $(CFLAGS) -c -o $@ $<

# Rule for building REX-objects (non-working)
%.Z80.o: %.c
	$(sdcc) $(SDCC_CFLAGS) -c $<


# Dummy rule to always rebuild the TAGS
force:

# Make etags for The One True Editor (no, not vi!)
TAGS: force
	find . -name "*.[chCH]" -print | etags -

# Cleanup
RM_FILES = *~ core *.o *.gcov *.da *.bb *.bbg *.log *.aux *.rex bb.out gmon.out zcc_opt.def *.map *.bin *.rst *.lst *.opt *.sym *.rel *.asm *.map *.b2

clean:
	@$(cd) examples/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) examples/sysdeps_test/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) examples/example_game/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) examples/null_game/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) examples/advlib_test/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) sysdeps/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) advlib/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) advglib/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(cd) advglib/lib/ && $(rm) $(RMOPTS) $(RM_FILES)
	@$(rm) $(RMOPTS) $(RM_FILES) $(TARGETS) $(REX_TARGETS)
