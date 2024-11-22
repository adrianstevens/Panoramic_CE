#pragma once

#include <snapi.h>
#include <regext.h>



class CObjStateAndNotifyMgr
{
public:
	CObjStateAndNotifyMgr(void);
	~CObjStateAndNotifyMgr(void);

	static void	RegisterWindowNotifications(HWND hWndTarget);

	// incoming
	static BOOL	GetLastCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
    static BOOL GetLastCallerName(TCHAR* szName, DWORD dwSize);
    static BOOL GetLastCallerType(TCHAR* szType, DWORD dwSize);
    static BOOL GetLastCallerOID(long* pOid);
    
	static BOOL	GetIncomingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
	static BOOL	GetIncomingCallerName(TCHAR* szName, DWORD dwSize);
	static BOOL	GetIncomingCallerContactOID(long* pOid);
    static BOOL GetPhoneOperator(TCHAR* szCarrier, DWORD dwSize);

	// outgoing
	static BOOL	GetOutgoingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize);
	static BOOL	GetOutgoingCallerName(TCHAR* szName, DWORD dwSize);
	static BOOL	GetOutgoingCallerContactOID(long* pOid);

	static BOOL GetBusyStatus();

    static BOOL GetBatteryLevel(int* iPcnt);
    static BOOL GetSignalStrength(int* iPcnt);

    static BOOL GetPhoneType(TCHAR* szPhoneType, DWORD dwSize);

    static BOOL GetActiveCallCount(int* iCount);
    static BOOL GetIsRoaming();

private:	// functions

private:	// variables
};
