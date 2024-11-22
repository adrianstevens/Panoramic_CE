#include "IssPicLoader.h"
#include "IssCommon.h"
#include "issdebug.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"

#define     TEMP_Dir    _T("Temp\\")

CIssPicLoader* g_objPicDLInstance = NULL;

TypePLHolder::TypePLHolder()
:bIsDeleted(FALSE)
,gdiImage(NULL)
{}

TypePLItem::TypePLItem()
:szURL(NULL)
,szFilename(NULL)
,uiImage(0)
,pfnLoadPic(NULL)
,gdi(NULL)
,psPicHolder(NULL)
,hWndNotif(NULL)
,uiMessage(0)
,lpClass(NULL)
,lpItem(NULL)
,ePicState(PL_InQueue)
{
    sizeImage.cx = 0;
    sizeImage.cy = 0;
}

TypePLItem::~TypePLItem()
{
    Destroy();
}

void TypePLItem::Destroy()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szURL);
    oStr->Delete(&szFilename);
    
	if(gdi)
		delete gdi;
	gdi         = NULL;
    uiImage     = 0;
    pfnLoadPic  = NULL;
    psPicHolder = NULL;
    hWndNotif   = NULL;
    uiMessage   = 0;
    ePicState   = PL_InQueue;
    sizeImage.cx = 0;
    sizeImage.cy = 0;
    lpClass     = NULL;
    lpItem      = NULL;
  
}

CIssPicLoader::CIssPicLoader(void)
:m_oStr(CIssString::Instance())
,m_bKillThreads(FALSE)
,m_hWndParent(NULL)
,m_hInst(NULL)
{
    g_objPicDLInstance = this;
    ClearAllTempFiles();
    InitializeCriticalSection(&m_crQueue);
    InitializeCriticalSection(&m_crPicHolder);
    InitializeCriticalSection(&m_crConnection);

    for(int i=0; i<NUM_Threads; i++)
    {
        m_hThreads[i]= NULL;
        m_hEvents[i] = NULL;
        m_bProcessing[i] = FALSE;
    }

    m_dwLastPost = GetTickCount();
}

CIssPicLoader::~CIssPicLoader(void)
{
    Destroy();
    DeleteCriticalSection(&m_crQueue);
    DeleteCriticalSection(&m_crPicHolder);
    DeleteCriticalSection(&m_crConnection);
}

void CIssPicLoader::Destroy()
{
    DestroyThreads();
    DeletePicArray(m_arrPics);
    DeletePicArray(m_arrQueue);
    ClearAllTempFiles();

    for(int i=0; i<m_arrPicHolder.GetSize(); i++)
    {
        TypePLHolder* psHolder = m_arrPicHolder[i];
        if(!psHolder)
            continue;
        delete psHolder;
    }
    m_arrPicHolder.RemoveAll();
}

void CIssPicLoader::DestroyThreads()
{
    SetThreadPriorities(THREAD_PRIORITY_HIGHEST);

    m_bKillThreads = TRUE;
    for(int i = 0; i<NUM_Threads; i++)
    {
        if(m_hThreads[i])
        {
            DWORD dwExitCode = STILL_ACTIVE;
            int iCount = 0;
            //PulseEvent(m_hEvents[i]);
            SetEvent(m_hEvents[i]);
            Sleep(5);

            // give it up to 2 seconds to turn itself off
            while(dwExitCode == STILL_ACTIVE && iCount++ < 4)
            {
                if(!GetExitCodeThread(m_hThreads[i], &dwExitCode))
                    break;

                if(dwExitCode != STILL_ACTIVE)
                    break;

                Sleep(500);
            }
            if(iCount >= 4)
                TerminateThread(m_hThreads[i], 999);

            // memory cleanup
            CloseHandle( m_hThreads[i] );
            m_hThreads[i] = NULL;
        }

        if(m_hEvents[i])
        {
            CloseHandle(m_hEvents[i]);
            m_hEvents[i] = NULL;
        }
        m_bProcessing[i] = FALSE;
    }

    m_bKillThreads= FALSE;
}

BOOL CIssPicLoader::IsEngineStarted()
{
    return m_hEvents[0]&&m_hThreads[0]?TRUE:FALSE;
}

HRESULT CIssPicLoader::StartEngine(HWND hWndParent, HINSTANCE hInst)
{
    HRESULT hr = S_OK;

    m_hWndParent = hWndParent;
    m_hInst      = hInst;
    DestroyThreads();

    for(int i=0; i<NUM_Threads; i++)
    {
        m_hEvents[i]    = CreateEvent(NULL, TRUE, FALSE, NULL);
        CPHR(m_hEvents[i], _T("m_hEvents[i]    = CreateEvent(NULL, FALSE, FALSE, NULL)"));

        m_hThreads[i]   = CreateThread(NULL, 0, &ThreadPicLoad, (LPVOID)i, CREATE_SUSPENDED, NULL);
        CPHR(m_hThreads[i], _T("m_hThreads[i]   = CreateThread(NULL, 0, &ThreadLoadImage, (LPVOID)this, CREATE_SUSPENDED, NULL)"));

        // make sure it runs in the background
        //SetThreadPriority(m_hThreads[i], THREAD_PRIORITY_BELOW_NORMAL);
		SetThreadPriority(m_hThreads[i], THREAD_PRIORITY_ABOVE_NORMAL);
        ResumeThread(m_hThreads[i]);
        Sleep(10);
    }

    Sleep(300);

	for(int i=0; i<NUM_Threads; i++)
	{
		if(!m_hThreads[i])
			continue;

		// make sure it runs in the background
		SetThreadPriority(m_hThreads[i], THREAD_PRIORITY_BELOW_NORMAL);
	}

Error:
    return hr;
}

void CIssPicLoader::SetThreadPriorities(int iPriority)
{
    for(int i=0; i<NUM_Threads; i++)
    {
        if(m_hThreads[i])
            SetThreadPriority(m_hThreads[i], iPriority);
    }
}

TypePLHolder* CIssPicLoader::AddPicToQueue(TCHAR*    szURL,
                                           TCHAR*    szFilename,
                                           HWND      hWndNotif /*= NULL*/,
                                           UINT      uiMessage /*= 0*/,
                                           int       iSizeX /*= 0*/,
                                           int       iSizeY /*= 0*/)
{
    HRESULT hr = S_OK;
    TypePLItem* sNew = NULL;
    TypePLHolder* sNewHolder = NULL;    

    CBARG(m_oStr->GetLength(szFilename) > 0 && m_oStr->GetLength(szURL) > 0, _T(""));

    sNewHolder = new TypePLHolder;
    CPHR(sNewHolder, _T("sNewHolder = new TypePLHolder;"));

	EnterCriticalSection(&m_crPicHolder);
    hr = m_arrPicHolder.AddElement(sNewHolder);
    LeaveCriticalSection(&m_crPicHolder);
    CHR(hr, _T("hr = m_arrPicHolder.AddElement(sNewHolder);"));

    sNew = new TypePLItem;
    CPHR(sNew, _T("TypePLItem* sNew = new TypePLItem"));	

    sNew->szURL         = m_oStr->CreateAndCopy(szURL);
    CPHR(sNew->szURL, _T("sNew->szURL = m_oStr->CreateAndCopy(szURL)"));
    sNew->szFilename    = m_oStr->CreateAndCopy(szFilename);
    CPHR(sNew->szFilename, _T("sNew->szFilename = m_oStr->CreateAndCopy(szFilename)"));

    sNew->psPicHolder = sNewHolder;
    sNew->hWndNotif = hWndNotif;
    sNew->uiMessage = uiMessage;
    sNew->sizeImage.cx = iSizeX;
    sNew->sizeImage.cy = iSizeY;

    EnterCriticalSection(&m_crQueue);
    hr = m_arrQueue.AddElement(sNew);
    LeaveCriticalSection(&m_crQueue);
    CHR(hr, _T("hr = m_arrQueue.AddElement(sNew)"));

    UpdateQueue();

Error:
    if(hr != S_OK && sNew)
        delete sNew;
	return (hr==S_OK?sNewHolder:NULL);
}

TypePLHolder* CIssPicLoader::AddPicToQueue(TCHAR*    szFilename,
                                           HWND      hWndNotif /*= NULL*/,
                                           UINT      uiMessage /*= 0*/,
                                           int       iSizeX /*= 0*/,
                                           int       iSizeY /*= 0*/)
{
    HRESULT hr = S_OK;
    TypePLItem* sNew = NULL;
    TypePLHolder* sNewHolder = NULL;    

    CBARG(m_oStr->GetLength(szFilename) > 0, _T(""));

    sNewHolder = new TypePLHolder;
    CPHR(sNewHolder, _T("sNewHolder = new TypePLHolder;"));

    EnterCriticalSection(&m_crPicHolder);
    hr = m_arrPicHolder.AddElement(sNewHolder);
    LeaveCriticalSection(&m_crPicHolder);
    CHR(hr, _T("hr = m_arrPicHolder.AddElement(sNewHolder);"));

    sNew = new TypePLItem;
    CPHR(sNew, _T("TypePLItem* sNew = new TypePLItem"));	

    sNew->szFilename    = m_oStr->CreateAndCopy(szFilename);
    CPHR(sNew->szFilename, _T("sNew->szFilename = m_oStr->CreateAndCopy(szFilename)"));

    sNew->psPicHolder = sNewHolder;
    sNew->hWndNotif = hWndNotif;
    sNew->uiMessage = uiMessage;
    sNew->sizeImage.cx = iSizeX;
    sNew->sizeImage.cy = iSizeY;

    EnterCriticalSection(&m_crQueue);
    hr = m_arrQueue.AddElement(sNew);
    LeaveCriticalSection(&m_crQueue);
    CHR(hr, _T("hr = m_arrQueue.AddElement(sNew)"));

    UpdateQueue();

Error:
    if(hr != S_OK && sNew)
        delete sNew;
    return (hr==S_OK?sNewHolder:NULL);
}

TypePLHolder* CIssPicLoader::AddPicToQueue(UINT      uiItem,
                                           HWND      hWndNotif /*= NULL*/,
                                           UINT      uiMessage /*= 0*/,
                                           int       iSizeX /*= 0*/,
                                           int       iSizeY /*= 0*/)
{
    HRESULT hr = S_OK;
    TypePLItem* sNew = NULL;
    TypePLHolder* sNewHolder = NULL;    

    CBARG(uiItem != 0, _T(""));

    sNewHolder = new TypePLHolder;
    CPHR(sNewHolder, _T("sNewHolder = new TypePLHolder;"));

    EnterCriticalSection(&m_crPicHolder);
    hr = m_arrPicHolder.AddElement(sNewHolder);
    LeaveCriticalSection(&m_crPicHolder);
    CHR(hr, _T("hr = m_arrPicHolder.AddElement(sNewHolder);"));

    sNew = new TypePLItem;
    CPHR(sNew, _T("TypePLItem* sNew = new TypePLItem"));	

    sNew->uiImage       = uiItem;
    sNew->psPicHolder   = sNewHolder;
    sNew->hWndNotif     = hWndNotif;
    sNew->uiMessage     = uiMessage;
    sNew->sizeImage.cx  = iSizeX;
    sNew->sizeImage.cy  = iSizeY;

    EnterCriticalSection(&m_crQueue);
    hr = m_arrQueue.AddElement(sNew);
    LeaveCriticalSection(&m_crQueue);
    CHR(hr, _T("hr = m_arrQueue.AddElement(sNew)"));

    UpdateQueue();

Error:
    if(hr != S_OK && sNew)
        delete sNew;
    return (hr==S_OK?sNewHolder:NULL);
}

TypePLHolder* CIssPicLoader::AddPicToQueue(PFN_LOADPIC pfnLoadPic,
                                           LPVOID      lpClass,
                                           LPARAM      lpItem /*= NULL*/,
                                           HWND      hWndNotif /*= NULL*/,
                                           UINT      uiMessage /*= 0*/,
                                           int       iSizeX /*= 0*/,
                                           int       iSizeY /*= 0*/)
{
    HRESULT hr = S_OK;
    TypePLItem* sNew = NULL;
    TypePLHolder* sNewHolder = NULL;    

    CBARG(pfnLoadPic != NULL && lpClass, _T(""));

    sNewHolder = new TypePLHolder;
    CPHR(sNewHolder, _T("sNewHolder = new TypePLHolder;"));

    EnterCriticalSection(&m_crPicHolder);
    hr = m_arrPicHolder.AddElement(sNewHolder);
    LeaveCriticalSection(&m_crPicHolder);
    CHR(hr, _T("hr = m_arrPicHolder.AddElement(sNewHolder);"));

    sNew = new TypePLItem;
    CPHR(sNew, _T("TypePLItem* sNew = new TypePLItem"));	

    sNew->pfnLoadPic    = pfnLoadPic;
    sNew->lpClass       = lpClass;
    sNew->lpItem        = lpItem;
    sNew->psPicHolder   = sNewHolder;
    sNew->hWndNotif     = hWndNotif;
    sNew->uiMessage     = uiMessage;
    sNew->sizeImage.cx  = iSizeX;
    sNew->sizeImage.cy  = iSizeY;

    EnterCriticalSection(&m_crQueue);
    hr = m_arrQueue.AddElement(sNew);
    LeaveCriticalSection(&m_crQueue);
    CHR(hr, _T("hr = m_arrQueue.AddElement(sNew)"));

    UpdateQueue();

Error:
    if(hr != S_OK && sNew)
        delete sNew;
    return (hr==S_OK?sNewHolder:NULL);
}

TypePLItem* CIssPicLoader::FindItem(CIssVector<TypePLItem>& arr, TCHAR* szURL, int iIndexExclude)
{
    TypePLItem* sPic = NULL;
    for(int i=0; i<arr.GetSize(); i++)
    {
        if(i == iIndexExclude)
            continue;
        TypePLItem* sTest = arr[i];
        if(sTest && 0 == m_oStr->Compare(sTest->szURL, szURL))
        {
            sPic = sTest;
            break;
        }
    }
    return sPic;
}

int CIssPicLoader::FindItemIndex(CIssVector<TypePLItem>& arr, TypePLItem* sCheck)
{
    if(!sCheck)
        return -1;

    for(int i=0; i<arr.GetSize(); i++)
    {
        if(arr[i] == sCheck)
            return i;
    }
    return -1;
}

void CIssPicLoader::UpdateQueue()
{
    EnterCriticalSection(&m_crQueue);
    for(int i=0; i<m_arrQueue.GetSize(); i++)
    {
        TypePLItem* sItem = m_arrQueue[i];
        if(!sItem)
            continue;

        if(sItem->ePicState == PL_InQueue)
        {
            // go through all the threads and kickstart the ones to start downloading
            for(int j=0; j<NUM_Threads; j++)
            {
                if(m_hEvents[j] && !m_bProcessing[j])
                {
                    m_bProcessing[j] = TRUE;
                    //PulseEvent(m_hEvents[j]);
                    SetEvent(m_hEvents[j]);
                    Sleep(5);
                    break;
                }
            }
            
        }
    }

    LeaveCriticalSection(&m_crQueue);
}

DWORD CIssPicLoader::ThreadPicLoad(LPVOID lpVoid)
{
    if(!g_objPicDLInstance)
        return 0;

    int iLoadIndex = (int)lpVoid;
    
    return g_objPicDLInstance->FnPicLoad(iLoadIndex);
}

DWORD CIssPicLoader::FnPicLoad(int iLoadIndex)
{
    DBG_OUT((_T("CIssPicLoader::FnPicLoad - iLoadIndex %d"), iLoadIndex));

    if(iLoadIndex < 0 || iLoadIndex >= NUM_Threads)
    {
        DBG_OUT((_T("CIssPicLoader::FnPicLoad - iLoadIndex %d is out of bounds"), iLoadIndex));
        return 0;
    }

    ResetEvent(m_hEvents[iLoadIndex]);

    HRESULT hr;
    TypePLItem* sPicItem = NULL;
    //int iQueueIndex = -1;
    while(!m_bKillThreads)
    {
        m_bProcessing[iLoadIndex] = FALSE;

        WaitForSingleObject(m_hEvents[iLoadIndex], INFINITE);
        ResetEvent(m_hEvents[iLoadIndex]);
        //DBG_OUT((_T("THREAD %d: m_hEvents triggered"), iLoadIndex));

        if(m_bKillThreads)
            break;

        m_bProcessing[iLoadIndex] = TRUE;

        sPicItem = NULL;
        //iQueueIndex = -1;
        EnterCriticalSection(&m_crQueue);
        for(int i=0; i<m_arrQueue.GetSize(); i++)
        {
            TypePLItem* sItem = m_arrQueue[i];
            if(!sItem)
                continue;

            if(sItem->ePicState == PL_InQueue)
            {
                sPicItem = sItem;
                sPicItem->ePicState = PL_Loading; // no other threads touch it
                break;
            }
        }
        LeaveCriticalSection(&m_crQueue);

        if(m_bKillThreads)
            break;

        if(!sPicItem)
        {
            m_bProcessing[iLoadIndex] = FALSE;
            UpdateQueue();
            continue;
        }        

        // let the other threads do things
        Sleep(5);

        if(m_bKillThreads)
            break;

        if(sPicItem)
        {
            //////////////////////////////////////////////////////////////////////////
            // attempt to download the file 
            hr = ProcessLoadFile(sPicItem);
            //DBG_OUT((_T("THREAD %d: Called ProcessLoadFile %d"), iLoadIndex, hr));

            if(m_bKillThreads)
                break;

            //////////////////////////////////////////////////////////////////////////
            // Remove the item off the queue
            //EnterCriticalSection(&m_crPics);
            EnterCriticalSection(&m_crQueue);

            int iIndex = FindItemIndex(m_arrQueue, sPicItem);
            if(iIndex != -1)
                m_arrQueue.RemoveElementAt(iIndex);  
            
            //////////////////////////////////////////////////////////////////////////
            // Add to our pic array
            if(hr == S_OK)
            {
                //DBG_OUT((_T("THREAD %d: hr = S_OK"), iLoadIndex));
                // add it to our pic array
                
                m_arrPics.AddElement(sPicItem);  

                //DBG_OUT((_T("THREAD %d: Added sPicItem to m_arrPics"), iLoadIndex));

                if(sPicItem->hWndNotif != NULL && sPicItem->uiMessage > 0)
                {
                    //DBG_OUT((_T("THREAD %d: updating sPicItem->gdiDest and calling window"), iLoadIndex));

                    EnterCriticalSection(&m_crPicHolder);
                    if(sPicItem->psPicHolder)
                        sPicItem->psPicHolder->gdiImage = sPicItem->gdi;
                    LeaveCriticalSection(&m_crPicHolder);

                    //*sPicItem->gdiDest = &sPicItem->gdi;
                    //if(GetTickCount() - m_dwLastPost > 700)
                    {
                        PostMessage(sPicItem->hWndNotif, sPicItem->uiMessage, 0,0);
                        m_dwLastPost = GetTickCount();
                    }
                }
            }
            else
            {
                //DBG_OUT((_T("THREAD %d: hr != S_OK"), iLoadIndex));
                delete sPicItem;
            }

            LeaveCriticalSection(&m_crQueue);
            //LeaveCriticalSection(&m_crPics);

            if(m_bKillThreads)
                break;

            
        }

        // let the other threads do things
        Sleep(5);

        if(m_bKillThreads)
            break; 

        m_bProcessing[iLoadIndex] = FALSE;             

        //DBG_OUT((_T("THREAD %d: Calling UpdateQueue"), iLoadIndex));
        UpdateQueue();

    }

    return 0;
}

void CIssPicLoader::DeletePicArray(CIssVector<TypePLItem>& arrItems)
{
    for(int i = 0; i<arrItems.GetSize(); i++)
    {
        TypePLItem* sItem = arrItems[i];
        if(sItem)
            delete sItem;
    }
    arrItems.RemoveAll();
}

HRESULT CIssPicLoader::ProcessLoadFile(TypePLItem* sItem)
{
    HRESULT hr = S_OK;
    CIssGDIEx gdiTemp;

    CBARG(!m_bKillThreads, _T(""));
    CBARG(sItem, _T(""));
    CBARG(m_oStr->GetLength(sItem->szFilename) > 0 || sItem->pfnLoadPic != NULL || sItem->uiImage != 0, _T(""));

    if(sItem->uiImage)
    {
        hr = gdiTemp.LoadImage(sItem->uiImage, m_hWndParent, m_hInst, TRUE);
        CHR(hr, _T("hr = gdiTemp.LoadImage(sItem->uiImage, m_hWndParent, m_hInst, TRUE);"));
    }
    else if(sItem->pfnLoadPic)
    {
        hr = sItem->pfnLoadPic(gdiTemp, sItem, sItem->lpClass, sItem->lpItem);
        CHR(hr, _T("hr = sItem->pfnLoadPic(sItem);"));
    }
    else if(m_oStr->GetLength(sItem->szURL) > 0)
    {
        hr = DownloadPic(sItem);
        CHR(hr, _T("hr = DownloadPic(oInet, sItem)"));

        hr = gdiTemp.LoadImage(sItem->szFilename, m_hWndParent, TRUE);
        CHR(hr, _T("hr = gdiTemp.LoadImage(sItem->szFilename, m_hWndParent, TRUE);"));
    }
    else
    {
        hr = gdiTemp.LoadImage(sItem->szFilename, m_hWndParent, TRUE);
        CHR(hr, _T("hr = gdiTemp.LoadImage(sItem->szFilename, m_hWndParent, TRUE);"));
    }

    CBARG(!m_bKillThreads, _T(""));

    sItem->gdi  = new CIssGDIEx;
    CPHR(sItem->gdi, _T("sItem->gdi  = new CIssGDIEx;"));

    // see if we have to resize anything
    if(sItem->sizeImage.cx == 0 && sItem->sizeImage.cy == 0)
    {
        // no resizing necessary
        hr = gdiTemp.MoveGdiMemory(*sItem->gdi);
        CHR(hr, _T("hr = gdiTemp.MoveGdiMemory(*sItem->gdi);"));
    }
    else
    {
        hr = ScaleImage(gdiTemp, *sItem->gdi, sItem->sizeImage, TRUE, RGB(255,0,255));
        CHR(hr, _T("hr = ScaleImage(gdiTemp, *sItem->gdi, sItem->sizeImage, TRUE, RGB(255,0,255));"));
    }
   
Error:

    return hr;
}

HRESULT CIssPicLoader::DownloadPic(TypePLItem* sItem)
{
    HRESULT hr = S_OK;
    BOOL bRet;
    int iNumTrys = 0;
    CIssInternet oInet;

    CBARG(sItem, _T(""));
    CBARG(m_oStr->GetLength(sItem->szFilename) > 0 && m_oStr->GetLength(sItem->szURL) > 0, _T(""));

    bRet = oInet.Init(NULL, 0, FALSE, TRUE, FALSE, TRUE);
    CBHR(bRet, _T("oInet.Init"));

    // delete the old file if it was there first
    DeleteFile(sItem->szFilename);

    TCHAR szTempFileName[STRING_MAX];
    GetTempFileName(szTempFileName);    

    // kickstart a connection if we have to
    if(!IsInternetConnection())
    {
        bRet = GoOnline();
        CBHR(bRet, _T("GoOnline()"));
    }

    // BUGBUG: I'm really only going to try once
    do 
    {
        // give it a chance to reset itself
        if(iNumTrys > 0)
            Sleep(500);

		//DBG_OUT((_T("oInet.DownloadFile - %s, %s"), sItem->szURL, szTempFileName));
        bRet = oInet.DownloadFile(sItem->szURL, szTempFileName, NULL, 0, NULL);
        iNumTrys++;
    }while(!bRet && iNumTrys < 1);    
    CBHR(bRet, _T("oInet.DownloadFile"));

    bRet = MoveFile(szTempFileName, sItem->szFilename);
    CBHR(bRet, _T("bRet = MoveFile(szTempFileName, sItem->szSaveFile)"));

Error:
    //DBG_OUT((_T("CIssPicLoader::DownloadPic returns %d"), hr));
    return hr;
}

void CIssPicLoader::GetTempFileName(TCHAR* szFileName)
{
    TCHAR szTemp[STRING_MAX];
    CreateGUIDName(szTemp, 10);

    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, TEMP_Dir);

    // create the accounts folder if it doesn't exist
    DWORD dwAttrib = GetFileAttributes(szFileName);
    if (-1 == dwAttrib)
    {
        CreateDirectory(szFileName, NULL);
    }

    m_oStr->Concatenate(szFileName, szTemp);
    m_oStr->Concatenate(szFileName, _T(".tmp"));
}

void CIssPicLoader::ClearAllTempFiles()
{
    TCHAR szFolder[STRING_MAX] = _T("");

    GetExeDirectory(szFolder);
    m_oStr->Concatenate(szFolder, TEMP_Dir);
    m_oStr->Concatenate(szFolder, _T("*.tmp"));

    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(szFolder, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) 
        return;

    TCHAR szFile[STRING_MAX];
    do 
    {
        int i=0;
        GetExeDirectory(szFile);
        m_oStr->Concatenate(szFile, TEMP_Dir);
        m_oStr->Concatenate(szFile, wfd.cFileName);
        DeleteFile(szFile);

    }while (FindNextFile(hFind, &wfd));
    FindClose(hFind);
}

void CIssPicLoader::ClearQueue()
{
    EnterCriticalSection(&m_crQueue);

    for(int i=m_arrQueue.GetSize()-1; i>=0; i--)
    {
        TypePLItem* sItem = m_arrQueue[i];
        if(sItem && sItem->ePicState == PL_InQueue)
        {
            delete sItem;
            m_arrQueue.RemoveElementAt(i);
        }
    }

    LeaveCriticalSection(&m_crQueue);
}

int CIssPicLoader::GetPicStoreCount()
{
    EnterCriticalSection(&m_crQueue);
    int iCount = m_arrPics.GetSize();
    LeaveCriticalSection(&m_crQueue);
    return iCount;
}

int CIssPicLoader::GetPicQueueCount()
{
    EnterCriticalSection(&m_crQueue);
    int iCount = m_arrQueue.GetSize();
    LeaveCriticalSection(&m_crQueue);
    return iCount;
}

int CIssPicLoader::GetPicHolderCount()
{
    EnterCriticalSection(&m_crPicHolder);
    int iCount = m_arrPicHolder.GetSize();
    LeaveCriticalSection(&m_crPicHolder);
    return iCount;
}

void CIssPicLoader::ClearAllAsyncItems()
{
    SetThreadPriorities(THREAD_PRIORITY_HIGHEST);

    //EnterCriticalSection(&m_crPics);
    EnterCriticalSection(&m_crQueue);

    // first take off the ones in the queue
    for(int i=m_arrQueue.GetSize()-1; i>=0; i--)
    {
        TypePLItem* sItem = m_arrQueue[i];
        if(sItem && sItem->ePicState == PL_InQueue)
        {
            delete sItem;
            m_arrQueue.RemoveElementAt(i);
        }
    }

    // take the ones we have stored out. NOTE: only async retrieved items
    /*for(int i=m_arrPics.GetSize()-1; i>=0; i--)
    {
        TypePLItem* sItem = m_arrPics[i];
        if(sItem)
        {
            delete sItem;
            m_arrPics.RemoveElementAt(i);
        }
    }*/

    LeaveCriticalSection(&m_crQueue);
    //LeaveCriticalSection(&m_crPics);

    SetThreadPriorities(THREAD_PRIORITY_BELOW_NORMAL);
}

void CIssPicLoader::UpdateImageLocation(TCHAR* szDestination, TCHAR* szOriginal)
{
    // check if they are the same first
    if(0 == m_oStr->Compare(szDestination, szOriginal) || IsFileExists(szDestination))
        return;

    BOOL bReturn = CopyFile(szOriginal, szDestination, FALSE);
    DBG_OUT((_T("CIssPicLoader::UpdateImageLocation CopyFile - %s, %s, %d"), szDestination, szOriginal, bReturn));
}

void CIssPicLoader::WaitForQueueToFinish(HWND hWndParent)
{
    SetThreadPriorities(THREAD_PRIORITY_ABOVE_NORMAL);

    BOOL bProcessing;

    do 
    {
        EnterCriticalSection(&m_crQueue);
        bProcessing = FALSE;
		for(int j=0; j<NUM_Threads; j++)
		{
			if(m_hEvents[j] && m_bProcessing[j])
			{
				bProcessing = TRUE;
				break;
			}
		}
        LeaveCriticalSection(&m_crQueue);

        if(!bProcessing)
            break;

        Sleep(300);
    } while(bProcessing);

    SetThreadPriorities(THREAD_PRIORITY_BELOW_NORMAL);
}

void CIssPicLoader::PurgePictures()
{
    DBG_OUT((_T("CIssPicLoader::PurgePictures()")));
    EnterCriticalSection(&m_crPicHolder);

    // first we delete the old handles
    for(int i=m_arrPicHolder.GetSize()-1; i>=0; i--)
    {
        TypePLHolder* sPic = m_arrPicHolder[i];
        if(sPic && sPic->bIsDeleted)
        {
            delete sPic;
            m_arrPicHolder.RemoveElementAt(i);
        }
    }

    EnterCriticalSection(&m_crQueue);
    for(int j=m_arrPics.GetSize()-1; j>=0; j--)
    {
        TypePLItem* sPicItem = m_arrPics[j];
        if(!sPicItem)
            continue;

        BOOL bFoundItem = FALSE;
        for(int i=0; i<m_arrPicHolder.GetSize(); i++)
        {
            TypePLHolder* sPicHolder = m_arrPicHolder[i];
            if(!sPicHolder)
                continue;

            if(sPicHolder->gdiImage && sPicHolder->gdiImage == sPicItem->gdi)
            {
                bFoundItem = TRUE;
                break;
            }
        }

        if(!bFoundItem)
        {
            delete sPicItem;
            m_arrPics.RemoveElementAt(j);
        }
    }
    LeaveCriticalSection(&m_crQueue);



    LeaveCriticalSection(&m_crPicHolder);
}
