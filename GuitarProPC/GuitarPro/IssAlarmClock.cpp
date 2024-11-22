#include "IssAlarmClock.h"

CIssAlarmClock::CIssAlarmClock(void)
:m_oStr(NULL)
{
    m_oStr = CIssString::Instance();
}

CIssAlarmClock::~CIssAlarmClock(void)
{
}

void CIssAlarmClock::Destroy()
{
    AlarmType* sType = NULL;

    for(int i = 0; i < m_arrAlarms.GetSize(); i++)
    {
        sType = m_arrAlarms[i];
        delete sType;
        sType = NULL;
    }

    m_arrAlarms.RemoveAll();
}

BOOL CIssAlarmClock::GetTime(TCHAR* szTime)
{
    if(szTime = NULL)
        return FALSE;

    SYSTEMTIME sTime;
    GetLocalTime(&sTime);

    m_oStr->Format(szTime, _T("%.2d:%.2d:%.2d%.4d"), sTime.wHour, sTime.wMinute, sTime.wSecond);

    return TRUE;
}

BOOL CIssAlarmClock::GetDate(TCHAR* szDate)
{
    if(szDate = NULL)
        return FALSE;

 //   SYSTEMTIME sTime;
 //   GetLocalTime(&sTime);

 //   m_oStr->Format(szTime, _T("%.2d:%.2d:%.2d%.4d"), sTime.wHour, sTime.wMinute, sTime.wSecond);

    return FALSE;
}

BOOL CIssAlarmClock::IsAlarmSet()
{
    AlarmType* sType = NULL;

    for(int i = 0; i < m_arrAlarms.GetSize(); i++)
    {
        sType = m_arrAlarms[i];

        if(sType)
            if(sType->Active == TRUE)
                return TRUE;
    }
    return FALSE;
}

int CIssAlarmClock::GetAlarmCount()
{
    return m_arrAlarms.GetSize();
}

BOOL CIssAlarmClock::AddAlarm(AlarmType* sAlarm)
{
    return FALSE;
}

BOOL CIssAlarmClock::OnSnooze()
{
    return FALSE;

}

BOOL CIssAlarmClock::OnStopAlarm()
{
    return FALSE;
}

AlarmType* CIssAlarmClock::GetNextAlarm()
{
    return NULL;
}

