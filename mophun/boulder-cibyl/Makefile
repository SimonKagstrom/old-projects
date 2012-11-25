######################################################################
##
## Copyright (C) 2004,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Makefile
##
## $Id: Makefile 1104 2005-01-16 05:59:19Z ska $
##
######################################################################
SUBDIRS   = ../lib tools
TARGET    = boulder.mpn
HEADERS   = res.h include/levels.h include/tiles.h \
	    include/boulder.h include/point.h include/dir.h include/types.h
OBJS      = res.o main.o elem.o sprite.o levels.o level_handling.o \
            callbacks.o status_field.o sound.o specials.o
INCLUDES  = -Iinclude -I. -I../lib/include
LDLIBS    = -lmenu -lutils
RESOURCES = gfx/sprites.bmp gfx/tiles.bmp $(wildcard levels/level.*.dat)
DATA_SIZE = 40960 # 40KB

TILE_IMGS = undiscovered.png empty.png dirt.png bomb_box.png magic_wall.png left_transport.png right_transport.png\
            iron_key.png gold_key.png red_key.png wooden_door.png iron_door.png red_door.png stone_wall.png \
	    weak_stone_wall.png teleporter_f0.png teleporter_f1.png exit_f0.png exit_f1.png \
            conveyor_left_f0.png conveyor_left_f1.png conveyor_f0.png conveyor_f1.png conveyor_right_f0.png \
            conveyor_right_f1.png
SPRITE_IMGS=player_up.png player_down.png player_left.png player_right.png boulder.png diamond1.png \
            diamond2.png diamond3.png bomb.png ghost.png flame.png flame2.png iron_key.png gold_key.png \
            red_key.png block.png lever_off.png lever_on.png

MONTAGEOPTS=-tile 1 -geometry +0+0
include ../Rules.mk

levels/level.%.dat: levels/level.%
	tools/level_packer/creator $<

gfx/basic_tiles.bmp: $(patsubst %.png,gfx/tiles/%.png,$(TILE_IMGS))
	montage -tile 1 -geometry 8x8 $+ $@

gfx/tiles.bmp: gfx/basic_tiles.bmp
	convert -modulate 50 $< b.bmp
	montage $(MONTAGEOPTS) gfx/basic_tiles.bmp b.bmp $@
	$(rm) -f b.bmp


gfx/sprites.bmp:  $(patsubst %.png,gfx/sprites/%.png,$(SPRITE_IMGS))
	montage -tile 1 -geometry 8x8 $+ a.bmp
	convert -modulate 50 a.bmp b.bmp
	montage -tile 1 -geometry 8 a.bmp b.bmp $@
	$(rm) -f a.bmp b.bmp
