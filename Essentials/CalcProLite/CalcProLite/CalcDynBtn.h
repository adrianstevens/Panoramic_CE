#pragma once
#include "windows.h"
#include "issdynbtn.h"


class CCalcDynBtn : public CIssDynBtn
{
public:
	CCalcDynBtn(void);
	~CCalcDynBtn(void);

	//Draw Functions
	BOOL	DrawButtonText(	HFONT	hFont, 
							HDC		hDC,
							RECT	rcBtnRect,
							TCHAR*	szTextOverride = NULL,
							TCHAR*	szExp = NULL,
							TCHAR* szAlt = NULL, 
							TCHAR* szAltExp = NULL,
							HFONT	hFontExp = NULL);

    BOOL	DrawButtonText(	HFONT	hFont, 
                            HDC		hDC,
                            RECT	rcBtnRect,
                            TCHAR*	szTextOverride = NULL,
                            TCHAR*	szExp = NULL,
                            HFONT	hFontExp = NULL);


	BOOL	Draw(HFONT		hFont, 
				HDC				hDC,
				TCHAR*			szTextOverride,
				TCHAR*			szExp = NULL,
				TCHAR*			szAltText = NULL,
				TCHAR*			szAltExp = NULL,
				HFONT			hFontExp = NULL);
	
};
