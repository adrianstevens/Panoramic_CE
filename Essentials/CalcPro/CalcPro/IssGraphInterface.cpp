#include "IssGraphInterface.h"
#include "globals.h"
#include "DlgGraph.h"
#include "IssRegistry.h"
#include "IssLocalisation.h"


CIssGraphInterface::CIssGraphInterface(void)
:m_oStr(CIssString::Instance())
,m_hFontDisplayTop(NULL)
,m_hFontExp(NULL)
,m_hFontBtnText(NULL)
,m_hFontSmall(NULL)
,m_iCurrentGraph(0)
,m_bSelected(FALSE)
{
    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
	{
        m_szFunctions[i] = NULL;
		m_szPolarFunctions[i] = NULL;
	}

    LoadRegistry();
}

CIssGraphInterface::~CIssGraphInterface(void)
{
  //  SaveCurrentGraph();

    SaveRegistry();

    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
    {
        m_oStr->Delete(&m_szFunctions[i]);
        m_oStr->Delete(&m_szPolarFunctions[i]);
    }

    CIssGDIEx::DeleteFont(m_hFontBtnText);
    CIssGDIEx::DeleteFont(m_hFontDisplayTop);
    CIssGDIEx::DeleteFont(m_hFontExp);
    CIssGDIEx::DeleteFont(m_hFontSmall);

 //   m_oFuncBtn.Destroy();

    m_oStr = NULL;
}

void CIssGraphInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    if((m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Graphing) ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
        CreateDisplays();
        CreateButtons();

        if(m_szFunctions[0] != NULL && m_iCurrentGraph == 0)
            m_oCalc->UpdateDisplays(_T(""), m_szFunctions[0]);
    }
}

BOOL CIssGraphInterface::Draw(HDC hdc, RECT& rcClip)
{
    DrawDisplay(hdc, rcClip);
    DrawButtons(hdc, rcClip);

    DrawDisplayText(hdc, rcClip);
    return FALSE;
}

BOOL CIssGraphInterface::DrawDisplay(HDC hdc, RECT& rcClip)
{
    m_oDisplay.Draw(hdc);

    return TRUE;
}

BOOL CIssGraphInterface::DrawButtons(HDC hdc, RECT& rcClip)
{
    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    TCHAR szLabel[STRING_SMALL];
    if(g_bGraphPolar)
        m_oStr->Format(szLabel, _T("f%i(θ)"), m_iCurrentGraph+1);
    else
        m_oStr->Format(szLabel, _T("f%i(x)"), m_iCurrentGraph+1);

    m_oFuncBtn.Draw(m_hFontBtnText, hdc, szLabel);
   
    DrawDownArrow(hdc, m_oFuncBtn.GetButtonRect(), 
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
        m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
        m_oBtnMan->GetSkin()->m_bUseTextShadow);
    return TRUE;
}

BOOL CIssGraphInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    m_rcDisplay.top = 0;
    m_rcDisplay.bottom = DISPLAY_HEIGHT;
    m_rcDisplay.left = rcClient.left;

    //set up the basic rects
    if(WIDTH(rcClient) > HEIGHT(rcClient))
    {
#ifndef WIN32_PLATFORM_WFSP
        m_rcDisplay.right = GetSystemMetrics(SM_CYSCREEN);
#else
        //also correct the height
        //m_rcDisplay.bottom = DISPLAY_HEIGHT*4/3 - DISPLAY_INDENT;			//start the alt buttons under the display
        m_rcDisplay.right = rcClient.right;    
#endif
    }
    else
    {
        m_rcDisplay.right = rcClient.right;    
    }

    //if we're displayed ... re-init
    if(m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Graphing)
    {
        CreateDisplays();
        CreateButtons();
    }

    //fonts
    CIssGDIEx::DeleteFont(m_hFontDisplayTop);
    CIssGDIEx::DeleteFont(m_hFontExp);

    m_hFontDisplayTop = CIssGDIEx::CreateFont(DISPLAY_TOP_TEXT_SM, 400, TRUE);
    m_hFontExp  = CIssGDIEx::CreateFont(EXPONENT_TEXT_SIZE, 400, TRUE);
    m_hFontSmall = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, 400, TRUE);

    return TRUE;
}

BOOL CIssGraphInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcDisplay, pt) == FALSE)
        return UNHANDLED;

    return m_oFuncBtn.OnLButtonDown(pt);
}

BOOL CIssGraphInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oFuncBtn.OnLButtonUp(pt))
        return OnFuncBtn(); 

    if(PtInRect(&m_oFuncBtn.GetButtonRect(), pt))
        return TRUE;

    if(PtInRect(&m_rcDisplay, pt) == FALSE)
        return UNHANDLED;

 //   if(PtInRect(&m_rcFunction, pt))
 //       g_bGraphPolar = !g_bGraphPolar;

    return UNHANDLED;
}

void CIssGraphInterface::DrawDisplayText(HDC hDC, RECT& rc)
{
    TCHAR szTemp[STRING_MAX];
    RECT rcTemp = m_oDisplay.GetPosition();
    rcTemp.left += DISPLAY_INDENT*2;
    rcTemp.right -= DISPLAY_INDENT*2;
    rcTemp.bottom -= DISPLAY_INDENT*2;//up by 2
    rcTemp.top += DISPLAY_INDENT;

    int iIndent = GetSystemMetrics(SM_CXICON);

    //draw the display text
    m_oCalc->GetEquation(szTemp);

    //check the width 
    SIZE sizeText;
    HFONT hOld = (HFONT)SelectObject(hDC, m_hFontExp);
    GetTextExtentPoint(hDC, szTemp, m_oStr->GetLength(szTemp), &sizeText);
    SelectObject(hDC, hOld);
    hOld = NULL;
   
    //too long
    if(sizeText.cx > WIDTH(rcTemp))
    {
        //reduce font and check again
        hOld = (HFONT)SelectObject(hDC, m_hFontSmall);
        GetTextExtentPoint(hDC, szTemp, m_oStr->GetLength(szTemp), &sizeText);
        SelectObject(hDC, hOld);

        if(sizeText.cx > WIDTH(rcTemp))
            DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_BOTTOM, m_hFontSmall, m_oBtnMan->GetSkin()->m_crText);
        else
            DrawText(hDC, szTemp, rcTemp, DT_LEFT | DT_BOTTOM, m_hFontSmall, m_oBtnMan->GetSkin()->m_crText);
    }
    else
    {    
        DrawText(hDC, szTemp, rcTemp, DT_LEFT | DT_BOTTOM, m_hFontExp, m_oBtnMan->GetSkin()->m_crText);
    }

    if(g_bGraphPolar)
        DrawText(hDC, _T("f(θ)="), rcTemp, DT_LEFT | DT_TOP, 
            m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);
    else
        DrawText(hDC, _T("f(x)="), rcTemp, DT_LEFT | DT_TOP, 
            m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);

    m_rcFunction = rcTemp;
    m_rcFunction.right = m_rcFunction.left + GetSystemMetrics(SM_CXICON)*2;

    //2nd / Hyp
    rcTemp.left += GetSystemMetrics(SM_CXICON);
    if(m_oCalc->GetCalcState() == CALCSTATE_2ndF ||
        m_oCalc->GetCalcState() == CALCSTATE_2ndF_Hyp)
    {
        m_oStr->StringCopy(szTemp, ID(IDS_BTN_SecondFunction), STRING_NORMAL, m_hInst);
        DrawText(hDC, szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);
    }

    rcTemp.left += iIndent;
    if(m_oCalc->GetCalcState() == CALCSTATE_Hyp ||
        m_oCalc->GetCalcState() == CALCSTATE_2ndF_Hyp)
    {
        m_oStr->StringCopy(szTemp, ID(IDS_BTN_Hyp), STRING_NORMAL, m_hInst);
        DrawText(hDC, szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);
    }

    rcTemp.left += iIndent;

    //DRG
    //DRG state
    if(m_oCalc->GetDRGState() == DRG_Degrees)
        m_oStr->StringCopy(szTemp, ID(IDS_BTN_Degrees), STRING_NORMAL, m_hInst);
    else if(m_oCalc->GetDRGState() == DRG_Radians)
        m_oStr->StringCopy(szTemp, ID(IDS_BTN_Radians), STRING_NORMAL, m_hInst);
    else
        m_oStr->StringCopy(szTemp, ID(IDS_BTN_Grad), STRING_NORMAL, m_hInst);
    DrawText(hDC, szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);

    //Memory
    rcTemp.left += iIndent;
    if(m_oCalc->IsMemoryFilled(0))
        DrawText(hDC, _T("M"), rcTemp, DT_LEFT | DT_TOP, m_hFontDisplayTop, m_oBtnMan->GetSkin()->m_crTextTop);



}

void CIssGraphInterface::CreateButtons()
{
   CalcProSkins* oSkin = m_oBtnMan->GetSkin();

    RECT rc;

    int iStyle = 4;

    rc = m_oDisplay.GetPosition();

    rc.left = rc.right + DISPLAY_INDENT;
    rc.right = rc.left + FUNC_BTN_WIDTH;

    //create the button based off of the display to keep things tidy
    m_oFuncBtn.Destroy();
    m_oFuncBtn.SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
    m_oFuncBtn.InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
        oSkin->m_typeBtnClr[iStyle].ButtonColor2,
        oSkin->m_typeBtnClr[iStyle].OutlineColor1,
        oSkin->m_typeBtnClr[iStyle].OutlineColor2);

    //init the Button
    m_oFuncBtn.Init(rc, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);

    //and finally ... fonts
    if(m_hFontBtnText == FALSE)
        m_hFontBtnText = CIssGDIEx::CreateFont(DISPLAY_HEIGHT*2/5, FW_BOLD, TRUE);


}

void CIssGraphInterface::CreateDisplays()
{
    CalcProSkins* oSkin = m_oBtnMan->GetSkin();//makes for tidier code
    if(oSkin == NULL)
        return;

    RECT rcTemp;

    rcTemp.top		= DISPLAY_INDENT;
	rcTemp.left		= DISPLAY_INDENT;
//	rcTemp.bottom	= rcTemp.top + DISPLAY_HEIGHT-2*DISPLAY_INDENT;
    rcTemp.bottom   = rcTemp.top + HEIGHT(m_rcDisplay) - 2*DISPLAY_INDENT;
	rcTemp.right	= m_rcDisplay.right - 2*DISPLAY_INDENT - FUNC_BTN_WIDTH;//make room for the button on the right

    if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)//ultra wide
        rcTemp.bottom = rcTemp.top + DISPLAY_TALL - 2*DISPLAY_INDENT;

    m_oDisplay.Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
        , oSkin->m_typeDisplay.ButtonColor2
        , oSkin->m_typeDisplay.OutlineColor1
        , oSkin->m_typeDisplay.OutlineColor2,
        DISPLAY_Grad_DS);
}

BOOL CIssGraphInterface::OnFuncBtn()
{
    SaveCurrentGraph();

    //pop menu 
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    TCHAR szText[STRING_MAX];

    //now add some graphs
    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
    {
        if(g_bGraphPolar)
            m_oStr->Format(szText, _T("f%i(θ) = "), i+1);
        else
            m_oStr->Format(szText, _T("f%i(x) = "), i+1);
        if(m_szFunctions[i]!= NULL)
            m_oStr->Concatenate(szText, m_szFunctions[i]);

        AppendMenu(hMenu, MF_STRING, i + IDMENU_Graph_Func, szText);
    }

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)m_iCurrentGraph, TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, NUMBER_OF_GRAPHS - 1, m_iCurrentGraph, MF_BYPOSITION);

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);
    pt.x	= m_oFuncBtn.GetButtonRect().right;
    pt.y	= m_oFuncBtn.GetButtonRect().bottom;

    ClientToScreen(m_hWndDlg, &pt);

    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

    DestroyMenu(hMenu);
    return TRUE;
}

BOOL CIssGraphInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam < IDMENU_Graph_Func || wParam > IDMENU_Graph_Func + NUMBER_OF_GRAPHS)
        return UNHANDLED;

    int iParam = LOWORD(wParam) - IDMENU_Graph_Func;

    ChangeGraph(iParam);

    return TRUE;
}

void CIssGraphInterface::SaveCurrentGraph()
{
    //we need to save the current graph
    m_oStr->Delete(&m_szFunctions[m_iCurrentGraph]);

    TCHAR szTemp[STRING_MAX*2];//just in case 
    m_oStr->Empty(szTemp);
    m_oCalc->GetEquation(szTemp);
    CheckEquation(szTemp);

	m_szFunctions[m_iCurrentGraph] = m_oStr->CreateAndCopy(szTemp);
}

void CIssGraphInterface::CheckEquation(TCHAR* szEquation)
{
    if(szEquation == NULL)
        return;

    int iIndex = 0; //brute force
    BOOL bOpp = FALSE;
    while(iIndex < m_oStr->GetLength(szEquation)-1)
    {
        if(szEquation[iIndex] == _T('X'))
        {   //make sure there's an operator (OR a bracket dumbass) following it ... otherwise insert a *
            //we're always in range
            bOpp = FALSE;
            if(szEquation[iIndex+1] == g_szBrackets[1][0])
                break;

            for(int i = 0; i < NUMBER_OF_Operators; i++)
            {
                if(szEquation[iIndex+1] == g_szOperators[i][0])
                {
                    bOpp = TRUE;
                    break;
                }
            }
            //also check for squared and cubed
            if(szEquation[iIndex+1] == _T('\x00B2') || szEquation[iIndex+1] == _T('\x00B3'))
                bOpp = TRUE;


            if(bOpp == FALSE)//insert the multi symbol
                m_oStr->Insert(szEquation, g_szOperators[OPP_Times], iIndex+1);
        }
        iIndex++;
    }



}

void CIssGraphInterface::ChangeGraph(int iGraph)
{
    if(iGraph < 0 || iGraph >= NUMBER_OF_GRAPHS)
        return;

    //we need to save the current graph
    SaveCurrentGraph();
    
    m_iCurrentGraph = iGraph;

    m_oCalc->AddClear(CLEAR_ClearAll);//gotta clear the equation

    if(m_szFunctions[m_iCurrentGraph] != NULL)
    {
        m_oCalc->UpdateDisplays(_T(""), m_szFunctions[m_iCurrentGraph]);
    }

	RECT rcTemp = m_rcDisplay;
	rcTemp.bottom *= 2;
	//causes a draw error if I don't extend the bottom
	InvalidateRect(m_hWndDlg, &rcTemp, FALSE);
}

TCHAR* CIssGraphInterface::GetFunction(int iIndex)
{
    if(iIndex < 0 || iIndex >= NUMBER_OF_GRAPHS)
        return NULL;

    return m_szFunctions[iIndex];
}

void CIssGraphInterface::LoadRegistry()
{
    TCHAR szKey[STRING_SMALL];
    TCHAR szTemp[STRING_MAX];

    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
    {
        m_oStr->Format(szKey, _T("Graph%i"), i);
        if(S_OK == GetKey(REG_KEY, szKey, szTemp, STRING_MAX))
        {
            m_oStr->Delete(&m_szFunctions[i]);         
            m_szFunctions[i] = m_oStr->CreateAndCopy(szTemp);
        }
    }
}

void CIssGraphInterface::SaveRegistry()
{
    TCHAR szKey[STRING_SMALL];

    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
    {
        if(m_szFunctions[i] != NULL)
        {
            m_oStr->Format(szKey, _T("Graph%i"), i);
            SetKey(REG_KEY, szKey, m_szFunctions[i], m_oStr->GetLength(m_szFunctions[i]));
        }
   }
}

BOOL CIssGraphInterface::OnGraph()
{
//	::SetCursor(LoadCursor(NULL, IDC_WAIT));

    SaveCurrentGraph();
#ifndef UNDER_CE
	EnableWindow(m_hWndDlg, FALSE);
	InvalidateRect(m_hWndDlg, NULL, FALSE);
#endif

    CDlgGraph oGraph;
    oGraph.SetFullScreen(m_oBtnMan->GetFullscreen());

	oGraph.SetMenu(m_oMenu, m_wndMenu);
    oGraph.UsePoints(g_bGraphPoints);
    oGraph.SetGraphAccuracy((int)g_bGraphFast);

    //its WAY too many calculations when the tape is enabled (oops)
    BOOL bUseTape = m_oCalc->GetUseTape();
    m_oCalc->SetUseTape(FALSE);

    for(int i = 0; i < NUMBER_OF_GRAPHS; i++)
    {
        if(m_szFunctions[i] != NULL)
            oGraph.SetGraph(m_szFunctions[i], i);
    }

//	SetCursor(NULL);

#ifdef UNDER_CE
    oGraph.DoModal(m_hWndDlg, m_hInst, IDD_DLG_BASIC);
#else
	RECT rc;
	GetWindowRect(m_hWndDlg, &rc);

	if(oGraph.Create(m_oStr->GetText(ID(IDS_MENU_CalcProGraph), m_hInst), NULL, m_hInst, m_oStr->GetText(ID(IDS_MENU_Graph), m_hInst),
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		0,
		DS_MODALFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX,
		NULL, IDI_CALCPRO))
	{
		MoveWindow(oGraph.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), TRUE);
		ShowWindow(oGraph.GetWnd(), SW_SHOW);
		ShowWindow(m_hWndDlg, SW_HIDE);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}

		ShowWindow(m_hWndDlg, SW_SHOW);
	}
	
	EnableWindow(m_hWndDlg, TRUE);
	InvalidateRect(m_hWndDlg, NULL, FALSE);
#endif
    m_oCalc->SetUseTape(bUseTape);
    return TRUE;
}

BOOL CIssGraphInterface::HandleDPad(int iVKKey)
{
    if(m_bSelected == FALSE && iVKKey == VK_UP)
    {
        m_bSelected = TRUE;
        return TRUE;
    }

    else if(m_bSelected == TRUE && iVKKey == VK_DOWN)
    {
        m_bSelected = FALSE;
        return TRUE;
    }

    else if(m_bSelected == TRUE && iVKKey == VK_RETURN)
    {
        OnFuncBtn();
        return TRUE;
    }

    else if(m_bSelected == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CIssGraphInterface::DrawSelector(HDC hdc, RECT& rc)
{
    if(m_bSelected == FALSE)
        return FALSE;


    RECT rcTemp = m_oFuncBtn.GetButtonRect();

    int iRadius = GetSystemMetrics(SM_CXICON)/6;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(WHITE_PEN));
    //	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
    RoundRect(hdc, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    return TRUE;



}
