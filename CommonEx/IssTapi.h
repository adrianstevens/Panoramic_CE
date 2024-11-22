/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssTapi.h
// Abstract: encapsulates TAPI functionality
//  
/***************************************************************************************************************/

#pragma once

#include "ole2.h"
#include "tapi.h"
#include "IssString.h"

#define WM_TAPI_Output		WM_USER + 400		// output tapi messages

struct LINESTRUCT
{
	int				iCurrStatus;			//	Current Status
	HLINEAPP 		hLineApp;				//	Line application
	DWORD			dwVersion;				//	TAPI Version
	DWORD			dwNumLines;				//	Number of lines
	DWORD			dwCurrLine;				//	Current line in Use
	HLINE			hCurrLine;				//	Current Line Handler
	DWORD			dwCurrAddr;				//	Current line Address
	HCALL			hCallCurr;				//	Call Handler
	int				iCallType;				//	InBound or OutBound
	DWORD			dwMediaMode;			//	Call media modes
	UINT			uiInputDev;				//	Input Device
	UINT			uiOutPutDev;			//	Output Device
	LINECALLBACK    LineCallback;			//	CallBack Function
};

struct CALLERIDINFO
{
    TCHAR    szNumber[STRING_MAX];
    TCHAR    szName[STRING_MAX];
	long	lOid;
	class CIssGDIEx* gdiPicture;
	CALLERIDINFO();
	~CALLERIDINFO();
};


class CIssTapi  
{
public:
    CIssTapi();
    virtual ~CIssTapi();

	static CIssTapi*    Instance();
    virtual HRESULT     Initialize(HINSTANCE hInst, HWND hWndDebugWindow = NULL);
    BOOL                IsPhoneEdition();
	BOOL                SetFlyMode(BOOL bFlying);
	BOOL                IsFlightMode();
    HRESULT             Destroy();
	static HRESULT GetPhoneNumber(LPTSTR szNumber, UINT cchNumber, UINT nLineNumber);
    static BOOL         MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces);

protected:
#ifdef USE_TAPI_DEBUGOUT
	void DebugOut(TCHAR* szFormat, ...);
#endif
    virtual void HandleLineCallState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineDevState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineReply(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineClose(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineCallInfo(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineMonitorDigits(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void HandleLineAppNewCall(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};
    virtual void GetCallerID(HCALL hCall);
    virtual void AcceptLine();
    virtual void DropCall(HCALL hCall);

    BOOL                GetDeviceID(TCHAR* szDeviceClass, UINT *iDeviceID);
    void                DeallocateCall(HCALL hCall);
    LPLINECALLINFO      GetCallInfo(HCALL hCall);
    LPLINEADDRESSCAPS   GetLineAddressCaps();
    LPLINECALLSTATUS    GetCallStatus(HCALL hCall);
    LPLINEDEVCAPS       GetDeviceCaps();
    void                CloseLine();
    void                AnswerLine();
    void                SetAppTopPriority(DWORD dwMediaMode);

    

private:
    static void ProcTAPIs(DWORD hDevice, 
                        DWORD dwMessage, 
                        DWORD dwInstance,
                        DWORD dwParam1, 
                        DWORD dwParam2, 
                        DWORD dwParam3);
    void TAPIProc(DWORD hDevice, 
                        DWORD dwMessage, 
                        DWORD dwInstance,
                        DWORD dwParam1, 
                        DWORD dwParam2, 
                        DWORD dwParam3);

protected:    // variables
    LINESTRUCT*  m_psLine;               // keeps all the line information
    HINSTANCE    m_hInstance;            // hInstance of the application
    
    LONG         m_lMakeCallRequestID;   // Request identifier returned by 
                                         // lineMakeCall.
    DWORD        m_lDropCallRequestID;   // Request identifier returned by 
                                         // lineDrop.
    BOOL         m_bCurrentLineAvail;    // Indicates line availability
    DWORD        m_dwNumDevs;            // Number of line devices available
    CIssString*  m_oStr;                 // String class
    TCHAR*       m_szDebug;              // string for debugging info
    FILE*        m_hDebugFile;           // File handle to output debugging info
    HWND         m_hWndDebug;            // debug window to send outputs
    CALLERIDINFO m_sCallerID;            // Caller ID structure
};