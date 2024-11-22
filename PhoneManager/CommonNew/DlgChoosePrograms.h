#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "DlgBase.h"

enum EnumLaunchType
{
    LT_Program = 0,
    LT_Setting,
    LT_FileOrFolder,
};

struct TypeLaunchItem
{
    EnumLaunchType eType;
    TCHAR*  szFriendlyName;
    TCHAR*  szPath;
    int     iIconIndex;
    POINT   ptLocation;
    CIssGDIEx gdiImage;
    TypeLaunchItem();
    ~TypeLaunchItem();
    void    Destroy();
    void    Clone(TypeLaunchItem* sItem);
    void    Launch(HWND hWnd);
};

class CDlgChoosePrograms : public CDlgBase
{
public:
	CDlgChoosePrograms(CIssVector<TypeLaunchItem>* arrExcludeItems = 0);
	~CDlgChoosePrograms(void);

    void            SetPreventDeleteList(BOOL bPreventDelete){m_bPreventDeleteList = bPreventDelete;};
    static DWORD    GetListTickCount(){return m_dwLastPopulate;};
    static DWORD    GetListCount(){return m_dwFullListCount;};
    static void     ResetListContent(){if(m_oMenu)m_oMenu->DestroyList();m_dwFullListCount = 0;};
	static void		DeleteAllContent();
    TCHAR*          GetPath();
    TCHAR*          GetFriendlyName();
    void            AddTodayIcon(BOOL bAdd){m_bAddTodayIcon = bAdd;};

    BOOL            OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL            DoModal(HWND hWndParent, HINSTANCE hInst, UINT uiDlgID);
      
protected:
	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnMouseMove(HWND hWnd, POINT& pt);
    BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:		// functions
	virtual void    PopulateList();
    void            FilterList();
    BOOL            IsExcludedItem(TypeLaunchItem* sItem);
    void            OnMenuLeft();
    void            OnMenuRight();
    virtual void    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            ScrollTo(TCHAR szChar);
    void            ScrollTo(TCHAR* szText);
    void            SearchItems(TCHAR* szPath);
    virtual void    DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, BOOL bIsHighlighted, TypeLaunchItem* sItem);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMyItem(LPVOID lpItem);
    static int      CompareLinks( const void *arg1, const void *arg2 );
    virtual void    MyCheckProgramsList();

    void            DrawSearch(CIssGDIEx& gdi, RECT& rcDraw);
    

protected:		// variables
    CIssVector<TypeLaunchItem>* m_arrExcludeList;
    static CIssKineticList*  m_oMenu;
    static BOOL             m_bPreventDeleteList;           // Should we not delete the list for faster loading
    static DWORD            m_dwFullListCount;
    static DWORD            m_dwLastPopulate;
    static BOOL             m_bProgamsList;
    HIMAGELIST              m_hImageList;
    BOOL                    m_bAddTodayIcon;

    HWND                    m_hWndParent;

    //search variables
    TCHAR                       m_szSearch[STRING_SMALL+1];//that's plenty
    BOOL                        m_bShowSearch;
};
