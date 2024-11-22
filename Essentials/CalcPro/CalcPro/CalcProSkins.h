#pragma once

#include "stdafx.h"
#include "CalcDynBtn.h"

#define NUMBER_OF_BTN_SCHEMES 6

struct ButtonColorsType
{
	COLORREF ButtonColor1;
	COLORREF ButtonColor2;
	COLORREF OutlineColor1;
	COLORREF OutlineColor2;
	COLORREF TextColor;
    COLORREF TextColor2;
};

class CalcProSkins
{
public:
	CalcProSkins(void);
	~CalcProSkins(void);

	COLORREF		GetBackgroundColor(){return m_crBackground;};
	void			SetBackgroundColor(COLORREF crBG){m_crBackground = crBG;};
	ButtonColorsType* GetButton(int iIndex){return &m_typeBtnClr[iIndex];};
	void			SetButtonType(BtnStyleType eBtnType){m_eBtnType = eBtnType;};
	BtnStyleType	GetButtonType(){return m_eBtnType;};	

    void            SetUseShadow(BOOL bShadow){m_bUseTextShadow = bShadow;};
    BOOL            GetUseShadow(){return m_bUseTextShadow;};

public:
	ButtonColorsType		m_typeDisplay;
	ButtonColorsType		m_typeBtnClr[NUMBER_OF_BTN_SCHEMES];
	BtnStyleType			m_eBtnType;
	COLORREF				m_crBackground;
    COLORREF                m_crText;
    COLORREF                m_crTextTop;
    COLORREF                m_crWS1;
    COLORREF                m_crWS2;

    BOOL                    m_bUseTextShadow;
	
};
