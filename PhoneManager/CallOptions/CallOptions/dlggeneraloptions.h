#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "GeneralOptions.h"
#include "ObjPhoneProfile.h"
#include "DlgBaseOptions.h"
#include "IssAreaCode.h"

enum EnumGeneralOptions
{
    GO_ShowIncomingCall,
    GO_ShowCheekGuard,
    GO_ShowPostCallScreen,
    GO_LookupMode,
    GO_HandleRingtones,
    GO_PhoneProfile,
    GO_All,
};

class CDlgGeneralOptions : public CDlgBaseOptions
{
public:
	CDlgGeneralOptions();
	~CDlgGeneralOptions(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:    // functions
    void            RefreshList();
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    static void     DeleteMyItem(LPVOID lpItem);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL			DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumGeneralOptions* eOption);
    void            ReadRegValues();
    void            SaveRegValues();
    void            GetItemTitle(EnumGeneralOptions eOption, TCHAR* szTitle);
    void            GetPhoneProfileText(EnumPhoneProfile ePhoneProfile, TCHAR* szText);
    void            GetLookupMode(EnumLookupType eNumLookup, TCHAR* szText);
    void            LaunchPopupMenu();
    void            SetSelection();

private:
	CObjPhoneProfile m_objPhoneProfile;
    DWORD            m_dwValues;
    EnumLookupType   m_eLookupMode;
    HFONT            m_hFontBtnText;
};
