#include <windows.h>
#include <pathcch.h>
#include <wtsapi32.h>

WCHAR *GetPrivateProfileStringAllocW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileName)
{
    SIZE_T nSize = 0;
    WCHAR *lpReturnedString = NULL;
    do
    {
        lpReturnedString = realloc(lpReturnedString, sizeof(WCHAR) * nSize++);
        GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    } while (GetLastError() == ERROR_MORE_DATA);
    return lpReturnedString;
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    INT iNumArgs = 0,
        iCount = 0;
    DWORD nSize = 0;
    WCHAR **pszArgvW = CommandLineToArgvW(GetCommandLineW(), &iNumArgs),
          lpFileName[MAX_PATH],
          *lpszSection = NULL,
          *lpszSections = NULL,
          *lpReturnedString = NULL;
    MSG msg = {0};
    struct HOTKEYS
    {
        INT iCount;
        WCHAR **szFileNames,
            **szArguments;
        UINT *uModifiers,
            *uVks;
    } htHotKeys = {.iCount = 0,
                   .szFileNames = NULL,
                   .szArguments = NULL,
                   .uModifiers = NULL,
                   .uVks = NULL};

    wcscpy(lpFileName, pszArgvW[0]);
    PathCchRemoveFileSpec(lpFileName, wcslen(lpFileName));
    wcscat(lpFileName, L"\\WinHotKeys.ini");

    RegisterHotKey(NULL, 1, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x45);
    RegisterHotKey(NULL, 2, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x53);
    RegisterHotKey(NULL, 3, MOD_WIN | MOD_ALT, 0x43);
    RegisterHotKey(NULL, 4, MOD_WIN | MOD_ALT, 0x50);

    do
    {
        nSize++;
        lpszSections = realloc(lpszSections, sizeof(WCHAR) * nSize);
        GetPrivateProfileSectionNamesW(lpszSections, nSize, lpFileName);
    } while (GetLastError() == ERROR_MORE_DATA);

    lpszSection = lpszSections;
    while (*lpszSection)
    {
        htHotKeys.iCount++;
        htHotKeys.szFileNames = realloc(htHotKeys.szFileNames, sizeof(WCHAR *) * htHotKeys.iCount);
        htHotKeys.szArguments = realloc(htHotKeys.szArguments, sizeof(WCHAR *) * htHotKeys.iCount);
        htHotKeys.szProcessNames = realloc(htHotKeys.szArguments, sizeof(WCHAR *) * htHotKeys.iCount);
        htHotKeys.uModifiers = realloc(htHotKeys.uModifiers, sizeof(UINT) * htHotKeys.iCount);
        htHotKeys.uVks = realloc(htHotKeys.uVks, sizeof(UINT) * htHotKeys.iCount);
        htHotKeys.szFileNames[htHotKeys.iCount - 1] = wcslwr(GetPrivateProfileStringAllocW(lpszSection, L"FileName", L"\0", lpFileName));
        htHotKeys.szArguments[htHotKeys.iCount - 1] = GetPrivateProfileStringAllocW(lpszSection, L"Arguments", L"", lpFileName);
        htHotKeys.uModifiers[htHotKeys.iCount - 1] = (GetPrivateProfileIntW(lpszSection, L"ModAlt", 0, lpFileName)
                                                          ? MOD_ALT
                                                          : 0) |
                                                     (GetPrivateProfileIntW(lpszSection, L"ModWin", 0, lpFileName)
                                                          ? MOD_WIN
                                                          : 0) |
                                                     (GetPrivateProfileIntW(lpszSection, L"ModShift", 0, lpFileName)
                                                          ? MOD_SHIFT
                                                          : 0) |
                                                     (GetPrivateProfileIntW(lpszSection, L"ModControl", 0, lpFileName)
                                                          ? MOD_CONTROL
                                                          : 0);
        lpReturnedString = GetPrivateProfileStringAllocW(lpszSection, L"HotKey", 0, lpFileName);
        htHotKeys.uVks[htHotKeys.iCount - 1] = wcslen(lpReturnedString) ? VkKeyScanW(lpReturnedString[0]) : -1;
        free(lpReturnedString);
        if (htHotKeys.uModifiers[htHotKeys.iCount - 1] ||
            htHotKeys.uVks[htHotKeys.iCount - 1] != -1)
            RegisterHotKey(0, htHotKeys.iCount + 4, htHotKeys.uModifiers[htHotKeys.iCount - 1] | MOD_NOREPEAT, htHotKeys.uVks[htHotKeys.iCount - 1]);
        lpszSection += wcslen(lpszSection) + 1;
    }
    free(lpszSections);
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (msg.message != WM_HOTKEY)
            continue;
        UINT uModifiers = LOWORD(msg.lParam),
             uVk = HIWORD(msg.lParam);

        if (uModifiers == MOD_WIN | MOD_ALT)
            switch (uVk)
            {
            case 0x45:
                HWND hWnd = 0;
                DWORD dwProcessId = 0;
                HANDLE hProcess = 0;
                if (!(hWnd = GetShellWindow()))
                    ShellExecuteW(NULL, NULL, L"C:\\Windows\\explorer.exe", NULL, L"C:\\Windows", SW_SHOWNORMAL);
                else
                {
                    GetWindowThreadProcessId(hWnd, &dwProcessId);
                    hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
                    TerminateProcess(hProcess, 1);
                    CloseHandle(hProcess);
                }
                break;
            case 0x53:
                ShellExecuteW(NULL, NULL, L"C:\\Windows\\System32\\snippingtool.exe", L"/clip", L"C:\\Windows\\System32", SW_SHOWNORMAL);
                break;

            case 0x43:
                ShellExecuteW(NULL, NULL, L"C:\\Windows\\System32\\cmd.exe", NULL, L"C:\\Windows\\System32", SW_SHOWNORMAL);
                break;

            case 0x50:
                ShellExecuteW(NULL, NULL, L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", NULL, L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0", SW_SHOWNORMAL);
                break;
            }
        for (INT iIndex = 0; iIndex < htHotKeys.iCount; iIndex++)
            if (htHotKeys.uModifiers[iIndex] == uModifiers &&
                htHotKeys.uVks[iIndex] == uVk)
                ShellExecuteW(NULL, NULL, htHotKeys.szFileNames[iIndex], htHotKeys.szArguments[iIndex], NULL, SW_SHOWNORMAL);
    }
    return 0;
}
