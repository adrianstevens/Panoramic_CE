#include "StdAfx.h"
#include "ObjCallOptionList.h"
#include "IssCommon.h"
#include "IssRegistry.h"
#include "CommonDefines.h"
#include "IssTapi.h"
#include "ObjStateAndNotifyMgr.h"
#include "PoomContacts.h"
#include "IssDebug.h"


TypeCallingRule::TypeCallingRule()
:eCallOptionType(CO_PhoneOffBusy)
,lOID(0)
,uiStartHour(0)
,uiEndHour(0)
{
    ZeroMemory(szDesc, sizeof(TCHAR)*STRING_MAX);
}

TypeCallingRule::~TypeCallingRule()
{}

void TypeCallingRule::Clone(TypeCallingRule* sClone)
{
    if(!sClone)
        return;
    eCallOptionType = sClone->eCallOptionType;
    lOID            = sClone->lOID;
    uiStartHour     = sClone->uiStartHour;
    uiEndHour       = sClone->uiEndHour;
    memcpy(szDesc, sClone->szDesc, sizeof(TCHAR)*STRING_MAX);
}

CObjCallOptionList::CObjCallOptionList(void)
:m_oStr(CIssString::Instance())
{
}

CObjCallOptionList::~CObjCallOptionList(void)
{
	Destroy();
}

BOOL CObjCallOptionList::Destroy()
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypeCallingRule* sItem = m_arrItems[i];
		if(sItem)
			delete sItem;
	}
	m_arrItems.RemoveAll();
	return TRUE;
}

TypeCallingRule*  CObjCallOptionList::GetItem(int iIndex) 
{
	if(iIndex >= 0 && iIndex < m_arrItems.GetSize())
		return m_arrItems[iIndex];

	return NULL;
}

BOOL CObjCallOptionList::ReloadList()
{
	Destroy();
	FILE *fp;

    DBG_OUT((_T("CObjCallOptionList::ReloadList()")));

	//open the file
	if((fp = _tfopen(TXT_FileCallOptionlist, _T("r"))) == NULL)
    {
        DBG_OUT((_T("CallOptionList file not found")));
        return FALSE;
    }

    //read file stuff
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int iItemSize = sizeof(TypeCallingRule);
    if(dwFileSize == 0 || dwFileSize%iItemSize != 0)
    {
        DBG_OUT((_T("CallOptionsList file size is not correct so deleting")));
        fclose(fp);
        DeleteFile(TXT_FileCallOptionlist);
        return FALSE;
    }

	int iItemsRead = 0;
	int iAmountRead = 0;	
	do
	{
		TypeCallingRule* sOption = new TypeCallingRule;
		iAmountRead = fread(sOption, iItemSize, 1, fp);
		if(iAmountRead == 1)
		{
            DBG_OUT((_T("CallOption item - %s"), sOption->szDesc));
			m_arrItems.AddElement(sOption);
			iItemsRead += iAmountRead;
		}
		else
			delete sOption;

	}
	while(iAmountRead);

	DBG_OUT((_T("CObjCallOptionList read %i items"), iItemsRead));

	// close the file
	fclose(fp);

	return TRUE;
}

BOOL CObjCallOptionList::AddItem(TypeCallingRule* sNewItem)
{
	// types: CO_RingerOffTime,	CO_PhoneOffBusy can have one only
	// all others, just add to list
	// at this point, we'll add them in order that they need to be read (the enum order)

    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        TypeCallingRule* sItem = GetItem(i);
        if(!sItem)
            continue;
        if(sItem->eCallOptionType == sNewItem->eCallOptionType)
        {
            BOOL bItemInList = FALSE;
            switch(sItem->eCallOptionType)
            {
            case CO_RingerOffTime:
                if(sItem->uiEndHour == sNewItem->uiEndHour && sItem->uiStartHour == sNewItem->uiStartHour)
                    bItemInList = TRUE;
                break;
            case CO_PhoneOffBusy:
            case CO_DenyUnknownCallers:
                bItemInList = TRUE;
                break;
            case CO_AllowContactNever:
            case CO_AllowContactAlways:
                if(sItem->lOID == sNewItem->lOID)
                    bItemInList = TRUE;
                break;
            case CO_AllowCategoryNever:
            case CO_AllowCategoryAlways:
                if(0 == m_oStr->Compare(sItem->szDesc, sNewItem->szDesc))
                    bItemInList = TRUE;
            }
            if(bItemInList)
            {
                DebugOut(_T("call option item already in call option list"));
                delete sItem;
                sItem = NULL;
                return FALSE;
            }
        }
    }

    TypeCallingRule* sSaveItem = new TypeCallingRule;
    if(!sSaveItem)
        return FALSE;

    sSaveItem->Clone(sNewItem);
	if(!AddSortedItem(sSaveItem))
    {
        delete sSaveItem;
        return FALSE;
    }
    else
        return TRUE;

}

BOOL CObjCallOptionList::AddSortedItem(TypeCallingRule* sItem)
{
	// get position to add item
	// add at the end of similar type
	CallRuleType nextCOType = (CallRuleType)(sItem->eCallOptionType + 1);
	if(nextCOType == NumCallOptionTypes)
	{
		// just add to end of list
		m_arrItems.AddElement(sItem);
		return SaveList();
	}

	int iPos = 0;
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeCallingRule* sCompareItem = GetItem(i);
		if(sCompareItem && sCompareItem->eCallOptionType == nextCOType)
		{
			iPos = i;
			break;
		}
	}

	ASSERT (iPos == 0 || iPos < m_arrItems.GetSize());
	if(iPos == 0 || iPos < m_arrItems.GetSize())
		return InsertItem(sItem, iPos);

	return FALSE;
}

BOOL CObjCallOptionList::InsertItem(TypeCallingRule* sItem, int iPos)
{
	if(iPos > m_arrItems.GetSize() || iPos < 0)
		return FALSE;

	if(!sItem)
		return FALSE;

	m_arrItems.AddElement(sItem);

	// BUGBUG: there is a bug in here somewhere so I'm doing it a better way
	/*for(int i = m_arrItems.GetSize()-1; i > iPos; i--)
	{
		TypeCallingRule* pTemp = m_arrItems[i];
		m_arrItems.SetElementAt(m_arrItems[i-1], i);
		m_arrItems.SetElementAt(pTemp, i-1);
	}*/
	if(iPos != m_arrItems.GetSize()-1)
		m_arrItems.Move(m_arrItems.GetSize()-1, iPos);

	return SaveList();
}

BOOL CObjCallOptionList::SaveList()
{
	DeleteFile(TXT_FileCallOptionlist);

	FILE* fp;

	//open the file
	if((fp = _tfopen(TXT_FileCallOptionlist, _T("w"))) == NULL)
		return FALSE;

	int iNumItems = 0;
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		size_t n;
		TypeCallingRule* sOption = m_arrItems[i];
		n = fwrite(sOption, sizeof(TypeCallingRule), 1, fp);
		if(n != 1)
		{
			DebugOut(_T("trouble writing file %s"), TXT_FileCallOptionlist);
		}
		iNumItems += (int)n;
	}

#ifdef DEBUG
    // BUGBUG: file won't save properly with this in, prob cause we have to set the file pointer back to the end
	/*DebugOut(_T("CObjCallOptionList wrote %i items %d bytes"), iNumItems, sizeof(TypeCallingRule));
	// debug test - get file size
	DWORD dwFileSize = 0;
	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	DebugOut(_T("file size: %d expected: %d"), dwFileSize, iNumItems * sizeof(TypeCallingRule));*/
#endif

	fclose(fp);

	NotifyListChanged();

	return TRUE;
}

BOOL CObjCallOptionList::DeleteItem(TypeCallingRule* sItem)
{
	int iListCount = m_arrItems.GetSize();

	m_arrItems.RemoveElement(sItem);
	if(--iListCount != m_arrItems.GetSize())
		return FALSE;

	if(SaveList())
		return TRUE;

	return FALSE;
}

void CObjCallOptionList::NotifyListChanged()
{
	// inc DWORD value which is watched by phone server app
	DWORD dwVal;
	GetKey(REG_KEY_ISS_PATH, REG_CallOptionsChanged, dwVal);
	++dwVal &= 0x0f;
	SetKey(REG_KEY_ISS_PATH, REG_CallOptionsChanged, dwVal);
}

void CObjCallOptionList::CheckCallOptions(void* callerID, DWORD& dwCallPermissions)
{
	CALLERIDINFO* sCallInfo = (CALLERIDINFO*)callerID;

	// allowed by default - can change if a rule removes it
	dwCallPermissions |= CALL_Allow;

	// the option list is in execution order
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeCallingRule* sRule = GetItem(i);
		if(!sRule)
			continue;

		switch(sRule->eCallOptionType)
		{
        case CO_DenyUnknownCallers:
            if(!IsValidOID(sCallInfo->lOid))
            {
                DebugOut(_T("ignoring call because of CO_DenyUnknownCallers call option"));
                dwCallPermissions = CALL_Reject;
                return;
            }
            break;
		case CO_AllowContactNever:
			{
				if(sRule->lOID == sCallInfo->lOid)
				{
					DebugOut(_T("ignoring call because of CO_AllowContactNever call option"));
					dwCallPermissions = CALL_Reject;
					return;
				}
			}
			break;
		case CO_AllowContactAlways:
			{
				if(sRule->lOID == sCallInfo->lOid)
				{
					DebugOut(_T("allowing call because of CO_AllowContactAlways call option"));
					dwCallPermissions |= CALL_Allow;
				}
				//return;
			}
			break;
		case CO_AllowCategoryNever:
			{
				if(IsCallerInCategory(sCallInfo, sRule))
				{
					DebugOut(_T("ignoring call because of CO_AllowCategoryNever call option"));
					dwCallPermissions = CALL_Reject;
					//return;	// is this final??
				}
			}
			break;
		case CO_AllowCategoryAlways:
			{
				if(IsCallerInCategory(sCallInfo, sRule))
				{
					DebugOut(_T("allowing call because of CO_AllowCategoryAlways call option"));
					dwCallPermissions |= CALL_Allow;
				}
			}
			break;
		case CO_RingerOffTime:		
			{
				// get current local time and check rule time
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);
				BOOL bRingerOff = FALSE;
				if(sRule->uiEndHour > sRule->uiStartHour)
				{
					if(sysTime.wHour >= sRule->uiStartHour && 
						sRule->uiEndHour > sysTime.wHour)
						bRingerOff = TRUE;
				}
				else if(sRule->uiEndHour < sRule->uiStartHour)	// time slot passes midnight
				{
					if(sysTime.wHour >= sRule->uiStartHour || 
						sysTime.wHour < sRule->uiEndHour)
						bRingerOff = TRUE;
				}
				if(bRingerOff)
				{
					DebugOut(_T("turning ringer off because of CO_RingerOffTime call option"));
					dwCallPermissions |= CALL_NoRing;
				}
			}
			break;
		case CO_PhoneOffBusy:
			{
				if(CObjStateAndNotifyMgr::GetBusyStatus())
				{
					DebugOut(_T("ignoring call because of CO_PhoneOffBusy call option"));
					dwCallPermissions = CALL_Reject;
				}

			}
			break;
		}
	}
}

BOOL CObjCallOptionList::IsCallerInCategory(void* pInfo, TypeCallingRule* pRule)
{
	CALLERIDINFO* sCallInfo = (CALLERIDINFO*)pInfo;

	if(!IsValidOID(sCallInfo->lOid))
		return FALSE;

	CPoomContacts* oPoom = CPoomContacts::Instance();
	if(!oPoom)
		return FALSE;

	TCHAR szCategories[STRING_LARGE];

	oPoom->GetContactCategories(szCategories, sCallInfo->lOid);
	CIssString* oStr = CIssString::Instance();
	int iFound = oStr->Find(szCategories, pRule->szDesc);
	if(iFound >= 0)
		return TRUE;

	return FALSE;
}

