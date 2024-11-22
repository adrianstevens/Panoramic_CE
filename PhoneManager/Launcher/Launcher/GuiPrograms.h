#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "IssKineticList.h"
#include "IssImgFont.h"
#include "DlgOptions.h"
#include "IssGDIEx.h"
#include "ObjMRUList.h"

enum EnumLink
{
    LINK_Normal,
    LINK_Directory,
    LINK_MRU,
};

struct TypeLink
{
	TCHAR*	szName;				// friendly name of the app
	TCHAR*	szPath;				// path of the found .lnk file
	int		iIconIndex;			// global icon index that is used
	EnumLink eLinkType;			// what type of link is this
	TypeLink();
	~TypeLink();
};


class CGuiPrograms: public CGuiBase, CIssKineticList
{
public:
	CGuiPrograms(void);
	~CGuiPrograms(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions);
	void            ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    BOOL			SetPosition(RECT& rc);

	BOOL			Draw(HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);

	void			RefreshLinks();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);

protected:


private:
	void			PopulateList(TCHAR* szPath, BOOL bAddDirectories = TRUE);
	void			EraseList();
    CIssImageSliced& GetBorder();
    CIssGDIEx&      GetBgGDI();
    void            DrawSearch(CIssGDIEx& gdi, RECT& rcDraw);
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeLink* sLink, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static int      CompareLinks( const void *arg1, const void *arg2 );

    void            DrawMRUList(CIssGDIEx& gdi, RECT& rcDraw, TypeLink* sLink, BOOL bIsHighlighted);
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem);
	BOOL			DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip, BOOL bAnimate = FALSE);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void            AnimateDirChange(BOOL bAnimateRight);
    int             GetNextPageItemIndex(BOOL bSearchUp = FALSE);
    void            GrabItem(POINT& pt, TypeItems* sItem);
    BOOL            HasFocus();
    BOOL            IsLinksInDir(TCHAR* szDir);

    static void     DeleteMyItem(LPVOID lpItem);

    void            LoseFocus(){KillAllTimers();};

    void            ScrollTo(TCHAR* szText);
	
private:
    CObjMRUList                 m_oMRU;

	HIMAGELIST					m_hImageList;
	CIssGDIEx					m_gdiBackground;
    CIssGDIEx                   m_gdiForwardArrow;
    CIssGDIEx                   m_gdiBackArrow;
    CIssImageSliced             m_imgBorder;      
    TCHAR                       m_szDir[STRING_MAX];
	TCHAR                       m_szStartDir[STRING_MAX];


	HFONT						m_hFontLetters;
	//HFONT						m_hFontContact;
	//HFONT						m_hFontContactBold;
	POINT						m_ptScreenSize;
	POINT						m_ptMouseDown;

	int                         m_iNumMRU;
    int                         m_iCurMRU;

	RECT						m_rcContactList;

    BOOL                        m_bCompactView;

    //search variables
    HFONT                       m_hFontSearch;
    TCHAR                       m_szSearch[STRING_SMALL+1];//that's plenty
    BOOL                        m_bShowSearch;
};
