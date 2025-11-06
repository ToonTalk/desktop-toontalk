# Start development server for ToonTalk Web
# Checks for node_modules and installs if needed

param(
    [switch]$SkipInstall
)

Write-Host "ToonTalk Web - Development Server" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# Check if Node.js is installed
$nodePath = Get-Command node -ErrorAction SilentlyContinue
if (-not $nodePath) {
    Write-Host "Error: Node.js not found!" -ForegroundColor Red
    Write-Host "Please install Node.js from https://nodejs.org/" -ForegroundColor Yellow
    exit 1
}

Write-Host "Node.js version:" -ForegroundColor Green
node --version
Write-Host ""

# Navigate to web directory
$webDir = Split-Path -Parent $PSScriptRoot
Set-Location $webDir

# Check if node_modules exists
if (-not (Test-Path "node_modules") -and -not $SkipInstall) {
    Write-Host "Installing dependencies..." -ForegroundColor Yellow
    npm install
    Write-Host ""
}

# Start dev server
Write-Host "Starting development server..." -ForegroundColor Green
Write-Host "Open your browser to: http://localhost:3000" -ForegroundColor Cyan
Write-Host ""
Write-Host "Press Ctrl+C to stop the server" -ForegroundColor Yellow
Write-Host ""

npm run dev
