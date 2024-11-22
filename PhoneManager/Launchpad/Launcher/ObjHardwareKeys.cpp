#include "ObjHardwareKeys.h"
#include "IssCommon.h"

#define ONE_SHOT_MOD        0x2001
#define VK_TTALK_ID         0x07
#define VK_TEND_ID          0x08
#define VK_TSOFT1_ID        0x09
#define VK_TSOFT2_ID        0x0a


CObjHardwareKeys* CObjHardwareKeys::m_instance  = NULL;

#ifdef UNDER_CE
extern "C" 
{ 
	typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam); 

#ifndef WH_KEYBOARD_LL
#define WH_KEYBOARD_LL   		20
#endif
#define HC_ACTION				0 


	typedef struct tagKBDLLHOOKSTRUCT 
	{ 
		DWORD vkCode;		// virtual key code 
		DWORD scanCode;		// scan code 
		DWORD flags;		// flags 
		DWORD time;			// time stamp for this message 
		DWORD dwExtraInfo;	// extra info from the driver or keybd_event 
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT; 


	HHOOK WINAPI SetWindowsHookExW( int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId ); 
	BOOL WINAPI UnhookWindowsHookEx( HHOOK hhk ); 
	LRESULT WINAPI CallNextHookEx( HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam ); 
	LRESULT CALLBACK KeyboardProc( int code,  WPARAM wParam, LPARAM lParam ); 
#define SetWindowsHookEx  SetWindowsHookExW 
} 
#endif

LRESULT CALLBACK CObjHardwareKeys::KBHook(int code, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam; 
	LRESULT    lResult = 0; 
    BOOL bHandled = FALSE;

	if ((code == HC_ACTION) &&
		(wParam == WM_KEYDOWN || wParam == WM_KEYUP) && 
		m_instance && 
		m_instance->m_hWndNotif) 
	{ 
#ifdef UNDER_CE
		switch(pKeyStruct->vkCode ) 
		{ 
		case VK_THOME:
			SendMessage(m_instance->m_hWndNotif, wParam, VK_THOME, 0);
            bHandled = TRUE;
			break;
		default:
			break; 
		} 
#endif
	} 


	if (0 == lResult && m_instance && !bHandled) 
		lResult = CallNextHookEx(m_instance->m_hKeyboardHook, code,  wParam, lParam); 

	// Return true if hook was handled, false to pass it on 
	return bHandled;//lResult; 
}

CObjHardwareKeys::CObjHardwareKeys(void)
:m_hWndNotif(NULL)
,m_hKeyboardHook(NULL)
{
}

CObjHardwareKeys::~CObjHardwareKeys(void)
{
	if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}
}

/********************************************************************
Function:    Instance

Arguments:                

Returns:    CIssString* - used to pass only one instance of the class 
so everyone will share the same instance

Comments:    Function used to pass back one global instance of this class
*********************************************************************/
CObjHardwareKeys* CObjHardwareKeys::Instance()
{
    if (m_instance == NULL)
    {
        m_instance  = new CObjHardwareKeys();
    }
    return m_instance;
}

/********************************************************************
Function:    DeleteInstance

Arguments:    

Returns:    void

Comments:    Function used to delete the global instance of this class
*********************************************************************/
void CObjHardwareKeys::DeleteInstance()
{
    if (m_instance != NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

BOOL CObjHardwareKeys::RegisterHotKeys(HWND hWndNofit, HINSTANCE hInst)
{
	m_hWndNotif = hWndNofit;

    // turning this all off because handling WM_KEYUP seems to just work
    //return TRUE;

	if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}

   
    m_hKeyboardHook = SetWindowsHookExW( WH_KEYBOARD_LL, 
									(HOOKPROC)KBHook,	// address of hook procedure 
									hInst,			// handle to application instance 
									0 );

    return (m_hKeyboardHook?TRUE:FALSE);
  
}


