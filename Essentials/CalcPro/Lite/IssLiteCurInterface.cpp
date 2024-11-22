#include "IssLiteCurInterface.h"
#include "globals.h"
#include "IssRegistry.h"
#include "IssCommon.h"

#define WM_INTERNET		WM_USER + 281

#define TIMER_TimeOut  10

CIssCurInterface::CIssCurInterface(void)
:m_oCalcDis(CIssFormatCalcDisplay::Instance())
,m_hFontBtnText(NULL)
,m_hFontBtnTextSm(NULL)
,m_hFontDisplay(NULL)
,m_hFontExp(NULL)
,m_hFontTop(NULL)
,m_eBidAsk(CBA_Both)
,m_iSelector(0)
{
	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT, 400, TRUE);

    m_oCur.Initialize();
    //load after the initialize so we we can set the to from
    LoadRegistry();
}

CIssCurInterface::~CIssCurInterface(void)
{
	//seriously ... you gotta check this cleanup ... think we've got some buttons to delete properly
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);
	CIssGDIEx::DeleteFont(m_hFontDisplay);
	CIssGDIEx::DeleteFont(m_hFontExp);
	CIssGDIEx::DeleteFont(m_hFontTop);

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

BOOL CIssCurInterface::Draw(HDC hdc, RECT& rc)
{
	FillRect(hdc, rc, m_oBtnMan->GetSkin()->GetBackgroundColor());

	DrawDisplays(hdc, rc);
	DrawDisplayText(hdc, rc);
	DrawCurBtns(hdc, rc);
	DrawMainBtns(hdc, rc);
    //m_oMenu->DrawMenu(hdc, rc, _T("File"), _T("Menu"));

	return TRUE;
}

BOOL CIssCurInterface::DrawDisplays(HDC hdc, RECT& rcClip)
{
	for(int i = 0; i < 2; i++)
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

    m_oCalcDis->CurrencyFormat(szBid, 
        _T(""),
        szBid,
        szValue,
        NULL,
        FALSE,
        m_oCalc->GetDisplayCharacters() - 3);

    //now draw the buy/sell amounts ... no exp formatting
    rcTemp.left += GetSystemMetrics(SM_CXICON);

    switch(m_eBidAsk)
    {
    default:
    case CBA_Both:
        m_oStr->Format(szFull, _T("bid:%s ask:%s"), szBid, szAsk);
    	break;
    case CBA_Ask:
        m_oStr->Format(szFull, _T("ask:%s"), szAsk);
    	break;
    case CBA_Bid:
        m_oStr->Format(szFull, _T("bid:%s"), szBid);
        break;
    }
    
    DrawText(hdc, szFull,rcTemp, DT_RIGHT | DT_TOP | DT_END_ELLIPSIS, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);

 	return TRUE;
}

BOOL CIssCurInterface::DrawCurBtns(HDC hdc, RECT& rcClip)
{
	TCHAR* szText[3] = {_T("From"), _T("To"), _T("Update")};

    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();
	
	for(int i = 0; i < (int)CBTN_Count; i++)
	{
		m_oCurBtns[i].Draw(m_hFontBtnTextSm, hdc, szText[i]);
	}

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

BOOL CIssCurInterface::DrawMainBtns(HDC hdc, RECT& rcClip)
{
#ifdef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        return TRUE;
#endif
	LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

	TCHAR* szText = NULL;

    HFONT hFontTemp = NULL;

    int iCount = sLayout->iMainBtnColumns*sLayout->iMainBtnRows;

	for(int i = 0; i < iCount; i++)
	{
		szText = sLayout->sMainButtons[i]->szLabel;
        hFontTemp = m_hFontBtnText;

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
    if((GetSystemMetrics(SM_CYSCREEN)%400) == 0)
    {   //ultra tall
        m_rcDisplay.bottom = 2*(DISPLAY_HEIGHT+2*DISPLAY_INDENT);//yeah yeah ... its just how it works out .. trust me
    }
    else
        m_rcDisplay.bottom = 2*(DISPLAY_HEIGHT-DISPLAY_INDENT);//yeah yeah ... its just how it works out .. trust me
	
	
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


    m_hFontDisplay = CIssGDIEx::CreateFont(DISPLAY_TEXT_SIZE, 400, TRUE);
    m_hFontExp     = CIssGDIEx::CreateFont(EXPONENT_TEXT_SIZE, 400, TRUE);

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

	for(int i = 0; i < NUM_MAIN_BUTTONS; i++)
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

	for(int i = 0; i < NUM_MAIN_BUTTONS; i++)
		if(m_oMainBtns[i].OnLButtonUp(pt))
		{
			InvalidateRect(hWnd, &m_oMainBtns[i].GetButtonRect(), FALSE);		
            InvalidateRect(hWnd, &m_rcDisplay, FALSE);		

			PostMessage(hWnd, WM_CALCENGINE, m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonType, 
				m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonValue);
			//and play the sound
			//PlaySounds(_T("IDR_WAVE_Click"));
			return TRUE;
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

void CIssCurInterface::CreateButtons()
{
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);

	CalcProSkins* oSkin = m_oBtnMan->GetSkin();

	CCalcDynBtn* oBtn = NULL;
	
	RECT rc;

    int iStyle = 4;

	//we'll start Cur converter buttons
	for(int i = 0; i < 2; i++)
	{
		rc = m_oDisplay[i].GetPosition();

		rc.left = rc.right + DISPLAY_INDENT;
		rc.right = rc.left + UNIT_BTN_WIDTH;

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

	//MAIN BUTTONS

	//lets use the basic 20 from main for now
	//based on our m_rcSize;
	int iButtonWidth	= 0;
	int iButtonHeight	= 0;
	RECT rcButtonSize	= {0,0,0,0};

	LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

	if(sLayout == NULL ||
		sLayout->iMainBtnColumns == 0 ||
		sLayout->iMainBtnRows == 0)
		return;

	oBtn = NULL;

	iButtonWidth		= (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - (sLayout->iMainBtnColumns - 1)*BTN_SPACING) / sLayout->iMainBtnColumns;
	iButtonHeight		= (HEIGHT(m_rcMainBtns) - BTN_SPACING - m_oMenu->GetMenuHeight() - (sLayout->iMainBtnRows - 1)*BTN_SPACING) / sLayout->iMainBtnRows;

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

			rcButtonSize.left	= BTN_EDGE_INDENT + i*(BTN_SPACING+iButtonWidth);
			rcButtonSize.top	= BTN_SPACING + j*(BTN_SPACING+iButtonHeight) + m_rcMainBtns.top;
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

	//and finally ... fonts
	if(m_hFontBtnText == FALSE)
		m_hFontBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
	if(m_hFontBtnTextSm == FALSE)
		m_hFontBtnTextSm = CIssGDIEx::CreateFont(DISPLAY_HEIGHT*2/5, FW_BOLD, TRUE);
	
}

void CIssCurInterface::CreateDisplays()
{
	CalcProSkins* oSkin = m_oBtnMan->GetSkin();//makes for tidier code

    int iDisplayHeight = DISPLAY_HEIGHT;

    if((GetSystemMetrics(SM_CYSCREEN)%400) == 0)
        iDisplayHeight += 3*DISPLAY_INDENT;

	RECT rcTemp;

    //FROM
	rcTemp.top		= DISPLAY_INDENT;
	rcTemp.left		= DISPLAY_INDENT;
	rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;
	rcTemp.right	= GetSystemMetrics(SM_CXSCREEN) - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	

	m_oDisplay[CDIS_From].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);

    //TO

	rcTemp.top		= rcTemp.bottom + DISPLAY_INDENT;
	rcTemp.left		= DISPLAY_INDENT;
	rcTemp.bottom	= rcTemp.top + iDisplayHeight-2*DISPLAY_INDENT;
	rcTemp.right	= GetSystemMetrics(SM_CXSCREEN) - 2*DISPLAY_INDENT - UNIT_BTN_WIDTH;//make room for the button on the right
	

	m_oDisplay[CDIS_To].Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
		, oSkin->m_typeDisplay.ButtonColor2
		, oSkin->m_typeDisplay.OutlineColor1
		, oSkin->m_typeDisplay.OutlineColor2,
		DISPLAY_Grad_DS);
}

BOOL CIssCurInterface::OnBtnFrom()
{
    if(!m_wndMenu)
        return FALSE;

    TCHAR szText[STRING_LARGE];
    TCHAR szSymbol[STRING_SMALL];
    TCHAR szName[STRING_LARGE];
    int i;
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(_T("Full list"));

    int iCount = m_oCur.GetCurrencyCount();
    if(iCount > 20)
        iCount = 20;

    for(i = 0; i < iCount; i++)
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
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList,
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

    TCHAR szText[STRING_LARGE];
    TCHAR szSymbol[STRING_SMALL];
    TCHAR szName[STRING_LARGE];

    int i;
    m_oStr->Empty(szText);

    m_wndMenu->ResetContent();

    if(m_oCur.GetNumFavorites())
        m_wndMenu->AddCategory(_T("Full list"));

    int iCount = m_oCur.GetCurrencyCount();
    if(iCount > 20)
        iCount = 20;

    for(i = 0; i < iCount; i++)
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
        OPTION_DrawScrollArrows|OPTION_AlwaysShowSelector|OPTION_CircularList,
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
    return TRUE;
}

BOOL CIssCurInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iParam = LOWORD(wParam);

    if(iParam >= IDMENU_Cur_From && iParam < IDMENU_Cur_From + 200)
	{	
        m_oCur.SetFrom(wParam - IDMENU_Cur_From);
		InvalidateRect(m_hWndDlg, &m_rcDisplay, FALSE);
	}
	else if(iParam >= IDMENU_Cur_To && iParam < IDMENU_Cur_To + 200)
	{
        m_oCur.SetTo(wParam - IDMENU_Cur_To);
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
    DWORD dwTemp;

    if(S_OK == GetKey(REG_KEY, _T("CurFrom"), dwTemp))
        m_oCur.SetFrom((int)dwTemp);

    if(S_OK == GetKey(REG_KEY, _T("CurTo"), dwTemp))
        m_oCur.SetTo((int)dwTemp);

    TCHAR szTemp[STRING_NORMAL];

    BOOL bFailed = FALSE;

    for(int i = 0; i < m_oCur.GetNumFavorites(); i++)
    {   
        m_oStr->Format(szTemp, _T("CurFav%i"), i);
        if(S_OK == GetKey(REG_KEY, szTemp, dwTemp))
            m_oCur.SetLastUsedIndex(i, dwTemp);
        else
        {
            bFailed = TRUE;
            break;
        }
    }

    if(bFailed)
    {
        for(int i = 0; i < m_oCur.GetCurrencyCount(); i++)
        {
            m_oCur.GetCurrency(i, szTemp, NULL, NULL);

            if(m_oStr->Compare(szTemp, _T("EUR")) == 0)
                m_oCur.SetLastUsedIndex(0, i);
            if(m_oStr->Compare(szTemp, _T("USD")) == 0)
                m_oCur.SetLastUsedIndex(1, i);
            if(m_oStr->Compare(szTemp, _T("JPY")) == 0)
                m_oCur.SetLastUsedIndex(2, i);
            if(m_oStr->Compare(szTemp, _T("GBP")) == 0)
                m_oCur.SetLastUsedIndex(3, i);
            if(m_oStr->Compare(szTemp, _T("CHF")) == 0)
                m_oCur.SetLastUsedIndex(4, i);
            if(m_oStr->Compare(szTemp, _T("CAD")) == 0)
                m_oCur.SetLastUsedIndex(5, i);
        }
    }
}

void CIssCurInterface::SaveRegistry()
{
    //save the too and from currencies
    //save the currency count
    SetKey(REG_KEY, _T("CurFrom"), (DWORD)m_oCur.GetFrom());
    SetKey(REG_KEY, _T("CurTo"), (DWORD)m_oCur.GetTo());

    TCHAR szTemp[STRING_NORMAL];
    for(int i = 0; i < m_oCur.GetNumFavorites(); i++)
    {   
        m_oStr->Format(szTemp, _T("CurFav%i"), i);
        
        DWORD dwTemp = m_oCur.GetLastUsedIndex(i);
        SetKey(REG_KEY, szTemp, dwTemp);
            
    }
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
            m_iSelector = CBTN_To;
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