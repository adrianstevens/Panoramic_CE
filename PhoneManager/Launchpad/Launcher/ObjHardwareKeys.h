#pragma once
#include "stdafx.h"

class CObjHardwareKeys
{
public:
	CObjHardwareKeys(void);
	~CObjHardwareKeys(void);
    static CObjHardwareKeys* Instance ();
    static void DeleteInstance  ();

	BOOL				RegisterHotKeys(HWND hWndNotif, HINSTANCE hInst);

private:	// functions
	static LRESULT CALLBACK	KBHook(int code, WPARAM wParam, LPARAM lParam);
private:	// variables
    static CObjHardwareKeys*   m_instance;
    HHOOK               m_hKeyboardHook; 
	HWND				m_hWndNotif;
    DWORD               m_dwTickCount;
};
