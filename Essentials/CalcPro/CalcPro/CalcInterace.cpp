#include "CalcInterace.h"
#include "globals.h"

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

int CCalcInterace::GetSystemMetrics(__in int nIndex)
{
	return GetSysMets(nIndex);
/*
#ifdef UNDER_CE
	return ::GetSystemMetrics(nIndex);
#else
	switch(nIndex)
	{
	case SM_CXSMICON:
		return 32;
		break;
	case SM_CXICON:
		return 64;
		break;
	case SM_CXSCREEN:
		return WINDOW_WIDTH;
		break;
	case SM_CYSCREEN:
		return WINDOW_HEIGHT;
		break;
	default:
		return ::GetSystemMetrics(nIndex);
		break;
	}
#endif */
}