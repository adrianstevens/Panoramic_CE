#include "WndDisplay.h"
#include "gx.h"

CWndDisplay::CWndDisplay(void)
{
}

CWndDisplay::~CWndDisplay(void)
{
}

BOOL CWndDisplay::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Display"));
	Refresh();
	return TRUE;
}


void CWndDisplay::Refresh()
{
	Clear();
	ShowDisplayInfo();
    ShowRawFrameBufferInfo();
}

void CWndDisplay::ShowDisplayInfo()
{
	TCHAR szTemp[STRING_MAX*2];
    m_oStr->Empty(szTemp);

	OutputSection(_T("Primary Display"));
    m_oStr->Format(szTemp, _T("%i"),GetSystemMetrics(SM_CXSCREEN));
    OutputString(_T("Screen Width:"), szTemp);

    m_oStr->Format(szTemp, _T("%i"),GetSystemMetrics(SM_CYSCREEN));
    OutputString(_T("Screen Height:"), szTemp);

    //DPI

    int iIcon = GetSystemMetrics(SM_CXICON);

    int iDPI = 0;
    
    //http://www.microsoft.com/downloads/details.aspx?FamilyID=06111A3A-A651-4745-88EF-3D48091A390B&displaylang=en
    switch(iIcon)
    {
    case 32:
        iDPI = 96;
        break;
    case 43:
    case 44:
    case 45:
#ifdef WIN32_PLATFORM_WFSP
        iDPI = 131;
#else
        iDPI = 128;
#endif
        break;
    case 64:
        iDPI = 192;
        break;
    default:
        iDPI = 3*iIcon;
        break;
    }

    m_oStr->IntToString(szTemp, iDPI);
    OutputString(_T("DPI:"), szTemp);
    
   // damn thing crashes on the WM5 emulator ... no idea why

    DISPLAY_DEVICE sDisplay;
	memset(&sDisplay, 0, sizeof(DISPLAY_DEVICE));
    sDisplay.cb = sizeof(DISPLAY_DEVICE);
	
	if(EnumDisplayDevices(NULL, 0, &sDisplay, 0)== FALSE)
		return;//failed

	//otherwise lets display some info
	OutputString(_T("Name"), sDisplay.DeviceName);

    if(m_oStr->GetLength(sDisplay.DeviceString) > 0)
    {
	    m_oStr->Empty(szTemp);
	    m_oStr->Concatenate(szTemp, sDisplay.DeviceString);
	    OutputString(_T("Additional Info: "), szTemp);
    }

	if(sDisplay.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE)
		OutputString(_T("This device is VGA compatible\r\n"), _T(""));
}

void CWndDisplay::ShowRawFrameBufferInfo()
{
	RawFrameBufferInfo rfbi;
	HDC hdc;

	hdc= GetDC(m_hWnd);
	
	if(hdc)
	{
		if(ExtEscape(hdc, GETRAWFRAMEBUFFER, 0, 0, 
			sizeof(RawFrameBufferInfo), (char *) &rfbi))
		{
			TCHAR szTemp[STRING_MAX];
			TCHAR szBitsPer[STRING_NORMAL];
			m_oStr->IntToString(szBitsPer, rfbi.wBPP);

            m_oStr->Format(szTemp, _T("%s"), szBitsPer);
			OutputString(_T("Bits per pixel:"), szTemp);
            
            switch(rfbi.wFormat)
			{
			case FORMAT_565:
                OutputString(_T("Pixel Format:"), _T("RGB 565"));
				break;
			case FORMAT_555:
				OutputString(_T("Pixel Format:"), _T("RGB 555"));
				break;
			default:
                if(rfbi.wBPP == 24)
                    OutputString(_T("Pixel Format:"), _T("RGB 888"));
                else if(rfbi.wBPP == 32)
                    OutputString(_T("Pixel Format:"), _T("RGBA 8888"));
                else
    				OutputString(_T("Non-standard Pixel Format"), _T(""));
			    break;
			}
		}
		ReleaseDC(m_hWnd, hdc);
	}

}


/*

bool ZGfx::GfxInitRawFrameBufferAccess()
{
RawFrameBufferInfo rfbi;
HDC hdc;
bool retval;
retval=false;
hdc=GetDC(m_hwnd);
if(hdc)
{
if(ExtEscape(hdc, GETRAWFRAMEBUFFER, 0, 0, 
sizeof(RawFrameBufferInfo), (char *) &rfbi))
{
if(rfbi.wFormat==FORMAT_565)
{
m_framebufwidth=rfbi.cxPixels; //store width
m_framebufheight=rfbi.cyPixels; //store height

m_xpitch=rfbi.cxStride; //store xpitch

m_ypitch=rfbi.cyStride; //store ypitch

m_cbpp=rfbi.wBPP; //store bits per pixel value

m_framebuf=rfbi.pFramePointer; //store pointer

retval=true;
}
}
ReleaseDC(m_hwnd,hdc);
}
return retval;
}

*/