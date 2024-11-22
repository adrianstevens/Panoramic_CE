#ifndef __ftp__
#define __ftp__

#include <io.h>

#define DEFAULT_PORT_NUM 21

#define PASSWORD_LENGTH 256

#define EINTR WSAEINTR 

#define bzero(x,y) memset(x,0,y)

#define bcopy(x,y,z) memcpy(y,x,z)

#define close closesocket

enum {
 LS = 0,
 BINARY,
 ASCII,
 PWD,
 CD,
 OPEN,
 CLOSE,
 QUIT,
 LCD,
 LLS,
 LDIR,
 USER,
 SHELL,
 IGNORE_COMMAND,
 GET,
 PUT,
 HELP,
 RHELP,

 FTP_COMPLETE=1, /* these will be used later */
 FTP_CONTINUE,
 FTP_ERROR
};


class ftp {

public:
	
	ftp();
	~ftp();

	int  DoOpen(char *);
	void DoList(char *);
	int  DoCD(char *);
	void DoShellCommand(char *);
	int  DoLogin(char *);
	void DoClose(void);
	void DoLCD( char *);
	int  DoPut( char *);
	int  DoGet( char *,int size=0);
	void DoLLS(char * );
	void DoBinary();
	void DoRhelp( char *);
	void DoAscii();
	void DoPWD();
	void DoDelete(char *);
	void DoMove(char *);
	void DoMoveTo(  char *);
	int  CheckFds(char *);
	int  DoMakeDir(  char *command);
	int DoCDUP();
private:
	char szBuffer[1025];  /* buffer used to read/write */
	char szUser[20];          /* stores username */
	char szPass[256];         /* stores user password */
	int  Connected;     /* flag for connect status */

	
	int hListenSocket;
	int hControlSocket;
	int hDataSocket;	
	int bSendPort;
	int ReadCommand;
	int bMode;

	int GetReply();
    int GetLine();
	void CleanUp();
	int SendControlMsg(char *, int);
	int SendDataMsg( char *szBuffer, int len);
	int ConnectToServer(char *name, char *port);
	int GetListenSocket();
	int InitWinsock();
	int AcceptConnection();
	void CloseControlConnection( void );
	void CloseDataConnection( int hDataSocket );
	void CloseListenSocket();
	int ReadDataMsg( char *szBuffer, int len);
	void GetPassword( char *szPass );
	int GetUnixInput(char *command);
	int GetWin32Input( char *command);
	int GetFile( char *fname);
	int PutFile( char *fname);
	
	int ReadControlMsg( char *szBuffer, int len);
	int CheckControlMsg( char *szPtr, int len);
	int CheckInput();

};

#endif