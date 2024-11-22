#include "DlgWSheetBreakEven.h"
#include "IssLocalisation.h"


CDlgWSheetBreakEven::CDlgWSheetBreakEven(void)
{
    LoadRegistryValues();
}

CDlgWSheetBreakEven::~CDlgWSheetBreakEven(void)
{
    SaveRegistryValues();
}

BOOL CDlgWSheetBreakEven ::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 5; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Profit), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Price), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_FixedCost), m_hInst));
    m_arrEntries[3]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_VarCost), m_hInst));
    m_arrEntries[4]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Quantity), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Currency;
    m_arrEntries[1]->eEntry = ENTRY_Currency;
    m_arrEntries[2]->eEntry = ENTRY_Currency;
    m_arrEntries[3]->eEntry = ENTRY_Currency;
    m_arrEntries[4]->eEntry = ENTRY_Double;

    m_arrEntries[0]->bGreyed = FALSE;
    m_arrEntries[1]->bGreyed = FALSE;
    m_arrEntries[2]->bGreyed = FALSE;
    m_arrEntries[3]->bGreyed = FALSE;
    m_arrEntries[4]->bGreyed = FALSE;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_BreakEvenSales), m_hInst));

    OnSize(m_hWnd, 0, 0);

    return TRUE;
}


BOOL CDlgWSheetBreakEven::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
    {
        SetScreenValue();
    }
    
    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}

void CDlgWSheetBreakEven::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(double);
    double dbTemp = 0.0;
    //	int	iTemp=0;

    if(S_OK == GetKey(REG_WorkSheets, _T("WSBrkE_PFT"), (LPBYTE)&dbTemp, dwSize))
        m_oBreakEven.SetProfit(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSBrkE_Price"), (LPBYTE)&dbTemp, dwSize))
        m_oBreakEven.SetUnitPrice(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSBrkE_FCost"), (LPBYTE)&dbTemp, dwSize))
        m_oBreakEven.SetFixedCost(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSBrkE_VCost"), (LPBYTE)&dbTemp, dwSize))
        m_oBreakEven.SetVariableCostPer(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSBrkE_Qnty"), (LPBYTE)&dbTemp, dwSize))
        m_oBreakEven.SetQuantity(dbTemp);

}

void CDlgWSheetBreakEven::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(double);
    double	dbTemp;

    dbTemp = m_oBreakEven.GetProfit();
    SetKey(REG_WorkSheets, _T("WSBrkE_PFT"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oBreakEven.GetUnitPrice();
    SetKey(REG_WorkSheets, _T("WSBrkE_Price"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oBreakEven.GetFixedCost();
    SetKey(REG_WorkSheets, _T("WSBrkE_FCost"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oBreakEven.GetVariableCost();
    SetKey(REG_WorkSheets, _T("WSBrkE_VCost"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oBreakEven.GetQuantity();
    SetKey(REG_WorkSheets, _T("WSBrkE_Qnty"), (LPBYTE)&dbTemp, dwSize);
}

void CDlgWSheetBreakEven::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
    case 0:
        m_oBreakEven.SetProfit(dbTemp);
        break;
    case 1:
        m_oBreakEven.SetUnitPrice(dbTemp);
        break;
    case 2:
        m_oBreakEven.SetFixedCost(dbTemp);
        break;	
    case 3:
        m_oBreakEven.SetVariableCostPer(dbTemp);
        break;
    case 4:
        m_oBreakEven.SetQuantity(dbTemp);
        break;
    default://Ooops if here
        MessageBox(NULL, m_oStr->GetText(ID(IDS_ERROR_IndexRange), m_hInst), m_oStr->GetText(ID(IDS_ERROR_PleaseContact), m_hInst), MB_ICONERROR);
        break;
    }
}

BOOL CDlgWSheetBreakEven::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    double dbTemp = 0;

    TCHAR* szTemp = NULL;

    switch(iTemp) 
    {
    case 0:
        szTemp = m_arrEntries[0]->szEntryValue;
        if(szTemp == NULL)
            return FALSE;
        dbTemp = m_oBreakEven.CalcProfit();
        break;
    case 1:
        szTemp = m_arrEntries[1]->szEntryValue;
        if(szTemp == NULL)
            return FALSE;
        dbTemp = m_oBreakEven.CalcUnitPrice();
        
        break;
    case 2:
        szTemp = m_arrEntries[2]->szEntryValue;
        if(szTemp == NULL)
            return FALSE;
        dbTemp = m_oBreakEven.CalcFixedCost();
        break;
    case 3:
        szTemp = m_arrEntries[3]->szEntryValue;
        if(szTemp == NULL)
            return FALSE;
        dbTemp = m_oBreakEven.CalcVariableCostPer();
        break;
    case 4:
        szTemp = m_arrEntries[4]->szEntryValue;
        if(szTemp == NULL)
            return FALSE;
        dbTemp = m_oBreakEven.CalcQuantity();
        
        break;
    default:
        return FALSE;
    }
    
    TCHAR szDouble[STRING_LARGE];

    m_oStr->DoubleToString(szDouble, dbTemp);
    m_oCalc->AddString(szDouble);
    m_oStr->StringCopy(szTemp, szDouble);//just keeping an eye on things ... leave here
    CDlgWSheet::OnCalc();

    return TRUE;
}

BOOL CDlgWSheetBreakEven::OnReset()
{
    m_oBreakEven.Clear();
    m_iEntryIndex = 0;
    m_iScreenIndex = 0;

    m_oStr->DoubleToString(m_arrEntries[0]->szEntryValue, m_oBreakEven.GetProfit());
    m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue, m_oBreakEven.GetUnitPrice());
    m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oBreakEven.GetFixedCost());
    m_oStr->DoubleToString(m_arrEntries[3]->szEntryValue, m_oBreakEven.GetVariableCost());
    m_oStr->DoubleToString(m_arrEntries[4]->szEntryValue, m_oBreakEven.GetQuantity());

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); //for N

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}