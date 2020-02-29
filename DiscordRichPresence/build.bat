@echo off
IF NOT [%~1]==[] (
    set BUILD_CONFIGURATION=%~1
)
IF NOT DEFINED BUILD_CONFIGURATION set BUILD_CONFIGURATION=Release

echo Building in '%BUILD_CONFIGURATION%' mode

mkdir "../%BUILD_CONFIGURATION%/"

REM Create resource file for the Config/UI
set winsdk_ver=10.0.17134.0
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.17134.0\x86\rc.exe" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.17134.0\um" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.17134.0\shared" Config.rc

REM Set Path/Env variables that cl.exe needs
set vcvarsall=x86  %winsdk_ver% -vcvars_ver=14.1
IF exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" %vcvarsall%
) ELSE (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %vcvarsall%
)
set includepaths=/I "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.16.27023\include" ^
/I "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17134.0\um" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.17134.0\ucrt" ^
/I "C:/Program Files (x86)/Winamp SDK/Winamp" ^
/I "../discord-rpc/win32-dynamic/include/"

REM Set CL.exe flags that match the .vcxproj values for Release|win32 https://docs.microsoft.com/en-us/visualstudio/msbuild/cl-task?view=vs-2019
REM DLL will be located a directory lower than this batch file, to match Visual Studio implementation
IF NOT BUILD_CONFIGURATION == Release (
    REM Optimization disabled, no function level linking, no intrinsic functions
    set buildflags=/Od /Yc /Gy /Oi /W3
) ELSE (
    set buildflags=/O2 /Gy /W3
)
set compilerflags=/nologo %buildflags% /DUNICODE /DWIN32 /DDEBUG /DDISCORDRICHPRESENCE_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /EHsc /std:c++14 %includepaths%
set linkerflags=/DLL /OUT:"../%BUILD_CONFIGURATION%/gen_DiscordRichPresence.dll" User32.LIB Config.res

echo Using build flags: %buildflags%

cl.exe %compilerflags% *.cpp /link %linkerflags%

REM Cleanup
del *.ilk *.obj *.pdb *.lib *.pch *.exp Config.res RCa22352