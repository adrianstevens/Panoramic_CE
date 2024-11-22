#include "ObjGui.h"
#include "aygshell.h"
#include "IssRegistry.h"
#include "issdebug.h"
#include "IssCommon.h"
#include "IssLocalisation.h"
#include "resource.h"
#include "ContactsGuiDefines.h"

#include "DlgOptions.h"

extern TypeOptions*			g_sOptions;

CObjGui::CObjGui(void)
:m_hWndParent(NULL)
,m_hInst(NULL)
,m_oStr(CIssString::Instance())
,m_iLineTextHeightTitle(-1)
,m_iLineTextHeightToolbar(-1)
,m_eCurrent(GUI_Favorites)
{
	m_hFontTitle			= CIssGDIEx::CreateFont(TITLE_TEXT_Height, FW_BOLD, TRUE);
	m_hFontToolbar			= CIssGDIEx::CreateFont(TEXT_HEIGHT_Large, FW_NORMAL, TRUE);
	m_hFontSearch           = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON), FW_BOLD, TRUE);
	m_hFontLetters			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

CObjGui::~CObjGui(void)
{
    Destroy();

	CIssGDIEx::DeleteFont(m_hFontTitle);
	CIssGDIEx::DeleteFont(m_hFontToolbar);
	CIssGDIEx::DeleteFont(m_hFontSearch);
	CIssGDIEx::DeleteFont(m_hFontLetters);
}

void CObjGui::Init(HWND hWndParent, HINSTANCE hInst)
{
    Destroy();
    m_hInst      = hInst;
    m_hWndParent = hWndParent;
}

void CObjGui::Destroy()
{
    m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	m_gdiIconArray.Destroy();
	m_gdiGlow.Destroy();
}

void CObjGui::AdjustWindow(HWND hWnd, BOOL bShowSIP)
{
    DWORD dwState;
	if(g_sOptions && g_sOptions->bShowFullScreen)
		dwState = SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON;
	else
		dwState = SHFS_SHOWTASKBAR|SHFS_SHOWSTARTICON|SHFS_HIDESIPBUTTON;


    ShowWindow(hWnd, SW_SHOW);
    SHFullScreen(hWnd, dwState); 

    if(!bShowSIP)
    {
        //get rid of the damn sip too ... just in case 
        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL); 
        if(hWndSip) 
            ShowWindow(hWndSip, SW_HIDE);
    }

    RECT rc, rcTaskBar; 

    HWND hWndTaskbar = NULL;

    hWndTaskbar= FindWindow(_T("HHTaskBar"), NULL);

    int iMenu = 0;
    if(hWndTaskbar)
    {
        GetWindowRect(hWndTaskbar,&rcTaskBar);
        iMenu = HEIGHT(rcTaskBar);
    }

    if(iMenu == 0)
    {
#ifdef WIN32_PLATFORM_PSPC 
        switch(GetSystemMetrics(SM_CXICON))
        {
        case 32:
            iMenu = 26;
            break;
        case 44:
            iMenu = 35;
            break;
        case 64:
            iMenu = 52;
            break;
        default:
            iMenu = GetSystemMetrics(SM_CXICON)*26/32;
            break;
        }
#else //smartphone
        //try this for now ....
        iMenu = GetSystemMetrics(SM_CXICON)*27/44;
#endif
    }

    int iYBottom = GetSystemMetrics(SM_CYSCREEN);
    if(bShowSIP)
        iYBottom -= iMenu;

	if(g_sOptions && g_sOptions->bShowFullScreen)
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
	else
		SetRect(&rc, 0, iMenu, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
	MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

}

BOOL CObjGui::OnSize(RECT& rcClient)
{
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
        return FALSE;

	m_bIsLandscape = (GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN));

    //if(m_bIsLandscape && HEIGHT(rcClient) == HEIGHT(m_rcToolbar) || !m_bIsLandscape && WIDTH(rcClient) == WIDTH(m_rcToolbar))
    //    return FALSE;

	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	m_gdiIconArray.Destroy();

	

	// move the toolbar
	RECT rcTemp = rcClient;

	if(m_bIsLandscape)	
	{
		int iMenuSize = 52;
		if(IsVGA())
			iMenuSize *= 2;
		rcTemp.right = iMenuSize;
	}
	else	// portrait, square				
	{
		int iMenuSize = 40;
		if(IsVGA())
			iMenuSize *= 2;
		rcTemp.top	= rcClient.bottom - iMenuSize;
	}

	m_rcToolbar = rcTemp;

	m_rcFavorites = m_rcPrograms = m_rcSettings = m_rcBack = m_rcToolbar;

	if(m_bIsLandscape)
	{
		int iHeight = HEIGHT(rcClient);
		m_rcFavorites.bottom	= iHeight/4;
		m_rcPrograms.top		= m_rcFavorites.bottom + 1;
		m_rcPrograms.bottom		= iHeight/2;
		m_rcSettings.top		= m_rcPrograms.bottom + 1;
		m_rcSettings.bottom		= 3*iHeight/4;
		m_rcBack.top			= 3*iHeight/4 + 1;
	}
	else
	{
		int iWidth = WIDTH(rcClient);
		m_rcFavorites.right		= iWidth/4;
		m_rcPrograms.left		= m_rcFavorites.right + 1;
		m_rcPrograms.right		= iWidth/2;
		m_rcSettings.left		= m_rcPrograms.right + 1;
		m_rcSettings.right		= 3*iWidth/4;
		m_rcBack.left			= 3*iWidth/4 + 1;

	}

	if(m_bIsLandscape)	
	{
		rcTemp.left = rcTemp.right+1;
		rcTemp.right = rcClient.right;
	}
	else	// portrait, square					
	{
		rcTemp.bottom = rcTemp.top-1;
		rcTemp.top = rcClient.top;
	}

	m_rcScreen = rcTemp;

	return TRUE;
}

void CObjGui::DrawScreen(RECT& rcClip, HDC dc)
{
    BitBlt(dc,
        rcClip.left, rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left, rcClip.top,
        SRCCOPY);
}

CIssGDIEx* CObjGui::GetGlow()
{
	if(m_gdiGlow.GetDC() == NULL)
	{
		m_gdiGlow.LoadImage(m_sSkin.uiGlow, m_hWndParent, m_hInst);
	}

	return &m_gdiGlow;
}

CIssGDIEx* CObjGui::GetGDI()
{
	if(m_gdiMem.GetDC() == NULL)
	{
		RECT rcClient;
		GetClientRect(m_hWndParent, &rcClient);
		HDC dc = GetDC(m_hWndParent);
		m_gdiMem.Create(dc, rcClient, FALSE, TRUE);
		ReleaseDC(m_hWndParent, dc);
	}

	return &m_gdiMem;
}

CIssGDIEx* CObjGui::GetIconArray()
{
	if(m_gdiIconArray.GetDC() == NULL)
	{
		m_gdiIconArray.LoadImage(IsVGA()?IDR_PROG_Icons_VGA:IDR_PROG_Icons, m_hWndParent, m_hInst);
	}

	return &m_gdiIconArray;
}

void CObjGui::InitBackground(RECT& rcClient)
{
	HRESULT hr = S_OK;
	RECT rcSeparator;
	CIssImageSliced imgBottom;
	
	CBARG(m_hWndParent && m_hInst, _T(""));

	hr = m_gdiBackground.Create(m_gdiMem, rcClient, FALSE, TRUE);
	CHR(hr, _T("hr = m_gdiBackground.Create(m_gdiMem, rcClient, FALSE, TRUE);"));

	// default to black
	FillRect(m_gdiBackground, rcClient, 0);

	// load the background
	if(g_sOptions->eBackground == BK_DefaultToday)
	{
		LoadDefaultBackground(rcClient);
	}
	else if(g_sOptions->eBackground == BK_Camera || g_sOptions->eBackground == BK_Picture)
	{
		LoadPictureBackground(rcClient);
	}
	else if(g_sOptions->eBackground == BK_Blue)
	{
		LoadColorBackground(rcClient, RGB(68, 134, 210));
	}
	else if(g_sOptions->eBackground == BK_Orange)
	{
		LoadColorBackground(rcClient, RGB(204, 102, 0));
	}
	else if(g_sOptions->eBackground == BK_Red)
	{
		LoadColorBackground(rcClient, RGB(102, 0, 0));
	}
	else if(g_sOptions->eBackground == BK_Green)
	{
		LoadColorBackground(rcClient, RGB(132, 211, 105));
	}
	else if(g_sOptions->eBackground == BK_Normal)
	{
		// draw the background
		RECT rc = {0};
		rc.bottom   = HEIGHT(rcClient);
		rc.right    = WIDTH(rcClient)*7/10;
		GradientFillRect(m_gdiBackground, rc, RGB(102,102,102), RGB(0,0,0), FALSE);
		rc.left     = rc.right;
		rc.right    = WIDTH(rcClient);
		GradientFillRect(m_gdiBackground, rc, RGB(106,106,106), RGB(12,12,12), FALSE);
	}

	m_gdiBackground.InitAlpha(FALSE);

	// draw the bottom bar
	hr = imgBottom.Initialize(m_hWndParent, m_hInst, IsLandscape()?m_sSkin.uiToolBarBackLan:m_sSkin.uiToolBarBack);
	CHR(hr, _T("hr = imgBottom.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_BOT_VGA:IDR_BOT);"));

	hr = imgBottom.SetSize(WIDTH(m_rcToolbar), HEIGHT(m_rcToolbar));
	CHR(hr, _T("hr = imgBottom.SetSize(WIDTH(m_rcToolbar), HEIGHT(m_rcToolbar));"));

	imgBottom.DrawImage(m_gdiBackground, m_rcToolbar.left, m_rcToolbar.top);

	// draw the separators
	rcSeparator = m_rcPrograms;

	if(IsLandscape())
	{
		rcSeparator.left	+= 4;
		rcSeparator.right	-= 4;

		rcSeparator.bottom = rcSeparator.top + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,0,1);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);

		rcSeparator.top	= m_rcSettings.top;
		rcSeparator.bottom = rcSeparator.top + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,0,1);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);

		rcSeparator.top		= m_rcBack.top;
		rcSeparator.bottom	= rcSeparator.top + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,0,1);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);
	}
	else
	{
		rcSeparator.top		+= 4;
		rcSeparator.bottom	-= 4;

		rcSeparator.right = rcSeparator.left + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,1,0);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);

		rcSeparator.left	= m_rcSettings.left;
		rcSeparator.right = rcSeparator.left + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,1,0);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);

		rcSeparator.left	= m_rcBack.left;
		rcSeparator.right	= rcSeparator.left + 1;
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_DARK, 150);
		OffsetRect(&rcSeparator,1,0);
		AlphaFillRect(m_gdiBackground, rcSeparator, COLOR_TOOLBAR_SEP_LIGHT, 150);
	}

	// draw the icons and all the text for the toolbar background
	DrawToolbarItem(m_gdiBackground, GUI_Favorites, m_rcFavorites);
	DrawToolbarItem(m_gdiBackground, GUI_Programs, m_rcPrograms);
	DrawToolbarItem(m_gdiBackground, GUI_Settings, m_rcSettings);
	DrawToolbarItem(m_gdiBackground, GUI_Quit, m_rcBack);

Error:
	return;
}

HRESULT CObjGui::LoadDefaultBackground(RECT& rcClient)
{
	HRESULT hr = S_OK;
	TCHAR szTemp[MAX_PATH] = _T("");
	CIssGDIEx gdiBackground;

	hr = GetKey(_T("Software\\Microsoft\\Today"), _T("Wall"), szTemp, MAX_PATH);

	if(hr != S_OK || m_oStr->IsEmpty(szTemp) || !IsFileExists(szTemp))
	{
		m_oStr->StringCopy(szTemp, _T("\\windows\\tdycust.gif"));
		if(!IsFileExists(szTemp))
		{
			m_oStr->Format(szTemp, _T("\\windows\\tdywater_%d_%d.gif"), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			if(!IsFileExists(szTemp))
			{
				m_oStr->StringCopy(szTemp, _T("\\windows\\tdywater.gif"));
				if(!IsFileExists(szTemp))
				{
					m_oStr->Format(szTemp, _T("\\windows\\stwater_%d_%d.gif"), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
					if(!IsFileExists(szTemp))
					{
						m_oStr->StringCopy(szTemp, _T("\\windows\\stwater.gif"));
						if(!IsFileExists(szTemp))
						{
							m_oStr->Format(szTemp, _T("\\windows\\stwater_%d_%d.jpg"), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
							if(!IsFileExists(szTemp))
							{
								m_oStr->StringCopy(szTemp, _T("\\windows\\stwater.jpg"));
								if(!IsFileExists(szTemp))
								{
									m_oStr->Format(szTemp, _T("\\windows\\stwater_%d_%d.png"), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
									if(!IsFileExists(szTemp))
									{
										m_oStr->StringCopy(szTemp, _T("\\windows\\stwater.png"));
										if(!IsFileExists(szTemp))
										{
											m_oStr->Format(szTemp, _T("\\windows\\default_stwater_%d_%d.gif"), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
											if(!IsFileExists(szTemp))
											{
												m_oStr->StringCopy(szTemp, _T("\\windows\\default_stwater.gif"));
												if(!IsFileExists(szTemp))
												{
													hr = E_FAIL;
													goto Error;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	hr = gdiBackground.LoadImage(szTemp, m_hWndParent, TRUE, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), ASPECT_Cropped);
	CHR(hr, _T("hr = gdiBackground.LoadImage(szBackgroundImage, m_hWndParent, TRUE);"));

	// draw the background image
	TileBackGround(m_gdiBackground, rcClient, gdiBackground, FALSE);

	// darken a bit
	//AlphaFillRect(m_gdiBackground, rcClient, 0, 100);

Error:
	return hr;
}

HRESULT CObjGui::LoadPictureBackground(RECT& rcClient)
{
	HRESULT hr = S_OK;
	TCHAR szTemp[MAX_PATH] = _T("");
	CIssGDIEx gdiBackground;

	hr = GetKey(REG_Key, REGVAL_Background, szTemp, MAX_PATH);
	CBARG(hr == S_OK && !m_oStr->IsEmpty(szTemp) && IsFileExists(szTemp), _T(""));

	hr = gdiBackground.LoadImage(szTemp, m_hWndParent, TRUE, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), ASPECT_Cropped);
	CHR(hr, _T("hr = gdiBackground.LoadImage(szBackgroundImage, m_hWndParent, TRUE);"));

	// draw the background image
	TileBackGround(m_gdiBackground, rcClient, gdiBackground, FALSE);

Error:
	return hr;
}

HRESULT CObjGui::LoadColorBackground(RECT& rcClient, COLORREF cr)
{
	HRESULT hr = S_OK;

	hr = GradientFillRect(m_gdiBackground, rcClient, 0, cr, FALSE);

	return hr;
}

void CObjGui::DrawToolbarItem(CIssGDIEx& gdi, EnumCurrentGui eGui, RECT& rcLocation, BOOL bDrawGlow /*= FALSE*/)
{
	CIssGDIEx* gdiIcons = GetIconArray();
	int iIconWidth = gdiIcons->GetWidth()/4;
	int iIconHeight= gdiIcons->GetHeight();

	RECT rcIcon = rcLocation;
	
	rcIcon.bottom  = rcLocation.bottom - GetLineTextHeightToolbar() - INDENT;

	if(!IsLandscape() && HEIGHT(rcLocation) > HEIGHT(m_rcToolbar))
		rcIcon.bottom  = rcLocation.bottom - GetLineTextHeightToolbar() + 2*INDENT;

	Draw(gdi,
		rcIcon.left + (WIDTH(rcIcon) - iIconWidth)/2, rcIcon.top + (HEIGHT(rcIcon)-iIconHeight)/2,
		iIconWidth, iIconHeight,
		*gdiIcons,
		(int)eGui*iIconWidth, 0);

	UINT uiText;
	switch(eGui)
	{
	case GUI_Favorites:
		uiText = ID(IDS_TOOLBAR_FAV);
		break;
	case GUI_Programs:
		uiText = ID(IDS_TOOLBAR_PROGRAMS);
		break;
	case GUI_Settings:
		uiText = ID(IDS_TOOLBAR_SETTINGS);
		break;
	case GUI_Quit:
	default:
		uiText = ID(IDS_Back);
		break;
	}
	rcIcon = rcLocation;
	rcIcon.top = rcIcon.bottom - GetLineTextHeightToolbar() - INDENT;

	// if this is in landscape and we're drawing on the toolbar
	if(IsLandscape() && WIDTH(rcLocation) > WIDTH(m_rcToolbar))
	{
		rcIcon.right = m_rcToolbar.right;
		rcIcon.top	-= GetLineTextHeightToolbar();
		rcIcon.bottom -= GetLineTextHeightToolbar();
	}	
	

	DrawTextShadow(gdi, m_oStr->GetText(uiText, m_hInst), rcIcon, DT_CENTER|DT_VCENTER, m_hFontToolbar, RGB(255,255,255), RGB(0,0,0));
}

void CObjGui::DrawBackground(RECT& rcClip, CIssGDIEx* gdi)
{
	if(!gdi)
		gdi = GetGDI();

	if(m_gdiBackground.GetDC() == NULL)
	{
		RECT rcClient;
		GetClientRect(m_hWndParent, &rcClient);
		InitBackground(rcClient);
	}

	BitBlt(gdi->GetDC(),
		   rcClip.left, rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiBackground,
		   rcClip.left, rcClip.top,
		   SRCCOPY);
}

int CObjGui::GetLineTextHeightTitle()
{
	if(m_iLineTextHeightTitle <= 0)
	{
		SIZE sizeText = {0};
		HFONT hFontOld = (HFONT)SelectObject(m_gdiMem, m_hFontTitle);
		GetTextExtentPoint( m_gdiMem, _T("Ap"), (int)_tcslen( _T("Ap") ), &sizeText);
		SelectObject(m_gdiMem, hFontOld);
		m_iLineTextHeightTitle = sizeText.cy;
	}
	return m_iLineTextHeightTitle;
}

int CObjGui::GetLineTextHeightToolbar()
{
	if(m_iLineTextHeightToolbar <= 0)
	{
		SIZE sizeText = {0};
		HFONT hFontOld = (HFONT)SelectObject(m_gdiMem, m_hFontToolbar);
		GetTextExtentPoint( m_gdiMem, _T("Ap"), (int)_tcslen( _T("Ap") ), &sizeText);
		SelectObject(m_gdiMem, hFontOld);
		m_iLineTextHeightToolbar = sizeText.cy;
	}
	return m_iLineTextHeightToolbar;
}

void CObjGui::DeleteBackground()
{
	m_gdiBackground.Destroy();
	
	m_sSkin.crDrawText = (g_sOptions->eBackground == BK_DefaultToday?GetSysColor(COLOR_HIGHLIGHTTEXT):RGB(255,255,255));
}

void CObjGui::SetSkin()
{
	m_gdiGlow.Destroy();

	m_sSkin.crDrawText = (g_sOptions->eBackground == BK_DefaultToday?GetSysColor(COLOR_HIGHLIGHTTEXT):RGB(255,255,255));

	switch(g_sOptions->eColorScheme)
	{
	case COLOR_Green:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_GREEN_Arrow_VGA:IDR_GREEN_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_GREEN_Arrow_Lan_VGA:IDR_GREEN_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_GREEN_Glow_VGA:IDR_GREEN_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_GREEN_IA_VGA:IDR_GREEN_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_GREEN_Selector_VGA:IDR_GREEN_Selector;
		m_sSkin.uiSelectorGlass		= IDR_GREEN_SelectorGlass_VGA;
		m_sSkin.crSelected			= (COLORREF)0x39AF77;
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_GREEN_DETAILS_ButtonSelectVGA:IDR_GREEN_DETAILS_ButtonSelect;
		break;
	case COLOR_Blue:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_BLUE_Arrow_VGA:IDR_BLUE_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_BLUE_Arrow_Lan_VGA:IDR_BLUE_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_BLUE_Glow_VGA:IDR_BLUE_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_BLUE_IA_VGA:IDR_BLUE_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_BLUE_Selector_VGA:IDR_BLUE_Selector;
		m_sSkin.uiSelectorGlass		= IDR_BLUE_SelectorGlass_VGA;
		m_sSkin.crSelected			= RGB(34,96,139);
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_BLUE_DETAILS_ButtonSelectVGA:IDR_BLUE_DETAILS_ButtonSelect;
		break;
	case COLOR_Orange:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_ORANGE_Arrow_VGA:IDR_ORANGE_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_ORANGE_Arrow_Lan_VGA:IDR_ORANGE_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_ORANGE_Glow_VGA:IDR_ORANGE_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_ORANGE_IA_VGA:IDR_ORANGE_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_ORANGE_Selector_VGA:IDR_ORANGE_Selector;
		m_sSkin.uiSelectorGlass		= IDR_ORANGE_SelectorGlass_VGA;
		m_sSkin.crSelected			= RGB(142,94,31);
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_ORANGE_DETAILS_ButtonSelectVGA:IDR_ORANGE_DETAILS_ButtonSelect;
		break;
	case COLOR_Pink:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_PINK_Arrow_VGA:IDR_PINK_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_PINK_Arrow_Lan_VGA:IDR_PINK_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_PINK_Glow_VGA:IDR_PINK_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_PINK_IA_VGA:IDR_PINK_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_PINK_Selector_VGA:IDR_PINK_Selector;
		m_sSkin.uiSelectorGlass		= IDR_PINK_SelectorGlass_VGA;
		m_sSkin.crSelected			= RGB(142,31,88);
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_PINK_DETAILS_ButtonSelectVGA:IDR_PINK_DETAILS_ButtonSelect;
		break;
	case COLOR_Purple:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_PURPLE_Arrow_VGA:IDR_PURPLE_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_PURPLE_Arrow_Lan_VGA:IDR_PURPLE_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_PURPLE_Glow_VGA:IDR_PURPLE_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_PURPLE_IA_VGA:IDR_PURPLE_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_PURPLE_Selector_VGA:IDR_PURPLE_Selector;
		m_sSkin.uiSelectorGlass		= IDR_PURPLE_SelectorGlass_VGA;
		m_sSkin.crSelected			= RGB(106,34,139);
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_PURPLE_DETAILS_ButtonSelectVGA:IDR_PURPLE_DETAILS_ButtonSelect;
		break;
	case COLOR_Red:
	default:
		m_sSkin.uiToolBarArrow		= IsVGA()?IDR_RED_Arrow_VGA:IDR_RED_Arrow;
		m_sSkin.uiToolBarArrowLan	= IsVGA()?IDR_RED_Arrow_Lan_VGA:IDR_RED_Arrow_Lan;
		m_sSkin.uiGlow				= IsVGA()?IDR_RED_Glow_VGA:IDR_RED_Glow;
		m_sSkin.uiImageArray		= IsVGA()?IDR_RED_IA_VGA:IDR_RED_IA;
		m_sSkin.uiSelector			= IsVGA()?IDR_RED_Selector_VGA:IDR_RED_Selector;
		m_sSkin.uiSelectorGlass		= IDR_RED_SelectorGlass_VGA;
		m_sSkin.crSelected			= RGB(160,13,13);
		m_sSkin.uiDetailsSelector	= IsVGA()?IDR_RED_DETAILS_ButtonSelectVGA:IDR_RED_DETAILS_ButtonSelect;
		break;
	}

	switch(g_sOptions->eSkin)
	{
	case SKIN_Glass:
	default:
		m_sSkin.uiToolBarBack		= IsVGA()?IDR_BOT_VGA:IDR_BOT;
		m_sSkin.uiToolBarBackLan	= IsVGA()?IDR_BOT_Lan_VGA:IDR_BOT_Lan;
		m_sSkin.uiToolBarSelector	= IsVGA()?IDR_SEL_VGA:IDR_SEL;
		m_sSkin.uiToolBarSelectorLan= IsVGA()?IDR_SEL_Lan_VGA:IDR_SEL_Lan;
		break;
	}
}

void CObjGui::DrawGlow(CIssGDIEx& gdi, RECT& rcLocation)
{
	GetGlow();

	Draw(gdi,
		 rcLocation.left + (WIDTH(rcLocation) - m_gdiGlow.GetWidth())/2, rcLocation.top + (HEIGHT(rcLocation) - m_gdiGlow.GetHeight())/2,
		 m_gdiGlow.GetWidth(), m_gdiGlow.GetHeight(),
		 m_gdiGlow);
}
