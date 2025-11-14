@echo off
REM Simple HTTP server for testing WASM on Windows

cd /d "%~dp0"

echo 🌐 Starting test server at http://localhost:8000
echo 📄 Open http://localhost:8000/test.html in your browser
echo.

REM Try to use npx http-server first
where npx >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    npx http-server -p 8000
    exit /b
)

REM Try Python 3
where python3 >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    python3 -m http.server 8000
    exit /b
)

REM Try Python
where python >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    python -m http.server 8000
    exit /b
)

REM Use Node.js from Emscripten
if exist "%USERPROFILE%\emsdk\node" (
    for /d %%i in ("%USERPROFILE%\emsdk\node\*") do (
        "%%i\bin\node.exe" -e "const http=require('http');const fs=require('fs');const path=require('path');const server=http.createServer((req,res)=>{const filePath='.'+(req.url==='/'?'/test.html':req.url);const ext=path.extname(filePath);const contentType={'.html':'text/html','.js':'application/javascript','.wasm':'application/wasm','.css':'text/css'}[ext]||'application/octet-stream';fs.readFile(filePath,(err,content)=>{if(err){res.writeHead(404);res.end('Not found');}else{res.writeHead(200,{'Content-Type':contentType});res.end(content);}});});server.listen(8000);console.log('Server running at http://localhost:8000/');"
        exit /b
    )
)

echo ❌ Error: No web server found!
echo Please install Node.js or Python, or install http-server:
echo   npm install -g http-server
pause
