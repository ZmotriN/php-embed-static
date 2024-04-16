@echo off

call configure --disable-all --enable-embed=static --with-win32std

type Makefile Makefile-static > Makefile2
del Makefile
rename Makefile2 Makefile

nmake
nmake static > NUL

DEVENV %~dp0embeder\embeder.sln /rebuild "Release console|x64"