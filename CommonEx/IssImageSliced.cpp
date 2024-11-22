/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssImageSliced.h 
// Abstract: Given an image, cut it up to make it fully scalable
// 
/***************************************************************************************************************/ 

#include "IssImageSliced.h"
#include "IssDebug.h"
#include "IssGDIDraw.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIssImageSliced::CIssImageSliced()
:m_bInitialized(FALSE)
{
}

CIssImageSliced::~CIssImageSliced()
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
void CIssImageSliced::Destroy()
{
    m_gdiImage.Destroy();
    m_gdiTL.Destroy();
    m_gdiT.Destroy();
    m_gdiTR.Destroy();
    m_gdiR.Destroy();
    m_gdiBR.Destroy();
    m_gdiB.Destroy();
    m_gdiBL.Destroy();
    m_gdiL.Destroy();
    m_gdiM.Destroy();
    m_bInitialized = FALSE;
    return;
}

HRESULT CIssImageSliced::Initialize(HWND hWnd, HINSTANCE hInst, UINT uiImage, BOOL bSelfdefined)
{
    HRESULT hr = S_OK;
    Destroy();

    CIssGDIEx gdiImage;

    hr = gdiImage.LoadImage(uiImage, hWnd, hInst, TRUE);
    CHR(hr, _T("gdiImage.LoadImage"));

    hr = CutImage(gdiImage, bSelfdefined);
    CHR(hr, _T("gdiImage.LoadImage"));
    m_bInitialized = TRUE;
Error:
    return hr;
}

HRESULT CIssImageSliced::Initialize(HWND hWnd, TCHAR* szFileName, BOOL bSelfdefined)
{
    HRESULT hr = S_OK;
    Destroy();

    CIssGDIEx gdiImage;

    hr = gdiImage.LoadImage(szFileName, hWnd, TRUE);
    CHR(hr, _T("gdiImage.LoadImage"));

    hr = CutImage(gdiImage, bSelfdefined);
    CHR(hr, _T("gdiImage.LoadImage"));
    m_bInitialized = TRUE;

Error:
    return hr;
}

HRESULT CIssImageSliced::Initialize(CIssGDIEx& gdiSrc, BOOL bSelfdefined /* = FALSE */)
{
    HRESULT hr = S_OK;
    Destroy();

    hr = CutImage(gdiSrc, bSelfdefined);
    CHR(hr, _T("gdiImage.LoadImage"));
    m_bInitialized = TRUE;

Error:
    return hr;
}

HRESULT CIssImageSliced::CutImage(CIssGDIEx& gdiImage, BOOL bSelfdefined)
{
    HRESULT hr = S_OK;
    RECT rc;

    int h1,h2,v1,v2;
    int iWidth      = gdiImage.GetWidth();
    int iHeight     = gdiImage.GetHeight();
    int iCutWidth   = (iWidth-1)/2;
    int iCutHeight  = (iHeight-1)/2;

    if(bSelfdefined)
    {
        hr = CalcImageSizes(gdiImage,h1,h2,v1,v2);
        CHR(hr, _T("CalcImageSizes"));
    }

    // top left
    if(bSelfdefined)
        SetRect(&rc, 1, 1, h1, v1);
    else
        SetRect(&rc, 0, 0, iCutWidth, iCutHeight);
    hr = m_gdiTL.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiTL.Create"));
    Draw(m_gdiTL, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // top
    if(bSelfdefined)
        SetRect(&rc, h1+1, 1, iWidth-1-h2, 1+v1);
    else
        SetRect(&rc, iCutWidth, 0, iCutWidth+1, iCutHeight);
    hr = m_gdiT.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiT.Create"));
    Draw(m_gdiT, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // top right
    if(bSelfdefined)
        SetRect(&rc, iWidth-1-h2, 1, iWidth-1,1+v1);
    else
        SetRect(&rc, iWidth-iCutWidth, 0, iWidth, iCutHeight);
    hr = m_gdiTR.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiTR.Create"));
    Draw(m_gdiTR, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // right
    if(bSelfdefined)
        SetRect(&rc, iWidth-1-h2, 1+v1, iWidth-1, iHeight-1-v2);
    else
        SetRect(&rc, iWidth - iCutWidth, iCutHeight, iWidth, iCutHeight+1);
    hr = m_gdiR.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiR.Create"));
    Draw(m_gdiR, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // bottom right
    if(bSelfdefined)
        SetRect(&rc, iWidth-1-h2, iHeight-1-v2, iWidth-1, iHeight-1);
    else
        SetRect(&rc, iWidth - iCutWidth, iHeight - iCutHeight, iWidth, iHeight);
    hr = m_gdiBR.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiBR.Create"));
    Draw(m_gdiBR, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // bottom
    if(bSelfdefined)
        SetRect(&rc, 1+h1, iHeight-1-v2, iWidth-1-h2, iHeight-1);
    else
        SetRect(&rc, iCutWidth, iHeight - iCutHeight, iCutWidth+1, iHeight);
    hr = m_gdiB.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiB.Create"));
    Draw(m_gdiB, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // bottom left
    if(bSelfdefined)
        SetRect(&rc, 1, iHeight-1-v2, 1+h1, iHeight-1);
    else
        SetRect(&rc, 0, iHeight - iCutHeight, iCutWidth, iHeight);
    hr = m_gdiBL.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiBL.Create"));
    Draw(m_gdiBL, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // left
    if(bSelfdefined)
        SetRect(&rc, 1, 1+v1, 1+h1, iHeight-1-v2);
    else
        SetRect(&rc, 0, iCutHeight, iCutWidth, iCutHeight+1);
    hr = m_gdiL.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiL.Create"));
    Draw(m_gdiL, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

    // middle
    if(bSelfdefined)
        SetRect(&rc, 1+h1, 1+v1, iWidth-1-h2, iHeight-1-v2);
    else
        SetRect(&rc, iCutWidth, iCutHeight, iCutWidth+1, iCutHeight+1);
    hr = m_gdiM.Create(gdiImage.GetDC(), rc, FALSE, TRUE);
    CHR(hr, _T("m_gdiM.Create"));
    Draw(m_gdiM, 0,0, WIDTH(rc), HEIGHT(rc), gdiImage, rc.left, rc.top, ALPHA_Copy);

Error:
    return hr;
}

HRESULT CIssImageSliced::GenerateImage(int iWidth, int iHeight)
{
    HRESULT hr = S_OK;
    RECT rcZero, rcTile;

    CBARG(m_gdiM.GetDC(), _T("slices not set yet"));

    hr = m_gdiImage.Create(m_gdiM.GetDC(), iWidth, iHeight, FALSE, TRUE);
    CHR(hr, _T("m_gdiImage.Create"));
    if(m_gdiM.GetAlpha())
    {
        hr = m_gdiImage.InitAlpha(TRUE);
        CHR(hr, _T("m_gdiImage.InitAlpha"));
    }

    SetRect(&rcZero, 0, 0, iWidth, iHeight);

    // draw the middle stuff first
    rcTile = rcZero;
    if(m_gdiM.GetWidth() == 1 && m_gdiM.GetHeight() == 1)
    {
        COLORREF crColor = m_gdiM.GetPixelColor(0,0);
        FillRect(m_gdiImage.GetDC(), rcTile, crColor);
        if(m_gdiM.GetAlpha())
            FillRectAlpha(m_gdiImage, rcTile, m_gdiM.GetAlphaValue(0,0));
    }
    else
        TileBackGround(m_gdiImage,    rcTile, m_gdiM,    FALSE);

 
    // draw the top part
    rcTile            = rcZero;
    rcTile.bottom    = m_gdiT.GetHeight();
    TileBackGround(m_gdiImage,    rcTile, m_gdiT,    FALSE);

    // draw the bottom part
    rcTile            = rcZero;
    rcTile.top        = rcZero.bottom - m_gdiB.GetHeight();
    for(int i=0; i<WIDTH(rcZero)+m_gdiB.GetWidth(); i+= m_gdiB.GetWidth())
    {
        rcTile.left        = i;
        rcTile.right    = rcTile.left + m_gdiB.GetWidth();
        if(rcTile.left >= WIDTH(rcZero))
            break;
        if(rcTile.right > rcZero.right)
            rcTile.right = rcZero.right;

        BitBlt(m_gdiImage.GetDC(), 
            rcTile.left, rcTile.top,
            WIDTH(rcTile), m_gdiB.GetHeight(),
            m_gdiB.GetDC(),
            0,0,
            SRCCOPY);
        if(m_gdiB.GetAlpha())
        {
            CopyAlpha(m_gdiImage,
                rcTile.left, rcTile.top,
                WIDTH(rcTile), m_gdiB.GetHeight(),
                m_gdiB,
                0,0);
        }
    }

    // draw the Left part
    rcTile            = rcZero;
    rcTile.right    = m_gdiL.GetWidth();
    TileBackGround(m_gdiImage,    rcTile, m_gdiL,    FALSE);

    // draw the right part
    rcTile            = rcZero;
    rcTile.left        = rcZero.right - m_gdiR.GetWidth();
    for(int i=0; i<HEIGHT(rcZero)+m_gdiR.GetHeight(); i+= m_gdiR.GetHeight())
    {
        rcTile.top        = i;
        rcTile.bottom    = rcTile.top + m_gdiR.GetHeight();
        if(rcTile.top >= HEIGHT(rcZero))
            break;
        if(rcTile.bottom > rcZero.bottom)
            rcTile.bottom = rcZero.bottom;

        BitBlt(m_gdiImage.GetDC(), 
            rcTile.left, rcTile.top,
            m_gdiR.GetWidth(), HEIGHT(rcTile),
            m_gdiR.GetDC(),
            0,0,
            SRCCOPY);
        if(m_gdiR.GetAlpha())
        {
            CopyAlpha(m_gdiImage,
                rcTile.left, rcTile.top,
                m_gdiR.GetWidth(), HEIGHT(rcTile),
                m_gdiR,
                0,0);
        }
    }
    //CIssGDI::TileBackGround(m_m_gdiImage.GetDC(),    rcTile, gdiR,    TRUE);

    // draw top left piece
    BitBlt(m_gdiImage.GetDC(),
        0,0,
        m_gdiTL.GetWidth(), m_gdiTL.GetHeight(),
        m_gdiTL.GetDC(),
        0, 0,
        SRCCOPY);
    if(m_gdiTL.GetAlpha())
    {
        CopyAlpha(m_gdiImage,
            0,0,
            m_gdiTL.GetWidth(), m_gdiTL.GetHeight(),
            m_gdiTL,
            0, 0);
    }

    // draw top Right piece
    BitBlt(m_gdiImage.GetDC(),
        rcZero.right - m_gdiTR.GetWidth(),0,
        m_gdiTR.GetWidth(), m_gdiTR.GetHeight(),
        m_gdiTR.GetDC(),
        0, 0,
        SRCCOPY);
    if(m_gdiTR.GetAlpha())
    {
        CopyAlpha(m_gdiImage,
            rcZero.right - m_gdiTR.GetWidth(),0,
            m_gdiTR.GetWidth(), m_gdiTR.GetHeight(),
            m_gdiTR,
            0, 0);
    }

    // draw Bottom Right piece
   BitBlt(m_gdiImage.GetDC(),
        rcZero.right - m_gdiBR.GetWidth(),rcZero.bottom - m_gdiBR.GetHeight(),
        m_gdiBR.GetWidth(), m_gdiBR.GetHeight(),
        m_gdiBR.GetDC(),
        0, 0,
        SRCCOPY);
    if(m_gdiBR.GetAlpha())
    {
        CopyAlpha(m_gdiImage,
            rcZero.right - m_gdiBR.GetWidth(),rcZero.bottom - m_gdiBR.GetHeight(),
            m_gdiBR.GetWidth(), m_gdiBR.GetHeight(),
            m_gdiBR,
            0, 0);
    }

    // draw Bottom Left piece
    BitBlt(m_gdiImage.GetDC(),
        0,rcZero.bottom - m_gdiBL.GetHeight(),
        m_gdiBL.GetWidth(), m_gdiBL.GetHeight(),
        m_gdiBL.GetDC(),
        0, 0,
        SRCCOPY);
    if(m_gdiBL.GetAlpha())
    {
        CopyAlpha(m_gdiImage,
            0,rcZero.bottom - m_gdiBL.GetHeight(),
            m_gdiBL.GetWidth(), m_gdiBL.GetHeight(),
            m_gdiBL,
            0, 0);
    }

Error:
    return hr;
}

HRESULT CIssImageSliced::SetSize(int iWidth, int iHeight)
{
    HRESULT hr = S_OK;

    // check if we have the proper size already
    if(m_gdiImage.GetWidth() == iWidth && m_gdiImage.GetHeight() == iHeight)
        goto Error;

    m_gdiImage.Destroy();

    hr = GenerateImage(iWidth, iHeight);
    CHR(hr, _T("GenerateImage"));

Error:
    return hr;
}
HRESULT CIssImageSliced::DrawImage(CIssGDIEx& gdiDest, int iX, int iY, EnumAlphaTransfer eTransfer)
{
    return Draw(gdiDest, iX, iY, m_gdiImage.GetWidth(), m_gdiImage.GetHeight(), m_gdiImage, 0, 0, eTransfer);
}

HRESULT CIssImageSliced::CopyImage(CIssGDIEx& gdiDest, int iX, int iY)
{
    HRESULT hr = S_OK;

    CBARG(gdiDest.GetDC()&&m_gdiImage.GetDC(), _T("gdiDest.GetDC()&&m_gdiImage.GetDC("));
    BitBlt(gdiDest.GetDC(),
        iX,iY,
        m_gdiImage.GetWidth(), m_gdiImage.GetHeight(),
        m_gdiImage.GetDC(),
        0, 0,
        SRCCOPY);
    if(m_gdiImage.GetAlpha())
    {
        CopyAlpha(gdiDest,
            iX,iY,
            m_gdiImage.GetWidth(), m_gdiImage.GetHeight(),
            m_gdiImage,
            0, 0,
            SRCCOPY);
    }

Error:
    return hr;
}

HRESULT CIssImageSliced::CalcImageSizes(CIssGDIEx& gdiImage, int& h1, int& h2, int& v1, int& v2)
{
    HRESULT hr = S_OK;

    int x,y;
    int iWidth  = gdiImage.GetWidth();
    int iHeight = gdiImage.GetHeight();
    CBARG(iWidth!=0&&iHeight!=0, _T(""));

    h1 = h2 = v1 = v2 = 0;

    for(x=1; x<iWidth; x++)
    {
        if(gdiImage.GetPixelColor(x, 0) == 0)
            break;
        h1++;
    }
    for(y=1; y<iHeight; y++)
    {
        if(gdiImage.GetPixelColor(0, y) == 0)
            break;
        v1++;
    }
    CBARG(h1!=0&&v1!=0&&h1!=iWidth&&h1!=iHeight, _T(""));

    for(x=iWidth-1; x>0; x--)
    {
        if(gdiImage.GetPixelColor(x, 0) == 0)
            break;
        h2++;
    }
    for(y=iHeight-1; y>0; y--)
    {
        if(gdiImage.GetPixelColor(0, y) == 0)
            break;
        v2++;
    }
    CBARG(h2!=0&&v2!=0&&h2!=iWidth&&h2!=iHeight, _T(""));

Error:
    return hr;
}

