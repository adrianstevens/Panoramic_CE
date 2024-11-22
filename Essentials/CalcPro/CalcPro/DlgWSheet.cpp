#include "DlgWSheet.h"
#include "IssGDIDraw.h"
#include "globals.h"
#include "IssDebug.h"
#include "gdiplusenums.h"
#include "IssLocalisation.h"

#define WS_INDENT GetSystemMetrics(SM_CXSMICON)/2

#define WM_WORKSHEET WM_USER + 400

#define WS_TEXT_INDENT (TEXT_INDENT*3)

CDlgWSheet::CDlgWSheet(void)
:m_oStr(CIssString::Instance())
,m_oCalc(NULL)
,m_oBtnHand(NULL)
,m_oBtnMan(NULL)
,m_hFontMainBtnText(NULL)
,m_hFontMainLongText(NULL)
,m_hFontDisplay(NULL)
,m_hFontDisplaySm(NULL)
,m_iEntryIndex(0)
,m_iScreenIndex(0)
,m_iLinesOnscreen(0)
,m_iTextHeight(0)
,m_hWndParent(NULL)
,m_bLandScape(FALSE)
,m_oHandBtns(NULL)
,m_iSelectorIndex(0)//smartphone only
,m_bDrawBtnSelector(FALSE)
,m_iButtonHeight(1)
,m_iButtonWidth(1)
,m_wndMenu(NULL)
{
#ifndef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CYSCREEN) < GetSystemMetrics(SM_CXSCREEN))
        m_bLandScape = TRUE;
#endif

    m_iTextHeight = GetSystemMetrics(SM_CXSMICON);//looks about right to me
    if(m_bLandScape)
        m_iSmallTextHeight = m_iTextHeight - 2*DISPLAY_INDENT;
    else
        m_iSmallTextHeight = m_iTextHeight - DISPLAY_INDENT;

    m_hFontDisplay = CIssGDIEx::CreateFont(m_iTextHeight, FW_BOLD, TRUE);
    m_hFontDisplaySm = CIssGDIEx::CreateFont(m_iTextHeight - 2*DISPLAY_INDENT, FW_BOLD, TRUE);
}

CDlgWSheet::~CDlgWSheet(void)
{
    Destroy();//not that it actually gets called ...
}

void CDlgWSheet::Destroy()
{
    DeleteFonts();

    DeleteDynBtnVector(&m_arrButtons);

    DeleteEntryArray();
}

void CDlgWSheet::DeleteEntryArray()
{
    for(int i = 0; i < m_arrEntries.GetSize(); i++)
    {
		TypeEntry* sItem = m_arrEntries[i];
        if(sItem)
        {
            m_oStr->Delete(&sItem->szEntryLabel);
			delete sItem;
        }
    }
    m_arrEntries.RemoveAll();
}

void CDlgWSheet::Init(TypeCalcInit* sInit, CHandleButtons* oHandBtns, BOOL bPlaySounds, HWND hWnd)
{
    if(sInit != NULL)
    {
        m_oBtnHand	= sInit->oBtnHand;
        m_oBtnMan	= sInit->oBtnMan;
        m_oCalc		= sInit->oCalc;
        m_wndMenu   = sInit->wndMenu;

        //calc light color
        COLORREF crText = m_oBtnMan->GetSkin()->m_crTextTop;
        COLORREF crBack = m_oBtnMan->GetSkin()->m_typeDisplay.ButtonColor2;
        m_crLight = RGB(GetRValue(crText)/4 + GetRValue(crBack)*3/4,
            GetGValue(crText)/4 + GetGValue(crBack)*3/4,
            GetBValue(crText)/4 + GetBValue(crBack)*3/4);

        m_oHandBtns = oHandBtns;
        m_bPlaySounds = bPlaySounds;

		m_hWndParent = hWnd;
    }

    m_szWindowSize.cx = 0;
    m_szWindowSize.cy = 0;
}

BOOL CDlgWSheet::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
#ifdef WIN32_PLATFORM_WFSP
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
#else
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR|SHIDIF_SIPDOWN;
#endif
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);

#ifdef WIN32_PLATFORM_WFSP
    //	BOOL bRet = SendMessage(m_oEdit.GetWnd(), EM_SETINPUTMODE, 0, EIM_TEXT );

    //	BOOL bRet = Edit_SetInputMode(m_oEdit.GetWnd(), IM_NUMBERS);
    SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON); 
    RECT rc; 
    GetClientRect(hWnd, &rc);
    SetRect(&rc, rc.top, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-rc.top); 
    MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
#endif

#ifdef WIN32_PLATFORM_PSPC
    if(m_oBtnMan->GetFullscreen())
    {
        SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON); 
        RECT rc; 
        GetClientRect(hWnd, &rc);
        SetRect(&rc, rc.top, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-rc.top); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
    }
#endif

#else //PC
	RECT rc;
	GetWindowRect(GetParent(hWnd), &rc);
	MoveWindow(hWnd, rc.left, rc.top, WINDOW_WIDTH, WINDOW_HEIGHT, TRUE);
	EnableWindow(hWnd, TRUE);
//	SetForegroundWindow(hWnd);
//	SetFocus(hWnd);
//	SetCapture(hWnd);
//	EnableWindow(hWnd, TRUE);
#endif
    InitScreenEntries();
    return TRUE;
}

BOOL CDlgWSheet::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnLButtonDown(pt, hWnd) == TRUE)
        return TRUE;

    if(PtInRect(&m_rcDisplay, pt))
    {

    }
    else if(PtInRect(&m_rcMainBtns, pt))
    {
        for(int i = 0; i < m_arrButtons.GetSize(); i++)
        {
            if(m_arrButtons[i] != NULL)
            {
                if(m_arrButtons[i]->OnLButtonDown(pt))
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL CDlgWSheet::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnLButtonUp(pt, hWnd) == TRUE)
        return TRUE;

    BOOL bRet = FALSE;

    if(PtInRect(&m_rcDisplay, pt))
    {
        for(int i = 0; i < m_iLinesOnscreen; i++)
        {
            if(i+m_iEntryIndex >= m_arrEntries.GetSize())
                goto Error;
            if(PtInRect(&m_rcEntries[i], pt))
            {
                SetScreenValue();//save whatever is in the current selector
                m_iScreenIndex = i;
                InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
                
                m_oCalc->AddString(m_arrEntries[i+m_iEntryIndex]->szEntryValue);
                bRet = TRUE;
                goto Error;
            }
        }
        if(pt.y < m_rcEntries[0].top)
        {
            if(OnKeyUp(m_hWnd, VK_UP, 0))
            {
                bRet=TRUE;
                goto Error;
            }
        }
        else if(pt.y > m_rcEntries[m_iLinesOnscreen-1].bottom)
        {
            if(OnKeyUp(m_hWnd, VK_DOWN, 0))
            {
                bRet=TRUE;
                goto Error;
            }
        }
    }
    else if(PtInRect(&m_rcMainBtns, pt))
    {
        for(int i = 0; i <  m_arrButtons.GetSize(); i++)
        {
            if(m_arrButtons[i] != NULL)
            {
                if(m_arrButtons[i]->OnLButtonUp(pt))
                {
                    if(m_bLandScape)
                    {
                        PostMessage(hWnd, WM_WORKSHEET, m_oBtnMan->GetCurrentLayout()->sAltButtons[i]->iButtonType, 
                            m_oBtnMan->GetCurrentLayout()->sAltButtons[i]->iButtonValue);
                    }
                    else
                    {
                        PostMessage(hWnd, WM_WORKSHEET, m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonType, 
                            m_oBtnMan->GetCurrentLayout()->sMainButtons[i]->iButtonValue);
                    }
                    InvalidateRect(hWnd, &m_rcDisplay, FALSE);
                    bRet = TRUE;
                    goto Error;
                }
            }
        }
    }

Error:

    if(bRet == TRUE)//and play the sound
        PlaySounds();

    return bRet;
}

BOOL CDlgWSheet::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	static RECT rcClip;
	GetClipBox(hDC, &rcClip);

	
	if(m_gdiMem.GetDC() == NULL)
    {
        m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);
		GetClientRect(hWnd, &rcClip); // PC ...
    }

    Draw(m_gdiMem.GetDC(), rcClip);

    m_oMenu.DrawMenu(m_gdiMem.GetDC(), rcClip, m_oStr->GetText(ID(IDS_MENU_Back), m_hInst), NULL);

    // draw all to the screen
    BitBlt( hDC,
        rcClip.left, rcClip.top, 
        WIDTH(rcClip),
        HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,
        rcClip.top,
        SRCCOPY);

    return TRUE;
}

void CDlgWSheet::Draw(HDC hDC, RECT& rcClip)
{
    FillRect(hDC, rcClip, m_oBtnMan->GetSkin()->GetBackgroundColor());

    //lets just do this once
    if(m_rcDisplay.left > rcClip.right ||
        m_rcDisplay.right < rcClip.left ||
        m_rcDisplay.top > rcClip.bottom ||
        m_rcDisplay.bottom < rcClip.top)
    {
    }
    else
    {
        m_oDisplay.Draw(hDC);
        DrawPanels(hDC, rcClip);
        DrawScreenEntries(hDC);
        DrawSelector(hDC);
    }
    //buttons are optimized individually
    DrawButtons(hDC, rcClip);
    DrawBtnSelector(hDC, rcClip);//not optomized
}

void CDlgWSheet::DrawButtonsSP12(HDC hDC, RECT& rcClip)
{
    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    if(sLayout == NULL)
        return;

    HFONT hFontTemp = NULL;

    for(int i = 0; i < m_arrButtons.GetSize(); i++)
    {
        if(m_arrButtons[i] != NULL)
        {
            if(m_arrButtons[i]->GetButtonRect().left > rcClip.right ||
                m_arrButtons[i]->GetButtonRect().right < rcClip.left ||
                m_arrButtons[i]->GetButtonRect().top > rcClip.bottom ||
                m_arrButtons[i]->GetButtonRect().bottom < rcClip.top)
                continue;

            if(m_oStr->GetLength(sLayout->sMainButtons[i]->szLabel) > 2)
                hFontTemp = m_hFontMainLongText;
            else
                hFontTemp = m_hFontMainBtnText;

            static TCHAR szTemp[] = _T(" ");
            TCHAR* szAlt = sLayout->sMainAltButtons[i]->szLabel;

            if(szAlt == NULL)
                szAlt = szTemp;


          //  m_arrButtons[i]->Draw(hFontTemp, hDC, sLayout->sMainButtons[i]->szLabel);
            m_arrButtons[i]->Draw(m_hFontMainLongText, hDC, sLayout->sMainButtons[i]->szLabel, 
                NULL, szAlt, NULL, m_hFontMainLongText);

            RECT rcTemp;

            //check for backspace
            if(sLayout->sMainAltButtons[i]->iButtonType == INPUT_Clear &&
                sLayout->sMainAltButtons[i]->iButtonValue == CLEAR_BackSpace)
            {
                rcTemp = m_arrButtons[i]->GetButtonRect();
                rcTemp.right -= WIDTH(rcTemp)*2/5;
                rcTemp.bottom -= HEIGHT(rcTemp)*2/5;

                DrawBackSpace(hDC, rcTemp,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor2,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor,
                FALSE);
            }
            else if(sLayout->sMainAltButtons[i]->iButtonType == INPUT_WS &&
                sLayout->sMainAltButtons[i]->iButtonValue == WS_Up)
            {
                rcTemp = m_arrButtons[i]->GetButtonRect();
                rcTemp.right -= WIDTH(rcTemp)*2/5;
                rcTemp.bottom -= HEIGHT(rcTemp)*2/5;

                DrawVertArrows(hDC, rcTemp,
                FALSE,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor2,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor,
                FALSE);
            }
            else if(sLayout->sMainAltButtons[i]->iButtonType == INPUT_WS &&
                sLayout->sMainAltButtons[i]->iButtonValue == WS_Down)
            {
                rcTemp = m_arrButtons[i]->GetButtonRect();
                rcTemp.right -= WIDTH(rcTemp)*2/5;
                rcTemp.bottom -= HEIGHT(rcTemp)*2/5;

                DrawVertArrows(hDC, rcTemp,
                TRUE,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor2,
                m_oBtnMan->GetSkin()->m_typeBtnClr[0].TextColor,
                FALSE);
            }
        }
    }
}

void CDlgWSheet::DrawButtonsSPLand(HDC hDC, RECT& rcClip)
{
    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    if(sLayout == NULL)
        return;

    HFONT hFontTemp = NULL;

    for(int i = 0; i < m_arrButtons.GetSize(); i++)
    {
        if(m_arrButtons[i] != NULL)
        {
            if(m_arrButtons[i]->GetButtonRect().left > rcClip.right ||
                m_arrButtons[i]->GetButtonRect().right < rcClip.left ||
                m_arrButtons[i]->GetButtonRect().top > rcClip.bottom ||
                m_arrButtons[i]->GetButtonRect().bottom < rcClip.top)
                continue;

            if(m_oStr->GetLength(sLayout->sMainButtons[i]->szLabel) > 2)
                hFontTemp = m_hFontMainLongText;
            else
                hFontTemp = m_hFontMainBtnText;

            m_arrButtons[i]->Draw(hFontTemp, hDC, sLayout->sMainButtons[i]->szLabel);

            //check for backspace
            if(sLayout->sMainButtons[i]->iButtonType == INPUT_Clear &&
                sLayout->sMainButtons[i]->iButtonValue == CLEAR_BackSpace)
                DrawBackSpace(hDC, m_arrButtons[i]->GetButtonRect(),
                m_oBtnMan->GetSkin()->GetButton(3)->TextColor,
                m_oBtnMan->GetSkin()->GetButton(3)->TextColor2,
                m_oBtnMan->GetSkin()->m_bUseTextShadow);
            else if(sLayout->sMainButtons[i]->iButtonType == INPUT_WS &&
                sLayout->sMainButtons[i]->iButtonValue == WS_Up)
                DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                FALSE,
                m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                m_oBtnMan->GetSkin()->m_bUseTextShadow);
            else if(sLayout->sMainButtons[i]->iButtonType == INPUT_WS &&
                sLayout->sMainButtons[i]->iButtonValue == WS_Down)
                DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                TRUE,
                m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                m_oBtnMan->GetSkin()->m_bUseTextShadow);
            
        }
    }
}

void CDlgWSheet::DrawButtons(HDC hDC, RECT& rcClip)
{
#ifdef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))//DON'T use the member variable here
    {
        DrawButtonsSP12(hDC, rcClip);
    }
    else
    {
        DrawButtonsSPLand(hDC, rcClip);
    }
    return;
#endif


    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    if(sLayout == NULL)
        return;

    HFONT hFontTemp = NULL;

    for(int i = 0; i < m_arrButtons.GetSize(); i++)
    {
        if(m_arrButtons[i] != NULL)
        {
            if(m_arrButtons[i]->GetButtonRect().left > rcClip.right ||
                m_arrButtons[i]->GetButtonRect().right < rcClip.left ||
                m_arrButtons[i]->GetButtonRect().top > rcClip.bottom ||
                m_arrButtons[i]->GetButtonRect().bottom < rcClip.top)
                continue;

            if(m_bLandScape)
            {
                if(m_oStr->GetLength(sLayout->sAltButtons[i]->szLabel) > 2)
                    hFontTemp = m_hFontMainLongText;
                else
                    hFontTemp = m_hFontMainBtnText;
                    
                m_arrButtons[i]->Draw(hFontTemp, hDC, sLayout->sAltButtons[i]->szLabel);

                //check for backspace
                if(sLayout->sAltButtons[i]->iButtonType == INPUT_Clear &&
                    sLayout->sAltButtons[i]->iButtonValue == CLEAR_BackSpace)
                    DrawBackSpace(hDC, m_arrButtons[i]->GetButtonRect(),
                    m_oBtnMan->GetSkin()->GetButton(3)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(3)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);

                else if(sLayout->sAltButtons[i]->iButtonType == INPUT_WS &&
                    sLayout->sAltButtons[i]->iButtonValue == WS_Up)
                    DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                    FALSE,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);
                else if(sLayout->sAltButtons[i]->iButtonType == INPUT_WS &&
                    sLayout->sAltButtons[i]->iButtonValue == WS_Down)
                    DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                    TRUE,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);
            }
            else //portrait
            {
                if(m_oStr->GetLength(sLayout->sMainButtons[i]->szLabel) > 2)
                    hFontTemp = m_hFontMainLongText;
                else
                    hFontTemp = m_hFontMainBtnText;

                m_arrButtons[i]->Draw(hFontTemp, hDC, sLayout->sMainButtons[i]->szLabel);

                //check for backspace
                if(sLayout->sMainButtons[i]->iButtonType == INPUT_Clear &&
                    sLayout->sMainButtons[i]->iButtonValue == CLEAR_BackSpace)
                    DrawBackSpace(hDC, m_arrButtons[i]->GetButtonRect(),
                    m_oBtnMan->GetSkin()->GetButton(3)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(3)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);
                else if(sLayout->sMainButtons[i]->iButtonType == INPUT_WS &&
                    sLayout->sMainButtons[i]->iButtonValue == WS_Up)
                    DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                    FALSE,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);
                else if(sLayout->sMainButtons[i]->iButtonType == INPUT_WS &&
                    sLayout->sMainButtons[i]->iButtonValue == WS_Down)
                    DrawVertArrows(hDC, m_arrButtons[i]->GetButtonRect(),
                    TRUE,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor,
                    m_oBtnMan->GetSkin()->GetButton(4)->TextColor2,
                    m_oBtnMan->GetSkin()->m_bUseTextShadow);
            }
        }
    }
}

BOOL CDlgWSheet::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;

    GetClientRect(m_hWnd, &rcClient);

#ifdef UNDER_CE
    //stupid windows mobile ... I mean seriously
    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rcClient))
        return FALSE;
#ifndef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CYSCREEN) < HEIGHT(rcClient))
        return FALSE;//I HATE windows mobile
#else   //seriously .. why the fuck is the window resized 8 times?
    if(GetSystemMetrics(SM_CYSCREEN) != HEIGHT(rcClient))
        return FALSE;//I HATE windows mobile
#endif 
#endif

    //save a little processing yo
    if(m_szWindowSize.cx == WIDTH(rcClient) &&
        m_szWindowSize.cy == HEIGHT(rcClient))
        return TRUE;

    m_szWindowSize.cx = WIDTH(rcClient);
    m_szWindowSize.cy = HEIGHT(rcClient);

    
    m_oMenu.Init(m_hWnd, m_hInst, m_oBtnMan->GetSkinType());

    rcClient.bottom -= m_oMenu.GetMenuHeight();

#ifndef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        m_bLandScape = TRUE;
    else 
        m_bLandScape = FALSE;
#else
    m_iSelectorIndex = 0;//reset it
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        m_bDrawBtnSelector = TRUE;
    else
        m_bDrawBtnSelector = FALSE;
#endif

    m_gdiMem.Destroy();

    m_rcDisplay.left = rcClient.left;
    m_rcDisplay.top = rcClient.top;

    m_rcMainBtns.right = rcClient.right;
    m_rcMainBtns.bottom = rcClient.bottom;

    int iIndent = WS_INDENT;

    

    if(m_bLandScape)
    {   //landscape 
        m_rcDisplay.left = rcClient.left;
        m_rcDisplay.top = rcClient.top;
        m_rcDisplay.right = WIDTH(rcClient)/2 + rcClient.left;
        m_rcDisplay.bottom = rcClient.bottom;

        m_rcMainBtns.left = m_rcDisplay.right;
        m_rcMainBtns.top = rcClient.top;
        m_rcMainBtns.right = rcClient.right;
        m_rcMainBtns.bottom = rcClient.bottom;//corrected

        m_iLinesOnscreen = (HEIGHT(m_rcDisplay) - WS_INDENT)/(m_iTextHeight + m_iSmallTextHeight) - 1; //should be about right .. gives us room for a title
    }
    else    //square or portrait
    {
        m_rcDisplay.right = rcClient.right;
#ifndef WIN32_PLATFORM_WFSP
        m_rcDisplay.bottom = HEIGHT(rcClient)/2 + rcClient.top;
#else
        if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
            m_rcDisplay.bottom = HEIGHT(rcClient)*3/4 + rcClient.top;
        else
            m_rcDisplay.bottom = HEIGHT(rcClient)/2 + rcClient.top;
#endif

        m_rcMainBtns.left = rcClient.left;
        m_rcMainBtns.top = m_rcDisplay.bottom;

        m_iLinesOnscreen = HEIGHT(m_rcDisplay)/m_iTextHeight - 2; //should be about right .. gives us room for a title
    }

    if(m_iLinesOnscreen >= m_arrEntries.GetSize())
    {
        m_iLeftTextEdge = TEXT_INDENT;
    }
    else
    {   //make room for the arrows
        m_iLeftTextEdge = TEXT_INDENT + iIndent + m_rcDisplay.left;
    }

    for(int i = 0; i < m_iLinesOnscreen; i++)
    {
        if(m_bLandScape == FALSE)//portrait ... too lazy to switch
        {
            m_rcEntries[i].top = m_rcDisplay.top + iIndent + (i+1)*m_iTextHeight;
            m_rcEntries[i].bottom = m_rcEntries[i].top + m_iTextHeight;

            m_rcEntries[i].left = m_iLeftTextEdge;
            m_rcEntries[i].right = m_rcDisplay.right - TEXT_INDENT;
        }
        else
        {
            m_rcEntries[i].top = m_rcDisplay.top + 2*iIndent + m_iTextHeight +(i)*(m_iTextHeight+m_iSmallTextHeight) + DISPLAY_INDENT;
            m_rcEntries[i].bottom = m_rcEntries[i].top + m_iTextHeight + m_iSmallTextHeight;

            m_rcEntries[i].left = m_rcDisplay.left + 3*DISPLAY_INDENT;
            m_rcEntries[i].right = m_rcDisplay.right - 3*DISPLAY_INDENT;
        }
    }
   

    //ah heck .. lets create stuff here
    CreateDisplay();
    CreateButtons();

    return TRUE;
}

void CDlgWSheet::CreateDisplay()
{
    CalcProSkins* oSkin = m_oBtnMan->GetSkin();//makes for tidier code

    RECT rcTemp = m_rcDisplay;

    rcTemp.left += DISPLAY_INDENT;
    rcTemp.right -= DISPLAY_INDENT;
    rcTemp.top += DISPLAY_INDENT;
    rcTemp.bottom -= DISPLAY_INDENT;

    m_oDisplay.Init(rcTemp, oSkin->m_typeDisplay.ButtonColor1
        , oSkin->m_typeDisplay.ButtonColor2
        , oSkin->m_typeDisplay.OutlineColor1
        , oSkin->m_typeDisplay.OutlineColor2,
        DISPLAY_Grad_DS);
    
}

BOOL CDlgWSheet::CreateButtons()
{
    for(int i = 0; i < 6; i++)
        m_pCalcBtns[i] = NULL;

    //clear out the fonts
    CIssGDIEx::DeleteFont(m_hFontMainBtnText);
    CIssGDIEx::DeleteFont(m_hFontMainLongText);

    DeleteDynBtnVector(&m_arrButtons);
    int iRows, iColumns;


    LayoutCalcType* sLayout = NULL;
    sLayout = m_oBtnMan->GetCurrentLayout();

    if(sLayout == NULL ||
        sLayout->iMainBtnColumns == 0 ||
        sLayout->iMainBtnRows == 0)
        return FALSE;

    if(m_bLandScape)
    {
        iRows = sLayout->iAltBtnRows;
        iColumns = sLayout->iAltBtnColumns;
    }
    else
    {   //smartphone we actually change the layout in the manager ...
        iRows = sLayout->iMainBtnRows;
        iColumns = sLayout->iMainBtnColumns;
    }

    
    if(m_bLandScape)
    {
        m_iButtonWidth    = (WIDTH(m_rcMainBtns) - BTN_EDGE_INDENT - (iColumns - 1)*BTN_SPACING) / iColumns;
        m_iButtonHeight   = (HEIGHT(m_rcMainBtns) - DISPLAY_INDENT - BTN_SPACING - (iRows - 1)*BTN_SPACING) / iRows;
    }
    else//portrait & square
    {
        m_iButtonWidth    = (WIDTH(m_rcMainBtns) - 2*BTN_EDGE_INDENT - (iColumns - 1)*BTN_SPACING) / iColumns;
        m_iButtonHeight   = (HEIGHT(m_rcMainBtns) - BTN_SPACING - (iRows - 1)*BTN_SPACING) / iRows;
    }

    

    CalcProSkins* oSkin = m_oBtnMan->GetSkin();
    int	iSkinIndex = 0;
    int iCurrentBtn = 0;

    RECT rcButtonSize;

    for(int j = 0; j < iRows; j++)
    {
        for(int i = 0; i < iColumns; i++)
        {
            iCurrentBtn = iColumns*j+i;

            if(iCurrentBtn == MAX_MAIN_BUTTONS)
            {    ASSERT(0);}


            if(m_bLandScape)
            {
                rcButtonSize.top	= DISPLAY_INDENT + j*(BTN_SPACING+m_iButtonHeight) + m_rcMainBtns.top;
                rcButtonSize.left	= i*(BTN_SPACING+m_iButtonWidth) + m_rcMainBtns.left;
                iSkinIndex = sLayout->sAltButtons[iCurrentBtn]->iSkinIndex;
            }
            else
            {
                rcButtonSize.top	= BTN_SPACING + j*(BTN_SPACING+m_iButtonHeight) + m_rcMainBtns.top;
                rcButtonSize.left	= BTN_EDGE_INDENT + i*(BTN_SPACING+m_iButtonWidth) + m_rcMainBtns.left;
                iSkinIndex = sLayout->sMainButtons[iCurrentBtn]->iSkinIndex;
            }
            
            
            rcButtonSize.right	= rcButtonSize.left + m_iButtonWidth;
            rcButtonSize.bottom	= rcButtonSize.top + m_iButtonHeight;
               

            CCalcDynBtn* oBtn = new CCalcDynBtn;
            if(oBtn == NULL)
                break;

            //just a helper function so we're not writing the sames code over and over ...
            //destroys the old, sets the colors, checks for a previous GDI, creates the button and saves the GDI if necessary
            //we can't assume these buttons are being created in order .... hmmmm
           // InitButton(oBtn, &rcButtonSize, oSkin, iSkinIndex, iButtonWidth, iButtonHeight);

            oBtn->Destroy();
            oBtn->SetTextColors(oSkin->m_typeBtnClr[iSkinIndex].TextColor, oSkin->m_typeBtnClr[iSkinIndex].TextColor2, oSkin->GetUseShadow());
            oBtn->InitAdvanced(oSkin->m_typeBtnClr[iSkinIndex].ButtonColor1,
                oSkin->m_typeBtnClr[iSkinIndex].ButtonColor2,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor1,
                oSkin->m_typeBtnClr[iSkinIndex].OutlineColor2);

                //init the Button
            oBtn->Init(rcButtonSize, _T(" "), m_hWnd, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, m_pCalcBtns[iSkinIndex]);//room for speed increase here
            if(m_pCalcBtns[iSkinIndex] == NULL)
                m_pCalcBtns[iSkinIndex] = oBtn;

            m_arrButtons.AddElement(oBtn);
        }
    }

    CreateFonts();

   

    return TRUE;
}

void CDlgWSheet::CreateFonts()
{
    //just in case we have an odd shaped screen
    int iTemp = min(m_iButtonWidth, m_iButtonHeight);

    //can't use the inline ifs because it effectively puts brackets around the defines which then don't evaluate correctly (integer math)
    if(GetSystemMetrics(SM_CYSCREEN) == GetSystemMetrics(SM_CXSCREEN))
    { //square
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO_SQ, FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*TEXT_LONG_RATIO_SQ, FW_BOLD, TRUE);
    }
    else if(GetSystemMetrics(SM_CYSCREEN) < GetSystemMetrics(SM_CXSCREEN))
    { //landscape
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*5/8, FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*4/9, FW_BOLD, TRUE);
    }
    else if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
    {   //ultra wide in portrait
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*5/8, FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp*3/7, FW_BOLD, TRUE);
    }
    else //portrait
    {
        if(m_hFontMainBtnText == FALSE)
            m_hFontMainBtnText = CIssGDIEx::CreateFont(iTemp*TEXT_HEIGHT_RATIO, FW_BOLD, TRUE);
        if(m_hFontMainLongText == FALSE)
            m_hFontMainLongText = CIssGDIEx::CreateFont(iTemp/2, FW_BOLD, TRUE);
    }
}

void CDlgWSheet::DeleteFonts()
{
    CIssGDIEx::DeleteFont(m_hFontMainBtnText);
    CIssGDIEx::DeleteFont(m_hFontMainLongText);
    CIssGDIEx::DeleteFont(m_hFontDisplay);
    CIssGDIEx::DeleteFont(m_hFontDisplaySm);
}

//lets blow out the button vectors
void CDlgWSheet::DeleteDynBtnVector(CIssVector<CCalcDynBtn>* arr)
{
    if(arr == NULL)
        return;

    CCalcDynBtn* oBtn = NULL;

    for(int i = 0; i < arr->GetSize(); i++)
    {
        oBtn = (*arr)[i];
        if(oBtn)
            delete oBtn;
        oBtn = NULL;	
    }
    arr->RemoveAll();
}

BOOL CDlgWSheet::InitScreenEntries()
{
    TCHAR szTemp[STR_NORMAL];

    TypeEntry* sEntry;

    for(int i = 0; i < 20; i++)
    {
        sEntry = new TypeEntry;

        m_oStr->Format(szTemp, _T("X%i"), i);
        sEntry->szEntryLabel = m_oStr->CreateAndCopy(szTemp);

        m_oStr->Format(szTemp, _T("%i"), i);

        m_oStr->StringCopy(sEntry->szEntryValue, szTemp);
   
        m_arrEntries.AddElement(sEntry);

    }

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_Worksheet), m_hInst));

    return TRUE;
}


void CDlgWSheet::SetScreenEntryFromCalcEngine()
{
    TCHAR szTemp[STR_NORMAL];
    m_oCalc->GetAnswer(szTemp);

    if(m_arrEntries[m_iEntryIndex+m_iScreenIndex] && m_arrEntries[m_iEntryIndex+m_iScreenIndex]->szEntryValue)
    {
        
        m_oStr->StringCopy(m_arrEntries[m_iEntryIndex+m_iScreenIndex]->szEntryValue, szTemp);

    }
}

void CDlgWSheet::DrawSelector(HDC hDC)
{
    if(m_iScreenIndex < 0 || 
        m_iScreenIndex >= m_iLinesOnscreen)
        return;

    RECT rcSelector = m_rcEntries[m_iScreenIndex];

    rcSelector.left -= DISPLAY_INDENT;
    rcSelector.right += DISPLAY_INDENT;
    rcSelector.right--;
    rcSelector.bottom--;

    FrameRect(hDC, rcSelector, m_oBtnMan->GetSkin()->m_crTextTop);
}

void CDlgWSheet::DrawBtnSelector(HDC hDC, RECT& rcClip)
{
    if(!m_bDrawBtnSelector)
        return;

    CCalcDynBtn* oBtn = NULL;
    oBtn = m_arrButtons[m_iSelectorIndex];

    if(oBtn == NULL)
        return;

    RECT rcTemp = oBtn->GetButtonRect();

    int iRadius = GetSystemMetrics(SM_CXICON)/6;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(hDC, GetStockObject(WHITE_PEN));
    //	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
    RoundRect(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

    SelectObject(hDC, oldBrush);
    SelectObject(hDC, oldPen);
}

void CDlgWSheet::DrawPanels(HDC hdc, RECT& rcClip)
{
    RECT rcTemp;

    if(m_bLandScape)
    {
        for(int i = 0; i < m_iLinesOnscreen; i++)
        {
            if(m_iEntryIndex + i >= m_arrEntries.GetSize())
                break;

            rcTemp = m_rcEntries[i];
            rcTemp.left -= DISPLAY_INDENT;
            rcTemp.right += DISPLAY_INDENT;

            InflateRect(&rcTemp, -1, -1);

            if((m_iEntryIndex+i)%2 == 0)
                FillRect(hdc, rcTemp, m_oBtnMan->GetSkin()->m_crWS1);
            else
                FillRect(hdc, rcTemp, m_oBtnMan->GetSkin()->m_crWS2);
        }
    }

}


BOOL CDlgWSheet::DrawScreenEntries(HDC hDC)
{
 /*   RECT rcColor = m_rcDisplay;
    rcColor.right = rcColor.left + GetSystemMetrics(SM_CXICON);
    rcColor.bottom = rcColor.top + GetSystemMetrics(SM_CXICON);

    COLORREF rcTest; 
    rcTest = RGB(GetRValue(m_oBtnMan->GetSkin()->m_crBackground)/2 + GetRValue(m_oBtnMan->GetSkin()->m_typeDisplay.ButtonColor1)/2,
        GetGValue(m_oBtnMan->GetSkin()->m_crBackground)/2 + GetGValue(m_oBtnMan->GetSkin()->m_typeDisplay.ButtonColor1)/2,
        GetBValue(m_oBtnMan->GetSkin()->m_crBackground)/2 + GetBValue(m_oBtnMan->GetSkin()->m_typeDisplay.ButtonColor1)/2);

    CIssGDIEx::FillRect(hDC, rcColor, rcTest);*/
    RECT rcTemp;

    TCHAR szTemp[STRING_MAX];

    //lets draw the memory indicator first
    if(m_oCalc->IsMemoryFilled(0))
    {
        rcTemp = m_rcDisplay;
        rcTemp.top += 2*DISPLAY_INDENT;
        rcTemp.right -= 3*DISPLAY_INDENT;
        DrawText(hDC, _T("M"), rcTemp, DT_RIGHT | DT_TOP, m_hFontDisplaySm, m_oBtnMan->GetSkin()->m_crTextTop);
    }


    COLORREF crValue;
    COLORREF crLabel;

    BOOL bArrows = FALSE;

    //Draw Scroll Arrows 
    if(m_iEntryIndex > 0)
    {
        DrawArrows(hDC, TRUE, m_rcDisplay.left+TEXT_INDENT*3/2, TEXT_INDENT);
        //Draw Up Arrow
        bArrows = TRUE;
    }

    if(m_arrEntries.GetSize() > m_iEntryIndex + m_iLinesOnscreen)
    {
        DrawArrows(hDC, FALSE, m_rcDisplay.left+TEXT_INDENT*3/2,  m_rcDisplay.bottom - 2*WS_INDENT);
        //Draw Down Arrow
        bArrows = TRUE;
    }

    //we're gonna set the screen entries here too cause its the best spot and I don't feel
    //like writing another function
    if(m_iScreenIndex != -1)
        SetScreenEntryFromCalcEngine();

    TypeEntry* sEntry = NULL;
    
    //And Draw the entries on screen
    int iCount = m_iLinesOnscreen;

    if(m_iLinesOnscreen > m_arrEntries.GetSize())
        iCount = m_arrEntries.GetSize();

    iCount = min(iCount, MAX_WS_ENTRIES);

    for(int i =0;i < iCount; i++)
    {
        sEntry = m_arrEntries[m_iEntryIndex+i];
        if(sEntry == NULL)
            return FALSE;

        if(sEntry->bGreyed)
        {
            crLabel = m_oBtnMan->GetSkin()->m_crTextTop;
            crValue = m_oBtnMan->GetSkin()->m_crTextTop;
        }
        else
        {
            crLabel = m_oBtnMan->GetSkin()->m_crText;
            crValue = m_oBtnMan->GetSkin()->m_crText;
        }

        //draw value
        m_oStr->StringCopy(szTemp, sEntry->szEntryValue);

        switch(sEntry->eEntry) 
        {
        case ENTRY_Int:
            FormatInt(szTemp);
            break;
        case ENTRY_Percent:
            FormatPercent(szTemp);
            break;
        case ENTRY_Currency:
            FormatMoney(szTemp);
            break;
        case ENTRY_Double:
        default:
            break;
        }

        SIZE sizeTemp;

        //select font for accurate calculations
        HFONT oldFont = (HFONT)SelectObject(hDC, m_hFontDisplay);

        //get the length of the value
        GetTextExtentPoint(hDC, szTemp, (int)_tcslen(szTemp), &sizeTemp);

        //find the left most point 
        int iXValue = m_rcEntries[0].right - sizeTemp.cx;

        SelectObject(hDC, m_hFontDisplaySm);
        
        //get the length of the label
        GetTextExtentPoint(hDC, sEntry->szEntryLabel, (int)_tcslen(sEntry->szEntryLabel), &sizeTemp);

        //put the font back
        SelectObject(hDC, oldFont);

        //find the right most point of the label
        int iXLabel = m_rcEntries[0].left + sizeTemp.cx;

        if(m_bLandScape)
        {
       //old method     if(iXLabel > iXValue)
            {
                //draw label
                DrawText(hDC, sEntry->szEntryLabel, m_rcEntries[i], 
                    DT_LEFT | DT_TOP, m_hFontDisplaySm, crLabel);

                //draw value
                DrawText(hDC, szTemp, m_rcEntries[i], 
                    DT_RIGHT | DT_BOTTOM, m_hFontDisplay, crValue);
            }
        }
        else
        {
            //draw label
            DrawText(hDC, sEntry->szEntryLabel, m_rcEntries[i], 
                DT_LEFT | DT_VCENTER, m_hFontDisplaySm, crLabel);

            //draw value
            DrawText(hDC, szTemp, m_rcEntries[i], 
                DT_RIGHT | DT_VCENTER, m_hFontDisplay, crValue);
        }
    }

    rcTemp = m_rcDisplay;
    rcTemp.top += 2*DISPLAY_INDENT;

    //draw the title
    DrawText(hDC, m_szTitle, rcTemp, DT_CENTER | DT_TOP, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);

    return TRUE;

}

void CDlgWSheet::FormatInt(TCHAR* szIn)
{
    if(szIn == NULL)
        return;

    if(m_oStr->Find(szIn, _T("e"), 0) != -1)
        return;

    int iDec = m_oStr->Find(szIn, _T("."), 0);
    if(iDec == -1)//its already an int
        return;

    else
        m_oStr->Delete(iDec, m_oStr->GetLength(szIn) - iDec, szIn);

}

void CDlgWSheet::FormatPercent(TCHAR* szIn)
{
#define MAX_LENGTH_PER 11

    if(szIn == NULL)
        return;

    if(szIn[m_oStr->GetLength(szIn)-1]==_T('.'))
        m_oStr->Delete(m_oStr->GetLength(szIn)-1, 1, szIn);

    if(m_oStr->GetLength(szIn) > MAX_LENGTH_PER)
        m_oStr->Delete(MAX_LENGTH_PER, m_oStr->GetLength(szIn)-MAX_LENGTH_PER, szIn);


    m_oStr->Concatenate(szIn, _T("%"));
}

void CDlgWSheet::FormatMoney(TCHAR* szIn)
{
    if(szIn == NULL)
        return;

    if(m_oStr->Find(szIn, _T("e"), 0) != -1)
        return;

    int iDec = m_oStr->Find(szIn, _T("."), 0);
    int iLen = m_oStr->GetLength(szIn);
    if(iDec == -1)//its already an int
        m_oStr->Concatenate(szIn, _T(".00")); //unlikely case
    else if(iDec == iLen - 1)
        m_oStr->Concatenate(szIn, _T("00")); //unlikely case
    else if(iDec == iLen - 2)
        m_oStr->Concatenate(szIn, _T("0")); //unlikely case
    else if(iDec == iLen - 3)
        return; //jackpot
    //and finally....chop some decimal places off...no I'm not rounding...
    else	
        m_oStr->Delete(iDec + 3, iLen - iDec - 3, szIn);
}

BOOL CDlgWSheet::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oHandBtns->OnChar(hWnd, wParam, lParam, m_oCalc, m_oBtnMan))
    {
        InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
        return TRUE;
    }
    return FALSE;
}

BOOL CDlgWSheet::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oHandBtns->OnKeyDown(hWnd, wParam, lParam, m_oCalc, m_oBtnMan))
    {
        InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
        return TRUE;
    }
    return FALSE;
}

BOOL CDlgWSheet::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //Now for our up and down we really only need on KeyUp
    //But I passed through both just in case
    switch(LOWORD(wParam))
    {
    case VK_TAB:
    case VK_DOWN:
        //I'm just gonna put the code here....TS
        if(m_iEntryIndex + m_iScreenIndex + 2> m_arrEntries.GetSize())
            break;
        if(m_iScreenIndex == m_iLinesOnscreen - 1)
            m_iEntryIndex++;
        else
            m_iScreenIndex++;
       m_oCalc->AddString(m_arrEntries[m_iScreenIndex+m_iEntryIndex]->szEntryValue);
        break;
    case VK_UP:
        if(m_iEntryIndex + m_iScreenIndex == 0)
            break;
        if(m_iScreenIndex > 0)
            m_iScreenIndex--;
        else 
            m_iEntryIndex--;
        m_oCalc->AddString(m_arrEntries[m_iScreenIndex+m_iEntryIndex]->szEntryValue);
        break;
#ifdef WIN32_PLATFORM_WFSP
    case 120: //#
        OnCalc();
        break;
    case VK_LEFT:
        m_iSelectorIndex--;
        if(m_iSelectorIndex < 0)
            m_iSelectorIndex = m_arrButtons.GetSize() - 1;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case VK_RIGHT:
        m_iSelectorIndex++;
        if(m_iSelectorIndex >= m_arrButtons.GetSize())
            m_iSelectorIndex = 0;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case VK_RETURN:
        if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
            break;

        if(m_oBtnMan == NULL || 
            m_oBtnMan->GetCurrentLayout()->sMainButtons[m_iSelectorIndex] == NULL)
            break;

        PostMessage(hWnd, WM_WORKSHEET, m_oBtnMan->GetCurrentLayout()->sMainButtons[m_iSelectorIndex]->iButtonType, 
            m_oBtnMan->GetCurrentLayout()->sMainButtons[m_iSelectorIndex]->iButtonValue);
        break;
    case VK_BACK:
        break;
#else
  /*  case VK_RETURN://make this behave like enter
        OnCalc();
        break;*/
#endif
    case VK_TSOFT1:
        SafeCloseWindow(0);
        Destroy();
        break;
    default:
        if(m_oHandBtns->OnKeyUp(hWnd, wParam, lParam, m_oCalc, m_oBtnMan))
        {
            InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
            return TRUE;
        }
        break;
    }

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}

void CDlgWSheet::OnBtnSetBtnPressed()
{
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);//REDRAW baby
}

BOOL CDlgWSheet::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_LEFT_SOFTKEY || uiMessage == WM_ICON_SOFTKEY)
    {
        SafeCloseWindow(0);
        Destroy();
    }

    //handle for smartphone - TEST!!!
    if(uiMessage == WM_WORKSHEET || uiMessage == WM_CALCENGINE)
    {
        if(wParam == INPUT_WS)
        {
            switch(lParam)
            {
            case WS_Up:
                PostMessage(m_hWnd, WM_KEYUP, VK_UP, 0);
            	break;
            case WS_Down:
                PostMessage(m_hWnd, WM_KEYUP, VK_DOWN, 0);
            	break;
            case WS_Back:
                SafeCloseWindow(0);
                Destroy();
                break;
            case WS_Comp:
                OnCalc();
                break;
            case WS_Graph:
                OnGraph();
                break;
            case WS_Load:
                OnLoad();
                break;
            case WS_Save:
                OnSave();
                break;
            default:
                break;
            }
            return TRUE;
        }
        else if(wParam == INPUT_Clear)
        {
            if(lParam == CLEAR_Reset)
            {
                OnReset();
                return TRUE;
            }
        }
        else if(wParam == INPUT_Graph)
        {
            OnGraph();
            return TRUE;
        }
        if(m_arrEntries[m_iEntryIndex+m_iScreenIndex] &&
            m_arrEntries[m_iEntryIndex+m_iScreenIndex]->bGreyed == FALSE)
            m_oCalc->CalcButtonPress((DWORD)wParam, (DWORD)lParam);
        return TRUE;
    }

    return UNHANDLED;
}

BOOL CDlgWSheet::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDMENU_Back 
#ifndef WIN32_PLATFORM_WFSP        
        || LOWORD(wParam) == IDOK
#endif
        )
    {
        SafeCloseWindow(0);
        Destroy();
        return TRUE;
    }

    return UNHANDLED;
}

BOOL CDlgWSheet::OnCalc()
{
    //lets do a little animation cause we're hardcore
    //flash the selector
    int iTemp = m_iScreenIndex;
    
    m_iScreenIndex = -1;
    RedrawWindow(m_hWnd, &m_rcDisplay, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    Sleep(25);
    m_iScreenIndex = iTemp;
    RedrawWindow(m_hWnd, &m_rcDisplay, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    Sleep(25);
    m_iScreenIndex = -1;
    RedrawWindow(m_hWnd, &m_rcDisplay, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    Sleep(25);
    m_iScreenIndex = iTemp;
    RedrawWindow(m_hWnd, &m_rcDisplay, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    return TRUE;
}


void CDlgWSheet::DrawArrows(HDC hdc, BOOL bUp, int iX, int iY)
{
    //Draw a down arrow

    POINT pt[7];

    int iScale = GetSystemMetrics(SM_CXICON)/32;

    int iWidth			= 12*iScale;
    int iWidthOfStick	= 4*iScale;
//    int iHeight			= 17*iScale;
//    int iHeightOfStick	= 11*iScale;

    int iHeight			= 6*iScale;
    int iHeightOfStick	= 0*iScale;

    if(bUp)
    {
        pt[0].x = iX - iWidthOfStick/2;
        pt[0].y = iY + iHeight;

        pt[1].x = pt[0].x + iWidthOfStick;
        pt[1].y = pt[0].y;

        pt[2].x = pt[1].x;
        pt[2].y = pt[1].y - iHeightOfStick;

        pt[3].x = pt[1].x + iWidth/2 - iWidthOfStick/2;
        pt[3].y = pt[2].y;

        pt[4].x = pt[0].x + iWidthOfStick/2;
        pt[4].y = pt[2].y - iHeight + iHeightOfStick;

        pt[5].x = pt[3].x - iWidth;
        pt[5].y = pt[3].y;

        pt[6].x = pt[0].x;
        pt[6].y = pt[3].y;	
    }
    else
    {
        pt[0].x = iX - iWidthOfStick/2;
        pt[0].y = iHeight/2 + iY;

        pt[1].x = pt[0].x + iWidthOfStick;
        pt[1].y = pt[0].y;

        pt[2].x = pt[1].x;
        pt[2].y = pt[1].y + iHeightOfStick;

        pt[3].x = pt[1].x + iWidth/2 - iWidthOfStick/2;
        pt[3].y = pt[2].y;

        pt[4].x = pt[0].x + iWidthOfStick/2;
        pt[4].y = pt[2].y + iHeight - iHeightOfStick;

        pt[5].x = pt[3].x - iWidth;
        pt[5].y = pt[3].y;

        pt[6].x = pt[0].x;
        pt[6].y = pt[3].y;
    }

    HPEN   hpNew			= CreatePen(PS_SOLID, 1, m_oBtnMan->GetSkin()->m_crText);
    HPEN   hpOld			= (HPEN)SelectObject(hdc, hpNew);

    HBRUSH brNew			= CreateSolidBrush(m_oBtnMan->GetSkin()->m_crTextTop);
    HBRUSH brOld			= (HBRUSH)SelectObject(hdc, brNew);

    Polygon(hdc, pt, 7);

    SelectObject(hdc, hpOld);
    DeleteObject(hpNew);

    SelectObject(hdc, brOld);
    DeleteObject(brNew);
}

void CDlgWSheet::PlaySounds()
{
    if(m_bPlaySounds == 1)
        PlaySound(_T("IDR_WAVE_TICK"), m_hInst,  SND_RESOURCE | SND_SYNC);
    else if(m_bPlaySounds == 2)
        PlaySound(_T("IDR_WAVE_TICKSOFT"), m_hInst,  SND_RESOURCE | SND_SYNC);
}

BOOL CDlgWSheet::OnReset()
{
    //default is nothing
    return FALSE;
}

BOOL CDlgWSheet::OnGraph()
{
    //again .. nothing
    return FALSE;
}

BOOL CDlgWSheet::OnLoad()
{
    return FALSE;
}

BOOL CDlgWSheet::OnSave()
{
    return FALSE;
}



BOOL CDlgWSheet::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE)
    {	
#ifdef WIN32_PLATFORM_WFSP
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
#else 
        if(m_oBtnMan->GetFullscreen())
        {
            SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
            RECT rc; 
            SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
            MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);           
        }
        else
        {
            SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON);  
        }
       
        //get rid of the damn sip too ... just in case
        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL);
        if(hWndSip)
            ShowWindow(hWndSip, SW_HIDE);
#endif
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
        //        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL);
        //      ShowWindow(hWndSip, SW_SHOW);
    }

#endif
    return bRet;
}

int CDlgWSheet::GetSystemMetrics(__in int nIndex)
{
    return GetSysMets(nIndex);
    /*
#ifdef UNDER_CE
	return ::GetSystemMetrics(nIndex);
#else
	switch(nIndex)
	{
	case SM_CXSMICON:
		return 32;
		break;
	case SM_CXICON:
		return 64;
		break;
	case SM_CXSCREEN:
		return WINDOW_WIDTH;
		break;
	case SM_CYSCREEN:
		return WINDOW_HEIGHT;
		break;
	default:
		return ::GetSystemMetrics(nIndex);
		break;
	}
#endif*/
}

BOOL CDlgWSheet::OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	SetWindowPos(m_hWndParent, NULL, rcWindow.left, rcWindow.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	return TRUE;
}