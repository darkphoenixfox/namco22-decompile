@echo off
rem build.bat -- builds tc2_leia.dll (src/leia/tc2_leia.cpp: the Leia SR weaver for Display > 3D) with MSVC.
rem   build.bat [LeiaSR SDK folder] [output folder]
rem The SDK folder defaults to %LEIASR_SDKROOT%; the output to the folder of this script. Put tc2_leia.dll beside TimeCrisis2.exe.
rem Needs Visual Studio 2022 (or its Build Tools) with the C++ tools; the computer that plays needs the Leia SR platform installed.
setlocal
set SDK=%~1
if "%SDK%"=="" set SDK=%LEIASR_SDKROOT%
if "%SDK%"=="" ( echo Give the LeiaSR SDK folder, or set LEIASR_SDKROOT. & exit /b 1 )
set OUT=%~2
if "%OUT%"=="" set OUT=%~dp0
rem (the "(x86)" of the path would end a for's parentheses: through a variable)
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`call "%VSWHERE%" -latest -products * -property installationPath`) do set "VS=%%i"
if not exist "%VS%\VC\Auxiliary\Build\vcvars64.bat" ( echo No Visual Studio C++ tools found. & exit /b 1 )
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul
set TP=%SDK%\third_party
cl /nologo /O2 /EHsc /MD /LD /DWIN32 /DSRDISPLAY_LAZYBINDING /I"%SDK%\include" /I"%TP%\OpenCV\include" /I"%TP%\GLog\include" /I"%TP%\asio\include" /I"%TP%\Leap\include" ^
   "%~dp0tc2_leia.cpp" /Fe"%OUT%\tc2_leia.dll" /Fo"%TEMP%\tc2_leia.obj" ^
   /link /LIBPATH:"%SDK%\lib" simulatedreality.lib SimulatedRealityCore.lib SimulatedRealityOpenGL.lib SimulatedRealityDisplays.lib "%TP%\OpenCV\lib\x64\opencv_world343.lib" user32.lib
exit /b %ERRORLEVEL%
