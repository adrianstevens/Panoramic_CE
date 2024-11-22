#pragma once

#include "IssWnd.h"
#include "commdlg.h"
#include "commctrl.h"
#include "IssGDIEx.h"

class CCtrlBrowse:public CIssWnd
{
public:
	CCtrlBrowse(void);
	~CCtrlBrowse(void);

	BOOL	Create(HWND hWndParent, HINSTANCE hInst, HFONT hFont = NULL);
	void	Destroy();
	BOOL	SetFilter(TCHAR* szFilter);
	BOOL	SetCurrentFolder(TCHAR* szDir);
	BOOL	IsRootPath();
	TCHAR*	GetCurrentFolder();
	TCHAR*	GetFilter(){return m_szFilter;};
	BOOL	GetCurrentFolderName(TCHAR* szFolderName);
	BOOL	OnNotifyReflect(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void	OnSelectItem(BOOL bDoubleTap = TRUE);
	TCHAR*	GetSelectedPath(){return m_szSelectedPath;};
	TCHAR*	GetSelectedFullPath(){return m_szSelectedFullPath;};
//	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("iSSNotepad.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};

protected:
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnItemChanged();
	void	OnGetDisplayInfo(NMLVDISPINFO * lpdi);
	BOOL	ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void	AddListItem(WIN32_FIND_DATA& pwfd);
	BOOL	IsReturnPath(TCHAR* szPath);
	BOOL	RefreshList();
	void	GoUpDirectory();
	static int	CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	WNDPROC	m_lpfnCtrl;
	TCHAR	m_szPathFolder[MAX_PATH];
	TCHAR	m_szFilter[STRING_LARGE];
	TCHAR*	m_szSelectedPath;
	TCHAR*	m_szSelectedFullPath;
};

class CDlgGetFile:public CIssWnd
{
public:
	CDlgGetFile(void);
	~CDlgGetFile(void);
	BOOL	GetSaveFile(OPENFILENAME* sSaveFile);
	BOOL	GetOpenFile(OPENFILENAME* sOpenFile);

	TCHAR*	GetCurrentPath(){return m_ctrlBrowse.GetCurrentFolder();};

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

	void	InitList();
	void	ClearFilter();
	void	SetCurrentFilterList();
	void	DrawText(HDC hDC, RECT rcClient);
	void	EndDialog(int nResult);
	void	OnSelectFile(TCHAR* szFileName, TCHAR* szFullPath);
	void	SaveFile(TCHAR* szFileName);
	BOOL	BuildFullPath(TCHAR* szFullPath);
	void	OnMenuSave();
	void	OnMenuOpen();
	
private:	// variables
	CIssGDIEx m_gdiMem;	
	CCtrlBrowse m_ctrlBrowse;	// this will hold our folders
	HWND	m_hWndName;			// name of the file
	HWND	m_hWndSpin;			// spin control, only used for smartphone
	HWND	m_hWndType;			// type of file
	RECT	m_rcName;			// location of the name title
	RECT	m_rcType;			// location of the type title
	RECT	m_rcDirectory;		// location of the directory title
	BOOL	m_bOpenFile;		// are we opening or saving a file
	OPENFILENAME* m_sOpenFile;	// attributes for open or save a file
	HFONT	m_hFontText;		// our normal text font to use
	//SHACTIVATEINFO		m_sSai;
};


