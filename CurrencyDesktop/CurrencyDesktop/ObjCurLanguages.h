#pragma once

#include "IssString.h"
#include "IssVector.h"

enum EnumLanguages
{
	L_English = 0,
	L_French,
	L_Spanish,
	L_German,
	L_Portuguese,
    L_Japanese, //5
	L_Chinese, //6
	L_Korean,
	L_Dutch,
	L_Italian,
	L_Count,
};

struct TypeLanItem
{
	TCHAR* szCode;
	TCHAR* szText[L_Count];
	TypeLanItem();
	~TypeLanItem();
};

class CObjCurLanguages
{
public:
	CObjCurLanguages(void);
	~CObjCurLanguages(void);

	HRESULT Initialize(HINSTANCE hInst);
	TypeLanItem* FindItem(TCHAR* szCode);

private:	
	static int CompareNameItems(const void* lp1, const void* lp2);

private:
	CIssString*	m_oStr;
	CIssVector<TypeLanItem>	m_arrCodes;
};
