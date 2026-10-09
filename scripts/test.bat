@echo off
rem Build and run the unit tests (same arguments as build.bat)
rem
rem Usage: scripts\test.bat [release|debug|debugoptimized] [clean] [-- meson options...]

call "%~dp0build.bat" test %*
exit /b %errorlevel%
