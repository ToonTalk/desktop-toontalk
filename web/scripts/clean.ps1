# Clean build artifacts for ToonTalk Web

Write-Host "ToonTalk Web - Clean Build Artifacts" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# Navigate to web directory
$webDir = Split-Path -Parent $PSScriptRoot
Set-Location $webDir

$itemsToRemove = @(
    "node_modules",
    "dist",
    "core\build",
    "package-lock.json"
)

foreach ($item in $itemsToRemove) {
    if (Test-Path $item) {
        Write-Host "Removing $item..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $item
        Write-Host "  Removed" -ForegroundColor Green
    } else {
        Write-Host "Skipping $item (not found)" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "Clean complete!" -ForegroundColor Green
Write-Host "Run 'npm install' to reinstall dependencies." -ForegroundColor Cyan
