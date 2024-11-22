#pragma once

#include "IssString.h"
#include "IssVector.h"
//#include "Preferences.h"

#define		SIZE_Title	64
#define		SIZE_Process MAX_PATH
#define		HISTORY_MAX	512
#define		UPDATE_TIME	1500

struct TypeSysHistory
{
	char chPowHistory[HISTORY_MAX];
	char chMemHistory[HISTORY_MAX];
};

#define		REFRESH_CPU		0x000000001
#define		REFRESH_Process	0x000000002
#define		REFRESH_Info	0x000000004
#define		REFRESH_All		REFRESH_CPU|REFRESH_Info|REFRESH_Process

struct TypeProcessInfo
{
	DWORD		dwCntUsage;     
	DWORD		dwProcessId;// same as th32ProcessID; 
	DWORD		dwInheritedFromProcessId;// same as th32ParentProcessID;     
	DWORD		dwCntThreads; 
	DWORD		dwth32ModuleID;     
	DWORD		dwth32DefaultHeapID;    
	DWORD		dwBlocksUsed;
	LONG		lpcPriClassBase;     
	BOOL        bflags; 
	BOOL		bSynchronized;					// this is used to find out if this process has been synchronized
	HWND        hWnd; 
	TCHAR		szFullName[MAX_PATH];
	TCHAR       szProcessName[SIZE_Process]; 
	TCHAR       szWindowTitle[SIZE_Title]; 
};



class CObjProcesses
{
public:
	CObjProcesses(void);
	~CObjProcesses(void);

	BOOL				Destroy();
	BOOL				CreateThreads(); //call before init if you want it
	void				Init(HWND hWndNotif, UINT uiMessage);
	void				Refresh(UINT uiRefresh);
	void				RefreshWindowsOnly();
	BOOL				ActiveProcess(TypeProcessInfo* sProcess);
	BOOL				KillProcess(TypeProcessInfo* sProcess, BOOL bRefreshList = TRUE);
	BOOL				KillAllTasks(BOOL bCallRefresh = TRUE);
	BOOL				IsUpdatingProcess(); // are we in the thread and updating the process info
	DWORD				GetLastUpdateProcess(){return m_dwLastUpdateProcess;};
	int					GetProcessCount(){return m_arrProcess.GetSize();};
	TypeProcessInfo*	GetProcess(int iIndex){return m_arrProcess[iIndex];};
	TypeProcessInfo*	FindProcess(TypeProcessInfo* sCompareProcess);
	TypeProcessInfo*	FindWindow(TypeProcessInfo* sCompareProcess);
	void				ResetProcessSynchorized();
	int					GetBattPercentage();

	BOOL				SetProcessThreadPriority(int iPriority);
	


private:	// functions
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd,	DWORD lParam);
	DWORD				GetBlockSize(DWORD dwProcessID);
	void				RefreshCPU();
	void				RefreshProcess();

	DWORD				GetThreadTick(FILETIME* a, FILETIME* b);
	static void			thIdle(LPVOID pvParams);
	static DWORD		ThreadProcess(LPVOID lpVoid);
	static int			CompareProcess(const void* lp1, const void* lp2);
	BOOL				IsSpecialWindow(HWND hWnd);
	BOOL				IsSpecialWindow(TCHAR* szWindowName);	
	
public:
	TypeSysHistory	m_sSysHistory;


private:	// variables
	CRITICAL_SECTION			m_cr;
	CIssString*					m_oStr;
	CIssVector<TypeProcessInfo>	m_arrProcess;
	HANDLE						m_hIdleThread;
	HANDLE						m_hThreadProcess;
	HWND						m_hWndNotif;
	UINT						m_uiMessage;
	DWORD						m_dwLastUpdateProcess;
	BOOL						m_bExcludeTaskman;
};
