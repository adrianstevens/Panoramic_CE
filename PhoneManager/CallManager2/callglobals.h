#pragma once


struct CALLIDINFO
{
    TCHAR    szNumber[STRING_MAX];
    TCHAR    szName[STRING_MAX];
    long	 lOid;
};

enum EnumPostCall
{
    POSTCALL_SMS,  
    POSTCALL_AddToContacts,
    POSTCALL_CallBack,
    POSTCALL_Count,
};

enum EnumSMSReply
{
    SMSR_BusyCall,
    SMSR_BusyYouCall,
    SMSR_Meeting,
    SMSR_Car,
    SMSR_Custom,
    SMSR_Count,
};

enum EnumCallType //we can go further using the privacy/block enum
{
    CTYPE_IncomingAnswered,
    CTYPE_IncomingMissed,
    CTYPE_IncomingIngored,
    CTYPE_OutgoingAnswered,
    CTYPE_OutgoingDropped,
    CTYPE_NoCall,
};

enum EnumAllowCall
{
    ACALL_Allow,
    ACALL_Block,
    ACALL_Private, //ie ... busy
    ACALL_Unknown,
};

enum EnumCallState
{
    CSTATE_Disconnected,
    CSTATE_Ringing,
    CSTATE_Answering,
    CSTATE_Connected,
    CSTATE_Dialing,
    CSTATE_OnHold,
    CSTATE_Unknown,
};