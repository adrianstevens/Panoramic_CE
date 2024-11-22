#pragma once
#include "DlgBase.h"
#include "IssImageSliced.h"
#include "ObjStateAndNotifyMgr.h"
#include "IssAreaCode.h"
#include "PoomContacts.h"
#include "DlgPostCall.h"
#include "CallManager.h"
#include "GeneralOptions.h"

enum EnumCallState
{
    CSTATE_Disconnected,
    CSTATE_Ringing,
    CSTATE_Answering,
    CSTATE_Connected,
    CSTATE_Dialing,
    CSTATE_OnHold,
    CSTATE_Unknown,
};


class CDlgCallScreen : public CDlgBase
{
public:
    CDlgCallScreen(void);
    ~CDlgCallScreen(void);

    BOOL        Show(BOOL bAgressive = FALSE);
    BOOL        Hide();

    BOOL        CreateWin(HWND hWndParent);

//    BOOL        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL        OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL        OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL        OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL        OnMouseMove(HWND hWnd, POINT& pt);
    BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);

    HRESULT     LoadImages(HWND hWnd, HINSTANCE hInst);

    EnumCallState   GetCallState(){return m_eCallState;};

    BOOL        OnAnswer();
    BOOL        OnIgnore();

private:
    void        LoadRegistry();
    void        SaveRegistry();

    BOOL        DrawText(CIssGDIEx& gdi, RECT& rcClip);
    BOOL        DrawBackground(CIssGDIEx& gdi, RECT& rcClip);
    BOOL        DrawButtons(CIssGDIEx& gdi, RECT& rcClip);
    BOOL        DrawGlobe(CIssGDIEx& gdi, RECT& rcClip);
    BOOL        DrawPicture(CIssGDIEx& gdi, RECT& rcClip);
    void        DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void        OnMenuLeft();
    void        OnMenuRight();
    BOOL        OnUnlock();
    void        KillAllTimers();

    void        SetTime();
    void        SetCallerName();
    void        SetCallerNumber();
    void        SetCarrier();
    void        SetCallType();
    void        SetCallerID();//oid

    void        OnIncomingCall();
    void        OnOutgoingCall();
    void        OnStopRinging();
    void        OnCallConnected();
    void        OnCallDisconnected();
    void        OnIncomingNameChange();
    void        OnIncomingOID();

    void        OnBattery();
    void        OnSignal();

    void        HandleAreaCode();

    BOOL        UpdateContact();

    void        StartGlobeAnimation();

    BOOL        MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces);

    RECT            GetSliderRect();

    CIssImageSliced& GetSliderImg();
    CIssImageSliced& GetSliderTrack();

    void            DrawSliderKnob(CIssGDIEx& gdi, RECT& rc);
    void            DrawSliderArrow(CIssGDIEx& gdi);
    void	        DrawSlider(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);

    void            AnimateSlider(POINT pt);
    BOOL            OnSliderButtonUp(POINT pt);
    BOOL            OnSliderButtonDown(POINT pt);

    void            CheckCallRules(BOOL bWait = FALSE);

    void            UnlockScreen();
    BOOL            UseCheekGuard();
    BOOL            UseIncoming();
    BOOL            UsePostCall();

    BOOL            LaunchPost();

    void            SetCallDefaults();


    static void     ResultCallBack(DWORD dwCode,
                                    HRESULT hrCmdID,
                                    const void *lpData,
                                    DWORD cbData,
                                    DWORD dwParam);

    static void     NotifyCallBack(DWORD dwCode,
                                    const void *lpData,
                                    DWORD cbData,
                                    DWORD dwParam);




private:
    CIssAreaCode		m_oArea;		// area codes and time zones
    CCallManager        m_oCallMan;
    CDlgPostCall        m_dlgPost;

    TypeContact         m_sContact;

    CIssImageSliced     m_imgLocation;
    CIssImageSliced     m_imgCallBack;
    CIssImageSliced     m_imgBorder;
    CIssImageSliced     m_imgMask;
    CIssImageSliced     m_imgBtn;
    CIssImageSliced     m_imgBtnGreen;
    CIssImageSliced     m_imgBtnRed;
    CIssImageSliced     m_imgSlider;            // Personal/Business slider
    CIssImageSliced     m_imgSliderTrack;       // Personal/Business slider track 

    SYSTEMTIME          m_sCallStartTime;

    CIssGDIEx           m_gdiGlobe;
    CIssGDIEx           m_gdiPicture;
    CIssGDIEx           m_gdiIcons;
    CIssGDIEx		    m_gdiSliderBg;	        // gdi to draw the slider bg to
    CIssGDIEx           m_gdiImgArrowArray;     // arrows for slider
    CIssGDIEx           m_gdiBatteryIndicator;
    CIssGDIEx           m_gdiSignalIndicator;
    CIssGDIEx           m_gdiIndicatorIcons;    //top bar icons
    CIssGDIEx           m_gdiDefaultPic;
    CIssGDIEx           m_gdiReflection;

    HFONT               m_hFontStatus;
    HFONT               m_hFontText;
    HFONT               m_hFontLocation;
    HFONT               m_hFontButtons;

    TCHAR               m_szSliderText[STRING_LARGE];
    TCHAR               m_szName[STRING_MAX];
    TCHAR               m_szLocation[STRING_MAX];
    TCHAR               m_szTime[STRING_LARGE];
    TCHAR               m_szPhoneNum[STRING_LARGE];
    TCHAR               m_szCarrier[STRING_LARGE];
    TCHAR               m_szPhoneType[STRING_SMALL];

    long                m_lOid;

    RECT                m_rcPicture;
    RECT                m_rcStatus;
    RECT                m_rcTime;
    RECT                m_rcCallInfo;
    RECT                m_rcName;
    RECT                m_rcPhoneNum;
    RECT                m_rcGlobe;
    RECT                m_rcYellow;
    RECT                m_rcLocation;
    RECT                m_rcLocalTime;
    RECT                m_rcAnswer;
    RECT                m_rcIgnore;
    RECT                m_rcSlider;

    RECT                m_rcIncoming;
    
    RECT                m_rcIndicators;
    RECT                m_rcBatt;
    RECT                m_rcBattMeter;
    RECT                m_rcSignal;
    RECT                m_rcSignalMeter;
    RECT                m_rcCarrier;
    RECT                m_rcRoaming;    

    int                 m_iCallTimer;
    int                 m_iGlobeFrame; //testing for now
    int                 m_iBtnSel;  //button selector index for the ol d-pad
    int                 m_iSliderPos;
    int                 m_iSliderStartX;
    int                 m_iBattLevel;
    int                 m_iSignalLevel;
    int				    m_iRingCount;

    EnumCallState       m_eCallState;
    EnumAllowCall       m_eAllow;
    EnumCallType        m_eCallType;
    EnumLookupType      m_eLookup;
            
    BOOL                m_bDrawReflection;
    BOOL                m_bDrawMenu;
    BOOL                m_bSliderDrag;
    BOOL                m_bUseCheekGuard;
    BOOL                m_bScreenLocked;

    //hack variables so we don't start the ringtones twice
    BOOL                m_bRingToneStarted;
    EnumAllowCall       m_eLastAllow;
    DWORD               m_dwLastRingTone;

};
