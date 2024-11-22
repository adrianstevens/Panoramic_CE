#include "WndGPS.h"

CWndGPS::CWndGPS(void)
{
}

CWndGPS::~CWndGPS(void)
{
}

BOOL CWndGPS::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("GPS"));
    //open the GPS right away
    m_oGPS.OpenGPS();

    Refresh();
    return TRUE;
}

void CWndGPS::Refresh()
{
    Clear();

    TCHAR szTemp[STRING_MAX];

    if(m_oGPS.OpenGPS() == TRUE)
    {
        OutputSection(_T("GPS Location"));
        m_oStr->Format(szTemp, _T("%.6f"), m_oGPS.GetAltitude());
        OutputString(_T("Altitude:"), szTemp);
        m_oStr->Format(szTemp, _T("%.6f"), m_oGPS.GetLat());
        OutputString(_T("Latitude:"), szTemp);
        m_oStr->Format(szTemp, _T("%.6f"), m_oGPS.GetLong());
        OutputString(_T("Longitude:"), szTemp);
        m_oStr->Format(szTemp, _T("%.2f knots"), m_oGPS.GetSpeed());
        OutputString(_T("Speed:"), szTemp);
        m_oStr->Format(szTemp, _T("%i"), m_oGPS.GetNumSatellites());
        OutputString(_T("# Satellites:"), szTemp);
    }
    else
    {
        OutputString( _T("Error"), _T("Failed to open GPS"));
    }

    //lets get some info
    //safe to call as much as you want
    if(m_oGPS.GetDeviceInfo() == TRUE)
    {
        OutputSection(_T("GPS Device"));

        GPS_DEVICE* oGPS = &m_oGPS.m_sDevice;
        SYSTEMTIME time;

        if(oGPS == NULL)
            return; //that's bad

        m_oStr->IntToString(szTemp, oGPS->dwVersion);
        OutputString(_T("GPS Version:"), szTemp);

        OutputString(oGPS->szGPSFriendlyName, _T(""));

        switch(oGPS->dwDeviceState)
        {
        case 0://SERVICE_STATE_OFF :
            m_oStr->StringCopy(szTemp, _T("Off"));
        	break;
        case 1: //SERVICE_STATE_ON:
            m_oStr->StringCopy(szTemp, _T("On"));
        	break;
        case 2:// SERVICE_STATE_STARTING_UP :
            m_oStr->StringCopy(szTemp, _T("Starting up"));
            break;
        case 3: //SERVICE_STATE_SHUTTING_DOWN :
            m_oStr->StringCopy(szTemp, _T("Shutting down"));
            break;
        case 4: //SERVICE_STATE_UNLOADING:
            m_oStr->StringCopy(szTemp, _T("Unloading"));
        	break;
        case 5: //SERVICE_STATE_UNINITIALIZED:
            m_oStr->StringCopy(szTemp, _T("Uninitialized"));
        	break;
        case 0xffffffff: //SERVICE_STATE_UNKNOWN:
            m_oStr->StringCopy(szTemp, _T("Unknown"));
        default:
            break;
        }

        OutputString(_T("Device State:"), szTemp);

        switch(oGPS->dwServiceState)
        {
        case 0://SERVICE_STATE_OFF :
            m_oStr->StringCopy(szTemp, _T("Off"));
            break;
        case 1: //SERVICE_STATE_ON:
            m_oStr->StringCopy(szTemp, _T("On"));
            break;
        case 2:// SERVICE_STATE_STARTING_UP :
            m_oStr->StringCopy(szTemp, _T("Starting up"));
            break;
        case 3: //SERVICE_STATE_SHUTTING_DOWN :
            m_oStr->StringCopy(szTemp, _T("Shutting down"));
            break;
        case 4: //SERVICE_STATE_UNLOADING:
            m_oStr->StringCopy(szTemp, _T("Unloading"));
            break;
        case 5: //SERVICE_STATE_UNINITIALIZED:
            m_oStr->StringCopy(szTemp, _T("Uninitialized"));
            break;
        case 0xffffffff: //SERVICE_STATE_UNKNOWN:
            m_oStr->StringCopy(szTemp, _T("Unknown"));
        default:
            break;
        }

        OutputString(_T("Service State:"), szTemp);



        OutputSection(_T("Last Updated"));
        FileTimeToSystemTime(&oGPS->ftLastDataReceived, &time);
        OutputTime(&time);



    }
    else
    {
        OutputString( _T("Error"), _T("Failed to find GPS"));
    }

   


}