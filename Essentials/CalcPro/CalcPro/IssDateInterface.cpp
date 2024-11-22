#include "IssDateInterface.h"
#include "IssRegistry.h"
#include "commctrl.h"
#include "IssLocalisation.h"


#define SET_BTN_SIZE (GetSystemMetrics(SM_CXICON)*6/7)
#define SET_BTN_WIDTH (GetSystemMetrics(SM_CXICON)*8/5)

CIssDateInterface::CIssDateInterface(void)
:m_hFontBtnText(NULL)
,m_hFontBtnTextSm(NULL)
,m_hFontTop(NULL)
,m_hFontControl(NULL)
,m_hWndDateFrom(NULL)
,m_hWndDateTo(NULL)
,m_hWndTimeFrom(NULL)
,m_hWndTimeTo(NULL)
,m_hWndDays(NULL)
,m_hWndHours(NULL)
,m_hWndMinutes(NULL)
,m_hWndSeconds(NULL)
,m_hFontLabel(NULL)
,m_iLabelSize(DISPLAY_TOP_TEXT_SM)
{
    m_bLand = FALSE;

    if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN))
        m_bLand = TRUE;

    if(m_bLand == FALSE)
        m_iLabelSize = m_iLabelSize*4/3;

    LoadRegistry();
}

CIssDateInterface::~CIssDateInterface(void)
{
	//seriously ... you gotta check this cleanup ... think we've got some buttons to delete properly
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);
	CIssGDIEx::DeleteFont(m_hFontTop);
    CIssGDIEx::DeleteFont(m_hFontControl);

    SaveRegistry();
}

void CIssDateInterface::LoadRegistry()
{




}

void CIssDateInterface::SaveRegistry()
{

}

void CIssDateInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    if((m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_DateTime) ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
	    CreateControls();
	    CreateButtons();
        CreateMainButtons();
        m_oDate.ClearAll();
    }
}

BOOL CIssDateInterface::Draw(HDC hdc, RECT& rc)
{
	FillRect(hdc, rc, m_oBtnMan->GetSkin()->GetBackgroundColor());

	DrawDisplays(hdc, rc);
	DrawDisplayText(hdc, rc);

	DrawBtns(hdc, rc);
    //m_oMenu->DrawMenu(hdc, rc, _T("File"), _T("Menu"));

    return TRUE;
}

BOOL CIssDateInterface::DrawDisplays(HDC hdc, RECT& rcClip)
{
	Rectangle(hdc, m_rcInterval, GetSysColor(COLOR_WINDOW), 0);

#ifndef UNDER_CE
    RECT rc;
    POINT pt;
    
    GetWindowRect(m_hWndDateFrom, &rc);
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(m_hWndDlg, &pt);
    SetRect(&rc, pt.x, pt.y, pt.x + WIDTH(rc), pt.y + HEIGHT(rc));
    Rectangle(hdc, rc, GetSysColor(COLOR_WINDOW), 0);

    GetWindowRect(m_hWndDateTo, &rc);
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(m_hWndDlg, &pt);
    SetRect(&rc, pt.x, pt.y, pt.x + WIDTH(rc), pt.y + HEIGHT(rc));
    Rectangle(hdc, rc, GetSysColor(COLOR_WINDOW), 0);

    GetWindowRect(m_hWndTimeFrom, &rc);
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(m_hWndDlg, &pt);
    SetRect(&rc, pt.x, pt.y, pt.x + WIDTH(rc), pt.y + HEIGHT(rc));
    Rectangle(hdc, rc, GetSysColor(COLOR_WINDOW), 0);

    GetWindowRect(m_hWndTimeTo, &rc);
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(m_hWndDlg, &pt);
    SetRect(&rc, pt.x, pt.y, pt.x + WIDTH(rc), pt.y + HEIGHT(rc));
    Rectangle(hdc, rc, GetSysColor(COLOR_WINDOW), 0);
#endif
	return TRUE;
}

BOOL CIssDateInterface::DrawDisplayText(HDC hdc, RECT& rcClip)
{
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Days), m_hInst), m_rcDays, DT_CENTER | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Hours), m_hInst), m_rcHours, DT_CENTER | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Min), m_hInst), m_rcMin, DT_CENTER | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Sec), m_hInst), m_rcSec, DT_CENTER | DT_TOP, m_hFontTop, m_oBtnMan->GetSkin()->m_crTextTop);


    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Start), m_hInst), m_rcLabels[0], DT_LEFT | DT_TOP, m_hFontTop, 0x0FFFFFF);
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_Interval), m_hInst), m_rcLabels[1], DT_LEFT | DT_TOP, m_hFontTop, 0x0FFFFFF);
    DrawText(hdc, m_oStr->GetText(ID(IDS_DATE_End), m_hInst), m_rcLabels[2], DT_LEFT | DT_TOP, m_hFontTop, 0x0FFFFFF);

	return TRUE;
}

void CIssDateInterface::UpdateEnd()
{
    SYSTEMTIME sTime;

    //get to
    m_oDate.GetEndDate(&sTime);

    //set to
    DateTime_SetSystemtime(m_hWndDateTo, GDT_VALID, &sTime);
    DateTime_SetSystemtime(m_hWndTimeTo, GDT_VALID, &sTime);
}

void CIssDateInterface::UpdateStart()
{
    SYSTEMTIME sTime;

    //get to
    m_oDate.GetStartDate(&sTime);

    //set to
    DateTime_SetSystemtime(m_hWndDateFrom, GDT_VALID, &sTime);
    DateTime_SetSystemtime(m_hWndTimeFrom, GDT_VALID, &sTime);
}

void CIssDateInterface::UpdateInterval()
{
    SYSTEMTIME sTime;
    TCHAR szTemp[STRING_NORMAL];
    int iTemp;

    m_oDate.GetInverval(&sTime);

    //check for some extra years
    iTemp = sTime.wDay;

    if(sTime.wYear)
        iTemp += sTime.wYear*365;

    m_oStr->IntToString(szTemp, iTemp);
    SetWindowText(m_hWndDays, szTemp);

    m_oStr->IntToString(szTemp, sTime.wHour);
    SetWindowText(m_hWndHours, szTemp);

    m_oStr->IntToString(szTemp, sTime.wMinute);
    SetWindowText(m_hWndMinutes, szTemp);

    m_oStr->IntToString(szTemp, sTime.wSecond);
    SetWindowText(m_hWndSeconds, szTemp);
}

BOOL CIssDateInterface::DrawBtns(HDC hdc, RECT& rcClip)
{
    TCHAR* szText[3] = {m_oStr->GetText(ID(IDS_BTN_Calc), m_hInst), m_oStr->GetText(ID(IDS_BTN_Calc), m_hInst), m_oStr->GetText(ID(IDS_BTN_Calc), m_hInst)};

    for(int i = 0; i < (int)DD_Count; i++)
    {
        m_oDateBtns[i].Draw(m_hFontBtnTextSm, hdc, szText[i]);
    }



    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    TCHAR* szTemp = NULL;

    for(int i = 0; i < 12; i++)
    {
        if(m_bLand)
        {
            szTemp = sLayout->sAltButtons[i]->szLabel;
            if(m_oStr->GetLength(szTemp) > 2)
                m_oMainBtns[i].Draw(m_hFontBtnTextSm, hdc, szTemp);
            else
                m_oMainBtns[i].Draw(m_hFontBtnText, hdc, szTemp);

            //here we'll check for special cases
            //backspace
            int iType = sLayout->sAltButtons[i]->iSkinIndex;

            if(sLayout->sAltButtons[i]->iButtonType == INPUT_Clear &&
                sLayout->sAltButtons[i]->iButtonValue == CLEAR_BackSpace)
                DrawBackSpace(hdc, m_oMainBtns[i].GetButtonRect(),
                m_oBtnMan->GetSkin()->GetButton(iType)->TextColor,
                m_oBtnMan->GetSkin()->GetButton(iType)->TextColor2,
                m_oBtnMan->GetSkin()->m_bUseTextShadow);
        }
        else
        {
            szTemp = sLayout->sMainButtons[i]->szLabel;
            if(m_oStr->GetLength(szTemp) > 2)
                m_oMainBtns[i].Draw(m_hFontBtnTextSm, hdc, szTemp);
            else
                m_oMainBtns[i].Draw(m_hFontBtnText, hdc, szTemp);

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
        

    }
	return TRUE;
}



BOOL CIssDateInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//if we're displayed ... re-init
	if(m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_DateTime)
	{
        m_bLand = FALSE;

        if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN))
            m_bLand = TRUE;

		CIssGDIEx::DeleteFont(m_hFontTop);
        CIssGDIEx::DeleteFont(m_hFontControl);

        m_hFontTop = NULL;
        m_hFontControl = NULL;

        m_iLabelSize = DISPLAY_TOP_TEXT_SM;

        if(m_bLand == FALSE)
            m_iLabelSize = m_iLabelSize*4/3;

        if(m_hFontTop == NULL)
            m_hFontTop     = CIssGDIEx::CreateFont(m_iLabelSize, 400, TRUE);
        if(m_hFontControl == NULL)
            m_hFontControl = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*15/16, 400, TRUE);

        CreateControls();
        CreateButtons();
        CreateMainButtons();
    }

	if(m_hFontTop != NULL)
		CIssGDIEx::DeleteFont(m_hFontTop);
	m_hFontTop     = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT_SM, 400, TRUE);

	if(m_hFontControl != NULL)
		CIssGDIEx::DeleteFont(m_hFontControl);
	m_hFontControl = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*15/16, 400, TRUE);

#ifndef UNDER_CE
	//to set the fonts
	SendMessage(m_hWndDateFrom, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndDateTo, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndTimeFrom, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndTimeTo, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndDays, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndHours, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndMinutes, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
	SendMessage(m_hWndSeconds, WM_SETFONT, (WPARAM)m_hFontControl, NULL);

	InvalidateRect(m_hWndDlg, NULL, FALSE);
#endif


	return TRUE;
}

BOOL CIssDateInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
    for(int i = 0; i < (int)DD_Count; i++)
    {
        if(m_oDateBtns[i].OnLButtonDown(pt))
            return TRUE;
    }

    //main buttons
    for(int i = 0; i < m_oBtnMan->GetCurrentLayout(m_hInst)->sMainButtons.GetSize(); i++)
    {
        if(m_oMainBtns[i].OnLButtonDown(pt))
            return TRUE;
    }

	return UNHANDLED;
}

BOOL CIssDateInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oDateBtns[0].OnLButtonUp(pt))
    {
        OnButtonFrom();
        return TRUE;
    }

    if(m_oDateBtns[1].OnLButtonUp(pt))
    {
        OnButtonInt();
        return TRUE;
    }

    if(m_oDateBtns[2].OnLButtonUp(pt))
    {
        OnButtonTo();
        return TRUE;
    }

    //main buttons
    for(int i = 0; i < m_oBtnMan->GetCurrentLayout(m_hInst)->sMainButtons.GetSize(); i++)
    {
        if(m_oMainBtns[i].OnLButtonUp(pt))
        {
            //handle the buttons and do something cool
            HandleMainButtons(i);
            return TRUE;
        }
    }
	return UNHANDLED;
}

void CIssDateInterface::HandleMainButtons(int iBtn)
{
    if(iBtn < 0)
        return;
    if(iBtn > 25)
        return;

    ButtonType* sBtn = NULL;

    if(m_bLand)
        sBtn = m_oBtnMan->GetCurrentLayout(m_hInst)->sAltButtons[iBtn];
    else
        sBtn = m_oBtnMan->GetCurrentLayout(m_hInst)->sMainButtons[iBtn];

    if(sBtn == NULL)
        return;
        
    //there's only 12 ... if statements here we go
    if(sBtn->iButtonType == INPUT_Number)
    {
        UINT uKey = sBtn->iButtonValue - NUM_0 + 0x30;//that's 0 yo

        //who knows if this will work consistently ... sp??
        keybd_event(uKey, MapVirtualKey(uKey, 0), 0, 0);
        keybd_event(uKey, MapVirtualKey(uKey, 0), KEYEVENTF_KEYUP, 0);
    }
    else if(sBtn->iButtonType == INPUT_Clear &&
        sBtn->iButtonValue == CLEAR_BackSpace)
    {
        UINT uKey = VK_BACK;

        //who knows if this will work consistently ... sp??
        keybd_event(uKey, MapVirtualKey(uKey, 0), 0, 0);
        keybd_event(uKey, MapVirtualKey(uKey, 0), KEYEVENTF_KEYUP, 0);
    }
    else if(sBtn->iButtonType == INPUT_Clear &&
        sBtn->iButtonValue == CLEAR_Reset)
    {
        m_oDate.ClearAll();
        UpdateInterval();
        UpdateEnd();
        UpdateStart();
    }
}

void CIssDateInterface::CreateButtons()
{
	CIssGDIEx::DeleteFont(m_hFontBtnText);
	CIssGDIEx::DeleteFont(m_hFontBtnTextSm);

	CalcProSkins* oSkin = m_oBtnMan->GetSkin();

	CCalcDynBtn* oBtn = NULL;
	
    int iStyle = 1;

    //lets create the 3 date buttons and go from there ... 
    RECT rcDisplay;
    RECT rc;

    POINT ptOffset;
    ptOffset.x = 0;
    ptOffset.y = 0;

    ScreenToClient(m_hWndDlg, &ptOffset);

    //From
    GetWindowRect(m_hWndDateFrom, &rcDisplay);
    
    rc.left = DISPLAY_INDENT;
    rc.right = rcDisplay.left - DISPLAY_INDENT + ptOffset.x;
    rc.top = rcDisplay.top + ptOffset.y;
    rc.bottom = rcDisplay.bottom + ptOffset.y;

    m_oDateBtns[0].Destroy();
    m_oDateBtns[0].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
    m_oDateBtns[0].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
        oSkin->m_typeBtnClr[iStyle].ButtonColor2,
        oSkin->m_typeBtnClr[iStyle].OutlineColor1,
        oSkin->m_typeBtnClr[iStyle].OutlineColor2);

    //init the Button
    m_oDateBtns[0].Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);
    oBtn = &m_oDateBtns[0];//so we only create the background once ... saves mem
    

    //Interval
    rc.top = m_rcInterval.top;
    rc.bottom = m_rcInterval.bottom;

    m_oDateBtns[1].Destroy();
    m_oDateBtns[1].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
    m_oDateBtns[1].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
        oSkin->m_typeBtnClr[iStyle].ButtonColor2,
        oSkin->m_typeBtnClr[iStyle].OutlineColor1,
        oSkin->m_typeBtnClr[iStyle].OutlineColor2);

    //init the Button
    m_oDateBtns[1].Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);


    //To
    GetWindowRect(m_hWndDateTo, &rcDisplay);
    rc.top = rcDisplay.top + ptOffset.y;
    rc.bottom = rcDisplay.bottom + ptOffset.y;

    m_oDateBtns[2].Destroy();
    m_oDateBtns[2].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
    m_oDateBtns[2].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
        oSkin->m_typeBtnClr[iStyle].ButtonColor2,
        oSkin->m_typeBtnClr[iStyle].OutlineColor1,
        oSkin->m_typeBtnClr[iStyle].OutlineColor2);

    //init the Button
    m_oDateBtns[2].Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);

	int iTemp = min(WIDTH(rc), HEIGHT(rc));

	//and finally ... fonts
	if(m_hFontBtnText == FALSE)
		m_hFontBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
	if(m_hFontBtnTextSm == FALSE)
		m_hFontBtnTextSm = CIssGDIEx::CreateFont(DISPLAY_HEIGHT*2/5, FW_BOLD, TRUE);
}

void CIssDateInterface::CreateMainButtons()
{
    CCalcDynBtn* oBtn = NULL;

    CalcProSkins* oSkin = m_oBtnMan->GetSkin();

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

    int iRows = 0;
    int iColumns = 0;

    if(m_bLand)
    {
        iRows = sLayout->iAltBtnRows;
        iColumns = sLayout->iAltBtnColumns;
    }
    else
    {
        iRows = sLayout->iMainBtnRows;
        iColumns = sLayout->iMainBtnColumns;
    }

    oBtn = NULL;

    iButtonWidth		= (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - (iColumns - 1)*BTN_SPACING) / iColumns;
    iButtonHeight		= (HEIGHT(m_rcMainBtns) - BTN_SPACING - m_oMenu->GetMenuHeight() - (iRows  - 1)*BTN_SPACING) / iRows;

    if(iButtonHeight < 1 ||
        iButtonWidth < 1)
        return;

    int	iSkinIndex = 0;
    int iCurrentBtn = 0;


    for(int j = 0; j < iRows; j++)
    {
        for(int i = 0; i < iColumns; i++)
        {
            iCurrentBtn = iColumns*j+i;

            rcButtonSize.left	= BTN_EDGE_INDENT + i*(BTN_SPACING+iButtonWidth);
            rcButtonSize.top	= BTN_SPACING + j*(BTN_SPACING+iButtonHeight) + m_rcMainBtns.top;
            rcButtonSize.right	= rcButtonSize.left + iButtonWidth;
            rcButtonSize.bottom	= rcButtonSize.top + iButtonHeight;

            //get the skin index
            if(m_bLand)
                iSkinIndex = sLayout->sAltButtons[iCurrentBtn]->iSkinIndex;
            else
                iSkinIndex = sLayout->sMainButtons[iCurrentBtn]->iSkinIndex;

            /*CCalcDynBtn* oBtn = new CCalcDynBtn;
            if(oBtn == NULL)
                break;*/

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

void CIssDateInterface::CreateControls()
{
    INITCOMMONCONTROLSEX iccex;
    iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_DATE_CLASSES;

    InitCommonControlsEx(&iccex);

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP| WS_BORDER | DTS_RIGHTALIGN | DTS_SHORTDATECENTURYFORMAT;

    if(m_hWndDateFrom == NULL)
		m_hWndDateFrom = CreateWindowEx(NULL, _T("SysDateTimePick32"), NULL, dwStyle,32, 50, 104, 32, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndDateFrom, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
    
    if(m_hWndDateTo == NULL)
        m_hWndDateTo = CreateWindowEx(NULL, _T("SysDateTimePick32"), NULL, dwStyle,32, 150, 104, 32, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndDateTo, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
      
    dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP| WS_BORDER | DTS_RIGHTALIGN | DTS_TIMEFORMAT;
    
    if(m_hWndTimeFrom == NULL)
		m_hWndTimeFrom = CreateWindowEx(NULL, _T("SysDateTimePick32"), NULL, dwStyle,136, 50, 104, 32, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndTimeFrom, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
    
    if(m_hWndTimeTo == NULL)
        m_hWndTimeTo = CreateWindowEx(NULL, _T("SysDateTimePick32"), NULL, dwStyle,136, 150, 104, 32, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndTimeTo, WM_SETFONT, (WPARAM)m_hFontControl, NULL);

    if(m_hWndDays == NULL)//don't change these Kat ... "Edit" defines the window type
        m_hWndDays = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_MULTILINE | ES_CENTER, 0, 0, 0, 0, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndDays, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
   
    if(m_hWndHours == NULL)
        m_hWndHours = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_MULTILINE | ES_CENTER, 0, 0, 0, 0, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndHours, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
 
    if(m_hWndMinutes == NULL)
        m_hWndMinutes = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_MULTILINE | ES_CENTER, 0, 0, 0, 0, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndMinutes, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
 
    if(m_hWndSeconds == NULL)
        m_hWndSeconds = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_MULTILINE | ES_CENTER, 0, 0, 0, 0, m_hWndDlg, NULL, m_hInst, NULL);
    SendMessage(m_hWndSeconds, WM_SETFONT, (WPARAM)m_hFontControl, NULL);
 
    UpdateInterval();
    UpdateStart();
    UpdateEnd();

    ShowWindow(m_hWndDateFrom, SW_SHOW);
    ShowWindow(m_hWndDateTo, SW_SHOW);
    ShowWindow(m_hWndTimeFrom, SW_SHOW);
    ShowWindow(m_hWndTimeTo, SW_SHOW);

    ShowWindow(m_hWndDays, SW_SHOW);
    ShowWindow(m_hWndHours, SW_SHOW);
    ShowWindow(m_hWndMinutes, SW_SHOW);
    ShowWindow(m_hWndSeconds, SW_SHOW);

    SetControls();
}

void CIssDateInterface::SetControls()
{
    RECT rcClient;
    GetClientRect(m_hWndDlg, &rcClient);

#ifndef UNDER_CE
	m_iLabelSize = DISPLAY_TOP_TEXT_SM;
#endif

    int iButtonHeight = SET_BTN_SIZE;

    if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))
        iButtonHeight = iButtonHeight*5/4;

    RECT rcTemp;

    rcTemp.left = rcClient.left + DISPLAY_INDENT;
    rcTemp.top = rcClient.top + DISPLAY_INDENT;
    rcTemp.right = rcClient.right;
    rcTemp.bottom = rcTemp.top + m_iLabelSize;

    m_rcLabels[0] = rcTemp;

    rcTemp.left = rcClient.left + SET_BTN_WIDTH;
    rcTemp.top = rcTemp.bottom + DISPLAY_INDENT;
    rcTemp.right = WIDTH(rcClient)/2 + rcTemp.left/2;
    rcTemp.bottom = rcTemp.top + iButtonHeight;

    //I think we can do everything here ... nice and simple
    if(m_hWndDateFrom)
        MoveWindow(m_hWndDateFrom, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    rcTemp.left = rcTemp.right;
    rcTemp.right = rcClient.right - DISPLAY_INDENT;

    if(m_hWndTimeFrom)
        MoveWindow(m_hWndTimeFrom, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    //label 2
    rcTemp.left = rcClient.left + DISPLAY_INDENT;
    rcTemp.top = rcTemp.bottom + DISPLAY_INDENT;
    rcTemp.right = rcClient.right;
    rcTemp.bottom = rcTemp.top + m_iLabelSize;

    m_rcLabels[1] = rcTemp;

    rcTemp.left = rcClient.left + SET_BTN_WIDTH;
    rcTemp.right = rcClient.right - DISPLAY_INDENT;

    rcTemp.top = rcTemp.bottom + DISPLAY_INDENT;
    rcTemp.bottom = rcTemp.top + iButtonHeight;

    //the interval display
    m_rcInterval = rcTemp;

    //set the individual rects now
    m_rcDays.top = m_rcInterval.top;
    m_rcDays.bottom = m_rcInterval.bottom;
    m_rcDays.left = m_rcInterval.left;
    m_rcDays.right = m_rcDays.left + WIDTH(m_rcInterval)/4;
    rcTemp = m_rcDays;
    rcTemp.top += (HEIGHT(m_rcDays)-GetSystemMetrics(SM_CXSMICON))*7/8;
    rcTemp.left++;
    rcTemp.right--;
    rcTemp.bottom = rcTemp.top + GetSystemMetrics(SM_CXSMICON);
    MoveWindow(m_hWndDays, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    m_rcHours.top = m_rcInterval.top;
    m_rcHours.bottom = m_rcInterval.bottom;
    m_rcHours.left = m_rcDays.right;
    m_rcHours.right = m_rcHours.left + WIDTH(m_rcInterval)/4;
    rcTemp = m_rcHours;
    rcTemp.top += (HEIGHT(m_rcDays)-GetSystemMetrics(SM_CXSMICON))*7/8;
    rcTemp.left++;
    rcTemp.right--;
    rcTemp.bottom = rcTemp.top + GetSystemMetrics(SM_CXSMICON);
    MoveWindow(m_hWndHours, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    m_rcMin.top = m_rcInterval.top;
    m_rcMin.bottom = m_rcInterval.bottom;
    m_rcMin.left = m_rcHours.right;
    m_rcMin.right = m_rcMin.left + WIDTH(m_rcInterval)/4;
    rcTemp = m_rcMin;
    rcTemp.top += (HEIGHT(m_rcDays)-GetSystemMetrics(SM_CXSMICON))*7/8;
    rcTemp.left++;
    rcTemp.right--;
    rcTemp.bottom = rcTemp.top + GetSystemMetrics(SM_CXSMICON);
    MoveWindow(m_hWndMinutes, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    m_rcSec.top = m_rcInterval.top;
    m_rcSec.bottom = m_rcInterval.bottom;
    m_rcSec.left = m_rcMin.right;
    m_rcSec.right = m_rcSec.left + WIDTH(m_rcInterval)/4;
    rcTemp = m_rcSec;
    rcTemp.top += (HEIGHT(m_rcDays)-GetSystemMetrics(SM_CXSMICON))*7/8;
    rcTemp.left++;
    rcTemp.right--;
    rcTemp.bottom = rcTemp.top + GetSystemMetrics(SM_CXSMICON);
    MoveWindow(m_hWndSeconds, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);
    //end interval

    rcTemp.left = rcClient.left + DISPLAY_INDENT;
    rcTemp.top = m_rcInterval.bottom + DISPLAY_INDENT;
    rcTemp.right = rcClient.right;
    rcTemp.bottom = rcTemp.top + m_iLabelSize;

    m_rcLabels[2] = rcTemp;

    rcTemp.left = rcClient.left + SET_BTN_WIDTH;
    rcTemp.right = WIDTH(rcClient)/2 + rcTemp.left/2;
    rcTemp.top = rcTemp.bottom + DISPLAY_INDENT;
    rcTemp.bottom = rcTemp.top + iButtonHeight;
    if(m_hWndDateTo)
        MoveWindow(m_hWndDateTo, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    rcTemp.left = rcTemp.right;
    rcTemp.right = rcClient.right - DISPLAY_INDENT;

    if(m_hWndTimeTo)
        MoveWindow(m_hWndTimeTo, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    m_rcMainBtns = rcClient;
    m_rcMainBtns.top = rcTemp.bottom;//done

    if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))
    {
        m_rcMainBtns.top += DISPLAY_INDENT;
        m_rcMainBtns.bottom -= DISPLAY_INDENT;
    }

}

BOOL CIssDateInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iParam = LOWORD(wParam);

	return UNHANDLED;
}

void CIssDateInterface::LostFocus()
{
    //hide the damn children
    ShowWindow(m_hWndDateFrom, SW_HIDE);
    ShowWindow(m_hWndDateTo, SW_HIDE);
    ShowWindow(m_hWndTimeFrom, SW_HIDE);
    ShowWindow(m_hWndTimeTo, SW_HIDE);

    ShowWindow(m_hWndDays, SW_HIDE);
    ShowWindow(m_hWndHours, SW_HIDE);
    ShowWindow(m_hWndMinutes, SW_HIDE);
    ShowWindow(m_hWndSeconds, SW_HIDE);
}
                              

void CIssDateInterface::OnButtonFrom()
{
    SYSTEMTIME temp;
    SYSTEMTIME temp2;
    TCHAR szTemp[STRING_NORMAL];
    int iTemp;

    //set to
    DateTime_GetSystemtime(m_hWndDateTo, &temp);
    DateTime_GetSystemtime(m_hWndTimeTo, &temp2);

    temp.wHour = temp2.wHour;
    temp.wMinute = temp2.wMinute;
    temp.wSecond = temp2.wSecond;

    m_oDate.SetEndDate(&temp);

    //set interval
    memset(&temp, 0, sizeof(SYSTEMTIME));
    GetWindowText(m_hWndDays, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);

    while(iTemp > 32635)
    {
        temp.wYear++;
        iTemp -= 365;
    }
    temp.wDay = iTemp;

    GetWindowText(m_hWndHours, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wHour = iTemp;

    GetWindowText(m_hWndMinutes, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wMinute = iTemp;

    GetWindowText(m_hWndSeconds, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wSecond = iTemp;

    m_oDate.SetInterval(&temp);

    UpdateStart();
}

void CIssDateInterface::OnButtonTo()
{
    SYSTEMTIME temp;
    SYSTEMTIME temp2;
    TCHAR szTemp[STRING_NORMAL];
    int iTemp;

    //set from
    DateTime_GetSystemtime(m_hWndDateFrom, &temp);
    DateTime_GetSystemtime(m_hWndTimeFrom, &temp2);

    temp.wHour = temp2.wHour;
    temp.wMinute = temp2.wMinute;
    temp.wSecond = temp2.wSecond;

    m_oDate.SetStartDate(&temp);

    //set interval
    memset(&temp, 0, sizeof(SYSTEMTIME));
    GetWindowText(m_hWndDays, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);

    while(iTemp > 32635)
    {
        temp.wYear++;
        iTemp -= 365;
    }
    temp.wDay = iTemp;

    GetWindowText(m_hWndHours, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wHour = iTemp;

    GetWindowText(m_hWndMinutes, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wMinute = iTemp;

    GetWindowText(m_hWndSeconds, szTemp, STRING_NORMAL);
    iTemp = m_oStr->StringToInt(szTemp);
    temp.wSecond = iTemp;

    m_oDate.SetInterval(&temp);

    UpdateEnd();
}

void CIssDateInterface::OnButtonInt()
{
    SYSTEMTIME temp;
    SYSTEMTIME temp2;

    //set from
    DateTime_GetSystemtime(m_hWndDateFrom, &temp);
    DateTime_GetSystemtime(m_hWndTimeFrom, &temp2);

    temp.wHour = temp2.wHour;
    temp.wMinute = temp2.wMinute;
    temp.wSecond = temp2.wSecond;

    m_oDate.SetStartDate(&temp);

    //set to
    DateTime_GetSystemtime(m_hWndDateTo, &temp);
    DateTime_GetSystemtime(m_hWndTimeTo, &temp2);

    temp.wHour = temp2.wHour;
    temp.wMinute = temp2.wMinute;
    temp.wSecond = temp2.wSecond;

    m_oDate.SetEndDate(&temp);

    //solve and get the values on screen
    UpdateInterval();
}

//#endif