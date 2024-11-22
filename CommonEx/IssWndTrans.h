#pragma once

#include "IssWnd.h"

#define  NM_PAINTBACKGROUND       9000

// notification structure
typedef struct tagNMPAINTBACKGROUND
{
	NMHDR   hdr;
	HDC		hDC;
	RECT	rcDraw;
} NMPAINTBACKGROUND, *LPNMPAINTBACKGROUND;

HRESULT IssUpdateControls(HWND hWnd, HDC hDCBackground, RECT* rcArea = NULL);

class CIssWndTrans:public CIssWnd
{
public:
	CIssWndTrans(void);
	~CIssWndTrans(void);

protected:	// functions
	void	ForceUpdateBackground(HDC hDCBackground);

protected:	// variables

};
