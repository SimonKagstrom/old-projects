# Makefile for the writememo project

# Cross-compile for windows. This name is kind of debian-specific. It is
# often called i386-mingw32msvc-gcc in other systems
wincc      = i586-mingw32msvc-gcc
cc         = gcc
ccopts     = -Wall -pedantic -ansi
ccunixopts = -g
ccwinopts  =

zcc        = zcc
zccopts    = +rex -vn -create-app

rm         = rm
rmopts     = -f


all: writememo.rex  \
     verifymemo.rex \
     generator      \
     generator.exe


writememo.rex: src/writememo.c src/read.h src/read.c src/file.h
	$(zcc) $(zccopts) -o writememo.bin src/writememo.c src/read.c

verifymemo.rex: src/verifymemo.c src/read.h src/read.c src/file.h
	$(zcc) $(zccopts) -o verifymemo.bin src/verifymemo.c src/read.c

generator: src/generator.c
	$(cc) $(ccopts) $(ccunixopts) -o $@ $+

generator.exe: src/generator.c
	$(wincc) $(ccopts) $(ccwinopts) -o $@ $+


clean:
	$(rm) $(rmopts) *.rex *.bin *~ *.def generator generator.exe src/*~