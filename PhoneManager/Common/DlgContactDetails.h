#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "DlgBase.h"
#include "PoomContacts.h"
#include "IssImageSliced.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "IssWndTouchMenu.h"

class CDlgContactDetails : public CDlgBase
{
public:
	CDlgContactDetails(BOOL bLastNameFirst = TRUE);
	~CDlgContactDetails(void);

	BOOL				SetIndex(int iIndex);
    void                SetLastNameFirst(BOOL bLastNameFirst){m_bLastNameFirst = bLastNameFirst;};
    void                SetAllowConvHistory(BOOL bAllowConvHistory){m_bAllowConvHistory = bAllowConvHistory;};
    void                PreloadImages(HWND hWnd, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssWndTouchMenu* wndMenu);
    void                ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInst);
    void                CloseWindow(int iReturn = IDOK);

    BOOL                Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate);

protected:
	BOOL				OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL				OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL				OnMouseMove(HWND hWnd, POINT& pt);
	BOOL				OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL                OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL				DrawContactPic(CIssGDIEx& gdi, RECT rcClient, RECT rcClip);
	void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumContactInfo* eInfo);
    void                OnMenuLeft();
    void                OnMenuRight();
	void				PerformOperation();
	void				MakeCall(TCHAR* szNumber);
	void				OpenHistory();
	BOOL				IsContactValid(void);
	BOOL				UpdateContact(void);
    void                UpdateMenu();
    void                ShowMenu();
    void                FadeInScreen();
    void                InitBackground(HWND hWnd, HINSTANCE hInst);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteMyItem(LPVOID lpItem);
    void                MoreInfo();
    void                EditContact();
    void                SetContactPicture();
    void                SetContactCamera();
    void                ContactRingtone();

private:
	CIssGDIEx*				m_gdiMem;
    CIssWndTouchMenu*       m_wndMenu;
	CIssGDIEx				m_gdiPicture;           // the image
	CIssImageSliced			m_imgButton;            // kinetic list button
    CIssImageSliced			m_imgButtonSel;         // kinetic list selected button
    CIssImageSliced         m_imgMask;              // picture's mask
    CIssImageSliced         m_imgBorder;            // picture's border
    CIssGDIEx               m_gdiBackArrow;         // the Back arrow
    CIssGDIEx               m_gdiReflection;        // image reflection
    CIssGDIEx               m_gdiDefaultPic;        // our default picture
    CIssKineticList         m_oMenu;

	HFONT				m_hFontText;
	HFONT				m_hFontBtnText;

	RECT				m_rcPicture;
	RECT				m_rcLastUsed;
	RECT				m_rcBackBtn;
    RECT                m_rcMenu;

	BOOL				m_bFadeIn;
    BOOL                m_bAnimate;//so we can enable/disable transitions 
    BOOL                m_bLastNameFirst;
    BOOL                m_bAllowConvHistory;        // should we allow user to select conversation history
	TypeContact			m_sContact;
	int					m_iIndex;
};
