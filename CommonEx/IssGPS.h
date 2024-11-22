#pragma once
#include "stdafx.h"
#include "gpsapi.h"
#include "IssString.h"

class CIssGPS
{
public:
    CIssGPS(void);
    ~CIssGPS(void);

    BOOL            GetGPSPosition(TCHAR* szPos, DWORD dwMaxAge = INFINITE);

    //public in case you want to open or close early
    //not required to be called first
    BOOL            IsGPSOn(){return m_hGPS?TRUE:FALSE;};
    BOOL            OpenGPS();
    void            CloseGPS();

    BOOL            GetDeviceInfo();
    DWORD           GetPosInfo(DWORD dwMaxAge = INFINITE);
    DWORD           GetLastAccessed(){return m_dwLastAccessed;};


    //GPS position information
    DWORD           GetVersion(){GetPosInfo(); return m_sPosition.dwVersion;};//we set this 

    double          GetLong(){GetPosInfo();return m_sPosition.dblLongitude;};
    double          GetLat(){GetPosInfo();return m_sPosition.dblLatitude;};
    
    float           GetSpeed(){GetPosInfo();return m_sPosition.flSpeed;}; //in knots
    float           GetHeading(){GetPosInfo();return m_sPosition.flHeading;};//in degrees
    float           GetAltitude(){GetPosInfo();return m_sPosition.flAltitudeWRTSeaLevel;};//sea level of course

    DWORD           GetNumSatellites(){GetPosInfo(); return m_sPosition.dwSatellitesInView;};

    SYSTEMTIME      GetGPSTime(){GetPosInfo();return m_sPosition.stUTCTime;};
    GPS_POSITION&   GetCurrentPosition(){return m_sPosition;};


    //GPS device information


private:
    void            Reset();
    

public:
    //lazyness for now
    GPS_DEVICE      m_sDevice;

    //just in case you want the rest of the info ... no real harm
    GPS_POSITION    m_sPosition;

private:
    CIssString*     m_oStr;
    HANDLE          m_hGPS;
    DWORD           m_dwLastAccessed;       // last time the GPS was accessed


    


};
