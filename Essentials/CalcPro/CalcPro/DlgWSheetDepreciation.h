#pragma once
#include "dlgwsheet.h"

#include "IssDrepreciation.h"

enum EnumDepScreenEntries
{
    DEP_Life,
    DEP_StartMonth,
    DEP_StartYear,
    DEP_Cost,
    DEP_Salvage,
    DEP_Year,
    DEP_Dep,
    DEP_RBV,
    DEP_RDV,
};

class CDlgWSheetDepreciation : public CDlgWSheet
{
public:
    CDlgWSheetDepreciation(void);
    ~CDlgWSheetDepreciation(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();

private:
    CIssDepreciation    m_oDep;
};
