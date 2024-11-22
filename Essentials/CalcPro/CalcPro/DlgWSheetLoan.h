#pragma once
#include "dlgwsheet.h"
#include "IssCarLoanCalc.h"

class CDlgWSheetLoan : public CDlgWSheet
{
public:
    CDlgWSheetLoan(void);
    ~CDlgWSheetLoan(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void            CalcLoanValues();

private:
    CIssCarLoanCalc     m_oLoan;
};
