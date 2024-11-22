#include "StdAfx.h"
#include "DlgSetVoicemail.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "CommonDefines.h"
#include "CommonGuiDefines.h"
#include "resource.h"
#include "IssCommon.h"


CDlgSetVoicemail::CDlgSetVoicemail()
:m_edtVMail(NULL)
{	
	m_hCaptionFont	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
	m_hBtnFont		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
}

CDlgSetVoicemail::~CDlgSetVoicemail(void)
{
	CIssGDIEx::DeleteFont(m_hCaptionFont);
	CIssGDIEx::DeleteFont(m_hBtnFont);

	Destroy();
}

BOOL CDlgSetVoicemail::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;

	SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= hWnd;
    mbi.nToolBarId	= IDR_MENU_Voicemail;
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

	InitControls();
	//CreateButtons();

    m_oStr->Empty(m_szVMail);

    SetWindowText(m_hWnd, _T("Voicemail"));

    // force the sip to come up
    SHSipPreference(m_hWnd, SIP_UP);

	return TRUE;
}

BOOL CDlgSetVoicemail::InitControls()
{
	// create list window
	m_edtVMail = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

    SendMessage(m_edtVMail, EM_LIMITTEXT, (WPARAM)20, 0L );   

	return TRUE;
}

BOOL CDlgSetVoicemail::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL ||
		m_gdiMem.GetWidth() != (rcClient.right - rcClient.left) || 
		m_gdiMem.GetHeight() != (rcClient.bottom - rcClient.top))
	{
		m_gdiMem.Destroy();
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);
	}

	FillRect(m_gdiMem.GetDC(), rcClient, 0x00ffffff);

	// draw the caption
    DrawText(m_gdiMem.GetDC(), _T("Set Voicemail number:"), 
		m_rcCaption, DT_SINGLELINE|DT_VCENTER|DT_LEFT, m_hCaptionFont, ::GetSysColor(COLOR_HIGHLIGHT));

	// Paint lines
	RECT rcLine = rcClient;
	rcLine.top = m_rcCaption.bottom;
	rcLine.bottom = rcLine.top + 1;
	FillRect(m_gdiMem.GetDC(), rcLine, 0);

	BitBlt(hDC,
		0,0,
		rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,
		m_gdiMem.GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgSetVoicemail::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Ok:
    case IDOK:
        GetWindowText(m_edtVMail, m_szVMail, STRING_MAX);
        SHSipPreference(m_hWnd, SIP_DOWN);
		EndDialog(hWnd, IDOK);        
	    break;
	case IDMENU_Cancel:
        SHSipPreference(m_hWnd, SIP_DOWN);
		EndDialog(hWnd, IDCANCEL);        
		break;
	}
	return TRUE;
}


BOOL CDlgSetVoicemail::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// set size the listbox and anything else
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	const int xIndent = GetSystemMetrics(SM_CXICON)/4;
	const int yIndent = xIndent;

	m_rcCaption = rcClient;
	m_rcCaption.bottom = m_rcCaption.top + GetSystemMetrics(SM_CYCAPTION);
	m_rcCaption.left += xIndent;
	m_rcCaption.right -= xIndent;

    MoveWindow(m_edtVMail, xIndent, m_rcCaption.bottom + yIndent,
                WIDTH(rcClient) - 2*xIndent, GetSystemMetrics(SM_CYVSCROLL), FALSE);



	return UNHANDLED;
}

