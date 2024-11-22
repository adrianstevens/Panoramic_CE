#pragma once

#include "stdafx.h"
#include "IssVector.h"
#include "IssString.h"

enum BlocklistItem_Type
{
	BlocklistItem_contact,	
	BlocklistItem_phoneNumber,	
	BlocklistItem_phonePrefix,	
	BlocklistItem_phoneAreaCode,	
	NumBlocklistTypes
};

struct TypeBlocklist
{
	BlocklistItem_Type	eType;			
	TCHAR				szID[STRING_MAX];
    LONG                lOid;
	TypeBlocklist();
	~TypeBlocklist();
};

class CObjBlocklist
{
public:
	CObjBlocklist(void);
	~CObjBlocklist(void);

	BOOL	Destroy();
	BOOL	ReloadList();
	BOOL	SaveList();
	int		GetListCount(){return m_arrItems.GetSize();};
	TypeBlocklist* GetItem(int iIndex) {return m_arrItems[iIndex];};
	BOOL	FindItem(TypeBlocklist* sItem);
    TypeBlocklist* FindItem(TCHAR* szID);
    TypeBlocklist* FindItem(long lOID);
	BOOL	AddItem(TypeBlocklist* sItem);
	BOOL	DeleteItem(TypeBlocklist* sItem);

	BOOL	CheckBlock(void* pInfo);

private:	// functions
	static int CompareItems(const void* lp1, const void* lp2);
	BOOL	ComparePhoneNumbers(TCHAR* szPhone1, TCHAR* szPhone2);
	BOOL	ComparePhonePrefix(TCHAR* szPhone, TCHAR* szPrefix);
	void	MakeNumeric(TCHAR* szString);
	void	NotifyListChanged();

private:	// Variables
	CIssString*		m_oStr;
	CIssVector<TypeBlocklist> m_arrItems;
};
