#pragma once

#include "IssGDIEx.h"

class CIssSprite
{
public:
	CIssSprite(void);
	~CIssSprite(void);

	BOOL	Destroy();
	BOOL	Initialize(UINT uiImage, HWND hWndParent, HINSTANCE hInst, int iNumFrames, int iTimeToComplete, BOOL bLoop);
	BOOL	SetSize(int cx, int cy);
	int		GetWidth(){return m_gdiImage.GetWidth();};
	int		GetHeight(){return m_gdiImage.GetHeight();};
	BOOL	Draw(CIssGDIEx& gdiDest, int x, int y, DWORD dwCurrentTick, DWORD dwAnimOffset = 0);
    BOOL	IsLoaded(){return m_gdiImage.GetDC()?TRUE:FALSE;};
	BOOL	IsDoneAnimating(){return (m_iCurrentFrame==-1?TRUE:FALSE);};
    void    ResetAnimation();

private:	// variables
	CIssGDIEx   m_gdiImage;				// our main image GDI
	int			m_iNumFrames;			// number of frames in the image array
	int			m_iTimeToComplete;		// Time to complete
	int			m_iTimePerFrame;		// number of milliseconds that each frame should take up
	BOOL		m_bLoop;				// should we loop after we're done the animation
	int			m_iCurrentFrame;		// the last frame that we should draw
	DWORD		m_dwStartAniTime;		// time that we started drawing the animation
	int			m_iWidth;				// width of a frame
};
