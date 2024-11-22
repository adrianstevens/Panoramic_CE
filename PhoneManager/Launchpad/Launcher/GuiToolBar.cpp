#include "StdAfx.h"
#include "GuiToolBar.h"
#include "IssRect.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "DlgLauncher.h"
#include "issdebug.h"
#include "resourceppc.h"
#include "IssGDIFX.h"
#include "ContactsUtil.h"
#include "IssLocalisation.h"
#include "ObjGui.h"

extern CObjGui* g_gui;

#define SELECTOR_DRAWOVER_AMOUNT        11
#define SELECTOR_DRAWOVER_AMOUNT_VGA    18

CGuiToolBar::CGuiToolBar() 
: m_bHasFocus(TRUE)
, m_eMouseState(MS_None)
, m_dwLockTicks(0)
{
}

CGuiToolBar::~CGuiToolBar(void)
{
}

void CGuiToolBar::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    m_gdiImgSelector.Destroy();   
}

BOOL CGuiToolBar::Init(HWND hWndParent, HINSTANCE hInst)
{
    CGuiBase::Init(hWndParent, hInst);


    // add the arrows
    ReloadColorSchemeItems(hWndParent, hInst);

    return TRUE;
}

BOOL CGuiToolBar::Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(IsRectInRect(m_rcSelector, rcClip))
	{
		RECT rcOffset;
		IntersectRect(&rcOffset, &m_rcSelector, &rcClip);

		if(m_gdiImgSelector.GetDC() == NULL)
			InitSelector();

		// draw the selector
		::Draw(gdi,
			   rcOffset,
			   m_gdiImgSelector,
			   rcOffset.left - m_rcSelector.left, rcOffset.top - m_rcSelector.top);

		EnumCurrentGui eCurrent = g_gui->GetCurrentGui();

		if(m_eMouseState != MS_None)
			g_gui->DrawGlow(gdi, m_rcSelector);

		if(m_eMouseState == MS_HoldAndDrag)
		{	
			// see where we are over
			if(g_gui->IsLandscape())
			{
				if(m_ptLastMousePos.y <= g_gui->RectFavorites().bottom)
					eCurrent = GUI_Favorites;
				else if(m_ptLastMousePos.y <= g_gui->RectPrograms().bottom)
					eCurrent = GUI_Programs;
				else if(m_ptLastMousePos.y <= g_gui->RectSettings().bottom)
					eCurrent = GUI_Settings;
				else
					eCurrent = GUI_Quit;		
			}
			else
			{
				if(m_ptLastMousePos.x <= g_gui->RectFavorites().right)
					eCurrent = GUI_Favorites;
				else if(m_ptLastMousePos.x <= g_gui->RectPrograms().right)
					eCurrent = GUI_Programs;
				else if(m_ptLastMousePos.x <= g_gui->RectSettings().right)
					eCurrent = GUI_Settings;
				else
					eCurrent = GUI_Quit;
			}
		}

		g_gui->DrawToolbarItem(gdi, eCurrent, m_rcSelector, m_eMouseState==MS_HoldAndDrag?TRUE:FALSE);

	}

	return TRUE;
}
    
BOOL CGuiToolBar::MoveGui(RECT rcLocation)
{
    m_gdiImgSelector.Destroy();

	m_rcSelector = GetSelectorLocation(g_gui->GetCurrentGui());

    return TRUE;
}
    

BOOL CGuiToolBar::OnLButtonDown(POINT pt)
{
	m_eMouseState = MS_None;
	m_iMaxMove = 0;

	if(PtInRect(&m_rcSelector, pt))
	{
		m_eMouseState		= MS_HoldAndDrag;
		m_ptLastMousePos	= m_ptLButtonDown = pt;

		if(g_gui->IsLandscape())
			m_iDragBtnOffset = pt.y - m_rcSelector.top;
		else
			m_iDragBtnOffset = pt.x - m_rcSelector.left;

		InvalidateRect(m_hWndParent, &m_rcSelector, FALSE);
		UpdateWindow(m_hWndParent);
	}
	else if(PtInRect(&g_gui->RectToolbar(), pt))
	{
		m_eMouseState	= MS_SelectAndAni;

		EnumCurrentGui eNew;
		
		if(PtInRect(&g_gui->RectFavorites(), pt))
			eNew = GUI_Favorites;
		else if(PtInRect(&g_gui->RectPrograms(), pt))
			eNew = GUI_Programs;
		else if(PtInRect(&g_gui->RectSettings(), pt))
			eNew = GUI_Settings;
		else
			eNew = GUI_Quit;

		// animate the new change
		SendMessage(m_hWndParent, WM_AniChange, eNew, 0);

		m_rcSelector = GetSelectorLocation(eNew);
	}


    return FALSE;
}

BOOL CGuiToolBar::OnLButtonUp(POINT pt)
{
	if(m_eMouseState == MS_None)
		return FALSE;

	if(m_eMouseState == MS_HoldAndDrag)
	{
		EnumCurrentGui eNew;

		if(g_gui->IsLandscape())
		{
			if(pt.y <= g_gui->RectFavorites().bottom)
				eNew = GUI_Favorites;
			else if(pt.y <= g_gui->RectPrograms().bottom)
				eNew = GUI_Programs;
			else if(pt.y <= g_gui->RectSettings().bottom)
				eNew = GUI_Settings;
			else
				eNew = GUI_Quit;		
		}
		else
		{
			if(pt.x <= g_gui->RectFavorites().right)
				eNew = GUI_Favorites;
			else if(pt.x <= g_gui->RectPrograms().right)
				eNew = GUI_Programs;
			else if(pt.x <= g_gui->RectSettings().right)
				eNew = GUI_Settings;
			else
				eNew = GUI_Quit;
		}


		RECT rcOld, rcUnion;

		rcOld = m_rcSelector;

		// should animate here
		m_rcSelector = GetSelectorLocation(eNew);

		UnionRect(&rcUnion, &m_rcSelector, &rcOld);
		InvalidateRect(m_hWndParent, &rcUnion, FALSE);

		if(eNew != g_gui->GetCurrentGui())
		{
			// PostMessage... change screen
			g_gui->SetCurrentGui(eNew);
			PostMessage(m_hWndParent, WM_CHANGE_Gui, eNew, 0);
		}
		else
		{
			int iHitSpace = WIDTH(m_rcSelector)/4;
			if(abs(pt.y-m_ptLastMousePos.y) < iHitSpace && abs(pt.x - m_ptLastMousePos.x) < iHitSpace && m_iMaxMove < iHitSpace)
			{
				PostMessage(m_hWndParent, WM_COMMAND, IDMENU_Menu, 0);
			}
		}
	}

	m_eMouseState = MS_None;

	return TRUE;
}

BOOL CGuiToolBar::OnMouseMove(POINT& pt)
{
    if(m_eMouseState == MS_None)
        return FALSE;

	if(m_eMouseState == MS_HoldAndDrag)
	{
		RECT rcOld, rcUnion;

		m_ptLastMousePos = pt;

		if(g_gui->IsLandscape())
		{
			// check for valid values
			if(pt.y < 0 || pt.y > HEIGHT(g_gui->RectToolbar()))
				return TRUE;			

			rcOld = m_rcSelector;

			m_rcSelector.top	= pt.y - m_iDragBtnOffset;
			m_rcSelector.bottom	= m_rcSelector.top + m_gdiImgSelector.GetHeight();

			m_iMaxMove = max(m_iMaxMove, abs(m_ptLButtonDown.y-pt.y));
		}
		else
		{
			// check for valid values
			if(pt.x < 0 || pt.x > WIDTH(g_gui->RectToolbar()))
				return TRUE;			

			rcOld = m_rcSelector;

			m_rcSelector.left	= pt.x - m_iDragBtnOffset;
			m_rcSelector.right	= m_rcSelector.left + m_gdiImgSelector.GetWidth();

			m_iMaxMove = max(m_iMaxMove, abs(m_ptLButtonDown.x-pt.x));
		}

		UnionRect(&rcUnion, &rcOld, &m_rcSelector);
		InvalidateRect(m_hWndParent, &rcUnion, FALSE);
		UpdateWindow(m_hWndParent);
	}

 
    return TRUE;
}
        
void CGuiToolBar::SetFocus(BOOL bHasFocus)
{
    m_bHasFocus = bHasFocus;

}
    

BOOL CGuiToolBar::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_RETURN:
    	break;
    default:
        return UNHANDLED;
        break;
    }


	BOOL bChanged = FALSE;
	if(!m_bHasFocus)	// getting focus - must redraw
		bChanged = TRUE;

    SetFocus(TRUE);
   /* EnumCurrentGui eOld = m_eSelected;

	if(WIDTH(m_rcLocation) > HEIGHT(m_rcLocation))	// portrait
	{
		if(wParam == VK_LEFT)		
		{
			if(m_eSelected > GUI_Favorites)
			{
				m_eSelected = (EnumCurrentGui)(m_eSelected - 1);
				bChanged = TRUE;
			}
		}
		else if(wParam == VK_RIGHT)	
		{
			if(m_eSelected < GUI_Quit)
			{
				m_eSelected = (EnumCurrentGui)(m_eSelected + 1);
				bChanged = TRUE;
			}
		}
		else if(wParam == VK_UP)	
		{
			m_bHasFocus = FALSE;
			bChanged = TRUE;
		}
        else if(wParam == VK_DOWN || m_eSelected != GUI_Favorites)
        {
            m_bHasFocus = FALSE;
            bChanged = TRUE;
        }
	}
	else										// landscape
	{
	    if(wParam == VK_RIGHT)		
		{
            m_bHasFocus = FALSE;
            bChanged = TRUE;
		}
		else if(wParam == VK_DOWN)	
		{
            if(m_eSelected < GUI_Quit)
            {
                m_eSelected = (EnumCurrentGui)(m_eSelected + 1);
                bChanged = TRUE;
            }
		}
		else if(wParam == VK_UP)	
		{
            if(m_eSelected > GUI_Favorites)
            {
                m_eSelected = (EnumCurrentGui)(m_eSelected - 1);
                bChanged = TRUE;
            }
		}
        else if(wParam == VK_LEFT || m_eSelected != GUI_Favorites)		
        {
            m_bHasFocus = FALSE;
            bChanged = TRUE;
        }


	}
	if(wParam == VK_RETURN)	
	{
        if(GetCurSelection() != m_eSelected)
        {
            CaptureAnimationGDI();
            DoChangeGui(m_eSelected);
       }
	}

	if(bChanged)
    {
        SetDirty(eOld, m_eSelected);
        // for now, make inval area big enough to paint the entire selector
        // to erase glow
        RECT rcInval = m_rcDirty;//m_rcLocation;
        if(IsLandscape())
            rcInval.left = m_rcDrawBounds.left;
        else
            rcInval.top = m_rcDrawBounds.top;

        m_dlgParent->SetDirty(rcInval);
		InvalidateRect(m_hWndParent, &rcInval, FALSE);
    }*/

	return FALSE;
}
  	
void CGuiToolBar::LoseFocus()	
{ 
    if(!m_bHasFocus)
        return;

    m_bHasFocus = FALSE; 

    // invalidate the selected button; it has lost focus
    // if selected is the 'selector', inval the entire btn
    /*SetDirty(0, 3);//BUGBUG ... shouldn't be nesessary but 
    RECT rcInval = m_rcToolbarBtns[m_eSelected];
    if(GetCurSelection() == m_eSelected)
    {
        if(IsLandscape())
            rcInval.right += GetSelectorDrawOverAmount();
        else
            rcInval.top -= GetSelectorDrawOverAmount();
    }
    InvalidateRect(m_hWndParent, &rcInval, FALSE);*/
}

RECT CGuiToolBar::GetSelectorLocation(EnumCurrentGui eGui)
{
	RECT rc;

	switch(eGui)
	{
	case GUI_Programs:
		rc = g_gui->RectPrograms();
		break;
	case GUI_Settings:
		rc = g_gui->RectSettings();
		break;
	case GUI_Quit:
		rc = g_gui->RectBack();
		break;
	case GUI_Favorites:
	default:
		rc = g_gui->RectFavorites();
		break;
	}

	if(m_gdiImgSelector.GetDC() == NULL)
		InitSelector();

	if(g_gui->IsLandscape())
	{
		rc.top		= rc.top + (HEIGHT(rc) - m_gdiImgSelector.GetHeight())/2;
		rc.bottom	= rc.top + m_gdiImgSelector.GetHeight();
		rc.right	= rc.left + m_gdiImgSelector.GetWidth();
	}
	else
	{
		rc.left		= rc.left + (WIDTH(rc) - m_gdiImgSelector.GetWidth())/2;
		rc.right	= rc.left + m_gdiImgSelector.GetWidth();
		rc.top		= rc.bottom - m_gdiImgSelector.GetHeight();
	}
	
	return rc;
}

void CGuiToolBar::InitSelector()
{
	m_gdiImgSelector.LoadImage(g_gui->IsLandscape()?g_gui->GetSkin().uiToolBarSelectorLan:g_gui->GetSkin().uiToolBarSelector, m_hWndParent, m_hInst, TRUE);

	CIssGDIEx gdiArrow;

	gdiArrow.LoadImage(g_gui->IsLandscape()?g_gui->GetSkin().uiToolBarArrowLan:g_gui->GetSkin().uiToolBarArrow, m_hWndParent, m_hInst, TRUE);

	if(g_gui->IsLandscape())
	{
		// draw the landscape arrow on the bottom right corner
		::Draw(m_gdiImgSelector,
			m_gdiImgSelector.GetWidth() - gdiArrow.GetWidth(), m_gdiImgSelector.GetHeight() - gdiArrow.GetHeight(),
			gdiArrow.GetWidth(), gdiArrow.GetHeight(),
			gdiArrow,
			0,0);
	}
	else
	{
		// draw the arrow on the top right corner of the selector
		::Draw(m_gdiImgSelector,
			m_gdiImgSelector.GetWidth() - gdiArrow.GetWidth(), 0,
			gdiArrow.GetWidth(), gdiArrow.GetHeight(),
			gdiArrow,
			0,0);
	}
}

void CGuiToolBar::ResetSelector()
{
	m_rcSelector = GetSelectorLocation(g_gui->GetCurrentGui());
}

