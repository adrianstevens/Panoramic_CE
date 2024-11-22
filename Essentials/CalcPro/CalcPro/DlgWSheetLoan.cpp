#include "DlgWSheetLoan.h"
#include "IssLocalisation.h"

CDlgWSheetLoan::CDlgWSheetLoan(void)
{
}

CDlgWSheetLoan::~CDlgWSheetLoan(void)
{
}

BOOL CDlgWSheetLoan::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 10; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Price), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_SalesTax), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_OtherFees), m_hInst));
    m_arrEntries[3]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_DownPayment), m_hInst));
    m_arrEntries[4]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TradeIn), m_hInst));
    m_arrEntries[5]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_NumberOfMonths), m_hInst));
    m_arrEntries[6]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_InterestRate), m_hInst));

    m_arrEntries[7]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_MonthlyPayment), m_hInst));
    m_arrEntries[8]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalInterest), m_hInst));
    m_arrEntries[9]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalRepaid), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Currency;
    m_arrEntries[1]->eEntry = ENTRY_Percent;
    m_arrEntries[2]->eEntry = ENTRY_Currency;
    m_arrEntries[3]->eEntry = ENTRY_Currency;
    m_arrEntries[4]->eEntry = ENTRY_Currency;
    m_arrEntries[5]->eEntry = ENTRY_Int;
    m_arrEntries[6]->eEntry = ENTRY_Percent;

    m_arrEntries[7]->eEntry = ENTRY_Currency;
    m_arrEntries[8]->eEntry = ENTRY_Currency;
    m_arrEntries[9]->eEntry = ENTRY_Currency;

    m_arrEntries[0]->bGreyed = FALSE;
    m_arrEntries[1]->bGreyed = FALSE;
    m_arrEntries[2]->bGreyed = FALSE;
    m_arrEntries[3]->bGreyed = FALSE;
    m_arrEntries[4]->bGreyed = FALSE;
    m_arrEntries[5]->bGreyed = FALSE;
    m_arrEntries[6]->bGreyed = FALSE;
    m_arrEntries[7]->bGreyed = TRUE;
    m_arrEntries[8]->bGreyed = TRUE;
    m_arrEntries[9]->bGreyed = TRUE;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_LoanCalculator), m_hInst));

    CalcLoanValues();

    return TRUE;
}

void CDlgWSheetLoan::CalcLoanValues()
{
    m_oLoan.CalculateValues();
    m_oStr->DoubleToString(m_arrEntries[7]->szEntryValue, m_oLoan.GetMonthlyPayments(NULL));
    m_oStr->DoubleToString(m_arrEntries[8]->szEntryValue, m_oLoan.GetTotalInterest(NULL));
    m_oStr->DoubleToString(m_arrEntries[9]->szEntryValue, m_oLoan.GetTotalCost(NULL));
}

BOOL CDlgWSheetLoan::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    double dbTemp = 0;

    switch(iTemp) 
    {
    default://move the screen to the bottom
        {
            int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
            int		iTemp2 = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);
            double  dbTemp2 = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
            switch(iTemp)
            {
            case 0:
                m_oLoan.SetCarPrice(dbTemp2);
                break;
            case 1:
                m_oLoan.SetSalesTaxPcnt(dbTemp2);
                break;
            case 2:
                m_oLoan.SetOtherFees(dbTemp2);
                break;
            case 3:
                m_oLoan.SetDownPayment(dbTemp2);
                break;
            case 4:
                m_oLoan.SetTradeIn(dbTemp2);
                break;
            case 5:
                m_oLoan.SetNumberOfMonths(iTemp2);
                break;
            case 6:
                m_oLoan.SetInterestRatePcnt(dbTemp2);
                break;
            }
        }

        if(m_iLinesOnscreen >= m_arrEntries.GetSize())
        {
            m_iEntryIndex = 0;
            m_iScreenIndex = m_arrEntries.GetSize()-1;
        }
        else
        {   //scroll to the bottom but keep it on screen
            m_iEntryIndex = m_arrEntries.GetSize() - m_iLinesOnscreen;
            m_iScreenIndex = m_iLinesOnscreen - 1;
        }
    case 9:
        CalcLoanValues();
        m_oCalc->AddString(m_arrEntries[9]->szEntryValue);
        break;
    case 8:
        CalcLoanValues();
        m_oCalc->AddString(m_arrEntries[8]->szEntryValue);
        break;
    case 7:
        CalcLoanValues();
        m_oCalc->AddString(m_arrEntries[7]->szEntryValue);
        break;

    }
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    CDlgWSheet::OnCalc();

    return TRUE;
}

void CDlgWSheetLoan::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp2 = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp2 = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case 0:
            m_oLoan.SetCarPrice(dbTemp2);
            break;
        case 1:
            m_oLoan.SetSalesTaxPcnt(dbTemp2);
            break;
        case 2:
            m_oLoan.SetOtherFees(dbTemp2);
            break;
        case 3:
            m_oLoan.SetDownPayment(dbTemp2);
            break;
        case 4:
            m_oLoan.SetTradeIn(dbTemp2);
            break;
        case 5:
            m_oLoan.SetNumberOfMonths(iTemp2);
            break;
        case 6:
            m_oLoan.SetInterestRatePcnt(dbTemp2);
            break;
        case 7:
        case 8:
        case 9:
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Error"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetLoan::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}

BOOL CDlgWSheetLoan::OnReset()
{
    m_oLoan.ResetValues();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oLoan.GetCarPrice(NULL));
    m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue, m_oLoan.GetSalesTaxPcnt(NULL));
    m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oLoan.GetOtherFees(NULL));
    m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oLoan.GetDownPayment(NULL));
    m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oLoan.GetTradeIn(NULL));
    m_oStr->IntToString(m_arrEntries[5]->szEntryValue, m_oLoan.GetNumberOfMonths(NULL));
    m_oStr->DoubleToString(m_arrEntries[6]->szEntryValue, m_oLoan.GetInterestRatePcnt(NULL));

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}