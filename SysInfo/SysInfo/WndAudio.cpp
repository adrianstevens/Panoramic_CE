#include "WndAudio.h"

CWndAudio::CWndAudio(void)
{
}

CWndAudio::~CWndAudio(void)
{
}

BOOL CWndAudio::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Audio"));
    Refresh();
    return TRUE;
}


void CWndAudio::Refresh()
{
    TCHAR szValue[STRING_MAX];
 //   TCHAR szName[STRING_LARGE];

    Clear();

    OutputSection(_T("Wave In"));
    for(int i = 0; i < m_oSound.GetNumWaveInDevices(); i++)
    {
        m_oSound.GetWaveInDeviceName(i, szValue);
        OutputString(_T(""), szValue);
    }

    OutputSection(_T("Wave Out"));
    for(int i = 0; i < m_oSound.GetNumWaveOutDevices(); i++)
    {
        m_oSound.GetWaveOutDeviceName(i, szValue);
        OutputString(_T(""), szValue);
    }
}