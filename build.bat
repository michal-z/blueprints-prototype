@echo off
setlocal enableextensions enabledelayedexpansion

set "NAME=blueprints-prototype"

:: (D)ebug, (R)elease
set CONFIG=D

set CFLAGS=/W4 /std:c++latest /GR- /EHsc /fp:except- /fp:precise /nologo

:: Options that measure compile/link times:
:: /Bt
:: /d2cgsummary
::set CFLAGS=%CFLAGS% /Bt

if %CONFIG%==D set CFLAGS=%CFLAGS% /GS /Zi /Od /D"_DEBUG" /MTd /RTCs
if %CONFIG%==R set CFLAGS=%CFLAGS% /GS- /O2 /Oi /Ot /Gy /MT /D"NDEBUG"

set LFLAGS=/INCREMENTAL:NO /NOLOGO /NOIMPLIB /NOEXP
if %CONFIG%==D SET LFLAGS=%LFLAGS% /DEBUG:FULL
if %CONFIG%==R SET LFLAGS=%LFLAGS%

if exist "%NAME%.exe" del "%NAME%.exe"

if not "%1"=="clean" goto clean_end
if exist "*.obj" del "*.obj"
if exist "*.pdb" del "*.pdb"
if exist "*.ifc" del "*.ifc"
if exist "imgui.lib" del "imgui.lib"
:clean_end

::
:: imgui
::
if exist imgui.lib goto imgui_end
cl %CFLAGS% /MP /c "src/external/imgui/*.cpp" /I "src/external" /Fd:"imgui.pdb"
lib.exe /NOLOGO *.obj /OUT:"imgui.lib"
if exist *.obj del *.obj
:imgui_end

::
:: std
::
if exist std.ifc goto std_end
cl %CFLAGS% /c "%VCToolsInstallDir%\modules\std.ixx" /Fd:"std.pdb"
:std_end

::
:: base
::
if exist base.h.ifc goto base_end
cl %CFLAGS% /exportHeader "src/base.h" /I "src/external" /Fd:"base.pdb"
:base_end

::
:: game
::
set SRC=^
src\gl.ixx ^
src\main.cpp

cl ^
%CFLAGS% /Fe:"%NAME%.exe" /Fd:"%NAME%.pdb" %SRC% /headerUnit "src/base.h=base.h.ifc" ^
/link ^
%LFLAGS% opengl32.lib std.obj imgui.lib SDL3.lib /subsystem:CONSOLE

:: Delete .obj and .ifc files (all except std.obj, std.ifc and base.h.ifc)
for %%f in (%SRC%) do if exist %%~nf.* del %%~nf.*

if "%1"=="run" if exist "%NAME%.exe" "%NAME%.exe"
