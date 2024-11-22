#pragma once

#include "stdafx.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssString.h"

struct TypeValue
{
	int 	iWidth;
	int		iIndex;

	TypeValue();
	~TypeValue();
};

class CIssImgFont
{
public:
	CIssImgFont(void);
	~CIssImgFont(void);

	BOOL	Destroy();
	BOOL	Initialize(UINT uiFont, HWND hWndParent, HINSTANCE hInst, int iHeight = -1);
	BOOL	IsLoaded();
	void	DrawText(CIssGDIEx& gdiDest, TCHAR* szText, RECT& rc, UINT uiFormat = DT_LEFT, int iAlpha = -1);
	int		GetHeight(){return m_gdiFont.GetHeight();};

private:	// variables
	CIssString*				m_oStr;
	CIssVector<TypeValue>	m_arrValues;
	CIssGDIEx				m_gdiFont;
};
