# WinHotKeys
Launch specific programs with custom hotkeys.

# Usage
1. Download the latest release.
2. Create a new file called `WinHotKeys.ini` in the same directory where `WinHotKeys.exe` is present.
    - Configuration File Reference:    
        Create a new section with the character to be used a hotkey.
        |Key|Configration|
        |-|-|
        |`FileName`|`<Path to File>` \| Point to the file to be launched.|
        |`ModWin`|`<0: Disabled \| 1: Enabled>` \| Use the Windows key as a hotkey modifier.|
        |`ModAlt`|`<0: Disabled \| 1: Enabled>` \| Use the Alt key as a hotkey modifier.|
        |`ModControl`|`<0: Disabled \| 1: Enabled>` \| Use the Control key as a hotkey modifier.|
        |`ModShift`|`<0: Disabled \| 1: Enabled>` \| Use the Shift key as a hotkey modifier.|
        |`Toggle`|`<0: Disabled \| 1: Enabled>` \| Make the hotkey a toggle.<br>When theh hotkey is pressed it will kill any running instances of the file to be launched, if no running instances exist then the file is launched.|
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
