param(
    [string]$CandidateExe = "artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe",
    [string]$SteamGameId = "",
    [string]$SteamAppId = "4767150",
    [string]$EvidenceDir = "artifacts/uat/steam"
)

$ErrorActionPreference = "Stop"

function Resolve-SteamExe {
    $registryPaths = @(
        "HKCU:\Software\Valve\Steam",
        "HKLM:\SOFTWARE\WOW6432Node\Valve\Steam",
        "HKLM:\SOFTWARE\Valve\Steam"
    )

    foreach ($keyPath in $registryPaths) {
        try {
            $value = (Get-ItemProperty -Path $keyPath -Name SteamExe -ErrorAction Stop).SteamExe
            if ($value -and (Test-Path $value)) {
                return $value
            }
        }
        catch {
            continue
        }
    }

    return $null
}

$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$candidatePath = Join-Path $root $CandidateExe
$evidencePath = Join-Path $root $EvidenceDir
New-Item -ItemType Directory -Path $evidencePath -Force | Out-Null

if (-not (Test-Path $candidatePath)) {
    throw "Candidate executable not found: $candidatePath"
}

$steamExe = Resolve-SteamExe
if (-not $steamExe) {
    throw "Steam installation was not found. Install/sign in to Steam first."
}

Write-Output "[steam-library-uat] Steam executable: $steamExe"
Write-Output "[steam-library-uat] Candidate executable: $candidatePath"

$reportFile = Join-Path $evidencePath ("steam-library-uat-" + (Get-Date -Format "yyyyMMdd-HHmmss") + ".md")

$report = @()
$report += "# Steam Library UAT Session"
$report += ""
$report += "- Timestamp: $(Get-Date -Format o)"
$report += "- SteamExe: $steamExe"
$report += "- CandidateExe: $candidatePath"
$report += "- SteamAppId: $SteamAppId"
$report += ""

if ([string]::IsNullOrWhiteSpace($SteamGameId)) {
    Write-Output "[steam-library-uat] No SteamGameId provided."
    if (-not [string]::IsNullOrWhiteSpace($SteamAppId)) {
        $appUri = "steam://run/$SteamAppId"
        Write-Output "[steam-library-uat] Attempting Steam app launch URI: $appUri"
        Start-Process $appUri | Out-Null
        Start-Sleep -Seconds 3
        $running = Get-Process | Where-Object { $_.ProcessName -like "BananaEngineer*" }
        if ($running) {
            Write-Output "[steam-library-uat] App launch process detected."
            $report += "## Status"
            $report += "- Launch check: PASS via steam://run/$SteamAppId (process detected)."
            $report += ""
            $report += "## Manual UAT"
            $report += "- Validate gameplay flow and Build Lab behavior from Steam-launched app."
        }
        else {
            Write-Output "[steam-library-uat] App process not detected yet; fallback to non-Steam flow guidance."
            $report += "## Status"
            $report += "- Launch check: WARN via steam://run/$SteamAppId (process not detected)."
            $report += ""
            $report += "## Next Steps"
            $report += "1. Confirm app ownership/branch entitlement in Steam partner account."
            $report += "2. If needed, add candidate as non-Steam game and rerun with -SteamGameId."
            $report += "3. Execute checklist in .specify/specs/001-steamworks-release-readiness/checklists/steam-uat-readiness.md."
        }
    }
    else {
        Write-Output "[steam-library-uat] Open Steam -> Games -> Add a Non-Steam Game, add BananaEngineer.exe, then rerun with -SteamGameId <id>."

        $report += "## Status"
        $report += "- Pending: add candidate as non-Steam game and provide SteamGameId."
        $report += ""
        $report += "## Next Steps"
        $report += "1. Add non-Steam game in Steam library."
        $report += "2. Find game id and rerun script with -SteamGameId."
        $report += "3. Execute checklist in .specify/specs/001-steamworks-release-readiness/checklists/steam-uat-readiness.md."
    }
}
else {
    $uri = "steam://rungameid/$SteamGameId"
    Write-Output "[steam-library-uat] Launching via Steam URI: $uri"
    Start-Process $uri | Out-Null

    Start-Sleep -Seconds 3
    $running = Get-Process | Where-Object { $_.ProcessName -like "BananaEngineer*" }

    $report += "## Status"
    if ($running) {
        $report += "- Launch check: PASS (process running via Steam launch URI)."
    }
    else {
        $report += "- Launch check: WARN (BananaEngineer process not detected yet)."
    }
    $report += "- SteamGameId: $SteamGameId"
    $report += ""
    $report += "## Manual UAT"
    $report += "- Validate session-room -> game-engine flow."
    $report += "- Validate Build Lab class/build/gear/combo interactions."
    $report += "- Capture screenshots/video evidence."
}

$reportText = $report -join "`r`n"
Set-Content -Path $reportFile -Value $reportText -Encoding UTF8
Write-Output "[steam-library-uat] Report written: $reportFile"
