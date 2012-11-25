######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 8037 2006-05-07 14:36:35Z ska $
##
######################################################################
SUBDIRS =
EXTRA_CLEAN=*.opt *.out *.bin *.def *.lst *.sym bitmaps.h *.lst *.rel *.sym *.map *.rst *.asm
OBJS    = main.o playfield.o utils.o sysdeps/sdl_rex_sysdeps.o
INCLUDES=-Isysdeps/include/
DEFINES =-DTARGET_SDL
LDLIBS  =-lSDL_ttf
TARGET  = tafl
VERSION = 1


%.z80.c: %.c
	cd `dirname $<` && ln -s `basename $<` $@

%.z80.o: %.z80.c
	sdcc --opt-code-size -mz80 -I/usr/local/share/sdcc/include/rex/ -Isysdeps/include/ -DTARGET_REX -c $<

tafl.sdcc.rex: tafl utils.z80.o main.z80.o playfield.z80.o sysdeps/rex_sysdeps.z80.o crt0.z80.o crt0.z80.lst
	link-r6k -f tafl.lnk
	rxbuild -c $(patsubst %.sdcc.rex,%.rel,$@) -o $@

crt0.z80.o: /usr/local/share/sdcc/lib/librex/crt0.o
	ln -s $< $@

crt0.z80.lst: /usr/local/share/sdcc/lib/librex/crt0.lst
	ln -s $< $@

include ~/scripts/build/Rules-sdl.mk

main.o: bitmaps.h

bitmaps.h: gfx/*.png
	conv2rex -n board_image gfx/board.png /dev/stdout > $@
	conv2rex -n board_corner_image gfx/board_corner.png /dev/stdout | cat >> $@
	conv2rex -n board_image_last_row gfx/board_last_row.png /dev/stdout | cat >> $@
	conv2rex -n board_corner_image_last_row gfx/board_corner_last_row.png /dev/stdout | cat >> $@
	conv2rex -n restart_button_image gfx/restart_button.png /dev/stdout | cat >> $@
	conv2rex -n black_button_image gfx/black_button.png /dev/stdout | cat >> $@
	conv2rex -n white_button_image gfx/white_button.png /dev/stdout | cat >> $@
	conv2rex -n human_button_image gfx/human_button.png /dev/stdout | cat >> $@
	conv2rex -n black_image gfx/black.png /dev/stdout | cat >> $@
	conv2rex -n white_image gfx/white.png /dev/stdout | cat >> $@
	conv2rex -n white_king_image gfx/white_king.png /dev/stdout | cat >> $@
#	conv2rex -n title_image gfx/title.png /dev/stdout | cat >> $@

tafl.rex: $(OBJS)
	zcc +rex -vn -create-app -D_NOFLOAT_ -DTARGET_REX $(INCLUDES) main.c playfield.c utils.c -o tafl.bin

install_rex: tafl.sdcc.rex
	wine rex2emu tafl.sdcc.rex addin1.out
	sudo cp addin1.out ~/emulator/windows/shared/emulator/rexdk/Emulator/addin1.OUT
	sync

rex_tafl-$(VERSION).tar.gz: clean
	tar --exclude="*.exe" --exclude="*.svn" --exclude="*~" --exclude="*.tar.gz" -cvzf $@ ../tafl
