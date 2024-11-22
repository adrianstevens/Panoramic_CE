// FtpSockClient.h: interface for the CFtpSockClient class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_FTPSOCKCLIENT_H__FD24295F_E047_11D3_8FA6_008048DBAA6B__INCLUDED_)
#define AFX_FTPSOCKCLIENT_H__FD24295F_E047_11D3_8FA6_008048DBAA6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "afxsock.h"

class CFtpSockClient  
{

public:
	//	Construction
	CFtpSockClient();

	virtual ~CFtpSockClient();

	//	Operations
/*******************************************************************
	Function	-	ConnectToServer

	Purpose		-	Used to connect to the remote host. Establishes
					a Socket connection to the remote machine, Event
					is Blocking.

	Params		-	1) server name, 
					2) port. 

	Returns		-	Result of the connection request, TRUE if 
					successful.
******************************************************************/
	BOOL	ConnectToServer( CString csServer, UINT unPort );


/*******************************************************************
	Function	-	UserLogin

	Purpose		-	used for User Login

	Params		-	1) Username
					2) password;

	Returns		-	result of login attempt, TRUE if successful
******************************************************************/
	BOOL	UserLogin(CString csUserName, CString csPassword ) ;

/*******************************************************************
	Function	-	DisconnectFromServer

	Purpose		-	used for Disconnecting the Server session. 
					Closes all open sockets, cancels pending requests.

	Params		-	void

	Returns		-	result of disconnect attempt, TRUE if successful
******************************************************************/
	BOOL	DisconnectFromServer();

/*******************************************************************
	Function	-	GetStatusFromServer

	Purpose		-	Function returns the status string from the server

	Params		-	void

	Returns		-	the status string as CString object
******************************************************************/
	CString GetStatusFromServer();


public:
	CString GetHELP();
	//	Attributes
/*====================================================================================================*/	
/*========================================Directory Routines==========================================*/
/*====================================================================================================*/	

/*******************************************************************
	Function	-	GetWorkingDirectory

	Purpose		-	Gets the current working directory on Remote host 

	Params		-	void

	Returns		-	result of chdir attempt, TRUE if successful
******************************************************************/
	CString	GetWorkingDirectory( );


/*******************************************************************
	Function	-	SetWorkingDirectory

	Purpose		-	Set The current working directory on Remote host 

	Params		-	1) The complete path of new directory

	Returns		-	result of chdir attempt, TRUE if successful
******************************************************************/
	BOOL	SetWorkingDirectory( CString csDirectory ) ;

/*******************************************************************
	Function	-	ChangeWorkingDirectory

	Purpose		-	Change the working directory on Remote host 

	Params		-	1) The complete path of new directory

	Returns		-	result of chdir attempt, TRUE if successful
******************************************************************/
	BOOL	ChangeWorkingDirectory( CString csDirectory ) ;

/*******************************************************************
	Function	-	MakeDirectory

	Purpose		-	Create directory on Remote host 

	Params		-	1) The complete path of new directory

	Returns		-	result of chdir attempt, TRUE if successful
******************************************************************/
	BOOL	MakeDirectory( CString csDirectory ) ;


/*******************************************************************
	Function	-	RemoveDirectory

	Purpose		-	Remove directory on Remote host. This function 
					will work if the dir to be deleted is empty.

	Params		-	1) The complete path of dir to remove

	Returns		-	result of delete attempt, TRUE if successful
******************************************************************/
	BOOL	RemoveDirectory( CString csDirectory );

	
/*====================================================================================================*/	
/*=============================================File Routines==========================================*/
/*====================================================================================================*/	
	
/*******************************************************************
	Function	-	GetFile

	Purpose		-	downloads the requested file from the server.

	Params		-	1) The complete path remote file,
					2) The complete path of local File

	Returns		-	result of download attempt, TRUE if successful
******************************************************************/
	BOOL	GetFile( CString csRemoteFile, CString csLocalFile ) ;


/*******************************************************************
	Function	-	PutFile

	Purpose		-	downloads the requested file from the server.

	Params		-	1) The complete path remote file,
					2) The complete path of local File
					Note :	If the Remote file name is not specified 
							then the file is uploaded witht the local 
							name.

	Returns		-	result of upload attempt, TRUE if successful
******************************************************************/
	BOOL	PutFile( CString csLocalFile, CString csRemoteFile ) ;

/*******************************************************************
	Function	-	MoveFile

	Purpose		-	downloads the requested file from the server.

	Params		-	1) The complete path source file,
					2) The complete path of destination File

	Returns		-	result of move attempt, TRUE if successful
******************************************************************/
	BOOL	MoveFile( CString csSourcePath, CString csDestPath ) ;

/*====================================================================================================*/	
/*=============================================Special Routines=======================================*/
/*====================================================================================================*/	

/*******************************************************************
	Function	-	ChangeFileAttrib

	Purpose		-	Change the attributes of the file.This command 
					may not work at all the servers. Also it is 
					limited to user who have the specific rights.

	Params		-	1) The complete path of file whose attribs are 
						to be changed,
					2) New Attributes

  	Returns		-	result of upload attempt, TRUE if successful
******************************************************************/
	BOOL	ChangeFileAttrib( CString csFilePath, CString csAttrib ) ;

/*******************************************************************
	Function	-	ChangeUser

	Purpose		-	Change the user. This is same as logging OFF and
					then logging on with a different username.

	Params		-	1) Username
					2) password
	Returns		-	result of upload attempt, TRUE if successful
******************************************************************/
	BOOL	ChangeUser( CString csUserName, CString csPassword ) ;

/*******************************************************************
	Function	-	SetTransferModeAscii

	Purpose		-	Set The Data Transfer mode to ascii

	Params		-	void
	
	Returns		-	result of attempt, TRUE if successful
******************************************************************/
	BOOL	SetTransferModeAscii( ) ;

/*******************************************************************
	Function	-	SetTransferModeImage

	Purpose		-	Set The Data Transfer mode image

	Params		-	void
	
	Returns		-	result of attempt, TRUE if successful
******************************************************************/
	BOOL	SetTransferModeImage( ) ;


/*******************************************************************
	Function	-	GetFileList

	Purpose		-	Set The Data Transfer mode image

	Params		-	The Directory whose files are to be listed
	
	Returns		-	The number of files in that dir
******************************************************************/
	UINT	GetFileList( CString csDirectory, CString &csFileList ) ;

private:
	//	Operations
/*******************************************************************
	Function	-	CreateChannel

	Purpose		-	Creates the control socket object, initialises
					socket and creates the socket handle.

	Params		-	void

	Returns		-	result of create attempt, TRUE if successful
******************************************************************/
	BOOL	CreateChannel(CString csServer, UINT unPort );

	
/*******************************************************************
	Function	-	CloseChannel

	Purpose		-	Does the cleanup for socket object

	Params		-	void

	Returns		-	void
******************************************************************/
	void	CloseChannel();


/*******************************************************************
	Function	-	SendCommand

	Purpose		-	Send an FTP Command to the server

	Params		-	1) Command string

	Returns		-	TRUE if the command was sent successfully.
******************************************************************/
	BOOL	SendCommand( CString csCommand ) ;

/*******************************************************************
	Function	-	GetResponse()

	Purpose		-	Get the response string from the server

	Params		-	void

	Returns		-	The response string from the server. 
******************************************************************/
	CString		GetResponse( ) ;


/*******************************************************************
	Function	-	SetErrorCode

	Purpose		-	Sets the error code occuring from a function.

	Params		-	1) Error code from GetLastError()

	Returns		-	void
******************************************************************/
	void	SetErrorCode( int nErrorCode ) ;

/*******************************************************************
	Function	-	SetErrorCode

	Purpose		-	Sets the error code occuring from a function.

	Params		-	void

	Returns		-	the error code set buy SetErrorCode()
******************************************************************/
	int		GetErrorCode( ) ;

/*******************************************************************
	Function	-	TraceErrorCode

	Purpose		-	traces the error  code for the connect routine
					in the output window

	Params		-	1) Error code from GetLastError()

	Returns		-	void
******************************************************************/
	void	TraceErrorCode( int nErrorCode) ;

/*******************************************************************
	Function	-	AssertFailure

	Purpose		-	1) Asserts that the socket hasn't been initialized 
						before use
					2) Asserts that there is no active session 

	Params		-	void

	Returns		-	True if Assertion is Valid
******************************************************************/
	BOOL AssertFailure();

private:

	CString m_csStatusString;

	BOOL OpenDataChannel();

//	members

	CString m_FileList;


	CString m_csTransferMode;

//	This Socket is our control Socket used for the main Communication 
//	with the server. 
//	This is a blocking socket. Created using CreateChannel() and 
//	Closed by CloseChannel()
	CSocket		*m_pCtrlSock;

//	This string is used for storing the current host, the socket is 
//	connected to.
	CString		m_csHost;

//	Boolean variable which identifies whether a session is currently 
//	established.
	BOOL		m_bSession;

//	integer variable which identifies stores the last error code
	int			m_nLastError;

// String has Help 
	CString		m_csHelpString;

	CSocket		m_SockSrvr;

};

#endif // !defined(AFX_FTPSOCKCLIENT_H__FD24295F_E047_11D3_8FA6_008048DBAA6B__INCLUDED_)
