#include "WndTime.h"

CWndTime::CWndTime(void)
{
}

CWndTime::~CWndTime(void)
{
}

BOOL CWndTime::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Device Time"));
	Refresh();
	return TRUE;
}

void CWndTime::Refresh()
{
	Clear();
	ShowTime();
}

void CWndTime::ShowTime()
{
	SYSTEMTIME  st;

	//local time
	
    OutputSection(_T("Local Time"));
	TIME_ZONE_INFORMATION sTZ;
	GetTimeZoneInformation(&sTZ);
	OutputString(sTZ.StandardName, _T(""));
	GetLocalTime(&st);
	OutputTime(&st);

	//system time
	GetSystemTime(&st);
    OutputSection(_T("System Time"));
	OutputTime(&st);

	//up time
    OutputSection(_T("System Up Time"));
	DWORD dwTick = GetTickCount();
	dwTick/=1000;//ms to sec
	int iDays = 0;
	int iHours = 0;
	int iMin = 0;
	int iSec = 0;

	while(dwTick > 86400)
	{
		iDays++;
		dwTick-=86400;
	}
	while(dwTick > 3600)
	{
		iHours++;
		dwTick-=3600;
	}
	while(dwTick > 60)
	{
		iMin++;
		dwTick-=60;
	}
	iSec = dwTick;
	TCHAR szTemp[STRING_LARGE];
	m_oStr->Format(szTemp, _T("%i days, %i hrs, %i min, %i sec"), iDays, iHours, iMin, iSec);
	OutputString(szTemp, _T(""));


}
