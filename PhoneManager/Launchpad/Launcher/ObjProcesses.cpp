#include "ObjProcesses.h"
#include <tlhelp32.h> 

//dirty but workable
BOOL		g_bThreadEnd;


CObjProcesses::CObjProcesses(void)
:m_oStr(CIssString::Instance())
,m_hThreadProcess(NULL)
,m_hWndNotif(NULL)
,m_uiMessage(0)
,m_dwLastUpdateProcess(0)
,m_bExcludeTaskman(TRUE)
,m_hIdleThread(NULL)
{
	
	memset(m_sSysHistory.chPowHistory, -1, HISTORY_MAX);
	m_sSysHistory.chPowHistory[0]	= 0;
	memset(m_sSysHistory.chMemHistory, -1, HISTORY_MAX);
	m_sSysHistory.chMemHistory[0]	= 0;

	InitializeCriticalSection(&m_cr);

	g_bThreadEnd = FALSE;
//	CreateThreads();
}

CObjProcesses::~CObjProcesses(void)
{
	g_bThreadEnd = TRUE;
	if(m_hIdleThread)
	{
		//...interesting....
		ResumeThread(m_hIdleThread);
		WaitForSingleObject(m_hIdleThread, 3000);
		CloseHandle(m_hIdleThread);
		m_hIdleThread = NULL;
	}

	if(m_hThreadProcess)
	{
		DWORD dwExitCode;
		if(GetExitCodeThread(m_hThreadProcess, &dwExitCode))
		{
			// if the thread is still going, forcefully kill it
			if(dwExitCode == STILL_ACTIVE)
				TerminateThread(m_hThreadProcess, 999);

			// memory cleanup
			CloseHandle(m_hThreadProcess);
		}
		m_hThreadProcess = NULL;
	}
	Destroy();

	DeleteCriticalSection(&m_cr);
}

BOOL CObjProcesses::Destroy()
{
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* lpInfo = m_arrProcess[i];
		if(lpInfo)
			delete lpInfo;
	}
	m_arrProcess.RemoveAll();
	return TRUE;
}

void CObjProcesses::Init(HWND hWndNotif, UINT uiMessage)
{
	m_hWndNotif		= hWndNotif;
	m_uiMessage		= uiMessage;
}

void CObjProcesses::Refresh(UINT uiRefresh)
{
	//RefreshProcess();

	if(uiRefresh & REFRESH_Process)
	{
		// process refreshes take a couple seconds so do it in a thread
		if(m_hThreadProcess)
		{
			DWORD dwExitCode;
			if(GetExitCodeThread(m_hThreadProcess, &dwExitCode))
			{
				// if the thread is still going, then let it finish off what it was doing
				if(dwExitCode == STILL_ACTIVE)
					return;
			}

			// memory cleanup
			CloseHandle(m_hThreadProcess);
			m_hThreadProcess = NULL;
		}
		m_hThreadProcess = CreateThread(NULL, 0, &ThreadProcess, (LPVOID)this, CREATE_SUSPENDED, NULL);

		// is the thread going?
		if(m_hThreadProcess)
		{
			// make sure it runs in the background
			SetThreadPriority(m_hThreadProcess, THREAD_PRIORITY_BELOW_NORMAL);
			ResumeThread(m_hThreadProcess);
		}

	}

	if(uiRefresh & REFRESH_CPU)
		RefreshCPU();

}

void CObjProcesses::RefreshProcess()
{
	if(!IsWindowVisible(m_hWndNotif))
		return;

	// start over
	Destroy();

	HINSTANCE       hKernel        = NULL; 
	HINSTANCE       hProcessSnap   = NULL; 
	PROCESSENTRY32	pe32  			= {0}; 


	// Obtain a module handle to toolhelp.dll 
//	hKernel = LoadLibrary(_T("toolhelp.dll")); 
//	if (!hKernel) 
//		return;
//http://nativemobile.blogspot.com/2006_01_01_nativemobile_archive.html
#define TH32CS_SNAPNOHEAPS 0x40000000

	// all processes currently in the system. 
//	hProcessSnap = (HINSTANCE)CreateToolhelp32Snapshot(TH32CS_SNAPNOHEAPS, 0); 
	hProcessSnap = (HINSTANCE)CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		//we're going to try a few variations to try and force it through
	//	hKernel = LoadLibrary(_T("toolhelp.dll")); 
	//	if (!hKernel) 
	//		return;

		if(hProcessSnap == INVALID_HANDLE_VALUE)
		{	//couldn't hurt to try...
			hProcessSnap = (HINSTANCE)CreateToolhelp32Snapshot(TH32CS_SNAPNOHEAPS, 0); 
		} 
		if(hProcessSnap == INVALID_HANDLE_VALUE)
		{
			if(hKernel)
				FreeLibrary(hKernel);
			return;
		}
	}

	// walk through all the processes
	pe32.dwSize = sizeof(PROCESSENTRY32);   // must be filled out before use 
	if (Process32First(hProcessSnap, &pe32)) //BUGBUG ... fails here on the G900 if we load the DLL .. using SNAPNOHEAPS didn't help
	{ 
		do 
		{
//#ifndef DEBUG
			// on release build we really don't want to see this task manager info
			if(GetCurrentProcessId() == pe32.th32ProcessID)
				continue;
//#endif
			// create our new structure
			TypeProcessInfo* sInfo = new TypeProcessInfo;
			ZeroMemory(sInfo, sizeof(TypeProcessInfo));

			LPTSTR pCurChar; 
			// strip path and leave executabe filename 
			if (_tcsstr(pe32.szExeFile, _T("\\"))) 
				pCurChar = _tcsrchr(pe32.szExeFile, _T('\\')); 
			else
				pCurChar = pe32.szExeFile;

			m_oStr->StringCopy(sInfo->szProcessName, pCurChar); 

			sInfo->bflags		= 0; 
			sInfo->dwProcessId	= pe32.th32ProcessID; 
			sInfo->dwCntThreads	= pe32.cntThreads;
			sInfo->dwCntUsage	= pe32.cntUsage;
			sInfo->dwInheritedFromProcessId = pe32.th32ParentProcessID;
			sInfo->dwth32ModuleID = pe32.th32ModuleID;

			// add it to our Vector
			//m_arrProcess.AddSortedElement(sInfo, CompareProcess);
			m_arrProcess.AddElement(sInfo);

		}while (Process32Next(hProcessSnap, &pe32)); 
	}
	CloseToolhelp32Snapshot(hProcessSnap);

	// get the block sizes for the processes
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* sInfo = m_arrProcess[i];
		if(sInfo)
			sInfo->dwBlocksUsed = GetBlockSize(sInfo->dwProcessId);
	}

	// enumerate all windows 
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)this); 

	// cleanup
	if(hKernel)
		FreeLibrary(hKernel);
}

void CObjProcesses::RefreshWindowsOnly()
{
	HCURSOR hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)); 

	m_bExcludeTaskman = FALSE;

	// start over
	Destroy();
	// enumerate all windows 
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)this); 

	SetCursor(hCursor);
}

int	CObjProcesses::CompareProcess(const void* lp1, const void* lp2)
{
	TypeProcessInfo* sInfo1 = (TypeProcessInfo*)lp1;
	TypeProcessInfo* sInfo2 = (TypeProcessInfo*)lp2;

	if(!sInfo1 || !sInfo2)
		return -1;

	if(sInfo1->dwProcessId < sInfo2->dwProcessId)
		return -1;
	else if(sInfo1->dwProcessId == sInfo2->dwProcessId)
		return 0;
	else
		return 1;
}

DWORD CObjProcesses::GetBlockSize(DWORD dwProcessID)
{
	if( dwProcessID == 0 )
		return 0;

	DWORD dwUsedHeap = 0;
	// -- heap
	HANDLE hSS;
	hSS = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST|0x40000000, dwProcessID);
	if( hSS == INVALID_HANDLE_VALUE )
		return 0;

	HEAPLIST32 hl32;
	memset(&hl32, 0, sizeof(HEAPLIST32));
	hl32.dwSize = sizeof(HEAPLIST32);

	if( !Heap32ListFirst(hSS, &hl32) )
	{
		CloseToolhelp32Snapshot(hSS);
		if( GetLastError() == ERROR_NO_MORE_FILES )
			return 0;
		return 0;
	}

	DWORD dwTotalHeapByte = 0;
	do
	{
		HEAPENTRY32 he32;
		memset(&he32, 0, sizeof(HEAPENTRY32));
		he32.dwSize = sizeof(HEAPENTRY32);

		if( !Heap32First( hSS, &he32, hl32.th32ProcessID, hl32.th32HeapID ) )
		{
			CloseToolhelp32Snapshot(hSS);
			return 0;
		}

		do
		{
			if( he32.dwFlags != LF32_FREE )
				dwTotalHeapByte += he32.dwBlockSize;
		}while( Heap32Next( hSS, &he32 ) );

		if( GetLastError() != ERROR_NO_MORE_FILES )
		{
			CloseToolhelp32Snapshot(hSS);
			return 0;
		}

		dwUsedHeap += dwTotalHeapByte;

	}while( Heap32ListNext( hSS, &hl32 ) );

	if( GetLastError() != ERROR_NO_MORE_FILES )
		dwUsedHeap = 0;

	CloseToolhelp32Snapshot(hSS);

	return dwUsedHeap;
}


/*DWORD CObjProcesses::GetBlockSize(DWORD dwProcessID)
{
	HANDLE hHeapSnapshot;
	HEAPLIST32 HeapList = {0};
	HEAPENTRY32 HeapEntry = {0};
	DWORD dwTotalBlocksUsed = 0;

	hHeapSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST|0x40000000, dwProcessID);
	if (INVALID_HANDLE_VALUE != hHeapSnapshot)
	{
		memset(&HeapList, 0, sizeof(HEAPLIST32));
		HeapList.dwSize = sizeof(HEAPLIST32);

		if (Heap32ListFirst(hHeapSnapshot, &HeapList))
		{        
			do        
			{   
				memset(&HeapEntry, 0, sizeof(HEAPENTRY32));
				HeapEntry.dwSize = sizeof(HEAPENTRY32);    

				if (Heap32First(hHeapSnapshot, &HeapEntry, HeapList.th32ProcessID, HeapList.th32HeapID) != FALSE)            
				{
					do                
					{   
						// save HeapEntry.dwAddress and 
						// HeapEntry.dwBlockSize for use later
						if(HeapEntry.dwFlags != LF32_FREE)
							dwTotalBlocksUsed += HeapEntry.dwBlockSize;
	
					} while (Heap32Next(hHeapSnapshot, &HeapEntry) != FALSE);            
				}            
				else                
					break;

			} while (Heap32ListNext(hHeapSnapshot, &HeapList) != FALSE);    
		}        

		CloseToolhelp32Snapshot(hHeapSnapshot);
	}
	return dwTotalBlocksUsed;
}*/

// check for any special windows
BOOL CObjProcesses::IsSpecialWindow(HWND hWnd)
{
	TCHAR szClass[STRING_LARGE] = _T("");

	GetClassName(hWnd, szClass, STRING_LARGE);
	if(m_oStr->CompareNoCase(szClass, _T("DesktopExplorerWindow")) == 0 ||
	   m_oStr->CompareNoCase(szClass, _T("Explore")) == 0)
	{
#ifdef DEBUG
		/*TCHAR szName[STRING_LARGE];
		GetWindowText(hWnd, szName, STRING_LARGE);
		int i=0;*/
#endif
		return TRUE;
	}

	return FALSE;
}

BOOL CObjProcesses::IsSpecialWindow(TCHAR* szWindowName)
{
	if(m_oStr->CompareNoCase(szWindowName, _T("CursorWindow"))==0 || 
	   m_oStr->CompareNoCase(szWindowName, _T("Phone "))==0 ||
	   m_oStr->CompareNoCase(szWindowName, _T("Tray"))==0 || 
	   m_oStr->CompareNoCase(szWindowName, _T("Launchpad")) == 0)
	   return TRUE;
	else if(m_oStr->CompareNoCase(szWindowName, _T("Task Manager"))==0 && m_bExcludeTaskman)
		return TRUE;
	return FALSE;
}

BOOL CObjProcesses::EnumWindowsProc(HWND hWnd, DWORD lParam)
{ 
	DWORD			pid;
	TCHAR           buf[SIZE_Title]; 
	CObjProcesses*	pThis = (CObjProcesses*)lParam;

	// get the processid for this window 
	if (!GetWindowThreadProcessId(hWnd, &pid))  
		return TRUE; 

	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);



	if((dwStyle & WS_VISIBLE) == 0 || (dwStyle & WS_OVERLAPPED) == 0)
		//Not a visible window. Don't include it.
		return TRUE;

#ifndef SOLEUS
	if((dwStyle & WS_CHILD) || (dwStyle & WS_POPUP))
		//Don't show these type of windows. 
		return TRUE;
#endif

	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if(dwExStyle & WS_EX_TOOLWINDOW)
		return TRUE;

	// the window must be a parent window
	HWND hWndParent = GetParent(hWnd);
	if(hWndParent)
		return TRUE;

	// we don't want any special windows that might screw things up
	if(pThis->IsSpecialWindow(hWnd))
		return TRUE;

	BOOL bFound = FALSE;

	// look for the task in the task list for this window 
	for (int i=0; i<pThis->m_arrProcess.GetSize(); i++) 
	{ 
		TypeProcessInfo* sInfo = pThis->m_arrProcess[i];
		if (sInfo && sInfo->dwProcessId == pid) 
		{
			bFound = TRUE;

			// as long as the window is visible and it's not this one
			if (IsWindowVisible(hWnd) && GetFocus() != hWnd) 
			{
				sInfo->hWnd	= hWnd;
				int nCnt = GetWindowText(hWnd, buf, SIZE_Title);
				buf[nCnt] = _T('\0');
				if (nCnt) 
					pThis->m_oStr->StringCopy(sInfo->szWindowTitle, buf); 

				// quick check to not show the cursor window or this window
				if(pThis->IsSpecialWindow(buf))
				{
					pThis->m_oStr->Empty(sInfo->szWindowTitle);
					sInfo->hWnd = NULL;
				}

				// let's get the full path of the process
				HANDLE hProcess = OpenProcess(0, FALSE, pid);
				if(hProcess)
					GetModuleFileName((HMODULE)hProcess, sInfo->szFullName, MAX_PATH);
				CloseHandle(hProcess);
			}
			break; 
		} 
	} 

	// if we haven't found it in the list
	if(!bFound && IsWindowVisible(hWnd) && GetFocus() != hWnd)
	{
		int nCnt = GetWindowText(hWnd, buf, SIZE_Title);
		buf[nCnt] = _T('\0');
		
		// quick check to not show the cursor window or this window
		if(pThis->IsSpecialWindow(buf))
			return TRUE;

		// Add a new one
		TypeProcessInfo* sInfo = new TypeProcessInfo;
		ZeroMemory(sInfo, sizeof(TypeProcessInfo));

		sInfo->hWnd	= hWnd;
		if (nCnt) 
			pThis->m_oStr->StringCopy(sInfo->szWindowTitle, buf); 

		// let's get the full path of the process
		HANDLE hProcess = OpenProcess(0, FALSE, pid);
		if(hProcess)
			GetModuleFileName((HMODULE)hProcess, sInfo->szFullName, MAX_PATH);
		CloseHandle(hProcess);

		sInfo->dwProcessId	= pid;
		sInfo->dwBlocksUsed = pThis->GetBlockSize(pid);

		int iIndex = pThis->m_oStr->FindLastOccurance(sInfo->szFullName, _T("\\"));
		if(iIndex == -1)
			pThis->m_oStr->StringCopy(sInfo->szProcessName, sInfo->szFullName);
		else
			pThis->m_oStr->StringCopy(sInfo->szProcessName, sInfo->szFullName, iIndex+1, pThis->m_oStr->GetLength(sInfo->szFullName) - iIndex-1);

		// add it to our list
		pThis->m_arrProcess.AddElement(sInfo);
	}

	// continue the enumeration 
	return TRUE; 
}

BOOL CObjProcesses::ActiveProcess(TypeProcessInfo* sProcess)
{
	if(!sProcess)
		return FALSE;

	if (sProcess->hWnd && !m_oStr->IsEmpty(sProcess->szWindowTitle)) 
	{
		//Move this window to the foreground.
		if(m_hWndNotif)
			PostMessage(m_hWndNotif, WM_COMMAND, (WPARAM)IDOK, 0);//get us out of the way
		SetForegroundWindow((HWND)(((DWORD)sProcess->hWnd) | 0x1));
		SetActiveWindow(sProcess->hWnd);
		SetFocus(sProcess->hWnd);
		//InvalidateRect((HWND)(((DWORD)sProcess->hWnd) | 0x1), NULL, FALSE);
		PostMessage((HWND)(((DWORD)sProcess->hWnd) | 0x1), WM_PAINT, 0, 0);//for the poorly behaved phone app
	}

	return TRUE;
}

BOOL CObjProcesses::KillAllTasks(BOOL bCallRefresh)
{
	HCURSOR hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE);

	EnterCriticalSection(&m_cr);

	BOOL bAllClosed = TRUE;
	BOOL bFoundSpecialWindow = FALSE;
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* sProcess = m_arrProcess[i];
		if(sProcess && sProcess->hWnd && !m_oStr->IsEmpty(sProcess->szWindowTitle))
		{
			if(IsSpecialWindow(sProcess->szWindowTitle) || IsSpecialWindow(sProcess->hWnd))
				bFoundSpecialWindow = TRUE;

			if(!KillProcess(sProcess, FALSE))
			{
				bAllClosed = FALSE;
				//break;
			}
		}
	}

	// if this is one of the weird apps like Excel, Outlook... they don't behave normal with the today screen
	// so we have to force a redraw of the today screen... not sure why
	if(bFoundSpecialWindow)
	{
		HWND hWnd = ::FindWindow(_T("DesktopExplorerWindow"), _T("Desktop"));
		if(hWnd)
			InvalidateRect(hWnd, NULL, FALSE);
	}

	// make a sleep here to give it time to fully close all down
	Sleep(500);

	// forcefull update
	if(bCallRefresh)
		RefreshProcess();

	// End the Wait cursor
	SetCursor(hCursor);
	LeaveCriticalSection(&m_cr);

	if(m_hWndNotif && m_uiMessage != 0)
		PostMessage(m_hWndNotif, m_uiMessage, 0,0);

	return bAllClosed;
}

BOOL CObjProcesses::KillProcess(TypeProcessInfo* sProcess, BOOL bRefreshList /*= TRUE*/)
{
	if(!sProcess)
		return FALSE;

	HANDLE hProcess; 
	BOOL bClosed = FALSE;

	HCURSOR hCursor; 
	
	if(bRefreshList)
	{
		hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)); 
		ShowCursor(TRUE);

		EnterCriticalSection(&m_cr);
	}

	// if there is no window then kill the process
	if (!sProcess->hWnd) 
	{ 
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, sProcess->dwProcessId); 
		if (hProcess) 
		{ 
			if (!TerminateProcess(hProcess, 1)) 
			{ 
				CloseHandle(hProcess); 

				// End the Wait cursor
				SetCursor(hCursor);
				LeaveCriticalSection(&m_cr);
				return FALSE; 
			} 

			bClosed = TRUE;
		} 
		else
		{
			// End the Wait cursor
			SetCursor(hCursor);
			LeaveCriticalSection(&m_cr);
			return FALSE;
		}
	} 
	else
	{
		// close the window
		PostMessage(sProcess->hWnd, WM_CLOSE, 0, 0); 
		PostMessage(sProcess->hWnd, WM_COMMAND, IDOK, 0);
	
		int iCount = 0;
		while(iCount < 10 && IsWindow(sProcess->hWnd))
		{
			iCount++;
			Sleep(300);
		}

		bClosed = (IsWindow(sProcess->hWnd)?FALSE:TRUE);
		
		/*if(!bClosed)
		{
			// now just double check that the process itself has closed
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, sProcess->dwProcessId); 
			if (hProcess) 
			{ 
				TerminateProcess(hProcess, 1);
				CloseHandle(hProcess);
			}
			bClosed = TRUE;
		}*/
	}

	if(bRefreshList)
	{
		// make a sleep here to give it time to fully close down
		Sleep(500);

		// forcefull update
		RefreshProcess();

		// End the Wait cursor
		SetCursor(hCursor);
		LeaveCriticalSection(&m_cr);

		if(m_hWndNotif && m_uiMessage != 0)
			PostMessage(m_hWndNotif, m_uiMessage, 0,0);
	}

	
	return bClosed;
}



//-----------------------------------------------------------------------------
// measure cpu power and ...
//-----------------------------------------------------------------------------
void CObjProcesses::RefreshCPU()
{
	static DWORD	dwLastThreadTime = 0;
	static DWORD	dwLastTickTime = 0;
	DWORD			dwCurrentThreadTime =0;
	DWORD			dwCurrentTickTime = 0;

	FILETIME		ftCreationTime;
	FILETIME		ftExitTime;
	FILETIME		ftKernelTime;
	FILETIME		ftUserTime;

	DWORD			dwCpuPower;

	MEMORYSTATUS	ms;

	ms.dwLength = sizeof(MEMORYSTATUS);

	// search status
	if(m_hIdleThread)
		SuspendThread(m_hIdleThread);

	dwCurrentTickTime = GetTickCount();
	GetThreadTimes(m_hIdleThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	dwCurrentThreadTime = GetThreadTick(&ftKernelTime, &ftUserTime);

	// calculate cpu power
	if( dwCurrentTickTime != dwLastTickTime || dwLastThreadTime != 0 || dwLastTickTime != 0)
		dwCpuPower = 100 - (((dwCurrentThreadTime - dwLastThreadTime) * 100) / (dwCurrentTickTime - dwLastTickTime));
	else
		dwCpuPower = 0;	// avoid divide by 0

	// memory history
	GlobalMemoryStatus(&ms);

	// Shift history
	memmove(m_sSysHistory.chPowHistory+1, m_sSysHistory.chPowHistory, HISTORY_MAX-1);
	memmove(m_sSysHistory.chMemHistory+1, m_sSysHistory.chMemHistory, HISTORY_MAX-1);

	// save history
	m_sSysHistory.chPowHistory[0] = (char)dwCpuPower;
	m_sSysHistory.chMemHistory[0] = (char)ms.dwMemoryLoad;

	// save status
	dwLastTickTime = GetTickCount();
	dwLastThreadTime = dwCurrentThreadTime;

	if(m_hIdleThread)
		ResumeThread(m_hIdleThread);
}

//-----------------------------------------------------------------------------
// helper
//-----------------------------------------------------------------------------
DWORD CObjProcesses::GetThreadTick(FILETIME* a, FILETIME* b)
{
	__int64 a64 = 0;
	__int64 b64 = 0;
	a64 = a->dwHighDateTime;
	a64 <<= 32;
	a64 += a->dwLowDateTime;

	b64 = b->dwHighDateTime;
	b64 <<= 32;
	a64 += b->dwLowDateTime;

	a64 += b64;

	// nano sec to milli sec
	a64 /= 10000;

	return (DWORD)a64;
}


//-----------------------------------------------------------------------------
// create thread(s)
//-----------------------------------------------------------------------------
BOOL CObjProcesses::CreateThreads()
{
	DWORD ThreadID;
	m_hIdleThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)thIdle, (LPVOID)&m_sSysHistory, CREATE_SUSPENDED, &ThreadID);

	if(m_hIdleThread == NULL )
	{
		return FALSE;
	}
	SetThreadPriority(m_hIdleThread, THREAD_PRIORITY_IDLE);
	ResumeThread(m_hIdleThread);
	return TRUE;
}

//-----------------------------------------------------------------------------
// dummy thread to measure cpu usage
//-----------------------------------------------------------------------------
void CObjProcesses::thIdle(LPVOID pvParams)
{	//yuck
	while(!g_bThreadEnd);
}

int CObjProcesses::GetBattPercentage()
{
	SYSTEM_POWER_STATUS_EX2 sStatus;
	
	if(!GetSystemPowerStatusEx2(&sStatus, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE))
		return 1;
	else
		return sStatus.BatteryLifePercent;
}

DWORD CObjProcesses::ThreadProcess(LPVOID lpVoid)
{
	if(!lpVoid)
		return 0;

	CObjProcesses* oThis = (CObjProcesses*)lpVoid;

	EnterCriticalSection(&oThis->m_cr);
	oThis->RefreshProcess();
	LeaveCriticalSection(&oThis->m_cr);

	// update the last time we update
	oThis->m_dwLastUpdateProcess = GetTickCount();

	// now that we have updated make sure to send a message to the parent
	if(oThis->m_hWndNotif && oThis->m_uiMessage != 0)
		PostMessage(oThis->m_hWndNotif, oThis->m_uiMessage, 0,0);

	return 0;
}

BOOL CObjProcesses::IsUpdatingProcess()
{
	if(m_hThreadProcess)
	{
		DWORD dwExitCode;
		if(GetExitCodeThread(m_hThreadProcess, &dwExitCode))
		{
			// if the thread is still going, then let it finish off what it was doing
			if(dwExitCode == STILL_ACTIVE)
				return TRUE;
		}

		// memory cleanup
		CloseHandle(m_hThreadProcess);
		m_hThreadProcess = NULL;
	}
	return FALSE;
}

void CObjProcesses::ResetProcessSynchorized()
{
	EnterCriticalSection(&m_cr);
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* sInfo = m_arrProcess[i];
		if(sInfo)
			sInfo->bSynchronized = FALSE;
	}
	LeaveCriticalSection(&m_cr);
}

TypeProcessInfo* CObjProcesses::FindProcess(TypeProcessInfo* sCompareProcess)
{
	EnterCriticalSection(&m_cr);
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* sInfo = m_arrProcess[i];
		if(sInfo && sInfo->dwProcessId == sCompareProcess->dwProcessId)
		{
			LeaveCriticalSection(&m_cr);
			return sInfo;
		}
	}
	LeaveCriticalSection(&m_cr);
	return NULL;
}

TypeProcessInfo* CObjProcesses::FindWindow(TypeProcessInfo* sCompareProcess)
{
	EnterCriticalSection(&m_cr);
	for(int i=0; i<m_arrProcess.GetSize(); i++)
	{
		TypeProcessInfo* sInfo = m_arrProcess[i];
		if(sInfo && sInfo->hWnd && sInfo->hWnd == sCompareProcess->hWnd)
		{
			LeaveCriticalSection(&m_cr);
			return sInfo;
		}
	}
	LeaveCriticalSection(&m_cr);
	return NULL;
}


BOOL CObjProcesses::SetProcessThreadPriority(int iPriority)
{
	if(m_hThreadProcess == NULL)
		return FALSE;

	return SetThreadPriority(m_hThreadProcess, iPriority);
}