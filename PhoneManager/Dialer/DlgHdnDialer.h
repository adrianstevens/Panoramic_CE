#pragma once

#include "IssWnd.h"
#include "phcanvas.h"

typedef HRESULT (* PFN_ADDLISTENER) (const PH_ID*, int, HWND, UINT, HPHONELISTENER*); 
typedef HRESULT (* PFN_REMOVELISTENER)(HPHONELISTENER);
typedef HRESULT (* PFN_DOVERB)(PH_VERB, BOOL, VPARAM);

class CDlgHdnDialer:public CIssWnd
{
public:
    CDlgHdnDialer(void);
    ~CDlgHdnDialer(void);

    void    CreateHiddenWindow(HINSTANCE hInst);
    void    DestroyHiddenWindow();

protected:
    BOOL    OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL    OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:    // variables
    HPHONELISTENER  m_hPhoneListener;
    PFN_ADDLISTENER m_pfnAddListener;
    PFN_REMOVELISTENER m_pfnRemoveListener;
    PFN_DOVERB         m_pfnDoVerb;
    HINSTANCE       m_hCanvasDll;
};
