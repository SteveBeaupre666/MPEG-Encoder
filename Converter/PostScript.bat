@ECHO OFF
if '%1'=='' goto error

set AppDir=..
set DllName=Converter.dll

copy %1\%DllName% %AppDir%\%DllName%

goto end
:error
ECHO Not enough arguments...
:end