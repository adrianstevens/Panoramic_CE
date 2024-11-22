#include "StdAfx.h"
#include "ListScroller.h"
#include "IssCommon.h"
#include "IssGDIEx.h"


#define MAX_SPEED			1000.0
#define MIN_ACTIVATE		0.1
#define MIN_SPEED			0.01
#define IDT_SROLL_TIMER		1

#define UPDATE_TIME			0


#define	FRICTION			30.0
#define SPEED_MULT			50.0
#define REFRESH_RATE		22

#define INC_ACCEL_FACTOR	1.2f

CListScroller::CListScroller(void)
: m_iScrollPos(0)
, m_dbSpeed(0.0) 
, m_dbFriction(FRICTION)
, m_eScroll(SCROLL_Stopped)
, m_eScrollDir(DIR_Down)
, m_iLastReDrawInc(0)
{
}

CListScroller::~CListScroller(void)
{
}

BOOL CListScroller::OnLButtonDown(POINT pt)
{
	if(PtInRect(&GetOwnerFrame(), pt) == FALSE)
		return FALSE;

	m_iScrollYStart = pt.y;
	m_iScrollYPos	= pt.y;
	m_dbSpeed		= 0.0;
	m_dwStartTime	= GetTickCount();
	KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);

	return TRUE;
}

BOOL CListScroller::OnLButtonUp(POINT pt)
{
	if(PtInRect(&GetOwnerFrame(), pt) == FALSE)
		return FALSE;

	if(m_eScroll == SCROLL_Scrolling)
	{
		m_dwStopTime = GetTickCount();
		int iPos = pt.y;

		//m_dbSpeed = (pt.y - m_iScrollYStart) * SPEED_MULT / (m_dwStopTime - m_dwStartTime);

		int iTicks = m_dwStopTime - m_dwStartTime;

		iTicks > 0 ?  iTicks = iTicks : iTicks = 1;
		m_dbSpeed = (pt.y - m_iScrollYStart) * SPEED_MULT;
		m_dbSpeed /= (float)iTicks;

		if(fabs(m_dbSpeed) > MIN_ACTIVATE)
		{
			if(m_dbSpeed > MAX_SPEED)
				m_dbSpeed = MAX_SPEED;
			else if(m_dbSpeed < MAX_SPEED * -1.0)
				m_dbSpeed = MAX_SPEED*-1.0;

			m_eScroll = SCROLL_Moving;
			//DebugOut(_T("CListScroller::OnLButtonUp() SCROLL_Scrolling - SCROLL_Moving %.2f "), m_dbSpeed);
			SetTimer(GetOwnerWindow(), IDT_SROLL_TIMER, REFRESH_RATE, NULL);
		}
		else
		{
			m_eScroll = SCROLL_Stopped;
			m_dbSpeed = 0.0;

		}
		return TRUE;
	}
	else if(m_eScroll == SCROLL_Moving)
	{
		m_eScroll = SCROLL_Stopped;
		m_dbSpeed = 0.0;

		return TRUE;
	}
	else if(m_eScroll == SCROLL_Stopped)
	{
		return TRUE;
		//SeleteItem(pt.y + m_iScrollPos);
	}
	else if(m_eScroll == SCROLL_To)	
	{
		m_eScroll = SCROLL_Stopped;
	}

	return -1;	
}

BOOL CListScroller::OnMouseMove(POINT pt)
{
	if(m_eScroll != SCROLL_Scrolling)
	{
		if( abs(pt.y-m_iScrollYStart) > 5)
			m_eScroll = SCROLL_Scrolling; //since we're actually dragging
	}	

	ReDrawList(pt.y - m_iScrollYPos);

	if(pt.y < m_iScrollYPos)
	{
		if(m_eScrollDir != DIR_Down)
		{
			m_dwStartTime = GetTickCount();
			m_iScrollYStart = pt.y;
		}
		//DebugOut(_T("setting m_eScrollDir = DIR_Down"));
		m_eScrollDir = DIR_Down;
	}
	else if(pt.y > m_iScrollYPos)
	{
		if(m_eScrollDir != DIR_Up)
		{
			m_dwStartTime = GetTickCount();
			m_iScrollYStart = pt.y;
		}
		//DebugOut(_T("setting m_eScrollDir = DIR_Up"));
		m_eScrollDir = DIR_Up;
	}

	m_iScrollYPos = pt.y;

	return TRUE;
}

BOOL CListScroller::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == IDT_SROLL_TIMER)
	{
		if(m_eScroll == SCROLL_To)
		{
			m_iScrollPos = GetNextScrollToPos();
			if(ScrollToDestReached())
			{
				// we're done
				KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
				m_eScroll = SCROLL_Stopped;
				m_iScrollPos = m_iScrollToDest;
			}
			ReDrawList();

			return FALSE;
		}

		if (fabs(m_dbSpeed) <= 0.01)
		{
			KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
			m_eScroll = SCROLL_Stopped;//since we're no longer moving
			m_dbSpeed = 0;
		} 
		else 
		{
			m_eScroll = SCROLL_Moving;
			m_dbSpeed -= m_dbSpeed / m_dbFriction;
			if(m_iLastReDrawInc != (int)m_dbSpeed)
			{
				// prevent unnecessary draws
				m_iLastReDrawInc = (int)m_dbSpeed;
				ReDrawList(m_iLastReDrawInc);
			}
		}
		return 0;
	}

	return -1;//UNHANDLED;
}

int	CListScroller::GetNextScrollToPos()
{
	static float fInc = 0.0;

	// 1)calculate the scroll increment once at beginning of scroll
	if(m_iScrollToStart == m_iScrollPos)
	{
		// calculate increment
		fInc = CalcScrollIncrement();
		if(m_eScrollDir == DIR_Up)
			fInc *= -1.0;
	}

	// 2) adjust the increment for where in the scroll area we are
	// first 1/3 of travel = accelerating
	BOOL bAccelerate = FALSE;
	if(m_eScrollDir == DIR_Down)
	{
		if((m_iScrollPos /*+ (int)fInc*/ - m_iScrollToStart) < (m_iScrollToDest - m_iScrollToStart) / 3)
			bAccelerate = TRUE;
	}
	else
	{
		if((m_iScrollToStart - m_iScrollPos /*+ (int)fInc*/) < (m_iScrollToStart - m_iScrollToDest) / 3)
			bAccelerate = TRUE;
	}

	// last 1/3 of travel = accelerating
	BOOL bDeccelerate = FALSE;
	if(m_eScrollDir == DIR_Down)
	{
		if((m_iScrollPos - m_iScrollToStart) > (m_iScrollToDest - m_iScrollToStart) * 2 / 3)
			bDeccelerate = TRUE;
	}
	else
	{
		if((m_iScrollToStart - m_iScrollPos) > (m_iScrollToStart - m_iScrollToDest) * 2 / 3)
			bDeccelerate = TRUE;
	}

	if(bAccelerate)
		fInc *= INC_ACCEL_FACTOR;
	if(bDeccelerate)
		fInc /= INC_ACCEL_FACTOR;

	// ensure fInc >= 1.0
	if(m_eScrollDir == DIR_Down)
		fInc = fInc > (float)1.0 ? fInc : (float)1.0;
	else
		fInc = fInc < (float)-1.0 ? fInc : (float)-1.0;

	//DebugOut(_T("increment: %.3f pos: %i %s%s"), 
	//	fInc, m_iScrollPos + (int)fInc, 
	//	bAccelerate ? _T("accelerate") : _T(""), 
	//	bDeccelerate ? _T("deccelerate") : _T(""));


	return m_iScrollPos + (int)fInc;
}

BOOL CListScroller::ScrollToDestReached()
{
	if(m_eScrollDir == DIR_Down && m_iScrollPos >= m_iScrollToDest)
		return TRUE;
	if(m_eScrollDir == DIR_Up && m_iScrollPos <= m_iScrollToDest)
		return TRUE;

	return FALSE;
}
	
float CListScroller::CalcScrollIncrement()
{
	// fill this in
	return 4.0;
}

void CListScroller::ReDrawList(int iScrollAmount)
{
	RECT rc = GetOwnerFrame();

	int iBottom = max(0, GetEndOfListYPos() - HEIGHT(rc)/2);

	int iLastScrollPos = m_iScrollPos;
	m_iScrollPos -= iScrollAmount;
	//bounds checking
	if(m_iScrollPos < 0)
	{
		m_iScrollPos = 0;
		m_dbSpeed = 0;
		KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
	}
	else if(m_iScrollPos > iBottom)
	{
		m_dbSpeed = 0;
		m_iScrollPos = iBottom;
	}

	if(m_iScrollPos != iLastScrollPos)
		ReDrawList();
}

void CListScroller::ScrollList()
{
	if(m_eScroll != SCROLL_Moving)
		return;

	int iScrollAmnt = 0;

	RECT rc;
	GetClientRect(GetOwnerWindow(), &rc);

	int iBottom = GetEndOfListYPos() - GetSystemMetrics(SM_CYSCREEN)/2;

	while(m_dbSpeed != 0.0)
	{
		if(fabs(m_dbSpeed) < MIN_SPEED)
		{
			m_dbSpeed = 0.0;
			m_eScroll = SCROLL_Stopped;
			return;
		}
		else
		{ //we're scrolling
			m_eScroll = SCROLL_Moving;
			m_dbSpeed -= m_dbSpeed/FRICTION;
			iScrollAmnt = (int)m_dbSpeed;

			m_iScrollPos -= iScrollAmnt;
			//bounds checking
			if(m_iScrollPos < 0)
			{
				m_iScrollPos = 0;
				m_dbSpeed = 0.0;
			}
			else if(m_iScrollPos > iBottom)
			{
				m_dbSpeed = 0.0;
				m_iScrollPos = iBottom;
			}

			ReDrawList();
		}

		//if(ShouldAbort(GetOwnerWindow()))
		//	break;
	}
}

void CListScroller::StopScrolling()
{ 
	m_dbSpeed = 0;
	KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
	m_eScroll = SCROLL_Stopped; 
}


void CListScroller::ScrollTo(int iYPos, BOOL bNoWait)
{
	//DebugOut(_T("scrolling to %d px"), iYPos);
	m_iScrollToStart = m_iScrollPos;
	m_iScrollToDest = min(iYPos, GetEndOfListYPos() - HEIGHT(GetOwnerFrame())/2);

	m_eScrollDir = m_iScrollToStart > m_iScrollToDest ? DIR_Up : DIR_Down;

	m_eScroll = SCROLL_To;


	if(bNoWait)//skip the animation
	{
		m_iScrollPos = m_iScrollToDest;
	}
	else
	{
		SetTimer(GetOwnerWindow(), IDT_SROLL_TIMER, REFRESH_RATE, NULL);
	}

}
