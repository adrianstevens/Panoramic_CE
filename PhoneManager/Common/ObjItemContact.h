#pragma once

#include "IssGDIEx.h"
#include "IssString.h"
#include "PoomContacts.h"
/*
enum EnumContact
{
	CONTACT_Name,
	CONTACT_Small,
	CONTACT_SmallName,
	CONTACT_Large,
	CONTACT_LargeName,
	CONTACT_Undefined,
};
*/
class CObjItemContact
{
public:
	CObjItemContact(void);
	~CObjItemContact(void);

	BOOL	Initialize(/*EnumContact eContact,*/ LONG lOid, POINT ptLocation, SIZE* sizeBlock);
	BOOL	Destroy();
	BOOL	DrawItem(HDC dc, RECT rcClient, CIssGDIEx& gdiDefaultImg, HFONT hFont, BOOL bDrawZeroBase = FALSE, POINT* ptOverride = NULL, BOOL bAlwaysText = FALSE);
	void	SetItemLocation(int iX, int iY);
	void	SetItemLocation(POINT pt);
	void	SetItemSize(SIZE sizeItem);
	BOOL	UpdateImage();
	BOOL	LoadContactInfo(/*EnumContact eContact*/);
	void	SetBlockSize(SIZE* sizeBlock){m_sizeBlock=sizeBlock;};
	LONG	GetOid(){return m_sContact.lOid;};
	POINT	GetLocation(){return m_ptLocation;};
	TypeContact* GetContactInfo(){return &m_sContact;};
	

private:	// functions
	void	DrawItemText(HDC dc, TCHAR* szText, HFONT hFont, RECT& rcLocation, UINT uiAlignment = DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS);


private:	// variables
	SIZE*			m_sizeBlock;			// size in pixels of our display size
	POINT			m_ptLocation;			// location counted in blocks
	TypeContact		m_sContact;				// contact information
	CIssString*		m_oStr;
};
