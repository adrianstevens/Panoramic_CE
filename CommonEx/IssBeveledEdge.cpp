#include "IssBeveledEdge.h"
#include "IssCommon.h"

CIssBeveledEdge::CIssBeveledEdge(void)
:m_hWndParent(NULL)
,m_hInst(NULL)
,m_uiRes(0)
,m_uiResAlpha(0)
{
	for(int i=0; i<EDGE_Count; i++)
	{
		m_uiImage[i] = 0;
		m_uiAlpha[i] = 0;
	}
}

CIssBeveledEdge::~CIssBeveledEdge(void)
{
	Destroy();
}

BOOL CIssBeveledEdge::Destroy()
{
	m_gdiBevel.Destroy();
	m_gdiBevelAlpha.Destroy();
	return TRUE;
}

BOOL CIssBeveledEdge::Initialize(HWND hWndParent, HINSTANCE hInst, 
								 UINT uiTL, UINT uiT, UINT uiTR, UINT uiR, 
								 UINT uiBR, UINT uiB, UINT uiBL, UINT uiL, UINT uiM)
{
	Destroy();
	m_hWndParent		= hWndParent;
	m_hInst				= hInst;
	m_uiImage[EDGE_TopLeft]		= uiTL;
	m_uiImage[EDGE_Top]			= uiT;
	m_uiImage[EDGE_TopRight]	= uiTR;
	m_uiImage[EDGE_Right]		= uiR;
	m_uiImage[EDGE_BottomRight]	= uiBR;
	m_uiImage[EDGE_Bottom]		= uiB;
	m_uiImage[EDGE_BottomLeft]	= uiBL;
	m_uiImage[EDGE_Left]		= uiL;
	m_uiImage[EDGE_Middle]		= uiM;
	m_uiRes						= 0;
	m_uiResAlpha				= 0;
	return TRUE;
}

BOOL CIssBeveledEdge::Initialize(HWND hWndParent, HINSTANCE hInst, UINT uiRes)
{
	Destroy();
	m_hWndParent		= hWndParent;
	m_hInst				= hInst;
	m_uiRes				= uiRes;
	return TRUE;
}

BOOL CIssBeveledEdge::InitializeAlpha(UINT uiTL, UINT uiT, UINT uiTR, UINT uiR, 
								 UINT uiBR, UINT uiB, UINT uiBL, UINT uiL, UINT uiM)
{
	m_uiAlpha[EDGE_TopLeft]		= uiTL;
	m_uiAlpha[EDGE_Top]			= uiT;
	m_uiAlpha[EDGE_TopRight]	= uiTR;
	m_uiAlpha[EDGE_Right]		= uiR;
	m_uiAlpha[EDGE_BottomRight]	= uiBR;
	m_uiAlpha[EDGE_Bottom]		= uiB;
	m_uiAlpha[EDGE_BottomLeft]	= uiBL;
	m_uiAlpha[EDGE_Left]		= uiL;
	m_uiAlpha[EDGE_Middle]		= uiM;
	m_uiRes						= 0;
	m_uiResAlpha				= 0;
	return TRUE;
}

BOOL CIssBeveledEdge::InitializeAlpha(UINT uiRes)
{
	m_uiResAlpha			= uiRes;
	return TRUE;
}

BOOL CIssBeveledEdge::SetLocation(RECT rcLocation)
{
	Destroy();
	m_rcLocation.Set(rcLocation);
	return TRUE;
}

BOOL CIssBeveledEdge::GenerateImages(HDC hDC)
{
	// create the normal image
	if(!m_gdiBevel.GetDC())
	{
		if(m_uiRes)
		{
			// we have one array image to work with
			if(!CreateImage(hDC, m_uiRes, m_gdiBevel))
				return FALSE;
		}
		else
		{
			// we supplied a set of images which might not all be the same size
			if(!CreateImageSliced(hDC, m_uiImage, m_gdiBevel))
				return FALSE;
		}
	}

	// create the alpha image if one is defined
	if(!m_gdiBevelAlpha.GetDC() && (m_uiAlpha[0] != 0 || m_uiResAlpha != 0))
	{
		if(m_uiResAlpha)
		{
			// we have one array image to work with
			if(!CreateImage(hDC, m_uiResAlpha, m_gdiBevelAlpha))
				return FALSE;
		}
		else
		{
			// we supplied a set of images which might not all be the same size
			if(!CreateImageSliced(hDC, m_uiAlpha, m_gdiBevelAlpha))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CIssBeveledEdge::Draw(HDC hDC)
{
	if(!GenerateImages(hDC))
		return FALSE;

	// Draw it
	TransparentBlt(hDC,
		m_rcLocation.left, m_rcLocation.top,
		WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
		m_gdiBevel.GetDC(),
		0,0,
		WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
		TRANSPARENT_COLOR);

	return TRUE;
}

BOOL CIssBeveledEdge::Draw(CIssGDI& gdi, int iAlpha /*= 255*/, POINT* ptLocation /* = NULL*/)
{
	if(!GenerateImages(gdi.GetDC()))
		return FALSE;

	// if there will be no alpha layer then just draw
	if(m_uiResAlpha == 0 && m_uiAlpha[0] == 0)
	{
		CIssGDI::BitBltAlpha(gdi, 
							m_rcLocation.left, m_rcLocation.top, 
							WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
							m_gdiBevel,
							0,0,
							iAlpha,
							TRANSPARENT_COLOR);
		return TRUE;
	}


	POINT pt;

	pt.x	= m_rcLocation.left;
	pt.y	= m_rcLocation.top;
	if(ptLocation)
	{
		pt.x	= ptLocation->x;
		pt.y	= ptLocation->y;
	}
	CIssGDI::MyAlphaImage(gdi,
							pt.x, pt.y, 
							WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
							m_gdiBevel,
							m_gdiBevelAlpha,
							0,0);

	// for testing
	/*RECT rc;
	rc.left		= pt.x;
	rc.top		= pt.y;
	rc.right	= rc.left + WIDTH(m_rcLocation);
	rc.bottom	= rc.top + HEIGHT(m_rcLocation);
	CIssGDI::FrameRect(gdi.GetDC(), rc, RGB(255,255,255));*/

	return TRUE;
}


BOOL CIssBeveledEdge::CreateImageSliced(HDC hDC, UINT uiImage[], CIssGDI& gdiBevel)
{
	if(!m_hWndParent || !m_hInst || 
		m_rcLocation.GetWidth() == 0 || m_rcLocation.GetHeight() == 0)
		return FALSE;

	for(int i=0; i<EDGE_Count; i++)
	{
		if(uiImage[i] == 0)
			return FALSE;
	}

	CIssGDI gdiTL, gdiT, gdiTR, gdiR, gdiBR, gdiB, gdiBL, gdiL, gdiM;

	if(!LoadImage(gdiTL, uiImage[EDGE_TopLeft], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiT, uiImage[EDGE_Top], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiTR, uiImage[EDGE_TopRight], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiR, uiImage[EDGE_Right], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiBR, uiImage[EDGE_BottomRight], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiB, uiImage[EDGE_Bottom], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiBL, uiImage[EDGE_BottomLeft], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiL, uiImage[EDGE_Left], m_hWndParent, m_hInst))
		return FALSE;
	if(!LoadImage(gdiM, uiImage[EDGE_Middle], m_hWndParent, m_hInst))
		return FALSE;

	CIssRect rcZero(m_rcLocation.Get());
	rcZero.ZeroBase();
	gdiBevel.Create(hDC, rcZero.Get(), FALSE, TRUE, FALSE);

	// draw the middle stuff first
	RECT rcTile = rcZero.Get();
	if(gdiM.GetWidth() == 1 && gdiM.GetHeight() == 1)
	{
		COLORREF crColor = gdiM.GetPixelColor(0,0);
		CIssGDI::FillRect(gdiBevel.GetDC(), rcTile, crColor);
	}
	else
		CIssGDI::TileBackGround(gdiBevel.GetDC(),	rcTile, gdiM,	FALSE);

	// draw the top part
	rcTile			= rcZero.Get();
	rcTile.bottom	= gdiT.GetHeight();
	CIssGDI::TileBackGround(gdiBevel.GetDC(),	rcTile, gdiT,	FALSE);

	// draw the bottom part
	rcTile			= rcZero.Get();
	rcTile.top		= rcZero.bottom - gdiB.GetHeight();
	/*CIssGDI::TileBackGround(gdiBevel.GetDC(),	rcTile, gdiB,	FALSE);*/
	for(int i=0; i<rcZero.GetWidth()+gdiB.GetWidth(); i+= gdiB.GetWidth())
	{
		rcTile.left		= i;
		rcTile.right	= rcTile.left + gdiB.GetWidth();
		if(rcTile.left >= rcZero.GetWidth())
			break;
		if(rcTile.right > rcZero.right)
			rcTile.right = rcZero.right;

		BitBlt(gdiBevel.GetDC(), 
			rcTile.left, rcTile.top,
			WIDTH(rcTile), gdiB.GetHeight(),
			gdiB.GetDC(),
			0,0,
			SRCCOPY);
	}

	// draw the Left part
	rcTile			= rcZero.Get();
	rcTile.right	= gdiL.GetWidth();
	CIssGDI::TileBackGround(gdiBevel.GetDC(),	rcTile, gdiL,	FALSE);

	// draw the right part
	rcTile			= rcZero.Get();
	rcTile.left		= rcZero.right - gdiR.GetWidth();
	for(int i=0; i<rcZero.GetHeight()+gdiR.GetHeight(); i+= gdiR.GetHeight())
	{
		rcTile.top		= i;
		rcTile.bottom	= rcTile.top + gdiR.GetHeight();
		if(rcTile.top >= rcZero.GetHeight())
			break;
		if(rcTile.bottom > rcZero.bottom)
			rcTile.bottom = rcZero.bottom;

		BitBlt(gdiBevel.GetDC(), 
			   rcTile.left, rcTile.top,
			   gdiR.GetWidth(), HEIGHT(rcTile),
			   gdiR.GetDC(),
			   0,0,
			   SRCCOPY);
	}
	//CIssGDI::TileBackGround(m_gdiBevel.GetDC(),	rcTile, gdiR,	TRUE);

	// draw top left piece
	BitBlt(gdiBevel.GetDC(),
		0,0,
		gdiTL.GetWidth(), gdiTL.GetHeight(),
		gdiTL.GetDC(),
		0, 0,
		SRCCOPY);

	// draw top Right piece
	BitBlt(gdiBevel.GetDC(),
		rcZero.right - gdiTR.GetWidth(),0,
		gdiTR.GetWidth(), gdiTR.GetHeight(),
		gdiTR.GetDC(),
		0, 0,
		SRCCOPY);

	// draw Bottom Right piece
	BitBlt(gdiBevel.GetDC(),
		rcZero.right - gdiBR.GetWidth(),rcZero.bottom - gdiBR.GetHeight(),
		gdiBR.GetWidth(), gdiBR.GetHeight(),
		gdiBR.GetDC(),
		0, 0,
		SRCCOPY);

	// draw Bottom Left piece
	BitBlt(gdiBevel.GetDC(),
		0,rcZero.bottom - gdiBL.GetHeight(),
		gdiBL.GetWidth(), gdiBL.GetHeight(),
		gdiBL.GetDC(),
		0, 0,
		SRCCOPY);

	return TRUE;
}

BOOL CIssBeveledEdge::CreateImage(HDC hDC, UINT uiRes, CIssGDI& gdiBevel)
{
	if(!m_hWndParent || !m_hInst || uiRes == 0 || m_rcLocation.GetWidth() == 0 || m_rcLocation.GetHeight() == 0)
		return FALSE;

	CIssGDI gdiArray;
	if(!LoadImage(gdiArray, uiRes, m_hWndParent, m_hInst))
		return FALSE;

	int iBlockSize = gdiArray.GetHeight();
	// we do a quick check to see if what we want to draw to is big enough
	// NOTE: the width and height need to be at least as big as two blocks
	if(2*iBlockSize > HEIGHT(m_rcLocation) || 2*iBlockSize > WIDTH(m_rcLocation))
		return FALSE;

	CIssRect rcZero(m_rcLocation.Get());
	rcZero.ZeroBase();
	gdiBevel.Create(hDC, rcZero.Get(), FALSE, TRUE, FALSE);

	CIssGDI gdiMiddle;
	gdiMiddle.Create(hDC, iBlockSize, iBlockSize, FALSE, FALSE);
	BitBlt(gdiMiddle.GetDC(),
		0,0,
		iBlockSize, iBlockSize,
		gdiArray.GetDC(),
		EDGE_Middle*iBlockSize, 0,
		SRCCOPY);

	// draw the middle stuff first
	CIssGDI::TileBackGround(gdiBevel.GetDC(),
							rcZero.Get(),
							gdiMiddle,
							FALSE);

	// draw the top part
	for(int i=0; i<WIDTH(rcZero); i+= iBlockSize)
	{
		BitBlt(gdiBevel.GetDC(),
			i,0,
			iBlockSize, iBlockSize,
			gdiArray.GetDC(),
			EDGE_Top*iBlockSize, 0,
			SRCCOPY);
	}

	// draw the bottom part
	for(int i=0; i<WIDTH(rcZero); i+= iBlockSize)
	{
		BitBlt(gdiBevel.GetDC(),
			i,rcZero.bottom-iBlockSize,
			iBlockSize, iBlockSize,
			gdiArray.GetDC(),
			EDGE_Bottom*iBlockSize, 0,
			SRCCOPY);
	}

	// draw the left part
	for(int i=0; i<HEIGHT(rcZero); i+= iBlockSize)
	{
		BitBlt(gdiBevel.GetDC(),
			0,i,
			iBlockSize, iBlockSize,
			gdiArray.GetDC(),
			EDGE_Left*iBlockSize, 0,
			SRCCOPY);
	}

	// draw the right part
	for(int i=0; i<HEIGHT(rcZero); i+= iBlockSize)
	{
		BitBlt(gdiBevel.GetDC(),
			rcZero.right-iBlockSize,i,
			iBlockSize, iBlockSize,
			gdiArray.GetDC(),
			EDGE_Right*iBlockSize, 0,
			SRCCOPY);
	}

	// draw top left piece
	BitBlt(gdiBevel.GetDC(),
				   0,0,
				   iBlockSize, iBlockSize,
				   gdiArray.GetDC(),
				   EDGE_TopLeft*iBlockSize, 0,
				   SRCCOPY);

	// draw top Right piece
	BitBlt(gdiBevel.GetDC(),
					rcZero.right - iBlockSize,0,
					iBlockSize, iBlockSize,
					gdiArray.GetDC(),
					EDGE_TopRight*iBlockSize, 0,
					SRCCOPY);

	// draw Bottom Right piece
	BitBlt(gdiBevel.GetDC(),
					rcZero.right - iBlockSize,rcZero.bottom - iBlockSize,
					iBlockSize, iBlockSize,
					gdiArray.GetDC(),
					EDGE_BottomRight*iBlockSize, 0,
					SRCCOPY);

	// draw Bottom Left piece
	BitBlt(gdiBevel.GetDC(),
					0,rcZero.bottom - iBlockSize,
					iBlockSize, iBlockSize,
					gdiArray.GetDC(),
					EDGE_BottomLeft*iBlockSize, 0,
					SRCCOPY);
	
	return TRUE;
}

