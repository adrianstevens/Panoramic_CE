#include "GuiPlayFav.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

extern TCHAR g_szFrets[18][5];
extern TCHAR g_szRoots[12][8];

CGuiPlayFav::CGuiPlayFav(void)
:m_iBottomFret(0)
,m_iChordCount(0)
{
}

CGuiPlayFav::~CGuiPlayFav(void)
{
}

BOOL CGuiPlayFav::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL)
        return FALSE;

    g_gui->DrawBackground(rcClip);
    g_gui->DrawStrings(rcClip, 0);

    DrawText(*g_gui->GetGDI(), rcClip);
    DrawFingers(*g_gui->GetGDI(), rcClip);
//    DrawPosButtons(*g_gui->GetGDI(), rcClip);

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

BOOL CGuiPlayFav::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(g_gui->GetFinger()->GetDC() == NULL)
        return FALSE;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iTopIndent = BUTTONS_HEIGHT + NUT_HEIGHT;
    int iLeftIndent = 84*GetSystemMetrics(SM_CXSCREEN)/480;
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

    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
        iHeight = IsVGA()?62:31;//tweak this

    m_rcGrid.left   = iLeftIndent + rcClient.left;
    m_rcGrid.right  = rcClient.right - iLeftIndent;
    m_rcGrid.top    = rcClient.top + iTopIndent;
    m_rcGrid.bottom = m_rcGrid.top + 5*iHeight;

    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
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

        SetRect(&m_rcChordNames[y], rcClient.left + (IsVGA()?6:3), m_rcFingers[0][y].top + (IsVGA()?6:3), 
            rcClient.left + iLeftIndent, m_rcFingers[0][y].bottom);
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

BOOL CGuiPlayFav::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return TRUE;
}

BOOL CGuiPlayFav::OnLButtonUp(HWND hWnd, POINT& pt)
{
    //check the chord names as we'll strum the chord
    for(int i = 0; i < m_iChordCount; i++)
    {
        if(PtInRect(&m_rcChordNames[i], pt))
        {
            //play the chord



        }

    }

    //check the individual fingers for single taps



    return TRUE;
}

BOOL CGuiPlayFav::PlayChord(int iIndex)
{
    if(g_options && g_gui->GetPlaySounds() == FALSE)
        return FALSE;

    UINT uiTemp = 0;

    int iSleep = 130;
    if(g_options->GetStrumStyle() == STRUM_Fast)
        iSleep = 0;

    g_gui->StopSFX();

    for(int i = 0; i < g_options->GetGen()->GetNumStrings(); i++)
    {
     /*   uiTemp = GetNoteRes(i);
        if(uiTemp != 0)
        {
            g_gui->PlaySFX(uiTemp - LOWEST_SOUND_RES);

            iSleep = max(iSleep, 35);
            Sleep(iSleep);
            iSleep -= 15;
        }*/
    }    




    return TRUE;

}

BOOL CGuiPlayFav::OnMouseMove(HWND hWnd, POINT& pt)
{

    return TRUE;
}

BOOL CGuiPlayFav::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

BOOL CGuiPlayFav::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

BOOL CGuiPlayFav::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{


    return TRUE;
}

void CGuiPlayFav::OnLostFocus()
{

}

void CGuiPlayFav::OnSetFocus()
{
    m_iChordCount = 0;
    m_iChordCount = min(GetMaxFrets(), g_options->GetGen()->GetFavCount());



}

BOOL CGuiPlayFav::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    ::DrawText(gdi.GetDC(), g_options->GetGen()->GetTuningText((int)g_options->GetTuning(), m_hInst, g_options->GetInstrument()), m_rcButtons[3], DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, g_gui->GetFont(FONT_Button), 0xFFFFFF);

    if(g_options == NULL)
        return FALSE;

    TCHAR szTemp[STRING_LARGE];

    //Fret labels on right
    int iTemp = m_iBottomFret;

    static POINT ptFretOff = {(WIDTH(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetWidth())/2, (HEIGHT(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetHeight())/2};


    //chord names 
    int iCount = min(GetMaxFrets(), g_options->GetGen()->GetFavCount());

    for(int i = 0; i < iCount; i++)
    {
        m_oStr->StringCopy(szTemp, g_szRoots[g_options->GetGen()->GetFavRoot(i)]);
        m_oStr->Concatenate(szTemp, _T("\r\n"));
        m_oStr->Concatenate(szTemp, g_options->GetGen()->GetChordText(g_options->GetGen()->GetFavType(i), m_hInst));

        //Root then type
          ::DrawTextShadow(gdi.GetDC(), szTemp, 
            m_rcChordNames[i], DT_LEFT, 
            g_gui->GetFont(FONT_Menu), 0xFFFFFF);
    }
  
    return TRUE;
}

BOOL CGuiPlayFav::DrawFingers(CIssGDIEx& gdi, RECT& rcClip)
{
    DWORD dwTextColor = 0xaaaaaa;//0xaa8888;

    int iCount = m_iChordCount;
    int iTemp;

    for(int f  = 0; f < iCount; f++)
    {
        for(int s = 0; s < 6; s++)
        {   
            if(g_options->GetLeftHanded() == TRUE)
                iTemp = 5 - s;
            else 
                iTemp = s;

            int iTemp = g_options->GetGen()->GetFavNoteIndex(f, s);

            if(iTemp >= ROOT_Count)
            {
                //draw the no-play image             
                continue;
            }
            
            //lets draw some notes yo ... awesome ....
            int iX = m_rcFingers[s][f].left + (WIDTH(m_rcFingers[s][f]) - g_gui->GetFinger()->GetWidth())/2;
            int iY = m_rcFingers[s][f].top + (HEIGHT(m_rcFingers[s][f]) - g_gui->GetFinger()->GetHeight())/2;//its fine if its negative

            Draw(gdi, iX, iY, g_gui->GetFinger()->GetWidth(), g_gui->GetFinger()->GetHeight(), *g_gui->GetFinger(), 
                0, 0, ALPHA_Normal, 127);



            ::DrawText(gdi.GetDC(), g_szRoots[iTemp], m_rcFingers[s][f], 
                DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);

        //    ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, f + m_iBottomFret + 1), m_rcFingers[s][f], 
        //        DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);
        }
    }
    return TRUE;
}

BOOL CGuiPlayFav::DrawPosButtons(CIssGDIEx& gdi, RECT& rcClip)
{

    return TRUE;
}

UINT CGuiPlayFav::GetNoteRes(int iString, int iFret)
{


    return 0;
}

void CGuiPlayFav::DrawArrow(HDC hdc, RECT& rcButton, COLORREF crColor, BOOL bDown)
{

}
