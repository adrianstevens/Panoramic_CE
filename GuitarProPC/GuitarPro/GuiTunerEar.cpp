#include "GuiTunerEar.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

#define IDT_TIMER_HOLD      1
#define IDT_REPEAT_TIMER    2
#define IDT_PLAYALL_TIMER   3

#define IDT_FADE_TIMER1     11
#define IDT_FADE_TIMER2     12
#define IDT_FADE_TIMER3     13
#define IDT_FADE_TIMER4     14
#define IDT_FADE_TIMER5     15
#define IDT_FADE_TIMER6     16

#define DELAY_PER_NOTE		600 


CGuiTunerEar::CGuiTunerEar(void)
{
    for(int i = 0; i < 6; i++)
    {
        m_eButtonState[i] = BSTATE_Off;
    }

    m_iFadeTimers[0] = IDT_FADE_TIMER1;
    m_iFadeTimers[1] = IDT_FADE_TIMER2;
    m_iFadeTimers[2] = IDT_FADE_TIMER3;
    m_iFadeTimers[3] = IDT_FADE_TIMER4;
    m_iFadeTimers[4] = IDT_FADE_TIMER5;
    m_iFadeTimers[5] = IDT_FADE_TIMER6;
}

CGuiTunerEar::~CGuiTunerEar(void)
{
    OnLostFocus();
}

void CGuiTunerEar::OnLostFocus()
{
    KillTimer(m_hWnd, IDT_TIMER_HOLD);
    KillTimer(m_hWnd, IDT_REPEAT_TIMER);
    
    for(int i = 0; i < 6; i++)
    {
        KillTimer(m_hWnd, m_iFadeTimers[i]);
        m_eButtonState[i] = BSTATE_Off;
    }
}

BOOL CGuiTunerEar::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL)
        return FALSE;

    g_gui->DrawBackground(rcClip);
    g_gui->DrawNut(rcClip);

    int iFade = 0;
    for(int i = g_options->GetGen()->GetNumStrings(); i < 6; i++)
        iFade = iFade | 1<<i;
    g_gui->DrawStrings(rcClip, iFade);

    DrawFingers(*g_gui->GetGDI(), rcClip);
    DrawText(*g_gui->GetGDI(), rcClip);

    return TRUE;
}

BOOL CGuiTunerEar::DrawFingers(CIssGDIEx& gdi, RECT& rc)
{
    static UINT uiAlpha = 255;
    static DWORD dwTextColor;

    for(int i = 0; i < 6; i++)
    {
        if(IsRectInRect(rc, m_rcFingers[i]) == FALSE)
            continue;

        dwTextColor = 0x888888;//0xaa8888;

        int iTemp = i;
        if(g_options->GetLeftHanded())
            iTemp = 5 - i;

        if(iTemp >= g_options->GetGen()->GetNumStrings())
            continue;

        int iX = m_rcFingers[i].left + (WIDTH(m_rcFingers[i]) - g_gui->GetFinger()->GetWidth())/2;
        int iY = m_rcFingers[i].top + (HEIGHT(m_rcFingers[i]) - g_gui->GetFinger()->GetHeight())/2;//its fine if its negative

        switch(m_eButtonState[i])
        {
        case BSTATE_Repeat:
            Draw(gdi, iX, iY, g_gui->GetFingerRed()->GetWidth(), g_gui->GetFingerRed()->GetHeight(), *g_gui->GetFingerRed(), 
                0, 0, ALPHA_Normal);
            dwTextColor = 0xFFFFFF;
            break;
        case BSTATE_40:
            uiAlpha = 102;
            //dwTextColor = 0xaa8888
            break;
        case BSTATE_50:
            uiAlpha = 127;
            dwTextColor = RGB(155,155,155);//0xaa8888
            break;
        case BSTATE_60:
            uiAlpha = 153;
            dwTextColor = RGB(175,175,175);//0xaa8888
            break;
        case BSTATE_70:
            uiAlpha = 178;
            dwTextColor = RGB(195,195,195);//0xaa8888
            break;
        case BSTATE_80:
            uiAlpha = 204;
            dwTextColor = RGB(215,215,215);//0xaa8888
            break;
        case BSTATE_90:
            uiAlpha = 229;
            dwTextColor = RGB(235,235,235);//0xaa8888
            break;
        case BSTATE_On:
            uiAlpha = 255;
            dwTextColor = 0xFFFFFF;//0xaa8888
             break;
        case BSTATE_Off:
            uiAlpha = 76;
            break;
        }

        if(m_eButtonState[i] != BSTATE_Repeat)
        {
            Draw(gdi, iX, iY, g_gui->GetFinger()->GetWidth(), g_gui->GetFinger()->GetHeight(), *g_gui->GetFinger(), 
                0, 0, ALPHA_Normal, uiAlpha);

            
        }

        //and draw the note
        ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(iTemp, 0), m_rcFingers[i], 
            DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), dwTextColor);
    }

    return FALSE;
}


BOOL CGuiTunerEar::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    ::DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_Play), m_hInst), m_rcButtons[0], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
	::DrawText(gdi.GetDC(), g_options->GetGen()->GetTuningText((int)g_options->GetTuning(), m_hInst, g_options->GetInstrument()), m_rcButtons[3], DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    return TRUE;
}

BOOL CGuiTunerEar::OnLButtonUp(HWND hWnd, POINT& pt)
{
    KillTimer(m_hWnd, IDT_TIMER_HOLD);

    if(g_options && g_gui->GetPlaySounds())
	{
        for(int i = 0; i < 6; i++)
        {
            if(PtInRect(&m_rcFingers[i], pt))
            {
                if(m_eButtonState[i] == BSTATE_Repeat)
                    return UNHANDLED;

                SetTimer(m_hWnd, m_iFadeTimers[i], FADE_OUT_SPEED, NULL);
                
                //m_eButtonState[i] = BSTATE_Off;
                
                int iTemp = i;
                if(g_options->GetLeftHanded())
                    iTemp = 5 - i;

                if(iTemp >= g_options->GetGen()->GetNumStrings())
                    continue;

                //play sound code goes here
                g_gui->PlaySFX(GetNoteRes(iTemp, 0) - LOWEST_SOUND_RES);

                InvalidateRect(m_hWnd, &m_rcFingers[i], FALSE);
                UpdateWindow(m_hWnd);
                
                return TRUE;
            }
        }
	}

	if(g_gui->GetPlaySounds() && PtInRect(&m_rcButtons[0], pt))
	{
		OnPlay();
		return TRUE;
	}

    if(PtInRect(&m_rcButtons[3], pt))
    {
        OnTuning();
        return TRUE;
    }

    return TRUE;
}

BOOL CGuiTunerEar::OnLButtonDown(HWND hWnd, POINT& pt)
{
    KillTimer(m_hWnd, IDT_REPEAT_TIMER);
    InvalidateRect(m_hWnd, NULL, FALSE);

    for(int i = 0; i < 6; i++)
    {
        if(m_eButtonState[i] == BSTATE_Repeat) 
            m_eButtonState[i] = BSTATE_Off;
    }

    for(int i = 0; i < 6; i++)
    {
        if(PtInRect(&m_rcFingers[i], pt))
        {
            m_eButtonState[i] = BSTATE_On;
            m_iSelected = i;
            SetTimer(m_hWnd, IDT_TIMER_HOLD, 400, NULL);
            UpdateWindow(m_hWnd);
            return TRUE;
        }
    }


    return TRUE;
}

BOOL CGuiTunerEar::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDT_TIMER_HOLD:
        KillTimer(m_hWnd, IDT_TIMER_HOLD);

        if(g_gui->GetPlaySounds() == FALSE)
            return TRUE;

        if(m_iSelected < 0 || m_iSelected > 5)
            return FALSE;

        m_eButtonState[m_iSelected] = BSTATE_Repeat;
        InvalidateRect(m_hWnd, &m_rcFingers[m_iSelected], FALSE);
        //      m_iSelected = -1;
        UpdateWindow(m_hWnd);
        SetTimer(m_hWnd, IDT_REPEAT_TIMER, 1500, NULL);
    case IDT_REPEAT_TIMER:
        switch(m_iSelected)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            int iTemp = m_iSelected;
            if(g_options->GetLeftHanded())
                iTemp = 5 - m_iSelected;
            g_gui->PlaySFX(GetNoteRes(iTemp, 0) - LOWEST_SOUND_RES);
            break;      
        }
        break;
    case IDT_FADE_TIMER1:
    case IDT_FADE_TIMER2:
    case IDT_FADE_TIMER3:
    case IDT_FADE_TIMER4:
    case IDT_FADE_TIMER5:
    case IDT_FADE_TIMER6:
        {
            int iBtn = (int)(wParam - IDT_FADE_TIMER1);

            if(m_eButtonState[iBtn] < BSTATE_40)
            {
                m_eButtonState[iBtn] = BSTATE_Off;
                KillTimer(hWnd, wParam);
            }
            else
            {
                int iTemp = m_eButtonState[iBtn];
                m_eButtonState[iBtn] = (EnumButtonState)(iTemp - 1);
            }
            //and force an update
            HDC dc = GetDC(hWnd);
            
            if(m_gdiAni.GetDC() == NULL)
            {
                RECT rcClient;
                GetClientRect(hWnd, &rcClient);
                m_gdiAni.Create(dc, rcClient);
            }

            g_gui->DrawBackground(m_rcFingers[iBtn], &m_gdiAni);
            g_gui->DrawStrings(m_rcFingers[iBtn], 0, &m_gdiAni);
            DrawFingers(m_gdiAni, m_rcFingers[iBtn]);
            BitBlt(dc, m_rcFingers[iBtn].left, m_rcFingers[iBtn].top, 
                WIDTH(m_rcFingers[iBtn]), HEIGHT(m_rcFingers[iBtn]), 
                m_gdiAni.GetDC(), m_rcFingers[iBtn].left, m_rcFingers[iBtn].top, 
                SRCCOPY);
            
            ReleaseDC(hWnd, dc);
        }
        break;
	case IDT_PLAYALL_TIMER:
		{
			if(m_iPlaying >= g_options->GetGen()->GetNumberofStrings())
			{
				KillTimer(m_hWnd, IDT_PLAYALL_TIMER);
				return TRUE;
			}

			POINT pt;

            int iTemp = m_iPlaying;
            if(g_options->GetLeftHanded())
                iTemp = 5 - m_iPlaying;

			pt.x = m_rcFingers[iTemp].left + 1;
			pt.y = m_rcFingers[iTemp].top + 1;

			DWORD dwTemp = pt.x + (pt.y << 16);

			PostMessage(hWnd, WM_LBUTTONDOWN, 0, (LPARAM)dwTemp);
			PostMessage(hWnd, WM_LBUTTONUP, 0, (LPARAM)dwTemp);
			m_iPlaying++;
		}
    default:
        return FALSE;
        break;
    }

    return TRUE;
}


UINT CGuiTunerEar::GetNoteRes(int iString, int iFret)
{
    return g_options->GetGen()->GetNoteRes(iString, iFret);
}

BOOL CGuiTunerEar::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient, rcGrid;
    GetClientRect(hWnd, &rcClient);

    int iTopIndent = BUTTONS_HEIGHT + NUT_HEIGHT;
    int iLeftIndent = 84*GetSysMets(SM_CXSCREEN)/480;

    //and the grid ... just an outline for now
    int iWidth = IsVGA()?62:31;
    int iHeight = IsVGA()?94:47;

    if(GetSysMets(SM_CYSCREEN) == 480 && GetSysMets(SM_CXSCREEN) == 320)
        iHeight = 58;

    rcGrid.left   = iLeftIndent + rcClient.left;
    rcGrid.right  = rcClient.right - iLeftIndent;
    rcGrid.top    = rcClient.top + iTopIndent;
    rcGrid.bottom = rcGrid.top + 5*iHeight;

    iWidth = WIDTH(rcGrid)/5;

    //and now the important
    for(int x = 0; x < 6; x++)
    {
        m_rcFingers[x].left = rcGrid.left + WIDTH(rcGrid)*x/5 - iWidth/2;
        m_rcFingers[x].right = m_rcFingers[x].left + WIDTH(rcGrid)/5;
        m_rcFingers[x].top = rcGrid.top + HEIGHT(rcGrid)*2/5;
        m_rcFingers[x].bottom = m_rcFingers[x].top + HEIGHT(rcGrid)/5;
    }

    iWidth = WIDTH(rcClient)/4;

    //top buttons
    SetRect(&m_rcButtons[0], rcClient.left, rcClient.top, rcClient.left + iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[1], rcClient.left + iWidth, rcClient.top, rcClient.left + 2*iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[2], rcClient.left + 2*iWidth, rcClient.top, rcClient.left + 3*iWidth, rcClient.top + BUTTONS_HEIGHT);
    SetRect(&m_rcButtons[3], rcClient.left + 3*iWidth, rcClient.top, rcClient.right, rcClient.top + BUTTONS_HEIGHT);

    return TRUE;
}

BOOL CGuiTunerEar::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam >= IDMENU_Tuning && wParam < IDMENU_Tuning + TUNING_Count)
    {
        //g_options->SetTuning(EnumTuning(wParam - IDMENU_Tuning));
		g_options->SetTuning((int)(wParam - IDMENU_Tuning));
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
    return UNHANDLED;
}

void CGuiTunerEar::OnTuning()
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

void CGuiTunerEar::OnPlay()
{
	m_iPlaying = 0;
	SetTimer(m_hWnd, IDT_PLAYALL_TIMER, DELAY_PER_NOTE, NULL);
}