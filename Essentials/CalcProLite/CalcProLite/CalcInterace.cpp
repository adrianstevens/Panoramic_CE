#include "CalcInterace.h"

CCalcInterace::CCalcInterace(void)
:m_hInst(NULL)
,m_hWndDlg(NULL)
,m_oCalc(NULL)
,m_oBtnHand(NULL)
,m_oBtnMan(NULL)
,m_wndMenu(NULL)
,m_oMenu(NULL)
,m_oConstants(NULL)
,m_iSelector(0)
,m_bExpired(TRUE)
,m_oStr(CIssString::Instance())
{
}

CCalcInterace::~CCalcInterace(void)
{
}

void CCalcInterace::Init(TypeCalcInit* sInit)
{
    if(sInit != NULL)
    {
        m_oBtnHand	= sInit->oBtnHand;
        m_oBtnMan	= sInit->oBtnMan;
        m_oCalc		= sInit->oCalc;
        m_wndMenu   = sInit->wndMenu;

        m_hWndDlg   = sInit->hWnd;
        m_hInst     = sInit->hInst;
        m_oMenu     = sInit->oMenu;
        m_oConstants= sInit->oConstants;
    }
}