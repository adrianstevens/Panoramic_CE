#pragma once
#include "isswnd.h"
#include "IssKineticList.h"
#include "IssPicControl.h"
#include "IssPicLoader.h"
#include "SudokuGlobals.h"
#include "ObjGui.h"

enum EnumOption
{
    OPTION_Skin,
    OPTION_PlaySounds,
    OPTION_Background,
    OPTION_ShowErrors,
    OPTION_ShowComplete, //lock state
    OPTION_HintInidicators, //show possible moves on the bottom buttons
    OPTION_Image, // what type of custom background
    OPTION_ShowBlankCells,
    OPTION_ShowHighlight,
    OPTION_AutoRemovePencil,
    OPTION_Language,
};

struct TypeOptions
{
    BOOL			bPlaySounds;
    EnumSudokuSkins eSkin;
    EnumBackground  eBackground;
    BOOL            bShowErrors;
    BOOL            bShowComplete;
    BOOL            bShowHints; //ie .. show possible pencils on the buttons
    BOOL            bShowHighlights;
    BOOL            bDrawBlank;
    BOOL            bAutoRemovePencil;
    TCHAR           szPath[MAX_PATH];
};

enum EnumSelection
{
    SEL_None,       // nothing selected
    SEL_Skin,       // skin popup selected
};

struct TypePicSkinItem
{
    TypePLHolder* sPic;

    TypePicSkinItem();
    ~TypePicSkinItem();
};

class CDlgOptions :	public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

    void	Init(HINSTANCE hInst, 
                 TypeOptions& sOptions);

	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoSudoku.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam); 
    BOOL    OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);


private:	// functions
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteItem(LPVOID lpItem);
    
    void                DrawImageControls(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc);


    void                GetSkinText(EnumSudokuSkins eSkin, TCHAR* szInfo);
    void                GetBackgroundText(EnumBackground eBack, TCHAR* szInfo);
    void                GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    void                PopulateOptions();
    void                LaunchPopupMenu();
    void                SetMenuSelection();
    void		        OnMenuLeft();
    void		        OnMenuRight();

    void                SaveSettings();

    void                LoadImages();

    void                SetCustomPreview();

    BOOL                IsCustomizable();

    void                OnHandleImageSelect();
    void                OnHandleTakePicture();
    void                OnHandleAddPicture();
    void                OnHandleCustomBackground();
    BOOL                AddPicture(TCHAR* szFileName);
    void                DrawButtons(RECT& rcClip);

    void                SetPreview();

    void                AdjustWindow(HWND hWnd, BOOL bFullScreen);

    // pic control functions
    static void		    DeleteMyPicListItem(LPVOID lpItem);
    static void         DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DrawTitle(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, LPVOID lpClass);
    static void         DrawDots(CIssGDIEx& gdi, RECT& rcDraw, int iCurrentIndex, int iTotalCount, LPVOID lpClass);
    static HRESULT      LoadPicItem(CIssGDIEx& gdiDest, TypePLItem* sItem, LPVOID lpClass, LPARAM lpItem);
    HRESULT             LoadMyPicItem(CIssGDIEx& gdiDest, LPARAM sItem);
    void                DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted);
    void                DrawPicControl(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                PopulatePicItems();
    void                ResetPicItems();
    void                PopulatePicControl();

    void                SetPicControlBackground(EnumSudokuSkins eSkin);


public:
    TypeOptions         m_sOptions;
    	

private:	// variables
    CIssVector<TypePicSkinItem> m_arrSkins;
    
    CIssKineticList     m_oMenu;
    CIssPicControl      m_oPicSkin;
    CIssPicLoader       m_oPicLoader;
    CRITICAL_SECTION    m_crLoad;

    CIssGDIEx           m_gdiBackground;
    CIssGDIEx           m_gdiPictureThumb;
    CIssGDIEx           m_gdiIcons;
    CIssGDIEx           m_gdiCustom;
    CIssGDIEx           m_gdiArray;
    CIssGDIEx           m_gdiOutline;
    CIssGDIEx           m_gdiPicCtrlBG;
    CIssGDIEx           m_gdiPicBackground;//full background for performance
    
    CIssImageSliced     m_imgButton;
    CIssImageSliced     m_imgOutline;

    EnumSelection       m_eSelection;
    EnumPicSelection    m_ePicSelection;

    TCHAR               m_szTemp[STRING_MAX];
    TCHAR               m_szPicFileName[STRING_MAX];
    TCHAR               m_szDeleteFile[STRING_MAX];

    RECT                m_rcPicSkin;
    RECT                m_rcCamera;
    RECT                m_rcPicture;
    RECT                m_rcGiven;

    HFONT				m_hFontBtnText;
    HFONT               m_hFontLabel;

    POINT               m_ptLastLButtonUp;
};
