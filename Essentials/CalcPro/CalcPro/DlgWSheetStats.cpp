#include "DlgWSheetStats.h"
#include "IssDebug.h"
#include "DlgListResults.h"
#include "DlgGraph.h"
#include "globals.h"
#include "IssLocalisation.h"


#ifdef UNDER_CE
#include "DlgGetFile.h"
#else
#include "stdio.h"
#endif

LPCTSTR lpcstrFilter = //Adrian said don't touch on March 11, 2010
_T("CSV Files (*.csv)\0*.csv\0")
_T("TXT Files (*.txt)\0*.txt\0")
_T("All Files (*.*)\0*.*\0")
_T("");

CDlgWSheetStats::CDlgWSheetStats(LaunchType eWS)
:m_szEquation(NULL)
{
    switch(eWS)
    {
    case LAUNCH_Power:
        m_eStatType = Stats_Power_Reg;
        break;
    case LAUNCH_LogReg:
        m_eStatType = Stats_Log_Reg;
        break;
    case LAUNCH_ExpReg:
        m_eStatType = Stats_Exponential_Reg;
        break;
    case LAUNCH_LinearReg:
    default:
        m_eStatType = Stats_Linear_Reg;
        break;
    }
    //I'm not saving this in the registry .. .way too lazy
    m_oStr->StringCopy(m_szPathFolder, _T("\\"));
}

CDlgWSheetStats::~CDlgWSheetStats(void)
{
    m_oStr->Delete(&m_szEquation);
}

BOOL CDlgWSheetStats::InitScreenEntries()
{
    //we'll just start with two sets of entries
    AddNextSet();
    AddNextSet();

    switch(m_eStatType)
    {
    case Stats_Power_Reg:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_INFO_PowerRegression), m_hInst));
    	break;
    case Stats_Log_Reg:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_INFO_LogRegression), m_hInst));
    	break;
    case Stats_Exponential_Reg:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_INFO_ExpRegression), m_hInst));
        break;
    case Stats_Ln_Reg:
    default:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_INFO_LinearRegression), m_hInst));    
        break;
    }
    
    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); 

    return TRUE;

}

void CDlgWSheetStats::AddNextSet()
{
    TCHAR szTemp[STRING_LARGE];

    //we "could" handle 1 variable stats here too ...
    int iEntry = m_arrEntries.GetSize()/2 + 1;

    //now we need both an X & a Y
    m_oStr->Format(szTemp, _T("X(%i):"), iEntry);
    TypeEntry* sEntry = new TypeEntry;
    sEntry->szEntryLabel = m_oStr->CreateAndCopy(szTemp);
    m_oStr->StringCopy(sEntry->szEntryValue, _T("0"));
    sEntry->bGreyed = FALSE;
    sEntry->eEntry = ENTRY_Double;

    m_arrEntries.AddElement(sEntry);
    sEntry = NULL;

    m_oStr->Format(szTemp, _T("Y(%i):"), iEntry);
    sEntry = new TypeEntry;
    m_oStr->StringCopy(sEntry->szEntryValue, _T("0"));
    sEntry->szEntryLabel = m_oStr->CreateAndCopy(szTemp);
    sEntry->bGreyed = FALSE;
    sEntry->eEntry = ENTRY_Double;

    m_arrEntries.AddElement(sEntry);
    sEntry = NULL;
}

BOOL CDlgWSheetStats::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //time to get down and dirty
    switch(LOWORD(wParam))
    {
    case VK_DOWN:
        if(m_iEntryIndex + m_iScreenIndex + 2> m_arrEntries.GetSize())
            AddNextSet();
    }

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);
}

BOOL CDlgWSheetStats::OnGraph()
{
    //gotta add the data before you can do anything
    m_oStats.ResetWorkSheet();

    //enter all of the values
    for(int i = 0; i < m_arrEntries.GetSize(); i+=2)
    {
        m_oStats.AddXValue(m_oStr->StringToDouble(m_arrEntries[i]->szEntryValue), i/2);
        m_oStats.AddYValue(m_oStr->StringToDouble(m_arrEntries[i+1]->szEntryValue), i/2);
    }
    m_oStats.SetStatsType(m_eStatType);

    m_oStats.Calculate();

    BuildLineEq();

    //and graph the sucker
    CDlgGraph dlgGraph;
    dlgGraph.SetFullScreen(m_oBtnMan->GetFullscreen());

    dlgGraph.SetMenu(&m_oMenu, m_wndMenu);
    dlgGraph.UsePoints(g_bGraphPoints);
    dlgGraph.SetGraphAccuracy((int)g_bGraphFast);

    //its WAY too many calculations when the tape is enabled (oops)
    BOOL bUseTape = m_oCalc->GetUseTape();
    m_oCalc->SetUseTape(FALSE);
    
    dlgGraph.SetGraph(m_szEquation, 0);

    for(int i = 0; i < m_oStats.GetNumberOfXYPairs(); i++)
    {
        dlgGraph.AddPoint(m_oStats.RecallXValue(i), m_oStats.RecallYValue(i));
    }

#ifdef UNDER_CE
    dlgGraph.DoModal(m_hWndParent, m_hInst, IDD_DLG_BASIC);
#else
	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	if(dlgGraph.Create(m_oStr->GetText(ID(IDS_MENU_CalcProGraph), m_hInst), NULL, m_hInst, m_oStr->GetText(ID(IDS_MENU_Graph), m_hInst),
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		0,
		DS_MODALFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX,
		NULL,
        IDI_CALCPRO))
	{
		MoveWindow(dlgGraph.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), TRUE);
		ShowWindow(dlgGraph.GetWnd(), SW_SHOW);
		ShowWindow(m_hWnd, SW_HIDE);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}

		ShowWindow(m_hWnd, SW_SHOW);
	}
	
	EnableWindow(m_hWnd, TRUE);
	InvalidateRect(m_hWnd, NULL, FALSE);
#endif
    m_oCalc->SetUseTape(bUseTape);
    SetForegroundWindow(GetParent(m_hWnd));
    return TRUE;
}

BOOL CDlgWSheetStats::OnCalc()
{
//#ifdef UNDER_CE
    TCHAR szTemp[STR_NORMAL];
    long double dbTemp;
    int iTemp;

    CDlgListResults dlgList;

    m_oStats.ResetWorkSheet();

    //enter all of the values
    for(int i = 0; i < m_arrEntries.GetSize(); i+=2)
    {
        m_oStats.AddXValue(m_oStr->StringToDouble(m_arrEntries[i]->szEntryValue), i/2);
        m_oStats.AddYValue(m_oStr->StringToDouble(m_arrEntries[i+1]->szEntryValue), i/2);
    }

    m_oStats.SetStatsType(m_eStatType);

    //solve
    if(!m_oStats.Calculate())
    {	
        ASSERT(0);
        return FALSE;
    }

    BuildLineEq();

      //lets open an answer DLG
      //now lets add some terms and stuff
    iTemp = m_oStats.GetNumberOfXYPairs();
    m_oStr->IntToString(szTemp, iTemp);
    dlgList.AddItem(_T("Data sets"), szTemp);

    m_oStats.GetMeanX(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_MeanOfX), m_hInst), szTemp);		

    m_oStats.GetMedianX(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("Median of X"), szTemp);		

    m_oStats.GetSX(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("SX"), szTemp);

    m_oStats.GetPopSX(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_DevOfX), m_hInst), szTemp);

    

    m_oStats.GetMeanY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_MeanOfY), m_hInst), szTemp);	

    m_oStats.GetMedianY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("Median of Y"), szTemp);	

    m_oStats.GetSY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("SY"), szTemp);

    m_oStats.GetPopSY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_DevOfY), m_hInst), szTemp);

    dlgList.AddItem(_T(""), _T(""));	

    m_oStats.GetSumX(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("\x2211X"), szTemp);

    m_oStats.GetSumX2(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("\x2211X²"), szTemp);

    m_oStats.GetSumY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("\x2211Y"), szTemp);		

    m_oStats.GetSumY2(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("\x2211Y²"), szTemp);

    m_oStats.GetSumXY(&dbTemp);
    m_oStr->DoubleToString(szTemp, dbTemp);
    dlgList.AddItem(_T("\x2211X×Y"), szTemp);		
    dlgList.AddItem(_T(""), _T(""));	


    switch(m_eStatType)
    {
    case Stats_Linear_Reg:
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Equation), m_hInst), _T("f(x)=a+b×x"));

        m_oStats.GetA(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_YIntercept), m_hInst), szTemp);

        m_oStats.GetB(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Slope), m_hInst), szTemp);

        m_oStats.GetR(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Regression), m_hInst), szTemp);	

        dbTemp = dbTemp*dbTemp;
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Determination), m_hInst), szTemp);	
    	break;
    case Stats_Exponential_Reg:
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Equation), m_hInst), _T("f(x)=a×b^x"));

        m_oStats.GetA(&dbTemp);
        dbTemp = pow(10, dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(_T("a"), szTemp);

        m_oStats.GetB(&dbTemp);
        dbTemp = pow(10, dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(_T("b"), szTemp);

        m_oStats.GetR(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Regression), m_hInst), szTemp);	

        dbTemp = dbTemp*dbTemp;
        m_oStr->DoubleToString(szTemp, dbTemp);
        dlgList.AddItem(m_oStr->GetText(ID(IDS_CALC_Determination), m_hInst), szTemp);	
        break;
    default:
        break;
    }

#ifdef UNDER_CE
    dlgList.DoModal(m_hWnd, m_hInst, IDD_DLG_BASIC);
#else
    dlgList.DoModal(m_hWnd, m_hInst, IDD_DLG_BASIC);
#endif
//#endif
    return TRUE;
}

BOOL CDlgWSheetStats::OnReset()
{
    m_oStats.ResetWorkSheet();
    DeleteEntryArray();
    AddNextSet();
    AddNextSet();
    m_iEntryIndex = 0;
    m_iScreenIndex = 0;

    InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}

BOOL CDlgWSheetStats::BuildLineEq()
{
    long double dbTemp;
    TCHAR szTemp[STR_LARGE];

    //Create the equation in terms of f(x)
    if(m_szEquation == NULL) //create the buffer of course
        m_szEquation = new TCHAR[STR_LARGE];
    m_oStr->Empty(m_szEquation);

    switch(m_eStatType)
    {
    case Stats_Exponential_Reg:
        
        long double dbA, dbB;
        m_oStats.GetA(&dbA);
        m_oStats.GetB(&dbB);

        dbA = pow(10, dbA);
        dbB = pow(10, dbB);

        m_oStr->Format(m_szEquation, _T("(%f)*(%f)^(X)"), dbA, dbB);

        break;
    case Stats_Linear_Reg:
    default:
        m_oStats.GetB(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        m_oStr->StringCopy(m_szEquation, szTemp);
        m_oStr->Concatenate(m_szEquation, _T("*X+"));
        m_oStats.GetA(&dbTemp);
        m_oStr->DoubleToString(szTemp, dbTemp);
        m_oStr->Concatenate(m_szEquation, szTemp);
        break;
    }


    //equation built y = ax+b
    return TRUE;
}

BOOL CDlgWSheetStats::OnLoad()
{
#ifdef UNDER_CE
    OPENFILENAME sOpenFileName;
    TCHAR szFile[STRING_MAX];
    BOOL bOpen = FALSE;

    m_oStr->Empty(szFile);

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWnd; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= (LPCTSTR)m_szPathFolder;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them

    CDlgGetFile	dlgGetFile;

    //open the file chooser dialog
    bOpen = dlgGetFile.GetOpenFile(&sOpenFileName);
    if(!bOpen)
        return FALSE;

    //only save it if they don't cancel
    m_oStr->StringCopy(m_szPathFolder, dlgGetFile.GetCurrentPath());

    FILE *fp;

    //open the file
    if((fp = _tfopen(sOpenFileName.lpstrFile, _T("rb"))) == NULL)
    {
        //the file was not able to be open
        //		MessageBox(_T("Could Not Open File for Reading"));
        return FALSE;
    }

    //read file stuff
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //the File was open so read in the contents in a buffer
    char *szFileBuffer = new char[dwFileSize+1];
    ZeroMemory(szFileBuffer, sizeof(CHAR)*(dwFileSize+1));

    if(szFileBuffer == NULL)
    {
        MessageBox(m_hWnd, m_oStr->GetText(ID(IDS_ERROR_OpenFile), m_hInst), m_oStr->GetText(ID(IDS_ERROR_CalcPro), m_hInst), MB_ICONEXCLAMATION);
        fclose(fp);
        return FALSE;
    }

    fread(szFileBuffer, sizeof(char), dwFileSize, fp);
    //end file reading

    LPTSTR		lpBuf = (LPTSTR)szFileBuffer;//just easier

    UINT nChars = 0;

    if( dwFileSize == 0 || lpBuf == NULL)
    {	//we'll be responsible and cleanup
        m_oStr->Delete(&szFileBuffer);
        CloseHandle( fp ); 
        fp=INVALID_HANDLE_VALUE;
    }

    nChars= dwFileSize / sizeof(TCHAR);

    //our data
    TCHAR* szText = new TCHAR[nChars+1];
    memcpy(szText, lpBuf, 2*nChars);

    szText[nChars]=_T('\0');

    //ok we're ready to rock ... clear the worksheet and load in the data!
    OnReset();

    int iIndex = 0;
    int iEntry = 0;
    int iNextComma = 0;

    TCHAR szTemp[STRING_LARGE];

    while(iNextComma < (int)nChars - 1)
    {
        iNextComma = m_oStr->Find(szText, _T(","), iNextComma);
        if(iNextComma > iIndex)
            m_oStr->StringCopy(szTemp,szText, iIndex, iNextComma-iIndex);
        else
            break;

        //now we need to do something with the value
        if(iEntry >= m_arrEntries.GetSize())
            AddNextSet();

        m_oStr->StringCopy(m_arrEntries[iEntry]->szEntryValue, szTemp);
        
        iIndex=iNextComma+1;
        iNextComma++;
        iEntry++;
    }
    m_oStr->Delete(&szText);
    szText = NULL;

    //and finally ... set the first entry into the calc engine
    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); 

    InvalidateRect(m_hWnd, NULL, FALSE);
#else //PC
    OPENFILENAME sOpenFileName;
    TCHAR szFile[STRING_MAX];
    BOOL bOpen = FALSE;

    m_oStr->Empty(szFile);

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWnd; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= (LPCTSTR)m_szPathFolder;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them

    //open the file chooser dialog
    bOpen = GetOpenFileName(&sOpenFileName);
    if(!bOpen)
        return FALSE;

    //only save it if they don't cancel
    m_oStr->StringCopy(m_szPathFolder, sOpenFileName.lpstrFile);

    FILE *fp;

    //open the file
    if((fp = _tfopen(sOpenFileName.lpstrFile, _T("rb"))) == NULL)
    {
        //the file was not able to be open
        //		MessageBox(_T("Could Not Open File for Reading"));
        return FALSE;
    }

    //read file stuff
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //the File was open so read in the contents in a buffer
    char *szFileBuffer = new char[dwFileSize+1];
    ZeroMemory(szFileBuffer, sizeof(CHAR)*(dwFileSize+1));

    if(szFileBuffer == NULL)
    {
        MessageBox(m_hWnd, m_oStr->GetText(ID(IDS_ERROR_OpenFile), m_hInst), m_oStr->GetText(ID(IDS_ERROR_CalcPro), m_hInst), MB_ICONEXCLAMATION);
        fclose(fp);
        return FALSE;
    }

    fread(szFileBuffer, sizeof(char), dwFileSize, fp);
    //end file reading

    LPTSTR		lpBuf = (LPTSTR)szFileBuffer;//just easier

    UINT nChars = 0;

    if( dwFileSize == 0 || lpBuf == NULL)
    {	//we'll be responsible and cleanup
        m_oStr->Delete(&szFileBuffer);
        CloseHandle( fp ); 
        fp=NULL; //INVALID_HANDLE_VALUE;
    }

    nChars= dwFileSize / sizeof(TCHAR);

    //our data
    TCHAR* szText = new TCHAR[nChars+1];
    memcpy(szText, lpBuf, 2*nChars);

    szText[nChars]=_T('\0');

    //ok we're ready to rock ... clear the worksheet and load in the data!
    OnReset();

    int iIndex = 0;
    int iEntry = 0;
    int iNextComma = 0;

    TCHAR szTemp[STRING_LARGE];

    while(iNextComma < (int)nChars - 1)
    {
        iNextComma = m_oStr->Find(szText, _T(","), iNextComma);
        if(iNextComma > iIndex)
            m_oStr->StringCopy(szTemp,szText, iIndex, iNextComma-iIndex);
        else
            break;

        //now we need to do something with the value
        if(iEntry >= m_arrEntries.GetSize())
            AddNextSet();

        m_oStr->StringCopy(m_arrEntries[iEntry]->szEntryValue, szTemp);
        
        iIndex=iNextComma+1;
        iNextComma++;
        iEntry++;
    }
    m_oStr->Delete(&szText);
    szText = NULL;

    //and finally ... set the first entry into the calc engine
    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); 

    InvalidateRect(m_hWnd, NULL, FALSE);



#endif
    return TRUE;
}

BOOL CDlgWSheetStats::OnSave()
{
#ifdef UNDER_CE
    if(m_arrEntries.GetSize() == 0)
        return FALSE;

    //otherwise we'll just save a CSV file
    OPENFILENAME sOpenFileName;
    TCHAR szFile[STRING_MAX];

    m_oStr->Empty(szFile);
    
    m_oStr->StringCopy(m_szPathFolder, _T("\\"));

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWnd; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= m_szPathFolder;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them
    
    CDlgGetFile	dlgGetFile;

    if(dlgGetFile.GetSaveFile(&sOpenFileName))
    {
        //save the path
        m_oStr->StringCopy(m_szPathFolder, dlgGetFile.GetCurrentPath());

        //find the length and ensure our vector is clean
        int iTextLen = 0;
        for(int i = 0; i < m_arrEntries.GetSize(); i++)
        {
            if(m_arrEntries[i] == NULL || m_arrEntries[i]->szEntryValue == NULL)
                return FALSE;
            iTextLen += m_oStr->GetLength(m_arrEntries[i]->szEntryValue);
            iTextLen++; //for the comma
        }

        //now we need to build up the string and write the file
        FILE	*fp;
        TCHAR*	szText = NULL;
        CHAR*	sText = NULL;

        szText = new TCHAR[iTextLen + 2];//not sure why its two - see notepad
        
        if(szText == NULL)
            return FALSE;
        ZeroMemory(szText, sizeof(TCHAR)*(iTextLen + 2));

        //open the file
        if((fp = _tfopen(szFile, _T("wb"))) == NULL)
        {
            //the file was not able to be open
            return FALSE;
        }

        int iIndex = 0; //we're going to a series of copies because concat isn't too efficient
        int iLen = 0;

        for(int i = 0; i < m_arrEntries.GetSize(); i++)
        {
            iLen = m_oStr->GetLength(m_arrEntries[i]->szEntryValue);
            memcpy(&szText[iIndex], m_arrEntries[i]->szEntryValue, sizeof(TCHAR)*iLen);
            iIndex+=iLen;
            //comma of course
            szText[iIndex]=_T(',');
            iIndex++;
        }

        //don't forget the stop char
        szText[iIndex] = _T('\0');

        //write to the file
        fwrite(szText, sizeof(TCHAR), iTextLen, fp);

        m_oStr->Delete(&szText);
        szText = NULL;

        //close the file
        fclose(fp);
    }
#else
   if(m_arrEntries.GetSize() == 0)
        return FALSE;

    //otherwise we'll just save a CSV file
    OPENFILENAME sOpenFileName;
    TCHAR szFile[STRING_MAX];

    m_oStr->Empty(szFile);
    
    m_oStr->StringCopy(m_szPathFolder, _T("\\"));

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWnd; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpcstrFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= m_szPathFolder;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them
    

    if(GetSaveFileName(&sOpenFileName))
    {
        //save the path
        m_oStr->StringCopy(m_szPathFolder, sOpenFileName.lpstrFile);

        //find the length and ensure our vector is clean
        int iTextLen = 0;
        for(int i = 0; i < m_arrEntries.GetSize(); i++)
        {
            if(m_arrEntries[i] == NULL || m_arrEntries[i]->szEntryValue == NULL)
                return FALSE;
            iTextLen += m_oStr->GetLength(m_arrEntries[i]->szEntryValue);
            iTextLen++; //for the comma
        }

        //now we need to build up the string and write the file
        FILE	*fp;
        TCHAR*	szText = NULL;
        CHAR*	sText = NULL;

        szText = new TCHAR[iTextLen + 2];//not sure why its two - see notepad
        
        if(szText == NULL)
            return FALSE;
        ZeroMemory(szText, sizeof(TCHAR)*(iTextLen + 2));

        //open the file
        if((fp = _tfopen(szFile, _T("wb"))) == NULL)
        {
            //the file was not able to be open
            return FALSE;
        }

        int iIndex = 0; //we're going to a series of copies because concat isn't too efficient
        int iLen = 0;

        for(int i = 0; i < m_arrEntries.GetSize(); i++)
        {
            iLen = m_oStr->GetLength(m_arrEntries[i]->szEntryValue);
            memcpy(&szText[iIndex], m_arrEntries[i]->szEntryValue, sizeof(TCHAR)*iLen);
            iIndex+=iLen;
            //comma of course
            szText[iIndex]=_T(',');
            iIndex++;
        }

        //don't forget the stop char
        szText[iIndex] = _T('\0');

        //write to the file
        fwrite(szText, sizeof(TCHAR), iTextLen, fp);

        m_oStr->Delete(&szText);
        szText = NULL;

        //close the file
        fclose(fp);
    }


#endif
    return TRUE;
}