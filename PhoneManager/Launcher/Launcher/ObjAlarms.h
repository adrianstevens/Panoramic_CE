#pragma once

#include "IssString.h"

#define NUMBER_OF_ALARMS    3

struct AlarmType
{
    DWORD   dwHours;        //24 hour clock relative to local time
    DWORD   dwMinutes;      
    DWORD   dwDays;         //we'll leave this as it is in the registry
    BOOL    bActive;        //is it set to on
    bool    bMon;           //and to make life easy
    bool    bTues;
    bool    bWed;
    bool    bThurs;
    bool    bFri;
    bool    bSat;
    bool    bSun;
};

class CObjAlarms
{
public:
    CObjAlarms(void);
    ~CObjAlarms(void);

    

private:
    HRESULT             GetAlarmInfo();//read in all three alarms
    AlarmType*          GetAlarm(int iAlarm);
    HRESULT             GetKey(TCHAR* szSubKey, 
                                TCHAR* szValue, 
                                LPBYTE lpData, 
                                DWORD& dwSize);

    BOOL                IsAlarmActiveIn24(int iAlarm);

    BOOL                IsAlarmActive();

private:
    CIssString*         m_oStr;

    AlarmType           m_sAlarms[NUMBER_OF_ALARMS];//since there's 3 ... I believe only 1 is used on smartphones but don't quote me on that




};
