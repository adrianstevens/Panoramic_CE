#include "StdAfx.h"
#include "DlgAddPhoneNumber.h"
#include "resource.h"
#include "ObjSkinEngine.h"
#include "CallOptionsGuiDefines.h"
#include "IssCommon.h"

#define IDMENU_Selection        5000
#define WM_Clear		        (WM_USER + 301)
#define WM_Number		        (WM_USER + 302)

CDlgAddPhoneNumber::CDlgAddPhoneNumber()
{
    m_sBlocklistItem.eType  = BlocklistItem_phoneNumber;
    ZeroMemory(m_sBlocklistItem.szID, STRING_LARGE*sizeof(TCHAR));
    m_oStr->Empty(m_szNumber);
    m_hBtnFont	    = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

CDlgAddPhoneNumber::~CDlgAddPhoneNumber(void)
{
    CIssGDIEx::DeleteFont(m_hBtnFont);
}

BOOL CDlgAddPhoneNumber::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();
	return TRUE;
}


BOOL CDlgAddPhoneNumber::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    TypeItems* sItem = m_oMenu.GetItem(0);
    if(!sItem)
        return TRUE;

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), 3*sItem->iHeight);

    CreateButtons(m_rcArea.top + 3*sItem->iHeight + 1);
	return TRUE;
}

BOOL CDlgAddPhoneNumber::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_Clear)
    {
        m_oStr->Empty(m_szNumber);
        InvalidateRect(hWnd, &m_rcArea, FALSE);
    }
    else if(uiMessage >= WM_Number && uiMessage < (WM_Number + 10))
        AddNumber(uiMessage-WM_Number);

    return TRUE;
}

BOOL CDlgAddPhoneNumber::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonDown(hWnd, pt))
    {}
    else if(m_btnClear.OnLButtonDown(pt))
    {}
    else
    {
        for(int i=0; i<10; i++)
        {
            if(m_btnNumber[i].OnLButtonDown(pt))
                break;
        }
    }

    return UNHANDLED;
}

BOOL CDlgAddPhoneNumber::OnLButtonUp(HWND hWnd, POINT& pt)
{

    if(CDlgBaseOptions::OnLButtonUp(hWnd, pt))
    {}
    else if(m_btnClear.OnLButtonUp(pt))
    {}
    else
    {
        for(int i=0; i<10; i++)
        {
            if(m_btnNumber[i].OnLButtonUp(pt))
                break;
        }
    }

    return UNHANDLED;
}

void CDlgAddPhoneNumber::RefreshList()
{
    // if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

    m_oMenu.AddItem(_T("Full phone number:"), IDMENU_Selection, (m_sBlocklistItem.eType == BlocklistItem_phoneNumber?FLAG_Radio:NULL), BlocklistItem_phoneNumber);
    m_oMenu.AddItem(_T("All numbers starting with:"), IDMENU_Selection, (m_sBlocklistItem.eType == BlocklistItem_phonePrefix?FLAG_Radio:NULL), BlocklistItem_phonePrefix);
    m_oMenu.AddItem(_T("All numbers with area code:"), IDMENU_Selection, (m_sBlocklistItem.eType == BlocklistItem_phoneAreaCode?FLAG_Radio:NULL), BlocklistItem_phoneAreaCode);

    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

void CDlgAddPhoneNumber::CreateButtons(int iTop)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    int iIconSize	= GetSystemMetrics(SM_CYICON);

    RECT rc = m_rcArea;

    rc.top      = iTop;
    rc.bottom   = iTop + iIconSize;
    rc.right    = rc.right -= iIconSize*5/2;
    InflateRect(&rc, -INDENT/2, -INDENT/2);

    m_oDisplay.Init(rc);

    rc.left = rc.right + INDENT/2;
    rc.right = m_rcArea.right;
    
    m_btnClear.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
    m_btnClear.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
    m_btnClear.Init(rc, _T("Clear"), m_hWnd, WM_Clear, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

    int iTopRow = iTop + iIconSize + INDENT/2;
    int iBotRow = iTopRow + iIconSize + INDENT/2;

    int iWidth = (WIDTH(m_rcArea) - 2*INDENT)/5;
    rc      = m_rcArea;
    rc.top  = iTop;
    rc.bottom= rc.top + iIconSize;
    TCHAR szNum[STRING_SMALL];

    for(int i=0; i<5; i++)
    {
        
        rc.left = m_rcArea.left + i*(iWidth+INDENT/2) + INDENT/2;
        rc.right= rc.left + iWidth;
        rc.top = iTopRow;
        rc.bottom = rc.top + iIconSize;

        m_btnNumber[i].InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
        m_btnNumber[i].SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
        m_oStr->IntToString(szNum, i);
        m_btnNumber[i].Init(rc, szNum, m_hWnd, WM_Number+i, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);
     
        rc.top = iBotRow;
        rc.bottom = rc.top + iIconSize;

        m_btnNumber[i+5].InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
        m_btnNumber[i+5].SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
        m_oStr->IntToString(szNum, i+5);
        m_btnNumber[i+5].Init(rc, szNum, m_hWnd, WM_Number+i+5, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);


    }

}


BOOL CDlgAddPhoneNumber::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
    case IDMENU_Selection:
        SetSelection();
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CDlgAddPhoneNumber::SetSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem)
        return;

    BlocklistItem_Type eType = (BlocklistItem_Type)sItem->lParam;

    // set the new radio item and turn all the rest off
    for(int i=0; i<m_oMenu.GetItemCount(); i++)
    {
        TypeItems* sCompare = m_oMenu.GetItem(i);
        if(!sCompare)
            continue;
        if(sCompare == sItem && sCompare->dwFlags&FLAG_Radio)   // no change just exit
            return;
        else if(sCompare == sItem)
            sCompare->dwFlags   = FLAG_Radio;
        else
            sCompare->dwFlags   = 0;
    }

    m_sBlocklistItem.eType      = eType;
    m_oStr->Empty(m_szNumber);

    InvalidateRect(m_hWnd, &m_rcArea, FALSE);
}

BOOL CDlgAddPhoneNumber::FormatFullNumber(TCHAR* szSrc, TCHAR* szDest)
{
	m_oStr->Empty(szDest);

	MakeNumeric(szSrc);
	m_oStr->Concatenate(szDest, szSrc);

	// this is user typed text - could be any length
	int iLength = m_oStr->GetLength(szDest);
	if(iLength == 11 && m_oStr->GetFirst(szDest) == _T('1'))
	{
		m_oStr->Insert(szDest, _T("+"), 0);
		m_oStr->Insert(szDest, _T(" ("), 2);
		m_oStr->Insert(szDest, _T(") "), 7);
		m_oStr->Insert(szDest, _T("-"), 12);
	}
	else if (iLength == 10)
	{
		m_oStr->Insert(szDest, _T("("), 0);
		m_oStr->Insert(szDest, _T(") "), 4);
		m_oStr->Insert(szDest, _T("-"), 9);
	}
	else if (iLength == 7)
	{
		m_oStr->Insert(szDest, _T("-"), 3);
	}
	else
	{
		// we can't format this
		m_oStr->Empty(szDest);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgAddPhoneNumber::FormatNumberPrefix(TCHAR* szSrc, TCHAR* szDest)
{

	MakeNumeric(szSrc);
	m_oStr->Empty(szDest);

	// this is user typed text - could be any length
	TCHAR szTemp[STRING_LARGE];
	m_oStr->Empty(szTemp);
	m_oStr->StringCopy(szTemp, szSrc);

	int iLength = m_oStr->GetLength(szTemp);
	if(iLength > 11)
	{
		// we can't format this
		return FALSE;
	}
	if(m_oStr->GetFirst(szTemp) == _T('1'))
	{
		if(iLength)
			m_oStr->Insert(szTemp, _T("+"), 0);
		if(iLength > 3)
			m_oStr->Insert(szTemp, _T(" ("), 2);
		if(iLength > 4)//6
			m_oStr->Insert(szTemp, _T(") "), 7);
		if(iLength > 7)//12
			m_oStr->Insert(szTemp, _T("-"), 12);
	}
	else if (iLength > 3)// 6
	{
		// contains area code for sure
		m_oStr->Insert(szTemp, _T("("), 0);
		m_oStr->Insert(szTemp, _T(") "), 4);
		if(iLength > 9)
			m_oStr->Insert(szTemp, _T("-"), 9);
	}
	else
	{
		// don't know if # starts with area code
		// are we going to check for valid area codes?
	}

	m_oStr->Concatenate(szDest, _T("#'s beginning with: "));
	m_oStr->Concatenate(szDest, szTemp);

	return TRUE;
}

BOOL CDlgAddPhoneNumber::GetAreaCode(TCHAR* szSrc, TCHAR* szDest)
{
	m_oStr->Empty(szDest);

	MakeNumeric(szSrc);
	m_oStr->Concatenate(szDest, szSrc);

	int iLength = m_oStr->GetLength(szDest);
	if(m_oStr->GetFirst(szDest) == _T('1'))
	{
		m_oStr->Delete(0, 1, szDest);
	}
	if (iLength >= 3)
	{
		// use first 3 digits
		m_oStr->Insert(szDest, _T("("), 0);
		m_oStr->Insert(szDest, _T(")"), 4);
		if(m_oStr->GetLength(szDest) > 5)
			m_oStr->Delete(5, m_oStr->GetLength(szDest) - 5, szDest);
	}
	else
	{
		m_oStr->Empty(szDest);
		return FALSE;
	}

	m_oStr->Insert(szDest, _T("#'s with area code: "));

	return TRUE;
}

BOOL CDlgAddPhoneNumber::MakeNumeric(TCHAR* szText)
{
	int iLength = 0;
	for(int i = 0; i < m_oStr->GetLength(szText) && i < 64; i++)
	{
		if (iswdigit (szText[i]))
		{
			szText[iLength] = szText[i];
			iLength += 1;
		}
	}

	// Terminate the string with NULL.
	szText[iLength] = _T('\0');

	return TRUE;

}

BOOL CDlgAddPhoneNumber::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) >= _T('0') && LOWORD(wParam) <= _T('9'))
        AddNumber(LOWORD(wParam)-_T('0'));
    return TRUE;
}

BOOL CDlgAddPhoneNumber::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_oMenu.GetItemCount() == 0)
    {
        RECT rc = m_rcArea;
        rc.top  += INDENT;
        DrawTextShadow(gdi, _T("No entries found"), rc, DT_CENTER|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

    RECT rc = m_oDisplay.GetPosition();
    if(IsRectInRect(rcClip, rc))
    {
        m_oDisplay.Draw(gdi);
        rc.right -= INDENT;
        DrawText(gdi, m_szNumber, rc, DT_RIGHT|DT_VCENTER, m_hFontLabel, RGB(0,0,0));
    }

    // draw buttons
    m_btnClear.Draw(m_hBtnFont, gdi);

    for(int i=0; i<10; i++)
        m_btnNumber[i].Draw(m_hBtnFont, gdi);

    return TRUE;
}

void CDlgAddPhoneNumber::OnMenuLeft()
{
    m_oStr->Empty(m_sBlocklistItem.szID);
    TCHAR szResult[STRING_MAX] = _T("");
    BOOL bRet = FALSE;

    switch(m_sBlocklistItem.eType)
    {
    case BlocklistItem_phoneNumber:
        if(FormatFullNumber(m_szNumber, szResult) && !m_oStr->IsEmpty(szResult))
            bRet = TRUE;
        break;
    case BlocklistItem_phonePrefix:
        if(FormatNumberPrefix(m_szNumber, szResult) && !m_oStr->IsEmpty(szResult))
            bRet = TRUE;
        break;
    case BlocklistItem_phoneAreaCode:
        if(GetAreaCode(m_szNumber, szResult) && !m_oStr->IsEmpty(szResult))
            bRet = TRUE;
        break;
    }

    if(bRet)
    {
        m_oStr->StringCopy(m_sBlocklistItem.szID, szResult);
        if(!m_sBlocklistItem.szID)
            bRet = FALSE;
    }

    SafeCloseWindow(bRet?IDOK:IDCANCEL);
}

void CDlgAddPhoneNumber::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgAddPhoneNumber::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Add Number"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAddPhoneNumber::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(!m_wndMenu.IsWindowUp(FALSE)) 
        DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgAddPhoneNumber::AddNumber(int iNum)
{
    if(m_oStr->GetLength(m_szNumber) >= 11 ||
       iNum < 0 ||
       iNum > 9)
        return;

    TCHAR szNum[5];
    m_oStr->IntToString(szNum, iNum);
    m_oStr->Concatenate(m_szNumber, szNum);
    InvalidateRect(m_hWnd, &m_rcArea, FALSE);
}
