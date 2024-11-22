#pragma once

#include "CalcGlobal.h"
#include "globals.h"
#include "CalcBtnManager.h"

#include "DlgWSheet.h"//the ol baseclass

#include "DlgWSheetBreakEven.h"
#include "DlgWSheetCashFlow.h"
#include "DlgWSheetCompoundInt.h"
#include "DlgWSheetDate.h"
#include "DlgWSheetDepreciation.h"
#include "DlgWSheetLease.h"
#include "DlgWSheetLoan.h"
#include "DlgWSheetMort.h"
#include "DlgWSheetPcntChange.h"
#include "DlgWSheetProfitMargin.h"
#include "DlgWSheetTVM.h"
#include "DlgWSheetTip.h"
#include "DlgWSheetStats.h"



/*
enum EnumWorkSheet
{
    WS_BreakEven,
    WS_CashFlow,
    WS_CompountInt,
    WS_Date,
    WS_Depreciation,
    WS_Lease,
    WS_Loan,
    WS_Mort,
    WS_PcntChange,
    WS_ProfitMargin,
    WS_TVM,
    WS_Count,
};*/

class CWSheetManager
{
public:
    CWSheetManager(void);
    ~CWSheetManager(void);

    HRESULT         LaunchWorkSheet(LaunchType eWS, TypeCalcInit* sInit, CHandleButtons* oHandBtns, BOOL bPlaySounds);

private:

private:
    CDlgWSheet* m_dlgWS;
};
