@echo off
setlocal
set "HERE=%~dp0"
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"

if not exist "%VCVARS%" goto :no_vcvars

echo [1/2] Initializing MSVC x64 Environment...
call "%VCVARS%" >nul
if errorlevel 1 goto :vcvars_failed

echo [2/2] Compiling Speedometer.asi...
if exist "%HERE%..\Speedometer.asi.old" del /f /q "%HERE%..\Speedometer.asi.old" >nul 2>&1
if exist "%HERE%..\Speedometer.asi" move /y "%HERE%..\Speedometer.asi" "%HERE%..\Speedometer.asi.old" >nul 2>&1

cl /nologo /LD /O2 /EHsc /std:c++20 /MT /W3 /DWIN32_LEAN_AND_MEAN /DNOMINMAX /I "%HERE%..\sdk" "%HERE%main.cpp" /Fo"%HERE%main.obj" /Fe"%HERE%..\Speedometer.asi" /link "%HERE%..\sdk\ScriptHookV.lib" user32.lib dxgi.lib

if errorlevel 1 goto :compile_failed

echo.
echo ========================================================
echo  SUCCESS: Speedometer.asi successfully compiled and built!
echo ========================================================
exit /b 0

:no_vcvars
echo [ERROR] vcvars64.bat not found in default Visual Studio paths.
exit /b 1

:vcvars_failed
echo [ERROR] Failed to initialize MSVC x64 environment.
exit /b 1

:compile_failed
echo [ERROR] Compilation failed.
exit /b 1
