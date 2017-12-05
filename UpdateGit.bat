@ECHO OFF

REM ////////////////////////////////////////////////////////////////////////////
REM // Setup files names
REM ////////////////////////////////////////////////////////////////////////////

REM set HFiles       =*.h
REM set CppFiles     =*.cpp
REM set DefFiles     =*.def
REM set SlnFiles     =*.sln
REM set VcxprojFiles =*.vcxproj
REM set FiltersFiles =*.filters
REM set UserFiles    =*.user

REM set MPEGEncoderFiles =MPEGEncoder.*
REM set MainUnitFiles    =MainUnit.*
REM set OpenDirUnitFiles =OpenDirectoryUnit.*
REM set ConverterFiles   =Converter.*

set DllFiles    =Converter.*
set CppFiles    =*.h;*.cpp;*.def;*.sln;*.vcxproj;*.filters;*.user
set DelphiFiles =*.pas*;*.dcu*;*.dfm*;*.ddp*;*.dof*;*.dpr*;*.cfg*;*.res
set TextFiles   =*.txt

REM ////////////////////////////////////////////////////////////////////////////
REM // Setup directories names
REM ////////////////////////////////////////////////////////////////////////////

set DllDir     =Converter
set DllCodeDir =source

set SrcDir     =C:\New Programming Folder\Programs\MPEG Encoder
set DstDir     =C:\GitHubTest\MPEG-Encoder

set SrcDirLv1=%SrcDir%
set SrcDirLv2=%SrcDir%\%DllDir%
set SrcDirLv3=%SrcDir%\%DllDir%\%DllCodeDir%

set DstDirLv1=%DstDir%
set DstDirLv2=%DstDir%\%DllDir%
set DstDirLv3=%DstDir%\%DllDir%\%DllCodeDir%

REM ////////////////////////////////////////////////////////////////////////////
REM // Cleanup the output directory
REM ////////////////////////////////////////////////////////////////////////////

IF EXIST "%DstDirLv2%" rmdir "%DstDirLv2%" /S /Q

IF EXIST "%DstDirLv1%" (

	del "%DstDirLv1%\%MPEGEncoderFiles%"   /F /Q
	del "%DstDirLv1%\%MainUnitFiles%"      /F /Q
	del "%DstDirLv1%\%OpenDirUnitFiles%"   /F /Q
	del "%DstDirLv1%\%ConverterFiles%"     /F /Q
	del "%DstDirLv1%\%TextFiles%"          /F /Q

	REM del "%DstDirLv1%\*.pas*" /F /Q
	REM del "%DstDirLv1%\*.dcu*" /F /Q
	REM del "%DstDirLv1%\*.dfm*" /F /Q
	REM del "%DstDirLv1%\*.ddp*" /F /Q
	REM del "%DstDirLv1%\*.dof*" /F /Q
	REM del "%DstDirLv1%\*.dpr*" /F /Q
	REM del "%DstDirLv1%\*.cfg*" /F /Q
	REM del "%DstDirLv1%\*.res*" /F /Q
)

REM ////////////////////////////////////////////////////////////////////////////
REM // Rebuild directories structure
REM ////////////////////////////////////////////////////////////////////////////
IF NOT EXIST "%DstDirLv1%" md "%DstDirLv1%"
IF NOT EXIST "%DstDirLv2%" md "%DstDirLv2%"
IF NOT EXIST "%DstDirLv3%" md "%DstDirLv3%"

REM ////////////////////////////////////////////////////////////////////////////
REM // Copy selected files 
REM ////////////////////////////////////////////////////////////////////////////

copy "%SrcDirLv1%\%MPEGEncoderFiles%"        "%DstDirLv1%\"
copy "%SrcDirLv1%\%MainUnitFiles%"           "%DstDirLv1%\"
copy "%SrcDirLv1%\%OpenDirUnitFiles%"  "%DstDirLv1%\"
copy "%SrcDirLv1%\Notes.txt"            "%DstDirLv1%\"

copy "%SrcDirLv2\*.h"                   "%DstDirLv2%\"
copy "%SrcDirLv2\*.cpp"                 "%DstDirLv2%\"
copy "%SrcDirLv2\*.def"                 "%DstDirLv2%\"
copy "%SrcDirLv2\*.sln"                 "%DstDirLv2%\"
copy "%SrcDirLv2\*.vcxproj"             "%DstDirLv2%\"
copy "%SrcDirLv2\*.filters"             "%DstDirLv2%\"
copy "%SrcDirLv2\*.user"                "%DstDirLv2%\"

copy "%SrcDirLv3\*.h"                   "%DstDirLv3%\"
copy "%SrcDirLv3\*.cpp"                 "%DstDirLv3%\"

REM :jmp

