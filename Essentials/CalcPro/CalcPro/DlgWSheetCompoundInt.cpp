#include "DlgWSheetCompoundInt.h"
#include "IssLocalisation.h"


CDlgWSheetCompoundInt::CDlgWSheetCompoundInt(void)
{
    LoadRegistryValues();
}

CDlgWSheetCompoundInt::~CDlgWSheetCompoundInt(void)
{
    SaveRegistryValues();
}

BOOL CDlgWSheetCompoundInt::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 4; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[PCNT_N]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_Periods), m_hInst));
    m_arrEntries[PCNT_IY]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_InterestRate), m_hInst));
    m_arrEntries[PCNT_PV]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_StartValue), m_hInst));
    m_arrEntries[PCNT_FV]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_FinalValue), m_hInst));

    m_arrEntries[PCNT_N]->eEntry = ENTRY_Int;
    m_arrEntries[PCNT_IY]->eEntry = ENTRY_Percent;
    m_arrEntries[PCNT_PV]->eEntry = ENTRY_Currency;
    m_arrEntries[PCNT_FV]->eEntry = ENTRY_Currency;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_CompoundInterest), m_hInst));

    OnSize(m_hWnd, 0, 0);

    return TRUE;
}

void CDlgWSheetCompoundInt::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);

    switch(iSwitch) 
    {
    case PCNT_N:
        m_oPcnt.SetNumPeriods(iTemp);
        break;
    case PCNT_IY:
        m_oPcnt.SetInterestRate(dbTemp);
        break;
    case PCNT_PV:
        m_oPcnt.SetOriginalValue(dbTemp);
        break;	
    case PCNT_FV:
        m_oPcnt.SetNewValue(dbTemp);
        break;
    default://Ooops if here
        MessageBox(NULL, m_oStr->GetText(ID(IDS_ERROR_IndexRange), m_hInst), m_oStr->GetText(ID(IDS_ERROR_PleaseContact), m_hInst), MB_ICONERROR);
        break;
    }
}

BOOL CDlgWSheetCompoundInt::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_DOWN || wParam == VK_UP)
    {
        SetScreenValue();
    }
    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}


void CDlgWSheetCompoundInt::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double dbTemp = 0.0;
    int	iTemp=0;

    if(S_OK == GetKey(REG_WorkSheets, _T("WSPC_PD"), (LPBYTE)&iTemp, dwSize))
        m_oPcnt.SetNumPeriods(iTemp);

    dwSize = sizeof(double);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSPC_CH"), (LPBYTE)&dbTemp, dwSize))
        m_oPcnt.SetInterestRate(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSPC_PVal"), (LPBYTE)&dbTemp, dwSize))
        m_oPcnt.SetOriginalValue(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSPC_FVal"), (LPBYTE)&dbTemp, dwSize))
        m_oPcnt.SetNewValue(dbTemp);



}

void CDlgWSheetCompoundInt::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double	dbTemp;
    int  iTemp;

    iTemp = m_oPcnt.GetNumPeriods();
    SetKey(REG_WorkSheets, _T("WSPC_PD"), (LPBYTE)&iTemp, dwSize);

    dwSize = sizeof(double);

    dbTemp = m_oPcnt.GetInterestRate();
    SetKey(REG_WorkSheets, _T("WSPC_CH"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oPcnt.GetOriginalValue();
    SetKey(REG_WorkSheets, _T("WSPC_PVal"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oPcnt.GetNewValue();
    SetKey(REG_WorkSheets, _T("WSPC_FVal"), (LPBYTE)&dbTemp, dwSize);
}

BOOL CDlgWSheetCompoundInt::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    switch(iTemp) 
    {
    case PCNT_N:
        m_oStr->IntToString(m_arrEntries[PCNT_N]->szEntryValue, m_oPcnt.CalcNumPeriods());
        m_oCalc->AddString(m_arrEntries[PCNT_N]->szEntryValue);
        break;
    case PCNT_IY:
        m_oStr->DoubleToString(m_arrEntries[PCNT_IY]->szEntryValue, m_oPcnt.CalcInterestRate());
        m_oCalc->AddString(m_arrEntries[PCNT_IY]->szEntryValue);
        break;
    case PCNT_PV:
        m_oStr->DoubleToString(m_arrEntries[PCNT_PV]->szEntryValue, m_oPcnt.CalcOriginalValue());
        m_oCalc->AddString(m_arrEntries[PCNT_PV]->szEntryValue);
        break;
    default:
    case PCNT_FV:
        m_oStr->DoubleToString(m_arrEntries[PCNT_FV]->szEntryValue, m_oPcnt.CalcNewValue());
        m_oCalc->AddString(m_arrEntries[PCNT_FV]->szEntryValue);
        break;
    }
    //I'm thinking no point in opening the List Control for 4 entries....
    //lets open an answer DLG
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    CDlgWSheet::OnCalc();

    return TRUE;
}

BOOL CDlgWSheetCompoundInt::OnReset()
{
    m_oPcnt.Clear();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;
    
    //we need to clear out the TCHARs too
    m_oStr->IntToString(m_arrEntries[PCNT_N]->szEntryValue, m_oPcnt.GetNumPeriods());
    m_oStr->DoubleToString(m_arrEntries[PCNT_IY]->szEntryValue, m_oPcnt.GetInterestRate());
    m_oStr->DoubleToString(m_arrEntries[PCNT_PV]->szEntryValue , m_oPcnt.GetOriginalValue());
    m_oStr->DoubleToString(	m_arrEntries[PCNT_FV]->szEntryValue, m_oPcnt.GetNewValue());

    m_oCalc->AddString(m_arrEntries[PCNT_N]->szEntryValue); //for N

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}