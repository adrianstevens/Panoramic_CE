#pragma once

#include "IssString.h"
#include "IssVector.h"
#include "stdafx.h"

struct TypeAppAssociation
{
	TCHAR*			szExtension;
	TCHAR*			szDescription;
	TCHAR*			szCommand;
	TCHAR*			szIconPath;
	TCHAR*			szDefaultDescription;
	TCHAR*			szDefaultCommand;
	TCHAR*			szDefaultIconPath;
	TCHAR*			szFileName;
};

struct TypeLink
{
	TCHAR*	szName;				// friendly name of the app
	TCHAR*	szPath;				// path of the found .lnk file
	TCHAR*	szExeName;			// exe that this link is associated with
	int		iIconIndex;			// global icon index that is used
};

class CObjFileAssociation
{
public:
	CObjFileAssociation(void);
	~CObjFileAssociation(void);

	void	Destroy();
	void	InitAppAssociation();
	void	FlushIconCache();
	void	RestoreExtension(TCHAR* szExt);
	BOOL	SetExtension(TCHAR* szExt, TCHAR* szDescription, TCHAR* szIconPath, TCHAR* szCommand);
	BOOL	FindExtensionInfo(	TCHAR* szExt, 
								TCHAR* szDescription, 
								TCHAR* szIconPath, 
								TCHAR* szCommand,
								TCHAR* szDefaultDescription,
								TCHAR* szDefaultIconPath,
								TCHAR* szDefaultCommand);
	int		FindSimilarFileName(TCHAR* szFileName);
	TCHAR*	ParseFileFromCommandString(TCHAR* szString);
	BOOL	MyDrawIcon(BOOL bLargeIcon, HDC hDC, TCHAR* szIconPath, TCHAR* szCommand, POINT ptLocation, HIMAGELIST hImg);
	int		GetSize(){return m_arrApps.GetSize();}
	TypeAppAssociation* GetApp(int iIndex){return m_arrApps[iIndex];}
	static BOOL	IsFileAssociationValid(TCHAR* szFileName);
	static void	RebootDevice();

private:
	
	void	BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
	BOOL	RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
	BOOL	GetKeyName(TCHAR* szExt, TCHAR* szKeyName);
	HKEY	GetDefaultIconKey(TCHAR* szKeyName);
	HKEY	GetShellCommandKey(TCHAR* szKeyName);
	HKEY	GetDescriptionKey(TCHAR* szKeyName);
	HKEY	GetExtKey(TCHAR* szExt);
	


private:
	CIssString* m_oStr;
	CIssVector<TypeAppAssociation> m_arrApps;
};

class CObjFindLinks
{
public:
	CObjFindLinks();
	~CObjFindLinks();

	void	Destroy();
	void	Initialize();
	int		FindLinkIndex(TCHAR* szAppName);
	TypeLink* FindLink(TCHAR* szAppName);
	int		GetSize(){return m_arrLinks.GetSize();}
	TypeLink* GetLink(int iIndex){return m_arrLinks[iIndex];}

private:	// functions
	void	SearchLinks(TCHAR* szPath);

private:	// variables
	CIssString*	m_oStr;
	CIssVector<TypeLink> m_arrLinks;
};
