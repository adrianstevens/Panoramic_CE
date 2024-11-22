#include "IssGPS.h"
#include "issdebug.h"

CIssGPS::CIssGPS(void)
:m_hGPS(NULL)
,m_oStr(CIssString::Instance())
{
    Reset();
}

CIssGPS::~CIssGPS(void)
{
    CloseGPS();
}

void CIssGPS::Reset()
{
    ZeroMemory(&m_sPosition, sizeof(GPS_POSITION));
    m_sPosition.dwSize = sizeof(GPS_POSITION);
    m_sPosition.dwVersion = GPS_VERSION_1;

    ZeroMemory(&m_sDevice, sizeof(GPS_DEVICE));
    m_sDevice.dwSize = sizeof(GPS_DEVICE);
    m_sDevice.dwVersion = GPS_VERSION_1;

    m_dwLastAccessed = 0;
}

DWORD CIssGPS::GetPosInfo(DWORD dwMaxAge)
{
    //lets try and open just in case it hasn't been started
    if(!OpenGPS())
        return ERROR_INVALID_HANDLE;

    ZeroMemory(&m_sPosition, sizeof(GPS_POSITION));
    m_sPosition.dwSize = sizeof(GPS_POSITION);
    m_sPosition.dwVersion = GPS_VERSION_1;

    return  GPSGetPosition(m_hGPS, &m_sPosition, dwMaxAge, 0);
}

BOOL CIssGPS::GetGPSPosition(TCHAR* szPos, DWORD dwMaxAge /* = INFINITE */)
{
    if(szPos == NULL)
        return FALSE;

    //lets try and open just in case it hasn't been started
    if(!OpenGPS())
        return FALSE;

    ZeroMemory(&m_sPosition, sizeof(GPS_POSITION));
    m_sPosition.dwSize = sizeof(GPS_POSITION);
    m_sPosition.dwVersion = GPS_VERSION_1;

    DWORD dwReturn = GetPosInfo(dwMaxAge);
    if(ERROR_SUCCESS == dwReturn)// && !(sPosition.dblLatitude == 0.00 && sPosition.dblLongitude == 0.00))
    {
        m_oStr->Format(szPos, _T("%Lf,%Lf"), m_sPosition.dblLatitude, m_sPosition.dblLongitude);
    }
    else
    {
        //could pass in an ID so we can localize
        m_oStr->StringCopy(szPos, _T("Error reading GPS location"));
        return FALSE;
    }
    
    return TRUE;
}

BOOL CIssGPS::OpenGPS()
{
    //already opened
    if(m_hGPS != NULL)
        return TRUE;

    DBG_OUT((_T("CIssGPS::OpenGPS()")));
    m_hGPS = GPSOpenDevice(NULL, NULL, NULL, 0);

    //obviously didn't work
    if(m_hGPS == NULL)
        return FALSE;

    m_dwLastAccessed = GetTickCount();

    return TRUE;
}

void CIssGPS::CloseGPS()
{
    if(m_hGPS == NULL)
        return;

    DBG_OUT((_T("CIssGPS::CloseGPS()")));
    GPSCloseDevice(m_hGPS);
    m_hGPS = NULL;
    m_dwLastAccessed = 0;
}

BOOL CIssGPS::GetDeviceInfo()
{
    ZeroMemory(&m_sDevice, sizeof(GPS_DEVICE));

    m_sDevice.dwVersion = GPS_VERSION_1;
    m_sDevice.dwSize = sizeof(GPS_DEVICE);

    if(ERROR_SUCCESS != GPSGetDeviceState(&m_sDevice))
        return FALSE;

    return TRUE;
}
