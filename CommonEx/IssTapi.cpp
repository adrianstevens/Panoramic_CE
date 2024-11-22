/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssTapi.cpp
// Abstract: encapsulates TAPI functionality
//  
/***************************************************************************************************************/

#include "IssTapi.h"
#include <tapi.h>
#include <extapi.h>
#include "stdio.h"
#include "IssDebug.h"
#include <tsp.h>
#include "IssGDIEx.h"
#ifndef USE_TAPI_DEBUGOUT
#include "IssCommon.h"
#endif

#define TAPI_API_LOW_VERSION    0x00020000
#define TAPI_API_HIGH_VERSION    0x00020000
#define EXT_API_LOW_VERSION        0x00010000
#define EXT_API_HIGH_VERSION    0x00010000

#define CELLTSP_LINENAME_STRING (L"Cellular Line")
#define EXIT_ON_NULL(_p)     \
	if (_p == NULL)    \
{   \
	hr = E_OUTOFMEMORY; \
	goto FuncExit; \
} 

#define EXIT_ON_FALSE(_f)     \
	if (!(_f))    \
{   \
	hr = E_FAIL; \
	goto FuncExit; \
} 

// debug Options
#define DEBUG_WindowEvents		FALSE	// send debug messages to our parent window
#define DEBUG_File				TRUE	// send debug messages to the file
#define DEBUG_Output			TRUE	// send debug messages to the debug output window


CIssTapi* g_poTapi    = NULL;
CALLERIDINFO::CALLERIDINFO()
{
	gdiPicture = NULL;
	lOid = 0;
}
CALLERIDINFO::~CALLERIDINFO()
{
	if(gdiPicture)
		gdiPicture->Destroy();
}
static CIssTapi* g_tapi = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssTapi::CIssTapi()
:m_hInstance(NULL)            
,m_psLine(NULL)    
,m_lMakeCallRequestID(0)
,m_lDropCallRequestID(0)
,m_bCurrentLineAvail(TRUE)    
,m_dwNumDevs(0)        
,m_szDebug(NULL)
,m_oStr(CIssString::Instance())
,m_hDebugFile(NULL)
,m_hWndDebug(NULL)
{
    m_oStr->Empty(m_sCallerID.szName);
    m_oStr->Empty(m_sCallerID.szNumber);
	g_tapi = this;
}

CIssTapi::~CIssTapi()
{
    Destroy();
	g_tapi = NULL;
}
CIssTapi* CIssTapi::Instance()
{
	return g_tapi;
}

/********************************************************************
Function    Destroy

Arguments:    

Returns:    

Comments:    Close all open TAPI handles
*********************************************************************/
HRESULT CIssTapi::Destroy()
{
    m_oStr->Delete(&m_szDebug);

    if(m_hDebugFile)
    {
        fclose(m_hDebugFile);
        m_hDebugFile = NULL;
    }

    if(m_psLine)
    {
        m_lDropCallRequestID = 0;
        m_lMakeCallRequestID = 0;

        // drop the call
        if(m_psLine->hCallCurr)
        {
            m_lDropCallRequestID = lineDrop(m_psLine->hCallCurr, NULL, 0);
            lineDeallocateCall(m_psLine->hCallCurr);
            m_psLine->hCallCurr = NULL;
        }

        // close the line
        if(m_psLine->hCurrLine)
        {
            lineClose(m_psLine->hCurrLine);
            m_psLine->hCurrLine = NULL;
            m_bCurrentLineAvail = TRUE;
        }

        // disconnect TAPI from the app
        if(m_psLine->hLineApp)
        {
            lineShutdown(m_psLine->hLineApp);
            m_psLine->hLineApp = NULL;
        }

        delete m_psLine;
        m_psLine = NULL;
    }

    return S_OK;
}

BOOL CIssTapi::SetFlyMode(BOOL bFlying)
{
	if(!m_psLine)
		return FALSE;

	if(!m_psLine->hCurrLine)
		return FALSE;

	long lResult;
	if(bFlying)
		lResult = lineSetEquipmentState(m_psLine->hCurrLine, LINEEQUIPSTATE_MINIMUM) ; 
	else
	{
		lResult = lineSetEquipmentState(m_psLine->hCurrLine, LINEEQUIPSTATE_FULL) ; 
		// DH: handle no service condition
		lResult = lineRegister(m_psLine->hCurrLine, LINEREGMODE_AUTOMATIC, NULL, 0);
	}

	return TRUE;
}

BOOL CIssTapi::IsFlightMode()
{
	if(!m_psLine)
		return FALSE;

	if(!m_psLine->hCurrLine)
		return FALSE;

	DWORD dwState, dwRadioSupport;
	long lResult = lineGetEquipmentState(m_psLine->hCurrLine, &dwState, &dwRadioSupport) ; 

	if(1)
		return (dwState == LINEEQUIPSTATE_MINIMUM);

}

// ***************************************************************************
// Function Name: GetTSPLineDeviceID
//
// Purpose: To get a TSP Line Device ID
//
// Arguments:
//    hLineApp = the HLINEAPP returned by lineInitializeEx
//    dwNumberDevices = also returned by lineInitializeEx
//    dwAPIVersionLow/High = min version of TAPI that we need
//    psTSPLineName = "Cellular Line"
//
// Return Values: Current Device ID
//
// Description:
//    This function returns the device ID of a named TAPI TSP.  The Device ID is 
//    used in the call to lineOpen
// ***************************************************************************
DWORD GetTSPLineDeviceID(const HLINEAPP hLineApp, 
                         const DWORD dwNumberDevices, 
                         const DWORD dwAPIVersionLow, 
                         const DWORD dwAPIVersionHigh, 
                         const TCHAR* const psTSPLineName)
{
    DWORD dwReturn = 0xffffffff;
    for(DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumberDevices ; dwCurrentDevID++)
    {
        DWORD dwAPIVersion;
        LINEEXTENSIONID LineExtensionID;
        if(0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, 
            dwAPIVersionLow, dwAPIVersionHigh, 
            &dwAPIVersion, &LineExtensionID))
        {
                LINEDEVCAPS LineDevCaps;
                LineDevCaps.dwTotalSize = sizeof(LineDevCaps);
                if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                    dwAPIVersion, 0, &LineDevCaps)) 
                {
                        BYTE* pLineDevCapsBytes = new BYTE[LineDevCaps.dwNeededSize];
                        if(0 != pLineDevCapsBytes) 
                        {
                            LINEDEVCAPS* pLineDevCaps = (LINEDEVCAPS*)pLineDevCapsBytes;
                            pLineDevCaps->dwTotalSize = LineDevCaps.dwNeededSize;
                            if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                                dwAPIVersion, 0, pLineDevCaps)) 
                            {
                                    if(0 == _tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), psTSPLineName)) 
                                    {
                                        dwReturn = dwCurrentDevID;
                                    }
                            }
                            delete[]  pLineDevCapsBytes;
                        }
                }
        }
    }
    return dwReturn;
}

/********************************************************************
Function    Initialize

Arguments:    hInstance    - app instance
            hWndDebugWindow - send debug messages to this window (Not used right now)

Returns:    

Comments:    Initilize the tapi monitoring
*********************************************************************/
HRESULT CIssTapi::Initialize(HINSTANCE hInstance, HWND hWndDebugWindow)
{
    HRESULT hr = S_OK;
    m_hInstance                = hInstance;
    m_hWndDebug                = hWndDebugWindow;
    m_lDropCallRequestID    = 0;
    m_lMakeCallRequestID    = 0;
    g_poTapi                = this;

    hr = Destroy();
    CHR(hr, _T("Destroy failed"));

    LINEINITIALIZEEXPARAMS liep;
    DWORD dwExtVersion;

    m_psLine = new LINESTRUCT;
    CPHR(m_psLine, _T("m_psLine not declared"));

    m_psLine->dwVersion = TAPI_API_HIGH_VERSION;
	m_psLine->hCallCurr = NULL;

	DebugOut(_T("Enter CIssTapi::Initialize"));
    // set the line init params
    liep.dwTotalSize    = sizeof(liep);
    liep.dwOptions        = LINEINITIALIZEEXOPTION_USEHIDDENWINDOW;

    LONG lRes = lineInitializeEx(&m_psLine->hLineApp, 
                         m_hInstance, 
                        (LINECALLBACK)&ProcTAPIs, TEXT("IssTapi"), 
                        &m_psLine->dwNumLines, &m_psLine->dwVersion, &liep);
    CBARG(lRes == 0, _T("lineInitializeEx failed"));

    // get the device ID
    m_psLine->dwCurrLine = GetTSPLineDeviceID(    m_psLine->hLineApp, m_psLine->dwNumLines, 
                                                    TAPI_API_LOW_VERSION, 
                                                    TAPI_API_HIGH_VERSION, 
                                                    CELLTSP_LINENAME_STRING);

    // error getting the line device ID?
    CBARG(0xffffffff != m_psLine->dwCurrLine, _T("0xffffffff == m_psLine->dwCurrLine")); 

    SetAppTopPriority(LINEMEDIAMODE_AUTOMATEDVOICE);

    // now try and open the line
    // fails const DWORD dwMediaMode = LINEMEDIAMODE_DATAMODEM | LINEMEDIAMODE_INTERACTIVEVOICE | LINEMEDIAMODE_UNKNOWN |LINEMEDIAMODE_AUTOMATEDVOICE;
    const DWORD dwMediaMode = LINEMEDIAMODE_DATAMODEM | LINEMEDIAMODE_INTERACTIVEVOICE /* | LINEMEDIAMODE_AUTOMATEDVOICE */;
    //const DWORD dwMediaMode = LINEMEDIAMODE_AUTOMATEDVOICE;

    //http://msdn2.microsoft.com/en-us/library/ms930957.aspx 
    lRes = lineOpen(m_psLine->hLineApp, m_psLine->dwCurrLine, 
                &m_psLine->hCurrLine, m_psLine->dwVersion, 0, 0, 
                LINECALLPRIVILEGE_OWNER|LINECALLPRIVILEGE_MONITOR, dwMediaMode, 0);
    CBARG(lRes == 0, _T("lineOpen failed"));

    // set up ExTAPI
    lRes = lineNegotiateExtVersion(m_psLine->hLineApp, m_psLine->dwCurrLine, 
                                m_psLine->dwVersion, EXT_API_LOW_VERSION, 
                                EXT_API_HIGH_VERSION, &dwExtVersion);
    CBARG(lRes == 0, _T("lineNegotiateExtVersion failed"));

    LPLINEDEVCAPS lpLineDevCap = GetDeviceCaps();
    CPHR(lpLineDevCap, _T("GetDeviceCaps failed"));

    // set which message we want to see
    lineSetStatusMessages(m_psLine->hCurrLine, lpLineDevCap->dwLineStates, 0);
    free(lpLineDevCap);

Error:
    if(hr != S_OK)
        Destroy();

    return hr;
}

/********************************************************************
Function    GetDeviceCaps

Arguments:    

Returns:    

Comments:    Retrieve the device capabilities
*********************************************************************/
#ifdef USE_TAPI_DEBUGOUT
void CIssTapi::DebugOut(TCHAR* szFormat, ...) 
{ 
#ifdef DEBUG 
	va_list vaMarker; 
	SYSTEMTIME	sysTime;

	GetSystemTime(&sysTime);

	if(NULL == m_szDebug)
		m_szDebug	= new TCHAR[2*STRING_MAX];

	static TCHAR* szTemp = NULL;

	if(!szTemp)
	{
		szTemp = new TCHAR[STRING_MAX];
		if(!szTemp)
			return;
	}

	m_oStr->Empty(m_szDebug);

	if(NULL == m_hDebugFile)
	{
		// first we open any file that may have existed and get rid of anything there
		m_hDebugFile = fopen("\\IssTapi.txt", "w"); 
		fwprintf(m_hDebugFile, _T("") ); 
		fclose(m_hDebugFile); 

		// now we open the file for appending
		m_hDebugFile = fopen("\\IssTapi.txt", "a");
	}

	// put in the time first
	m_oStr->Format(m_szDebug, _T("%02d:%02d:%02d: "), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	// DH: assemble msg in one string...(this was called by different threads and coming out messed up)
	va_start(vaMarker, szFormat); 
	m_oStr->Format(szTemp, szFormat, vaMarker); 
	va_end(vaMarker); 

	if(DEBUG_WindowEvents)
		SendMessage(m_hWndDebug, WM_TAPI_Output, 0, (LPARAM)szTemp);

	m_oStr->Concatenate(m_szDebug, szTemp);
	m_oStr->Concatenate(m_szDebug, _T("\r\n"));
	if(DEBUG_Output)
	{
		OutputDebugString(m_szDebug); 
	}
	if(DEBUG_File)
	{
		fwprintf( m_hDebugFile, m_szDebug ); 
		fflush(m_hDebugFile);
	}


	/*
	// send it to our various outputs
	if(DEBUG_Output)
		OutputDebugString(m_szDebug);
	if(DEBUG_File)
		fwprintf( m_hDebugFile, m_szDebug ); 

	va_start(vaMarker, szFormat); 
	m_oStr->Format(m_szDebug, szFormat, vaMarker); 
	va_end(vaMarker); 

	// send again to our various outputs
	if(DEBUG_Output)
	{
		OutputDebugString(m_szDebug); 
		OutputDebugString(_T("\r\n")); 
	}
	if(DEBUG_File)
	{
		fwprintf( m_hDebugFile, m_szDebug ); 
		fwprintf( m_hDebugFile, _T("\r\n") ); 
		fflush(m_hDebugFile);
	}
	if(DEBUG_WindowEvents)
		SendMessage(m_hWndDebug, WM_TAPI_Output, 0, (LPARAM)m_szDebug);
	*/


#endif
}
#endif // USE_TAPI_DEBUGOUT
LPLINEDEVCAPS CIssTapi::GetDeviceCaps()
{
    LONG lReturn;
    DWORD structSize;
    static LPLINEDEVCAPS lineDevCap;

    lineDevCap = NULL;
    if (!m_psLine)
        return NULL;

    lineDevCap = (LPLINEDEVCAPS)calloc(1,sizeof(LINEDEVCAPS));
    lineDevCap->dwTotalSize = sizeof(LINEDEVCAPS);

    for(;;) 
    {
        lReturn = lineGetDevCaps(m_psLine->hLineApp,m_psLine->dwCurrLine,m_psLine->dwVersion, 0, lineDevCap);
        if (lReturn!=0) 
        {
            // Error occured
            break;
        } 
        else 
        {
            if (lineDevCap->dwTotalSize < lineDevCap->dwNeededSize) 
            {
                structSize = lineDevCap->dwNeededSize;
                free(lineDevCap);
                lineDevCap = (LPLINEDEVCAPS)calloc(1, structSize);
                lineDevCap->dwTotalSize = structSize;

            } 
            else 
            {
                return lineDevCap;
            }
        }
    }



    if (lineDevCap!=NULL)
        free(lineDevCap);

    return NULL;
}

/********************************************************************
Function    IsPhoneEdition

Arguments:    

Returns:    TRUE if Phone Edition

Comments:    checks if we're running a phone edition
*********************************************************************/
BOOL CIssTapi::IsPhoneEdition()
{
    HMODULE hDll = LoadLibrary(_T("\\windows\\ril.dll"));
    if(hDll)
    {
        FreeLibrary(hDll);
        return TRUE;
    }

    return FALSE;
}


/********************************************************************
Function    ProcTapis

Arguments:    see tapi documentation

Returns:    

Comments:    our static proc to recieve tapi messages
*********************************************************************/
void CIssTapi::ProcTAPIs(  DWORD hDevice, 
                           DWORD dwMessage, 
                           DWORD dwInstance,
                           DWORD dwParam1, 
                           DWORD dwParam2, 
                           DWORD dwParam3)
{
    if(g_poTapi)
        g_poTapi->TAPIProc(hDevice, dwMessage, dwInstance, dwParam1, dwParam2, dwParam3);
}

void CIssTapi::TAPIProc(  DWORD hDevice, 
                          DWORD dwMessage, 
                          DWORD dwInstance,
                          DWORD dwParam1, 
                          DWORD dwParam2, 
                          DWORD dwParam3)
{
    //DebugOut(_T("CIssTapi::TAPIProc - %d,%d,%d,%d,%d"), dwMessage, dwInstance,dwParam1, dwParam2, dwParam3);
    
    // make sure this is up and going
    if(!m_psLine)
        return;

    switch(dwMessage)
    {
    case LINE_APPNEWCALL:
		//DebugOut(_T("TAPIProc - LINE_APPNEWCALL"));
        HandleLineAppNewCall(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_CALLSTATE:
		DebugOut(_T("TAPIProc - LINE_CALLSTATE"));
        HandleLineCallState(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_LINEDEVSTATE:
		//DebugOut(_T("TAPIProc - LINE_LINEDEVSTATE"));
        HandleLineDevState(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_CALLINFO:
		//DebugOut(_T("TAPIProc - LINE_CALLINFO"));
        HandleLineCallInfo(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_CLOSE:
		//DebugOut(_T("TAPIProc - LINE_CLOSE"));
        HandleLineClose(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_REPLY:
		//DebugOut(_T("TAPIProc - LINE_REPLY"));
        HandleLineReply(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    case LINE_MONITORDIGITS:
		//DebugOut(_T("TAPIProc - LINE_MONITORDIGITS"));
        HandleLineMonitorDigits(hDevice, dwParam1, dwParam2, dwParam3);
        break;
    default:
        //DebugOut(_T("TAPIProc - Unhandled"));
        break;
    }
}

/********************************************************************
Function    GetCallerID

Arguments:    hCall - current call in place

Returns:    

Comments:    Get the current caller ID
*********************************************************************/
void CIssTapi::GetCallerID(HCALL hCall)
{
    m_oStr->Empty(m_sCallerID.szName);
    m_oStr->Empty(m_sCallerID.szNumber);

    // if we don't have a line open
    if(!m_psLine)
        return;

    LPLINECALLINFO lpCallInfo = GetCallInfo(hCall);
    if(!lpCallInfo)
        return;

    int        iNameSize;
    char    *pCharTemp;
    char    szTemp[STRING_MAX];
    DWORD    offset;
    int        i;

    m_oStr->Empty(szTemp);

    pCharTemp = (char *) lpCallInfo;

    // if there is a name
    if (lpCallInfo->dwCallerIDFlags & LINECALLPARTYID_NAME) 
    {
        offset        = lpCallInfo->dwCallerIDNameOffset;
        iNameSize    = ((lpCallInfo->dwCallerIDNameSize >= STRING_MAX)? STRING_MAX - 1 : lpCallInfo->dwCallerIDNameSize);

        for(i=0 ; i<iNameSize/2; i++)
            m_sCallerID.szName[i] = *(pCharTemp + offset + i);

        m_sCallerID.szName[i] = _T('\0');
    }

    // if there is an address
    if(lpCallInfo->dwCallerIDFlags & LINECALLPARTYID_ADDRESS)
    {
        offset        = lpCallInfo->dwCallerIDOffset;
        iNameSize    = ((lpCallInfo->dwCallerIDSize >= STRING_MAX)? STRING_MAX - 1 : lpCallInfo->dwCallerIDSize);

        for(i=0; i<iNameSize/2; i++)
            m_sCallerID.szNumber[i] = *(pCharTemp + offset + 2*i);

        m_sCallerID.szNumber[i] = _T('\0');
    }



}

/********************************************************************
Function    GetCallInfo

Arguments:    hCall - the current call

Returns:    

Comments:    Get the current call info
*********************************************************************/
LPLINECALLINFO CIssTapi::GetCallInfo(HCALL hCall)
{
    LONG lReturn;
    DWORD structSize;
    static LPLINECALLINFO lineCallInfo;

    lineCallInfo = NULL;

    if (hCall) 
    {
        lineCallInfo = (LPLINECALLINFO)calloc(1,sizeof(LINECALLINFO));
        lineCallInfo->dwTotalSize = sizeof(LINECALLINFO);

        // loop until we created the proper size for the struct
        for(int iCount=0;iCount<0xFF;iCount++)
        {
            lReturn = lineGetCallInfo(hCall, lineCallInfo);

            if(lReturn == 0 && 
                lineCallInfo->dwTotalSize >= lineCallInfo->dwNeededSize)// successfully created the lineCallInfo
                return lineCallInfo;
            //because apparently sometimes it IS ok to give it a smaller buffer .... dear god
            else if(lReturn == LINEERR_STRUCTURETOOSMALL || lineCallInfo->dwTotalSize < lineCallInfo->dwNeededSize)
            {
                //we need to resize the struct
				DebugOut(_T("GetCallInfo Error - Struct too Small"));
                structSize = lineCallInfo->dwNeededSize;
                free(lineCallInfo);
                lineCallInfo = (LPLINECALLINFO)calloc(1, structSize);
                lineCallInfo->dwTotalSize = structSize;
            }
            else //error
            {
                switch(lReturn)
                {
                case LINEERR_INVALCALLHANDLE:
					DebugOut(_T("GetCallInfo Error - LINEERR_INVALCALLHANDLE"));
                    break;
                case LINEERR_RESOURCEUNAVAIL:
					DebugOut(_T("GetCallInfo Error - LINEERR_RESOURCEUNAVAIL"));
                    break;
                case LINEERR_INVALPOINTER:
					DebugOut(_T("GetCallInfo Error - LINEERR_INVALPOINTER"));
                    break;
                case LINEERR_NOMEM:
					DebugOut(_T("GetCallInfo Error - LINEERR_NOMEM"));
                    break;
                case LINEERR_UNINITIALIZED:
					DebugOut(_T("GetCallInfo Error - LINEERR_UNINITIALIZED"));
                    break;
                case LINEERR_OPERATIONFAILED:
					DebugOut(_T("GetCallInfo Error - LINEERR_OPERATIONFAILED"));
                    break;
                case LINEERR_OPERATIONUNAVAIL:
					DebugOut(_T("GetCallInfo Error - LINEERR_OPERATIONUNAVAIL"));
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }

    if (lineCallInfo != NULL) 
        free(lineCallInfo);
    return NULL;//something went wrong    
}

/********************************************************************
Function    GetLineAddressCaps

Arguments:    

Returns:    

Comments:    Get the line capabilities
*********************************************************************/
LPLINEADDRESSCAPS CIssTapi::GetLineAddressCaps()
{
    LONG lReturn;
    DWORD structSize;
    static LPLINEADDRESSCAPS lineAddressCaps;

    lineAddressCaps = NULL;

    if (m_psLine == NULL)
        return NULL;

    lineAddressCaps = (LPLINEADDRESSCAPS)calloc(1,sizeof(LINEADDRESSCAPS));
    lineAddressCaps->dwTotalSize = sizeof(LINEADDRESSCAPS);

    for(;;) 
    {
        lReturn = lineGetAddressCaps(m_psLine->hLineApp,m_psLine->dwCurrLine,m_psLine->dwCurrAddr,    m_psLine->dwVersion, 0, lineAddressCaps);
        if (lReturn!=0) 
            break;
        else 
        {
            if (lineAddressCaps->dwTotalSize < lineAddressCaps->dwNeededSize) 
            {
                structSize = lineAddressCaps->dwNeededSize;
                free(lineAddressCaps);
                lineAddressCaps = (LPLINEADDRESSCAPS)calloc(1, structSize);
                lineAddressCaps->dwTotalSize = structSize;

            } 
            else 
                return lineAddressCaps;
        }
    }


    if (lineAddressCaps != NULL)
        free(lineAddressCaps);

    // some error occured
    return NULL;
}

/********************************************************************
Function    AcceptLine

Arguments:    

Returns:    

Comments:    Pick up the call
*********************************************************************/
void CIssTapi::AcceptLine()
{
    if (m_psLine == NULL)
        return;

    if (m_psLine->hCallCurr != (HCALL) NULL) 
    {
        LONG lReturn = lineAccept(m_psLine->hCallCurr, NULL, 0);
		DebugOut(_T("AcceptLine - LineAccept %l"), lReturn);
    }
}

/********************************************************************
Function    DeallocateCall

Arguments:    

Returns:    

Comments:    clean up any active calls and return memory
*********************************************************************/
void CIssTapi::DeallocateCall(HCALL hCall)
{
    if(!hCall)
        return;

    LPLINECALLSTATUS pCallStatus = GetCallStatus(hCall);
    if(pCallStatus)
    {
        if(pCallStatus->dwCallState & LINECALLSTATE_IDLE)
            lineDeallocateCall(hCall);
        
        free(pCallStatus);
    }
}

/********************************************************************
Function    GetCallStatus

Arguments:    hCall - current call

Returns:    

Comments:    get the status of the current call
*********************************************************************/
LPLINECALLSTATUS CIssTapi::GetCallStatus(HCALL hCall)
{
    LONG lReturn;
    DWORD structSize;
    static LPLINECALLSTATUS lineCallStatus = NULL;

    if (!hCall)
        return NULL;

    if(!lineCallStatus)
    {
        lineCallStatus = (LPLINECALLSTATUS)calloc(1,sizeof(LINECALLSTATUS));
        if(!lineCallStatus)
            return NULL;
        lineCallStatus->dwTotalSize = sizeof(LINECALLSTATUS);
    }

    for(;;) 
    {
        lReturn = lineGetCallStatus(hCall, lineCallStatus);
        if (lReturn!=0) 
		{
			// debug
			int a = 0;
			switch(lReturn)
			{
			case LINEERR_INVALCALLHANDLE:
				break;
			case LINEERR_RESOURCEUNAVAIL:
				break;
			case LINEERR_INVALPOINTER:
				break;
			case LINEERR_STRUCTURETOOSMALL:
				break;
			case LINEERR_NOMEM:
				break;
			case LINEERR_UNINITIALIZED:
				break;
			case LINEERR_OPERATIONFAILED:
				break;
			case LINEERR_OPERATIONUNAVAIL:
				break;

			}
			break;
		}
        else 
        {
            if (lineCallStatus->dwTotalSize < lineCallStatus->dwNeededSize) 
            {
                structSize = lineCallStatus->dwNeededSize;
                free(lineCallStatus);
                lineCallStatus = NULL;
                lineCallStatus = (LPLINECALLSTATUS)calloc(1, structSize);
                lineCallStatus->dwTotalSize = structSize;

            } 
            else 
                return lineCallStatus;
        }
    }

    if (lineCallStatus != NULL)
    {
        free(lineCallStatus);
        lineCallStatus = NULL;
    }

    return NULL;
}

/********************************************************************
Function    DropCall

Arguments:    hCall - Current Call

Returns:    

Comments:    Drop the call 
*********************************************************************/
void CIssTapi::DropCall(HCALL hCall)
{
    static    int bReentered = FALSE;

    if (bReentered)
        return;
    
    bReentered = TRUE;

    // Use g_dwLineDropID to prevent lineDrop twice.
    if (hCall && (!m_lDropCallRequestID)) 
    {
        LPLINECALLSTATUS pCallStatus = GetCallStatus(hCall);

        if (pCallStatus != NULL) 
        {
            if (!(pCallStatus->dwCallState & LINECALLSTATE_IDLE)) 
            {
                m_lDropCallRequestID = lineDrop(hCall, NULL, 0);

                /********************
                *    Error happened    *
                *********************/
                if ( ((LONG) m_lDropCallRequestID) < 0) 
                {
                    /****************************
                    *    Deallocate current call    *
                    *****************************/
                    DeallocateCall(hCall);
                    if (m_psLine->hCallCurr == hCall)
                        m_psLine->hCallCurr = (HCALL) NULL;

                    /********************************************
                    *    Close TAPI and Reinitialize it again.    *
                    *********************************************/
                    Destroy();
                    Initialize(m_hInstance, m_hWndDebug);
                }
            }
            free(pCallStatus);
        }
    }
    bReentered = FALSE;

}

/********************************************************************
Function    GetDeviceID

Arguments:    

Returns:    

Comments:    Get the current device ID
*********************************************************************/
BOOL CIssTapi::GetDeviceID(TCHAR* szDeviceClass, UINT *iDeviceID)
{
    LONG lReturn;
    DWORD structSize;
    LPVARSTRING lpVarString = NULL;

    lpVarString = NULL;

    if (m_psLine==NULL) 
        return FALSE;
    if(m_psLine->hCallCurr == NULL)
        return FALSE;
    

    lpVarString = (LPVARSTRING)calloc(1, sizeof(VARSTRING));
    if(!lpVarString)
        return FALSE;
    lpVarString->dwTotalSize = sizeof(VARSTRING);

    for(;;) 
    {
        //lReturn = lineGetID(m_psLine->hCurrLine, m_psLine->dwCurrAddr, m_psLine->hCallCurr, LINECALLSELECT_LINE, lpVarString, szDeviceClass);
        lReturn = lineGetID(0, 0, m_psLine->hCallCurr, LINECALLSELECT_CALL, lpVarString, szDeviceClass);

        if (lReturn!=0) 
            break;
        else 
        {
            if (lpVarString->dwTotalSize < lpVarString->dwNeededSize) 
            {
                structSize = lpVarString->dwNeededSize;
                free(lpVarString);
                lpVarString = NULL;

                lpVarString = (LPVARSTRING)calloc(1, structSize);
                if(!lpVarString)
                    return FALSE;
                lpVarString->dwTotalSize = structSize;
            } 
            else 
            {
                *iDeviceID = *((UINT *) ((LPBYTE) lpVarString +    lpVarString->dwStringOffset));

                free(lpVarString);
                lpVarString = NULL;
                return  TRUE;
            }
        }
    }

    if (lpVarString!=NULL)
        free(lpVarString);

    return FALSE;
}

/********************************************************************
Function    CloseLine

Arguments:    

Returns:    

Comments:    Close the monitoring lines
*********************************************************************/
void CIssTapi::CloseLine()
{
    if (!m_psLine)
        return;

    if(!m_psLine->hCallCurr)
        return;
    
    LONG lReturn = lineClose(m_psLine->hCurrLine);
    m_psLine->hCurrLine = (HLINE) NULL;

}

/********************************************************************
Function    AnswerLine

Arguments:    

Returns:    

Comments:    Answer the call 
*********************************************************************/
void CIssTapi::AnswerLine()
{
    if (!m_psLine)
        return;
    
    if(!m_psLine->hCallCurr)
        return;

    // Initiate this value to prevent lineDrop twice.
    m_lDropCallRequestID = 0;
    m_lMakeCallRequestID = lineAnswer(m_psLine->hCallCurr,NULL,0);

    switch(m_lMakeCallRequestID)
    {
    case LINEERR_INUSE:
		DebugOut(_T("ANS CALL - Line In User"));
        break;
    case LINEERR_INVALCALLHANDLE:
		DebugOut(_T("ANS CALL - Invalid Call Handle"));
        break;
    case LINEERR_OPERATIONFAILED:
		DebugOut(_T("ANS CALL - Operation Failed"));
        break;
    case LINEERR_INVALCALLSTATE:
		DebugOut(_T("ANS CALL - Invalid Call State"));
        break;
    case LINEERR_RESOURCEUNAVAIL:
		DebugOut(_T("ANS CALL - Resource Unaval"));
        break;
    case LINEERR_INVALPOINTER:
		DebugOut(_T("ANS CALL - Invalid Pointer"));
        break;
    case LINEERR_UNINITIALIZED:
		DebugOut(_T("ANS CALL - Unintialized"));
        break;
    case LINEERR_NOMEM:
		DebugOut(_T("ANS CALL - Not enough Memory"));
        break;
    case LINEERR_USERUSERINFOTOOBIG:
		DebugOut(_T("ANS CALL - User info is too big"));
        break;
    case LINEERR_NOTOWNER:
		DebugOut(_T("ANS CALL - App doesn't own the call"));
        break;
    default:
		DebugOut(_T("ANS CALL - Call answered"));
        break;
    }

    if ( ((LONG)m_lMakeCallRequestID) < 0) 
    {
        /************************************
        *    Error happened while answering    *
        *************************************/
        DropCall(m_psLine->hCallCurr);
    }

}

/********************************************************************
Function    SetAppTopPriority

Arguments:    

Returns:    

Comments:    Try and set ourselves so the app is first to get TAPI messages
*********************************************************************/
void CIssTapi::SetAppTopPriority(DWORD dwMediaMode)
{
    TCHAR szAppPath[STRING_MAX];

    GetModuleFileName(NULL, szAppPath, STRING_MAX);

    int r = (int)lineSetAppPriority(szAppPath,dwMediaMode, 0,0,0,1);

	DebugOut(_T("lineSetAppApriority - %d"), r);
}

/********************************************************************
Function    MakeCanonicalNum

Arguments:    szPhoneNum - retrieve and set number
            szAreaCode - current area code
            bUseSpaces - wether spaces should be used or not

Returns:    

Comments:    Return the phone number in canonical form
*********************************************************************/
BOOL CIssTapi::MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces)
{
    int    iLength        = 0;
    int    iStartPos    = 0;

    CIssString* oStr = CIssString::Instance();

    if(oStr->IsEmpty(szPhoneNum))
        return FALSE;

    TCHAR szCanPhoneNum[TAPIMAXDESTADDRESSSIZE + 1];

    for(int i = 0; i < oStr->GetLength(szPhoneNum); i++)
    {
        if (iswdigit (szPhoneNum[i]))
        {
            szPhoneNum[iLength] = szPhoneNum[i];
            iLength += 1;
        }
    }

    // Terminate the string with NULL.
    szPhoneNum[iLength] = _T('\0');

    if (iLength == 10)
    {
        //Make the phone number in the format "+1 (xxx) xxx-xxxx"
        oStr->StringCopy(szCanPhoneNum, _T("+1 "));
        szCanPhoneNum[3]  = '(';
        szCanPhoneNum[4]  = szPhoneNum[iStartPos];
        szCanPhoneNum[5]  = szPhoneNum[iStartPos + 1];
        szCanPhoneNum[6]  = szPhoneNum[iStartPos + 2];
        szCanPhoneNum[7]  = _T(')');
        szCanPhoneNum[8]  = _T(' ');
        szCanPhoneNum[9]  = szPhoneNum[iStartPos + 3];
        szCanPhoneNum[10] = szPhoneNum[iStartPos + 4];
        szCanPhoneNum[11] = szPhoneNum[iStartPos + 5];
        szCanPhoneNum[12] = _T('-');
        szCanPhoneNum[13] = szPhoneNum[iStartPos + 6];
        szCanPhoneNum[14] = szPhoneNum[iStartPos + 7];
        szCanPhoneNum[15] = szPhoneNum[iStartPos + 8];
        szCanPhoneNum[16] = szPhoneNum[iStartPos + 9];
        szCanPhoneNum[17] = _T('\0');

        if(szAreaCode != NULL)
        {
            oStr->Empty(szAreaCode);
            szAreaCode[0] = szPhoneNum[iStartPos];
            szAreaCode[1] = szPhoneNum[iStartPos + 1];
            szAreaCode[2] = szPhoneNum[iStartPos + 2];
            szAreaCode[3] = _T('\0');
        }
    }
    else if(iLength == 11)
    {
        if (szPhoneNum[0] != '1') //if its 11 digits it better start with a 1 for now ... 
            //could also be xx xx xxx xxxx but we'll worry about that later
            return FALSE;      
        else
        {
            //Make the phone number in the format "+1 (xxx) xxx-xxxx"
            szCanPhoneNum[0]  = _T('+');
            szCanPhoneNum[1]  = szPhoneNum[iStartPos];
            szCanPhoneNum[2]  = _T(' ');
            szCanPhoneNum[3]  = _T('(');
            szCanPhoneNum[4]  = szPhoneNum[iStartPos + 1];
            szCanPhoneNum[5]  = szPhoneNum[iStartPos + 2];
            szCanPhoneNum[6]  = szPhoneNum[iStartPos + 3];
            szCanPhoneNum[7]  = _T(')');
            szCanPhoneNum[8]  = _T(' ');
            szCanPhoneNum[9]  = szPhoneNum[iStartPos + 4];
            szCanPhoneNum[10] = szPhoneNum[iStartPos + 5];
            szCanPhoneNum[11] = szPhoneNum[iStartPos + 6];
            szCanPhoneNum[12] = _T('-');
            szCanPhoneNum[13] = szPhoneNum[iStartPos + 7];
            szCanPhoneNum[14] = szPhoneNum[iStartPos + 8];
            szCanPhoneNum[15] = szPhoneNum[iStartPos + 9];
            szCanPhoneNum[16] = szPhoneNum[iStartPos + 10];
            szCanPhoneNum[17] = _T('\0');

            if(szAreaCode != NULL)
            {
                oStr->Empty(szAreaCode);
                szAreaCode[0] = szPhoneNum[iStartPos + 1];
                szAreaCode[1] = szPhoneNum[iStartPos + 2];
                szAreaCode[2] = szPhoneNum[iStartPos + 3];
                szAreaCode[3] = _T('\0');
            }
        }
    }
    else
    {
        return FALSE;
    }

    // Copy the newly created phone number back to lpszPhoneNum.
    oStr->StringCopy(szPhoneNum, szCanPhoneNum);

    return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
// Function: SHGetPhoneNumber
// szNumber - Out Buffer for the phone number
// cchNumber - size of sznumber in characters
// nLineNumber - In which phone line (1 or 2) to get the number for
/////////////////////////////////////////////////////////////////////////////
HRESULT CIssTapi::GetPhoneNumber(LPTSTR szNumber, UINT cchNumber, UINT nLineNumber)
{
	HRESULT  hr = E_FAIL;
	LRESULT  lResult = 0;
	HLINEAPP hLineApp;
	DWORD    dwNumDevs;  //number of line devices
	DWORD    dwAPIVersion = TAPI_API_HIGH_VERSION;
	LINEINITIALIZEEXPARAMS liep;

#define CAPS_BUFFER_SIZE    512

	DWORD dwTAPILineDeviceID;
	const DWORD dwAddressID = nLineNumber - 1;
	BYTE* pCapBuf = NULL;

	liep.dwTotalSize = sizeof(liep);
	liep.dwOptions   = LINEINITIALIZEEXOPTION_USEEVENT;

	//initialize line before accessing
	if (SUCCEEDED(lineInitializeEx(&hLineApp, 0, 0, TEXT("ExTapi_Lib"), &dwNumDevs, &dwAPIVersion, &liep)))
	{

		DWORD dwCapBufSize = CAPS_BUFFER_SIZE;
		LINEEXTENSIONID  LineExtensionID;
		LINEDEVCAPS*     pLineDevCaps = NULL;
		LINEADDRESSCAPS* placAddressCaps = NULL;

		pCapBuf = new BYTE[dwCapBufSize];
		EXIT_ON_NULL(pCapBuf);

		pLineDevCaps = (LINEDEVCAPS*)pCapBuf;
		pLineDevCaps->dwTotalSize = dwCapBufSize;

		// Get TSP Line Device ID
		dwTAPILineDeviceID = 0xffffffff;
		for (DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumDevs ; dwCurrentDevID++)
		{
			//ensure TAPI, service provider, and application are all using the same versions
			if (0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, TAPI_API_LOW_VERSION, TAPI_API_HIGH_VERSION,
				&dwAPIVersion, &LineExtensionID))
			{
				lResult = lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, pLineDevCaps);

				//increase buffer size if too small to hold the device capabilities
				if (dwCapBufSize < pLineDevCaps->dwNeededSize)
				{
					if(pCapBuf)
					{
						delete[] pCapBuf;
						pCapBuf = NULL;
					}
					dwCapBufSize = pLineDevCaps->dwNeededSize;
					pCapBuf = new BYTE[dwCapBufSize];
					EXIT_ON_NULL(pCapBuf);

					pLineDevCaps = (LINEDEVCAPS*)pCapBuf;
					pLineDevCaps->dwTotalSize = dwCapBufSize;

					lResult = lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, pLineDevCaps);
				}
				//lResult of 0 means the device capabilities were successfully returned
				if ((0 == lResult) &&
					(0 == _tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), CELLTSP_LINENAME_STRING)))
				{
					dwTAPILineDeviceID = dwCurrentDevID;
					break;
				}
			} 
		} 

		placAddressCaps = (LINEADDRESSCAPS*)pCapBuf;
		placAddressCaps->dwTotalSize = dwCapBufSize;

		lResult = lineGetAddressCaps(hLineApp, dwTAPILineDeviceID, dwAddressID, dwAPIVersion, 0, placAddressCaps);

		//increase buffer size if too small to hold the address capabilities
		if (dwCapBufSize < placAddressCaps->dwNeededSize)
		{
			if(pCapBuf)
			{
				delete[] pCapBuf;
				pCapBuf = NULL;
			}
			dwCapBufSize = placAddressCaps->dwNeededSize;
			pCapBuf = new BYTE[dwCapBufSize];
			EXIT_ON_NULL(pCapBuf);

			placAddressCaps = (LINEADDRESSCAPS*)pCapBuf;
			placAddressCaps->dwTotalSize = dwCapBufSize;

			lResult = lineGetAddressCaps(hLineApp, dwTAPILineDeviceID, dwAddressID, dwAPIVersion, 0, placAddressCaps);
		}
		//lResult of 0 means the address capabilities were successfully returned
		if (0 == lResult)
		{
			if (szNumber)
			{
				szNumber[0] = TEXT('\0');

				EXIT_ON_FALSE(0 != placAddressCaps->dwAddressSize);

				// A non-zero dwAddressSize means a phone number was found
				ASSERT(0 != placAddressCaps->dwAddressOffset);
				PWCHAR tsAddress = (WCHAR*)(((BYTE*)placAddressCaps)+placAddressCaps->dwAddressOffset);

				StringCchCopy(szNumber, cchNumber, tsAddress);
			}

			hr = S_OK;
		} 

		if(pCapBuf)
		{
			delete[] pCapBuf;
			pCapBuf = NULL;
		}
	} // End if ()

FuncExit:
	if(pCapBuf)
	{
		delete[] pCapBuf;
		pCapBuf = NULL;
	}

	lineShutdown(hLineApp);

	return hr;
}
