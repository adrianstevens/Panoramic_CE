#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "IssImageSliced.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseSettings.h"

#define MAX_Array_Width		9	//200	// maximum blocks for landscape
#define MAX_Array_Height	9	//200	// maximum blocks height

#define UND					-1	// Not defined


enum 
{   
    FAV_MENU_AddItem = 0,
    FAV_MENU_RemoveItem,
    FAV_MENU_NewProgram,
    FAV_MENU_NewSetting,
    FAV_MENU_SmartArrange,
    FAV_MENU_LockItems,
    NUM_FAV_MENU_Items
};
enum
{
    eProgramsPage,
    eGamesPage,
    NumFavPages
};

enum LButtonPressedState
{
    PS_noPress = 0,
    PS_checkTap,
    PS_pressed,
    PS_dragging
};

class CGuiFavorites:public CGuiBase
{
public:
	CGuiFavorites(void);
	~CGuiFavorites(void);

	BOOL	    Destroy();
	BOOL	    Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions);
	BOOL	    Draw(HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL	    OnLButtonDown(POINT pt);
	BOOL	    OnLButtonUp(POINT pt);
	BOOL	    OnMouseMove(POINT pt);
	BOOL	    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	    OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL	    OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 		OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    MoveGui(RECT rcLocation);
	BOOL 	    AddMenuItems();

    HRESULT     PreloadImages(HWND hWnd, HINSTANCE hInstance);
    void        ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    void        ResetSliderText();


	virtual BOOL	HasFocus();
	virtual void	LoseFocus();

    void        SetIconSize(EnumFavSize eFavSize);


private:	// functions
    // read / write
	void	        ReadRegistry();
	HRESULT	        SaveRegistry(int iItem = UND);

    // drawing
	void	        DrawItems(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip, int iExcludeItem, int ePage);
	void	        DrawItems(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip, int iExcludeItem = UND);
    void            DrawMovingItem();
	void	        DrawBackground(HDC dc, RECT& rcClient, RECT& rcClip);
	void	        DrawGrid(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void	        DrawSlider(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void	        DrawSelected(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawSliderBg(CIssGDIEx& gdi);
    void            DrawSliderKnob(CIssGDIEx& gdi, RECT& rc);
    void            DrawSliderArrow(CIssGDIEx& gdi);
    void            AnimateSlider(POINT pt, int iDestPage);
    void            SlidePage();
    HRESULT         Render();
    void            SetDirty(POINT ptStart, POINT ptEnd);   // args are matrix locations
    void            SetSliderDirty();
    void            AddItem();

    BOOL            OnSliderButtonDown(POINT pt);
    BOOL            OnSliderButtonUp(POINT pt);
    BOOL            OnSliderMouseMove(POINT pt);

    TCHAR*          GetSliderText();
	void	        DestroyLinkArray(CIssVector<TypeLaunchItem>& arrLinks);
    void            DestroyTempImages();

    // icon handling
	void	        CheckBounds(POINT& pt);
	POINT	        GetMatrixLocation(POINT pt);
	BOOL	        IsSwapStraight(POINT pt);
	BOOL	        IsSwapAndFindRoom(POINT pt);
	int		        GetItemIndex(POINT& pt);
	void	        SwapMatrixIndex(int& iHoldingIndex, POINT& pt);
	BOOL	        IsOverlapping(RECT& rcBig, RECT& rcSmall);
	BOOL	        IsSpaceAvailable(int** iMatrix, POINT& ptMatrix);
	BOOL	        SetMatrixValue(int** iMatrix, POINT& ptMatrix, int iValue, int iWidth, int iHeight);
	void	        CopyItemLink(CIssVector<TypeLaunchItem>* arrDest, CIssVector<TypeLaunchItem>* arrSrc);
	BOOL	        AutoArrangeItems(int** iMatrix, CIssVector<TypeLaunchItem>& arrContact);
	int		        FindItemLessThan(int iX, int iY, int iXCheck, int iYCheck);
	int		        FindItemGreaterThan(int iX, int iY, int iXCheck, int iYCheck);
	int		        FindLastItem();
	BOOL	        SmartArrangeItems();

    // dimensions
    int             GetSliderHeight();
    int             GetSliderTrackHeight();
    int             GetSliderKnobHeight();
    int             GetShadowOffset();

    // matrix
	BOOL	        FillMatrix(TypeLaunchItem* sItem, int iIndex, BOOL bPersonal);
	int**	        CreateMatrix(int iHeight, int iWidth);
	void	        DeleteMatrix(int** iMatrix, int iHeight);
	void	        CopyMatrix(int** iMatrixDest, int** iMatrixSrc, int iHeight, int iWidth);
	void	        SetMatrixOrientation(BOOL bLandscape = FALSE);
    void            ClearMatrix(int** iMatrix);
    int**           GetCurMatrix() {return (m_ePage == eProgramsPage) ?  m_iProgramsMatrix : m_iGamesMatrix;};

	void	        OnContextMenu(POINT pt);
	BOOL	        ResetSelectedItem();
	BOOL	        RemoveItem(int iIndex, int iArray = -1);
	void	        AddProgram();
    void            AddSetting();
    void            AddFileFolder();
	void	        LaunchItem();
	void	        CloseWindow();

	static void     fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	        AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);

    // gdi objects
    CIssGDIEx&      GetMovingItemMask();
    CIssGDIEx&      GetPlaceHolderImg();
    CIssGDIEx&      GetMovingNameImage(/*RECT& rcSize*/);
    CIssGDIEx&      GetNameBubbleAlphaMask(RECT rcSize);
    CIssGDIEx&      GetSliderBg();

    CIssImageSliced& GetSelector();
    CIssImageSliced& GetItemAlphaMask();
    CIssImageSliced& GetItemBorder();
    CIssImageSliced& GetItemDropShadow();
    CIssImageSliced& GetSliderImg(BOOL bLan = FALSE);
    CIssImageSliced& GetSliderTrack();
    CIssImageSliced& GetGridImg();

	int		        GetMaxItems()				{ return m_sizeMatrix.cx * m_sizeMatrix.cy;	}
    void            GetItemSize(RECT& rcClient, int& w, int& h);
    RECT            GetItemRect(POINT ptMatrixPos);
    RECT            GetItemRect(int iMatrixX, int iMatrixY);
    RECT            GetMovingNamePosition(RECT rcImage);
    RECT            GetSliderRect();
    RECT            GetSliderRect(int iPage, BOOL bDrag);
    int             GetSelected();
    BOOL            HasSelection();
    void            SetSelection(int x, int y);
    CIssVector<TypeLaunchItem> &GetCurLinkArray();
    CIssVector<TypeLaunchItem> &GetLinkArray(int iWhich);

    void            InitImage(RECT& rcFrame, TypeLaunchItem* sItem);
    BOOL            DrawItem(TypeLaunchItem* sItem, HDC dc, RECT rcDest, CIssGDIEx& gdiImage, HFONT hFont, BOOL bDrawZeroBase = FALSE, POINT* ptOverride = NULL, BOOL bAlwaysText  = FALSE);
    void            DrawItemText(HDC dc, TCHAR* szText, HFONT hFont, RECT& rcLocation, UINT uiAlignment = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);

private:	// variables
	HFONT				m_hFont;
	HFONT				m_hLabelFont;
	HFONT				m_hLabelFontRot;
	CIssGDIEx			m_gdiMovingItem;		// save the item image that we are moving
	CIssGDIEx			m_gdiMovingItemMask;	// rounded rect mask
	CIssGDIEx		    m_gdiPlaceholder;	    // use this image if no user pic
	CIssGDIEx		    m_gdiAddNew;	        // '+' image
	CIssGDIEx		    m_gdiNameBubble;	    // assembled gdi with name frame and text
	CIssGDIEx		    m_gdiNameBubbleMask;	// mask for name bubble gdi
	CIssGDIEx		    m_gdiSliderBg;	        // gdi to draw the slider bg to
    CIssGDIEx           m_gdiImgArrowArray;     // arrows for slider

    CIssImageSliced     m_imgGrid;              // grid that frames items
    CIssImageSliced     m_imgSelector;          // yellow round rect image
    CIssImageSliced     m_imgAlphaMask;         // mask for drawing item images
    CIssImageSliced     m_imgItemBorder;        // default border for items
    CIssImageSliced     m_imgItemDropShadow;    // for moving item
    CIssImageSliced     m_imgNameBubble;        // frame that holds moving item name
    CIssImageSliced     m_imgSlider;            // Personal/Business slider
    CIssImageSliced     m_imgSliderLan;         // Personal/Business slider landscape
    CIssImageSliced     m_imgSliderTrack;       // Personal/Business slider track 
    CIssImageSliced     m_imgSliderTrackLan;    // Personal/Business slider track landscape

	CIssGDIEx			m_gdiShine;
	CIssGDIEx			m_gdiBgTemp;
	CIssImageSliced		m_imgMask;
	CIssImageSliced		m_imgBorder;

	int**				m_iProgramsMatrix;      //[MAX_Array_Height][MAX_Array_Width];	// Array holds indexes 
	int**				m_iGamesMatrix;         //[MAX_Array_Height][MAX_Array_Width];	// Array holds indexes 
	CIssVector<TypeLaunchItem> m_arrPrograms;	// our array of personal contacts
	CIssVector<TypeLaunchItem> m_arrGames;	    // our array of business contacts
	
    SIZE				m_sizeMatrix;			// actual number of  blocks in the matrix
	SIZE				m_sizeBlock;			// size of one block item in pixels
	SIZE				m_sizeOffset;			// offset from for moving an item
	POINT				m_ptHolding;		    // point we were holding
	POINT				m_ptLastHolding;		// last point we were holding
	POINT				m_ptLastLButtonDown;	// last point we clicked
    POINT               m_ptSelection;
    LButtonPressedState m_eBtnPressState;       // see enums
	
    BOOL				m_bContextMenu;			// context menu is up
	BOOL				m_bLockItems;			// locked items, can't move them
    BOOL                m_bMouseDown;
    BOOL                m_bPressSelected;
    BOOL                m_bSliderDrag;
    
    long                m_ePage;
    int					m_iHoldingIndex;		// icon we are moving
    int                 m_iSliderPos;
    
    DWORD               m_dwTickCountGrab;

    RECT                m_rcSlider;             // the full slider area
    RECT                m_rcGrid;
    RECT                m_rcDragDraw;           // store this for invalidation
    RECT                m_rcMovingName;         // size only - for position use GetMovingNameLocation()

    TCHAR               m_szSliderText[2][STRING_NORMAL];
};
