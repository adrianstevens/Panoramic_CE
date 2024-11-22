#include "DlgMainMenu.h"
#include "DlgAbout.h"
#include "DlgOptions.h"
#include "DlgHighScores.h"
#include "resource.h"
#include "IssGDIEffects.h"
#include "stdafx.h"
#include "DlgExit.h"
#include "IssCommon.h"
#include "IssMenuHandler.h"

#define MENU_BTNS_Grad1		RGB(204,225,255)
#define MENU_BTNS_Grad2		RGB(145,173,209)
#define MENU_BTNS_Outline1	RGB(13,50,90)
#define MENU_BTNS_Outline2	RGB(255,255,255)
#define MENU_BTNS_TextColor	RGB(13,50,90)
#define	WM_MENU_Button		WM_USER+100

CDlgMainMenu::CDlgMainMenu(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_iSelected(0)
{
}

CDlgMainMenu::~CDlgMainMenu(void)
{
	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
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
#ifdef WIN32_PLATFORM_WFSP
        LaunchHelp(_T("HexeGems.htm"), m_hInst);
#else
		CreateProcess(_T("peghelp"), _T("HexeGems.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#endif
		break;
	case IDMENU_Exit:
	{
		CDlgExit dlgExit;
		dlgExit.Init(m_oKeys);
		if(IDYES != dlgExit.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
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

	for(int i=0; i<BTN_Count; i++)
		m_gdiButtons[i].Destroy();

	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iBtn = -1;
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

	InvalidateRect(m_hWnd, &rcDraw.Get(), FALSE);

	return TRUE;
}

BOOL CDlgMainMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_ESCAPE:
    case VK_TSOFT1:
        SafeCloseWindow(IDMENU_NewGame);
        break;
    case VK_TSOFT2:
        ShowMenu();
        break;
    }
	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcItems[BTN_NewGame], pt))
	{
		SafeCloseWindow(IDMENU_NewGame);
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
			int iOldSelected = m_iSelected;
			m_iSelected = i;
			CIssRect rcDraw(m_rcItems[iOldSelected]);
			DrawBackground(rcDraw);			
			DrawButton(iOldSelected);	
			DrawBackground(m_rcItems[i]);
			DrawButton(m_iSelected);
			rcDraw.Concatenate(m_rcItems[i]);
			InvalidateRect(m_hWnd, &rcDraw.Get(), FALSE);
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

		m_oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Soft1|KEY_Soft2);

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
	CIssGDIEffects::FadeIn(hDC, m_gdiMem, 900, fnAnimateIn, this);
	return TRUE;
}

void CDlgMainMenu::InitButtons()
{
	RECT rcSize;

	if(m_gdiButtons[0].GetDC() != NULL)
		return;

	int iWidth		= GetSystemMetrics(SM_CXSCREEN);
	int iHeight		= GetSystemMetrics(SM_CYSCREEN);
	int iBtnWidth	= iWidth/2;
#ifdef WIN32_PLATFORM_WFSP
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*6/5;
#else
	int iBtnHeight	= GetSystemMetrics(SM_CXICON)*3/2;
#endif
	int iBtnSpacing	= 1*iHeight/100;
	int iShadowSpace= 2*iHeight/100;
	int iTopOffset	= (int)20*iHeight/100;

	rcSize.left		= 0;
	rcSize.top		= 0;
	rcSize.right	= iBtnWidth;
	rcSize.bottom	= iBtnHeight;
	if(m_btnItem.GetImageDC() == NULL || m_btnItem.GetAlphaDC() == NULL)
	{
		m_btnItem.Initialize(m_hWnd, m_hInst, 
						   IDR_MN_TL, IDR_MN_T, IDR_MN_TR, IDR_MN_R, IDR_MN_BR, IDR_MN_B, IDR_MN_BL, IDR_MN_L, IDR_MN_M);
		m_btnItem.InitializeAlpha(IDR_MNA_TL, IDR_MNA_T, IDR_MNA_TR, IDR_MNA_R, IDR_MNA_BR, IDR_MNA_B, IDR_MNA_BL, IDR_MNA_L, IDR_MNA_M);
		m_btnItem.SetLocation(rcSize);
		m_btnItem.GenerateImages(m_gdiMem.GetDC());
	}

	TCHAR szText[STRING_MAX];
	for(int i=0; i<BTN_Count; i++)
	{
		// get the label
		switch(i)
		{
		/*case BTN_Resume:
			m_oStr->StringCopy(szText, IDS_MENU_Resume, STRING_MAX, m_hInst);
			break;*/
		case BTN_NewGame:
			m_oStr->StringCopy(szText, IDS_MENU_NewGame, STRING_MAX, m_hInst);
			break;
		/*case BTN_Quit:
			m_oStr->StringCopy(szText, IDS_MENU_Quit, STRING_MAX, m_hInst);
			break;*/
		case BTN_Menu:
			m_oStr->StringCopy(szText, IDS_MENU_Menu, STRING_MAX, m_hInst);
			break;
		}

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


		m_gdiButtons[i].Create(m_btnItem.GetImageDC(), m_btnItem.GetWidth(), m_btnItem.GetHeight(), TRUE, TRUE);
		CIssRect rcZero(rcSize);
		rcZero.ZeroBase();
		rcZero.Translate(1,1);
		CIssGDI::DrawText(m_gdiButtons[i].GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(m_gdiButtons[i].GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
	//	m_gdiButtons[i].ConvertToDIB();

	}


}

void CDlgMainMenu::fnAnimateIn(CIssGDI& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateIn(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateIn(CIssGDI& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	int iNewLocationY;
	int iStartLocationY	= GetSystemMetrics(SM_CYSCREEN);
	for(int i=0; i<BTN_Count; i++)
	{
		iNewLocationY		= iStartLocationY + (m_rcItems[i].top - iStartLocationY)*iAnimeStep/iAnimTotal;

		CIssGDI::MyAlphaImage(gdiDest,
			m_rcItems[i].left, iNewLocationY,
			m_gdiButtons[i].GetWidth(), m_gdiButtons[i].GetHeight(),
			m_gdiButtons[i],
			*m_btnItem.GetAlphaGDI(),
			0,0);
	}
}

void CDlgMainMenu::fnAnimateOut(CIssGDI& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateOut(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateOut(CIssGDI& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);


	int iNewLocationY;
	int iEndLocationY	= GetSystemMetrics(SM_CYSCREEN) + m_gdiButtons[0].GetHeight();
	for(int i=0; i<BTN_Count; i++)
	{
		iNewLocationY		=  m_rcItems[i].top + (iEndLocationY - m_rcItems[i].top)*iAnimeStep/iAnimTotal;
		
		CIssGDI::MyAlphaImage(gdiDest,
			m_rcItems[i].left, iNewLocationY,
			m_gdiButtons[i].GetWidth(), m_gdiButtons[i].GetHeight(),
			m_gdiButtons[i],
			*m_btnItem.GetAlphaGDI(),
			0,0);
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

	CIssGDIEffects::FadeIn(hDC, m_gdiMem, 900, fnAnimateOut, this);
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

	m_oMenu.Initialize(m_hWnd, m_hInst, RGB(255,255,255), RGB(0,100,200), TRUE, IDR_BEVEL);

	//m_oMenu.AppendMenu(hMenu, MF_STRING|(m_oGame->IsGameInPlay()?NULL:MF_GRAYED), IDMENU_Resume, IDS_MENU_Resume);
	//m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Options, IDS_MENU_Options);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, IDS_MENU_HiScores);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_About, IDS_MENU_About);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Help, IDS_MENU_Help);
	m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
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
		CIssGDI gdiBackground;

		int iWidth		= GetSystemMetrics(SM_CXSCREEN);
		int iHeight		= GetSystemMetrics(SM_CYSCREEN);

		if(iWidth >= 320 && iHeight >= 320)
			LoadImage(gdiBackground, IDR_SPLASH_VGA, m_hWnd, m_hInst);
		else
			LoadImage(gdiBackground, IDR_SPLASH, m_hWnd, m_hInst);

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

	CIssGDI::MyAlphaImage(m_gdiMem,
							m_rcItems[iBtnIndex].left, m_rcItems[iBtnIndex].top,
							m_gdiButtons[iBtnIndex].GetWidth(), m_gdiButtons[iBtnIndex].GetHeight(),
							m_gdiButtons[iBtnIndex],
							*m_btnItem.GetAlphaGDI(),
							0,0);
}

void CDlgMainMenu::ShowAboutScreen()
{
	CDlgAbout dlgAbout;
	dlgAbout.Init(m_oKeys);
	dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowOptionsScreen()
{
	CDlgOptions dlgOptions;
	dlgOptions.Init(m_oSoundFX, m_oGame, m_oKeys);
	dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowHighScoreScreen()
{
	CDlgHighScores dlgHighScores;
	dlgHighScores.Init(m_oGame, m_oKeys);
	dlgHighScores.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}



