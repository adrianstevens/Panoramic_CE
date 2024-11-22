#include "ObjFileAssociation.h"
#include "IssRegistry.h"
#include "stdafx.h"
#include "path.h"
#include <uniqueid.h>	// for the reboot stuff

#define TXT_Backup	_T("iSSBackup")

#define IOCTL_HAL_REBOOT CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

CObjFileAssociation::CObjFileAssociation(void)
:m_oStr(CIssString::Instance())
{
}

CObjFileAssociation::~CObjFileAssociation(void)
{
	Destroy();
}

void CObjFileAssociation::Destroy()
{
	for(int i=0; i<m_arrApps.GetSize(); i++)
	{
		TypeAppAssociation* sApp = m_arrApps[i];
		if(sApp)
		{
			m_oStr->Delete(&sApp->szExtension);
			m_oStr->Delete(&sApp->szFileName);
			m_oStr->Delete(&sApp->szDescription);
			m_oStr->Delete(&sApp->szCommand);
			m_oStr->Delete(&sApp->szIconPath);
			m_oStr->Delete(&sApp->szDefaultDescription);
			m_oStr->Delete(&sApp->szDefaultCommand);
			m_oStr->Delete(&sApp->szDefaultIconPath);
			delete sApp;
		}
	}
	m_arrApps.RemoveAll();
}

void CObjFileAssociation::RebootDevice()
{
	KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
}

typedef int (*SHFLUSHCACHE)(BOOL);

void CObjFileAssociation::FlushIconCache()
{
	// we'll do a soft reset
	RebootDevice();

	return;  	
	
	// this doesn't work right now, I get a data abort

	HMODULE hCache = LoadLibrary(_T("ceshell.dll"));
	if(hCache)
	{
		SHFLUSHCACHE pShFlushCache = (SHFLUSHCACHE)GetProcAddress(hCache, _T("SHFlushCache")); 
		if(pShFlushCache)
			pShFlushCache(TRUE);
		FreeLibrary(hCache);
	}
}

BOOL CObjFileAssociation::MyDrawIcon(BOOL bLargeIcon, HDC hDC, TCHAR* szIconPath, TCHAR* szCommand, POINT ptLocation, HIMAGELIST hImg)
{
	if(m_oStr->IsEmpty(szIconPath))
		return FALSE;

	TCHAR szTempPath[MAX_PATH];
	m_oStr->StringCopy(szTempPath, szIconPath);
	LPWSTR pwsz = wcschr(szTempPath, L',');
	if (pwsz)
	{
		*pwsz = L'\0';
		int iIndex = _ttol(++pwsz);
		if(0 == m_oStr->CompareNoCase(pwsz, _T("ceshell.dll")))
		{
			if (hImg)
			{
				ImageList_Draw(hImg, iIndex, hDC, ptLocation.x, ptLocation.y, ILD_TRANSPARENT);
				return TRUE;
			}
		}
		else
		{
			// if the path is _not_ ceshell.dll
			HICON hLarge = NULL;
			HICON hSmall = NULL;
			PathRemoveQuotesAndArgs(szTempPath);
			if(::ExtractIconEx(szTempPath, iIndex, &hLarge, &hSmall, 1) && hLarge && hSmall)
			{
				::DrawIcon(hDC, ptLocation.x, ptLocation.y, (bLargeIcon?hLarge:hSmall));
				DestroyIcon(hLarge);
				DestroyIcon(hSmall);
				return TRUE;
			}
			else
			{
				// ExtractIconEx didn't work so let's see if we can do it manually
				m_oStr->StringCopy(szTempPath, szCommand);
				PathRemoveQuotesAndArgs(szTempPath);
				if(::ExtractIconEx(szTempPath, 0, &hLarge, &hSmall, 1) && hLarge && hSmall)
				{
					::DrawIcon(hDC, ptLocation.x, ptLocation.y, (bLargeIcon?hLarge:hSmall));
					DestroyIcon(hLarge);
					DestroyIcon(hSmall);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CObjFileAssociation::IsFileAssociationValid(TCHAR* szFileName)
{
	CIssString* oStr = CIssString::Instance();
	TCHAR* szExt = PathFindExtension(szFileName);
	if(oStr->IsEmpty(szExt))
		return FALSE;

	if(szExt[0] == _T('.'))
		szExt++;

	// our banned file association list
	if(oStr->CompareNoCase(szExt, _T("exe")) == 0 ||
	   oStr->CompareNoCase(szExt, _T("cab")) == 0 ||
	   oStr->CompareNoCase(szExt, _T("cpl")) == 0)
		return FALSE;

	return TRUE;
}

void CObjFileAssociation::InitAppAssociation()
{
	// start with a new list
	Destroy();

	//TCHAR szTemp[MAX_PATH];
	TCHAR szSubKey[MAX_PATH];
	TCHAR szExt[STRING_LARGE];
	TCHAR szCommand[MAX_PATH];
	TCHAR szDescription[MAX_PATH];
	TCHAR szIconPath[MAX_PATH];
	TCHAR szDefaultCommand[MAX_PATH];
	TCHAR szDefaultDescription[MAX_PATH];
	TCHAR szDefaultIconPath[MAX_PATH];
	DWORD dwDataSize = MAX_PATH;
	int iCount = 0;

	while(TRUE)
	{
		dwDataSize = MAX_PATH*sizeof(TCHAR);
		if(ERROR_SUCCESS != RegEnumKeyEx(HKEY_CLASSES_ROOT, iCount++, szSubKey, &dwDataSize, NULL, NULL, NULL, NULL))
			break;

		// we only want keys that start with a '.'
		if(szSubKey[0] != _T('.') || m_oStr->GetLength(szSubKey)<2)
			continue;

		// copy the extension name
		m_oStr->StringCopy(szExt, szSubKey, 1, m_oStr->GetLength(szSubKey)-1);

		// here is where we'll skip some extensions we don't want to look at
		if(m_oStr->CompareNoCase(szExt, _T("exe")) == 0 ||
		   m_oStr->CompareNoCase(szExt, _T("cab")) == 0 ||
		   m_oStr->CompareNoCase(szExt, _T("cpl")) == 0)
			continue;

		if(FindExtensionInfo(szExt, szDescription, szIconPath, szCommand, szDefaultDescription, szDefaultIconPath, szDefaultCommand))
		{
			// Add it to our Vector
			TypeAppAssociation* sApp = new TypeAppAssociation;
			ZeroMemory(sApp, sizeof(TypeAppAssociation));

			sApp->szExtension				= m_oStr->CreateAndCopy(szExt);
			if(!m_oStr->IsEmpty(szDescription))
				sApp->szDescription			= m_oStr->CreateAndCopy(szDescription);
			if(!m_oStr->IsEmpty(szIconPath))
				sApp->szIconPath			= m_oStr->CreateAndCopy(szIconPath);
			if(!m_oStr->IsEmpty(szCommand))
				sApp->szCommand				= m_oStr->CreateAndCopy(szCommand);
			if(!m_oStr->IsEmpty(szDefaultDescription))
				sApp->szDefaultDescription	= m_oStr->CreateAndCopy(szDefaultDescription);
			if(!m_oStr->IsEmpty(szDefaultIconPath))
				sApp->szDefaultIconPath		= m_oStr->CreateAndCopy(szDefaultIconPath);
			if(!m_oStr->IsEmpty(szDefaultCommand))
				sApp->szDefaultCommand		= m_oStr->CreateAndCopy(szDefaultCommand);

			TCHAR* szFileName	= ParseFileFromCommandString(szCommand);

			if(!m_oStr->IsEmpty(szFileName))
				sApp->szFileName	= m_oStr->CreateAndCopy(szFileName);

			m_oStr->Delete(&szFileName);
	
			m_arrApps.AddElement(sApp);
		}

	}

}

int CObjFileAssociation::FindSimilarFileName(TCHAR* szFileName)
{
	if(m_oStr->IsEmpty(szFileName))
		return -1;

	for(int i=0; i<m_arrApps.GetSize(); i++)
	{
		TypeAppAssociation* sApp = m_arrApps[i];
		if(sApp && !m_oStr->IsEmpty(sApp->szFileName) && 0 == m_oStr->CompareNoCase(sApp->szFileName, szFileName))
			return i;
	}
	return -1;
}

TCHAR* CObjFileAssociation::ParseFileFromCommandString(TCHAR* szString)
{
	if(m_oStr->IsEmpty(szString))
		return NULL;

	TCHAR* szTemp = m_oStr->CreateAndCopy(szString);
	TCHAR* p = NULL;

	// if we have a comma we're trying to separate then do so
	if(-1 == m_oStr->Find(szTemp, _T(",")))
	{
		if (p = _tcsstr(szTemp, _T("%1")))
		{
			// find the first space before %1, since the parameter
			// format maybe file:%1  or "%1"
			for (p--; *p != _T(' '); p--)
				;
			*p = _T('\0');
		}	
	}
	else
	{
		if (p = _tcsstr(szTemp, _T(",")))
			*p = _T('\0');

	}

	// remove the space at the tail
	for (DWORD dwDataSize = _tcslen(szTemp) - 1; dwDataSize >= 0 && szTemp[dwDataSize] == _T(' '); dwDataSize--)
		szTemp[dwDataSize] = _T('\0');

	// remove any quotes so we're just dealing with the filename
	PathRemoveQuotesAndArgs(szTemp);

	return szTemp;
}

BOOL CObjFileAssociation::GetKeyName(TCHAR* szExt, TCHAR* szKeyName)
{
	HKEY hKey = GetExtKey(szExt);
	if(!hKey)
		return FALSE;

	DWORD dwType		= REG_SZ;
	DWORD dwDataSize	= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szKeyName, sizeof(TCHAR)*MAX_PATH);
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szKeyName, &dwDataSize) || dwDataSize == 0)
	{
		// there is currently no association with this file type
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);
	return TRUE;
}

HKEY CObjFileAssociation::GetExtKey(TCHAR* szExt)
{
	TCHAR	szTemp[MAX_PATH] = _T("");

	m_oStr->StringCopy(szTemp, szExt);
	if(szTemp[0] != _T('.'))
		m_oStr->Insert(szTemp, _T("."));

	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, szTemp, NULL, NULL, &hKey))
		return NULL;	// this really shouldn't happen
	return hKey;
}

HKEY CObjFileAssociation::GetDefaultIconKey(TCHAR* szKeyName)
{
	TCHAR	szTemp[MAX_PATH];
	HKEY	hKey;
	m_oStr->StringCopy(szTemp, szKeyName);
	m_oStr->Concatenate(szTemp, _T("\\DefaultIcon"));

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, szTemp, NULL, NULL, &hKey))
		return hKey;
	else
		return NULL;
}

HKEY CObjFileAssociation::GetShellCommandKey(TCHAR* szKeyName)
{
	TCHAR	szTemp[MAX_PATH];
	HKEY	hKey;
	m_oStr->StringCopy(szTemp, szKeyName);
	m_oStr->Concatenate(szTemp, _T("\\Shell\\Open\\Command"));

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, szTemp, NULL, NULL, &hKey))
		return hKey;
	else
		return NULL;
}

HKEY CObjFileAssociation::GetDescriptionKey(TCHAR* szKeyName)
{
	HKEY hKey;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, szKeyName, NULL, NULL, &hKey))
		return hKey;
	else
		return NULL;
}

BOOL CObjFileAssociation::FindExtensionInfo(TCHAR* szExt, 
											TCHAR* szDescription, 
											TCHAR* szIconPath, 
											TCHAR* szCommand,
											TCHAR* szDefaultDescription,
											TCHAR* szDefaultIconPath,
											TCHAR* szDefaultCommand)
{
	if(m_oStr->IsEmpty(szExt) || 
	   !szDescription || 
	   !szIconPath || 
	   !szCommand ||
	   !szDefaultCommand ||
	   !szDefaultIconPath ||
	   !szDefaultDescription)
		return FALSE;

	m_oStr->Empty(szDescription);
	m_oStr->Empty(szIconPath);
	m_oStr->Empty(szCommand);
	m_oStr->Empty(szDefaultCommand);
	m_oStr->Empty(szDefaultDescription);
	m_oStr->Empty(szDefaultIconPath);

	TCHAR	szTemp[MAX_PATH];
	// find out the key this extension is associated with
	if(!GetKeyName(szExt, szTemp))
		return FALSE;

	HKEY hKey = GetDescriptionKey(szTemp);
	if(!hKey)
		return FALSE;	// this really shouldn't happen

	// read in the description
	DWORD dwType		= REG_SZ;
	DWORD dwDataSize	= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szDescription, sizeof(TCHAR)*MAX_PATH);
	RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szDescription, &dwDataSize);	// not too important if this fails

	// try and read the default description
	dwType		= REG_SZ;
	dwDataSize	= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szDefaultDescription, sizeof(TCHAR)*MAX_PATH);
	RegQueryValueEx(hKey, TXT_Backup, NULL, &dwType, (LPBYTE)szDefaultDescription, &dwDataSize);	// not too important if this fails
	
	RegCloseKey(hKey);

	hKey = GetShellCommandKey(szTemp);
	if(!hKey)
		return FALSE;

	dwType		= REG_SZ;
	dwDataSize	= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szCommand, sizeof(TCHAR)*MAX_PATH);
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szCommand, &dwDataSize) || dwDataSize == 0)
	{
		// we don't know how to launch this extension
		RegCloseKey(hKey);
		return FALSE;
	}

	// try and read the default command
	dwType		= REG_SZ;
	dwDataSize	= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szDefaultCommand, sizeof(TCHAR)*MAX_PATH);
	RegQueryValueEx(hKey, TXT_Backup, NULL, &dwType, (LPBYTE)szDefaultCommand, &dwDataSize);	// not too important if this fails

	RegCloseKey(hKey);

	hKey = GetDefaultIconKey(szTemp);
	// this is an optional part right now
	if(hKey)
	{
		dwType		= REG_SZ;
		dwDataSize	= MAX_PATH;
		ZeroMemory(szIconPath, sizeof(TCHAR)*MAX_PATH);
		RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szIconPath, &dwDataSize);

		// try and read the default iconpath
		dwType		= REG_SZ;
		dwDataSize	= MAX_PATH*sizeof(TCHAR);
		ZeroMemory(szDefaultIconPath, sizeof(TCHAR)*MAX_PATH);
		RegQueryValueEx(hKey, TXT_Backup, NULL, &dwType, (LPBYTE)szDefaultIconPath, &dwDataSize);	// not too important if this fails

		RegCloseKey(hKey);
	}
	return TRUE;
}

void CObjFileAssociation::BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
	if(!hKey || m_oStr->IsEmpty(szDestination))
		return;

	// first we check if there is already a backup because we don't want to do it twice
	DWORD dwType	= REG_SZ;
	DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
	TCHAR szTemp[MAX_PATH];
	ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
	if(ERROR_SUCCESS == RegQueryValueEx(hKey, szDestination, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
		return;

	// read in the data
	dwType	= REG_SZ;
	dwDataSize= MAX_PATH*sizeof(TCHAR);
	ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSource, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize) || dwDataSize == 0)
		return;

	// set the backup data
	RegSetValueEx(hKey, szDestination, NULL, dwType, (LPBYTE)szTemp, dwDataSize);
	RegFlushKey(hKey);
}

BOOL CObjFileAssociation::RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
	// read in the backed up value
	DWORD dwType	= REG_SZ;
	DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
	TCHAR szTemp[MAX_PATH];
	ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSource, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
		return FALSE; 

	// restore to original location
	RegSetValueEx(hKey, szDestination, NULL, dwType, (LPBYTE)szTemp, dwDataSize);

	// delete the backup copy
	RegDeleteValue(hKey, szSource);

	// flush it out
	RegFlushKey(hKey);

	return TRUE;
}

void CObjFileAssociation::RestoreExtension(TCHAR* szExt)
{
	TCHAR szKeyName[MAX_PATH];
	if(!GetKeyName(szExt, szKeyName))
		return;

	// go through and delete this whole key structure
	if(0 == m_oStr->Find(szKeyName, _T("iSS")))
		CIssRegistry::RegDeleteKeyNT(HKEY_CLASSES_ROOT, szKeyName);

	HKEY hKey = GetExtKey(szExt);
	if(!hKey)
		return;

	// try and restore the value with what we have backed up and if there is
	// nothing then just delete this extension altogether
	if(!RestoreValue(hKey, TXT_Backup, NULL))
	{
		RegCloseKey(hKey);
		m_oStr->StringCopy(szKeyName, szExt);
		m_oStr->Insert(szKeyName, _T("."));
		CIssRegistry::RegDeleteKeyNT(HKEY_CLASSES_ROOT, szKeyName);
	}
	else
		RegCloseKey(hKey);


	return;

	
}

//////////////////////////////////////////////////////////////////////////
// Example
// szExt	- mp3
// szDescription - MP3 Format Sound
// szIconPath - wmplayer.exe,-2001
// szCommand - "wmplayer.exe" "%1"
//////////////////////////////////////////////////////////////////////////
BOOL CObjFileAssociation::SetExtension(TCHAR* szExt, TCHAR* szDescription, TCHAR* szIconPath, TCHAR* szCommand)
{
	TCHAR szKeyName[MAX_PATH];
	TCHAR szTemp[MAX_PATH];
	HKEY hKey;
	DWORD dwDisposition;
	DWORD dwSize;

	hKey = GetExtKey(szExt);
	if(!hKey)
	{
		// there is no previous association so we're going to have to make it
		m_oStr->StringCopy(szKeyName, _T("."));
		m_oStr->Concatenate(szKeyName, szExt);

		if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT, szKeyName, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
			return FALSE;
	}
	else
	{
		BackupValue(hKey, NULL, TXT_Backup);
	}
	

	m_oStr->StringCopy(szKeyName, _T("iSS"));
	m_oStr->Concatenate(szKeyName, szExt);

	dwSize = (m_oStr->GetLength(szKeyName)+1)*sizeof(TCHAR);
	if(ERROR_SUCCESS != RegSetValueEx(hKey, NULL, NULL, REG_SZ, (LPBYTE)szKeyName, dwSize))
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	RegFlushKey(hKey);
	RegCloseKey(hKey);


	// set the new description
	if(!m_oStr->IsEmpty(szDescription))
	{
		hKey = GetDescriptionKey(szKeyName);
		if(!hKey)
		{
			// Description key defined so we'll have to make it
			if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT, szKeyName, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
				return FALSE;
		}
		//else
		//	BackupValue(hKey, NULL, TXT_Backup);
		dwSize = (m_oStr->GetLength(szDescription)+1)*sizeof(TCHAR);
		if(ERROR_SUCCESS != RegSetValueEx(hKey, NULL, NULL, REG_SZ, (LPBYTE)szDescription, dwSize))
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}

	// set the new icon path
	if(!m_oStr->IsEmpty(szIconPath))
	{
		hKey = GetDefaultIconKey(szKeyName);
		if(!hKey)
		{
			m_oStr->StringCopy(szTemp, szKeyName);
			m_oStr->Concatenate(szTemp, _T("\\DefaultIcon"));
			// Icon path was not defined so we'll have to make it
			if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT, szTemp, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
				return FALSE;
		}
		//else
		//	BackupValue(hKey, NULL, TXT_Backup);
		dwSize = (m_oStr->GetLength(szIconPath)+1)*sizeof(TCHAR);
		if(ERROR_SUCCESS != RegSetValueEx(hKey, NULL, NULL, REG_SZ, (LPBYTE)szIconPath, dwSize))
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}

	// set the new command string
	if(!m_oStr->IsEmpty(szCommand))
	{
		hKey = GetShellCommandKey(szKeyName);
		if(!hKey)
		{
			m_oStr->StringCopy(szTemp, szKeyName);
			m_oStr->Concatenate(szTemp, _T("\\Shell\\Open\\Command"));

			// command string not defined so we'll have to make it
			if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT, szTemp, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
				return FALSE;
		}
		//else
		//	BackupValue(hKey, NULL, TXT_Backup);
		dwSize = (m_oStr->GetLength(szCommand)+1)*sizeof(TCHAR);
		if(ERROR_SUCCESS != RegSetValueEx(hKey, NULL, NULL, REG_SZ, (LPBYTE)szCommand, dwSize))
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}


	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
// CObjFindLinks Class
//////////////////////////////////////////////////////////////////////////

CObjFindLinks::CObjFindLinks()
:m_oStr(CIssString::Instance())
{

}

CObjFindLinks::~CObjFindLinks()
{
	Destroy();
}

void CObjFindLinks::Destroy()
{
	for(int i=0; i<m_arrLinks.GetSize(); i++)
	{
		TypeLink* sLink = m_arrLinks[i];
		if(sLink)
		{
			m_oStr->Delete(&sLink->szName);
			m_oStr->Delete(&sLink->szPath);
			m_oStr->Delete(&sLink->szExeName);
			delete sLink;
		}
	}
	m_arrLinks.RemoveAll();
}

void CObjFindLinks::Initialize()
{
	// clear out the old
	Destroy();

	TCHAR szStartMenu[MAX_PATH] = _T("");

	SHGetSpecialFolderPath(NULL, szStartMenu, CSIDL_STARTMENU, FALSE);

	// recursively search the start menu
	SearchLinks(szStartMenu);

}

void CObjFindLinks::SearchLinks(TCHAR* szPath)
{
	WIN32_FIND_DATA wfd;

	TCHAR szSearch[MAX_PATH];

	m_oStr->StringCopy(szSearch, szPath);
	if(szSearch[m_oStr->GetLength(szSearch)-1] != _T('\\'))
		m_oStr->Concatenate(szSearch, _T("\\"));
	m_oStr->Concatenate(szSearch, _T("*.*"));

	HANDLE hFind = FindFirstFile(szSearch, &wfd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// get the full path to the file or folder
			m_oStr->StringCopy(szSearch, szPath);
			if(m_oStr->Compare(szPath, _T("\\"))!=0)
				m_oStr->Concatenate(szSearch, _T("\\"));
			m_oStr->Concatenate(szSearch, wfd.cFileName);

			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				// recursively search down
				SearchLinks(szSearch);
			else
			{
				// we're only looking for .lnk files
				int iIndex = m_oStr->Find(szSearch, _T(".lnk"));
				if(iIndex == -1)
					continue;

				// special case
				if(m_oStr->Compare(wfd.cFileName, _T("icon.lnk"))==0)
					continue;

				TCHAR szFriendlyName[MAX_PATH];
				TCHAR szFileName[MAX_PATH];
				// get the friendly name
				m_oStr->StringCopy(szFriendlyName, wfd.cFileName);
				iIndex		= m_oStr->Find(szFriendlyName, _T(".lnk"));
				int iLen	= m_oStr->GetLength(szFriendlyName);
				if(iIndex != -1)
					m_oStr->Delete(iIndex, 4, szFriendlyName);

				if(!SHGetShortcutTarget(szSearch, szFileName, MAX_PATH))
					continue;

				PathRemoveQuotesAndArgs(szFileName);

				TypeLink* sLink = new TypeLink;
				ZeroMemory(sLink, sizeof(TypeLink));

				SHFILEINFO sfi = {0};
				// get the icon index
				if (SHGetFileInfo(szSearch, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
					sLink->iIconIndex = sfi.iIcon;

				sLink->szName	= m_oStr->CreateAndCopy(szFriendlyName);
				sLink->szPath	= m_oStr->CreateAndCopy(szSearch);
				sLink->szExeName= m_oStr->CreateAndCopy(szFileName);

				// add to our vector
				m_arrLinks.AddElement(sLink);

			}
		} while(FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}
}

TypeLink* CObjFindLinks::FindLink(TCHAR* szAppName)
{
	int iIndex = FindLinkIndex(szAppName);
	if(iIndex != -1)
		return m_arrLinks[iIndex];
	else
		return NULL;
}

int	CObjFindLinks::FindLinkIndex(TCHAR* szAppName)
{
	if(m_oStr->IsEmpty(szAppName))
		return -1;

	for(int i=0; i<m_arrLinks.GetSize(); i++)
	{
		TypeLink* sLink = m_arrLinks[i];
		if(sLink && sLink->szExeName)
		{
			if(m_oStr->FindNoCase(sLink->szExeName, szAppName) >= 0)
				return i;
		}
	}
	return -1;
}

