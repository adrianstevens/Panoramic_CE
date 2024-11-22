#include "DlgHdnDialer.h"
#include "ServerCommunicate.h"
#include "IssDebug.h"
#include "IssCommon.h"

#define WM_PHONE_Listener       WM_USER + 1000

CDlgHdnDialer::CDlgHdnDialer(void)
:m_hPhoneListener(NULL)
,m_hCanvasDll(NULL)
,m_pfnAddListener(NULL)
,m_pfnRemoveListener(NULL)
,m_pfnDoVerb(NULL)
{
}

CDlgHdnDialer::~CDlgHdnDialer(void)
{
}

void CDlgHdnDialer::CreateHiddenWindow(HINSTANCE hInst)
{
    HWND hWndParent = FindWindow(_T("HHTaskBar"), _T(""));
    if(CIssWnd::Create(WND_HND_Dialer, hWndParent, hInst, CLASS_HND_Dialer))
    {
        /*MSG msg;
        while( GetMessage( &msg,NULL,0,0 ) && GetWnd())
        {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }*/
    }
}

void CDlgHdnDialer::DestroyHiddenWindow()
{
    if(m_hWnd && IsWindow(m_hWnd))
        DestroyWindow(m_hWnd);
    m_hWnd = NULL;

    if(m_hPhoneListener && m_pfnRemoveListener)
        m_pfnRemoveListener(m_hPhoneListener);
    m_hPhoneListener = NULL;

    if(m_hCanvasDll)
        FreeLibrary(m_hCanvasDll);
    m_hCanvasDll = NULL;
    m_pfnAddListener = NULL;
    m_pfnRemoveListener = NULL;
    m_pfnDoVerb = NULL;
}

BOOL CDlgHdnDialer::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DBG_OUT((_T("CDlgHdnDialer::OnInitDialog")));

    m_hCanvasDll = LoadLibrary(_T("tpcutil.dll"));
    if(m_hCanvasDll)
    {
        m_pfnDoVerb        = (PFN_DOVERB)GetProcAddress(m_hCanvasDll, (LPCWSTR)46);
        m_pfnAddListener   = (PFN_ADDLISTENER)GetProcAddress(m_hCanvasDll, (LPCWSTR)47);
        m_pfnRemoveListener= (PFN_REMOVELISTENER)GetProcAddress(m_hCanvasDll, (LPCWSTR)48);        
    }

    PH_ID arrID[] = 
    {PH_ID_PHONE_STATE};
    
    if(m_pfnAddListener)
    {
        //HRESULT hr = m_pfnAddListener(arrID, sizeof(arrID)/sizeof(PH_ID), m_hWnd, WM_PHONE_Listener, &m_hPhoneListener);
    }

    return TRUE;
}

BOOL CDlgHdnDialer::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_PHONE_Listener)
    {
        DBG_OUT((_T("WM_PHONE_Listener wParam - %d, lParam - %d"), wParam, lParam));
        return TRUE;
    }

    return UNHANDLED;
}

BOOL CDlgHdnDialer::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DBG_OUT((_T("CDlgHdnDialer::OnCopyData")));

    COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
    if(!cds || !cds->lpData)
        return TRUE;

    DBG_OUT((_T("dwData - %d"), cds->dwData));
    DBG_OUT((_T("cbData - %d"), cds->cbData));
    DBG_OUT((_T("lpData - %d"), cds->lpData));

    if(cds->dwData == 0 && m_pfnDoVerb) // verb action
    {
        PH_VERB* pVerb = (PH_VERB*)cds->lpData;
        PH_VERB pMyVerb;
        memcpy(&pMyVerb, pVerb, sizeof(PH_VERB));
        m_pfnDoVerb(pMyVerb, FALSE, NULL);
    }

    return TRUE;

}
