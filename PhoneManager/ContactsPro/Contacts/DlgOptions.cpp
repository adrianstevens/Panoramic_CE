#include "StdAfx.h"
#include "resource.h"
#include "DlgOptions.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssCommon.h"

#define IDMENU_Buttons      5000
#define IDMENU_Selection    5001
#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*3/4)

CDlgOptions::CDlgOptions(void)
:m_bFadeIn(FALSE)
{
    ZeroMemory(&m_sOptions, sizeof(TypeOptions));
	m_hFontText		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
	m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}


BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.SetSelected(IDR_PNG_ContactsSelector);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.PreloadImages(hWnd, m_hInst);

    m_eColorScheme = g_eColorScheme;

    // Add Our Options here
    PopulateOptions();

    m_bFadeIn = TRUE;
	return TRUE;
}

BOOL CDlgOptions::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    // when the popup menu is up then darken out the rest of the screen a little
    if(m_wndMenu.IsWindowUp(FALSE))
        AlphaFillRect(gdi, m_rcArea, 0, 150);

	if(m_bFadeIn == TRUE)
    {
        FadeInScreen();
		return FALSE;
    }

	return TRUE;
}

void CDlgOptions::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
	DrawText(gdi, _T("Options"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));

}

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
    case OPTION_ShowSliderBar:
        if(m_sOptions.bShowSliderBar)
            m_oStr->StringCopy(szInfo, _T("On"));
        else
            m_oStr->StringCopy(szInfo, _T("Off"));
        break;
    case OPTION_SortContacts:
        switch(m_sOptions.eSortContacts)
        {
        case SORT_FirstName:
            m_oStr->StringCopy(szInfo, _T("First name"));
            break;
        case SORT_LastName:
            m_oStr->StringCopy(szInfo, _T("Last name"));
            break;
        case SORT_Email:
            m_oStr->StringCopy(szInfo, _T("Email"));
            break;
        case SORT_Company:
            m_oStr->StringCopy(szInfo, _T("Company name"));
            break;
        }
        break;
    case OPTION_SMSHistory:
        switch(m_sOptions.eShowSMS)
        {
        case SMS_All:
            m_oStr->StringCopy(szInfo, _T("All"));
            break;
        case SMS_Incoming:
            m_oStr->StringCopy(szInfo, _T("Incoming"));
            break;
        case SMS_Outgoing:
            m_oStr->StringCopy(szInfo, _T("Outgoing"));
            break;
        case SMS_Off:
            m_oStr->StringCopy(szInfo, _T("Off"));
            break;
        }
        break;
    case OPTION_CallHistory:
        switch(m_sOptions.eShowCall)
        {
        case CALL_All:
            m_oStr->StringCopy(szInfo, _T("All"));
            break;
        case CALL_Incoming:
            m_oStr->StringCopy(szInfo, _T("Incoming"));
            break;
        case CALL_Outgoing:
            m_oStr->StringCopy(szInfo, _T("Outgoing"));
            break;
        case CALL_Missed:
            m_oStr->StringCopy(szInfo, _T("Missed"));
            break;
        case CALL_Off:
            m_oStr->StringCopy(szInfo, _T("Off"));
            break;
        }
        break;
    case OPTION_ColorScheme:
        switch(m_eColorScheme)
        {
        case COLOR_Blue:
            m_oStr->StringCopy(szInfo, _T("Blue"));
            break;
        case COLOR_Green:
            m_oStr->StringCopy(szInfo, _T("Green"));
            break;
        case COLOR_Orange:
            m_oStr->StringCopy(szInfo, _T("Orange"));
            break;
        case COLOR_Pink:
            m_oStr->StringCopy(szInfo, _T("Pink"));
            break;
        case COLOR_Purple:
            m_oStr->StringCopy(szInfo, _T("Purple"));
            break;
        case COLOR_Red:
            m_oStr->StringCopy(szInfo, _T("Red"));
            break;
        }
        break;
    case OPTION_ShowHistory:
        switch(m_sOptions.eShowHistory)
        {
        case HISTORY_All:
            m_oStr->StringCopy(szInfo, _T("All"));
            break;
        case HISTORY_LastMonth:
            m_oStr->StringCopy(szInfo, _T("Last month"));
            break;
        case HISTORY_Last100:
            m_oStr->StringCopy(szInfo, _T("Last 100"));
            break;
        case HISTORY_Last200:
            m_oStr->StringCopy(szInfo, _T("Last 200"));
            break;
        }
        break;
    }


	// title text
	rcText			= rc;
	rcText.left		+= INDENT;
	DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
	rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(0,0,0):RGB(167,167,167)));

	return TRUE;
}

void CDlgOptions::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{

    RECT rc = m_rcBottomBar;

    if(!m_wndMenu.IsWindowUp(FALSE))
    {
        rc.right = m_rcBottomBar.right/2;
        DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }

    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgOptions::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgOptions::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnTimer(wParam, lParam);
}


BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
    {
    case IDMENU_Buttons:
        LaunchPopupMenu();
        break;
    case IDMENU_Selection:
        SetMenuSelection();
        break;
    default:
        return UNHANDLED;
    }

	return TRUE;
}

void CDlgOptions::FadeInScreen()
{
	m_bFadeIn = FALSE;
	HDC hDC = GetDC(m_hWnd);
	FadeIn(hDC, *m_gdiMem);
	ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
	if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
		return FALSE;

    m_oMenu.OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));
    return UNHANDLED;
}

void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

    //////////////////////////////////////////////////////////////////////////
    // General options
    m_oMenu.AddCategory(_T("General"));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_ColorScheme;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    //////////////////////////////////////////////////////////////////////////
    // Favorites options
    m_oMenu.AddCategory(_T("Favorites"));

    eOption = new EnumOption;
    *eOption = OPTION_ShowSliderBar;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    //////////////////////////////////////////////////////////////////////////
    // Contacts options
    m_oMenu.AddCategory(_T("Contacts"));

    eOption = new EnumOption;
    *eOption = OPTION_SortContacts;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    //////////////////////////////////////////////////////////////////////////
    // History options
    m_oMenu.AddCategory(_T("History"));

    eOption = new EnumOption;
    *eOption = OPTION_ShowHistory;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);
    eOption = new EnumOption;
    *eOption = OPTION_CallHistory;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);
    eOption = new EnumOption;
    *eOption = OPTION_SMSHistory;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
        return TRUE;
    
    return CDlgBase::OnLButtonUp(hWnd, pt);
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = m_wndMenu.GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
    case OPTION_ShowSliderBar:
        m_sOptions.bShowSliderBar = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_SortContacts:
        m_sOptions.eSortContacts = (EnumSortContacts)sMenuItem->lParam;
        break;
    case OPTION_SMSHistory:
        m_sOptions.eShowSMS = (EnumShowSMS)sMenuItem->lParam;
        break;
    case OPTION_CallHistory:
        m_sOptions.eShowCall = (EnumShowCall)sMenuItem->lParam;
        break;
    case OPTION_ColorScheme:
        {
        m_eColorScheme = (EnumColorScheme)sMenuItem->lParam;

        EnumColorScheme eOld = g_eColorScheme;
        g_eColorScheme = m_eColorScheme;
        // show the new color scheme
        m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
        m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
        m_oMenu.PreloadImages(m_hWnd, m_hInst);

        m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
        m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
        m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
        m_wndMenu.SetBackground(IDR_PNG_Group);
        m_wndMenu.PreloadImages(m_hWnd, m_hInst);

        g_eColorScheme = eOld;
        InvalidateRect(m_hWnd, NULL, FALSE);
        }
        break;
    case OPTION_ShowHistory:
        m_sOptions.eShowHistory = (EnumShowHistory)sMenuItem->lParam;
        break;
    }
}

void CDlgOptions::LaunchPopupMenu()
{
    m_wndMenu.ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    GetItemTitle(*eOption, szTitle);
    m_wndMenu.AddCategory(szTitle);

    switch(*eOption)
    {
    case OPTION_ShowSliderBar:
        m_wndMenu.AddItem(_T("On"), IDMENU_Selection, (m_sOptions.bShowSliderBar?FLAG_Radio:NULL), TRUE);
        m_wndMenu.AddItem(_T("Off"), IDMENU_Selection, (!m_sOptions.bShowSliderBar?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_SortContacts:
        m_wndMenu.AddItem(_T("First name"), IDMENU_Selection, (m_sOptions.eSortContacts==SORT_FirstName?FLAG_Radio:NULL), SORT_FirstName);
        m_wndMenu.AddItem(_T("Last name"), IDMENU_Selection, (m_sOptions.eSortContacts==SORT_LastName?FLAG_Radio:NULL), SORT_LastName);
        m_wndMenu.AddItem(_T("Email"), IDMENU_Selection, (m_sOptions.eSortContacts==SORT_Email?FLAG_Radio:NULL), SORT_Email);
        m_wndMenu.AddItem(_T("Company Name"), IDMENU_Selection, (m_sOptions.eSortContacts==SORT_Company?FLAG_Radio:NULL), SORT_Company);
        break;
    case OPTION_SMSHistory:
        m_wndMenu.AddItem(_T("All"), IDMENU_Selection, (m_sOptions.eShowSMS==SMS_All?FLAG_Radio:NULL),SMS_All);
        m_wndMenu.AddItem(_T("Incoming"), IDMENU_Selection, (m_sOptions.eShowSMS==SMS_Incoming?FLAG_Radio:NULL), SMS_Incoming);
        m_wndMenu.AddItem(_T("Outgoing"), IDMENU_Selection, (m_sOptions.eShowSMS==SMS_Outgoing?FLAG_Radio:NULL),SMS_Outgoing);
        m_wndMenu.AddItem(_T("Off"), IDMENU_Selection, (m_sOptions.eShowSMS==SMS_Off?FLAG_Radio:NULL), SMS_Off);
        break;
    case OPTION_CallHistory:
        m_wndMenu.AddItem(_T("All"), IDMENU_Selection, (m_sOptions.eShowCall==CALL_All?FLAG_Radio:NULL), CALL_All);
        m_wndMenu.AddItem(_T("Incoming"), IDMENU_Selection, (m_sOptions.eShowCall==CALL_Incoming?FLAG_Radio:NULL), CALL_Incoming);
        m_wndMenu.AddItem(_T("Outgoing"), IDMENU_Selection, (m_sOptions.eShowCall==CALL_Outgoing?FLAG_Radio:NULL), CALL_Outgoing);
        m_wndMenu.AddItem(_T("Missed"), IDMENU_Selection, (m_sOptions.eShowCall==CALL_Missed?FLAG_Radio:NULL), CALL_Missed);
        m_wndMenu.AddItem(_T("Off"), IDMENU_Selection, (m_sOptions.eShowCall==CALL_Off?FLAG_Radio:NULL), CALL_Off);
        break;
    case OPTION_ColorScheme:
        
        m_wndMenu.AddItem(_T("Blue"), IDMENU_Selection, (m_eColorScheme==COLOR_Blue?FLAG_Radio:NULL), COLOR_Blue);
        m_wndMenu.AddItem(_T("Green"), IDMENU_Selection, (m_eColorScheme==COLOR_Green?FLAG_Radio:NULL), COLOR_Green);
        m_wndMenu.AddItem(_T("Orange"), IDMENU_Selection, (m_eColorScheme==COLOR_Orange?FLAG_Radio:NULL), COLOR_Orange);
        m_wndMenu.AddItem(_T("Pink"), IDMENU_Selection, (m_eColorScheme==COLOR_Pink?FLAG_Radio:NULL), COLOR_Pink);
        m_wndMenu.AddItem(_T("Purple"), IDMENU_Selection, (m_eColorScheme==COLOR_Purple?FLAG_Radio:NULL), COLOR_Purple);
        m_wndMenu.AddItem(_T("Red"), IDMENU_Selection, (m_eColorScheme==COLOR_Red?FLAG_Radio:NULL), COLOR_Red);
        break;
    case OPTION_ShowHistory:
        m_wndMenu.AddItem(_T("All"), IDMENU_Selection, (m_sOptions.eShowHistory==HISTORY_All?FLAG_Radio:NULL), HISTORY_All);
        m_wndMenu.AddItem(_T("Last month"), IDMENU_Selection, (m_sOptions.eShowHistory==HISTORY_LastMonth?FLAG_Radio:NULL), HISTORY_LastMonth);
        m_wndMenu.AddItem(_T("Last 100"), IDMENU_Selection, (m_sOptions.eShowHistory==HISTORY_Last100?FLAG_Radio:NULL), HISTORY_Last100);
        m_wndMenu.AddItem(_T("Last 200"), IDMENU_Selection, (m_sOptions.eShowHistory==HISTORY_Last200?FLAG_Radio:NULL), HISTORY_Last200);
        break;
    }

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,m_rcTitle.bottom,
        rcClient.right*3/4, HEIGHT(m_rcArea),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case OPTION_ShowSliderBar:
        m_oStr->StringCopy(szTitle, _T("Show slider bar:"));
        break;
    case OPTION_SortContacts:
        m_oStr->StringCopy(szTitle, _T("Sort contact list by:"));
        break;
    case OPTION_SMSHistory:
        m_oStr->StringCopy(szTitle, _T("Show SMS history:"));
        break;
    case OPTION_CallHistory:
        m_oStr->StringCopy(szTitle, _T("Show call history:"));
        break;
    case OPTION_ColorScheme:
        m_oStr->StringCopy(szTitle, _T("Color scheme:"));
        break;
    case OPTION_ShowHistory:
        m_oStr->StringCopy(szTitle, _T("Show history:"));
        break;
    }
}