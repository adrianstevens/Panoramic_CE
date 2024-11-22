#pragma once

#include "IssWnd.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "ObjItemContact.h"

class CDlgAddContact:public CIssWnd
{
public:
	CDlgAddContact(void);
	~CDlgAddContact(void);

	void			Init(CPoomContacts* poomContacts, CIssVector<CObjItemContact>* arrItems){m_poomContacts = poomContacts;m_arrItems = arrItems;};
	BOOL			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL            OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

public:
	int			m_iSelectedItem;


private:	// variables
	CPoomContacts*	m_poomContacts;
	CIssVector<CObjItemContact>* m_arrItems;
	HWND			m_hWndCmb;

	HFONT			m_hFontTitle;

	RECT			m_rcTitle;
	RECT			m_rcCombo;

	BOOL			m_bMoved;
};
