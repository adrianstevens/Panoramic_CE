#include "ObjGui.h"
#include "Resource.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "winuserm.h"


#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Selected      RGB(149,149,149)

CObjGui::CObjGui(void)
:m_hWndParent(NULL)
,m_hInst(NULL)
,eMenuSelect(MENU_None)
,m_oStr(CIssString::Instance())
,iXCellOff(0)
,iYCellOff(0)
{
    hFontLarge    = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE);

    sSkin.eSkin = SSKIN_Count;
    eGameType   = STYPE_Sudoku;

    SetRect(&rcCell, 0, 0, 0, 0);

    szWindow.cx = 0;
    szWindow.cy = 0;
}

CObjGui::~CObjGui(void)
{
    Destroy();
    CIssGDIEx::DeleteFont(hFontLarge);
}

void CObjGui::Destroy()
{
    gdiMem.Destroy();
    gdiBottomBar.Destroy();
    gdiBackground.Destroy();
    gdiMenuArray.Destroy();
    imgSelector.Destroy();
    imgGlow.Destroy();
}


void CObjGui::Init(HWND hWndParent, HINSTANCE hInst)
{
//    Destroy();
    m_hInst = hInst;
    m_hWndParent = hWndParent;

    LoadTitle();
}

void CObjGui::InitGDI(RECT& rcClient, HDC dc)
{
    if(m_hWndParent == NULL || m_hInst == NULL)
        return;

    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
        return;

    if(WIDTH(rcClient) == gdiMem.GetWidth() && HEIGHT(rcClient) == gdiMem.GetHeight())
        return;

    gdiMem.Create(dc, WIDTH(rcClient), HEIGHT(rcClient), FALSE, TRUE);

    InitBottomBar(rcClient);
    if(gdiBackground.GetDC() == NULL)
        LoadBackground(eGameType);
    //InitBackground(rcClient);

    if(!imgSelector.IsLoaded())
        imgSelector.Initialize(m_hWndParent, m_hInst, IDB_PNG_MenuSelector);

    if(!imgGlow.IsLoaded())
        imgGlow.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_MenuGlow_HR:IDB_PNG_MenuGlow);

    if(!gdiMenuArray.GetDC())
        gdiMenuArray.LoadImage(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray, m_hWndParent, m_hInst, TRUE);


    if(wndMenu.GetBackground() == IDR_PNG_MenuBack &&
        wndMenu.GetSelected() == IDB_PNG_MenuSelector &&
        wndMenu.GetImageArray() == (IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray))
        return;

    wndMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    wndMenu.SetBackground(IDR_PNG_MenuBack);
    wndMenu.SetSelected(IDB_PNG_MenuSelector);
    wndMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    wndMenu.PreloadImages(m_hWndParent, m_hInst);
}

void CObjGui::DrawScreen(RECT& rcClip, HDC dc)
{
    BitBlt(dc,
        rcClip.left, rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        gdiMem.GetDC(),
        rcClip.left, rcClip.top,
        SRCCOPY);
}

void CObjGui::DrawBottomBar(RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed /* = FALSE */, BOOL bRightMenuGrayed /* = FALSE */)
{
    DrawBottomBar(gdiMem, rcClip, szLeftMenu, szRightMenu, bLeftMenuGrayed, bRightMenuGrayed);
}

void CObjGui::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed, BOOL bRightMenuGrayed)
{
    if(!IsRectInRect(rcClip, rcBottomBar))
        return;

    Draw(gdi, rcBottomBar, gdiBottomBar, 0, 0);

    if(imgGlow.GetWidth() != WIDTH(rcBottomBar)/2 || imgGlow.GetHeight() != HEIGHT(rcBottomBar))
        imgGlow.SetSize(WIDTH(rcBottomBar)/2, HEIGHT(rcBottomBar));

    RECT rcText = rcBottomBar;
    if(szLeftMenu)
    {
        if(eMenuSelect == MENU_Left)
            imgGlow.DrawImage(gdi, rcBottomBar.left, rcBottomBar.top);

        rcText.right    = rcText.right/2;
        ::DrawText(gdi, szLeftMenu, rcText, DT_CENTER | DT_VCENTER | DT_NOPREFIX, hFontLarge, 0xDDDDDD);
    }

    rcText = rcBottomBar;
    if(szRightMenu)
    {
        if(eMenuSelect == MENU_Right)
            imgGlow.DrawImage(gdi, rcBottomBar.right/2, rcBottomBar.top);

        rcText.left    = rcText.right/2;
        ::DrawText(gdi, szRightMenu, rcText, DT_CENTER | DT_VCENTER | DT_NOPREFIX, hFontLarge, 0xDDDDDD);
    }
}

void CObjGui::DrawTitle(RECT& rcClip)
{
    if(!IsRectInRect(rcClip, rcBackground))
        return;

    RECT rc;
    IntersectRect(&rc, &rcClip, &rcBackground);

    Draw(gdiMem,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        gdiTitle,
        rc.left, rc.top - rcBackground.top);
}


void CObjGui::DrawBackground(RECT& rcClip, EnumSudokuType eSudoku)
{
    if(!IsRectInRect(rcClip, rcBackground))
        return;

    RECT rc;
    IntersectRect(&rc, &rcClip, &rcBackground);

  //  if(sSkin.eSkin == SSKIN_Notepad)
  //      if(eSudoku != STYPE_Sudoku || m_bUseNotepadBG == FALSE)
        {
            FillRect(gdiMem.GetDC(), rc, 0xFFFFFF);
            return;
        }

    Draw(gdiMem,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        gdiBackground,
        rc.left, rc.top - rcBackground.top);
}


void CObjGui::InitBottomBar(RECT& rcClient)
{
    if(m_hWndParent == NULL || m_hInst == NULL || gdiBottomBar.GetWidth() == WIDTH(rcClient))
        return;

    CIssGDIEx gdiBar;


    gdiBar.LoadImage(IsVGA()?IDB_PNG_Menu_HR:IDB_PNG_Menu, m_hWndParent, m_hInst, TRUE);

    gdiBottomBar.Create(gdiMem, WIDTH(rcClient), gdiBar.GetHeight(), FALSE, TRUE);
    gdiBottomBar.InitAlpha(TRUE);

    Draw(gdiBottomBar, 0,0, gdiBar.GetWidth(), gdiBar.GetHeight(), gdiBar, 0,0, ALPHA_Copy);
    gdiBar.FlipVertical();
    Draw(gdiBottomBar, WIDTH(rcClient) - gdiBar.GetWidth(), 0, gdiBar.GetWidth(), gdiBar.GetHeight(), gdiBar, 0,0, ALPHA_Copy);

    if(WIDTH(rcClient) - 2*gdiBar.GetWidth() > 0)
    {
        CIssGDIEx gdiMiddle;
        gdiMiddle.LoadImage(IsVGA()?IDB_PNG_MenuMid_HR:IDB_PNG_MenuMid, m_hWndParent, m_hInst, TRUE);
        RECT rc;
        SetRect(&rc, gdiBar.GetWidth(), 0, WIDTH(rcClient)-gdiBar.GetWidth(), gdiBar.GetHeight());
        TileBackGround(gdiBottomBar, rc, gdiMiddle, TRUE);
    }

    rcBottomBar = rcClient;
    rcBottomBar.top = rcBottomBar.bottom - gdiBar.GetHeight();
}

BOOL CObjGui::OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
    if(eMenuSelect != MENU_None)
    {   
        if(PtInRect(&rcBottomBar, pt))
        {
            if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2 && eMenuSelect == MENU_Left)
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
            else if(pt.x >= GetSystemMetrics(SM_CXSCREEN)/2 && eMenuSelect == MENU_Right)
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        }

        eMenuSelect = MENU_None;
        // redraw the menu
        InvalidateRect(hWnd, &rcBottomBar, FALSE);
        ::UpdateWindow(hWnd);
        return TRUE;
    }
    return FALSE;
}

BOOL CObjGui::OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
    eMenuSelect = MENU_None;
    if(PtInRect(&rcBottomBar, pt))
    {        
        if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2)
            eMenuSelect = MENU_Left;
        else
            eMenuSelect = MENU_Right;
        // redraw the menu
        InvalidateRect(hWnd, &rcBottomBar, FALSE);
        ::UpdateWindow(hWnd);
        return TRUE;
    }
    return FALSE;
}


BOOL CObjGui::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {   
    case VK_TSOFT1:
        eMenuSelect = MENU_Left;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
        break;
    case VK_TSOFT2:
        eMenuSelect = MENU_Right;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        break;
    default:
        return UNHANDLED;
    }

    // redraw the menu
    InvalidateRect(hWnd, &rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    eMenuSelect = MENU_None;
    InvalidateRect(hWnd, &rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    return TRUE;
}

BOOL CObjGui::OnSize(HWND hWnd, RECT rcClient)
{
    if(WIDTH(rcClient) == szWindow.cx && 
        HEIGHT(rcClient) == szWindow.cy)
        return TRUE;

    //we'll keep the loads to a minimum
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
        return TRUE;
    if(HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return TRUE;

    szWindow.cx = WIDTH(rcClient);
    szWindow.cy = HEIGHT(rcClient);

    if(WIDTH(rcClient) != gdiMem.GetWidth())
    {
        HDC dc = GetDC(hWnd);        
        InitGDI(rcClient, dc);
        ReleaseDC(hWnd, dc);
    }
    gdiMem.Destroy();
    gdiTitle.Destroy();
    gdiBackground.Destroy();
    LoadTitle();
    SetSkin();//just the default
    return TRUE;

}

BOOL CObjGui::LoadSkin(EnumSudokuSkins eSkin, EnumBackground eBackground, BOOL bForceReload)
{
    BOOL bReload = FALSE;

    //lots of reasons to reload
    if(bForceReload == TRUE)
        bReload = TRUE;
    else if(GetSystemMetrics(SM_CXSCREEN) != gdiBackground.GetWidth() ||
        GetSystemMetrics(SM_CYSCREEN) != gdiBackground.GetHeight())
        bReload = TRUE;
    else if(eSkin != sSkin.eSkin)
        bReload = TRUE;
    else if(eBackground != sSkin.eBackground)
        bReload = TRUE;


    if(bReload == FALSE)
        return TRUE;

    if(eSkin == SSKIN_Count)
        return FALSE;//so we can init the gui before loading the skin

    sSkin.eSkin = eSkin;
    sSkin.eBackground = eBackground;

    sSkin.uiCellArray2 = IsVGA()?IDB_PNG_SQS_Killer_HR:IDB_PNG_SQS_Killer;
    sSkin.iNumOutlineColors = 1;
    sSkin.crOutline1 = 0;//black for the custom skins
    sSkin.crKakuroTotals = 0;

    switch(eSkin)
    {
    case SSKIN_Glass:
        sSkin.crPencil1      =  0xDDDDDD;
        sSkin.crPencil2      =  0x444444;
        sSkin.crPencilKakuro =  0xDDDDDD;  
        sSkin.crSelector     =  0x00FFFF;
        sSkin.crTopText      =  0xFFFFFF; //RGB(108,168,225);
        sSkin.crTotal1       =  0;//killer sudoku total values
        sSkin.crTotal2       =  0;//killer sudoku total values
        sSkin.crOutline1     =  RGB(255,153,0);
        sSkin.crOutline2     =  RGB(0,204,0);
        sSkin.crOutline3     =  RGB(245,0,255);
        sSkin.iNumOutlineColors = 3;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Glass_HR:IDB_PNG_SQS_Glass;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntGlassWhite_HR:IDB_PNG_BtnFntGlassWhite;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntGlassGray_HR:IDB_PNG_BtnFntGlassGray;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  RGB(12,51,104);
        sSkin.crGrad2       =  0;


        break;
    case SSKIN_Wood:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x222222;
        sSkin.crPencilKakuro=  0xDDDDDD;  
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF;
        sSkin.crTotal1      =  0x00FFFF;
        sSkin.crTotal2      =  0;
        sSkin.crOutline1     =  0x00FFFF;
        sSkin.crKakuroTotals = 0xFFFFFF;

        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Wood_HR:IDB_PNG_SQS_Wood;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntGrayWhite_HR:IDB_PNG_BtnFntGrayWhite;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntGrayBlack_HR:IDB_PNG_BtnFntGrayBlack;


        if(GetSystemMetrics(SM_CXICON) > 32)//320x320 loads the big bg
            sSkin.uiBackground = IDB_PNG_BGWood_HR;
        else
            sSkin.uiBackground  =  IDB_PNG_BGWood;
        sSkin.crGrad1       =  0;
        sSkin.crGrad2       =  0;
    	break;
    case SSKIN_Notepad:
        sSkin.crPencil1     =  0x666666;
        sSkin.crPencil2     =  0x666666;
        sSkin.crPencilKakuro=  0x666666;  
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0x666666;
        sSkin.crTotal1      =  0xFFFFFF;
        sSkin.crTotal2      =  0xFFFFFF;
        sSkin.crOutline1    =  RGB(0,0,255);
        sSkin.crOutline2    =  RGB(255,0,0);
        sSkin.crKakuroTotals = 0x444444;
        sSkin.crOutline3    =  0x444444;
        sSkin.iNumOutlineColors = 3;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Notepad_HR:IDB_PNG_SQS_Notepad;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntNoteBlue_HR:IDB_PNG_BtnFntNoteBlue;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntNoteRed_HR:IDB_PNG_BtnFntNoteRed;

        sSkin.uiBackground  =  IsVGA()?IDB_PNG_BGNotepad_HR:IDB_PNG_BGNotepad;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;

        //lets handle this here


        break;
    case SSKIN_Grey:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0xDDDDDD;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0xDDDDDD;
        sSkin.crTotal1      =  0xFFFFFF;
        sSkin.crTotal2      =  0;
        sSkin.crOutline1    =  RGB(60,153,247);
        sSkin.crOutline2    =  RGB(0,204,0);
        sSkin.crOutline3    =  RGB(245,0,255);
        sSkin.iNumOutlineColors = 3;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Gray_HR:IDB_PNG_SQS_Gray;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntGrayWhite_HR:IDB_PNG_BtnFntGrayWhite;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntGrayBlack_HR:IDB_PNG_BtnFntGrayBlack;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  RGB(40,44,49);
        sSkin.crGrad2       =  RGB(103,111,122);
        break;
    case SSKIN_Custom0:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0x444444;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1    =  0x666666;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom0_HR:IDB_PNG_SQS_Custom0;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntGlassWhite_HR:IDB_PNG_BtnFntGlassWhite;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntGlassGray_HR:IDB_PNG_BtnFntGlassGray;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;

        break;
    case SSKIN_Custom1:
        sSkin.crPencil1     =  0x333333;
        sSkin.crPencil2     =  0x333333;
        sSkin.crPencilKakuro=  0x333333;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1     =  0;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom1_HR:IDB_PNG_SQS_Custom1;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntCWhite2_HR:IDB_PNG_BtnFntCWhite2;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;
        
        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;

    	break;
    case SSKIN_Custom2:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0x444444;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1     =  0;
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom2_HR:IDB_PNG_SQS_Custom2;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntCWhite2_HR:IDB_PNG_BtnFntCWhite2;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;

    	break;
    case SSKIN_Custom3:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0x444444;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1     =  0x666666;
        sSkin.iSelAlpha     =  100; 
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom3_HR:IDB_PNG_SQS_Custom3;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntCWhite2_HR:IDB_PNG_BtnFntCWhite2;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;
        break;
    case SSKIN_Custom4:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0x444444;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1     =  0x666666;
        sSkin.iSelAlpha     =  100; 
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom4_HR:IDB_PNG_SQS_Custom4;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;
        break;
    case SSKIN_Custom5:
        sSkin.crPencil1     =  0xDDDDDD;
        sSkin.crPencil2     =  0x444444;
        sSkin.crPencilKakuro=  0x444444;
        sSkin.crSelector    =  0x00FFFF;
        sSkin.crTopText     =  0xFFFFFF; //0;
        sSkin.crTotal1      =  0x666666;
        sSkin.crTotal2      =  0x666666;
        //sSkin.crOutline1     =  0x666666;
        sSkin.iSelAlpha     =  100; 
        sSkin.uiCellArray   =  IsVGA()?IDB_PNG_SQS_Custom5_HR:IDB_PNG_SQS_Custom5;
        sSkin.uiFont1       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;
        sSkin.uiFont2       =  IsVGA()?IDB_PNG_BtnFntCBlack2_HR:IDB_PNG_BtnFntCBlack2;

        sSkin.uiBackground  =  0;
        sSkin.crGrad1       =  0xFFFFFF;
        sSkin.crGrad2       =  0xFFFFFF;
            
        break;
    default: 
        return FALSE;
        break;
    }

    if(WIDTH(rcCell) > 0)
    {
        sSkin.gdiSQs.Destroy();
        sSkin.gdiFont1.Destroy();
        sSkin.gdiFont2.Destroy();
                
        SetCellSize(rcCell, iGridSize, eGameType);
    }

    LoadBackground(eGameType);

    return TRUE;
}

void CObjGui::SetCellSize(RECT& rcSize, int GridSize, EnumSudokuType eType)
{
    eGameType = eType;

    if(HEIGHT(rcSize) == sSkin.gdiSQs.GetHeight())
        return;
    if(m_hInst == NULL || m_hWndParent == NULL)
        return;
    if(sSkin.uiCellArray == 0)
        return;

    BOOL bScale = TRUE;
    if(GridSize == 9 && 
        GetSystemMetrics(SM_CXSCREEN)%240 == 0)
        bScale = FALSE;


    rcCell = rcSize;
    iGridSize = GridSize;

    //lets scale these puppies
    CIssGDIEx gdiTemp;
    SIZE size;

    //squares first
    if(sSkin.uiCellArray == -1)
    {
        sSkin.gdiSQs.Destroy();
    }
    else 
    {
        size.cy = HEIGHT(rcSize);
        size.cx = WIDTH(rcSize)*SIZE_CELLARRAY2;
        gdiTemp.LoadImage(sSkin.uiCellArray2, m_hWndParent, m_hInst);
        ::ScaleImage(gdiTemp, sSkin.gdiSQs2, size, FALSE, 0);

        size.cx = WIDTH(rcSize)*SIZE_CELLARRAY;
        gdiTemp.LoadImage(sSkin.uiCellArray, m_hWndParent, m_hInst);
        ::ScaleImage(gdiTemp, sSkin.gdiSQs, size, FALSE, 0);

      /*  if(bScale)
        {
            double dbScale = (double)(HEIGHT(rcSize))/(double)(gdiTemp.GetHeight());//what else can I do???

            gdiTemp.LoadImage(sSkin.uiFont1, m_hWndParent, m_hInst);
            size.cx = (int)((double)(gdiTemp.GetWidth())*dbScale/9.0);
            size.cy = (int)((double)(gdiTemp.GetHeight())*dbScale);
            size.cx *= 9;
            ::ScaleImage(gdiTemp, sSkin.gdiFont1, size, FALSE, 0);

            gdiTemp.LoadImage(sSkin.uiFont2, m_hWndParent, m_hInst);
            size.cx = (int)((double)(gdiTemp.GetWidth())*dbScale/9.0);
            size.cy = (int)((double)(gdiTemp.GetHeight())*dbScale);
            iFontWidth = size.cx;//save it now
            size.cx *= 9;
            ::ScaleImage(gdiTemp, sSkin.gdiFont2, size, FALSE, 0);
        }
        else*/
        {
            sSkin.gdiFont1.LoadImage(sSkin.uiFont1, m_hWndParent, m_hInst);
            sSkin.gdiFont2.LoadImage(sSkin.uiFont2, m_hWndParent, m_hInst);
            iFontWidth = sSkin.gdiFont1.GetWidth()/9;
        }
       

        iXCellOff = (WIDTH(rcSize) - iFontWidth)/2;
        iYCellOff = (HEIGHT(rcSize) - sSkin.gdiFont1.GetHeight())/2;
    }

    //we'll call LoadBrackground here for f'n notepad
    if(iGridSize == 9 && sSkin.eSkin == SSKIN_Notepad)
        LoadBackground(eGameType);
}

void CObjGui::NextSkin(EnumSudokuType eType)
{
    eGameType = eType; //for good ol notepad
    int iTemp = sSkin.eSkin;
    iTemp++;

    if(iTemp >= SSKIN_Count)
        iTemp = 0;

    LoadSkin((EnumSudokuSkins)iTemp, sSkin.eBackground);
}

void CObjGui::NextBackground()
{
    int iTemp = sSkin.eBackground;
    iTemp++;

    if(iTemp >= BACKGROUND_Count)
        iTemp;

    LoadSkin(sSkin.eSkin, (EnumBackground)iTemp);
}

BOOL CObjGui::LoadTitle()
{
    if(m_hInst == NULL || m_hWndParent == NULL)
        return FALSE;


    //title counts as background
    if(gdiTitle.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IsVGA()?IDB_PNG_Title_HR:IDB_PNG_Title, m_hWndParent, m_hInst);

        RECT rcTemp;
        SetRect(&rcTemp, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        gdiTitle.Create(gdiTemp.GetDC(), rcTemp);

        int iXOffSet = (gdiTemp.GetWidth() - WIDTH(rcTemp))/2;

        Draw(gdiTitle, rcTemp, gdiTemp, iXOffSet, 0);
    }
    return TRUE;
}

BOOL CObjGui::LoadBackground(EnumSudokuType eType)
{
    if(m_hInst == NULL || m_hWndParent == NULL)
        return FALSE;

    eGameType = eType;

    //first is notepad
    if(sSkin.eSkin == SSKIN_Notepad)
    {
        //has to be normal portrait and a 9x9
        if( UseNotePadBG(eType) )
            LoadBackground(sSkin.uiBackground);
        else
            LoadBackground(sSkin.crGrad1, sSkin.crGrad2);

    }
    else if(sSkin.uiBackground != 0)
        LoadBackground(sSkin.uiBackground);
    else if(sSkin.eBackground == BACKGROUND_Default ||
        sSkin.eSkin == SSKIN_Glass ||
        sSkin.eSkin == SSKIN_Grey)
        LoadBackground(sSkin.crGrad1, sSkin.crGrad2);
    else if(sSkin.eBackground == BACKGROUND_Custom && m_oStr->GetLength(m_szImagePath))
    {
        if(LoadBackground(m_szImagePath) == FALSE)//looks about right
        {
            m_oStr->Empty(m_szImagePath);//clear it and try again .. redundant but that's ok
            return LoadBackground(eGameType);
        }
    }
    else
        LoadBackground(GetBackgroundColor(FALSE), GetBackgroundColor(TRUE));

    return TRUE;
}


BOOL CObjGui::LoadBackground(UINT uiRes)
{
    //m_oStr->Empty(m_szImagePath);
    //BUGBUG rotate wood and notepad for highres land 
    //scale notepad for 320x320
    gdiBackground.Destroy();
    GetClientRect(m_hWndParent, &rcBackground);

    if(sSkin.eSkin == SSKIN_Wood)
    {
        gdiBackground.LoadImage(uiRes, m_hWndParent, m_hInst);
        //might have to rotate
        if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
            gdiBackground.Rotate();//easy enough
    }
    else
    {
        gdiBackground.LoadImage(uiRes, m_hWndParent, m_hInst);
    }
    return TRUE;
}

BOOL CObjGui::LoadBackground(COLORREF crGrad1, COLORREF crGrad2)
{
    //m_oStr->Empty(m_szImagePath);
    gdiBackground.Destroy();

    SetRect(&rcBackground, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    //GetClientRect(m_hWndParent, &rcBackground);
    HDC dc = GetDC(m_hWndParent);

    gdiBackground.Create(dc, rcBackground);

    SetRect(&rcBackground, 0, 0, WIDTH(rcBackground), HEIGHT(rcBackground));
    GradientFillRect(gdiBackground, rcBackground, crGrad1, crGrad2, FALSE);

	ReleaseDC(m_hWndParent, dc);

    return TRUE;
}

BOOL CObjGui::LoadBackground(TCHAR* szFileName)
{
    return SetImageBackground(szFileName);
    return TRUE;
}

BOOL CObjGui::UseFont1(EnumSquares eSquare)
{
    switch(sSkin.eSkin)
    {
    case SSKIN_Glass:
        switch(eSquare)
        {
        case SQ_Board1://black
        case SQ_Guess1:
        case SQ_Highlighted:
        case SQ_Selector:
            return FALSE;
        	break;
        case SQ_Board2://white
        case SQ_Guess2:
        case SQ_Locked:
        case SQ_Given:
        default:
            return TRUE;
        	break;
        }
        break;
    case SSKIN_Wood:
        switch(eSquare)
        {
        case SQ_Highlighted:
        case SQ_Selector:
            return FALSE;
            break;
        case SQ_Board2:
        case SQ_Guess2:
        case SQ_Locked:
        case SQ_Given:
        case SQ_Board1:
        case SQ_Guess1:
        default:
            return TRUE;
            break;
        }
        break;
    case SSKIN_Notepad://blue is true ... red is false
        switch(eSquare)
        {
        case SQ_Given:
            return FALSE;
            break;
        case SQ_Board2:
        case SQ_Guess2:
        case SQ_Locked:
        case SQ_Board1:
        case SQ_Guess1:
        case SQ_Highlighted:
        case SQ_Selector:
        default:
            return TRUE;
            break;
        }
        break;
    case SSKIN_Grey:
        switch(eSquare)
        {
        case SQ_Board1://black
        case SQ_Guess1:
        case SQ_Highlighted:
        case SQ_Selector:
        case SQ_Board2://white
        case SQ_Guess2:
        case SQ_Locked:        
            return FALSE;
            break;
        case SQ_Given:
        default:
            return TRUE;
            break;
        }
        break;
    case SSKIN_Custom1:
        switch(eSquare)
        {
        case SQ_Board1:
        case SQ_Guess1:
        case SQ_Board2:
        case SQ_Guess2:
        case SQ_Highlighted:
        case SQ_Selector:
        case SQ_Given:        
            return FALSE;
            break;
        case SQ_Locked:
        default:
            return TRUE;
            break;
        }
        break;
    case SSKIN_Custom3:
        switch(eSquare)
        {
        case SQ_Board1:
        case SQ_Guess1:
        case SQ_Board2:
        case SQ_Guess2:
        case SQ_Highlighted:
        case SQ_Selector:
        case SQ_Locked:        
            return FALSE;
            break;
        case SQ_Given:
        default:
            return TRUE;
            break;
        }
        break;
    default:
        switch(eSquare)
        {
        case SQ_Board1://black
        case SQ_Guess1:
        case SQ_Board2://white
        case SQ_Guess2:
        case SQ_Highlighted:
        case SQ_Selector:
            return FALSE;
            break;
        case SQ_Locked:
        case SQ_Given:
        default:
            return TRUE;
            break;
        }
        break;
    }
}

COLORREF CObjGui::GetPencilMark(EnumSquares eSquare, EnumSudokuType eType)
{
    if(eType == STYPE_Kakuro)
    {
        if(eSquare == SQ_Selector)
        switch(sSkin.eSkin)
        {
        case SSKIN_Grey:
        case SSKIN_Glass:
        case SSKIN_Wood:
            return sSkin.crPencil2;//dark for the selector
            break;
        case SSKIN_Custom0:
        default:
            break;
        }
        return sSkin.crPencilKakuro;
    }
    else if(eType == STYPE_Killer || eType == STYPE_KenKen)
    {
        if(sSkin.eSkin == SSKIN_Glass)
            return sSkin.crPencilKakuro;
        if(sSkin.eSkin == SSKIN_Grey)
            return sSkin.crPencil1;
    }
    
    if(UseFont1(eSquare))
        return sSkin.crPencil1;
    return sSkin.crPencil2;
}

CIssGDIEx* CObjGui::GetFontGDI(EnumSquares eSquare)
{
    if(UseFont1(eSquare))
        return &sSkin.gdiFont1;
    return &sSkin.gdiFont2;
}

COLORREF CObjGui::GetTotalColor(EnumSquares eSquares)
{
    return sSkin.crPencilKakuro;
//    if(UseFont1(eSquares))
//        return sSkin.crTotal1;
//    return sSkin.crTotal2;
}

//for skin correction based on game and skin
EnumSquares CObjGui::GetSquareEnum(EnumSquares eSquare, EnumSudokuType eType, BOOL bSameRowAsSelector)
{
    switch(eType)
    {
    case STYPE_Kakuro:
        switch(sSkin.eSkin)
        {
        case SSKIN_Glass:
        	break;
        case SSKIN_Grey:
            break;
        case SSKIN_Notepad:
            if(eSquare == SQ_Selector)
                return SQ_Given;//3rd ...
            else if(eSquare == SQ_Kakuro)
                return SQ_Kakuro;//9th
            else
                return SQ_Board2;//2nd
            break;
        case SSKIN_Custom0:
        case SSKIN_Custom1:
        case SSKIN_Custom4:
        case SSKIN_Custom5:
            //if(eSquare == SQ_Board2 || eSquare == SQ_Guess1 || eSquare == SQ_Guess2)
            if(eSquare == SQ_Locked)
                return SQ_Board1;
            break;
        case SSKIN_Custom2:
            if(eSquare == SQ_Locked)
            //if(eSquare == SQ_Board2 || eSquare == SQ_Guess1 
            //   eSquare == SQ_Board1 || eSquare == SQ_Guess2 )
                return SQ_Locked;
            break;
        case SSKIN_Custom3:
            if(eSquare == SQ_Locked)
            //if(eSquare == SQ_Guess1 ||  eSquare == SQ_Board1 || eSquare == SQ_Guess2 )
                return SQ_Board2;
            break;
        default://all custom skins
            break;
        }
    	break;
    case STYPE_KenKen:
    case STYPE_Killer:
        switch(sSkin.eSkin)
        {
        case SSKIN_Glass:
            if(eSquare == SQ_Board1)
                return SQ_Killer3;
            if(eSquare == SQ_Board2)
                return SQ_Killer4;
            if(eSquare == SQ_Selector)
                return SQ_KillerSel2;
            break;
        case SSKIN_Grey:
            if(eSquare == SQ_Board1)
                return SQ_Killer1;
            if(eSquare == SQ_Board2)
                return SQ_Killer2;
         // if(eSquare == SQ_Selector)
         //     return SQ_KillerSel;
            if(eSquare == SQ_Highlighted)
                return SQ_None;//blank
            break;
        case SSKIN_Notepad:
            if(eSquare == SQ_Guess1)
                return SQ_Board1;
            if(eSquare == SQ_Guess2)
                return SQ_Board2;
            if(eSquare == SQ_Highlighted)
                return SQ_None;//blank
            break;
        default:
            break;
        }
        break;    
    case STYPE_Sudoku:
        switch(sSkin.eSkin)
        {
        //only correction
        case SSKIN_Notepad:
            if(UseNotePadBG(eGameType))
            {
                if(eSquare == SQ_Board1 ||
                    eSquare == SQ_Guess1 ||
                    eSquare == SQ_Board2 ||
                    eSquare == SQ_Guess2 ||
                    eSquare == SQ_Given)
                    return SQ_None; //blank over the notepad
            }
            if(eSquare == SQ_Guess1 || eSquare == SQ_Board1 || eSquare == SQ_Given)
                return SQ_Board1;
            if(eSquare == SQ_Guess2 || eSquare == SQ_Board2)
                return SQ_Board2;
            if(eSquare == SQ_Highlighted)
            {
                if(bSameRowAsSelector)
                    return SQ_Locked;
                else
                    return SQ_Highlighted;//here we need to check and sometimes return locked
            }
            if(eSquare == SQ_Selector)
                return SQ_Selector;
             return SQ_None;//blank cell
             break;
        default:
           break;
        }
    default:
        break;
    }


    /*
    switch(eSquare)
    {
    case SQ_Board1:
        break;
    case SQ_Board2:
        break;
    case SQ_Given:
        break;
    case SQ_Guess1:
        break;
    case SQ_Guess2:
        break;
    case SQ_Locked:
        break;
    case SQ_Highlighted:
        break;
    case SQ_Selector:
        break;
    case SQ_Kakuro:
        break;
    default:
        break;
    }*/



    return eSquare;
}

COLORREF CObjGui::GetOutlineColor(int iSection, BOOL bHighlight)
{
    int iColor = iSection%sSkin.iNumOutlineColors;

    if( (sSkin.eSkin == SSKIN_Glass || sSkin.eSkin == SSKIN_Grey) &&
        bHighlight)
        return RGB(255,240,0); //yellow BUGBUG hack

    if(iColor == 0)
        return sSkin.crOutline1;
    if(iColor == 1)
        return sSkin.crOutline2;
    if(iColor == 2)
        return sSkin.crOutline3;
    if(iColor == 3)
        return sSkin.crOutline4;
    return sSkin.crOutline5;
}

COLORREF CObjGui::GetBackgroundColor(BOOL bColor1)
{
    return GetBackgroundColor(bColor1, sSkin.eBackground);
}

COLORREF CObjGui::GetBackgroundColor(BOOL bColor1, EnumBackground eBack)
{
    switch(eBack)
    {
    case BACKGROUND_White:
        return RGB(255,255,255);
        break;
    case BACKGROUND_Black:
        return RGB(0,0,0);
        break;
    case BACKGROUND_Grey:
        return RGB(102,102,102);
        break;
    case BACKGROUND_Green:
        return RGB(146,189,103);
        break;
    case BACKGROUND_LPink:
        return RGB(255,185,209);
        break;
    case BACKGROUND_Orange:
        return RGB(255,145,70);
        break;
    case BACKGROUND_Blue:
        return RGB(104,163,210);
        break;
    case BACKGROUND_DBlue:
        return RGB(40,66,98);
        break;
    case BACKGROUND_DGreen:
        return RGB(49,85,43);
        break;
    case BACKGROUND_Pink:
        return RGB(255,91,149);
        break;
    case BACKGROUND_Purple:
        return RGB(151,118,201);
        break;
    case BACKGROUND_PurpleWhite:
        return bColor1?RGB(106,66,168):0xFFFFFF;
        break;
    case BACKGROUND_BlackWhite:
        return bColor1?RGB(0,0,0):0xFFFFFF;
        break;
    case BACKGROUND_GreenWhite:
        return bColor1?RGB(146,189,103):0xFFFFFF;
        break;
    case BACKGROUND_BlueWhite:
        return bColor1?RGB(104,163,210):0xFFFFFF;
        break;
    case BACKGROUND_DBlueWhite:
        return bColor1?RGB(40,66,98):0xFFFFFF;
        break;
    case BACKGROUND_PinkWhite:
        return bColor1?RGB(255,91,149):0xFFFFFF;
        break;
    case BACKGROUND_OrangeWhite:
        return bColor1?RGB(255,109,8):0xFFFFFF;
        break;
    case BACKGROUND_GreyBlack:
        return bColor1?RGB(153,153,153):0;
        break;
    case BACKGROUND_RedBlack:
        return bColor1?RGB(98,0,0):0;
        break;
    case BACKGROUND_PurpleBlack:
        return bColor1?RGB(98,0,130):0;
        break;
    case BACKGROUND_BlueBlack:
        return bColor1?RGB(40,117,210):0;
        break;
    case BACKGROUND_GreenBlack:
        return bColor1?RGB(88,112,56):0;
        break;
    case BACKGROUND_DBlueBlue:
        return bColor1?RGB(0,18,91):RGB(44,117,170);
        break;
    case BACKGROUND_BlueGreen:
        return bColor1?RGB(44,117,170):RGB(120,186,75);
        break;
    case BACKGROUND_DGReenGreen:
        return bColor1?RGB(0,55,0):RGB(120,186,75);
        break;
    }
    return 0xffffff;
}

BOOL CObjGui::SetImageBackground(TCHAR* szPath)
{
    if(m_oStr->IsEmpty(szPath))
    {
        m_oStr->Empty(m_szImagePath);     
        return FALSE;
    }

    if(m_oStr->Compare(szPath, m_szImagePath) == 0 &&
        gdiBackground.GetDC())
        return TRUE; //already have that one set

    m_oStr->StringCopy(m_szImagePath, szPath);
    
    if(m_hWndParent == NULL)
        return TRUE;//I know it failed but we don't want to erase the path

    if(sSkin.eSkin < SSKIN_Custom0)
        return TRUE;

    //ok we're going to scale regardless because pImage factory isn't so pretty

    if(S_OK != gdiBackground.LoadImage(szPath, m_hWndParent, TRUE, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), ASPECT_Cropped))
    {
        m_oStr->Empty(m_szImagePath);     
        return FALSE;
    }

    sSkin.eBackground = BACKGROUND_Custom;

    return TRUE;

  /*  CIssGDIEx   gdiTemp;
    CIssGDIEx   gdiCrop;
    POINT       ptIndent;

    if(S_OK != gdiTemp.LoadImage(szPath, m_hWndParent))
    {
        m_oStr->Empty(m_szImagePath);     
        return FALSE;
    }

    RECT rcClient;
    SetRect(&rcClient, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
   // GetClientRect(m_hWndParent, &rcClient);

    int iTemp;
    RECT rcImage;

    SetRect(&rcImage, 0, 0, gdiTemp.GetWidth(), gdiTemp.GetHeight());

    if( ((double)gdiTemp.GetWidth()/(double)WIDTH(rcClient)   )  > 
        ((double)gdiTemp.GetHeight()/(double)HEIGHT(rcClient) ) )
    {
        //the sides get cropped
        //calc new width
        iTemp = gdiTemp.GetHeight()*WIDTH(rcClient)/HEIGHT(rcClient);
        ptIndent.x = (gdiTemp.GetWidth() - iTemp)/2;
        ptIndent.y = 0;

        if(ptIndent.x < 0)
            ptIndent.x = 0;

        gdiCrop.Create(gdiTemp.GetDC(), iTemp, gdiTemp.GetHeight());
        
        BitBlt(gdiCrop.GetDC(), 0, 0, gdiCrop.GetWidth(), gdiCrop.GetHeight(),
            gdiTemp.GetDC(), ptIndent.x, ptIndent.y, SRCCOPY);
        
    }
    else //otherwise its perfect or the top and bottom get cropped
    {
        //calc new height
        iTemp = gdiTemp.GetWidth()*HEIGHT(rcClient)/WIDTH(rcClient);
        ptIndent.x = 0;
        ptIndent.y = (gdiTemp.GetHeight()- iTemp)/2;

        if(ptIndent.y < 0)
            ptIndent.y = 0;

        gdiCrop.Create(gdiTemp.GetDC(), gdiTemp.GetWidth(), iTemp);

        BitBlt(gdiCrop.GetDC(), 0, 0, gdiCrop.GetWidth(), gdiCrop.GetHeight(),
            gdiTemp.GetDC(), ptIndent.x, ptIndent.y, SRCCOPY);
    }

    SIZE szTemp;
    szTemp.cx = WIDTH(rcClient);
    szTemp.cy = HEIGHT(rcClient);

    ScaleImage(gdiCrop, gdiBackground, szTemp, FALSE, 0);

    sSkin.eBackground = BACKGROUND_Custom;

    return TRUE;*/
}

BOOL CObjGui::GetImagePath(TCHAR* szPath)
{
    m_oStr->Empty(szPath);

    if(m_oStr->IsEmpty(m_szImagePath) == FALSE)
    {
        m_oStr->StringCopy(szPath, m_szImagePath);
        return TRUE;
    }
    return FALSE;
}

BOOL CObjGui::UseNotePadBG(EnumSudokuType eType)
{
    if(eType != STYPE_Sudoku)
        return FALSE;

    if ( (iGridSize == 9) &&
         (GetSystemMetrics(SM_CXSCREEN)%240 == 0) &&
         (GetSystemMetrics(SM_CXSCREEN) != GetSystemMetrics(SM_CYSCREEN)) )
         return TRUE;
    return FALSE;
}