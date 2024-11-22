#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "DlgOptions.h"
#include "IssGDIEx.h"

enum EnumCPLGroups
{
    GROUP_Personal = 0,
    GROUP_Settings,
    GROUP_Connection,
    GROUP_Unknown,
};

struct TypeCPL
{
	TCHAR*	        szName;				// friendly name of the app
	EnumCPLGroups   eGroup;             // group it belongs in
    TCHAR*          szLaunch;           // launch string
    int             iIconIndex;
	TypeCPL();
	~TypeCPL();
};


class CGuiSettings: public CGuiBase, CIssKineticList
{
public:
	CGuiSettings(void);
	~CGuiSettings(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst);
	void            ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    BOOL			SetPosition(RECT& rc);

	BOOL			Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);

	void			RefreshCPL();	        // called if list has changed
	BOOL 	        AddMenuItems();

protected:


private:
	HRESULT			PopulateList(CIssVector<TypeCPL>& arrCPL);
	void			EraseList();
    void            DrawSearch(CIssGDIEx& gdi, RECT& rcDraw);
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeCPL* sLink, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static int      CompareCPLNoSections( const void *arg1, const void *arg2);
    static int      CompareCPL( const void *arg1, const void *arg2);

    BOOL            HasFocus();
    EnumCPLGroups   GetCPLGroup(TCHAR* szRegName);

    static void     DeleteMyItem(LPVOID lpItem);

    void            LoseFocus(){KillAllTimers();};

    void            ScrollTo(TCHAR* szText);
	
private:
    HIMAGELIST                  m_hImageList;

	RECT						m_rcContactList;

    TCHAR                       m_szSearch[STRING_SMALL+1];//that's plenty
    BOOL                        m_bShowSearch;
};
