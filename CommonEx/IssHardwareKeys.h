#pragma once
#include "stdafx.h"

typedef BOOL (__stdcall *UNREGISTERFUNC1PROC)( UINT, UINT );

#define KEY_Talk		0x001
#define KEY_End			0x001 << 1
#define KEY_Soft1		0x001 << 2
#define KEY_Soft2		0x001 << 3
#define KEY_All			KEY_Talk | KEY_End | KEY_Soft1 | KEY_Soft2

enum EnumHandleMethod
{
    HANDLE_KHook,
    HANDLE_OnKeyUp,
    HANDLE_Message,
};

class CIssHardwareKeys
{
public:
	CIssHardwareKeys(void);
	~CIssHardwareKeys(void);
    static CIssHardwareKeys* Instance ();
    static void DeleteInstance  ();

	BOOL				RegisterHotKeys(HWND hWnd, HINSTANCE hInst, UINT uiKeyFlag, EnumHandleMethod eMethod = HANDLE_KHook);
    BOOL                OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam); //so we can reset the onshot mod


private:	// functions
	static LRESULT CALLBACK	KBHook(int code, WPARAM wParam, LPARAM lParam);
private:	// variables
    static CIssHardwareKeys*   m_instance;
	HINSTANCE			m_hCoreDll;
	UNREGISTERFUNC1PROC m_procUndergisterFunc;

    EnumHandleMethod    m_eHMethod;

    HHOOK               m_hKeyboardHook; 
	HWND				m_hWndNotif;
	UINT				m_uiKeyFlag;
    DWORD               m_dwTickCount;
};
