@echo off

call configure --disable-all --enable-cli --enable-embed=static --with-winbinder --with-win32std

type Makefile Makefile-static > Makefile2
del Makefile
rename Makefile2 Makefile

nmake
nmake static > NUL