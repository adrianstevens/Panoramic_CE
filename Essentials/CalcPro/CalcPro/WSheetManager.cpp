#include "WSheetManager.h"

CWSheetManager::CWSheetManager(void)
:m_dlgWS(NULL)
{

}

CWSheetManager::~CWSheetManager(void)
{
    if(m_dlgWS)
    {
        delete m_dlgWS;
        m_dlgWS = NULL;
    }
}

HRESULT CWSheetManager::LaunchWorkSheet(LaunchType eWS, TypeCalcInit* sInit, CHandleButtons* oHandBtns, BOOL bPlaySounds)
{   
    HRESULT hr = S_OK;

    if(sInit == NULL ||
        sInit->hInst == NULL ||
        sInit->hWnd == NULL ||
        sInit->oBtnHand == NULL || 
        sInit->oBtnMan == NULL ||
        sInit->oCalc == NULL ||
        sInit->oMenu == NULL)
    {
        ASSERT(0);
        return E_INVALIDARG;
    }

    //clear the current display
    sInit->oCalc->AddClear(CLEAR_ClearAll);

    CalcOperationType eOldCalcOp = sInit->oCalc->GetCalcMode();
    DisplayType eOldDisplay = sInit->oCalc->GetCalcDisplay();

    sInit->oCalc->SetCalcMode(CALC_OrderOfOpps);
    sInit->oCalc->SetCalcDisplay(DISPLAY_Float);

    EnumCalcType eOldCalcType = sInit->oBtnMan->GetCalcType();

    sInit->oBtnMan->SetCalcType(CALC_WorkSheet, sInit->hInst);

    if(m_dlgWS)
    {
        delete m_dlgWS;
        m_dlgWS = NULL;
    }
    
    switch(eWS)
    {
    case LAUNCH_BreakEven:
        m_dlgWS = new CDlgWSheetBreakEven;
    	break;
    case LAUNCH_CashFlow:
        m_dlgWS = new CDlgWSheetCashFlow;
    	break;
    case LAUNCH_CompoundInt:
        m_dlgWS = new CDlgWSheetCompoundInt;
        break;
    case LAUNCH_Date:
        m_dlgWS = new CDlgWSheetDate;
        break;
    case LAUNCH_Depreciation:
        m_dlgWS = new CDlgWSheetDepreciation;
    	break;
    case LAUNCH_Lease:
        m_dlgWS = new CDlgWSheetLease;
    	break;
    case LAUNCH_Loan:
        m_dlgWS = new CDlgWSheetLoan;
        break;
    case LAUNCH_Mort:
        m_dlgWS = new CDlgWSheetMort;
        break;
    case LAUNCH_PercentChange:
        m_dlgWS = new CDlgWSheetPcntChange;
        break;
    case LAUNCH_ProfitMargin:
        m_dlgWS = new CDlgWSheetProfitMargin;
        break;
    case LAUNCH_TVM:
        m_dlgWS = new CDlgWSheetTVM;
        break;
    case LAUNCH_Tip:
        m_dlgWS = new CDlgWSheetTip;
        break;
    case LAUNCH_LinearReg:
    case LAUNCH_ExpReg:
    case LAUNCH_LogReg:
    case LAUNCH_Power:
        m_dlgWS = new CDlgWSheetStats(eWS);
        sInit->oBtnMan->SetCalcType(CALC_WSStats, sInit->hInst);
        break;
    default:
        m_dlgWS = NULL;
        hr = E_INVALIDARG;
        goto Error;
        break;
    }

    if(m_dlgWS)
    {
        m_dlgWS->Init(sInit, oHandBtns, bPlaySounds, sInit->hWnd);
#ifdef UNDER_CE
        m_dlgWS->DoModal(sInit->hWnd, sInit->hInst, IDD_DLG_BASIC);
#else
RECT rc;
		GetWindowRect(sInit->hWnd, &rc);

		if(m_dlgWS->Create(_T("Calc Pro"), NULL, sInit->hInst, _T("Worksheet"),
			CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
			0,
			DS_MODALFRAME | WS_CAPTION | WS_SYSMENU, NULL, IDI_CALCPRO))
		{
			MoveWindow(m_dlgWS->GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), TRUE);
			ShowWindow(m_dlgWS->GetWnd(), SW_SHOW);
			ShowWindow(sInit->hWnd, SW_HIDE);

			MSG msg;
			while( GetMessage( &msg, NULL,0,0 ) ) /* jump into message pump */
			{
				//if(msg.message == WM_QUIT || msg.message == WM_CLOSE)
				//	break;
				TranslateMessage( &msg );
				DispatchMessage ( &msg );
			}
			ShowWindow(sInit->hWnd, SW_SHOW);
		}
#endif
    }

Error:
#ifndef UNDER_CE
	DestroyWindow(m_dlgWS->GetWnd());
	InvalidateRect(sInit->hWnd, NULL, FALSE);

#endif
    //always put it back
    hr = sInit->oBtnMan->SetCalcType(eOldCalcType, sInit->hInst);
    sInit->oCalc->SetCalcMode(eOldCalcOp);
    sInit->oCalc->SetCalcDisplay(eOldDisplay);
    return hr;

}
