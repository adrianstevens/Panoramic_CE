/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssDynDisplay.h 
// Abstract: Creates display images for applications such as calculators
// 
/***************************************************************************************************************/ 

#pragma once

#include "IssGDIEx.h"

enum EnumDisplay
{
        DISPLAY_Calc,
        DISPLAY_DoubleStroke,
        DISPLAY_Grad_DS,
};

class CIssDynDisplay  
{
public:
        CIssDynDisplay();
        virtual ~CIssDynDisplay();

    HRESULT  Init(RECT&         rc, 
                 COLORREF       crBackground1 = RGB(200,200,200),                                                     
                 COLORREF       crBackground2 = RGB(200,200,200), 
                 COLORREF       crHiLite1     = RGB(255,255,255), 
                 COLORREF       crHiLite2     = RGB(0,0,0), 
                 EnumDisplay    eDisplay      = DISPLAY_Calc, 
                 CIssDynDisplay* pDyn         = NULL);

    HRESULT  Draw(HDC dc);
    HRESULT  Destroy();
        
    RECT     GetPosition(){return m_rcDyn;};//useful
    HDC      GetDisplayDC(){return m_gdiDisplay.GetDC();};//might be useful

private:
    BOOL     DrawAt(HDC dc, RECT& rc);

    HRESULT  CreateDisplay();
    BOOL     CreateDisplayCalc();
    BOOL     CreateDisplayDoubleStroke();
    BOOL     CreateDisplayGradientDoubleStroke();

private:
    CIssGDIEx m_gdiDisplay;   // GDI for the button up and down states

    EnumDisplay                     m_eDisplay;
    COLORREF                        m_crBG1;
    COLORREF                        m_crBG2;
    COLORREF                        m_crHiLite1;
    COLORREF                        m_crHiLite2;

    RECT                            m_rcDyn;

    CIssDynDisplay                  *m_pDyn;
};

