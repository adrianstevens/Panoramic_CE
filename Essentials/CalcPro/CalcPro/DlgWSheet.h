#pragma once
#include "IssWnd.h"
#include "CalcInterace.h"//for the includes and the struct 
#include "IssVector.h"
#include "IssRegistry.h"

#define REG_WorkSheets _T("SOFTWARE\\Pano\\CalcPro\\Worksheets")

enum EntryType
{
    ENTRY_Double,
    ENTRY_Int,
    ENTRY_Percent,
    ENTRY_Currency,
};

#define MAX_WS_ENTRIES 15

struct TypeEntry
{
    TCHAR*	szEntryLabel;
    TCHAR	szEntryValue[STRING_NORMAL];
    EntryType eEntry;
    BOOL    bGreyed;
    BOOL    bLongValue;//might not need this 
};

class CDlgWSheet : public CIssWnd
{
public:
    CDlgWSheet(void);
    ~CDlgWSheet(void);

    BOOL        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL        OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL        OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam);

    virtual void Draw(HDC hDC, RECT& rcClip); // easier so we don't have to override OnPaint
    virtual void DrawButtons(HDC hDC, RECT& rcClip);
    virtual void DrawButtonsSP12(HDC hDC, RECT& rcClip);
    virtual void DrawButtonsSPLand(HDC hDC, RECT& rcClip);

    virtual void Init(TypeCalcInit* sInit, CHandleButtons* oHandBtns, BOOL bPlaySounds, HWND hWnd);

    virtual void OnBtnSetBtnPressed();//so the interface can let us know something has been pressed....we could do this via a message too

	int			 GetSystemMetrics(__in int nIndex);

protected:
    virtual BOOL OnCalc();
    virtual BOOL OnReset();
    virtual BOOL OnGraph();
    virtual BOOL OnLoad();
    virtual BOOL OnSave();
    
    BOOL        DrawScreenEntries(HDC hDC);
    void        DrawSelector(HDC hDC);

    virtual BOOL InitScreenEntries();
    void        SetScreenEntryFromCalcEngine();

    void        CreateDisplay();
    BOOL        CreateButtons();

    void		DeleteDynBtnVector(CIssVector<CCalcDynBtn>* arr);

    void        FormatInt(TCHAR* szIn);
    void        FormatPercent(TCHAR* szIn);
    void        FormatMoney(TCHAR* szIn);

    void        DeleteEntryArray();

    void        DrawPanels(HDC hdc, RECT& rcClip);//probably too lazy to optomize

    void        DrawArrows(HDC hdc, BOOL bUp, int iX, int iY);

    void        DrawBtnSelector(HDC hDC, RECT& rcClip);

    void        PlaySounds();
    
    virtual void Destroy();

    virtual void SetScreenValue(){};//used when the selector is moved for any reason



    void        DeleteFonts();
    void        CreateFonts();


protected:
    CIssString*         m_oStr;
    CCalcMenu           m_oMenu;
    
    CIssWndTouchMenu*   m_wndMenu;      // Popup menu

    CHandleButtons*     m_oHandBtns;

    CIssGDIEx           m_gdiMem;//created and destroyed ... I'm good with it

    CIssDynDisplay      m_oDisplay;

    CIssVector<CCalcDynBtn>	m_arrButtons;
    CCalcDynBtn*        m_pCalcBtns[6]; // so we don't create the same buttons twice

    CIssCalculator*		m_oCalc;
    CHandleButtons*		m_oBtnHand;//makes sure we don't create more btns than we need
    CCalcBtnManager*	m_oBtnMan;//reads the config files

    CIssVector<TypeEntry> m_arrEntries;

    SIZE                m_szWindowSize;

    TCHAR               m_szTitle[STRING_LARGE];

    RECT                m_rcMainBtns;
    RECT                m_rcDisplay;

    RECT                m_rcEntries[MAX_WS_ENTRIES];//how many could there possibly be?

    HWND                m_hWndParent;

    HFONT		        m_hFontMainBtnText;
    HFONT		        m_hFontMainLongText;
    HFONT               m_hFontDisplay;
    HFONT               m_hFontDisplaySm;

    int					m_iEntryIndex; //Index for the value at the top of the screen
    int					m_iScreenIndex; //Index for the onscreen selected Value
    int                 m_iLinesOnscreen;
    int                 m_iTextHeight;
    int                 m_iSmallTextHeight;

    int                 m_iLeftTextEdge;//useful for the selector
    int                 m_iSelectorIndex;//smartphone selector ... not the worksheet

    //for font creation
    int                 m_iButtonWidth;
    int                 m_iButtonHeight;	

    BOOL                m_bLandScape;//just easier
    BOOL                m_bPlaySounds;//passed in
    BOOL                m_bDrawBtnSelector;

    COLORREF            m_crLight;//if the text gets too long


    TypeEntry*          m_sTemp;
};
