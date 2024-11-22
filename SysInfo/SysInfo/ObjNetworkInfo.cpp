#include "ObjNetworkInfo.h"
#include "iphlpapi.h"
//#include "Preferences.h"
#include "ObjProcesses.h"



CObjNetworkInfo::CObjNetworkInfo(void)
:m_oStr(CIssString::Instance())
,m_iCurrentIndex(-1)
,m_szDeviceName(NULL)
,m_szHostName(NULL)
,m_szIPAdress(NULL)
,m_szSubnetMask(NULL)
,m_szGateWay(NULL)
,m_szDNS1(NULL)
,m_szDNS2(NULL)
,m_szDHCP(NULL)
,m_szMacAddress(NULL)
,m_szLeaseObtained(NULL)
,m_szLeaseExpires(NULL)
,m_szDomainName(NULL)
,m_szWinPrimary(NULL)
,m_szWinSecondary(NULL)
,m_bDHCPEnabled(FALSE)
,m_bDNSEnabled(FALSE)
,m_bProxyEnbled(FALSE)
,m_eDeviceType(DT_Unknown)
,m_dwLastTickCount(0)
,m_dwBytesSent(0)
,m_dwBytesRecieved(0)
,m_dwMaxDownloadRate(100)
,m_dwMaxUploadRate(100)
,m_dwMaxRate(100)
,m_dwLinkSpeed(0)
,m_dwUploadRate(0)
,m_dwDownloadRate(0)
,m_dwLastBytesSent(0)
,m_dwLastBytesRecieved(0)
,m_hWndNotif(NULL)
,m_uiNotif(WM_USER)
,m_bCloseThread(FALSE)
,m_uiUpdated(0)
{
	ZeroMemory(m_chDownloadRate, sizeof(char)*MAX_HISTORY);
	ZeroMemory(m_chUploadRate, sizeof(char)*MAX_HISTORY);
	InitializeCriticalSection(&m_cr);
}

CObjNetworkInfo::~CObjNetworkInfo(void)
{
	Destroy();

	DeleteCriticalSection(&m_cr);
}

BOOL CObjNetworkInfo::Destroy()
{
	if(m_hThreadNetworkInfo)
	{
		DWORD dwExitCode;
		if(GetExitCodeThread(m_hThreadNetworkInfo, &dwExitCode))
		{
			// if the thread is still going
			if(dwExitCode == STILL_ACTIVE)
			{
				// let's try and gracefully shut it down
				m_bCloseThread = TRUE;
				int iCount = 0;
				do 
				{
					Sleep(50);
					GetExitCodeThread(m_hThreadNetworkInfo, &dwExitCode);
					iCount++;
				} while(dwExitCode == STILL_ACTIVE && iCount < 20);

				// if the thread is still going we have to forcefully kill it
				if(dwExitCode == STILL_ACTIVE)
					TerminateThread(m_hThreadNetworkInfo, 999);

				// memory cleanup
				CloseHandle(m_hThreadNetworkInfo);
			}
			m_hThreadNetworkInfo = NULL;
		}
	}

	ResetValues();
	ZeroMemory(m_chDownloadRate, sizeof(char)*MAX_HISTORY);
	ZeroMemory(m_chUploadRate, sizeof(char)*MAX_HISTORY);

	return TRUE;
}

BOOL CObjNetworkInfo::Init(HWND hWndNotif, UINT uiNotif)
{
	m_hWndNotif	= hWndNotif;
	m_uiNotif	= uiNotif;

	StartThread();
	return TRUE;
}

void CObjNetworkInfo::StartThread()
{
	// refreshes take a couple seconds so do it in a thread
	if(m_hThreadNetworkInfo)
	{
		DWORD dwExitCode;
		if(GetExitCodeThread(m_hThreadNetworkInfo, &dwExitCode))
		{
			// if the thread is still going
			if(dwExitCode == STILL_ACTIVE)
				return;
		}

		// memory cleanup
		CloseHandle(m_hThreadNetworkInfo);
		m_hThreadNetworkInfo = NULL;
	}
	m_hThreadNetworkInfo = CreateThread(NULL, 0, &ThreadRefreshNetworkInfo, (LPVOID)this, CREATE_SUSPENDED, NULL);

	// is the thread going?
	if(m_hThreadNetworkInfo)
	{
		m_bCloseThread = FALSE;

		// make sure it runs in the background
		SetThreadPriority(m_hThreadNetworkInfo, THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(m_hThreadNetworkInfo);
	}
}

DWORD CObjNetworkInfo::ThreadRefreshNetworkInfo(LPVOID lpVoid)
{
	CObjNetworkInfo* oEng = (CObjNetworkInfo*)lpVoid;
	if(!oEng)
		return 0;

	while(!oEng->m_bCloseThread)
	{
		// do the actual refresh in the thread
		oEng->RefreshNetworkInfo();

		// quick check to see if the thread should close
		if(oEng->m_bCloseThread)
			break;

		// Tell the UI that there is new stuff
		PostMessage(oEng->m_hWndNotif, oEng->m_uiNotif, 0,0);
		Sleep(UPDATE_TIME);
	}

	return 0;
}

void CObjNetworkInfo::RefreshNetworkInfo()
{
	// check to see if we should stop updating
	if(m_bCloseThread)
		return;

	EnterCriticalSection(&m_cr);

	// Shift history
	memmove(m_chUploadRate+1, m_chUploadRate, MAX_HISTORY-1);
	memmove(m_chDownloadRate+1, m_chDownloadRate, MAX_HISTORY-1);

	// set the new entry to 0
	m_chUploadRate[0]	= 0;
	m_chDownloadRate[0] = 0;

	// set the update to nothing
	m_uiUpdated = 0;

	LeaveCriticalSection(&m_cr);

	EnterCriticalSection(&m_cr);
	if(!GetActiveInterface())
	{
		ResetValues();
		LeaveCriticalSection(&m_cr);
		return;
	}
	LeaveCriticalSection(&m_cr);

	// check to see if we should stop updating
	if(m_bCloseThread)
		return;

	EnterCriticalSection(&m_cr);
	if(!GetNetworkInfo())
	{
		ResetValues();
		LeaveCriticalSection(&m_cr);
		return;
	}
	LeaveCriticalSection(&m_cr);

	// check to see if we should stop updating
	if(m_bCloseThread)
		return;

	EnterCriticalSection(&m_cr);
	if(!GetTransferInfo())
		ResetValues();
	LeaveCriticalSection(&m_cr);

	return;
}

void CObjNetworkInfo::ResetValues()
{
	m_uiUpdated = UPDATE_All;	// update everything
	m_iCurrentIndex = -1;
	m_oStr->Delete(&m_szDeviceName);
	m_oStr->Delete(&m_szHostName);
	m_oStr->Delete(&m_szIPAdress);
	m_oStr->Delete(&m_szSubnetMask);
	m_oStr->Delete(&m_szGateWay);
	m_oStr->Delete(&m_szDNS1);
	m_oStr->Delete(&m_szDNS2);
	m_oStr->Delete(&m_szDHCP);
	m_oStr->Delete(&m_szMacAddress);
	m_oStr->Delete(&m_szLeaseObtained);
	m_oStr->Delete(&m_szLeaseExpires);
	m_oStr->Delete(&m_szDomainName);
	m_oStr->Delete(&m_szWinPrimary);
	m_oStr->Delete(&m_szWinSecondary);
	m_bDHCPEnabled	= FALSE;
	m_bDNSEnabled	= FALSE;
	m_bProxyEnbled	= FALSE;
	m_eDeviceType	= DT_Unknown;
	m_dwLastTickCount	= 0;
	m_dwBytesSent		= 0;
	m_dwBytesRecieved	= 0;
	m_dwLastBytesRecieved= 0;
	m_dwLastBytesSent	= 0;
	m_dwUploadRate		= 0;
	m_dwDownloadRate	= 0;
	m_dwMaxUploadRate	= 100;
	m_dwMaxDownloadRate	= 100;
	m_dwMaxRate			= 100;
	m_dwLinkSpeed		= 0;
}


BOOL CObjNetworkInfo::GetActiveInterface()
{
	// Interface name enumeration
	IP_INTERFACE_INFO *pIpInterface = NULL;
	DWORD dwInterfaceSize = 0;

	// Find out the size of the interface table
	if(GetInterfaceInfo(NULL, &dwInterfaceSize) != ERROR_INSUFFICIENT_BUFFER)
		return FALSE;

	pIpInterface	= (IP_INTERFACE_INFO*) new BYTE[dwInterfaceSize];
	if(!pIpInterface)
		return FALSE;

	if(GetInterfaceInfo(pIpInterface, &dwInterfaceSize) != NO_ERROR) 
	{
		delete [] pIpInterface;
		return FALSE;
	}

	// Note: if there are no adapters attached then we draw the Icon with the X
	if(pIpInterface->NumAdapters == 0 || m_oStr->IsEmpty(pIpInterface->Adapter[0].Name))
	{
		delete [] pIpInterface;
		return FALSE;
	}

	// do a check here to see if our previous check has changed and if it has then we will have to reset everything
	if(m_iCurrentIndex != pIpInterface->Adapter[0].Index || !m_szDeviceName || m_oStr->Compare(m_szDeviceName, pIpInterface->Adapter[0].Name) != 0)
	{
		ResetValues();
	}

	m_iCurrentIndex	= pIpInterface->Adapter[0].Index;
	m_oStr->Delete(&m_szDeviceName);
	m_szDeviceName	= m_oStr->CreateAndCopy(pIpInterface->Adapter[0].Name);

	delete [] pIpInterface;

#define	NDISUIO_DEVICE_NAME						TEXT("UIO1:")
#define OID_GEN_PHYSICAL_MEDIUM                 0x00010202
#define METHOD_BUFFERED							0
#define FILE_ANY_ACCESS							0
#ifndef FSCTL_NDISUIO_BASE
#define FSCTL_NDISUIO_BASE 						0x00000012
#define CTL_CODE( DeviceType, Function, Method, Access ) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif

#define _NDISUIO_CTL_CODE(_Function, _Method, _Access)   CTL_CODE(FSCTL_NDISUIO_BASE, _Function, _Method, _Access)
#define IOCTL_NDISUIO_QUERY_OID_VALUE			_NDISUIO_CTL_CODE(0x201, METHOD_BUFFERED, FILE_ANY_ACCESS)

	enum _NDIS_PHYSICAL_MEDIUM
	{
		NdisPhysicalMediumUnspecified,
		NdisPhysicalMediumWirelessLan,
		NdisPhysicalMediumCableModem,
		NdisPhysicalMediumPhoneLine,
		NdisPhysicalMediumPowerLine,
		NdisPhysicalMediumDSL,      // includes ADSL and UADSL (G.Lite)
		NdisPhysicalMediumFibreChannel,
		NdisPhysicalMedium1394,
		NdisPhysicalMediumWirelessWan,
		NdisPhysicalMediumNativeWiFi,
		NdisPhysicalMediumMax       // Not a real physical type, defined as an upper-bound
	};

	typedef ULONG NDIS_OID, *PNDIS_OID;

	typedef struct _NDISUIO_QUERY_OID
	{
		NDIS_OID        Oid;
		PTCHAR			ptcDeviceName;	
		UCHAR           Data[sizeof(ULONG)];

	} NDISUIO_QUERY_OID, *PNDISUIO_QUERY_OID;

	HANDLE hDevice = CreateFile( /*DD_NDIS_DEVICE_NAME*/NDISUIO_DEVICE_NAME, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		NULL,  
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
		INVALID_HANDLE_VALUE );  


	// Query the device for its type
	DWORD    dwBytesReturned		= 0;
	BOOL	 bResult				=  FALSE;

	PNDISUIO_QUERY_OID pQueryOid = NULL;
	UCHAR QueryBuffer[sizeof(NDISUIO_QUERY_OID)+sizeof(DWORD)];

	pQueryOid					= (PNDISUIO_QUERY_OID) &QueryBuffer[0];
	pQueryOid->ptcDeviceName	= m_szDeviceName;
	pQueryOid->Oid				= OID_GEN_PHYSICAL_MEDIUM;

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(	hDevice, 
			IOCTL_NDISUIO_QUERY_OID_VALUE,		//IOCTL_NDIS_QUERY_GLOBAL_STATS,	this does not work in CE
			(LPVOID) pQueryOid,
			sizeof(QueryBuffer),
			(LPVOID) pQueryOid,
			sizeof(QueryBuffer),
			&dwBytesReturned,
			NULL);

		// Close the device
		CloseHandle(hDevice);

		if(!bResult)
		{
			/*DWORD dwError = GetLastError();			
			TCHAR* sz = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dwError,
				0,
				(LPTSTR)&sz,
				0,
				NULL);
			if(sz)
				delete [] sz;*/
		}
	}

	// find out what kind of device
	EnumDeviceType eOld = m_eDeviceType;
	m_eDeviceType	= DT_Unknown;
	if(bResult)
	{
		DWORD dwPhysicalMedium = (DWORD)pQueryOid->Data[0];
		switch((_NDIS_PHYSICAL_MEDIUM)dwPhysicalMedium)
		{
		case NdisPhysicalMediumWirelessLan:
		case NdisPhysicalMediumNativeWiFi:
			// wifi
			m_eDeviceType	= DT_Wifi;
			break;
		case NdisPhysicalMediumWirelessWan:
			// wirelesss CDMA, GPRS, etc.
			m_eDeviceType = DT_WirelessWan;
			break;
		default:
			// ethernet of some sort, assume active sync
			m_eDeviceType = DT_Ethernet;
			break;
		}
	}
	else
	{
		// The driver call failed, so if we're docked we put active sync
		// otherwise we assume GPRS
		if(IsDocked())
			m_eDeviceType = DT_Ethernet;
	}

	if(eOld != m_eDeviceType)
		m_uiUpdated	|= UPDATE_DeviceType;

	return TRUE;
}

BOOL CObjNetworkInfo::IsDocked()
{

	//First, check if the tcpip reg key is present.
	//We add this step, as the 2nd IP address check seems to stay around after the WiFi connection has been disconnected.
	HKEY hKey = NULL;
	LRESULT lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Comm\\Tcpip\\Hosts\\ppp_peer"), 0L, 0L, &hKey);

	if(lr == ERROR_SUCCESS && hKey)
	{
		//Yes, we're really connected.
		RegCloseKey(hKey);
		hKey = NULL;
	}
	else
	{

		// we may have WM2005...don't return just yet.
		lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Comm\\Tcpip\\Hosts\\dtpt_peer"), 0L, 0L, &hKey);

		if(lr == ERROR_SUCCESS && hKey)
		{
			//Yes, we're really connected.
			RegCloseKey(hKey);
			hKey = NULL;
		}
		else
		{
			// we're not connected at all on either Wm2005 or earlier devices.
			return FALSE;
		}
	}

	//If this reg key was present, we are connected to the web.
	//However, we could be connected via Wi-Fi or GPRS. Check - are we actually docked?
	ULONG ulOutBufLen = 0;
	CIssString* oStr = CIssString::Instance();

	DWORD dwResult = GetAdaptersInfo(NULL, &ulOutBufLen);
	if(dwResult == ERROR_BUFFER_OVERFLOW)
	{
		//Got how long the buffer should be. Alloc the buffer.
		BYTE* pbBuffer = new BYTE[ulOutBufLen];
		if(pbBuffer)
		{
			dwResult = GetAdaptersInfo((PIP_ADAPTER_INFO)pbBuffer, &ulOutBufLen);
			IP_ADAPTER_INFO* pSearch = (PIP_ADAPTER_INFO)pbBuffer;

			while(pSearch)
			{
				if(pSearch->CurrentIpAddress)
				{
					if( (strcmp(pSearch->CurrentIpAddress->IpAddress.String, "192.168.55.101") == 0) || 
						(strcmp(pSearch->CurrentIpAddress->IpAddress.String, "192.168.55.100") == 0) ||
						(oStr->Find(pSearch->CurrentIpAddress->IpAddress.String, "169.254.") != -1))
					{
						//We're docked - detected a USB connection.
						delete [] pbBuffer;

						return TRUE;
					}
				}
				pSearch = pSearch->Next;
			}
			delete [] pbBuffer;
		}
	}

	return FALSE;
}

BOOL CObjNetworkInfo::GetNetworkInfo()
{
	PIP_ADAPTER_INFO	pAdapter		= NULL;
	DWORD				dwRetVal		= 0;
	FIXED_INFO *		FixedInfo		= NULL;
	IP_ADDR_STRING *	pIPAddr			= NULL;
	ULONG				ulOutBufLen		= 0;
	IP_ADAPTER_INFO *	pIpAdapterInfo	= NULL;
	DWORD				dwAdapterSize	= 0;

	// reset all the old info we're going to change here
	/*m_oStr->Delete(&m_szHostName);
	m_oStr->Delete(&m_szDomainName);
	m_oStr->Delete(&m_szDNS1);
	m_oStr->Delete(&m_szDNS2);
	m_bDNSEnabled	= FALSE;
	m_bProxyEnbled	= FALSE;
	m_oStr->Delete(&m_szLeaseExpires);
	m_oStr->Delete(&m_szLeaseObtained);
	m_oStr->Delete(&m_szGateWay);
	m_oStr->Delete(&m_szSubnetMask);
	m_oStr->Delete(&m_szIPAdress);
	m_oStr->Delete(&m_szWinPrimary);
	m_oStr->Delete(&m_szWinSecondary);
	m_oStr->Delete(&m_szDHCP);
	m_bDHCPEnabled = FALSE;*/

	// Find out the size of the adapter info table, allocate and call again
	if(GetAdaptersInfo(NULL, &dwAdapterSize) != ERROR_BUFFER_OVERFLOW)
		return FALSE;

	pIpAdapterInfo		= (IP_ADAPTER_INFO*) new BYTE[dwAdapterSize];
	if(!pIpAdapterInfo)
		return FALSE;

	if(GetAdaptersInfo(pIpAdapterInfo, &dwAdapterSize) != NO_ERROR) 
	{
		//LocalFree(pIpAdapterInfo);
		delete [] pIpAdapterInfo;
		return FALSE;
	}

	pAdapter = pIpAdapterInfo;

	ulOutBufLen = sizeof( FIXED_INFO );
	FixedInfo	= (FIXED_INFO*) new BYTE[ulOutBufLen];

	// retrieve the network parameters.  get back the length
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams( FixedInfo, &ulOutBufLen ) ) 
	{
		delete [] FixedInfo;
		FixedInfo = (FIXED_INFO*) new BYTE[ulOutBufLen];
	}

	if ( dwRetVal = GetNetworkParams( FixedInfo, &ulOutBufLen ) ) 
	{
		delete [] pIpAdapterInfo;
		delete [] FixedInfo;
		return FALSE;
	}

	//Now lets fill our Network Structure

	//Host Name
	if(m_oStr->Compare(m_szHostName, FixedInfo->HostName) != 0)
	{
		m_uiUpdated |= UPDATE_HostName;
		m_oStr->Delete(&m_szHostName);
		m_szHostName	= m_oStr->CreateAndCopy(FixedInfo->HostName);
	}

	//Domain Name
	if(m_oStr->Compare(m_szDomainName, FixedInfo->DomainName) != 0)
	{
		m_uiUpdated |= UPDATE_DomainName;
		m_oStr->Delete(&m_szDomainName);
		m_szDomainName	= m_oStr->CreateAndCopy(FixedInfo->DomainName);
	}

	//DNS Servers
	if(m_oStr->Compare(m_szDNS1, FixedInfo->DnsServerList.IpAddress.String) != 0)
	{
		m_uiUpdated |= UPDATE_DNS1;
		m_oStr->Delete(&m_szDNS1);
		m_szDNS1	= m_oStr->CreateAndCopy(FixedInfo->DnsServerList.IpAddress.String);
	}

	pIPAddr = FixedInfo -> DnsServerList.Next;
	if(pIPAddr && m_oStr->Compare(m_szDNS2, pIPAddr->IpAddress.String) != 0)
	{
		m_uiUpdated |= UPDATE_DNS2;
		m_oStr->Delete(&m_szDNS2);
		m_szDNS2	= m_oStr->CreateAndCopy(pIPAddr->IpAddress.String);
	}

	if(m_bDNSEnabled != FixedInfo->EnableDns)
		m_uiUpdated |= UPDATE_DNSEnabled;
	m_bDNSEnabled=(BOOL)FixedInfo->EnableDns;

	//Proxy Enabled?
	if(m_bProxyEnbled != FixedInfo->EnableProxy)
		m_uiUpdated |= UPDATE_ProxyEnbled;
	m_bProxyEnbled=(BOOL)FixedInfo->EnableProxy;

	SYSTEMTIME	sysTime;
	TCHAR		szTemp[STRING_MAX];
	TCHAR		szDate[STRING_LARGE];
	TCHAR		szTime[STRING_LARGE];

	while (pAdapter)
	{
		if((int)pAdapter->Index == m_iCurrentIndex)
		{
			// lease obtained
			TimeToSystemTime(pAdapter->LeaseExpires, &sysTime);
			GetDateFormat(NULL, NULL, &sysTime, TEXT("MM/dd/yy "), szDate, sizeof(szDate));
			GetTimeFormat(NULL, NULL, &sysTime, TEXT("hh:mm"), szTime, sizeof(szTime));
			m_oStr->StringCopy(szTemp, szDate);
			m_oStr->Concatenate(szTemp, szTime);
			if(m_oStr->Compare(m_szLeaseExpires, szTemp) != 0)
			{
				m_uiUpdated |= UPDATE_LeaseExpires;
				m_oStr->Delete(&m_szLeaseExpires);
				m_szLeaseExpires	= m_oStr->CreateAndCopy(szTemp);
			}

			// lease expires
			TimeToSystemTime(pAdapter->LeaseObtained, &sysTime);
			GetDateFormat(NULL, NULL, &sysTime, TEXT("MM/dd/yy "), szDate, sizeof(szDate));
			GetTimeFormat(NULL, NULL, &sysTime, TEXT("hh:mm"), szTime, sizeof(szTime));
			m_oStr->StringCopy(szTemp, szDate);
			m_oStr->Concatenate(szTemp, szTime);
			if(m_oStr->Compare(m_szLeaseObtained, szTemp) != 0)
			{
				m_uiUpdated |= UPDATE_LeaseObtained;
				m_oStr->Delete(&m_szLeaseObtained);
				m_szLeaseObtained	= m_oStr->CreateAndCopy(szTemp);
			}

			// gateway
			if(m_oStr->Compare(m_szGateWay, pAdapter->GatewayList.IpAddress.String) != 0)
			{
				m_uiUpdated |= UPDATE_GateWay;
				m_oStr->Delete(&m_szGateWay);
				m_szGateWay	= m_oStr->CreateAndCopy(pAdapter->GatewayList.IpAddress.String);
			}

			// subnetmask
			if(m_oStr->Compare(m_szSubnetMask, pAdapter->IpAddressList.IpMask.String) != 0)
			{
				m_uiUpdated |= UPDATE_SubnetMask;
				m_oStr->Delete(&m_szSubnetMask);
				m_szSubnetMask	= m_oStr->CreateAndCopy(pAdapter->IpAddressList.IpMask.String);
			}

			// ipaddress
			if(m_oStr->Compare(m_szIPAdress, pAdapter->IpAddressList.IpAddress.String) != 0)
			{
				m_uiUpdated |= UPDATE_IPAdress;
				m_oStr->Delete(&m_szIPAdress);
				m_szIPAdress	= m_oStr->CreateAndCopy(pAdapter->IpAddressList.IpAddress.String);
			}

			// Win Servers
			if(m_oStr->Compare(m_szWinPrimary, pAdapter->PrimaryWinsServer.IpAddress.String) != 0)
			{
				m_uiUpdated |= UPDATE_WinPrimary;
				m_oStr->Delete(&m_szWinPrimary);
				m_szWinPrimary	= m_oStr->CreateAndCopy(pAdapter->PrimaryWinsServer.IpAddress.String);
			}

			if(m_oStr->Compare(m_szWinSecondary, pAdapter->SecondaryWinsServer.IpAddress.String) != 0)
			{
				m_uiUpdated |= UPDATE_WinSecondary;
				m_oStr->Delete(&m_szWinSecondary);
				m_szWinSecondary	= m_oStr->CreateAndCopy(pAdapter->SecondaryWinsServer.IpAddress.String);
			}

			//DHCP
			if(m_oStr->Compare(m_szDHCP, pAdapter->DhcpServer.IpAddress.String) != 0)
			{
				m_uiUpdated |= UPDATE_DHCP;
				m_oStr->Delete(&m_szDHCP);
				m_szDHCP	= m_oStr->CreateAndCopy(pAdapter->DhcpServer.IpAddress.String);
			}

			if(m_bDHCPEnabled != pAdapter->DhcpEnabled)
				m_uiUpdated |= UPDATE_DHCPEnabled;
			m_bDHCPEnabled	= pAdapter->DhcpEnabled;
		}

		pAdapter = pAdapter->Next;
	}

	// Cleanup our stuff
	delete [] pIpAdapterInfo;
	delete [] FixedInfo;

	return TRUE;
}

void CObjNetworkInfo::TimeToSystemTime(time_t t, SYSTEMTIME *pSysTime)
{
	FILETIME ft, ftLocal;

	if(!pSysTime)
		return;

	// Convert to File Time
	LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD) ll;
	ft.dwHighDateTime = (DWORD) (ll >>32);

	// Convert to Local System Time
	FileTimeToLocalFileTime(&ft, &ftLocal);
	FileTimeToSystemTime(&ftLocal, pSysTime);
	return;
}

BOOL CObjNetworkInfo::GetTransferInfo()
{
	MIB_IFROW	sIfRow = {0};
	sIfRow.dwIndex	= m_iCurrentIndex;

	if (GetIfEntry(&sIfRow) != NO_ERROR) 
	{
		// reset some of the stuff
		m_oStr->Delete(&m_szMacAddress);
		m_dwBytesRecieved	= 0;
		m_dwBytesSent		= 0;
		m_dwMaxUploadRate	= 100;
		m_dwMaxDownloadRate	= 100;
		m_dwMaxRate			= 100;
		m_dwUploadRate		= 0;
		m_dwDownloadRate	= 0;
		m_dwLastBytesSent	= 0;
		m_dwLastBytesRecieved = 0;
		m_dwLastTickCount	= 0;
		m_dwLinkSpeed		= 0;
		//ZeroMemory(m_chDownloadRate, sizeof(char)*MAX_HISTORY);
		//ZeroMemory(m_chUploadRate, sizeof(char)*MAX_HISTORY);

		return FALSE;
	}

	DWORD dwNewTick = GetTickCount();

	if(m_dwLinkSpeed != sIfRow.dwSpeed)
		m_uiUpdated	|= UPDATE_LinkSpeed;
	m_dwLinkSpeed = sIfRow.dwSpeed;

	//MAC Address
	char macStr[30];
	int j;
	for (j = 0 ; j < (int)sIfRow.dwPhysAddrLen; ++j)
	{
		sprintf( &macStr[j*3], "%02X-", sIfRow.bPhysAddr[j] );
	}
	macStr[j*3-1] = '\0';
	if(m_oStr->Compare(m_szMacAddress, macStr) != 0)
	{
		m_uiUpdated |= UPDATE_MacAddress;
		m_oStr->Delete(&m_szMacAddress);
		m_szMacAddress	= m_oStr->CreateAndCopy(macStr);
	}

	// save the transfer information
	DWORD dwTemp = sIfRow.dwInOctets;
	if(m_dwBytesRecieved != dwTemp)
		m_uiUpdated |= UPDATE_BytesRecieved;
	m_dwBytesRecieved	= dwTemp;

	dwTemp	= sIfRow.dwOutOctets;
	if(m_dwBytesSent != dwTemp)
		m_uiUpdated |= UPDATE_BytesSent;
	m_dwBytesSent		= dwTemp;

	// increment our traffic counters
	if(m_dwLastTickCount == 0 || dwNewTick == m_dwLastTickCount)
	{
		// this is the first check so zero for now
		m_dwMaxUploadRate	= 100;
		m_dwMaxDownloadRate	= 100;
		m_dwMaxRate			= 100;
		m_dwUploadRate		= 0;
		m_dwDownloadRate	= 0;
		//ZeroMemory(m_chDownloadRate, sizeof(char)*MAX_HISTORY);
		//ZeroMemory(m_chUploadRate, sizeof(char)*MAX_HISTORY);
		m_dwLastBytesSent	= m_dwBytesSent;
		m_dwLastBytesRecieved = m_dwBytesRecieved;
		m_dwLastTickCount	= dwNewTick;
		m_uiUpdated			|= UPDATE_UploadRate;
		m_uiUpdated			|= UPDATE_DownloadRate;
//		m_uiUpdated			|= UPDATE_MaxUploadRate;
//		m_uiUpdated			|= UPDATE_MaxDownloadRate;
		m_uiUpdated			|= UPDATE_MaxRate;
		return TRUE;
	}

	dwTemp			= 1000*(m_dwBytesSent-m_dwLastBytesSent)/(dwNewTick-m_dwLastTickCount);
	if(m_dwUploadRate != dwTemp)
		m_uiUpdated	|= UPDATE_UploadRate;
	m_dwUploadRate	= dwTemp;

	dwTemp			= 1000*(m_dwBytesRecieved-m_dwLastBytesRecieved)/(dwNewTick-m_dwLastTickCount);
	if(m_dwDownloadRate!= dwTemp)
		m_uiUpdated	|= UPDATE_DownloadRate;
	m_dwDownloadRate	= dwTemp;

//	DWORD dwNewMax = max(m_dwUploadRate,m_dwDownloadRate);
//	DWORD dwOldMax = max(m_dwMaxDownloadRate,m_dwMaxUploadRate);
	DWORD dwNewMax = m_dwUploadRate + m_dwDownloadRate;

	// see if we have to change the graph scale
	if(dwNewMax > m_dwMaxRate)
	{
		dwNewMax = GetRoundedRate(dwNewMax);

		// scale all the values to the new maximum height
		for(int i=0; i<MAX_HISTORY; i++)
		{
			m_chUploadRate[i]	= (char)(m_chUploadRate[i]*m_dwMaxRate/dwNewMax);
			m_chDownloadRate[i] = (char)(m_chDownloadRate[i]*m_dwMaxRate/dwNewMax);
		}

		m_dwMaxRate = dwNewMax;
	}

	//not used
	if(m_dwMaxUploadRate < m_dwUploadRate)
	{
		m_uiUpdated		  |= UPDATE_MaxUploadRate;
		m_dwMaxUploadRate = m_dwUploadRate;
	}

	//not used
	if(m_dwMaxDownloadRate < m_dwDownloadRate)
	{
		m_uiUpdated			|= UPDATE_MaxDownloadRate;
		m_dwMaxDownloadRate = m_dwDownloadRate;
	}

	// Shift history .. done in Refresh Now
/*	memmove(m_chUploadRate+1, m_chUploadRate, MAX_HISTORY-1);
	memmove(m_chDownloadRate+1, m_chDownloadRate, MAX_HISTORY-1); */

	// save history
	m_chUploadRate[0]	= (char)(100*m_dwUploadRate/m_dwMaxRate);
	m_chDownloadRate[0] = (char)(100*m_dwDownloadRate/m_dwMaxRate);

	// update the old values
	m_dwLastBytesSent		= m_dwBytesSent;
	m_dwLastBytesRecieved	= m_dwBytesRecieved;
	m_dwLastTickCount		= dwNewTick;

	return TRUE;	
}

DWORD CObjNetworkInfo::GetRoundedRate(DWORD dwIn)
{
	DWORD dwPow10 = 10;

	while(dwPow10*10 < dwIn)
	{
		dwPow10 *= 10;
	}

	DWORD dwRounded = dwPow10;

	while(dwRounded < dwIn)
	{
		dwRounded += dwPow10;
	}

	return dwRounded;

}
