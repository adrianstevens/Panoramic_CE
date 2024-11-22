// FtpSockClient.cpp: implementation of the CFtpSockClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Ftptest.h"
#include "FtpSockClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		RES_CODE_SEGMENT	3
#define		SERVER_READY		220
#define		USER_OK				331
#define		PASS_OK				230
#define		DIR_OK				257
#define		CHDIR_OK			250
#define		DELDIR_OK			250
#define		MOVE_OK				350
#define		PORT_OK				200
#define		DATA_OK				150
#define		GET_OK				226
#define		SESSION_OVER		221

//#define		MOVE_DONE		350


#define		POS_HYPHEN		3
#define		CRLF			_T("\r\n")
#define		DIR_SEPERATOR	_T("/")
#define		BLANK			_T("")
#define		MAX_BUFFER		256
#define		ZERO			0

#define		ASCII			_T("A")
#define		IMAGE			_T("I")

#define		ERR_SOCK_LOOKUP		5000
#define		ERR_SOCK_CREATE		5001
#define		ERR_SOCK_LISTEN		5002
#define		ERR_SOCK_PORT		5003
#define		ERR_SOCK_ACCEPT		5004
#define		ERR_SOCK_LOCFILE	5004

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpSockClient::CFtpSockClient( )
				:m_pCtrlSock( NULL ),
				m_bSession( FALSE ),
				m_csHost( _T("") ),
				m_csTransferMode(_T("A"))
{
	
}

CFtpSockClient::~CFtpSockClient()
{
	CloseChannel();
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::CreateChannel(CString csServer, UINT unPort )
{
	ASSERT( m_pCtrlSock == NULL );

	//	allocate socket from heap
	if( ! ( m_pCtrlSock = new CSocket ) )
		return FALSE;

	if( ! m_pCtrlSock -> Create(unPort, SOCK_STREAM, csServer))
	{
		SetErrorCode( ERR_SOCK_CREATE );
		return FALSE;
	}

	//	The memory was allocated fine and the create attempt was successfull
	return TRUE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

void CFtpSockClient::CloseChannel()
{
	if( m_pCtrlSock != NULL )
	{
		delete m_pCtrlSock;
		m_pCtrlSock = NULL;
	}

} 
/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::ConnectToServer( CString csServer, UINT unPort )
{

	// First create the socket.
	CreateChannel(csServer, unPort);

	ASSERT ( m_pCtrlSock != NULL );

	ASSERT ( csServer.GetLength() > 0 );
	
	if( m_pCtrlSock == NULL )
	{
		TRACE(_T("\nCode Message - Control socket has not been created ! \r\n") );
	
		return FALSE;
	}

	m_bSession = m_pCtrlSock -> Connect( csServer, unPort );

	// This while debugging will trace the output in the ouput window
	int nErrorCode = GetLastError();
	TraceErrorCode( nErrorCode );

	CString csWelcome = GetResponse();
	if( GetErrorCode() != SERVER_READY ) 
		m_bSession = FALSE;

	return m_bSession;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::DisconnectFromServer()
{
	ASSERT( m_pCtrlSock != NULL );
	
	if( m_pCtrlSock == NULL )
	{
		TRACE(_T("\nCode Message - Control socket has not been created ! \r\n") );
	
		return FALSE;
	}

	ASSERT ( m_bSession );

	if( ! m_bSession )
	{
		TRACE(_T("\nCode Message - No Session Established"));
		return TRUE;
	}


	BOOL bRetVal = SendCommand( L"QUIT" );
	GetResponse();

	if( !bRetVal || GetErrorCode( ) != SESSION_OVER )
	{
		TRACE(_T("\nServer Message - Could not disconnect session from server \r\n"));
		return FALSE;
	}

	CloseChannel();

	return TRUE;
}
/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::UserLogin( CString csUserName, CString csPassword ) 
{

	CString csCommand = _T("USER ") + csUserName;

	BOOL bRetVal = SendCommand( csCommand );
	GetResponse();

	if( !bRetVal || GetErrorCode( ) != USER_OK )
	{
		TRACE(_T("\nServer Message - User access not allowed. \r\n"));
		return FALSE;
	}

	csCommand = _T("PASS ") + csPassword;
	
	bRetVal = SendCommand( csCommand );
	GetResponse();
	if( ! bRetVal || GetErrorCode( ) != PASS_OK )
	{
		TRACE(_T("\nServer Message - User access not allowed. \r\n"));
		return FALSE;
	}

	TRACE(_T("\nServer Message - User logged in. \r\n"));

	return TRUE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	
BOOL CFtpSockClient::SendCommand( CString csCommand ) 
{
	ASSERT ( m_pCtrlSock != NULL );
	if( m_pCtrlSock == NULL )
	{
		TRACE(_T("\nCode Message - Socket not initialized"));
		return TRUE;
	}

	ASSERT ( m_bSession );
	if( ! m_bSession )
	{
		TRACE(_T("\nCode Message - No Session Established"));
		return TRUE;
	}

	if( csCommand.IsEmpty() )
		return FALSE;
	
	if( csCommand.Right(2) != CRLF )
		csCommand += CRLF;

	UINT nLen = csCommand.GetLength();

	char *pBuffer = new char[ nLen + 1 ];
	WideCharToMultiByte ( CP_ACP, 0, csCommand.GetBuffer( csCommand.GetLength() ), -1, pBuffer, nLen + 1, NULL, NULL );
	UINT nSent = 0;
	do
	{
		nSent += m_pCtrlSock ->Send( pBuffer, nLen );
		if( nSent == 0 )
		{
			if( pBuffer != NULL )
			{
				delete [] pBuffer;
				pBuffer = NULL;
			}
			return FALSE;
		}
	}while ( nSent < nLen );
	
	csCommand.ReleaseBuffer();

	if( pBuffer != NULL )
	{
		delete [] pBuffer;
		pBuffer = NULL;
	}

	return TRUE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	
CString CFtpSockClient::GetResponse( ) 
{
	ASSERT ( m_pCtrlSock != NULL );
	if( m_pCtrlSock == NULL )
	{
		TRACE(_T("\nCode Message - Socket not initialized"));
		return _T("");
	}

	ASSERT ( m_bSession );
	if( ! m_bSession )
	{
		TRACE(_T("\nCode Message - No Session Established"));
		return _T("");
	}
	
	CString csResp = _T("");

	CString csCompleteString = _T("");
	
	char ch;
	
	BOOL bCheckMultiLine = TRUE;
	
	while(bCheckMultiLine)
	{
		while( ( csResp.Right(2) != CRLF ) )
		{
			int nRead = m_pCtrlSock -> Receive( &ch, 1, 0 );
			csResp += ch;
		}

		csCompleteString += csResp;

		if( csResp[POS_HYPHEN] == ' ' )
			bCheckMultiLine = FALSE;
		csResp = _T("");
	}		

	CString csTemp = csCompleteString.Left( RES_CODE_SEGMENT );

	int nLen = csTemp.GetLength();


	char pBuffer[ 4 ];

	WideCharToMultiByte ( CP_ACP, 0, csTemp.GetBuffer( csTemp.GetLength() ), -1, pBuffer, nLen + 1, NULL, NULL );
	SetErrorCode( atoi( pBuffer ) );
	csTemp.ReleaseBuffer();

	return m_csStatusString = csCompleteString;
}
/*====================================================================================================*/	
/*====================================================================================================*/	

void CFtpSockClient::TraceErrorCode(int nErrorCode) 
{
	if (0 != nErrorCode)   
	{      
		switch( nErrorCode )      
		{
         case WSAEADDRINUSE: 
            TRACE(_T("\nCode Message - The specified address is already in use. \r\n") );
            break;         
		 case WSAEADDRNOTAVAIL: 
            TRACE(_T("\nCode Message - The specified address is not available from the local machine. \r\n") );            
			break;
         case WSAEAFNOSUPPORT: 
            TRACE(_T("\nCode Message - Addresses in the specified family cannot be used with this socket. \r\n") );            
			break;
         case WSAECONNREFUSED: 
            TRACE(_T("\nCode Message - The attempt to connect was forcefully rejected. \r\n") );            
			break;         
		 case WSAEDESTADDRREQ: 
            TRACE(_T("\nCode Message - A destination address is required. \r\n") );
            break;         
		 case WSAEFAULT: 
            TRACE(_T("\nCode Message - The lpSockAddrLen argument is incorrect. \r\n") );
            break;         
		 case WSAEINVAL: 
            TRACE(_T("\nCode Message - The socket is already bound to an address. \r\n") );            
			break;         
		 case WSAEISCONN: 
            TRACE(_T("\nCode Message - The socket is already connected. \r\n") );
            break;         
		 case WSAEMFILE: 
            TRACE(_T("\nCode Message - No more file descriptors are available. \r\n") );
            break;         
		 case WSAENETUNREACH: 
            TRACE(_T("\nCode Message - The network cannot be reached from this host at this time. \r\n") );            
			break;         
		 case WSAENOBUFS: 
            TRACE(_T("\nCode Message - No buffer space is available. The socket cannot be connected. \r\n") );            
			break;
         case WSAENOTCONN: 
            TRACE(_T("\nCode Message - The socket is not connected. \r\n") );            
			break;
         case WSAENOTSOCK: 
            TRACE(_T("\nCode Message - The descriptor is a file, not a socket. \r\n") );
            break;         
		 case WSAETIMEDOUT: 
            TRACE(_T("\nCode Message - The attempt to connect timed out without establishing a connection. \r\n") );            
			break;
		 case WSAECONNABORTED: 
            TRACE(_T("\nCode Message - The virtual circuit was aborted due to timeout or other failure. \r\n") );            
			break;
         default:            
			 TCHAR szError[256];
            wsprintf(szError, L"Code Message - Connect error: %d", nErrorCode);
            TRACE(szError);            
			break;      
		}
	}
	else
		TRACE(_T("\nCode Message - Connect attempt Successful") );
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::AssertFailure()
{
	return FALSE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

CString	CFtpSockClient::GetWorkingDirectory( )
{
	CString csCommand = _T("PWD");
	BOOL bRetVal = SendCommand( csCommand );
	CString csDir = GetResponse();
	if( bRetVal && GetErrorCode() == DIR_OK )
	{
		int nFound = csDir.Find(L"\"");
		if( nFound != -1 )
		{
			csDir = csDir.Right( csDir.GetLength() - nFound -1  );
			nFound = csDir.Find( L"\"" );
			csDir = csDir.Left( nFound );
			return csDir;
		}
		return BLANK;
	}
	return BLANK;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::SetWorkingDirectory( CString csDirectory )
{
	ASSERT( csDirectory != BLANK );
	if( csDirectory == BLANK )
		return FALSE;
	
	CString csCommand = _T("CWD ") + csDirectory;

	BOOL bRetVal = SendCommand( csCommand );
	
	GetResponse();
	
	if( bRetVal && GetErrorCode() == CHDIR_OK )
		return TRUE;

	return FALSE;
}
/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::ChangeWorkingDirectory( CString csDirectory )
{
	return SetWorkingDirectory( csDirectory );
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::MakeDirectory( CString csDirectory )
{
	if( csDirectory == BLANK )
		csDirectory = "New Folder";
	CString csNewDir = GetWorkingDirectory();

	if( csNewDir.Right(1) != DIR_SEPERATOR )
		csNewDir += DIR_SEPERATOR;

	CString csCommand = _T("MKD ");
	csCommand += csDirectory;

	BOOL bRetVal = SendCommand( csCommand );
	
	GetResponse();

	if( bRetVal && GetErrorCode() == DIR_OK )
		return TRUE;

	return FALSE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

BOOL CFtpSockClient::RemoveDirectory( CString csDirectory )
{
	ASSERT( csDirectory != BLANK );
	
	if( csDirectory == BLANK )
		return FALSE;
	
	CString csCommand = _T("RMD ");
	csCommand += csDirectory;
	
	BOOL bRetVal = SendCommand( csCommand );
	
	GetResponse();

	if( bRetVal && GetErrorCode() == DELDIR_OK )
		return TRUE;

	return FALSE;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

void CFtpSockClient::SetErrorCode( int nErrorCode ) 
{
	m_nLastError = nErrorCode;
}

/*====================================================================================================*/	
/*====================================================================================================*/	

int CFtpSockClient::GetErrorCode() 
{
	return m_nLastError;
}

CString CFtpSockClient::GetHELP()
{
	SendCommand( _T("HELP") );
	m_csHelpString = GetResponse();
	return m_csHelpString;
}


BOOL CFtpSockClient::GetFile( CString csRemoteFile, CString csLocalFile )
{
	if( !OpenDataChannel() )
		return FALSE;

	CString csCommand = _T("RETR ") + csRemoteFile;
	BOOL bRetVal = SendCommand( csCommand );
	GetResponse();
	if( !bRetVal || GetErrorCode() != DATA_OK )
	{
		m_SockSrvr.Close();
		return FALSE;
	}

	CAsyncSocket	DataSocket;
	// accept inbound data connection from server
	if( ! m_SockSrvr.Accept( DataSocket ) )
	{
		SetErrorCode ( ERR_SOCK_ACCEPT );
		DataSocket.Close();
		m_SockSrvr.Close();
		return FALSE; 
	}

	CFile	cfDataFile;

	if( ! cfDataFile.Open( csLocalFile, CFile::modeCreate | CFile::modeWrite ) )
	{
		SetErrorCode( ERR_SOCK_LOCFILE );
		return FALSE;
	}

	char szBuffer[ MAX_BUFFER ];
	int nNumBytesRead;
	CString csLocFileBuffer;
	while ( TRUE )
	{
		memset( szBuffer, '\0', MAX_BUFFER );

		nNumBytesRead = DataSocket.Receive( szBuffer, MAX_BUFFER - 1 );
					
		if( ! nNumBytesRead || nNumBytesRead == SOCKET_ERROR ) 
			break; // ( EOF || network error )

		csLocFileBuffer += szBuffer;
	}

	cfDataFile.Write( csLocFileBuffer, csLocFileBuffer.GetLength() );

	DataSocket.Close();
	
	m_SockSrvr.Close();

	cfDataFile.Close();

	GetResponse();

	if( GetErrorCode() != GET_OK )
		return FALSE;

	return TRUE;
}

BOOL CFtpSockClient::PutFile( CString csLocalFile, CString csRemoteFile )
{

	if( !OpenDataChannel() )
		return FALSE;

	CString csCommand = _T("STOR ") + csRemoteFile;

	BOOL bRetVal = SendCommand( csCommand );
	GetResponse();
	if( !bRetVal || GetErrorCode() != DATA_OK )
	{
		m_SockSrvr.Close();
		return FALSE;
	}

	CAsyncSocket	DataSocket;
	// accept inbound data connection from server
	if( ! m_SockSrvr.Accept( DataSocket ) )
	{
		SetErrorCode ( ERR_SOCK_ACCEPT );
		DataSocket.Close();
		m_SockSrvr.Close();
		return FALSE; 
	}

	CFile	cfDataFile;

	if( ! cfDataFile.Open( csLocalFile, CFile::modeRead | CFile::shareDenyNone ) )
	{
		SetErrorCode( ERR_SOCK_LOCFILE );
		return FALSE;
	}

	char szBuffer[ MAX_BUFFER ];
	int nNumBytesRead;
	int nNumBytesSent;
	CString csLocFileBuffer;
	while ( TRUE )
	{
		memset( szBuffer,'\0', MAX_BUFFER );

		nNumBytesRead = cfDataFile.Read( szBuffer, MAX_BUFFER );
		
		if(! nNumBytesRead ) //EOF
			break; 
		
		if( ( nNumBytesSent = DataSocket.Send( szBuffer, nNumBytesRead, 0 )) == SOCKET_ERROR ) 
			break;
		
		// if we sent fewer bytes than we read from file, rewind file pointer
		if( nNumBytesRead != nNumBytesSent ) 
			cfDataFile.Seek( nNumBytesSent - nNumBytesRead, CFile::current );
	}

	DataSocket.Close();
	
	m_SockSrvr.Close();

	cfDataFile.Close();

	GetResponse();

	if( GetErrorCode() != GET_OK )
		return FALSE;

	return TRUE;
}

BOOL CFtpSockClient::MoveFile( CString csSourcePath, CString csDestPath )
{
	ASSERT( csSourcePath != BLANK );
	ASSERT( csDestPath != BLANK );	
	if( csSourcePath == BLANK || csDestPath == BLANK )
		return FALSE;
	
	CString csCommand = _T("RNFR ") + csSourcePath;
	BOOL bRetVal = SendCommand( csCommand );
	GetResponse();
	if( bRetVal && GetErrorCode() == MOVE_OK )
	{
		csCommand = _T("RNTO ") + csDestPath;
		bRetVal = SendCommand( csCommand );
		GetResponse();
//		if( bRetVal && GetErrorCode() == MOVE_DONE )
//		{
//		}
		return TRUE;
	}
	
	return FALSE;
}





BOOL CFtpSockClient::OpenDataChannel()
{
	CString csLocHost;
	unsigned int nLocPort;

	// get the local IP address off the control channel socket
	if( ! m_pCtrlSock -> GetSockName( csLocHost, nLocPort ) ) 
	{
		
		return FALSE;
	}
	
	int nFound;
	// convert returned '.' in ip address to ','
	while( 1 ) 
	{
		if( ( nFound = csLocHost.Find( L"." ) ) == -1 ) 
			break;
		csLocHost.SetAt( nFound, ',' );
	}

	// create listen socket (let MFC choose the port) & start the socket listening
	if( ! m_SockSrvr.Create ( 0, SOCK_STREAM, NULL )  )
	{
		SetErrorCode( ERR_SOCK_CREATE );
		return FALSE;
	}

	if( ! m_SockSrvr.Listen ( ) ) 
	{
		SetErrorCode( ERR_SOCK_LISTEN );
		m_SockSrvr.Close();
		return FALSE;
	}

	CString csTempAddr;
	// get the port that MFC chose
	if( ! m_SockSrvr.GetSockName( csTempAddr, nLocPort ) ) 
	{
		SetErrorCode( ERR_SOCK_LOOKUP );
		m_SockSrvr.Close();
		return FALSE;
	}

	// set Transfer Type
	BOOL bRetVal = SendCommand( m_csTransferMode );

	GetResponse();


	// convert the port number to 2 bytes + add to the local IP
	csLocHost.Format( csLocHost + _T(",%d,%d"),nLocPort / 256 , nLocPort % 256 );

	// Send Port Command
	bRetVal = SendCommand ( _T("PORT ") + csLocHost );
	GetResponse();
	if( !bRetVal || GetErrorCode() != PORT_OK )
	{
		SetErrorCode( ERR_SOCK_PORT );
		m_SockSrvr.Close();
		return FALSE;
	}

	return TRUE;
}


BOOL CFtpSockClient::SetTransferModeAscii( )
{
	m_csTransferMode = ASCII;
	return TRUE;
}

BOOL CFtpSockClient::SetTransferModeImage()
{
	m_csTransferMode = IMAGE;
	return TRUE;
}


//		The attributes to be set sholu be like 777, 040, - UNIX chmod options actually
BOOL CFtpSockClient::ChangeFileAttrib( CString csFilePath, CString csAttrib )
{
	ASSERT( csFilePath != BLANK );
	ASSERT( csAttrib != BLANK );

	if( csFilePath == BLANK || csAttrib == BLANK )
		return FALSE;
	
	CString csCommand = _T("SITE CHMOD ");
	csCommand += csFilePath;

	if( ! SendCommand( csCommand ) )
		return FALSE;
	
	GetResponse();

	return TRUE;
}

BOOL CFtpSockClient::ChangeUser( CString csUserName, CString csPassword )
{
	ASSERT( csUserName != BLANK );
	ASSERT( csPassword != BLANK );

	if( csUserName == BLANK || csPassword == BLANK )
		return FALSE;

	CString csCommand = _T("USER ") + csUserName;
	
	BOOL bRetVal = SendCommand( csCommand );
	GetResponse();

	if( !bRetVal || GetErrorCode() != USER_OK )
		return FALSE;
	
	csCommand = _T("PASS ") + csPassword;
	
	bRetVal = SendCommand( csCommand );
	GetResponse();

	if( !bRetVal || GetErrorCode() != PASS_OK )
		return FALSE;
	return TRUE;
}

UINT CFtpSockClient::GetFileList( CString csDirectory, CString& csFileList )
{
	if( !OpenDataChannel() )
		return ZERO;

	CString csCommand = _T("LIST -L");

	BOOL bRetVal = SendCommand( csCommand );
	
	GetResponse();
	if( !bRetVal || GetErrorCode() != DATA_OK )
	{
		m_SockSrvr.Close();
		return ZERO;
	}

	CAsyncSocket	DataSocket;
	// accept inbound data connection from server
	if( ! m_SockSrvr.Accept( DataSocket ) )
	{
		SetErrorCode ( ERR_SOCK_ACCEPT );
		DataSocket.Close();
		m_SockSrvr.Close();
		return ZERO;
	}


	char szBuffer[ MAX_BUFFER ];
	int nNumBytesRead;

	while ( TRUE )
	{
		memset( szBuffer, '\0', MAX_BUFFER );

		nNumBytesRead = DataSocket.Receive( szBuffer, MAX_BUFFER - 1 );
					
		if( ! nNumBytesRead || nNumBytesRead == SOCKET_ERROR ) 
			break; // ( EOF || network error )

		m_FileList += szBuffer;
	}

	csFileList = m_FileList;
	DataSocket.Close();
	
	m_SockSrvr.Close();

	GetResponse();

	if( GetErrorCode() != GET_OK )
		return ZERO;

	return 10;
}

CString CFtpSockClient::GetStatusFromServer()
{
	return m_csStatusString;
}
