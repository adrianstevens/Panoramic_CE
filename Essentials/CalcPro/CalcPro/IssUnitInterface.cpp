#include "IssUnitInterface.h"
#include "globals.h"
#include "IssRegistry.h"
#include "IssLocalisation.h"


CIssUnitInterface::CIssUnitInterface(void)
:m_oCalcDis(CIssFormatCalcDisplay::Instance())
,m_hFontBtnText(NULL)
,m_hFontBtnTextSm(NULL)
,m_hFontDisplay(NULL)
,m_hFontExp(NULL)
,m_hFontTop(NULL)
,m_hFontBtnTextLong(NULL)
,m_iSelector(0)
{
	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, 400, TRUE);
	LoadRegistry();

}

CIssUnitInterface::~CIssUnitInterface(void)
{
	//seriously ... you gotta check this cleanup ... think we've got some buttons to delete properly
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);
	CIssGDIEx::DeleteFont(m_hFontDisplay);
	CIssGDIEx::DeleteFont(m_hFontExp);
	CIssGDIEx::DeleteFont(m_hFontTop);
    CIssGDIEx::DeleteFont(m_hFontBtnTextLong);

    SaveRegistry();
}


void CIssUnitInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    if(m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_UnitConv ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
	    CreateDisplays();
	    CreateButtons();

	    //sets up our strings and such
		SaveRegistry();
	    m_oUnit.Init(m_hInst);
		LoadRegistry();
    }
}

BOOL CIssUnitInterface::Draw(CIssGDIEx& gdiMem, RECT& rc)
{
    HDC hdc = gdiMem.GetDC();

	FillRect(hdc, rc, m_oBtnMan->GetSkin()->GetBackgroundColor());

	DrawDisplays(hdc, rc);
	DrawDisplayText(hdc, rc);
	DrawUnitBtns(hdc, rc);
	DrawMainBtns(gdiMem, rc);


	return TRUE;
}

BOOL CIssUnitInterface::DrawDisplays(HDC hdc, RECT& rcClip)
{
	for(int i = 0; i < 3; i++)
	{
		m_oDisplay[i].Draw(hdc);
	}

	return TRUE;
}

BOOL CIssUnitInterface::DrawDisplayText(HDC hdc, RECT& rcClip)
{
	static TCHAR szTemp[STRING_LARGE];
	static TCHAR szUnits[STRING_NORMAL];
    static TCHAR szFull[STRING_LARGE];
    static TCHAR szValue[STRING_LARGE];
	static RECT rcTemp;

    static DisplayType eDisplay = m_oCalc->GetCalcDisplay();

    if(m_bExpired)
    {
        rcTemp = m_oDisplay[UDIS_From].GetPosition();
        DrawText(hdc, m_oStr->GetText(ID(IDS_MSG_TrialExpired), m_hInst), rcTemp, DT_CENTER | DT_VCENTER, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
        return TRUE;
    }

    //no fractions on the unit converter
    if(eDisplay == DISPLAY_Fractions)
        m_oCalc->SetCalcDisplay(DISPLAY_Float);

	m_oUnit.GetSectionName(m_oUnit.GetSection(), szTemp);
	//the conversion type
    if(m_oStr->GetLength(szTemp) > 13)//just a guess
        DrawText(hdc, szTemp, m_oDisplay[UDIS_Category].GetPosition(), DT_CENTER | DT_VCENTER, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    else
        DrawText(hdc, szTemp, m_oDisplay[UDIS_Category].GetPosition(), DT_CENTER | DT_VCENTER, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);


	//draw the conversion type and the units
	//From
	rcTemp = m_oDisplay[UDIS_From].GetPosition();
	rcTemp.top += DISPLAY_INDENT;
	rcTemp.left += 2*DISPLAY_INDENT;
	rcTemp.right -= 2*DISPLAY_INDENT;

	m_oUnit.GetFromUnits(szUnits);
	DrawText(hdc, szUnits, rcTemp, DT_LEFT | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

    m_oStr->Empty(szValue);
    m_oStr->Empty(szUnits);
    m_oCalc->GetAnswer(szFull, szValue, szUnits, FALSE, FALSE);//no regional 
    m_oCalcDis->NumberFormat(szFull, szFull, szValue, szUnits, FALSE, m_iDisplayChars);//last value is the display characters ... we'll keep it small for this

    if(m_oStr->GetLength(szUnits) == 0)
    {
        DrawText(hdc, szFull, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
    }
    else
    { //exponents and string
        rcTemp.right -= GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
        
        rcTemp.top -= DISPLAY_INDENT/2;

        rcTemp.right += GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szUnits, rcTemp, DT_RIGHT | DT_TOP, m_hFontExp, m_oBtnMan->GetSkin()->m_crText);
       
    }
    

	//To
	rcTemp = m_oDisplay[UDIS_To].GetPosition();
	rcTemp.top += DISPLAY_INDENT;
	rcTemp.left += 2*DISPLAY_INDENT;
	rcTemp.right -= 2*DISPLAY_INDENT;

	m_oUnit.GetToUnits(szUnits);
	DrawText(hdc, szUnits, rcTemp, DT_LEFT | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

    //// GET "TO" VALUES /////////////////////////////////////////////////////////////////
    m_oUnit.DisplayResults(szFull, szFull, UNIT_DISPLAY_FULL);

    m_oStr->Empty(szValue);
    m_oStr->Empty(szUnits);

    // convert the number value to the proper regional format
    m_oCalcDis->NumberFormat(szFull, szFull, szValue, szUnits, FALSE, m_iDisplayChars);//last value is the display characters ... we'll keep it small for this
   
    if(m_oStr->GetLength(szUnits) == 0)
    {
        DrawText(hdc, szFull, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
    }
    else
    {
        rcTemp.right -= GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);

        rcTemp.top -= DISPLAY_INDENT/2;

        rcTemp.right += GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szUnits, rcTemp, DT_RIGHT | DT_TOP, m_hFontExp, m_oBtnMan->GetSkin()->m_crText);
    }

    m_oCalc->SetCalcDisplay(eDisplay);
	return TRUE;
}

BOOL CIssUnitInterface::DrawUnitBtns(HDC hdc, RECT& rcClip)
{
//	TCHAR* szText[3] = {_T("From"), _T("To"), _T("Conversion")};

    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szTemp, ID(IDS_BTN_From), STRING_NORMAL, m_hInst);
    TCHAR szTemp2[STRING_NORMAL];
    m_oStr->StringCopy(szTemp2, ID(IDS_BTN_To), STRING_NORMAL, m_hInst);
    TCHAR szTemp3[STRING_NORMAL];
    m_oStr->StringCopy(szTemp3, ID(IDS_BTN_Conversion), STRING_NORMAL, m_hInst);
	
	/*for(int i = 0; i < (int)UBTN_Count; i++)
	{
		m_oUnitBtns[i].Draw(m_hFontBtnTextSm, hdc, szText[i]);
	}*/

    m_oUnitBtns[0].Draw(m_hFontBtnTextSm, hdc, szTemp);
    m_oUnitBtns[1].Draw(m_hFontBtnTextSm, hdc, szTemp2);

    if(m_oStr->GetLength(szTemp3) > 10)//thanks german
        m_oUnitBtns[2].Draw(m_hFontTop, hdc, szTemp3);
    else
        m_oUnitBtns[2].Draw(m_hFontBtnTextSm, hdc, szTemp3);

    DrawDownArrow(hdc, m_oUnitBtns[0].GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);

    DrawDownArrow(hdc, m_oUnitBtns[1].GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);

    DrawDownArrow(hdc, m_oUnitBtns[2].GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);


	return TRUE;
}

BOOL CIssUnitInterface::DrawKeyboardBtns(CIssGDIEx& gdiMem, RECT& rcClip)
{
	LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();
	CIssVector<ButtonType>* arrBtnType = &m_oBtnMan->GetCurrentLayout()->sKeyboardButtons;

	if(sLayout == NULL)
		return FALSE;

	if(sLayout->sKeyboardButtons.GetSize() == 0)
		return FALSE;

	CCalcDynBtn* oBtn = NULL;

	HFONT hFontTemp;
    RECT rcButton;

	int iButton = 0;

	for(int i = 0; i < sLayout->sKeyboardButtons.GetSize(); i++)
	{
		oBtn = NULL;
		oBtn = &m_oMainBtns[i];

		if(oBtn)
		{
			//now lets optimize a bit
			if(rcClip.left > oBtn->GetButtonRect().right ||
				rcClip.right < oBtn->GetButtonRect().left ||
				rcClip.top > oBtn->GetButtonRect().bottom ||
				rcClip.bottom < oBtn->GetButtonRect().top)
				continue;

			iButton = i;

			//we're good... carry on
			if(m_oStr->GetLength(sLayout->sKeyboardButtons[iButton]->szLabel) > 2)
				hFontTemp = m_hFontBtnTextLong;
			else
				hFontTemp = m_hFontBtnText;

			//oBtn->Draw(hFontTemp, hdc, sLayout->sKeyboardButtons[iButton]->szLabel);

            rcButton = oBtn->GetButtonRect();
            AlphaFillRect(gdiMem, rcButton, 0xFFFFFF, 127);

            oBtn->DrawButtonText(hFontTemp, gdiMem.GetDC(), oBtn->GetButtonRect(), sLayout->sKeyboardButtons[iButton]->szLabel, NULL, hFontTemp);

			static RECT rcTemp;
			rcTemp = oBtn->GetButtonRect();
			rcTemp.right -= GetSystemMetrics(SM_CXSMICON)/6;
			if(m_oBtnMan->GetKBLetters())
				::DrawText(gdiMem.GetDC(), m_oBtnMan->GetKeyName(i), rcTemp, DT_TOP | DT_RIGHT, m_hFontTop, 0xFFFFFF);
		}
	}

	return TRUE;
}


BOOL CIssUnitInterface::DrawMainBtns(CIssGDIEx& gdiMem, RECT& rcClip)
{
    
	if(ShowKBButtons())
        return DrawKeyboardBtns(gdiMem, rcClip);

    HDC hdc = gdiMem.GetDC();


	LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

	TCHAR* szText = NULL;

    HFONT hFontTemp = NULL;

    int iCount = sLayout->iMainBtnColumns*sLayout->iMainBtnRows;
	for(int i = 0; i < iCount; i++)
	{
        szText = sLayout->sMainButtons[i]->szLabel;
        hFontTemp = m_hFontBtnText;

        if(m_oStr->GetLength(sLayout->sMainButtons[i]->szLabel) > 2)
            hFontTemp = m_hFontBtnTextLong;

//#ifdef WIN32_PLATFORM_WFSP //draw the extra labels yo
        //we're going to do a bit of a gross hack ... if we're on landscape we're not going to draw the main buttons
       /* if(m_oBtnHand->GetDeviceType() == DEVTYPE_SPLand)
            continue;

        else */if(m_oBtnHand->GetDeviceType() == DEVTYPE_SP)
        {
            m_oMainBtns[i].Draw(m_hFontBtnTextSm, hdc, szText, NULL, sLayout->sMainAltButtons[i]->szLabel, NULL, m_hFontTop);
        }
        else
//#endif
        {
            m_oMainBtns[i].Draw(hFontTemp, hdc, szText);
        }

        //here we'll check for special cases
        //backspace
        int iType = sLayout->sMainButtons[i]->iSkinIndex;

        if(sLayout->sMainButtons[i]->iButtonType == INPUT_Clear &&
            sLayout->sMainButtons[i]->iButtonValue == CLEAR_BackSpace)
            DrawBackSpace(hdc, m_oMainBtns[i].GetButtonRect(),
            m_oBtnMan->GetSkin()->GetButton(iType)->TextColor,
            m_oBtnMan->GetSkin()->GetButton(iType)->TextColor2,
            m_oBtnMan->GetSkin()->m_bUseTextShadow);
	}
	return TRUE;
}



BOOL CIssUnitInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	
	m_rcDisplay.top = 0;
	m_rcDisplay.left = rcClient.left;
	m_rcDisplay.right = rcClient.right;

	//set up the basic rects
#ifdef UNDER_CE
    if((GetSystemMetrics(SM_CYSCREEN)%400) == 0)
    {   //ultra tall
        m_rcDisplay.bottom = 3*(DISPLAY_HEIGHT+2*DISPLAY_INDENT);//yeah yeah ... its just how it works out .. trust me
    }
    else
#endif
    if(IsLandscape())//landscape

	{
		m_rcDisplay.bottom = 2*(DISPLAY_HEIGHT-DISPLAY_INDENT);//yeah yeah ... its just how it works out .. trust me
	}
	else
	{
		m_rcDisplay.bottom = 3*(DISPLAY_HEIGHT-DISPLAY_INDENT);//yeah yeah ... its just how it works out .. trust me
	}
	
	m_rcMainBtns = rcClient;
	m_rcMainBtns.top = m_rcDisplay.bottom;

	//if we're displayed ... re-init
	if(m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_UnitConv)
	{
		CreateDisplays();
		CreateButtons();
	}

    //fonts
    CIssGDIEx::DeleteFont(m_hFontDisplay);
    CIssGDIEx::DeleteFont(m_hFontExp);

    m_iDisplayChars = 14;

    if(IsLandscape())
    {//landscape
        m_hFontDisplay = CIssGDIEx::CreateFont(DISPLAY_TEXT_SIZE-GetSystemMetrics(SM_CXSMICON)*3/16, 400, TRUE);
        m_hFontExp     = CIssGDIEx::CreateFont(EXPONENT_TEXT_SIZE-GetSystemMetrics(SM_CXSMICON)*3/16, 400, TRUE);
        if((WIDTH(rcClient)%400)!= 0)
            m_iDisplayChars = 10;//otherwise ultra wide and we're good
    }
    else
    {
        m_hFontDisplay = CIssGDIEx::CreateFont(DISPLAY_TEXT_SIZE, 400, TRUE);
        m_hFontExp     = CIssGDIEx::CreateFont(EXPONENT_TEXT_SIZE, 400, TRUE);
        
    }

	if(m_hFontTop)
		CIssGDIEx::DeleteFont(m_hFontTop);
	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, 400, TRUE);

	return TRUE;
}

BOOL CIssUnitInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
	for(int i = 0; i < UBTN_Count; i++)
		if(m_oUnitBtns[i].OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &m_oUnitBtns[i].GetButtonRect(), FALSE);		
			return TRUE;
		}

	for(int i = 0; i < NUM_KB_BUTTONS; i++)
		if(m_oMainBtns[i].OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &m_oMainBtns[i].GetButtonRect(), FALSE);	
			return TRUE;
		}

    if(PtInRect(&m_oDisplay[UDIS_From].GetPosition(), pt) || PtInRect(&m_oDisplay[UDIS_To].GetPosition(), pt))
    { 
        TCHAR szTemp[STRING_LARGE];
        m_oCalc->GetAnswer(szTemp);
        m_oUnit.GetResults(szTemp, szTemp);
        m_oCalc->AddString(szTemp);
        m_oCalc->AddEquals(EQUALS_Equals);
        m_oUnit.Switch();
        InvalidateRect(hWnd, &m_rcDisplay, FALSE);
    }
    else if(PtInRect(&m_oDisplay[UDIS_Category].GetPosition(), pt))
    {
        int iTemp = m_oUnit.GetSection();
        iTemp++;
        if(iTemp >= m_oUnit.GetSectionCount())
            iTemp = 0;
        m_oUnit.SetSection(iTemp);
        InvalidateRect(hWnd, &m_rcDisplay, FALSE);
    }

	return UNHANDLED;
}

BOOL CIssUnitInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
	for(int i = 0; i < UBTN_Count; i++)
		if(m_oUnitBtns[i].OnLButtonUp(pt))
		{
			InvalidateRect(hWnd, &m_oUnitBtns[i].GetButtonRect(), FALSE);	

			switch(i)
			{
			case UBTN_Category:
				OnBtnConversion();
				break;
			case UBTN_From:
				OnBtnFrom();
				break;
			case UBTN_To:
				OnBtnTo();
			    break;
			default:
			    break;
			}

			return TRUE;
		}

	for(int i = 0; i < NUM_KB_BUTTONS; i++)
    {
		if(m_oMainBtns[i].OnLButtonUp(pt))
		{
            if( m_oBtnMan->GetKBType() != KB_Original)
            {
                DWORD dwType = m_oBtnMan->GetCurrentLayout(NULL)->sKeyboardButtons[i]->iButtonType;
                DWORD dwID = m_oBtnMan->GetCurrentLayout(NULL)->sKeyboardButtons[i]->iButtonValue;
                m_oCalc->CalcButtonPress(dwType, dwID);
                InvalidateRect(m_hWndDlg, NULL, FALSE);
            }
            else
            {
			    InvalidateRect(hWnd, &m_oMainBtns[i].GetButtonRect(), FALSE);		
                InvalidateRect(hWnd, &m_rcDisplay, FALSE);		

			    PostMessage(hWnd, WM_CALCENGINE, m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonType, 
				    m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonValue);
            }
			//and play the sound
			//PlaySounds(_T("IDR_WAVE_Click"));
			return TRUE;
		}
    }

	return UNHANDLED;
}

void CIssUnitInterface::CreateKBButtons()
{
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout(m_hInst);

    if(sLayout == NULL ||
        sLayout->iMainBtnColumns == 0 ||
        sLayout->iMainBtnRows == 0)
        return;

    int iButtonWidth  = 0;
    int iButtonHeight = 0;
    RECT rcButtonSize = {0, 0, 0, 0};

    iButtonWidth = (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - 9*BTN_SPACING)/10;
    iButtonHeight = (HEIGHT(m_rcMainBtns) - m_oMenu->GetMenuHeight() - BTN_EDGE_INDENT - 2*BTN_SPACING)/3; 

    CalcProSkins* oSkin = m_oBtnMan->GetSkin();
//    CCalcDynBtn* oBtn = NULL;

    int iSkinIndex = 0;
    int iCurrentBtn = 0;

    switch(m_oBtnMan->GetSkinType())
    {
    case SKIN_Pink:
    case SKIN_Blue:    
        iSkinIndex = 1;
        break;
    case SKIN_Black:
    case SKIN_Brown:
    case SKIN_Green:
    case SKIN_Silver:
    default:
        iSkinIndex = 0;
        break;
    }

    //hard coded but oh well
    for(int i = 0; i < 10; i++) //three rows at once
    {
        //row 1
        rcButtonSize.left   = BTN_EDGE_INDENT + i*(BTN_SPACING+iButtonWidth);
        rcButtonSize.top    = BTN_SPACING + m_rcMainBtns.top;
        rcButtonSize.right  = rcButtonSize.left + iButtonWidth;
        rcButtonSize.bottom = rcButtonSize.top + iButtonHeight;

        //get the skin index
        //iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
        m_oMainBtns[iCurrentBtn].Destroy();
        m_oMainBtns[iCurrentBtn].SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, FALSE);
        m_oMainBtns[iCurrentBtn].InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
            oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
            oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
            oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

        //init the Button
        m_oMainBtns[iCurrentBtn].Init(rcButtonSize, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);
        iCurrentBtn++;


        //row 2
        if(i < 9)
        {
            rcButtonSize.top   += (BTN_SPACING + iButtonHeight);
            rcButtonSize.bottom+= (BTN_SPACING + iButtonHeight);

            //iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
            m_oMainBtns[iCurrentBtn].Destroy();
            m_oMainBtns[iCurrentBtn].SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, FALSE);
            m_oMainBtns[iCurrentBtn].InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
                oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

            //init the Button
            m_oMainBtns[iCurrentBtn].Init(rcButtonSize, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);
            iCurrentBtn++;
        }

        //row 3
        if(i > 7)//no first button
            continue;

        rcButtonSize.top   += (BTN_SPACING + iButtonHeight);
        rcButtonSize.bottom+= (BTN_SPACING + iButtonHeight);

        //iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
        m_oMainBtns[iCurrentBtn].Destroy();
        m_oMainBtns[iCurrentBtn].SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, FALSE);
        m_oMainBtns[iCurrentBtn].InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
            oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
            oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
            oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

        //init the Button
        m_oMainBtns[iCurrentBtn].Init(rcButtonSize, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);
        iCurrentBtn++;
    }
}

void CIssUnitInterface::CreateMainButtons()
{
	if(ShowKBButtons())
	{
		CreateKBButtons();
		return;
	}

    //MAIN BUTTONS
    //based on our m_rcSize;
    int iButtonWidth	= 0;
    int iButtonHeight	= 0;
    int iTopIndent      = 0;
    int iBtnSpaceH      = 1;
    int iBtnSpaceV      = 1;
    RECT rcButtonSize	= {0,0,0,0};

    CalcProSkins* oSkin = m_oBtnMan->GetSkin();

    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    if(sLayout == NULL ||
        sLayout->iMainBtnColumns == 0 ||
        sLayout->iMainBtnRows == 0)
        return;

//    oBtn = NULL;

    iButtonWidth		= (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - (sLayout->iMainBtnColumns - 1)*BTN_SPACING) / sLayout->iMainBtnColumns;
#ifdef UNDER_CE
    if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
    {
        iButtonHeight		= (HEIGHT(m_rcMainBtns) - m_oMenu->GetMenuHeight() - (sLayout->iMainBtnRows - 1)*iBtnSpaceV) / sLayout->iMainBtnRows;
        iBtnSpaceH          = BTN_SPACING;
        iBtnSpaceV          = 1;
    }
    else
#endif
    {
        iButtonHeight		= (HEIGHT(m_rcMainBtns) - BTN_SPACING - m_oMenu->GetMenuHeight() - (sLayout->iMainBtnRows - 1)*BTN_SPACING) / sLayout->iMainBtnRows;
        iTopIndent          = BTN_SPACING;
        iBtnSpaceH          = BTN_SPACING;
        iBtnSpaceV          = BTN_SPACING;
    }

    if(iButtonHeight < 1 ||
        iButtonWidth < 1)
        return;

    int	iSkinIndex = 0;
    int iCurrentBtn = 0;

    for(int j = 0; j < sLayout->iMainBtnRows; j++)
    {
        for(int i = 0; i < sLayout->iMainBtnColumns; i++)
        {
            iCurrentBtn = sLayout->iMainBtnColumns*j+i;

            if(iCurrentBtn == NUM_MAIN_BUTTONS)
            {	ASSERT(0);}

            rcButtonSize.left	= BTN_EDGE_INDENT + i*(iBtnSpaceH+iButtonWidth);
            rcButtonSize.top	= iTopIndent + j*(iBtnSpaceV+iButtonHeight) + m_rcMainBtns.top;
            rcButtonSize.right	= rcButtonSize.left + iButtonWidth;
            rcButtonSize.bottom	= rcButtonSize.top + iButtonHeight;

            //get the skin index
            iSkinIndex = sLayout->sMainButtons[iCurrentBtn]->iSkinIndex;

            //create the buttons based off of the displays to keep things tidy
            m_oMainBtns[iCurrentBtn].Destroy();
            m_oMainBtns[iCurrentBtn].SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, oSkin->m_bUseTextShadow);
            m_oMainBtns[iCurrentBtn].InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
                oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

            //init the Button
            m_oMainBtns[iCurrentBtn].Init(rcButtonSize, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);
        }
    }

    int iTemp = min(iButtonHeight, iButtonWidth);

    if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
    {
        //and finally ... fonts
        if(m_hFontBtnText == NULL)
            m_hFontBtnText = CIssGDIEx::CreateFont(iTemp, FW_BOLD, TRUE);
        if(m_hFontBtnTextSm == NULL)
            m_hFontBtnTextSm = CIssGDIEx::CreateFont(DISPLAY_HEIGHT*2/5, FW_BOLD, TRUE);
        if(m_hFontBtnTextLong == NULL)
            m_hFontBtnTextLong = CIssGDIEx::CreateFont(iTemp*3/4, FW_BOLD, TRUE);
    }
    else
    {
        //and finally ... fonts
        if(m_hFontBtnText == NULL)
            m_hFontBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
        if(m_hFontBtnTextSm == NULL)
            m_hFontBtnTextSm = CIssGDIEx::CreateFont(DISPLAY_HEIGHT*2/5, FW_BOLD, TRUE);
        if(m_hFontBtnTextLong == NULL)
            m_hFontBtnTextLong = CIssGDIEx::CreateFont(iTemp/2, FW_BOLD, TRUE);
    }
}

void CIssUnitInterface::CreateButtons()
{
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);

	CalcProSkins* oSkin = m_oBtnMan->GetSkin();

	CCalcDynBtn* oBtn = NULL;
	
	RECT rc;

	//we'll start unit converter buttons
	for(int i = 0; i < 3; i++)
	{
		rc = m_oDisplay[i].GetPosition();

		rc.left = rc.right + DISPLAY_INDENT;
		rc.right = rc.left + UNIT_BTN_WIDTH;

		if(i == (int)UBTN_Category)
		{
            rc.right += (UNIT_BTN_WIDTH_LRG - UNIT_BTN_WIDTH);
			oBtn = NULL;
		}

		//create the buttons based off of the displays to keep things tidy
		m_oUnitBtns[i].Destroy();
		m_oUnitBtns[i].SetTextColors(oSkin->m_typeBtnClr[4].TextColor, oSkin->m_typeBtnClr[4].TextColor2, oSkin->m_bUseTextShadow);
		m_oUnitBtns[i].InitAdvanced(oSkin->m_typeBtnClr[4].ButtonColor1,
			oSkin->m_typeBtnClr[4].ButtonColor2,
			oSkin->m_typeBtnClr[4].OutlineColor1,
			oSkin->m_typeBtnClr[4].OutlineColor2);

		//init the Button
		m_oUnitBtns[i].Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);
		
		oBtn = &m_oUnitBtns[0];//so we only create the background once ... saves mem
	}

	CreateMainButtons();
}

void CIssUnitInterface::CreateDisplays()
{
    if(m_oBtnMan == NULL)
    {
        ASSERT(0);
        return;
    }

	RECT rcClient;
	GetClientRect(m_hWndDlg, &rcClient);

    int iDisplayHeight = DISPLAY_HEIGHT; 

	if(ShowKBButtons() == TRUE && IsLandscape() == TRUE)
		iDisplayHeight = iDisplayHeight*2/3+2;
	else if((GetSystemMetrics(SM_CYSCREEN)%400) == 0)
        iDisplayHeight += 3*DISPLAY_INDENT;

   
    CalcProSkins* oSkin = m_oBtnMan->GetSkin();//makes for tidier code

	RECT rcTemp;

    //FROM
    if(IsLandscape() && ShowKBButtons() == FALSE)
	{	
		rcTemp.top		= DISPLAY_INDENT;
		rcTemp.left		= DISPLAY_INDENT;
		rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;
		rcTemp.right	= WIDTH(rcClient)/2 - DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	}
	else//portrait
	{
		rcTemp.top		= DISPLAY_INDENT;
		rcTemp.left		= DISPLAY_INDENT;
		rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;
		rcTemp.right	= rcClient.right - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	}

	m_oDisplay[UDIS_From].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);

    //TO
    if(IsLandscape() && ShowKBButtons() == FALSE)
	{
		rcTemp.left		= rcTemp.right + 2*DISPLAY_INDENT + UNIT_BTN_WIDTH;
		rcTemp.right	= rcClient.right - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	}
	else
	{
		rcTemp.top		= rcTemp.bottom + DISPLAY_INDENT;
		rcTemp.left		= DISPLAY_INDENT;
		rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;
		rcTemp.right	= rcClient.right - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	}

	m_oDisplay[UDIS_To].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);

    //CONVERSION
	rcTemp.top		= rcTemp.bottom + DISPLAY_INDENT;
	rcTemp.left		= DISPLAY_INDENT;
	rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;

    rcTemp.right	= rcClient.right - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH_LRG;//make room for the button on the right

	m_oDisplay[UDIS_Category].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);

}

BOOL CIssUnitInterface::OnBtnFrom()
{
    if(!m_wndMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();
    for(int i = 0; i < m_oUnit.GetSubSectionCount(); i++)
    {
        m_oUnit.GetSubSectionName(i, szText);
        m_wndMenu->AddItem(szText, IDMENU_Unit_From + i, (i==m_oUnit.GetFromSelected()?FLAG_Radio:NULL));
    }
    m_wndMenu->SetSelectedItemIndex(m_oUnit.GetFromSelected(), TRUE);

    RECT rcBtn = m_oUnitBtns[UBTN_From].GetButtonRect();
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu->GetMenuHeight();

    m_wndMenu->PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);

	/*HMENU hMenu = CreatePopupMenu();
	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];

	for(int i = 0; i < m_oUnit.GetSubSectionCount(); i++)
	{
		m_oUnit.GetSubSectionName(i, szText);
		AppendMenu(hMenu, MF_STRING, i + IDMENU_Unit_From, szText);
	}

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_oUnit.GetFromSelected(), TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, m_oUnit.GetSubSectionCount() - 1, m_oUnit.GetFromSelected(), MF_BYPOSITION);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWndDlg, &rc);
	pt.x	= m_oUnitBtns[UBTN_From].GetButtonRect().right;
	pt.y	= m_oUnitBtns[UBTN_From].GetButtonRect().bottom;

	ClientToScreen(m_hWndDlg, &pt);

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

	DestroyMenu(hMenu);*/
	return TRUE;
}

BOOL CIssUnitInterface::OnBtnTo()
{
    if(!m_wndMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();
    for(int i = 0; i < m_oUnit.GetSubSectionCount(); i++)
    {
        m_oUnit.GetSubSectionName(i, szText);
        m_wndMenu->AddItem(szText, IDMENU_Unit_To + i, (i==m_oUnit.GetToSelected()?FLAG_Radio:NULL));
    }
    m_wndMenu->SetSelectedItemIndex(m_oUnit.GetToSelected(), TRUE);

    RECT rcBtn = m_oUnitBtns[UBTN_To].GetButtonRect();
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu->GetMenuHeight();

    m_wndMenu->PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);

	/*HMENU hMenu = CreatePopupMenu();
	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];
	m_oStr->Empty(szText);

	for(int i = 0; i < m_oUnit.GetSubSectionCount(); i++)
	{
		m_oUnit.GetSubSectionName(i, szText);
		AppendMenu(hMenu, MF_STRING, i + IDMENU_Unit_To, szText);
	}

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_oUnit.GetToSelected(), TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, m_oUnit.GetSubSectionCount() - 1, m_oUnit.GetToSelected(), MF_BYPOSITION);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWndDlg, &rc);
	pt.x	= m_oUnitBtns[UBTN_To].GetButtonRect().right;
	pt.y	= m_oUnitBtns[UBTN_To].GetButtonRect().bottom;

	ClientToScreen(m_hWndDlg, &pt);

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

	DestroyMenu(hMenu);*/
	return TRUE;
}

BOOL CIssUnitInterface::OnBtnConversion()
{
    if(!m_wndMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();
    for(int i = 0; i < m_oUnit.GetSectionCount(); i++)
    {
        m_oUnit.GetSectionName(i, szText);
        m_wndMenu->AddItem(szText, IDMENU_Unit_Conv + i, (i==m_oUnit.GetSection()?FLAG_Radio:NULL));
    }
    m_wndMenu->SetSelectedItemIndex(m_oUnit.GetSection(), TRUE);

    RECT rcBtn = m_oUnitBtns[UBTN_Category].GetButtonRect();

    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu->GetMenuHeight();

    m_wndMenu->PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);

	/*HMENU hMenu = CreatePopupMenu();
	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];
	m_oStr->Empty(szText);

	for(int i = 0; i < m_oUnit.GetSectionCount(); i++)
	{
		m_oUnit.GetSectionName(i, szText);
		AppendMenu(hMenu, MF_STRING, IDMENU_Unit_Conv + i, szText);
	}

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_oUnit.GetSection(), TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 
    CheckMenuRadioItem(hMenu, 0, m_oUnit.GetSectionCount() - 1, m_oUnit.GetSection(), MF_BYPOSITION);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWndDlg, &rc);
	pt.x	= m_oUnitBtns[UBTN_Category].GetButtonRect().right;
	pt.y	= m_oUnitBtns[UBTN_Category].GetButtonRect().bottom;

	ClientToScreen(m_hWndDlg, &pt);

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

	DestroyMenu(hMenu);*/
	return TRUE;
}

BOOL CIssUnitInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iParam = LOWORD(wParam);

	if(iParam >= IDMENU_Unit_Conv && iParam < IDMENU_Unit_Conv + 100)
	{
		m_oUnit.SetSection(iParam - IDMENU_Unit_Conv);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else if(iParam >= IDMENU_Unit_From && iParam < IDMENU_Unit_From + 100)
	{	
		m_oUnit.SetSubSectionFrom(iParam - IDMENU_Unit_From);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else if(iParam >= IDMENU_Unit_To && iParam < IDMENU_Unit_To + 100)
	{
		m_oUnit.SetSubSectionTo(iParam - IDMENU_Unit_To);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else
		return FALSE;

	return TRUE;
}

BOOL CIssUnitInterface::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
    return UNHANDLED;
}

void CIssUnitInterface::LoadRegistry()
{
    DWORD dwTemp;

    if(S_OK == GetKey(REG_KEY, _T("UnitCount"), dwTemp))
        m_oUnit.SetSection((int)dwTemp);
    else 
        dwTemp = 0;

    if(dwTemp == m_oUnit.GetSectionCount())//means we haven't added a new section
    {
        for(int i = 0; i < m_oUnit.GetSectionCount(); i++)
        {
            TCHAR szRegKey[STRING_NORMAL];
            m_oStr->Format(szRegKey, _T("UnitSectionTo%i"), i);

            if(S_OK == GetKey(REG_KEY, szRegKey, dwTemp))
            {
                m_oUnit.SetSection(i);
                m_oUnit.SetSubSectionTo(dwTemp);
            }

            m_oStr->Format(szRegKey, _T("UnitSectionFrom%i"), i);

            if(S_OK == GetKey(REG_KEY, szRegKey, dwTemp))
            {
                m_oUnit.SetSection(i);
                m_oUnit.SetSubSectionFrom(dwTemp);
            }
        }
    }

    if(S_OK == GetKey(REG_KEY, _T("UnitType"), dwTemp))
        m_oUnit.SetSection((int)dwTemp);
}

void CIssUnitInterface::SaveRegistry()
{
    int iTemp = m_oUnit.GetSection();//we'll put it back just in case we do this in the middle

    SetKey(REG_KEY, _T("UnitType"), (DWORD)m_oUnit.GetSection());
    SetKey(REG_KEY, _T("UnitCount"), (DWORD)m_oUnit.GetSectionCount());

    for(int i = 0; i < m_oUnit.GetSectionCount(); i++)
    {
        //m_oUnit.SetSection(i);
        DWORD dwTemp;

        TCHAR szRegKey[STRING_NORMAL];
        m_oStr->Format(szRegKey, _T("UnitSectionTo%i"), i);
        dwTemp = (DWORD)m_oUnit.GetToSelected(i);
        SetKey(REG_KEY, szRegKey, dwTemp);

        m_oStr->Format(szRegKey, _T("UnitSectionFrom%i"), i);
        dwTemp = (DWORD)m_oUnit.GetFromSelected(i);
        SetKey(REG_KEY, szRegKey, dwTemp);
    }
    m_oUnit.SetSection(iTemp);
}

BOOL CIssUnitInterface::DrawSelector(HDC hdc, RECT& rc)
{
    if(m_iSelector < 0 || m_iSelector >= UBTN_Count)
        return FALSE;

    RECT rcTemp = m_oUnitBtns[m_iSelector].GetButtonRect();

    int iRadius = GetSystemMetrics(SM_CXICON)/6;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(WHITE_PEN));
    //	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
    RoundRect(hdc, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    return TRUE;
}

BOOL CIssUnitInterface::HandleDPad(int iVKKey)
{
    switch(iVKKey)
    {
    case VK_RIGHT:
    case VK_DOWN:
        m_iSelector++;
        if(m_iSelector >= UBTN_Count)
            m_iSelector = 0;
        break;
    case VK_UP:
    case VK_LEFT:
        m_iSelector--;
        if(m_iSelector < 0)
            m_iSelector = UBTN_Category;
        break;
    case VK_RETURN:
        switch(m_iSelector)
        {
        case UBTN_From:
            OnBtnFrom();
            break;
        case UBTN_To:
            OnBtnTo();
            break;
        case UBTN_Category:
            OnBtnConversion();
            break;
        default:
            break;
        }
        break;
    default:
        return FALSE;
        break;
    }

    InvalidateRect(m_hWndDlg, NULL, FALSE);
    return TRUE;
}

BOOL CIssUnitInterface::ShowKBButtons()
{
	if(m_oBtnMan->GetKBType() != KB_Original)
		return TRUE;
	return FALSE;
#ifdef WIN32_PLATFORM_WFSP
	if(IsLandscape())
		return TRUE;
	if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
		return TRUE;
#endif
	return FALSE;
}

BOOL CIssUnitInterface::IsLandscape()
{
#ifdef UNDER_CE
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        return TRUE;
    else
        return FALSE;
#else
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

    if(WIDTH(rcClient) > HEIGHT(rcClient))
        return TRUE;
    else
        return FALSE;
#endif
}