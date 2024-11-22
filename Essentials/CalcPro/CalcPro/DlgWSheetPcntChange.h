#pragma once
#include "dlgwsheet.h"
#include "IssPecentChange.h"


class CDlgWSheetPcntChange : public CDlgWSheet
{
public:
    CDlgWSheetPcntChange(void);
    ~CDlgWSheetPcntChange(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();

private:
    CIssPecentChange    m_oPChange;
};
