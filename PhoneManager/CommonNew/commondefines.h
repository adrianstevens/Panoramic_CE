// CommonDefines.h: defines common to all PhoneManager apps
//
//////////////////////////////////////////////////////////////////////

#pragma once

// notifications
#define WM_NOTIFY_BASE				WM_USER + 500
#define WM_NOTIFY_INCOMING			WM_NOTIFY_BASE 
#define WM_NOTIFY_CALLERNAME		WM_NOTIFY_BASE + 1
#define WM_NOTIFY_BLOCKLISTCHANGE	WM_NOTIFY_BASE + 2
#define WM_NOTIFY_RINGRULECHANGE	WM_NOTIFY_BASE + 3
#define WM_NOTIFY_CALLOPTIONCHANGE	WM_NOTIFY_BASE + 4
#define WM_NOTIFY_OUTGOING			WM_NOTIFY_BASE + 5
#define WM_NOTIFY_CONNECTED         WM_NOTIFY_BASE + 6
#define WM_NOTIFY_BATTLEVEL         WM_NOTIFY_BASE + 7
#define WM_NOTIFY_SIGNALLEVEL       WM_NOTIFY_BASE + 8
#define WM_NOTIFY_CARRIER           WM_NOTIFY_BASE + 9
#define WM_NOTIFY_PHONEPROFILE      WM_NOTIFY_BASE + 10
#define WM_NOTIFY_AIRPLANE          WM_NOTIFY_BASE + 11
#define WM_NOTIFY_PROFILE_RINGVOLUME WM_NOTIFY_BASE + 12
#define WM_NOTIFY_PROFILE_RINGNAME  WM_NOTIFY_BASE + 13
#define WM_NOTIFY_DATETIME          WM_NOTIFY_BASE + 14
#define WM_NOTIFY_INCOMINGOID       WM_NOTIFY_BASE + 15
#define WM_NOTIFY_OUTGOINGOID       WM_NOTIFY_BASE + 16
#define WM_NOTIFY_GENERALOPTIONS    WM_NOTIFY_BASE + 17
#define WM_NOTIFY_INCOMINGNUMBER    WM_NOTIFY_BASE + 18

#define WM_NOTIFY_END				WM_NOTIFY_BASE + 99
#define WM_CLOSE_StartMenu          WM_USER + 6123

#define REG_KEY_ISS_PATH			_T("SOFTWARE\\Pano\\PhoneGenius")
#define REG_TodayItems_Path			_T("SOFTWARE\\Pano\\PhoneGenius\\Today\\Items")
#define REG_TodayContacts_Path		_T("SOFTWARE\\Pano\\PhoneGenius\\Today\\Contacts")
#define REG_PhoneProfile			_T("PhoneProfile")
#define REG_BlockListChanged		_T("Blocklistchanged")
#define REG_RingRuleChanged			_T("Ringtonerulechanged")
#define REG_CallOptionsChanged		_T("Calloptionschanged")
#define REG_LockItems               _T("LockItems")

#define TXT_FileBlocklist		    _T("\\windows\\pano_blocklist.dat")
#define TXT_FileCallOptionlist		_T("\\windows\\pano_calloptionlist.dat")
#define TXT_FileRingtonRulelist		_T("\\windows\\pano_ringtonerulelist.dat")

#define VERSION_PhoneGenius         _T("1.9.2") //not yet released
#define VERSION_AreaCodeGenius      _T("1.3.1") //minor update for samsung
#define VERSION_CallGenius          _T("1.8.8")
#define VERSION_Contacts            _T("1.9.1")
#define VERSION_Launcher            _T("1.6.3")

// calling options
#define CALL_Reject			0x000
#define CALL_Allow			0x001
#define CALL_NoRing			0x001 << 1
#define CALL_NoVibrate		0x001 << 2

// utilities
static BOOL IsValidOID(long lOid)
{
	switch(lOid)
	{
	case -1:	// assume undefined??
	case 0:		// none
	case 1:		// unknown
		return FALSE;
	}

	return TRUE;
}
