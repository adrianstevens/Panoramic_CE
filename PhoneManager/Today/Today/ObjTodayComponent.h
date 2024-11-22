#pragma once

#include "IssString.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"
#include "IssImgFont.h"
#include "DlgContactDetails.h"
#include "GuiBackground.h"
#include "ObjPhoneProfile.h"
#include "ObjWirelessDevices.h"
#include "ObjAlarms.h"
#include "Regext.h"
#include "DlgChoosePrograms.h"

#define HEIGHT_Text                 (GetSystemMetrics(SM_CXICON)*13/32)
#define IDMENU_SmartArrange			100
#define IDMENU_LockItems			101
#define IDMENU_AddItem				102
#define IDMENU_RemoveItem			103
#define IDMENU_EditItems			104
#define IDMENU_RestoreDefaults      105
#define IDMENU_PhoneOptions         106
#define IDMENU_SetVoicemail         107
#define IDMENU_Profile			    200

#define COMP_HEIGHT_Provider        1
#define COMP_WIDTH_Provider         8
#define COMP_HEIGHT_TimeLarge       5
#define COMP_HEIGHT_TimeSmall       3
#define COMP_WIDTH_Time             15
#define COMP_HEIGHT_Message         3
#define COMP_WIDTH_Message          3
#define COMP_HEIGHT_Contact         3
#define COMP_WIDTH_Contact          3


enum ComponentType
{
	CompType_Sys_Profile = 0,
	CompType_Info_Provider,
	CompType_Info_TimeLarge,
	CompType_Info_TimeSmall,
	CompType_Message_Email,
	CompType_Message_MissedCall,
	CompType_Message_SMS,
	CompType_Message_VoiceMail,
    CompType_Contact,
    //CompType_Message_Wifi,
    //CompType_Message_Bluetooth,
    CompType_Launch_Program,
    CompType_Launch_Setting,
	NumComponentTypes
};

static TCHAR* GetComponentName(ComponentType eType)
{
	switch(eType)
	{
	case CompType_Sys_Profile:			return _T("Phone Profile");
	case CompType_Info_Provider:		return _T("Provider");
	case CompType_Info_TimeLarge:		return _T("Date/Time Large");
	case CompType_Info_TimeSmall:		return _T("Date/Time Small");
	case CompType_Message_Email:		return _T("Email");
	case CompType_Message_MissedCall:	return _T("Missed Calls");
	case CompType_Message_SMS:			return _T("SMS");
	case CompType_Message_VoiceMail:	return _T("Voicemail");
    //case CompType_Message_Wifi:	        return _T("Wi-Fi");
    //case CompType_Message_Bluetooth:	return _T("Bluetooth");
    case CompType_Launch_Program:	    return _T("Add Program...");
    case CompType_Launch_Setting:	    return _T("Add Setting...");
	}
	return _T("");
}

class CObjTodayComponentBase
{
public:
	CObjTodayComponentBase(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst, long lID = 0);
	virtual ~CObjTodayComponentBase(void);

	static void		DeleteAllContent();
	virtual BOOL	Draw(CIssGDIEx& gdi, 
						RECT rcClient, 
						COLORREF crText, 
						COLORREF crBg, 
						BOOL bHasFocus = FALSE,
						BOOL bSelected = FALSE,
						BOOL bZeroBase = FALSE,
                        BOOL bAddAlpha = FALSE);
    virtual void    DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase = FALSE){};
	RECT			GetFrame();
	void			SetLocation(RECT& rc);
	void			SetLocationUndefined();
	BOOL			IsLocationUndefined()			{ return GetFrame().right == 0;}
	void			SetMatrixLocation(RECT& rc);
	RECT			GetMatrixRect()					{ return m_rcMatrixPos;	}
	ComponentType	GetType()						{ return m_eCompType;	}
	static int		GetBlockSize()					{ return m_iBlockSize;	}
	long			GetID()							{ return m_lID;			}

	virtual void	ExecuteItem(HWND hWnd, POINT pt){}
    virtual void	Refresh(HWND hWnd);
	virtual TCHAR*	GetName()						{ return GetComponentName(m_eCompType);	}

protected:			// variables
	static int		    m_iBlockSize;
	static const int    m_iFrameIndent;
    static HWND         m_hWnd;
    static HINSTANCE    m_hInst;
    static CIssImageSliced* m_imgSelector;
	ComponentType	    m_eCompType;
    CIssString*		    m_oStr;

private:			// variables
	long			m_lID;
	RECT			m_rcMatrixPos;	// based on block size
    
};

////////////////////////////////////////////////////////////////////////
class CObjTodayProfile : public CObjTodayComponentBase
{
public:
	CObjTodayProfile(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst);
	~CObjTodayProfile();
	virtual BOOL	Draw(CIssGDIEx& gdi, 
		                RECT rcClient, 
		                COLORREF crText, 
		                COLORREF crHighlight, 
		                BOOL bHasFocus,
		                BOOL bSelected,
                        BOOL bZeroBase,
                        BOOL bAddAlpha = FALSE);

    void	        Refresh(HWND hWnd);
    void	        ExecuteItem(HWND hWnd, POINT pt);
    int             GetMenuItemHeight(){return m_gdiProfile.GetHeight();};
    void            DrawMenuItem(CIssGDIEx& gdi, RECT rc, EnumPhoneProfile eProfile, COLORREF crText);

private:
	CIssGDIEx			m_gdiProfile;
	CObjPhoneProfile	m_objProfile;
};

//////////////////////////////////////////////////////////////////////

class CObjTodayInfoComponent : public CObjTodayComponentBase
{
public:
	CObjTodayInfoComponent(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst);
	~CObjTodayInfoComponent();
	virtual BOOL	Draw(CIssGDIEx& gdi, 
		                RECT rcClient, 
		                COLORREF crText, 
		                COLORREF crBg, 
		                BOOL bHasFocus = FALSE,
		                BOOL bSelected = FALSE,
                        BOOL bZeroBase = FALSE,
                        BOOL bAddAlpha = FALSE);
    void            DrawText(CIssGDIEx& gdi, HFONT hFontText,BOOL bZeroBase = FALSE);
    void            DrawTextTime(CIssGDIEx& gdi, HFONT hFontText, RECT rcLocation);
	void            DrawTimeLarge(CIssGDIEx& gdi, RECT rcLocation, BOOL bAddAlpha);
	void            DrawTimeSmall(CIssGDIEx& gdi, RECT rcLocation, BOOL bAddAlpha);
	void			ExecuteItem(HWND hWnd, POINT pt);
    void            InitImages();

    void            Refresh(HWND hWnd);

private:
    CObjAlarms      m_oAlarms;
    CIssImageSliced m_imgBK;
    CIssImgFont     m_imgFont;
    CIssGDIEx       m_gdiAlarm;
    HFONT           m_hFontAlarms;
    //CIssGDIEx       m_gdiShadow;
};

//////////////////////////////////////////////////////////////////////

class CObjTodayMessageComponent : public CObjTodayComponentBase
{
public:
	CObjTodayMessageComponent(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdi, CGuiBackground* guiBackground);
	~CObjTodayMessageComponent();
	static void	DeleteNotify();
	virtual BOOL	Draw(CIssGDIEx& gdi, 
		                RECT rcClient, 
		                COLORREF crText, 
		                COLORREF crBg, 
		                BOOL bHasFocus = FALSE,
		                BOOL bSelected = FALSE,
                        BOOL bZeroBase = FALSE,
                        BOOL bAddAlpha = FALSE);
	virtual	void	DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase = FALSE );
	void			InitImages();
	void	        Refresh(HWND hWnd);
	int				GetMessageCount(ComponentType eType);
	TCHAR*			GetMessageLabel(ComponentType eType);
	void			ExecuteItem(HWND hWnd, POINT pt);

private:
	DWORD		m_dwCount;
	CIssGDIEx	m_gdiImage;
	static CIssImageSliced* m_imgNotify;
    CIssGDIEx*  m_gdiMem;
    CGuiBackground* m_guiBackground;
    //static CObjWirelessDevices m_objDevices;
    HREGNOTIFY  m_hNotify;
};


/////////////////////////////////////////////////////////////////////////////
class CObjTodayContactComponent : public CObjTodayComponentBase
{
public:
	CObjTodayContactComponent(TCHAR* szName, CDlgContactDetails* dlg, CGuiBackground* gui, CIssGDIEx* gdi, RECT rcPosition, HWND hWndParent, HINSTANCE hInst, long lOid);
	~CObjTodayContactComponent();
	virtual BOOL	Draw(CIssGDIEx& gdi, 
		                    RECT rcClient, 
		                    COLORREF crText, 
		                    COLORREF crBg, 
		                    BOOL bHasFocus = FALSE,
		                    BOOL bSelected = FALSE,
                            BOOL bZeroBase = FALSE,
                            BOOL bAddAlpha = FALSE);
	virtual	void	DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase = FALSE );
	void			ExecuteItem(HWND hWnd, POINT pt);
    void            Refresh(HWND hWnd);
	TCHAR*			GetName()				{ return m_szName;	}

private:		// functions
	void			OnContextMenu(HWND hWnd, POINT pt);
    void            InitImages(RECT& rcFrame);
    void            DrawDefaultImage(CIssGDIEx& gdi, RECT& rcFrame, BOOL bAddAlpha);
    void            DrawImage(CIssGDIEx& gdi, RECT& rcFrame, BOOL bAddAlpha);

private:		// variables
	TCHAR*			m_szName;
    CIssGDIEx       m_gdiPic;
    BOOL            m_bCheckedImage;
    CDlgContactDetails* m_dlgContactDetails;
    CGuiBackground*     m_guiBackground;
    CIssGDIEx*          m_gdiMem;
    BOOL            m_bDefaultImage;
	

};

/////////////////////////////////////////////////////////////////////////////
class CObjTodayLauncherComponent : public CObjTodayComponentBase
{
public:
    CObjTodayLauncherComponent( RECT rcPosition, 
                                HWND hWndParent, 
                                HINSTANCE hInst, 
                                TCHAR* szPath, 
                                TCHAR* szFriendlyName,
                                TCHAR* szRegName,
                                EnumLaunchType eType);
    ~CObjTodayLauncherComponent();
    virtual BOOL	Draw(CIssGDIEx& gdi, 
        RECT rcClient, 
        COLORREF crText, 
        COLORREF crBg, 
        BOOL bHasFocus = FALSE,
        BOOL bSelected = FALSE,
        BOOL bZeroBase = FALSE,
        BOOL bAddAlpha = FALSE);
    void			ExecuteItem(HWND hWnd, POINT pt);
    void            Refresh(HWND hWnd);
    TCHAR*          GetPath(){return m_szPath;};
    TCHAR*          GetFriendlyName(){return m_szFriendlyName;};
    TCHAR*          GetRegName(){return m_szRegName;};

private:		// functions
    void            InitImages(RECT& rcFrame);
    void            DrawImage(CIssGDIEx& gdi, RECT& rcFrame, BOOL bAddAlpha);

private:		// variables
    CIssGDIEx       m_gdiPic;
    TCHAR*          m_szPath;
    TCHAR*          m_szFriendlyName;
    TCHAR*          m_szRegName;

};