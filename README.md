# Auto Squirrel Installer Retry

This script automatically retries Squirrel-based installers when files are locked by another process.

## File

- `auto-squirrel-install.ps1`

## Important first step

Your error happened because PowerShell was run from a folder that **did not contain** `auto-squirrel-install.ps1`.

Use one of these patterns:

1. `cd` into the folder containing the script, then run it.
2. Or pass the **full absolute path** to `-File`.

## Quick usage (recommended)

If `Update.exe` is in your current folder, AppName and InstallerPath are auto-detected:

```powershell
powershell -ExecutionPolicy Bypass -File C:\FULL\PATH\TO\auto-squirrel-install.ps1 -MaxAttempts 6 -RetryDelaySeconds 8 -ExtraProcessNames SignalRgbLauncher
```

## Explicit usage

```powershell
powershell -ExecutionPolicy Bypass -File C:\FULL\PATH\TO\auto-squirrel-install.ps1 -AppName VortxEngine -InstallerPath C:\FULL\PATH\TO\Update.exe -MaxAttempts 6 -RetryDelaySeconds 8 -ExtraProcessNames SignalRgbLauncher
```

## What it does

- Stops likely lock-holder processes (`<AppName>`, `Update`, `Squirrel`, plus any extras you pass in).
- Runs the installer with Squirrel-style arguments (`--install .`) from the installer's own folder.
- Retries up to `MaxAttempts` with a delay.
- Exits `0` on success and `1` on failure.

## Notes

- Run from an elevated shell if your installer requires admin rights.
- If antivirus keeps locking package files, consider excluding `%LocalAppData%\<AppName>` temporarily.
