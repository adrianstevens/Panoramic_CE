#include "path.h"
#include "StdAfx.h"
#include "GuiFavorites.h"
#include "IssRegistry.h"
#include "IssRect.h"
#include "IssCommon.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "ContactsGuiDefines.h"
#include "IssGDIDraw.h"
#include "GuiToolbar.h"
#include "IssWndTouchMenu.h"
#include "ContactsUtil.h"
#include "issdebug.h"
#include "DlgOptions.h"
#include "DlgGetFile.h"
#include "IssLocalisation.h"

#include "Shlobj.h"
#include <Cpl.h>

#ifndef CPL_IDNAME
#define CPL_IDNAME    100
#endif

typedef LONG (*CPLAPPLET) (HWND hwndCPl, UINT  msg, LPARAM  lParam1, LPARAM  lParam2);

#define UND_LOCATION                POINT(0,0)


extern void DebugContacts(TCHAR* szFunction, BOOL bStart);


// struct for saving item to registry
struct TypeItemContact
{
	TCHAR       szName[STRING_MAX];
    TCHAR       szPath[STRING_MAX];
	POINT		ptLocation;
    EnumLaunchType eType;
    int         iIconIndex;
};

CGuiFavorites::CGuiFavorites(void)
: m_iHoldingIndex(UND)
, m_bLockItems(FALSE)
, m_bContextMenu(FALSE)
, m_iProgramsMatrix(NULL)
, m_iGamesMatrix(NULL)
, m_bMouseDown(FALSE)
, m_bSliderDrag(FALSE)
, m_ePage(0)
, m_eBtnPressState(PS_noPress)
{
	m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*11/32, FW_NORMAL, TRUE);
	m_hLabelFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
	m_hLabelFontRot = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE, _T("Tahoma"), 270);

    SetSelection(UND, UND);
}

CGuiFavorites::~CGuiFavorites(void)
{
    CDlgChoosePrograms::ResetListContent();
    CDlgChooseSettings::ResetListContent();
	SaveRegistry();
	Destroy();
	DeleteMatrix(m_iProgramsMatrix, MAX_Array_Height);
	m_iProgramsMatrix = NULL;
	DeleteMatrix(m_iGamesMatrix, MAX_Array_Height);
	m_iGamesMatrix = NULL;
	CIssGDIEx::DeleteFont(m_hFont);
	CIssGDIEx::DeleteFont(m_hLabelFont);
	CIssGDIEx::DeleteFont(m_hLabelFontRot);
}

BOOL CGuiFavorites::Destroy()
{
    if(m_iProgramsMatrix)
    {
	    for(int i=0; i<MAX_Array_Height; i++)
		    for(int j=0; j<MAX_Array_Width; j++)
			    m_iProgramsMatrix[i][j]	= UND;
    }

    if(m_iGamesMatrix)
    {
	    for(int i=0; i<MAX_Array_Height; i++)
		    for(int j=0; j<MAX_Array_Width; j++)
			    m_iGamesMatrix[i][j]	= UND;
    }

	DestroyLinkArray(m_arrPrograms);
	DestroyLinkArray(m_arrGames);
    DestroyTempImages();

    m_gdiImgArrowArray.Destroy();
    m_gdiAddNew.Destroy();
    m_imgGrid.Destroy();
    m_gdiPlaceholder.Destroy();
    m_imgSelector.Destroy();
    m_imgAlphaMask.Destroy();
    m_imgItemBorder.Destroy();
    m_imgItemDropShadow.Destroy();
    m_imgSlider.Destroy();       
    m_imgSliderLan.Destroy();    
    m_imgSliderTrack.Destroy(); 
    m_imgSliderTrackLan.Destroy();
    m_imgNameBubble.Destroy();     

	return TRUE;
}

void CGuiFavorites::DestroyTempImages()
{
	m_gdiMovingItem.Destroy();
    m_gdiMovingItemMask.Destroy();
    m_gdiNameBubbleMask.Destroy();
    m_gdiSliderBg.Destroy();
}

void CGuiFavorites::DestroyLinkArray(CIssVector<TypeLaunchItem>& arrLinks)
{
	for(int i=0; i< arrLinks.GetSize(); i++)
	{
		TypeLaunchItem* sLaunch = arrLinks[i];
		if(sLaunch)
			delete sLaunch;
	}
	arrLinks.RemoveAll();
}

BOOL CGuiFavorites::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    PreloadImages(hWndParent, m_hInst);
    
    //m_oStr->StringCopy(m_szSliderText[0], IDS_SLIDER_Programs,  STRING_NORMAL, m_hInst);
    //m_oStr->StringCopy(m_szSliderText[1], IDS_SLIDER_Games,  STRING_NORMAL, m_hInst);
    ResetSliderText();

    return TRUE;
}

void CGuiFavorites::ResetSliderText()
{
    TCHAR szText[STRING_NORMAL] = _T("");
    CDlgOptions::GetSliderName(m_sOptions->eSlideLeft, szText);
    m_oStr->StringCopy(m_szSliderText[0], szText);
    CDlgOptions::GetSliderName(m_sOptions->eSlideRight, szText);
    m_oStr->StringCopy(m_szSliderText[1], szText);
    m_gdiSliderBg.Destroy();
}

BOOL CGuiFavorites::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{ 
        SetAllDirty();
	}

	return UNHANDLED;
}

void CGuiFavorites::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    m_imgSelector.Destroy();
    m_imgSelector.Initialize(hWnd, hInstance, SKIN(IDR_PNG_FavoritesImgSelect));
}

HRESULT CGuiFavorites::PreloadImages(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

    // sliced images
    UINT uiRes;
    uiRes = IsVGA() ? IDR_PNG_FavoritesGridVGA : IDR_PNG_FavoritesGrid;
    hr = m_imgGrid.Initialize(hWnd, hInstance, uiRes);
    CHR(hr, _T("m_imgGrid.Initialize()"));

    uiRes = IsVGA() ? IDR_PNG_FavoritesImgAlphaVGA : IDR_PNG_FavoritesImgAlpha;
    hr = m_imgAlphaMask.Initialize(hWnd, hInstance, IDR_PNG_FavoritesImgAlpha);
    CHR(hr, _T("m_imgAlphaMask.Initialize()"));

    if(!m_imgSelector.IsLoaded())
        m_imgSelector.Initialize(m_hWndParent, m_hInst, SKIN(IDR_PNG_FavoritesImgSelect));

    uiRes = IsVGA() ? IDR_PNG_FavoritesImgBorderVGA : IDR_PNG_FavoritesImgBorder;
    if(!m_imgItemBorder.IsLoaded())
        m_imgItemBorder.Initialize(m_hWndParent, m_hInst, uiRes);

    uiRes = IsVGA() ? IDR_PNG_FavoritesNameBblVGA : IDR_PNG_FavoritesNameBbl;
    hr = m_imgNameBubble.Initialize(hWnd, hInstance, uiRes);
    CHR(hr, _T("m_imgNameBubble.Initialize()"));

    
    // load once images
    if(!m_gdiAddNew.GetDC())
    {
        uiRes = IsVGA() ? IDR_PNG_FavoritesAddVGA : IDR_PNG_FavoritesAdd;
        hr = m_gdiAddNew.LoadImage(uiRes, m_hWndParent, m_hInst);
        CHR(hr, _T("m_gdiAddNew.LoadImage()"));
    }


    if(!m_gdiImgArrowArray.GetDC())
    {
        UINT uiRes = IsVGA() ?  IDR_PNG_FavoritesArrowArrayVGA : IDR_PNG_FavoritesArrowArray;
        hr = m_gdiImgArrowArray.LoadImage(uiRes, m_hWndParent, m_hInst);
        CHR(hr, _T("m_gdiImgArrowArray.LoadImage()"));
    }


Error:
    return hr;
}

BOOL CGuiFavorites::AddMenuItems()
{
    int iCount = GetCurLinkArray().GetSize();
    TypeLaunchItem* sLaunch = NULL;

    if(HasSelection())
    {
        int iIndex = GetSelected();
        sLaunch = GetCurLinkArray()[iIndex];
    }

	if(iCount&&sLaunch)
    {
        m_wndMenu->AddItem(ID(IDS_MENU_RemoveItem), m_hInst, IDMENU_RemoveItem, NULL);
    }
    else
    {
        m_wndMenu->AddItem(ID(IDS_MENU_AddProgram),  m_hInst,   IDMENU_AddProgram);
        m_wndMenu->AddItem(ID(IDS_MENU_AddSetting),  m_hInst,   IDMENU_AddSetting);
        m_wndMenu->AddItem(ID(IDS_MENU_AddFileFolder), m_hInst, IDMENU_AddFileFolder);
    }
    
	m_wndMenu->AddSeparator();
    m_wndMenu->AddItem(ID(IDS_MENU_LockItems),  m_hInst,  IDMENU_LockItems, m_bLockItems?FLAG_Check:0);

	return TRUE;
}
   
CIssGDIEx& CGuiFavorites::GetSliderBg()
{
    if(!m_gdiSliderBg.GetDC())
    {
        m_gdiSliderBg.Create(m_gdiMem->GetDC(), m_rcSlider);
        DrawSliderBg(m_gdiSliderBg);

        // text
        RECT rcText = {0,0,m_gdiSliderBg.GetWidth(), m_gdiSliderBg.GetHeight()};
        if(IsLandscape())
        {
            // the positioning of angled text seems unpredictable at this time...
            InflateRect(&rcText, 0, -(HEIGHT(rcText)/16));
        //    rcText.right += IsVGA() ? 80 : 40;

            // first we have to select the new font into the DC if it's needed
            SIZE sSize;

            HFONT hOldFont = (m_hLabelFontRot?(HFONT)SelectObject(m_gdiSliderBg.GetDC(), m_hLabelFontRot):NULL);
            
            GetTextExtentPoint(m_gdiSliderBg.GetDC(), m_szSliderText[0], (int)_tcslen(m_szSliderText[0]), &sSize);
            //no this really is the right math ....
            rcText.right += sSize.cx;
            rcText.right -= GetSliderHeight()/2;
            rcText.right += sSize.cy/2;

            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0);
            OffsetRect(&rcText, 1,-1);
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, COLOR_FAVORITES_BG_TEXT);

            //and put it back
            rcText.right -= sSize.cx;

            if(IsVGA())
                rcText.right += 1;
            OffsetRect(&rcText, -1,1);

            GetTextExtentPoint(m_gdiSliderBg.GetDC(), m_szSliderText[1], (int)_tcslen(m_szSliderText[1]), &sSize);
            rcText.right += sSize.cx;
            rcText.top = rcText.bottom - sSize.cx;
            
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0);
            OffsetRect(&rcText, 1,-1);
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, COLOR_FAVORITES_BG_TEXT);

            if(hOldFont)
                SelectObject(m_gdiSliderBg.GetDC(), hOldFont);
        }
        else
        {
            InflateRect(&rcText, -(WIDTH(rcText)/16), 0);
            OffsetRect(&rcText, 1, 1);
            ::DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_LEFT | DT_VCENTER, m_hLabelFont, 0);
            ::DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_VCENTER, m_hLabelFont, 0);
            OffsetRect(&rcText, -1, -1);
            ::DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_LEFT | DT_VCENTER, m_hLabelFont, COLOR_FAVORITES_BG_TEXT);
            ::DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_VCENTER, m_hLabelFont, COLOR_FAVORITES_BG_TEXT);
        }
    }
    return m_gdiSliderBg;
}
    
int CGuiFavorites::GetSliderHeight()
{
    return IsVGA() ?  FAVORITES_SLIDER_HEIGHT * 2 : FAVORITES_SLIDER_HEIGHT;
}

int CGuiFavorites::GetSliderKnobHeight()
{
    return IsVGA() ?  FAVORITES_SLIDER_KNOB_HEIGHT * 2 : FAVORITES_SLIDER_KNOB_HEIGHT;
}

int CGuiFavorites::GetSliderTrackHeight()
{
    return IsVGA() ?  FAVORITES_SLIDER_TRACK_HEIGHT * 2 : FAVORITES_SLIDER_TRACK_HEIGHT;
}

BOOL CGuiFavorites::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

    DBG_CONTACTS(_T("CGuiFavorites::MoveGui()"), TRUE);

	CGuiBase::MoveGui(rcLocation);

    int iSize = (int)m_sOptions->eFavSize; //just a little cleaner

    DestroyTempImages();

    if(IsLandscape())
    {
        SetRect(&m_rcSlider, rcLocation.right - GetSliderHeight(), 0, rcLocation.right, HEIGHT(rcLocation));
        m_rcGrid = rcLocation;
        m_rcGrid.right = m_rcSlider.left;
    }
    else
    {
        SetRect(&m_rcSlider, 0, 0, WIDTH(rcLocation), GetSliderHeight());
        m_rcGrid = rcLocation;
        m_rcGrid.top = m_rcSlider.bottom;
    }

    // set the block size and matrix orientation
	int iMinLength		= min(WIDTH(m_rcGrid), HEIGHT(m_rcGrid));
	int iBlockSizeMin	= (iMinLength - (FAVORITES_GRID_BORDER*2)) / iSize;	

    // this is getting a bit sketchy because our grid is getting very square
    BOOL bMinDimensionIsWidth = WIDTH(m_rcGrid) < HEIGHT(m_rcGrid);

	if(IsLandscape())
	{
		m_sizeMatrix.cy	= iSize;
		m_sizeMatrix.cx	= (long)((float)(iSize*WIDTH(m_rcGrid))/(float)(HEIGHT(m_rcGrid)) + 0.5f);// let's round it
        if(bMinDimensionIsWidth)
        {
		    m_sizeBlock.cx	= iBlockSizeMin;
		    m_sizeBlock.cy	= (HEIGHT(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cy;
        }
        else
        {
		    m_sizeBlock.cy	= iBlockSizeMin;
		    m_sizeBlock.cx	= (WIDTH(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cx;
        }
	}
	else
	{
		m_sizeMatrix.cx	= iSize;
		m_sizeMatrix.cy	= (long)((float)(iSize*HEIGHT(m_rcGrid))/(float)WIDTH(m_rcGrid) + 0.5f);// let's round it
        if(bMinDimensionIsWidth)
        {
		    m_sizeBlock.cx	= iBlockSizeMin;
		    m_sizeBlock.cy	= (HEIGHT(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cy;
        }
        else
        {
		    //m_sizeBlock.cy	= iBlockSizeMin;    // asumes that m_sizeMatrix.cy == 4
		    m_sizeBlock.cy	= iBlockSizeMin * iSize / m_sizeMatrix.cy;    
		    m_sizeBlock.cx	= (WIDTH(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cx;
        }
	}

	// we'll initialize the matrices
	if(!m_iProgramsMatrix)
		m_iProgramsMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	if(!m_iGamesMatrix)
		m_iGamesMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	
    SetMatrixOrientation(WIDTH(m_rcGrid) > HEIGHT(m_rcGrid));
    
    ReadRegistry();

    AutoArrangeItems(m_iGamesMatrix, m_arrGames);
    AutoArrangeItems(m_iProgramsMatrix, m_arrPrograms);

    // re-create all the images if they are defined
    /*for(int i = 0; i < m_arrPrograms.GetSize(); i++)
    {
	    TypeLaunchItem* sLink = m_arrPrograms[i];
	    if(sLink)
        {
        //    sLink->UpdateImage();
            RECT rcTemp = {sLink->ptLocation.x, sLink->ptLocation.y, sLink->ptLocation.x + m_sizeBlock.cx, sLink->ptLocation.y + m_sizeBlock.cy};
            sLink->gdiImage.Destroy();
            InitImage(rcTemp, sLink);
        }
    }
    
    for(int i=0; i < m_arrGames.GetSize(); i++)
    {
        TypeLaunchItem* sLink = m_arrGames[i];
        if(sLink)
        {
            RECT rcTemp = {sLink->ptLocation.x, sLink->ptLocation.y, sLink->ptLocation.x + m_sizeBlock.cx, sLink->ptLocation.y + m_sizeBlock.cy};
            sLink->gdiImage.Destroy();
            InitImage(rcTemp, sLink);
        }
    }*/

    // pre-render images
    GetGridImg();
    GetSliderBg();
    GetSliderImg(IsLandscape());
    GetItemBorder();
    GetItemAlphaMask();
    GetSelector();

    CGuiBase::SetAllDirty();

    DBG_CONTACTS(_T("CGuiFavorites::MoveGui()"), FALSE);

	return TRUE;
}

void CGuiFavorites::SetMatrixOrientation(BOOL bLandscape /*= FALSE*/)
{
	if(m_sizeMatrix.cx == m_sizeMatrix.cy)					// square
		return;

	if(m_sizeMatrix.cx > m_sizeMatrix.cy && bLandscape)		// already landscape
		return;
	if(m_sizeMatrix.cx < m_sizeMatrix.cy && !bLandscape)	// already portrait
		return;

    ///////////////////////////////////////////////////////////////////
    //// Personal ////
	int** iNewMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	
	// do the transformation x=y
	for(int x = 0; x < MAX_Array_Width; x++)
	{
		for(int y = 0; y < MAX_Array_Height; y++)
		{
			iNewMatrix[x][y] = m_iProgramsMatrix[y][x];
		}
	} 

	// copy it back to the original matrix
	for(int y=0; y<MAX_Array_Height; y++)
	{
		for(int x=0; x<MAX_Array_Width; x++)
		{
			m_iProgramsMatrix[y][x] = iNewMatrix[y][x];

			if(m_iProgramsMatrix[y][x] != UND)
			{
				POINT ptLocation = {x,y};
				TypeLaunchItem* sItem = m_arrPrograms[m_iProgramsMatrix[y][x]];
				if(sItem)
				{
				    sItem->ptLocation = ptLocation;
				}
			}
		}
	}	

	DeleteMatrix(iNewMatrix, MAX_Array_Height);

    ///////////////////////////////////////////////////////////////////
    //// Business ////
	iNewMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	
	// do the transformation x=y
    for(int x = 0; x < MAX_Array_Width; x++)
	{
		for(int y = 0; y < MAX_Array_Height; y++)
		{
			iNewMatrix[x][y] = m_iGamesMatrix[y][x];
		}
	}

	// copy it back to the original matrix
	for(int y=0; y<MAX_Array_Height; y++)
	{
		for(int x=0; x<MAX_Array_Width; x++)
		{
			m_iGamesMatrix[y][x] = iNewMatrix[y][x];

			if(m_iGamesMatrix[y][x] != UND)
			{
				POINT ptLocation = {x,y};
				TypeLaunchItem* sItem = m_arrGames[m_iGamesMatrix[y][x]];
				if(sItem)
				{
				    sItem->ptLocation = ptLocation;
				}
			}
		}
	}	

	DeleteMatrix(iNewMatrix, MAX_Array_Height);
}

CIssVector<TypeLaunchItem> &CGuiFavorites::GetCurLinkArray()
{
    if(m_ePage == eProgramsPage)
        return m_arrPrograms;
    else
        return m_arrGames;
}
    
CIssVector<TypeLaunchItem> &CGuiFavorites::GetLinkArray(int iWhich)
{
    if(iWhich == eProgramsPage)
        return m_arrPrograms;
    else
        return m_arrGames;
}


#define REG_Save_LocationPrograms			_T("SOFTWARE\\Pano\\Launcher\\Favs\\Programs")
#define REG_Save_LocationGames  			_T("SOFTWARE\\Pano\\Launcher\\Favs\\Games")


void CGuiFavorites::ReadRegistry()
{
	HKEY hKey       = NULL;
	DWORD dwType	= 0;
	DWORD dwIndex	= 0;
	DWORD dwBufSize = 0;
	DWORD dwStringSize = 0;
	TypeItemContact sItem;
	TCHAR szItemName[STRING_MAX];

    DBG_OUT((_T("CGuiFavorites::ReadRegistry")));
    /////////////////////////////////////////////////////////////////////
    // Personal 
    // clear array
    DestroyLinkArray(m_arrPrograms);
    ClearMatrix(m_iProgramsMatrix);
	//  Make sure that the destination doesn't already exist.
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_Save_LocationPrograms,0,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS) 
	while(TRUE)
	{
		dwBufSize	= sizeof(TypeItemContact);
		dwStringSize= STRING_MAX;
		ZeroMemory(&sItem, sizeof(TypeItemContact));

		if (RegEnumValue(hKey, dwIndex++,
						szItemName, &dwStringSize, 
						NULL, &dwType, 
						(LPBYTE)&sItem,
						&dwBufSize) != ERROR_SUCCESS)
			break;

        // has to be the exact size we want
        if(dwBufSize != sizeof(TypeItemContact))
            continue;
        //DBG_OUT((_T("RegEnumValue dwIndex:%d, szItemName:%s, dwBufSize:%d, RealBufSize:%d"), dwIndex, szItemName, dwBufSize, sizeof(TypeItemContact)));

        // don't allow more then one item in the same location
        int iTemp = m_iProgramsMatrix[sItem.ptLocation.y][sItem.ptLocation.x];

        if(iTemp > 0)
        {
            //DBG_OUT((_T("Program item %s overlapping at %d,%d"), szItemName, sItem.ptLocation.x, sItem.ptLocation.y));
            sItem.ptLocation.x = UND;
            sItem.ptLocation.y = UND;
        }

		TypeLaunchItem* sNewItem = new TypeLaunchItem;

        sNewItem->ptLocation = sItem.ptLocation;
        sNewItem->iIconIndex = sItem.iIconIndex;
        sNewItem->eType = sItem.eType;
        sNewItem->szFriendlyName = m_oStr->CreateAndCopy(sItem.szName);
        sNewItem->szPath = m_oStr->CreateAndCopy(sItem.szPath);

        //DBG_OUT((_T("Adding new Program Item:")));
        //DBG_OUT((_T("xy: %d,%d i;conindex: %d type: %d"), sNewItem->ptLocation.x, sNewItem->ptLocation.y, sNewItem->iIconIndex, sNewItem->eType));
        //DBG_OUT((_T("Friendly: %s"), sNewItem->szFriendlyName));
        //DBG_OUT((_T("Path: %s"), sNewItem->szPath));
    
        RECT rc = {sItem.ptLocation.x, sItem.ptLocation.y, sItem.ptLocation.x + m_sizeBlock.cx, sItem.ptLocation.y + m_sizeBlock.cy};
        InitImage(rc, sNewItem);
		m_arrPrograms.AddElement(sNewItem);
		FillMatrix(sNewItem, m_arrPrograms.GetSize()-1, TRUE);
	}
    if(hKey)
	    RegCloseKey(hKey);

    dwType	= 0;
	dwIndex	= 0;
	dwBufSize = 0;
	dwStringSize = 0;

    /////////////////////////////////////////////////////////////////////
    // Business 
    // clear array
    DestroyLinkArray(m_arrGames);
    ClearMatrix(m_iGamesMatrix);

    hKey = NULL;
	//  Make sure that the destination doesn't already exist.
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_Save_LocationGames,0,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS) 
	while(TRUE)
	{
		dwBufSize	= sizeof(TypeItemContact);
		dwStringSize= STRING_MAX;
		ZeroMemory(&sItem, sizeof(TypeItemContact));

		if (RegEnumValue(hKey, dwIndex++,
						szItemName, &dwStringSize, 
						NULL, &dwType, 
						(LPBYTE)&sItem,
						&dwBufSize) != ERROR_SUCCESS)
			break;

        // has to be the exact size we want
        if(dwBufSize != sizeof(TypeItemContact))
            continue;

        // don't allow more then one item in the same location
        if(m_iGamesMatrix[sItem.ptLocation.y][sItem.ptLocation.x] != UND)
        {
            //DBG_OUT((_T("Game item %s overlapping at %d,%d"), szItemName, sItem.ptLocation.x, sItem.ptLocation.y));
            continue;
        }

		TypeLaunchItem* sNewItem = new TypeLaunchItem;

        sNewItem->ptLocation = sItem.ptLocation;
        sNewItem->iIconIndex = sItem.iIconIndex;
        sNewItem->eType = sItem.eType;
        sNewItem->szFriendlyName = m_oStr->CreateAndCopy(sItem.szName);
        sNewItem->szPath = m_oStr->CreateAndCopy(sItem.szPath);

        //DBG_OUT((_T("Adding new Game Item:")));
        //DBG_OUT((_T("xy: %d,%d iconindex: %d type: %d"), sNewItem->ptLocation.x, sNewItem->ptLocation.y, sNewItem->iIconIndex, sNewItem->eType));
        //DBG_OUT((_T("Friendly: %s"), sNewItem->szFriendlyName));
        //DBG_OUT((_T("Path: %s"), sNewItem->szPath));

        RECT rc = {sItem.ptLocation.x, sItem.ptLocation.y, sItem.ptLocation.x + m_sizeBlock.cx, sItem.ptLocation.y + m_sizeBlock.cy};
        InitImage(rc, sNewItem);
        m_arrGames.AddElement(sNewItem);
        FillMatrix(sNewItem, m_arrGames.GetSize()-1, FALSE);
	}

    if(hKey)
	    RegCloseKey(hKey);

}
HRESULT CGuiFavorites::SaveRegistry(int iItem /*= UND*/)
{
    HRESULT hr;

    DBG_OUT((_T("CGuiFavorites::SaveRegistry: %d"), iItem));

    // save the page that we are on 
    TCHAR* szSaveLocation = (m_ePage == eProgramsPage) ? REG_Save_LocationPrograms : REG_Save_LocationGames;

	// delete it all first
    if(iItem == UND)
	    RegDeleteKeyNT(HKEY_CURRENT_USER, szSaveLocation);
    //CHR(hr, _T("RegDeleteKeyNT failed"));

	HKEY hKey;
	hr = RegCreateKeyEx(HKEY_CURRENT_USER, szSaveLocation, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    CHR(hr, _T("RegCreateKeyEx failed"));

	TCHAR szItem[STRING_MAX];
	TypeItemContact sItemContact;
	for(int i=0; i<GetCurLinkArray().GetSize(); i++)
	{
		// this is if we want to save just one item
		if(iItem != UND && iItem != i)
			continue;

        TypeLaunchItem* sItem = GetCurLinkArray()[i];
		if(sItem)
		{
            sItemContact.ptLocation = sItem->ptLocation;
            sItemContact.iIconIndex = sItem->iIconIndex;
            sItemContact.eType = sItem->eType;
            m_oStr->StringCopy(sItemContact.szName, sItem->szFriendlyName);
            m_oStr->StringCopy(sItemContact.szPath, sItem->szPath);

			m_oStr->Format(szItem, _T("c%X"), i);

            DBG_OUT((_T("Saving REG item: %s"), szItem));

			// save this item to the registry
			hr = RegSetValueEx(hKey, szItem, 0,REG_BINARY, (LPBYTE)&sItemContact, sizeof(TypeItemContact));
            CHR(hr, _T("RegSetValueEx failed"));
		}
	}

	RegFlushKey(hKey);
    RegCloseKey(hKey);
    
	// now set it back if we need to
	if(m_sizeMatrix.cx > m_sizeMatrix.cy)
		SetMatrixOrientation(TRUE);
Error:
    return hr;
}

BOOL CGuiFavorites::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsOverlapping(rcClient, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiFavorites::Draw()"), TRUE);
    if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }

    Render();

    RECT rcBlit = rcClip;
    rcBlit.bottom = min(rcBlit.bottom, m_rcLocation.bottom);
    rcBlit.left = max(rcBlit.left, m_rcLocation.left);
    BitBlt(hDC,
		rcBlit.left,rcBlit.top,
		WIDTH(rcBlit), HEIGHT(rcBlit),
		m_gdiBg->GetDC(),
		rcBlit.left,rcBlit.top,
		SRCCOPY);

    // draw the moving item over top
    if(m_eBtnPressState == PS_dragging)
        DrawMovingItem();

    DBG_CONTACTS(_T("CGuiFavorites::Draw()"), FALSE);
	return TRUE;
}
    
HRESULT CGuiFavorites::Render()
{
    // redraw whatever is dirty
    HRESULT hr = S_OK;
    if(IsRectEmpty(&m_rcDirty))
        return hr;

    DrawBackground(m_gdiBg->GetDC(), m_rcLocation, m_rcDirty);
	DrawGrid(*m_gdiBg, m_rcGrid, m_rcDirty);
	DrawItems(*m_gdiBg, m_rcGrid, m_rcDirty, m_iHoldingIndex);
    if(m_eBtnPressState != PS_dragging)
	    DrawSelected(*m_gdiBg, m_rcGrid, m_rcDirty);
    DrawSlider(*m_gdiBg, m_rcLocation, m_rcDirty);

    SetRectEmpty(&m_rcDirty);
    return hr;
}

void CGuiFavorites::SetDirty(POINT ptStart, POINT ptEnd)
{
    RECT rc1 = GetItemRect(ptStart);
    RECT rc2 = GetItemRect(ptEnd);

    OffsetRect(&rc1, m_rcLocation.left, m_rcLocation.top);
    OffsetRect(&rc2, m_rcLocation.left, m_rcLocation.top);

    if(!IsRectEmpty(&m_rcDirty))
        UnionRect(&rc1, &rc1, &m_rcDirty);
    UnionRect(&m_rcDirty, &rc1, &rc2);
}

int CGuiFavorites::GetShadowOffset()
{
    return IsVGA() ? FAVORITES_SHADOW_OFFSET * 2 : FAVORITES_SHADOW_OFFSET;
}

void CGuiFavorites::DrawMovingItem()
{
    // get rects
    int w, h;
    GetItemSize(m_rcGrid, w, h);
	RECT rc, rcBack;

    // current item
	rc.left		= m_ptHolding.x - m_sizeOffset.cx;
	rc.top		= m_ptHolding.y - m_sizeOffset.cy;
	rc.right	= rc.left + w;
	rc.bottom	= rc.top + h;

    // current shadow
    RECT rcShadow = rc;
    OffsetRect(&rcShadow, GetShadowOffset(), GetShadowOffset());

    // current back
    rcBack = GetItemRect(GetMatrixLocation(m_ptHolding));
    // GetItemRect() is referenced to m_rcLocation, so offset it
    OffsetRect(&rcBack, m_rcLocation.left, m_rcLocation.top);

	// 1) draw the rectangle of exactly where the Icon will be placed if the user let's go
    CIssGDIEx gdiTemp;
    gdiTemp.Create(m_gdiMem->GetDC(), rcBack);
    gdiTemp.SetAlphaMask(GetMovingItemMask());
    RECT rcIcon = {0,0,WIDTH(rcBack), HEIGHT(rcBack)};
    Rectangle(gdiTemp.GetDC(), rcIcon, COLOR_TEXT_SELECTED,COLOR_TEXT_SELECTED);
    ::Draw(*m_gdiMem, rcBack, gdiTemp, 0, 0, ALPHA_Normal, 100);

    // 2) draw the drop shadow
    GetItemDropShadow().DrawImage(*m_gdiMem, rcShadow.left, rcShadow.top);

    // 3) the moving item
    ::Draw(*m_gdiMem, rc, m_gdiMovingItem);

    // 4) draw selector over moving item
    GetSelector().DrawImage(*m_gdiMem, rc.left, rc.top);

    // 5) draw the name window
    ::Draw(*m_gdiMem, GetMovingNamePosition(rc), m_gdiNameBubble);
}

void CGuiFavorites::SlidePage()
{
    // let's just draw both pages and slide it
    // we also have to draw the selector overlap

    HDC dc = GetDC(m_hWndParent);

    CIssGDIEx gdiTemp;
    gdiTemp.Create(dc, WIDTH(m_rcGrid)*2, HEIGHT(m_rcGrid));

    RECT rcAll = {0,0,gdiTemp.GetWidth(), gdiTemp.GetHeight()};
    RECT rc1, rc2;
    rc1 = rc2 = rcAll;
    rc1.right = rc2.left = WIDTH(rcAll)/2;

    DrawBackground(gdiTemp.GetDC(), rc1, rc1);
    DrawBackground(gdiTemp.GetDC(), rc2, rc2);
    DrawGrid(gdiTemp, rc1, rc1);
    DrawGrid(gdiTemp, rc2, rc2);
	DrawItems(gdiTemp, rc1, rc1, UND, eGamesPage);
	DrawItems(gdiTemp, rc2, rc2, UND, eProgramsPage);
	DrawSelected(gdiTemp, rc1, rc1);
	DrawSelected(gdiTemp, rc2, rc2);


    DWORD dwStart = GetTickCount();
    BOOL bDec = (m_ePage == eGamesPage);

    int iNumFrames = WIDTH(rc1);

    float fAnimateTime = FAVORITES_PAGE_ANIMATE_TIME;
    int i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
    int iStartFrame = bDec ? rc1.right : 0;
    while(i < iNumFrames)
    {
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
        int iAmount = i;
        if(bDec)
            iAmount = -iAmount;

        // gdiTemp to offscreen
		BitBlt(m_gdiBg->GetDC(),
			m_rcGrid.left,m_rcGrid.top,
			WIDTH(m_rcGrid), HEIGHT(m_rcGrid),
			gdiTemp.GetDC(),
			iStartFrame + iAmount, 0,
			SRCCOPY);

        // selector
        CGuiToolBar::GetToolbar()->DrawSelector(*m_gdiBg, m_rcGrid, m_rcGrid, FALSE);

        // all to screen
		BitBlt(dc,
			m_rcGrid.left,m_rcGrid.top,
			WIDTH(m_rcGrid), HEIGHT(m_rcGrid),
			m_gdiBg->GetDC(),
			m_rcGrid.left,m_rcGrid.top,
			SRCCOPY);

    }
	ReleaseDC(m_hWndParent, dc);

}

CIssGDIEx& CGuiFavorites::GetMovingItemMask()
{
    if(!m_gdiMovingItemMask.GetDC())
    {
        int w,h;
        GetItemSize(m_rcGrid, w, h);
        CIssRect rcFull;
        rcFull.Set(0,0,w,h);
        m_gdiMovingItemMask.Create(m_gdiMovingItem.GetDC(), rcFull.Get(), FALSE, TRUE);
        GetItemAlphaMask().DrawImage(m_gdiMovingItemMask, 0, 0);
    }

    return m_gdiMovingItemMask;
}
  
CIssImageSliced& CGuiFavorites::GetItemAlphaMask()
{
    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesImgAlphaVGA : IDR_PNG_FavoritesImgAlpha;
    if(!m_imgAlphaMask.IsLoaded())
        m_imgAlphaMask.Initialize(m_hWndParent, m_hInst, uiRes);

    int w, h;
    GetItemSize(m_rcGrid, w, h);

    if(w != m_imgAlphaMask.GetWidth() || h != m_imgAlphaMask.GetHeight())
        m_imgAlphaMask.SetSize(w, h);

    return m_imgAlphaMask;
}
    
CIssGDIEx& CGuiFavorites::GetNameBubbleAlphaMask(RECT rcSize)
{
    // recreate the gdi in the given size...
    // and draw the mask to it
    m_gdiNameBubbleMask.Create(m_gdiMem->GetDC(), WIDTH(rcSize), HEIGHT(rcSize));

    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesImgAlphaVGA : IDR_PNG_FavoritesImgAlpha;
    if(!m_imgAlphaMask.IsLoaded())
    {
        m_imgAlphaMask.Initialize(m_hWndParent, m_hInst, uiRes);
        // make sure we have an alpha value
        //m_imgAlphaMask.GetImage().InitAlpha(FALSE);
    }

    if(WIDTH(rcSize) != m_imgAlphaMask.GetWidth() || HEIGHT(rcSize) != m_imgAlphaMask.GetHeight())
        m_imgAlphaMask.SetSize(WIDTH(rcSize), HEIGHT(rcSize));

    m_imgAlphaMask.DrawImage(m_gdiNameBubbleMask, 0, 0);

    return m_gdiNameBubbleMask;
}
    

CIssImageSliced& CGuiFavorites::GetSelector()
{
    if(!m_imgSelector.IsLoaded())
        m_imgSelector.Initialize(m_hWndParent, m_hInst, SKIN(IDR_PNG_FavoritesImgSelect));
    
    int w, h;
    GetItemSize(m_rcGrid, w, h);

    if(w != m_imgSelector.GetWidth() || h != m_imgSelector.GetHeight())
        m_imgSelector.SetSize(w, h);

    return m_imgSelector;
}

CIssImageSliced& CGuiFavorites::GetItemBorder()
{
    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesImgBorderVGA : IDR_PNG_FavoritesImgBorder;
    if(!m_imgItemBorder.IsLoaded())
        m_imgItemBorder.Initialize(m_hWndParent, m_hInst, uiRes);
    
    int w, h;
    GetItemSize(m_rcGrid, w, h);

    if(w != m_imgItemBorder.GetWidth() || h != m_imgItemBorder.GetHeight())
        m_imgItemBorder.SetSize(w, h);

    return m_imgItemBorder;
}

CIssImageSliced& CGuiFavorites::GetItemDropShadow()
{
    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesImgShadowVGA : IDR_PNG_FavoritesImgShadow;

    if(!m_imgItemDropShadow.IsLoaded())
        m_imgItemDropShadow.Initialize(m_hWndParent, m_hInst, uiRes);

    int w, h;
    GetItemSize(m_rcGrid, w, h);

    if(w != m_imgItemDropShadow.GetWidth() || h != m_imgItemDropShadow.GetHeight())
        m_imgItemDropShadow.SetSize(w, h);

    return m_imgItemDropShadow;
}
  
CIssImageSliced& CGuiFavorites::GetSliderImg(BOOL bLan /*= FALSE*/)
{
    int w, h;

    if(bLan)
    {
        //h = (HEIGHT(m_rcLocation) - 14) * 2 / 5;
        h = GetSystemMetrics(SM_CXICON)*3;
        w = GetSliderKnobHeight();

        UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderLanVGA : IDR_PNG_FavoritesSliderLan;
        if(!m_imgSliderLan.IsLoaded())
            m_imgSliderLan.Initialize(m_hWndParent, m_hInst, uiRes);

        if(w != m_imgSliderLan.GetWidth() || h != m_imgSliderLan.GetHeight())
            m_imgSliderLan.SetSize(w, h);

        return m_imgSliderLan;
    }
    else
    {
        h = GetSliderKnobHeight();
        w = (WIDTH(m_rcSlider) - 14) * 2 / 5;

        UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderVGA : IDR_PNG_FavoritesSlider;
        if(!m_imgSlider.IsLoaded())
            m_imgSlider.Initialize(m_hWndParent, m_hInst, uiRes);

        if(w != m_imgSlider.GetWidth() || h != m_imgSlider.GetHeight())
            m_imgSlider.SetSize(w, h);

        return m_imgSlider;
    }
}
  
CIssImageSliced& CGuiFavorites::GetGridImg()
{
    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesGridVGA : IDR_PNG_FavoritesGrid;
    if(!m_imgGrid.IsLoaded())
        m_imgGrid.Initialize(m_hWndParent, m_hInst, uiRes);

    int w, h;
    GetItemSize(m_rcGrid, w, h);

    if(w != m_imgGrid.GetWidth() || h != m_imgGrid.GetHeight())
        m_imgGrid.SetSize(w, h);

    return m_imgGrid;
}

CIssImageSliced& CGuiFavorites::GetSliderTrack()
{
    int w, h;
    if(IsLandscape())
    {
        h = HEIGHT(m_rcLocation) - (IsVGA() ? 16 : 8);
        w = GetSliderTrackHeight();

        UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderBgLanVGA : IDR_PNG_FavoritesSliderBgLan;
        if(!m_imgSliderTrackLan.IsLoaded())
            m_imgSliderTrackLan.Initialize(m_hWndParent, m_hInst, uiRes);

        if(w != m_imgSliderTrackLan.GetWidth() || h != m_imgSliderTrackLan.GetHeight())
            m_imgSliderTrackLan.SetSize(w, h);

        return m_imgSliderTrackLan;
    }
    else
    {
        w = WIDTH(m_rcLocation) - (IsVGA() ? 16 : 8);
        h = GetSliderTrackHeight();

        UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderBgVGA : IDR_PNG_FavoritesSliderBg;
        if(!m_imgSliderTrack.IsLoaded())
            m_imgSliderTrack.Initialize(m_hWndParent, m_hInst, uiRes);

        if(w != m_imgSliderTrack.GetWidth() || h != m_imgSliderTrack.GetHeight())
            m_imgSliderTrack.SetSize(w, h);

        return m_imgSliderTrack;
    }
}
  
CIssGDIEx&  CGuiFavorites::GetMovingNameImage()
{
    // make sure we have an item
	TypeLaunchItem* sItem = GetCurLinkArray()[m_iHoldingIndex];

    TCHAR szName[STRING_MAX];
    m_oStr->Empty(szName);

    if(sItem)
    {
        m_oStr->StringCopy(szName, sItem->szFriendlyName);
    }

    // calculate the size
    RECT rc = {0,0,m_rcGrid.right,20};  // start with something valid
    SIZE sTemp = {0,0};
    if(GetTextExtentPoint(m_gdiMem->GetDC(), szName, m_oStr->GetLength(szName), &sTemp))
    {
        rc.right = sTemp.cx + FAVORITES_TEXT_MARGIN*4;  
        rc.bottom = sTemp.cy + FAVORITES_TEXT_MARGIN*2;
    }

    // (re)create the gdi
    m_gdiNameBubble.Create(m_gdiMem->GetDC(), WIDTH(rc), HEIGHT(rc));

    // draw the bg frame image
    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesNameBblVGA : IDR_PNG_FavoritesNameBbl;
    if(!m_imgNameBubble.IsLoaded())
        m_imgNameBubble.Initialize(m_hWndParent, m_hInst, uiRes);
    if(WIDTH(rc) != m_imgNameBubble.GetWidth() || HEIGHT(rc) != m_imgNameBubble.GetHeight())
        m_imgNameBubble.SetSize(WIDTH(rc), HEIGHT(rc));

    Rectangle(m_gdiNameBubble.GetDC(), rc, 0, 0);
    m_imgNameBubble.DrawImage(m_gdiNameBubble, 0, 0);
    m_gdiNameBubble.SetAlphaMask(GetNameBubbleAlphaMask(rc));

    // draw the text
    ::DrawText(m_gdiNameBubble.GetDC(), szName, rc, DT_CENTER | DT_VCENTER);

    m_rcMovingName = rc;    // store the calculated size 

    return m_gdiNameBubble;
}

RECT CGuiFavorites::GetMovingNamePosition(RECT rcImageRect)
{
    RECT rc;

    // center above rcImageRect and check bounds
    rc.left = max(0, rcImageRect.left + WIDTH(rcImageRect)/2 - WIDTH(m_rcMovingName)/2);
    rc.right = rc.left + WIDTH(m_rcMovingName);
    if(rc.right > m_rcGrid.right)
    {
        rc.right = m_rcGrid.right;
        rc.left = rc.right - WIDTH(m_rcMovingName);
    }
    if(rc.left < m_rcGrid.left)
    {
        rc.left = m_rcGrid.left;
        rc.right = rc.left + WIDTH(m_rcMovingName);
    }
    rc.top = max(0, rcImageRect.top - HEIGHT(m_rcMovingName) - 4);
    rc.bottom = rc.top + HEIGHT(m_rcMovingName);
    return rc;
}
    
BOOL CGuiFavorites::OnChar(WPARAM wParam, LPARAM lParam)
{
	return FALSE;

}

BOOL CGuiFavorites::HasSelection()
{
    return (m_ptSelection.x > UND && m_ptSelection.x < m_sizeMatrix.cx) && 
        (m_ptSelection.y > UND && m_ptSelection.y < m_sizeMatrix.cy);
}

void CGuiFavorites::SetSelection(int x, int y)
{
    if(x >= UND && x < m_sizeMatrix.cx && y >= UND && y < m_sizeMatrix.cy)
    {
        m_ptSelection.x = x;
        m_ptSelection.y = y;
    }
}

int CGuiFavorites::GetSelected()
{
    for(int i = 0; i < GetCurLinkArray().GetSize(); i++)
    {
        TypeLaunchItem* sItem = GetCurLinkArray()[i];
        if(sItem)
        {
            if(sItem->ptLocation.x == m_ptSelection.x &&
                sItem->ptLocation.y == m_ptSelection.y)
                return i;
        }
    }
    return UND;
}

BOOL CGuiFavorites::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bResult = TRUE;
    POINT ptOldSelection = m_ptSelection;
	if(!HasSelection())
	{
		// we are moving from toolbar onto favorites matrix
		if(wParam == GetVKey(KEY_TakeFocus))
		{
            // just select the nearest block
            if(IsLandscape())
                SetSelection(0,0);
            else
                SetSelection(0,m_sizeMatrix.cy-1);
        }
        else
            return FALSE;
	}
	else if(wParam == GetVKey(KEY_Left))
    {
        if(m_ptSelection.x > 0)
            m_ptSelection.x--;
        else if(LOWORD(wParam) == GetVKey(KEY_LoseFocus))
        {
            return FALSE;
        }
    }
	else if(wParam == GetVKey(KEY_Right))
	{
        if(m_ptSelection.x < m_sizeMatrix.cx-1)
            m_ptSelection.x++;
	}
	else if(wParam == GetVKey(KEY_Up))
    {
        if(m_ptSelection.y > 0)
            m_ptSelection.y--;
    }
	else if(wParam == GetVKey(KEY_Down))
	{
        if(m_ptSelection.y < m_sizeMatrix.cy-1)
            m_ptSelection.y++;
        else if(wParam == GetVKey(KEY_LoseFocus))
        {
            return FALSE;
        }
	}
	else if(wParam == VK_RETURN)
	{
		if(GetSelected() != UND)
			LaunchItem();
        else
            AddProgram();
	}
	else
    {
        
    }

    if(ptOldSelection.x != m_ptSelection.x || ptOldSelection.y != m_ptSelection.y)
    {
        SetDirty(ptOldSelection, m_ptSelection);
	    InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
    }
    return bResult;
    
}

BOOL CGuiFavorites::HasFocus()
{
	return HasSelection();
}

void CGuiFavorites::LoseFocus()
{
    //DebugOut(_T("CGuiFavorites::LoseFocus()"));
    if(m_eBtnPressState != PS_pressed)
    {
        CGuiBase::SetDirty(m_rcLocation);
        InvalidateRect(m_hWndParent, NULL, FALSE);
    }
        
    SetDirty(m_ptSelection, m_ptSelection);
    SetSelection(UND, UND);
    m_eBtnPressState = PS_pressed;
    InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
    m_eBtnPressState = PS_noPress;
}


BOOL CGuiFavorites::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}
    
BOOL CGuiFavorites::OnSliderButtonDown(POINT pt)
{
    if((m_eBtnPressState == PS_noPress || m_eBtnPressState == PS_checkTap) && PtInRect(&GetSliderRect(), pt))
    {
        m_bSliderDrag = TRUE;
        m_iSliderPos = 0;
    }

    return UNHANDLED;
}

BOOL CGuiFavorites::OnSliderButtonUp(POINT pt)
{
    if(m_eBtnPressState != PS_noPress && m_eBtnPressState != PS_checkTap)
    {
        m_eBtnPressState = PS_noPress;
        CGuiBase::SetDirty(m_rcLocation);
        InvalidateRect(m_hWndParent, NULL, FALSE);     
        return UNHANDLED;
    }

    int eOrigPage = m_ePage;
    int eNewPage = eOrigPage;
    if(m_bSliderDrag)
    {
        if(IsLandscape())
        {
            if(GetSliderRect().top - m_rcSlider.top  < m_rcSlider.bottom - GetSliderRect().bottom)
                eNewPage = eProgramsPage;
            else
                eNewPage = eGamesPage;
        }
        else
        {
            if(GetSliderRect().left - m_rcSlider.left  < m_rcSlider.right - GetSliderRect().right)
                eNewPage = eProgramsPage;
            else
                eNewPage = eGamesPage;
        }

        // now animate the slider
        AnimateSlider(pt, eNewPage);
        
        m_bSliderDrag = FALSE;
        m_ePage = eNewPage;
    }
    else
    {
        //check the locations
        RECT rcTemp;

        if(IsLandscape())
        {
            rcTemp = m_rcSlider;
            rcTemp.bottom -= HEIGHT(rcTemp)/2;
        }
        else
        {
            rcTemp = m_rcSlider;
            rcTemp.right -= WIDTH(rcTemp)/2;
        }
        if(PtInRect(&rcTemp, pt))
            eNewPage = eProgramsPage;
        else
            eNewPage = eGamesPage;

        if(eOrigPage != eNewPage)
        {
            POINT ptTemp;

            ptTemp.x = rcTemp.left;
            ptTemp.y = rcTemp.top;

            if(eNewPage == eGamesPage)
            {
                ptTemp.x = rcTemp.right;
                ptTemp.y = rcTemp.bottom;
            }
    
            // now animate the slider
            AnimateSlider(pt, eNewPage);
            m_ePage = eNewPage;
        }
    }

    // invalidate the entire screen
    if(eOrigPage != m_ePage)
    {
        SetSliderDirty();
        InvalidateRect(m_hWndParent, &m_rcSlider, FALSE);
        SlidePage();
        InvalidateRect(m_hWndParent, &m_rcLocation, FALSE);
    }
    else
        InvalidateRect(m_hWndParent, &m_rcSlider, FALSE);

    return UNHANDLED;
}
        
void CGuiFavorites::SetSliderDirty()
{
    if(IsRectEmpty(&m_rcDirty))
        m_rcDirty = m_rcSlider;
    else
        UnionRect(&m_rcDirty, &m_rcDirty, &m_rcSlider);
}

void CGuiFavorites::AnimateSlider(POINT pt, int iDestPage)
{
    if(IsLandscape())
        m_iSliderPos = pt.y - m_ptLastLButtonDown.y;
    else
        m_iSliderPos = pt.x - m_ptLastLButtonDown.x;

    int iStart;
    int iEnd;
    if(IsLandscape())
    {
        iStart = GetSliderRect(m_ePage, TRUE).top;
        iEnd = GetSliderRect(iDestPage, FALSE).top;
    }
    else
    {
        iStart = GetSliderRect(m_ePage, TRUE).left;
        iEnd = GetSliderRect(iDestPage, FALSE).left;
    }

    //DebugOut(_T("animating from %d to %d"), iStart, iEnd);

    HDC dc = GetDC(m_hWndParent);
    DWORD dwStart = GetTickCount();
    BOOL bInc = (iEnd - iStart > 0);
    int iNumFrames = abs(iEnd - iStart);
    int iMaxTravel = IsLandscape() ? HEIGHT(m_rcSlider)/2 : WIDTH(m_rcSlider)/2;
    float fAnimateTime = 100.0f + (float)FAVORITES_SLIDER_ANIMATE_TIME * iNumFrames / iMaxTravel;
    int i = GetCurrentFrame(dwStart, iNumFrames);
    int iLast;

    m_bSliderDrag = TRUE;//to fool the GetSliderRect
    while(i < iNumFrames)
    {
        iLast = i;
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
        int iAmount = i-iLast;
        if(!bInc)
            iAmount = -iAmount;
        m_iSliderPos += iAmount;

        DrawSlider(*m_gdiBg, m_rcSlider, m_rcSlider);
		BitBlt(dc,
			m_rcSlider.left,m_rcSlider.top,
			WIDTH(m_rcSlider), HEIGHT(m_rcSlider),
			m_gdiBg->GetDC(),
			m_gdiBg->GetWidth() - WIDTH(m_rcSlider),m_rcSlider.top,
			SRCCOPY);
    }
    m_bSliderDrag = FALSE;
	ReleaseDC(m_hWndParent, dc);

}
BOOL CGuiFavorites::OnSliderMouseMove(POINT pt)
{
    if(IsLandscape())
        m_iSliderPos = pt.y - m_ptLastLButtonDown.y;
    else
        m_iSliderPos = pt.x - m_ptLastLButtonDown.x;

    // draw the slider
    if(m_bSliderDrag)
    {
        HDC dc = GetDC(m_hWndParent);
        DrawSlider(*m_gdiBg, m_rcSlider, m_rcSlider);   
		BitBlt(dc,
			m_rcSlider.left,m_rcSlider.top,
			WIDTH(m_rcSlider), HEIGHT(m_rcSlider),
			m_gdiBg->GetDC(),
			m_gdiBg->GetWidth() - WIDTH(m_rcSlider),m_rcSlider.top,
			SRCCOPY);
		ReleaseDC(m_hWndParent, dc);
    }

    return UNHANDLED;
}

BOOL CGuiFavorites::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
    {
        // lose focus
        SetDirty(m_ptSelection, m_ptSelection);
        //SetSelection(UND, UND);
        InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
		return FALSE;
    }

    m_bMouseDown = TRUE;
	m_ptLastLButtonDown = pt;
    m_dwTickCountGrab   = GetTickCount();
    m_eBtnPressState = PS_checkTap;

	if(PtInRect(&m_rcSlider, pt))
		return OnSliderButtonDown(pt);

    // select it
    POINT ptNewSelection = GetMatrixLocation(pt);
    POINT ptOld = m_ptSelection;
    m_bPressSelected = (m_ptSelection.x == ptNewSelection.x && m_ptSelection.y == ptNewSelection.y);
    SetSelection(ptNewSelection.x, ptNewSelection.y);
	m_iHoldingIndex = GetSelected();

    SetDirty(ptNewSelection, ptOld);
    InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);


    if(m_iHoldingIndex != UND)
	{
        // store the moving item gdi
		TypeLaunchItem* sItem = GetCurLinkArray()[m_iHoldingIndex];
		if(!sItem)
			return TRUE;
		POINT ptItem = sItem->ptLocation;
        RECT rcItem = GetItemRect(ptItem);
		m_sizeOffset.cx = pt.x - rcItem.left - m_rcGrid.left;
		m_sizeOffset.cy = pt.y - rcItem.top;
		// save the item image that we are moving
        m_gdiMovingItem.Create(m_gdiMem->GetDC(), WIDTH(rcItem), HEIGHT(rcItem));
        m_gdiMovingItem.SetAlphaMask(GetMovingItemMask());
		DrawItem(sItem, m_gdiMovingItem.GetDC(), rcItem, sItem->gdiImage, m_hFont, TRUE, NULL, m_sOptions->bAlwaysShowText || sItem->eType == LT_FileOrFolder);
        
        // create and store the moving name image
        GetMovingNameImage();
    }
	return TRUE;
}

BOOL CGuiFavorites::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE && m_eBtnPressState != PS_pressed)
		return FALSE;

    m_bMouseDown = FALSE;

	if(m_bSliderDrag || PtInRect(&m_rcSlider, pt))
		return OnSliderButtonUp(pt);

    // any place we mouse up will be selected
    // select this position
    POINT ptMatrix = GetMatrixLocation(pt);
    POINT ptOldSelection = m_ptSelection;

    BOOL bWasTapped = (GetTickCount()-m_dwTickCountGrab) < FAVORITES_MAX_TapLaunchTime;
    BOOL bDoLaunch = bWasTapped || m_bPressSelected;
    BOOL bWasDragging = (m_eBtnPressState == PS_dragging);

    SetSelection(ptMatrix.x, ptMatrix.y);

	if(m_iHoldingIndex == UND/*&& !m_bContextMenu*/)
	{
        // add new
        if(bDoLaunch)
            AddItem();
    }
	if(m_iHoldingIndex != UND /*&& !m_bContextMenu*/)
	{
		if(m_eBtnPressState == PS_dragging)
		{
			if(!m_bLockItems)
			{
				// first check if we in bounds, and correct if we're not
				CheckBounds(m_ptLastHolding);

				// try and place the icon down
				if(!IsSwapStraight(m_ptLastHolding))
				{
					if(IsSwapAndFindRoom(m_ptLastHolding))
						SaveRegistry();
				}
				else
					SaveRegistry(m_iHoldingIndex);
			}
		}
		else
		{
			// launch the item
            if(bDoLaunch)
			    LaunchItem();
		}

		ReleaseCapture();
		m_gdiMovingItem.Destroy();
		//m_gdiMovingBack.Destroy();
	}

	m_iHoldingIndex = UND;
    m_eBtnPressState = PS_noPress;
    SetDirty(ptMatrix, ptOldSelection);
    RECT rcInval = m_rcDirty;
    if(bWasDragging)
        UnionRect(&rcInval, &m_rcDirty, &m_rcDragDraw);
	InvalidateRect(m_hWndParent, NULL, FALSE);
	m_bContextMenu = FALSE;

	return TRUE;
}

BOOL CGuiFavorites::OnMouseMove(POINT pt)
{
    if(!PtInRect(&m_rcLocation, pt))
        return FALSE;

	if(m_bSliderDrag)
		return OnSliderMouseMove(pt);

    // evaluate m_eBtnPressState
    int iMoveThreshold = IsVGA() ? FAVORITES_MIN_DragMoveThresh * 2 : FAVORITES_MIN_DragMoveThresh;
    if(m_eBtnPressState == PS_checkTap && 
       (GetTickCount()-m_dwTickCountGrab > FAVORITES_MAX_TapLaunchTime ||
        abs(m_ptLastLButtonDown.x - pt.x) >= iMoveThreshold ||
        abs(m_ptLastLButtonDown.y - pt.y) >= iMoveThreshold))
        m_eBtnPressState = PS_pressed;

    if(m_eBtnPressState == PS_checkTap)
        return TRUE;
    
	if(m_iHoldingIndex != UND && !m_bLockItems)
    {
        BOOL bPressStateChanged =  FALSE;
        if(m_eBtnPressState != PS_dragging)
        {
            SetDirty(m_ptSelection, m_ptSelection);
            bPressStateChanged = TRUE;
        }

        m_eBtnPressState = PS_dragging;

        // collect invalidation area...
        int w, h;
        GetItemSize(m_rcGrid, w, h);

        // 1) previous item
		RECT rcItem, rcLastItem, rcBack;
		rcLastItem.left		= m_ptLastHolding.x - m_sizeOffset.cx;
		rcLastItem.top		= m_ptLastHolding.y - m_sizeOffset.cy;
		rcLastItem.right	= rcLastItem.left + w;
		rcLastItem.bottom	= rcLastItem.top + h;
		CIssRect rcFull(rcLastItem);
		
        // 2) previous shadow
        RECT rcLastShadow = rcLastItem;
        OffsetRect(&rcLastShadow, GetShadowOffset(), GetShadowOffset());
		rcFull.Concatenate(rcLastShadow);

        // 3) previous back
        rcBack = GetItemRect(GetMatrixLocation(m_ptLastHolding));
        // GetItemRect() is referenced to m_rcGrid, so offset it
        OffsetRect(&rcBack, m_rcLocation.left, m_rcLocation.top);
		rcFull.Concatenate(rcBack);

		// do some quick checks to make sure our movement is not out of bounds,
		// and if it is then make the adjustments
		CheckBounds(pt);	
        m_ptHolding = pt;
        m_ptLastHolding = pt;

        // 4) current item
		rcItem.left		= pt.x - m_sizeOffset.cx;
		rcItem.top		= pt.y - m_sizeOffset.cy;
		rcItem.right	= rcItem.left + w;
		rcItem.bottom	= rcItem.top + h;
		rcFull.Concatenate(rcItem);

        // 5) current shadow
        RECT rcShadow = rcItem;
        OffsetRect(&rcShadow, GetShadowOffset(), GetShadowOffset());
		rcFull.Concatenate(rcShadow);
    
        // 6) current back
        rcBack = GetItemRect(GetMatrixLocation(pt));
        // GetItemRect() is referenced to m_rcLocation, so offset it
        OffsetRect(&rcBack, m_rcLocation.left, m_rcLocation.top);
		rcFull.Concatenate(rcBack);

        // 7) previous name buble
        RECT rcPreviousName = GetMovingNamePosition(rcLastItem);
        rcFull.Concatenate(rcPreviousName);

        // 8) current name buble
        RECT rcName = GetMovingNamePosition(rcItem);
        rcFull.Concatenate(rcName);

        // 9) original location
        if(bPressStateChanged)
            rcFull.Concatenate(GetItemRect(m_ptSelection));

        InvalidateRect(m_hWndParent, &rcFull.Get(), FALSE);
        UpdateWindow(m_hWndParent);
    }

	return TRUE;
}

BOOL CGuiFavorites::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_SmartArrange:
		SmartArrangeItems();
		InvalidateRect(m_hWndParent, NULL, FALSE);
		break;
	case IDMENU_LockItems:
		m_bLockItems = !m_bLockItems;
		break;
	case IDMENU_RemoveItem:
        if(RemoveItem(GetSelected()))
        {
            SaveRegistry();
            InvalidateRect(m_hWndParent, NULL, FALSE);
        }
        break;
	case IDMENU_AddProgram:
		AddProgram();
		break;
    case IDMENU_AddSetting:
        AddSetting();
        break;
    case IDMENU_AddFileFolder:
        AddFileFolder();
        break;
	}
	return TRUE;
}

BOOL CGuiFavorites::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_FadeIn)
	{
		m_bFadeIn = FALSE;
		HDC hDC = GetDC(m_hWndParent);
		FadeIn(hDC, *m_gdiMem, 900, fnAnimateIn, this);
		ReleaseDC(m_hWndParent, hDC);
		InvalidateRect(m_hWndParent, NULL, FALSE);
	}
	return TRUE;
}
  
void CGuiFavorites::AddItem()
{
    m_wndMenu->ResetContent();
    m_wndMenu->AddCategory(ID(IDS_MENU_AddFavorite), m_hInst);

    m_wndMenu->AddItem(ID(IDS_MENU_AddProgram), m_hInst, IDMENU_AddProgram);
    m_wndMenu->AddItem(ID(IDS_MENU_AddSetting), m_hInst, IDMENU_AddSetting);
    m_wndMenu->AddItem(ID(IDS_MENU_AddFileFolder), m_hInst, IDMENU_AddFileFolder);
    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    Sleep(500);

    RECT rcWindow;
    GetWindowRect(m_hWndParent, &rcWindow);

    /*int iX = WIDTH(GetFrame())/6;

    int iY = GetFrame().bottom - 10 + rcWindow.top;

    m_wndMenu->PopupMenu(m_hWndParent, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        iX,10,WIDTH(GetFrame())*2/3,iY,
        GetFrame().left, iY, iX,10, ADJUST_Bottom);*/

    int iWidth = WIDTH(GetFrame())*5/6;

    int iY = HEIGHT(GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

    m_wndMenu->PopupMenu(m_hWndParent, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        GetFrame().left + (WIDTH(GetFrame()) - iWidth)/2, rcWindow.top,
        iWidth, iY,
        0, 0, 0,0, ADJUST_Bottom);


}

void CGuiFavorites::DrawBackground(HDC dc, RECT& rcClient, RECT& rcClip)
{
    RECT rc = rcClip;
    rc.left = max(rcClient.left, rcClip.left);

    FillRect(dc, rc, COLOR_FAVORITES_BORDER);
}
    
void CGuiFavorites::GetItemSize(RECT& rcClient, int& w, int& h)
{
    w = (WIDTH(rcClient) - FAVORITES_GRID_BORDER*2) / m_sizeMatrix.cx;
    h = (HEIGHT(rcClient) - FAVORITES_GRID_BORDER*2) / m_sizeMatrix.cy;

    // this would be max size...
    // now pad it
    w -= FAVORITES_GRID_MARGIN*2;
    h -= FAVORITES_GRID_MARGIN*2;
}
    
RECT CGuiFavorites::GetItemRect(int iMatrixX, int iMatrixY)
{
    POINT pt = {iMatrixX, iMatrixY};
    return GetItemRect(pt);
}

RECT CGuiFavorites::GetItemRect(POINT ptMatrixPos)
{
    RECT rc = {0,0,0,0};

    // ensure ptMatrix is valid
    if(ptMatrixPos.x > m_sizeMatrix.cx || ptMatrixPos.y > m_sizeMatrix.cy || ptMatrixPos.x < 0 || ptMatrixPos.y < 0)
        return rc;
    
    int w, h;
    GetItemSize(m_rcGrid, w, h);
    rc.left = /*m_rcGrid.left + */FAVORITES_GRID_BORDER + FAVORITES_GRID_MARGIN + ptMatrixPos.x * m_sizeBlock.cx;
    rc.top = m_rcGrid.top + FAVORITES_GRID_BORDER + FAVORITES_GRID_MARGIN + ptMatrixPos.y * m_sizeBlock.cy;
    rc.right = rc.left + w;
    rc.bottom = rc.top + h;

    return rc;
}

void CGuiFavorites::DrawGrid(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, rcClient))
        return;

    // draw a bunch of these
    RECT rcDraw;
    for(int x = 0; x < m_sizeMatrix.cx; x++)
    {
         for(int y = 0; y < m_sizeMatrix.cy; y++)
        {
            int l,t;
            l = rcClient.left + FAVORITES_GRID_BORDER + FAVORITES_GRID_MARGIN + x*m_sizeBlock.cx;
            t = rcClient.top + FAVORITES_GRID_BORDER + FAVORITES_GRID_MARGIN + y*m_sizeBlock.cy;
            SetRect(&rcDraw, l, t, l + m_imgGrid.GetWidth(), t + m_imgGrid.GetHeight());

            if(IsRectInRect(rcClip, rcDraw))
                m_imgGrid.DrawImage(gdi, rcDraw.left, rcDraw.top);
                //::Draw(gdi, rcDraw, m_imgGrid.GetImage());
        }
    }
}

void CGuiFavorites::DrawSelected(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, rcClient))
        return;

	if(!HasSelection())
		return;

    RECT rcSelected = GetItemRect(m_ptSelection);
    // offsets for drawing page slide
    int iYOffset = 0;
    if(!IsLandscape() && rcClient.top == 0)
        iYOffset = -GetSliderHeight();

    OffsetRect(&rcSelected, rcClient.left, iYOffset);

    if(::IsRectInRect(rcSelected, rcClip))
        GetSelector().DrawImage(gdi, rcSelected.left, rcSelected.top);
}


void CGuiFavorites::DrawSliderBg(CIssGDIEx& gdi)
{
    // don't call this directly - use GetSliderBg()

    RECT rcGrad1, rcGrad2;
    // zero the rects
    SetRect(&rcGrad1, 0, 0, WIDTH(m_rcSlider), HEIGHT(m_rcSlider));
    rcGrad2 = rcGrad1;

    // slider bg
    if(IsLandscape())
    {
        rcGrad1.left = rcGrad2.right = rcGrad1.right - (GetSliderHeight() * 3 / 5);
        rcGrad2.left  = 0;//rcClient.right - FAVORITES_SLIDER_HEIGHT;
        GradientHorzFillRect(gdi, rcGrad1, COLOR_SLIDER_GRAD1_ST, COLOR_SLIDER_GRAD1_END);
        GradientHorzFillRect(gdi, rcGrad2, COLOR_SLIDER_GRAD2_ST, COLOR_SLIDER_GRAD2_END);
    }
    else
    {
        rcGrad1.bottom = rcGrad2.top = rcGrad1.top + (FAVORITES_SLIDER_HEIGHT * 3 / 5);
        rcGrad2.bottom  = GetSliderHeight();
        GradientFillRect(gdi, rcGrad1, COLOR_SLIDER_GRAD1_ST, COLOR_SLIDER_GRAD1_END);
        GradientFillRect(gdi, rcGrad2, COLOR_SLIDER_GRAD2_ST, COLOR_SLIDER_GRAD2_END);
    }
    // slider track
    int h = GetSliderTrack().GetHeight();
    int w = GetSliderTrack().GetWidth();
    RECT rcTrack = {0,0,WIDTH(m_rcSlider), HEIGHT(m_rcSlider)};//rcClient;
    if(IsLandscape())
        w-= 1; //adjust for rounding
    InflateRect(&rcTrack, (w - WIDTH(m_rcSlider))/2, (h - HEIGHT(m_rcSlider))/2);
    GetSliderTrack().DrawImage(gdi, rcTrack.left, rcTrack.top);

}
void CGuiFavorites::DrawSlider(CIssGDIEx& gdi, RECT& rc, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, m_rcSlider))
        return;

	BitBlt(gdi.GetDC(),
		gdi.GetWidth() - WIDTH(m_rcSlider), rc.top,
		WIDTH(m_rcSlider), HEIGHT(m_rcSlider),
		GetSliderBg().GetDC(),
		0, 0,
		SRCCOPY);

    DrawSliderKnob(gdi, rc);
}
    
RECT CGuiFavorites::GetSliderRect()
{
    return GetSliderRect(m_ePage, m_bSliderDrag);
}

RECT CGuiFavorites::GetSliderRect(int iPage, BOOL bDrag)
{
    RECT rc = m_rcSlider;

    if(IsLandscape())
    {
        int iIndent = (WIDTH(m_rcSlider) - GetSliderImg(TRUE).GetWidth()) / 2;
        int iSlideAmount = HEIGHT(m_rcSlider) - iIndent * 2;
        iSlideAmount -= (IsVGA() ? 5 : 1);
        //int iBtnHeight = iSlideAmount * 2 / 5;
        int iBtnHeight = GetSliderImg(TRUE).GetHeight();

        int iTravel = iSlideAmount - iBtnHeight + 1;
        InflateRect(&rc, -iIndent, -(iIndent));
        rc.top += 1;
        if(IsVGA())
            rc.bottom -= 4;
        if(iPage == eProgramsPage)
        {
            rc.bottom = rc.top + iBtnHeight;
            if(bDrag)
            {
                int iOffset = m_iSliderPos < 0 ? 0 : min(m_iSliderPos, iTravel);
                OffsetRect(&rc, 0, iOffset);
            }
        }
        else
        {
            rc.top = rc.bottom - iBtnHeight + 1;
            if(bDrag)
            {
                int iOffset = m_iSliderPos > 0 ? 0 : max(m_iSliderPos, -iTravel);
                OffsetRect(&rc, 0, iOffset);
            }
        }
    }
    else
    {
        int iIndent = (HEIGHT(m_rcSlider) - GetSliderImg().GetHeight()) / 2;
        int iSlideAmount = WIDTH(m_rcSlider) - iIndent * 2;
        iSlideAmount += (IsVGA() ? -4 : 1);
        int iBtnWidth = iSlideAmount * 2 / 5;
        int iTravel = iSlideAmount - iBtnWidth;
        InflateRect(&rc, -(iIndent), -iIndent);
        rc.left += 1;
        rc.right += (IsVGA() ? -3 : 2);
        if(iPage == eProgramsPage)
        {
            rc.right = rc.left + iBtnWidth;
            if(bDrag)
            {
                int iOffset = m_iSliderPos < 0 ? 0 : min(m_iSliderPos, iTravel);
                OffsetRect(&rc, iOffset, 0);
            }
        }
        else
        {
            rc.left = rc.right - iBtnWidth;
            if(bDrag)
            {
                int iOffset = m_iSliderPos > 0 ? 0 : max(m_iSliderPos, -iTravel);
                OffsetRect(&rc, iOffset, 0);
            }
        }
    }

    return rc;
}

void CGuiFavorites::DrawSliderKnob(CIssGDIEx& gdi, RECT& rc)
{
    const int iTextIndent = 12;
    RECT rcSlider;
    if(IsLandscape())
    {
        //1) slider image
        rcSlider = GetSliderRect();
        // since this is drawn on the right side and some of our rects are not 0-based,
        // check to see if an offset is needed
        if(rcSlider.right > gdi.GetWidth())
            OffsetRect(&rcSlider, -m_rcLocation.left, 0);
        GetSliderImg(IsLandscape()).DrawImage(gdi, rcSlider.left, rcSlider.top);

        // 2) slider text
        // the positioning of angled text seems unpredictable at this time...
        RECT rcSliderLan = rcSlider;
        if(m_ePage == eProgramsPage)
        {
       //     GetClientRect(m_hWndParent, &rcSliderLan);
            

            HFONT hFontOld = NULL;

            if(m_hLabelFontRot)
                hFontOld = (HFONT)SelectObject(gdi.GetDC(), m_hLabelFontRot);   
            
            SIZE sTemp;
            GetTextExtentPoint(gdi.GetDC(), GetSliderText(), m_oStr->GetLength(GetSliderText()), &sTemp);

            if(hFontOld)
                SelectObject(gdi.GetDC(), hFontOld);  


            rcSliderLan.left -= 2*sTemp.cx;
            rcSliderLan.right += 2*sTemp.cx;

            rcSliderLan.left += (sTemp.cx + sTemp.cy);
            rcSliderLan.top += (sTemp.cy);

            rcSliderLan.left += sTemp.cy/3;//not sure ... fudge factor

             // OffsetRect(&rcSliderLan, m_rcLocation.left - (IsVGA() ? 16:8), iTextIndent);
            ::DrawText(gdi.GetDC(), GetSliderText(), rcSliderLan, DT_CENTER | DT_TOP, m_hLabelFontRot, 0x5a5a5a);


         /*   rcSliderLan.left = rcSliderLan.right - HEIGHT(rcSliderLan);
            OffsetRect(&rcSliderLan, m_rcLocation.left - (IsVGA() ? 16:8), iTextIndent);
            ::DrawText(gdi.GetDC(), GetSliderText(), rcSliderLan, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0x5a5a5a);*/
       }
        else
        {
            HFONT hFontOld = NULL;

            if(m_hLabelFontRot)
                hFontOld = (HFONT)SelectObject(gdi.GetDC(), m_hLabelFontRot);   

            SIZE sTemp;
            GetTextExtentPoint(gdi.GetDC(), GetSliderText(), m_oStr->GetLength(GetSliderText()), &sTemp);

            if(hFontOld)
                SelectObject(gdi.GetDC(), hFontOld);  


            rcSliderLan.left -= 2*sTemp.cx;
            rcSliderLan.right += 2*sTemp.cx;

            rcSliderLan.left += (sTemp.cx + sTemp.cy);
            rcSliderLan.bottom -= (sTemp.cy);
            rcSliderLan.top = rcSliderLan.bottom - sTemp.cx;

            rcSliderLan.left += sTemp.cy/3;//not sure ... fudge factor

            // OffsetRect(&rcSliderLan, m_rcLocation.left - (IsVGA() ? 16:8), iTextIndent);
            ::DrawText(gdi.GetDC(), GetSliderText(), rcSliderLan, DT_CENTER | DT_TOP, m_hLabelFontRot, 0x5a5a5a);


    //        rcSliderLan.left = rcSliderLan.right - HEIGHT(rcSliderLan);
    //        OffsetRect(&rcSliderLan, m_rcLocation.left - (IsVGA() ? 16:7), (IsVGA() ? 74 : 28));
    //        ::DrawText(gdi.GetDC(), GetSliderText(), rcSliderLan, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0x5a5a5a);
        }
    }
    else
    {
        //1) slider image
        rcSlider = GetSliderRect();
        GetSliderImg(IsLandscape()).DrawImage(gdi, rcSlider.left, rcSlider.top);
        // 2) slider text
        if(m_ePage == eProgramsPage)
        {
            rcSlider.left += iTextIndent;
            ::DrawText(gdi.GetDC(), GetSliderText(), rcSlider, DT_LEFT | DT_VCENTER, m_hLabelFont, 0x5a5a5a);
        }
        else
        {
            rcSlider.right -= iTextIndent;
            ::DrawText(gdi.GetDC(), GetSliderText(), rcSlider, DT_RIGHT | DT_VCENTER, m_hLabelFont, 0x5a5a5a);
        }
    }

    // 3) arrow
    DrawSliderArrow(gdi);

}
    
void  CGuiFavorites::DrawSliderArrow(CIssGDIEx& gdi)
{
    RECT rcSlider = GetSliderRect();

    if(rcSlider.right > gdi.GetWidth())
        OffsetRect(&rcSlider, -m_rcLocation.left, 0);

    int iArrowIndex;
    if(IsLandscape())
    {
        if(m_ePage == eProgramsPage)
        {
            // draw down arrow on the bottom
            rcSlider.top = rcSlider.bottom - WIDTH(rcSlider);
            iArrowIndex = 0;
        }
        else
        {
            // draw up arrow on the top
            rcSlider.bottom = rcSlider.top + WIDTH(rcSlider);
            iArrowIndex = 1;
        }
        OffsetRect(&rcSlider, 1, -1);
    }
    else
    {
        if(m_ePage == eProgramsPage)
        {
            // draw right arrow on the right
            rcSlider.left = rcSlider.right - HEIGHT(rcSlider);
            iArrowIndex = 2;
        }
        else
        {
            // draw left arrow on the left
            rcSlider.right = rcSlider.left + HEIGHT(rcSlider);
            iArrowIndex = 3;
        }
        OffsetRect(&rcSlider, -1, -1);
    }

    int iInflate = (HEIGHT(rcSlider) - m_gdiImgArrowArray.GetHeight())/2;
    InflateRect(&rcSlider, -iInflate, -iInflate);
    ::Draw(gdi,
        rcSlider.left, rcSlider.top,
        WIDTH(rcSlider), HEIGHT(rcSlider),
        m_gdiImgArrowArray,
        iArrowIndex*m_gdiImgArrowArray.GetHeight(), 0);  

}

TCHAR* CGuiFavorites::GetSliderText()
{
    if(m_ePage == eProgramsPage)
        return m_szSliderText[0];
    else
        return m_szSliderText[1];

}
	
void CGuiFavorites::DrawItems(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip, int iExcludeItem /*= UND*/)
{
    DrawItems(gdi, rcClient, rcClip, iExcludeItem, m_ePage);
}

void CGuiFavorites::DrawItems(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip, int iExcludeItem, int ePage)
{
    if(!IsRectInRect(rcClip, rcClient))
        return;

    RECT rcItem = {0,0,WIDTH(GetItemRect(0,0)), HEIGHT(GetItemRect(0,0))};
    CIssGDIEx gdiTemp;
    gdiTemp.Create(gdi.GetDC(), rcItem);

    BOOL bSaveRegistry = FALSE;

    gdiTemp.SetAlphaMask(GetItemAlphaMask().GetImage());
    int **iMatrix = ePage == eProgramsPage ? m_iProgramsMatrix : m_iGamesMatrix;
    CIssVector<TypeLaunchItem> &arrLinks = ePage == eProgramsPage ? m_arrPrograms : m_arrGames;
	for(int i = 0; i < arrLinks.GetSize(); i++)
	{
		TypeLaunchItem* sItem = arrLinks[i];
		if(sItem /*&& i != iExcludeItem*/)
		{
            if(i == iExcludeItem && m_eBtnPressState == PS_dragging)
                continue;
            // draw to the temp gdi, then draw it to the caller gdi

            // if it is off the matrix, then set it valid
            // can happen if matrix size changes between portrait and landscape
            if(sItem->ptLocation.x >= m_sizeMatrix.cx || sItem->ptLocation.y >= m_sizeMatrix.cy)
            {
                SetMatrixValue(iMatrix, 
					sItem->ptLocation, UND, 1, 1);

                sItem->ptLocation.x = UND;
                sItem->ptLocation.y = UND;
                
                AutoArrangeItems(iMatrix, arrLinks);
                bSaveRegistry = TRUE;
            }

            RECT rcDraw = GetItemRect(sItem->ptLocation);
            // these offsets are applied when drawing the page slide
            int iYOffset = 0;
            if(!IsLandscape() && rcClient.top == 0)
                iYOffset = -GetSliderHeight();
            OffsetRect(&rcDraw, rcClient.left, iYOffset);

            if(!IsRectInRect(rcDraw, rcClip))
                continue;

            DrawItem(sItem, gdiTemp.GetDC(), rcItem/*rcClient*/, sItem->gdiImage, 
                m_hFont, TRUE, NULL, m_sOptions->bAlwaysShowText || sItem->eType == LT_FileOrFolder);

            ::Draw(gdi, rcDraw, gdiTemp);

            // draw the border
            GetItemBorder().DrawImage(gdi, rcDraw.left,rcDraw.top);
		}
	}

    if(bSaveRegistry)
        SaveRegistry();

    // now draw the 'addnew' img
    for(int x = 0; x < m_sizeMatrix.cx; x++)
    {
        for(int y = 0; y < m_sizeMatrix.cy; y++)
        {
            if(iMatrix[y][x] == UND)
            {
                // draw the add new image here
                RECT rc;
                rc.left = /*m_rcGrid.left + */FAVORITES_GRID_BORDER + x*m_sizeBlock.cx;
                rc.left += m_sizeBlock.cx/2 - m_gdiAddNew.GetWidth()/2;
                rc.top = m_rcGrid.top + FAVORITES_GRID_BORDER + y*m_sizeBlock.cy;
                rc.top += m_sizeBlock.cy/2 - m_gdiAddNew.GetHeight()/2;
                rc.right = rc.left + m_gdiAddNew.GetWidth();
                rc.bottom = rc.top + m_gdiAddNew.GetHeight();
                
                // these offsets are applied when drawing the page slide
                int iYOffset = 0;
                if(!IsLandscape() && rcClient.top == 0)
                    iYOffset = -GetSliderHeight();
                OffsetRect(&rc, rcClient.left, iYOffset);

                if(!IsRectInRect(rc, rcClip))
                    continue;

                if(m_bMouseDown && m_ptSelection.x == x && m_ptSelection.y == y)
                {
                    // draw glow image
                    if(m_gdiGlowImg)
                    {
                        int iGlowWidth = m_gdiGlowImg->GetWidth();
                        int iInflate = (iGlowWidth - WIDTH(rc)) / 2;
                        RECT rcGlow = rc;
                        InflateRect(&rcGlow, iInflate, iInflate);
                        ::Draw(gdi, rcGlow, *m_gdiGlowImg);
                    }
                }
                ::Draw(gdi, rc, m_gdiAddNew);
            }
        }
    }

}

void CGuiFavorites::CheckBounds(POINT& pt)
{
	TypeLaunchItem* sItem = GetCurLinkArray()[m_iHoldingIndex];
	if(sItem == NULL)
		return;

    int w,h;
    GetItemSize(m_rcGrid, w, h);

	// left side
	if((pt.x - m_sizeOffset.cx) < m_rcGrid.left)
		pt.x = m_rcGrid.left + m_sizeOffset.cx;
	// right side
	else if((pt.x - m_sizeOffset.cx + w) >  m_rcGrid.right)
		pt.x = m_rcGrid.right + m_sizeOffset.cx - w;

	// top
	if((pt.y - m_sizeOffset.cy) < m_rcGrid.top)
		pt.y = m_rcGrid.top + m_sizeOffset.cy;
	// bottom
	else if((pt.y - m_sizeOffset.cy + h) > m_rcGrid.bottom)
		pt.y = m_rcGrid.bottom + m_sizeOffset.cy - h;
}


// given a point on the screen find out where on the screen it hits the matrix array
POINT CGuiFavorites::GetMatrixLocation(POINT pt)
{
    POINT ptMatrix = {-1,-1};//0};	// undefined

    // 0-base the POINT for this calculation
    pt.x -= m_rcGrid.left;
    pt.y -= m_rcGrid.top;

	// get the new location we want (this will be an index in the matrix array)
	ptMatrix.x = pt.x/m_sizeBlock.cx;
	ptMatrix.y = pt.y/m_sizeBlock.cy;

    //Bounds CHECKING!!!!!
    if(ptMatrix.x >= m_sizeMatrix.cx - 1)
        ptMatrix.x = m_sizeMatrix.cx - 1;

    if(ptMatrix.y >= m_sizeMatrix.cy - 1)
        ptMatrix.y = m_sizeMatrix.cy - 1;


	return ptMatrix;
}


// Does one rect overlap the other rect
BOOL CGuiFavorites::IsOverlapping(RECT& rcBig, RECT& rcSmall)
{
	int loX = max(rcBig.left, rcSmall.left);
	int hiX = min(rcBig.right, rcSmall.right);
	int loY = max(rcBig.top, rcSmall.top);
	int hiY = min(rcBig.bottom, rcSmall.bottom);

	if(loX >= hiX || loY >= hiY)
		return FALSE;
	else
		return TRUE;
}

void CGuiFavorites::SwapMatrixIndex(int& iHoldingIndex, POINT& pt)
{
	// set the offset to the first one first
	pt.x -= m_sizeOffset.cx;
	pt.y -= m_sizeOffset.cy;

	// get the new location we want (this will be an index in the matrix array)
	int xMatrix = pt.x/m_sizeBlock.cx;
	int yMatrix = pt.y/m_sizeBlock.cy;

	TypeLaunchItem* sItem = GetCurLinkArray()[iHoldingIndex];
	if(!sItem)
		return;

    int **iMatrix = GetCurMatrix();
	int iNewX, iNewY, iOldX, iOldy;
	int iTemp;

    POINT ptItem	= sItem->ptLocation;
	iNewX = xMatrix;
	iNewY = yMatrix;
	iOldX = ptItem.x;
	iOldy = ptItem.y;
	if(iNewX > m_sizeMatrix.cx || iOldX > m_sizeMatrix.cx ||
	   iNewY > m_sizeMatrix.cy || iOldy > m_sizeMatrix.cy)
	   return;

	iTemp = iMatrix[iOldy][iOldX];

	iMatrix[iOldy][iOldX] = iMatrix[iNewY][iNewX];
	iMatrix[iNewY][iNewX] = iTemp;			

    sItem->ptLocation.x = xMatrix;
    sItem->ptLocation.y = yMatrix;
}

BOOL CGuiFavorites::FillMatrix(TypeLaunchItem* sItem, int iIndex, BOOL bPersonal)
{
	if(sItem == NULL)
		return FALSE;

    int **iMatrix = bPersonal ? m_iProgramsMatrix : m_iGamesMatrix;
	// loop through and set the matrix 
	POINT ptItem  = sItem->ptLocation;
	if(ptItem.x >= m_sizeMatrix.cx || ptItem.y >= m_sizeMatrix.cy)
		return FALSE;
	if(ptItem.x < 0 || ptItem.y < 0)
		return FALSE;

	iMatrix[ptItem.y][ptItem.x] = iIndex;

	return TRUE;

}

BOOL CGuiFavorites::AutoArrangeItems(int** iMatrix, CIssVector<TypeLaunchItem>& arrContact)
{
	int		iCY		= m_sizeMatrix.cy;
	int		iCX		= m_sizeMatrix.cx;
	POINT	ptMatrix;
	BOOL	bSetIcon= FALSE;	// Have we set this icon down properly

	// loop through all the icons in the list
	for(int i=0; i<arrContact.GetSize(); i++)
	{
		TypeLaunchItem* sItem = arrContact[i];
		if(!sItem)
			continue;
		POINT ptItem = sItem->ptLocation;

		// if a certain icon has been set as undefined then we want to find a place to lay it down
		if(ptItem.x == UND && ptItem.y == UND)
		{	
			// reset the flag
			bSetIcon = FALSE;

			// loop through and try and set this icon down
			for(int y=0;y<iCY; y++)
			{
				for(int x=0; x<iCX; x++)
				{
					// if the icon is already set
					if(bSetIcon)
						break;

					ptMatrix.x = x;
					ptMatrix.y = y;
					if(IsSpaceAvailable(iMatrix, ptMatrix))
					{
						// set the new icon down
						DBG_OUT((_T("setting item %d at %d %d"), i,ptMatrix.x, ptMatrix.y));
						SetMatrixValue(iMatrix, ptMatrix, i, 1, 1);

						// also set the primary location in the link array
                        sItem->ptLocation.x = ptMatrix.x;
                        sItem->ptLocation.y = ptMatrix.y;

						bSetIcon = TRUE;
						break;
					}
				}
			}

			// was the icon set properly?
			if(!bSetIcon)
				return FALSE;
		}
	}

	return TRUE;
}

int CGuiFavorites::GetItemIndex(POINT& pt)
{
	int x = pt.x/m_sizeBlock.cx;
	int y = pt.y/m_sizeBlock.cy;

	if(y < 0 || y > m_sizeMatrix.cy)
		return UND;

	if(x < 0 || x > m_sizeMatrix.cx)
		return UND;

	return GetCurMatrix()[y][x];
}

// can we just do a straight swap or ?, if so then swap away
BOOL CGuiFavorites::IsSwapStraight(POINT pt)
{
    // now we alway can do this since our block size is 1x1

	if(m_iHoldingIndex == UND)
		return FALSE;

	// get the new location we want (this will be an index in the matrix array)
	TypeLaunchItem* sMovingItem = GetCurLinkArray()[m_iHoldingIndex];
	if(!sMovingItem)
		return FALSE;

	POINT ptDest = GetMatrixLocation(pt);
	POINT ptSrc = sMovingItem->ptLocation;

	// now do a check to see if this block is valid within our bounds
	if(ptDest.x > m_sizeMatrix.cx || ptDest.y > m_sizeMatrix.cy)
		return FALSE;

    int **iCurMatrix = GetCurMatrix();
	int iDestIndex = iCurMatrix[ptDest.y][ptDest.x];

    BOOL bRet = FALSE;

    // are we landing on an existing item?
    if(iDestIndex == UND)
    {
        // null the old matrix position
        iCurMatrix[ptSrc.y][ptSrc.x] = UND;

        bRet = TRUE;
    }
    else
    {
        // move this existing item to moving item src location
        TypeLaunchItem* sExistingItem = GetCurLinkArray()[iDestIndex];
        if(sExistingItem)
        {
            sExistingItem->ptLocation = ptSrc;
            iCurMatrix[ptSrc.y][ptSrc.x] = iDestIndex;
        }

        bRet = FALSE;
    }

    // now set down the moving item
    sMovingItem->ptLocation = ptDest;
    iCurMatrix[ptDest.y][ptDest.x] = m_iHoldingIndex;

	return bRet;

}

BOOL CGuiFavorites::IsSwapAndFindRoom(POINT pt)
{
	// just in case
	if(m_iHoldingIndex == UND)
		return FALSE;

	// get the new location we want (this will be an index in the matrix array)
	POINT ptMatrix = GetMatrixLocation(pt);

	// set the offset so we're looking at the top left corner or the icon
	// we add one so we don't get any divide errors
	pt.x = pt.x - m_sizeOffset.cx + 1;
	pt.y = pt.y - m_sizeOffset.cy + 1;

	TypeLaunchItem* sItem	= GetCurLinkArray()[m_iHoldingIndex];
	POINT ptItem			= sItem->ptLocation;

	// now do a check to see if this block is valid within our bounds
	if(ptMatrix.x  >= m_sizeMatrix.cx || 
		ptMatrix.y >= m_sizeMatrix.cy)
		return FALSE;

	// check the icon to see if 
	int			iCheckIndex;										// going through and checking all the indexes
	int**		iNewMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);		// if everything works then we will use this one
	CIssVector<TypeLaunchItem> arrNewItems;						// temp Item Array

	// copy the contents of the real array into the temp one
	CopyItemLink(&arrNewItems, &GetCurLinkArray());
    int **iCurMatrix = GetCurMatrix();
	CopyMatrix(iNewMatrix, iCurMatrix, MAX_Array_Height, MAX_Array_Width);

	// set the current location to UNDEFINED so we can try and place other icons there
	if(ptMatrix.x != ptItem.x || ptMatrix.y != ptItem.y)
		SetMatrixValue(iNewMatrix, ptItem, UND, 1, 1);

	TypeLaunchItem*  sCheckItem;
	POINT			 ptCheckItem;

	// check the new location we would like to set the icon down, and if anything is there
	// we remove it so later we can auto arrange the icons
	iCheckIndex = iNewMatrix[/*y +*/ ptMatrix.y][/*x +*/ ptMatrix.x];

			// if we're checking a non active area or if it's the same icon
	if(iCheckIndex == UND)
	{
		//continue;
	}
	else if(iCheckIndex != m_iHoldingIndex)
	{
		sCheckItem = arrNewItems[iCheckIndex];
        if(sCheckItem)
        {
		    ptCheckItem     = sCheckItem->ptLocation;

		    // set the matrix so it doesn't have this icon anymore
		    SetMatrixValue(iNewMatrix, ptCheckItem, UND, 1, 1);

		    // also set the link array to UNDEFINED so we can try and find a place
		    // for it later
		    ptCheckItem.x = UND;
		    ptCheckItem.y = UND;
            sCheckItem->ptLocation = ptCheckItem;
        }
	}

	TypeLaunchItem* sHoldingItem = arrNewItems[m_iHoldingIndex];
	if(!sHoldingItem)
	{
		DestroyLinkArray(arrNewItems);
		DeleteMatrix(iNewMatrix, MAX_Array_Height);
		return FALSE;
	}

	// now set the desired link to the new location
	SetMatrixValue(iNewMatrix,
					ptMatrix,
					m_iHoldingIndex, 1, 1);

	// can we place these new icons elsewhere?
	if(!AutoArrangeItems(iNewMatrix, arrNewItems))
	{
		DestroyLinkArray(arrNewItems);
		DeleteMatrix(iNewMatrix, MAX_Array_Height);
		return FALSE;
	}

	// if we reached here we know that all the icons are within the desired rectangle so
	// we now want to swap the locations
	CopyMatrix(iCurMatrix, iNewMatrix, MAX_Array_Height, MAX_Array_Width);
	CopyItemLink(&GetCurLinkArray(), &arrNewItems);
	DestroyLinkArray(arrNewItems);

	// set the primary location for the desired icon
	ptItem.x	= ptMatrix.x;
	ptItem.y	= ptMatrix.y;
    sItem->ptLocation = ptItem;
	
	return TRUE;
}

// is there room for this item in the provided matrix spot?
BOOL CGuiFavorites::IsSpaceAvailable(int** iMatrix, POINT& ptMatrix)
{
    // if we're trying to set stuff that is out of bounds
	if(ptMatrix.x >= m_sizeMatrix.cx || ptMatrix.y >= m_sizeMatrix.cy)
		return FALSE;

	if(iMatrix[ptMatrix.y][ptMatrix.x] != UND)
		return FALSE;

	// yup
	return TRUE;
}

// set a certain value in the matrix
BOOL CGuiFavorites::SetMatrixValue(int** iMatrix, POINT& ptMatrix, int iValue, int iWidth, int iHeight)
{
	if(ptMatrix.x < 0 || ptMatrix.y < 0 || ptMatrix.x >= MAX_Array_Width || ptMatrix.y >= MAX_Array_Height)
		return FALSE;

	// if we're trying to set stuff that is out of bounds (unless we are setting UND)
	if(iValue != UND && ((ptMatrix.x + iWidth) > m_sizeMatrix.cx || (ptMatrix.y + iHeight) > m_sizeMatrix.cy))
		return FALSE;

	// if we have already set it don't do it twice
	// This was done to resolve a bug so it will still work for all situations
	if(iMatrix[ptMatrix.y][ptMatrix.x] == iValue)
		return FALSE;

	int x,y;
	for(x = 0; x < iWidth; x++)
	{
		for(y = 0; y < iHeight; y++)
		{
			iMatrix[y + ptMatrix.y][x + ptMatrix.x] = iValue;
		}
	}
	return TRUE;
}

void CGuiFavorites::CopyItemLink(CIssVector<TypeLaunchItem>* arrDest, CIssVector<TypeLaunchItem>* arrSrc)
{
	for(int i=0; i<arrSrc->GetSize(); i++)
	{
		TypeLaunchItem* sSrc = (*arrSrc)[i];
		TypeLaunchItem* sDest= (*arrDest)[i];
		if(!sSrc)
			return;
		if(!sDest)
		{
			// create a new one
			sDest = new TypeLaunchItem;
			arrDest->AddElement(sDest);
		}
		// copy over the info
		//sDest->SetBlockSize(&m_sizeBlock);
		
        sDest->ptLocation = sSrc->ptLocation;
	}
}

int CGuiFavorites::FindItemGreaterThan(int iX, int iY, int iXCheck, int iYCheck)
{
	int		iIndexMatch			= UND;
	double  dbClosestDistance	= 500.0; //start with a large one so we can find something
	double	dbCheckDistance		= 0;
	RECT	rcLocation, rcIcon;

	rcLocation.left		= iXCheck;
	rcLocation.top		= iYCheck;
	rcLocation.right	= m_sizeMatrix.cx;
	rcLocation.bottom	= m_sizeMatrix.cy;

	// loop through all the icons and find out if we have an item
	for(int i=0; i < GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem	= GetCurLinkArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->ptLocation;
		rcIcon.left		= ptItem.x;
		rcIcon.top		= ptItem.y;
		rcIcon.right	= rcIcon.left + 1/*sizeItem.cx*/;
		rcIcon.bottom	= rcIcon.top + 1/*sizeItem.cy*/;

		// is this icon worth checking
		if(IsContainedIn(rcLocation, rcIcon))
		{
			// find the distance between the two icons
			dbCheckDistance = sqrt(pow(abs(ptItem.x - iX), 2.0) + pow(abs(ptItem.y - iY),2.0));
			if(dbCheckDistance < dbClosestDistance)
			{
				// we have a match, keep checking
				dbClosestDistance	= dbCheckDistance;
				iIndexMatch			= i;
			}
		}
	}

	return iIndexMatch;
}

int CGuiFavorites::FindItemLessThan(int iX, int iY, int iXCheck, int iYCheck)
{
	int		iIndexMatch			= UND;
	double  dbClosestDistance	= 500.0; //start with a large one so we can find something
	double	dbCheckDistance		= 0;
	RECT	rcLocation, rcIcon;

	rcLocation.left		= 0;
	rcLocation.top		= 0;
	rcLocation.right	= iXCheck;
	rcLocation.bottom	= iYCheck;

	// loop through all the icons and find out if we have an item
	for(int i=0; i < GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem	= GetCurLinkArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->ptLocation;
		rcIcon.left		= ptItem.x;
		rcIcon.top		= ptItem.y;
		rcIcon.right	= rcIcon.left + 1/*sizeItem.cx*/;
		rcIcon.bottom	= rcIcon.top + 1/*sizeItem.cy*/;

		// is this icon worth checking
		if(IsContainedIn(rcLocation, rcIcon))
		{
			// find the distance between the two icons
			dbCheckDistance = sqrt(pow(abs(ptItem.x - iX + 1/*sizeItem.cx*/), 2.0) + pow(abs(ptItem.y - iY + 1/*sizeItem.cy*/),2.0));
			if(dbCheckDistance < dbClosestDistance)
			{
				// we have a match, keep checking
				dbClosestDistance	= dbCheckDistance;
				iIndexMatch			= i;
			}
		}
	}

	return iIndexMatch;
}

int CGuiFavorites::FindLastItem()
{
	// return the index of item that's closest to the button (last row, leftmost)

	if(GetCurLinkArray().GetSize() == 0)
		return -1;

	int iReturnIndex = -1;

	// 1) loop through all the icons and find last row
	int iHighestY = 0;
	for(int i=0; i < GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem	= GetCurLinkArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->ptLocation;
		iHighestY = max(iHighestY, ptItem.y);
	}

	// 2) loop through all the icons and find last item in the last x row
	int iLowestX = m_sizeMatrix.cx;
	for(int i=0; i < GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem = GetCurLinkArray()[i];
		if(!sItem)
			continue;
		POINT ptItem = sItem->ptLocation;
		if(ptItem.y < iHighestY)
			continue;

		// set at least 1 item
		if(iReturnIndex < 0)
			iReturnIndex = i;

		if(ptItem.x < iLowestX)
		{
			iLowestX = ptItem.x;
			iReturnIndex = i;
		}
	}

	return iReturnIndex;
}


BOOL CGuiFavorites::SmartArrangeItems()
{
	int		iCY		= m_sizeMatrix.cy;
	int		iCX		= m_sizeMatrix.cx;
	POINT	ptMatrix;
	BOOL	bSetIcon= FALSE;	// Have we set this icon down properly

	// loop through and make everything undefined
    int **iCurMatrix = GetCurMatrix();
	for(int y=0;y<iCY; y++)
		for(int x=0; x<iCX; x++)
			iCurMatrix[y][x] = UND;

	for(int i=0; i<GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem = GetCurLinkArray()[i];
		if(!sItem)
			continue;
		sItem->ptLocation.x = UND;
        sItem->ptLocation.y = UND;
	}

	// now nothing is placed down on the matrix
	// loop through all the icons in the list
	for(int i=0; i<GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem = GetCurLinkArray()[i];
		if(!sItem)
			continue;

		POINT ptItem		= sItem->ptLocation;
		// if a certain icon has been set as undefined then we want to find a place to lay it down
		if(ptItem.x == UND && ptItem.y == UND /*&& eItem == (EnumContact)iLink*/)
		{	
			// reset the flag
			bSetIcon = FALSE;

			// loop through and try and set this icon down
			for(int y=0; y < iCY; y++)
			{
				for(int x = 0; x < iCX; x++)
				{
					// if the icon is already set
					if(bSetIcon)
						break;

					ptMatrix.x = x;
					ptMatrix.y = y;
					if(IsSpaceAvailable(iCurMatrix, ptMatrix))
					{
						// set the new icon down
						SetMatrixValue(iCurMatrix, ptMatrix, i, 1, 1);

						// also set the primary location in the link array
						sItem->ptLocation = ptMatrix;

						bSetIcon = TRUE;
						break;
					}
				}
			}

			// was the icon set properly?
			if(!bSetIcon)
				break;
		}
	}

	return TRUE;

}

void CGuiFavorites::OnContextMenu(POINT pt)
{
}

BOOL CGuiFavorites::RemoveItem(int iIndex, int iArray /*= -1*/)
{
	// validity checking
	if(iIndex == UND)
		return FALSE;

	TypeLaunchItem* sItem = GetCurLinkArray()[iIndex];
	if(!sItem)
		return FALSE;

	// delete the link now
	delete sItem;
	GetCurLinkArray().RemoveElementAt(iIndex);

	// now decrement all the indexes in the matrix by one
    int ** iCurMatrix = GetCurMatrix();
	int x,y;
	for(x = 0; x < m_sizeMatrix.cx; x++)
	{
		for(y = 0; y < m_sizeMatrix.cy; y++)
		{
			if(iCurMatrix[y][x] == iIndex)
				iCurMatrix[y][x] = UND;
			else if(iCurMatrix[y][x] > iIndex)
				iCurMatrix[y][x] --;
		}
	}

	return TRUE;
}
void CGuiFavorites::AddProgram()
{
    CDlgChoosePrograms dlgAdd;
    dlgAdd.SetPreventDeleteList(TRUE);
    dlgAdd.SetFullScreen(FALSE);
    dlgAdd.AddTodayIcon(TRUE);
    
    dlgAdd.Init(m_gdiMem, m_guiBackground);

    if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
    {
        TypeLaunchItem* sItem = new TypeLaunchItem;
        sItem->szFriendlyName = m_oStr->CreateAndCopy(dlgAdd.GetFriendlyName());
        sItem->szPath = m_oStr->CreateAndCopy(dlgAdd.GetPath());
        sItem->eType = LT_Program;
        sItem->ptLocation = m_ptSelection;
        RECT rc = {m_ptSelection.x, m_ptSelection.y, m_ptSelection.x + m_sizeBlock.cx, m_ptSelection.y + m_sizeBlock.cy};
        InitImage(rc, sItem);

        GetCurLinkArray().AddElement(sItem);

        int **iCurMatrix = GetCurMatrix();
        int iNewIndex = GetCurLinkArray().GetSize()-1;

        SetMatrixValue(iCurMatrix, m_ptSelection, iNewIndex, 1, 1);
        AutoArrangeItems(iCurMatrix, GetCurLinkArray());
        SaveRegistry(iNewIndex);
        InvalidateRect(m_hWndParent, NULL, FALSE);
    }
}

void CGuiFavorites::AddSetting()
{
    CDlgChooseSettings dlgAdd;
    dlgAdd.SetPreventDeleteList(TRUE);
    dlgAdd.SetFullScreen(FALSE);

    dlgAdd.Init(m_gdiMem, m_guiBackground);

    if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
    {
        TypeLaunchItem* sItem = new TypeLaunchItem;
        sItem->szFriendlyName = m_oStr->CreateAndCopy(dlgAdd.GetFriendlyName());
        sItem->szPath = m_oStr->CreateAndCopy(dlgAdd.GetPath());
        sItem->eType = LT_Setting;
        sItem->ptLocation = m_ptSelection;
        RECT rc = {m_ptSelection.x, m_ptSelection.y, m_ptSelection.x + m_sizeBlock.cx, m_ptSelection.y + m_sizeBlock.cy};
        InitImage(rc, sItem);

        GetCurLinkArray().AddElement(sItem);

        int **iCurMatrix = GetCurMatrix();
        int iNewIndex = GetCurLinkArray().GetSize()-1;

        SetMatrixValue(iCurMatrix, m_ptSelection, iNewIndex, 1, 1);
        AutoArrangeItems(iCurMatrix, GetCurLinkArray());
        SaveRegistry(iNewIndex);
        InvalidateRect(m_hWndParent, NULL, FALSE);
    }
}

void CGuiFavorites::AddFileFolder()
{

    OPENFILENAME sOpenFileName;
    TCHAR szFile[256];

    m_oStr->Empty(szFile);

    LPCTSTR lpcstrFilter = 
        _T("All Files (*.*)\0*.*\0")
        _T("");

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWndParent; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= NULL;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them

    BOOL bOpen = FALSE;

    CDlgGetFile	dlgGetFile;

    //open the file chooser dialog
    bOpen = dlgGetFile.GetOpenFile(&sOpenFileName);
    if(bOpen && m_oStr->GetLength(szFile) > 0)//only save it if they don't cancel
    {
        TCHAR szFriendlyName[STRING_MAX];
        m_oStr->StringCopy(szFriendlyName, szFile);
        DWORD dwFileAttributes = GetFileAttributes(szFile);
        if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            //m_oStr->StringCopy(szFriendlyName, dlgGetFile.GetCurrentPath());
            //m_oStr->StringCopy(szFile, dlgGetFile.GetCurrentPath());
            PathStripPath(szFriendlyName);
            //PathRemoveArgs(szFriendlyName);
            //PathRemoveExtension(szFriendlyName);
        }
        else
        {
            PathStripPath(szFriendlyName);
            PathRemoveArgs(szFriendlyName);
            PathRemoveExtension(szFriendlyName);
        }
        TypeLaunchItem* sItem = new TypeLaunchItem;
        sItem->szFriendlyName = m_oStr->CreateAndCopy(szFriendlyName);
        sItem->szPath = m_oStr->CreateAndCopy(szFile);
        sItem->eType = LT_FileOrFolder;
        sItem->ptLocation = m_ptSelection;
        RECT rc = {m_ptSelection.x, m_ptSelection.y, m_ptSelection.x + m_sizeBlock.cx, m_ptSelection.y + m_sizeBlock.cy};
        InitImage(rc, sItem);

        GetCurLinkArray().AddElement(sItem);

        int **iCurMatrix = GetCurMatrix();
        int iNewIndex = GetCurLinkArray().GetSize()-1;

        SetMatrixValue(iCurMatrix, m_ptSelection, iNewIndex, 1, 1);
        AutoArrangeItems(iCurMatrix, GetCurLinkArray());
        SaveRegistry(iNewIndex);
        InvalidateRect(m_hWndParent, NULL, FALSE);
    }

    int i=0;


}

void CGuiFavorites::LaunchItem()
{
    //suppose its time
    TypeLaunchItem* sItem = GetCurLinkArray()[GetSelected()];
    
    if(sItem == NULL)
        return;

    //SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
    if(m_sOptions->bHideOnAppLaunch)
        ShowWindow(m_hWndParent, SW_HIDE);
    sItem->Launch(m_hWndParent);

/*    if(sItem->eType == LT_Program)
    {
        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	= sizeof(sei);
        sei.hwnd	= m_hWndParent;
        sei.nShow	= SW_SHOWNORMAL;
        sei.lpFile  = sItem->szPath;

         // give back taskbar so user can close the outlook window
        SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

        ShellExecuteEx(&sei);

    }
    else
    {
        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	= sizeof(sei);
        sei.hwnd	= m_hWndParent;
        sei.nShow	= SW_SHOWNORMAL;
        sei.lpFile  = _T("\\Windows\\ctlpnl.exe");
        sei.lpParameters = sItem->szPath;

        // give back taskbar so user can close the outlook window
        SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

        ShellExecuteEx(&sei);
    }*/
}

void CGuiFavorites::CloseWindow()
{
	EndDialog(m_hWndParent, IDOK);
}

int** CGuiFavorites::CreateMatrix(int iHeight, int iWidth)
{
    DBG_OUT((_T("CGuiFavorites::CreateMatrix - iHeight:%d, iWidth:%d"), iHeight, iWidth));
    iWidth = min(MAX_Array_Width, iWidth);
    iHeight= min(MAX_Array_Height, iHeight);

	int** iMatrix = new int*[iHeight];
    if(!iMatrix)
        return NULL;

	for (int i = 0; i < iWidth; i++)
		iMatrix [i] = new int[iHeight];

	for(int i=0; i<iHeight; i++)
		for(int j=0; j<iWidth; j++)
			iMatrix[i][j]	= UND;

	return iMatrix;
}

void CGuiFavorites::DeleteMatrix(int** iMatrix, int iHeight)
{
	if(!iMatrix)
		return;

	for (int i = 0; i < iHeight; i++)
		delete [] iMatrix[i];
	delete [] iMatrix;
	iMatrix = NULL;
}
    
void CGuiFavorites::ClearMatrix(int** iMatrix)
{
    try
    {
         for(int x = 0; x < m_sizeMatrix.cx; x++)
        {
            for(int y = 0; y < m_sizeMatrix.cy; y++)
            {
                iMatrix[y][x] = UND;
            }
        }
   }
    catch(...)
    {
        ASSERT(FALSE);
    }
}

void CGuiFavorites::CopyMatrix(int** iMatrixDest, int** iMatrixSrc, int iHeight, int iWidth)
{
	if(!iMatrixDest || !iMatrixSrc)
		return;

	for(int i=0; i<iHeight; i++)
		for(int j=0; j<iWidth; j++)
			iMatrixDest[i][j]	= iMatrixSrc[i][j];
}

void CGuiFavorites::fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CGuiFavorites* lpThis = (CGuiFavorites*)lpClass;
	lpThis->AnimateIn(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CGuiFavorites::AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{
	DrawBackground(gdiDest.GetDC(), rcClient, rcClient);

	if(GetCurLinkArray().GetSize() == 0)
		return;

	POINT ptStart = {rcClient.right/2, 0};
	POINT ptCurrent;
	RECT rcEnd;
	for(int i=0; i<GetCurLinkArray().GetSize(); i++)
	{
		TypeLaunchItem* sItem = GetCurLinkArray()[i];
		if(sItem)
		{
			rcEnd = GetItemRect(sItem->ptLocation);//sItem->GetRect();
			ptCurrent.x	 = ptStart.x + (rcEnd.left - ptStart.x)*iAnimeStep/iAnimTotal;
			ptCurrent.y	 = ptStart.y + (rcEnd.top - ptStart.y)*iAnimeStep/iAnimTotal;
            DrawItem(sItem, gdiDest.GetDC(), rcClient, sItem->gdiImage, m_hFont, FALSE, &ptCurrent, m_sOptions->bAlwaysShowText || sItem->eType == LT_FileOrFolder);
		}
	}
}


void CGuiFavorites::InitImage(RECT& rcFrame, TypeLaunchItem* sItem)
{
    int iWidth = WIDTH(rcFrame);
    int iHeight= HEIGHT(rcFrame);

    if(sItem == NULL)
        return;

    if(sItem->gdiImage.GetDC() == NULL || iWidth != sItem->gdiImage.GetWidth() || iHeight != sItem->gdiImage.GetHeight())
    {
        CIssGDIEx gdiImage;
        
		if(!m_imgMask.IsLoaded())
            m_imgMask.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);

        if(!m_imgBorder.IsLoaded())
            m_imgBorder.Initialize(m_hWndParent, m_hInst, IsVGA()?IDR_PNG_ProgBorderVGA:IDR_PNG_ProgBorder);

        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);

        UINT uiTemp = IsVGA()?IDR_PNG_ShineVGA:IDR_PNG_Shine;

        if(GetSystemMetrics(SM_CXICON) > 42 && GetSystemMetrics(SM_CXICON) < 46)
            uiTemp = IDR_PNG_Shine128;
        
        if(m_gdiShine.GetDC() == NULL)
            m_gdiShine.LoadImage(uiTemp, m_hWndParent, m_hInst, TRUE);

        uiTemp = IsVGA()?IDR_PNG_DropBackVGA:IDR_PNG_DropBack;

        if(GetSystemMetrics(SM_CXICON) > 42 && GetSystemMetrics(SM_CXICON) < 46)
            uiTemp = IDR_PNG_DropBack128;

        if(m_gdiBgTemp.GetDC() == NULL)
            m_gdiBgTemp.LoadImage(uiTemp, m_hWndParent, m_hInst, TRUE);

        gdiImage.Create(m_gdiBgTemp, m_gdiBgTemp.GetWidth(), m_gdiBgTemp.GetHeight(), TRUE, TRUE);

        BOOL bDrawShine = TRUE;

        if(sItem->eType == LT_Program)
        {
            if(0 == m_oStr->Compare(m_oStr->GetText(IDS_Today), sItem->szPath))
            {
                CIssGDIEx gdiToday;
                gdiToday.LoadImage(IsVGA()?IDR_PNG_TodayVGA:IDR_PNG_Today, m_hWndParent, m_hInst, TRUE);

                SIZE sz;
                sz.cx   = m_gdiBgTemp.GetWidth();
                sz.cy   = m_gdiBgTemp.GetHeight();

                ScaleImage(gdiToday, gdiImage, sz, TRUE, TRANSPARENT_COLOR);
                bDrawShine = FALSE;
            }
            else
            {
                SHFILEINFO sfi = {0};
                HIMAGELIST hImg = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
                    SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

                ZeroMemory(&sfi, sizeof(SHFILEINFO));
                int iIndex = 0;
                // get the icon index
                if (SHGetFileInfo(sItem->szPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
                    iIndex = sfi.iIcon;

                // draw the icon centered 
                ImageList_Draw( hImg, 
                    iIndex, gdiImage, 
                    (gdiImage.GetWidth()-GetSystemMetrics(SM_CXICON))/2, 
                    (gdiImage.GetHeight()-GetSystemMetrics(SM_CYICON))/2,
                    ILD_TRANSPARENT);
            }
        }
        else if(sItem->eType == LT_FileOrFolder)
        {
            SHFILEINFO sfi = {0};
            HIMAGELIST hImg = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
                SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

            ZeroMemory(&sfi, sizeof(SHFILEINFO));
            int iIndex = 0;
            // get the icon index
            if (SHGetFileInfo(sItem->szPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
                iIndex = sfi.iIcon;

            // draw the icon centered 
            ImageList_Draw( hImg, 
                iIndex, gdiImage, 
                (gdiImage.GetWidth()-GetSystemMetrics(SM_CXICON))/2, 
                (gdiImage.GetHeight()-GetSystemMetrics(SM_CYICON))/2,
                ILD_TRANSPARENT);
        }
        else
        {
            TCHAR szPath[STRING_MAX];
            TCHAR szItem[STRING_LARGE];
            ZeroMemory(szPath, sizeof(TCHAR)*STRING_MAX);
            ZeroMemory(szItem, sizeof(TCHAR)*STRING_LARGE);

            int iIndex = m_oStr->Find(sItem->szPath, _T(","));
            m_oStr->StringCopy(szPath, sItem->szPath, 0, iIndex);
            m_oStr->StringCopy(szItem, sItem->szPath, iIndex+1, m_oStr->GetLength(sItem->szPath)-iIndex-1);
            iIndex = m_oStr->StringToInt(szItem);

            //Got a control panel applet.
            HINSTANCE hCPL = LoadLibrary(szPath);
            if(hCPL)
            {
                CPLAPPLET pCPLApplet = (CPLAPPLET)GetProcAddress(hCPL, _T("CPlApplet"));
                if(pCPLApplet)
                {
                    pCPLApplet(m_hWndParent, CPL_INIT, 0, 0);
                    //Find the name of this CPL applet.
                    NEWCPLINFO CplInfo = {0};
                    CplInfo.dwSize = sizeof(NEWCPLINFO);

                    if(pCPLApplet(m_hWndParent, CPL_NEWINQUIRE, iIndex, (LPARAM)&CplInfo) == 0)
                    {
                        // draw the icon centered 
                        DrawIcon(gdiImage,
                            (m_gdiBgTemp.GetWidth()-GetSystemMetrics(SM_CXICON))/2, 
                            (m_gdiBgTemp.GetHeight()-GetSystemMetrics(SM_CYICON))/2,
                            CplInfo.hIcon);
                    }
                    pCPLApplet(m_hWndParent, CPL_EXIT, 0, 0);
                }
                FreeLibrary(hCPL);
            }
        }

        // draw the shine on top
        if(bDrawShine)
            ::Draw(gdiImage,
                0,0,
                m_gdiShine.GetWidth(), m_gdiShine.GetHeight(),
                m_gdiShine,
                0,0);

        SIZE sz;
        sz.cx   = iWidth;
        sz.cy   = iHeight;
        ScaleImage(gdiImage, sItem->gdiImage, sz, FALSE, 0);

        // add the roundness and border
        sItem->gdiImage.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(sItem->gdiImage, 0,0, ALPHA_AddValue);
    }
}

BOOL CGuiFavorites::DrawItem(TypeLaunchItem* sItem, HDC dc, RECT rcDest, CIssGDIEx& gdiImage, HFONT hFont, BOOL bDrawZeroBase /*= FALSE*/, POINT* ptOverride /*= NULL*/, BOOL bAlwaysText /* = FALSE*/)
{
    // changing the way this works...
    // all drawing is now 0-based
    // rcDest is the size of dest gdi

    if(sItem == NULL)
        return FALSE;

    CIssRect rc(rcDest);

    // if we draw to an override location
    if(ptOverride)
    {
        int iWidth	= rc.GetWidth();
        int iHeight	= rc.GetHeight();
        rc.left		= ptOverride->x;
        rc.top		= ptOverride->y;
        rc.right	= rc.left + iWidth;
        rc.bottom	= rc.top + iHeight;
    }

    if(bDrawZeroBase)
        rc.ZeroBase();

    if(gdiImage)
    {
        int iSize = max(m_sizeBlock.cx, m_sizeBlock.cy);


        BitBlt(dc,
            rc.left, rc.top,
            rc.GetWidth(), rc.GetHeight(),
            gdiImage.GetDC(),
            (gdiImage.GetWidth() - rc.GetWidth())/2, (gdiImage.GetHeight() - rc.GetHeight())/2,
            SRCCOPY);
    }
    else
    {
        return FALSE;
    }

    if(gdiImage == NULL || bAlwaysText)
    {
        TCHAR szName[STRING_MAX] = _T("");
        m_oStr->StringCopy(szName, sItem->szFriendlyName);

        RECT rcText = rc.Get();

        rcText = rc.Get();

        RECT rcTemp = rcText;
        DrawText(dc, sItem->szFriendlyName, rcTemp, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, hFont);

        rcText.top = rcText.bottom - HEIGHT(rcTemp) - INDENT;
        DrawItemText(dc, sItem->szFriendlyName, hFont, rcText, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
    }

    return TRUE;
}

void CGuiFavorites::DrawItemText(HDC dc, TCHAR* szText, HFONT hFont, RECT& rcLocation, UINT uiAlignment /*= DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS*/)
{
    static int iOffset = GetSystemMetrics(SM_CXICON)/32;//nice and tight

    RECT rcTemp = rcLocation;
    OffsetRect(&rcTemp, iOffset,iOffset);
    DrawText(dc, szText, rcTemp, uiAlignment, hFont, RGB(50,50,50));
    OffsetRect(&rcTemp, -iOffset,-iOffset);
    DrawText(dc, szText, rcTemp, uiAlignment, hFont, RGB(255,255,255));
}

void CGuiFavorites::SetIconSize(EnumFavSize eFavSize)
{
    RECT rc = m_rcLocation;

    SetRect(&m_rcLocation, 0, 0, 0, 0);

    //and we need to reset the matrix and the size ....
    MoveGui(rc);
    //AutoArrangeItems(m_iProgramsMatrix, m_arrPrograms);
    //AutoArrangeItems(m_iGamesMatrix, m_arrGames);
}
