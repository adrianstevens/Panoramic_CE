#include "DlgWSheetTVM.h"
#include "globals.h"
#include "IssLocalisation.h"

CDlgWSheetTVM::CDlgWSheetTVM(void)
{
    LoadRegistryValues();

    m_oTVM.SetBegYearPayments(!g_bTVMEndPay);
}

CDlgWSheetTVM::~CDlgWSheetTVM(void)
{
    SaveRegistryValues();
}


BOOL CDlgWSheetTVM::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < (int)TVM_Count; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[TVM_N]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_NumTerms), m_hInst));
    m_arrEntries[TVM_IY]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_InterestYear), m_hInst));
    m_arrEntries[TVM_PV]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_PresentValue), m_hInst));
    m_arrEntries[TVM_PMT]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Payment), m_hInst));
    m_arrEntries[TVM_FV]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_FutureValue), m_hInst));
    m_arrEntries[TVM_PY]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_PaymentsYear), m_hInst));
    m_arrEntries[TVM_CY]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_CompYear), m_hInst));

    m_arrEntries[TVM_N]->eEntry	    = ENTRY_Int;
    m_arrEntries[TVM_IY]->eEntry	= ENTRY_Percent;
    m_arrEntries[TVM_PV]->eEntry	= ENTRY_Currency;
    m_arrEntries[TVM_PMT]->eEntry   = ENTRY_Currency;
    m_arrEntries[TVM_FV]->eEntry	= ENTRY_Currency;
    m_arrEntries[TVM_PY]->eEntry	= ENTRY_Int;
    m_arrEntries[TVM_CY]->eEntry	= ENTRY_Int;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_TVM), m_hInst));

    return TRUE;
}

void CDlgWSheetTVM::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case TVM_N:
            m_oTVM.SetNumberOfPeriods(iTemp);
            break;
        case TVM_IY:
            m_oTVM.SetInterestRateYear(dbTemp);
            break;
        case TVM_PV:
            m_oTVM.SetPresentValue(dbTemp);
            break;	
        case TVM_PMT:
            m_oTVM.SetPayment(dbTemp);
            break;
        case TVM_FV:
            m_oTVM.SetFutureValue(dbTemp);
            break;
        case TVM_PY:
            m_oTVM.SetPaymentsPerYear(iTemp);
            break;
        case TVM_CY:
            m_oTVM.SetCompPMTPerYear(iTemp);
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetTVM::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
     if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}



void CDlgWSheetTVM::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double dbTemp = 0.0;
    int	iTemp=0;

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_NTrm"), (LPBYTE)&iTemp, dwSize))
        m_oTVM.SetNumberOfPeriods(iTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_PY"), (LPBYTE)&iTemp, dwSize))
        m_oTVM.SetPaymentsPerYear(iTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_CY"), (LPBYTE)&iTemp, dwSize))
        m_oTVM.SetCompPMTPerYear(iTemp);

    dwSize = sizeof(double);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_IYR"), (LPBYTE)&dbTemp, dwSize))
        m_oTVM.SetInterestRateYear(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_PVal"), (LPBYTE)&dbTemp, dwSize))
        m_oTVM.SetPresentValue(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_PMT"), (LPBYTE)&dbTemp, dwSize))
        m_oTVM.SetPayment(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSTVM_FVal"), (LPBYTE)&dbTemp, dwSize))
        m_oTVM.SetFutureValue(dbTemp);
}


void CDlgWSheetTVM::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double	dbTemp;
    int  iTemp;

    iTemp = m_oTVM.RecallNumberOfPeriods();
    SetKey(REG_WorkSheets, _T("WSTVM_NTrm"), (LPBYTE)&iTemp, dwSize);

    iTemp = m_oTVM.RecallNumberOfPaymentsPerYear();
    SetKey(REG_WorkSheets, _T("WSTVM_PY"), (LPBYTE)&iTemp, dwSize);

    iTemp = m_oTVM.RecallNumberOfCompPerYear();
    SetKey(REG_WorkSheets, _T("WSTVM_CY"), (LPBYTE)&iTemp, dwSize);

    dwSize = sizeof(double);

    dbTemp = m_oTVM.RecallInterestRateYear();
    SetKey(REG_WorkSheets, _T("WSTVM_IYR"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oTVM.RecallPresentValue();
    SetKey(REG_WorkSheets, _T("WSTVM_PVal"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oTVM.RecallPayment();
    SetKey(REG_WorkSheets, _T("WSTVM_PMT"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oTVM.RecallFutureValue();
    SetKey(REG_WorkSheets, _T("WSTVM_FVal"), (LPBYTE)&dbTemp, dwSize);
}

BOOL CDlgWSheetTVM::OnCalc()
{
//    m_oTVM.SetBegYearPayments(!g_Pref->bEndOfPeriodPayments);

    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    switch(iTemp) 
    {
    case TVM_N:
        m_oTVM.CalcNumberOfPeriods();
        m_oStr->IntToString(m_arrEntries[TVM_N]->szEntryValue, m_oTVM.RecallNumberOfPeriods());
        m_oCalc->AddString(m_arrEntries[TVM_N]->szEntryValue);
        break;
    case TVM_PMT:
        m_oTVM.CalcPayment();
        m_oStr->DoubleToString(	m_arrEntries[TVM_PMT]->szEntryValue, m_oTVM.RecallPayment());
        m_oCalc->AddString(m_arrEntries[TVM_PMT]->szEntryValue);
        break;
    case TVM_PV:
        m_oTVM.CalcPresentValue();
        m_oStr->DoubleToString(m_arrEntries[TVM_PV]->szEntryValue , m_oTVM.RecallPresentValue());
        m_oCalc->AddString(m_arrEntries[TVM_PV]->szEntryValue);
        break;
    case TVM_FV:
        m_oTVM.CalcFutureValue();
        m_oStr->DoubleToString(m_arrEntries[TVM_FV]->szEntryValue, m_oTVM.RecallFutureValue());
        m_oCalc->AddString(m_arrEntries[TVM_FV]->szEntryValue);
        break;
    case TVM_IY:
        m_oTVM.CalcInterestRateYear();
        m_oStr->DoubleToString(m_arrEntries[TVM_IY]->szEntryValue, m_oTVM.RecallInterestRateYear());
        m_oCalc->AddString(m_arrEntries[TVM_IY]->szEntryValue);
        break;
    default:
        return FALSE;
        break;
    }
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}

BOOL CDlgWSheetTVM::OnReset()
{
    m_oTVM.Reset();
    m_iEntryIndex = 0;
    m_iScreenIndex = 0;

    m_oStr->IntToString(m_arrEntries[TVM_N]->szEntryValue, m_oTVM.RecallNumberOfPeriods());
    m_oStr->DoubleToString(m_arrEntries[TVM_IY]->szEntryValue, m_oTVM.RecallInterestRateYear());
    m_oStr->DoubleToString(m_arrEntries[TVM_PV]->szEntryValue , m_oTVM.RecallPresentValue());
    m_oStr->DoubleToString(	m_arrEntries[TVM_PMT]->szEntryValue, m_oTVM.RecallPayment());
    m_oStr->DoubleToString(m_arrEntries[TVM_FV]->szEntryValue, m_oTVM.RecallFutureValue());
    m_oStr->IntToString(m_arrEntries[TVM_CY]->szEntryValue, m_oTVM.RecallNumberOfCompPerYear());
    m_oStr->IntToString(m_arrEntries[TVM_PY]->szEntryValue, m_oTVM.RecallNumberOfPaymentsPerYear());

    m_oCalc->AddString(m_arrEntries[TVM_N]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}