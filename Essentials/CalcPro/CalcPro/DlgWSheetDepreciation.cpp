#include "DlgWSheetDepreciation.h"
#include "IssLocalisation.h"



CDlgWSheetDepreciation::CDlgWSheetDepreciation(void)
{
    LoadRegistryValues();
}

CDlgWSheetDepreciation::~CDlgWSheetDepreciation(void)
{
    SaveRegistryValues();
}


BOOL CDlgWSheetDepreciation::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 9; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[DEP_Life]->szEntryLabel		= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Life), m_hInst));
    m_arrEntries[DEP_StartMonth]->szEntryLabel	= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartMonth), m_hInst));
    m_arrEntries[DEP_StartYear]->szEntryLabel	= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartYear), m_hInst));
    m_arrEntries[DEP_Cost]->szEntryLabel		= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Cost), m_hInst));
    m_arrEntries[DEP_Salvage]->szEntryLabel		= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Salary), m_hInst));
    m_arrEntries[DEP_Year]->szEntryLabel		= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Year), m_hInst));
    m_arrEntries[DEP_Dep]->szEntryLabel			= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Depreciation), m_hInst));
    m_arrEntries[DEP_RBV]->szEntryLabel			= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_RBVal), m_hInst));
    m_arrEntries[DEP_RDV]->szEntryLabel			= m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_RDVal), m_hInst));

    m_arrEntries[DEP_Life]->eEntry				= ENTRY_Int;
    m_arrEntries[DEP_StartMonth]->eEntry		= ENTRY_Int;
    m_arrEntries[DEP_StartYear]->eEntry			= ENTRY_Int;
    m_arrEntries[DEP_Cost]->eEntry				= ENTRY_Currency;
    m_arrEntries[DEP_Salvage]->eEntry			= ENTRY_Currency;
    m_arrEntries[DEP_Year]->eEntry				= ENTRY_Int;
    m_arrEntries[DEP_Dep]->eEntry				= ENTRY_Currency;
    m_arrEntries[DEP_RBV]->eEntry				= ENTRY_Currency;
    m_arrEntries[DEP_RDV]->eEntry				= ENTRY_Currency;

    m_arrEntries[DEP_Dep]->bGreyed				= TRUE;
    m_arrEntries[DEP_RBV]->bGreyed				= TRUE;
    m_arrEntries[DEP_RDV]->bGreyed				= TRUE;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_Depreciation), m_hInst));

    return TRUE;
}

void CDlgWSheetDepreciation::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
        case DEP_Life:
            m_oDep.SetAssetLife(iTemp);
            break;
        case DEP_StartMonth:
            m_oDep.SetStartMonth(iTemp);
            break;
        case DEP_StartYear:
            m_oDep.SetStartYear(iTemp);
            break;	
        case DEP_Cost:
            m_oDep.SetCostofAsset(dbTemp);
            break;
        case DEP_Salvage:
            m_oDep.SetSalvageValue(dbTemp);
            break;
        case DEP_Year:
            m_oDep.SetYearToCompute(iTemp);
            break;
        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Contact Panoramic Software Inc"), MB_ICONERROR);
        case DEP_Dep:
        case DEP_RBV:
        case DEP_RDV:
            break;
    }
}

BOOL CDlgWSheetDepreciation::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);


    if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();
        
    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}



void CDlgWSheetDepreciation::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double dbTemp = 0.0;
    int	iTemp=0;

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_Life"), (LPBYTE)&iTemp, dwSize))
        m_oDep.SetAssetLife(iTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_SMnth"), (LPBYTE)&iTemp, dwSize))
        m_oDep.SetStartMonth(iTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_SYear"), (LPBYTE)&iTemp, dwSize))
        m_oDep.SetStartYear(iTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_Year"), (LPBYTE)&iTemp, dwSize))
        m_oDep.SetYearToCompute(iTemp);

    dwSize = sizeof(double);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_Cost"), (LPBYTE)&dbTemp, dwSize))
        m_oDep.SetCostofAsset(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSDep_Sal"), (LPBYTE)&dbTemp, dwSize))
        m_oDep.SetSalvageValue(dbTemp);
}

void CDlgWSheetDepreciation::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double	dbTemp;
    int  iTemp;

    iTemp = m_oDep.GetAssetLife();
    SetKey(REG_WorkSheets, _T("WSDep_Life"), (LPBYTE)&iTemp, dwSize);

    iTemp = m_oDep.GetStartMonth();
    SetKey(REG_WorkSheets, _T("WSDep_SMnth"), (LPBYTE)&iTemp, dwSize);

    iTemp = m_oDep.GetStartYear();
    SetKey(REG_WorkSheets, _T("WSDep_SYear"), (LPBYTE)&iTemp, dwSize);

    iTemp = m_oDep.GetYearToCompute();
    SetKey(REG_WorkSheets, _T("WSDep_Year"), (LPBYTE)&iTemp, dwSize);

    dwSize = sizeof(double);

    dbTemp = m_oDep.GetCostOfAsset();
    SetKey(REG_WorkSheets, _T("WSDep_Cost"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oDep.GetSalvageValue();
    SetKey(REG_WorkSheets, _T("WSDep_Sal"), (LPBYTE)&dbTemp, dwSize);
}

BOOL CDlgWSheetDepreciation::OnCalc()
{
    //make sure the current value is entered
    if(m_iScreenIndex > 0)
        m_iScreenIndex--;
    else if(m_iEntryIndex > 0)
        m_iScreenIndex--;

 //   OnBtnPressed(INPUT_WS, WS_Down);
    m_iEntryIndex = 6;
    m_iScreenIndex = 0;
   
    m_oDep.Calc();
    m_oStr->DoubleToString(m_arrEntries[DEP_Dep]->szEntryValue, m_oDep.GetDep());
    m_oStr->DoubleToString(m_arrEntries[DEP_RBV]->szEntryValue, m_oDep.GetRemainingBookValue());
    m_oStr->DoubleToString(m_arrEntries[DEP_RDV]->szEntryValue, m_oDep.GetRemainingDepValue());
    //now we need to set the calc engine value depending on what's currently selected
    {
        switch(m_iScreenIndex + m_iEntryIndex)
        {
        case DEP_Dep:
            m_oCalc->AddString(m_arrEntries[DEP_Dep]->szEntryValue);
            break;
        case DEP_RBV:
            m_oCalc->AddString(m_arrEntries[DEP_RBV]->szEntryValue);
            break;
        case DEP_RDV:
            m_oCalc->AddString(m_arrEntries[DEP_RDV]->szEntryValue);
            break;
        }
    }

    return TRUE;
}

BOOL CDlgWSheetDepreciation::OnReset()
{
    m_oDep.ClearWorksheet();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->IntToString(m_arrEntries[DEP_Life]->szEntryValue, m_oDep.GetAssetLife());
    m_oStr->IntToString(m_arrEntries[DEP_StartMonth]->szEntryValue, m_oDep.GetStartMonth());
    m_oStr->IntToString(m_arrEntries[DEP_StartYear]->szEntryValue, m_oDep.GetStartYear());
    m_oStr->DoubleToString(m_arrEntries[DEP_Cost]->szEntryValue, m_oDep.GetCostOfAsset());
    m_oStr->DoubleToString(m_arrEntries[DEP_Salvage]->szEntryValue, m_oDep.GetSalvageValue());
    m_oStr->IntToString(m_arrEntries[DEP_Year]->szEntryValue, m_oDep.GetYearToCompute());
    m_oStr->DoubleToString(m_arrEntries[DEP_Dep]->szEntryValue, m_oDep.GetDep());
    m_oStr->DoubleToString(m_arrEntries[DEP_RBV]->szEntryValue, m_oDep.GetRemainingBookValue());
    m_oStr->DoubleToString(m_arrEntries[DEP_RDV]->szEntryValue, m_oDep.GetRemainingDepValue());


    m_oCalc->AddString(m_arrEntries[DEP_Life]->szEntryValue);

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}