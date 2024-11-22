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
	CALC_Scientific,
	CALC_SciRPN, //sci with RPN layout
	CALC_BaseConversions,
	CALC_Graphing,
	CALC_UnitConv,
    CALC_Currency,
    CALC_DateTime,
    CALC_Constants,
	CALC_Financial,
    CALC_Statistics,
    CALC_WorkSheet,//this is kind of a hack but oh well
    CALC_WSStats,
    CALC_NumberOfLayouts,
};

enum EnumCalcProSkins
{	
	SKIN_Blue,
	SKIN_Black,
//	SKIN_SciCalc,
	SKIN_Brown,
    SKIN_Green,
	SKIN_Pink,
    SKIN_Silver,
    SKIN_Custom,
    SKIN_Count,
};

enum EnumKeyboard
{
    KB_Original,
    KB_QWERTY,
    KB_AZERTY,
    KB_QWERTZ,
    KB_Count,
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
    CIssVector<ButtonType> sKeyboardButtons;
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
	void				SetSkinType(EnumCalcProSkins eSkin, HINSTANCE hInst, BOOL bForceUpdate = FALSE);

    BOOL                GetUseAdvMem(){return m_bAdvancedMem;};
    void                SetUseAdvMem(BOOL bAdvMem){m_bAdvancedMem = bAdvMem;};
    BOOL                GetUseRPN(){return m_bRPN;};
    void                SetUseRPN(BOOL bRPN){m_bRPN = bRPN;};

    int                 GetKBOffset(){return m_i1Indent;};
    void                SetKBOffset(int iOff){m_i1Indent = iOff;};

    EnumKeyboard        GetKBType(){return m_eKeyboard;};
    void                SetKBType(EnumKeyboard eKB){m_eKeyboard = eKB;};

    BOOL                GetKBLetters(){return m_bKBLetters;};
    void                SetKBLetters(BOOL bKB){m_bKBLetters = bKB;};

    BOOL                GetFullscreen(){return m_bFullscreen;};
    void                SetFullscreen(BOOL bFUll){m_bFullscreen = bFUll;};

    TCHAR*              GetKeyName(int iBtn);

private: 
	HRESULT				LoadLayout(EnumCalcType eCalcType, HINSTANCE hInst);
    HRESULT             LoadKeyboard(LayoutCalcType* sLayout, CIssIniEx& IniButtons);//called from loadlayout to setup the keyboard
	void				SetButtonLabels(ButtonType* sBtn);
	void				LoadSkin(CIssIniEx* iniButton);
	int					CalcButtonID(int iWidth, int iHeight, int iColorScheme){return (int)pow((double)iColorScheme, 10.0)+iWidth+iHeight;}; //somewhat a unique ID
	void				DeleteButtons(CIssVector<ButtonType>& sButtons);
	void				DeleteGlobalStrings();
	void				DeleteAllButtons();

    void                UpdateMemoryButtons(ButtonType* sButton);
    void                UpdateRPNButtons(ButtonType* sButton);

    TCHAR*              GetButtonName(int iBtn);

private:
	CIssString*			m_oStr;
	CalcProSkins		m_oSkin;
	LayoutCalcType*		m_sLayouts[CALC_NumberOfLayouts];
	EnumCalcType		m_eCalcType;
    EnumCalcType        m_ePrevCalc;
	EnumCalcProSkins	m_eCalcSkin;
    EnumKeyboard        m_eKeyboard;

    BOOL                m_bKBLetters;
    BOOL                m_bAdvancedMem;
    BOOL                m_bRPN;
    BOOL                m_bFullscreen;

    int                 m_i1Indent; //offset from the left for the 1 button ....
};
