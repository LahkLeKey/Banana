param(
    [string]$DepotId = "4767151",
    [string]$BuildId = "dev-candidate",
    [string]$ContentRoot = "artifacts/steam-build/dev-candidate/win-unpacked",
    [string]$OutDir = "artifacts/steam-build/dev-candidate/http-upload",
    [switch]$BuildCandidate
)

$ErrorActionPreference = "Stop"

function Resolve-TargetPath {
    param(
        [string]$Root,
        [string]$InputPath
    )

    if ($InputPath -match '^[A-Za-z]:[\\/]' -or $InputPath -match '^\\\\' -or $InputPath -match '^/') {
        return $InputPath
    }

    return Join-Path $Root $InputPath
}

$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $root

if ($BuildCandidate) {
    Write-Output "[steam-http-zip] building candidate first"
    bash scripts/build-steam-playable-candidate.sh
}

$contentPath = Resolve-TargetPath -Root $root -InputPath $ContentRoot
if (-not (Test-Path $contentPath)) {
    throw "Content root not found: $contentPath"
}
$contentPath = (Resolve-Path $contentPath).Path

$outPath = Resolve-TargetPath -Root $root -InputPath $OutDir
New-Item -ItemType Directory -Path $outPath -Force | Out-Null
$outPath = (Resolve-Path $outPath).Path

$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$zipPath = Join-Path $outPath ("depot-" + $DepotId + "-" + $stamp + ".zip")

if (Test-Path $zipPath) {
    Remove-Item -Path $zipPath -Force
}

Compress-Archive -Path (Join-Path $contentPath "*") -DestinationPath $zipPath -CompressionLevel Optimal

$size = (Get-Item $zipPath).Length
Write-Output "[steam-http-zip] zip ready: $zipPath"
Write-Output "[steam-http-zip] bytes: $size"
Write-Output "[steam-http-zip] Upload in Steamworks for depot $DepotId (Standard by default)."
