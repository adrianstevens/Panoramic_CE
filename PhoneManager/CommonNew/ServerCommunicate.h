#pragma once

#include "phcanvas.h"
#include "IssDebug.h"
#include "IssString.h"

#define WND_HND_Dialer      _T("PanoHdnDlrWnd")
#define CLASS_HND_Dialer    _T("PanoHdnDlrClass")
#define REG_BackupOEM       _T("PanoBackup")
#define KEY_Skin            _T("Security\\Phone\\Skin")


static HRESULT DoVerbAction(PH_VERB sVerb)
{
    HRESULT hr = S_OK;
    static HWND hWndHidden = NULL;
    
    if(!hWndHidden || !IsWindow(hWndHidden))
    {
        HWND hWndToolbar = FindWindow(_T("HHTaskBar"), _T(""));
        if(hWndToolbar)
        {
            CIssString* oStr = CIssString::Instance();
            TCHAR szText[255];
            HWND hWndChild = GetWindow(hWndToolbar, GW_CHILD);
            while(hWndChild)
            {
                oStr->Empty(szText);
                GetWindowText(hWndChild, szText, 255);
                if(0 == oStr->Compare(szText, WND_HND_Dialer))
                {
                    hWndHidden = hWndChild;
                    break;
                }
                hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
            }
        }
    }
    CPHR(hWndHidden, _T("FindWindow Failed"));

    COPYDATASTRUCT cds;
    cds.dwData = 0;
    cds.cbData = sizeof(PH_VERB);
    cds.lpData = &sVerb;
    SendMessage(hWndHidden, WM_COPYDATA, 0, (LPARAM)&cds);

Error:
    return hr;
}
