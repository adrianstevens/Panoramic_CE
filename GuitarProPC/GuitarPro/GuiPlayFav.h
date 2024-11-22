#pragma once
#include "guibase.h"

class CGuiPlayFav : public CGuiBase
{
public:
    CGuiPlayFav(void);
    ~CGuiPlayFav(void);


    EnumGUI             GetGUI(){return GUI_Play;};

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);

    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnMouseMove(HWND hWnd, POINT& pt);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void                OnLostFocus();
    void                OnSetFocus();

private:
    BOOL                DrawText(CIssGDIEx& gdi, RECT& rcClip);
    BOOL                DrawFingers(CIssGDIEx& gdi, RECT& rcClip);
    BOOL                DrawPosButtons(CIssGDIEx& gdi, RECT& rcClip);

    UINT                GetNoteRes(int iString, int iFret);

/*    void                OnTuning();
    void                OnRoot();
    void                OnScale();*/


 /*   void                OnFretUp();
    void                OnFretDown();*/

    void                DrawArrow(HDC hdc, RECT& rcButton, COLORREF crColor, BOOL bDown);

    BOOL                PlayChord(int iIndex);//looks about right .....



private:
    RECT                m_rcPlayArea; //faster redraws by skipping the menu/top buttons

    RECT                m_rcGrid;
    RECT                m_rcFretNums[MAX_FRETS];
    RECT                m_rcChordNames[MAX_FRETS];

    RECT                m_rcOpenStrings[6];
    RECT                m_rcFingers[6][MAX_FRETS];
    RECT                m_rcButtons[4];
    RECT                m_rcPosition[2];

    POINT               m_ptLastPressed;//string and fret

    int                 m_iBottomFret;

    int                 m_iChordCount; //we'll probably get rid of this but fine for now
};
