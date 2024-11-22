#include "StdAfx.h"
#include "DlgChooseCategory.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "ObjSkinEngine.h"

#define IDMENU_Selection        5000

CDlgChooseCategory::CDlgChooseCategory()
{
	m_oStr->Empty(m_szCategory);
}

CDlgChooseCategory::~CDlgChooseCategory(void)
{
}

BOOL CDlgChooseCategory::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LoadCategories();
	return TRUE;
}

BOOL CDlgChooseCategory::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    m_oMenu.OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}


BOOL CDlgChooseCategory::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Selection:
		OnMenuLeft();
		break;
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgChooseCategory::GetCategoryFromSelection()
{
    if(m_oMenu.GetItemCount() == 0)
        return FALSE;

	TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || m_oStr->IsEmpty(sItem->szText))
        return FALSE;

	m_oStr->StringCopy(m_szCategory, sItem->szText);
    return TRUE;
}

void CDlgChooseCategory::LoadCategories()
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

	CPoomContacts*  pOPoom = CPoomContacts::Instance();

	// if we want to save selected, store it first...

	// empty the list
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_Bounce);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

	TCHAR szAllTheCategories[STRING_MAX];
	pOPoom->GetContactCategories(szAllTheCategories);

    TCHAR szItem[STRING_LARGE];
	TCHAR* szToken;
	szToken = m_oStr->StringToken(szAllTheCategories, _T(","));
	while (szToken != NULL)
	{
        m_oStr->StringCopy(szItem, szToken);
		m_oStr->Trim(szItem);
        m_oMenu.AddItem(szItem, IDMENU_Selection);
		szToken = m_oStr->StringToken(NULL, _T(","));
	}

	// sort by string compare
	m_oMenu.SortList(CompareItems);
    m_oMenu.SetSelectedItemIndex(0, TRUE);

    ::SetCursor(hCursor);
}


int CDlgChooseCategory::CompareItems(const void* lp1, const void* lp2)
{
	TCHAR* szText1	= (TCHAR*)lp1;
	TCHAR* szText2	= (TCHAR*)lp2;

	CIssString* oStr = CIssString::Instance();

	if(!szText1 || !szText2 || oStr->IsEmpty(szText1) || oStr->IsEmpty(szText2))
		return -1;

	return oStr->Compare(szText1, szText2);
}

BOOL CDlgChooseCategory::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_oMenu.GetItemCount() == 0)
    {
        RECT rc = m_rcArea;
        rc.top  += INDENT;
        DrawTextShadow(gdi, _T("No categories found"), rc, DT_CENTER|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgChooseCategory::OnMenuLeft()
{
    BOOL bRet = GetCategoryFromSelection();
    SafeCloseWindow(bRet?IDOK:IDCANCEL);
}

void CDlgChooseCategory::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgChooseCategory::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select Category"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgChooseCategory::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}
