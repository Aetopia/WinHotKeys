#include <windows.h>
#include <pathcch.h>
#include <shlwapi.h>
#include <wtsapi32.h>
#include <stdio.h>

typedef struct HOTKEYS
{
    WCHAR **szFileNames,
        **szArguments, **szProcessNames;
    UINT *uModifiers;
    UINT *uVk;
    BOOL *bSingleInstance;
} HOTKEYS;

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
    //  return (WCHAR *)lpReturnedString;
}
BOOL IsProcessRunning(WCHAR *lpProcessName)
{
    PWTS_PROCESS_INFOW pProcessInfo = {0};
    DWORD dwCount = 0;
    BOOL bRunning = FALSE;
    if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &pProcessInfo, &dwCount))
        return FALSE;
    for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
        if (wcscmp(lpProcessName, pProcessInfo[dwIndex].pProcessName))
        {
            bRunning = TRUE;
            break;
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
            **szArguments, **szProcessNames;
        UINT *uModifiers;
        UINT *uVks;
        BOOL *bSingleInstances;
    } htHotKeys = {.iCount = 0,
                   .szFileNames = NULL,
                   .szArguments = NULL,
                   .szProcessNames = NULL,
                   .uModifiers = NULL,
                   .uVks = NULL,
                   .bSingleInstances = NULL};

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
        htHotKeys.szFileNames = realloc(htHotKeys.szFileNames, sizeof(WCHAR *) * iCount++);
        htHotKeys.szArguments = realloc(htHotKeys.szArguments, sizeof(WCHAR *) * iCount++);
        htHotKeys.szProcessNames = realloc(htHotKeys.szArguments, sizeof(WCHAR *) * iCount++);
        htHotKeys.uModifiers = realloc(htHotKeys.uModifiers, sizeof(UINT) * iCount++);
        htHotKeys.uVks = realloc(htHotKeys.uVks, sizeof(UINT) * iCount++);
        htHotKeys.bSingleInstances = realloc(htHotKeys.uVks, sizeof(UINT) * iCount++);
        htHotKeys.szFileNames[iCount - 1] = GetPrivateProfileStringAllocW(lpszSection, L"FileName", NULL, lpFileName);
        lpszSection += wcslen(lpszSection) + 1;
    }
    free(lpszSections);

    for (INT iIndex = 0; iIndex < iCount; iIndex++)
    {
        printf("%ls\n", htHotKeys.szFileNames[iIndex]);
    }
    return 0;
}
