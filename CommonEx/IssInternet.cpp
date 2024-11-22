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

#include "IssInternet.h"

#include <iphlpapi.h>
#include "windows.h"
#include "issdebug.h"
#include "IssCommon.h"
#ifndef WIN32_PLATFORM_WFSP
#include "Icmpapi.h"
#endif
#include "winsock.h"
#include "urlmon.h"
#include "IssBase64.h"

#pragma comment( lib,"urlmon.lib")
#pragma comment( lib,"iphlpapi.lib")
#ifdef UNDER_CE
#include "IssConnection.h"
#include "regext.h"
#include "snapi.h"
#pragma comment( lib, "cellcore.lib" )
#pragma comment( lib, "winsock.lib")
#pragma comment(lib, "Wininet.lib")
#if defined(ARMV4) || defined(_i386)            // assuming when we debug on the emulator it will be in EVC4
#pragma comment( lib, "ws2.lib")
#else
//#ifndef WIN32_PLATFORM_WFSP
//#pragma comment( lib, "icmplib.lib")
//#endif
#endif
#endif

// when we debug we don't want any timeouts for us
#ifdef DEBUG
#define CONNECT_TIMEOUT         60000//*/INFINITE - infinate seems to cause a lot more 12002 errors in debug mode.
#define DOWNLOAD_TIMEOUT        2000000 //*/INFINITE
#else
#define CONNECT_TIMEOUT         30000
#define DOWNLOAD_TIMEOUT        2000000
#endif

#define SIZE_RECEIVE_BUFFER     1024
#define URL_SIZE                512
#define SIZE_Post               1024
#define TEST_URL                _T("http://www.google.com")

#define __HTTP_ACCEPT           _T("Accept: */*\r\n")

BOOL IsInternetConnection()
{
#ifndef UNDER_CE
    return TRUE;
#else
    CIssConnection oConnection;

    // the connection manager way of checking if we're online
    if(E_FAIL == oConnection.IsAvailable(TEST_URL, TRUE))
        return FALSE;
    else
        return TRUE;
#endif
}

BOOL GoOnline()
{
#ifndef UNDER_CE
    return TRUE;
#else
    HRESULT hr = S_OK;
    //CIssInternet oInet;
    CIssConnection oConnection;

    //BOOL bRet = oInet.Init(hWnd, WM_USER, FALSE, TRUE, FALSE, TRUE);
    //CBHR(bRet, _T("BOOL bRet = oInet.Init(hWnd, WM_USER, FALSE, TRUE, FALSE, TRUE);"));

    // quick check to see if we can access the internet
    if(E_FAIL == oConnection.IsAvailable(TEST_URL, TRUE))
    {
        // we didn't succeed so let's force a connection
        hr = oConnection.AttemptConnect(TEST_URL, TRUE);
        if(E_FAIL == hr)
            goto Error;

        // wait up to 10 seconds
        int iCount =0;
        while(E_FAIL == oConnection.IsAvailable(TEST_URL, TRUE))
        {
            SystemIdleTimerReset();

            iCount++;
            Sleep(1000);

            if(iCount>=40)
            {
                hr = E_FAIL;
                goto Error;
            }
        }
    }

Error:

    return (hr==E_FAIL?FALSE:TRUE);
#endif
}

char* URLEncoder::Encode(char* szURL) 
{
    CIssString* oStr = CIssString::Instance();
    if(!oStr || !szURL)
        return NULL;

    int iLen = oStr->GetLength(szURL);
    if(iLen == 0)
        return NULL;

    char* szText1 = new char[5*iLen + 1];
	if(!szText1)
		return NULL;

    char szText2[2*STRING_SMALL];
    ZeroMemory(szText1, sizeof(char)*(5*iLen + 1));

    for(int i=0;i<iLen;i++) 
    {
        if(IsOrdinaryChar(szURL[i]))
            oStr->Concatenate(szText1, szURL[i]);
        //else if(szURL[i] == ' ') 
        //    oStr->Concatenate(szText1, "+");
        else 
        {
            ZeroMemory(szText2, sizeof(char)*2*STRING_SMALL);
            //printf(szText2, "%%%x", szURL[i]);
            int iVal = (int)szURL[i];
            iVal = iVal&255; // we only care about the first two bytes
            oStr->Format(szText2,"%%%X",iVal);
            oStr->Concatenate(szText1, szText2);
        }
    }

    char* szNewText = oStr->CreateAndCopyChar(szText1);

	oStr->Delete(&szText1);

	return szNewText;
}

TCHAR* URLEncoder::Encode(TCHAR* szURL) 
{
    CIssString* oStr = CIssString::Instance();
    if(!oStr || !szURL)
        return NULL;

    int iLen = oStr->GetLength(szURL);
	if(iLen == 0)
		return NULL;

	TCHAR* szText1 = new TCHAR[5*iLen + 1];
	if(!szText1)
		return NULL;

	TCHAR szText2[2*STRING_SMALL];
	ZeroMemory(szText1, sizeof(TCHAR)*(5*iLen + 1));

    for(int i=0;i<iLen;i++) 
    {
        if(IsOrdinaryChar(szURL[i]))
            oStr->Concatenate(szText1, szURL[i]);
        //else if(szURL[i] == _T(' ')) 
        //    oStr->Concatenate(szText1, _T("+"));
        else 
        {
            ZeroMemory(szText2, sizeof(TCHAR)*STRING_SMALL);
            oStr->Format(szText2,_T("%%%X"),szURL[i]);
            oStr->Concatenate(szText1, szText2);
        }
    }

	TCHAR* szNewText = oStr->CreateAndCopy(szText1);

	oStr->Delete(&szText1);

	return szNewText;
}

BOOL URLEncoder::IsOrdinaryChar(TCHAR c) 
{
	if( ( c >= _T('0') && c <= _T('9')) || 
		( c >= _T('A') && c <= _T('Z')) || 
		(c >= _T('a') && c <= _T('z')) ||
		(c == _T('-')) || 
		(c == _T('_')) ||
		(c == _T('.')) ||
		(c == _T('~')))
        return TRUE;

    return FALSE;
}

BOOL URLEncoder::IsOrdinaryChar(char c) 
{
    //if(isalpha(c) || isdigit(c))
    //    return TRUE;
    if( ( c >= '0' && c <= '9') || 
		( c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ||
		(c == '-') || 
		(c == '_') ||
		(c == '.') ||
		(c == '~'))
        return TRUE;

    return FALSE;
}

TCHAR* URLDecoder::Decode(TCHAR* szURL) 
{
    CIssString* oStr = CIssString::Instance();
    if(!oStr)
        return NULL;

    int iLen = oStr->GetLength(szURL);
    if(iLen == 0)
        return NULL;

    TCHAR* szText1 = new TCHAR[iLen + 1];
	if(!szText1)
		return NULL;

    TCHAR szText2[STRING_SMALL];
    ZeroMemory(szText1, sizeof(TCHAR)*(iLen + 1));

    for(int i=0;i<iLen;i++) 
    {
        if(szURL[i] == _T('+'))
            oStr->Concatenate(szText1, _T(" "));
        else if(szURL[i] == _T('%')) 
        {
            TCHAR hex[4];			
            hex[0] = szURL[++i];
            hex[1] = szURL[++i];
            hex[2] = _T('\0');		
            ZeroMemory(szText2, sizeof(TCHAR)*STRING_SMALL);
            oStr->Format(szText2,_T("%c"),ConvertToDec(hex));
            oStr->Concatenate(szText1, szText2);
        }
        else
            oStr->Concatenate(szText1, szURL[i]);
    }

    TCHAR* szNew = oStr->CreateAndCopy(szText1);

	oStr->Delete(&szText1);
	return szNew;
}

int URLDecoder::ConvertToDec(const TCHAR* hex) 
{
    CIssString* oStr = CIssString::Instance();

    int ret = 0;
    TCHAR tmp[4];
    int len = oStr->GetLength((TCHAR*)hex);
    for(int i=0;i<len;i++) 
    {        
        tmp[0] = hex[i];
        tmp[1] = _T('\0');
        GetAsDec(tmp);
        int tmp_i = oStr->StringToInt(tmp);
        int rs = 1;
        for(int j=i;j<(len-1);j++) 
            rs *= 16;
        ret += (rs * tmp_i);
    }
    return ret;
}

void URLDecoder::GetAsDec(TCHAR* hex) 
{
    CIssString* oStr = CIssString::Instance();
    TCHAR tmp = _tolower(hex[0]);
    if(tmp == _T('a'))
        oStr->StringCopy(hex,_T("10"));
    else if(tmp == _T('b')) 
        oStr->StringCopy(hex,_T("11"));
    else if(tmp == _T('c')) 
        oStr->StringCopy(hex,_T("12"));
    else if(tmp == _T('d')) 
        oStr->StringCopy(hex,_T("13"));
    else if(tmp == _T('e')) 
        oStr->StringCopy(hex,_T("14"));
    else if(tmp == _T('f')) 
        oStr->StringCopy(hex,_T("15"));
    else if(tmp == _T('g')) 
        oStr->StringCopy(hex,_T("16"));    
}


CIssInternet::CIssInternet()
:m_hWndParent(NULL)
,m_uiMessage(0)
,m_bAsyncOnly(FALSE)
,m_bForceConnection(FALSE)
,m_bUseProxy(FALSE)
,m_bNewThread(FALSE)
,m_szBuffer(NULL)
,m_szBufferChar(NULL)
,m_lpPostData(NULL)
,m_uiPostDataLen(0)
,m_szUrlRequest(NULL)
,m_szSaveFile(NULL)
,m_hThreadContact(NULL)
,m_hThreadDownload(NULL)
,m_hEventOpen(NULL)
,m_hEventRead(NULL)
,m_hEventFailed(NULL)
,m_hOpen(NULL)
,m_hSaveFile(NULL)
,m_hInternetConnect(NULL)
,m_hInternetRequest(NULL)
,m_szServer(NULL)
,m_szEndPoint(NULL)
,m_szHeader(NULL)
,m_szHead(NULL)
,m_szFoot(NULL)
,m_szReceiveBuffer(NULL)
,m_iBytesDownloaded(0)
,m_szUserName(NULL)
,m_szPassword(NULL)
,m_bMultipart(FALSE)
,m_szRequestHeader(NULL)
,m_uiRequestHeaderLen(0)
,m_szOAuth(NULL)
,m_bAuthAsIs(FALSE)
,m_oStr(CIssString::Instance())
{

        m_dwConnectionFlags  =  INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_COOKIES; // always grab from wire
        ::InitializeCriticalSection( &m_CriticalSection );
}



CIssInternet::~CIssInternet()
{
        Destroy();
        ::DeleteCriticalSection( &m_CriticalSection );
}


/********************************************************************
Function        Destroy

Arguments:      

Returns:        

Comments:       Clean up all memory and close all threads
*********************************************************************/
BOOL CIssInternet::Destroy()
{
        DestroyContactVariables();
        DestroyContactParams();
        DestroyInternetParams();

        if(m_hThreadContact)
        {
                if(GetExitCodeThread(m_hThreadContact, &m_dwExitCode))
                {
                        // if the thread is still going, forcefully kill it
                        if(m_dwExitCode == STILL_ACTIVE)
                                TerminateThread(m_hThreadContact, 999);

                        // memory cleanup
                        CloseHandle( m_hThreadContact );                        
                }
                m_hThreadContact = NULL;
        }

        if(m_hOpen)
        {
                InternetCloseHandle(m_hOpen);
                m_hOpen = NULL;
        }

		m_oStr->Delete(&m_szOAuth);
		m_bAuthAsIs = FALSE;

        return TRUE;
}

/********************************************************************
Function        DestroyInternetParams

Arguments:      

Returns:        

Comments:       clean up internet related memory
*********************************************************************/
BOOL CIssInternet::DestroyInternetParams()
{
        if(m_hSaveFile)
        {
                CloseHandle((HANDLE)m_hSaveFile);
                m_hSaveFile = NULL;
        }

        if(m_hInternetRequest)
        {
                InternetCloseHandle(m_hInternetRequest);
                m_hInternetRequest = NULL;
        }

        if(m_hInternetConnect)
        {
                InternetCloseHandle(m_hInternetConnect);
                m_hInternetConnect = NULL;
        }
        
        m_oStr->Delete(&m_szServer);
        m_oStr->Delete(&m_szEndPoint);
        m_oStr->Delete(&m_szHeader);
        m_oStr->Delete(&m_szReceiveBuffer);
		
        return TRUE;
}

/********************************************************************
Function        DestroyContactVariables

Arguments:      

Returns:        

Comments:       clean up contact related memory
*********************************************************************/
BOOL CIssInternet::DestroyContactVariables()
{
        m_oStr->Delete(&m_szSaveFile);
        m_oStr->Delete(&m_szUrlRequest);
        m_oStr->Delete(&m_szUserName);
        m_oStr->Delete(&m_szPassword);
		m_oStr->Delete(&m_szHead);
        m_oStr->Delete(&m_szFoot);
		
        if(m_lpPostData)
            delete [] m_lpPostData;

        // reset UINTs and BOOL..
        m_szBuffer              = NULL;
        m_szBufferChar          = NULL;
        m_uiBuffLen             = 0;
        m_bPost                 = FALSE;
        m_bMultipart            = FALSE;
        m_uiPostDataLen         = 0;
        m_szRequestHeader       = NULL;
        m_uiRequestHeaderLen    = 0;
        m_lpPostData            = NULL;

        return TRUE;
}

BOOL CIssInternet::SetOAuth(TCHAR* szOAuth, BOOL bAuthAsIs)
{
	m_oStr->Delete(&m_szOAuth);
	m_bAuthAsIs = bAuthAsIs;
	if(!szOAuth)
		return FALSE;
	m_szOAuth = m_oStr->CreateAndCopy(szOAuth);
	return (m_szOAuth?TRUE:FALSE);
}

/********************************************************************
Function        DestroyContactParams

Arguments:      

Returns:        

Comments:       clean up Contact related memory
*********************************************************************/
BOOL CIssInternet::DestroyContactParams()
{
        if(m_hThreadDownload)
        {
                if(GetExitCodeThread(m_hThreadDownload, &m_dwExitCode))
                {
                        // if the thread is still going, forcefully kill it
                        if(m_dwExitCode == STILL_ACTIVE)
                                TerminateThread(m_hThreadDownload, 911);
                        
                        // memory cleanup
                        CloseHandle( m_hThreadDownload );                       
                }
                m_hThreadDownload = NULL;
        }

        if(m_hEventRead)
        {
                CloseHandle(m_hEventRead);
                m_hEventRead = NULL;
        }

        if(m_hEventOpen)
        {
                CloseHandle(m_hEventOpen);
                m_hEventOpen = NULL;
        }

        if(m_hEventFailed)
        {
                CloseHandle(m_hEventFailed);
                m_hEventFailed = NULL;
        }

        return TRUE;
}

/********************************************************************
Function        GetOSVersion

Arguments:      

Returns:        

Comments:       simplified retrieval of the OS version
*********************************************************************/
int CIssInternet::GetOSVersion()
{
        OSVERSIONINFO VersionInfo;
         memset(&VersionInfo, 0, sizeof(VersionInfo));
    VersionInfo.dwMajorVersion = 3; //Safe default (Pocket PC circa 2000).
    VersionInfo.dwMinorVersion = 0; //Safe default (Pocket PC circa 2000).

    //Get the version information for this device.
    GetVersionEx(&VersionInfo);
        if(VersionInfo.dwMajorVersion == 3 &&
       VersionInfo.dwMinorVersion == 0 &&
       VersionInfo.dwBuildNumber        < 11000)
    {
        return 2000;
    }
    else if(VersionInfo.dwMajorVersion == 3)
    {
        return 2002;
    }
        else if(VersionInfo.dwMajorVersion == 5)
                return 2005;

    return 2003;
}




#ifndef WIN32_PLATFORM_WFSP
#ifdef UNDER_CE
/********************************************************************
Function        Ping

Arguments:      szAddress       - address to ping
                        szName          - returned Name
                        prtt            - no idea
                        iWaitTime       - timeout

Returns:        -1: error in address, 0: ping FALSE, 1:ping TRUE

Comments:       Ping implementation
*********************************************************************/
int CIssInternet::Ping( TCHAR *Address, TCHAR *szName, int *prtt ,int iTtl, int iWaitTime)
{

        // WM2005 isn't compatible with this ping function when it's docked (as it takes a local 169 address)
        // so we just see if it is docked to determine if we are connected.   If we are sub Wm2005 or not docked, 
        //we ping as usual....this is assuming the actual computer the 2005 device is connected to has the internet...no workaround yet.
        if(GetOSVersion() == 2005)
        {
                if(IsDocked())
                {
                        return 1;
                }
        }
        
#ifdef ARMV4
        return (int)InternetCheckConnection(_T("http://www.yahoo.com"),FLAG_ICC_FORCE_CONNECTION ,0);
#endif

        /* Ping code that does work...but the Windows InternetcheckConnection might be better. */
        IPAddr                ipAddress ;
        IP_OPTION_INFORMATION ipInfo ;
        ICMP_ECHO_REPLY       icmpEcho; 
        HANDLE                hFile;    
        char                  strHost  [ MAX_PATH ] ;
        TCHAR                 szPCName [ MAX_PATH ]  = { TEXT( "" ) } ;
        int                   iRet = -1 ;
        struct in_addr iaDest;          // Internet address structure
    LPHOSTENT pHost;                    // Pointer to host entry structure

        _tcscpy( szName , TEXT("") );
        WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)Address , -1, strHost, sizeof( strHost ), NULL, FALSE );
    ipAddress = inet_addr(strHost);
    iaDest.s_addr = inet_addr(strHost);
    if (iaDest.s_addr == INADDR_NONE)
        {
        pHost = gethostbyname(strHost);
            if( pHost )
                {
                        char *pIP ;
                        iaDest.S_un.S_addr = *(DWORD *)(*pHost->h_addr_list) ;
                        pIP = inet_ntoa( iaDest ) ;
                    if( strlen( pIP ) )
                   MultiByteToWideChar( CP_ACP, 0, pIP , -1, szPCName, sizeof( szPCName ) );
                    if( _tcslen( szPCName ) )
                        _tcscpy( szName , szPCName );
                }
        }
    else
        {
        pHost = gethostbyaddr((const char *)&iaDest, 
                        sizeof(struct in_addr), AF_INET);
            if( pHost )
                {
                MultiByteToWideChar( CP_ACP, 0, pHost->h_name , -1, szPCName, sizeof( szPCName ) );
                    if( _tcslen( szPCName ) )
                        _tcscpy( szName , szPCName );
                }
        }
        if( pHost )
                ipAddress = *(DWORD *)(*pHost->h_addr_list) ;
    if (ipAddress != INADDR_NONE)
        {       
                iRet = 0 ;
                hFile = IcmpCreateFile();

                // Set some reasonable default values
                ipInfo.Ttl   = iTtl ;
                ipInfo.Tos   = 0;
                ipInfo.Flags = 0;
                ipInfo.OptionsSize = 0 ;
                ipInfo.OptionsData = NULL ;
                icmpEcho.Status    = IP_SUCCESS ;
                IcmpSendEcho( hFile , ipAddress , NULL , 0 ,
                                  (PIP_OPTION_INFORMATION)&ipInfo,
                                  &icmpEcho , sizeof(ICMP_ECHO_REPLY) , iWaitTime ) ;   
                IcmpCloseHandle(hFile) ;
                if ( icmpEcho.Status == IP_SUCCESS )
            iRet = 1 ;
                *prtt = icmpEcho.RoundTripTime ;
        }
        return iRet ;
        
}
#endif
#endif

/********************************************************************
Function        Init

Arguments:      hWndParent         - parent window to send messages to
                uiMessage          - WM_USER message to send custom message to
                bNewThread         - create a new thread when we call download
                bUseProxy          - use proxy server if needed
                bAsyncOnly         - Only connect if active sync
                bForceConnection   - if not connected will try and kick start a connection

Returns:        TRUE if successful

Comments:       Initialize the connection
*********************************************************************/
BOOL CIssInternet::Init(HWND    hWndParent, 
                                                UINT    uiMessage, 
                                                BOOL    bNewThread, 
                                                BOOL    bUseProxy, 
                                                BOOL    bAsyncOnly, 
                                                BOOL    bForceConnection,
												BOOL	bOverrideAgent)
{
        // kill anything that might be going right now
        Destroy();

        
        // Try and Open an internet session
        if(!OpenConnection(bOverrideAgent))
                return FALSE;

        m_hWndParent            = hWndParent;
        m_uiMessage                     = uiMessage;
        m_bNewThread            = bNewThread;
        m_bUseProxy                     = bUseProxy;
        m_bAsyncOnly            = bAsyncOnly;
        m_bForceConnection      = bForceConnection;
        m_bAssumeConnected      = FALSE;

        return TRUE;
}

/*

    PURPOSE:    Download a file from the web

    PARAMETERS: szSaveFile      - Download contents to this file
                szBuffer        - Return the contents of the download into this buffer.  can be NULL.
                szBufferChar    - Return the contents of the download into this buffer which is a CHAR.  can be NULL.
                uiBuffLen       - Length of the declared buffer
                szUrlRequest    - Server you want to download from Eg. www.implicitsoftware.com/currency.ini

    RESULT:     TRUE if it worked

    NOTES:      If szBuffer or szBufferChar is set then we HAVE to assume that the file we are downloading is a
                Text file.  If you want to download a non Text file you should only save it to file with szSaveFile.

*/
BOOL CIssInternet::DownloadFile(TCHAR*  szUrlRequest,
                                                                TCHAR*  szSaveFile, 
                                                                TCHAR*  szBuffer, 
                                                                UINT    uiBuffLen,
                                                                CHAR*   szBufferChar)
{
        if(szBuffer)
        {
                m_oStr->Empty(szBuffer);
        }

        if(szBufferChar)
        {
                m_oStr->Empty(szBufferChar);
        }

        return Contact(szUrlRequest, szSaveFile, szBuffer, szBufferChar, uiBuffLen, FALSE, NULL,0);
}

/*

    PURPOSE:    GET or POST a URL

    PARAMETERS: szSaveFile      - Download contents to this file
                szBuffer        - Return the contents of the download into this buffer.  can be NULL.
                uiBuffLen       - Length of the declared buffer
                szUrlRequest    - Server you want to download from Eg. http://www.implicitsoftware.com/currency.ini
                                  Note: you need the http:// for it to work properly
                bPost           - GET or POST
                szPostData      - Post your data.  Can be NULL
                                
    RESULT:     TRUE if it worked

*/
BOOL CIssInternet::Contact(TCHAR*       szUrlRequest,   
                           TCHAR*       szSaveFile,
                           TCHAR*       szBuffer,  
                           CHAR*        szBufferChar,
                           UINT         uiBuffLen,  
                           BOOL         bPost,  
                           LPVOID       lpPostData,
                           UINT         uiPostDataLen,
                           TCHAR*       szUserName,
                           TCHAR*       szPassword,
                           BOOL         bMultipart,
                           CHAR*       szHead,
                           CHAR*       szFoot,
                           TCHAR*       szRequestHeader,
                           UINT         uiRequestHeaderLen)
{
        // reset the number of Bytes downloaded
        m_iBytesDownloaded = 0;

        // if we want use Active Sync only
        if(m_bAsyncOnly)
        {
                if(!IsDocked())
                        return FALSE;
        }

        // if we're not trying to start up a connection then we want to fail if there is nothing
        // up at the moment
        if(!m_bForceConnection)
        {
                if(!IsConnected())
                        return FALSE;
        }

        // save these values so they can be used in the thread function
        if(!SaveContactVariables( szSaveFile,
                                                          szBuffer,
                                                          szBufferChar,
                                                          uiBuffLen,
                                                          szUrlRequest,
                                                          bPost,
                                                          lpPostData,
                                                          uiPostDataLen,
                                                          szUserName,
                                                          szPassword,
                                                          szHead,
                                                          szFoot,
                                                          szRequestHeader,
                                                          uiRequestHeaderLen))
                return FALSE;


        m_bMultipart = bMultipart;

        if(m_bNewThread)
        {
                // find out if the thread is going or if we used it last time so clean up any remaining
                // thread objects (Note: exitthread which is implicitly called won't do the last cleanup
                // so we have to do it ourselves

                if(m_hThreadContact)
                {
                        if(GetExitCodeThread(m_hThreadContact, &m_dwExitCode))
                        {
                                // if the thread is still going, we don't want to create another one
                                if(m_dwExitCode == STILL_ACTIVE)
                                        return FALSE;
                                
                                // memory cleanup
                                CloseHandle( m_hThreadContact );

                        }
                        m_hThreadContact = NULL;
                }
                
                m_hThreadContact = CreateThread(NULL, 0, &ThreadLaunchContact, (LPVOID)this, CREATE_SUSPENDED, NULL);

                // is the thread going?
                if(m_hThreadContact)
                {
                        // make sure it runs in the background
                        SetThreadPriority(m_hThreadContact, THREAD_PRIORITY_BELOW_NORMAL);
                        ResumeThread(m_hThreadContact);
                        return TRUE;
                }
                else
                        return FALSE;
        }
        else
        {
            if(bPost)
            {
                return fnDownloadPost();
            }
            else
            {
                if((m_szUserName && m_szPassword) || m_szHead || m_szOAuth)
                    return fnDownloadAlt();
                else
                    return fnDownloadGet();
            }
        }
        return TRUE;
}

/********************************************************************
Function        IsConnected

Arguments:      

Returns:        TRUE if we are connected

Comments:       are we currently connected?
*********************************************************************/
BOOL CIssInternet::IsConnected()
{
        
        if(GetTickCount() < 10000)
                return FALSE;

        if(m_bAssumeConnected)
        {
                return TRUE;
        }

    ULONG ulOutBufLen = 0;

    DWORD dwResult = GetAdaptersInfo(NULL, &ulOutBufLen);
    if(dwResult == ERROR_BUFFER_OVERFLOW)
    {
        //Got how long the buffer should be. Alloc the buffer.
        BYTE* pbBuffer = new BYTE[ulOutBufLen];
        if(pbBuffer)
        {
            dwResult = GetAdaptersInfo((PIP_ADAPTER_INFO)pbBuffer, &ulOutBufLen);
            IP_ADAPTER_INFO* pSearch = (PIP_ADAPTER_INFO)pbBuffer;
                        

            while(pSearch)
            {
                if(pSearch->CurrentIpAddress)
                                {
                                        delete [] pbBuffer;
                                        pbBuffer = NULL;
                                        return TRUE;
                }
                pSearch = pSearch->Next;
            }
            delete [] pbBuffer;
        }
    }

        return FALSE;
        
}

/********************************************************************
Function:       SaveContactVariables    

Arguments:      szSaveFile      - Optional: save everything to this filename (for downloading a file)
                szBuffer        - Optional: save to buffer (usually a string buffer)
                szBufferChar    - Optional: save to byte buffer so can be anything
                uiBuffLen       - length of the given buffer
                szUrlRequest    - URL we want to access
                bPost           - GET or POST
                szPostData      - Optional: data to post

Returns:        TRUE if successful

Comments:       Save variables to prepare for download/upload
*********************************************************************/
BOOL CIssInternet::SaveContactVariables(TCHAR*  szSaveFile,  
                                                                                TCHAR*  szBuffer,  
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
                                                                                TCHAR*       szRequestHeader,
                                                                                UINT         uiRequestHeaderLen)
{

        // clean up any old values
        DestroyContactVariables();

        // If we don't have the critical stuff, return False.
        if(!szUrlRequest)
        {
                return FALSE;
        }

		int iLenURL = m_oStr->GetLength(szUrlRequest);
		if(iLenURL <= 0)
			return FALSE;

        URL_COMPONENTS crackedURL;
        m_szServer              = new TCHAR[iLenURL + 1];
        m_szEndPoint            = new TCHAR[iLenURL + 1];
		ZeroMemory(m_szServer, sizeof(TCHAR)*(iLenURL+1));
		ZeroMemory(m_szEndPoint, sizeof(TCHAR)*(iLenURL+1));
        
        // crack the URL
        ZeroMemory(&crackedURL, sizeof(URL_COMPONENTS));
        crackedURL.dwStructSize         = sizeof(URL_COMPONENTS);
        crackedURL.lpszHostName         = m_szServer;
        crackedURL.dwHostNameLength     = iLenURL;
        crackedURL.lpszUrlPath          = m_szEndPoint;
        crackedURL.dwUrlPathLength      = iLenURL;
        
        // check and see if the URL is even valid
        BOOL bRet = InternetCrackUrl(szUrlRequest, 0, 0, &crackedURL);
        m_oStr->Delete(&m_szServer);
        m_oStr->Delete(&m_szEndPoint);

        if(!bRet)
                return FALSE;

        if(szSaveFile)
                m_szSaveFile = m_oStr->CreateAndCopy(szSaveFile);
        
        // this is to write it to a memory buffer
        if(szBuffer)
                m_szBuffer = szBuffer;

        if(szRequestHeader)
            m_szRequestHeader   = szRequestHeader;

        // this is to write it to a memory buffer
        if(szBufferChar)
                m_szBufferChar = szBufferChar;

        m_uiBuffLen             = uiBuffLen;
        m_uiRequestHeaderLen    = uiRequestHeaderLen;
        m_szUrlRequest  = m_oStr->CreateAndCopy(szUrlRequest);
        m_bPost                 = bPost;

        if(szUserName)
           m_szUserName         = m_oStr->CreateAndCopy(szUserName);

        if(szPassword)
            m_szPassword        = m_oStr->CreateAndCopy(szPassword);

        if(szHead)
            m_szHead            = m_oStr->CreateAndCopyChar(szHead);

        if(szFoot)
            m_szFoot            = m_oStr->CreateAndCopyChar(szFoot);

        if(m_bPost && lpPostData) 
        {
            m_lpPostData = new BYTE[uiPostDataLen+1];
            ZeroMemory(m_lpPostData, sizeof(BYTE)*(uiPostDataLen+1));
            memcpy(m_lpPostData, lpPostData, sizeof(BYTE)*uiPostDataLen);          
        }

        m_uiPostDataLen = uiPostDataLen;


        return TRUE;


}

DWORD WINAPI CIssInternet::ThreadLaunchContact(LPVOID lpVoid)
{
        CIssInternet*   oInternet = (CIssInternet*) lpVoid;
        
        return oInternet->fnContact();
}
 
DWORD WINAPI CIssInternet::ThreadDownload(LPVOID lpVoid)
{
        CIssInternet*   oInternet = (CIssInternet*) lpVoid;

        if(oInternet->m_bPost)
                return oInternet->fnDownloadPost();
        else
        {
            if((oInternet->m_szUserName && oInternet->m_szPassword) || oInternet->m_szHead)
                return oInternet->fnDownloadAlt();
            else
                return oInternet->fnDownloadGet();
        }
        return 0;
}

/********************************************************************
Function        fnContact

Arguments:      

Returns:        

Comments:       function for multi threaded download
*********************************************************************/
BOOL CIssInternet::fnContact()
{
        //serialize access.  this is ok for this implementation, since the calls are serialized anyway
        ::EnterCriticalSection( &m_CriticalSection );

        // cleanup
        DestroyContactParams();

        m_hEventOpen    = ::CreateEvent( NULL, TRUE, FALSE, NULL );     //manual, not signaled
        m_hEventRead    = ::CreateEvent( NULL, TRUE, FALSE, NULL );     //manual, not signaled
        m_hEventFailed  = ::CreateEvent( NULL, TRUE, FALSE, NULL );     //manual, not signaled
        if( !m_hEventOpen || !m_hEventRead || !m_hEventFailed)
        {
                DestroyContactParams();
                DestroyInternetParams();

                ::LeaveCriticalSection( &m_CriticalSection );
                return FALSE;
        }

        // if we want to forcefully start a connection using the connection manager
        // This is not guaranteed to work so we continue on if it fails

        /*
        if(m_bForceConnection)
        {
                if(m_oConnection.GetProxyServer())
                {
                        m_bUseProxy = TRUE;
                }
                else
                {
                        m_bUseProxy = FALSE;
                }
                HRESULT hr = m_oConnection.IsAvailable(L"http://www.google.com",m_bUseProxy );
                if(hr != S_OK)
                {
                        hr = m_oConnection.AttemptConnect(L"http://www.google.com", m_bUseProxy); //Try slow connect
    
                        //Wait for a result. At most 10 seconds
                        for(int i=0; i<20; i++)
                        {
                                if(IsConnected())
                                        break;                          

                                Sleep(500);
                        }
                }
        }
        */


        // tell the calling window that the connection has been started
        if(m_hWndParent && m_uiMessage > WM_USER)
                PostMessage(m_hWndParent,m_uiMessage,INET_CON_CREATED,0);

        m_hThreadDownload = CreateThread( NULL, 0, CIssInternet::ThreadDownload, (LPVOID)this, CREATE_SUSPENDED, NULL );
        
        if( m_hThreadDownload )
        {
                //Low priority so it doesn't take up too many time slices.
                SetThreadPriority( m_hThreadDownload, THREAD_PRIORITY_BELOW_NORMAL );
                ResumeThread(m_hThreadDownload);
        }

        HANDLE pHandles[2];

        pHandles[0]     = m_hEventOpen;
        pHandles[1]     = m_hEventFailed;

        //open the URL within eOpenTimeout, or kill and abort
        DWORD dwReturn = WaitForMultipleObjects(2, pHandles, FALSE, CONNECT_TIMEOUT);
        if(dwReturn == WAIT_TIMEOUT             ||
           (dwReturn - WAIT_OBJECT_0) == 1||
           dwReturn == WAIT_FAILED)
        {
                // cleanup
                DestroyContactParams();
                
                if(m_hWndParent && m_uiMessage > WM_USER)
                        PostMessage(m_hWndParent,m_uiMessage,INET_CON_ERROR_OPEN,0);

                // close any internet resources that might have been created in the thread
                DestroyInternetParams();

                m_iBytesDownloaded = 0;

                ::LeaveCriticalSection( &m_CriticalSection );
                return FALSE;
        }

        pHandles[0]     = m_hEventRead;
        pHandles[1]     = m_hEventFailed;

        //open the URL within eOpenTimeout, or kill and abort
        dwReturn = WaitForMultipleObjects(2, pHandles, FALSE, DOWNLOAD_TIMEOUT);
        if(dwReturn == WAIT_TIMEOUT             ||
           (dwReturn - WAIT_OBJECT_0) == 1||
           dwReturn == WAIT_FAILED)
        {
                // cleanup
                DestroyContactParams();

                if(m_hWndParent && m_uiMessage > WM_USER)
                        PostMessage(m_hWndParent,m_uiMessage,INET_CON_ERROR_READ,0);

                // close any internet resources that might have been created in the thread
                DestroyInternetParams();

                ::LeaveCriticalSection( &m_CriticalSection );

                m_iBytesDownloaded = 0;
                return FALSE;
        }

        // cleanup
        DestroyContactParams();

        ::LeaveCriticalSection( &m_CriticalSection );
        return TRUE;
}

BOOL CIssInternet::GetUserAgent(TCHAR* szAgent, BOOL	bOverrideAgent)
{
    CIssString* oStr = CIssString::Instance();

    if(!szAgent)
        return FALSE;

    oStr->Empty(szAgent);

	if(bOverrideAgent)
	{
		oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 4.01; Windows CE; PPC; 240x320)"));
		return TRUE;
	}

#ifdef UNDER_CE

    BOOL bIsSmartphone = FALSE;

    // check if we're using smartphone
    TCHAR szPlatform[STRING_NORMAL];
    ZeroMemory(szPlatform, sizeof(TCHAR)*STRING_NORMAL);
    if(SystemParametersInfo(SPI_GETPLATFORMTYPE, STRING_NORMAL, szPlatform, 0))
    {
        if(0 == oStr->Compare(szPlatform, _T("SmartPhone")))
        {
            bIsSmartphone = TRUE;
        }
    }

    switch(::GetOSVersion())
    {
    case OS_65:
        if(bIsSmartphone)
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; Windows Phone 6.5)"));
        else
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; Windows Phone 6.5)"));
        break;
    case OS_614:
        if(bIsSmartphone)
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 8.12; MSIEMobile 6.0)"));
        else
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 8.12; MSIEMobile 6.0)"));
        break;
    case OS_61:
        if(bIsSmartphone)
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 7.11)"));
        else
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 7.11)"));
        break;
    case OS_6:
        if(bIsSmartphone)
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 6.12)"));
        else
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 6.12)"));
        break;
    case OS_5:
        if(bIsSmartphone)
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 4.01; Windows CE; Smartphone; 176x220)"));
        else
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 4.01; Windows CE; PPC; 240x320)"));
        break;
    default:
    {
        char szTemp[STRING_MAX];
        DWORD dwSize = STRING_MAX;
        if(NO_ERROR != ObtainUserAgentString(0, szTemp, &dwSize))
            oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; PPC)"));
        else
            oStr->StringCopy(szAgent, szTemp);
    }
        break;
        
    }



    return TRUE;

#else
    char szTemp[STRING_MAX];
    DWORD dwSize = STRING_MAX;
    if(NO_ERROR != ObtainUserAgentString(0, szTemp, &dwSize))
        oStr->StringCopy(szAgent, _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; PPC)"));
    else
        oStr->StringCopy(szAgent, szTemp);

    return TRUE;

#endif
}

/********************************************************************
Function        OpenConnection

Arguments:      

Returns:        

Comments:       Open our internet connection.  This must be call first.
*********************************************************************/
BOOL CIssInternet::OpenConnection(BOOL	bOverrideAgent)
{
        if(m_hOpen)
        {
            // Close existing connection if we have one, so we can start fresh.
            InternetCloseHandle(m_hOpen);
            m_hOpen = NULL;
        }

        TCHAR szUserAgent[STRING_MAX] = _T("");
        GetUserAgent(szUserAgent, bOverrideAgent);

#ifdef UNDER_CE
        CIssConnection oConnection;

        if(E_FAIL == oConnection.IsAvailable(TEST_URL, TRUE))
            return FALSE;

        if(oConnection.GetProxyServer())
        {
            m_hOpen = InternetOpen(szUserAgent,INTERNET_OPEN_TYPE_PROXY,oConnection.GetProxyServer(),NULL,0);
        }
        else
            m_hOpen = InternetOpen(szUserAgent,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
            //m_hOpen = InternetOpen(szUserAgent,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
#else
        m_hOpen = InternetOpen(szUserAgent,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
#endif

        if(!m_hOpen)
        {
            DWORD dwError = GetLastError();
            return FALSE;
        }

        
        // If its not equal to NULL then we are using a proxy, and need its address
        // to pass into InternetOpen
//      if(lptProxy != NULL) 
//      {
//              m_hOpen = InternetOpen(USER_AGENT,INTERNET_OPEN_TYPE_PROXY,lptProxy,NULL,0);
//      }
//      else
//      {
//              //m_hOpen = InternetOpen(_T("CeHttp"),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
//              m_hOpen = InternetOpen(USER_AGENT,INTERNET_OPEN_TYPE_DIRECT,lptProxy,NULL,0);
//      }


//      m_hOpen = InternetOpen(USER_AGENT,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);

        DWORD dwTimeout = CONNECT_TIMEOUT - 5000;
        DWORD dwTimeOut = 1000*60*30;
        InternetSetOption(m_hOpen, INTERNET_OPTION_CONNECT_TIMEOUT,         &dwTimeout, sizeof(DWORD));

        //dwTimeout = 12000;
        InternetSetOption(m_hOpen, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeout, sizeof(DWORD));
        InternetSetOption(m_hOpen, INTERNET_OPTION_CONTROL_SEND_TIMEOUT,    &dwTimeout, sizeof(DWORD));

        
        

        return TRUE;
}

/********************************************************************
Function        ReadWebContents

Arguments:      hInetConnection

Returns:        

Comments:       We have a successful connection so now read what has been returned
*********************************************************************/
BOOL CIssInternet::ReadWebContents(HINTERNET hInetConnection)
{
        // do we want to save to a file
        if(m_szSaveFile)
        {
                m_hSaveFile = (HANDLE) CreateFile(m_szSaveFile,  GENERIC_WRITE,  FILE_SHARE_WRITE,
                                                                                NULL,                   CREATE_ALWAYS,  FILE_ATTRIBUTE_NORMAL,
                                                                                NULL);
        }
        
        DWORD dwWritten = 0;    
        
        // Read data onto disk until we are done 
        DWORD dwNumberOfBytesRead       = 0;
        DWORD dwBufWritten                      = 0;
        
        /*if(m_szReceiveBuffer)
        {
                delete [] m_szReceiveBuffer;
                m_szReceiveBuffer = NULL;
        }*/
        
        m_oStr->Delete(&m_szReceiveBuffer);
        m_szReceiveBuffer                       = new CHAR[SIZE_RECEIVE_BUFFER+1];
        ZeroMemory(m_szReceiveBuffer,SIZE_RECEIVE_BUFFER+1);
        
        // Find out the total bytes that we are going to read
        // BUGBUG:  for some reason this is not accurate - need to check Content-Length header for actual filesize...

		/*TCHAR szURL[4*STRING_MAX] = _T("");
		DWORD dwSize = sizeof(TCHAR)*STRING_MAX;
		if(!HttpQueryInfo(m_hOpen, HTTP_QUERY_LOCATION, NULL, &dwSize, NULL))
		{
			DWORD lastError = GetLastError();

			//ERROR_HTTP_HEADER_NOT_FOUND
			int i=0;
		}
		if(!HttpQueryInfo(m_hInternetConnect,
						HTTP_QUERY_LOCATION,
						szURL,
						&dwSize,
						NULL))
		{
			DWORD lastError = GetLastError();
			int i=0;
		}
        
        DWORD dwNumberofBytesAvailable = 0;
		if(!InternetQueryDataAvailable(m_hOpen, &dwNumberofBytesAvailable,0,0))
		{
			int i=0;
		}
		if(!InternetQueryDataAvailable(m_hInternetConnect, &dwNumberofBytesAvailable,0,0))
		{
			int i=0;
		}
		dwSize = sizeof(DWORD);
		if(!HttpQueryInfo(m_hInternetConnect,
						 HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,
						 &dwNumberofBytesAvailable,
						 &dwSize,
						 NULL))
        //if(InternetQueryDataAvailable(hInetConnection, &dwNumberofBytesAvailable,0,0))
        {
                // tell the calling window
                //if(m_hWndParent){}
                        //PostMessage(m_hWndParent, m_uiMessage, INET_CON_TOTAL_DL, dwNumberofBytesAvailable);
        }*/
        
        // start reading the contents of the HTML file
        while(InternetReadFile(hInetConnection, m_szReceiveBuffer, SIZE_RECEIVE_BUFFER, &dwNumberOfBytesRead))
        {
#ifdef UNDER_CE
            SystemIdleTimerReset();
#endif

            if(dwNumberOfBytesRead == 0 && m_szReceiveBuffer[0] == 0)
                break;

            if(dwNumberOfBytesRead == 0)
                dwNumberOfBytesRead = m_oStr->GetLength(m_szReceiveBuffer);

            // do we want to save the contents to a file
            if(m_szSaveFile)
            {
                if(!WriteFile((HANDLE)m_hSaveFile,m_szReceiveBuffer,dwNumberOfBytesRead, &dwWritten,NULL))
                {
                    // tell the calling window that our buffer is too damn small
                    if(m_hWndParent && m_uiMessage > WM_USER)
                            PostMessage(m_hWndParent,m_uiMessage,INET_CON_ERROR_BUF_LEN_SMALL,0);
                    
                    // cleanup
                    DestroyInternetParams();
                    
                    return FALSE;
                }
                
                // tell the calling window that we have downloaded more info
                if(m_hWndParent && m_uiMessage > WM_USER)
				{
					if(abs((int)(GetTickCount() - m_dwLastUpdate)) > 700)
					{
						PostMessage(m_hWndParent,m_uiMessage,INET_CON_WRITTEN_BYTES,dwWritten);
						m_dwLastUpdate = GetTickCount();
					}
				}
                        
            }
            
            // we have to add the null at the end so it doesn't go off into infinity
	        m_szReceiveBuffer[dwNumberOfBytesRead] = '\0';
            
            // save to a buffer?
            if(m_szBuffer || m_szBufferChar)
            {
                // if our max buffer length is not too big
                if(dwBufWritten + dwNumberOfBytesRead < m_uiBuffLen)
                {                                                               
                    if(m_szBuffer)
                        m_oStr->Concatenate(m_szBuffer,m_szReceiveBuffer);
                        //memcpy(m_szBuffer+dwBufWritten, m_szReceiveBuffer, sizeof(char)*dwNumberOfBytesRead);
                    if(m_szBufferChar)
                        //m_oStr->Concatenate(m_szBufferChar, m_szReceiveBuffer);
                        memcpy(m_szBufferChar+dwBufWritten, m_szReceiveBuffer, sizeof(char)*dwNumberOfBytesRead);
                    // tell the calling window that we have downloaded more info
                    if(m_hWndParent && m_uiMessage > WM_USER)
					{
						if(abs((int)(GetTickCount() - m_dwLastUpdate)) > 700)
						{
							PostMessage(m_hWndParent,m_uiMessage,INET_CON_WRITTEN_BYTES,dwNumberOfBytesRead);
							m_dwLastUpdate = GetTickCount();
						}
					}
                }
                else
                {
                    // tell the calling window that our buffer is too damn small
                    if(m_hWndParent && m_uiMessage > WM_USER)
                        PostMessage(m_hWndParent,m_uiMessage,INET_CON_ERROR_BUF_LEN_SMALL,0);

                    // cleanup
                    DestroyInternetParams();

                    m_oStr->Delete(&m_szReceiveBuffer);                             

                    return FALSE;
                }


            }

            dwBufWritten+=dwNumberOfBytesRead;

			m_iBytesDownloaded = dwBufWritten;

            ZeroMemory(m_szReceiveBuffer,SIZE_RECEIVE_BUFFER+1);

            // put a sleep here so other threads of the same priority can run (aka. simultaneous downloads)
            Sleep(5);
        }

        

        // signal the event that we are done reading
        if(m_hSaveFile)
        {
            CloseHandle((HANDLE)m_hSaveFile);
            m_hSaveFile = NULL;
        }
        //SetEvent(m_hEventRead);
        m_oStr->Delete(&m_szReceiveBuffer);

        
        return TRUE;
}

/********************************************************************
Function        CleanURL

Arguments:      

Returns:        

Comments:       quick clean of the URL string provided
*********************************************************************/
void CIssInternet::CleanURL()
{
        // if there is no HTTP in the beginning put it there
        if(m_oStr->Find(m_szUrlRequest,_T("http://")) == -1 && m_oStr->Find(m_szUrlRequest,_T("https://")) == -1)
        {
			TCHAR szNewURL[URL_SIZE];
			m_oStr->StringCopy(szNewURL, _T("http://"));
			m_oStr->Concatenate(szNewURL, m_szUrlRequest);
			m_oStr->Delete(&m_szUrlRequest);
			m_szUrlRequest = m_oStr->CreateAndCopy(szNewURL);
        }

        // if there is a / at the end take it off
        int iLen =  m_oStr->GetLength(m_szUrlRequest);
        if(m_szUrlRequest[iLen - 1] == _T('/'))
                m_szUrlRequest[iLen - 1] = _T('\0');
}

/********************************************************************
Function        fnDownloadAlt

Arguments:      

Returns:        

Comments:       There are two ways to download files and there is no way to
                        know which one will work so we try the other way first and 
                        then have this as a backup if it didn't work.
*********************************************************************/
BOOL CIssInternet::fnDownloadAlt()
{       
    // clean any resources
	HRESULT hr = S_OK;
	BOOL bRet;
    DWORD dwCount = 0;
	int iLenURL = m_oStr->GetLength(m_szUrlRequest);
    DestroyInternetParams();

	CBARG(m_hOpen, _T(""));
	CBARG(iLenURL > 0, _T(""));
    
    URL_COMPONENTS crackedURL;
    m_szServer      = new TCHAR[iLenURL + 1];
	CPHR(m_szServer, _T("m_szServer = new TCHAR[iLenURL+1];"));
	ZeroMemory(m_szServer, sizeof(TCHAR)*(iLenURL+1));

    m_szEndPoint    = new TCHAR[iLenURL+1];
	CPHR(m_szEndPoint, _T("m_szEndPoint = new TCHAR[];"));
	ZeroMemory(m_szEndPoint, sizeof(TCHAR)*(iLenURL+1));
    
    // crack the URL
    ZeroMemory(&crackedURL, sizeof(URL_COMPONENTS));
    crackedURL.dwStructSize         = sizeof(URL_COMPONENTS);
    crackedURL.lpszHostName         = m_szServer;
    crackedURL.dwHostNameLength     = iLenURL;
    crackedURL.lpszUrlPath          = m_szEndPoint;
    crackedURL.dwUrlPathLength      = iLenURL;
    
    // Clean up the URL string here
    CleanURL();
    
    bRet = InternetCrackUrl(m_szUrlRequest, 0, 0, &crackedURL);
    CBHR(bRet, _T("bRet = InternetCrackUrl(m_szUrlRequest, 0, 0, &crackedURL);"));

    m_hInternetConnect              = InternetConnect(m_hOpen, 
                                                    m_szServer,
                                                    crackedURL.nPort,
                                                    //m_szUserName,m_szPassword,
                                                    NULL, NULL,
                                                    INTERNET_SERVICE_HTTP,0,0);
    CPHR(m_hInternetConnect, _T("InternetConnect Failed"));

    DWORD dwTimeOut = 1000*60*30;
    InternetSetOption(m_hInternetConnect, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD));
    InternetSetOption(m_hInternetConnect, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD));
    InternetSetOption(m_hInternetConnect, INTERNET_OPTION_CONTROL_SEND_TIMEOUT,    &dwTimeOut, sizeof(DWORD));

    
    {
        LPTSTR  pszAcceptTypes [] = {TEXT("text/html, image/gif, image/jpeg, text/xml, */*"), NULL}; 
        m_hInternetRequest = HttpOpenRequest(m_hInternetConnect,
                                            _T("GET"),
                                            m_szEndPoint, 
                                            NULL, NULL,
                                            (LPCTSTR*)pszAcceptTypes,
                                            m_dwConnectionFlags,
                                            0);
    }
    
    CPHR(m_hInternetRequest, _T("HttpOpenRequest"));

    InternetSetOption(m_hInternetRequest, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD));
    InternetSetOption(m_hInternetRequest, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD));
    InternetSetOption(m_hInternetRequest, INTERNET_OPTION_CONTROL_SEND_TIMEOUT,    &dwTimeOut, sizeof(DWORD));
    
    m_szHeader = new TCHAR[2*URL_SIZE];
	CPHR(m_szHeader, _T(""));
    ZeroMemory(m_szHeader, sizeof(TCHAR)*2*URL_SIZE);

    if(m_szUserName && m_szPassword)
    {
        // have to flush the basic auth
        InternetSetOption(0, INTERNET_OPTION_END_BROWSER_SESSION, 0, 0);

        CIssBase64 oBase64;
        CHAR szBasicAuth[STRING_MAX];
        CHAR szReturned[STRING_MAX];
        ZeroMemory(szBasicAuth, sizeof(char)*STRING_MAX);
        ZeroMemory(szReturned, sizeof(char)*STRING_MAX);

        m_oStr->StringCopy(m_szHeader, m_szUserName);
        m_oStr->Concatenate(m_szHeader, _T(":"));
        m_oStr->Concatenate(m_szHeader, m_szPassword);

        int iChars = WideCharToMultiByte(CP_UTF8, 0, m_szHeader, m_oStr->GetLength(m_szHeader), szBasicAuth, STRING_MAX, NULL, NULL);

        oBase64.Encode((LPBYTE)szBasicAuth, 0, iChars, szReturned);

        m_oStr->StringCopy(m_szHeader, _T("Authorization: Basic "));
        m_oStr->Concatenate(m_szHeader, szReturned);
        m_oStr->Concatenate(m_szHeader, _T("\r\n"));
        m_oStr->Concatenate(m_szHeader,_T("Accept: text/html, image/gif, image/jpeg, text/xml, */*\r\n"));
    }
	else if(m_szOAuth)
	{
		// have to flush the basic auth
		InternetSetOption(0, INTERNET_OPTION_END_BROWSER_SESSION, 0, 0);

		m_oStr->StringCopy(m_szHeader, _T("Authorization: OAuth "));
		m_oStr->Concatenate(m_szHeader, m_szOAuth);
		m_oStr->Concatenate(m_szHeader, _T("\r\n"));
		m_oStr->Concatenate(m_szHeader,_T("Accept: text/html, image/gif, image/jpeg, text/xml, */*\r\n"));

	}
    else
        m_oStr->StringCopy(m_szHeader,_T("Accept: text/html, image/gif, image/jpeg, text/xml, */*\r\n"));

    if(m_szHead)
        m_oStr->Concatenate(m_szHeader, m_szHead);

    
    
    dwCount = 0;
    while(dwCount < 2)
    {
        // Try to send the request off for our data, if we can't request it, clean up, 
        // if successful save the data into the buffer with InternetReadFile
        bRet = HttpSendRequest(m_hInternetRequest, 
                            (LPCTSTR)m_szHeader,
                            m_oStr->GetLength(m_szHeader),
                            NULL, 
                            0);
        if(bRet)
            break;

        Sleep(5);
        DBG_OUT((_T("CIssInternet::fnDownloadAlt() - HttpSendRequest failed - retrying")));
        dwCount++;
    }
    CBHR(bRet, _T("HttpSendRequest"));

	DWORD dwError = getHttpCode();
	DBG_OUT((_T("CIssInternet::fnDownloadAlt() - return %d"), dwError));

	
	CBARG(dwError < HTTP_STATUS_BAD_REQUEST || dwError > HTTP_STATUS_RETRY_WITH, _T("GET ALT - Error"));
        
    if(m_hWndParent && m_uiMessage > WM_USER)
        PostMessage(m_hWndParent,m_uiMessage,INET_CON_GET_SUCCESS,0);

    // tell the Contact thread that we have opened properly
    if(m_hEventOpen)
        SetEvent(m_hEventOpen);

    bRet = ReadWebContents(m_hInternetRequest);
    CBHR(bRet, _T("bRet = ReadWebContents(m_hInternetConnect);"));

    
Error:

	if(hr != S_OK)
    {
        DWORD dwError = GetLastError();

#ifdef DEBUG
		HttpStatusText();
#endif

        // 12029 = ERROR_INTERNET_CANNOT_CONNECT - The attempt to connect to the server failed.

        DBG_OUT((_T("HTTP Error Code: %d"), dwError));

        TCHAR* sz = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL, dwError, 0,(LPTSTR)&sz, 0, NULL);
        if(sz)
        {
            DBG_OUT((sz));
            delete [] sz;
        }

        DBG_OUT((_T("m_szUrlRequest - %s"), m_szUrlRequest));
        if(m_szUserName)
            DBG_OUT((_T("m_szUserName - %s"), m_szUserName));
        if(m_szPassword)
            DBG_OUT((_T("m_szPassword - %s"), m_szPassword));

        DestroyInternetParams();

        if(m_hEventFailed)
            SetEvent(m_hEventFailed);

        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_GET_FAILED,0);
    }
    else
    {
        DestroyInternetParams();

        // tell the calling window everything worked properly
        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_DOWNLOAD_COMPLETE,0);

        if(m_hEventRead)
            SetEvent(m_hEventRead);
    }
    
    
	return hr==S_OK?TRUE:FALSE;
}

/********************************************************************
Function        fnDownloadGet

Arguments:      

Returns:        

Comments:       Steps for downloading a file off the "interweb"
*********************************************************************/
BOOL CIssInternet::fnDownloadGet()
{
    // clean any resources
    HRESULT hr = S_OK;
    DWORD dwCount = 0;
    BOOL bRet;
    DestroyInternetParams();

    CBARG(m_hOpen, _T(""));
    CBARG(m_oStr->GetLength(m_szUrlRequest) > 0, _T(""));
        

    if(m_szUserName && m_szPassword)
    {
        InternetSetOption(m_hOpen, INTERNET_OPTION_USERNAME, m_szUserName, m_oStr->GetLength(m_szUserName) + 1);
        InternetSetOption(m_hOpen, INTERNET_OPTION_PASSWORD, m_szPassword, m_oStr->GetLength(m_szPassword) + 1);
    }

    dwCount = 0;
    while(dwCount < 2)
    {
        m_hInternetConnect = InternetOpenUrl(m_hOpen, m_szUrlRequest, NULL, 0, m_dwConnectionFlags, 0);

        if(m_hInternetConnect)
            break;

        Sleep(5);
        DBG_OUT((_T("CIssInternet::fnDownloadGet() - InternetOpenUrl failed - retrying")));
        dwCount++;
    }
    CPHR(m_hInternetConnect, _T("InternetOpenUrl"));

	DWORD dwError = getHttpCode();
	DBG_OUT((_T("CIssInternet::fnDownloadGet() - return %d"), dwError));
	CBARG(dwError < HTTP_STATUS_BAD_REQUEST || dwError > HTTP_STATUS_RETRY_WITH, _T("GET - Error"));

    if(m_hWndParent && m_uiMessage > WM_USER)
        PostMessage(m_hWndParent,m_uiMessage,INET_CON_GET_SUCCESS,0);

    // tell the Contact thread that we have opened properly
    if(m_hEventOpen)
        SetEvent(m_hEventOpen);

    bRet = ReadWebContents(m_hInternetConnect);
    CBHR(bRet, _T("bRet = ReadWebContents(m_hInternetConnect);"));


Error:
    if(hr != S_OK)
    { 
        DWORD dwError = GetLastError();

        // 12029 = ERROR_INTERNET_CANNOT_CONNECT - The attempt to connect to the server failed.

        DBG_OUT((_T("HTTP Error Code: %d"), dwError));

        TCHAR* sz = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL, dwError, 0,(LPTSTR)&sz, 0, NULL);
        if(sz)
        {
            DBG_OUT((sz));
            delete [] sz;
        }

        DBG_OUT((_T("m_szUrlRequest - %s"), m_szUrlRequest));
        if(m_szUserName)
            DBG_OUT((_T("m_szUserName - %s"), m_szUserName));
        if(m_szPassword)
            DBG_OUT((_T("m_szPassword - %s"), m_szPassword));

        DestroyInternetParams();

        if(m_hEventFailed)
            SetEvent(m_hEventFailed);

        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_GET_FAILED,0);
    }
    else
    {
        DestroyInternetParams();

        // tell the calling window everything worked properly
        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_DOWNLOAD_COMPLETE,0);

        if(m_hEventRead)
            SetEvent(m_hEventRead);
    }

    
    return hr==S_OK?TRUE:FALSE;
}

/********************************************************************
Function        fnDownloadPost

Arguments:      

Returns:        

Comments:       function for posting data
*********************************************************************/
BOOL CIssInternet::fnDownloadPost()
{

    HRESULT hr = S_OK;
    DWORD dwError = HTTP_STATUS_OK;
    BOOL bRet = TRUE;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    TCHAR* szURLFileName = NULL;
	int iLenURL = m_oStr->GetLength(m_szUrlRequest);
    
	CBARG(m_hOpen, _T(""));
	CBARG(iLenURL > 0, _T(""));
    
    // clean any resources
    DestroyInternetParams();
    
    URL_COMPONENTS crackedURL;
	m_szServer      = new TCHAR[iLenURL + 1];
	CPHR(m_szServer, _T("m_szServer = new TCHAR[iLenURL+1];"));
	ZeroMemory(m_szServer, sizeof(TCHAR)*(iLenURL+1));

	m_szEndPoint    = new TCHAR[iLenURL+1];
	CPHR(m_szEndPoint, _T("m_szEndPoint = new TCHAR[];"));
	ZeroMemory(m_szEndPoint, sizeof(TCHAR)*(iLenURL+1));

	// crack the URL
	ZeroMemory(&crackedURL, sizeof(URL_COMPONENTS));
	crackedURL.dwStructSize         = sizeof(URL_COMPONENTS);
	crackedURL.lpszHostName         = m_szServer;
	crackedURL.dwHostNameLength     = iLenURL;
	crackedURL.lpszUrlPath          = m_szEndPoint;
	crackedURL.dwUrlPathLength      = iLenURL;
    
    // Clean up the URL string here
    CleanURL();
    
    bRet = InternetCrackUrl(m_szUrlRequest, 0, 0, &crackedURL);
    CBHR(bRet, _T("InternetCrackUrl"));
       
    m_hInternetConnect              = InternetConnect(  m_hOpen, 
                                                        m_szServer,
                                                        crackedURL.nPort,
                                                        m_szUserName,m_szPassword,
                                                        INTERNET_SERVICE_HTTP,0,0);
    CPHR(m_hInternetConnect, _T("InternetConnect"));
             
    LPTSTR  pszAcceptTypes [] = {TEXT("application/x-www-form-urlencoded;charset=utf-8"), NULL}; 
	//LPTSTR  pszAcceptTypes [] = {TEXT("*/*"), NULL}; 
    LPTSTR  pszAcceptTypesMulti [] = {TEXT("*/*"), NULL}; 

	DWORD dwConnectionFlags = m_dwConnectionFlags;

	if(m_bMultipart)
	{
		dwConnectionFlags = INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
	}
	else if(-1 != m_oStr->Find(m_szUrlRequest, _T("https")))
	{
		dwConnectionFlags |= INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP|INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS|INTERNET_FLAG_IGNORE_CERT_DATE_INVALID|INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
	}

    m_hInternetRequest = HttpOpenRequest(m_hInternetConnect,
                                        _T("POST"),
                                        m_szEndPoint, 
                                        NULL, NULL,
                                        (m_bMultipart?(LPCTSTR*)pszAcceptTypesMulti:(LPCTSTR*)pszAcceptTypes),
                                        //(m_bMultipart?INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT | INTERNET_FLAG_RELOAD:m_dwConnectionFlags),
                                        dwConnectionFlags,
                                        0);
    CPHR(m_hInternetRequest, _T("HttpOpenRequest"));
    
    m_szHeader = new TCHAR[SIZE_Post+1];
	CPHR(m_szHeader, _T(""));
    ZeroMemory(m_szHeader, sizeof(TCHAR)*(SIZE_Post+1));

	if(m_szOAuth && m_bAuthAsIs)
	{
		m_oStr->StringCopy(m_szHeader, m_szOAuth);
		m_oStr->Concatenate(m_szHeader, _T("\r\n"));

		bRet = HttpAddRequestHeaders( m_hInternetRequest, m_szHeader, -1, HTTP_ADDREQ_FLAG_ADD);
		CBHR(bRet, _T("HttpAddRequestHeaders oauth"));

		ZeroMemory(m_szHeader, sizeof(TCHAR)*(SIZE_Post+1));
	}
	else if(m_szOAuth)
	{

		m_oStr->StringCopy(m_szHeader, _T("Authorization: OAuth "));
		m_oStr->Concatenate(m_szHeader, m_szOAuth);
		m_oStr->Concatenate(m_szHeader, _T("\r\n"));

		bRet = HttpAddRequestHeaders( m_hInternetRequest, m_szHeader, -1, HTTP_ADDREQ_FLAG_ADD);
		CBHR(bRet, _T("HttpAddRequestHeaders oauth"));

		ZeroMemory(m_szHeader, sizeof(TCHAR)*(SIZE_Post+1));
	}
    
    if(m_bMultipart && m_szHead && m_szFoot && m_lpPostData)
    {

        m_oStr->Format(m_szHeader,_T("Content-Type: multipart/form-data; boundary=%s\r\n"), TEXT_Boundary);

        int iLenHead = m_oStr->GetLength(m_szHead);
        int iLenTail = m_oStr->GetLength(m_szFoot);

        bRet = HttpAddRequestHeaders( m_hInternetRequest, m_szHeader, -1, HTTP_ADDREQ_FLAG_ADD);
        CBHR(bRet, _T("HttpAddRequestHeaders multipart"));

        TCHAR szFileName[STRING_MAX];
        m_oStr->StringCopy(szFileName, (TCHAR*)m_lpPostData);
        hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        CBARG(hFile != INVALID_HANDLE_VALUE, _T(""));

        INTERNET_BUFFERS BufferIn = {0};
        BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );

		DWORD dwTotalFileSize = GetFileSize(hFile, NULL);

        BufferIn.dwBufferTotal = iLenHead + iLenTail + dwTotalFileSize;

#ifdef UNDER_CE
		SystemIdleTimerReset();
#endif

        bRet = HttpSendRequestEx( m_hInternetRequest, &BufferIn, NULL, HSR_INITIATE, 0);
        CBHR(bRet, _T("HttpSendRequestEx"));

        DWORD dwSum = 0;
        unsigned long ulWrote;

        // stream header
        bRet = InternetWriteFile(m_hInternetRequest, (const void*)m_szHead, iLenHead, &ulWrote);
        CBHR(bRet, _T("InternetWriteFile"));
        dwSum += ulWrote;

        DWORD dwBytesRead;
        BYTE pBuffer[1024]; // Read from file in 1K chunks

        // stream binary file
        do
        {
#ifdef UNDER_CE
            SystemIdleTimerReset();
#endif

            bRet = ReadFile(hFile, pBuffer, sizeof(pBuffer), &dwBytesRead, NULL);
            if(!bRet)
                break;

            bRet = InternetWriteFile( m_hInternetRequest, pBuffer, dwBytesRead, &ulWrote);
            if(!bRet)
                break;

            dwSum += ulWrote;

			if(m_hWndParent && m_uiMessage > WM_USER)
			{
				// make sure we don't send too many messages and overload the system
				//if(abs(GetTickCount() - m_dwLastUpdate) > 700)
				{
					//PostMessage(m_hWndParent, m_uiMessage, INET_CON_POST_PCT, dwSum*100/dwTotalFileSize);
					SendMessage(m_hWndParent, m_uiMessage, INET_CON_POST_PCT, dwSum*100/dwTotalFileSize);
					m_dwLastUpdate = GetTickCount();
				}
			}

        }while (dwBytesRead == sizeof(pBuffer));


        // stream tailer
        bRet = InternetWriteFile(m_hInternetRequest, (const void*)m_szFoot, iLenTail, &ulWrote);
        CBHR(bRet, _T("InternetWriteFile"));
        dwSum += ulWrote;

        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;


        bRet = HttpEndRequest(m_hInternetRequest, NULL, HSR_INITIATE, 0);
        CBHR(bRet, _T("HttpEndRequest"));

    }
    else
    {
        m_oStr->StringCopy(m_szHeader,_T("Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n"));
    
        // Try to send the request off for our data, if we can't request it, clean up, 
        // if successful save the data into the buffer with InternetReadFile
        bRet = HttpSendRequest(m_hInternetRequest, 
                            (LPCTSTR)m_szHeader,
                            m_oStr->GetLength(m_szHeader),
                            m_lpPostData, 
                            m_uiPostDataLen);
        CBHR(bRet, _T("HttpSendRequest"));
    }

	dwError = getHttpCode();
	DBG_OUT((_T("CIssInternet::fnDownloadPost() - return %d"), dwError));
	CBARG(dwError < HTTP_STATUS_BAD_REQUEST || dwError > HTTP_STATUS_RETRY_WITH, _T("POST - Error"));

    if(m_hWndParent && m_uiMessage > WM_USER)
        PostMessage(m_hWndParent,m_uiMessage,INET_CON_POST_SUCCESS,0);

    // tell the Contact thread that we have opened properly
    if(m_hEventOpen)
        SetEvent(m_hEventOpen);

    if(m_oStr->GetLength(m_szRequestHeader) > 0 && m_uiRequestHeaderLen > 0)
    {
        DWORD dwLen = (DWORD)m_uiRequestHeaderLen;
        bRet = HttpQueryInfo(m_hInternetRequest,
            HTTP_QUERY_CUSTOM,
            m_szRequestHeader,
            &dwLen, 0);
    }

    // try and read the page
    bRet = ReadWebContents(m_hInternetRequest);
    CBHR(bRet, _T("ReadWebContents(m_hInternetRequest)"));
            
    

Error:
    // cleanup
    DestroyInternetParams();

    if(hr != S_OK)
    {

        if(dwError != HTTP_STATUS_OK)
        {
            DBG_OUT((_T("HTTP Error Code: %d"), dwError));

            TCHAR* sz = NULL;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL, dwError, 0,(LPTSTR)&sz, 0, NULL);
            if(sz)
            {
                DBG_OUT((sz));
                delete [] sz;
            }
        }        

        if(m_hEventFailed)
            SetEvent(m_hEventFailed);

        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_POST_FAILED,0);
    }
    else
    {
        // tell the calling window everything worked properly
        if(m_hWndParent && m_uiMessage > WM_USER)
            PostMessage(m_hWndParent,m_uiMessage,INET_CON_DOWNLOAD_COMPLETE,0);

        if(m_hEventRead)
            SetEvent(m_hEventRead);
    }

    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    
    m_oStr->Delete(&szURLFileName);

    return (hr==S_OK?TRUE:FALSE);
}
BOOL CIssInternet::StartRequest(TCHAR* szUrlRequest)
{
	// check for bad values
	if(m_oStr->IsEmpty(szUrlRequest) || 
	   m_hOpen == NULL)
		return FALSE;

	// clean out Internet parameters
	DestroyInternetParams();

	// send off our request
	m_hInternetConnect = InternetOpenUrl(m_hOpen, szUrlRequest, NULL, 0, m_dwConnectionFlags, 0);
	if(!m_hInternetConnect)
	{
		DWORD dwError = GetLastError();
		return FALSE;
	}

    TCHAR szBuffer[STRING_MAX];
    ZeroMemory(szBuffer, sizeof(TCHAR) * STRING_MAX);
    DWORD dwBufSize = STRING_MAX;

    HttpQueryInfo(m_hInternetConnect, HTTP_QUERY_STATUS_CODE,&szBuffer, &dwBufSize,NULL);

    int iReturnCode = m_oStr->StringToInt(szBuffer);

    return (iReturnCode == HTTP_STATUS_OK? TRUE:FALSE);

}

BOOL CIssInternet::GetRequestHeader(TCHAR* szHeaderName, TCHAR* szResult, DWORD dwResultLen)
{
	// check for bad values
	if(m_oStr->IsEmpty(szHeaderName) || 
	   szResult == NULL				 || 
	   dwResultLen == 0				 ||
	   m_hInternetConnect == NULL)
	   return FALSE;

	m_oStr->StringCopy(szResult, szHeaderName);
	
	// read in the appropriate header
	BOOL bReturn = HttpQueryInfo(m_hInternetConnect,
								 HTTP_QUERY_CUSTOM,
								 szResult,
								 &dwResultLen, 0);

	return bReturn;
}


DWORD CIssInternet::getHttpCode()
{
	DWORD rc = 0;
	DWORD resultlen = sizeof(DWORD);
	BOOL bReturn = HttpQueryInfo(m_hInternetRequest?m_hInternetRequest:m_hInternetConnect,
								 HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE,
								 &rc,
								 &resultlen, 0);

	return rc;
}

void CIssInternet::HttpStatusText()
{
	TCHAR szStatus[4*STRING_MAX];
	DWORD dwLen = sizeof(TCHAR)*4*STRING_MAX;
	ZeroMemory(szStatus, dwLen);

	BOOL bReturn = HttpQueryInfo(m_hInternetRequest?m_hInternetRequest:m_hInternetConnect,
		HTTP_QUERY_STATUS_TEXT,
		szStatus,
		&dwLen, 0);

	DBG_OUT((_T("Status Text - return %s"), szStatus));
}

/********************************************************************
Function        IsDocked

Arguments:      

Returns:        

Comments:       Are we currently docked?
*********************************************************************/
BOOL CIssInternet::IsDocked(CRITICAL_SECTION* csSection)
{
        
#ifdef UNDER_CE
        if(csSection)
                EnterCriticalSection(csSection);
        
    //First, check if the TCPIP reg key is present.
    //We add this step, as the 2nd IP address check seems to stay around after the WiFi connection has been disconnected.
    HKEY hKey = NULL;
    LRESULT lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Comm\\Tcpip\\Hosts\\ppp_peer"), 0L, 0L, &hKey);

    if(lr == ERROR_SUCCESS && hKey)
    {
        //Yes, we're really connected.
        RegCloseKey(hKey);
        hKey = NULL;
    }
        else
        {
                
                // we may have WM2005...don't return just yet.
                lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Comm\\Tcpip\\Hosts\\dtpt_peer"), 0L, 0L, &hKey);
                
                if(lr == ERROR_SUCCESS && hKey)
                {
                        //Yes, we're really connected.
                        RegCloseKey(hKey);
                        hKey = NULL;
                }
                else
                {
                        if(csSection)
                                LeaveCriticalSection(csSection);

                        
                        // we're not connected at all on either Wm2005 or earlier devices.
                        return FALSE;
                }
        }
        
        if(csSection)
                LeaveCriticalSection(csSection);


    //If this reg key was present, we are connected to the web.
    //However, we could be connected via WiFi or GPRS. Check - are we actually docked?
    ULONG ulOutBufLen = 0;
        CIssString* oStr = CIssString::Instance();

    DWORD dwResult = GetAdaptersInfo(NULL, &ulOutBufLen);
    if(dwResult == ERROR_BUFFER_OVERFLOW)
    {
        //Got how long the buffer should be. Alloc the buffer.
        BYTE* pbBuffer = new BYTE[ulOutBufLen];
        if(pbBuffer)
        {
            dwResult = GetAdaptersInfo((PIP_ADAPTER_INFO)pbBuffer, &ulOutBufLen);
            IP_ADAPTER_INFO* pSearch = (PIP_ADAPTER_INFO)pbBuffer;

            while(pSearch)
            {
                if(pSearch->CurrentIpAddress)
                {
                    if( (strcmp(pSearch->CurrentIpAddress->IpAddress.String, "192.168.55.101") == 0) || 
                                                (strcmp(pSearch->CurrentIpAddress->IpAddress.String, "192.168.55.100") == 0) ||
                                                (oStr->Find(pSearch->CurrentIpAddress->IpAddress.String, "169.254.") != -1))
                    {
                        //We're docked - detected a USB connection.
                        delete [] pbBuffer;
                                                
                        return TRUE;
                    }
                }
                pSearch = pSearch->Next;
            }
            delete [] pbBuffer;
        }
    }

    return FALSE;
#else
        return TRUE;
#endif
}

HRESULT CIssInternet::UploadFile(TCHAR* szUrlRequest, 
                                 TCHAR* szFilePath, 
                                 TCHAR* szSaveFile, 
                                 TCHAR* szUsername, 
                                 TCHAR* szPassword,
                                 TCHAR* szHead,
                                 TCHAR* szFoot)
{
    HRESULT hr = S_OK;
    BOOL bRet = TRUE;
    TCHAR* szURLFileName = NULL;

    CHAR szNewHead[SIZE_Post];
    CHAR szNewTail[SIZE_Post];
    ZeroMemory(szNewHead, sizeof(char)*SIZE_Post);
    ZeroMemory(szNewTail, sizeof(char)*SIZE_Post);

    CBARG(szUrlRequest&&szFilePath, _T(""));

    int iLen = m_oStr->GetLength(szHead);
    if(iLen > 0)
    {
        //m_oStr->StringCopy(szNewHead, szHead);
        int iChars = WideCharToMultiByte(CP_UTF8, 0, szHead, iLen, szNewHead, SIZE_Post, NULL, NULL);
        CBARG(iChars > 0, _T("int iChars = WideCharToMultiByte(CP_UTF8, 0, szHead, iLen, szNewHead, SIZE_Post, NULL, NULL);"));
    }
    else
    {
        // default header
        TCHAR szFileName[STRING_MAX];
        TCHAR szFileType[STRING_NORMAL];

        m_oStr->StringCopy(szFileName, szFilePath);

        int iLen    = m_oStr->GetLength(szFilePath);
        int iIndex  = m_oStr->FindLastOccurance(szFilePath, _T("."));
        CBARG(iIndex != -1, _T("m_oStr->FindLastOccurance(szFilePath"));
        m_oStr->StringCopy(szFileType, szFilePath, iIndex + 1, iLen - iIndex - 1);
        iIndex      = m_oStr->FindLastOccurance(szFilePath, _T("\\"));
        if(iIndex == -1)
            m_oStr->StringCopy(szFileName, szFilePath);
        else
            m_oStr->StringCopy(szFileName, szFilePath, iIndex + 1, iLen - iIndex - 1);


        szURLFileName = URLEncoder::Encode(szFileName);
        CPHR(szURLFileName, _T("szURLFilename = URLEncoder::Encode(szFileName);"));

        m_oStr->StringCopy(szNewHead, "--");
        m_oStr->Concatenate(szNewHead, TEXT_Boundary);
        m_oStr->Concatenate(szNewHead, "\r\nContent-Disposition: form-data; name=\"image\"; filename=\"");
        m_oStr->Concatenate(szNewHead, szURLFileName);
        m_oStr->Concatenate(szNewHead, "\"\r\nContent-Type: image/");
        m_oStr->Concatenate(szNewHead, szFileType);
        m_oStr->Concatenate(szNewHead, "\r\n\r\n");
    }

    iLen = m_oStr->GetLength(szFoot);
    if(iLen > 0)
    {
        int iChars = WideCharToMultiByte(CP_UTF8, 0, szFoot, iLen, szNewTail, SIZE_Post, NULL, NULL);
        CBARG(iChars > 0, _T("int iChars = WideCharToMultiByte(CP_UTF8, 0, szFoot, iLen, szNewTail, SIZE_Post, NULL, NULL);"));
        //m_oStr->StringCopy(szNewTail, szFoot);
    }   
    else
    {
        // default footer
        m_oStr->StringCopy(szNewTail, "\r\n--");
        m_oStr->Concatenate(szNewTail, TEXT_Boundary);
        m_oStr->Concatenate(szNewTail, "--\r\n");
    }

    //char szPostData[STRING_MAX];
    //m_oStr->StringCopy(szPostData, szFilePath);
    iLen = sizeof(TCHAR)*m_oStr->GetLength(szFilePath);
    bRet = Contact(szUrlRequest, szSaveFile, NULL, NULL, 0, TRUE, szFilePath, iLen, szUsername, szPassword, TRUE, szNewHead, szNewTail);
    CBHR(bRet, _T("Contact"));

Error:

    m_oStr->Delete(&szURLFileName);
    return hr;
}

HRESULT	CIssInternet::GetRedirectURL(TCHAR* szURL, TCHAR* szReturnedURL)
{
	HRESULT hr = S_OK;
	BOOL bRet;

	HINTERNET hInternetConnect = NULL;
	HINTERNET hInternetRequest = NULL;

	CBARG(szURL && szReturnedURL, _T(""));

	m_oStr->StringCopy(szReturnedURL, szURL);

	TCHAR szUserAgent[STRING_MAX] = _T("");
	GetUserAgent(szUserAgent);

	hInternetConnect = InternetOpen(szUserAgent,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_SECURE);

	//hInternetConnect = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_SECURE);
	CPHR(hInternetConnect, _T("InternetOpen Failed"));

	hInternetRequest = InternetOpenUrl(hInternetConnect,szURL,NULL,0, INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_SECURE, 0);
	CPHR(hInternetRequest, _T("InternetOpenUrl Failed"));

	TCHAR szBuffer[STRING_MAX];
	ZeroMemory(szBuffer, sizeof(TCHAR) * STRING_MAX);
	DWORD dwBufSize = STRING_MAX;

	bRet = HttpQueryInfo(hInternetRequest, HTTP_QUERY_STATUS_CODE,&szBuffer, &dwBufSize,NULL);
	CBHR(bRet, _T("HttpQueryInfo"));

	int iReturnCode = m_oStr->StringToInt(szBuffer);


Error:

	if(hInternetRequest)
		InternetCloseHandle(hInternetRequest);

	if(hInternetConnect)
		InternetCloseHandle(hInternetConnect);

	return hr;
}