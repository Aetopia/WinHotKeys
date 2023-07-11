# WinHotKeys
Launch specific programs with custom hotkeys.

# Usage
1. Download the latest release.
2. Create a new file called `WinHotKeys.ini` in the same directory where `WinHotKeys.exe` is present.
    - Configuration File Reference:
        Create a new section with the character to be used a hotkey.
        |Key|Configration|
        |-|-|
        |FileName|Point to the file to be launched.|
        |ModWin|Use the Windows key as a hotkey modifier.|
        |ModAlt|Use the Alt key as a hotkey modifier.|
        |ModControl|Use the Control key as a hotkey modifier.|
        |ModShift|Use the Shift key as a hotkey modifier.|
    - Example
        ```ini
        [e]
        FileName = explorer.exe
        ModWin = 1
        ModAlt = 1
        Toggle = 1

        [c]
        FileName = cmd.exe
        ModWin = 1
        ModAlt = 1
        ```
3. Start the program, this will run in the background.

# Building
1. Install [`GCC`](https://winlibs.com/) & [`UPX`](https://upx.github.io) for optional compression.
2. Run `build.bat`.
