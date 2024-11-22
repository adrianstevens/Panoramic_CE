#pragma once
#include "dlgwsheet.h"
#include "IssProfitMargin.h"

class CDlgWSheetProfitMargin :
    public CDlgWSheet
{
public:
    CDlgWSheetProfitMargin(void);
    ~CDlgWSheetProfitMargin(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();

private:
    CIssProfitMargin    m_oProfitMar;
};
