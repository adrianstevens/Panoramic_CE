#include "DlgTrialOver.h"
#include "resource.h"
#include "IssGDIDraw.h"
#include "IssCommon.h"
#include "issdebug.h"
#include <aygshell.h>

#define WND_Name				_T("WndTrialOver")
#define WND_Class				_T("ClassTrialOver")
#define MENU_Height				(IsVGA()?46:23)
#define TEXT_HEIGHT_Small		(GetSystemMetrics(SM_CXICON)*13/32)
#define INDENT                  (GetSystemMetrics(SM_CXICON)/4)
#define IDMENU_SelectItem		5000

int g_iIndent = INDENT;

TypePurchase::TypePurchase()
:szTitle(NULL)
,szDescription(NULL)
,szURLLocation(NULL)
,iIconIndex(-1)
{}

TypePurchase::~TypePurchase()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szTitle);
	oStr->Delete(&szDescription);
	oStr->Delete(&szURLLocation);
	iIconIndex = -1;
}

CDlgTrialOver::CDlgTrialOver(void)
:m_szTitle(NULL)
,m_szMessage(NULL)
,m_szClose(NULL)
,m_iNumImages(-1)
,m_iFontHeight(-1)
,m_iFontSmallHeight(-1)
,m_bBtnDown(FALSE)
{
	m_hFont = CIssGDIEx::CreateFont(TEXT_HEIGHT_Small, FW_BOLD, TRUE);
	m_hFontSmall = CIssGDIEx::CreateFont(TEXT_HEIGHT_Small, FW_NORMAL, TRUE);
}

CDlgTrialOver::~CDlgTrialOver(void)
{
	CIssGDIEx::DeleteFont(m_hFont);
	CIssGDIEx::DeleteFont(m_hFontSmall);
	m_oStr->Delete(&m_szTitle);
	m_oStr->Delete(&m_szMessage);
	m_oStr->Delete(&m_szClose);

	m_oList.ResetContent();

	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypePurchase* sItem = m_arrItems[i];
		if(sItem)
			delete sItem;
	}
	m_arrItems.RemoveAll();
}

void CDlgTrialOver::Initialize(UINT uiTitle,
							   UINT uiMessage,
							   UINT uiNoMoreAppsMessage,
							   UINT uiClose,
							   UINT uiImageArray,
							   UINT uiBuyBtn,
							   UINT uiMenuArray)
{
	m_uiTitle		= uiTitle;
	m_uiMessage		= uiMessage;
	m_uiNoMoreAppsMessage = uiNoMoreAppsMessage;
	m_uiClose		= uiClose;
	m_uiImageArray	= uiImageArray;
	m_uiBuyBtn		= uiBuyBtn;
	m_uiMenuArray	= uiMenuArray;
}

BOOL CDlgTrialOver::Launch(HINSTANCE hInst, HWND hWndParent /*= NULL*/)
{
	if(hWndParent)
	{
		return CIssWnd::DoModal(hWndParent, hInst, IDD_DLG_Basic);
	}
	else
	{
		if(CIssWnd::Create(WND_Name, NULL, hInst, WND_Class, 0, 0, 0, 0, 0, WS_VISIBLE))
		{
			ShowWindow(m_hWnd, SW_SHOW);

			MSG msg;
			while( GetMessage( &msg,NULL,0,0 ) )
			{
				TranslateMessage( &msg );
				DispatchMessage ( &msg );
			}
			return TRUE;
		}
	}
	return TRUE;
}

BOOL CDlgTrialOver::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_szTitle		= m_oStr->CreateAndCopy(m_uiTitle, m_hInst);
	m_szClose		= m_oStr->CreateAndCopy(m_uiClose, m_hInst);

#ifdef NMA
	m_szMessage		= m_oStr->CreateAndCopy(m_uiNoMoreAppsMessage, m_hInst);

#else
	m_szMessage		= m_oStr->CreateAndCopy(m_uiMessage, m_hInst);

	if(m_arrItems.GetSize() == 0)
		return TRUE;

	m_gdiImageArray.LoadImage(m_uiImageArray, m_hWnd, m_hInst);
	m_gdiBuyApp.LoadImage(m_uiBuyBtn, m_hWnd, m_hInst);

	m_oList.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_Bounce|OPTION_DrawScrollArrows);
	m_oList.SetCustomDrawFunc(DrawListItem, this);
	m_oList.SetDeleteItemFunc(DeleteMyListItem);
	m_oList.SetImageArray(m_uiMenuArray);
	m_oList.PreloadImages(m_hWnd, m_hInst);

	RefreshList();

    InvalidateRect(hWnd, NULL, FALSE);

#endif
	return TRUE;
}

void CDlgTrialOver::RefreshList()
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypePurchase* sPurchase = m_arrItems[i];
		if(!sPurchase)
			continue;

		m_oList.AddItem((LPVOID)sPurchase, IDMENU_SelectItem);

	}

	m_oList.SetSelectedItemIndex(0, TRUE);

}

void CDlgTrialOver::RecalculateHeights()
{
	int iWidth = GetSystemMetrics(SM_CXSCREEN) - m_gdiImageArray.GetHeight() - 2*g_iIndent - m_oList.GetImageArrayGDI().GetHeight();

	for(int i=0; i<m_oList.GetItemCount(); i++)
	{
		TypeItems* sItem = m_oList.GetItem(i);
		if(!sItem || !sItem->lpItem)
			continue;

		TypePurchase* sPurchase = (TypePurchase*)sItem->lpItem;

		sItem->iHeight	= sItem->iHeightSel = max(m_gdiImageArray.GetHeight(), GetFontHeight() + GetTextHeight(sPurchase->szDescription, iWidth, m_hFontSmall)) + 5*g_iIndent/2;
	}
}

void CDlgTrialOver::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
	if(!lpClass || !sItem || !sItem->lpItem)
		return;

	CDlgTrialOver* pClass = (CDlgTrialOver*)lpClass;
	TypePurchase* sPurchase = (TypePurchase*)sItem->lpItem;
	pClass->DrawMyListItem(gdi, rcDraw, sPurchase, bIsHighlighted);
}

void CDlgTrialOver::DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePurchase* sItem, BOOL bIsHighlighted)
{
	RECT rc = rcDraw;

	if(bIsHighlighted)
	{
		rc.right--;
		rc.bottom--;
        rc.left+=2;
		FrameRect(gdi, rc, RGB(125,125,125));
	}

	Draw(gdi,
		 rcDraw.left + g_iIndent, rcDraw.top + g_iIndent,
		 m_gdiImageArray.GetHeight(), m_gdiImageArray.GetHeight(),
		 m_gdiImageArray,
		 sItem->iIconIndex*m_gdiImageArray.GetHeight(), 0);

	rc.left	= m_gdiImageArray.GetHeight() + 2*g_iIndent;
	rc.top	+= g_iIndent;
	rc.bottom = rc.top + GetFontHeight();
	DrawText(gdi, sItem->szTitle, rc, DT_LEFT|DT_VCENTER, m_hFont, RGB(255,255,255));

	rc.top	= rc.bottom + g_iIndent/2;
	rc.bottom = rcDraw.bottom;
	if(sItem->szDescription)
		DrawText(gdi, sItem->szDescription, rc, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX, m_hFontSmall, RGB(125,125,125));

	Draw(gdi,
		 rcDraw.right - m_gdiBuyApp.GetWidth() - g_iIndent, rcDraw.top + g_iIndent,
		 m_gdiBuyApp.GetWidth(), m_gdiBuyApp.GetHeight(),
		 m_gdiBuyApp);

}

void CDlgTrialOver::DeleteMyListItem(LPVOID lpItem)
{
	if(!lpItem)
		return;

	lpItem  = NULL;
}

BOOL CDlgTrialOver::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC,  rcClient, FALSE, TRUE, FALSE);

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	DrawBackground(rcClip);
	DrawTitle(rcClip);
	DrawMessage(rcClip);
	DrawMenuBar(rcClip);

	m_oList.OnDraw(m_gdiMem, rcClient, rcClip);

	BitBlt(hDC,
		   rcClip.left, rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiMem,
		   rcClip.left, rcClip.top,
		   SRCCOPY);

	return TRUE;
}

void CDlgTrialOver::DrawBackground(RECT& rcClip)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		RECT rc;
		SetRect(&rc, 0,0,m_gdiMem.GetWidth(), m_gdiMem.GetHeight());
		m_gdiBackground.Create(m_gdiMem, rc, FALSE, TRUE, FALSE);

		FillRect(m_gdiBackground, rc, 0);

		rc = m_rcTitle;
		rc.right --;
		rc.bottom -- ;
		GradientFillRect(m_gdiBackground, m_rcTitle, RGB(133,133,133),  RGB(52,54,51), FALSE);
		FrameRect(m_gdiBackground, rc, RGB(30,30,30));

		rc = m_rcMenuBar;
		rc.right --;
		rc.bottom -- ;
		GradientFillRect(m_gdiBackground, m_rcMenuBar, RGB(133,133,133),  RGB(52,54,51), FALSE);
		FrameRect(m_gdiBackground, rc, RGB(30,30,30));

		// draw the separator
		rc = m_rcMenuBar;
		rc.left = m_rcMenuBar.right/2;
		rc.right = rc.left + 1;
		rc.top += IsVGA()?2:1;
		rc.bottom -= IsVGA()?2:1;

		AlphaFillRect(m_gdiBackground, rc, RGB(80,80,80), 150);
		OffsetRect(&rc,1,0);
		AlphaFillRect(m_gdiBackground, rc, RGB(200,200,200), 150);
	}

	BitBlt(m_gdiMem,
		   rcClip.left, rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiBackground,
		   rcClip.left, rcClip.top,
		   SRCCOPY);
}

void CDlgTrialOver::DrawTitle(RECT& rcClip)
{
	if(!IsRectInRect(rcClip, m_rcTitle) || !m_szTitle)
		return;

	RECT rc = m_rcTitle;
	rc.left	+= 2*g_iIndent;

	DrawText(m_gdiMem, m_szTitle, rc, DT_LEFT|DT_VCENTER, m_hFont, RGB(255,255,255));
}

void CDlgTrialOver::DrawMessage(RECT& rcClip)
{
	if(!IsRectInRect(rcClip, m_rcTrialOverMessage) || !m_szMessage)
		return;

	RECT rc = m_rcTrialOverMessage;
	rc.top	+= g_iIndent;

	DrawText(m_gdiMem, m_szMessage, rc, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX | DT_CENTER, m_hFont, RGB(255,255,255));
}

void CDlgTrialOver::DrawMenuBar(RECT& rcClip)
{
	if(!IsRectInRect(rcClip, m_rcMenuBar))
		return;

	RECT rc = m_rcMenuBar;
	rc.right /= 2;

	if(m_bBtnDown)
	{
		FrameRect(m_gdiMem, rc, RGB(255,255,255), 2);
		rc.top ++;
		rc.bottom ++;
		rc.left ++;
		rc.right ++;
	}

	if(m_szClose)
		DrawText(m_gdiMem, m_szClose, rc, DT_CENTER|DT_VCENTER, m_hFont, RGB(255,255,255));
	else
		DrawText(m_gdiMem, _T("Close"), rc, DT_CENTER|DT_VCENTER, m_hFont, RGB(255,255,255));
}

BOOL CDlgTrialOver::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	

	return UNHANDLED;
}

BOOL CDlgTrialOver::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	m_rcTitle		= rcClient;
	m_rcTitle.bottom= rcClient.top + MENU_Height;

	m_rcMenuBar		= rcClient;
	m_rcMenuBar.top = m_rcMenuBar.bottom - MENU_Height;

	m_rcTrialOverMessage = rcClient;
	m_rcTrialOverMessage.top	= m_rcTitle.bottom;
	m_rcTrialOverMessage.bottom = m_rcTitle.bottom + 2*g_iIndent + GetTextHeight(m_szMessage, WIDTH(rcClient), m_hFont);

	m_oList.OnSize(0, m_rcTrialOverMessage.bottom,
				   WIDTH(rcClient), m_rcMenuBar.top - m_rcTrialOverMessage.bottom);

	RecalculateHeights();

	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	return TRUE;
}

BOOL CDlgTrialOver::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oList.OnTimer(wParam, lParam))
		return TRUE;

	KillTimer(hWnd, wParam);
	return TRUE;
}

int CDlgTrialOver::GetFontHeight()
{
	if(m_iFontHeight == -1)
	{
		SIZE sizeText = {0};
		HDC dc = GetDC(m_hWnd);
		HFONT hFontOld = (HFONT)SelectObject(dc, m_hFont);
		GetTextExtentPoint(dc, _T("Ap"), (int)_tcslen( _T("Ap") ), &sizeText);
		SelectObject(dc, hFontOld);
		ReleaseDC(m_hWnd, dc);
		m_iFontHeight = sizeText.cy;
	}
	return m_iFontHeight;
}

int CDlgTrialOver::GetFontSmallHeight()
{
	if(m_iFontSmallHeight == -1)
	{
		SIZE sizeText = {0};
		HDC dc = GetDC(m_hWnd);
		HFONT hFontOld = (HFONT)SelectObject(dc, m_hFontSmall);
		GetTextExtentPoint( dc, _T("Ap"), (int)_tcslen( _T("Ap") ), &sizeText);
		SelectObject(dc, hFontOld);
		ReleaseDC(m_hWnd, dc);
		m_iFontSmallHeight = sizeText.cy;
	}
	return m_iFontSmallHeight;
}

int CDlgTrialOver::GetTextHeight(TCHAR* szText, int iWidth, HFONT hFont)
{
	if(!szText || (int)_tcslen(szText) == 0)
		return 0;

	RECT rc;
	SetRect(&rc, 0, 0, iWidth, GetSystemMetrics(SM_CYSCREEN));
	HDC dc = GetDC(m_hWnd);
	HFONT hFontOld = (HFONT)SelectObject(dc, hFont);
	::DrawText(dc, szText, -1, &rc, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX | DT_CALCRECT);
	SelectObject(dc, hFontOld);
	ReleaseDC(m_hWnd, dc);
	return HEIGHT(rc);
}

BOOL CDlgTrialOver::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE /*|| LOWORD(wParam) == WA_CLICKACTIVE*/) 
	{
		SHFullScreen(hWnd, SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON);
		MoveWindow(hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE); 
        SetForegroundWindow(hWnd);
	}
	return TRUE;
}

BOOL CDlgTrialOver::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{   
	case VK_TSOFT1:
	case VK_TSOFT2:
		CloseWindow();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgTrialOver::OnLButtonDown(HWND hWnd, POINT& pt)
{	
	m_bBtnDown = FALSE;

	if(PtInRect(&m_rcMenuBar, pt))
	{
		m_bBtnDown = TRUE;
		InvalidateRect(hWnd, &m_rcMenuBar, FALSE);
		UpdateWindow(hWnd);
	}
	else if(m_oList.OnLButtonDown(pt))
	{}

	return UNHANDLED;
}

BOOL CDlgTrialOver::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(m_bBtnDown)
	{
		m_bBtnDown = FALSE;
		InvalidateRect(hWnd, &m_rcMenuBar, FALSE);
		UpdateWindow(hWnd);
	}

	if(m_oList.OnLButtonUp(pt))
	{}
	else if(PtInRect(&m_rcMenuBar, pt))
	{
		CloseWindow();
	}
	return UNHANDLED;
}

BOOL CDlgTrialOver::OnMouseMove(HWND hWnd, POINT& pt)
{
	return m_oList.OnMouseMove(pt);
}

BOOL CDlgTrialOver::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_SelectItem:
		SelectItem();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CDlgTrialOver::SelectItem()
{
	TypeItems* sItem = m_oList.GetSelectedItem();
	if(!sItem || !sItem->lpItem)
	{
		MessageBeep(MB_ICONHAND);
		return;
	}

	TypePurchase* sPurchase = (TypePurchase*)sItem->lpItem;

	if(!sPurchase->szURLLocation)
	{
		MessageBeep(MB_ICONHAND);
		return;
	}

	CDlgMoreApps::LaunchMarketplace(sPurchase->szURLLocation, m_hWnd);
	CloseWindow();
}


BOOL CDlgTrialOver::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*switch(LOWORD(wParam))
	{
	case _T('a'):
		PostMessage(m_hWndHtml, DTM_NAVIGATE, NULL, (LPARAM)m_szURL);
		break;
	default:
		break;
	}*/
	return UNHANDLED;
}

BOOL CDlgTrialOver::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oList.OnKeyDown(wParam, lParam);
}


void CDlgTrialOver::CloseWindow()
{
	if(GetParent(m_hWnd) == NULL)
	{
		PostQuitMessage(0);
	}
	else
	{
		SafeCloseWindow(IDOK);
	}
}

void CDlgTrialOver::AddItem(int iIconIndex, 
							TCHAR* szTitle,
							TCHAR* szDescription,
							TCHAR* szURL)
{
	TypePurchase* sNew = new TypePurchase;

	sNew->iIconIndex	= iIconIndex;
	sNew->szTitle		= m_oStr->CreateAndCopy(szTitle);
	if(szDescription)
		sNew->szDescription	= m_oStr->CreateAndCopy(szDescription);
	sNew->szURLLocation = m_oStr->CreateAndCopy(szURL);

	m_arrItems.AddElement(sNew);
}

