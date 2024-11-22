#include "DlgWSheetDate.h"
#include "IssLocalisation.h"


CDlgWSheetDate::CDlgWSheetDate(void)
{
    LoadRegistryValues();
}

CDlgWSheetDate::~CDlgWSheetDate(void)
{
    SaveRegistryValues();
}


BOOL CDlgWSheetDate ::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 8; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartYear), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartMonth), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartDay), m_hInst));
    m_arrEntries[3]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_EndYear), m_hInst));
    m_arrEntries[4]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_EndMonth), m_hInst));
    m_arrEntries[5]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_EndDay), m_hInst));
    m_arrEntries[6]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Difference), m_hInst));
    m_arrEntries[7]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Diff360), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Int;
    m_arrEntries[1]->eEntry = ENTRY_Int;
    m_arrEntries[2]->eEntry = ENTRY_Int;
    m_arrEntries[3]->eEntry = ENTRY_Int;
    m_arrEntries[4]->eEntry = ENTRY_Int;
    m_arrEntries[5]->eEntry = ENTRY_Int;
    m_arrEntries[6]->eEntry = ENTRY_Int;
    m_arrEntries[7]->eEntry = ENTRY_Int;

    m_arrEntries[6]->bGreyed = TRUE;
    m_arrEntries[7]->bGreyed = TRUE;

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_DateCalculator), m_hInst));

    OnReset();

    return TRUE;
}

void CDlgWSheetDate::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case 0:
            m_oDateCalc.SetStartYear(iTemp);
            break;
        case 1:
            m_oDateCalc.SetStartMonth(iTemp);
            break;
        case 2:
            m_oDateCalc.SetStartDay(iTemp);
            break;
        case 3:
            m_oDateCalc.SetEndYear(iTemp);
            break;
        case 4:
            m_oDateCalc.SetEndMonth(iTemp);
            break;
        case 6:
        case 7:
            break;
        case 5:
            m_oDateCalc.SetEndDay(iTemp);
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Error"), MB_ICONERROR);
            break;
    }


}

BOOL CDlgWSheetDate::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
    {
        SetScreenValue();
    }
    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}

BOOL CDlgWSheetDate::OnCalc()
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
                m_oDateCalc.SetStartYear(iTemp2);
                break;
            case 1:
                m_oDateCalc.SetStartMonth(iTemp2);
                break;
            case 2:
                m_oDateCalc.SetStartDay(iTemp2);
                break;
            case 3:
                m_oDateCalc.SetEndYear(iTemp2);
                break;
            case 4:
                m_oDateCalc.SetEndMonth(iTemp2);
                break;
            case 5:
                m_oDateCalc.SetEndDay(iTemp2);
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
    case 7:
        m_oStr->IntToString(m_arrEntries[6]->szEntryValue, m_oDateCalc.CalcNormal());
        m_oStr->DoubleToString(m_arrEntries[7]->szEntryValue, m_oDateCalc.Calc30360());
        m_oCalc->AddString(m_arrEntries[7]->szEntryValue);
        break;
    case 6:
        m_oStr->IntToString(m_arrEntries[6]->szEntryValue, m_oDateCalc.CalcNormal());
        m_oStr->DoubleToString(m_arrEntries[7]->szEntryValue, m_oDateCalc.Calc30360());
        m_oCalc->AddString(m_arrEntries[6]->szEntryValue);
        break;
    }
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    CDlgWSheet::OnCalc();

    return TRUE;
}

BOOL CDlgWSheetDate::OnReset()
{
    m_oDateCalc.Clear();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->IntToString(m_arrEntries[0]->szEntryValue, m_oDateCalc.GetStartYear());
    m_oStr->IntToString(m_arrEntries[1]->szEntryValue, m_oDateCalc.GetStartMonth());
    m_oStr->IntToString(m_arrEntries[2]->szEntryValue, m_oDateCalc.GetStartDay());
    m_oStr->IntToString(m_arrEntries[3]->szEntryValue, m_oDateCalc.GetEndYear());
    m_oStr->IntToString(m_arrEntries[4]->szEntryValue, m_oDateCalc.GetEndMonth());
    m_oStr->IntToString(m_arrEntries[5]->szEntryValue, m_oDateCalc.GetEndDay());
    m_oStr->IntToString(m_arrEntries[6]->szEntryValue, 0);
    m_oStr->IntToString(m_arrEntries[7]->szEntryValue, 0);

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); 

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}