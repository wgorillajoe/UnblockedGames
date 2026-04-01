@echo off
REM Build a.exe with MinGW g++
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o a.exe main.cpp
if %errorlevel% neq 0 (
  echo Build failed.
  exit /b %errorlevel%
)
echo Build complete: a.exe
