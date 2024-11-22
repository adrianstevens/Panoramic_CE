#include "StdAfx.h"
#include "ObjRingtoneRuleList.h"
#include "IssCommon.h"
#include "IssTapi.h"
#include "PoomContacts.h"
#include "IssRegistry.h"
#include "IssString.h"
#include "CommonDefines.h"
#include "soundfile.h"
#include "IssDebug.h"


////////////////////////////////////////////////////////////////////////////////////////
CObjRingtoneRuleList::CObjRingtoneRuleList(void)
{
}

CObjRingtoneRuleList::~CObjRingtoneRuleList(void)
{
	Destroy();
}

BOOL CObjRingtoneRuleList::Destroy()
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypeRingtoneRule* sItem = m_arrItems[i];
		if(sItem)
			delete sItem;
	}
	m_arrItems.RemoveAll();
	return TRUE;
}

TypeRingtoneRule*  CObjRingtoneRuleList::GetItem(int iIndex) 
{
	if(iIndex >= 0 && iIndex < m_arrItems.GetSize())
		return m_arrItems[iIndex];

	return NULL;
}

BOOL CObjRingtoneRuleList::ReloadList()
{
	Destroy();
	FILE *fp;

	DBG_OUT((_T("CObjRingtoneRuleList::ReloadList()")));

	//open the file
	fp = _tfopen(TXT_FileRingtonRulelist, _T("r"));
	if(fp == NULL)
    {
        DBG_OUT((_T("RingtoneRuleList file not found")));
		CheckAddDefaultItem();
		return TRUE;
    }

    //read file stuff
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int iItemSize = sizeof(TypeRingtoneRule);
    if(dwFileSize == 0 || dwFileSize%iItemSize != 0)
    {
        DBG_OUT((_T("RingtoneRulelist file size (%d,%d) is not correct so deleting"), dwFileSize, iItemSize));
        fclose(fp);
        fp = NULL;
        DeleteFile(TXT_FileRingtonRulelist);
        goto Error;
    }

	int iItemsRead = 0;
	int iAmountRead = 0;
	do
	{
		TypeRingtoneRule* sRule = new TypeRingtoneRule;
        if(!sRule)
            break;
		iAmountRead = fread(sRule, iItemSize, 1, fp);
		if(iAmountRead == 1)
		{
			m_arrItems.AddElement(sRule);
			iItemsRead += iAmountRead;
		}
		else
			delete sRule;

	}
	while(iAmountRead);

    DBG_OUT((_T("CObjRingtoneRuleList read %i items"), iItemsRead));

Error:
    // close the file
    if(fp)
        fclose(fp);

    CheckAddDefaultItem();

	return TRUE;
}

void CObjRingtoneRuleList::CheckAddDefaultItem()
{
	TypeRingtoneRule* sRingtone = m_arrItems[0];
	// make sure we have a stored default item
	if(!sRingtone || sRingtone->eRuleType != RR_Default)
	{
		DBG_OUT((_T("CObjRingtoneRuleList::ReloadList() - setting default ringtone")));
		// add a default record to the list
		TypeRingtoneRule* defaultRingtoneRule = new TypeRingtoneRule;
		defaultRingtoneRule->eRuleType = RR_Default;
		defaultRingtoneRule->eRingType = RT_VibrateAndRing;
		defaultRingtoneRule->iOID = 0;
		ZeroMemory(&defaultRingtoneRule->ringToneInfo, sizeof(SNDFILEINFO));
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(defaultRingtoneRule->szDesc, _T("Default ringtone"));

		HRESULT hr;
		SNDFILEINFO	sfRingtone1;		
		hr = SndGetSound(SND_EVENT_RINGTONELINE1, &sfRingtone1);
		if(SUCCEEDED(hr))
		{

			defaultRingtoneRule->ringToneInfo = sfRingtone1;
			InsertItem(defaultRingtoneRule, 0, FALSE);
		}
		else
		{
			SNDFILEINFO* sFileList = NULL;
			int iFileCount = 0;
			hr = SndGetSoundFileList(SND_EVENT_RINGTONELINE1 ,SND_LOCATION_ALL, &sFileList, &iFileCount);
			if(SUCCEEDED(hr) && sFileList && iFileCount > 0)
			{
				// just copy the first one over
				memcpy(&defaultRingtoneRule->ringToneInfo, &sFileList[0], sizeof(SNDFILEINFO));
				InsertItem(defaultRingtoneRule, 0, FALSE);
			}
			else
				delete defaultRingtoneRule;

			if(sFileList)
				LocalFree(sFileList);
		}
	}
}

BOOL CObjRingtoneRuleList::AddItem(TypeRingtoneRule* sItem, BOOL bSaveToReg)
{
	// check list to see if it is already in it...
	BOOL bHaveItem = FALSE;
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeRingtoneRule* sCompareItem = m_arrItems[i];
		if(sItem->eRuleType == RR_Contact && sCompareItem->eRuleType == RR_Contact)
		{
			// check oid
			if(sCompareItem->iOID == sItem->iOID)
				bHaveItem = TRUE;
		}
		else if(sItem->eRuleType == RR_Category && sCompareItem->eRuleType == RR_Category)
		{
			// check category
			if(CIssString::Instance()->Compare(sCompareItem->szDesc, sItem->szDesc) == 0)
				bHaveItem = TRUE;
		}
	}
	if(bHaveItem)
	{
		DebugOut(_T("item %s already in ringtone list"), sItem->szDesc);
		//delete sItem;
		//sItem = NULL;
		return FALSE;
	}

	int iListCount = m_arrItems.GetSize();
	m_arrItems.AddElement(sItem);
	if(++iListCount != m_arrItems.GetSize())
		return FALSE;

	if(IsValidOID(sItem->iOID) && sItem->eRuleType == RR_Contact)
	{
		//first lets get it out of poom
		CPoomContacts*  pOPoom = CPoomContacts::Instance();
		if(pOPoom)
		{
			IContact* pContact = pOPoom->GetContactFromOID(sItem->iOID);
			if(pContact)
			{   //make sure to set poom too
				pOPoom->SetContactRingtone(pContact, &sItem->ringToneInfo);
				pContact->Release();
			}
		}
	}

	if(bSaveToReg)
		return SaveList();
	else
		return TRUE;
}

BOOL CObjRingtoneRuleList::InsertItem(TypeRingtoneRule* sItem, int iPos, BOOL bNotify)
{
	if(iPos > m_arrItems.GetSize() || iPos < 0)
		return FALSE;

	if(!sItem)
		return FALSE;


	m_arrItems.AddElement(sItem);

	// BUGBUG: there is a better way to do this
	/*for(int i = m_arrItems.GetSize()-1; i > iPos; i--)
	{
		TypeRingtoneRule* pTemp = m_arrItems[i];
		m_arrItems.SetElementAt(m_arrItems[i-1], i);
		m_arrItems.SetElementAt(pTemp, i-1);
	}*/
	if(m_arrItems.GetSize()-1 != iPos)
		m_arrItems.Move(m_arrItems.GetSize()-1, iPos);

	return SaveList(bNotify);
}

BOOL CObjRingtoneRuleList::DeleteItem(TypeRingtoneRule* sItem)
{
	int iListCount = m_arrItems.GetSize();

    if(sItem == NULL)
        return FALSE;

	if(IsValidOID(sItem->iOID) && sItem->eRuleType == RR_Contact)
	{
		//first lets get it out of poom
		CPoomContacts*  pOPoom = CPoomContacts::Instance();
		if(pOPoom)
		{
			IContact* pContact = pOPoom->GetContactFromOID(sItem->iOID);
			if(pContact)
			{   //make sure nothing is set in poom too
				pOPoom->RemoveContactRingtone(pContact);
				pContact->Release();
			}
		}
	}

	if(sItem)
		delete sItem;
	m_arrItems.RemoveElement(sItem);
	if(--iListCount != m_arrItems.GetSize())
		return FALSE;

	if(SaveList())
		return TRUE;

	return FALSE;
}

BOOL CObjRingtoneRuleList::SaveList(BOOL bNotify)
{

	DeleteFile(TXT_FileRingtonRulelist);

	FILE* fp;

	//open the file
	if((fp = _tfopen(TXT_FileRingtonRulelist, _T("w"))) == NULL)
		return FALSE;

	int iNumItems = 0;
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		size_t n;
		TypeRingtoneRule* sRule = m_arrItems[i];
        if(!sRule)
            continue;
		n = fwrite(sRule, sizeof(TypeRingtoneRule), 1, fp);
		if(n != 1)
		{
			DBG_OUT((_T("trouble writing file %s"), TXT_FileRingtonRulelist));
		}
		iNumItems += (int)n;
	}

#ifdef DEBUG
	/*DebugOut(_T("CObjRingtoneRuleList wrote %i items %d bytes"), iNumItems, sizeof(TypeRingtoneRule));
	// debug test - get file size
	DWORD dwFileSize = 0;
	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	DebugOut(_T("file size: %d expected: %d"), dwFileSize, iNumItems * sizeof(TypeRingtoneRule));*/
#endif



	fclose(fp);

    if(bNotify)
	    NotifyListChanged();

	return TRUE;
}

BOOL CObjRingtoneRuleList::GetRingtoneRule(void* pCallerID, TypeRingtoneRule* sRingtoneRule)
{
	if(!m_arrItems.GetSize())
		return FALSE;

	CALLERIDINFO* sCallerInfo = (CALLERIDINFO*)pCallerID;
	if(!IsValidOID(sCallerInfo->lOid))
	{
		// use the default
		TypeRingtoneRule* sItem = m_arrItems[0];
		if(!sItem)
			return FALSE;
		ASSERT(sItem->eRuleType == RR_Default);
		CopyMemory(sRingtoneRule, sItem, sizeof(TypeRingtoneRule));
		//DebugOut(_T("using default ringtone"));
		return TRUE;
	}

	//////////////////// 1) check for a category ringtone /////////////////////////
	// need to know if the contact belongs to a category...
	CIssString oStr;
	TCHAR szCategories[STRING_MAX];
	CPoomContacts* pPoom = CPoomContacts::Instance();
	if(!pPoom)
		return 0;

	// now, the contact can belong to more than one category
	// let's check all categories for a rule
	// we'll just use the first one we find
	pPoom->GetContactCategories(szCategories, sCallerInfo->lOid);
	if(oStr.GetLength(szCategories))
	{
		TCHAR* pszCategory;
		pszCategory = oStr.StringToken(szCategories, _T(","));
		do
		{
			oStr.Trim(pszCategory);
			for(int i = 0; i < m_arrItems.GetSize(); i++)
			{
				TypeRingtoneRule* sItem = m_arrItems[i];
				if(sItem && sItem->eRuleType == RR_Category)
				{
					if(oStr.Compare(pszCategory, sItem->szDesc) == 0)
					{
						CopyMemory(sRingtoneRule, sItem, sizeof(TypeRingtoneRule));
						//DebugOut(_T("using category ringtone rule: %s"), pszCategory);
						return TRUE;
					}
				}
			}
			pszCategory = oStr.StringToken(NULL, _T(","));
		}while(pszCategory);
	}

	/////////////////////// 2) check for a contact ringtone //////////////////////////////
	// if there is a ringtone rule for the contact, use it...
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeRingtoneRule* sItem = m_arrItems[i];
		if(sItem && sItem->eRuleType == RR_Contact)
		{
			if(sCallerInfo->lOid == sItem->iOID)
			{
				CopyMemory(sRingtoneRule, sItem, sizeof(TypeRingtoneRule));
				//DebugOut(_T("using contact ringtone rule"));
				return TRUE;
			}
		}
	}

	// otherwise, check if poom has a ringtone for contact
	SNDFILEINFO  sfInfo;
	IContact* pContact = pPoom->GetContactFromOID(sCallerInfo->lOid);
	if(pContact)
	{
		pPoom->GetContactRingtone(pContact, &sfInfo);
		pContact->Release();

		if(sfInfo.sstType == SND_SOUNDTYPE_FILE &&
			oStr.GetLength(sfInfo.szPathName))
		{
			// assemble the TypeRingtoneRule 
			sRingtoneRule->eRuleType = RR_Contact;
			sRingtoneRule->eRingType = m_arrItems[0]->eRingType; // the default
			sRingtoneRule->iOID = sCallerInfo->lOid;
			sRingtoneRule->ringToneInfo = sfInfo;
			DebugOut(_T("using contact ringtone from poom"));
			return TRUE;
		}
	}

	////////////////// 3) get the ISS default ringtone ///////////////////////////////
	
	// use the default
	TypeRingtoneRule* sItem = m_arrItems[0];
	if(!sItem)
		return FALSE;

	ASSERT(sItem->eRuleType == RR_Default);
	CopyMemory(sRingtoneRule, sItem, sizeof(TypeRingtoneRule));
	//DebugOut(_T("using default ringtone"));
	return TRUE;
}

void CObjRingtoneRuleList::NotifyListChanged()
{
	// inc DWORD value which is watched by phone server app
	DWORD dwVal;
	GetKey(REG_KEY_ISS_PATH, REG_RingRuleChanged, dwVal);
	++dwVal &= 0x0f;
	SetKey(REG_KEY_ISS_PATH, REG_RingRuleChanged, dwVal);
}

void CObjRingtoneRuleList::FindAllContactRingtones()
{
    CPoomContacts* pPoom = CPoomContacts::Instance();
	if(!pPoom)
		return;

	BOOL bAdditions = FALSE;
	SNDFILEINFO sFileInfo;
    int iCount = pPoom->GetNumContacts();
    for(int i=0; i<iCount; i++)
    {
		IContact* pContact = pPoom->GetContactFromIndex(i);
		if(!pContact)
			continue;
		
		ZeroMemory(&sFileInfo, sizeof(SNDFILEINFO));

		// see if the contact has a ringtone
		if(pPoom->GetContactRingtone(pContact, &sFileInfo))
		{
			TypeRingtoneRule* sRule = new TypeRingtoneRule;
			if(sRule)
			{
				LONG lOid;
				pContact->get_Oid(&lOid);

				sRule->eRingType	= RT_VibrateAndRing;
				sRule->eRuleType	= RR_Contact;
				sRule->iOID			= lOid;
				sRule->ringToneInfo = sFileInfo;
				pPoom->GetFormatedNameText(sRule->szDesc, pContact, TRUE);

				// add this to our list
				if(!AddItem(sRule, FALSE))
					delete sRule;	
				else
					bAdditions = TRUE;
			}
		}

		pContact->Release();
    }

	if(bAdditions)
		SaveList();
}
