#include "CalcBtnManager.h"
#include "ButtonLabels.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "IssRegistry.h"

CCalcBtnManager::CCalcBtnManager():
m_eCalcType(CALC_NumberOfLayouts), //yes this looks wrong BUT .. Load Registry will always set this
m_ePrevCalc(CALC_NumberOfLayouts),
m_oStr(CIssString::Instance()),
m_eCalcSkin(SKIN_Blue),
m_bAdvancedMem(FALSE)
{
	for(int i = 0; i < CALC_NumberOfLayouts; i++)
		m_sLayouts[i] = NULL;

    DWORD dwTemp = 0;
#define REG_KEY _T("SOFTWARE\\Pano\\CalcProLite")
    //set the Advanced Memory
    if(S_OK == GetKey(REG_KEY, _T("AdvMem"), dwTemp))
        m_bAdvancedMem=(BOOL)dwTemp;

#ifdef DEBUG
 //   m_bAdvancedMem = TRUE;
#endif
}

CCalcBtnManager::~CCalcBtnManager(void)
{
	DeleteAllButtons();
}

void CCalcBtnManager::DeleteAllButtons()
{
	for(int i = 0; i < CALC_NumberOfLayouts; i++)
	{
		LayoutCalcType* sLayout = m_sLayouts[i];
		if(sLayout)
		{   //clean em up
			//buttons are shared between layouts
			DeleteButtons(sLayout->sMainButtons);
			DeleteButtons(sLayout->sMainAltButtons);//only for SP Main at the moment
			DeleteButtons(sLayout->sAltButtons);	

			DeleteButtons(sLayout->s2ndFButtons);
			DeleteButtons(sLayout->sHypButtons);
			DeleteButtons(sLayout->s2ndHypFButtons);

			delete m_sLayouts[i];
			m_sLayouts[i] = NULL;
		}
	}
}

void CCalcBtnManager::DeleteButtons(CIssVector<ButtonType>& sButtons)
{
	for(int i=0; i<sButtons.GetSize(); i++)
	{
		ButtonType* sButton = sButtons[i];
		if(sButton)
		{
			//they're global and shared
	//		m_oStr->Delete(&sButton->szLabel);
	//		m_oStr->Delete(&sButton->szLabelExp);
			delete sButton;
		}
	}
	sButtons.RemoveAll();
}

void CCalcBtnManager::DeleteGlobalStrings()
{
	int i;
	for(i = 0; i < NUMBER_OF_Numbers; i++)
	{
		m_oStr->Delete(&g_szNumberLabels[i]);
		g_szNumberLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Operators; i++)
	{
		m_oStr->Delete(&g_szOperatorLabels[i]);
		g_szOperatorLabels[i] = NULL;
		m_oStr->Delete(&g_szOperatorExp[i]);
		g_szOperatorExp[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Functions; i++)
	{
		m_oStr->Delete(&g_szFunctionLabels[i]);
		g_szFunctionLabels[i] = NULL;
		m_oStr->Delete(&g_szFunctionExp[i]);
		g_szFunctionExp[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Constants; i++)
	{
		m_oStr->Delete(&g_szConstantLabels[i]);
		g_szConstantLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Brackets; i++)
	{
		m_oStr->Delete(&g_szBracketLabels[i]);
		g_szBracketLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Memory; i++)
	{
		m_oStr->Delete(&g_szMemoryLabels[i]);
		g_szMemoryLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Clear; i++)
	{
		m_oStr->Delete(&g_szClearLabels[i]);
		g_szClearLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Numbers; i++)
	{
		m_oStr->Delete(&g_szNumberLabels[i]);
		g_szNumberLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Bases; i++)
	{
		m_oStr->Delete(&g_szBaseLabels[i]);
		g_szBaseLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_RPN; i++)
	{
		m_oStr->Delete(&g_szRPNLabels[i]);
		g_szRPNLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_CalcStates; i++)
	{
		m_oStr->Delete(&g_szCalcStateLabels[i]);
		g_szCalcStateLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Clipboard; i++)
	{
		m_oStr->Delete(&g_szClipboardLabels[i]);
		g_szClipboardLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Worksheet; i++)
	{
		m_oStr->Delete(&g_szWorksheetLabels[i]);
		g_szWorksheetLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Launch; i++)
	{
		m_oStr->Delete(&g_szLaunchLabels[i]);
		g_szLaunchLabels[i] = NULL;
	}

	for(i = 0; i < NUMBER_OF_Graphing; i++)
	{
		m_oStr->Delete(&g_szGraphLabels[i]);
		g_szGraphLabels[i] = NULL;
	}
}

HRESULT CCalcBtnManager::SetCalcType(EnumCalcType eType, HINSTANCE hInst)
{
    HRESULT hr = S_OK;
    CBHRE((eType >= 0), _T("CalcBtnMan - invalid calc type"), E_INVALIDARG);
    CBHRE((m_eCalcType != eType), _T("CalcBtnMan - CalcType already set"), S_OK);

	if(m_sLayouts[eType] == NULL)
	{	//we have to initialize the layout
        hr = LoadLayout(eType, hInst), _T("LoadLayout Failed"), E_FAIL;
        CHRE(hr, _T("Load Layout failed"), E_FAIL);
	}

	//important of course but we'll only set it if we don't fail
    m_ePrevCalc = m_eCalcType;
    if(m_ePrevCalc == CALC_NumberOfLayouts)
        m_ePrevCalc = CALC_Standard;

    
	m_eCalcType = eType;

Error:
    //double check
    if(m_eCalcType < 0 || m_eCalcType >= CALC_NumberOfLayouts)
        m_eCalcType = CALC_Standard;
	return hr;
}

void CCalcBtnManager::LoadSkin(CIssIniEx* iniButton)
{
	if(iniButton == NULL)
		return;

	TCHAR szValue[STRING_NORMAL];
	TCHAR szNumber[STRING_SMALL];
	
	ButtonColorsType* btnColors;

	int iTemp;
	iniButton->GetValue(&iTemp, _T("Init"), _T("BtnType"), 0);
	m_oSkin.SetButtonType(BtnStyleType(iTemp));

    iniButton->GetValue(&iTemp, _T("Init"), _T("UseShadow"), RGB(0,0,1));
    m_oSkin.SetUseShadow(BOOL(iTemp));

	for(int i = 0; i < 6; i++)
	{
		btnColors = m_oSkin.GetButton(i);
		m_oStr->IntToString(szNumber, i+1);
		m_oStr->StringCopy(szValue, _T("Btn"));
		m_oStr->Concatenate(szValue, szNumber);
		m_oStr->Concatenate(szValue, _T("Color1"));
		iniButton->GetValue(&btnColors->ButtonColor1, _T("Init"), szValue, RGB(0,0,1));

		m_oStr->StringCopy(szValue, _T("Btn"));
		m_oStr->Concatenate(szValue, szNumber);
		m_oStr->Concatenate(szValue, _T("Color2"));
		iniButton->GetValue(&btnColors->ButtonColor2, _T("Init"), szValue, RGB(0,0,1));

		m_oStr->StringCopy(szValue, _T("Btn"));
		m_oStr->Concatenate(szValue, szNumber);
		m_oStr->Concatenate(szValue, _T("HiLite1"));
		iniButton->GetValue(&btnColors->OutlineColor1, _T("Init"), szValue, RGB(0,0,1));

		m_oStr->StringCopy(szValue, _T("Btn"));
		m_oStr->Concatenate(szValue, szNumber);
		m_oStr->Concatenate(szValue, _T("HiLite2"));
		iniButton->GetValue(&btnColors->OutlineColor2, _T("Init"), szValue, RGB(0,0,1));

		m_oStr->StringCopy(szValue, _T("Btn"));
		m_oStr->Concatenate(szValue, szNumber);
		m_oStr->Concatenate(szValue, _T("Text1"));
		iniButton->GetValue(&btnColors->TextColor, _T("Init"), szValue, RGB(0,0,1));

        m_oStr->StringCopy(szValue, _T("Btn"));
        m_oStr->Concatenate(szValue, szNumber);
        m_oStr->Concatenate(szValue, _T("Text2"));
        iniButton->GetValue(&btnColors->TextColor2, _T("Init"), szValue, RGB(0,0,1));
	}

    iniButton->GetValue(&m_oSkin.m_crBackground, _T("Skin"), _T("Background"), RGB(0,0,1));
    iniButton->GetValue(&m_oSkin.m_crText, _T("Skin"), _T("TextColor"), RGB(0,0,1));
    iniButton->GetValue(&m_oSkin.m_crTextTop, _T("Skin"), _T("TextTopColor"), RGB(0,0,1));

    iniButton->GetValue(&m_oSkin.m_crWS1, _T("Skin"), _T("WS1"), RGB(0,0,1));
    iniButton->GetValue(&m_oSkin.m_crWS2, _T("Skin"), _T("WS2"), RGB(0,0,1));

	m_oStr->StringCopy(szValue, _T("Color1"));
	iniButton->GetValue(&m_oSkin.m_typeDisplay.ButtonColor1, _T("Skin"), szValue, RGB(0,0,1));

	m_oStr->StringCopy(szValue, _T("Color2"));
	iniButton->GetValue(&m_oSkin.m_typeDisplay.ButtonColor2, _T("Skin"), szValue, RGB(0,0,1));

	m_oStr->StringCopy(szValue, _T("HiLite1"));
	iniButton->GetValue(&m_oSkin.m_typeDisplay.OutlineColor1, _T("Skin"), szValue, RGB(0,0,1));

	m_oStr->StringCopy(szValue, _T("HiLite2"));
	iniButton->GetValue(&m_oSkin.m_typeDisplay.OutlineColor2, _T("Skin"), szValue, RGB(0,0,1));

	m_oStr->StringCopy(szValue, _T("TextColor"));
	iniButton->GetValue(&m_oSkin.m_typeDisplay.TextColor, _T("Skin"), szValue, RGB(0,0,1));

    m_oStr->StringCopy(szValue, _T("TextColorTop"));
    iniButton->GetValue(&m_oSkin.m_typeDisplay.TextColor2, _T("Skin"), szValue, RGB(0,0,1));
}

void CCalcBtnManager::SetSkinType(EnumCalcProSkins eSkin, HINSTANCE hInst)
{
	if(eSkin == m_eCalcSkin &&
        m_sLayouts[m_eCalcSkin] != NULL)
		return;

    if(eSkin < 0 || eSkin >= SKIN_Count)
        return;

	m_eCalcSkin = eSkin;

	if(hInst != NULL)
	{
		DeleteAllButtons();	
		LoadLayout(m_eCalcType, hInst);
    }
}

//was originally just loading the standard skin
HRESULT CCalcBtnManager::LoadLayout(EnumCalcType eCalcType, HINSTANCE hInst)
{
    HRESULT hr = S_OK;

	//only load the layout if it hasn't previously been loaded
	//hard code values for now ... read em in later
	LayoutCalcType*		sLayout = m_sLayouts[eCalcType]; //just less typing

    CIssIniEx IniLayout;
    CIssIniEx IniButtons;
    ButtonType* sButton = NULL;
    
    //check the ol hInst
    CBHRE(hInst != NULL, _T("hInstance is NULL"), E_INVALIDARG);

	if(sLayout != NULL)
		goto Error;
	else	//if it doesn't exist create some memory
		sLayout = new LayoutCalcType;

    

    CPHR(sLayout, _T("layout failed to create"));

	BOOL bRet = FALSE;
	switch(m_eCalcSkin)
	{
/*	case SKIN_SciCalc:
		hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_SCICALC"), hInst);
		break;*/
	case SKIN_Black:
		hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_BLACK"), hInst);
		break;
	default:
    case SKIN_Blue:
		hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_BLUE"), hInst);
		break;
    case SKIN_Green:
        hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_GREEN"), hInst);
        break;
/*	case SKIN_Brown:
		hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_BROWN"), hInst);
	    break;*/
    case SKIN_Pink:
        hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_PINK"), hInst);
        break;
    case SKIN_Silver: 
        hr = IniButtons.OpenFromResrouce(_T("IDR_TXT_BUTTON_SILVER"), hInst);
        break;
    case SKIN_Custom:
        {
            TCHAR szFile[STRING_MAX];
            GetExeDirectory(szFile);
            m_oStr->Concatenate(szFile, _T("\\custom.ini"));
            hr = IniButtons.Open(szFile);
        }
        break;
	}

    CHR(hr, _T("Failed to load skin.ini"));

	//populate the skin class
	LoadSkin(&IniButtons);

	//lets load the correct layout here
	TCHAR szTemp[STRING_LARGE];
	switch(eCalcType)
	{
	default:
	case CALC_Standard:
		m_oStr->StringCopy(szTemp, _T("IDR_TXT_LAYOUTSTANDARD"));
		break;
    case CALC_Currency://same layout for main buttons 
		m_oStr->StringCopy(szTemp, _T("IDR_TXT_LAYOUTUNIT"));
		break;
     }

    hr = IniLayout.OpenFromResrouce(szTemp, hInst);
    CHR(hr, _T("Failed to load button layout"));
	
	IniLayout.GetValue(&sLayout->iAltBtnColumns,	_T("Init"), _T("AltBtnColumns"), 3); 
	IniLayout.GetValue(&sLayout->iAltBtnRows,		_T("Init"), _T("AltBtnRows"), 3); 
	IniLayout.GetValue(&sLayout->iMainBtnColumns,	_T("Init"), _T("MainBtnColumns"), 3); 
	IniLayout.GetValue(&sLayout->iMainBtnRows,		_T("Init"), _T("MainBtnRows"), 3); 

	//now some longish manual coding
	
	int iButtons = sLayout->iMainBtnColumns*sLayout->iMainBtnRows;
	int i = 0;

	TCHAR szButton[STRING_SMALL];
	TCHAR szButtonName[STRING_NORMAL];
	
	for(i = 0; i < iButtons; i++)
	{
        m_oStr->Empty(szButtonName);
		sButton = new ButtonType;
		ZeroMemory(sButton, sizeof(ButtonType));

		m_oStr->IntToString(szButton, i+1);
		m_oStr->Insert(szButton, _T("Btn"), 0);

		if(S_OK == IniLayout.GetValue(szButtonName, _T("MainButtons"), szButton, _T("Plus")))
		{
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

            //bit of a hack but oh well
            if(m_bAdvancedMem == TRUE && sButton->iButtonType == 4)
                UpdateMemoryButtons(sButton);

			SetButtonLabels(sButton);

			sLayout->sMainButtons.AddElement(sButton);
		}
		else
		{
			delete sButton;
		}
		
#ifdef WIN32_PLATFORM_WFSP //keep things quick for now .. secondary function of main buttons on the 12 key devices
		sButton = new ButtonType;
		ZeroMemory(sButton, sizeof(ButtonType));

		if(S_OK == IniLayout.GetValue(szButtonName, _T("MainAlt"), szButton, _T("Plus")))
		{
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

            //bit of a hack but oh well
            if(m_bAdvancedMem == TRUE && sButton->iButtonType == 4)
                UpdateMemoryButtons(sButton);

			SetButtonLabels(sButton);

			sLayout->sMainAltButtons.AddElement(sButton);
		}
		else //I hate to do this every time but its gotta be checked
		{
			delete sButton;
		}
#endif
	}

	iButtons = sLayout->iAltBtnColumns*sLayout->iAltBtnRows;

    //gonna do a little hack to save some work
    BOOL bAlt = TRUE;
    BOOL b2nd = TRUE;
    
	for(i = 0; i < iButtons; i++)
	{
		m_oStr->IntToString(szButton, i+1);
		m_oStr->Insert(szButton, _T("Btn"), 0);
        m_oStr->Empty(szButtonName);

		if(bAlt && S_OK == IniLayout.GetValue(szButtonName, _T("AltButtons"), szButton, _T("Plus")))
		{
			sButton = new ButtonType;
			ZeroMemory(sButton, sizeof(ButtonType));
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

            //bit of a hack but oh well
            if(m_bAdvancedMem == TRUE && sButton->iButtonType == 4)
                UpdateMemoryButtons(sButton);

			SetButtonLabels(sButton);

			sLayout->sAltButtons.AddElement(sButton);
		}
        else
            bAlt = FALSE;

		//if we have 2ndF
		if(b2nd && S_OK == IniLayout.GetValue(szButtonName, _T("2nd"), szButton, _T("Plus")))
		{
			sButton = new ButtonType;
			ZeroMemory(sButton, sizeof(ButtonType));
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

			SetButtonLabels(sButton);

			sLayout->s2ndFButtons.AddElement(sButton);
		}
        else
            b2nd = FALSE;

		//if we have Hyp
		if(b2nd && S_OK == IniLayout.GetValue(szButtonName, _T("Hyp"), szButton, _T("Plus")))
		{
			sButton = new ButtonType;
			ZeroMemory(sButton, sizeof(ButtonType));
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

			SetButtonLabels(sButton);

			sLayout->sHypButtons.AddElement(sButton);
		}

		//if we have 2ndf & hyp
		if(b2nd && S_OK == IniLayout.GetValue(szButtonName, _T("Hyp2nd"), szButton, _T("Plus")))
		{
			sButton = new ButtonType;
			ZeroMemory(sButton, sizeof(ButtonType));
			IniButtons.GetValue(&sButton->iButtonType, szButtonName, _T("BtnType"), 0);
			IniButtons.GetValue(&sButton->iButtonValue, szButtonName, _T("BtnId"), 0);
			IniButtons.GetValue(&sButton->iSkinIndex, szButtonName, _T("BtnStyle"), 0);

			SetButtonLabels(sButton);

			sLayout->s2ndHypFButtons.AddElement(sButton);
		}
	}

	m_sLayouts[eCalcType] = sLayout;
    sLayout = NULL;

Error:
    return hr;
}

void CCalcBtnManager::SetButtonLabels(ButtonType* sBtn)
{
	switch(sBtn->iButtonType)
	{
	case INPUT_Number:
		sBtn->szLabel		= g_szNumberLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Operator:
		sBtn->szLabel		= g_szOperatorLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= g_szOperatorExp[sBtn->iButtonValue];
		break;
	case INPUT_Function:
		sBtn->szLabel		= g_szFunctionLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= g_szFunctionExp[sBtn->iButtonValue];
		break;
	case INPUT_Bracket:
		sBtn->szLabel		= g_szBracketLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Memory:
		sBtn->szLabel		= g_szMemoryLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Equals:
		sBtn->szLabel		= g_szEqualsLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Clear:
		if(sBtn->iButtonValue  == CLEAR_BackSpace)
		{
			sBtn->szLabel		= NULL;
			sBtn->szLabelExp	= NULL;
			break;
		}
		sBtn->szLabel		= g_szClearLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_BaseConversion:
		sBtn->szLabel		= g_szBaseLabels[sBtn->iButtonValue-(2)];//bug bug ... should probably check this
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Constants:
        if(sBtn->iButtonValue == CONSTANT_Last_Ans)
            sBtn->szLabel   = g_szConstantLabels[NUMBER_OF_Constants-1];
        else if(sBtn->iButtonValue == CONSTANT_LastX)
            sBtn->szLabel   = g_szConstantLabels[NUMBER_OF_Constants-2];
        else
		    sBtn->szLabel		= g_szConstantLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_CalcState:
		sBtn->szLabel		= g_szCalcStateLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Graph:
		sBtn->szLabel		= g_szGraphLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_Clipboard:
        sBtn->szLabel		= g_szClipboardLabels[sBtn->iButtonValue];
        sBtn->szLabelExp	= NULL;
        break;
	case INPUT_RPN:
		sBtn->szLabel		= g_szRPNLabels[sBtn->iButtonValue];
		sBtn->szLabelExp	= NULL;
		break;
	case INPUT_WS:
        sBtn->szLabel		= g_szWorksheetLabels[sBtn->iButtonValue];
        sBtn->szLabelExp	= NULL;
        break;
	case INPUT_Launch:	
        sBtn->szLabel		= g_szLaunchLabels[sBtn->iButtonValue];
        sBtn->szLabelExp	= NULL;
        break;
	case INPUT_NULL:		
	case INPUT_Custom:	
	default:
		sBtn->szLabel		= NULL;
		sBtn->szLabelExp	= NULL;
		break;
	}
}

LayoutCalcType* CCalcBtnManager::GetLayout(EnumCalcType eCalcType, HINSTANCE hInst)
{
    if(eCalcType < 0 || eCalcType >= CALC_NumberOfLayouts)
        return NULL;

    //now .. has it been loaded?
    if(m_sLayouts[eCalcType] == NULL)
    {
        if(S_OK != LoadLayout(eCalcType, hInst))
            return NULL;
    }

	return m_sLayouts[eCalcType];
}

LayoutCalcType* CCalcBtnManager::GetCurrentLayout(HINSTANCE hInst)
{
    return GetLayout(m_eCalcType, hInst);
}

void CCalcBtnManager::UpdateMemoryButtons(ButtonType* sButton)
{
    if(sButton == NULL)
        return;

    switch(sButton->iButtonValue)
    {
    case MEMORY_StoreMem1:
        sButton->iButtonValue = CALCSTATE_MemoryStore;
        sButton->iButtonType = INPUT_CalcState;
        break;
    case MEMORY_RecallMem1:
        sButton->iButtonValue = CALCSTATE_MemoryRecall;
        sButton->iButtonType = INPUT_CalcState;
    	break;
    case MEMORY_ClearMem1:
        sButton->iButtonValue = MEMORY_1;
        break;
    case MEMORY_Plus1:
        sButton->iButtonValue = MEMORY_2;
        break;
    case MEMORY_MINUS:
        sButton->iButtonValue = MEMORY_3;
        break;
    }


}