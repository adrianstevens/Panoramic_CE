#include "DlgWSheetCashFlow.h"

CDlgWSheetCashFlow::CDlgWSheetCashFlow(void)
{
    LoadRegistryValues();
}

CDlgWSheetCashFlow::~CDlgWSheetCashFlow(void)
{
    SaveRegistryValues();
}



BOOL CDlgWSheetCashFlow::InitScreenEntries()
{
    DeleteEntryArray();

    for(int i = 0; i < 5; i++)
    {
        TypeEntry* sEntry = new TypeEntry;
        m_arrEntries.AddElement(sEntry);
    }

    OnSize(m_hWnd, 0, 0);

    return TRUE;
}

BOOL CDlgWSheetCashFlow::OnCalc()
{

    return FALSE;
}

BOOL CDlgWSheetCashFlow::OnReset()
{
    m_oCashFlow.Clear();
    m_iScreenIndex = 0;
    m_iEntryIndex = 0;
    InvalidateRect(m_hWnd, &m_rcDisplay, FALSE);
    return TRUE;
}
