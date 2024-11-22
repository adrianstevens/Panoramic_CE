#pragma once

#include "IssString.h"
#include "IssInternet.h"
#include "IssGDIEx.h"
#include "IssVector.h"

#define NUM_Threads     3

enum EnumPLState
{
    PL_InQueue = 0,
    PL_Loading,
    PL_Error,
    PL_Complete,
};

struct TypePLHolder
{
    BOOL        bIsDeleted;
    CIssGDIEx*  gdiImage;
    TypePLHolder();
};

struct TypePLItem;
typedef HRESULT (*PFN_LOADPIC)(CIssGDIEx&, TypePLItem*, LPVOID, LPARAM);

struct TypePLItem
{
    TCHAR*      szURL;              // URL to download from
    TCHAR*      szFilename;         // Image file to be saved to
    UINT        uiImage;            // resource of image to load
    PFN_LOADPIC pfnLoadPic;         // custom function to fill the GDI
    CIssGDIEx*   gdi;               // the image
    TypePLHolder* psPicHolder;      // our exposed pic holder
    HWND        hWndNotif;          // Window to notify when pic is ready
    UINT        uiMessage;          // message to notify when pic is ready
    EnumPLState ePicState;          // current state the picture is in 
    SIZE        sizeImage;          // when image is loaded, size it needs to be set, 0,0 means don't size
    LPARAM      lpItem;
    LPVOID      lpClass;

    TypePLItem();
    ~TypePLItem();
    void Destroy();
};

class CIssPicLoader
{
public:
    CIssPicLoader(void);
    ~CIssPicLoader(void);

    void    Destroy();
    void    ClearQueue();
    void    WaitForQueueToFinish(HWND hWndParent);
    void    ClearAllAsyncItems();
    void    PurgePictures();
    HRESULT StartEngine(HWND hWndParent, HINSTANCE hInst);
    BOOL    IsEngineStarted();
    TypePLHolder* AddPicToQueue(  TCHAR*    szURL,
                                  TCHAR*    szFilename,
                                  HWND      hWndNotif = NULL,
                                  UINT      uiMessage = 0,
                                  int       iSizeX = 0,
                                  int       iSizeY = 0);
    TypePLHolder* AddPicToQueue(TCHAR*    szFilename,
                                HWND      hWndNotif = NULL,
                                UINT      uiMessage = 0,
                                int       iSizeX = 0,
                                int       iSizeY = 0);
    TypePLHolder* AddPicToQueue(UINT      uiItem,
                                HWND      hWndNotif = NULL,
                                UINT      uiMessage = 0,
                                int       iSizeX = 0,
                                int       iSizeY = 0);
    TypePLHolder* AddPicToQueue(PFN_LOADPIC pfnLoadPic,
                                LPVOID      lpClass,
                                LPARAM      lpItem = NULL,
                                HWND      hWndNotif = NULL,
                                UINT      uiMessage = 0,
                                int       iSizeX = 0,
                                int       iSizeY = 0);
    int         GetPicStoreCount();
    int         GetPicQueueCount();
    int         GetPicHolderCount();
    void        SetThreadPriorities(int iPriority);

private:    // functions
    HRESULT     ProcessLoadFile(TypePLItem* sItem);
    HRESULT     DownloadPic(TypePLItem* sItem);
    void        GetTempFileName(TCHAR* szFileName);
    void        DeletePicArray(CIssVector<TypePLItem>& arrItems);
    void        ClearAllTempFiles();
    static DWORD ThreadPicLoad(LPVOID lpVoid);
    DWORD       FnPicLoad(int iLoadIndex);
    void        DestroyThreads();
    void        UpdateQueue();
    TypePLItem* FindItem(CIssVector<TypePLItem>& arr, TCHAR* szURL, int iIndexExclude = -1);
    int         FindItemIndex(CIssVector<TypePLItem>& arr, TypePLItem* sCheck);
    void        UpdateImageLocation(TCHAR* szDestination, TCHAR* szOriginal);


private:    // variables
    CIssString*         m_oStr;
    HWND                m_hWndParent;
    HINSTANCE           m_hInst;
    CIssVector<TypePLItem> m_arrQueue;
    CIssVector<TypePLItem> m_arrPics;
    CIssVector<TypePLHolder> m_arrPicHolder;
    CRITICAL_SECTION    m_crQueue;
    CRITICAL_SECTION    m_crPicHolder;
    CRITICAL_SECTION    m_crConnection;
    HANDLE              m_hThreads[NUM_Threads];
    HANDLE              m_hEvents[NUM_Threads];
    BOOL                m_bProcessing[NUM_Threads];
    BOOL                m_bKillThreads;
    DWORD               m_dwLastPost;
};
