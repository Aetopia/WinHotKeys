@echo off
cd "%~dp0"
gcc -mwindows -s -Os WinMain.c -o WinHotKeys.exe
upx --best --ultra-brute WinHotKeys.exe>nul 2>&1