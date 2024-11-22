/* (C) 2003 XDA Developers  itsme@xs4all.nl
 *
 * $Header$
 */
#ifndef __RIL_H__
#define __RIL_H__

// not all constants and structures are entered here yet.
// see http://ofi.epoline.org/view/GetDossier?dosnum=1233343
//
// based on ril.h from tstril 
// ( see http://xda-developers.com/RIL )
//
#ifdef __cplusplus
extern "C" {
#endif

#define RIL_PARAM_MDCS_TYPE				(0x00000001)
#define RIL_PARAM_MDCS_FLAGS			(0x00000002)
#define RIL_PARAM_MDCS_MSGCLASS			(0x00000004)
#define RIL_PARAM_MDCS_INDICATION		(0x00000008)
#define RIL_PARAM_MDCS_LANGUAGE			(0x00000010)
#define RIL_PARAM_MDCS_ALL				(0x0000001F)

#define RIL_MSG_IN_DELIVER				(0x00010001)
#define RIL_MSG_IN_STATUS				(0x00010002)

#define RIL_MSGFLAG_NONE					(0x00000000)
#define RIL_MSGFLAG_MORETOSEND				(0x00000001)
#define RIL_MSGFLAG_REPLYPATH				(0x00000002)
#define RIL_MSGFLAG_HEADER					(0x00000004)
#define RIL_MSGFLAG_REJECTDUPS				(0x00000008)
#define RIL_MSGFLAG_STATUSREPORTRETURNED	(0x00000010)
#define RIL_MSGFLAG_STATUSREPORTREQUESTED	(0x00000020)
#define RIL_MSGFLAG_CAUSEDBYCOMMAND			(0x00000040)
#define RIL_MSGFLAG_ALL						(0x000000FF)


#define RIL_DCSTYPE_GENERAL				(0x00000001)
#define RIL_DCSTYPE_MSGWAIT				(0x00000002)
#define RIL_DCSTYPE_MSGCLASS			(0x00000004)
#define RIL_DCSTYPE_LANGUAGE			(0x00000008)

/* uncertain */
#define RIL_NCLASS_FUNCRESULT			(0x80000000)

/* verified from RSUpgrade.exe */
#define RIL_NCLASS_CALLCTRL				(0x00010000)
#define RIL_NCLASS_MESSAGE				(0x00020000)
#define RIL_NCLASS_NETWORK				(0x00040000)
#define RIL_NCLASS_SUPSERVICE			(0x00080000)

/* uncertain */
#define RIL_NCLASS_PHONEBOOK			(0x00100000)
#define RIL_NCLASS_SIMTOOLKIT			(0x00200000)
#define RIL_NCLASS_MISC					(0x00400000)
#define RIL_NCLASS_RADIOSTATE			(0x00800000)

#define RIL_NCLASS_ALL					(0x00FF0000)

// Notify class Call Control

#define RIL_NOTIFY_RING					(0x00000001)
#define RIL_NOTIFY_CONNECT				(0x00000002)
#define RIL_NOTIFY_DISCONNECT			(0x00000003)
#define RIL_NOTIFY_DATASVCNEGOTIATED	(0x00000004)
#define RIL_NOTIFY_CALLSTATECHANGED		(0x00000005)
#define RIL_NOTIFY_EMERGENCYMODEENTERED	(0x00000006)
#define RIL_NOTIFY_EMERGENCYMODEEXITED	(0x00000007)
#define RIL_NOTIFY_EMERGENCYHANGUP		(0x00000008)
#define RIL_NOTIFY_HSCSDPARMSNEGOTIATED	(0x00000009)
#define RIL_NOTIFY_DIAL					(0x0000000A)

// Notify class Message

#define RIL_NOTIFY_MESSAGE				(0x00000001)
#define RIL_NOTIFY_BCMESSAGE			(0x00000002)
#define RIL_NOTIFY_STATUSMESSAGE		(0x00000003)
#define RIL_NOTIFY_MSGSTORED			(0x00000004)
#define RIL_NOTIFY_MSGDELETED			(0x00000005)
#define RIL_NOTIFY_MSGSTORAGECHANGED	(0x00000006)
#define RIL_NOTIFY_MESSAGE_IN_SIM		(0x00000007)
#define RIL_NOTIFY_BCMESSAGE_IN_SIM		(0x00000008)
#define RIL_NOTIFY_STATUSMESSAGE_IN_SIM	(0x00000009)

/* unknown, guessed */
#define RIL_SENDOPT_NONE				(0x00000001)
#define RIL_SENDOPT_PERSISTLINK			(0x00000002)


#define MAXLENGTH_ADDRESS				256
#define MAXLENGTH_SUBADDR				256
#define MAXLENGTH_DESCRIPTION			256
#define MAXLENGTH_OPERATOR				32
#define MAXLENGTH_OPERATOR_LONG			32
#define MAXLENGTH_OPERATOR_SHORT		16
#define MAXLENGTH_OPERATOR_NUMERIC		16
#define MAXLENGTH_SERVCTR				256
#define MAXLENGTH_PASSWORD				256
#define MAXLENGTH_ERRSHORT				256
#define MAXLENGTH_ERRLONG				256
#define MAXLENGTH_EQUIPINFO				256
#define MAXLENGTH_PHONEBOOKADDR			256
#define MAXLENGTH_PHONEBOOKTEXT			256
#define MAXLENGTH_CURRENCY				256
#define MAXLENGTH_AREAID				256
#define MAXLENGTH_CELLID				256
#define MAXLENGTH_HDR					256
#define MAXLENGTH_MSG					256
#define MAXLENGTH_CMD					256
#define MAXLENGTH_MSGIDS				256
#define MAXLENGTH_USERID				256
#define MAXLENGTH_DTMF					256
#define MAXLENGTH_GPRSADDRESS			256
#define MAXLENGTH_GPRSACCESSPOINTNAME	256


	
typedef DWORD HRIL;

typedef struct {
    DWORD cbSize;
    DWORD dwParams;
    DWORD dwReadBitsPerSecond;
    DWORD dwWrittenBitsPerSecond;
} RILSERIALPORTSTATS;

typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	DWORD dwType;
	DWORD dwNumPlan;
	WCHAR wszAddress[MAXLENGTH_ADDRESS];
} RILADDRESS;
typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	DWORD dwType;
	WCHAR wszSubAddress[MAXLENGTH_SUBADDR];
} RILSUBADDRESS;


typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	DWORD dwType;
	DWORD dwFlags;
	DWORD dwMsgClass;
	DWORD dwAlphabet;
	DWORD dwIndication;
	DWORD dwLanguage;
} RILMSGDCS;

typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	RILADDRESS raSvcCtrAddress;
	DWORD dwType;
	DWORD dwFlags;

	union {

		struct {
			RILADDRESS raOrigAddress;
			DWORD dwProtocolID;
			RILMSGDCS rmdDataCoding;
			SYSTEMTIME stSCReceiveTime;
			DWORD cbHdrLength;
			DWORD cchMsgLength;
			BYTE rgbHdr[MAXLENGTH_HDR];
			BYTE rgbMsg[MAXLENGTH_MSG];
		} msgInDeliver;

		struct {
			DWORD dwTgtMsgReference;
			RILADDRESS raTgtRecipAddress;
			SYSTEMTIME stTgtSCReceiveTime;
			SYSTEMTIME stTgtDischargeTime;
			DWORD dwTgtDlvStatus;
			DWORD dwProtocolID;
			RILMSGDCS rmdDataCoding;
			DWORD cbHdrLength;
			DWORD cchMsgLength;
			BYTE rgbHdr[MAXLENGTH_HDR];
			BYTE rgbMsg[MAXLENGTH_MSG];
		} msgInStatus;

		struct {
			RILADDRESS raDestAddress;
			DWORD dwProtocolID;
			RILMSGDCS rmdDataCoding;
			DWORD dwVPFormat;
			SYSTEMTIME stVP;
			DWORD cbHdrLength;
			DWORD cchMsgLength;
			BYTE rgbHdr[MAXLENGTH_HDR];
			BYTE rgbMsg[MAXLENGTH_MSG];
		} msgOutSubmit;

		struct {
			DWORD dwProtocolID;
			DWORD dwCommandType;
			DWORD dwTgtMsgReference;
			RILADDRESS raDestAddress;
			DWORD cbCmdLength;
			BYTE rgbCmd[MAXLENGTH_CMD];
		} msgOutCommand;

		struct {
			DWORD dwGeoScope;
			DWORD dwMsgCode;
			DWORD dwUpdateNumber;
			DWORD dwID;
			RILMSGDCS rmdDataCoding;
			DWORD dwTotalPages;
			DWORD dwPageNumber;
			DWORD cchMsgLength;
			BYTE rgbMsg[MAXLENGTH_MSG];
		} msgBcGeneral;

		// Maybe incomplete ...
		struct {
			DWORD cchMsgLength;
			BYTE rgbMsg[MAXLENGTH_MSG];
		} msgOutRaw;
	};

} RILMESSAGE;


typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	DWORD dwIndex;
	DWORD dwStatus;
	RILMESSAGE rmMessage;
} RILMESSAGEINFO;

typedef struct {
	DWORD cbSize;
	DWORD dwParams;
	DWORD dwMobileCountryCode;
	DWORD dwMobileNetworkCode;
	DWORD dwLocationAreaCode;
	DWORD dwCellID;
	DWORD dwBaseStationID;
	DWORD dwBroadcastControlChannel;
	DWORD dwRxLevel;
	DWORD dwRxLevelFull;
	DWORD dwRxLevelSub;
	DWORD dwRxQuality;
	DWORD dwRxQualityFull;
	DWORD dwRxQualitySub;
	DWORD dwIdleTimerSlot;
	DWORD dwTimingAdvance;
	DWORD dwGPRSCellID;
	DWORD dwGPRSBaseStationID;
} RILCELLTOWERINFO;


typedef void (CALLBACK *RILNOTIFYCALLBACK)(DWORD dwCode, const void *lpData, 
										   DWORD cbdata, DWORD dwParam);

typedef void (CALLBACK *RILRESULTCALLBACK)(DWORD dwCode, HRESULT hrCmdID, 
										   const void *lpData, DWORD cbdata, DWORD dwParam);


/* Function prototypes, based on an export from the DLL and info from the patent */

//HRESULT RIL_AddCallForwarding(HRIL hRil, DWORD dwReason, const RILCALLFORWARDINGSETTINGS*lpSettings, DWORD dwAddressId);
//HRESULT RIL_AddPreferredOperator(HRIL hRil, DWORD dwIndex, const RILOPERATORNAMES *lpOperatorNames);

HRESULT RIL_Answer(HRIL hRil);
HRESULT RIL_CancelSupServiceDataSession(HRIL hRil);
HRESULT RIL_ChangeCallBarringPassword(HRIL hRil, DWORD dwType, LPCSTR lpwszOldPassword, LPCSTR lpwszNewPassword);
HRESULT RIL_ChangeLockingPassword(HRIL hRil, DWORD dwFacility, LPCSTR lpwszOldPassword, LPCSTR lpwszNewPassword);
HRESULT RIL_ClearCCBSRegistration(HRIL hRil, DWORD dwCCBSIndex);

HRESULT RIL_Deinitialize(HRIL hril);

HRESULT RIL_DeleteGPRSContext(HRIL hRil, DWORD dwContextID);
HRESULT RIL_DeleteMinimumQualityOfService(HRIL hRil, DWORD dwContextID);
HRESULT RIL_DeleteMsg(HRIL hRil, DWORD dwIndex);
HRESULT RIL_DeletePhonebookEntry(HRIL hRil, DWORD dwIndex);
HRESULT RIL_DeleteRequestedQualityOfService(HRIL hRil, DWORD dwContextID);

HRESULT RIL_DevSpecific(HRIL hRil, BYTE*params, DWORD dwSize);

HRESULT RIL_Dial(HRIL hRil, LPCSTR lpszAddress, DWORD dwType, DWORD dwOptions);
HRESULT RIL_DisableNotifications(HRIL hRil, DWORD dwNotificationClasses);
HRESULT RIL_EnableNotifications(HRIL hRil, DWORD dwNotificationClasses);

//HRESULT RIL_EnterGPRSDataMode(HRIL hRil, const RILENTERGPRSDATAMODE* lpEnterGprsDataMode);
HRESULT RIL_FetchSimToolkitCmd(HRIL hRil);
HRESULT RIL_GetAudioDevices(HRIL hRil);
HRESULT RIL_GetAudioGain(HRIL hRil);
HRESULT RIL_GetAudioMuting(HRIL hRil);
HRESULT RIL_GetBearerServiceOptions(HRIL hRil);
HRESULT RIL_GetCallBarringStatus(HRIL hRil, DWORD dwType, DWORD dwInfoClass, LPCSTR lpszPassword);
HRESULT RIL_GetCallerIdSettings(HRIL hRil);
HRESULT RIL_GetCallForwardingSettings(HRIL hRil, DWORD dwReason, DWORD dwAddressId);
HRESULT RIL_GetCallList(HRIL hRil);
HRESULT RIL_GetCallWaitingSettings(HRIL hRil, DWORD dwInfoClass);
HRESULT RIL_GetCCBSStatus(HRIL hRil, DWORD dwCCBSIndex);
HRESULT RIL_GetCellBroadcastMsgConfig(HRIL hRil);
HRESULT RIL_GetCellTowerInfo(HRIL hRil);
HRESULT RIL_GetClosedGroupSettings(HRIL hRil);
HRESULT RIL_GetCostInfo(HRIL hRil);
HRESULT RIL_GetCurrentAddressId(HRIL hRil);
HRESULT RIL_GetCurrentOperator(HRIL hRil, DWORD dwFormat);
HRESULT RIL_GetDataCompression(HRIL hRil);
HRESULT RIL_GetDevCaps(HRIL hRil, DWORD dwCapsType);
HRESULT RIL_GetDialedIdSettings(HRIL hRil);
HRESULT RIL_GetDriverVersion(HRIL hRil);
HRESULT RIL_GetEquipmentInfo(HRIL hRil);
HRESULT RIL_GetEquipmentState(HRIL hRil);
HRESULT RIL_GetErrorCorrection(HRIL hRil);
HRESULT RIL_GetGPRSAddress(HRIL hRil, DWORD dwContextID);
HRESULT RIL_GetGPRSAttached(HRIL hRil);
HRESULT RIL_GetGPRSClass(HRIL hRil);
HRESULT RIL_GetGPRSContextActivatedList(HRIL hRil);
HRESULT RIL_GetGPRSContextList(HRIL hRil);
HRESULT RIL_GetGPRSRegistrationStatus(HRIL hRil);
HRESULT RIL_GetHideConnectedIdSettings(HRIL hRil);
HRESULT RIL_GetHideIdSettings(HRIL hRil);
HRESULT RIL_GetHSCSDCallSettings(HRIL hRil);
HRESULT RIL_GetHSCSDOptions(HRIL hRil);
HRESULT RIL_GetLineStatus(HRIL hRil);
HRESULT RIL_GetLockingStatus(HRIL hRil, DWORD dwFacility, LPCSTR lpszPassword);
HRESULT RIL_GetMinimumQualityOfServiceList(HRIL hRil);
HRESULT RIL_GetMOSMSService(HRIL hRil);
HRESULT RIL_GetMsgConfig(HRIL hRil);
HRESULT RIL_GetMsgServiceOptions(HRIL hRil);
HRESULT RIL_GetOperatorList(HRIL hRil);
HRESULT RIL_GetPhonebookOptions(HRIL hRil);
HRESULT RIL_GetPhoneLockedState(HRIL hRil);
HRESULT RIL_GetPreferredOperatorList(HRIL hRil, DWORD dwFormat);
HRESULT RIL_GetRadioPresence(HRIL hRil, DWORD* dwRadioPresence);
HRESULT RIL_GetRegistrationStatus(HRIL hRil);
HRESULT RIL_GetRequestedQualityOfServiceList(HRIL hRil);
HRESULT RIL_GetRLPOptions(HRIL hRil);
HRESULT RIL_GetSerialPortHandle(HRIL hRil, HANDLE* lphSerial);
HRESULT RIL_GetSerialPortStatistics(HRIL hRil, const RILSERIALPORTSTATS* lpSerialPortStats);
HRESULT RIL_GetSignalQuality(HRIL hRil);
HRESULT RIL_GetSimRecordStatus(HRIL hRil, DWORD dwFileID);
HRESULT RIL_GetSimToolkitProfile(HRIL hRil);
HRESULT RIL_GetSubscriberNumbers(HRIL hRil);
HRESULT RIL_GetSystemTime(HRIL hRil);
HRESULT RIL_GetUserIdentity(HRIL hRil);
HRESULT RIL_GPRSAnswer(HRIL hRil);
HRESULT RIL_Hangup(HRIL hRil);

HRESULT RIL_Initialize(DWORD dwIndex, RILRESULTCALLBACK pfnResult,
					   RILNOTIFYCALLBACK pfnNotify, DWORD dwNotificationClasses,
					   DWORD dwParam, HRIL *lphRil);

HRESULT RIL_InitializeEmergency(DWORD dwIndex, RILRESULTCALLBACK pfnResult,
					   RILNOTIFYCALLBACK pfnNotify, DWORD dwNotificationClasses,
					   DWORD dwParam, HRIL *lphRil);
HRESULT RIL_ManageCalls(HRIL hRil, DWORD dwCommand, DWORD dwID);
HRESULT RIL_ReadMsg(HRIL hRil, DWORD dwIndex);
HRESULT RIL_ReadPhonebookEntries(HRIL hRil, DWORD dwStartIndex, DWORD dwEndIndex);
//HRESULT RIL_RegisterOnNetwork(HRIL hRil, DWORD dwMode, const RILOPERATORNAMES* lpOperatorNames);
HRESULT RIL_RemoveCallForwarding(HRIL hRil, DWORD dwReason, DWORD dwInfoClasses, DWORD dwAddressId);
HRESULT RIL_RemovePreferredOperator(HRIL hRil, DWORD dwIndex);
HRESULT RIL_RestoreMsgConfig(HRIL hRil, DWORD dwIndex);
HRESULT RIL_SaveMsgConfig(HRIL hRil);
HRESULT RIL_SendDTMF(HRIL hRil, LPCSTR lpszChars, DWORD dwDuration);

HRESULT RIL_SendMsg(HRIL hRil, const RILMESSAGE* lpMessage, DWORD dwOptions);

HRESULT RIL_SendMsgAcknowledgement(HRIL hRil, BOOL fSuccess);
//HRESULT RIL_SendRestrictedSimCmd(HRIL hRil, DWORD dwCommand, const RILSIMCMDPARAMETERS* lpParameters, const BYTE *lpbData, DWORD dwSize);
HRESULT RIL_SendSimCmd(HRIL hRil, const BYTE *lpbComamnd, DWORD dwSize);
HRESULT RIL_SendSimToolkitCmdResponse(HRIL hRil, const BYTE *lpbResponse, DWORD dwSize);
HRESULT RIL_SendSimToolkitEnvelopeCmd(HRIL hRil, const BYTE *lpbComamnd, DWORD dwSize);
HRESULT RIL_SendStoredMsg(HRIL hRil, DWORD dwIndex, DWORD dwOptions);
HRESULT RIL_SendSupServiceData(HRIL hRil, const BYTE *lpbData, DWORD dwSize);
//HRESULT RIL_SetAudioDevices(HRIL hRil, const RILAUDIODEVICEINFO* lpAudioDeviceInfo);
//HRESULT RIL_SetAudioGain(HRIL hRil, const RILGAININFO* lpGainInfo);
HRESULT RIL_SetAudioMuting(HRIL hRil, BOOL fEnable);
//HRESULT RIL_SetBearerServiceOptions(HRIL hRil, const RILBEARERSVCINFO* lpBearerServiceInfo);
HRESULT RIL_SetCallBarringStatus(HRIL hRil, DWORD dwType, DWORD dwInfoClass, LPCSTR lpszPassword, DWORD dwStatus);
HRESULT RIL_SetCallerIdStatus(HRIL hRil, DWORD dwStatus);
HRESULT RIL_SetCallForwardingStatus(HRIL hRil, DWORD dwReason, DWORD dwInfoClasses, DWORD dwStatus, DWORD dwAddressId);
HRESULT RIL_SetCallWaitingStatus(HRIL hRil, DWORD dwInfoClasses, DWORD dwStatus);

HRESULT RIL_SetCellBroadcastMsgConfig(HRIL hRil);
//HRESULT RIL_SetClosedGroupSettings(HRIL hRil, const RILCLOSEDGROUPSETTINGS* lpSettings);
//HRESULT RIL_SetCostInfo(HRIL hRil, const RILCOSTINFO* lpConstInfo, LPCSTR lpszPassword);
HRESULT RIL_SetCurrentAddressId(HRIL hRil, DWORD dwAddressId);
//HRESULT RIL_SetDataCompression(HRIL hRil, const RILDATACOMPINFO* lpDataCompInfo);
HRESULT RIL_SetDialedIdStatus(HRIL hRil, DWORD dwStatus);
HRESULT RIL_SetDTMFMonitoring(HRIL hRil, BOOL fEnable);
HRESULT RIL_SetEquipmentState(HRIL hRil, DWORD dwEquipmentState);
//HRESULT RIL_SetErrorCorrection(HRIL hRil, const RILERRORCORRECTIONINFO* lpErrorCorrectionInfo);
HRESULT RIL_SetGPRSAttached(HRIL hRil, BOOL fAttached);
HRESULT RIL_SetGPRSClass(HRIL hRil, DWORD dwClass);
//HRESULT RIL_SetGPRSContext(HRIL hRil, const RILGPRSCONTEXT* lpGprsContext);
HRESULT RIL_SetGPRSContextActivated(HRIL hRil, DWORD dwContextID, BOOL fContextActivation);
HRESULT RIL_SetHideConnectedIdStatus(HRIL hRil, DWORD dwStatus);
HRESULT RIL_SetHideIdStatus(HRIL hRil, DWORD dwStatus);
//HRESULT RIL_SetHSCSDOptions(HRIL hRil, const RILHSCSDINFO* lpHscsdInfo);
HRESULT RIL_SetLockingStatus(HRIL hRil, DWORD dwFacility, LPCSTR lpszPassword);
//HRESULT RIL_SetMinimumQualityOfService(HRIL hRil, const RILGPRSQOSPROFILE* lpGprsQosProfile);
HRESULT RIL_SetMOSMSService(HRIL hRil, DWORD dwMoSmsService);
//HRESULT RIL_SetMsgConfig(HRIL hRil, const RILMSGCONFIG* lpMsgConfigInfo);
//HRESULT RIL_SetMsgServiceOptions(HRIL hRil, const RILMSGSERVICEINFO* lpMsgServiceInfo);
//HRESULT RIL_SetPhonebookOptions(HRIL hRil, const RILPHONEBOOKINFO* lpPhonebookInfo);
//HRESULT RIL_SetRequestedQualityOfService(HRIL hRil, const RILGPRSQOSPROFILE* lpGprsQosProfile);
//HRESULT RIL_SetRLPOptions(HRIL hRil, const RILRLPINFO* lpRlpInfo);
//HRESULT RIL_SetSimToolkitProfile(HRIL hRil, const BYTE* lpbProfile, DWORD dwSize);
HRESULT RIL_TerminateSimToolkitSession(HRIL hRil, DWORD dwCause);
HRESULT RIL_TransferCall(HRIL hRil, const RILADDRESS* lpAddress, const RILSUBADDRESS* lpSubAddress);
HRESULT RIL_UnlockPhone(HRIL hRil, LPCSTR lpszPassword, LPCSTR lpszNewPassword);
HRESULT RIL_UnregisterFromNetwork(HRIL hRil);
HRESULT RIL_WriteMsg(HRIL hRil, const RILMESSAGE *lpMessage, DWORD dwStatus);
//HRESULT RIL_WritePhonebookEntry(HRIL hRil, const RILPHONEBOOKENTRY* lpEntry);



#ifdef __cplusplus
}
#endif

#endif // __RIL_H__
