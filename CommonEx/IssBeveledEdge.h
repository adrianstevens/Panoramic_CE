#pragma once

#include "IssGDI.h"
#include "IssRect.h"

enum EnumEdges
{
	EDGE_TopLeft = 0,
	EDGE_Top,
	EDGE_TopRight,
	EDGE_Right,
	EDGE_BottomRight,
	EDGE_Bottom,
	EDGE_BottomLeft,
	EDGE_Left,
	EDGE_Middle,
	EDGE_Count,
};

class CIssBeveledEdge
{
public:
	CIssBeveledEdge(void);
	~CIssBeveledEdge(void);

	BOOL	Destroy();
	BOOL	Initialize(HWND hWndParent, HINSTANCE hInst, UINT uiRes);
	BOOL	Initialize(HWND hWndParent, HINSTANCE hInst, UINT uiTL, UINT uiT, UINT uiTR, UINT uiR, UINT uiBR, UINT uiB, UINT uiBL, UINT uiL, UINT uiM);
	BOOL	InitializeAlpha(UINT uiRes);
	BOOL	InitializeAlpha(UINT uiTL, UINT uiT, UINT uiTR, UINT uiR, UINT uiBR, UINT uiB, UINT uiBL, UINT uiL, UINT uiM);
	BOOL	SetLocation(RECT rcLocation);
	BOOL	Draw(HDC hDC);
	BOOL	Draw(CIssGDI& gdi, int iAlpha = 255, POINT* ptLocation = NULL);
	int		GetWidth(){return WIDTH(m_rcLocation);};
	int		GetHeight(){return HEIGHT(m_rcLocation);};
	RECT	GetLocation(){return m_rcLocation;};
	BOOL	GenerateImages(HDC hDC);
	HDC		GetImageDC(){return m_gdiBevel.GetDC();};
	HDC		GetAlphaDC(){return m_gdiBevelAlpha.GetDC();};
	CIssGDI* GetAlphaGDI(){return &m_gdiBevelAlpha;};

private:	// function
	BOOL	CreateImage(HDC hDC, UINT uiRes, CIssGDI& gdiBevel);
	BOOL	CreateImageSliced(HDC hDC, UINT uiImage[], CIssGDI& gdiBevel);

private:	// variables
	HWND		m_hWndParent;
	HINSTANCE	m_hInst;
	UINT		m_uiRes;
	UINT		m_uiResAlpha;
	UINT		m_uiImage[EDGE_Count];
	UINT		m_uiAlpha[EDGE_Count];
	CIssRect	m_rcLocation;
	CIssGDI		m_gdiBevel;
	CIssGDI		m_gdiBevelAlpha;
};
