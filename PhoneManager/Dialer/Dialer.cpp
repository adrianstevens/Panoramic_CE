// Dialer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "Dialer.h"
#include "PhCanvas.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "resource.h"
#include "IssRegistry.h"
#include "PhoneResources.h"
#include "DlgHdnDialer.h"
#include "ServerCommunicate.h"
#include "commondefines.h"
#include "GeneralOptions.h"

#define COLORREF_WHITE              RGB(255,255,255)
#define COLORREF_BLACK              RGB(0  ,0  ,0  )

#define FONT_HT_VERBBUTTONS         22
#define FONT_HT_DIALBUTTONS_MAJOR   28
#define FONT_HT_DIALBUTTONS_MINOR   24     
#define FONT_HT_IDC_STATUSAREA      25
#define FONT_HT_IDC_CALLERLIST      25

HINSTANCE	    g_hInst;
CDlgHdnDialer*   g_pdlgHdnDialer = NULL;

HRESULT LoadOEMDll();
BOOL    ProcHiddenWindow(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

// Phone behavior overriding functions
HRESULT PHExtInitialize(void *pVoid);
HRESULT PHExtShowPhoneMsg(PH_MSG phmsg, LPCWSTR psz1, LPCWSTR psz2, BOOL fRemove, DWORD dwSupSvcStatus, BOOL* pfDial); 
HRESULT PHExtDoVerb(PH_VERB verb, BOOL fValidateOnly, VPARAM vp);
HRESULT PHExtGetPhoneViewInfo(PH_VIEW view, HINSTANCE * phInstance, PH_VIEWINFOFLAGS * pvif, LPPHONEVIEWHOOKPROC * plpfnHook, LPCTSTR * plpTemplateName);
HRESULT PHExtGetPhoneControlMetrics(PH_VIEW view, PH_VIEWINFOFLAGS vif, DWORD idControl, LPCTSTR pszValue, LPDWORD lpData);
HRESULT PHExtOnPhoneStateChange(const PHPHONESTATE* c_pps);

typedef HRESULT (*PFN_Initialize)(void *);
typedef HRESULT (*PFN_ShowPhoneMsg)(PH_MSG , LPCWSTR , LPCWSTR , BOOL , DWORD , BOOL * );
typedef HRESULT (*PFN_DoVerb)(PH_VERB , BOOL , VPARAM );
typedef HRESULT (*PFN_GetPhoneViewInfo)(PH_VIEW , HINSTANCE * , PH_VIEWINFOFLAGS * , LPPHONEVIEWHOOKPROC * , LPCTSTR * );
typedef HRESULT (*PFN_GetPhoneControlMetrics)(PH_VIEW , PH_VIEWINFOFLAGS , DWORD , LPCTSTR , LPDWORD );
typedef HRESULT (*PFN_OnPhoneStateChange)(const PHPHONESTATE* );

HINSTANCE       g_hOEMDll = NULL;
PFN_Initialize  g_pfnInitialize = NULL;
PFN_ShowPhoneMsg g_pfnShowPhoneMsg = NULL;
PFN_DoVerb      g_pfnDoVerb = NULL;
PFN_GetPhoneViewInfo g_pfnGetPhoneViewInfo = NULL;
PFN_GetPhoneControlMetrics g_pfnGetPhoneControlMetrics = NULL;
PFN_OnPhoneStateChange g_pfnOnPhoneStateChange = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = (HINSTANCE)hModule;
        if(!g_pdlgHdnDialer)
            g_pdlgHdnDialer = new CDlgHdnDialer;
        if(g_pdlgHdnDialer)
            g_pdlgHdnDialer->CreateHiddenWindow(g_hInst);
        LoadOEMDll();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
        break;
	case DLL_PROCESS_DETACH:
        if(g_pdlgHdnDialer)
            g_pdlgHdnDialer->DestroyHiddenWindow();
        if(g_hOEMDll)
            FreeLibrary(g_hOEMDll);
		break;
	}
    return TRUE;
}

HRESULT LoadOEMDll()
{
    if(g_hOEMDll)
        FreeLibrary(g_hOEMDll);
        
    g_hOEMDll = NULL;
    g_pfnInitialize = NULL;
    g_pfnShowPhoneMsg = NULL;
    g_pfnDoVerb = NULL;
    g_pfnGetPhoneViewInfo = NULL;
    g_pfnGetPhoneControlMetrics = NULL;
    g_pfnOnPhoneStateChange = NULL;

    HRESULT hr = S_OK;
    HKEY hKey = NULL;
    long lRet;

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_Skin, NULL, NULL, &hKey);
    CBARG(lRet==ERROR_SUCCESS, _T(""));

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    lRet = RegQueryValueEx(hKey, REG_BackupOEM, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize);
    CBARG(lRet==ERROR_SUCCESS && _tcslen(szTemp) > 0, _T(""));

    g_hOEMDll = LoadLibrary(szTemp);
    CPHR(g_hOEMDll, _T(""));

    g_pfnInitialize             = (PFN_Initialize)GetProcAddress(g_hOEMDll, _T("PHExtInitialize"));
    g_pfnShowPhoneMsg           = (PFN_ShowPhoneMsg)GetProcAddress(g_hOEMDll, _T("PHExtShowPhoneMsg"));
    g_pfnDoVerb                 = (PFN_DoVerb)GetProcAddress(g_hOEMDll, _T("PHExtDoVerb"));
    g_pfnGetPhoneViewInfo       = (PFN_GetPhoneViewInfo)GetProcAddress(g_hOEMDll, _T("PHExtGetPhoneViewInfo"));
    g_pfnGetPhoneControlMetrics = (PFN_GetPhoneControlMetrics)GetProcAddress(g_hOEMDll, _T("PHExtGetPhoneControlMetrics"));
    g_pfnOnPhoneStateChange     = (PFN_OnPhoneStateChange)GetProcAddress(g_hOEMDll, _T("PHExtOnPhoneStateChange"));

Error:
    if(hKey)
        RegCloseKey(hKey);
    return hr;
}

/*
PHExtInitialize

This will be the first API call in the the OEM extension
dll and will be called only once.  


[in]  pVoid  : Reserved, currently unused.

returns:
S_OK : success
*/
extern "C" HRESULT PHExtInitialize(void *pVoid)
{
    UNREFERENCED_PARAMETER(pVoid);

    DBG_OUT((_T("PHExtInitialize")));

    if(g_hOEMDll && g_pfnInitialize)
        return g_pfnInitialize(pVoid);

    return S_OK;
}

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
extern "C" HRESULT PHExtShowPhoneMsg(PH_MSG phmsg, LPCWSTR psz1, LPCWSTR psz2, BOOL fRemove, DWORD dwSupSvcStatus, BOOL* pfDial)
{
    HRESULT hr = E_NOTIMPL;

    //DBG_OUT((_T("PHExtShowPhoneMsg")));

    // To override behavior, return S_OK and insert code here.  If you wish 
    // to let default processing
    // still occur after having intercepted the message, return E_NOTIMPL.

    if(g_hOEMDll && g_pfnShowPhoneMsg)
        return g_pfnShowPhoneMsg((PH_MSG)phmsg, psz1, psz2, fRemove, dwSupSvcStatus, pfDial);

    return hr;
}

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
extern "C" HRESULT PHExtDoVerb(PH_VERB verb, BOOL fValidateOnly, VPARAM vp)
{
    HRESULT hr = E_NOTIMPL;

    //DBG_OUT((_T("PHExtDoVerb")));

    if(g_hOEMDll && g_pfnDoVerb)
        return g_pfnDoVerb(verb, fValidateOnly, vp);

    return hr;
}

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
extern "C" HRESULT PHExtGetPhoneViewInfo(PH_VIEW view, HINSTANCE* phInstance, PH_VIEWINFOFLAGS* pvif, LPPHONEVIEWHOOKPROC*  plpfnHook, LPCTSTR* plpTemplateName)
{
    HRESULT hr = E_NOTIMPL;

    DBG_OUT((_T("PHExtGetPhoneViewInfo")));

    // set defaults
    //*phInstance     = NULL;
    //*plpfnHook      = NULL;
    //*plpTemplateName= NULL;

    switch(view)
    {
    case PH_VIEW_INCOMING:
    {

        DBG_OUT((_T("View - PH_VIEW_INCOMING")));
        DWORD dwValues = FLAG_SHOW_Cheekguard|FLAG_SHOW_IncomingCall|FLAG_SHOW_PostCall;
        GetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, dwValues);

        if(dwValues & FLAG_SHOW_IncomingCall)
        {
            hr = S_OK;
            //*phInstance = g_hInst;
            //*plpTemplateName = MAKEINTRESOURCE(IDD_DLG_Basic);
            *pvif = (PH_VIEWINFOFLAGS) (*pvif | PH_VIF_ENABLEHOOK);
            *plpfnHook = ProcHiddenWindow;            
        }
        else if(g_hOEMDll && g_pfnGetPhoneViewInfo)
        {
            hr = g_pfnGetPhoneViewInfo(view, phInstance, pvif, plpfnHook, plpTemplateName);
            //if(*plpTemplateName != NULL && *phInstance == NULL)
            //    *phInstance = g_hOEMDll;
        }

        break;
    }
    default:
        DBG_OUT((_T("View - other")));
        if(g_hOEMDll && g_pfnGetPhoneViewInfo)
        {
            hr = g_pfnGetPhoneViewInfo(view, phInstance, pvif, plpfnHook, plpTemplateName);
            //if(*plpTemplateName != NULL && *phInstance == NULL)
            //    *phInstance = g_hOEMDll;
        }
    }

    return hr;
}

BOOL ProcHiddenWindow(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    //DBG_OUT((_T("ProcHiddenWindow - %d"), uiMessage));
    switch(uiMessage)
    {
    case WM_INITDIALOG:
        break;
    }
    return TRUE;
}

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
extern "C" HRESULT PHExtOnPhoneStateChange(const PHPHONESTATE* c_pps)
{
    HRESULT hr = E_NOTIMPL;

    DBG_OUT((_T("PHExtOnPhoneStateChange")));

    if(g_hOEMDll && g_pfnOnPhoneStateChange)
        return g_pfnOnPhoneStateChange(c_pps);

    return hr;
}

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
extern "C" HRESULT PHExtGetPhoneControlMetrics(PH_VIEW view, PH_VIEWINFOFLAGS vif, DWORD idControl, LPCTSTR pszValue, LPDWORD lpData)
{
    HRESULT hr = E_NOTIMPL;

    //DBG_OUT((_T("PHExtGetPhoneControlMetrics")));

    if(g_hOEMDll && g_pfnGetPhoneControlMetrics)
        return g_pfnGetPhoneControlMetrics(view, vif, idControl, pszValue, lpData);

    /*static const TCHAR c_szTahomaFont[] = TEXT("Tahoma");
    static const TCHAR c_szNinaFont[] = TEXT("Nina");
    static const TCHAR c_szValTxtNm[] = TEXT("Nm");
    static const TCHAR c_szValTxtHt[] = TEXT("Ht");
    static const TCHAR c_szValTxtWt[] = TEXT("Wt");
    static const TCHAR c_szValTxtMajorNm[] = TEXT("majorNm");
    static const TCHAR c_szValTxtMajorHt[] = TEXT("majorHt");
    static const TCHAR c_szValTxtMajorWt[] = TEXT("majorWt");
    static const TCHAR c_szValTxtMinorNm[] = TEXT("minorNm");
    static const TCHAR c_szValTxtMinorHt[] = TEXT("minorHt");
    static const TCHAR c_szValTxtMinorWt[] = TEXT("minorWt");
    static const TCHAR c_szValTxtFlags[] = TEXT("textFlags");
    static const TCHAR c_szValTxtMajorFlags[] = TEXT("majorFlags");
    static const TCHAR c_szValTxtMinorFlags[] = TEXT("minorFlags");
    static const TCHAR c_szValTxtColor[] = TEXT("textColor");
    static const TCHAR c_szValTxtLayoutFlags[] = TEXT("textLayoutFlags");
    int cbData = 0;


    // Check to see if we're in the call progress portrait, square or landscape view
    if((PH_VIEW_PROGRESS == view) && ((PH_VIF_PORTRAIT == vif) || (PH_VIF_SQUARE == vif) || (PH_VIF_LANDSCAPE == vif)))
    {
        switch(idControl)
        {
        case IDC_HISTORY_SPEAKER:
            // Fall through - want the same text settings as IDC_NOTEPAD
        case IDC_MUTE:
            // Fall through - want the same text settings as IDC_NOTEPAD
        //case IDC_HOLDSWAP:
            // Fall through - want the same text settings as IDC_NOTEPAD
        case IDC_TALKEND:
            // Fall through - want the same text settings as IDC_NOTEPAD
        case IDC_CONTACTS:
            // Fall through - want the same text settings as IDC_NOTEPAD
        case IDC_NOTEPAD:
            // Specify a value for the textFlags field
            if(0 == _tcscmp(c_szValTxtFlags, pszValue))
            {
                *lpData = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
                hr = S_OK;
            }

            // Specify a font name
            if(0 == _tcscmp(c_szValTxtNm, pszValue))
            {
                // Must allocate a buffer for lpData if data is a string
                // The caller of PHExtGetPhoneControlMetrics will call 
                // LocalFree for the strings (documented in phcanvas.h)
                cbData = sizeof(c_szTahomaFont);
                *lpData = (DWORD)LocalAlloc(LMEM_FIXED, cbData);
                if (NULL == *lpData)
                {
                    goto Error;
                }
                hr = StringCbCopy((TCHAR*)*lpData, cbData, c_szTahomaFont);
                if(FAILED(hr))
                {
                    goto Error;
                }
                hr = S_OK;
            }

            // Specify a font height
            if(0 == _tcscmp(c_szValTxtHt, pszValue))
            {
                *lpData = FONT_HT_VERBBUTTONS;
                hr = S_OK;
            }

            // Specify a font weight
            if(0 == _tcscmp(c_szValTxtWt, pszValue))
            {
                *lpData = FW_MEDIUM;
                hr = S_OK;
            }
            break;

        case IDC_ELAPSEDTIME:
            // Fall through - want the same text settings as IDC_CALLERLIST
        case IDC_CALLSTATE:
            // Fall through - want the same text settings as IDC_CALLERLIST
        case IDC_STATUSAREA:
            // Fall through - want the same text settings as IDC_CALLERLIST
        case IDC_CALLERLIST:
            // Specify a font name
            if(0 == _tcscmp(c_szValTxtNm, pszValue))
            {
                // Must allocate a buffer for lpData if data is a string
                // The caller of PHExtGetPhoneControlMetrics will call 
                // LocalFree for the strings (documented in phcanvas.h)
                cbData = sizeof(c_szTahomaFont);
                *lpData = (DWORD)LocalAlloc(LMEM_FIXED, cbData);
                if (NULL == *lpData)
                {
                    goto Error;
                }
                hr = StringCbCopy((TCHAR*)*lpData, cbData, c_szTahomaFont);
                if(FAILED(hr))
                {
                    goto Error;
                }
                hr = S_OK;
            }

            // Specify a font height
            if(0 == _tcscmp(c_szValTxtHt, pszValue))
            {
                *lpData = FONT_HT_IDC_CALLERLIST;
                hr = S_OK;
            }

            // Specify a font weight
            if(0 == _tcscmp(c_szValTxtWt, pszValue))
            {
                *lpData = FW_MEDIUM;
                hr = S_OK;
            }

            // Specify a text color
            if(0 == _tcscmp(c_szValTxtColor, pszValue))
            {
                *lpData = COLORREF_BLACK;
                hr = S_OK;
            }
            break;

        default:
            break;
        }
    }

Error:*/
    return hr;
}


