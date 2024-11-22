#pragma once
#include "guibase.h"
#include "FreqTuner.h"

enum EnumPitchIndicators
{
    PITCH_Off,//blue
    PITCH_Faint,
    PITCH_Med,
    PITCH_Bright,
    PITCH_Full,
    PITCH_LOff,//left blend
    PITCH_LFaint,
    PITCH_LMed,
    PITCH_LBright,
    PITCH_RBright,
    PITCH_RMed,
    PITCH_RFaint,
    PITCH_ROff,//right blend
    PITCH_GOff,//green (center)
    PITCH_GFaint,
    PITCH_GMed,
    PITCH_GBright,
    PITCH_GFull,
    PITCH_Count,
};

#define NUMBER_OF_ARROWS 4


class CGuiGuitarTuner : public CGuiBase
{
public:
    CGuiGuitarTuner(void);
    ~CGuiGuitarTuner(void);

    void            Init(HWND hWnd, HINSTANCE hInst);
    EnumGUI         GetGUI(){return GUI_Tuner;};

    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL            OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void            OnSetFocus();
    void            OnLostFocus();

    void            ResetGDI();
  
private:
    BOOL            DrawText(CIssGDIEx& gdi, RECT& rc);
    BOOL            DrawPitchBar(CIssGDIEx& gdi, RECT& rc, int iPitch);//should be between -50 & 50
    BOOL            DrawGraph(CIssGDIEx& gdi, RECT& rc);
    BOOL            DrawQualityBar(CIssGDIEx& gdi, RECT& rc, int iPcnt);
        
    void            InitBackground();
    void            ClearValues();

    CIssGDIEx*      GetPitchImage();
    CIssGDIEx*      GetFlatArrows();
    CIssGDIEx*      GetSharpArrows();
    CIssGDIEx*      GetInTuneArrow();
   
    

private:
    CFreqTuner          m_oFreq;    

    CIssImageSliced     m_imgSigQual;

    CIssGDIEx           m_gdiBackground;
    CIssGDIEx           m_gdiBars;
    CIssGDIEx           m_gdiFlatArrows;
    CIssGDIEx           m_gdiSharpArrows;
    CIssGDIEx           m_gdiInTunerArrow;



    HFONT               m_hFontFreq;
    HFONT               m_hFontNote;

    RECT                m_rcFreq;
    RECT                m_rcGraph;
    RECT                m_rcGraphText;
    RECT                m_rcSigStren;
    RECT                m_rcSigQual;
    RECT                m_rcSigQualText;
    RECT                m_rcNote;
    RECT                m_rcOctive;
    RECT                m_rcLEDArea;
    RECT                m_rcPitch;//ie sharp or flat
    RECT                m_rcPitchText;
    RECT                m_rcFlat; //left
    RECT                m_rcSharp;
    RECT                m_rcInTune; //green arrow at the top ....

    RECT                m_rcUpdateRgn;


    RECT                m_rcTitle; //might ditch this for the controls .....



    int                 m_iNumBars;
    int                 m_iCenter;

    //tuner state
    int                 m_iPcntTune;
    int                 m_iSigQual;
    TCHAR               m_szNote[STRING_SMALL];
    TCHAR               m_szOctave[STRING_SMALL];
    TCHAR               m_szFreq[STRING_NORMAL];
};
