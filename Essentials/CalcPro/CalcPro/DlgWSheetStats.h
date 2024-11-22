#pragma once
#include "dlgwsheet.h"
#include "IssStats.h"

class CDlgWSheetStats :
    public CDlgWSheet
{
public:
    CDlgWSheetStats(LaunchType eWS);
    ~CDlgWSheetStats(void);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL            OnCalc();
    BOOL            OnReset();
    BOOL            OnGraph();
    BOOL            BuildLineEq();
    BOOL            OnLoad();
    BOOL            OnSave();

    BOOL			InitScreenEntries();


    void            AddNextSet();

private:
    CIssStats       m_oStats;  
    TCHAR*          m_szEquation;

    TCHAR           m_szPathFolder[STRING_MAX];

    EnumStatistics  m_eStatType;

};
