/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssGDIEx.h 
// Abstract: Windows base class to simplify handling common windows messages 
// 
/***************************************************************************************************************/ 

#pragma once

#include "windows.h"
#include "tchar.h"
#ifdef UNDER_CE
#include <imaging.h>
#else
#include "unknwn.h"
#include <gdiplus.h>
#pragma comment( lib, "gdiplus.lib" )
#endif  //UNDER_CE

#define TRANSPARENT_COLOR    0xF800F8
#define TRANSPARENT_WORD     0xF81F
#define ALPHA_Full           255

#define WIDTH(rc) (rc.right - rc.left)
#define HEIGHT(rc) (rc.bottom - rc.top)

enum EnumMaintainAspect
{
    ASPECT_None,
    ASPECT_Full,
    ASPECT_Cropped,
};

class CIssGDIEx  
{
public:
    CIssGDIEx();
    virtual ~CIssGDIEx();

    operator HDC(){return m_hDC;};

    HRESULT         Create( HDC hDC, 
                            int iWidth, 
                            int iHeight, 
                            BOOL bCopyContents = FALSE, 
                            BOOL bCreateBits = TRUE, 
                            BOOL bPaintTransparent = FALSE, 
                            int iBpp = -1
                            );
    HRESULT         Create( HDC hDC, 
                            RECT& rcSize, 
                            BOOL bCopyContents = FALSE, 
                            BOOL bCreateBits = TRUE, 
                            BOOL bPaintTransparent = FALSE, 
                            int iBpp = -1
                            );
    HRESULT         CreateReflection(CIssGDIEx& gdiImage);
    HRESULT         InitAlpha(BOOL bSetTransparent = FALSE);
    HRESULT         ResetAlpha(BOOL bSetTransparent = FALSE);
#ifdef UNDER_CE
    HRESULT         LoadImage(  UINT uiResource, 
                                HWND hWnd, 
                                HINSTANCE hInst, 
                                BOOL bCreateBits = TRUE,
                                int iDesiredWidth = -1,
                                int iDesiredHeight= -1,
                                EnumMaintainAspect eAspect = ASPECT_None);
     
    HRESULT         LoadImage(  TCHAR* szFileName,
                                HWND hWnd,
                                BOOL bCreateBits = TRUE,
                                int iDesiredWidth = -1,
                                int iDesiredHeight= -1,
                                EnumMaintainAspect eAspect = ASPECT_None);
#else
     HRESULT         LoadImage(  UINT uiResource, 
                                HWND hWnd, 
                                HINSTANCE hInst, 
                                BOOL bCreateBits = TRUE);
     HRESULT         LoadImage(  TCHAR* szFileName,
                                HWND hWnd,
                                BOOL bCreateBits = TRUE);
#endif


    HRESULT         LoadImage(  HBITMAP hbmImage,
                                HWND hWnd,
                                BOOL bCreateBits = TRUE);
    void            Destroy();
    HDC             GetDC(){return m_hDC;};
    int             GetWidth(){return (int)m_sBitmap.bmWidth;};
    int             GetHeight(){return (int)m_sBitmap.bmHeight;};
    BITMAP          GetBitmapInfo(){return m_sBitmap;};
    HBITMAP         GetOldHBitmap(){return m_hbmOld;};
    HBITMAP         GetHBitmap(){return m_hbmBitmap;};
    LPVOID          GetBits(){return m_pBuffer;};
    LPBYTE          GetAlpha(){return m_pAlpha;};
    int             GetBPP(){return m_sBitmap.bmBitsPixel;};
    HRESULT         ConvertToBPP(int iNewBPP);    
    COLORREF        GetPixelColor(int iXpos, int iYpos);
    HRESULT         SetPixelColor(int iXpos, int iYpos, COLORREF crColor);
    BYTE            GetAlphaValue(int iXpos, int iYpos);
    HRESULT         SetAlphaValue(int iXpos, int iYpos, BYTE btAlpha);
    HRESULT         GetAlphaMask(CIssGDIEx& gdiMask);
    HRESULT         SetAlphaMask(CIssGDIEx& gdiMask, BOOL bTakeMin = FALSE);
    HRESULT         ConvertToDDB();
    HRESULT         ConvertToDIB();
    HRESULT         FlipVertical();
    HRESULT         FlipHorizontal();
    HRESULT         Rotate(BOOL bLeft = FALSE);//defaults to clockwise
    HRESULT         GrayScale();
    HRESULT         MoveGdiMemory(HDC& hDC, 
                                  BITMAP& sBitmap, 
                                  HBITMAP& hbmBitmap, 
                                  HBITMAP& hbmOld, 
                                  LPVOID& pBuffer,
                                  LPBYTE& pAlpha,
                                  HBRUSH& hBrush, 
                                  HBRUSH& hOldBrush, 
                                  HPEN& hPen, 
                                  HPEN& hOldPen, 
                                  HFONT& hFont, 
                                  HFONT& hOldFont
                                  );
    HRESULT         MoveGdiMemory(CIssGDIEx& gdiDest);
	HRESULT			ScaleImage(int iCX, int iCY);

    // static functions
    static HFONT    CreateFont(int iHeight, int iWeight = FW_NORMAL, BOOL bUseClearType = FALSE, TCHAR* szFont = _T("Tahoma"), int iRotation = 0, int iWidth = 0, BOOL bItalics = FALSE);
    static BOOL     DeleteFont(HFONT& hFont);

    // set our GDI drawing objects
    BOOL            SetBrush(COLORREF crColor);
    BOOL            SetPen(COLORREF crColor, int iPenStyle = PS_SOLID, int iWidth = 0);
    BOOL            SetFont(int iHeight, int iWeight = FW_NORMAL, BOOL bUseClearType = FALSE, TCHAR* szFont = _T("Tahoma"), int iRotation = 0);
    BOOL            SetBrush(HBRUSH hBrush);
    BOOL            SetPen(HPEN hPen);
    BOOL            SetFont(HFONT hFont);
    BOOL            SetTextColor(COLORREF crColor, int iBkgndMode = TRANSPARENT);

    static HPEN     CreatePen(COLORREF crColor, int iPenStyle = PS_SOLID, int iWidth = 0);
    static HBRUSH   CreateBrush(COLORREF crColor);
    
    HBRUSH          GetBrush(){return m_hBrush;};
    HPEN            GetPen(){return m_hPen;};
    HFONT           GetFont(){return m_hFont;};

    int             DrawText(TCHAR* szText,RECT& rc, UINT uiFormat = DT_LEFT);

    HRESULT         SaveToFile(TCHAR* szFileName);
private:    // Functions
    HRESULT         ExtractAlphaValues(CIssGDIEx& gdiWhite);
    HBITMAP         Create16BitDIB(int iWidth, int iHeight);
    HBITMAP         Create24BitDIB(int iWidth, int iHeight);
    HBITMAP         Create32BitDIB(int iWidth, int iHeight);
    HBITMAP         CreateUnknownDIB(int iWidth, int iHeight);
    HRESULT         CreateReflection16(CIssGDIEx& gdiImage);
    HRESULT         CreateReflection24(CIssGDIEx& gdiImage);
    HRESULT         FlipVertical16();
    HRESULT         FlipVertical24();
    HRESULT         FlipHorizontal16();
    HRESULT         FlipHorizontal24();
    HRESULT         GrayScale16();
    HRESULT         GrayScale24();
    int             GetSystemBitsPerPixel();
    HRESULT         DrawLoadedImage(HWND hWnd,
                                    BOOL bCreateBits,
                                    int iWidth,
                                    int iHeight,
#ifdef UNDER_CE
                                    PixelFormat sPixelFormat,
                                    IImage* pImage,
#else
                                    Gdiplus::PixelFormat sPixelFormat,
                                    Gdiplus::Image* pImage,
#endif
                                    
                                    int iPImageWidth = -1,
                                    int iPImageHeight = -1
                                    );


private:    // variables
    HDC             m_hDC;            // Device Context
    BITMAP          m_sBitmap;        // Containing bitmap information
    HBITMAP         m_hbmBitmap;    // Created Bitmap
    HBITMAP         m_hbmOld;        // Old Bitmap handle
    LPVOID          m_pBuffer;        // point to the bits
    LPBYTE          m_pAlpha;       // alpha values 0 - 31

    HBRUSH          m_hBrush;        // a Brush that can be set and added to the DC
    HBRUSH          m_hOldBrush;    // the original brush created
    HPEN            m_hPen;            // a Pen that can be set and added to the DC
    HPEN            m_hOldPen;        // the original pen created
    HFONT           m_hFont;        // a font that can be set and added to the DC
    HFONT           m_hOldFont;        // the original font created
};

