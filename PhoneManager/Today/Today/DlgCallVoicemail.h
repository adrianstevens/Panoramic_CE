#pragma once

#include "DlgBase.h"
#include "IssImageSliced.h"

class CDlgCallVoicemail:public CDlgBase
{
public:
    CDlgCallVoicemail(void);
    ~CDlgCallVoicemail(void);

    void                SetVMailNumber(TCHAR* szVMail){m_szVMail = szVMail;};

protected:
    BOOL                OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL				OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL				OnLButtonDown(HWND hWnd, POINT& pt);
    //BOOL				OnMouseMove(HWND hWnd, POINT& pt);
    
private:    // functions
    void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                OnMenuLeft();
    void                OnMenuRight();
    BOOL                OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
 
private:    // variables
    CIssImageSliced			m_imgButton;            // kinetic list button
    CIssImageSliced			m_imgButtonSel;         // kinetic list selected button
    RECT                    m_rcButtonSet;
    RECT                    m_rcButtonYes;
    RECT                    m_rcButtonNo;
    RECT                    m_rcTextDial;
    BOOL                    m_bButtonSetSelected;
    BOOL                    m_bButtonNoSelected;
    TCHAR*                  m_szVMail;              // voicemail number
 
};
