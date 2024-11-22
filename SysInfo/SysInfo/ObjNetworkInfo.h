#pragma once

#include "stdafx.h"
#include "IssString.h"

#define MAX_HISTORY						512

#define UPDATE_DeviceName				0x00000001	
#define UPDATE_HostName					0x00000002	
#define UPDATE_IPAdress					0x00000004	
#define UPDATE_SubnetMask				0x00000008	
#define UPDATE_GateWay					0x00000010	
#define UPDATE_DNS1						0x00000020	
#define UPDATE_DNS2						0x00000040	
#define UPDATE_DHCP						0x00000080	
#define UPDATE_MacAddress				0x00000100	
#define UPDATE_LeaseObtained			0x00000200	
#define UPDATE_LeaseExpires				0x00000400	
#define UPDATE_DomainName				0x00000800	
#define UPDATE_WinPrimary				0x00001000	
#define UPDATE_WinSecondary				0x00002000	
#define UPDATE_DHCPEnabled				0x00004000	
#define UPDATE_DNSEnabled				0x00008000	
#define UPDATE_ProxyEnbled				0x00010000	
#define UPDATE_DeviceType				0x00020000	
#define UPDATE_BytesSent				0x00040000	
#define UPDATE_BytesRecieved			0x00080000	
#define UPDATE_UploadRate				0x00100000	
#define UPDATE_DownloadRate				0x00200000	
#define UPDATE_MaxUploadRate			0x00400000
#define UPDATE_MaxDownloadRate			0x00800000
#define UPDATE_MaxRate					0x01000000
#define UPDATE_LinkSpeed				0x02000000
#define UPDATE_All						0xFFFFFFFF


enum EnumDeviceType
{
	DT_Unknown,
	DT_Wifi,
	DT_WirelessWan,
	DT_Ethernet,
};


class CObjNetworkInfo
{
public:
	CObjNetworkInfo(void);
	~CObjNetworkInfo(void);

	BOOL	Destroy();
	BOOL	Init(HWND hWndNotif, UINT uiNotif);
	UINT	GetUpdates(){return m_uiUpdated;};
	void	Lock(){EnterCriticalSection(&m_cr);};
	void	Unlock(){LeaveCriticalSection(&m_cr);};

private:	// functions
	BOOL	GetActiveInterface();
	BOOL	GetNetworkInfo();
	BOOL	GetTransferInfo();
	BOOL	IsDocked();
	void	TimeToSystemTime(time_t t, SYSTEMTIME *pSysTime);
	void	RefreshNetworkInfo();
	void	ResetValues();
	void	StartThread();
	static DWORD		ThreadRefreshNetworkInfo(LPVOID lpVoid);

	DWORD	GetRoundedRate(DWORD dwIn);

public:		// variables
	// I know it's not safe to put as public but it makes it easier on me
	// and I have to make sure to call lock and unlock

	// all the network info
	TCHAR*				m_szDeviceName;
	TCHAR*  			m_szHostName;
	TCHAR*				m_szIPAdress;
	TCHAR*				m_szSubnetMask;
	TCHAR*				m_szGateWay;
	TCHAR*				m_szDNS1;
	TCHAR*				m_szDNS2;
	TCHAR*				m_szDHCP;
	TCHAR*				m_szMacAddress;
	TCHAR*				m_szLeaseObtained;
	TCHAR*				m_szLeaseExpires;
	TCHAR*				m_szDomainName;
	TCHAR*				m_szWinPrimary;
	TCHAR*				m_szWinSecondary;
	BOOL				m_bDHCPEnabled;
	BOOL				m_bDNSEnabled;
	BOOL				m_bProxyEnbled;
	EnumDeviceType		m_eDeviceType;
	DWORD				m_dwBytesSent;
	DWORD				m_dwBytesRecieved;
	DWORD				m_dwUploadRate;
	DWORD				m_dwDownloadRate;
	DWORD				m_dwMaxUploadRate;
	DWORD				m_dwMaxDownloadRate;
	DWORD				m_dwMaxRate;
	char				m_chUploadRate[MAX_HISTORY];
	char				m_chDownloadRate[MAX_HISTORY];
	DWORD				m_dwLinkSpeed;

private:	// variables
	CIssString*			m_oStr;
	int					m_iCurrentIndex;
	HWND				m_hWndNotif;
	UINT				m_uiNotif;				// notification message index (WM_USER + ...)
	HANDLE				m_hThreadNetworkInfo;
	CRITICAL_SECTION	m_cr;	
	BOOL				m_bCloseThread;

	DWORD				m_dwLastTickCount;
	DWORD				m_dwLastBytesSent;
	DWORD				m_dwLastBytesRecieved;

	UINT				m_uiUpdated;			// to find out what has been updated

};
