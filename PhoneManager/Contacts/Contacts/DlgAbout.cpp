#include "DlgAbout.h"
#include "IssGDIDraw.h"
#include "CommonDefines.h"

CDlgAbout::CDlgAbout(void)
{
}

CDlgAbout::~CDlgAbout(void)
{
}

void CDlgAbout::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    ::DrawText(gdi, _T("About Contacts Genius"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAbout::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Close"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

BOOL CDlgAbout::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcArea;
    rc.top += GetSystemMetrics(SM_CXICON)/2;

    TCHAR szTemp[2*STRING_MAX];
    m_oStr->Format(szTemp, _T("Phone Genius\r\n%s\r\n\r\nContacts Genius\r\n%s\r\n\r\nCopyright 2009\r\nPanoramic Software Inc\r\nAll Rights Reserved\r\nwww.panoramicsoft.com\r\n"), VERSION_PhoneGenius, VERSION_Contacts);
    ::DrawTextShadow(gdi, 
        szTemp, 
        rc, 
        DT_CENTER | DT_TOP, 
        m_hFontLabel, 
        RGB(255,255,255),
        RGB(0,0,0));
    return TRUE;
}

void CDlgAbout::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgAbout::OnMenuRight()
{
    SafeCloseWindow(IDOK);
}
