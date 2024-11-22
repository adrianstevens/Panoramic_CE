// SetupDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "SetupDll.h"
#include "resource.h"
#include "IssString.h"
#include "IssRegistry.h"
#include "IssKey.h"

HINSTANCE	g_hInst;

#define REG_PanoCalcPro		_T("SOFTWARE\\Pano\\Scientific")


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = (HINSTANCE)hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

SETUP_API codeINSTALL_INIT Install_Init(HWND hwndParent, 
							  BOOL fFirstCall,
							  BOOL fPreviouslyInstalled, 
							  LPCTSTR pszInstallDir)
{
	CIssKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\Panoramic\\PanoScientific"), _T("SOFTWARE\\Pano\\PSC2"), 0x0, 0x4593ACD1, 0xAAABDC44, 4, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	return codeINSTALL_INIT_CONTINUE;
}

SETUP_API codeINSTALL_EXIT Install_Exit(HWND    hwndParent,
							            LPCTSTR pszInstallDir,      // final install directory
							            WORD    cFailedDirs,
							            WORD    cFailedFiles,
							            WORD    cFailedRegKeys,
							            WORD    cFailedRegVals,
							            WORD    cFailedShortcuts)
        {

//    CObjAssociations obj;
//    obj.SetCalcOverride((TCHAR*)pszInstallDir);
	

	return codeINSTALL_EXIT_DONE;
}

SETUP_API codeUNINSTALL_INIT Uninstall_Init(HWND hwndParent,
								            LPCTSTR pszInstallDir)
{
   // restore to the originals if they were set
//    CObjAssociations obj;
//    obj.RestoreCalcOverride();

	return codeUNINSTALL_INIT_CONTINUE;
}

SETUP_API codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
	CIssString* oStr = CIssString::Instance();

    // we should delete the key directories
    TCHAR szMsg[STRING_MAX];
    oStr->StringCopy(szMsg, IDS_MSG_DeleteSettings, STRING_MAX, g_hInst);
    if(IDYES == MessageBox(hwndParent, szMsg, _T("Uninstall"), MB_YESNO))
    {
        RegDeleteKeyNT(HKEY_CURRENT_USER, REG_PanoCalcPro);
    }
    
    return codeUNINSTALL_EXIT_DONE;
}

