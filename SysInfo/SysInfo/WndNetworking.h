#pragma once
#include "wndinfoedit.h"
#include "ObjNetworkInfo.h"

class CWndNetworking :
	public CWndInfoEdit
{
public:
	CWndNetworking(void);
	~CWndNetworking(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    void		Refresh();

public:
    void        GetNetworkInfo();

private:
    CObjNetworkInfo             m_oNetwork;
};
