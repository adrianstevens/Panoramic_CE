#include "DlgContactMoreInfo.h"
#include "resource.h"
#include "DlgMsgBox.h"

#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*14/32)
#define HEIGHT_Indent           (GetSystemMetrics(SM_CXICON)*3/32)

CDlgContactMoreInfo::CDlgContactMoreInfo(void)
:m_szTitle(NULL)
,m_szBirthday(NULL)
,m_szDepartment(NULL)
,m_szCategory(NULL)
,m_szWebpage(NULL)
,m_szHomeAddress(NULL)
,m_szBusinessAddress(NULL)
,m_szNotes(NULL)
{
    if(m_hFontLabel)
        CIssGDIEx::DeleteFont(m_hFontLabel);

    m_hFontLabel = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
}

CDlgContactMoreInfo::~CDlgContactMoreInfo(void)
{
    Destroy();
}

void CDlgContactMoreInfo::Destroy()
{
    m_oStr->Delete(&m_szTitle);
    m_oStr->Delete(&m_szBirthday);
    m_oStr->Delete(&m_szDepartment);
    m_oStr->Delete(&m_szCategory);
    m_oStr->Delete(&m_szWebpage);
    m_oStr->Delete(&m_szHomeAddress);
    m_oStr->Delete(&m_szBusinessAddress);
	m_oStr->Delete(&m_szNotes);
}

void CDlgContactMoreInfo::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("More Info"), rc, DT_LEFT | DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(51,51,51));

}

void CDlgContactMoreInfo::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(IsRectInRect(rc, rcClip) && m_gdiBackArrow)
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

        // draw the arrow too
        Draw(gdi,
            WIDTH(m_rcBottomBar)/8 - m_gdiBackArrow->GetWidth(), rc.top + (HEIGHT(m_rcBottomBar)-m_gdiBackArrow->GetHeight())/2,
            m_gdiBackArrow->GetWidth(), m_gdiBackArrow->GetHeight(),
            *m_gdiBackArrow,
            0,0);
    }
}

void CDlgContactMoreInfo::PopulateList()
{
	if(!m_oMenu)
		return;

    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_Bounce);
    m_oMenu->SetSelected(IDR_PNG_ContactsSelector);
    m_oMenu->SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->PreloadImages(m_hWnd, m_hInst);
    m_oMenu->ResetContent();

    Destroy();

    int iLines = 0;
    int iIndex = 0;

    CPoomContacts* pPoom = CPoomContacts::Instance();
    if(!pPoom)
        return;

    RECT rcSize = {0, 0, GetSystemMetrics(SM_CXSCREEN) - HEIGHT_Indent*2, HEIGHT_Text};

    m_szTitle = pPoom->GetUserData(CI_JobTitle, m_sContact.lOid);

    m_szCategory = pPoom->GetUserData(CI_Category, m_sContact.lOid);
    if(m_szCategory)
    {
        iLines = GetLinesOfText(m_szCategory, m_hFontSmall, rcSize);

        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Category;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(iLines + 1);
        m_oMenu->AddSeparator();
    }

    m_szBirthday = pPoom->GetUserData(CI_Birthday, m_sContact.lOid);
    if(m_szBirthday)
    {
        iLines = GetLinesOfText(m_szBirthday, m_hFontSmall, rcSize);

        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Birthday;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(iLines + 1);
        m_oMenu->AddSeparator();
    }

    m_szHomeAddress = pPoom->GetUserData(CI_HomeAddress, m_sContact.lOid);
    if(m_szHomeAddress)
    {
        iLines = GetLinesOfText(m_szHomeAddress, m_hFontSmall, rcSize);

        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_HomeAddress;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(iLines + 1);
        m_oMenu->AddSeparator();
    }

    m_szDepartment = pPoom->GetUserData(CI_Department, m_sContact.lOid);
    if(m_szDepartment)
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Department;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(1);
        m_oMenu->AddSeparator(); 
    }

    m_szBusinessAddress = pPoom->GetUserData(CI_BusinessAddress, m_sContact.lOid);
    if(m_szBusinessAddress)
    {
        iLines = GetLinesOfText(m_szBusinessAddress, m_hFontSmall, rcSize);

        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_BusinessAddress;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(iLines + 1);
        m_oMenu->AddSeparator();
    }

    m_szWebpage = pPoom->GetUserData(CI_Webpage, m_sContact.lOid);
    if(m_szWebpage)
    {
        iLines = GetLinesOfText(m_szWebpage, m_hFontSmall, rcSize);

        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Webpage;
        m_oMenu->AddItem(eInfo, IDMENU_Selection);
        SetMenuItemHeight(iLines + 1);
        m_oMenu->AddSeparator();
    }

	m_szNotes = pPoom->GetUserData(CI_Notes, m_sContact.lOid);
	if(m_szNotes)
	{
       // CleanNotesBuffer(m_szNotes);
        iLines = GetLinesOfText(m_szNotes, m_hFontSmall, rcSize);

        if(iLines)
        {
		    EnumContactInfo* eInfo = new EnumContactInfo;
		    *eInfo = CI_Notes;
		    m_oMenu->AddItem(eInfo, IDMENU_Selection);
		    SetMenuItemHeight(iLines + 1);	// height for on line of text
        }

		// dynamically calculate the height of the notes section
	/*	RECT rcTest;
		SetRect(&rcTest, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		::DrawText(m_gdiMem->GetDC(), m_szNotes, rcTest, DT_RIGHT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, m_hFontLabel );

		TypeItems* sItem = m_oMenu.GetItem(m_oMenu.GetItemCount() - 1);
		if(sItem)
		{
			sItem->iHeight		+= HEIGHT(rcTest);
			sItem->iHeightSel	+= HEIGHT(rcTest);
		} */
		m_oMenu->AddSeparator();
	}
}

void CDlgContactMoreInfo::SetMenuItemHeight(int iTextHeight)
{
	if(!m_oMenu)
		return;

    TypeItems* sItem = m_oMenu->GetItem(m_oMenu->GetItemCount() - 1);
    if(!sItem)
        return;

	SIZE size;
	HFONT hOld = (HFONT)SelectObject(m_gdiMem->GetDC(), m_hFontLabel);
	GetTextExtentPoint( m_gdiMem->GetDC(), _T("A"), (int)_tcslen( _T("A") ), &size);
	SelectObject(m_gdiMem->GetDC(), hOld);

    sItem->iHeight      = iTextHeight*size.cy + 2*HEIGHT_Indent;
    sItem->iHeightSel   = iTextHeight*size.cy + 2*HEIGHT_Indent;
}

void CDlgContactMoreInfo::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
{
    RECT rc = rcDraw;
    rc.bottom = rc.top + HEIGHT(rcDraw)/2;

    if(!m_oStr->IsEmpty(m_sContact.szCompany))
        DrawTextShadow(gdi, m_sContact.szCompany, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.top  = rc.bottom;
    rc.bottom = rcDraw.bottom;

    if(!m_oStr->IsEmpty(m_szTitle))
        DrawTextShadow(gdi, m_szTitle, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

}

void CDlgContactMoreInfo::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumContactInfo* eInfo = (EnumContactInfo*)lpItem;
    delete eInfo;
}

void CDlgContactMoreInfo::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgContactMoreInfo* pThis = (CDlgContactMoreInfo*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumContactInfo* eInfo = (EnumContactInfo*)sItem->lpItem;

    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eInfo);
}

BOOL CDlgContactMoreInfo::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumContactInfo* eInfo)
{
    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR* szInfo = NULL;

    BOOL bSpaceHeader = FALSE;

    switch(*eInfo)
    {
    case CI_Birthday:
        szInfo = m_szBirthday;
        m_oStr->StringCopy(szTitle, _T("Birthday:"));
        bSpaceHeader = TRUE;
        break;
    case CI_Department:
        szInfo = m_szDepartment;
        m_oStr->StringCopy(szTitle, _T("Department:"));
        bSpaceHeader = TRUE;
        break;
    case CI_Category:
        szInfo = m_szCategory;
        m_oStr->StringCopy(szTitle, _T("Category:"));
        break;
	case CI_Notes:
		szInfo = m_szNotes;
		m_oStr->StringCopy(szTitle, _T("Notes:"));
		bSpaceHeader = TRUE;
		break;
    case CI_Webpage:
        szInfo = m_szWebpage;
        m_oStr->StringCopy(szTitle, _T("Website:"));
        bSpaceHeader = TRUE;
        break;
    case CI_HomeAddress:
        szInfo = m_szHomeAddress;
        m_oStr->StringCopy(szTitle, _T("Home address:"));
        bSpaceHeader = TRUE;
        break;
    case CI_BusinessAddress:
        szInfo = m_szBusinessAddress;
        m_oStr->StringCopy(szTitle, _T("Business address:"));
        bSpaceHeader = TRUE;
        break;
    }

    //just get it off the selector
    rc.left += 2;
    rc.right-= 2;
    rc.top += HEIGHT_Indent;
    rc.bottom -= HEIGHT_Indent;
    DrawTextShadow(gdi, szTitle, rc, DT_LEFT|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    if(bSpaceHeader)
        rc.top += HEIGHT_Text;
    DrawTextShadow(gdi, szInfo, rc, DT_RIGHT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    return TRUE;
}

void CDlgContactMoreInfo::OnSelectMenuItem(int iIndex, TypeItems* sItem)
{

	TCHAR szTitle[STRING_MAX] = _T("");
	TCHAR* szInfo = NULL;
	EnumContactInfo* eInfo = (EnumContactInfo*)sItem->lpItem;

	switch(*eInfo)
	{
	case CI_Birthday:
		szInfo = m_szBirthday;
		m_oStr->StringCopy(szTitle, _T("Birthday"));
		break;
	case CI_Department:
		szInfo = m_szDepartment;
		m_oStr->StringCopy(szTitle, _T("Department"));
		break;
	case CI_Category:
		szInfo = m_szCategory;
		m_oStr->StringCopy(szTitle, _T("Category"));
		break;
	case CI_Notes:
		szInfo = m_szNotes;
		m_oStr->StringCopy(szTitle, _T("Notes"));
		break;
	case CI_Webpage:
		szInfo = m_szWebpage;
		m_oStr->StringCopy(szTitle, _T("Webpage"));
		break;
	case CI_HomeAddress:
		szInfo = m_szHomeAddress;
		m_oStr->StringCopy(szTitle, _T("Home address"));
		break;
	case CI_BusinessAddress:
		szInfo = m_szBusinessAddress;
		m_oStr->StringCopy(szTitle, _T("Business address"));
		break;
	}

	SendTextToClipboard(szInfo, m_hWnd);

	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, szTitle);
	m_oStr->Concatenate(szText, _T(" text copied to the clipboard"));
	//m_oStr->Format(szText, _T("%s text copied to the clipboard"), szTitle);

	CDlgMsgBox msgBox;
	msgBox.Init(m_gdiMem, m_guiBackground);
    int iReturnVal = msgBox.PopupMessage(szText, _T("Copy to Clipboard"), m_hWnd, m_hInst, MB_OK);
}

BOOL CDlgContactMoreInfo::SendTextToClipboard(TCHAR* szSource, HWND hWnd)
{
	HGLOBAL hMem;

	int iLen = m_oStr->GetLength(szSource) + 1;

	::OpenClipboard(hWnd);
	::EmptyClipboard();
	hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, 2*iLen);
	memset(GlobalLock(hMem), 0, 2*iLen);

#ifdef UNICODE
	UINT uFmt = CF_UNICODETEXT;
	//	WCHAR* pCh;
	TCHAR* pCh;
	//	pCh = (unsigned short*)GlobalLock(hMem);
	pCh = (TCHAR*)GlobalLock(hMem);
#else
	UINT uFmt = CF_TEXT;
	char* pCh;
	pCh = (char*)GlobalLock(hMem);
#endif

	for (int i = 0; i < m_oStr->GetLength(szSource); i++) 
	{
		*(pCh+i) = szSource[i];
	}

	if (NULL == SetClipboardData(uFmt, hMem)) 
	{

	}
	CloseClipboard();
	return TRUE;
}

int CDlgContactMoreInfo::GetLinesOfText(TCHAR* szSource, HFONT hFont, RECT rc)
{
    RECT rcTemp = rc;

    DrawText(m_gdiMem->GetDC(), szSource, rcTemp, DT_RIGHT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, hFont);

    //now check what changed in rcTemp
    if(HEIGHT(rcTemp) != HEIGHT(rc)) //then it adjusted the bottom
    {
        return HEIGHT(rcTemp)/HEIGHT(rc);   
    }
    return 1;
}

//cause they like to put in blank lines
void CDlgContactMoreInfo::CleanNotesBuffer(TCHAR* szNotes)
{
    int iSearch = m_oStr->GetLength(szNotes) - 4;
    int iLocation = m_oStr->Find(szNotes, _T("\r\n\r\n"), iSearch);

    while(iLocation != -1)
    {
        m_oStr->Delete(iLocation, 2, szNotes);
        iSearch = m_oStr->GetLength(szNotes) - 4;
        iLocation = m_oStr->Find(szNotes, _T("\r\n\r\n"), iSearch);
    }
}