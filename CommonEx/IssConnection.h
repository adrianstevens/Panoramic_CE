#pragma once

// WINCE Only

#ifdef UNDER_CE
#include "windows.h"
#include <connmgr.h>
#include <connmgr_proxy.h>

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

class CIssConnection
{
public:
    CIssConnection();
    virtual ~CIssConnection();

    void SetParent(HWND hWnd, UINT uiMessage);

    //
    // Can we get to this resource 'quickly'
    //
    HRESULT IsAvailable( LPCTSTR pszPath=NULL, BOOL bProxy=FALSE );

    //
    // Attempts to connect to the network
    // overried the Do* methods to see what's happening.
    //
    HRESULT AttemptConnect( LPCTSTR pszPath=NULL, BOOL bProxy=FALSE );
    HRESULT HangupConnection();

    BOOL    IsProxyRequired() { return m_bProxyRequired; }
    TCHAR*	GetProxyUserName(){return m_ProxyInfo.szUsername;};
    TCHAR*	GetProxyPassword(){return m_ProxyInfo.szPassword;};

    //
    // Properties on this object
    //
    HANDLE  GetConnection() { return m_hConnection; }
    DWORD   GetStatus()     { return m_dwStatus; }
    void    SetCache(BOOL b){ m_bCache=b; }
    BOOL    GetCache() { return m_bCache; }
    GUID    GetNetworkGuid() { return m_gNetwork; }
    BOOL    GetProxy() { return m_bProxy; }
    void    SetProxy(BOOL bProxy) { m_bProxy = bProxy; }
    LPTSTR  GetNetworkPath() { return m_pszPath; }
    LPTSTR  GetProxyServer() { if(m_bProxyRequired) return m_ProxyInfo.szProxyServer; else return NULL; }

    static LPTSTR GetInternetPath() { return CIssConnection::s_szInternetPath; }
    static LPTSTR GetCorpNetPath() { return CIssConnection::s_szCorpNetPath; }

    HWND	m_hWnd;
    UINT	m_uiMessage;



protected:      // Methods to override.
    //
    // return S_OK to carry on.
    // return E_FAIL to hangup the connection
    //
    virtual HRESULT DoStatusUpdate(DWORD dwStatus);

    //
    // Override these methods
    //

    //
    // Called when we 
    //
    virtual HRESULT DoEstablishingConnection();

    //
    // Called when there was an error while connecting
    // generally due to network connection not being available (no modem, no nic etc).
    //
    virtual HRESULT DoConnectingError();

    //
    // Called when a connection is now available.
    //
    virtual HRESULT DoConnected();

    //
    // Called when the existing connection has been disconnected
    // by another network request. we return E_FAIL to hangup here
    //
    virtual HRESULT DoDisconnected();

    //
    // Called when we are waiting for the network to become available.
    //
    virtual HRESULT DoWaitingForConnection();

    //
    // Called when we have released the connection
    //
    virtual HRESULT DoReleaseConnection();

    //
    // Sets the network GUID from a path.
    //
    HRESULT GetNetworkFromPath(LPCTSTR pszPath);

    //
    // Thread stub cast and calls.
    //
    static DWORD s_ConnectionThread(LPVOID pData);

    //
    // Thread proc
    // Starts a connection to the network
    //
    DWORD ConnectionThread();

private:
    LPTSTR  m_pszPath;
    BOOL    m_bProxy;
    HANDLE  m_hConnection;  // Connection Manager Handle
    BOOL    m_bCache;       // should we cache this connection when we 'hangup'
    GUID    m_gNetwork;     // the GUID for the network we are connecting to.
    DWORD   m_dwStatus;      // last connection status
    HANDLE  m_hThreadStop;  // Event
    HANDLE  m_hConnectionThread;    // Thread
    BOOL    m_bProxyRequired;

    BOOL    CheckForRequiredProxy(HANDLE hConn);


    // Rigged paths that will map  to the correct GUID.
    static LPTSTR s_szInternetPath;
    static LPTSTR s_szCorpNetPath;

    PROXY_CONFIG    m_ProxyInfo;
};
#endif

