#pragma once

#include "IssTapi.h"
#include "IssAreaCode.h"
#include "PoomContacts.h"


#define WM_PHONE_RINGING		WM_USER + 300
#define WM_CALL_ENDED			WM_USER + 301
#define WM_CALL_CONNECTED		WM_USER + 302
#define WM_PHONE_DIALIING		WM_USER + 303
#define WM_CALL_ONHOLD			WM_USER + 304

//#define WM_NOTIFY_INCOMING		WM_USER + 320

typedef long (*PLAYEVENTSOUND)(LPCTSTR, int);

enum EnumCallState
{
	CSTATE_Disconnected,
	CSTATE_Ringing,
	CSTATE_Connected,
	CSTATE_Dialing,
	CSTATE_OnHold,
	CSTATE_Unknown,
};


class CIssCallManager:public CIssTapi
{
public:
	CIssCallManager(void);
	~CIssCallManager(void);


	BOOL				Initialize(HINSTANCE hInst, HWND hWndDlg, HWND hWndDebugWindow = NULL);
	EnumCallState		GetCallState();

	BOOL				EndCall();
	BOOL				AnswerPhone(){AcceptLine(); return TRUE;};
	
	//incoming call
	TCHAR*				GetCallerNumber(){return m_sCallerID.szNumber;};
	TCHAR*				GetCallerName(){return m_sCallerID.szName;};
	CIssGDIEx*			GetCallerPicture(){return m_sCallerID.gdiPicture;}
	TCHAR*				GetCallerLocation();

	// notifications
	void				ReceiveNotification(UINT uiMessage, WPARAM wParam, LPARAM lParam);
	void				HandleIncoming(BOOL bBegin);
	void				HandleOutgoing();
	BOOL				HandleCallerName();
	void				GetCallerInfo();	// from ObjStateAndNoifyMgr
	void				HandleAreaCode();
	void				GetCallerPicture(long lOid);
	
protected:
	void 				HandleLineCallState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineDevState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineReply(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineClose(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineCallInfo(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineMonitorDigits(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				HandleLineAppNewCall(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	void 				DropCall(HCALL hCall);

	BOOL				DisplayIncomingCall();

	void				ClearCallerIDInfo();

	void				AcceptLine();
	void				AnswerLine();
	BOOL				CallAllowed(DWORD &dwCallPermissions);

private:	// functions

private:	// variables
	CIssAreaCode		m_oArea;		// area codes and time zones

	EnumCallState		m_eCallStatus;

	HWND				m_hWndDlg;
	HINSTANCE			m_hInst;

	HINSTANCE			m_hDLL;


};
