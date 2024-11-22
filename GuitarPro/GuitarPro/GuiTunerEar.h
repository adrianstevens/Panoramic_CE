#pragma once
#include "guibase.h"

enum EnumButtonState
{
    BSTATE_Off,
//    BSTATE_30,
    BSTATE_40,
    BSTATE_50,
    BSTATE_60,
    BSTATE_70,
    BSTATE_80,
    BSTATE_90,
    BSTATE_On,
    BSTATE_Repeat,

};

class CGuiTunerEar : public CGuiBase
{
public:
    CGuiTunerEar(void);
    ~CGuiTunerEar(void);

    EnumGUI             GetGUI(){return GUI_TunerEar;};

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void                OnLostFocus();

private:
    BOOL                DrawFingers(CIssGDIEx& gdi, RECT& rc);
    BOOL                DrawText(CIssGDIEx& gdi, RECT& rcClip);

    UINT                GetNoteRes(int iString, int iFret);

    void                OnTuning();
	void				OnPlay(); //play all notes

private:
    CIssGDIEx           m_gdiAni;
    
    EnumButtonState     m_eButtonState[6];

    RECT                m_rcFingers[6];
    RECT                m_rcButtons[4];//large middle button
	RECT				m_rcPlay; //button to play all notes ... place on 4th fret

    int                 m_iFadeTimers[6];//just ids ... makes the code tidier

    int                 m_iSelected;
	int					m_iPlaying;

};
