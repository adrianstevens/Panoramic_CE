#pragma once

#include "GuiBase.h"
#include "IssGDIEx.h"
#include "ContactsGuiDefines.h"

#define NUM_TOOLBAR_BTNS	4

enum EnumCurrentGui
{
    GUI_Unknown = -1,
	GUI_Favorites,
	GUI_Contacts,
	GUI_CallLog,
	GUI_Quit,
};

class CDlgContacts;
class CGuiToolBar:public CGuiBase
{
public:
	CGuiToolBar();
	~CGuiToolBar(void);

	BOOL 	            Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContacts* dlgParent);
    void                ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
	BOOL				Draw(HDC hDC, RECT rcClient, RECT& rcClip);
	BOOL				MoveGui(RECT rcLocation);

	BOOL				OnLButtonDown(POINT pt);
	BOOL				OnLButtonUp(POINT pt);
	BOOL 			    OnMouseMove(POINT& pt);
	BOOL				OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

    HRESULT             DrawSelector(CIssGDIEx& gdi, RECT& rc, RECT& rcClip, BOOL bDrawDestZero = TRUE);
    BOOL                IsDragging()        { return m_bHoldAndDrag; }
    static CGuiToolBar* GetToolbar();
    void                SetFocus(BOOL bHasFocus);
    virtual BOOL        HasFocus() { return m_bHasFocus; }
	virtual void	    LoseFocus();
    EnumCurrentGui      GetCurSelection();


private:
	HRESULT				DrawButtonLabels(CIssGDIEx& gdi);
    HRESULT             DrawToolbarBg(CIssGDIEx& gdi);
    HRESULT             DrawSelected(CIssGDIEx& gdi);
    HRESULT             DrawBtnIcon(CIssGDIEx& gdi, RECT& rc, int iBtnIndex, BOOL bExcludeGlow = FALSE);
    HRESULT             DoChangeGui(EnumCurrentGui eNew);
    HRESULT             CaptureAnimationGDI(/*BOOL bIncludeBg = TRUE*/);
    HRESULT             DrawAnimationBg(/*CIssGDIEx& gdi, RECT& rcClip*/);
    HRESULT             Render();
    void                SetDirty(int iStart, int iEnd);
    int                 GetSelectorDrawOverAmount();
    int                 GetDragThreshold();
    int                 GetSelectorHeight();

private:	// variables
	EnumCurrentGui		m_eSelected;		// selected item
	EnumCurrentGui		m_eLButtonDown;		// button down item
    DWORD               m_dwLockTicks;

	BOOL				m_bHasFocus;
	BOOL				m_bBtnDown;    
    BOOL                m_bHoldAndDrag;
    int                 m_iSelectorPos;     

    POINT               m_ptLastMousePos;      // for drawing drag button
    POINT               m_ptLButtonDown;      
    int                 m_iDragBtnOffset;

    // images
    CIssGDIEx           m_gdiMovingBtn;
    CIssGDIEx           m_gdiImgSelector;
    CIssGDIEx           m_gdiImgSelectorLan;
    CIssGDIEx           m_gdiImgIconArray;

	HFONT				m_hFontBtnText;

	RECT				m_rcToolbarBtns[NUM_TOOLBAR_BTNS];
	RECT				m_rcDrawBounds;
    TCHAR               m_szBtnText[NUM_TOOLBAR_BTNS][STRING_NORMAL];
    CDlgContacts*       m_dlgParent;

};
