@echo off
setlocal

echo [1/3] Checking Python...
python --version >nul 2>&1
if errorlevel 1 (
  echo Python is not installed or not in PATH.
  echo Install from https://www.python.org/downloads/
  pause
  exit /b 1
)

echo [2/3] Installing/updating dependencies...
python -m pip install -r requirements.txt
if errorlevel 1 (
  echo Failed to install dependencies.
  pause
  exit /b 1
)

echo [3/3] Running guild leaver...
python main.py

echo.
echo Finished. Press any key to close.
pause >nul
endlocal
