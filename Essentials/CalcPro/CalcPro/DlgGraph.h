#pragma once
#include "stdafx.h"
#include "isswnd.h"
#include "calc.h"
#include "IssGDIEx.h"
#include "IssSciCalcUtil.h"
#include "Resource.h"
#include "IssImageSliced.h"
#include "CalcMenu.h"
#include "IssWndTouchMenu.h"

class CDlgGraph : public CIssWnd
{
#define MAX_GRAPHS 8

#define MAX_POINTS	100


enum EnumGraphMode
{
	MODE_Zoom,
	MODE_Translate,
	MODE_GetLocation,
};

enum EnumZoomMode
{
    ZOOM_Normal,
    ZOOM_X,
    ZOOM_Y,
};

public:
	CDlgGraph(void);
	~CDlgGraph(void);

    void    SetFullScreen(BOOL bFull){m_bFullscreen = bFull;};
	BOOL	SetGraph(TCHAR* szGraph, int iGraph);
	void	SetGraphAccuracy(int i); //just use 0, 1, 2 hi,med,low
	void	UsePoints(BOOL bUsePoints){m_bDrawLines = !bUsePoints;};
	BOOL	AddPoint(double dbX, double dbY);
	BOOL	SetDisplayRect(double dbLeft, double dbRight, double dbTop, double dbBottom); //think of it as a window, and we'll pass in a Rect to define our viewing window (this is for Stats)
	void	SetMenu(CCalcMenu* oMenu, CIssWndTouchMenu* wndMenu){m_oMenu = oMenu;m_wndMenu = wndMenu;};

private:
    COLORREF GetGraphColor(int i);

    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
//	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return TRUE;};
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);									
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);								
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);									
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL    OnSaveGraph();


	void	DrawGraphs(HDC dc);
	void	DrawSpecPoints(HDC dc);
	void	DrawAxis(HDC dc);
	void	DrawGrid(HDC dc);
	void    DrawSelect(HDC dc);
	void	DrawScale(HDC dc);
    void    DrawOtherText(HDC dc);
	void	DrawPointLocation(HDC dc);//we'll draw axis and onscreen co-rds
    void    DrawZoomButtons(CIssGDIEx* gdi, RECT rcClip);

	int		XToScreen(double dbX);
	int		YToScreen(double dbY);

	BOOL	TranslateGraph(POINT p);//This function will move the graph if you tap on the edges of the screen

	void	ResetValues();

	BOOL	CalcGraphPoints();

	//Popup Menus
	void	OnMenuGraph();
	void	OnMenuMode();


	void	ZoomIn();
	void	ZoomOut();
	void	OnMenuReset();
	void	OnMenuTranslate();
	void	OnMenuZoom();
	void	OnMenuTrackPoint();
	void	OnGraph1();
	void	OnGraph2();
	void	OnGraph3();
	void	OnGraph4();

	void	FormatScaleText(TCHAR* szNum, double dbNum);

	void	CenterGraph();

    void    FitToData();

	int		GetSystemMetrics(__in int nIndex);

	
private:
	SciCalc*		m_oCalc; //insert your favorite string parsing calc engine here  
	CIssSciCalcUtil* m_oSciUtil;
	CCalcMenu*		m_oMenu;
	CIssWndTouchMenu*   m_wndMenu;      // Popup menu

    CIssImageSliced m_imgButton;

	TCHAR			*m_szGraphs[MAX_GRAPHS];

	CIssGDIEx		m_oGDI;
	CIssGDIEx 	    m_oGDIBuffer;
    
	HMENU			m_hMenuBar;
	HFONT			m_hFontText;
	HWND			m_hWndMenu;

    EnumGraphMode	m_eGraphMode;
    EnumZoomMode    m_eZoomMode;

	//Pen This
	HPEN			m_hPen[MAX_GRAPHS];
	HPEN			m_hOldPen;
	HPEN			m_hPenGrid;
	HPEN			m_hPenSelect;

	int				m_iScreenWidth;
	int				m_iScreenHeight;
	int				m_iAcuracy; //0 is max....value defines how many points are skipped

	double			m_dbXScale;		//the number of X values on screen as a double...so 10 goes from -5 to +5
	double			m_dbYScale;
	double			m_dbXTranslation;//positive is positive etc...ie + 1 means we're centered +1 to the right of the center
	double			m_dbYTranslation;//translations are exact to graph, not scaled 

	//Graph Points
	long int		*m_iYGraph[MAX_GRAPHS];

	//Specific Points (used for regression
	double			m_dbSpecX[MAX_POINTS];
	double			m_dbSpecY[MAX_POINTS];
	int				m_iNumPoints;

	BOOL			m_bDrawLines;
	BOOL			m_bDrawGrid;
	BOOL			m_bMouseMoved;
	BOOL			m_bLButtonDown;
    BOOL            m_bFullscreen;

    RECT            m_rcZoomIn;
    RECT            m_rcZoomOut;
	
	POINT			m_ptStartSelect;
	POINT			m_ptEndSelect;

	POINT			m_ptMoveTo;
    POINT           m_ptWindowSize;

	int				m_iGraphSelected;

	COLORREF		m_crBackGround;
	COLORREF		m_crText;
	COLORREF		m_crLineColor;
	COLORREF		m_crAxisColor;
};
