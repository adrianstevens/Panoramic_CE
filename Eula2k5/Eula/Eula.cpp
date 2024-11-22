// Eula.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "eula.h"
#include <windows.h>
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include "IssString.h"
#include "IssRegistry.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hwndCB;					// The command bar handle
TCHAR szTotal[20000];
 TCHAR szPiece[2000];

static SHACTIVATEINFO s_sai;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
HWND				CreateRpCommandBar(HWND);
void				SetAcceptEula();


int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable = NULL;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

#ifndef WIN32_PLATFORM_WFSP
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EULA));
#endif // !WIN32_PLATFORM_WFSP

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
#ifndef WIN32_PLATFORM_WFSP
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
#endif // !WIN32_PLATFORM_WFSP
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EULA));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

    g_hInst = hInstance; // Store instance handle in our global variable

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_EULA, szWindowClass, MAX_LOADSTRING);

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }
	RECT	rect;
	GetClientRect(hWnd, &rect);

	HWND wndDesktop = GetDesktopWindow();

	DialogBox(hInstance,
					  MAKEINTRESOURCE(IDD_DLG_EULA),
					  wndDesktop,
					  (DLGPROC)WndProc);

	
	/*hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{	
		return FALSE;
	}
    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }
#endif // WIN32_PLATFORM_PSPC

    ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);*/


    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

	CIssString* oStr = CIssString::Instance();

#if defined(SHELL_AYGSHELL) && !defined(WIN32_PLATFORM_WFSP)
    static SHACTIVATEINFO s_sai;
#endif // SHELL_AYGSHELL && !WIN32_PLATFORM_WFSP
	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
#ifdef WIN32_PLATFORM_WFSP
                case IDOK:
                    DestroyWindow(hWnd);
                    break;
#endif // WIN32_PLATFORM_WFSP
#ifdef WIN32_PLATFORM_PSPC
                case IDOK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;
#endif // WIN32_PLATFORM_PSPC
				case IDMENU_Accept:
					SetAcceptEula();
					// fall through
				case IDMENU_Deny:
					EndDialog(hWnd, 0);

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
		case WM_INITDIALOG:
#ifdef SHELL_AYGSHELL
			{
			
			SHINITDLGINFO shidi;
			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hWnd;
			SHInitDialog(&shidi);
			g_hwndCB = CreateRpCommandBar(hWnd);
            // Initialize the shell activate info structure
            memset(&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
#endif // SHELL_AYGSHELL
  
            oStr->SetResourceInstance(g_hInst); 

            oStr->StringCopy(szTotal, IDS_LICENSETEXT, 2000); 
            oStr->StringCopy(szPiece, IDS_LICENSETEXT2, 2000); 
            oStr->Concatenate(szTotal, szPiece); 
            oStr->StringCopy(szPiece, IDS_LICENSETEXT3, 2000); 
            oStr->Concatenate(szTotal, szPiece); 
            oStr->StringCopy(szPiece, IDS_LICENSETEXT4, 2000); 
            oStr->Concatenate(szTotal, szPiece); 
            oStr->StringCopy(szPiece, IDS_LICENSETEXT5, 2000); 
            oStr->Concatenate(szTotal, szPiece); 
            oStr->StringCopy(szPiece, IDS_LICENSETEXT6, 2000); 
            oStr->Concatenate(szTotal, szPiece); 
  
            SetWindowText(GetDlgItem(hWnd, IDC_EDT_EULA), szTotal);
			}
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            
            // TODO: Add any drawing code here...
            
            EndPaint(hWnd, &ps);
            break;
		case WM_SIZE:
			RECT rc;
			GetClientRect(hWnd, &rc);

			MoveWindow(GetDlgItem(hWnd, IDC_EDT_EULA),0,0, rc.right-rc.left, rc.bottom - rc.top, FALSE);
			break;
        case WM_DESTROY:
#ifdef SHELL_AYGSHELL
			CommandBar_Destroy(g_hwndCB);
#endif // SHELL_AYGSHELL
            PostQuitMessage(0);
            break;

#ifdef SHELL_AYGSHELL
        case WM_ACTIVATE:
            // Notify shell of our activate message
            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;
#endif // SHELL_AYGSHELL && !WIN32_PLATFORM_WFSP

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND CreateRpCommandBar(HWND hwnd)
{
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	mbi.nToolBarId = IDR_MENU_AD;
	mbi.hInstRes   = g_hInst;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;

	if (!SHCreateMenuBar(&mbi)) 
		return NULL;

	return mbi.hwndMB;
}

// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SHINITDLGINFO shidi;

	switch (message)
	{
		case WM_INITDIALOG:
			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);
			return TRUE; 

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

BOOL GetExeDirectory(TCHAR* szDir)
{
	CIssString *oStr = CIssString::Instance();

	if(NULL == szDir)
		return FALSE;

	TCHAR szTemp[STRING_MAX];

	int iFileLength = GetModuleFileName(NULL, szTemp, STRING_MAX);
	if(iFileLength == 0)
		return FALSE;

	// search for the first backslash in reverse order
	for(int i=iFileLength-1; i>=0; i--)
	{
		if(szTemp[i] == _T('\\'))
		{
			oStr->StringCopy(szDir, szTemp, 0, i+1);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL LoadFile(TCHAR *szFileName, char * szFile, int iMax)
{
	HANDLE hFile;

	DWORD dwDisposition = OPEN_EXISTING;
			  
	// Open the INI File for reading
	hFile = ::CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
	  dwDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
	  return FALSE;
	}
	

	DWORD	dwNumberOfBytesRead;

	// Open the File for Reading
	if(! ::ReadFile(hFile,szFile, iMax, &dwNumberOfBytesRead, NULL))
	{
		CloseHandle(hFile);
		hFile = NULL;
		return FALSE;
	}

	szFile[dwNumberOfBytesRead] = '\0'; 

	CloseHandle(hFile);
	hFile = NULL;

	return TRUE;
}

void SetAcceptEula()
{
	CIssString *oStr = CIssString::Instance();

	CIssRegistry oReg;

	char  szFile[30000];
	TCHAR szFileName[STRING_MAX];

	GetExeDirectory(szFileName);

	oStr->Concatenate(szFileName, _T("eula.txt"));

	if(!LoadFile(szFileName, szFile, 30000))
		return;	

	char		szEndl[] = "\n";
	char		*szToken;
	TCHAR		szLine1 [STRING_MAX];
	TCHAR		szLine2 [STRING_MAX];

	// Read the First Line in the File
	szToken = oStr->StringToken(szFile, szEndl);

	if(!szToken)
		return;

	oStr->StringCopy(szLine1, szToken);
	oStr->Trim(szLine1);

	szToken = oStr->StringToken(NULL, szEndl);

	if(!szToken)
		return;

	oStr->StringCopy(szLine2, szToken);
	oStr->Trim(szLine2);

	TCHAR szKey[STRING_MAX] = _T("SOFTWARE\\Pano\\");

	oStr->Concatenate(szKey, szLine2);

	//set the key
	if(!oReg.SetKey(szKey, _T("EULA"), 1))
		return;

	GetExeDirectory(szFileName);

	oStr->Concatenate(szFileName, szLine1);

	//lauch the app again
    CreateProcess(szFileName, _T(""), NULL, NULL,
				  FALSE, 0, NULL, NULL, NULL, NULL);
}
