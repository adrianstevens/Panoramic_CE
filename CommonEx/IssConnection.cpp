#include "IssConnection.h"

#ifdef UNDER_CE
CIssConnection::CIssConnection()
{
    m_hThreadStop = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hConnectionThread = NULL;
    m_pszPath = NULL;
    m_uiMessage = 0;
    m_hWnd = NULL;
}

CIssConnection::~CIssConnection()
{
    if(m_hConnectionThread)
    {
        DWORD dwExitCode = 0;
        if(GetExitCodeThread(m_hConnectionThread, &dwExitCode))
        {
            // if the thread is still going, forcefully kill it
            if(dwExitCode == STILL_ACTIVE)
                TerminateThread(m_hConnectionThread, 999);

            // memory cleanup
            CloseHandle( m_hConnectionThread );
        }
        m_hConnectionThread = NULL;
    }
    m_hConnectionThread = NULL;
    HangupConnection();

    if( m_hThreadStop )
    {
        CloseHandle( m_hThreadStop );
        m_hThreadStop=NULL;
    }


    delete [] m_pszPath;
    m_pszPath = NULL;
}


void CIssConnection::SetParent(HWND hWnd, UINT uiMessage)
{
    m_hWnd = hWnd;
    m_uiMessage = uiMessage;
}


//
// Can we get to this resource 'quickly'
//
HRESULT CIssConnection::IsAvailable( LPCTSTR pszPath, BOOL bProxy)
{
    HRESULT hr=GetNetworkFromPath(pszPath);
    SetProxy(bProxy);

    if(SUCCEEDED(hr))
    {
        HANDLE hConn = NULL;

        CONNMGR_CONNECTIONINFO ci = { 0 };
        ci.cbSize           = sizeof(ci);
        ci.dwParams         = CONNMGR_PARAM_GUIDDESTNET | CONNMGR_PARAM_MAXCONNLATENCY;
        ci.dwFlags          = bProxy?CONNMGR_FLAG_PROXY_HTTP /*| CONNMGR_FLAG_PROXY_SOCKS5*/ :0;
        ci.ulMaxConnLatency = 4000;         // 4 second
        ci.bDisabled        = FALSE; //example had TRUE
        ci.bExclusive		= FALSE; //not in the example
        ci.dwPriority       = CONNMGR_PRIORITY_USERINTERACTIVE;
        ci.guidDestNet      = GetNetworkGuid();

        if (SUCCEEDED(hr=ConnMgrEstablishConnection(&ci, &hConn)))
        {
            DWORD dwResult = WaitForSingleObject(hConn, 400);

            switch (dwResult)
            {
            case WAIT_OBJECT_0:
                {
                    DWORD dwStatus;
                    HRESULT hResult;

                    hResult = ConnMgrConnectionStatus(hConn, &dwStatus);

                    if( SUCCEEDED(hResult) && ( (dwStatus == CONNMGR_STATUS_CONNECTED) || (dwStatus == CONNMGR_STATUS_CONNECTIONDISABLED) ))
                    {
                        hr=S_OK;
                        m_bProxyRequired = CheckForRequiredProxy(hConn);
                    }
                    else if(dwStatus == CONNMGR_STATUS_NOPATHTODESTINATION)
                    {
                        hr = E_FAIL;
                    }
                    else
                    {
                        hr=S_FALSE;
                    }
                    break;
                }

            case WAIT_TIMEOUT:
                hr=E_FAIL;
                break;
            }
            ConnMgrReleaseConnection(hConn, FALSE);
        }
    }
    return hr;
}

//
// Attempts to connect to the network
// overried the Do* methods to see what's happening.
//
HRESULT CIssConnection::AttemptConnect( LPCTSTR pszPath, BOOL bProxy)
{
    HRESULT hr=GetNetworkFromPath(pszPath);
    SetProxy(bProxy);

    if( m_hThreadStop == NULL )
        return E_INVALIDARG;

    if( SUCCEEDED(hr) )
    {
        if( SUCCEEDED ( hr=HangupConnection() ))
        {
            // kick off new thread,
            DWORD dwDummy;

            m_hConnectionThread = CreateThread(NULL, 0, s_ConnectionThread, (LPVOID)this, 0, &dwDummy);

            if ( m_hConnectionThread == NULL )
                hr=E_FAIL;
        }
    }
    return hr;
}

HRESULT CIssConnection::HangupConnection()
{
    if( m_hConnectionThread )
    {
        SetEvent(m_hThreadStop);
        //wait a few seconds at most for the thread to die
        WaitForSingleObject(m_hThreadStop, 3000); 

        if(m_hConnectionThread)
        {
            DWORD dwExitCode = 0;
            if(GetExitCodeThread(m_hConnectionThread, &dwExitCode))
            {
                // if the thread is still going, forcefully kill it
                if(dwExitCode == STILL_ACTIVE)
                    TerminateThread(m_hConnectionThread, 912);

                // memory cleanup
                CloseHandle( m_hConnectionThread );
            }
            m_hConnectionThread = NULL;
        }
        m_hConnectionThread = NULL;
    }
    return S_OK;
}


HRESULT CIssConnection::DoStatusUpdate(DWORD dwStatus)
{
    HRESULT hRes=S_OK;
    if( dwStatus & CONNMGR_STATUS_DISCONNECTED )
    {
        if( dwStatus != CONNMGR_STATUS_DISCONNECTED )
            hRes = DoConnectingError();
        else
            hRes= DoDisconnected();

        SetCache( dwStatus == CONNMGR_STATUS_DISCONNECTED );
    }
    else if ( dwStatus == CONNMGR_STATUS_CONNECTED )
    {
        hRes = DoConnected();
    }
    else if ( dwStatus & CONNMGR_STATUS_WAITINGCONNECTION )
    {   
        hRes = DoWaitingForConnection();
    }
    return hRes;
}

//
// Override these methods
//

//
// Called when we 
//
HRESULT CIssConnection::DoEstablishingConnection()
{
    return S_OK;
}

//
// Called when there was an error while connecting
// generally due to network connection not being available (no modem, no nic etc).
//
HRESULT CIssConnection::DoConnectingError()
{
    // we received an error to do with connecting.
    SHELLEXECUTEINFO sei = {0};
    TCHAR szExec[MAX_PATH];
    wsprintf( szExec, TEXT("-CMERROR 0x%x -report"), GetStatus() );
    sei.cbSize = sizeof(sei);
    sei.hwnd = NULL;
    sei.lpFile = TEXT(":MSREMNET");
    sei.lpParameters = szExec;
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteEx( &sei );
    return E_FAIL;
}

//
// Called when a connection is now available.
//
HRESULT CIssConnection::DoConnected()
{
    CheckForRequiredProxy( GetConnection() );

    if(m_hWnd)
        PostMessage(m_hWnd,m_uiMessage,INET_DIALUP_CONNECTION_MADE,0);

    return S_OK;
}

//
// Called when the existing connection has been disconnected
// by another network request. we return E_FAIL to hangup here
//
HRESULT CIssConnection::DoDisconnected()
{
    if(m_hWnd)
        PostMessage(m_hWnd,m_uiMessage,INET_DIALUP_CONNECTION_LOST,0);
    return E_FAIL;
}

//
// Called when we are waiting for the network to become available.
//
HRESULT CIssConnection::DoWaitingForConnection()
{
    return S_OK;
}

//
// Called when we have released the connection
//
HRESULT CIssConnection::DoReleaseConnection()
{
    return S_OK;
}

//
// Sets the network GUID from a path.
//
HRESULT CIssConnection::GetNetworkFromPath(LPCTSTR pszPath)
{
    if( pszPath )
    {
        if( m_pszPath )
        {
            delete [] m_pszPath;
            m_pszPath = NULL;
        }
        m_pszPath = new TCHAR[lstrlen(pszPath)+1];
        if( m_pszPath == NULL ) 
            return E_OUTOFMEMORY;
        lstrcpy(m_pszPath, pszPath);
    }
    return ConnMgrMapURL(m_pszPath, &m_gNetwork, 0);
}

//
// Thread stub cast and calls.
//
DWORD CIssConnection::s_ConnectionThread(LPVOID pData)
{
    CIssConnection * pConnection=(CIssConnection*)pData;
    if( pData )
        return pConnection->ConnectionThread();
    return 0;
}


//
// Thread proc
// Starts a connection to the network
//
DWORD CIssConnection::ConnectionThread()
{
#ifdef ARMV4
    if(!InternetAttemptConnect(0) == ERROR_SUCCESS)
#endif
    {
        HANDLE hThisThread=m_hConnectionThread;
        CONNMGR_CONNECTIONINFO ConnInfo={0};
        ConnInfo.cbSize=sizeof(ConnInfo);


        ConnInfo.dwParams=CONNMGR_PARAM_GUIDDESTNET ;
        ConnInfo.dwFlags=IsProxyRequired() ? CONNMGR_FLAG_PROXY_HTTP /*| CONNMGR_FLAG_PROXY_SOCKS5*/ : 0;
        ConnInfo.dwPriority=CONNMGR_PRIORITY_USERINTERACTIVE ;
        ConnInfo.guidDestNet = GetNetworkGuid();

        HRESULT hr = ConnMgrEstablishConnection(&ConnInfo, &m_hConnection);
        if( FAILED( hr ) )
        {
            DoConnectingError();
            SetCache(FALSE);
        }
        else
        {
            DoEstablishingConnection();

            HANDLE hObjects[2];
            hObjects[0]=m_hConnection;
            hObjects[1]=m_hThreadStop;
            BOOL    bStop=FALSE;

            ResetEvent(m_hThreadStop);

            while( bStop == FALSE )
            {
                DWORD dwResult = WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE); 

                if (dwResult == (WAIT_OBJECT_0))
                { 
                    HRESULT hr;
                    DWORD   dwStatus;
                    hr=ConnMgrConnectionStatus(m_hConnection,&dwStatus);
                    m_dwStatus = dwStatus;
                    if( SUCCEEDED(hr))
                    {
                        if( DoStatusUpdate(m_dwStatus) != S_OK )
                            bStop=TRUE;
                    }
                    else
                    {
                        m_dwStatus=hr;
                        bStop=TRUE;
                    }
                }
                else // failures, or signalled to stop.
                {
                    bStop = TRUE;
                    ResetEvent(m_hThreadStop);
                }
            }
        }

        DoReleaseConnection();

        // Release the connection, caching if we should.
        if( m_hConnection )
        {
            ConnMgrReleaseConnection(m_hConnection, GetCache() );
        }

        CloseHandle(hThisThread);

        return GetStatus();
    }
#ifdef ARMV4
    else
    {
        if(m_hWnd)
            PostMessage(m_hWnd,m_uiMessage,INET_DIALUP_CONNECTION_MADE,0);
        return S_OK;
    }
#endif

}

BOOL CIssConnection::CheckForRequiredProxy(HANDLE hConn)
{
    const GUID My_IID_ConnPrv_IProxyExtension	= 
    { 0xaf96b0bd, 0xa481, 0x482c, { 0xa0, 0x94, 0xa8, 0x44, 0x87, 0x67, 0xa0, 0xc0 } };

    m_bProxyRequired=FALSE;
    ZeroMemory(&m_ProxyInfo, sizeof(m_ProxyInfo));
    m_ProxyInfo.dwType = CONNMGR_FLAG_PROXY_HTTP;
    if (SUCCEEDED(ConnMgrProviderMessage(   hConn,
        &My_IID_ConnPrv_IProxyExtension,
        NULL,
        0,
        0,
        (PBYTE)&m_ProxyInfo,
        sizeof(m_ProxyInfo))))
    {
        if (m_ProxyInfo.dwType == CONNMGR_FLAG_PROXY_HTTP)
        {
            m_bProxyRequired=TRUE;
            // SECURITY: Zero out the username/password from memory.
            //ZeroMemory(&(m_ProxyInfo.szUsername), sizeof(m_ProxyInfo.szUsername));
            //ZeroMemory(&(m_ProxyInfo.szPassword), sizeof(m_ProxyInfo.szPassword));
        }
    }
    return m_bProxyRequired;
}
#endif
