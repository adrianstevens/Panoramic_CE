#include "stdafx.h"
#include "ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock.h>
#include <direct.h>
#include <conio.h> 



ftp::ftp() {
	Connected = 0;
	hListenSocket = INVALID_SOCKET;
	hControlSocket = INVALID_SOCKET;
	hDataSocket = INVALID_SOCKET;
	bSendPort      = 1;
	ReadCommand = 0;
	bMode=ASCII;

	InitWinsock();
	
};

ftp::~ftp() {
	
};


/*
 * DoOpen
 * this function is called when the o,open command is issued.
 * it connects to the requested host, and logs the user in
 *
 */
int ftp::DoOpen( char *command)
{
  char *szHost=NULL;  /* remote host */
  int responseCode=0;
   /* 
    * do not do anything if we are already connected.
    */
   if( Connected ) {
       printf("Already connected.  Close connection first.\n");
       fflush(stdout);
       return 220;
   }
   
   /*
    * extract the remote host from command line (if given ).
    * make a copy of the host name with strdup.
    */
   if(!stricmp( command, "open") )
	   printf("hello\n");
   if(!strcmp(command,"open") || !strcmp(command,"o")) {

       printf("Host:"); fgets(szBuffer,1024,stdin);
       (void)strtok(szBuffer,"\n");
       szHost = (char *)strdup(szBuffer);;
   }
   else if( !strncmp(command,"open ",5))
	szHost = strdup(&command[5]);
   else if( !strncmp(command,"o ",2) )
	szHost = strdup(&command[2]);
   else
	szHost = strdup(command);


   printf("Connecting to %s\n",szHost);
   hControlSocket = ConnectToServer(szHost,"21");

#if (defined(WIN32) || defined(_WIN32) )
   Sleep(1);
#else
   sleep(1);
#endif
   responseCode=hControlSocket;
   if( hControlSocket > 0)  {
     printf("Connected to %s\n",szHost);
     Connected = 1;         /* we ar now connected */
     responseCode=GetReply();            /* get reply (welcome message) from server */
     
   }
   free(szHost); /* free the strdupped string */
   return responseCode;
}



/*
 * DoLogin
 * this function logs the user into the remote host.
 * prompts for username and password
 * or parses username and password from command (user <username>:<password>)
 */
int ftp::DoLogin( char *command)
{
 char *User=NULL;
 char *Password=NULL;
 int responseCode=0;
  if( command && *command)

     User=strdup(&command[5]);

   if( Connected )  {

     /* 
      * ignore leading whitespace
      */
     while(User && (*User == ' ' || *User == '\t') && *User)
	 User++;
	 Password = strchr(User, ':');
	 if (Password) {
		printf("password given\n");
		*Password = '\0';
		Password++;
	 };
     /*
      * if user name was not provided via command line, read it in.
      */
     if(!User || !(*User) ) {
	printf("Login:"); fgets(szUser,20,stdin);
	User = szUser;
	(void)strtok(szUser,"\n");   /* remove '\n' */
     }

     /*
      * send user name & password to server  & get reply message
      */
     sprintf(szBuffer,"USER %s\r\n",User);
     SendControlMsg(szBuffer,(int)strlen(szBuffer));
     responseCode=GetReply();
	 if (!Password) {
		GetPassword( szPass );
		Password = szPass;
	 };

     sprintf(szBuffer,"PASS %s\r\n",Password);
	 if(331==responseCode)
	 {
		 SendControlMsg(szBuffer,(int)strlen(szBuffer));
		 responseCode=GetReply();
	 }
   }
   else
   {
      printf("Not Connected.\n");
	  return responseCode;
	}

   free(User); /* free the strdupped string */
   return responseCode;
}

/*
 * DoClose
 * closes connection to the ftp server
 */
void ftp::DoClose( void )
{
   if( !Connected  ) {
     printf("Not Connected.\n");
    
   }
   else {
	   SendControlMsg("quit\r\n",6);
	   GetReply();
	   CloseControlConnection();
	   hControlSocket = -1;
#if (defined(WIN32) || defined(_WIN32) )
   
	   //SetConsoleTitle("dftp: Connection closed");
#endif

	   Connected = 0;
   }

}



void ftp::DoDelete(  char *command)
{ 

	if( !Connected ) {
      printf("Not Connected.\n");
      return;
   }

   /*
    * obtain a listening socket
    */
   if( GetListenSocket() < 0) {
       printf("Cannot obtain a listen socket.\n");
       return;
   }

   sprintf(szBuffer,"DELE %s\r\n", command);
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   (void)GetReply();
   
}


void ftp::DoMove(  char *command)
{ 
	char *files= (char *)calloc((int)strlen(command)+1,1);
	int len=(int)strlen(command);
	strncpy(files,(const char *)command,len);
	char *origFileName = strtok( files," " );
	char *newFileName = strtok( NULL,"\n" );
	if( !Connected ) {
      printf("Not Connected.\n");
      return;
   }

   /*
    * obtain a listening socket
    */
   if( GetListenSocket() < 0) {
       printf("Cannot obtain a listen socket.\n");
       return;
   }


	sprintf(szBuffer,"RNFR %s\r\n", origFileName);
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   if(350 ==GetReply())
   {
	   sprintf(szBuffer,"RNTO %s\r\n",newFileName);
		DoMoveTo(szBuffer);
		(void)GetReply();
   }
   
}


void ftp::DoMoveTo(  char *command)
{ 
	char szBuffer[64]={0};
	if( !Connected ) {
      printf("Not Connected.\n");
      return;
   }

   sprintf(szBuffer,"%s\r\n", command);
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   (void)GetReply();
	   
	
   
}

/*
 * DoList
 * perform directory listing i.e: ls
 */
void ftp::DoList( char *command)
{
   char newbuffer[1024];
   if( !Connected ) {
      printf("Not Connected.\n");
      return;
   }
   FILE *fptr;
   if( (fptr = fopen( "data.txt", "w" )) == NULL )
      printf( "The file 'data2' was not opened\n" );

   /*
    * obtain a listening socket
    */
   if( GetListenSocket() < 0) {
       printf("Cannot obtain a listen socket.\n");
       return;
   }
   
   /*
    * parse command
    */
   if( !strcmp(command,"ls") )  {
       sprintf(szBuffer,"NLST\r\n");
   }
   else if( !strcmp(command,"dir") ) 
       sprintf(szBuffer,"LIST\r\n");

   else if( !strncmp(command, "ls ",3)) {
       while( *command == ' ') command++;
       sprintf(szBuffer,"LIST %s\r\n",&command[3]);
   }
   /*
    * send command to server and get response
    */
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   memset(szBuffer,0,1024);
   GetReply();

   /*
    * accept server's connection
    */
   if(AcceptConnection() < 0) {
      printf("Cannot accept connection.\n");
      return;
   }
   CloseListenSocket();       /* close listening socket */

   /*
    * display directory listing.
    */
   while( ReadDataMsg(szBuffer,1024) > 0) {
       fflush(stdout);
       printf(szBuffer);
	  
	   int j=0;
	   for(int i=0;i<1023;i++)
	   {
		  
		   if(szBuffer[i]==13)
		   {
			newbuffer[j++]= '\n';
			i++;
		   
		   }
		   else
		   {
			   if(szBuffer[i]==32)
			   {
					newbuffer[j++]='|';
					while(szBuffer[i]==32)
						i++;
			   }
		   
				newbuffer[j++]= szBuffer[i];
		   }
	   }
	   newbuffer[j]= 0;
   	   fprintf( fptr, "%s",newbuffer );
       memset(szBuffer,0,1024);
	   memset(newbuffer,0,1024);
   }
   /*
    * read response
    */
   fclose(fptr);
   CloseDataConnection(  hDataSocket );
   (void)GetReply();

}

/*
 * DoCD
 * chang to another directory on the remote system
 */
int ftp::DoCD( char *command)
{
   char *dir=&command[2];
	 int responseCode=0;

   if( !Connected ) {
       printf("Not Connected.\n");
       return responseCode;
   }

   /*
    * ignore leading whitespace
    */
   while( *dir && (*dir == ' ' || *dir == '\t') ) 
       dir++;

   /*
    * if dir is not specified, read it in
    */
   if( ! (*dir) ) {
      printf("Remote directory:");
      fgets(szBuffer,1024,stdin);
      (void)strtok(szBuffer,"\n");
      dir = (char *)strdup(szBuffer);
      while( *dir && (*dir) == ' ') 
       dir++;
      if( !(*dir) ) {
	printf("Usage: cd remote-directory\n");
	return responseCode;
      }
   }
   
   /*
    * send command to server and read response
    */
   sprintf(szBuffer, "CWD %s\r\n",dir);
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   responseCode=GetReply();
   return responseCode;
}

/*
 * DoLCD
 * change directory on the local system
 */
void ftp::DoLCD( char *command)
{

   char *dir = &command[3];

   while(*dir && (*dir == ' ' || *dir == '\t') ) dir++;

   /*
    * if dir is not specified, then print the current dir
    */
   if( ! *dir ) {
      dir = getcwd((char *)NULL,256);
      if( !dir)
	perror("getcwd");
      else
	printf("Current directory is: %s\n",dir);
   }
   else {
      if( chdir(dir) < 0) 
	perror("chdir");
      else {
	dir = getcwd((char *)NULL,256);
	if( !dir)
	    perror("getcwd");
	else
	printf("Current directory is: %s\n",dir);
      }
   }
}

/*
 * DoLLS
 * perform local directory listing.  winqvt implements this, but
 * this is not supported by many systems.  it's not really needed
 * since we already have '!' command.  you can just do !ls or !dir.
 */
void ftp::DoLLS( char *command )
{
#if ( !defined(_WIN32) || !defined(WIN32) )
    system("ls");
#else
    system("dir");
#endif
}

/*
 * function to pass commands to the system, ie: dir.
 * this function gets called when '!' is encountered
 */
void DoShellCommand( char *command )
{
  command++;  /* ignore '!' */
#if ( !defined(_WIN32) || !defined(WIN32) )

    system(command);
#else
	if( !command || !*command)
		system("cmd");   /* have to fix this for win95 */
	else                     /* maybe i should use 'start' */
		system(command); /* programatically determine which */ 
#endif                           /* system we are on, then make the  */
				 /* appropriate call */
}


/*
 * DoBinary
 * set file transfer mode to binary
 */
void ftp::DoBinary()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE I\r\n");
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   GetReply();
   printf("File transfer modes set to binary.\n");
   bMode = BINARY;
}

/*
 * DoAscii
 * set file transfer mode to ascii text
 */
void ftp::DoAscii()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE A\r\n");
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   GetReply();
   printf("File transfer modes set to ascii.\n");
   bMode = ASCII;

}

/*
 * DoGet
 * retrieve a file from the remote host.  calls GetFile(..)
 */
int ftp::DoGet( char *command,int size)
{


  if( !Connected ) {
      printf("Not Connected.\n");
      return 0;
  }
  //(void)strtok(command," ");
  //GetFile(strtok((char *)NULL, " "));
  return (GetFile(command));
}

/*
 * DoPut
 * send a file to the remote host.  calls PutFile(..)
 */
int ftp::DoPut( char *command )
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return 0;
  }
  //(void)strtok(command," ");
  //PutFile(strtok((char *)NULL, " "));
  return (PutFile(command));
}

/*
 * DoRhelp
 * sends a help command to the server.
 */
void ftp::DoRhelp( char *command )
{
  char *szCommand;

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  (void)strtok(command," ");
  szCommand=strtok((char *)NULL, " ");

  if( szCommand && *szCommand )
     sprintf(szBuffer,"HELP %s\r\n",szCommand);
  else 
     sprintf(szBuffer, "HELP\r\n");
  
  SendControlMsg(szBuffer, (int)strlen(szBuffer));
  GetReply();
}

/*
 * retrieves the current directory on the remote host
 */
void ftp::DoPWD()
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  sprintf(szBuffer, "PWD\r\n");
  SendControlMsg(szBuffer,(int)strlen(szBuffer));
  GetReply();
}



int ftp::GetListenSocket()

{

    int sockfd, flag=1,len;

    struct sockaddr_in  serv_addr, TempAddr;

    char *port,*ipaddr;

    char szBuffer[64]={0};



    /*

     * Open a TCP socket (an Internet stream socket).

     */

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

	perror("socket");

	return INVALID_SOCKET;

    }



    /*

     * Fill in structure fields for binding

     */

   if( bSendPort ) {

       bzero((char *) &serv_addr, sizeof(serv_addr));

       serv_addr.sin_family      = AF_INET;

       serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

       serv_addr.sin_port        = htons(0); /* let system choose */

   }

   else {

     /* reuse the control socket then */

      if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,

         (char *)&flag,sizeof(flag)) < 0) {



	 perror("setsockopt");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	 close(sockfd);

#else

         closesocket(sockfd);

#endif

	 return INVALID_SOCKET;



      }

   }

    /*

     * bind the address to the socket

     */

    if (bind(sockfd,(struct sockaddr *)&serv_addr, 

	     sizeof(serv_addr)) < 0) {

	perror("bind");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

	return INVALID_SOCKET;

    }

    

    len = sizeof(serv_addr);

    if(getsockname(sockfd,

		   (struct sockaddr *)&serv_addr,

		   &len)<0) {

       perror("getsockname");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

       return INVALID_SOCKET;

    }

    len = sizeof(TempAddr);

    if(getsockname(hControlSocket,

		   (struct sockaddr *)&TempAddr,

		   &len)<0) {

       perror("getsockname");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

       return INVALID_SOCKET;

    }



    ipaddr = (char *)&TempAddr.sin_addr;

    port  = (char *)&serv_addr.sin_port;



#define  UC(b)  (((int)b)&0xff)



    sprintf(szBuffer,"PORT %d,%d,%d,%d,%d,%d\r\n",

          UC(ipaddr[0]), UC(ipaddr[1]), UC(ipaddr[2]), UC(ipaddr[3]),

          UC(port[0]), UC(port[1]));

    /*

     * allow ftp server to connect

     * allow only one server

     */

    if( listen(sockfd, 1) < 0) {

	perror("listen");

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

	close(sockfd);

#else

        closesocket(sockfd);

#endif

	return INVALID_SOCKET;

    }

    SendControlMsg(szBuffer,(int)strlen(szBuffer));

    GetReply();

    hListenSocket = sockfd;

    return sockfd;

}



int ftp::AcceptConnection()

{



    struct sockaddr_in cli_addr;

    int clilen = sizeof(cli_addr);

    int sockfd;



    sockfd = accept(hListenSocket, (struct sockaddr *) &cli_addr,

			   &clilen);

    if (sockfd < 0) {

        perror("accept");

	return INVALID_SOCKET;

    }

   

    hDataSocket = sockfd;

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

    close(hListenSocket);

#else

    closesocket(hListenSocket);

#endif

    return sockfd;



}





int ftp::ConnectToServer(char *name, char *port)



{

  int s;
  unsigned int portnum;



  struct sockaddr_in server;

  struct hostent *hp;



  while( name && *name == ' ') name++;

  if( !name || ! (*name) )

      return INVALID_SOCKET;



  portnum = atoi(port);

  bzero((char *) &server, sizeof(server));



  if( isdigit(name[0])) {

   server.sin_family      = AF_INET;

   server.sin_addr.s_addr = inet_addr(name);

   server.sin_port        = htons(portnum);

  }

  else{ 

   if ( (hp = gethostbyname(name)) == NULL)

    {
#if (!defined( _WIN32 ) || !defined( WIN32 ))

      perror("gethostbyname");
#else 
	  printf("gethostbyname: error code: %d\r\n", WSAGetLastError());

#endif
	  return INVALID_SOCKET;

    }

    bcopy(hp->h_addr,(char *) &server.sin_addr,hp->h_length);

    server.sin_family = hp->h_addrtype;

    server.sin_port = htons(portnum);  

   }/* else */



  /* create socket */

  if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 1) {
#if (!defined( _WIN32 ) || !defined( WIN32 ))
	  perror("socket");
#else
	  printf("socket: error code: %d\r\n", WSAGetLastError());
#endif

    return INVALID_SOCKET;

  }

  

  if (connect(s,(struct sockaddr *)&server, sizeof(server))< 0) {

#if (!defined( _WIN32 ) || !defined( WIN32 ))
	  perror("connect");
#else
	  TRACE("connect: error code: %d\r\n", WSAGetLastError());
#endif    
	  

    return INVALID_SOCKET;

  }

  

  setsockopt(s,SOL_SOCKET,SO_LINGER,0,0);

  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);

  setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);

  hDataSocket = s;

  

  return s;

}



int ftp::ReadControlMsg( char *szBuffer, int len)

{

  int ret;

   

   if( (ret=recv(hControlSocket,szBuffer,len,0)) <= 0)

       return 0;

   

   return ret;

}



int ftp::SendControlMsg( char *szBuffer, int len)

{

   

   if( send(hControlSocket,szBuffer,len,0) <= 0)

       return 0;

   

   return 1;

}



int ftp::ReadDataMsg( char *szBuffer, int len)

{

   int ret;



   if( (ret=recv(hDataSocket,szBuffer,len,0)) <= 0)

       return 0;

   

   return ret;

}



int ftp::SendDataMsg( char *szBuffer, int len)

{

   

   if( send(hDataSocket,szBuffer,len,0) <= 0)

       return 0;

   

   return 1;

}



void ftp::CloseDataConnection( int hDataSocket )

{

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

      close(hDataSocket);

#else

      closesocket(hDataSocket);

#endif



      hDataSocket = INVALID_SOCKET;

}



void ftp::CloseControlConnection( void )

{

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

      close(hControlSocket);

#else

      closesocket(hControlSocket);

#endif



      hControlSocket = INVALID_SOCKET;

}


void ftp::CloseListenSocket( void )
{
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
      close(hListenSocket);
#else
      closesocket(hListenSocket);
#endif

      hListenSocket = INVALID_SOCKET;
}


int ftp::CheckControlMsg( char *szPtr, int len)

{

    

    return recv(hControlSocket,szPtr,len,MSG_PEEK);

}







int ftp::GetLine()

{

   int done=0, iRetCode =0, iLen, iBuffLen=0;

   char *szPtr = szBuffer, nCode[3]={0},ch=0;



   while( (iBuffLen < 1024) && 

	  (CheckControlMsg(&ch,1)  > 0) ){

        iLen = ReadControlMsg(&ch,1);

	iBuffLen += iLen;

	*szPtr = ch;

	szPtr += iLen;

	if( ch == '\n' )

	    break;    // we have a line: return

   }

   *(szPtr+1) = (char)0;



   strncpy(nCode, szBuffer, 3);



   return (atoi(nCode));



}



int ftp::GetReply()

{

   int done = 0, iRetCode = 0;

     

   memset(szBuffer,0,1024);

   while(!done ) {

        iRetCode = GetLine();
	(void)strtok(szBuffer,"\r\n");

	puts(szBuffer);

	if( szBuffer[3] != '-' && iRetCode > 0 )

	    done = 1;

	memset(szBuffer,0,1024);

   }

   return iRetCode;

}



int ftp::CheckInput()
{


#if (!defined(WIN32) || !defined(_WIN32) )
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_CLR (fileno(stdin),&readfds);

  timeout.tv_sec = 0 ;                /* 0-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */
  FD_SET(fileno(stdin),&readfds);

  i=select ( fileno(stdin)+1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */

  if (errno == EINTR && i ==-1)  {
 	  return 0;

  }
  return ( FD_ISSET(fileno(stdin),&readfds) );
#else
	   return (kbhit() );
#endif

}

int  ftp::CheckFds( char *command)
{
 int rval, i;
 fd_set readfds, writefds, exceptfds;
 struct timeval timeout;
 char *szInput=command;


/*  memset(command,0,1024); */
  memset(szBuffer,0,1024);
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
#if (!defined(WIN32) || !defined(_WIN32) )
  FD_CLR (fileno(stdin),&readfds);
#endif
  if( hControlSocket > 0) 
      FD_CLR (hControlSocket,&readfds);
  timeout.tv_sec = 0 ;                /* 1-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */

#if (!defined( _WIN32 ) || !defined( WIN32 ) )
  FD_SET(fileno(stdin),&readfds);
#endif

  if( hControlSocket > 0) 
      FD_SET(hControlSocket,&readfds);

  i=select ((hControlSocket > 0) ? (hControlSocket+1) : 1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */
if (errno == EINTR && i ==-1)  {
   /*memset(command,0,1024);*/
   return 0;
}


if( (hControlSocket > 0) && FD_ISSET(hControlSocket, &readfds) ) 
{
  if ( ( rval = ReadControlMsg(szBuffer,1024))  > 0)
	  {
	  printf(szBuffer);
	  printf("ftp>");
	  fflush(stdout);
	  return 0;
	  }
  else {
	 printf("\r\nConnection closed by server\r\n");
#if (defined(WIN32) || defined(_WIN32) )

	 //SetConsoleTitle("dftp: Connection closed");
#endif
	 
	 CloseControlConnection();
	 hControlSocket = -1;
	 return 0;
  }

 }

#if (!defined( _WIN32 ) || !defined( WIN32 ) )

   if( FD_ISSET(fileno(stdin),&readfds) )  
       return (ReadCommand = GetUnixInput(command));
   return (ReadCommand = 0);
#else
    
   return (ReadCommand = GetWin32Input(command));

#endif

}


int ftp::InitWinsock()
{

	WSADATA WsaData;

	if( !WSAStartup(0x0101,&WsaData) ) {
		//SetConsoleTitle("dftp: Connection closed");
		return 1;
	}
	else {
	 printf("Winsock cannot be started\r\n");
	 return 0;
	}
	
}

void ftp::CleanUp()
{

	WSACleanup();
}



/*
 * GetPassword
 * read in the user's password.  turn off echoing before reading.
 * then turn echoing back on.
 */
void ftp::GetPassword( char *szPass )
{
     
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
     EchoOff();
     (void)printf("Password:");
     (void)fgets(szPass,PASSWORD_LENGTH,stdin);
     (void)strtok(szPass,"\n");
     EchoOn(stdin,1);
#else
     /*
      * for WIN32 we just use getch() instead of getche()
      */
     int i=0;
     char ch=0;
	 
	 (void)printf("Password:");

     while( (i < PASSWORD_LENGTH) && ( ch != '\r') ) {
	  
	  ch = getch();
	  if( ch != '\r' && ch != '\n')
	     szPass[i++] = ch;
     }
#endif
     (void)printf("\r\n");
}

/*
 * GetUnixInput
 * function called to get user input on the UNIX side.  
 */

int ftp::GetUnixInput(char *command)
{
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
	char ch;
	int i;
	
	/*
	 * ignore leading whitespace
	 */
	while( (ch=getchar()) == ' ' || ch == '\t') ;
	
	if( ch != '\n') {
	    command[0] = ch;
	    fgets(&command[1],1024,stdin);
	    strtok(command,"\n");
	    i = (int)strlen(command) - 1;

	    while( i>0 && isspace(command[i]))
	       i--;
	    if( i>= 0)
	       command[i+1] = 0;

	}
	return 1;
#else
       return 0;
#endif
}

/*
 * GetWin32Input
 * called to get input on the WIN32 side.
 */
int ftp::GetWin32Input( char *command)
{
#if (defined( _WIN32 ) || defined( WIN32 )) 
   char ch;
   static int i=0;   
   
   /*
    * i, above, is static because this function can be called
    * many times before '\r' is pressed.  we set i=0 when
    * '\r' is encountered.
    */


   while( kbhit() ) {
      if( (ch=getch()) == 0)
	  getch();          /* ignore */
      else {
	 if( ch == '\r') {
	     command[i] = 0;
	     i = 0;
	     printf("\r\n");
	     i = (int)strlen(command) - 1;

	     /*
	      * ignore trailing whitespace
	      */
	     while( i>0 && isspace(command[i]))
		i--;
		 if( i>= 0)
			 command[i+1] = 0;
		 i = 0;

	     return 1;
	 }
	 else if( ch == (char)8 && i > 0) {   /* process backspace */
	     printf("%c %c",8,8);
	     command[i] = 0;
	     i--;
	 }
	 else if( ( ch >= 32) && ( ch <= 126) ) {
	     printf("%c", ch);
	     command[i++] = ch;
	 }
      }
   }
   return 0;
#else
   return 0; 
#endif
}


/*
 * GetFile
 * called to retrive a file from remote host
 */
int ftp::GetFile( char *fname)
{
   FILE *fp=NULL;
   int fd, nTotal=0, nBytesRead=0, aborted=0;
   char *abortstr = "ABOR\r\n", ch;
	int responseCode=0;

   /*/void (*OldHandler)(int); */

   /*
    * did we get a filename?
    */
   if( !fname || ! (*fname)) {
      printf("No file specified.\n");
      return 0;
   }

   
   /*
    * obtain a listen socket
    */
   if( GetListenSocket() < 0) {
       fclose(fp);
       return 0;
   }
   
   /*
    * send command to server and read response
    */
   sprintf(szBuffer,"RETR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,(int)strlen(szBuffer))) {
      fclose(fp);
      return 0;
   }
   responseCode=GetReply();

   if(150==responseCode)
   {
	
		/*
		* open the file with current mode
		*/
		if(! (fp=fopen(fname,(bMode==ASCII) ? "wt" : "wb"))) {
		perror("file open");
		return 0;
		}

	   /*
		* accept server connection
		*/
	   if( AcceptConnection() <= 0) {
		   fclose(fp);
		   return 0;
	   }
	   /* 
		* now get file and store
		*/
   

	   fd = fileno(fp);

   
	   /*/ this did not work :((
		*  so i am taking it out.  well, i am commenting it out
		*  if you figure it out, then use it.  the way i did it
		*  now works well.
	   */
	/*/   OldHandler = signal(SIGINT,SigHandler);
		 retval = setjmp(abortfile);

	   if( retval != 0 ) {
		  printf("\r\nAborting\r\n");
		  fflush(stdout);
		  close(fd);
		  CloseDataConnection(hDataSocket);
		  if( OldHandler)
		  (void)signal(SIGINT,OldHandler);
		  ControlCHit = 0;
		  GetReply();
		  return;
	   }
	*/

	#if (defined(_WIN32) || defined(WIN32))
		
	   printf("Press ESC to abort\r\n");
	#else
	   printf("Type q and hit return to abort\r\n");
	#endif
	   while( (nBytesRead=ReadDataMsg(szBuffer,1024)) > 0) {
       
		   
		   write(fd,szBuffer,nBytesRead);
		   nTotal+=nBytesRead;
		   printf("%s : %d received\r",fname,nTotal);
		   if( CheckInput() ) {
	#if (defined(_WIN32) || defined(WIN32))

				if( !(ch = getch()) ) getch();
				if( ch == 27 )
					aborted = 1;
	#else
				ch = getchar();
				if( ch != '\n') {
					while( getchar() != '\n') ;      /* read 'til new line */
				}
				if( ch == 'q') 
					aborted = 1;
	#endif
		   }
		   
		   /*
			* did we abort?
			*/
		   if( aborted ) {
			  
			   printf("\r\nAbort: Waiting for server to finish.");
			   SendControlMsg(abortstr,(int)strlen(abortstr));
			   break;
		   }
	   }

	   if( aborted ) {         // ignore everything if aborted.

		   while( (nBytesRead=ReadDataMsg(szBuffer,1024)) > 0);
		   GetReply();
	   }



	   printf("\r\n");
	   close(fd);
	   fclose(fp);
	   CloseDataConnection(hDataSocket);
	 /*/  ControlCHit = 0; */
	  responseCode= GetReply();
   }
   
   return responseCode;
}

/*
 * PutFile
 * called to transfer a file to the remote host using the current
 * file transfer mode.  it's just like GetFile.
 *
 * i have commented out lines that would have helped with trapping
 * ctrl-c because longjmp did not work :((
 * if you figure it out, then uncomment them
 */
 int ftp::PutFile( char *fname)
{
   FILE *fp=NULL;
   int fd,retcode=0, nTotal=0, nBytesRead=0, aborted=0;
   char *abortstr = "ABOR\r\n", ch;



   if( !fname || ! (*fname)) {
      printf("No file specified.\n");
      return retcode;
   }
   if(! (fp=fopen(fname,(bMode==ASCII) ? "rt" : "rb"))) {
      perror("file open");
      return retcode;
   }

   if( GetListenSocket() < 0) {
       fclose(fp);
       return retcode;
   }
   
   /*
    * send command to server & read reply
    */
   sprintf(szBuffer,"STOR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,(int)strlen(szBuffer))) {
      fclose(fp);
      return retcode;
   }
   retcode= GetReply();
   if(retcode!=150)
	   return retcode;
   
   /*
    * accept server connection
    */
   if( AcceptConnection() <= 0) {
       fclose(fp);
       return retcode;
   }
   /* 
    * now send file
    */
   
   fd = fileno(fp);
#if (defined(_WIN32) || defined(WIN32))
	
   printf("Press ESC to abort\r\n");
#else
   printf("Type q and hit return to abort\r\n");
#endif
   while( (nBytesRead=read(fd,szBuffer,1024)) > 0) {
      SendDataMsg(szBuffer,nBytesRead);
      nTotal+=nBytesRead;
      printf("%s : %d sent\r",fname,nTotal);
	   if( CheckInput() ) {
#if (defined(_WIN32) || defined(WIN32))

			if( !(ch = getch()) ) getch();
			if( ch == 27 )
				aborted = 1;
#else
			ch = getchar();
			if( ch != '\n') {
				while( getchar() != '\n') ;      /* read 'til new line */
			}
			if( ch == 'q') 
				aborted = 1;
#endif
	   }

	   /*
	    * send an abort command to server if we aborted.
	    */
	   if( aborted ) {
		  
		   printf("\r\nAbort: Waiting for server to finish.");
		   SendControlMsg(abortstr,(int)strlen(abortstr));
		   break;
	   }
   }

   printf("\r\n");
   /*
    * close data connection
    */
   CloseDataConnection(hDataSocket);
   close(fd);
   fclose(fp);
   retcode=GetReply();
   return retcode;

}


int ftp::DoMakeDir(  char *command)
{ 
	int responseCode=0;

	if( !Connected ) {
      printf("Not Connected.\n");
      return responseCode;
   }

    if( GetListenSocket() < 0) {
       printf("Cannot obtain a listen socket.\n");
       return responseCode;
   }


   sprintf(szBuffer,"MKD %s\r\n", command);
   SendControlMsg(szBuffer,(int)strlen(szBuffer));
   responseCode=GetReply();
   return responseCode;
   
}


int ftp::DoCDUP()
{
	int rectCode=0;
  if( !Connected ) {
      printf("Not Connected.\n");
      return 0;
  }
  sprintf(szBuffer, "CDUP\r\n");
  SendControlMsg(szBuffer,(int)strlen(szBuffer));
  rectCode=GetReply();
  return rectCode;
}
