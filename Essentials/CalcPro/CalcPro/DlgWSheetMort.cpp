#include "DlgWSheetMort.h"
#include "IssDebug.h"
#include "globals.h"
#include "IssLocalisation.h"


CDlgWSheetMort::CDlgWSheetMort(void)
{
    m_sTemp = NULL;
}

CDlgWSheetMort::~CDlgWSheetMort(void)
{
    m_oMort.SetPaymentSchedule(g_ePaymentSched);
}

void CDlgWSheetMort::CalcMortValues()
{
    m_oMort.SetPaymentSchedule(g_ePaymentSched);
    BOOL bRet = m_oMort.FillTables();

    if(bRet == FALSE)
    {    ASSERT(0);}

#ifdef DEBUG
    int iTemp = m_oMort.GetNumberOfPayments();
    double dbTemp = m_oMort.GetInterest();
    dbTemp = m_oMort.GetTotal(GET_TOTAL_INTEREST);
    dbTemp = m_oMort.GetMonthlyPaymentAmt();
    dbTemp = m_oMort.GetTotal(GET_GRAND_TOTAL);
#endif

    m_oStr->IntToString(m_arrEntries[5]->szEntryValue, m_oMort.GetNumberOfPayments());
    m_oStr->DoubleToString(m_arrEntries[6]->szEntryValue, m_oMort.GetTotal(GET_TOTAL_INTEREST));
    m_oStr->DoubleToString(m_arrEntries[7]->szEntryValue, m_oMort.GetMonthlyPaymentAmt());
    m_oStr->DoubleToString(m_arrEntries[8]->szEntryValue, m_oMort.GetTotal(GET_GRAND_TOTAL));
}

BOOL CDlgWSheetMort ::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 9; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Principal), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Terms), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_InterestRate), m_hInst));
    m_arrEntries[3]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_DownPayment), m_hInst));
    m_arrEntries[4]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_MonthlyFees), m_hInst));

    m_arrEntries[5]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalPayments), m_hInst));
    m_arrEntries[6]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalInterest), m_hInst));
    m_arrEntries[7]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_MonthlyPayment), m_hInst));
    m_arrEntries[8]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalRepaid), m_hInst));
    
    m_arrEntries[0]->eEntry = ENTRY_Currency;
    m_arrEntries[1]->eEntry = ENTRY_Int;
    m_arrEntries[2]->eEntry = ENTRY_Percent;
    m_arrEntries[3]->eEntry = ENTRY_Currency;
    m_arrEntries[4]->eEntry = ENTRY_Currency;
    m_arrEntries[5]->eEntry = ENTRY_Int;
    m_arrEntries[6]->eEntry = ENTRY_Currency;
    m_arrEntries[7]->eEntry = ENTRY_Currency;
    m_arrEntries[8]->eEntry = ENTRY_Currency;

    m_arrEntries[0]->bGreyed = FALSE;
    m_arrEntries[1]->bGreyed = FALSE;
    m_arrEntries[2]->bGreyed = FALSE;
    m_arrEntries[3]->bGreyed = FALSE;
    m_arrEntries[4]->bGreyed = FALSE;
    m_arrEntries[5]->bGreyed = TRUE;
    m_arrEntries[6]->bGreyed = TRUE;
    m_arrEntries[7]->bGreyed = TRUE;
    m_arrEntries[8]->bGreyed = TRUE;

    OnReset();

    //Title
    switch(g_ePaymentSched)
    {
    default:
    case PAYMENT_MONTHLY:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_Monthly), m_hInst));
        break;
    case PAYMENT_MONTHLY_ACCELERATED:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_MonthlyAccel), m_hInst));
        break;
    case PAYMENT_BIWEEKLY:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_BiWeekly), m_hInst));
        break;
    case PAYMENT_BIWEEKLY_ACCELERATED:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_BiWeeklyAccel), m_hInst));
        break;
    case PAYMENT_WEEKLY:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_Weekly), m_hInst));
        break;
    case PAYMENT_WEEKLY_ACCELERATED:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_WeeklyAccel), m_hInst));
        break;
    case PAYMENT_SEMI_MONTHLY:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_SemiMonthly), m_hInst));
        break;
    case PAYMENT_SEMI_MONTHLY_ACCELERATED:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_SemiMonthlyAccel), m_hInst));
        break;
    case PAYMENT_BIMONTHLY:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_BiMonthly), m_hInst));
        break;
    case PAYMENT_BIMONTHLY_ACCELERATED:
        m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_OPT_BiMonthlyAccel), m_hInst));
        break;
    }
        
    m_oStr->Insert(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_Mortgage), m_hInst));

    OnSize(m_hWnd, 0, 0);

    m_sTemp = m_arrEntries[2];

    return TRUE;
}

void CDlgWSheetMort::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case 0:
            m_oMort.SetPrinciple(dbTemp);
            break;
        case 1:
            m_oMort.SetTermInYears(iTemp);
            break;
        case 2:
            m_oMort.SetInterest(dbTemp);
            break;
        case 3:
            m_oMort.SetDownPayment(dbTemp);
            break;
        case 4:
            m_oMort.SetOtherRecurringCosts(dbTemp);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Error"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetMort::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);


    if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}

BOOL CDlgWSheetMort::OnCalc()
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
                m_oMort.SetPrinciple(dbTemp2);
                break;
            case 1:
                m_oMort.SetTermInYears(iTemp2);
                break;
            case 2:
                m_oMort.SetInterest(dbTemp2);
                break;
            case 3:
                m_oMort.SetDownPayment(dbTemp2);
                break;
            case 4:
                m_oMort.SetOtherRecurringCosts(dbTemp2);
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
    case 8:
        CalcMortValues();
        m_oCalc->AddString(m_arrEntries[8]->szEntryValue);
        break;
    case 7:
        CalcMortValues();
        m_oCalc->AddString(m_arrEntries[7]->szEntryValue);
        break;
    case 6:
        CalcMortValues();
        m_oCalc->AddString(m_arrEntries[6]->szEntryValue);
        break;
    case 5:
        CalcMortValues();   
        m_oCalc->AddString(m_arrEntries[5]->szEntryValue);
        break;
    }
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    CDlgWSheet::OnCalc();

    return TRUE;
}

BOOL CDlgWSheetMort::OnReset()
{
    m_oMort.Restart();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oMort.GetPrinciple());
    m_oStr->IntToString(m_arrEntries[1]->szEntryValue, m_oMort.GetTermInYears());
    m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oMort.GetInterest());
    m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oMort.GetDownPayment());
    m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oMort.GetOtherRecurringCosts());

    CalcMortValues();

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}