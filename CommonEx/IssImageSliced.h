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

#pragma once

#include "IssGDIEx.h"
#include "IssGDIDraw.h"

class CIssImageSliced  
{
public:
    CIssImageSliced();
    virtual ~CIssImageSliced();

    void    Destroy();
    HRESULT Initialize(HWND hWnd, HINSTANCE hInst, UINT uiImage, BOOL bSelfdefined = FALSE);
    HRESULT Initialize(HWND hWnd, TCHAR* szFileName, BOOL bSelfdefined = FALSE);
    HRESULT Initialize(CIssGDIEx& gdiSrc, BOOL bSelfdefined = FALSE);
    HRESULT SetSize(int iWidth, int iHeight);
    HRESULT DrawImage(CIssGDIEx& gdiDest, int iX, int iY, EnumAlphaTransfer eTransfer = ALPHA_Normal);
    HRESULT CopyImage(CIssGDIEx& gdiDest, int iX, int iY);
    BOOL    IsLoaded(){return (m_gdiImage.GetDC()?TRUE:FALSE);};
    BOOL    IsInitialized(){return m_bInitialized;};
    int     GetWidth(){return m_gdiImage.GetWidth();};
    int     GetHeight(){return m_gdiImage.GetHeight();};
    HRESULT CutImage(CIssGDIEx& gdiImage, BOOL bSelfdefined = FALSE);
    CIssGDIEx& GetImage(){return m_gdiImage;};
	int		GetMinHeight(){return (m_gdiTL.GetHeight() + m_gdiL.GetHeight() + m_gdiBL.GetHeight());};
	int		GetMinWidth(){return (m_gdiTL.GetWidth() + m_gdiT.GetWidth() + m_gdiTR.GetWidth());};
    int     GetBottomHeight(){return m_gdiBL.GetHeight();};
    
private:    // Functions
    HRESULT GenerateImage(int iWidth, int iHeight);
    HRESULT CalcImageSizes(CIssGDIEx& gdiImage, int& h1, int& h2, int& v1, int& v2);

private:    // variables
    CIssGDIEx   m_gdiImage; // full image that we draw with
    CIssGDIEx   m_gdiTL;    // Top left slice
    CIssGDIEx   m_gdiT;     // Top slice
    CIssGDIEx   m_gdiTR;    // Top right slice
    CIssGDIEx   m_gdiR;     // Right slice
    CIssGDIEx   m_gdiBR;    // Bottom Right slice
    CIssGDIEx   m_gdiB;     // Bottom slice
    CIssGDIEx   m_gdiBL;    // Bottom Left slice
    CIssGDIEx   m_gdiL;     // Left slice
    CIssGDIEx   m_gdiM;     // Middle slice (not sure if it's needed yet)

    BOOL        m_bInitialized;
};

