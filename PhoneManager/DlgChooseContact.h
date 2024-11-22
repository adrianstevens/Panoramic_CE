#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "ObjItemContact.h"
#include "DlgBase.h"


class CDlgChooseContact : public CDlgBase
{
public:
	CDlgChooseContact(HWND hWndMainApp, CIssVector<class CObjItemContact>* arrExcludeItems = 0);
	~CDlgChooseContact(void);

    int				GetSelectedOID()	{ return m_iContactOid;	}
    void			FindOIDFromSelection();
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

private:		// functions
	void			LoadContacts();
    void            FilterContacts();
    BOOL            IsExcludedItem(long lOid);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            ScrollTo(TCHAR szChar);

private:		// variables
    CIssVector<CObjItemContact>* m_arrExcludeList;
	long			        m_iContactOid;
    static CIssKineticList  m_oMenu;
    static BOOL             m_bPreventDeleteList;           // Should we not delete the list for faster loading
    static DWORD            m_dwFullListCount;
    static DWORD            m_dwLastPopulate;

};
