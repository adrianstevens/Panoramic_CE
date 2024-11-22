#pragma once

#include <snapi.h>
#include <regext.h>

enum WindowsMessage
{
	WM_NOTIFY_TIME = WM_USER + 500,
	WM_NOTIFY_DATE,
	WM_NOTIFY_MISSED,
	WM_NOTIFY_SMS,
	WM_NOTIFY_VOICEMAIL,
	WM_NOTIFY_EMAIL,
    WM_NOTIFY_PROFILE,
    WM_NOTIFY_TALKING,
    WM_NOTIFY_RINGVOLUME,
    WM_NOTIFY_RINGNAME,
    WM_NOTIFY_BLUETOOTH,
    WM_NOTIFY_WIFI,
    WM_NOTIFY_FLIGHTMODE,
};

// our own defines
#define SN_VOICEMAILPHONENUMBER_ROOT	HKEY_CURRENT_USER
#define SN_VOICEMAILPHONENUMBER_PATH	_T("Software\\Microsoft\\Vmail")
#define SN_VOICEMAILPHONENUMBER_VALUE	_T("PhoneNumber1")




class CIssStateAndNotify
{
public:
	CIssStateAndNotify(void);
	~CIssStateAndNotify(void);

    static void InitializeNotifications();
    static void DestroyNotifications();

	static BOOL	RegisterWindowNotification(HWND hWndTarget, WindowsMessage msg);
	static BOOL	RegisterCallbackNotification(DWORD dwUserData, void* callback);
    static BOOL RegisterCustomWindowNotification(HWND hWndTarget, HKEY hKeyStart, TCHAR* szKey, TCHAR* szValue, UINT uiMessage);

	// incoming
	static BOOL	GetLastCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
	static BOOL	GetIncomingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
	static BOOL	GetIncomingCallerName(TCHAR* szName, DWORD dwSize);
	static BOOL	GetIncomingCallerContactOID(long* pOid);

	// outgoing
	static BOOL	GetOutgoingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
	static BOOL	GetOutgoingCallerName(TCHAR* szName, DWORD dwSize);
	static BOOL	GetOutgoingCallerContactOID(long* pOid);

    static BOOL GetPhoneRadioOff();
	static BOOL GetBusyStatus();
	static int	GetUnreadEmailCount();
	static int	GetMissedCallsCount();
	static BOOL	ResetMissedCallsCount();
	static int	GetUnreadSMSCount();
	static int	GetUnReadVoicemailCount();
	static BOOL	GetServiceProviderName(TCHAR* szName, DWORD dwSize);
	static BOOL GetVoiceMailNumber(TCHAR* szName, DWORD dwSize);

private:	// functions

private:	// variables
};
