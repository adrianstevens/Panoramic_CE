#include "StdAfx.h"
#include "DlgContactCallLog.h"
#include "IssGDIFX.h"
#include "IssRect.h"
#include "ContactsGuiDefines.h"
#include "IssGDIDraw.h"

DlgContactCallLog::DlgContactCallLog(void)
: m_gdiFadeInBg(0)
//, m_Contact(0)
{
	m_hFontNormal	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
	m_hFontBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

DlgContactCallLog::~DlgContactCallLog(void)
{
	CIssGDIEx::DeleteFont(m_hFontNormal);
	CIssGDIEx::DeleteFont(m_hFontBold);
}

BOOL DlgContactCallLog::Init(IContact* pIContact, TypeContact* psContact, CIssGDIEx* gdiBg)
{
	m_gdiFadeInBg = gdiBg;
	m_oCallList.SetCurContact(pIContact);
	m_sContact = psContact;

	return TRUE;
}

BOOL DlgContactCallLog::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_oCallList.Init(hWnd, m_hInst, NULL, NULL,NULL,  NULL);
	m_bFadeIn = TRUE;
	PostMessage(m_hWnd, WM_FadeIn, 0,0);
	return TRUE;
}

void DlgContactCallLog::OnContextMenu(POINT pt)
{
	HMENU hMenuContext		= CreatePopupMenu();
	if(!hMenuContext)
		return;

	EnumShowCall eCallType = m_oCallList.GetShowCallType();

	AppendMenu(hMenuContext, MF_STRING|(eCallType==CALL_All?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_All,    _T("All"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Incoming?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Incoming,    _T("Incoming"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Missed?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Missed,    _T("Missed"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Outgoing?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Outgoing,    _T("Outgoing"));

	// test
	HWND hTest = GetWnd();
	//Display it.
	TrackPopupMenu(	hMenuContext, 
		TPM_LEFTALIGN|TPM_TOPALIGN, 
		pt.x,pt.y, 
		0, 
		GetWnd(), NULL);

	DestroyMenu(hMenuContext);
	return;
}

BOOL DlgContactCallLog::OnLButtonDown(HWND hWnd, POINT& pt)
{
	m_ptLButtonDown = pt;

	if(PtInRect(&m_rcHeader, pt))
	{
		SHRGINFO    shrg;
		shrg.cbSize		= sizeof(shrg);
		shrg.hwndClient = GetWnd();
		shrg.ptDown.x	= pt.x;
		shrg.ptDown.y	= pt.y;
		shrg.dwFlags	= SHRG_RETURNCMD/*|SHRG_LONGDELAY*/;
		if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU)
		{
			// put the drop down menu
			OnContextMenu(pt);
		}
	}
	else if(PtInRect(&m_rcListBox, pt))
	{
		m_oCallList.OnLButtonDown(pt);
	}
	//else if (PtInRect(&m_rcBackBtn, pt))
	//	EndDialog(m_hWnd, IDOK);

	return UNHANDLED;
}

BOOL DlgContactCallLog::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcListBox, pt) && PtInRect(&m_rcListBox, m_ptLButtonDown))	
		m_oCallList.OnLButtonUp(pt);
	else if(PtInRect(&m_rcBackBtn, pt) && PtInRect(&m_rcBackBtn, m_ptLButtonDown))
		EndDialog(m_hWnd, IDOK);

	return UNHANDLED;
}
BOOL DlgContactCallLog::OnMouseMove(HWND hWnd, POINT& pt)
{
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

	if(PtInRect(&m_rcListBox, pt))	
		m_oCallList.OnMouseMove(pt);

	return TRUE;
}

BOOL DlgContactCallLog::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_oCallList.OnTimer(wParam, lParam);

	return UNHANDLED;

}

BOOL DlgContactCallLog::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case (IDMENU_ChangeItem +  CALL_All):
	case (IDMENU_ChangeItem +  CALL_Incoming):
	case (IDMENU_ChangeItem +  CALL_Missed):
	case (IDMENU_ChangeItem +  CALL_Outgoing):
		{
			/*m_oCallList.SetShowCallType(( Call_Type)(wParam - IDMENU_ChangeItem));*/
			m_oCallList.ResetScrollPos();
			InvalidateRect(GetWnd(), NULL, FALSE);
		}
		break;
	}
	
	return TRUE;
}

BOOL DlgContactCallLog::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	// bg to fade in on...
	if(m_bFadeIn)
	{
		BitBlt(hDC,
			0,0,
			WIDTH(rcClient), HEIGHT(rcClient),
			m_gdiFadeInBg->GetDC(),
			0,0,			
			SRCCOPY);
	}

	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);

	DrawBackground(m_gdiMem.GetDC(), rcClient);
	DrawHeader(m_gdiMem.GetDC(), rcClient);
	DrawNavButtons(m_gdiMem.GetDC(), rcClient);
	m_oCallList.Draw(m_gdiMem.GetDC(), m_rcListBox);

	if(m_bFadeIn == TRUE)
		return TRUE;

	BitBlt(hDC,
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}
BOOL DlgContactCallLog::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_FadeIn)
	{
		m_bFadeIn = FALSE;
		HDC hDC = GetDC(m_hWnd);
		//CIssGDIEffects::FadeIn(hDC, m_gdiMem);
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		CIssGDIEx gdiTemp;
		gdiTemp.Create(m_gdiMem.GetDC(), rcClient, TRUE, TRUE, FALSE);
		SweepRight(hDC, gdiTemp);
		ReleaseDC(m_hWnd, hDC);
	}

	return UNHANDLED;
}


BOOL DlgContactCallLog::DrawBackground(HDC hdc, RECT rcClient)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		m_gdiBackground.Create(hdc, rcClient, FALSE);
		
		DrawGradientGDI(m_gdiBackground.GetDC(), rcClient, 0, 0x555555);
		
	}
	return BitBlt(hdc,
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiBackground.GetDC(),
		0,0,
		SRCCOPY);
}

BOOL DlgContactCallLog::DrawHeader(HDC hdc, RECT rcClient)
{
	// pic
	DrawContactPic(hdc, rcClient);


	// show: incoming
	// create 4 rects for text...
	RECT rcInfo = m_rcHeader;

	rcInfo.left		= m_rcPicture.right;
	rcInfo.bottom	= m_rcPicture.bottom;
	rcInfo.top		= m_rcPicture.top;

	RECT rcLineOne, rcLineTwoLeft, rcLineTwoRight;
	rcLineOne = rcLineTwoLeft = rcLineTwoRight = rcInfo;

	rcLineOne.bottom = rcLineTwoLeft.top = rcLineTwoRight.top = rcInfo.top + HEIGHT(rcInfo) / 2;
	rcLineOne.left = rcLineTwoLeft.left = rcLineOne.left + GetSystemMetrics(SM_CXSMICON);
	rcLineTwoLeft.right = rcLineTwoRight.left = rcLineOne.left + GetSystemMetrics(SM_CXSMICON)*3;
	
	//static BSTR szHeading1 = _T("Contact: ");
	static BSTR szHeading2 = _T("Show: ");
	TCHAR* szContactName = m_oCallList.GetCurContactName();
	TCHAR* szDisplayType = m_oCallList.GetCallTypeString();

	//CIssGDI::DrawText(hdc, szHeading1,		rcLineOneLeft,	DT_RIGHT | DT_BOTTOM | DT_END_ELLIPSIS, m_hFontText, 0xFFFFFF);
	DrawText(hdc, szContactName,	rcLineOne,		DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS,	m_hFontBold,	0xFFFFFF);
	DrawText(hdc, szHeading2,		rcLineTwoLeft,	DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS,	m_hFontNormal,	0xFFFFFF);
	DrawText(hdc, szDisplayType,	rcLineTwoRight, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS,	m_hFontNormal,	0xA0FFFF);


	return TRUE;
}

BOOL DlgContactCallLog::DrawContactPic(HDC hdc, RECT rcClient)
{
	if(!m_sContact->gdiPicture)
	{
		if(m_gdiPic.GetDC() == NULL)
		{
			m_gdiPic.Create(hdc, m_rcPicture, FALSE);
			RECT rcTemp = {0, 0, WIDTH(m_rcPicture), HEIGHT(m_rcPicture)};
			GradientAngleFillRect(m_gdiPic, rcTemp, 0xFFFFFF, 0);
			rcTemp.right--;
			rcTemp.bottom--;
			FrameRect(m_gdiPic.GetDC(), rcTemp, 0xFFFFFF, 1);
		}

		BitBlt(hdc,
			m_rcPicture.left,m_rcPicture.top,
			WIDTH(m_rcPicture), HEIGHT(m_rcPicture),
			m_gdiPic.GetDC(),
			0,0,
			SRCCOPY);
	}
	else
	{	
		BitBlt(hdc,
			m_rcPicture.left,m_rcPicture.top,
			WIDTH(m_rcPicture), HEIGHT(m_rcPicture),
			m_sContact->gdiPicture->GetDC(),
			(m_sContact->gdiPicture->GetWidth() - WIDTH(m_rcPicture))/2, (m_sContact->gdiPicture->GetHeight() - HEIGHT(m_rcPicture))/2,
			SRCCOPY);

		m_rcPicture.right --;
		m_rcPicture.bottom--;
		FrameRect(hdc, m_rcPicture, RGB(255,255,255));
		m_rcPicture.right ++;
		m_rcPicture.bottom++;
	}

	

	return TRUE;
}

BOOL DlgContactCallLog::DrawNavButtons(HDC hdc, RECT rcClient)
{
	if(m_gdiNavButton.GetDC() == 0)
	{
		RECT rcTemp = {0, 0, WIDTH(m_rcBackBtn), HEIGHT(m_rcBackBtn)};
		m_gdiNavButton.Create(hdc, m_rcBackBtn, FALSE);

		FillRect(m_gdiNavButton, rcTemp, TRANSPARENT_COLOR);
		ShinyRoundRectangle(m_gdiNavButton, rcTemp, BTN_ACTIVE_GRAD1, 
			GetSystemMetrics(SM_CXSMICON)/2, GetSystemMetrics(SM_CXSMICON)/2, 
			BTN_ACTIVE_GRAD1, BTN_ACTIVE_GRAD2, BTN_ACTIVE_GRAD3, BTN_ACTIVE_GRAD4);
	}

	
	TransparentImage(hdc,
		m_rcBackBtn.left,m_rcBackBtn.top,
		WIDTH(m_rcBackBtn), HEIGHT(m_rcBackBtn),
		m_gdiNavButton.GetDC(),
		0,0,
		WIDTH(m_rcBackBtn), HEIGHT(m_rcBackBtn),
		TRANSPARENT_COLOR);


	DrawText(hdc, _T("Done"), m_rcBackBtn, DT_CENTER | DT_VCENTER, m_hFontNormal, 0xFFFFFF);

	return TRUE;
}


BOOL DlgContactCallLog::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE)	
	{
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);
		RECT rc;
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

		return TRUE;
	}

	return UNHANDLED;
}

void DlgContactCallLog::DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB)
{
	unsigned int Shift = 8;
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = rc.left;
	vert [0] .y      = rc.top;
	vert [0] .Red    = GetRValue(StartRGB) << Shift;
	vert [0] .Green  = GetGValue(StartRGB) << Shift;
	vert [0] .Blue   = GetBValue(StartRGB) << Shift;
	vert [0] .Alpha  = 0x0000;
	vert [1] .x      = rc.right;
	vert [1] .y      = rc.bottom; 
	vert [1] .Red    = GetRValue(EndRGB) << Shift;
	vert [1] .Green  = GetGValue(EndRGB) << Shift;
	vert [1] .Blue   = GetBValue(EndRGB) << Shift;
	vert [1] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(tdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
}

BOOL DlgContactCallLog::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	m_gdiNavButton.Destroy();

	int iIndent = GetSystemMetrics(SM_CXSMICON)/2;
	int iBtnHeight = GetSystemMetrics(SM_CXSMICON)*2;
	int iBtnWidth = iBtnHeight* 2;

	m_rcHeader = m_rcListBox = m_rcFooter = rc;
	m_rcHeader.bottom = m_rcListBox.top = HEIGHT(rc) / 4;
	m_rcListBox.bottom = m_rcFooter.top = rc.bottom - (iIndent*2 + iBtnHeight);

	m_rcPicture = m_rcContactName = m_rcHeader;
	m_rcPicture.left += iIndent;
	m_rcPicture.top += iIndent;
	m_rcPicture.bottom -= iIndent;
	m_rcPicture.right = m_rcPicture.left + HEIGHT(m_rcPicture);

	m_rcContactName.left = m_rcPicture.right;
	m_rcContactName.top += iIndent;
	m_rcContactName.right -= iIndent;
	m_rcContactName.bottom	= m_rcContactName.top + GetSystemMetrics(SM_CXSMICON)*3;

	m_rcBackBtn.top = m_rcFooter.top + iIndent;
	m_rcBackBtn.bottom = m_rcBackBtn.top +iBtnHeight;
	m_rcBackBtn.left = rc.left + WIDTH(rc)/2 - iBtnWidth/2;
	m_rcBackBtn.right = m_rcBackBtn.left + iBtnWidth;

	m_oCallList.MoveGui(m_rcListBox);

	return UNHANDLED;
}
