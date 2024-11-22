#include "StdAfx.h"
#include "DlgSysInfo.h"
#include "resource.h"
#include "IssRegistry.h"
#include "Commdlg.h"
#include "Winuser.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"

#include "WndSystem.h"
#include "WndSystemMetrics.h"
#include "WndCPU.h"
#include "WndMemory.h"
#include "WndStorage.h"
#include "WndNetworking.h"
#include "WndDisplay.h"
#include "WndCommPorts.h"
#include "WndTime.h"
#include "WndPower.h"
#include "WndBenchmark.h"
#include "WndTest.h"
#include "WndAudio.h"
#include "WndGPS.h"

#define TEXT_COLOR			0xFFFFFF
#define SysInfo_TEXT_SIZE	GetSystemMetrics(SM_CXICON)*2/5/*/2*/
#define OTHER_TEXT_SIZE		GetSystemMetrics(SM_CXICON)*3/5
#define HEIGHT_Text         GetSystemMetrics(SM_CXICON)*4/9

#define IDMENU_Help			518
#define IDMENU_About		519
#define IDMENU_Exit			520

#define IDT_Timer			100
#define IDT_Timer_Hourly	101
#define IDT_Time_Alarm		102

CDlgSysInfo* g_dlgSysInfo = NULL;

CDlgSysInfo::CDlgSysInfo(void)
:m_hFontText(NULL)
,m_hFontTime(NULL)
,m_iHiScore(0)
{
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    DWORD dwTemp;

    if(S_OK == GetKey(REG_KEY, _T("BestScore"), dwTemp))
        m_iHiScore = (BOOL)dwTemp;

    m_hFontBtnLabel = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
}

CDlgSysInfo::~CDlgSysInfo(void)
{
	SetKey(REG_KEY, _T("BestScore"), (DWORD)m_iHiScore);
}

BOOL CDlgSysInfo::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iIndent = GetSystemMetrics(SM_CXICON)/3;

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

	m_gdiMem.Destroy();

	m_rcTime.left	= 0;
	m_rcTime.top	= iIndent/4;
	m_rcTime.right	= rcClient.right;
	m_rcTime.bottom	= m_rcTime.top + SysInfo_TEXT_SIZE;

	m_rcDate		= m_rcTime;
	m_rcDate.top	= m_rcTime.bottom;
	m_rcDate.bottom	= m_rcDate.top + OTHER_TEXT_SIZE;

	//lets leave a little room

	m_rcAlarm1Label.top		= m_rcDate.bottom + iIndent;
	m_rcAlarm1Label.left	= iIndent;
	m_rcAlarm1Label.right	= rcClient.right;
	m_rcAlarm1Label.bottom	= m_rcAlarm1Label.top + OTHER_TEXT_SIZE;

	return TRUE;
}


BOOL CDlgSysInfo::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_SysInfo;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
		ASSERT(0);
#endif

    m_hFontTime = CIssGDIEx::CreateFont(SysInfo_TEXT_SIZE, FW_BOLD, TRUE);
	m_hFontText = CIssGDIEx::CreateFont(OTHER_TEXT_SIZE, FW_NORMAL, TRUE);

	GetTime();
	SetTimer(m_hWnd, IDT_Timer, 1000, NULL);
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}


BOOL CDlgSysInfo::DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip)
{
    if(m_gdiBack.GetDC() == NULL || WIDTH(rcClient) != m_gdiBack.GetWidth() || HEIGHT(rcClient) != m_gdiBack.GetHeight())
    {
        m_gdiBack.Create(gdiMem.GetDC(), rcClient, FALSE, TRUE);

        GradientFillRect(m_gdiBack, rcClient, RGB(70,70,70), 0, FALSE);

        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IDR_PNG_Background, m_hWnd, m_hInst, TRUE);

        CIssGDIEx gdiScale;
        SIZE szSize;

        if(gdiTemp.GetWidth() > WIDTH(rcClient) || 
            gdiTemp.GetHeight() > HEIGHT(rcClient))
        {
            szSize.cx = gdiTemp.GetWidth()/2;
            szSize.cy = gdiTemp.GetHeight()/2;

            ScaleImage(gdiTemp, gdiScale, szSize, FALSE, 0);

            //centered        
            // int iX = WIDTH(rcClient)/2 - szSize.cx/2;
            // int iY = HEIGHT(rcClient)/2 - szSize.cy/2;

            //bottom right
            int iX = WIDTH(rcClient) - szSize.cx;
            int iY = HEIGHT(rcClient) - szSize.cy - GetSystemMetrics(SM_CXSMICON);

            Draw(m_gdiBack, iX, iY, szSize.cx, szSize.cy, gdiScale);
        }
        else
        {
            //centered
            //int iX = WIDTH(rcClient)/2 - gdiTemp.GetWidth()/2;
            //int iY = HEIGHT(rcClient)/2 - gdiTemp.GetHeight()/2;
            //bottom right
            int iX = WIDTH(rcClient) - gdiTemp.GetWidth();
            int iY = HEIGHT(rcClient) - gdiTemp.GetHeight() - GetSystemMetrics(SM_CXSMICON);

            Draw(m_gdiBack, iX, iY, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp);
        }
    }
    Draw(gdiMem, rcClip, m_gdiBack, rcClip.left, rcClip.top);

    return TRUE;
}

BOOL CDlgSysInfo::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || m_gdiMem.GetWidth() != WIDTH(rcClient) || m_gdiMem.GetHeight() != HEIGHT(rcClient))
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    DrawBackground(m_gdiMem, rcClient, rcClip);

    DrawText(m_gdiMem.GetDC(), _T("Mobile System Info 0.8"), m_rcAlarm1Label,	DT_CENTER | DT_TOP, m_hFontBtnLabel, TEXT_COLOR);

    RECT rcTemp = rcClient;
    rcTemp.right -= GetSystemMetrics(SM_CXSMICON)/3;
    rcTemp.top += GetSystemMetrics(SM_CXSMICON)/3;
    DrawText(m_gdiMem.GetDC(), m_szTime, rcTemp, DT_RIGHT | DT_TOP, m_hFontTime, 0xFFFFFF);

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;



/*	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);

	FillRect(m_gdiMem.GetDC(), rcClient, 0xFFFFFF);

    DrawText(m_gdiMem.GetDC(), m_szTime, rcClient, DT_RIGHT | DT_BOTTOM, m_hFontText, TEXT_COLOR);
//	DrawText(m_gdiMem.GetDC(), m_szDate, m_rcDate, DT_CENTER | DT_TOP, m_hFontText, TEXT_COLOR);

    DrawText(m_gdiMem.GetDC(), _T("Mobile Benchmark"), m_rcAlarm1Label,	DT_CENTER | DT_TOP, m_hFontText, TEXT_COLOR);

    TCHAR szTemp[STRING_NORMAL];
    m_oStr->Format(szTemp, _T("Best Score: %i"), m_iHiScore);

    RECT rcTemp = m_rcAlarm1Label;

    OffsetRect(&rcTemp, 0, GetSystemMetrics(SM_CXICON));

    DrawText(m_gdiMem.GetDC(), szTemp, rcTemp,	DT_CENTER | DT_TOP, m_hFontText, 0x0000DD);


	BitBlt(hDC, rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiMem.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	return FALSE;*/
}

BOOL CDlgSysInfo::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	return FALSE;
}


BOOL CDlgSysInfo::OnLButtonDown(HWND hWnd, POINT& pt)
{
	return FALSE;
}

BOOL CDlgSysInfo::OnLButtonUp(HWND hWnd, POINT& pt)
{
	return FALSE;
}

BOOL CDlgSysInfo::OnMouseMove(HWND hWnd, POINT& pt)
{
	return FALSE;
}
BOOL CDlgSysInfo::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL CDlgSysInfo::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL CDlgSysInfo::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_Timer)
	{
		KillTimer(m_hWnd, IDT_Timer);
		GetTime();
		InvalidateRect(hWnd, NULL, FALSE);
		SetTimer(m_hWnd, IDT_Timer, GetTickCount()%1000, NULL);
	}

	return FALSE;
}
BOOL CDlgSysInfo::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}
BOOL CDlgSysInfo::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Power:
		OnMenuPower();
		break;
	case IDMENU_Time:
		OnMenuTime();
		break;
	case IDMENU_Memory:
		OnMenuMemory();
		break;
	case IDMENU_Storage:
		OnMenuStorage();
		break;
	case IDMENU_CPU:
		OnMenuCPU();
		break;
	case IDMENU_System:
		OnMenuSystem();
		break;
    case IDMENU_Test:
        OnMenuHardwareTest();
        break;
	case IDMENU_SystemMetrics:
		OnMenuSystemMetrics();
		break;
	case IDMENU_Network:
		OnMenuNetworking();
		break;
	case IDMENU_Display:
		OnMenuDisplay();
		break;
	case IDMENU_CommPorts:
		OnMenuCommPorts();
		break;
    case IDMENU_Bench:
        OnMenuBenchmark();
        break;
    case IDMENU_Audio:
        OnMenuAudio();
        break;
    case IDMENU_GPS:
        OnMenuGPS();
        break;
	case IDMENU_Menu:
		OnMenuLeft();
		break;	
	case IDMENU_Components:
		OnMenuRight();
		break;
	case IDMENU_Help:
		OnHelp();
		break;
	case IDMENU_Cancel:
		break;
	case IDMENU_About:
		MessageBox(m_hWnd, _T("Mobile System Info v0.8.2\r\n© 2007-2009 \
			www.panoramicsoft.com"),  _T("Mobile System Info"), MB_OK);
		break;
	case IDMENU_Exit:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgSysInfo::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return FALSE;

}


BOOL CDlgSysInfo::OnMenuLeft()
{
	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;
	
	TCHAR szText[STRING_MAX];

	// *************************
	m_oStr->StringCopy(szText, IDS_MENU_About, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_About, szText);
	m_oStr->StringCopy(szText, IDS_MENU_Exit, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Exit, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.left + GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;


	//Display it.
	TrackPopupMenu(	hMenu, 
		TPM_LEFTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWnd, NULL);

	DestroyMenu(hMenu);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuRight()
{
	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];

	// *************************
    m_oStr->StringCopy(szText, _T("Benchmark"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Bench, szText);

    m_oStr->StringCopy(szText, _T("Audio"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Audio, szText);
	
	m_oStr->StringCopy(szText, _T("CPU Info"));
	AppendMenu(hMenu, MF_STRING, IDMENU_CPU, szText);
	m_oStr->StringCopy(szText, _T("Comm Ports"));
	AppendMenu(hMenu, MF_STRING, IDMENU_CommPorts, szText);
    m_oStr->StringCopy(szText, _T("Device Time"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Time, szText);
    m_oStr->StringCopy(szText, _T("Display"));
	AppendMenu(hMenu, MF_STRING, IDMENU_Display, szText);
    m_oStr->StringCopy(szText, _T("GPS"));
    AppendMenu(hMenu, MF_STRING, IDMENU_GPS, szText);
  	m_oStr->StringCopy(szText, _T("Memory"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Memory, szText);
    m_oStr->StringCopy(szText, _T("Networking"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Network, szText);
	m_oStr->StringCopy(szText, _T("Power"));
	AppendMenu(hMenu, MF_STRING, IDMENU_Power, szText);
    m_oStr->StringCopy(szText, _T("Storage"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Storage, szText);
    m_oStr->StringCopy(szText, _T("System Info"));
    AppendMenu(hMenu, MF_STRING, IDMENU_System, szText);
    m_oStr->StringCopy(szText, _T("Test Inputs"));
    AppendMenu(hMenu, MF_STRING, IDMENU_Test, szText);
 //   m_oStr->StringCopy(szText, _T("System Metrics"));
//    AppendMenu(hMenu, MF_STRING, IDMENU_SystemMetrics, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;


	//Display it.
	TrackPopupMenu(	hMenu, 
		TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWnd, NULL);

	DestroyMenu(hMenu);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuSystem()
{
	CWndSystem wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuDisplay()
{
	CWndDisplay wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuTime()
{
	CWndTime wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuSystemMetrics()
{
	CWndSystemMetrics wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuBenchmark()
{
    CWndBenchmark wndBench;
    wndBench.Init(m_hInst);
    wndBench.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
    wndBench.SetBestScore(m_iHiScore);
    wndBench.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
    if(wndBench.GetScore() > m_iHiScore)
        m_iHiScore = wndBench.GetScore();//save the highscore 

    return TRUE;
}

BOOL CDlgSysInfo::OnMenuHardwareTest()
{
    CWndTest wndText;
    wndText.Init(m_hInst);
    wndText.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
    wndText.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
    return TRUE;
}


BOOL CDlgSysInfo::OnMenuMemory()
{
	CWndMemory wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuStorage()
{
	CWndStorage wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuNetworking()
{
	CWndNetworking wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuCommPorts()
{
	CWndCommPorts wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuCPU()
{
	CWndCPU wndSys;
	wndSys.Init(m_hInst);
    wndSys.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
	wndSys.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	return TRUE;
}

BOOL CDlgSysInfo::OnMenuAudio()
{
    CWndAudio wndAudio;
    wndAudio.Init(m_hInst);
    wndAudio.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
    wndAudio.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    return TRUE;
}

BOOL CDlgSysInfo::OnMenuGPS()
{
    CWndGPS wndGPS;
    wndGPS.Init(m_hInst);
    wndGPS.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
    wndGPS.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    return TRUE;
}

BOOL CDlgSysInfo::OnMenuPower()
{
    CWndPower wndPower;
    wndPower.Init(m_hInst);
    wndPower.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel);
    wndPower.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
    return TRUE;
}

BOOL CDlgSysInfo::OnMenuContext(POINT pt)
{
	return TRUE;
}

BOOL CDlgSysInfo::OnHelp()
{
	// launch the help
//	LaunchHelp(_T("SysInfo.htm"), m_hInst);
	return TRUE;
}



BOOL CDlgSysInfo::OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
/*	SHHandleWMSettingChange(m_hWnd, SPI_SETSIPINFO, lParam, &m_sSai); 
	OnSize(hWnd, NULL, NULL);
	SetFocus(m_edtCtrl.GetWnd());*/
	return UNHANDLED;
}

void CDlgSysInfo::GetTime()
{
	GetLocalTime(&m_sSysTime);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_sSysTime, _T("h':'mm':'ss tt"),m_szTime, STRING_LARGE);

	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &m_sSysTime, NULL, m_szDate, STRING_LARGE);	

	
}

