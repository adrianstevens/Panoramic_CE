#include "DlgAbout.h"
#include "IssGDIDraw.h"
#include "CommonDefines.h"
#include "Resource.h"
#include "IssLocalisation.h"

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
    ::DrawText(gdi, m_oStr->GetText(ID(IDS_About_Launchpad), m_hInst), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAbout::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_Close), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

BOOL CDlgAbout::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcArea;
    rc.top += GetSystemMetrics(SM_CXICON)/2;

    TCHAR szTemp[2*STRING_MAX];
    m_oStr->Format(szTemp, m_oStr->GetText(ID(IDS_VERSION_Launchpad), m_hInst), VERSION_Launcher);
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
