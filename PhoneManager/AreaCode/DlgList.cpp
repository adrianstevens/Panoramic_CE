#include "StdAfx.h"
#include "DlgList.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"
#include "Resource.h"
#include "IssGDIFX.h"

#define IDMENU_Area             (WM_USER + 400) //if you're gonna use it for math include the brackets

#define IDMENU_SelectItem               5000

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

#define LIST_MIN_FOR_HEADERS            19

CIssKineticList*    CDlgList::m_oMenu = NULL;
BOOL                CDlgList::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD               CDlgList::m_dwFullListCount = 0;

CDlgList::CDlgList(HWND hWndMainApp, CIssAreaCode* oArea, EnumLookupType eLookup)
:m_bShowSearch(FALSE)
,m_bScrollAlpha(FALSE)
,m_hFontText(NULL)
{
    m_hWndParent = hWndMainApp;
    m_eLookupType = eLookup;
    m_oArea = oArea;

    if(!m_oMenu)
        m_oMenu = new CIssKineticList;

    m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*15/32, FW_BOLD, TRUE);
}

CDlgList::~CDlgList(void)
{
    if(!m_bPreventDeleteList)
        ResetListContent();

    CIssGDIEx::DeleteFont(m_hFontText);
    m_hImgFontAlpha.Destroy();
}

void CDlgList::ResetListContent()
{
	if(m_oMenu)
	{
		delete m_oMenu;
		m_oMenu = NULL;
	}
	m_dwFullListCount = 0;
};

BOOL CDlgList::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;


    // do other menu initialization here
    m_oMenu->Initialize(hWnd, hWnd, m_hInst, dwFlags, TRUE);
    m_oMenu->SetSelectedItemIndex(0, TRUE);
    m_oMenu->SetCustomDrawFunc(DrawListItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteListItem);

    // we only fully repopulate if we don't have the same amount of contacts as before or our current list is empty
    if(m_oArea->GetCodeCount() != m_dwFullListCount || m_oMenu->GetItemCount() == 0)
    {
        PopulateList();
    }
 



    return TRUE;
}

void CDlgList::PopulateList()
{
    if(!m_oMenu)
        return;

    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu->PreloadImages(m_hWnd, m_hInst);

    // if we want to save selected, store it first...
    TCHAR szText[STRING_MAX];
    TCHAR szCategory[2];
    m_oStr->StringCopy(szCategory, _T(" "));

    // empty the list
    m_oMenu->ResetContent();

    m_oStr->Empty(m_szSearch);
    m_dwFullListCount   = m_oArea->GetCodeCount();

    AreaCodeType* area = NULL;

    if(m_eLookupType == LT_CountryCodes)
    {
        for(int i = 0; i < m_oArea->GetCountryCodeCount(); i++)
        {
            area = m_oArea->GetCountryCodeInfo(i);

            if(area == NULL)
                continue;

            m_oStr->Format(szText, _T("%s (%i)"), area->szCountry, area->iCountryCode);

            MenuType* menu = new MenuType;
            menu->szText = m_oStr->CreateAndCopy(szText);
            menu->iImage = area->iImageIndex;

            if(menu->szText[0] != szCategory[0])
            {
                szCategory[0] = menu->szText[0];
                m_oMenu->AddCategory(szCategory);
            }

            m_oMenu->AddItem((LPVOID)menu, IDMENU_SelectItem);

            //m_oMenu->AddItem(szText, IDMENU_SelectItem);//IDMENU_Area + i);
        }
    }
    else if(m_eLookupType == LT_AreaCodes)
    {
        for(int i = 0; i < m_oArea->GetAreaCodeCount(); i++)
        {
            area = m_oArea->GetAreaCodeInfo(i);

            if(area == NULL)
                continue;

            m_oStr->Format(szText, _T("%s (%i)"), area->szRegion, area->iAreaCode);

            MenuType* menu = new MenuType;
            menu->szText = m_oStr->CreateAndCopy(szText);
            menu->iImage = area->iImageIndex;

            if(menu->szText[0] != szCategory[0])
            {
                szCategory[0] = menu->szText[0];
                m_oMenu->AddCategory(szCategory);
            }

            m_oMenu->AddItem((LPVOID)menu, IDMENU_SelectItem);

            //m_oMenu->AddItem(szText, IDMENU_SelectItem);//IDMENU_Area + i);
        }
    }
    else
    {
        m_oArea->SetLookupType(m_eLookupType);

        for(int i = 0; i < m_oArea->GetOtherCount(); i++)
        {
            area = m_oArea->GetOtherInfo(i);

            if(area == NULL)
                continue;

            if(m_eLookupType == LT_UnitedKingdom)
                m_oStr->Format(szText, _T("%s (0%i)"), area->szRegion, area->iAreaCode);
            else
                m_oStr->Format(szText, _T("%s (%i)"), area->szRegion, area->iAreaCode);

            MenuType* menu = new MenuType;
            menu->szText = m_oStr->CreateAndCopy(szText);
            menu->iImage = area->iImageIndex;

            if(m_oArea->GetOtherCount() > LIST_MIN_FOR_HEADERS && menu->szText[0] != szCategory[0])
            {
                szCategory[0] = menu->szText[0];
                m_oMenu->AddCategory(szCategory);
            }

            m_oMenu->AddItem((LPVOID)menu, IDMENU_SelectItem);
            
            //m_oMenu->AddItem(szText, IDMENU_SelectItem);//IDMENU_Area + i);
        }
    }
}


void CDlgList::ScrollTo(TCHAR* szText)
{
    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    if(!m_oMenu)
        return;

    if(m_oStr->GetLength(szText) == 2)
        Sleep(1);

    //find the scroll position
    for(; iEntryPos < m_oMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu->GetItem(iEntryPos);
        if(!sItem)
            return;

        if(sItem->eType == ITEM_Unknown)
        {
            MenuType* menu = (MenuType*)sItem->lpItem;

            int iFind = m_oStr->FindNoCase(menu->szText, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(menu->szText, szText))
                break;

            iScrollToPos = iEntryPos;
        }
    }

    //otherwise do nothing
    //Error:

    if(iScrollToPos == m_oMenu->GetSelectedItemIndex())
        return;

    m_oMenu->SetSelectedItemIndex(iScrollToPos, TRUE);
    m_oMenu->ShowSelectedItem(TRUE);
}


//we'll make this smart - if its already on that letter it'll go to the next one of the same letter
//otherwise back to the start
//cases we need to check: 1 of the letter, 2 or more, none, none at the bottom
void CDlgList::ScrollTo(TCHAR szChar)
{
    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    TCHAR c = 0;

    if(!m_oMenu)
        return;

    //find the scroll position
    for(; iEntryPos < m_oMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu->GetItem(iEntryPos);
        if(!sItem)
            return;

        if(sItem->eType != ITEM_Unknown)
        {   
            c = sItem->szText[0];
        }
        else
        {
            MenuType* menu = (MenuType*)sItem->lpItem;
            c = menu->szText[0];
        }
       
        m_oStr->Capitalize(&c);
        if(c == szChar)
        {
            iScrollToPos = iEntryPos;
            break;
        }
        if(c > szChar)
        {
            break;
        }
        iScrollToPos = iEntryPos;
    }

    //get the current first letter
    TCHAR szCur = 0;
    TCHAR szNext = 0;
    sItem = NULL;
    sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex());

    if(sItem && sItem->szText)
    {
        szCur = sItem->szText[0];
        m_oStr->Capitalize(&szChar);

        if(szCur == c)//same letter
        {
            sItem = NULL;
            //see what's one position past the current
            sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex() + 1);
            if(sItem && sItem->szText)
            {
                szNext = sItem->szText[0];

                if(szNext == szCur)//we're golden
                    iScrollToPos = m_oMenu->GetSelectedItemIndex() + 1;
                //otherwise do nothing
            }
        }
    }

    if(iScrollToPos == m_oMenu->GetSelectedItemIndex())
        return;

    m_oMenu->SetSelectedItemIndex(iScrollToPos, TRUE);
    m_oMenu->ShowSelectedItem(TRUE);
}

BOOL CDlgList::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);

    static int iLen = 0;

    if(m_oStr->IsEmpty(m_szSearch))
        iLen = 0;

    if(iLen + 1 == STRING_SMALL)
        return UNHANDLED;

    if(iswalpha(c) || c == _T(' ') || c == _T('-') || c == _T('#') || c == _T('_') || c == _T(',') || c == _T('*'))
    {   
        //    ScrollTo(toupper(c));

        //hack for now
        m_szSearch[iLen] = toupper(c);
        m_szSearch[iLen+1] = _T('\0');
        KillTimer(m_hWnd, IDT_TIMER_Search);
        SetTimer(m_hWnd, IDT_TIMER_Search, Search_Timeout, NULL);
        iLen++;

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        ScrollTo(m_szSearch);
    }
    //backspace I think
    if(c == VK_BACK && iLen > 0)
    {
        m_szSearch[iLen-1] = m_szSearch[iLen];
        m_szSearch[iLen] = _T('\0');
        iLen--;
        KillTimer(m_hWnd, IDT_TIMER_Search);
        SetTimer(m_hWnd, IDT_TIMER_Search, Search_Timeout, NULL);

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        ScrollTo(m_szSearch);
    }

    return UNHANDLED;
}

BOOL CDlgList::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(!m_oMenu)
        return TRUE;

    if(m_gdiAphaBg.GetDC() == NULL)
        LoadImages();

    if(m_oMenu->GetItemCount() == 0)
    {
        DrawTextShadow(gdi, _T("No Locations Found"), rcClient, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), 0);
    }
    else
    {
        m_oMenu->OnDraw(gdi, rcClient, rcClip);
        DrawAlphabet(gdi, rcClip);
        DrawSearch(gdi, rcClient);
    }

    return TRUE;
}


void CDlgList::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;
    
    m_hImgFontAlpha.DrawText(gdi, m_szSearch, rcDraw, DT_CENTER | DT_VCENTER);
    //DrawTextShadow(gdi, m_szSearch, rcDraw, DT_CENTER|DT_VCENTER, m_hFontSearch, RGB(150,150,150), 0);
}


BOOL CDlgList::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(!m_oMenu)
        return TRUE;

    if(PtInRect(&m_rcAlpha, pt))
    {
        m_bScrollAlpha = TRUE;
        DrawAlphabetPopUp(pt);
    }

    return m_oMenu->OnLButtonDown(pt);
}

BOOL CDlgList::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_bScrollAlpha)
    {
        m_bScrollAlpha = FALSE;
        InvalidateRect(m_hWndParent, NULL, FALSE);

        TCHAR szChar = _T('A' + (TCHAR)((pt.y - m_rcAlphaText.top - HEIGHT(m_rcTitle))/(m_iAlphaTextSpacing))) ;
        ScrollTo(szChar);

        m_gdiTemp.Destroy();
        InvalidateRect(m_hWnd, NULL, FALSE);
        return UNHANDLED;
    }


    if(m_oMenu && m_oMenu->OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < WIDTH(m_rcBottomBar)/2)
        {
            SafeCloseWindow(IDOK);
        }
        else
        {
            SafeCloseWindow(IDCANCEL);
        }
    }
    return TRUE;
}

BOOL CDlgList::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(!m_oMenu)
        return TRUE;

    if(m_bScrollAlpha)
    {
        DrawAlphabetPopUp(pt);
        return TRUE;
    }

    return m_oMenu->OnMouseMove(pt);
}

BOOL CDlgList::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(!m_oMenu)
        return TRUE;

    return m_oMenu->OnKeyDown(wParam, lParam);
}


BOOL CDlgList::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIMER_Search)
    {
        KillTimer(hWnd, IDT_TIMER_Search);
        m_oStr->Empty(m_szSearch);
        m_bShowSearch = FALSE;
        //could fade out here ....
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
    if(!m_oMenu)
        return TRUE;

    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgList::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    int iIndent = INDENT;
    int iAlphaWidth = IsVGA() ? CONTACTS_ALPHA_WIDTH*2 : CONTACTS_ALPHA_WIDTH;

    //m_oMenu.OnSize(iIndent,HEIGHT(m_rcTitle) + iIndent,WIDTH(rc)-2*iIndent, HEIGHT(rc)-2*iIndent - HEIGHT(m_rcBottomBar) - HEIGHT(m_rcBottomBar));
    if(m_oMenu)
        m_oMenu->OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea) - iAlphaWidth, HEIGHT(m_rcArea));


    m_rcAlpha = rc;
    
    m_rcAlpha.left = m_rcAlpha.right - iAlphaWidth;
    m_rcAlpha.top += HEIGHT(m_rcTitle);
    m_rcAlpha.top -= GetSystemMetrics(SM_CXICON)/8;
    m_rcAlpha.bottom -= HEIGHT(m_rcBottomBar);

    m_gdiAphaBg.Destroy();


    return TRUE;
}

void CDlgList::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;

    AreaCodeType* area = NULL;

    if(m_oArea->GetLookupType() > LT_AreaCodes)
    {   //its a country
        area = m_oArea->GetOtherInfo(0);

        if(area)
        {
            DrawText(gdi, area->szCountry, rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
            return;
        }//otherwise keep going
    }

    DrawText(gdi, _T("Select Location:"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
    
    

     

    
}

void CDlgList::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

BOOL CDlgList::DrawAlphabet(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(!IsRectInRect(rcDraw, m_rcAlpha))
        return FALSE;

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    RECT rc = rcDraw;
    rc.bottom = min(rc.bottom, rcClient.bottom);
    rc.left = max(rc.left, rcClient.left);

    return BitBlt(gdi.GetDC(),
        m_rcAlpha.left, m_rcAlpha.top,
        WIDTH(m_rcAlpha), HEIGHT(m_rcAlpha),
        m_gdiAphaBg.GetDC(),
        0,0,
        SRCCOPY);

    return FALSE;
}

void CDlgList::OnMenuLeft()
{
    //FindOIDFromSelection();
    SafeCloseWindow(IDOK);
}

void CDlgList::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}



BOOL CDlgList::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDOK:
//    case IDMENU_Ok:
    case IDMENU_SelectItem:
        SafeCloseWindow(IDOK);
        break;
        SafeCloseWindow(IDCANCEL);
        break;
    default:
        return UNHANDLED;
    }
    return TRUE;
}

void CDlgList::LoadImages()
{
    if(!m_hImgFontAlpha.IsLoaded())
    {
        int h = GetSystemMetrics(SM_CXICON)*4/5;
        UINT uiRes = IsVGA() ? IDR_PNG_FontVGA : IDR_PNG_Font;
        m_hImgFontAlpha.Initialize(uiRes, m_hWndParent, m_hInst, -1);
    }

    if(m_gdiAphaBg.GetDC() == NULL)
    {
        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);

        m_gdiAphaBg.Create(m_gdiMem->GetDC(), m_rcAlpha/*rcClient*/, TRUE);

        RECT rcAlphaFrame = {0,0,WIDTH(m_rcAlpha), HEIGHT(m_rcAlpha)};
      
      //  Rectangle(m_gdiAphaBg, rcAlphaFrame, COLOR_CONTACTS_ALPHA_BG,COLOR_CONTACTS_ALPHA_BG);
        Rectangle(m_gdiAphaBg, rcAlphaFrame, 0x949694,0x949694);


        
        int iFontSize = HEIGHT(m_rcAlpha)/26 + 2;//11 worked nicely on PPC 

        HFONT hFontAlpha = CIssGDIEx::CreateFont(iFontSize, FW_NORMAL, TRUE);
        m_iAlphaTextSpacing = (iFontSize-2);
        m_iAlphaWidth = m_iAlphaTextSpacing*5/3;
        int iTotalSize = m_iAlphaTextSpacing*26;
        int iStart = 0;

        TCHAR szLetter[2];
        szLetter[1] = _T('\0');

        m_rcAlphaText = m_rcAlpha;
        m_rcAlphaText.top = iStart;
        m_rcAlphaText.bottom -= (iStart - m_rcAlpha.top);

        RECT rcAlpha = rcAlphaFrame;
        rcAlpha.top		 = iStart;
        rcAlpha.bottom	 = rcAlpha.top + m_iAlphaTextSpacing;

        //Time to draw the Alphabet
        for(int i = 0; i < 26; i++)
        {
            szLetter[0] = _T('A')+i;
            DrawText(m_gdiAphaBg.GetDC(), szLetter, rcAlpha, DT_CENTER | DT_TOP, hFontAlpha, 0);
            rcAlpha.top += m_iAlphaTextSpacing;
            rcAlpha.bottom += m_iAlphaTextSpacing;
        }
        CIssGDIEx::DeleteFont(hFontAlpha);
    }

    if(!m_gdiAlphaPopUp.GetDC())
    {
        if(IsVGA())
            m_gdiAlphaPopUp.LoadImage(IDR_PNG_ContactsLetterBubbleVGA, m_hWndParent, m_hInst, TRUE);
        else
            m_gdiAlphaPopUp.LoadImage(IDR_PNG_ContactsLetterBubble, m_hWndParent, m_hInst, TRUE);
    }
}

BOOL CDlgList::DrawAlphabetPopUp(POINT pt)
{
    if(pt.y < m_rcAlphaText.top)
        pt.y = m_rcAlphaText.top;
    else if(pt.y >= m_rcAlphaText.bottom)
        pt.y = m_rcAlphaText.bottom - 1;


    if(m_gdiTemp.GetDC() == NULL || m_gdiTemp.GetWidth() != m_gdiTemp.GetWidth())    
        m_gdiTemp.Create(m_gdiMem->GetDC(), m_gdiMem->GetWidth(), m_gdiMem->GetHeight(), TRUE);

    HDC hdc = GetDC(m_hWnd);
    static RECT rcTemp;
    static POINT ptOldPos = pt;
    static TCHAR szText[2] = {_T('\0'),_T('\0')};

    RECT rc;
    rc.right	= m_rcAlpha.left;//a little bigger than needed but oh well
    rc.left	    = rc.right - m_gdiAlphaPopUp.GetWidth();
    rc.top		= ptOldPos.y;
    rc.bottom	= rc.top + m_gdiAlphaPopUp.GetHeight();

    BitBlt(m_gdiMem->GetDC(), 
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_gdiTemp.GetDC(),//m_gdiTempBack.GetDC(), 
        rc.left, rc.top, 
        SRCCOPY);

    int iCharOffset = max(0, (pt.y - m_rcAlphaText.top - HEIGHT(m_rcTitle))/(m_iAlphaTextSpacing));

    //now we're going to center the popup
    int iAlphaHeight = m_gdiAlphaPopUp.GetHeight();
    pt.y -= (iAlphaHeight/2);
    if(pt.y < m_rcAlpha.top)
        pt.y = m_rcAlpha.top;
    if(pt.y > m_rcAlpha.bottom - iAlphaHeight)
        pt.y = m_rcAlpha.bottom - iAlphaHeight;


    szText[0] = _T('A') + iCharOffset;

    RECT rcNew = rc;
    rcNew.top		= pt.y;
    rcNew.bottom	= rcNew.top + iAlphaHeight;

    ::Draw(*m_gdiMem, rcNew,  m_gdiAlphaPopUp);

    RECT rcText = rcNew;
    rcText.right = rcText.left + (long)(WIDTH(rcText)*3/4);
    rcText.top   = rcText.top + (m_gdiAlphaPopUp.GetHeight() - m_hImgFontAlpha.GetHeight())/2 + m_hImgFontAlpha.GetHeight()/12;
    m_hImgFontAlpha.DrawText(*m_gdiMem, szText, rcText, DT_CENTER | DT_TOP);

  //  rc.Concatenate(rcNew);
    
    rc.right = m_rcAlpha.right;
    UnionRect(&rc, &rc, &rcNew);


    BitBlt(hdc, 
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_gdiMem->GetDC(), 
        rc.left, rc.top, 
        SRCCOPY);

    ptOldPos = pt;

    ReleaseDC(m_hWndParent, hdc);
    return TRUE;
}


void CDlgList::LoadFlag(int iIndex)
{
    static int iTemp = -1;

    //probably don't need to load it every time
    if(m_gdiFlag.GetDC() != NULL &&
        iTemp == iIndex)
        return;

    iTemp = iIndex;

    m_gdiFlag.Destroy();

    if(iTemp < 1)
        return;

    CIssGDIEx gdiTemp;

    gdiTemp.LoadImage(IDR_PNG_1 + iTemp - 1, m_hWnd, m_hInst);

    SIZE sz;

    sz.cy = gdiTemp.GetHeight()*GetSystemMetrics(SM_CXICON)/64+1;
    sz.cx = gdiTemp.GetWidth()*GetSystemMetrics(SM_CXICON)/64;

    //ScaleImageFast
    ScaleImage(gdiTemp, m_gdiFlag, sz, FALSE, 0);

    
}


void CDlgList::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{

    if(!sItem || sItem->lpItem == NULL || sItem->eType != ITEM_Unknown)
        return;

    MenuType* menu = (MenuType*)sItem->lpItem;

    int iIndent = INDENT;
    InflateRect(&rcDraw, -iIndent, 0);

    if(bIsHighlighted)
    {
        LoadFlag(menu->iImage);

        int iGap = (HEIGHT(rcDraw) - m_gdiFlag.GetHeight())/2;

        //Flag it up yo
        RECT rcFlag;
        rcFlag.right = rcDraw.right + 1;
        rcFlag.left = rcFlag.right - m_gdiFlag.GetWidth();

        rcFlag.top = rcDraw.top + iGap;
        rcFlag.bottom = rcFlag.top + m_gdiFlag.GetHeight();

        ::Draw(gdi, rcFlag, m_gdiFlag);
    }

    ::DrawText(gdi.GetDC(), menu->szText, 
        rcDraw, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontText, COLOR_TEXT_NORMAL);

    
}

void CDlgList::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgList* pList = (CDlgList*)lpClass;
    pList->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

void CDlgList::DeleteListItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    CIssString* oStr = CIssString::Instance();

    MenuType* menu = (MenuType*)lpItem;

    if(menu->szText)
        oStr->Delete(&menu->szText);

    delete menu;
    menu = NULL;
}

//Why you ask?
//because if we remove the separators/section headers then the list lines up exactly with the vector
//in the area code class and we don't have to search for everything again
int CDlgList::GetSelected()
{
    int iRet = 0;

    for(int i = 0; i < m_oMenu->GetSelectedItemIndex(); i++)
    {
        if(m_oMenu->GetItem(i))
        {
            if(m_oMenu->GetItem(i)->eType == ITEM_Unknown)
                iRet++;
        }
    }

    return iRet;
}

