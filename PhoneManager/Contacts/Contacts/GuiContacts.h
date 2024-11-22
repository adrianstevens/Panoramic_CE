#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "IssKineticList.h"
#include "DlgContactDetails.h"
#include "IssImgFont.h"
#include "DlgOptions.h"
#include "ObjContactImgLoader.h"

struct TypeListItem
{
    long lOid;
    TCHAR* szTitle;
    TCHAR* szSecondary;
    TypeListItem();
    ~TypeListItem();
};

class CGuiContacts: public CGuiBase, CIssKineticList
{
public:
	CGuiContacts(void);
	~CGuiContacts(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails);
	void            ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    BOOL			SetPosition(RECT& rc);

	BOOL			Draw(CIssGDIEx& gdiMem, HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);

	void			RefreshContacts();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            CheckItemCategoryNeeded(IContact* pContact);

protected:
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);
    void            DrawSearch(CIssGDIEx& gdi, RECT& rcDraw);

private:
	void			PopulateList();
	void			EraseList();
    int             FindContact(long lOid);
    BOOL            CategoryExists(TCHAR* szText);
    BOOL            ApplyContactChanges(TypeContact* pSrc, TypeContact* pDst);
    HRESULT         PoomItemAdded(long lOid);
    HRESULT         PoomItemChanged(long lOid);
    HRESULT         PoomItemDeleted(long lOid);

    CIssGDIEx&      GetAlphaPopupGDI();
    CIssGDIEx&      GetBgGDI();
    CIssImgFont&    GetAlphaFont();
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
	static void		DeleteContactItem(LPVOID lpItem);

	BOOL			DrawAlphaBg(HDC hdc, RECT& rcClient, RECT& rcClip);
	BOOL			DrawAlphabetPopUp(POINT pt);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void			ScrollTo(TCHAR szChar, BOOL bLettersOnly = FALSE);
    void            ScrollTo(TCHAR* szText);

	static int		CompareContacts( const void *arg1, const void *arg2 );
    static void     GetCompareString(TCHAR* pszCompare, long& lOid, int iCompareCount);

    virtual BOOL    SelecteItem(int iIndex);
	
    virtual BOOL    HasFocus();
    void            GetPrimaryPhoneNumber(TCHAR* szDestination, IContact* pContact);

    void            LoseFocus(){KillAllTimers();};
	
private:
    CDlgContactDetails* m_dlgContactDetails;
	CPoomContacts*				m_pPoom;

	CIssGDIEx					m_gdiAphaBg;
	CIssGDIEx					m_gdiAlphaPopUp;
	HFONT						m_hFontLetters;
	HFONT						m_hFontContact;
	HFONT						m_hFontContactBold;
	CIssImgFont					m_hImgFontAlpha;
    CObjContactImgLoader        m_imgContact;

    TCHAR                       m_szSearch[STRING_SMALL+1];//that's plenty

	POINT						m_ptScreenSize;
	POINT						m_ptMouseDown;

	int							m_iTitleHeight;
	int							m_iLetterHeight;
	int							m_iContactHeight;
    int                         m_iContactHeightSelected;
    int                         m_iKeyRepeatCount;

	int							m_iAlphaWidth;
	int							m_iAlphaTextSpacing;

    //hack for fade in
    BOOL                        m_bFadeIn;
    BOOL                        m_bShowSearch;

	RECT						m_rcAlpha;
	RECT						m_rcAlphaText;
    RECT						m_rcContactList;
	BOOL						m_bScrollAlpha;
};
