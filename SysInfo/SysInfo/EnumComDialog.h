// EnumComDialog.h : interface of the CEnumComDialog class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SerialPortEnum.h"


class CEnumComDialog : 
	public CAppStdDialogImpl<CEnumComDialog>,
	public CUpdateUI<CEnumComDialog>,
	public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_APP_DLG_CLASS(NULL, IDR_MAINFRAME, L"Software\\WTL\\EnumCom")

	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

// CAppWindow operations

	/*
	bool AppHibernate( bool bHibernate)
	{
		// Insert your code here or delete member if not relevant
		return bHibernate;
	}

	bool AppNewInstance( LPCTSTR lpstrCmdLine)
	{
		// Insert your code here or delete member if not relevant
		return false;
	}

	void AppSave()
	{
		CAppInfo info;
		// Insert your code here or delete member if not relevant
	}
	*/

#ifdef WIN32_PLATFORM_WFSP
	void AppBackKey() 
	{
		StdCloseDialog(IDCANCEL);
	}
#endif

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CEnumComDialog)
	END_UPDATE_UI_MAP()


	BEGIN_MSG_MAP(CEnumComDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CAppStdDialogImpl<CEnumComDialog>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CListViewCtrl	listPort;
		CSerialPortEnum	portEnum;
		size_t			i;
		int				iItem = 0;

		HWND hMenuBar = CreateMenuBar(ATL_IDM_MENU_DONECANCEL);
		UIAddToolBar(hMenuBar);
		UIAddChildWindowContainer(m_hWnd);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		// Attach the control
		listPort.Attach(GetDlgItem(IDC_LIST_PORT));

		// Create the columns
		listPort.InsertColumn(0, _T("Port"),        LVCFMT_LEFT,  80, -1);
		listPort.InsertColumn(1, _T("Description"), LVCFMT_LEFT, 120,  1);

		// Insert the items
		for(i = 0; i < portEnum.GetCount(); ++i)
		{
			const CSerialPortInfo	*pInfo = portEnum[i];

			if(pInfo != NULL)
			{
				iItem = listPort.AddItem(iItem, 0, pInfo->GetPortName());

				listPort.SetItemText(iItem, 1, pInfo->GetDescription());
			}
		}

		return bHandled = FALSE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

private:
};

