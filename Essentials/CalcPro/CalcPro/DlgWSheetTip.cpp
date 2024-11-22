#include "DlgWSheetTip.h"
#include "IssLocalisation.h"

CDlgWSheetTip::CDlgWSheetTip(void)
{
}

CDlgWSheetTip::~CDlgWSheetTip(void)
{
}

BOOL CDlgWSheetTip::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 6; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_BillAmount), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TipPercent), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_NumPeople), m_hInst));
    m_arrEntries[3]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TipAmount), m_hInst));
    m_arrEntries[4]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_TotalPer), m_hInst));
    m_arrEntries[5]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_BillTotal), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Currency;
    m_arrEntries[1]->eEntry = ENTRY_Percent;
    m_arrEntries[2]->eEntry = ENTRY_Int;
    m_arrEntries[3]->eEntry = ENTRY_Currency;
    m_arrEntries[4]->eEntry = ENTRY_Currency;
    m_arrEntries[5]->eEntry = ENTRY_Currency;

    m_arrEntries[3]->bGreyed = TRUE;
    m_arrEntries[4]->bGreyed = TRUE;
    m_arrEntries[5]->bGreyed = TRUE;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_TipCalculator), m_hInst));

    return TRUE;
}

void CDlgWSheetTip::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);
    double  dbDouble = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case 0:
            m_oTip.SetAmount(m_arrEntries[iSwitch]->szEntryValue);
            break;
        case 1:
            m_oTip.SetPercentage(dbDouble);
            break;
        case 2:
            m_oTip.SetNumberofPeople(iTemp-1);
            break;	
        case 3:
        case 4:
        case 5:
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetTip::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}


BOOL CDlgWSheetTip::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;

    switch(iTemp) 
    {
    default://move the screen to the bottom
        {
            int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
            int		iTemp2 = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

            switch(iTemp)
            {
            case 0:
                m_oTip.SetAmount(m_arrEntries[iSwitch]->szEntryValue);
                break;
            case 1:
                m_oTip.SetPercentage(iTemp2);
                break;
            case 2:
                m_oTip.SetNumberofPeople(iTemp2-1);
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
    case 5:
        m_oTip.CalculateResults();
        m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oTip.GetTipAmount());
        m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oTip.GetTotalPerPerson());
        m_oStr->DoubleToString(m_arrEntries[5]->szEntryValue, m_oTip.GetTotal());
        m_oCalc->AddString(m_arrEntries[5]->szEntryValue);
        break;
    case 3:
        //calc the results
        m_oTip.CalculateResults();
        m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oTip.GetTipAmount());
        m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oTip.GetTotalPerPerson());
        m_oStr->DoubleToString(m_arrEntries[5]->szEntryValue, m_oTip.GetTotal());
        m_oCalc->AddString(m_arrEntries[3]->szEntryValue);
        break;
    case 4:
        m_oTip.CalculateResults();
        m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oTip.GetTipAmount());
        m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oTip.GetTotalPerPerson());
        m_oStr->DoubleToString(m_arrEntries[5]->szEntryValue, m_oTip.GetTotal());
        m_oCalc->AddString(m_arrEntries[4]->szEntryValue);
        break;
    }
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    CDlgWSheet::OnCalc();

    return TRUE;
}

BOOL CDlgWSheetTip::OnReset()
{
    m_oTip.Reset();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oTip.GetBillAmount());
    m_oStr->IntToString(m_arrEntries[1]->szEntryValue, m_oTip.GetPercent());
    m_oStr->IntToString(m_arrEntries[2]->szEntryValue, m_oTip.GetNumberOfPeople());
    m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oTip.GetTipAmount());
    m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oTip.GetTotalPerPerson());
    m_oStr->DoubleToString(m_arrEntries[5]->szEntryValue, m_oTip.GetTotal());

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}