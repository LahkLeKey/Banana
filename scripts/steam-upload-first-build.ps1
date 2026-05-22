param(
    [string]$AppId = "4767150",
    [string]$DepotId,
    [string]$BuildId = "dev-candidate",
    [string]$ContentRoot = "artifacts/steam-build/dev-candidate/win-unpacked",
    [string]$Branch = "default",
    [string]$SteamCmdPath = "",
    [string]$SteamUsername = "",
    [string]$SteamPassword = "",
    [string]$SteamGuardCode = "",
    [switch]$BuildCandidate,
    [switch]$Execute
)

$ErrorActionPreference = "Stop"

$steamCmdZipUrl = "https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip"

function Select-SteamCmdInstallRoot {
    $preferred = "C:\Program Files\SteamCMD"
    if (Test-Path "C:\Program Files") {
        return $preferred
    }

    return "C:\Program Files (x86)\SteamCMD"
}

function Install-SteamCmd {
    param([string]$InstallRoot)

    Write-Output "[steam-upload] SteamCMD not found. Installing from official zip: $steamCmdZipUrl"
    New-Item -ItemType Directory -Path $InstallRoot -Force | Out-Null

    $tmpZip = Join-Path $env:TEMP ("steamcmd-" + [Guid]::NewGuid().ToString("N") + ".zip")
    try {
        Invoke-WebRequest -Uri $steamCmdZipUrl -OutFile $tmpZip
        Expand-Archive -Path $tmpZip -DestinationPath $InstallRoot -Force
    }
    finally {
        if (Test-Path $tmpZip) {
            Remove-Item -Path $tmpZip -Force -ErrorAction SilentlyContinue
        }
    }

    $installedExe = Join-Path $InstallRoot "steamcmd.exe"
    if (-not (Test-Path $installedExe)) {
        throw "SteamCMD installation completed but steamcmd.exe was not found at expected path: $installedExe"
    }

    Write-Output "[steam-upload] SteamCMD installed: $installedExe"
    return $installedExe
}

function Resolve-SteamCmd {
    param([string]$ExplicitPath)

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPath) -and (Test-Path $ExplicitPath)) {
        return (Resolve-Path $ExplicitPath).Path
    }

    $candidates = @(
        "C:\\Program Files\\SteamCMD\\steamcmd.exe",
        "C:\\Program Files (x86)\\SteamCMD\\steamcmd.exe",
        "C:\\Program Files (x86)\\Steam\\steamcmd.exe",
        "C:\\Program Files\\Steam\\steamcmd.exe",
        "C:\\steamcmd\\steamcmd.exe"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    $command = Get-Command steamcmd.exe -ErrorAction SilentlyContinue
    if ($null -ne $command -and -not [string]::IsNullOrWhiteSpace($command.Source) -and (Test-Path $command.Source)) {
        return (Resolve-Path $command.Source).Path
    }

    return $null
}

function Invoke-SteamCmd {
    param(
        [string]$SteamCmdExe,
        [string[]]$SteamArgs
    )

    $steamCmdDir = Split-Path -Parent $SteamCmdExe

    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = $SteamCmdExe
    $psi.WorkingDirectory = $steamCmdDir
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $false
    $psi.RedirectStandardError = $false

    # Avoid inheriting Git Bash/MSYS runtime environment that can crash SteamCMD.
    $minimalPath = @(
        $steamCmdDir,
        (Join-Path $env:SystemRoot "System32"),
        $env:SystemRoot,
        (Join-Path $env:SystemRoot "System32\Wbem"),
        (Join-Path $env:SystemRoot "System32\WindowsPowerShell\v1.0")
    ) -join ';'

    $psi.EnvironmentVariables["PATH"] = $minimalPath

    foreach ($varName in @("MSYSTEM", "MSYS", "MSYS2_PATH_TYPE", "MINGW_PREFIX", "CHERE_INVOKING")) {
        if ($psi.EnvironmentVariables.ContainsKey($varName)) {
            $psi.EnvironmentVariables.Remove($varName)
        }
    }

    $escapedArgs = $SteamArgs | ForEach-Object {
        if ($_ -match '[\s"]') {
            '"' + ($_ -replace '"', '\\"') + '"'
        }
        else {
            $_
        }
    }
    $psi.Arguments = ($escapedArgs -join ' ')

    $invokeViaCmd = {
        $cmdPsi = New-Object System.Diagnostics.ProcessStartInfo
        $cmdPsi.FileName = "cmd.exe"
        $cmdPsi.WorkingDirectory = $steamCmdDir
        $cmdPsi.UseShellExecute = $false
        $cmdPsi.RedirectStandardOutput = $false
        $cmdPsi.RedirectStandardError = $false
        $cmdPsi.EnvironmentVariables["PATH"] = $minimalPath

        foreach ($varName in @("MSYSTEM", "MSYS", "MSYS2_PATH_TYPE", "MINGW_PREFIX", "CHERE_INVOKING")) {
            if ($cmdPsi.EnvironmentVariables.ContainsKey($varName)) {
                $cmdPsi.EnvironmentVariables.Remove($varName)
            }
        }

        $quotedExe = '"' + ($SteamCmdExe -replace '"', '""') + '"'
        $cmdPsi.Arguments = "/d /c $quotedExe $($psi.Arguments)"

        $cmdProc = New-Object System.Diagnostics.Process
        $cmdProc.StartInfo = $cmdPsi
        [void]$cmdProc.Start()
        $cmdProc.WaitForExit()
        Write-Host "[steam-upload] steamcmd cmd-wrapper exit code: $($cmdProc.ExitCode)"
        return [int]$cmdProc.ExitCode
    }

    $proc = New-Object System.Diagnostics.Process
    $proc.StartInfo = $psi
    try {
        [void]$proc.Start()
    }
    catch {
        if ($_.Exception.Message -like "*Access is denied*") {
            Write-Host "[steam-upload] direct SteamCMD launch was denied; retrying via cmd /c wrapper."
            return & $invokeViaCmd
        }
        throw
    }

    $proc.WaitForExit()
    $nativeExitCode = $proc.ExitCode
    Write-Host "[steam-upload] steamcmd direct exit code: $nativeExitCode"

    if ($nativeExitCode -lt 0) {
        Write-Host "[steam-upload] steamcmd returned negative exit code via direct launch; retrying via cmd /c wrapper."
        return & $invokeViaCmd
    }

    return $nativeExitCode
}

function Resolve-SteamCmdStderrPath {
    param([string]$SteamCmdExe)

    $steamCmdDir = Split-Path -Parent $SteamCmdExe
    $candidate = Join-Path $steamCmdDir "logs\stderr.txt"
    if (Test-Path $candidate) {
        return $candidate
    }

    return $null
}

if ([string]::IsNullOrWhiteSpace($DepotId)) {
    throw "DepotId is required. Example: -DepotId 4767151"
}

$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if ($BuildCandidate) {
    Write-Output "[steam-upload] Building playable candidate first."
    $buildScript = Join-Path $root "scripts/build-steam-playable-candidate.sh"
    if (-not (Test-Path $buildScript)) {
        throw "Required script missing: $buildScript"
    }
    bash $buildScript
}

$contentPath = Join-Path $root $ContentRoot

if (-not (Test-Path $contentPath)) {
    throw "ContentRoot not found: $contentPath"
}

$exePath = Join-Path $contentPath "BananaEngineer.exe"
if (-not (Test-Path $exePath)) {
    throw "Expected executable missing: $exePath"
}

$steamPipeRoot = Join-Path $root ("artifacts/steam-build/" + $BuildId + "/steam-pipe")
$logsDir = Join-Path $steamPipeRoot "logs"
$depotsDir = Join-Path $steamPipeRoot "depots"
New-Item -ItemType Directory -Path $logsDir -Force | Out-Null
New-Item -ItemType Directory -Path $depotsDir -Force | Out-Null

$depotVdfPath = Join-Path $depotsDir ("depot_build_" + $DepotId + ".vdf")
$appVdfPath = Join-Path $steamPipeRoot ("app_build_" + $AppId + ".vdf")

# Steam VDF strings need escaped Windows paths.
$escapedContentPath = $contentPath.Replace("\", "\\")
$escapedLogsPath = $logsDir.Replace("\", "\\")
$escapedDepotVdfPath = $depotVdfPath.Replace("\", "\\")
$buildDesc = "Banana Engineer " + $BuildId + " " + (Get-Date -Format "yyyy-MM-dd HH:mm:ss")

$depotVdf = @"
"DepotBuildConfig"
{
    "DepotID" "$DepotId"
    "ContentRoot" "$escapedContentPath"
    "FileMapping"
    {
        "LocalPath" "*"
        "DepotPath" "."
        "recursive" "1"
    }
    "FileExclusion" "*.pdb"
}
"@

$appVdf = @"
"AppBuild"
{
    "AppID" "$AppId"
    "Desc" "$buildDesc"
    "BuildOutput" "$escapedLogsPath"
    "ContentRoot" "$escapedContentPath"
    "SetLive" "$Branch"
    "Depots"
    {
        "$DepotId" "$escapedDepotVdfPath"
    }
}
"@

Set-Content -Path $depotVdfPath -Value $depotVdf -Encoding ASCII
Set-Content -Path $appVdfPath -Value $appVdf -Encoding ASCII

Write-Output "[steam-upload] AppID: $AppId"
Write-Output "[steam-upload] DepotID: $DepotId"
Write-Output "[steam-upload] BuildID: $BuildId"
Write-Output "[steam-upload] Branch(SetLive): $Branch"
Write-Output "[steam-upload] ContentRoot: $contentPath"
Write-Output "[steam-upload] Generated: $appVdfPath"
Write-Output "[steam-upload] Generated: $depotVdfPath"

if (-not $Execute) {
    Write-Output "[steam-upload] Dry run complete."
    Write-Output "[steam-upload] Re-run with -Execute -SteamUsername <username> to upload with steamcmd."
    exit 0
}

$steamCmd = Resolve-SteamCmd -ExplicitPath $SteamCmdPath
if (-not $steamCmd) {
    $installRoot = Select-SteamCmdInstallRoot
    $steamCmd = Install-SteamCmd -InstallRoot $installRoot
}

if ([string]::IsNullOrWhiteSpace($SteamUsername)) {
    throw "SteamUsername is required when -Execute is used."
}

Write-Output "[steam-upload] Using steamcmd: $steamCmd"
Write-Output "[steam-upload] Running steamcmd bootstrap preflight."

$bootstrapExitCode = Invoke-SteamCmd -SteamCmdExe $steamCmd -SteamArgs @("+quit")
if ($bootstrapExitCode -ne 0) {
    if ($bootstrapExitCode -eq 0) {
        Write-Output "[steam-upload] Bootstrap succeeded after retry."
    }
}

if ($bootstrapExitCode -lt 0) {
    $alternateCandidates = @(
        "C:\Program Files\SteamCMD\steamcmd.exe",
        "C:\Program Files (x86)\SteamCMD\steamcmd.exe",
        "C:\Program Files (x86)\Steam\steamcmd.exe",
        "C:\Program Files\Steam\steamcmd.exe",
        "C:\steamcmd\steamcmd.exe"
    )

    foreach ($candidate in $alternateCandidates) {
        if (-not (Test-Path $candidate)) {
            continue
        }
        if ($candidate -ieq $steamCmd) {
            continue
        }

        Write-Output "[steam-upload] Trying alternate SteamCMD binary: $candidate"
        $candidateExitCode = Invoke-SteamCmd -SteamCmdExe $candidate -SteamArgs @("+quit")
        if ($candidateExitCode -eq 0) {
            Write-Output "[steam-upload] Alternate SteamCMD bootstrap succeeded; using: $candidate"
            $steamCmd = $candidate
            $bootstrapExitCode = 0
            break
        }

        Write-Output "[steam-upload] Alternate SteamCMD bootstrap failed with exit code: $candidateExitCode"
    }
}

if ($bootstrapExitCode -ne 0) {
    $stderrPath = Resolve-SteamCmdStderrPath -SteamCmdExe $steamCmd
    if ($bootstrapExitCode -eq 7) {
        $details = ""
        if ($stderrPath) {
            $details = " Inspect stderr log: $stderrPath"
        }
        throw "steamcmd bootstrap failed with exit code 7 (offline/update failure). Confirm internet access for Steam servers, proxy/firewall allowlist, and rerun. If needed run steamcmd once in an elevated PowerShell to complete first-time update.${details}"
    }

    if ($bootstrapExitCode -eq -1073741571) {
        $details = ""
        if ($stderrPath) {
            $details = " Inspect stderr log: $stderrPath"
        }
        throw "steamcmd bootstrap failed with exit code -1073741571 (steamcmd process crash). Use the real binary path (for Chocolatey installs: C:\ProgramData\chocolatey\lib\steamcmd\tools\steamcmd.exe) via -SteamCmdPath or STEAMCMD_PATH and retry.${details}"
    }

    throw "steamcmd bootstrap failed with exit code $bootstrapExitCode."
}

Write-Output "[steam-upload] Running upload. Steam Guard may prompt interactively."

$arguments = @(
    "+login", $SteamUsername
)

if (-not [string]::IsNullOrWhiteSpace($SteamPassword)) {
    $arguments += $SteamPassword
}

if (-not [string]::IsNullOrWhiteSpace($SteamGuardCode)) {
    $arguments += "+set_steam_guard_code"
    $arguments += $SteamGuardCode
}

$arguments += "+run_app_build"
$arguments += $appVdfPath
$arguments += "+quit"

$uploadExitCode = Invoke-SteamCmd -SteamCmdExe $steamCmd -SteamArgs $arguments
if ($uploadExitCode -ne 0) {
    $stderrPath = Resolve-SteamCmdStderrPath -SteamCmdExe $steamCmd
    if ($uploadExitCode -eq 7) {
        $details = ""
        if ($stderrPath) {
            $details = " Inspect stderr log: $stderrPath"
        }
        throw "steamcmd upload failed with exit code 7 (offline/update failure). Confirm internet access for Steam servers, proxy/firewall allowlist, and rerun. If needed run steamcmd once in an elevated PowerShell to complete first-time update.${details}"
    }

    throw "steamcmd upload failed with exit code $uploadExitCode."
}

Write-Output "[steam-upload] Upload complete. Verify build appears under Steamworks > App Admin > Builds."
