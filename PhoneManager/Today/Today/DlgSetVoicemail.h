#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"

class CDlgSetVoicemail: public CIssWnd
{
public:
	CDlgSetVoicemail();
	~CDlgSetVoicemail(void);
    TCHAR*      GetVmailNumber(){return m_szVMail;};

private:			// functions
	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);					
	BOOL 		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL		InitControls();

private:		// variables
    CIssGDIEx   m_gdiMem;
	HWND		m_edtVMail;
	HFONT		m_hCaptionFont;
	HFONT		m_hBtnFont;

	RECT		m_rcCaption;
    TCHAR       m_szVMail[STRING_MAX];

};
