/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSDynBtn.cpp 
// Abstract: Comprehensive Button Class with 9+ button styles. 
// 
/***************************************************************************************************************/ 

#include "windows.h"
#include "IssDynBtn.h"
#include "IssString.h"
#include "math.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIssDynBtn::CIssDynBtn():
 m_eBtnStyle(BTN_Flat)
,m_hParentWnd(NULL)
,m_szBtnChar(NULL)
,m_uiMessage(0)
,m_iBtnNum(0)
,m_bBtnDown(FALSE)
,m_crColor1(RGB(0,0,0))
,m_crColor2(RGB(0,0,0))
,m_crOutline1(RGB(0,0,0))
,m_crOutline2(RGB(0,0,0))
,m_pDynBtn(NULL)
,m_bUseShadow(FALSE)
,m_crTextColor(RGB(255,255,255))
,m_crTextShadow(RGB(0,0,0))
,m_bOldBtnDown(FALSE)
,m_bRadioBtn(FALSE)
,m_bDisabled(FALSE)
,m_gdiDisabled(NULL)
{
}

CIssDynBtn::~CIssDynBtn()
{
        Destroy();
}


/******************************************************************************
    Sets the Button Colors....otherwise they default to Grey
******************************************************************************/
HRESULT CIssDynBtn::InitAdvanced(COLORREF crGrad1, 
                                  COLORREF crGrad2, 
                                  COLORREF crOutline1, 
                                  COLORREF crOutline2,
                                  COLORREF crBackgroundOutline,
                                  INT     iBackgroundLineThickness,
                                  BOOL    bRadioButton  )
{
    m_crColor1                      = crGrad1;
    m_crColor2                      = crGrad2;
    m_crOutline1                    = crOutline1;
    m_crOutline2                    = crOutline2;
    m_crBackground                  = crBackgroundOutline;
    m_iBackgroundThickness          = iBackgroundLineThickness;
    m_bRadioBtn                     = bRadioButton;

    return S_OK;
}


/******************************************************************************
    This Function Initializes the button and actually calls GenerateBtn
    so all colors should be set first

    rcSize:         Guess
    szChar:         Button Text
    HWND:           Handle to the parent window that the messages will be sent too
    UINT:           The message that will be sent
    iBtnNm:         Gets sent with the message so the receiving class knows who sent it
    crColor:        Backup Color....overwritten 
    eBtnStyle:      Button Style...check the enum in the .h
    pDynBtn:        Pointer to another button class...we'll use this so we don't genereate the same button color/style twice

    ***NOTE

    You definitely want to call InitAdvanced() First to set the button colors

/******************************************************************************/
HRESULT CIssDynBtn::Init(RECT                   rcSize, 
                          TCHAR*                szChar, 
                          HWND                  hWnd, 
                          UINT                  uiMessage,
                          int                   iBtnNum,
                          COLORREF              crColor,
                          BtnStyleType          eBtnStyle,
                          CIssDynBtn*           pDynBtn)
{
    HRESULT hr = S_OK;

    memcpy(&m_rcBtnRect, &rcSize, sizeof(RECT));

    // Destroy the old info
    Destroy();

    SetString(szChar);           
//        return E_INVALIDARG;

    CHHRE(hWnd, _T("Parent Wnd"), E_INVALIDARG);
    
    // save these values
    m_eBtnStyle                     = eBtnStyle; 
    m_hParentWnd                    = hWnd;
    m_uiMessage                     = uiMessage;
    m_iBtnNum                       = iBtnNum;
    m_bBtnDown                      = FALSE;

    if(pDynBtn)// if we got a point to another class we use that generated button already
        m_pDynBtn = pDynBtn;
    else
    {
        BOOL bRet = GenerateBtn();// Generate a new button
        CBHR(bRet, _T("GenerateBtn"));
    }

Error:
    return hr;
}

/********************************************************************
Function        DisableButton

Arguments:      bDisabled - TRUE to disable

Returns:        BOOL - true if successful

Comments:       Greys it out and ignores input
*********************************************************************/
HRESULT CIssDynBtn::DisableButton(BOOL bDisabled)
{
    m_bDisabled = bDisabled; 
    return S_OK;
}

/********************************************************************
Function        ResetRadioButton

Arguments:      

Returns:        TRUE

Comments:       Basically just forces the button to an unselected state
*********************************************************************/
HRESULT CIssDynBtn::ResetRadioButton()
{
    m_bBtnDown = FALSE;
    return S_OK;
}

/************************************************************************
        Sets the Text Colors and if we're going to use a text Shadow                                                           
************************************************************************/
HRESULT CIssDynBtn::SetTextColors(COLORREF crPrimaryTextColor, COLORREF crShadowColor, BOOL bUseShadow /* = FALSE */)
{
    m_bUseShadow            = bUseShadow;
    if(m_bUseShadow)
    {
        m_crTextColor       = crShadowColor;
        m_crTextShadow      = crPrimaryTextColor;
    }
    else
    {
        m_crTextColor       = crPrimaryTextColor;
        m_crTextShadow      = crShadowColor;
    }
    
    return S_OK;
}


/************************************************************************
        CleanUp                                                           
************************************************************************/
HRESULT CIssDynBtn::Destroy()
{
    HRESULT hr = S_OK;

    m_gdiButton.Destroy();
    m_pDynBtn = NULL;

	CIssString* oStr = CIssString::Instance();
    
    // destroy the old string
	oStr->Delete(&m_szBtnChar);

    SAFE_DELETE(m_gdiDisabled);

    return hr;
}


/************************************************************************
        Set new text for the button...not sure what uses this                                                           
************************************************************************/
HRESULT CIssDynBtn::SetString(TCHAR* szChar)
{
    HRESULT hr = S_OK;

    CIssString* oStr = CIssString::Instance();
    
    CBHRE(oStr->IsEmpty(szChar) == FALSE, _T("empty string"), E_INVALIDARG);

    //SAFE_DELETE_ARRAY(m_szBtnChar);
	oStr->Delete(&m_szBtnChar);

    m_szBtnChar = oStr->CreateAndCopy(szChar);// create a new string

    CBHRE(oStr->IsEmpty(szChar) == FALSE, _T("failed to create string"), E_ABORT);


Error:
    return hr;
}


/************************************************************************
        Handles Button Down Calls                                                         
************************************************************************/
BOOL CIssDynBtn::OnLButtonDown(POINT& point)
{
    if(PtInRect(&m_rcBtnRect, point) && !m_bDisabled)
    {
        if(m_bRadioBtn)
        {
                // Post a Message to the parent window indicating what button was pressed
                PostMessage(m_hParentWnd, 
                            m_uiMessage,
                            (WPARAM)m_iBtnNum, 
                            0);     
                // switch the down state
                m_bBtnDown = !m_bBtnDown;

        }
        else
                m_bBtnDown = TRUE;
        InvalidateRect(m_hParentWnd,&m_rcBtnRect,FALSE);
        return TRUE;
    }
    return FALSE;
}


/************************************************************************
        Handles Button Up Calls                                                        
************************************************************************/
BOOL CIssDynBtn::OnLButtonUp(POINT& point)
{
    if(m_bBtnDown && !m_bRadioBtn && !m_bDisabled)
    {
        m_bBtnDown = FALSE;
        
        if(PtInRect(&m_rcBtnRect, point))
        {       // Post a Message to the parent window indicating what button was pressed
            PostMessage(m_hParentWnd, 
                        m_uiMessage,
                        (WPARAM)m_iBtnNum, 
                        0);                     

            InvalidateRect(m_hParentWnd, &m_rcBtnRect,FALSE);
            return TRUE;
        }
        InvalidateRect(m_hParentWnd, &m_rcBtnRect,FALSE);
    }
    return FALSE;
}

/************************************************************************
        Handles Button Mouse Move Calls                                                           
************************************************************************/
BOOL CIssDynBtn::OnMouseMove(POINT& point)
{
    if( m_bBtnDown && !PtInRect(&m_rcBtnRect, point) && !m_bRadioBtn && !m_bDisabled)
    {
        m_bBtnDown = FALSE;
        InvalidateRect(m_hParentWnd, &m_rcBtnRect,FALSE);
        return TRUE;
    }
    return FALSE;
}

/************************************************************************
        Returns the Button Rect (public)                                                         
************************************************************************/
HRESULT CIssDynBtn::GetButtonRect(RECT &rc)
{
    memcpy(&rc, &m_rcBtnRect, sizeof(RECT));        

    return S_OK;
}

RECT CIssDynBtn::GetButtonRect()
{
    return m_rcBtnRect;
}

/************************************************************************
        Moves the Button (public)                                                          
************************************************************************/
HRESULT CIssDynBtn::MoveButton(POINT pt)
{
    m_rcBtnRect.right       = m_rcBtnRect.right - m_rcBtnRect.left + pt.x;
    m_rcBtnRect.left        = pt.x;

    m_rcBtnRect.bottom      = m_rcBtnRect.bottom - m_rcBtnRect.top + pt.y;
    m_rcBtnRect.top         = pt.y;

    return S_OK;
}

/************************************************************************
        Returns the Button Location (public)                                                           
************************************************************************/
POINT CIssDynBtn::GetBtnLocation()
{
    POINT pt;
    pt.x = m_rcBtnRect.left;
    pt.y = m_rcBtnRect.top;

    return pt;
}

/************************************************************************
        Generates the Buttons after the variables have been initialized
        
        Will call the appropriate GenerateBtn function depending on 
        the button style
************************************************************************/

BOOL CIssDynBtn::GenerateBtn()
{
    int             iHeight = m_rcBtnRect.bottom - m_rcBtnRect.top;
    int             iWidth  = 2*(m_rcBtnRect.right - m_rcBtnRect.left);

    RECT rcButton;
    rcButton.left   = 0;
    rcButton.right  = iWidth;
    rcButton.top    = 0;
    rcButton.bottom = iHeight;

    // Init the GDI
    HDC dc  = GetDC(NULL);
    m_gdiButton.Create(dc, rcButton, FALSE, TRUE);
    ReleaseDC(NULL, dc);

    // Fill it Pink
    FillRect(m_gdiButton.GetDC(), rcButton, TRANSPARENT_COLOR);

            // create the button according to the button style
    switch(m_eBtnStyle) 
    {
    case BTN_Keyboard:
            return GenerateBtnKeyboard(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Keyboard2:
            return GenerateBtnKeyboard2(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Grad:
            return GenerateBtnGrad(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Bevel:
            return GenerateBtnBevel(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Ellipse:
            return GenerateBtnEllipse(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_HP12c:
            return GenerateBtnHP12c(m_gdiButton, iWidth, iHeight);
            break;
    default:
    case BTN_Flat:
            return GenerateBtnFlat(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Shine:
            return GenerateBtnShine(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Shine2:
            return GenerateBtnShine2(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Shine3:
            return GenerateBtnShine3(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Vista:
            return GenerateBtnVista(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Butter:
            return GenerateBtnButters(m_gdiButton, iWidth, iHeight);
            break;
    case BTN_Adrian:
            return GenerateBtnAdrian(m_gdiButton, iWidth, iHeight);
            break;
    }

    return FALSE;
}

/************************************************************************
        Generates The Basic Button
        It has an outline, one color for the update and a second color for 
                the downstate
************************************************************************/
BOOL CIssDynBtn::GenerateBtnFlat(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    RECT rcTemp;
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    Rectangle(oGDI.GetDC(), rcTemp, m_crColor1, m_crOutline1);
    
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    Rectangle(oGDI.GetDC(), rcTemp, m_crColor2, m_crOutline1);

    return TRUE;
}

/************************************************************************
        Generates The HP Style Button
        It has an outline, one color for the update and a second color for 
                the downstate
************************************************************************/
BOOL CIssDynBtn::GenerateBtnHP12c(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
#define ELLIPSE_RADIUS_LARGE    GetSystemMetrics(SM_CXSMICON)/2
#define THICKNESS               GetSystemMetrics(SM_CXSMICON)*3/16

    RECT rcTemp;
    //Up State
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crColor1, m_crColor1, ELLIPSE_RADIUS_LARGE, ELLIPSE_RADIUS_LARGE);

    rcTemp.left     = THICKNESS;
    rcTemp.right    = iWidth/2 - THICKNESS;
    rcTemp.top      = THICKNESS;
    rcTemp.bottom   = iHeight - THICKNESS;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crOutline2, m_crOutline2, (GetSystemMetrics(SM_CXSMICON)/4), (GetSystemMetrics(SM_CXSMICON)/4));

    rcTemp.top      = THICKNESS + 1;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crColor2, m_crColor2, (GetSystemMetrics(SM_CXSMICON)/4), (GetSystemMetrics(SM_CXSMICON)/4));

    rcTemp.bottom   = iHeight*2/3;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crOutline1, m_crOutline1, (GetSystemMetrics(SM_CXSMICON)/8), (GetSystemMetrics(SM_CXSMICON)/8));
    
    //DownState     
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crColor1, m_crColor1, ELLIPSE_RADIUS_LARGE, ELLIPSE_RADIUS_LARGE);

    rcTemp.left     = iWidth/2 + THICKNESS;
    rcTemp.right    = iWidth - THICKNESS;
    rcTemp.top      = THICKNESS + (GetSystemMetrics(SM_CXSMICON)/16);
    rcTemp.bottom   = iHeight - THICKNESS;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crOutline2, m_crOutline2, (GetSystemMetrics(SM_CXSMICON)/4), (GetSystemMetrics(SM_CXSMICON)/4));

    rcTemp.top      = THICKNESS + 1 + (GetSystemMetrics(SM_CXSMICON)/16);
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crColor2, m_crColor2, (GetSystemMetrics(SM_CXSMICON)/4), (GetSystemMetrics(SM_CXSMICON)/4));

    rcTemp.bottom   = iHeight*2/3 + (GetSystemMetrics(SM_CXSMICON)/16);
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crOutline1, m_crOutline1, (GetSystemMetrics(SM_CXSMICON)/8), (GetSystemMetrics(SM_CXSMICON)/8));
    
    return TRUE;
}


/************************************************************************
        Generates The HP Style Button
        It has an outline, one color for the update and a second color for 
                the downstate
************************************************************************/
BOOL CIssDynBtn::GenerateBtnAdrian(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    int iOffset = 0;
    if(1 != (GetSystemMetrics(SM_CXSMICON)/16))
        iOffset = 1;

    RECT rcTemp;
    //Up State
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    int iCurveLarge = GetSystemMetrics(SM_CXICON)/6;
    int iCurveSmall = GetSystemMetrics(SM_CXICON)/15;

    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurveLarge, iCurveLarge, m_crColor1, m_crColor2);

    rcTemp.left     = iCurveSmall - iOffset;
    rcTemp.right    = iWidth/2 - iCurveSmall/2;
    rcTemp.top      = iCurveSmall - iOffset;
    rcTemp.bottom   = (int)((double)iHeight/8.0*7.0);
    GradientRoundRectangle(oGDI, rcTemp, NULL_COLOR, iCurveSmall, iCurveSmall, m_crColor2, m_crColor1);

    //DownState     
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurveLarge, iCurveLarge, m_crColor1, m_crColor2);

    rcTemp.left     = iWidth/2 + iCurveSmall - iOffset;
    rcTemp.right    = iWidth - iCurveSmall/2;
    rcTemp.top      = iCurveSmall - iOffset + iCurveSmall/2;
    rcTemp.bottom   = (int)((double)iHeight/8.0*7.0) + iCurveSmall/2;
    GradientRoundRectangle(oGDI, rcTemp, NULL_COLOR, iCurveSmall, iCurveSmall, m_crColor2, m_crColor1);

    return TRUE;
}


/************************************************************************
        Generates The Basic Gradient Button
************************************************************************/
BOOL CIssDynBtn::GenerateBtnGrad(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    RECT rcTemp;
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    GradientFillRect(oGDI, rcTemp, m_crColor1, m_crColor2);
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);
    
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    GradientFillRect(oGDI, rcTemp, m_crColor2, m_crColor1);
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);

    return TRUE;
}


/************************************************************************
        Generates The Slightly less Shiny Button
************************************************************************/
BOOL CIssDynBtn::GenerateBtnShine2(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
   return GenerateBtnShine(oGDI, iWidth, iHeight, 0.0);
}

/************************************************************************
        Generates The Shine without the double border
************************************************************************/
BOOL CIssDynBtn::GenerateBtnShine3(CIssGDIEx& oGDI, int& iWidth, int& iHeight, double fPercent)
{
    COLORREF crTemp;

    RECT rcTemp;

    int iR, iG, iB;

    iR = GetRValue(m_crColor2) + (int)((255.0 - GetRValue(m_crColor2)) * fPercent); // 1 - %...so 80%
    iG = GetGValue(m_crColor2) + (int)((255.0 - GetGValue(m_crColor2)) * fPercent);
    iB = GetBValue(m_crColor2) + (int)((255.0 - GetBValue(m_crColor2)) * fPercent);

    crTemp = RGB(iR, iG, iB);

    //Up State
    SetRect(&rcTemp, 0, 0, iWidth/2, iHeight);
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);

 
    rcTemp.left     += 1;
    rcTemp.right    -= 1;
    rcTemp.top      += 1;
    rcTemp.bottom    = iHeight/2;
    GradientFillRect(oGDI, rcTemp, m_crColor1, crTemp);

    //Down State
    rcTemp.top      = iHeight/2;
    rcTemp.bottom   = iHeight - 1;
    FillRect(oGDI.GetDC(), rcTemp, m_crColor2);
    
    rcTemp.bottom   = iHeight;
    rcTemp.top      = 0;
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);

    rcTemp.left             += 1;
    rcTemp.right    -= 1;
    rcTemp.top              += 1;
    rcTemp.bottom   =  iHeight/2;
    GradientFillRect(oGDI, rcTemp, crTemp, m_crColor1);

    rcTemp.top              = iHeight/2;
    rcTemp.bottom   = iHeight - 2;
    FillRect(oGDI.GetDC(), rcTemp, m_crColor1);

    return TRUE;

}


/************************************************************************
        Generates The Shiny Gradient Button
************************************************************************/
BOOL CIssDynBtn::GenerateBtnShine(CIssGDIEx& oGDI, int& iWidth, int& iHeight, double fPercent)
{
    COLORREF crTemp;

    int iR, iG, iB;

    iR = GetRValue(m_crColor2) + (int)((255.0 - GetRValue(m_crColor2)) * fPercent); // 1 - %...so 80%
    iG = GetGValue(m_crColor2) + (int)((255.0 - GetGValue(m_crColor2)) * fPercent);
    iB = GetBValue(m_crColor2) + (int)((255.0 - GetBValue(m_crColor2)) * fPercent);

    crTemp = RGB(iR, iG, iB);

    if(m_crColor2 == m_crColor1)//stupid hack for "TrueLove"
    {
            iR = GetRValue(m_crColor2) + (int)((255.0 - GetRValue(m_crColor2)) * 0.75); // 1 - %...so 25%
            iG = GetGValue(m_crColor2) + (int)((255.0 - GetGValue(m_crColor2)) * 0.75);
            iB = GetBValue(m_crColor2) + (int)((255.0 - GetBValue(m_crColor2)) * 0.75);     
            m_crColor1 = RGB(iR, iG, iB);
    }

    RECT rcTemp;

    //Up State
    rcTemp.left             = 0;
    rcTemp.top              = 0;
    rcTemp.right    = iWidth;
    rcTemp.bottom   = iHeight;
    
    FillRect(oGDI.GetDC(), rcTemp, m_crOutline2);

    rcTemp.right    = iWidth/2;
    rcTemp.bottom   = iHeight;
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);

    rcTemp.left             += 2;
    rcTemp.right    -= 2;
    rcTemp.top              += 2;
    rcTemp.bottom    = iHeight/2;
    GradientFillRect(oGDI, rcTemp, m_crColor1, crTemp);

    //Down State
    rcTemp.top              = iHeight/2;
    rcTemp.bottom   = iHeight - 2;
    FillRect(oGDI.GetDC(), rcTemp, m_crColor2);
    
    rcTemp.bottom   = iHeight;
    rcTemp.top              = 0;
    rcTemp.left             = iWidth/2;
    rcTemp.right    = iWidth;
    Rectangle(oGDI.GetDC(), rcTemp, NULL_COLOR, m_crOutline1);

    rcTemp.left             += 2;
    rcTemp.right    -= 2;
    rcTemp.top              += 2;
    rcTemp.bottom   =  iHeight/2;
    GradientFillRect(oGDI, rcTemp, crTemp, m_crColor1);

    rcTemp.top              = iHeight/2;
    rcTemp.bottom   = iHeight - 2;
    FillRect(oGDI.GetDC(), rcTemp, m_crColor1);

    return TRUE;

        
}

/************************************************************************
        Generates The Bevel Gradient Button
************************************************************************/
BOOL CIssDynBtn::GenerateBtnBevel(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    RECT rcTemp;
    //Up State
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    int iCurve      = GetSystemMetrics(SM_CXICON)/6;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurve, iCurve, m_crColor1, m_crColor2);

    //DownState     
    rcTemp.left     = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurve, iCurve, m_crColor2, m_crColor1);

    return TRUE;
}

/************************************************************************
        Generates The Ellipse Gradient Button
************************************************************************/
BOOL CIssDynBtn::GenerateBtnEllipse(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
#define SHADOW_OFFSET (GetSystemMetrics(SM_CXSMICON)/16)
    int iSOffset = SHADOW_OFFSET;

    if(m_crOutline2 == RGB(255,0,255))
    {
            iSOffset = 0;
    }


    int iOffset = 0;
    if(1 != (GetSystemMetrics(SM_CXSMICON)/16))
            iOffset = 1;

    int iRadius = iHeight/2;

    RECT rcTemp;
    //Up State
    rcTemp.left             = 0;
    rcTemp.right    = iWidth/2 - iSOffset;
    rcTemp.top              = iSOffset;
    rcTemp.bottom   = iHeight;
    RoundRectangle(oGDI.GetDC(), rcTemp, m_crOutline2, m_crOutline2, iRadius, iRadius);

    rcTemp.left             = iSOffset;
    rcTemp.right    = iWidth/2;
    rcTemp.top              = 0;
    rcTemp.bottom   = iHeight - iSOffset;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iRadius, iRadius, m_crColor1, m_crColor2);

    //DownState     
    rcTemp.left             = iWidth/2 + iSOffset;
    rcTemp.right    = iWidth;
    rcTemp.top              = 0;
    rcTemp.bottom   = iHeight - iSOffset;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iRadius, iRadius, m_crColor2, m_crColor1);

    return TRUE;
}

/************************************************************************
        Generates The New Keyboard Button
***********************************************************************/
BOOL CIssDynBtn::GenerateBtnKeyboard(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
#define BORDER_RATIO 10
    int iBorderWidth;
    
    //find the edge thickness
    if(iWidth/2 < iHeight)
            iBorderWidth = iWidth/2/BORDER_RATIO;
    else
            iBorderWidth = iHeight/BORDER_RATIO;

    //FillRect with the first color
    RECT rcTemp;
    rcTemp.bottom = iHeight;
    rcTemp.top    = 0;
    rcTemp.left       = 0;
    rcTemp.right  = iWidth;

    //Fill Everything
    FillRect(oGDI.GetDC(), rcTemp, m_crColor1);
    //adjust Rect and angle fill
    rcTemp.left = iBorderWidth;
    rcTemp.right = iWidth/2 - iBorderWidth;
    rcTemp.top = iBorderWidth-1;
    rcTemp.bottom = iHeight - iBorderWidth;
    GradientAngleFillRect(oGDI, rcTemp, m_crColor1, m_crColor2);
    //and the down state
    rcTemp.top = iBorderWidth;
    rcTemp.left = iWidth/2 + iBorderWidth;
    rcTemp.right = iWidth - iBorderWidth;
    rcTemp.bottom = iHeight - iBorderWidth + 1;
    GradientAngleFillRect(oGDI, rcTemp, m_crColor2, m_crColor1);

    //pens
    HPEN hPen1 = CIssGDIEx::CreatePen(m_crColor1);
    HPEN hPen2 = CIssGDIEx::CreatePen(m_crColor2);
    HPEN hPenOld = oGDI.GetPen();

    //Draw the angle outline
    POINT pt[2];

    oGDI.SetPen(hPen2);

    for(int i = 0; i < iBorderWidth; i++)
    {
        //left up state 
        pt[0].x = i;
        pt[1].x = i;
        pt[0].y = 0;
        pt[1].y = iHeight - i;

        Polyline(oGDI.GetDC(), pt, 2);

        //top upstate
        pt[0].x = 0;
        pt[1].x = iWidth/2 - 1 - i;
        pt[0].y = i;
        pt[1].y = i;

        Polyline(oGDI.GetDC(), pt, 2);

        //right downstate
        pt[0].x = iWidth - i -1;
        pt[1].x = iWidth - i -1;
        pt[0].y = i;
        pt[1].y = iHeight;

        Polyline(oGDI.GetDC(), pt, 2);

        //bottom downstate
        pt[0].x = iWidth/2+1+i;
        pt[1].x = iWidth;
        pt[0].y = iHeight - i -1;
        pt[1].y = iHeight - i -1;

        Polyline(oGDI.GetDC(), pt, 2);
    }

    oGDI.SetPen(hPenOld);
    DeleteObject(hPen1);
    DeleteObject(hPen2);

    return TRUE;
}

/********************************************************************
Function        GenerateBtnKeyboard2

Arguments:      

Returns:        TRUE

Comments:       Create the keyboard2 button style
*********************************************************************/
BOOL CIssDynBtn::GenerateBtnKeyboard2(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    int iOffset = 0;
    if(1 != (GetSystemMetrics(SM_CXSMICON)/16))
            iOffset = 1;

    RECT rcTemp;
    //Up State
    rcTemp.left     = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top      = 0;
    rcTemp.bottom   = iHeight;
    int iCurveLarge = GetSystemMetrics(SM_CXICON)/6;
    int iCurveSmall = GetSystemMetrics(SM_CXICON)/15;

    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurveLarge, iCurveLarge, m_crColor1, m_crColor2);

    rcTemp.left             = iCurveSmall - iOffset;
    rcTemp.right    = iWidth/2 - iCurveSmall/2;
    rcTemp.top              = iCurveSmall - iOffset;
    rcTemp.bottom   = (int)((double)iHeight/6.0*5.0);
    GradientRoundRectangle(oGDI, rcTemp, NULL_COLOR, iCurveSmall, iCurveSmall, m_crColor2, m_crColor1);

    //DownState     
    rcTemp.left             = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top              = 0;
    rcTemp.bottom   = iHeight;
    GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurveLarge, iCurveLarge, m_crColor1, m_crColor2);

    rcTemp.left             = iWidth/2 + iCurveSmall - iOffset;
    rcTemp.right    = iWidth - iCurveSmall/2;
    rcTemp.top              = iCurveSmall - iOffset + iCurveSmall/2;
    rcTemp.bottom   = (int)((double)iHeight/6.0*5.0) + iCurveSmall/2;
    GradientRoundRectangle(oGDI, rcTemp, NULL_COLOR, iCurveSmall, iCurveSmall, m_crColor2, m_crColor1);

    return TRUE;
}


//double gradient to look clear and shiny ... beveled .. most work done in IssGDI
BOOL CIssDynBtn::GenerateBtnVista(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    RECT rcTemp;
    //Up State
    rcTemp.left             = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top              = 0;
    rcTemp.bottom   = iHeight;
    int iCurve              = GetSystemMetrics(SM_CXICON)/6;
    ShinyRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurve, iCurve, m_crColor1, m_crColor2, m_crOutline1, m_crOutline2);

    //DownState     
    rcTemp.left             = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top              = 0;
    rcTemp.bottom   = iHeight;
    ShinyRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurve, iCurve, m_crColor1, m_crColor2, m_crOutline1, m_crOutline2);
    //CIssGDI::GradientRoundRectangle(oGDI, rcTemp, m_crOutline1, iCurve, iCurve, m_crColor2, m_crColor1);

    return TRUE;
}

//double gradient to look clear and shiny ... beveled .. most work done in IssGDI
BOOL CIssDynBtn::GenerateBtnButters(CIssGDIEx& oGDI, int& iWidth, int& iHeight)
{
    RECT rcTemp;

    BOOL bNoDither = FALSE;

    rcTemp.left = 0;
    rcTemp.top = 0;
    rcTemp.bottom = iHeight;
    rcTemp.right = iWidth;

    
    //fill our button with a transparent color
    FillRect(oGDI.GetDC(), rcTemp, TRANSPARENT_COLOR);


    //temp for angle gradient
    CIssGDIEx gdiTemp;
    CIssGDIEx gdiTemp2;
    gdiTemp.Create(oGDI.GetDC(), rcTemp, FALSE, TRUE, FALSE);
    gdiTemp2.Create(oGDI.GetDC(), rcTemp, FALSE, TRUE, TRUE);
    GradientFillRect(gdiTemp, rcTemp, m_crOutline2, m_crOutline1, bNoDither);

    //Up State
    rcTemp.left        = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top        = 0;
    rcTemp.bottom    = iHeight;
    int iCurve        = GetSystemMetrics(SM_CXICON)/6;

    //draw a black round rect for our button
    RoundRectangle(oGDI.GetDC(), rcTemp, 0, 0, iCurve, iCurve);

    //replace non transparent pixels with the angle fill rect
    for(int x = 0; x < iWidth/2; x++)
    {
        for(int y = 0; y < iHeight; y++)
        {
            if(oGDI.GetPixelColor(x, y) != TRANSPARENT_COLOR)
                oGDI.SetPixelColor(x, y, gdiTemp.GetPixelColor(x,y));
        }
    }

    //and finally .. draw our inner gradient
    int iIndent = 2;//GetSystemMetrics(SM_CXICON)/32;
    rcTemp.left += iIndent;
    rcTemp.top  += iIndent;
    rcTemp.right -= iIndent;
    rcTemp.bottom -= iIndent;

    //draw another black round rect into our temp2
    RoundRectangle(gdiTemp2.GetDC(), rcTemp, 0, 0, iCurve/2, iCurve/2);

    //draw the gradient into our first temp
    GradientFillRect(gdiTemp, rcTemp, m_crColor1, m_crColor2, bNoDither);

    //replace non transparent pixels with the angle fill rect
    for(int x = 0; x < iWidth/2; x++)
    {
        for(int y = 0; y < iHeight; y++)
        {
            if(gdiTemp2.GetPixelColor(x, y) != TRANSPARENT_COLOR)
                oGDI.SetPixelColor(x, y, gdiTemp.GetPixelColor(x,y));
        }
    }
    
    
    //DownState    
    iIndent = 1;
    rcTemp.left        = 0;
    rcTemp.right    = iWidth/2;
    rcTemp.top        = 0;
    rcTemp.bottom    = iHeight;
    GradientFillRect(gdiTemp, rcTemp, m_crOutline1, m_crOutline2, bNoDither);
    
    rcTemp.left        = iWidth/2;
    rcTemp.right    = iWidth;
    rcTemp.top        = 0;
    rcTemp.bottom    = iHeight;

    //draw a black round rect for our button
    RoundRectangle(oGDI.GetDC(), rcTemp, 0, 0, iCurve, iCurve);

    //replace non transparent pixels with the angle fill rect
    for(int x = 0; x < iWidth/2; x++)
    {
        for(int y = 0; y < iHeight; y++)
        {
            if(oGDI.GetPixelColor(x + rcTemp.left, y) != TRANSPARENT_COLOR)
                oGDI.SetPixelColor(x + rcTemp.left, y, gdiTemp.GetPixelColor(x,y));
        }
    }

    rcTemp.left = rcTemp.left + iIndent - iWidth/2;
    rcTemp.top  += iIndent;
    rcTemp.right = rcTemp.right - iIndent - iWidth/2;
    rcTemp.bottom -= iIndent;

    //draw the gradient into our first temp
    GradientFillRect(gdiTemp, rcTemp, m_crColor1, m_crColor2, bNoDither);

    //replace non transparent pixels with the angle fill rect
    for(int x = 0; x < iWidth/2; x++)
    {
        for(int y = 0; y < iHeight; y++)
        {
            if(gdiTemp2.GetPixelColor(x, y) != TRANSPARENT_COLOR)
                oGDI.SetPixelColor(x+iWidth/2, y, gdiTemp.GetPixelColor(x,y));
        }
    }
    return TRUE;
}


/************************************************************************
        Lets Draw our Button

        use this Function and NOT Draw button
************************************************************************/
HRESULT CIssDynBtn::Draw(HFONT      hFont, 
                      HDC           hDC,
                      TCHAR*        szTextOverride  /* NULL*/,
                      TCHAR*        szExp                   /* NULL */,
                      HFONT         hFontExp                /* NULL */)
{
    HRESULT hr = S_OK;

    CHHRE(hDC, _T("Invalid hdc"), E_INVALIDARG);

    HDC  hdcDest = hDC;
    RECT rcBtnRect;


    memcpy(&rcBtnRect, &m_rcBtnRect, sizeof(RECT));

    if(m_bDisabled)
    {
        rcBtnRect.left          = 0;
        rcBtnRect.top           = 0;
        rcBtnRect.right         = m_rcBtnRect.right-m_rcBtnRect.left;
        rcBtnRect.bottom        = m_rcBtnRect.bottom - m_rcBtnRect.top;

        if(!m_gdiDisabled)
        {
                m_gdiDisabled = new CIssGDIEx;
                
                // create the disabled image for drawing
                m_gdiDisabled->Create(hDC,rcBtnRect);
                // draw a pink background
                HBRUSH hbrPink = CIssGDIEx::CreateBrush(TRANSPARENT_COLOR);
                FillRect(m_gdiDisabled->GetDC(), &rcBtnRect, hbrPink);
                DeleteObject(hbrPink);
        }
        hdcDest = m_gdiDisabled->GetDC();
        CHHRE(hDC, _T("Invalid hdc"), E_INVALIDARG);
    }

    if(m_pDynBtn) //if the pointer is valid draw the button now
        m_pDynBtn->DrawButton(hdcDest, m_bBtnDown, rcBtnRect);
    else
        DrawButton(hdcDest, m_bBtnDown, rcBtnRect);

    if(! DrawButtonText(hFont, hdcDest, rcBtnRect, szTextOverride, szExp, hFontExp))
        hr = E_INVALIDARG;

    if(m_bDisabled && m_gdiDisabled)
    {
        // add our grayed image on top of the button
        AlphaFillRect(*m_gdiDisabled, rcBtnRect, RGB(200,200,200), 160);

        // draw back to the screen buffer
#ifndef UNDER_CE
        TransparentBlt(hDC,
#else
        TransparentImage(hDC,
#endif
            m_rcBtnRect.left,
            m_rcBtnRect.top,
            m_rcBtnRect.right - m_rcBtnRect.left,
            m_rcBtnRect.bottom - m_rcBtnRect.top,
            m_gdiDisabled->GetDC(),
            0,0,
            m_rcBtnRect.right - m_rcBtnRect.left,
            m_rcBtnRect.bottom - m_rcBtnRect.top,
            TRANSPARENT_COLOR);
    }
Error:
    return hr;
}


/************************************************************************
        Draw the Button when Draw is Called
************************************************************************/
HRESULT CIssDynBtn::DrawButton(HDC         hDC,
                                BOOL&   bBtnDown,
                                RECT&   rcBtnRect)
{
        RECT rcTemp;
        rcTemp.left     = 0;
        rcTemp.top      = 0;
        rcTemp.right    = 0;
        rcTemp.bottom   = 0;

        return DrawButton(hDC, bBtnDown, rcBtnRect, rcTemp);
}


//Draw the generated button in its current state
HRESULT CIssDynBtn::DrawButton(HDC         hDC,
                                BOOL&   bBtnDown,
                                RECT&   rcBtnRect,
                                RECT    rcRect)
{
    HRESULT hr = S_OK;

    if(bBtnDown)
    {
#ifndef UNDER_CE
        TransparentBlt(hDC,
#else
        TransparentImage(hDC,
#endif
                       rcBtnRect.left               - rcRect.left,
                       rcBtnRect.top                - rcRect.top,
                       rcBtnRect.right              - rcBtnRect.left        - rcRect.left,
                       rcBtnRect.bottom             - rcBtnRect.top         - rcRect.top,
                       m_gdiButton.GetDC(),
                       rcBtnRect.right              - rcBtnRect.left,
                       0,
                       rcBtnRect.right              - rcBtnRect.left,
                       rcBtnRect.bottom             - rcBtnRect.top,
                       TRANSPARENT_COLOR);
    }
    else // draw the up button
    {
#ifndef UNDER_CE
      TransparentBlt(hDC,
#else
      TransparentImage(hDC,
#endif
                       rcBtnRect.left               - rcRect.left,
                       rcBtnRect.top                - rcRect.top,
                       rcBtnRect.right              - rcBtnRect.left        - rcRect.left,
                       rcBtnRect.bottom             - rcBtnRect.top         - rcRect.top,
                       m_gdiButton.GetDC(),
                       0,
                       0,
                       rcBtnRect.right              - rcBtnRect.left,
                       rcBtnRect.bottom             - rcBtnRect.top,
                       TRANSPARENT_COLOR);
    }
    return hr;
}

/************************************************************************
        Draw the Button Text when Draw is Called
************************************************************************/
BOOL CIssDynBtn::DrawButtonText(HFONT hFont, HDC hDC, RECT rcBtnRect, TCHAR* szTextOverride /* = NULL */, TCHAR* szExp /* = NULL */, HFONT hFontExp /* = NULL */)
{
    TCHAR* szText = NULL;
    BOOL bUseExp;

    if(szExp == NULL || _tcslen(szExp) == 0)        
        bUseExp = FALSE;
    else                            
        bUseExp = TRUE;

    // are we using the quick override of the saved text
    szText  = szTextOverride?szTextOverride:m_szBtnChar;
    if(!szText)
        return TRUE;

    SIZE sizeTxt;
    RECT rcTxt;
    SIZE sizeExp;
    RECT rcExp;
    sizeExp.cx              = 0;
    sizeExp.cy              = 0;
    rcExp.left              = 0;
    rcExp.right             = 0;
    rcExp.top               = 0;
    rcExp.bottom    = 0;

    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

    // Find the size of the text we want to place on the button
    GetTextExtentPoint( hDC, szText, (int)_tcslen( szText ), &sizeTxt);

    if(bUseExp)
    {
            SelectObject(hDC, hFontExp);
            GetTextExtentPoint( hDC, szExp, (int)_tcslen( szExp ), &sizeExp);
            SelectObject(hDC, hFont);
    }
    // line up the text within the button
    rcTxt.left              = rcBtnRect.left + (rcBtnRect.right - sizeTxt.cx - sizeExp.cx - rcBtnRect.left)/2;
    rcTxt.top               = rcBtnRect.top + (rcBtnRect.bottom - sizeTxt.cy - rcBtnRect.top)/2 ;
    rcTxt.top          += m_bBtnDown?(GetSystemMetrics(SM_CXSMICON)/16):0;
    rcTxt.right             = rcTxt.left + sizeTxt.cx;
    rcTxt.bottom    = rcTxt.top + sizeTxt.cy;

    if(bUseExp)
    {
            rcExp.left              = rcTxt.right;
            rcExp.right             = rcExp.left + sizeExp.cx;
            rcExp.bottom    = rcTxt.bottom - (rcTxt.bottom - rcTxt.top)/3   ;
            rcExp.top               = rcExp.bottom - sizeExp.cy;

            //Now Re-Center
            int iOffset             = (sizeExp.cy)/3 - (sizeTxt.cy)/6;

            rcExp.bottom    += iOffset;
            rcExp.top               += iOffset;

            rcTxt.top               += iOffset;     
            rcTxt.bottom    += iOffset; 
            
    }

    // draw the text to the screen
    int iOldMode = SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, m_crTextColor);
    DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
    if(bUseExp)
    {
        SelectObject(hDC, hFontExp);
        DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
        SelectObject(hDC, hFont);
    }       
    if(m_bUseShadow)
    {
        SetTextColor(hDC, m_crTextShadow);
        rcTxt.left++;
        rcTxt.right++;
        rcTxt.top--;
        rcTxt.bottom--;
        rcExp.left++;
        rcExp.right++;
        rcExp.top--;
        rcExp.bottom--;
        DrawText(hDC, szText, -1, &rcTxt, DT_SINGLELINE|DT_TOP|DT_LEFT);
        if(bUseExp)
            DrawText(hDC, szExp, -1, &rcExp, DT_SINGLELINE|DT_TOP|DT_LEFT);
    }

    SetBkMode(hDC, iOldMode);
    SelectObject(hDC, oldFont);

    return TRUE;
}


