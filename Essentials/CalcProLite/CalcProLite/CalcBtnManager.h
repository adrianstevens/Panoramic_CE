#pragma once

#include "stdafx.h"
#include "IssVector.h"
#include "IssIniEx.h"
#include "IssString.h"
#include "CalcProSkins.h"
#include "math.h"

enum EnumCalcType
{
	CALC_Standard,
    CALC_Currency,
    CALC_NumberOfLayouts,
};

enum EnumCalcProSkins
{	
	SKIN_Blue,
	SKIN_Black,
//	SKIN_SciCalc,
//	SKIN_Brown,
    SKIN_Green,
	SKIN_Pink,
    SKIN_Silver,
    SKIN_Custom,
    SKIN_Count,
};

struct ButtonType	//information not stored in the button class
{					//if needed we can add a pointer to the actual IssDynButton
	int		iButtonType;	//number, operator, etc
	int		iButtonValue;	//index within that type
	int		iSkinIndex;
	TCHAR*	szLabel;		//we'll just point this to the string in the header
	TCHAR*	szLabelExp;
	//we can add keyboard information here as well ... good stuff
};

struct LayoutCalcType
{
	int iMainBtnRows;
	int	iMainBtnColumns;
	int	iAltBtnRows;
	int iAltBtnColumns;
	CIssVector<ButtonType> sMainButtons;
	CIssVector<ButtonType> sMainAltButtons;	//to get the values and the label
	CIssVector<ButtonType> sAltButtons;
	//only for Sci at the moment ... maybe used for others later
	CIssVector<ButtonType> s2ndFButtons;
	CIssVector<ButtonType> s2ndHypFButtons;
	CIssVector<ButtonType> sHypButtons;
 };

class CCalcBtnManager
{
public:
	CCalcBtnManager(void);
	~CCalcBtnManager(void);

	EnumCalcType		GetCalcType(){return m_eCalcType;};
    EnumCalcType        GetPrevCalc(){return m_ePrevCalc;};
	HRESULT				SetCalcType(EnumCalcType eType, HINSTANCE hInst);
	LayoutCalcType*		GetCurrentLayout(HINSTANCE hInst = NULL);
	LayoutCalcType*		GetLayout(EnumCalcType eCalcType, HINSTANCE hInst);//this only works if its been initialized ... hmmm
	CalcProSkins*		GetSkin(){return &m_oSkin;};

	EnumCalcProSkins	GetSkinType(){return m_eCalcSkin;};
	void				SetSkinType(EnumCalcProSkins eSkin, HINSTANCE hInst);

    BOOL                GetUseAdvMem(){return m_bAdvancedMem;};
    void                SetUseAdvMem(BOOL bAdvMem){m_bAdvancedMem = bAdvMem;};

private: 
	HRESULT				LoadLayout(EnumCalcType eCalcType, HINSTANCE hInst);
	void				SetButtonLabels(ButtonType* sBtn);
	void				LoadSkin(CIssIniEx* iniButton);
	int					CalcButtonID(int iWidth, int iHeight, int iColorScheme){return (int)pow((double)iColorScheme, 10.0)+iWidth+iHeight;}; //somewhat a unique ID
	void				DeleteButtons(CIssVector<ButtonType>& sButtons);
	void				DeleteGlobalStrings();
	void				DeleteAllButtons();

    void                UpdateMemoryButtons(ButtonType* sButton);

private:
	CIssString*			m_oStr;
	CalcProSkins		m_oSkin;
	LayoutCalcType*		m_sLayouts[CALC_NumberOfLayouts];
	EnumCalcType		m_eCalcType;
    EnumCalcType        m_ePrevCalc;
	EnumCalcProSkins	m_eCalcSkin;

    BOOL                m_bAdvancedMem;
};
