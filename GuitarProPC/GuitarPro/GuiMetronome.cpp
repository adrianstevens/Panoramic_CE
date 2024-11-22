#include "GuiMetronome.h"
#include "IssGDIFX.h"
#include "general_midi.h"
#include "Metronome\BeatBox_WAV.h"
#include "IssLocalisation.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

#define IDT_METRO_TIMER     10
#define IDT_UPDATE_BPM      11
#define IDT_UPDATE_METER    12
#define IDT_LED_FLASH       13

#define IDT_TAPnHOLD        14
#define IDT_STOPWATCH       15

#define TAPnHOLD_TIME       100
#define UPDATE_BPM_TIME     250 //just a short delay

//large Text
#define BPM_TEXT_SIZE       (IsVGA()?110:55)
#define METER_TEXT_SIZE     (IsVGA()?70:35)

#define GREY_TEXT   0x999999
#define LIGHT_TEXT  0xffffff


CGuiMetronome::CGuiMetronome(void)
{
    m_bPlaying = FALSE;
    m_MetronomeStyle = metPlain;
    m_BPMeasure = 0;
    m_BPMinute = 0;
    m_iStopWatch = 0;
    SetStopwatchString();

    m_BeatData.resize(MAX_BPMEASURE);
    m_midi_instrument.resize(MAX_SOUNDS, 0);
    m_midi_volume.resize(MAX_SOUNDS, 0);
    m_blink_size.resize(MAX_SOUNDS, 0);
    
    m_oStr->Empty(m_szBPM);
    m_oStr->Empty(m_szMeter);

    //test code
    m_BPMeasure = 1;

    m_hFontBPM = CIssGDIEx::CreateFont(BPM_TEXT_SIZE, FW_NORMAL, TRUE);
    m_hFontMeter = CIssGDIEx::CreateFont(METER_TEXT_SIZE, FW_NORMAL, TRUE);

    LoadSettings();

    m_eBtnDown = METBTN_Count;//we'll use this as "none"
}

CGuiMetronome::~CGuiMetronome(void)
{
    if(m_bPlaying)
    {
        Stop();
    }

    CIssGDIEx::DeleteFont(m_hFontBPM);
    CIssGDIEx::DeleteFont(m_hFontMeter);
}

void CGuiMetronome::Init(HWND hWnd, HINSTANCE hInst)
{
    CGuiBase::Init(hWnd, hInst);
    
    //and get the settings
    m_BPMinute = g_options->GetMetroBPM();
    m_BPMeasure = g_options->GetMetroMeter();

    SetStrings();
}

BOOL CGuiMetronome::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL)
        return FALSE;

    if(m_gdiBackground.GetDC() == NULL)
        InitBackground();

    //background
    BitBlt(g_gui->GetGDI()->GetDC(), rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), m_gdiBackground.GetDC(), rcClip.left, rcClip.top, SRCCOPY);

    DrawLEDs(*g_gui->GetGDI(), rcClip);
    DrawDisplay(*g_gui->GetGDI(), rcClip);
    DrawBtns(*g_gui->GetGDI(), rcClip);
    g_gui->DrawMenu(rcClip, g_gui->GetGDI(), FALSE);


    return TRUE;
}

void CGuiMetronome::InitBackground()
{
    if(g_gui == NULL || m_hWnd == NULL)
        return;

    //and set the buttons
    LoadImages();
    m_imgBtn.SetSize(WIDTH(m_rcBtns[METBTN_BPM_Up]), HEIGHT(m_rcBtns[METBTN_BPM_Up]));

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    HDC dc = GetDC(m_hWnd);
    m_gdiBackground.Create(dc, rcClient);
    ReleaseDC(m_hWnd, dc);

    //gradient background
    GradientFillRect(m_gdiBackground, rcClient, 0x525252, 0, FALSE);

    //draw the display
    CIssGDIEx gdiDis;
    gdiDis.Create(m_gdiBackground.GetDC(), m_rcDisplay);
    RECT rcTemp;

    int iW = WIDTH(m_rcDisplay);
    int iH = HEIGHT(m_rcDisplay);

    SetRect(&rcTemp, 0, 0, iW, iH);

    GradientFillRect(gdiDis, rcTemp, 0x323232, 0, FALSE);

    gdiDis.SetAlphaMask(g_gui->GetAlpha(iW, iH)->GetImage());
    g_gui->GetImgOutline(iW, iH)->DrawImage(gdiDis, 0, 0, ALPHA_AddValue);
    Draw(m_gdiBackground, m_rcDisplay, gdiDis);
    
    //draw the line for the meter
    int iY = m_rcLed1.top + HEIGHT(m_rcLed1)/2;
    int iX1 = m_rcLed1.right - WIDTH(m_rcLed1)/4;
    int iX2 = m_rcLed2.left + WIDTH(m_rcLed2)/4;

    ::Line(m_gdiBackground.GetDC(), iX1, iY, iX2, iY, 0x323232, IsVGA()?2:1);

    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_Meter), m_hInst), m_rcMeterLabel, DT_LEFT, g_gui->GetFont(FONT_MetroLabels), GREY_TEXT);

    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_Meter), m_hInst), m_rcMeterBtnLabel, DT_CENTER, g_gui->GetFont(FONT_MetroLabels), GREY_TEXT);
    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_Tempo), m_hInst), m_rcTempoBtnLabel, DT_CENTER, g_gui->GetFont(FONT_MetroLabels), GREY_TEXT);

    m_imgBtn.DrawImage(m_gdiBackground, m_rcBtns[METBTN_BPM_Up].left, m_rcBtns[METBTN_BPM_Up].top);
    m_imgBtn.DrawImage(m_gdiBackground, m_rcBtns[METBTN_BPM_Down].left, m_rcBtns[METBTN_BPM_Down].top);
    m_imgBtn.DrawImage(m_gdiBackground, m_rcBtns[METBTN_Meter_Up].left, m_rcBtns[METBTN_Meter_Up].top);
    m_imgBtn.DrawImage(m_gdiBackground, m_rcBtns[METBTN_Meter_Down].left, m_rcBtns[METBTN_Meter_Down].top);
    m_imgBtn.DrawImage(m_gdiBackground, m_rcBtns[METBTN_StartStop].left, m_rcBtns[METBTN_StartStop].top);

    //lets try drawing the start/stop images on the button
    POINT ptTemp;
    ptTemp.y = m_rcBtns[METBTN_StartStop].bottom - 3*m_gdiStartStop.GetHeight()/2;
    ptTemp.x = m_rcBtns[METBTN_StartStop].left + (WIDTH(m_rcBtns[METBTN_StartStop]) - m_gdiStartStop.GetWidth())/2;

    Draw(m_gdiBackground, ptTemp.x, ptTemp.y, m_gdiStartStop.GetWidth(), m_gdiStartStop.GetHeight(), m_gdiStartStop, 0, 0, ALPHA_Normal, 70);
}

BOOL CGuiMetronome::DrawDisplay(CIssGDIEx& gdi, RECT rcClip)
{
    if(IsRectInRect(rcClip, m_rcDisplay) == FALSE)
        return FALSE;

    ::DrawText(gdi, m_szMeter, m_rcMeter, DT_CENTER, g_gui->GetFont(FONT_MetroLabels), LIGHT_TEXT);
    ::DrawText(gdi, m_szBPM, m_rcBPM, DT_CENTER | DT_VCENTER, m_hFontBPM, LIGHT_TEXT);
    if(m_bPlaying) 
        ::DrawText(gdi, m_szMeter, m_rcCurrentMeter, DT_CENTER | DT_VCENTER, m_hFontMeter, LIGHT_TEXT);
    else
        ::DrawText(gdi, m_oStr->GetText(ID(IDS_MSG_SetBPM), m_hInst), m_rcTap, DT_CENTER, g_gui->GetFont(FONT_MetroLabels), LIGHT_TEXT);

    ::DrawText(gdi, m_szTempoMarking, m_rcTempoLabel, DT_RIGHT, g_gui->GetFont(FONT_MetroLabels), GREY_TEXT);

    //start stop
    ::Draw(gdi, m_rcPlayStopInd, m_gdiStartStop, (m_bPlaying?0:m_gdiStartStop.GetWidth()/2));
    return TRUE;
}

void CGuiMetronome::FlashLED(int iLed)
{
    HDC dc = GetDC(m_hWnd);

    RECT rcTemp;
    if(iLed < 0 || iLed > 3)
        return;
    if(iLed < 2)
        rcTemp = m_rcLed1;
    else
        rcTemp = m_rcLed2;

    BitBlt(g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        m_gdiBackground.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ::Draw(*g_gui->GetGDI(), rcTemp, *g_gui->GetLEDs(), WIDTH(rcTemp)*iLed);

    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
            
    ReleaseDC(m_hWnd, dc);
}

//green indicators and the beat number
void CGuiMetronome::UpdateMeterInd()
{
    if(m_BPMeasure < 3)
        return;

    HDC dc = GetDC(m_hWnd);

    RECT rcTemp;

    if(m_CurrentBeat == 0)
    {   //clear it
        rcTemp = m_rcLed1;
        rcTemp.right = m_rcLed2.left;
        rcTemp.left = m_rcLed1.right;

        BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiBackground.GetDC(),
            rcTemp.left, rcTemp.top, SRCCOPY);
    }

    BitBlt(dc, m_rcCurrentMeter.left, m_rcCurrentMeter.top, WIDTH(m_rcCurrentMeter), HEIGHT(m_rcCurrentMeter), m_gdiBackground.GetDC()
        , m_rcCurrentMeter.left, m_rcCurrentMeter.top, SRCCOPY);
    {
        m_oStr->IntToString(m_szCurrentBeat, m_CurrentBeat+1);
        DrawText(dc, m_szCurrentBeat, m_rcCurrentMeter, DT_CENTER | DT_VCENTER, m_hFontMeter, 0xFFFFFF);
    }

    int iW = (m_rcLed2.left - m_rcLed1.right)/(m_BPMeasure + 1);

    //now draw the current indicator
    rcTemp.top = m_rcLed1.top + (HEIGHT(m_rcLed1) - m_gdiMeter.GetHeight())/2;
    rcTemp.bottom = rcTemp.top + m_gdiMeter.GetHeight();
    rcTemp.left = m_rcLed1.right + iW*(m_CurrentBeat+1);
    rcTemp.right = rcTemp.left + m_gdiMeter.GetWidth();

    BitBlt(g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        m_gdiBackground.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);

    ::Draw(*g_gui->GetGDI(), rcTemp, m_gdiMeter);

    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);

    ReleaseDC(m_hWnd, dc);
}

BOOL CGuiMetronome::DrawLEDs(CIssGDIEx& gdi, RECT rcClip)
{
    if(g_gui == NULL)
        return FALSE;

    int iW = g_gui->GetLEDs()->GetWidth()/4;

    //green
    if(IsRectInRect(rcClip, m_rcLed1))
        Draw(gdi, m_rcLed1, *g_gui->GetLEDs(), iW);

    //red
    if(IsRectInRect(rcClip, m_rcLed2))
        Draw(gdi, m_rcLed2, *g_gui->GetLEDs(), iW*3);

    //draw the thin lines depending on the meter
/*    if(m_BPMeasure > 2)
    {
        POINT ptTemp;
        ptTemp.y = m_rcLed1.top + (HEIGHT(m_rcLed1) - m_gdiMeter.GetHeight())/2;

        for(int i = 1; i < (int)m_CurrentBeat; i++)
        {
            ptTemp.x = m_rcLed1.right + i*(m_rcLed2.left - m_rcLed1.right)/(m_BPMeasure-1);
            ::Draw(gdi, ptTemp.x, ptTemp.y, m_gdiMeter.GetWidth(), m_gdiMeter.GetHeight(), m_gdiMeter);
        }
    }*/
   
    return TRUE;
}


//up off, down off, up on, down on
BOOL CGuiMetronome::DrawBtns(CIssGDIEx& gdi, RECT rcClip)
{
    if(m_gdiBtnIcons.GetDC() == NULL)
        m_gdiBtnIcons.LoadImage(IsVGA()?IDB_PNG_BtnArrows_HR:IDB_PNG_BtnArrows, m_hWnd, m_hInst);

    //just draw the down state for now
    static POINT pt = {(m_imgBtn.GetWidth() - m_gdiBtnIcons.GetWidth()/4)/2, (m_imgBtn.GetHeight() - m_gdiBtnIcons.GetHeight())/2};
    static int iW = m_gdiBtnIcons.GetWidth()/4;
    static int iH = m_gdiBtnIcons.GetHeight();

    if(IsRectInRect(m_rcBtns[METBTN_BPM_Up], rcClip))
    {
        if(m_eBtnDown == METBTN_BPM_Up)
            Draw(gdi, m_rcBtns[METBTN_BPM_Up].left + pt.x, m_rcBtns[METBTN_BPM_Up].top + pt.y, iW, iH, m_gdiBtnIcons, iW*2);
        else
            Draw(gdi, m_rcBtns[METBTN_BPM_Up].left + pt.x, m_rcBtns[METBTN_BPM_Up].top + pt.y, iW, iH, m_gdiBtnIcons, 0);
    }
    if(IsRectInRect(m_rcBtns[METBTN_BPM_Down], rcClip))
    {
        if(m_eBtnDown == METBTN_BPM_Down)
            Draw(gdi, m_rcBtns[METBTN_BPM_Down].left + pt.x, m_rcBtns[METBTN_BPM_Down].top + pt.y, iW, iH, m_gdiBtnIcons, iW*3);
        else
            Draw(gdi, m_rcBtns[METBTN_BPM_Down].left + pt.x, m_rcBtns[METBTN_BPM_Down].top + pt.y, iW, iH, m_gdiBtnIcons, iW);
    }
    if(IsRectInRect(m_rcBtns[METBTN_Meter_Up], rcClip))
    {
        if(m_eBtnDown == METBTN_Meter_Up)
            Draw(gdi, m_rcBtns[METBTN_Meter_Up].left + pt.x, m_rcBtns[METBTN_Meter_Up].top + pt.y, iW, iH, m_gdiBtnIcons, iW*2);
        else
            Draw(gdi, m_rcBtns[METBTN_Meter_Up].left + pt.x, m_rcBtns[METBTN_Meter_Up].top + pt.y, iW, iH, m_gdiBtnIcons, 0);
    }

    if(IsRectInRect(m_rcBtns[METBTN_Meter_Down], rcClip))
    {
        if(m_eBtnDown == METBTN_Meter_Down)
            Draw(gdi, m_rcBtns[METBTN_Meter_Down].left + pt.x, m_rcBtns[METBTN_Meter_Down].top + pt.y, iW, iH, m_gdiBtnIcons, iW*3);
        else
            Draw(gdi, m_rcBtns[METBTN_Meter_Down].left + pt.x, m_rcBtns[METBTN_Meter_Down].top + pt.y, iW, iH, m_gdiBtnIcons, iW);
    }
    if(IsRectInRect(m_rcBtns[METBTN_StartStop], rcClip))
    {
        DWORD dwTextClr = GREY_TEXT;
        if(m_eBtnDown == METBTN_StartStop)
            dwTextClr = RGB(204,204,0);

        if(m_bPlaying)
            ::DrawText(gdi, m_oStr->GetText(ID(IDS_MENU_Stop), m_hInst), m_rcBtns[METBTN_StartStop], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), dwTextClr);
        else
            ::DrawText(gdi, m_oStr->GetText(ID(IDS_MENU_Start), m_hInst), m_rcBtns[METBTN_StartStop], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), dwTextClr);
    }

    if(IsRectInRect(m_rcStopWatch, rcClip))
    {
        ::DrawText(gdi, m_szTimer, m_rcStopWatch, DT_CENTER, g_gui->GetFont(FONT_MetroLabels), GREY_TEXT);
    }
    
    return TRUE;
}

BOOL CGuiMetronome::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == UWM_BeatBox_BEAT_OCCURRED_wpBlinkSize_lpNULL)
    {
        m_CurrentBeat = lParam;
        if(m_CurrentBeat >= m_BPMeasure)
            m_CurrentBeat = 0;

        KillTimer(m_hWnd, IDT_LED_FLASH);
        //bugbug ... gotta work on this
        SetTimer(m_hWnd, IDT_LED_FLASH, m_MetronomeBeatsEveryThisMany_ms/3, NULL);
        m_ApproxElapsedTime_ms = 0;

        if(m_CurrentBeat == m_BPMeasure - 1)
            FlashLED(2);
        FlashLED(0);//go green
        UpdateMeterInd();

        return TRUE;
    }

    return UNHANDLED;
}

BOOL CGuiMetronome::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDT_TAPnHOLD:
        {
            switch(m_eBtnDown)
            {
            case METBTN_BPM_Up:
                if(m_BPMinute < MAX_BPM)
                    m_BPMinute++;
                InvalidateRect(hWnd, &m_rcBPM, FALSE);
                InvalidateRect(hWnd, &m_rcTempoLabel, FALSE);
            	break;
            case METBTN_BPM_Down:
                if(m_BPMinute > MIN_BPM)
                    m_BPMinute--;
                InvalidateRect(hWnd, &m_rcBPM, FALSE);
                InvalidateRect(hWnd, &m_rcTempoLabel, FALSE);                
                break;
            case METBTN_Meter_Up:
                if(m_BPMeasure < MAX_METER)
                    m_BPMeasure++;
                else
                    m_BPMeasure = 1;
                InvalidateRect(hWnd, &m_rcMeter, FALSE);
                break;
            case METBTN_Meter_Down:
                if(m_BPMeasure < 2)
                    m_BPMeasure = MAX_METER;
                else
                    m_BPMeasure--;
                InvalidateRect(m_hWnd, &m_rcMeter, FALSE);
                break;
            default:
                KillTimer(hWnd, IDT_TAPnHOLD);
                return TRUE;
            }
            SetStrings();
            UpdateWindow(hWnd);
        }
        break;
    case IDT_METRO_TIMER:
        
    	break;
    case IDT_UPDATE_BPM:
        KillTimer(hWnd, IDT_UPDATE_BPM);
        UpdateBPM();
    	break;
    case IDT_UPDATE_METER:
        {
        KillTimer(hWnd, IDT_UPDATE_METER); 
         // initialize things so that it's Playing a measure with a strong first beat
        BOOL bPlaying = m_bPlaying;
        Stop();

        m_BeatData[0].set_single_value(1);

        if(m_BPMeasure > 1)
        {
            for(int i = 1; i < (int)m_BPMeasure; i++)
                m_BeatData[i].set_single_value(0);
        }
        else
        {   //complete and total hack for single measure
            for(int i = 1; i < (int)(m_BPMeasure+1); i++)
                m_BeatData[i].set_single_value(1);
        }
        

        if (bPlaying)
        {
            if(m_BPMeasure == 1)
            {
                m_BPMeasure = 2;
                Play();
                m_BPMeasure = 1;
            }
            else
                Play();
        }

        //and update 
        if(m_BPMeasure > 1)
            g_options->SetMetroMeter(m_BPMeasure);
        else //gross hack
            g_options->SetMetroMeter(m_BPMeasure+1);
        }
        break;
    case IDT_LED_FLASH:
        KillTimer(m_hWnd, IDT_LED_FLASH);
        if(m_CurrentBeat == m_BPMeasure - 1)
            FlashLED(3);
        FlashLED(1); //green off
        break;
    case IDT_STOPWATCH:
        m_iStopWatch++;
        InvalidateRect(m_hWnd, &m_rcStopWatch, FALSE);
        SetStopwatchString();
        break;
    default:
        return UNHANDLED;
        break;
    }
    return TRUE;
   
}

BOOL CGuiMetronome::OnLButtonDown(HWND hWnd, POINT& pt)
{
    KillTimer(hWnd, IDT_TAPnHOLD);
    
    //more important so we'll do this first
    if(m_bPlaying == FALSE && PtInRect(&m_rcTap, pt))
    {
        Tap();
        return TRUE;
    }
    
    SetTimer(hWnd, IDT_TAPnHOLD, TAPnHOLD_TIME, NULL);
    
    if(PtInRect(&m_rcBtns[METBTN_Meter_Up], pt))
    {
        m_eBtnDown = METBTN_Meter_Up;
    }
    else if(PtInRect(&m_rcBtns[METBTN_Meter_Down], pt))
    {
        m_eBtnDown = METBTN_Meter_Down;
    }
    else if(PtInRect(&m_rcBtns[METBTN_BPM_Up], pt))
    {
        m_eBtnDown = METBTN_BPM_Up;
    }
    else if(PtInRect(&m_rcBtns[METBTN_BPM_Down], pt))
    {
        m_eBtnDown = METBTN_BPM_Down;
    }
    else if(PtInRect(&m_rcBtns[METBTN_StartStop], pt))
    {
        m_eBtnDown = METBTN_StartStop;
    }
    else
    {
        if(m_eBtnDown != METBTN_Count)
        {
            InvalidateRect(m_hWnd, &m_rcBtns[m_eBtnDown], FALSE);
            UpdateWindow(m_hWnd);
            m_eBtnDown = METBTN_Count;
        }
        KillTimer(hWnd, IDT_TAPnHOLD);
        m_eBtnDown = METBTN_Count;
        return UNHANDLED;
    }
        
    InvalidateRect(m_hWnd, &m_rcBtns[m_eBtnDown], FALSE);
    UpdateWindow(m_hWnd);//show the button state
    return TRUE;
}

void CGuiMetronome::TempoUp()
{
    if(m_eBtnDown == METBTN_BPM_Up)
    {
        if(m_BPMinute < MAX_BPM)
            m_BPMinute++;
    }

    KillTimer(m_hWnd, IDT_UPDATE_BPM);

    if(m_bPlaying)
        SetTimer(m_hWnd, IDT_UPDATE_BPM, UPDATE_BPM_TIME, NULL);

    InvalidateRect(m_hWnd, &m_rcBPM, FALSE);
}

void CGuiMetronome::TempoDown()
{
    if(m_eBtnDown == METBTN_BPM_Down)
    {
        if(m_BPMinute > MIN_BPM)
            m_BPMinute--;

        KillTimer(m_hWnd, IDT_UPDATE_BPM);

        if(m_bPlaying)
            SetTimer(m_hWnd, IDT_UPDATE_BPM, UPDATE_BPM_TIME, NULL);

        InvalidateRect(m_hWnd, &m_rcBPM, FALSE);
    }
}

void CGuiMetronome::StartStop()
{
    if(m_eBtnDown == METBTN_StartStop)
    {
        if(!m_bPlaying)
            Play();
        else
            Stop();

        InvalidateRect(m_hWnd, &m_rcPlayStopInd, FALSE);
    }
}

void CGuiMetronome::Tap()
{
    if(m_bPlaying)
        return;

    //a few static variables
    static int iNextTick = 0;//just makes it easier
    static int iCurrentTick = 0;
    static BOOL bSetBPM = FALSE;
    static DWORD dwLastTick = 0;

    //if its been too long reset
    
    iCurrentTick = iNextTick;
    m_dwTaps[iCurrentTick] = GetTickCount();

    iNextTick++;
    if(iNextTick >= NUM_TAPS)
    {
        bSetBPM = TRUE;
        iNextTick = 0;
    }

    if(bSetBPM && (m_dwTaps[iCurrentTick] - m_dwTaps[iNextTick] > 10000))
    {   //reset everything
        m_dwTaps[0] = m_dwTaps[iCurrentTick];
        iCurrentTick = 0;
        iNextTick = 0;
        bSetBPM = FALSE;
    }

    if(bSetBPM)
    {   //calc the bpm
        m_BPMinute = (NUM_TAPS - 1)*60000/(m_dwTaps[iCurrentTick] - m_dwTaps[iNextTick]);
        UpdateBPM();
        SetStrings();
        InvalidateRect(m_hWnd, &m_rcBPM, FALSE);
    }

    FlashLED(0);
}


BOOL CGuiMetronome::OnLButtonUp(HWND hWnd, POINT& pt)
{
    KillTimer(hWnd, IDT_TAPnHOLD);

    if(m_bPlaying == FALSE && PtInRect(&m_rcTap, pt))
    {
        FlashLED(1);
        return TRUE;
    }

    if(PtInRect(&m_rcBtns[METBTN_Meter_Up], pt))
    {
        if(m_eBtnDown == METBTN_Meter_Up)
        {
            if(m_BPMeasure < MAX_METER)
                m_BPMeasure++;
            else
                m_BPMeasure = 1;

            KillTimer(m_hWnd, IDT_UPDATE_METER);
            SetTimer(m_hWnd, IDT_UPDATE_METER, UPDATE_BPM_TIME, NULL);
            InvalidateRect(m_hWnd, &m_rcMeter, FALSE);
        }
    }
    else if(PtInRect(&m_rcBtns[METBTN_Meter_Down], pt))
    {
        if(m_eBtnDown == METBTN_Meter_Down)
        {
            if(m_BPMeasure < 2)
                m_BPMeasure = MAX_METER;
            else
                m_BPMeasure--;

            KillTimer(m_hWnd, IDT_UPDATE_METER);
            SetTimer(m_hWnd, IDT_UPDATE_METER, UPDATE_BPM_TIME, NULL);
            InvalidateRect(m_hWnd, &m_rcMeter, FALSE);
        }
    }
    else if(PtInRect(&m_rcBtns[METBTN_BPM_Up], pt))
    {
        TempoUp();

    }
    else if(PtInRect(&m_rcBtns[METBTN_BPM_Down], pt))
    {
        TempoDown();
    }
    else if(PtInRect(&m_rcBtns[METBTN_StartStop], pt))
    {
        StartStop();
    }
    else 
    {
        if(m_eBtnDown != METBTN_Count)
        {
            InvalidateRect(m_hWnd, &m_rcBtns[m_eBtnDown], FALSE);
            UpdateWindow(m_hWnd);
            m_eBtnDown = METBTN_Count;
        }
        return UNHANDLED;
    }

    SetStrings();
//    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    InvalidateRect(m_hWnd, &m_rcBtns[m_eBtnDown], FALSE);
    m_eBtnDown = METBTN_Count;
    UpdateWindow(m_hWnd);
     
    return TRUE;
}

void CGuiMetronome::UpdateBPM()
{
    BOOL bPlaying = m_bPlaying;

    if(m_bPlaying)
    {
    //    if (m_autopTicker.get())
    //        m_autopTicker->SetTempo(m_BPMinute);
        Stop();
    }

    if(m_BPMinute < MIN_BPM)
        m_BPMinute = MIN_BPM;
    else if(m_BPMinute > MAX_BPM)
        m_BPMinute = MAX_BPM;

    m_CurrentBeat = 0;

    double const BPS = (((double)m_BPMinute)/60.0);
    double const BeatEveryThisMany_ms = 1000/BPS;
    m_MetronomeBeatsEveryThisMany_ms = (const unsigned long)BeatEveryThisMany_ms;

    g_options->SetMetroBPM(m_BPMinute);//update the options/settings class

    if(bPlaying)
        Play();
}

BOOL CGuiMetronome::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    BOOL bIsSquare = FALSE;
    if(GetSysMets(SM_CXSCREEN) == GetSysMets(SM_CYSCREEN))
        bIsSquare = TRUE;

    //int iIndent = (IsVGA()?12:6);
    int iIndent = GetSysMets(SM_CXICON)*6/32;
    int iDisplayHeight = max((HEIGHT(rcClient) - MENU_HEIGHT)/2, IsVGA()?240:120);
    //lets make it a little taller since we've got the space
    if(bIsSquare == FALSE || GetSysMets(SM_CYSCREEN) == 320)
        iDisplayHeight += iDisplayHeight/4;
    int iLabelWidth = GetSysMets(SM_CXICON)*3/2;//IsVGA()?80:40;
    int iLabelHeight = IsVGA()?40:20;
    int iStartStopH = IsVGA()?16:8;
    int iLEDH = IsVGA()?60:30;
    int iBtnSize = GetSysMets(SM_CXICON)*40/32;//this we'll scale properly

    int iTemp;

    SetRect(&m_rcDisplay, rcClient.left + iIndent, rcClient.top + iIndent, rcClient.right - iIndent, iDisplayHeight + rcClient.top);
    SetRect(&m_rcMeterLabel, m_rcDisplay.left + iIndent*2, m_rcDisplay.top + iIndent*2, m_rcDisplay.left + iIndent*2 + iLabelWidth, m_rcDisplay.top + iIndent*2 + iLabelHeight);
    SetRect(&m_rcMeter, m_rcMeterLabel.right, m_rcDisplay.top + iIndent*2, m_rcMeterLabel.right + iLabelWidth, m_rcDisplay.top + iIndent*2 + iLabelHeight);
    SetRect(&m_rcTempoLabel, m_rcDisplay.right - 2*iIndent - 2*iLabelWidth, m_rcDisplay.top + iIndent*2, m_rcDisplay.right - 2*iIndent, m_rcDisplay.top + iIndent*2 + iLabelHeight);
    iTemp = m_rcDisplay.top + (HEIGHT(m_rcDisplay) - iStartStopH)/2;
    SetRect(&m_rcPlayStopInd, m_rcDisplay.left+iIndent*3, iTemp, m_rcDisplay.left+3*iIndent+iStartStopH, iTemp + iStartStopH);
    iTemp = m_rcDisplay.top + (HEIGHT(m_rcDisplay) - METER_TEXT_SIZE)/2;
    SetRect(&m_rcCurrentMeter, m_rcMeter.left, iTemp, m_rcMeter.left + METER_TEXT_SIZE, iTemp + METER_TEXT_SIZE);
    iTemp = m_rcDisplay.top + (HEIGHT(m_rcDisplay) - BPM_TEXT_SIZE)/2;
    SetRect(&m_rcBPM, m_rcDisplay.right - 2*iIndent - 2*BPM_TEXT_SIZE, iTemp, m_rcDisplay.right - 2*iIndent, iTemp + BPM_TEXT_SIZE);
    
    SetRect(&m_rcLed1, m_rcDisplay.left+iIndent, m_rcDisplay.bottom - iLEDH - iIndent, m_rcDisplay.left + iLEDH+iIndent, m_rcDisplay.bottom - iIndent);
    SetRect(&m_rcLed2, m_rcDisplay.right - iLEDH - iIndent, m_rcDisplay.bottom - iLEDH - iIndent, m_rcDisplay.right - iIndent, m_rcDisplay.bottom - iIndent);

    iTemp = (rcClient.bottom - MENU_HEIGHT - m_rcDisplay.bottom - iBtnSize - iLabelHeight)/2 + m_rcDisplay.bottom;
    SetRect(&m_rcBtns[METBTN_BPM_Up], rcClient.left + iIndent, iTemp, rcClient.left + iIndent + iBtnSize, iTemp + iBtnSize);
    SetRect(&m_rcBtns[METBTN_BPM_Down], m_rcBtns[METBTN_BPM_Up].right + iIndent, m_rcBtns[METBTN_BPM_Up].top, m_rcBtns[METBTN_BPM_Up].right + iIndent + iBtnSize, m_rcBtns[METBTN_BPM_Up].bottom);
    SetRect(&m_rcBtns[METBTN_Meter_Up], m_rcBtns[METBTN_BPM_Down].right + iIndent, m_rcBtns[METBTN_BPM_Down].top, m_rcBtns[METBTN_BPM_Down].right + iIndent + iBtnSize, m_rcBtns[METBTN_BPM_Down].bottom);
    SetRect(&m_rcBtns[METBTN_Meter_Down], m_rcBtns[METBTN_Meter_Up].right + iIndent, m_rcBtns[METBTN_Meter_Up].top, m_rcBtns[METBTN_Meter_Up].right + iIndent + iBtnSize, m_rcBtns[METBTN_Meter_Up].bottom);
    SetRect(&m_rcBtns[METBTN_StartStop], m_rcBtns[METBTN_Meter_Down].right + iIndent, m_rcBtns[METBTN_Meter_Down].top, m_rcBtns[METBTN_Meter_Down].right + iIndent + iBtnSize, m_rcBtns[METBTN_Meter_Down].bottom);
    
    SetRect(&m_rcTempoBtnLabel, m_rcBtns[METBTN_BPM_Up].left, m_rcBtns[METBTN_BPM_Up].bottom, m_rcBtns[METBTN_BPM_Down].right, m_rcBtns[METBTN_BPM_Up].bottom + iLabelHeight);
    SetRect(&m_rcMeterBtnLabel, m_rcBtns[METBTN_Meter_Up].left, m_rcBtns[METBTN_Meter_Up].bottom, m_rcBtns[METBTN_Meter_Down].right, m_rcBtns[METBTN_Meter_Up].bottom + iLabelHeight);
    SetRect(&m_rcStopWatch, m_rcBtns[METBTN_StartStop].left, m_rcBtns[METBTN_Meter_Up].bottom, m_rcBtns[METBTN_StartStop].right,  m_rcBtns[METBTN_Meter_Up].bottom + iLabelHeight);

    m_rcTap = m_rcCurrentMeter;
    m_rcTap.left -= GetSysMets(SM_CXICON);
    m_rcTap.right += GetSysMets(SM_CXICON);
    
    /*    SetRect(&m_rcBtns[METBTN_BPM_Up], rcClient.left + 2*iIndent, m_rcDisplay.bottom + 2*iIndent, rcClient.left + 2*iIndent + iBtnSize, m_rcDisplay.bottom + 2*iIndent + iBtnSize);
    SetRect(&m_rcBtns[METBTN_BPM_Down], m_rcBtns[METBTN_BPM_Up].right + iIndent, m_rcBtns[METBTN_BPM_Up].top, m_rcBtns[METBTN_BPM_Up].right + iIndent + iBtnSize, m_rcBtns[METBTN_BPM_Up].bottom);
    SetRect(&m_rcBtns[METBTN_StartStop], m_rcDisplay.right - 2*iIndent - iBtnSize, 
        m_rcBtns[METBTN_BPM_Up].top, m_rcDisplay.right - 2*iIndent, m_rcBtns[METBTN_BPM_Up].bottom);
    iTemp = (m_rcBtns[METBTN_StartStop].left - m_rcBtns[METBTN_BPM_Down].right - iBtnSize)/2 + m_rcBtns[METBTN_BPM_Down].right;
    SetRect(&m_rcBtns[METBTN_Meter_Up], iTemp, m_rcBtns[METBTN_BPM_Up].top, iTemp + iBtnSize, m_rcBtns[METBTN_BPM_Up].bottom);

    SetRect(&m_rcTempoBtnLabel, m_rcBtns[METBTN_BPM_Up].left, m_rcBtns[METBTN_BPM_Up].bottom, m_rcBtns[METBTN_BPM_Down].right, m_rcBtns[METBTN_BPM_Up].bottom + iLabelHeight);
    SetRect(&m_rcMeterBtnLabel, m_rcBtns[METBTN_Meter_Up].left - 2*iIndent, m_rcTempoBtnLabel.top, m_rcBtns[METBTN_Meter_Up].right + 2*iIndent, m_rcTempoBtnLabel.bottom);
*/
    return TRUE;
}

void CGuiMetronome::SetStrings()
{
    m_oStr->IntToString(m_szBPM, m_BPMinute);
    m_oStr->IntToString(m_szMeter, m_BPMeasure);
    m_oStr->IntToString(m_szCurrentBeat, 0);//BUGBUG

    if(m_BPMinute > 199)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Prestissimo), m_hInst));// > 199
    else if(m_BPMinute > 167)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Presto), m_hInst)); //> 167
    else if(m_BPMinute > 119)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Allegro), m_hInst)); //> 119
    else if(m_BPMinute > 107)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Moderato), m_hInst)); //> 107
    else if(m_BPMinute > 75)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Andante), m_hInst)); //> 75
    else if(m_BPMinute > 65)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Adagio), m_hInst)); //> 65
    else if(m_BPMinute > 59)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Larghetto), m_hInst)); //> 59
    else if(m_BPMinute > 39)
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_OPT_Largo), m_hInst)); //> 39
    else
        m_oStr->StringCopy(m_szTempoMarking, m_oStr->GetText(ID(IDS_INFO_Tempo), m_hInst));
}

void CGuiMetronome::SetStopwatchString()
{
    static SYSTEMTIME sTime;
    ZeroMemory(&sTime, sizeof(SYSTEMTIME));

    sTime.wHour   = m_iStopWatch/3600;
    sTime.wMinute = m_iStopWatch/60;
    sTime.wSecond = m_iStopWatch%60;
  /*  if(sTime.wHour > 0)
        GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sTime, _T("hh';'mm':'ss"), m_szTimer, STRING_LARGE);
    else*/
        GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sTime, _T("mm':'ss"), m_szTimer, STRING_LARGE);

}

BOOL CGuiMetronome::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_LEFT:
        
    	break;
    case VK_RIGHT:
        NextSoundSet();
    	break;
    case VK_UP:
        m_eBtnDown = METBTN_BPM_Up;
        TempoUp();
        break;
    case VK_DOWN:
        m_eBtnDown = METBTN_BPM_Down;
        TempoDown();
        break;
    case VK_RETURN:
        if(!m_bPlaying)
            Play();
        else
            Stop();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    default:
        return UNHANDLED;
        break;
    }

    m_eBtnDown = METBTN_Count;

    SetStrings();
    UpdateTimer();

    return TRUE;
}

void CGuiMetronome::UpdateTimer()
{

}

BOOL CGuiMetronome::Play()
{
    if(m_eMetSounds != g_options->GetMetroSound())
        LoadSettings();

    InvalidateRect(m_hWnd, &m_rcTap, FALSE);

    m_iStopWatch = 0;//reset the count
    SetStopwatchString();
    SetTimer(m_hWnd, IDT_STOPWATCH, 1000, NULL);

    unsigned long nLocInMeasure = 0;

    std::vector<std::vector<long                     > > alInstrumentNums;
    std::vector<std::vector<long                     > > alVolumes;
    std::vector<std::vector<long                     > > alBeatSizes;

    for (unsigned long iBeatQueueLoop = 0; iBeatQueueLoop < m_BPMeasure; ++iBeatQueueLoop)
    {
        if(m_BeatData[nLocInMeasure].is_single_value())
        {
            int const instrument_num = m_BeatData[nLocInMeasure].get_single_value();

            std::vector<long                     > atmplVolumes  ;
            std::vector<long                     > atmplBeatSizes;
            std::vector<long                     > atmplInstrumentNums;

            if (instrument_num >= 0)
            {
                atmplVolumes  .push_back(m_midi_volume[instrument_num]             );
                atmplBeatSizes.push_back(m_blink_size[instrument_num]              );
                atmplInstrumentNums.push_back(instrument_num);
            }
            else
            {
                atmplVolumes  .push_back(0);
                atmplBeatSizes.push_back(0);
                atmplInstrumentNums.push_back(-1);
            }

            alVolumes  .push_back(atmplVolumes  );
            alInstrumentNums  .push_back(atmplInstrumentNums);
            alBeatSizes.push_back(atmplBeatSizes);

        }
        else
        {//!!!This if-else is a bit wierd, you'd think it could be done with a single block of code.
            int multi_length = m_BeatData[nLocInMeasure].get_multiple_length();
            std::vector<long                     > atmplVolumes  ;
            std::vector<long                     > atmplBeatSizes;
            std::vector<long                     > atmplInstrumentNums;
            for(int i = 0; i < multi_length; i++)
            {
                int const instrument_num = m_BeatData[nLocInMeasure].get_multiple_value(i);
                std::basic_string<TCHAR> const strInstrument = (instrument_num >= 0)?
                    AlphaInstrumentStrings[MidiToAlpha[m_midi_instrument[instrument_num]]]:_T("");

                if (instrument_num >= 0)
                {
                    atmplVolumes  .push_back(m_midi_volume[instrument_num]             );
                    atmplBeatSizes.push_back(m_blink_size[instrument_num]              );
                    atmplInstrumentNums.push_back(instrument_num);
                }
                else
                {
                    atmplVolumes  .push_back(0);
                    atmplBeatSizes.push_back(0);
                    atmplInstrumentNums.push_back(-1);
                }
            }
            alVolumes  .push_back(atmplVolumes  );
            alInstrumentNums  .push_back(atmplInstrumentNums);
            alBeatSizes.push_back(atmplBeatSizes);
        }
        ++nLocInMeasure;
    }
    nLocInMeasure = 0;

    m_autopTicker = (std::auto_ptr<IBeatBox>)NULL;
    if (alInstrumentNums.size())
    {
#ifdef USE_WEIRD_MIDI
        m_autopTicker = (std::auto_ptr<IBeatBox>) new CBeatBox_MID(
#else //USE_WEIRD_WAV
        m_autopTicker = (std::auto_ptr<IBeatBox>) new CBeatBox_WAV(
#endif
            alInstrumentNums, m_midi_instrument, m_midi_volume, m_blink_size, m_BPMinute, m_hWnd);
    }

     // And, finally, set this behemoth Playing!
    if (m_autopTicker.get() != NULL)
    {
        //m_autopTicker->SetTempo(120);
        m_autopTicker->Play();

        m_bPlaying = TRUE;
    }
    return TRUE;
}

BOOL CGuiMetronome::Stop()
{
    if(m_bPlaying)
        m_autopTicker->Stop();

    m_bPlaying = FALSE;

    KillTimer(m_hWnd, IDT_STOPWATCH);

    InvalidateRect(m_hWnd, NULL, FALSE);
    
    return TRUE;
}

long  const s_DefaultInstruments[MAX_SOUNDS] =
{GM1_SIDE_STICK,GM1_BASS_DRUM_1,GM1_LOW_WOOD_BLOCK};

void CGuiMetronome::LoadSettings()
{
    m_BPMinute = g_options->GetMetroBPM();
    m_BPMeasure = g_options->GetMetroMeter();
    m_eMetSounds = g_options->GetMetroSound();

    UpdateBPM();

    for(char i = 0; i < 2; i++)
    {
        //m_midi_instrument[i] = s_DefaultInstruments[i];
        m_midi_instrument[i] = i+1+2*g_options->GetMetroSound();
        m_midi_volume[i] = 127;
        m_blink_size[i] = 8;
        m_MetronomeStyle = metPlain;
    }

    int iValue1 = 1;
    int iValue2 = 0;

    m_BeatData[0].set_single_value(iValue1);
    for(int i = 1; i < (int)m_BPMeasure; i++)
        m_BeatData[i].set_single_value(iValue2);
}

void CGuiMetronome::LoadImages()
{
    m_imgBtn.Initialize(m_hWnd, m_hInst, IDB_PNG_Btn);
    m_imgDisplay.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_Border_HR:IDB_PNG_Border);

    m_gdiMeter.LoadImage(IsVGA()?IDB_PNG_Meter_HR:IDB_PNG_Meter, m_hWnd, m_hInst);
    m_gdiStartStop.LoadImage(IsVGA()?IDB_PNG_OnOff_HR:IDB_PNG_OnOff, m_hWnd, m_hInst);
}

void CGuiMetronome::OnLostFocus()
{
    //save settings to g_options
    g_options->SetMetroBPM(m_BPMinute);
    g_options->SetMetroMeter(m_BPMeasure);
}

void CGuiMetronome::NextSoundSet()
{
    BOOL bPlay = FALSE;

    if(m_bPlaying)
    {
        bPlay = TRUE;
        StartStop();
    }

    EnumMetroSounds eSound = g_options->GetMetroSound();

    switch(eSound)
    {
    case METSND_Standard:
        eSound = METSND_Wood;
        break;;
    case METSND_Wood:
        eSound = METSND_Metal;
        break;
    default:
        eSound = METSND_Standard;
        break;
    }

    g_options->SetMetroSound(eSound);
//    LoadSettings();

    if(bPlay)
        StartStop();

}

void CGuiMetronome::ResetGDI()
{
    m_gdiBackground.Destroy();
}