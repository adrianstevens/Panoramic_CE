/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssGDIFX.cpp
// Abstract: Usefull Effects rountines base on IssGDIEx
//  
/***************************************************************************************************************/

#include "IssGDIFX.h"
#include "math.h"
#include "IssDebug.h"

#define ANIMATION_TIME            900.0 //in milliseconds
#define ANIMATION_TIME_FAST        500.0
#define ANIMATION_TIME_SLOW        1500.0

//////////////////////////////////////////////////////////////////////////
//    AnimateFromTo
//
//    hdcScreen       - Screen DC to draw to
//    gdiBackground   - Draw the background of the screen (note: this has to be the same size as hdcScreen for now)
//    gdiAniObject    - the object that we want to animate overtop
//    ptFrom          - starting location of the animation (top left of gdiAniObject)
//    ptTo            - ending location of the animation (top left of gdiAniObject)
//    iXStart         - starting location from within gdiAniObject of what to draw
//    iYStart         - starting location from within gdiAniObject of what to draw
//    iWidth, iHeight - width and height from within gdiAniObject of what to draw
//    iDrawFrames     - number of frames to draw
//////////////////////////////////////////////////////////////////////////
HRESULT AnimateFromTo(HDC        hdcScreen,
                                   CIssGDIEx&    gdiBackground,
                                   CIssGDIEx&    gdiAniObject,
                                   POINT    ptFrom,
                                   POINT    ptTo,
                                   int        iXStart,
                                   int        iYStart,
                                   int        iWidth,
                                   int        iHeight,
                                   int        iDrawFrames)
{
    HRESULT hr = S_OK;
    CIssGDIEx    gdiTemp;
    CBARG(hdcScreen, _T("hdcScreen"));
    CBARG(gdiBackground.GetDC(), _T("gdiBackground.GetDC()"));
    CBARG(gdiAniObject.GetDC(), _T("gdiAniObject.GetDC()"));
    CBARG(iWidth != 0, _T("iWidth != 0"));
    CBARG(iHeight != 0, _T("iHeight != 0"));
    CBARG(iDrawFrames != 0, _T("iDrawFrames != 0"));
    CBARG(gdiAniObject.GetHeight() >= iHeight, _T("gdiAniObject.GetHeight() >= iHeight"));
    CBARG(gdiAniObject.GetWidth() >= iWidth, _T("gdiAniObject.GetWidth()    >= iWidth"));

    float fXStep = (float)(ptTo.x - ptFrom.x)/(float)iDrawFrames;
    float fYStep = (float)(ptTo.y - ptFrom.y)/(float)iDrawFrames;

    int iXDraw        = ptFrom.x;
    int iYDraw        = ptFrom.y;
    int    iXOldDraw    = ptFrom.x;
    int iYOldDraw    = ptFrom.y;

    RECT rcDraw;
    rcDraw.left        = min(ptFrom.x, ptTo.x);
    rcDraw.top        = min(ptFrom.y, ptTo.y);
    rcDraw.right    = max(ptFrom.x, ptTo.x) + iWidth;
    rcDraw.bottom    = max(ptFrom.y, ptTo.y) + iHeight;

    // this is so we don't do too many bitblt's to the screen at once
    hr = gdiTemp.Create(hdcScreen, rcDraw, FALSE, FALSE, TRUE);
    CHR(hr, _T("gdiTemp.Create"));

    // draw the Full background
    BitBlt(hdcScreen,
           rcDraw.left, rcDraw.top,
           rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
           gdiBackground.GetDC(),
           rcDraw.left, rcDraw.top,
           SRCCOPY);

    // draw the full background to the temp GDI
    BitBlt(gdiTemp.GetDC(),
           0,0,
           rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
           gdiBackground.GetDC(),
           rcDraw.left, rcDraw.top,
           SRCCOPY);

    RECT rcScreen;
    for(int i=0; i<iDrawFrames; i++)
    {
        rcScreen.left    = min(iXOldDraw, iXDraw);
        rcScreen.top    = min(iYOldDraw, iYDraw);
        rcScreen.right    = max(iXOldDraw, iXDraw) + iWidth;
        rcScreen.bottom    = max(iYOldDraw, iYDraw) + iHeight;

        // Draw the old Background
        BitBlt(gdiTemp.GetDC(),
               iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
               iWidth, iHeight,
               gdiBackground.GetDC(),
               iXOldDraw, iYOldDraw,
               SRCCOPY);

        // Draw the new animated object
        TransparentBlt(    gdiTemp.GetDC(),
                        iXDraw - rcDraw.left, iYDraw - rcDraw.top,
                        iWidth, iHeight,
                        gdiAniObject.GetDC(),
                        iXStart, iYStart,
                        iWidth, iHeight,
                        RGB(255, 0, 255));

        // draw it all to the screen
        BitBlt( hdcScreen,
                rcScreen.left,rcScreen.top,
                rcScreen.right - rcScreen.left, rcScreen.bottom - rcScreen.top,
                gdiTemp.GetDC(),
                rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
                SRCCOPY);

        // update the values and draw again
        iXOldDraw    = iXDraw;
        iYOldDraw    = iYDraw;
        iXDraw        = (int)(i*fXStep) + ptFrom.x;//+= iXStep;
        iYDraw        = (int)(i*fYStep) + ptFrom.y;//+= iYStep;
    }

    if(iXDraw != ptTo.x || iYDraw != ptTo.y)
    {    //draw the final frame
        // update the values and draw again
        iXDraw        = ptTo.x;//+= iXStep;
        iYDraw        = ptTo.y;//+= iYStep;

        rcScreen.left    = min(iXOldDraw, iXDraw);
        rcScreen.top    = min(iYOldDraw, iYDraw);
        rcScreen.right    = max(iXOldDraw, iXDraw) + iWidth;
        rcScreen.bottom    = max(iYOldDraw, iYDraw) + iHeight;

        // Draw the old Background
        BitBlt(gdiTemp.GetDC(),
            iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
            iWidth, iHeight,
            gdiBackground.GetDC(),
            iXOldDraw, iYOldDraw,
            SRCCOPY);

        // Draw the new animated object
        TransparentBlt(    gdiTemp.GetDC(),
            iXDraw - rcDraw.left, iYDraw - rcDraw.top,
            iWidth, iHeight,
            gdiAniObject.GetDC(),
            iXStart, iYStart,
            iWidth, iHeight,
            RGB(255, 0, 255));

        // draw it all to the screen
        BitBlt( hdcScreen,
            rcScreen.left,rcScreen.top,
            rcScreen.right - rcScreen.left, rcScreen.bottom - rcScreen.top,
            gdiTemp.GetDC(),
            rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
            SRCCOPY);

    }

Error:
    return hr;
}

HRESULT SlideUp(HWND hWnd, CIssGDIEx& gdiDest, double dbSpeed)
{
    HRESULT hr = S_OK;
    RECT rcArea;
    GetClientRect(hWnd, &rcArea);

    CIssGDIEx gdiSrc;

    HDC hdcScreen = ::GetDC(hWnd);

    hr = gdiSrc.Create(hdcScreen, rcArea, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    hr = ScrollTransition(hdcScreen, rcArea, gdiSrc, rcArea, gdiDest, rcArea, SCROLL_Vertical, FALSE, dbSpeed);
    CHR(hr, _T("ScrollTransition"));
    
Error:
    ReleaseDC(hWnd, hdcScreen);

    return hr;
}

HRESULT SlideDown(HWND hWnd, CIssGDIEx& gdiDest, double dbSpeed)
{
    HRESULT hr = S_OK;
    RECT rcArea;
    GetClientRect(hWnd, &rcArea);

    CIssGDIEx gdiSrc;

    HDC hdcScreen = ::GetDC(hWnd);

    hr = gdiSrc.Create(hdcScreen, rcArea, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    hr = ScrollTransition(hdcScreen, rcArea, gdiSrc, rcArea, gdiDest, rcArea, SCROLL_Vertical, TRUE, dbSpeed);
    CHR(hr, _T("ScrollTransition"));

Error:
    ReleaseDC(hWnd, hdcScreen);

    return hr;
}




HRESULT FadeIn(HWND hWnd, 
               CIssGDIEx& gdiDest, 
               double dbSpeed /*= 900.0*/, 
               FNDRAWDESTFRAME fnDest /*= NULL*/, 
               LPVOID lpClass /*= NULL*/
               )
{
    HRESULT hr = S_OK;
    RECT rcArea;
    GetClientRect(hWnd, &rcArea);

    CIssGDIEx gdiSrc;

    HDC hdcScreen = ::GetDC(hWnd);

    hr = gdiSrc.Create(hdcScreen, rcArea, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    hr = AlphaTransition(hdcScreen, rcArea, gdiSrc, rcArea, gdiDest, rcArea, dbSpeed, fnDest, lpClass);
    CHR(hr, _T("AlphaTransition"));

Error:
    ReleaseDC(hWnd, hdcScreen);

    return hr;
}

HRESULT FadeIn(HDC hdcScreen, 
               CIssGDIEx& gdiDest, 
               double dbSpeed /*= 900.0*/, 
               FNDRAWDESTFRAME fnDest /*= NULL*/, 
               LPVOID lpClass /*= NULL*/
               )
{
    RECT rcArea;
    rcArea.left        = 0;
    rcArea.top        = 0;
    rcArea.right    = GetSystemMetrics(SM_CXSCREEN);
    rcArea.bottom    = GetSystemMetrics(SM_CYSCREEN);

    if(HEIGHT(rcArea) != gdiDest.GetHeight() ||
        WIDTH(rcArea) != gdiDest.GetWidth())
        return S_FALSE;

    CIssGDIEx gdiSrc;

    gdiSrc.Create(hdcScreen, rcArea, TRUE, TRUE);

    return AlphaTransition(hdcScreen, rcArea, gdiSrc, rcArea, gdiDest, rcArea, dbSpeed, fnDest, lpClass);
}

HRESULT SlideRight(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed )
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc, gdiAlpha;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));

    DWORD    dwTickOld;
    int        iCounter    = 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;
    int        iWidth        = gdiDest.GetWidth();
    int        iHeight        = gdiDest.GetHeight();

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    hr = gdiAlpha.Create(hdcScreen, iWidth, iHeight, FALSE, TRUE);
    CHR(hr, _T("gdiAlpha.Create"));

    dwTickOld =    GetTickCount();

    RECT rcSrc, rcDest;

    rcSrc.top        = rcDest.top    = 0;
    rcSrc.bottom    = rcDest.bottom = iHeight;

    // loop through
    for(int iAnimStep=0; iAnimStep<iWidth; iAnimStep++)
    {
        dbI+= dbStep;
        iAnimStep        = (int)dbI;
        if(iAnimStep > dbI && iAnimStep > 0)
            iAnimStep--;
        else if(iAnimStep > iWidth)
            iAnimStep = iWidth;    // make it draw one last time

        if(iAnimStep < iWidth)
        {
            rcDest.left        = iAnimStep;
            rcDest.right    = iWidth;
            rcSrc.left        = 0;
            rcSrc.right        = iWidth - iAnimStep;
            BitBlt(gdiAlpha.GetDC(),
                   rcDest.left,rcDest.top,
                   rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,
                   gdiSrc.GetDC(),
                   rcSrc.left, rcSrc.top,
                   SRCCOPY);
        }

        if(iAnimStep > 0)
        {
            rcDest.left        = 0;
            rcDest.right    = iAnimStep-1;
            rcSrc.left        = iWidth - (iAnimStep-1);
            rcSrc.right        = iWidth;
            BitBlt(gdiAlpha.GetDC(),
                   rcDest.left,rcDest.top,
                   rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,
                   gdiDest.GetDC(),
                   rcSrc.left, rcSrc.top,
                   SRCCOPY);
        }


        // update the screen
        BitBlt(hdcScreen,
               0,0,
               iWidth,iHeight,
               gdiAlpha.GetDC(),
               0,0,
               SRCCOPY);

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*iWidth / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }

    // final frame
    BitBlt(hdcScreen,
        0,0,
        iWidth,iHeight,
        gdiDest.GetDC(),
        0,0,
        SRCCOPY);

Error:
    return hr;
}

HRESULT SlideLeft(HDC hdcScreen, 
                  CIssGDIEx& gdiDest, 
                  double dbSpeed /*= 900.0*/
                  )
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc, gdiAlpha;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));

    DWORD    dwTickOld;
    int        iCounter    = 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;
    int        iWidth        = gdiDest.GetWidth();
    int        iHeight        = gdiDest.GetHeight();

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    hr = gdiAlpha.Create(hdcScreen, iWidth, iHeight, FALSE, TRUE);
    CHR(hr, _T("gdiAlpha.Create"));

    RECT rcSrc, rcDest;
    rcSrc.top    = rcDest.top = 0;
    rcSrc.bottom= rcDest.bottom = iHeight;

    dwTickOld =    GetTickCount();

    // loop through
    for(int iAnimStep=0; iAnimStep<iWidth; iAnimStep++)
    {
        dbI+= dbStep;
        iAnimStep        = (int)dbI;
        if(iAnimStep > dbI && iAnimStep > 0)
            iAnimStep--;
        else if(iAnimStep > iWidth)
            iAnimStep = iWidth;    // make it draw one last time

        if(iAnimStep < iWidth)
        {
            rcDest.left        = 0;
            rcDest.right    = iWidth-iAnimStep;
            rcSrc.left        = iAnimStep;
            rcSrc.right        = iWidth;
            BitBlt(gdiAlpha.GetDC(),
                rcDest.left,rcDest.top,
                rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,
                gdiSrc.GetDC(),
                rcSrc.left, rcSrc.top,
                SRCCOPY);
        }

        if(iAnimStep > 0)
        {
            rcDest.left        = iWidth - iAnimStep + 1;
            rcDest.right    = iWidth;
            rcSrc.left        = 0;
            rcSrc.right        = iAnimStep + 1;
            BitBlt(gdiAlpha.GetDC(),
                rcDest.left,rcDest.top,
                rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,
                gdiDest.GetDC(),
                rcSrc.left, rcSrc.top,
                SRCCOPY);
        }

        // update the screen
        // update the screen
        BitBlt(hdcScreen,
            0,0,
            iWidth, iHeight,
            gdiAlpha.GetDC(),
            0, 0,
            SRCCOPY);

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*iWidth / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }

    // final frame
    BitBlt(hdcScreen,
        0,0,
        iWidth, iHeight,
        gdiDest.GetDC(),
        0, 0,
        SRCCOPY);

Error:
    return hr;
}

HRESULT SweepRight(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed /*= 900.0*/)
{
	if(gdiDest.GetBPP() != 16)
		return S_FALSE;

    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc;
	WORD* wWeights = NULL;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));

    int        iWidth        = gdiDest.GetWidth();
    int        iHeight        = gdiDest.GetHeight();
    DWORD    dwTickOld;
    int        iFadeWidth    = iWidth/15;
    int        iCounter    = 0;
    int        iOldAnimStep= 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;
    WORD    wColor, wR, wG, wB;
    WORD    wWeight1, wWeight2;
    

    wWeights = new WORD[iFadeWidth];
	CPHR(wWeights, _T("wWeights = new WORD[iFadeWidth];"));


    // make a quick weight reference so we only do it once
    for(int i=0; i<iFadeWidth; i++)
        wWeights[i] = i*255/iFadeWidth;

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    RECT rc;
    rc.top        = 0;
    rc.bottom    = iHeight;

    dwTickOld =    GetTickCount();

    // loop through
    for(int iAnimStep=0; iAnimStep<iWidth; iAnimStep++)
    {
        dbI+= dbStep;
        iOldAnimStep    = iAnimStep;
        iAnimStep        = (int)dbI;
        if(iAnimStep > dbI && iAnimStep > 0)
            iAnimStep--;
        else if(iAnimStep > iWidth)
        {
            iAnimStep = iWidth;    // make it draw one last time
        }

        LPBYTE lpvDest        = (LPBYTE)gdiDest.GetBits();
        CPHR(lpvDest, _T("lpvDest"));
    
        LPBYTE lpvBuf        = (LPBYTE)gdiSrc.GetBits();
        CPHR(lpvBuf, _T("lpvBuf"));

        LPBYTE lpvOffsetBuf        = lpvBuf;
        LPBYTE lpvOffsetDest    = lpvDest;

        if(iAnimStep < iWidth && iFadeWidth > 0)
        {
            for(int y=0; y<iHeight; y++)
            {
                lpvOffsetBuf    = lpvBuf + iAnimStep*sizeof(WORD) + y*iWidth*sizeof(WORD);
                lpvOffsetDest    = lpvDest+ iAnimStep*sizeof(WORD) + y*iWidth*sizeof(WORD);
                for(int x=iAnimStep; x<iWidth && x < iAnimStep+iFadeWidth; x++)
                {
                    wWeight1    = wWeights[x-iAnimStep];
                    wWeight2    = 255 - wWeight1;
                    wColor  = *(WORD*)lpvOffsetBuf;
                    wR = (WORD)((wColor >> 11) & 0x1f) * wWeight1;
                    wG = (WORD)((wColor >> 5) & 0x3f) * wWeight1;
                    wB = (WORD)(wColor & 0x1f) * wWeight1;
                    wColor  = *(WORD*)lpvOffsetDest;
                    wR = (WORD)(((wColor >> 11)&0x1f) * wWeight2 +wR)>>8;
                    wG = (WORD)(((wColor >> 5)&0x3f) * wWeight2 + wG)>>8;
                    wB = (WORD)((wColor & 0x1f)* wWeight2 + wB)>>8;
                    wColor  = (WORD)( ( wR & 0xff ) << 11 | ( wG & 0xff ) << 5 | ( wB & 0xff ) );
                    *(WORD*)lpvOffsetBuf = wColor;

                    lpvOffsetBuf += sizeof(WORD);
                    lpvOffsetDest+= sizeof(WORD);
                }
            }
        }

        // Clear up the old faded stuff
        if(iAnimStep-iOldAnimStep > 0)
        {
            rc.left        = iOldAnimStep-1;
            rc.right    = min(iWidth, iAnimStep+1);
            BitBlt(gdiSrc.GetDC(),
                rc.left,rc.top,
                rc.right-rc.left, rc.bottom-rc.top,
                gdiDest.GetDC(),
                rc.left, rc.top,
                SRCCOPY);
        }

        // update the screen
        rc.left        = iOldAnimStep-1;
        rc.right    = min(iWidth, iAnimStep+iFadeWidth);

        // update the screen
        BitBlt(hdcScreen,
            rc.left,rc.top,
            rc.right-rc.left, rc.bottom-rc.top,
            gdiSrc.GetDC(),
            rc.left, rc.top,
            SRCCOPY);

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*iWidth / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }

    // final frame
    BitBlt(hdcScreen,
        0,0,
        iWidth, iHeight,
        gdiDest.GetDC(),
        0, 0,
        SRCCOPY);

Error:
	if(wWeights)
		delete [] wWeights;

    return hr;
}

HRESULT SweepLeft(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed /*= 900.0*/)
{
	if(gdiDest.GetBPP() != 16)
		return S_FALSE;

    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc;
	WORD* wWeights = NULL;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));

    int        iWidth        = gdiDest.GetWidth();
    int        iHeight        = gdiDest.GetHeight();
    DWORD    dwTickOld;
    int        iFadeWidth    = iWidth/15;
    int        iCounter    = 0;
    int        iOldAnimStep= 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;
    WORD    wColor, wR, wG, wB;
    WORD    wWeight1, wWeight2;

    wWeights = new WORD[iFadeWidth];
	CPHR(wWeights, _T("wWeights = new WORD[iFadeWidth];"));

    // make a quick weight reference so we only do it once
    for(int i=0; i<iFadeWidth; i++)
    {
        wWeights[i] = (iFadeWidth-i)*255/iFadeWidth;
    }

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    RECT rc;
    rc.top        = 0;
    rc.bottom    = iHeight;

    dwTickOld =    GetTickCount();

    // loop through
    for(int iAnimStep=0; iAnimStep<iWidth; iAnimStep++)
    {
        dbI+= dbStep;
        iOldAnimStep    = iAnimStep;
        iAnimStep        = (int)dbI;
        if(iAnimStep > dbI && iAnimStep > 0)
            iAnimStep--;
        else if(iAnimStep > iWidth)
        {
            iAnimStep = iWidth;    // make it draw one last time
        }

        LPBYTE lpvDest        = (LPBYTE)gdiDest.GetBits();
        CPHR(lpvDest, _T("lpvDest"));

        LPBYTE lpvBuf        = (LPBYTE)gdiSrc.GetBits();
        CPHR(lpvBuf, _T("lpvBuf"));

        LPBYTE lpvOffsetBuf        = lpvBuf;
        LPBYTE lpvOffsetDest    = lpvDest;

        int iOffset;

        if(iAnimStep < iWidth && iFadeWidth > 0)
        {
            for(int y=0; y<iHeight; y++)
            {
                iOffset            = iWidth - iFadeWidth - iAnimStep;
                iOffset            = min(iWidth-1, iOffset);
                iOffset            = max(0, iOffset);
                lpvOffsetBuf    = lpvBuf + iOffset*sizeof(WORD) + y*iWidth*sizeof(WORD);
                lpvOffsetDest    = lpvDest+ iOffset*sizeof(WORD) + y*iWidth*sizeof(WORD);
                for(int x=iAnimStep; x<iWidth && x < iAnimStep+iFadeWidth; x++)
                {
                    wWeight1    = wWeights[x-iAnimStep];
                    wWeight2    = 255 - wWeight1;
                    wColor  = *(WORD*)lpvOffsetBuf;
                    wR = (WORD)((wColor >> 11) & 0x1f) * wWeight1;
                    wG = (WORD)((wColor >> 5) & 0x3f) * wWeight1;
                    wB = (WORD)(wColor & 0x1f) * wWeight1;
                    wColor  = *(WORD*)lpvOffsetDest;
                    wR = (WORD)(((wColor >> 11)&0x1f) * wWeight2 +wR)>>8;
                    wG = (WORD)(((wColor >> 5)&0x3f) * wWeight2 + wG)>>8;
                    wB = (WORD)((wColor & 0x1f)* wWeight2 + wB)>>8;
                    wColor  = (WORD)( ( wR & 0xff ) << 11 | ( wG & 0xff ) << 5 | ( wB & 0xff ) );
                    *(WORD*)lpvOffsetBuf = wColor;

                    lpvOffsetBuf += sizeof(WORD);
                    lpvOffsetDest+= sizeof(WORD);
                }
            }
        }

        // Clear up the old faded stuff
        if(iAnimStep-iOldAnimStep > 0)
        {
            rc.left        = max(0, iWidth - iAnimStep - 1);
            rc.right    = iWidth - iOldAnimStep;
            BitBlt(gdiSrc.GetDC(),
                rc.left,rc.top,
                rc.right-rc.left, rc.bottom-rc.top,
                gdiDest.GetDC(),
                rc.left, rc.top,
                SRCCOPY);
        }

        rc.left        = max(0, iWidth - iAnimStep - iFadeWidth);
        rc.right    = iWidth - iOldAnimStep;

        // update the screen
        BitBlt(hdcScreen,
            rc.left,rc.top,
            rc.right-rc.left, rc.bottom-rc.top,
            gdiSrc.GetDC(),
            rc.left, rc.top,
            SRCCOPY);

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*iWidth / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }
    // final frame
    BitBlt(hdcScreen,
        0,0,
        iWidth, iHeight,
        gdiDest.GetDC(),
        0, 0,
        SRCCOPY);

Error:
	if(wWeights)
		delete [] wWeights;

    return hr;
}

HRESULT Trapazoid(HDC hdcScreen, 
                  CIssGDIEx& gdiDest, 
                  POINT ptTL, 
                  POINT ptTR, 
                  POINT ptBL, 
                  POINT ptBR, 
                  double dbSpeed
                  )
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));
    CBARG(ptBL.y != ptTL.y, _T("ptBL.y != ptTL.y"));
    CBARG(ptBR.y != ptTR.y, _T("ptBL.y != ptTL.y"));
    CBARG(ptBR.y != ptTL.y, _T("ptBR.y != ptTL.y"));

    int        iWidth        = max(ptTR.x - ptTL.x, ptBR.x - ptBL.x);
    int        iHeight        = ptBL.y - ptTL.y;
    int        iCounter    = 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, TRUE);
    CHR(hr, _T("gdiSrc.Create"));

    LPBYTE lpvDest        = (LPBYTE)gdiDest.GetBits();
    CPHR(lpvDest, _T("lpvDest"));

    LPBYTE lpvBuf        = (LPBYTE)gdiSrc.GetBits();
    CPHR(lpvBuf, _T("lpvBuf"));

    int iXStart = 0;
    int iXEnd    = 0;
    int iYColor, iXColor;
    double dbRatioX    = 0.0;
    double dbRatioY = 0.0;
    WORD* wPixel;
    WORD* wBuffer;

    // k = 1/Slope
    double kLeft    = 0;
    double kRight    = 0;

    // left
    // f(y) = x1 + k(y - y1) 
    kLeft = (double)(ptBL.x - ptTL.x)/(double)(ptBL.y - ptTL.y);

    // right 
    kRight = (double)(ptBR.x - ptTR.x)/(double)(ptBR.y - ptTR.y);

    dbRatioY = (double)iHeight/(double)(ptBR.y - ptTL.y);

    // loop through all the bits and change it to the proper color
    for (int y=0; y<(ptBR.y-ptTL.y); y++) 
    {
        iXStart = ptTL.x + (int)(kLeft*y);
        iXEnd    = ptTR.x + (int)(kRight*y);
        iYColor    = (int)((double)y*dbRatioY);

        dbRatioX = (double)iWidth/(double)(iXEnd-iXStart);

        wPixel = (WORD *) (lpvDest + iWidth*sizeof(WORD)*(iHeight - iYColor));
        wBuffer= (WORD *) (lpvBuf + iWidth*sizeof(WORD)*(iHeight - y - ptTL.y));

        for (int x=0; x<(iXEnd-iXStart); x++) 
        {
            iXColor = (int)(x*dbRatioX);
            wBuffer[iXStart+x] = wPixel[iXColor];
        }
    }

    BitBlt(hdcScreen,
        0,0,
        iWidth, iHeight,
        gdiSrc.GetDC(),
        0, 0,
        SRCCOPY);

Error:
    return hr;
}

HRESULT CubeHorzUp(HDC hdcScreen, CIssGDIEx& gdiDest, RECT rcSrc, RECT rcDest, double dbSpeed)
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiSrc;
    POINT ptLowerEnd;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));
    CBARG(rcDest.top != rcSrc.top,_T("rcDest.top != rcSrc.top"));
    CBARG(rcSrc.top != rcDest.bottom,_T("rcSrc.top != rcDest.bottom"));
    
    int        iWidth        = gdiDest.GetWidth();
    int        iHeight        = gdiDest.GetHeight();
    DWORD    dwTickOld;
    int        iFadeWidth    = iWidth/15;
    int        iCounter    = 0;
    int        iOldAnimStep= 0;
    int        iMaxDistance= 0;
    double    dbStep        = 0;
    double    dbActualTime= 0;
    double    dbI            = 0.0;
    double    dbINew        = 0.0;
    double    dbIOld        = 0.0;
    POINT    ptTL, ptTR, ptBL, ptBR;

    // k = 1/Slope
    double kTopLeft        = 0;
    double kTopRight    = 0;
    double kBottomLeft    = 0;
    double kBottomRight    = 0;
    double kLowerLeft    = 0;
    double kLowerRight    = 0;

    hr = gdiSrc.Create(hdcScreen, iWidth, iHeight, TRUE, FALSE);
    CHR(hr, _T("gdiSrc.Create"));
    StretchBlt(gdiSrc.GetDC(),
               0,0,
               iWidth, iHeight,
               hdcScreen,
               rcSrc.left, rcSrc.top,
               rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top,
               SRCCOPY);

    ptLowerEnd.x    = rcSrc.left + (rcSrc.right-rcSrc.left)/2;
    ptLowerEnd.y    = rcDest.bottom;

    CBARG(rcSrc.bottom != ptLowerEnd.y,_T("rcSrc.bottom != ptLowerEnd.y"));

    // figure out the slopes for each four corners
    kTopLeft    = (double)(rcSrc.left - rcDest.left)/(double)(rcDest.top - rcSrc.top);
    kTopRight    = (double)(rcSrc.right - rcDest.right)/(double)(rcDest.top - rcSrc.top);
    kBottomLeft = (double)(rcDest.left - rcSrc.left)/(double)(rcSrc.top - rcDest.bottom);
    kBottomRight= (double)(rcDest.right - rcSrc.right)/(double)(rcSrc.top - rcDest.bottom);
    kLowerLeft = (double)(ptLowerEnd.x - rcSrc.left)/(double)(rcSrc.bottom - ptLowerEnd.y);
    kLowerRight= (double)(ptLowerEnd.x - rcSrc.right)/(double)(rcSrc.bottom - ptLowerEnd.y);

    iMaxDistance= max(rcSrc.top - rcDest.top, rcDest.bottom - rcSrc.top);


    dwTickOld =    GetTickCount();

    // loop through
    for(int iAnimStep=0; iAnimStep<100; iAnimStep++)
    {
        dbI+= dbStep;
        iOldAnimStep    = iAnimStep;
        iAnimStep        = (int)dbI;
        if(iAnimStep > dbI && iAnimStep > 0)
            iAnimStep--;
        else if(iAnimStep > 100)
            iAnimStep = 100;    // make it draw one last time

        int iTopStep    = iAnimStep*iAnimStep/100;
        int iBottomStep = iAnimStep;

        ptTL.y    = ptTR.y    = rcDest.top +        (rcSrc.top - rcDest.top)*(100-iTopStep)/100;
        ptBL.y    = ptBR.y    = rcSrc.top +    (rcDest.bottom - rcSrc.top)*iBottomStep/100;
        ptTL.x    = rcDest.left - (int)(kTopLeft*(ptTL.y-rcDest.top));
        ptTR.x    = rcDest.right - (int)(kTopRight*(ptTL.y-rcDest.top));
        ptBL.x    = rcSrc.left - (int)(kBottomLeft*(ptBL.y-rcSrc.top));
        ptBR.x    = rcSrc.right - (int)(kBottomRight*(ptBL.y-rcSrc.top));

        // draw the top part
        hr = Trapazoid(hdcScreen, gdiDest, ptTL, ptTR, ptBL, ptBR, dbSpeed);
        CHR(hr, _T("Trapazoid"));

        ptTL.y    = ptTR.y = ptBL.y;
        ptTL.x    = ptBL.x;
        ptTR.x    = ptBR.x;

        ptBL.y    = ptBR.y    = rcSrc.bottom + (rcDest.bottom - rcSrc.bottom)*iBottomStep/100;
        ptBL.x    = rcSrc.left - (int)(kLowerLeft*(ptBL.y-rcSrc.bottom));
        ptBR.x    = rcSrc.right - (int)(kLowerRight*(ptBL.y-rcSrc.bottom));

        hr = Trapazoid(hdcScreen, gdiSrc, ptTL, ptTR, ptBL, ptBR, dbSpeed);
        CHR(hr, _T("Trapazoid"));

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*100 / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            CBARG(iCounter != 0, _T("iCounter != 0"));
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }
    // final frame
    BitBlt(hdcScreen,
        0,0,
        iWidth, iHeight,
        gdiDest.GetDC(),
        0, 0,
        SRCCOPY);

Error:
    return hr;
}

/*

We'll handle all four directions in this function

*/
HRESULT ScrollTransition(HDC hdcScreen, 
                         RECT& rcArea, 
                         CIssGDIEx& gdiSrc, 
                         RECT& rcSrc, 
                         CIssGDIEx& gdiDest, 
                         RECT& rcDest, 
                         SCROLL_Direction eScroll, 
                         BOOL bDirection, 
                         double dbSpeed
                         )
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(gdiSrc.GetDC(),_T("gdiSrc.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));
    CBARG(rcArea.right >= rcArea.left && rcArea.bottom >= rcArea.top,_T("rcArea.right >= rcArea.left && rcArea.bottom >= rcArea.top"));
    CBARG(rcSrc.right >= rcSrc.left && rcSrc.bottom >= rcSrc.top,_T("rcSrc.right >= rcSrc.left && rcSrc.bottom >= rcSrc.top"));
    CBARG(rcDest.right >= rcDest.left && rcDest.bottom >= rcDest.top ,_T("rcDest.right >= rcDest.left && rcDest.bottom >= rcDest.top "));

    DWORD            dwTickOld    = 0;
    int                iAnimationSize;
    int                iCounter    = 0;
    double            dbStep        = 0;
    double            dbActualTime= 0;
    double            dbI            = 0.0;
    double            dbINew        = 0.0;
    double            dbIOld        = 0.0;

    if(eScroll == SCROLL_Horizontal)
    {
        iAnimationSize = rcArea.right - rcArea.left;
        for(int i = 0; i <iAnimationSize; i++)
        {
            dbI+= dbStep;
            i = (int)dbI;
            if(i > dbI)
                i--;
            if(dbI > iAnimationSize)
                break;//we're done

            if(bDirection)
            {
                BitBlt(hdcScreen, 
                    rcArea.right-rcArea.left-i,
                    rcArea.top,
                    i,
                    rcArea.bottom - rcArea.top,
                    gdiDest.GetDC(),
                    rcDest.left,
                    rcDest.top,//
                    SRCCOPY);

                // old
                BitBlt(hdcScreen,
                    rcArea.left,
                    rcArea.top,
                    rcArea.right-rcArea.left-i,
                    rcArea.bottom - rcArea.top,
                    gdiSrc.GetDC(),
                    rcSrc.left + i,
                    rcSrc.top,
                    SRCCOPY);
            }
            else
            {
                BitBlt(hdcScreen, 
                    rcArea.left,
                    rcArea.top,
                    i,
                    rcArea.bottom - rcArea.top,
                    gdiDest.GetDC(),
                    rcDest.left + rcArea.right-rcArea.left-i,
                    rcDest.top,//
                    SRCCOPY);

                // old
                BitBlt(hdcScreen,
                    rcArea.left+i,
                    rcArea.top,//
                    rcArea.right-rcArea.left-i,
                    rcArea.bottom - rcArea.top,
                    gdiSrc.GetDC(),
                    rcSrc.left,
                    rcSrc.top,
                    SRCCOPY);
            }

            //Speed Control Code
            if(dbIOld == 0 && iCounter == 0)
                dwTickOld =    GetTickCount();

            dbActualTime = GetTickCount() - dwTickOld;

            dbI += dbStep;

            dbINew = dbActualTime*(double)iAnimationSize / dbSpeed;

            iCounter++;

            if(dbIOld < dbINew)
            {
                CBARG(iCounter != 0, _T("iCounter != 0"));
                dbStep = (dbINew)/((double)iCounter);
                dbIOld = dbINew;
            }
        }


    }
    else    //Vertical
    {
        iAnimationSize = rcArea.bottom - rcArea.top;
        for(int i =0 ;i < iAnimationSize; i+=(int)dbStep)
        {
            dbI += dbStep;
            i = (int)dbI;
            if(i > dbI)
                i--;

            if(bDirection)
            {
                //scrolling down
                BitBlt(hdcScreen, 
                    rcArea.left,
                    rcArea.top,
                    rcArea.right-rcArea.left,
                    i,
                    gdiDest.GetDC(),
                    rcDest.left,
                    rcDest.top + rcArea.bottom-rcArea.top-i,
                    SRCCOPY);

                // old
                BitBlt(hdcScreen,
                    rcArea.left,
                    rcArea.top+i,//
                    rcArea.right-rcArea.left,
                    rcArea.bottom-rcArea.top-i,
                    gdiSrc.GetDC(),
                    rcSrc.left,
                    rcSrc.top,
                    SRCCOPY);
            }
            //scrolling up
            else
            {    //new
                BitBlt(hdcScreen,
                    rcArea.left,
                    rcArea.top+rcArea.bottom-rcArea.top-i,
                    rcArea.right-rcArea.left,
                    i,
                    gdiDest.GetDC(),
                    rcDest.left,
                    rcDest.top,
                    SRCCOPY);

                // old
                BitBlt(hdcScreen,
                    rcArea.left,
                    rcArea.top,
                    rcArea.right-rcArea.left,
                    rcArea.bottom-rcArea.top-i,
                    gdiSrc.GetDC(),
                    rcSrc.left,
                    rcSrc.top + i,
                    SRCCOPY);
            }

            //Speed Control Code
            dbActualTime = GetTickCount() - dwTickOld;

            dbI += dbStep;

            if(dbI > iAnimationSize-1)
            {
                break;
            }


            dbINew = dbActualTime*(double)iAnimationSize / dbSpeed;
            if(dbIOld == 0 && dbINew != 00)
                dbIOld = dbINew;
            iCounter++;

            if(dbIOld != dbINew)
            {
                CBARG(iCounter != 0, _T("iCounter != 0"));
                dbStep = (dbINew - dbIOld)/((double)iCounter);
                dbIOld = dbINew;
                iCounter = 0;
            }
        }
    }
Error:
    return hr;
}

/********************************************************************************************
Main Animiation Entry Point

Pass in the Transition type for eTransition

Direction

Not used for Fade or "smooth"

VERT -> TRUE is Up...FALSE is down
HORZ -> TRUE is Right, FALSE is left

RECT rcArea ... where to draw on the hdcScreen
RECT rcSrc  ... where to Animate FROM on the Source GDI
RECT rcDest ... where to Animate FROM on the Destination GDI

********************************************************************************************/

HRESULT Animate(HDC hdcScreen, 
                RECT& rcArea, 
                CIssGDIEx& gdiSrc, 
                RECT& rcSrc,
                CIssGDIEx& gdiDest, 
                RECT& rcDest, 
                Transition_Type eTransition, 
                BOOL bDirection, 
                Animation_Type eSpeed
                )
{
    HRESULT hr = S_OK;
    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(gdiSrc.GetDC(),_T("gdiSrc.GetDC()"));
    CBARG(eSpeed > 0 && eSpeed <= 2,_T("eSpeed > 0 && eSpeed <= 2"));

    double dbSpeed = 0;
    switch(eSpeed) 
    {
    case ANIMATION_Normal:
        dbSpeed = ANIMATION_TIME;
        break;
    case ANIMATION_Fast:
        dbSpeed = ANIMATION_TIME_FAST;
        break;
    case ANIMATION_Slow:
        dbSpeed = ANIMATION_TIME_SLOW;
        break;
    default:
        break;
    }

    switch(eTransition) 
    {
    default:
    case TRANSITION_Scroll_Vertical:
        hr = ScrollTransition(hdcScreen, rcArea, gdiSrc, rcSrc, gdiDest,    rcDest, SCROLL_Vertical, bDirection, dbSpeed);
        break;
    case TRANSITION_Scroll_Horizontal:
        hr = ScrollTransition(hdcScreen, rcArea, gdiSrc, rcSrc, gdiDest,  rcDest,    SCROLL_Horizontal, bDirection, dbSpeed);
        break;
    case TRANSITION_Fade:
        hr = AlphaTransition(hdcScreen, rcArea, gdiSrc, rcSrc, gdiDest, rcDest, dbSpeed);
        break;
    }
Error:
    return hr;
}

HRESULT SmoothTransition(HDC hdcScreen, RECT& rcArea, CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest)
{
    RECT rc;

    rc.left        = 0;
    rc.top        = 0;
    rc.right    = rcArea.right - rcArea.left;
    rc.bottom    = rcArea.bottom - rcArea.top;
    return AlphaTransition( hdcScreen, rcArea, gdiSrc, rc, gdiDest, rc, ANIMATION_TIME);
}

// smooth animation between one Image to the other.  Note: both gdiSrc and gdiDest have to be the same size
HRESULT AlphaTransition(HDC hdcScreen, 
                        RECT& rcArea, 
                        CIssGDIEx& gdiSrc, 
                        RECT& rcSrc,
                        CIssGDIEx& gdiDest, 
                        RECT& rcDest, 
                        double dbSpeed, 
                        FNDRAWDESTFRAME fnDest /*= NULL*/, 
                        LPVOID lpClass /*= NULL*/
                        )        // resulting image you want
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiAlha;

	//we only support 16 bit for now
	if(gdiDest.GetBPP() != 16 || gdiDest.GetBPP() != gdiSrc.GetBPP())
	{
		hr = E_INVALIDARG;
		goto Error;
	}

    CBARG(gdiDest.GetDC(),_T("gdiDest.GetDC() "));
    CBARG(gdiSrc.GetDC(),_T("gdiSrc.GetDC() "));
    CBARG(gdiDest.GetBits(),_T("gdiDest.GetBits() "));
    CBARG(gdiSrc.GetBits(),_T("gdiSrc.GetBits() "));
    CBARG(hdcScreen,_T("hdcScreen"));
    CBARG(dbSpeed > 0.0f,_T("dbSpeed > 0.0f"));
    CBARG(rcArea.right >= rcArea.left && rcArea.bottom >= rcArea.top,_T("rcArea.right >= rcArea.left && rcArea.bottom >= rcArea.top"));
    CBARG(rcSrc.right >= rcSrc.left && rcSrc.bottom >= rcSrc.top,_T("rcSrc.right >= rcSrc.left && rcSrc.bottom >= rcSrc.top"));
    CBARG(rcDest.right >= rcDest.left && rcDest.bottom >= rcDest.top ,_T("rcDest.right >= rcDest.left && rcDest.bottom >= rcDest.top "));

    int                iCounter    = 0;
    double            dbStep        = 0;
    double            dbActualTime= 0;
    double            dbI            = 0.0;
    double            dbINew        = 0.0;
    double            dbIOld        = 0.0;

    hr = gdiAlha.Create(hdcScreen, rcArea);
    CHR(hr, _T("gdiAlha.Create"));

    if(gdiSrc.GetBits() == NULL)
    {
        hr = gdiSrc.ConvertToDIB();
        CHR(hr, _T("gdiSrc.ConvertToDIB()"));
    }

    if(gdiDest.GetBits() == NULL)
    {
        hr = gdiDest.ConvertToDIB();
        CHR(hr, _T("gdiDest.ConvertToDIB()"));
    }

    WORD    *lprgbAlpha, *lprgbCurrent, *lprgbNew;
    DWORD    dwWeight1;    
    DWORD    dwWeight2;
    DWORD    dwTickOld;
    WORD    wR,wG,wB,wR2,wG2,wB2,wColor;

    dwTickOld =    GetTickCount();

    // loop through
    for(int iAlpha=0; iAlpha<32; iAlpha++)
    {
        dbI+= dbStep;
        iAlpha = (int)dbI;
        if(iAlpha > dbI && iAlpha > 0)
            iAlpha--;
        else if(iAlpha > 32)
            break;

        // for progressive animation
        if(fnDest)
            fnDest(gdiDest, rcDest, lpClass, iAlpha, 32);

        // Initialize the surface pointers.
        lprgbAlpha        = (WORD *)((LPBYTE)gdiAlha.GetBits());
        lprgbCurrent    = (WORD *)((LPBYTE)gdiSrc.GetBits()+gdiSrc.GetHeight() - rcSrc.bottom);    // BUGBUG: this is wrong, only works cause we've been doing full screen fades
        lprgbNew        = (WORD *)((LPBYTE)gdiDest.GetBits()+gdiDest.GetHeight() - rcDest.bottom);

        dwWeight1    = 32 - iAlpha;
        dwWeight2    = iAlpha;

        // loop through all the bits and change it to the proper color
        for (int y=0; y<rcArea.bottom - rcArea.top; y++) 
        {
            for (int x=0; x<rcArea.right - rcArea.left; x++) 
            {
                wColor    = lprgbCurrent[x+rcSrc.left];
                wR = (WORD)((wColor >> 11) & 0x1f);
                wG = (WORD)((wColor >> 5) & 0x3f);
                wB = (WORD)(wColor & 0x1f);
                if(RGB(wR,wG,wB)== TRANSPARENT_WORD)
                    continue;//for images with transparencies

                wColor = lprgbNew[x+rcDest.left];
                wR2 = (WORD)((wColor >> 11) & 0x1f);
                wG2 = (WORD)((wColor >> 5) & 0x3f);
                wB2 = (WORD)(wColor & 0x1f);
                wR  = (BYTE)((((DWORD)wR * dwWeight1) + ((DWORD)wR2*dwWeight2)) >> 5);
                wG    = (BYTE)((((DWORD)wG * 2*dwWeight1) + ((DWORD)wG2*dwWeight2*2)) >> 6);
                wB  = (BYTE)((((DWORD)wB * dwWeight1) + ((DWORD)wB2*dwWeight2)) >> 5);
                wColor  = (WORD)(wR<<11|wG<<5|wB);
                lprgbAlpha[x] = wColor;
            }

            // Move to next scan line.
            lprgbAlpha    = (WORD *)((LPBYTE)lprgbAlpha + gdiAlha.GetBitmapInfo().bmWidthBytes);
            lprgbCurrent= (WORD *)((LPBYTE)lprgbCurrent + gdiSrc.GetBitmapInfo().bmWidthBytes);
            lprgbNew    = (WORD *)((LPBYTE)lprgbNew + gdiDest.GetBitmapInfo().bmWidthBytes);
        }

        // draw it to the screen
        BitBlt(hdcScreen, 
            rcArea.left,rcArea.top,
            rcArea.right - rcArea.left,
            rcArea.bottom - rcArea.top,
            gdiAlha.GetDC(),
            0,
            0,
            SRCCOPY);

        //Speed Control Code
        dbActualTime = GetTickCount() - dwTickOld;

        dbI += dbStep;

        dbINew = dbActualTime*32.0 / dbSpeed;
        if(dbIOld == 0 && dbINew != 00)
            dbIOld = dbINew;
        iCounter++;

        if(dbIOld != dbINew)
        {
            CBARG(iCounter != 0, _T("iCounter != 0"));
            dbStep = (dbINew - dbIOld)/((double)iCounter);
            dbIOld = dbINew;
            iCounter = 0;
        }
    }
    // final frame
    BitBlt(hdcScreen,
        rcArea.left,rcArea.top,
        rcArea.right - rcArea.left,
        rcArea.bottom - rcArea.top,
        gdiDest.GetDC(),
        0, 0, 
        SRCCOPY);

/* //wrong ... Adrian - Nov 17/09
    BitBlt(hdcScreen,
        0,0,
        rcArea.right - rcArea.left,
        rcArea.bottom - rcArea.top,
        gdiDest.GetDC(),
        0, 0,
        SRCCOPY);*/

Error:
    return hr;
}

int GetCurrentFrame(DWORD &dwStartTime, int iTargetNumFrames /* = 100 */, double dbAniTime /* = 900.0 */)
{
    if(dbAniTime <= 0.0f)
        return iTargetNumFrames;//tell it to draw the last frame

    double dActualTime    = GetTickCount() - dwStartTime;

    int iCurrentFrame    =  (int)((double)iTargetNumFrames*dActualTime/dbAniTime);

    if(iCurrentFrame > iTargetNumFrames)
        iCurrentFrame = iTargetNumFrames;

    return iCurrentFrame;
}

// scale an image using bilinear interpolation and place it in gdiDest
HRESULT ScaleImage(CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest, SIZE sizeDest, BOOL bUseTransparencies, COLORREF crTranparentColor)
{
    HRESULT hr = S_OK;

    CIssGDIEx gdiMySrc;

    CBHRE((gdiSrc.GetDC() != NULL), _T("ScaleImage source DC is NULL"), E_INVALIDARG);

    int iOldWidth	= gdiSrc.GetWidth();
    int iOldHeight	= gdiSrc.GetHeight();
    int iNewWidth	= sizeDest.cx;
    int iNewHeight	= sizeDest.cy;

    float xScale, yScale, fX, fY;
    xScale = (float)iOldWidth  / (float)iNewWidth;
    yScale = (float)iOldHeight / (float)iNewHeight;

    // make sure we can access the bits for gdiSrc
    if(gdiSrc.GetBits() == NULL)
        gdiSrc.ConvertToDIB();

    gdiMySrc.Create(gdiSrc.GetDC(), gdiSrc.GetWidth(), gdiSrc.GetHeight(), TRUE, TRUE, FALSE, 24);

    // go through and convert anything transparent to black so nothing else is affected by the pink color
    if(bUseTransparencies)
    {
        for(int y=0; y<gdiSrc.GetHeight(); y++)
        {
            for(int x=0; x<gdiSrc.GetWidth(); x++)
            {
                if(gdiSrc.GetPixelColor(x,y) == crTranparentColor)
                    gdiMySrc.SetPixelColor(x,y, RGB(0,0,0));
            }
        }
    }

    // create our destination image
    gdiDest.Create(gdiSrc.GetDC(), iNewWidth, iNewHeight, FALSE, TRUE, FALSE, 24);

    // if we have a larger image to draw to
    if (!(iOldWidth > iNewWidth && iOldHeight > iNewHeight)) 
    {
        long ifX, ifY, ifX1, ifY1, xmax, ymax;
        float ir1, ir2, ig1, ig2, ib1, ib2, dx, dy;
        BYTE r,g,b;
        COLORREF rgb1, rgb2, rgb3, rgb4;
        xmax = iOldWidth - 1;
        ymax = iOldHeight- 1;
        for(long y=0; y<iNewHeight; y++)
        {
            fY = y * yScale;
            ifY = (int)fY;
            ifY1 = min(ymax, ifY+1);
            dy = fY - ifY;
            for(long x=0; x<iNewWidth; x++)
            {
                fX = x * xScale;
                ifX = (int)fX;
                ifX1 = min(xmax, ifX+1);
                dx = fX - ifX;

                // Interpolate using the four nearest pixels in the source
                rgb1=gdiMySrc.GetPixelColor(ifX,ifY);
                rgb2=gdiMySrc.GetPixelColor(ifX1,ifY);
                rgb3=gdiMySrc.GetPixelColor(ifX,ifY1);
                rgb4=gdiMySrc.GetPixelColor(ifX1,ifY1);

                // Interpolate in x direction:
                ir1 = GetRValue(rgb1)*(1 - dy) + GetRValue(rgb3)*dy;
                ig1 = GetGValue(rgb1)*(1 - dy) + GetGValue(rgb3)*dy;
                ib1 = GetBValue(rgb1)*(1 - dy) + GetBValue(rgb3)*dy;
                ir2 = GetRValue(rgb2)*(1 - dy) + GetRValue(rgb4)*dy;
                ig2 = GetGValue(rgb2)*(1 - dy) + GetGValue(rgb4)*dy;
                ib2 = GetBValue(rgb2)*(1 - dy) + GetBValue(rgb4)*dy;
                // Interpolate in y:
                r = (BYTE)(ir1 * (1 - dx) + ir2 * dx);
                g = (BYTE)(ig1 * (1 - dx) + ig2 * dx);
                b = (BYTE)(ib1 * (1 - dx) + ib2 * dx);
                // Set output
                gdiDest.SetPixelColor(x,y,RGB(r,g,b));
            }
        } 
    } 
    else 
    {
        //high resolution shrink
        const long ACCURACY = 1000;
        long i,j; // index for faValue
        long x,y; // coordinates in  source image
        BYTE* pSource;
        BYTE* pDest = (BYTE*)gdiDest.GetBits();
        long* naAccu  = new long[3 * iNewWidth + 3];
        long* naCarry = new long[3 * iNewWidth + 3];
        long* naTemp;
        long  nWeightX,nWeightY;
        float fEndX;
        long nScale = (long)(ACCURACY * xScale * yScale);

        memset(naAccu,  0, sizeof(long) * 3 * iNewWidth);
        memset(naCarry, 0, sizeof(long) * 3 * iNewWidth);

        int u, v = 0; // coordinates in dest image
        float fEndY = yScale - 1.0f;
        for (y = 0; y < iOldHeight; y++)
        {
            pSource = (BYTE*)gdiMySrc.GetBits() + y * gdiMySrc.GetBitmapInfo().bmWidthBytes;
            u = i = 0;
            fEndX = xScale - 1.0f;
            if ((float)y < fEndY) 
            {       
                // complete source row goes into dest row
                for (x = 0; x < iOldWidth; x++)
                {
                    if ((float)x < fEndX)
                    {       
                        // complete source pixel goes into dest pixel
                        for (j = 0; j < 3; j++)	
                            naAccu[i + j] += (*pSource++) * ACCURACY;
                    } 
                    else 
                    {       
                        // source pixel is splitted for 2 dest pixels
                        nWeightX = (long)(((float)x - fEndX) * ACCURACY);
                        for (j = 0; j < 3; j++)
                        {
                            naAccu[i] += (ACCURACY - nWeightX) * (*pSource);
                            naAccu[3 + i++] += nWeightX * (*pSource++);
                        }
                        fEndX += xScale;
                        u++;
                    }
                }
            } 
            else 
            {       
                // source row is splitted for 2 dest rows       
                nWeightY = (long)(((float)y - fEndY) * ACCURACY);
                for (x = 0; x < iOldWidth; x++)
                {
                    if ((float)x < fEndX)
                    {       
                        // complete source pixel goes into 2 pixel
                        for (j = 0; j < 3; j++)
                        {
                            naAccu[i + j] += ((ACCURACY - nWeightY) * (*pSource));
                            naCarry[i + j] += nWeightY * (*pSource++);
                        }
                    } 
                    else 
                    {       
                        // source pixel is splitted for 4 dest pixels
                        nWeightX = (int)(((float)x - fEndX) * ACCURACY);
                        for (j = 0; j < 3; j++) 
                        {
                            naAccu[i] += ((ACCURACY - nWeightY) * (ACCURACY - nWeightX)) * (*pSource) / ACCURACY;
                            *pDest++ = (BYTE)(naAccu[i] / nScale);
                            naCarry[i] += (nWeightY * (ACCURACY - nWeightX) * (*pSource)) / ACCURACY;
                            naAccu[i + 3] += ((ACCURACY - nWeightY) * nWeightX * (*pSource)) / ACCURACY;
                            naCarry[i + 3] = (nWeightY * nWeightX * (*pSource)) / ACCURACY;
                            i++;
                            pSource++;
                        }
                        fEndX += xScale;
                        u++;
                    }
                }
                if (u < iNewWidth)
                { // possibly not completed due to rounding errors
                    for (j = 0; j < 3; j++) 
                        *pDest++ = (BYTE)(naAccu[i++] / nScale);
                }
                naTemp = naCarry;
                naCarry = naAccu;
                naAccu = naTemp;
                memset(naCarry, 0, sizeof(int) * 3);    // need only to set first pixel zero
                pDest = (BYTE*)gdiDest.GetBits() + (++v * gdiDest.GetBitmapInfo().bmWidthBytes);
                fEndY += yScale;
            }
        }
        if (v < iNewHeight)
        {	
            // possibly not completed due to rounding errors
            for (i = 0; i < 3 * iNewWidth; i++) 
                *pDest++ = (BYTE)(naAccu[i] / nScale);
        }
        delete [] naAccu;
        delete [] naCarry;
    }

    // properly set the transparent colors
    if(bUseTransparencies)
    {
        for(long y=0; y<iNewHeight; y++)
        {
            fY = (float)(y * yScale + 0.5);
            for(long x=0; x<iNewWidth; x++)
            {
                fX = (float)(x * xScale + 0.5);
                if(gdiSrc.GetPixelColor((int)fX, (int)fY) == TRANSPARENT_COLOR)
                    gdiDest.SetPixelColor(x,y, TRANSPARENT_COLOR);
            }
        }
    }

    // if the source has alpha values then we need to bring those over too
    if(gdiSrc.GetAlpha())
    {
        gdiDest.InitAlpha(TRUE);
        for(long y=0; y<iNewHeight; y++)
        {
            fY = (float)(y * yScale + 0.5);//put the correction factors back in - Adrian - Nov 17/09
            fY = (fY>gdiSrc.GetHeight()-1?gdiSrc.GetHeight()-1:fY);
            for(long x=0; x<iNewWidth; x++)
            {
                fX = (float)(x * xScale + 0.5);
                fX = (fX>gdiSrc.GetWidth()-1?gdiSrc.GetWidth()-1:fX);
                gdiDest.SetAlphaValue(x,y, gdiSrc.GetAlphaValue((int)fX, (int)fY));
            }
        }
    }

    // make sure we're in the correct bit depth
    hr = gdiDest.ConvertToBPP(gdiSrc.GetBPP());

Error:
    return S_OK;//BUGBUG
}

HRESULT ScaleImageFast(CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest, SIZE sizeDest, BOOL bUseTransparencies, COLORREF crTranparentColor)
{
    HRESULT hr = S_OK;

    CIssGDIEx gdiMySrc;

    CBHRE((gdiSrc.GetDC() != NULL), _T("ScaleImage source DC is NULL"), E_INVALIDARG);

    int iOldWidth	= gdiSrc.GetWidth();
    int iOldHeight	= gdiSrc.GetHeight();
    int iNewWidth	= sizeDest.cx;
    int iNewHeight	= sizeDest.cy;

    // make sure we can access the bits for gdiSrc
    if(gdiSrc.GetBits() == NULL)
        gdiSrc.ConvertToDIB();

    gdiMySrc.Create(gdiSrc.GetDC(), gdiSrc.GetWidth(), gdiSrc.GetHeight(), TRUE, TRUE, FALSE, gdiSrc.GetBPP());

    // go through and convert anything transparent to black so nothing else is affected by the pink color
    if(bUseTransparencies)
    {
        for(int y=0; y<gdiSrc.GetHeight(); y++)
        {
            for(int x=0; x<gdiSrc.GetWidth(); x++)
            {
                if(gdiSrc.GetPixelColor(x,y) == crTranparentColor)
                    gdiMySrc.SetPixelColor(x,y, RGB(0,0,0));
            }
        }
    }

    // create our destination image
    gdiDest.Create(gdiSrc.GetDC(), iNewWidth, iNewHeight, FALSE, TRUE, FALSE, gdiSrc.GetBPP());
    if(bUseTransparencies == FALSE)
        gdiDest.InitAlpha(TRUE);

    //now loop through and set all of the pixels
    for(long y=0; y<iNewHeight; y++)
    {
        for(long x=0; x<iNewWidth; x++)
        {   
            gdiDest.SetPixelColor(x, y, gdiMySrc.GetPixelColor(x*iOldWidth/iNewWidth, y*iOldHeight/iNewHeight));
            if(bUseTransparencies == FALSE)        
                gdiDest.SetAlphaValue(x, y, gdiSrc.GetAlphaValue(x*iOldWidth/iNewWidth, y*iOldHeight/iNewHeight));
        }
    }
Error:
    return hr;
}