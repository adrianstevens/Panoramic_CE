#pragma once
#include "dlgwsheet.h"
#include "IssPcntCompound.h"

enum EnumPcntScreenEntries
{
    PCNT_N,
    PCNT_IY,
    PCNT_PV,
    PCNT_FV,

};

class CDlgWSheetCompoundInt :
    public CDlgWSheet
{
public:
    CDlgWSheetCompoundInt(void);
    ~CDlgWSheetCompoundInt(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void            SetScreenValue();

private:
    BOOL            OnCalc();
    BOOL            OnReset();

    BOOL			InitScreenEntries();

    void			LoadRegistryValues();
    void			SaveRegistryValues();

private:
    //percent change if the number of periods is one 
    //otherwise its compound interest
    CIssPcntCompound        m_oPcnt;
};
