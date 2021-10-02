@echo off

set fonttool=fonttool.exe
set fontdir=fonts

REM ensure the folder exists
mkdir %fontdir%

REM and delete any old files
del %fontdir%\*.tga
del %fontdir%\*.ftab
del %fontdir%\*.fraw


set extracharsets=greek-coptic,cyrllic
set unicodecharsets=cjk-unified,hangul-syllables,hiragana,katakana


%fonttool% -f "Arial" -h 14 -o %fontdir%\Arial -c latin
%fonttool% -f "Bauhaus 93" -h 20 -o %fontdir%\Bauhaus93 -c latin



pause
