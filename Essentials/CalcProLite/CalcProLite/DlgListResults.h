#pragma once
#include "isswnd.h"
#include "IssVector.h"

struct AddType
{
    TCHAR* sz1;
    TCHAR* sz2;
    TCHAR* sz3;
};

class CDlgListResults :
    public CIssWnd
{
public:
    CDlgListResults(void);
    ~CDlgListResults(void);

    BOOL            OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL            AddItem(TCHAR* szTitle, TCHAR* szValue);

private:
    BOOL            InitControls();
    void            DeleteVector();

    void            AddItemsToList();


private:
    CIssVector<AddType>     m_arrAdditions;
    HWND            m_hWndList;

    HFONT           m_hFont;
};
