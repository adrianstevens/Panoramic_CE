#pragma once

#include "DlgBase.h"
#include "IssWndTouchMenu.h"
#include "IssKineticList.h"
#include "PoomContacts.h"

#define IDMENU_Selection        5001
#define IDMENU_PopupMenu        5002

class CDlgPerContactBase:public CDlgBase
{
public:
    CDlgPerContactBase(void);
    ~CDlgPerContactBase(void);

	static void			DeleteAllContent();
    BOOL				SetIndex(int iIndex){m_iContactIndex = iIndex;return TRUE;};
    void                SetNameOverride(TCHAR* szName);
    static void         PreloadImages(HWND hWnd, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssWndTouchMenu* wndMenu);
    static void         ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInst);
    virtual void        CloseWindow(int iReturn = IDOK);
    BOOL                Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate);

protected:
    virtual BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL		OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
    virtual BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
    virtual BOOL		OnMouseMove(HWND hWnd, POINT& pt);
    virtual BOOL		OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL                OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    virtual void        DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL				DrawContactPic(CIssGDIEx& gdi, RECT rcClient, RECT rcClip);
    void                DrawContactBg(CIssGDIEx& gdi, RECT rcClient, RECT rcClip);
    void                DrawContactText(CIssGDIEx& gdi, RECT rcClient, RECT rcClip);
    void                OnMenuLeft();
    void                OnMenuRight();
    void                UpdateContact();

    virtual void        PopulateList(){};
    virtual void        OnSelectPopupMenuItem(int iIndex, TypeItems* sItem){};
    virtual void        OnSelectMenuItem(int iIndex, TypeItems* sItem){};
    virtual void        OnMenu(){};
    virtual void        DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw){};

    void                EditContact();


protected:    // variables
    CIssGDIEx				m_gdiPicture;           // the image
    static CIssGDIEx*		m_gdiMem;
    static CIssWndTouchMenu*       m_wndMenu;
    static CIssImageSliced*  m_imgMask;              // picture's mask
    static CIssImageSliced*  m_imgBorder;            // picture's border
    static CIssImageSliced*  m_imgBg;                // background
    static CIssGDIEx*        m_gdiDefaultPic;        // our default picture
    static CIssGDIEx*        m_gdiBackArrow;         // the Back arrow
    static CIssKineticList*  m_oMenu;

    HFONT                   m_hFontTimeDate;

    TCHAR*                  m_szNameOverride;       // custom Name

    RECT                    m_rcPicture;
    RECT                    m_rcConctactArea;       // area to draw contact info

    TypeContact             m_sContact;
      

    int                     m_iContactIndex;        // index to find the contact

    BOOL                    m_bAnimate;

};
