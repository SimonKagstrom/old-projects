@echo off

rem I hope this works - it is untested, but I think it shuold be fairly
rem straightforward :-). It is based on Daverex' make.bat file for the
rem WriteMemo project which in turn is based on Daniels make.bat-file.

zcc +rex -vn -create-app -o writememo.bin src\writememo.c src\read.c
zcc +rex -vn -create-app -o verifymemo.bin src\verifymemo.c src\read.c
