#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "IssImageSliced.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseSettings.h"
#include "DlgContactDetails.h"
#include "IssPicControl.h"
#include "IssPicLoader.h"
#include "ObjProcesses.h"

#define MAX_NUM_Pages	6
#define MAX_Items		20

enum EnumFavType
{
	FAV_Program = 0,
	FAV_Setting,
	FAV_FileOrFolder,
	FAV_Contact,
};
enum EnumTitleName
{
    TTL_Programs = 0,
    TTL_Games,
    TTL_Apps,
    TTL_Settings,
    TTL_Favorites,
    TTL_Utilities,
    TTL_Music,
    TTL_Folders,
    TTL_Files,
    TTL_Videos,
	TTL_Contacts,
    TTL_Pictures,
	TTL_Personal,
	TTL_Business,
    TTL_Blank,
	TTL_Count,
};



struct TypeFavItem
{
	TCHAR*	szFriendlyName;
	TCHAR*	szPath;
	int		iIndex;
	RECT	rcLoc;
	EnumFavType eType;
    TypePLHolder* sPic;
	BOOL	bDrawName;

	TypeFavItem();
	~TypeFavItem();
	void Destroy();
	void Launch(HWND hWnd);
};

struct TypePage
{
	BYTE	arrGrid[MAX_Items][MAX_Items];
	CIssVector<TypeFavItem>	arrFavs;
    EnumTitleName eTitle;

	TypePage();
	~TypePage();
	void Destroy();
};


class CGuiFavorites:public CGuiBase
{
public:
	CGuiFavorites(void);
	~CGuiFavorites(void);

	BOOL	    Destroy();
	BOOL	    Init(HWND hWndParent, HINSTANCE hInst);
	BOOL	    Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL	    OnLButtonDown(POINT pt);
	BOOL	    OnLButtonUp(POINT pt);
	BOOL	    OnMouseMove(POINT pt);
	BOOL	    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	    OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL	    OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL		OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL 		OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	    MoveGui(RECT rcLocation);
	BOOL 	    AddMenuItems();

    void        ClearImageLoader();
    void        ClearImageLoaderQueue();
    void        UpdateImageLoader();

    HRESULT     PreloadImages(HWND hWnd, HINSTANCE hInstance);
    void        ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    void        ResetSliderText();


	virtual BOOL	HasFocus();
	virtual void	LoseFocus();

    void        SetIconSize();
	void		OnTimerChange();
	BOOL		HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);


private:	// functions
    // read / write
	HRESULT	        ReadRegistry();
	HRESULT	        SaveRegistry();
	HRESULT			SaveRegistryPage(int iIndex, BOOL bSaveGridOnly = FALSE);
	HRESULT			AddItem(TCHAR* szFriendlyName, TCHAR* szPath, int iIconIndex, EnumFavType eType, TypePage* sPage = NULL);

	void			DrawTitle(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void			DrawRecycleBin(CIssGDIEx& gdi, RECT& rcClip, BOOL bDrawGlow);
	void			DrawItem(CIssGDIEx& gdi, RECT rcLocation, TypeFavItem* sFav, CIssGDIEx* gdiBox, CIssGDIEx* gdiShine, RECT rcTemp, BOOL bDrawTextAlways, BOOL bDrawMoving = FALSE);

	void			OnAddNewItem();
	void			OnRunningPrograms();
	void			OnSelectRunningProgram();
	void			OnCloseAll();
	void			OnSoftReset();
    HRESULT         AddProgram();
    HRESULT         AddSetting();
	HRESULT			AddContact();
    HRESULT         AddFileFolder();
	void			DeletePages();
	HRESULT			EnsurePages();
	HRESULT			AddPage();
	HRESULT			RemoveItem(int iPage, POINT ptSelectedItem);
	HRESULT			LaunchSelectedItem();

	BOOL			IsValidGrid(POINT& ptItem);

	static void		DrawRunningPrograms(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
	void			DrawMyRunningPrograms(CIssGDIEx& gdi, RECT& rcDraw, TypeProcessInfo* sInfo);
	static void		DeleteRunningProgramItem(LPVOID lpItem);
	static void		DeleteMyPicListItem(LPVOID lpItem);
	static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static HRESULT  PicLoader(CIssGDIEx& gdi, TypePLItem* sItem, LPVOID lpclass, LPARAM lpItem);
	void            DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted);
    HRESULT         MyPicLoader(CIssGDIEx& gdi, TypePLItem* sItem, LPARAM lpItem);

	CIssGDIEx*		GetBox();
	CIssGDIEx*		GetShine();
	CIssGDIEx*		GetPlus();
	CIssGDIEx*		GetSelector();

    TCHAR*          GetTitleName(EnumTitleName eTitle = TTL_Count);
	void			LaunchMenuSelector();
	void			SetCurrentTitle();
	HRESULT         PoomItemChanged(long lOid);
	HRESULT         PoomItemDeleted(long lOid);
	HRESULT			LoadNewerStyleIcon(CIssGDIEx& gdiDest, TCHAR* szSourceLink, BOOL bIsSetting);

    // drawing
private:	// variables
	CRITICAL_SECTION m_crImages;
	CIssPicControl	m_oPicControl;
    CIssPicLoader   m_oPicLoader;
	CIssVector<TypePage> m_arrPages;

	CIssGDIEx		m_gdiBox;
	CIssGDIEx		m_gdiShine;
	CIssGDIEx		m_gdiPlus;
	CIssGDIEx		m_gdiDots;
	CIssGDIEx		m_gdiRecycleBin;
	CIssGDIEx		m_gdiSelector;
	CIssGDIEx		m_gdiRunningPrograms;
	CIssGDIEx		m_gdiCloseIcon;
	CIssImageSliced m_imgTitle;

#ifndef NOPHONE
	CDlgContactDetails m_dlgContactDetails;
#endif
	CObjProcesses	m_objWindows;

	RECT			m_rcTitle;
	RECT			m_rcRunningPrograms;
	RECT			m_rcPicControl;
	RECT			m_rcRecycleBin;			// for deleting the item

	int				m_iNumHorz;
	int				m_iNumVert;
	int				m_iHorzSpacing;
	int				m_iVertSpacing;
	int				m_iIconSize;

	POINT			m_ptMouseDown;
	POINT			m_ptMouseCurrent;		// current XY position
	POINT			m_ptRealMouseCurrent;	// used for recycle bin

	POINT			m_ptLastMouse;			// last mouse position
	POINT			m_ptDropItem;
	POINT			m_ptOffset;				// offset to selected item
	POINT			m_ptSelectedItem;		// are we selecting an item

	BOOL			m_bGrabAndMove;
	BOOL			m_bLButtonDown;
	BOOL			m_bSelectTitle;

	HFONT			m_hTitleLan;			// landscape title

	DWORD			m_dwTickSaveContent;
};
