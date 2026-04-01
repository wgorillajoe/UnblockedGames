# Wheel and Shifter Test Utility

This project provides a simple console program to test:
- **Thrustmaster T248** (wheel)
- **Logitech G Shifter**

It also lets you change and save settings.

## Build (Windows)

Use MinGW g++:

```bat
build_windows.bat
```

This creates `a.exe`.

## Run

From command prompt:

```bat
a.exe --no-pause
```

For double-click usage (prevents instant close):

```bat
run_a.bat
```

## Features

- Menu-driven tool
- Change steering, force feedback, brake/clutch sensitivity
- Toggle invert pedals and sequential mode
- Run separate tests for wheel and shifter
- Save/load settings from `wheel_settings.cfg`


> Tip: If you launch `a.exe` directly by double-clicking it, the program now waits for Enter before closing.
