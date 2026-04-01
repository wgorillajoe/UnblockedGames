@echo off
REM Run a.exe and keep terminal open for double-click usage
if not exist a.exe (
  echo a.exe was not found. Build first with build_windows.bat
  pause
  exit /b 1
)

a.exe
pause
