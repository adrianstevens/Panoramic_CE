//#define INITGUID

// *************** stupid ass hack to make fucking COM compile, thanks MS fuckers
#include "wtypes.h"
#include "imaging.h"

const GUID GUID_NULL;
DEFINE_GUID(GUID_NULL, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
// ***************

#include "initguid.h"
#include "PoomContacts.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"
#include "aygshell.h"

#pragma comment( lib,"oleaut32.lib")
#pragma comment( lib,"ole32.lib")
#pragma comment( lib,"pimstore.lib")


int CompareFunc( const void *arg1, const void *arg2 )
{
	TypeContact* sI1	= (TypeContact*)arg1;
	TypeContact* sI2	= (TypeContact*)arg2;

	// we don't really care about equal ones because this is supposed to be unique ID's
	if(sI1->lOid < sI2->lOid)
		return -1;
	else if(sI1->lOid == sI2->lOid)
		return 0;
	else
		return 1;
}



TypeContact::TypeContact()
:szLastName(NULL)
,szFirstName(NULL)
,szHomeNumber(NULL)
,szHomeNumber2(NULL)
,szHomeFax(NULL)
,szMobileNumber(NULL)
,szWorkNumber(NULL)
,szWorkNumber2(NULL)
,szWorkFax(NULL)
,szPager(NULL)
,szCar(NULL)
,szEmailAddr(NULL)
,szEmailAddr2(NULL)
,szEmailAddr3(NULL)
,szCompany(NULL)
,gdiPicture(NULL)
,lOid(0)
{}

TypeContact::~TypeContact()
{
	Clear();
}


void TypeContact::Clear()
{
	CIssString* oStr = CIssString::Instance();

	// clear everything out
	oStr->Delete(&szLastName);
	oStr->Delete(&szFirstName);
	oStr->Delete(&szHomeNumber);
    oStr->Delete(&szHomeNumber2);
    oStr->Delete(&szHomeFax);
	oStr->Delete(&szMobileNumber);
	oStr->Delete(&szWorkNumber);
    oStr->Delete(&szWorkNumber2);
    oStr->Delete(&szWorkFax);
    oStr->Delete(&szPager);
    oStr->Delete(&szCar);
	oStr->Delete(&szEmailAddr);
    oStr->Delete(&szEmailAddr2);
    oStr->Delete(&szEmailAddr3);
	oStr->Delete(&szCompany);
	if(gdiPicture)
	{
		delete gdiPicture;
		gdiPicture = NULL;
	}
	lOid		= 0;
}


CPoomContacts* CPoomContacts::m_Instance = NULL;

CPoomContacts::CPoomContacts(void)
:m_polApp(NULL)
,m_pFolder(NULL)
,m_pContacts(NULL)
,m_pItem(NULL)
//,m_iNumContacts(0)
,m_bInitialized(FALSE)
,m_oStr(CIssString::Instance())
{
	ASSERT(m_Instance == NULL);
	m_Instance = this;
	Initialize();
}

CPoomContacts::CPoomContacts(HWND hWndApp, BOOL bShowSimContacts)
:m_polApp(NULL)
,m_pFolder(NULL)
,m_pContacts(NULL)
,m_pItem(NULL)
,m_bInitialized(FALSE)
,m_oStr(CIssString::Instance())
{
	ASSERT(m_Instance == NULL);
	m_Instance = this;
	Initialize(hWndApp, bShowSimContacts);
}

CPoomContacts::~CPoomContacts(void)
{
	Destroy();
	CoUninitialize();
}

BOOL CPoomContacts::Destroy()
{
	//m_iNumContacts = 0;
	if(m_pItem)
	{
		m_pItem->Release();
		m_pItem = NULL;
	}

	if(m_pContacts)
	{
		m_pContacts->Release();
		m_pContacts = NULL;
	}

	if(m_pFolder)
	{
		m_pFolder->Release();
		m_pFolder = NULL;
	}

	if(m_polApp)
	{
		m_polApp->Logoff();
		m_polApp->Release();
		m_polApp = NULL;
	}

    m_bInitialized = FALSE;

	return TRUE;
}

CPoomContacts* CPoomContacts::Instance()
{
	if(!m_Instance)
	{
		m_Instance = new CPoomContacts();
	}

	return m_Instance;
}

void CPoomContacts::DeleteInstance()
{
	delete m_Instance;
	m_Instance = NULL;
}


BOOL CPoomContacts::Initialize(HWND hWndApp /*= NULL*/, BOOL bShowSimContacts /*=TRUE*/)
{
	if(m_bInitialized)
		return m_bInitialized;

	HRESULT hr;

	// kill off any used values now
	Destroy();

	// this should always be called at least once
	CoInitializeEx(NULL, 0);

	hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_INPROC_SERVER, IID_IPOutlookApp2, (LPVOID *) &(m_polApp));
	if (FAILED(hr))
		return FALSE; 

	// try and log onto the server
	if(m_polApp)
	{
		// Log on to Pocket Outlook
		HRESULT hr = m_polApp->Logon((long)hWndApp);
		if( FAILED( hr ) )
		{
			m_polApp->Release();
			m_polApp = NULL;
			return FALSE;
		}
	} 

	m_polApp->GetDefaultFolder(olFolderContacts, &m_pFolder);
	if(!m_pFolder)
	{
		Destroy();
		return FALSE;
	}

	m_pFolder->get_Items(&m_pContacts);
	if(!m_pContacts)
	{
		Destroy();
		return FALSE;
	}

    
    m_pContacts->QueryInterface(IID_IPOlItems3, (LPVOID *) &m_pItem);
	if(!m_pItem)
	{
        bShowSimContacts = FALSE;
        m_pContacts->QueryInterface(IID_IPOlItems2, (LPVOID *) &m_pItem);
        if(!m_pItem)
        {
		    Destroy();
		    return FALSE;
        }
	}

    // only if we want them
    if(bShowSimContacts)
        m_pItem->IncludeSimContacts();

    // Get the IItem interface for the IFolder.
    IItem     *pFolderItem   = NULL;
    m_pFolder->QueryInterface(IID_IItem, (LPVOID*)&pFolderItem);

    // Set the folder's properties for notifications
    CEPROPVAL propval   = {0};
    propval.propid      = PIMPR_FOLDERNOTIFICATIONS;
    propval.val.ulVal   = PIMFOLDERNOTIFICATION_ALL;
    if(pFolderItem)
    {
        pFolderItem->SetProps(0, 1, &propval);
        pFolderItem->Release();
    }

	// find out how many contacts there are
	//m_pItem->get_Count(&m_iNumContacts);

	m_bInitialized = TRUE;
	return TRUE;
}

// get the contact from the index in the list
IContact* CPoomContacts::GetContactFromIndex(int iIndex)
{
	IContact* pContact = NULL;
	if(!GetNumContacts() || !m_pItem)
		return pContact;

#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
	if(iIndex > GetNumContacts() || iIndex < 0)
		return pContact;

	m_pItem->Item(iIndex, (IDispatch **) &pContact);	
#else
	if(iIndex > GetNumContacts()-1 || iIndex < 0)
		return pContact;

	m_pItem->Item(iIndex+1, (IDispatch **) &pContact);	
#endif

	return pContact;
}

LONG CPoomContacts::GetUserOID(int iIndex)
{
	if(!GetNumContacts() || !m_pItem)
		return FALSE;

	IContact* pContact		= GetContactFromIndex(iIndex);
    if(!pContact)
        return 0;

	LONG		lTestOid	= 0;
	pContact->get_Oid(&lTestOid);
	pContact->Release();
	pContact = NULL;

	return lTestOid;
}

int CPoomContacts::FindContactFromName(TCHAR* szNameGiven)
{
    if(!GetNumContacts() || !m_pItem || m_oStr->IsEmpty(szNameGiven))
        return -1;

    IItem *pItem = NULL;
    IContact *pContact = NULL;
    CEPROPID propidFound = 0;

    HRESULT hr = FindMatchingContact(m_polApp, szNameGiven, FMCF_FINDFILEAS|FMCF_INCLUDESIM, &pItem, &propidFound);
    if(SUCCEEDED(hr) && pItem)
    {
        LONG lOID = 0;
        pItem->get_Oid(&lOID);
        pItem->Release();

        return GetIndexFromOID(lOID);
    }

    /*TCHAR       szName[STRING_MAX] = _T("");
    IContact*	pContact	= NULL;
    LONG		lTestOid	= 0;

    //////////////////////////////////////////////////////////////////////////
    // 

    // search through all the contacts and see if we can find the unique OID
    int iNumContacts = GetNumContacts();
    for(int i=0; i < iNumContacts; i++)
    {
        pContact = NULL;
        m_pItem->Item(i+1, (IDispatch **) &pContact);	// maintain 1-based indexing of IPOlItems
        if(!pContact)
            continue;

        if()
        {
            GetFormatedNameText(szName, pContact);
            pContact->Release();
            pContact = NULL;
            if(0 == m_oStr->Compare(szNameGiven, szName))
            {
                return i+1;
            }            
        }
    }*/

    return -1;
}

int CPoomContacts::FindContactFromNumber(TCHAR* szNumber)
{
	if(!GetNumContacts() || !m_pItem || m_oStr->IsEmpty(szNumber))
		return -1;

	IItem *pItem = NULL;
	IContact *pContact = NULL;
	CEPROPID propidFound = 0;

	HRESULT hr = FindMatchingContact(m_polApp, szNumber, FMCF_FINDPHONE|FMCF_INCLUDESIM, &pItem, &propidFound);
	if(SUCCEEDED(hr) && pItem)
	{
		LONG lOID = 0;
		pItem->get_Oid(&lOID);
		pItem->Release();

		return GetIndexFromOID(lOID);
	}

	return -1;
}

// get the contact info from the unique object id
IContact* CPoomContacts::GetContactFromOID(LONG lOid)
{
	if(!GetNumContacts() || !m_pItem)
		return NULL;

    int iIndex = GetIndexFromOID(lOid);
    if(iIndex == -1)
        return NULL;

    IContact*	pContact	= NULL;
    m_pItem->Item(iIndex, (IDispatch **) &pContact);

    return pContact;

}

int CPoomContacts::GetIndexFromOID(LONG lOid)
{
	if(!GetNumContacts() || !m_pItem)
		return -1;

    int iIndex = -1;
    if(SUCCEEDED(GetItemIndexFromOid(m_pItem, lOid, (DWORD*)&iIndex)) && iIndex != -1)
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
        return iIndex + 1;
#else
        return iIndex;
#endif
    else
        return -1;

	/*IContact*	pContact	= NULL;
	LONG		lTestOid	= 0;

    int iReturnVal = -1;
    BOOL bFound = FALSE;
	// search through all the contacts and see if we can find the unique OID
    int iNumContacts = GetNumContacts();
	for(int i=0; i <= iNumContacts; i++)
	{
		m_pItem->Item(i, (IDispatch **) &pContact);	
		if(pContact)
		{
			pContact->get_Oid(&lTestOid);
			if(lTestOid == lOid)
            {
				iReturnVal =  i;
                bFound = TRUE;
            }
			pContact->Release();
			pContact = NULL;
		}
        if(bFound)
            break;
	}

	return iReturnVal;*/
}


BOOL CPoomContacts::LaunchEditContactScreen(HWND hWnd, int iIndex)
{
	if(!GetNumContacts() || !m_pItem)
		return FALSE;

	IContact*	pContact	= NULL;
	LONG		lTestOid	= 0;

	m_pItem->Item(iIndex, (IDispatch **) &pContact);
    if(!pContact)
        return FALSE;

	// get the IItem interface
	IItem *pContactItem = NULL;
	HRESULT hr = pContact->QueryInterface(IID_IItem, (LPVOID*)&pContactItem);
	if(hr != S_OK)
	{
		pContact->Release();
	}

	// display the contact's Edit dialog
	pContactItem->Edit(hWnd);


	pContactItem->Release();
	//pContact->Display();
    pContact->Release();

	return TRUE;
}


BOOL CPoomContacts::LaunchEditContactScreen(HWND hWnd, LONG lOid)
{
	return LaunchEditContactScreen(hWnd, GetIndexFromOID(lOid));
}

BOOL CPoomContacts::GetContactInfo(IContact *pContact, 
								   TypeContact &sContact, 
								   BOOL bRetrievePicture /*= FALSE*/,
								   int iDesiredPicWidth /*= 0*/,
								   int iDesiredPicHeight /*= 0*/,
								   COLORREF crBackground /*= TRANSPARENT_COLOR*/)
{
	if(!pContact)
		return FALSE;

	BSTR szText = NULL;

    HRESULT hr = S_OK;

	// clear out all the old stuff
	sContact.Clear();

	// get all the contact info and place it into the structure
	hr = pContact->get_Oid(&sContact.lOid);

    sContact.szFirstName = GetUserData(CI_FirstName, pContact);
	sContact.szMiddleName = GetUserData(CI_MiddleName, pContact);
    sContact.szLastName = GetUserData(CI_LastName, pContact);
    sContact.szMobileNumber = GetUserData(CI_MobileNumber, pContact);
    sContact.szHomeNumber = GetUserData(CI_HomeNumber, pContact);
    sContact.szHomeNumber2 = GetUserData(CI_HomeNumber2, pContact);
    sContact.szHomeFax = GetUserData(CI_HomeFax, pContact);
    sContact.szWorkNumber = GetUserData(CI_WorkNumber, pContact);
    sContact.szWorkNumber2 = GetUserData(CI_WorkNumber2, pContact);
    sContact.szWorkFax = GetUserData(CI_WorkFax, pContact);
    sContact.szPager = GetUserData(CI_Pager, pContact);
    sContact.szCar = GetUserData(CI_Car, pContact);
    sContact.szEmailAddr = GetUserData(CI_Email, pContact);
    sContact.szEmailAddr2 = GetUserData(CI_Email2, pContact);
    sContact.szEmailAddr3 = GetUserData(CI_Email3, pContact);
    sContact.szCompany = GetUserData(CI_Company, pContact);

	if(bRetrievePicture)
	{
		sContact.gdiPicture = GetUserPicture(pContact,
											 iDesiredPicWidth,
											 iDesiredPicHeight,
											 crBackground);
	}

	return TRUE;
}

BOOL CPoomContacts::GetContactCategories(TCHAR* pszCategories)
{
	IFolder    * pFolder = NULL;
	IItem * pFolderIItem = NULL;
	CEPROPVAL    * pVals = NULL;
	int           cProps = 1;
	ULONG       cbBuffer = 0;

	CEPROPID rgPropIDs[] = { PIMPR_FOLDER_CATEGORIES  };

    if(!m_polApp)
        return FALSE;

	HANDLE hHeap = GetProcessHeap();
	if(!hHeap)
		return E_FAIL;

	// Get the IFolder object (Contacts, Contacts, Tasks).
	if(SUCCEEDED(m_polApp->GetDefaultFolder(olFolderContacts, &pFolder)))
	{
		// Get the IItem object representing a IFolder object.
		if(SUCCEEDED(pFolder->QueryInterface(IID_IItem, (LPVOID*)&pFolderIItem)))
		{
			// Get the list of categories.
			if(SUCCEEDED(pFolderIItem->GetProps(rgPropIDs, CEDB_ALLOWREALLOC, cProps, &pVals, &cbBuffer, hHeap)))
			{
				// Copy the list of categories for use outside of this function.
				StringCchCopy(pszCategories, /*cchCategories*/STRING_MAX, pVals->val.lpwstr);
			}
		}
	}
	// Free resources.
	HeapFree(hHeap, 0, pVals);
    if(pFolderIItem)
    {
        pFolderIItem->Release();
        pFolderIItem = NULL;
    }
	
    if(pFolder)
    {
        pFolder->Release();
        pFolder = NULL;
    }

	return TRUE;
}

BOOL CPoomContacts::GetContactCategories(TCHAR* pszCategories, long lOid)
{
	IItem * pContactIItem = NULL;
	CEPROPVAL    * pVals = NULL;
	int           cProps = 1;
	ULONG       cbBuffer = 0;

	CEPROPID rgPropIDs[] = { PIMPR_CATEGORIES  };

	HANDLE hHeap = GetProcessHeap();
	if(!hHeap)
		return FALSE;

	IContact* pContact = GetContactFromOID(lOid);
	if(!pContact)
		return FALSE;

	// Get the IItem object representing an IContact object.
	if(SUCCEEDED(pContact->QueryInterface(IID_IItem, (LPVOID*)&pContactIItem)))
	{
		// Get the list of categories.
		if(SUCCEEDED(pContactIItem->GetProps(rgPropIDs, CEDB_ALLOWREALLOC, cProps, &pVals, &cbBuffer, hHeap)))
		{
			// Copy the list of categories for use outside of this function.
			StringCchCopy(pszCategories, /*cchCategories*/STRING_MAX, pVals->val.lpwstr);
		}
	}

	// Free resources.
	HeapFree(hHeap, 0, pVals);
    if(pContactIItem)
    {
	    pContactIItem->Release();
        pContactIItem = NULL;
    }

    if(pContact)
    {
	    pContact->Release();
        pContact = NULL;
    }
	
	return TRUE;
}

BOOL CPoomContacts::SetContactRingtone(IContact* pContact, SNDFILEINFO* pInfo)
{
	IItem *		  pIItem = NULL;
	int           cProps = 1;

    if(!pContact)
        return FALSE;

	CEVALUNION val;
	val.lpwstr = (pInfo->sstType == SND_SOUNDTYPE_NONE) ? NULL : pInfo->szPathName;

	CEPROPVAL    propVals = {0};
	propVals.propid = PIMPR_RINGTONE;
	propVals.val	= val;

	BOOL bResult = FALSE;
	// Get the IItem object representing an IContact object.
	if(SUCCEEDED(pContact->QueryInterface(IID_IItem, (LPVOID*)&pIItem)))
	{
		// Set the props.
		if(SUCCEEDED(pIItem->SetProps(0, cProps, &propVals)))
			if(SUCCEEDED(pIItem->Save()))
				bResult = TRUE;
        if(pIItem)
        {
		    pIItem->Release();
            pIItem = NULL;
        }
	}

	return bResult;
}

BOOL CPoomContacts::RemoveContactRingtone(IContact* pContact)
{
    IItem *		  pIItem = NULL;
    int           cProps = 1;
    BOOL		  bResult = FALSE;

    if(!pContact)
        return FALSE;

    CEVALUNION val;
    val.lpwstr = NULL;//no ring tone dude

    CEPROPVAL    propVals = {0};
    propVals.propid = PIMPR_RINGTONE;
    propVals.val	= val;


    // Get the IItem object representing an IContact object.
    if(SUCCEEDED(pContact->QueryInterface(IID_IItem, (LPVOID*)&pIItem)))
    {
        // Set the props.
        if(SUCCEEDED(pIItem->SetProps(0, cProps, &propVals)))
            if(SUCCEEDED(pIItem->Save()))
                bResult = TRUE;
        if(pIItem)
        {
            pIItem->Release();
            pIItem = NULL;
        }
    }
    return bResult;
}

BOOL CPoomContacts::GetContactRingtone(IContact* pContact, SNDFILEINFO* pInfo)
{
	IItem *		  pIItem = NULL;
	int           cProps = 1;
	CEPROPVAL    * pVals = NULL;
	BOOL		 bResult = FALSE;
	ULONG       cbBuffer = 0;
	CEPROPID rgPropIDs[] = { PIMPR_RINGTONE  };

    if(!pContact)
        return FALSE;

	HANDLE hHeap = GetProcessHeap();
	if(!hHeap)
		return bResult;

	// Get the IItem object representing an IContact object.
	if(SUCCEEDED(pContact->QueryInterface(IID_IItem, (LPVOID*)&pIItem)))
	{
		// Get the props.
		if(SUCCEEDED(pIItem->GetProps(rgPropIDs, CEDB_ALLOWREALLOC, cProps, &pVals, &cbBuffer, hHeap)))
		{
			// fill in the SNDFILEINFO struct
			// the path
			if((pVals->wFlags & CEDB_PROPNOTFOUND) == 0)
			{
				m_oStr->StringCopy(pInfo->szPathName, pVals->val.lpwstr);

				// the friendly name
				int iStart = 1 + m_oStr->FindLastOccurance(pInfo->szPathName, _T("\\"));
				int iEnd = m_oStr->FindLastOccurance(pInfo->szPathName, _T("."));
				if(iEnd < m_oStr->GetLength(pInfo->szPathName) && iStart < iEnd)
				{
					m_oStr->StringCopy(pInfo->szDisplayName, pInfo->szPathName, iStart, iEnd - iStart);
					m_oStr->Trim(pInfo->szDisplayName);
				}
				// the type (at this point, assumes always a file)
				pInfo->sstType = SND_SOUNDTYPE_FILE;
                bResult = TRUE;
			}
			else
			{
				pInfo->sstType = SND_SOUNDTYPE_NONE;
				m_oStr->StringCopy(pInfo->szDisplayName, _T("None"));
				m_oStr->StringCopy(pInfo->szPathName, _T(""));
                bResult = FALSE;
			}			
		}
        if(pIItem)
        {
		    pIItem->Release();
            pIItem = NULL;
        }
	}

	HeapFree(hHeap, 0, pVals);

	return bResult;
}

TCHAR* CPoomContacts::GetUserData(EnumContactInfo eInfo, int iIndex)
{
    IContact* pContact = GetContactFromIndex(iIndex);
    if(!pContact)
        return NULL;
    TCHAR* szValue = GetUserData(eInfo, pContact);

    pContact->Release();
    return szValue;
}

TCHAR* CPoomContacts::GetUserData(EnumContactInfo eInfo, LONG lOid)
{
    IContact* pContact = GetContactFromOID(lOid);
    if(!pContact)
        return NULL;
    TCHAR* szValue = GetUserData(eInfo, pContact);

    pContact->Release();
    return szValue;
}

TCHAR* CPoomContacts::GetUserData(EnumContactInfo eInfo, IContact* pContact)
{
    if(!pContact)
        return NULL;

    BSTR szText			= NULL;
    TCHAR* szReturn		= NULL;

    switch(eInfo)
    {
    case CI_FileAs:
        pContact->get_FileAs(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_FirstName:
        pContact->get_FirstName(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
	case CI_MiddleName:
		pContact->get_MiddleName(&szText);
        m_oStr->Trim(szText);
		if(!m_oStr->IsEmpty(szText))
			szReturn = m_oStr->CreateAndCopy(szText);
		break;
    case CI_LastName:
        //Last Name
        pContact->get_LastName(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_MobileNumber:
    case CI_SMS:
        pContact->get_MobileTelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_HomeNumber:
        pContact->get_HomeTelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_HomeNumber2:
        pContact->get_Home2TelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_HomeFax:
        pContact->get_HomeFaxNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_WorkNumber:
        pContact->get_BusinessTelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_WorkNumber2:
        pContact->get_Business2TelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_WorkFax:
        pContact->get_BusinessFaxNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_Pager:
        pContact->get_PagerNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_Car:
        pContact->get_CarTelephoneNumber(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
        {
            szReturn = m_oStr->CreateAndCopy(szText);
        //    szReturn = MakeStrictPhoneNumber(szReturn);
        }
        break;
    case CI_Email:
        pContact->get_Email1Address(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Email2:
        pContact->get_Email2Address(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Email3:
        pContact->get_Email3Address(&szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Company:
        pContact->get_CompanyName(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Birthday:
        {
            DATE sDate = 0;
            SYSTEMTIME sysTime;
            TCHAR szDate[STRING_MAX] = _T("");
            if(S_OK != pContact->get_Birthday(&sDate) || sDate <= 0)
                break;
           
            if(FALSE == m_polApp->VariantTimeToSystemTime(sDate, &sysTime))
                break;
            // quick check to see if we have a valid value
            SYSTEMTIME sysLocal;
            GetLocalTime(&sysLocal);
            if(sysLocal.wYear < sysTime.wYear)
                break;
            GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &sysTime, NULL, szDate, STRING_LARGE);
            if(!m_oStr->IsEmpty(szDate))
                szReturn = m_oStr->CreateAndCopy(szDate);
        }
        break;
    case CI_Anniversary:
        {
            DATE sDate = 0;
            SYSTEMTIME sysTime;
            TCHAR szDate[STRING_MAX] = _T("");
            if(S_OK != pContact->get_Anniversary(&sDate) || sDate <= 0)
                break;
            if(S_OK != m_polApp->VariantTimeToSystemTime(sDate, &sysTime))
                break;
            // quick check to see if we have a valid value
            SYSTEMTIME sysLocal;
            GetLocalTime(&sysLocal);
            if(sysLocal.wYear < sysTime.wYear)
                break;
            GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &sysTime, NULL, szDate, STRING_LARGE);
            if(!m_oStr->IsEmpty(szDate))
                szReturn = m_oStr->CreateAndCopy(szDate);
        }
        break;
    case CI_Department:
        pContact->get_Department(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_JobTitle:
        pContact->get_JobTitle(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Category:
        pContact->get_Categories(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
    case CI_Webpage:
        pContact->get_WebPage(&szText);
        m_oStr->Trim(szText);
        if(!m_oStr->IsEmpty(szText))
            szReturn = m_oStr->CreateAndCopy(szText);
        break;
	case CI_Notes:
		pContact->get_Body(&szText);
        m_oStr->Trim(szText);
		if(!m_oStr->IsEmpty(szText))
			szReturn = m_oStr->CreateAndCopy(szText);
		break;
    case CI_HomeAddress:
        {
            TCHAR szAddress[STRING_MAX] = _T("");
            pContact->get_HomeAddressStreet(&szText);
            if(!m_oStr->IsEmpty(szText))
                m_oStr->Concatenate(szAddress, szText);
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }
            BSTR szCity = NULL;
            BSTR szState = NULL;
            pContact->get_HomeAddressCity(&szCity);
            pContact->get_HomeAddressState(&szState);
            if(!m_oStr->IsEmpty(szCity) || !m_oStr->IsEmpty(szState))
                m_oStr->Concatenate(szAddress, _T("\n"));
            if(!m_oStr->IsEmpty(szCity))
            {
                m_oStr->Concatenate(szAddress, szCity);
                if(!m_oStr->IsEmpty(szState))
                    m_oStr->Concatenate(szAddress, _T(", "));
            }
            if(szCity)
            {
                SysFreeString(szCity);  szCity = NULL;
            }
            if(!m_oStr->IsEmpty(szState))
                m_oStr->Concatenate(szAddress, szState);
            if(szState)
            {
                SysFreeString(szState);  szState = NULL;
            }

            pContact->get_HomeAddressCountry(&szText);
            if(!m_oStr->IsEmpty(szText))
            {
                m_oStr->Concatenate(szAddress, _T("\n"));
                m_oStr->Concatenate(szAddress, szText);
            }
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }

            pContact->get_HomeAddressPostalCode(&szText);
            if(!m_oStr->IsEmpty(szText))
            {
                m_oStr->Concatenate(szAddress, _T("\n"));
                m_oStr->Concatenate(szAddress, szText);
            }
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }

            m_oStr->Trim(szAddress);

            if(!m_oStr->IsEmpty(szAddress))
                szReturn = m_oStr->CreateAndCopy(szAddress);
        }
        break;
    case CI_BusinessAddress:
        {
            TCHAR szAddress[STRING_MAX] = _T("");
            pContact->get_BusinessAddressStreet(&szText);
            if(!m_oStr->IsEmpty(szText))
                m_oStr->Concatenate(szAddress, szText);
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }
            BSTR szCity = NULL;
            BSTR szState = NULL;
            pContact->get_BusinessAddressCity(&szCity);
            pContact->get_BusinessAddressState(&szState);
            if(!m_oStr->IsEmpty(szCity) || !m_oStr->IsEmpty(szState))
                m_oStr->Concatenate(szAddress, _T("\n"));
            if(!m_oStr->IsEmpty(szCity))
            {
                m_oStr->Concatenate(szAddress, szCity);
                if(!m_oStr->IsEmpty(szState))
                    m_oStr->Concatenate(szAddress, _T(", "));
            }
            if(szCity)
            {
                SysFreeString(szCity);  szCity = NULL;
            }
            if(!m_oStr->IsEmpty(szState))
                m_oStr->Concatenate(szAddress, szState);
            if(szState)
            {
                SysFreeString(szState);  szState = NULL;
            }
            
            pContact->get_BusinessAddressCountry(&szText);
            if(!m_oStr->IsEmpty(szText))
            {
                m_oStr->Concatenate(szAddress, _T("\n"));
                m_oStr->Concatenate(szAddress, szText);
            }
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }
            
            pContact->get_BusinessAddressPostalCode(&szText);
            if(!m_oStr->IsEmpty(szText))
            {
                m_oStr->Concatenate(szAddress, _T("\n"));
                m_oStr->Concatenate(szAddress, szText);
            }
            if(szText)
            {
                SysFreeString(szText);  szText = NULL;
            }
            
            m_oStr->Trim(szAddress);

            if(!m_oStr->IsEmpty(szAddress))
                szReturn = m_oStr->CreateAndCopy(szAddress);
        }
        break;
    default:
        return NULL;
    }

    if(szText)
    {
        SysFreeString(szText);
        szText = NULL;
    }

    return szReturn;
}



TCHAR* CPoomContacts::GetUserName(LONG lOid)
{
	return GetUserName(GetIndexFromOID(lOid));
}

CIssGDIEx* CPoomContacts::GetUserPicture(LONG lOid, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp)
{
	HRESULT   hr;
	IItem*    pItem = NULL;
	IStream*  pStream = NULL;
	ULONG     ulSize;
	CIssGDIEx*  gdiImage = NULL;

    if(!m_polApp)
        return NULL;

	hr = m_polApp->GetItemFromOidEx(lOid, 0, &pItem);
	if(FAILED(hr)) goto Error;

	// Extract the picture from the contact
	hr = pItem->OpenProperty(PIMPR_PICTURE, GENERIC_READ, &pStream);
	if(FAILED(hr)) goto Error;

	if(GetStreamSize(pStream, &ulSize) == FALSE)
		goto Error;

	// In some cases, the property may exist even if there is no picture.
	// Make sure we can access the stream and don't have a 0 byte stream
	if(ulSize == 0)
		goto Error;

	gdiImage = GetBitmapFromStream(pStream, iDesiredWidth, iDesiredHeight, crBackground, bScaleUp);

Error:
	if(pItem) pItem->Release();
	if(pStream) pStream->Release();

	return gdiImage;
}

int	CPoomContacts::GetNumContacts()
{
	int count = 0;
    if(m_pContacts)
		m_pContacts->get_Count(&count);

	return count;
};

CIssGDIEx* CPoomContacts::GetUserPicture(int iIndex, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp)
{
	IContact* pContact	= NULL;

	pContact = GetContactFromIndex(iIndex);
	if(pContact == NULL)
		return NULL;
	CIssGDIEx* gdiPic = GetUserPicture(pContact, iDesiredWidth, iDesiredHeight, crBackground, bScaleUp);
	pContact->Release();
	pContact = NULL;
	return gdiPic;
}

CIssGDIEx* CPoomContacts::GetUserPicture(IContact* pContact, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp)
{
	if(pContact == NULL || iDesiredHeight == 0 || iDesiredWidth == 0)
		return NULL;

	LONG lOid;
	HRESULT hr = pContact->get_Oid(&lOid);
	if(FAILED(hr))
		return NULL;

	return GetUserPicture(lOid, iDesiredWidth, iDesiredHeight, crBackground, bScaleUp);
}

TCHAR* CPoomContacts::GetUserName(int iIndex)
{
	IContact* pContact	= NULL;

	pContact = GetContactFromIndex(iIndex);
	if(!pContact)
		return NULL;

	TCHAR* szReturn = GetUserName(pContact);
	
	pContact->Release();
	pContact = NULL;

	return szReturn;
}

TCHAR* CPoomContacts::GetUserName(IContact* pContact)
{
	TCHAR* szReturn		= NULL;

	if(!pContact)
		return NULL;

	TCHAR* szFirst = GetUserFirstName(pContact);
	TCHAR* szLast  = GetUserLastName(pContact);

	int iLen = 3 + m_oStr->GetLength(szFirst) + m_oStr->GetLength(szLast); //one for the stop and one for the space
	szReturn = new TCHAR[iLen];
	memset(szReturn, 0, sizeof(TCHAR)*iLen);

	m_oStr->StringCopy(szReturn, szFirst);
	if(!m_oStr->IsEmpty(szLast) && !m_oStr->IsEmpty(szFirst))
		m_oStr->Concatenate(szReturn, _T(" "));
	m_oStr->Concatenate(szReturn, szLast);

	/*m_oStr->StringCopy(szReturn, szLast);
	if(m_oStr->GetLength(szReturn) > 0 &&
	m_oStr->GetLength(szFirst) > 0)
	m_oStr->Concatenate(szReturn, _T(", "));
	m_oStr->Concatenate(szReturn, szFirst);*/

	m_oStr->Delete(&szFirst);
	m_oStr->Delete(&szLast);

	return szReturn;
}

BOOL CPoomContacts::SetContact(TypeContact& sContact)
{
	if(m_pFolder == NULL)
		return FALSE;

	IContact* pContact = NULL;
	IPOutlookItemCollection* pItems = NULL;

	m_pFolder->get_Items(&pItems);
	if(pItems == NULL)
		return FALSE;
	
    pItems->Add((IDispatch**)&pContact);
	if(sContact.szFirstName != NULL)
		pContact->put_FirstName(sContact.szFirstName);
	if(sContact.szLastName != NULL)
		pContact->put_LastName(sContact.szLastName);
	if(sContact.szFirstName != NULL)
		pContact->put_Email1Address(sContact.szEmailAddr);
	if(sContact.szLastName != NULL)
		pContact->put_HomeTelephoneNumber(sContact.szHomeNumber);
	if(sContact.szFirstName != NULL)
		pContact->put_BusinessTelephoneNumber(sContact.szWorkNumber);
	if(sContact.szLastName != NULL)
		pContact->put_MobileTelephoneNumber(sContact.szMobileNumber);

    // Save the new contact.
    pContact->Save();

	// find out what the unique id is
	pContact->get_Oid(&sContact.lOid);

	pContact->Release();
    pContact = NULL;
	pItems->Release();
	pItems = NULL;

	return TRUE;
}
    
HRESULT CPoomContacts::CreateNew(HWND hWnd, TCHAR* szPhoneNum)
{
    HRESULT hr = S_OK;
    IPOutlookApp* pOutApp = NULL;
    IFolder* pFolder = NULL;
    IContact* pContact = NULL;
    IPOutlookItemCollection* pContacts = NULL;

    CPHR(hWnd, _T(""));

    hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_INPROC_SERVER, IID_IPOutlookApp, (LPVOID *) &(pOutApp));
    CHR(hr, _T("hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_INPROC_SERVER, IID_IPOutlookApp, (LPVOID *) &(pOutApp));"));

    // try and log onto the server
    hr = pOutApp->Logon((long)hWnd);    
    CHR(hr, _T("hr = m_polApp->Logon((long)hWnd);"));

    hr = pOutApp->GetDefaultFolder(olFolderContacts, &pFolder);
    CHR(hr, _T("hr = pOutApp->GetDefaultFolder(olFolderContacts, &pFolder);"));

    hr = pFolder->get_Items(&pContacts);
    CHR(hr, _T("hr = pFolder->get_Items(&pContacts);"));

	

	//hr = m_pFolder->get_Items(&pItems);
    //CHR(hr, _T("m_pContactsFolder->get_Items()"));
	

    //hr = pOutApp->CreateItem(olContactItem, reinterpret_cast<IDispatch**>(&pContact));
    //CHR(hr, _T("m_polApp->CreateItem()"));

	//   hr = pContact->Display();
 //   CHR(hr, _T("pContact->Display()"));


    hr = pContacts->Add((IDispatch**)&pContact);
    CHR(hr, _T("hr = pContacts->Add((IDispatch**)&pContact);"));

    if(szPhoneNum)
    {
        pContact->put_HomeTelephoneNumber(szPhoneNum);
    }

	hr = pContact->Save();
	CHR(hr, _T("pContact->Save()"));

	long lOid;
	hr = pContact->get_Oid(&lOid);
	CHR(hr, _T("hr = pContact->get_Oid(&lOid);"));


	/*pContact->Release();
	pContact = NULL;*/

	LaunchEditContactScreen(hWnd, lOid);

    // get the IItem interface
    /*IItem *pContactItem = NULL;
    hr = pContact->QueryInterface( IID_IItem, (LPVOID*)&pContactItem );
	CHR(hr, _T("hr = pContact->QueryInterface( IID_IItem, (LPVOID*)&pContactItem );"));

    // display the contact's Edit dialog
    pContactItem->Edit(hWnd);*/

Error:
	/*if(pContactItem)
	{
		pContactItem->Release();
		pContactItem = NULL;
	}*/

    if(pContact)
        pContact->Release();

    if(pContacts)
        pContacts->Release();

    if(pFolder)
        pFolder->Release();

    if(pOutApp)
    {
        pOutApp->Logoff();
        pOutApp->Release();
    }


    return hr;
}

HRESULT CPoomContacts::DeleteContact(LONG lOid)
{
    HRESULT hr = S_OK;

    IContact* pContact = GetContactFromOID(lOid);
    if(pContact)
    {
        hr = pContact->Delete();
        CHR(hr, _T("CPoomContacts::DeleteContact() failed"));
    }

Error:
    if(pContact)
        pContact->Release();
    return hr;
}

// a little less creating and destroying
BOOL CPoomContacts::MakeStrictPhoneNumber(TCHAR* szPhoneNumber, TCHAR* szStrict)
{
	if(m_oStr->IsEmpty(szPhoneNumber))
		return FALSE;
	if(szStrict == NULL)
		return FALSE;
	
	int iLength = 0;

	for(int i = 0; i < m_oStr->GetLength(szPhoneNumber); i++)
	{
		if (iswdigit (szPhoneNumber[i]))
		{
			szStrict[iLength] = szPhoneNumber[i];
			iLength += 1;
		}
	}

	// Terminate the string with NULL.
	szStrict[iLength] = _T('\0');

	if(m_oStr->GetLength(szStrict) < 11)
		m_oStr->Insert(szStrict, _T("1"));
	
	return TRUE;
}

TCHAR* CPoomContacts::MakeStrictPhoneNumber(TCHAR* szPhoneNumber)
{
	if(m_oStr->IsEmpty(szPhoneNumber))
		return szPhoneNumber;
	int iLength = 0;

	for(int i = 0; i < m_oStr->GetLength(szPhoneNumber); i++)
	{
		if (iswdigit (szPhoneNumber[i]))
		{
			szPhoneNumber[iLength] = szPhoneNumber[i];
			iLength += 1;
		}
	}

	// Terminate the string with NULL.
	szPhoneNumber[iLength] = _T('\0');

	if(m_oStr->GetLength(szPhoneNumber) < 11)
	{
		TCHAR szTemp[STRING_LARGE];
		
		m_oStr->StringCopy(szTemp, szPhoneNumber);
		m_oStr->Insert(szTemp, _T("1"));
		m_oStr->Delete(&szPhoneNumber);
		szPhoneNumber = m_oStr->CreateAndCopy(szTemp);
	}

	return szPhoneNumber;
}

BOOL CPoomContacts::GetUserOID(TCHAR* szPhoneNum, long* pOid)
{
	// most of this copy of CPoomContacts::GetUserName(TCHAR* szPhoneNum, TCHAR* szName)

	BOOL bResult = FALSE;
	if(szPhoneNum == NULL || pOid == NULL)
		return bResult;

	if(!GetNumContacts() || !m_pItem)
		return bResult;

	IContact*	pContact	= NULL;
	//LONG		lOid	= 0;

	// I can't imagine the phone numbers will be longer than 32
	TCHAR	szNum[STRING_LARGE];

	MakeStrictPhoneNumber(szPhoneNum, szNum);

	// now we simply have to go through all contacts and check all of the phone 
	// numbers .... I can't imagine this is going to be fast with large
	// contact lists ... we might have to cache some data
	for(int i = 0; i < GetNumContacts(); i++)
	{
		// we'll go through the 3 basic numbers for now ... I'll expand this later
		m_pItem->Item(i, (IDispatch **) &pContact);
		if(pContact)
		{
			if(DoesUserOwnNumber(pContact, szNum))
			{
				if(S_OK == pContact->get_Oid(pOid))
				{
					bResult = TRUE;
				}
				//free it up and call it a day
				pContact->Release();
				pContact = NULL;
				return bResult;
			}

			pContact->Release();
			pContact = NULL;
		}
	}
	return bResult;
}

// we'll have to search through all contacts and compare the phone numbers .... yuck

BOOL CPoomContacts::GetUserName(TCHAR* szPhoneNum, TCHAR* szName)
{
	if(szPhoneNum == NULL || szName == NULL)
		return FALSE;

	if(!GetNumContacts() || !m_pItem)
		return FALSE;

	IContact*	pContact	= NULL;
	LONG		lTestOid	= 0;

	// I can't imagine the phone numbers will be longer than 32
	TCHAR	szNum[STRING_LARGE];
	
	MakeStrictPhoneNumber(szPhoneNum, szNum);

	// now we simply have to go through all contacts and check all of the phone 
	// numbers .... I can't imagine this is going to be fast with large
	// contact lists ... we might have to cache some data
	for(int i = 0; i < GetNumContacts(); i++)
	{
		// we'll go through the 3 basic numbers for now ... I'll expand this later
		m_pItem->Item(i, (IDispatch **) &pContact);
		if(pContact)
		{
			if(DoesUserOwnNumber(pContact, szNum))
			{
				// get the name ... copy it out and call it a day
				// crazy this actually worked
				BSTR	szText	= NULL;
				if(S_OK == pContact->get_FirstName(&szText))//because we behave ourselves
				{
					m_oStr->StringCopy(szName, szText);
					m_oStr->Concatenate(szName, _T(" "));
					SysFreeString(szText);
                    szText = NULL;
				}
				if(S_OK == pContact->get_LastName(&szText))
				{
					m_oStr->Concatenate(szName, szText);
					SysFreeString(szText);
                    szText = NULL;
				}
				//free it up and call it a day
				pContact->Release();
				pContact = NULL;
				return TRUE;
			}

			pContact->Release();
			pContact = NULL;
		}
	}
	return FALSE;
}

/*
	Phone Number Methods:

	We'll check for all 10 but since the query will just fail it 
	should be minimal overhead

	However, we may want to specify the incoming source 
	
	IContact::get_RadioTelephoneNumber 
	IContact::get_BusinessFaxNumber 
	IContact::get_MobileTelephoneNumber 
	IContact::get_PagerNumber 
	IContact::get_BusinessTelephoneNumber 
	IContact::get_HomeTelephoneNumber 
	IContact::get_Home2TelephoneNumber 
	IContact::get_HomeFaxNumber 
	IContact::get_CarTelephoneNumber 
	IContact::get_AssistantTelephoneNumber 
*/

BOOL CPoomContacts::DoesUserOwnNumber(IContact* pContact, TCHAR* szPhoneNum)
{
	if(pContact == NULL)
		return FALSE;

	TCHAR	szTemp[STRING_LARGE];
	BSTR	szText	= NULL;
	TCHAR	szExtractedNum[STRING_LARGE];

	if(S_OK == pContact->get_HomeTelephoneNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}	

	szText	= NULL;

	if(S_OK == pContact->get_MobileTelephoneNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	szText	= NULL;
	
	if(S_OK == pContact->get_BusinessTelephoneNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	szText	= NULL;

	if(S_OK == pContact->get_BusinessFaxNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	szText	= NULL;

	if(S_OK == pContact->get_Home2TelephoneNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	szText	= NULL;
	
	if(S_OK == pContact->get_HomeFaxNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	szText	= NULL;

	if(S_OK == pContact->get_CarTelephoneNumber(&szText))
	{
		m_oStr->StringCopy(szExtractedNum, szText);
		MakeStrictPhoneNumber(szExtractedNum, szTemp);

		if(m_oStr->Compare(szPhoneNum, szTemp) == 0)
		{
			SysFreeString(szText);
            szText = NULL;
			return TRUE;
		}
		SysFreeString(szText);
        szText = NULL;
	}

	return FALSE;
}
// **************************************************************************
// Function Name: GetStreamSize
// 
// Purpose: Given an IStream, returns the size of the stream.  This is needed
//          for streams that do not support the Stat method
//
// Arguments:
//    IN  IStream*  pStream - stream to determine size for
//    OUT ULONG*    pulSize - size of stream
//
// Return Values:
//    HRESULT - S_OK if success, failure code if not
//
// Side Effects:
//    The stream pointer always resets to the beginning
//

BOOL CPoomContacts::GetStreamSize(IStream* pStream, ULONG* pulSize)
{
	HRESULT hr;
	LARGE_INTEGER  li = {0};
	ULARGE_INTEGER uliZero = {0};
	ULARGE_INTEGER uli;

	if(pStream == NULL || pulSize == NULL)
		return FALSE;

	hr = pStream->Seek(li, STREAM_SEEK_END, &uli);
	if(FAILED(hr))
		return FALSE;

	*pulSize = uli.LowPart;
	hr = S_OK;

	if (SUCCEEDED(hr))
	{
		// Move the stream back to the beginning of the file
		hr = pStream->Seek(li, STREAM_SEEK_SET, &uliZero);
	}

	return (BOOL)SUCCEEDED(hr);
}

CIssGDIEx* CPoomContacts::ISSGDIFromImage(IN IImage * pImage, IN COLORREF crBackColor)
{
	HRESULT    hr;
	ImageInfo  ii;
	RECT       rc = { 0 };

	if(pImage == NULL)
		return NULL;

	// Get image width/height
	hr = pImage->GetImageInfo(&ii);
	if(FAILED(hr))
		return NULL;

	CIssGDIEx* gdiImage = new CIssGDIEx;
	if(gdiImage == NULL)
		return NULL;

	rc.right	= ii.Width;
	rc.bottom	= ii.Height;


	if(gdiImage->Create(NULL, rc, FALSE, TRUE, FALSE) != S_OK)
	{
		delete gdiImage;
		return NULL;
	}

	FillRect(gdiImage->GetDC(), rc, crBackColor);

	// Draw into DC/DIB
	hr = pImage->Draw(gdiImage->GetDC(), &rc, NULL);
	if(FAILED(hr))
	{
		delete gdiImage;
		return NULL;
	}

	return gdiImage;
}

CIssGDIEx* CPoomContacts::GetBitmapFromStream(IStream* pStream, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp)
{
	HRESULT hr;
	HBITMAP           hBitmap = NULL;

	IImagingFactory*  pFactory = NULL;
	IImage*           pImage   = NULL;
	IImage*           pThumbnail = NULL;
	ImageInfo         imgInfo = {0};

	if(pStream == NULL)
		return NULL;

	// Use a little imaging help
	hr = CoCreateInstance(CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void**) &pFactory);
	if(FAILED(hr))
		return NULL;

	hr = pFactory->CreateImageFromStream(pStream, &pImage);
	if(FAILED(hr))
		goto Error;

	hr = pImage->GetImageInfo(&imgInfo);
	if(FAILED(hr) || imgInfo.Width == 0 || imgInfo.Height == 0)
		goto Error;

	// Scale to the new size
	ScaleProportional(iDesiredWidth, iDesiredHeight, &imgInfo.Width, &imgInfo.Height, bScaleUp);

	// Get the new image
	hr = pImage->GetThumbnail(imgInfo.Width, imgInfo.Height, &pThumbnail);
	if(FAILED(hr))
		goto Error;

	// Convert this to CIssGDI, our target format
	CIssGDIEx* gdiImage = ISSGDIFromImage(pThumbnail, crBackground);

Error:
	if(pFactory) pFactory->Release();
	if(pImage) pImage->Release();
	if(pThumbnail) pThumbnail->Release();

	return gdiImage;

}

void CPoomContacts::ScaleProportional(UINT uFitToWidth, UINT uFitToHeight, UINT *puWidthToScale, UINT *puHeightToScale, BOOL bScaleUp)
{
	if(puWidthToScale == NULL || puHeightToScale == NULL)
		return;

    // if we plan on scaling up
    if(bScaleUp)
    {
        if(*puWidthToScale < *puHeightToScale)
        {
            *puHeightToScale	= (*puHeightToScale)*uFitToWidth/(*puWidthToScale);
            *puWidthToScale		= uFitToWidth;
        }
        else
        {
            *puWidthToScale		= (*puWidthToScale)*uFitToHeight/(*puHeightToScale);
            *puHeightToScale	= uFitToHeight;		
        }
    }
    else
    {
        // Scale (*puWidthToScale, *puHeightToScale) to fit within (uFitToWidth, uFitToHeight), while
        // maintaining the aspect ratio
        int nScaledWidth = ::MulDiv(*puWidthToScale, uFitToHeight, *puHeightToScale);

        // If we didn't overflow and the scaled width does not exceed bounds
        if (nScaledWidth >= 0 && nScaledWidth <= (int)uFitToWidth)
        {
            *puWidthToScale  = nScaledWidth;
            *puHeightToScale = uFitToHeight;
        }
        else
        {
            *puHeightToScale = ::MulDiv(*puHeightToScale, uFitToWidth, *puWidthToScale);

            // The height *must* be within the bounds [0, uFitToHeight] since we overflowed
            // while fitting to height
            ASSERT(*puHeightToScale >= 0 && *puHeightToScale <= uFitToHeight);

            *puWidthToScale  = uFitToWidth;
        }
    }

	

	return;
}

BOOL CPoomContacts::GetFormatedNameText(TCHAR* szName, LONG lOid, BOOL bLastNameFirst)
{
    IContact* pListItem = GetContactFromOID(lOid);
    if(!pListItem || !szName)
        return FALSE;
    
    BOOL bRet = GetFormatedNameText(szName, pListItem, bLastNameFirst);
    pListItem->Release();
    return bRet;
}

BOOL CPoomContacts::GetFormatedNameText(TCHAR* szName, IContact* pContact, BOOL bLastNameFirst)
{
    if(!pContact || !szName)
        return FALSE;
    
    TCHAR* szLastName	= GetUserData(CI_LastName, pContact);
	TCHAR* szMiddleName = GetUserData(CI_MiddleName, pContact);
    TCHAR* szFirstName	= GetUserData(CI_FirstName, pContact);
    TCHAR* szBusiness   = NULL;
    TCHAR* szFileAs     = NULL;
    if(m_oStr->IsEmpty(szLastName) && m_oStr->IsEmpty(szMiddleName) && m_oStr->IsEmpty(szFirstName))
    {
        // we only do this if we really can't find a name
        szBusiness  = GetUserData(CI_Company, pContact);
        szFileAs    = GetUserData(CI_FileAs, pContact);
    }
    
	BOOL bReturn = GetFormatedNameText(szName, szFirstName, szMiddleName, szLastName, szBusiness, szFileAs, bLastNameFirst);

    m_oStr->Delete(&szLastName);
	m_oStr->Delete(&szMiddleName);
    m_oStr->Delete(&szFirstName);
    m_oStr->Delete(&szBusiness);
    m_oStr->Delete(&szFileAs);

    return bReturn;
}

BOOL CPoomContacts::GetFormatedNameText(TCHAR* szName, 
								TCHAR* szGivenFirstName, 
								TCHAR* szGivenMiddleName, 
								TCHAR* szGivenLastName, 
                                TCHAR* szBusiness,
                                TCHAR* szFileAs,
								BOOL bLastNameFirst)
{
	if(!szName)
		return FALSE;

	m_oStr->Empty(szName);

	m_oStr->Trim(szGivenFirstName);
	m_oStr->Trim(szGivenMiddleName);
	m_oStr->Trim(szGivenLastName);
    m_oStr->Trim(szBusiness);
    m_oStr->Trim(szFileAs);

	int iLenFirst	= m_oStr->GetLength(szGivenFirstName);
	int iLenMid		= m_oStr->GetLength(szGivenMiddleName);
	int iLenLast	= m_oStr->GetLength(szGivenLastName);
    int iLenBusiness= m_oStr->GetLength(szBusiness);
    int iLenFileAs  = m_oStr->GetLength(szFileAs);
	if(!iLenFirst && !iLenMid && !iLenLast && !iLenBusiness && !iLenFileAs)
		return FALSE;

    // if the business name is defined then just use it
    if(!iLenFirst && !iLenMid && !iLenLast && iLenBusiness > 0)
    {
        m_oStr->StringCopy(szName, szBusiness);
        return TRUE;
    }
    else if(!iLenFirst && !iLenMid && !iLenLast && iLenFileAs > 0)
    {
        m_oStr->StringCopy(szName, szFileAs);
        return TRUE;
    }

	if(bLastNameFirst)
	{
		if(iLenLast)
		{
			m_oStr->StringCopy(szName, szGivenLastName);
			if(iLenFirst || iLenMid)
			{
				m_oStr->Concatenate(szName, _T(", "));
				m_oStr->Concatenate(szName, szGivenFirstName);
				if(iLenFirst && iLenMid)
					m_oStr->Concatenate(szName, _T(" "));
				m_oStr->Concatenate(szName, szGivenMiddleName);
			}
		}
		else if(iLenMid)
		{
			m_oStr->StringCopy(szName, szGivenMiddleName);
			if(iLenFirst)
			{
				m_oStr->Concatenate(szName, _T(", "));
				m_oStr->Concatenate(szName, szGivenFirstName);
			}
		}
		else
		{
			m_oStr->StringCopy(szName, szGivenFirstName);
		}
		
	}
	else
	{
		if(iLenFirst)
		{
			m_oStr->StringCopy(szName, szGivenFirstName);
			if(iLenMid)
			{
				m_oStr->Concatenate(szName, _T(" "));
				m_oStr->Concatenate(szName, szGivenMiddleName);
			}
			if(iLenLast)
			{
				m_oStr->Concatenate(szName, _T(" "));
				m_oStr->Concatenate(szName, szGivenLastName);
			}
		}
		else if(iLenMid)
		{
			m_oStr->StringCopy(szName, szGivenMiddleName);
			if(iLenLast)
			{
				m_oStr->Concatenate(szName, _T(" "));
				m_oStr->Concatenate(szName, szGivenLastName);
			}
		}
		else
		{
			m_oStr->StringCopy(szName, szGivenLastName);
		}
	}
	return TRUE;
}

BOOL CPoomContacts::SetContactPicture(LONG lOid)
{
    BOOL bReturn = FALSE;
    TCHAR szPicFilename[MAX_PATH];
    // put up the picture selector
    if(S_OK == SelectPicture(szPicFilename))
    {
        if(S_OK == AddPictureToContact(lOid, szPicFilename))
            bReturn = TRUE;
    }
    return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// SaveFileToStream
//
//  [IN]        pszFileName - pic file to copy over to the ISteram.
//  [IN/OUT]    pStream     - IStream used to set the contacts picture property.
//  [IN]        bResize     - Bool val to signify if the stream should be truncated.
//                            (useful if overwriting an existing picture)
//
//  Saves picture file to an IStream that can be used to upload it to a contact.
//
HRESULT CPoomContacts::SaveFileToStream(LPTSTR pszFileName, IStream *pStream, BOOL bResize)
{
    HRESULT     hr          = S_OK;
    HANDLE      hFile       = 0;
    ULONG       cbWritten   = 0;
    DWORD       dwSizeLow   = 0, 
        dwSizeHigh  = 0, 
        dwError     = 0,
        dwBytes     = 0;
    ULARGE_INTEGER uli      = {0};
    int         i           = 0;
    BYTE    *pAllbyteData   = NULL;

    CBARG(NULL != pszFileName, _T(""));
    CBARG(NULL != pStream, _T(""));

    // Open the file for reading.
    hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) 
    {
        hr = E_INVALIDARG;
        goto Error;
    }

    // Get the file size.
    dwSizeLow = GetFileSize (hFile, & dwSizeHigh);          
    if (dwSizeLow == 0xFFFFFFFF && 
        (dwError = GetLastError()) != NO_ERROR )
    { 
        hr = E_FAIL;
        goto Error;
    }     

    if(bResize)
    {
        // Reset the stream's size (ie truncate the stream size)
        uli.LowPart     = dwSizeLow;
        uli.HighPart    = dwSizeHigh;
        hr = pStream->SetSize(uli); 
        CHR(hr, _T(""));
    }

    pAllbyteData = new BYTE[dwSizeLow];
    CBARG(pAllbyteData, _T(""));    

    if (ReadFile(hFile, pAllbyteData, dwSizeLow, &dwBytes, NULL)) 
    {
        // Write the bytes from the file to the stream
        hr = pStream->Write(pAllbyteData, dwSizeLow, &cbWritten);
        if (cbWritten != dwSizeLow || FAILED(hr))
        {
            DBG_OUT((TEXT("Wrote %u of %u bytes to stream"), cbWritten, dwSizeLow));
            goto Error;
        }        
    }
    else
    {
        DBG_OUT((TEXT("Read from file failed!")));
        hr = E_FAIL;
        goto Error;
    }        

    // Commit the steam to the contact object.
    hr = pStream->Commit(0);

Error:
    delete[] (pAllbyteData);

    if(hFile != INVALID_HANDLE_VALUE && hFile != 0)
    {
        CloseHandle(hFile);
    }
    return hr;    
}

HRESULT CPoomContacts::AddPictureToContact(LONG lOid, LPTSTR pszPictureFile)
{
    HRESULT     hr          = S_OK;
    IStream     *pStream    = NULL;

    if(!m_polApp)
        return E_FAIL;

    IItem* pItem = NULL;
    if(S_OK != m_polApp->GetItemFromOidEx(lOid, 0, &pItem))
        return E_FAIL;

    CBARG(NULL != pItem, _T(""));
    CBARG(NULL != pszPictureFile, _T(""));

    // Open the picture property for the contact.
    hr = pItem->OpenProperty(PIMPR_PICTURE, GENERIC_READ | GENERIC_WRITE, &pStream);
    CHR(hr, _T(""));
    CBARG(NULL != pStream, _T(""));

    // Add the picture to the contact's picture property
    // NOTE: Ideally you would want to scale the picture file to a smaller thumbnail
    // so you don't waste storage space (in the database) when adding a huge picture. 
    // This has intentionally not been done here as it is out of the scope of this sample.
    hr = SaveFileToStream(pszPictureFile, pStream, TRUE);
    CHR(hr, _T(""));

    // Save the contact (with the picture added), to the underlying store.
    hr = pItem->Save();
    CHR(hr, _T(""));

Error:
    if(pStream)
        pStream->Release();

    if(pItem)
        pItem->Release();

    return hr;
}


HRESULT CPoomContacts::SelectPicture(TCHAR* szPictureFile)
{
    HRESULT         hr      = S_OK;
    OPENFILENAMEEX  ofnex   = {0};
    BOOL            bResult = FALSE;

    CPHR(szPictureFile, _T("pszPictureFile is NULL"));

    // Setup the Picture picker structure
    ofnex.lStructSize   = sizeof(ofnex);
    ofnex.ExFlags       = OFN_EXFLAG_THUMBNAILVIEW;    
    ofnex.lpstrFile     = szPictureFile;
    ofnex.nMaxFile      = MAX_PATH;

    // Call the Picture picker UI. Lets the user select a pic.
    bResult = GetOpenFileNameEx(&ofnex);
    CBHR(bResult, _T("GetOpenFileNameEx failed"));

Error:
    return hr;    
}
