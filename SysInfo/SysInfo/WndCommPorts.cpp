#include "WndCommPorts.h"
#include "IssRegistry.h"

CWndCommPorts::CWndCommPorts(void)
{
	m_oStr = CIssString::Instance();
}

CWndCommPorts::~CWndCommPorts(void)
{
	DeleteVector();
}

void CWndCommPorts::DeleteVector()
{
	SerialPortType* sPort;

	for( int i = 0; i < m_arrSerialPorts.GetSize(); i++)
	{
		sPort = m_arrSerialPorts[i];
		if(sPort)
			delete sPort;
	}

	m_arrSerialPorts.RemoveAll();
}

BOOL CWndCommPorts::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Comm Ports"));
	Refresh();
	return TRUE;
}

void CWndCommPorts::Refresh()
{
	Clear();
	DeleteVector();
	GetCommPortInfo();
}

void CWndCommPorts::GetCommPortInfo()
{
	EnumeratePorts();//assume the vector is clear
	EnumerateBTPorts();
	//now add everything to our edit control

	
	for(int i = 0; i < m_arrSerialPorts.GetSize(); i++)
	{
		OutputString(m_arrSerialPorts[i]->szPortName, m_arrSerialPorts[i]->szDescription);
	}
}

void CWndCommPorts::EnumeratePorts()
// Enumerates all the serial ports
{
	HKEY	hKey;
	LONG	lRes;

	Clear();

	lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Drivers\\Active"), 0, KEY_READ, &hKey);

	if(lRes == ERROR_SUCCESS)
	{
		DWORD	dwIndex;

		for(dwIndex = 0; ; ++dwIndex)
		{
			TCHAR	szKeyName[128];
			DWORD	dwNameSize	= 128;

			lRes = RegEnumKeyEx(hKey, dwIndex, szKeyName, &dwNameSize, NULL, NULL, NULL, NULL);
			if(lRes == ERROR_SUCCESS)
			{
				HKEY	hKeyDrv;

				lRes = GetActiveDriverKey(szKeyName, &hKeyDrv);
				if(lRes == ERROR_SUCCESS)
				{
					TCHAR szDriverName[STRING_MAX];
					TCHAR szDriverKey[STRING_MAX];
					TCHAR szFriendlyName[STRING_MAX];

					m_oStr->Empty(szDriverName);
					m_oStr->Empty(szDriverKey);
					m_oStr->Empty(szFriendlyName);

					if(GetKeyValue(hKeyDrv, _T("Name"), *szDriverName) == ERROR_SUCCESS)
					{
						TCHAR szName[4];
						m_oStr->StringCopy(szName, szDriverName, 0, 3);

						// Check if this is a COM port
						if(m_oStr->CompareNoCase(szName, _T("COM")) == 0)
						{
							if(GetKeyValue(hKeyDrv, _T("Key"), *szDriverKey) == ERROR_SUCCESS)
							{
								HKEY	hKeyBase;

								lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szDriverKey, 0, KEY_READ, &hKeyBase);
								if(lRes == ERROR_SUCCESS)
								{
									GetKeyValue(hKeyBase, _T("FriendlyName"), *szFriendlyName);
									RegCloseKey(hKeyBase);
								}
							}

							//ok .. we have the info ... lets save it in the vector
							SerialPortType* sPort = new SerialPortType;
							m_oStr->StringCopy(sPort->szPortName, szDriverName);
							m_oStr->StringCopy(sPort->szDescription, szFriendlyName);
							
							m_arrSerialPorts.AddElement(sPort);
						}
					}

					RegCloseKey(hKeyDrv);
				}
			}
			else
				break;
		}

		RegCloseKey(hKey);
	}
}

void CWndCommPorts::EnumerateBTPorts()
{
	LONG	lRes;
	HKEY	hKey;

	TCHAR*	szMsBtPorts = 	_T("Software\\Microsoft\\Bluetooth\\Serial\\Ports");

	lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szMsBtPorts, 0, KEY_READ, &hKey);
	if(lRes == ERROR_SUCCESS)
	{
		DWORD	dwIndex;

		for(dwIndex = 0; ; ++dwIndex)
		{
			TCHAR	szKeyName[128];
			DWORD	dwNameSize	= 128;

			lRes = RegEnumKeyEx(hKey, dwIndex, szKeyName, &dwNameSize, NULL, NULL, NULL, NULL);
			if(lRes == ERROR_SUCCESS)
			{
				//CString	strFullKey(pszMsBtPorts);
				HKEY	hKeyPort;
				TCHAR szFullKey[STRING_MAX];
				m_oStr->StringCopy(szFullKey, szMsBtPorts);
				m_oStr->Concatenate(szFullKey, _T("\\"));
				m_oStr->Concatenate(szFullKey, szKeyName);

				lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szFullKey, 0, KEY_READ, &hKeyPort);
				if(lRes == ERROR_SUCCESS)
				{
					TCHAR szPortName[STRING_LARGE];

					if(GetKeyValue(hKeyPort, _T("Port"), *szPortName) == ERROR_SUCCESS)
					{
						m_oStr->Concatenate(szPortName, _T(":"));

						//ok .. we have the info ... lets save it in the vector
						SerialPortType* sPort = new SerialPortType;
						m_oStr->StringCopy(sPort->szPortName, szPortName);
						m_oStr->StringCopy(sPort->szDescription, _T("Bluetooth"));

						m_arrSerialPorts.AddElement(sPort);
					}
					RegCloseKey(hKeyPort);
				}
			}
			else
				break;
		}

		RegCloseKey(hKey);
	}
}



HRESULT CWndCommPorts::GetActiveDriverKey(TCHAR* szKeyName, HKEY* hKey)
{
	TCHAR szTemp[STRING_MAX];
	m_oStr->Format(szTemp, _T("%s%s"), _T("Drivers\\Active\\"), szKeyName);

	HRESULT res = ERROR_SUCCESS;
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemp, 0, KEY_READ, hKey);

	return res;

}

HRESULT CWndCommPorts::GetKeyValue(HKEY hKey, TCHAR* szValueName, TCHAR& szValue)
{
	LONG	lRes;
	DWORD	dwSize	= 0, dwType;

	lRes = RegQueryValueEx(hKey, szValueName, NULL, &dwType, NULL, &dwSize);
	if(lRes == ERROR_SUCCESS)
	{
		TCHAR*	szTemp = new TCHAR[dwSize / sizeof(TCHAR)];

		if(dwSize > 0)
		{
			lRes = RegQueryValueEx(hKey, szValueName, NULL, &dwType, (BYTE*)szTemp, &dwSize);				

			if(lRes == ERROR_SUCCESS)
				m_oStr->StringCopy(&szValue, szTemp);

			delete [] szTemp;
		}
		else
			lRes = ERROR_NOT_ENOUGH_MEMORY;
	}

	return lRes;
}
