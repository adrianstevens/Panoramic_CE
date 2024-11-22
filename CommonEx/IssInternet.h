/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssInternet.h
// Abstract: Internet download/upload class. Fully multithreaded or singlethreaded
//  
/***************************************************************************************************************/ 

#pragma once

#include "IssString.h"
#ifdef UNDER_CE
#include "IssConnection.h"
#endif
#include "Wininet.h"

#define TEXT_Boundary           _T("-----------------------------4151483823793")

#ifndef UNDER_CE
enum EnumInetCondition
{
    INET_CON_CREATED,
    INET_CON_FAILED,
    INET_CON_ERROR_OPEN,
    INET_CON_ERROR_READ,
    INET_CON_POST_FAILED,
    INET_CON_POST_SUCCESS,
	INET_CON_POST_PCT,
    INET_CON_GET_FAILED,
    INET_CON_GET_SUCCESS,
    //INET_CON_TOTAL_DL,
    INET_CON_WRITTEN_BYTES,
    INET_CON_DOWNLOAD_COMPLETE,
    INET_CON_ERROR_BUF_LEN_SMALL,
    INET_DIALUP_CONNECTION_MADE,
    INET_DIALUP_CONNECTION_LOST,
    INET_RETURN_BAD_REQUEST,
    INET_RETURN_NOT_FOUND,
};
#endif

class CIssInternet;

BOOL    IsInternetConnection();
BOOL    GoOnline();

class URLEncoder 
{
public:
    static TCHAR* Encode(TCHAR* szURL);
    static char* Encode(char* szURL);
private:
    static BOOL IsOrdinaryChar(TCHAR c);
    static BOOL IsOrdinaryChar(char c);
};

class URLDecoder 
{
public:
    static TCHAR* Decode(TCHAR* szURL);
private:
    static int ConvertToDec(const TCHAR* hex);
    static void GetAsDec(TCHAR* hex);
};

class CIssInternet  
{
public:
        CIssInternet();
        virtual ~CIssInternet();

        BOOL    Init(           HWND hWndParent, 
                                                UINT uiMessage, 
                                                BOOL bNewThread, 
                                                BOOL bUseProxy, 
                                                BOOL bAsyncOnly, 
                                                BOOL bForceConnection,
												BOOL bOverrideAgent = FALSE);

        BOOL    Destroy();
        BOOL	IsInitialized(){if(m_hOpen) return TRUE; return FALSE;};
		static BOOL    GetUserAgent(TCHAR* szAgent, BOOL bOverrideAgent = FALSE);
        
        void    SetAsyncOnly(BOOL bAsyncOnly){m_bAsyncOnly = bAsyncOnly;};
        BOOL    GetAsyncOnly(){return m_bAsyncOnly;};

        BOOL    Hangup();
		HRESULT	GetRedirectURL(TCHAR* szURL, TCHAR* szReturnedURL);

        BOOL	StartRequest(TCHAR* szUrlRequest);
        BOOL	GetRequestHeader(TCHAR* szHeaderName, TCHAR* szResult, DWORD dwResultLen);
        DWORD	getHttpCode();
        BOOL    DownloadFile(TCHAR* szUrlRequest,
                                                TCHAR*  szSaveFile, 
                                                TCHAR*  szBuffer, 
                                                UINT    uiBuffLen,
                                                CHAR*   szBufferChar = NULL);
		BOOL	SetOAuth(TCHAR* szOauth, BOOL bAuthAsIs = FALSE);
        HRESULT UploadFile(TCHAR* szUrlRequest, 
                            TCHAR* szFilePath, 
                            TCHAR* szSaveFile = NULL, 
                            TCHAR* szUsername = NULL, 
                            TCHAR* szPassword = NULL, 
                            TCHAR* szHead = NULL,
                            TCHAR* szFoot = NULL);

#ifndef WIN32_PLATFORM_WFSP
        int             Ping( TCHAR *Address , TCHAR *szName, int *prtt ,int iTtl = 256,  int iWaitTime = 2000);
#endif

        BOOL    Contact(TCHAR*  szUrlRequest,   
                                   TCHAR*       szSaveFile,
                                   TCHAR*       szBuffer, 
                                   CHAR*        szBufferChar,
                                   UINT         uiBuffLen,  
                                   BOOL         bPost,  
                                   LPVOID       lpPostData,
                                   UINT         uiPostDataLen,
                                   TCHAR*       szUserName = NULL,
                                   TCHAR*       szPassword = NULL,
                                   BOOL         bMultipart = FALSE,
                                   CHAR*       szHead = NULL,
                                   CHAR*       szFoot = NULL,
                                   TCHAR*       szRequestHeader = NULL,
                                   UINT         uiRequestHeaderLen = 0);

        BOOL    IsConnected();                                          // Returns a state determined by the connection manager
        BOOL    IsDownloading(){return (m_hThreadDownload?TRUE:FALSE);};
        
        int             GetBytesDownloaded(){return m_iBytesDownloaded;};

        void    SetAssumeConnected(BOOL bAssumedConnected){m_bAssumeConnected =bAssumedConnected;};

        static BOOL     IsDocked(CRITICAL_SECTION* csSection = NULL);

        
private:
        BOOL    fnContact();                                            // perform the full contact operation
        BOOL    fnDownloadPost();                                       // perform the POST Download operation
        BOOL    fnDownloadGet();                                        // perform the GET  Download operation
        BOOL    fnDownloadAlt();                                        // perform the GET  Alternate Download operation
        BOOL    ReadWebContents(HINTERNET hInetConnection);     // Performs the reading of webpages and files and such
        void    CleanURL();                                                     // cleanup the URL string

        BOOL    OpenConnection(BOOL	bOverrideAgent);
        
        int             GetOSVersion();
        
        static  DWORD   WINAPI ThreadLaunchContact(LPVOID lpVoid);
        static  DWORD   WINAPI ThreadDownload(LPVOID lpVoid);   
        BOOL    SaveContactVariables(TCHAR*     szSaveFile,                     // This function is only called by the contact function to save the parameters
                                                                TCHAR*  szBuffer,                       // so we can access them from a static function
                                                                CHAR*   szBufferChar,
                                                                UINT    uiBuffLen,
                                                                TCHAR*  szUrlRequest, 
                                                                BOOL    bPost,
                                                                LPVOID  lpPostData, 
                                                                UINT    uiPostDataLen,
                                                                TCHAR*  szUserName,
                                                                TCHAR*  szPassword,
                                                                CHAR*  szHead,
                                                                CHAR*  szFoot,
                                                                TCHAR*  szRequestHeader,
                                                                UINT    uiRequestHeaderLen);  

        BOOL    DestroyContactVariables();
        BOOL    DestroyContactParams();
        BOOL    DestroyInternetParams();
		void	HttpStatusText();

private:
        CIssString*                     m_oStr;                         
        DWORD                           m_dwConnectionFlags;
        HANDLE                          m_hThreadContact;
        HANDLE                          m_hThreadDownload;
        HANDLE                          m_hEventOpen;                   // Open Event
        HANDLE                          m_hEventRead;                   // Read Event
        HANDLE                          m_hEventFailed;                 // a Failure has been signaled
        CRITICAL_SECTION        m_CriticalSection;              // our critical section for the fncontact fn
        HINTERNET                       m_hOpen;                                // Handle to an open connection if we have one.
        TCHAR*                          m_szSaveFile;                   // Contact Function Parameter
        TCHAR*                          m_szBuffer;                             // Contact Function Parameter
        CHAR*                           m_szBufferChar;                 // Contact Function Parameter
        UINT                            m_uiBuffLen;                    // Contact Function Parameter
        TCHAR*                          m_szRequestHeader;
        UINT                            m_uiRequestHeaderLen;
        BOOL                            m_bPost;                                // Contact Function Parameter
        LPVOID                          m_lpPostData;                   // Contact Function Parameter
        UINT                            m_uiPostDataLen;                // Contact Function Parameter
        TCHAR*                          m_szUrlRequest;                 // Contact Function Parameter
        TCHAR*                          m_szUserName;
        TCHAR*                          m_szPassword;
		TCHAR*							m_szOAuth;
		BOOL							m_bAuthAsIs;

        HWND                            m_hWndParent;                   // Handle to the parent window which will receive state change messages
        UINT                            m_uiMessage;                    // Message to post to the parent window
        BOOL                            m_bNewThread;                   // TRUE if a new thread should be created to download content.
        BOOL                            m_bUseProxy;                    // TRUE if a proxy should be used.
        BOOL                            m_bAsyncOnly;                   // ActiveSync connection only.
        BOOL                            m_bForceConnection;             // TRUE if a connection should be created if one doesn't exist...FALSE otherwise
        BOOL                            m_bMultipart;

        // Internet Download Params
        HINTERNET                       m_hInternetConnect;             // Internet Connection handle
        HINTERNET                       m_hInternetRequest;             // Internet Request handle
        HANDLE                           m_hSaveFile;                    // File to Save
        TCHAR*                          m_szServer;                             // The server to download from Eg. www.yahoo.com
        TCHAR*                          m_szEndPoint;                   // Endpoint Eg. index.html
        TCHAR*                          m_szHeader;                     // Header needed for the HTTP get or post
        CHAR*                           m_szHead;
        CHAR*                           m_szFoot;
        CHAR*                           m_szReceiveBuffer;              // Buffer we use to read stuff in
        
		DWORD							m_dwLastUpdate;
        DWORD                           m_dwExitCode;                   // The exit codes of either threads
        int                                     m_iBytesDownloaded;             // Set within readwebpage, passed back to called through DownloadFile
        BOOL                            m_bAssumeConnected; // TRUE if you want IsConnected to return true always (gprs dialup fix)
};
