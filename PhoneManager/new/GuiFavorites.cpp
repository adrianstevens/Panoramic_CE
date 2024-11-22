#include "StdAfx.h"
#include "GuiFavorites.h"
#include "IssRegistry.h"
#include "IssRect.h"
#include "IssCommon.h"
#include "DlgAddContact.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "DlgContactDetails.h"
#include "ContactsGuiDefines.h"
#include "IssGDIDraw.h"
#include "GuiToolbar.h"
#include "IssWndTouchMenu.h"
#include "ContactsUtil.h"
#include "DlgChooseContact.h"
#include "issdebug.h"
#include <phone.h>
#include "DlgPerContactBase.h"

#define UND_LOCATION                POINT(0,0)


extern void DebugContacts(TCHAR* szFunction, BOOL bStart);


// struct for saving item to registry
struct TypeItemContact
{
	LONG		lOid;
	POINT		ptLocation;
};



CGuiFavorites::CGuiFavorites(void)
: m_iHoldingIndex(UND)
, m_bLockItems(FALSE)
, m_bContextMenu(FALSE)
, m_iPersonalMatrix(NULL)
, m_iBusinessMatrix(NULL)
, m_bMouseDown(FALSE)
, m_bSliderDrag(FALSE)
, m_ePage(0)
, m_eBtnPressState(PS_noPress)
{
	m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
	m_hLabelFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
	m_hLabelFontRot = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE, _T("Tahoma"), 270);

    SetSelection(UND, UND);
}

CGuiFavorites::~CGuiFavorites(void)
{
	SaveRegistry();
	Destroy();
	DeleteMatrix(m_iPersonalMatrix, MAX_Array_Height);
	m_iPersonalMatrix = NULL;
	DeleteMatrix(m_iBusinessMatrix, MAX_Array_Height);
	m_iBusinessMatrix = NULL;
	CIssGDIEx::DeleteFont(m_hFont);
	CIssGDIEx::DeleteFont(m_hLabelFont);
	CIssGDIEx::DeleteFont(m_hLabelFontRot);
}

BOOL CGuiFavorites::Destroy()
{
	for(int i=0; i<MAX_Array_Height; i++)
		for(int j=0; j<MAX_Array_Width; j++)
			m_iPersonalMatrix[i][j]	= UND;

	for(int i=0; i<MAX_Array_Height; i++)
		for(int j=0; j<MAX_Array_Width; j++)
			m_iBusinessMatrix[i][j]	= UND;

	DestroyContactArray(m_arrPersonal);
	DestroyContactArray(m_arrBusiness);
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

void CGuiFavorites::DestroyContactArray(CIssVector<CObjItemContact>& arrContacts)
{
	for(int i=0; i<arrContacts.GetSize(); i++)
	{
		CObjItemContact* objContact = arrContacts[i];
		if(objContact)
			delete objContact;
	}
	arrContacts.RemoveAll();
}

BOOL CGuiFavorites::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    m_dlgContactDetails = dlgContactDetails;

    PreloadImages(hWndParent, m_hInst);
    
    m_oStr->StringCopy(m_szSliderText[0], IDS_SLIDER_Personal,  STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(m_szSliderText[1], IDS_SLIDER_Business,  STRING_NORMAL, m_hInst);

    return TRUE;
}

BOOL CGuiFavorites::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE) 
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
    int iCount = GetCurContactArray().GetSize();
    CObjItemContact* pContact = NULL;

    if(HasSelection())
    {
        int iIndex = GetSelected();
        pContact = GetCurContactArray()[iIndex];
    }
	//m_wndMenu->AddItem(IDS_MENU_AddItem,  m_hInst,   IDMENU_AddItem);
    m_wndMenu->AddItem(IDS_MENU_RemoveItem, m_hInst, IDMENU_RemoveItem, iCount&&pContact?NULL:FLAG_Grayed);
	//m_wndMenu->AddSeparator();

	//m_wndMenu->AddItem(IDS_MENU_EditContact, m_hInst, IDMENU_EditContact);
	//m_wndMenu->AddItem(IDS_MENU_NewContact,  m_hInst, IDMENU_NewContact);
	m_wndMenu->AddSeparator();

    m_wndMenu->AddItem(IDS_MENU_LockItems,  m_hInst,  IDMENU_LockItems, m_bLockItems?FLAG_Check:0);

	return TRUE;
}
    
BOOL CGuiFavorites::LaunchPhoneMenu(TypeContact* pInfo)
{
    if(!pInfo)
        return FALSE;

	m_wndMenu->ResetContent();

    if(m_oStr->GetLength(pInfo->szHomeNumber))
	    m_wndMenu->AddItem(pInfo->szHomeNumber,     IDMENU_CallHome, 0);
    if(m_oStr->GetLength(pInfo->szMobileNumber))
	    m_wndMenu->AddItem(pInfo->szMobileNumber,  IDMENU_CallMobile, 0);
    if(m_oStr->GetLength(pInfo->szWorkNumber))
	    m_wndMenu->AddItem(pInfo->szMobileNumber, IDMENU_CallWork, 0);

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

	m_wndMenu->PopupMenu(m_hWndParent, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
		10,HEIGHT(m_rcLocation)/3,WIDTH(m_rcLocation)*2/3,m_rcLocation.bottom - 10,
		m_rcLocation.left, m_rcLocation.bottom - 10, 10,10, ADJUST_Bottom);

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
            rcText.right += IsVGA() ? 70 : 35;
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0);
            OffsetRect(&rcText, 1,-1);
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[0], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, COLOR_FAVORITES_BG_TEXT);

            rcText.bottom += 6;
            rcText.top = rcText.bottom - (IsVGA() ? 100:50);
            if(IsVGA())
                rcText.right += 1;
            OffsetRect(&rcText, -1,1);
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, 0);
            OffsetRect(&rcText, 1,-1);
            DrawText(m_gdiSliderBg.GetDC(), m_szSliderText[1], rcText, DT_RIGHT | DT_TOP, m_hLabelFontRot, COLOR_FAVORITES_BG_TEXT);
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
	int iBlockSizeMin	= (iMinLength - (FAVORITES_GRID_BORDER*2)) / 4;	

    // this is getting a bit sketchy because our grid is getting very square
    BOOL bMinDimensionIsWidth = WIDTH(m_rcGrid) < HEIGHT(m_rcGrid);

	if(IsLandscape())
	{
		m_sizeMatrix.cy	= 4;
		m_sizeMatrix.cx	= (long)((float)(4*WIDTH(m_rcGrid))/(float)(HEIGHT(m_rcGrid)) + 0.5f);// let's round it
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
		m_sizeMatrix.cx	= 4;
		m_sizeMatrix.cy	= (long)((float)(4*HEIGHT(m_rcGrid))/(float)WIDTH(m_rcGrid) + 0.5f);// let's round it
        if(bMinDimensionIsWidth)
        {
		    m_sizeBlock.cx	= iBlockSizeMin;
		    m_sizeBlock.cy	= (HEIGHT(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cy;
        }
        else
        {
		    //m_sizeBlock.cy	= iBlockSizeMin;    // asumes that m_sizeMatrix.cy == 4
		    m_sizeBlock.cy	= iBlockSizeMin * 4 / m_sizeMatrix.cy;    
		    m_sizeBlock.cx	= (WIDTH(m_rcGrid) - (FAVORITES_GRID_BORDER*2)) / m_sizeMatrix.cx;
        }
	}

	// we'll initialize the matrices
	if(!m_iPersonalMatrix)
		m_iPersonalMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	if(!m_iBusinessMatrix)
		m_iBusinessMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);
	
    ReadRegistry();

	SetMatrixOrientation(WIDTH(m_rcGrid) > HEIGHT(m_rcGrid));

    // re-create all the images if they are defined
    for(int i=0; i<m_arrPersonal.GetSize(); i++)
    {
	    CObjItemContact* objItem = m_arrPersonal[i];
	    if(objItem)
		    objItem->UpdateImage();
    }
    
    for(int i=0; i<m_arrBusiness.GetSize(); i++)
    {
	    CObjItemContact* objItem = m_arrBusiness[i];
	    if(objItem)
		    objItem->UpdateImage();
    }

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
			iNewMatrix[x][y] = m_iPersonalMatrix[y][x];
		}
	}

	// copy it back to the original matrix
	for(int y=0; y<MAX_Array_Height; y++)
	{
		for(int x=0; x<MAX_Array_Width; x++)
		{
			m_iPersonalMatrix[y][x] = iNewMatrix[y][x];

			if(m_iPersonalMatrix[y][x] != UND)
			{
				POINT ptLocation = {x,y};
				CObjItemContact* objItem = m_arrPersonal[m_iPersonalMatrix[y][x]];
				if(objItem)
				{
					objItem->SetItemLocation(ptLocation);
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
			iNewMatrix[x][y] = m_iBusinessMatrix[y][x];
		}
	}

	// copy it back to the original matrix
	for(int y=0; y<MAX_Array_Height; y++)
	{
		for(int x=0; x<MAX_Array_Width; x++)
		{
			m_iBusinessMatrix[y][x] = iNewMatrix[y][x];

			if(m_iBusinessMatrix[y][x] != UND)
			{
				POINT ptLocation = {x,y};
				CObjItemContact* objItem = m_arrBusiness[m_iBusinessMatrix[y][x]];
				if(objItem)
				{
					objItem->SetItemLocation(ptLocation);
				}
			}
		}
	}	

	DeleteMatrix(iNewMatrix, MAX_Array_Height);
}

CIssVector<CObjItemContact> &CGuiFavorites::GetCurContactArray()
{
    if(m_ePage == ePersonalPage)
        return m_arrPersonal;
    else
        return m_arrBusiness;
}
    
CIssVector<CObjItemContact> &CGuiFavorites::GetContactArray(int iWhich)
{
    if(iWhich == ePersonalPage)
        return m_arrPersonal;
    else
        return m_arrBusiness;
}

void CGuiFavorites::ReadRegistry()
{
	HKEY hKey;
	DWORD dwType	= 0;
	DWORD dwIndex	= 0;
	DWORD dwBufSize = 0;
	DWORD dwStringSize = 0;
	TypeItemContact sItem;
	TCHAR szItemName[STRING_MAX];

    /////////////////////////////////////////////////////////////////////
    // Personal 
    // clear array
    DestroyContactArray(m_arrPersonal);
    ClearMatrix(m_iPersonalMatrix);
	//  Make sure that the destination doesn't already exist.
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_Save_LocationPersonal,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) 
		return;

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

		CObjItemContact* sNewItem = new CObjItemContact;
		if(!sNewItem->Initialize(/*sItem.eContact, */sItem.lOid, sItem.ptLocation, &m_sizeBlock))
		{
			delete sNewItem;
			break;
		}
		m_arrPersonal.AddElement(sNewItem);
		FillMatrix(sNewItem, m_arrPersonal.GetSize()-1, TRUE);
	}

	RegCloseKey(hKey);

    dwType	= 0;
	dwIndex	= 0;
	dwBufSize = 0;
	dwStringSize = 0;

    /////////////////////////////////////////////////////////////////////
    // Business 
    // clear array
    DestroyContactArray(m_arrBusiness);
    ClearMatrix(m_iBusinessMatrix);

	//  Make sure that the destination doesn't already exist.
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_Save_LocationBusiness,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) 
		return;

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

		CObjItemContact* sNewItem = new CObjItemContact;
		if(!sNewItem->Initialize(sItem.lOid, sItem.ptLocation, &m_sizeBlock))
		{
			delete sNewItem;
			break;
		}
		m_arrBusiness.AddElement(sNewItem);
		FillMatrix(sNewItem, m_arrBusiness.GetSize()-1, FALSE);
	}

	RegCloseKey(hKey);

}

HRESULT CGuiFavorites::SaveRegistry(int iItem /*= UND*/)
{
    HRESULT hr;

    // save the page that we are on 
    TCHAR* szSaveLocation = (m_ePage == ePersonalPage) ? REG_Save_LocationPersonal : REG_Save_LocationBusiness;

	// delete it all first
	hr = RegDeleteKeyNT(HKEY_CURRENT_USER, szSaveLocation);
    CHR(hr, _T("RegDeleteKeyNT failed"));

	HKEY hKey;
	RegCreateKeyEx(HKEY_CURRENT_USER, szSaveLocation, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    CHR(hr, _T("RegCreateKeyEx failed"));

	TCHAR szItem[STRING_MAX];
	TypeItemContact sItemContact;
	for(int i=0; i<GetCurContactArray().GetSize(); i++)
	{
		// this is if we want to save just one item
		if(iItem != UND && iItem != i)
			continue;

        CObjItemContact* sItem = GetCurContactArray()[i];
		if(sItem)
		{
			sItemContact.lOid		= sItem->GetOid();
			sItemContact.ptLocation = sItem->GetLocation();
			m_oStr->Format(szItem, _T("c%X"), sItemContact.lOid);

			// save this item to the registry
			hr = RegSetValueEx(hKey, szItem, 0,REG_BINARY, (LPBYTE)&sItemContact, sizeof(TypeItemContact));
            CHR(hr, _T("RegSetValueEx failed"));
		}
	}

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
	DrawItems(gdiTemp, rc1, rc1, UND, eBusinessPage);
	DrawItems(gdiTemp, rc2, rc2, UND, ePersonalPage);
	DrawSelected(gdiTemp, rc1, rc1);
	DrawSelected(gdiTemp, rc2, rc2);


    DWORD dwStart = GetTickCount();
    BOOL bDec = (m_ePage == eBusinessPage);

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
        h = (HEIGHT(m_rcLocation) - 14) * 2 / 5;
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
  
CIssGDIEx& CGuiFavorites::GetPlaceHolderImg()
{
    if(!m_gdiPlaceholder.GetDC())
    {
        int w, h;
        GetItemSize(m_rcGrid, w, h);
        RECT rcItem = {0,0,w,h};

        UINT uiRes = IsVGA() ? IDR_PNG_FavoritesImgDefaultVGA : IDR_PNG_FavoritesImgDefault;
        CIssGDIEx gdiTemp;
        gdiTemp.Create(m_gdiMem->GetDC(), rcItem, FALSE, TRUE);
        gdiTemp.LoadImage(uiRes, m_hWndParent, m_hInst);

        int iImageW = gdiTemp.GetWidth();
        int iImageH = gdiTemp.GetHeight();

        //ScaleImage
        BOOL bNeedsScaling = (gdiTemp.GetWidth() > w*1.25) || (gdiTemp.GetHeight() > h*1.25);
        if(bNeedsScaling)
        {
            // maintain aspect ratio
            SIZE s;
            if(h > w)
            {
                s.cy = h;
                s.cx = (long)((float)h / (float)iImageH * iImageW);
            }
            else
            {
                s.cx = w;
                s.cx = (long)((float)w / (float)iImageW * iImageH);
            }
            m_gdiPlaceholder.Create(m_gdiMem->GetDC(), rcItem, FALSE, TRUE);
            ScaleImage(gdiTemp, m_gdiPlaceholder, s, FALSE, 0);
        }
        else
        {
            // load it straight in
            m_gdiPlaceholder.LoadImage(uiRes, m_hWndParent, m_hInst);
        }
    }

    return m_gdiPlaceholder;
}

CIssGDIEx&  CGuiFavorites::GetMovingNameImage()
{
    // make sure we have an item
	CObjItemContact* sItem = GetCurContactArray()[m_iHoldingIndex];

    TCHAR szName[STRING_LARGE];
    m_oStr->Empty(szName);

    if(sItem)
    {
        TypeContact* sContactInfo = sItem->GetContactInfo();
           if(sContactInfo)
           {
               m_oStr->Concatenate(szName, sContactInfo->szFirstName);
               m_oStr->Concatenate(szName, _T(" "));
               m_oStr->Concatenate(szName, sContactInfo->szLastName);
           }
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
    for(int i = 0; i < GetCurContactArray().GetSize(); i++)
    {
        CObjItemContact* pItem = GetCurContactArray()[i];
        if(pItem)
        {
            if(pItem->GetLocation().x == m_ptSelection.x &&
                pItem->GetLocation().y == m_ptSelection.y)
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
            //SetSelection(UND, UND);
            //bResult = FALSE;
            //LoseFocus();
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
            //SetSelection(UND, UND);
            //bResult = FALSE;
            //LoseFocus();
            return FALSE;
        }
	}
	else if(wParam == VK_RETURN)
	{
		if(GetSelected() != UND)
			LaunchItem();
        else
            AddItem();
	}
	else
    {
        //SetSelection(UND, UND);
		//bResult = FALSE;
    }

    if(ptOldSelection.x != m_ptSelection.x || ptOldSelection.y != m_ptSelection.y)
    {
        SetDirty(ptOldSelection, m_ptSelection);
	    InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
    }
    return bResult;
    
}
void MakeCall(TCHAR* szAddress)
{
	PHONEMAKECALLINFO mci = {0};
	mci.cbSize = sizeof(mci);
	mci.dwFlags = 0;
	mci.pszDestAddress = szAddress;
	PhoneMakeCall(&mci);
}

void MakeCall()
{
    HWND hWndPhone = ::FindWindow(0, _T("Phone"));
    if(hWndPhone)
        ::SetForegroundWindow(hWndPhone);
}

BOOL CGuiFavorites::HasFocus()
{
	return HasSelection();
}

void CGuiFavorites::LoseFocus()
{
    //DebugOut(_T("CGuiFavorites::LoseFocus()"));
    SetDirty(m_ptSelection, m_ptSelection);
    SetSelection(UND, UND);
    InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
}


BOOL CGuiFavorites::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_TTALK)
    {
        /*// launch a call, 
        // if contact selected, use number
        // if more than 1 number, lunch a cal menu
        if(GetSelected() == UND)
            MakeCall();
        else
            DialContact();
        */

        // 1-13-2009
        // messagebox: ask if user wants to call selected contact
        // if so, launch details
        // else open windows phone dlg
    }

	return FALSE;
}
    
int CGuiFavorites::CountContactPhoneNumbers(TypeContact* pInfo)
{
    int iCount = 0;
    if(m_oStr->GetLength(pInfo->szHomeNumber))
        iCount++;
    if(m_oStr->GetLength(pInfo->szMobileNumber))
        iCount++;
    if(m_oStr->GetLength(pInfo->szWorkNumber))
        iCount++;

    return iCount;
}
/*
void CGuiFavorites::DialContact()
{
    CObjItemContact* pContact = GetCurContactArray()[GetSelected()];
    if(pContact)
    {
        TypeContact* pInfo = pContact->GetContactInfo();
        if(pInfo)
        {
            if(CountContactPhoneNumbers(pInfo) > 1)
            {
                // set up a menu
                LaunchPhoneMenu(pInfo);
            }
            else if(CountContactPhoneNumbers(pInfo) == 1)
            {
                // dial the first number you find
                if(m_oStr->GetLength(pInfo->szHomeNumber))
                    MakeCall(pInfo->szHomeNumber);
                else if(m_oStr->GetLength(pInfo->szMobileNumber))
                    MakeCall(pInfo->szMobileNumber);
                else if(m_oStr->GetLength(pInfo->szWorkNumber))
                    MakeCall(pInfo->szWorkNumber);
            }
            else
                MakeCall();
        }
    }
}
*/
BOOL CGuiFavorites::OnSliderButtonDown(POINT pt)
{
    if(PtInRect(&GetSliderRect(), pt))
    {
        m_bSliderDrag = TRUE;
        m_iSliderPos = 0;
    }

    return UNHANDLED;
}

BOOL CGuiFavorites::OnSliderButtonUp(POINT pt)
{
    int eOrigPage = m_ePage;
    int eNewPage = eOrigPage;
    if(m_bSliderDrag)
    {
        if(IsLandscape())
        {
            if(GetSliderRect().top - m_rcSlider.top  < m_rcSlider.bottom - GetSliderRect().bottom)
                eNewPage = ePersonalPage;
            else
                eNewPage = eBusinessPage;
        }
        else
        {
            if(GetSliderRect().left - m_rcSlider.left  < m_rcSlider.right - GetSliderRect().right)
                eNewPage = ePersonalPage;
            else
                eNewPage = eBusinessPage;
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
            eNewPage = ePersonalPage;
        else
            eNewPage = eBusinessPage;

        if(eOrigPage != eNewPage)
        {
            POINT ptTemp;

            ptTemp.x = rcTemp.left;
            ptTemp.y = rcTemp.top;

            if(eNewPage == eBusinessPage)
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
		CObjItemContact* sItem = GetCurContactArray()[m_iHoldingIndex];
		if(!sItem)
			return TRUE;
		POINT ptItem = sItem->GetLocation();
        RECT rcItem = GetItemRect(ptItem);
		m_sizeOffset.cx = pt.x - rcItem.left - m_rcGrid.left;
		m_sizeOffset.cy = pt.y - rcItem.top;
		// save the item image that we are moving
        m_gdiMovingItem.Create(m_gdiMem->GetDC(), WIDTH(rcItem), HEIGHT(rcItem));
        m_gdiMovingItem.SetAlphaMask(GetMovingItemMask());
		sItem->DrawItem(m_gdiMovingItem.GetDC(), rcItem, GetPlaceHolderImg(), m_hFont, TRUE);

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
					SaveRegistry();
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
			//ResetSelectedItem();
			InvalidateRect(m_hWndParent, NULL, FALSE);
		}
		break;
	case IDMENU_AddItem:
		AddItem();
		break;
	case IDMENU_EditContact:
        {
             // give back taskbar so user can close the outlook window
		    SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
            CObjItemContact* pContact = GetCurContactArray()[GetSelected()];
            if(pContact)
                CPoomContacts::Instance()->LaunchEditContactScreen(pContact->GetOid());
       }
		break;
	case IDMENU_NewContact:
        {
            // give back taskbar so user can close the outlook window
		    SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
            CPoomContacts::Instance()->CreateNew();
        }
		break;
	case IDMENU_CallHome:
	case IDMENU_CallMobile:
	case IDMENU_CallWork:
        {
            CObjItemContact* pContact = GetCurContactArray()[GetSelected()];
            if(pContact)
            {
                TypeContact* pInfo = pContact->GetContactInfo();
                if(!pInfo)
                    break;
                if(wParam == IDMENU_CallHome)
                    MakeCall(pInfo->szHomeNumber);
                else if(wParam == IDMENU_CallMobile)
                    MakeCall(pInfo->szMobileNumber);
                else if(wParam == IDMENU_CallWork)
                    MakeCall(pInfo->szWorkNumber);
            }
        }
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
    
BOOL CGuiFavorites::HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case PIM_ITEM_CREATED_LOCAL: 
    case PIM_ITEM_CREATED_REMOTE: 
        // don't care
        break;
    case PIM_ITEM_DELETED_LOCAL: 
    case PIM_ITEM_DELETED_REMOTE: 
            PoomItemDeleted(wParam);
        break;
    case PIM_ITEM_CHANGED_LOCAL: 
    case PIM_ITEM_CHANGED_REMOTE: 
            PoomItemChanged(wParam);
        break;
    }
    return TRUE;
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
    if(ptMatrixPos.x > m_sizeMatrix.cx || ptMatrixPos.y > m_sizeMatrix.cy)
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
        int iBtnHeight = iSlideAmount * 2 / 5;
        int iTravel = iSlideAmount - iBtnHeight + 1;
        InflateRect(&rc, -iIndent, -(iIndent));
        rc.top += 1;
        if(IsVGA())
            rc.bottom -= 4;
        if(iPage == ePersonalPage)
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
        if(iPage == ePersonalPage)
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
        if(m_ePage == ePersonalPage)
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
        if(m_ePage == ePersonalPage)
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
        if(m_ePage == ePersonalPage)
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
        if(m_ePage == ePersonalPage)
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
    if(m_ePage == ePersonalPage)
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

    gdiTemp.SetAlphaMask(GetItemAlphaMask().GetImage());

    int **iMatrix = ePage == ePersonalPage ? m_iPersonalMatrix : m_iBusinessMatrix;
    CIssVector<CObjItemContact> &arrContacts = ePage == ePersonalPage ? m_arrPersonal : m_arrBusiness;
	for(int i = 0; i < arrContacts.GetSize(); i++)
	{
		CObjItemContact* sItem = arrContacts[i];
		if(sItem /*&& i != iExcludeItem*/)
		{
            if(i == iExcludeItem && m_eBtnPressState == PS_dragging)
                continue;
            // draw to the temp gdi, then draw it to the caller gdi

            // if it is off the matrix, then set it valid
            // can happen if matrix size changes between portrait and landscape
            if(sItem->GetLocation().x >= m_sizeMatrix.cx || sItem->GetLocation().y >= m_sizeMatrix.cy)
            {
                SetMatrixValue(iMatrix, 
					sItem->GetLocation(), UND, 1, 1);
                sItem->SetItemLocation(UND, UND);
                AutoArrangeItems(iMatrix, arrContacts);
                SaveRegistry();
            }

            RECT rcDraw = GetItemRect(sItem->GetLocation());
            // these offsets are applied when drawing the page slide
            int iYOffset = 0;
            if(!IsLandscape() && rcClient.top == 0)
                iYOffset = -GetSliderHeight();
            OffsetRect(&rcDraw, rcClient.left, iYOffset);

            if(!IsRectInRect(rcDraw, rcClip))
                continue;

			sItem->DrawItem(gdiTemp.GetDC(), rcItem/*rcClient*/, GetPlaceHolderImg(), 
                m_hFont, TRUE);

            ::Draw(gdi, rcDraw, gdiTemp);

            // draw the border
            GetItemBorder().DrawImage(gdi, rcDraw.left,rcDraw.top);
		}
	}
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
	CObjItemContact* sItem = GetCurContactArray()[m_iHoldingIndex];
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

	CObjItemContact* sItem = GetCurContactArray()[iHoldingIndex];
	if(!sItem)
		return;

    int **iMatrix = GetCurMatrix();
	int iNewX, iNewY, iOldX, iOldy;
	int iTemp;

    POINT ptItem	= sItem->GetLocation();
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

	sItem->SetItemLocation(xMatrix, yMatrix);
}

BOOL CGuiFavorites::FillMatrix(CObjItemContact* sItem, int iIndex, BOOL bPersonal)
{
	if(sItem == NULL)
		return FALSE;

    int **iMatrix = bPersonal ? m_iPersonalMatrix : m_iBusinessMatrix;
	// loop through and set the matrix 
	POINT ptItem  = sItem->GetLocation();
	if(ptItem.x >= m_sizeMatrix.cx || ptItem.y >= m_sizeMatrix.cy)
		return FALSE;
	if(ptItem.x < 0 || ptItem.y < 0)
		return FALSE;

	iMatrix[ptItem.y][ptItem.x] = iIndex;

	return TRUE;

}

BOOL CGuiFavorites::AutoArrangeItems(int** iMatrix, CIssVector<CObjItemContact>& arrContact)
{
	int		iCY		= m_sizeMatrix.cy;
	int		iCX		= m_sizeMatrix.cx;
	POINT	ptMatrix;
	BOOL	bSetIcon= FALSE;	// Have we set this icon down properly

	// loop through all the icons in the list
	for(int i=0; i<arrContact.GetSize(); i++)
	{
		CObjItemContact* sItem = arrContact[i];
		if(!sItem)
			continue;
		POINT ptItem = sItem->GetLocation();

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
					if(IsSpaceAvailable(iMatrix, sItem, ptMatrix))
					{
						// set the new icon down
						DBG_OUT((_T("setting item %d at %d %d"), i,ptMatrix.x, ptMatrix.y));
						SetMatrixValue(iMatrix, ptMatrix, i, 1, 1);

						// also set the primary location in the link array
						sItem->SetItemLocation(ptMatrix);

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
	CObjItemContact* sMovingItem = GetCurContactArray()[m_iHoldingIndex];
	if(!sMovingItem)
		return FALSE;

	POINT ptDest = GetMatrixLocation(pt);
	POINT ptSrc = sMovingItem->GetLocation();

	// now do a check to see if this block is valid within our bounds
	if(ptDest.x > m_sizeMatrix.cx || ptDest.y > m_sizeMatrix.cy)
		return FALSE;

    int **iCurMatrix = GetCurMatrix();
	int iDestIndex = iCurMatrix[ptDest.y][ptDest.x];

    // are we landing on an existing item?
    if(iDestIndex == UND)
    {
        // null the old matrix position
        iCurMatrix[ptSrc.y][ptSrc.x] = UND;
    }
    else
    {
        // move this existing item to moving item src location
        CObjItemContact* sExistingItem = GetCurContactArray()[iDestIndex];
        if(sExistingItem)
        {
            sExistingItem->SetItemLocation(ptSrc);
            iCurMatrix[ptSrc.y][ptSrc.x] = iDestIndex;
        }
    }

    // now set down the moving item
    sMovingItem->SetItemLocation(ptDest);
    iCurMatrix[ptDest.y][ptDest.x] = m_iHoldingIndex;

	return TRUE;

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

	CObjItemContact* sItem	= GetCurContactArray()[m_iHoldingIndex];
	POINT ptItem			= sItem->GetLocation();

	// now do a check to see if this block is valid within our bounds
	if(ptMatrix.x  >= m_sizeMatrix.cx || 
		ptMatrix.y >= m_sizeMatrix.cy)
		return FALSE;

	// check the icon to see if 
	int			iCheckIndex;										// going through and checking all the indexes
	int**		iNewMatrix = CreateMatrix(MAX_Array_Height, MAX_Array_Width);		// if everything works then we will use this one
	CIssVector<CObjItemContact> arrNewItems;						// temp Item Array

	// copy the contents of the real array into the temp one
	CopyItemContacts(&arrNewItems, &GetCurContactArray());
    int **iCurMatrix = GetCurMatrix();
	CopyMatrix(iNewMatrix, iCurMatrix, MAX_Array_Height, MAX_Array_Width);

	// set the current location to UNDEFINED so we can try and place other icons there
	if(ptMatrix.x != ptItem.x || ptMatrix.y != ptItem.y)
		SetMatrixValue(iNewMatrix, ptItem, UND, 1, 1);

	CObjItemContact* sCheckItem;
	POINT			 ptCheckItem;

	// check the new location we would like to set the icon down, and if anything is there
	// we remove it so later we can autoarrange the icons
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
		    ptCheckItem		= sCheckItem->GetLocation();

		    // set the matrix so it doesn't have this icon anymore
		    SetMatrixValue(iNewMatrix, ptCheckItem, UND, 1, 1);

		    // also set the link array to UNDEFINED so we can try and find a place
		    // for it later
		    ptCheckItem.x = UND;
		    ptCheckItem.y = UND;
		    sCheckItem->SetItemLocation(ptCheckItem);                
        }
	}

	CObjItemContact* sHoldingItem = arrNewItems[m_iHoldingIndex];
	if(!sHoldingItem)
	{
		DestroyContactArray(arrNewItems);
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
		DestroyContactArray(arrNewItems);
		DeleteMatrix(iNewMatrix, MAX_Array_Height);
		return FALSE;
	}

	// if we reached here we know that all the icons are within the desired rectangle so
	// we now want to swap the locations
	CopyMatrix(iCurMatrix, iNewMatrix, MAX_Array_Height, MAX_Array_Width);
	CopyItemContacts(&GetCurContactArray(), &arrNewItems);
	DestroyContactArray(arrNewItems);

	// set the primary location for the desired icon
	ptItem.x	= ptMatrix.x;
	ptItem.y	= ptMatrix.y;
	sItem->SetItemLocation(ptItem);

	return TRUE;
}

// is there room for this item in the provided matrix spot?
BOOL CGuiFavorites::IsSpaceAvailable(int** iMatrix, CObjItemContact* sItem, POINT& ptMatrix)
{
	if(sItem == NULL)
		return FALSE;

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
	if(ptMatrix.x == UND || ptMatrix.y == UND)
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

void CGuiFavorites::CopyItemContacts(CIssVector<CObjItemContact>* arrDest, CIssVector<CObjItemContact>* arrSrc)
{
	for(int i=0; i<arrSrc->GetSize(); i++)
	{
		CObjItemContact* sSrc = (*arrSrc)[i];
		CObjItemContact* sDest= (*arrDest)[i];
		if(!sSrc)
			return;
		if(!sDest)
		{
			// create a new one
			sDest = new CObjItemContact;
			arrDest->AddElement(sDest);
		}
		// copy over the info
		sDest->SetBlockSize(&m_sizeBlock);
		sDest->SetItemLocation(sSrc->GetLocation());
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
	for(int i=0; i < GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem	= GetCurContactArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->GetLocation();
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
	for(int i=0; i < GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem	= GetCurContactArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->GetLocation();
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

	if(GetCurContactArray().GetSize() == 0)
		return -1;

	int iReturnIndex = -1;

	// 1) loop through all the icons and find last row
	int iHighestY = 0;
	for(int i=0; i < GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem	= GetCurContactArray()[i];
		if(!sItem)
			continue;
		POINT ptItem	= sItem->GetLocation();
		iHighestY = max(iHighestY, ptItem.y);
	}

	// 2) loop through all the icons and find last item in the last x row
	int iLowestX = m_sizeMatrix.cx;
	for(int i=0; i < GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem = GetCurContactArray()[i];
		if(!sItem)
			continue;
		POINT ptItem = sItem->GetLocation();
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

	for(int i=0; i<GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem = GetCurContactArray()[i];
		if(!sItem)
			continue;
		sItem->SetItemLocation(UND, UND);
	}

	// now nothing is placed down on the matrix
	// loop through all the icons in the list
	for(int i=0; i<GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem = GetCurContactArray()[i];
		if(!sItem)
			continue;

		POINT ptItem		= sItem->GetLocation();
		// if a certain icon has been set as undefined then we want to find a place to lay it down
		if(ptItem.x == UND && ptItem.y == UND /*&& eItem == (EnumContact)iLink*/)
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
					if(IsSpaceAvailable(iCurMatrix, sItem, ptMatrix))
					{
						// set the new icon down
						SetMatrixValue(iCurMatrix, ptMatrix, i, 1, 1);

						// also set the primary location in the link array
						sItem->SetItemLocation(ptMatrix);

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
	/*HMENU hMenuContext		= CreatePopupMenu();
	HMENU hMenuChange		= CreatePopupMenu();
	if(!hMenuContext)
		return;

	EnumContact eContact = CONTACT_Undefined;
	CObjItemContact* sItem = m_arrContacts[m_iHoldingIndex];
	if(sItem)
		eContact = sItem->GetContactType();

	int iAllow = GetMaxItems();
	int iHave = m_arrContacts.GetSize();
	BOOL bDisableAdd = m_arrContacts.GetSize() >= GetMaxItems();

	AppendMenu(hMenuChange, MF_STRING|(eContact==CONTACT_Name?MF_CHECKED:NULL), IDMENU_ChangeItem+CONTACT_Name,    _T("Name only"));
	AppendMenu(hMenuChange, MF_STRING|(eContact==CONTACT_Small?MF_CHECKED:NULL), IDMENU_ChangeItem+CONTACT_Small,    _T("Small"));
	AppendMenu(hMenuChange, MF_STRING|(eContact==CONTACT_SmallName?MF_CHECKED:NULL), IDMENU_ChangeItem+CONTACT_SmallName,    _T("Small with name"));
	AppendMenu(hMenuChange, MF_STRING|(eContact==CONTACT_Large?MF_CHECKED:NULL), IDMENU_ChangeItem+CONTACT_Large,    _T("Large"));
	AppendMenu(hMenuChange, MF_STRING|(eContact==CONTACT_LargeName?MF_CHECKED:NULL), IDMENU_ChangeItem+CONTACT_LargeName,    _T("Large with name"));

	AppendMenu(hMenuContext, MF_STRING|(m_iHoldingIndex==UND?MF_GRAYED:NULL), IDMENU_SelectItem,  _T("Select Item"));
	AppendMenu(hMenuContext, MF_STRING|(bDisableAdd?MF_GRAYED:NULL), IDMENU_AddItem,  _T("Add Item"));
	AppendMenu(hMenuContext, MF_STRING|(m_iHoldingIndex==UND?MF_GRAYED:NULL), IDMENU_RemoveItem,  _T("Remove Item"));
	AppendMenu(hMenuContext, MF_STRING|(m_iHoldingIndex==UND?MF_GRAYED:NULL), IDMENU_SelectItem,  _T("Select Item"));
	AppendMenu(hMenuContext, MF_POPUP|(m_iHoldingIndex==UND?MF_GRAYED:NULL), (UINT)hMenuChange, _T("Change Item"));
	AppendMenu(hMenuContext, MF_SEPARATOR, 0,  NULL);
	AppendMenu(hMenuContext, MF_STRING|(m_arrContacts.GetSize()==0?MF_GRAYED:NULL), IDMENU_SmartArrange,  _T("Smart Arrange"));
	AppendMenu(hMenuContext, MF_STRING|(m_bLockItems?MF_CHECKED:NULL), IDMENU_LockItems,  _T("Lock Items"));
	AppendMenu(hMenuContext, MF_SEPARATOR, 0,  NULL);
	AppendMenu(hMenuContext, MF_STRING, IDMENU_Exit,  _T("Cancel"));

	//Display it.
	TrackPopupMenu(	hMenuContext, 
		TPM_LEFTALIGN|TPM_TOPALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWndParent, NULL);

	DestroyMenu(hMenuContext);
	DestroyMenu(hMenuChange);
	return;
    */
}

BOOL CGuiFavorites::ResetSelectedItem()
{
	//m_iSelectedIndex = FindItemGreaterThan(0, 0, 0, 0);
	//DebugOut(_T("ResetSelectedItem() m_iSelectedIndex = %i"), m_iSelectedIndex);
	return TRUE;
};

BOOL CGuiFavorites::RemoveItem(int iIndex, int iArray /*= -1*/)
{
	// validity checking
	if(iIndex == UND)
		return FALSE;

	CObjItemContact* sItem = GetCurContactArray()[iIndex];
	if(!sItem)
		return FALSE;

	// delete the link now
	delete sItem;
	GetCurContactArray().RemoveElementAt(iIndex);

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
/*
BOOL CGuiFavorites::ChangeItem(int iIndex, EnumContact eNewContactType)
{
	CObjItemContact* sItem = GetCurContactArray()[iIndex];
	if(!sItem)
		return FALSE;

	EnumContact eContact = sItem->GetContactType();
	if(eContact == eNewContactType)
		return FALSE;

	POINT ptItem = sItem->GetLocation();
	SIZE sizeItem= sItem->GetSize();

	if(!sItem->SetContactType(eNewContactType))
		return FALSE;

	// take off the old item from the array
    int **iCurMatrix = GetCurMatrix();
	SetMatrixValue(iCurMatrix, 
					ptItem,
					UND,
					sizeItem.cx,
					sizeItem.cy);

	sItem->SetItemLocation(UND, UND);

	for(int x = 0; x < sizeItem.cx; x++)
	{
		for(int y = 0; y < sizeItem.cy; y++)
		{
			if(iCurMatrix[y + ptItem.y][x + ptItem.x] != UND)
				return AutoArrangeItems(iCurMatrix, GetCurContactArray());
		}
	}

	// if we get here then we know that we can place the icon down without moving it
	SetMatrixValue(iCurMatrix, 
					ptItem,
					iIndex,
					sizeItem.cx,
					sizeItem.cy);

	sItem->SetItemLocation(ptItem);
	return TRUE;
}
*/
void CGuiFavorites::AddItem()
{
    /*CDlgPerContactBase dlg;
    dlg.Init(m_gdiMem, m_guiBackground);
    dlg.Launch(m_hWndParent, m_hInst, TRUE);
    return;*/

	CDlgChooseContact dlgAdd(m_hWndParent, &GetCurContactArray());

    dlgAdd.Init(m_gdiMem, m_guiBackground);
	if(IDOK == dlgAdd.Launch(m_hWndParent, m_hInst, FALSE))
	{
		LONG lOID;
		lOID	= dlgAdd.GetSelectedOID();
        if(lOID == UND)
        {
            return;
        }
		CObjItemContact* sItem = new CObjItemContact;
		sItem->Initialize(/*CONTACT_Small, */lOID, m_ptSelection, &m_sizeBlock);
		sItem->UpdateImage();
		GetCurContactArray().AddElement(sItem);

        int **iCurMatrix = GetCurMatrix();
        int iNewIndex = GetCurContactArray().GetSize()-1;
        
        SetMatrixValue(iCurMatrix, m_ptSelection, iNewIndex, 1, 1);
		AutoArrangeItems(iCurMatrix, GetCurContactArray());
        SaveRegistry();
		InvalidateRect(m_hWndParent, NULL, FALSE);
	}
}

void CGuiFavorites::LaunchItem()
{
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;

	CObjItemContact* sItem = GetCurContactArray()[GetSelected()];
	if(!sItem)
		return;

	TypeContact* sConctact = sItem->GetContactInfo();
	if(!sItem)
		return;

#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
	int iIndex = oPoom->GetIndexFromOID(sConctact->lOid);
#else
	int iIndex = oPoom->GetIndexFromOID(sConctact->lOid) - 1;
#endif

	m_dlgContactDetails->SetIndex(iIndex);
    m_dlgContactDetails->Init(m_gdiMem, m_guiBackground);
	int iReturn = m_dlgContactDetails->Launch(m_hWndParent, m_hInst, TRUE);

    if(iReturn == IDOK)
    {
    }
}

void CGuiFavorites::CloseWindow()
{
	EndDialog(m_hWndParent, IDOK);
}

int** CGuiFavorites::CreateMatrix(int iHeight, int iWidth)
{
	int** iMatrix = new int*[iHeight];
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
                iMatrix[x][y] = UND;
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

	if(GetCurContactArray().GetSize() == 0)
		return;

	POINT ptStart = {rcClient.right/2, 0};
	POINT ptCurrent;
	RECT rcEnd;
	for(int i=0; i<GetCurContactArray().GetSize(); i++)
	{
		CObjItemContact* sItem = GetCurContactArray()[i];
		if(sItem)
		{
			rcEnd = GetItemRect(sItem->GetLocation());//sItem->GetRect();
			ptCurrent.x	 = ptStart.x + (rcEnd.left - ptStart.x)*iAnimeStep/iAnimTotal;
			ptCurrent.y	 = ptStart.y + (rcEnd.top - ptStart.y)*iAnimeStep/iAnimTotal;
			sItem->DrawItem(gdiDest.GetDC(), rcClient, GetPlaceHolderImg(), m_hFont, FALSE, &ptCurrent);
		}
	}
}

HRESULT CGuiFavorites::PoomItemChanged(long lOid)
{
    HRESULT hr = S_OK;
    TypeContact sNewInfo;

    CPoomContacts * oPoom = CPoomContacts::Instance();
    IContact* pContact	= oPoom->GetContactFromOID(lOid);
    CPHRE(pContact, _T("pContact is NULL"), E_POINTER);

    for(int i = 0; i < NumFavPages; i++)
    {
        // get the item from array
        int iContactIndex = -1;
        CObjItemContact* objContact = FindContact(lOid, iContactIndex, i);
        // there might not be one
        if(objContact)
        {
            // just reload it
            objContact->Initialize(lOid, objContact->GetLocation(), &m_sizeBlock);
            objContact->UpdateImage();

            if(m_ePage == i)
            {
                POINT ptLocation = objContact->GetLocation();
                SetDirty(ptLocation, ptLocation);
                InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
            }
       }
    }

Error:
    if(pContact)
        pContact->Release();

    return hr;
}

CObjItemContact* CGuiFavorites::FindContact(long lOid, int& iArrayIndex, int iPage)
{
    if(iPage >= NumFavPages)
        return 0;

    for(int i = 0; i < GetContactArray(iPage).GetSize(); i++)
    {
        CObjItemContact* pItem = GetContactArray(iPage)[i];
        if(pItem && pItem->GetOid() == lOid)
        {
            iArrayIndex = i;
            return pItem;
        }
    }

    return 0;
}

HRESULT CGuiFavorites::PoomItemDeleted(long lOid)
{
    HRESULT hr = S_OK;
    // get the item from array
    for(int i= 0; i < NumFavPages; i++)
    {
        int iContactIndex = -1;
        CObjItemContact* objContact = FindContact(lOid, iContactIndex, i);
        // there might not be one
        if(!objContact)
            return E_FAIL;

        if(iContactIndex != -1)
        {
            BOOL bResult = RemoveItem(iContactIndex);
            CBHRE(bResult, _T("PoomItemDeleted() failed"), E_UNEXPECTED);
        }
    }

Error:
    return hr;
}
