#pragma once
#include "isswnd.h"
#include "IssGDI.h"
#include "HexGame.h"
#include "IssBeveledEdge.h"
#include "IssMenuHandler.h"
#include "IssSoundFX.h"
#include "IssImgFont.h"
#include "IssSprite.h"
#include "IssHardwareKeys.h"

#define REG_KEY			_T("SOFTWARE\\Pano\\HexeGems")

#define ANI_NO_RENDER	999999
#define ANI_ORB_WAIT	7

enum EnumColor
{
	COLOR_Color1,
	COLOR_Color2,
	COLOR_Outline,
};

enum EnumBGColor
{
	BGCOLOR_Grad1,
	BGCOLOR_Grad2,
	BGCOLOR_Hex,
	BGCOLOR_Grid,
};

enum EnumHexType
{
	HEXTYPE_Normal,
	HEXTYPE_Med,
	HEXTYPE_Small,
	HEXTYPE_Ghost,
};

enum EnumSoundEffects
{
	EFFECT_Down,
	EFFECT_GameOver,
	EFFECT_LevelUp,
	EFFECT_LineClear,
	EFFECT_LineClearMulti,
	EFFECT_Move,
	EFFECT_NewHiScore,
	EFFECT_Swtich,
	EFFECT_Explode,
};

enum EnumImgItems
{
	II_Star,
	II_Switch,
	II_Blank,
	II_Orb,
	II_Count,
};

struct TypeColorInfo
{
	COLORREF	crBackground;
	COLORREF	crAlpha1;		//used for the gradient overlay alpha mask
	COLORREF	crAlpha2;
	//now the color values and locations
	int			iPcnt1;	
	int			iPcnt2;	
	COLORREF	crColor1;
	COLORREF	crColor2;
	COLORREF	crColor3;
	COLORREF	crColor4;
};



class CDlgHextris:public CIssWnd
{
public:
	CDlgHextris(void);
	~CDlgHextris(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnMenuLeft();
	BOOL	OnMenuRight();
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	DrawHexagon(HDC hdc, POINT ptGridLocation, COLORREF rgbOutline, COLORREF rgbColor, int iSize, BOOL bDrawHiLite = FALSE);
	BOOL	DrawHexagon(HDC hdc, POINT ptLocation, int iColorIndex, EnumHexType eHexType);
	BOOL	DrawHexagon(CIssGDI &gdiDest, POINT ptLocation, int iColorIndex, EnumHexType EnumHexType);//using our blit function
	BOOL	DrawOrb(CIssGDI &gdiDest, POINT ptLocation, int x, int y);//we'll be animating
	BOOL	DrawBackground(HDC hdc, RECT rcClient);

	BOOL	DrawFloatingMsg(CIssGDI &gdiDest, RECT rcClient);

	BOOL	DrawCurrentPiece(CIssGDI &gdiDest, RECT rcClient);	
	BOOL	DrawGameBoard(CIssGDI &gdiDest, RECT rcClient);
	BOOL	DrawOrbs(CIssGDI &gdiDest, RECT rcClient); //so the orbs on the game board animate
	BOOL	DrawText(CIssGDI &gdiDest, RECT rcClient, BOOL bDrawStatic);
	BOOL	DrawPiecePreview(CIssGDI &gdiDest, RECT rcClient);
	BOOL	DrawLevelProgress(CIssGDI &gdiDest, RECT rcClient);

	BOOL	DrawGhost(HDC hdc, RECT rcClient);

	BOOL	ShowSplashScreen();
	BOOL	ShowMainMenu();
	BOOL	ShowMenu();

private:	// Functions
	
	void	NewGame();

	void	AnimateLineCleared();
	void	AnimateLevelChangeChallenge();
	void	AnimateEndGame();
	void	AnimateFourOrbs();
	void	AnimateDoubleOrbs();

	void	Pause(BOOL bPauseSound = TRUE);
	void	Resume();

	void	PlaySounds(EnumSoundEffects eSound);

	BOOL 	CreateGDIHex(HDC hdc, CIssGDI& gdiHex, int iWidth, int iHeight, int iHexSize, UINT res, CIssGDI* gdiGiven = NULL);
	COLORREF	GetBackgroundColor(EnumBGColor eColor);

	BOOL	ShouldAbort();

	BOOL	HackAnimation(); //press A to activate
	void	ReloadBackground(HWND hWndSplash = NULL, int iMaxPercent = 100);
	void	ReloadHexagons(HWND hWndSplash = NULL, int iMaxPercent = 100);
	void	ReloadFonts();
	void	ReloadSounds();
	BOOL	LoadImgArray(CIssGDI &oGDIDest, UINT res, int &iNumFrames, int iX, int iY);
	BOOL	LoadImgArrayAlpha(CIssGDI &oGDIDest, UINT res, int iNumFrames, int iX, int iY);

	void	ResetAniArray();//reset all of the values in our animation timing array

	int		GetDistance(POINT pt1, POINT pt2);

	int		SetAniArrayLineClear();//NO ORBS
	int		SetAniArrayOrbLineClear();//returns the max animation offset 
	int		SetAniArrayDoubleOrb();//returns the max animation offset 
	void	StartAddScore(int iScore);
	void	StartAddMessage(TCHAR* szText);//make sure its upper case

	void	ResetFPS();
	BOOL	CreateLevelGradients(CIssGDI &gdiDest, TypeColorInfo& sColorInfo);
	void	DrawAlphaImages(CIssGDI& gdiDest1, CIssGDI& gdiDest2, CIssGDI& gdiAlphaChannel, RECT rcDraw);
	



private:	// Variables
	CIssMenuHandler		m_oMenu;
	CIssSoundFX			m_oSoundFX;
	CIssHardwareKeys	m_oKeys;				// our hardware key override
	CIssGDI				m_gdiMem;
	CIssGDI				m_gdiBackground;
	CIssGDI				m_gdiTempBack;			// background with pieces drawn 
	CIssGDI				m_gdiHex;				// full array of gems
	CIssGDI				m_gdiHexMed;			// medium sized array of gems
	CIssGDI				m_gdiHexSmall;			// small sized array of gems
	CIssGDI				m_gdiHexGhost;
	CIssGDI				m_gdiAnimation;			// test the animation yo
	CIssGDI				m_gdiExplosion;
	CIssGDI				m_gdiExplosionAlpha;
	CIssGDI				m_gdiFireball;
	CIssGDI				m_gdiFireballAlpha;
	CIssGDI				m_gdiImages;
	CIssGDI				m_gdiImagesAlpha;
	CIssGDI				m_gdiPulse;
	CIssGDI				m_gdiPulseAlpha;
	CIssGDI				m_gdiLevelBackground;
	CIssGDI				m_gdiLevel;
	CIssImgFont			m_imgFont;
	CIssSprite			m_sprtOrb;

	CHexGame			m_oGame;

	HFONT				m_hFontText;
	
	RECT				m_rcScore;
	RECT				m_rcLevel;
	RECT				m_rcLevel2;
	RECT				m_rcBonus;
	RECT				m_rcN;
	RECT				m_rcE;
	RECT				m_rcX;
	RECT				m_rcT;

	RECT				m_rcLevelProg;



	RECT				m_rcOrbs[MAX_SAVED_GEMS];//maybe this should be per orb ...
	RECT				m_rcLines;
	RECT				m_rcPreview1;			// location of the preview section
	RECT				m_rcPreview2;
	RECT				m_rcSaved;				// location of the saved piece section
	RECT				m_rcGS3;				// location of the third game screen section
	RECT				m_rcFPS;//if we ever want to display it
	RECT				m_rcTime;
	RECT				m_rcMenu;//for the ol menu button

	RECT				m_rcBackGround; //rect for the bevel class

	int					m_iHexSize;			//since it'll vary ... length of one side in pixels
	int					m_iHexSizeMed;
	int					m_iHexSizeSmall;
	SIZE				m_sizeHex;			//actual full hexagon in pixels
	SIZE				m_sizeHexMed;
	SIZE				m_sizeHexSmall;

	int					m_iHexSizeCos30;
	int					m_iHexSizeMedCos30;
	int					m_iHexSizeSmallCos30;

	int					m_iNumExplosionFrames;
	int					m_iCurrentExplosionFrame;
	int					m_iNumFireballFrames;
	int					m_iNumPulseFrames;

	//from the top left corner ... so NOT absolute left (it'll be indented half the hex size
	POINT				m_ptGameField[GAME_WIDTH][MAX_GAME_HEIGHT];//we'll save em all when we draw the background

	//these arrays are set up so we don't have to calc positions every time
	POINT				m_ptPreview1[4][5];//next piece
	POINT				m_ptPreview2[4][5];//piece after next
	POINT				m_ptSaved[4][5];//banked baby

	int					m_iAniArray[GAME_WIDTH][MAX_GAME_HEIGHT];

	//Controls
	WORD				m_wLeft;
	WORD				m_wRight;
	WORD				m_wDown;
	WORD				m_wDrop;
	WORD				m_wRotate;
	WORD				m_wRotateAlt;
	WORD				m_wSwitch;

	DWORD				m_dwStartTime;
	DWORD				m_dwFrameCount;
	DWORD				m_dwTickCount;

	DWORD				m_dwStartScoreAddition;
	DWORD				m_dwStartFloatMsg;
									

	BOOL				m_bShowSplashScreen;
	BOOL				m_bIsBackgroundDirty; //its very dirty

	TCHAR				m_szFloatMsg[STRING_LARGE];
	BOOL				m_bDrawFloat;


};