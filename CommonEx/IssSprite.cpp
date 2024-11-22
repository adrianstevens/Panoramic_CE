#include "IssSprite.h"
#include "IssGDIFx.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"

CIssSprite::CIssSprite(void)
:m_iNumFrames(0)
,m_iTimePerFrame(0)
,m_bLoop(FALSE)
,m_iCurrentFrame(0)
,m_dwStartAniTime(0)
,m_iTimeToComplete(0)
,m_iWidth(0)
{
}

CIssSprite::~CIssSprite(void)
{
	Destroy();
}

BOOL CIssSprite::Destroy()
{
	m_gdiImage.Destroy();
	m_iNumFrames		= 0;
	m_iTimePerFrame	= 0;
	m_bLoop				= FALSE;
	m_iCurrentFrame		= 0;
	m_iTimeToComplete	= 0;
	m_dwStartAniTime	= 0;
	m_iWidth			= 0;
	return TRUE;
}

BOOL CIssSprite::Initialize(UINT uiImage, HWND hWndParent, HINSTANCE hInst, int iNumFrames, int iTimeToComplete, BOOL bLoop)
{
	Destroy();

	m_gdiImage.LoadImage(uiImage, hWndParent, hInst);

	if(	m_gdiImage.GetDC() == NULL || 
		m_gdiImage.GetWidth() == 0 || 
		m_gdiImage.GetHeight() == 0		)
		return FALSE;

	m_iNumFrames		= iNumFrames;
	m_iTimeToComplete	= iTimeToComplete;
	m_iTimePerFrame		= iTimeToComplete/iNumFrames;
	m_bLoop				= bLoop;
	m_iWidth			= m_gdiImage.GetWidth()/iNumFrames;

    ResetAnimation();

	return TRUE;
}

void CIssSprite::ResetAnimation()
{
    m_iCurrentFrame = 0;
    m_dwStartAniTime= 0;

}

BOOL CIssSprite::SetSize(int cx, int cy)
{
	if(cx == 0 || cy == 0  || !m_gdiImage.GetDC())
		return FALSE;

	SIZE sizeImage;
	sizeImage.cx	= cx*m_iNumFrames;
	sizeImage.cy	= cy;
	m_iWidth		= cx;

	CIssGDIEx gdiImage;
	ScaleImage(m_gdiImage, gdiImage, sizeImage, FALSE, 0);

	m_gdiImage.Create(gdiImage.GetDC(), sizeImage.cx, cy, FALSE, TRUE);
    m_gdiImage.InitAlpha(TRUE);
    ::Draw(m_gdiImage, 0,0, gdiImage.GetWidth(), gdiImage.GetHeight(), gdiImage, 0,0, ALPHA_Copy);

	return TRUE;
}

BOOL CIssSprite::Draw(CIssGDIEx& gdiDest, int x, int y, DWORD dwCurrentTick, DWORD dwAnimOffset /*= 0*/)
{
	// no more animation
	if(m_iCurrentFrame == -1)
		return TRUE;

	if(dwAnimOffset != 0)
		dwCurrentTick += dwAnimOffset;

	// if we haven't started animating yet
	if(m_dwStartAniTime == 0)
	{
		m_dwStartAniTime	= dwCurrentTick;
		m_iCurrentFrame		= 0;
	}
	else 
	{
		int iTimeDiff = abs(dwCurrentTick-m_dwStartAniTime);
		if(iTimeDiff == 0)
		{}
		else if(m_bLoop)
		{
			m_iCurrentFrame = iTimeDiff/m_iTimePerFrame;
			m_iCurrentFrame = m_iCurrentFrame%m_iNumFrames;
		}
		else if(iTimeDiff > m_iTimeToComplete)
		{
			// we're done animating
			m_dwStartAniTime = 0;
			m_iCurrentFrame	 = -1;
			return FALSE;
		}
		else
		{
			m_iCurrentFrame = iTimeDiff/m_iTimePerFrame;
		}
	}

    ::Draw(gdiDest,
		x,y,
		m_iWidth, m_gdiImage.GetHeight(),
		m_gdiImage,
		m_iCurrentFrame*m_iWidth, 0);

	return TRUE;
}
