#include "IssWndTrans.h"
#include "IssGDI.h"

HRESULT IssUpdateControls(HWND hWnd, HDC hDCBackground, RECT* rcArea)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	static CIssGDI gdiScreen;

	// GDI screen only needs to be created once, we do this for speed
	if(gdiScreen.GetHeight() != GetSystemMetrics(SM_CYSCREEN) || 
		gdiScreen.GetWidth() != GetSystemMetrics(SM_CXSCREEN))
		gdiScreen.Create(hDCBackground, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), FALSE, TRUE);

	// NOTE: hDCBackground should be the full size of the hWnd provided, if not we will get drawing errors
	BitBlt(gdiScreen.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		hDCBackground,
		0,0,
		SRCCOPY);

	POINT ptScreen;
	RECT rcWindow;
	NMPAINTBACKGROUND sBackground;
	ZeroMemory(&sBackground, sizeof(NMPAINTBACKGROUND));

	HWND hWndChild	= GetWindow(hWnd, GW_CHILD);
	hWndChild		= GetWindow(hWndChild, GW_HWNDLAST);	// we have to go in reverse Z-order

	// loop through and tell all the controls to repaint themselves
	while(hWndChild)
	{
		// need coordinates relative to the client HWND
		GetWindowRect(hWndChild, &rcWindow);
		ptScreen.x	= rcWindow.left;
		ptScreen.y	= rcWindow.top;
		ScreenToClient(hWnd, &ptScreen);

		sBackground.hdr.hwndFrom	= hWnd;
		sBackground.hdr.code		= NM_PAINTBACKGROUND;
		sBackground.hDC				= gdiScreen.GetDC();
		sBackground.rcDraw			= rcWindow;

		// if there is an area defined then lets make sure to check if we're in that area.
		// This will optimize drawing time.
		// NOTE: Everything is in screen coordinates
		if(rcArea)
		{
			// check if we're outside the area
			if(rcWindow.bottom <= rcArea->top || rcWindow.top >= rcArea->bottom || rcWindow.right <= rcArea->left || rcWindow.left >= rcArea->right)
			{
				hWndChild = GetWindow(hWndChild, GW_HWNDPREV);
				continue;
			}
			else
			{
				sBackground.rcDraw.left		= max(rcWindow.left, rcArea->left);
				sBackground.rcDraw.top		= max(rcWindow.top, rcArea->top);
				sBackground.rcDraw.right	= min(rcWindow.right, rcArea->right);
				sBackground.rcDraw.bottom	= min(rcWindow.bottom, rcArea->bottom);
			}
		}

		SendMessage(hWndChild, WM_NOTIFY, 0, (LPARAM)&sBackground);

		hWndChild = GetWindow(hWndChild, GW_HWNDPREV);
	}

	return S_OK;
}


CIssWndTrans::CIssWndTrans(void)
{
}

CIssWndTrans::~CIssWndTrans(void)
{
}

void CIssWndTrans::ForceUpdateBackground(HDC hDCBackground)
{
	RECT rcScreen;
	GetWindowRect(m_hWnd, &rcScreen);

	NMPAINTBACKGROUND sBackground;
	ZeroMemory(&sBackground, sizeof(NMPAINTBACKGROUND));
	sBackground.hdr.hwndFrom	= m_hWnd;
	sBackground.hdr.code		= NM_PAINTBACKGROUND;
	sBackground.hDC				= hDCBackground;
	sBackground.rcDraw			= rcScreen;
	BOOL bRet = (BOOL)::SendMessage(GetParent(m_hWnd), WM_NOTIFY, 0, (LPARAM)&sBackground);
}


