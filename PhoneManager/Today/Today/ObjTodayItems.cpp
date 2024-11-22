#include "StdAfx.h"
#include "ObjTodayItems.h"
#include "CommonDefines.h"
#include "IssString.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "CommonDefines.h"
#include "CommonGuiDefines.h"
#include "PoomContacts.h"
#include "IssDebug.h"
#include "IssCommon.h"
#include "ObjSkinEngine.h"
#include "resource.h"
#include "DlgAddTodayItem.h"

#define COLOR_TEXT_NORMAL 0xFFFFFF 


CObjTodayItems::CObjTodayItems(void)
:m_oStr(CIssString::Instance())
,m_bLockItems(FALSE)
,m_iSelectedIndex(-1)
,m_hWndOwner(NULL)
,m_hInst(NULL)
,m_bLoading(TRUE)
,m_pgdiMem(NULL)
{
    m_hFontText = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
}

CObjTodayItems::~CObjTodayItems(void)
{
	Destroy();
    CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CObjTodayItems::Initialize(HWND hWnd, HINSTANCE hInst, CIssGDIEx& gdiMem)
{
	m_hWndOwner = hWnd;
    m_hInst     = hInst;
    m_pgdiMem   = &gdiMem;

    // preload menu stuff
    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.PreloadImages(hWnd, m_hInst);

    m_guiBackground.Init(hWnd, hInst);
    m_guiBackground.PreloadImages();

    m_dlgContactDetails.PreloadImages(hWnd, hInst, &gdiMem, &m_wndMenu);

	return TRUE;
}

BOOL CObjTodayItems::Destroy()
{
	for(int i = 0; i < m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = (CObjTodayComponentBase*)m_arrComponents.GetElement(i);
		if(pItem)
		{
			delete pItem;
			pItem = 0;
		}
	}
	m_arrComponents.RemoveAll();

	return TRUE;
}

BOOL CObjTodayItems::Refresh(HWND hwnd)
{
	// get or create  default items in registry
	Destroy();
	ReadItems();
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
    rcClient.right = GetSystemMetrics(SM_CXSCREEN);
	CheckAutoArrangeItems(rcClient.right);

	return TRUE;
}

void CObjTodayItems::SetDefaultLayout()
{
    // remove everything from the registry
    Destroy();
    
    ResetLayout();

    Refresh(m_hWndOwner);  
    InvalidateRect(m_hWndOwner, NULL, FALSE);
}

void CObjTodayItems::ResetLayout()
{
    RegDeleteKeyNT(HKEY_CURRENT_USER,REG_TodayItems_Path);

    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iBlockSize = CObjTodayComponentBase::GetBlockSize();
    int iNumBlocksPerLine = iScreenWidth/iBlockSize;

    int iX = 0;
    int iY = 0;
    // we add 5 components
    int iMessageSkip = iNumBlocksPerLine/5;
    RECT rc;

    iX = (iNumBlocksPerLine - (4*iMessageSkip + COMP_WIDTH_Message))/2;

    //////////////////////////////////////////////////////////////////////////
    // Add the profile
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Message, iY + COMP_HEIGHT_Message);
    SetTodayRegItem(GetComponentName(CompType_Sys_Profile), CompType_Sys_Profile, rc);

    //////////////////////////////////////////////////////////////////////////
    // Add the Email
    iX += iMessageSkip;
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Message, iY + COMP_HEIGHT_Message);
    SetTodayRegItem(GetComponentName(CompType_Message_Email), CompType_Message_Email, rc);

    //////////////////////////////////////////////////////////////////////////
    // Add the SMS
    iX += iMessageSkip;
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Message, iY + COMP_HEIGHT_Message);
    SetTodayRegItem(GetComponentName(CompType_Message_SMS), CompType_Message_SMS, rc);

    //////////////////////////////////////////////////////////////////////////
    // Add the Voicemail
    iX += iMessageSkip;
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Message, iY + COMP_HEIGHT_Message);
    SetTodayRegItem(GetComponentName(CompType_Message_VoiceMail), CompType_Message_VoiceMail, rc);

    //////////////////////////////////////////////////////////////////////////
    // Add the Missed Call
    iX += iMessageSkip;
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Message, iY + COMP_HEIGHT_Message);
    SetTodayRegItem(GetComponentName(CompType_Message_MissedCall), CompType_Message_MissedCall, rc);

    //////////////////////////////////////////////////////////////////////////
    // Add the Date/Time Large item
    iY = COMP_HEIGHT_Message;
    iX = (iNumBlocksPerLine-COMP_WIDTH_Time)/2;
    SetRect(&rc, iX, iY, iX + COMP_WIDTH_Time, iY + COMP_HEIGHT_TimeLarge);
    SetTodayRegItem(GetComponentName(CompType_Info_TimeLarge), CompType_Info_TimeLarge, rc);
}

BOOL CObjTodayItems::ReadItems()
{
    // just to be safe
    Destroy();

    DWORD dwValue;
    if(S_OK == GetKey(REG_KEY_ISS_PATH, REG_LockItems, dwValue))
        m_bLockItems = (BOOL)dwValue;

	// get all items in registry
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_TodayItems_Path,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) 
		return FALSE;

	DWORD dwType	= 0;
	DWORD dwIndex	= 0;
	DWORD dwBufSize;
    BYTE btBuffer[2000];
	DWORD dwStringSize = 0;
	int iMax = IsVGA()?800:400;
	TCHAR szItemName[STRING_MAX];

	while(TRUE)
	{
		dwStringSize    = STRING_MAX;
        dwBufSize       = 2000;
		ZeroMemory(&btBuffer, 2000);

		if (RegEnumValue(hKey, dwIndex++,
			szItemName, &dwStringSize, 
			NULL, &dwType, 
			(LPBYTE)&btBuffer,
			&dwBufSize) != ERROR_SUCCESS)
			break;

        if(dwBufSize == sizeof(RegTodayLaunchItem))
        {
            RegTodayLaunchItem* sItem = (RegTodayLaunchItem*)&btBuffer;
            CObjTodayComponentBase* pObjItem = new CObjTodayLauncherComponent(sItem->rcPosition, 
                                                                                m_hWndOwner, m_hInst, 
                                                                                sItem->szPath, 
                                                                                sItem->szFriendlyName,
                                                                                szItemName, 
                                                                                sItem->lType == CompType_Launch_Program?LT_Program:LT_Setting);
            m_arrComponents.AddElement(pObjItem);
            continue;
        }

        // has to be an exact size match
        if(dwBufSize != sizeof(RegTodayItem))
            continue;

        RegTodayItem* sItem = (RegTodayItem*)&btBuffer;

        // if it's an unknown value then skip
        if(sItem->lType >= NumComponentTypes)
            continue;

        // make sure the position of this icon are within bounds
        //if(sItem.rcPosition.left > iMax || sItem.rcPosition.right > iMax || sItem.rcPosition.top > iMax || sItem.rcPosition.bottom > iMax)
        //   continue;

		if(m_oStr->Compare(szItemName, _T("Default")) == 0)
			continue;

		CObjTodayComponentBase* pObjItem = 0;
		if(sItem->lType == CompType_Sys_Profile)
			pObjItem = new CObjTodayProfile((ComponentType)sItem->lType, sItem->rcPosition, m_hWndOwner, m_hInst);
		else if(sItem->lType <= CompType_Info_TimeSmall)
			pObjItem = new CObjTodayInfoComponent((ComponentType)sItem->lType, sItem->rcPosition, m_hWndOwner, m_hInst);
		else if(sItem->lType == CompType_Contact)
			pObjItem = new CObjTodayContactComponent(szItemName, &m_dlgContactDetails, &m_guiBackground, m_pgdiMem, sItem->rcPosition, m_hWndOwner, m_hInst, sItem->lID);
		else
			pObjItem = new CObjTodayMessageComponent((ComponentType)sItem->lType, sItem->rcPosition, m_hWndOwner, m_hInst, m_pgdiMem, &m_guiBackground);

		m_arrComponents.AddElement(pObjItem);
	}

	if(m_arrComponents.GetSize() > 1)
		m_bLoading = TRUE;

	RegCloseKey(hKey);

	return TRUE;
}

int CObjTodayItems::GetWindowHeight()
{
	int iLowestY = IsVGA()?20:10;
	for(int i = 0; i < m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = GetItem(i);
		if(!pItem)
			continue;

		iLowestY = max(iLowestY, pItem->GetFrame().bottom);
	}

    // add a buffer
    iLowestY += IsVGA()?4:2;

    int iMax = IsVGA()?800:400;

    iLowestY = min(iMax, iLowestY);

    DBG_OUT((_T("CObjTodayItems::GetWindowHeight - %d"), iLowestY));
    return iLowestY;
}

void CObjTodayItems::RefreshItem(ComponentType eType, LONG lID)
{
	for(int i = 0; i < m_arrComponents.GetSize(); i++)
	{
        CObjTodayComponentBase* sComponent = m_arrComponents[i];
        if(!sComponent)
            continue;
		if(eType == sComponent->GetType() || eType == NumComponentTypes)
		{
            // if it's a contact change then the ID has to match as well
            if(eType == CompType_Contact && lID != sComponent->GetID() && eType != NumComponentTypes)
                continue;

			sComponent->Refresh(m_hWndOwner);
		}
	}
}
void CObjTodayItems::RemoveItem(HWND hwnd)
{
    
	CObjTodayComponentBase* pItem = GetSelectedItem();
    HRESULT hr;
	if(pItem && m_iSelectedIndex >= 0 && m_iSelectedIndex < m_arrComponents.GetSize())
	{
        if(pItem->GetType() == CompType_Launch_Setting || pItem->GetType() == CompType_Launch_Program)
        {
            CObjTodayLauncherComponent* pLaunchItem = (CObjTodayLauncherComponent*)pItem;
            hr = DeleteValue(REG_TodayItems_Path, pLaunchItem->GetRegName());
        }
        else
		    hr = DeleteValue(REG_TodayItems_Path, pItem->GetName());
        if(hr == S_OK)
        {
            delete pItem;
            m_arrComponents.RemoveElementAt(m_iSelectedIndex);
            m_iSelectedIndex--;
		    InvalidateRect(hwnd, NULL, FALSE);
        }
        else
            MessageBeep(MB_ICONHAND);
	}
}

BOOL CObjTodayItems::SetTodayRegItem(TCHAR* szName, long lType, RECT& rc, long lID)
{
	HKEY hKey;

	RegTodayItem sItem = {lType, rc, lID};

	if(RegCreateKeyEx(HKEY_CURRENT_USER, REG_TodayItems_Path, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, szName, 0,REG_BINARY, (LPBYTE)&sItem, sizeof(RegTodayItem));
        RegFlushKey(hKey);
        RegCloseKey(hKey);
		return TRUE;
	}

	return FALSE;
}

BOOL CObjTodayItems::SetTodayRegLauncherItem(EnumLaunchType eType, 
                                             TCHAR* szFriendlyName, 
                                             TCHAR* szPath,
                                             RECT& rcLocation)
{
    HKEY hKey;

    if(m_oStr->IsEmpty(szPath) || m_oStr->IsEmpty(szFriendlyName))
        return FALSE;

    TCHAR szName[STRING_MAX];

    if(RegCreateKeyEx(HKEY_CURRENT_USER, REG_TodayItems_Path, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return FALSE;

    // find a key that's not taken
    int i;
    RegTodayLaunchItem sItem;
    DWORD dwSize;
    for(i=0; i<100; i++)
    {
        m_oStr->Format(szName, _T("Launcher_%.3d"), i);
        dwSize = sizeof(RegTodayLaunchItem);
        if(RegQueryValueEx(hKey, szName, 0, 0, (LPBYTE)&sItem, &dwSize))
            break;

    }

    ZeroMemory(&sItem, sizeof(RegTodayLaunchItem));
    sItem.lType = eType==LT_Program?CompType_Launch_Program:CompType_Launch_Setting;
    sItem.rcPosition = rcLocation;
    m_oStr->StringCopy(sItem.szPath, szPath);
    m_oStr->StringCopy(sItem.szFriendlyName, szFriendlyName);

    m_oStr->Format(szName, _T("Launcher_%.3d"), i);
    RegSetValueEx(hKey, szName, 0,REG_BINARY, (LPBYTE)&sItem, sizeof(RegTodayLaunchItem));

    RegFlushKey(hKey);
    RegCloseKey(hKey);

    return TRUE;
}

BOOL CObjTodayItems::Draw(CIssGDIEx& gdi, 
						  RECT& rcClient, 
                          RECT& rcClip,
						  COLORREF crText, 
						  COLORREF crHighlight, 
						  BOOL bHasFocus,
						  //int iSelectedIndex,
						  int iExcludeIndex)
{
    /*if(m_gdiBits.GetDC() == NULL || WIDTH(rcClient) != m_gdiBits.GetWidth() || HEIGHT(rcClient) != m_gdiBits.GetHeight())
        m_gdiBits.Create(gdi, rcClient, FALSE, TRUE, FALSE);*/

	// copy background to the parent
    //BitBlt(m_gdiBits, rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), gdi, rcClip.left, rcClip.top, SRCCOPY);

	//if(bHasFocus)
	//	AlphaFillRect(m_gdiBits, rcClip, crHighlight, 200);

	for(int i = 0; i < m_arrComponents.GetSize(); i++)
	{
		if(i != iExcludeIndex)
		{
            CObjTodayComponentBase* oComponent = m_arrComponents[i];
            if(oComponent && IsRectInRect(rcClip, oComponent->GetFrame()))
            {
			    oComponent->Draw(gdi, rcClient, crText, 0x0045bb45, bHasFocus, (m_iSelectedIndex == i));
                oComponent->DrawText(gdi, m_hFontText);
            }
		}
	}

    // copy everything back
    /*BitBlt(gdi, rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), m_gdiBits, rcClip.left, rcClip.top, SRCCOPY);

    // now draw text if needed (with cleartype)
    for(int i = 0; i < m_arrComponents.GetSize(); i++)
    {
        if(i != iExcludeIndex)
        {
            CObjTodayComponentBase* oComponent = m_arrComponents[i];
            if(oComponent && IsRectInRect(rcClip, oComponent->GetFrame()))
                oComponent->DrawText(gdi, m_hFontText);
        }
    }*/

	return TRUE;
}

int CObjTodayItems::GetItemMouseOver(POINT& pt)
{
	int iResult = UND;
	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = m_arrComponents[i];
		if(pItem && PtInRect(&pItem->GetFrame(), pt))
			return i;
	}
	return iResult;
}

int	CObjTodayItems::GetItemConflicting(RECT& rc, CObjTodayComponentBase* pExclude)
{
	int iResult = UND;

	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pCheckItem = GetItem(i);
		if(pCheckItem && pCheckItem != pExclude && IsOverlappedRect(rc, pCheckItem->GetFrame()))
			return i;
	}
	return iResult;
}

int CObjTodayItems::GetNumItemsRectOver(RECT& rc, CObjTodayComponentBase* pExclude)
{
	int iResult = 0;
	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pCompare = m_arrComponents[i];
		if(pCompare && pExclude != pCompare && IsOverlappedRect(rc, pCompare->GetFrame()))
			iResult++;
	}
	//DebugOut(_T("num items overlapping rect: %d"), iResult);
	return iResult;
}

CObjTodayComponentBase* CObjTodayItems::GetItem(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_arrComponents.GetSize())
		return NULL;

	return (CObjTodayComponentBase*)m_arrComponents.GetElement(iIndex);
}

CObjTodayComponentBase*  CObjTodayItems::GetSelectedItem()
{
	return GetItem(m_iSelectedIndex);
}

BOOL CObjTodayItems::Save()
{
    RegDeleteKeyNT(HKEY_CURRENT_USER,REG_TodayItems_Path);
	for(int i = 0; i < m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = GetItem(i);
		if(pItem)
        {
            if(pItem->GetType() == CompType_Launch_Setting || pItem->GetType() == CompType_Launch_Program)
            {
                CObjTodayLauncherComponent* pLaunchItem = (CObjTodayLauncherComponent*)pItem;
                SetTodayRegLauncherItem(pItem->GetType()==CompType_Launch_Program?LT_Program:LT_Setting, 
                                        pLaunchItem->GetFriendlyName(), 
                                        pLaunchItem->GetPath(),
                                        pItem->GetMatrixRect());
            }
            else
			    SetTodayRegItem(pItem->GetName(), pItem->GetType(), pItem->GetMatrixRect(), pItem->GetID());
        }
	}

    SetKey(REG_KEY_ISS_PATH, REG_LockItems, (DWORD)m_bLockItems);

	return TRUE;
}

BOOL CObjTodayItems::CheckAutoArrangeItems(int iRightBounds)
{
	if(iRightBounds <= 0)
		return FALSE;

	int iMatrixRightBounds = iRightBounds / CObjTodayComponentBase::GetBlockSize();
	BOOL bAnyChanged = FALSE;

	// set any out of bounds RECT's undefined
	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = GetItem(i);
		if(!pItem)
			continue;

		if(pItem->GetMatrixRect().right > iMatrixRightBounds || pItem->GetMatrixRect().right == 0)
		{
			pItem->SetLocationUndefined();
			bAnyChanged = TRUE;
		}
		if(GetNumItemsRectOver(pItem->GetFrame(), pItem) > 0)
		{
			pItem->SetLocationUndefined();
			bAnyChanged = TRUE;
		}
	}
	

	if(bAnyChanged)
	{
		AutoArrangeItems(iRightBounds);
		return TRUE;
	}

	return FALSE;
}

BOOL CObjTodayItems::AutoArrangeItems(int iRightBounds)
{
	//ASSERT(iRightBounds);
	if(iRightBounds <= 0)
		return FALSE;

	int iMatrixRightBounds = iRightBounds / CObjTodayComponentBase::GetBlockSize();
	BOOL bAnyChanged = FALSE;

	// set valid position for all undefined RECT's
	// loop through all the icons in the list
	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = GetItem(i);
		if(!pItem)
			continue;

		// if a certain icon has been set as undefined then we want to find a place to lay it down
		if(pItem->GetMatrixRect().right == 0)
		{	
			if(PlaceIconNextAvailablePos(pItem, iMatrixRightBounds))
				bAnyChanged = TRUE;
		}
	}

	if(bAnyChanged)
	{
		Save();
		return TRUE;
	}

	return FALSE;
}

BOOL CObjTodayItems::PlaceIconNextAvailablePos(CObjTodayComponentBase* pItem, int iMatrixRightBounds, BOOL bNewRow)
{
    if(!pItem)
        return FALSE;

	// loop through and try and set this icon down
	int w = WIDTH(pItem->GetMatrixRect());
	int h = HEIGHT(pItem->GetMatrixRect());
	int y = 0;
	int iBlock = CObjTodayComponentBase::GetBlockSize();

	while(TRUE)
	{
		// sanity check
		if(y > 100)
			break;
		for(int x=0; x<=iMatrixRightBounds - w; x++)
		{
			if(bNewRow && x > 0)
				break;
			RECT rcTest = {x * iBlock, y * iBlock, (x+w) * iBlock, (y+h) * iBlock};
			if(GetNumItemsRectOver(rcTest, NULL) == 0)
			{
				// set it down here
				RECT rcLocation = {x, y, x+w, y+h};
				pItem->SetMatrixLocation(rcLocation);
				return TRUE;
			}
		}
		y++;
	}
	return FALSE;
}

BOOL CObjTodayItems::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	if(!GetClientRect(hWnd, &rcClient))
		return FALSE;

    rcClient.right = GetSystemMetrics(SM_CXSCREEN);

    RECT rcOld  = {0,0,0,0};
	RECT rcItem = {rcClient.left-1, rcClient.top-1, rcClient.right+1, rcClient.bottom+1};
    CObjTodayComponentBase* pItem = GetSelectedItem();
	if(pItem)
    {
		rcItem = pItem->GetFrame(); 
        rcOld = pItem->GetFrame();
    }

	int iNewSelection = UND;

	switch(wParam)
	{
	case VK_UP:
		iNewSelection = FindItemLessThan(CENTERX(rcItem), CENTERY(rcItem), rcClient.right, rcItem.bottom-1, rcClient);
		break;
	case VK_LEFT:
		iNewSelection = FindItemLessThan(CENTERX(rcItem), CENTERY(rcItem), rcItem.right-1, rcClient.bottom, rcClient);
		break;
	case VK_DOWN:
		iNewSelection = FindItemGreaterThan(CENTERX(rcItem), CENTERY(rcItem), 0, rcItem.top+1, rcClient);
		break;
	case VK_RIGHT:
		iNewSelection = FindItemGreaterThan(CENTERX(rcItem), CENTERY(rcItem), rcItem.left+1, 0, rcClient);
		break;
	case VK_RETURN:
		{
			if(m_iSelectedIndex > -1)
			{
				CObjTodayComponentBase* pItem = GetSelectedItem();
				if(pItem)
				{
					POINT pt = {CENTERX(pItem->GetFrame()), CENTERY(pItem->GetFrame())};
					pItem->ExecuteItem(hWnd, pt);
				}
			}
		}
	default:

		return FALSE;
	}

	if(iNewSelection != UND)
	{
		//DebugOut(_T("setting selection: %d"), m_iSelectedIndex);
		m_iSelectedIndex = iNewSelection;

        CObjTodayComponentBase* pItem = GetSelectedItem();
        if(pItem)
        {
            RECT rcFull = pItem->GetFrame();
            rcFull.left = min(rcOld.left, rcFull.left);
            rcFull.right= max(rcOld.right, rcFull.right);
            rcFull.top  = min(rcOld.top, rcFull.top);
            rcFull.bottom= max(rcOld.bottom, rcFull.bottom);
            InvalidateRect(hWnd, &rcFull, FALSE);
        }
		return TRUE;
	}

	return FALSE;
}

int CObjTodayItems::FindItemGreaterThan(int iX, int iY, int iXCheck, int iYCheck, RECT& rcClient)
{
	int		iIndexMatch			= UND;
	double  dbClosestDistance	= 5000.0; //start with a large one so we can find something
	double	dbCheckDistance		= 0;
	RECT	rcLocation;//, rcIcon;

	rcLocation.left		= iXCheck;
	rcLocation.top		= iYCheck;
	rcLocation.right	= rcClient.right;
	rcLocation.bottom	= rcClient.bottom;

	// DH: favor row or column
	BOOL bIsRow = (rcLocation.left > rcLocation.top);
	float fXExponent = 2.0f; //bIsRow ? 2.0f : 3.0f;
	float fYExponent = 2.0f; //bIsRow ? 3.0f : 2.0f;

	// loop through all the icons and find out if we have an item
	for(int i=0; i < m_arrComponents.GetSize(); i++)
	{
		if(i == m_iSelectedIndex)
			continue;
		CObjTodayComponentBase* pItem	= GetItem(i);
		if(!pItem)
			continue;

		// is this icon worth checking
		if(IsContainedIn(rcLocation, pItem->GetFrame()))
		{
			// find the distance between the two icons
			dbCheckDistance = sqrt(pow(abs(CENTERX(pItem->GetFrame()) - iX), fXExponent) + 
				pow(abs(CENTERY(pItem->GetFrame()) - iY), fYExponent));
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

int CObjTodayItems::FindItemLessThan(int iX, int iY, int iXCheck, int iYCheck, RECT& rcClient)
{
	int		iIndexMatch			= UND;
	double  dbClosestDistance	= 5000.0; //start with a large one so we can find something
	double	dbCheckDistance		= 0;
	RECT	rcLocation;//, rcIcon;

	rcLocation.left		= 0;
	rcLocation.top		= 0;
	rcLocation.right	= iXCheck;
	rcLocation.bottom	= iYCheck;

	// DH: favor row or column
	BOOL bIsRow = (rcLocation.right < rcClient.right);
	float fXExponent = 2.0f; //bIsRow ? 2.0f : 3.0f;
	float fYExponent = 2.0f; //bIsRow ? 3.0f : 2.0f;

	// loop through all the icons and find out if we have an item
	for(int i=0; i < m_arrComponents.GetSize(); i++)
	{
		if(i == m_iSelectedIndex)
			continue;
		CObjTodayComponentBase* pItem	= GetItem(i);
		if(!pItem)
			continue;

		// is this icon worth checking
		if(IsContainedIn(rcLocation, pItem->GetFrame()))
		{
			// find the distance between the two icons
			dbCheckDistance = sqrt(pow(abs(CENTERX(pItem->GetFrame()) - iX /*+ sizeItem.cx*/), fXExponent) + 
				pow(abs(CENTERY(pItem->GetFrame()) - iY /*+ sizeItem.cy*/),fYExponent));
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

BOOL CObjTodayItems::SmartArrangeItems(HWND hWnd)
{
	/*RECT rcClient;
	if(!GetClientRect(hWnd, &rcClient))
		return FALSE;

	int iMatrixRightBounds = rcClient.right / CObjTodayComponentBase::GetBlockSize();

	// loop through and make everything undefined
	for(int i=0; i<m_arrComponents.GetSize(); i++)
	{
		CObjTodayComponentBase* pItem = GetItem(i);
		if(!pItem)
			continue;
		pItem->SetLocationUndefined();
	}

	// now nothing is placed down on the matrix

	// make a temp array for placing icons
	CIssVector<CObjTodayComponentBase> arrTemp;
	arrTemp.Copy(m_arrComponents);

	// we will start a new row for each icon height
	// that way they will all be in neat even rows
	BOOL bNewRow = TRUE;	
	int iHeight = 1;
	while(arrTemp.GetSize())
	{
		// sanity check
		if(iHeight > 100)
			return FALSE;

		int i = UND;
		while(++i < arrTemp.GetSize())
		{
			CObjTodayComponentBase* pItem = arrTemp[i];
			if(!pItem)
				continue;
			if(!pItem->IsLocationUndefined())
				continue;

			if(HEIGHT(pItem->GetMatrixRect()) == iHeight)
			{
				PlaceIconNextAvailablePos(pItem, iMatrixRightBounds, bNewRow);
				bNewRow = FALSE;
				arrTemp.RemoveElement(pItem);
				i = UND;	// start over
			}
		}
		iHeight++;
		bNewRow = TRUE;
	}
	Save();*/

	return TRUE;

}

int CObjTodayItems::GetMenuItemHeight()
{
    CObjTodayComponentBase* oBase = GetSelectedItem();
    if(!oBase || oBase->GetType() != CompType_Sys_Profile)
        return 0;

    CObjTodayProfile* oProfile = (CObjTodayProfile*)oBase;
    return oProfile->GetMenuItemHeight();
}

void CObjTodayItems::DrawMenuItem(HDC hDC, RECT rcLocation, DWORD dwItemData, COLORREF crText)
{
    RECT rc     = rcLocation;
    rc.right    = WIDTH(rc);
    rc.bottom   = HEIGHT(rc);
    rc.left     = rc.top = 0;

    CObjTodayComponentBase* oBase = GetSelectedItem();
    if(!oBase || oBase->GetType() != CompType_Sys_Profile || !m_pgdiMem)
        return;

    CObjTodayProfile* oProfile = (CObjTodayProfile*)oBase;

    BitBlt(*m_pgdiMem, 0,0, WIDTH(rc), HEIGHT(rc), hDC, rcLocation.left, rcLocation.top, SRCCOPY);
    oProfile->DrawMenuItem(*m_pgdiMem, rc, (EnumPhoneProfile)dwItemData, crText);
    BitBlt(hDC, rcLocation.left,rcLocation.top, WIDTH(rc), HEIGHT(rc), m_pgdiMem->GetDC(), 0,0, SRCCOPY);
}