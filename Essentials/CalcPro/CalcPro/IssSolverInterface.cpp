#include "IssSolverInterface.h"
#include "globals.h"
#include "IssRegistry.h"
#include "IssLocalisation.h"

CIssSolverInterface::CIssSolverInterface(void)
:m_oStr(CIssString::Instance())
,m_hFontDisplayTop(NULL)
,m_hFontExp(NULL)
,m_hFontBtnText(NULL)
,m_hFontSmall(NULL)
,m_iCurrentGraph(0)
,m_bSelected(FALSE)
{
#ifdef DEBUG
    
 //   TCHAR szEq[STRING_MAX];
 //   TCHAR szVar[STRING_SMALL];

//    double dbGuess;
//    double dbAns = 0;

 //   m_oStr->StringCopy(szEq, _T("4)

 


    //lets test it out
 //   m_oSolver.Compute(szEq, szVar, dbGuess, dbAns);

    


#endif
    LoadRegistry();
}

CIssSolverInterface::~CIssSolverInterface(void)
{
  //  SaveCurrentGraph();

    SaveRegistry();

    CIssGDIEx::DeleteFont(m_hFontBtnText);
    CIssGDIEx::DeleteFont(m_hFontDisplayTop);
    CIssGDIEx::DeleteFont(m_hFontExp);
    CIssGDIEx::DeleteFont(m_hFontSmall);

    m_oStr = NULL;
}

void CIssSolverInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    if((m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Graphing) ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
        CreateDisplays();
        CreateButtons();

    }
}

BOOL CIssSolverInterface::Draw(HDC hdc, RECT& rcClip)
{
    DrawDisplay(hdc, rcClip);
    DrawButtons(hdc, rcClip);

    DrawDisplayText(hdc, rcClip);
    return FALSE;
}

BOOL CIssSolverInterface::DrawDisplay(HDC hdc, RECT& rcClip)
{
    m_oDisplay.Draw(hdc);

    return TRUE;
}

BOOL CIssSolverInterface::DrawButtons(HDC hdc, RECT& rcClip)
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

BOOL CIssSolverInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
        m_rcDisplay.bottom = DISPLAY_HEIGHT*4/3 - DISPLAY_INDENT;			//start the alt buttons under the display
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

BOOL CIssSolverInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcDisplay, pt) == FALSE)
        return UNHANDLED;

    return m_oFuncBtn.OnLButtonDown(pt);
}

BOOL CIssSolverInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_oFuncBtn.GetButtonRect(), pt))
        return TRUE;

    if(PtInRect(&m_rcDisplay, pt) == FALSE)
        return UNHANDLED;

    return UNHANDLED;
}

void CIssSolverInterface::DrawDisplayText(HDC hDC, RECT& rc)
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

void CIssSolverInterface::CreateButtons()
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

void CIssSolverInterface::CreateDisplays()
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


BOOL CIssSolverInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

void CIssSolverInterface::LoadRegistry()
{
//    TCHAR szKey[STRING_SMALL];
//    TCHAR szTemp[STRING_MAX];


    
}

void CIssSolverInterface::SaveRegistry()
{
//    TCHAR szKey[STRING_SMALL];

}

BOOL CIssSolverInterface::OnGraph()
{
    return TRUE;
}

BOOL CIssSolverInterface::HandleDPad(int iVKKey)
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
        return TRUE;
    }

    else if(m_bSelected == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CIssSolverInterface::DrawSelector(HDC hdc, RECT& rc)
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
