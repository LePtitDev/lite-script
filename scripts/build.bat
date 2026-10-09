@echo off
rem Build the library, the tests, the examples and the playground (litescript-pg.exe)
rem
rem Usage: scripts\build.bat [release|debug|debugoptimized] [clean] [test] [-- meson options...]
rem   release, debug...  Build type (release by default), each type has its directory builddir\<type>
rem   clean              Delete the build directory before building
rem   test               Run the unit tests after the build
rem   -- options         Options given to "meson setup" (or "meson configure" if already configured),
rem                      for example: scripts\build.bat -- -Dtests=false -Dhttp=disabled
rem
rem Needs Python with Meson and Ninja (python -m pip install meson ninja) and Visual Studio 2022
rem (the environment of Visual Studio is activated automatically if cl.exe isn't in the PATH).

setlocal EnableDelayedExpansion

set "ROOT=%~dp0.."
set "BUILD_TYPE=release"
set "CLEAN=0"
set "TEST=0"
set "OPTIONS="
set "ARGS=%*"

:parse
if "%~1"=="" goto parsed
if "%~1"=="--" goto options
if /i "%~1"=="release" (set "BUILD_TYPE=release" & shift & goto parse)
if /i "%~1"=="debug" (set "BUILD_TYPE=debug" & shift & goto parse)
if /i "%~1"=="debugoptimized" (set "BUILD_TYPE=debugoptimized" & shift & goto parse)
if /i "%~1"=="clean" (set "CLEAN=1" & shift & goto parse)
if /i "%~1"=="test" (set "TEST=1" & shift & goto parse)
if /i "%~1"=="-h" goto usage
if /i "%~1"=="--help" goto usage
echo Unknown argument "%~1"
goto usage_error

:options
rem The options are taken from the original command line ("=" separates the arguments of a batch)
if "%~2"=="" goto parsed
set "OPTIONS=!ARGS:*-- =!"

:parsed
set "BUILD_DIR=%ROOT%\builddir\%BUILD_TYPE%"

rem Find Meson: the command "meson" or the Python module
set "MESON="
where meson >nul 2>nul
if not errorlevel 1 set "MESON=meson"
if not defined MESON (
    for %%P in (python py python3) do (
        if not defined MESON (
            %%P -c "import mesonbuild" >nul 2>nul
            if not errorlevel 1 set "MESON=%%P -m mesonbuild.mesonmain"
        )
    )
)
if not defined MESON (
    echo Meson isn't installed, install it with: python -m pip install meson ninja
    exit /b 1
)

rem Activate the environment of Visual Studio if the compiler isn't available
set "VSENV="
where cl >nul 2>nul
if errorlevel 1 set "VSENV=--vsenv"

if "%CLEAN%"=="1" if exist "%BUILD_DIR%" (
    echo Deleting %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%" || exit /b 1
)

if not exist "%BUILD_DIR%\meson-private\coredata.dat" (
    %MESON% setup "%BUILD_DIR%" "%ROOT%" --buildtype=%BUILD_TYPE% %VSENV% %OPTIONS% || exit /b 1
) else if defined OPTIONS (
    %MESON% configure "%BUILD_DIR%" %OPTIONS% || exit /b 1
)

%MESON% compile -C "%BUILD_DIR%" || exit /b 1

if "%TEST%"=="1" (
    %MESON% test -C "%BUILD_DIR%" --print-errorlogs || exit /b 1
)

echo.
if exist "%BUILD_DIR%\playground\litescript-pg.exe" (
    for %%F in ("%BUILD_DIR%\playground\litescript-pg.exe") do echo Playground: %%~fF
)
exit /b 0

:usage
echo Usage: scripts\build.bat [release^|debug^|debugoptimized] [clean] [test] [-- meson options...]
exit /b 0

:usage_error
echo Usage: scripts\build.bat [release^|debug^|debugoptimized] [clean] [test] [-- meson options...]
exit /b 2
