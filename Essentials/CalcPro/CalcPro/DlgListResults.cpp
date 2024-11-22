#include "DlgListResults.h"
#include "stdafx.h"
#include "resource.h"
#include "IssGDIEx.h"

#ifndef UNDER_CE
#include "commctrl.h"
#endif

CDlgListResults::CDlgListResults(void)
:m_hWndList(NULL)
,m_hFont(NULL)
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_NORMAL, TRUE);
}

CDlgListResults::~CDlgListResults(void)
{
    if(m_hWndList)
    {
        ListView_DeleteAllItems(m_hWndList);
        m_hWndList = NULL;
    }

    CIssGDIEx::DeleteFont(m_hFont);
    DeleteVector();
}

void CDlgListResults::DeleteVector()
{
    AddType* sAdd = NULL;

    for(int i = 0; i < m_arrAdditions.GetSize(); i++)
    {
        sAdd = m_arrAdditions[i];

        if(sAdd != NULL)
        {
            m_oStr->Delete(&sAdd->sz1);
            m_oStr->Delete(&sAdd->sz2);

            delete sAdd;
            sAdd = NULL;
        }
    }

    m_arrAdditions.RemoveAll();
}

BOOL CDlgListResults::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= hWnd;
    mbi.nToolBarId	= IDR_MENU_WSheet;
    mbi.hInstRes	= m_hInst;
    if(!SHCreateMenuBar(&mbi))
    {
    }
#endif
    InitControls();
    AddItemsToList();
    ShowWindow(m_hWndList, SW_SHOW);
       


    return TRUE;
}

BOOL CDlgListResults::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    if(m_hWndList)
    {
        MoveWindow(m_hWndList, 
            0,0,
            rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
            FALSE);

        if(GetWindowLong(m_hWndList, GWL_STYLE) & WS_VSCROLL)
        {
            ListView_SetColumnWidth(m_hWndList, 0, (rcClient.right - rcClient.left- GetSystemMetrics(SM_CXVSCROLL))*4/9);
            ListView_SetColumnWidth(m_hWndList, 1, (rcClient.right - rcClient.left- GetSystemMetrics(SM_CXVSCROLL))*5/9);
        }
        else
        {
            ListView_SetColumnWidth(m_hWndList, 0, (rcClient.right - rcClient.left)*2/5);
            ListView_SetColumnWidth(m_hWndList, 1, (rcClient.right - rcClient.left)*3/5);
        }
    }		

    return TRUE;
}

BOOL CDlgListResults::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDMENU_Back)
        SafeCloseWindow(0);

    return TRUE;
}

BOOL CDlgListResults::InitControls()
{
    //make the list control
    if(m_hWndList == NULL)
    {
        DWORD dwStyle	= WS_CHILD | WS_VISIBLE | WS_TABSTOP |LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | LVS_REPORT;
#ifdef UNDER_CE
        DWORD dwExStyle = LVS_EX_NOHSCROLL;
#else
        DWORD dwExStyle = 0;
#endif


        m_hWndList = CreateWindowEx(dwExStyle, WC_LISTVIEW, NULL,
            dwStyle,
            0, 0, 0, 0, m_hWnd, NULL, 
            m_hInst, NULL);
      
        //
        //  We only need to create one column
        //
        LV_COLUMN LVColumn = {0};

        LVColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

        LVColumn.fmt		= LVCFMT_LEFT;
        LVColumn.cx			= 240;		// we'll change this later
        LVColumn.iSubItem	= 0;
        ListView_InsertColumn(m_hWndList, 0, &LVColumn);
        ListView_InsertColumn(m_hWndList, 1, &LVColumn);

        //set the font
        SendMessage(m_hWndList, WM_SETFONT, (WPARAM)m_hFont, NULL);
    }
    return TRUE;
}

BOOL CDlgListResults::AddItem(TCHAR* szTitle, TCHAR* szValue)
{
    AddType* sAdd = new AddType;

    sAdd->sz1 = m_oStr->CreateAndCopy(szTitle);
    sAdd->sz2 = m_oStr->CreateAndCopy(szValue);

    m_arrAdditions.AddElement(sAdd);

    return TRUE;
}

void CDlgListResults::AddItemsToList()
{
    for(int i = 0; i < m_arrAdditions.GetSize(); i++)
    {
        AddType* sAdd = m_arrAdditions[i];
        if(!sAdd)
            continue;

        LVITEM lvItem	= {0};
        lvItem.mask		= LVIF_TEXT;
        lvItem.pszText	= sAdd->sz1;
        lvItem.iItem	= ListView_GetItemCount(m_hWndList);
        lvItem.iItem    = ListView_InsertItem(m_hWndList, &lvItem);

        lvItem.mask		= LVIF_TEXT;
        lvItem.iSubItem	= 1;
        lvItem.pszText	= sAdd->sz2;
        ListView_SetItem(m_hWndList, &lvItem);
    }


}
