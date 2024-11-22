#pragma once
#include "IssVector.h"
#include "IssString.h"


enum CallRuleType
{
	// these are in order of execution
	CO_RingerOffTime,		
	CO_PhoneOffBusy,
	CO_AllowContactNever,
	CO_AllowCategoryNever,
	CO_AllowContactAlways,
	CO_AllowCategoryAlways,
    CO_DenyUnknownCallers,
	NumCallOptionTypes
};

struct TypeAddRule
{
	const TCHAR* szRuleText;
	CallRuleType eRuleType;
};

const TypeAddRule kAddCallingRuleTable[] = { 
	{_T("Turn ringer off during time..."), CO_RingerOffTime},
	{_T("Ignore calls during appointments with busy status"), CO_PhoneOffBusy},
	{_T("Always allow calls from contact..."), CO_AllowContactAlways},
	{_T("Always allow calls from category..."), CO_AllowCategoryAlways},
	{_T("Never allow calls from contact..."), CO_AllowContactNever},
	{_T("Never allow calls from category..."), CO_AllowCategoryNever},
    {_T("Deny callers not in contact list"), CO_DenyUnknownCallers}
};

static TCHAR* GetCallingRuleTableText(CallRuleType eType)
{
    for(int i = 0; i < NumCallOptionTypes; i++)
    {
        if(eType == kAddCallingRuleTable[i].eRuleType)
            return (TCHAR*)kAddCallingRuleTable[i].szRuleText;
    }
    return NULL;
}

static int GetCallingRuleTableIndex(CallRuleType eType)
{
	for(int i = 0; i < NumCallOptionTypes; i++)
	{
		if(eType == kAddCallingRuleTable[i].eRuleType)
			return i;
	}
	return -1;
}


struct TypeCallingRule
{
	CallRuleType	eCallOptionType;	
	TCHAR			szDesc[STRING_MAX];	// for category rule, contact rule
	long			lOID;				// for contact rule
	UINT			uiStartHour;		// for time rule	
	UINT			uiEndHour;			// for time rule
    TypeCallingRule();
    ~TypeCallingRule();
    void            Clone(TypeCallingRule* sClone);
};


class CObjCallOptionList
{
public:
	CObjCallOptionList(void);
	~CObjCallOptionList(void);

	BOOL	Destroy();
	BOOL	ReloadList();
	BOOL	SaveList();
	int		GetListCount(){return m_arrItems.GetSize();}
	TypeCallingRule* GetItem(int iIndex);
 
	BOOL	AddItem(TypeCallingRule* sNewItem);
	BOOL	DeleteItem(TypeCallingRule* sItem);

	void	CheckCallOptions(void* callerID, DWORD& dwCallPermissions);
	void	NotifyListChanged();

private:	// function
	BOOL	AddSortedItem(TypeCallingRule* sItem);
	BOOL	InsertItem(TypeCallingRule* sItem, int iPos);
	BOOL	IsCallerInCategory(void* pInfo, TypeCallingRule* pRule);


private:	// Variables
	CIssVector<TypeCallingRule> m_arrItems;
    CIssString*     m_oStr;

};
