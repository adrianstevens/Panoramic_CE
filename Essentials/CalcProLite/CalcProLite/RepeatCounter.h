#pragma once

class CRepeatCounter
{
public:
	CRepeatCounter();	
	~CRepeatCounter(void);
	void	SetTimerLength(int iTime);	// time in milliseconds
	BOOL	AddButtonPress(DWORD dwTime, int iButtonNumber);	// time from function GetTickCount()
	BOOL	HasReturnedTrue();
	void	Reset();

private:



public:


private:

	DWORD		m_dwStartcount;
	int			m_iLastButton;
	int			m_iWaitTime;
	BOOL		m_bHasReturnedTrue;



};
