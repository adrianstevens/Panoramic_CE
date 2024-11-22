#include "CardMenu.h"
#include "Resource.h"

#ifdef WIN32_PLATFORM_WFSP
#define MENU_HEIGHT (GetSystemMetrics(SM_CXICON))
#define MENU_ALPHA_HEIGHT (GetSystemMetrics(SM_CXICON)*2)
#define TEXT_INDENT (GetSystemMetrics(SM_CXICON)/8)
#else
#define MENU_HEIGHT (GetSystemMetrics(SM_CXICON)*5/4)
#define MENU_ALPHA_HEIGHT (GetSystemMetrics(SM_CXICON)*9/4)
#define TEXT_INDENT (GetSystemMetrics(SM_CXICON)/8)
#endif

#define MENU_BCOLOR 0x2C2D2F

#define TEXT_COLOR 0xFFFFFF
#define TEXT_DIMMED 0x999999
#define TEXT_GREEN 0x8DEEA4//0x8DD3A4  

CCardMenu::CCardMenu(void)
:m_eSelBetting(BET_25)
,m_eSelGamePlay(GAMEBTN_Hit)
#ifdef WIN32_PLATFORM_WFSP
,m_bShowSelector(TRUE)
#else
,m_bShowSelector(FALSE)
#endif
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, 700, TRUE);
    m_oStr = CIssString::Instance();
}

CCardMenu::~CCardMenu(void)
{
    CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CCardMenu::DrawSelector(CIssGDIEx& gdi, RECT& rc, CIssBJGame* oGame)
{
    if(m_bShowSelector == FALSE || oGame->IsFinishedDealing() == FALSE)
        return FALSE;

    static RECT rcTemp;

//    rcTemp = rcLocation;

    if(oGame->GetGameState()==GSTATE_Bet ||
        oGame->GetGameState()==GSTATE_EndOfGame)
    {
        if((int)m_eSelBetting < 0)
            return FALSE;

        if(m_eSelBetting < BET_Bet)
            rcTemp = m_rcTopText[m_eSelBetting];
        else if(m_eSelBetting < BET_Count)
            rcTemp = m_rcBottomText[m_eSelBetting - 4];
    }
    else if(oGame->GetGameState() != GSTATE_DealerMove)
    {
        if((int)m_eSelGamePlay < 0)
            return FALSE;

        if(m_eSelGamePlay < GAMEBTN_Insurance)
            rcTemp = m_rcTopText[m_eSelGamePlay];
        else if(m_eSelGamePlay < GAMEBTN_Count)
            rcTemp = m_rcBottomLrg[m_eSelGamePlay - 4];

        if(m_eSelGamePlay == GAMEBTN_Menu)
            rcTemp = m_rcBottomText[3];
    }
    else
    {
        return FALSE;
    }

    rcTemp.left++;
    rcTemp.right -= 2;

    FrameRect(gdi.GetDC(), rcTemp, 0xFFFFFF, 1);


    return TRUE;
}

BOOL CCardMenu::DrawMenu(CIssGDIEx& gdi, RECT& rc, CIssBJGame* oGame)
{
    if(m_gdiMenu.GetDC() == NULL)
    {   //we've got some work to do ...
        RECT rcTemp;
        SetRect(&rcTemp, 0, 0, WIDTH(m_rcMenu), HEIGHT(m_rcMenu));

        m_gdiMenu.Create(gdi.GetDC(), WIDTH(rcTemp), HEIGHT(rcTemp), FALSE, TRUE);        

        FillRect(m_gdiMenu, rcTemp, 0x2C2A28);
        InflateRect(&rcTemp, -1, -1);
        FillRect(m_gdiMenu, rcTemp, 0x545655);
        rcTemp.top++;

        rcTemp.bottom = HEIGHT(m_rcMenu)/2;
        GradientFillRect(m_gdiMenu, rcTemp, 0x4A4D4B, 0x171C1D);

        rcTemp.top = rcTemp.bottom;
        rcTemp.bottom = HEIGHT(m_rcMenu)-1;
        FillRect(m_gdiMenu, rcTemp, 0);
    }

    BOOL bDrawLines = FALSE;

    switch(oGame->GetGameState())
    {
    case GSTATE_PlayerFirstMove:
    case GSTATE_PlayerMove:
    case GSTATE_SplitMove:
        bDrawLines = TRUE;
        {
            AlphaFillRect(gdi, m_rcMenuAlpha, MENU_BCOLOR, 167);
            FrameRect(gdi.GetDC(), m_rcMenuAlpha, MENU_BCOLOR);
            
            DrawText(gdi.GetDC(), _T("Hit"), m_rcTopText[MENU_Hit], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Hit"), m_rcTopText[MENU_Hit], DT_CENTER | DT_VCENTER);
            
            DrawText(gdi.GetDC(), _T("Stand"), m_rcTopText[MENU_Stand], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Stand"), m_rcTopText[MENU_Stand], DT_CENTER | DT_VCENTER);
            
            if(oGame->ShowDoubleButton())
                DrawText(gdi.GetDC(), _T("Double"), m_rcTopText[MENU_Double], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
                //m_fntWhite->DrawText(gdi, _T("Double"), m_rcTopText[MENU_Double], DT_CENTER | DT_VCENTER);
            else
                DrawText(gdi.GetDC(), _T("Double"), m_rcTopText[MENU_Double], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGray->DrawText(gdi, _T("Double"), m_rcTopText[MENU_Double], DT_CENTER | DT_VCENTER);

            if(oGame->ShowSplitButton())
                DrawText(gdi.GetDC(), _T("Split"), m_rcTopText[MENU_Split], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
                //m_fntWhite->DrawText(gdi, _T("Split"), m_rcTopText[MENU_Split], DT_CENTER | DT_VCENTER);
            else
                DrawText(gdi.GetDC(), _T("Split"), m_rcTopText[MENU_Split], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGray->DrawText(gdi, _T("Split"), m_rcTopText[MENU_Split], DT_CENTER | DT_VCENTER);

            break;
        }
    case GSTATE_Bet:
        bDrawLines = TRUE;
        {
            AlphaFillRect(gdi, m_rcMenuAlpha, MENU_BCOLOR, 167);
            FrameRect(gdi.GetDC(), m_rcMenuAlpha, MENU_BCOLOR);

            if(oGame->Show25())
                DrawText(gdi.GetDC(), _T("$25"), m_rcTopText[MENU_25], DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
            else
                DrawText(gdi.GetDC(), _T("$25"), m_rcTopText[MENU_25], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGreen->DrawText(gdi, _T("$25"), m_rcTopText[MENU_25], DT_CENTER | DT_VCENTER);
            if(oGame->Show50())
                DrawText(gdi.GetDC(), _T("$50"), m_rcTopText[MENU_50], DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
            else
                DrawText(gdi.GetDC(), _T("$50"), m_rcTopText[MENU_50], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGreen->DrawText(gdi, _T("$50"), m_rcTopText[MENU_50], DT_CENTER | DT_VCENTER);
            if(oGame->Show100())
                DrawText(gdi.GetDC(), _T("$100"), m_rcTopText[MENU_100], DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
            else
                DrawText(gdi.GetDC(), _T("$100"), m_rcTopText[MENU_100], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGreen->DrawText(gdi, _T("$100"), m_rcTopText[MENU_100], DT_CENTER | DT_VCENTER);
            if(oGame->Show500())
                DrawText(gdi.GetDC(), _T("$500"), m_rcTopText[MENU_500], DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
            else
                DrawText(gdi.GetDC(), _T("$500"), m_rcTopText[MENU_500], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
                //m_fntGreen->DrawText(gdi, _T("$500"), m_rcTopText[MENU_500], DT_CENTER | DT_VCENTER);
            break;
        }
    case GSTATE_EndOfGame:
        if(oGame->IsFinishedDealing())
        {
            AlphaFillRect(gdi, m_rcMenuAlpha, MENU_BCOLOR, 167);
            FrameRect(gdi.GetDC(), m_rcMenuAlpha, MENU_BCOLOR);

            if(oGame->IsSplitting())
            {   //two scores yo
                TCHAR szPlayer[STRING_NORMAL];
                TCHAR szSplit[STRING_NORMAL];
                TCHAR szResults[STRING_NORMAL*2];

                switch(oGame->GetPlayerResult())
                {
                case GRESULT_Win:
                    m_oStr->StringCopy(szPlayer, _T("Win"));
                    break;
                case GRESULT_Lose:
                    m_oStr->StringCopy(szPlayer, _T("Lose"));
                    break;
                case GRESULT_BlackJack:
                    m_oStr->StringCopy(szPlayer, _T("Blackjack!"));
                    break;
                case GRESULT_Surrender:
                    m_oStr->StringCopy(szPlayer, _T("Surrender"));
                default:
                case GRESULT_Draw:
                    m_oStr->StringCopy(szPlayer, _T("Draw"));
                    break;
                }

                switch(oGame->GetSplitResult())
                {
                case GRESULT_Win:
                    m_oStr->StringCopy(szSplit, _T("Win"));
                    break;
                case GRESULT_Lose:
                    m_oStr->StringCopy(szSplit, _T("Lose"));
                    break;
                case GRESULT_BlackJack:
                    m_oStr->StringCopy(szSplit, _T("Blackjack!"));
                    break;
                case GRESULT_Surrender:
                    m_oStr->StringCopy(szSplit, _T("Surrender"));
                default:
                case GRESULT_Draw:
                    m_oStr->StringCopy(szSplit, _T("Draw"));
                    break;
                }

                m_oStr->Format(szResults, _T("%s - %s"), szPlayer, szSplit);
                DrawText(gdi.GetDC(), szResults, m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
           }

            else
            {
                switch(oGame->GetPlayerResult())
                {
                case GRESULT_Win:
                    if(oGame->DealerBusts())
                        DrawText(gdi.GetDC(), _T("Dealer Busts - Win!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Dealer Busts - Win!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                    else
                        DrawText(gdi.GetDC(), _T("Win!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Win!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                    break;
                case GRESULT_Lose:
                    DrawText(gdi.GetDC(), _T("Lose"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Lose"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                    break;
                case GRESULT_BlackJack:
                    DrawText(gdi.GetDC(), _T("Blackjack!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Blackjack!"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                    break;
                case GRESULT_Draw:
                    DrawText(gdi.GetDC(), _T("Draw"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Draw"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                    break;
                case GRESULT_Surrender:
                    DrawText(gdi.GetDC(), _T("Surrender"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
                    //m_fntGreen->DrawText(gdi, _T("Surrender"), m_rcMenuAlphaText, DT_CENTER | DT_VCENTER);
                default:
                    break;
                }
            }


        }
        break;
    }

    BitBlt(gdi.GetDC(), m_rcMenu.left, m_rcMenu.top, WIDTH(m_rcMenu), HEIGHT(m_rcMenu),
        m_gdiMenu.GetDC(), 0, 0, SRCCOPY);

    if(bDrawLines == TRUE)
    {
        HPEN hPenGrey = CreatePen(0, 1, RGB(84,85,86));
        HPEN hPenOld = (HPEN)SelectObject(gdi.GetDC(), hPenGrey);

        POINT ptLine[2];

        ptLine[0].y = m_rcMenuAlphaText.top;
        ptLine[1].y = m_rcMenuAlphaText.bottom;
    
        for(int i = 0; i < 3; i++)
        {
            ptLine[0].x = m_rcTopText[i].right;
            ptLine[1].x = m_rcTopText[i].right;

            Polyline(gdi.GetDC(), ptLine, 2);
        }

        SelectObject(gdi.GetDC(), hPenOld);
        DeleteObject(hPenGrey);
    }

    bDrawLines = FALSE;

    switch(oGame->GetGameState())
    {
    
    case GSTATE_EndOfGame:
    case GSTATE_Bet:
        bDrawLines = TRUE;
        if(oGame->GetGameState() == GSTATE_Bet)
            DrawText(gdi.GetDC(), _T("Bet"), m_rcBottomText[MENU_Bet], DT_CENTER | DT_VCENTER, m_hFont, TEXT_GREEN);
        else
            DrawText(gdi.GetDC(), _T("Bet"), m_rcBottomText[MENU_Bet], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
        //m_fntWhite->DrawText(gdi, _T("Bet"), m_rcBottomText[MENU_Bet], DT_CENTER | DT_VCENTER);
        if(oGame->ShowDealButton())
            DrawText(gdi.GetDC(), _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER);
        else
            DrawText(gdi.GetDC(), _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
            //m_fntGray->DrawText(gdi, _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER);
        if(oGame->ShowDealButton())
            DrawText(gdi.GetDC(), _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER);
        else
            DrawText(gdi.GetDC(), _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
            //m_fntGray->DrawText(gdi, _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER);

        break;
    
        break;
    case GSTATE_PlayerFirstMove:
    case GSTATE_PlayerMove:
    case GSTATE_SplitMove:
        if(oGame->ShowInsuranceButton())
            DrawText(gdi.GetDC(), _T("Insurance"), m_rcBottomLrg[0], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Insurance"), m_rcBottomLrg[0], DT_CENTER | DT_VCENTER);
        else
            DrawText(gdi.GetDC(), _T("Insurance"), m_rcBottomLrg[0], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
            //m_fntGray->DrawText(gdi, _T("Insurance"), m_rcBottomLrg[0], DT_CENTER | DT_VCENTER);
        if(oGame->ShowSurrenderButton())
            DrawText(gdi.GetDC(), _T("Surrender"), m_rcBottomLrg[1], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
            //m_fntWhite->DrawText(gdi, _T("Surrender"), m_rcBottomLrg[1], DT_CENTER | DT_VCENTER);
        else
            DrawText(gdi.GetDC(), _T("Surrender"), m_rcBottomLrg[1], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
            //m_fntGray->DrawText(gdi, _T("Surrender"), m_rcBottomLrg[1], DT_CENTER | DT_VCENTER);
        break;
    case GSTATE_DealerMove:
    case GSTATE_Deal:
        bDrawLines = TRUE;
        DrawText(gdi.GetDC(), _T("Bet"), m_rcBottomText[MENU_Bet], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
        //m_fntGray->DrawText(gdi, _T("Bet"), m_rcBottomText[MENU_Bet], DT_CENTER | DT_VCENTER);
        DrawText(gdi.GetDC(), _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
        //m_fntGray->DrawText(gdi, _T("Deal"), m_rcBottomText[MENU_Deal], DT_CENTER | DT_VCENTER);
        DrawText(gdi.GetDC(), _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER, m_hFont, TEXT_DIMMED);
        //m_fntGray->DrawText(gdi, _T("Reset"), m_rcBottomText[MENU_Reset], DT_CENTER | DT_VCENTER);
        break;
    default:
        break;
    }

    DrawText(gdi.GetDC(), _T("Menu"), m_rcBottomText[MENU_Menu], DT_CENTER | DT_VCENTER, m_hFont, TEXT_COLOR);
    //m_fntWhite->DrawText(gdi, _T("Menu"), m_rcBottomText[MENU_Menu], DT_CENTER | DT_VCENTER);

    HPEN hPenGrey = CreatePen(0, 1, RGB(84,85,86));
    HPEN hPenDark = CreatePen(0, 1, RGB(26,30,31));
    HPEN hPenOld = (HPEN)SelectObject(gdi.GetDC(), hPenDark);

    POINT ptLine[2];

    ptLine[0].y = m_rcMenuText.top - TEXT_INDENT;
    ptLine[1].y = m_rcMenuText.bottom + TEXT_INDENT;

    RECT* rcMenu = NULL;
    if(bDrawLines)
        rcMenu = m_rcBottomText;
    else 
        rcMenu = m_rcBottomLrg;

    for(int i = 0; i < 3; i++)
    {
        if(bDrawLines == FALSE && i == 2)
            continue;
        
        ptLine[0].x = rcMenu[i].right;
        ptLine[1].x = rcMenu[i].right;

        Polyline(gdi.GetDC(), ptLine, 2);
    }

    SelectObject(gdi.GetDC(), hPenGrey);

    for(int i = 0; i < 3; i++)
    {
        if(bDrawLines == FALSE && i == 2)
            continue;

        ptLine[0].x = rcMenu[i].right+1;
        ptLine[1].x = rcMenu[i].right+1;

        Polyline(gdi.GetDC(), ptLine, 2);
    }

    SelectObject(gdi.GetDC(), hPenOld);
    DeleteObject(hPenGrey);
    DeleteObject(hPenDark);

    DrawSelector(gdi, rc, oGame);


    return TRUE;
}

BOOL CCardMenu::OnSize(HWND hWnd)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    m_gdiMenu.Destroy();

    int iWidth = 0;

    //menus
    m_rcMenuAlpha = rcClient;
    m_rcMenuAlpha.top = m_rcMenuAlpha.bottom - MENU_ALPHA_HEIGHT;

    m_rcMenu.bottom = m_rcMenuAlpha.bottom - TEXT_INDENT;
    m_rcMenu.left = m_rcMenuAlpha.left + TEXT_INDENT;
    m_rcMenu.right = m_rcMenuAlpha.right - TEXT_INDENT;
    m_rcMenu.top = m_rcMenu.bottom - MENU_HEIGHT;

    m_rcMenuText = m_rcMenu;
    InflateRect(&m_rcMenuText, -1*TEXT_INDENT, -1*TEXT_INDENT);

    iWidth = WIDTH(m_rcMenuText)/4;
    m_rcBottomText[0] = m_rcMenuText;
    m_rcBottomText[0].right = m_rcBottomText[0].left + iWidth;

    m_rcBottomText[1] = m_rcMenuText;
    m_rcBottomText[1].left = m_rcBottomText[0].right;
    m_rcBottomText[1].right = m_rcBottomText[1].left + iWidth;

    m_rcBottomText[2] = m_rcMenuText;
    m_rcBottomText[2].left = m_rcBottomText[1].right;
    m_rcBottomText[2].right = m_rcBottomText[2].left + iWidth;

    m_rcBottomText[3] = m_rcMenuText;
    m_rcBottomText[3].left = m_rcBottomText[2].right;

    iWidth = (WIDTH(m_rcMenuText) - iWidth)/2;
    m_rcBottomLrg[0] = m_rcMenuText;
    m_rcBottomLrg[0].right = m_rcBottomLrg[0].left + iWidth;

    m_rcBottomLrg[1] = m_rcMenuText;
    m_rcBottomLrg[1].left = m_rcBottomLrg[0].right;
    m_rcBottomLrg[1].right = m_rcBottomText[3].left;


    m_rcMenuAlphaText = m_rcMenuAlpha;
    InflateRect(&m_rcMenuText, -1*TEXT_INDENT, -2*TEXT_INDENT);
    m_rcMenuAlphaText.bottom = m_rcMenu.top - TEXT_INDENT;


    m_rcMenuAlphaText.top += TEXT_INDENT;
 //   m_rcMenuAlphaText.left += TEXT_INDENT;
 //   m_rcMenuAlphaText.right -= TEXT_INDENT;

    iWidth = WIDTH(m_rcMenuAlphaText)/4;
    m_rcTopText[0] = m_rcMenuAlphaText;
    m_rcTopText[0].right = m_rcTopText[0].left + iWidth;

    m_rcTopText[1] = m_rcMenuAlphaText;
    m_rcTopText[1].left = m_rcTopText[0].right;
    m_rcTopText[1].right = m_rcTopText[1].left + iWidth;

    m_rcTopText[2] = m_rcMenuAlphaText;
    m_rcTopText[2].left = m_rcTopText[1].right;
    m_rcTopText[2].right = m_rcTopText[2].left + iWidth;

    m_rcTopText[3] = m_rcMenuAlphaText;
    m_rcTopText[3].left = m_rcTopText[2].right;

    return TRUE;

}

BOOL CCardMenu::OnLButtonUp(HWND hWnd, POINT pt, CIssBJGame* oGame)
{
    m_bShowSelector = FALSE;

    if(pt.y < m_rcMenuAlphaText.top)
        return FALSE;

    if(PtInRect(&m_rcMenuAlphaText, pt))
    {
        switch(oGame->GetGameState())
        {
        case GSTATE_Bet:
            if(PtInRect(&m_rcTopText[MENU_25], pt))
            {
                if(oGame->Add25ChipBet())
                    PostMessage(hWnd, MESSAGE_25, 0,0);
            }
            else if(PtInRect(&m_rcTopText[MENU_50], pt))
            {
                if(oGame->Add50ChipBet())
                    PostMessage(hWnd, MESSAGE_50, 0,0);
            }
            else if(PtInRect(&m_rcTopText[MENU_100], pt))
            {
                if(oGame->Add100ChipBet())
                PostMessage(hWnd, MESSAGE_100, 0,0);
            }
            else if(PtInRect(&m_rcTopText[MENU_500], pt))
            {
                if(oGame->Add500ChipBet())
                    PostMessage(hWnd, MESSAGE_500, 0,0);
            }
            break;
        case GSTATE_PlayerFirstMove:
        case GSTATE_PlayerMove:
        case GSTATE_SplitMove:
            if(PtInRect(&m_rcTopText[MENU_Hit], pt))
                oGame->Hit();
            if(PtInRect(&m_rcTopText[MENU_Stand], pt))
                oGame->Stand();
            if(PtInRect(&m_rcTopText[MENU_Double], pt))
                oGame->DoubleDown();
            if(PtInRect(&m_rcTopText[MENU_Split], pt))
                oGame->Split();
            break;
        case GSTATE_EndOfGame:
        case GSTATE_DealerMove:
            break;
        case GSTATE_Deal:
            break;
        }
    }
    else if(PtInRect(&m_rcMenuText, pt))
    {
        switch(oGame->GetGameState())
        {
        case GSTATE_EndOfGame:
            if(PtInRect(&m_rcBottomText[MENU_Bet], pt))
                oGame->Bet();
        case GSTATE_Bet:
            if(PtInRect(&m_rcBottomText[MENU_Deal], pt))
            {
                oGame->Deal();
            }
            else if(PtInRect(&m_rcBottomText[MENU_Reset], pt))
            {
                oGame->ResetBetAmount();
            }

            break;
        case GSTATE_PlayerFirstMove:
        case GSTATE_PlayerMove:
        case GSTATE_SplitMove:
            if(PtInRect(&m_rcBottomLrg[0], pt))
                oGame->Insurance();
            else if(PtInRect(&m_rcBottomLrg[1], pt))
                oGame->Surrender();
            break;
        case GSTATE_DealerMove:
            break;
        case GSTATE_Deal:
            break;
        }
    }

    if(PtInRect(&m_rcBottomText[3], pt))//Menu
        PostMessage(hWnd, WM_COMMAND, IDMENU_Menu, 0);


    //its in our area so we're "handling" it for now
    return TRUE;
}

BOOL CCardMenu::OnKeyUp(HWND hWnd, WPARAM wParam, CIssBJGame* oGame)
{
    if(oGame->IsFinishedDealing() == FALSE)
        return FALSE;

    m_bShowSelector = TRUE;

    if(oGame->GetGameState()==GSTATE_Bet ||
        oGame->GetGameState()==GSTATE_EndOfGame)
    {
        if(oGame->GetGameState() == GSTATE_EndOfGame)
        {
            if(wParam == VK_UP || wParam == VK_DOWN)
                oGame->Bet();
        }

        switch(wParam)
        {
        case VK_RIGHT:
            if(m_eSelBetting == BET_Menu)
                m_eSelBetting = BET_25;
            else
                m_eSelBetting = (EnumBetting)(m_eSelBetting + 1);
            break;
        case VK_LEFT:
            if(m_eSelBetting == BET_25)
                m_eSelBetting = BET_Menu;
            else
                m_eSelBetting = (EnumBetting)(m_eSelBetting - 1);
            break;
        case VK_UP:
            if(m_eSelBetting > 3)
                m_eSelBetting = (EnumBetting)(m_eSelBetting - 4);
            else
                m_eSelBetting = (EnumBetting)(m_eSelBetting + 4);
            break;
        case VK_DOWN:
            if(m_eSelBetting > 3)
                m_eSelBetting = (EnumBetting)(m_eSelBetting - 4);
            else
                m_eSelBetting = (EnumBetting)(m_eSelBetting + 4);
            break;
        case VK_RETURN:
            {
                switch(m_eSelBetting)
                {
                case BET_25:
                    if(oGame->Add25ChipBet())
                        PostMessage(hWnd, MESSAGE_25, 0,0);
                    break;
                case BET_50:
                    if(oGame->Add50ChipBet())
                        PostMessage(hWnd, MESSAGE_50, 0,0);
                    break;
                case BET_100:
                    if(oGame->Add100ChipBet())
                        PostMessage(hWnd, MESSAGE_100, 0,0);
                    break;
                case BET_500:
                    if(oGame->Add500ChipBet())
                        PostMessage(hWnd, MESSAGE_500, 0,0);
                    break;
                case BET_Bet:
                    oGame->Bet();
                    break;
                case BET_Deal:
                    oGame->Deal();
                    break;
                case BET_Clear:
                    oGame->ResetBetAmount();
                    break;
                case BET_Menu:
                    PostMessage(hWnd, WM_COMMAND, IDMENU_Menu, 0);
                	break;
                default:
                    break;
                }
            }
            break;
        }
    }
    else if(oGame->GetGameState() != GSTATE_DealerMove)
    {
        switch(wParam)
        {
        case VK_RIGHT:
            if(m_eSelGamePlay == GAMEBTN_Menu)
                m_eSelGamePlay = GAMEBTN_Hit;
            else
                m_eSelGamePlay = (EnumGameplayButtons)(m_eSelGamePlay + 1);
            break;
        case VK_LEFT:
            if(m_eSelGamePlay == GAMEBTN_Hit)
                m_eSelGamePlay = GAMEBTN_Menu;
            else
                m_eSelGamePlay = (EnumGameplayButtons)(m_eSelGamePlay - 1);
            break;
        case VK_UP:
            if(m_eSelGamePlay > 3)
                m_eSelGamePlay = (EnumGameplayButtons)(m_eSelGamePlay - 4);
            else
                m_eSelGamePlay = (EnumGameplayButtons)(m_eSelGamePlay + 4);
            if((int)m_eSelGamePlay > 7)
                m_eSelGamePlay = GAMEBTN_Menu;
            break;
        case VK_DOWN:
            if(m_eSelGamePlay > 3)
            {
                if(m_eSelGamePlay == GAMEBTN_Menu)
                    m_eSelGamePlay = GAMEBTN_Split;
                else if(m_eSelGamePlay == GAMEBTN_Insurance)
                    m_eSelGamePlay = GAMEBTN_Stand;
                else 
                    m_eSelGamePlay = GAMEBTN_Hit;
            }
            else
            {
                if(m_eSelGamePlay < 2)
                    m_eSelGamePlay = GAMEBTN_Surrender;
                else if(m_eSelGamePlay == 2)
                    m_eSelGamePlay = GAMEBTN_Insurance;
                else
                    m_eSelGamePlay = GAMEBTN_Menu;
            }
            break;
        
        case VK_RETURN:
            {
                switch(m_eSelGamePlay)
                {
                case GAMEBTN_Hit:
                    oGame->Hit();
            	    break;
                case GAMEBTN_Stand:
                    oGame->Stand();
            	    break;
                case GAMEBTN_Double:
                    oGame->DoubleDown();
                    break;
                case GAMEBTN_Split:
                    oGame->Split();
                    break;
                case GAMEBTN_Surrender:
                    oGame->Surrender();
            	    break;
                case GAMEBTN_Insurance:
                    oGame->Insurance();
            	    break;
                case GAMEBTN_Menu:
                    PostMessage(hWnd, WM_COMMAND, IDMENU_Menu, 0);
                    break;
                default:
                    break;
                }
            }

            break;
        }
    }    
    
    return TRUE;
}