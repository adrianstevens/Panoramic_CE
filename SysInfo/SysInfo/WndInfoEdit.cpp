#include "WndInfoEdit.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "Resource.h"
#include "Windowsx.h"//for edit control macros .. interesting
#include "IssCommon.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0])) 

#define COLOR_TEXT_NORMAL               (COLORREF)0xCFCFCF
#define COLOR_TEXT_SELECTED             (COLORREF)0x77AF39
#define HEIGHT_Text                     (GetSystemMetrics(SM_CXICON)*4/9)

#define IDT_TIMER                       161

CWndInfoEdit::CWndInfoEdit(void)
:m_hInst(NULL)
,m_hFontDisplay(NULL)
,m_iEntryIndex(0)
,m_iScreenIndex(0)
,m_iLinesOnscreen(0)
,m_iTextHeight(0)
,m_iCount(0)
{
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    m_iTextHeight = GetSystemMetrics(SM_CXSMICON)/2;//looks about right to me
    m_hFontDisplay = CIssGDIEx::CreateFont(m_iTextHeight, FW_BOLD, TRUE);
}

CWndInfoEdit::~CWndInfoEdit(void)
{
	Destroy();
}

void CWndInfoEdit::Destroy()
{
    DeleteFont(m_hFontDisplay);
    DeleteArray();
}

void CWndInfoEdit::DeleteArray()
{
    TypeEntry* sEntry;

    for(int i = 0; i < m_arrEntries.GetSize(); i++)
    {
        sEntry = m_arrEntries[i];
        if(sEntry)
            delete sEntry;
    }

    m_arrEntries.RemoveAll();

}

BOOL CWndInfoEdit::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	mbi.nToolBarId	= IDR_MENU_Modal;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
		ASSERT(0);
#endif
    m_oList.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oList.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oList.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_oList.PreloadImages(hWnd, m_hInst);
    m_oList.SetCustomDrawFunc(DrawButtonItem, this);
    m_oList.SetDeleteItemFunc(DeleteMenuItem);
    m_oList.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oList.SetSelectedItemIndex(0, TRUE);

	return TRUE;
}
/*
void CWndInfoEdit::CreateEditControl()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE | ES_READONLY | WS_HSCROLL;

	m_hEdit = CreateWindowEx(0, TEXT("edit"), NULL,
		dwStyle,
		0, 0, 0, 0, m_hWnd, NULL, 
		m_hInst, NULL);	

	//removes the soft carrage returns ... might help
	SendMessage(m_hEdit, EM_FMTLINES, (WPARAM)FALSE, NULL );   // remove soft EOLs

	//set the font
	if(m_hFont)
		SendMessage(m_hEdit, WM_SETFONT, (WPARAM)m_hFont, (LPARAM) true);

	//move the window
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	MoveWindow( m_hEdit, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE );

	ShowWindow(m_hEdit, SW_SHOW);

	//and give it focus
	BringWindowToTop(m_hEdit);
	SetFocus(m_hEdit);
	SendMessage(m_hEdit, EM_SCROLLCARET, 0, 0);
}*/

BOOL CWndInfoEdit::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Back:
		OnMenuBack();
		break;
	case IDMENU_Refresh:
		OnMenuRefresh();
		break;
	default:
		return UNHANDLED;
		break;
	}
	return TRUE;
}

void CWndInfoEdit::OnMenuBack()
{
	EndDialog(m_hWnd, 0);
}

void CWndInfoEdit::OnMenuRefresh()
{
	Refresh();
}

LPCTSTR CWndInfoEdit::StringFromResources(UINT uStringID)
{
	// NOTE: Passing NULL for the 3rd parameter of LoadString causes it to
	// return a pointer to the string in the resource file. It requires that 
	// the resource file is compiled with the "/n" switch (see SDK docs).
	return (LPCTSTR) LoadString(m_hInst, uStringID, NULL, 0);
}


void CWndInfoEdit::Clear()
{
//	Edit_SetText(m_hEdit, TEXT(""));
    DeleteArray();
    m_oList.ResetContent();
    m_iCount = 0;
}

void CWndInfoEdit::OutputSection(LPCTSTR lpSection)
{
    m_oList.AddCategory((TCHAR*)lpSection);
}


void CWndInfoEdit::OutputString(LPCTSTR lpName, LPCTSTR lpValue)
{
//	Edit_SetSel(m_hEdit, Edit_GetTextLength(m_hEdit), Edit_GetTextLength(m_hEdit));
//	Edit_ReplaceSel(m_hEdit, lptszBuffer);
//  SendMessage(m_hEdit, WM_KEYUP, VK_DOWN, 0);
    

    TypeEntry* sEntry = NULL;
    
    sEntry = new TypeEntry;

    //valid cast for mobile ... not for anything else
    m_oStr->StringCopy(sEntry->szEntryLabel, (TCHAR*)lpName);
    m_oStr->StringCopy(sEntry->szEntryValue, (TCHAR*)lpValue);

    m_arrEntries.AddElement(sEntry);

    //we'll do the list here:
    int* iTemp = NULL;
    iTemp = new int;
    *iTemp = m_iCount;
    m_oList.AddItem(iTemp, 5000);
    m_iCount++;

}

void CWndInfoEdit::OutputResource(UINT uResourceID)
{
	OutputString(StringFromResources(uResourceID), _T(""));
}

void CWndInfoEdit::OutputTitle(UINT uResourceID)
{
	/*OutputString(TEXT("\r\n\r\n"));
	OutputResource(uResourceID);
	OutputString(TEXT(":\r\n"));*/
}

void CWndInfoEdit::OutputFormattedString(LPCTSTR tszFormat,...)
{
	TCHAR tszBuffer[MAX_PATH];
	va_list vaMarker;

	va_start(vaMarker, tszFormat);
	FormatMessage(FORMAT_MESSAGE_FROM_STRING, tszFormat, 0, 0, tszBuffer, 
		ARRAY_SIZE(tszBuffer), &vaMarker);
	va_end(vaMarker);

	OutputString(tszBuffer, _T(""));
}

BOOL CWndInfoEdit::DrawBackground(CIssGDIEx& gdiMem, RECT& rcClip)
{
    Draw(gdiMem, rcClip, *m_gdiBack, rcClip.left, rcClip.top);

    return TRUE;
}

BOOL CWndInfoEdit::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oList.OnTimer(wParam, lParam))
        return TRUE;

    return UNHANDLED;
}

BOOL CWndInfoEdit::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    DrawBackground(*m_gdiMem, rcClip);

    if(IsRectInRect(rcClip, m_rcTitle))
    {
        DrawTextShadow(*m_gdiMem, m_szName, m_rcTitle, DT_CENTER|DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), 0);
    }

    m_oList.OnDraw(*m_gdiMem, rcClient, rcClip);

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;
}
/*
BOOL CWndInfoEdit::DrawScreenEntries(HDC hDC)
{
#define INDENT_TEXT GetSystemMetrics(SM_CXSMICON)/2
#define TEXT_COLOR 0x001111

    RECT rcTemp;

    SetRect(&rcTemp, INDENT_TEXT, INDENT_TEXT, GetSystemMetrics(SM_CXSCREEN) - INDENT_TEXT, INDENT_TEXT + 2*m_iTextHeight);

    for(int i = 0; i < m_arrEntries.GetSize(); i++)
    {
        DrawText(hDC, m_arrEntries[i]->szEntryValue, rcTemp, DT_LEFT, m_hFont, TEXT_COLOR);
        rcTemp.top += m_iTextHeight*5/3;
        rcTemp.bottom += m_iTextHeight*5/3;
    }

    return TRUE;
}*/

BOOL CWndInfoEdit::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    return TRUE;
}

BOOL CWndInfoEdit::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    // first check if we need to reload the images first
    if(m_sizeWindow.cx	== (rc.right - rc.left)&&
        m_sizeWindow.cy	== (rc.bottom - rc.top))
        return TRUE;

    // update our size structure
    m_sizeWindow.cx		= rc.right - rc.left;
    m_sizeWindow.cy		= rc.bottom - rc.top;


    if(GetSystemMetrics(SM_CXSCREEN) != m_gdiMem->GetWidth() && WIDTH(rc) == GetSystemMetrics(SM_CXSCREEN) && m_gdiMem->GetWidth() != 0)
    {
        // close the window if we go from portrait to landscape
        SafeCloseWindow(IDOK);
        return TRUE;
    }

    SetRect(&m_rcTitle, rc.left, rc.top, rc.right, rc.top + GetSystemMetrics(SM_CXICON));
    SetRect(&m_rcList, rc.left, m_rcTitle.bottom, rc.right, rc.bottom);

    m_oList.OnSize(m_rcList.left,m_rcList.top, WIDTH(m_rcList), HEIGHT(m_rcList));

    return TRUE;
}

BOOL CWndInfoEdit::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE) 
    {
        ::SetWindowText(GetWnd(), _T("Wifi Item"));
    }

    return UNHANDLED;
}

BOOL CWndInfoEdit::OnLButtonDown(HWND hWnd, POINT& pt)
{

    if(m_oList.OnLButtonDown(pt))
    {}

    return TRUE;
}

BOOL CWndInfoEdit::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oList.OnLButtonUp(pt))
    {}

    return TRUE;
}

BOOL CWndInfoEdit::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oList.OnMouseMove(pt))
    {}

    return TRUE;
}

BOOL CWndInfoEdit::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oList.OnKeyDown(wParam, lParam);
}

BOOL CWndInfoEdit::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

BOOL CWndInfoEdit::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}


void CWndInfoEdit::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CWndInfoEdit* pThis = (CWndInfoEdit*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    int* iIndex = (int*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, iIndex);
}

BOOL CWndInfoEdit::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, int* iIndex)
{
    if(bIsHighlighted)
    {
        if(!m_imgButtonSel->IsLoaded() || WIDTH(rc) != m_imgButtonSel->GetWidth() || HEIGHT(rc) != m_imgButtonSel->GetHeight())
        {
            m_imgButtonSel->Initialize(m_hWnd, m_hInst, IDR_PNG_DetailsBtnGreen);
            m_imgButtonSel->SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButtonSel->DrawImage(gdi, rc.left, rc.top);
    }
    else
    {
        if(!m_imgButton->IsLoaded() || WIDTH(rc) != m_imgButton->GetWidth() || HEIGHT(rc) != m_imgButton->GetHeight())
        {
            m_imgButton->Initialize(m_hWnd, m_hInst, IDR_PNG_DetailsBtn);
            m_imgButton->SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButton->DrawImage(gdi, rc.left, rc.top);
    }

    TypeEntry* sType = (TypeEntry*)m_arrEntries.GetElement(*iIndex);
    if(sType == NULL)
        return FALSE;

    TCHAR* szName = sType->szEntryLabel;
    TCHAR* szValue = sType->szEntryValue;

//    TCHAR* szName = m_arrEntries->arrParmNames[*iIndex];
//    TCHAR* szValue= m_arrEntries->arrParmValues[*iIndex];
    if(!szName || !szValue)
        return FALSE;

    RECT rcText;
    rcText			= rc;
    rcText.left		+= GetSystemMetrics(SM_CXSMICON)/2;
    rcText.right    -= GetSystemMetrics(SM_CXSMICON);

    DrawTextShadow(gdi, szName, rcText, DT_LEFT | DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), RGB(0,0,0));
    DrawTextShadow(gdi, szValue, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), RGB(0,0,0));

    return TRUE;
}

void CWndInfoEdit::Init(CIssGDIEx* gdiMem, 
                        CIssGDIEx* gdiBackground, 
                        CIssImageSliced* imgButton,
                        CIssImageSliced* imgButtonSel,
                        HFONT      hFontLabel)
{
    m_gdiMem        = gdiMem;
    m_gdiBack       = gdiBackground;
    m_imgButton     = imgButton;
    m_imgButtonSel  = imgButtonSel;
    m_hFontBtnLabel = hFontLabel;
}

void CWndInfoEdit::DeleteMenuItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    int* iMenu = (int*)lpItem;

    CIssString* oStr = CIssString::Instance();

    if(iMenu)
        delete iMenu;
}


void CWndInfoEdit::OutputTime(const SYSTEMTIME* pcTime)
{
    if(pcTime == NULL)
        return;

    TCHAR szTemp[STRING_MAX];
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, pcTime, _T("h':'mm':'ss tt"),szTemp, STRING_LARGE);
    OutputString(szTemp, _T(""));
    GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, pcTime, NULL, szTemp, STRING_LARGE);	
    OutputString(szTemp, _T(""));

    switch(pcTime->wDayOfWeek)
    {
    case 0:
        OutputString(_T("Sunday"), _T(""));
        break;
    case 1:
        OutputString(_T("Monday"), _T(""));
        break;
    case 2:
        OutputString(_T("Tuesday"), _T(""));
        break;
    case 3:
        OutputString(_T("Wednesday"), _T(""));
        break;
    case 4:
        OutputString(_T("Thursday"), _T(""));
        break;
    case 5:
        OutputString(_T("Friday"), _T(""));
        break;
    case 6:
        OutputString(_T("Saturday"), _T(""));
        break;
    }

}