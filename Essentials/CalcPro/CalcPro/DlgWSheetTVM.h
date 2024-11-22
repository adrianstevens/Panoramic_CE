#pragma once
#include "dlgwsheet.h"
#include "IssTVM.h"

enum ScreenEntries
{
    TVM_N,
    TVM_IY,
    TVM_PV,
    TVM_PMT,
    TVM_FV,
    TVM_PY,
    TVM_CY,
    TVM_Count,
};

class CDlgWSheetTVM :
    public CDlgWSheet
{
public:
    CDlgWSheetTVM(void);
    ~CDlgWSheetTVM(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();
private:
    CIssTVM         m_oTVM;
};
