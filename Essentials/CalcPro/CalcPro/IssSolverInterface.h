#pragma once
#include "calcinterace.h"
#include "IssDynDisplay.h"
#include "CalcDynBtn.h"
#include "IssString.h"
#include "EquationSolver.h"

class CIssSolverInterface :  public CCalcInterace
{
public:
    CIssSolverInterface(void);
    ~CIssSolverInterface(void);

    void		    Init(TypeCalcInit* sInit);

    BOOL            Draw(HDC hdc, RECT& rcClip);

    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);

    BOOL            OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

    TCHAR*          GetFunction(int iIndex);
    
    BOOL            OnGraph();

    BOOL            HandleDPad(int iVKKey);

    BOOL            IsSelected(){return m_bSelected;};

    BOOL            DrawSelector(HDC hdc, RECT& rc);
private:
    BOOL            DrawDisplay(HDC hdc, RECT& rcClip);
    BOOL            DrawButtons(HDC hdc, RECT& rcClip);//probably only one
    void            DrawDisplayText(HDC hDC, RECT& rc);
    

    void		    CreateButtons();//we'll pump these on Draw to keep loading times fast
    void		    CreateDisplays();

    void            LoadRegistry();
    void            SaveRegistry();


private:
  //  CEquationSolver m_oSolver;

    CIssDynDisplay  m_oDisplay;
    CCalcDynBtn     m_oFuncBtn;
    
    CIssString*     m_oStr;

    RECT            m_rcDisplay;
    RECT            m_rcFunction;

    HFONT           m_hFontDisplayTop;
    HFONT           m_hFontExp;
    HFONT           m_hFontBtnText;
    HFONT           m_hFontSmall;

    int             m_iCurrentGraph;

    BOOL            m_bSelected; // is the selector on the graph button?

};
