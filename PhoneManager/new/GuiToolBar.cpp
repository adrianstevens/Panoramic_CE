#include "StdAfx.h"
#include "GuiToolBar.h"
#include "IssRect.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "DlgContacts.h"
#include "issdebug.h"
#include "resourceppc.h"
#include "IssGDIFX.h"
#include "ContactsUtil.h"

#define SELECTOR_DRAWOVER_AMOUNT        11
#define SELECTOR_DRAWOVER_AMOUNT_VGA    18

HRESULT ToolbarAnimateFromTo(HDC hdcScreen,
					  CIssGDIEx&	gdiBackground,
					  CIssGDIEx&	gdiAniObject,
					  POINT		ptFrom,
					  POINT		ptTo,
					  int		iXStart,
					  int		iYStart,
					  int		iWidth,
					  int		iHeight,
					  int		iDrawFrames,
                      int       iTime   // in ms
                      );

static CGuiToolBar* s_guiToolbar = 0;
CGuiToolBar::CGuiToolBar() 
: m_bHasFocus(TRUE)
, m_eSelected(GUI_Favorites)
, m_eLButtonDown(GUI_Favorites)
, m_bBtnDown(FALSE)
, m_hFontBtnText(NULL)
, m_dwLockTicks(0)
{
    s_guiToolbar = this;
    int iTextSize = 14;
    if(IsVGA())
        iTextSize*=2;
	m_hFontBtnText = CIssGDIEx::CreateFont(iTextSize, FW_NORMAL, TRUE);
}

CGuiToolBar::~CGuiToolBar(void)
{
	CIssGDIEx::DeleteFont(m_hFontBtnText);
    s_guiToolbar = 0;
}

void CGuiToolBar::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    CIssGDIEx gdiArrow, gdiArrowLan;
    gdiArrow.LoadImage(SKIN(IDR_PNG_Arrow), hWnd, hInstance, TRUE);
    gdiArrowLan.LoadImage(SKIN(IDR_PNG_Arrow_Lan), hWnd, hInstance, TRUE);

    // draw the arrow on the top right corner of the selector
    ::Draw(m_gdiImgSelector,
         m_gdiImgSelector.GetWidth() - gdiArrow.GetWidth(), 0,
         gdiArrow.GetWidth(), gdiArrow.GetHeight(),
         gdiArrow,
         0,0);

    // draw the landscape arrow on the bottom right corner
    ::Draw(m_gdiImgSelectorLan,
         m_gdiImgSelectorLan.GetWidth() - gdiArrowLan.GetWidth(), m_gdiImgSelectorLan.GetHeight() - gdiArrowLan.GetHeight(),
         gdiArrowLan.GetWidth(), gdiArrowLan.GetHeight(),
         gdiArrowLan,
         0,0);
    
}

BOOL CGuiToolBar::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContacts* dlgParent)
{
    CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);
    m_dlgParent = dlgParent;

    if(IsVGA())
    {
        m_gdiImgSelector.LoadImage(IDR_PNG_ToolbarSelectorVGA_p, hWndParent, hInst, TRUE);
        m_gdiImgSelectorLan.LoadImage(IDR_PNG_ToolbarSelectorVGA_l, hWndParent, hInst, TRUE);
        m_gdiImgIconArray.LoadImage(IDR_PNG_ToolbarIconArrayVGA, hWndParent, hInst, TRUE);
    }
    else
    {
        m_gdiImgSelector.LoadImage(IDR_PNG_ToolbarSelector_p, hWndParent, hInst, TRUE);
        m_gdiImgSelectorLan.LoadImage(IDR_PNG_ToolbarSelector_l, hWndParent, hInst, TRUE);
        m_gdiImgIconArray.LoadImage(IDR_PNG_ToolbarIconArray, hWndParent, hInst, TRUE);
    }

    // add the arrows
    ReloadColorSchemeItems(hWndParent, hInst);

    m_oStr->StringCopy(m_szBtnText[0], IDS_TOOLBAR_FAV,         STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(m_szBtnText[1], IDS_TOOLBAR_CONTACTS,    STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(m_szBtnText[2], IDS_TOOLBAR_HISTORY,     STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(m_szBtnText[3], IDS_TOOLBAR_EXIT,        STRING_NORMAL, m_hInst);

    // set selector
    EnumCurrentGui eSelection = (EnumCurrentGui)0;
    if(m_dlgParent)
        eSelection = GetCurSelection();
    m_eSelected = eSelection;

    return TRUE;
}
int CGuiToolBar::GetDragThreshold()
{
    return IsVGA() ? TOOLBAR_MIN_DragMoveThresh * 2 : TOOLBAR_MIN_DragMoveThresh;
}

int CGuiToolBar::GetSelectorDrawOverAmount()
{
    if(IsVGA())
        return SELECTOR_DRAWOVER_AMOUNT_VGA;

    return SELECTOR_DRAWOVER_AMOUNT;
}
    
EnumCurrentGui CGuiToolBar::GetCurSelection()
{
    return m_dlgParent->GetCurSelection();
}

BOOL CGuiToolBar::Draw(HDC hDC, RECT rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    //if(m_bHoldAndDrag)
    //    return TRUE;

	if(m_gdiBg->GetDC() == NULL)
    {
        RECT rcClient;
        ::GetWindowRect(m_hWndParent, &rcClient);
		m_gdiBg->Create(hDC, rcClient, FALSE, TRUE, TRUE/*FALSE*/);
    }

    Render();

	BitBlt(hDC,
		   m_rcLocation.left, m_rcLocation.top,
		   WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
		   m_gdiBg->GetDC(),
		   m_rcLocation.left, m_rcLocation.top,
		   SRCCOPY);


	return TRUE;
}
    
HRESULT CGuiToolBar::Render()
{
    // redraw whatever is dirty
    HRESULT hr = S_OK;
    if(IsRectEmpty(&m_rcDirty))
        return hr;

    DrawToolbarBg(*m_gdiBg);

    DrawButtonLabels(*m_gdiBg);

    SetRectEmpty(&m_rcDirty);
    return hr;
}

HRESULT CGuiToolBar::DrawToolbarBg(CIssGDIEx& gdi)
{
    HRESULT hr = S_OK;
    // create 3 fill rects
    RECT rc = m_rcDirty;//m_rcLocation;

    RECT rcBase, rcGrad1, rcGrad2;
    rcBase = rcGrad1 = rcGrad2 = rc;

    if(IsLandscape())
    {
        rcBase.right = rcGrad1.left = rcBase.left + (long)(WIDTH(m_rcLocation)*0.05);
        rcGrad1.right = rcGrad2.left = rcGrad1.left + (long)(WIDTH(m_rcLocation)*0.7);
        FillRect(gdi, rcBase, COLOR_TOOLBAR_BASE);
        GradientHorzFillRect(gdi, rcGrad1, COLOR_TOOLBAR_GRAD1_ST, COLOR_TOOLBAR_GRAD1_END, FALSE);
        GradientHorzFillRect(gdi, rcGrad2, COLOR_TOOLBAR_GRAD2_ST, COLOR_TOOLBAR_GRAD2_END, FALSE);

        // separators
        int iBtnOffset = HEIGHT(m_rcLocation) / 4 - 1;
        RECT rcSeperator = rc;
        rc.right -= 4;
        rc.top = 0;
        rc.bottom = rc.top + 1;
        for(int i = 0; i < NUM_TOOLBAR_BTNS - 1; i++)
        {
            // light (button left)
            hr = AlphaFillRect(gdi, rc, COLOR_TOOLBAR_SEP_LIGHT, 150);
            CHR(hr, _T("DrawToolbarBg()"));
            // dark (button right)
            OffsetRect(&rc, 0,iBtnOffset);
            hr = AlphaFillRect(gdi, rc, COLOR_TOOLBAR_SEP_DARK, 150);
            CHR(hr, _T("DrawToolbarBg()"));
            OffsetRect(&rc, 0,1);
        }
    }
    else
    {
        rcBase.top = rcGrad1.bottom = rcBase.bottom - (long)(HEIGHT(m_rcLocation)*0.05);
        rcGrad1.top = rcGrad2.bottom = rcGrad1.bottom - (long)(HEIGHT(m_rcLocation)*0.7);
        FillRect(gdi, rcBase, COLOR_TOOLBAR_BASE);
        GradientFillRect(gdi, rcGrad1, COLOR_TOOLBAR_GRAD1_END, COLOR_TOOLBAR_GRAD1_ST, FALSE);
        GradientFillRect(gdi, rcGrad2, COLOR_TOOLBAR_GRAD2_END, COLOR_TOOLBAR_GRAD2_ST, FALSE);
        // separators
        int iBtnOffset = WIDTH(m_rcLocation) / 4 - 1;
        RECT rcSeperator = rc;
        rc.top += 4;
        rc.left = 0;
        rc.right = rc.left + 1;
        for(int i = 0; i < NUM_TOOLBAR_BTNS - 1; i++)
        {
            // light (button left)
            hr = AlphaFillRect(gdi, rc, COLOR_TOOLBAR_SEP_LIGHT, 150);
            CHR(hr, _T("DrawToolbarBg()"));
            // dark (button right)
            OffsetRect(&rc, iBtnOffset,0);
            hr = AlphaFillRect(gdi, rc, COLOR_TOOLBAR_SEP_DARK, 150);
            CHR(hr, _T("DrawToolbarBg()"));
            OffsetRect(&rc, 1,0);
        }
  }
Error:
    return hr;
}

CGuiToolBar* CGuiToolBar::GetToolbar()
{
    return s_guiToolbar;
}


HRESULT CGuiToolBar::DrawSelector(CIssGDIEx& gdi, RECT& rc, RECT& rcClip, BOOL bDrawDestZero /*= TRUE*/)
{
    HRESULT hr = S_OK;
    int iSelectorIndex = (int)GetCurSelection();
    COLORREF cr = /*( iSelectorIndex ==  m_eSelected) ? COLOR_TEXT_SELECTED :*/ COLOR_TEXT_NORMAL;

    if(IsLandscape())
    {
        // note: caller gdi's are all 0-based (i.e. GuiContacts)
        // for adjustment capability,
        // image width is wider that we need for landscape - use the amount needed for this toolbar
        int iBtnHeight = HEIGHT(m_rcLocation)/4;
        int iBtnWidth = WIDTH(m_rcLocation) + GetSelectorDrawOverAmount();  
                                                   
        int y = iSelectorIndex * iBtnHeight;
        y -= (m_gdiImgSelectorLan.GetHeight() - iBtnHeight) / 2;

        // see if drawing is needed
        if(iBtnWidth <= rcClip.left || y >= rcClip.bottom || y + iBtnWidth <= rcClip.top)
            return S_OK;

        int x = m_rcLocation.left;

        if(bDrawDestZero)
        {
            hr = ::Draw(gdi, x/*rc.left*/, y, iBtnWidth-rc.left,m_gdiImgSelectorLan.GetHeight(), 
                m_gdiImgSelectorLan, 
                m_gdiImgSelectorLan.GetWidth() - iBtnWidth + rc.left + x, 0);
        }
        else
        {
            hr = ::Draw(gdi, /*x*/rc.left, y, iBtnWidth-rc.left,m_gdiImgSelectorLan.GetHeight(), 
                m_gdiImgSelectorLan, 
                m_gdiImgSelectorLan.GetWidth() - iBtnWidth + rc.left + x, 0);
        }

        RECT rcBtn = m_rcToolbarBtns[iSelectorIndex];
        rcBtn.top += HEIGHT(m_rcToolbarBtns[iSelectorIndex])/8;
        rcBtn.bottom -= HEIGHT(m_rcToolbarBtns[iSelectorIndex])/6;//12;

        if(rcBtn.right > rc.left)
            DrawText(gdi.GetDC(), m_szBtnText[iSelectorIndex], rcBtn, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
        if(rcBtn.right > rc.left)
            DrawBtnIcon(gdi, rcBtn, iSelectorIndex);
    }
    else
    {
        // for adjustment capability,
        // image height is higher that we need for landscape - use the amount needed for this toolbar
       int iBtnWidth = WIDTH(m_rcLocation)/4;
        int iBtnHeight = HEIGHT(m_rcLocation) + GetSelectorDrawOverAmount();  
        int x = iSelectorIndex * iBtnWidth;
        x -= (m_gdiImgSelector.GetWidth() - iBtnWidth) / 2;
        int y = m_rcLocation.bottom - iBtnHeight;//m_gdiImgSelector.GetHeight();
        int h = rcClip.bottom-y;

        if(y < rcClip.bottom)
            hr = ::Draw(gdi, x, y, m_gdiImgSelector.GetWidth(),h, m_gdiImgSelector);
        
        RECT rcBtn = m_rcToolbarBtns[iSelectorIndex];
        rcBtn.bottom -= 4;

        if(rcBtn.top < rcClip.bottom)
        {
		    DrawText(gdi.GetDC(), m_szBtnText[iSelectorIndex], rcBtn, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
            DrawBtnIcon(gdi, rcBtn, iSelectorIndex);
        }
    }

   return hr;
}

HRESULT CGuiToolBar::DrawButtonLabels(CIssGDIEx& gdi)
{
    HRESULT hr = S_OK;

    CBARG(m_dlgParent != NULL, _T("CGuiToolBar::DrawButtonLabels()"));

    if(IsLandscape())
    {
        for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
        {
            if(!IsRectInRect(m_rcToolbarBtns[i], m_rcDirty))
                continue;
            // button text
            COLORREF cr = /*i == m_eSelected ? COLOR_TEXT_SELECTED :*/ COLOR_TEXT_NORMAL; 
            RECT rc = m_rcToolbarBtns[i];//{0, m_rcToolbarBtns[i].top, WIDTH(m_rcToolbarBtns[i]), m_rcToolbarBtns[i].bottom };
            rc.top += HEIGHT(m_rcToolbarBtns[i])/8;//8;
            rc.bottom -= HEIGHT(m_rcToolbarBtns[i])/6;//10;
            if(m_bBtnDown && i == m_eLButtonDown)
            {
                OffsetRect(&rc, 1,1);
                DrawText(gdi.GetDC(), m_szBtnText[i], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
            }
            else
            {
                DrawText(gdi.GetDC(), m_szBtnText[i], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
            }
            DrawBtnIcon(gdi, rc, i, /*m_bBtnDown*/m_bHoldAndDrag );
        }
    }
    else
    {
 	    for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
	    {
            if(!IsRectInRect(m_rcToolbarBtns[i], m_rcDirty))
                continue;
            // button text
            COLORREF cr = /*i == m_eSelected ? COLOR_TEXT_SELECTED :*/ COLOR_TEXT_NORMAL; 
            RECT rc = m_rcToolbarBtns[i];//{m_rcToolbarBtns[i].left,0,m_rcToolbarBtns[i].right,HEIGHT(m_rcToolbarBtns[i])};
            rc.bottom -= 4;
		    if(m_bBtnDown && i == m_eLButtonDown)
		    {
			    OffsetRect(&rc, 1,1);
			    DrawText(gdi.GetDC(), m_szBtnText[i], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
		    }
		    else
            {
			    DrawText(gdi.GetDC(), m_szBtnText[i], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, cr);
            }
            DrawBtnIcon(gdi, rc, i, m_bHoldAndDrag);
	    }
   }

Error:
	return hr;
}

HRESULT CGuiToolBar::DrawBtnIcon(CIssGDIEx& gdi, RECT& rc, int iBtnIndex, BOOL bExcludeGlow /*= FALSE*/)
{
    HRESULT hr = S_OK;

    RECT rcIcon = rc;
    rcIcon.left = rc.left + WIDTH(rc)/2 - m_gdiImgIconArray.GetHeight()/2;
    rcIcon.right = rcIcon.left + m_gdiImgIconArray.GetHeight();
    rcIcon.top += (HEIGHT(rc)/12);
    rcIcon.bottom = rcIcon.top + WIDTH(rcIcon);

    // draw glow image
    if(m_gdiGlowImg && !bExcludeGlow && HasFocus() && iBtnIndex == m_eSelected)
    {
        int iGlowWidth = m_gdiGlowImg->GetWidth();
        int iInflate = (iGlowWidth - WIDTH(rcIcon)) / 2;
        RECT rcGlow;
        rcGlow.top = (rcIcon.top + HEIGHT(rcIcon)/2) - (iGlowWidth / 2);//rc.top;
        rcGlow.bottom = rcGlow.top + iGlowWidth;    // make square
        rcGlow.left = (rc.left + WIDTH(rc)/2) - (iGlowWidth / 2);
        rcGlow.right = rcGlow.left  + iGlowWidth;

        UnionRect(&m_rcDrawBounds, &m_rcDrawBounds, &rcGlow);
        ::Draw(gdi, rcGlow, *m_gdiGlowImg);
    }

    ::Draw(gdi,
        rcIcon.left, rcIcon.top,
        WIDTH(rcIcon), HEIGHT(rcIcon),
        m_gdiImgIconArray,
        iBtnIndex*m_gdiImgIconArray.GetHeight(), 0);  
	
    return hr;
}

BOOL CGuiToolBar::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

	BOOL bLandscape = (rcLocation.right - rcLocation.left < rcLocation.bottom - rcLocation.top);

	for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
		m_rcToolbarBtns[i] = rcLocation;

	if(bLandscape)
	{
		int iBtnHeight = HEIGHT(rcLocation) / NUM_TOOLBAR_BTNS;
		int y = 0;
		for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
		{
			m_rcToolbarBtns[i].top = y;
			y += iBtnHeight;
			m_rcToolbarBtns[i].bottom = y;
		}
	}
	else
	{
		int iBtnWidth = WIDTH(rcLocation) / NUM_TOOLBAR_BTNS;
		int x = 0;
		for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
		{
			m_rcToolbarBtns[i].left = x;
			x += iBtnWidth;
			m_rcToolbarBtns[i].right = x;
		}
	}

    if(IsLandscape())
        m_iSelectorPos = m_rcToolbarBtns[m_eSelected].top;
    else
        m_iSelectorPos = m_rcToolbarBtns[m_eSelected].left;

    SetAllDirty();
    m_rcDrawBounds = m_rcLocation;

    return TRUE;
}
    
void CGuiToolBar::SetDirty(int iStart, int iEnd)
{
    if(iStart < 0)
        iStart = 0;
    if(iEnd >= NUM_TOOLBAR_BTNS)
        iEnd = NUM_TOOLBAR_BTNS - 1;

    UnionRect(&m_rcDirty, &m_rcDirty, &m_rcToolbarBtns[iStart]);
    UnionRect(&m_rcDirty, &m_rcDirty, &m_rcToolbarBtns[iEnd]);
}

BOOL CGuiToolBar::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

    // don't take focus here - could be menu launch
	//m_bHasFocus = TRUE;
	m_bBtnDown = TRUE;
    m_ptLButtonDown = pt;

	for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
	{
		if(PtInRect(&m_rcToolbarBtns[i], pt))
		{
			m_eLButtonDown = (EnumCurrentGui)i;
            SetDirty(m_eSelected, m_eSelected);
            m_eSelected = m_eLButtonDown;
            break;
		}
	}

    if(IsLandscape())
        m_iDragBtnOffset = pt.y - m_rcToolbarBtns[m_eLButtonDown].top;
    else
        m_iDragBtnOffset = pt.x - m_rcToolbarBtns[m_eLButtonDown].left;

    // for button press drawing
    SetDirty(m_eSelected, GetCurSelection());
    RECT rcInval;
    UnionRect(&rcInval, &m_rcToolbarBtns[m_eSelected], &m_rcToolbarBtns[GetCurSelection()]);

    // add the selctor area
    if(IsLandscape())
        rcInval.right += GetSelectorDrawOverAmount();
    else
        rcInval.top -= GetSelectorDrawOverAmount();

	InvalidateRect(m_hWndParent, &rcInval, FALSE);

    m_ptLastMousePos = pt;

    return TRUE;
}

BOOL CGuiToolBar::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	m_bBtnDown = FALSE;

    // we have clicked in the toolbar, but not on selector
    if(PtInRect(&m_rcLocation, m_ptLButtonDown) && !PtInRect(&m_rcToolbarBtns[GetCurSelection()], pt))
	    SetFocus(TRUE);

    EnumCurrentGui eLast = GetCurSelection();
	EnumCurrentGui eNew = (EnumCurrentGui)0;

    if(m_bHoldAndDrag)
    {
        if(IsLandscape())
             m_iSelectorPos = pt.y - m_iDragBtnOffset;
        else
           m_iSelectorPos = pt.x - m_iDragBtnOffset;
    }
    else
    {
        if(IsLandscape())
            m_iSelectorPos = m_rcToolbarBtns[eLast].top;
        else
            m_iSelectorPos = m_rcToolbarBtns[eLast].left;
    }

    // test from center of selector
    POINT ptBtnTest = pt;
    if(m_bHoldAndDrag)
    {
        if(IsLandscape())
            ptBtnTest.y = m_iSelectorPos + HEIGHT(m_rcToolbarBtns[m_eLButtonDown])/2;
        else
            ptBtnTest.x = m_iSelectorPos + WIDTH(m_rcToolbarBtns[m_eLButtonDown])/2;
    }

	for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
	{
		if(PtInRect(&m_rcToolbarBtns[i], ptBtnTest))
		{
            eNew = (EnumCurrentGui)i;
            m_eSelected = eNew;
            break;
		}
	}

    // lock out until previous animation is complete
    if(GetTickCount() < m_dwLockTicks)
    {
        InvalidateRect(m_hWndParent, &m_rcLocation, FALSE);
        return TRUE;
    }

    if(m_bHoldAndDrag)
    {
	    if(eNew != eLast)
	    {
		    m_eSelected = eNew;
	    }
        SetDirty(eNew, eLast);
        // add the selctor area
        RECT rcInval = m_rcDirty;
        if(IsLandscape())
            rcInval.right += GetSelectorDrawOverAmount();
        else
            rcInval.top -= GetSelectorDrawOverAmount();

        DoChangeGui(m_eSelected);
	    InvalidateRect(m_hWndParent, &rcInval, FALSE);
    }
    else
    {
 	    if(eNew != eLast)
	    {
		    m_eSelected = eNew;
            DoChangeGui(m_eSelected);
	    }
        else if(m_eLButtonDown == eNew)
        {
		    m_dlgParent->LaunchMenu();
	        InvalidateRect(m_hWndParent, &m_rcLocation, FALSE);
        }
   }
    m_bHoldAndDrag = FALSE;

	return TRUE;
}

BOOL CGuiToolBar::OnMouseMove(POINT& pt)
{
    if(!m_bBtnDown)
        return UNHANDLED;

    if(m_eSelected == UND)
        return UNHANDLED;

    if(!m_bHoldAndDrag && abs(m_ptLastMousePos.x - pt.x) < GetDragThreshold() && 
        abs(m_ptLastMousePos.y - pt.y) < GetDragThreshold())
        return UNHANDLED;

    // set m_bHoldAndDrag
    if(m_eSelected == GetCurSelection() && m_eSelected == m_eLButtonDown)
        m_bHoldAndDrag = TRUE;

    SetFocus(TRUE);

    HRESULT hr;
    CIssGDIEx	gdiTemp;

    // update selector position
    int iLastSelectorPos = m_iSelectorPos;
    if(IsLandscape())
        m_iSelectorPos = pt.y - m_iDragBtnOffset;
    else
        m_iSelectorPos = pt.x - m_iDragBtnOffset;

    // see if there is a new selection
    // there is a new selection if the center of btn crosses a boundary
    POINT ptBtnTest = pt;
    if(m_bHoldAndDrag)
    {
        if(IsLandscape())
            ptBtnTest.y = m_iSelectorPos + HEIGHT(m_rcToolbarBtns[m_eLButtonDown])/2;
        else
            ptBtnTest.x = m_iSelectorPos + WIDTH(m_rcToolbarBtns[m_eLButtonDown])/2;
    }

	for(int i = 0; i < NUM_TOOLBAR_BTNS; i++)
	{
		if(PtInRect(&m_rcToolbarBtns[i], ptBtnTest))
		{
            if(m_eSelected != i)
            {
                EnumCurrentGui eWasSelected = m_eSelected;
                m_eSelected = (EnumCurrentGui)i;
                if(m_bHoldAndDrag)
                {
                    // draw a new selector
                    CaptureAnimationGDI();
                }
                else
                {
                    // invalidate old and new - all for now
                    SetDirty(eWasSelected, m_eSelected);
	                InvalidateRect(m_hWndParent, &m_rcDirty, FALSE);
                }
            }
            break;
		}
	}

    if(!m_bHoldAndDrag)
        return UNHANDLED;

    // if we're dragging a toolbar icon, then draw it
    // like an animation - just one frame
    // should already be captured - just draw it
    HDC hdcScreen = ::GetDC(m_dlgParent->GetWnd());

    RECT rcDraw = m_rcToolbarBtns[m_eLButtonDown];
    RECT rcBtn = m_rcToolbarBtns[m_eLButtonDown];

    if(IsLandscape())
    {
        m_iSelectorPos = pt.y - m_iDragBtnOffset;
        iLastSelectorPos = m_ptLastMousePos.y - m_iDragBtnOffset;

        int iBtnHeight = HEIGHT(m_rcToolbarBtns[m_eLButtonDown]);
        int iBtnOffset = (m_gdiImgSelectorLan.GetHeight() - iBtnHeight) / 2;

        // draw area
        rcDraw.top = min(m_iSelectorPos, iLastSelectorPos);
        rcDraw.bottom = max(m_iSelectorPos, iLastSelectorPos) + HEIGHT(m_rcToolbarBtns[m_eLButtonDown]);
        rcDraw.top -= iBtnOffset;
        rcDraw.bottom += iBtnOffset;
        rcDraw.right = rcDraw.left + GetSelectorHeight();//m_gdiImgSelectorLan.GetWidth();

         // selector area (relative to draw area)
        rcBtn.top = m_iSelectorPos;
        rcBtn.top -= iBtnOffset;
        rcBtn.top -= rcDraw.top;    // zero it
        rcBtn.bottom = rcBtn.top + m_gdiImgSelectorLan.GetHeight();
        rcBtn.left = 0;
        rcBtn.right = rcBtn.left + m_gdiImgSelectorLan.GetWidth();

    }
    else
    {
        m_iSelectorPos = pt.x - m_iDragBtnOffset;
        iLastSelectorPos = m_ptLastMousePos.x - m_iDragBtnOffset;

        int iBtnWidth = WIDTH(m_rcToolbarBtns[m_eLButtonDown]);
        int iBtnOffset = (m_gdiImgSelector.GetWidth() - iBtnWidth) / 2;

        // draw area
        rcDraw.left = min(m_iSelectorPos, iLastSelectorPos);
        rcDraw.right = max(m_iSelectorPos, iLastSelectorPos) + WIDTH(m_rcToolbarBtns[m_eLButtonDown]);
        rcDraw.left -= iBtnOffset;
        rcDraw.right += iBtnOffset;
        rcDraw.top = rcDraw.bottom - GetSelectorHeight();//m_gdiImgSelector.GetHeight();

         // selector area (relative to draw area)
        rcBtn.left = m_iSelectorPos;
        rcBtn.left -= iBtnOffset;
        rcBtn.left -= rcDraw.left;  // zero it
        rcBtn.bottom = HEIGHT(rcDraw);
        rcBtn.top = rcBtn.bottom - GetSelectorHeight();//m_gdiImgSelector.GetHeight();
        rcBtn.right = rcBtn.left + m_gdiImgSelector.GetWidth();

    }

    if(!m_gdiMovingBtn.GetDC())
        CaptureAnimationGDI();

    hr = gdiTemp.Create(hdcScreen, rcDraw, FALSE, TRUE, TRUE);
    CHR(hr, _T("gdiTemp.Create"));

    // Draw the old Background
    BitBlt(gdiTemp.GetDC(),
       0, 0,
       WIDTH(rcDraw), HEIGHT(rcDraw),
       m_gdiBg->GetDC(),
       rcDraw.left, rcDraw.top,
       SRCCOPY);

    hr = ::Draw(gdiTemp, rcBtn, m_gdiMovingBtn);

    // draw it all to the screen
    BitBlt( hdcScreen,
            rcDraw.left,rcDraw.top,
            rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
            gdiTemp.GetDC(),
            0, 0,
            SRCCOPY);


    ::ReleaseDC(m_dlgParent->GetWnd(), hdcScreen);

    m_ptLastMousePos = pt;

Error:
    return UNHANDLED;
}
        
void CGuiToolBar::SetFocus(BOOL bHasFocus)
{
    m_dlgParent->NotifyToolbarFocus(bHasFocus);
    m_bHasFocus = bHasFocus;

}
    
int CGuiToolBar::GetSelectorHeight()
{
    if(IsLandscape())
        return WIDTH(m_rcLocation) + GetSelectorDrawOverAmount();
    else
        return HEIGHT(m_rcLocation) + GetSelectorDrawOverAmount();
}

HRESULT CGuiToolBar::CaptureAnimationGDI()
{
    HRESULT hr = S_OK;

    // animated button
    m_gdiMovingBtn.Destroy();
    if(IsLandscape())
    {
        int iBtnWidth = WIDTH(m_rcLocation)+ GetSelectorDrawOverAmount();
        int iBtnHeight = m_gdiImgSelectorLan.GetHeight();
        if(m_gdiMovingBtn.GetDC() == NULL || 
            m_gdiMovingBtn.GetWidth() != iBtnWidth || m_gdiMovingBtn.GetHeight() != iBtnHeight)
            m_gdiMovingBtn.Create(m_gdiBg->GetDC(), iBtnWidth, iBtnHeight);
 
        ::Draw(m_gdiMovingBtn, 
            0,0,iBtnWidth, m_gdiImgSelectorLan.GetHeight(), 
            m_gdiImgSelectorLan,
            m_gdiImgSelectorLan.GetWidth() - iBtnWidth,0,
            ALPHA_Copy);


        int w = m_gdiMovingBtn.GetWidth();
        w -= (GetSelectorDrawOverAmount()+2);
        RECT rc = {0,0,w, m_gdiMovingBtn.GetHeight() - (IsVGA() ? 20 : 16)};//4};
        DrawText(m_gdiMovingBtn.GetDC(), m_szBtnText[m_eSelected], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, COLOR_TEXT_NORMAL);

        // button icon
        rc.top += (IsVGA() ? 20 : 12);
        DrawBtnIcon(m_gdiMovingBtn, rc, m_eSelected, TRUE);
    }
    else 
    {
        int iBtnWidth = m_gdiImgSelector.GetWidth();
        int iBtnHeight = HEIGHT(m_rcLocation) + GetSelectorDrawOverAmount();
        if(m_gdiMovingBtn.GetDC() == NULL || 
            m_gdiMovingBtn.GetWidth() != iBtnWidth || m_gdiMovingBtn.GetHeight() != iBtnHeight)
            m_gdiMovingBtn.Create(m_gdiBg->GetDC(), iBtnWidth, iBtnHeight);
            m_gdiMovingBtn.Create(m_gdiBg->GetDC(), iBtnWidth, iBtnHeight);
        ::Draw(m_gdiMovingBtn, 0,0,iBtnWidth, iBtnHeight, m_gdiImgSelector,0,0,ALPHA_Copy);
        RECT rc = {0,0,iBtnWidth, iBtnHeight - 4};
        DrawText(m_gdiMovingBtn.GetDC(), m_szBtnText[m_eSelected], rc, DT_CENTER | DT_BOTTOM, m_hFontBtnText, COLOR_TEXT_NORMAL);
       
        // button icon
        rc.top += (HEIGHT(rc)/6);
        DrawBtnIcon(m_gdiMovingBtn, rc, m_eSelected, TRUE);

    }
    return hr;
}

HRESULT CGuiToolBar::DoChangeGui(EnumCurrentGui eNew)
{
    // launch animation
    HRESULT hr = S_OK;
	
    if(GetCurSelection() != eNew)
        PostMessage(m_hWndParent, WM_CHANGE_Gui, WPARAM(eNew), 0);

    POINT ptFrom, ptTo;
    int iMoveDistance = 0;
    int iTotalDistance = 0;
    if(IsLandscape())
    {
        int iBtnHeight = HEIGHT(m_rcLocation)/4;
        int iBtnOffset = (m_gdiImgSelectorLan.GetHeight() - iBtnHeight) / 2;
        iTotalDistance = HEIGHT(m_rcLocation) - iBtnHeight;

        //ptFrom.y = (int)GetCurSelection() * iBtnHeight;
        ptFrom.y = m_iSelectorPos;
        ptFrom.y -= iBtnOffset;
        ptFrom.x = 0;//m_rcLocation.bottom - m_gdiImgSelector.GetHeight();

        ptTo = ptFrom;
        ptTo.y = (int)eNew * iBtnHeight;
        ptTo.y -= iBtnOffset;
        iMoveDistance = abs(ptTo.y - ptFrom.y);
    }
    else
    {
        int iBtnWidth = WIDTH(m_rcLocation)/4;
        int iBtnOffset = (m_gdiImgSelector.GetWidth() - iBtnWidth) / 2;
        iTotalDistance = WIDTH(m_rcLocation) - iBtnWidth;

        ptFrom.x = m_iSelectorPos;
        ptFrom.x -= iBtnOffset;
        ptFrom.y = m_rcLocation.top - GetSelectorDrawOverAmount();

        ptTo = ptFrom;
        ptTo.x = (int)eNew * iBtnWidth;
        ptTo.x -= iBtnOffset;
        iMoveDistance = abs(ptTo.x - ptFrom.x);
    }
    
    CaptureAnimationGDI();

    HDC hDC = ::GetDC(m_dlgParent->GetWnd());
    ToolbarAnimateFromTo(hDC,
        *m_gdiBg,
        m_gdiMovingBtn,
        ptFrom,
        ptTo,
        0,
        0,
        m_gdiMovingBtn.GetWidth(),
        m_gdiMovingBtn.GetHeight(),
        iMoveDistance,
        TOOLBAR_ANIMATE_TIME*iMoveDistance/iTotalDistance);
    ::ReleaseDC(m_dlgParent->GetWnd(), hDC);

    m_dwLockTicks = GetTickCount() + 300;

    // set selector position
    if(IsLandscape())
        m_iSelectorPos = m_rcToolbarBtns[m_eSelected].top;
    else
        m_iSelectorPos = m_rcToolbarBtns[m_eSelected].left;

    return hr;
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
    EnumCurrentGui eOld = m_eSelected;

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
	}
	else										// landscape
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
		else if(wParam == VK_DOWN)	
		{
		}
		else if(wParam == VK_UP)	
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
            rcInval.right = m_rcDrawBounds.right;
        else
            rcInval.top = m_rcDrawBounds.top;

        m_dlgParent->SetDirty(rcInval);
		InvalidateRect(m_hWndParent, &rcInval, FALSE);
    }

	return FALSE;
}
  	
void CGuiToolBar::LoseFocus()	
{ 
    if(!m_bHasFocus)
        return;

    m_bHasFocus = FALSE; 

    // invalidate the selected button; it has lost focus
    // if selected is the 'selector', inval the entire btn
    SetDirty(m_eSelected, m_eSelected);
    RECT rcInval = m_rcToolbarBtns[m_eSelected];
    if(GetCurSelection() == m_eSelected)
    {
        if(IsLandscape())
            rcInval.right += GetSelectorDrawOverAmount();
        else
            rcInval.top -= GetSelectorDrawOverAmount();
    }
    InvalidateRect(m_hWndParent, &rcInval, FALSE);
}


//////////////////////////////////////////////////////////////////////////////
// custom toolbar animation - from IssGDIFX
//
#ifndef PI
#define PI 3.1415926538
#endif

HRESULT ToolbarAnimateFromTo(HDC    hdcScreen,
                                   CIssGDIEx&	gdiBackground,
                                   CIssGDIEx&	gdiAniObject,
                                   POINT	ptFrom,
                                   POINT	ptTo,
                                   int		iXStart,
                                   int		iYStart,
                                   int		iWidth,
                                   int		iHeight,
                                   int		iDrawFrames,
                                   int      iTime   // in ms
                                   )
{
    HRESULT hr = S_OK;
    CIssGDIEx	gdiTemp;
    CBARG(hdcScreen, _T("hdcScreen"));
    CBARG(gdiBackground.GetDC(), _T("gdiBackground.GetDC()"));
    CBARG(gdiAniObject.GetDC(), _T("gdiAniObject.GetDC()"));
    CBARG(iWidth != 0, _T("iWidth != 0"));
    CBARG(iHeight != 0, _T("iHeight != 0"));
    CBARG(iDrawFrames != 0, _T("iDrawFrames != 0"));
    CBARG(gdiAniObject.GetHeight() >= iHeight, _T("gdiAniObject.GetHeight() >= iHeight"));
    CBARG(gdiAniObject.GetWidth() >= iWidth, _T("gdiAniObject.GetWidth()	>= iWidth"));

    float fXStep = (float)(ptTo.x - ptFrom.x)/(float)iDrawFrames;
    float fYStep = (float)(ptTo.y - ptFrom.y)/(float)iDrawFrames;

    int iXDraw		= ptFrom.x;
    int iYDraw		= ptFrom.y;
    int	iXOldDraw	= ptFrom.x;
    int iYOldDraw	= ptFrom.y;

    RECT rcDraw;
    rcDraw.left		= min(ptFrom.x, ptTo.x);
    rcDraw.top		= min(ptFrom.y, ptTo.y);
    rcDraw.right	= max(ptFrom.x, ptTo.x) + iWidth;
    rcDraw.bottom	= max(ptFrom.y, ptTo.y) + iHeight;

    hr = gdiTemp.Create(hdcScreen, rcDraw, FALSE, TRUE, TRUE);
    CHR(hr, _T("gdiTemp.Create"));

    // draw the full background to the temp GDI
    BitBlt(gdiTemp.GetDC(),
           0,0,
           rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
           gdiBackground.GetDC(),
           rcDraw.left, rcDraw.top,
           SRCCOPY);

    DWORD dwCurTime = GetTickCount();
    int i = GetCurrentFrame(dwCurTime, iDrawFrames, (double)iTime);

    const float fAngleStep = (float)PI/iDrawFrames;
    RECT rcScreen;
    while(i < iDrawFrames)
    {
        rcScreen.left	= min(iXOldDraw, iXDraw);
        rcScreen.top	= min(iYOldDraw, iYDraw);
        rcScreen.right	= max(iXOldDraw, iXDraw) + iWidth;
        rcScreen.bottom	= max(iYOldDraw, iYDraw) + iHeight;

        // Draw the old Background
        BitBlt(gdiTemp.GetDC(),
               iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
               iWidth, iHeight,
               gdiBackground.GetDC(),
               iXOldDraw, iYOldDraw,
               SRCCOPY);
        // draw moving image
        hr = Draw(gdiTemp, iXDraw - rcDraw.left, iYDraw - rcDraw.top, iWidth, iHeight, gdiAniObject);

        // draw it all to the screen
        BitBlt( hdcScreen,
                rcScreen.left,rcScreen.top,
                WIDTH(rcScreen), HEIGHT(rcScreen),
                gdiTemp.GetDC(),
                rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
                SRCCOPY);

        // update the values for next draw
        i = GetCurrentFrame(dwCurTime, iDrawFrames, (double)iTime);

        // give the movement some shape (accelerate-decelerate)
        int iCenterFrame = iDrawFrames / 2;
        float fAngle = (float)PI - (i * fAngleStep); 
        float fScaled = iCenterFrame + (iCenterFrame * (float)cos(fAngle));

        iXOldDraw	= iXDraw;
        iYOldDraw	= iYDraw;
        iXDraw		= (int)(/*i*/fScaled*fXStep) + ptFrom.x;
        iYDraw		= (int)(/*i*/fScaled*fYStep) + ptFrom.y;
        //DebugOut(_T("i = %d factored: %.2f"), i, fScaled);
    }

    if(iXDraw != ptTo.x || iYDraw != ptTo.y)
    {	//draw the final frame
        // update the values and draw again
        iXDraw		= ptTo.x;
        iYDraw		= ptTo.y;

        rcScreen.left	= min(iXOldDraw, iXDraw);
        rcScreen.top	= min(iYOldDraw, iYDraw);
        rcScreen.right	= max(iXOldDraw, iXDraw) + iWidth;
        rcScreen.bottom	= max(iYOldDraw, iYDraw) + iHeight;

        // Draw the old Background
        BitBlt(gdiTemp.GetDC(),
            iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
            iWidth, iHeight,
            gdiBackground.GetDC(),
            iXOldDraw, iYOldDraw,
            SRCCOPY);

        // draw moving image
        hr = Draw(gdiTemp, iXDraw - rcDraw.left, iYDraw - rcDraw.top, iWidth, iHeight, gdiAniObject);

        // draw it all to the screen
        BitBlt( hdcScreen,
            rcScreen.left,rcScreen.top,
            rcScreen.right - rcScreen.left, rcScreen.bottom - rcScreen.top,
            gdiTemp.GetDC(),
            rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
            SRCCOPY);

    }

Error:
    return hr;
}
