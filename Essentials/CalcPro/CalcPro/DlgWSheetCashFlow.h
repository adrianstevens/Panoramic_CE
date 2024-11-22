#pragma once
#include "dlgwsheet.h"

#include "IssCashFlow.h"

class CDlgWSheetCashFlow :
    public CDlgWSheet
{
public:
    CDlgWSheetCashFlow(void);
    ~CDlgWSheetCashFlow(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam){return UNHANDLED;};

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues(){};
    void			SaveRegistryValues(){};

private:
    CIssCashFlow    m_oCashFlow;
};
