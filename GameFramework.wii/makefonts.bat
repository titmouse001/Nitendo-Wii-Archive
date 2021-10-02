@echo off

set fonttool=fonttool.exe
set fontdir=fonts

REM ensure the folder exists
mkdir %fontdir%
mkdir %fontdir%\small
mkdir %fontdir%\medium
mkdir %fontdir%\large

REM and delete any old files
del %fontdir%\*.tga /s
del %fontdir%\*.ftab /s
del %fontdir%\*.fraw /s

Rem only need the 'basic Latin' ASCII set 
set CharSet=Latin

REM set extracharsets=greek-coptic,cyrllic
REM set unicodecharsets=cjk-unified,hangul-syllables,hiragana,katakana

REM some examples - just use any windows font you like

%fonttool% -f "Bauhaus 93" -h 20 -o %fontdir%\small\Bauhaus93 -c %CharSet%
%fonttool% -f "Bauhaus 93" -h 30 -o %fontdir%\medium\Bauhaus93 -c %CharSet%
%fonttool% -f "Bauhaus 93" -h 40 -o %fontdir%\large\Bauhaus93 -c %CharSet%

%fonttool% -f "Berlin Sans FB" -h 20 -o %fontdir%\small\BerlinSansFB -c %CharSet%
%fonttool% -f "Berlin Sans FB" -h 30 -o %fontdir%\medium\BerlinSansFB -c %CharSet%
%fonttool% -f "Berlin Sans FB" -h 40 -o %fontdir%\large\BerlinSansFB -c %CharSet%

%fonttool% -f "Arial" -h 20 -o %fontdir%\small\Arial -c %CharSet%
%fonttool% -f "Arial" -h 30 -o %fontdir%\medium\Arial -c %CharSet%
%fonttool% -f "Arial" -h 40 -o %fontdir%\large\Arial -c %CharSet%

rem %fonttool% -f "Arial Unicode MS" -h 14 -o %fontdir%\small -c %unicodecharsets%
pause
