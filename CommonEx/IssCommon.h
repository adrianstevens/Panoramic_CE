/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssCommon.h
// Abstract: Common routines accross projects
//  
/***************************************************************************************************************/

#pragma once

#include "IssWnd.h"

enum EnumOSVersion
{
    OS_5 = 0,
    OS_6,
    OS_61,
    OS_614,
    OS_65,
};

HRESULT     DebugOut(TCHAR* szFormat, ...) ;
void        DebugProfile(TCHAR* szFunction, BOOL bStart);
void        DebugOutMemory();
HRESULT     EndModalDialog(HWND hWnd, int iReturn);
BOOL        ListAddString(HWND hWnd, TCHAR* szText);
BOOL        ListSetCurSel(HWND hWnd, int iIndex);
BOOL        ListGetCurSel(HWND hWnd);
BOOL        ListSetItemData(HWND hWnd, int iIndex, LPVOID lpData);
int         ListResetContent(HWND hWnd);
int         GetWindowHeight(HWND hWnd);
BOOL        IsVGA();
BOOL        IsSquare();
BOOL		IsSmartphone();
BOOL        IsRectInRect(RECT& rc1, RECT& rc2);
void        CreateGUIDName(TCHAR* szResult, int iNumChars);
void        CreateGUIDAlpha(TCHAR* szResult, int iNumChars);
HRESULT     GetExeDirectory(TCHAR* szDir);
HRESULT     GetInstallDirectory(TCHAR* szDestDir, TCHAR* szAppName);
BOOL	    LaunchEula(TCHAR* szKey);
BOOL	    LaunchHelp(TCHAR* szHelp, HINSTANCE hInst);
HRESULT     SetupListControl( UINT uiListControl, 
                          HWND hWndParent, 
                          HINSTANCE hInstance,
                          UINT uiStartEntry,
                          UINT uiEndEntry,
                          int  iSelectedItem,
                          int  iIndexFromBottom,
                          UINT uiSpinControl);

HRESULT    MoveListControl(UINT uiListControl, 
                         HWND hWndParent, 
                         int  iIndexFromBottom,
                         UINT uiSpinControl);

HWND       CreateSoftKey( HWND hWndParent,
                       HINSTANCE hInst,
                       UINT uiIdCommand1=0,
                       UINT uiIdString1=0,
                       UINT uiIdCommand2=0,
                       UINT uiIdString2=0);

int         GetMenuHeight();

BOOL        IsFileExists(TCHAR* szPath);

DWORD       GetFileSize(TCHAR* szFile);


HINSTANCE   LoadStringResources(HINSTANCE hInst);

HRESULT     SystemTimeToUtc(SYSTEMTIME sysTime, int& iTime, BOOL bIsLocal = FALSE);
HRESULT     UtcToSystemTime(int iTime, SYSTEMTIME& sysTime);

BOOL	    ShouldAbort(HWND hWnd);

EnumOSVersion GetOSVersion();
