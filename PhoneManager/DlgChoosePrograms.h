#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "DlgBase.h"

enum EnumLaunchType
{
    LT_Program,
    LT_Setting,
};

struct TypeLaunchItem
{
    EnumLaunchType eType;
    TCHAR*  szFriendlyName;
    TCHAR*  szPath;
    int     iIconIndex;
    POINT   ptLocation;
    TypeLaunchItem();
    ~TypeLaunchItem();
    void    Destroy();
    void    Clone(TypeLaunchItem* sItem);
};

class CDlgChoosePrograms : public CDlgBase
{
public:
	CDlgChoosePrograms(CIssVector<TypeLaunchItem>* arrExcludeItems = 0);
	~CDlgChoosePrograms(void);

    void            SetPreventDeleteList(BOOL bPreventDelete){m_bPreventDeleteList = bPreventDelete;};
    static DWORD    GetListTickCount(){return m_dwLastPopulate;};
    static DWORD    GetListCount(){return m_dwFullListCount;};
    static void     ResetListContent(){m_oMenu.DestroyList();m_dwFullListCount = 0;};

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
    void            SearchItems(TCHAR* szPath);
    virtual void    DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, BOOL bIsHighlighted, TypeLaunchItem* sItem);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMyItem(LPVOID lpItem);
    static int      CompareLinks( const void *arg1, const void *arg2 );
    

protected:		// variables
    CIssVector<TypeLaunchItem>* m_arrExcludeList;
    static CIssKineticList  m_oMenu;
    static BOOL             m_bPreventDeleteList;           // Should we not delete the list for faster loading
    static DWORD            m_dwFullListCount;
    static DWORD            m_dwLastPopulate;
    HIMAGELIST              m_hImageList;
};
