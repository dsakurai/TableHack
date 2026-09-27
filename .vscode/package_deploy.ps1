param(
  [string]$Workspace = ${PWD}
)

$deploy = Join-Path $Workspace "build\deploy\release\example-portable"
$zip = Join-Path $Workspace "build\example-portable.zip"

if (-not (Test-Path $deploy)) {
  Write-Error "Deploy folder not found: $deploy"
  exit 1
}

if (Test-Path $zip) {
  Remove-Item -Force $zip
}

Compress-Archive -Path (Join-Path $deploy '*') -DestinationPath $zip -Force
Write-Host "Created $zip"