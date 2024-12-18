#include "StdAfx.h"
#include "DlgSudoku.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "DlgMsgBox.h"
#include "DlgSplashScreen.h"
#include "DlgAbout.h"
#include "SudokuGlobals.h"
#include "DlgOptions.h"
#include "DlgStats.h"
#include "IssLocalisation.h"
#include "DlgMoreApps.h"

#define REG_PS			_T("PlaySounds")
#define REG_BG			_T("Background")
#define REG_CB          _T("CustomBackground")
#define REG_CG          _T("GameType")
#define REG_AP          _T("AutoPencil")
#define REG_ARP         _T("AutoRemovePencil")
#define REG_SH          _T("ShowHints")
#define REG_DH          _T("DrawHilights")
#define REG_DB          _T("DrawBlank")
#define REG_SE          _T("ShowErrors")
#define REG_LC          _T("LockCompleted")


#define IDT_Time_Timer  1

#define HINT_PENALTY    15 //in seconds


#define GRID_INDENT     (IsVGA()?8:4)
#define GRID_SPACING    (IsVGA()?2:1)
#define SECTION_SPACING (IsVGA()?10:5)

extern CObjGui* g_gui;

CDlgSudoku::CDlgSudoku(void)
:m_hFontText(NULL)
,m_bShowSplashScreen(FALSE)
,m_bMessageBoxUp(FALSE)
,m_iTimer(0)
,m_bSelectValues(FALSE)
,m_bPenMode(TRUE)
,m_iButtonUp(0)//nothing
,m_iMenuHighlight(-1)
,m_bShowSolution(FALSE)
{
    m_oStr->Empty(m_szTime);
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    // create our fonts ... should be find on 128 DPI
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
    m_hFontPencil       	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*11/32, FW_NORMAL, TRUE);
    m_hFontTotals           = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*12/32, FW_BOLD, TRUE);
    m_hFontTitle            = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*17/32, FW_BOLD, TRUE);

    // read values from the registry
    DWORD dwVal	= 0;

    if(S_OK == GetKey(REG_KEY, REG_PS, dwVal))
        m_sOptions.bPlaySounds = (BOOL)dwVal;
    else
        m_sOptions.bPlaySounds = TRUE;

    if(S_OK == GetKey(REG_KEY, REG_DH, dwVal))
        m_sOptions.bShowHighlights = (BOOL)dwVal;
    else
        m_sOptions.bShowHighlights = FALSE;

    if(S_OK == GetKey(REG_KEY, REG_DB, dwVal))
        m_sOptions.bDrawBlank = (BOOL)dwVal;
    else
        m_sOptions.bDrawBlank = FALSE;

    if(S_OK == GetKey(REG_KEY, REG_BG, dwVal))
        m_sOptions.eSkin = (EnumSudokuSkins)dwVal;
    else
        m_sOptions.eSkin = SSKIN_Glass;

    if(S_OK == GetKey(REG_KEY, REG_CB, dwVal))
        m_sOptions.eBackground = (EnumBackground)dwVal;
    else
        m_sOptions.eBackground = BACKGROUND_Default;

    if(S_OK == GetKey(REG_KEY, REG_AP, dwVal))
        m_oGame.SetAutoPencilMarks(BOOL(dwVal));
    else
        m_oGame.SetAutoPencilMarks(FALSE);
    m_sOptions.bAutoRemovePencil = m_oGame.GetAutoPencilMarks();

    if(S_OK == GetKey(REG_KEY, REG_ARP, dwVal))
        m_oGame.SetAutoRemovePencil(BOOL(dwVal));
    else
        m_oGame.SetAutoRemovePencil(FALSE);
    m_sOptions.bAutoRemovePencil = m_oGame.GetAutoRemovePencil();

    if(S_OK == GetKey(REG_KEY, REG_SH, dwVal))
        m_oGame.SetShowHints(BOOL(dwVal));
    else
        m_oGame.SetShowHints(FALSE);
    m_sOptions.bShowHints = m_oGame.GetShowHints();
    
    if(S_OK == GetKey(REG_KEY, REG_SE, dwVal))
        m_sOptions.bShowErrors = (BOOL(dwVal));
    else
        m_sOptions.bShowErrors = TRUE;

    if(S_OK == GetKey(REG_KEY, REG_LC, dwVal))
        m_sOptions.bShowComplete = (BOOL)dwVal;
    else
        m_sOptions.bShowComplete = TRUE;
    //don't forget to tell the engine
    m_oGame.SetShowLocked(m_sOptions.bShowComplete);

    if(S_OK == GetKey(REG_KEY, REG_CG, dwVal))
        m_oGame.SetGameType((EnumSudokuType)dwVal);

    TCHAR szTemp[MAX_PATH];
    if(S_OK == GetKey(REG_KEY, _T("Path"), szTemp, MAX_PATH))
    {
        g_gui->SetImageBackground(szTemp);
        m_oStr->StringCopy(m_sOptions.szPath, szTemp);
    }
    else
    {
        m_oStr->Empty(m_sOptions.szPath);
    }
    
    //set the gui but we'll load it later
    g_gui->sSkin.eSkin = m_sOptions.eSkin;
    g_gui->sSkin.eBackground = m_sOptions.eBackground;

    ResetSelector();
}

CDlgSudoku::~CDlgSudoku(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontPencil);
	CIssGDIEx::DeleteFont(m_hFontTitle);
    CIssGDIEx::DeleteFont(m_hFontTotals);

    KillTimer(m_hWnd, IDT_Time_Timer);

    SaveRegistry();
}

void CDlgSudoku::SaveRegistry()
{
    // write values to the registry
    DWORD dwVal	= (DWORD)m_sOptions.bPlaySounds;
    SetKey(REG_KEY, REG_PS, dwVal);
    dwVal	    = (DWORD)m_sOptions.bShowHighlights;
    SetKey(REG_KEY, REG_DH, dwVal);
    dwVal	    = (DWORD)m_sOptions.bDrawBlank;
    SetKey(REG_KEY, REG_DB, dwVal);
    dwVal	    = (DWORD)m_sOptions.bShowErrors;
    SetKey(REG_KEY, REG_SE, dwVal);
    dwVal	    = (DWORD)m_sOptions.bShowComplete;
    SetKey(REG_KEY, REG_LC, dwVal);
    dwVal		= (DWORD)g_gui->sSkin.eSkin;//this is really the only one we care about
    SetKey(REG_KEY, REG_BG, dwVal);
    dwVal	    = (DWORD)g_gui->sSkin.eBackground;
    SetKey(REG_KEY, REG_CB, dwVal);
    dwVal	    = (DWORD)m_oGame.GetGameType();
    SetKey(REG_KEY, REG_CG, dwVal);
    dwVal	    = (DWORD)m_oGame.GetAutoPencilMarks();
    SetKey(REG_KEY, REG_AP, dwVal);
    dwVal	    = (DWORD)m_sOptions.bAutoRemovePencil;
    SetKey(REG_KEY, REG_ARP, dwVal);
    dwVal	    = (DWORD)m_oGame.GetShowHints();
    SetKey(REG_KEY, REG_SH, dwVal);

    TCHAR szTemp[MAX_PATH];
    g_gui->GetImagePath(szTemp);
    SetKey(REG_KEY, _T("Path"), szTemp, MAX_PATH);

}

void CDlgSudoku::CalculateLayout()
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    //we'll need to do this properly for other orientations but good enough for now
    int iTopIndent = 0;
    int iBottomIndent = 0;
    int iLeftIndent = 0;
    int iRightIndent = 0;

    int iGridSize = m_oGame.GetGridSize();
    int iHeight = (IsVGA()?78:38);//height of button

    if(WIDTH(rcClient) >= HEIGHT(rcClient))
    {
        iTopIndent = GetSystemMetrics(SM_CXSMICON);
        iBottomIndent = iHeight - (IsVGA()?2:1);//make room for our buttons
        iLeftIndent = GRID_INDENT;
        iRightIndent = (IsVGA()?68:34);//GetSystemMetrics(SM_CXICON)*3/2;
    }
    else
    {
        iTopIndent = GetSystemMetrics(SM_CXSMICON);
        iBottomIndent = (IsVGA()?96:48);//GetSystemMetrics(SM_CXICON)*3/2;
        iLeftIndent = GRID_INDENT;
        iRightIndent = GRID_INDENT;
    }

    //default for sudoku
    int iGrid = GRID_SPACING;
    int iSection = SECTION_SPACING;

    if(m_oGame.GetGameType() != STYPE_Sudoku)
        //|| g_gui->sSkin.eSkin == SSKIN_Notepad)
    {
        iSection = 0;
        iGrid = 0;
    }

    int iXSize = (WIDTH(rcClient) - iLeftIndent - iRightIndent - 2*iSection - (iGridSize-3)*iGrid)/iGridSize;
    int iYSize = (HEIGHT(rcClient) - iTopIndent - iBottomIndent - 2*iSection - (iGridSize-3)*iGrid)/iGridSize;

    int iSize = min(iXSize, iYSize);

    int iX = iLeftIndent;
    if(GetSystemMetrics(SM_CYSCREEN) <= GetSystemMetrics(SM_CXSCREEN))
        iX = (WIDTH(rcClient) - iRightIndent - 2*iSection - (iGridSize-3)*iGrid - iGridSize*iSize)/2;

    int iOverSize = 0;
    if(m_sOptions.eSkin > SSKIN_Grey)
        iOverSize = 1;

    for(int x = 0; x < iGridSize; x++)
    {
        int iY = iTopIndent;//reset it each column
        for(int y = 0; y < iGridSize; y++)
        {
            SetRect(&m_rcGrid[x][y], iX, iY, iX + iSize + iOverSize, iY + iSize + iOverSize);

            if(y == 2 || y == 5)
                iY += iSection;
            else
                iY += iGrid;
            iY += iSize;
        }

        if(x == 2 || x == 5)
            iX += iSection;
        else
            iX += iGrid;
        iX += iSize;
    }

    //tell the gui
    g_gui->SetCellSize(m_rcGrid[0][0], m_oGame.GetGridSize(), m_oGame.GetGameType());

    //entry values
    iLeftIndent = GRID_INDENT;
    iRightIndent = GRID_INDENT;

    int iIndent;

    //Top Text
    SetRect(&m_rcTime,      m_rcGrid[0][0].left, 0, m_rcGrid[m_oGame.GetGridSize()-1][0].right, iTopIndent);
    SetRect(&m_rcRemaining, m_rcGrid[0][0].left, 0, m_rcGrid[m_oGame.GetGridSize()-1][0].right, iTopIndent);

    if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
    {
        //rough button positions
        iSize = (WIDTH(rcClient) - 5*GRID_SPACING)/4;

        SetRect(&m_rcBottomBar, rcClient.left, rcClient.bottom - (IsVGA()?68:34), rcClient.right, rcClient.bottom);
        //tight on the menu
        SetRect(&m_rcUndo, rcClient.left + GRID_SPACING, m_rcBottomBar.top, GRID_SPACING + iSize,m_rcBottomBar.bottom);
        SetRect(&m_rcHint, m_rcUndo.right + GRID_SPACING, m_rcUndo.top, m_rcUndo.right + GRID_SPACING + iSize, m_rcUndo.bottom);
        SetRect(&m_rcPencil, m_rcHint.right + GRID_SPACING, m_rcUndo.top, m_rcHint.right + GRID_SPACING + iSize, m_rcUndo.bottom);
        SetRect(&m_rcMenu, m_rcPencil.right + GRID_SPACING, m_rcUndo.top, m_rcPencil.right + GRID_SPACING + iSize, m_rcUndo.bottom);

        if(m_gdiIconGlow.GetDC())
        {
            m_ptGlowOffset.x = (WIDTH(m_rcUndo) - m_gdiIconGlow.GetWidth())/2;
            m_ptGlowOffset.y = (HEIGHT(m_rcUndo) - m_gdiIconGlow.GetHeight())/2;
        }

        iSize = WIDTH(rcClient)/9;

        if(iSize*9 == WIDTH(rcClient))
        {   //never gonna happen
            iIndent = 0;
        }
        else
        {
            iSize++;
            iIndent = (WIDTH(rcClient) - iSize*9)/2;

            iIndent = (WIDTH(rcClient) - 9*iSize)/2;//will be negative
        }
       

        if(m_imgBtn.IsInitialized())
            m_imgBtn.SetSize(iSize, iHeight);
        if(m_imgBtnBack.IsInitialized())
            m_imgBtnBack.SetSize(iSize, iHeight);
        
        for(int i = 0; i < 9; i++)
            SetRect(&m_rcValues[i], iIndent + iSize*i,
            m_rcBottomBar.top - iHeight,
            iIndent + iSize*i + iSize,
            m_rcBottomBar.top);

        m_rcGameBoard.left = min(m_rcGrid[0][0].left, m_rcValues[0].left);
        m_rcGameBoard.top  = m_rcGrid[0][0].top; //gonna need to update the text up top too
        m_rcGameBoard.right= rcClient.right;//m_rcGrid[m_oGame.GetGridSize() -1][m_oGame.GetGridSize() -1].right;
        m_rcGameBoard.bottom= m_rcValues[0].bottom;//m_rcGrid[m_oGame.GetGridSize() -1][m_oGame.GetGridSize() -1].bottom;

        if(m_imgTop.IsInitialized())
            m_imgTop.SetSize(WIDTH(m_rcBottomBar), iTopIndent);

    }
    else //square and landscape get the menu on the side
    {
        //rough button positions
        iSize = (HEIGHT(rcClient) - 5*GRID_SPACING)/4;

        SetRect(&m_rcBottomBar, rcClient.right - (IsVGA()?68:34), rcClient.top, rcClient.right, rcClient.bottom);
        
        SetRect(&m_rcUndo, m_rcBottomBar.left, rcClient.top + GRID_SPACING, m_rcBottomBar.right, rcClient.top + GRID_SPACING + GRID_SPACING + iSize);
        SetRect(&m_rcHint, m_rcUndo.left, m_rcUndo.bottom + GRID_SPACING, m_rcUndo.right, m_rcUndo.bottom + GRID_SPACING + iSize);
        SetRect(&m_rcPencil, m_rcUndo.left, m_rcHint.bottom + GRID_SPACING, m_rcUndo.right, m_rcHint.bottom + GRID_SPACING + iSize);
        SetRect(&m_rcMenu, m_rcUndo.left, m_rcPencil.bottom + GRID_SPACING, m_rcUndo.right, m_rcPencil.bottom + GRID_SPACING + iSize);

        if(m_gdiIconGlow.GetDC())
        {
            m_ptGlowOffset.x = (WIDTH(m_rcUndo) - m_gdiIconGlow.GetWidth())/2;
            m_ptGlowOffset.y = (HEIGHT(m_rcUndo) - m_gdiIconGlow.GetHeight())/2;
        }

        iHeight = WIDTH(m_rcBottomBar);
        iSize = (WIDTH(rcClient) - iHeight)/9;
  //      iIndent = (WIDTH(rcClient) - iSize*9 - iHeight)/2;
        if(iSize*9 == (WIDTH(rcClient) - iHeight)/9)
        {   //never gonna happen
            iIndent = 0;
        }
        else
        {
            iSize++;
            iIndent = (WIDTH(rcClient) - iHeight - iSize*9)/2;
        }

        
        if(m_imgBtn.IsInitialized())
            m_imgBtn.SetSize(iSize, iHeight);
        if(m_imgBtnBack.IsInitialized())
            m_imgBtnBack.SetSize(iSize, iHeight);
                
        for(int i = 0; i < 9; i++)
            SetRect(&m_rcValues[i], iIndent + iSize*i,
                rcClient.bottom - iHeight,
                iIndent + iSize*i + iSize,
                rcClient.bottom);
        
        m_rcGameBoard.left = min(m_rcGrid[0][0].left, m_rcValues[0].left);
        m_rcGameBoard.top  = m_rcGrid[0][0].top;
        m_rcGameBoard.right= m_rcMenu.left;
        m_rcGameBoard.bottom= m_rcValues[0].bottom;

        if(m_imgTop.IsInitialized())
            m_imgTop.SetSize(m_rcMenu.left - rcClient.left, iTopIndent);
    }

    if(m_imgMenu.IsInitialized())
        m_imgMenu.SetSize(WIDTH(m_rcBottomBar), HEIGHT(m_rcBottomBar));

    m_rcNumButtons.left = m_rcValues[0].left;
    m_rcNumButtons.right = m_rcValues[8].right;
    m_rcNumButtons.top = m_rcValues[0].top - HEIGHT(m_rcValues[0]);
    m_rcNumButtons.bottom = m_rcValues[0].bottom;

    //corrections since I fudged the numbers off screen
    if(m_rcNumButtons.left < 0)
        m_rcNumButtons.left = 0;
    if(m_rcNumButtons.right > GetSystemMetrics(SM_CXSCREEN))
        m_rcNumButtons.right = GetSystemMetrics(SM_CXSCREEN);

    if(m_rcGameBoard.left < 0)
        m_rcGameBoard.left = 0;
    if(m_rcGameBoard.right > GetSystemMetrics(SM_CXSCREEN))
        m_rcGameBoard.right = GetSystemMetrics(SM_CXSCREEN);
}

BOOL CDlgSudoku::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
        return TRUE;

    if(HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return TRUE;

    m_sizeWindow.cx = WIDTH(rcClient);
    m_sizeWindow.cy = HEIGHT(rcClient);

    if(m_hInst != NULL)
    {
        if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))     
            m_imgMenu.Initialize(hWnd, m_hInst, IsVGA()?IDB_PNG_Bar_HR:IDB_PNG_Bar);
        else
            m_imgMenu.Initialize(hWnd, m_hInst, IsVGA()?IDB_PNG_BarLand_HR:IDB_PNG_BarLand);
    }

    if(m_gdiIcons.GetDC())
    {   //quick hack ...
        ReloadSkin();
        g_gui->OnSize(hWnd, rcClient);
    }
	return TRUE;
}

BOOL CDlgSudoku::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	g_gui->Init(hWnd, m_hInst);

    m_oStr->SetResourceInstance(m_hInst);
    g_cLocale.Init(REG_KEY, m_hInst, SUPPORT_English|SUPPORT_Italian|SUPPORT_Portuguese|SUPPORT_Dutch|SUPPORT_German|SUPPORT_French|SUPPORT_Spanish|SUPPORT_Japanese, 
        TRUE);


#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif
    
    //g_gui->LoadSkin(m_sOptions.eSkin, m_sOptions.eBackground);

	// put up the main menu first
	m_bShowSplashScreen = TRUE;

    m_oGame.Initialize(m_hInst, m_hWnd);
    

	return TRUE;
}

void CDlgSudoku::LoadImages(HWND hWndSplash, int iMaxPercent)
{
    int iCurPercent = 0;
    if(hWndSplash)
        iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

    int iStep = (iMaxPercent)/5;

    if(m_gdiIcons.GetDC() == NULL)
        m_gdiIcons.LoadImage(IsVGA()?IDB_PNG_Icons_HR:IDB_PNG_Icons, hWndSplash, m_hInst);

    if(m_gdiIconGlow.GetDC() == NULL)
        m_gdiIconGlow.LoadImage(IsVGA()?IDB_PNG_IconGlow_HR:IDB_PNG_IconGlow, hWndSplash, m_hInst);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    if(m_imgTop.IsLoaded() == FALSE)
        m_imgTop.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_TopBar_HR:IDB_PNG_TopBar);

   iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    if(m_imgBtn.IsLoaded() == FALSE)
        m_imgBtn.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_Button_HR:IDB_PNG_Button);

    if(m_imgBtnBack.IsLoaded() == FALSE)
        m_imgBtnBack.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_ButtonBot_HR:IDB_PNG_ButtonBot);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
   
    if(m_gdiBtnFontGrey.GetDC() == NULL)
        m_gdiBtnFontGrey.LoadImage(IsVGA()?IDB_PNG_BtnFntGray_HR:IDB_PNG_BtnFntGray, hWndSplash, m_hInst);

    if(m_gdiBtnFontWhite.GetDC() == NULL)
        m_gdiBtnFontWhite.LoadImage(IsVGA()?IDB_PNG_BtnFntWhite_HR:IDB_PNG_BtnFntWhite, hWndSplash, m_hInst);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    RECT rcClient;
    GetClientRect(hWndSplash, &rcClient);
    ReloadSkin();
    g_gui->OnSize(hWndSplash, rcClient); 

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    CalculateLayout();//just in case
}

BOOL CDlgSudoku::IsPreDrawnCell(int x, int y)
{
    if(x == m_ptSelector.x && y == m_ptSelector.y)
        return FALSE;

    if(m_oGame.IsGiven(x, y))
        return TRUE;
    if(m_oGame.IsKakuro(x, y))
        return TRUE;

    return FALSE;
}

void CDlgSudoku::DrawBackground(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_gdiBackground.GetDC() == NULL)
    {
        m_gdiBackground.Create(g_gui->gdiBackground.GetDC(), 
            g_gui->gdiBackground.GetWidth(),
            g_gui->gdiBackground.GetHeight(),
            TRUE);

        //draw the top bar
        m_imgTop.DrawImage(m_gdiBackground, 0, 0);

        //now draw the menu
        m_imgMenu.DrawImage(m_gdiBackground, m_rcBottomBar.left, m_rcBottomBar.top);

        //draw the divider images
        if(WIDTH(m_rcBottomBar) > HEIGHT(m_rcBottomBar))
        {
            int iSpacing = (WIDTH(m_rcBottomBar))/4;
            int iIndent = (HEIGHT(m_rcBottomBar))/4;

            POINT ptS, ptE;

            for(int i = 0; i < 3; i++)
            {
                ptS.x = iSpacing + i*iSpacing-1;
                ptS.y = m_rcBottomBar.top + iIndent;
                ptE.x = ptS.x; 
                ptE.y = m_rcBottomBar.bottom - iIndent;

                Line(m_gdiBackground.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);

                ptE.x++;
                ptS.x++;

                Line(m_gdiBackground.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);
            }
        }
        else //on the side
        {
            int iSpacing = (HEIGHT(m_rcBottomBar))/4;
            int iIndent = (WIDTH(m_rcBottomBar))/4;

            POINT ptS, ptE;

            for(int i = 0; i < 3; i++)
            {
                ptS.x = m_rcBottomBar.left + iIndent;
                ptS.y = iSpacing + i*iSpacing;
                ptE.x = m_rcBottomBar.right - iIndent; 
                ptE.y = iSpacing + i*iSpacing;

                Line(m_gdiBackground.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);

                ptE.y++;
                ptS.y++;

                Line(m_gdiBackground.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);
            }

        }

        //now draw the buttons without numbers
        RECT rcTemp;
        
        for(int i = 0; i < 9; i++)
        {
            if(IsRectInRect(rcClip, m_rcValues[i]) == FALSE)
                continue;

            rcTemp = m_rcValues[i];
            if(i+1 == m_iButtonUp)
                OffsetRect(&rcTemp, 0, 0 - m_imgBtn.GetHeight()/2);

            m_imgBtn.DrawImage(m_gdiBackground, rcTemp.left, rcTemp.top);
        }

        //any given cells ... any kakoru cells
        m_ptSelector.x = -1;
        m_ptSelector.y = -1;

        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);
        DrawCells(m_gdiBackground, rcClient, TRUE);
/*
        for(int x = 0; x < m_oGame.GetGridSize(); x++)
        {
            for(int y = 0; y < m_oGame.GetGridSize(); y++)
            {   //optimized
                if(IsPreDrawnCell(x, y))
                    DrawCell(m_gdiBackground, x, y);
            }
        }*/

        m_ptSelector.x = 0;
        m_ptSelector.y = 0; //good enough
    }
    //should be a little quicker
    ::Draw(gdi, rcClip, m_gdiBackground, rcClip.left, rcClip.top);

}

BOOL CDlgSudoku::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

    DrawBackground(g_gui->gdiMem, rcClip);

    UpdateCells(g_gui->gdiMem, rcClip);

    // draw all the text
    DrawText(g_gui->gdiMem, rcClip);

    DrawButtons(g_gui->gdiMem, rcClip);
    
    g_gui->DrawScreen(rcClip, hDC);

	return TRUE;
}

BOOL CDlgSudoku::UpdateCells(CIssGDIEx& gdi, RECT& rcClip)
{
    for(int x = 0; x < m_oGame.GetGridSize(); x++)
    {
        for(int y = 0; y < m_oGame.GetGridSize(); y++)
        {   
            if(IsRectInRect(rcClip, m_rcGrid[x][y]))
                DrawCellUpdate(gdi, x, y);
        }
    }
    return TRUE;
}

BOOL CDlgSudoku::DrawCells(CIssGDIEx& gdi, RECT& rcClip, BOOL bNoText)
{
    for(int x = 0; x < m_oGame.GetGridSize(); x++)
    {
        for(int y = 0; y < m_oGame.GetGridSize(); y++)
        {   
            if(IsRectInRect(rcClip, m_rcGrid[x][y]))
                DrawCell(gdi, x, y, bNoText);
        }
    }
    return TRUE;
}

BOOL CDlgSudoku::DrawCellFont(CIssGDIEx& gdi, int iX, int iY, int iValue, EnumSquares eSquares)
{
    if(g_gui->sSkin.gdiFont1.GetDC() == NULL)
        return TRUE;//one check is enough

    CIssGDIEx* gdiFont = g_gui->GetFontGDI(eSquares);

    if(gdiFont == NULL)
        return FALSE;

    ::Draw(gdi,  
        m_rcGrid[iX][iY].left + g_gui->iXCellOff, 
        m_rcGrid[iX][iY].top + g_gui->iYCellOff,
        gdiFont->GetWidth()/9,
        gdiFont->GetHeight(),
        *gdiFont,
        (iValue-1)*g_gui->iFontWidth);

    return TRUE;


}

BOOL CDlgSudoku::DrawCellBackground(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquares)
{
    if(g_gui->sSkin.gdiSQs.GetDC() == NULL)
        return TRUE;

    BOOL bUse2 = FALSE;

    if(eSquares == SQ_None)
        return TRUE;//blank so we can leave

    int iTemp = (int)eSquares;

    if(iTemp >= SIZE_CELLARRAY)
    {
        bUse2 = TRUE;
        iTemp -= SIZE_CELLARRAY;
    }

    if(bUse2)
    {
        ::Draw(gdi,  
            m_rcGrid[iX][iY].left, 
            m_rcGrid[iX][iY].top,
            WIDTH(m_rcGrid[iX][iY]),
            HEIGHT(m_rcGrid[iX][iY]),
            g_gui->sSkin.gdiSQs2,
            iTemp*HEIGHT(m_rcGrid[iX][iY]));
    }
    else
    {
        ::Draw(gdi,  
            m_rcGrid[iX][iY].left, 
            m_rcGrid[iX][iY].top,
            WIDTH(m_rcGrid[iX][iY]),
            HEIGHT(m_rcGrid[iX][iY]),
            g_gui->sSkin.gdiSQs,
            iTemp*HEIGHT(m_rcGrid[iX][iY]));
    }

    return TRUE;
}

BOOL CDlgSudoku::DrawCell(CIssGDIEx& gdi, int iX, int iY, BOOL bNoText)
{
    static TCHAR szNum[4];

    if(iX < 0 || iX >= m_oGame.GetGridSize())
        return FALSE;
    if(iY < 0 || iY >= m_oGame.GetGridSize())
        return FALSE;

    //probably need to re-copy the background ....
    static EnumSquares eSquareOrig;
    static EnumSquares eSquare;
    static EnumDraw eDraw;

    //draw the locked cells
    if(m_oGame.IsLocked(iX, iY))
    {
        eSquare = SQ_Locked;
        eDraw = DRAW_Given;
    }
#ifdef DEBUG
    else if(m_bShowSolution)
    {
        eSquare = SQ_Board1;
        eDraw = DRAW_Given;
    }
#endif
    else if(m_oGame.IsGiven(iX, iY))
    {
        eSquare = SQ_Given;
        eDraw = DRAW_Given;
    }
    else if(m_oGame.GetGuess(iX, iY) != 0)
    {
        if(m_oGame.GetGameType() == STYPE_Kakuro)
            eSquare = SQ_Locked;
        else if(IsAltColor(iX, iY))
            eSquare = SQ_Guess2;
        else
            eSquare = SQ_Guess1;
        eDraw = DRAW_Guess;
    }
    else if(m_oGame.IsBlank(iX, iY))
    {
        //eDraw = DRAW_None;
        eDraw = DRAW_Blank;
    }
    else if(m_oGame.IsKakuro(iX, iY))
    {
        eSquare = SQ_Kakuro;
        eDraw = DRAW_Kakuro;
    }
    else//pencil marks
    {
        if(m_oGame.GetGameType() == STYPE_Kakuro)
            eSquare = SQ_Locked;
        else if(IsAltColor(iX, iY))
            eSquare = SQ_Board2;
        else
            eSquare = SQ_Board1;
        eDraw = DRAW_PencilMarks;
    }

    //draw some selector action here too ... should keep things nice and fast
    if(m_ptSelector.x == iX && m_ptSelector.y == iY)
        eSquare = SQ_Selector;
    else if(DrawHighlight(iX, iY) == TRUE)
        eSquare = SQ_Highlighted;

    //to correct for alternate games 
    eSquareOrig = eSquare;
    eSquare = g_gui->GetSquareEnum(eSquare, m_oGame.GetGameType(), (iX == m_ptSelector.x)?TRUE:FALSE);

    switch(eDraw)
    {
    case DRAW_Guess:
        DrawCellBackground(gdi, iX, iY, eSquare);
        DrawCellFont(gdi, iX, iY, m_oGame.GetGuess(iX, iY), eSquare);
        if(m_sOptions.bShowErrors &&
            m_oGame.GetGuess(iX, iY) != m_oGame.GetSolution(iX, iY))
        {
            ::DrawText(gdi.GetDC(), _T("× "), m_rcGrid[iX][iY], DT_RIGHT | DT_BOTTOM, m_hFontPencil, RGB(255,0,0));
        }
        break;
    case DRAW_Given:
        DrawCellBackground(gdi, iX, iY, eSquare);
        if(g_gui->sSkin.eSkin == SSKIN_Notepad)
            DrawCellFont(gdi, iX, iY, m_oGame.GetSolution(iX, iY), SQ_Given);
        else
            DrawCellFont(gdi, iX, iY, m_oGame.GetSolution(iX, iY), eSquare);
    	break;
    case DRAW_PencilMarks:
        DrawCellBackground(gdi, iX, iY, eSquare);
        if(bNoText == FALSE)
            DrawPencilMarks(gdi, iX, iY, eSquare);
        break;
    case DRAW_Kakuro:
        {
            DrawCellBackground(gdi, iX, iY, eSquare);
     
            RECT rcTemp = m_rcGrid[iX][iY];
            InflateRect(&rcTemp, (IsVGA()?-4:-2), (IsVGA()?-4:-2));
            rcTemp.top -= (IsVGA()?4:2);
            rcTemp.bottom += (IsVGA()?2:1);

            if(m_oGame.GetHorzTotal(iX, iY) > 0)
            {
                m_oStr->IntToString(szNum, m_oGame.GetHorzTotal(iX, iY));
                ::DrawText(gdi.GetDC(), szNum, rcTemp, DT_RIGHT | DT_TOP, m_hFontTotals);
            }

            if(m_oGame.GetVertTotal(iX, iY) > 0)
            {
                m_oStr->IntToString(szNum, m_oGame.GetVertTotal(iX, iY));
                ::DrawText(gdi.GetDC(), szNum, rcTemp, DT_LEFT | DT_BOTTOM, m_hFontTotals);
            }
        }
        break;
    case DRAW_Blank:
        if(m_sOptions.bDrawBlank)
        {   //after careful testing ... this is the best look ... go figure ...
            DrawCellBackground(gdi, iX, iY, SQ_Given);
        }
        break;      
    case DRAW_None:
    default:
        //do nothing
        break;
    }
    
     //additional step for killer sudoku
    if(m_oGame.GetGameType() == STYPE_Killer ||
        m_oGame.GetGameType() == STYPE_KenKen)
    {
        DrawOutlineItem(gdi, iX, iY);
        if(bNoText == FALSE)
            DrawTotal(gdi, iX, iY, eSquare);
    }
    else if(m_oGame.GetGameType() == STYPE_Greater)
    {
        DrawGreater(gdi, iX, iY, eSquare);
    }

    return TRUE;
}


BOOL CDlgSudoku::DrawCellUpdate(CIssGDIEx& gdi, int iX, int iY)
{
    static TCHAR szNum[4];

    if(iX < 0 || iX >= m_oGame.GetGridSize())
        return FALSE;
    if(iY < 0 || iY >= m_oGame.GetGridSize())
        return FALSE;

    BOOL bFullRedraw = FALSE;
    static EnumSquares eSquare;
    static EnumDraw eDraw;

    if(iX == m_ptSelector.x && iY == m_ptSelector.y)
        return DrawCell(gdi, iX, iY);//pass the buck

    eSquare = SQ_None;
    eDraw = DRAW_None;

    //draw the locked cells
    if(m_oGame.IsLocked(iX, iY))
    {    }
    else if(m_oGame.IsGiven(iX, iY))
    {    }
    else if(m_oGame.GetGuess(iX, iY) != 0)
    {    
        eDraw = DRAW_Guess;
    }
    else if(m_oGame.IsBlank(iX, iY))
    {    }
    else if(m_oGame.IsKakuro(iX, iY))
    {    }
    else//pencil marks
    {   //I need the eSquare set for the pencil mark font color
        if(m_oGame.GetGameType() == STYPE_Kakuro)
            eSquare = SQ_Locked;
        else if(IsAltColor(iX, iY))
            eSquare = SQ_Board2;
        else
            eSquare = SQ_Board1;
        eDraw = DRAW_PencilMarks;
    }

    //draw some selector action here too ... should keep things nice and fast
    if(m_ptSelector.x == iX && m_ptSelector.y == iY)
        eSquare = SQ_Selector;
    else if(DrawHighlight(iX, iY) == TRUE)
        eSquare = SQ_Highlighted;

    //to correct for alternate games 
    //uhhggg
    if(eSquare != SQ_Highlighted)
        eSquare = g_gui->GetSquareEnum(eSquare, m_oGame.GetGameType(), (iX == m_ptSelector.x)?TRUE:FALSE);//hack for notepad

    switch(eDraw)
    {
    case DRAW_PencilMarks:
    //    DrawCellBackground(gdi, iX, iY, eSquare);
        DrawPencilMarks(gdi, iX, iY, eSquare);
        break;
    case DRAW_Guess:
        {
            if(m_sOptions.bShowErrors &&
                m_oGame.GetGuess(iX, iY) != m_oGame.GetSolution(iX, iY))
            {
                ::DrawText(gdi.GetDC(), _T("× "), m_rcGrid[iX][iY], DT_RIGHT | DT_BOTTOM, m_hFontPencil, RGB(255,0,0));
            }
        }
    case DRAW_None:
    default:
        //do nothing
        break;
    }

    //always draw the highlights ... this is gonna be a performance hit ...
    if(m_sOptions.bShowHighlights && eSquare == SQ_Highlighted)
    {
        eSquare = g_gui->GetSquareEnum(eSquare, m_oGame.GetGameType(), (iX == m_ptSelector.x)?TRUE:FALSE);//hack for notepad
        DrawCell(gdi, iX, iY);
    }

     //additional step for killer sudoku
    if(m_oGame.GetGameType() == STYPE_Killer ||
        m_oGame.GetGameType() == STYPE_KenKen)
    {
        DrawTotal(gdi, iX, iY, eSquare);

        //gross but whatever 
        if(DrawHighlight(iX, iY) == 2)
            DrawOutlineItem(gdi, iX, iY);
    }
    else if(m_oGame.GetGameType() == STYPE_Greater)
    {
        DrawGreater(gdi, iX, iY, eSquare);
    }
    return TRUE;
}



BOOL CDlgSudoku::DrawPencilMarks(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare)
{
    TCHAR szNum[8];
    BOOL bIndicator = FALSE;
    BOOL bDraw6th = FALSE;
    RECT rcPencil;
    RECT rcTemp = m_rcGrid[iX][iY];

    if(m_oGame.GetGameType() == STYPE_Killer ||
        m_oGame.GetGameType() == STYPE_KenKen)
        InflateRect(&rcTemp, (IsVGA()?-4:-2), (IsVGA()?-4:-2));

    int	iTemp	= 0;
    int iTemp2	= 0;
    int j = 0;

    int iPencilMark = m_oGame.GetPencilMarks(iX,iY);

    if(iPencilMark == 0)
        return FALSE;

    COLORREF crColor = g_gui->GetPencilMark(eSquare, m_oGame.GetGameType());

    for(int i = 0; i < 7; i++) //6 for the numbers...one for the extra indicator
    {
        m_oStr->Empty(szNum);
        if(i < 6)
        {
            rcPencil.left	= rcTemp.left + (i%3)*(rcTemp.right - rcTemp.left)/3 + 1;//we'll just nudge it in to keep it tidy
            rcPencil.right	= rcPencil.left + (rcTemp.right - rcTemp.left)/3;
            rcPencil.top	= rcTemp.top  + (i/3)*(rcTemp.right - rcTemp.left)/2;
            rcPencil.bottom = rcPencil.top  + (rcTemp.right - rcTemp.left)/2;
        }
        
        for(j; j < 9; j++)
        {
            iTemp	= (int)(pow(2,j));
            iTemp2	= iPencilMark & iTemp;
            if(iTemp2 == iTemp)
            {
                if(i < 5)
                {
                    m_oStr->IntToString(szNum, j+1);
                    ::DrawText(gdi.GetDC(), szNum, rcPencil,  DT_CENTER | DT_VCENTER, m_hFontPencil, crColor);
                }
                else if(i < 6)
                {
                    bDraw6th = j;
                }
                else
                {
                    bIndicator = TRUE;
                }
                j++;
                break;
            }
        }
    }

    if(bIndicator) // draw the "more" indicator
        ::DrawText(gdi.GetDC(), _T("..."), rcPencil, DT_CENTER | DT_VCENTER, m_hFontPencil, crColor);
    else if(bDraw6th)
    {
        m_oStr->IntToString(szNum, bDraw6th+1);
        ::DrawText(gdi.GetDC(), szNum, rcPencil,  DT_CENTER | DT_VCENTER, m_hFontPencil, crColor);
    }



    return TRUE;
}

BOOL CDlgSudoku::DrawGreater(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare)
{
    if(m_oGame.GetGameType() != STYPE_Greater)
        return FALSE;

  //  ::DrawText(gdi.GetDC(), _T("˄"), m_rcGrid[iX][iY], DT_RIGHT | DT_VCENTER, m_hFontPencil, g_gui->GetTotalColor(eSquare));
    
    POINT pt[3];

    HPEN hPen = CIssGDIEx::CreatePen(g_gui->GetOutlineColor(0, FALSE), PS_SOLID, (int)IsVGA()+1);
    HPEN hOld = (HPEN)SelectObject(gdi.GetDC(), hPen);

    //aim for about 1/6th of the square
    int iSize = WIDTH(m_rcGrid[0][0])/5;

    if(m_oGame.GetGHorizontal(iX, iY) == GDIR_Left)
    {
        pt[0].x = m_rcGrid[iX][iY].left - iSize/2 - 1;
        pt[0].y = m_rcGrid[iX][iY].top + HEIGHT(m_rcGrid[iX][iY])/2 - iSize;
        pt[1].x = pt[0].x + iSize;
        pt[1].y = pt[0].y + iSize;
        pt[2].x = pt[0].x-1;//correction 
        pt[2].y = pt[0].y + 2*iSize+1;

        Polyline(gdi.GetDC(), pt, 3);

    }
    else if(m_oGame.GetGHorizontal(iX, iY) == GDIR_Right)
    {
        pt[0].x =  m_rcGrid[iX][iY].left - 1 + iSize/2;
        pt[0].y = m_rcGrid[iX][iY].top + HEIGHT(m_rcGrid[iX][iY])/2 - iSize;
        pt[1].x = pt[0].x - iSize;
        pt[1].y = pt[0].y + iSize;
        pt[2].x = pt[0].x+1;
        pt[2].y = pt[0].y + 2*iSize+1;

        Polyline(gdi.GetDC(), pt, 3);    
    }

    if(m_oGame.GetGVertical(iX, iY) == GDIR_Down)
    {
        pt[0].x = m_rcGrid[iX][iY].left + WIDTH(m_rcGrid[iX][iY])/2 - iSize;
        pt[0].y = m_rcGrid[iX][iY].top + iSize/2;
        pt[1].x = pt[0].x + iSize;
        pt[1].y = pt[0].y - iSize;
        pt[2].x = pt[0].x + 2*iSize+1;
        pt[2].y = pt[0].y+1;

        Polyline(gdi.GetDC(), pt, 3);
    }
    else if(m_oGame.GetGVertical(iX, iY) == GDIR_Up)
    {
        pt[0].x = m_rcGrid[iX][iY].left + WIDTH(m_rcGrid[iX][iY])/2 - iSize;
        pt[0].y = m_rcGrid[iX][iY].top - iSize/2;
        pt[1].x = pt[0].x + iSize;
        pt[1].y = pt[0].y + iSize;
        pt[2].x = pt[0].x + 2*iSize+1;
        pt[2].y = pt[0].y-1;

        Polyline(gdi.GetDC(), pt, 3);
    }

    SelectObject(gdi.GetDC(), hOld);
    DeleteObject(hPen);

    return TRUE;
}

BOOL CDlgSudoku::DrawTotal(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare)
{
    if(m_oGame.DrawTotal(iX, iY) == FALSE)
        return FALSE;
    if(m_bShowSolution == TRUE)
        return FALSE;

    static TCHAR szTemp[STRING_SMALL];

    if(m_oGame.GetGameType() == STYPE_Killer)
    {
        m_oStr->IntToString(szTemp, m_oGame.GetTotal(iX, iY));

        RECT rcTemp = m_rcGrid[iX][iY];
        rcTemp.left += WIDTH(rcTemp)/7;
        rcTemp.top += HEIGHT(rcTemp)/10;

        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFontTotals, g_gui->GetTotalColor(eSquare));
    }
    else if(m_oGame.GetGameType() == STYPE_KenKen)
    {
        m_oStr->IntToString(szTemp, m_oGame.GetKenKenAnswer(iX, iY));
        
        RECT rcTemp = m_rcGrid[iX][iY];
        rcTemp.left += WIDTH(rcTemp)/9;
        rcTemp.top += HEIGHT(rcTemp)/10;
        rcTemp.bottom -= HEIGHT(rcTemp)/10;

        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFontTotals, g_gui->GetTotalColor(eSquare));

        switch(m_oGame.GetKenKenOp(iX, iY))
        {
        case OP_Plus:
            m_oStr->StringCopy(szTemp, _T("+"));
            break;
        case OP_Minus:
            m_oStr->StringCopy(szTemp, _T("-"));
            break;;
        case OP_Times:
            m_oStr->StringCopy(szTemp, _T("×"));
            break;
        case OP_Divide:
            m_oStr->StringCopy(szTemp, _T("÷"));
            break;  
        }
       
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_BOTTOM, m_hFontTotals, g_gui->GetTotalColor(eSquare));
    }
    return TRUE;
}


BOOL CDlgSudoku::DrawOutlineItem(CIssGDIEx& gdi, int x, int y)
{
    POINT	pt[2];
    RECT	rcTemp		= m_rcGrid[x][y];
    int		iLineIndent = WIDTH(rcTemp) / 14; //lets see how this goes

    DWORD dwType = m_oGame.GetOutline(x, y);

    COLORREF crOutline = g_gui->GetOutlineColor(m_oGame.GetSamuIndex(x, y), DrawHighlight(x, y));


    HPEN hPenOutline = CreatePen(0, 0, crOutline);
    HPEN hPenOld = (HPEN)SelectObject(gdi.GetDC(), hPenOutline);

    //so the 4 overdraw functions draw outside of the cell if needed ... so we are drawing parts of the outline twice ... don't care

    //over draw left
    if(x > 0 &&
        m_oGame.GetSamuIndex(x, y) == m_oGame.GetSamuIndex(x-1, y))
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = m_rcGrid[x-1][y].right - iLineIndent;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);

        pt[0].y = m_rcGrid[x][y].bottom - iLineIndent - 1;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);
    }

    //over draw right
    if(x < NUM_COLUMNS - 1 &&
        m_oGame.GetSamuIndex(x, y) == m_oGame.GetSamuIndex(x+1, y))
    {
        pt[0].x = m_rcGrid[x][y].right - iLineIndent;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = m_rcGrid[x+1][y].left + iLineIndent;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);

        pt[0].y = m_rcGrid[x][y].bottom - iLineIndent - 1;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);
    }

    //overdraw top
    if(y > 0 &&
        m_oGame.GetSamuIndex(x, y) == m_oGame.GetSamuIndex(x, y-1))
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = pt[0].x;
        pt[1].y = m_rcGrid[x][y-1].bottom - iLineIndent;

        Polyline(gdi.GetDC(), pt, 2);

        pt[0].x = m_rcGrid[x][y].right - iLineIndent - 1;
        pt[1].x = pt[0].x;

        Polyline(gdi.GetDC(), pt, 2);
    }

    //overdraw bottom
    if(y < NUM_COLUMNS - 1 &&
        m_oGame.GetSamuIndex(x, y) == m_oGame.GetSamuIndex(x, y+1))
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].bottom - iLineIndent;
        pt[1].x = pt[0].x;
        pt[1].y = m_rcGrid[x][y+1].top + iLineIndent;
        //pt[1].y = m_rcGrid[x][y+1].top;

        Polyline(gdi.GetDC(), pt, 2);

        pt[0].x = m_rcGrid[x][y].right - iLineIndent - 1;
        pt[1].x = pt[0].x;

        Polyline(gdi.GetDC(), pt, 2);
    }

    //draw left line
    if((x > 0 &&
        (m_oGame.GetSamuIndex(x, y) != m_oGame.GetSamuIndex(x-1, y))) ||
        x == 0)
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = pt[0].x;
        pt[1].y = m_rcGrid[x][y].bottom - iLineIndent;
        Polyline(gdi.GetDC(), pt, 2);
    }

    //draw right line
    if((x < 8 &&
        (m_oGame.GetSamuIndex(x, y) != m_oGame.GetSamuIndex(x+1, y))) ||
        x == 8)
    {
        pt[0].x = m_rcGrid[x][y].right - iLineIndent-1;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = pt[0].x;
        pt[1].y = m_rcGrid[x][y].bottom - iLineIndent;

        Polyline(gdi.GetDC(), pt, 2);
    }

    //draw top line
    if((y > 0 &&
        (m_oGame.GetSamuIndex(x, y) != m_oGame.GetSamuIndex(x, y-1))) ||
        y == 0)
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].top + iLineIndent;
        pt[1].x = m_rcGrid[x][y].right - iLineIndent;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);
    }			


    //draw bottom line
    if((y < 8 &&
        (m_oGame.GetSamuIndex(x, y) != m_oGame.GetSamuIndex(x, y+1))) ||
        y == 8)
    {
        pt[0].x = m_rcGrid[x][y].left + iLineIndent;
        pt[0].y = m_rcGrid[x][y].bottom - iLineIndent-1;
        pt[1].x = m_rcGrid[x][y].right - iLineIndent;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);
    }
    
    SelectObject(gdi.GetDC(), hPenOld);
    DeleteObject(hPenOutline);
    hPenOutline = NULL;

    return TRUE;
}

BOOL CDlgSudoku::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    static BOOL bUnlock = FALSE;
    static BOOL iCount = 0;

    if(bUnlock == FALSE)
    {
        if(wParam == _T('p'))  iCount = 1;
        else if(wParam == _T('a') && iCount == 1)  iCount++;
        else if(wParam == _T('n') && iCount == 2)  iCount++;
        else if(wParam == _T('o') && iCount == 3)  {bUnlock = TRUE; PlaySounds(_T("IDR_WAVE_HINT"));}
        else iCount = 0;
    }

#ifdef DEBUG
    bUnlock = TRUE;
#endif
    
	switch(wParam)
	{
	case _T('1')://pile 1
	case _T('2')://pile 2
	case _T('3')://pile 3
	case _T('4')://pile 4
	case _T('5')://pile 5
	case _T('6')://pile 6
	case _T('7')://pile 7
    case _T('8'):	
    case _T('9'):
        OnNumber((DWORD)wParam - _T('0'));
        break;
    case _T('a'):
    case _T('A'):
        m_oGame.SetAutoPencilMarks(!m_oGame.GetAutoPencilMarks());
        InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
        break;
    case _T('b'):
    case _T('B'):
        m_sOptions.bDrawBlank = !m_sOptions.bDrawBlank;
        InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
        break;
    case _T('c'):
    case _T('C'):
        m_oGame.ClearPencilMarks();
        InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
        break;
    case _T('d'):
    case _T('D'):
        m_bShowSolution = !m_bShowSolution;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('e'):
    case _T('E'):
        m_sOptions.bShowErrors = !m_sOptions.bShowErrors;
        m_oGame.SetShowErrors(m_sOptions.bShowErrors);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('h'):
    case _T('H'):
        OnMenuHint();
        break;
    case _T('i'):
    case _T('I'):
        g_gui->gdiBackground.Destroy();
        g_gui->NextSkin(m_oGame.GetGameType());
        m_sOptions.eSkin = g_gui->sSkin.eSkin;//important
        m_sOptions.eBackground = g_gui->sSkin.eBackground;//important
        m_gdiBackground.Destroy();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('l'):
    case _T('L'):
        m_sOptions.bShowComplete = !m_sOptions.bShowComplete;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('u'):
    case _T('U'):
        OnMenuUndo();
        break;
   default:
       if(bUnlock == FALSE)
            return FALSE;
       break;
    }


    if(bUnlock)
    switch(wParam)
    {

    case _T('s'):
    case _T('S'):
        m_oGame.SetGameType(STYPE_Sudoku);
        OnNewGame();
        break;
    case _T('k'):
    case _T('K'):
        m_oGame.SetGameType(STYPE_Kakuro);
        OnNewGame();
        break;
    case _T('u'):
    case _T('U')://new game
        m_oGame.SetGameType(STYPE_Killer);
        OnNewGame();
        break;
    case _T('e'):
    case _T('E'):
        m_oGame.SetGameType(STYPE_KenKen);
        OnNewGame();
        break;
    case _T('g'):
    case _T('G'):
        m_oGame.SetGameType(STYPE_Greater);
        OnNewGame();
        break;
    case _T('n'):
    case _T('N')://new game
        OnNewGame();
        break;
    case _T('q'):
        PostQuitMessage(0);
        break;
    case _T('r'):
    case _T('R'):
        ReloadSkin();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('v'):
    case _T('V'):
        OnMenuSolve();
        break;
    case _T('j'):
    case _T('J'):
        g_gui->NextBackground();
        m_sOptions.eSkin = g_gui->sSkin.eSkin;//important
        m_sOptions.eBackground = g_gui->sSkin.eBackground;//important
        m_gdiBackground.Destroy();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('f'):
    case _T('F'):
        AnimateEndGameFireworks2();
        break;
    case _T('w'):
    case _T('W'):
        AnimateEndGameScale();
        break;
    case _T('x'):
    case _T('X'):
        AnimateEndGameSolitaire();
        break;
    case _T('y'):
    case _T('Y'):
        AnimateEndGameSelector();
        break;
    case _T('z'):
    case _T('Z'):
        AnimateEndGameFade();
        break;
    }
	//drop that sip
	//SHSipPreference(m_hWnd, SIP_DOWN);
	
    //could and should optimize this  BUGBUG
    return TRUE;

}

BOOL CDlgSudoku::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		break;
	}
	return TRUE;
}

BOOL CDlgSudoku::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    g_gui->OnKeyUp(hWnd, wParam, lParam);

    int iLoopCount = 0;
    
    switch(LOWORD(wParam))
	{
    case VK_TSOFT1:
        break;
    case VK_TSOFT2:
        OnMenuMenu();
        break;
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		break;
	case VK_RIGHT:
        m_ptSelector.x++;
        if(m_ptSelector.x >= m_oGame.GetGridSize())
            m_ptSelector.x = 0;
        while(m_oGame.IsBlank(m_ptSelector.x, m_ptSelector.y))
        {
            if(m_ptSelector.x >= m_oGame.GetGridSize())
                m_ptSelector.x = 0;
            else
                m_ptSelector.x++;
            iLoopCount++; if(iLoopCount > 12) break;//I fear endless loops
        }
		break;
	case VK_LEFT:
        m_ptSelector.x--;
        if(m_ptSelector.x < 0)
            m_ptSelector.x = m_oGame.GetGridSize() - 1;
        while(m_oGame.IsBlank(m_ptSelector.x, m_ptSelector.y))
        {
            if(m_ptSelector.x < 0)
                m_ptSelector.x = m_oGame.GetGridSize() - 1;
            else
                m_ptSelector.x--;
            iLoopCount++; if(iLoopCount > 12) break;//I fear endless loops
        }
		break;
	case VK_UP:
        m_ptSelector.y--;
        if(m_ptSelector.y < 0)
            m_ptSelector.y = m_oGame.GetGridSize() - 1;
        while(m_oGame.IsBlank(m_ptSelector.x, m_ptSelector.y))
        {
            if(m_ptSelector.y < 0)
                m_ptSelector.y = m_oGame.GetGridSize() - 1;
            else
                m_ptSelector.y--;
            iLoopCount++; if(iLoopCount > 12) break;//I fear endless loops
        }
        break;
	case VK_DOWN:
        m_ptSelector.y++;
        if(m_ptSelector.y >= m_oGame.GetGridSize())
            m_ptSelector.y = 0;
        while(m_oGame.IsBlank(m_ptSelector.x, m_ptSelector.y))
        {
            if(m_ptSelector.y >= m_oGame.GetGridSize())
                 m_ptSelector.y = 0;
            else
                m_ptSelector.y++;
            iLoopCount++; if(iLoopCount > 12) break;//I fear endless loops
        }
		break;
	}
    InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CDlgSudoku::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_Time_Timer)
    {
        m_iTimer++;
        
        FormatTime(m_iTimer, m_szTime);

        InvalidateRect(m_hWnd, &m_rcTime, FALSE);

        if(m_hWnd == GetForegroundWindow()) 
            SystemIdleTimerReset();
    }
    return FALSE;
}

void CDlgSudoku::FormatTime(int iTime, TCHAR* szTime)
{
    static SYSTEMTIME sTime;
    ZeroMemory(&sTime, sizeof(SYSTEMTIME));

    sTime.wHour   = iTime/3600;
    sTime.wMinute = iTime/60;
    sTime.wSecond = iTime%60;
    if(sTime.wHour > 0)
        GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sTime, _T("hh';'mm':'ss"), szTime, STRING_LARGE);
    else
        GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sTime, _T("mm':'ss"), szTime, STRING_LARGE);

}

BOOL CDlgSudoku::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

BOOL CDlgSudoku::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
    TCHAR szTemp[STRING_MAX];

	switch(LOWORD(wParam))
	{
	case IDMENU_GameSelect:
    case IDMENU_Menu:
        ShowMainMenu();
        break;
    case IDMENU_Left:
        OnMenuPencil();
        break;
    case IDMENU_Right:	
        OnMenuMenu();
	    break;
    case IDMENU_NewGame:
        {
            m_oStr->StringCopy(szTemp, ID(IDS_MSG_NewGame), STRING_MAX, m_hInst);
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
            {  
                 PlaySounds(_T("IDR_WAVE_MENU"));
                 OnNewGame();
            }
            else
                PlaySounds(_T("IDR_WAVE_MENU"));

        }
        InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_RestartGame:
		{
            m_oStr->StringCopy(szTemp, ID(IDS_MSG_RestartGame), STRING_MAX, m_hInst);
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
            {
                PlaySounds(_T("IDR_WAVE_MENU"));
                m_oGame.RestartGame();	    
                m_gdiBackground.Destroy();
                m_iTimer = 0;
            }
            else
                PlaySounds(_T("IDR_WAVE_MENU"));
		}
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
    case IDMENU_Undo:
        OnMenuUndo();
		break;
	case IDMENU_Quit:
    case IDMENU_Exit:
        {   
            CDlgMsgBox dlgMsg;
            if(IDYES == dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
            {
                PlaySounds(_T("IDR_WAVE_MENU"));
                PostQuitMessage(0);
            }
            else
                PlaySounds(_T("IDR_WAVE_MENU"));
        }
        break;
    case IDMENU_Options:
        OnMenuOptions();
        break;
    case IDMENU_Help:
		CDlgMainMenu::LaunchHelp(m_hWnd, m_hInst);
        break;
    case IDMENU_Stats:
        OnMenuStats();
        break;
    case IDMENU_About:
        {
            CDlgAbout dlgAbout;
            dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        }
        break;
	case IDMENU_MoreApps:
		{
			CDlgMoreApps dlgMore;
			dlgMore.Launch(m_hInst, m_hWnd);
		}
		break;
    case IDMENU_AutoPencilMarks:
        m_oGame.SetAutoPencilMarks(!m_oGame.GetAutoPencilMarks());
        InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
        break;
    case IDMENU_Solve:
        OnMenuSolve();
        break;
    case IDMENU_ClearPencilMarks:
        m_oGame.ClearPencilMarks();
        InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
        break;

	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgSudoku::OnMenuStats()
{
    CDlgStats dlgStats;
    dlgStats.Init(&m_oGame);
   dlgStats.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
    return TRUE;
}

BOOL CDlgSudoku::OnMenuOptions()
{
    CDlgOptions dlgOptions;
    m_sOptions.eSkin = g_gui->sSkin.eSkin;//make sure we're in sync
    m_sOptions.eBackground = g_gui->sSkin.eBackground;
    m_sOptions.bShowHints = m_oGame.GetShowHints();//again .. stay in sync
    dlgOptions.Init(m_hInst, m_sOptions);
    if(IDOK == dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
    {
        //just copy the whole damn thing
        memcpy(&m_sOptions, &dlgOptions.m_sOptions, sizeof(TypeOptions));
        m_oGame.SetShowHints(m_sOptions.bShowHints);
        m_oGame.SetShowLocked(m_sOptions.bShowComplete);
        m_oGame.SetAutoRemovePencil(m_sOptions.bAutoRemovePencil);
        g_gui->gdiBackground.Destroy();//lets just do this every time
        g_gui->SetImageBackground(m_sOptions.szPath);
        g_gui->LoadSkin(m_sOptions.eSkin, m_sOptions.eBackground);
        m_gdiBackground.Destroy();

        SaveRegistry();
    }
    return TRUE;
}

BOOL CDlgSudoku::OnMenuHint()
{


    if(!m_oGame.Hint())
        return FALSE;

    //move selector
    m_ptSelector.x = m_oGame.GetLastHint().x;
    m_ptSelector.y = m_oGame.GetLastHint().y;

    m_iTimer += HINT_PENALTY;

    if(m_oGame.IsGameComplete())//no highscore for finishing with cheat
    {
        TCHAR szTemp[STRING_MAX];
        m_oStr->StringCopy(szTemp, ID(IDS_MSG_NewGame2), STRING_MAX, m_hInst);

        CDlgMsgBox dlgMsg;
        if(IDYES == dlgMsg.PopupMessage(szTemp, m_hWnd,
            m_hInst, MB_YESNO))
        {
            PlaySounds(_T("IDR_WAVE_MENU"));
            OnNewGame();
        }
        else
        {
            PlaySounds(_T("IDR_WAVE_MENU"));
            ShowMainMenu();
        }
    }
    InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
    return TRUE;
}


BOOL CDlgSudoku::OnMenuUndo()
{
     return m_oGame.Undo();

}


BOOL CDlgSudoku::OnMenuSolve()
{
    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);
    m_oGame.Solve();

    //lets make this work
    if(m_oGame.IsGameComplete(DEFAULT_TIME))//no high scores
    {
        TCHAR szTemp[STRING_MAX];
        m_oStr->StringCopy(szTemp, ID(IDS_MSG_NewGame2), STRING_MAX, m_hInst);

        AnimateSolve();
        CDlgMsgBox dlgMsg;
        if(IDYES == dlgMsg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
        {
            PlaySounds(_T("IDR_WAVE_MENU"));
            OnNewGame();
        }
        else
        {
            PlaySounds(_T("IDR_WAVE_MENU"));
            ShowMainMenu();
        }
    }
    return TRUE;
}

BOOL CDlgSudoku::DrawHighlight(int iX, int iY)
{
    if(m_oGame.GetGameType() == STYPE_Sudoku && m_sOptions.bShowHighlights)
    {
        if(m_ptSelector.x == iX)
            return TRUE;
        if(m_ptSelector.y == iY)
            return TRUE;

      /*  if( (m_ptSelector.x / 3 == iX / 3) &&
            (m_ptSelector.y / 3 == iY / 3) )
            return TRUE;*/
    }

    else if(m_oGame.GetGameType() == STYPE_Killer ||
        m_oGame.GetGameType() == STYPE_KenKen)
    {
        if(m_sOptions.eSkin == SSKIN_Notepad)
            return FALSE;

        if(m_oGame.GetSamuIndex(iX, iY) == m_oGame.GetSamuIndex(m_ptSelector.x, m_ptSelector.y))
        {
            if(m_sOptions.eSkin == SSKIN_Grey || 
                m_sOptions.eSkin == SSKIN_Glass)
                return 2;
            return TRUE;
        }
    }

    return FALSE;
}


BOOL CDlgSudoku::DrawSelector(CIssGDIEx& gdi, RECT& rcClip)
{
/*    //now lets draw the rest
    //row & column

    if(m_oGame.GetGameType() == STYPE_Sudoku)
    {
        for(int i = 0; i < 9; i++)
        {
            if(i != m_ptSelector.y && IsRectInRect(rcClip, m_rcGrid[m_ptSelector.x][i]))
                AlphaFillRect(gdi, m_rcGrid[m_ptSelector.x][i], 0x00FFFF, 40);
            if(i != m_ptSelector.x && IsRectInRect(rcClip, m_rcGrid[i][m_ptSelector.y]))
                AlphaFillRect(gdi, m_rcGrid[i][m_ptSelector.y], 0x00FFFF, 40);
            //rest of the subset
        }

        //sections
        int x = m_ptSelector.x / 3;
        int y = m_ptSelector.y / 3;

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                if(i == m_ptSelector.x%3)
                    continue;
                if(j == m_ptSelector.y%3)
                    continue;
                if(IsRectInRect(rcClip, m_rcGrid[x*3+i][y*3+j]))
                    AlphaFillRect(gdi, m_rcGrid[x*3+i][y*3+j], 0x00FFFF, 40);
            }
        }
    }
    if(m_oGame.GetGameType() == STYPE_Killer)
    {
        for(int i = 0; i < 9; i++)
        {
            for(int j = 0; j < 9; j++)
            {
                if(m_oGame.GetSamuIndex(i, j) == m_oGame.GetSamuIndex(m_ptSelector.x, m_ptSelector.y))
                {
                    AlphaFillRect(gdi, m_rcGrid[i][j], 0x00FFFF, 40);
                }
            }
        }
    }

    if(IsRectInRect(rcClip, m_rcGrid[m_ptSelector.x][m_ptSelector.y]))
        AlphaFillRect(gdi, m_rcGrid[m_ptSelector.x][m_ptSelector.y], 0x00FFFF, 100);
  */
    return TRUE;
}

BOOL CDlgSudoku::DrawButtons(CIssGDIEx& gdi, RECT& rcClip)
{
    RECT rcTemp;

    //bottom bar is now on the background GDI
    //if(IsRectInRect(m_rcBottomBar, rcClip))
    //    m_imgMenu.DrawImage(gdi, m_rcBottomBar.left, m_rcBottomBar.top);

    int iNumWidth = m_gdiBtnFontGrey.GetWidth() / 9;
    int iXOff = (WIDTH(m_rcValues[0]) - iNumWidth)/2;
    int iYOff =(IsVGA()?16:8);// (HEIGHT(m_rcValues[0]) - m_gdiBtnFontGrey.GetHeight())/2;

    for(int i = 0; i < 9; i++)
    {
        if(IsRectInRect(rcClip, m_rcValues[i]) == FALSE)
            continue;

        rcTemp = m_rcValues[i];
        if(i+1 == m_iButtonUp)
        {
            //only draw the button if its up ... otherwise its on the background gdi
            FillRect(gdi.GetDC(), rcTemp, 0);
            OffsetRect(&rcTemp, 0, 0 - m_imgBtn.GetHeight()/2);
            m_imgBtn.DrawImage(gdi, rcTemp.left, rcTemp.top);
        }

        if(m_oGame.IsPencilMarkValid(i+1, m_ptSelector.x, m_ptSelector.y))
            ::Draw(gdi, rcTemp.left + iXOff, rcTemp.top + iYOff,
            iNumWidth, m_gdiBtnFontGrey.GetHeight(), m_gdiBtnFontWhite, iNumWidth*i);
        else
            ::Draw(gdi, rcTemp.left + iXOff, rcTemp.top + iYOff,
            iNumWidth, m_gdiBtnFontGrey.GetHeight(), m_gdiBtnFontGrey, iNumWidth*i);
        
      /*  m_oStr->IntToString(szNum, i+1);

        if(m_oGame.IsPencilMarkValid(i+1, m_ptSelector.x, m_ptSelector.y))
            ::DrawText(gdi, szNum, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0x888888);
        else
            ::DrawText(gdi, szNum,rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);*/
    }

    int iXOffSet = (WIDTH(m_rcUndo) - m_gdiIcons.GetWidth()/5)/2;
    int iYOffSet = (HEIGHT(m_rcUndo) - m_gdiIcons.GetHeight())/2;

    int iXIcon = m_gdiIcons.GetWidth()/5;
    int iYIcon = m_gdiIcons.GetHeight();


    if(IsRectInRect(rcClip, m_rcUndo))
    {
        if(m_iMenuHighlight == MENU_Undo)
        ::Draw(gdi, m_rcUndo.left + m_ptGlowOffset.x, 
            m_rcUndo.top + m_ptGlowOffset.y, 
            m_gdiIconGlow.GetWidth(),
            m_gdiIconGlow.GetHeight(),
            m_gdiIconGlow);

        ::Draw(gdi, m_rcUndo.left + iXOffSet, m_rcUndo.top + iYOffSet, iXIcon, iYIcon, m_gdiIcons,
            0, 0);
    }

    if(IsRectInRect(rcClip, m_rcHint))
    {
        if(m_iMenuHighlight == MENU_Hint)
        ::Draw(gdi, m_rcHint.left + m_ptGlowOffset.x, 
            m_rcHint.top + m_ptGlowOffset.y, 
            m_gdiIconGlow.GetWidth(),
            m_gdiIconGlow.GetHeight(),
            m_gdiIconGlow);
        
        ::Draw(gdi, m_rcHint.left + iXOffSet, m_rcHint.top + iYOffSet, iXIcon, iYIcon, m_gdiIcons,
            iXIcon, 0);

        
    }

    if(IsRectInRect(rcClip, m_rcPencil))
    {
        if(m_iMenuHighlight == MENU_Pencil)
        ::Draw(gdi, m_rcPencil.left + m_ptGlowOffset.x, 
            m_rcPencil.top + m_ptGlowOffset.y, 
            m_gdiIconGlow.GetWidth(),
            m_gdiIconGlow.GetHeight(),
            m_gdiIconGlow);

        if(m_bPenMode)
            ::Draw(gdi, m_rcPencil.left + iXOffSet, m_rcPencil.top + iYOffSet, iXIcon, iYIcon, m_gdiIcons,
            iXIcon*4, 0);
        else
            ::Draw(gdi, m_rcPencil.left + iXOffSet, m_rcPencil.top + iYOffSet, iXIcon, iYIcon, m_gdiIcons,
            iXIcon*3, 0);
    }
    
    if(IsRectInRect(rcClip, m_rcMenu))
    {   
        if(m_iMenuHighlight == MENU_Menu)
        ::Draw(gdi, m_rcMenu.left + m_ptGlowOffset.x, 
            m_rcMenu.top + m_ptGlowOffset.y, 
            m_gdiIconGlow.GetWidth(),
            m_gdiIconGlow.GetHeight(),
            m_gdiIconGlow);

        ::Draw(gdi, m_rcMenu.left + iXOffSet, m_rcMenu.top + iYOffSet, iXIcon, iYIcon, m_gdiIcons,
            iXIcon*2, 0);
    }
    return TRUE;
}

BOOL CDlgSudoku::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcTime) == FALSE)
        return FALSE;

    static TCHAR szTemp[STRING_NORMAL];
    static TCHAR szTemp2[STRING_NORMAL];

 
    ::DrawText(gdi.GetDC(), m_szTime, m_rcTime, DT_LEFT | DT_VCENTER, m_hFontText, g_gui->sSkin.crTopText);

    m_oStr->StringCopy(szTemp2, ID(IDS_MENU_Free), STRING_NORMAL, m_hInst);

    m_oStr->Format(szTemp, _T("%s: %i"), szTemp2, m_oGame.GetCellsFree());
    ::DrawText(gdi.GetDC(), szTemp, m_rcTime, DT_RIGHT | DT_VCENTER, m_hFontText, g_gui->sSkin.crTopText);

    if(m_oGame.GetGameType() == STYPE_Killer)
    {
        m_oStr->StringCopy(szTemp2, ID(IDS_MENU_Total), STRING_NORMAL, m_hInst);

        m_oStr->Format(szTemp, _T("%s: %i"), szTemp2, m_oGame.GetSamuValue(m_ptSelector.x, m_ptSelector.y));
        ::DrawText(gdi.GetDC(), szTemp, m_rcTime, DT_CENTER | DT_VCENTER, m_hFontText, g_gui->sSkin.crTopText);
    }
    else if(m_oGame.GetGameType() == STYPE_KenKen)
    {
        m_oStr->IntToString(szTemp, m_oGame.GetKenKenAnswer(m_ptSelector.x, m_ptSelector.y));

        switch(m_oGame.GetKenKenOp(m_ptSelector.x, m_ptSelector.y))
        {
        case OP_Plus:
            m_oStr->Concatenate(szTemp, _T("+"));
            break;
        case OP_Minus:
            m_oStr->Concatenate(szTemp, _T("-"));
            break;;
        case OP_Times:
            m_oStr->Concatenate(szTemp, _T("×"));
            break;
        case OP_Divide:
            m_oStr->Concatenate(szTemp, _T("÷"));
            break;  
        }  
        ::DrawText(gdi.GetDC(), szTemp, m_rcTime, DT_CENTER | DT_VCENTER, m_hFontText, g_gui->sSkin.crTopText);
    }
	return TRUE;
}

void CDlgSudoku::ReloadSkin()
{
    if(m_hWnd == NULL)
        return;

    RECT rcClient;
    m_gdiBackground.Destroy();
    GetClientRect(m_hWnd, &rcClient);
    CalculateLayout();
}


void CDlgSudoku::OnNewGame()
{
    KillTimer(m_hWnd, IDT_Time_Timer);
    m_iTimer = 0;
    m_oGame.NewGame();
    ReloadSkin();
    ResetSelector();
    InvalidateRect(m_hWnd, NULL, FALSE);
    SetTimer(m_hWnd, IDT_Time_Timer, 1000, NULL);
}

void CDlgSudoku::PlaySounds(TCHAR* szWave)
{
	if(m_sOptions.bPlaySounds)
		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC);
}


BOOL CDlgSudoku::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_UPDATECELL)
    {
        FadeTransitionCell(wParam, lParam);

        POINT ptTemp = m_ptSelector;
        m_ptSelector.x = -1;
        m_ptSelector.y = -1;

        //redraw the background
        //because we probably drew the selector onto the background
        ::Draw(m_gdiBackground, m_rcGrid[wParam][lParam], g_gui->gdiBackground,
            m_rcGrid[wParam][lParam].left, m_rcGrid[wParam][lParam].top);
        //then draw the cell
        DrawCell(m_gdiBackground, (int)wParam, (int)lParam, TRUE);
        m_ptSelector = ptTemp;

        return TRUE;
    }

    if(uiMessage == WM_UNDOMOVE)
    {
        int iTemp = m_ptSelector.x;
        m_ptSelector.x = -1;

        //gotta draw the background as well
        BitBlt(m_gdiBackground.GetDC(), 
            m_rcGrid[wParam][lParam].left, m_rcGrid[wParam][lParam].top, 
            WIDTH(m_rcGrid[wParam][lParam]), HEIGHT(m_rcGrid[wParam][lParam]),
            g_gui->gdiBackground, 
            m_rcGrid[wParam][lParam].left, m_rcGrid[wParam][lParam].top, 
            SRCCOPY);
        
        DrawCell(m_gdiBackground, wParam, lParam, TRUE);
        m_ptSelector.x = iTemp;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    
    return UNHANDLED;
}

BOOL CDlgSudoku::OnMouseMove(HWND hWnd, POINT& pt)
{
    for(int i = 0; i < 9; i++)
    {
        if(PtInRect(&m_rcValues[i], pt))
        {
            m_iButtonUp = 1 +i;
            InvalidateRect(m_hWnd, &m_rcNumButtons, FALSE); //bugbug ....
            return TRUE;
        }
    }

    m_iButtonUp = 0;

    for(int x = 0; x < m_oGame.GetGridSize(); x++)
    {
        for(int y = 0; y < m_oGame.GetGridSize(); y++)
        {
            if(PtInRect(&m_rcGrid[x][y], pt))
            {
                //move the selector
                m_ptSelector.x = x;
                m_ptSelector.y = y;
                InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
                return TRUE;
            }
        }
    }

    return UNHANDLED;
}

BOOL CDlgSudoku::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcUndo, pt))
    {
        m_iMenuHighlight = 0;
        InvalidateRect(m_hWnd, &m_rcUndo, FALSE);
    }

    else if(PtInRect(&m_rcHint, pt))
    {
        m_iMenuHighlight = 1;
        InvalidateRect(m_hWnd, &m_rcHint, FALSE);
    }

    else if(PtInRect(&m_rcPencil, pt))
    {
        m_iMenuHighlight = 2;
        InvalidateRect(m_hWnd, &m_rcPencil, FALSE);
    }

    else if(PtInRect(&m_rcMenu, pt))
    {
        m_iMenuHighlight = 3;
        InvalidateRect(m_hWnd, &m_rcMenu, FALSE);
    }

    if(m_iMenuHighlight > -1)
        UpdateWindow(m_hWnd);

    return TRUE; //g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgSudoku::OnMenuPencil()
{
    PlaySounds(_T("IDR_WAVE_MENU"));
    m_bPenMode = !m_bPenMode;
    InvalidateRect(m_hWnd, &m_rcPencil, FALSE);
    return TRUE;
}

BOOL CDlgSudoku::OnLButtonUp(HWND hWnd, POINT& pt)
{
 /*   for(int x = 0; x < m_oGame.GetGridSize(); x++)
    {
        for(int y = 0; y < m_oGame.GetGridSize(); y++)
        {
            if(PtInRect(&m_rcGrid[x][y], pt))
            {
                //move the selector
                m_ptSelector.x = x;
                m_ptSelector.y = y;
                InvalidateRect(m_hWnd, &m_rcGameBoard, FALSE);
                return TRUE;
            }
        }
    }*/

    if(m_iMenuHighlight != -1)
    {
        switch((EnumMenuButtons)m_iMenuHighlight)
        {
        case MENU_Undo:
            InvalidateRect(m_hWnd, &m_rcUndo, FALSE);
        	break;
        case MENU_Hint:
            InvalidateRect(m_hWnd, &m_rcHint, FALSE);
        	break;
        case MENU_Pencil:
            InvalidateRect(m_hWnd, &m_rcPencil, FALSE);
            break;
        case MENU_Menu:
            InvalidateRect(m_hWnd, &m_rcMenu, FALSE);
            break;
        }
        m_iMenuHighlight = -1;
        UpdateWindow(m_hWnd);
    }

    m_iButtonUp = 0;

    for(int i = 0; i < 9; i++)
    {
        if(PtInRect(&m_rcValues[i], pt))
        {
            OnNumber(i+1);
            return TRUE;
        }
    }
    
    if(PtInRect(&m_rcMenu, pt))
    {
        PlaySounds(_T("IDR_WAVE_MENU"));
        OnMenuMenu();
        return TRUE;
    }

    if(PtInRect(&m_rcPencil, pt))
    {
        return OnMenuPencil();
    }

    if(PtInRect(&m_rcHint, pt))
    {
        if(OnMenuHint())
            PlaySounds(_T("IDR_WAVE_HINT"));
        else
            PlaySounds(_T("IDR_WAVE_MENU"));
        return TRUE;
    }

    if(PtInRect(&m_rcUndo, pt))
    {
        if(OnMenuUndo())
            PlaySounds(_T("IDR_WAV_UNDO"));
        else
            PlaySounds(_T("IDR_WAVE_MENU"));
        return TRUE;
    }
    return UNHANDLED;
}

void CDlgSudoku::UpdateSelector(int iOldSel)
{

}

BOOL CDlgSudoku::ShouldAbort()
{
	MSG msg;

	if (MsgWaitForMultipleObjects(0, NULL, FALSE, 5, QS_ALLINPUT) != WAIT_OBJECT_0)
		return FALSE;

	if(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
	{
		switch(LOWORD(msg.message))
		{
	//	case WM_KEYUP:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_COMMAND:
		case WM_USER:
		case WM_SETFOCUS:
			return TRUE;
		}

		//	PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
		//	TranslateMessage((LPMSG)&msg);
		//	DispatchMessage((LPMSG)&msg);
	}
	return FALSE;
}

BOOL CDlgSudoku::OnMenuMenu()
{
    g_gui->wndMenu.ResetContent();

    TCHAR szTemp[STRING_NORMAL];

    m_oStr->StringCopy(szTemp, ID(IDS_MENU_NewGame), STRING_NORMAL, m_hInst);
    g_gui->wndMenu.AddItem(szTemp, IDMENU_GameSelect);

    m_oStr->StringCopy(szTemp, ID(IDS_MENU_RestartGame), STRING_NORMAL, m_hInst);
    g_gui->wndMenu.AddItem(szTemp, IDMENU_RestartGame);

    m_oStr->StringCopy(szTemp, ID(IDS_MENU_Solve), STRING_NORMAL, m_hInst);
    g_gui->wndMenu.AddItem(szTemp, IDMENU_Solve);

    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(ID(IDS_MENU_AutoPencil), m_hInst, IDMENU_AutoPencilMarks, (m_oGame.GetAutoPencilMarks()?FLAG_Check:NULL));
    g_gui->wndMenu.AddItem(ID(IDS_MENU_ClearPencil), m_hInst, IDMENU_ClearPencilMarks, (m_oGame.GetAutoPencilMarks()?FLAG_Grayed:NULL));
    g_gui->wndMenu.AddSeparator();
#ifndef NMA
//	g_gui->wndMenu.AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Stats), m_hInst, IDMENU_Stats);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Exit), m_hInst, IDMENU_Exit);

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}

#define SHADOW_FADE (IsVGA()?8:4)

BOOL CDlgSudoku::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {	
            
        //this should only exist when using a smartphone in landscape orientation (for now anyways)
        bRet = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

        if(m_bShowSplashScreen == TRUE)
        {
            m_bShowSplashScreen = FALSE;

			//g_gui->Init(hWnd, m_hInst);

            // put up the splash screen
            ShowSplashScreen();		
        }
        else
        {
            KillTimer(m_hWnd, IDT_Time_Timer);
            SetTimer(m_hWnd, IDT_Time_Timer, 1000, NULL);
        }
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
        KillTimer(m_hWnd, IDT_Time_Timer);
		SaveRegistry();
    }
#endif
    return bRet;
}



BOOL CDlgSudoku::ShowSplashScreen()
{
    // put up the splash screen first
    CDlgSplashScreen dlgSplashScreen;
    dlgSplashScreen.Create(_T("Splash"), NULL, m_hInst, _T("SplashClass"));
    ShowWindow(dlgSplashScreen.GetWnd(), SW_SHOW);
    MSG msg;

    // force the message pump to go through while waiting for the first WM_PAINT
    while( GetMessage( &msg,dlgSplashScreen.GetWnd(),0,0 ) ) 
    {
        TranslateMessage( &msg );
        DispatchMessage ( &msg );

        switch(msg.message)
        {
        case WM_LOAD_Sounds:

            break;
        case WM_LOAD_Fonts:
            break;
        case WM_LOAD_Background:

            break;
        case WM_lOAD_Images:
            LoadImages(dlgSplashScreen.GetWnd(), 100);
            SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 100, 0);
            break;
        }

        if(msg.message == WM_lOAD_Images)
            break;
    }

    ShowMainMenu();
    return TRUE;
}

BOOL CDlgSudoku::ShowMainMenu()
{
    m_dlgMenu.Init(&m_oGame, m_sOptions);
    UINT uiResult = (UINT)m_dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    m_oGame.SetShowHints(m_sOptions.bShowHints);
    m_oGame.SetShowLocked(m_sOptions.bShowComplete);

    switch(uiResult)
    {
    case IDMENU_NewGame:
        OnNewGame();
        break;
    case IDMENU_Exit:
        PostQuitMessage(0);
        return TRUE;
    default:
    case IDMENU_Resume:
        if(g_gui->IsBackgroundLoaded() == FALSE)
        {
            g_gui->LoadBackground(m_oGame.GetGameType());
        }
        ResetSelector();
        break;
    }

    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);
    return TRUE;
}

BOOL CDlgSudoku::OnNumber(int iNum)
{
    if(iNum < 1 || iNum > 9)
        return FALSE;
    if(m_bPenMode)
    {
        if(iNum == m_oGame.GetGuess(m_ptSelector.x, m_ptSelector.y))
            PlaySounds(_T("IDR_WAV_UNSET"));
        else
            PlaySounds(_T("IDR_WAV_SET"));
        
        m_oGame.OnSetGuess(m_ptSelector.x, m_ptSelector.y, iNum);
        //good place to check animations and end game
        if(m_oGame.IsGameComplete(m_iTimer))
        {
            InvalidateRect(m_hWnd, NULL, FALSE);
            UpdateWindow(m_hWnd);//force the ol redraw before the animation
            AnimateEndGame();
            CDlgMsgBox dlgMsg;

            TCHAR szGameType[STRING_NORMAL];
            TCHAR szDiff[STRING_NORMAL];
            TCHAR szMsg[STRING_MAX];

            switch(m_oGame.GetDifficulty())
            {
            case DIFF_VeryEasy:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_VeryEasy), STRING_NORMAL, m_hInst);
            	break;
            case DIFF_Easy:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_Easy), STRING_NORMAL, m_hInst);
            	break;
            default:
            case DIFF_Medium:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_Normal), STRING_NORMAL, m_hInst);
                break;
            case DIFF_Hard:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_Hard), STRING_NORMAL, m_hInst);
                break;
            case DIFF_VeryHard:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_Expert), STRING_NORMAL, m_hInst);
                break;
            case DIFF_Hardcore:
                m_oStr->StringCopy(szDiff, ID(IDS_PREF_Genius), STRING_NORMAL, m_hInst);
                break;
            }

            switch(m_oGame.GetGameType())
            {
            case STYPE_Sudoku:
                m_oStr->StringCopy(szGameType, ID(IDS_Sudoku), STRING_NORMAL, m_hInst);
            	break;
            case STYPE_Killer:
                m_oStr->StringCopy(szGameType, ID(IDS_KillerSudoku), STRING_NORMAL, m_hInst);
            	break;
            case STYPE_Kakuro:
                m_oStr->StringCopy(szGameType, ID(IDS_Kakuro), STRING_NORMAL, m_hInst);
                break;
            case STYPE_KenKen:
                m_oStr->StringCopy(szGameType, ID(IDS_KenKen), STRING_NORMAL, m_hInst);
                break;
            default:
                break;
            }

            TCHAR szTemp[STRING_MAX];
            m_oStr->StringCopy(szTemp, ID(IDS_MSG_Congrats), STRING_MAX, m_hInst);

            m_oStr->Format(szMsg, szTemp, szDiff, szGameType, m_szTime);

            TCHAR szAvg[STRING_NORMAL];
            TCHAR szBest[STRING_NORMAL];

            FormatTime(m_oGame.GetAvgTime(m_oGame.GetGameType(), m_oGame.GetDifficulty()), szAvg);
            FormatTime(m_oGame.GetBestTime(m_oGame.GetGameType(), m_oGame.GetDifficulty()), szBest);

            m_oStr->Format(szTemp, _T("\r\n(Avg: %s / Best: %s)"), szAvg, szBest);
            m_oStr->Concatenate(szMsg, szTemp);
     //       if(IDYES == dlgMsg.PopupMessage(_T("Congratulations!\r\n\r\nWould you like to play a new game?"), m_hWnd,
            if(IDYES == dlgMsg.PopupMessage(szMsg, m_hWnd, m_hInst, MB_YESNO))
            {
                PlaySounds(_T("IDR_WAVE_MENU"));
                OnNewGame();
            }
            else
            {
                PlaySounds(_T("IDR_WAVE_MENU"));
                ShowMainMenu();
            }
            return TRUE;
        }
    }
    else
    {
        m_oGame.OnSetPencilMark(m_ptSelector.x, m_ptSelector.y, iNum);
    }

    InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}


BOOL CDlgSudoku::IsAltColor(int iX, int iY)
{
    if(m_oGame.GetGameType() == STYPE_Kakuro)
        return FALSE;

    if((iX/3 == 1) ^ (iY/3 == 1))
        return TRUE;

    return FALSE;
}

BOOL CDlgSudoku::FadeTransitionCell(int iX, int iY, int iTime )
{
    if(m_hWnd == NULL)
        return FALSE;
    if(m_gdiBackground.GetDC() == NULL)
        return FALSE;
    
    HDC dc = GetDC(m_hWnd);

    if(dc == NULL)
        return FALSE;
    

    //save original 
    CIssGDIEx gdiFrom;
    CIssGDIEx gdiTo;

    gdiFrom.Create(dc, m_rcGrid[iX][iY], TRUE);
    
    //draw the background
    ::Draw(m_gdiBackground, m_rcGrid[iX][iY], g_gui->gdiBackground,
        m_rcGrid[iX][iY].left, m_rcGrid[iX][iY].top);
    //draw the cell onto the background gdi
    DrawCell(m_gdiBackground, iX, iY, TRUE);
    gdiTo.Create(m_gdiBackground.GetDC(), m_rcGrid[iX][iY], TRUE);

    RECT rcCell;
    SetRect(&rcCell, 0, 0, WIDTH(m_rcGrid[0][0]), HEIGHT(m_rcGrid[0][0]));

    AlphaTransition(dc, m_rcGrid[iX][iY], gdiFrom, rcCell, gdiTo, rcCell, iTime);

    ReleaseDC(m_hWnd, dc);

    return TRUE;
}

BOOL CDlgSudoku::FadeOutCell(int iX, int iY, int iTime )
{
    if(m_hWnd == NULL)
        return FALSE;
    if(m_gdiBackground.GetDC() == NULL)
        return FALSE;

    HDC dc = GetDC(m_hWnd);

    if(dc == NULL)
        return FALSE;

    //save original 
    CIssGDIEx gdiFrom;
    CIssGDIEx gdiTo;

    gdiFrom.Create(dc, m_rcGrid[iX][iY], TRUE);

    //draw the background
    ::Draw(m_gdiBackground, m_rcGrid[iX][iY], g_gui->gdiBackground,
        m_rcGrid[iX][iY].left, m_rcGrid[iX][iY].top);
    //draw the cell onto the background gdi
    //DrawCell(m_gdiBackground, iX, iY, TRUE);
    gdiTo.Create(m_gdiBackground.GetDC(), m_rcGrid[iX][iY], TRUE);

    RECT rcCell;
    SetRect(&rcCell, 0, 0, WIDTH(m_rcGrid[0][0]), HEIGHT(m_rcGrid[0][0]));

    AlphaTransition(dc, m_rcGrid[iX][iY], gdiFrom, rcCell, gdiTo, rcCell, iTime);

    ReleaseDC(m_hWnd, dc);

    return TRUE;

}

BOOL CDlgSudoku::AnimateEndGame()
{
    int iTemp = rand()%5;

    switch(iTemp)
    {
    case 0:
        AnimateEndGameSelector();
    	break;
    case 1:
        AnimateEndGameSolitaire();
    	break;
    case 2:
        AnimateEndGameFireworks();
        break;
    case 3:
        AnimateEndGameFireworks2();
    case 4:
    default:
        AnimateEndGameFade();
        break;
    }


    return TRUE;
}

BOOL CDlgSudoku::AnimateSolve()
{
    HDC hDC = GetDC(m_hWnd);

    //get it off screen
    m_ptSelector.x = -1;
    m_ptSelector.y = -1;

    for(int y = 0; y < m_oGame.GetGridSize(); y++)
    {
        for(int x = 0; x < m_oGame.GetGridSize(); x++)
        {
            if(m_oGame.IsGiven(x, y))
                continue;
            if(m_oGame.IsBlank(x, y))
                continue;
            if(m_oGame.IsKakuro(x, y))
                continue;
            //so we are going to be animating some cells we don't need to ... but I don't care much
            FadeTransitionCell(x, y, 180);
            if(ShouldAbort())
                goto Error;
        }
    }
    Sleep(200);

Error:
    ReleaseDC(m_hWnd, hDC);

    return TRUE;    
}

COLORREF CDlgSudoku::BlendColors(COLORREF cr1, COLORREF cr2, int iWeight)
{
    int iR = GetRValue(cr1)*iWeight/100 + GetRValue(cr2)*(100-iWeight)/100;
    int iG = GetGValue(cr1)*iWeight/100 + GetGValue(cr2)*(100-iWeight)/100;
    int iB = GetBValue(cr1)*iWeight/100 + GetBValue(cr2)*(100-iWeight)/100;

    return RGB(iR, iG, iB);


}

COLORREF CDlgSudoku::GetRandomFireColor()
{
    int iColor = rand()%8;

    COLORREF cr1;

    switch(iColor)
    {
    case 0:
        cr1 = 0xCC66FF;//pink
        break;
    case 1:
        cr1 = 0x00FF00;//green
        break;
    case 2:
        cr1 = 0x00FFFF;//yellow
        break;
    case 3:
        cr1 = 0x0000FF;//red
        break;
    case 4:
        cr1 = 0xFFFFFF;//white
        break;
    case 5:
        cr1 = 0x00CCFF;//orange
        break;
    case 6:
        cr1 = 0xFF9900;//light blue
        break;
    case 7:
        cr1 = 0xFF66CC;//light purple;
        break;


    }

 /*   switch(iColor)
    {
    case 0:
        cr1 = 0x9314ff;
        break;
    case 1:
        cr1 = 0xFF00FF;
        break;
    case 2:
        cr1 = 0x22FF22;
        break;
    case 3:
        cr1 = 0x2222FF;
        break;
    case 4:
        cr1 = 0xFF2222;
        break;
    default:
        cr1 = 0xFFFFFF;
        break;
    }*/

    return cr1;

}

BOOL CDlgSudoku::AnimateEndGameFireworks()
{
#define NUM_BITS 64
#define VELOCITY 0.8f
#define NUM_EXP 5

    //POINT ptBits[NUM_BITS];
    float fVx[NUM_BITS];
    float fVy[NUM_BITS];
    float fX[NUM_BITS];
    float fY[NUM_BITS];

    float fSize = 8.0; //seems ok for now

    float fScale = (float)GetSystemMetrics(SM_CXICON)/32.0f;

    COLORREF cr;

    CIssGDIEx gdiTemp, gdiBack;

    FadeToBlank();

    HDC dc = GetDC(m_hWnd);

    gdiTemp.Create(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), FALSE);
    gdiBack.Create(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), TRUE);


    RECT rc;

    for(int e = 0; e < NUM_EXP; e++)
    {
        fSize = 8.0f * fScale;

        cr = GetRandomFireColor();
 
        int iXStart = GetSystemMetrics(SM_CXSCREEN)/2 - 32 + rand()%64;
        int iYStart = GetSystemMetrics(SM_CXSCREEN)/4 - 32 + rand()%64;

        PlaySounds(_T("IDR_WAV_FIREWORKS"));

        for(int i = 0; i < NUM_BITS; i++)
        {
            fX[i] = (float)iXStart;
            fY[i] = (float)iYStart;

            fVx[i] = sin((float)i/(float)NUM_BITS*(6.284f))*VELOCITY*(0.8f + (float)(rand()%40)/200.0f);
            fVy[i] = cos((float)i/(float)NUM_BITS*(6.284f))*VELOCITY*(float)(rand()%50)/50.0f - 0.5;

            fVx[i] *= fScale;
            fVy[i] *= fScale;
        }

        while(fSize > 1.0)
        {
            //blank the screen
            BitBlt(gdiTemp.GetDC(), 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiBack.GetDC(), 0, 0, SRCCOPY);
           
            //draw the fireworks
            for(int i = 0; i < NUM_BITS; i++)
            {
                rc.left = (int)fX[i];
                rc.top = (int)fY[i];
                rc.right = rc.left + (int)fSize;
                rc.bottom = rc.top + (int)fSize;

                FillRect(gdiTemp, rc, cr);
            }

            BitBlt(dc, 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp.GetDC(), 0, 0, SRCCOPY);

            //update positions
            for(int i = 0; i < NUM_BITS; i++)
            {
                fX[i] += fVx[i];
                fY[i] += fVy[i];

                fVy[i] += (0.03f*fScale); //gravity ... ish
                
            }
            
            //adjust timing
            fSize -= (0.1f*fScale); //just testing

            if(ShouldAbort())
                goto error;
            Sleep(5);
        }
    }

error:

    ReleaseDC(m_hWnd, dc);

    return TRUE;
}


BOOL CDlgSudoku::AnimateEndGameFireworks2()
{
#define NUM_BITS 64
#define VELOCITY 0.8f
#define NUM_EXP 5

    //POINT ptBits[NUM_BITS];
    float fVx[NUM_BITS];
    float fVy[NUM_BITS];
    float fX[NUM_BITS];
    float fY[NUM_BITS];

    float fSize = 8.0; //seems ok for now

    float fScale = (float)GetSystemMetrics(SM_CXICON)/32.0f;

    COLORREF cr, cr1, cr2;

    CIssGDIEx gdiTemp, gdiBack;

    FadeToBlank();

    HDC dc = GetDC(m_hWnd);

    gdiTemp.Create(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), FALSE);
    gdiBack.Create(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), TRUE);


    RECT rc;

    int iMaxSize = (int)(8.0f * fScale);

    for(int e = 0; e < NUM_EXP; e++)
    {
        fSize = 8.0f * fScale;

        cr1 = GetRandomFireColor();
        cr2 = GetRandomFireColor();
        
        int iXStart = GetSystemMetrics(SM_CXSCREEN)/2 - 32 + rand()%64;
        int iYStart = GetSystemMetrics(SM_CXSCREEN)/4 - 32 + rand()%64;

        PlaySounds(_T("IDR_WAV_FIREWORKS"));

        for(int i = 0; i < NUM_BITS; i++)
        {
            fX[i] = (float)iXStart;
            fY[i] = (float)iYStart;

            fVx[i] = sin((float)i/(float)NUM_BITS*(6.284f))*VELOCITY*(0.8f + (float)(rand()%40)/200.0f);
            fVy[i] = cos((float)i/(float)NUM_BITS*(6.284f))*VELOCITY*(float)(rand()%50)/50.0f - 0.5;

            fVx[i] *= fScale;
            fVy[i] *= fScale;
        }

        while(fSize > 1.0)
        {
            //blank the screen
            BitBlt(gdiTemp.GetDC(), 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiBack.GetDC(), 0, 0, SRCCOPY);

            cr = BlendColors(cr1, cr2, (int)fSize*100/iMaxSize);

            //draw the fireworks
            for(int i = 0; i < NUM_BITS; i++)
            {
                rc.left = (int)fX[i];
                rc.top = (int)fY[i];
                rc.right = rc.left + (int)fSize;
                rc.bottom = rc.top + (int)fSize;

                FillRect(gdiTemp, rc, cr);
            }

            BitBlt(dc, 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp.GetDC(), 0, 0, SRCCOPY);

            //update positions
            for(int i = 0; i < NUM_BITS; i++)
            {
                fX[i] += fVx[i];
                fY[i] += fVy[i];

                fVy[i] += (0.03f*fScale); //gravity ... ish

            }

            //adjust timing
            fSize -= (0.1f*fScale); //just testing

            if(ShouldAbort())
                goto error;
            Sleep(5);
        }
    }

error:

    ReleaseDC(m_hWnd, dc);

    return TRUE;
}




BOOL CDlgSudoku::AnimateEndGameSelector()
{
    HDC hDC = GetDC(m_hWnd);

    for(int y = 0; y < m_oGame.GetGridSize(); y++)
    {
        for(int x = 0; x < m_oGame.GetGridSize(); x++)
        {
            if(m_oGame.IsBlank(x, y) && m_sOptions.bDrawBlank == FALSE)
                continue;

            m_ptSelector.x = x;
            m_ptSelector.y = y;

            if((x+y)%3)
                PlaySounds(_T("IDR_WAV_DING"));
            FadeTransitionCell(x, y, 100);

            if(ShouldAbort())
                goto Error;
        }
    }

Error:
    FadeToBlank();
    ReleaseDC(m_hWnd, hDC);

    return TRUE;    
}

BOOL CDlgSudoku::FadeToBlank()
{
    HDC hDC = GetDC(m_hWnd);

    //now fade to blank
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    ::Draw(m_gdiBackground, m_rcGrid[0][0].left, m_rcGrid[0][0].top,
        m_rcGrid[m_oGame.GetGridSize() -1][0].right -  m_rcGrid[0][0].left,
        m_rcGrid[0][m_oGame.GetGridSize() -1].bottom -  m_rcGrid[0][0].top,
        g_gui->gdiBackground,
        m_rcGrid[0][0].left, m_rcGrid[0][0].top, ALPHA_Normal);



    BitBlt(g_gui->gdiMem.GetDC(), 0, 0, WIDTH(rcClient), HEIGHT(rcClient), hDC, 0, 0, SRCCOPY);
    AlphaTransition(hDC, rcClient, g_gui->gdiMem, rcClient, m_gdiBackground, rcClient);
    ReleaseDC(m_hWnd, hDC);
    return TRUE;
}

//based off of solitaire
//not complete
BOOL CDlgSudoku::AnimateEndGameBounce()
{
    POINT ptV;
    POINT pt, ptOld;

    int dxp, dyp;

    int iTileWidth		= m_rcGrid[0][0].right - m_rcGrid[0][0].left;
    int iTileHeight		= m_rcGrid[0][0].bottom - m_rcGrid[0][0].top;

    RECT rc;
    GetClientRect(m_hWnd, &rc);

    HDC hDC = GetDC(m_hWnd);

    dxp		= rc.right;
    dyp		= rc.bottom - iTileHeight;

    for(int y = 0; y < m_oGame.GetGridSize(); y++)
    {
        for(int x = 0; x < m_oGame.GetGridSize(); x++)
        {
            //    if(m_oGame.GetTileType(x,y) != TP_Kakuro)
            //        continue;

            if(m_oGame.IsBlank(x, y) && m_sOptions.bDrawBlank == FALSE)
                continue;

            PlaySounds(_T("IDR_WAV_POP"));

            pt.x	= m_rcGrid[x][y].left;
            pt.y	= m_rcGrid[x][y].top;

            ptV.x = rand() % 110 - 65;  // favor up
            if(abs(ptV.x) < 15)  /* kludge so doesn't bounce forever */
            {
                if(ptV.x < 0)
                    ptV.x = -20;
                else
                    ptV.x = 20;
            }
            ptV.y = rand() % 110 - 75;

            while(pt.x > -iTileWidth && pt.x < dxp)
            {
                BitBlt(hDC,
                    pt.x, pt.y,
                    iTileWidth, iTileHeight,
                    m_gdiBackground.GetDC(),
                    m_rcGrid[x][y].left,m_rcGrid[x][y].top,
                    SRCCOPY);
                pt.x += ptV.x/10;
                pt.y += ptV.y/10;
                ptV.y+= 3;
                if(pt.y > dyp && ptV.y > 0)
                    ptV.y = -(ptV.y*8)/10;

                if(ShouldAbort())
                    goto Error;

                
            }
        }
    }

Error:

    ReleaseDC(m_hWnd, hDC);
    return TRUE;
}

BOOL CDlgSudoku::AnimateEndGameSolitaire()
{
    POINT ptV;
    POINT pt;

    int dxp, dyp;

    int iTileWidth		= m_rcGrid[0][0].right - m_rcGrid[0][0].left;
    int iTileHeight		= m_rcGrid[0][0].bottom - m_rcGrid[0][0].top;

    RECT rc;
    GetClientRect(m_hWnd, &rc);

    HDC hDC = GetDC(m_hWnd);

    dxp		= rc.right;
    dyp		= rc.bottom - iTileHeight;

    for(int y = 0; y < m_oGame.GetGridSize(); y++)
    {
        for(int x = 0; x < m_oGame.GetGridSize(); x++)
        {
        //    if(m_oGame.GetTileType(x,y) != TP_Kakuro)
        //        continue;

            if(m_oGame.IsBlank(x, y) && m_sOptions.bDrawBlank == FALSE)
                continue;

            PlaySounds(_T("IDR_WAV_POP"));

            pt.x	= m_rcGrid[x][y].left;
            pt.y	= m_rcGrid[x][y].top;

            ptV.x = rand() % 110 - 65;  // favor up
            if(abs(ptV.x) < 15)  /* kludge so doesn't bounce forever */
            {
                if(ptV.x < 0)
                    ptV.x = -20;
                else
                    ptV.x = 20;
            }
            ptV.y = rand() % 110 - 75;

            while(pt.x > -iTileWidth && pt.x < dxp)
            {
                BitBlt(hDC,
                    pt.x, pt.y,
                    iTileWidth, iTileHeight,
                    m_gdiBackground.GetDC(),
                    m_rcGrid[x][y].left,m_rcGrid[x][y].top,
                    SRCCOPY);
                pt.x += ptV.x/10;
                pt.y += ptV.y/10;
                ptV.y+= 3;
                if(pt.y > dyp && ptV.y > 0)
                    ptV.y = -(ptV.y*8)/10;

                if(ShouldAbort())
                    goto Error;
            }
        }
    }

Error:

    ReleaseDC(m_hWnd, hDC);
    return TRUE;
}

BOOL CDlgSudoku::AnimateEndGameScale()
{
    HDC dc = GetDC(m_hWnd);
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    CIssGDIEx gdiTemp;
    CIssGDIEx gdiScale; //we'll scale to this bad boy

    gdiTemp.Create(dc, m_rcGrid[5][5], TRUE);

    //draw the background
    ::Draw(m_gdiBackground, m_rcGrid[5][5], g_gui->gdiBackground,
        m_rcGrid[5][5].left, m_rcGrid[5][5].top);
    //draw the cell onto the background gdi
    DrawCell(m_gdiBackground, 5, 5, TRUE);
    gdiTemp.Create(m_gdiBackground.GetDC(), m_rcGrid[5][5], TRUE);

    SIZE szScale;
    szScale.cx = gdiTemp.GetWidth()+1;
    szScale.cy = gdiTemp.GetHeight()+1;

    int iXPos, iYPos;

    //lets see how fast this is ....
    while(szScale.cx < WIDTH(rcClient))
    {
        //ScaleImageFast(gdiTemp, gdiScale, szScale, FALSE, 0);  

        iXPos = (WIDTH(rcClient) - szScale.cx)/2;
        iYPos = (HEIGHT(rcClient) - szScale.cy)/2;



        StretchBlt(dc, iXPos, iYPos, szScale.cx, szScale.cy, gdiTemp.GetDC(), 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight(), SRCCOPY);

        //lets just bitblt to try it
       // BitBlt(dc, iXPos, iYPos, gdiScale.GetWidth(), gdiScale.GetHeight(), gdiScale.GetDC(), 0, 0, SRCCOPY);

        szScale.cx += (IsVGA()?10:5);
        szScale.cy += (IsVGA()?10:5);
    }

    ReleaseDC(m_hWnd, dc);
    return TRUE;
}

BOOL CDlgSudoku::AnimateEndGameFade()
{
    int iTotal = m_oGame.GetGridSize()*m_oGame.GetGridSize();
    byte btArray[MAX_GRID_SIZE*MAX_GRID_SIZE];
    byte btTemp;

    //initialize byte array in order
    for(byte i = 0; i < iTotal; i++)
        btArray[i] = i;
    
    //randomize byte array
    for(int j = 0; j < 250; j++)//should be plenty
    {
        int iSwap = rand()%(iTotal-1);
        btTemp = btArray[iSwap];
        btArray[iSwap] = btArray[iSwap+1];
        btArray[iSwap+1] = btTemp;
    }

    PlaySounds(_T("IDR_WAVE_HINT"));

    BOOL bDown = rand()%2;

    HDC hDC = GetDC(m_hWnd);

    int x, y;

    for(int k = 0; k < iTotal; k++)
    {
        if(bDown)
        {
            y = btArray[k]/m_oGame.GetGridSize();
            x = btArray[k]%m_oGame.GetGridSize();
        }
        else
        {
            x = btArray[k]/m_oGame.GetGridSize();
            y = btArray[k]%m_oGame.GetGridSize();
        }
        
        if(m_oGame.IsBlank(x, y) && m_sOptions.bDrawBlank == FALSE)
            continue;

        //remove the cell
        FadeOutCell(x, y, 100);

        if(ShouldAbort())
            goto Error;
    }

    Sleep(300);

Error:
    ReleaseDC(m_hWnd, hDC);

    return TRUE;   
}

void CDlgSudoku::ResetSelector()
{
    m_ptSelector.x = 0;
    m_ptSelector.y = 0;

    if(m_oGame.GetGameType() == STYPE_Kakuro)
    {
        while(m_oGame.IsBlank(m_ptSelector.x, m_ptSelector.y))
        {
            m_ptSelector.x++;
            if(m_ptSelector.x >= m_oGame.GetGridSize())
            {
                m_ptSelector.x = 0;
                break;
            }
        }
    }
}