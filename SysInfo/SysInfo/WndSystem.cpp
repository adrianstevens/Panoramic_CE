#include "WndSystem.h"

CWndSystem::CWndSystem(void)
{
}

CWndSystem::~CWndSystem(void)
{
}

BOOL CWndSystem::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("System"));
	Refresh();
	return TRUE;
}

void CWndSystem::Refresh()
{
	Clear();
	ShowGetSystemInfo();
}

void CWndSystem::ShowGetSystemInfo()
{
	BOOL iRet;
	OSVERSIONINFO vi;
    BOOL bPocketPC = TRUE;

	ZeroMemory(&vi, sizeof(vi));
	vi.dwOSVersionInfoSize = sizeof(vi);
	iRet = GetVersionEx(&vi);

	TCHAR szTemp[STRING_MAX];

	DWORD dwTemp;
	SystemParametersInfo( SPI_GETPLATFORMTYPE, (INT)&dwTemp, szTemp, 0 );
	OutputString(_T("Platform Type: "), szTemp);

	m_oStr->Format(szTemp, _T("%i.%i.%i"), vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber);
	OutputString(_T("OS Version:"), szTemp);

//	m_oStr->Format(szTemp, _T("%i"), vi.dwPlatformId);
//  OutputString(_T("Platform ID:"), szTemp);

	SystemParametersInfo( SPI_GETOEMINFO, STRING_MAX, szTemp, 0 );
	OutputString(_T("OEM:"), szTemp);

    //now figure out device
    //we know we're at least WM 5
    if(vi.dwMinorVersion < 2 && vi.dwMajorVersion == 5)
        OutputString(_T("Windows Mobile 5"), _T(""));
    else
        OutputString(_T("Windows Mobile 6.x"), _T(""));


}

