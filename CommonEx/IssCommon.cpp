/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssCommon.cpp
// Abstract: Common routines accross projects
//  
/***************************************************************************************************************/

#include "IssCommon.h"
#include "IssString.h"
#define INITGUID
#include <initguid.h>
#undef INITGUID

#include "IssDebug.h"
#include "IssRegistry.h"
#include <tlhelp32.h>  // for process info

#ifndef UNDER_CE
#include "stdio.h"
#endif

#define DEBUG_FILE              FALSE   // if you want to debug out to file

EnumOSVersion GetOSVersion()
{
    OSVERSIONINFO versionInfo;
    ::GetVersionEx(&versionInfo);

    if(versionInfo.dwBuildNumber >= 21234)
        return OS_65;
    else if(versionInfo.dwBuildNumber >= 20757)
        return OS_614;
    else if(versionInfo.dwBuildNumber >= 19202)
        return OS_61;
    else if(versionInfo.dwBuildNumber >= 17740)
        return OS_6;
    else
        return OS_5;
}

void CreateGUIDName(TCHAR* szResult, int iNumChars)
{
    if(!szResult || iNumChars == 0 || iNumChars > 33)
        return;

    // seed the seed of randomness
    srand( GetTickCount() );

    int iVal;

    // we create our own GUID here
    memset(szResult, 0, sizeof(TCHAR)*iNumChars+2);
    for(int i=0; i<iNumChars; i++)
    {
        // pick a number between 0 and 61
        iVal = (int)((double)rand() / (RAND_MAX + 1) * 62);
        if(iVal < 10)
            iVal += 48;
        else if(iVal < 36)
            iVal += (65 - 10);
        else
            iVal += (97 - 36);

        szResult[i] = iVal;
    }
}

void CreateGUIDAlpha(TCHAR* szResult, int iNumChars)
{
    if(!szResult || iNumChars == 0 || iNumChars > 33)
        return;

    // seed the seed of randomness
    srand( GetTickCount() );

    int iVal;

    // we create our own GUID here
    memset(szResult, 0, sizeof(TCHAR)*iNumChars+2);
    for(int i=0; i<iNumChars; i++)
    {
        // pick a number between 0 and 9
        iVal = (int)((double)rand() / (RAND_MAX + 1) * 10);
        iVal += 48;
        szResult[i] = iVal;
    }
}

BOOL IsFileExists(TCHAR* szPath)
{
    return (UINT)GetFileAttributes(szPath) != (UINT)-1;
}

BOOL IsSmartphone()
{
	BOOL bIsSmartphone = FALSE;

#ifdef UNDER_CE
	CIssString* oStr = CIssString::Instance();
	// check if we're using smartphone
	TCHAR szPlatform[STRING_NORMAL];
	ZeroMemory(szPlatform, sizeof(TCHAR)*STRING_NORMAL);
	if(SystemParametersInfo(SPI_GETPLATFORMTYPE, STRING_NORMAL, szPlatform, 0))
	{
		if(0 == oStr->Compare(szPlatform, _T("SmartPhone")))
		{
			bIsSmartphone = TRUE;
		}
	}
#endif
	return bIsSmartphone;
}

DWORD GetFileSize(TCHAR* szFile)
{
    FILE* fp;

    //open the file
    if((fp = _tfopen(szFile, _T("rb"))) == NULL)
        return 0;

    //read file size
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fclose(fp);

    return dwFileSize;
}


/********************************************************************
Function        EndModalDialog

Arguments:      hWnd            - dialog to close
                iReturn         - return value

Returns:        

Comments:       close a modal dialog
*********************************************************************/
HRESULT EndModalDialog(HWND hWnd, int iReturn)
{
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
    EndDialog(hWnd, iReturn);
#elif SOLEUS
    FxEndDialog(hWnd, iReturn);
#endif
    return S_OK;
}

/********************************************************************
Function        ListAddString

Arguments:      hWnd    - the control
                        szText  - text to add to the control

Returns:        

Comments:       Add a string to a list (or combo box) control
*********************************************************************/
BOOL ListAddString(HWND hWnd, TCHAR* szText)
{
    return (BOOL)SendMessage(hWnd, 
#ifdef WIN32_PLATFORM_PSPC
    CB_ADDSTRING, 
#else
    LB_ADDSTRING,
#endif
    0, (LPARAM)szText);

}

/********************************************************************
Function        ListSetItemData

Arguments:      hWnd    - the control
                        iIndex  - index in the list
                        lpData  - data value to save

Returns:        

Comments:       Set the data in the control according to the index given
*********************************************************************/
BOOL ListSetItemData(HWND hWnd, int iIndex, LPVOID lpData)
{
    return (BOOL)SendMessage(hWnd, 
#ifdef WIN32_PLATFORM_PSPC
    CB_SETITEMDATA, 
#else
    LB_SETITEMDATA,
#endif
    iIndex, (LPARAM)lpData);
}

/********************************************************************
Function        ListSetCurSel

Arguments:      hWnd    - the control
                        iIndex  - current index to set

Returns:        

Comments:       Set the current index in the control list
*********************************************************************/
BOOL ListSetCurSel(HWND hWnd, int iIndex)
{
    return (BOOL)SendMessage(hWnd, 
#ifdef WIN32_PLATFORM_PSPC
    CB_SETCURSEL, 
#else
    LB_SETCURSEL,
#endif
    (WPARAM)iIndex, 0);
}

/********************************************************************
Function        ListGetCurSel

Arguments:      hWnd - the control

Returns:        int - the current index

Comments:       Retrieve the current index in the list
*********************************************************************/
int ListGetCurSel(HWND hWnd)
{
    return (BOOL)SendMessage(hWnd, 
#ifdef WIN32_PLATFORM_PSPC
    CB_GETCURSEL, 
#else
    LB_GETCURSEL,
#endif
    0, 0);
}

/********************************************************************
Function        ListResetContent

Arguments:      hWnd - the control

Returns:        

Comments:       Reset all the content in the list (ie. delete everything)
*********************************************************************/
int ListResetContent(HWND hWnd)
{
    return (BOOL)SendMessage(hWnd, 
#ifdef WIN32_PLATFORM_PSPC
    CB_RESETCONTENT, 
#else
    LB_RESETCONTENT,
#endif
    0, 0);
}

/********************************************************************
Function        MoveListControl

Arguments:      uiListControl   - resource of the list control
                        hWndParent              - parent window
                        iIndexFromBottom- index from the bottom of the screen
                        uiSpinControl   - optional: resource of the spin control (only smartphone)

Returns:        

Comments:       This function lines up multiple list controls in a row starting from
                        the bottom.  We use this mostly in our options screens for games
*********************************************************************/
HRESULT MoveListControl(UINT uiListControl, 
                                         HWND hWndParent, 
                                         int  iIndexFromBottom,
                                         UINT uiSpinControl)
{
    HRESULT hr = S_OK;

    RECT rcClient;

    GetClientRect(hWndParent, &rcClient);

    // set up the Starting level list box and move it to the bottom of the screen
    HWND hWndList = GetDlgItem(hWndParent, uiListControl);
    

    //      int iXOffset = (rcClient.right - rcList.right + rcList.left)/2;
    int indent      = GetSystemMetrics(SM_CXICON)/8;
    int iHeight     = GetSystemMetrics(SM_CXICON)*5/8-1;
    int iWidth      = GetSystemMetrics(SM_CXSCREEN) - 2*indent;
    
    int iYOffset = rcClient.bottom - (indent + iIndexFromBottom*iHeight);
    
    MoveWindow(hWndList, 
            indent,
            iYOffset,
            iWidth,
#ifdef WIN32_PLATFORM_WFSP
            iHeight,
#else
            5*iHeight,
#endif
            FALSE);

#ifdef WIN32_PLATFORM_WFSP
        RECT rcSpin;
        HWND hWndSpin = GetDlgItem(hWndParent, uiSpinControl);
        GetClientRect(hWndSpin, &rcSpin);
        SetWindowPos(hWndSpin, 
                HWND_TOPMOST,
                GetSystemMetrics(SM_CXSCREEN) - indent - (rcSpin.right - rcSpin.left),
                iYOffset,
                rcSpin.right - rcSpin.left,
                iHeight,
                SWP_SHOWWINDOW);
#endif
    return hr;
}

/********************************************************************
Function        SetupListControl

Arguments:      uiListControl   - resource of the list control
                hWndParent      - parent window
                hInstance       - instance of the resources
                uiStartEntry    - starting resource in the string table to put into the control
                uiEndEntry      - ending resource in the string table to put into the control
                iSelectedItem   - current item to select
                iIndexFromBottom- index from the bottom of the screen
                uiSpinControl   - optional: resource of the spin control (only smartphone)

Returns:        

Comments:       Sets up a list control.  Used mostly in our games in the options screen.
*********************************************************************/
HRESULT SetupListControl(UINT uiListControl, 
                      HWND hWndParent, 
                      HINSTANCE hInstance,
                      UINT uiStartEntry,
                      UINT uiEndEntry,
                      int  iSelectedItem,
                      int  iIndexFromBottom,
                      UINT uiSpinControl)
{
    HRESULT hr = S_OK;

    CHHRE(hWndParent, _T("Invalid parent hwnd"), E_INVALIDARG);

    CIssString* oStr = CIssString::Instance();
    TCHAR szText[STRING_LARGE];

    MoveListControl(uiListControl, hWndParent, iIndexFromBottom, uiSpinControl);

    HWND hWndList = GetDlgItem(hWndParent, uiListControl);

    CHHRE(hWndList, _T("Invalid hwnd"), E_ABORT);

    ListResetContent(hWndList);
    for(UINT i=uiStartEntry; i<= uiEndEntry; i++)
    {
        oStr->StringCopy(szText, i, STRING_LARGE, hInstance);
        
        ListAddString(hWndList, szText);
    }
    ListSetCurSel(hWndList, iSelectedItem);

Error:
    return hr;
}

/********************************************************************
Function        GetWindowHeight

Arguments:      hWnd - window to retrieve the height

Returns:        

Comments:       This is a useful function to retrieve the true height of
                        the provided window.  It will calculate the total height
                        minue the heights of the two menu bars.
*********************************************************************/
int GetWindowHeight(HWND hWnd)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);

    return GetSystemMetrics(SM_CYSCREEN) - 2*rc.top;
}

/********************************************************************
Function        IsVGA

Arguments:      

Returns:        

Comments:       do we have VGA resolution?
*********************************************************************/
BOOL IsVGA()
{
#ifdef UNDER_CE
    return  (GetSystemMetrics(SM_CXICON) > 44);
#else
    return TRUE;
#endif
}

/********************************************************************
Function        IsSquare

Arguments:      

Returns:        

Comments:       is this square screen?
*********************************************************************/
BOOL IsSquare()
{
    return  (GetDeviceCaps(NULL, HORZRES) == GetDeviceCaps(NULL, VERTRES));
}

HRESULT GetInstallDirectory(TCHAR* szDestDir, TCHAR* szAppName)
{
    HRESULT hr = S_OK;
    CIssString* oStr = CIssString::Instance();
    TCHAR szKey[STRING_MAX];

    ZeroMemory(szKey, sizeof(TCHAR)*STRING_MAX);

    CBARG(szDestDir && szAppName, _T(""));

    oStr->Empty(szDestDir);
    oStr->StringCopy(szKey, _T("Software\\Apps\\Panoramic "));
    oStr->Concatenate(szKey, szAppName);

    hr = GetKey(szKey, _T("InstallDir"), szDestDir, STRING_MAX);
    CHR(hr, _T("hr = GetKey(szKey, _T(\"InstallDir\"), szDestDir, STRING_MAX);"));

    oStr->Concatenate(szDestDir, _T("\\"));
Error:
    return hr;
}

/********************************************************************
Function        GetExeDirectory

Arguments:      szDir - output exe directory here

Returns:        

Comments:       This function is for application that just need the directory
                        that it currently resides in.
*********************************************************************/
HRESULT GetExeDirectory(TCHAR* szDir)
{
    CIssString *oStr = CIssString::Instance();

    if(NULL == szDir)
        return E_INVALIDARG;

    TCHAR szTemp[STRING_MAX];

    int iFileLength = GetModuleFileName(NULL, szTemp, STRING_MAX);
    if(iFileLength == 0)
        return E_INVALIDARG;

    // search for the first backslash in reverse order
    for(int i=iFileLength-1; i>=0; i--)
    {
        if(szTemp[i] == _T('\\'))
        {
            oStr->StringCopy(szDir, szTemp, 0, i+1);
            return S_OK;
        }
    }

    return E_INVALIDARG;
}

BOOL LaunchEula(TCHAR* szKey)
{
// only for PPC and SP for now
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
#ifndef DEBUG
	CIssString *oStr = CIssString::Instance();
	DWORD dw;
	if (GetKey(szKey,_T("EULA"),dw) != S_OK)
	{
		TCHAR szFileName[STRING_MAX];
	
		GetExeDirectory(szFileName);	
		oStr->Concatenate(szFileName, _T("EULA.EXE"));

		//lauch the app again
		CreateProcess(szFileName, 
					  _T(""), 
					  NULL, NULL,
					  FALSE, 0, 
					  NULL, NULL, 
					  NULL, NULL);

		return TRUE;
	}
#endif
#endif
	return FALSE;
}
BOOL LaunchHelp(TCHAR* szHelp, HINSTANCE hInst)
{
	SHELLEXECUTEINFO shell;
	shell.cbSize		= sizeof(SHELLEXECUTEINFO);
	shell.lpVerb		= _T("OPEN"); 
	shell.fMask			= SEE_MASK_NOCLOSEPROCESS ;
	shell.hwnd			= NULL;
//#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	shell.lpFile		= _T("iexplore.exe");
//#else
//	shell.lpFile		= _T("wapbrowse.exe");
//#endif
	shell.lpDirectory	= NULL;
	shell.hInstApp		= hInst;
	shell.nShow			= SW_SHOW;

	TCHAR szTemp[STRING_MAX];
	GetExeDirectory(szTemp);

	CIssString* oStr = CIssString::Instance();
	oStr->Concatenate(szTemp, szHelp);

	shell.lpParameters	= szTemp; 

	ShellExecuteEx(&shell);

	return TRUE;
}


/********************************************************************
Function        CreateSoftKey

Arguments:      hWndParent      - parent window for the softkeys
                        hInst           - resource instance
                        uiIdCommand1- ID for the left soft key (0 to disable)
                        uiIdString1     - string resource for the left softkey (0 to disable)
                        uiIdCommand2- ID for the right soft key (0 to disable)
                        uiIdString2     - string resource for the right softkey (0 to disable)

Returns:        window handle of the menu

Comments:       Dynamically create a Soleus menu
*********************************************************************/
HWND CreateSoftKey(HWND hWndParent,
                   HINSTANCE hInst,
                   UINT uiIdCommand1,
                   UINT uiIdString1,
                   UINT uiIdCommand2,
                   UINT uiIdString2)
{
        HWND hWndSoftwkey = NULL;
#ifdef SOLEUS

        SOFTKEYITEM sSoftKeys[2];

        sSoftKeys[0].flags =    SKIF_MASK_STRING |
                                                        SKIF_S_IDSTRING  |
                                                        SKIF_MASK_ENABLED|
                                                        SKIF_E_ENABLED   |
                                                        SKIF_MASK_COMMAND|
                                                        SKIF_C_COMMAND;
        sSoftKeys[0].idCommand  = uiIdCommand1;
        sSoftKeys[0].s.idString = uiIdString1;

        sSoftKeys[1].flags =    SKIF_MASK_STRING |
                                                        SKIF_S_IDSTRING  |
                                                        SKIF_MASK_ENABLED|
                                                        SKIF_E_ENABLED   |
                                                        SKIF_MASK_COMMAND|
                                                        SKIF_C_COMMAND;
        sSoftKeys[1].idCommand  = uiIdCommand2;
        sSoftKeys[1].s.idString = uiIdString2;

        //
        // Disable softkeys if invalid command or softkey text resource id
        //
        if ( (0 == uiIdCommand1) || (0 == uiIdString1) )
                sSoftKeys[0].flags &= ~(SKIF_E_ENABLED);

        if ( (0 == uiIdCommand2) || (0 == uiIdString2) )
                sSoftKeys[1].flags &= ~(SKIF_E_ENABLED);

    // create the softkeys now
    SoftKeysCreate( hWndParent,
                    2,
                    sSoftKeys,
                    (HFXMODULE)hInst,
                    &hWndSoftwkey);

    // we don't show the keyboard
    SoftKeysShowSIPButton(hWndSoftwkey, FALSE);    
#endif
    return hWndSoftwkey;
}

void DebugProfile(TCHAR* szFunction, BOOL bStart)
{
#ifdef DEBUG
    static DWORD s_dwStartTicks = GetTickCount();
    if(bStart)
    {
        s_dwStartTicks = GetTickCount();
    }
    else
    {
        DebugOut(_T("%s completed in %d ms"), szFunction, GetTickCount() - s_dwStartTicks);
    }
#endif
}

void DebugOutMemory()
{
#ifdef UNDER_CE
#ifdef DEBUG
    #pragma comment( lib, "toolhelp.lib" )

    DWORD dwCurrentProcess = GetCurrentProcessId();

    HANDLE hHeapSnapshot;
    HEAPLIST32 HeapList = {0};
    HEAPENTRY32 HeapEntry = {0};
    DWORD dwTotalBlocksUsed = 0;

    hHeapSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, dwCurrentProcess);
    if (INVALID_HANDLE_VALUE != hHeapSnapshot)
    {
		HeapList.dwSize = sizeof(HEAPLIST32);

		if (Heap32ListFirst(hHeapSnapshot, &HeapList) != FALSE)
		{        
			do        
			{            
				HeapEntry.dwSize = sizeof(HEAPENTRY32);    

				if (Heap32First(hHeapSnapshot, &HeapEntry, HeapList.th32ProcessID, HeapList.th32HeapID) != FALSE)            
				{
					do                
					{   
					// save HeapEntry.dwAddress and 
					// HeapEntry.dwBlockSize for use later
					dwTotalBlocksUsed += HeapEntry.dwBlockSize;

					} while (Heap32Next(hHeapSnapshot, &HeapEntry) != FALSE);            
				}            
				else                
					break;

			} while (Heap32ListNext(hHeapSnapshot, &HeapList) != FALSE);    
		}        

		CloseToolhelp32Snapshot(hHeapSnapshot);
    }

    if(dwTotalBlocksUsed > 0)
		DebugOut(_T("***** Memory Used: %d"), dwTotalBlocksUsed);
#endif
#endif
}

/********************************************************************
Function        DebugOut

Arguments:      szFormat - format of the text to output

Returns:        

Comments:       Output debug statements to debug window and also to file if 
                        defined.  Will also put a timestamp
*********************************************************************/
HRESULT DebugOut(TCHAR* szFormat, ...) 
{ 
#ifdef DEBUG 
//#ifdef UNDER_CE
    va_list vaMarker; 
    SYSTEMTIME      sysTime;
    FILE* fp = NULL;

    // Open the file for appending
    if(DEBUG_FILE)
            fp = fopen("\\iSSDebug.txt", "a"); 

    GetSystemTime(&sysTime);

    CIssString* oStr = CIssString::Instance();

    TCHAR* szDebug  = new TCHAR[2*STRING_MAX];
    oStr->Empty(szDebug);

    // put in the time first
    oStr->Format(szDebug, _T("%02d:%02d:%02d: "), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    // send it to our various outputs
    OutputDebugString(szDebug);

    if(DEBUG_FILE)
            fwprintf( fp, szDebug ); 

    va_start(vaMarker, szFormat); 
    vswprintf(szDebug, szFormat, vaMarker); 
    va_end(vaMarker); 

    // send again to our various outputs
    OutputDebugString(szDebug); 
    OutputDebugString(_T("\r\n")); 

    if(DEBUG_FILE)
    {
            fwprintf( fp, szDebug ); 
            fwprintf( fp, _T("\r\n"));
    }

    if(fp)
            fclose(fp);

    oStr->Delete(&szDebug);
//#endif
#endif
    return S_OK;
}

/********************************************************************
Function        LoadStringResources

Arguments:      hInst - app instance to retrieve the exe location

Returns:        HINSTANCE of the resource

Comments:       For Soleus only.  Load the mui file properly.  This will
                        check the current language and try and load the mui, but if 
                        it can't find it it will default to a non specific mui language
                        file
*********************************************************************/
HINSTANCE LoadStringResources(HINSTANCE hInst)
{
    TCHAR szFileName[2*STRING_MAX];

    CIssString* oStr = CIssString::Instance();

    oStr->Empty(szFileName);

    // get the full file name
    GetModuleFileName(hInst, szFileName, STRING_MAX);

#ifdef DEBUG 
    DebugOut(_T("GetModuleFileName - %s"), szFileName);
#endif

    // take off the .exe name
    oStr->Delete(oStr->GetLength(szFileName)-4, 4, szFileName);

    // get the current language and add it to our string
    LANGID dwLangID = GetUserDefaultUILanguage();
    TCHAR szAddString[STRING_LARGE];
    oStr->Format(szAddString, _T("Res.dll.%.4X.mui"), dwLangID);
    oStr->Concatenate(szFileName, szAddString);

#ifdef DEBUG 
    DebugOut(_T("AddString - %s"), szAddString);
    DebugOut(_T("Calling Loadlibrary - %s"), szFileName);
#endif

    HINSTANCE hStringResource = (HINSTANCE)LoadLibrary(szFileName);

    if(NULL == hStringResource)
    {
            // load the default "AppRes.dll"
            oStr->Delete(oStr->GetLength(szFileName) - 9, 9, szFileName);

#ifdef DEBUG
            DebugOut(_T("Failed, calling default loadlibrary - %s"), szFileName);
#endif

            hStringResource = (HINSTANCE)LoadLibrary(szFileName);

            // this is bad if it doesn't find the default one
            if(NULL == hStringResource)
            {
                    oStr->Insert(szFileName, _T("Unable to load "), 0);
                    MessageBox(NULL, szFileName, _T(""), MB_ICONERROR);
                    return NULL;
            }
    }

    return hStringResource;

}

/********************************************************************
Function        GetMenuHeight

Arguments:      

Returns:        menu height

Comments:       Because of limitations of the WinCE we need to get a function
                        that will accurately find the menu height
*********************************************************************/
int GetMenuHeight()
{
    int iWidth  = GetSystemMetrics(SM_CXSCREEN);
    int iHeight = GetSystemMetrics(SM_CYSCREEN);


#ifdef WIN32_PLATFORM_WFSP
    if(iWidth == 176)
        return 20;//confirmed
    else if(iWidth <= 320)
        return 27;
    else 
        return GetSystemMetrics(SM_CYMENU);


#elif WIN32_PLATFORM_PSPC
    if(iWidth == 240 ||
        iWidth == 320)
        return 26;//standard Pocket PCs
else if(iWidth == 480 || 
        iWidth == 640)
        return 52;
    else 
        return GetSystemMetrics(SM_CYMENU);
#elif UNDER_CE
    return GetSystemMetrics(SM_CYMENU);
#endif
    return 0; //desktop
}

/********************************************************************
Function        SystemTimeToUtc

Arguments:      sysTime - given current system time
                        iTime   - output unix time

Returns:        

Comments:       convert windows system time to unix standard time
*********************************************************************/
HRESULT SystemTimeToUtc(SYSTEMTIME sysTime, 
                     int& iTime,
					 BOOL bIsLocal)
{
    FILETIME ft; 
    FILETIME ftLocal;
    ULARGE_INTEGER i64; 
    time_t t; 


    // 116444736000000000 is the value for 01/01/1970 00:00 
	if(bIsLocal)
	{
		SystemTimeToFileTime(&sysTime, &ftLocal); 
		LocalFileTimeToFileTime(&ftLocal, &ft);
	}
	else
	{
		SystemTimeToFileTime(&sysTime, &ft); 
	}
    i64.LowPart = ft.dwLowDateTime; 
    i64.HighPart = ft.dwHighDateTime; 
    t = (time_t)((i64.QuadPart - 116444736000000000) / 10000000); 

    iTime = (int)t;

    return S_OK;
}

/********************************************************************
Function        UtcToSystemTime

Arguments:      iTime   - current unix time
                        sysTime - output system time

Returns:        

Comments:       Convert current unix time to windows system time
*********************************************************************/
HRESULT UtcToSystemTime(int iTime, 
                     SYSTEMTIME& sysTime)
{
    FILETIME ft;
    FILETIME ftLocal;
    time_t t                        = (time_t)iTime;
    LONGLONG ll                     = Int32x32To64(t, 10000000) + 116444736000000000;
    ft.dwLowDateTime        = (DWORD) ll;
    ft.dwHighDateTime       = (DWORD)(ll >>32);

    FileTimeToLocalFileTime(&ft, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);

    return S_OK;
}

BOOL ShouldAbort(HWND hWnd)
{
	MSG msg;

	if (MsgWaitForMultipleObjects(0, NULL, FALSE, 5, QS_ALLINPUT) != WAIT_OBJECT_0)
		return FALSE;

	if(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		switch(LOWORD(msg.message))
		{
			//	case WM_KEYUP:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_COMMAND:
		case WM_USER:
		case WM_SETFOCUS:
			return TRUE;
		}

		//	PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
		//	TranslateMessage((LPMSG)&msg);
		//	DispatchMessage((LPMSG)&msg);
	}
	return FALSE;
}

BOOL IsRectInRect(RECT& rc1, RECT& rc2)
{
    return !(rc1.left >= rc2.right || rc1.right <= rc2.left || rc1.top >= rc2.bottom || rc1.bottom <= rc2.top);
}

