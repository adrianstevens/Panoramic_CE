#include "path.h"
#include "StdAfx.h"
#include "GuiFavorites.h"
#include "IssRegistry.h"
#include "IssRect.h"
#include "IssCommon.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "ContactsGuiDefines.h"
#include "IssGDIDraw.h"
#include "IssWndTouchMenu.h"
#include "ContactsUtil.h"
#include "issdebug.h"
#include "DlgOptions.h"
#include "DlgGetFile.h"
#include "IssLocalisation.h"
#include "DlgChooseContact.h"
#include "DlgMsgBox.h"
#include "Path.h"
#include "pm.h"

#include "ObjGui.h"
#include "DlgOptions.h"

#include "Shlobj.h"
#include <Cpl.h>

#ifndef CPL_IDNAME
#define CPL_IDNAME    100
#endif

typedef LONG (*CPLAPPLET) (HWND hwndCPl, UINT  msg, LPARAM  lParam1, LPARAM  lParam2);

#define IDT_GrabAndMove			5500
#define IDMENU_Select			3000
#define IDMENU_AddNewItem		3001
#define IDMENU_SetTitle			3002

#define GRID_Undefined			255
#define REG_Save_Location		_T("SOFTWARE\\Pano\\Launcher\\Favs\\%d")
#define REGVAL_Grid				_T("Grid")
#define REGVAL_Title            _T("Title")
#define REGVAL_NumItems			_T("NumItems")
#define REGVAL_Item				_T("Item%d")

#define MOVE_Factor				(m_iIconSize/4)

struct TypeRegFavItem
{
	TCHAR	szFriendlyName[STRING_MAX];
	TCHAR	szPath[STRING_MAX];
	int		iIndex;
	EnumFavType eType;
};

extern CObjGui*				g_gui;
extern TypeOptions*			g_sOptions;
extern CIssWndTouchMenu*	g_wndMenu;

TypeFavItem::TypeFavItem()
:eType(FAV_Program)
,szFriendlyName(NULL)
,szPath(NULL)
,iIndex(-1)
,sPic(NULL)
,bDrawName(FALSE)
{
	SetRect(&rcLoc, 0,0,0,0);
}

TypeFavItem::~TypeFavItem()
{
	Destroy();
}

void TypeFavItem::Destroy()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szFriendlyName);
	oStr->Delete(&szPath);
	iIndex	= -1;
	eType	= FAV_Program;
	bDrawName = FALSE;
	SetRect(&rcLoc, 0,0,0,0);

    if(sPic)
        sPic->bIsDeleted = TRUE;
    sPic = NULL;
}

void TypeFavItem::Launch(HWND hWnd)
{
	CIssString* oStr = CIssString::Instance();
	if(oStr->IsEmpty(szPath))
	{
		MessageBeep(MB_ICONHAND);
		return;
	}

	if(eType == FAV_Program)
	{
		if(0 == oStr->Compare(oStr->GetText(IDS_Today), szPath))
		{
			HWND hWndDesktop = FindWindow(_T("DesktopExplorerWindow"), NULL);
			if(hWndDesktop)
				SetForegroundWindow((HWND)((ULONG) hWndDesktop | 0x00000001));
		}
		else
		{
			SHELLEXECUTEINFO sei = {0};
			sei.cbSize	    = sizeof(sei);
			sei.hwnd	    = hWnd;
			sei.nShow	    = SW_SHOWNORMAL;
			sei.lpFile      = szPath;
			ShellExecuteEx(&sei);
		}
	}
	else if(eType == FAV_FileOrFolder)
	{
		DWORD dwFileAttributes = GetFileAttributes(szPath);
		if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CreateProcess(_T("fexplore.exe"), 
				szPath,
				NULL, NULL, 
				FALSE, 0, 
				NULL, NULL, 
				NULL, NULL);
		}
		else
		{
			SHELLEXECUTEINFO sei = {0};
			sei.cbSize	    = sizeof(sei);
			sei.hwnd	    = hWnd;
			sei.nShow	    = SW_SHOWNORMAL;
			sei.lpFile      = szPath;
			ShellExecuteEx(&sei);
		}
	}
	else if(eType == FAV_Setting)
	{
		SHELLEXECUTEINFO sei = {0};
		sei.cbSize	    = sizeof(sei);
		sei.hwnd	    = hWnd;
		sei.nShow	    = SW_SHOWNORMAL;
		sei.lpFile      = _T("\\Windows\\ctlpnl.exe");
		sei.lpParameters= szPath;
		ShellExecuteEx(&sei);
	}
	
}

TypePage::TypePage()
:eTitle(TTL_Count)
{
	for(int x=0; x<MAX_Items; x++)
		for(int y=0; y<MAX_Items; y++)
			arrGrid[x][y] = GRID_Undefined;
}

TypePage::~TypePage()
{
	Destroy();
}

void TypePage::Destroy()
{
	for(int x=0; x<MAX_Items; x++)
		for(int y=0; y<MAX_Items; y++)
			arrGrid[x][y] = GRID_Undefined;

	for(int i=0; i<arrFavs.GetSize(); i++)
	{
		TypeFavItem* sFav = arrFavs[i];
		if(sFav)
			delete sFav;
	}
	arrFavs.RemoveAll();
}


CGuiFavorites::CGuiFavorites(void)
:m_iNumHorz(1)
,m_iNumVert(1)
,m_iHorzSpacing(1)
,m_iVertSpacing(1)
,m_iIconSize(1)
,m_bGrabAndMove(FALSE)
,m_hTitleLan(NULL)
,m_dwTickSaveContent(0)
{
	m_ptSelectedItem.x = UND;
	m_ptSelectedItem.y = UND;

	InitializeCriticalSection(&m_crImages);

}

CGuiFavorites::~CGuiFavorites(void)
{
    //CDlgChoosePrograms::ResetListContent();
    //CDlgChooseSettings::ResetListContent();
	SaveRegistry();
	Destroy();

	CIssGDIEx::DeleteFont(m_hTitleLan);
	DeleteCriticalSection(&m_crImages);
}

BOOL CGuiFavorites::Destroy()
{
   DeletePages();

	return TRUE;
}

void CGuiFavorites::DeletePages()
{
	for(int i=0; i<m_arrPages.GetSize(); i++)
	{
		TypePage* sPage = m_arrPages[i];
		if(!sPage)
			continue;
		delete sPage;
	}
	m_arrPages.RemoveAll();

	m_oPicControl.ResetContent();
}

BOOL CGuiFavorites::Init(HWND hWndParent, HINSTANCE hInst)
{
	CGuiBase::Init(hWndParent, hInst);

    PreloadImages(hWndParent, m_hInst);

	m_oPicControl.Initialize(hWndParent, hWndParent, hInst, OPTION_SnapCenter|OPTION_SlidePerPage);
	m_oPicControl.SetCustomDrawFunc(this, DrawListItem, NULL, NULL);
	m_oPicControl.SetDeleteItemFunc(DeleteMyPicListItem);

    m_oPicLoader.StartEngine(hWndParent, hInst);

#ifndef NOPHONE
	m_dlgContactDetails.PreloadImages(hWndParent, hInst);
#endif

	ReadRegistry();

    return TRUE;
}

BOOL CGuiFavorites::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

void CGuiFavorites::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
	m_gdiSelector.Destroy();

#ifndef NOPHONE
	m_dlgContactDetails.ReloadColorSchemeItems(m_hWndParent, m_hInst);
#endif
}

HRESULT CGuiFavorites::PreloadImages(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

//Error:
    return hr;
}

BOOL CGuiFavorites::AddMenuItems()
{
	TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
	if(!sPage)
		return TRUE;

	if(IsValidGrid(m_ptSelectedItem))
	{
		int iIndex = sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y];
		TypeFavItem* sFav = sPage->arrFavs[iIndex];
		if(sFav)
		{
			g_wndMenu->AddItem(ID(IDS_MENU_RemoveItem), m_hInst, IDMENU_RemoveItem, NULL);
			g_wndMenu->AddSeparator();
		}
		else
		{
			g_wndMenu->AddItem(ID(IDS_MENU_AddProgram),  m_hInst,   IDMENU_AddProgram);
			g_wndMenu->AddItem(ID(IDS_MENU_AddSetting),  m_hInst,   IDMENU_AddSetting);
			g_wndMenu->AddItem(ID(IDS_MENU_AddFileFolder), m_hInst, IDMENU_AddFileFolder);
#ifndef NOPHONE
			g_wndMenu->AddItem(ID(IDS_MENU_AddContact), m_hInst, IDMENU_NewContact);
#endif
			g_wndMenu->AddSeparator();
		}

	}

	return TRUE;
}
   
BOOL CGuiFavorites::MoveGui(RECT rcLocation)
{
	if(EqualRect(&rcLocation, &m_rcLocation))
		return TRUE;

	CGuiBase::MoveGui(rcLocation);

	m_rcTitle		= rcLocation;
	m_rcPicControl	= rcLocation;
	if(g_gui->IsLandscape())
	{
		m_rcTitle.left		= m_rcTitle.right - TITLE_Height;
		m_rcPicControl.right= m_rcTitle.left - 1;
		m_rcRunningPrograms = m_rcTitle;
		m_rcRunningPrograms.bottom = m_rcRunningPrograms.top + 2*WIDTH(m_rcRunningPrograms);
	}
	else
	{
		m_rcTitle.bottom	= m_rcTitle.top + TITLE_Height;
		m_rcPicControl.top	= m_rcTitle.bottom + 1;
		m_rcRunningPrograms = m_rcTitle;
		m_rcRunningPrograms.right = m_rcRunningPrograms.left + 2*HEIGHT(m_rcRunningPrograms);
	}

	m_oPicControl.SetIndentSize(0);
	m_oPicControl.SetItemSize(WIDTH(m_rcPicControl), HEIGHT(m_rcPicControl));
	m_oPicControl.OnSize(m_rcPicControl.left, m_rcPicControl.top, WIDTH(m_rcPicControl), HEIGHT(m_rcPicControl));	

	SetIconSize();
   
	return TRUE;
}



HRESULT CGuiFavorites::ReadRegistry()
{
	HRESULT hr = S_OK;

	HKEY hKey = NULL;
	LRESULT lResult;
	TCHAR szKey[STRING_MAX];
	int iNumPages = 0;
	TypePage* sPage;
	DWORD dwDataSize;
	DWORD dwType;
	DWORD dwNumItems;
	TypeRegFavItem sRegItem;

	// start over
	DeletePages();

	// first count how many pages there are
	for(int i=MAX_NUM_Pages-1; i>=0; i--)
	{
		m_oStr->Format(szKey, REG_Save_Location, i);
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER,szKey,0,KEY_QUERY_VALUE,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			iNumPages = i+1;
			RegCloseKey(hKey);
			hKey = NULL;
			break;
		}
	}

	if(iNumPages > 0)
	{
		for(int i=0; i<iNumPages; i++)
		{
			hr = AddPage();
			CHR(hr, _T("hr = AddPage();"));
		}
	}
	else
	{
		iNumPages = 2;
		hr = EnsurePages();
		CHR(hr, _T("hr = EnsurePages();"));
	}

	for(int i=0; i<iNumPages; i++)
	{
		if(hKey)
		{
			RegCloseKey(hKey);
			hKey = NULL;
		}

		sPage = m_arrPages[i];
		if(!sPage)
			continue;

		m_oStr->Format(szKey, REG_Save_Location, i);
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER,szKey,0,KEY_QUERY_VALUE,&hKey);
		if(lResult != ERROR_SUCCESS)
			continue;

		dwDataSize = sizeof(sPage->arrGrid);
		dwType = REG_BINARY;
		lResult = RegQueryValueEx(hKey, REGVAL_Grid, NULL, &dwType, (LPBYTE)sPage->arrGrid, &dwDataSize);
		if(lResult != ERROR_SUCCESS || dwDataSize != sizeof(sPage->arrGrid))
			continue;

        dwDataSize = sizeof(DWORD);
        dwType = REG_DWORD;
        lResult = RegQueryValueEx(hKey, REGVAL_Title, NULL, &dwType, (LPBYTE)&dwNumItems, &dwDataSize);
        if(lResult != ERROR_SUCCESS || dwDataSize != sizeof(DWORD))
            continue;

        sPage->eTitle = (EnumTitleName)dwNumItems;

		dwNumItems = 0;
		dwDataSize = sizeof(DWORD);
		dwType = REG_DWORD;
		lResult = RegQueryValueEx(hKey, REGVAL_NumItems, NULL, &dwType, (LPBYTE)&dwNumItems, &dwDataSize);
		if(lResult != ERROR_SUCCESS)
			continue;

		for(int j=0; j<(int)dwNumItems; j++)
		{
			ZeroMemory(&sRegItem, sizeof(TypeRegFavItem));
			dwDataSize = sizeof(TypeRegFavItem);
			dwType = REG_BINARY;

			m_oStr->Format(szKey, REGVAL_Item, j);
			lResult = RegQueryValueEx(hKey, szKey, NULL, &dwType, (LPBYTE)&sRegItem, &dwDataSize);
			if(lResult != ERROR_SUCCESS || dwDataSize != sizeof(TypeRegFavItem) || S_OK != AddItem(sRegItem.szFriendlyName, sRegItem.szPath, sRegItem.iIndex, sRegItem.eType, sPage))
			{	
				// problem reading or creating the item, so clear out the page
				sPage->Destroy();
				break;
			}
		}
	}

	// add an extra page if needed
	EnsurePages();

Error:

	m_oPicControl.SetSelectedItemIndex(0, TRUE);

	if(hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return hr;
}

HRESULT CGuiFavorites::SaveRegistry()
{
    HRESULT hr = S_OK;
	TCHAR szKey[STRING_MAX];
	TypePage* sPage;

	for(int i=MAX_NUM_Pages-1; i>=0; i--)
	{
		sPage = m_arrPages[i];
		if(sPage)
			SaveRegistryPage(i);
		else
		{
			m_oStr->Format(szKey, REG_Save_Location, i);
			RegDeleteKeyNT(HKEY_CURRENT_USER, szKey);
		}
	}

    return hr;
}

HRESULT CGuiFavorites::SaveRegistryPage(int iIndex, BOOL bSaveGridOnly)
{
	HRESULT hr = S_OK;
	LRESULT lResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	TCHAR szKey[STRING_MAX];
	DWORD dwType, dwDataSize, dwNum;
	TypePage* sPage = m_arrPages[iIndex];
	TypeRegFavItem sSaveItem;
	TypeFavItem* sFav;

	CPHR(sPage, _T(""));

	m_oStr->Format(szKey, REG_Save_Location, iIndex);

	if(!bSaveGridOnly)
		RegDeleteKeyNT(HKEY_CURRENT_USER, szKey);

	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, szKey, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition);
	CBARG(lResult == ERROR_SUCCESS, _T(""));

	dwType		= REG_BINARY;
	dwDataSize	= sizeof(sPage->arrGrid);
	lResult = RegSetValueEx(hKey, REGVAL_Grid, NULL, dwType, (LPBYTE)&sPage->arrGrid, dwDataSize);
	CBARG(lResult == ERROR_SUCCESS, _T(""));

    dwType      = REG_DWORD;
    dwDataSize  = sizeof(DWORD);
    dwNum       = (DWORD)sPage->eTitle;
    lResult = RegSetValueEx(hKey, REGVAL_Title, NULL, dwType, (LPBYTE)&dwNum, dwDataSize);
    CBARG(lResult == ERROR_SUCCESS, _T(""));

	if(bSaveGridOnly)
		goto Error;

	dwType		= REG_DWORD;
	dwDataSize	= sizeof(DWORD);
	dwNum		= sPage->arrFavs.GetSize();
	lResult = RegSetValueEx(hKey, REGVAL_NumItems, NULL, dwType, (LPBYTE)&dwNum, dwDataSize);
	CBARG(lResult == ERROR_SUCCESS, _T(""));

	for(int i=0; i<sPage->arrFavs.GetSize(); i++)
	{
		sFav = sPage->arrFavs[i];
		CPHR(sFav, _T(""));

		ZeroMemory(&sSaveItem, sizeof(TypeRegFavItem));
		m_oStr->StringCopy(sSaveItem.szFriendlyName, sFav->szFriendlyName);
		m_oStr->StringCopy(sSaveItem.szPath, sFav->szPath);
		sSaveItem.iIndex = sFav->iIndex;
		sSaveItem.eType	 = sFav->eType;

		m_oStr->Format(szKey, REGVAL_Item, i);

		dwType		= REG_BINARY;
		dwDataSize	= sizeof(TypeRegFavItem);
		lResult = RegSetValueEx(hKey, szKey, NULL, dwType, (LPBYTE)&sSaveItem, dwDataSize);
		CBARG(lResult == ERROR_SUCCESS, _T(""));
	}

Error:
	if(hKey)
	{
		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}

	return hr;
}

void CGuiFavorites::OnAddNewItem()
{
	g_wndMenu->CloseWindow();
	g_wndMenu->ResetlastTickUsed();

	g_wndMenu->ResetContent();
    g_wndMenu->AddCategory(ID(IDS_MENU_AddFavorite), m_hInst);

    g_wndMenu->AddItem(ID(IDS_MENU_AddProgram), m_hInst, IDMENU_AddProgram);
    g_wndMenu->AddItem(ID(IDS_MENU_AddSetting), m_hInst, IDMENU_AddSetting);
    g_wndMenu->AddItem(ID(IDS_MENU_AddFileFolder), m_hInst, IDMENU_AddFileFolder);
#ifndef NOPHONE
	g_wndMenu->AddItem(ID(IDS_MENU_AddContact), m_hInst, IDMENU_NewContact);
#endif
    g_wndMenu->SetSelectedItemIndex(0, TRUE);

    RECT rcWindow;
    GetWindowRect(m_hWndParent, &rcWindow);

    int iWidth = WIDTH(GetFrame())*5/6;

    int iY = HEIGHT(GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

    g_wndMenu->PopupMenu(m_hWndParent, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        GetFrame().left + (WIDTH(GetFrame()) - iWidth)/2, rcWindow.top,
        iWidth, iY,
        0, 0, 0,0, ADJUST_Bottom);

}

void CGuiFavorites::OnRunningPrograms()
{
	::SetCursor(LoadCursor(NULL, IDC_WAIT));

	m_objWindows.RefreshWindowsOnly();

	g_wndMenu->CloseWindow();
	g_wndMenu->ResetlastTickUsed();
	g_wndMenu->SetCustomDrawFunc(DrawRunningPrograms, this);
	g_wndMenu->SetDeleteItemFunc(DeleteRunningProgramItem);

	g_wndMenu->ResetContent();
	//g_wndMenu->AddCategory(_T("Running Programs"));

	int iHeight = GetSystemMetrics(SM_CYSMICON) + INDENT;

	BOOL bAddedItem = FALSE;
	for(int i=0; i<m_objWindows.GetProcessCount(); i++)
	{
		TypeProcessInfo* sInfo = m_objWindows.GetProcess(i);
		if(sInfo && sInfo->hWnd && !m_oStr->IsEmpty(sInfo->szWindowTitle))
		{
			bAddedItem = TRUE;
			g_wndMenu->AddItem((LPVOID)sInfo, IDMENU_RunningPrograms);

			// make sure it's high enough
			TypeItems* sAdded = g_wndMenu->GetItem(g_wndMenu->GetItemCount()-1);
			if(sAdded && (sAdded->iHeight < iHeight || sAdded->iHeightSel < iHeight))
				sAdded->iHeight = sAdded->iHeightSel = iHeight;
		}
	}

	if(bAddedItem)
		g_wndMenu->AddSeparator();
	g_wndMenu->AddItem(ID(IDS_MENU_CloseAll), m_hInst, IDMENU_CloseAll, bAddedItem?NULL:FLAG_Grayed);
	g_wndMenu->AddItem(ID(IDS_MENU_SoftReset), m_hInst, IDMENU_SoftReset);

	g_wndMenu->SetSelectedItemIndex(0, TRUE);

	RECT rcWindow;
	GetWindowRect(m_hWndParent, &rcWindow);

	int iWidth = WIDTH(GetFrame())*7/8;

	int iY = HEIGHT(GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

	::SetCursor(NULL);

	g_wndMenu->PopupMenu(m_hWndParent, m_hInst,
		OPTION_AlwaysShowSelector|OPTION_CircularList,
		GetFrame().left + (WIDTH(GetFrame()) - iWidth)/2, rcWindow.top,
		iWidth, iY,
		0, 0, 0,0, ADJUST_Bottom);

}

void CGuiFavorites::DeleteRunningProgramItem(LPVOID lpItem)
{
	// do nothing
}

void CGuiFavorites::DrawRunningPrograms(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
	CGuiFavorites* pGui = (CGuiFavorites*)lpClass;
	if(!pGui || !sItem)
		return;

	TypeProcessInfo* sInfo = (TypeProcessInfo*)sItem->lpItem;
	pGui->DrawMyRunningPrograms(gdi, rcDraw, sInfo);
}

void CGuiFavorites::DrawMyRunningPrograms(CIssGDIEx& gdi, RECT& rcDraw, TypeProcessInfo* sInfo)
{
	RECT rc = rcDraw;
	//rc.left += INDENT/2;
    rc.left += INDENT;
	rc.right-= INDENT;

	//Draw the app icon
	SHFILEINFO sFileInfo = {0};
	DWORD dwResult = SHGetFileInfo(sInfo->szFullName, 0L, &sFileInfo, sizeof(sFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
	if(dwResult != 0 && sFileInfo.hIcon != INVALID_HANDLE_VALUE && sFileInfo.hIcon) //Unsure what hIcon returns if no icon.
	{
		DrawIcon(gdi.GetDC(),
			rc.left,
			rc.top + (HEIGHT(rc) - GetSystemMetrics(SM_CYSMICON))/2,
			sFileInfo.hIcon);		
	}

	rc.left = rc.left + GetSystemMetrics(SM_CYSMICON) + INDENT/2;

	if(m_gdiCloseIcon.GetDC() == NULL)
		m_gdiCloseIcon.LoadImage(IsVGA()?IDR_Close_VGA:IDR_Close, m_hWndParent, m_hInst, TRUE);

	// draw the close icon
	RECT rcClose = rc;
	rcClose.left = rcClose.right - m_gdiCloseIcon.GetWidth();
	::Draw(gdi,
		   rcClose.left,
		   rcClose.top + (HEIGHT(rcClose)-m_gdiCloseIcon.GetHeight())/2,
		   m_gdiCloseIcon.GetWidth(), m_gdiCloseIcon.GetHeight(),
		   m_gdiCloseIcon);

	rc.right	= rcClose.left - INDENT/2;

	// draw the memory as compactly as possible
	TCHAR szMem[STRING_NORMAL] = _T("");
	SIZE sizeText = {0};
	if(sInfo->dwBlocksUsed)
	{
		DWORD dwSize = sInfo->dwBlocksUsed>>10;
		if(1000 < dwSize)
		{
			double db = (double)dwSize/1000;
			m_oStr->Format(szMem, _T("%.1fM"), db);
		}
		else
		{
			m_oStr->Format(szMem, _T("%dK"), dwSize);
		}

		GetTextExtentExPoint(gdi, 
			szMem, 
			m_oStr->GetLength(szMem), 
			0,
			NULL,NULL,
			&sizeText);
		sizeText.cx += INDENT;

		::DrawText(gdi, szMem, rc, DT_RIGHT|DT_VCENTER|DT_END_ELLIPSIS, g_gui->GetFontTitle(), RGB(255,255,255));	

		rc.right -= sizeText.cx;
	}	

	::DrawText(gdi, sInfo->szWindowTitle, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, g_gui->GetFontTitle(), RGB(255,255,255));
}

HRESULT CGuiFavorites::AddProgram()
{
	HRESULT hr = S_OK;

    CDlgChoosePrograms dlgAdd;
#ifndef NOPHONE
	CDlgChooseContact::ResetListContent();
#endif
	//CDlgChooseSettings::ResetListContent();
	m_dwTickSaveContent = GetTickCount();

    CBARG(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND, _T(""));

    
    dlgAdd.SetPreventDeleteList(TRUE);
    dlgAdd.AddTodayIcon(TRUE);

    if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
    {
        TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
        CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];"));

        hr = AddItem(dlgAdd.GetFriendlyName(),
                     dlgAdd.GetPath(),
                     -1,
                     FAV_Program,
                     sPage);
        CHR(hr, _T("AddItem"));

        // set the grid item
        sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y] = sPage->arrFavs.GetSize()-1;

        // save to the registry
        SaveRegistryPage(m_oPicControl.GetSelectedItemIndex());

        UpdateImageLoader();

        EnsurePages();
    }

Error:
    m_ptSelectedItem.x  = UND;
    m_ptSelectedItem.y  = UND;

    // did an error occur?
    if(hr != S_OK)
        MessageBeep(MB_ICONHAND);

    return hr;
}

HRESULT CGuiFavorites::AddContact()
{
	HRESULT hr = S_OK;
#ifndef NOPHONE
	BOOL bRet;
	TCHAR szName[STRING_MAX];
	IContact* pContact = NULL;

	// delete other two content
	CDlgChoosePrograms::ResetListContent();
	CDlgChooseSettings::ResetListContent();
	m_dwTickSaveContent = GetTickCount();

	CDlgChooseContact dlgAdd(m_hWndParent, NULL, TRUE);

	CBARG(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND, _T(""));

	dlgAdd.SetPreventDeleteList(TRUE);

	if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
	{
		TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
		CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];"));

		LONG lOID;
		lOID	= dlgAdd.GetSelectedOID();
		CBARG(lOID != UND, _T(""));

		CPoomContacts* oPoom = CPoomContacts::Instance();
		CPHR(oPoom, _T("CPoomContacts* oPoom = CPoomContacts::Instance();"));

		pContact	= oPoom->GetContactFromOID(lOID);
		CPHR(pContact, _T("pContact	= oPoom->GetContactFromOID(lOID);"));	

		bRet = oPoom->GetFormatedNameText(szName, pContact, FALSE);
		CBHR(bRet, _T("bRet = oPoom->GetFormatedNameText(szName, pContact, FALSE);"));

		hr = AddItem(szName,
			NULL,
			(int)lOID,
			FAV_Contact,
			sPage);
		CHR(hr, _T("AddItem"));

		// set the grid item
		sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y] = sPage->arrFavs.GetSize()-1;

		// save to the registry
		SaveRegistryPage(m_oPicControl.GetSelectedItemIndex());

		UpdateImageLoader();

		EnsurePages();
	}

Error:
	m_ptSelectedItem.x  = UND;
	m_ptSelectedItem.y  = UND;

	if(pContact)
		pContact->Release();

	// did an error occur?
	if(hr != S_OK)
		MessageBeep(MB_ICONHAND);
#endif
	return hr;
}

HRESULT CGuiFavorites::AddSetting()
{
    HRESULT hr = S_OK;
    CDlgChooseSettings dlgAdd;

#ifndef NOPHONE
	CDlgChooseContact::ResetListContent();
#endif
	//CDlgChoosePrograms::ResetListContent();
	m_dwTickSaveContent = GetTickCount();

    CBARG(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND, _T(""));

    dlgAdd.SetPreventDeleteList(TRUE);

    if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
    {
        TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
        CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];"));

        hr = AddItem(dlgAdd.GetFriendlyName(),
            dlgAdd.GetPath(),
            -1,
            FAV_Setting,
            sPage);
        CHR(hr, _T("AddItem"));

        // set the grid item
        sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y] = sPage->arrFavs.GetSize()-1;

        // save to the registry
        SaveRegistryPage(m_oPicControl.GetSelectedItemIndex());

		UpdateImageLoader();

        EnsurePages();
    }

Error:
    m_ptSelectedItem.x  = UND;
    m_ptSelectedItem.y  = UND;

    // did an error occur?
    if(hr != S_OK)
        MessageBeep(MB_ICONHAND);

    return hr;
}

HRESULT CGuiFavorites::AddFileFolder()
{
    HRESULT hr = S_OK;
    OPENFILENAME sOpenFileName;
    TCHAR szFile[256];
    BOOL bOpen = FALSE;
    CDlgGetFile	dlgGetFile;

#ifndef NOPHONE
	CDlgChooseContact::ResetListContent();
#endif
	CDlgChoosePrograms::ResetListContent();
	CDlgChooseSettings::ResetListContent();
	m_dwTickSaveContent = GetTickCount();

    CBARG(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND, _T(""));    

    m_oStr->Empty(szFile);

    LPCTSTR lpcstrFilter = 
        _T("All Files (*.*)\0*.*\0")
        _T("");

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWndParent; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= NULL;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them
   
	if(g_sOptions->bShowFullScreen)
		SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);\

    //open the file chooser dialog
    bOpen = dlgGetFile.GetOpenFile(&sOpenFileName);
    if(bOpen && m_oStr->GetLength(szFile) > 0)//only save it if they don't cancel
    {
        TCHAR szFriendlyName[STRING_MAX];
        m_oStr->StringCopy(szFriendlyName, szFile);
        DWORD dwFileAttributes = GetFileAttributes(szFile);
        if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            PathStripPath(szFriendlyName);
        }
        else
        {
            PathStripPath(szFriendlyName);
            PathRemoveArgs(szFriendlyName);
            PathRemoveExtension(szFriendlyName);
        }

        TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
        CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];"));

        hr = AddItem(szFriendlyName,
            szFile,
            -1,
            FAV_FileOrFolder,
            sPage);
        CHR(hr, _T("AddItem"));

        // set the grid item
        sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y] = sPage->arrFavs.GetSize()-1;

        // save to the registry
        SaveRegistryPage(m_oPicControl.GetSelectedItemIndex());

        UpdateImageLoader();

        EnsurePages();
    }

Error:
    m_ptSelectedItem.x  = UND;
    m_ptSelectedItem.y  = UND;

    // did an error occur?
    if(hr != S_OK)
        MessageBeep(MB_ICONHAND);

    return hr;


}

void CGuiFavorites::OnTimerChange()
{
	if(GetTickCount() - m_dwTickSaveContent > 60000)	// we save it for one minute
	{
		DBG_OUT((_T("CGuiFavorites::OnTimerChange() - Deleting lists to save memory")));
#ifndef NOPHONE
		CDlgChooseContact::ResetListContent();
#endif
		CDlgChoosePrograms::ResetListContent();
		CDlgChooseSettings::ResetListContent();
		m_dwTickSaveContent = GetTickCount();
	}

	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Store: %d"), m_oPicLoader.GetPicStoreCount()));
	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Queue: %d"), m_oPicLoader.GetPicQueueCount()));
	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Holder: %d"), m_oPicLoader.GetPicHolderCount()));
	//m_oPicLoader.PurgePictures();
	UpdateImageLoader();
	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Store: %d"), m_oPicLoader.GetPicStoreCount()));
	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Queue: %d"), m_oPicLoader.GetPicQueueCount()));
	DBG_OUT((_T("CGuiFavorites::OnTimerChange - Pic Holder: %d"), m_oPicLoader.GetPicHolderCount()));
}

HRESULT	CGuiFavorites::AddItem(TCHAR* szFriendlyName, TCHAR* szPath, int iIconIndex, EnumFavType eType, TypePage* sPage /*= NULL*/)
{
	HRESULT hr = S_OK;
	TypeFavItem* sNewItem = NULL;

	if(sPage == NULL)
		sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];

	CBARG(sPage && !m_oStr->IsEmpty(szFriendlyName), _T(""));

	sNewItem = new TypeFavItem;
	CPHR(sNewItem, _T("sNewItem = new TypeFavItem;"));

	sNewItem->szFriendlyName	= m_oStr->CreateAndCopy(szFriendlyName);
	CPHR(sNewItem->szFriendlyName, _T("sNewItem->szFriendlyName	= m_oStr->CreateAndCopy(szFriendlyName);"));

	if(szPath)
	{
		sNewItem->szPath			= m_oStr->CreateAndCopy(szPath);
		CPHR(sNewItem->szPath, _T("sNewItem->szPath			= m_oStr->CreateAndCopy(szPath);"));
	}

	sNewItem->iIndex			= iIconIndex;
	sNewItem->eType				= eType;

	hr = sPage->arrFavs.AddElement(sNewItem);
	CHR(hr, _T("hr = sPage->arrFavs.AddElement(sNewItem);"));

Error:
	if(hr != S_OK && sNewItem)
		delete sNewItem;

	return hr;
}

HRESULT CGuiFavorites::EnsurePages()
{
	HRESULT hr = S_OK;

	if(m_arrPages.GetSize() == MAX_NUM_Pages)
		goto Error;

	if(m_arrPages.GetSize() == 0)
	{
		// create two default pages
		hr = AddPage();
		CHR(hr, _T("default page 1"));

		hr = AddPage();
		CHR(hr, _T("default page 2"));

	}
	else
	{
		// check the last page and see if it has any values
		TypePage* sPage = m_arrPages[m_arrPages.GetSize()-1];
		CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_arrPages.GetSize()-1];"));

		if(sPage->arrFavs.GetSize() > 0)
		{
			// there are some favorites so let's create a new empty page
			hr = AddPage();
			CHR(hr, _T("hr = AddPage();"));
		}
	}

Error:
	return hr;
}

HRESULT CGuiFavorites::AddPage()
{
	HRESULT hr = S_OK;
	TypePage* sPage = NULL;

	CBARG(m_arrPages.GetSize() < MAX_NUM_Pages, _T(""));

	// create two default pages
	sPage = new TypePage;
	CPHR(sPage, _T("sPage = new TypePage;"));

	hr = m_arrPages.AddElement(sPage);
	CHR(hr, _T("hr = m_arrPages.AddElement(sPage);"));

	hr = m_oPicControl.AddItem(_T("1"), IDMENU_Select, (LPARAM)1, (LPVOID)sPage);
	sPage = NULL;
	CHR(hr, _T("hr = m_oPicControl.AddItem(_T(\"1\"), IDMENU_Select, (LPARAM)1, (LPVOID)sPage);"));

Error:
	if(hr != S_OK && sPage)
		delete sPage;
	return hr;
}

BOOL CGuiFavorites::Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	DrawTitle(gdi, rcClient, rcClip);

	m_oPicControl.OnDraw(gdi, rcClient, rcClip);

	
	return TRUE;
}

void CGuiFavorites::DrawRecycleBin(CIssGDIEx& gdi, RECT& rcClip, BOOL bDrawGlow)
{
	if(!IsRectInRect(m_rcRecycleBin, rcClip))
		return;

	if(m_gdiRecycleBin.GetDC() == NULL)
	{
		m_gdiRecycleBin.LoadImage(IsVGA()?IDR_RecycleBinVGA:IDR_RecycleBin, m_hWndParent, m_hInst);
	}

	RECT rc = m_rcRecycleBin;

	rc.left	= rc.right - m_gdiRecycleBin.GetWidth();
	rc.top	= rc.bottom - m_gdiRecycleBin.GetHeight();
	m_rcRecycleBin = rc;

	if(bDrawGlow)
	{
		g_gui->DrawGlow(gdi, rc);
		::Draw(gdi, rc, m_gdiRecycleBin, 0,0,ALPHA_Normal, 100);
	}
	else
		::Draw(gdi, rc, m_gdiRecycleBin);
}

void CGuiFavorites::DrawTitle(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	if(!IsRectInRect(rcClip, m_rcTitle))
		return;

	if(!m_imgTitle.IsInitialized())
	{
		if(g_gui->IsLandscape())
			m_imgTitle.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_Top_Lan_VGA:IDR_Top_Lan);
		else
			m_imgTitle.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_Top_VGA:IDR_Top);
	}

	if(m_imgTitle.GetWidth() != WIDTH(m_rcTitle) || m_imgTitle.GetHeight() != HEIGHT(m_rcTitle))
		m_imgTitle.SetSize(WIDTH(m_rcTitle), HEIGHT(m_rcTitle));

	// draw the background
	m_imgTitle.DrawImage(gdi, m_rcTitle.left, m_rcTitle.top);

	// draw the running programs
	if(IsRectInRect(m_rcRunningPrograms, rcClip))
	{
		if(!m_gdiRunningPrograms.GetDC())
			m_gdiRunningPrograms.LoadImage(IsVGA()?IDR_RP_VGA:IDR_RP, m_hWndParent, m_hInst, TRUE);
		
		::Draw(gdi, 
			   m_rcRunningPrograms,
			   m_gdiRunningPrograms);
	}

	if(m_gdiDots.GetDC() == NULL)
		m_gdiDots.LoadImage(IsVGA()?IDR_Dots_VGA:IDR_Dots, m_hWndParent, m_hInst);

	int iNum = m_arrPages.GetSize();
	int iSize = m_gdiDots.GetHeight();
	int iIndent = INDENT/2;
	int iStart;
	RECT rcText = m_rcTitle;

    TCHAR* szTitle = GetTitleName();

	if(g_gui->IsLandscape())
	{
		iStart = (HEIGHT(m_rcTitle) - iNum*(iSize+iIndent))/2;

		for(int i=0; i<iNum; i++)
		{
			::Draw(gdi,
				m_rcTitle.left+iIndent, iStart + i*(iSize+iIndent),
				iSize, iSize,
				m_gdiDots,
				i==m_oPicControl.GetSelectedItemIndex()?iSize:0, 0);
		}

		if(szTitle)
		{
			HFONT hFontOld = NULL;

			if(!m_hTitleLan)
				m_hTitleLan = CIssGDIEx::CreateFont(TITLE_TEXT_Height, FW_BOLD, TRUE, _T("Tahoma"), 270);

			hFontOld = (HFONT)SelectObject(gdi.GetDC(), m_hTitleLan);   

			SIZE sTemp;
			GetTextExtentPoint(gdi.GetDC(), szTitle, m_oStr->GetLength(szTitle), &sTemp);

			if(hFontOld)
				SelectObject(gdi.GetDC(), hFontOld); 

			rcText = m_rcTitle;

			rcText.left += (iSize+iIndent);

			rcText.top	= rcText.top + (HEIGHT(rcText) - sTemp.cx)/2;
			rcText.right= rcText.right - (WIDTH(rcText) - sTemp.cy)/2;

            RECT rcTemp;
            GetClientRect(m_hWndParent, &rcTemp);

             //why you ask? because we need room for the width of unrotated text because the MS functions are stupid
            //and we also need to add the height of the un roated text because the point of rotation is the top left
            //doing it this way with DT_TOP|DT_RIGHT effectively places the text in the TOP LEFT when rotated by 270
            rcTemp = rcText;
            rcTemp.left = rcText.left + (IsVGA()?4:2); //fudge factor as it was bit too close to the dots
            rcTemp.right = rcTemp.left + sTemp.cx + sTemp.cy;
            
            		
			DrawTextShadow(gdi, szTitle, rcTemp, DT_TOP | DT_RIGHT, m_hTitleLan, RGB(255,255,255), RGB(0,0,0));

       //     FrameRect(gdi.GetDC(), rcTemp, 0xFFFFFF);

		}
	}
	else
	{
		iStart = (WIDTH(m_rcTitle) - iNum*(iSize+iIndent))/2;

		for(int i=0; i<iNum; i++)
		{
			::Draw(gdi,
				 iStart + i*(iSize+iIndent), m_rcTitle.bottom - (iSize + iIndent),
				 iSize, iSize,
				 m_gdiDots,
				 i==m_oPicControl.GetSelectedItemIndex()?iSize:0, 0);
		}

		rcText.bottom -= (iSize+iIndent);
        if(szTitle)
		    DrawTextShadow(gdi, szTitle, rcText, DT_CENTER|DT_VCENTER, g_gui->GetFontTitle(), RGB(255,255,255), RGB(0,0,0));
	}

}
    
    
BOOL CGuiFavorites::OnChar(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case _T('1'):
	case _T('r'):
	case _T('R'):
		OnRunningPrograms();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CGuiFavorites::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// if nothing valid is currently selected, grab a valid one
	if(!IsValidGrid(m_ptSelectedItem))
	{
		m_ptSelectedItem.x	= 0;
		m_ptSelectedItem.y	= m_iNumVert - 1;
		InvalidateRect(m_hWndParent, &g_gui->RectScreen(), FALSE);
		return TRUE;
	}

	// we have a selection
	switch(wParam)
	{
	case VK_RETURN:
		LaunchSelectedItem();
		return TRUE;
		break;
	case VK_UP:
		if(m_ptSelectedItem.y == 0)
			m_ptSelectedItem.y = m_iNumVert - 1;
		else
			m_ptSelectedItem.y --;
		break;
	case VK_DOWN:
		if(m_ptSelectedItem.y == m_iNumVert - 1)
			m_ptSelectedItem.y = 0;
		else
			m_ptSelectedItem.y ++;
		break;
	case VK_LEFT:
		if(m_ptSelectedItem.x == 0)
			return m_oPicControl.OnKeyDown(wParam, lParam);
		else
			m_ptSelectedItem.x --;
		break;
	case VK_RIGHT:
		if(m_ptSelectedItem.x == m_iNumHorz - 1)
			return m_oPicControl.OnKeyDown(wParam, lParam);
		else
			m_ptSelectedItem.x ++;
		break;
	default:
		return UNHANDLED;
		break;
	}

	InvalidateRect(m_hWndParent, &g_gui->RectScreen(), FALSE);
	return TRUE;
}

BOOL CGuiFavorites::HasFocus()
{
	return TRUE;
}

void CGuiFavorites::LoseFocus()
{
  
}


BOOL CGuiFavorites::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
    

BOOL CGuiFavorites::OnLButtonDown(POINT pt)
{
	m_bLButtonDown = FALSE;
	m_bSelectTitle = FALSE;
	
	if(!PtInRect(&g_gui->RectScreen(), pt))
		return FALSE;

	m_ptSelectedItem.x = UND;
	m_ptSelectedItem.y = UND;
	m_ptDropItem.x = UND;
	m_ptDropItem.y = UND;

	if(PtInRect(&m_rcTitle, pt))
	{
		if(PtInRect(&m_rcRunningPrograms, pt))
			OnRunningPrograms();
		else
			m_bSelectTitle = TRUE;
			
		return TRUE;
	}

	m_bLButtonDown = TRUE;

	if(!PtInRect(&m_rcPicControl, pt))
		return FALSE;

	//DBG_OUT((_T("CGuiFavorites::OnLButtonDown")));

	m_ptMouseDown = m_ptMouseCurrent = m_ptRealMouseCurrent = m_ptLastMouse = pt;
	m_bGrabAndMove = FALSE;

	// if we're in the middle of a move don't do anything
	if(m_oPicControl.GetMovingState() != PICSCROLL_Stopped)
		return m_oPicControl.OnLButtonDown(pt);

	// see if we are selecting an item
	TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
	if(!sPage)
		return FALSE;

	RECT rcItem;
	for(int y = 0; y<m_iNumVert; y++)
	{
		for(int x = 0; x<m_iNumHorz; x++)
		{
			rcItem.left		= m_rcPicControl.left + m_iHorzSpacing + x*(m_iHorzSpacing+m_iIconSize);
			rcItem.right	= rcItem.left + m_iIconSize;
			rcItem.top		= m_rcPicControl.top + m_iVertSpacing + y*(m_iVertSpacing + m_iIconSize);
			rcItem.bottom	= rcItem.top + m_iIconSize;

			if(PtInRect(&rcItem, pt))
			{
				m_ptSelectedItem.x = m_ptDropItem.x = x;
				m_ptSelectedItem.y = m_ptDropItem.y = y;

				// has to be a valid item to move around
				int iIndex = sPage->arrGrid[x][y];
				TypeFavItem* sFav = sPage->arrFavs[iIndex];
				if(sFav)
				{			

					m_ptOffset.x	= pt.x - rcItem.left;
					m_ptOffset.y	= pt.y - rcItem.top;

					// timer to figure out if we're grabbing or moving
					SetTimer(m_hWndParent, IDT_GrabAndMove, 800, NULL);
				}

				InvalidateRect(m_hWndParent, &m_rcPicControl, FALSE);

				return m_oPicControl.OnLButtonDown(pt);
			}
		}
	}


	return m_oPicControl.OnLButtonDown(pt);
}

BOOL CGuiFavorites::OnLButtonUp(POINT pt)
{
	KillTimer(m_hWndParent, IDT_GrabAndMove);

	if(m_bSelectTitle && PtInRect(&m_rcTitle, pt) && !PtInRect(&m_rcRunningPrograms, pt))
	{
		LaunchMenuSelector();
		return TRUE;
	}

	if(!m_bLButtonDown)
		return FALSE;

	m_bLButtonDown = FALSE;

	if(m_bGrabAndMove)
	{
		m_bGrabAndMove = FALSE;

		TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
		if(PtInRect(&m_rcRecycleBin, pt) && IsValidGrid(m_ptSelectedItem))
		{
			// should we delete the item
			RemoveItem(m_oPicControl.GetSelectedItemIndex(), m_ptSelectedItem);

			MessageBeep(MB_OK);	// deleted item

			m_ptSelectedItem.x = UND;
			m_ptSelectedItem.y = UND;

		}
		else if(sPage && 
		   IsValidGrid(m_ptSelectedItem) &&
		   IsValidGrid(m_ptDropItem) && 
		   (m_ptSelectedItem.x != m_ptDropItem.x || m_ptSelectedItem.y != m_ptDropItem.y))	// has to be a different cell
		{
			int iHolding = sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y];
			int iDrop	 = sPage->arrGrid[m_ptDropItem.x][m_ptDropItem.y];

			// swap them
			sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y] = iDrop;
			sPage->arrGrid[m_ptDropItem.x][m_ptDropItem.y] = iHolding;

			// save to the registry
			SaveRegistryPage(m_oPicControl.GetSelectedItemIndex(), TRUE);

			m_ptSelectedItem = m_ptDropItem;
		}
		InvalidateRect(m_hWndParent, &m_rcPicControl, FALSE);

		return TRUE;
	}

	//DBG_OUT((_T("CGuiFavorites::OnLButtonUp")));

	BOOL bRet =  m_oPicControl.OnLButtonUp(pt);

	if(IsValidGrid(m_ptSelectedItem))
		LaunchSelectedItem();	

	return bRet;
}

BOOL CGuiFavorites::OnMouseMove(POINT pt)
{
	if(!m_bLButtonDown)
		return FALSE;

	m_ptLastMouse		= m_ptMouseCurrent;
	m_ptMouseCurrent	= m_ptRealMouseCurrent = pt;

	if(m_bGrabAndMove)
	{
		// make sure the current mouse position stays within the screen RECT
		if(m_ptMouseCurrent.x - m_ptOffset.x < m_rcPicControl.left)
			m_ptMouseCurrent.x = m_rcPicControl.left + m_ptOffset.x;
		if(m_ptMouseCurrent.y - m_ptOffset.y < m_rcPicControl.top)
			m_ptMouseCurrent.y = m_rcPicControl.top + m_ptOffset.y;
		if(m_ptMouseCurrent.x - m_ptOffset.x + m_iIconSize > m_rcPicControl.right)
			m_ptMouseCurrent.x = m_rcPicControl.right + m_ptOffset.x - m_iIconSize;
		if(m_ptMouseCurrent.y - m_ptOffset.y + m_iIconSize > m_rcPicControl.bottom)
			m_ptMouseCurrent.y = m_rcPicControl.bottom + m_ptOffset.y - m_iIconSize;

		
		RECT rcTotal, rcUpdate, rcDrop;
		POINT ptNewDrop;

		rcUpdate.left	= min(m_ptLastMouse.x, m_ptMouseCurrent.x) - m_ptOffset.x;
		rcUpdate.top	= min(m_ptLastMouse.y, m_ptMouseCurrent.y) - m_ptOffset.y;
		rcUpdate.right	= max(m_ptLastMouse.x, m_ptMouseCurrent.x) - m_ptOffset.x + m_iIconSize;
		rcUpdate.bottom	= max(m_ptLastMouse.y, m_ptMouseCurrent.y) - m_ptOffset.y + m_iIconSize;

		ptNewDrop.x	= 0;
		ptNewDrop.y  = 0;

		// find out the square we are in if we were to drop the item right now
		for(int y = m_iNumVert - 1; y >= 0; y--)
		{
			if(m_ptMouseCurrent.y >= (m_rcPicControl.top + m_iVertSpacing + y*(m_iVertSpacing + m_iIconSize)))
			{
				ptNewDrop.y = y;
				break;
			}
		}
		for(int x = m_iNumHorz-1; x >= 0; x--)
		{
			if(m_ptMouseCurrent.x >= (m_rcPicControl.left + m_iHorzSpacing + x*(m_iHorzSpacing + m_iIconSize)))
			{
				ptNewDrop.x = x;
				break;
			}
		}

		// this is so we make sure to redraw over the old drop item
		if(m_ptDropItem.x != UND && m_ptDropItem.y != UND)
		{
			rcDrop.left		= m_rcPicControl.left + m_iHorzSpacing + m_ptDropItem.x*(m_iHorzSpacing + m_iIconSize);
			rcDrop.top		= m_rcPicControl.top + m_iVertSpacing + m_ptDropItem.y*(m_iVertSpacing + m_iIconSize);
			rcDrop.right	= rcDrop.left + m_iIconSize;
			rcDrop.bottom	= rcDrop.top + m_iIconSize;

			UnionRect(&rcUpdate, &rcUpdate, &rcDrop);
		}

		m_ptDropItem = ptNewDrop;

		rcDrop.left		= m_rcPicControl.left + m_iHorzSpacing + m_ptDropItem.x*(m_iHorzSpacing + m_iIconSize);
		rcDrop.top		= m_rcPicControl.top + m_iVertSpacing + m_ptDropItem.y*(m_iVertSpacing + m_iIconSize);
		rcDrop.right	= rcDrop.left + m_iIconSize;
		rcDrop.bottom	= rcDrop.top + m_iIconSize;

		UnionRect(&rcTotal, &rcUpdate, &rcDrop);

		InvalidateRect(m_hWndParent, &rcTotal, FALSE);
		UpdateWindow(m_hWndParent);
		
		return TRUE;
	}
	else if(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND)
	{
		// we are selecting something so let's see we're still supposed to select it or have we moved too much
		int iMoveFactor = MOVE_Factor;

		if(abs(m_ptMouseDown.x-pt.x) > iMoveFactor || abs(m_ptMouseDown.y-pt.y)>iMoveFactor)
		{
			KillTimer(m_hWndParent, IDT_GrabAndMove);

			// reset
			m_ptSelectedItem.x = UND;
			m_ptSelectedItem.y = UND;
		}
	}

	//DBG_OUT((_T("CGuiFavorites::OnMouseMove")));

	return m_oPicControl.OnMouseMove(pt);

}

BOOL CGuiFavorites::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_Select:
		break;
	case IDMENU_AddNewItem:
		OnAddNewItem();
		break;
    case IDMENU_AddProgram:
        AddProgram();
        break;
    case IDMENU_AddSetting:
        AddSetting();
        break;
    case IDMENU_AddFileFolder:
        AddFileFolder();
        break;
	case IDMENU_NewContact:
		AddContact();
		break;
	case IDMENU_SetTitle:
		SetCurrentTitle();
		break;
	case IDMENU_RunningPrograms:
		OnSelectRunningProgram();
		break;
	case IDMENU_CloseAll:
		OnCloseAll();
		break;
	case IDMENU_SoftReset:
		OnSoftReset();
		break;
	case IDMENU_RemoveItem:
		RemoveItem(m_oPicControl.GetSelectedItemIndex(), m_ptSelectedItem);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CGuiFavorites::OnSelectRunningProgram()
{
	TypeItems* sItem = g_wndMenu->GetSelectedItem();
	if(!sItem || !sItem->lpItem)
	{
		MessageBeep(MB_ICONHAND);
		return;
	}

	TypeProcessInfo* sInfo = (TypeProcessInfo*)sItem->lpItem;

	RECT rcDraw = g_wndMenu->GetDrawRect(g_wndMenu->GetSelectedItemIndex());
	POINT ptLastButtonUp = g_wndMenu->GetLastButtonUp();	

	// if we pressed the x icon
	if(PtInRect(&rcDraw, ptLastButtonUp) && ptLastButtonUp.x > rcDraw.right - HEIGHT(rcDraw))
	{
		::SetCursor(LoadCursor(NULL, IDC_WAIT));
		m_objWindows.KillProcess(sInfo, FALSE);
		::SetCursor(NULL);
		// put the menu back up
		OnRunningPrograms();
	}
	else
	{
		ShowWindow(m_hWndParent, SW_MINIMIZE);
		m_objWindows.ActiveProcess(sInfo);
	}
}

void CGuiFavorites::OnCloseAll()
{
	// find em and destroy em
	m_objWindows.KillAllTasks(FALSE);

	// put the menu back up
	OnRunningPrograms();
}

void CGuiFavorites::OnSoftReset()
{
	CDlgMsgBox dlgMsg;

	if(IDYES == dlgMsg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_SoftReset), m_hInst), _T(""), m_hWndParent, m_hInst, MB_YESNO))
	{
		SetSystemPowerState (NULL, POWER_STATE_RESET, 0);
	}
}

BOOL CGuiFavorites::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_PicItemChanged)
	{
		InvalidateRect(m_hWndParent, &m_rcTitle, FALSE);
		return TRUE;
	}
    else if(uiMessage == WM_PicLoader)
    {
        if(m_oPicControl.GetMovingState() == PICSCROLL_Stopped)
            InvalidateRect(m_hWndParent, &g_gui->RectScreen(), FALSE);
    }
	return UNHANDLED;
}

BOOL CGuiFavorites::OnTimer(WPARAM wParam, LPARAM lParam)
{
	//DBG_OUT((_T("CGuiFavorites::OnTimer")));

	if(wParam == IDT_GrabAndMove)
	{
		KillTimer(m_hWndParent, IDT_GrabAndMove);

		if(m_ptSelectedItem.x != UND && m_ptSelectedItem.y != UND)
		{
			// we are selecting something so let's see we're still supposed to select it or have we moved too much
			int iMoveFactor = MOVE_Factor;

			if(abs(m_ptMouseDown.x-m_ptMouseCurrent.x) <= iMoveFactor && abs(m_ptMouseDown.y-m_ptMouseCurrent.y) <= iMoveFactor)
			{
				m_bGrabAndMove = TRUE;

				m_rcRecycleBin.right		= m_rcPicControl.right;
				m_rcRecycleBin.left			= m_rcPicControl.right - m_iIconSize/2;
				//m_rcRecycleBin.left			= m_rcPicControl.left;
				m_rcRecycleBin.bottom		= m_rcPicControl.bottom;
				//m_rcRecycleBin.right		= m_rcPicControl.left + m_iIconSize/2;
				m_rcRecycleBin.top			= m_rcPicControl.bottom - m_iIconSize/2;

				InvalidateRect(m_hWndParent, &m_rcPicControl, FALSE);

				// lose focus of the picture control
				m_oPicControl.OnLButtonUp(m_ptMouseCurrent);
			}
		}


		return TRUE;
	}
	return m_oPicControl.OnTimer(wParam, lParam);
}
  




void CGuiFavorites::SetIconSize()
{
	if(g_gui->IsLandscape())
	{
		m_iVertSpacing = GetSystemMetrics(SM_CXICON)/8;
		m_iHorzSpacing = IsVGA()?2:1;//minimum Y spacing

		m_iNumVert = (int)g_sOptions->eFavSize;

		m_iIconSize = (HEIGHT(m_rcPicControl) - m_iVertSpacing)/m_iNumVert; 
		m_iIconSize -= m_iVertSpacing;

		//now recalc the indent
		m_iVertSpacing = (HEIGHT(m_rcPicControl) - m_iNumVert*m_iIconSize)/(m_iNumVert + 1);

		//now calc the number of columns
		m_iNumHorz = (WIDTH(m_rcPicControl) - m_iHorzSpacing)/(m_iIconSize + m_iHorzSpacing);
		//and recalc the Y spacing
		m_iHorzSpacing = (WIDTH(m_rcPicControl) - m_iNumHorz*m_iIconSize)/(m_iNumHorz + 1);
	}
	else
	{
		m_iHorzSpacing = GetSystemMetrics(SM_CXICON)/8;
		m_iVertSpacing = IsVGA()?2:1;//minimum Y spacing

		m_iNumHorz = (int)g_sOptions->eFavSize;

		m_iIconSize = (WIDTH(m_rcPicControl) - m_iHorzSpacing)/m_iNumHorz; 
		m_iIconSize -= m_iHorzSpacing;

		//now recalc the indent
		m_iHorzSpacing = (WIDTH(m_rcPicControl) - m_iNumHorz*m_iIconSize)/(m_iNumHorz + 1);

		//now calc the number of rows and the y spacing
		m_iNumVert = (HEIGHT(m_rcPicControl) - m_iVertSpacing)/(m_iIconSize + m_iVertSpacing);
		//and recalc the Y spacing
		m_iVertSpacing = (HEIGHT(m_rcPicControl) - m_iNumVert*m_iIconSize)/(m_iNumVert + 1);
	}

	// set the rects for the items, makes it easier for testing points
	TypePage* sPage;
	TypeFavItem* sItem;
	int iIndex;
	BOOL bClearImages = FALSE;
	for(int iPages=0; iPages<m_arrPages.GetSize(); iPages++)
	{
		sPage = m_arrPages[iPages];
		if(!sPage)
			continue;

		for(int y=0; y<m_iNumVert; y++)
		{
			for(int x=0; x<m_iNumHorz; x++)
			{
				iIndex = sPage->arrGrid[x][y];
				if(iIndex == GRID_Undefined)
					continue;

				sItem = sPage->arrFavs[iIndex];
				if(!sItem)
					continue;

				sItem->rcLoc.left	= m_rcPicControl.left + m_iHorzSpacing + x*(m_iHorzSpacing+m_iIconSize);
				sItem->rcLoc.right	= sItem->rcLoc.left + m_iIconSize + m_iHorzSpacing;
				sItem->rcLoc.top	= m_rcPicControl.top + m_iVertSpacing + y*(m_iVertSpacing + m_iIconSize);
				sItem->rcLoc.bottom	= sItem->rcLoc.top + m_iIconSize + m_iVertSpacing;

				// see if we have to redraw all the images
				if(!bClearImages && sItem->sPic && sItem->sPic->gdiImage && sItem->sPic->gdiImage->GetDC())
				{
					if(sItem->sPic->gdiImage->GetWidth() != m_iIconSize || sItem->sPic->gdiImage->GetHeight() != m_iIconSize)
						bClearImages = TRUE;	// we have to clear everything out
				}
			}
		}
	}

	CIssGDIEx::DeleteFont(m_hTitleLan);

	DBG_OUT((_T("CGuiFavorites::SetIconSize()")));
	if(bClearImages)
		ClearImageLoader();

	EnterCriticalSection(&m_crImages);
	m_gdiBox.Destroy();
	m_gdiShine.Destroy();
	LeaveCriticalSection(&m_crImages);

	m_gdiSelector.Destroy();
	m_gdiPlus.Destroy();	// size could change
	m_imgTitle.Destroy();

	UpdateImageLoader(); 
}

void CGuiFavorites::DeleteMyPicListItem(LPVOID lpItem)
{

}

void CGuiFavorites::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
	//FrameRect(gdi, rcDraw, 0);
	CGuiFavorites* oGui = (CGuiFavorites*)lpClass;
	oGui->DrawMyListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

void CGuiFavorites::DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted)
{
	RECT rcTemp, rcTemp2, rcOffset;
	POINT ptCentered;

	CIssGDIEx* gdiPlus = GetPlus();
	CIssGDIEx* gdiBox = GetBox();
	CIssGDIEx* gdiShine = GetShine();
	CIssGDIEx* gdiSelector = GetSelector();
    TypePage* sPage = (TypePage*)sItem->lpItem;
    TypeFavItem* sFavItem;

    if(!gdiPlus || !sPage || !gdiBox || !gdiShine || !gdiSelector)
        return;

	ptCentered.x	= (m_iIconSize - gdiPlus->GetWidth())/2;
	ptCentered.y	= (m_iIconSize - gdiPlus->GetHeight())/2;

	for(int x = 0; x < m_iNumHorz; x++)
	{
		rcTemp.left		= rcDraw.left + m_iHorzSpacing + x*(m_iHorzSpacing+m_iIconSize);
		rcTemp.right	= rcTemp.left + m_iIconSize;
		for(int y = 0; y < m_iNumVert; y++)
		{
			rcTemp.top	= rcDraw.top  + m_iVertSpacing + y*(m_iVertSpacing+m_iIconSize);
			rcTemp.bottom = rcTemp.top + m_iIconSize;

            if(!IsRectInRect(m_oPicControl.GetClipRect(), rcTemp) || !IsRectInRect(m_rcPicControl, rcTemp))
                continue;

			sFavItem = sPage->arrFavs[sPage->arrGrid[x][y]];

			// if we're grabing an item and moving it around this will be the cell that it will be dropped in, so put a glow on it
			if(m_bGrabAndMove && m_ptDropItem.x == x && m_ptDropItem.y == y)
			{
				if(!PtInRect(&m_rcRecycleBin, m_ptRealMouseCurrent))	// can't be in the recycle bin range
					g_gui->DrawGlow(gdi, rcTemp);
			}

			// if this is the item we are moving
			if(m_bGrabAndMove && m_ptSelectedItem.x == x && m_ptSelectedItem.y == y)
				continue;	

            if(!sFavItem)
            {
                rcTemp2 = rcTemp;
                rcTemp2.left += ptCentered.x;
                rcTemp2.top  += ptCentered.y;
                rcTemp2.right    = rcTemp2.left + gdiPlus->GetWidth();
                rcTemp2.bottom   = rcTemp2.top + gdiPlus->GetHeight();
                IntersectRect(&rcOffset, &m_rcPicControl, &rcTemp2);

				// do we need to draw the glow first?
				if(m_ptSelectedItem.x == x && m_ptSelectedItem.y == y)
					g_gui->DrawGlow(gdi, rcTemp);

			    ::Draw(gdi, rcOffset, *gdiPlus, rcOffset.left - rcTemp2.left, rcOffset.top - rcTemp2.top);

				
            }
            else
            {
                IntersectRect(&rcOffset, &m_rcPicControl, &rcTemp);			

				DrawItem(gdi, rcOffset, sFavItem, gdiBox, gdiShine, rcTemp, g_sOptions->bAlwaysShowText);

				// should we draw the glow?
				if(m_ptSelectedItem.x == x && m_ptSelectedItem.y == y)
				{
					IntersectRect(&rcOffset, &m_rcPicControl, &rcTemp);
					::Draw(gdi, rcOffset, *gdiSelector, rcOffset.left - rcTemp.left, rcOffset.top - rcTemp.top);
				}
            }
			
		}
	}

	// if this is the item we are moving
	if(m_bGrabAndMove && IsValidGrid(m_ptSelectedItem))
	{
		// we have to make sure we are only drawing the current page
		TypePage* sCurrent = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
		if(sCurrent == sPage)
		{
			// draw the recycle bin if necessary
			DrawRecycleBin(gdi, m_oPicControl.GetClipRect(), PtInRect(&m_rcRecycleBin, m_ptRealMouseCurrent));
			
			rcTemp.left		= m_ptMouseCurrent.x - m_ptOffset.x;
			rcTemp.top		= m_ptMouseCurrent.y - m_ptOffset.y;
			rcTemp.right	= rcTemp.left + m_iIconSize;
			rcTemp.bottom	= rcTemp.top + m_iIconSize;

			sFavItem = sPage->arrFavs[sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y]];
			if(sFavItem)
			{
				DrawItem(gdi, rcTemp, sFavItem, gdiBox, gdiShine, rcTemp, g_sOptions->bAlwaysShowText, TRUE);
				::Draw(gdi, rcTemp, *gdiSelector, 0, 0, ALPHA_Normal, 125);
			}
		}
	}


}

void CGuiFavorites::DrawItem(CIssGDIEx& gdi, RECT rcLocation, TypeFavItem* sFav, CIssGDIEx* gdiBox, CIssGDIEx* gdiShine, RECT rcTemp, BOOL bDrawTextAlways, BOOL bDrawMoving)
{
	if(!gdiBox || !sFav || !gdiShine)
		return;

	if(sFav->sPic && sFav->sPic->gdiImage)
	{
		::Draw(gdi, rcLocation, *sFav->sPic->gdiImage, rcLocation.left - rcTemp.left, rcLocation.top - rcTemp.top, ALPHA_Normal, bDrawMoving?125:255);
	}
	else
	{
		EnterCriticalSection(&m_crImages);
		::Draw(gdi, rcLocation, *gdiBox, rcLocation.left - rcTemp.left, rcLocation.top - rcTemp.top, ALPHA_Normal, bDrawMoving?125:255);
		::Draw(gdi, rcLocation, *gdiShine, rcLocation.left - rcTemp.left, rcLocation.top - rcTemp.top, ALPHA_Normal, bDrawMoving?125:255);
		LeaveCriticalSection(&m_crImages);
	}

	// draw the text on the item
	if(bDrawTextAlways || !(sFav->sPic && sFav->sPic->gdiImage) || sFav->bDrawName)
	{
		RECT rc = rcTemp;

		DrawText(gdi, sFav->szFriendlyName, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, g_gui->GetFontText());

		rcTemp.top = rcTemp.bottom - HEIGHT(rc) - INDENT;
		DrawTextShadow(gdi, sFav->szFriendlyName, rcTemp, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK, g_gui->GetFontText(), RGB(255,255,255), RGB(50,50,50));
	}
}

CIssGDIEx* CGuiFavorites::GetBox()
{
	if(m_gdiBox.GetDC() == NULL)
	{
		EnterCriticalSection(&m_crImages);
		
		SIZE sz = {m_iIconSize, m_iIconSize};
		
		CIssGDIEx gdiTemp;
		gdiTemp.LoadImage(IDR_Box_VGA, m_hWndParent, m_hInst);

		::ScaleImage(gdiTemp, m_gdiBox, sz, FALSE, 0);

		LeaveCriticalSection(&m_crImages);
	}
	return &m_gdiBox;
}

CIssGDIEx* CGuiFavorites::GetShine()
{
	if(m_gdiShine.GetDC() == NULL)
	{
		EnterCriticalSection(&m_crImages);

		SIZE sz = {m_iIconSize, m_iIconSize};

		CIssGDIEx gdiTemp;
		gdiTemp.LoadImage(IDR_Box_Shine_VGA, m_hWndParent, m_hInst);

		::ScaleImage(gdiTemp, m_gdiShine, sz, FALSE, 0);

		LeaveCriticalSection(&m_crImages);
	}
	return &m_gdiShine;
}

CIssGDIEx* CGuiFavorites::GetPlus()
{
	if(m_gdiPlus.GetDC() == NULL)
	{

		SIZE sz = {m_iIconSize/2, m_iIconSize/2};

		CIssGDIEx gdiTemp;
		gdiTemp.LoadImage(IDR_Add_VGA, m_hWndParent, m_hInst);

		::ScaleImage(gdiTemp, m_gdiPlus, sz, FALSE, 0);
	}
	return &m_gdiPlus;
}

CIssGDIEx* CGuiFavorites::GetSelector()
{
	if(m_gdiSelector.GetDC() == NULL)
	{

		SIZE sz = {m_iIconSize, m_iIconSize};

		CIssGDIEx gdiTemp;
		gdiTemp.LoadImage(g_gui->GetSkin().uiSelectorGlass, m_hWndParent, m_hInst);

		::ScaleImage(gdiTemp, m_gdiSelector, sz, FALSE, 0);
	}
	return &m_gdiSelector;
}

void CGuiFavorites::ClearImageLoader()
{
	DBG_OUT((_T("CGuiFavorites::ClearImageLoader()")));
    ::SetCursor(LoadCursor(NULL, IDC_WAIT));

	for(int i=0; i<m_arrPages.GetSize(); i++)
	{
		TypePage* sPage = m_arrPages[i];
		if(!sPage)
			continue;

		for(int j=0; j<sPage->arrFavs.GetSize(); j++)
		{
			TypeFavItem* sFav = sPage->arrFavs[j];
			if(!sFav)
				continue;

			if(sFav->sPic)
			{
				sFav->sPic->bIsDeleted = TRUE;
				sFav->sPic = NULL;  // don't keep it if we don't use it
			}
		}
	}

    m_oPicLoader.ClearAllAsyncItems();
    m_oPicLoader.WaitForQueueToFinish(m_hWndParent);
    m_oPicLoader.ClearQueue();
	m_oPicLoader.PurgePictures();

    SetCursor(NULL);
}

void CGuiFavorites::ClearImageLoaderQueue()
{
    ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_oPicLoader.ClearAllAsyncItems();
    m_oPicLoader.WaitForQueueToFinish(m_hWndParent);
	m_oPicLoader.PurgePictures();

    SetCursor(NULL);
}

void CGuiFavorites::UpdateImageLoader()
{
	DBG_OUT((_T("CGuiFavorites::UpdateImageLoader()")));

    ClearImageLoaderQueue();

    for(int i=0; i<m_arrPages.GetSize(); i++)
    {
        TypePage* sPage = m_arrPages[i];
        if(!sPage)
            continue;

        for(int j=0; j<sPage->arrFavs.GetSize(); j++)
        {
            TypeFavItem* sFav = sPage->arrFavs[j];
            if(!sFav || (sFav->sPic && sFav->sPic->gdiImage && sFav->sPic->gdiImage->GetDC()))
                continue;

            if(sFav->sPic)
            {
                sFav->sPic->bIsDeleted = TRUE;
                sFav->sPic = NULL;  // don't keep it if we don't use it
            }

            sFav->sPic = m_oPicLoader.AddPicToQueue((PFN_LOADPIC)PicLoader, (LPVOID)this, (LPARAM)sFav, m_hWndParent, WM_PicLoader);

        }
    }

	m_oPicLoader.PurgePictures();
}

HRESULT CGuiFavorites::PicLoader(CIssGDIEx& gdi, TypePLItem* sItem, LPVOID lpclass, LPARAM lpItem)
{
    CGuiFavorites* pClass = (CGuiFavorites*)lpclass;
    if(!pClass)
        return E_FAIL;
    return pClass->MyPicLoader(gdi, sItem, lpItem);
}

HRESULT CGuiFavorites::MyPicLoader(CIssGDIEx& gdi, TypePLItem* sItem, LPARAM lpItem)
{
    HRESULT hr = S_OK;
    RECT rc;
	CIssGDIEx* gdiPic = NULL;

	CBARG(lpItem, _T(""));

    SetRect(&rc, 0,0,m_iIconSize, m_iIconSize);

	EnterCriticalSection(&m_crImages);
	CIssGDIEx* gdiMem = g_gui->GetGDI();
	if(gdiMem && gdiMem->GetDC())
		hr = gdi.Create(gdiMem->GetDC(), rc, FALSE, TRUE);
	else
		hr = E_FAIL;
	LeaveCriticalSection(&m_crImages);
    CHR(hr, _T("hr = gdi.Create(g_gui->GetGDI()->GetDC(), m_iIconSize, m_iIconSize, FALSE, TRUE);"));

	hr = gdi.InitAlpha(TRUE);
	CHR(hr, _T("hr = gdi.InitAlpha(TRUE);"));

    CIssGDIEx* gdiBox = GetBox();
    CPHR(gdiBox, _T("CIssGDIEx* gdiBox = GetBox();"));

	CIssGDIEx* gdiShine = GetShine();
	CPHR(gdiShine, _T("CIssGDIEx* gdiShine = GetShine();"));

	TypeFavItem* sFav = (TypeFavItem*)lpItem;

	if(g_sOptions->bDrawTiles || sFav->eType == FAV_Contact)
	{
		EnterCriticalSection(&m_crImages);
		::Draw(gdi, rc, *gdiBox, 0, 0, ALPHA_Copy);
		LeaveCriticalSection(&m_crImages);
	}	

	int iLength = m_iIconSize - INDENT;

	if(sFav->eType == FAV_Program)
	{
		if(0 == m_oStr->Compare(m_oStr->GetText(IDS_Today), sFav->szPath))
		{
			CIssImageSliced imgMask;

			hr = imgMask.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_DETAILS_Mask_VGA:IDR_DETAILS_Mask);
			CHR(hr, _T("hr = imgMask.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_DETAILS_Mask_VGA:IDR_DETAILS_Mask);"));

			hr = imgMask.SetSize(iLength, iLength);
			CHR(hr, _T("hr = imgMask.SetSize(m_iIconSize, m_iIconSize);"));

			CIssGDIEx gdiToday;
			hr = gdiToday.LoadImage(IsVGA()?IDR_PNG_TodayVGA:IDR_PNG_Today, m_hWndParent, m_hInst, TRUE, iLength, iLength, ASPECT_None);
			CHR(hr, _T("hr = gdiToday.LoadImage(IsVGA()?IDR_PNG_TodayVGA:IDR_PNG_Today, m_hWndParent, m_hInst, TRUE, iLength, iLength);"));

			hr = gdiToday.SetAlphaMask(imgMask.GetImage());
			CHR(hr, _T("hr = gdiFinalPic->SetAlphaMask(imgMask.GetImage());"));

			::Draw(gdi, 
				(m_iIconSize - gdiToday.GetWidth())/2, (m_iIconSize - gdiToday.GetHeight())/2, 
				iLength, iLength, 
				gdiToday, 
				0, 0,
				ALPHA_AddValue);			
		}
		else
		{
			CIssGDIEx gdiPic;
			
			if(S_OK != LoadNewerStyleIcon(gdiPic, sFav->szPath, FALSE))
			{
				SHFILEINFO sfi = {0};
				HIMAGELIST hImg = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

				ZeroMemory(&sfi, sizeof(SHFILEINFO));
				int iIndex = 0;
				// get the icon index
				if (SHGetFileInfo(sFav->szPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
					iIndex = sfi.iIcon;				

				hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);
				CHR(hr, _T("hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);"));

				// draw the icon centered 
				ImageList_Draw( hImg, 
					iIndex, gdiPic, 
					0, 
					0,
					ILD_TRANSPARENT);

				gdiPic.InitAlpha(TRUE);

				// we have to figure out the proper alpha values
				COLORREF cr;
				for(int x=0; x<gdiPic.GetWidth(); x++)
				{
					for(int y=0; y<gdiPic.GetHeight(); y++)
					{
						cr = gdiPic.GetPixelColor(x,y);
						if(!(GetRValue(cr) > 240 && GetGValue(cr) < 10 && GetBValue(cr) > 240))
							gdiPic.SetAlphaValue(x,y, 255);
						else
							gdiPic.SetPixelColor(x,y, 0);
					}
				}

				if(!g_sOptions->bDrawTiles)
				{
					hr = gdiPic.ScaleImage(iLength, iLength);
					CHR(hr, _T("hr = gdiPic.ScaleImage(m_iIconSize, m_iIconSize);"));
				}
			}

			::Draw(gdi,
				   (m_iIconSize - gdiPic.GetWidth())/2, (m_iIconSize - gdiPic.GetHeight())/2,
				   gdiPic.GetWidth(), gdiPic.GetHeight(),
				   gdiPic,
				   0,0,
				   ALPHA_AddValue);
		}
	}
	else if(sFav->eType == FAV_FileOrFolder)
	{
		SHFILEINFO sfi = {0};
		HIMAGELIST hImg = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

		ZeroMemory(&sfi, sizeof(SHFILEINFO));
		int iIndex = 0;
		// get the icon index
		if (SHGetFileInfo(sFav->szPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
			iIndex = sfi.iIcon;

		CIssGDIEx gdiPic;

		hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);
		CHR(hr, _T("hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);"));

		// draw the icon centered 
		ImageList_Draw( hImg, 
			iIndex, gdiPic, 
			0, 
			0,
			ILD_TRANSPARENT);

		gdiPic.InitAlpha(TRUE);

		// we have to figure out the proper alpha values
		COLORREF cr;
		for(int x=0; x<gdiPic.GetWidth(); x++)
		{
			for(int y=0; y<gdiPic.GetHeight(); y++)
			{
				cr = gdiPic.GetPixelColor(x,y);
				if(!(GetRValue(cr) > 240 && GetGValue(cr) < 10 && GetBValue(cr) > 240))
					gdiPic.SetAlphaValue(x,y, 255);
				else
					gdiPic.SetPixelColor(x,y, 0);
			}
		}

		if(!g_sOptions->bDrawTiles)
		{
			hr = gdiPic.ScaleImage(iLength, iLength);
			CHR(hr, _T("hr = gdiPic.ScaleImage(m_iIconSize, m_iIconSize);"));
		}

		

		::Draw(gdi,
			(m_iIconSize - gdiPic.GetWidth())/2, (m_iIconSize - gdiPic.GetHeight())/2,
			gdiPic.GetWidth(), gdiPic.GetHeight(),
			gdiPic,
			0,0,
			ALPHA_AddValue);

		sFav->bDrawName = TRUE;

	}
	else if(sFav->eType == FAV_Setting)
	{
		CIssGDIEx gdiPic;
		if(S_OK == LoadNewerStyleIcon(gdiPic, sFav->szPath, TRUE))
		{
			::Draw(gdi,
				(m_iIconSize - gdiPic.GetWidth())/2, (m_iIconSize - gdiPic.GetHeight())/2,
				gdiPic.GetWidth(), gdiPic.GetHeight(),
				gdiPic,
				0,0,
				ALPHA_AddValue);
		}
		else
		{
			TCHAR szPath[STRING_MAX];
			TCHAR szItem[STRING_LARGE];
			ZeroMemory(szPath, sizeof(TCHAR)*STRING_MAX);
			ZeroMemory(szItem, sizeof(TCHAR)*STRING_LARGE);

			int iIndex = m_oStr->Find(sFav->szPath, _T(","));
			m_oStr->StringCopy(szPath, sFav->szPath, 0, iIndex);
			m_oStr->StringCopy(szItem, sFav->szPath, iIndex+1, m_oStr->GetLength(sFav->szPath)-iIndex-1);
			iIndex = m_oStr->StringToInt(szItem);

			//Got a control panel applet.
			HINSTANCE hCPL = LoadLibrary(szPath);
			if(hCPL)
			{
				CPLAPPLET pCPLApplet = (CPLAPPLET)GetProcAddress(hCPL, _T("CPlApplet"));
				if(pCPLApplet)
				{
					pCPLApplet(m_hWndParent, CPL_INIT, 0, 0);
					//Find the name of this CPL applet.
					NEWCPLINFO CplInfo = {0};
					CplInfo.dwSize = sizeof(NEWCPLINFO);

					if(pCPLApplet(m_hWndParent, CPL_NEWINQUIRE, iIndex, (LPARAM)&CplInfo) == 0)
					{
						CIssGDIEx gdiPic;

						hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);
						//CHR(hr, _T("hr = gdiPic.Create(gdi, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), FALSE, TRUE, TRUE);"));

						// draw the icon centered 
						DrawIcon(gdiPic,
							0, 
							0,
							CplInfo.hIcon);

						gdiPic.InitAlpha(TRUE);

						// we have to figure out the proper alpha values
						COLORREF cr;
						for(int x=0; x<gdiPic.GetWidth(); x++)
						{
							for(int y=0; y<gdiPic.GetHeight(); y++)
							{
								cr = gdiPic.GetPixelColor(x,y);
								if(!(GetRValue(cr) > 240 && GetGValue(cr) < 10 && GetBValue(cr) > 240))
									gdiPic.SetAlphaValue(x,y, 255);
								else
									gdiPic.SetPixelColor(x,y, 0);
								
							}
						}

						if(!g_sOptions->bDrawTiles)
						{
							hr = gdiPic.ScaleImage(iLength, iLength);
							CHR(hr, _T("hr = gdiPic.ScaleImage(m_iIconSize, m_iIconSize);"));
						}

						::Draw(gdi,
							(m_iIconSize - gdiPic.GetWidth())/2, (m_iIconSize - gdiPic.GetHeight())/2,
							gdiPic.GetWidth(), gdiPic.GetHeight(),
							gdiPic,
							0,0,
							ALPHA_AddValue);
					}
					pCPLApplet(m_hWndParent, CPL_EXIT, 0, 0);
				}
				FreeLibrary(hCPL);
			}
		}		
	}
	else if(sFav->eType == FAV_Contact)
	{
		CPoomContacts* oPoom = CPoomContacts::Instance();
		CPHR(oPoom, _T("CPoomContacts* oPoom = CPoomContacts::Instance();"));

		CIssImageSliced imgMask;

		hr = imgMask.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_DETAILS_Mask_VGA:IDR_DETAILS_Mask);
		CHR(hr, _T("hr = imgMask.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_DETAILS_Mask_VGA:IDR_DETAILS_Mask);"));

		hr = imgMask.SetSize(iLength, iLength);
		CHR(hr, _T("hr = imgMask.SetSize(m_iIconSize, m_iIconSize);"));

		EnterCriticalSection(&m_crImages);
		gdiPic = oPoom->GetUserPicture((LONG)sFav->iIndex, iLength, iLength, RGB(0,0,0), TRUE);
		LeaveCriticalSection(&m_crImages);

		if(!gdiPic)
		{
            // default picture
			CIssGDIEx gdiDefault;

			hr = gdiDefault.LoadImage(IsVGA()?IDR_DETAILS_Default_VGA:IDR_DETAILS_Default, m_hWndParent, m_hInst, TRUE, iLength, iLength, ASPECT_None);
			CHR(hr, _T("hr = gdiDefault.LoadImage(IsVGA()?IDR_DETAILS_Default_VGA:IDR_DETAILS_Default, m_hWndParent, m_hInst, TRUE, iLength, iLength, ASPECT_None);"));

			hr = gdiDefault.SetAlphaMask(imgMask.GetImage());
			CHR(hr, _T("hr = gdiDefault.SetAlphaMask(imgMask.GetImage());"));

			::Draw(gdi, 
				(m_iIconSize - gdiDefault.GetWidth())/2, (m_iIconSize - gdiDefault.GetHeight())/2, 
				iLength, iLength, 
				gdiDefault, 
				0, 0,
				ALPHA_AddValue);

			sFav->bDrawName = TRUE;
		}
		else
		{
			CIssGDIEx gdiFinalPic;
			
			hr = gdiFinalPic.Create(gdi, iLength, iLength, FALSE, TRUE);
			CHR(hr, _T("hr = gdiFinalPic.Create(gdi, m_iIconSize, m_iIconSize, FALSE, TRUE);"));

			::Draw(gdiFinalPic, 
					0, 0, 
					iLength, iLength, 
					*gdiPic, 
					abs(gdiPic->GetWidth()-iLength)/2, abs(gdiPic->GetHeight()-iLength)/2,
					ALPHA_Normal);

			hr = gdiFinalPic.SetAlphaMask(imgMask.GetImage());
			CHR(hr, _T("hr = gdiFinalPic->SetAlphaMask(imgMask.GetImage());"));

			::Draw(gdi, 
				(m_iIconSize - gdiFinalPic.GetWidth())/2, (m_iIconSize - gdiFinalPic.GetHeight())/2, 
				iLength, iLength, 
				gdiFinalPic, 
				0, 0,
				ALPHA_AddValue);
		}
	}


	if(g_sOptions->bDrawTiles || sFav->eType == FAV_Contact)
	{
		EnterCriticalSection(&m_crImages);
		::Draw(gdi, rc, *gdiShine, 0, 0);
		LeaveCriticalSection(&m_crImages);
	}

Error:
	if(gdiPic)
		delete gdiPic;

    return hr;
}

HRESULT CGuiFavorites::LoadNewerStyleIcon(CIssGDIEx& gdiDest, TCHAR* szSourceLink, BOOL bIsSetting)
{
	HRESULT hr = S_OK;
	TCHAR szKey[STRING_MAX] = _T("");
	TCHAR szTemp[STRING_MAX] = _T("");
	HKEY hKey = NULL;
	HKEY hKeySrc = NULL;
	LRESULT lr;
	CIssGDIEx gdiTemp;
	int iLen = m_oStr->GetLength(szSourceLink);

	// length has to be greater then "\\windows\\"
	CBARG(iLen > 0, _T(""));	

	if(bIsSetting)
	{
		lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Security\\Shell\\StartInfo\\Start\\Settings"), 0L, 0L, &hKeySrc);
		CBARG(lr == ERROR_SUCCESS, _T(""));

		m_oStr->StringCopy(szTemp, szSourceLink);	// copy the setting cpl without the \windows directory

		// take off windows directory
		int iIndex = m_oStr->FindNoCase(szTemp, _T("\\windows\\"));
		if(iIndex == 0)
			m_oStr->Delete(0, 9, szTemp);
		
		lr = RegOpenKeyEx(hKeySrc, szTemp, 0L, 0L, &hKey);
		if(lr != ERROR_SUCCESS)
		{
			m_oStr->StringCopy(szKey, _T("Connections\\"));
			m_oStr->Concatenate(szKey, szTemp);
			lr = RegOpenKeyEx(hKeySrc, szKey, 0L, 0L, &hKey);
			if(lr != ERROR_SUCCESS)
			{
				m_oStr->StringCopy(szKey, _T("Personal\\"));
				m_oStr->Concatenate(szKey, szTemp);
				lr = RegOpenKeyEx(hKeySrc, szKey, 0L, 0L, &hKey);
				if(lr != ERROR_SUCCESS)
				{
					m_oStr->StringCopy(szKey, _T("System\\"));
					m_oStr->Concatenate(szKey, szTemp);
					lr = RegOpenKeyEx(hKeySrc, szKey, 0L, 0L, &hKey);
				}
			}
		}

		CBARG(lr == ERROR_SUCCESS, _T(""));

	}
	else
	{
		lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Security\\Shell\\StartInfo\\Start"), 0L, 0L, &hKeySrc);
		CBARG(lr == ERROR_SUCCESS, _T(""));

		SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROGRAMS, FALSE);
		int iIndex = m_oStr->FindNoCase(szSourceLink, szTemp);
		if(iIndex == -1)
		{
			SHGetSpecialFolderPath(NULL, szTemp, CSIDL_STARTMENU, FALSE);
			iIndex = m_oStr->FindNoCase(szSourceLink, szTemp);
		}

		CBARG(iIndex == 0, _T(""));

		m_oStr->StringCopy(szKey, szSourceLink);
		m_oStr->Delete(0, m_oStr->GetLength(szTemp) + 1, szKey);

		lr = RegOpenKeyEx(hKeySrc, szKey, 0L, 0L, &hKey);
		CBARG(lr == ERROR_SUCCESS, _T(""));
	}


	DWORD dwType	= REG_SZ;
	DWORD dwDataSize= STRING_MAX*sizeof(TCHAR);
	ZeroMemory(szTemp, sizeof(TCHAR)*STRING_MAX);
	lr = RegQueryValueEx(hKey, _T("Icon"), NULL, &dwType, (LPBYTE)szTemp, &dwDataSize);
	CBARG(lr == ERROR_SUCCESS, _T(""));

	hr = gdiTemp.LoadImage(szTemp, m_hWndParent, TRUE);
	CHR(hr, _T("hr = gdiTemp.LoadImage(szTemp, m_hWndParent, TRUE);"));

	SIZE sz;

	if(g_sOptions->bDrawTiles)
	{
		sz.cx	= GetSystemMetrics(SM_CXICON);
		sz.cy	= GetSystemMetrics(SM_CYICON);
	}
	else
	{
		sz.cx = sz.cy = m_iIconSize - INDENT;
	}

	hr = ScaleImage(gdiTemp, gdiDest, sz, TRUE, TRANSPARENT_COLOR);
	CHR(hr, _T("hr = ScaleImage(gdiTemp, gdiDest, sz, TRUE, TRANSPARENT_COLOR);"));

Error:
	if(hKey)
		RegCloseKey(hKey);
	if(hKeySrc)
		RegCloseKey(hKeySrc);
	return hr;
}

BOOL CGuiFavorites::IsValidGrid(POINT& ptItem)
{
	if(ptItem.x == UND || ptItem.y == UND ||
		ptItem.x < 0 || ptItem.x >= MAX_Items || ptItem.x >= m_iNumHorz ||
	   ptItem.y < 0 || ptItem.y >= MAX_Items || ptItem.y >= m_iNumVert)
	   return FALSE;
	else
	   return TRUE;
}

TCHAR* CGuiFavorites::GetTitleName(EnumTitleName eTitle)
{
	if(eTitle == TTL_Count)
	{
		TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
		if(!sPage)
			return NULL;
		eTitle = sPage->eTitle;
	}

    switch(eTitle)
    {
    case TTL_Programs:
        return m_oStr->GetText(ID(IDS_SLIDER_Programs), m_hInst);
        break;
    case TTL_Games:
        return m_oStr->GetText(ID(IDS_SLIDER_Games), m_hInst);
        break;
    case TTL_Apps:
        return m_oStr->GetText(ID(IDS_SLIDER_Apps), m_hInst);
        break;
    case TTL_Settings:
        return m_oStr->GetText(ID(IDS_SLIDER_Settings), m_hInst);
        break;
    case TTL_Favorites:
        return m_oStr->GetText(ID(IDS_SLIDER_Favorites), m_hInst);
        break;
    case TTL_Utilities:
        return m_oStr->GetText(ID(IDS_SLIDER_Utilities), m_hInst);
        break;
    case TTL_Music:
        return m_oStr->GetText(ID(IDS_SLIDER_Music), m_hInst);
        break;
    case TTL_Folders:
        return m_oStr->GetText(ID(IDS_SLIDER_Folders), m_hInst);
        break;
    case TTL_Files:
        return m_oStr->GetText(ID(IDS_SLIDER_Files), m_hInst);
        break;
    case TTL_Videos:
        return m_oStr->GetText(ID(IDS_SLIDER_Videos), m_hInst);
        break;
    case TTL_Pictures:
        return m_oStr->GetText(ID(IDS_SLIDER_Pictures), m_hInst);
        break;
	case TTL_Personal:
		return m_oStr->GetText(ID(IDS_SLIDER_Personal), m_hInst);
		break;
	case TTL_Business:
		return m_oStr->GetText(ID(IDS_SLIDER_Business), m_hInst);
		break;
	case TTL_Contacts:
		return m_oStr->GetText(ID(IDS_SLIDER_Contacts), m_hInst);
		break;
	case TTL_Count:
		return m_oStr->GetText(ID(IDS_SLIDER_SelectTitle), m_hInst);
		break;
    case TTL_Blank:
    default:
        break;
    }
    return NULL;
}

void CGuiFavorites::LaunchMenuSelector()
{
	g_wndMenu->ResetContent();

	TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
	if(!sPage)
		return;

	TCHAR* szTitle;
	int iSelector = 0;
	for(int i=0; i<(int)TTL_Count; i++)
	{
		if((EnumTitleName)i == sPage->eTitle)
			iSelector = i;

		szTitle = GetTitleName((EnumTitleName)i);
		if(szTitle)
			g_wndMenu->AddItem(szTitle, IDMENU_SetTitle, (i==(int)sPage->eTitle?FLAG_Check:NULL), i);
		else
			g_wndMenu->AddItem(ID(IDS_SLIDER_Blank), m_hInst, IDMENU_SetTitle, (i==(int)sPage->eTitle?FLAG_Check:NULL), TTL_Blank);

	}

	g_wndMenu->SetSelectedItemIndex(iSelector, TRUE);

	RECT rcWindow;
	GetWindowRect(m_hWndParent, &rcWindow);


	int iWidth = WIDTH(GetFrame())*5/6;

	int iY = HEIGHT(GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

	g_wndMenu->PopupMenu(m_hWndParent, m_hInst,
		OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_DrawScrollArrows,
		GetFrame().left + (WIDTH(GetFrame()) - iWidth)/2, rcWindow.top,
		iWidth, iY,
		0, 0, 0,0, ADJUST_Bottom);
}

void CGuiFavorites::SetCurrentTitle()
{
	TypeItems* sItem = g_wndMenu->GetSelectedItem();
	if(!sItem)
		return;

	TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
	if(!sPage)
		return;

	sPage->eTitle = (EnumTitleName)sItem->lParam;

	SaveRegistryPage(m_oPicControl.GetSelectedItemIndex(), TRUE);
}

HRESULT CGuiFavorites::RemoveItem(int iPage, POINT ptSelectedItem)
{
	HRESULT hr = S_OK;

	BOOL bRet = IsValidGrid(ptSelectedItem);
	CBHR(bRet, _T("BOOL bRet = IsValidGrid(m_ptSelectedItem);"));

	TypePage* sPage = m_arrPages[iPage];
	CPHR(sPage, _T("TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];"));

	int iIndex = sPage->arrGrid[ptSelectedItem.x][ptSelectedItem.y];
	TypeFavItem* sFav = sPage->arrFavs[iIndex];
	CPHR(sFav, _T("TypeFavItem* sFav = sPage->arrFavs[iIndex];"));

	// delete the item
	delete sFav;
	sPage->arrFavs.RemoveElementAt(iIndex);
	sPage->arrGrid[ptSelectedItem.x][ptSelectedItem.y] = GRID_Undefined;

	// bump the items down one so they still line up
	for(int y=0; y<MAX_Items; y++)
	{
		for(int x=0; x<MAX_Items; x++)
		{
			if(sPage->arrGrid[x][y] == GRID_Undefined)
				continue;

			if(sPage->arrGrid[x][y] >= iIndex)
				sPage->arrGrid[x][y] --;

		}
	}

	SaveRegistryPage(iPage, FALSE);


Error:
	return hr;
}

BOOL CGuiFavorites::HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case PIM_ITEM_CREATED_LOCAL: 
	case PIM_ITEM_CREATED_REMOTE: 
		// don't care
		break;
	case PIM_ITEM_DELETED_LOCAL: 
	case PIM_ITEM_DELETED_REMOTE: 
		PoomItemDeleted(wParam);
		break;
	case PIM_ITEM_CHANGED_LOCAL: 
	case PIM_ITEM_CHANGED_REMOTE: 
		PoomItemChanged(wParam);
		break;
	}
	return TRUE;
}

HRESULT CGuiFavorites::PoomItemChanged(long lOid)
{
	BOOL bFoundItem = FALSE;
	for(int iPage= 0; iPage < m_arrPages.GetSize(); iPage++)
	{
		TypePage* sPage = m_arrPages[iPage];
		if(!sPage)
			continue;

		for(int iFavItem = 0; iFavItem < sPage->arrFavs.GetSize(); iFavItem++)
		{
			TypeFavItem* sFav = sPage->arrFavs[iFavItem];
			if(!sFav || sFav->eType != FAV_Contact || sFav->iIndex != (int)lOid)
				continue;

			// delete the old stuff
			m_oStr->Delete(&sFav->szFriendlyName);

			if(sFav->sPic)
				sFav->sPic->bIsDeleted = TRUE;
			sFav->sPic = NULL;

			CPoomContacts* oPoom = CPoomContacts::Instance();
			IContact* pContact = oPoom->GetContactFromOID(sFav->iIndex);
			TCHAR szName[STRING_MAX] = _T("");
			oPoom->GetFormatedNameText(szName, pContact, FALSE);
			
			sFav->szFriendlyName = m_oStr->CreateAndCopy(szName);

			bFoundItem = TRUE;
		}

		if(bFoundItem)
			SaveRegistryPage(iPage);
	}

	if(bFoundItem)
		UpdateImageLoader();

	return S_OK;
}

HRESULT CGuiFavorites::PoomItemDeleted(long lOid)
{
	HRESULT hr = S_OK;
	// get the item from array
	for(int iPage= 0; iPage < m_arrPages.GetSize(); iPage++)
	{
		TypePage* sPage = m_arrPages[iPage];
		if(!sPage)
			continue;

		for(int iFavItem = 0; iFavItem < sPage->arrFavs.GetSize(); iFavItem++)
		{
			TypeFavItem* sFav = sPage->arrFavs[iFavItem];
			if(!sFav || sFav->eType != FAV_Contact || sFav->iIndex != (int)lOid)
				continue;

			BOOL bFound = FALSE;
			for(int y=0; y<MAX_Items; y++)
			{
				if(bFound)
					break;
				for(int x=0; x<MAX_Items; x++)
				{
					if(sPage->arrGrid[x][y] == iFavItem)
					{
						POINT ptItem;
						ptItem.x	= x;
						ptItem.y	= y;
						bFound = TRUE;
						RemoveItem(iPage, ptItem);
					}

					if(bFound)
						break;
				}
			}

			break;
		}
	}

//Error:
	return hr;
}

HRESULT CGuiFavorites::LaunchSelectedItem()
{
	HRESULT hr = S_OK;

	CBARG(IsValidGrid(m_ptSelectedItem), _T(""));

	// try and launch this item
	TypePage* sPage = m_arrPages[m_oPicControl.GetSelectedItemIndex()];
	CPHR(sPage, _T(""));

	int iIndex = sPage->arrGrid[m_ptSelectedItem.x][m_ptSelectedItem.y];
	TypeFavItem* sFav = sPage->arrFavs[iIndex];
	if(sFav)
	{
		// make sure the pic control is back to what is always was
		m_oPicControl.ShowSelectedItem(FALSE);

		if(sFav->eType == FAV_Contact)
		{
			CPoomContacts* pPoom = CPoomContacts::Instance();
			if(pPoom)
			{
				int iIndex = pPoom->GetIndexFromOID(sFav->iIndex);
#ifndef NOPHONE
				m_dlgContactDetails.SetLastNameFirst(TRUE);
				m_dlgContactDetails.SetIndex(iIndex);
				if(IDCANCEL == m_dlgContactDetails.Launch(m_hWndParent, m_hInst, TRUE))
				{
					if(g_sOptions->bHideOnAppLaunch)
						ShowWindow(m_hWndParent, SW_MINIMIZE);
				}
#endif
			}
		}
		else
		{
			if(g_sOptions->bHideOnAppLaunch)
				ShowWindow(m_hWndParent, SW_MINIMIZE);

			sFav->Launch(m_hWndParent);

			
		}
	}
	else
	{
		PostMessage(m_hWndParent, WM_COMMAND, IDMENU_AddNewItem, 0);
	}

Error:
	if(hr != S_OK)
		MessageBeep(MB_ICONHAND);
	return hr;
}
