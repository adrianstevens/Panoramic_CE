#include "CwndCPU.h"

CwndCPU::CwndCPU(void)
{
}

CwndCPU::~CwndCPU(void)
{
}

BOOL CwndCPU::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
	Refresh();
	return TRUE;
}


void CwndCPU::Refresh()
{
	Clear();
	ShowGetSystemInfo();
}

void CwndCPU::ShowGetSystemInfo()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	OutputFormattedString(TEXT("# Of Processors: %1!lu!\r\n"), (ULONG)si.dwNumberOfProcessors);

	switch(si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		OutputString(TEXT("Processor Type: 386 \r\n"));
		break;
	case PROCESSOR_INTEL_486:
		OutputString(TEXT("Processor Type: 486 \r\n"));
		break;
	case PROCESSOR_INTEL_PENTIUM:
		OutputString(TEXT("Processor Type: Pentium \r\n"));
		break;
	case PROCESSOR_INTEL_PENTIUMII:
		OutputString(TEXT("Processor Type: Pentium II\r\n"));
		break;
	case PROCESSOR_STRONGARM:
		OutputString(TEXT("Processor Type: StrongARM\r\n"));
		break;
	case PROCESSOR_ARM720:
		OutputString(TEXT("Processor Type: ARM 720\r\n"));
		break;
	case PROCESSOR_ARM820:
		OutputString(TEXT("Processor Type: ARM 820\r\n"));
		break;
	case PROCESSOR_ARM920:
		OutputString(TEXT("Processor Type: ARM 920\r\n"));
		break;
	case PROCESSOR_ARM_7TDMI:
		OutputString(TEXT("Processor Type: ARM 7TDMI\r\n"));
		break;
	default:
		OutputString(TEXT("Processor Type: Unknown\r\n"));
		break;
	}

	ShowCPUInfo(si.wProcessorArchitecture, si.wProcessorLevel, si.wProcessorRevision);

	OutputFormattedString(TEXT("Active Processor Mask: %1!lu!\r\n"), (ULONG)si.dwActiveProcessorMask);
	OutputFormattedString(TEXT("OEM ID: %1!lu!\r\n"), (ULONG)si.dwOemId);
	OutputFormattedString(TEXT("Page Size: %1!lu!\r\n"), (ULONG)si.dwPageSize);
	OutputFormattedString(TEXT("Min App Addr: 0x%1!p!\r\n"), si.lpMinimumApplicationAddress);
	OutputFormattedString(TEXT("Max App Addr: 0x%1!p!\r\n"), si.lpMaximumApplicationAddress);

}

void CwndCPU::ShowCPUInfo(WORD wProcessorArchitecture, 
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

	OutputString(TEXT("Processor Architecture: "));
	OutputString(lptszCPUArch);
	OutputString(_T("\r\n"));

	OutputString(TEXT("Processor Level: "));
	OutputString(lptszCPULevel);
	OutputString(_T("\r\n"));

	OutputString(TEXT("Processor Revision: "));
	OutputString(lptszCPURev);
	OutputString(_T("\r\n"));
}
