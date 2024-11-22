#include "DlgWSheetProfitMargin.h"
#include "IssLocalisation.h"

CDlgWSheetProfitMargin::CDlgWSheetProfitMargin(void)
{
    LoadRegistryValues();
}

CDlgWSheetProfitMargin::~CDlgWSheetProfitMargin(void)
{
    SaveRegistryValues();
}


BOOL CDlgWSheetProfitMargin ::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 3; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Cost), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Price), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Profit), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Currency;
    m_arrEntries[1]->eEntry = ENTRY_Currency;
    m_arrEntries[2]->eEntry = ENTRY_Percent;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_ProfitMargin), m_hInst));

    return TRUE;
}

void CDlgWSheetProfitMargin::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case 0:
            m_oProfitMar.SetCost(dbTemp);
            break;
        case 1:
            m_oProfitMar.SetSellPrice(dbTemp);
            break;	
        case 2:
            m_oProfitMar.SetProfitMargin(dbTemp);
            break;

        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetProfitMargin::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
       SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}


void CDlgWSheetProfitMargin::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(double);
    double dbTemp = 0.0;


    if(S_OK == GetKey(REG_WorkSheets, _T("WSProf_Cost"), (LPBYTE)&dbTemp, dwSize))
        m_oProfitMar.SetCost(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSProf_Price"), (LPBYTE)&dbTemp, dwSize))
        m_oProfitMar.SetSellPrice(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSProf_Profit"), (LPBYTE)&dbTemp, dwSize))
        m_oProfitMar.SetProfitMargin(dbTemp);



}

void CDlgWSheetProfitMargin::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(double);
    double	dbTemp;

    dbTemp = m_oProfitMar.GetCost();
    SetKey(REG_WorkSheets, _T("WSProf_Cost"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oProfitMar.GetSellPrice();
    SetKey(REG_WorkSheets, _T("WSProf_Price"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oProfitMar.GetProfitMargin();
    SetKey(REG_WorkSheets, _T("WSProf_Profit"), (LPBYTE)&dbTemp, dwSize);
}

BOOL CDlgWSheetProfitMargin::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    switch(iTemp) 
    {
    case 0:
        m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oProfitMar.CalcCost());
        m_oCalc->AddString(m_arrEntries[0]->szEntryValue);
        break;
    case 1:
        m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue, m_oProfitMar.CalcSellPrice());
        m_oCalc->AddString(m_arrEntries[1]->szEntryValue);
        break;
    case 2:
        m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oProfitMar.CalcProfit());
        m_oCalc->AddString(m_arrEntries[2]->szEntryValue);
        break;
    default:
        return FALSE;
        break;
    }
    //I'm thinking no point in opening the List Control for 4 entries....
    //lets open an answer DLG
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    return TRUE;
}

BOOL CDlgWSheetProfitMargin::OnReset()
{
    m_oProfitMar.Clear();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oProfitMar.GetCost());
    m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue , m_oProfitMar.GetSellPrice());
    m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oProfitMar.GetProfitMargin());

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}