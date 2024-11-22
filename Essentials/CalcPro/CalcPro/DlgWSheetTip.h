#pragma once
#include "dlgwsheet.h"
#include "IssTipCalculator.h"

class CDlgWSheetTip :
    public CDlgWSheet
{
public:
    CDlgWSheetTip(void);
    ~CDlgWSheetTip(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues(){};
    void			SaveRegistryValues(){};


private:
    CIssTipCalculator   m_oTip;

};
