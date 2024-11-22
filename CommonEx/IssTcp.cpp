/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssTcp.h
// Abstract: Create, handle, and destroy a TCP client/server
//  
/***************************************************************************************************************/ 
#include "IssTcp.h"
#include "tchar.h"
#include "stdio.h"
#include "IssDebug.h"

#ifdef UNDER_CE
#pragma comment( lib, "winsock.lib" )
#else
#pragma comment(lib, "Ws2_32.lib")
#endif

#define MSG_HEADER_LEN 6



CIssTcp::CIssTcp(void)
:m_tcpSocket(INVALID_SOCKET)
,m_oStr(CIssString::Instance())
{
    InitializeCriticalSection(&m_csSocket);
}

CIssTcp::~CIssTcp(void)
{
    Destroy();
    DeleteCriticalSection(&m_csSocket);
}

/********************************************************************
Function:    Destroy

Arguments:        

Returns:    

Comments:    Tear down any open TCP connections
*********************************************************************/
HRESULT CIssTcp::Destroy()
{
    if(m_tcpSocket != INVALID_SOCKET)
    {
        closesocket(m_tcpSocket);
        WSACleanup();
        m_tcpSocket = INVALID_SOCKET;
    }

    return S_OK;
}

/********************************************************************
Function:    GetLocalIP

Arguments:    szIp        - retrieve local ip
            szLocalName    - retrieve local name

Returns:    

Comments:    get our network name and ip
*********************************************************************/
HRESULT CIssTcp::GetLocalIP(TCHAR* szIP, TCHAR* szLocalName)
{
    HRESULT hr = S_OK;

    HOSTENT *LocalAddress = NULL;
    char    *szBuff = NULL;

    CBARG(szIP && szLocalName, _T("szIp or szLocalName are NULL"));

    // Create new string buffer
    szBuff = new char[STRING_MAX];
    CPHR(szBuff, _T("szBuff not created"));

    // Reset the string buffer
    memset(szBuff, '\0', STRING_MAX);
    m_oStr->Empty(szIP);
    m_oStr->Empty(szLocalName);

    // Get computer name
    CBARG(gethostname(szBuff, STRING_MAX) == 0, _T("gethostname failed"));

    m_oStr->StringCopy(szLocalName, szBuff);
    
    // Get the local PC IP address
    LocalAddress = gethostbyname(szBuff);

    // Reset the string buffer
    memset(szBuff, '\0', 256);
    // Compose the obtain ip address
    sprintf(szBuff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF, LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
    
    m_oStr->StringCopy(szIP, szBuff);

Error:

    m_oStr->Delete(&szBuff);
    return hr;
}

/********************************************************************
Function:    IsConnected

Arguments:        

Returns:    

Comments:    do we currently have a TCP connection up?
*********************************************************************/
BOOL CIssTcp::IsConnected()
{
    //EnterCriticalSection(&m_csSocket);
    // see if our tcp socket is even valid
    if(m_tcpSocket==INVALID_SOCKET||m_tcpSocket == NULL)
        return FALSE;

    // BUGBUG: Not exactly sure why... but this doesn't work in Server mode
    // will have to redo to find out the state of the connections
    /*if(m_eMode == MODE_Client)
    {
        fd_set    fdWrite  = { 0 };

        SOCKET s = (SOCKET) m_tcpSocket;

        // Set Descriptor
        if ( !FD_ISSET( s, &fdWrite ) )
            FD_SET( s, &fdWrite );

        // Select function set write timeout
        int res = select( (int)s+1, NULL, &fdWrite, NULL, NULL );
        if ( res <= 0)
            return FALSE;
    }*/
    //LeaveCriticalSection(&m_csSocket);

    return TRUE;
};


// ------------------------------------------------------------
// Init -- Init TCP socket to modem
//
// Inputs: szPort    - port number to listen on
//           eMode    - Client or server
// 
// Outputs:
//  
// Returns: true if success, fail otherwise
//  
// Notes:
// 
HRESULT CIssTcp::Init(int iPort, EnumTcpMode eMode)
{
    HRESULT hr = S_OK;

    // clean out old memory
    hr = Destroy();
    CHR(hr, _T("Destroy failed"));

    m_eMode = eMode;

    WSADATA wsaData;
    //int rc;

    BOOL bDontLinger = TRUE;
    LINGER sLingerOpt;

    do 
    {  // do-while for easy break on error
        // Note: This is very important to have in there as it will stop working after a while
        if (WSAStartup(MAKEWORD(2,2),&wsaData)) 
            break;

        m_tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_tcpSocket == INVALID_SOCKET)
            break;

        sLingerOpt.l_onoff = 0;
        sLingerOpt.l_linger = 0;

        CBARG(setsockopt(m_tcpSocket, SOL_SOCKET, SO_DONTLINGER, (char*) &bDontLinger, sizeof(BOOL)) == 0, _T("setsockopt failed"))
        CBARG(setsockopt(m_tcpSocket, SOL_SOCKET, SO_LINGER, (char*) &sLingerOpt, sizeof(LINGER)) == 0, _T("setsockopt failed"));

        if (eMode == MODE_Client)
            goto Error;
        else   //  (SERVER == mode)
        {
            m_sLocalAddress.sin_family        = AF_INET;
            m_sLocalAddress.sin_addr.s_addr = INADDR_ANY; 
            m_sLocalAddress.sin_port        = htons(iPort);

            if (SOCKET_ERROR == bind(m_tcpSocket, (SOCKADDR *)&m_sLocalAddress, sizeof(m_sLocalAddress)))
                break;
            if (SOCKET_ERROR == listen(m_tcpSocket, SOMAXCONN))
                break;
            else
                goto Error;
        }
    } while (0);

Error:
    if(hr != S_OK)
    {
        WSACleanup();
        if(m_tcpSocket != INVALID_SOCKET)
            closesocket(m_tcpSocket);
    }
    return hr;
}

// ------------------------------------------------------------
// Accept -- Accepts a connection request
//
// Inputs:    szSrcAddress    - IP address of incoming connection request
//            szPort            - new port number to continue communication on
//
// Outputs:
//
// Returns:    true if success, fail otherwise
//
HRESULT CIssTcp::Accept(TCHAR* szSrcAddress, int& iPort)
{
    HRESULT hr = S_OK;
    m_iAddressLen = 16;
    m_tcpSocket = accept(m_tcpSocket, (struct sockaddr*) &m_sRemoteIP, &m_iAddressLen);
    CBARG(INVALID_SOCKET != m_tcpSocket, _T("accept failed"))

    m_oStr->StringCopy(szSrcAddress, inet_ntoa(m_sRemoteIP.sin_addr));
    iPort = ntohs(m_sRemoteIP.sin_port);
    
Error:
    return hr;
}

// ------------------------------------------------------------
// Open -- Open TCP socket to server
//
// Inputs:    destAddress    - remote IP address to connect to
//            remPort        - remote port number to connect to
// 
// Outputs:
//  
// Returns:    1        - on non-blocking sockets, it may not be possible to connect
//                      immediately.  Therefore, we signal that the connection is
//                      attempting a connection and the calling function should wait
//                      for it.  Wait time is determined by the calling function.
//            0        - no error, connection successful
//            error    - error occurred, connect command unsuccessful
//  
// Notes:
// 
HRESULT CIssTcp::Open (TCHAR* szDestAddress, int iPort)
{
    HRESULT hr = S_OK;

    CBARG(!m_oStr->IsEmpty(szDestAddress), _T("szDestAddress is not set"));

    // make sure to convert the name here to an IP address
    char szName[STRING_MAX];
    m_oStr->StringCopy(szName, szDestAddress);

    hostent* hEnt;

    // If the user input is an alpha name for the host, use gethostbyname()
    // If not, get host by addr (assume IPv4)
    if (isalpha(szName[0])) 
    {
        hEnt = gethostbyname(szName);
        CBARG(hEnt, _T("gethostbyname failed"));
        CBARG(hEnt->h_addrtype == AF_INET, _T("hEnt->h_addrtype == AF_INET"));

        in_addr **ppip=(in_addr**)hEnt->h_addr_list; 
        in_addr ip=**ppip;
        char* szValue = inet_ntoa(ip);
        CBARG(szValue, _T("inet_ntoa failed"));
        m_oStr->StringCopy(szName, szValue);
    }

    m_sDestAddress.sin_family        = AF_INET;
    m_sDestAddress.sin_addr.s_addr    = inet_addr(szName);
    m_sDestAddress.sin_port            = htons(iPort);

    int iReturn = connect(m_tcpSocket, (SOCKADDR *) &m_sDestAddress, sizeof(m_sDestAddress));
    CBARG(iReturn==0, _T("connect failed"));
Error:
    return hr;
}


// ------------------------------------------------------------
// Recieve -- Receive TCP packet
//
// Inputs:    buf        - address of receive buffer
//    
// 
// Outputs:
//  
// Returns:    rc        - number of bytes received
//  
// Notes:
//    Checks for any available data.  First makes a call to get the size of the data.
//
int    CIssTcp::Recieve(LPVOID lpBuf, DWORD dwBufferSize, DWORD dwTimeout)
{
    if (m_tcpSocket == INVALID_SOCKET)
        return INVALID_SOCKET;

    /*fd_set    fdRead  = { 0 };
    fd_set  fdExcept = {0};
    TIMEVAL    stTime;
    TIMEVAL    *pstTime = NULL;

    if ( INFINITE != dwTimeout ) 
    {
        stTime.tv_sec    = dwTimeout/1000;
        stTime.tv_usec    = dwTimeout % 1000;
        pstTime            = &stTime;
    }

    FD_ZERO(&fdRead);
    FD_ZERO(&fdExcept);

    FD_SET(m_tcpSocket,&fdRead);
    FD_SET(m_tcpSocket,&fdExcept);

    if(select(0,&fdRead,NULL,&fdExcept,&stTime)==SOCKET_ERROR)
    {
        //LeaveCriticalSection(&m_csSocket);
        return SOCK_Error;
    }
    if(FD_ISSET(m_tcpSocket,&fdExcept))
    {
        //LeaveCriticalSection(&m_csSocket);
        return SOCK_Nothing;
    } 
    if(!FD_ISSET(m_tcpSocket,&fdRead))
    {
        //LeaveCriticalSection(&m_csSocket);
        return SOCK_Nothing;
    } */

    //unsigned long NonBlockingEnabled = 0;
    //ioctlsocket(m_tcpSocket, FIONBIO, &NonBlockingEnabled);

    // we have something to read
    memset(lpBuf, 0, sizeof(char)*dwBufferSize);

    // try and read some data
    int iReturn = recv(m_tcpSocket, (char*)lpBuf, dwBufferSize, 0);

    return iReturn;
}

// ------------------------------------------------------------
// Send -- Send TCP packet
//
// Inputs:    szDestAddress    - remote address to send to
//            szRemPort        - remote port to send to
//            lpBuf            - address of transmit buffer
//            iLength            - size of message
// 
// Outputs:
//  
// Returns:    true        - send completed successfully
//            false        - send unsuccessful
//  
// Notes:
// 
HRESULT CIssTcp::Send(LPVOID lpBuf, int iLength, DWORD dwTimeout)
{
    HRESULT hr = S_OK;

    CBARG(m_tcpSocket != INVALID_SOCKET, _T("m_tcpSocket = INVALID_SOCKET"));
    CBARG(lpBuf && iLength != 0, _T("lpBuf && iLength != 0"));

    //unsigned long NonBlockingEnabled = 0;
    //ioctlsocket(m_tcpSocket, FIONBIO, &NonBlockingEnabled);

    fd_set fdWrite;
    fd_set fdExcept;

    int iResp;

    while(TRUE)
    {
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(m_tcpSocket,&fdWrite);
        FD_SET(m_tcpSocket,&fdExcept);
        
        iResp = select(0,NULL,&fdWrite,&fdExcept,NULL);
        CBARG(iResp!=SOCKET_ERROR, _T("select returns SOCKET_ERROR"));

        FD_ISSET(m_tcpSocket,&fdExcept);
 
        if(FD_ISSET(m_tcpSocket,&fdWrite))
        {    
            char* szBuf        = (char*)lpBuf;
            int iLengthLeft = iLength;
            int iWritten    = 0;
            int iCount        = 0;

            while(iCount < 100 && iWritten != iLengthLeft && iLengthLeft > 0 && m_tcpSocket)
            {
                iLengthLeft -= iWritten;
                szBuf        += iWritten;

                // send our data
                iWritten = send(m_tcpSocket, szBuf, iLengthLeft, 0);

                iCount ++;
                if(iWritten != iLengthLeft)
                    Sleep(500);
            }
            break;
        }

        // give it some time
        Sleep(1000);
    }

Error:

    return hr;
}

/********************************************************************
Function:    IsDataToRead

Arguments:        

Returns:    TRUE if there is data to read right now

Comments:    Is there data to read?
*********************************************************************/
BOOL CIssTcp::IsDataToRead()
{
    if(!m_tcpSocket)
        return FALSE;

    unsigned long arg;

    // check if there is something to read
    if (ioctlsocket( m_tcpSocket, FIONREAD, &arg ) == SOCKET_ERROR)
    {
        Destroy();
        return FALSE;
    }

    if (arg > 0) // check if there is any data in the read buffer
        return TRUE;
    else
        return FALSE;
}

/********************************************************************
Function:    SetNonBlocking

Arguments:    iNonBlocking - which mode to set to

Returns:    

Comments:    Set the blocking mode
*********************************************************************/
HRESULT CIssTcp::SetNonBlocking(int iNonBlocking)
{
    HRESULT hr = S_OK;
    CBARG(m_tcpSocket, _T("m_tcpSocket Invalid"));

    unsigned long NonBlockingEnabled = iNonBlocking;
    ioctlsocket(m_tcpSocket, FIONBIO, &NonBlockingEnabled);

Error:
    return hr;
}
