#include "IssHardwareKeys.h"
#include "IssCommon.h"

#define ONE_SHOT_MOD        0x2001
#define VK_TTALK_ID         0x07
#define VK_TEND_ID          0x08
#define VK_TSOFT1_ID        0x09
#define VK_TSOFT2_ID        0x0a


CIssHardwareKeys* CIssHardwareKeys::m_instance  = NULL;

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

LRESULT CALLBACK CIssHardwareKeys::KBHook(int code, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam; 
	LRESULT    lResult = 0; 
    BOOL bHandled = FALSE;

	if ((code == HC_ACTION) &&
		(wParam == WM_KEYDOWN || wParam == WM_KEYUP) && 
		m_instance && 
		m_instance->m_hWndNotif && 
		m_instance->m_uiKeyFlag &&
		IsWindow(m_instance->m_hWndNotif) && 
		IsWindowVisible(m_instance->m_hWndNotif) &&
        (GetTickCount()-m_instance->m_dwTickCount) > 3000) 
	{ 
#ifdef UNDER_CE
		//DebugOut(_T("Key: %d"), pKeyStruct->vkCode);
		switch(pKeyStruct->vkCode ) 
		{ 

		case VK_TTALK: 
			if(m_instance->m_uiKeyFlag&KEY_Talk)
            {
                SendMessage(m_instance->m_hWndNotif, wParam/*WM_KEYUP*/, VK_TTALK, 0);
                bHandled = TRUE;
            }
			break;
		case VK_TEND:
			if(m_instance->m_uiKeyFlag&KEY_End)
            {
                SendMessage(m_instance->m_hWndNotif, WM_KEYUP, VK_TEND, 0);
                bHandled = TRUE;
            }
			break;
		case VK_TSOFT1:
			if(m_instance->m_uiKeyFlag&KEY_Soft1)
            {
                SendMessage(m_instance->m_hWndNotif, WM_KEYUP, VK_TSOFT1, 0);
                bHandled = TRUE;
            }
			break;
		case VK_TSOFT2:
			if(m_instance->m_uiKeyFlag&KEY_Soft2)
            {
                SendMessage(m_instance->m_hWndNotif, WM_KEYUP, VK_TSOFT2, 0);
                bHandled = TRUE;
            }
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

CIssHardwareKeys::CIssHardwareKeys(void)
:m_hCoreDll(NULL)
,m_procUndergisterFunc(NULL)
,m_hWndNotif(NULL)
,m_uiKeyFlag(0)
,m_hKeyboardHook(NULL)
,m_eHMethod(HANDLE_KHook)
{
}

CIssHardwareKeys::~CIssHardwareKeys(void)
{
	if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}

	if (m_hCoreDll)
	{
		::FreeLibrary(m_hCoreDll);
		m_hCoreDll = NULL;
	}
	m_procUndergisterFunc = NULL;
}

/********************************************************************
Function:    Instance

Arguments:                

Returns:    CIssString* - used to pass only one instance of the class 
so everyone will share the same instance

Comments:    Function used to pass back one global instance of this class
*********************************************************************/
CIssHardwareKeys* CIssHardwareKeys::Instance()
{
    if (m_instance == NULL)
    {
        m_instance  = new CIssHardwareKeys();
    }
    return m_instance;
}

/********************************************************************
Function:    DeleteInstance

Arguments:    

Returns:    void

Comments:    Function used to delete the global instance of this class
*********************************************************************/
void CIssHardwareKeys::DeleteInstance()
{
    if (m_instance != NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

BOOL CIssHardwareKeys::RegisterHotKeys(HWND hWnd, HINSTANCE hInst, UINT uiKeyFlag, EnumHandleMethod eMethod)
{
	m_hWndNotif = hWnd;
	m_uiKeyFlag = uiKeyFlag;

    // turning this all off because handling WM_KEYUP seems to just work
    //return TRUE;

	if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}

    m_eHMethod = eMethod;

    if(m_eHMethod == HANDLE_OnKeyUp)
    {
        return TRUE;
    }
    else if(m_eHMethod == HANDLE_KHook)
    {
	    m_hKeyboardHook = SetWindowsHookExW( WH_KEYBOARD_LL, 
										(HOOKPROC)KBHook,	// address of hook procedure 
										hInst,			// handle to application instance 
										0 );

        return (m_hKeyboardHook?TRUE:FALSE);
    }
    else
#ifdef UNDER_CE
    {   //lets register some keys
        if(uiKeyFlag & VK_TTALK)
            ::RegisterHotKey(hWnd, VK_TTALK_ID, ONE_SHOT_MOD, VK_TTALK);
        if(uiKeyFlag & VK_TEND)
            ::RegisterHotKey(hWnd, VK_TEND_ID, ONE_SHOT_MOD, VK_TEND);
        if(uiKeyFlag & VK_TSOFT1)
            ::RegisterHotKey(hWnd, VK_TSOFT1_ID, ONE_SHOT_MOD, VK_TSOFT1);
        if(uiKeyFlag & VK_TSOFT2)
            ::RegisterHotKey(hWnd, VK_TSOFT2_ID, ONE_SHOT_MOD, VK_TSOFT2);
    }
    m_dwTickCount = GetTickCount();
#endif
    return TRUE;
}


//only for the one shot method
BOOL CIssHardwareKeys::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_eHMethod != HANDLE_Message)
        return UNHANDLED;

    switch(lParam >> 16)
    {
#ifdef UNDER_CE
    case VK_TTALK:
        ::RegisterHotKey(hWnd, VK_TTALK_ID, ONE_SHOT_MOD, VK_TTALK);
    	break;
    case VK_TEND:
        ::RegisterHotKey(hWnd, VK_TEND_ID, ONE_SHOT_MOD, VK_TEND);
    	break;
    case VK_TSOFT1:
        ::RegisterHotKey(hWnd, VK_TSOFT1_ID, ONE_SHOT_MOD, VK_TSOFT1);
        break;
    case VK_TSOFT2:
        ::RegisterHotKey(hWnd, VK_TSOFT2_ID, ONE_SHOT_MOD, VK_TSOFT2);
        break;
#endif
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;

}


