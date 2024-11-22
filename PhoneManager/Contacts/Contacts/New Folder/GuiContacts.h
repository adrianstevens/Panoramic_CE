#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "IssKineticList.h"
#include "DlgContactDetails.h"
#include "IssImgFont.h"
#include "DlgOptions.h"

class CGuiContacts: public CGuiBase, CIssKineticList
{
public:
	CGuiContacts(void);
	~CGuiContacts(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails);
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

	void			RefreshContacts();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            CheckItemCategoryNeeded(TypeContact* sContact);

protected:
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);

private:
	void			PopulateList();
	void			EraseList();
    TypeContact*    FindContact(long lOid, int& iIndex);
    BOOL            CategoryExists(TCHAR* szText);
    BOOL            ApplyContactChanges(TypeContact* pSrc, TypeContact* pDst);
    HRESULT         PoomItemAdded(long lOid);
    HRESULT         PoomItemChanged(long lOid);
    HRESULT         PoomItemDeleted(long lOid);

    CIssImageSliced& GetBorder();
    CIssGDIEx&      GetAlphaPopupGDI();
    CIssGDIEx&      GetBgGDI();
    CIssImgFont&    GetAlphaFont();
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
	static void		DeleteContactItem(LPVOID lpItem);

	BOOL			DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip);
	BOOL			DrawAlphabetPopUp(POINT pt);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void			ScrollTo(TCHAR szChar);

	static int		CompareContacts( const void *arg1, const void *arg2 );
    static void     GetCompareString(TCHAR* pszCompare, TypeContact* sContact, int iCompareCount);

    virtual BOOL    SelecteItem(int iIndex);
	
    virtual BOOL    HasFocus();
    TCHAR*          GetPrimaryPhoneNumber(TypeContact* sContact);
	
private:
    CDlgContactDetails* m_dlgContactDetails;

	CIssGDIEx					m_gdiBackground;
	CIssGDIEx					m_gdiAlphaPopUp;

    CIssImageSliced             m_imgBorder;              
	
	HFONT						m_hFontLetters;
	HFONT						m_hFontContact;
	HFONT						m_hFontContactBold;
	CIssImgFont					m_hImgFontAlpha;

	POINT						m_ptScreenSize;
	POINT						m_ptMouseDown;

	int							m_iTitleHeight;
	int							m_iLetterHeight;
	int							m_iContactHeight;
    int                         m_iContactHeightSelected;

	int							m_iAlphaWidth;
	int							m_iAlphaTextSpacing;

	RECT						m_rcAlpha;
	RECT						m_rcAlphaText;
	RECT						m_rcContactList;
	BOOL						m_bScrollAlpha;
};
