#include "DlgWSheetPcntChange.h"
#include "DlgWSheetCompoundInt.h"
#include "IssLocalisation.h"


CDlgWSheetPcntChange::CDlgWSheetPcntChange(void)
{
    LoadRegistryValues();
}

CDlgWSheetPcntChange::~CDlgWSheetPcntChange(void)
{
    SaveRegistryValues();
}


BOOL CDlgWSheetPcntChange ::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 3; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        sEntry->bGreyed = FALSE;
        m_arrEntries.AddElement(sEntry);
    }

    m_arrEntries[0]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_PeriodsYear), m_hInst));
    m_arrEntries[1]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_NominalRate), m_hInst));
    m_arrEntries[2]->szEntryLabel = m_oStr->CreateAndCopy(m_oStr->GetText(ID(IDS_INFO_EffectiveRate), m_hInst));

    m_arrEntries[0]->eEntry = ENTRY_Int;
    m_arrEntries[1]->eEntry = ENTRY_Percent;
    m_arrEntries[2]->eEntry = ENTRY_Percent;

    OnReset();

    m_oStr->StringCopy(m_szTitle, m_oStr->GetText(ID(IDS_TITLE_PercentChange), m_hInst));

    return TRUE;
}

void CDlgWSheetPcntChange::SetScreenValue()
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);


    switch(iSwitch) 
    {
        case PCNT_N:
            m_oPChange.SetPeriodsPerYr(iTemp);
            break;
        case PCNT_IY:
            m_oPChange.SetNominalRate(dbTemp);
            break;
        case PCNT_PV:
            m_oPChange.SetEffectiveRate(dbTemp);
            break;	

        default://Ooops if here
            //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            break;
    }
}

BOOL CDlgWSheetPcntChange::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int		iSwitch = m_iEntryIndex+m_iScreenIndex; //its just less typing
    double	dbTemp = m_oStr->StringToDouble(m_arrEntries[iSwitch]->szEntryValue);
    int		iTemp = m_oStr->StringToInt(m_arrEntries[iSwitch]->szEntryValue);


    if(wParam == VK_DOWN || wParam == VK_UP)
        SetScreenValue();

    return CDlgWSheet::OnKeyUp(hWnd, wParam, lParam);//call the base class after we set our values
}


void CDlgWSheetPcntChange::LoadRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double	dbTemp = 0.0;
    int		iTemp=0;

    if(S_OK == GetKey(REG_WorkSheets, _T("WSICon_CY"), (LPBYTE)&iTemp, dwSize))
        m_oPChange.SetPeriodsPerYr(iTemp);

    dwSize      = sizeof(double);
    if(S_OK == GetKey(REG_WorkSheets, _T("WSICon_NOM"), (LPBYTE)&dbTemp, dwSize))
        m_oPChange.SetNominalRate(dbTemp);

    if(S_OK == GetKey(REG_WorkSheets, _T("WSICon_EFF"), (LPBYTE)&dbTemp, dwSize))
        m_oPChange.SetEffectiveRate(dbTemp);
}

void CDlgWSheetPcntChange::SaveRegistryValues()
{
    DWORD   dwSize      = sizeof(int);
    double	dbTemp;
    int		iTemp;

    iTemp = m_oPChange.GetPeriodsPerYr();
    SetKey(REG_WorkSheets, _T("WSICon_CY"), (LPBYTE)&iTemp, dwSize);

    dwSize      = sizeof(double);
    dbTemp = m_oPChange.GetNominalRate();
    SetKey(REG_WorkSheets, _T("WSICon_NOM"), (LPBYTE)&dbTemp, dwSize);

    dbTemp = m_oPChange.GetEffectiveRate();
    SetKey(REG_WorkSheets, _T("WSICon_EFF"), (LPBYTE)&dbTemp, dwSize);
}

BOOL CDlgWSheetPcntChange::OnCalc()
{
    //calc whatever value is selected
    int iTemp = m_iEntryIndex + m_iScreenIndex;
    switch(iTemp) 
    {
    case 1:
        m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue, m_oPChange.CalcNominalRate());
        m_oCalc->AddString(m_arrEntries[1]->szEntryValue);
        break;
    case 2:
        m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue, m_oPChange.CalcEffectiveRate());
        m_oCalc->AddString(m_arrEntries[2]->szEntryValue);
        break;
    }
    //I'm thinking no point in opening the List Control for 4 entries....
    //lets open an answer DLG
    //just set the appropriate answer
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);

    return TRUE;
}

BOOL CDlgWSheetPcntChange::OnReset()
{
    m_oPChange.Clear();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;

    m_oStr->IntToString(m_arrEntries[0]->szEntryValue,m_oPChange.GetPeriodsPerYr());
    m_oStr->DoubleToString(m_arrEntries[1]->szEntryValue, m_oPChange.GetNominalRate());
    m_oStr->DoubleToString(m_arrEntries[2]->szEntryValue , m_oPChange.GetEffectiveRate());

    m_oCalc->AddString(m_arrEntries[0]->szEntryValue); //for N

    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}