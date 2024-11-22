/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssDynDisplay.cpp 
// Abstract: Creates display images for applications such as calculators
// 
/**************************************************************************************************************/ 

#include "windows.h"
#include "IssDynDisplay.h"
#include "IssDynBtn.h"
#include "IssGDIDraw.h"
#include "IssDynDisplay.h"
#include "IssDebug.h"

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIssDynDisplay::CIssDynDisplay():
 m_eDisplay(DISPLAY_Calc)
,m_pDyn(NULL)
{
        
}

CIssDynDisplay::~CIssDynDisplay()
{
    Destroy();
}

//clean up our GDI memory
HRESULT CIssDynDisplay::Destroy()
{
    m_gdiDisplay.Destroy(); 
    return S_OK;
}

//Initialize the size and color of the display
HRESULT CIssDynDisplay::Init(RECT&                   rc, 
                            COLORREF                crBackground1,                                                  
                            COLORREF                crBackground2, 
                            COLORREF                crHiLite1, 
                            COLORREF                crHiLite2, 
                            EnumDisplay             eDisplay, 
                            CIssDynDisplay* pDyn)
{
    HRESULT hr = S_OK;

    Destroy();

    memcpy(&m_rcDyn, &rc, sizeof(RECT));

    m_crBG1     = crBackground1;
    m_crBG2     = crBackground2;        
    m_crHiLite1 = crHiLite1;
    m_crHiLite2 = crHiLite2;
    m_eDisplay  = eDisplay;
    m_pDyn      = pDyn;

    hr = CreateDisplay();
    CHRE(hr, _T("Create Failed"), E_ABORT);

Error:
    return hr;
}

/************************************************************************
    Generates the Display after the variables have been initialized
    
    Will call the appropriate CreateDisplay function depending on 
    the button style
************************************************************************/
HRESULT CIssDynDisplay::CreateDisplay()
{
    HRESULT hr = S_OK;
    BOOL bRet;

    int     iHeight = m_rcDyn.bottom - m_rcDyn.top;
    int     iWidth  = m_rcDyn.right - m_rcDyn.left;

    // Init the GDI
    HDC dc = GetDC(NULL);
    hr = m_gdiDisplay.Create(dc, m_rcDyn, FALSE, TRUE);
    ReleaseDC(NULL, dc);
    CHRE(hr, _T("Display Create Failed - prob not an issue"), E_ABORT);

    // Fill it Pink
    FillRect(m_gdiDisplay.GetDC(), m_rcDyn, RGB(255,0,255));

    //*****************************************************************
    switch(m_eDisplay) 
    {
    case DISPLAY_Calc:
    default:
        bRet = CreateDisplayCalc();
        CBHR(bRet, _T("CreateDisplayCalc"));
        break;
    case DISPLAY_DoubleStroke:
        bRet = CreateDisplayDoubleStroke();
        CBHR(bRet, _T("CreateDisplayDoubleStroke"));
        break;
    case DISPLAY_Grad_DS:
        bRet = CreateDisplayGradientDoubleStroke();
        CBHR(bRet, _T("CreateDisplayGradientDoubleStroke"));
        break;
    }

Error:
    return hr;
}

//Creates the "Calc" display type
BOOL CIssDynDisplay::CreateDisplayCalc()
{
    POINT pt[3];

    RECT rcDisplay;

    rcDisplay.left  = 0;
    rcDisplay.top   = 0;
    rcDisplay.right = m_rcDyn.right - m_rcDyn.left;
    rcDisplay.bottom= m_rcDyn.bottom - m_rcDyn.top;

    FillRect(m_gdiDisplay.GetDC(), rcDisplay, m_crBG1);
    
    HPEN Pen1 =     ::CreatePen(PS_SOLID, GetSystemMetrics(SM_CXSMICON)/16, m_crHiLite1);
    HPEN Pen2 =     ::CreatePen(PS_SOLID, GetSystemMetrics(SM_CXSMICON)/16, m_crHiLite2);

    HPEN OldPen;

    //Crap below is to correct for crappy Polyline Function sucking ass and 
    //difs between VGA & QVGA
    
    pt[0].x = rcDisplay.left - 1 + GetSystemMetrics(SM_CXSMICON)/16;
    pt[0].y = rcDisplay.bottom- (1);

    pt[1].x = rcDisplay.right- (1);
    pt[1].y = rcDisplay.bottom- (1);

    pt[2].x = rcDisplay.right- (1);
    pt[2].y = rcDisplay.top - 2 + GetSystemMetrics(SM_CXSMICON)/16;

    OldPen = (HPEN)SelectObject(m_gdiDisplay.GetDC(), Pen1);

    Polyline(m_gdiDisplay.GetDC(), pt, 3);

    pt[2].y = rcDisplay.top - 1 + GetSystemMetrics(SM_CXSMICON)/16;
    pt[1].x = rcDisplay.left - 1 + GetSystemMetrics(SM_CXSMICON)/16;
    pt[1].y = rcDisplay.top - 1 + GetSystemMetrics(SM_CXSMICON)/16;

    SelectObject(m_gdiDisplay.GetDC(), Pen2);

    Polyline(m_gdiDisplay.GetDC(), pt, 3);
    
    SelectObject(m_gdiDisplay.GetDC(), OldPen);

    DeleteObject(Pen1);
    DeleteObject(Pen2);

    return TRUE;
}

//Creates the "DoubleStroke" display type
BOOL CIssDynDisplay::CreateDisplayDoubleStroke()
{
    RECT rcDisplay;

    rcDisplay.left  = 0;
    rcDisplay.top   = 0;
    rcDisplay.right = m_rcDyn.right - m_rcDyn.left;
    rcDisplay.bottom= m_rcDyn.bottom - m_rcDyn.top;

    Rectangle(m_gdiDisplay.GetDC(), rcDisplay, m_crBG1, m_crHiLite2);
    
    rcDisplay.left++;
    rcDisplay.top++;
    rcDisplay.right--;
    rcDisplay.bottom--;

    Rectangle(m_gdiDisplay.GetDC(), rcDisplay, m_crBG1, m_crHiLite1);
    

    return TRUE;
}

//Creates the "Gradient Double Stroke" display type
BOOL CIssDynDisplay::CreateDisplayGradientDoubleStroke()
{
    RECT rcDisplay;
    BOOL bNoDither = FALSE;

    rcDisplay.left  = 0;
    rcDisplay.top   = 0;
    rcDisplay.right = m_rcDyn.right - m_rcDyn.left;
    rcDisplay.bottom= m_rcDyn.bottom - m_rcDyn.top;

    Rectangle(m_gdiDisplay.GetDC(), rcDisplay, m_crBG1, m_crHiLite2);

    rcDisplay.left++;
    rcDisplay.top++;
    rcDisplay.right--;
    rcDisplay.bottom--;

    Rectangle(m_gdiDisplay.GetDC(), rcDisplay, m_crBG1, m_crHiLite1);

    rcDisplay.left++;
    rcDisplay.top++;
    rcDisplay.right--;
    rcDisplay.bottom--;

    GradientFillRect(m_gdiDisplay, rcDisplay, m_crBG1, m_crBG2, bNoDither);


    return TRUE;
}

//Draw our display from the GDI
HRESULT CIssDynDisplay::Draw(HDC dc)
{
    HRESULT hr = S_OK;

    if(m_pDyn != NULL)
    {
        CBHRE(m_pDyn->DrawAt(dc, m_rcDyn), _T("Draw Failed"), E_ABORT);
    }
    else
    {
        CBHRE(DrawAt(dc, m_rcDyn), _T("Draw Failed"), E_ABORT);
    }

Error:
    return hr;
}

//Draw at a specific location - ie, if you want to override the saved location
BOOL CIssDynDisplay::DrawAt(HDC dc, RECT& rc)
{
    if(dc == NULL)
        return FALSE;

#ifndef UNDER_CE
    TransparentBlt(dc,
#else
    TransparentImage(dc,
#endif
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        m_gdiDisplay.GetDC(), 
        0,
        0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        RGB(255,0,255));

        
   return TRUE;
}