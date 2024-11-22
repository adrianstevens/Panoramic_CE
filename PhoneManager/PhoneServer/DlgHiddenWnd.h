#pragma once
#include "isswnd.h"
#include "IssCallManager.h"
#include "PoomContacts.h"
#include "DlgConnect.h"

class CDlgHiddenWnd : public CIssWnd
{
public:
	CDlgHiddenWnd(void);
	~CDlgHiddenWnd(void);

	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:
	BOOL	DisplayIncomingCall();

private:
	CIssString*		m_oStr;
	CIssCallManager	m_oCallManager;
};
