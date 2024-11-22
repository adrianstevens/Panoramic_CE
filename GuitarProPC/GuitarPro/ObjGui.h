#pragma once

#include "IssGDIEx.h"
#include "IssString.h"
#include "IssImageSliced.h"
#include "IssWndTouchMenu.h"
//#include "HSound.h"

#define MENU_HEIGHT             64//(GetSystemMetrics(SM_CXICON))//(IsVGA()?64:32)
#define BUTTONS_HEIGHT          48//(IsVGA()?48:24)
#define NUT_HEIGHT              34//(IsVGA()?34:17)
#define STRING_WIDTH            11//(IsVGA()?11:6)

#define MENU_TEXT_COLOR         0xDDDDDD


enum EnumMenuSelect
{
    MENU_Left,
    MENU_Right,
    MENU_None,
};

enum EnumMenuIcons
{
    MICON_Chords,
    MICON_Tuner,
    MICON_Metronome,
    MICON_Menu,
    MICON_Favorites,
    MICON_Play,
    MICON_Genres,
    MICON_AddToFav,
    MICON_Count,
};

enum EnumSkin
{
    SKIN_1,
    SKIN_2,
    SKIN_3,
    SKIN_4,
    SKIN_5,
    SKIN_6,
    SKIN_7,
    SKIN_Count,
};


enum EnumFont
{
    FONT_Button,
    FONT_Menu,
    FONT_MenuSmall,
    FONT_MenuLarge,//options & about ... scaled to icon
    FONT_Fingers,
    FONT_Frets,
    FONT_MetroLabels,
    FONT_Count,


};

#define IDMENU_Left         10000
#define IDMENU_Right        10001

class CObjGui
{
public:
    CObjGui(void);
    ~CObjGui(void);

    void                Init(HWND hWndParent, HINSTANCE hInst);

    BOOL                DrawToScreen(RECT& rcClip, HDC dc);
    BOOL                DrawBackground(RECT& rcClip, CIssGDIEx* gdi = NULL);
    BOOL                DrawMenu(RECT& rcClip, CIssGDIEx* gdi = NULL, BOOL bDrawIcons = TRUE);
    BOOL                DrawTopButtons(RECT& rcClip, CIssGDIEx* gdi = NULL);
    BOOL                DrawNut(RECT& rcClip, CIssGDIEx* gdi = NULL);
    BOOL                DrawStrings(RECT& rcClip, int iFade, CIssGDIEx* gdi = NULL);

    BOOL                OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL                OnSize(HWND hWnd, RECT rcClient);

    CIssGDIEx*          GetFinger();
    CIssGDIEx*          GetFingerDark();
    CIssGDIEx*          GetFingerRed();
    CIssGDIEx*          GetFingerGrey();
    CIssGDIEx*          GetNoNote();
    CIssGDIEx*          GetGDI(BOOL bDestroy = FALSE);
    CIssGDIEx*          GetBackground(BOOL bDestroy = FALSE);
    CIssGDIEx*          GetAltBack(BOOL bDestroy = FALSE);
    CIssGDIEx*          GetLEDs();
    CIssGDIEx*          GetMenuIcons();
    CIssGDIEx*          GetMenuCheck();
    CIssGDIEx*          GetMenuArrow();
    CIssGDIEx*          GetMenu();
    CIssGDIEx*          GetStar(BOOL bGrey = FALSE);
    CIssGDIEx*          GetGlow();
    CIssImageSliced*    GetAlpha(int iW, int iH);
    CIssImageSliced*    GetImgOutline(int iW, int iH);

    HFONT               GetFont(EnumFont eFont);

    void                PlaySFX(int iIndex);
    void                PlayClick();
    void                StopSFX(){/*m_oSoundFX.StopSFX();*/};

    EnumSkin            GetSkin(){return m_eSkin;};
    void                SetSkin(EnumSkin eSkin);

    BOOL                GetPlaySounds(){return m_bPlaySounds;};
    void                SetPlaySounds(BOOL bPlaySounds);//{m_bPlaySounds = bPlaySounds;};


    int                 GetStringX(int iString);

    void                VibrateString(int iString);

    HINSTANCE           GethInst(){return m_hInst;};

private:
    void                Destroy();

    void                InitBottomBar(RECT& rcClient);
    void                InitBackground(RECT& rcClient);
    void                InitStrings();

    void                LoadSounds();

    

public:
    CIssWndTouchMenu    m_wndMenu; //for now
    RECT                m_rcBottomBar;
    
private:
//	CHSound   			m_oSoundFX;

    CIssGDIEx           m_gdiMem;
    CIssGDIEx           m_gdiBackground;
    CIssGDIEx           m_gdiAltBack; //for the options and about 
    CIssGDIEx           m_gdiMenuCheck;//to show the current item
    CIssGDIEx           m_gdiMenuUp;//the little up arrow
    CIssGDIEx           m_gdiBottomBar;
    CIssGDIEx           m_gdiNut;
    CIssGDIEx           m_gdiMenuIcons;

    CIssGDIEx           m_gdiFinger;
    CIssGDIEx           m_gdiFingerRed;
    CIssGDIEx           m_gdiFingerGrey;
    CIssGDIEx           m_gdiFingerDark;
    CIssGDIEx           m_gdiNoNote;
    CIssGDIEx           m_gdiStrings[6];

    CIssGDIEx           m_gdiStar;
    CIssGDIEx           m_gdiStarGrey;

    CIssGDIEx           m_gdiLEDs;
    CIssGDIEx           m_gdiGlow;

    CIssImageSliced     m_imgAlpha;
    CIssImageSliced     m_imgImgOutline;
    CIssImageSliced     m_imgMenu;//bottom menu
    CIssImageSliced     m_imgButtons;//for the top

    HFONT               m_hFonts[FONT_Count];

    EnumMenuSelect      m_eMenuSelect;
    EnumSkin            m_eSkin;      
    
    RECT                m_rcBackground;
    RECT                m_rcButtons;
    RECT                m_rcNut;    //white bar at the top
    RECT                m_rcStrings[6];
    RECT                m_rcStringsLeft[6];//just easier

    CIssString*         m_oStr;    

    HWND                m_hWndParent;
    HINSTANCE           m_hInst;

    BOOL                m_bPlaySounds;

    int                 m_iClickIndex;


};



static int GetSysMets(__in int nIndex)
{
#ifdef UNDER_CE
	return GetSystemMetrics(nIndex);
#else
	switch(nIndex)
	{
	case SM_CXSMICON:
		return 32;
		break;
	case SM_CXICON:
		return 64;
		break;
	case SM_CXSCREEN:
		return 480;
		break;
	case SM_CYSCREEN:
		return 640;
		break;
	default:
		return ::GetSystemMetrics(nIndex);
		break;
	}
#endif
}