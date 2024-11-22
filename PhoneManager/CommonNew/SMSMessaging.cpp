#include "SMSMessaging.h"
#include "IssDebug.h"
#include <mapidefs.h>
#include <mapiutil.h>
#include <mapix.h>

//save SMS messages to a file
//http://blogs.msdn.com/windowsmobile/archive/2007/04/23/practical-use-of-mapi.aspx

//mobile secretary
//http://blogs.msdn.com/windowsmobile/archive/2006/10/23/the-mobile-secretary.aspx

enum EnumSMSMessageProps
{
    Enum_ENTRYID = 0,
    Enum_SENDER_NAME, 
    Enum_SUBJECT,
    Enum_MESSAGE_DELIVERY_TIME,
    Enum_MESSAGE_FLAGS,
    NumSMSProps
};

TypeSMSMessage::TypeSMSMessage()
:szNumber(NULL)
,szSubject(NULL)
,bRead(TRUE)
,bReceived(FALSE)
{}

TypeSMSMessage::~TypeSMSMessage()
{
	Clear();
}


void TypeSMSMessage::Clear()
{
	CIssString* oStr = CIssString::Instance();

	// clear everything out
	oStr->Delete(&szNumber);
	oStr->Delete(&szSubject);
}

void TypeSMSMessage::Clone(TypeSMSMessage* sSMS)
{
    CIssString* oStr = CIssString::Instance();
    Clear();
    if(!sSMS)
        return;
    if(!oStr->IsEmpty(sSMS->szNumber))
        szNumber = oStr->CreateAndCopy(sSMS->szNumber);
    if(!oStr->IsEmpty(sSMS->szSubject))
        szSubject = oStr->CreateAndCopy(sSMS->szSubject);
    bRead       = sSMS->bRead;
    bReceived   = sSMS->bReceived;
    ft          = sSMS->ft;
}

CSMSInterface* CSMSInterface::m_Instance = NULL;

CSMSInterface::CSMSInterface(void)
:m_pSession(NULL)
,m_oStr(CIssString::Instance())
{
	//ASSERT(m_Instance == NULL);
	//m_Instance = this;
	Initialize();
}

CSMSInterface::~CSMSInterface(void)
{
	CloseSession();
}

HRESULT CSMSInterface::OpenSession()
{
    HRESULT hr = S_FALSE;

	hr = CloseSession();

    hr = MAPIInitialize(NULL);
    CHR(hr, _T("MAPIInitialize() failed"));

    hr = MAPILogonEx(0, NULL, NULL, 0, (LPMAPISESSION *)&m_pSession);
    CHR(hr, _T("MAPILogonEx() failed"));

Error:
    return hr;
}

HRESULT CSMSInterface::CloseSession()
{
    HRESULT hr = S_FALSE;

    if(!m_pSession)
        return hr;

    hr = m_pSession->Logoff(0, 0, 0);
    m_pSession->Release();
    m_pSession = NULL;
    MAPIUninitialize();

	return hr;
}

CSMSInterface* CSMSInterface::Instance()
{
	if(!m_Instance)
	{
		m_Instance = new CSMSInterface();
	}

	return m_Instance;
}

void CSMSInterface::DeleteInstance()
{
    if(m_Instance)
	    delete m_Instance;
	m_Instance = NULL;
}


BOOL CSMSInterface::Initialize(HWND hWndApp /*= NULL*/)
{
	return TRUE;
}

HRESULT CSMSInterface::GetSMSMessages(CIssVector<TypeSMSMessage> &arrMsgList, int iHowMany, FILETIME ftExpired, EnumSMSFolder eFolder)
{
    HRESULT hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    int iMsgCount = 0;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };


    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (NumSMSProps, sptaMsgFolder) = { NumSMSProps, 
                                                            PR_ENTRYID,
                                                            PR_SENDER_NAME, 
                                                            PR_SUBJECT, 
                                                            PR_MESSAGE_DELIVERY_TIME,
                                                            PR_MESSAGE_FLAGS};


    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));
    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));


    while(TRUE)
    {
        // check to see if this is the SMS store
        if(pMsgStoreSRowSet)
            FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;

        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }
         

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));
        CPHR(pStore, _T("pStore"));

        //Read folder        
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        if(eFolder == Enum_CE_IPM_DRAFTS_ENTRYID)
            rgTag[1] = PR_CE_IPM_DRAFTS_ENTRYID;
        else if(eFolder == Enum_IPM_SENTMAIL_ENTRYID)
            rgTag[1] = PR_IPM_SENTMAIL_ENTRYID;

        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        CPHR(rgprops, _T("rgprops"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));
        
        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));
        
        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));
        
        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            if(pFolderSRowSet)
                FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0) || pFolderSRowSet->aRow[0].lpProps == NULL)
            {
                FreeProws(pFolderSRowSet);
                pFolderSRowSet = NULL;
                break;
            }

            SPropValue *pval = pFolderSRowSet->aRow[0].lpProps; 

            // here we can check if this one goes beyond the time limit
            if(ftExpired.dwHighDateTime || ftExpired.dwLowDateTime)
            {
                FILETIME ftLocal;
                FileTimeToLocalFileTime(&pval[Enum_MESSAGE_DELIVERY_TIME].Value.ft, &ftLocal);

                if(CompareFileTime(&ftExpired, &ftLocal) >= 0)
                {
                    FreeProws (pFolderSRowSet);
                    pFolderSRowSet = NULL; 
                    break;
                }
            }
                            
            // get read status
            BOOL bRead = FALSE;
            ULONG ulFlags = pval[Enum_MESSAGE_FLAGS].Value.ul;
            if(ulFlags & MSGFLAG_READ)
                bRead = TRUE;

            //Read Email filed value, which is correspond with spta
            // get number/name aka address
            LPCTSTR pszAddress; 
            if(eFolder == Enum_CE_IPM_INBOX_ENTRYID)
            {
                pszAddress = pval[Enum_SENDER_NAME].Value.lpszW;
            }
            else
            {
                // try to get an IMessage object
                IMAPITable * pRecipientTable = NULL;
                IMessage *pMsg = NULL;
                ULONG msgtype;

                // get the recipient table
                hr = pStore->OpenEntry(pval->Value.bin.cb, (LPENTRYID)pval->Value.bin.lpb, NULL, 0, &msgtype, (IUnknown**)&pMsg);
                if(hr != S_OK || !pMsg)
                    break;

                hr = pMsg->GetRecipientTable(0, &pRecipientTable);
                if(hr != S_OK || pRecipientTable ==  NULL)
                {
                    pMsg->Release();
                    pMsg = NULL;
                    break;
                }

                SizedSPropTagArray(3,rcols) = {3, {PR_DISPLAY_NAME,PR_EMAIL_ADDRESS,PR_RECIPIENT_TYPE} };
                SRowSet *rsRecipients = NULL;
                while(1)
                {
                    // Free the previous row
                    FreeProws (rsRecipients);
                    rsRecipients = NULL; 

                    hr = pRecipientTable->QueryRows (1, 0, &rsRecipients);
                    if ((hr != S_OK) || (rsRecipients == NULL) || (rsRecipients->cRows == 0) || rsRecipients->aRow[0].lpProps == NULL)
                    {
                        FreeProws (rsRecipients);
                        rsRecipients = NULL; 
                        break;
                    }

                    SPropValue *pRecipientVal = rsRecipients->aRow[0].lpProps; 

                    //LPCTSTR pszRecipientName = pRecipientVal[0].Value.lpszW;    // not seeing this one
                    pszAddress = pRecipientVal[1].Value.lpszW;  // assume 1 for now
                    int a = 0;
                }

                FreeProws(rsRecipients);
                rsRecipients = NULL; 

                pMsg->Release();
                pMsg = NULL;

                pRecipientTable->Release();
                pRecipientTable = NULL;

            }

            // subject
            LPCTSTR pszSubject = pval[Enum_SUBJECT].Value.lpszW;

            // create object and add to the list
            TypeSMSMessage* sMsg = new TypeSMSMessage;
            if(!sMsg)
                break;

            if(pszAddress)
                sMsg->szNumber = m_oStr->CreateAndCopy((TCHAR*)pszAddress);
            if(pszSubject)
                sMsg->szSubject = m_oStr->CreateAndCopy((TCHAR*)pszSubject);

            sMsg->ft = pval[Enum_MESSAGE_DELIVERY_TIME].Value.ft;
            sMsg->bReceived =  (eFolder == Enum_CE_IPM_INBOX_ENTRYID);
            if(sMsg->bReceived)
                sMsg->bRead = bRead;

            hr = arrMsgList.AddElement(sMsg);
            if(hr != S_OK)
            {
                delete sMsg;
                break;
            }
            iMsgCount++;
            // check if we have enough
            if(iHowMany > 0 && iMsgCount >= iHowMany)
                break;
        }
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    return hr;
}

HRESULT CSMSInterface::SetSMSMessagesRead(int iHowMany)
{
    // iHowMany should be how many messages are unread
    // we can stop after processing that amount

    HRESULT hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    int iMsgCount = 0;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (2, sptaMsgFolder) = { 2, PR_ENTRYID, PR_MESSAGE_FLAGS };

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));
    CPHR(pMsgStoresTable, _T("pMsgStoresTable"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        // have we processed enough?
        if(iMsgCount >= iHowMany)
            break;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws(pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws(pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));
        CPHR(pStore, _T("pStore"));

        //Read folder
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));
        
        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));
        
        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            // have we processed enough?
            if(iMsgCount >= iHowMany)
                break;

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0) || !pFolderSRowSet->aRow[0].lpProps)
            {
                FreeProws (pFolderSRowSet);
                pFolderSRowSet = NULL; 
                break;
            }

            SPropValue *pval = pFolderSRowSet->aRow[0].lpProps; 

            ULONG ulFlags = pval[1].Value.ul;
            IMessage *pMsg = NULL;
            ULONG msgtype;

            // get the read status
            if(!(ulFlags & MSGFLAG_READ))
            {
                hr = pStore->OpenEntry(pval->Value.bin.cb, (LPENTRYID)pval->Value.bin.lpb, NULL, MAPI_MODIFY , &msgtype, (IUnknown**)&pMsg);
                if(hr == S_OK)
                {
                    ulFlags |= MSGFLAG_READ;
                    pval[1].Value.ul = ulFlags;
                    hr = pMsg->SetProps(2, pval, NULL);
                    iMsgCount++;
                }
            }
            if(pMsg)
            {
                pMsg->Release();
                pMsg = NULL;
            }

        }
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }
    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    return hr;
}

HRESULT CSMSInterface::SetSMSMessageRead(TypeSMSMessage* sMsg)
{
    HRESULT hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (4, sptaMsgFolder) = { 4, PR_ENTRYID, PR_MESSAGE_FLAGS , PR_SENDER_NAME, PR_MESSAGE_DELIVERY_TIME};

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    BOOL bFound = FALSE;
    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        // are we done yet?
        if(bFound)
            break;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read folder
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));
        
        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));
        
        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            // are we done yet?
            if(bFound)
                break;

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0) || !pFolderSRowSet->aRow[0].lpProps)
            {
                FreeProws (pFolderSRowSet);
                pFolderSRowSet = NULL; 
                break;
            }

            SPropValue *pval = pFolderSRowSet->aRow[0].lpProps; 

            ULONG ulFlags = pval[1].Value.ul;
            IMessage *pMsg = NULL;
            ULONG msgtype;

            // get the read status
            if(!(ulFlags & MSGFLAG_READ))
            {
                hr = pStore->OpenEntry(pval->Value.bin.cb, (LPENTRYID)pval->Value.bin.lpb, NULL, MAPI_MODIFY , &msgtype, (IUnknown**)&pMsg);
                if(hr == S_OK)
                {
                    // see if this one matches ours
                    // if the delivery time and sender match, then this is most certainly the one
                    if(sMsg->ft.dwHighDateTime == pval[3].Value.ft.dwHighDateTime &&
                        sMsg->ft.dwLowDateTime == pval[3].Value.ft.dwLowDateTime &&
                        m_oStr->Compare(sMsg->szNumber, pval[2].Value.lpszW) == 0)
                    {
                        ulFlags |= MSGFLAG_READ;
                        pval[1].Value.ul = ulFlags;
                        hr = pMsg->SetProps(2, pval, NULL);
                        sMsg->bRead = TRUE;
                        bFound = TRUE;
                    }
                }
            }
            if(pMsg)
            {
                pMsg->Release();
                pMsg = NULL;
            }

        }
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    return hr;
}

HRESULT CSMSInterface::CheckMessageRead(CIssVector<TypeSMSMessage>& arrMsgs)
{
    // all we want to do here is set sMsg->bRead and bFound so we can get out of here
    HRESULT hr = S_OK;

    if(arrMsgs.GetSize() == 0)
        return hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (4, sptaMsgFolder) = { 4, PR_ENTRYID, PR_MESSAGE_FLAGS , PR_SENDER_NAME, PR_MESSAGE_DELIVERY_TIME};

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    int iFoundCount = 0;
    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        // are we done yet?
        if(iFoundCount == arrMsgs.GetSize())
            break;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read folder
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));
        
        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));
        
        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            // are we done yet?
            if(iFoundCount == arrMsgs.GetSize())
                break;

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0) || !pFolderSRowSet->aRow[0].lpProps)
            {
                FreeProws (pFolderSRowSet);
                pFolderSRowSet = NULL; 
                break;
            }

            SPropValue *pval = pFolderSRowSet->aRow[0].lpProps; 

            ULONG ulFlags = pval[1].Value.ul;
            IMessage *pMsg = NULL;
            ULONG msgtype;

            // get the read status
            if(ulFlags & MSGFLAG_READ)
            {
                hr = pStore->OpenEntry(pval->Value.bin.cb, (LPENTRYID)pval->Value.bin.lpb, NULL, MAPI_MODIFY , &msgtype, (IUnknown**)&pMsg);
                if(hr == S_OK)
                {
                    for(int i=0; i<arrMsgs.GetSize(); i++)
                    {
                        TypeSMSMessage* sMsg = arrMsgs[i];
                        if(!sMsg || sMsg->bRead)
                            continue;

                        // see if this one matches ours
                        // if the delivery time and sender match, then this is most certainly the one
                        if(sMsg->ft.dwHighDateTime == pval[3].Value.ft.dwHighDateTime &&
                           sMsg->ft.dwLowDateTime == pval[3].Value.ft.dwLowDateTime &&
                           m_oStr->Compare(sMsg->szNumber, pval[2].Value.lpszW) == 0)
                        {
                            sMsg->bRead = TRUE;
                            iFoundCount++;
                            break;
                        }

                    }
                    
                }
            }
            if(pMsg)
            {
                pMsg->Release();
                pMsg = NULL;
            }

        }
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    return hr;
}

HRESULT CSMSInterface::CountReceivedMessages(int& iCount)
{
    HRESULT hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;
    LPSPropValue    rgFolderProps           = NULL;

    iCount = 0;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (2, sptaMsgFolder) = { 2, PR_ENTRYID, PR_CONTENT_COUNT };

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }
 
        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read folder
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        // try to get the item count for this folder...
        if(rgFolderProps)
        {
            MAPIFreeBuffer(rgFolderProps);
            rgFolderProps = NULL;
        }
        hr = pFolder->GetProps((LPSPropTagArray)&sptaMsgFolder,MAPI_UNICODE,&cValues,&rgFolderProps);
        CHR(hr, _T("pFolder->GetProps() failed"));

        iCount = rgFolderProps[1].Value.i;
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    if(rgFolderProps)
    {
        MAPIFreeBuffer(rgFolderProps);
        rgFolderProps = NULL;
    }

    hr = CloseSession();

    return hr;

}

HRESULT CSMSInterface::CountSentMessages(int& iCount)
{
    HRESULT hr;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;
    LPSPropValue    rgFolderProps           = NULL;

    iCount = 0;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (2, sptaMsgFolder) = { 2, PR_ENTRYID, PR_CONTENT_COUNT };

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read drafts folder
        ULONG cValues;
        ULONG rgTag[]={1, PR_CE_IPM_DRAFTS_ENTRYID}; 
        ULONG ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        // try to get the item count for this folder...
        if(rgFolderProps)
        {
            MAPIFreeBuffer(rgFolderProps);
            rgFolderProps = NULL;
        }
        hr = pFolder->GetProps((LPSPropTagArray)&sptaMsgFolder,MAPI_UNICODE,&cValues,&rgFolderProps);
        CHR(hr, _T("pFolder->GetProps() failed"));

        iCount = rgFolderProps[1].Value.i;

        // now Read sent folder
        rgprops=NULL;
        rgTag[1]= PR_IPM_SENTMAIL_ENTRYID; 
        ulObjType = 0;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));
        
        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        // try to get the item count for this folder...
        if(rgFolderProps)
        {
            MAPIFreeBuffer(rgFolderProps);
            rgFolderProps = NULL;
        }
        hr = pFolder->GetProps((LPSPropTagArray)&sptaMsgFolder,MAPI_UNICODE,&cValues,&rgFolderProps);
        CHR(hr, _T("pFolder->GetProps() failed"));

        iCount += rgFolderProps[1].Value.i;
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }
        
    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    if(rgFolderProps)
    {
        MAPIFreeBuffer(rgFolderProps);
        rgFolderProps = NULL;
    }

    hr = CloseSession();

    return hr;
}

HRESULT CSMSInterface::DeleteSMSMessage(TypeSMSMessage* sMsg)
{
    //a bit brute force but the code is too messy to do it in the other function
    if(S_OK == DeleteSMSMessage(sMsg, Enum_CE_IPM_INBOX_ENTRYID))
        return S_OK;

    if(S_OK == DeleteSMSMessage(sMsg, Enum_IPM_SENTMAIL_ENTRYID))
        return S_OK;

    if(S_OK == DeleteSMSMessage(sMsg, Enum_CE_IPM_DRAFTS_ENTRYID))
        return S_OK;

    return E_FAIL;
}

HRESULT CSMSInterface::DeleteSMSMessage(TypeSMSMessage* sMsg, EnumSMSFolder eFolder)
{
    HRESULT hr = E_FAIL;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (4, sptaMsgFolder) = { 4, PR_ENTRYID, PR_MESSAGE_FLAGS , PR_SENDER_NAME, PR_MESSAGE_DELIVERY_TIME};

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));


    BOOL bFound = FALSE;
    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        // are we done yet?
        if(bFound)
            break;
        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read folder
        ULONG cValues;
        ULONG ulObjType = 0;

        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        if(eFolder == Enum_CE_IPM_DRAFTS_ENTRYID)
            rgTag[1] = PR_CE_IPM_DRAFTS_ENTRYID;
        else if(eFolder == Enum_IPM_SENTMAIL_ENTRYID)
            rgTag[1] = PR_IPM_SENTMAIL_ENTRYID;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE,&cValues,&rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));

        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));

        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));

        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            // are we done yet?
            if(bFound)
                break;

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0) || !pFolderSRowSet->aRow[0].lpProps)
            {
                FreeProws (pFolderSRowSet);
                pFolderSRowSet = NULL;
                break;
            }

            SPropValue *pval = pFolderSRowSet->aRow[0].lpProps; 

            ULONG ulFlags = pval[1].Value.ul;
            IMessage *pMsg = NULL;
            ULONG msgtype;

            //open the message
            hr = pStore->OpenEntry(pval->Value.bin.cb, (LPENTRYID)pval->Value.bin.lpb, NULL, MAPI_MODIFY , &msgtype, (IUnknown**)&pMsg);
            if(hr == S_OK)
            {
                // see if this one matches ours
                // if the delivery time and sender match, then this is most certainly the one
                if(sMsg->ft.dwHighDateTime == pval[3].Value.ft.dwHighDateTime &&
                    sMsg->ft.dwLowDateTime == pval[3].Value.ft.dwLowDateTime)
                //    && m_oStr->Compare(sMsg->szNumber, pval[2].Value.lpszW) == 0)
                {
                    //lets move this badboy
                    //make a list of messages to be moved ... obviously just one for now
            
                 //   LPMAPIFOLDER pWasteBasket = NULL;
                 //   10 bucks says I gotta declare memory here first
                 //   hr = GetWastebasketForFolder(m_pSession, pFolder, &pWasteBasket);

                    ENTRYLIST* lst = NULL;
                    ULONG cbNeeded = sizeof(SBinaryArray) + sizeof(SBinary);//I think ... betcha its more than we need .. I can live with that
                    hr = MAPIAllocateBuffer(cbNeeded, (LPVOID*)&lst);
                    if(lst == NULL || hr != S_OK)
                    {
                        if(pMsg)
                            pMsg->Release();
                        pMsg = NULL;
                        break;
                    }
                     
                    lst->cValues = 1;
                    BYTE* pb;
                    pb = (BYTE*)lst;
                    //hard core
                    pb += sizeof(SBinaryArray);
                    lst->lpbin = (SBinary*)pb;
                    
                    lst->lpbin[0].cb = pval->Value.bin.cb;
                    lst->lpbin[0].lpb = pval->Value.bin.lpb;

                    pb += sizeof(SBinary);

                    if(pb > (BYTE*)lst + cbNeeded)
                    {
                        if(pMsg)
                            pMsg->Release();
                        pMsg = NULL;
                        MAPIFreeBuffer(lst);
                        break;
                    }
                    
                    //http://msdn.microsoft.com/en-us/library/bb446204.aspx
                    //IMAPIFolder::CopyMessages
                 /*   hr = pFolder->CopyMessages(lst, //ENTRYLIST - can't be NULL
                                    NULL,//must be NULL
                                    pWasteBasket,//destination folder
                                    0,//must be 0
                                    NULL,//ignored for mobile
                                    MESSAGE_MOVE);*/

                    hr = pFolder->DeleteMessages(lst, 0, 0, 0);

              //      pWasteBasket->Release();

                    MAPIFreeBuffer(lst);

                    bFound = TRUE;
                }
            }
            
            if(pMsg)
            {
                pMsg->Release();
                pMsg = NULL;
            }

        }
    }

Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }

    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }

    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    if(bFound == FALSE)
        hr = E_FAIL;

    return hr;
}

HRESULT CSMSInterface::DeleteAllSMS()
{
    HRESULT hr = E_FAIL;

    hr = DeleteAllSMS(Enum_CE_IPM_INBOX_ENTRYID);
    if(hr != S_OK)
        return hr;

    hr = DeleteAllSMS(Enum_IPM_SENTMAIL_ENTRYID);
    if(hr != S_OK)
        return hr;

    hr = DeleteAllSMS(Enum_CE_IPM_DRAFTS_ENTRYID);

    return hr;
}

HRESULT CSMSInterface::DeleteAllSMS(EnumSMSFolder eFolder)
{
    HRESULT hr = S_OK;

    IMAPITable     * pMsgStoresTable        = NULL;
    IMAPITable     * pFolderContentsTable   = NULL;
    SRowSet        * pMsgStoreSRowSet       = NULL;
    SRowSet        * pFolderSRowSet         = NULL;
    IMsgStore      * pStore                 = NULL;
    IMAPIFolder     *pFolder                = NULL;
    LPSPropValue    rgprops                 = NULL;

    static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
    static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };
    static const SizedSPropTagArray (4, sptaMsgFolder) = { 4, PR_ENTRYID, PR_MESSAGE_FLAGS , PR_SENDER_NAME, PR_MESSAGE_DELIVERY_TIME};

    hr = OpenSession();
    CHR(hr, _T("CSMSInterface::OpenSession() failed"));
    CPHR(m_pSession, _T("m_pSession"));

    hr = m_pSession->GetMsgStoresTable(0, &pMsgStoresTable);
    CHR(hr, _T("GetMsgStoresTable() failed"));

    hr = pMsgStoresTable->SetColumns((SPropTagArray *) &spta, 0);
    CHR(hr, _T("pMsgStoresTable->SetColumns() failed"));

    BOOL bFound = FALSE;
    while(TRUE)
    {
        // check to see if this is the SMS store
        FreeProws (pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;

        // are we done yet?
        if(bFound)
            break;
        hr = pMsgStoresTable->QueryRows (1, 0, &pMsgStoreSRowSet);
        CHR(hr, _T("pMsgStoresTable->QueryRows() failed"));

        if ((hr != S_OK) || (pMsgStoreSRowSet == NULL) || (pMsgStoreSRowSet->cRows == 0) || pMsgStoreSRowSet->aRow[0].cValues != spta.cValues || !pMsgStoreSRowSet->aRow[0].lpProps)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        SPropValue *pval = pMsgStoreSRowSet->aRow[0].lpProps;
        if(pval[0].ulPropTag != PR_DISPLAY_NAME || pval[1].ulPropTag != PR_ENTRYID)
        {
            FreeProws (pMsgStoreSRowSet);
            pMsgStoreSRowSet = NULL;
            break;
        }

        if (_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
            continue;

        if(pStore)
        {
            pStore->Release();
            pStore   = NULL;
        }
        hr = m_pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);
        CHR(hr, _T("OpenMsgStore() failed"));

        //Read folder
        ULONG cValues;
        ULONG ulObjType = 0;

        ULONG rgTag[]={1, PR_CE_IPM_INBOX_ENTRYID}; 
        if(eFolder == Enum_CE_IPM_DRAFTS_ENTRYID)
            rgTag[1] = PR_CE_IPM_DRAFTS_ENTRYID;
        else if(eFolder == Enum_IPM_SENTMAIL_ENTRYID)
            rgTag[1] = PR_IPM_SENTMAIL_ENTRYID;

        if(rgprops)
        {
            MAPIFreeBuffer(rgprops);
            rgprops = NULL;
        }
        hr=pStore->GetProps((LPSPropTagArray)rgTag,MAPI_UNICODE, &cValues, &rgprops);
        CHR(hr, _T("pStore->GetProps() failed"));

        if(pFolder)
        {
            pFolder->Release();
            pFolder   = NULL;
        }
        hr = pStore->OpenEntry(rgprops->Value.bin.cb, (LPENTRYID)rgprops->Value.bin.lpb, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);
        CHR(hr, _T("pStore->OpenEntry() failed"));

        if(pFolderContentsTable)
        {
            pFolderContentsTable->Release();
            pFolderContentsTable   = NULL;
        }
        hr = pFolder->GetContentsTable(0, &pFolderContentsTable);
        CHR(hr, _T("pFolder->GetContentsTable() failed"));

        hr = pFolderContentsTable->SortTable((SSortOrderSet *)&sortOrderSet, 0);
        CHR(hr, _T("pFolderContentsTable->SortTable() failed"));

        hr = pFolderContentsTable->SetColumns ((SPropTagArray *) &sptaMsgFolder, 0); 
        CHR(hr, _T("pFolderContentsTable->SetColumns() failed"));

        // iterate through each message in the table
        while (TRUE)
        {
            // Free the previous row
            FreeProws (pFolderSRowSet);
            pFolderSRowSet = NULL; 

            // are we done yet?
            if(bFound)
                break;

            hr = pFolderContentsTable->QueryRows (1, 0, &pFolderSRowSet);
            if ((hr != S_OK) || (pFolderSRowSet == NULL) || (pFolderSRowSet->cRows == 0))
            {
                FreeProws (pFolderSRowSet);
                pFolderSRowSet = NULL; 
                break;
            }

            ENTRYLIST* pList = NULL;

            ULONG cbNeeded = (sizeof(SBinaryArray) + pFolderSRowSet->cRows * (sizeof(SBinary)));
            //allocate a buffer to hold all of the data
            hr = MAPIAllocateBuffer(cbNeeded, (LPVOID*)&pList);
            CHR(hr, _T("MAPIAllocateFailed"));
       //     ASSERT(pList == NULL);

            //set the number of items in entry list
            pList->cValues = pFolderSRowSet->cRows;
            //set pList->lpbin to the place in the buffer where the array items will be filled
            BYTE* pb;
            pb = (BYTE*)pList;
            pb += sizeof(SBinaryArray);
            pList->lpbin = (SBinary*)pb;//not how I would have done it but fine ... just spacing out the pointer
            //loop through and add all of the messages to delete
            for(int iItems = 0; iItems < (int)pList->cValues; iItems++)
            {
                pList->lpbin[iItems].cb = pFolderSRowSet->aRow[iItems].lpProps[0].Value.bin.cb;
                pList->lpbin[iItems].lpb = pFolderSRowSet->aRow[iItems].lpProps[0].Value.bin.lpb;
                //keep an eye on mem usage
                pb += sizeof(SBinary);
            }

            if(pb <= ((BYTE*)pList + cbNeeded))
            {
                //nuke em!
                hr = pFolder->DeleteMessages(pList, 0, 0, 0);
            }            

            MAPIFreeBuffer(pList);
        }
    }


Error:
    if(pMsgStoresTable)
    {
        pMsgStoresTable->Release();
        pMsgStoresTable   = NULL;
    }

    if(pFolderContentsTable)
    {
        pFolderContentsTable->Release();
        pFolderContentsTable   = NULL;
    }

    if(pMsgStoreSRowSet)
    {
        FreeProws(pMsgStoreSRowSet);
        pMsgStoreSRowSet = NULL;
    }

    if(pFolderSRowSet)
    {
        FreeProws(pFolderSRowSet);
        pFolderSRowSet = NULL;
    }

    if(pStore)
    {
        pStore->Release();
        pStore   = NULL;
    }

    if(pFolder)
    {
        pFolder->Release();
        pFolder   = NULL;
    }
    if(rgprops)
    {
        MAPIFreeBuffer(rgprops);
        rgprops = NULL;
    }

    hr = CloseSession();

    return hr;
}


HRESULT CSMSInterface::GetWastebasketForFolder(IMAPISession *m_pSession, LPMAPIFOLDER pFolder, LPMAPIFOLDER* ppfldrWastebasket)
{    
    HRESULT hr = E_FAIL;    
    
    IMsgStore* pms = NULL;    
    ULONG cItems;    
    ULONG rgtagsFldr[] = { 1, PR_OWN_STORE_ENTRYID };    
    ULONG rgtagsMsgStore[] = { 1, PR_IPM_WASTEBASKET_ENTRYID };    
    LPSPropValue rgprops = NULL;    
    // This method assumes that the CALLER already logged on to a MAPISession    
    if (!m_pSession || !pFolder)        
        CHR(E_FAIL, _T("Session not open or no folder"));      
    
    // Now request the PR_OWN_STORE_ENTRYID on the folder.  This is the    
    // ENTRYID of the message store that owns the folder object.    
    hr = pFolder->GetProps((LPSPropTagArray)rgtagsFldr, MAPI_UNICODE, &cItems, &rgprops);    
    
    CHR(hr, _T("Get Wastebasket Failed"));    
//    CBR(PR_OWN_STORE_ENTRYID == rgprops[0].ulPropTag);    
    if(PR_OWN_STORE_ENTRYID == rgprops[0].ulPropTag)
    {
        goto Error;
    }
    

    // Now open the message store object.    
    hr = m_pSession->OpenEntry(rgprops[0].Value.bin.cb,            
                            (LPENTRYID)rgprops[0].Value.bin.lpb,            
                            NULL, 0, NULL, (LPUNKNOWN*)&pms);   

    CHR(hr, _T("Get Wastebasket Failed"));    
    
    MAPIFreeBuffer(rgprops);    
    rgprops = NULL;    
    
    // Get the ENTRYID of the wastebasket for the message store    
    hr = pms->GetProps((LPSPropTagArray)rgtagsMsgStore, MAPI_UNICODE, &cItems, &rgprops);    
    CHR(hr, _T("Get Wastebasket Failed"));    
    
    // Now open the correct wastebasket and return it to the caller.    
    //CBR(PR_IPM_WASTEBASKET_ENTRYID == rgprops[0].ulPropTag);  
    if(PR_IPM_WASTEBASKET_ENTRYID != rgprops[0].ulPropTag)
        goto Error;

    hr = m_pSession->OpenEntry(rgprops[0].Value.bin.cb,            
                                (LPENTRYID)rgprops[0].Value.bin.lpb,            
                                NULL, 0, NULL, (LPUNKNOWN*)ppfldrWastebasket);    
    
    CHR(hr, _T("Get Wastebasket Failed"));

Error:    
    MAPIFreeBuffer(rgprops);    

    if(pms)
    {
        pms->Release();
        pms   = NULL;
    }
    return hr;
}