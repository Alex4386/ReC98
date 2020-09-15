@echo off
echo Running the first, 32-bit part of the ReC98 build process.

tasm32 1>NUL 2>NUL
if errorlevel 9009 goto no_tasm32

: Neither BCC32 nor TASM32 automatically create nonexisting output
: directories. Tup would, but not everybody can use it.
for /L %%i in (1,1,5) do mkdir bin\th0%%i 2>NUL

tup

goto eof

:no_tasm32
echo Could not find TASM32.
echo Please make sure that the BIN directory of Turbo Assembler 5.0 is in your PATH.
goto eof

:eof
echo -------------------------------------------------------------------------------
