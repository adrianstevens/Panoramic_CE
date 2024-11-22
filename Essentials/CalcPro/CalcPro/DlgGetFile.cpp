#include "stdafx.h"
#include "DlgGetFile.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "IssLocalisation.h"

#ifndef SOLEUS
#include "aygshell.h"
#include "tpcshell.h"
#endif

#define WM_ControlKeyUp		WM_USER + 200
#define WM_ControlKeyDown	WM_USER + 201
#define WM_UpdateName		WM_USER + 202
#define WM_SelectFile		WM_USER + 203
#define WM_UpdateDirectory	WM_USER + 300

#ifdef WIN32_PLATFORM_PSPC
#define CLB_GETCURSEL		CB_GETCURSEL
#define CLB_ADDSTRING		CB_ADDSTRING
#define CLB_SETITEMDATA		CB_SETITEMDATA
#define CLB_SETCURSEL		CB_SETCURSEL
#define CLB_GETCOUNT		CB_GETCOUNT
#define CLB_GETITEMDATA		CB_GETITEMDATA
#define CLB_RESETCONTENT	CB_RESETCONTENT
#else
#define CLB_GETCURSEL		LB_GETCURSEL
#define CLB_ADDSTRING		LB_ADDSTRING
#define CLB_SETITEMDATA		LB_SETITEMDATA
#define CLB_SETCURSEL		LB_SETCURSEL
#define CLB_GETCOUNT		LB_GETCOUNT
#define CLB_GETITEMDATA		LB_GETITEMDATA
#define CLB_RESETCONTENT	LB_RESETCONTENT
#endif

struct TypeFiles
{
	TCHAR*	szFileName;
	DWORD	dwAttributes; 
};

#define TXT_ReturnPath		_T("..")

CDlgGetFile::CDlgGetFile(void)
:m_hWndName(NULL)
,m_hWndSpin(NULL)
,m_hWndType(NULL)
,m_sOpenFile(NULL)
,m_bOpenFile(TRUE)
,m_hFontText(NULL)
{
	m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_NORMAL, TRUE);
}

CDlgGetFile::~CDlgGetFile(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CDlgGetFile::GetSaveFile(OPENFILENAME* sSaveFile)
{
	if(!sSaveFile				|| 
	   !sSaveFile->hwndOwner	|| 
	   !sSaveFile->hInstance	||
	   !sSaveFile->lpstrFile	||
	   !sSaveFile->lpstrFilter)
		return FALSE;

	m_sOpenFile	= sSaveFile;
	m_bOpenFile = FALSE;

	BOOL bReturn = CIssWnd::DoModal(m_sOpenFile->hwndOwner, m_sOpenFile->hInstance, IDD_DLG_BASIC);

	return (bReturn == IDOK?TRUE:FALSE);
}

BOOL CDlgGetFile::GetOpenFile(OPENFILENAME* sOpenFile)
{
	if(!sOpenFile				|| 
		!sOpenFile->hwndOwner	|| 
		!sOpenFile->hInstance	||
		!sOpenFile->lpstrFile	||
		!sOpenFile->lpstrFilter)
		return FALSE;

	m_sOpenFile	= sOpenFile;
	m_bOpenFile = TRUE;

	BOOL bReturn = CIssWnd::DoModal(m_sOpenFile->hwndOwner, m_sOpenFile->hInstance, IDD_DLG_BASIC);

	return (bReturn == IDOK?TRUE:FALSE);
}

BOOL CDlgGetFile::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hWndName = CreateWindowEx(0, TEXT("edit"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
		0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);
	SendMessage(m_hWndName, EM_LIMITTEXT, (WPARAM)40, 0L );

#ifdef WIN32_PLATFORM_PSPC
	m_hWndType = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 
		0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	int iDropWindowSize = 150;
	if (GetSystemMetrics(SM_CYSCREEN) > 320)
		iDropWindowSize = 300;

	//SendMessage(m_hWndType, CB_SETITEMHEIGHT, -1, iDropWindowSize);
	SendMessage(m_hWndType, CB_SETEXTENDEDUI, 1, 0);
#else
	m_hWndType = CreateWindowEx(0, _T("LISTBOX"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 
		0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndSpin = CreateWindowEx(0, _T("msctls_updown32"), NULL,
		WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 
		0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);
#endif

	m_ctrlBrowse.SetCurrentFolder((TCHAR*)m_sOpenFile->lpstrInitialDir);
	m_ctrlBrowse.Create(hWnd, m_hInst, m_hFontText);
	
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= (m_bOpenFile?IDR_MENU_OpenMenu:IDR_MENU_SaveMenu);
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
		ASSERT(0);

#ifdef WIN32_PLATFORM_WFSP
	::SendMessage(	mbi.hwndMB, 
		SHCMBM_OVERRIDEKEY, 
		VK_TBACK, 
		MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif

#endif

	if(m_bOpenFile)
		SetWindowText(m_hWnd, m_oStr->GetText(ID(IDS_MENU_OpenFile), m_hInst));
	else
		SetWindowText(m_hWnd, m_oStr->GetText(ID(IDS_MENU_SaveFile), m_hInst));

	InitList();

	SetCurrentFilterList();

	//memset(&m_sSai, 0, sizeof (m_sSai));  
	//m_sSai.cbSize = sizeof (m_sSai);


	return TRUE;
}

BOOL CDlgGetFile::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
	if(HIWORD(lParam)==VK_TBACK && (0 != (MOD_KEYUP & LOWORD(lParam))))
	{
		SHSendBackToFocusWindow(WM_HOTKEY, wParam, lParam);
	}
#endif
	return UNHANDLED;
}

BOOL CDlgGetFile::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
	int	iIndent		= GetSystemMetrics(SM_CXICON)/8;

	m_rcName.left	= iIndent;
	m_rcName.top	= iIndent;
	m_rcName.right	= rcClient.right/4;
	m_rcName.bottom = m_rcName.top + iIconSize;

	m_rcType		= m_rcName;
	m_rcType.top	= m_rcName.bottom + iIndent;
	m_rcType.bottom	= m_rcType.top + iIconSize;

	m_rcDirectory		= m_rcType;
	m_rcDirectory.right	= rcClient.right - iIndent;
	m_rcDirectory.top	= m_rcType.bottom + iIndent;
	m_rcDirectory.bottom= m_rcDirectory.top + iIconSize;

	m_gdiMem.Destroy();

	if(m_hWndName)
	{
		MoveWindow(m_hWndName,
				   m_rcName.right, m_rcName.top,
				   rcClient.right - m_rcName.right-iIndent, iIconSize,
				   FALSE);
	}
	if(m_hWndType)
	{
#ifdef WIN32_PLATFORM_PSPC
		int iDropWindowSize = 100;
		if (GetSystemMetrics(SM_CYSCREEN) > 320)
			iDropWindowSize = 300;
		MoveWindow(m_hWndType,
			m_rcType.right, m_rcType.top,
			rcClient.right - m_rcType.right-iIndent, iDropWindowSize,
			FALSE);
#else
		MoveWindow(m_hWndType,
			m_rcType.right, m_rcType.top,
			rcClient.right - m_rcType.right-iIndent, iIconSize,
			FALSE);

		// don't forget the spin control
		SetWindowPos(m_hWndSpin, 
			HWND_TOPMOST,
			rcClient.right-iIndent - iIconSize, m_rcType.top,
			iIconSize,
			iIconSize,
			SWP_SHOWWINDOW);
#endif
	}

	if(m_ctrlBrowse.GetWnd())
	{
		int iColumnWidth = rcClient.right - 2*iIndent;
		MoveWindow(m_ctrlBrowse.GetWnd(), 
				   iIndent, m_rcDirectory.bottom + iIndent,
				   iColumnWidth, rcClient.bottom - m_rcDirectory.bottom - 2*iIndent,
				   FALSE);

		GetClientRect(m_ctrlBrowse.GetWnd(), &rcClient);

		iColumnWidth = rcClient.right - 2*iIndent;
//		if(GetWindowLong(m_ctrlBrowse.GetWnd(), GWL_STYLE) & WS_VSCROLL)
//			iColumnWidth -= GetSystemMetrics(SM_CXVSCROLL);

		ListView_SetColumnWidth(m_ctrlBrowse.GetWnd(), 0, iColumnWidth);
	}
	return TRUE;
}

BOOL CDlgGetFile::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient.right, m_rcDirectory.bottom, FALSE, FALSE, FALSE);
	
	RECT rcMem = {0};
	rcMem.right	= m_gdiMem.GetWidth();
	rcMem.bottom	= m_gdiMem.GetHeight();
	FillRect(m_gdiMem, rcMem, RGB(255,255,255));

	DrawText(m_gdiMem, rcMem);

	// draw to the screen
	BitBlt(hDC,
		   0,0,
		   rcMem.right, rcMem.bottom,
		   m_gdiMem,
		   0,0,
		   SRCCOPY);

	return TRUE;
}

BOOL CDlgGetFile::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmh = (LPNMHDR)lParam;
	if(lpnmh->hwndFrom == m_hWndType)
	{
		switch(lpnmh->code)
		{
		/*case LVN_DELETEITEM:
		{
			// make sure to clean up memory
			NMLISTVIEW* lpItem = (NMLISTVIEW*)lpnmh;
			if(lpItem->lParam)
				delete [] (TCHAR*)lpItem->lParam;
			break;
		}*/
		case LVN_ITEMCHANGED:
		case CBN_SELCHANGE:
			SetCurrentFilterList();
			break;
		}
	}
	else if(lpnmh->hwndFrom == m_ctrlBrowse.GetWnd())
		return m_ctrlBrowse.OnNotifyReflect(hWnd, wParam, lParam);

	return UNHANDLED;
}

BOOL CDlgGetFile::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(lParam == (LPARAM)m_hWndType)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
			SetCurrentFilterList();

	}
	else
	{
		switch(LOWORD(wParam))
		{
	#ifdef WIN32_PLATFORM_PSPC
		case IDOK:
	#endif
		case IDMENU_Save:
			OnMenuSave();
			break;
		case IDMENU_Open:
			OnMenuOpen();
			break;
		case IDMENU_Cancel:
			EndDialog(IDCANCEL);
			break;
		case IDMENU_Menu:
			EndDialog(IDCANCEL);
			break;
		default:
			break;
		}
	}
	return UNHANDLED;
}
void CDlgGetFile::OnMenuSave()
{
	TCHAR szFullPath[MAX_PATH];
	if(!BuildFullPath(szFullPath))
		return;

	SaveFile(szFullPath);
}

void CDlgGetFile::OnMenuOpen()
{
	TCHAR szFullPath[MAX_PATH];
	m_oStr->Empty(szFullPath);
	if(!BuildFullPath(szFullPath))
		return;

	// it has to exist
	DWORD dwAttributes = GetFileAttributes(szFullPath);
	if(dwAttributes == ((DWORD)-1))
	{
		MessageBox(m_hWnd, m_oStr->GetText(ID(IDS_ERROR_FileDoesNotExist), m_hInst), m_oStr->GetText(ID(IDS_ERROR_Error), m_hInst), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_oStr->StringCopy(m_sOpenFile->lpstrFile, szFullPath);
	EndDialog(IDOK);
}

BOOL CDlgGetFile::BuildFullPath(TCHAR* szFullPath)
{
	if(!szFullPath)
		return FALSE;

	m_oStr->Empty(szFullPath);
	m_oStr->StringCopy(szFullPath, m_ctrlBrowse.GetCurrentFolder());
	if(m_oStr->IsEmpty(szFullPath))
		return FALSE;

	if(!m_ctrlBrowse.IsRootPath())
		m_oStr->Concatenate(szFullPath, _T("\\"));

	TCHAR szFileName[STRING_MAX];
	m_oStr->Empty(szFileName);
	GetWindowText(m_hWndName, szFileName, STRING_MAX);
	if(m_oStr->IsEmpty(szFileName))
	{
		m_ctrlBrowse.OnSelectItem();
		return TRUE;
	}

	m_oStr->Concatenate(szFullPath, szFileName);

	DWORD dwAttributes = GetFileAttributes(szFullPath);
	if(dwAttributes & FILE_ATTRIBUTE_DIRECTORY && dwAttributes != ((DWORD)-1))
		return FALSE;

	// see if we have to add the extension
	if(dwAttributes == ((DWORD)-1))
	{
		TCHAR szExt[STRING_SMALL];
		m_oStr->StringCopy(szExt, m_ctrlBrowse.GetFilter());

		// if there is no extension to add
		if(m_oStr->Compare(szExt, _T("*.*"))==0 ||
		   m_oStr->IsEmpty(szExt))
		   m_oStr->StringCopy(szExt, _T("*.txt")); // just make a default for now

		m_oStr->Delete(0,1, szExt);

		// see if we have to add the extension
		int iIndex = m_oStr->Find(szFullPath, szExt, 0);
		if(iIndex == -1)
			m_oStr->Concatenate(szFullPath, szExt);
	}

	return TRUE;
}


BOOL CDlgGetFile::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgGetFile::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgGetFile::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE)
		SetFocus(m_ctrlBrowse.GetWnd());

	return UNHANDLED;
}

BOOL CDlgGetFile::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = UNHANDLED;
	//if(wParam == SPI_SETSIPINFO)  
		//bRet = SHHandleWMSettingChange(m_hWnd, wParam, lParam, &m_sSai); 
	
	return bRet;
}

BOOL CDlgGetFile::OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

void CDlgGetFile::InitList()
{
	int nIndex;
	TCHAR szFilter[STRING_MAX];
	TCHAR szExt[STRING_MAX];
	LPTSTR pszSrc, p;
	pszSrc = (LPTSTR)m_sOpenFile->lpstrFilter;

	ListResetContent(m_hWndType);
	while (TRUE) 
	{
		// filter
		p = szFilter;
		while (*pszSrc != NULL) 
		{
			*p++ = *pszSrc;
			pszSrc++;
		}
		*p = NULL;
		if (*pszSrc == NULL && *(pszSrc + 1) == NULL)
			break;
		pszSrc++;

		// ext
		p = szExt;
		while (*pszSrc != NULL) 
		{
			*p++ = *pszSrc;
			pszSrc++;
		}
		*p = NULL;
		if (_tcslen(szFilter) && _tcslen(szExt)) 
		{
			int nIndex = SendMessage(m_hWndType, CLB_ADDSTRING,  0, (LPARAM)szFilter);
			if (nIndex == CB_ERR)
				break;

			p = new TCHAR[_tcslen(szExt) + 1];
			_tcscpy(p, szExt);
			SendMessage(m_hWndType, CLB_SETITEMDATA, (WPARAM)nIndex, (LPARAM)p);
		}
		if (*pszSrc == NULL && *(pszSrc + 1) == NULL)
			break;
		pszSrc++;
	}

	nIndex = m_sOpenFile->nFilterIndex > 0 ? m_sOpenFile->nFilterIndex - 1 : 0;
	ListSetCurSel(m_hWndType, nIndex);
}

void CDlgGetFile::SetCurrentFilterList()
{
	int iIndex = ListGetCurSel(m_hWndType);
	if(-1 == iIndex)
		return;

	TCHAR* szFilter = (TCHAR*)SendMessage(m_hWndType, CLB_GETITEMDATA, iIndex, 0);
	if(szFilter)
		m_ctrlBrowse.SetFilter(szFilter);
}

void CDlgGetFile::DrawText(HDC hDC, RECT rcClient)
{
    ::DrawText(hDC, m_oStr->GetText(ID(IDS_INFO_Name), m_hInst), m_rcName, DT_LEFT|DT_VCENTER, m_hFontText, RGB(0,0,0));
    ::DrawText(hDC, m_oStr->GetText(ID(IDS_INFO_Type), m_hInst), m_rcType, DT_LEFT|DT_VCENTER, m_hFontText, RGB(0,0,0));

	FillRect(hDC, m_rcDirectory, RGB(0,0,0));
	RECT rcTemp = m_rcDirectory;
	rcTemp.left++;
	rcTemp.top++;
	rcTemp.right--;
	rcTemp.bottom--;
	if(GetSystemMetrics(SM_CXSCREEN) > 320)
	{
		rcTemp.left++;
		rcTemp.top++;
		rcTemp.right--;
		rcTemp.bottom--;
	}
	FillRect(hDC, rcTemp, GetSysColor(COLOR_BTNFACE));

	int iIndent		= GetSystemMetrics(SM_CXICON)/8;
	rcTemp.left		+= iIndent;
	rcTemp.right	-= iIndent;

	TCHAR szDirectoryName[STRING_MAX];
	if(!m_ctrlBrowse.GetCurrentFolderName(szDirectoryName))
		return;

	::DrawText(hDC, szDirectoryName, rcTemp, DT_LEFT|DT_VCENTER, m_hFontText, RGB(0,0,0));
}

void CDlgGetFile::ClearFilter()
{
	int n = SendMessage(m_hWndType, CLB_GETCOUNT, 0, 0);
	for (int i = 0; i < n; i++) 
	{
		LPTSTR p = (LPTSTR)SendMessage(m_hWndType, CLB_GETITEMDATA, i, 0);
		if(p)
			delete p;
	}
	SendMessage(m_hWndType, CLB_RESETCONTENT, 0, 0);
}

void CDlgGetFile::EndDialog(int nResult)
{
	// kill off any memory here
	ClearFilter();

	m_ctrlBrowse.Destroy();
    SafeCloseWindow(nResult);
	//::EndDialog(m_hWnd, nResult);
}

BOOL CDlgGetFile::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_ControlKeyUp:
		SetFocus(m_hWndType);
		break;
	case WM_ControlKeyDown:
		SetFocus(m_hWndName);
		break;
	case WM_UpdateDirectory:
		// just redraw the new directory
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_UpdateName:
	{
		TCHAR* szNewText = (TCHAR*)lParam;
		if(szNewText)
			SetWindowText(m_hWndName, szNewText);
		break;
	}
	case WM_SelectFile:
		OnSelectFile(m_ctrlBrowse.GetSelectedPath(), m_ctrlBrowse.GetSelectedFullPath());
		break;
	default:
		return UNHANDLED;
		break;
	}
	return TRUE;
}

void CDlgGetFile::OnSelectFile(TCHAR* szFileName, TCHAR* szFullPath)
{
	if(NULL == szFileName || NULL == szFullPath)
		return;

	// if we're opening a file we just save and exit
	if(m_bOpenFile)
	{
		m_oStr->StringCopy(m_sOpenFile->lpstrFile, szFullPath);
		EndDialog(IDOK);
		return;
	}
	else
		SaveFile(szFullPath);
}

void CDlgGetFile::SaveFile(TCHAR* szFileName)
{
	if(m_oStr->IsEmpty(szFileName))
		return;

	DWORD dwAttributes = GetFileAttributes(szFileName);
	if(dwAttributes & FILE_ATTRIBUTE_DIRECTORY && dwAttributes != ((DWORD)-1))
		return;
	else if(dwAttributes != ((DWORD)-1))
	{
		// file exists so put up the question to overwrite
		if(IDYES != MessageBox(m_hWnd, m_oStr->GetText(ID(IDS_MSG_FileExists), m_hInst), m_oStr->GetText(ID(IDS_MSG_Warning), m_hInst), MB_YESNO | MB_ICONQUESTION))
			return;
	}

	// copy the file and save
	m_oStr->StringCopy(m_sOpenFile->lpstrFile, szFileName);
	EndDialog(IDOK);
}


//////////////////////////////////////////////////////////////////////////
// Implementation of CtrlBrowse
//////////////////////////////////////////////////////////////////////////

CCtrlBrowse::CCtrlBrowse(void)
:m_szSelectedFullPath(NULL)
,m_szSelectedPath(NULL)
{
	m_oStr->Empty(m_szPathFolder);
	m_oStr->Empty(m_szFilter);
}

CCtrlBrowse::~CCtrlBrowse(void)
{
	Destroy();
}

void CCtrlBrowse::Destroy()
{
	if(m_hWnd && IsWindow(m_hWnd))
	{
		ListView_DeleteAllItems(m_hWnd);
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
	m_oStr->Delete(&m_szSelectedPath);
	m_oStr->Delete(&m_szSelectedFullPath);
}

BOOL CCtrlBrowse::Create(HWND hWndParent, HINSTANCE hInst, HFONT hFont)
{
	m_hInst = hInst;
	m_bHandlePaint	= FALSE;
	m_eWndType		= WND_Window;

	if ((m_hWnd = CreateWindowEx(0,
		WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER|
		WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL /*| LVS_OWNERDRAWFIXED | LVS_OWNERDATA*/ | LVS_NOCOLUMNHEADER, 
		0, 0, 0, 0, hWndParent,
		(HMENU) NULL, hInst, NULL)) == NULL)
		return FALSE;

	//and now override the procs
	SetWindowLong( m_hWnd, GWL_USERDATA, reinterpret_cast<long>( this ) ); 
	m_lpfnCtrl = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)ProcWnd);

	if(hFont)
		SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, NULL);

	//
	//  We only need to create one column
	//
	LV_COLUMN LVColumn = {0};

	LVColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

	LVColumn.fmt		= LVCFMT_LEFT;
	LVColumn.cx			= 35;		// we'll change this later
	LVColumn.iSubItem	= 0;
	ListView_InsertColumn(m_hWnd, 0, &LVColumn);

	SHFILEINFO sfi = {0};
	HIMAGELIST himlSmall = NULL;

	himlSmall = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	if (himlSmall)
		ListView_SetImageList(m_hWnd, himlSmall, LVSIL_SMALL);


	return TRUE;
}

BOOL CCtrlBrowse::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_UP:
	{
		int iIndex = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
		if(iIndex == 0)
			PostMessage(GetParent(m_hWnd), WM_ControlKeyUp, 0,0);
		break;
	}
	case VK_DOWN:
	{
		int iIndex = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
		if(iIndex == ListView_GetItemCount(m_hWnd)-1)
			PostMessage(GetParent(m_hWnd), WM_ControlKeyDown, 0,0);
		break;
	}
	case VK_RIGHT:
		PostMessage(GetParent(m_hWnd), WM_ControlKeyDown, 0,0);
		break;
	case VK_LEFT:
		PostMessage(GetParent(m_hWnd), WM_ControlKeyUp, 0,0);
		break;
	}
	return UNHANDLED;
}

BOOL CCtrlBrowse::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_GETDLGCODE)
		return (BOOL) (DLGC_WANTALLKEYS);

	return CallWindowProc(m_lpfnCtrl, hWnd, message, wParam, lParam);
}

BOOL CCtrlBrowse::SetFilter(TCHAR* szFilter)
{
	if(m_oStr->IsEmpty(szFilter))
		return FALSE;

	m_oStr->StringCopy(m_szFilter, szFilter);

	return RefreshList();
}

BOOL CCtrlBrowse::SetCurrentFolder(TCHAR* szDir)
{
	// remove all the old stuff
	ListView_DeleteAllItems(m_hWnd);
	m_oStr->Empty(m_szPathFolder);

	TCHAR szPath[MAX_PATH];
	if(m_oStr->IsEmpty(szDir))
		m_oStr->StringCopy(szPath, _T("\\"));
	else
	{
		m_oStr->StringCopy(szPath, szDir);

		// make a quick check to see if there is a \ at the end
		if(m_oStr->GetLast(szPath) == _T('\\') && m_oStr->GetLength(szPath) > 1)
			m_oStr->Delete(m_oStr->GetLength(szPath)-1, 1, szPath);
	}

	// see if this new path is valid
	DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr == 0xFFFFFFFF || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;

	// copy the path over
	m_oStr->StringCopy(m_szPathFolder, szPath);

	return RefreshList();
}

BOOL CCtrlBrowse::RefreshList()
{
	if(!m_hWnd)
		return FALSE;

	HCURSOR hCursor = ::SetCursor(LoadCursor(NULL, IDC_WAIT));
	SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

	// remove all the old stuff
	ListView_DeleteAllItems(m_hWnd);

	TCHAR szPath[MAX_PATH];
	m_oStr->StringCopy(szPath, m_szPathFolder);

	WIN32_FIND_DATA wfd;
	if(!IsRootPath())
	{
		m_oStr->StringCopy(wfd.cFileName, TXT_ReturnPath);
		AddListItem(wfd);

		m_oStr->Concatenate(szPath, _T("\\*.*"));
	}
	else
		m_oStr->Concatenate(szPath, _T("*.*"));

	// look for folders only
	HANDLE hFind = FindFirstFile(szPath, &wfd);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				AddListItem(wfd);
			/*else if (m_listExt.IsEmpty())
			AddListItem(szPath, &wfd);
			else if (IsFolderShortcut(szPath, wfd.cFileName))
			AddListItem(szPath, &wfd);*/
		}
		while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}

	if(!m_oStr->IsEmpty(m_szFilter))
	{
		m_oStr->StringCopy(szPath, m_szPathFolder);

		if(!IsRootPath())
		{
			m_oStr->Concatenate(szPath, _T("\\"));
			m_oStr->Concatenate(szPath, m_szFilter);
		}
		else
			m_oStr->Concatenate(szPath, m_szFilter);

		// look for the files
		HANDLE hFind = FindFirstFile(szPath, &wfd);
		if (hFind != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					AddListItem(wfd);
			}
			while (FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}

	}

	// tell the parent to change the directory name
	PostMessage(GetParent(m_hWnd), WM_UpdateDirectory, 0,0);

	// sort by name
	ListView_SortItems(m_hWnd, CompareItems, this);

	SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iColumnWidth = rcClient.right-rcClient.left;
	if(GetWindowLong(m_hWnd, GWL_STYLE) & WS_VSCROLL)
		iColumnWidth -= GetSystemMetrics(SM_CXVSCROLL);

	ListView_SetColumnWidth(m_hWnd, 0, iColumnWidth);
	::SetCursor(hCursor);

	return TRUE;
}

int CCtrlBrowse::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	CCtrlBrowse* pThis	= (CCtrlBrowse*)lParamSort;
	TypeFiles* sFile1		= (TypeFiles*)lParam1;
	TypeFiles* sFile2		= (TypeFiles*)lParam2;

	if(!pThis || !sFile1 || !sFile2)
		return 0;

	if(pThis->IsReturnPath(sFile1->szFileName))
		return -1;
	else if(pThis->IsReturnPath(sFile2->szFileName))
		return 1;
	else if((sFile1->dwAttributes&FILE_ATTRIBUTE_DIRECTORY) && !(sFile2->dwAttributes&FILE_ATTRIBUTE_DIRECTORY))
		return -1;
	else if(!(sFile1->dwAttributes&FILE_ATTRIBUTE_DIRECTORY) && (sFile2->dwAttributes&FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	else 
		return pThis->m_oStr->CompareNoCase(sFile1->szFileName, sFile2->szFileName);
}

TCHAR* CCtrlBrowse::GetCurrentFolder()
{
	return m_szPathFolder;
}

BOOL CCtrlBrowse::GetCurrentFolderName(TCHAR* szFolderName)
{
	if(!szFolderName)
		return FALSE;

	m_oStr->Empty(szFolderName);

	if(m_oStr->IsEmpty(m_szPathFolder))
		return FALSE;

	if(IsRootPath())
	{
		m_oStr->StringCopy(szFolderName, m_oStr->GetText(ID(IDS_INFO_MyDevice), m_hInst));
		return TRUE;
	}

	int iIndex = m_oStr->FindLastOccurance(m_szPathFolder, _T("\\"));
	if(-1 == iIndex)
		return FALSE;

	m_oStr->StringCopy(szFolderName, m_szPathFolder, iIndex + 1, m_oStr->GetLength(m_szPathFolder) - iIndex - 1);

	return TRUE;
}

void CCtrlBrowse::AddListItem(WIN32_FIND_DATA& pwfd)
{
	TypeFiles* sFiles = new TypeFiles;
	ZeroMemory(sFiles, sizeof(TypeFiles));

	sFiles->dwAttributes	= pwfd.dwFileAttributes;
	sFiles->szFileName	= m_oStr->CreateAndCopy(pwfd.cFileName);;

	// i could save the extra info but I won't right now
	LVITEM lvItem	= {0};
	lvItem.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvItem.pszText	= LPSTR_TEXTCALLBACK;
	lvItem.iImage	= I_IMAGECALLBACK;
	lvItem.iItem	= ListView_GetItemCount(m_hWnd);
	lvItem.lParam	= (LPARAM) sFiles;

	lvItem.iItem = ListView_InsertItem(m_hWnd, &lvItem);	
}

BOOL CCtrlBrowse::OnNotifyReflect(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmh = (LPNMHDR)lParam;

	switch(lpnmh->code)
	{
	case LVN_DELETEITEM:
	{
		NMLISTVIEW* nmlv = (NMLISTVIEW*)lpnmh;
		if(nmlv->lParam)
		{
			TypeFiles* sFile = (TypeFiles*)nmlv->lParam;
			m_oStr->Delete(&sFile->szFileName);
			delete sFile;
		}
		break;
	}
	case LVN_ENDLABELEDIT:
		//return ApplyLabelEdit((NMLVDISPINFO*)lpnmh);
		break;
	case LVN_BEGINLABELEDIT:
		//return BeginLabelEdit((NMLVDISPINFO*)lpnmh);
		break;
	case LVN_ITEMCHANGED:
		OnItemChanged();
		break;
	case LVN_GETDISPINFO:
		OnGetDisplayInfo((NMLVDISPINFO *)lpnmh);
		break;
	case NM_CLICK:
	//case NM_DBLCLK:
	case NM_RETURN:	
		OnSelectItem();
		break;
	case NM_RECOGNIZEGESTURE:
		return TRUE;	// don't accept gestures here
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CCtrlBrowse::OnItemChanged()
{
	LVITEM lvi = {0};
	lvi.mask		= LVIF_PARAM;
	lvi.iItem = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
	if(-1 == lvi.iItem)
	if (!ListView_GetItem(m_hWnd, &lvi))
		return FALSE;

	if (!ListView_GetItem(m_hWnd, &lvi))
		return FALSE;

	TCHAR* szPath = (TCHAR*)lvi.lParam;
	if(szPath == NULL)
		return FALSE;

	if(m_oStr->Compare(szPath, TXT_ReturnPath) == 0)
		return FALSE;

	TCHAR szFullPath[MAX_PATH];

	m_oStr->StringCopy(szFullPath, m_szPathFolder);

	if(!IsRootPath())
		m_oStr->Concatenate(szFullPath, _T("\\"));
	m_oStr->Concatenate(szFullPath, szPath);

	// check if we're dealing with a file properly
	DWORD dwFileAttributes = GetFileAttributes(szFullPath);
	if(dwFileAttributes == ((DWORD)-1) ||
	   dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	   return FALSE;

	// we know we're dealing with a file
	// strip off the extension
	TCHAR szExt[STRING_SMALL];
	TCHAR szNewFileName[MAX_PATH];
	m_oStr->StringCopy(szNewFileName, szPath);
	m_oStr->StringCopy(szExt, m_szFilter);
	m_oStr->Delete(0,1, szExt);
	int iIndex = m_oStr->Find(szPath, szExt);
	if(iIndex != -1)
		m_oStr->Delete(iIndex, m_oStr->GetLength(szExt), szNewFileName);
	SendMessage(GetParent(m_hWnd), WM_UpdateName, 0, (WPARAM)szNewFileName);	

	return TRUE;
}

void CCtrlBrowse::OnSelectItem()
{
	LVITEM lvi = {0};
	lvi.mask		= LVIF_PARAM;
	lvi.iItem = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
	if(-1 == lvi.iItem)
		return;

	if (!ListView_GetItem(m_hWnd, &lvi))
		return;

	TypeFiles* sFile = (TypeFiles*)lvi.lParam;
	TCHAR* szPath = sFile->szFileName;
	if(szPath == NULL)
		return;

	if(IsReturnPath(szPath))
	{
		GoUpDirectory();
		return;
	}

	TCHAR szFullPath[MAX_PATH];
	m_oStr->StringCopy(szFullPath, m_szPathFolder);

	if(!IsRootPath())
		m_oStr->Concatenate(szFullPath, _T("\\"));
	m_oStr->Concatenate(szFullPath, szPath);

	// check if we're dealing with a file properly
	DWORD dwFileAttributes = GetFileAttributes(szFullPath);
	if(sFile->dwAttributes == ((DWORD)-1))
		return;

	// if it's a folder then go into that folder
	if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		m_oStr->StringCopy(m_szPathFolder, szFullPath);
		RefreshList();
		return;
	}

	m_oStr->Delete(&m_szSelectedPath);
	m_oStr->Delete(&m_szSelectedFullPath);
	m_szSelectedPath	= m_oStr->CreateAndCopy(szPath);
	m_szSelectedFullPath= m_oStr->CreateAndCopy(szFullPath);

	// we know it's a file so select the file
	PostMessage(GetParent(m_hWnd), WM_SelectFile, (WPARAM)NULL, (LPARAM)NULL);
}

void CCtrlBrowse::GoUpDirectory()
{
	int iIndex = m_oStr->FindLastOccurance(m_szPathFolder, _T("\\"));
	if(-1 == iIndex)
		return;

	// take off the last directory
	m_oStr->Delete(iIndex, m_oStr->GetLength(m_szPathFolder)-iIndex, m_szPathFolder);
	if(m_oStr->IsEmpty(m_szPathFolder))
		m_oStr->StringCopy(m_szPathFolder, _T("\\"));

	// refresh the list
	RefreshList();
}

void CCtrlBrowse::OnGetDisplayInfo(NMLVDISPINFO * lpdi)
{
	if(m_oStr->IsEmpty(m_szPathFolder) || lpdi->item.lParam == NULL)
		return;

	// Tell the listview to store the data
	lpdi->item.mask |= LVIF_DI_SETITEM;

	TypeFiles* sFile = (TypeFiles*)lpdi->item.lParam;

	if(lpdi->item.mask & LVIF_IMAGE)
	{
		// check if this is the .. path
		if(IsReturnPath(sFile->szFileName))
		{
			lpdi->item.iImage = 0;	// for the directory
			if(lpdi->item.mask & LVIF_TEXT)
				m_oStr->StringCopy(lpdi->item.pszText, sFile->szFileName);
			return;
		}

		SHFILEINFO sfi = {0};
		TCHAR szFileName[MAX_PATH];
		m_oStr->StringCopy(szFileName, m_szPathFolder);
		if(!IsRootPath())
			m_oStr->Concatenate(szFileName, _T("\\"));
		m_oStr->Concatenate(szFileName, sFile->szFileName);

		UINT uFlags = SHGFI_SYSICONINDEX | SHGFI_LARGEICON;

		// get the icon index
		if (SHGetFileInfo(szFileName, 0, 
						  &sfi, sizeof(SHFILEINFO), 
						  SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
		{
			// set correct icon
		/*	if (sfi.dwAttributes & SFGAO_GHOSTED)
			{
				lpdi->item.mask		|= LVIF_STATE;
				lpdi->item.stateMask|= LVIS_CUT;
				lpdi->item.state	|= LVIS_CUT;
			}
			if (sfi.dwAttributes & SFGAO_SHARE)
			{
				lpdi->item.mask		|= LVIF_STATE;
				lpdi->item.state	&= ~LVIS_OVERLAYMASK;
				lpdi->item.state	|= INDEXTOOVERLAYMASK(1);
				lpdi->item.stateMask|= LVIS_OVERLAYMASK;
			}
			else if (sfi.dwAttributes & SFGAO_LINK)
			{
				lpdi->item.mask		|= LVIF_STATE;
				lpdi->item.state	&= ~LVIS_OVERLAYMASK;
				lpdi->item.state	|= INDEXTOOVERLAYMASK(2);
				lpdi->item.stateMask|= LVIS_OVERLAYMASK;
			}*/
			lpdi->item.iImage = sfi.iIcon;
		}
	}
	if(lpdi->item.mask & LVIF_TEXT)
		m_oStr->StringCopy(lpdi->item.pszText, sFile->szFileName);

}

BOOL CCtrlBrowse::IsRootPath()
{
	return (m_oStr->Compare(m_szPathFolder, _T("\\"))==0);
}

BOOL CCtrlBrowse::IsReturnPath(TCHAR* szPath)
{
	return (m_oStr->Compare(szPath, TXT_ReturnPath)==0);
}

