#pragma once
#include "guibase.h"


enum EnumButtonState
{
    BSTATE_Off,
    BSTATE_On,
    BSTATE_Repeat,

};

struct StructTuning
{
    TCHAR szName[STRING_NORMAL];
    TCHAR szString1[STRING_NORMAL];
    TCHAR szString2[STRING_NORMAL];
    TCHAR szString3[STRING_NORMAL];
    TCHAR szString4[STRING_NORMAL];
    TCHAR szString5[STRING_NORMAL];
    TCHAR szString6[STRING_NORMAL];
    TCHAR szWave1[STRING_NORMAL];
    TCHAR szWave2[STRING_NORMAL];
    TCHAR szWave3[STRING_NORMAL];
    TCHAR szWave4[STRING_NORMAL];
    TCHAR szWave5[STRING_NORMAL];
    TCHAR szWave6[STRING_NORMAL];
    int   iCount;
};


class CGuiGuitarTuner : public CGuiBase
{
public:
    CGuiGuitarTuner(void);
    ~CGuiGuitarTuner(void);

    void            Init(HWND hWnd, HINSTANCE hInst);

    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
  
private:
    BOOL            DrawText(CIssGDIEx& gdi, RECT& rc);
    BOOL            DrawButtons(CIssGDIEx& gdi, RECT& rc);
    
    void            LoadTuning();
    void            LoadImages(HWND hWnd, HINSTANCE hInst);

	void			ReloadSounds();
    

private:
    CIssImageSliced     m_imgBtn;
    CIssImageSliced     m_imgBtnRed;
    CIssImageSliced     m_imgBtnGreen;

    StructTuning        m_sTuning[9];

    EnumButtonState     m_eButtonState[6];

    RECT                m_rcTopText;

    RECT                m_rcBtns[6];

    RECT                m_rcBottomText;

    HFONT				m_hfButtonText;
    HFONT				m_hfScreenText;

    BOOL				m_bPlaySounds;

    int                 m_iTuning;
    int                 m_iSelected;


};
