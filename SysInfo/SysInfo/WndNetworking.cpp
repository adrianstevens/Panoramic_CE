#include "WndNetworking.h"

#define WM_NET_MESSAGE WM_USER + 400

CWndNetworking::CWndNetworking(void)
{
}

CWndNetworking::~CWndNetworking(void)
{
}


BOOL CWndNetworking::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);

    m_oStr->StringCopy(m_szName, _T("Networking"));

    Refresh();

    return TRUE;
}

void CWndNetworking::Refresh()
{
    m_oNetwork.Init(m_hWnd, WM_NET_MESSAGE);

 //   Clear();
 //   TCHAR szValue[STRING_MAX];
 //   m_oStr->StringCopy(szValue, m_oNetwork.m_szDeviceName);
//    OutputString(_T("Network:"), szValue);


 /*   m_oStr->Format(szValue, _T("%.0f bytes"), dbTotalMem);
    m_oStr->StringCopy(szName, _T("Total Memory:"));
    OutputString(szName, szValue);

    m_oStr->Format(szValue, _T("%.0f bytes"), dbAvalMem);
    m_oStr->StringCopy(szName, _T("Total Free:"));
    OutputString(szName, szValue);*/

}

void CWndNetworking::GetNetworkInfo()
{
    Clear();
    TCHAR szValue[STRING_MAX];
//    TCHAR szName[STRING_LARGE];

    m_oNetwork.Lock();


    m_oStr->StringCopy(szValue, m_oNetwork.m_szDeviceName);
    OutputString(_T("Network:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szHostName);
    OutputString(_T("Host Name:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szIPAdress);
    OutputString(_T("IP Address:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szSubnetMask);
    OutputString(_T("Subnet Mask:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szGateWay);
    OutputString(_T("Gateway:"), szValue);

    if(m_oNetwork.m_bDNSEnabled)
        m_oStr->StringCopy(szValue, m_oNetwork.m_szDNS1);
    else		
        m_oStr->StringCopy(szValue, _T("Disabled"));
    OutputString(_T("DNS:"), szValue);

    if(m_oNetwork.m_bDNSEnabled)
        m_oStr->StringCopy(szValue, m_oNetwork.m_szDNS2);
    else		
        m_oStr->StringCopy(szValue, _T("Disabled"));
    OutputString(_T("DNS 2:"), szValue);

    if(m_oNetwork.m_bDHCPEnabled)
        m_oStr->StringCopy(szValue, m_oNetwork.m_szDHCP);
    else
        m_oStr->StringCopy(szValue, _T("Disabled"));
    OutputString(_T("DHCP:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szMacAddress);
    OutputString(_T("MAC Address:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szLeaseObtained);
    OutputString(_T("Lease Obtained:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szLeaseExpires);
    OutputString(_T("Lease Expires:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szDomainName);
    //Domain Name:
    OutputString(_T(""), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szWinPrimary);
    OutputString(_T("WINS Primary:"), szValue);

    m_oStr->StringCopy(szValue, m_oNetwork.m_szWinSecondary);
    OutputString(_T("WINS Secondary:"), szValue);

    m_oStr->StringCopy(szValue, (m_oNetwork.m_bProxyEnbled?_T("Enabled"):_T("Disabled")));
    OutputString(_T("Proxy:"), szValue);

    
    switch(m_oNetwork.m_eDeviceType)
    {
    case DT_Ethernet:
        m_oStr->StringCopy(szValue, _T("Ethernet"));
        break;
    case DT_Wifi:
        m_oStr->StringCopy(szValue, _T("WiFi"));
        break;
    case DT_WirelessWan:
        m_oStr->StringCopy(szValue, _T("WAN"));
        break;
    case DT_Unknown:
        m_oStr->StringCopy(szValue, _T("Unknown"));
        break;
    }
    OutputString(_T("Type:"), szValue);

  /*  m_oStr->StringCopy(m_szLabel, IDS_NETWORK_BytesSent, STRING_NORMAL, m_hInst);
    FormatNumber(szValue, m_oNetwork.m_dwBytesSent);
    OutputString(_T(":"), szValue);

    m_oStr->StringCopy(m_szLabel, IDS_NETWORK_BytesReceived, STRING_NORMAL, m_hInst);
    FormatNumber(szValue, m_oNetwork.m_dwBytesRecieved);

    m_oStr->StringCopy(m_szLabel, IDS_NETWORK_DownloadRate, STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(szTemp, IDS_NETWORK_bps, STRING_SMALL, m_hInst);
    FormatNumber(szValue, m_oNetwork.m_dwDownloadRate);
    m_oStr->Concatenate(szValue, szTemp);

    m_oStr->StringCopy(m_szLabel, IDS_NETWORK_UploadRate, STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(szTemp, IDS_NETWORK_bps, STRING_SMALL, m_hInst);
    FormatNumber(szValue, m_oNetwork.m_dwUploadRate);
    m_oStr->Concatenate(szValue, szTemp);

    m_oStr->StringCopy(m_szLabel, IDS_NETWORK_LinkSpeed, STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(szTemp, IDS_NETWORK_bps, STRING_SMALL, m_hInst);
    FormatNumber(szValue, m_oNetwork.m_dwLinkSpeed);
    m_oStr->Concatenate(szValue, szTemp);*/

    m_oNetwork.Unlock();
    InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CWndNetworking::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_NET_MESSAGE)
    {
        GetNetworkInfo();
        return TRUE;
    }
    else
    {
        return CWndInfoEdit::OnUser(hWnd, uiMessage, wParam, lParam);
    }



}