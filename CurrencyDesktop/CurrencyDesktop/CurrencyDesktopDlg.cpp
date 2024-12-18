// CurrencyDesktopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CurrencyDesktop.h"
#include "CurrencyDesktopDlg.h"
#include "IssInternet.h"
#include "IssIniEx.h"
#include "issdebug.h"
#include "zip.h"
#include "FtpSockClient.h"
#include "ftp.h"
#include "IssCommon.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_CURRENCYDOWNLOAD     WM_USER + 1

#define SIZE_PAGE		80000
TCHAR		g_szBuffer[SIZE_PAGE];		// our phat buffer

TypeCurrency::TypeCurrency()
:szAbv(NULL)
,szName(NULL)
,szSymbol(NULL)
,szExchange(NULL)
,szBid(NULL)
,szAsk(NULL)
{}

TypeCurrency::~TypeCurrency()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szAbv);
    oStr->Delete(&szName);
    oStr->Delete(&szSymbol);
    oStr->Delete(&szExchange);
    oStr->Delete(&szBid);
    oStr->Delete(&szAsk);
}

// CCurrencyDesktopDlg dialog
CCurrencyDesktopDlg::CCurrencyDesktopDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCurrencyDesktopDlg::IDD, pParent)
,m_oStr(CIssString::Instance())
,m_hWndOutput(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCurrencyDesktopDlg::~CCurrencyDesktopDlg()
{
    DestroyArray();
    m_oStr->DeleteInstance();
}

void CCurrencyDesktopDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CCurrencyDesktopDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_CURRENCYDOWNLOAD, OnStartDownloading)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCurrencyDesktopDlg message handlers

BOOL CCurrencyDesktopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_objLanguages.Initialize(AfxGetResourceHandle());

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    m_hWndOutput		= ::GetDlgItem(m_hWnd, IDC_EDT_Output);

    if(!LoadInitialCurrencies())
        return TRUE;

    PostMessage(WM_CURRENCYDOWNLOAD, 0, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCurrencyDesktopDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCurrencyDesktopDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCurrencyDesktopDlg::DestroyArray()
{
    for(int i=0; i<m_arrCurrency.GetSize(); i++)
    {
        TypeCurrency* sCurrency = m_arrCurrency[i];
        if(sCurrency)
            delete sCurrency;
    }
    m_arrCurrency.RemoveAll();
}

BOOL CCurrencyDesktopDlg::LoadInitialCurrencies()
{
    CIssIniEx ini;

    AddOutput(_T("Loading initial currencies..."));

    if(S_OK != ini.OpenFromResrouce(_T("CURRENCY"), AfxGetResourceHandle()))
    {
        AddOutput(_T("Error loading initial currencies"));
        return FALSE;
    }

    int iNumValues = 0;

    if(S_OK != ini.GetValue(&iNumValues, _T("Info"), _T("NumItems"), 0))
    {
        AddOutput(_T("Error loading initial currencies"));
        return FALSE;
    }

	m_ctrlProgress.SetRange(0, iNumValues);
    m_ctrlProgress.SetStep(1);
    m_ctrlProgress.SetPos(0);

    TCHAR szFullText[STRING_MAX];
    TCHAR szID[STRING_SMALL];

    AddOutput(_T("Found %d currency entries"), iNumValues);
    for(int i=0; i<iNumValues; i++)
    {
        m_oStr->IntToString(szID, i+1);
        if(S_OK != ini.GetValue(szFullText, _T("Items"), szID, _T("")))
        {
            AddOutput(_T("Error finding value %s"), szID);
            return FALSE;
        }

        TypeCurrency* sNew = new TypeCurrency;
        if(!sNew)
        {
            AddOutput(_T("Error creating sNew at value %s"), szID);
            return FALSE;
        }

        if(!FillCurrencyStruct(sNew, szFullText))
        {
            AddOutput(_T("Error filling struct at value %s"), szID);
            delete sNew;
            return FALSE;
        }
		else if(m_oStr->Compare(sNew->szAbv, "EUR") == 0)
		{
		//	sNew->szSymbol = _T("€");
		}

		

        m_arrCurrency.AddElement(sNew);

    }


    AddOutput(_T("Successfully loaded initial currencies"));
    return TRUE;
}

void CCurrencyDesktopDlg::AddOutput(TCHAR* szFormat, ...)
{
    if(NULL == szFormat || NULL == m_hWndOutput)
        return;

    va_list vaMarker; 

    TCHAR* szDebug	= new TCHAR[2*STRING_MAX];
    va_start(vaMarker, szFormat); 
    vswprintf(szDebug, szFormat, vaMarker); 
    va_end(vaMarker); 	

    int iLen1		= ::GetWindowTextLength(m_hWndOutput);
    int iLen2		= m_oStr->GetLength(szDebug);
    TCHAR* szTotal	= new TCHAR[iLen1+iLen2+5]; 

    m_oStr->Empty(szTotal);
    ::GetWindowText(m_hWndOutput, szTotal, iLen1+1);
    if(!m_oStr->IsEmpty(szTotal))
        m_oStr->Insert(szTotal, _T("\r\n"));
    m_oStr->Insert(szTotal, szDebug);
    ::SetWindowText(m_hWndOutput, szTotal);
    delete [] szTotal;
    m_oStr->Delete(&szDebug);

    UpdateWindow();
}

BOOL CCurrencyDesktopDlg::FillCurrencyStruct(TypeCurrency* sCurrency, TCHAR* szFullText)
{
    int iOldIndex;
    int iIndex = m_oStr->Find(szFullText, _T(","), 0);
    if(-1 == iIndex)
        return FALSE;

    sCurrency->szAbv    = m_oStr->CreateAndCopy(szFullText, 0, iIndex);
    if(!sCurrency->szAbv)
        return FALSE;

    iOldIndex = iIndex+1;
    iIndex = m_oStr->Find(szFullText, _T(","), iOldIndex);
    if(-1 == iIndex)
        return FALSE;

    sCurrency->szName   = m_oStr->CreateAndCopy(szFullText, iOldIndex, iIndex-iOldIndex);
    if(!sCurrency->szName)
        return FALSE;

    // last one
    sCurrency->szSymbol = m_oStr->CreateAndCopy(szFullText, iIndex + 1, m_oStr->GetLength(szFullText) - iIndex - 1);
//	if(m_oStr->Compare(sCurrency->szAbv, "EUR") == 0)
//		Sleep(0);

    if(!sCurrency->szSymbol)
        return FALSE;

    return TRUE;
}

BOOL CCurrencyDesktopDlg::FillCurrencyStructQuotes(TypeCurrency* sCurrency, TCHAR* szFullText)
{
    int iOldIndex;
    int iIndex = m_oStr->Find(szFullText, _T(","), 0);
    if(-1 == iIndex)
        return FALSE;

    iOldIndex = iIndex + 1;
    iIndex = m_oStr->Find(szFullText, _T(","), iOldIndex);
    if(-1 == iIndex)
        return FALSE;

    sCurrency->szExchange   = m_oStr->CreateAndCopy(szFullText, iOldIndex, iIndex-iOldIndex);
    if(!sCurrency->szExchange)
        return FALSE;

    // this is the date
    iOldIndex = iIndex + 1;
    iIndex = m_oStr->Find(szFullText, _T(","), iOldIndex);
    if(-1 == iIndex)
        return FALSE;

    // this is the time
    iOldIndex = iIndex + 1;
    iIndex = m_oStr->Find(szFullText, _T(","), iOldIndex);
    if(-1 == iIndex)
        return FALSE;

    iOldIndex = iIndex + 1;
    iIndex = m_oStr->Find(szFullText, _T(","), iOldIndex);
    if(-1 == iIndex)
        return FALSE;

    sCurrency->szBid   = m_oStr->CreateAndCopy(szFullText, iOldIndex, iIndex-iOldIndex);
    if(!sCurrency->szBid)
        return FALSE;

    sCurrency->szAsk = m_oStr->CreateAndCopy(szFullText, iIndex + 1, m_oStr->GetLength(szFullText) - iIndex - 1);
    if(!sCurrency->szAsk)
        return FALSE;

    return TRUE;
}


BOOL CCurrencyDesktopDlg::DownloadCurrencies()
{
    CIssInternet oInet;
    oInet.Init(NULL, WM_USER, FALSE, FALSE, FALSE, TRUE);

    TCHAR szText[STRING_MAX];
    TCHAR szURL[] = _T("http://download.finance.yahoo.com/d/quotes.csv?s=USD%s=X&f=sl1d1t1ba&e=.csv");

    AddOutput(_T("Downloading currencies..."));

    for(int i=0; i<m_arrCurrency.GetSize(); i++)
    {
        TypeCurrency* sCurrency = m_arrCurrency[i];
        if(!sCurrency)
            continue;

        m_oStr->Format(szText, szURL, sCurrency->szAbv);
        ZeroMemory(g_szBuffer, sizeof(TCHAR)*SIZE_PAGE);
        if(!oInet.DownloadFile(szText, NULL, g_szBuffer, SIZE_PAGE))
        {
            AddOutput(_T("Error downloading currency at value %d"), i);
            return FALSE;
        }

        m_oStr->Trim(g_szBuffer);

        if(!FillCurrencyStructQuotes(sCurrency, g_szBuffer))
        {
            AddOutput(_T("Error Fill Currency quotes at value %d"), i);
            return FALSE;
        }

		//hack for israeli symbol
		/*if(m_oStr->Compare(_T("ILS"), sCurrency->szAbv) == 0)
		{
			sCurrency->szSymbol = _T("₪");
		}*/


        MSG stMsg = { 0 };
        while( PeekMessage( &stMsg, m_hWnd, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &stMsg );
            DispatchMessage( &stMsg );

            if(stMsg.message == WM_DESTROY)
                break;            
        }

        m_ctrlProgress.SetPos(i);
    }

    AddOutput(_T("Successfully downloaded currencies!"));
    return TRUE;
}   

void CCurrencyDesktopDlg::SetUSD()
{
    AddOutput(_T("Setting USD to 1.00"));

    for(int i = 0; i < m_arrCurrency.GetSize(); i++)
    {
        if(m_oStr->Compare(m_arrCurrency[i]->szAbv, _T("USD")) == 0)
        {
               m_oStr->Delete(&m_arrCurrency[i]->szExchange);
               m_oStr->Delete(&m_arrCurrency[i]->szBid);
               m_oStr->Delete(&m_arrCurrency[i]->szAsk);

               m_arrCurrency[i]->szExchange = m_oStr->CreateAndCopy(_T("1.000"));
               m_arrCurrency[i]->szBid = m_oStr->CreateAndCopy(_T(".999"));
               m_arrCurrency[i]->szAsk = m_oStr->CreateAndCopy(_T("1.001"));
        }
    }
}

void CCurrencyDesktopDlg::AddEuroCurrencies()
{
    TCHAR szCur[STRING_NORMAL];

    TypeCurrency* sCur = NULL;

    AddOutput(_T("Adding 13 classic European Currencies"));
    //get the Euro exchange rate so we can convert to USD
    double dbEuro = 0;

    for(int i = 0; i < m_arrCurrency.GetSize(); i++)
    {
        if(m_oStr->Compare(m_arrCurrency[i]->szAbv, _T("EUR")) == 0)
        {
            dbEuro = m_oStr->StringToDouble(m_arrCurrency[i]->szExchange);
            break;
        }
    }

    //lets fix XPF first
    AddOutput(_T("Correcting XPF Exchange Rate"));
    for(int i = 0; i < m_arrCurrency.GetSize(); i++)
    {
        sCur = m_arrCurrency[i];
        if(sCur == NULL)
            continue;
        if(m_oStr->Compare(sCur->szAbv, _T("XPF")) == 0)
        {
            m_oStr->DoubleToString(szCur, dbEuro/0.00838);

            m_oStr->Delete(&sCur->szAsk);
            m_oStr->Delete(&sCur->szBid);
            m_oStr->Delete(&sCur->szExchange);

            sCur->szAsk = m_oStr->CreateAndCopy(szCur);
            sCur->szBid = m_oStr->CreateAndCopy(szCur);
            sCur->szExchange = m_oStr->CreateAndCopy(szCur);
            break;
        }
    }


    
    //13 hard coded currencies ... I can live with that

    int iInsertIndex = 0;
    int iCompare;

    //ATS
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("ATS"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Austria Schilling"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("S"));
    m_oStr->DoubleToString(szCur, dbEuro*13.7603);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);
    
    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //BEF
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("BEF"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Belgium Franc"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("Bf"));
    m_oStr->DoubleToString(szCur, dbEuro*40.3399);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //NLG
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("NLG"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Dutch Guilder"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("fl."));
    m_oStr->DoubleToString(szCur, dbEuro*2.20371);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //FRM
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("FRF"));
    sCur->szName = m_oStr->CreateAndCopy(_T("France Franc"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("FF"));
    m_oStr->DoubleToString(szCur, dbEuro*6.55957);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //FIM
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("FIM"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Finland Markaa"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("FmK"));
    m_oStr->DoubleToString(szCur, dbEuro*5.94573);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //DEM
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("DEM"));
    sCur->szName = m_oStr->CreateAndCopy(_T("German Deutsche Mark"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("DM"));
    m_oStr->DoubleToString(szCur, dbEuro*1.95583);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //GRD
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("GRD"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Greece Drachma"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("Dr"));
    m_oStr->DoubleToString(szCur, dbEuro*340.750);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //IEP
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("IEP"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Ireland Pound"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("IR£"));
    m_oStr->DoubleToString(szCur, dbEuro*0.787564);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //ITL
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("ITL"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Italy Lira"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("L"));
    m_oStr->DoubleToString(szCur, dbEuro*1936.27);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //LUF
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("LUF"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Luxenbourg Franc"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("Lux"));
    m_oStr->DoubleToString(szCur, dbEuro*40.3399);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //PTE
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("PTE"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Portugal Escudo"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("Esc"));
    m_oStr->DoubleToString(szCur, dbEuro*200.482);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);

    //ESP
    iInsertIndex = 0;
    sCur = new TypeCurrency;
    sCur->szAbv = m_oStr->CreateAndCopy(_T("ESP"));
    sCur->szName = m_oStr->CreateAndCopy(_T("Spain Peseta"));
    sCur->szSymbol = m_oStr->CreateAndCopy(_T("Pts"));
    m_oStr->DoubleToString(szCur, dbEuro*166.386);
    sCur->szExchange = m_oStr->CreateAndCopy(szCur);
    sCur->szBid = m_oStr->CreateAndCopy(szCur);
    sCur->szAsk = m_oStr->CreateAndCopy(szCur);

    for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
    {
        iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

        if(iCompare > 0)
            break;
    }

    m_arrCurrency.AddElement(sCur);
    m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);


	//MTL
	iInsertIndex = 0;
	sCur = new TypeCurrency;
	sCur->szAbv = m_oStr->CreateAndCopy(_T("MTL"));
	sCur->szName = m_oStr->CreateAndCopy(_T("Malta Lira"));
	sCur->szSymbol = m_oStr->CreateAndCopy(_T("Lm"));
	m_oStr->DoubleToString(szCur, dbEuro*0.4293);
	sCur->szExchange = m_oStr->CreateAndCopy(szCur);
	sCur->szBid = m_oStr->CreateAndCopy(szCur);
	sCur->szAsk = m_oStr->CreateAndCopy(szCur);

	for(iInsertIndex = 0; iInsertIndex < m_arrCurrency.GetSize(); iInsertIndex++)
	{
		iCompare = m_oStr->Compare(m_arrCurrency[iInsertIndex]->szName, sCur->szName);

		if(iCompare > 0)
			break;
	}

	m_arrCurrency.AddElement(sCur);
	m_arrCurrency.Move(m_arrCurrency.GetSize()-1, iInsertIndex);
}

LRESULT CCurrencyDesktopDlg::OnStartDownloading(WPARAM wParam, LPARAM lParam)
{
    if(!DownloadCurrencies())
        return S_FALSE;

    AddEuroCurrencies();

    SetUSD();

	SaveCurrencyValues(_T("7"), L_English, FALSE);
	SaveCurrencyValues(_T("_fr7"), L_French, FALSE);
	SaveCurrencyValues(_T("_de7"), L_German, FALSE);
	SaveCurrencyValues(_T("_es7"), L_Spanish, FALSE);
	SaveCurrencyValues(_T("_pt7"), L_Portuguese, FALSE);
	SaveCurrencyValues(_T("_ja7"), L_Japanese, FALSE);

	SaveCurrencyValues(_T("_cz7"), L_Chinese, FALSE);
	SaveCurrencyValues(_T("_ko7"), L_Korean, FALSE);
	SaveCurrencyValues(_T("_du7"), L_Dutch, FALSE);
	SaveCurrencyValues(_T("_it7"), L_Italian, FALSE);

	SaveCurrencyValues(_T("i"), L_English, FALSE, FALSE);
	SaveCurrencyValues(_T("_fri"), L_French, FALSE, FALSE);
	SaveCurrencyValues(_T("_dei"), L_German, FALSE, FALSE);
	SaveCurrencyValues(_T("_esi"), L_Spanish, FALSE, FALSE);
	SaveCurrencyValues(_T("_pti"), L_Portuguese, FALSE, FALSE);
	SaveCurrencyValues(_T("_jai"), L_Japanese, FALSE, FALSE);

	SaveCurrencyValues(_T("_czi"), L_Chinese, FALSE, FALSE);
	SaveCurrencyValues(_T("_koi"), L_Korean, FALSE, FALSE);
	SaveCurrencyValues(_T("_dui"), L_Dutch, FALSE, FALSE);
	SaveCurrencyValues(_T("_iti"), L_Italian, FALSE, FALSE);

    SaveCurrencyValues();
	SaveCurrencyValues(_T("_fr"), L_French);
	SaveCurrencyValues(_T("_de"), L_German);
	SaveCurrencyValues(_T("_es"), L_Spanish);
	SaveCurrencyValues(_T("_pt"), L_Portuguese);
    SaveCurrencyValues(_T("_ja"), L_Japanese);

	SaveCurrencyValues(_T("_cz"), L_Chinese);
	SaveCurrencyValues(_T("_ko"), L_Korean);
	SaveCurrencyValues(_T("_du"), L_Dutch);
	SaveCurrencyValues(_T("_it"), L_Italian);

   if(!UploadFile())
       return S_FALSE;

    Sleep(500);
    EndDialog(0);

    return S_OK;
}

BOOL CCurrencyDesktopDlg::SaveCurrencyValues(TCHAR* szExt, EnumLanguages eLang, BOOL bPassword /* true */, BOOL bZip)
{
	switch(eLang)
	{
	case L_English:
		AddOutput(_T("Saving Currencies to file - English"));
		break;
	case L_French:
		AddOutput(_T("Saving Currencies to file - French"));
		break;
	case L_Spanish:
		AddOutput(_T("Saving Currencies to file - Spanish"));
		break;
	case L_German:
		AddOutput(_T("Saving Currencies to file - German"));
		break;
	case L_Portuguese:
		AddOutput(_T("Saving Currencies to file - Portuguese"));
		break;
    case L_Japanese:
        AddOutput(_T("Saving Currencies to file - Japanese"));
        break;
	case L_Chinese:
		AddOutput(_T("Saving Currencies to file - Chinese"));
		break;
	case L_Korean:
		AddOutput(_T("Saving Currencies to file - Korean"));
		break;
	case L_Dutch:
		AddOutput(_T("Saving Currencies to file - Dutch"));
		break;
	case L_Italian:
		AddOutput(_T("Saving Currencies to file - Italian"));
		break;
	default:
		return FALSE;
	}
    
    TCHAR szText[STRING_MAX];
	TCHAR szTemp[STRING_MAX];

    CIssIniEx ini;

    ini.AddSection(_T("Info"));
    ini.AddSection(_T("Currencies"));

    SYSTEMTIME sysGiven;
    GetLocalTime(&sysGiven);
    TCHAR szDate[STRING_LARGE];
    TCHAR szTime[STRING_LARGE];
    TCHAR szNum[STRING_LARGE];
    GetDateFormat(NULL, DATE_SHORTDATE, &sysGiven, NULL, szDate, STRING_LARGE);
    GetTimeFormat(NULL, 0, &sysGiven, _T(" hh':'mm GMT"), szTime, STRING_LARGE);

	m_oStr->StringCopy(szText, _T("http://www.panoramicsoft.com/mobileapps/calcpro/currency"));
	if(szExt)
		m_oStr->Concatenate(szText, szExt);
	m_oStr->Concatenate(szText, _T(".ini"));
	ini.AddAttribute(_T("Info"), _T("download"), szText);

    m_oStr->StringCopy(szText, szDate);
    m_oStr->Concatenate(szText, szTime);

    
    ini.AddAttribute(_T("Info"), _T("lastupdate"), szText);

    int iTime;
    SystemTimeToUtc(sysGiven, iTime);
    m_oStr->IntToString(szText, iTime);
    ini.AddAttribute(_T("Info"), _T("utc"), szText);

    m_oStr->IntToString(szText, m_arrCurrency.GetSize());
    ini.AddAttribute(_T("Info"), _T("numcurrencies"), szText);


    for(int i=0; i<m_arrCurrency.GetSize(); i++)
    {
        TypeCurrency* sCurrency = m_arrCurrency[i];
        if(!sCurrency)
            continue;

		m_oStr->StringCopy(szTemp, sCurrency->szName);
		if(eLang != L_English)
		{
			//if(eLang == L_Portuguese && 0 == m_oStr->Compare(sCurrency->szAbv, _T("XAG")))
			//	int i=1;

			TypeLanItem* sItem = m_objLanguages.FindItem(sCurrency->szAbv);
			if(!sItem || !sItem->szText[eLang])
			{
				DBG_OUT((_T("Warning: no translation for %s"), sCurrency->szAbv));
			}
			else
			{
				m_oStr->StringCopy(szTemp, sItem->szText[eLang]);
			}
		}

        m_oStr->IntToString(szNum, i+1);
        m_oStr->Format(szText, _T("%s,%s,%s,%s,%s,%s"), sCurrency->szAbv, szTemp, sCurrency->szSymbol, sCurrency->szExchange, sCurrency->szBid, sCurrency->szAsk);

        ini.AddAttribute(_T("Currencies"), szNum, szText);
    }


	if(bZip == false)
	{ //just save and return ... no password without zip of course
		m_oStr->StringCopy(szText, _T("currency"));
		if(szExt)
			m_oStr->Concatenate(szText, szExt);
		m_oStr->Concatenate(szText, _T(".ini"));

		ini.SaveUnicode(szText);
	}
	else if(!bPassword)
	{
		ini.SaveUTF8(_T("currency-nozip.ini"));//need utf8 for WP7 but not for WinMo ... checking on iPhone
	}
    else if(eLang == L_Japanese)
	{    
		ini.SaveUnicode(_T("currency-nozip.ini"));
	}
	else
	{
		ini.Save(_T("currency-nozip.ini"));
	}

    AddOutput(_T("Successfully saved currency file"));

	if(bZip)
	{
		m_oStr->StringCopy(szText, _T("currency"));
		if(szExt)
			m_oStr->Concatenate(szText, szExt);
		m_oStr->Concatenate(szText, _T(".ini"));

		//set the password ... long and complex ... no brute force cracks here
		HZIP hZip = CreateZip(szText, bPassword?"26n1CE7ry26":NULL);

		ZRESULT z = ZipAdd(hZip, _T("currency-nozip.ini"), _T("currency-nozip.ini"));

		CloseZip(hZip);
	}

    return TRUE;
}

BOOL CCurrencyDesktopDlg::UploadFile()
{

    int responseCode=0;
    char cmdOpen[250] = "";
    char cmdLogin[250] = "";
    char cmdLCD[200] = "";
    char cmdCD[200] = "";
    ftp theFtpConnection;

    UpdateData(TRUE);
    strcpy(	cmdOpen, "open " );
    strcat( cmdOpen,  "panoramicsoft.com");//"panoramic.retrix.net");
    strcpy(cmdLogin, "user ");
    strcat(cmdLogin,"panoramic");
    strcat(cmdLogin,":");
    strcat(cmdLogin,"flash37");
    strcpy(cmdCD,"cd /Websites/panoramicsoft.com/mobileapps/calcpro");
   
    AddOutput(_T("Uploading currency files"));
	UpdateData(TRUE);

    responseCode=theFtpConnection.DoOpen(cmdOpen);
    if(responseCode==INVALID_SOCKET)
    {
        AddOutput(_T("FAILED: could not open FTP connection"));
        return FALSE;
    }
    responseCode=theFtpConnection.DoLogin(cmdLogin);
    if(responseCode!=230)
    {
        AddOutput(_T("FAILED: could not login to FTP"));
        theFtpConnection.DoClose();
        return TRUE;
    }

    responseCode= theFtpConnection.DoCD(cmdCD);
    if(responseCode!=250)
    {
        AddOutput(_T("FAILED: could not change FTP directory"));
        theFtpConnection.DoClose();
        return TRUE;
    }

    theFtpConnection.DoBinary();

	//WP7
	AddOutput(_T("Uploading currency7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency7.ini");
	AddOutput(_T("Uploading currency_fr7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_fr7.ini");
	AddOutput(_T("Uploading currency_es7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_es7.ini");
	AddOutput(_T("Uploading currency_de7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_de7.ini");
	AddOutput(_T("Uploading currency_pt7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_pt7.ini");
	AddOutput(_T("Uploading currency_ja7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_ja7.ini");

	AddOutput(_T("Uploading currency_cz7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_cz7.ini");
	AddOutput(_T("Uploading currency_ko7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_ko7.ini");
	AddOutput(_T("Uploading currency_du7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_du7.ini");
	AddOutput(_T("Uploading currency_it7.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_it7.ini");

	//iApple
	AddOutput(_T("Uploading currencyi.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currencyi.ini");
	AddOutput(_T("Uploading currency_fri.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_fri.ini");
	AddOutput(_T("Uploading currency_esi.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_esi.ini");
	AddOutput(_T("Uploading currency_dei.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_dei.ini");
	AddOutput(_T("Uploading currency_pti.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_pti.ini");
	AddOutput(_T("Uploading currency_jai.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_jai.ini");

	AddOutput(_T("Uploading currency_czi.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_czi.ini");
	AddOutput(_T("Uploading currency_koi.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_koi.ini");
	AddOutput(_T("Uploading currency_dui.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_dui.ini");
	AddOutput(_T("Uploading currency_iti.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_iti.ini");


	//WinMo & PC
	AddOutput(_T("Uploading currency.ini"));
	UpdateData(TRUE);
    responseCode = theFtpConnection.DoPut("currency.ini");
	AddOutput(_T("Uploading currency_fr.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_fr.ini");
	AddOutput(_T("Uploading currency_es.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_es.ini");
	AddOutput(_T("Uploading currency_de.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_de.ini");
	AddOutput(_T("Uploading currency_pt.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_pt.ini");
    AddOutput(_T("Uploading currency_ja.ini"));
    UpdateData(TRUE);
    responseCode = theFtpConnection.DoPut("currency_ja.ini");

	AddOutput(_T("Uploading currency_cz.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_cz.ini");
	AddOutput(_T("Uploading currency_ko.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_ko.ini");
	AddOutput(_T("Uploading currency_du.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_du.ini");
	AddOutput(_T("Uploading currency_it.ini"));
	UpdateData(TRUE);
	responseCode = theFtpConnection.DoPut("currency_it.ini");

    theFtpConnection.DoClose();

    AddOutput(_T("Successfully uploaded currency file"));
 
    return TRUE;
}

SOCKADDR_IN CCurrencyDesktopDlg::GetHostByName(char* szName, USHORT usPort)
{
    SOCKADDR_IN sockTemp;
    memset(&sockTemp, 0, sizeof sockTemp);

    if(!szName)
        return sockTemp;

    hostent* pHostEnt = gethostbyname(szName);
    if (pHostEnt == NULL)
    {
        // didn't work
    }
    else
    {
        ULONG* pulAddr = (ULONG*) pHostEnt->h_addr_list[0];
        sockTemp.sin_family     = AF_INET;
        sockTemp.sin_port       = htons(usPort);
        sockTemp.sin_addr.s_addr = *pulAddr; // address is already in network byte order
    }

    return sockTemp;
}