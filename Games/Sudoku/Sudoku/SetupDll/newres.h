#ifndef __NEWRES_H__
#define __NEWRES_H__

#define  SHMENUBAR RCDATA
#if !(defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300))
	#undef HDS_HORZ  
	#undef HDS_BUTTONS 
	#undef HDS_HIDDEN 

	#include <commctrl.h>
	// for MenuBar
	#define I_IMAGENONE		(-2)
	#define NOMENU			0xFFFF
	#define IDS_SHNEW		1
	#define IDM_SHAREDNEW        10
	#define IDM_SHAREDNEWDEFAULT 11

	// for Tab Control
	#define TCS_SCROLLOPPOSITE      0x0001   // assumes multiline tab
	#define TCS_BOTTOM              0x0002
	#define TCS_RIGHT               0x0002
	#define TCS_VERTICAL            0x0080
	#define TCS_MULTISELECT         0x0004  // allow multi-select in button mode
	#define TCS_FLATBUTTONS         0x0008	
#else
	#if !defined(WCEOLE_ENABLE_DIALOGEX)
		#define DIALOGEX DIALOG DISCARDABLE
	#endif
	#include <commctrl.h>
	#define  SHMENUBAR RCDATA
	#if defined(WIN32_PLATFORM_PSPC) && (_WIN32_WCE >= 300)
		#include <aygshell.h>
		#define AFXCE_IDR_SCRATCH_SHMENU  28700
	#else
		#define I_IMAGENONE		(-2)
		#define NOMENU			0xFFFF
		#define IDS_SHNEW		1

		#define IDM_SHAREDNEW        10
		#define IDM_SHAREDNEWDEFAULT 11
	#endif // _WIN32_WCE_PSPC
	#define AFXCE_IDD_SAVEMODIFIEDDLG 28701
#endif //_WIN32_WCE_PSPC
#ifdef RC_INVOKED
#ifndef _INC_WINDOWS
#define _INC_WINDOWS
	#include "winuser.h"           // extract from windows header
	#include "winver.h"   
#endif
#endif


#endif //__NEWRES_H__
