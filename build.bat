@echo off
cd "%~dp0"
gcc -mwindows -s -Os WinMain.c -lwtsapi32 -lpathcch -o WinHotKeys.exe
upx --best --ultra-brute WinHotKeys.exe>nul 2>&1