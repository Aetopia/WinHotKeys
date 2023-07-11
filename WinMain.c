#include <windows.h>
#include <pathcch.h>
#include <wtsapi32.h>
#include <shlwapi.h>

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

BOOL IsProcessSingleInstanceW(WCHAR *lpProcessName)
{
    WTS_PROCESS_INFOW *pProcessInfo = NULL;
    DWORD dwCount = 0;
    BOOL bRunning = FALSE;
    HANDLE hProcess = 0;
    WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &pProcessInfo, &dwCount);
    for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
        if (!wcscmp(lpProcessName, _wcslwr(pProcessInfo[dwIndex].pProcessName)))
        {
            bRunning = TRUE;
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pProcessInfo[dwIndex].ProcessId);
            TerminateProcess(hProcess, 1);
            CloseHandle(hProcess);
        }
    }
    WTSFreeMemory(pProcessInfo);
    return bRunning;
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
            **szArguments,
            **szProcessNames;
        BOOL *bToggles;
        UINT *uModifiers,
            *uVks;
    } htHotKeys = {.iCount = 0,
                   .szFileNames = NULL,
                   .szArguments = NULL,
                   .uModifiers = NULL,
                   .szProcessNames = NULL,
                   .bToggles = NULL,
                   .uVks = NULL};

    wcscpy(lpFileName, pszArgvW[0]);
    PathCchRemoveFileSpec(lpFileName, wcslen(lpFileName));
    wcscat(lpFileName, L"\\WinHotKeys.ini");

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
        htHotKeys.szProcessNames = realloc(htHotKeys.szProcessNames, sizeof(WCHAR *) * htHotKeys.iCount);
        htHotKeys.bToggles = realloc(htHotKeys.bToggles, sizeof(BOOL) * htHotKeys.iCount);
        htHotKeys.uModifiers = realloc(htHotKeys.uModifiers, sizeof(UINT) * htHotKeys.iCount);
        htHotKeys.uVks = realloc(htHotKeys.uVks, sizeof(UINT) * htHotKeys.iCount);

        htHotKeys.szFileNames[htHotKeys.iCount - 1] = _wcslwr(GetPrivateProfileStringAllocW(lpszSection, L"FileName", L"\0", lpFileName));
        htHotKeys.szProcessNames[htHotKeys.iCount - 1] = malloc(sizeof(WCHAR) * (wcslen(htHotKeys.szFileNames[htHotKeys.iCount - 1]) + 1));
        wcscpy(htHotKeys.szProcessNames[htHotKeys.iCount - 1], htHotKeys.szFileNames[htHotKeys.iCount - 1]);
        PathStripPathW(htHotKeys.szProcessNames[htHotKeys.iCount - 1]);
        htHotKeys.szArguments[htHotKeys.iCount - 1] = GetPrivateProfileStringAllocW(lpszSection, L"Arguments", L"\0", lpFileName);
        htHotKeys.bToggles[htHotKeys.iCount - 1] = GetPrivateProfileIntW(lpszSection, L"Toggle", 0, lpFileName);
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

        htHotKeys.uVks[htHotKeys.iCount - 1] = wcslen(lpszSection) ? VkKeyScanW(_wcslwr(lpszSection)[0]) : -1;
        if (htHotKeys.uModifiers[htHotKeys.iCount - 1] ||
            htHotKeys.uVks[htHotKeys.iCount - 1] != -1)
            RegisterHotKey(0, htHotKeys.iCount, htHotKeys.uModifiers[htHotKeys.iCount - 1] | MOD_NOREPEAT, htHotKeys.uVks[htHotKeys.iCount - 1]);
        lpszSection += wcslen(lpszSection) + 1;
    }
    free(lpszSections);
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (msg.message != WM_HOTKEY)
            continue;
        UINT uModifiers = LOWORD(msg.lParam),
             uVk = HIWORD(msg.lParam);

        for (INT iIndex = 0; iIndex < htHotKeys.iCount; iIndex++)
            if (htHotKeys.uModifiers[iIndex] == uModifiers &&
                htHotKeys.uVks[iIndex] == uVk)
                if (htHotKeys.bToggles[iIndex] ? !IsProcessSingleInstanceW(htHotKeys.szFileNames[iIndex]) : TRUE)
                    ShellExecuteW(NULL, NULL, htHotKeys.szFileNames[iIndex], htHotKeys.szArguments[iIndex], NULL, SW_SHOWNORMAL);
    }
    return 0;
}
