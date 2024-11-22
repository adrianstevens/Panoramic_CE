#include "Metronome.h"

CMetronome::CMetronome(void)
:m_hWnd(NULL)
,m_iMeter(0)
,m_iBPM(120)
,m_bOn(TRUE)
{

}

CMetronome::~CMetronome(void)
{
}

BOOL CMetronome::OnTempoDown()
{
    if(m_iBPM > MIN_BPM)
    {
        m_iBPM--;
        return TRUE;
    }
    return FALSE;
}

BOOL CMetronome::OnTempoUp()
{
    if(m_iBPM < MAX_BPM)
    {
        m_iBPM++;
        return TRUE;
    }
    return FALSE;
}

BOOL CMetronome::OnMeterUp()
{
    if(m_iMeter == 0)
    {
        m_iMeter = 2;
    }
    else if(m_iMeter < MAX_METER)
    {
        m_iMeter++;
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CMetronome::OnMeterDown()
{
    if(m_iMeter == 2)
        m_iMeter = 0;
    else if(m_iMeter > MIN_METER)
        m_iMeter--;
    else
        return FALSE;
    return TRUE;
}

BOOL CMetronome::OnTap()
{
    return FALSE;
}

BOOL CMetronome::SetBPM(int iBPM)
{
    if(iBPM >= MIN_BPM && iBPM <= MAX_BPM)
    {
        m_iBPM = iBPM;
        return TRUE;
    }
    return FALSE;
}

int CMetronome::GetBPM()
{
    return m_iBPM;
}

int CMetronome::GetTimerTiming()
{
    //a full minute divided by beats per minute ... pretty simple really
    return 60000/m_iBPM;
}

BOOL CMetronome::SetMeter(int iMeter)
{
    if(iMeter >= MIN_METER && iMeter <= MAX_METER)
    {
        m_iMeter = iMeter;
        return TRUE;
    }
    return FALSE;
}

int CMetronome::GetMeter()
{
    return m_iMeter;
}