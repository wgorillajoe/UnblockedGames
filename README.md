# Auto Squirrel Installer Retry

This script automatically retries Squirrel-based installers when files are locked by another process.

## File

- `auto-squirrel-install.ps1`

## What it does

- Stops likely lock-holder processes (`<AppName>`, `Update`, `Squirrel`, plus any extras you pass in).
- Runs the installer with Squirrel-style arguments (`--install .`).
- Retries up to `MaxAttempts` with a delay.
- Exits `0` on success and `1` on failure.

## Usage

```powershell
powershell -ExecutionPolicy Bypass -File .\auto-squirrel-install.ps1 `
  -AppName "VortxEngine" `
  -InstallerPath "C:\Path\To\Update.exe" `
  -MaxAttempts 6 `
  -RetryDelaySeconds 8 `
  -ExtraProcessNames @("SignalRgbLauncher")
```

## Notes

- Run from an elevated shell if your installer requires admin rights.
- If antivirus keeps locking package files, consider excluding `%LocalAppData%\<AppName>` temporarily.
