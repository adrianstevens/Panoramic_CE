#include "StdAfx.h"
#include "DlgGuitarTuner.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssGDIDraw.h"


CDlgGuitarTuner::CDlgGuitarTuner()
:m_guiCurrent(NULL)
{
	// read values from the registry
	LoadRegSettings();

	ZeroMemory(&m_sizeWindow, sizeof(SIZE));
		
	m_oStr = CIssString::Instance();
    m_guiCurrent = (&m_guiGuitarChords);
 }

CDlgGuitarTuner::~CDlgGuitarTuner(void)
{
	// write values to the registry
	SaveRegSettings();
}

void CDlgGuitarTuner::Init(HINSTANCE hInst)
{
	m_hInst = hInst;

}

BOOL CDlgGuitarTuner::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

	// start the wait cursor because this could take a few seconds
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE); 
	
    // End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

    m_guiGuitarChords.OnSize(hWnd, wParam, lParam);
    m_guiGuitarTuner.OnSize(hWnd, wParam, lParam);
    m_guiMetronome.OnSize(hWnd, wParam, lParam);

	return TRUE;
}

BOOL CDlgGuitarTuner::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN |  SHIDIF_SIPDOWN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

/*	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_GuitarTuner;
	mbi.hInstRes	= m_hInst;
	    if(!SHCreateMenuBar(&mbi))
	{
		int i = 0;
	}*/
#endif
    m_guiGuitarChords.Init(hWnd, m_hInst);
    m_guiGuitarTuner.Init(hWnd, m_hInst);
    m_guiMetronome.Init(hWnd, m_hInst);

	return TRUE;
}

BOOL CDlgGuitarTuner::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnPaint(hWnd, hDC, rcClient);
	return TRUE;
}

BOOL CDlgGuitarTuner::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnTimer(hWnd, wParam, lParam);
    return UNHANDLED;
}

BOOL CDlgGuitarTuner::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnLButtonUp(hWnd, pt);
    return UNHANDLED;
}

BOOL CDlgGuitarTuner::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnLButtonDown(hWnd, pt);

	return UNHANDLED;
}

BOOL CDlgGuitarTuner::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnMouseMove(m_hWnd, pt);
	return FALSE;
}



BOOL CDlgGuitarTuner::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_LeftHanded:
		m_guiGuitarChords.SetLeft(!m_guiGuitarChords.IsLeft());
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_FretNumbers:
		m_guiGuitarChords.SetShowFretNum(!m_guiGuitarChords.ShowFretNum());
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
    case IDMENU_Guitar:
        m_guiCurrent = (&m_guiGuitarTuner);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case IDMENU_Metronome:
        m_guiCurrent = (&m_guiMetronome);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case IDMENU_Chords:
        m_guiCurrent = (&m_guiGuitarChords);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
	case IDOK:
		//OnKeyDown(hWnd, VK_RETURN, NULL);
		//OnKeyUp(hWnd, VK_RETURN, NULL);
		break;
    case IDMENU_Menu:
        //MessageBox(m_hWnd, _T("Guitar Tuner 1.0.0"), _T("About"), MB_OK);
        OnMenuMenu();
        break;
    case IDMENU_Mode:
        OnMenuMode();
        break;
    case IDMENU_Quit:
        PostQuitMessage(0);
        break;
    case IDMENU_About:
        OnAbout();
        break;
    case IDMENU_Options:
        OnOptions();
        break;
    case IDMENU_Help:
        
#ifdef WIN32_PLATFORM_PSPC
        CreateProcess(_T("peghelp"), _T("Panoguitartuner.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#else
        LaunchHelp( _T("Panoguitartuner.htm"), m_hInst);
#endif
        break;
    default:
        m_guiCurrent->OnCommand(hWnd, wParam, lParam);
        break;
	}

	return FALSE;
}

BOOL CDlgGuitarTuner::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

void CDlgGuitarTuner::LoadRegSettings()
{
	DWORD dwVal	= 0;
}

void CDlgGuitarTuner::SaveRegSettings()
{
	DWORD dwTemp = 0;
}

BOOL CDlgGuitarTuner::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgGuitarTuner::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgGuitarTuner::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
 	return UNHANDLED;
}

BOOL CDlgGuitarTuner::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {	
        //this should only exist when using a smartphone in landscape orientation (for now anyways)
        bRet = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

    }
#endif
    return bRet;
}

BOOL CDlgGuitarTuner::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //InvalidateRect(m_hWnd, NULL, FALSE);
    switch(LOWORD(wParam))
    {
    case _T('q'):
        PostQuitMessage(0);
    	break;
    case _T('c'):
    	break;
    case _T('t'):
        break;
    case _T('p'):
        break;
    case _T('s'):
        OnSize(hWnd, 0, 0);
        break;
    default:
        break;
    }


    return TRUE;
}


BOOL CDlgGuitarTuner::OnMenuMenu()
{
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];

	if(m_guiCurrent->GetGUI() == GUI_Chords)
	{
		m_oStr->StringCopy(szText, IDS_MENU_LeftHanded, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING | m_guiGuitarChords.IsLeft()?MF_CHECKED:NULL, IDMENU_LeftHanded, szText);

		m_oStr->StringCopy(szText, IDS_MENU_FretNumbers, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING | m_guiGuitarChords.ShowFretNum()?MF_CHECKED:NULL, IDMENU_FretNumbers, szText);

	}

//    m_oStr->StringCopy(szText, IDS_MENU_Options, STRING_MAX, m_hInst);
//    AppendMenu(hMenu, MF_STRING, IDMENU_Options, szText);
    m_oStr->StringCopy(szText, IDS_MENU_About, STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_About, szText);
    m_oStr->StringCopy(szText, IDS_MENU_Quit, STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_Quit, szText);

    /*   int iChecked = 0;
    if(m_oBtnManger.GetCalcType() == CALC_Standard)
    iChecked = 0;
    else if(m_oBtnManger.GetCalcType() == CALC_Scientific)
    iChecked = 1;
    else if(m_oBtnManger.GetCalcType() == CALC_BaseConversions)
    iChecked = 2;

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)iChecked, TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, 3, iChecked, MF_BYPOSITION);*/

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    pt.x	= rc.left + GetSystemMetrics(SM_CXVSCROLL);
    pt.y    = rc.bottom;
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

    DestroyMenu(hMenu);

    return TRUE;
}

BOOL CDlgGuitarTuner::OnMenuMode()
{
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];

    
	m_oStr->StringCopy(szText, IDS_MENU_Chords, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Chords, szText);

	m_oStr->StringCopy(szText, IDS_MENU_Guitar, STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_Guitar, szText);
//    m_oStr->StringCopy(szText, IDS_MENU_Metronome, STRING_MAX, m_hInst);
//    AppendMenu(hMenu, MF_STRING, IDMENU_Metronome, szText);

 /*   int iChecked = 0;
    if(m_oBtnManger.GetCalcType() == CALC_Standard)
        iChecked = 0;
    else if(m_oBtnManger.GetCalcType() == CALC_Scientific)
        iChecked = 1;
    else if(m_oBtnManger.GetCalcType() == CALC_BaseConversions)
        iChecked = 2;

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)iChecked, TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, 3, iChecked, MF_BYPOSITION);*/

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
    pt.y    = rc.bottom;
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

    DestroyMenu(hMenu);

    return TRUE;
}

BOOL CDlgGuitarTuner::OnOptions()
{

    return TRUE;
}

BOOL CDlgGuitarTuner::OnAbout()
{
    MessageBox(m_hWnd, _T("Panoramic Guitar Pal 0.9.0"), _T("About"), MB_OK);
    return TRUE;
}