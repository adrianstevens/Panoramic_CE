#include "WndCPU.h"

CWndCPU::CWndCPU(void)
{
}

CWndCPU::~CWndCPU(void)
{
}

BOOL CWndCPU::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("CPU Info"));
	Refresh();
	return TRUE;
}


void CWndCPU::Refresh()
{
	Clear();
	ShowGetSystemInfo();
}

void CWndCPU::ShowGetSystemInfo()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

    TCHAR szTemp[STRING_MAX];
    m_oStr->Format(szTemp, _T("%u"), (ULONG)si.dwNumberOfProcessors);
	OutputString(_T("# Of Processors:"), szTemp);

	switch(si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		OutputString(TEXT("Processor Type:"), _T("386"));
		break;
	case PROCESSOR_INTEL_486:
		OutputString(TEXT("Processor Type:"), _T("486"));
		break;
	case PROCESSOR_INTEL_PENTIUM:
		OutputString(TEXT("Processor Type:"), _T("Pentium"));
		break;
	case PROCESSOR_INTEL_PENTIUMII:
		OutputString(TEXT("Processor Type:"), _T("Pentium II"));
		break;
	case PROCESSOR_STRONGARM:
		OutputString(TEXT("Processor Type:"), _T("StrongARM"));
		break;
	case PROCESSOR_ARM720:
		OutputString(TEXT("Processor Type:"), _T("ARM 720"));
		break;
	case PROCESSOR_ARM820:
		OutputString(TEXT("Processor Type:"), _T("ARM 820"));
		break;
	case PROCESSOR_ARM920:
		OutputString(TEXT("Processor Type:"), _T("ARM 920"));
		break;
	case PROCESSOR_ARM_7TDMI:
		OutputString(TEXT("Processor Type:"), _T("ARM 7TDMI"));
		break;
	default:
        OutputFormattedString(TEXT("Processor Type:"), _T("Unknown:%i"), si.dwProcessorType);
		break;
	}

	ShowCPUInfo(si.wProcessorArchitecture, si.wProcessorLevel, si.wProcessorRevision);

//    seems kind of useless to me
//    m_oStr->Format(szTemp, _T("%u"), (ULONG)si.dwActiveProcessorMask);
//    OutputString(_T("Active Processor Mask:"), szTemp);

    m_oStr->Format(szTemp, _T("%u"), (ULONG)si.dwOemId);
    OutputString(_T("OEM ID:"), szTemp);

    m_oStr->Format(szTemp, _T("%u bytes"), (ULONG)si.dwPageSize);
    OutputString(_T("Page Size:"), szTemp);

    m_oStr->Format(szTemp, _T("%u"), si.lpMinimumApplicationAddress);
    OutputString(_T("Min App. Address:"), szTemp);

    m_oStr->Format(szTemp, _T("%u"), si.lpMaximumApplicationAddress);
	OutputString(_T("Max App Address:"), szTemp);

}

void CWndCPU::ShowCPUInfo(WORD wProcessorArchitecture, 
						  WORD wProcessorLevel,
						  WORD wProcessorRevision)
{
	const TCHAR *lptszCPUArch;
	const TCHAR *lptszCPULevel;
	const TCHAR *lptszCPURev;
	const TCHAR *lptszUnknown;

	lptszUnknown = _T("Unknown");
	lptszCPULevel = lptszUnknown;
	lptszCPURev = lptszUnknown;

	switch(wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_ARM:
        if(wProcessorLevel == 4)
            lptszCPUArch = TEXT("ARMV4");
        else
		    lptszCPUArch = TEXT("ARM"); 
		break;

	case PROCESSOR_ARCHITECTURE_INTEL: 
		lptszCPUArch = TEXT("INTEL"); 

		switch(wProcessorLevel)
		{
		case 4:
			lptszCPULevel = TEXT("80486");
			break;
		case 5:
			lptszCPULevel = TEXT("Pentium");
			break;
		case 6:
			lptszCPULevel = TEXT("Pentium Pro");
			break;
		}
		break;

	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		lptszCPUArch = lptszUnknown;
		break;
	}

    TCHAR szTemp[STRING_MAX];

    m_oStr->Format(szTemp, _T("%s"), lptszCPUArch);   
    OutputString(_T("Processor Architecture:"), szTemp);

    //really more of a desktop thing
 /*   if(lptszCPULevel == NULL)
        m_oStr->Format(szTemp, _T("%i"), wProcessorLevel);   
    else
        m_oStr->Format(szTemp, _T("%s"), lptszCPULevel);
    OutputString(_T("Processor Level:"), szTemp);*/

    m_oStr->Format(szTemp, _T("%i"), wProcessorRevision); 
    OutputString(_T("Processor Revision:"), szTemp);


}

float CWndCPU::ProcSpeedCalc()
{
    /*
    RdTSC:
    It's the Pentium instruction "ReaD Time Stamp Counter". It measures the
    number of clock cycles that have passed since the processor was reset, as a
    64-bit number. That's what the <CODE>_emit lines do.*/
#define RdTSC __asm _emit 0x0f __asm _emit 0x31

    // variables for the clock-cycles:
    __int64 cyclesStart = 0, cyclesStop = 0;
    // variables for the High-Res Performance Counter:
    unsigned __int64 nCtr = 0, nFreq = 0, nCtrStop = 0;


    // retrieve performance-counter frequency per second:
    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &nFreq)) return 0;

    // retrieve the current value of the performance counter:
    QueryPerformanceCounter((LARGE_INTEGER *) &nCtrStop);

    // add the frequency to the counter-value:
    nCtrStop += nFreq;


  /*  _asm
    {// retrieve the clock-cycles for the start value:
        RdTSC
            mov DWORD PTR cyclesStart, eax
            mov DWORD PTR [cyclesStart + 4], edx
    }

    do{
        // retrieve the value of the performance counter
        // until 1 sec has gone by:
        QueryPerformanceCounter((LARGE_INTEGER *) &nCtr);
    }while (nCtr < nCtrStop);

    _asm
    {// retrieve again the clock-cycles after 1 sec. has gone by:
        RdTSC
            mov DWORD PTR cyclesStop, eax
            mov DWORD PTR [cyclesStop + 4], edx
    }

    // stop-start is speed in Hz divided by 1,000,000 is speed in MHz
    return    ((float)cyclesStop-(float)cyclesStart) / 1000000;*/

    return 0;
}
