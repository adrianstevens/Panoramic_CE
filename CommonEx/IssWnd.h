/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSWnd.h 
// Abstract: Windows base class to simplify handling common windows messages  
// 
/***************************************************************************************************************/ 
#pragma once

#include "IssString.h"

#define UNHANDLED	-1

enum EnumWndType	// have we created the window as a dialog or plain window
{
	WND_Dialog,
	WND_Window,
};

class CIssWnd  
{
public:		// functions
	CIssWnd();
	virtual ~CIssWnd();
	BOOL	Destroy();
    void    SafeCloseWindow(UINT uiReturn = IDOK);
	
	BOOL	DoModal(HWND		hWndParent, 
					HINSTANCE	hInst,
					UINT		uiDlgID);
	BOOL	DoModalNoRes(HWND hWndParent, HINSTANCE hInst, DWORD dwStyle = DS_MODALFRAME | DS_SETFOREGROUND | WS_POPUP | WS_CAPTION | WS_SYSMENU);
	BOOL	DoModaless(HWND		hWndParent, 
					   HINSTANCE hInst,
					   UINT		uiDlgID);
	BOOL	Create(	 TCHAR*		szWindowName,
					 HWND		hWndParent,
					 HINSTANCE	hInst,
					 TCHAR*		szClassName	= NULL,
					 int		x			= CW_USEDEFAULT,
					 int		y			= CW_USEDEFAULT,
					 int		cx			= CW_USEDEFAULT,
					 int		cy			= CW_USEDEFAULT,
					 DWORD		dwExtStyle	= 0,
					 DWORD		dwStyle		= 0,
					 HMENU		hMenu       = NULL,
                     UINT       uiIcon      = 0);//icon resource
	BOOL	CreateChildWindow(HINSTANCE hInstance,
								HWND hWndParent,
								UINT uiResource);
	HWND	GetWnd(){return m_hWnd;};
	operator HWND(){return m_hWnd;};


	static BOOL CALLBACK ProcWnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // this is public because sometimes we pass this as a parameter

protected:	// functions
	virtual BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)							{return UNHANDLED;};
	virtual BOOL	OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual BOOL	OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)		{return UNHANDLED;};
	virtual BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return TRUE;};
	virtual BOOL	OnLButtonUp(HWND hWnd, POINT& pt)									{return UNHANDLED;};//return 0 if handled
	virtual BOOL	OnLButtonDown(HWND hWnd, POINT& pt)									{return UNHANDLED;};//return 0 if handled
	virtual BOOL	OnMouseMove(HWND hWnd, POINT& pt)									{return UNHANDLED;};
	virtual BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)					{m_hWnd=NULL;return TRUE;};
	virtual BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnSysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{return (BOOL)DefWindowProc(hWnd, message, wParam, lParam);};
	virtual BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnHorizontalScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnVerticalScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnHibernate(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnSysColorChange(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnCtlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnCtlColorBtn(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnExitMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnCaptureChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)			{return UNHANDLED;};
	virtual BOOL	OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)					{if(m_eWndType == WND_Dialog)
																							EndDialog(m_hWnd, 0);
																						else 
																							PostQuitMessage(0); 
																						return TRUE;};
	virtual BOOL	OnMCNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};

private:	// functions
	ATOM	RegisterMyClass(UINT uiIcon = 0);
	BOOL	CheckWindow(TCHAR* szWindowName);

protected:	// variables
	CIssString* m_oStr;
	HWND		m_hWnd;
	HINSTANCE	m_hInst;
	BOOL		m_bHandlePaint;		// do we want our class to handle any paints
	EnumWndType m_eWndType;			// type of window created

private:	// variables
	ATOM		m_aClass;
	TCHAR*		m_szClassName;
};

