#pragma once

#include "stdafx.h"
#include "IssString.h"
#include "IssVector.h"

enum EnumDaysOfTheWeek
{
    DAY_Sunday,
    DAY_Monday,
    DAY_Tuesday,
    DAY_Wednesday,
    DAY_Thursday,
    DAY_Friday,
    DAY_Saturday
}; //probably won't need this ...

enum EnumSnoozeTimes
{
    SNOOZE_5,
    SNOOZE_10,
    SNOOZE_15,
    SNOOZE_20,
    SNOOZE_30,
    SNOOZE_60,
    SNOOZE_Count,
};

enum EnumAlarmType
{
    ALARM_MP3,
    ALARM_Wave,
};

struct AlarmType
{
    BOOL            Active;
    int             iAlarmHour;
    int             iAlarmMin;
    int             iAlarmDays;//days of the week
    EnumAlarmType   eType;    //is it a wave file or mp3
    TCHAR           szAlarm[MAX_PATH];//name of file to play
};

#define SUNDAY      1
#define MONDAY      2
#define TUESDAY     4
#define WEDNESDAY   8
#define THURSDAY    16
#define FRIDAY      32
#define SATURDAY    64


class CIssAlarmClock
{
public:
    CIssAlarmClock(void);
    ~CIssAlarmClock(void);

    BOOL                    GetTime(TCHAR* szTime);
    BOOL                    GetDate(TCHAR* szDate);
    BOOL                    IsAlarmSet();

    int                     GetAlarmCount();
    BOOL                    AddAlarm(AlarmType* sAlarm);

    BOOL                    OnSnooze();
    BOOL                    OnStopAlarm();

    AlarmType*              GetNextAlarm(); // get the time of the next alarm to be played

    BOOL                    PlayAlarm();

private:
    void                    Destroy();

private:
    CIssString*             m_oStr;
    CIssVector<AlarmType>   m_arrAlarms;

};
