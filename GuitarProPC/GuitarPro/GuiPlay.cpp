#include "GuiPlay.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

extern TCHAR g_szFrets[18][5];
extern TCHAR g_szRoots[12][8];

#define IDT_FINGER_TIMER 1
#define IDT_FADE_TIMER   2




#define FINGER_TIMEOUT 500 //ms of course ... prolly delete this sucker

CGuiPlay::CGuiPlay(void)
{
    m_ptLastPressed.x = -1;
    m_ptLastPressed.y = -1;

    m_iBottomFret = 0;
}

CGuiPlay::~CGuiPlay(void)
{
}

BOOL CGuiPlay::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL)
        return FALSE;

    g_gui->DrawBackground(rcClip);
    if(m_iBottomFret == 0)
        g_gui->DrawNut(rcClip);
    g_gui->DrawStrings(rcClip, 0);

    DrawText(*g_gui->GetGDI(), rcClip);
    DrawFingers(*g_gui->GetGDI(), rcClip);
    DrawPosButtons(*g_gui->GetGDI(), rcClip);

    BitBlt(hDC,
        rcClip.left, rcClip.top, 
        rcClip.right - rcClip.left,
        rcClip.bottom - rcClip.top,
        g_gui->GetGDI()->GetDC(),
        rcClip.left,
        rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CGuiPlay::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
   // ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_OPT_Shift), m_hInst), m_rcButtons[2], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    ::DrawText(gdi.GetDC(), g_options->GetGen()->GetTuningText((int)g_options->GetTuning(), m_hInst, g_options->GetInstrument()), m_rcButtons[3], DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, g_gui->GetFont(FONT_Button), 0xFFFFFF);

    if(g_options->GetScale() != SCALE_Count)//none
    {
        ::DrawText(gdi.GetDC(), g_szRoots[g_options->GetRoot()], m_rcButtons[0], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
        ::DrawText(gdi.GetDC(), g_options->GetGen()->GetScalesText((int)g_options->GetScale(), m_hInst), m_rcButtons[1], DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    }
    else
    {
        ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_OPT_ScalesOff), m_hInst), m_rcButtons[1], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    }
    
    if(g_options == NULL)
        return FALSE;

    TCHAR szTemp[STRING_LARGE];

    //Fret labels on right
    int iTemp = m_iBottomFret;
    
    static POINT ptFretOff = {(WIDTH(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetWidth())/2, (HEIGHT(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetHeight())/2};

    for(int i = 0; i < GetMaxFrets(); i++)
    {
        if(iTemp + i >= 17 || IsRectInRect(m_rcFretNums[i], rcClip) == FALSE)
            continue;
        if(g_options->GetFretMarkers() == FMARKERS_Numbered)
        {
            m_oStr->IntToString(szTemp, iTemp + i + 1);
            if(m_oStr->GetLength(szTemp))
            {
                ::Draw(gdi, m_rcFretNums[i].left + ptFretOff.x , m_rcFretNums[i].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
                ::DrawText(gdi.GetDC(), szTemp, m_rcFretNums[i], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);
            }
        }
        else
        {
            if(m_oStr->GetLength(g_szFrets[iTemp+i]))
            {
                ::Draw(gdi, m_rcFretNums[i].left + ptFretOff.x, m_rcFretNums[i].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
                ::DrawText(gdi.GetDC(), g_szFrets[iTemp+i], m_rcFretNums[i], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);
            }
        }
    }
    return TRUE;
}


//called from DrawFingers
BOOL CGuiPlay::DrawScale(CIssGDIEx& gdi, RECT& rcClip)
{
#define ALPHA_FINGERS   127 //50%

    DWORD dwTextColor = 0xaaaaaa;//0xaa8888;

    int iTemp;

    for(int s = 0; s < 6; s++)
    {
        if(g_options->GetLeftHanded() == TRUE)
            iTemp = 5 - s;
        else 
            iTemp = s;

        if(iTemp >= g_options->GetGen()->GetNumStrings())
            continue;

        //nut notes
        if(m_iBottomFret == 0 &&
            GetScaleDrawType(iTemp, 0, g_options->GetRoot()) != SDRAW_None)
        {
            int iX = m_rcOpenStrings[s].left + (WIDTH(m_rcOpenStrings[s]) - g_gui->GetFingerGrey()->GetWidth())/2;
            int iY = m_rcOpenStrings[s].top + (HEIGHT(m_rcOpenStrings[s]) - g_gui->GetFingerGrey()->GetHeight())/2;//its fine if its negative

            Draw(gdi, iX, iY, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey(), 
                0, 0, ALPHA_Normal);//nice and faint

            //and draw the note
            ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, 0), m_rcOpenStrings[s], 
                DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);
        }

        for(int f = 0; f < GetMaxFrets(); f++)
        {
            if( IsRectInRect(m_rcFingers[s][f], rcClip) == FALSE)
                continue;

            int iX = m_rcFingers[s][f].left + (WIDTH(m_rcFingers[s][f]) - g_gui->GetFinger()->GetWidth())/2;
            int iY = m_rcFingers[s][f].top + (HEIGHT(m_rcFingers[s][f]) - g_gui->GetFinger()->GetHeight())/2;//its fine if its negative

            switch(GetScaleDrawType(iTemp, f + m_iBottomFret+1, g_options->GetRoot()))
            {
            case SDRAW_Note:
                Draw(gdi, iX, iY, g_gui->GetFingerDark()->GetWidth(), g_gui->GetFingerDark()->GetHeight(), *g_gui->GetFingerDark(), 
                    0, 0, ALPHA_Normal, ALPHA_FINGERS);
                //and draw the note
                ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, f + m_iBottomFret + 1), m_rcFingers[s][f], 
                    DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);
            	break;
            case SDRAW_Root:
                Draw(gdi, iX, iY, g_gui->GetFinger()->GetWidth(), g_gui->GetFinger()->GetHeight(), *g_gui->GetFinger(), 
                    0, 0, ALPHA_Normal, ALPHA_FINGERS);
                //and draw the note
                ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, f + m_iBottomFret + 1), m_rcFingers[s][f], 
                    DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);
                break;
            default:
                break;
            }
        }
    }
    return TRUE;
}

BOOL CGuiPlay::DrawPosButtons(CIssGDIEx& gdi, RECT& rcClip)
{
    static POINT ptFretOff = {(WIDTH(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetWidth())/2, (HEIGHT(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetHeight())/2};
    static POINT ptTri[3];//3 points for the ol triangle

    if(m_iBottomFret > -1 && IsRectInRect(rcClip, m_rcPosition[0] ))
    {
        ::Draw(gdi, m_rcPosition[0].left + ptFretOff.x , m_rcPosition[0].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
        RECT rcTemp = m_rcPosition[0];
        rcTemp.bottom -= (IsVGA()?4:2);//hack 
        
        DrawArrow(gdi.GetDC(), rcTemp, 0x333333, FALSE);
    }
    
    if(m_iBottomFret < (12 - GetMaxFrets())  && IsRectInRect(rcClip, m_rcPosition[1] ))
    {
        ::Draw(gdi, m_rcPosition[1].left + ptFretOff.x , m_rcPosition[1].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
        DrawArrow(gdi.GetDC(), m_rcPosition[1], 0x333333, TRUE);
    }
    
    return TRUE;
}


BOOL CGuiPlay::DrawFingers(CIssGDIEx& gdi, RECT& rcClip)
{
    if(g_options->GetScale() != SCALE_Count)
        DrawScale(gdi, rcClip);

    if(m_ptLastPressed.x == -1)
        return FALSE;


    DWORD dwAlpha = m_iAlpha; //was 159
    DWORD dwTextColor = RGB(m_iAlpha, m_iAlpha, m_iAlpha);//0xFFFFFF; //was 0xaa8888

    int iTemp = m_ptLastPressed.x;
    if(g_options->GetLeftHanded())
        iTemp = 5 - m_ptLastPressed.x;

    int y = m_ptLastPressed.y;

    if(y == -1)//on the nut yo ...
    {
        if(m_iBottomFret != 0)
            return FALSE;//no need

        int iX = m_rcOpenStrings[m_ptLastPressed.x].left + (WIDTH(m_rcOpenStrings[m_ptLastPressed.x]) - g_gui->GetFingerGrey()->GetWidth())/2;
        int iY = m_rcOpenStrings[m_ptLastPressed.x].top + (HEIGHT(m_rcOpenStrings[m_ptLastPressed.x]) - g_gui->GetFingerGrey()->GetHeight())/2;//its fine if its negative

        Draw(gdi, iX, iY, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey(), 
            0, 0, ALPHA_Normal, 159);

        //and draw the note
        ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, m_iBottomFret), m_rcOpenStrings[m_ptLastPressed.x], 
            DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0x333333);
    }

    else
    {
        int iX = m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y].left + (WIDTH(m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y]) - g_gui->GetFinger()->GetWidth())/2;
        int iY = m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y].top + (HEIGHT(m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y]) - g_gui->GetFinger()->GetHeight())/2;//its fine if its negative

        if(g_options->GetScale() != SCALE_Count &&        
            SDRAW_Note == GetScaleDrawType(iTemp, m_ptLastPressed.y + m_iBottomFret+1, g_options->GetRoot()))
        {
            Draw(gdi, iX, iY, g_gui->GetFingerDark()->GetWidth(), g_gui->GetFingerDark()->GetHeight(), *g_gui->GetFingerDark(), 
            0, 0, ALPHA_Normal, dwAlpha);//nice and faint
        }
        else
        {
            Draw(gdi, iX, iY, g_gui->GetFinger()->GetWidth(), g_gui->GetFinger()->GetHeight(), *g_gui->GetFinger(), 
                0, 0, ALPHA_Normal, dwAlpha);//nice and faint
        }

        //and draw the note
        ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, m_ptLastPressed.y + m_iBottomFret + 1), m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y], 
            DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);
    }
    return TRUE;
}

BOOL CGuiPlay::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam >= IDMENU_Scale && wParam < IDMENU_Scale + SCALE_Count + 5)
    {
        g_options->SetScale(EnumScales(wParam - IDMENU_Scale));
        InvalidateRect(hWnd, &m_rcPlayArea, FALSE);
        return TRUE;
    }

    else if(wParam >= IDMENU_Root && wParam < IDMENU_Root + ROOT_Count)
    {   //handle the roots
        g_options->SetRoot(EnumChordRoot(wParam - IDMENU_Root));
        InvalidateRect(hWnd, &m_rcPlayArea, FALSE);
        return TRUE;
    }

    else if(wParam >= IDMENU_Tuning && wParam < IDMENU_Tuning + TUNING_Count)
    {
        //g_options->SetTuning(EnumTuning(wParam - IDMENU_Tuning));
		g_options->SetTuning(wParam - IDMENU_Tuning);
        InvalidateRect(hWnd, &m_rcPlayArea, FALSE);
        return TRUE;
    }
    return UNHANDLED;
}

BOOL CGuiPlay::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(g_gui->GetFinger()->GetDC() == NULL)
        return FALSE;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iTopIndent = BUTTONS_HEIGHT + NUT_HEIGHT;
    int iLeftIndent = 84*GetSysMets(SM_CXSCREEN)/480;
    int iWidth;
    int iHeight;

    //so lets draw some rects for now as fill
    iWidth = (WIDTH(rcClient))/4;

    //top buttons
    SetRect(&m_rcButtons[0], rcClient.left, rcClient.top, rcClient.left + iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[1], rcClient.left + iWidth, rcClient.top, rcClient.left + 2*iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[2], rcClient.left + 2*iWidth, rcClient.top, rcClient.left + 3*iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[3], rcClient.left + 3*iWidth, rcClient.top, rcClient.right, rcClient.top + BUTTONS_HEIGHT);

    //and the grid ... 
    iHeight = IsVGA()?94:47;

    if(GetSysMets(SM_CYSCREEN)%240 == 0)
        iHeight = IsVGA()?62:31;//tweak this
    if(GetSysMets(SM_CYSCREEN) == 480 && GetSysMets(SM_CXSCREEN) == 320)
        iHeight = 58;

    m_rcGrid.left   = iLeftIndent + rcClient.left;
    m_rcGrid.right  = rcClient.right - iLeftIndent;
    m_rcGrid.top    = rcClient.top + iTopIndent;
    m_rcGrid.bottom = m_rcGrid.top + 5*iHeight;

    if(GetSysMets(SM_CYSCREEN)%240 == 0)
        m_rcGrid.top -= IsVGA()?8:4;

    int iString = 0;

    //and now the important part
    for(int x = 0; x < 6; x++)
    {
        iString = x;
        if(g_options->GetLeftHanded())
            iString = 5 - x;//a little strange but correct
        for(int y = 0; y < GetMaxFrets(); y++)
        {
            m_rcFingers[x][y].left = g_gui->GetStringX(iString) - g_gui->GetFinger()->GetWidth()/2;
            m_rcFingers[x][y].right = m_rcFingers[x][y].left + g_gui->GetFinger()->GetWidth();
            m_rcFingers[x][y].top = m_rcGrid.top + HEIGHT(m_rcGrid)*y/5;
            m_rcFingers[x][y].bottom = m_rcFingers[x][y].top + HEIGHT(m_rcGrid)/5;
        }

        SetRect(&m_rcOpenStrings[x], m_rcFingers[x][0].left, rcClient.top + BUTTONS_HEIGHT, m_rcFingers[x][0].right, rcClient.top + BUTTONS_HEIGHT + NUT_HEIGHT);
    }

    for(int y = 0; y < GetMaxFrets(); y++)
    {
        SetRect(&m_rcFretNums[y], rcClient.right - iLeftIndent/2, m_rcFingers[0][y].top, 
            rcClient.right, m_rcFingers[0][y].bottom);
    }

    m_rcPosition[0] = m_rcFretNums[0];
    m_rcPosition[1] = m_rcFretNums[GetMaxFrets()-1];

    m_rcPosition[0].left = rcClient.left;
    m_rcPosition[1].left = rcClient.left;
    m_rcPosition[0].right = rcClient.left + iLeftIndent/2;
    m_rcPosition[1].right = rcClient.left + iLeftIndent/2;

    m_rcPlayArea = rcClient;
    m_rcPlayArea.top += BUTTONS_HEIGHT;
    m_rcPlayArea.bottom -= MENU_HEIGHT;

    return TRUE;
}

BOOL CGuiPlay::OnLButtonUp(HWND hWnd, POINT& pt)
{
    int iString;
    for(int s = 0; s < 6; s++)
    {
        if(g_options->GetLeftHanded())
            iString = 5-s;
        else
            iString = s;

        if(iString >= g_options->GetGen()->GetNumStrings())
            continue;

        for(int f = 0; f < GetMaxFrets(); f++)
        {
            if(PtInRect(&m_rcFingers[s][f], pt))
            {
                //make noise
                if(g_gui && g_options)
                {
                    if(g_gui->GetPlaySounds())
                    {
                        if(g_options->GetLeftHanded())
                            g_gui->PlaySFX(GetNoteRes(iString, f+1+m_iBottomFret) - LOWEST_SOUND_RES);
                        else
                            g_gui->PlaySFX(GetNoteRes(iString, f+1+m_iBottomFret) - LOWEST_SOUND_RES);
                    }

                    g_gui->VibrateString(s);

                    //redraw the previous finger location
                    if(m_ptLastPressed.x != -1 && m_ptLastPressed.y != -1)
                        InvalidateRect(m_hWnd, &m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y], FALSE);
                    else if(m_ptLastPressed.x != -1)
                        InvalidateRect(m_hWnd, &m_rcOpenStrings[m_ptLastPressed.x], FALSE);

                    m_ptLastPressed.x = s;
                    m_ptLastPressed.y = f;

                    InvalidateRect(m_hWnd, &m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y], FALSE);
                    m_iAlpha = 255;
                    UpdateWindow(m_hWnd);
                    KillTimer(m_hWnd, IDT_FINGER_TIMER);
                    SetTimer(m_hWnd, IDT_FINGER_TIMER, FADE_OUT_SPEED, NULL);


                    return TRUE;                    
                }
            }
        }
        //check for the open strings here 
        if(PtInRect(&m_rcOpenStrings[s], pt))
        {
            if(g_gui->GetPlaySounds())
            {
                if(g_options->GetLeftHanded())
                    g_gui->PlaySFX(GetNoteRes(iString, 0) - LOWEST_SOUND_RES); 
                else
                    g_gui->PlaySFX(GetNoteRes(iString, 0) - LOWEST_SOUND_RES); 
            }

            //redraw the previous finger location
            if(m_ptLastPressed.x != -1 && m_ptLastPressed.y != -1)
                InvalidateRect(m_hWnd, &m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y], FALSE);
            else if(m_ptLastPressed.x != -1)
                InvalidateRect(m_hWnd, &m_rcOpenStrings[m_ptLastPressed.x], FALSE);

            m_ptLastPressed.x = s;
            m_ptLastPressed.y = -1;//for open

            InvalidateRect(m_hWnd, &m_rcOpenStrings[m_ptLastPressed.x], FALSE);
            UpdateWindow(m_hWnd);
            KillTimer(m_hWnd, IDT_FINGER_TIMER);
            SetTimer(m_hWnd, IDT_FINGER_TIMER, FINGER_TIMEOUT, NULL);
        }
    }

 /*   if(PtInRect(&m_rcButtons[2], pt))
    {
        OnShift();
        return TRUE;
    }

    else */if(PtInRect(&m_rcButtons[1], pt))
    {
        OnScale();
    }
    else if(PtInRect(&m_rcButtons[0], pt))
    {
        OnRoot();
    }

    else if(PtInRect(&m_rcButtons[3], pt))
    {
        OnTuning();
        return TRUE;
    }

    else if(PtInRect(&m_rcPosition[0], pt))
    {
        OnFretDown();
    }
    else if(PtInRect(&m_rcPosition[1], pt))
    {
        OnFretUp();
    }
    return UNHANDLED;
}

BOOL CGuiPlay::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_UP:
        OnFretDown();
        break;
    case VK_DOWN:
        OnFretUp();
        break;
    case VK_RETURN:
        PlayScale();
        break;
    default:
        return UNHANDLED;
        break;
    }


    return TRUE;
}



UINT CGuiPlay::GetNoteRes(int iString, int iFret)
{
    return g_options->GetGen()->GetNoteRes(iString, iFret);
}

BOOL CGuiPlay::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_FINGER_TIMER)
    {
        if(m_ptLastPressed.y != -1)
        {
            InvalidateRect(hWnd, &m_rcFingers[m_ptLastPressed.x][m_ptLastPressed.y], FALSE);
        }
        else
        {
            InvalidateRect(hWnd, &m_rcOpenStrings[m_ptLastPressed.x], FALSE);
        }

        if(m_iAlpha <= 127)
        {
            KillTimer(hWnd, IDT_FINGER_TIMER);
            m_ptLastPressed.x = -1;
            m_ptLastPressed.y = -1;
        }


        UpdateWindow(hWnd);

        m_iAlpha -= 20;
        return TRUE;
    }

    return UNHANDLED;
}

void CGuiPlay::OnLostFocus()
{
    //just in case
    KillTimer(m_hWnd, IDT_FINGER_TIMER);
}

void CGuiPlay::OnTuning()
{
    g_gui->m_wndMenu.ResetContent();

    for(int i = 0; i < TUNING_Count; i++)
        g_gui->m_wndMenu.AddItem(g_options->GetGen()->GetTuningLongText(i, m_hInst), IDMENU_Tuning+i, 0);

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetTuning(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_Bounce|OPTION_DrawScrollArrows|OPTION_DrawScrollBar,
        rcClient.left,rcClient.top,
        WIDTH(rcClient), HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        rcClient.left,0,0,0, ADJUST_Bottom);
}

void CGuiPlay::OnRoot()
{
    g_gui->m_wndMenu.ResetContent();

    for(int i = 0; i < ROOT_Count; i++)
        g_gui->m_wndMenu.AddItem(g_szRoots[i], IDMENU_Root+i, 0);

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetRoot(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector | OPTION_Bounce|OPTION_DrawScrollArrows|OPTION_DrawScrollBar,
        rcClient.left,rcClient.top,
        WIDTH(rcClient)*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        rcClient.left,0,0,0, ADJUST_Bottom);

}

void CGuiPlay::OnScale()
{
    g_gui->m_wndMenu.ResetContent();

    int i = 0;

    for(i = 0; i < SCALE_Count; i++)
        g_gui->m_wndMenu.AddItem(g_options->GetGen()->GetScalesText(i, m_hInst), IDMENU_Scale+i,0);

    g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Off), m_hInst), IDMENU_Scale+SCALE_Count,0);

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetScale(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector | OPTION_Bounce|OPTION_DrawScrollArrows|OPTION_DrawScrollBar,
        rcClient.left,rcClient.top,
        WIDTH(rcClient)*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        rcClient.left,0,0,0, ADJUST_Bottom);

}

void CGuiPlay::OnShift()
{
    if(m_iBottomFret == 0)
        m_iBottomFret = -1;
    else
        m_iBottomFret = 0;
    InvalidateRect(m_hWnd, &m_rcPlayArea, FALSE);
}


void CGuiPlay::OnFretUp()
{
    if(m_iBottomFret < 12 - GetMaxFrets())
    {
        m_iBottomFret++;
        g_gui->PlayClick();
        InvalidateRect(m_hWnd, &m_rcPlayArea, FALSE);
    }
}

void CGuiPlay::OnFretDown()
{
    if(m_iBottomFret > -1)
    {
        m_iBottomFret--;
        g_gui->PlayClick();
        InvalidateRect(m_hWnd, &m_rcPlayArea, FALSE);
    }
}

EnumScaleDraw CGuiPlay::GetScaleDrawType(int iString, int iFret, EnumChordRoot eRoot)
{
    //first find the note
    int iNote = g_options->GetGen()->GetNoteValue(iString, iFret);//the call can correct for any offsets not here
    byte btTemp;

    //we need to correct for the root now ... easiest is to adjust iNote 
    iNote -= (int)eRoot;
    if(iNote < 0)
        iNote += 12;

    //so the engine has the scale in the byte array
    for(int i = 0; i < SCALE_SIZE; i++)
    {
        btTemp = g_options->GetGen()->GetScaleValue(i);
        if(btTemp == iNote && i == 0)
            return SDRAW_Root;
        else if(btTemp == iNote)
            return SDRAW_Note;
        else if(btTemp == NOTE_NOT_DEF)
            return SDRAW_None;
        //continue        
    }
    return SDRAW_None;
}


void CGuiPlay::DrawArrow(HDC hdc, RECT& rcButton, COLORREF crColor, BOOL bDown)
{
    //so the width is approx double the height ... converges to as the size grows ....
    //fortunately our buttons are fat
    
    int iIndent = WIDTH(rcButton)/3; //so 1/3
    
    POINT pt[3];

    HPEN   hpNew;
    HBRUSH brNew;

    HPEN   hpOld;	
    HBRUSH brOld;

    pt[0].x = rcButton.left + iIndent;
    pt[1].x = rcButton.right - iIndent;

    if((pt[1].x - pt[0].x)%2 != 0)
        pt[1].x--;//need an odd width but the points are inclusive
    int iHeight = (pt[1].x - pt[0].x)/2;
    pt[2].x = pt[0].x + iHeight;

    if(bDown)
    {
        pt[0].y = rcButton.top + (HEIGHT(rcButton) - iHeight)/2;
        pt[1].y = pt[0].y;
        pt[2].y = pt[0].y + iHeight;
    }
    else
    {
        pt[2].y = rcButton.top + (HEIGHT(rcButton) - iHeight)/2;
        pt[0].y = pt[2].y + iHeight;
        pt[1].y = pt[0].y;
        
    }   
   

    hpNew	= CreatePen(PS_SOLID,1,crColor);
    brNew	= CreateSolidBrush(crColor);

    hpOld	= (HPEN)SelectObject(hdc, hpNew);
    brOld	= (HBRUSH)SelectObject(hdc, brNew);

    Polygon(hdc, pt, 3);

    SelectObject(hdc, hpOld);
    DeleteObject(hpNew);

    SelectObject(hdc, brOld);
    DeleteObject(brNew);
}

BOOL CGuiPlay::PlayScale(BOOL bAni /* = TRUE */)
{
    //find the middle starting point
    int iScaleLen = g_options->GetGen()->GetScaleLength();
    UINT uiRes;

    //loop through the scale
    for(int i = 0; i < iScaleLen; i++)
    {
        uiRes = IDR_WAV_AMid + g_options->GetGen()->GetRoot() + g_options->GetGen()->GetScaleValue(i);

        //drop an octave for bass guitars
        if(g_options->GetGen()->GetInstrument() == INST_4StringBass)
            //|| g_options->GetGen()->GetInstrument() == INST_5StringBass)
            uiRes -= 12;

        if(g_gui->GetPlaySounds())
        {
            g_gui->PlaySFX(uiRes - LOWEST_SOUND_RES);
            Sleep(400);//seems about right ... sleep sucks .. should use a timer
        }
    }

    //and the root one more time
    uiRes = IDR_WAV_AMid + g_options->GetGen()->GetRoot() + 12;

    if(g_gui->GetPlaySounds())
        g_gui->PlaySFX(uiRes - LOWEST_SOUND_RES);

    return TRUE;
}