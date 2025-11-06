# Build WASM module for ToonTalk Web
# Wrapper script that calls core/build.ps1

Write-Host "ToonTalk Web - WASM Build" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
Write-Host ""

# Navigate to web/core directory
$webDir = Split-Path -Parent $PSScriptRoot
$coreDir = Join-Path $webDir "core"

if (-not (Test-Path $coreDir)) {
    Write-Host "Error: core directory not found!" -ForegroundColor Red
    exit 1
}

Set-Location $coreDir

# Run the core build script
if (Test-Path "build.ps1") {
    .\build.ps1
} else {
    Write-Host "Error: build.ps1 not found in core directory!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "WASM build complete!" -ForegroundColor Green
Write-Host "Restart your dev server to use the new build." -ForegroundColor Yellow
