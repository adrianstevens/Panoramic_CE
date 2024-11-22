#include "WndMemory.h"

CWndMemory::CWndMemory(void)
{
}

CWndMemory::~CWndMemory(void)
{
}

BOOL CWndMemory::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);

    m_oStr->StringCopy(m_szName, _T("Memory"));

    Refresh();
    
    return TRUE;
}

void CWndMemory::Refresh()
{
    Clear();
    
    //here is fine
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);

    double dbTemp       = (double)(ms.dwTotalPhys);
    double dbTotalMem	= dbTemp/1024.0;//in kbytes

    dbTemp              = (double)(ms.dwAvailPhys);
    double dbAvalMem	= dbTemp/1024.0;//in kbytes

    TCHAR szValue[STRING_MAX];
    TCHAR szName[STRING_LARGE];

    m_oStr->Format(szValue, _T("%.0f kb"), dbTotalMem);
    m_oStr->StringCopy(szName, _T("Total Memory:"));
    OutputString(szName, szValue);

    m_oStr->Format(szValue, _T("%.0f kb"), dbAvalMem);
    m_oStr->StringCopy(szName, _T("Total Free:"));
    OutputString(szName, szValue);

    double dbPcnt = dbAvalMem/dbTotalMem*100.0;
    m_oStr->Format(szValue, _T("%.2f %%"), dbPcnt);
    m_oStr->StringCopy(szName, _T("Percent Free:"));
    OutputString(szName, szValue);

}