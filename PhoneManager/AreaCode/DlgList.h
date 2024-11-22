#pragma once
#include "dlgbase.h"
#include "IssKineticList.h"
#include "IssAreaCode.h"
#include "IssImgFont.h"

struct MenuType
{
    TCHAR* szText;
    int iImage;
};

class CDlgList : public CDlgBase
{
public:
    CDlgList(HWND hWndMainApp, CIssAreaCode* oArea, EnumLookupType eLookup);
    ~CDlgList(void);

    static void             ResetListContent();

    void                    SetPreventDeleteList(BOOL bPreventDelete){m_bPreventDeleteList = bPreventDelete;};

    int                     GetSelected();
    TypeItems*              GetSelectedEntry(){return m_oMenu->GetSelectedItem();};

protected:
    BOOL 			        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                    OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                    OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                    OnMouseMove(HWND hWnd, POINT& pt);
    BOOL			        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                    OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    void                    OnMenuLeft();
    void                    OnMenuRight();
    void                    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                    DrawSearch(CIssGDIEx& gdi, RECT& rcDraw);
    BOOL                    DrawAlphabet(CIssGDIEx& gdi, RECT& rcDraw);
    BOOL                    DrawAlphabetPopUp(POINT pt);



    BOOL                    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                    ScrollTo(TCHAR szChar);
    void                    ScrollTo(TCHAR* szText);

    void                    PopulateList();

    void                    LoadImages();
    void                    LoadFlag(int iIndex);

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteListItem(LPVOID lpItem);

private:
    CIssAreaCode*           m_oArea;
    static CIssKineticList* m_oMenu;

    CIssGDIEx               m_gdiAphaBg;
    CIssGDIEx		        m_gdiAlphaPopUp;
    CIssGDIEx               m_gdiTemp;
    CIssGDIEx               m_gdiFlag;

    CIssImgFont				m_hImgFontAlpha;

    RECT					m_rcAlpha;
    RECT					m_rcAlphaText;

    int						m_iAlphaWidth;
    int						m_iAlphaTextSpacing;

    HWND                    m_hWndParent;

    static DWORD            m_dwFullListCount;

    HFONT                   m_hFontText;
    TCHAR                   m_szSearch[STRING_SMALL+1];//that's plenty
    BOOL                    m_bShowSearch;
    BOOL                    m_bScrollAlpha;

    static BOOL             m_bPreventDeleteList;

    EnumLookupType          m_eLookupType;

};
