@echo off

call configure --disable-all --enable-cli --enable-embed=static

type Makefile Makefile-static > Makefile2
del Makefile
rename Makefile2 Makefile

nmake
nmake static > NUL