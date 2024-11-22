/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssGDIFX.h
// Abstract: Usefull Effects rountines base on IssGDIEx
//  
/***************************************************************************************************************/

#pragma once

#include "IssGDIEx.h"

enum Transition_Type
{
    TRANSITION_Fade,
    TRANSITION_Scroll_Vertical,
    TRANSITION_Scroll_Horizontal,
};

enum Animation_Type
{
    ANIMATION_Normal,        // 900
    ANIMATION_Fast,            // 1500
    ANIMATION_Slow,            // 500
    ANIMATION_None,            // 0
};

enum SCROLL_Direction
{
    SCROLL_Vertical,
    SCROLL_Horizontal,
};

// progressive animation function prototype
typedef void (*FNDRAWDESTFRAME)(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);

HRESULT AnimateFromTo(HDC hdcScreen,
                      CIssGDIEx&    gdiBackground,
                      CIssGDIEx&    gdiAniObject,
                      POINT        ptFrom,
                      POINT        ptTo,
                      int        iXStart,
                      int        iYStart,
                      int        iWidth,
                      int        iHeight,
                      int        iDrawFrames = 5
                      );
HRESULT FadeIn(HWND hWnd, 
               CIssGDIEx& gdiDest, 
               double dbSpeed = 900.0, 
               FNDRAWDESTFRAME fnDest = NULL, 
               LPVOID lpClass = NULL
               );
HRESULT FadeIn(HDC hdcScreen, 
               CIssGDIEx& gdiDest, 
               double dbSpeed = 900.0, 
               FNDRAWDESTFRAME fnDest = NULL, 
               LPVOID lpClass = NULL
               );
HRESULT SweepRight(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SweepLeft(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SlideRight(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SlideLeft(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SlideUp(HWND hWnd, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SlideDown(HWND hWnd, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT Trapazoid(HDC hdcScreen, 
                  CIssGDIEx& gdiDest, 
                  POINT ptTL, POINT ptTR, POINT ptBL, POINT ptBR, 
                  double dbSpeed = 900.0
                  );
HRESULT CubeHorzUp(HDC hdcScreen, 
                   CIssGDIEx& gdiDest, 
                   RECT rcSrc, 
                   RECT rcDest, 
                   double dbSpeed = 900
                   );
HRESULT SlideUp(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT SlideDown(HDC hdcScreen, CIssGDIEx& gdiDest, double dbSpeed = 900.0);
HRESULT AlphaTransition(HDC hdcScreen, 
                        RECT& rcArea, 
                        CIssGDIEx& gdiSrc, 
                        RECT& rcSrc,
                        CIssGDIEx& gdiDest, 
                        RECT& rcDest, 
                        double dbSpeed = 900.0, 
                        FNDRAWDESTFRAME fnDest = NULL, 
                        LPVOID lpClass = NULL
                        );
HRESULT SmoothTransition(HDC hdcScreen, RECT& rcArea, CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest);
HRESULT ScrollTransition(HDC hdcScreen, 
                         RECT& rcArea, 
                         CIssGDIEx& gdiSrc, 
                         RECT& rcSrc, 
                         CIssGDIEx& gdiDest, 
                         RECT& rcDest, 
                         SCROLL_Direction eScroll = SCROLL_Vertical, 
                         BOOL bDirection = FALSE, 
                         double dbSpeed = 900.0
                         );
HRESULT Animate(HDC hdcScreen, 
                RECT& rcArea, 
                CIssGDIEx& gdiSrc, 
                RECT& rcSrc,
                CIssGDIEx& gdiDest, 
                RECT& rcDest, 
                Transition_Type eTransition, 
                BOOL bDirection, 
                Animation_Type eSpeed = ANIMATION_Normal
                );
int     GetCurrentFrame(DWORD &dwStartTime, int iTargetNumFrames = 100, double dbAniTime = 900.0);

HRESULT ScaleImage(CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest, SIZE sizeDest, BOOL bUseTransparencies, COLORREF crTranparentColor);
HRESULT ScaleImageFast(CIssGDIEx& gdiSrc, CIssGDIEx& gdiDest, SIZE sizeDest, BOOL bUseTransparencies, COLORREF crTranparentColor);


