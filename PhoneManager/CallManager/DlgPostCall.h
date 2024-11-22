#pragma once
#include "dlgbase.h"
#include "IssImageSliced.h"
#include "IssKineticList.h"
#include "PoomContacts.h"
#include "ObjBlocklist.h"
#include "CallManager.h"
#include "IssWndTouchMenu.h"
#include "DlgMoreInfo.h"

enum EnumPostCall
{
    POSTCALL_SMS,  
    POSTCALL_AddToContacts,
    POSTCALL_CallBack,
    POSTCALL_Count,
};

enum EnumSMSReply
{
    SMSR_BusyCall,
    SMSR_BusyYouCall,
    SMSR_Meeting,
    SMSR_Car,
    SMSR_Custom,
    SMSR_Count,
};

enum EnumCallType //we can go further using the privacy/block enum
{
    CTYPE_IncomingAnswered,
    CTYPE_IncomingMissed,
    CTYPE_IncomingIngored,
    CTYPE_OutgoingAnswered,
    CTYPE_OutgoingDropped,
    CTYPE_NoCall,
};


struct PostCallType
{
    TCHAR* szName;
    TCHAR* szPhoneNum;
    TCHAR* szType;
    TCHAR* szLocation;
    LONG lOid;
    SYSTEMTIME sStartTime;
    int iDurration;
    CObjBlocklist* objBlockList;
    EnumCallType eCallType;
    EnumAllowCall eAllow;
    //prolly a call type or something ....


};

class CDlgPostCall : public CDlgBase
{
public:
    CDlgPostCall(void);
    ~CDlgPostCall(void);

    BOOL            Show(BOOL bAgressive = FALSE);
    BOOL            Hide();


    BOOL            CreateWin(HWND hWndParent);

    BOOL            Initialize(PostCallType* sType);

    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnMouseMove(HWND hWnd, POINT& pt);
    BOOL            OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

    HRESULT         LoadImages(HWND hWnd, HINSTANCE hInst);

private:
    BOOL            DrawText(CIssGDIEx& gdi, RECT& rcClip);
    BOOL            DrawBackground(CIssGDIEx& gdi, RECT& rcClip);
    BOOL            DrawButtons(CIssGDIEx& gdi, RECT& rcClip);
    BOOL            DrawPicture(CIssGDIEx& gdi, RECT& rcClip);
    BOOL            DrawIndicators(CIssGDIEx& gdi, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);

    void            OnMenuLeft();

    //kinetic
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumPostCall* eOption);
    void                DrawButton(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc);
    void                PopulateContent(int iSelectedIndex);


    void            OnBattery();
    void            OnSignal();

    void            AnimateSlider(POINT pt);
    BOOL            OnSliderButtonUp(POINT pt);
    BOOL            OnSliderButtonDown(POINT pt);
    CIssImageSliced& GetSliderImg();
    CIssImageSliced& GetSliderTrack();
    void            DrawSliderArrow(CIssGDIEx& gdi);
    void            DrawSliderKnob(CIssGDIEx& gdi, RECT& rc);
    RECT            GetSliderRect();
    void            DrawSlider(CIssGDIEx& gdi, RECT& rc, RECT& rcClip);

    void            SetCallerName();
    void            SetCallerNumber();
    void            SetCarrier();
    void            SetCallType();
    void            OnColorSchemeChange(HWND hWndParent, HINSTANCE hInst);

    BOOL            UpdateContact();

    static void     DeleteMyItem(LPVOID lpItem);

    BOOL            ShowSMSList();
    BOOL            ShowCallBack();
    BOOL            ShowBlocked();


    BOOL            OnBlock();
    BOOL            OnUnblock();
    BOOL            OnCallBack();
    BOOL            OnAddToContacts();
    BOOL            OnSMS();
    BOOL            OnMoreInfo();

    BOOL            SendSMSMessage(TCHAR* szMsg);

    BOOL            FormatStrictNum(TCHAR* szSource, TCHAR* szDest, BOOL bRemove1 = FALSE);

    void            SetTimeOut();

private:
    CIssKineticList     m_oMenu;
    TypeContact         m_sContact;
    CObjBlocklist*      m_objBlockList;
    CIssWndTouchMenu    m_wndMenu;
    CDlgMoreInfo        m_dlgMore;

    EnumCallType        m_eCallType;
    EnumAllowCall       m_eAllow;

    CIssImageSliced     m_imgInfoBtn;
    CIssImageSliced     m_imgBtn;
    CIssImageSliced     m_imgBtnGrn;    
    CIssImageSliced     m_imgCallBack;
    CIssImageSliced     m_imgMask;
    CIssImageSliced     m_imgBorder;
    CIssImageSliced     m_imgSlider;            // Personal/Business slider
    CIssImageSliced     m_imgSliderTrack;       // Personal/Business slider track 

    CIssGDIEx           m_gdiPicture;
    CIssGDIEx           m_gdiBatteryIndicator;
    CIssGDIEx           m_gdiSignalIndicator;
    CIssGDIEx           m_gdiIndicatorIcons;    //top bar icons
    CIssGDIEx           m_gdiImgArrowArray;
    CIssGDIEx           m_gdiMenuIcons;
    CIssGDIEx           m_gdiDefaultPic;

    SYSTEMTIME          m_sCallStartTime;

    HFONT               m_hFontStatus;
    HFONT               m_hFontText;
    HFONT               m_hFontButtons;
    HFONT               m_hLabelFont;
    HFONT               m_hFontLocation;

    TCHAR               m_szSliderText[STRING_LARGE];
    TCHAR               m_szName[STRING_MAX];
    TCHAR               m_szLocation[STRING_MAX];
    TCHAR               m_szTime[STRING_LARGE];
    TCHAR               m_szPhoneNum[STRING_LARGE];
    TCHAR               m_szPhoneType[STRING_SMALL];
    TCHAR               m_szCarrier[STRING_LARGE];
    TCHAR               m_szCall[STRING_NORMAL];
    LONG                m_lOid;//to load the picture

    RECT                m_rcIndicators;
    RECT                m_rcSlider;
    RECT                m_rcName;
    RECT                m_rcPhoneNum;
    RECT                m_rcPicture;
    RECT                m_rcStatus;
    RECT                m_rcTime;
    RECT                m_rcShadow;
    RECT                m_rcInfoBtn;
    RECT                m_rcGreenBar;

    BOOL                m_bSliderDrag;
    BOOL                m_bShowSMSList;//so we're not asking poom every time
    BOOL                m_bBlocked;

    int                 m_iBattLevel;
    int                 m_iSignalLevel;
    int                 m_iSliderPos;
    int                 m_iSliderStartX;
    int                 m_iCallDurration;
    int                 m_iForgroundCount;
};
