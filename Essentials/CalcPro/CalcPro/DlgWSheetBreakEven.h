#pragma once
#include "dlgwsheet.h"

#include "IssBreakEven.h"

class CDlgWSheetBreakEven : public CDlgWSheet
{
public:
    CDlgWSheetBreakEven(void);
    ~CDlgWSheetBreakEven(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();
    
    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();
   

private:
    CIssBreakEven       m_oBreakEven;
};
