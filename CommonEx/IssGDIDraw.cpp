/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSGDIDraw.cpp 
// Abstract: Windows base class to simplify handling common windows messages 
// 
/***************************************************************************************************************/ 

#include "IssGDIDraw.h"
#include "math.h"
#include "IssDebug.h"
#include "IssString.h"

//////////////////////////////////////////////////////////////////////////
// these are the private functions
HRESULT     EnsureLocation(CIssGDIEx& gdiDest, 
                           int& xoriginDest,
                           int& yoriginDest,
                           int& wDest,
                           int& hDest,
                           CIssGDIEx& gdiSrc, 
                           int& xoriginSrc, 
                           int& yoriginSrc
                           );
HRESULT     DrawWithAlpha(CIssGDIEx& gdiDest,
                          int xoriginDest, 
                          int yoriginDest, 
                          int wDest, 
                          int hDest, 
                          CIssGDIEx& gdiSrc,
                          int xoriginSrc, 
                          int yoriginSrc,
                          int iAlpha = -1
                          );
HRESULT     DrawWithAlpha16(CIssGDIEx& gdiDest,
                            int xoriginDest, 
                            int yoriginDest, 
                            int wDest, 
                            int hDest, 
                            CIssGDIEx& gdiSrc,
                            int xoriginSrc, 
                            int yoriginSrc,
                            int iAlpha = -1
                            );
HRESULT     DrawWithAlpha24(CIssGDIEx& gdiDest,
                            int xoriginDest, 
                            int yoriginDest, 
                            int wDest, 
                            int hDest, 
                            CIssGDIEx& gdiSrc,
                            int xoriginSrc, 
                            int yoriginSrc,
                            int iAlpha = -1
                            );
HRESULT     AdjustThemeColors16(CIssGDIEx& gdi, COLORREF crLo, COLORREF crHi);
HRESULT     AlphaFillRect16(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha);
HRESULT     AlphaFillRect24(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha);
HRESULT     ShinyRoundRectangle16(CIssGDIEx& gdiDest,    
                                  RECT& rcDraw, 
                                  COLORREF crOutline, 
                                  int iRoundWidth, int iRoundHeight, 
                                  COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                                  );
HRESULT     ShinyRoundRectangle24(CIssGDIEx& gdiDest,    
                                  RECT& rcDraw, 
                                  COLORREF crOutline, 
                                  int iRoundWidth, int iRoundHeight, 
                                  COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                                  );
HRESULT     GradientHorzFillRect16(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     GradientHorzFillRect24(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     GradientFillRect16(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     GradientFillRect24(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2);
HRESULT     GradientRoundRectangle16(CIssGDIEx& gdiDest, 
                                   RECT& rcDraw, 
                                   COLORREF crOutline,
                                   int iRoundWidth, 
                                   int iRoundHeight, 
                                   COLORREF crGrad1, 
                                   COLORREF crGrad2
                                   );
HRESULT     GradientRoundRectangle24(CIssGDIEx& gdiDest, 
                                   RECT& rcDraw, 
                                   COLORREF crOutline,
                                   int iRoundWidth, 
                                   int iRoundHeight, 
                                   COLORREF crGrad1, 
                                   COLORREF crGrad2
                                   );

// End private functions
//////////////////////////////////////////////////////////////////////////


HRESULT Draw(CIssGDIEx& gdiDest, RECT rcDest, CIssGDIEx& gdiSrc, int iXStart, int iYStart, EnumAlphaTransfer eTransfer, int iAlpha)
{
    return Draw(gdiDest, rcDest.left, rcDest.top, WIDTH(rcDest), HEIGHT(rcDest), gdiSrc, iXStart, iYStart, eTransfer, iAlpha);
}

HRESULT Draw(CIssGDIEx& gdiDest, 
             int xoriginDest,
             int yoriginDest,
             int wDest,
             int hDest,
             CIssGDIEx& gdiSrc, 
             int xoriginSrc, 
             int yoriginSrc,
             EnumAlphaTransfer eTransfer,
             int iAlpha
             )
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC(), _T("gdiDest has no DC"));
    CBARG(gdiSrc.GetDC(), _T("gdiSrc has no DC"));

    if(gdiSrc.GetAlpha() == NULL || 
       gdiDest.GetBits() == NULL || 
       gdiSrc.GetBits() == NULL  ||
       eTransfer == ALPHA_None   ||
       eTransfer == ALPHA_Copy   /*||
       eTransfer == ALPHA_AddValue*/)
    {
        // straight BitBlt
        BOOL bRet = BitBlt(gdiDest.GetDC(),
                           xoriginDest, yoriginDest,
                           wDest, hDest,
                           gdiSrc.GetDC(),
                           xoriginSrc, yoriginSrc,
                           SRCCOPY);
        CBHR(bRet, _T("BitBlt failed"));

        if((eTransfer == ALPHA_Copy || eTransfer == ALPHA_AddValue) && gdiSrc.GetAlpha())
        {
            hr = CopyAlpha(gdiDest,
                           xoriginDest, yoriginDest,
                           wDest, hDest,
                           gdiSrc,
                           xoriginSrc, yoriginSrc,
                           (eTransfer == ALPHA_Copy?FALSE:TRUE));
            CHR(hr, _T("CopyAlpha failed"));
        }
    }
    else if(eTransfer == ALPHA_AddValue)
    {
        hr = DrawWithAlpha(gdiDest,
            xoriginDest, yoriginDest,
            wDest, hDest,
            gdiSrc,
            xoriginSrc, yoriginSrc,
            iAlpha);
        //CHR(hr, _T("DrawWithAlpha failed"));
        if(hr != S_OK)
            goto Error;

        hr = CopyAlpha(gdiDest,
            xoriginDest, yoriginDest,
            wDest, hDest,
            gdiSrc,
            xoriginSrc, yoriginSrc,
            (eTransfer == ALPHA_Copy?FALSE:TRUE));
        CHR(hr, _T("CopyAlpha failed"));
    }
    else
    {
        hr = DrawWithAlpha(gdiDest,
                            xoriginDest, yoriginDest,
                            wDest, hDest,
                            gdiSrc,
                            xoriginSrc, yoriginSrc,
                            iAlpha);
        //CHR(hr, _T("DrawWithAlpha failed"));
        if(hr != S_OK)
            goto Error;
    }
Error:
    return hr;
}

HRESULT     CopyAlpha(CIssGDIEx& gdiDest, 
                      RECT rcDest, 
                      CIssGDIEx& gdiSrc, 
                      int iXStart, 
                      int iYStart,
                      BOOL bAddAlphaVal
                      )
{
    return CopyAlpha(gdiDest, rcDest.left, rcDest.top, WIDTH(rcDest), HEIGHT(rcDest), gdiSrc, iXStart, iYStart, bAddAlphaVal);
}

HRESULT     CopyAlpha(CIssGDIEx& gdiDest, 
                      int xoriginDest,
                      int yoriginDest,
                      int wDest,
                      int hDest,
                      CIssGDIEx& gdiSrc, 
                      int xoriginSrc, 
                      int yoriginSrc,
                      BOOL bAddAlphaVal
                      )
{
    HRESULT hr = S_OK;

    if(gdiSrc.GetAlpha() == NULL)
        goto Error;     // no need to go further

    CBARG(gdiDest.GetDC()&&gdiSrc.GetDC(), _T("gdiDest.GetDC()&&gdiSrc.GetDC()"));

    hr = EnsureLocation(gdiDest, xoriginDest, yoriginDest, wDest, hDest, gdiSrc, xoriginSrc, yoriginSrc);

    if(!gdiDest.GetAlpha())
    {
        hr = gdiDest.InitAlpha(TRUE);
        CHR(hr, _T("gdiDest.InitAlpha()"));
    }

    int iYOffsetSrc = 0;
    int iYOffsetDest= 0;
    BYTE btSrc, btDest;

    LPBYTE lpSrc    = ((LPBYTE)gdiSrc.GetAlpha() + gdiSrc.GetWidth()*(gdiSrc.GetHeight() - (yoriginSrc+hDest)));
    LPBYTE lpDest   = ((LPBYTE)gdiDest.GetAlpha() + gdiDest.GetWidth()*(gdiDest.GetHeight() - (yoriginDest+hDest)));

    for(int y=0; y<hDest && y+yoriginDest<gdiDest.GetHeight() && y+yoriginSrc<gdiSrc.GetHeight(); y++)
    {
        for(int x=0; x<wDest && x+xoriginDest<gdiDest.GetWidth() && x+xoriginSrc<gdiSrc.GetWidth(); x++)
        {
            btSrc    = lpSrc[iYOffsetSrc+x+xoriginSrc];
            btDest    = lpDest[iYOffsetDest+x+xoriginDest];

            if(btSrc > btDest || !bAddAlphaVal)
                lpDest[iYOffsetDest+x + xoriginDest] = btSrc;
        }
        iYOffsetSrc += gdiSrc.GetWidth();
        iYOffsetDest+= gdiDest.GetWidth();
    }


Error:
    return hr;
}

HRESULT AdjustThemeColors(CIssGDIEx& gdi, COLORREF crLo, COLORREF crHi)
{
    HRESULT hr = S_OK;

    CBARG(gdi.GetDC(), _T("no DC for AdjustThemeColors"));
    CBARG(gdi.GetBits(), _T("no bits for AdjustThemeColors"));

    switch(gdi.GetBPP())
    {
    case 16:
        hr = AdjustThemeColors16(gdi, crLo, crHi);
        break;
    default:
        hr = E_INVALIDARG;
    }
    CHR(hr, _T("AdjustThemeColors"));

Error:
    return hr;
}

HRESULT AdjustThemeColors16(CIssGDIEx& gdi, COLORREF crLo, COLORREF crHi)
{
    HRESULT hr = S_OK;

    WORD* lprgbDest = (WORD*)gdi.GetBits();
    WORD  wColor;
    WORD  wR, wG, wB;
    WORD  wRLo,wGLo,wBLo;
    WORD  wRHi, wGHi, wBHi;
    DWORD dwWidthBytesDest    = (DWORD)gdi.GetBitmapInfo().bmWidthBytes;

    // separate out the color values
    wRLo    = GetRValue(crLo)*31/255;
    wGLo    = GetGValue(crLo)*63/255;
    wBLo    = GetBValue(crLo)*31/255;
    wRHi    = GetRValue(crHi)*31/255;
    wGHi    = GetGValue(crHi)*63/255;
    wBHi    = GetBValue(crHi)*31/255;

    for(int y=0; y<gdi.GetHeight(); y++)
    {
        for(int x=0; x<gdi.GetWidth(); x++)
        {
            wColor	= lprgbDest[x];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);

            wR = wG>>1;
            wB = wG>>1;

            wR   = (BYTE)((((WORD)wRLo * (31-wR)) + ((WORD)wRHi * wR)) >> 5);
            wG   = (BYTE)((((WORD)wGLo * (63-wG)) + ((WORD)wGHi * wG)) >> 6);
            wB   = (BYTE)((((WORD)wBLo * (31-wB)) + ((WORD)wBHi * wB)) >> 5);

            /*if(wG > 31)
            {
                wR = (wR-15)*2;
                wG = (wG-31)*2;
                wB = (wB-15)*2;
                wR   = (BYTE)((((WORD)wRLo * (31-wR)) + ((WORD)0 * wR)) >> 5);
                wG   = (BYTE)((((WORD)wGLo * (63-wG)) + ((WORD)0 * wG)) >> 6);
                wB   = (BYTE)((((WORD)wBLo * (31-wB)) + ((WORD)0 * wB)) >> 5);
            }
            else
            {
                wR*=2;
                wG*=2;
                wB*=2;
                wR   = (BYTE)((((WORD)wRLo * (31-wR)) + ((WORD)31 * wR)) >> 5);
                wG   = (BYTE)((((WORD)wGLo * (63-wG)) + ((WORD)63 * wG)) >> 6);
                wB   = (BYTE)((((WORD)wBLo * (31-wB)) + ((WORD)31 * wB)) >> 5);
            }*/

            lprgbDest[x] = (WORD)(wR<<11|wG<<5|wB);
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
    }

//Error:
    return hr;
}

HRESULT DrawWithAlpha(CIssGDIEx& gdiDest,
                        int xoriginDest, 
                        int yoriginDest, 
                        int wDest, 
                        int hDest, 
                        CIssGDIEx& gdiSrc,
                        int xoriginSrc, 
                        int yoriginSrc,
                        int iAlpha
                        )
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetBPP()==gdiSrc.GetBPP(), _T("gdiDest.GetBPP()==gdiSrc.GetBPP()"));

    switch(gdiDest.GetBPP())
    {
    case 16:
        hr = DrawWithAlpha16(gdiDest, xoriginDest, yoriginDest, wDest, hDest, gdiSrc, xoriginSrc, yoriginSrc, iAlpha);
        break;
    case 24:
        hr = DrawWithAlpha24(gdiDest, xoriginDest, yoriginDest, wDest, hDest, gdiSrc, xoriginSrc, yoriginSrc, iAlpha);
        break;
    default:
        hr = E_INVALIDARG;
    }
    //CHR(hr, _T("DrawWithAlpha"));

Error:
    return hr;
}

HRESULT DrawWithAlpha16(CIssGDIEx& gdiDest,
                           int xoriginDest, 
                           int yoriginDest, 
                           int wDest, 
                           int hDest, 
                           CIssGDIEx& gdiSrc,
                           int xoriginSrc, 
                           int yoriginSrc,
                           int iAlpha)
{
    HRESULT hr = S_OK;

    LPVOID lpDest        = gdiDest.GetBits();
    LPVOID lpSrc        = gdiSrc.GetBits();
    LPBYTE lpAlpha        = gdiSrc.GetAlpha();
    CBARG(lpDest&&lpSrc&&lpAlpha&&gdiDest.GetBPP()==16&&gdiSrc.GetBPP()==16, _T("bad values with DrawWithAlpha"));

    hr = EnsureLocation(gdiDest, xoriginDest, yoriginDest, wDest, hDest, gdiSrc, xoriginSrc, yoriginSrc);
    //CHR(hr, _T("EnsureLocation failed"));
    if(hr != S_OK)
        goto Error;

    WORD*        lprgbDest;
    WORD*        lprgbSrc;
    WORD        wColor;

    WORD        wR,wG,wB;
    WORD        wRD, wGD, wBD;
    DWORD        dwWeight1;//        = 255 - uiAlpha;
    DWORD        dwWeight2;//        = uiAlpha;
    int         iYOffset = 0;

    DWORD        dwWidthBytesDest    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;
    DWORD        dwWidthBytesSrc        = (DWORD)gdiSrc.GetBitmapInfo().bmWidthBytes;

    lprgbDest  = (WORD*)((LPBYTE)lpDest + dwWidthBytesDest*(gdiDest.GetHeight() - (yoriginDest+hDest)));
    lprgbSrc   = (WORD*)((LPBYTE)lpSrc  + dwWidthBytesSrc*(gdiSrc.GetHeight() - (yoriginSrc+hDest)));
    lpAlpha    = ((LPBYTE)lpAlpha+ gdiSrc.GetWidth()*(gdiSrc.GetHeight() - (yoriginSrc+hDest)));

    for(int y=0; y<hDest && y+yoriginDest<gdiDest.GetHeight() && y+yoriginSrc<gdiSrc.GetHeight(); y++)
    {
        for(int x=0; x<wDest && x+xoriginDest<gdiDest.GetWidth() && x+xoriginSrc<gdiSrc.GetWidth(); x++)
        {
            dwWeight1    = (DWORD)lpAlpha[iYOffset+x+xoriginSrc];

            // if we have a global alpha scale
            if(iAlpha > -1 && iAlpha < 255)
                dwWeight1 = dwWeight1*iAlpha/255;

            dwWeight2    = ALPHA_Full - dwWeight1;
            if(dwWeight1 < 5)
                continue;
            else if(dwWeight1 > 250) // ALPHA_Full)
                lprgbDest[x+xoriginDest] = lprgbSrc[x+xoriginSrc];

            //dwWeight1= dwWeight1>>3;    // a little bit of accuracy is lost here 
            //dwWeight2= dwWeight2>>3;

            wColor    = lprgbDest[x+xoriginDest];
            wRD = (WORD)((wColor >> 11) & 0x1f);
            wGD = (WORD)((wColor >> 5) & 0x3f);
            wBD = (WORD)(wColor & 0x1f);

            wColor    = lprgbSrc[x+xoriginSrc];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);

            wRD   = (BYTE)((((WORD)wR * dwWeight1>>3) + ((WORD)wRD * dwWeight2>>3)) >> 5);
            wGD   = (BYTE)((((WORD)wG * dwWeight1>>2) + ((WORD)wGD * dwWeight2>>2)) >> 6);
            wBD   = (BYTE)((((WORD)wB * dwWeight1>>3) + ((WORD)wBD * dwWeight2>>3)) >> 5);
            lprgbDest[x+xoriginDest] = (WORD)(wRD<<11|wGD<<5|wBD);
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytesSrc);
        iYOffset += gdiSrc.GetWidth();
    }

Error:
    return hr;
}

HRESULT DrawWithAlpha24(CIssGDIEx& gdiDest,
                        int xoriginDest, 
                        int yoriginDest, 
                        int wDest, 
                        int hDest, 
                        CIssGDIEx& gdiSrc,
                        int xoriginSrc, 
                        int yoriginSrc,
                        int iAlpha
                        )
{
    HRESULT hr = S_OK;

    LPVOID lpDest        = gdiDest.GetBits();
    LPVOID lpSrc        = gdiSrc.GetBits();
    LPBYTE lpAlpha        = gdiSrc.GetAlpha();
    CBARG(lpDest&&lpSrc&&lpAlpha&&gdiDest.GetBPP()==24&&gdiSrc.GetBPP()==24, _T("bad values with DrawWithAlpha"));

    hr = EnsureLocation(gdiDest, xoriginDest, yoriginDest, wDest, hDest, gdiSrc, xoriginSrc, yoriginSrc);
    //CHR(hr, _T("EnsureLocation failed"));
    if(hr != S_OK)
        goto Error;

    RGBTRIPLE*        lprgbDest;
    RGBTRIPLE*        lprgbSrc;

    DWORD        dwWeight1;//        = 255 - uiAlpha;
    DWORD        dwWeight2;//        = uiAlpha;
    int         iYOffset = 0;

    DWORD        dwWidthBytesDest    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;
    DWORD        dwWidthBytesSrc        = (DWORD)gdiSrc.GetBitmapInfo().bmWidthBytes;

    lprgbDest  = (RGBTRIPLE*)((LPBYTE)lpDest + dwWidthBytesDest*(gdiDest.GetHeight() - (yoriginDest+hDest)));
    lprgbSrc   = (RGBTRIPLE*)((LPBYTE)lpSrc  + dwWidthBytesSrc*(gdiSrc.GetHeight() - (yoriginSrc+hDest)));
    lpAlpha    = ((LPBYTE)lpAlpha+ gdiSrc.GetWidth()*(gdiSrc.GetHeight() - (yoriginSrc+hDest)));

    for(int y=0; y<hDest && y+yoriginDest<gdiDest.GetHeight() && y+yoriginSrc<gdiSrc.GetHeight(); y++)
    {
        for(int x=0; x<wDest && x+xoriginDest<gdiDest.GetWidth() && x+xoriginSrc<gdiSrc.GetWidth(); x++)
        {
            dwWeight1    = (DWORD)lpAlpha[iYOffset+x+xoriginSrc];

            // if we have a global alpha scale
            if(iAlpha > -1 && iAlpha < 255)
                dwWeight1 = dwWeight1*iAlpha/255;

            dwWeight2    = ALPHA_Full - dwWeight1;
            if(dwWeight1 == 0)
                continue;
            else if(dwWeight1 == ALPHA_Full)
            {
                lprgbDest[x+xoriginDest].rgbtRed = lprgbSrc[x+xoriginSrc].rgbtRed;
                lprgbDest[x+xoriginDest].rgbtGreen = lprgbSrc[x+xoriginSrc].rgbtBlue;
                lprgbDest[x+xoriginDest].rgbtBlue = lprgbSrc[x+xoriginSrc].rgbtBlue;
            }

            lprgbDest[x+xoriginDest].rgbtRed   = (BYTE)((((DWORD)lprgbSrc[x+xoriginSrc].rgbtRed * dwWeight1) + ((DWORD)lprgbDest[x+xoriginDest].rgbtRed * dwWeight2)) >> 8);
            lprgbDest[x+xoriginDest].rgbtGreen   = (BYTE)((((DWORD)lprgbSrc[x+xoriginSrc].rgbtGreen * dwWeight1) + ((DWORD)lprgbDest[x+xoriginDest].rgbtGreen * dwWeight2)) >> 8);
            lprgbDest[x+xoriginDest].rgbtBlue   = (BYTE)((((DWORD)lprgbSrc[x+xoriginSrc].rgbtBlue * dwWeight1) + ((DWORD)lprgbDest[x+xoriginDest].rgbtBlue * dwWeight2)) >> 8);
        }
        lprgbDest = (RGBTRIPLE *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (RGBTRIPLE *)((LPBYTE)lprgbSrc + dwWidthBytesSrc);
        iYOffset += gdiSrc.GetWidth();
    }

Error:
    return hr;
}

/********************************************************************
Function:    FillRect

Arguments:    See Win32 documentation

Returns:    TRUE if successful

Comments:    Simplifies fillrect calls over the Win32 version
*********************************************************************/
HRESULT FillRect(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crColor)
{
    return FillRect(gdi.GetDC(), rcDraw, crColor);
}

HRESULT FillRect(HDC hDC, RECT& rcDraw, COLORREF crColor)
{
    HRESULT hr = S_OK;

    HBRUSH    hbr        = CreateSolidBrush(crColor);
    CPHR(hbr, _T("CreateSolidBrush"));
    ::FillRect(hDC, &rcDraw, hbr);
Error:
    if(hbr)
        DeleteObject((HBRUSH)hbr);
    return hr;
}

HRESULT EnsureLocation( CIssGDIEx& gdiDest, 
                        int& xoriginDest,
                        int& yoriginDest,
                        int& wDest,
                        int& hDest,
                        CIssGDIEx& gdiSrc, 
                        int& xoriginSrc, 
                        int& yoriginSrc
                        )
{
    HRESULT hr = S_OK;

    // make sure the src and dst drawing will be above zero
    if(xoriginDest < 0)
    {
        xoriginSrc    = xoriginSrc - xoriginDest;
        wDest        = wDest + xoriginDest;
        xoriginDest = 0;        
    }

    if(yoriginDest < 0)
    {
        yoriginSrc    = yoriginSrc - yoriginDest;
        hDest        = hDest + yoriginDest;
        yoriginDest = 0;
    }

    if(xoriginDest + wDest > gdiDest.GetWidth())
    {
        wDest = gdiDest.GetWidth() - xoriginDest;
    }

    if(yoriginDest + hDest > gdiDest.GetHeight())
    {
        hDest = gdiDest.GetHeight() - yoriginDest;
    }

    if(xoriginSrc + wDest > gdiSrc.GetWidth())
    {
        wDest = gdiSrc.GetWidth();
    }

    if(yoriginSrc + hDest > gdiSrc.GetHeight())
    {
        hDest = gdiSrc.GetHeight();
    }

    if(!(gdiDest.GetWidth() >= xoriginDest))
        hr = E_INVALIDARG;
    else if(!(gdiDest.GetHeight() >= yoriginDest))
        hr = E_INVALIDARG;
    else if(!(gdiSrc.GetWidth() >= (wDest - xoriginSrc)))
        hr = E_INVALIDARG;
    else if(!(gdiSrc.GetHeight() >= (hDest - yoriginSrc)))
        hr = E_INVALIDARG;
    else if(!(xoriginSrc >= 0))
        hr = E_INVALIDARG;
    else if(!(yoriginSrc >= 0))
        hr = E_INVALIDARG;
//Error:
    return hr;

}

HRESULT DrawTextShadow(HDC hDC, 
                           TCHAR* szText,
                           RECT& rc, 
                           UINT    uiFormat, 
                           HFONT hFont, 
                           COLORREF crColor, 
                           COLORREF crShadow,
                           int    iBkMode
                           )
{
    HRESULT hr = S_OK;

    OffsetRect(&rc, 1, 1);
    hr = DrawText(hDC, szText, rc, uiFormat, hFont, crShadow, iBkMode);
    CHR(hr, _T("DrawText"));

    OffsetRect(&rc, -1, -1);
    hr = DrawText(hDC, szText, rc, uiFormat, hFont, crColor, iBkMode);
    CHR(hr, _T("DrawText"));

Error:
    return hr;
}

HRESULT DrawTextShadow(HDC hDC, 
                           TCHAR* szText,
                           int iXPos, 
                           int iYpos, 
                           UINT    uiFormat, 
                           HFONT hFont, 
                           COLORREF crColor, 
                           COLORREF crShadow,
                           int    iBkMode
                           )
{
    HRESULT hr = S_OK;

    iXPos++; iYpos++;
    hr = DrawText(hDC, szText, iXPos, iYpos, uiFormat, hFont, crShadow, iBkMode);
    CHR(hr, _T("DrawText"));

    iXPos--; iYpos--;
    hr = DrawText(hDC, szText, iXPos, iYpos, uiFormat, hFont, crColor, iBkMode);
    CHR(hr, _T("DrawText"));

Error:
    return hr;
}

/********************************************************************************************

Draw some text to the DC provided

hDC			- dc to draw to
szText		- Text to use
rc			- bounding rectangle to draw in
uiFormat	- any combination of DT_LEFT DT_RIGHT DT_BOTTOM DT_TOP DT_CENTER DT_VCENTER DT_WORDBREAK
hFont		- you can provide a font if you like
crColor		- text color (255,0,255) is considered unset
iBkMode		- background mode, TRANSPARENT is default, or OPAQUE

********************************************************************************************/
HRESULT DrawText(HDC hDC,TCHAR* szText, RECT& rc, UINT    uiFormat, HFONT hFont, COLORREF crColor, int iBkMode)
{
#ifndef UNDER_CE
	if(uiFormat & DT_BOTTOM || uiFormat & DT_VCENTER)
		uiFormat = uiFormat | DT_SINGLELINE;
#endif

    HRESULT hr          = S_OK;
    COLORREF OldColour  = TRANSPARENT_COLOR;
    HFONT hOldFont      = NULL;
    int iOldBackMode    = -1;
    TCHAR* szDrawText   = NULL;

    CIssString* oStr = CIssString::Instance();
    CBARG(hDC&&szText&&rc.right>=rc.left&&rc.bottom>=rc.top, _T(""));

    // set the drawing options
    if(hFont)
        hOldFont = (HFONT)SelectObject(hDC, hFont);    

    if(crColor != TRANSPARENT_COLOR)
        OldColour = ::SetTextColor(hDC, crColor);

    iOldBackMode = SetBkMode(hDC, iBkMode);

    if(DT_END_ELLIPSIS & uiFormat)
    {
        SIZE sTemp;
        GetTextExtentPoint(hDC, szText, oStr->GetLength(szText), &sTemp);

        if((sTemp.cx > rc.right - rc.left)) //our text is too long .. so do something about it
        {
            szDrawText = oStr->CreateAndCopy(szText);
            CPHR(szDrawText, _T("szDrawText"));

            //for(int j = 1; j < oStr->GetLength(szDrawText)-4; j++)//just some loops..it should never max out (unless your RECT is wrong...)
			int iLen = 0;

            while(TRUE)
            {
				iLen = oStr->GetLength(szDrawText);
				if(iLen < 5)
					break;
                szDrawText[iLen-2] = _T('.');
                szDrawText[iLen-3] = _T('.');
                szDrawText[iLen-4] = _T('.');
                szDrawText[iLen-1] = _T('\0');
                GetTextExtentPoint(hDC, szDrawText, oStr->GetLength(szDrawText), &sTemp);
                if(sTemp.cx <= rc.right - rc.left)
                    break;//we fit!
            }
            uiFormat -= DT_END_ELLIPSIS;
            ::DrawText(hDC, szDrawText, -1, &rc, uiFormat);
        }
        else
        {
            uiFormat -= DT_END_ELLIPSIS;
            ::DrawText(hDC, szText, -1, &rc, uiFormat);
        }
    }
    else
    {
        // draw the text
        ::DrawText(hDC, szText, -1, &rc, uiFormat);
    }     

Error:
    oStr->Delete(&szDrawText);

    // reset all the old stuff back
    if(iOldBackMode != -1)
        SetBkMode(hDC, iOldBackMode);

    if(OldColour != TRANSPARENT_COLOR)
        ::SetTextColor(hDC, OldColour);

    if(hOldFont)
        SelectObject(hDC, hOldFont);  

    return hr;
}

/********************************************************************************************

Draw some text to the DC provided

Same as above except location is specified by iXPos and iYPos.
Width and Height are determined by the text content and the given font

********************************************************************************************/
HRESULT DrawText(HDC hDC, TCHAR* szText,int iXPos, int iYpos, UINT uiFormat, HFONT hFont, COLORREF crColor, int iBkMode)
{
    RECT rcSize;
    SIZE sSize;

    // first we have to select the new font into the DC if it's needed
    HFONT hOldFont = (hFont?(HFONT)SelectObject(hDC, hFont):NULL);

    GetTextExtentPoint(hDC, szText, (int)_tcslen(szText), &sSize);
    sSize.cx += 2;        // BUGBUG: for some reason this causes a warning for the compiler

    // set up the rect according to the text Extents
    rcSize.top        = iYpos;
    rcSize.bottom    = rcSize.top + sSize.cy;

    if(uiFormat == DT_RIGHT)
    {
        rcSize.left        = iXPos - sSize.cx;
        rcSize.right    = iXPos;
    }
    else if(uiFormat == DT_CENTER)
    {
        rcSize.left        = iXPos - sSize.cx/2;
        rcSize.right    = iXPos + sSize.cx/2;
    }
    else    //left
    {
        rcSize.left        = iXPos;
        rcSize.right    = rcSize.left + sSize.cx;
    }

    // set the font back 
    if(hOldFont)
        SelectObject(hDC, hOldFont);

    return DrawText(hDC, szText, rcSize, DT_LEFT, hFont, crColor, iBkMode);

}

/****************************************************

TileBackGround

RECT rc        Pass in Window Rect so we can correct for tile alignment among child windows

bRelativeRect...is case you want to draw to a rect
that is smaller than your HDC..otherwise it assumes
the hdc is the same size

****************************************************/
HRESULT TileBackGround(CIssGDIEx& gdiDest, RECT& rc, CIssGDIEx& gdiTexture, BOOL bRelativeRect)
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC()&&gdiTexture.GetDC(), _T("gdiDest.GetDC()&&gdiTexture.GetDC()"));

    int iY;//top location to draw texture
    int iX;//left location to draw texture
    int iItemHeight;//height of texture to copy
    int iItemWidth;//width of texture to copy

    int iHeightToDraw;
    int iWidthToDraw;
    RECT rcDraw;

    iY = rc.top;

    int iTW = gdiTexture.GetHeight();
    int iTH = gdiTexture.GetWidth();

    do 
    {
        iItemHeight    = gdiTexture.GetHeight() - iY % gdiTexture.GetHeight();

        iX = rc.left;

        do 
        {
            iItemWidth = gdiTexture.GetWidth() - iX % gdiTexture.GetWidth();

            //set the height and width based off of the amount of texture to draw
            iWidthToDraw    = iItemWidth;
            iHeightToDraw    = iItemHeight;

            //now check if we're drawing outside of our rect
            if(iWidthToDraw + iX > rc.right)
                iWidthToDraw = rc.right - iX;
            if(iHeightToDraw + iY > rc.bottom)
                iHeightToDraw = rc.bottom - iY;

            BitBlt(gdiDest.GetDC(),
                iX - (bRelativeRect?0:rc.left), 
                iY - (bRelativeRect?0:rc.top),
                iWidthToDraw, 
                iHeightToDraw,
                gdiTexture.GetDC(),
                (iItemWidth == 0?0:gdiTexture.GetWidth() - iItemWidth),
                (iItemHeight== 0?0:gdiTexture.GetHeight() - iItemHeight),
                SRCCOPY);

            // if there are alpha values
            if(gdiTexture.GetAlpha())
            {
                rcDraw.left     = iX - (bRelativeRect?0:rc.left);
                rcDraw.top      = iY - (bRelativeRect?0:rc.top);
                rcDraw.right    = rcDraw.left + iWidthToDraw;
                rcDraw.bottom   = rcDraw.top + iHeightToDraw;
                CopyAlpha(gdiDest,
                          rcDraw.left, rcDraw.top,
                          WIDTH(rcDraw), HEIGHT(rcDraw),
                          gdiTexture,
                          (iItemWidth == 0?0:gdiTexture.GetWidth() - iItemWidth),
                          (iItemHeight== 0?0:gdiTexture.GetHeight() - iItemHeight)
                          );
            }

            iX += (iItemWidth == 0?gdiTexture.GetWidth():iItemWidth);

        } while(iX < rc.right);

        iY += (iItemHeight == 0?gdiTexture.GetHeight():iItemHeight);

    } while(iY < rc.bottom);

Error:
    return hr;
}

HRESULT FillRectAlpha(CIssGDIEx& gdiDest, RECT& rcDraw, BYTE btAlpha)
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC(), _T("gdiDest.GetDC() is NULL"));

    if(!gdiDest.GetAlpha())
    {
        hr = gdiDest.InitAlpha();
        CHR(hr, _T("gdiDest.InitAlpha()"));
    }

    int iYOffsetDest= 0;

    LPBYTE lpDest   = ((LPBYTE)gdiDest.GetAlpha() + gdiDest.GetWidth()*(gdiDest.GetHeight() - (rcDraw.top+HEIGHT(rcDraw))));

    for(int y=0; y<HEIGHT(rcDraw) && y+rcDraw.top<gdiDest.GetHeight(); y++)
    {
        for(int x=0; x<WIDTH(rcDraw) && x+rcDraw.left<gdiDest.GetWidth(); x++)
        {
            lpDest[iYOffsetDest+x+rcDraw.left] = btAlpha;
        }
        iYOffsetDest+= gdiDest.GetWidth();
    }

Error:
    return hr;
}

HRESULT AlphaFillRect(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha)
{
    HRESULT hr = S_OK;

    switch(gdi.GetBPP())
    {
    case 16:
        hr = AlphaFillRect16(gdi, rcDraw, crAlpha, uiAlpha);
        break;
    case 24:
    case 32:
        hr = AlphaFillRect24(gdi, rcDraw, crAlpha, uiAlpha);
        break;
    default:
        hr = E_INVALIDARG;
    }
    CHR(hr, _T("AlphaFillRect"));

Error:
    return hr;
}

HRESULT AlphaFillRect16(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha)
{
    HRESULT hr = S_OK;
    CBARG(gdi.GetBits(), _T("no bits for AlphaFillRect16"));

    if(uiAlpha > 255)
        uiAlpha = 255;

    if(rcDraw.left < 0)
        rcDraw.left = 0;
    if(rcDraw.top < 0)
        rcDraw.top = 0;
    if(rcDraw.right > gdi.GetWidth())
        rcDraw.right = gdi.GetWidth();
    if(rcDraw.bottom > gdi.GetHeight())
        rcDraw.bottom = gdi.GetHeight();

    WORD*    lprgbSrc;
    DWORD        dwWeight1        = 255 - uiAlpha;
    DWORD        dwWeight2        = uiAlpha;
    DWORD        dwWidthBytes    = (DWORD)gdi.GetBitmapInfo().bmWidthBytes;
    WORD        wR,wG,wB,wColor;

    // Initialize the surface pointers.
    lprgbSrc  = (WORD *)((LPBYTE)gdi.GetBits()  + dwWidthBytes*(gdi.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    for (int y=0; y<(rcDraw.bottom - rcDraw.top); y++) 
    {
        for (int x=rcDraw.left; x<rcDraw.right; x++) 
        {
            wColor    = lprgbSrc[x];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);
            wR <<= 3;
            wG <<= 2;
            wB <<= 3;
            if(RGB(wR,wG,wB)== TRANSPARENT_COLOR)
                continue;//for images with transparencies

            wR  = (BYTE)((((DWORD)wR * dwWeight1) + ((DWORD)GetRValue(crAlpha) * dwWeight2)) >> 8);
            wG    = (BYTE)((((DWORD)wG * dwWeight1) + ((DWORD)GetGValue(crAlpha) * dwWeight2)) >> 8);
            wB  = (BYTE)((((DWORD)wB * dwWeight1) + ((DWORD)GetBValue(crAlpha)* dwWeight2)) >> 8);
            wColor  = (WORD)((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3);
            lprgbSrc[x] = wColor;
        }

        // Move to next scan line.
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

Error:
    return hr;
}



HRESULT AlphaFillRect24(CIssGDIEx& gdi, RECT& rcDraw, COLORREF crAlpha, UINT uiAlpha)
{
    HRESULT hr = S_OK;
    CBARG(gdi.GetBits(), _T("no bits for AlphaFillRect24"));

    if(uiAlpha > 255)
        uiAlpha = 255;

    if(rcDraw.left < 0)
        rcDraw.left = 0;
    if(rcDraw.top < 0)
        rcDraw.top = 0;
    if(rcDraw.right > gdi.GetWidth())
        rcDraw.right = gdi.GetWidth();
    if(rcDraw.bottom > gdi.GetHeight())
        rcDraw.bottom = gdi.GetHeight();

    RGBTRIPLE*    lprgbSrc;
    DWORD        dwWeight1        = 255 - uiAlpha;
    DWORD        dwWeight2        = uiAlpha;
    DWORD        dwWidthBytes    = (DWORD)gdi.GetBitmapInfo().bmWidthBytes;
    WORD wR,wG,wB;

    // Initialize the surface pointers.
    lprgbSrc  = (RGBTRIPLE *)((LPBYTE)gdi.GetBits()  + dwWidthBytes*(gdi.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    for (int y=0; y<(rcDraw.bottom - rcDraw.top); y++) 
    {
        for (int x=rcDraw.left; x<rcDraw.right; x++) 
        {
            wR = lprgbSrc[x].rgbtRed;
            wG = lprgbSrc[x].rgbtGreen;
            wB = lprgbSrc[x].rgbtBlue;

            if(RGB(wR,wG,wB)== TRANSPARENT_COLOR)
                continue;//for images with transparencies

            lprgbSrc[x].rgbtRed  = (BYTE)((((DWORD)wR * dwWeight1) + ((DWORD)GetRValue(crAlpha) * dwWeight2)) >> 8);
            lprgbSrc[x].rgbtGreen = (BYTE)((((DWORD)wG * dwWeight1) + ((DWORD)GetGValue(crAlpha) * dwWeight2)) >> 8);
            lprgbSrc[x].rgbtBlue  = (BYTE)((((DWORD)wB * dwWeight1) + ((DWORD)GetBValue(crAlpha)* dwWeight2)) >> 8);
        }

        // Move to next scan line.
        lprgbSrc = (RGBTRIPLE *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

Error:
    return hr;
}

HRESULT RoundRectangle(HDC hdc, RECT& rcDraw, COLORREF crColor, COLORREF crOutline, int iRoundWidth, int iRoundHeight)
{
    HPEN oldPen;
    HPEN newPen;

    HBRUSH oldBrush;
    HBRUSH newBrush;

    newPen = ::CreatePen(PS_SOLID, 1, crOutline);
    if(crOutline == RGB(255,4,255))
        oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
    else
        oldPen = (HPEN)SelectObject(hdc, newPen);

    newBrush = CreateSolidBrush(crColor);

    if(crColor == RGB(255,4,255))
        oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    else
        oldBrush = (HBRUSH)SelectObject(hdc, newBrush);

    RoundRect(hdc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, iRoundWidth, iRoundHeight);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(newPen);
    DeleteObject(newBrush);

    return S_OK;
}

HRESULT ShinyRoundRectangle(CIssGDIEx& gdiDest,    
                                  RECT& rcDraw, 
                                  COLORREF crOutline, 
                                  int iRoundWidth, int iRoundHeight, 
                                  COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                                  )
{
    HRESULT hr = S_OK;

    switch(gdiDest.GetBPP())
    {
    case 16:
        hr = ShinyRoundRectangle16(gdiDest, rcDraw, crOutline, iRoundWidth, iRoundHeight, crGrad1, crGrad2, crGrad3, crGrad4);
        break;
    case 24:
    case 32:
        hr = ShinyRoundRectangle24(gdiDest, rcDraw, crOutline, iRoundWidth, iRoundHeight, crGrad1, crGrad2, crGrad3, crGrad4);
        break;
    default:
        hr = E_INVALIDARG;
    }
    CHR(hr, _T("DrawShinyRoundRectangle16"));

Error:
    return hr;
}

HRESULT ShinyRoundRectangle24(CIssGDIEx& gdiDest,    
                              RECT& rcDraw, 
                              COLORREF crOutline, 
                              int iRoundWidth, int iRoundHeight, 
                              COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                              )
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    //First we draw our round 
    hr = RoundRectangle(gdiDest.GetDC(), rcDraw, NULL_COLOR, crOutline, iRoundWidth, iRoundHeight);
    CHR(hr, _T("DrawRoundRectangle failed"));

    WORD wR, wG, wB;

    wR = GetRValue(crGrad2) + (WORD)((255.0 - GetRValue(crGrad2)) * 0.2);
    wG = GetGValue(crGrad2) + (WORD)((255.0 - GetGValue(crGrad2)) * 0.2);
    wB = GetBValue(crGrad2) + (WORD)((255.0 - GetBValue(crGrad2)) * 0.2);

    RGBTRIPLE*    lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));

        if(i > (rcDraw.bottom - rcDraw.top)/2)
        {
            dbWeight *= 2;
        }
        else
        {
            dbWeight -= 0.5;
            dbWeight *= 2;
        }
        dwWeight1 = (DWORD)(dbWeight*255);
        dwWeight2 = 255 - dwWeight1;


        for (int x=rcDraw.left; x<rcDraw.right; x++)
        {
            //check if the pixel is transparent
            if( lprgbSrc[x].rgbtRed == GetRValue(NULL_COLOR) &&
                lprgbSrc[x].rgbtGreen == GetGValue(NULL_COLOR) &&
                lprgbSrc[x].rgbtBlue == GetBValue(NULL_COLOR))
            {
                if(i > (rcDraw.bottom - rcDraw.top)/2)//if we're less than half way
                {
                    lprgbSrc[x].rgbtRed = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
                    lprgbSrc[x].rgbtGreen = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
                    lprgbSrc[x].rgbtBlue = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) + ((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
                }
                else
                {
                    lprgbSrc[x].rgbtRed = (BYTE)((((DWORD)GetRValue(crGrad4) * dwWeight1) + ((DWORD)GetRValue(crGrad3) * dwWeight2)) >> 8);
                    lprgbSrc[x].rgbtGreen = (BYTE)((((DWORD)GetGValue(crGrad4) * dwWeight1) + ((DWORD)GetGValue(crGrad3) * dwWeight2)) >> 8);
                    lprgbSrc[x].rgbtBlue = (BYTE)((((DWORD)GetBValue(crGrad4) * dwWeight1) + ((DWORD)GetBValue(crGrad3) * dwWeight2)) >> 8);
                }
            }
        }
        // Move to next scan line.
        lprgbSrc = (RGBTRIPLE *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }


Error:
    return hr;


}


HRESULT ShinyRoundRectangle16(CIssGDIEx& gdiDest,    
                                  RECT& rcDraw, 
                                  COLORREF crOutline, 
                                  int iRoundWidth, int iRoundHeight, 
                                  COLORREF crGrad1, COLORREF crGrad2, COLORREF crGrad3, COLORREF crGrad4
                                  )
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    //First we draw our round 
    hr = RoundRectangle(gdiDest.GetDC(), rcDraw, NULL_COLOR, crOutline, iRoundWidth, iRoundHeight);
    CHR(hr, _T("DrawRoundRectangle failed"));

    int iR, iG, iB;

    iR = GetRValue(crGrad2) + (int)((255.0 - GetRValue(crGrad2)) * 0.2);
    iG = GetGValue(crGrad2) + (int)((255.0 - GetGValue(crGrad2)) * 0.2);
    iB = GetBValue(crGrad2) + (int)((255.0 - GetBValue(crGrad2)) * 0.2);

    //similar to DrawGradient    
    WORD*        lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;
    WORD        wR,wG,wB,wColor;

    // Initialize the surface pointers.
    lprgbSrc  = (WORD *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));

        if(i > (rcDraw.bottom - rcDraw.top)/2)
        {
            dbWeight *= 2;
        }
        else
        {
            dbWeight -= 0.5;
            dbWeight *= 2;
        }
        dwWeight1 = (DWORD)(dbWeight*255);
        dwWeight2 = 255 - dwWeight1;


        for (int x=rcDraw.left; x<rcDraw.right; x++)
        {
            //check if the pixel is transparent

            wColor    = lprgbSrc[x];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);
            wR <<= 3;
            wG <<= 2;
            wB <<= 3;
            if(RGB(wR,wG,wB) == NULL_COLOR)
            {
                if(i > (rcDraw.bottom - rcDraw.top)/2)//if we're less than half way
                {
                    wR = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
                    wG = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
                    wB = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) + ((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
                }
                else
                {
                    wR = (BYTE)((((DWORD)GetRValue(crGrad4) * dwWeight1) + ((DWORD)GetRValue(crGrad3) * dwWeight2)) >> 8);
                    wG = (BYTE)((((DWORD)GetGValue(crGrad4) * dwWeight1) + ((DWORD)GetGValue(crGrad3) * dwWeight2)) >> 8);
                    wB = (BYTE)((((DWORD)GetBValue(crGrad4) * dwWeight1) + ((DWORD)GetBValue(crGrad3) * dwWeight2)) >> 8);
                }

                wColor  = (WORD)((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3);
                lprgbSrc[x] = wColor;
            }
        }
        // Move to next scan line.
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

Error:
    return hr;
}

HRESULT GradientFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2, BOOL bUseIssVersion)
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC(), _T(""));

    if(bUseIssVersion)
    {
        switch(gdiDest.GetBPP())
        {
        case 16:
        hr = GradientFillRect16(gdiDest, rcDraw, crGrad1, crGrad2);
        break;
        case 24:
        case 32:
        hr = GradientFillRect24(gdiDest, rcDraw, crGrad1, crGrad2);
        break;
        default:
        hr = E_INVALIDARG;
        break;
        }
        CHR(hr, _T("GradientFillRect failed"));
    }
    else
    {
        unsigned int Shift = 8;
        TRIVERTEX        vert[2] ;
        GRADIENT_RECT    gRect;
        vert [0] .x      = rcDraw.left;
        vert [0] .y      = rcDraw.top;
        vert [0] .Red    = GetRValue(crGrad1) << Shift;
        vert [0] .Green  = GetGValue(crGrad1) << Shift;
        vert [0] .Blue   = GetBValue(crGrad1) << Shift;
        vert [0] .Alpha  = 0x0000;
        vert [1] .x      = rcDraw.right;
        vert [1] .y      = rcDraw.bottom; 
        vert [1] .Red    = GetRValue(crGrad2) << Shift;
        vert [1] .Green  = GetGValue(crGrad2) << Shift;
        vert [1] .Blue   = GetBValue(crGrad2) << Shift;
        vert [1] .Alpha  = 0x0000;
        gRect.UpperLeft  = 0;
        gRect.LowerRight = 1;
        GradientFill(gdiDest.GetDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
    }

Error:
    return hr;
}

HRESULT GradientHorzFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2, BOOL bUseIssVersion)
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC(), _T(""));

    if(bUseIssVersion)
    {
        switch(gdiDest.GetBPP())
        {
        case 16:
        hr = GradientHorzFillRect16(gdiDest, rcDraw, crGrad1, crGrad2);
        break;
        case 24:
        case 32:
        hr = GradientHorzFillRect24(gdiDest, rcDraw, crGrad1, crGrad2);
        break;
        default:
        hr = E_INVALIDARG;
        break;
        }
        CHR(hr, _T("GradientHorzFillRect failed"));
    }
    else
    {
        unsigned int Shift = 8;
        TRIVERTEX        vert[2] ;
        GRADIENT_RECT    gRect;
        vert [0] .x      = rcDraw.left;
        vert [0] .y      = rcDraw.top;
        vert [0] .Red    = GetRValue(crGrad1) << Shift;
        vert [0] .Green  = GetGValue(crGrad1) << Shift;
        vert [0] .Blue   = GetBValue(crGrad1) << Shift;
        vert [0] .Alpha  = 0x0000;
        vert [1] .x      = rcDraw.right;
        vert [1] .y      = rcDraw.bottom; 
        vert [1] .Red    = GetRValue(crGrad2) << Shift;
        vert [1] .Green  = GetGValue(crGrad2) << Shift;
        vert [1] .Blue   = GetBValue(crGrad2) << Shift;
        vert [1] .Alpha  = 0x0000;
        gRect.UpperLeft  = 0;
        gRect.LowerRight = 1;
        GradientFill(gdiDest.GetDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
    }    

Error:
    return hr;
}

HRESULT GradientAngleFillRect(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2)
{
    HRESULT hr = S_OK;

    RECT rcTemp;
    rcTemp.left = 0;
    rcTemp.right = 1;
    rcTemp.top = 0;
    rcTemp.bottom = rcDraw.bottom - rcDraw.top + rcDraw.right - rcDraw.left;

    int iBPP = gdiDest.GetBPP();

    CIssGDIEx gdiTemp;
    gdiTemp.Create(gdiDest.GetDC(), rcTemp, FALSE, TRUE, FALSE, iBPP);

    GradientFillRect(gdiTemp, rcTemp, crGrad1, crGrad2); 

    // bitblt the rest for fast performance
    for(int j=0; j< rcDraw.right-rcDraw.left; j++)
    {
        BitBlt(gdiDest.GetDC(),
            j+rcDraw.left,
            rcDraw.top,
            1,
            (rcDraw.bottom - rcDraw.top),
            gdiTemp.GetDC(),
            0,
            j,
            SRCCOPY);        
    }
    return hr;
}

HRESULT GradientRadialFillRect(CIssGDIEx& gdiDest,  int iRadialWidth, RECT& rcDrawEnd, COLORREF crGrad1, COLORREF crGrad2)
{
    HRESULT hr = S_OK;

    // Gradient params
    int iWidth    = rcDrawEnd.right - rcDrawEnd.left - 1;
    int iHeight = rcDrawEnd.bottom - rcDrawEnd.top - 1;
    int iSize    = min(iWidth, iHeight);

    // Draw gradient
    double dbPcnt = 0.0;
    WORD wR, wG, wB;
    COLORREF crColor;
    POINT ptStart, ptEnd;

    RECT rcTemp;


    for (int i=0; i<iSize/2; i++)
    {
        // Start and end points
        ptStart.x = (rcDrawEnd.left + rcDrawEnd.right)/2 - (iWidth/2-i) + 2;
        ptStart.y = (rcDrawEnd.top + rcDrawEnd.bottom)/2 - (iHeight/2-i) + 2;
        ptEnd.x      = (rcDrawEnd.left + rcDrawEnd.right)/2 + (iWidth/2-i) - 2;
        ptEnd.y      = (rcDrawEnd.top + rcDrawEnd.bottom)/2 + (iHeight/2-i) - 2;

        rcTemp.left   = ptStart.x;
        rcTemp.top    = ptStart.y;
        rcTemp.right  = ptEnd.x;
        rcTemp.bottom = ptEnd.y;

        // check if we're in bounds to do the drawing
        if(rcTemp.left > rcDrawEnd.left+iRadialWidth ||
            rcTemp.top > rcDrawEnd.top+iRadialWidth ||
            rcTemp.right < rcDrawEnd.right-iRadialWidth ||
            rcTemp.bottom < rcDrawEnd.bottom-iRadialWidth)
            continue;

        // Gradient color percent
        dbPcnt = 1 - (double)(i-iRadialWidth) / (double)(iSize/2 - iRadialWidth);

        // Gradient color
        wR    = (unsigned char)(GetRValue(crGrad1)*dbPcnt) + (unsigned char)(GetRValue(crGrad2)*(1-dbPcnt));
        wG    = (unsigned char)(GetGValue(crGrad1)*dbPcnt) + (unsigned char)(GetGValue(crGrad2)*(1-dbPcnt));
        wB    = (unsigned char)(GetBValue(crGrad1)*dbPcnt) + (unsigned char)(GetBValue(crGrad2)*(1-dbPcnt));
        crColor    = RGB(wR, wG, wB);

        // Gradient
        hr = RoundRectangle(gdiDest, rcTemp, crColor, crColor, iRadialWidth, iRadialWidth);
        CHR(hr, _T("GradientRadialFillRect"));
    }

//    return TRUE;
Error:
    return hr;
}

HRESULT GradientHorzFillRect16(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2)
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    WORD*        lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    WORD        wR,wG,wB,wColor;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (WORD *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - 1));

    // loop through all the bits and change it to the proper color
    // Note: this is only the first column
    for (int i=0; i<(rcDraw.right - rcDraw.left); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.right - rcDraw.left)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        wR    = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
        wG    = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
        wB  = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) +((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
        wColor  = (WORD)((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3);
        lprgbSrc[rcDraw.left+i] = wColor;
    }

    // bitblt the rest for fast performance
    int j = rcDraw.top+1;
    int iHeight = 1;
    while(j < rcDraw.bottom)
    {
        //we always blit from the left edge
        //the amount we blit doubles every loop
        if(j + iHeight < rcDraw.bottom)
        {
            BitBlt(gdiDest.GetDC(),
                rcDraw.left,
                j,
                rcDraw.right - rcDraw.left,
                iHeight,
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
        }
        //if we're over half way we don't want to blit by iWidth
        //so we blit by the amount remaining but still from the left edge
        //and we break out of the loop as we're done
        else
        {
            BitBlt(gdiDest.GetDC(),
                rcDraw.left,
                j,
                rcDraw.right-rcDraw.left,
                rcDraw.bottom - j,
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
            break;
        }

        j += iHeight;
        iHeight *= 2;
    }

Error:
    return hr;
}

HRESULT GradientHorzFillRect24(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2)
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    RGBTRIPLE*    lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - 1));

    // loop through all the bits and change it to the proper color
    // Note: this is only the first column
    for (int i=0; i<(rcDraw.right - rcDraw.left); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.right - rcDraw.left)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        lprgbSrc[rcDraw.left+i].rgbtRed   = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
        lprgbSrc[rcDraw.left+i].rgbtGreen = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
        lprgbSrc[rcDraw.left+i].rgbtBlue  = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) +((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
    }

    // bitblt the rest for fast performance
    int j = rcDraw.top+1;
    int iHeight = 1;
    while(j < rcDraw.bottom)
    {
        //we always blit from the left edge
        //the amount we blit doubles every loop
        if(j + iHeight < rcDraw.bottom)
        {
            BitBlt(gdiDest.GetDC(),
                rcDraw.left,
                j,
                rcDraw.right - rcDraw.left,
                iHeight,
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
        }
        //if we're over half way we don't want to blit by iWidth
        //so we blit by the amount remaining but still from the left edge
        //and we break out of the loop as we're done
        else
        {
            BitBlt(gdiDest.GetDC(),
                rcDraw.left,
                j,
                rcDraw.right-rcDraw.left,
                rcDraw.bottom - j,
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
            break;
        }

        j += iHeight;
        iHeight *= 2;
    }

Error:
    return hr;
}


HRESULT GradientFillRect16(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2)
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    WORD*        lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    WORD        wR,wG,wB,wColor;

    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (WORD *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    // Note: this is only the first column
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        wR    = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
        wG    = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
        wB  = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) +((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
        //we need to drop a little accuracy off wG
        //        wG = wG >> 3;
        //        wG = wG << 3;

        //wColor  = (WORD)((wR>>3)<<11) | ((wG>>3)<<6) | (wB>>3);
        wColor  = (WORD)((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3);
        lprgbSrc[rcDraw.left] = wColor;
        // Move to next scan line.
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

    // bitblt the rest for fast performance
    int j = rcDraw.left+1;
    int iWidth = 1;
    while(j < rcDraw.right)
    {
        //we always blit from the left edge
        //the amount we blit doubles every loop
        if(j + iWidth < rcDraw.right)
        {
            BitBlt(gdiDest.GetDC(),
                j,
                rcDraw.top,
                iWidth,
                (rcDraw.bottom - rcDraw.top),
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
        }
        //if we're over half way we don't want to blit by iWidth
        //so we blit by the amount remaining but still from the left edge
        //and we break out of the loop as we're done
        else
        {
            BitBlt(gdiDest.GetDC(),
                j,
                rcDraw.top,
                rcDraw.right - j,
                (rcDraw.bottom - rcDraw.top),
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
            break;
        }

        j += iWidth;
        iWidth *= 2;
    }

Error:
    return hr;
}



HRESULT DrawRectangle(HDC hdc, RECT& rcDraw, COLORREF crColor, COLORREF crOutline)
{
    {
        HPEN oldPen;
        HPEN newPen;

        HBRUSH oldBrush;
        HBRUSH newBrush;

        newPen = ::CreatePen(PS_SOLID, 1, crOutline);
        oldPen = (HPEN)SelectObject(hdc, newPen);

        newBrush = CreateSolidBrush(crColor);

        if(crColor == RGB(255,4,255))
            oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        else
            oldBrush = (HBRUSH)SelectObject(hdc, newBrush);

        Rectangle(hdc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(newPen);
        DeleteObject(newBrush);

        return S_OK;
    }
}

HRESULT GradientFillRect24(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crGrad1, COLORREF crGrad2)
{

    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    RGBTRIPLE*    lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    // Note: this is only the first column
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        lprgbSrc[rcDraw.left].rgbtRed   = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) +
            ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
        lprgbSrc[rcDraw.left].rgbtGreen = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) +
            ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
        lprgbSrc[rcDraw.left].rgbtBlue  = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) +
            ((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
        // Move to next scan line.
        lprgbSrc = (RGBTRIPLE *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

    // bitblt the rest for fast performance
    int j = rcDraw.left+1;
    int iWidth = 1;
    while(j < rcDraw.right)
    {
        //we always blit from the left edge
        //the amount we blit doubles every loop
        if(j + iWidth < rcDraw.right)
        {
            BitBlt(gdiDest.GetDC(),
                j,
                rcDraw.top,
                iWidth,
                (rcDraw.bottom - rcDraw.top),
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
        }
        //if we're over half way we don't want to blit by iWidth
        //so we blit by the amount remaining but still from the left edge
        //and we break out of the loop as we're done
        else
        {
            BitBlt(gdiDest.GetDC(),
                j,
                rcDraw.top,
                rcDraw.right - j,
                (rcDraw.bottom - rcDraw.top),
                gdiDest.GetDC(),
                rcDraw.left,
                rcDraw.top,
                SRCCOPY);    
            break;
        }

        j += iWidth;
        iWidth *= 2;
    }

Error:
    return hr;
}

HRESULT GradientRoundRectangle(CIssGDIEx& gdiDest, 
                                   RECT& rcDraw, 
                                   COLORREF crOutline, 
                                   int iRoundWidth, 
                                   int iRoundHeight, 
                                   COLORREF crGrad1, 
                                   COLORREF crGrad2
                                   )
{
    HRESULT hr = S_OK;

    switch(gdiDest.GetBPP())
    {
    case 16:
        hr = GradientRoundRectangle16(gdiDest, rcDraw, crOutline, iRoundWidth, iRoundHeight, crGrad1, crGrad2);
        break;
    case 24:
    case 32:
        hr = GradientRoundRectangle24(gdiDest, rcDraw, crOutline, iRoundWidth, iRoundHeight, crGrad1, crGrad2);
        break;
    default:
        hr = E_INVALIDARG;
        break;
    }
    CHR(hr, _T("GradientRoundRectangle failed"));

Error:
    return hr;
}

HRESULT GradientRoundRectangle16(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crOutline, int iRoundWidth, int iRoundHeight, COLORREF crGrad1, COLORREF crGrad2 )
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC() && gdiDest.GetBits(), _T("gdiDest no HDC or Bits"))

    //First we draw our round 
    hr = RoundRectangle(gdiDest.GetDC(), rcDraw, NULL_COLOR, crOutline, iRoundWidth, iRoundHeight);
    CHR(hr, _T("RoundRectangle failed"));   

    //similar to DrawGradient    
    WORD*        lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;
    WORD        wR,wG,wB,wColor;

    // Initialize the surface pointers.
    lprgbSrc  = (WORD *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));

    // loop through all the bits and change it to the proper color
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        for (int x=rcDraw.left; x<rcDraw.right; x++)
        {
            wColor    = lprgbSrc[x];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);
            wR <<= 3;
            wG <<= 2;
            wB <<= 3;
            if(RGB(wR,wG,wB)== NULL_COLOR)
            {
                wR = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
                wG = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
                wB = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) + ((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
                wColor  = (WORD)((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3);
                lprgbSrc[x] = wColor;
            }
        }
        // Move to next scan line.
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

Error:
    return hr;
}

HRESULT GradientRoundRectangle24(CIssGDIEx& gdiDest, RECT& rcDraw, COLORREF crOutline, int iRoundWidth, int iRoundHeight, COLORREF crGrad1, COLORREF crGrad2 )
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    //First we draw our round 
    hr = RoundRectangle(gdiDest.GetDC(), rcDraw, NULL_COLOR, crOutline, iRoundWidth, iRoundHeight);
    CHR(hr, _T("RoundRectangle failed"));   

    RGBTRIPLE*    lprgbSrc;
    double        dbWeight=0;
    DWORD        dwWeight1=0;
    DWORD        dwWeight2=0;
    DWORD        dwWidthBytes    = (DWORD)gdiDest.GetBitmapInfo().bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc  = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()  + dwWidthBytes*(gdiDest.GetHeight() - rcDraw.bottom));


    // loop through all the bits and change it to the proper color
    for (int i=0; i<(rcDraw.bottom - rcDraw.top); i++) 
    {
        dbWeight = (1-(i/(float)(rcDraw.bottom - rcDraw.top)));
        dwWeight1 = (DWORD)(dbWeight*255.0);
        dwWeight2 = 255 - dwWeight1;

        for (int x=rcDraw.left; x<rcDraw.right; x++)
        {
            if( lprgbSrc[x].rgbtRed == GetRValue(NULL_COLOR) &&
                lprgbSrc[x].rgbtGreen == GetGValue(NULL_COLOR) &&
                lprgbSrc[x].rgbtBlue == GetBValue(NULL_COLOR))
            {
                lprgbSrc[x].rgbtRed = (BYTE)((((DWORD)GetRValue(crGrad2) * dwWeight1) + ((DWORD)GetRValue(crGrad1) * dwWeight2)) >> 8);
                lprgbSrc[x].rgbtGreen = (BYTE)((((DWORD)GetGValue(crGrad2) * dwWeight1) + ((DWORD)GetGValue(crGrad1) * dwWeight2)) >> 8);
                lprgbSrc[x].rgbtBlue = (BYTE)((((DWORD)GetBValue(crGrad2) * dwWeight1) + ((DWORD)GetBValue(crGrad1) * dwWeight2)) >> 8);
            }
        }
        // Move to next scan line.
        lprgbSrc = (RGBTRIPLE *)((LPBYTE)lprgbSrc + dwWidthBytes);
    }

Error:
    return hr;
}

HRESULT DitherImage(CIssGDIEx& gdiDest)
{
    HRESULT hr = S_OK; 

    switch(gdiDest.GetBPP())
    {
    case 24:
    case 32:
        break;
    case 16:
    default:
        hr = E_INVALIDARG;
        break;
    }
    CHR(hr, _T("DitherImage nothing to do or invalid BPP"));

    CBARG(gdiDest.GetDC()&&gdiDest.GetBits(), _T("gdiDest no HDC or Bits"));

    byte btQErR, btQErG, btQErB;
    byte btR, btG, btB;

    DWORD        dwWidthInBytes = 3*gdiDest.GetWidth();


    RGBTRIPLE*    lprgb1;
    RGBTRIPLE*    lprgb2;

    lprgb1 = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()+dwWidthInBytes);
    lprgb2 = (RGBTRIPLE *)((LPBYTE)gdiDest.GetBits()+2*dwWidthInBytes);//second line

    for(int j = 1; j < gdiDest.GetHeight() -1; j++) //edge conditions
    {
        for(int i = 1; i < gdiDest.GetWidth()-2; i++) //edge condition
        {
            //save the 24 bit values
            btR = lprgb1[i].rgbtRed;
            btG = lprgb1[i].rgbtGreen;
            btB = lprgb1[i].rgbtBlue;

            //set to the 16 bit pallet
            lprgb1[i].rgbtRed    = (((lprgb1[i].rgbtRed)>>3)<<3);
            lprgb1[i].rgbtGreen = (((lprgb1[i].rgbtGreen)>>2)<<2);
            lprgb1[i].rgbtBlue    = (((lprgb1[i].rgbtBlue)>>3)<<3);

            //find the error
            btQErR = btR - lprgb1[i].rgbtRed;
            btQErG = btG - lprgb1[i].rgbtGreen;
            btQErB = btB - lprgb1[i].rgbtBlue;

            if(btQErR != 0 && btQErG != 0 && btQErB != 0)
            {
                //dither time
                lprgb1[i+1].rgbtRed        = lprgb1[i+1].rgbtRed + ((7*btQErR)>>4);
                lprgb1[i+1].rgbtGreen    = lprgb1[i+1].rgbtGreen + ((7*btQErG)>>4);
                lprgb1[i+1].rgbtBlue    = lprgb1[i+1].rgbtBlue + ((7*btQErB)>>4);

                lprgb2[i-1].rgbtRed        = lprgb2[i-1].rgbtRed + ((3*btQErR)>>4);
                lprgb2[i-1].rgbtGreen    = lprgb2[i-1].rgbtGreen + ((3*btQErG)>>4);
                lprgb2[i-1].rgbtBlue    = lprgb2[i-1].rgbtBlue + ((3*btQErB)>>4);

                lprgb2[i].rgbtRed        = lprgb2[i].rgbtRed + ((5*btQErR)>>4);
                lprgb2[i].rgbtGreen        = lprgb2[i].rgbtGreen + ((5*btQErG)>>4);
                lprgb2[i].rgbtBlue        = lprgb2[i].rgbtBlue + ((5*btQErB)>>4);

                lprgb2[i+1].rgbtRed        = lprgb2[i+1].rgbtRed + ((btQErR)>>4);
                lprgb2[i+1].rgbtGreen    = lprgb2[i+1].rgbtGreen + ((btQErG)>>4);
                lprgb2[i+1].rgbtBlue    = lprgb2[i+1].rgbtBlue + ((btQErB)>>4);
            }
        }

        lprgb1 = lprgb2;
        lprgb2 = (RGBTRIPLE *)((LPBYTE)lprgb2 + dwWidthInBytes);
    }

Error:
    return hr;
}

HRESULT Line(HDC hDC, int iXStart, int iYStart, int iXEnd, int iYEnd, COLORREF crLine, int iPenWidth)
{
    HRESULT hr = S_OK;

    CBARG(hDC, _T(""));

    POINT p[2];

    p[0].x    = iXStart;
    p[0].y    = iYStart;
    p[1].x    = iXEnd;
    p[1].y    = iYEnd;    

    HPEN hPen = CIssGDIEx::CreatePen(crLine, PS_SOLID, iPenWidth);
    if(hPen == NULL)
        return FALSE;

    HPEN hOldPen= (HPEN)SelectObject(hDC, hPen);
    Polyline(hDC, p, 2);
    SelectObject(hDC, hOldPen);
    DeleteObject(hPen);

Error:
    return hr;
}

HRESULT FrameRect(HDC hdc, RECT& rcDraw, COLORREF crOutline, int iPenWidth)
{
    POINT p[5];

    p[0].x    = rcDraw.left;
    p[0].y    = rcDraw.top;
    p[1].x    = rcDraw.right;
    p[1].y    = rcDraw.top;
    p[2].x    = rcDraw.right;
    p[2].y    = rcDraw.bottom;
    p[3].x    = rcDraw.left;
    p[3].y    = rcDraw.bottom;
    p[4].x    = rcDraw.left;
    p[4].y    = rcDraw.top;

    HPEN hPen = CIssGDIEx::CreatePen(crOutline, PS_SOLID, iPenWidth);
    if(hPen == NULL)
    {
        return E_INVALIDARG;
    }
    HPEN hOldPen= (HPEN)SelectObject(hdc, hPen);
    Polyline(hdc, p, 5);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    return S_OK;
}

HRESULT Rectangle(HDC hdc, 
                  RECT& rcDraw, 
                  COLORREF crColor, 
                  COLORREF crOutline)
{
    HPEN oldPen;
    HPEN newPen;

    HBRUSH oldBrush;
    HBRUSH newBrush;

    newPen = ::CreatePen(PS_SOLID, 1, crOutline);
    oldPen = (HPEN)SelectObject(hdc, newPen);

    newBrush = CreateSolidBrush(crColor);

    if(crColor == NULL_COLOR)
        oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    else
        oldBrush = (HBRUSH)SelectObject(hdc, newBrush);

    Rectangle(hdc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(newPen);
    DeleteObject(newBrush);

    return S_OK;
}