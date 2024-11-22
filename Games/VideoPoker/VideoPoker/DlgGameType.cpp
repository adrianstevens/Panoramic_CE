#include "DlgGameType.h"
#include "ObjGui.h"
#include "Resource.h"
#include "IssCommon.h"

extern CObjGui* g_gui;

#define IDMENU_SelectItem               5000

#define SELECTOR_EXPANSE (IsVGA()?10:5)

CIssKineticList*    CDlgGameType::m_oMenu = NULL;
BOOL                CDlgGameType::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD               CDlgGameType::m_dwFullListCount = 0;


CDlgGameType::CDlgGameType(void)
{
    if(!m_oMenu)
        m_oMenu = new CIssKineticList;
    m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*15/32, FW_BOLD, TRUE);
    m_bGameInProgress = FALSE;
    m_eGameType = PTYPE_10sOrBetter;
}

CDlgGameType::~CDlgGameType(void)
{
    CIssGDIEx::DeleteFont(m_hFontText);

	if(m_oMenu)
		delete m_oMenu;
	m_oMenu = NULL;
}

void CDlgGameType::Init(BOOL bGameInProgress, EnumPokerType eGame)
{
    m_bGameInProgress = bGameInProgress; 
    m_eGameType = eGame;

    if(m_oMenu->GetItemCount() != 0)
        m_oMenu->SetSelectedItemIndex(m_eGameType, TRUE);

};


BOOL CDlgGameType::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
     HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);
#endif


    if(g_gui)
        g_gui->Init(hWnd, m_hInst);

    //list options ... gonna keep it pretty minimal
    DWORD dwFlags   = OPTION_Bounce | OPTION_CircularList;

    // do other menu initialization here
    m_oMenu->Initialize(hWnd, hWnd, m_hInst, dwFlags, TRUE);

    if(m_oMenu->GetItemCount() == 0)
    {
        m_oMenu->SetSelectedItemIndex(0, TRUE);
        m_oMenu->SetCustomDrawFunc(DrawListItem, this);
        m_oMenu->SetDeleteItemFunc(DeleteListItem);

        m_oMenu->ResetContent();
        PopulateList();
    }

    ::SetCursor(hCursor);

    return TRUE;
}



void CDlgGameType::PopulateList()
{
    if(!m_oMenu)
        return;

    //just tell it we have 6 things ...
    for(int i = 0; i < 6; i++)
    {
        m_oMenu->AddItem(LPVOID(IDMENU_SelectItem + i), IDMENU_SelectItem + i, 0, 0);
    }

    //should default selector too
    m_oMenu->SetSelectedItemIndex(m_eGameType, TRUE);
}



void CDlgGameType::LoadImages()
{
    if(m_gdiGames[0].GetDC() != NULL)
        return;

    CIssGDIEx gdiTemp;
    gdiTemp.LoadImage(IDB_PNG_MenuSep, m_hWnd, m_hInst);

    RECT rcTemp;
    rcTemp.left = 0;
    rcTemp.top = 0;
    rcTemp.right = GetSystemMetrics(SM_CXSCREEN) - 2*SELECTOR_EXPANSE;
    rcTemp.bottom = rcTemp.top + gdiTemp.GetHeight();

    SIZE szTemp;
    szTemp.cx = WIDTH(rcTemp);
    szTemp.cy = HEIGHT(rcTemp);

    //now scale 
    m_gdiSeparator.Create(gdiTemp.GetDC(), rcTemp, FALSE, TRUE);
    TileBackGround(m_gdiSeparator, rcTemp, gdiTemp, FALSE);


    //	ScaleImage(gdiTemp, m_gdiSeparator, szTemp, TRUE, 0);



    //matches the enum ... too lazy to copy/paste
    m_gdiGames[0].LoadImage(IsVGA()?IDB_PNG_Game10s_HR:IDB_PNG_Game10s, m_hWnd, m_hInst);
    m_gdiGames[1].LoadImage(IsVGA()?IDB_PNG_GameJacks_HR:IDB_PNG_GameJacks, m_hWnd, m_hInst);
    m_gdiGames[2].LoadImage(IsVGA()?IDB_PNG_GameBonus_HR:IDB_PNG_GameBonus, m_hWnd, m_hInst);
    m_gdiGames[3].LoadImage(IsVGA()?IDB_PNG_GameAces_HR:IDB_PNG_GameAces, m_hWnd, m_hInst);
    m_gdiGames[4].LoadImage(IsVGA()?IDB_PNG_GameDouble_HR:IDB_PNG_GameDouble, m_hWnd, m_hInst);
    m_gdiGames[5].LoadImage(IsVGA()?IDB_PNG_GameStud_HR:IDB_PNG_GameStud, m_hWnd, m_hInst);

    m_imgSelector.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_Selector_HR:IDB_PNG_Selector);
    m_imgSelector.SetSize(GetSystemMetrics(SM_CXSCREEN), m_gdiGames->GetHeight()+2*SELECTOR_EXPANSE);

    m_oMenu->SetItemHeights(m_gdiGames[0].GetHeight() + 2*SELECTOR_EXPANSE, m_gdiGames[0].GetHeight()+ 2*SELECTOR_EXPANSE);
    //m_oMenu->SetItemHeights(m_gdiGames[0].GetHeight(), m_gdiGames[0].GetHeight()+ SELECTOR_EXPANSE);
}

BOOL CDlgGameType::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);
    LoadImages();

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    // draw the background
    g_gui->DrawBackground(rcClip);

    if(m_oMenu->GetItemCount() == 0)
    {
        DrawTextShadow(g_gui->gdiMem, _T("No Games Found"), rcClient, DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255), 0);
    }
    else
    {
        m_oMenu->OnDraw(g_gui->gdiMem, rcClient, rcClip);
    }

    if(m_bGameInProgress)
        g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, _T("Select"), _T("Cancel"));
    else
        g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, _T("Select"), NULL);

    // draw all to the screen
    g_gui->DrawScreen(rcClip, hDC);

    return TRUE;
}

BOOL CDlgGameType::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(!m_oMenu)
        return TRUE;

    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgGameType::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) >= IDMENU_SelectItem && LOWORD(wParam) < IDMENU_SelectItem + 10)
    {
        int iRet = LOWORD(wParam) - IDMENU_SelectItem;
        SafeCloseWindow(iRet);	// let the main window handle this
        return TRUE;
    }

    switch(LOWORD(wParam))
    {
    case IDOK://return pressed on menu ... gotta  get the index
    case IDMENU_Left:
        {
            int iRet = m_oMenu->GetSelectedItemIndex();//should be fine
            SafeCloseWindow(LOWORD(iRet));	// let the main window handle this
        }
        break;
    case IDMENU_Right:
        if(m_bGameInProgress)
            SafeCloseWindow(LOWORD(-1));	// let the main window handle this
        break;   
    default:
        return UNHANDLED;
    }
    return TRUE;

}

BOOL CDlgGameType::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if(m_oMenu)
        m_oMenu->OnSize(rcClient.left,rcClient.top, WIDTH(rcClient), HEIGHT(rcClient));


    return TRUE;
}


BOOL CDlgGameType::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu->OnKeyDown(wParam, lParam);
}

BOOL CDlgGameType::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_TSOFT1:
        {
            int iRet = m_oMenu->GetSelectedItemIndex();//should be fine
            SafeCloseWindow(LOWORD(iRet));	// let the main window handle this
        }
        break;
    case VK_TSOFT2:
        if(m_bGameInProgress)
            SafeCloseWindow(LOWORD(-1));	// let the main window handle this
        break;
    case VK_ESCAPE:
        SafeCloseWindow(0);
        break;

    }
    return TRUE;
}

BOOL CDlgGameType::OnLButtonDown(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonDown(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return TRUE;
    return m_oMenu->OnLButtonDown(pt);
}

BOOL CDlgGameType::OnLButtonUp(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonUp(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return TRUE;
    return m_oMenu->OnLButtonUp(pt);
}

BOOL CDlgGameType::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return TRUE;
    return m_oMenu->OnMouseMove(pt);
}

BOOL CDlgGameType::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

        return TRUE; 
    }
    return UNHANDLED;
}

BOOL CDlgGameType::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}








void CDlgGameType::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{

    if(!sItem)
        return;

    int iIndex = sItem->uiMenuID - IDMENU_SelectItem;

    CIssGDIEx* gdiGame = &m_gdiGames[iIndex];

    if(gdiGame == NULL)
        return;

    Draw(gdi, rcDraw.left+ GetSystemMetrics(SM_CXICON), rcDraw.top+SELECTOR_EXPANSE, gdiGame->GetWidth(), gdiGame->GetHeight(), *gdiGame);

    if(iIndex < 5)
        Draw(gdi, rcDraw.left + SELECTOR_EXPANSE, rcDraw.bottom - m_gdiSeparator.GetHeight(), m_gdiSeparator.GetWidth(), m_gdiSeparator.GetHeight(),m_gdiSeparator);

    if(bIsHighlighted)
    {
        //draw the selector
        m_imgSelector.DrawImage(gdi, rcDraw.left, rcDraw.top);

    }
}

void CDlgGameType::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgGameType* pList = (CDlgGameType*)lpClass;
    pList->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}


void CDlgGameType::DeleteListItem(LPVOID lpItem)
{

}
