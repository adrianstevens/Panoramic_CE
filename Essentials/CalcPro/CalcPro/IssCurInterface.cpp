#include "IssCurInterface.h"
#include "globals.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssLocalisation.h"

#define WM_INTERNET		WM_USER + 281

#define TIMER_TimeOut  10

CIssCurInterface::CIssCurInterface(void)
:m_oCalcDis(CIssFormatCalcDisplay::Instance())
,m_hFontBtnText(NULL)
,m_hFontBtnTextSm(NULL)
,m_hFontBtnTextLong(NULL)
,m_hFontDisplay(NULL)
,m_hFontExp(NULL)
,m_hFontTop(NULL)
,m_eBidAsk(CBA_Both)
,m_iSelector(0)
,m_bUpdating(FALSE)
{
//	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, 400, TRUE);

    m_oCur.Initialize();
    //load after the initialize so we we can set the to from
    LoadRegistry();

    m_eCDisplayState		= CDISPLAY_LastUpdated;
}

CIssCurInterface::~CIssCurInterface(void)
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

void CIssCurInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    if((m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Currency) ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
	    CreateDisplays();
	    CreateButtons();
    }
}

BOOL CIssCurInterface::Draw(CIssGDIEx& gdiMem, RECT& rc)
{
    HDC hdc = gdiMem.GetDC();

	FillRect(hdc, rc, m_oBtnMan->GetSkin()->GetBackgroundColor());

	DrawDisplays(hdc, rc);
	DrawDisplayText(hdc, rc);
	DrawCurBtns(hdc, rc);
	DrawMainBtns(gdiMem, rc);
    //m_oMenu->DrawMenu(hdc, rc, _T("File"), _T("Menu"));

	return TRUE;
}

BOOL CIssCurInterface::DrawDisplays(HDC hdc, RECT& rcClip)
{
	for(int i = 0; i < 3; i++)
	{
		m_oDisplay[i].Draw(hdc);
	}

	return TRUE;
}

BOOL CIssCurInterface::DrawDisplayText(HDC hdc, RECT& rcClip)
{
	static TCHAR szTemp[STRING_LARGE];
	static TCHAR szExp[STRING_NORMAL];
    static TCHAR szCurSym[STRING_SMALL];
    static TCHAR szFull[STRING_LARGE];
    static TCHAR szValue[STRING_LARGE];
    static TCHAR szBid[STRING_NORMAL];
    static TCHAR szAsk[STRING_NORMAL];
	static RECT rcTemp;

    if(m_bExpired)
    {
        rcTemp = m_oDisplay[CDIS_From].GetPosition();
        DrawText(hdc, m_oStr->GetText(ID(IDS_MSG_TrialExpired), m_hInst), rcTemp, DT_CENTER | DT_VCENTER, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
        return TRUE;
    }

	//draw the conversion type and the units
	//From
	rcTemp = m_oDisplay[CDIS_From].GetPosition();
	rcTemp.top += DISPLAY_INDENT;
	rcTemp.left += 2*DISPLAY_INDENT;
	rcTemp.right -= 2*DISPLAY_INDENT;

    m_oCur.GetCurrency(m_oCur.GetFrom(), szExp, NULL, szCurSym);
    DrawText(hdc, szExp, rcTemp, DT_LEFT | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

    ////// READ IN FROM VALUES FROM CALC CLASS ////////////////////////////////
    m_oCalc->GetAnswer(szFull, szValue, szExp, FALSE, FALSE);

    ////// convert using regional settings ///////////
    m_oCalcDis->CurrencyFormat(szFull, 
        szCurSym,
        NULL,
        szValue,
        szExp,
        FALSE,
        m_oCalc->GetDisplayCharacters() - 3);

    if(m_oStr->GetLength(szExp) == 0)
    {
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
    }
    else
    { //exponents and string
        rcTemp.right -= GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);

        rcTemp.top -= DISPLAY_INDENT/2;
        
        rcTemp.right += GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szExp, rcTemp, DT_RIGHT | DT_TOP, m_hFontExp, m_oBtnMan->GetSkin()->m_crText);
    }
    

	//To
	rcTemp = m_oDisplay[CDIS_To].GetPosition();
	rcTemp.top += DISPLAY_INDENT;
	rcTemp.left += 2*DISPLAY_INDENT;
	rcTemp.right -= 2*DISPLAY_INDENT;

    m_oCur.GetCurrency(m_oCur.GetTo(), szExp, NULL, szCurSym);
	DrawText(hdc, szExp, rcTemp, DT_LEFT | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

    ////// READ IN TO VALUES ///////////////////////////////////////////
    m_oCur.SetInput(szFull);
    m_oCur.GetResults(szFull, CUR_DISPLAY_FULL, szAsk, szBid);

    m_oStr->StringCopy(szValue, szFull);

    // convert the number value to the proper regional format
    m_oCalcDis->CurrencyFormat(szFull, 
        szCurSym,
        NULL,
        szValue,
        szExp,
        FALSE,
        m_oCalc->GetDisplayCharacters() - 3);


    if(m_oStr->GetLength(szExp) == 0)
    {
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);
    }
    else
    { //exponents and string
        rcTemp.right -= GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szValue, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crText);

        rcTemp.top -= DISPLAY_INDENT/2;

        rcTemp.right += GetSystemMetrics(SM_CXSMICON);
        DrawText(hdc, szExp, rcTemp, DT_RIGHT | DT_TOP, m_hFontExp, m_oBtnMan->GetSkin()->m_crText);
    }

    rcTemp = m_oDisplay[1].GetPosition();
    rcTemp.top += DISPLAY_INDENT;
    rcTemp.right -= 2*DISPLAY_INDENT;

    m_oCalcDis->CurrencyFormat(szAsk, 
        _T(""),
        szAsk,
        szValue,
        NULL,
        FALSE,
        m_oCalc->GetDisplayCharacters() - 3);

    //so we can change on the fly
    EnumCurrencyBidAsk eBidAsk = m_eBidAsk;

    if(m_oStr->GetLength(szAsk) > 8)
        eBidAsk = CBA_Bid;

    m_oCalcDis->CurrencyFormat(szBid, 
        _T(""),
        szBid,
        szValue,
        NULL,
        FALSE,
        m_oCalc->GetDisplayCharacters() - 3);

    if(m_oStr->GetLength(szBid) > 8)
    {
        if(eBidAsk == CBA_Bid)
            eBidAsk = CBA_None;
        else
            eBidAsk = CBA_Ask;
    }

    //now draw the buy/sell amounts ... no exp formatting
    rcTemp.left += GetSystemMetrics(SM_CXICON);

#ifdef WIN32_PLATFORM_WFSP
    eBidAsk = CBA_None;//lazy but effective
#endif


    switch(eBidAsk)
    {
    default:
    case CBA_Both:
        m_oStr->Format(szFull, m_oStr->GetText(ID(IDS_INFO_BidAsk), m_hInst), szBid, szAsk);
    	break;
    case CBA_Ask:
        m_oStr->Format(szFull, m_oStr->GetText(ID(IDS_INFO_Ask), m_hInst), szAsk);
    	break;
    case CBA_Bid:
        m_oStr->Format(szFull, m_oStr->GetText(ID(IDS_INFO_Bid), m_hInst), szBid);
        break;
    case CBA_None:
        m_oStr->Empty(szFull);
        break;
    }
    
    DrawText(hdc, szFull, rcTemp, DT_RIGHT | DT_TOP | DT_END_ELLIPSIS, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

    DWORD dwAlignLrg = DT_CENTER | DT_BOTTOM;
    DWORD dwAlignSm = DT_CENTER | DT_TOP;

#ifdef WIN32_PLATFORM_WFSP   
    dwAlignLrg = DT_RIGHT | DT_BOTTOM;
    dwAlignSm = DT_LEFT | DT_TOP;
#endif

    rcTemp = m_oDisplay[CDIS_Update].GetPosition();
    rcTemp.left += 2*DISPLAY_INDENT;
    rcTemp.right -= 2*DISPLAY_INDENT;


    switch(m_eCDisplayState)
    {
    case CDISPLAY_Downloading:
      //  DrawText(hdc, _T("Updating"), m_oDisplay[CDIS_Update].GetPosition(), DT_CENTER | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
        DrawText(hdc, m_oStr->GetText(ID(IDS_MSG_Connecting), m_hInst), rcTemp, dwAlignSm, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    	break;
    case CDISPLAY_Error:
        DrawText(hdc, m_oStr->GetText(ID(IDS_ERROR_Error), m_hInst), rcTemp, dwAlignLrg, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
        DrawText(hdc, m_oStr->GetText(ID(IDS_ERROR_UnableToConnect), m_hInst), rcTemp, dwAlignSm, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    	break;
    case CDISPLAY_LastUpdated:
    default:
        //last update
        DrawText(hdc, m_oCur.GetLastUpdatedDate(), rcTemp, dwAlignLrg, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);

        m_oStr->Format(szTemp, m_oStr->GetText(ID(IDS_MSG_Updated), m_hInst), m_oCur.GetLastUpdatedTime());
        DrawText(hdc, szTemp, rcTemp, dwAlignSm, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

        break;
    }

  	return TRUE;
}

BOOL CIssCurInterface::DrawCurBtns(HDC hdc, RECT& rcClip)
{
    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szTemp, ID(IDS_BTN_From), STRING_NORMAL, m_hInst);
    TCHAR szTemp2[STRING_NORMAL];
    m_oStr->StringCopy(szTemp2, ID(IDS_BTN_To), STRING_NORMAL, m_hInst);
    TCHAR szTemp3[STRING_NORMAL];
    m_oStr->StringCopy(szTemp3, ID(IDS_BTN_Update), STRING_NORMAL, m_hInst);

    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();
	
    m_oCurBtns[0].Draw(m_hFontBtnTextSm, hdc, szTemp);
    m_oCurBtns[1].Draw(m_hFontBtnTextSm, hdc, szTemp2);

    if(m_oStr->GetLength(szTemp3) > 10)//thanks spanish
        m_oCurBtns[2].Draw(m_hFontTop, hdc, szTemp3);
    else
        m_oCurBtns[2].Draw(m_hFontBtnTextSm, hdc, szTemp3);

    DrawDownArrow(hdc, m_oCurBtns[0].GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);

    DrawDownArrow(hdc, m_oCurBtns[1].GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);

    return TRUE;
}

BOOL CIssCurInterface::DrawKeyboardBtns(CIssGDIEx& gdiMem, RECT& rcClip)
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


BOOL CIssCurInterface::DrawMainBtns(CIssGDIEx& gdiMem, RECT& rcClip)
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

#ifdef WIN32_PLATFORM_WFSP //draw the extra labels yo
        //we're going to do a bit of a gross hack ... if we're on landscape we're not going to draw the main buttons
        if(m_oBtnHand->GetDeviceType() == DEVTYPE_SPLand)
            continue;

        else if(m_oBtnHand->GetDeviceType() == DEVTYPE_SP)
        {
            m_oMainBtns[i].Draw(m_hFontBtnTextSm, hdc, szText, NULL, sLayout->sMainAltButtons[i]->szLabel, NULL, m_hFontTop);
        }
        else
#endif
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



BOOL CIssCurInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
    if(IsLandscape())
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
	if(m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Currency)
	{
		CreateDisplays();
		CreateButtons();
	}

    //fonts
    CIssGDIEx::DeleteFont(m_hFontDisplay);
    CIssGDIEx::DeleteFont(m_hFontExp);

    m_iDisplayChars = 15;

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
#ifdef WIN32_PLATFORM_WFSP
    m_hFontTop     = CIssGDIEx::CreateFont( (GetSysMets(SM_CXICON)*11/32), 400, TRUE);
#else    
	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, 400, TRUE);
#endif
		
	return TRUE;
}

BOOL CIssCurInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
	for(int i = 0; i < CBTN_Count; i++)
		if(m_oCurBtns[i].OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &m_oCurBtns[i].GetButtonRect(), FALSE);		
			return TRUE;
		}

	for(int i = 0; i < NUM_KB_BUTTONS; i++)
		if(m_oMainBtns[i].OnLButtonDown(pt))
		{
			InvalidateRect(hWnd, &m_oMainBtns[i].GetButtonRect(), FALSE);	
			return TRUE;
		}

   /* if(PtInRect(&m_oDisplay[CDIS_From].GetPosition(), pt) || PtInRect(&m_oDisplay[CDIS_To].GetPosition(), pt))
    { 
        TCHAR szTemp[STRING_LARGE];
        m_oCalc->GetAnswer(szTemp);
        m_oUnit.GetResults(szTemp, szTemp);
        m_oCalc->AddString(szTemp);
        m_oCalc->AddEquals(EQUALS_Equals);
        m_oUnit.Switch();
        InvalidateRect(hWnd, &m_rcDisplay, FALSE);
    }
    else if(PtInRect(&m_oDisplay[CDIS_Category].GetPosition(), pt))
    {
        int iTemp = m_oUnit.GetSection();
        iTemp++;
        if(iTemp >= m_oUnit.GetSectionCount())
            iTemp = 0;
        m_oUnit.SetSection(iTemp);
        InvalidateRect(hWnd, &m_rcDisplay, FALSE);
    }*/

	return UNHANDLED;
}

BOOL CIssCurInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
	for(int i = 0; i < CBTN_Count; i++)
		if(m_oCurBtns[i].OnLButtonUp(pt))
		{
			switch(i)
			{
			case CBTN_Update:
				OnBtnUpdate();
				break;
			case CBTN_From:
				OnBtnFrom();
				break;
			case CBTN_To:
				OnBtnTo();
			    break;
			default:
			    break;
			}
            InvalidateRect(hWnd, &m_oCurBtns[i].GetButtonRect(), FALSE);	
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
			    //and play the sound
			    //PlaySounds(_T("IDR_WAVE_Click"));
            }
			return TRUE;
		}
    }

    if(PtInRect(&m_oDisplay[0].GetPosition(), pt) || PtInRect(&m_oDisplay[1].GetPosition(), pt))
    {
        TCHAR szTemp[STRING_LARGE];
        m_oCur.GetResults(szTemp, CUR_DISPLAY_FULL, NULL, NULL);
        m_oCur.Switch();
        m_oCalc->AddString(szTemp);
        InvalidateRect(m_hWndDlg, NULL, FALSE);
    }

	return UNHANDLED;
}


void CIssCurInterface::CreateKBButtons()
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
      //  iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
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

         //   iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
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

       // iSkinIndex = sLayout->sKeyboardButtons[iCurrentBtn]->iSkinIndex;
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


void CIssCurInterface::CreateButtons()
{
    if(ShowKBButtons())
        CreateKBButtons();

	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);

	CalcProSkins* oSkin = m_oBtnMan->GetSkin();

	CCalcDynBtn* oBtn = NULL;
	
	RECT rc;

    int iStyle = 4;

	//we'll start Cur converter buttons
	for(int i = 0; i < 3; i++)
	{
		rc = m_oDisplay[i].GetPosition();

		rc.left = rc.right + DISPLAY_INDENT;
		rc.right = rc.left + UNIT_BTN_WIDTH;

		if(i == (int)CBTN_Update)
		{
			rc.right += (UNIT_BTN_WIDTH_LRG - UNIT_BTN_WIDTH);
			oBtn = NULL;
            iStyle = 1;
		}

		//create the buttons based off of the displays to keep things tidy
		m_oCurBtns[i].Destroy();
		m_oCurBtns[i].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
		m_oCurBtns[i].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
			oSkin->m_typeBtnClr[iStyle].ButtonColor2,
			oSkin->m_typeBtnClr[iStyle].OutlineColor1,
			oSkin->m_typeBtnClr[iStyle].OutlineColor2);

		//init the Button
		m_oCurBtns[i].Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);
		
		oBtn = &m_oCurBtns[0];//so we only create the background once ... saves mem
	}


    if(ShowKBButtons())
    {
        CreateKBButtons();
        return;
    }

	//MAIN BUTTONS

	//lets use the basic 20 from main for now
	//based on our m_rcSize;
    int iButtonWidth	= 0;
    int iButtonHeight	= 0;
    int iTopIndent      = 0;
    int iBtnSpaceH      = 1;
    int iBtnSpaceV      = 1;
	RECT rcButtonSize	= {0,0,0,0};

	LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

	if(sLayout == NULL ||
		sLayout->iMainBtnColumns == 0 ||
		sLayout->iMainBtnRows == 0)
		return;

	oBtn = NULL;

    iButtonWidth		= (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - (sLayout->iMainBtnColumns - 1)*BTN_SPACING) / sLayout->iMainBtnColumns;
    if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
    {
        iButtonHeight		= (HEIGHT(m_rcMainBtns) - m_oMenu->GetMenuHeight() - (sLayout->iMainBtnRows - 1)*iBtnSpaceV) / sLayout->iMainBtnRows;
        iBtnSpaceH          = BTN_SPACING;
        iBtnSpaceV          = 1;
    }
    else
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

			//CCalcDynBtn* oBtn = new CCalcDynBtn;
			//if(oBtn == NULL)
			//	break;

			//just a helper function so we're not writing the sames code over and over ...
			//destroys the old, sets the colors, checks for a previous GDI, creates the button and saves the GDI if necessary
			//we can't assume these buttons are being created in order .... hmmmm
			//		InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);

			//		m_arrMainButtons.AddElement(oBtn);

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

BOOL CIssCurInterface::IsLandscape()
{
	RECT rcClient;
	GetClientRect(m_hWndDlg, &rcClient);
#ifdef UNDER_CE
	if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
		return TRUE;
	else
		return FALSE;
#else
	if(WIDTH(rcClient) > HEIGHT(rcClient))
		return TRUE;
	else
		return FALSE;
#endif
}

void CIssCurInterface::CreateDisplays()
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

	m_oDisplay[CDIS_From].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
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

	m_oDisplay[CDIS_To].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);

    //CONVERSION
	rcTemp.top		= rcTemp.bottom + DISPLAY_INDENT;
	rcTemp.left		= DISPLAY_INDENT;
	rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;

    rcTemp.right	= rcClient.right - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH_LRG;//make room for the button on the right

	m_oDisplay[CDIS_Update].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);
}

BOOL CIssCurInterface::OnBtnFrom()
{
    if(!m_wndMenu)
        return FALSE;

    //so our previously used currencies work
    if(m_bUpdating)
    {
        LoadRegistry();
        m_bUpdating = FALSE;
    }

    TCHAR szText[STRING_LARGE];
    TCHAR szSymbol[STRING_SMALL];
    TCHAR szName[STRING_LARGE];
    int i;
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(m_oStr->GetText(ID(IDS_MENU_Recent), m_hInst));
    for(i=0; i<m_oCur.GetNumFavorites(); i++)
    {
        m_oCur.GetFavorites(i, szSymbol, szName, NULL);
        m_oStr->Format(szText, _T("(%s) %s"), szSymbol, szName);
        m_wndMenu->AddItem(szText, IDMENU_Cur_From + m_oCur.GetFavoriteIndex(i), NULL);
    }

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(m_oStr->GetText(ID(IDS_MENU_FullList), m_hInst));
    for(i = 0; i < m_oCur.GetCurrencyCount(); i++)
    {
        m_oCur.GetCurrency(i, szSymbol, szName, NULL);
        m_oStr->Format(szText, _T("(%s) %s"), szSymbol, szName);
        m_wndMenu->AddItem(szText, IDMENU_Cur_From + i, NULL);
    }

    m_wndMenu->SetSelectedItemIndex((m_oCur.GetNumFavorites()?1:0), TRUE);

    RECT rcBtn = m_oCurBtns[CBTN_From].GetButtonRect();
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu->GetMenuHeight();

    m_wndMenu->PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);

	/*HMENU hMenu = CreatePopupMenu();
	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];

	for(int i = 0; i < m_oCur.GetCurrencyCount(); i++)
	{
        m_oCur.GetCurrency(i, NULL, szText, NULL);
		AppendMenu(hMenu, MF_STRING, i + IDMENU_Cur_From, szText);
	}

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_oCur.GetFrom(), TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, m_oCur.GetCurrencyCount() - 1, m_oCur.GetFrom(), MF_BYPOSITION);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWndDlg, &rc);
	pt.x	= m_oCurBtns[CBTN_From].GetButtonRect().right;
	pt.y	= m_oCurBtns[CBTN_From].GetButtonRect().bottom;

	ClientToScreen(m_hWndDlg, &pt);

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

	DestroyMenu(hMenu);*/
	return TRUE;
}

BOOL CIssCurInterface::OnBtnTo()
{
    if(!m_wndMenu)
        return FALSE;

    if(m_bUpdating)
    {
        LoadRegistry();
        m_bUpdating = FALSE;
    }

    TCHAR szText[STRING_LARGE];
    TCHAR szSymbol[STRING_SMALL];
    TCHAR szName[STRING_LARGE];

    int i;
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(m_oStr->GetText(ID(IDS_MENU_Recent), m_hInst));
    for(i=0; i<m_oCur.GetNumFavorites(); i++)
    {
        m_oCur.GetFavorites(i, szSymbol, szName, NULL);
        m_oStr->Format(szText, _T("(%s) %s"), szSymbol, szName);
        m_wndMenu->AddItem(szText, IDMENU_Cur_To + m_oCur.GetFavoriteIndex(i), NULL);
    }

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(m_oStr->GetText(ID(IDS_MENU_FullList), m_hInst));
    for(i = 0; i < m_oCur.GetCurrencyCount(); i++)
    {
        m_oCur.GetCurrency(i, szSymbol, szName, NULL);
        m_oStr->Format(szText, _T("(%s) %s"), szSymbol, szName);
        m_wndMenu->AddItem(szText, IDMENU_Cur_To + i, NULL);
    }

    m_wndMenu->SetSelectedItemIndex((m_oCur.GetNumFavorites()?1:0), TRUE);

    RECT rcBtn = m_oCurBtns[CBTN_To].GetButtonRect();
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);

    POINT ptBtnScreen;
    ClientToScreen(m_hWndDlg, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= m_oMenu->GetMenuHeight();

    m_wndMenu->PopupMenu(m_hWndDlg, m_hInst,
        OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn),
        ADJUST_Bottom);

    /*HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];

    for(int i = 0; i < m_oCur.GetCurrencyCount(); i++)
    {
        m_oCur.GetCurrency(i, NULL, szText, NULL);
        AppendMenu(hMenu, MF_STRING, i + IDMENU_Cur_To, szText);
    }

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_oCur.GetTo(), TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, m_oCur.GetCurrencyCount() - 1, m_oCur.GetTo(), MF_BYPOSITION);

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);
    pt.x	= m_oCurBtns[CBTN_To].GetButtonRect().right;
    pt.y	= m_oCurBtns[CBTN_To].GetButtonRect().bottom;

    ClientToScreen(m_hWndDlg, &pt);

    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

    DestroyMenu(hMenu);*/

    //just in case
    SaveRegistry();
    return TRUE;
}

BOOL CIssCurInterface::OnBtnUpdate()
{
    SaveRegistry();
    DownloadCurrency();
    m_bUpdating = TRUE;
    InvalidateRect(m_hWndDlg, &m_oDisplay[2].GetPosition(), FALSE);
	return TRUE;
}

BOOL CIssCurInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iParam = LOWORD(wParam);

    if(iParam >= IDMENU_Cur_From && iParam < IDMENU_Cur_From + 200)
	{	
        m_oCur.SetFrom((int)wParam - IDMENU_Cur_From);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else if(iParam >= IDMENU_Cur_To && iParam < IDMENU_Cur_To + 200)
	{
        m_oCur.SetTo((int)wParam - IDMENU_Cur_To);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else
    {
		return FALSE;
    }

	return TRUE;
}

BOOL CIssCurInterface::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
    return UNHANDLED;
}

void CIssCurInterface::LoadRegistry()
{
    DWORD dwTemp = 3;

    if(S_OK == GetKey(REG_KEY, _T("CurFrom"), dwTemp))
        m_oCur.SetFrom((int)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("CurTo"), dwTemp))
        m_oCur.SetTo((int)dwTemp);

    TCHAR szTemp[STRING_SMALL];

    TCHAR szAbv0[STRING_SMALL] = _T("EUR");
    TCHAR szAbv1[STRING_SMALL] = _T("USD");
    TCHAR szAbv2[STRING_SMALL] = _T("JPY");
    TCHAR szAbv3[STRING_SMALL] = _T("GBP");
    TCHAR szAbv4[STRING_SMALL] = _T("CHF");
    TCHAR szAbv5[STRING_SMALL] = _T("CAD");

    GetKey(REG_KEY, _T("CF0"), szAbv0, dwTemp);
    GetKey(REG_KEY, _T("CF1"), szAbv1, dwTemp);
    GetKey(REG_KEY, _T("CF2"), szAbv2, dwTemp);
    GetKey(REG_KEY, _T("CF3"), szAbv3, dwTemp);
    GetKey(REG_KEY, _T("CF4"), szAbv4, dwTemp);
    GetKey(REG_KEY, _T("CF5"), szAbv5, dwTemp);


    for(int i = 0; i < m_oCur.GetCurrencyCount(); i++)
    {
        m_oCur.GetCurrency(i, szTemp, NULL, NULL);

        if(m_oStr->Compare(szTemp, szAbv0) == 0)
            m_oCur.SetLastUsedIndex(0, i);
        if(m_oStr->Compare(szTemp, szAbv1) == 0)
            m_oCur.SetLastUsedIndex(1, i);
        if(m_oStr->Compare(szTemp, szAbv2) == 0)
            m_oCur.SetLastUsedIndex(2, i);
        if(m_oStr->Compare(szTemp, szAbv3) == 0)
            m_oCur.SetLastUsedIndex(3, i);
        if(m_oStr->Compare(szTemp, szAbv4) == 0)
            m_oCur.SetLastUsedIndex(4, i);
        if(m_oStr->Compare(szTemp, szAbv5) == 0)
            m_oCur.SetLastUsedIndex(5, i);
    }
}

void CIssCurInterface::SaveRegistry()
{
    //save the too and from currencies
    //save the currency count
    SetKey(REG_KEY, _T("CurFrom"), (DWORD)m_oCur.GetFrom());
    SetKey(REG_KEY, _T("CurTo"), (DWORD)m_oCur.GetTo());

    TCHAR szTemp[STRING_NORMAL];
    DWORD dwTemp = 0;
    for(int i = 0; i < m_oCur.GetNumFavorites(); i++)
    {   
        m_oStr->Format(szTemp, _T("CF%i"), i);
        //new way via abbreviation 
        TCHAR szAbv[STRING_SMALL];
        m_oCur.GetCurrency(m_oCur.GetLastUsedIndex(i), szAbv, NULL, NULL);
        dwTemp = m_oStr->GetLength(szAbv);
        SetKey(REG_KEY, szTemp, szAbv, dwTemp);

        //old way via index
        //DWORD dwTemp = m_oCur.GetLastUsedIndex(i);
        //SetKey(REG_KEY, szTemp, dwTemp);
    }
}


BOOL CIssCurInterface::DownloadCurrency()
{
//#ifdef UNDER_CE
    if(m_oInternet.IsDownloading())
        return FALSE;

    // force a redraw here
    m_eCDisplayState = CDISPLAY_Downloading;
    InvalidateRect(m_hWndDlg, NULL, FALSE);
    UpdateWindow(m_hWndDlg);

    // this might take a while
    if(!EstablishConnection())
    {
        // keep trying

        //m_eCDisplayState = CDISPLAY_Error;
        //InvalidateRect(m_hWndDlg, NULL, FALSE);
        //return FALSE;
    }

    TCHAR szDownloadDir[STRING_MAX];
	TCHAR szURL[STRING_MAX];

    GetExeDirectory(szDownloadDir);
    m_oStr->Concatenate(szDownloadDir, _T("currency2.ini"));

    m_oInternet.Init(m_hWndDlg,
        WM_INTERNET,
#ifdef UNDER_CE
        TRUE, //crashes on PC ...
#else
        FALSE,
#endif
        TRUE,
        FALSE,
        TRUE);

	m_oStr->Empty(szURL);

	// get the proper language
	if(g_cLocale.GetCurrentLanguage() == LANG_ENGLISH)
	{
		m_oStr->StringCopy(szURL, _T("http://www.panoramicsoft.com/mobileapps/calcpro/currency.ini"));
	}
	else
	{
		g_cLocale.GetLanguageCode(szURL);
		m_oStr->Insert(szURL, _T("http://www.panoramicsoft.com/mobileapps/calcpro/currency_"));
		m_oStr->Concatenate(szURL, _T(".ini"));
	}

    if(m_oInternet.DownloadFile(szURL, szDownloadDir, NULL, NULL))
    {
        m_eCDisplayState = CDISPLAY_Downloading;
    }
    else
    {
        m_eCDisplayState = CDISPLAY_Error;
    }

    InvalidateRect(m_hWndDlg, NULL, FALSE);
//#endif
    return TRUE;
}

BOOL CIssCurInterface::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_INTERNET)
    {
        switch(wParam)
        {	
        case INET_CON_DOWNLOAD_COMPLETE:
        {
            TCHAR szOldFile[STRING_MAX];
            TCHAR szNewFile[STRING_MAX];

            GetExeDirectory(szOldFile);
            m_oStr->Concatenate(szOldFile, _T("currency.ini"));
            GetExeDirectory(szNewFile);
            m_oStr->Concatenate(szNewFile, _T("currency2.ini"));
            SetFileAttributes(szOldFile, FILE_ATTRIBUTE_NORMAL);
            DeleteFile(szOldFile);
            MoveFile(szNewFile, szOldFile);

            // reinitialize the structure
            m_oCur.Initialize();
            break;
        }
        case INET_CON_CREATED:
        case INET_CON_GET_SUCCESS:
        case INET_CON_WRITTEN_BYTES:
            return TRUE;
            break;
        default:
            m_eCDisplayState = CDISPLAY_Error;
            break;
        }

        InvalidateRect(hWnd, NULL, FALSE);
        // Start a Timer so we can go back to the normal date display		
        SetTimer(hWnd, TIMER_TimeOut, 4000, NULL);
        return TRUE;

    }

    return UNHANDLED;
}

BOOL CIssCurInterface::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    KillTimer(hWnd, TIMER_TimeOut);
    m_eCDisplayState = CDISPLAY_LastUpdated;
    InvalidateRect(m_hWndDlg, NULL, FALSE);

    return TRUE;
}

BOOL CIssCurInterface::EstablishConnection()
{
#ifdef UNDER_CE
    TCHAR szPostURL[] = _T("http://www.google.com");

    // quick check to see if we can access the internet
    if(!SUCCEEDED(m_oConnection.IsAvailable(szPostURL, TRUE)))
    {
        // we didn't succeed so let's force a connection
        if(!SUCCEEDED(m_oConnection.AttemptConnect(szPostURL, TRUE)))
        {
            return FALSE;
        }

        // wait up to 10 seconds
        int iCount =0;
        while(!SUCCEEDED(m_oConnection.IsAvailable(szPostURL, TRUE)))
        {
            iCount++;
            Sleep(1000);

            if(iCount>=10)
            {
                return FALSE;
            }
        }
    }
#endif
    return TRUE;
}

BOOL CIssCurInterface::DrawSelector(HDC hdc, RECT& rc)
{
    if(m_iSelector < 0 || m_iSelector >= CBTN_Count)
        return FALSE;

    RECT rcTemp = m_oCurBtns[m_iSelector].GetButtonRect();

    int iRadius = GetSystemMetrics(SM_CXICON)/6;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(WHITE_PEN));
    //	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
    RoundRect(hdc, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    return TRUE;
}

BOOL CIssCurInterface::HandleDPad(int iVKKey)
{
    switch(iVKKey)
    {
    case VK_RIGHT:
    case VK_DOWN:
        m_iSelector++;
        if(m_iSelector >= CBTN_Count)
            m_iSelector = 0;
    	break;
    case VK_LEFT:
    case VK_UP:
        m_iSelector--;
        if(m_iSelector < 0)
            m_iSelector = CBTN_Update;
        break;
    case VK_RETURN:
        switch(m_iSelector)
        {
        case CBTN_From:
            OnBtnFrom();
        	break;
        case CBTN_To:
            OnBtnTo();
        	break;
        case CBTN_Update:
            OnBtnUpdate();
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

BOOL CIssCurInterface::ShowKBButtons()
{
	if(m_oBtnMan->GetKBType() != KB_Original)
		return TRUE;
	return FALSE;
#ifdef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        return TRUE;
	if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
		return TRUE;
#endif
    return FALSE;
}
