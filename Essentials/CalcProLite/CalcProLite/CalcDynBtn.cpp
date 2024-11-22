#include "CalcDynBtn.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"

CCalcDynBtn::CCalcDynBtn(void)
{
}

CCalcDynBtn::~CCalcDynBtn(void)
{
}

/************************************************************************
Draw the Button Text when Draw is Called
************************************************************************/
BOOL CCalcDynBtn::DrawButtonText(HFONT hFont, HDC hDC, RECT rcBtnRect, TCHAR* szTextOverride /* = NULL */, TCHAR* szExp /* = NULL */, TCHAR* szAlt, TCHAR* szAltExp, HFONT hFontExp /* = NULL */)
{
#define BTN_TEXT_INDENT (m_rcBtnRect.bottom - m_rcBtnRect.top)/10

	if(szAltExp == NULL && szAlt == NULL)
	{
		DrawButtonText(hFont, hDC, rcBtnRect, szTextOverride, szExp, hFontExp);//why do extra work?
		return TRUE;//NULL text is ok
	}

	TCHAR *szText;
	BOOL bUseExp;

	if(szExp == NULL || _tcslen(szExp) == 0)	bUseExp = FALSE;
	else				bUseExp = TRUE;

	// are we using the quick override of the saved text
	szText	= szTextOverride?szTextOverride:m_szBtnChar;

	SIZE sizeTxt;
	RECT rcTxt;
	SIZE sizeExp;
	RECT rcExp;
	sizeExp.cx		= 0;
	sizeExp.cy		= 0;
	rcExp.left		= 0;
	rcExp.right		= 0;
	rcExp.top		= 0;
	rcExp.bottom	= 0;

	HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

	// Find the size of the text we want to place on the button
	GetTextExtentPoint( hDC, szText, (int)_tcslen( szText ), &sizeTxt);

	if(bUseExp)
	{
		SelectObject(hDC, hFontExp);
		GetTextExtentPoint( hDC, szExp, (int)_tcslen( szExp ), &sizeExp);
		SelectObject(hDC, hFont);
	}
	// line up the text within the button
	rcTxt.left		= rcBtnRect.left  + (rcBtnRect.right - sizeTxt.cx - sizeExp.cx - rcBtnRect.left)/2;
	rcTxt.top		= rcBtnRect.top + (rcBtnRect.bottom - sizeTxt.cy - rcBtnRect.top);// - BTN_TEXT_INDENT;
	rcTxt.top	   += m_bBtnDown?1:0;
	rcTxt.right		= rcBtnRect.right - BTN_TEXT_INDENT;
	rcTxt.bottom	= rcTxt.top + sizeTxt.cy;

	if(bUseExp)
	{
		rcExp.left		= rcTxt.right;
		rcExp.right		= rcExp.left + sizeExp.cx;
		rcExp.bottom	= rcTxt.bottom - (rcTxt.bottom - rcTxt.top)/3   ;
		rcExp.top		= rcExp.bottom - sizeExp.cy;

		//Now Re-Center
		int iOffset		= (sizeExp.cy)/3 - (sizeTxt.cy)/6;

		rcExp.bottom	+= iOffset;
		rcExp.top		+= iOffset;

		rcTxt.top		+= iOffset; 	
		rcTxt.bottom	+= iOffset; 
	}

	// draw the text to the screen
	int iOldMode = SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, m_crTextColor);
	DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_RIGHT);//set only for non exp labels for now
	if(bUseExp)
	{
		SelectObject(hDC, hFontExp);
		DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
		SelectObject(hDC, hFont);
	}	
	//now if we're here we know we have alt text
	// line up the text within the button
	rcTxt.left		= rcBtnRect.left + BTN_TEXT_INDENT;
	rcTxt.top		= rcBtnRect.top;
	rcTxt.top	   += m_bBtnDown?1:0;
	rcTxt.right		= rcBtnRect.right;
	rcTxt.bottom	= rcBtnRect.bottom;
	SelectObject(hDC, hFontExp);
	DrawText(hDC, szAlt, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);

/*	if(m_bUseShadow)
	{
		SetTextColor(hDC, m_crTextShadow);
		rcTxt.left++;
		rcTxt.right++;
		rcTxt.top--;
		rcTxt.bottom--;
		rcExp.left++;
		rcExp.right++;
		rcExp.top--;
		rcExp.bottom--;
		DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
		if(bUseExp)
			DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
	}*/

	SetBkMode(hDC, iOldMode);
	SelectObject(hDC, oldFont);

	return TRUE;
}


/************************************************************************
Draw the Button Text when Draw is Called
************************************************************************/
BOOL CCalcDynBtn::DrawButtonText(HFONT hFont, HDC hDC, RECT rcBtnRect, TCHAR* szTextOverride /* = NULL */, TCHAR* szExp /* = NULL */, HFONT hFontExp /* = NULL */)
{
    if(szTextOverride == NULL)
        return FALSE;

    TCHAR *szText;
    BOOL bUseExp;

    if(szExp == NULL || _tcslen(szExp) == 0)	bUseExp = FALSE;
    else				bUseExp = TRUE;

    // are we using the quick override of the saved text
    szText	= szTextOverride?szTextOverride:m_szBtnChar;

    SIZE sizeTxt;
    RECT rcTxt;
    SIZE sizeExp;
    RECT rcExp;
    sizeExp.cx		= 0;
    sizeExp.cy		= 0;
    rcExp.left		= 0;
    rcExp.right		= 0;
    rcExp.top		= 0;
    rcExp.bottom	= 0;

    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

    // Find the size of the text we want to place on the button
    GetTextExtentPoint( hDC, szText, (int)_tcslen( szText ), &sizeTxt);

    if(bUseExp)
    {
        SelectObject(hDC, hFontExp);
        GetTextExtentPoint( hDC, szExp, (int)_tcslen( szExp ), &sizeExp);
        SelectObject(hDC, hFont);
    }
    // line up the text within the button
    rcTxt.left		= rcBtnRect.left + (rcBtnRect.right - sizeTxt.cx - sizeExp.cx - rcBtnRect.left)/2;
    rcTxt.top		= rcBtnRect.top + (rcBtnRect.bottom - sizeTxt.cy - rcBtnRect.top)/2 ;
    rcTxt.top	   += m_bBtnDown?1:0;
    rcTxt.right		= rcTxt.left + sizeTxt.cx;
    rcTxt.bottom	= rcTxt.top + sizeTxt.cy;

    if(bUseExp)
    {
        rcExp.left		= rcTxt.right;
        rcExp.right		= rcExp.left + sizeExp.cx;
        rcExp.bottom	= rcTxt.bottom - (rcTxt.bottom - rcTxt.top)/3   ;
        rcExp.top		= rcExp.bottom - sizeExp.cy;

        //Now Re-Center
        int iOffset		= (sizeExp.cy)/3 - (sizeTxt.cy)/6;

        rcExp.bottom	+= iOffset;
        rcExp.top		+= iOffset;

        rcTxt.top		+= iOffset; 	
        rcTxt.bottom	+= iOffset; 

    }

    // draw the text to the screen
    int iOldMode = SetBkMode(hDC, TRANSPARENT);
    
    //we're gonna play here a little
/*  SetTextColor(hDC, m_crTextShadow);
    rcTxt.left--;
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
    rcTxt.left+=2;
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
    rcTxt.left--;
    rcTxt.top++;
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
    rcTxt.top-=2;
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
    rcTxt.top++;*/

 /*   if(m_bUseShadow)
    {
        rcTxt.left++;
        rcTxt.top++;
        DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
        rcTxt.left--;
        rcTxt.top--;
    }*/

    if(m_bUseShadow)
    {
        rcTxt.left++;
        rcTxt.right++;
        rcTxt.top++;
        rcTxt.bottom++;
        rcExp.left++;
        rcExp.right++;
        rcExp.top++;
        rcExp.bottom++;
    }

    
    SetTextColor(hDC, m_crTextColor);
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);

    if(bUseExp)
    {
        SelectObject(hDC, hFontExp);
        DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
        SelectObject(hDC, hFont);
    }	
    if(m_bUseShadow)
    {
        SetTextColor(hDC, m_crTextShadow);
        rcTxt.left--;
        rcTxt.right--;
        rcTxt.top--;
        rcTxt.bottom--;
        rcExp.left--;
        rcExp.right--;
        rcExp.top--;
        rcExp.bottom--;
        DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
        if(bUseExp)
            DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
    }

    SetBkMode(hDC, iOldMode);
    SelectObject(hDC, oldFont);

    return TRUE;
}


/************************************************************************
Lets Draw our Button

use this Function and NOT Draw button
************************************************************************/
BOOL CCalcDynBtn::Draw(HFONT	hFont, 
					  HDC		hDC,
					  TCHAR*	szTextOverride,
					  TCHAR*	szExp,
					  TCHAR*	szAltText,
					  TCHAR*	szAltExp,
					  HFONT		hFontExp)
{
	HDC  hdcDest = hDC;
	RECT rcBtnRect;

	memcpy(&rcBtnRect, &m_rcBtnRect, sizeof(RECT));

	if(m_bDisabled)
	{
		rcBtnRect.left		= 0;
		rcBtnRect.top		= 0;
		rcBtnRect.right		= m_rcBtnRect.right-m_rcBtnRect.left;
		rcBtnRect.bottom	= m_rcBtnRect.bottom - m_rcBtnRect.top;

		if(!m_gdiDisabled)
		{
			m_gdiDisabled = new CIssGDIEx;

			// create the disabled image for drawing
			m_gdiDisabled->Create(hDC,rcBtnRect);
			// draw a pink background
			HBRUSH hbrPink = CIssGDIEx::CreateBrush(TRANSPARENT_COLOR);
			FillRect(m_gdiDisabled->GetDC(), &rcBtnRect, hbrPink);
			DeleteObject(hbrPink);
		}
		hdcDest = m_gdiDisabled->GetDC();
	}

	if(m_pDynBtn) //if the pointer is valid draw the button now
		m_pDynBtn->DrawButton(hdcDest, m_bBtnDown, rcBtnRect);
	else
		DrawButton(hdcDest, m_bBtnDown, rcBtnRect);

	BOOL bRet = DrawButtonText(hFont, hdcDest, rcBtnRect, szTextOverride, szExp, szAltText, szAltExp, hFontExp);

	if(m_bDisabled && m_gdiDisabled)
	{
		// add our grayed image on top of the button
		AlphaFillRect(*m_gdiDisabled, rcBtnRect, RGB(200,200,200), 160);

		// draw back to the screen buffer
#ifndef UNDER_CE
		TransparentBlt(hDC,
#else
		TransparentImage(hDC,
#endif
			m_rcBtnRect.left,
			m_rcBtnRect.top,
			m_rcBtnRect.right - m_rcBtnRect.left,
			m_rcBtnRect.bottom - m_rcBtnRect.top,
			m_gdiDisabled->GetDC(),
			0,0,
			m_rcBtnRect.right - m_rcBtnRect.left,
			m_rcBtnRect.bottom - m_rcBtnRect.top,
			TRANSPARENT_COLOR);
	}

	return bRet;
}