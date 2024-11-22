//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
#pragma once

#include "phone.h"

/********************************************************************
 ********************************************************************
 ********************************************************************
    Section 1:
    Phone State:  This section documents the phone state exposed
    within the phone application.  Note that all of these API's must
    be called from within the phone process.  More complete documentation
    for these API's is contained elsewhere in this file.

    Phone state API summary:
    These API's are exported from tpcutil.dll.
    
    All of the phone state is stored as PH_IDs. 
    PH_ID // A particular item of phone state.  ex: accumulator, count of incoming calls, etc.
    
    
    // Query and set the PH_IDs.
    PHGetInt            // Get a PH_ID of type int.
    PHGetDWORD          // Get a PH_ID of type DWORD.
    PHGetStringPtr      // Get a PH_ID of type const TCHAR *.
    PHGetStringToBuffer // Fill a buffer with a string PH_ID.
    PHGetBlobPtr        // Get a PH_ID of type const blob *.
    PHSetAccumulator    // Set the string in the accumulator.

    // Subscribe/unsubscribe to notifications of phone state changes.
    PHAddListener       // Ask for notifications for phone state changes.
    PHRemoveListener    // Stop notifications of changes in the phone state.
    PHGetChangedIDs     // Get an array of the changed PH_IDs (phone states).

    // Manipulation of current phone state
    PHSetExtPhoneState  // Add non tapi phone calls to the phone application.
    PHSetCallLogInfo    // Change the way tapi calls are written to the call log.

    // Perform common phone verbs like dial, hold, etc.
    PHDoVerb            // Ask the phone application to perform some action.

    // Show either an error message or a custom message.
    PHShowPhoneMsg
 ********************************************************************
 ********************************************************************
 ********************************************************************/

 
/********************************************************************
    Below are a number of components of the PHPHONESTATE structure.
 ********************************************************************/

// various information about a call.
// used in PHCALLINFO
typedef enum tagPH_CALLINFOFLAGS
{
     PH_CIF_FINDSTATE_MASK           =    0x0000000F,
     PH_CIF_FINDSTATE_SHIFT          =    0,
     PH_CIF_FINDNOTSTARTED           =    0x00000000,
     PH_CIF_FINDSTARTED              =    0x00000001,
     PH_CIF_FINDSUCCEEDED            =    0x00000002,
     PH_CIF_FINDFAILED               =    0x00000003,
     PH_CIF_INCOMING                 =    0x00000080,
     PH_CIF_OUTGOING                 =    0x00000100,
     PH_CIF_DIRECTION_MASK           =    (PH_CIF_INCOMING | PH_CIF_OUTGOING),
     PH_CIF_CLOG_WRITENOTSTARTED     =    0x00000000,
     PH_CIF_CLOG_WRITESTARTED        =    0x00001000,
     PH_CIF_CLOG_WRITEPENDING        =    0x00002000,
     PH_CIF_CLOG_WRITESUCCEEDED      =    0x00003000,
     PH_CIF_CLOG_WRITEFAILED         =    0x00004000,
     PH_CIF_CLOG_WRITESTATE_MASK     =    0x0000F000,     
     PH_CIF_CALLNETWORK_CELLULAR     =    0x00000000,      
     PH_CIF_CALLNETWORK_VOIP         =    0x00010000,   
     PH_CIF_CALLNETWORK_MASK         =    (PH_CIF_CALLNETWORK_VOIP | PH_CIF_CALLNETWORK_CELLULAR), 
} PH_CALLINFOFLAGS;

// Buffer sizes used in PHCALLINFO
#define PH_CCH_DISPLAY_NAME 80
#define PH_CCH_DISPLAY_NUMBER 64
#define PH_CCH_DISPLAY_COMPANY_NAME 80
#define PH_CCH_DISPLAY_TITLE 80
#define PH_CCH_DISPLAY_METHOD 10

// Use this to refer to a specific call in a verb
DECLARE_HANDLE(HPHONECALL);


// Caller ID state for a call.
// used in PHCALLINFO
typedef enum tagPH_CALLERIDSTATE
{
    PH_CID_UNAVAILABLE,  // Caller-ID information has not been provided by the network
    PH_CID_BLOCKED,      // Caller-ID information has been blocked by the caller
    PH_CID_VALID,        // Caller-ID information is valid
    PH_CID_AMBIGUOUS,    // Caller-ID information is unknown because the end-user has pressed flash (only affects CDMA)
} PH_CALLERIDSTATE;

// Possible states of a call in a group.
// used in PHCALLINFO
typedef enum tagPH_CALLSTATE
{
    PH_CS_UNKNOWN,                          // Call is in an indeterminate state
    PH_CS_AVAILABLE,                        // Incoming call available
    PH_CS_CALLING,                          // Attempting to setup a call
    PH_CS_TALKING,                          // In a voice call
    PH_CS_HOLD,                             // Call is on hold
    PH_CS_IDLE,                             // Call has been disconnected
    PH_CS_IDLE_BUSY,                        // Same as PH_CS_IDLE - Disconnect reason was that the other party was busy
    PH_CS_IDLE_NOANSWER,                    // Same as PH_CS_IDLE - Disconnect reason was that the other party did not answer
    PH_CS_IDLE_NODIALTONE,                  // Same as PH_CS_IDLE - Disconnect reason was network problems
    PH_CS_IDLE_UNREACHABLE,                 // Same as PH_CS_IDLE - Disconnect reason was network problems and/or the other party was unreachable
    PH_CS_IDLE_BADADDRESS,                  // Same as PH_CS_IDLE - Disconnect reason was that the number dialed was invalid
    PH_CS_IDLE_CANCELLED,                   // Same as PH_CS_IDLE - Disconnect reason was that the call was cancelled by the end-user
    PH_CS_IDLE_PHONECONNECTIONFAILURE,      // Same as PH_CS_IDLE - Disconnect reason was that the phone connection failed
    PH_CS_IDLE_INVALIDSIMCARD,              // Same as PH_CS_IDLE - Disconnect reason was that the SIM card was invalid
    PH_CS_IDLE_SIMCARDBUSY,                 // Same as PH_CS_IDLE - Disconnect reason was that the SIM card was busy
    PH_CS_IDLE_NETWORKSERVICENOTAVAILABLE,  // Same as PH_CS_IDLE - Disconnect reason was that network service was not available
    PH_CS_IDLE_EMERGENCYONLY,               // Same as PH_CS_IDLE - Disconnect reason was that the radio is only handling emergency requests
    PH_CS_IDLE_ERROR,                       // Same as PH_CS_IDLE - Disconnect reason was that an error occurred
    PH_CS_IN_CONFERENCE,                    // Call is one member of a conference call
    PH_CS_IDLE_CONGESTION,                  // Same as PH_CS_IDLE - Disconnect reason was that the network is congested
    PH_CS_IDLE_TEMPFAILURE,                 // Same as PH_CS_IDLE - Disconnect reason was that the call was dropped
    PH_CS_IDLE_SIMCALLBLOCKED,              // Same as PH_CS_IDLE - Disconnect reason was that the call was blocked by USIM
    PH_CS_IDLE_SIMCALLMODIFIEDTOUSSD,       // Same as PH_CS_IDLE - Disconnect reason was that the call was modified by USIM to another calltype
    PH_CS_IDLE_DESTINATIONBARRED,           // Same as PH_CS_IDLE - Disconnect reason was that the call was barred
    PH_CS_IDLE_FDNRESTRICT,                  // Same as PH_CS_IDLE - Disconnect reason was that call was FDN blocked

    //The following call states are understood at the application level
    //but not over a cellular network. It is up to either the VoIP implementation
    //or the OEM canvas plugin to place calls in this state.
    //Note for integration: the following call states should always be the LAST ones in this enumaration
    PH_CS_TRANSFERRING,                     // Call is being transferred 
    PH_CS_WAIT_FOR_SIMPLE_TRANSFER,         // Wait for a simple transfer
    PH_CS_WAIT_FOR_ATTENDED_TRANSFER,       // Wait for a attended transfer
    PH_CS_IDLE_TRANSFERCOMPLETE,            // Same as PH_CS_IDLE - Disconnect reason was that the transfer completed successfully

} PH_CALLSTATE;

// These flags determine how the phone application treats calls created by the phone canvas developer using 
// PHSetExtPhoneState.  By default, these calls will be added to the call log
// and the caller id will be used to find a contact.  However, these actions may be disabled
// for these calls.  If you need to prevent call log entries from being created for non-user
// calls reported in PH_ID_PHONE_STATE, see PHSetCallLogInfo.
typedef enum tagPH_CALLBEHAVIORFLAGS
{
    PH_CBF_LOGENTRY             =     0x00000000,  // When the call is in an ended state, it will be
                                                   // written to the call log.  This call if it is
                                                   // missed will also cause a missed call notification
                                                   // to appear.
    PH_CBF_NOLOGENTRY           =     0x00000001,  // Not written to the call log.
    PH_CBF_LOGENTRYMASK         =     0x00000001,  
    PH_CBF_CALLERIDLOOKUP       =     0x00000000,  // When call is first created, a caller id lookup is performed on szDisplayNumber and szDisplayName.
    PH_CBF_NOCALLERIDLOOKUP     =     0x00000002,  // No caller id lookup.
    PH_CBF_CALLERIDMASK         =     0x00000002,   
    PH_CBF_CALLPROGRESSONCALL   =     0x00000000,  // call progress comes forward when the call connects
    PH_CBF_HIDEPROGRESSONCALL   =     0x00000004,  // call progress does not come forward on PPC when call connects
    PH_CBF_PROGRESSONCALLMASK   =     0x00000004,  // The previous two flags are PocketPC only, they are ignored for smartphone.
    PH_CBF_SHOWDIALERONCALL     =     0x00000008,  // Show the dialer instead of call progress when the call connects
    PH_CBF_SHOWDIALERMASK       =     0x00000008,
} PH_CALLBEHAVIORFLAGS;

// These flags are written to the call log for calls set with PHSetExtPhoneState.
// when the PH_CBF_LOGENTRY behavior flag is set.  Note that call log entries are not created
// until the call ends.
typedef enum tagPH_CALLLOGFLAGS
{
    PH_CLF_DDIRECTIONMASK     =     0x00000001,  // indicates if the call incoming or outgoing
    PH_CLF_DINCOMING          =     0x00000000, 
    PH_CLF_DOUTGOING          =     0x00000001,

    PH_CLF_MMISSEDMASK        =     0x00000002,  // indicates if the call was missed.
    PH_CLF_MMISSED            =     0x00000000, 
    PH_CLF_MNOTMISSED         =     0x00000002,

    PH_CLF_EENDEDMASK         =     0x00000004,  
    PH_CLF_ENOEND             =     0x00000000,  
    PH_CLF_EENDED             =     0x00000004,  // call was ended
    
    PH_CLF_VVOICEMASK         =     0x00000008,
    PH_CLF_VVOICE             =     0x00000000,  // voice call
    PH_CLF_VDATA              =     0x00000008,  // data call

    PH_CLF_RROAMMASK          =     0x00000010,
    PH_CLF_RLOCAL             =     0x00000000,  // local call
    PH_CLF_RROAM              =     0x00000010,  // roaming call

    PH_CLF_LLINEMASK          =     0x00000300,  // represents the outgoing line which made the call.
    PH_CLF_LLINE0             =     0x00000000,
    PH_CLF_LLINE1             =     0x00000100,
    PH_CLF_LLINE2             =     0x00000200,
    PH_CLF_LLINEOTHER         =     0x00000300,

    PH_CLF_CICALLERIDMASK     =     0x00000c00,  // call id availablility
    PH_CLF_CIUNAVAILABLE      =     0x00000000,  // caller id number not available
    PH_CLF_CIBLOCKED          =     0x00000400,  // caller id number blocked
    PH_CLF_CIAVAILABLE        =     0x00000800,  // caller id available

    PH_CLF_UENDEDTIMEMASK     =     0x00001000,  // indicates if we write ended time to call log.
    PH_CLF_UHAVEENDEDTIME     =     0x00000000,  // have ended time
    PH_CLF_UNOENDEDTIEM       =     0x00001000,  // no ended time, if for some reason it is indeterminate

    PH_CLF_ICUSTOMICONMASK    =     0x00010000,
    PH_CLF_INOCUSTOMICON      =     0x00000000,  
    PH_CLF_ICUSTOMICON        =     0x00010000,  // allows you to specify a different icon for specific calls.  
                                                 // Notes 1 and 2 in Section 4 below further document how to 
                                                 // change the icons used by the call log application.

    PH_CLF_CALLNETWORK_CELLULAR =   0x00000000,  // indicate if this call is made by cellular network
    PH_CLF_CALLNETWORK_VOIP     =   0x00100000,  // or VoIP network  
    PH_CLF_CALLNETWORK_MASK     =   0x00100000, 

    PH_CLF_CONTACTUPDATE        =   0x00000000,  
    PH_CLF_NOCONTACTUPDATE      =   0x01000000,  // set to TRUE to prevent changes to the corresponding contact 
    PH_CLF_CONTACTUPDATE_MASK   =   0x01000000,  // affecting this call log entry

} PH_CALLLOGFLAGS;

// Used to refer to the index of an invalid call.
#define PH_ICALL_INVALID (-1)
   
// Full information for a call
// an array of these is contained in PHPHONESTATE
typedef struct tagPHCALLINFO
{
    int iCallNextInGroup; // PH_ICALL_INVALID if there is no next call
    TCHAR szDisplayName[PH_CCH_DISPLAY_NAME];
    TCHAR szDisplayNumber[PH_CCH_DISPLAY_NUMBER];
    TCHAR szDisplayCompanyName[PH_CCH_DISPLAY_COMPANY_NAME];
    TCHAR szDisplayTitle[PH_CCH_DISPLAY_TITLE];
    TCHAR szDisplayMethod[PH_CCH_DISPLAY_METHOD];
    TCHAR szRingTone[MAX_PATH];
    CEOID oidContact; // oid to use with the POOM API.
    CEPROPID pidMethod; // prop id of the matching method. 
    ULONGLONG ullStart;
    ULONGLONG ullEnd;
    UINT idCall; // display number for GSM requirements
    PH_CALLERIDSTATE cidstate;
    PH_CALLSTATE callstate;
    PH_CALLINFOFLAGS callinfoflags;
    PH_CALLLOGFLAGS calllogflags; // Only valid for calls set using PHSetExtPhoneState
    PH_CALLBEHAVIORFLAGS callbehaviorflags; // Only valid for calls created using PHSetExtPhoneState
    CEOID oidClog;  // null for active calls since the Clog entry isn't created until the call ends.  valid for PH_ID_LASTCALL.
    DWORD dwAddressID; // line ID that this call came in on (for phones that support multiple phone numbers)  May be INVALID_ADDRESS_ID (from tpcmgr.h)  Should generally be PH_LINE_1 or PH_LINE_2
    DWORD dwUser; // DWORD that the phone canvas developer is free to use.  It is ignored by MSFT.
    HPHONECALL hcall;
    TCHAR szNoteFile[MAX_PATH]; //Path to notes file associated with this call
} PHCALLINFO;

//Line ID macros (for PHCALLINFO.dwAddressID)
#define PH_LINE_1       0
#define PH_LINE_2       1
#define PH_LINE_VOIP    2 
#define PH_LINE_OTHER   3

// Helper to get the status of a contact lookup
__inline DWORD PHCI_GetFindState(const PHCALLINFO* pphci)
{
    return((pphci->callinfoflags & PH_CIF_FINDSTATE_MASK) >> PH_CIF_FINDSTATE_SHIFT);
}

// Possible states of a group.  a group is a collection of calls.
// a group only has one call in it unless it is a conference.
// used in PHGROUPINFO.
typedef enum tagPH_GROUPSTATE
{
    PH_GS_IDLE,
    PH_GS_CALLING,        // outgoing
    PH_GS_RINGING,        // incoming
    PH_GS_BUSY,
    PH_GS_TALKING,
    PH_GS_HOLD,
    PH_GS_ENDED,
    PH_GS_CONGESTION,     // network busy.  Occurs when the RIL driver gives a RIL_E_CONGESTION disconnect code. 
    PH_GS_DROPPEDCALL,    // network dropped call.  Occurs when RIL driver gives a RIL_RESULT_CALLDROPPED disconnect code.

    //The following states cannot be acheived by using the default implementation (cell network).
    //It is up to the VoIP implementation or OEM plugin to place/understand groups into these states.
    PH_GS_TRANSFERRING = PH_GS_DROPPEDCALL + 5, // transferring a call
    PH_GS_WAIT_FOR_SIMPLE_TRANSFER,             // wait for a simple transfer
    PH_GS_WAIT_FOR_ATTENDED_TRANSFER,           // wait for a attended transfer

} PH_GROUPSTATE;

// Info for a group of calls.  Only groups that are
// conference calls will have more than one call.
typedef struct tagPHGROUPINFO
{
    PH_GROUPSTATE groupstate;
    int cCalls;
    BOOL fMuted;     // TRUE if the microphone is muted for this call group
    int iCallFirst;  // PH_ICALL_INVALID if there is no first call.
    DWORD dwUser; // DWORD that the phone canvas developer controls. It is ignored by MSFT.
} PHGROUPINFO;

// Used to refer to the index of an invalid group.
#define PH_IGROUP_INVALID (-1)

/* 
    Data structure for PH_ID_PHONE_STATE:
    Contains all the information for all the groups and calls 
    on the phone.  A group is a collection of calls. The only
    type of group with more than one call is a (GSM) conference call.

    A call should be counted in only one of the counts in PHPHONESTATE,
    with the exception of cConnected.  cConnected is the total count of
    all connected calls (cHeld + cTalking).

    Note that this structure has an array of possibly more
    than one group in the rgGroups member.  Following that array
    there is an array of PHCALLINFO starting at offset cbCallsOffset.
    The PHPS_GetCallInfo functions are provided to allow easier
    access to a particular call in this array.
*/
typedef struct tagPHPHONESTATE
{
    int cGroups;              // the count of groups contained in rgGroups.
    int cTalking;             // Number of groups in the talking state
    int cBusy;                // Number of groups in the busy state
    int cConnected;           // Number of groups that are connected to something (ie. talking or held calls, not ended, idle, etc.)
    int cHeld;                // Number of groups that are held.
    int cOutgoing;            // Number of outgoing groups (ie. currently dialing groups)
    int cIncoming;            // Number of incoming groups (ie. currently ringing groups)
    int cEnded;               // Number of ended groups
    int iGroup1;              // index of the group that you may want to put at the top of the UI
    int iGroup2;              // index of a secondary group (put in a less prominant place in the UI than iGroup1)
    DWORD dwUser;             // DWORD that the phone canvas developer controls. It is ignored by MSFT.
    
    int cbCallsOffset;        // Number of bytes from the start of this structure to the start of an array of PHCALLINFO's.  Use the PHPS_GetCallInfo macro for easier access.
    PHGROUPINFO rgGroups[1];
} PHPHONESTATE;

// Helper to get a PHCALLINFO given a PHPHONESTATE structure pointer
__inline PHCALLINFO* PHPS_GetCallInfo(PHPHONESTATE* pphps, int iCall)
{
    return(((PHCALLINFO*)(((BYTE*)pphps) + pphps->cbCallsOffset)) + iCall);
}

#ifdef __cplusplus
// const Helper to get a PHCALLINFO given a PHPHONESTATE structure pointer
__inline const PHCALLINFO* PHPS_GetCallInfo(const PHPHONESTATE* pphps, int iCall)
{
    return(((PHCALLINFO*)(((BYTE*)pphps) + pphps->cbCallsOffset)) + iCall);
}
#endif

/********************************************************************
    Flags for PH_ID_ICON_STATE
*********************************************************************/

typedef enum tagPH_ICON
{
    PH_ICON_ROAM        = 0x00000001,  // Roaming
    PH_ICON_MUTE        = 0x00000002,  // call volume is muted.
    PH_ICON_ANALOG      = 0x00000004,  // analog roaming (RIL_ROAMSTATUS_ANALOG).
    PH_ICON_CALLFWD     = 0x00000008,  // call forwarding is enabled.
    PH_ICON_CALLFWD1    = 0x00000010,  // call forwarding for line 1 is enabled,  line 2 is disabled.
    PH_ICON_CALLFWD2    = 0x00000020,  // call forwarding for line 1 is disabled, line 2 is enabled.
    PH_ICON_CALLFWD12   = 0x00000040,  // call forwarding for line 1 is enabled,  line 2 is enabled.
    PH_ICON_VPRIVACY    = 0x00000080,  // RIL has reported RIL_CALLPRIVACY_STANDARD.
    PH_ICON_GPRS        = 0x00000100,  // GPRS attached.  CGREG reported home or roaming.
    PH_ICON_1XRTT       = 0x00000200,  // RIL has reported RIL_SYSTEMTYPE_1XRTTPACKET.
    PH_ICON_LOCATIONON  = 0x00000400,  // RIL_NOTIFY_LOCATION has reported location services are on.
    PH_ICON_LOCATIONOFF = 0x00000800,  // RIL_NOTIFY_LOCATION has reported location services are off.
    PH_ICON_BUSY        = 0x00001000,  // an outgoing call is busy.
    PH_ICON_BTH_HF_ON   = 0x00002000,  // bluetooth hands free is enabled.
    PH_ICON_RESERVED1   = 0x00004000,  // reserved
    PH_ICON_RESERVED2   = 0x00008000,  // reserved
    PH_ICON_RESERVED3   = 0x00010000,  // reserved
    PH_ICON_RESERVED4   = 0x00020000,  // reserved
    PH_ICON_ANALOGROAM  = 0x40000000,  // RIL reported RIL_ROAMSTATUS_ANALOG.
    PH_ICON_DIGITALROAM = 0x80000000,  // RIL reported RIL_ROAMSTATUS_DIGITAL.
} PH_ICON;


/********************************************************************
    List of phone data IDs.
*********************************************************************/
typedef enum tagPH_ID
{
    PH_ID_PHONE_STATE,           // Blob:    Information about all voice calls on the system.  Contents is PHPHONESTATE structure.
    PH_ID_LASTCALL_INFO,         // Blob:    Information about the last call in the call log.  Contents is PHCALLINFO structure.
    PH_ID_TALKING,               // int:     Number of calls that are in 'talking' state
    PH_ID_BUSY,                  // int:     Number of busy calls
    PH_ID_CONNECTED,             // int:     Number of calls that are in 'connected' state
    PH_ID_HELD,                  // int:     Number of calls that are on hold
    PH_ID_INCOMING,              // int:     Number of incoming calls
    PH_ID_OUTGOING,              // int:     Number of outgoing calls
    PH_ID_ACCUMULATOR,           // TCHAR *: Contents of the accumulator (digits typed by the end-user.)
    PH_ID_ACCUMULATOR_EMPTY,     // DWORD:   Flag indicating whether the accumulator is empty or not
    PH_ID_VERB_AVAILABILITY,     // DWORD:   Availability of phone verbs.  Each bit represents a verb.
    PH_ID_OPERATOR,              // TCHAR *: Current Operator display name
    PH_ID_ICON_STATE,            // DWORD:   Flags that enable and disable icons in the UI
    PH_ID_CALL_ENDED_TIMEOUT,    // DWORD:   Set to 1 when phone state times out from Ended to Last call
    PH_ID_EXT_PHONE_STATE,       // Blob:    Information about additional non-tapi voice calls on the system.  Contents is PHPHONESTATE structure.
                                 //          This data is set by the phone canvas developer via PHSetExtPhoneState.   
    PH_ID_ACCUMULATOR_EX,        // TCHAR *: Contents of the accumulator (digits & letters typed by the end-user.)
    PH_ID_SMARTDIAL_NAME,        // TCHAR *: Name of selected smartdial entry (supported only on PPC).
    PH_ID_SMARTDIAL_NUMBER,      // TCHAR *: Number of selected smartdial entry (supported only on PPC).
    PH_ID_SMARTDIAL_SHOWN,       // DWORD:   Set to 1 when Smartdial control is shown, 0 otherwise (supported only on PPC).
    PH_ID_VOIP_OPERATOR,         // TCHAR *: Current VoIP Operator display name
    PH_ID_UI_VOLUME,             // DWORD:   Current in-call volume setting reflected in the UI.  Ranges from 0 to 5.
    PH_ID_SMARTDIAL_MATCH_COUNT, // DWORD:   The number of matches found by smartdial (supported only on PPC).
    PH_ID_COUNT
} PH_ID;

/********************************************************************
    views in the phone application (cprog.exe).
    Not all of these views may be valid on a specific device.
 ********************************************************************/
typedef enum tagPH_VIEW
{
    PH_VIEW_INCOMING,            // overridable: incoming call view
    PH_VIEW_DIALER,              // overridable: PocketPC dialer view
    PH_VIEW_PRIV_CALL,           // overridable: remove a call from a conference (GSM only)
    PH_VIEW_NETWORK_SELECTION,   // overridable: manual network selection (GSM only)
    PH_VIEW_CALL_HISTORY,        // not overridable: see Section 4 for instructions on how to change the default bitmaps.
    PH_VIEW_SPEED_DIAL,          // not overridable: see Section 4 for instructions on how to change the default bitmaps.
    PH_VIEW_SIM_PIN,             // not overridable: see Section 4 for instructions on how to change the default bitmaps.
    PH_VIEW_PROGRESS,            // overridable: call progress or active calls view
    PH_VIEW_SMARTDIALER,         // not overridable: PocketPC only SmartDialer view

    PH_VIEW_PREFERRED_DIALER,    // not overridable: PocketPC only. This is a virtual view.
                                 // Using this view with the PH_VERB_SWITCHVIEW verb will switch the view to
                                 // either PH_VIEW_SMARTDIALER or PH_VIEW_DIALER view, based on screen orientation
                                 // and other factors. See Section 4 - Controlling default keypad state for more details.

    PH_VIEW_COUNT
} PH_VIEW;


/********************************************************************
     The verbs supported by the phone application
 ********************************************************************/
#define PH_VERB_FIRST_STATIC_VERB 64

typedef UINT VPARAM;  // verb parameters
typedef enum tagPH_VERB
{
    PH_VERB_TALK,                    // no VPARAM needed, uses the string in the accumulator.
    PH_VERB_END,                     // VPARAM (optional) specifies which call group to end. Valid groups are a subset of PH_GROUPSTATE (PH_GS_CALLING, PH_GS_TALKING, PH_GS_HOLD). If no VPARAM (i.e NULL), the phone app chooses which call group to end
    PH_VERB_CHLD1,                   // no VPARAM needed. Perform AT+CHLD=1 which ends the active call and answers the incoming call
    PH_VERB_HOLD,                    // no VPARAM needed, holds active call.
    PH_VERB_UNHOLD,                  // no VPARAM needed, unholds active call.
    PH_VERB_SWAP,                    // no VPARAM needed, swaps current calls.
    PH_VERB_PRIVATE,                 // VPARAM is the HPHONECALL to make private.
    PH_VERB_CONFERENCE,              // no VPARAM needed, conferences current calls.
    PH_VERB_MUTE,                    // no VPARAM needed, mutes the phone.
    PH_VERB_UNMUTE,                  // no VPARAM needed, unmutes the phone.
    PH_VERB_FLASH,                   // no VPARAM needed, uses the string in the accumulator.
    PH_VERB_REJECT_INCOMING,         // VPARAM is a boolean indicating if the call should be hidden from the UI (TRUE) or if it should be shown normally as an ended call (FALSE).
    PH_VERB_ACCEPT_INCOMING,         // no VPARAM needed, answers the incoming call.
    PH_VERB_SPEAKERPHONEON,          // no VPARAM needed, enables the speaker phone.
    PH_VERB_SPEAKERPHONEOFF,         // no VPARAM needed, disables the speaker phone.
    PH_VERB_BTH_HF_ON,               // no VPARAM needed, enables the bluetooth hands free audio during the current call.
    PH_VERB_BTH_HF_OFF,              // no VPARAM needed, disables the bluetooth hands free audio during the current call.
    PH_VERB_NEWCONTACT,              // no VPARAM needed. creates a contact getting the info from either the current active call, the accumulator (Pocket PC only), or the last connected or outgoing call in the call log (Pocket PC only).
    PH_VERB_VIEWCONTACT,             // if no VPARAM is provided, defaults to showing the contact for the current call.  otherwise, VPARAM should be the oidContact. If the contact doesn't exist, it launches the Contacts App

    //The xxx_TRANSFER verbs is currently not enabled in the phone application - it is up to
    //either the voip implementation or the OEM plugin to choose to handle these verbs, but 
    //they might be natively supported by phone application later.
    PH_VERB_SIMPLE_TRANSFER,         // no VPARAM needed, choosing 'simple transfer' from menu makes it to 'wait for transfer' state
    PH_VERB_ATTENDED_TRANSFER,       // no VPARAM needed, choosing 'attended transfer' from menu makes it to 'wait for transfer' state
    PH_VERB_COMPLETE_TRANSFER,       // no VPARAM needed, do the actual transfer for given phone number from accumulator
    PH_VERB_CANCEL_TRANSFER,         // no VPARAM needed, cancel the transfer, goes back from 'wait for transfer' state
    PH_VERB_DIAL_STRING,             // VPARAM should be a pointer to PHDIALSTRING, indicating the number to dial, optionally the name.
    
    PH_VERB_REC_START,               // no VPARAM, starts the recording of the active conversation.
    PH_VERB_REC_STOP,                // no VPARAM, stops the recording of the active conversation.
    
    PH_VERB_LAST_DYNAMIC_VERB,       // the availability of the above verbs is represented as a bit in the PH_ID_VERB_AVAILABILITY DWORD.  see PHVerbToFlag.
    
    PH_VERB_NEWSMS = PH_VERB_FIRST_STATIC_VERB,  // VPARAM is optional.  can be a const TCHAR pointer to a phone number.
    PH_VERB_NEWNOTE,                 // no VPARAM needed.  this is supported on Pocket PC only.  creates a note from the current active call or from the last connected or outgoing call in the call log.
    PH_VERB_LAUNCHOPTIONS,           // VPARAM is an int specifying which phone settings tab to show.  Pocket PC only.
    PH_VERB_LAUNCHCONTACTS,          // no VPARAM needed.  switches to the contacts application.
    PH_VERB_LAUNCHCALENDAR,          // no VPARAM needed.  switches to the calendar application.
    PH_VERB_DIALVMAIL,               // no VPARAM needed.  dials the voice mail number.
    PH_VERB_DIALSPEEDDIAL,           // the VPARAM should be the integer shortcut associated with the speed dial entry.
    PH_VERB_MINIMIZE,                // The VPARAM should be a PH_VIEW.
    PH_VERB_SWITCHVIEW,              // The VPARAM should be a pointer to PHSWITCHVIEW.
    PH_VERB_UPDATE_INACTIVITY_TIMER, // The VPARAM should be an enum of PH_UPDATEINACTIVITYTIMER.
    PH_VERB_ADD_ACCUMULATOR,         // The VPARAM should be a pointer to a PHADDACCUMULATOR structure.
    PH_VERB_REMOVE_ACCUMULATOR,      // VPARAM is a boolean indicating if the entire accumulator should be cleared (TRUE) or if just one digit should be removed (FALSE).
    PH_VERB_LAST_STATIC_VERB,        // static verbs above are always available. 
} PH_VERB;

// Used to convert a PH_VERB to a bit for PH_ID_VERB_AVAILABILITY.
// Note that only dynamic PH_VERB's that depend on the current phone
// state will be represented in PH_ID_VERB_AVAILABILITY.  All PH_VERB's
// greater than or equal to PH_VERB_NEWSMS are considered static verbs 
// that are always available.
// Clients can also check a verb's state by calling PHDoVerb
// with fValidateOnly set (see below.)
// If we end up with more than 32 dynamic verbs, we will have an additional
// PH_ID_ for the next 32.  We need this so clients can get notified
// when a verb's availability changes.
__inline DWORD PHVerbToFlag(PH_VERB verb)
{
    return 1 << verb;
}



// used with PH_VERB_ADD_ACCUMULATOR.
// conveys the state of a key on the keyboard or a key on a button.
// when a key is pressed, PH_KEYSTATE_DOWN should be sent allowing the
// the phone application to start playing DTMF.
// when it is released, PH_KEYSTATE_UP should be sent which notifies
// the phone application to stop playing DTMF.
// if the key is held,  PH_KEYSTATE_HELD should be sent which notifies
// the phone application to try to dial the speed dial number.
typedef enum tagPH_KEYSTATE
{
    PH_KEYSTATE_UP    = 0,
    PH_KEYSTATE_DOWN  = 0x1,
    PH_KEYSTATE_HELD  = 0x2,
    PH_KEYSTATE_CANCEL= 0x4      // Not yet implemented
} PH_KEYSTATE;

// used with PH_VERB_ADD_ACCUMULATOR.
typedef struct tagPHADDACCUMULATOR
{
    UINT vkKeyCode;             // Key code
    PH_KEYSTATE ksKeyState;  // Key flags, up/down/held
} PHADDACCUMULATOR;

// used with PH_VERB_DIAL_STRING
// This indicates whether or not a dial string will be
// permitted to execute a supplemental services command.
// Safe codes are defined as codes that will not harm the
// SIM (such as codes to change one's SIM PIN, which could lock the phone).
typedef enum tagPH_SUPSVC_SECURITY
{
    PH_SUPSVCS_DISALLOWED = 0,
    PH_SUPSVCS_SAFE_CODES_ALLOWED,
    PH_SUPSVCS_ALL_CODES_ALLOWED,
    PH_SUPSVCS_MSFT_DEFAULT, // Let the phone application choose based on whether
                             // or not the dial string has an associated name.
} PH_SUPSVC_SECURITY;

// used with PH_VERB_DIAL_STRING to dial a given dial string and contact name (optional).
typedef struct tagPHDIALSTRING
{
    PH_SUPSVC_SECURITY phSupSvcLevel;
    TCHAR* pszDestinationAddress;
    TCHAR* pszCalledParty;
} PHDIALSTRING;

// used with PH_VERB_SWITCHVIEW when switching to PH_VIEW_CALL_HISTORY.
typedef enum tagPH_CALLHISTORYFILTER
{
    PH_CALLHISTORY_ALLCALLS,
    PH_CALLHISTORY_MISSED,
    PH_CALLHISTORY_INCOMING,
    PH_CALLHISTORY_OUTGOING,
} PH_CALLHISTORYFILTER;

// used with PH_VERB_SWITCHVIEW when switching to PH_VIEW_DIALER.
typedef enum tagPH_DIALERFILTER
{
    PH_DIALER_DEFAULT,
    PH_DIALER_RESETSMARTDIAL,
} PH_DIALERFILTER;

// used with PH_VERB_SWITCHVIEW 
typedef struct tagPHSWITCHVIEW
{
    PH_VIEW view;
    union
    {
        PH_CALLHISTORYFILTER filter;
        PH_DIALERFILTER reset;
    };
} PHSWITCHVIEW;

/*
    The Phone INACTIVITY_TIMER

    Purpose:
        If the end-user doesn't interact with the PH_VIEW_PROGRESS view
        within x seconds following the end of all phone calls, it will
        minimize itself.  This inactivity timer fires after 10 seconds
        by default. The default value can be changed by modifying the 
        dword value of "CallProgressIdleTimeout" under the [HKCU\ControlPanel\Phone]
        registry key.  This value specifies the inactivity time
        in milliseconds.  If the end-user interacts with the a control in
        the PH_VIEW_PROGRESS view, the control should reset the inactivity
        timer via the PH_VERB_UPDATE_INACTIVITY_TIMER verb.  To stop 
        the inactivity timer and prevent the PH_VIEW_PROGRESS view from
        minimizing itself, UIT_END can be used.
 */
typedef enum tagPH_UPDATEINACTIVITYTIMER
{
    PH_UIT_RESET,      // Reset the inactivity timer
    PH_UIT_END         // End the inactivity timer
} PH_UPDATEINACTIVITYTIMER;

/*
    PHDoVerb can be used to perform common phone actions such as dialing
        and answering calls.  See the list of PH_VERBs above.
    [in]  verb : The verb to execute.
    [in]  fValidateOnly : if true, the verb won't be performed but will check to see
                          if the verb can be performed.  Ex: ending a call can't be
                          performed unless there is an active or incoming call.
    [in]  VPARAM  : The verb param (VPARAM) documented with the PH_VERBs.
    
    returns:
    S_OK : success.  if fValidateOnly is true, the verb is available now.
    E_FAIL : failure.  if fValidateOnly is true, the verb is NOT available now.
    E_NOTIMPL : failure.  if fValidateOnly is true, the verb will never be available.
             examples: PH_VERB_HOLD is only available when there's an active call not on hold.
             PH_VERB_FLASH is never available for GSM-based phones.
    E_INVALIDARG  : failure
    E_OUTOFMEMORY : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHDoVerb(PH_VERB verb, BOOL fValidateOnly, VPARAM vp);











/********************************************************************
  Phone State change message

  Each client can register this message via PHAddListener.
  Clients can choose another message id if desired when registering
  with PHAddListener.

 WPARAM - hListener
 LPARAM - 0

*********************************************************************/
#define WM_PH_CHANGE (WM_USER + 1)

// used to uniquely identify listeners
DECLARE_HANDLE(HPHONELISTENER);

const HPHONELISTENER INVALID_HPHONELISTENER = (HPHONELISTENER)-1;

#ifdef __cplusplus
extern "C" {
#endif
/*
    PHAddListener allows the caller to specify an hwnd and message that
       will be posted to clients when the state of phone data changes.
    [in]  pData : Pointer to an array of PH_IDs.
    [in]  cData : Count of the number of PH_IDs specified in pData.
    [in]  hwndNotify : uMsg will be posted to hwndNotify whenever
                     the data represented by a PH_ID in pData changes.
    [in]  uMsg  : The message that should be posted to hwndNotify.
    [out] phListener : pointer to a listener handle.  AddListener will
                      populate this value for later use by the client
                      in PHRemoveListener.
    
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG  : failure
    E_OUTOFMEMORY : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHAddListener(const PH_ID* pData, int cData, HWND hwndNotify, UINT uMsg, HPHONELISTENER* phListener);











/*
    PHRemoveListener allows a client to disable future notifications of
        change notifications requested in PHAddListener.  Message
        notifications currently in the message queue will not be
        removed.
    [in]  hListener : The HPHONELISTENER returned by PHAddListener in phListener.
    
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHRemoveListener(HPHONELISTENER hListener);











/*
    PHGetChangedIDs : Returns an array of PH_IDs that have changed since the previous call
                      to PHGetChangedIDs.
    [in]     hListener : The HPHONELISTENER returned by PHAddListener in phListener.
    [out]    pcids : The count of PH_IDs returned in ppids.
    [in/out] ppids : An array of PH_IDs that have been changed since the last time
                     this HPHONELISTENER called this function.  This will only return
                     the PH_IDs subscribed to by this HPHONELISTENER via the PHAddListener
                     function.  The caller must free this buffer using LocalFree.
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG  : failure
    E_OUTOFMEMORY : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetChangedIDs(HPHONELISTENER hListener, int* pcids, PH_ID** ppids);












/*
    PHGetInt : Returns the value of a PH_ID of type int.
    [in]  phid : PH_ID whose data will be returned.
    [out] pi   : Pointer to an integer.  The integer value of the PH_ID.
    
    returns:
    S_OK : success
    E_PENDING : indicates this PH_ID has no data yet. ex: PH_ID_TALKING before
                any calls have been made.
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetInt(PH_ID phid, int* pi);











/*
    PHGetDWORD : Returns the value of a PH_ID of type DWORD.
    [in]  phid : PH_ID whose data will be returned.
    [out] pdw  : Pointer to a DWORD.  The DWORD value of the PH_ID.
    
    returns:
    S_OK : success
    E_PENDING : indicates this PH_ID has no data yet.
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetDWORD(PH_ID phid, DWORD* pdw);











/*
    PHGetStringPtr : Returns a pointer to the string data for a PH_ID of type TCHAR *.
    [in]  phid : PH_ID whose data will be returned.
    [out] ppsz  : Pointer to a TCHAR *.  The string data for the PH_ID.  This memory should
                  not be freed.
    
    returns:
    S_OK : success
    E_PENDING : indicates this PH_ID has no data yet. ex: PH_ID_ACCUMULATOR before
                any calls have been made.
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetStringPtr(PH_ID phid, const TCHAR** ppsz);











/*
    PHGetStringToBuffer : Copies string data for a PH_ID of type TCHAR * into the specified string buffer.
    [in]     phid  : PH_ID whose data will be returned.
    [out]    psz   : Pointer to a string.  The string data for the PH_ID will be copied into the memory specified by psz.
    [in/out] pcch  : Pointer to an integer.  The number of characters in the string buffer psz.  If psz is NULL, 
                     the number of characters needed not including the null terminator will be
                     returned in *pcch.
    
    returns:
    S_OK : success
    E_PENDING : indicates this PH_ID has no data yet.
    E_INVALIDARG : failure
    STRSAFE_E_*  : returns any of the errors in strsafe.h.
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetStringToBuffer(PH_ID phid, TCHAR* psz, int *pcch);











/*
    PHGetBlobPtr : Returns a pointer to the blob of data for a PH_ID of type blob.
    [in]  phid : PH_ID whose data will be returned.
    [out] pcb  : Pointer to a DWORD.  The count of bytes returned in ppb.
    [out] ppb  : Pointer to a blob.  The data for the PH_ID.  This memory should not be freed.

    returns:
    S_OK : success
    E_PENDING : indicates this PH_ID has no data yet. ex: PH_ID_LASTCALL_INFO before
                any calls have been made.
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHGetBlobPtr(PH_ID phid, DWORD* pcb, const BYTE** ppb);











/*
    PHSetAccumulator : Replaces the contents of the phone accumulator with the 
                       specified string.  Use PHDoVerb with 
                       PH_VERB_ADD_ACCUMULATOR, to append digits to the
                       accumulator.
    [in]  pszAccumulator : The string to put in the phone accumulator.
    
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHSetAccumulator(const TCHAR* pszAccumulator);











// In PH_ID_PHONE_STATE, iGroup1 refers to the “most important” group that should be shown 
// by the phone application.  iGroup2 refers to a secondary less important group that should 
// be shown.  If you would like to set iGroup1 and/or iGroup2 for PH_ID_PHONE_STATE, you must 
// set iGroup1 and/or iGroup2 by using PHSetExtPhoneState.  The phone application will copy these 
// values you set with PHSetExtPhoneState into PH_ID_PHONE_STATE.  However, you must specify which 
// array the iGroup1 and/or iGroup2 values reference.  
//
// - PH_GI_INVALID        This value indicates that the specified index does not refer to a group.
// - PH_GI_DEFAULT        This value causes the phone application to ignore iGroup1 and iGroup2 in the 
//                        PHPHONESTATE you are setting.  However, in this case the phone 
//                        application will consider calls from both PH_ID_PHONE_STATE and the new PHPHONESTATE. 
//                        It will populate the overall iGroup1 and iGroup2 itself using this information. If PH_GI_DEFAULT 
//                        is specified, it must be specified for both phgiGroup1 and phgiGroup2, unless
//                        there is only 1 group being set by the phone canvas developer.  In that case it is valid for phgiGroup2 to 
//                        be PH_GI_INVALID.  PH_GI_DEFAULT can only be used if there are 2 or less groups
//                        of calls total in PH_ID_PHONE_STATE and PH_ID_EXT_PHONE_STATE.
// - PH_GI_EXTPHONESTATE  lets the phone application know that iGroup indexes into the rgGroup array 
//                        that is being set in PHSetExtPhoneState.
// - PH_GI_MSFTPHONESTATE lets the phone application know that iGroup indexes into the rgGroup array 
//                        for the current phone state in PH_ID_PHONE_STATE.


typedef enum tagPH_GROUPINDEX
{
    PH_GI_INVALID = -1,   // Index does not refer to anything.  Use only if you have less than 2 groups.
    PH_GI_DEFAULT,        // Index refers to groups being set in PHSetExtPhoneState.  MSFT
                          // should select the proper iGroup1 and iGroup2.  Use only if you have 2
                          // groups or less, otherwise, use either PH_GI_EXTPHONESTATE or PH_GI_MSFTPHONESTATE.
    PH_GI_EXTPHONESTATE,  // Index refers to the array of groups being set in PHSetExtPhoneState..
    PH_GI_MSFTPHONESTATE, // Index refers to the array of groups already existing in PH_ID_PHONE_STATE.
} PH_GROUPINDEX;










/*
    PHSetExtPhoneState  : This function allows the phone canvas developer to create phone calls from outside of tapi.  Calling this
                          function will set the PH_ID_EXT_PHONE_STATE with the data provided.  In addition,
                          the calls and groups contained in that PHPHONESTATE will be merged into the 
                          PH_ID_PHONE_STATE as well.  This allows the default MSFT provided controls to respond
                          to calls created from outside of tapi.  For example, the VOIP call state could be stored here
                          causing the phone application to treat tapi calls and VOIP calls the same in the UI.
    [in]     ppsExt     : A pointer to the PHPHONESTATE containing calls you wish to add to the UI.
    [in]     cbSize     : DWORD containing the size in bytes of ppsExt.
    [in]     phgiGroup1 : The PH_GROUPINDEX indicating which group ppsExt->iGroup1 refers
                          to.
    [in]     phgiGroup2 : The PH_GROUPINDEX indicating which group ppsExt->iGroup2 refers
                          to.
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG  : failure
    E_OUTOFMEMORY : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHSetExtPhoneState(PHPHONESTATE* ppsExt, DWORD cbSize, PH_GROUPINDEX phgiGroup1, PH_GROUPINDEX phgiGroup2);










/*
    PHSetCallLogInfo    : This function allows the phone canvas developer to suppress calls from being written to the call
                          log and also to modify the call log entry itself that is being written by the 
                          phone application.
    [in]     hCall      : Handle to the phone call that the phone canvas developer wishes to modify.
    [in]     fLog       : Boolean indicating if the call should be logged.  FALSE indicates that the call will not be
                          written to the call log.
    [in]     phclFlags  : The PH_CALLLOGFLAGS that the phone canvas developer wishes to set for the call.  Typically, PH_CLF_ICUSTOMICON
                          is used to indicate that a call is actually an external call type.  In this case, the call log 
                          entry will use the phone canvas developer defined icon for this call type when displaying the call log.
    [in]     phclMask   : The PH_CALLLOGFLAGS that the phone canvas developer wishes to modify, using the values contained in
                          phclFlags.  Typically, this is set to PH_CLF_ICUSTOMICONMASK with phclFlags being set to PH_CLF_ICUSTOMICON
                          to mark a call as an external call type.                          
    returns:
    S_OK : success
    E_FAIL : failure
    E_INVALIDARG  : failure
    E_OUTOFMEMORY : failure
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHSetCallLogInfo(HPHONECALL hCall, BOOL fLog, PH_CALLLOGFLAGS phclFlags, PH_CALLLOGFLAGS phclMask);


/********************************************************************
    Phone application message boxes.
 ********************************************************************/
typedef enum tagPH_MSG
{
    // dial requests
    PH_MSG_THREEWAYCALLREQUEST, // used on CDMA when the end-user attempts to dial from outside the phone app during a call. 
    PH_MSG_CONFIRMDIALREQUEST,  // client app wants the end-user to confirm the dial request.  The Smartphone default is an automatic confirmation.  The PocketPC default is to populate the accumulator. 
    PH_MSG_SIMPHONEBOOKENTRY,   // bubble used to confirm the dialing of a SIM PB entry.

    // sup svcs
    PH_MSG_SUPSVCEXECUTING,
    PH_MSG_SUPSVCSUCCEEDED,
    PH_MSG_SUPSVCFAILED,

    // other
    PH_MSG_VOICEMAIL,
    PH_MSG_MISSEDCALL,
    PH_MSG_SPEAKERPHONEON,
    
    // errors these can be specified in PHShowPhoneMsg
    PH_MSG_ERRORCALLDROPPED,             // any dropped call that the ui doesn't end (ie called party hangs up).
    PH_MSG_ERRORNETWORKDROPPED,          // network dropped the call.  a call entering PH_CS_IDLE_TEMPFAILURE will cause this message.
                                         // Occurs when RIL driver gives a RIL_RESULT_CALLDROPPED disconnect code.
    PH_MSG_ERRORINVALIDDESTADDRESS,      // unable to complete the call as dialed.
    PH_MSG_ERRORNOTADMIN,                // service codes must be dialed directly by the end-user from the dialer
    PH_MSG_ERRORCONFERENCEFAILED,        // unable to conference the calls.
    PH_MSG_ERRORVOICEMAILSWITCHLINES,    // failed to dial the voice mail number on the alternative line
    PH_MSG_ERROREND,
    PH_MSG_ERRORPRIVATE,
    PH_MSG_ERRORANSWER,
    PH_MSG_ERRORREDIRECT,
    PH_MSG_ERRORFLASH,
    PH_MSG_ERRORHOLD,
    PH_MSG_ERRORSWAP,
    PH_MSG_ERRORUNHOLD,
    PH_MSG_ERRORMUTE,
    PH_MSG_ERRORUNMUTE,
    PH_MSG_ERRORDIAL,
    PH_MSG_ERRORNODIALTONE,
    PH_MSG_ERRORUNREACHABLE,
    PH_MSG_ERRORBADADDRESS,
    PH_MSG_ERRORPHONECONNECTIONFAILURE,
    PH_MSG_ERRORINVALIDSIM,
    PH_MSG_ERRORSIMBUSY,
    PH_MSG_ERRORNETWORKSERVICENOTAVAILABLE,
    PH_MSG_ERROREMERGENCYONLY,
    PH_MSG_ERRORRADIOOFF,
    PH_MSG_ERROROPERATIONFAILED,
    PH_MSG_ERROROUTGOINGCALLNOFREELINES,  // cannot place call.  you must end one current call before placing another.
    PH_MSG_ERRORNOVMAILNUMBER,            // No voicemail number is configured and the user tried to dial voicemail.  See section 4.
    PH_MSG_ERRORTRANSFER,                 // unable to transfer the call
    PH_MSG_CALLMODIFIEDBYSIM,             // SIM call control modified the destination address.

    // External message
    PH_MSG_EXT,

    PH_MSG_ERRORDESTINATIONBARRED,
    PH_MSG_ERRORFDNRESTRICT,
    
    PH_MSG_ERROR_CALLREC_FAILURE,
    PH_MSG_ERROR_CALLREC_OUT_OF_STORAGE,

    PH_MSG_LAST
} PH_MSG;

/*
    PHShowPhoneMsg
    
    Used to display any of the PH_MSG_ERROR messages or the PH_MSG_EXT message.


    [in] phmsg     : Indicates which message the phone application needs to present
                     to the end-user.
    [in] pszText   : Ignored for all messages besides PH_MSG_EXT.  Must be a pointer
                     to a non-empty string for PH_MSG_EXT.
                     Not all messages are supported.
                     PH_MSG_EXT                 pszText must not be NULL. pszTitle can be NULL.
                     PH_MSG_THREEWAYCALLREQUEST not supported
                     PH_MSG_CONFIRMDIALREQUEST  not supported.
                     PH_MSG_SIMPHONEBOOKENTRY   not supported.
                     PH_MSG_SUPSVCSUCCEEDED     not supported.
                     PH_MSG_SUPSVCFAILED        not supported.
                     PH_MSG_VOICEMAIL           not supported.
                     PH_MSG_MISSEDCALL          not supported.
                     PH_MSG_SPEAKERPHONEON      not supported.
                     PH_MSG_ERROREND            not supported.
                     PH_MSG_ERRORFLASH          not supported.
                     PH_MSG_ERRORMUTE           not supported.
                     PH_MSG_ERRORUNMUTE         not supported.
                     PH_MSG_ERRORRADIOOFF       not supported.
    [in] pszTitle  : Ignored for all messages besides PH_MSG_EXT.  If NULL, a
                     default title will be used.
                     

    returns:
    S_OK         : the phone application displayed the message.
    E_INVALIDARG : failure.
    E_NOTIMPL    : this error message is not implemented or supported from PHShowPhoneMsg.

*/
HRESULT PHShowPhoneMsg(PH_MSG phmsg, LPCWSTR pszText, LPCWSTR pszTitle); 
#ifdef __cplusplus
}
#endif













/********************************************************************
 ********************************************************************
 ********************************************************************
    Section 2:
    Phone Extension API:  This section documents the phone API's that
    may be implemented by the OEM.  These API's will be called by
    the phone application within the cprog.exe process.  Note that
    use of any of the above Phone state API's must be called from
    this same thread.  Also, each of these API's is optional and
    none need be implemented.


    HRESULT PHExtInitialize             // Allows OEM to do any initialization needed for the PHExt API's.
    HRESULT PHExtDoVerb                 // OEM verbs for non-tapi actions.
    HRESULT PHExtShowPhoneMsg           // OEM implementation of phone message boxes.
    HRESULT PHExtGetPhoneViewInfo       // Provides OEM dialog templates and window hook procedures.
    HRESULT PHExtGetPhoneControlMetrics // Customize phone control behavior.
    HRESULT PHExtOnPhoneStateChange     // Allows OEM to get first notification of a phone state change.
    
    To enable these API's, the following registry keys must be set:
      [HKLM\Security\Phone\Skin]
      "Enabled"=dword:1         // Must be set to 1 to enable any
                                // OEM customizations of the phone
                                // application.
      "ext"="oemphone.dll"      // Location of the PHExt API's.
                                // These API's are implemented by 
                                // the OEM as named exports from this dll.
 ********************************************************************
 ********************************************************************
 ********************************************************************/











/*
    PHExtInitialize
    
    This will be the first API call in the the OEM extension
    dll and will be called only once.  
    
    
    [in]  pVoid  : Reserved, currently unused.
    
    returns:
    S_OK : success
*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtInitialize(void *pVoid);










/*
    PHExtDoVerb
    
    Used to override verbs PH_VERB_TALK through
    PH_VERB_REMOVE_ACCUMULATOR and to indicate which of these
    verbs is available.  This functionality allows OEMs to
    provide non-cellcore based calls while re-using the default
    MSFT UI.
    
    [in]  verb  : The verb to execute.
    [in]  fValidateOnly : If true, the verb should not be performed but the OEM should
                          return whether or not the verb can be performed at that specific
                          instant.  The phone application will call this function often as
                          the phone's state changes in order to determine which actions can
                          be taken.  Phone menus and softkeys will reflect the results of
                          this call.                          
                          Ex: ending a call can't be performed unless there is an active or 
                          incoming call.  If a phone canvas developer is using extended
                          calls, when an active call is created with PHSetExtPhoneState,
                          this PHExtDoVerb would return S_OK for PH_VERB_END, if it is called 
                          with fValidateOnly set to true.  Once the call is ended, subsequent 
                          calls to PHExtDoVerb with fValidateOnly set to true would 
                          return either E_FAIL (indicating that end will not work under any 
                          circumstances), or E_NOTIMPL (indicating that while PHExtDoVerb won't
                          handle the PH_VERB_END, the phone application is free to determine if
                          it can).
    [in]  VPARAM  : A pointer to one of the verb structures.  This pointer can
                    be NULL for PH_VERBs that don't need additional information.
    
    returns:
    S_OK : success.  Verb is successfully executed.  If fValidateOnly is true,
                     this indicates the verb can be handled by the OEM and
                     PH_ID_VERB_AVAILABILITY will show the verb as 
                     available.
    E_FAIL : failure.  if fValidateOnly is true, the verb will be
                       shown as unavailable in the PH_ID_VERB_AVAILABILITY
                       dword.
    E_NOTIMPL : MSFT should provide the implementation for this verb.
*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtDoVerb(PH_VERB verb, BOOL fValidateOnly, VPARAM vp);













/*
    PHExtShowPhoneMsg
    
    Used to override any of the PH_MSG message boxes shown 
    by the phone application.

    [in] phmsg     : Indicates which message the phone application needs to present
                     to the end-user.
    [in] psz1      : For all messages besides (PH_MSG_CONFIRMDIALREQUEST and
                     PH_MSG_SIMPHONEBOOKENTRY), this is the message that the phone application
                     will present by default.  In the case of the PH_MSG_CONFIRMDIALREQUEST and
                     PH_MSG_SIMPHONEBOOKENTRY, this string represents the phone number that should
                     be presented to the end-user before dialing.
    [in] psz2      : For PH_MSG_SIMPHONEBOOKENTRY, this represents the name stored in the
                     phone book entry.  Otherwise, this is unused.
    [in] fRemove   : When TRUE, the Pocket PC notification for this message will be removed.
    [in] dwSupSvcStatus : The HRESULT from the sup svc or SIM request.  HRESULT's are in simmgr.h and supsvcs.h.
    [inout] pfDial : For PH_MSG_CONFIRMDIALREQUEST and PH_MSG_SIMPHONEBOOKENTRY , this parameter
                     indicates if the phone application should dial the number provided in
                     psz1.

    returns:
    S_OK      : the phone application will not display this PH_MSG.
    E_NOTIMPL : the phone application should display the default message box for this PH_MSG.

*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtShowPhoneMsg(PH_MSG phmsg, LPCWSTR psz1, LPCWSTR psz2, BOOL fRemove, DWORD dwSupSvcStatus, BOOL* pfDial); 











/*
    PHExtGetPhoneViewInfo
    
    Used to provide a new layout for the dialog box representing the phone view
    specified in parameter "view".  Not all views support this functionality.  See the
    below table.

    Either PH_VIF_SQUARE, PH_VIF_PORTRAIT, or PH_VIF_LANDSCAPE will be set in the
    pvif member indicating the dialog template that the phone application is
    requesting.  To specify a dialog template, fill in the phInstance parameter
    and the plpTemplateName.  phInstance identifies a module that
    contains a dialog box template named by the plpTemplateName parameter.
    plpTemplateName  is a long pointer to a null-terminated string that names
    the dialog box template resource in the module identified by the phInstance
    parameter. This template is substituted for the phone application’s dialog
    box template used to create the phone view. For numbered dialog box
    resources, plpTemplateName can be a value returned by the MAKEINTRESOURCE
    macro.

    Additionally, the OEM can provide a plpfnHook hook procedure in the phone
    view.  The hook procedure can process messages sent to the dialog box
    representing the phone view.  To enable a hook procedure, add the
    PH_VIF_ENABLEHOOK flag to the pvif parameter and specify the address of
    the hook procedure in the plpfnHook parameter.  The hook procedure should
    return TRUE to indicate that the phone application should not process
    this message.  A return value of FALSE will cause  the phone application
    to continue with its default handling of the message.

    Note that if controls are added and/or removed, their control ids should not
    coincide with the same id's used in the default layouts.  The phone dialogs
    may attempt to communicate with these controls via their control id.

    This function is called by the phone application both when phone
    view is being created and when the phone view needs to rotate
    due to a screen orientation change.  This allows the phone application
    to use different dialog templates for portrait and landscape.
    
    Table:
    PH_VIEW_INCOMING          // template : incoming call view
    PH_VIEW_DIALER            // template : PocketPC dialer view
    PH_VIEW_CALL_PICKER       // template : remove a call from a conference (GSM only)
    PH_VIEW_NETWORK_SELECTION // template : manual network selection (GSM only)
    PH_VIEW_CALL_HISTORY      // NA (for PocketPC only, bitmaps can be changed)
    PH_VIEW_SPEED_DIAL        // NA (for PocketPC only, bitmaps can be changed)
    PH_VIEW_SIM_PIN,          // NA (for PocketPC only, bitmaps can be changed)
    PH_VIEW_PROGRESS          // template : call progress or active calls view
    
    [in]     view : Any of the PH_VIEW enums.  It indicates which view
                      the phone application is creating or rotating.  Call history,
                      speed dial, and the unlock SIM PIN views are not replaceable 
                      by the OEM.  Hence, the phone application will not call
                      PHExtGetPhoneViewInfo for these views.
    [in/out] pvif            : PH_VIF_SQUARE, PH_VIF_PORTRAIT, and PH_VIF_LANDSCAPE
                               are set as input.  The client can add PH_VIF_ENABLEHOOK to 
                               indicate that plpfnHook is valid.
    [out]    phInstance      : see above.
    [out]    plpTemplateName : See above.
    [out]    plpfnHook       : See above.
    
    returns:
    S_OK      : success.
    E_NOTIMPL : the phone application should use its defaults.
*/
typedef BOOL (APIENTRY *LPPHONEVIEWHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef enum tagPH_VIEWINFOFLAGS
{
     PH_VIF_ENABLEHOOK =      0x00000001,
     PH_VIF_SQUARE =          0x00000002,
     PH_VIF_PORTRAIT =        0x00000004,
     PH_VIF_LANDSCAPE =       0x00000008,
} PH_VIEWINFOFLAGS;

#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtGetPhoneViewInfo
(
    PH_VIEW  view,
    HINSTANCE*  phInstance,
    PH_VIEWINFOFLAGS* pvif,
    LPPHONEVIEWHOOKPROC* plpfnHook,
    LPCTSTR*    plpTemplateName
); 











/*
    PHExtGetPhoneControlMetrics
    
    Each of the phone controls documented in Section 3 has configurable
    metrics.  These metrics can be configured per view in the registry.
    For fine-grained control, each instance of a control may have its
    metrics configured via PHExtGetPhoneControlMetrics.
    
    This function is called by the phone application both when control
    is being created and when the phone view needs to rotate due to a 
    screen orientation change.  This allows the phone application  controls
    to have different metrics for both landscape and portrait.

    PHExtGetPhoneControlMetrics is called to retrieve the value
    each metric supported by the particular control.  All of the
    metrics for each control are documented below in Section 3.

    [in]  view      : Any of the PH_VIEW enums.  It indicates which view
                      the phone application is creating or rotating.  Call history,
                      speed dial, and the unlock SIM PIN views are not replaceable 
                      by the OEM.  Hence, the phone application will not call
                      PHExtGetPhoneControlMetrics for these views.
    [in]  vif       : Either PH_VIF_SQUARE, PH_VIF_PORTRAIT, or PH_VIF_LANDSCAPE will
                      be set indicating the mode of the current view.
    [in]  idControl : The id of the control as specified in the resource template.
    [in]  pszValue  : The metric being requested.  See below.  Ex: "Nm", "Ht", etc.
    [out] lpData    : Pointer to a DWORD that should be populated with the value
                      of the requested metric.  In the case of strings, lpData
                      should be populated with a pointer to the string.
                      The caller of PHExtGetPhoneControlMetrics will call LocalFree 
                      for the strings.
    returns:
    S_OK      : success. lpData has been populated.
    E_NOTIMPL : the phone application should use defaults obtained from the registry.
                if there are no defaults in the registry, the phone application will
                use its built-in defaults.
*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtGetPhoneControlMetrics
(
    PH_VIEW  view,
    PH_VIEWINFOFLAGS vif,
    DWORD       idControl,
    LPCTSTR     pszValue,
    LPDWORD     lpData
); 






/*
    PHExtOnPhoneStateChange

    This function is called by the phone application when PH_ID_PHONE_STATE is 
    about to change as a result of a change in the state of TAPI calls.  This function
    allows a phone canvas to receive first notification when the PH_ID_PHONESTATE is
    about to change.  PHSetExtPhoneState can be called as a result of this in order to
    change the phone state.  An example of this could be placing an extended call on 
    hold when a TAPI call is being made.

    Note that in addition to the phone application calling this function, any listeners
    to the PH_ID_PHONE_STATE subscribed via PHAddListener will also be notified of the change.
    This function is synchronous with the phone call state change, performance is important.

    [in]  c_pps      : A pointer to the new PHPHONESTATE about to be set in PH_ID_PHONE_STATE.

    returns:
    S_OK      : An update occured.  This should be returned if anything is done in response
                to this notification (any calls to PHDoVerb).
    E_NOTIMPL : Nothing was updated.

*/
#ifdef __cplusplus
extern "C"
#endif
HRESULT PHExtOnPhoneStateChange
(
    const PHPHONESTATE* c_pps
);



/********************************************************************
 ********************************************************************
 ********************************************************************
    Section 3
    Win32 Phone UI Components: 
      This section documents the Phone UI components provided by MSFT.
      These components are either standard Win32 controls or Win32
      dialogs.  Each component is a certain CNTLTYPE as documented
      in the below table.
      
      The various phone views within the phone application are
      fully customizable because their dialog template may be specified
      via the PHExtGetPhoneView API.  To move, add, resize, or remove
      controls from a view, specify a new resource template when the 
      PHExtGetPhoneViewInfo API is called.  To add new functionality
      or entirely replace the default UI, specify a new resource
      template and a hook procedure when the PHExtGetPhoneViewInfo API
      is called.
      
      Metrics such as the fonts, color, backgrounds, or bitmaps for
      the default phone components may be customized.  For each view
      and orientation, the metrics for each CNTLTYPE may be configured
      as a whole with registry keys.  If you'd like to configure the
      metrics for individual components, PHExtGetPhoneControlMetrics
      must be implemented.
      
      Table CNTLTYPE's
        Text
        PlainButton
        VerbButton
        DialButton
        EraseButton
        TalkEndButton
        Other
        None

      The rest of this section documents each component, its metrics
      and the registry keys used to configure metrics for each
      CNTLTYPE.

      To enable OEM customization and specify the location of the 
      PHExtGetPhoneViewInfo, PHExtGetPhoneControlMetrics, 
      PHExtShowPhoneMsg and PHExtDoVerb API's, 
      [HKLM\Security\Phone\Skin]
      "Enabled"=dword:1         // Must be set to 1 to enable any
                                // OEM customizations of the phone
                                // application.
      "ext"="oemphone.dll"      // Location of the PHExtGetPhoneViewInfo,
                                // PHExtGetPhoneControlMetrics, and
                                // PHExtShowPhoneMsg, and PHExtDoVerb API's.
                                // These API's are implemented by 
                                // the OEM as exports from this dll.

      The default UI components provided by MSFT and used in the various
      phone views are as follows:
       WC_ACCUMULATOR   : Displays the accumulator string.
       WC_BRANDING      : Displays the operator's branding bitmap.
       WC_INVALIDPHONE  : Displays invalid SIM, missing SIM and phone off.  
                  Hidden by default.  
                  By default buddied to WC_BRANDING, WC_STATUSICONS, 
                  WC_CALLSTATE, WC_ELAPSEDTIME, WC_CALLERIDINFO and WC_CONTACTPICTURE
                  controls causing them to hide when it’s shown.
                  Because WC_CONTACTPICTURE and WC_STATUSICONS show and hide themselves, they are tied to the visibility state 
                  of WC_INVALIDPHONE using the mainControl property of WC_CONTACTPICTURE and WC_STATUSICONS.
       WC_CALERTCONTACT : Displays the incoming call's caller id information
                          as follows:
                          Name
                          Company (if exists and different from name)
                          Number
       WC_CALLERLIST    : Displays either the calls on hold (GSM-only) or 
                          the calls in a conference.
       WC_CALLSTATE     : Displays the current call state. Ex: Dialing,
                          Busy, Ended, On Hold, Last Call.
       WC_CALLERIDINFO  : Displays the caller id information for the
                          current call state as follows:
                          Name or Company if there's no name.
                          Number
       WC_CONTACTPICTURE: Displays the contact's picture.
       WC_ELAPSEDTIME   : Shows elapsed time for the active call.
       WC_LAYOUTHELPER  : Used to resize or move a component based on the
                          visiblity of an adjacent component.  For example,
                          in the default views provided by MSFT, this control
                          is used to resize the WC_CALLERIDINFO when the
                          WC_CONTACTPICTURE is shown.
       WC_SOLIDLINE     : Draws a solid line.  Used as a seperator in
                          some phone views.
       WC_STATUSICONS   : Draws icons representing the current PH_ID_ICON_STATE.
       WC_TEXTUI        : Draws static text.

       For Pocket PC only:
       WC_BUTTON        : Represents the buttons in the phone views.

       For smartphone only:
       WC_CALERTBACKGROUND  :Draws background of the incoming call alert dialog.
       
     As noted above, "Enabled" must be set to 1 under the [HKLM\Security\Phone\Skin]
     key, otherwise the phone components will use their defaults and will not
     use either the registry or PHExtGetPhoneControlMetrics.

     To get its configuration, each component will call PHExtGetPhoneControlMetrics
     for each metric it needs.  If PHExtGetPhoneControlMetrics is not implemented
     or returns E_NOTIMPL, the component will look in the registry for that metric.
     If there's no registy entry, it will use a default value.
     
     The registry key where the metrics are located is defined as follows:
     [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\<CNTLTYPE>].
     
     <PHONE_VIEW> must be one of the following: INCOMING, DIALER, CALL_PICKER,
         NETWORK_SELECTION, PROGRESS.
     <CNTLTYPE> must be one of the following: Text, PlainButton, VerbButton,
         DialButton, EraseButton, TalkEndButton, Other.
     <ORIENTATION> must be of of the following: portrait, landscape, square.

     As an example, suppose the resource template for the DIALER portrait
     view contained the following to specify that a WC_BRANDING should be
     shown:
          CONTROL     "", 23010, "MS_PHONE_BRANDING", 0, 4, 0, 108, 23
     The phone app would search for the configuration as follows:
         1. PHExtGetPhoneControlMetrics API
         2. [HKLM\Security\Phone\Skin\DIALER\portrait\Text]
         3. Use internal default.

   Phone UI Component APIs:
     This API should be used by non-default controls implemented by the phone canvas developer.
     It should be used by the window proc of the non-default control. More complete documentation
     is found elsewhere in this file on the specific API.
     
     This API is exported from Tpcutil.dll.

     // Instruct parent view to paint background for child control.
     PHPaintChildBackground

 ********************************************************************
 ********************************************************************
 ********************************************************************/

     










     //  Accumulator window class.
     #define WC_ACCUMULATOR TEXT("MS_PHONE_ACCUMULATOR")
     // branding window class
     #define WC_BRANDING TEXT("MS_PHONE_BRANDING")
     // calert contact window class
     #define WC_CALERTCONTACT TEXT("MS_PHONE_CALERTCONTACT")
     // caller list window class
     #define WC_CALLERLIST TEXT("MS_PHONE_CALLERLIST")
     //  call state window class
     #define WC_CALLSTATE TEXT("MS_PHONE_CALLSTATE")
     //  caller id info window class
     #define WC_CALLERIDINFO TEXT("MS_PHONE_CALLERIDINFO")
     // static text window class
     #define WC_TEXTUI TEXT("MS_PHONE_TEXT")
     // No sim / invalid sim / phone off  window class
     #define WC_INVALIDPHONE TEXT("MS_PHONE_INVALIDPHONE")
     // incoming call alert background window class (smartphone only)
     #define WC_CALERTBACKGROUND TEXT("MS_VIRTUAL_LIST_VIEW_CE_1.0")
     // SmartDIal window class (Pocket PC only)
     #define WC_SMARTDIAL TEXT("MS_PHONE_SMARTDIAL")
     /*
     Configuration for text components:
     CNTLTYPE: Text
     The following components are of CNTLTYPE Text.
       WC_BRANDING
       WC_CALERTCONTACT
       WC_CALLERLIST
       WC_CALLSTATE
       WC_CALLERIDINFO
       WC_SMARTDIAL
       The following values will be queried from PHExtGetPhoneControlMetrics.  If 
       PHExtGetPhoneControlMetrics returns E_NOTIMPL, these values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Text].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       textColor : Value of type COLORREF that specifies the color of the text.
       textDisabledColor : Value of type COLORREF that specifies the color of the text
           when the component is disabled.
       textFlags : Specifies the alignment of the text; uses the same flags as DrawText,
           for example DT_LEFT, DT_VCENTER, etc.  To use these flags in a registry setting,
           see their values in public\common\sdk\inc\wingdi.h.
       backgroundColor : By default, text will be painted transparently.  If this COLORREF
           is specified, the component's background will have this color.
       The following font settings follow the standard "System Font Registry Settings"
       documented in the Platform Builder and are used to populate a LOGFONT structure.
       Nm : Indicates the font name.  If this value is not specified, the values of Ht, It, Wt, 
           and Cs will not be queried, and the control will use its default font.
       Ht : Indicates the font height.
       It : When set to 1, indicates that the font is italic.
       Wt : Indicates the font weight or boldness.
       CS : Indicates the font character set.

       The font settings (Nm, Ht, It, Wt, and Cs) do not apply to the WC_ACCUMULATOR.

       Additionally, the following value applies to the WC_ACCUMULATOR.       
       AccumulatorHideEmpty: BOOL indicating if the accumulator should be hidden when empty.
       highlightColor : Value of type COLOREF that specifies the color of the selection highlight bar. If not specified, use system highlight color.
       minPixelsFromScreenEdge: Number of pixels we want our accumulator text to be drawn from the left & right edges of the screen. This will be scaled automatically for HIDPI.
       stretchable: BOOL indicating if the accumulator can stretch to the left when the accumulator text doesn’t fit in the control.
       cxStretchToLeft: UINT that specifies the number, in pixels, the left end that the accumulator can stretch. stretchable must be 1.

       The following values apply to the WC_BRANDING
       showDefaultOperator : BOOL indicating if the WC_BRANDING should show the default operator for the next outgoing call. By default, this
           setting is true for DIALER and SMARTDIALER views and false for all other views where the WC_BRANDING displays operator for the incoming
           call or the current active call or for the last call made. When the above does not apply (e.g. failed to obtain PH_ID_LASTCALL_INFO),
           the control displays the default operator.

       The following values apply to the WC_CALERTCONTACT and WC_CALLERIDINFO
       displayContactTitle: BOOL indicating if the incoming call screen and caller id windows should display contact "Title" information
       displayContactCompany: BOOL indicating if the incoming call screen and caller id windows should display contact "Company" information
       majorColor : see Text CNTLTYPE.
       minorColor : see Text CNTLTYPE.
       majorDisabledColor : see Text CNTLTYPE.
       minorDisabledColor : see Text CNTLTYPE.
       majorFlags : see Text CNTLTYPE.
       minorFlags : see Text CNTLTYPE.
       majorNm : see Text CNTLTYPE.
       minorNm : see Text CNTLTYPE.
       majorHt : see Text CNTLTYPE.
       minorHt : see Text CNTLTYPE.
       majorIt : see Text CNTLTYPE.
       minorIt : see Text CNTLTYPE.
       majorWt : see Text CNTLTYPE.
       minorWt : see Text CNTLTYPE.
       majorCS : see Text CNTLTYPE.
       minorCS : see Text CNTLTYPE.

       major indicates the style used to display the most important information in the contact details name if available, otherwise the number. 
       The rest of the information will be displayed using minor style. If minor style is not provided major style will be used to display the information.
       
       The following values apply to the WC_CALLERLIST
       rowHeight: UINT indicating the height in pixels of each row in the WC_CALLERLIST. This is automatically scaled on hidpi resolutions
       showActiveGroup: BOOL indicating if the WC_CALLERLIST should show the active call group. By default, this setting is false and 
             the WC_CALLERLIST displays calls that are on hold
       highlightSelection: BOOL indicating if the WC_CALLERLIST should highlight the selected item. If this flag is set, the control also
             sends WM_NOTIFY messages to its parent window with a NMHDR pointer as LPARAM. The parent window should listen for LVN_ITEMCHANGED
             and LVN_KEYDOWN notifications to keep track of selection changes and key presses.
             Note that if this flag is set, the control can steal focus away from other controls if clicked on or tabbed to
       highlightColor: Value of type COLOREF that specifies the color of the selection highlight bar.
       
       The following values apply to the WC_SMARTDIAL
       textForegroundColor : Value of type COLORREF that specifies the color of the text.
       backgroundColor : By default the background is system background color. If this COLORREF
           is specified, the component's background will have this color.
       highlightColor : Value of type COLORREF that specifies the color of the selection highlight bar.
       highlightTextColor : Value of type COLORREF that specifies the color of the text of the selected bar.
       matchesTextColor : Value of type COLORREF that specifies the color for the text in the matches accumulator
       matchesHighlightColor : Value of type COLORREF that specifies the color of the selection highlight bar for the matches accumulator.                               
       bkTransparent : 1 indicates transparent background, 0 otherwise.
       bkGradient : 1 indicates gradient background, 0 otherwise. bkGradient takes precedence over bkTransparent and backgroundColor.
       allowHide: 1 indicates that the smartdial control can hide itself when a call is in progress and not on hold, 0 otherwise.
       matchAlignment: 1 indicates that switching between views should not change the index of the top item in the listview, if possible
       rowHeight: Override the default smartdial control row height in pixels. This will be scaled for HIDPI.
       matchesHeight: UINT indicating the height in pixels of the matches accumulator. This will be scaled for HIDPI. 
                      If 0, the matches accumulator is not displayed. 
                      NOTE: matchesHeight reduces the effective size of the listview, and thus an adjustment to the rowHeight setting might be necessary.
                            i.e If N is the height of the WC_SMARTDIAL control, then the height of the listview will be N - matchesHeight, and 
                            an appropriate rowHeight will be the integer result of (N - matchesHeight) / (number of desired rows)
       
       Finally, for WC_TEXTUI, the following values are supported.
       textValue : The string that should be displayed.
     */
     




     
     
     
     
     
       // button window class (Pocket PC only)
       #define WC_BUTTON TEXT("MS_PHONE_BUTTON")
       /*
       Configuration for button components:
       CNTLTYPE: PlainButton
       Any component created from the WC_BUTTON class that is not a CNTLTYPE
       TalkEndButton, EraseButton, VerbButton, or DialButton, is a PlainButton
       CNTLTYPE.  See the sections for these control types for availability of 
       and variations on the settings listed below.  The button components fully
       support text configuration as documented with the Text CNTLTYPE.
       The following values and the values documented with the Text CNTLTYPE
       will be queried from PHExtGetPhoneControlMetrics.  If 
       PHExtGetPhoneControlMetrics returns E_NOTIMPL, these values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\PlainButton].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.

       backgroundColorNormal : If no bitmaps are assigned, this COLORREF gives the
           opaque background color painted for the component.
       backgroundColorPressed : If no bitmaps are assigned, this COLORREF gives the
           opaque background color painted for the component when pressed.
       backgroundColorDisabled : If no bitmaps are assigned, this COLORREF gives the
           opaque background color painted for the component when disabled.

       To assign no bitmap for a state, set the corresponding bitmap path to an empty string.
       Otherwise the default Microsoft bitmap will be used.
       
       bmpNormal : Specifies the file path for the image of  
           the button in its normal state.
       bmpPressed : Specifies the file path for the image of  
           the button in its pressed state.
       bmpDisabled : Specifies the file path for the image of  
           the button in its disabled state.
       bmpGlyph : Specifies the file path for an overlay glyph that will be drawn over
           the button when there is room.  A glyph positioning flag must be specified
           in bmpFlags in order for a glyph to be used.
       bmpFlags : These flags control the alignment of the bmp within the button.*/
           const DWORD BMF_HMASK                   = 0x0000000F;
           const DWORD BMF_HALIGN_LEFT             = 0x00000000; // Align the bitmap to the left edge
           const DWORD BMF_HALIGN_CENTER           = 0x00000001; // Align the bitmap to the center
           const DWORD BMF_HALIGN_RIGHT            = 0x00000002; // Align the bitmap to the right edge
           const DWORD BMF_HALIGN_TILE             = 0x00000003; // Tile the bitmap, aligned to the left edge

           const DWORD BMF_H2VSHIFT                = 4; // Number of bits to shift the vertical flags to be equivalent to the horizontal flags

           const DWORD BMF_VMASK                   = 0x000000F0;
           const DWORD BMF_VALIGN_TOP              = 0x00000000; // Align the bitmap to the top edge
           const DWORD BMF_VALIGN_CENTER           = 0x00000010; // Align the bitmap to the center
           const DWORD BMF_VALIGN_BOTTOM           = 0x00000020; // Align the bitmap to the bottom edge
           const DWORD BMF_VALIGN_TILE             = 0x00000030; // Tile the bitmap, aligned to the top edge

           const DWORD BMF_GLYPH_MASK              = 0x00000F00; // These are not alignment flags and only affect positioning relative to text
           const DWORD BMF_GLYPH_POS_LEFT          = 0x00000100; // Draw the glyph to the left of the text.
           const DWORD BMF_GLYPH_POS_RIGHT         = 0x00000200; // Draw the glyph to the right of the text.
           const DWORD BMF_GLYPH_POS_NOTEXT        = 0x00000300; // Draw the glyph with no text (necessary when no text is specified).
       /*         
       bmpTransparency : This flag determines which color in the given control 
           appears transparent against the dialog background.  If the value of
           the flag maps to one of the following constants, we look at the color
           of that pixel to determine the color that will be transparent.  If
           the value does not map to the transparency constants, the value is
           read as a COLORREF value which defines the color of the transparency.
           To explicitly disable any transparency, set this value to CLR_INVALID
           (see wingdi.h).
           */
           const COLORREF BMT_TOPLEFTPIXEL         = 0x01000000; // Use the top-left pixel in the bitmap as the transparent color
           const COLORREF BMT_TOPRIGHTPIXEL        = 0x02000000; // Use the top-right pixel in the bitmap as the transparent color
           const COLORREF BMT_BOTTOMLEFTPIXEL      = 0x03000000; // Use the bottom-left pixel in the bitmap as the transparent color
           const COLORREF BMT_BOTTOMRIGHTPIXEL     = 0x04000000; // Use the bottom-right pixel in the bitmap as the transparent color












       /*
       CNTLTYPE: DialButton
       Any component created from the WC_BUTTON class with one of the
       following id's is a DialButton.  DialButton's are used to represent the
       touch screen key pad buttons for dialing 0-9, * and #.
       */
           #define IDC_DIALBUTTON_ONE   (23041) // 1
           #define IDC_DIALBUTTON_TWO   (23042) // 2abc
           #define IDC_DIALBUTTON_THREE (23043) // 3def
           #define IDC_DIALBUTTON_FOUR  (23044) // 4ghi
           #define IDC_DIALBUTTON_FIVE  (23045) // 5jkl
           #define IDC_DIALBUTTON_SIX   (23046) // 6mno
           #define IDC_DIALBUTTON_SEVEN (23047) // 7pqrs
           #define IDC_DIALBUTTON_EIGHT (23048) // 8tuv
           #define IDC_DIALBUTTON_NINE  (23049) // 9wxyz
           #define IDC_DIALBUTTON_ZERO  (23050) // 0 or 0+
           #define IDC_DIALBUTTON_STAR  (23051) // *
           #define IDC_DIALBUTTON_POUND (23052) // #
       /*
       The DialButton components fully support button configuration as documented with 
       the PlainButton CNTLTYPE.  In addition, the following behavior can be customized.
       The text on each DialButton is documented above.  For example, the
       IDC_DIALBUTTON_ONE has the text "1" and the IDC_DIALBUTTON_TWO has the
       text "2abc."  The first character of text, 0-9, * or # is known as the
       majorText and the rest of the characters are known as the minorText.
       For example, the IDC_DIALBUTTON_ONE has the major text "1" and no minor
       text.  The IDC_DIALBUTTON_TWO has the major text "2" and the minor text
       "abc."  All of the text configuration for a Text CNTLTYPE applies to
       both the major and minor text.
       The following values and the values documented with the PlainButton CNTLTYPE
       will be queried from PHExtGetPhoneControlMetrics.  If 
       PHExtGetPhoneControlMetrics returns E_NOTIMPL, these values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\DialButton].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.

       majorColor : see Text CNTLTYPE.
       minorColor : see Text CNTLTYPE.
       majorDisabledColor : see Text CNTLTYPE.
       minorDisabledColor : see Text CNTLTYPE.
       majorFlags : see Text CNTLTYPE.
       minorFlags : see Text CNTLTYPE.  The horizontal alignment flags must
                    match the majorFlags horizontal alignment flags.
       majorNm : see Text CNTLTYPE.
       minorNm : see Text CNTLTYPE.
       majorHt : see Text CNTLTYPE.
       minorHt : see Text CNTLTYPE.
       majorIt : see Text CNTLTYPE.
       minorIt : see Text CNTLTYPE.
       majorWt : see Text CNTLTYPE.
       minorWt : see Text CNTLTYPE.
       majorCS : see Text CNTLTYPE.
       minorCS : see Text CNTLTYPE.
       textLayoutFlags: These flags control the layout of the major and minor text: 
       */
           const DWORD TLF_SWAPMAJORMINOR          = 0x00000001; // Swap major and minor text, putting major text on the right
           const DWORD TLF_NOMINORTEXT             = 0x00000002; // Omit the minor text and draw only the major text
       /*

       










       CNTLTYPE: VerbButton
       Any component created from the WC_BUTTON class with one of the
       following id's is a VerbButton.  VerbButton's are used to represent
       the various PH_VERBs or combinations of PH_VERBs.
       */
           #define IDC_HISTORY           (23011) // switches to the call history PH_VIEW
           #define IDC_HISTORY_SPEAKER   (23016) // toggles between speaker phone on, speaker phone off and call history
           #define IDC_SPEED             (23012) // switches to the speed dial PH_VIEW
           #define IDC_HOLDSWAPTRANSFER  (23013) // GSM only: toggles between hold, unhold, transfer and swap
           #define IDC_NOTEPAD           (23015) // creates a new note.
           #define IDC_CONTACTS          (23027) // switches to contacts
           #define IDC_SPEAKER           (23028) // toggles between speaker phone on and off.
           #define IDC_END               (23029) // end button.
           #define IDC_TALK              (23030) // talk button.
           #define IDC_MUTE              (23031) // toggles between mute and unmute.
           #define IDC_MUTE_SAVE       (23054) // toggles between mute, unmute, and save to contacts at the end of a call
       /*
       The VerbButton components fully support button configuration as documented with 
       the PlainButton CNTLTYPE.  They also fully support text configuration as documented
       with the Text CNTLTYPE.
       The values documented with the PlainButton and Text CNTLTYPE
       will be queried from PHExtGetPhoneControlMetrics.  If 
       PHExtGetPhoneControlMetrics returns E_NOTIMPL, these values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\VerbButton].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
     










       CNTLTYPE: EraseButton
       Any component created from the WC_BUTTON class with one of the
       following id's is a EraseButton.  EraseButton's are used to represent
       PH_VERB_REMOVE_ACCUMULATOR verb.
       */
           #define IDC_ERASE_BUTTON   (23001) // erases characters from the accumulator
       /*
       The EraseButton is a specialized VerbButton that supports using a different
       bitmap from the other VerbButtons.  As such, it is configured just like a
       VerbButton.  If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the configuration values 
       will be queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\EraseButton].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       










       CNTLTYPE: TalkEndButton
       Any component created from the WC_BUTTON class with one of the
       following id's is a TalkEndButton.  TalkEndButton's are used to toggle
       between talk and end.  When a call is active, the button represents the
       End verb.  Otherwise, it represents the Talk verb.
       */
           #define IDC_TALKEND        (23014) // toggles between talk and end.
       /*
       The TalkEndButton is a specialized VerbButton that supports using a different
       bitmap for the Talk and End states.  As such, it is configured just like a
       VerbButton with the below exceptions.
       If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the configuration values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\TalkEndButton].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       
       backgroundColorNormalTalk   : see PlainButton CNTLTYPE.
       backgroundColorNormalEnd    : see PlainButton CNTLTYPE.
       backgroundColorPressedTalk  : see PlainButton CNTLTYPE.
       backgroundColorPressedEnd   : see PlainButton CNTLTYPE.
       backgroundColorDisabledTalk : see PlainButton CNTLTYPE.
       backgroundColorDisabledEnd  : see PlainButton CNTLTYPE.
       bmpNormalTalk       : see PlainButton CNTLTYPE.
       bmpNormalEnd        : see PlainButton CNTLTYPE.
       bmpPressedTalk      : see PlainButton CNTLTYPE.
       bmpPressedEnd       : see PlainButton CNTLTYPE.
       bmpDisabledTalk     : see PlainButton CNTLTYPE.
       bmpDisabledEnd      : see PlainButton CNTLTYPE.
       bmpGlyphTalk        : see PlainButton CNTLTYPE.
       bmpGlyphEnd         : see PlainButton CNTLTYPE.
       bmpFlagsTalk        : see PlainButton CNTLTYPE.
       bmpFlagsEnd         : see PlainButton CNTLTYPE.
       bmpTransparencyTalk : see PlainButton CNTLTYPE.
       bmpTransparencyEnd  : see PlainButton CNTLTYPE.
       */











       // Background images on the views don't need a window class.
       /*
       CNTLTYPE: Other
       The default background image of the PH_VIEW_PROGRESS and PH_VIEW_DIALER 
       can each be configured as follows. These are the only views
       whose default background image can be configured, except on Smartphone (see below).
       PHExtGetPhoneControlMetrics will not be used to get this info.
       Instead, the phone application will examine the registry key,
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Other],
       for the bmp configuration as documented in PlainButton.
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.

       On Smartphone only, the PH_VIEW_INCOMING view's background can 
       be customized.  It supports the same customization as other RAI applications.
       This is used to support end user theming on the device.
       For more information, please reference the AK documentation.

       Its settings are read from the reg key below.  For example, in order to use
       the background mybackground.gif, it would be set as follows:
       [HKLM\Software\Microsoft\Shell\RAI\:CallAlert]
           "BKBitmapFile"="\\windows\\mybackground.gif"

       In order to disable this customization (for instance if the customer does
       not want important branding in the incoming call view removed by a skin installed
       by an end user), the setting found in the reg key below can be set to prevent
       RAI customization in the incoming call slip.
       
       [HKLM\Security\Phone\Skin\INCOMING\<ORIENTATION>\Other]
           "disableBackground"=dword:1

       Furthermore, if the OEM wishes to specify their own background, they can use the
       setting "useOEMRAI".  Then the RAI settings for the call alert dialog will be read
       from [HKLM\Software\Microsoft\Shell\RAI\:OEMCallAlert]. For example, if the OEM wanted
       to use their own background file and disable theming, they would specify

       [HKLM\Security\Phone\Skin\INCOMING\<ORIENTATION>\Other]
            "useOEMRAI"=dword:1

       [HKLM\Software\Microsoft\Shell\RAI\:OEMCallAlert]
           "BKBitmapFile"="\\windows\\OEMbackground.gif"     
      
       For the PH_VIEW_PRIV_CALL, the background color can be configured. The phone application
       will examine the above registry key for the backgroundColor configuration as documented in
       PlainButton. This is the only view whose background color can be configured


       The following flag and values are specific to the PH_VIEW_DIALER (PPC only)
       If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the configuration values will be
       queried under the registry key
       [HKLM\Security\Phone\Skin\PH_VIEW_DIALER\<ORIENTATION>\Other].
       The values for <ORIENTATION> are specified elsewhere.
       dialpadFlags: Toggles switching between PH_VIEW_DIALER and PH_VIEW_PROGRESS (call progress) at the start of a new call.
                     The flag can be either PH_DIALPAD_DEFAULT, PH_DIALPAD_ALWAYS_UP or PH_DIALPAD_ALWAYS_DOWN                                    
       */
       #define PH_DIALPAD_DEFAULT       0     // Always switch to call progress unless overriden by PH_CALLBEHAVIORFLAGS or the call is to voicemail, an emergency number, or an IOTA number
       #define PH_DIALPAD_ALWAYS_UP     1     // Never switch to call progress irrespective of PH_CALLBEHAVIORFLAGS 
       #define PH_DIALPAD_ALWAYS_DOWN   2     // Always switch to call progress irrespective of PH_CALLBEHAVIORFLAGS
       









       // elapsed time window class
       #define WC_ELAPSEDTIME TEXT("MS_PHONE_ELAPSEDTIME")
       /*
       CNTLTYPE: Text
       Any component created from the WC_ELAPSEDTIME class is simply a Text
       CNTLTYPE and can be configured as documented there.  However, the
       frequency of its update can be configured with the following
       registry key.  Note this key applies to all instances of this
       component.
       [HKLM\Security\Phone\Skin]
       refreshInterval : Time in milliseconds between updates of the elapsed
                         time of active calls.
       
     
       */
       










       // Status Icons window class.
       #define WC_STATUSICONS TEXT("MS_PHONE_STATUSICONS")
       /*
       CNTLTYPE: Other
       Any component created from the WC_STATUSICONS class is a CNTLTYPE
       Other.  If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the
       configuration values will be queried under the registry key:
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Other].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       The bit flags documented above as PH_ICON_ represent the icons that the
       icons component can display.  To supply a different bitmap for these
       icons, see Section 4.
       
       Icons : Bit field of PH_ICON_ flags that determine which icons will be
               displayed based on the PH_ID_ICON_STATE.
       IconColor: If this COLORREF is specified and the icon strip is monochrome, the icons
                  will be drawn in this color instead of the default from the image.
       mainControl  : Specifies the main control id. If the main control is visible, hide the WC_STATUSICONS and the WC_CONTACTPICTURE.
                      This behavior can overridden in PHExtGetPhoneControlMetrics
       
       */
       










       //  the SolidLine window class
       #define WC_SOLIDLINE TEXT("MS_PHONE_SOLIDLINE")
       /*
       CNTLTYPE: Other
       Any component created from the MS_PHONE_SOLIDLINE class is a
       CNTLTYPE Other.  If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the
       configuration values will be queried under the registry key:
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Other].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       SolidLineColor : If this COLORREF is specified, the line will be 
                        this color.
       
       */
       










       // contact picture window class
       #define WC_CONTACTPICTURE TEXT("MS_PHONE_CONTACTPICTURE")
       /*
       CNTLTYPE: Other
       Any component created from the WC_CONTACTPICTURE class is a
       CNTLTYPE Other.  If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the
       configuration values will be queried under the registry key:
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Other].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       showIncoming : If this BOOLEAN is true, the control will display an
                      incoming call if possible.  If not possible or if
                      showIncoming is false, it will display the current call.
       defaultImage : Specifies the path to load the default image. If this string 
                      value is not empty, the control will show this image as the
                      default picture for calls that dont have a picture 
                      associated. To disable this feature set the value to an empty string.
       showBorder : If this BOOLEAN is true, the control will display a 1 pixel border 
                    around the picture.
       borderColor : Specifies the border color. This value is only valid if showBorder 
                     is set to 1. If showBorder is but borderColor is not set, a 
                     default color will be used.
       mainControl  : Specifies the main control id. If the main control is visible, hide the WC_STATUSICONS and the WC_CONTACTPICTURE.
                      This behavior can overridden in PHExtGetPhoneControlMetrics
       */    
       










       // LayoutHelper window class
       #define WC_LAYOUTHELPER TEXT("MS_PHONE_LAYOUTHELPER")
       /*
       CNTLTYPE: Other
       Any component created from the WC_LAYOUTHELPER class is a
       CNTLTYPE Other.  Used to resize or move one to four components
       based on the visiblity of an adjacent component.  For example,
       in the default views provided by MSFT, this control is used to resize the
       WC_CALLERIDINFO when the WC_CONTACTPICTURE is shown.  Also, it moves the
       elapsed time control and regulates the visibility state of WC_INVALIDPHONE over 
       WC_BRANDING, WC_STATUSICONS, WC_CALLSTATE, WC_ELAPSEDTIME and 
       WC_CALLERIDINFO.
       If PHExtGetPhoneControlMetrics returns E_NOTIMPL, the
       configuration values will be queried under the registry key:
       [HKLM\Security\Phone\Skin\<PHONE_VIEW>\<ORIENTATION>\Other].
       The values for <PHONE_VIEW> and <ORIENTATION> are specified elsewhere.
       mainControl : This is the id of the control whose visibility determines
                     the placement of the buddy controls.
       buddyControl0 : Id of a control which should move or resize to use the
                       area vacated by the mainControl when it's invisible.
       buddyControl1 : Same as buddyControl0 but specifies another control id.
       buddyControl2 : Same as buddyControl0 but specifies another control id.
       buddyControl3 : Same as buddyControl0 but specifies another control id.
       buddyControl4 : Same as buddyControl0 but specifies another control id.
       buddyControl5 : Same as buddyControl0 but specifies another control id.
       */
            //Layout helper flags for buddy alignment
            #define LHF_LEFT        1       // Buddy control is on left  side of main control
            #define LHF_RIGHT       2       // Buddy control is on right side of main control
            #define LHF_STRETCH     4       // When main control hides, stretch the buddy to fill the space
            #define LHF_MOVE        8       // When main control hides, move the control to an edge of main control.
            #define LHF_HIDE       16       // When main control hides, show this control. When the main control shows itself, hide this control. The other LHF flags cannot be specified when LHF_HIDE is used.
            #define LHF_NOTIFY     32       // When main control hides or shows itself, send a WM_PH_LAYOUTHELPER_NOTIFY message to the buddy control. Should not be used with any other LHF flags
            #define LHF_TOP        64       // Buddy control is on top  side of main control
            #define LHF_BOTTOM    128       // Buddy control is on bottom side of main control
                                            
       /*
       buddyControlFlags0 : A combo of the above flags directing how the LayoutHelper
                            should treat buddyControl0 when the main control's
                            visibility changes.
       buddyControlFlags1 : Same as buddyControlFlags0 but specific to buddyControl1
       buddyControlFlags2 : Same as buddyControlFlags0 but specific to buddyControl2
       buddyControlFlags3 : Same as buddyControlFlags0 but specific to buddyControl3
       buddyControlFlags4 : Same as buddyControlFlags0 but specific to buddyControl4
       buddyControlFlags5 : Same as buddyControlFlags0 but specific to buddyControl5
       */
       

/********************************************************************
  Phone Layout Helper Notification

  Controls that are buddied to a mainControl via a layouthelper using the LHF_NOTIFY flag
  should listen to this message to be notified of changes to the mainControl's visibility

 WPARAM - control id of the main control
 LPARAM - visibility state of mainControl - 0 == hidden, 1 == visible

*********************************************************************/
#define WM_PH_LAYOUTHELPER_NOTIFY (WM_USER + 2)




/********************************************************************
    WM_PH_PAINTCHILDBACKGROUND

    Purpose:
        Paints the background of a child window.  
        
    WPARAM - hwnd of child window
    LPARAM - DC to draw to
    return value - A view dialog should SetWindowLong to the DWL_MSGRESULT
    if it processes this message and draws the child's background.
 ********************************************************************/
#define WM_PH_PAINTCHILDBACKGROUND   (WM_USER + 3)





/*
    PHPaintChildBackground: 

    Instructs the view dialog to draw the background for a control.
    If a non-default Phone UI component is specified in a dialog template 
    and the phone canvas developer wishes to draw the background for that control, it
    should call PHPaintChildBackground.

    Calling this function will send a message to the view dialog
    specified, sending a WM_PH_PAINTCHILDBACKGROUND, with a WPARAM of 
    the hwnd of the child control, and an LPARAM of the HDC to paint to.

    [in]  hwndControl     : HWND of the child control whose background should be painted.
    [in]  hwndParentView  : HWND of the view dialog who should do the painting.  Note:
                            not all view dialogs support this by default.  Only DIALER and
                            PROGRESS have backgrounds to paint, thus only those two are valid.  
    [in]  hdcChild        : HDC of the child window to paint.
    
    returns:
    S_OK : success
    E_FAIL : failure.  Not all view dialogs support drawing background.
    RPC_E_WRONG_THREAD : Must be called from the main UI thread.  All of the
                         PHExt* API's are called on the main UI thread.
*/
HRESULT PHPaintChildBackground(HWND hwndControl, HWND hwndParentView, HDC hdcChild);







/********************************************************************
 ********************************************************************
 ********************************************************************
    Section 4:
    Miscellaneous Items:  This section documents other phone extension
    mechanisms.

    1.  Replacing other bitmaps.
    2.  Adding custom bitmaps to the call log for specific calls.
    3.  Menu extensions
    4.  Helper Objects
    5.  Overriding the default dialing sequence.
    6.  Override speed dial execution.
    7.  Changing the emergency call list.
    8.  Preventing IOTA calls from being added to the call log.
    9.  Changing the default CDMA voice mail number.
   10.  Preventing the phone application from coming to the foreground when a call is
        dialed.
   11.  Turning the screen on for incoming calls and preventing the CPU from sleeping
        during active calls.
   12.  Configuring the phone number and caller-id look-up algorithm.
   13.  Configuring the phone for CDMA, GSM and optional features.
   14.  Supporting multi-lines.
   15.  Setting ring tones.
   16.  Setting a voicemail error message (used by PH_MSG_ERRORNOVMAILNUMBER)
   17.  Auto-Prefix Dialing
   18.  Activating Speed Dial On Talk
   19.  Controlling Network settings UI for manual and preferred network selection
   20.  Disabling the Edit Before Call dialog box
   21.  Disabling "Fixed Dialing" option in services settings in control panel (PPC only)
   22.  Turning off matches accumulator in smartdial
   23.  Controlling default keypad state
   24.  Turning off SIP button in full screen smartdial's menu bar
   25.  Handling '+' for international dialing
   26.  Adding "Go to Contacts" menu item to PPC smartdial's SK2 menu
   27.  Adding a custom ringtone directory
   28.  Enabling roaming icons for call history 
   29.  Controlling the handling of special characters in dial strings
   30. Writing a non-callpol voice call information into the call log database
 ********************************************************************
 ********************************************************************
 ********************************************************************/

/*
    1. Replacing other bitmaps:
       The bitmaps for the call log, speed dial, SIM Security and the
       WC_STATUSICONS control can be replaced using the
       standard bitmap replacement mechanisms supported elsewhere.
       See public\ossvcs\oak\inc\OverBmp.h. 

       Note that the button bitmaps for SIM Security (IDB_OVERRIDE_SIMSEC_*)
       are assumed to contain a transparent area. The upper left pixel of the 
       bitmap determines the RGB value of all pixels in the bitmap which 
       are drawn transparently.
*/


/*
    2.  Adding custom bitmaps to the call log for specific calls.  Calls in
        the call log may be marked with the LGIF_OCUSTOMICO bit to cause the
        call log to use a different bitmap for that call.  See 
        public\ossvcs\oak\inc\OverBmp.h to specify the bitmap.
*/

/*
    3. Menu extensions.
       The call log, speed dial, smart dial, and phone application all provide
       menu extensions.  The phone application does not support an app
       abstraction. However, the other applications support the IDataObject
       as their app extension.  They return an ItemRefArray containing
       an ItemRef for the selected object.  The ItemRef contains the CEOID and 
       CEPROPID for the selected object. See public\apps\oak\samples\MenuExt for 
       sample code illustrating how to add menu extensions and use ItemRef's.
       ItemRef's are defined in public\shellw\oak\inc\appext.h.  The supported
       ITEM_TYPE_ID's are listed below. (for ITI_ContactItemRef and ITI_PimItemRef 
       please refer to public\apps\sdk\inc\pimstore.h)
*/

// {72D7D1D4-EC08-4a74-B2E3-0C98C4EF6E14}
static const GUID  ITI_CallHistoryItemRef = {0x72d7d1d4, 0xec08, 0x4a74, {0xb2, 0xe3, 0xc, 0x98, 0xc4, 0xef, 0x6e, 0x14}};

// {518BDDC9-77A3-436c-BD53-A3D9B2ECABF7}
static const GUID ITI_SpeedDialItemRef = {0x518bddc9, 0x77a3, 0x436c, {0xbd, 0x53, 0xa3, 0xd9, 0xb2, 0xec, 0xab, 0xf7}};

// {0E91ED23-A962-4246-B53F-22984B5A73E6}
static const GUID ITI_SimItemRef = {0xe91ed23, 0xa962, 0x4246, {0xb5, 0x3f, 0x22, 0x98, 0x4b, 0x5a, 0x73, 0xe6}};



/*
    4. Helper Objects
       The speed dial and the Pocket PC settings provide their top-level hwnd
       to app extension helper objects.  These app extension COM objects must
       implement IObjectWithSite.  The following registry keys must be set
       to enable the creation of the app extensions.
       [HKLM\System\<Class>\Browser Helper Objects\<CLSID>]       
       <Class> can be PhoneSettings or MSScut.  (MSScut is for speed dial.)
       <CLSID> is the Class ID of the extension, as registered for the COM server.

       The application will set the app abstraction to be the site of the app
       extension using IObjectWithSite::SetSite().  This site can be QueryInterface'd
       for an IID_IOleWindow interface.  The IID_IOleWindow interface will return
       the top-level hwnd for the application via IOleWindow::GetWindow().
       In both cases, the browser helper objects are created BEFORE the top-level hwnd receives its WM_ACTIVATE.
       For MSScut, this happens right after WM_CREATE and for PhoneSettings, during WM_INITDIALOG.        
       Sample code implementing a helper object app extension can be found in
       public\apps\oak\samples\phonebho.
*/

/*
    5. Overriding the default dialing sequence.
       As every digit is entered into the phone app, the entire dial string is
       parsed to determine if special action should be taken immediately.  Also whenever
       any phone number is dialed, the dial string is parsed to determine if special
       action is necessary.  This dial parser functionality is used to enable
       GSM supplementary services and CDMA akey functionality.  The default dial
       parsing provided by MSFT may be overridden.  See public\ossvcs\oak\inc\dpext.h
       for details.
*/

/*
    6. Override speed dial execution.
       The execution of speed dials can be modified.
       See shellw\oak\inc\phonep.h for details.
*/

/*
    7. Changing the emergency call list.
       The list of numbers considered to be emergency dial strings is stored in the
       registry
       [HKLM\Security\ECall]
       "List"=multi_sz:""
       For GSM, 112 and the numbers from the SIM are added to this list by default.
        
    8. Preventing IOTA calls from being added to the call log.
       Calls to the numbers in the following registry key will not be added to the
       call log.
       [HKLM\Security\Phone\NoLog]
       "List"=multi_sz:""
        
    9. Changing the default CDMA voice mail number.
       For CDMA, the default voice mail number will be loaded from the following
       registry key.  If this key doesn't exist, the device's phone number will
       be used.
       [HKCU\Software\Microsoft\Vmail]
       "CarrierProviderNumber1"=""

   10. Preventing the phone application from coming to the foreground when a call is
       dialed.
       PHMakeCallEx is a PPC-only version of PhoneMakeCall which allows you to pass
       in a flag that will suppress the phone application from coming to the foreground
       when the call is dialed.  PHMakeCallEx is exported from tpcutil.dll.
*/
// PhoneMakeCall flags
#define PMCF_DEFAULT                0x00000001
#define PMCF_PROMPTBEFORECALLING    0x00000002
#define PMCF_EDITBEFORECALLING      0x00000020
#define PMCF_ALLOWSUPSVCS           0x00000040  //Allows execution of supsvcs commands like MMI
#define PMCF_NOPROGRESSINFOONCALL   0x00010000  // prevents the phone application from appearing
                                                // at the start of this call.


LONG PHMakeCallEx(PHONEMAKECALLINFO *ppmci);

/*
   11. Turning the screen on for incoming calls and preventing the CPU from sleeping
       during active calls.  The phone app will automatically turn the screen on for
       incoming calls.  To disable this behavior, set the following reg key.
       [HKCU\ControlPanel\Phone]
       "TurnScreenOn"=dword:0

       The phone app will automatically prevent the CPU from sleeping by calling 
       SystemIdleTimerReset periodically.  To disable this behavior, set the following
       reg key.
       [HKCU\ControlPanel\Phone]
       "Sleep"=dword:1
       
        

*/

/*
   12.  Configuring the phone number and caller-id look-up algorithm.
        When receiving or making a call, the phone application will try to match
        the phone number to a contact in the contacts database.  Here's a high
        level description of the caller-id lookup.

        Step 1 : Get the name, if possible.
        Step 1a: For incoming calls:
                 The name supplied by RIL in the wszDescription field of the RILCALLINFO 
                 structure returned by RIL_GetCallList will be used.
        Step 1b: For outgoing calls:
                 The name specified by the API used to place the call will be used.
                 Ex: The lpszCalledParty parameter to tapiRequestMakeCall.
        Step 1c: If there is no such name, check to see if this number matches any
                 number in the emergency dialing list.  Use the special "Emergency"
                 string if it does.

        Step 2 : Search contacts.
        Step 2a: Search for a contact with a PIMPR_FILEAS property that matches
                 the name from step 1.  If the phone number in step 1 matches
                 a phone number for this contact, use this contact.
        Step 2b: If step 2a fails, search for the phone number in contacts 
                 using the POOM API FindMatchingContact.
        Step 2c: If step 2b fails, check to see if the number matches the voice 
                 mail number.  If it does, use the special "Voicemail" string.

                 Final Result
        Step 3 : Results from step 2 will replace results from step 1.
        
        Note that the results of the POOM API FindMatchingContact can be configured
        as below.  FindMatchingContact considers phone numbers whose trailing 6 digits
        match to be match candidates.  The number of trailing digits that must match
        can be configured with the below registry key.
        [HKCU\ControlPanel\Phone]
        "CallIDMatch"=dword:5    ;Lower the matching default to 5 trailing digits
*/

/*
   13.  Configuring the phone for CDMA, GSM and optional features.
        
        [HKEY_CURRENT_USER\ControlPanel\Phone]
           "Features"=dword:0002B820  ; Suggested defaults for CDMA
           "Features"=dword:0206C7D8  ; Suggested defaults for GSM

        [HKEY_LOCAL_MACHINE\Security\Phone]
           "Features"=dword:00000000  ; Suggested defaults for CDMA
           "Features"=dword:00000018  ; Suggested defaults for GSM

        The above registry keys are a set of bit flags representing support
        for various CDMA, GSM and other features.  This registry value must
        be specified in platform.reg.  See below for documentation of each
        bit flag.  The first registry key, [HKEY_CURRENT_USER\ControlPanel\Phone],
        supports all bits below except for SHDFL_PHONE_LOCKFACILITY and 
        SHDFL_PHONE_SIMSUPPORT.        

        The second registry key only supports the SHDFL_PHONE_SIMSUPPORT and 
        SHDFL_PHONE_LOCKFACILITY bits currently. They need to be set there for 
        phones that use a SIM and support PIN locking.  It is statically read at boot.

        These first few flags are dynamically generated by MSFT software during boot.  Do not
        set them.  These flags are not stored in or read from the registry.
        #define SHDFL_NOPHONE                         0x00000000
        #define SHDFL_PHONE                           0x00000001
        #define SHDFL_PHONE_RADIOPRESENT              0x00000002
        #define SHDFL_PHONE_RADIOON                   0x00000004
        #define SHDFL_PHONE_SUPPORTMULTILINES         0x00010000 : This flag is not used from or stored in the registry.  See topic 14.  Supporting multi-lines.


        These flags are statically read from the registry:

        #define SHDFL_PHONE_LOCKFACILITY              0x00000008 : The radio supports PIN locking.  SIM Security UI should be shown.
        #define SHDFL_PHONE_SIMSUPPORT                0x00000010 : The radio supports a SIM.  UI presenting SIM features should be shown.
        #define SHDFL_PHONE_VOICEDIALCONNECTS         0x00000020 : Voice calls are considered connected as soon as RIL_Dial succeeds.
                                                                 : If this bit is cleared, calls will not be considered connected
                                                                 : until RIL_GetCallList shows that this call is in the
                                                                 : RIL_CALLSTAT_ACTIVE state.
        #define SHDFL_PHONE_UNIQUECALLTRACKING        0x00000040 : Implies that RIL_GetCallList report the state of each connected
                                                                 : call and the UI should reflect this state.  In other words,
                                                                 : GSM call state semantics are implied.  Clearing this bit
                                                                 : implies CDMA call state semantics.
        #define SHDFL_PHONE_AUTOTHREEWAYCALLING       0x00000080 : When set,   dialing a second call will result in a call to RIL_Dial.
                                                                 : When clear, dialing a second call will result in a call to RIL_Flash.
        #define SHDFL_PHONE_INTLPLUS                  0x00000100 : The radio/network supports dialing '+' for international calls.  If not set
                                                                 : any leading '+' will be translated to the correct dialing prefix (if locale 
                                                                 : information is available).
        #define SHDFL_PHONE_SMSUNICODE                0x00000200 : If set, sending SMS in Unicode format is supported.  If clear,
                                                                 : SMS Unicode UI will be hidden.
        #define SHDFL_PHONE_0340SMS                   0x00000400 : Use the GSM SMS standard to send SMS.
        #define SHDFL_PHONE_SUPPORTVOICEPRIVACY       0x00000800 : Voice privacy UI should be shown.
        #define SHDFL_PHONE_AKEY                      0x00001000 : A-Key UI should be shown.
        #define SHDFL_PHONE_IS637SMS                  0x00002000 : Use the CDMA SMS standard to send SMS.
        #define SHDFL_PHONE_SUPPORTGPRS               0x00004000 : GPRS UI should be shown.
        #define SHDFL_PHONE_SUPPORTMOSMS              0x00008000 : The ability to send SMS should be shown in the UI.
        #define SHDFL_PHONE_SUPPORTSPEAKERPHONE       0x00020000 : Speakerphone UI should be shown.  A driver supporting
                                                                 : IOCTL_SPEAKER_GETMODE and IOCTL_SPEAKER_SETMODE such as
                                                                 : the sample driver in public\ossvcs\oak\drivers\SpeakerPhone exists.
        #define SHDFL_PHONE_SUPPORTWCMP               0x00040000 : Indicates that WCMP is supported over the SMS transport.
        #define SHDFL_PHONE_SUPPORTAUTOPREFIX         0x00080000 : Turns on Auto-Prefix Number dialing. See topic 17
        #define SHDFL_PHONE_SUPPORTINTERNATIONALCODE  0x00100000 : Press and hold "0" in dialer prepopulates it with international dialing code
        #define SHDFL_PHONE_SUPPORTLOCATIONICONS      0x01000000 : Locations icons should be shown based on location status as
                                                                 : reported by the RIL_NOTIFY_LOCATION notification and the
                                                                 : RIL_GetCurrentLocationStatus API.
        #define SHDFL_PHONE_SUPPORT0230DIALPARSING    0x02000000 : GSM supplementary service codes should parsed as specifed by the
                                                                 : ETSI standard.
*/

/*
   14.  Supporting multi-lines.
        In magneto, multi-line support is provided for Smartphone only.  The following
        registry key is checked.  If the registry doesn't exist, we'll default to showing 
        the multi-line UI if the SIM indicates that ALS is supported.
        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "ML"=dword:0           ; Show multi-line UI if the SIM indicates that ALS is supported.
        "ML"=dword:1           ; Show multi-line UI.  Ignore the ALS information on the SIM.
        "ML"=dword:2           ; Hide multi-line UI.  Ignore the ALS information on the SIM.

        Additionally, the following RIL API's must be supported: RIL_SetCurrentAddressId,
        RIL_GetCurrentAddressId, and RIL_GetSubscriberNumbers.  For incoming calls, the 
        line can be specified in the dwAddressId parameter of RILRINGINFO and RILCALLWAITINGINFO.

        
        Presuming the above regkey has been set to enable multi-line UI, 
        we also can also toggle off and on certain multi-line capabilities
        The following reg key is checked. If it doesn't exist, we default to ALL multi-line capabilities.
        It is a set of bit-flags that determines which multi-line capabilities to show in the UI.
        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "MLFeatures"=dword:7    ;(Suggested default setting. Same as not having this regkey)        

        The value of MLFeatures is based on the following bit flags
        #define SHDFL_NOML_EX             0x00000000           ;Exclude multi-line outgoing line UI, vmail and callforwarding icons
                                                               ;Incoming line number still shows up in the incoming call UI 
        #define SHDFL_ML_OUTGOING         0x00000001           ;Show multi-line outgoing line UI
        #define SHDFL_ML_VMAIL            0x00000002           ;Show multi-line vmail UI support
        #define SHDFL_ML_FORWARD          0x00000004           ;Show multi-line callforwarding icons        
        
*/

/* 
   15.  Setting ring tones.
        For information on setting the ring tone for line 1 or line 2, querying valid directories
        for ring tones, or querying available ring tones, see
        public\shellw\sdk\inc\soundfile.h
        For information on adding custom ringtone directory see #28.
*/

/* 
   16.  Setting a voicemail error message (used by PH_MSG_ERRORNOVMAILNUMBER)
        If no voicemail number is configured, the phone app provides the option of specifying a
        message that will help the user learn how to configure their voicemail number.  The following
        registry key can be used to set the error message:
        [HKLM\Security\ResOver]
        "55"="<Custom Message>"  ; PH_MSG_ERRORNOVMAILNUMBER will display a message containing "<Custom Message>"

        This message will be displayed when the user tries to call their voicemail using speed dial '1' 
        or any pre-defined voicemail access function (such as voicemail notifications, the home screen plug-in, 
        etc).  If the registry key is not set or is empty, the phone app will never attempt to display this
        message.  If the OEM uses this PH_MSG with PHShowPhoneMsg, they must either set the registry key or
        intercept the message and display their own UI with PHExtShowPhoneMsg, or the phone app
        will display a blank error.
*/


/*

   17.  Auto-Prefix Dialing

        Currently, auto-prefix is only supported on Smartphone
        Setting auto-prefix feature flag (SHDFL_PHONE_SUPPORTAUTOPREFIX) in registry enables the Auto-Prefix
        dialing UI and feature. When the user selects this feature and fills in an area code, the phone prepends 
        this area code to every dial string. 

        The regkeys discussed below configure different aspects of Auto-Prefix dialing:

        To automatically select auto-prefix dialing in Call Options Settings, the following regkey should be set.
        If it isn't set, the user has to select the option manually in Call Option in the Settings menu
        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "AutoPrefix"=dword:1  ;Always use auto-prefix dialing if auto-prefix feature flags has been turned on
        "AutoPrefix"=dword:0  ;(default)

        As the user might want to dial a different area code than the one set in Call Options, 
        we can define special characters which override the auto-prefix so the user has the ability 
        to manually enter a different area code, if desired.     
        All special characters which override the auto-prefix feature when entered into the accumulator 
        by the user must be set in the following registry key. 
        If there are no special characters, do not create the value.  
        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "AutoPrefixOverrides"=<string:List of single characters. No spaces or delimiters in-between characters> 

        e.g.
        "AutoPrefixOverrides"="*#" 
        In this case, if auto-prefix is turned on and area code is set to "425", dialing "12345" in smart dial
        will call "42512345" but dialing "*12345" or "#12345" will call "12345"

*/

/* 18.  Activating Speed Dial On Talk
        Setting the following registry value will enable the Speed Dial On Talk feature.  This
        feature allows the user to dial a speed dial number by hitting talk after dialing one or two digits.
        This feature should not be enabled in a region where emergency numbers are one or two digits.

        [HKEY_LOCAL_MACHINE\Security\Phone]
        "SpeedDialOnTalk"=dword:00000001      ; Activate Speed Dial On Talk
*/

/*

    19. Controlling Network settings UI for manual and preferred network selection
    
        Enhanced Operator Name String (EONS or E-ONS) is a specification that allows for advanced control 
        of the operator name string.  With EONS in place, operators would like to remove the end-user ability 
        to set preferred networks or manually set networks.  Thus, a set of protected registry keys are defined
        to control these end-user settings.

        Setting the following registry key to a positive value will cause Manual Network Selection UI
        elements (as well as Preferred Network Selection UI elements) to be hidden from Phone->Network
        settings. With this change, even if the network selection was set to Manual before this registry key was set,
        network selection through Find a Network will behave as if Automatic mode is selected.
    
            [HKEY_LOCAL_MACHINE\Security\Phone]
            "ManualDisable"=dword:00000001      ; Disable manual network selection
    
        Similarly, setting the following registry key to a positive value will cause Preferred Network Selection UI
        elements to be hidden from Phone->Network settings. Note that, if manual network selection is already
        disabled as described above, this won't have any affect since disabling manual network selection will
        automatically disable preferred network selection.
    
            [HKEY_LOCAL_MACHINE\Security\Phone]
            "PreferredDisable"=dword:00000001      ; Disable preferred network selection
    
        For both of these registry keys, not having the registry key is treated as value set to 0 (which means
        the default behavior of not disabling)
    
*/    

/*
    20. Disabling the Edit Before Call dialog box

    If an OEM would like to remove the Edit Before Call dialog box (invoked via PhoneMakeCall with the 
    PMCF_EDITBEFORECALLING flag), the OEM can set the following reg key:

        [HKEY_LOCAL_MACHINE\Software\Microsoft\Shell\Phone]
        "DisableEditBeforeCall" = dword:00000001    ; Disable Edit Before Call

    If this value is set, then instead of displaying the Edit Before Call prompt, the API will then send a 
    PH_MSG_CONFIRMDIALREQUEST message to any PHExtShowPhoneMsg, and dial if dialing is confirmed.
    
*/

/*
    21. Disabling the Fixed Dialing option in the List Box of the Services tab in the Phone Settings (PPC only)

    If an OEM would like to remove the option for "Fixed Dialing" in the List Box which shows
    available services on the "Services" tab for the Phone settings control panel, the following
    registry key could be set:

        [HKEY_LOCAL_MACHINE\ControlPanel\FixedDialing]
        "DisableMenu" = dword: 00000001; disables the menu item

    If this value is set to a non-zero value, the menu item is not displayed.  The default value is 0.  
    If the key is not set, the menu item is displayed as normal.  This is for PocketPC only.

*/


/* 

   22.  Turning off matches accumulator in smartdial

        In Pocket PC and Smartphone Smartdial, we display a second accumulator that contains the current set of the strings
        the filter/accumulator input matches to. This feature can be turned off by setting the registry key below.    
        NOTE: On Pocket PC, this registry key trumps the matchesHeight setting for the WC_SMARTDIAL control.
              As a result the rowHeight setting for WC_SMARTDIAL might require adjusting as described in Section 3. 
   
           [HKEY_CURRENT_USER\ControlPanel\Phone]
           "NoMatchesAccum"=dword:00000001               ; Disable matches accumulator

        Not having the registry key is treated as value set to 0 (which means the default behavior of not disabling)
*/

/* 

   23.  Controlling default keypad state 

        In Pocket PC, we have two dialer keypad states, Down and Up. Down displays the full screen smartdial view (PH_VIEW_SMARTDIALER),
        and Up shows the keypad (PH_VIEW_DIALER) and shrinks the smartdial view to a couple of entries. 
        In some orientations, especially landscape and square, there might be a need to control the default keypad state 
        in order to show more entries. The regkeys below are used to configure the default keypad state when the phone is idle.

        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "KeypadStateOnLaunch"=dword:00000000               ; Keypad up by default. Always show PH_VIEW_DIALER on launch
        "KeypadStateOnLaunch"=dword:00000001               ; Keypad down by default. Always show PH_VIEW_SMARTDIALER  on launch

        If this regkey is not present, we default to keypad up (dword:00000000)

        We also allow specifying which orientations (Portrait, Square or Landscape) the above regkey applies

        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "KeypadStateOrientations"=dword

        The values for KeypadStateOrientations is any combination of the below PH_VIEWINFOFLAGS (See Section 2 for declaration)
             PH_VIF_SQUARE    =       0x00000002   ; KeypadStateOnLaunch applies to Square screen
             PH_VIF_PORTRAIT  =       0x00000004   ; KeypadStateOnLaunch applies to Portrait screen
             PH_VIF_LANDSCAPE =       0x00000008   ; KeypadStateOnLaunch applies to Landscape screen

        For example, to apply the default keypad state described in HKEY_CURRENT_USER\ControlPanel\Phone\KeyPadStateOnLaunch 
        to only landscape, KeypadStateOrientations should be set to 0x8. 
        For both portrait and landscape, it should be set to 0xC (0x8|0x4)

        If this regkey is not present, we apply the value of HKEY_CURRENT_USER\ControlPanel\Phone\KeyPadStateOnLaunch to all orientations  

*/

/* 

    24. Turning off SIP button in full screen smartdial's menu bar

        In Pocket PC, a SIP button is typically displayed in the menu bar when full screen smartdial is open.
        This feature can be turned off by setting the registry key below.    
   
        [HKEY_CURRENT_USER\ControlPanel\Phone]
        "SIP"=dword:00000000               ; Disable SIP button in full screen smartdial menu bar

        Not having the registry key is treated as value set to 1 (which means the default behavior of enabling)
*/

/*
    25. Handling '+' for international dialing

        If a device's hardware supports dialing '+' for international calls, the SHDFL_PHONE_INTLPLUS bit should be set 
        in the phone feature flags (see topic 13).  If the SHDFL_PHONE_INTLPLUS flag is not set, we will attempt to translate 
        any leading '+' in a phone number to an appropriate dialing prefix when placing an outgoing call.  To work correctly 
        this requires that the device is configured for the country in which the phone is being used.  The translation 
        is done based on locale and not based on the registry key used for the international code feature (see below).  
        We look up the current locale and get the appropriate IDD and NDD for the current country.  If the number is within
        the current country, we replace the '+' and country code with the NDD for that country.  If the number is international
        we replace the '+' with the IDD for the current country.  The IDD and NDD information can be overridden using the
        following registry key:
            [HKEY_LOCAL_MACHINE\Security\Phone]
            "DialingPrefixData"=<string: List of country/IDD/NDD information>

        The format of this string is "CC,IDD,NDD;CC,IDD,NDD;".  CC is the country code as defined by ITU.164/ITU.212.  IDD is
        the international direct dialing prefix.  NDD is the national direct dialing prefix (or trunk prefix).
        If a country has multiple NDDs, they should be separated by commas.  Only the individual countries whose data should 
        be replaced need to be specified.  A country cannot be removed from the default table, it can only have its data replaced.

        
        To prevent '+' from showing up on the Pocket PC on-screen keypad, the following registry key should be set:
            [HKEY_CURRENT_USER\ControlPanel\Phone]
            "NoKeypadPlus"=dword:00000001           ; Hide the '+' on the zero key on Pocket PC's on-screen keypad
        Note that pressing and holding zero to get a '+' is based on the SHDFL_PHONE_INTLPLUS flag and not this registry
        key.

        Smartphone supports pre-populating the dialer with the current international direct dialing prefix.  This 
        feature requires the SHDFL_PHONE_SUPPORTINTERNATIONALCODE flag to be set (again, see topic 13), as well as
        the following registry key:
            [HKEY_CURRENT_USER\ControlPanel\Phone]
            "InternationalCode"="011"               ; substitute the correct international code for '011'
        When these values are set, pressing and holding zero will put the international code in the accumulator instead of
        a '+'.
*/

/* 

    26.  Adding "Go to Contacts" menu item to PPC smartdial's SK2 menu

        In Pocket PC, an extra menu item can be added to smartdial's SKU menu to open the Contacts application.
        This feature can be useful for users who prefer to find contacts via the Contact application's UI.
            [HKEY_CURRENT_USER\ControlPanel\Phone]
            "GoToContacts"=dword:00000001               ; Add "Go to Contacts" menu item to PPC smartdial's SK2 menu

        Not having the registry key is treated as value set to 0
        (which means the default behavior of not having the "Go to Contacts" menu item)
*/

/* 

    27.  Adding a custom ringtone directory

        The Snd APIs will additionally look for valid ringtone files in the specified absolute path.
        [HKEY_LOCAL_MACHINE\ControlPanel\Sounds\CustomSounds\Ringtones]
        "Directory"=string    ; Length of the path cannot exceed MAX_PATH characters.
    
*/

/*
    28.  Enabling roaming icons for call history 
    
         Incoming and Outgoing calls in Call History, can show additional roaming status of those calls in the icon,
         based on the following registry key.
             [HKEY_LOCAL_MACHINE\Security\Phone]
             "ShowRoamingIconsInCallHistory"=dword:00000001  

         Not having the registry key is treated as value set to 0
        (which means the default behavior is to not show roaming icons for roaming incoming/outgoing calls)
*/

/*

    29.  Controlling the handling of special characters in dial strings

        This customization controls how the phone application handles pause
        characters (i.e. 'p' and ',') and SupSvc (Supplementary Services) codes
        for GSM devices.  CDMA devices ignore this customization.
        
        Customization is controlled by the following registry data:
            [HKEY_LOCAL_MACHINE\Security\Phone]
            "SendCodesToRilDial" (REG_DWORD)

        Supported bits are defined in regshared.h:
            SENDCODES_PAUSE
            SENDCODES_CLIR

        By default, when a user dials a string, the phone application sends the
        portion of the dial string before the first pause character to the radio
        and then perform pauses and sending of DTMFs directly.  For example,
        if the user dials 4251234567p890 then the phone application will send
        4251234567 to the radio.  Once the call connects, the phone
        application will pause then send DTMFs 8, 9, and 0.

        If the SENDCODES_PAUSE bit is set then the phone application will 
        send dial strings containing pause characters to the radio after
        converting any 'p' characters to ','  For example, if the user dials
        4251234567p890 then the radio will be sent 4251234567,890.  The phone
        application will not perform any pausing nor will it send any further
        DTMFs once the call connects.  
        Note:  Radios should pause for 3 seconds whenever a comma is encountered
        in a dial string before processing the rest of the dial string.


        By default, the phone application will parse out SupSvc (Supplementary
        Services) CLIR (Calling Line Identification Restriction) codes
        from a dial string prior to sending it to the radio.  The SupSvc codes
        (e.g. #31#) will be sent individually and directly to the radio
        without checking against the FDN (Fixed Dialing Numbers) list first.

        If the SENDCODES_CLIR bit is set then the phone application will
        forward dial strings that contain SupSvc CLIR commands directly to the
        radio.  In these scenarios, the radio is responsible for validating the
        dial string against the FDN list if FDN has been enabled.  The radio is
        also responsible for ensuring that SupSvc codes within the dial string
        are executed properly.

        For example, suppose a user dials the string #31#5551212 in order to
        dial the phone number 555-1212 without sending their caller-id information.
        If the SENDCODES_CLIR bit is set then the phone application will send
        the entire dial string to the radio and the radio becomes fully responsible
        for handling the dial string.  If the SENDCODES_CLIR bit is not set then
        the phone application will first send out the request to turn off caller-id
        and then follow that with a separate dial request for 555-1212.

        The radio should already be able to handle dial strings containing non-CLIR
        SupSvc codes properly with the FDN list regardless of whether or not the
        SENDCODES_CLIR bit is set.  The sample code in public\cellcore\ril\driver\gsm
        provides several examples of SupSvc code implementations that work properly
        with the FDN list.
*/

/*

 30.    API that allows OEMs to write voice call information to the call log database
 
        PhoneSetCallLogEntry writes the call log information to the database
        INPUT   : CALLLOGENTRY structure that specifies the call information to be logged
        OUTPUT: S_OK if succeeded
                E_INVALIDARG indicates that something is wrong with one/more parameters
                E_FAIL: if error occured while writing to the database

        The following entries in the CALLLOGENTRY structure are compulsory
        FILETIME ftStartTime;
        FILETIME ftEndTime;
        Both these entries have to be set correctly, and it is expected that ftStartTime <= ftEndTime

        DWORD dwLogFlags;        // flags to indicate the type of the call
        This dword indicates the type of the call based on the bit map defined in public\apps\oak\inc\phlog.h

        CEOID oidContact;        // oid of the PIM contact
        If specified, this indicates the contact oid to which this log entry has to be associated.

        CEPROPID pidProp;        // prop-id of the contact (one of the PIMPR properties defined in pimstore.h)
        Communication property of the contact (for example: mobile is PIMPR_MOBILE_TELEPHONE_NUMBER). 
        This field is ignored if valid oidContact is not specified.

*/

        HRESULT PhoneSetCallLogEntry(PCALLLOGENTRY pentry);

