######################################################################
##
## Copyright (C) 2004,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Makefile
##
## $Id: Makefile 2174 2005-03-18 07:00:30Z ska $
##
######################################################################
SUBDIRS   = tools ../lib
TARGET    = brucelee.mpn
HEADERS   = res.h include/game.h include/point.h include/rect.h
OBJS      = res.o main.o levels.o anims.o level_handling.o
INCLUDES  = -Iinclude -I. -I../lib/include
LDLIBS    = -lmenu -lutils
LEVELS    = levels/level_0.h levels/level_1.h
RESOURCES = gfx/tiles.bmp gfx/sprites.bmp $(LEVELS) levels/tile_bounds.h
DATA_SIZE = 40960 # 40KB

TILE_IMGS = gfx/levels/screen_0.xcf gfx/levels/screen_1.xcf
SPRITE_IMGS=idle_left.png hit_left_f0.png crouch_left_f0.png running_left_f0.png \
            running_left_f1.png jump_left_f0.png jump_left_f1.png kick_left_f0.png kick_left_f1.png \
            jump_up_f0.png jump_up_f1.png climb_f0.png climb_f1.png \
            idle_right.png hit_right_f0.png crouch_right_f0.png running_right_f0.png \
            running_right_f1.png jump_right_f0.png jump_right_f1.png kick_right_f0.png kick_right_f1.png \
            jump_up_f0.png jump_up_f1.png climb_f0.png climb_f1.png

MONTAGEOPTS=-tile 1 -geometry +0+0
include ../Rules.mk


gfx/sprites.bmp:  $(patsubst %.png,gfx/sprites/%.png,$(SPRITE_IMGS))
	montage -tile 1 -geometry 30x24 $+  -bordercolor black $@

gfx/tiles.bmp: $(TILE_IMGS)
	cd gfx/levels/ && ../../tools/create_level_tiles.sh $(patsubst gfx/levels/%,%,$+)
	cp gfx/levels/level_tiles.bmp gfx/tiles.bmp

levels/tile_bounds.h: $(TILE_IMGS)
	echo "rect_t tile_bounds[] = {" > $@
	tools/imgbound/imgbound -g 8 8 gfx/levels/mask_tiles.bmp >> $@
	echo "};" >> $@

gfx/bruce_bounding.h: gfx/sprites.bmp
	echo "rect_t bruce_bounding[] = {" > $@
	tools/imgbound/imgbound -g 30 24 $< >> $@
	echo "};" >> $@

levels/level_%.h: gfx/levels/level_tiles.bmp gfx/levels/mask_tiles.bmp gfx/levels/event_tiles.bmp gfx/levels/screen_%.xcf
	./tools/create_level_map.sh $(patsubst levels/%.h,%,$@) $+ > $@
