@echo off
setlocal

set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\native

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if exist "%BUILD_DIR%\CMakeCache.txt" del /q "%BUILD_DIR%\CMakeCache.txt"
if exist "%BUILD_DIR%\CMakeFiles" rmdir /s /q "%BUILD_DIR%\CMakeFiles"

cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b 1

echo Native build completed. Artifacts: %BUILD_DIR%\bin\Release
