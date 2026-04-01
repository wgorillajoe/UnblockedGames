param(
    [Parameter(Mandatory = $true)]
    [string]$AppName,

    [Parameter(Mandatory = $true)]
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

function Stop-LockingProcesses {
    param([string[]]$ProcessNames)

    foreach ($name in $ProcessNames) {
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

    Write-Log "Launching installer: $Path"
    $proc = Start-Process -FilePath $Path -ArgumentList '--install', '.' -Wait -PassThru
    return $proc.ExitCode
}

$appRoot = Join-Path $LocalAppDataPath $AppName
$packagesDir = Join-Path $appRoot 'packages'
$knownProcesses = @(
    $AppName,
    'Update',
    'Squirrel'
) + $ExtraProcessNames

Write-Log "AppName: $AppName"
Write-Log "InstallerPath: $InstallerPath"
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
        $exitCode = Invoke-SquirrelInstall -Path $InstallerPath
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
