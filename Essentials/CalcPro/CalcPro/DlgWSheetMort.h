#pragma once
#include "dlgwsheet.h"
#include "IssMortgage.h"

class CDlgWSheetMort :
    public CDlgWSheet
{
public:
    CDlgWSheetMort(void);
    ~CDlgWSheetMort(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();

    void            CalcMortValues();

private:

private:
    CCalcMortgage   m_oMort;
};
