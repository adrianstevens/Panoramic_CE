#pragma once

#include "stdafx.h"
#include "IssVector.h"
#include "IssString.h"
#include "ObjRingtoneEngine.h"
#include <soundfile.h>


enum EnumRingtoneRuleType
{
	RR_Category,	
	RR_Contact,
	RR_Default,
	NumRingtoneRuleTypes
};

struct TypeRingtoneRule
{
	EnumRingtoneRuleType	eRuleType;	
	EnumRingType			eRingType;
	int						iOID;
	TCHAR					szDesc[STRING_MAX];
	SNDFILEINFO				ringToneInfo;
};


class CObjRingtoneRuleList
{
public:
	CObjRingtoneRuleList(void);
	~CObjRingtoneRuleList(void);


    void    FindAllContactRingtones();
	BOOL	Destroy();
	BOOL	ReloadList();
	BOOL	SaveList(BOOL bNotify = TRUE);
	int		GetListCount(){return m_arrItems.GetSize();};
	TypeRingtoneRule* GetItem(int iIndex);

	BOOL	AddItem(TypeRingtoneRule* sItem, BOOL bSaveToReg = TRUE);
	BOOL	InsertItem(TypeRingtoneRule* sItem, int iPos, BOOL bNotify = TRUE);
	BOOL	DeleteItem(TypeRingtoneRule* sItem);
	
	BOOL	GetRingtoneRule(void* pCallerID, TypeRingtoneRule* sRingtoneRule);


private:	// functions
	void	NotifyListChanged();
	void	CheckAddDefaultItem();

private:	// Variables
	CIssVector<TypeRingtoneRule> m_arrItems;

};
