#pragma once
#include "guibase.h"


enum EnumPlayButtons
{
    PLAYBTNS_FretUp,
    PLAYBTNS_FretDown,
    PLAYBTNS_None,
    PLAYBTNS_Root,
};

enum EnumScaleDraw
{
    SDRAW_Note,
    SDRAW_Root,
    SDRAW_None,
};

class CGuiPlay : public CGuiBase
{
public:
    CGuiPlay(void);
    ~CGuiPlay(void);

    EnumGUI             GetGUI(){return GUI_Play;};

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);

    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void                OnLostFocus();

private:
    BOOL                DrawText(CIssGDIEx& gdi, RECT& rcClip);
    BOOL                DrawFingers(CIssGDIEx& gdi, RECT& rcClip);
    BOOL                DrawScale(CIssGDIEx& gdi, RECT& rcClip);
    BOOL                DrawPosButtons(CIssGDIEx& gdi, RECT& rcClip);
    
    UINT                GetNoteRes(int iString, int iFret);

    void                OnTuning();
    void                OnRoot();
    void                OnScale();
    void                OnShift();//go between normal and 0 fret

    void                OnFretUp();
    void                OnFretDown();

    EnumScaleDraw       GetScaleDrawType(int iString, int iFret, EnumChordRoot eRoot);

    void                DrawArrow(HDC hdc, RECT& rcButton, COLORREF crColor, BOOL bDown);

    BOOL                PlayScale(BOOL bAni = TRUE);

private:
    RECT                m_rcPlayArea; //faster redraws by skipping the menu/top buttons

    RECT                m_rcGrid;
    RECT                m_rcFretNums[MAX_FRETS];

    RECT                m_rcOpenStrings[6];
    RECT                m_rcFingers[6][MAX_FRETS];
    RECT                m_rcButtons[4];
    RECT                m_rcPosition[2];

    POINT               m_ptLastPressed;//string and fret

    int                 m_iBottomFret;

    int                 m_iAlpha;

};
