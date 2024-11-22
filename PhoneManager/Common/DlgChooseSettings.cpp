#include "StdAfx.h"
#include "DlgChooseSettings.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"
#include "IssDebug.h"
#include "Shlobj.h"
#include <Cpl.h>
#include "IssLocalisation.h"

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

#ifndef CPL_IDNAME
#define CPL_IDNAME    100
#endif

typedef LONG (*CPLAPPLET) (HWND hwndCPl, UINT  msg, LPARAM  lParam1, LPARAM  lParam2);

#define IDMENU_SelectItem               5000

HIMAGELIST CDlgChooseSettings::m_hImageListCPL = NULL;

CDlgChooseSettings::CDlgChooseSettings(CIssVector<TypeLaunchItem>* arrExcludeItems /*= 0*/)
{
    m_arrExcludeList= arrExcludeItems;
}

CDlgChooseSettings::~CDlgChooseSettings(void)
{
    if(!m_bPreventDeleteList)
    {
       ResetListContent();

        if(m_hImageListCPL)
        {
            ImageList_Destroy(m_hImageListCPL);
            m_hImageListCPL = NULL;
        }
    }
}

void CDlgChooseSettings::MyCheckProgramsList()
{
    
}

void CDlgChooseSettings::DestroyAllContent()
{
    if(m_hImageListCPL)
    {
        ImageList_Destroy(m_hImageListCPL);
        m_hImageListCPL = NULL;
    }
    ResetListContent();
    m_dwFullListCount = 0;
}

BOOL CDlgChooseSettings::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    SHFILEINFO sfi = {0};
    m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

    if(m_bProgamsList)
    {
        ResetListContent();
        m_bProgamsList = FALSE;
    }

	if(!m_oMenu)
		return TRUE;

    // we only fully repopulate if we don't have the same amount of contacts as before or our current list is empty
    if(m_oMenu->GetItemCount() == 0)
        PopulateList();
    else
    {
        FilterList();

        DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList;
        if(m_oMenu->GetItemCount() > 20)
            dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

        // do other menu initialization here
        m_oMenu->SetCustomDrawFunc(CDlgChoosePrograms::DrawListItem, this);
        m_oMenu->SetDeleteItemFunc(DeleteMyItem);
        m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
        m_oMenu->SetSelectedItemIndex(0, TRUE);
    }

    ::SetCursor(hCursor);

    return TRUE;
}

void CDlgChooseSettings::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, m_oStr->GetText(ID(IDS_SELECT_Setting), m_hInst), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}


void CDlgChooseSettings::PopulateList()
{
    m_oStr->Empty(m_szSearch);

	if(!m_oMenu)
		return;

    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu->SetCustomDrawFunc(CDlgChoosePrograms::DrawListItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->PreloadImages(m_hWnd, m_hInst);

    // find all the settings
    if(m_hImageListCPL)
    {
        ImageList_Destroy(m_hImageListCPL);
        m_hImageListCPL = NULL;
    }
    m_hImageListCPL = ImageList_Create(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), ILC_COLOR|ILC_MASK, 0, 100);
    SearchCPLItems();


    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;
    if(m_oMenu->GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_Bounce);

    // do other menu initialization here
    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
    m_oMenu->SortList(CompareLinks);
    m_oMenu->SetItemHeights(GetSystemMetrics(SM_CXICON) + INDENT, GetSystemMetrics(SM_CXICON) + INDENT);
    m_oMenu->SetSelectedItemIndex(0, TRUE);

}

void CDlgChooseSettings::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, BOOL bIsHighlighted, TypeLaunchItem* sItem)
{
    ImageList_Draw(m_hImageListCPL, sItem->iIconIndex, gdi, rcDraw.left + INDENT/2, rcDraw.top + INDENT/2, ILD_TRANSPARENT);

    rcDraw.left += (GetSystemMetrics(SM_CXICON) + INDENT);

    DrawTextShadow(gdi, sItem->szFriendlyName, rcDraw, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

HRESULT CDlgChooseSettings::SearchCPLItems()
{
    //Go through the \\windows directory looking for *.cpl files.
    WIN32_FIND_DATA wfd;
    TypeLaunchItem* sItem = NULL;
    HRESULT hr = S_OK;

    TCHAR szSearch[MAX_PATH];

    m_oStr->StringCopy(szSearch, _T("\\windows\\*.cpl"));

    HANDLE hFind = FindFirstFile(szSearch, &wfd);
    CBARG(hFind != INVALID_HANDLE_VALUE, _T(""));
	CPHR(m_oMenu, _T(""));

    TCHAR szParam[MAX_PATH] = _T("");						//L"cplmain.cpl,";
    TCHAR szName[MAX_PATH]	= _T("");
    TCHAR szRegName[MAX_PATH] = _T("");

    do
    {
        //Got a control panel applet.
        HINSTANCE hCPL = LoadLibrary(wfd.cFileName);
        if(!hCPL)
            continue;

        CPLAPPLET pCPLApplet = (CPLAPPLET)GetProcAddress(hCPL, _T("CPlApplet"));
        if(pCPLApplet)
        {
            pCPLApplet(m_hWnd, CPL_INIT, 0, 0);

            LONG lNumApplets = pCPLApplet(m_hWnd, CPL_GETCOUNT, 0, 0);
            for(int i=0; i<lNumApplets; i++)
            {
                //Find the unique name for this CPL applet.
                pCPLApplet(m_hWnd, CPL_IDNAME, i, (LPARAM)&szRegName);

                //Find the name of this CPL applet.
                NEWCPLINFO CplInfo = {0};
                CplInfo.dwSize = sizeof(NEWCPLINFO);

                long lResult = pCPLApplet(m_hWnd, CPL_NEWINQUIRE, i, (LPARAM)&CplInfo);
                if(CplInfo.szName && CplInfo.hIcon)
                {
                    m_oStr->Format(szName, _T(",%d"), i);
                    m_oStr->StringCopy(szParam, wfd.cFileName);
                    m_oStr->Concatenate(szParam, szName);

                    sItem = new TypeLaunchItem;
                    CPHR(sItem, _T("sItem is NULL"));
                    sItem->szFriendlyName = m_oStr->CreateAndCopy(CplInfo.szName);
                    CPHR(sItem->szFriendlyName, _T("sItem->szFriendlyName is NULL"));
                    sItem->szPath = m_oStr->CreateAndCopy(szParam);
                    CPHR(sItem->szPath, _T("sItem->szPath is NULL"));
                    sItem->eType = LT_Setting;
                    sItem->iIconIndex = ImageList_ReplaceIcon(m_hImageListCPL, -1, CplInfo.hIcon);

                    if(IsExcludedItem(sItem))
                    {
                        delete sItem;
                        sItem = NULL;
                        continue;
                    }
                    m_oMenu->AddItem(sItem, IDMENU_SelectItem);
                    sItem = NULL;
                }
            }

            pCPLApplet(m_hWnd, CPL_EXIT, 0, 0);
        }

        FreeLibrary(hCPL);

    } while(FindNextFile(hFind, &wfd));

Error:
    if(hFind != INVALID_HANDLE_VALUE)
        FindClose(hFind);
    if(sItem && hr != S_OK)
        delete sItem;
    return hr;
}
