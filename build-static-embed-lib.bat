@echo off

call configure --disable-all --enable-embed=static

type Makefile Makefile-static > Makefile2
del Makefile
rename Makefile2 Makefile

nmake
nmake static > NUL

DEVENV %~dp0embeder\embeder.sln /rebuild "Release console|x64"