######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 16910 2007-12-07 06:54:12Z ska $
##
######################################################################
SUBDIRS =
EXTRA_CLEAN=*.opt *.out *.bin *.def *.lst *.sym bitmaps.h *.lst *.rel *.sym *.map *.rst *.asm sysdeps/*.o
OBJS    = game.o playfield.o battle.o point.o sysdeps/sdl_rex_sysdeps.o
INCLUDES=-Isysdeps/include/ -Iinclude
DEFINES =-DTARGET_SDL
LDLIBS  =-lSDL_ttf
TARGET  = archon
VERSION = 1


%.z80.c: %.c
	cd `dirname $<` && ln -s `basename $<` $@

%.z80.o: %.z80.c
	sdcc --opt-code-size -mz80 -I/usr/local/share/sdcc/include/rex/ -Iinclude -Isysdeps/include/ -DTARGET_REX -c $<

archon.sdcc.rex: archon game.z80.o playfield.z80.o sysdeps/rex_sysdeps.z80.o crt0.z80.o crt0.z80.lst
	link-r6k -f archon.lnk
	rxbuild -c $(patsubst %.sdcc.rex,%.rel,$@) -o $@

crt0.z80.o: /usr/local/share/sdcc/lib/librex/crt0.o
	ln -s $< $@

crt0.z80.lst: /usr/local/share/sdcc/lib/librex/crt0.lst
	ln -s $< $@

include ~/scripts/build/Rules-sdl.mk

game.o: bitmaps.h

bitmaps.h: gfx/*.png
	conv2rex -u -n image_dark gfx/dark.png /dev/stdout > $@
	conv2rex -u -n image_light gfx/light.png /dev/stdout | cat >> $@
	conv2rex -u -n image_magenta gfx/magenta.png /dev/stdout | cat >> $@
	conv2rex -u -n image_powerpoint gfx/powerpoint.png /dev/stdout | cat >> $@
	conv2rex -u -n image_valkyrie gfx/valkyrie.png /dev/stdout | cat >> $@
	conv2rex -u -n image_golem gfx/golem.png /dev/stdout | cat >> $@
	conv2rex -u -n image_unicorn gfx/unicorn.png /dev/stdout | cat >> $@
	conv2rex -u -n image_dinji gfx/dinji.png /dev/stdout | cat >> $@
	conv2rex -u -n image_wizard gfx/wizard.png /dev/stdout | cat >> $@
	conv2rex -u -n image_phoenix gfx/phoenix.png /dev/stdout | cat >> $@
	conv2rex -u -n image_archer gfx/archer.png /dev/stdout | cat >> $@
	conv2rex -u -n image_knight gfx/knight.png /dev/stdout | cat >> $@
	conv2rex -u -n image_banshee gfx/banshee.png /dev/stdout | cat >> $@
	conv2rex -u -n image_troll gfx/troll.png /dev/stdout | cat >> $@
	conv2rex -u -n image_basilisk gfx/basilisk.png /dev/stdout | cat >> $@
	conv2rex -u -n image_shapeshifter gfx/shapeshifter.png /dev/stdout | cat >> $@
	conv2rex -u -n image_sorceress gfx/sorceress.png /dev/stdout | cat >> $@
	conv2rex -u -n image_dragon gfx/dragon.png /dev/stdout | cat >> $@
	conv2rex -u -n image_manticore gfx/manticore.png /dev/stdout | cat >> $@
	conv2rex -u -n image_goblin gfx/goblin.png /dev/stdout | cat >> $@
#	conv2rex -n title_image gfx/title.png /dev/stdout | cat >> $@

install_rex: archon.sdcc.rex
	wine rex2emu archon.sdcc.rex addin1.out
	sudo cp addin1.out ~/emulator/windows/shared/emulator/rexdk/Emulator/addin1.OUT
	sync

rex_archon-$(VERSION).tar.gz: clean
	tar --exclude="*.exe" --exclude="*.svn" --exclude="*~" --exclude="*.tar.gz" -cvzf $@ ../archon
