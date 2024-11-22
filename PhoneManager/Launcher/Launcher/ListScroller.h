#pragma once


enum EnumScroll
{
	SCROLL_Scrolling,
	SCROLL_Moving,
	SCROLL_Stopped,
	SCROLL_To,
};

enum EnumScrollDir
{
	DIR_Down,
	DIR_Up,
};


class CListScroller
{
public:
	CListScroller(void);
	~CListScroller(void);

	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);

	int				GetScrollPos()		{ return m_iScrollPos; }
	void			ResetScrollPos()	{ m_iScrollPos = 0; }
	BOOL			IsStopped()			
    { 
        return (m_eScroll == SCROLL_Stopped); 
    }//any type of "movement"
	EnumScroll		GetMovementState()	{ return m_eScroll;};
	
//	BOOL			IsMoving()			{ return (m_eScroll == SCROLL_Moving); }
	void			StopScrolling();			

protected:
	virtual	HWND	GetOwnerWindow(void)	= 0;	// derived class must implement
	virtual RECT&	GetOwnerFrame(void)	= 0;
	virtual int		GetEndOfListYPos(void)	= 0;
	virtual void	ReDrawList(void)		= 0;
	void			ScrollTo(int iYPos, BOOL bNoWait = FALSE);
	void			ReDrawList(int iScrollAmount);

private:
	void			ScrollList();
	int				GetNextScrollToPos();
	BOOL			ScrollToDestReached();
	float			CalcScrollIncrement();	// in pixels

	int				m_iScrollPos;			//in pixels
	int				m_iLastReDrawInc;

	double			m_dbSpeed;
	double			m_dbFriction;

	EnumScroll		m_eScroll;
	EnumScrollDir	m_eScrollDir;

	int				m_iScrollYStart;
	int				m_iScrollYPos;

	int				m_iScrollToStart;
	int				m_iScrollToDest;

	DWORD			m_dwStartTime;
	DWORD			m_dwStopTime;
};
