@echo off
REM Windows build script for ToonTalk WASM

echo Setting up Emscripten environment...
call %USERPROFILE%\emsdk\emsdk_env.bat

echo.
echo Building ToonTalk WASM...
cd /d "%~dp0"

REM Clean previous build
if exist build rmdir /s /q build

REM Configure with Emscripten
emcmake cmake -B build -S .

REM Build
emmake make -C build

echo.
echo ✅ Build complete!
echo Output files are in build\toontalk-core.js and build\toontalk-core.wasm
echo.
echo Run 'serve.sh' or 'serve.bat' to test in browser
pause
