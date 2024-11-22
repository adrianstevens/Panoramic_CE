/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSDynBtn.h 
// Abstract: Comprehensive Button Class with 9+ button styles. 
// 
/***************************************************************************************************************/ 

#pragma once

#include "Wingdi.h"
#include "IssGDIEx.h"

#define NUMBER_OF_BTN_STYLES 11

enum BtnStyleType
{
    BTN_Keyboard,  //0
    BTN_Flat,
    BTN_Grad,
    BTN_Bevel,
    BTN_Ellipse,
    BTN_HP12c,              //5
    BTN_Shine,              //6
    BTN_Shine2,             //7
    BTN_Keyboard2,  //8
    BTN_Shine3,
    BTN_Vista,
    BTN_Butter,     //11
    BTN_Adrian,
    BTN_Counts,
};

class CIssDynBtn  
{
public:
    CIssDynBtn();
    virtual ~CIssDynBtn();

    //Initialize and Destroy Functions
    HRESULT InitAdvanced(COLORREF crGrad1,  //This should always be called first
                         COLORREF crGrad2,
                         COLORREF crOutline1,
                         COLORREF crOutline2,
                         COLORREF crBackgroundOutline           = RGB(0,0,0),
                         INT      iBackgroundLineThickness      = 0,
                         BOOL     bRadioButton                  = FALSE);


    HRESULT Init(RECT  rcSize,          //This function inits and generates the button
                         TCHAR*  szChar,                 //call InitAdvanced First!!!
                         HWND    hWnd, 
                         UINT    uiMessage,
                         int     iBtnNum,
                         COLORREF crColor,
                         BtnStyleType eBtnStyle,
                         CIssDynBtn* pDynBtn = NULL);

    HRESULT SetTextColors(COLORREF crPrimaryTextColor,     //Can be called before or after Init
                          COLORREF crShadowColor,
                          BOOL bUseShadow = FALSE);

    HRESULT  SetString(TCHAR* szChar);
    TCHAR*   GetString(){return m_szBtnChar;};
    
    HRESULT  Destroy();

    //Draw
    HRESULT  Draw(HFONT   hFont, 
                HDC             hDC,
                TCHAR*  szTextOverride = NULL,
                TCHAR*  szExp = NULL,
                HFONT   hFontExp = NULL);
    
    HRESULT  DrawButton(HDC hDC, BOOL& bBtnDown, RECT& rcBtnRect);
    HRESULT  DrawButton(HDC hDC, BOOL& bBtnDown, RECT& rcBtnRect, RECT rcRect);

    //Input Functions
    virtual BOOL OnLButtonDown(POINT& point);
    virtual BOOL OnLButtonUp(POINT& point);
    virtual BOOL OnMouseMove(POINT& point);

    //Helper Functions for various apps
    HRESULT DisableButton(BOOL bDisabled);
    int     GetBtnHeight()          {return m_rcBtnRect.bottom - m_rcBtnRect.top;};
    int     GetBtnWidth()           {return m_rcBtnRect.right - m_rcBtnRect.left;};
    POINT   GetBtnLocation();
    BOOL    IsButtonDown()          {return m_bBtnDown;};
    HRESULT GetButtonRect(RECT &rc);
    RECT    GetButtonRect();
    HRESULT MoveButton(POINT pt);
    HRESULT ResetRadioButton();
    
    
protected:
    //Generate Button Functions
    BOOL    GenerateBtn();
    BOOL    GenerateBtnFlat(     CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnGrad(     CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnBevel(    CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnEllipse(  CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnKeyboard( CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnHP12c(    CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnShine(    CIssGDIEx& oGDI, int& iWidth, int& iHeight, double fPercent = 0.2); //shine 
    BOOL    GenerateBtnShine2(   CIssGDIEx& oGDI, int& iWidth, int& iHeight); //less shiny
    BOOL    GenerateBtnShine3(   CIssGDIEx& oGDI, int& iWidth, int& iHeight, double fPercent = 0.15); //shine without the double border
    BOOL    GenerateBtnKeyboard2(CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnVista(    CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnButters(  CIssGDIEx& oGDI, int& iWidth, int& iHeight);
    BOOL    GenerateBtnAdrian (  CIssGDIEx& oGDI, int& iWidth, int& iHeight);

    //Draw Functions
    BOOL    DrawButtonText(HFONT   hFont, 
                           HDC     hDC,
                           RECT    rcBtnRect,
                           TCHAR*  szTextOverride = NULL,
                           TCHAR*  szExp = NULL,
                           HFONT   hFontExp = NULL);

public:
    TCHAR*                  m_szBtnChar;    // Text to draw on the button
    BOOL                    m_bOldBtnDown;

protected:      
    //Classes
    CIssDynBtn*             m_pDynBtn;      // Pointer to another class that has
    CIssGDIEx               m_gdiButton;    // GDI for the button up and down states
    CIssGDIEx*              m_gdiDisabled;  // if we're going to draw a disabled button

    //General
    BtnStyleType            m_eBtnStyle;    // Which button style to use
    BOOL                    m_bRadioBtn;    // Are we using the Radio Button style
    BOOL                    m_bDisabled;    // is the button disabled
    BOOL                    m_bBtnDown;     // Bool to track up/down state

    //Text Variables
    COLORREF                m_crTextColor;
    COLORREF                m_crTextShadow;
    BOOL                    m_bUseShadow;


    //Init
    UINT                    m_uiMessage;    // Message ID to send to Parent
    HWND                    m_hParentWnd;   // Handle to the Parent window
    int                     m_iBtnNum;      // Button Number Pressed...this is passed in and then sent out as a windows message
    RECT                    m_rcBtnRect;    // location of the button to draw

    //InitAdvanced
    COLORREF                m_crColor1;     // Gradient color start
    COLORREF                m_crColor2;     // Gradient color end
    COLORREF                m_crOutline1;   // Outline start
    COLORREF                m_crOutline2;   // Outline end
    COLORREF                m_crBackground; // Not Used
    int                     m_iBackgroundThickness; //not used anymore - legacy compatibly for old projects
    

};

