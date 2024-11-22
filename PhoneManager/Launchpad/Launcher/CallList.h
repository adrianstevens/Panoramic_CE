#pragma once
#include "guibase.h"
//#include "ListScroller.h"
#include "IssKineticList.h"
#include "CallLog.h"
#include "PoomContacts.h"
#include "DlgOptions.h"


class CCallList : public CGuiBase, CIssKineticList
{
public:


	CCallList(void);
    CCallList(CGuiBase* guiOwner);
	~CCallList(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBackground, CGuiBackground* guiBackground, TypeOptions* sOptions);

	HWND			GetOwnerWindow()					{ return m_hWndParent; }
	int				GetEndOfListYPos();
	RECT&			GetOwnerFrame()						{ return m_rcLocation;}

	BOOL			Draw(HDC hDC, RECT& rcClient);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL			DrawBackground(HDC hdc, RECT rcClient);
	BOOL			MoveGui(RECT rcLocation);

	//void			SetShowCallType(Call_Type type);
	EnumShowCall	GetShowCallType();
	CALLLOGENTRY*	GetCallEntry(int iIndex);
	void			SetCurContact(IContact* pContact);
	TCHAR*			GetCurContactName();
	TCHAR*			GetCallTypeString();
	BOOL			GetEntryTimeString(CALLLOGENTRY& entry, TCHAR* szTime);
	BOOL			GetEntryDurationString(CALLLOGENTRY& entry, TCHAR* szDuration);
	void			ResetScrollPos();
	void			ReBuildCallList();
	BOOL			IsStopped()					{ return CIssKineticList::IsStopped(); }
	int				GetScrollPos()					{ return CIssKineticList::GetScrollPos(); }
	int				GetItemIndexAtPos(int iYpos)	{ return iYpos / m_iEntryHeight; }
    // call this if you want the owner to draw the list
    void            SetOwner(CGuiBase* guiOwner)    { m_guiOwner = guiOwner;}

private:
	void			ReDrawList();
	void			DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB);

	BOOL			DrawEntries(HDC hdc, RECT rcClient);
	BOOL			DrawLogEntry(HDC hdc, RECT rcClient, CALLLOGENTRY& entry, int iYpos);

	int				GetNumCallEntries();
	BOOL			GetCallEntry(int iIndex, CALLLOGENTRY& entry);
	BOOL			CompareContactName(CALLLOGENTRY &entry);
	BOOL			FormatContactName(TCHAR* pszName);
	void			DestroyCallList();
	BOOL			ShouldDrawScrollbar();



private:
	CIssVector<CALLLOGENTRY>	m_callItems;
	CIssGDIEx						m_gdiBackground;
	CIssGDIEx						m_gdiLogEntryBg[2];
	CIssGDIEx						m_gdiOffscreen;

	HFONT						m_hFontEntry;
	HFONT						m_hFontEntryBold;

	CCallLog					m_oCallLog;
	IContact*					m_Contact;

	int							m_iLetterHeight;
	int							m_iEntryHeight;

	//Call_Type					m_eShowCallType;

	COLORREF					m_colorBG[2];
	int							m_iBgColorIndex;

	class CScrollBar*			m_scrollbar;
    CGuiBase*                   m_guiOwner;
};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////// scrollbar class /////////////////////////////////////////

class CScrollBar
{
public:
	CScrollBar();
	~CScrollBar(){}

	BOOL			Draw(HDC hdc, RECT& rcClient, int iYPos, int iListHeight);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	float			OnMouseMove(POINT pt, RECT& rcClient, int iListHeight);

	BOOL			IsScrollActive()				{ return m_bScrollActive;    }


private:
	int				m_iLastMousePos;
	BOOL			m_bScrollActive;
	RECT			m_rcScrollbar;
};