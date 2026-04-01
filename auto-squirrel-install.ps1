param(
    [string]$AppName,

    [string]$InstallerPath,

    [int]$MaxAttempts = 5,
    [int]$RetryDelaySeconds = 6,

    [string]$LocalAppDataPath = $env:LOCALAPPDATA,

    [string[]]$ExtraProcessNames = @()
)

$ErrorActionPreference = 'Stop'

function Write-Log {
    param([string]$Message)
    $ts = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
    Write-Host "[$ts] $Message"
}

function Resolve-InstallerPath {
    param([string]$InputPath)

    if ($InputPath) {
        return (Resolve-Path -Path $InputPath).Path
    }

    $candidates = @(
        (Join-Path (Get-Location).Path 'Update.exe'),
        (Join-Path $PSScriptRoot 'Update.exe')
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return (Resolve-Path -Path $candidate).Path
        }
    }

    throw "Installer not found. Pass -InstallerPath or place Update.exe in the current/script directory."
}

function Resolve-AppName {
    param(
        [string]$ProvidedAppName,
        [string]$ResolvedInstallerPath
    )

    if ($ProvidedAppName) {
        return $ProvidedAppName
    }

    $parent = Split-Path -Path (Split-Path -Path $ResolvedInstallerPath -Parent) -Leaf
    if (-not [string]::IsNullOrWhiteSpace($parent)) {
        return $parent
    }

    throw 'Could not determine AppName. Pass -AppName explicitly.'
}

function Stop-LockingProcesses {
    param([string[]]$ProcessNames)

    foreach ($name in $ProcessNames | Select-Object -Unique) {
        if ([string]::IsNullOrWhiteSpace($name)) {
            continue
        }

        try {
            $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
            if ($null -ne $procs) {
                foreach ($p in $procs) {
                    Write-Log "Stopping process: $($p.ProcessName) (PID $($p.Id))"
                    Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue
                }
            }
        }
        catch {
            Write-Log "Warning: could not stop process '$name' : $($_.Exception.Message)"
        }
    }
}

function Invoke-SquirrelInstall {
    param([string]$Path)

    if (-not (Test-Path $Path)) {
        throw "Installer not found: $Path"
    }

    $workingDir = Split-Path -Path $Path -Parent
    Write-Log "Launching installer: $Path"
    $proc = Start-Process -FilePath $Path -ArgumentList '--install', '.' -WorkingDirectory $workingDir -Wait -PassThru
    return $proc.ExitCode
}

$resolvedInstallerPath = Resolve-InstallerPath -InputPath $InstallerPath
$resolvedAppName = Resolve-AppName -ProvidedAppName $AppName -ResolvedInstallerPath $resolvedInstallerPath

$appRoot = Join-Path $LocalAppDataPath $resolvedAppName
$packagesDir = Join-Path $appRoot 'packages'
$knownProcesses = @(
    $resolvedAppName,
    'Update',
    'Squirrel'
) + $ExtraProcessNames

Write-Log "AppName: $resolvedAppName"
Write-Log "InstallerPath: $resolvedInstallerPath"
Write-Log "App root: $appRoot"
Write-Log "Max attempts: $MaxAttempts"

for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    Write-Log "Attempt $attempt of $MaxAttempts"

    Stop-LockingProcesses -ProcessNames $knownProcesses

    if (Test-Path $packagesDir) {
        try {
            Write-Log "Touching packages directory to verify accessibility: $packagesDir"
            Get-ChildItem -Path $packagesDir -ErrorAction Stop | Out-Null
        }
        catch {
            Write-Log "Packages directory busy/unavailable: $($_.Exception.Message)"
        }
    }

    try {
        $exitCode = Invoke-SquirrelInstall -Path $resolvedInstallerPath
        if ($exitCode -eq 0) {
            Write-Log 'Install completed successfully.'
            exit 0
        }

        Write-Log "Installer exited with code $exitCode"
    }
    catch {
        Write-Log "Install attempt failed: $($_.Exception.Message)"
    }

    if ($attempt -lt $MaxAttempts) {
        Write-Log "Waiting $RetryDelaySeconds seconds before retry..."
        Start-Sleep -Seconds $RetryDelaySeconds
    }
}

Write-Log 'Install failed after all retry attempts.'
exit 1
