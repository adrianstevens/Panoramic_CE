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
#pragma once

#include "winsock.h"
#include "IssString.h"

#define        SOCK_Error   -2
#define        SOCK_Nothing -3


enum EnumTcpMode
{
    MODE_Client,
    MODE_Server,
};

class CIssTcp
{
public:
    CIssTcp(void);
    ~CIssTcp(void);

    HRESULT    Init(int iPort, EnumTcpMode eMode = MODE_Client);
    HRESULT    Open(TCHAR* szDestAddress, int iPort);
    HRESULT    Destroy();
    HRESULT    Accept(TCHAR* szSrcAddress, int& iPort);
    int        Recieve(LPVOID lpBuf, DWORD dwBufferSize, DWORD dwTimeout = INFINITE);
    HRESULT    Send(LPVOID lpBuf, int iLength, DWORD dwTimeout = INFINITE);
    BOOL       IsConnected();
    HRESULT    GetLocalIP(TCHAR* szIP, TCHAR* szLocalName);
    BOOL       IsDataToRead();
    HRESULT    SetNonBlocking(int iNonBlocking);

private:    // variables
    CIssString*        m_oStr;
    SOCKET             m_tcpSocket;
    sockaddr_in        m_sDestAddress;
    sockaddr_in        m_sLocalAddress;
    sockaddr_in        m_sRemoteIP;
    int                m_iAddressLen;
    EnumTcpMode        m_eMode;
    CRITICAL_SECTION  m_csSocket;
};
