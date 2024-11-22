#include "StdAfx.h"
#include "DlgOptions.h"
#include "IssCommon.h"
#include "Resource.h"

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*4/9)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)

CDlgOptions::CDlgOptions(void)
:m_wndMenu(NULL)
{
    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

BOOL CDlgOptions::Init(TypeOptions& sOptions, CIssWndTouchMenu* wndMenu)
{
	m_sOptions      = sOptions;
    m_wndMenu       = wndMenu;

	return TRUE;
}


BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//these are up here for a reason 
	//InitControls();
	//SetControls();

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
	mbi.nToolBarId	= IDR_MENU_Options;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
	{
		int i = 0;
	}
#endif
    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDR_PNG_MenuSelect);
    m_oMenu.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    PopulateOptions();

	return TRUE;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_ESCAPE)
    {
        SafeCloseWindow(0);
        return TRUE;
    }


    return FALSE;
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), HEIGHT(rc));

    return TRUE;
}


BOOL CDlgOptions::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || m_gdiMem.GetWidth() != WIDTH(rcClient) || m_gdiMem.GetHeight() != HEIGHT(rcClient))
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    // draw the background
    FillRect(m_gdiMem, rcClip, RGB(0,0,0));

    m_oMenu.OnDraw(m_gdiMem, rcClient, rcClip);

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
	
	return TRUE;
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
    {
    case IDMENU_Cancel:
		SafeCloseWindow(IDCANCEL);
        break;
    case IDMENU_Save:
        SafeCloseWindow(IDOK);
        break;
    case IDMENU_Select:
    case IDOK:
        LaunchPopupMenu();
        break;
    case IDMENU_SelectMenu:
        SetMenuSelection();
        break;
	}

	return TRUE;
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}


BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt))
    {}

    return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
        return TRUE;

    return TRUE;
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

/*BOOL CDlgOptions::InitControls()
{
	//Check Boxes
	m_hWndCheckUseRegional = CreateWindowEx(0, _T("Button"), NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 60, 60, m_hWnd, NULL, 
		m_hInst, NULL);

	SetWindowText(m_hWndCheckUseRegional, _T("Use Regional Settings"));
	
    m_hWndCheckPlaySounds = CreateWindowEx(0, _T("Button"), NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    SetWindowText(m_hWndCheckPlaySounds, _T("Play Sounds"));

	m_hWndCheckUseTape = CreateWindowEx(0, _T("Button"), NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 60, 60, m_hWnd, NULL, 
		m_hInst, NULL);

	SetWindowText(m_hWndCheckUseTape, _T("Record Tape"));

    m_hWndCheckAdvMem = CreateWindowEx(0, _T("Button"), NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    SetWindowText(m_hWndCheckAdvMem, _T("Adv. Memory"));


//static text
	m_hWndStaticCalcMode = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndStaticNotation = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndStaticSize = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndStaticRPNStack = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndStaticSkins = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndStaticFixed = CreateWindowEx(0, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

//list controls
#ifndef WIN32_PLATFORM_WFSP
	m_hWndListCalcMode = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndListNotation = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndListSize = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndListRPNStack = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndListSkins = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

	m_hWndListFixedDigits = CreateWindowEx(0, _T("ComboBox"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);

#else
    m_hWndListCalcMode = CreateWindowEx(0, _T("LISTBOX"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
		m_hInst, NULL);

    m_hWndSpin1 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndListNotation = CreateWindowEx(0, _T("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndSpin2 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndListSize = CreateWindowEx(0, _T("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndSpin3 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndListRPNStack = CreateWindowEx(0, _T("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndSpin4 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);
    
    m_hWndListSkins = CreateWindowEx(0, _T("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

    m_hWndSpin5 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);

	m_hWndListFixedDigits = CreateWindowEx(0, _T("LISTBOX"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY , 50, 50, 60, 60, m_hWnd, NULL, 
		m_hInst, NULL);

    m_hWndSpin6 = CreateWindowEx(0, _T("msctls_updown32"), NULL,
        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 50, 50, 60, 60, m_hWnd, NULL, 
        m_hInst, NULL);
#endif

	ListAddString(m_hWndListCalcMode, _T("Normal (Chain)"));
	ListAddString(m_hWndListCalcMode, _T("Algebraic"));
	ListAddString(m_hWndListCalcMode, _T("Direct Algebraic"));
	ListAddString(m_hWndListCalcMode, _T("R.P.N."));
	ListAddString(m_hWndListCalcMode, _T("Expression"));

	ListAddString(m_hWndListSize, _T("Byte (8 bit)"));
	ListAddString(m_hWndListSize, _T("Word (16 bit)"));
	ListAddString(m_hWndListSize, _T("Dword (32 bit)"));
	ListAddString(m_hWndListSize, _T("Large (48 bit)"));

	ListAddString(m_hWndListRPNStack, _T("4 (X,Y,Z,T)"));
	ListAddString(m_hWndListRPNStack, _T("5"));
	ListAddString(m_hWndListRPNStack, _T("10"));
	ListAddString(m_hWndListRPNStack, _T("50"));
	
	ListAddString(m_hWndListNotation, _T("Normal"));
	ListAddString(m_hWndListNotation, _T("Fixed"));
	ListAddString(m_hWndListNotation, _T("Scientific"));
	ListAddString(m_hWndListNotation, _T("Engineering"));
	ListAddString(m_hWndListNotation, _T("Fractions"));

	//lets keep these in enum order for now
	ListAddString(m_hWndListSkins, _T("Metallic Blue"));
	ListAddString(m_hWndListSkins, _T("Midnight Black"));
//	ListAddString(m_hWndListSkins, _T("Classic Calc"));
//	ListAddString(m_hWndListSkins, _T("Pro Brown"));
    ListAddString(m_hWndListSkins, _T("Wireless Green"));
    ListAddString(m_hWndListSkins, _T("Glam Pink"));
#ifdef DEBUG
    ListAddString(m_hWndListSkins, _T("From File"));
#endif
	//for fixed
	TCHAR szTemp[STRING_SMALL];
	for(int i = 0; i < 9; i++)
	{
		m_oStr->IntToString(szTemp, i);
		ListAddString(m_hWndListFixedDigits, szTemp);
	}

#ifdef WIN32_PLATFORM_WFSP
    SetWindowText(m_hWndStaticNotation, _T("Notation:"));
    SetWindowText(m_hWndStaticCalcMode, _T("Calc Mode:"));
    SetWindowText(m_hWndStaticRPNStack, _T("RPN Stack:"));
    SetWindowText(m_hWndStaticSize, _T("Conversion:"));
    SetWindowText(m_hWndStaticSkins, _T("Skin:"));
    SetWindowText(m_hWndStaticFixed, _T("Fixed Digits:"));//this needs to be named better
#else
	SetWindowText(m_hWndStaticNotation, _T("Notation:"));
	SetWindowText(m_hWndStaticCalcMode, _T("Calculator Mode:"));
	SetWindowText(m_hWndStaticRPNStack, _T("RPN Stack Size:"));
	SetWindowText(m_hWndStaticSize, _T("Conversion Buffer:"));
	SetWindowText(m_hWndStaticSkins, _T("Skin:"));
    SetWindowText(m_hWndStaticFixed, _T("Fixed Digits:"));//this needs to be named better
#endif
	return TRUE;
}


int CDlgOptions::SetControls()
{
#ifdef WIN32_PLATFORM_PSPC
	DWORD dwSetCurSel	= CB_SETCURSEL;
	DWORD dwGetCount	= CB_GETCOUNT;
	DWORD dwGetText		= CB_GETLBTEXT;
#else
	DWORD dwSetCurSel	= LB_SETCURSEL;
	DWORD dwGetCount	= LB_GETCOUNT;
	DWORD dwGetText		= LB_GETTEXT;
#endif

	//and finally ... set the check box
	if(*m_bUseRegional == TRUE)
		SendMessage(m_hWndCheckUseRegional, BM_SETCHECK, BST_CHECKED, 0);

	if(*m_bPlaySounds == TRUE)
		SendMessage(m_hWndCheckPlaySounds, BM_SETCHECK, BST_CHECKED, 0);

	if(*m_bUseTape == TRUE)
		SendMessage(m_hWndCheckUseTape, BM_SETCHECK, BST_CHECKED, 0);

    if(*m_bAdvMem == TRUE)
        SendMessage(m_hWndCheckAdvMem, BM_SETCHECK, BST_CHECKED, 0);

	DWORD dwTemp;

	//calculator mode
	switch(*m_eMode)
	{
	default:
	case CALC_Chain:
		dwTemp = 0;
		break;
	case CALC_OrderOfOpps:
		dwTemp = 1;
		break;
	case CALC_DAL:
		dwTemp = 2;
	    break;
	case CALC_RPN:
		dwTemp = 3;
	    break;
	case CALC_String:
		dwTemp = 4;
		break;
	}

	PostMessage(m_hWndListCalcMode, dwSetCurSel, dwTemp, 0);

	//bits for base conversion
	switch(m_oCalc->GetBaseBits())
	{
		case 8:
			dwTemp = 0;
			break;
		case 16:
			dwTemp = 1;
			break;
		case 32:
			dwTemp = 2;
			break;
		case 48:
		default:
			dwTemp = 3;
			break;
	}

	PostMessage(m_hWndListSize, dwSetCurSel, dwTemp, 0);

	//RPN stack size
	switch(m_oCalc->GetRPNStackSize())
	{
	case 4:
		dwTemp = 0;
		break;
	case 5:
		dwTemp = 1;
		break;
	case 10:
		dwTemp = 2;
		break;
	case 50:
	default:
		dwTemp = 3;
		break;
	}

	PostMessage(m_hWndListRPNStack, dwSetCurSel, dwTemp, 0);


	//Notation
	dwTemp = (DWORD)m_oCalc->GetCalcDisplay();
	PostMessage(m_hWndListNotation, dwSetCurSel, dwTemp, 0);

	//skin .. this one is easier
	PostMessage(m_hWndListSkins, dwSetCurSel, (DWORD)*m_eSkins, 0);

	//fixed digits
	PostMessage(m_hWndListFixedDigits, dwSetCurSel, (DWORD)m_oCalc->GetFixedDecDigits(), 0);
	
	return TRUE;
}*/





/*void CDlgOptions::SaveSettings()
{

#ifdef WIN32_PLATFORM_PSPC
	DWORD 			dwGetText	= CB_GETLBTEXT;//WM_GETTEXT
	DWORD			dwCurSel	= CB_GETCURSEL;
#else
	DWORD 			dwGetText	= LB_GETTEXT;
	DWORD			dwCurSel	= LB_GETCURSEL;
#endif
	DWORD dwTemp = SendMessage(m_hWndListCalcMode, dwCurSel, 0, 0);

	switch(dwTemp)
	{
	case 0:
	default:			
		*m_eMode = CALC_Chain;
		break;
	case 1:
		*m_eMode = CALC_OrderOfOpps;
		break;
	case 2:
		*m_eMode = CALC_DAL;
		break;
	case 3:
		*m_eMode = CALC_RPN;
		break;
	case 4:
		*m_eMode = CALC_String;
		break;
	}

	dwTemp = SendMessage(m_hWndListRPNStack, dwCurSel, 0, 0);

	switch(dwTemp)
	{
	case 0:
	default:			
		m_oCalc->SetRPNStackSize(4);
		break;
	case 1:
		m_oCalc->SetRPNStackSize(5);
		break;
	case 2:
		m_oCalc->SetRPNStackSize(10);
		break;
	case 3:
		m_oCalc->SetRPNStackSize(50);
		break;
	}

	//display formatting
	dwTemp = SendMessage(m_hWndListNotation, dwCurSel, 0, 0);
	m_oCalc->SetCalcDisplay((DisplayType)dwTemp);


	//base bits
	dwTemp = SendMessage(m_hWndListSize, dwCurSel, 0, 0);

	switch(dwTemp)
	{
	case 0:
	default:			
		m_oCalc->SetBaseBits(8);
		break;
	case 1:
		m_oCalc->SetBaseBits(16);
		break;
	case 2:
		m_oCalc->SetBaseBits(32);
		break;
	case 3:
		m_oCalc->SetBaseBits(48);
		break;
	}

	//calc skin
	dwTemp = SendMessage(m_hWndListSkins, dwCurSel, 0, 0);
	*m_eSkins = (EnumCalcProSkins)dwTemp;

	//regional settings
	if(BST_CHECKED==SendMessage(m_hWndCheckUseRegional, BM_GETCHECK,0,0))
		*m_bUseRegional = TRUE;
	else 
		*m_bUseRegional = FALSE;

	//play sounds
	if(BST_CHECKED==SendMessage(m_hWndCheckPlaySounds, BM_GETCHECK,0,0))
		*m_bPlaySounds = TRUE;
	else 
		*m_bPlaySounds = FALSE;

	//use tape
	if(BST_CHECKED==SendMessage(m_hWndCheckUseTape, BM_GETCHECK,0,0))
		*m_bUseTape = TRUE;
	else 
		*m_bUseTape = FALSE;

    //advanced memory
    if(BST_CHECKED==SendMessage(m_hWndCheckAdvMem, BM_GETCHECK, 0, 0))
        *m_bAdvMem = TRUE;
    else
        *m_bAdvMem = FALSE;

	//fixed digits
	dwTemp = SendMessage(m_hWndListFixedDigits, dwCurSel, 0, 0);
	m_oCalc->SetFixedDecDigits((int)dwTemp);


}*/

void CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgOptions* pThis = (CDlgOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumOption* eInfo = (EnumOption*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eInfo);
}

BOOL CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption)
{
    // draw the down arrow
    Draw(gdi,
        rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0);

    RECT rcText;

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);

    switch(*eOption)
    {
    case OPTION_UseRegionalSettings:
        m_oStr->StringCopy(szInfo, m_sOptions.bUseRegionalSettings?_T("On"):_T("Off"));
        break;
    case OPTION_PlaySounds:
        //m_oStr->StringCopy(szInfo, m_sOptions.bPlaySounds?_T("On"):_T("Off"));
        GetSoundModeText(m_sOptions.bPlaySounds, szInfo);
        break;
    case OPTION_RecordTape:
        m_oStr->StringCopy(szInfo, m_sOptions.bRecordTape?_T("On"):_T("Off"));
        break;
    case OPTION_AdvancedMemory:
        m_oStr->StringCopy(szInfo, m_sOptions.bAdvancedMemory?_T("On"):_T("Off"));
        break;

    case OPTION_GraphAccuracy:
        m_oStr->StringCopy(szInfo, m_sOptions.bGraphFast?_T("On"):_T("Off"));
        break;
    case OPTION_GraphUsePoints:
        m_oStr->StringCopy(szInfo, m_sOptions.bGraphUsePoints?_T("On"):_T("Off"));
        break;
    case OPTION_EndPeriodPayments:
        m_oStr->StringCopy(szInfo, m_sOptions.bTVMEndPeriod?_T("On"):_T("Off"));
        break;

    case OPTION_CalculatorMode:
        GetCalcModeText(m_sOptions.eCalculatorMode, szInfo);
        break;
    case OPTION_Notation:
        GetNotationText(m_sOptions.eNotation, szInfo);
        break;
    case OPTION_ConversionBuffer:
        GetConversionBufferText(m_sOptions.iConversionBuffer, szInfo);
        break;
    case OPTION_RPNStackSize:
        GetRPNStackSizeText(m_sOptions.iRPNStackSize, szInfo);
        break;
    case OPTION_Skin:
        GetSkinsText(m_sOptions.eSkin, szInfo);
        break;
    case OPTION_FixedDigits:
        GetFixedDigitsText(m_sOptions.iFixedDigits, szInfo);
        break;

    }

    // title text
    rcText			= rc;
    rcText.left		+= INDENT;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?TEXTCOLOR_Normal:TEXTCOLOR_Item));


    return TRUE;
}

void CDlgOptions::GetFixedDigitsText(int iFixedDigits, TCHAR* szInfo)
{
    if(iFixedDigits < 0 || iFixedDigits > 8)
        return;
    m_oStr->IntToString(szInfo, iFixedDigits);
}

void CDlgOptions::GetSoundModeText(BOOL bPlaySounds, TCHAR* szInfo)
{
    switch((int)bPlaySounds)
    {
    case 0:
        m_oStr->StringCopy(szInfo, _T("Off"));
    	break;
    case 1:
        m_oStr->StringCopy(szInfo, _T("On"));
    	break;
    case 2:
        m_oStr->StringCopy(szInfo, _T("Quiet"));
        break;
    default:
        break;
    }
}

void CDlgOptions::GetSkinsText(EnumCalcProSkins eSkin, TCHAR* szInfo)
{
    switch(eSkin)
    {
    case SKIN_Blue:
        m_oStr->StringCopy(szInfo, _T("Metallic blue"));
        break;
    case SKIN_Black:
        m_oStr->StringCopy(szInfo, _T("Midnight black"));
        break;
    case SKIN_Green:
        m_oStr->StringCopy(szInfo, _T("Wireless green"));
        break;
    case SKIN_Pink:
        m_oStr->StringCopy(szInfo, _T("Glam pink"));
        break;
    case SKIN_Silver:
        m_oStr->StringCopy(szInfo, _T("Blue Steel"));
        break;
    }
}

void CDlgOptions::GetRPNStackSizeText(int iRPNStackSize, TCHAR* szInfo)
{
    switch(iRPNStackSize)
    {
    case 4:
        m_oStr->StringCopy(szInfo, _T("4(X,Y,Z,T)"));
        break;
    case 5:
        m_oStr->StringCopy(szInfo, _T("5"));
        break;
    case 10:
        m_oStr->StringCopy(szInfo, _T("10"));
        break;
    case 50:
        m_oStr->StringCopy(szInfo, _T("50"));
        break;
    }
}

void CDlgOptions::GetConversionBufferText(int iConversionBuffer, TCHAR* szInfo)
{
    switch(iConversionBuffer)
    {
    case 8:
        m_oStr->StringCopy(szInfo, _T("Byte (8 bit)"));
        break;
    case 16:
        m_oStr->StringCopy(szInfo, _T("Word (16 bit)"));
        break;
    case 32:
        m_oStr->StringCopy(szInfo, _T("DWord (32 bit)"));
        break;
    case 48:
        m_oStr->StringCopy(szInfo, _T("Large (48 bit)"));
        break;
    }
}

void CDlgOptions::GetNotationText(DisplayType eNotation, TCHAR* szInfo)
{
    switch(eNotation)
    {
    case DISPLAY_Float:
        m_oStr->StringCopy(szInfo, _T("Normal"));
        break;
    case DISPLAY_Fixed:
        m_oStr->StringCopy(szInfo, _T("Fixed"));
        break;
    case DISPLAY_Scientific:
        m_oStr->StringCopy(szInfo, _T("Scientific"));
        break;
    case DISPLAY_Engineering:
        m_oStr->StringCopy(szInfo, _T("Engineering"));
        break;
    case DISPLAY_Fractions:
        m_oStr->StringCopy(szInfo, _T("Fractions"));
        break;
    }
}

void CDlgOptions::GetCalcModeText(CalcOperationType eCalculatorMode, TCHAR* szInfo)
{
    switch(eCalculatorMode)
    {
    case CALC_Chain:
        m_oStr->StringCopy(szInfo, _T("Normal"));
        break;
    case CALC_OrderOfOpps:
        m_oStr->StringCopy(szInfo, _T("Algebraic"));
        break;
    case CALC_DAL:
        m_oStr->StringCopy(szInfo, _T("Direct Algebraic"));
        break;
    case CALC_RPN:
        m_oStr->StringCopy(szInfo, _T("R.P.N."));
        break;
    case CALC_String:
        m_oStr->StringCopy(szInfo, _T("Expression"));
        break;
    }
}

void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case OPTION_UseRegionalSettings:
        m_oStr->StringCopy(szTitle, _T("Use regional settings:"));
        break;
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, _T("Play sounds:"));
        break;
    case OPTION_RecordTape:
        m_oStr->StringCopy(szTitle, _T("Record tape:"));
        break;
    case OPTION_AdvancedMemory:
        m_oStr->StringCopy(szTitle, _T("Advanced memory:"));
        break;
    case OPTION_CalculatorMode:
        m_oStr->StringCopy(szTitle, _T("Calculator mode:"));
        break;
    case OPTION_Notation:
        m_oStr->StringCopy(szTitle, _T("Notation:"));
        break;
    case OPTION_ConversionBuffer:
        m_oStr->StringCopy(szTitle, _T("Conversion buffer:"));
        break;
    case OPTION_RPNStackSize:
        m_oStr->StringCopy(szTitle, _T("RPN stack size:"));
        break;
    case OPTION_Skin:
        m_oStr->StringCopy(szTitle, _T("Skin:"));
        break;
    case OPTION_FixedDigits:
        m_oStr->StringCopy(szTitle, _T("Fixed digits:"));
        break;
    case OPTION_GraphAccuracy:
        m_oStr->StringCopy(szTitle, _T("Fast graphing:"));
        break;
    case OPTION_GraphUsePoints:
        m_oStr->StringCopy(szTitle, _T("Show points only:"));
        break;
    case OPTION_EndPeriodPayments:
        m_oStr->StringCopy(szTitle, _T("End period payments (TVM):"));
        break;


    }
}

void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

    m_oMenu.AddCategory(_T("General"));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_UseRegionalSettings;
    m_oMenu.AddItem(eOption, IDMENU_Select);   

    m_oMenu.AddCategory(_T("Calculator"));

    eOption = new EnumOption;
    *eOption = OPTION_CalculatorMode;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Notation;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_FixedDigits;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

void CDlgOptions::LaunchPopupMenu()
{
    m_wndMenu->ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    TCHAR szInfo[STRING_MAX];
    GetItemTitle(*eOption, szTitle);
    m_wndMenu->AddCategory(szTitle);
    int i;

    switch(*eOption)
    {
    case OPTION_UseRegionalSettings:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bUseRegionalSettings?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bUseRegionalSettings?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_PlaySounds:
     //   m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bPlaySounds?FLAG_Radio:NULL), TRUE);
     //   m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bPlaySounds?FLAG_Radio:NULL), FALSE);
        GetSoundModeText(0, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==0?FLAG_Radio:NULL), 0);
        GetSoundModeText(1, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==1?FLAG_Radio:NULL), 1);
        GetSoundModeText(2, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==2?FLAG_Radio:NULL), 2);
        break;
    case OPTION_RecordTape:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bRecordTape?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bRecordTape?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_AdvancedMemory:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bAdvancedMemory?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bAdvancedMemory?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_GraphAccuracy:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bGraphFast?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bGraphFast?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_GraphUsePoints:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bGraphUsePoints?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bGraphUsePoints?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_EndPeriodPayments:
        m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bTVMEndPeriod?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bTVMEndPeriod?FLAG_Radio:NULL), FALSE);
        break;

    case OPTION_CalculatorMode:        
        for(i=CALC_DAL; i<=CALC_Chain; i++)
        {
            CalcOperationType eCalcMode = (CalcOperationType)i;
            GetCalcModeText(eCalcMode, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eCalculatorMode == eCalcMode?FLAG_Radio:NULL), eCalcMode);
        }
        break;
    case OPTION_Notation:
        for(i=DISPLAY_Float; i<=DISPLAY_Fractions; i++)
        {
            DisplayType eDisplay = (DisplayType)i;
            GetNotationText(eDisplay, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eNotation == eDisplay?FLAG_Radio:NULL), eDisplay);
        }
        break;
    case OPTION_ConversionBuffer:
        i = 8;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 16;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 32;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 48;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);        
        break;
    case OPTION_RPNStackSize:
        i = 4;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 5;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 10;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 50;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        break;
    case OPTION_Skin:
        for(i=SKIN_Blue; i<=SKIN_Silver; i++)
        {
            EnumCalcProSkins eSkins = (EnumCalcProSkins)i;
            GetSkinsText(eSkins, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eSkin == eSkins?FLAG_Radio:NULL), eSkins);
        }
        break;
    case OPTION_FixedDigits:   
        for(i=0; i<=8; i++)
        {
            GetFixedDigitsText(i, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iFixedDigits == i?FLAG_Radio:NULL), i);
        }
        break;
 
    }

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    m_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = m_wndMenu->GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
    case OPTION_UseRegionalSettings:
        m_sOptions.bUseRegionalSettings = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_PlaySounds:
        m_sOptions.bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_RecordTape:
        m_sOptions.bRecordTape = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_AdvancedMemory:
        m_sOptions.bAdvancedMemory = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_CalculatorMode:
        m_sOptions.eCalculatorMode = (CalcOperationType)sMenuItem->lParam;
        break;
    case OPTION_Notation:
        m_sOptions.eNotation = (DisplayType)sMenuItem->lParam;
        break;
    case OPTION_ConversionBuffer:
        m_sOptions.iConversionBuffer = (int)sMenuItem->lParam;
        break;
    case OPTION_RPNStackSize:
        m_sOptions.iRPNStackSize = (int)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        m_sOptions.eSkin = (EnumCalcProSkins)sMenuItem->lParam;
        break;
    case OPTION_FixedDigits:
        m_sOptions.iFixedDigits = (int)sMenuItem->lParam;
        break;
    case OPTION_GraphAccuracy:
        m_sOptions.bGraphFast = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_GraphUsePoints:
        m_sOptions.bGraphUsePoints = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_EndPeriodPayments:
        m_sOptions.bTVMEndPeriod = (BOOL)sMenuItem->lParam;
        break;
    }
}

void CDlgOptions::DeleteItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    EnumOption* eOption = (EnumOption*)lpItem;
    if(eOption)
        delete eOption;
}
