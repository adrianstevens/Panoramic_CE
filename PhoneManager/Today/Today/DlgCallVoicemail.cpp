#include "DlgCallVoicemail.h"
#include "resource.h"
#include "CommonDefines.h"
#include "IssGDIDraw.h"
#include "ObjSkinEngine.h"
#include "DlgSetVoicemail.h"
#include "ObjVoicemail.h"
#include "aygshell.h"

#define VOICEMAIL_Text  _T("Edit Voicemail Number")

CDlgCallVoicemail::CDlgCallVoicemail(void)
:m_bButtonSetSelected(FALSE)
,m_bButtonNoSelected(FALSE)
,m_szVMail(NULL)
{
}

CDlgCallVoicemail::~CDlgCallVoicemail(void)
{

}

BOOL CDlgCallVoicemail::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_imgButtonSel.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_ButtonSelectedVGA:IDR_PNG_ButtonSelected);
    m_imgButton.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_ButtonVGA:IDR_PNG_Button);
    return TRUE;
}

BOOL CDlgCallVoicemail::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    HDC dc      = GetDC(hWnd);
    HFONT hOld  = (HFONT)SelectObject(dc, m_hFontLabel);
    SIZE sz;
    GetTextExtentPoint(dc, VOICEMAIL_Text, m_oStr->GetLength(VOICEMAIL_Text), &sz);
    SelectObject(dc, hOld);
    ReleaseDC(hWnd, dc);

    int iButtonHeight   = HEIGHT(m_rcTitle)*3/2;
    int iContentHeight  = 5*iButtonHeight;
    int iYStart         = m_rcArea.top + (HEIGHT(m_rcArea) - iContentHeight)/2;
    int iYesNoWidth     = WIDTH(m_rcArea)/4;

    m_rcTextDial        = m_rcArea;
    m_rcTextDial.top    = iYStart;
    m_rcTextDial.bottom = m_rcTextDial.top + iButtonHeight;

    m_rcButtonYes.top   = iYStart + 1*iButtonHeight;
    m_rcButtonYes.bottom= m_rcButtonYes.top + iButtonHeight;
    m_rcButtonYes.right = m_rcArea.right/2 - 3*INDENT;
    m_rcButtonYes.left  = m_rcButtonYes.right - iYesNoWidth;
    m_rcButtonNo        = m_rcButtonYes;
    m_rcButtonNo.left   = m_rcArea.right/2 + 3*INDENT;
    m_rcButtonNo.right  = m_rcButtonNo.left + iYesNoWidth;

    m_rcButtonSet.top   = iYStart + 4*iButtonHeight;
    m_rcButtonSet.bottom= m_rcButtonSet.top + iButtonHeight;
    m_rcButtonSet.left  = m_rcArea.left + (WIDTH(m_rcArea) - sz.cx - 6*INDENT)/2;
    m_rcButtonSet.right = m_rcButtonSet.left + sz.cx + 6*INDENT;

    return TRUE;
}

BOOL CDlgCallVoicemail::OnLButtonDown(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(PtInRect(&m_rcButtonSet, pt))
    {
        m_bButtonSetSelected = TRUE;
        InvalidateRect(hWnd, &m_rcButtonSet, FALSE);
        return TRUE;
    }
    else if(PtInRect(&m_rcButtonNo, pt))
    {
        m_bButtonNoSelected = TRUE;
        InvalidateRect(hWnd, &m_rcButtonNo, FALSE);
        return TRUE;
    }
    else if(PtInRect(&m_rcButtonYes, pt))
    {
        SafeCloseWindow(IDOK);
        return TRUE;
    }

    return CDlgBase::OnLButtonDown(hWnd, pt);
}

BOOL CDlgCallVoicemail::OnLButtonUp(HWND hWnd, POINT& pt)
{

    if(m_bButtonSetSelected && PtInRect(&m_rcButtonSet, pt))
    {
        SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
        // do the reset stuff
        CDlgSetVoicemail dlgVoice;
        if(dlgVoice.DoModal(m_hWnd, m_hInst, IDD_DLG_Modal) == IDOK)
        {
            if(m_oStr->GetLength(dlgVoice.GetVmailNumber()))
            {
                // save the new voicemail number in the registry
                CObjVoicemail objVmail;
                objVmail.SetVmailNumber(dlgVoice.GetVmailNumber());
                m_oStr->StringCopy(m_szVMail, dlgVoice.GetVmailNumber());
            }
        }
        m_bButtonSetSelected = FALSE;
        InvalidateRect(hWnd, &m_rcButtonSet, FALSE);
    }
    else if(m_bButtonNoSelected && PtInRect(&m_rcButtonNo, pt))
    {
        m_bButtonNoSelected  = FALSE;
        InvalidateRect(hWnd, &m_rcButtonNo, FALSE);
        SafeCloseWindow(IDCANCEL);
    }

    m_bButtonSetSelected = FALSE;
    m_bButtonNoSelected  = FALSE;
    return CDlgBase::OnLButtonUp(hWnd, pt);
}

void CDlgCallVoicemail::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    TCHAR szTopBar[STRING_MAX];
    m_oStr->Format(szTopBar, _T("Voicemail - %s"), (m_oStr->GetLength(m_szVMail)>0?m_szVMail:_T("Undefined")));
    DrawText(gdi, szTopBar, rc, DT_LEFT | DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(51,51,51));
}

void CDlgCallVoicemail::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    //DrawTextShadow(gdi, _T("Yes"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgCallVoicemail::OnMenuLeft()
{
   SafeCloseWindow(IDOK);
}

void CDlgCallVoicemail::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

BOOL CDlgCallVoicemail::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(IsRectInRect(m_rcButtonSet, rcClip))
    {
        CIssImageSliced* img = m_bButtonSetSelected?&m_imgButtonSel:&m_imgButton;
        if(WIDTH(m_rcButtonSet) != img->GetWidth() || HEIGHT(m_rcButtonSet) != img->GetHeight())
            img->SetSize(WIDTH(m_rcButtonSet), HEIGHT(m_rcButtonSet));
        img->DrawImage(gdi, m_rcButtonSet.left, m_rcButtonSet.top);

        DrawTextShadow(gdi, VOICEMAIL_Text, m_rcButtonSet, DT_VCENTER|DT_CENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }

    if(IsRectInRect(m_rcButtonNo, rcClip))
    {
        CIssImageSliced* img = m_bButtonNoSelected?&m_imgButtonSel:&m_imgButton;
        if(WIDTH(m_rcButtonNo) != img->GetWidth() || HEIGHT(m_rcButtonNo) != img->GetHeight())
            img->SetSize(WIDTH(m_rcButtonNo), HEIGHT(m_rcButtonNo));
        img->DrawImage(gdi, m_rcButtonNo.left, m_rcButtonNo.top);

        DrawTextShadow(gdi, _T("No"), m_rcButtonNo, DT_VCENTER|DT_CENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }

    if(IsRectInRect(m_rcButtonYes, rcClip))
    {
        CIssImageSliced* img = &m_imgButtonSel;
        if(WIDTH(m_rcButtonYes) != img->GetWidth() || HEIGHT(m_rcButtonYes) != img->GetHeight())
            img->SetSize(WIDTH(m_rcButtonYes), HEIGHT(m_rcButtonYes));
        img->DrawImage(gdi, m_rcButtonYes.left, m_rcButtonYes.top);

        DrawTextShadow(gdi, _T("Yes"), m_rcButtonYes, DT_VCENTER|DT_CENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }

    DrawTextShadow(gdi, _T("Dial Voicemail?"), m_rcTextDial, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    return TRUE;
}

BOOL CDlgCallVoicemail::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    /*switch(wParam)
    {
    case IDMENU_Items:
        break;
    case IDMENU_None:
        break;
    default:
        return CDlgBase::OnCommand(hWnd, wParam, lParam);
    }*/
    return TRUE;
}



