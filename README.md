# WinHotKeys
Launch specific programs with custom hotkeys.

# Usage
1. Download the latest release from [GitHub Releases](https://github.com/Aetopia/WinHotKeys/releases/latest).
2. Create a new file called `WinHotKeys.ini` in the same directory where `WinHotKeys.exe` is present.
    - Configuration File Reference:    
        Create a new section with the name of the section being the virtual keycode as an integer, this represents the hotkey to be used.
        You must have atleast one modifier key to use the desired hotkey.
        - How to get the desired virtual keycode as an integer?
            1. Find the keycode value of your desired hotkey from this link:<br>
                https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
            2. Paste the hexadecimal value of the hotkey in `PowerShell` and hit enter.
            3. This will return the integer represent of the keycode, use this as the section name.

        |Key|Configration|
        |-|-|
        |`FileName`|`<Path to File>` \| Point to the file to be launched.|
        |`Arguments`|`<File Arguments>` \| Arguments to be passed to the file to be launched.|
        |`ModWin`|`<0: Disabled \| 1: Enabled>` \| Use the Windows key as a hotkey modifier.|
        |`ModAlt`|`<0: Disabled \| 1: Enabled>` \| Use the Alt key as a hotkey modifier.|
        |`ModControl`|`<0: Disabled \| 1: Enabled>` \| Use the Control key as a hotkey modifier.|
        |`ModShift`|`<0: Disabled \| 1: Enabled>` \| Use the Shift key as a hotkey modifier.|
        |`Toggle`|`<0: Disabled \| 1: Enabled>` \| Make the hotkey a toggle.<br>When the hotkey is pressed it will kill any running instances of the file to be launched, if no running instances exist then the file is launched.|
    - Example 

        ```ini
        [69]
        FileName = explorer.exe
        ModWin = 1
        ModAlt = 1
        Toggle = 1

        [67]
        FileName = cmd.exe
        ModWin = 1
        ModAlt = 1
        ```
3. Start the program, this will run in the background.

# Building
1. Install [`GCC`](https://winlibs.com/) & [`UPX`](https://upx.github.io) for optional compression.
2. Run `build.bat`.
