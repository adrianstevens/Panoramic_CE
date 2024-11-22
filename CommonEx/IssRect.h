/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssRect.h
// Abstract: Class to encapsulate common routines around RECT structure
//  
/***************************************************************************************************************/ 
#pragma once

#include "windows.h"

class CIssRect
{
public:
        CIssRect(void);
        CIssRect(CIssRect& rc);
        CIssRect(RECT& rc);
        ~CIssRect(void);

        operator RECT();
        BOOL operator==(RECT& rc);
        BOOL operator==(CIssRect& rc);

        HRESULT     Set(int iLeft, int iTop, int iRight, int iBottom);
        HRESULT     Set(CIssRect& rc);
        HRESULT     Set(RECT& rc);
        RECT        Get();
        HRESULT     Stretch(int iStretch);
        HRESULT     Translate(int iX, int iY);
        HRESULT     Concatenate(RECT rc);
        HRESULT     ZeroBase();
        int         GetWidth(){return right - left;};
        int         GetHeight(){return bottom - top;};
        BOOL        PtInRect(POINT& pt);
        BOOL        RectInRect(RECT& rc);
        BOOL        RectInRect(CIssRect& rc);

public: // variables
        int             left;
        int             right;
        int             top;
        int             bottom;
};
