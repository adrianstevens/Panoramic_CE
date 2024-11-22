#include "IssSciInterface.h"
#include "IssRegistry.h"
#include "DlgSciOptions.h"
#include "DlgSciAbout.h"
#include "Resource.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "globals.h"
#include "IssWndTouchMenu.h"
#include "IssDebug.h"
#include "IssCommon.h"
#include "stdafx.h"
#include "IssLocalisation.h"
#include "DlgMoreApps.h"

//percent based on portrait
#define DISPLAY_HEIGHT_PCNT		15
#define ALT_BTNS_HEIGHT_PCNT	30
#define MAIN_BTNS_HEIGHT_PCNT	55
#define MENU_Height             46
#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)

#define TM_Back_Key				500

BOOL                 g_bGraphPoints;
BOOL                 g_bGraphPolar;
BOOL                 g_bGraphFast;

TypeMenu::TypeMenu()
:szMenu(NULL)
,uiMenu(0)
{}

TypeMenu::~TypeMenu()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szMenu);
    uiMenu = 0;
}


CIssCalcInterface::CIssCalcInterface(HINSTANCE hInst):
 m_hFontMainBtnText(NULL)
,m_hFontMainLongText(NULL)
,m_hFontAltBtnText(NULL)
,m_hFontAltLongText(NULL)
,m_hFontDisplayTop(NULL)
,m_hFontDisplay(NULL)
,m_hFontAltXLText(NULL)
,m_hFontMenu(NULL)
,m_hFontRPN(NULL)
,m_bUseRegionalSettings(FALSE)
,m_eState(CALCSTATE_Normal)
,m_bBackPressed(FALSE)
,m_bHoldBack(FALSE)
,m_bPlaySounds(TRUE)
,m_bAnimate2ndHyp(TRUE)
,m_eCalcMode(CALC_OrderOfOpps)
,m_iMemoryIndex(0)//always start on the first one
{
	m_hInst				= hInst;
	m_oStr				= CIssString::Instance();
	m_oCalc				= CIssCalculator::Instance();
#ifdef WIN32_PLATFORM_WFSP
	m_oCalc->SetDisplayCharacters(15);//for now .. works nicely on PPC ... check SP
#else
	m_oCalc->SetDisplayCharacters(17);//for now .. works nicely on PPC ... check SP
#endif

	m_hFontDisplay		= CIssGDIEx::CreateFont(DISPLAY_TEXT_SIZE, FW_NORMAL, TRUE);
	m_hFontExponent		= CIssGDIEx::CreateFont(EXPONENT_TEXT_SIZE, FW_NORMAL, TRUE);
	m_hFontDisplayTop	= CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, FW_NORMAL, TRUE);
	m_hFontSmallTop		= CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT_SM, FW_NORMAL, TRUE);
    m_hFontRPN          = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	LoadRegistry();

#ifdef WIN32_PLATFORM_WFSP
	m_bShowSelector = TRUE;//smart phone only please
#else
	m_bShowSelector = FALSE;
#endif

    m_oConstants.LoadConstants(m_hInst);

    m_oBtnMan = &m_oBtnManger;//not needed but done for completeness
}

CIssCalcInterface::~CIssCalcInterface(void)
{
	Destroy();
	SaveRegistry();
}

void CIssCalcInterface::Destroy()
{
	CIssGDIEx::DeleteFont(m_hFontMainBtnText);
	CIssGDIEx::DeleteFont(m_hFontMainLongText);
    CIssGDIEx::DeleteFont(m_hFontAltBtnText);
	CIssGDIEx::DeleteFont(m_hFontAltLongText);
    CIssGDIEx::DeleteFont(m_hFontAltXLText);
	CIssGDIEx::DeleteFont(m_hFontDisplayTop);
	CIssGDIEx::DeleteFont(m_hFontSmallTop);
	CIssGDIEx::DeleteFont(m_hFontDisplay);
	CIssGDIEx::DeleteFont(m_hFontExponent);
    CIssGDIEx::DeleteFont(m_hFontMenu);
    CIssGDIEx::DeleteFont(m_hFontRPN);

	DeleteButtonVector();

	DeleteDynBtnVector(&m_arrMainButtons);
	DeleteDynBtnVector(&m_arrAltButtons);
	DeleteDynBtnVector(&m_arr2ndFButtons);
	DeleteDynBtnVector(&m_arrHypFButtons);
	DeleteDynBtnVector(&m_arr2ndHypFButtons);

}

//lets blow out the button vectors
void CIssCalcInterface::DeleteDynBtnVector(CIssVector<CCalcDynBtn>* arr)
{
	if(arr == NULL)
		return;

	CCalcDynBtn* oBtn = NULL;

	for(int i = 0; i < arr->GetSize(); i++)
	{
		oBtn = (*arr)[i];
		if(oBtn)
			delete oBtn;
		oBtn = NULL;	
	}
	arr->RemoveAll();
}

void CIssCalcInterface::LoadRegistry()
{
    DWORD dwTemp;

	//regional settings
	if(S_OK == GetKey(REG_KEY_SCI, _T("RegionalSettings"), dwTemp))
		m_bUseRegionalSettings = (BOOL)dwTemp;
	else 
		m_bUseRegionalSettings = FALSE;

	if(S_OK == GetKey(REG_KEY_SCI, _T("PlaySounds"), dwTemp))
		m_bPlaySounds = (BOOL)dwTemp;
	else 
		m_bPlaySounds = TRUE;

	//use tape
	if(S_OK == GetKey(REG_KEY_SCI, _T("UseTape"), dwTemp))
		m_oCalc->SetUseTape((BOOL)dwTemp);
	else 
		m_oCalc->SetUseTape(FALSE);

	//animate 2ndhyp
	if(S_OK == GetKey(REG_KEY_SCI, _T("Ani2ndHyp"), dwTemp))
		m_bAnimate2ndHyp = (BOOL)dwTemp;
	else 
		m_bAnimate2ndHyp = (FALSE);

	//set the current skin
	if(S_OK == GetKey(REG_KEY_SCI, _T("Skin"), dwTemp))
		m_oBtnManger.SetSkinType((EnumCalcProSkins)dwTemp, NULL);
	else
		m_oBtnManger.SetSkinType(SKIN_Blue, NULL);

	//calculator type
	if(S_OK == GetKey(REG_KEY_SCI, _T("CalcType"), dwTemp))
		m_oBtnManger.SetCalcType((EnumCalcType)dwTemp, m_hInst);
	else
		m_oBtnManger.SetCalcType(CALC_Scientific, m_hInst);

	//set the calc mode
	if(S_OK == GetKey(REG_KEY_SCI, _T("CalcMode"), dwTemp))
		m_eCalcMode = (CalcOperationType)dwTemp;
    UpdateCalcMode(m_eCalcMode);

	//set the base bits
	if(S_OK == GetKey(REG_KEY_SCI, _T("BaseBits"), dwTemp))
		m_oCalc->SetBaseBits((__int8)dwTemp);
	else
		m_oCalc->SetBaseBits(32);

	//set the notation
	if(S_OK == GetKey(REG_KEY_SCI, _T("Notation"), dwTemp))
		m_oCalc->SetCalcDisplay((DisplayType)dwTemp);
	else
		m_oCalc->SetCalcDisplay(DISPLAY_Float);

	//set the RPN Stack Size
	if(S_OK == GetKey(REG_KEY_SCI, _T("RPNStack"), dwTemp))
		m_oCalc->SetRPNStackSize((__int8)dwTemp);
	else
		m_oCalc->SetRPNStackSize(4);

	if(S_OK == GetKey(REG_KEY_SCI, _T("FixedDigits"), dwTemp))
		m_oCalc->SetFixedDecDigits((int)dwTemp);


    if(S_OK == GetKey(REG_KEY_SCI, _T("GraphFast"), dwTemp))
        g_bGraphFast = ((int)dwTemp);

    if(S_OK == GetKey(REG_KEY_SCI, _T("GraphPoints"), dwTemp))
        g_bGraphPoints = ((int)dwTemp);

    if(S_OK == GetKey(REG_KEY_SCI, _T("DRG"), dwTemp))
        m_oCalc->SetDRGState((DRGStateType)dwTemp);

    //otherwise default is fine
    TCHAR szTemp[STRING_MAX];
    TCHAR szMemLabel[STRING_NORMAL];

    //save the memory - might as well do all 9 locations 
    for(int i = 0; i < NUMBER_OF_MemoryBanks; i++)
    {
        m_oStr->Format(szMemLabel, _T("Memory%i"), i);
        if(S_OK == GetKey(REG_KEY_SCI, szMemLabel, szTemp, STRING_MAX))
            m_oCalc->SetMemory(szTemp, i);
    }
}

void CIssCalcInterface::SaveRegistry()
{
    //Memory
    TCHAR szTemp[STRING_MAX];
    TCHAR szMemLabel[STRING_NORMAL];

    BOOL bTemp = m_oCalc->GetUseTape();
    m_oCalc->SetUseTape(FALSE);

    DWORD dwValue = (DWORD)bTemp;
    SetKey(REG_KEY_SCI, _T("UseTape"), dwValue);

    for(int i = 0; i < NUMBER_OF_MemoryBanks; i++)
    {
        m_oStr->Format(szMemLabel, _T("Memory%i"), i);
        m_oCalc->GetMemory(szTemp, i);
        SetKey(REG_KEY_SCI, szMemLabel, szTemp, m_oStr->GetLength(szTemp));
    }

	dwValue = (DWORD)m_oBtnManger.GetCalcType();
	SetKey(REG_KEY_SCI, _T("CalcType"), dwValue);

	dwValue = (DWORD)m_eCalcMode;
	SetKey(REG_KEY_SCI, _T("CalcMode"), dwValue);

	dwValue = (DWORD)m_oCalc->GetRPNStackSize();
	SetKey(REG_KEY_SCI, _T("RPNStack"), dwValue);

	dwValue = (DWORD)m_oCalc->GetCalcDisplay();
	SetKey(REG_KEY_SCI, _T("Notation"), dwValue);

	dwValue = (DWORD)m_oCalc->GetBaseBits();
	SetKey(REG_KEY_SCI, _T("BaseBits"), dwValue);

	dwValue = (DWORD)m_bUseRegionalSettings;
	SetKey(REG_KEY_SCI, _T("RegionalSettings"), dwValue);

    dwValue = (DWORD)m_oBtnManger.GetUseAdvMem();
    SetKey(REG_KEY_SCI, _T("AdvMem"), dwValue);

	dwValue = (DWORD)m_bPlaySounds;
	SetKey(REG_KEY_SCI, _T("PlaySounds"), dwValue);

	dwValue = (DWORD)m_oBtnManger.GetSkinType();
	SetKey(REG_KEY_SCI, _T("Skin"), dwValue);

	dwValue = (DWORD)m_bAnimate2ndHyp;
	SetKey(REG_KEY_SCI, _T("Ani2ndHyp"), dwValue);

	dwValue = (DWORD)m_oCalc->GetFixedDecDigits();
	SetKey(REG_KEY_SCI, _T("FixedDigits"), dwValue);


    dwValue = (DWORD)g_bGraphFast;
    SetKey(REG_KEY_SCI, _T("GraphFast"), dwValue);

    dwValue = (DWORD)g_bGraphPoints;
    SetKey(REG_KEY_SCI, _T("GraphPoints"), dwValue);

    dwValue = (DWORD)m_oCalc->GetDRGState();
    SetKey(REG_KEY_SCI, _T("DRG"), dwValue);

    //so it saves the tape if required
    m_oCalc->SetUseTape(bTemp);
}

void CIssCalcInterface::DeleteButtonVector()
{
	TypeButtonBuffer* oBtnBuf = NULL;

	for(int i = 0; i < m_arrButtons.GetSize(); i++)
	{
		oBtnBuf = m_arrButtons[i];
		if(oBtnBuf)
			delete oBtnBuf;
		oBtnBuf = NULL;
	}
	m_arrButtons.RemoveAll();
}

BOOL CIssCalcInterface::Draw(HDC hdc, RECT& rc)
{

    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
        DrawBackground(hdc, rc);
        m_oGraphInt.Draw(hdc, rc);
        DrawButtons(hdc, rc);
        DrawSelector(hdc, rc);
        break;
    case CALC_Constants:
        DrawBackground(hdc, rc);
        m_oConstInt.Draw(hdc, rc);
        DrawButtons(hdc, rc);
        DrawSelector(hdc, rc);
        DrawDisplay(hdc, rc);
        break;
    default://code below
        DrawBackground(hdc, rc);
        DrawButtons(hdc, rc);
        DrawSelector(hdc, rc);
        DrawDisplay(hdc, rc);
        break;
    }

    if(m_wndMenu.IsWindowUp())
        m_oMenu.DrawMenu(hdc, rc, m_oStr->GetText(ID(IDS_MENU_Cancel), m_hInst), NULL);
    else
    {
        TCHAR szTemp[STRING_NORMAL];
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_File), STRING_NORMAL, m_hInst);
        m_oMenu.DrawMenu(hdc, rc, szTemp, m_oStr->GetText(ID(IDS_MENU_Menu), m_hInst));

    }
	return TRUE;
}

void CIssCalcInterface::DrawBackground(HDC hDC, RECT& rc)
{
	FillRect(hDC, rc,	m_oBtnManger.GetSkin()->m_crBackground);
}

void CIssCalcInterface::DrawDisplay(HDC hDC, RECT& rc)
{
    //make sure we need to draw it
    if(rc.top > m_rcDisplayArea.bottom ||
        rc.left > m_rcDisplayArea.right)
        return;

	static TCHAR szTemp[STRING_NORMAL];//Declare buffer
	static RECT rcTemp;
	static HFONT hFontTop;
		
	rcTemp = m_rcDisplayArea;

	//draw the display itself
	m_oDisplay.Draw(hDC);

    if(m_bExpired)
    {
        DrawText(hDC, m_oStr->GetText(ID(IDS_MSG_TrialExpired), m_hInst), m_rcDisplayArea, DT_CENTER | DT_VCENTER, m_hFontDisplay, m_oBtnManger.GetSkin()->m_crTextTop);
        return;
    }

    //fractions
	if(m_oCalc->GetCalcDisplay() == DISPLAY_Fractions &&
		m_oCalc->GetCalcBase() == 10)
	{
		if(m_eCalcMode == CALC_String)
		{
			DrawDisplayTextFracExp(hDC, rc);
			hFontTop = m_hFontSmallTop;
		}
		else
		{
			DrawDisplayTextFraction(hDC, rc);
			hFontTop = m_hFontDisplayTop;
		}
    }
    //string / expression
	else if(m_eCalcMode == CALC_String)
	{
		DrawDisplayTextExpression(hDC, rc);
		hFontTop = m_hFontSmallTop;
	}
    //RPN and no fractions ... tough shit
	else if(m_eCalcMode == CALC_RPN)
    {
        DrawDisplayTextRPN(hDC, rc);
        hFontTop = m_hFontSmallTop;
    }
    //normal
    else
	{
		DrawDisplayText(hDC, rc);
		hFontTop = m_hFontDisplayTop;
	}

	if(m_oCalc->GetCalcState() == CALCSTATE_2ndF ||
		m_oCalc->GetCalcState() == CALCSTATE_2ndF_Hyp)
	{
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_SecondFunction), STRING_NORMAL, m_hInst);
		DrawText(hDC, szTemp, m_rc2ndF, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
	}

	if(m_oCalc->GetCalcState() == CALCSTATE_Hyp ||
		m_oCalc->GetCalcState() == CALCSTATE_2ndF_Hyp)
	{
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_Hyp), STRING_NORMAL, m_hInst);
		DrawText(hDC, szTemp, m_rcHyp, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
	}

    if(m_oCalc->GetCalcState() == CALCSTATE_MemoryRecall)
    {
        m_oStr->StringCopy(szTemp, _T("RCL"));
        DrawText(hDC, szTemp, m_rcHyp, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
    }

    if(m_oCalc->GetCalcState() == CALCSTATE_MemoryStore)
    {
        m_oStr->StringCopy(szTemp, _T("STO"));
        DrawText(hDC, szTemp, m_rcHyp, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
    }

	//draw the base
	if(m_oCalc->GetCalcBase() == 2)
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_Binary), STRING_NORMAL, m_hInst);
	else if(m_oCalc->GetCalcBase() == 8)
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_Octal), STRING_NORMAL, m_hInst);
	else if(m_oCalc->GetCalcBase() == 16)
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_Hex), STRING_NORMAL, m_hInst);
    else if(m_oCalc->GetShowBCD() == TRUE)
        m_oStr->StringCopy(szTemp, _T("BCD"));
	else
		m_oStr->StringCopy(szTemp, ID(IDS_BTN_Decimal), STRING_NORMAL, m_hInst);

	DrawText(hDC, szTemp, m_rcBase, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);

	//draw the DRG state (if we're in BASE 10)
	if(m_oCalc->GetCalcBase() == 10)
	{
		if(m_oCalc->GetDRGState() == DRG_Degrees)
			m_oStr->StringCopy(szTemp, ID(IDS_BTN_Degrees), STRING_NORMAL, m_hInst);
		else if(m_oCalc->GetDRGState() == DRG_Radians)
			m_oStr->StringCopy(szTemp, ID(IDS_BTN_Radians), STRING_NORMAL, m_hInst);
		else
			m_oStr->StringCopy(szTemp, ID(IDS_BTN_Grad), STRING_NORMAL, m_hInst);
		
		DrawText(hDC, szTemp, m_rcDRG, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
	}

	if(m_oCalc->IsMemoryFilled(0))
		DrawText(hDC, _T("M"), m_rcM1, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);

		
	//draw the display type if not normal SCI ENG FIX FRA
	//this won't show on some smart phones .. and I don't care
	if(m_oCalc->GetCalcBase() == 10 && m_oCalc->GetShowBCD() == FALSE)
	{
		switch(m_oCalc->GetCalcDisplay())
		{
		case DISPLAY_Fractions:
			m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_FRAC), m_hInst));
			break;
		case DISPLAY_Engineering:
			m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_BTN_Decimal), m_hInst));
			break;
		case DISPLAY_Scientific:
			m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_SCI), m_hInst));
			break;
		case DISPLAY_Fixed:
			m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_FIX), m_hInst));
			break;
        case DISPLAY_DMS:
            m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_DMS), m_hInst));
            break;
        case DISPLAY_BCD:
            m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_BCD), m_hInst));
            break;
		default:
			m_oStr->StringCopy(szTemp, m_oStr->GetText(ID(IDS_OPT_NORM), m_hInst));
			break;
		}
	}
	else if(m_oCalc->GetCalcBase() != 10)
	{ 
		switch(m_oCalc->GetBaseBits())//Adrian said to leave these. March 18/2010
		{
		case 8:
			m_oStr->StringCopy(szTemp, _T("8bit"));
			break;
		case 16:
			m_oStr->StringCopy(szTemp, _T("16bit"));
			break;
		default:		
		case 32:
			m_oStr->StringCopy(szTemp, _T("32bit"));
			break;
		case 48:
		    m_oStr->StringCopy(szTemp, _T("48bit"));
		    break;
		}

	}
	DrawText(hDC, szTemp, m_rcDisplay, DT_CENTER | DT_TOP, hFontTop, m_oBtnManger.GetSkin()->m_crTextTop);
}

void CIssCalcInterface::DrawDisplayText(HDC hDC, RECT& rc)
{
	static TCHAR szLastPressed[STRING_MAX];
	RECT rcTemp = m_rcDisplayArea;
	rcTemp.bottom += GetSystemMetrics(SM_CXICON)/16;//down by 2

	HFONT hText = m_hFontDisplay;

	//draw the display text
	m_oCalc->GetAnswer(m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

	//check for non-base 10 and length
    //might as well handle it for other numbers too ... 
	if(m_oStr->GetLength(m_szExp) == 0)
    //if(m_oCalc->GetCalcBase() != 10)
	{
		int iLen = m_oStr->GetLength(m_szResult);
		
		if(iLen > 24)
		{
			hText = m_hFontDisplayTop;
			rcTemp.bottom -= 2;
		}
		else if(iLen > 19)
			hText = m_hFontExponent;
	}

	if(m_oStr->GetLength(m_szExp) == 0)
	{	
		DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_BOTTOM | DT_END_ELLIPSIS, 
			hText, m_oBtnManger.GetSkin()->m_crText);
	}
	else
	{	//gotta make room for the exponent
		rcTemp.bottom -= TEXT_INDENT;
		DrawText(hDC, m_szExp, rcTemp, DT_RIGHT | DT_BOTTOM, 
			m_hFontExponent, m_oBtnManger.GetSkin()->m_crText);
		rcTemp.bottom += TEXT_INDENT;
		//rcTemp = m_rcDisplayArea;
		rcTemp.right -= GetSystemMetrics(SM_CXICON)*5/4;
		DrawText(hDC, m_szValue, rcTemp, DT_RIGHT | DT_BOTTOM, 
			hText, m_oBtnManger.GetSkin()->m_crText);
	}

	m_oCalc->GetEquation(szLastPressed);

	//draw the last pressed 
	rcTemp = m_rcDisplayArea;
	rcTemp.left += TEXT_INDENT;
	DrawText(hDC, szLastPressed, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);//, RGB(170,109,82));
}

void CIssCalcInterface::DrawDisplayTextFraction(HDC hDC, RECT& rc)
{
	static TCHAR szLastPressed[STRING_MAX];
	RECT rcTemp = m_rcDisplayArea;
	rcTemp.bottom += GetSystemMetrics(SM_CXICON)/16;//down by 2

	//draw the display text
	m_oCalc->GetAnswer(m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

	//we need to figure out if we have a fraction or not
	//result cannot have a decimal place
	//all three values should be filled to be interesting
	if(m_oStr->Find(m_szResult, _T(".")) != -1 || m_oStr->GetLength(m_szValue) == 0)
	{
		DrawDisplayText(hDC, rc);
		return;
	}

    //m_szValue is the numerator
	//m_szExp is the denominator
	rcTemp.bottom -= GetSystemMetrics(SM_CXSMICON);
	DrawText(hDC, m_szValue, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontExponent, m_oBtnManger.GetSkin()->m_crText);

    //our line
    SIZE sizeText;

    HFONT hFontOld = (HFONT)SelectObject(hDC, m_hFontExponent);
    GetTextExtentPoint(hDC, m_szExp, m_oStr->GetLength(m_szExp), &sizeText);
    SelectObject(hDC, hFontOld);

	rcTemp.top		+= GetSystemMetrics(SM_CXSMICON);
	rcTemp.bottom	+= GetSystemMetrics(SM_CXSMICON);
	DrawText(hDC, m_szExp, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontExponent, m_oBtnManger.GetSkin()->m_crText);

    //now lets draw the line between the fraction
    static POINT pt[2];

    int iOffset = (GetSystemMetrics(SM_CXSMICON)*3/16);

    pt[0].y = pt[1].y = rcTemp.bottom - sizeText.cy + iOffset;
    pt[0].x = rcTemp.right;
    pt[1].x = pt[0].x - sizeText.cx;

    if(GetSystemMetrics(SM_CXICON) > 42 && GetSystemMetrics(SM_CXICON) < 46)
    {   //don't ask me why .. stupid 128 dpi
        pt[0].y += 2;
        pt[1].y += 2;
    }

    HPEN hNew = CIssGDIEx::CreatePen(m_oBtnManger.GetSkin()->m_crText, PS_SOLID, GetSystemMetrics(SM_CXSMICON)/16);
    HPEN hPenOld = (HPEN)SelectObject(hDC, hNew);

    //should probably create a pen here 
    Polyline(hDC, pt, 2);
    SelectObject(hDC, hPenOld);
    DeleteObject(hNew);

    //end line
	
	//adjust our display by the width of the fraction and draw the the whole number (m_szResult)
	rcTemp.right -= (sizeText.cx + GetSystemMetrics(SM_CXSMICON)/2);

	DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontDisplay, m_oBtnManger.GetSkin()->m_crText);




	m_oCalc->GetEquation(szLastPressed);

	//draw the last pressed 
	rcTemp = m_rcDisplayArea;
	rcTemp.left += TEXT_INDENT;
	
	DrawText(hDC, szLastPressed, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);//, RGB(170,109,82));
}


void CIssCalcInterface::DrawDisplayTextFracExp(HDC hDC, RECT& rc)
{
	static TCHAR szLastPressed[STRING_MAX];
	RECT rcTemp = m_rcDisplayArea;

	//draw the display text
	m_oCalc->GetAnswer(m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

	//we need to figure out if we have a fraction or not
	//result cannot have a decimal place
	//all three values should be filled to be interesting
	if(m_oStr->Find(m_szResult, _T(".")) != -1 || m_oStr->GetLength(m_szValue) == 0)
	{	//we don't have a fraction ... pretty clever eh?
		DrawDisplayTextExpression(hDC, rc);
		return;
	}

	//m_szValue is the numerator
	//m_szExp is the denominator
	rcTemp.bottom -= GetSystemMetrics(SM_CXSMICON);
	DrawText(hDC, m_szValue, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);
	rcTemp.top		+= GetSystemMetrics(SM_CXSMICON);
	rcTemp.bottom	+= GetSystemMetrics(SM_CXSMICON);

	DrawText(hDC, m_szExp, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);

	//get the width of the fraction so we can offset the whole number and draw the fraction line
	SIZE sizeText;

	HFONT hFontOld = (HFONT)SelectObject(hDC, m_hFontDisplayTop);
	GetTextExtentPoint(hDC, m_szExp, m_oStr->GetLength(m_szExp), &sizeText);
	SelectObject(hDC, hFontOld);

	//now lets draw the line between the fraction
	static POINT pt[2];
	pt[0].y = pt[1].y = rcTemp.top + GetSystemMetrics(SM_CXSMICON)/4;
	pt[0].x = rcTemp.right;
	pt[1].x = pt[0].x - sizeText.cx;

	//should probably create a pen here 
	Polyline(hDC, pt, 2);

	//adjust our display by the width of the fraction and draw the the whole number (m_szResult)
	rcTemp.right -= (sizeText.cx + TEXT_INDENT);
	rcTemp.top = m_rcDisplayArea.top;//make lots of room

	DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_BOTTOM, 
		m_hFontDisplay, m_oBtnManger.GetSkin()->m_crText);


	m_oCalc->GetEquation(szLastPressed);

	//draw the equation
	rcTemp = m_rcDisplayArea;
	rcTemp.left += TEXT_INDENT;
	rcTemp.top += GetSystemMetrics(SM_CXICON)/5;
	DrawText(hDC, szLastPressed, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);//, RGB(170,109,82));
}

void CIssCalcInterface::DrawDisplayTextRPN(HDC hDC, RECT& rc)
{
    TCHAR szLastPressed[STRING_MAX];
    RECT rcTemp = m_rcDisplayArea;
    rcTemp.left += TEXT_INDENT;
    rcTemp.bottom += GetSystemMetrics(SM_CXICON)/16;//down by 2

    //draw the display text
    m_oCalc->GetAnswer(m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

    rcTemp.bottom -= TEXT_INDENT/3;
    DrawText(hDC, _T("X:"), rcTemp, DT_LEFT | DT_BOTTOM, 
        m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);

    DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_BOTTOM, 
        m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);

    m_oCalc->GetEquation(szLastPressed);
    

    if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
    {
        m_oCalc->GetRPNStackEntry(0, m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

        //draw the last pressed 
        rcTemp = m_rcDisplayArea;
        rcTemp.left += TEXT_INDENT;
        rcTemp.top += (TEXT_INDENT + (IsVGA()?2:1));

        //   DrawText(hDC, szLastPressed, rcTemp, DT_RIGHT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);
        DrawText(hDC, _T("Y:"), rcTemp, DT_LEFT | DT_VCENTER, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);
        DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_VCENTER, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);

        m_oCalc->GetRPNStackEntry(1, m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);
        DrawText(hDC, _T("Z:"), rcTemp, DT_LEFT | DT_TOP, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);
        DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_TOP, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);
    }
    else
    {
        m_oCalc->GetRPNStackEntry(0, m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

        //draw the last pressed 
        rcTemp = m_rcDisplayArea;
        rcTemp.left += TEXT_INDENT;
        rcTemp.top += (TEXT_INDENT + (IsVGA()?2:1));

        //   DrawText(hDC, szLastPressed, rcTemp, DT_RIGHT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);
        DrawText(hDC, _T("Y:"), rcTemp, DT_LEFT | DT_TOP, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);
        DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_TOP, m_hFontRPN, m_oBtnManger.GetSkin()->m_crText);
    }
}

void CIssCalcInterface::DrawDisplayTextExpression(HDC hDC, RECT& rc)
{
	TCHAR szLastPressed[STRING_MAX];
	RECT rcTemp = m_rcDisplayArea;
	rcTemp.bottom += GetSystemMetrics(SM_CXICON)/16;//down by 2

	//draw the display text
	m_oCalc->GetAnswer(m_szResult, m_szValue, m_szExp, TRUE, m_bUseRegionalSettings);

	if(m_oStr->GetLength(m_szExp) == 0)
	{	
		DrawText(hDC, m_szResult, rcTemp, DT_RIGHT | DT_BOTTOM, 
			m_hFontExponent, m_oBtnManger.GetSkin()->m_crText);
	}
	else
	{	//gotta make room for the exponent
		rcTemp.bottom -= TEXT_INDENT;
		DrawText(hDC, m_szExp, rcTemp, DT_RIGHT | DT_BOTTOM, 
			m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);
		rcTemp.bottom += TEXT_INDENT;
		//rcTemp = m_rcDisplayArea;
		rcTemp.right -= GetSystemMetrics(SM_CXICON)*3/4;
		DrawText(hDC, m_szValue, rcTemp, DT_RIGHT | DT_BOTTOM, 
			m_hFontExponent, m_oBtnManger.GetSkin()->m_crText);
	}

	m_oCalc->GetEquation(szLastPressed);

	//draw the last pressed 
	rcTemp = m_rcDisplayArea;
	rcTemp.left += TEXT_INDENT;
	rcTemp.top += TEXT_INDENT;

    //check the width 
    SIZE sizeText;
    HFONT hOld = (HFONT)SelectObject(hDC, m_hFontDisplayTop);
    GetTextExtentPoint(hDC, szLastPressed, m_oStr->GetLength(szLastPressed), &sizeText);
    SelectObject(hDC, hOld);

    if(sizeText.cx > WIDTH(rcTemp))
        DrawText(hDC, szLastPressed, rcTemp, DT_RIGHT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);
    else
	    DrawText(hDC, szLastPressed, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnManger.GetSkin()->m_crText);

    //don't forget to draw the up and down arrows for the equation buffer yo
    //lets try triangles in the top left
    if(m_oCalc->IsScrollUpAvalible())
        DrawArrows(hDC, TRUE, GetSystemMetrics(SM_CXSMICON)*2/3, m_rcDisplayArea.top);
    if(m_oCalc->IsScrollDownAvalible())
        DrawArrows(hDC, FALSE, GetSystemMetrics(SM_CXSMICON)*4/3, m_rcDisplayArea.top);
}

void CIssCalcInterface::DrawArrows(HDC hdc, BOOL bUp, int iX, int iY)
{
    //Draw a down arrow

    POINT pt[7];

    int iScale = GetSystemMetrics(SM_CXICON)/32;

    int iWidth			= 8*iScale;
    int iWidthOfStick	= 0;
    //    int iHeight			= 17*iScale;
    //    int iHeightOfStick	= 11*iScale;

    int iHeight			= 4*iScale;
    int iHeightOfStick	= 0*iScale;

    if(bUp)
    {
        pt[0].x = iX - iWidthOfStick/2;
        pt[0].y = iY + iHeight*3/2;//offsetting 

        pt[1].x = pt[0].x + iWidthOfStick;
        pt[1].y = pt[0].y;

        pt[2].x = pt[1].x;
        pt[2].y = pt[1].y - iHeightOfStick;

        pt[3].x = pt[1].x + iWidth/2 - iWidthOfStick/2;
        pt[3].y = pt[2].y;

        pt[4].x = pt[0].x + iWidthOfStick/2;
        pt[4].y = pt[2].y - iHeight + iHeightOfStick;

        pt[5].x = pt[3].x - iWidth;
        pt[5].y = pt[3].y;

        pt[6].x = pt[0].x;
        pt[6].y = pt[3].y;	
    }
    else
    {
        pt[0].x = iX - iWidthOfStick/2;
        pt[0].y = iHeight/2 + iY;

        pt[1].x = pt[0].x + iWidthOfStick;
        pt[1].y = pt[0].y;

        pt[2].x = pt[1].x;
        pt[2].y = pt[1].y + iHeightOfStick;

        pt[3].x = pt[1].x + iWidth/2 - iWidthOfStick/2;
        pt[3].y = pt[2].y;

        pt[4].x = pt[0].x + iWidthOfStick/2;
        pt[4].y = pt[2].y + iHeight - iHeightOfStick;

        pt[5].x = pt[3].x - iWidth;
        pt[5].y = pt[3].y;

        pt[6].x = pt[0].x;
        pt[6].y = pt[3].y;
    }

          

    HPEN   hpNew			= CreatePen(PS_SOLID, 1, m_oBtnMan->GetSkin()->m_crText);
    HPEN   hpOld			= (HPEN)SelectObject(hdc, hpNew);

    HBRUSH brNew			= CreateSolidBrush(m_oBtnMan->GetSkin()->m_crTextTop);
    HBRUSH brOld			= (HBRUSH)SelectObject(hdc, brNew);

    Polygon(hdc, pt, 7);

    SelectObject(hdc, hpOld);
    DeleteObject(hpNew);

    SelectObject(hdc, brOld);
    DeleteObject(brNew);
}

void CIssCalcInterface::DrawSelector(HDC hDC, RECT& rc)
{
	if(m_bShowSelector == FALSE)
		return;

    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
        m_oGraphInt.DrawSelector(hDC, rc);
        break;
    case CALC_Constants:
        m_oConstInt.DrawSelector(hDC, rc);
        return;
        break;
    default:
        break;
    }

	CCalcDynBtn* oBtn = NULL;
	oBtn = m_arrAltButtons[m_iSelector];
	
	if(oBtn == NULL)
		return;

	RECT rcTemp = oBtn->GetButtonRect();

	int iRadius = GetSystemMetrics(SM_CXICON)/6;

	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
	HPEN oldPen = (HPEN)SelectObject(hDC, GetStockObject(WHITE_PEN));
//	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
	RoundRect(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

	SelectObject(hDC, oldBrush);
	SelectObject(hDC, oldPen);
}

void CIssCalcInterface::DrawButtons(HDC hDC, RECT& rc)
{
	LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();
    CIssVector<ButtonType>* arrBtnType = &m_oBtnManger.GetCurrentLayout()->sMainButtons;

	if(sLayout == NULL)
		return;

	CCalcDynBtn* oBtn = NULL;
	int i, j;

	HFONT hFontTemp;

    int iButton = 0;

	//Main Buttons
    if(rc.left < m_rcMainButtons.right ||
        rc.right > m_rcMainButtons.left ||
        rc.top < m_rcMainButtons.bottom ||
        rc.bottom > m_rcMainButtons.top)
	for(i = 0; i < sLayout->iMainBtnColumns; i++)
	{
		for(j = 0; j < sLayout->iMainBtnRows; j++)
		{
            oBtn = NULL;
			oBtn = m_arrMainButtons[sLayout->iMainBtnColumns*j+i];
			if(oBtn)
			{
				//now lets optimize a bit
				if(rc.left > oBtn->GetButtonRect().right ||
					rc.right < oBtn->GetButtonRect().left ||
					rc.top > oBtn->GetButtonRect().bottom ||
					rc.bottom < oBtn->GetButtonRect().top)
					continue;
				
                iButton = sLayout->iMainBtnColumns*j+i;

				//we're good... carry on
				if(m_oStr->GetLength(sLayout->sMainButtons[iButton]->szLabel) > 2)
					hFontTemp = m_hFontMainLongText;
				else
					hFontTemp = m_hFontMainBtnText;
								
#ifdef WIN32_PLATFORM_WFSP //draw the extra labels yo
				//we're going to do a bit of a gross hack ... if we're on landscape we're not going to draw the main buttons
				if(GetDeviceType() == DEVTYPE_SPLand)
					continue;

        		else if(GetDeviceType() == DEVTYPE_SP)
				{
					oBtn->Draw(hFontTemp, hDC, sLayout->sMainButtons[iButton]->szLabel, NULL, 
						sLayout->sMainAltButtons[iButton]->szLabel, NULL, m_hFontMainLongText); //there's no exponents in the main buttons anyways
				}
				else
#endif
				{
					oBtn->Draw(hFontTemp, hDC, sLayout->sMainButtons[iButton]->szLabel);
				}

                //here we'll check for special cases
                //backspace
                if((*arrBtnType)[iButton]->iButtonType == INPUT_Clear &&
                    (*arrBtnType)[iButton]->iButtonValue == CLEAR_BackSpace)
                {
                    DrawBackSpace(hDC, oBtn->GetButtonRect(), 
                        m_oBtnManger.GetSkin()->GetButton((*arrBtnType)[iButton]->iSkinIndex)->TextColor,
                        m_oBtnManger.GetSkin()->GetButton((*arrBtnType)[iButton]->iSkinIndex)->TextColor2,
                        m_oBtnManger.GetSkin()->m_bUseTextShadow);
                }
			}
		}
	}

	//Alt Buttons ... lets do this clever
    ButtonType* sBtnType = NULL;
    //bounds checking for performance
    if(rc.left < m_rcAltButtons.right ||
        rc.right > m_rcAltButtons.left ||
        rc.top < m_rcAltButtons.bottom ||
        rc.bottom > m_rcAltButtons.top)
	for(i = 0; i < sLayout->iAltBtnColumns; i++)
	{
		for(j = 0; j < sLayout->iAltBtnRows; j++)
		{
			arrBtnType = NULL;
			sBtnType = NULL;
			oBtn = NULL;

            iButton = sLayout->iAltBtnColumns*j+i;
			
			switch(m_oCalc->GetCalcState())
			{
			case CALCSTATE_2ndF:
				arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndFButtons;
				oBtn = m_arr2ndFButtons[iButton];
				break;
			case CALCSTATE_Hyp:
				arrBtnType = &m_oBtnManger.GetCurrentLayout()->sHypButtons;
				oBtn = m_arrHypFButtons[iButton];
				break;
			case CALCSTATE_2ndF_Hyp:
				arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons;
				oBtn = m_arr2ndHypFButtons[iButton];
				break;
			default:
				arrBtnType = &m_oBtnManger.GetCurrentLayout()->sAltButtons;
				oBtn = m_arrAltButtons[iButton];
				break;
			}

			if(oBtn)
			{
				//now lets optimize a bit
				if(rc.left > oBtn->GetButtonRect().right ||
					rc.right < oBtn->GetButtonRect().left ||
					rc.top > oBtn->GetButtonRect().bottom ||
					rc.bottom < oBtn->GetButtonRect().top)
					continue;

				if(arrBtnType)
					sBtnType = (*arrBtnType)[iButton];
				if(sBtnType == NULL)
				{
					ASSERT(0);//means our buttons haven't been initialized
					return;//not good
				}

                int iLen = m_oStr->GetLength(sBtnType->szLabel);

                if(iLen < 3 && m_oStr->IsEmpty(sBtnType->szLabelExp))
					hFontTemp = m_hFontAltBtnText;
                else if(iLen < 5 || FALSE == m_oStr->IsEmpty(sBtnType->szLabelExp))
                    hFontTemp = m_hFontAltLongText;
				else
					hFontTemp = m_hFontAltXLText;
                
				if(sBtnType->szLabel == NULL)
					int i = 2;

				oBtn->Draw(hFontTemp, hDC, sBtnType->szLabel, sBtnType->szLabelExp, NULL, NULL, hFontTemp);

				//here we'll check for special cases
				//backspace
				if((*arrBtnType)[iButton]->iButtonType == INPUT_Clear &&
					(*arrBtnType)[iButton]->iButtonValue == CLEAR_BackSpace)
				{
					DrawBackSpace(hDC, oBtn->GetButtonRect(), 
                        m_oBtnManger.GetSkin()->GetButton((*arrBtnType)[iButton]->iSkinIndex)->TextColor,
                         m_oBtnManger.GetSkin()->GetButton((*arrBtnType)[iButton]->iSkinIndex)->TextColor2,
                          m_oBtnManger.GetSkin()->m_bUseTextShadow);
    			}
			}
		}
	}
}

BOOL CIssCalcInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	DetectAndSetDeviceType();//sets the device type in CHandleButtons

    //initialize our menu
    m_oMenu.Init(hWnd, m_hInst, m_oBtnManger.GetSkinType());

    GetClientRect(hWnd, &m_rcSize);
    m_hWndDlg = hWnd;

	//and delete the fonts
	CIssGDIEx::DeleteFont(m_hFontMainBtnText);
	CIssGDIEx::DeleteFont(m_hFontMainLongText);
	CIssGDIEx::DeleteFont(m_hFontAltBtnText);
	CIssGDIEx::DeleteFont(m_hFontAltLongText);
    CIssGDIEx::DeleteFont(m_hFontAltXLText);

	InitDisplay();
	InitAltButtons();
	InitMainButtons();
    InitMenu();
    UpdateWindowText();

	SetDisplayRects();//top displays

    m_oConstInt.SetPosition(hWnd, m_iYMainStart, m_iYAltStart);

	//Unit Converter

    CCalcInterace* oInt = NULL;

    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
        oInt = &m_oGraphInt;
        break;
    case CALC_Constants:
        oInt = &m_oConstInt;
        break;
    default:
        oInt = NULL;
        break;
    }

    if(oInt != NULL)
    {
        TypeCalcInit sInit;
        sInit.oBtnHand  = &m_oBtnHandler;
        sInit.oBtnMan   = &m_oBtnManger;
        sInit.oCalc     = m_oCalc;
        sInit.hWnd      = hWnd;
        sInit.hInst     = m_hInst;
        sInit.wndMenu   = &m_wndMenu;
        sInit.oMenu     = &m_oMenu;
        sInit.oConstants= &m_oConstants;

        oInt->Init(&sInit);
    }
    
    m_oGraphInt.OnSize(hWnd, wParam, lParam);
    m_oConstInt.OnSize(hWnd, wParam, lParam);

	return TRUE;
}


void CIssCalcInterface::SetDisplayRects()
{
	RECT rcTemp = m_rcDisplayArea;
	rcTemp.bottom += GetSystemMetrics(SM_CXICON)/16;//down by 2

#ifdef WIN32_PLATFORM_WFSP
#define MULT_RATIO 3/2
#else
#define MULT_RATIO 2
#endif

	rcTemp.left += GetSystemMetrics(SM_CXSMICON);
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*MULT_RATIO;
	m_rc2ndF = rcTemp;

	rcTemp.left = rcTemp.right;
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*MULT_RATIO;
	m_rcHyp = rcTemp;

	rcTemp.left = rcTemp.right;
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*MULT_RATIO;
	m_rcBase = rcTemp;

	rcTemp.left = rcTemp.right;
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*MULT_RATIO;
	m_rcDRG = rcTemp;

	rcTemp.left = rcTemp.right;
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*7/3;
	m_rcDisplay = rcTemp;

	rcTemp.left = rcTemp.right;
	rcTemp.right = rcTemp.left + GetSystemMetrics(SM_CXSMICON)*MULT_RATIO;
	m_rcM1 = rcTemp;
}

BOOL CIssCalcInterface::InitDisplay()
{
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

#ifdef UNDER_CE
    rcClient.top = 0;
    rcClient.bottom = GetSystemMetrics(SM_CYSCREEN);
    //quick temp hack
#endif
	BOOL bWideOrSquare		= FALSE;

	//are we in wide screen??
	if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN))
		bWideOrSquare = TRUE;

	if(bWideOrSquare)
	{
		if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN)) //wide
		{	//Landscape
#ifdef WIN32_PLATFORM_WFSP
			//smartphone landscape ... don't need the main buttons
			m_iYMainStart				= GetSystemMetrics(SM_CYSCREEN);			//leave this for now
			m_iYAltStart				= DISPLAY_HEIGHT*4/3 - DISPLAY_INDENT;			//start the alt buttons under the display

			m_rcDisplayArea.left		= DISPLAY_INDENT;
			m_rcDisplayArea.right		= GetSystemMetrics(SM_CXSCREEN);		// full width for the display
			m_rcDisplayArea.top			= DISPLAY_INDENT;
			m_rcDisplayArea.bottom		= m_iYAltStart;


#else//pocket PC
			m_iYMainStart = DISPLAY_HEIGHT - DISPLAY_INDENT;			
			m_iYAltStart = m_rcSize.top;

			m_rcDisplayArea.left		= DISPLAY_INDENT;
			m_rcDisplayArea.right		= HEIGHT(rcClient)-DISPLAY_INDENT;
			m_rcDisplayArea.top			= DISPLAY_INDENT;
			m_rcDisplayArea.bottom		= m_iYMainStart;
#endif
		}
		else //square
		{
			m_iYAltStart = DISPLAY_HEIGHT - DISPLAY_INDENT;
			
			m_rcDisplayArea.left		= DISPLAY_INDENT;
			m_rcDisplayArea.right		= GetSystemMetrics(SM_CXSCREEN)-DISPLAY_INDENT,DISPLAY_INDENT;
			m_rcDisplayArea.top			= DISPLAY_INDENT;
			m_rcDisplayArea.bottom		= m_iYAltStart;
		}
	}
	else //portrait
	{
		m_iYAltStart = DISPLAY_HEIGHT - DISPLAY_INDENT;

        if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)//ultra wide
            m_iYAltStart = DISPLAY_TALL - DISPLAY_INDENT;
		
		m_rcDisplayArea.left			= DISPLAY_INDENT;
		m_rcDisplayArea.right			= GetSystemMetrics(SM_CXSCREEN)-DISPLAY_INDENT,DISPLAY_INDENT;
		m_rcDisplayArea.top				= DISPLAY_INDENT;
		m_rcDisplayArea.bottom			= m_iYAltStart;
	}

	CalcProSkins* oSkin = m_oBtnManger.GetSkin();//makes for tidier code

	m_oDisplay.Init(m_rcDisplayArea, oSkin->m_typeDisplay.ButtonColor1
					, oSkin->m_typeDisplay.ButtonColor2
					, oSkin->m_typeDisplay.OutlineColor1
					, oSkin->m_typeDisplay.OutlineColor2,
					DISPLAY_Grad_DS);

    //wow ... seriously ... wow
	m_rcDisplayArea.left	+= DISPLAY_INDENT;
	m_rcDisplayArea.right	-= 3*DISPLAY_INDENT;
	m_rcDisplayArea.top		+= DISPLAY_INDENT;
	m_rcDisplayArea.bottom	-= DISPLAY_INDENT;

	return TRUE;
}

//always called AFTER InitAltButtons
BOOL CIssCalcInterface::InitMainButtons()
{	
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

#ifdef UNDER_CE
    rcClient.top = 0;
    rcClient.bottom = GetSystemMetrics(SM_CYSCREEN);
    //quick temp hack
#endif

    //clean em up first
	DeleteDynBtnVector(&m_arrMainButtons);

	//based on our m_rcSize;
	int iButtonWidth	= 0;
	int iButtonHeight	= 0;
	RECT rcButtonSize	= {0,0,0,0};

	BOOL bWide			= FALSE;
	BOOL bSquare		= FALSE;
	LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout(m_hInst);

	if(sLayout == NULL ||
		sLayout->iMainBtnColumns == 0 ||
		sLayout->iMainBtnRows == 0)
		return FALSE;

	CCalcDynBtn* oBtn = NULL;
	
	//are we in wide screen??
	if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
		bWide = TRUE;
	else if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
		bSquare = TRUE;

	m_rcMainButtons.top = m_iYMainStart;
	m_rcMainButtons.left = 0;

	//Main Buttons First
	if(bWide)
		m_rcMainButtons.right		= HEIGHT(rcClient);
	else
	    m_rcMainButtons.right		= WIDTH(rcClient);
	
    m_rcMainButtons.bottom	= HEIGHT(m_rcSize) - m_iYMainStart + m_rcMainButtons.top - m_oMenu.GetMenuHeight();
	iButtonWidth		= (WIDTH(m_rcMainButtons) - 2*BTN_EDGE_INDENT - (sLayout->iMainBtnColumns - 1)*BTN_SPACING) / sLayout->iMainBtnColumns;
	iButtonHeight		= (HEIGHT(m_rcMainButtons) - BTN_SPACING - (sLayout->iMainBtnRows - 1)*BTN_SPACING) / sLayout->iMainBtnRows;

	CalcProSkins* oSkin = m_oBtnManger.GetSkin();
	int	iSkinIndex = 0;
	int iCurrentBtn = 0;
	
	for(int j = 0; j < sLayout->iMainBtnRows; j++)
	{
		for(int i = 0; i < sLayout->iMainBtnColumns; i++)
		{
			iCurrentBtn = sLayout->iMainBtnColumns*j+i;
			
			if(iCurrentBtn == MAX_MAIN_BUTTONS)
            {	ASSERT(0);}

			rcButtonSize.left	= BTN_EDGE_INDENT + i*(BTN_SPACING+iButtonWidth);
			rcButtonSize.top	= BTN_SPACING + j*(BTN_SPACING+iButtonHeight) + m_iYMainStart;
			rcButtonSize.right	= rcButtonSize.left + iButtonWidth;
			rcButtonSize.bottom	= rcButtonSize.top + iButtonHeight;

			//get the skin index
			iSkinIndex = sLayout->sMainButtons[iCurrentBtn]->iSkinIndex;

			CCalcDynBtn* oBtn = new CCalcDynBtn;
			if(oBtn == NULL)
				break;

			//just a helper function so we're not writing the sames code over and over ...
			//destroys the old, sets the colors, checks for a previous GDI, creates the button and saves the GDI if necessary
			//we can't assume these buttons are being created in order .... hmmmm
			InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);

			m_arrMainButtons.AddElement(oBtn);
		}
	}

	//just in case we have an odd shaped screen
	int iTemp = min(iButtonWidth, iButtonHeight);

	//can't use the inline ifs because it effectively puts brackets around the defines which then don't evaluate correctly (integer math)
	if(bSquare)
	{
		if(m_hFontMainBtnText == FALSE)
			m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO_SQ, FW_BOLD, TRUE);
		if(m_hFontMainLongText == FALSE)
			m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_SQ, FW_BOLD, TRUE);
	}
    else if(bWide)
    {
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_LAND, FW_BOLD, TRUE);
    }
    else if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
    {   //wide in portrait
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO - (IsVGA()?4:2), FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO - (IsVGA()?6:3), FW_BOLD, TRUE);
    }
	else
	{   //portrait
		if(m_hFontMainBtnText == FALSE)
			m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
		if(m_hFontMainLongText == FALSE)
			m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO, FW_BOLD, TRUE);
	}
	return TRUE;
}

//always called BEFORE InitMainButtons
//this will init the 2ndF/Hyp alt buttons too
HRESULT CIssCalcInterface::InitAltButtons()
{	
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

#ifdef UNDER_CE
    rcClient.top = 0;
    rcClient.bottom = GetSystemMetrics(SM_CYSCREEN);
    //quick temp hack
#endif

    HRESULT hr = S_OK;

	//since we're starting over and we're first
	DeleteButtonVector();//array of pointers to buttons with actual GDIs
	DeleteDynBtnVector(&m_arrAltButtons);//and of course the buttons themselves
	DeleteDynBtnVector(&m_arr2ndFButtons);
	DeleteDynBtnVector(&m_arrHypFButtons);
	DeleteDynBtnVector(&m_arr2ndHypFButtons);


	//based on our m_rcSize;
	int iButtonWidth	= 0;
	int iButtonHeight	= 0;
	int iAreaWidth		= 0;
	int iAreaHeight		= 0;
	RECT rcButtonSize	= {0,0,0,0};

	BOOL bWide			= FALSE;
	BOOL bSquare		= FALSE;
    //checked below .. all good
    LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();

    //are we in wide screen??
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        bWide = TRUE;
    else if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
        bSquare = TRUE;

	if(sLayout == NULL ||
		sLayout->iAltBtnRows == 0 ||
		sLayout->iAltBtnColumns == 0)
    {
        //we're still gonna m_iYMainStart ... yeah its a hack  .. bite me
        if(!bWide)
        {
            iAreaHeight		= (m_rcSize.bottom - m_rcSize.top - m_oMenu.GetMenuHeight())*ALT_BTNS_HEIGHT_PCNT/100;
            m_iYMainStart   = m_iYAltStart+iAreaHeight;
        }
#ifdef WIN32_PLATFORM_WFSP 
        if(bWide)
        {

        }

#endif
        goto Error;
    }

#ifndef WIN32_PLATFORM_WFSP
	if(bWide)//we're in landscape
	{
		iAreaWidth			= WIDTH(rcClient) - HEIGHT(rcClient);
		iAreaHeight			= (m_rcSize.bottom - m_rcSize.top);//since we're not using all 100% - alt buttons on the side
		iButtonWidth		= (iAreaWidth - BTN_EDGE_INDENT - (sLayout->iAltBtnRows - 1)*BTN_SPACING) / sLayout->iAltBtnRows;
		iButtonHeight		= (iAreaHeight - BTN_TOP_INDENT - m_oMenu.GetMenuHeight() - (sLayout->iAltBtnColumns - 1)*BTN_SPACING) / sLayout->iAltBtnColumns;

		m_rcAltButtons.left = HEIGHT(rcClient);
		m_rcAltButtons.right= WIDTH(rcClient);
		m_rcAltButtons.top	= m_iYAltStart;
		m_rcAltButtons.bottom= m_iYAltStart + iAreaHeight - m_oMenu.GetMenuHeight();
	}
	else
#endif
	{

		iAreaHeight			= (m_rcSize.bottom - m_rcSize.top - m_oMenu.GetMenuHeight())*ALT_BTNS_HEIGHT_PCNT/100;
#ifdef WIN32_PLATFORM_WFSP
        if(bWide)
            iAreaHeight			= (m_rcSize.bottom - m_rcSize.top)-HEIGHT(m_rcDisplayArea) - m_oMenu.GetMenuHeight() - TEXT_INDENT;
#endif	
		iAreaWidth			= GetSystemMetrics(SM_CXSCREEN);
		
		iButtonWidth		= (iAreaWidth - 2*BTN_EDGE_INDENT - (sLayout->iAltBtnColumns - 1)*BTN_SPACING) / sLayout->iAltBtnColumns;
		iButtonHeight		= (iAreaHeight - BTN_TOP_INDENT - (sLayout->iAltBtnRows - 1)*BTN_SPACING) / sLayout->iAltBtnRows;

		m_rcAltButtons.left = 0;
		m_rcAltButtons.right= GetSystemMetrics(SM_CXSCREEN);
		m_rcAltButtons.top	= m_iYAltStart;
		m_rcAltButtons.bottom= m_iYAltStart + iAreaHeight;
	}

	CalcProSkins* oSkin = m_oBtnManger.GetSkin();
	int	iSkinIndex = 0;
	int iCurrentBtn = 0;

	for(int j = 0; j < sLayout->iAltBtnRows; j++)
	{
		for(int i = 0; i < sLayout->iAltBtnColumns; i++)
		{
			iCurrentBtn = sLayout->iAltBtnColumns*j+i;
#ifndef WIN32_PLATFORM_WFSP
			if(bWide)//if we're in wide mode we'll do things a little differently
			{
				rcButtonSize.left	= (sLayout->iAltBtnRows-1-j)*(BTN_SPACING+iButtonWidth) + HEIGHT(rcClient);
				rcButtonSize.top	= BTN_TOP_INDENT + (sLayout->iAltBtnColumns-1-i)*(BTN_SPACING+iButtonHeight);
				rcButtonSize.right	= rcButtonSize.left + iButtonWidth;
				rcButtonSize.bottom	= rcButtonSize.top + iButtonHeight;
			}
			else
#endif
			{
				rcButtonSize.left	= BTN_EDGE_INDENT + i*(BTN_SPACING+iButtonWidth);
				rcButtonSize.top	= BTN_TOP_INDENT + j*(BTN_SPACING+iButtonHeight) + m_iYAltStart;
				rcButtonSize.right	= rcButtonSize.left + iButtonWidth;
				rcButtonSize.bottom	= rcButtonSize.top + iButtonHeight;
			}

			//get the skin index
			iSkinIndex = sLayout->sAltButtons[iCurrentBtn]->iSkinIndex;

			CCalcDynBtn* oBtn = new CCalcDynBtn;
			if(oBtn == NULL)
				break;

			//just a helper function so we're not writing the sames code over and over ...
			InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);
			m_arrAltButtons.AddElement(oBtn);

			//now ... 2ndF
			if(sLayout->s2ndFButtons.GetSize() > 0 &&
				sLayout->s2ndFButtons.GetSize() >= iCurrentBtn)
			{
				oBtn = NULL;
				oBtn = new CCalcDynBtn;
				if(oBtn == NULL)
					break;
				iSkinIndex = sLayout->s2ndFButtons[iCurrentBtn]->iSkinIndex;//not initialized ??
				InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);
				m_arr2ndFButtons.AddElement(oBtn);
			}
			//2ndF & HYP
			if(sLayout->s2ndHypFButtons.GetSize() > 0 &&
				sLayout->s2ndHypFButtons.GetSize() >= iCurrentBtn)
			{
				oBtn = NULL;
				oBtn = new CCalcDynBtn;
				if(oBtn == NULL)
					break;
				iSkinIndex = sLayout->s2ndHypFButtons[iCurrentBtn]->iSkinIndex;//not initialized ??
				InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);
				m_arr2ndHypFButtons.AddElement(oBtn);
			}
			//Just HYP
			if(sLayout->sHypButtons.GetSize() > 0 &&
				sLayout->sHypButtons.GetSize() >= iCurrentBtn)
			{
				oBtn = NULL;
				oBtn = new CCalcDynBtn;
				if(oBtn == NULL)
					break;
				iSkinIndex = sLayout->sHypButtons[iCurrentBtn]->iSkinIndex;//not initialized ??
				InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);
				m_arrHypFButtons.AddElement(oBtn);
			}
		}
	}

	//set the location of the main buttons here
	if(!bWide)
		m_iYMainStart = rcButtonSize.bottom;

	int iTemp = min(iButtonHeight, iButtonWidth);

	if(bSquare)
	{
		if(m_hFontAltBtnText == NULL)
			m_hFontAltBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO_SQ, FW_NORMAL, TRUE);
		if(m_hFontAltLongText == NULL)
			m_hFontAltLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_SQ, FW_NORMAL, TRUE);
        if(m_hFontAltXLText == NULL)
            m_hFontAltXLText = CIssGDIEx::CreateFont(iTemp*3/5, FW_NORMAL, TRUE);
    }
    else if(bWide)
    {
        if(m_hFontAltBtnText == FALSE)
            m_hFontAltBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_NORMAL, TRUE);
        if(m_hFontAltLongText == FALSE)
            m_hFontAltLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_LAND, FW_NORMAL, TRUE);
        if(m_hFontAltXLText == NULL)
            m_hFontAltXLText = CIssGDIEx::CreateFont(iTemp*9/20, FW_NORMAL, TRUE);
    }
    else if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
    {   //wide in portrait
        if(m_hFontAltBtnText == FALSE)
            m_hFontAltBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_NORMAL, TRUE);
        if(m_hFontAltLongText == FALSE)
            m_hFontAltLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_LAND, FW_NORMAL, TRUE);
        if(m_hFontAltXLText == NULL)
            m_hFontAltXLText = CIssGDIEx::CreateFont(iTemp*17/40, FW_NORMAL, TRUE);
    }
	else
	{
		if(m_hFontAltBtnText == NULL)
			m_hFontAltBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_NORMAL, TRUE);
		if(m_hFontAltLongText == NULL)
			m_hFontAltLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO, FW_NORMAL, TRUE);
        if(m_hFontAltXLText == NULL)
            m_hFontAltXLText = CIssGDIEx::CreateFont(iTemp*11/20, FW_NORMAL, TRUE);
	}


Error:
	return hr;
}

//destroys the old, sets the colors, checks for a previous GDI, creates the button and saves the GDI if necessary
BOOL CIssCalcInterface::InitButton(CCalcDynBtn* oDynButton, RECT* rc, CalcProSkins* oSkin, int iSkinIndex, int iWidth, int iHeight)
{
	oDynButton->Destroy();
	oDynButton->SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, oSkin->GetUseShadow());
	oDynButton->InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
		oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
		oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
		oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

	CCalcDynBtn* oBtn = GetButton(iWidth, iHeight, iSkinIndex);

	//init the Button
	oDynButton->Init(*rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);

	//save the button
	if(oBtn == NULL)
		SaveButton(oDynButton, iWidth, iHeight, iSkinIndex);

	return TRUE;
}


//file menu
BOOL CIssCalcInterface::OnMenuLeft()
{
    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu.ResetContent();

    //m_wndMenu.AddCategory(_T("Calc Pro"));
    m_oStr->StringCopy(szText, ID(IDS_MENU_Copy), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_Copy);
    m_oStr->StringCopy(szText, ID(IDS_MENU_Paste), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_Paste);
    m_wndMenu.AddSeparator();

    //if there's no tape in use don't show the option yo
    if(m_oCalc->GetUseTape() == TRUE)
    {
        m_oStr->StringCopy(szText, ID(IDS_MENU_ViewTape), STRING_MAX, m_hInst);
        m_wndMenu.AddItem(szText, IDMENU_ViewTape);
        m_wndMenu.AddSeparator();
    }


    m_oStr->StringCopy(szText, m_oStr->GetText(ID(IDS_MENU_Options), m_hInst));
    m_wndMenu.AddItem(szText, IDMENU_Options);

    m_oStr->StringCopy(szText, ID(IDS_MENU_Help), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_Help);
    m_oStr->StringCopy(szText, ID(IDS_MENU_About), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_About);

#ifndef NMA
    m_oStr->StringCopy(szText, ID(IDS_MENU_MoreApps), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_MoreApps);
#endif

    m_wndMenu.AddSeparator();
    m_oStr->StringCopy(szText, ID(IDS_MENU_Exit), STRING_MAX, m_hInst);
    m_wndMenu.AddItem(szText, IDMENU_Exit);
    
    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
    {
        m_wndMenu.SetItemHeights(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CXICON));
    }

    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    RECT rcBtn;
    rcBtn.left  = 0;
    rcBtn.top   = rc.bottom - m_oMenu.GetMenuHeight();
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right/2;

	POINT ptBtnScreen;
	ptBtnScreen.x = 0;
	ptBtnScreen.y = 0;

    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu.GetMenuHeight();

    m_wndMenu.PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn)
#ifdef WIN32_PLATFORM_WFSP
        );
#else
        ,ADJUST_Bottom);
#endif


	return TRUE;
}

//menu view
BOOL CIssCalcInterface::OnMenuRight()
{
    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu.ResetContent();
    m_wndMenu.SetDeleteItemFunc(DeleteMenuItem);

    m_oStr->StringCopy(szText, ID(IDS_OPT_Scientific), STRING_MAX, m_hInst);
    TypeMenu* sMenu = new TypeMenu;
    sMenu->szMenu   = m_oStr->CreateAndCopy(szText);
    sMenu->uiMenu   = 1;
    m_wndMenu.AddItem(sMenu, IDMENU_Scientific, (m_oBtnManger.GetCalcType()==CALC_Scientific?FLAG_Radio:NULL));
    
    m_oStr->StringCopy(szText, ID(IDS_MENU_BaseConv), STRING_MAX, m_hInst);
    sMenu = new TypeMenu;
    sMenu->szMenu   = m_oStr->CreateAndCopy(szText);
    sMenu->uiMenu   = 2;
    m_wndMenu.AddItem(sMenu, IDMENU_BaseConv, (m_oBtnManger.GetCalcType()==CALC_BaseConversions?FLAG_Radio:NULL));
    
    m_oStr->StringCopy(szText, ID(IDS_MENU_Graph), STRING_MAX, m_hInst);
    sMenu = new TypeMenu;
    sMenu->szMenu   = m_oStr->CreateAndCopy(szText);
    sMenu->uiMenu   = 3;
    m_wndMenu.AddItem(sMenu, IDMENU_Graph, (m_oBtnManger.GetCalcType()==CALC_Graphing?FLAG_Radio:NULL));
    
    m_oStr->StringCopy(szText, ID(IDS_MENU_Constants), STRING_MAX, m_hInst);
    sMenu = new TypeMenu;
    sMenu->szMenu   = m_oStr->CreateAndCopy(szText);
    sMenu->uiMenu   = 7;
    m_wndMenu.AddItem(sMenu, IDMENU_Constants, (m_oBtnManger.GetCalcType()==CALC_Constants?FLAG_Radio:NULL));

    int iChecked = 0;
    if(m_oBtnManger.GetCalcType() == CALC_Scientific || m_oBtnManger.GetCalcType() == CALC_SciRPN)
        iChecked = 0;
    else if(m_oBtnManger.GetCalcType() == CALC_BaseConversions)
        iChecked = 1;
    else if(m_oBtnManger.GetCalcType() == CALC_Graphing)
        iChecked = 2;
    else if(m_oBtnManger.GetCalcType() == CALC_Constants)
        iChecked = 3;

    m_wndMenu.SetSelectedItemIndex(iChecked, TRUE);
    m_wndMenu.SetCustomDrawFunc(DrawIconItem, this);
    
    int iHeight = (IsVGA()?2*MENU_Height:MENU_Height);
    m_wndMenu.SetItemHeights(iHeight, iHeight);

    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.right/2;
    rcBtn.top   = rc.bottom - m_oMenu.GetMenuHeight();
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right;

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu.GetMenuHeight();

    m_wndMenu.PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn));

	return TRUE;
}

BOOL CIssCalcInterface::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
    case WM_LEFT_SOFTKEY:
        OnMenuLeft();
        break;
    case WM_RIGHT_SOFTKEY:
        OnMenuRight();
        break;
    case WM_ICON_SOFTKEY:
        OnLastCalc();
        break;
	case WM_CALCENGINE:
        //check for the constants button
        if(wParam == (int)INPUT_CalcState && lParam == (int)CALCSTATE_Constants)
        {
            OnMenuConstants();
        }

		//a bit of a hack but works nicely ...
		//check for the graph button
		if(wParam == 16 && lParam == 0)
		{
			m_oGraphInt.OnGraph();
			return TRUE;
		}

		m_oCalc->CalcButtonPress(wParam, lParam);
		//check for a mode change and invalidate as needed
		if(m_oCalc->GetCalcState() != m_eState)
		{
			ChangeCalcState();
            return TRUE;
		}
		break;
    }

    return UNHANDLED;
}

void CIssCalcInterface::ChangeCalcState()
{
	m_eState = m_oCalc->GetCalcState();

	if(m_bAnimate2ndHyp)
	{
		RECT rcClient;
		GetClientRect(m_hWndDlg, &rcClient);

		CIssGDIEx gdiOld;
		CIssGDIEx gdiNew;
		HDC dc;
		dc = GetDC(m_hWndDlg);
		gdiNew.Create(dc, WIDTH(rcClient), HEIGHT(rcClient), FALSE, TRUE);
		gdiOld.Create(dc, WIDTH(rcClient), HEIGHT(rcClient), TRUE, TRUE);
		
		Draw(gdiNew.GetDC(), m_rcAltButtons);

		SCROLL_Direction eDir = SCROLL_Vertical;
		BOOL bDir = FALSE;

		if(m_eState == CALCSTATE_Normal)
			bDir = TRUE;

		if(GetSystemMetrics(SM_CYSCREEN) < GetSystemMetrics(SM_CXSCREEN))
			eDir = SCROLL_Horizontal;

		ScrollTransition(dc, m_rcAltButtons, gdiOld, m_rcAltButtons, gdiNew, m_rcAltButtons, eDir, bDir);

		ReleaseDC(m_hWndDlg, dc);
	}

	InvalidateRect(m_hWndDlg, &m_rcAltButtons, FALSE);
	InvalidateRect(m_hWndDlg, &m_rcHyp, FALSE);
	InvalidateRect(m_hWndDlg, &m_rc2ndF, FALSE);
}

BOOL CIssCalcInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//these only handle command IDs in a specific range for the menus 
    if(m_oGraphInt.OnCommand(hWnd, wParam, lParam) == TRUE)
        return TRUE;
    if(m_oConstInt.OnCommand(hWnd, wParam, lParam) == TRUE)
        return TRUE;

    if(LOWORD(wParam) >= IDMENU_ConstFav && LOWORD(wParam) < IDMENU_ConstFav+100)
    {
        OnMenuConstantSelect(LOWORD(wParam)-IDMENU_ConstFav);
        return TRUE;
    }

	switch(LOWORD(wParam))
	{
	case IDMENU_MoreApps:
		OnMenuMoreApps();
		break;
	case IDMENU_Options:
		OnMenuOptions();
		break;
	case IDMENU_View:
		OnMenuRight();
		break;
	case IDMENU_File:
		OnMenuLeft();
		break;
	case IDMENU_Exit:
		Destroy();
		PostQuitMessage(0);
		break;
	case IDMENU_About:	
        {
            CDlgSciAbout dlgAbout;
            dlgAbout.Init(&m_oMenu);
            dlgAbout.DoModal(m_hWndDlg, m_hInst, IDD_DLG_BASIC);
            break;
        }
	case IDMENU_Help:	
		LaunchHelp();
		break;
	case IDMENU_Edit:
		break;
	case IDMENU_Copy:
		m_oCalc->AddClipBoard(CLIPBOARD_Copy);
		break;
	case IDMENU_Paste:	
		m_oCalc->AddClipBoard(CLIPBOARD_Paste);
		InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
		break;
	case IDMENU_Standard:
		ChangeCalcModes(CALC_Standard);
		break;
	case IDMENU_Scientific:
		ChangeCalcModes(CALC_Scientific);
		break;
	case IDMENU_BaseConv:
		ChangeCalcModes(CALC_BaseConversions);
		break;
	case IDMENU_Graph:
		ChangeCalcModes(CALC_Graphing);
		break;
    case IDMENU_Constants:
        ChangeCalcModes(CALC_Constants);
        break;
    case IDMENU_ViewTape:
        OnViewTape();
        break;
	case IDMENU_Regional:
		m_bUseRegionalSettings = !m_bUseRegionalSettings;
		InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
		break;
	default:
		return UNHANDLED;
	    break;
	}

	//needed for the constants ... not really worried about performance
	InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);

	return TRUE;
}

void CIssCalcInterface::LaunchHelp()
{
	TCHAR szHelpFile[STRING_MAX];

	m_oStr->Empty(szHelpFile);

	switch(g_cLocale.GetCurrentLanguage())
	{
	case LANG_FRENCH:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_fr.htm"));
		break;
	case LANG_GERMAN:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_de.htm"));
		break;
	case LANG_SPANISH:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_es.htm"));
		break;
	case LANG_JAPANESE:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_jp.htm"));
		break;
	case LANG_DUTCH:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_du.htm"));
		break;
	case LANG_KOREAN:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_ko.htm"));
		break;
	case LANG_ITALIAN:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_it.htm"));
		break;
	case LANG_PORTUGUESE:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_pt.htm"));
		break;
	case LANG_RUSSIAN:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific_ru.htm"));
		break;
	default:
		m_oStr->StringCopy(szHelpFile, _T("PanoScientific.htm"));
		break;
	}

	if(IsSmartphone())
	{
		SHELLEXECUTEINFO shell;
		shell.cbSize		= sizeof(SHELLEXECUTEINFO);
		shell.lpVerb		= _T("OPEN"); 
		shell.fMask			= SEE_MASK_NOCLOSEPROCESS ;
		shell.hwnd			= NULL;
		shell.lpFile		= _T("iexplore.exe");
		shell.lpDirectory	= NULL;
		shell.hInstApp		= m_hInst;
		shell.nShow			= SW_SHOW;

		//TCHAR szTemp[STRING_MAX];
		//GetExeDirectory(szTemp);

		//m_oStr->Concatenate(szTemp, szHelpFile);

		m_oStr->Insert(szHelpFile, _T("\\Windows\\"));
		shell.lpParameters	= szHelpFile; 
		ShellExecuteEx(&shell);
	}
	else
	{
		CreateProcess(_T("peghelp"), szHelpFile, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
	}

}

void CIssCalcInterface::OnMenuMoreApps()
{
	CDlgMoreApps dlgMoreApps;
	dlgMoreApps.Launch(m_hInst, m_hWndDlg);
}

BOOL CIssCalcInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
    BOOL bRet = UNHANDLED;

    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt, hWnd) != UNHANDLED)
        return TRUE;
	//we'll handle out interfaces here
    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
        if(m_oGraphInt.OnLButtonDown(hWnd, pt) == TRUE)
            bRet = TRUE;
        break;
    case CALC_Constants:
        if(m_oConstInt.OnLButtonDown(hWnd, pt) == TRUE)
            bRet = TRUE;
        break;
    default:
        break;
    }

    if(bRet == TRUE)
        goto Error;

	//main buttons
	CCalcDynBtn* oBtn = NULL;
	int i;
	for(i = 0; i < m_arrMainButtons.GetSize(); i++)
	{
		oBtn = NULL;
		oBtn = m_arrMainButtons[i];
		if(oBtn && oBtn->OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);
			bRet = TRUE;
            goto Error;
		}
	}

	for(i = 0; i < m_arrAltButtons.GetSize(); i++)
	{
		CIssVector<ButtonType>* arrBtnType = NULL;
		ButtonType* sBtnType = NULL;

		switch(m_oCalc->GetCalcState())
		{
		case CALCSTATE_2ndF:
			oBtn = m_arr2ndFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndFButtons;
			break;
		case CALCSTATE_Hyp:
			oBtn = m_arrHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sHypButtons;
			break;
		case CALCSTATE_2ndF_Hyp:
			oBtn = m_arr2ndHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons;
			break;
		default:
			oBtn = m_arrAltButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sAltButtons;
			break;
		}

		if(arrBtnType)
			sBtnType = (*arrBtnType)[i];
		if(sBtnType == NULL)
        {
			bRet = FALSE;//not good
            goto Error;
        }

		if(oBtn && oBtn->OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);
			bRet = TRUE;
            goto Error;
		}
	}

#ifdef UNDER_CE
    SHRGINFO    shrg;

    shrg.cbSize = sizeof(shrg);
    shrg.hwndClient = hWnd;
    shrg.ptDown.x = pt.x;
    shrg.ptDown.y = pt.y;
    shrg.dwFlags = SHRG_RETURNCMD|SHRG_LONGDELAY;

    if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU) 
    {
        OnMenuContext(pt);
    }
#endif

Error:
    if(bRet == TRUE)//and play the sound
        PlaySounds();

	return bRet;
}

BOOL CIssCalcInterface::OnMenuContext(POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];

    m_oStr->StringCopy(szText, ID(IDS_MENU_Copy), STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_Copy, szText);
    m_oStr->StringCopy(szText, ID(IDS_MENU_Paste), STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_Paste, szText);

    ClientToScreen(m_hWndDlg, &pt);

    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);
    DestroyMenu(hMenu);

    return TRUE;
}

BOOL CIssCalcInterface::HandleDisplayTaps(POINT& pt)
{
	//if we're not base 10 we could change the bits
	if(m_oCalc->GetCalcBase() != 10)
	{
		if(PtInRect(&m_rcDisplay, pt))
		{	//change the base bits
			switch(m_oCalc->GetBaseBits())
			{
			case 8:
				m_oCalc->SetBaseBits(16);
				break;
			case 16:
				m_oCalc->SetBaseBits(32);
				break;
			case 32:
				m_oCalc->SetBaseBits(48);
				break;
       //     case 48:
       //         m_oCalc->SetBaseBits(64);
       //         break;
			default:
				m_oCalc->SetBaseBits(8);
				break;
			}
			InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
			return TRUE;
		}
		else if(PtInRect(&m_rcBase, pt))
		{
			switch(m_oCalc->GetCalcBase())
			{
			case 2:
				m_oCalc->SetCalcBase(8);
				break;
			case 8:
			default:
				m_oCalc->SetCalcBase(10);
				break;
			case 16:
			    m_oCalc->SetCalcBase(2);
				break;
			}
			InvalidateRect(m_hWndDlg, &m_rcBase, FALSE);
			return TRUE;
		}
       	return FALSE;
	}

	//otherwise we need to either
	//change the DRG
	if(PtInRect(&m_rcDisplay, pt))
	{
		//or change the display
		DisplayType eDisplay = m_oCalc->GetCalcDisplay();

		eDisplay = DisplayType(1 +(int)eDisplay);
#ifdef DEBUG
        if((int)eDisplay >= (int)DISPLAY_Count)
            eDisplay = DISPLAY_Float;
#else
		if((int)eDisplay >= (int)DISPLAY_DMS)
			eDisplay = DISPLAY_Float;
#endif
		m_oCalc->SetCalcDisplay(eDisplay);
	}
	else if(PtInRect(&m_rcDRG, pt))
	{
		m_oCalc->CalcButtonPress(INPUT_Function, FUNCTION_DRG);
	}
	else if(PtInRect(&m_rcBase, pt) && m_oBtnManger.GetCalcType() == CALC_BaseConversions)
		m_oCalc->SetCalcBase(16);
    else if(PtInRect(&m_rcM1, pt))
    {
        TCHAR szBody[STRING_MAX*NUMBER_OF_MemoryBanks];
        TCHAR szMemory[STRING_NORMAL];

        m_oStr->Empty(szBody);
        if(m_oBtnManger.GetUseAdvMem())
        {
            TCHAR szTemp[STRING_MAX];

            m_oStr->Empty(szTemp);

            for(int i = 0; i < NUMBER_OF_MemoryBanks; i++)
            {
                m_oCalc->GetMemory(szMemory, i);
                m_oStr->Format(szTemp,_T("Memory%i: %s\r\n"), i, szMemory);
                m_oStr->Concatenate(szBody, szTemp);
            }
            MessageBox(m_hWndDlg, szBody, _T("Memory"), MB_OK);
        }
        else if(m_oCalc->IsMemoryFilled(0))
        {
            m_oCalc->GetMemory(szMemory, 0);
            m_oStr->Format(szBody,_T("Memory: %s"), szMemory);
            MessageBox(m_hWndDlg, szBody, _T("Memory"), MB_OK);
        }

        
    }
    //if we're in RPN mode and the stack isn't huge we'll show it too
    else if(m_oCalc->GetCalcMode() == CALC_RPN && m_oCalc->GetRPNStackSize() < 11)
    {
        OnMenuRPNStack();
    }

	InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);

	return TRUE;
}


BOOL CIssCalcInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt, hWnd) != UNHANDLED)
        return TRUE;
	//we'll handle out interfaces here
    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
        if(m_oGraphInt.OnLButtonUp(hWnd, pt) == TRUE)
            return TRUE;
        break;
    case CALC_Constants:
        if(m_oConstInt.OnLButtonUp(hWnd, pt) == TRUE)
            return TRUE;
    default:
        break;
    }

	//this probably helps performance 
	if(PtInRect(&m_rcDisplayArea, pt))
		return HandleDisplayTaps(pt);

	CCalcDynBtn* oBtn = NULL;
	int i;
	for(i = 0; i < m_arrMainButtons.GetSize(); i++)
	{
		oBtn = NULL;
		oBtn = m_arrMainButtons[i];
		if(oBtn && oBtn->OnLButtonUp(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);
			InvalidateRect(hWnd, &m_rcDisplayArea, FALSE);
			PostMessage(hWnd, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->sMainButtons[i]->iButtonType, 
					 m_oBtnManger.GetCurrentLayout()->sMainButtons[i]->iButtonValue);
			return TRUE;
		}
	}

	for(i = 0; i < m_arrAltButtons.GetSize(); i++)
	{
		CIssVector<ButtonType>* arrBtnType = NULL;
		ButtonType* sBtnType = NULL;

		switch(m_oCalc->GetCalcState())
		{
		case CALCSTATE_2ndF:
			oBtn = m_arr2ndFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndFButtons;
			break;
		case CALCSTATE_Hyp:
			oBtn = m_arrHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sHypButtons;
			break;
		case CALCSTATE_2ndF_Hyp:
			oBtn = m_arr2ndHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons;
			break;
		default:
			oBtn = m_arrAltButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sAltButtons;
			break;
		}
		
		if(arrBtnType)
			sBtnType = (*arrBtnType)[i];
		if(sBtnType == NULL)
			return FALSE;//not good
		
		if(oBtn && oBtn->OnLButtonUp(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);
			InvalidateRect(hWnd, &m_rcDisplayArea, FALSE);
			PostMessage(hWnd, WM_CALCENGINE, sBtnType->iButtonType, 
				sBtnType->iButtonValue);
			return TRUE;
		}
	}

	//alt buttons
	return UNHANDLED;
}

BOOL CIssCalcInterface::OnMouseMove(HWND hWnd, POINT& pt)
{
	CCalcDynBtn* oBtn = NULL;
	int i;
	for(i = 0; i < m_arrMainButtons.GetSize(); i++)
	{
		oBtn = NULL;
		switch(m_oCalc->GetCalcState())
		{
		case CALCSTATE_2ndF:
			oBtn = m_arr2ndFButtons[i];
			break;
		case CALCSTATE_Hyp:
			oBtn = m_arrHypFButtons[i];
			break;
		case CALCSTATE_2ndF_Hyp:
			oBtn = m_arr2ndHypFButtons[i];
			break;
		default:
			oBtn = m_arrAltButtons[i];
			break;
		}
		if(oBtn && oBtn->OnMouseMove(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);		
			return TRUE;
		}
	}

	for(i = 0; i < m_arrAltButtons.GetSize(); i++)
	{
		CIssVector<ButtonType>* arrBtnType = NULL;
		ButtonType* sBtnType = NULL;

		switch(m_oCalc->GetCalcState())
		{
		case CALCSTATE_2ndF:
			oBtn = m_arr2ndFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndFButtons;
			break;
		case CALCSTATE_Hyp:
			oBtn = m_arrHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sHypButtons;
			break;
		case CALCSTATE_2ndF_Hyp:
			oBtn = m_arr2ndHypFButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons;
			break;
		default:
			oBtn = m_arrAltButtons[i];
			arrBtnType = &m_oBtnManger.GetCurrentLayout()->sAltButtons;
			break;
		}

		if(arrBtnType)
			sBtnType = (*arrBtnType)[i];
		if(sBtnType == NULL)
			return FALSE;//not good

		if(oBtn && oBtn->OnMouseMove(pt))
		{
			InvalidateRect(hWnd, &oBtn->GetButtonRect(), FALSE);
			return TRUE;
		}
	}

	//alt buttons
	return UNHANDLED;
}

void CIssCalcInterface::ChangeCalcModes(EnumCalcType eCalcType)
{
    m_eState = CALCSTATE_Normal;
    m_oCalc->AddStates(CALCSTATE_Normal);

	// ANI CODE
	/*HDC hDC = GetDC(m_hWndDlg);
	RECT rc;

	GetClientRect(m_hWndDlg, &rc);

	CIssGDI gdiPrev;
	CIssGDI gdiCur;

	gdiPrev.Create(hDC, rc, TRUE, TRUE);
	gdiCur.Create(hDC, rc, FALSE, TRUE);*/

	//hack some RPN action
	if(eCalcType == CALC_Scientific && m_eCalcMode == CALC_RPN)
		eCalcType = CALC_SciRPN;
	else if(eCalcType == CALC_SciRPN && m_eCalcMode != CALC_RPN)
		eCalcType = CALC_Scientific;

	if(eCalcType == m_oBtnManger.GetCalcType())
		return;

    NotifyLostFocus();//clean up, hide children, etc

	if(S_OK != m_oBtnManger.SetCalcType(eCalcType, m_hInst))
		return;

    //expression, alg, etc .. gotta ignore options for some calcs
    UpdateCalcMode(m_eCalcMode);

	//and delete the fonts
	CIssGDIEx::DeleteFont(m_hFontMainBtnText);
	CIssGDIEx::DeleteFont(m_hFontMainLongText);
	CIssGDIEx::DeleteFont(m_hFontAltBtnText);
	CIssGDIEx::DeleteFont(m_hFontAltLongText);
    CIssGDIEx::DeleteFont(m_hFontAltXLText);

	m_iSelector = 0;//reset the position

	// ANI CODE
	/*Draw(gdiCur.GetDC(), rc);
	//CIssGDIEffects::ScrollTransition(hDC, rc, gdiPrev, rc, gdiCur, rc, SCROLL_Vertical, TRUE, 500);
	CIssGDIEffects::AlphaTransition(hDC, rc, gdiPrev, rc, gdiCur, rc);
	ReleaseDC(m_hWndDlg, hDC);*/

    CCalcInterace* oInt = NULL;

	switch(eCalcType)
	{
	case CALC_Standard:
    case CALC_Scientific:
    case CALC_Financial:
    case CALC_Statistics:
	default://we'll make the scientific setting the default
		m_oCalc->SetCalcBase(10);
	case CALC_BaseConversions:
        InitAltButtons();
        InitMainButtons();
		break;
    case CALC_Constants:
        m_oCalc->SetCalcBase(10);//graphing must use base 10 ... we're not putting this stuff back
        InitAltButtons();//no buttons but we need to clear it        
        InitMainButtons();
        oInt = &m_oConstInt;
        break;
	case CALC_Graphing:
		m_oCalc->SetCalcBase(10);//graphing must use base 10 ... we're not putting this stuff back
		m_oCalc->AddClear(CLEAR_Clear);
        InitAltButtons();
        InitMainButtons();
        oInt = &m_oGraphInt; //for the extra displays
		break;
    }

    //for the extra groups
    if(oInt != NULL)
    {
        TypeCalcInit sInit;
        sInit.oBtnHand  = &m_oBtnHandler;
        sInit.oBtnMan   = &m_oBtnManger;
        sInit.oCalc     = m_oCalc;
        sInit.hWnd      = m_hWndDlg;
        sInit.hInst     = m_hInst;
        sInit.wndMenu   = &m_wndMenu;
        sInit.oMenu     = &m_oMenu;
        sInit.oConstants= &m_oConstants;

        oInt->Init(&sInit);
    }

    UpdateWindowText();

	//this is a complete redraw sort of state
	InvalidateRect(m_hWndDlg, NULL, FALSE);
}

void CIssCalcInterface::UpdateWindowText()
{

    /*
    //For Mobile2Market cert
    SetWindowText(m_hWndDlg, _T("Calc Pro"));
    return;
    */

    TCHAR szWindowText[STRING_LARGE];

    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Standard:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Standard), STRING_LARGE, m_hInst);
        break;
    case CALC_Scientific:
    case CALC_SciRPN:
        m_oStr->StringCopy(szWindowText, ID(IDS_OPT_Scientific), STRING_LARGE, m_hInst);
        break;
    case CALC_Financial:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Financial), STRING_LARGE, m_hInst);
        break;
    case CALC_Statistics:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Stats), STRING_LARGE, m_hInst);
        break;
    case CALC_BaseConversions:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_BaseConv), STRING_LARGE, m_hInst);
        break;
     case CALC_Constants:
         m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Constants), STRING_LARGE, m_hInst);
         break;
    case CALC_Graphing:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Graph), STRING_LARGE, m_hInst);
        break;
    case CALC_Currency:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Currency), STRING_LARGE, m_hInst);
        break;
    case CALC_UnitConv:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_Unit), STRING_LARGE, m_hInst);
        break;
    case CALC_DateTime:
        m_oStr->StringCopy(szWindowText, ID(IDS_MENU_DateTime), STRING_LARGE, m_hInst);
        break;
    default:
        m_oStr->Empty(szWindowText);
        break;
    }

    m_oStr->Insert(szWindowText, _T("Scientific - "));

    SetWindowText(m_hWndDlg, szWindowText);
}


CCalcDynBtn* CIssCalcInterface::GetButton(int iWidth, int iHeight, int iButtonStyle)
{
	int iSize;
	if((iSize = m_arrButtons.GetSize()) == 0)
		return NULL;

	TypeButtonBuffer* oBtnBuf = NULL;

	for(int i = 0; i < iSize; i++)
	{
		oBtnBuf = m_arrButtons[i];
		if(oBtnBuf && 
			oBtnBuf->btWidth == iWidth  &&
			oBtnBuf->btHeight == iHeight &&
			oBtnBuf->btColorIndex == iButtonStyle)
			return oBtnBuf->oButton; //we found a match!
	}
	return NULL;
}

BOOL CIssCalcInterface::SaveButton(CCalcDynBtn* oBtn, int iWidth, int iHeight, int iButtonStyle)
{
	if(oBtn == NULL)
		return FALSE;

	//make sure this one isn't already saved
	if(GetButton(iWidth, iHeight, iButtonStyle))
		return FALSE;

	//otherwise add it
	TypeButtonBuffer* oBtnBuf = new TypeButtonBuffer;
	oBtnBuf->btHeight = iHeight;
	oBtnBuf->btWidth  = iWidth;
	oBtnBuf->btColorIndex = iButtonStyle;
	oBtnBuf->oButton = oBtn;

	m_arrButtons.AddElement(oBtnBuf);
	return TRUE;
}

/*
a	A (HEX)
b	B (HEX)
c	C (HEX)
d	D (HEX)
e	E (HEX)
f	F (HEX)
g	2ndF	(check calc mode)
h	Hyp		(check calc mode)
i	inverse (1/x)
j	***reserved
k	***reserved
l	log (log base 10)
m	DRG (mode)
n	ln (natural log)
o	***reserved
p	plus/minus
q	squared
r	square root
s	sin
t	tan
u	cos
v	MS
w	MR
x	MC
y	M+
z	M-
*/

//Here we'll handle all of the incoming button presses
BOOL CIssCalcInterface::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oBtnHandler.OnChar(hWnd, wParam, lParam, m_oCalc, &m_oBtnManger))
	{
        InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);        
		return TRUE;
	}
	return FALSE;
	
}

BOOL CIssCalcInterface::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(HandleDPad(wParam))
		return TRUE;//if we handle here we don't pass it on ... D-pad of course on SP


	if(m_oBtnHandler.OnKeyDown(hWnd, wParam, lParam, m_oCalc, &m_oBtnManger))
	{
		InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CIssCalcInterface::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_bBackPressed)
	{
		::KillTimer(hWnd, TM_Back_Key);
		m_bBackPressed = FALSE;

		if(m_bHoldBack)
			return TRUE;
		
		m_oCalc->AddClear(CLEAR_BackSpace);
		::InvalidateRect(hWnd, NULL, FALSE);

		return TRUE;
	}

    BOOL bHandled = TRUE;
        
    switch(wParam)
    {
    case VK_LEFT:
        bHandled = NextCalc(FALSE);
    	break;
    case VK_RIGHT:
        bHandled = NextCalc(TRUE);
    	break;
    case VK_UP:
        bHandled = m_oCalc->OnScrollUp();
        if(bHandled)
            InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
        break;
    case VK_DOWN:
        bHandled = m_oCalc->OnScrollDown();
        if(bHandled)
            InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
        break;
    case VK_TSOFT1:
        bHandled = OnMenuLeft();
    	break;
    case VK_TSOFT2:
        bHandled = OnMenuRight();
    	break;
    default:
        bHandled = FALSE;
        break;
    }

	if(bHandled == FALSE && m_oBtnHandler.OnKeyUp(hWnd, wParam, lParam, m_oCalc, &m_oBtnManger))
	{
		InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
		return TRUE;
	}
	return TRUE;
}

BOOL CIssCalcInterface::HandleDPad(int iVKKey)
{
#ifdef WIN32_PLATFORM_WFSP //only useful for smartphone at the moment
	int iOldSel= m_iSelector; 
    if(iOldSel == -1)
        iOldSel = 0;
	int iKey = iVKKey;

    switch(m_oBtnManger.GetCalcType())
    {
    case CALC_Graphing:
       {//if we're at top then move up to graph
            LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();	
            if(m_iSelector < sLayout->iAltBtnColumns)
            {
                if(m_oGraphInt.HandleDPad(iVKKey))
                {
                    InvalidateRect(m_hWndDlg, NULL, FALSE);
                    if(m_oGraphInt.IsSelected())
                        m_iSelector = -1;
                    else
                        m_iSelector = sLayout->iAltBtnColumns - 1;
                    return TRUE;
                }
            }
       }
        break;
    case CALC_Constants:
        return  m_oConstInt.HandleDPad(iVKKey);
        break;
    default:
        break;
    }

	BOOL bWide = FALSE;
	if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN) && GetDeviceType() != DEVTYPE_SPLand)
		bWide = TRUE;

	if(bWide)
	{
		switch(iVKKey)
		{
		case VK_LEFT:
			iKey = VK_DOWN;
			break;
		case VK_RIGHT:
			iKey = VK_UP;
			break;
		case VK_DOWN:
			iKey = VK_LEFT;
		    break;
		case VK_UP:
			iKey = VK_RIGHT;
		    break;
		}
	}
	
	switch(iKey)
	{
	case VK_UP:
		{
			LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();	
			m_iSelector -= sLayout->iAltBtnColumns;
			if(m_iSelector < 0)//get it back into range
			{
				while(m_iSelector < sLayout->iAltBtnColumns*sLayout->iAltBtnRows)
					m_iSelector += sLayout->iAltBtnColumns;
				m_iSelector -= sLayout->iAltBtnColumns;
			}
			InvalidateRect(m_hWndDlg, &m_arrAltButtons[iOldSel]->GetButtonRect(), FALSE);
			InvalidateRect(m_hWndDlg, &m_arrAltButtons[m_iSelector]->GetButtonRect(), FALSE);
			break;
		}
	case VK_DOWN:
		{
			LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();	
			m_iSelector += sLayout->iAltBtnColumns;
			if(m_iSelector >= sLayout->iAltBtnColumns*sLayout->iAltBtnRows)//get it back into range
			{
				while(m_iSelector >= 0)
					m_iSelector -= sLayout->iAltBtnColumns;
				m_iSelector += sLayout->iAltBtnColumns;
			}
			InvalidateRect(m_hWndDlg, &m_arrAltButtons[iOldSel]->GetButtonRect(), FALSE);
			InvalidateRect(m_hWndDlg, &m_arrAltButtons[m_iSelector]->GetButtonRect(), FALSE);
			break;
		}
	case VK_RIGHT:
		//need to check the layouts
		{
			LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();	
			int iTemp = m_iSelector/sLayout->iAltBtnColumns;
			m_iSelector++;
			if(m_iSelector/sLayout->iAltBtnColumns != iTemp)
				m_iSelector -= sLayout->iAltBtnColumns;

		}
		InvalidateRect(m_hWndDlg, &m_arrAltButtons[iOldSel]->GetButtonRect(), FALSE);
		InvalidateRect(m_hWndDlg, &m_arrAltButtons[m_iSelector]->GetButtonRect(), FALSE);
		break;
	case VK_LEFT:
		{
			LayoutCalcType* sLayout = m_oBtnManger.GetCurrentLayout();	
			int iTemp = m_iSelector/sLayout->iAltBtnColumns;
			m_iSelector--;
			if(m_iSelector/sLayout->iAltBtnColumns != iTemp || m_iSelector < 0)
				m_iSelector += sLayout->iAltBtnColumns;
		}
		InvalidateRect(m_hWndDlg, &m_arrAltButtons[iOldSel]->GetButtonRect(), FALSE);
		InvalidateRect(m_hWndDlg, &m_arrAltButtons[m_iSelector]->GetButtonRect(), FALSE);
		break;
	case VK_RETURN:
        switch(m_oCalc->GetCalcState())
        {
        case CALCSTATE_2ndF:
            PostMessage(m_hWndDlg, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->s2ndFButtons[m_iSelector]->iButtonType, 
                m_oBtnManger.GetCurrentLayout()->s2ndFButtons[m_iSelector]->iButtonValue);
        	break;
        case CALCSTATE_Hyp:
            PostMessage(m_hWndDlg, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->sHypButtons[m_iSelector]->iButtonType, 
                m_oBtnManger.GetCurrentLayout()->sHypButtons[m_iSelector]->iButtonValue);
            break;
        case CALCSTATE_2ndF_Hyp:
            PostMessage(m_hWndDlg, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons[m_iSelector]->iButtonType, 
                m_oBtnManger.GetCurrentLayout()->s2ndHypFButtons[m_iSelector]->iButtonValue);
            break;
        case CALCSTATE_Normal:
        default:        
            PostMessage(m_hWndDlg, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->sAltButtons[m_iSelector]->iButtonType, 
                m_oBtnManger.GetCurrentLayout()->sAltButtons[m_iSelector]->iButtonValue);
            break;
        }

    	InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
#endif
	return FALSE;
}

BOOL CIssCalcInterface::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// if they were holding onto the back key
	if(wParam == TM_Back_Key)
	{
		m_bHoldBack = TRUE;
		::KillTimer(m_hWndDlg, TM_Back_Key);

		m_oCalc->AddClear(CLEAR_Clear);
		::InvalidateRect(m_hWndDlg, &m_rcDisplayArea, FALSE);
	
		return TRUE;
	}
    
    if(m_oBtnManger.GetCalcType() == CALC_Constants)
        return m_oConstInt.OnTimer(hWnd, wParam, lParam);
    
	return UNHANDLED;
}

BOOL CIssCalcInterface::OnBackKey()
{
	//backspace key has been pressed
	if(m_bBackPressed)
		return FALSE;
	m_bBackPressed = TRUE;
	m_bHoldBack	   = FALSE;	// we're going to test to see if we're gonna hold onto it
	::KillTimer(m_hWndDlg, TM_Back_Key);
	::SetTimer(m_hWndDlg, TM_Back_Key, 300, NULL);
	return TRUE;
}

void CIssCalcInterface::DetectAndSetDeviceType()
{
#ifdef WIN32_PLATFORM_WFSP
	if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
		m_oBtnHandler.SetDeviceType(DEVTYPE_SP);
	else
		m_oBtnHandler.SetDeviceType(DEVTYPE_SPLand);
#elif WIN32_PLATFORM_PSPC
	m_oBtnHandler.SetDeviceType(DEVTYPE_PPC);
#endif
}


BOOL CIssCalcInterface::InitMenu()
{
    if(m_wndMenu.GetBackground() == IDR_PNG_MenuBack &&
       m_wndMenu.GetSelected() == IDR_PNG_MenuSelect &&
       m_wndMenu.GetImageArray() == (IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray))
       return TRUE;

    m_wndMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_wndMenu.SetBackground(IDR_PNG_MenuBack);
    m_wndMenu.SetSelected(IDR_PNG_MenuSelect);
    m_wndMenu.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_wndMenu.PreloadImages(m_hWndDlg, m_hInst);

    // load the icons for the menu here too
    m_gdiIcons.LoadImage((IsVGA()?IDR_PNG_MenuIconsVGA:IDR_PNG_MenuIcons), m_hWndDlg, m_hInst, TRUE);

    CIssGDIEx::DeleteFont(m_hFontMenu);
    int iHeight = MENU_Height*GetSystemMetrics(SM_CXICON)/96;

//    int iHeight = (IsVGA()?2*MENU_Height:MENU_Height);
//    iHeight /= 3;

    m_hFontMenu = CIssGDIEx::CreateFont(iHeight, FW_BOLD, TRUE);

    return TRUE;
}


BOOL CIssCalcInterface::OnMenuOptions()
{
	CDlgSciOptions dlgOptions;

	TypeOptions sOptions;

    sOptions.bUseRegionalSettings   = m_bUseRegionalSettings;
    sOptions.bPlaySounds            = m_bPlaySounds;
    sOptions.bRecordTape            = m_oCalc->GetUseTape();
    sOptions.bAdvancedMemory        = m_oBtnManger.GetUseAdvMem();
    sOptions.eCalculatorMode        = m_eCalcMode;
    sOptions.eNotation              = m_oCalc->GetCalcDisplay();
    sOptions.iConversionBuffer      = m_oCalc->GetBaseBits();
    sOptions.iRPNStackSize          = m_oCalc->GetRPNStackSize();
    sOptions.eSkin                  = m_oBtnManger.GetSkinType();
    sOptions.iFixedDigits           = m_oCalc->GetFixedDecDigits();
    //sOptions.eSize					= g_eCalcSize;
    sOptions.eKB                    = m_oBtnManger.GetKBType();
    sOptions.iKBOffset              = m_oBtnManger.GetKBOffset();
    sOptions.bKBLetters             = m_oBtnManger.GetKBLetters();
    sOptions.bFullScreen            = m_oBtnManger.GetFullscreen();

    //these should probably be global ... I guess ...
    sOptions.bGraphFast             = g_bGraphFast;
    sOptions.bGraphUsePoints        = g_bGraphPoints;

	//set variables
	dlgOptions.Init(sOptions, &m_wndMenu, &m_oMenu, m_hWndDlg);

	if(IDOK == dlgOptions.DoModal(m_hWndDlg, m_hInst, IDD_DLG_BASIC))
    {
        sOptions = dlgOptions.GetOptions();

        m_bUseRegionalSettings      = sOptions.bUseRegionalSettings;
        m_bPlaySounds               = sOptions.bPlaySounds;
        m_eCalcMode                 = sOptions.eCalculatorMode;

        g_bGraphFast                  = sOptions.bGraphFast;
        g_bGraphPoints                = sOptions.bGraphUsePoints;
        //gbTVMEndPay                  = sOptions.bTVMEndPeriod;


        m_oCalc->SetUseTape(sOptions.bRecordTape);
        m_oCalc->SetCalcDisplay(sOptions.eNotation);
        m_oCalc->SetBaseBits(sOptions.iConversionBuffer);
        m_oCalc->SetRPNStackSize(sOptions.iRPNStackSize);
        m_oCalc->SetFixedDecDigits(sOptions.iFixedDigits);

       
        UpdateCalcMode(m_eCalcMode);

        ChangeCalcModes(m_oBtnManger.GetCalcType());//RPN hack

        BOOL bUseRPN = FALSE;

        //this is here so we can manually swap the RPN buttons 
        if(m_eCalcMode == CALC_RPN)
            bUseRPN = TRUE;
	    
        //check if the skin has changed or we've change memory modes
        if(sOptions.eSkin != m_oBtnManger.GetSkinType() || 
            m_oBtnManger.GetUseAdvMem() != sOptions.bAdvancedMemory || 
            m_oBtnManger.GetUseRPN() != bUseRPN ||
            m_oBtnManger.GetKBType() != sOptions.eKB ||
            m_oBtnManger.GetKBOffset() != sOptions.iKBOffset ||
            m_oBtnManger.GetKBLetters() != sOptions.bKBLetters ||
            m_oBtnManger.GetFullscreen() != sOptions.bFullScreen)
        {
            //set the memory mode
            m_oBtnManger.SetUseAdvMem(sOptions.bAdvancedMemory);
            m_oBtnManger.SetUseRPN(bUseRPN);
            m_oBtnManger.SetKBType(sOptions.eKB);
            m_oBtnManger.SetKBOffset(sOptions.iKBOffset);
            m_oBtnManger.SetKBLetters(sOptions.bKBLetters);
            m_oBtnManger.SetFullscreen(sOptions.bFullScreen);

            m_oMenu.Init(m_hWndDlg, m_hInst, sOptions.eSkin);

		    HDC			hDC = GetDC(m_hWndDlg);
		    CIssGDIEx	gdiPrev;
		    RECT		rc;
		    GetClientRect(m_hWndDlg, &rc);

		    gdiPrev.Create(hDC, rc, FALSE, TRUE);
		    Draw(gdiPrev.GetDC(), rc);

		    CIssGDIEx	gdiCur;
    		
		    gdiCur.Create(hDC, rc, FALSE, TRUE);

            //always recreate the buttons
		    m_oBtnManger.SetSkinType(sOptions.eSkin, m_hInst, TRUE);
            		
		    InitDisplay();
		    InitAltButtons();
		    InitMainButtons();
            InitMenu();

            //don't forget the extras
            m_oGraphInt.Init(NULL);
            m_oConstInt.Init(NULL);
            
		    Draw(gdiCur.GetDC(), rc);//draw the new skin to our gdi

		    AlphaTransition(hDC, rc, gdiPrev, rc, gdiCur, rc);

		    ReleaseDC(m_hWndDlg, hDC);
	    }
	    //save the registry here
	    SaveRegistry();
    }

	return TRUE;
}

void CIssCalcInterface::PlaySounds()
{
	if(m_bPlaySounds == 1)
        PlaySound(_T("IDR_WAVE_TICK"), m_hInst,  SND_RESOURCE | SND_SYNC);
    else if(m_bPlaySounds == 2)
        PlaySound(_T("IDR_WAVE_TICKSOFT"), m_hInst,  SND_RESOURCE | SND_SYNC);
}

void CIssCalcInterface::NotifyLostFocus()
{

}

//just pass in the current mode if you don't want to change it
void CIssCalcInterface::UpdateCalcMode(CalcOperationType eMode)
{
    m_eCalcMode = eMode; 

    switch(m_oBtnManger.GetCalcType())
    {
    default:
    case CALC_Standard:
    case CALC_Scientific:
    case CALC_BaseConversions:
    case CALC_Statistics:
    case CALC_Financial:
    case CALC_Constants:
    case CALC_DateTime:
        m_oCalc->SetCalcMode(m_eCalcMode);
    	break;
    case CALC_Graphing:
        m_oCalc->SetCalcMode(CALC_String);
        break;
    case CALC_UnitConv:
    case CALC_Currency:
        if(m_eCalcMode == CALC_String)
            m_oCalc->SetCalcMode(CALC_OrderOfOpps);
        else
            m_oCalc->SetCalcMode(m_eCalcMode);
        break;
    }
}

BOOL CIssCalcInterface::OnLastCalc()
{
    //animate here??
    ChangeCalcModes(m_oBtnManger.GetPrevCalc());
    return TRUE;
}

BOOL CIssCalcInterface::NextCalc(BOOL bForward)
{
#ifdef WIN32_PLATFORM_WFSP
    return FALSE;
 #endif // _DEBUG

    EnumCalcType eNextCalc;

    if(bForward) 
    {
        eNextCalc = (EnumCalcType)(m_oBtnManger.GetCalcType()+1);
        
        //now some special cases
        if(m_oBtnManger.GetCalcType() == CALC_Constants)
            eNextCalc = CALC_Scientific;
        else if(eNextCalc > CALC_Graphing)
            eNextCalc = CALC_Constants;
        else if(eNextCalc == CALC_SciRPN)
            eNextCalc = CALC_BaseConversions;

        if(eNextCalc == CALC_Scientific &&
            m_oCalc->GetCalcMode() == CALC_RPN)
            eNextCalc = CALC_SciRPN;
    }
    else
    {   //back
        eNextCalc = (EnumCalcType)(m_oBtnManger.GetCalcType()-1);

        if(eNextCalc < 1)
            eNextCalc = CALC_Constants;//just easier
        else if(eNextCalc > CALC_Graphing)
            eNextCalc = CALC_Graphing;
        else if(eNextCalc == CALC_SciRPN && m_oCalc->GetCalcMode() != CALC_RPN)
            eNextCalc = CALC_Scientific;
        else if(eNextCalc == CALC_Scientific) //MUST be RPN so go one more
            eNextCalc = CALC_Constants;
    }

    //ok good fun ... next calc has been figured out 
    CIssGDIEx gdiNext;
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);
    HDC dc = GetDC(m_hWndDlg);

    //set the next calc
    ChangeCalcModes(eNextCalc);

    //create the new buffer and draw it
    gdiNext.Create(dc, rcClient, FALSE, TRUE);
    Draw(gdiNext.GetDC(), rcClient);

    //animation time

    if(bForward)
        SweepLeft(dc, gdiNext, 500);
    else
        SweepRight(dc, gdiNext, 500);

    ReleaseDC(m_hWndDlg, dc);
    return TRUE;
}

void CIssCalcInterface::OnViewTape()
{
    SHELLEXECUTEINFO sei = {0};
    sei.cbSize	= sizeof(sei);
    sei.hwnd	= m_hWndDlg;
    sei.nShow	= SW_SHOWNORMAL;

    TCHAR szFullPath[STRING_MAX]= _T("");

    // just in case
    if(!m_oCalc->GetUseTape())
        return;

    m_oCalc->GetFullTapePath(szFullPath);
    
    sei.lpFile	= szFullPath;
    ShellExecuteEx(&sei);
}

void CIssCalcInterface::DrawRPNItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CIssCalcInterface* pThis = (CIssCalcInterface*)lpClass;
    if(pThis)
        pThis->DrawRPNItem(gdi, rcDraw, sItem, bIsHighlighted);
}


void CIssCalcInterface::DrawRPNItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    RECT rc = rcDraw;

    int* iIndex = (int*)sItem->lpItem;
    if(!iIndex)
        return;

    TCHAR szTemp[STRING_NORMAL];

    if(*iIndex == m_oCalc->GetRPNStackSize() - 1)
        m_oCalc->GetAnswer(szTemp);
    else if(!m_oCalc->GetRPNStackEntry(m_oCalc->GetRPNStackSize() - 2 - *iIndex, szTemp))
        return;

    rc.right -= GetSystemMetrics(SM_CXICON)/4;
    rc.left += GetSystemMetrics(SM_CXICON)/8;

    if(*iIndex == m_oCalc->GetRPNStackSize() - 1)
    {
        DrawText(gdi, _T("X:"), rc, DT_LEFT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    }
    else if(*iIndex == m_oCalc->GetRPNStackSize() - 2)
    {
        DrawText(gdi, _T("Y:"), rc, DT_LEFT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    }
    else if(*iIndex == m_oCalc->GetRPNStackSize() - 3)
    {
        DrawText(gdi, _T("Z:"), rc, DT_LEFT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    }
    else if(*iIndex == m_oCalc->GetRPNStackSize() - 4)
    {
        DrawText(gdi, _T("T:"), rc, DT_LEFT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    }
    else
    {
        TCHAR szLabel[STRING_NORMAL];
        m_oStr->Format(szLabel, _T("%i:"), m_oCalc->GetRPNStackSize() - *iIndex);
        DrawText(gdi, szLabel, rc, DT_LEFT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    }

    DrawText(gdi, szTemp, rc, DT_RIGHT|DT_VCENTER, m_hFontRPN, TEXTCOLOR_Normal);
    
}


void CIssCalcInterface::DrawConstantsItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CIssCalcInterface* pThis = (CIssCalcInterface*)lpClass;
    if(pThis)
        pThis->DrawConstantsItem(gdi, rcDraw, sItem, bIsHighlighted);
}

void CIssCalcInterface::DrawConstantsItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    RECT rc = rcDraw;
    rc.top      += HEIGHT(rcDraw)*1/9;
    if(bIsHighlighted)
        rc.bottom   = rc.top + HEIGHT(rcDraw)*1/3;
    else
        rc.bottom   = rc.top + HEIGHT(rcDraw)*4/9;

    int* iIndex = (int*)sItem->lpItem;
    if(!iIndex)
        return;

    TCHAR* szName = m_oConstants.GetFavoriteName(*iIndex);
    TCHAR* szGroup= m_oConstants.GetFavoriteGroup(*iIndex);
    if(!szName || !szGroup)
        return;

    rc.left += GetSystemMetrics(SM_CXICON)/8;

    DrawText(gdi, szGroup, rc, DT_LEFT|DT_TOP|DT_END_ELLIPSIS, m_hFontSmallTop, TEXTCOLOR_Selected);

    rc.top      = rc.bottom;
    rc.bottom   = rcDraw.bottom - HEIGHT(rcDraw)*1/9;

    if(bIsHighlighted)
    {
        rc.top = rcDraw.top + HEIGHT(rcDraw)*1/9;     
        DrawText(gdi, szName, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontDisplayTop, TEXTCOLOR_Normal);
    }
    else
        DrawText(gdi, szName, rc, DT_LEFT|DT_TOP|DT_END_ELLIPSIS, m_hFontDisplayTop, TEXTCOLOR_Normal);

    if(bIsHighlighted == FALSE)
        return;

    //draw the value
    szName = m_oConstants.GetFavoriteConstant(*iIndex);
    DrawText(gdi, szName, rc, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS, m_hFontSmallTop, TEXTCOLOR_Selected);

}

void CIssCalcInterface::DrawIconItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CIssCalcInterface* pThis = (CIssCalcInterface*)lpClass;
    if(pThis)
        pThis->DrawIconItem(gdi, rcDraw, sItem, bIsHighlighted);
}

void CIssCalcInterface::DrawIconItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(m_gdiIcons.GetDC() == NULL)
        return;

    TypeMenu* sMenu = (TypeMenu*)sItem->lpItem;
    if(!sMenu)
        return;

    int iOffSet = m_gdiIcons.GetHeight()/8;

    ::Draw(gdi,
         rcDraw.left - iOffSet, rcDraw.top + (HEIGHT(rcDraw) - m_gdiIcons.GetHeight())/2,
         m_gdiIcons.GetHeight(), m_gdiIcons.GetHeight(),
         m_gdiIcons,
         sMenu->uiMenu*m_gdiIcons.GetHeight(), 0);

    RECT rc;
    if(m_gdiVertBar.GetDC() == NULL)
    {
        int iHeight = m_gdiIcons.GetHeight();
        m_gdiVertBar.Create(gdi.GetDC(), 1, iHeight, FALSE, TRUE);
        
        SetRect(&rc, 0, 0, 1, iHeight/4);
        GradientFillRect(m_gdiVertBar, rc, 0, 0xFFFFFF);
        SetRect(&rc, 0, iHeight/4, 1, iHeight*3/4);
        FillRect(m_gdiVertBar, rc, 0xFFFFFF);
        SetRect(&rc, 0, iHeight*3/4, 1, iHeight);
        GradientFillRect(m_gdiVertBar, rc, 0xFFFFFF, 0);
    }
    BitBlt(gdi,
           rcDraw.left + m_gdiIcons.GetHeight(), rcDraw.top + (HEIGHT(rcDraw)-m_gdiVertBar.GetHeight())/2,
           m_gdiVertBar.GetWidth(), m_gdiVertBar.GetHeight(),
           m_gdiVertBar,
           0,0,
           SRCCOPY);

    rc = rcDraw;
    rc.left += (m_gdiIcons.GetHeight() + m_gdiIcons.GetHeight()/4);

    DrawText(gdi.GetDC(), sMenu->szMenu, rc, DT_LEFT|DT_VCENTER, m_hFontMenu, (bIsHighlighted?TEXTCOLOR_Selected:TEXTCOLOR_Normal));

    /*rc = rcDraw;
    rc.top  = rc.bottom - 1;
    FillRect(gdi, rc, RGB(32,32,32));*/
}

void CIssCalcInterface::OnMenuRPNStack()
{
    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu.ResetContent();
    m_wndMenu.SetDeleteItemFunc(DeleteConstantsItem);

    m_wndMenu.AddCategory(_T("RPN Stack"));

    int iStackSize = m_oCalc->GetRPNStackSize();

    for(int i = 0; i < iStackSize; i++)
    {
        int* iIndex = new int;
        *iIndex = i;
        m_wndMenu.AddItem(iIndex, -1, NULL);
    }

    m_wndMenu.SetSelectedItemIndex(1, TRUE);
    m_wndMenu.SetCustomDrawFunc(DrawRPNItem, this);

    int iHeight = GetSystemMetrics(SM_CXICON);
    m_wndMenu.SetItemHeights(iHeight, iHeight);

    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.right/2;
    rcBtn.top   = rc.bottom - m_oMenu.GetMenuHeight();
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right;

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu.GetMenuHeight();

    DWORD dwOptions = 0;

    if(m_oCalc->GetRPNStackSize() > 5)
        dwOptions = OPTION_DrawScrollArrows;

    m_wndMenu.PopupMenu(m_hWndDlg, m_hInst,
        dwOptions,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);
}

void CIssCalcInterface::OnMenuConstants()
{
    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu.ResetContent();
    m_wndMenu.SetDeleteItemFunc(DeleteConstantsItem);

    if(m_oConstants.GetFavoriteCount() == 0)
        return;

    m_wndMenu.AddCategory(_T("Constant Favorites"));
    for(int i=0; i<m_oConstants.GetFavoriteCount(); i++)
    {
        int* iConstIndex = new int;
        *iConstIndex = i;
        m_wndMenu.AddItem(iConstIndex, IDMENU_ConstFav + i, NULL);
        /*TCHAR* szName = m_oConstants.GetFavoriteName(i);
        TCHAR* szGroup= m_oConstants.GetFavoriteGroup(i);
        if(!szName || !szGroup)
            continue;
        m_wndMenu.AddCategory(szGroup);
        m_wndMenu.AddItem(szName, IDMENU_ConstFav + i, NULL);*/
    }

    m_wndMenu.SetSelectedItemIndex(1, TRUE);
    m_wndMenu.SetCustomDrawFunc(DrawConstantsItem, this);
    
    int iHeight = GetSystemMetrics(SM_CXICON)*6/5;
    m_wndMenu.SetItemHeights(iHeight, iHeight);

    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.right/2;
    rcBtn.top   = rc.bottom - m_oMenu.GetMenuHeight();
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right;

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu.GetMenuHeight();

    m_wndMenu.PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);
}

void CIssCalcInterface::OnMenuConstantSelect(int iIndex)
{
    TCHAR* szConstant = m_oConstants.GetFavoriteConstant(iIndex);
    if(!szConstant)
        return;

    m_oCalc->AddString(szConstant);
    InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
}

void CIssCalcInterface::DeleteMenuItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    TypeMenu* sMenu = (TypeMenu*)lpItem;

    CIssString* oStr = CIssString::Instance();

    if(sMenu)
    {
        if(sMenu->szMenu)
            oStr->Delete(&sMenu->szMenu);
        delete sMenu;
    }
}

void CIssCalcInterface::DeleteConstantsItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    int* iTemp = (int*)lpItem;

    if(iTemp)
        delete iTemp;
}