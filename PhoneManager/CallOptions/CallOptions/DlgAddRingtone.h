#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssDynBtn.h"
#include <soundfile.h>
#include "ObjRingtoneRuleList.h"
#include "DlgBaseOptions.h"

enum EnumAddRingtone
{
    AR_Rule,
    AR_RingType,
    AR_RingTone,
};


class CDlgAddRingtone : public CDlgBaseOptions
{
public:
	CDlgAddRingtone(CObjRingtoneRuleList* objRuleList);
	~CDlgAddRingtone(void);
	
	void			SetEditItem(TypeRingtoneRule* sItem);
    TypeRingtoneRule* GetRule(){return &m_sRule;};
    static void     GetRintTypeText(EnumRingType eRing, TCHAR* szText);

protected:
	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);

private:
    void            RefreshList();
	void			CreateButtons(int iTop);
	void			LoadList();
	void			AddRingtone();
	void			UpdateRingtone();
	void			UpdateDefaultRingtone();
	void			AddContactRingtone();
	void			AddCategoryRingtone();
	BOOL			StoreRingtoneRule(TypeRingtoneRule* pRule);
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL			DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumAddRingtone* eRing);
    static void     DeleteMyItem(LPVOID lpItem);
    void            GetItemTitle(EnumAddRingtone eRing, TCHAR* szText);
    void            GetRuleText(EnumRingtoneRuleType eRule, TCHAR* szText, BOOL bPopupMenu = FALSE);
    
    void            LaunchPopupMenu();
    void            SetSelection();
	void			OnPlay();
	void			OnStop();

private:
	HFONT				m_hBtnFont;
    HFONT               m_hFontBtnText;

	CIssDynBtn			m_btnPlay;
	CIssDynBtn			m_btnStop;

	SNDFILEINFO*		m_sFileList;
    int                 m_iFileCount;

	CObjPhoneProfile	m_objPhoneProfile;
	CObjRingtoneEngine	m_objRingtoneEngine;
	CObjRingtoneRuleList* m_objRuleList;
	TypeRingtoneRule	m_sRule;
	BOOL				m_bEditMode;
};
