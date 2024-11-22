#include "WndPower.h"

CWndPower::CWndPower(void)
{
}

CWndPower::~CWndPower(void)
{
}

BOOL CWndPower::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Power"));
    Refresh();
    return TRUE;
}


void CWndPower::Refresh()
{
    Clear();

    TCHAR szTemp[STRING_MAX];

    SYSTEM_POWER_STATUS_EX2 sStatus;

    if(GetSystemPowerStatusEx2(&sStatus, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE))
    {
        if(sStatus.BatteryLifePercent == BATTERY_PERCENTAGE_UNKNOWN)
            m_oStr->StringCopy(szTemp, _T("Unknown"));
        else
            m_oStr->Format(szTemp, _T("%u%%"), sStatus.BatteryLifePercent);
        OutputString(_T("Battery charge:"), szTemp);

        DWORD dwTemp = sStatus.BatteryLifeTime;
        DWORD dwHours = 0;
        DWORD dwMin = 0;
        DWORD dwSec = 0;

        dwHours = dwTemp/(3600);
        dwTemp = dwTemp%(3600);

        dwMin = dwTemp/60;
        dwTemp = dwTemp%60;

        dwSec = dwTemp;

//        m_oStr->Format(szTemp, _T("%us"), sStatus.BatteryLifeTime);
        if(sStatus.BatteryLifeTime != -1)
        {
            m_oStr->Format(szTemp, _T("%ih:%im:%is"), dwHours, dwMin, dwSec);
            OutputString(_T("Charge remaining:"), szTemp);
        }
            
        double dbTemp = sStatus.BatteryTemperature/10.0;
        m_oStr->Format(szTemp, _T("%.1f °C"), dbTemp);
        OutputString(_T("Battery temp:"), szTemp);

        m_oStr->Format(szTemp, _T("%umV"), sStatus.BatteryVoltage);
        OutputString(_T("Battery Voltage:"), szTemp);

        if(sStatus.BackupBatteryLifePercent == BATTERY_PERCENTAGE_UNKNOWN)
            m_oStr->StringCopy(szTemp, _T("Unknown"));
        else
            m_oStr->Format(szTemp, _T("%u%%"), sStatus.BackupBatteryLifePercent);
        OutputString(_T("Backup remaining:"), szTemp);

        switch(sStatus.BatteryChemistry)
        {
        case BATTERY_CHEMISTRY_ALKALINE :
            OutputString(_T("Batt. Type:"), _T("Alkaline"));
        	break;
        case BATTERY_CHEMISTRY_NICD :
            OutputString(_T("Batt. Type:"), _T("Nickel-cadmium"));
        	break;
        case BATTERY_CHEMISTRY_NIMH:
            OutputString(_T("Batt. Type:"), _T("Nickel-metal hydride"));
            break;
        case BATTERY_CHEMISTRY_LION :
            OutputString(_T("Batt. Type:"), _T("Lithium Ion"));
            break;
        case BATTERY_CHEMISTRY_LIPOLY :
            OutputString(_T("Batt. Type:"), _T("Lithium polymer"));
        	break;
        case BATTERY_CHEMISTRY_UNKNOWN :
        default:
            OutputString(_T("Unknown battery type"), _T(""));
            break;
        }
    }
}

/*
SPI_GETBATTERYIDLETIMEOUT 
Gets the amount of time that Windows CE will stay on with battery power 
before it suspends due to user inaction. The pvParam parameter points 
to a DWORD that returns the time in seconds. 
This flag is ignored if pvParam is zero. 
*/
/*BOOL spiGETBATTERYIDLETIMEOUT(DWORD *timeout){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  

	fRetStatus = SystemParametersInfo( SPI_GETBATTERYIDLETIMEOUT, uiParam, timeout, 0 );
	return fRetStatus;
} */
/*
SPI_GETEXTERNALIDLETIMEOUT 
Gets the amount of time that Windows CE will stay on with AC power 
before it suspends due to user inaction. The pvParam parameter 
points to a DWORD that returns the time in seconds. 
This flag is ignored if pvParam is zero. 
*/
/*DWORD spiGETEXTERNALIDLETIMEOUT (DWORD *timeout){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  

	fRetStatus = SystemParametersInfo( SPI_GETEXTERNALIDLETIMEOUT, uiParam, timeout, 0 );
	return fRetStatus;
} */
/*
SPI_GETWAKEUPIDLETIMEOUT 
Gets the amount of time that Windows CE will stay on after a user notification 
that reactivates a suspended device. The pvParam parameter points to a DWORD 
that returns the time in seconds. This flag is ignored if pvParam is zero. 
*/
/*DWORD spiGETWAKEUPIDLETIMEOUT (DWORD *timeout){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  

	fRetStatus = SystemParametersInfo( SPI_GETWAKEUPIDLETIMEOUT, uiParam, timeout, 0 );
	return fRetStatus;
} */
/*
SPI_SETBATTERYIDLETIMEOUT 
Sets the amount of time that Windows CE will stay on with battery power 
before it suspends due to user inaction. The Windows CE operating system 
will remain on, with battery power, as long as the keyboard or touch screen 
is active. The uiParam parameter specifies the time to set in seconds. 
This flag is ignored if uiParam is set to zero. 
*/
/*DWORD  spiSETBATTERYIDLETIMEOUT (DWORD *timeout){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  

	fRetStatus = SystemParametersInfo( SPI_SETBATTERYIDLETIMEOUT, uiParam, timeout, 0 );
	return fRetStatus;
} */
/*
SPI_SETEXTERNALIDLETIMEOUT 
Sets the amount of time that Windows CE will stay on with AC power before 
it suspends due to user inaction. The Windows CE operating system will remain on, 
with AC power, as long as the keyboard or touch screen is active. 
The uiParam parameter specifies the time in seconds. This flag is ignored 
if uiParam is set to zero. 
*/
/*DWORD spiSETEXTERNALIDLETIMEOUT (DWORD *timeout){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  

	fRetStatus = SystemParametersInfo( SPI_SETEXTERNALIDLETIMEOUT, uiParam, timeout, 0 );
	return fRetStatus;
} */
/*
SPI_SETWAKEUPIDLETIMEOUT 
Sets the amount of time that Windows CE will stay on after a user notification 
that reactivates the suspended device. The uiParam parameter specifies the time in seconds. This flag is ignored if uiParam is set to zero. 
*/
/*DWORD spiSETWAKEUPIDLETIMEOUT (){
	BOOL fRetStatus = FALSE;
	UINT uiParam = 0;  
	PVOID pvParam = NULL;

	fRetStatus = SystemParametersInfo( SPI_SETWAKEUPIDLETIMEOUT, uiParam, pvParam, 0 );
	return fRetStatus;
} */