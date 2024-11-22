#pragma once
#include "dlgwsheet.h"

#include "IssFDateCalc.h"

class CDlgWSheetDate : public CDlgWSheet
{
public:
    CDlgWSheetDate(void);
    ~CDlgWSheetDate(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues(){};
    void			SaveRegistryValues(){};

private:
    CIssFDateCalc   m_oDateCalc;
};
