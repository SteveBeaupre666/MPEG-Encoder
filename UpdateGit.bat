@ECHO OFF

REM ////////////////////////////////////////////////////////////////////////////
REM // Setup directories names
REM ////////////////////////////////////////////////////////////////////////////

set SrcDir =source
set DllDir =Converter

set GitDir =C:\GitHubTest
set DstDir =%GitDir%\MPEG-Encoder

set SrcDir =C:\New Programming Folder\Programs\MPEG Encoder

set SrcDirLv1=%SrcDir%
set SrcDirLv2=%SrcDir%\%DllDir%
set SrcDirLv3=%SrcDir%\%DllDir%\%DllCodeDir%

set DstDirLv1=%DstDir%
set DstDirLv2=%DstDir%\%DllDir%
set DstDirLv3=%DstDir%\%DllDir%\%DllCodeDir%

REM ////////////////////////////////////////////////////////////////////////////
REM // Cleanup the output directory
REM ////////////////////////////////////////////////////////////////////////////

ECHO Deleting directory...
REM IF EXIST "%DstDirLv1%" rmdir "%DstDirLv1%" /S /Q

del "%DstDirLv1%\*.txt"
del "%DstDirLv1%\MPEGEncoder.*"
del "%DstDirLv1%\MainUnit.*"
del "%DstDirLv1%\OpenDirectoryUnit.*"
del "%DstDirLv1%\Converter.dll"

IF EXIST "%DstDirLv2%" rmdir "%DstDirLv2%" /S /Q

goto jmp

REM ////////////////////////////////////////////////////////////////////////////
REM // Rebuild directories structure
REM ////////////////////////////////////////////////////////////////////////////

md "%DstDirLv1%"
md "%DstDirLv2%"
md "%DstDirLv3%"

REM ////////////////////////////////////////////////////////////////////////////
REM // Copy selected files 
REM ////////////////////////////////////////////////////////////////////////////

copy "%SrcDirLv1%\*.txt"                "%DstDirLv1%\"
copy "%SrcDirLv1%\MPEGEncoder.*"        "%DstDirLv1%\"
copy "%SrcDirLv1%\MainUnit.*"           "%DstDirLv1%\"
copy "%SrcDirLv1%\OpenDirectoryUnit.*"  "%DstDirLv1%\"
copy "%SrcDirLv1%\Converter.dll"        "%DstDirLv1%\"

copy "%SrcDirLv2%\*.h"                  "%DstDirLv2%\"
copy "%SrcDirLv2%\*.cpp"                "%DstDirLv2%\"
copy "%SrcDirLv2%\*.def"                "%DstDirLv2%\"
copy "%SrcDirLv2%\*.sln"                "%DstDirLv2%\"
copy "%SrcDirLv2%\*.vcxproj"            "%DstDirLv2%\"
copy "%SrcDirLv2%\*.filters"            "%DstDirLv2%\"
copy "%SrcDirLv2%\*.user"               "%DstDirLv2%\"

copy "%SrcDirLv3%\*.h"                  "%DstDirLv3%\"
copy "%SrcDirLv3%\*.cpp"                "%DstDirLv3%\"

:jmp

