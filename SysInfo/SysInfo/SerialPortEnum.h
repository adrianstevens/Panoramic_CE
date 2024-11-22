#pragma once

#include "SerialPortInfo.h"


// CSerialPortEnum
//
//		Enumerates Windows Mobile serial ports 
//		including Microsoft Bluetooth virtual COM port
//
class CSerialPortEnum
{
public:

	CSerialPortEnum(void)
	{
		EnumeratePorts();
	}

	~CSerialPortEnum(void)
	{
		Clear();
	}

	// Gets the number of entries in the list
	size_t GetCount()
	{
		return m_ports.GetCount();
	}

	// Clears the list
	void Clear()
	{
		size_t	i;

		for(i = 0; i < m_ports.GetCount(); ++i)
			delete m_ports[i];
		m_ports.SetCount(0);
	}

	// Gets a CSerialPortInfo at the given position
	const CSerialPortInfo* operator[] (size_t i) const
	{
		return m_ports[i];
	}

	// Enumerates all the serial ports
	size_t EnumeratePorts()
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

					lRes = OpenActiveDriverKey(szKeyName, &hKeyDrv);
					if(lRes == ERROR_SUCCESS)
					{
						CString	strDriverName,
								strDriverKey,
								strFriendlyName;

						if(GetKeyValue(hKeyDrv, _T("Name"), strDriverName) == ERROR_SUCCESS)
						{
							CString	strPrefix(strDriverName.Left(3));

							// Check if this is a COM port
							if(strPrefix.CompareNoCase(_T("COM")) == 0)
							{
								if(GetKeyValue(hKeyDrv, _T("Key"), strDriverKey) == ERROR_SUCCESS)
								{
									HKEY	hKeyBase;

									lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strDriverKey, 0, KEY_READ, &hKeyBase);
									if(lRes == ERROR_SUCCESS)
									{
										GetKeyValue(hKeyBase, _T("FriendlyName"), strFriendlyName);
										RegCloseKey(hKeyBase);
									}
								}

								CSerialPortInfo* pInfo = new CSerialPortInfo(strDriverName, strFriendlyName);

								if(pInfo != NULL)
									m_ports.Add(pInfo);
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

		AddMsBtPorts();

		return m_ports.GetCount();
	}

private:
	CAtlArray<CSerialPortInfo*>	m_ports;

	// Open the given active driver key
	LONG OpenActiveDriverKey(LPCTSTR pszDriver, HKEY *phKey)
	{
		CString	strFullName(_T("Drivers\\Active\\"));
		LONG	lRes;

		strFullName += pszDriver;

		lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strFullName, 0, KEY_READ, phKey);

		return lRes;
	}

	LONG GetKeyValue(HKEY hKey, LPCTSTR pszValueName, CString &strValue)
	{
		LONG	lRes;
		DWORD	dwSize	= 0,
				dwType;

		lRes = RegQueryValueEx(hKey, pszValueName, NULL, &dwType, NULL, &dwSize);
		if(lRes == ERROR_SUCCESS)
		{
			TCHAR*	pszValue = new TCHAR[dwSize / sizeof(TCHAR)];

			if(pszValue != NULL)
			{
				lRes = RegQueryValueEx(hKey, pszValueName, NULL, &dwType, (BYTE*)pszValue, &dwSize);				
				
				if(lRes == ERROR_SUCCESS)
					strValue = pszValue;
				delete [] pszValue;
			}
			else
				lRes = ERROR_NOT_ENOUGH_MEMORY;
		}

		return lRes;
	}

	// Adds any Microsoft Bluetooth Serial Ports
	void AddMsBtPorts()
	{
		LONG	lRes;
		HKEY	hKey;

		LPCTSTR	pszMsBtPorts = 	_T("Software\\Microsoft\\Bluetooth\\Serial\\Ports");

		lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszMsBtPorts, 0, KEY_READ, &hKey);
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
					CString	strFullKey(pszMsBtPorts);
					HKEY	hKeyPort;

					strFullKey += _T("\\");
					strFullKey += szKeyName;
			
					lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strFullKey, 0, KEY_READ, &hKeyPort);
					if(lRes == ERROR_SUCCESS)
					{
						CString	strPortName;

						if(GetKeyValue(hKeyPort, _T("Port"), strPortName) == ERROR_SUCCESS)
						{
							strPortName += _T(":");

							CSerialPortInfo* pInfo = new CSerialPortInfo(strPortName, _T("Bluetooth"));

							if(pInfo != NULL)
								m_ports.Add(pInfo);
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
};
