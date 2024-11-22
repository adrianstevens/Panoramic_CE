// CurrencyDesktopDlg.h : header file
//

#pragma once
#include "winsock2.h"
#include "IssString.h"
#include "IssVector.h"
#include "afxcmn.h"
#include "ObjCurLanguages.h"


struct TypeCurrency
{
    TCHAR*  szAbv;
    TCHAR*  szName;
    TCHAR*  szSymbol;
    TCHAR*  szExchange;
    TCHAR*  szBid;
    TCHAR*  szAsk;

    TypeCurrency();
    ~TypeCurrency();
};


// CCurrencyDesktopDlg dialog
class CCurrencyDesktopDlg : public CDialog
{
// Construction
public:
	CCurrencyDesktopDlg(CWnd* pParent = NULL);	// standard constructor
    ~CCurrencyDesktopDlg();

// Dialog Data
	enum { IDD = IDD_CURRENCYDESKTOP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

    void        DestroyArray();
    BOOL        LoadInitialCurrencies();
    LRESULT     OnStartDownloading(WPARAM wParam, LPARAM lParam);
    BOOL        DownloadCurrencies();
    void        AddEuroCurrencies();
   // void        CorrectCurrencies();
    void        SetUSD();

    void	    AddOutput(TCHAR* szFormat, ...);
    BOOL        FillCurrencyStruct(TypeCurrency* sCurrency, TCHAR* szFullText);
    BOOL        FillCurrencyStructQuotes(TypeCurrency* sCurrency, TCHAR* szFullText);
    BOOL        SaveCurrencyValues(TCHAR* szExt = NULL, EnumLanguages eLang = L_English, BOOL bPassword = TRUE, BOOL bZip = TRUE);

    BOOL        UploadFile();//ftp upload
    SOCKADDR_IN GetHostByName(char* szName, USHORT usPort);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:    // variables
    CIssString* m_oStr;
    CIssVector<TypeCurrency>    m_arrCurrency;
    HWND        m_hWndOutput;
	CObjCurLanguages m_objLanguages;
public:
    CProgressCtrl m_ctrlProgress;
};
