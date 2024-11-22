#pragma once

#include "windows.h"
#include "IssString.h"
#include "IssVector.h"

#include <cemapi.h>

struct TypeSMSMessage
{
	TCHAR*	szNumber;
	TCHAR*	szSubject;
    FILETIME ft;
    BOOL    bReceived;
    BOOL    bRead;

	TypeSMSMessage(void);
	~TypeSMSMessage(void);

	void Clear();						// clear out so we start with a blank slate
    void Clone(TypeSMSMessage* sSMS);
};

enum EnumSMSFolder
{
    Enum_CE_IPM_INBOX_ENTRYID = 0,  // received sms
    Enum_CE_IPM_DRAFTS_ENTRYID,     // sent sms normally goes here
    Enum_IPM_SENTMAIL_ENTRYID       // this folder can be checked as well
};


class CSMSInterface
{
public:
	CSMSInterface(void);
	~CSMSInterface(void);

	static CSMSInterface*	Instance();
	static void				DeleteInstance();

	BOOL 					Initialize(HWND hWndApp = NULL);
	HRESULT 				OpenSession();
	HRESULT 				CloseSession();

    HRESULT                 GetSMSMessages(CIssVector<TypeSMSMessage> &arrMsgList, int iHowMany, FILETIME ftExpired, EnumSMSFolder eFolder);
    HRESULT                 SetSMSMessagesRead(int iHowMany);
    HRESULT                 SetSMSMessageRead(TypeSMSMessage* sMsg);
    HRESULT                 CheckMessageRead(CIssVector<TypeSMSMessage>& arrMsgs);
    HRESULT                 DeleteSMSMessage(TypeSMSMessage* sMsg);
    HRESULT                 DeleteSMSMessage(TypeSMSMessage* sMsg, EnumSMSFolder eFolder);
    HRESULT                 DeleteAllSMS();
    HRESULT                 DeleteAllSMS(EnumSMSFolder eFolder);

    HRESULT                 CountReceivedMessages(int& iCount);
    HRESULT                 CountSentMessages(int& iCount);

private:
    HRESULT                 GetWastebasketForFolder(IMAPISession *m_pSession, LPMAPIFOLDER pFolder, LPMAPIFOLDER* ppfldrWastebasket);
	
private:	// variables
	CIssString*				m_oStr;
	//BOOL					m_bInitialized;		// quick hack for now..
    ICEMAPISession*         m_pSession;
	static CSMSInterface*	m_Instance;

};
