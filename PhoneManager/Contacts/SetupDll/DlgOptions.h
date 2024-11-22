#pragma once

#include "IssWnd.h"
#include "ObjAssociations.h"

class CDlgOptions:public CIssWnd
{
public:
	CDlgOptions(CObjAssociations* objAssociation);
	~CDlgOptions(void);

	void	Init(TCHAR* szInstallDir){m_szDirectory = szInstallDir;};
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:
	void	OnMenuOk();

private:
	HWND	m_hStaticMsg;
	HWND	m_hChkTodaySoftkey;
	HWND	m_hChkReplaceDefault;
	TCHAR*	m_szDirectory;
    CObjAssociations* m_objAssociation;

};
