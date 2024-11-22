#include "DlgMsgBox.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssLocalisation.h"
#include "ContactsGuiDefines.h"

CDlgMsgBox::CDlgMsgBox(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_bYesNo(FALSE)
,m_szText(NULL)
,m_szTitle(NULL)
{
}

CDlgMsgBox::~CDlgMsgBox(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
    m_oStr->Delete(&m_szText);
    m_oStr->Delete(&m_szTitle);
}

int CDlgMsgBox::PopupMessage(UINT uiMessage, UINT uiTitle, HWND hWndParent, HINSTANCE hInst, UINT uiFlag)
{
    TCHAR szText[STRING_MAX*2];
    m_oStr->StringCopy(szText, uiMessage, STRING_MAX*2, hInst);

    TCHAR* szTitle = NULL;
    if(uiTitle != -1)
    {

        szTitle = new TCHAR[STRING_MAX];
        m_oStr->StringCopy(szTitle, uiTitle, STRING_MAX, hInst);
    }


    return PopupMessage(szText, szTitle, hWndParent, hInst, uiFlag);

    m_oStr->Delete(&szTitle);
}

int CDlgMsgBox::PopupMessage(TCHAR* szMessage, TCHAR* szTitle, HWND hWndParent, HINSTANCE hInst, UINT uiFlag)
{
    if(szMessage == NULL)
        return 0;
    
    m_oStr->Delete(&m_szText);
    m_szText = m_oStr->CreateAndCopy(szMessage);

    m_oStr->Delete(&m_szTitle);
    if(szTitle != NULL)
        m_szTitle = m_oStr->CreateAndCopy(szTitle);

    if(uiFlag == MB_OK)
        m_bYesNo = FALSE;
    else
        m_bYesNo = TRUE;

    //BOOL bRet = DoModal(hWndParent, hInst, IDD_DLG_Basic);
	BOOL bRet = CDlgBase::Launch(hWndParent, hInst, FALSE);
    return (int)bRet;
}
void CDlgMsgBox::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	RECT rc = m_rcTitle;
	rc.left += INDENT;
    if(m_szTitle)
	    ::DrawText(gdi, m_szTitle, rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgMsgBox::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	RECT rc = m_rcBottomBar;
	rc.right = m_rcBottomBar.right/2;
    if(m_bYesNo)
        DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_Yes), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    else
        DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_MENU_Ok), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

	rc = m_rcBottomBar;
	rc.left = m_rcBottomBar.right/2;
    if(m_bYesNo)
        DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_No), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

BOOL CDlgMsgBox::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgMsgBox::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CDlgBase::OnSize(hWnd, wParam, lParam);

	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
	int iBtnWidth		= GetSystemMetrics(SM_CXSCREEN)/4;
	int iBtnHeight		= GetSystemMetrics(SM_CYICON);
	int iTextHeight		= GetSystemMetrics(SM_CXICON)*3/6;


	m_rcText			= rcClient;
	m_rcText.top		+= GetSystemMetrics(SM_CXICON);
    m_rcText.bottom     -= GetSystemMetrics(SM_CXICON);
    m_rcText.left       += GetSystemMetrics(SM_CXSMICON);
    m_rcText.right      -= GetSystemMetrics(SM_CXSMICON);

    if(m_bYesNo)
    {
	    m_rcYes.left		= rcClient.right/2 - iBtnWidth - iShadowSpace;
	    m_rcYes.right		= m_rcYes.left + iBtnWidth;
	    m_rcYes.top			= m_rcText.bottom;;
	    m_rcYes.bottom		= m_rcYes.top + iBtnHeight;

	    m_rcNo				= m_rcYes;
	    m_rcNo.left			= rcClient.right/2 + iShadowSpace;
	    m_rcNo.right		= m_rcNo.left + iBtnWidth;
    }
    else
    {
        m_rcYes.left		= rcClient.right/2 - iBtnWidth/2;
        m_rcYes.right		= m_rcYes.left + iBtnWidth;
        m_rcYes.top			= m_rcText.bottom;
        m_rcYes.bottom		= m_rcYes.top + iBtnHeight;

        SetRect(&m_rcNo, 0,0,0,0);
    }

	return TRUE;
}

BOOL CDlgMsgBox::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
        SafeCloseWindow(m_bYesNo?IDYES:IDOK);
		break;
	case VK_TSOFT2:
    case VK_ESCAPE:
        SafeCloseWindow(m_bYesNo?IDNO:IDOK);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;

}
void CDlgMsgBox::OnMenuLeft()
{
	SafeCloseWindow(m_bYesNo?IDYES:IDOK);
}
void CDlgMsgBox::OnMenuRight()
{
    if(m_bYesNo)
	    SafeCloseWindow(IDNO);
}

BOOL CDlgMsgBox::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgMsgBox::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	//CDlgBase::OnDraw(m_gdiMem, rcClient, rcClient);
	//DrawText();

	//FadeIn(hDC, m_gdiMem);

	return TRUE;
}

BOOL CDlgMsgBox::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	DrawText(gdi);
	return TRUE;
}


void CDlgMsgBox::DrawText(CIssGDIEx& gdi)
{
	/*RECT rc = m_rcText;
	rc.left ++;
	rc.top ++;
	::DrawText(gdi, m_szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
	::DrawText(gdi, m_szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(255,255,255));	*/
    ::DrawTextShadow(gdi, m_szText, m_rcText, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(255,255,255), RGB(0,0,0));
}




