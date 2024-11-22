/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSGDIDraw.h 
// Abstract: Windows base class to simplify handling common windows messages 
// 
/***************************************************************************************************************/ 

#pragma once

#include "IssGDIEx.h"

#ifndef DT_END_ELLIPSIS 
#define DT_END_ELLIPSIS  0x00008000 
#endif

#define NULL_COLOR                      RGB(248,4,248)

#define Blit(dcSrc, dcDest, rcClip) \
    BitBlt(dcDest, rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), \
            dcSrc, rcClip.left, rcClip.top, SRCCOPY);

#define BlitTo(dcSrc, dcDest, rc) \
    BitBlt(dcDest, rc.left, rc.top, WIDTH(rc), HEIGHT(rc), \
    dcSrc, 0, 0, SRCCOPY);

enum EnumAlphaTransfer
{
    ALPHA_None,         // don't copy alpha values
    ALPHA_Normal,       // draw on dest DC with src DC but use Src Alpha
    ALPHA_Copy,         // copy over dest DC alpha with src DC alpha
    ALPHA_AddValue,     // copy over dest DC alpha with src DC alpha only if it's value is higher (ie. less transparent)
};

HRESULT     Draw(CIssGDIEx& gdiDest, 
                 RECT rcDest, 
                 CIssGDIEx& gdiSrc, 
                 int iXStart = 0, 
                 int iYStart = 0,
                 EnumAlphaTransfer eTransfer = ALPHA_Normal,
                 int iAlpha = 255
                 );
HRESULT     Draw(CIssGDIEx& gdiDest, 
                 int xoriginDest,
                 int yoriginDest,
                 int wDest,
                 int hDest,
                 CIssGDIEx& gdiSrc, 
                 int xoriginSrc = 0, 
                 int yoriginSrc = 0,
                 EnumAlphaTransfer eTransfer = ALPHA_Normal,
                 int iAlpha = 255
                 );
HRESULT     CopyAlpha(CIssGDIEx& gdiDest, 
                 RECT rcDest, 
                 CIssGDIEx& gdiSrc, 
                 int iXStart = 0, 
                 int iYStart = 0,
                 BOOL bAddAlphaVal = FALSE
                 );
HRESULT     CopyAlpha(CIssGDIEx& gdiDest, 
                 int xoriginDest,
                 int yoriginDest,
                 int wDest,
                 int hDest,
                 CIssGDIEx& gdiSrc, 
                 int xoriginSrc = 0, 
                 int yoriginSrc = 0,
                 BOOL bAddAlphaVal = FALSE);
HRESULT     FillRect(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crColor);
HRESULT     FillRect(HDC hDC, RECT& rcDraw, COLORREF crColor);
HRESULT     FillRectAlpha(CIssGDIEx& gdiDest, RECT& rcDraw, BYTE btAlpha);
HRESULT     TileBackGround(CIssGDIEx& gdiDest, RECT& rc, CIssGDIEx& gdiTexture, BOOL bRelativeRect);
HRESULT     GradientFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2, BOOL bUseIssVersion = TRUE);
HRESULT     GradientHorzFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2, BOOL bUseIssVersion = TRUE);
HRESULT     GradientAngleFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     GradientRadialFillRect(CIssGDIEx& gdiDest, int iRadialWidth, RECT& rcDrawEnd, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     FrameRect(HDC hdc, RECT& rcDraw, COLORREF crOutline, int iPenWidth = 1);
HRESULT     Line(HDC hDC, int iXStart, int iYStart, int iXEnd, int iYEnd, COLORREF crLine, int iPenWidth = 1);


HRESULT     DrawText(HDC hDC, 
                     TCHAR* szText,
                     RECT& rc, 
                     UINT    uiFormat = DT_LEFT, 
                     HFONT hFont = NULL, 
                     COLORREF crColor = TRANSPARENT_COLOR, 
                     int    iBkMode = TRANSPARENT
                     );

HRESULT     DrawText(HDC hDC, 
                     TCHAR* szText,
                     int iXPos, 
                     int iYpos, 
                     UINT    uiFormat = DT_LEFT, 
                     HFONT hFont = NULL, 
                     COLORREF crColor = TRANSPARENT_COLOR, 
                     int    iBkMode = TRANSPARENT
                     );
HRESULT     DrawTextShadow(HDC hDC, 
                     TCHAR* szText,
                     RECT& rc, 
                     UINT    uiFormat = DT_LEFT, 
                     HFONT hFont = NULL, 
                     COLORREF crColor = TRANSPARENT_COLOR, 
                     COLORREF crShadow = 0,
                     int    iBkMode = TRANSPARENT
                     );

HRESULT     DrawTextShadow(HDC hDC, 
                     TCHAR* szText,
                     int iXPos, 
                     int iYpos, 
                     UINT    uiFormat = DT_LEFT, 
                     HFONT hFont = NULL, 
                     COLORREF crColor = TRANSPARENT_COLOR, 
                     COLORREF crShadow = 0,
                     int    iBkMode = TRANSPARENT
                     );

HRESULT     AdjustThemeColors(CIssGDIEx& gdi, COLORREF crLo, COLORREF crHi);
HRESULT     AlphaFillRect(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha);

HRESULT     RoundRectangle(HDC hdc, 
                               RECT& rcDraw, 
                               COLORREF crColor, COLORREF crOutline, 
                               int iRoundWidth, int iRoundHeight
                               );

HRESULT     ShinyRoundRectangle(CIssGDIEx& gdiDest,    
                               RECT& rcDraw, 
                               COLORREF crOutline, 
                               int iRoundWidth, int iRoundHeight, 
                               COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                               );

HRESULT     GradientRoundRectangle(CIssGDIEx& gdiDest, 
                               RECT& rcDraw, 
                               COLORREF crOutline,
                               int iRoundWidth, 
                               int iRoundHeight, 
                               COLORREF crGrad1, 
                               COLORREF crGrad2
                               );

HRESULT     Rectangle(HDC hdc, 
                               RECT& rcDraw, 
                               COLORREF crColor, 
                               COLORREF crOutline
                               );

HRESULT     DitherImage(CIssGDIEx& gdiDest);

