#include "StdAfx.h"
#include "DlgGuitarPro.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"

#include "DlgOptions.h"
#include "DlgMsgBox.h"
#include "DlgAbout.h"

#include "GuiGuitarChords.h"
#include "GuiGuitarTuner.h"
#include "GuiMetronome.h"
#include "IssLocalisation.h"
#include "shellapi.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

#define IDT_MENU_TIMER 1000
#define MENU_GLOW_TIME 500

CDlgGuitarPro::CDlgGuitarPro(HINSTANCE hInst)
:m_guiCurrent(NULL)
,m_eMenuUp(MENUUP_None)
,m_bPanRight(FALSE)
,m_iMenuGlow(-1)
{
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    m_guiCurrent = (&m_guiGuitarChords);

	m_hInst = hInst;

    if(g_options)
        g_options->Init(m_hInst);

    LoadRegistry();
}

CDlgGuitarPro::~CDlgGuitarPro(void)
{
    OnDestroy(NULL, 0, 0);
}

void CDlgGuitarPro::LoadRegistry()
{
    DWORD dwTemp;

    //General
    if(S_OK == GetKey(REG_KEY, _T("Skin"), dwTemp))
        g_gui->SetSkin((EnumSkin)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Frets"), dwTemp))
        g_options->SetFretMarkers((EnumFretMarkers)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Left"), dwTemp))
        g_options->SetLeftHanded((BOOL)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Sound"), dwTemp))
        g_gui->SetPlaySounds((BOOL)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Tuning"), dwTemp))
        g_options->SetTuning(dwTemp, INST_Guitar);
    if(S_OK == GetKey(REG_KEY, _T("TuningB4"), dwTemp))
        g_options->SetTuning(dwTemp, INST_4StringBass);
    if(S_OK == GetKey(REG_KEY, _T("TuningLute"), dwTemp))
        g_options->SetTuning(dwTemp, INST_Lute);
    if(S_OK == GetKey(REG_KEY, _T("TuningBanjo"), dwTemp))
        g_options->SetTuning(dwTemp, INST_Banjo);
	if(S_OK == GetKey(REG_KEY, _T("TuningUkulele"), dwTemp))
		g_options->SetTuning(dwTemp, INST_Ukulele);
	if(S_OK == GetKey(REG_KEY, _T("Instrument"), dwTemp))
		g_options->GetGen()->SetInstrument(EnumInstrument(dwTemp));

    //chords
    if(S_OK == GetKey(REG_KEY, _T("Root"), dwTemp))
        g_options->SetRoot((EnumChordRoot)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Chord"), dwTemp))
        g_options->SetChordType((EnumChordType)dwTemp, TRUE);

    if(S_OK == GetKey(REG_KEY, _T("Strum"), dwTemp))
        g_options->SetStrumStyle(EnumStrum(dwTemp));



    //tuner
    if(S_OK == GetKey(REG_KEY, _T("Sample"), dwTemp))
        g_options->SetSampleFreq((EnumSampleFreqID)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Trigger"), dwTemp))
        g_options->SetTrigger((int)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("BPM"), dwTemp))
        g_options->SetMetroBPM(dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("Meter"), dwTemp))
        g_options->SetMetroMeter(dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("MetroSound"), dwTemp))
        g_options->SetMetroSound(EnumMetroSounds(dwTemp));

    
}

void CDlgGuitarPro::SaveRegistry()
{
    DWORD dwValue;

    //general
    dwValue = (DWORD)g_gui->GetSkin();
        SetKey(REG_KEY, _T("Skin"), dwValue);

    dwValue = (DWORD)g_options->GetFretMarkers();
        SetKey(REG_KEY, _T("Frets"), dwValue);

    dwValue = (DWORD)g_options->GetLeftHanded();
        SetKey(REG_KEY, _T("Left"), dwValue);

    dwValue = (DWORD)g_gui->GetPlaySounds();
        SetKey(REG_KEY, _T("Sound"), dwValue);

    //chords
    dwValue = (DWORD)g_options->GetRoot();
        SetKey(REG_KEY, _T("Root"), dwValue);

    dwValue = (DWORD)g_options->GetChordType();
        SetKey(REG_KEY, _T("Chord"), dwValue);

    dwValue = (DWORD)g_options->GetStrumStyle();
        SetKey(REG_KEY, _T("Strum"), dwValue);



    dwValue = (DWORD)g_options->GetTuning(INST_Guitar);
        SetKey(REG_KEY, _T("Tuning"), dwValue);
    dwValue = (DWORD)g_options->GetTuning(INST_4StringBass);
        SetKey(REG_KEY, _T("TuningB4"), dwValue);
    dwValue = (DWORD)g_options->GetTuning(INST_Lute);
        SetKey(REG_KEY, _T("TuningLute"), dwValue);
    dwValue = (DWORD)g_options->GetTuning(INST_Banjo);
        SetKey(REG_KEY, _T("TuningBanjo"), dwValue);
	dwValue = (DWORD)g_options->GetTuning(INST_Ukulele);
		SetKey(REG_KEY, _T("TuningUkulele"), dwValue);
   /* dwValue = (DWORD)g_options->GetTuning();
        SetKey(REG_KEY, _T("TuningB5"), dwValue);
    dwValue = (DWORD)g_options->GetTuning();
        SetKey(REG_KEY, _T("TuningBanjo"), dwValue);
    dwValue = (DWORD)g_options->GetTuning();
        SetKey(REG_KEY, _T("TuningLute"), dwValue);*/

    dwValue = (DWORD)g_options->GetInstrument();
        SetKey(REG_KEY, _T("Instrument"), dwValue);

    //tuner
    dwValue = (DWORD)g_options->GetSampleFreq();
        SetKey(REG_KEY, _T("Sample"), dwValue);

    dwValue = (DWORD)g_options->GetiTrigger();
        SetKey(REG_KEY, _T("Trigger"), dwValue);

    //metronome
    dwValue = (DWORD)g_options->GetMetroBPM();
        SetKey(REG_KEY, _T("BPM"), dwValue);

    dwValue = (DWORD)g_options->GetMetroMeter();
        SetKey(REG_KEY, _T("Meter"), dwValue);

    dwValue = (DWORD)g_options->GetMetroSound();
        SetKey(REG_KEY, _T("MetroSound"), dwValue);   

}

BOOL CDlgGuitarPro::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(GetForegroundWindow() != hWnd)
        return FALSE;


    //lets check the orientation just in case
  /*  DEVMODE devmode = {0};
    devmode.dmSize = sizeof(DEVMODE);

    devmode.dmFields = DM_DISPLAYORIENTATION;
    ChangeDisplaySettingsEx(NULL, &devmode, 0, CDS_TEST, NULL);

    if(devmode.dmDisplayOrientation != DMDO_DEFAULT && 
        GetSystemMetrics(SM_CXSCREEN) != GetSystemMetrics(SM_CYSCREEN) )
    {
        memset(&devmode, 0, sizeof(DEVMODE));
        devmode.dmSize = sizeof(DEVMODE);
        devmode.dmDisplayOrientation = DMDO_DEFAULT; 
        devmode.dmFields = DM_DISPLAYORIENTATION;
        ChangeDisplaySettingsEx(NULL, &devmode, hWnd, 0, NULL);

        return TRUE;
    }*/

    return UNHANDLED;
}

BOOL CDlgGuitarPro::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	SetWindowSize();

	GetClientRect(m_hWnd, &rcClient);

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

    g_gui->OnSize(hWnd, rcClient);
	HDC dc = GetDC(hWnd);

	ReleaseDC(hWnd, dc);

	// start the wait cursor because this could take a few seconds
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE); 


    //really shouldn't have too 
    m_guiGuitarChords.OnSize(hWnd, wParam, lParam);
    m_guiGuitarTuner.OnSize(hWnd, wParam, lParam);
	m_guiMetronome.OnSize(hWnd, wParam, lParam);
    m_guiPlay.OnSize(hWnd, wParam, lParam);
    m_guiTunerEar.OnSize(hWnd, wParam, lParam);
    m_guiPlayFav.OnSize(hWnd, wParam, lParam);

    //set the menu rects
    int iMenuWidth = WIDTH(rcClient)/4;

    for(int i = 0; i < 4; i++)
    {
        m_rcMenu[i].left = rcClient.left + i*iMenuWidth;
        m_rcMenu[i].right = rcClient.left + (i+1)*iMenuWidth;
        m_rcMenu[i].bottom = rcClient.bottom;
        m_rcMenu[i].top = rcClient.bottom - MENU_HEIGHT;

        m_rcMenuUp[i] = m_rcMenu[i];
        m_rcMenuUp[i].bottom = m_rcMenu[i].top;
        m_rcMenuUp[i].top = m_rcMenuUp[i].bottom - MENU_HEIGHT;

    }

    m_rcMenuUpFull = m_rcMenuUp[0];
    m_rcMenuUpFull.right = m_rcMenuUp[3].right;

    m_rcBottomBar = m_rcMenu[0];
    m_rcBottomBar.right = m_rcMenu[3].right;


	// End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

	return TRUE;
}



void CDlgGuitarPro::SetWindowSize()
{

#ifdef UNDER_CE
	OnActivate(m_hWndDlg, (WPARAM)WA_ACTIVE, NULL);
	return;
#else

	POINT pt;
	DWORD dwTemp;

	if(S_OK == GetKey(REG_KEY, _T("XPos"), dwTemp))
		pt.x = dwTemp;
	else
		pt.x = 20;
	if(S_OK == GetKey(REG_KEY, _T("YPos"), dwTemp))
		pt.y = dwTemp;
	else 
		pt.y = 20;

	SetWindowPos(m_hWnd, NULL, pt.x, pt.y, 480, 640+30, SWP_NOREDRAW);
//	InitMenu();
#endif
}


BOOL CDlgGuitarPro::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif
    //let's get iSSLocalisation going
    m_oStr->SetResourceInstance(m_hInst);
    g_cLocale.Init(REG_KEY, m_hInst, SUPPORT_English|SUPPORT_German|SUPPORT_Spanish|SUPPORT_Italian|SUPPORT_French|SUPPORT_Dutch|SUPPORT_Japanese, TRUE);

    g_gui->Init(hWnd, m_hInst);

    m_guiGuitarChords.Init(hWnd, m_hInst);
    m_guiGuitarTuner.Init(hWnd, m_hInst);
    m_guiMetronome.Init(hWnd, m_hInst);
    m_guiPlay.Init(hWnd, m_hInst);
    m_guiTunerEar.Init(hWnd, m_hInst);
    m_guiPlayFav.Init(hWnd, m_hInst);


#ifndef UNDER_CE
/*	memset(&m_notifyData, 0, sizeof( NOTIFYICONDATA )) ;
	m_notifyData.cbSize = sizeof(NOTIFYICONDATA);
	m_notifyData.hWnd = hWnd;
	m_notifyData.uID = ID_TRAY_APP_ICON;
	m_notifyData.uFlags = NIF_ICON | NIF_MESSAGE | 	NIF_TIP;
	m_notifyData.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.
	//m_notifyData.hIcon = (HICON)LoadImage( NULL, _T("calcpro.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE  ) ;
	m_notifyData.hIcon = (HICON)LoadImage( m_hInst, MAKEINTRESOURCE(IDI_CALCPRO), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR  ) ;
	m_oStr->StringCopy(m_notifyData.szTip, _T("Panoramic Calc Pro\r\nwww.panoramicsoft.com"));*/

	//HCURSOR hCursor = ::SetCursor(LoadCursor(NULL, IDC_ARROW));

	POINT pt;
	DWORD dwTemp;

	if(S_OK == GetKey(REG_KEY, _T("XPos"), dwTemp))
		pt.x = dwTemp;
	else
		pt.x = 20;
	if(S_OK == GetKey(REG_KEY, _T("YPos"), dwTemp))
		pt.y = dwTemp;
	else 
		pt.y = 20;

	SetWindowPos(hWnd, NULL, pt.x, pt.y, 480, 640, SWP_SHOWWINDOW);
#endif

    return TRUE;
}

void CDlgGuitarPro::LoadImages(HWND hWndSplash, int iMaxPercent)
{
}

BOOL CDlgGuitarPro::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	// draw the background
/*	g_gui->DrawBackground(rcClip);
    g_gui->DrawNut(rcClip);
    g_gui->DrawStrings(rcClip, 0);
    g_gui->DrawToScreen(rcClip, hDC);*/

    if(m_guiCurrent)
        m_guiCurrent->OnPaint(hWnd, hDC, rcClient);

    //draw the menu text
    DrawMenu(*g_gui->GetGDI(), rcClip);
    DrawText(g_gui->GetGDI()->GetDC(), rcClip);
    DrawMenuExtras(*g_gui->GetGDI(), rcClip);
    

  //  g_gui->DrawToScreen(rcClip, hDC);

    if(m_bPanRight)
        PanRight();

    BitBlt(hDC,
        rcClip.left, rcClip.top, 
        rcClip.right - rcClip.left,
        rcClip.bottom - rcClip.top,
        g_gui->GetGDI()->GetDC(),
        rcClip.left,
        rcClip.top,
        SRCCOPY);

	return TRUE;
}

BOOL CDlgGuitarPro::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case _T('1')://pile 1
	case _T('2')://pile 2
	case _T('3')://pile 3
	case _T('4')://pile 4
        g_options->SetInstrument(INST_4StringBass);
        break;
	case _T('5')://pile 5
//        g_options->SetInstrument(INST_5StringBass);
        break;
	case _T('6')://pile 6
        g_options->SetInstrument(INST_Guitar);
        break;
	case _T('7')://pile 7
        g_options->SetInstrument(INST_Lute);
		break;
	case _T('8'):
		g_options->SetInstrument(INST_Ukulele);
		break;
    case _T('9'):
        g_options->SetInstrument(INST_Banjo);
        break;
    case _T('o'):
    case _T('O'):
        PostMessage(hWnd, WM_COMMAND, IDMENU_Options, 0);
        break;
	case _T('d'):
		break;
	case _T('N'):
	case _T('n'):
		break;
	case _T('p'):
		break;
	case _T('0'):
	case _T('u'):
		break;
	case _T('r'):
	case _T('R'):
        InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case _T('c'):
		break;
	case _T('e'):
        break;
    case _T('q'):
		break;
    case _T('f'):
    case _T('F'):
#ifdef DEBUG
        OnMenuPlayFav();
#endif
        break;


	default:
		return FALSE;
	    break;
	}
	//drop that sip
//	SHSipPreference(m_hWnd, SIP_DOWN);
	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;

}

BOOL CDlgGuitarPro::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_guiCurrent && m_guiCurrent->OnKeyDown(hWnd, wParam, lParam) == TRUE)
        return TRUE;

	switch(wParam)
	{
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		break;
	}
	return TRUE;
}

BOOL CDlgGuitarPro::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    g_gui->OnKeyUp(hWnd, wParam, lParam);

    if(m_guiCurrent && m_guiCurrent->OnKeyUp(hWnd, wParam, lParam) == TRUE)
        return TRUE;
    
    switch(LOWORD(wParam))
	{
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		break;
	case VK_RIGHT:
      /*  if(m_guiCurrent)
        {
            EnumGUI eGui = m_guiCurrent->GetGUI();
            eGui = EnumGUI((int)eGui + 1);
            if(eGui >= GUI_Count)
                eGui = GUI_Chords;
            SetCurrentGui(eGui);
        }*/
		break;
	case VK_LEFT:
		break;
	case VK_UP:
	case VK_DOWN:
		
        break;
/*    case VK_TSOFT2:
        OnMenuMenu();
        break;
	case VK_F4:
#if !defined(WIN32_PLATFORM_WFSP) && !defined(WIN32_PLATFORM_PSPC)
		FxEndDialog(m_hWnd, TRUE);	
#endif
		break;*/
	}
	return TRUE;
}

BOOL CDlgGuitarPro::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_MENU_TIMER)
    {
        KillTimer(hWnd, IDT_MENU_TIMER);
        if(m_iMenuGlow > 0 && m_iMenuGlow < 4)
            InvalidateRect(hWnd, &m_rcMenu[m_iMenuGlow], FALSE);
        m_iMenuGlow = -1;
        UpdateWindow(hWnd);
    }

    if(m_guiCurrent)
        return m_guiCurrent->OnTimer(hWnd, wParam, lParam);
    return UNHANDLED;
}

BOOL CDlgGuitarPro::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_guiCurrent->OnLostFocus();
    SaveRegistry();
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

BOOL CDlgGuitarPro::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
    switch(LOWORD(wParam))
	{
//	case IDMENU_Menu:
    case IDMENU_Left:
		OnMenuMenu();
		break;
    case IDMENU_Right:
		break;
	case IDMENU_MoreApps:
		OnMenuMoreApps();
		break;
	case IDMENU_Quit:
    case IDMENU_Exit:
        {   
         //   CDlgMsgBox dlgMsg;
         //   if(IDYES == dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
              PostQuitMessage(0);
        }
        break;
    case IDMENU_Options:
		OnOptions();

        break;
    case IDMENU_Help:
        //CreateProcess(_T("peghelp"), _T("PanoGuitarSuite.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        LaunchHelp();
        break;
    case IDMENU_About:
		OnAbout();

        break;

	default:
        if(m_guiCurrent->OnCommand(hWnd, wParam, lParam) == TRUE)
            return TRUE;
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgGuitarPro::OnAbout()
{
	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	CDlgAbout dlgAbout;
	dlgAbout.m_hWndParent = m_hWnd;

	if(dlgAbout.Create(m_oStr->GetText(ID(IDS_MENU_Options), m_hInst), NULL, m_hInst, m_oStr->GetText(ID(IDS_MENU_Options), m_hInst),
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		0,
		DS_MODALFRAME | WS_CAPTION | WS_SYSMENU, NULL, IDI_GUITARPRO))
	{
		MoveWindow(dlgAbout.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), TRUE);
		ShowWindow(dlgAbout.GetWnd(), SW_SHOW);
		ShowWindow(m_hWnd, SW_HIDE);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}

		ShowWindow(m_hWnd, SW_SHOW);
	}
	return TRUE;
}

BOOL CDlgGuitarPro::OnOptions()
{
	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	CDlgOptions dlgOptions;
	dlgOptions.m_hWndParent = m_hWnd;
	BOOL bLeft = g_options->GetLeftHanded();

	DWORD idLang = g_cLocale.GetCurrentLanguage();

	if(dlgOptions.Create(m_oStr->GetText(ID(IDS_MENU_Options), m_hInst), NULL, m_hInst, m_oStr->GetText(ID(IDS_MENU_Options), m_hInst),
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		0,
		DS_MODALFRAME | WS_CAPTION | WS_SYSMENU, NULL, IDI_GUITARPRO))
	{
		MoveWindow(dlgOptions.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), TRUE);
		ShowWindow(dlgOptions.GetWnd(), SW_SHOW);
		ShowWindow(m_hWnd, SW_HIDE);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}

		ShowWindow(m_hWnd, SW_SHOW);
	}





//	dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

	if(bLeft != g_options->GetLeftHanded())
		g_gui->GetBackground(TRUE);

	if(idLang != g_cLocale.GetCurrentLanguage())
	{
		m_guiGuitarTuner.ResetGDI();
		m_guiGuitarChords.ResetGDI();
		m_guiMetronome.ResetGDI();
		m_guiPlay.ResetGDI();
		m_guiPlayFav.ResetGDI();
		m_guiTunerEar.ResetGDI();
	}

	m_bPanRight = TRUE;
	SaveRegistry(); //people just prefer it

	return TRUE;
}

BOOL CDlgGuitarPro::DrawText(HDC hDC, RECT& rcClip)
{
    if(rcClip.bottom < m_rcMenu[0].top)
        return FALSE;

    static HFONT hFont;

    if(g_cLocale.GetCurrentLanguage() == LANG_ENGLISH)
        hFont = g_gui->GetFont(FONT_Menu);
    else
        hFont = g_gui->GetFont(FONT_MenuSmall);
    
    if(m_guiCurrent->GetGUI() == GUI_Play)
        ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Play), m_hInst), m_rcMenu[0], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);
    else
        ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Chords), m_hInst), m_rcMenu[0], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);

    if(m_guiCurrent->GetGUI() == GUI_TunerEar)
        ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_TuneByEar), m_hInst), m_rcMenu[1], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);
    else
        ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Tuner), m_hInst), m_rcMenu[1], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);
    ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Metronome), m_hInst), m_rcMenu[2], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);
    ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Menu), m_hInst), m_rcMenu[3], DT_CENTER | DT_BOTTOM, hFont, MENU_TEXT_COLOR);
	return TRUE;
}

BOOL CDlgGuitarPro::DrawMenuExtras(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcBottomBar) == FALSE)
        return TRUE;

    POINT ptLocation;
    POINT ptArrow;

    int iIndent = IsVGA()?12:6; 

    switch(m_guiCurrent->GetGUI())
    {
    case GUI_Chords:
    case GUI_Play:
        ptLocation.x = m_rcMenu[0].right - g_gui->GetMenuCheck()->GetWidth() - iIndent;
        ptLocation.y = m_rcMenu[0].top + iIndent;

        ptArrow.x = m_rcMenu[0].left + iIndent;
        ptArrow.y = m_rcMenu[0].top + iIndent;

        break;
    case GUI_Tuner:
    case GUI_TunerEar:
        ptLocation.x = m_rcMenu[1].right - g_gui->GetMenuCheck()->GetWidth() - iIndent;
        ptLocation.y = m_rcMenu[1].top + iIndent;

        ptArrow.x = m_rcMenu[1].left + iIndent;
        ptArrow.y = m_rcMenu[1].top + iIndent;

        break;
    case GUI_Metronome:
        ptLocation.x = m_rcMenu[2].right - g_gui->GetMenuCheck()->GetWidth() - iIndent;
        ptLocation.y = m_rcMenu[2].top + iIndent;

        ptArrow.x =  -1;
        ptArrow.y = -1;
  
    	break;
    default:
        return FALSE;
        break;
    }

    ::Draw(gdi, ptLocation.x, ptLocation.y, g_gui->GetMenuCheck()->GetWidth(), g_gui->GetMenuCheck()->GetHeight(), *g_gui->GetMenuCheck());

    if(ptArrow.x != -1)
        ::Draw(gdi, ptArrow.x, ptArrow.y, g_gui->GetMenuArrow()->GetWidth(), g_gui->GetMenuArrow()->GetHeight(), *g_gui->GetMenuArrow());

    return TRUE;
}


BOOL CDlgGuitarPro::DrawMenu(CIssGDIEx& gdi, RECT& rcClip, int iHLIndex)
{
    static RECT rc;
    
    if(IsRectInRect(rcClip, m_rcBottomBar) == FALSE)
        return TRUE;

    IntersectRect(&rc, &rcClip, &m_rcBottomBar);

    CIssGDIEx* gMenu = g_gui->GetMenuIcons();

   /*  Draw(gdi,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        *g_gui->GetMenu(),
        rc.left, rc.top - m_rcBottomBar.top);*/

    //and the icons
    //align top/center
    POINT ptLoc;
    int iMW = WIDTH(m_rcBottomBar)/4; //width of each menu button .. ie 1/4 of the whole thing
    int iIW = gMenu->GetWidth()/(MICON_Count); //width of the individual icon images in the array
    ptLoc.y = m_rcBottomBar.top;

    ptLoc.x = (iMW - iIW)/2;
    if(m_guiCurrent->GetGUI() == GUI_Play)
        Draw(gdi, ptLoc.x, ptLoc.y, iIW, gMenu->GetHeight(), *gMenu, iIW*MICON_Play, 0);
    else
        Draw(gdi, ptLoc.x, ptLoc.y, iIW, gMenu->GetHeight(), *gMenu, iIW*MICON_Chords, 0);

    ptLoc.x += iMW;//so just move along the width of the menu button
    Draw(gdi, ptLoc.x, ptLoc.y, iIW, gMenu->GetHeight(), *gMenu, iIW*MICON_Tuner, 0);

    ptLoc.x += iMW;
    Draw(gdi, ptLoc.x, ptLoc.y, iIW, gMenu->GetHeight(), *gMenu, iIW*MICON_Metronome, 0);

    ptLoc.x += iMW;
    Draw(gdi, ptLoc.x, ptLoc.y, iIW, gMenu->GetHeight(), *gMenu, iIW*MICON_Menu, 0);

    //and finally ... the dividers
    int iSpacing = WIDTH(m_rcBottomBar)/4;
    int iIndent = HEIGHT(m_rcBottomBar)/5;

    POINT ptS, ptE;

    for(int i = 0; i < 3; i++)
    {
        ptS.x = iSpacing + i*iSpacing-1;
        ptS.y = m_rcBottomBar.top + iIndent;
        ptE.x = iSpacing + i*iSpacing-1; 
        ptE.y = m_rcBottomBar.bottom - iIndent;

        Line(gdi.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);

        ptE.x++;
        ptS.x++;

        Line(gdi.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);
    }


    return TRUE;
}


BOOL CDlgGuitarPro::DrawMenuUp(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(m_rcMenuUpFull, rcClip) == FALSE)
        return FALSE;

    switch(m_eMenuUp)
    {
    case MENUUP_Chords:
        AlphaFillRect(gdi, m_rcMenuUpFull, 0, 200);
        ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_Chords), m_hInst), m_rcMenuUp[0], DT_CENTER | DT_BOTTOM, g_gui->GetFont(FONT_Menu), MENU_TEXT_COLOR);
        ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_PlayMode), m_hInst), m_rcMenuUp[1], DT_CENTER | DT_BOTTOM, g_gui->GetFont(FONT_Menu), MENU_TEXT_COLOR);
    	break;
     case MENUUP_Tuner:   
        AlphaFillRect(gdi, m_rcMenuUpFull, 0, 200);
        ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_Digital), m_hInst), m_rcMenuUp[0], DT_CENTER | DT_BOTTOM, g_gui->GetFont(FONT_Menu), MENU_TEXT_COLOR);
        ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_TuneByEar), m_hInst), m_rcMenuUp[1], DT_CENTER | DT_BOTTOM, g_gui->GetFont(FONT_Menu), MENU_TEXT_COLOR);
        break;
    default:
        //AlphaFillRect(gdi, m_rcMenuUpFull, 0, 200);
        return FALSE;
        break;
    }

    return TRUE;
}


BOOL CDlgGuitarPro::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnUser(m_hWnd, uiMessage, wParam, lParam);
	return TRUE;
}


BOOL CDlgGuitarPro::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_guiCurrent)
        return m_guiCurrent->OnMouseMove(m_hWnd, pt);
    return FALSE;
}


BOOL CDlgGuitarPro::OnLButtonDown(HWND hWnd, POINT& pt)
{
//  g_gui->OnLButtonDown(hWnd, pt);
    for(int i = 0; i < 4; i++)
    {
        if(PtInRect(&m_rcMenu[i], pt))
        {
            int iX = m_rcMenu[i].left + (WIDTH(m_rcMenu[i]) - g_gui->GetGlow()->GetWidth())/2;

            ::Draw(*g_gui->GetGDI(), iX, m_rcMenu[i].top, 
                g_gui->GetGlow()->GetWidth(), g_gui->GetGlow()->GetHeight(),
                *g_gui->GetGlow());

            HDC dc = GetDC(hWnd);
            BitBlt(dc, m_rcMenu[i].left, m_rcMenu[i].top, WIDTH(m_rcMenu[i]), HEIGHT(m_rcMenu[i]),
                g_gui->GetGDI()->GetDC(), m_rcMenu[i].left, m_rcMenu[i].top, SRCCOPY);
            ReleaseDC(hWnd, dc);
        }
    }

    if(m_guiCurrent)
        return m_guiCurrent->OnLButtonDown(hWnd, pt);

    return UNHANDLED;
}

BOOL CDlgGuitarPro::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcMenu[0], pt))
    {
        if(m_guiCurrent->GetGUI() != GUI_Chords)
            OnMenuChordSel();
        else
            OnMenuPlay();
        return TRUE;
    }
    else if(PtInRect(&m_rcMenu[1], pt))
    {
        if(m_guiCurrent->GetGUI() != GUI_Tuner)     
            OnMenuTunerDigital();
        else
            OnMenuTunerEar();
    }
    else if(PtInRect(&m_rcMenu[2], pt))
        OnMenuMetronome();
    else if(PtInRect(&m_rcMenu[3], pt))
        OnMenuMenu();
    else if(m_guiCurrent)
        return m_guiCurrent->OnLButtonUp(hWnd, pt);
    return TRUE;
}

BOOL CDlgGuitarPro::ShouldAbort()
{
    MSG msg;
    
    Sleep(1);

    if(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
    {
        switch(LOWORD(msg.message))
        {
        //	case WM_KEYUP:
        case WM_CHAR:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_COMMAND:
        case WM_USER:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_ACTIVATE:
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            return TRUE;
        }
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
    }
	return FALSE;
}

BOOL CDlgGuitarPro::OnMenuMenu()
{
    g_gui->m_wndMenu.ResetContent();

    g_gui->m_wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
#ifndef NMA
//	g_gui->m_wndMenu.AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif
    g_gui->m_wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    g_gui->m_wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    g_gui->m_wndMenu.AddSeparator();
    g_gui->m_wndMenu.AddItem(ID(IDS_MENU_Exit), m_hInst, IDMENU_Exit);

    g_gui->m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    //align right
    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.left+WIDTH(rcClient)/4,rcClient.top,
        WIDTH(rcClient)*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        rcClient.left,0,0,0, ADJUST_Bottom);

    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuPlayFav()
{
    SetCurrentGui(GUI_PlayFav);
    InvalidateRect(m_hWnd, &m_rcMenuUpFull, FALSE);
    return TRUE;
}


BOOL CDlgGuitarPro::OnMenuChords()
{
    if(m_eMenuUp == MENUUP_Chords)
    {
        m_eMenuUp = MENUUP_None;
    }
    else if(m_guiCurrent && 
        (m_guiCurrent->GetGUI() == GUI_Play || m_guiCurrent->GetGUI() == GUI_Chords) )
    {
        m_eMenuUp = MENUUP_Chords;
    }
    else
    {
        SetCurrentGui(GUI_Chords);
    }
    InvalidateRect(m_hWnd, &m_rcMenuUpFull, FALSE);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuChordSel()
{
    SetCurrentGui(GUI_Chords);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuPlay()
{
    SetCurrentGui(GUI_Play);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuTuner()
{
    if(m_eMenuUp == MENUUP_Tuner)
    {
        m_eMenuUp = MENUUP_None;
    }
    else if(m_guiCurrent && 
        (m_guiCurrent->GetGUI() == GUI_Tuner || m_guiCurrent->GetGUI() == GUI_TunerEar) )
    {
        m_eMenuUp = MENUUP_Tuner;
    }
    else
    {
        SetCurrentGui(GUI_Tuner);
    }
    InvalidateRect(m_hWnd, &m_rcMenuUpFull, FALSE);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuTunerDigital()
{
    SetCurrentGui(GUI_Tuner);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuTunerEar()
{
    SetCurrentGui(GUI_TunerEar);
    return TRUE;
}

BOOL CDlgGuitarPro::OnMenuMetronome()
{
    SetCurrentGui(GUI_Metronome);
    return TRUE;
}




BOOL CDlgGuitarPro::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

        //quick hack for orientation
        OnSettingChange(hWnd, 0, 0);
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {

    }
#endif
    return bRet;
}

void CDlgGuitarPro::SetCurrentGui(EnumGUI eGui)
{
    if(m_guiCurrent)
        m_guiCurrent->OnLostFocus();

    switch(eGui)
    {
    case GUI_Chords:
        m_guiCurrent = &m_guiGuitarChords;
    	break;
    case GUI_Tuner:
        m_guiCurrent = &m_guiGuitarTuner;
    	break;
    case GUI_TunerEar:
        m_guiCurrent = &m_guiTunerEar;
        break;
    case GUI_Metronome:
        m_guiCurrent = &m_guiMetronome;
        break;
    case GUI_PlayFav:
        m_guiCurrent = &m_guiPlayFav;
        break;
    case GUI_Play:
    default:
        m_guiCurrent = &m_guiPlay;
        break;
    }

    if(m_guiCurrent)
        m_guiCurrent->OnSetFocus();

    m_eMenuUp = MENUUP_None;
    InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgGuitarPro::PanRight()
{
    m_bPanRight = FALSE;
    HDC hDC = GetDC(m_hWnd);
    SlideRight(hDC, *g_gui->GetGDI(), 500);
    ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgGuitarPro::OnMenuMoreApps()
{

	return TRUE;
}


void CDlgGuitarPro::LaunchHelp()
{
    TCHAR szHelpFile[STRING_MAX];

    m_oStr->Empty(szHelpFile);

    switch(g_cLocale.GetCurrentLanguage())
    {
    case LANG_FRENCH:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_fr.htm"));
        break;
    case LANG_GERMAN:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_de.htm"));
        break;
    case LANG_SPANISH:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_es.htm"));
        break;
    case LANG_JAPANESE:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_jp.htm"));
        break;
    case LANG_DUTCH:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_du.htm"));
        break;
    case LANG_KOREAN:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_ko.htm"));
        break;
    case LANG_ITALIAN:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_it.htm"));
        break;
    case LANG_PORTUGUESE:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_pt.htm"));
        break;
    case LANG_RUSSIAN:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite_ru.htm"));
        break;
    default:
        m_oStr->StringCopy(szHelpFile, _T("PanoGuitarSuite.htm"));
        break;
    }
#ifdef UNDER_CE
    if(IsSmartphone())
#else
    if(TRUE)//for PC
#endif
    {
        SHELLEXECUTEINFO shell;
        shell.cbSize		= sizeof(SHELLEXECUTEINFO);
        shell.lpVerb		= _T("OPEN"); 
        shell.fMask			= SEE_MASK_NOCLOSEPROCESS ;
        shell.hwnd			= NULL;
        shell.lpFile		= _T("iexplore.exe");
        shell.lpDirectory	= NULL;
        shell.hInstApp		= m_hInst;
        shell.nShow			= SW_SHOW;

        TCHAR szTemp[MAX_PATH];
        GetExeDirectory(szTemp);

        m_oStr->Concatenate(szTemp, szHelpFile);

       // m_oStr->Insert(szHelpFile, _T("\\Windows\\"));
       // shell.lpParameters	= szHelpFile; 
		shell.lpParameters = szTemp;
        ShellExecuteEx(&shell);

        return;
    }
    else
    {
        CreateProcess(_T("peghelp"), szHelpFile, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    }

}

BOOL CDlgGuitarPro::OnMCNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	mciSendCommand(wParam, MCI_CLOSE, 0, NULL);


	return UNHANDLED;
}