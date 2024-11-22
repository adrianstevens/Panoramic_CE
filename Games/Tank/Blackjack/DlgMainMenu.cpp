#include "DlgMainMenu.h"
#include "DlgAbout.h"
//#include "DlgOptions.h"
#include "DlgHighScores.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "DlgMsgBox.h"
#include "IssCommon.h"

#define MENU_BTNS_Grad1		RGB(204,225,255)
#define MENU_BTNS_Grad2		RGB(145,173,209)
#define MENU_BTNS_Outline1	RGB(13,50,90)
#define MENU_BTNS_Outline2	RGB(255,255,255)
#define MENU_BTNS_TextColor	RGB(13,50,90)
#define	WM_MENU_Button		WM_USER+100
#define TEXT_INDENT         (GetSystemMetrics(SM_CXICON)/4) 

CDlgMainMenu::CDlgMainMenu(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_iSelected(0)
{
}

CDlgMainMenu::~CDlgMainMenu(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
}

BOOL CDlgMainMenu::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

	m_bFirstTime = TRUE;


	return TRUE;
}

BOOL CDlgMainMenu::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

	if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	if((rcClip.left == rcClient.left	&&
		rcClip.right== rcClient.right	&&
		rcClip.top  == rcClient.top		&&
		rcClip.bottom == rcClient.bottom) ||
		m_gdiImage.GetDC() == NULL)
	{
		DrawBackground(rcClient);

		for(int i=0; i<BTN_Count; i++)
			DrawButton(i);
	}

	// draw all to the screen
	BitBlt(hDC,
		rcClip.left,rcClip.top,
		WIDTH(rcClip), HEIGHT(rcClip),
		m_gdiMem.GetDC(),
		rcClip.left,rcClip.top,
		SRCCOPY);


	return TRUE;
}

BOOL CDlgMainMenu::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgMainMenu::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
   	case IDMENU_About:
		ShowAboutScreen();
		break;
	case IDMENU_Options:
		ShowOptionsScreen();
		break;
	case IDMENU_HiScores:
		ShowHighScoreScreen();
		break;
	case IDMENU_Help:
#ifdef WIN32_PLATFORM_PSPC
        CreateProcess(_T("peghelp"), _T("PanoBlackjack.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#else
        LaunchHelp( _T("PanoBlackjack.htm"), m_hInst);
#endif
		break;
	case IDMENU_Exit:
	{
		CDlgMsgBox dlgMsg;
		if(IDYES != dlgMsg.PopupMessage(IDS_MSG_Exit, m_hWnd, m_hInst, MB_YESNO))
			break;
	}
	case IDMENU_NewGame:
	case IDMENU_Resume:
		SafeCloseWindow(LOWORD(wParam));	// let the main window handle this
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgMainMenu::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiImage.Destroy();
	m_gdiMem.Destroy();
	m_btnItem.Destroy();

	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*int iBtn = -1;
	int iOldSelected = m_iSelected;

	switch(wParam)
	{
	case VK_UP:
		if(m_iSelected == 0)
			m_iSelected = BTN_Count-2;
		else
			m_iSelected --;
		break;
	case VK_DOWN:
		if(m_iSelected >= BTN_Count-2)
			m_iSelected = 0;
		else
			m_iSelected ++;
		break;
	case VK_RETURN:
		iBtn = m_iSelected;
		break;
	}

	if(iBtn > -1 && iBtn < BTN_Count-2)
	{	
		PostMessage(m_hWnd, WM_MENU_Button + iBtn, 0,0);

	}

	CIssRect rcDraw(m_rcItems[iOldSelected]);
	DrawBackground(rcDraw);
	DrawButton(iOldSelected);

	DrawBackground(m_rcItems[m_iSelected]);
	DrawButton(m_iSelected);
	rcDraw.Concatenate(m_rcItems[m_iSelected]);

	InvalidateRect(m_hWnd, &rcDraw.Get(), FALSE);*/

	return TRUE;
}

BOOL CDlgMainMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_TSOFT1:
        SafeCloseWindow(m_bIsGameInPlay?IDMENU_Resume:IDMENU_NewGame);
        break;
    case VK_TSOFT2:
        ShowMenu();
        break;
    case VK_ESCAPE:
        SafeCloseWindow(0);
        break;

    }
	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcItems[BTN_NewGame], pt))
	{
        SafeCloseWindow(m_bIsGameInPlay?IDMENU_Resume:IDMENU_NewGame);
	}
	else if(PtInRect(&m_rcItems[BTN_Menu], pt))
	{
		DrawBackground(m_rcItems[BTN_Menu]);
		DrawButton(BTN_Menu, TRUE);
	}
	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonUp(HWND hWnd, POINT& pt)
{
	for(int i = 0; i < BTN_Count-1; i++)
	{
		if(PtInRect(&m_rcItems[i], pt))
		{
			/*int iOldSelected = m_iSelected;
			m_iSelected = i;
			CIssRect rcDraw(m_rcItems[iOldSelected]);
			DrawBackground(rcDraw);			
			DrawButton(iOldSelected);	
			DrawBackground(m_rcItems[i]);
			DrawButton(m_iSelected);
			rcDraw.Concatenate(m_rcItems[i]);
			InvalidateRect(m_hWnd, &rcDraw.Get(), FALSE);*/
			PostMessage(m_hWnd, WM_MENU_Button + i, 0,0);
			break;
		}
	}
	if(PtInRect(&m_rcItems[BTN_Menu], pt))
	{
		DrawBackground(m_rcItems[BTN_Menu]);
		DrawButton(BTN_Menu, TRUE);
		PostMessage(m_hWnd, WM_MENU_Button + BTN_Menu, 0,0);
	}
	return TRUE;
}

BOOL CDlgMainMenu::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

		return TRUE; 
	}
	return UNHANDLED;
}

BOOL CDlgMainMenu::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnMeasureItem(hWnd, wParam, lParam);
}

BOOL CDlgMainMenu::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnDrawItem(hWnd, wParam, lParam);
}

BOOL CDlgMainMenu::OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnEnterMenuLoop(hWnd, wParam, lParam);
}

BOOL CDlgMainMenu::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_MENU_Button + BTN_Menu)
		ShowMenu();
	/*else if(uiMessage == WM_MENU_Button + BTN_Resume)
	{}*/
	else if(uiMessage == WM_MENU_Button + BTN_NewGame)
	{}
/*	else if(uiMessage == WM_MENU_Button + BTN_Quit)
	{}*/

	return TRUE;
}

BOOL CDlgMainMenu::FadeInScreen(HDC hDC, RECT rcClient)
{
	InitBackground();
	InitButtons();
	FadeIn(hDC, m_gdiMem, 900, fnAnimateIn, this);
	return TRUE;
}

void CDlgMainMenu::InitButtons()
{
	RECT rcSize;

	if(m_btnItem.IsLoaded())
		return;

	int iWidth		= GetSystemMetrics(SM_CXSCREEN);
	int iHeight		= GetSystemMetrics(SM_CYSCREEN);
	int iBtnWidth	= iWidth/2 -iWidth/8;
	int iBtnHeight	= GetSystemMetrics(SM_CYICON);
	int iBtnSpacing	= 1*iHeight/100;
	int iShadowSpace= 0;//2*iHeight/100;
	int iTopOffset	= (int)20*iHeight/100;

	rcSize.left		= 0;
	rcSize.top		= 0;
	rcSize.right	= iBtnWidth;
	rcSize.bottom	= iBtnHeight;
    m_btnItem.Initialize(m_hWnd, m_hInst, (IsVGA()?IDR_PNG_MenuBtnVGA:IDR_PNG_MenuBtn));
    m_btnItem.SetSize(iBtnWidth, iBtnHeight);

	for(int i=0; i<BTN_Count; i++)
	{
		// get the location
		rcSize.left		= 0-iShadowSpace;
		rcSize.right	= rcSize.left + iBtnWidth;
		rcSize.top		= iHeight - iBtnHeight + iShadowSpace;
		rcSize.bottom	= rcSize.top + iBtnHeight;

		if(i == BTN_Menu)
		{
			rcSize.left		= iWidth - iBtnWidth + iShadowSpace;
			rcSize.right	= rcSize.left + iBtnWidth;
			rcSize.top		= iHeight - iBtnHeight + iShadowSpace;
			rcSize.bottom	= rcSize.top + iBtnHeight;
		}

		m_rcItems[i]		= rcSize;
	}


}

void CDlgMainMenu::fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateIn(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	int iNewLocationY;
	int iStartLocationY	= GetSystemMetrics(SM_CYSCREEN);
    RECT rc;
	for(int i=0; i<BTN_Count; i++)
	{
		iNewLocationY		= iStartLocationY + (m_rcItems[i].top - iStartLocationY)*iAnimeStep/iAnimTotal;

		/*CIssGDI::MyAlphaImage(gdiDest,
			m_rcItems[i].left, iNewLocationY,
			m_gdiButtons[i].GetWidth(), m_gdiButtons[i].GetHeight(),
			m_gdiButtons[i],
			*m_btnItem.GetAlphaGDI(),
			0,0);*/
        //m_btnItem.DrawImage(gdiDest,m_rcItems[i].left, iNewLocationY);

        rc.left     = m_rcItems[i].left;
        rc.top      = iNewLocationY;
        rc.right    = rc.left + m_btnItem.GetWidth();
        rc.bottom   = rc.top + m_btnItem.GetHeight();
        DrawBtnText(i, rc);
	}
}

void CDlgMainMenu::fnAnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateOut(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);


	int iNewLocationY;
	int iEndLocationY	= GetSystemMetrics(SM_CYSCREEN) + m_btnItem.GetHeight();
    RECT rc;
	for(int i=0; i<BTN_Count; i++)
	{
		iNewLocationY		=  m_rcItems[i].top + (iEndLocationY - m_rcItems[i].top)*iAnimeStep/iAnimTotal;
		
		/*CIssGDI::MyAlphaImage(gdiDest,
			m_rcItems[i].left, iNewLocationY,
			m_gdiButtons[i].GetWidth(), m_gdiButtons[i].GetHeight(),
			m_gdiButtons[i],
			*m_btnItem.GetAlphaGDI(),
			0,0);*/
        //m_btnItem.DrawImage(gdiDest,m_rcItems[i].left, iNewLocationY);

        rc.left     = m_rcItems[i].left;
        rc.top      = iNewLocationY;
        rc.right    = rc.left + m_btnItem.GetWidth();
        rc.bottom   = rc.top + m_btnItem.GetHeight();
        DrawBtnText(i, rc);
	}
}

BOOL CDlgMainMenu::AnimateOutScreen()
{
	HDC hDC = GetDC(m_hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	BitBlt(m_gdiMem.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	FadeIn(hDC, m_gdiMem, 900, fnAnimateOut, this);
	ReleaseDC(m_hWnd, hDC);
	return TRUE;
}

void CDlgMainMenu::ShowMenu()
{
	if(m_oMenu.IsMenuUp())
		return;

	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return;

	m_oMenu.Initialize(m_hWnd, m_hInst, RGB(255,255,255), 0x8DEEA4, TRUE, 0);

	//m_oMenu.AppendMenu(hMenu, MF_STRING|(m_oGame->IsGameInPlay()?NULL:MF_GRAYED), IDMENU_Resume, IDS_MENU_Resume);
	//m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
    if(m_bIsGameInPlay)
        m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_NewGame, IDS_MENU_NewGame);
    else
        m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Resume, IDS_MENU_Resume);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Options, IDS_MENU_Options);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, IDS_MENU_HiScores);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_About, IDS_MENU_About);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Help, IDS_MENU_Help);
//	m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Exit, IDS_MENU_Exit);

	POINT pt;
	int iShadowSpace= 3*GetSystemMetrics(SM_CYSCREEN)/100;
	RECT rc = m_rcItems[BTN_Menu];
	pt.x	= rc.right - iShadowSpace;
	pt.y	= rc.top + iShadowSpace;

	//Display it.
	m_oMenu.PopupMenu(	hMenu, 
		TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y);

	DestroyMenu(hMenu);
}

void CDlgMainMenu::InitBackground()
{
	if(!m_gdiImage.GetDC())
	{
		CIssGDIEx gdiBackground;

		int iWidth		= GetSystemMetrics(SM_CXSCREEN);
		int iHeight		= GetSystemMetrics(SM_CYSCREEN);

		if(GetSystemMetrics(SM_CXICON) > 44)
			//LoadImage(gdiBackground, IDR_SPLASH_VGA, m_hWnd, m_hInst);
            gdiBackground.LoadImage(IDR_PNG_TitleVGA, m_hWnd, m_hInst, FALSE);
		else
			//LoadImage(gdiBackground, IDR_SPLASH, m_hWnd, m_hInst);
            gdiBackground.LoadImage(IDR_PNG_Title, m_hWnd, m_hInst, FALSE);

		m_gdiImage.Create(m_gdiMem.GetDC(), iWidth, iHeight, FALSE, TRUE);

		BitBlt(m_gdiImage.GetDC(),
			0,0,
			iWidth, iHeight,
			gdiBackground.GetDC(),
			max(0, (gdiBackground.GetWidth()-iWidth)/2), 0,
			SRCCOPY);
	}
}

void CDlgMainMenu::DrawBackground(RECT rcLocation)
{
	InitBackground();

	// copy the background over
	BitBlt(m_gdiMem.GetDC(),
		rcLocation.left,rcLocation.top,
		WIDTH(rcLocation), HEIGHT(rcLocation),
		m_gdiImage.GetDC(),
		rcLocation.left,rcLocation.top,
		SRCCOPY);
}

void CDlgMainMenu::DrawButton(int iBtnIndex, BOOL bDrawDown)
{
	InitButtons();

	/*CIssGDI::MyAlphaImage(m_gdiMem,
							m_rcItems[iBtnIndex].left, m_rcItems[iBtnIndex].top,
							m_gdiButtons[iBtnIndex].GetWidth(), m_gdiButtons[iBtnIndex].GetHeight(),
							m_gdiButtons[iBtnIndex],
							*m_btnItem.GetAlphaGDI(),
							0,0);*/

    //m_btnItem.DrawImage(m_gdiMem,m_rcItems[iBtnIndex].left, m_rcItems[iBtnIndex].top);


    RECT rc;
    rc.left     = m_rcItems[iBtnIndex].left;
    rc.top      = m_rcItems[iBtnIndex].top;
    rc.right    = rc.left + m_btnItem.GetWidth();
    rc.bottom   = rc.top + m_btnItem.GetHeight();
    DrawBtnText(iBtnIndex, rc);

}

void CDlgMainMenu::DrawBtnText(int iBtnIndex, RECT& rc)
{
    RECT rcShadow = rc;
    rcShadow.left   ++;
    rcShadow.top    ++;
    rcShadow.right  ++;
    rcShadow.bottom ++;

    switch(iBtnIndex)
    {
    case BTN_NewGame:
        rcShadow.left += TEXT_INDENT;
        rc.left       += TEXT_INDENT;
        DrawText(m_gdiMem, m_bIsGameInPlay?_T("Resume"):_T("New Game"), rcShadow, DT_LEFT|DT_VCENTER, m_hFontText, RGB(100,100,100));
        DrawText(m_gdiMem, m_bIsGameInPlay?_T("Resume"):_T("New Game"), rc, DT_LEFT|DT_VCENTER, m_hFontText, RGB(255,255,255));
        break;
    case BTN_Menu:
        rcShadow.right  -= TEXT_INDENT;
        rc.right        -= TEXT_INDENT;
        DrawText(m_gdiMem, _T("Menu"), rcShadow, DT_RIGHT|DT_VCENTER, m_hFontText, RGB(100,100,100));
        DrawText(m_gdiMem, _T("Menu"), rc, DT_RIGHT|DT_VCENTER, m_hFontText, RGB(255,255,255));
        break;
    }
}

void CDlgMainMenu::ShowAboutScreen()
{
	CDlgAbout dlgAbout;
	dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowOptionsScreen()
{
	CDlgOptions dlgOptions;
	dlgOptions.Init(m_psOptions);
	dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowHighScoreScreen()
{
	CDlgHighScores dlgHighScores;
	dlgHighScores.Init(m_oHighScores);
	dlgHighScores.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}



