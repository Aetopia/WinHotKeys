#include <windows.h>
#include <pathcch.h>
#include <stdio.h>

typedef struct HOTKEY
{
    WCHAR *szFileName, *szArguments, *szProcess;
    INT iHotKey;
    BOOL bSingleInstance;
} HOTKEY;

WCHAR *GetPrivateProfileStringAllocW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileName)
{
    SIZE_T nSize = 0;
    WCHAR *lpReturnedString = NULL;
    do
    {
        lpReturnedString = realloc(lpReturnedString, sizeof(WCHAR) * nSize++);
        GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    } while (GetLastError());
    return lpReturnedString;
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    INT iNumArgs = 0,
        nCount = 0;
    SIZE_T nSize = 0;
    WCHAR **pszArgvW = CommandLineToArgvW(GetCommandLineW(), &iNumArgs),
          lpFileName[MAX_PATH],
          *lpszSection = NULL,
          *lpszSections = NULL,
          *lpReturnedString = NULL;
    HOTKEY *phtHotKeys = NULL;

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
        nSize = 0;
        phtHotKeys = realloc(phtHotKeys, sizeof(HOTKEY *) * nCount++);
        phtHotKeys[nCount - 1].szFileName = GetPrivateProfileStringAllocW(lpszSection, L"FileName", NULL, lpFileName);
        phtHotKeys[nCount - 1].szArguments = GetPrivateProfileStringAllocW(lpszSection, L"Arguments", NULL, lpFileName);
        memcpy(&phtHotKeys[nCount - 1].iHotKey, &((INT){GetPrivateProfileIntW(lpszSection, L"HotKey", 0, lpFileName)}), sizeof(INT));
        memcpy(&phtHotKeys[nCount - 1].bSingleInstance, &((BOOL){GetPrivateProfileIntW(lpszSection, L"SingleInstance", FALSE, lpFileName)}), sizeof(BOOL));
        lpszSection += wcslen(lpszSection) + 1;
    }
    free(lpszSections);
    return 0;
}
