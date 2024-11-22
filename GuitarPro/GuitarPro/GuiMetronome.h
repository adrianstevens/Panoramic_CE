#pragma once
#include "guibase.h"

#include "group_beat.h"
#include "metronome\IBeatBox.h"

#define MAX_BPM 240
#define MIN_BPM  40

#define MAX_METER 12
#define MIN_METER 0 

#define NUM_TAPS  8

enum EnumMetroBtns
{
    METBTN_BPM_Up,
    METBTN_BPM_Down,
    METBTN_Meter_Up,
    METBTN_Meter_Down,
    METBTN_StartStop,
    METBTN_Count,
};

class CGuiMetronome : public CGuiBase
{
public:
    CGuiMetronome(void);
    ~CGuiMetronome(void);

    EnumGUI             GetGUI(){return GUI_Metronome;};

    void                Init(HWND hWnd, HINSTANCE hInst);

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL                OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

    //not sure yet ....
    void                OnSetFocus(){};
    void                OnLostFocus();

    void                ResetGDI();

private:
    void                SetStrings();
    void                SetStopwatchString();

    BOOL                DrawDisplay(CIssGDIEx& gdi, RECT rcClip);
    BOOL                DrawLEDs(CIssGDIEx& gdi, RECT rcClip);
    BOOL                DrawBtns(CIssGDIEx& gdi, RECT rcClip);

    void                UpdateTimer();

    void                DrawFlash();//temp code

    BOOL                Play();
    BOOL                Stop();

    void                LoadSettings();

    void                LoadImages();

    void                InitBackground();

    void                UpdateBPM();//from the int

    void                FlashLED(int iLed);//0, 1, 2 or 3 ... on or off states
    void                UpdateMeterInd();

    void                TempoUp();
    void                TempoDown();
    void                StartStop();
    void                Tap();

    void                NextSoundSet();

    

private:
/*******************/
    enum METRONOME_STYLES{metPlain, metMeasure, metGroup};
    METRONOME_STYLES    m_MetronomeStyle;

    BOOL                m_bPlaying;
    BOOL                m_bBlinking;

    double              m_MaxBPM;
    unsigned long       m_BPMeasure;
    unsigned long       m_BPMinute;
    unsigned long       m_CurrentBeat; //added by me .. gotta sync

    unsigned long       m_TimerInterval_ms;
    unsigned long       m_ApproxElapsedTime_ms;
    unsigned long       m_MetronomeBeatsEveryThisMany_ms;

    std::vector<group_beat> m_BeatData;
    std::vector<int>        m_midi_instrument;
    std::vector<int>        m_midi_volume;
    std::vector<int>        m_blink_size;
    std::auto_ptr<IBeatBox> m_autopTicker;
/*******************/

    TCHAR               m_szBPM[STRING_SMALL];//tempo .. beats per min yo
    TCHAR               m_szMeter[STRING_SMALL];//current setting shown on display
    TCHAR               m_szCurrentBeat[STRING_SMALL];//current beat in the meter
    TCHAR               m_szTempoMarking[STRING_NORMAL];//italian
    TCHAR               m_szTimer[STRING_NORMAL]; //stop watch ....

    CIssGDIEx           m_gdiBackground;
    CIssGDIEx           m_gdiBtnIcons;
    CIssGDIEx           m_gdiMeter; //thin green bar
    CIssGDIEx           m_gdiStartStop;

    CIssImageSliced     m_imgDisplay;
    CIssImageSliced     m_imgBtn;

    EnumMetroBtns       m_eBtnDown;
    EnumMetroSounds     m_eMetSounds;

    HFONT               m_hFontBPM;
    HFONT               m_hFontMeter;

    RECT                m_rcBtns[METBTN_Count];
   

    RECT                m_rcDisplay; //large display
    RECT                m_rcLed1;//left led
    RECT                m_rcLed2;//right led
    RECT                m_rcBPM;//large Tempo in display
    RECT                m_rcTempoLabel;//place where is says "tempo" ... italian ...
    RECT                m_rcTempoBtnLabel;//"tempo" under buttons
    RECT                m_rcMeter;    // ie 4/12
    RECT                m_rcMeterDown;
    RECT                m_rcCurrentMeter;//large meter value
    RECT                m_rcMeterBtnLabel;//"meter" under buttons
    RECT                m_rcMeterLabel;//"meter" on display
    RECT                m_rcMeterInidcator;//hmmm ...
    RECT                m_rcPlayStopInd;//play stop indicators on display
    RECT                m_rcStopWatch;

    RECT                m_rcTap;

    DWORD               m_dwTaps[NUM_TAPS];

    int                 m_iStopWatch;
    
};
