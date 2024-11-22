#pragma once
#include "wndinfoedit.h"
#include "Tank.h"
#include "Fractal.h"

class CWndBenchmark : public CWndInfoEdit
{
public:
    CWndBenchmark(void);
    ~CWndBenchmark(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();
    BOOL        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
//    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

    int         GetScore(){return m_iScore;};
    void        SetBestScore(int iScore){m_iHiScore = iScore;};

private:
    BOOL        IntegerBM();
    BOOL        FloatingPointBM();
    BOOL        MemoryBM();
    BOOL        FileIOBM();
    BOOL        FillRateBM();
    BOOL        FractalBM();
    BOOL        IntegerDraw();
    

private:
//    CFractal    m_oFractal;

    CTank       m_oTank;

    UINT        m_uiIntTime;
    UINT        m_uiFloatTime;
    UINT        m_uiMemoryTime;
    UINT        m_uiFileIOTime;
    UINT        m_uiFillRateTime;
    UINT        m_uiFractalTime;

    int         m_iScore;
    int         m_iHiScore;

    UINT        m_uiFillRate;
    double      m_dbFrameRate;

    BOOL        m_bTesting;

    DWORD       m_dwMath;
    double      m_dbMath;



};
