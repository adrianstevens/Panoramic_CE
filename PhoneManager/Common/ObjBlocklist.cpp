#include "ObjBlocklist.h"
#include "IssTapi.h"
#include "IssRegistry.h"
#include "CommonDefines.h"
#include "IssDebug.h"



TypeBlocklist::TypeBlocklist()
:eType(BlocklistItem_contact)
,lOid(0)
{
    ZeroMemory(szID, STRING_MAX*sizeof(TCHAR));
}

TypeBlocklist::~TypeBlocklist()
{
}

CObjBlocklist::CObjBlocklist()
:m_oStr(CIssString::Instance())
{

}

CObjBlocklist::~CObjBlocklist()
{
	Destroy();
}

BOOL CObjBlocklist::Destroy()
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypeBlocklist* sItem = m_arrItems[i];
		if(sItem)
			delete sItem;
	}
	m_arrItems.RemoveAll();
	return TRUE;
}

BOOL CObjBlocklist::ReloadList()
{
	Destroy();
	FILE *fp;

    DBG_OUT((_T("CObjBlocklist::ReloadList()")));

	//open the file
	if((fp = _tfopen(TXT_FileBlocklist, _T("r"))) == NULL)
    {
        DBG_OUT((_T("BlockList file not found")));
        return FALSE;
    }

	//read file stuff
	DWORD dwFileSize = 0;
	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

    int iItemSize = sizeof(TypeBlocklist);
    if(dwFileSize == 0 || dwFileSize%iItemSize != 0)
    {
        DBG_OUT((_T("BlockList file size is not correct so deleting")));
        fclose(fp);
        DeleteFile(TXT_FileBlocklist);
        return FALSE;
    }

    int iItemsRead = 0;
    int iAmountRead = 0;
    
    do
    {
        TypeBlocklist* sNewItem = new TypeBlocklist;
        iAmountRead = fread(sNewItem, iItemSize, 1, fp);
        if(iAmountRead == 1)
        {
            DBG_OUT((_T("Blocklist item - %s"), sNewItem->szID));
            if(S_OK != m_arrItems.AddSortedElement(sNewItem, CompareItems))
            {
                delete sNewItem;
                continue;
            }
            iItemsRead += iAmountRead;
        }
        else
            delete sNewItem;

    }while(iAmountRead);

	fclose(fp);

	return TRUE;
}

BOOL CObjBlocklist::AddItem(TypeBlocklist* sItem)
{
    DBG_OUT((_T("CObjBlocklist::AddItem")));
    if(sItem->eType == BlocklistItem_contact && !IsValidOID(sItem->lOid))
    {
        DBG_OUT((_T("Failed: Contact without a valid OID")));
        return FALSE;
    }
    else if (m_oStr->IsEmpty(sItem->szID))
    {
        DBG_OUT((_T("Failed: string is invalid")));
        return FALSE;
    }

	// check list to see if it is already in it...
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeBlocklist* sCompareItem = m_arrItems[i];
        if(!sCompareItem)
            continue;

        if(sCompareItem->eType == sItem->eType)
        {
            if(sCompareItem->eType == BlocklistItem_contact)
            {
                if(sItem->lOid == sCompareItem->lOid)
                {
                    DBG_OUT((_T("Duplicate item found")));
                    return FALSE;
                }
            }
            else if(m_oStr->Compare(sCompareItem->szID, sItem->szID) == 0)
            {
                DBG_OUT((_T("Duplicate item found")));
			    return FALSE;
            }
        }
	}

	int iListCount = m_arrItems.GetSize();
	m_arrItems.AddElement(sItem);
	if(++iListCount != m_arrItems.GetSize())
		return FALSE;

	if(SaveList())
		return TRUE;

	return FALSE;
}

BOOL CObjBlocklist::DeleteItem(TypeBlocklist* sItem)
{
	if(!sItem)
		return FALSE;

	int iListCount = m_arrItems.GetSize();

	m_arrItems.RemoveElement(sItem);
	if(--iListCount != m_arrItems.GetSize())
		return FALSE;

	delete sItem;

	if(SaveList())
		return TRUE;

	return FALSE;
}

BOOL CObjBlocklist::SaveList()
{

	DeleteFile(TXT_FileBlocklist);

	FILE* fp;

	//open the file
	if((fp = _tfopen(TXT_FileBlocklist, _T("w"))) == NULL)
		return FALSE;

    int iNumItems = 0;
    for(int i=0; i<m_arrItems.GetSize(); i++)
    {
        size_t n;
        TypeBlocklist* sBlock = m_arrItems[i];
        n = fwrite(sBlock, sizeof(TypeBlocklist), 1, fp);
        if(n != 1)
        {
            DebugOut(_T("trouble writing file %s"), TXT_FileBlocklist);
        }
        iNumItems += (int)n;
    }

	fclose(fp);
	NotifyListChanged();

	return TRUE;
}

BOOL CObjBlocklist::FindItem(TypeBlocklist* sItem)
{
	TypeBlocklist* sItemFound = (TypeBlocklist*)m_arrItems.SearchSortedArray(sItem, CompareItems);
	return (sItemFound?TRUE:FALSE);
}

TypeBlocklist* CObjBlocklist::FindItem(TCHAR* szID)
{
    if(m_oStr->IsEmpty(szID))
        return NULL;

    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        TypeBlocklist* sItem = m_arrItems[i];
        if(!sItem)
            continue;

        if(0 == m_oStr->Compare(szID, sItem->szID))
            return sItem;
    }
    return NULL;
}

TypeBlocklist* CObjBlocklist::FindItem(long lOID)
{
    if(!IsValidOID(lOID))
        return NULL;

    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        TypeBlocklist* sItem = m_arrItems[i];
        if(!sItem)
            continue;

        if(sItem->eType == BlocklistItem_contact && sItem->lOid == lOID)
            return sItem;
    }
    return NULL;
}

BOOL CObjBlocklist::CheckBlock(void* pInfo)
{
    if(!pInfo)
        return FALSE;

	CALLERIDINFO* sCallerInfo = (CALLERIDINFO*)pInfo;
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		TypeBlocklist* sItem = m_arrItems[i];
		if(sItem)
		{
			if(sItem->eType == BlocklistItem_contact)
			{
                if(sItem->lOid == sCallerInfo->lOid)
                    return TRUE;
			}
			else if(sItem->eType == BlocklistItem_phoneAreaCode)
			{
				if(ComparePhonePrefix(sCallerInfo->szNumber, sItem->szID))
					return TRUE;
			}
			else if(sItem->eType == BlocklistItem_phonePrefix)
			{
				if(ComparePhonePrefix(sCallerInfo->szNumber, sItem->szID))
					return TRUE;
			}
			else if(sItem->eType == BlocklistItem_phoneNumber)
			{
				if(ComparePhoneNumbers(sCallerInfo->szNumber, sItem->szID)) 
					return TRUE;
			}
		}
	}

	return FALSE;
}
// return TRUE if phone numbers are the same - ignore leading '1'
BOOL CObjBlocklist::ComparePhoneNumbers(TCHAR* szPhone1, TCHAR* szPhone2)
{
	TCHAR szPhone1Numeric[STRING_LARGE];
	TCHAR szPhone2Numeric[STRING_LARGE];

	m_oStr->StringCopy(szPhone1Numeric, szPhone1);
	m_oStr->StringCopy(szPhone2Numeric, szPhone2);

	MakeNumeric(szPhone1Numeric);
	MakeNumeric(szPhone2Numeric);

	TCHAR* pszPhone1 = szPhone1Numeric;
	TCHAR* pszPhone2 = szPhone2Numeric;

	// remove leading '1'
	if(pszPhone1[0] == _T('1'))
		pszPhone1++;
	if(pszPhone2[0] == _T('1'))
		pszPhone2++;

	return (m_oStr->Compare(pszPhone1, pszPhone2) == 0);
}

// return TRUE if phone number contains phone prefix - ignore leading '1'
BOOL CObjBlocklist::ComparePhonePrefix(TCHAR* szPhone, TCHAR* szPrefix)
{
	TCHAR szTestPhone[STRING_LARGE];
	TCHAR szTestPrefix[STRING_LARGE];

	m_oStr->StringCopy(szTestPhone, szPhone);
	m_oStr->StringCopy(szTestPrefix, szPrefix);

	MakeNumeric(szTestPhone);
	MakeNumeric(szTestPrefix);

	TCHAR* pszStr1 = szTestPhone;
	TCHAR* pszStr2 = szTestPrefix;

	// remove leading '1'
	if(pszStr1[0] == _T('1'))
		pszStr1++;
	if(pszStr2[0] == _T('1'))
		pszStr2++;

	// make equal length
	const int iLength = m_oStr->GetLength(pszStr2);
	pszStr1[iLength] = _T('\0');

	return (m_oStr->Compare(pszStr1, pszStr2) == 0);
}

void CObjBlocklist::MakeNumeric(TCHAR* szString)
{
	int iLength = 0;
	for(int i = 0; i < m_oStr->GetLength(szString); i++)
	{
		if (iswdigit (szString[i]))
			szString[iLength++] = szString[i];
	}

	// Terminate the string with NULL.
	szString[iLength] = _T('\0');
}

int CObjBlocklist::CompareItems(const void* lp1, const void* lp2)
{
	TypeBlocklist* sBlock1	= (TypeBlocklist*)lp1;
	TypeBlocklist* sBlock2	= (TypeBlocklist*)lp2;

	CIssString* oStr = CIssString::Instance();

	if(!sBlock1 || !sBlock2 || oStr->IsEmpty(sBlock1->szID) || oStr->IsEmpty(sBlock2->szID))
		return -1;

    if(sBlock1->eType == sBlock2->eType)
    {
        if(sBlock1->eType == BlocklistItem_contact)
            return sBlock1->lOid - sBlock2->lOid;
    }
    
    return oStr->Compare(sBlock1->szID, sBlock2->szID);
}

void CObjBlocklist::NotifyListChanged()
{
	// inc DWORD value which is watched by phone server app
	DWORD dwVal;
	GetKey(REG_KEY_ISS_PATH, REG_BlockListChanged, dwVal);
	++dwVal &= 0x0f;
	SetKey(REG_KEY_ISS_PATH, REG_BlockListChanged, dwVal);
}
