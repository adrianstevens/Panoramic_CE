#include "StdAfx.h"
#include "ObjItemContact.h"
#include "IssRect.h"
#include "IssGDIDraw.h"
#include "IssCommon.h"
#include "ContactsGuiDefines.h"

#define PAD_WIDTH	2//4


CObjItemContact::CObjItemContact(void)
:m_oStr(CIssString::Instance())
//,m_eContactType(CONTACT_Large)
,m_sizeBlock(NULL)
{
	//ZeroMemory(&m_sizeItem, sizeof(SIZE));
	ZeroMemory(&m_ptLocation, sizeof(POINT));
}

CObjItemContact::~CObjItemContact(void)
{
	Destroy();
}

BOOL CObjItemContact::Initialize(/*CPoomContacts& oPoom, EnumContact eContact, */LONG lOid, POINT ptLocation, SIZE* sizeBlock)
{
	Destroy();

	m_ptLocation	= ptLocation;
	m_sizeBlock		= sizeBlock;
	m_sContact.lOid	= lOid;

	return LoadContactInfo(/*eContact*/);	
}

BOOL CObjItemContact::LoadContactInfo()
{
	BOOL bRetrievePic = TRUE;

    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

	IContact* pContact	= oPoom->GetContactFromOID(m_sContact.lOid);
	if(!pContact)
		return FALSE;

	// clear out anything old that might have been there
	m_sContact.Clear();
	
	// retrieve all the contact info as well as the potential info
	if(!oPoom->GetContactInfo(pContact, m_sContact/*, bRetrievePic*/))
		return FALSE;

	return TRUE;
}

BOOL CObjItemContact::UpdateImage()
{
	//UpdateLocation();
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

	int iSize = max(/*m_sizeItem.cx * */ m_sizeBlock->cx, /*m_sizeItem.cy * */ m_sizeBlock->cy);

	if(m_sContact.gdiPicture)
	{
		m_sContact.gdiPicture->Destroy();
		m_sContact.gdiPicture = NULL;
	}

	m_sContact.gdiPicture = oPoom->GetUserPicture(m_sContact.lOid, 
        /*m_sizeItem.cx * */m_sizeBlock->cx - PAD_WIDTH, 
        /*m_sizeItem.cy * */m_sizeBlock->cy - PAD_WIDTH, 
        RGB(0,0,0), TRUE);

	return TRUE;
}

BOOL CObjItemContact::Destroy()
{
	//ZeroMemory(&m_sizeItem, sizeof(SIZE));
	ZeroMemory(&m_ptLocation, sizeof(POINT));
	m_sContact.Clear();
	//m_eContactType = CONTACT_Large;

	return TRUE;
}


BOOL CObjItemContact::DrawItem(HDC dc, RECT rcDest, CIssGDIEx& gdiDefaultImg, HFONT hFont, BOOL bDrawZeroBase /*= FALSE*/, POINT* ptOverride /*= NULL*/, BOOL bAlwaysText /* = FALSE*/)
{
    // changing the way this works...
    // all drawing is now 0-based
    // rcDest is the size of dest gdi

	//if(m_sizeItem.cx == 0 || m_sizeItem.cy == 0)
	//	return FALSE;

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

	if(m_sContact.gdiPicture)
	{
		int iSize = max(/*m_sizeItem.cx * */m_sizeBlock->cx, /*m_sizeItem.cy * */m_sizeBlock->cy);


		BitBlt(dc,
			   rc.left, rc.top,
			   rc.GetWidth(), rc.GetHeight(),
			   m_sContact.gdiPicture->GetDC(),
			   (m_sContact.gdiPicture->GetWidth() - rc.GetWidth())/2, (m_sContact.gdiPicture->GetHeight() - rc.GetHeight())/2,
			   SRCCOPY);
	}
	else
	{
        Rectangle(dc, rc.Get(), COLOR_FAVORITES_PLACEHOLDER_BG, COLOR_FAVORITES_PLACEHOLDER_BG);
	    BitBlt(dc,
	       rc.left, rc.top,
	       rc.GetWidth(), rc.GetHeight(),
	       gdiDefaultImg.GetDC(),
	       (gdiDefaultImg.GetWidth() - rc.GetWidth())/2, (gdiDefaultImg.GetHeight() - rc.GetHeight())/2,
	       SRCCOPY);
    }

    if(m_sContact.gdiPicture == NULL || bAlwaysText)
    {
		TCHAR szName[STRING_MAX] = _T("");

        CPoomContacts* pPoom = CPoomContacts::Instance();
        if(pPoom)
        {
            pPoom->GetFormatedNameText(szName, m_sContact.szFirstName, m_sContact.szMiddleName, m_sContact.szLastName, m_sContact.szCompany, NULL, TRUE);
        }

		/*if(!m_oStr->IsEmpty(m_sContact.szFirstName))
		{
			m_oStr->Concatenate(szName, m_sContact.szFirstName);
			if(!m_oStr->IsEmpty(m_sContact.szFirstName))
			{
				m_oStr->Concatenate(szName, _T(" "));
				//m_oStr->Concatenate(szName, ("\r\n"));
			}
		}

		if(!m_oStr->IsEmpty(m_sContact.szLastName))
			m_oStr->Concatenate(szName, m_sContact.szLastName);*/

        static int iEdgeIndent = GetSystemMetrics(SM_CXSMICON)/4;

        RECT rcText = rc.Get();

        rcText = rc.Get();
        RECT rcTemp = rcText;
        rcTemp.left += iEdgeIndent;
        rcTemp.right -= iEdgeIndent;
        DrawText(dc, szName, rcTemp, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, hFont);

        rcText.top = rcText.bottom - HEIGHT(rcTemp) - INDENT;
        rcText.left += iEdgeIndent;
        rcText.right -= iEdgeIndent;


        DrawItemText(dc, szName, hFont, rcText, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);


		/*RECT rcText = rc.Get();
		rcText.bottom = rcText.top + HEIGHT(rcText) / 2;
		DrawItemText(dc, m_sContact.szFirstName, hFont, rcText, DT_CENTER | DT_BOTTOM | DT_WORD_ELLIPSIS);

		rcText = rc.Get();
		rcText.top = rcText.bottom - HEIGHT(rcText) / 2;
		DrawItemText(dc, m_sContact.szLastName, hFont, rcText,DT_CENTER | DT_TOP | DT_WORD_ELLIPSIS);*/
	}

	return TRUE;
}

void CObjItemContact::DrawItemText(HDC dc, TCHAR* szText, HFONT hFont, RECT& rcLocation, UINT uiAlignment /*= DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS*/)
{
    static int iOffset = GetSystemMetrics(SM_CXICON)/32;

	RECT rcTemp = rcLocation;
    OffsetRect(&rcTemp, iOffset,iOffset);
	DrawText(dc, szText, rcTemp, uiAlignment, hFont, RGB(50,50,50));
    OffsetRect(&rcTemp, -iOffset,-iOffset);
	DrawText(dc, szText, rcTemp, uiAlignment, hFont, RGB(255,255,255));
}

void CObjItemContact::SetItemLocation(int iX, int iY)
{
	m_ptLocation.x		= iX;
	m_ptLocation.y		= iY;
	//UpdateLocation();
}

void CObjItemContact::SetItemLocation(POINT pt)
{
	m_ptLocation		= pt;
	//UpdateLocation();
}
/*
void CObjItemContact::SetItemSize(SIZE sizeItem)
{
	m_sizeItem			= sizeItem;
	//UpdateLocation();
}
*/
//void CObjItemContact::UpdateLocation()
//{
//	m_rcLocation.left	= FAVORITES_GRID_BORDER + m_ptLocation.x*m_sizeBlock->cx;
//	m_rcLocation.top	= FAVORITES_GRID_BORDER + m_ptLocation.y*m_sizeBlock->cy;
//	m_rcLocation.right	= m_rcLocation.left + m_sizeItem.cx*m_sizeBlock->cx;
//	m_rcLocation.bottom	= m_rcLocation.top + m_sizeItem.cy*m_sizeBlock->cy;
//}

