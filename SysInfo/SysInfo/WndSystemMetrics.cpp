#include "WndSystemMetrics.h"

CWndSystemMetrics::CWndSystemMetrics(void)
{
}

CWndSystemMetrics::~CWndSystemMetrics(void)
{
}

BOOL CWndSystemMetrics::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("System Metrics"));
	Refresh();
	return TRUE;
}

void CWndSystemMetrics::Refresh()
{
	Clear();
	ShowGetSystemMetrics();
}

void CWndSystemMetrics::ShowGetSystemMetrics()
{
	

	OutputFormattedString(TEXT("# of monitors: %1!lu!"), GetSystemMetrics(SM_CMONITORS));
	OutputFormattedString(TEXT("Window border width: %1!lu!"), GetSystemMetrics(SM_CXBORDER));
	OutputFormattedString(TEXT("Window border height: %1!lu!"), GetSystemMetrics(SM_CYBORDER));
//	OutputFormattedString(TEXT("Cursor width: %1!lu!\r\n"), GetSystemMetrics(SM_CXCURSOR));
//	OutputFormattedString(TEXT("Cursor Hidth: %1!lu!\r\n"), GetSystemMetrics(SM_CYCURSOR));

    //DPI

    int iIcon = GetSystemMetrics(SM_CXICON);

    int iDPI = 0;

    switch(iIcon)
    {
    case 32:
        iDPI = 96;
    	break;
    case 44:
    case 45:
        iDPI = 128;//131 for SP ... get er later
    	break;
    case 64:
        iDPI = 192;
        break;
    default:
        iDPI = 3*iIcon;
        break;
    }

    OutputFormattedString(TEXT("DPI: %1!lu!"), iDPI);




}