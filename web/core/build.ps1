# Build script for ToonTalk WASM core (Windows PowerShell)
# Requires Emscripten SDK (emsdk) to be installed and activated

Write-Host "ToonTalk WASM Core Build Script (Windows)" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Check if emscripten is available
$emccPath = Get-Command emcc -ErrorAction SilentlyContinue
if (-not $emccPath) {
    Write-Host "Error: Emscripten (emcc) not found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install and activate emsdk:" -ForegroundColor Yellow
    Write-Host "  git clone https://github.com/emscripten-core/emsdk.git" -ForegroundColor White
    Write-Host "  cd emsdk" -ForegroundColor White
    Write-Host "  .\emsdk install latest" -ForegroundColor White
    Write-Host "  .\emsdk activate latest" -ForegroundColor White
    Write-Host "  .\emsdk_env.ps1" -ForegroundColor White
    Write-Host ""
    Write-Host "Then run this script again." -ForegroundColor Yellow
    exit 1
}

Write-Host "Using Emscripten:" -ForegroundColor Green
emcc --version | Select-Object -First 1

# Create build directory
$buildDir = "build"
if (Test-Path $buildDir) {
    Write-Host "Cleaning existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

Write-Host "Creating build directory..." -ForegroundColor Green
New-Item -ItemType Directory -Path $buildDir | Out-Null
Set-Location $buildDir

Write-Host "Running CMake..." -ForegroundColor Green
emcmake cmake ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "Building WASM module..." -ForegroundColor Green
emmake make

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host ""
Write-Host "Build complete! Output files:" -ForegroundColor Green
Get-ChildItem toontalk-core.* | Format-Table Name, Length -AutoSize

Set-Location ..

Write-Host ""
Write-Host "Files are ready in: core\build\" -ForegroundColor Cyan
Write-Host "The web app will load toontalk-core.js and toontalk-core.wasm" -ForegroundColor Cyan
