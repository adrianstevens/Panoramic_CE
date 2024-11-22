#include "StdAfx.h"
#include "RepeatCounter.h"

CRepeatCounter::CRepeatCounter()
{
	m_iWaitTime = 400;		// default to 1 sec time delay
	Reset();
}

CRepeatCounter::~CRepeatCounter(void)
{
}

void CRepeatCounter::SetTimerLength(int iTime)
{
	m_iWaitTime	= iTime;
}

void CRepeatCounter::Reset()
{
	m_bHasReturnedTrue	= FALSE;
	m_dwStartcount		= 0;
	m_iLastButton		= -1;
}

BOOL CRepeatCounter::AddButtonPress(DWORD dwTime, int iButtonNumber)
{
	// check to see if the same button has been pressed
	if (iButtonNumber != m_iLastButton)
	{
		Reset();
		m_iLastButton  = iButtonNumber;
		m_dwStartcount = dwTime;
		return FALSE;
	}

	// now we have the same button pressed as last time
	// check to see if our button has been repeating long enough
	if ((int)(dwTime - m_dwStartcount) < m_iWaitTime)
	{
		return FALSE;
	}
	else
	{
		// Our button has been repeating long enough
		if(m_bHasReturnedTrue == FALSE)
		{
			// return true and set the boolean
			m_bHasReturnedTrue = TRUE;
			return TRUE;
		}
		else
		{
			// return false
			return FALSE;
		}
	}
}

BOOL CRepeatCounter::HasReturnedTrue()
{
	return m_bHasReturnedTrue;
}