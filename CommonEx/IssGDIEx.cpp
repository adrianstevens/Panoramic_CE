/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssGDIEx.cpp 
// Abstract: Windows base class to simplify handling common windows messages 
// 
/***************************************************************************************************************/ 

#include "IssGDIEx.h"
#include "math.h"
#include "IssString.h"
#include "IssDebug.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#define INITGUID
#include <initguid.h>
#undef INITGUID

#ifdef UNDER_CE
#include <imgguids.h>
#endif

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

struct DIBINFOEX : public BITMAPINFO
{
    RGBQUAD	 arColors[255];    // Color table info - adds an extra 255 entries to palette

    operator LPBITMAPINFO()          { return (LPBITMAPINFO) this; }
    operator LPBITMAPINFOHEADER()    { return &bmiHeader;          }
    RGBQUAD* ColorTable()            { return bmiColors;           }
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIssGDIEx::CIssGDIEx()
:m_hDC(NULL)
,m_hbmBitmap(NULL)
,m_hbmOld(NULL)
,m_pBuffer(NULL)
,m_hBrush(NULL)
,m_hOldBrush(NULL)
,m_hPen(NULL)        
,m_hOldPen(NULL)
,m_hFont(NULL)    
,m_hOldFont(NULL)
,m_pAlpha(NULL)
{
    ZeroMemory(&m_sBitmap, sizeof(BITMAP));
}

CIssGDIEx::~CIssGDIEx()
{
    Destroy();
}

/********************************************************************
Function:    Destroy

Arguments:                

Returns:    TRUE is successful 

Comments:    Clean's up pointers and buffers
            Can be called safely at any time
*********************************************************************/
void CIssGDIEx::Destroy()
{
    if(m_hOldFont && m_hDC)
    {
        SelectObject(m_hDC, m_hOldFont);
        m_hOldFont = NULL;
    }

    if(m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = NULL;
    }

    if(m_hOldBrush && m_hDC)
    {
        SelectObject(m_hDC, m_hOldBrush);
        m_hOldBrush = NULL;
    }

    if(m_hBrush)
    {
        DeleteObject(m_hBrush);
        m_hBrush = NULL;
    }

    if(m_hOldPen && m_hDC)
    {
        SelectObject(m_hDC, m_hOldPen);
        m_hOldPen = NULL;
    }

    if(m_hPen)
    {
        DeleteObject(m_hPen);
        m_hPen = NULL;
    }

    if(m_hbmOld)
    {
        SelectObject(m_hDC, m_hbmOld);
        m_hbmOld = NULL;
    }

    if(m_hDC)
    {
        DeleteDC(m_hDC);
        m_hDC = NULL;
    }

    if(m_hbmBitmap)
    {
        DeleteObject(m_hbmBitmap);
        m_hbmBitmap = NULL;
    }

    SAFE_DELETE_ARRAY(m_pAlpha);
	m_pBuffer = NULL;

    ZeroMemory(&m_sBitmap, sizeof(BITMAP));
    return;
}

/****************************************************************************************

    Create()

    Creates our display buffer

    hDC                    Device context that we're basing our GDI from
    rcSize                Size of the new buffer
    bCopyContents        Option to copy the contents of the hDC into our new buffer
    bCreateBits            Option to create buffer as a Dibsection...create for graphics but
                            doesn't allow the use of clear type hence its defaulted to false
    bPaintTransparent    Should we draw the image all in pink after we create it, only works if bCopyContents is FALSE
    iBpp                Specify the bits per pixel.  Valid values are 16 & 24 ONLY

****************************************************************************************/
HRESULT CIssGDIEx::Create(HDC hDC, 
                     RECT& rcSize, 
                     BOOL bCopyContents, 
                     BOOL bCreateBits, 
                     BOOL bPaintTransparent, 
                     int iBpp /*= 16*/
                     )
{
    HRESULT hr = S_OK;

    // clean up any memory that was there
    Destroy();

    m_hDC = CreateCompatibleDC(hDC);
    CPHR(m_hDC, _T("CreateCompatibleDC failed"));

    int iWidth    = WIDTH(rcSize);
    int iHeight    = HEIGHT(rcSize);

    // Create bitmap and receive pointer to bits into pBuffer
    if(bCreateBits)
    {
        switch(iBpp)
        {
        case 32:
            m_hbmBitmap    = Create32BitDIB(iWidth, iHeight);
            break;
        case 24:
            m_hbmBitmap    = Create24BitDIB(iWidth, iHeight);
            break;
        case 16:
            m_hbmBitmap    = Create16BitDIB(iWidth, iHeight);
            break;
        default:
            m_hbmBitmap = CreateUnknownDIB(iWidth, iHeight);
            break;
        }
        CPHR(m_hbmBitmap, _T("CreateDIBSection failed"));    
    }
    else
    {
        m_hbmBitmap            = CreateCompatibleBitmap(hDC, iWidth, iHeight);
        CPHR(m_hbmBitmap, _T("CreateCompatibleBitmap failed"));
    }

    m_hbmOld = (HBITMAP)SelectObject(m_hDC,m_hbmBitmap);

    // get the dimensions
    GetObject(m_hbmBitmap, sizeof(BITMAP), &m_sBitmap);

    // do we want to copy what is in the original DC?
    if(bCopyContents)
    {
        BitBlt(m_hDC,
               0,0,
               iWidth,iHeight,
               hDC,
               rcSize.left,rcSize.top,
               SRCCOPY);
    }
    else if(bPaintTransparent)
    {
        RECT rc;
        SetRect(&rc, 0, 0, iWidth, iHeight);
        FillRect(*this, rc, TRANSPARENT_COLOR);
    }

Error:
    if(FAILED(hr))
        Destroy();

    return hr;
}


/****************************************************************************************

Comments above

This implementation allows passing in a Width and Height instead of a Rect

****************************************************************************************/
HRESULT CIssGDIEx::Create(HDC hDC, 
                     int iWidth, 
                     int iHeight, 
                     BOOL bCopyContents, 
                     BOOL bCreateBits, 
                     BOOL bPaintTranparent, 
                     int iBpp /*= 16*/
                     )
{
    RECT rcSize;
    SetRect(&rcSize, 0, 0, iWidth, iHeight);

    return Create(hDC, rcSize, bCopyContents, bCreateBits, bPaintTranparent, iBpp);
}

HRESULT CIssGDIEx::InitAlpha(BOOL bSetTransparent)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC, _T("no HDC yet"));

    if(m_pAlpha)
    {
        // we already have an alpha array so just reset it
        hr = ResetAlpha(bSetTransparent);
        goto Error;
    }    

    m_pAlpha = new BYTE[m_sBitmap.bmWidth*m_sBitmap.bmHeight];
    CPHR(m_pAlpha, _T("m_pAlpha memory not created"));
    hr = ResetAlpha(bSetTransparent);

Error:
    return hr;
}

HRESULT CIssGDIEx::ResetAlpha(BOOL bSetTransparent)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC&&m_pAlpha, _T("no m_pAlpha"));

    BYTE btValue = 0;
    switch(m_sBitmap.bmBitsPixel)
    {
    case 16:
        btValue = (bSetTransparent?0:ALPHA_Full);
        break;
    default:
        btValue = (bSetTransparent?0:255);
        break;
    }
    memset(m_pAlpha, btValue, sizeof(BYTE)*m_sBitmap.bmWidth*m_sBitmap.bmHeight);
Error:
    return hr;
}

HRESULT CIssGDIEx::CreateReflection(CIssGDIEx& gdiImage)
{
    HRESULT hr = S_OK;
    CBARG(gdiImage.GetDC()&&gdiImage.GetBits()&&gdiImage.GetWidth()&&gdiImage.GetHeight(), _T(""));

    switch(gdiImage.GetBitmapInfo().bmBitsPixel)
    {
    case 16:
        hr = CreateReflection16(gdiImage);
        break;
    default:
        hr = CreateReflection24(gdiImage);
        break;
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::CreateReflection16(CIssGDIEx& gdiImage)
{
    HRESULT hr = S_OK;

    hr = Create(gdiImage.GetDC(), gdiImage.GetWidth(), gdiImage.GetHeight()/3, FALSE, TRUE, 16);
    CHR(hr, _T("CreateReflection16 - could not create gdi"));

    hr = InitAlpha(TRUE);
    CHR(hr, _T("CreateReflection16 - could not create Alpha"));

    DWORD        dwWidthBytesDest    = (DWORD)gdiImage.GetBitmapInfo().bmWidthBytes;

    LPBYTE       lpAlphaSrc= (gdiImage.GetAlpha()?(LPBYTE)(gdiImage.GetAlpha() + GetWidth()*GetHeight()):NULL);
    LPBYTE       lpAlphaDest= m_pAlpha;
    WORD*        lprgbSrc  = (WORD*)((LPBYTE)gdiImage.GetBits() + dwWidthBytesDest*GetHeight());;
    WORD*        lprgbDest = (WORD*)((LPBYTE)m_pBuffer);

    BYTE         btGradient;

    for(int y=0; y<GetHeight(); y++)
    {
        btGradient = 110*y/GetHeight();

        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but upside down
            lprgbDest[x] = lprgbSrc[x];

            // copy the alpha, if there is a src alpha then scale it
            lpAlphaDest[x] = lpAlphaSrc?btGradient*lpAlphaSrc[x]/255:btGradient;
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc - dwWidthBytesDest);
        lpAlphaDest = lpAlphaDest + GetWidth();
        if(lpAlphaSrc)
            lpAlphaSrc = lpAlphaSrc - GetWidth();
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::CreateReflection24(CIssGDIEx& gdiImage)
{
    HRESULT hr = S_OK;

    hr = Create(gdiImage.GetDC(), gdiImage.GetWidth(), gdiImage.GetHeight()/3, FALSE, TRUE, 24);
    CHR(hr, _T("CreateReflection24 - could not create gdi"));

    hr = InitAlpha(TRUE);
    CHR(hr, _T("CreateReflection24 - could not create Alpha"));

    DWORD        dwWidthBytesDest    = (DWORD)gdiImage.GetBitmapInfo().bmWidthBytes;

    LPBYTE       lpAlphaSrc= (gdiImage.GetAlpha()?(LPBYTE)(gdiImage.GetAlpha() + GetWidth()*GetHeight()):NULL);
    LPBYTE       lpAlphaDest= m_pAlpha;
    RGBTRIPLE*        lprgbSrc  = (RGBTRIPLE*)((LPBYTE)gdiImage.GetBits() + dwWidthBytesDest*GetHeight());;
    RGBTRIPLE*        lprgbDest = (RGBTRIPLE*)((LPBYTE)m_pBuffer);

    BYTE         btGradient;

    for(int y=0; y<GetHeight(); y++)
    {
        btGradient = 110*y/GetHeight();

        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but upside down
            lprgbDest[x].rgbtRed = lprgbSrc[x].rgbtRed;
            lprgbDest[x].rgbtGreen = lprgbSrc[x].rgbtGreen;
            lprgbDest[x].rgbtBlue = lprgbSrc[x].rgbtBlue;

            // copy the alpha, if there is a src alpha then scale it
            lpAlphaDest[x] = lpAlphaSrc?btGradient*lpAlphaSrc[x]/255:btGradient;
        }
        lprgbDest = (RGBTRIPLE*)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (RGBTRIPLE*)((LPBYTE)lprgbSrc - dwWidthBytesDest);
        lpAlphaDest = lpAlphaDest + GetWidth();
        if(lpAlphaSrc)
            lpAlphaSrc = lpAlphaSrc - GetWidth();
    }

Error:
    return hr;
}

/********************************************************************
Function:    ConvertToDDB

Arguments:    

Returns:    TRUE if successful

Comments:    Converts the current buffer/bitmap to a Device Dependant bitmap
            This is useful if we need to draw cleartype text over the bitmap
*********************************************************************/
HRESULT CIssGDIEx::ConvertToDDB()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    // nothing has been created yet
    CBARG((m_hDC), _T("m_hDC == NULL"));

    // if there are no bits then we know we are good.
    if(m_pBuffer == NULL)
        goto Error;

    // create new memory but in DDB and copy over the current image
    hr = gdiTemp.Create(m_hDC, m_sBitmap.bmWidth, m_sBitmap.bmHeight, TRUE, FALSE, m_sBitmap.bmBitsPixel);
    CHR(hr, _T("gdiTemp.Create failed"));

    // destroy all the old memory
    Destroy();

    // copy over all the info here
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
                          m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:

    // NOTE: we don't destroy gdiTemp because now all the memory is now pointing to this class
    return hr;
}

/********************************************************************
Function:    ConvertToBPP

Arguments:    

Returns:    TRUE if successful

Comments:    Converts the current buffer/bitmap to a certain BPP
            note: does not dither image. 
*********************************************************************/
HRESULT CIssGDIEx::ConvertToBPP(int iNewBPP)
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    // nothing has been created yet
    CBARG((m_hDC), _T("m_hDC == NULL"));

    if(m_sBitmap.bmBitsPixel == iNewBPP)
        goto Error; // we're good already

    // create new memory but in keep it if we want it in DIB or DDB
    hr = gdiTemp.Create(m_hDC, m_sBitmap.bmWidth, m_sBitmap.bmHeight, TRUE, (m_pBuffer?TRUE:FALSE), FALSE, iNewBPP);
    CHR(hr, _T("gdiTemp.Create failed"));

    RECT rc;
    SetRect(&rc, 0,0,m_sBitmap.bmWidth, m_sBitmap.bmHeight);

    // if it carries alpha values
    if(m_pAlpha)
        CopyAlpha(gdiTemp, rc, *this, 0, 0 );

    // destroy all the old memory
    Destroy();

    // copy over all the info here
    /*hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
                                m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);*/
	hr = gdiTemp.MoveGdiMemory(*this);

Error:

    // NOTE: we don't destroy gdiTemp because now all the memory is now pointing to this class
    return hr;
}

/********************************************************************
Function:    ConvertToDIB

Arguments:    

Returns:    TRUE if successful

Comments:    Converts the current buffer/bitmap to a Device Independant bitmap
            This will give us direct access to the bits of the image but cleartype no longer works when drawing text

*********************************************************************/
HRESULT CIssGDIEx::ConvertToDIB()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    // nothing has been created yet
    CBARG((m_hDC), _T("m_hDC == NULL"));

    // if there are bits then we know we are good.
    if(m_pBuffer)
        goto Error;

    // create new memory but in DIB and copy over the current image
    hr = gdiTemp.Create(m_hDC, m_sBitmap.bmWidth, m_sBitmap.bmHeight, TRUE, TRUE, m_sBitmap.bmBitsPixel);
    CHR(hr, _T("gdiTemp.Create failed"));

    // destroy all the old memory
    Destroy();

    // copy over all the info here
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:

    return hr;
}

/********************************************************************
Function:    MoveGdiMemory

Arguments:    gdiSrc

Returns:    TRUE if successful

Comments:    Moves the contents of our GDI to gdiDest.  Move NOT copy
*********************************************************************/
HRESULT CIssGDIEx::MoveGdiMemory(CIssGDIEx& gdiDest)
{
    // copy over all the info here
    return MoveGdiMemory(gdiDest.m_hDC, gdiDest.m_sBitmap, gdiDest.m_hbmBitmap, gdiDest.m_hbmOld, gdiDest.m_pBuffer, gdiDest.m_pAlpha,
        gdiDest.m_hBrush, gdiDest.m_hOldBrush, gdiDest.m_hPen, gdiDest.m_hOldPen, gdiDest.m_hFont, gdiDest.m_hOldFont);
}



/********************************************************************
Function:    MoveGdiMemory

Arguments:    See Win32 documentation

Returns:    TRUE if successful

Comments:    Moves the contents of our GDI to gdiDest.  Move NOT copy
*********************************************************************/
// literally copy over the memory locations to another memory class
HRESULT CIssGDIEx::MoveGdiMemory(HDC& hDC, 
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
                            )
{
    HRESULT hr = S_OK;

    CBARG((m_hDC), _T("m_hDC is NULL"));

    // copy all the memory references over
    hDC            = m_hDC;
    hbmBitmap    = m_hbmBitmap;
    hbmOld        = m_hbmOld;
    pBuffer        = m_pBuffer;
    pAlpha      = m_pAlpha;
    hBrush        = m_hBrush;
    hOldBrush    = m_hOldBrush;
    hPen        = m_hPen;
    hOldPen        = m_hOldPen;
    hFont        = m_hFont;
    hOldFont    = m_hOldFont;
    memcpy(&sBitmap, &m_sBitmap, sizeof(BITMAP));

    // now that all the memory is moved over we have to blank out our pointers here so we
    // don't accidentally destroy the memory while another class is using it
    m_hDC        = NULL;
    m_hbmBitmap    = NULL;
    m_hbmOld    = NULL;
    m_pBuffer    = NULL;
    m_pAlpha    = NULL;
    m_hBrush    = NULL;
    m_hOldBrush    = NULL;
    m_hPen        = NULL;
    m_hOldPen    = NULL;
    m_hFont        = NULL;
    m_hOldFont    = NULL;
    ZeroMemory(&m_sBitmap, sizeof(BITMAP));

Error:
    
    return hr;
}

#ifdef UNDER_CE
HRESULT CIssGDIEx::LoadImage(   TCHAR* szFileName,
                              HWND hWnd,
                              BOOL bCreateBits,
                              int iDesiredWidth /*= -1*/,
                              int iDesiredHeight /*= -1*/,
                              EnumMaintainAspect eAspect /* ASPECT_None */)
{
    HRESULT hr          = S_OK;
    IImage* pImage      = NULL;
    IImage* pDesiredImage = NULL;
    ImageInfo sImageInfo;

    CBARG(szFileName && *szFileName && hWnd, _T("szFileName or hWnd invalid"));

    static IImagingFactory * pImgFactory = NULL;
    if (!pImgFactory)
    {
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

        // Create the imaging class factory
        CoCreateInstance( CLSID_ImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IImagingFactory,
            (void **)&pImgFactory);
    }
    CPHR(pImgFactory, _T("pImgFactory is NULL"));

    // Create GDI+ image from file
    hr = pImgFactory->CreateImageFromFile(szFileName, &pImage);
    CHR(hr, _T("pImgFactory->CreateImageFromFile"));

    // get the size of the image and find out if it has any alpha values
    hr = pImage->GetImageInfo(&sImageInfo);
    CHR(hr, _T("pImage->GetImageInfo failed"));

    if(iDesiredWidth != -1 && iDesiredHeight != -1)
    {
        RECT rcLoadSize;

        SetRect(&rcLoadSize, 0, 0, iDesiredWidth, iDesiredHeight);

        if( ((double)sImageInfo.Width/(double)iDesiredWidth   )  > 
            ((double)sImageInfo.Height/(double)iDesiredHeight ) )

        {   //means that the ratio of the destination is narrower than the source ...
            if(eAspect == ASPECT_Cropped)
            {   //crop the sides.... so create a larger rect and we'll blit it properly later
                rcLoadSize.right = (LONG)((double)iDesiredHeight*(double)sImageInfo.Width/(double)sImageInfo.Height);
            }
            else if(eAspect == ASPECT_Full)
            {   //reduce the height and we'll draw it on a black background later
                rcLoadSize.bottom = (LONG)((double)iDesiredWidth*(double)sImageInfo.Height/(double)sImageInfo.Width);
            }

        }
        else //if we're actually exact then either code is fine
        {
            if(eAspect == ASPECT_Cropped)
            {
                rcLoadSize.bottom = (LONG)((double)iDesiredWidth*(double)sImageInfo.Height/(double)sImageInfo.Width);
            }
            else if(eAspect == ASPECT_Full)
            {
                rcLoadSize.right = (LONG)((double)iDesiredHeight*(double)sImageInfo.Width/(double)sImageInfo.Height);
            }
        }

            
      //  hr = pImage->GetThumbnail(iDesiredWidth, iDesiredHeight, &pDesiredImage);
        hr = pImage->GetThumbnail(WIDTH(rcLoadSize), HEIGHT(rcLoadSize), &pDesiredImage);
        CHR(hr, _T("hr = pImage->GetThumbnail(iDesiredWidth, iDesiredHeight, &pDesiredImage);"));

        pImage->Release();
        pImage = NULL;

        hr = DrawLoadedImage(hWnd, bCreateBits, iDesiredWidth, iDesiredHeight, sImageInfo.PixelFormat, pDesiredImage, WIDTH(rcLoadSize), HEIGHT(rcLoadSize));
    }
    else
    {
        hr = DrawLoadedImage(hWnd, bCreateBits, sImageInfo.Width, sImageInfo.Height, sImageInfo.PixelFormat, pImage);        
    }
    CHR(hr, _T("DrawLoadedImage"));

Error:
    if(pImage)
        pImage->Release();

    if(pDesiredImage)
        pDesiredImage->Release();

       return hr;
}
#else
HRESULT CIssGDIEx::LoadImage(   TCHAR* szFileName,
                                 HWND hWnd,
                                 BOOL bCreateBits
                                 )
{
    HRESULT hr = S_OK;
    Gdiplus::Image* pImage = NULL;
    Gdiplus::PixelFormat sPixelFormat;
    int iWidth, iHeight;
    ULONG_PTR   gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    CBARG(szFileName && *szFileName && hWnd, _T("szFileName or hWnd invalid"));

    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    pImage = Gdiplus::Image::FromFile(szFileName, TRUE);
    CPHR(pImage, _T("pImage"));

    // for testing
    /*{
    HDC dc = ::GetDC(hWnd);
    Gdiplus::Graphics g(dc);
    g.DrawImage(pImage, 0,0, 40, 40);
    ReleaseDC(hWnd, dc);
    }*/

    // get the size of the image and find out if it has any alpha values
    sPixelFormat= pImage->GetPixelFormat();
    iWidth      = pImage->GetWidth();
    iHeight     = pImage->GetHeight();

    hr = DrawLoadedImage(hWnd, bCreateBits, iWidth, iHeight, sPixelFormat, pImage);
    CHR(hr, _T("DrawLoadedImage"));

Error:
    if(pImage)
        delete pImage;

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return hr;
}
#endif // UNDER_CE

#ifdef UNDER_CE
HRESULT CIssGDIEx::LoadImage(UINT uiResource, 
                          HWND hWnd, 
                          HINSTANCE hInst, 
                          BOOL bCreateBits,
                          int iDesiredWidth /*= -1*/,
                          int iDesiredHeight /*= -1*/,
                          EnumMaintainAspect eAspect  /* ASPECT_None */)
{
    HRESULT hr          = S_OK;

    HRSRC hrsrc         = NULL;
    HGLOBAL hResource   = NULL;
    void*   pImageData  = NULL;
    IImage* pImage      = NULL;
    IImage* pDesiredImage = NULL;
    ImageInfo sImageInfo;

    CBARG((hWnd&&hInst&&uiResource), _T("no hWnd or hInst or uiResource"));

    static IImagingFactory * pImgFactory = NULL;
    if (!pImgFactory)
    {
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        //CHR(hr, _T("CoInitialize failed"));

        // Create the imaging class factory
        CoCreateInstance( CLSID_ImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IImagingFactory,
            (void **)&pImgFactory);
    }
    CPHR(pImgFactory, _T("pImgFactory is NULL"));

    // try and load the resource, NOTE: all resource images have to be kept in the "GIF" section
    hrsrc = FindResource(hInst, MAKEINTRESOURCE(uiResource), TEXT("GIF"));
    CPHR(hrsrc, _T("FindResource failed"));

    hResource = LoadResource(hInst, hrsrc);
    CPHR(hResource, _T("LoadResource failed"));

    // Get a pointer to the data
    pImageData = (void *)LockResource(hResource);
    CPHR(pImageData, _T("LockResource failed"));

    DWORD dwImageSize = SizeofResource(hInst, hrsrc);

    // Create GDI+ image from file
    hr = pImgFactory->CreateImageFromBuffer(pImageData,
                                            dwImageSize,
                                            DISPOSAL_NONE,
                                            &pImage);
    CHR(hr, _T("pImgFactory->CreateImageFromBuffer"));

    // get the size of the image and find out if it has any alpha values
    hr = pImage->GetImageInfo(&sImageInfo);
    CHR(hr, _T("pImage->GetImageInfo failed"));

    if(iDesiredWidth != -1 && iDesiredHeight != -1)
    {
		RECT rcLoadSize;

		SetRect(&rcLoadSize, 0, 0, iDesiredWidth, iDesiredHeight);

		if( ((double)sImageInfo.Width/(double)iDesiredWidth   )  > 
			((double)sImageInfo.Height/(double)iDesiredHeight ) )

		{   //means that the ratio of the destination is narrower than the source ...
			if(eAspect == ASPECT_Cropped)
			{   //crop the sides.... so create a larger rect and we'll blit it properly later
				rcLoadSize.right = (LONG)((double)iDesiredHeight*(double)sImageInfo.Width/(double)sImageInfo.Height);
			}
			else if(eAspect == ASPECT_Full)
			{   //reduce the height and we'll draw it on a black background later
				rcLoadSize.bottom = (LONG)((double)iDesiredWidth*(double)sImageInfo.Height/(double)sImageInfo.Width);
			}

		}
		else //if we're actually exact then either code is fine
		{
			if(eAspect == ASPECT_Cropped)
			{
				rcLoadSize.bottom = (LONG)((double)iDesiredWidth*(double)sImageInfo.Height/(double)sImageInfo.Width);
			}
			else if(eAspect == ASPECT_Full)
			{
				rcLoadSize.right = (LONG)((double)iDesiredHeight*(double)sImageInfo.Width/(double)sImageInfo.Height);
			}
		}

		hr = pImage->GetThumbnail(WIDTH(rcLoadSize), HEIGHT(rcLoadSize), &pDesiredImage);
		CHR(hr, _T("hr = pImage->GetThumbnail(iDesiredWidth, iDesiredHeight, &pDesiredImage);"));

		pImage->Release();
		pImage = NULL;

		hr = DrawLoadedImage(hWnd, bCreateBits, iDesiredWidth, iDesiredHeight, sImageInfo.PixelFormat, pDesiredImage, WIDTH(rcLoadSize), HEIGHT(rcLoadSize));
    }
    else
    {
        hr = DrawLoadedImage(hWnd, bCreateBits, sImageInfo.Width, sImageInfo.Height, sImageInfo.PixelFormat, pImage);        
    }
    CHR(hr, _T("DrawLoadedImage"));

Error:
    if(pImage)
        pImage->Release();

    if(pDesiredImage)
        pDesiredImage->Release();

    return hr;
}
#else
HRESULT CIssGDIEx::LoadImage(UINT uiResource, 
                             HWND hWnd, 
                             HINSTANCE hInst, 
                             BOOL bCreateBits
                             )
{
    HRESULT hr          = S_OK;

    HRSRC hrsrc         = NULL;
    HGLOBAL hResource   = NULL;
    void*   pImageData  = NULL;
    HGLOBAL hBuffer     = NULL; 
    IStream* pStream    = NULL;
    int iWidth, iHeight;
    ULONG_PTR   gdiplusToken;
    Gdiplus::Image* pImage = NULL;
    Gdiplus::PixelFormat sPixelFormat;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    CBARG((hWnd&&hInst&&uiResource), _T("no hWnd or hInst or uiResource"));

    // try and load the resource, NOTE: all resource images have to be kept in the "GIF" section
    hrsrc = FindResource(hInst, MAKEINTRESOURCE(uiResource), TEXT("GIF"));
    CPHR(hrsrc, _T("FindResource failed"));

    hResource = LoadResource(hInst, hrsrc);
    CPHR(hResource, _T("LoadResource failed"));

    // Get a pointer to the data
    pImageData = (void *)LockResource(hResource);
    CPHR(pImageData, _T("LockResource failed"));

    DWORD dwImageSize = SizeofResource(hInst, hrsrc);

    hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, dwImageSize);
    CPHR(hBuffer, _T("hBuffer GlobalAlloc"));

    void* pBuffer = ::GlobalLock(hBuffer);
    CPHR(pBuffer, _T("GlobalLock"));

    CopyMemory(pBuffer, pImageData, dwImageSize);

    hr = ::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream);
    CHR(hr, _T("CreateStreamOnHGlobal"));

    pImage = Gdiplus::Image::FromStream(pStream);
    pStream->Release();
    CPHR(pImage, _T("Gdiplus::Image::FromStream"));

    // get the size of the image and find out if it has any alpha values
    sPixelFormat= pImage->GetPixelFormat();
    iWidth      = pImage->GetWidth();
    iHeight     = pImage->GetHeight();

    hr = DrawLoadedImage(hWnd, bCreateBits, iWidth, iHeight, sPixelFormat, pImage);
    CHR(hr, _T("DrawLoadedImage"));

Error:
    if(pImage)
        delete pImage;

    if(hBuffer)
    {
        ::GlobalUnlock(hBuffer);
        ::GlobalFree(hBuffer);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return hr;
}
#endif // UNDER_CE

#ifdef UNDER_CE
HRESULT CIssGDIEx::LoadImage( HBITMAP hbmImage,
                              HWND hWnd,
                              BOOL bCreateBits)
{
    HRESULT hr = S_OK;
    BOOL bRes;
    CBARG(hbmImage&&hWnd, _T(""));
    BITMAP sBitmap;
    HDC hDC = NULL;
    HBITMAP hbmOld = NULL;

    bRes = (GetObject(hbmImage, sizeof(sBitmap), &sBitmap) == sizeof(sBitmap));
    CBHR(bRes,_T("GetObject"));

    CBARG(sBitmap.bmWidth&&sBitmap.bmHeight, _T(""));

    if(sBitmap.bmBitsPixel == 32)
    {
        HDC dc  = ::GetDC(hWnd);
        hDC    = CreateCompatibleDC(dc);
        hbmOld    = (HBITMAP)SelectObject(hDC, hbmImage);
        hr = Create(hDC, sBitmap.bmWidth, sBitmap.bmHeight, FALSE, TRUE, FALSE, 32);
        ReleaseDC(hWnd, dc);
        CHR(hr, _T("Create"));

        RECT rc;
        SetRect(&rc, 0, 0, sBitmap.bmWidth, sBitmap.bmHeight);
        FillRect(m_hDC, rc, RGB(0,0,0));    // draw it black first

        BLENDFUNCTION sPixelBlend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        AlphaBlend( m_hDC, 
                    0, 0, 
                    sBitmap.bmWidth, sBitmap.bmHeight, 
                    hDC,
                    0,0,
                    sBitmap.bmWidth, sBitmap.bmHeight,
                    sPixelBlend);

        CIssGDIEx gdiWhite;
        hr = gdiWhite.Create(m_hDC, sBitmap.bmWidth, sBitmap.bmHeight, FALSE, TRUE, FALSE, 32);
        CHR(hr, _T("gdiWhite.Create"));

        FillRect(gdiWhite.GetDC(), rc, RGB(255,255,255));    // draw it White

        AlphaBlend( gdiWhite.GetDC(), 
                    0, 0, 
                    sBitmap.bmWidth, sBitmap.bmHeight, 
                    hDC,
                    0,0,
                    sBitmap.bmWidth, sBitmap.bmHeight,
                    sPixelBlend);

        hr = ExtractAlphaValues(gdiWhite);
        CHR(hr, _T("ExtractAlphaValues failed"));
    }
    else
    {
        HDC dc  = ::GetDC(hWnd);
        hDC    = CreateCompatibleDC(dc);
        hbmOld    = (HBITMAP)SelectObject(hDC, hbmImage);
        hr = Create(hDC, sBitmap.bmWidth, sBitmap.bmHeight, TRUE, bCreateBits);
        ReleaseDC(hWnd, dc);
        CHR(hr, _T("Create"));
    }

    

Error:
    if(hbmOld && hDC)
        SelectObject(hDC, hbmOld);
    if(hDC)
        DeleteDC(hDC);

    return hr;
}
#else
HRESULT CIssGDIEx::LoadImage( HBITMAP hbmImage,
                             HWND hWnd,
                             BOOL bCreateBits
                             )
{
    HRESULT hr = S_OK;
    int iWidth, iHeight;
    ULONG_PTR   gdiplusToken;
    Gdiplus::Bitmap* pImage = NULL;
    Gdiplus::PixelFormat sPixelFormat;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    CBARG((hWnd&&hbmImage), _T("no hWnd or hInst or uiResource"));

    pImage = Gdiplus::Bitmap::FromHBITMAP(hbmImage, NULL);
    CPHR(pImage, _T("Gdiplus::Bitmap::FromHBITMAP"));

    // get the size of the image and find out if it has any alpha values
    sPixelFormat= pImage->GetPixelFormat();
    iWidth      = pImage->GetWidth();
    iHeight     = pImage->GetHeight();

    hr = DrawLoadedImage(hWnd, bCreateBits, iWidth, iHeight, sPixelFormat, pImage);
    CHR(hr, _T("DrawLoadedImage"));

Error:
    if(pImage)
        delete pImage;

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return hr;
}
#endif // UNDER_CE

HRESULT CIssGDIEx::DrawLoadedImage(   HWND hWnd,
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
                                      int iPImageWidth,
                                      int iPImageHeight
                                      )
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiWhite;

    if(iPImageWidth == -1)
        iPImageWidth = iWidth;
    if(iPImageHeight == -1)
        iPImageHeight = iHeight;

    HDC dc = ::GetDC(hWnd);
    // if we have alpha values in the image make sure that we create bits
    hr = Create(dc, iWidth, iHeight, FALSE, (bCreateBits||sPixelFormat&PixelFormatAlpha), FALSE, 24);
    ReleaseDC(hWnd, dc);
    CHR(hr, _T("Create failed"));

    RECT rc;
    SetRect(&rc, 0, 0, iWidth, iHeight);
    FillRect(m_hDC, rc, RGB(0,0,0));    // draw it black first

#ifdef UNDER_CE
    if(iPImageHeight == iHeight &&
        iPImageWidth == iWidth)
    {
    /*hr = */pImage->Draw(m_hDC, &rc, NULL);
    }
    else
    {   
        //start with it black
        FillRect(m_hDC, &rc, 0);
        int iXStart = 0;
        int iYStart = 0;

        //will be negative if we're cropping
        iXStart = (iWidth - iPImageWidth)/2;
        iYStart = (iHeight - iPImageHeight)/2;
        
        if(iXStart < 0 || iYStart < 0)
        {   //crop time
            //should never happen
            if(iXStart > 0)
                iXStart = 0;
            if(iYStart > 0)
                iYStart = 0;        

            CIssGDIEx gdiTemp;
            gdiTemp.Create(m_hDC, iPImageWidth, iPImageHeight);

            RECT rcImage;
            SetRect(&rcImage, 0, 0, iPImageWidth, iPImageHeight);
            pImage->Draw(gdiTemp.GetDC(), &rcImage, NULL);

            //now blt to our DC
            BitBlt(m_hDC, 0, 0, iWidth, iHeight, gdiTemp.GetDC(), 0 - iXStart, 0 - iYStart, SRCCOPY);
        }
        else
        {   
            //should never happen
            if(iXStart < 0)
                iXStart = 0;
            if(iYStart < 0)
                iYStart = 0;
            
            //draw centered
            RECT rcImage;
            SetRect(&rcImage, iXStart, iYStart, iPImageWidth + iXStart, iPImageHeight + iYStart);
            pImage->Draw(m_hDC, &rcImage, NULL);
        }
    }

#else
    {
        Gdiplus::Graphics g(m_hDC);
        g.DrawImage(pImage, 0,0, iWidth, iHeight);
    }
#endif
    CHR(hr, _T("pImage->Draw failed"));
    // use for testing
    /*dc = ::GetDC(hWnd);
    BitBlt(dc, 0, 0, iWidth, iHeight, m_hDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, dc);*/
    

    // if there are alpha values then we want to extract them
    if(sPixelFormat & PixelFormatAlpha)
    {
        hr = gdiWhite.Create(m_hDC, rc, FALSE, TRUE, FALSE, 24);
        CHR(hr, _T("gdiWhite.Create failed"));

        FillRect(gdiWhite.GetDC(), rc, RGB(255,255,255));    // draw a white background
#ifdef UNDER_CE
        /*hr =*/ pImage->Draw(gdiWhite.GetDC(), &rc, NULL);
#else
        {
            Gdiplus::Graphics g(gdiWhite.GetDC());
            g.DrawImage(pImage, 0,0, iWidth, iHeight);
        }
#endif
        CHR(hr, _T("pImage->Draw failed"));
        // use for testing
        /*dc = ::GetDC(hWnd);
        BitBlt(dc, 0, 0, sImageInfo.Width, sImageInfo.Height, gdiWhite.GetDC(), 0, 0, SRCCOPY);
        ReleaseDC(hWnd, dc);*/
        
        hr = ExtractAlphaValues(gdiWhite);
        CHR(hr, _T("ExtractAlphaValues failed"));
    }

    // if we're 16 bit we have to dumb it down for speed
    if(GetSystemBitsPerPixel() == 16)
    {
        hr = ConvertToBPP(16);
        CHR(hr, _T("ConvertToBPP"));
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::ExtractAlphaValues(CIssGDIEx& gdiWhite)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC, _T("HDC is NULL"));
    CBARG(m_pBuffer, _T("m_pBuffer is NULL"));
    CBARG(gdiWhite.GetDC(), _T("HDC is NULL for White"));
    CBARG(gdiWhite.GetBits(), _T("GetBits is NULL for white"));
    CBARG(m_sBitmap.bmWidth == gdiWhite.GetWidth(), _T("White and Black width are not the same"));
    CBARG(m_sBitmap.bmHeight == gdiWhite.GetHeight(), _T("White and Black height are not the same"));
    CBARG(gdiWhite.GetBPP() == GetBPP(), _T("White and Black BPP are not the same"));

    // create our alpha array
    hr = InitAlpha();
    CHR(hr, _T("InitAlpha failed"));
    
    // now go through and subtract both the white and black background images to get the alpha
    RGBTRIPLE*    lprgbWhite;
    RGBTRIPLE*    lprgbBlack;
    WORD        wR,wG,wB;
    WORD        wRD, wGD, wBD;
    int         iYOffset = 0;

    DWORD       dwWidthBytesWhite    = (DWORD)gdiWhite.GetBitmapInfo().bmWidthBytes;
    DWORD       dwWidthBytesBlack    = (DWORD)m_sBitmap.bmWidthBytes;

    lprgbWhite    = (RGBTRIPLE*)gdiWhite.GetBits();
    lprgbBlack    = (RGBTRIPLE*)m_pBuffer;

    for(int y=0; y<m_sBitmap.bmHeight; y++)
    {
        for(int x=0; x<m_sBitmap.bmWidth; x++)
        {
            //wColorB        = lprgbBlack[x];
            //wColorW        = lprgbWhite[x];
            // quick check to see if we have full opacity
            // if(wColorB == wColorW)
            if(lprgbBlack[x].rgbtRed == lprgbWhite[x].rgbtRed && 
                lprgbBlack[x].rgbtGreen == lprgbWhite[x].rgbtGreen &&
                lprgbBlack[x].rgbtBlue == lprgbWhite[x].rgbtBlue)
            {
                m_pAlpha[iYOffset + x]  = ALPHA_Full;   // full opacity
                continue;
            }

            wRD = (WORD)lprgbWhite[x].rgbtRed;
            wGD = (WORD)lprgbWhite[x].rgbtGreen;
            wBD = (WORD)lprgbWhite[x].rgbtBlue;

            wR = (WORD)lprgbBlack[x].rgbtRed;
            wG = (WORD)lprgbBlack[x].rgbtGreen;
            wB = (WORD)lprgbBlack[x].rgbtBlue;

            //find the difference
            wRD = wRD - wR;
            wGD = wGD - wG;
            wBD = wBD - wB;

            wRD = 0xff - wRD;
            wGD = 0xff - wGD;
            wBD = 0xff - wBD;

            if(wRD != 0)
                wR	= 0xff*wR/wRD;
            if(wGD != 0)
                wG	= 0xff*wG/wGD;
            if(wBD != 0)
                wB	= 0xff*wB/wBD;

            m_pAlpha[iYOffset + x]  = (BYTE)wGD;
            lprgbBlack[x].rgbtRed = (BYTE)wR;
            lprgbBlack[x].rgbtGreen = (BYTE)wG;
            lprgbBlack[x].rgbtBlue = (BYTE)wB;
        }
        iYOffset    += m_sBitmap.bmWidth;
        lprgbWhite	= (RGBTRIPLE *)((LPBYTE)lprgbWhite + dwWidthBytesWhite);
        lprgbBlack  = (RGBTRIPLE *)((LPBYTE)lprgbBlack+ dwWidthBytesBlack);
    }
Error:

    return hr;
}

HBITMAP CIssGDIEx::Create16BitDIB(int iWidth, int iHeight)
{
    LPBITMAPINFO lpbi;

    if(!m_hDC)
        return NULL;

    lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) + (3 * sizeof(RGBQUAD))];  
    if(!lpbi)
        return NULL;

    ZeroMemory(lpbi,sizeof(BITMAPINFOHEADER) + (3 * sizeof(RGBQUAD)));
    lpbi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);  
    lpbi->bmiHeader.biWidth			= iWidth; 
    lpbi->bmiHeader.biHeight		= iHeight;  
    lpbi->bmiHeader.biPlanes		= 1;  
    lpbi->bmiHeader.biBitCount		= 16; 
    lpbi->bmiHeader.biCompression	= BI_BITFIELDS; // see help on  BITMAPINFOHEADER  
    lpbi->bmiHeader.biSizeImage		= 0; 
    lpbi->bmiHeader.biXPelsPerMeter = 0;  
    lpbi->bmiHeader.biYPelsPerMeter = 0;  
    lpbi->bmiHeader.biClrUsed		= 3; // specifies 3 entries in color table  
    lpbi->bmiHeader.biClrImportant	= 3;  

    unsigned long *ptr = (unsigned long *)(lpbi->bmiColors);  
    ptr[0] = 0xf800;  
    ptr[1] = 0x07e0;  
    ptr[2] = 0x001F;  

    HBITMAP hbm			= CreateDIBSection(	m_hDC,
                                            lpbi,
                                            DIB_RGB_COLORS,
                                            (void**)&m_pBuffer,
                                            NULL,
                                            0);
    SAFE_DELETE_ARRAY(lpbi);
    return hbm;
}

HBITMAP CIssGDIEx::Create24BitDIB(int iWidth, int iHeight)
{
    if(!m_hDC)
        return NULL;

    DIBINFOEX  dibInfo;
    dibInfo.bmiHeader.biBitCount		= 24;
    dibInfo.bmiHeader.biClrImportant	= 0;
    dibInfo.bmiHeader.biClrUsed			= 0;
    dibInfo.bmiHeader.biCompression		= 0;
    dibInfo.bmiHeader.biHeight			= iHeight;
    dibInfo.bmiHeader.biPlanes			= 1;
    dibInfo.bmiHeader.biSize			= 40;
    dibInfo.bmiHeader.biSizeImage		= iWidth*iHeight*3;
    dibInfo.bmiHeader.biWidth			= iWidth;
    dibInfo.bmiHeader.biXPelsPerMeter	= 3780;
    dibInfo.bmiHeader.biYPelsPerMeter	= 3780;
    dibInfo.bmiColors[0].rgbBlue		= 0;
    dibInfo.bmiColors[0].rgbGreen		= 0;
    dibInfo.bmiColors[0].rgbRed			= 0;
    dibInfo.bmiColors[0].rgbReserved	= 0;

    HBITMAP hbm			= CreateDIBSection(	m_hDC,
                                            (const BITMAPINFO*)dibInfo,
                                            DIB_RGB_COLORS,
                                            (void**)&m_pBuffer,
                                            NULL,
                                            0);

    return hbm;
}

HBITMAP CIssGDIEx::Create32BitDIB(int iWidth, int iHeight)
{
    if(!m_hDC)
        return NULL;

    DIBINFOEX  dibInfo;
    dibInfo.bmiHeader.biBitCount		= 32;
    dibInfo.bmiHeader.biClrImportant	= 0;
    dibInfo.bmiHeader.biClrUsed			= 0;
    dibInfo.bmiHeader.biCompression		= BI_RGB;
    dibInfo.bmiHeader.biHeight			= iHeight;
    dibInfo.bmiHeader.biPlanes			= 1;
    dibInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
    dibInfo.bmiHeader.biSizeImage		= iWidth*iHeight*sizeof(DWORD);
    dibInfo.bmiHeader.biWidth			= iWidth;
    //dibInfo.bmiHeader.biXPelsPerMeter	= 3780;
    //dibInfo.bmiHeader.biYPelsPerMeter	= 3780;
    dibInfo.bmiColors[0].rgbBlue		= 0;
    dibInfo.bmiColors[0].rgbGreen		= 0;
    dibInfo.bmiColors[0].rgbRed			= 0;
    dibInfo.bmiColors[0].rgbReserved	= 0;

    HBITMAP hbm			= CreateDIBSection(	m_hDC,
                                            (const BITMAPINFO*)dibInfo,
                                            DIB_RGB_COLORS,
                                            (void**)&m_pBuffer,
                                            NULL,
                                            0);

    return hbm;
}

HBITMAP CIssGDIEx::CreateUnknownDIB(int iWidth, int iHeight)
{
    if(!m_hDC)
        return NULL;

    int iBpp = GetSystemBitsPerPixel();
    switch(iBpp)
    {
    case 24:
    case 32:
        return Create24BitDIB(iWidth, iHeight);
        break;
    case 16:
        return Create16BitDIB(iWidth, iHeight);
        break;
    }   

    return NULL;
}

int CIssGDIEx::GetSystemBitsPerPixel()
{
    static int iBpp = -1;
    if(iBpp != -1)
        return iBpp;

    DEVMODE sDevMode;
    sDevMode.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &sDevMode))
    {
        iBpp = 16;  // set it to some default so we don't have to check again
        return iBpp;
    }

    iBpp = sDevMode.dmBitsPerPel;
    return iBpp;
}

BYTE CIssGDIEx::GetAlphaValue(int iXpos, int iYpos)
{
    // there is no alpha values
    if(m_pAlpha == NULL)
    {
        return (m_sBitmap.bmBitsPixel==16?ALPHA_Full:255);
    }

    int iWidth = m_sBitmap.bmWidth;
    int iHeight= m_sBitmap.bmHeight;

    if(iXpos >= iWidth || iYpos >= iHeight || iXpos < 0 || iYpos < 0)
        return (m_sBitmap.bmBitsPixel==16?ALPHA_Full:255);

    LPBYTE lpDest   = ((LPBYTE)m_pAlpha + iWidth*(iHeight - iYpos - 1));
    return lpDest[iXpos];
}

HRESULT CIssGDIEx::SetAlphaValue(int iXpos, int iYpos, BYTE btAlpha)
{
    HRESULT hr = S_OK;
    CBARG(m_pAlpha, _T(""));

    int iWidth = m_sBitmap.bmWidth;
    int iHeight= m_sBitmap.bmHeight;

    CBARG(iXpos>=0 && iYpos>=0 && iXpos<iWidth && iYpos<iHeight, _T(""));

    LPBYTE lpDest = ((LPBYTE)m_pAlpha + iWidth*(iHeight - iYpos - 1));
    lpDest[iXpos] = btAlpha;

Error:
    return hr;
}

//Get the pixel color at location iXPos and iYPos from our current GDI
//Works for both 16 bit and 24 bit buffers - DIB & DDB
COLORREF CIssGDIEx::GetPixelColor(int iXpos, int iYpos)
{
    if(!m_hDC) return 0;

    // if we're outside of our image
    if(iXpos >= GetWidth() || iYpos >= GetHeight())
        return TRANSPARENT_COLOR;

    if(iXpos < 0 || iYpos < 0)
        return TRANSPARENT_COLOR;

    // if we're in DDB we don't have access to the bits to we do it the slow way
    if(!GetBits())
        return GetPixel(m_hDC,iXpos, iYpos);

    if(m_sBitmap.bmBitsPixel == 24)
    {
        RGBTRIPLE*    lprgbSrc;

        // Initialize the surface pointers.
        lprgbSrc  = (RGBTRIPLE *)((LPBYTE)m_pBuffer  + m_sBitmap.bmWidthBytes*(m_sBitmap.bmHeight - iYpos - 1));
        return RGB(lprgbSrc[iXpos].rgbtRed,lprgbSrc[iXpos].rgbtGreen,lprgbSrc[iXpos].rgbtBlue);
    }
    else
    {
        WORD*    lprgbSrc;

        // Initialize the surface pointers.
        lprgbSrc  = (WORD *)((LPBYTE)m_pBuffer  + m_sBitmap.bmWidthBytes*(m_sBitmap.bmHeight - iYpos - 1));
        WORD wColor = lprgbSrc[iXpos];
        WORD wR, wG, wB;
        wR = (WORD)((wColor >> 11) & 0x1f);
        wG = (WORD)((wColor >> 5) & 0x3f);
        wB = (WORD)(wColor & 0x1f);
        wR <<= 3;
        wG <<= 2;
        wB <<= 3;
        return RGB(wR,wG,wB);
    }
}

//Set the pixel color at location iXPos and iYPos in our current GDI
//Works for both 16 bit and 24 bit buffers - DIB & DDB
HRESULT CIssGDIEx::SetPixelColor(int iXpos, int iYpos, COLORREF crColor)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC, _T("no m_hDC"));

    if(iXpos < 0 || iYpos < 0)
        return FALSE;

    // if we're outside of our image
    if(iXpos >= GetWidth() || iYpos >= GetHeight())
        return FALSE;

    // if we're in DDB we don't have access to the bits to we do it the slow way
    if(!GetBits())
        return SetPixel(m_hDC,iXpos, iYpos, crColor);

    if(m_sBitmap.bmBitsPixel == 24)
    {
        RGBTRIPLE*    lprgbSrc;

        // Initialize the surface pointers.
        lprgbSrc  = (RGBTRIPLE *)((LPBYTE)m_pBuffer  + m_sBitmap.bmWidthBytes*(m_sBitmap.bmHeight - iYpos - 1));
        lprgbSrc[iXpos].rgbtRed        = GetRValue(crColor);
        lprgbSrc[iXpos].rgbtGreen    = GetGValue(crColor);
        lprgbSrc[iXpos].rgbtBlue    = GetBValue(crColor);
    }
    else
    {
        WORD*    lprgbSrc;

        // Initialize the surface pointers.
        lprgbSrc  = (WORD *)((LPBYTE)m_pBuffer  + m_sBitmap.bmWidthBytes*(m_sBitmap.bmHeight - iYpos - 1));
        WORD wColor  = (WORD)((GetRValue(crColor)>>3)<<11) | ((GetGValue(crColor)>>2)<<5) | (GetBValue(crColor)>>3);
        lprgbSrc[iXpos] = wColor;
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::GetAlphaMask(CIssGDIEx& gdiMask)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC&&m_pAlpha, _T("m_hDC&&m_pAlpha"));

    RECT rc;
    SetRect(&rc, 0, 0, m_sBitmap.bmWidth, m_sBitmap.bmHeight);

    hr = gdiMask.Create(m_hDC, rc, FALSE, TRUE);
    FillRect(gdiMask, rc, RGB(0,0,0));
    CHR(hr, _T("gdiMask.Create"));

    int iYOffset = 0;
    WORD*    lprgbMask = (WORD*)gdiMask.GetBits();
    DWORD dwWidthBytes = gdiMask.GetBitmapInfo().bmWidthBytes;

    WORD wR     = 0;
    WORD wG     = 0;
    WORD wB     = 0;

    for(int y=0; y<gdiMask.GetHeight(); y++)
    {
        for(int x=0; x<gdiMask.GetWidth(); x++)
        {
            wG = (WORD)m_pAlpha[x+iYOffset]*63/255;
            lprgbMask[x]    = (WORD)(wR<<11|wG<<5|wB);
        }
        lprgbMask = (WORD *)((LPBYTE)lprgbMask + dwWidthBytes);
        iYOffset    += gdiMask.GetWidth();
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::SetAlphaMask(CIssGDIEx& gdiMask, BOOL bTakeMin)
{
    HRESULT hr = S_OK;

    CBARG(m_hDC&&gdiMask.GetBits(), _T("m_hDC&&gdiMask.GetBits()"));
    CBARG(m_sBitmap.bmWidth==gdiMask.GetWidth(), _T("Mask width not the same"));
    CBARG(m_sBitmap.bmHeight==gdiMask.GetHeight(), _T("Mask height not the same"));

    if(bTakeMin == FALSE || !m_pAlpha)
    {
        hr = InitAlpha();
        CHR(hr, _T("InitAlpha failed"));
    }

    int iYOffset = 0;
    //DWORD dwWidthBytes = gdiMask.GetBitmapInfo().bmWidthBytes;
    COLORREF cr;

    for(int y=0; y<gdiMask.GetHeight(); y++)
    {
        for(int x=0; x<gdiMask.GetWidth(); x++)
        {
            //m_pAlpha[x+iYOffset] = (BYTE)(lprgbMask[x] & 0x1f);
            cr = gdiMask.GetPixelColor(x, gdiMask.GetHeight() - y - 1);
			if(m_pAlpha[x+iYOffset] == 255 || bTakeMin == FALSE)
				m_pAlpha[x+iYOffset] = GetGValue(cr);
        }
        iYOffset    += gdiMask.GetWidth();
    }

Error:
    return hr;
}

/********************************************************************************************

Create a Font

iHeight		- Height of the text
iWeight		- FW_NORMAL (default), or FW_BOLD
bUseClearType - If you want to use ClearType (default to FALSE)
szFont		- Font name (default to Tahoma)
iRotation	- Should the text be rotated (0 is default), 9 will rotate 90 degrees

********************************************************************************************/
HFONT CIssGDIEx::CreateFont(int iHeight, int iWeight, BOOL bUseClearType, TCHAR* szFont, int iRotation, int iWidth, BOOL bItalics)
{
    if(iHeight < 0 || iWeight < 0 || szFont == NULL)
    {
        return NULL;
    }

    LOGFONT lf;

    // fill the structure to the desired attributes
    lf.lfHeight			= iHeight;
    lf.lfWidth			= iWidth;
    lf.lfEscapement		= 10*iRotation;
    lf.lfOrientation	= 10*iRotation;
    lf.lfWeight			= iWeight;
    lf.lfItalic			= bItalics;
    lf.lfUnderline		= 0;
    lf.lfStrikeOut		= 0;
    lf.lfCharSet		= DEFAULT_CHARSET;
    lf.lfOutPrecision	= 0;
    lf.lfClipPrecision	= 0;
    lf.lfQuality		= (bUseClearType?5:0);
    lf.lfPitchAndFamily = 0;
    _tcscpy(lf.lfFaceName, szFont);

    //create the font for use
    return CreateFontIndirect(&lf);
}

BOOL CIssGDIEx::DeleteFont(HFONT& hFont)
{
    if(hFont == NULL)
        return TRUE;

    DeleteObject(hFont);
    hFont = NULL;

    return TRUE;
}

HPEN CIssGDIEx::CreatePen(COLORREF crColor, int iPenStyle /* = PS_SOLID */, int iWidth /* = 0 */)
{
    if(iWidth < 0)
    {
        return NULL;
    }

    return ::CreatePen(iPenStyle, iWidth, crColor);
}

// create a brush with the appropriate color
HBRUSH CIssGDIEx::CreateBrush(COLORREF crColor)
{
    return CreateSolidBrush(crColor);
}

BOOL CIssGDIEx::SetPen(HPEN hPen)
{
    if(!m_hDC) return FALSE;

    // reset to our original pen upon creation
    if(m_hOldPen)
    {
        SelectObject(m_hDC, m_hOldPen);
        m_hOldPen = NULL;
    }

    // destroy a pen if it's there
    if(m_hPen)
    {
        DeleteObject(m_hPen);
        m_hPen = NULL;
    }

    // set it to the DC
    if(hPen)
    {
        m_hOldPen = (HPEN)SelectObject(m_hDC, hPen);
        return TRUE;
    }

    return FALSE;
}

HRESULT CIssGDIEx::Rotate(BOOL bLeft /* = FALSE */)
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetHeight(), GetWidth(), FALSE, TRUE, m_sBitmap.bmBitsPixel);
    CHR(hr, _T("Rotate - could not create gdi"));
    
    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("Rotate - could not create Alpha"));
    }

    //original 
    int iWidth = GetWidth();
    int iHeight = GetHeight();

    if(bLeft)//counter clockwise
    {
        for(int x = 0; x < GetWidth(); x++)
        {
            for(int y = 0; y < GetHeight(); y++)
            {
                gdiTemp.SetPixelColor(iHeight - y - 1, x, GetPixelColor(x, y));
                if(m_pAlpha)
                    gdiTemp.SetAlphaValue(iHeight - y- 1, x, GetAlphaValue(x, y));
            }
        }
    }

    else//clockwise
    {
        for(int x = 0; x < GetWidth(); x++)
        {
            for(int y = 0; y < GetHeight(); y++)
            {
                gdiTemp.SetPixelColor(y, iWidth - x - 1, GetPixelColor(x, y));
                if(m_pAlpha)
                    gdiTemp.SetAlphaValue(y, iWidth - x- 1, GetAlphaValue(x, y));
            }
        }
    }


    Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;



}

HRESULT CIssGDIEx::FlipHorizontal()
{
    HRESULT hr = S_OK;
    CBARG(m_hDC&&m_pBuffer&&GetWidth()&&GetHeight(), _T(""));

    switch(m_sBitmap.bmBitsPixel)
    {
    case 16:
        hr = FlipHorizontal16();
        break;
    default:
        FlipHorizontal24();
        break;
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::FlipHorizontal16()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 16);
    CHR(hr, _T("FlipHorizontal16 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("FlipHorizontal16 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha + GetWidth()*(GetHeight()-1);//bottom
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    WORD*        lprgbSrc  = (WORD*)((LPBYTE)m_pBuffer + dwWidthBytesDest*(GetHeight()-1));
    WORD*        lprgbDest = (WORD*)((LPBYTE)gdiTemp.GetBits());

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image
            lprgbDest[x] = lprgbSrc[x];

            // copy the alpha
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[x];
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc - dwWidthBytesDest);

        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc - GetWidth();
        }
    }

    Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}



HRESULT CIssGDIEx::FlipHorizontal24()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 24);
    CHR(hr, _T("FlipHorizontal24 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("FlipHorizontal24 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha + GetWidth()*(GetHeight()-1);//right cause alpha is always 1 byte per
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    RGBTRIPLE*         lprgbSrc  = (RGBTRIPLE* )((LPBYTE)m_pBuffer + dwWidthBytesDest*(GetHeight()-1));
    RGBTRIPLE*         lprgbDest = (RGBTRIPLE* )((LPBYTE)gdiTemp.GetBits());

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but from right to left
            lprgbDest[x].rgbtRed = lprgbSrc[x].rgbtRed;
            lprgbDest[x].rgbtGreen = lprgbSrc[x].rgbtGreen;
            lprgbDest[x].rgbtBlue = lprgbSrc[x].rgbtBlue;

            // copy the alpha from right to left
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[GetWidth() - x - 1];
        }
        lprgbDest = (RGBTRIPLE* )((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (RGBTRIPLE* )((LPBYTE)lprgbSrc - dwWidthBytesDest);
        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc - GetWidth();
        }
    }
    Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}

HRESULT CIssGDIEx::ScaleImage(int iCX, int iCY)
{
	HRESULT hr = S_OK;
	CIssGDIEx gdiTemp;

	CBARG(m_hDC, _T("CIssGDIEx::ScaleImage"));

	if(GetWidth() == iCX && GetHeight() == iCY)
		goto Error;	// all good

	SIZE sz;
	sz.cx		= iCX;
	sz.cy		= iCY;

	hr = ::ScaleImage(*this, gdiTemp, sz, TRUE, TRANSPARENT_COLOR);
	CHR(hr, _T("hr = ::ScaleImage(*this, gdiTemp, sz, TRUE, TRANSPARENT_COLOR);"));

	hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha, m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);
	CHR(hr, _T("hr = gdiTemp.MoveGdiMemory"));

Error:
	return hr;
}

HRESULT CIssGDIEx::FlipVertical()
{
    HRESULT hr = S_OK;
    CBARG(m_hDC&&m_pBuffer&&GetWidth()&&GetHeight(), _T(""));

    switch(m_sBitmap.bmBitsPixel)
    {
    case 16:
        hr = FlipVertical16();
        break;
    default:
        hr = FlipVertical24();
        break;
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::FlipVertical16()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 16);
    CHR(hr, _T("FlipVertical16 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("FlipVertical16 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha;
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    WORD*        lprgbSrc  = (WORD*)((LPBYTE)m_pBuffer);
    WORD*        lprgbDest = (WORD*)((LPBYTE)gdiTemp.GetBits());

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but from right to left
            lprgbDest[x] = lprgbSrc[GetWidth() - x - 1];

            // copy the alpha from right to left
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[GetWidth() - x - 1];
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytesDest);
        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc + GetWidth();
        }
    }
	Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}



HRESULT CIssGDIEx::FlipVertical24()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 24);
    CHR(hr, _T("FlipVertical24 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("FlipVertical24 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha;
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    RGBTRIPLE*         lprgbSrc  = (RGBTRIPLE* )((LPBYTE)m_pBuffer);
    RGBTRIPLE*         lprgbDest = (RGBTRIPLE* )((LPBYTE)gdiTemp.GetBits());

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but from right to left
            lprgbDest[x].rgbtRed = lprgbSrc[GetWidth() -x - 1].rgbtRed;
            lprgbDest[x].rgbtGreen = lprgbSrc[GetWidth() -x - 1].rgbtGreen;
            lprgbDest[x].rgbtBlue = lprgbSrc[GetWidth() -x - 1].rgbtBlue;

            // copy the alpha from right to left
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[GetWidth() - x - 1];
        }
        lprgbDest = (RGBTRIPLE* )((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (RGBTRIPLE* )((LPBYTE)lprgbSrc + dwWidthBytesDest);
        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc + GetWidth();
        }
    }
	Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}

HRESULT CIssGDIEx::GrayScale()
{
    HRESULT hr = S_OK;
    CBARG(m_hDC&&m_pBuffer&&GetWidth()&&GetHeight(), _T(""));

    switch(m_sBitmap.bmBitsPixel)
    {
    case 16:
        hr = GrayScale16();
        break;
    default:
        hr = GrayScale24();
        break;
    }

Error:
    return hr;
}

HRESULT CIssGDIEx::GrayScale16()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 16);
    CHR(hr, _T("GrayScale16 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("GrayScale16 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha;
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    WORD*        lprgbSrc  = (WORD*)((LPBYTE)m_pBuffer);
    WORD*        lprgbDest = (WORD*)((LPBYTE)gdiTemp.GetBits());
    WORD wColor;
    WORD wR, wG, wB;

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but from right to left
            wColor = lprgbSrc[x];
            wR = (WORD)((wColor >> 11) & 0x1f);
            wG = (WORD)((wColor >> 5) & 0x3f);
            wB = (WORD)(wColor & 0x1f);
            wR <<= 3;
            wG <<= 2;
            wB <<= 3;
            wR  = RGB2GRAY(wR, wG, wB);
            lprgbDest[x] = (WORD)((wR>>3)<<11) | ((wR>>2)<<5) | (wR>>3);

            // copy the alpha
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[x];
        }
        lprgbDest = (WORD *)((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (WORD *)((LPBYTE)lprgbSrc + dwWidthBytesDest);
        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc + GetWidth();
        }
    }
    Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}

HRESULT CIssGDIEx::SaveToFile(TCHAR* szFileName)
{
    HRESULT hr = S_OK;
    HBITMAP hBmpFile = NULL;
    HDC hBmpFileDC = NULL;

    CBARG(szFileName != NULL, _T("save image failed"));
    CBARG(m_hDC != NULL, _T("save image failed"));

    BITMAPINFO bi;
    void *pBits     = NULL;
    int iWidth      = GetWidth();
    int iHeight     = GetHeight();

    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize         = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth        = iWidth;
    bi.bmiHeader.biHeight       = iHeight;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = GetBPP();
    bi.bmiHeader.biCompression  = BI_RGB;
    bi.bmiHeader.biSizeImage    = GetBPP()/8*iWidth*iHeight;

    hBmpFileDC                  = CreateCompatibleDC(GetDC());
    hBmpFile                    = CreateDIBSection(GetDC(), &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    SelectObject(hBmpFileDC, hBmpFile);
    BitBlt(hBmpFileDC, 0, 0, iWidth, iHeight, GetDC(), 0, 0, SRCCOPY);

    HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwRet = 0;
        BITMAPFILEHEADER bmfHeader;
        ZeroMemory(&bmfHeader, sizeof(BITMAPFILEHEADER));
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmfHeader.bfSize = bi.bmiHeader.biSizeImage + bmfHeader.bfOffBits;
        bmfHeader.bfType = 'MB';//BUGBUG ... maybe

        WriteFile(hFile, &bmfHeader, sizeof(bmfHeader), &dwRet, NULL);
        WriteFile(hFile, &bi.bmiHeader, sizeof(bi.bmiHeader), &dwRet, NULL);
        WriteFile(hFile, pBits, bi.bmiHeader.biSizeImage, &dwRet, NULL);
        CloseHandle(hFile);
    }

    CloseClipboard();


Error:
    DeleteDC(hBmpFileDC);
    DeleteObject(hBmpFile);

    return hr;
}


HRESULT CIssGDIEx::GrayScale24()
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    hr = gdiTemp.Create(m_hDC, GetWidth(), GetHeight(), FALSE, TRUE, 24);
    CHR(hr, _T("GrayScale24 - could not create gdi"));

    if(m_pAlpha)
    {
        hr = gdiTemp.InitAlpha(TRUE);
        CHR(hr, _T("GrayScale24 - could not create Alpha"));
    }

    DWORD        dwWidthBytesDest    = (DWORD)m_sBitmap.bmWidthBytes;

    LPBYTE       lpAlphaSrc = NULL;
    LPBYTE       lpAlphaDest= NULL;
    if(m_pAlpha)
    {
        lpAlphaSrc= m_pAlpha;
        lpAlphaDest= gdiTemp.GetAlpha();
    }
    RGBTRIPLE*         lprgbSrc  = (RGBTRIPLE* )((LPBYTE)m_pBuffer);
    RGBTRIPLE*         lprgbDest = (RGBTRIPLE* )((LPBYTE)gdiTemp.GetBits());

    for(int y=0; y<GetHeight(); y++)
    {
        for(int x=0; x<GetWidth(); x++)
        {
            // copy the image but from right to left
            lprgbDest[x].rgbtRed = RGB2GRAY(lprgbSrc[x].rgbtRed,lprgbSrc[x].rgbtGreen,lprgbSrc[x].rgbtBlue);
            lprgbDest[x].rgbtGreen = lprgbDest[x].rgbtRed;
            lprgbDest[x].rgbtBlue = lprgbDest[x].rgbtRed;

            // copy the alpha
            if(lpAlphaSrc && lpAlphaDest)
                lpAlphaDest[x] = lpAlphaSrc[x];
        }
        lprgbDest = (RGBTRIPLE* )((LPBYTE)lprgbDest + dwWidthBytesDest);
        lprgbSrc = (RGBTRIPLE* )((LPBYTE)lprgbSrc + dwWidthBytesDest);
        if(lpAlphaSrc && lpAlphaDest)
        {
            lpAlphaDest = lpAlphaDest + GetWidth();
            lpAlphaSrc  = lpAlphaSrc + GetWidth();
        }
    }
    Destroy();
    hr = gdiTemp.MoveGdiMemory(m_hDC, m_sBitmap, m_hbmBitmap, m_hbmOld, m_pBuffer, m_pAlpha,
        m_hBrush, m_hOldBrush, m_hPen, m_hOldPen, m_hFont, m_hOldFont);

Error:
    return hr;
}