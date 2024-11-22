#include "HandleButtons.h"
#include "IssWnd.h"
#include "IssCommon.h"
#include "IssDebug.h"

#define WM_CALCENGINE WM_USER + 123

CHandleButtons::CHandleButtons(void)
:m_uiMessage(0)
,m_eLastHandled(LAST_None)
,m_eLastReceived(LAST_None)
,m_bBackPressed(FALSE)
,m_eDevType(DEVTYPE_PPC)
{

}

CHandleButtons::~CHandleButtons(void)
{
}

/*

BOOL CHandleButtons::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc)
{
	m_eLastReceived = LAST_Char;

	//numbers
	int iChar = LOWORD(wParam);

	//we'll handle the 12 buttons in KEY_DOWN/KEY_UP

	switch(iChar)
	{
#ifndef WIN32_PLATFORM_WFSP
	case _T('0'):
	case _T('1'):
	case _T('2'):
	case _T('3'):
	case _T('4'):
	case _T('5'):
	case _T('6'):
	case _T('7'):
	case _T('8'):
	case _T('9'):
		oCalc->AddNumber(NumberType(NUM_0+iChar-48));
		break;
#endif
	case VK_BACK:
		oCalc->AddClear(CLEAR_BackSpace);
		break;
	case _T('.'):
		oCalc->AddNumber(NUM_Period);
		break;
		//Operators
	case _T('+'):
		oCalc->AddOperator(OPP_Plus);
		break;
	case _T('-'):
		oCalc->AddOperator(OPP_Minus);
		break;
	case _T('*'):
		oCalc->AddOperator(OPP_Times);
		break;
	case _T('/'):
		oCalc->AddOperator(OPP_Divide);
		break;
//	case VK_RETURN://we'll use it for the selector otherwise
//		if(m_bShowSelector == TRUE)
//			break;
	case _T('='):
		oCalc->AddEquals(EQUALS_Equals);
		break;
	case _T('^'):
		oCalc->AddOperator(OPP_XY);
		break;
		//Functions
	case _T('%'):
		oCalc->AddFunction(FUNCTION_Percentage);
		break;
	case _T('!'):
		oCalc->AddFunction(FUNCTION_Factorial);
		break;
	case _T('('):
		oCalc->AddBrackets(BRACKET_Open);
		break;
	case _T(')'):
		oCalc->AddBrackets(BRACKET_Close);
		break;
	case _T('a'):
	case _T('A'):
		if(oCalc->GetCalcBase() != 10)
			oCalc->AddNumber(NUM_A);
		break;
	case _T('b'):
	case _T('B'):
		if(oCalc->GetCalcBase() != 10)	
			oCalc->AddNumber(NUM_B);
		break;
	case _T('c'):
	case _T('C'):
		if(oCalc->GetCalcBase() != 10)
			oCalc->AddNumber(NUM_C);
		else
			oCalc->AddFunction(FUNCTION_Cos);
		break;
	case _T('d'):
	case _T('D'):
		if(oCalc->GetCalcBase() != 10)
			oCalc->AddNumber(NUM_D);
		else
			oCalc->AddFunction(FUNCTION_DRG);
		break;
	case _T('e'):
	case _T('E'):
		if(oCalc->GetCalcBase() != 10)
			oCalc->AddNumber(NUM_E);
		else
			oCalc->AddOperator(OPP_EXP);
		break;
	case _T('f'):
	case _T('F'):
		if(oCalc->GetCalcBase() != 10)	
			oCalc->AddNumber(NUM_F);
		break;
	case _T('g'):
	case _T('G'):
		oCalc->AddStates(CALCSTATE_2ndF);
		break;
	case _T('h'):
	case _T('H'):
		oCalc->AddStates(CALCSTATE_Hyp);
		break;
	case _T('i'):
	case _T('I'):
		oCalc->AddFunction(FUNCTION_Inverse);
		break;
	case _T('j'):
	case _T('J'):
		oCalc->AddFunction(FUNCTION_EX);
		break;
	case _T('k'):
	case _T('K'):
		oCalc->AddFunction(FUNCTION_10X);
		break;
	case _T('l'):
	case _T('L'):
		oCalc->AddFunction(FUNCTION_Log);
		break;
	case _T('m'):
	case _T('M'):
		break;
	case _T('n'):
	case _T('N'):
		oCalc->AddFunction(FUNCTION_LN);
		break;
	case _T('o'):
	case _T('O'):
		oCalc->AddFunction(FUNCTION_PlusMinus);
		break;
	case _T('p'):
	case _T('P'):
		oCalc->AddConstants(CONSTANT_Pi);
		break;
	case _T('q'):
	case _T('Q'):

		break;
	case _T('r'):
	case _T('R'):
		oCalc->AddOperator(OPP_XrY);
		break;
	case _T('s'):
	case _T('S'):
		oCalc->AddFunction(FUNCTION_Sin);
		break;
	case _T('t'):
	case _T('T'):
		oCalc->AddFunction(FUNCTION_Tan);
		break;
	case _T('u'):
	case _T('U'):
		oCalc->AddOperator(OPP_EXP);
		break;
	case _T('v'):
	case _T('V'):
		oCalc->AddMemory(MEMORY_ClearMem1);
		break;
	case _T('w'):
	case _T('W'):
		oCalc->AddMemory(MEMORY_RecallMem1);
		break;
	case _T('x'):
	case _T('X'):
		oCalc->AddMemory(MEMORY_StoreMem1);
		break;
	case _T('y'):
	case _T('Y'):
		oCalc->AddMemory(MEMORY_PLUS);
		break;
	case _T('z'):
	case _T('Z'):
		oCalc->AddMemory(MEMORY_MINUS);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CHandleButtons::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc)
{
	m_eLastReceived = LAST_KeyDown;
#ifdef WIN32_PLATFORM_WFSP
	int iKey = LOWORD(wParam);

	if(m_oRep.AddButtonPress(GetTickCount(), iKey))
	{
		int iBtnIndex = -1; //relative to the ini file

		//hit the alt button
		switch(iKey)
		{
			//now we have to get the mappings out of the vector
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
		case _T('8'):
		case _T('9'):
			iBtnIndex = iKey - 49;//because 1 is the 0 button
			break;
		case 119://*
			iBtnIndex = 9;
			break;
		case _T('0'):
			iBtnIndex = 10;
			break;
		case 120://#
			iBtnIndex = 11;
			break;
		default:
			return FALSE;
			break;
		}
		if(iBtnIndex > -1)//double redundant - I don't like data aborts
		{
			PostMessage(hWnd, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->sMainAltButtons[iBtnIndex]->iButtonType, 
				m_oBtnManger.GetCurrentLayout()->sMainAltButtons[iBtnIndex]->iButtonValue);
			InvalidateRect(hWnd, &m_rcDisplayArea, FALSE);
		}

	}
	return TRUE;
#endif
	return FALSE;
}

BOOL CHandleButtons::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc)
{
	m_eLastReceived = LAST_KeyUp;

#ifdef WIN32_PLATFORM_WFSP //only useful for smartphone at the moment
	int iKey = LOWORD(wParam);
	int iBtnIndex = -1; 


	switch(iKey)
	{
	case VK_ESCAPE://for the back key
		if(m_bBackPressed)
		{
			::KillTimer(hWnd, TM_Back_Key);
			m_bBackPressed = FALSE;

			if(!m_bHoldBack)
				oCalc->AddClear(CLEAR_BackSpace);
			::InvalidateRect(hWnd, &m_rcDisplayArea, FALSE);
			return TRUE;
		}
		break;
	case _T('0'):
		iBtnIndex = 10;
		break;
	case _T('1'):
	case _T('2'):
	case _T('3'):
	case _T('4'):
	case _T('5'):
	case _T('6'):
	case _T('7'):
	case _T('8'):
	case _T('9'):
		iBtnIndex = iKey - 49;
		break;
	case 119://*
		iBtnIndex = 9;
		break;
	case 120: //#
		iBtnIndex = 11;
		break;
	case VK_DELETE:
		oCalc->AddClear(CLEAR_Clear);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP:
	case VK_RETURN:
		HandleDPad(iKey);
		break;
	default:
		return FALSE;
		break;
	}

	if(iBtnIndex > -1)
	{
		if(m_oRep.HasReturnedTrue() == FALSE)
		{
			PostMessage(hWnd, WM_CALCENGINE, m_oBtnManger.GetCurrentLayout()->sMainButtons[iBtnIndex]->iButtonType, 
				m_oBtnManger.GetCurrentLayout()->sMainButtons[iBtnIndex]->iButtonValue);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		m_oRep.Reset();//I think we always clear on a keyup ....
	}

	return TRUE;
#endif
	return FALSE;

	return FALSE;
}*/

BOOL CHandleButtons::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
#ifdef DEBUG
    DebugOut(_T("OnChar w:%d l:%d"), (int)wParam, (int)lParam);
#endif

	m_eLastReceived = LAST_Char;

	switch(m_eDevType)
	{
	case DEVTYPE_PPC:
		return OnCharPPC(hWnd, wParam, lParam, oCalc, oMng);	
		break;
	case DEVTYPE_SP:
		return OnCharSP(hWnd, wParam, lParam, oCalc, oMng);
		break;
	case DEVTYPE_SPLand:
		return OnCharSPLand(hWnd, wParam, lParam, oCalc, oMng);
		break;
	}

	return UNHANDLED;
		
/*
*/

}

/*
	On Smart phone .... KeyDown gets 229: #define VK_PROCESSKEY     0xE5
	for the 12 key number pad ... so we can skip ahead to KEY_UP for the value

	On normal 12 button smart phones ... we'll handle the tap and hold here ... otherwise we'll do the normal button press
	in WM_KEYUP (or WM_CHAR)

*/
BOOL CHandleButtons::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
#ifdef DEBUG
	DebugOut(_T("OnKeyDown w:%d l:%d"), (int)wParam, (int)lParam);
#endif

	m_eLastReceived = LAST_KeyDown;
	
	switch(m_eDevType)
	{
	case DEVTYPE_PPC:
	default:	
		return OnKeyDownPPC(hWnd, wParam, lParam, oCalc, oMng);	
		break;
	case DEVTYPE_SP:
		return OnKeyDownSP(hWnd, wParam, lParam, oCalc, oMng);
		break;
	case DEVTYPE_SPLand:
		return OnKeyDownSPLand(hWnd, wParam, lParam, oCalc, oMng);
	    break;
	}

	return UNHANDLED;
}

BOOL CHandleButtons::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
#ifdef DEBUG
	DebugOut(_T("OnKeyUp w:%d l:%d"), (int)wParam, (int)lParam);
#endif
	m_eLastReceived = LAST_KeyUp;

	switch(m_eDevType)
	{
	case DEVTYPE_PPC:
	default:	
		return OnKeyUpPPC(hWnd, wParam, lParam, oCalc, oMng);	
		break;
	case DEVTYPE_SP:
		return OnKeyUpSP(hWnd, wParam, lParam, oCalc, oMng);
		break;
	case DEVTYPE_SPLand:
		return OnKeyUpSPLand(hWnd, wParam, lParam, oCalc, oMng);
		break;
	}

	return UNHANDLED;


}

BOOL CHandleButtons::OnKeyDownPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	//we'll handle the D-Pad here


	return TRUE;
}

BOOL CHandleButtons::OnKeyDownSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	m_eLastHandled = LAST_None;//we always want to reset this for SP

    DebugOut(_T("KeyDowned %i\r\n"), wParam);

	int iKey = LOWORD(wParam);

	if(m_oRep.AddButtonPress(GetTickCount(), iKey))
	{
		int iBtnIndex = -1; //relative to the ini file

		//hit the alt button
		switch(iKey)
		{
			//now we have to get the mappings out of the vector
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
		case _T('8'):
		case _T('9'):
			iBtnIndex = iKey - 49;//because 1 is the 0 button
			break;
        case _T('0'):
            iBtnIndex = 10;
            break;
    //* & Number keys don't send repeat messages (of course)

    //	case 119://*
	//		iBtnIndex = 9;
	//		break;
	
	//	case 120://#
	//		iBtnIndex = 11;
	//		break;
		default:
			return UNHANDLED;
			break;
		}

        if(iBtnIndex > -1)//double redundant - I don't like data aborts
		{
			if(oMng->GetCurrentLayout(NULL)->sMainAltButtons.GetSize() > iBtnIndex)
			{   //do it now???
				SendMessage(hWnd, WM_CALCENGINE, oMng->GetCurrentLayout(NULL)->sMainAltButtons[iBtnIndex]->iButtonType, 
					oMng->GetCurrentLayout(NULL)->sMainAltButtons[iBtnIndex]->iButtonValue);

				m_eLastHandled = LAST_KeyDown;

                DebugOut(_T("Tiggered\r\n"));
                //invalidate the wholw damn screen
                InvalidateRect(hWnd, NULL, FALSE);
                UpdateWindow(hWnd);

				return TRUE;
			}
			else
			{	
				ASSERT(0);
			}
		}
	}

	return UNHANDLED;
}

BOOL CHandleButtons::OnKeyDownSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	return TRUE;
}

BOOL CHandleButtons::OnKeyUpPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	return TRUE;
}

BOOL CHandleButtons::OnKeyUpSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    int iKey = LOWORD(wParam);
    int iBtnIndex = -1; 

	if(m_eLastHandled != LAST_None && iKey != VK_ESCAPE)
		return UNHANDLED;

	//then we've gotta step up to the plate
	//handle numbers, d pad, and return
	m_eLastHandled	= LAST_None;

	switch(iKey)
	{
	
    //doesn't work as is ... think its an old copy/paste
  /*  case VK_ESCAPE://for the back key
		if(m_bBackPressed)
		{
			::KillTimer(hWnd, TM_Back_Key);
			m_bBackPressed = FALSE;
			if(!m_bHoldBack)
				oCalc->AddClear(CLEAR_BackSpace);
			::InvalidateRect(hWnd, &m_rcDisplayArea, FALSE);
			return TRUE;
		}
		break;*/
    //end uncomment

    
    case _T('0'):
		iBtnIndex = 10;
		break;
	case _T('1'):
	case _T('2'):
	case _T('3'):
	case _T('4'):
	case _T('5'):
	case _T('6'):
	case _T('7'):
	case _T('8'):
	case _T('9'):
		iBtnIndex = iKey - 49;
		break;
	case 119://*
		iBtnIndex = 9;
		break;
	case 120: //#
		iBtnIndex = 11;
		break;
	case VK_DELETE:
		oCalc->AddClear(CLEAR_Clear);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP:
	case VK_RETURN:
	//	HandleDPad(iKey);
		break;
    case VK_ESCAPE:
        //oCalc->AddClear(CLEAR_BackSpace);
        iBtnIndex = VK_ESCAPE;//bit of a hack I know
        break;
	default:
		return UNHANDLED;
		break;
	}

    DWORD dwCount= (DWORD)lParam & 0xFFFF;

    if(iBtnIndex == VK_ESCAPE)
    {   //http://msdn.microsoft.com/en-us/library/aa924525.aspx
        //lParam  
        //0-15 repeat count
        //16-23 scan code
        //29 context code
        //30 previous key state
        //31 transition state (up or down ... what the hell?)

        //we gotta post a clear message
        PostMessage(hWnd, WM_CALCENGINE, INPUT_Clear, 
            CLEAR_BackSpace);

        m_eLastHandled = LAST_KeyUp;

        return TRUE;
    }

    else if(iBtnIndex > -1)
	{
		if(m_oRep.HasReturnedTrue() == FALSE)
		{
			if(iBtnIndex >= oMng->GetCurrentLayout()->sMainButtons.GetSize())
			{
				ASSERT(0);
				return UNHANDLED;
			}

			PostMessage(hWnd, WM_CALCENGINE, oMng->GetCurrentLayout()->sMainButtons[iBtnIndex]->iButtonType, 
				oMng->GetCurrentLayout()->sMainButtons[iBtnIndex]->iButtonValue);
			InvalidateRect(hWnd, NULL, FALSE);
			m_eLastHandled = LAST_KeyUp;
		}
		m_oRep.Reset();//I think we always clear on a keyup ....
	}

	return TRUE;
}

BOOL CHandleButtons::OnKeyUpSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    if(LOWORD(wParam) == VK_BACK)
    {
        oCalc->AddClear(CLEAR_BackSpace);
    }
	return TRUE;
}

BOOL CHandleButtons::OnCharStandard(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    int iChar = LOWORD(wParam);

    //we'll handle the 12 buttons in KEY_DOWN/KEY_UP
    //handle all characters
    switch(iChar)
    {
    case _T('a'):
    case _T('A'):
        if(oCalc->GetCalcBase() != 10)
            oCalc->AddNumber(NUM_A);
        break;
    case _T('b'):
    case _T('B'):
        if(oCalc->GetCalcBase() != 10)	
            oCalc->AddNumber(NUM_B);
        break;
    case _T('c'):
    case _T('C'):
        if(oCalc->GetCalcBase() != 10)
            oCalc->AddNumber(NUM_C);
        else
            oCalc->AddFunction(FUNCTION_Cos);
        break;
    case _T('d'):
    case _T('D'):
        if(oCalc->GetCalcBase() != 10)
            oCalc->AddNumber(NUM_D);
        else
            oCalc->AddFunction(FUNCTION_DRG);
        break;
    case _T('e'):
    case _T('E'):
        if(oCalc->GetCalcBase() != 10)
            oCalc->AddNumber(NUM_E);
        else
            oCalc->AddOperator(OPP_EXP);
        break;
    case _T('f'):
    case _T('F'):
        if(oCalc->GetCalcBase() != 10)	
            oCalc->AddNumber(NUM_F);
        break;
    case _T('g'):
    case _T('G'):
        oCalc->AddStates(CALCSTATE_2ndF);
        break;
    case _T('h'):
    case _T('H'):
        oCalc->AddStates(CALCSTATE_Hyp);
        break;
    case _T('i'):
    case _T('I'):
        oCalc->AddFunction(FUNCTION_Inverse);
        break;
    case _T('j'):
    case _T('J'):
        oCalc->AddFunction(FUNCTION_EX);
        break;
    case _T('k'):
    case _T('K'):
        oCalc->AddFunction(FUNCTION_10X);
        break;
    case _T('l'):
    case _T('L'):
        oCalc->AddFunction(FUNCTION_Log);
        break;
    case _T('m'):
    case _T('M'):
        oCalc->AddMemory(MEMORY_PLUS);
        break;
    case _T('n'):
    case _T('N'):
        oCalc->AddFunction(FUNCTION_LN);
        break;
    case _T('o'):
    case _T('O'):
        oCalc->AddFunction(FUNCTION_PlusMinus);
        break;
    case _T('p'):
    case _T('P'):
        oCalc->AddConstants(CONSTANT_Pi);
        break;
    case _T('q'):
    case _T('Q'):
        break;
    case _T('r'):
    case _T('R'):
        oCalc->AddOperator(OPP_XrY);
        break;
    case _T('s'):
    case _T('S'):
        oCalc->AddFunction(FUNCTION_Sin);
        break;
    case _T('t'):
    case _T('T'):
        oCalc->AddFunction(FUNCTION_Tan);
        break;
    case _T('u'):
    case _T('U'):
        oCalc->AddOperator(OPP_EXP);
        break;
    case _T('v'):
    case _T('V'):
        oCalc->AddMemory(MEMORY_ClearMem1);
        break;
    case _T('w'):
    case _T('W'):
        oCalc->AddMemory(MEMORY_RecallMem1);
        break;
    case _T('x'):
    case _T('X'):
        oCalc->AddOperator(OPP_Times);
        break;
    case _T('y'):
    case _T('Y'):
        oCalc->AddMemory(MEMORY_StoreMem1);
        break;
    case _T('z'):
    case _T('Z'):
        oCalc->AddMemory(MEMORY_PLUS);
        break;
    default: 
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL CHandleButtons::OnCharQWERTY(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    int iChar = LOWORD(wParam);

    //we'll handle the 12 buttons in KEY_DOWN/KEY_UP
    //handle all characters
    DWORD dwType;
    DWORD dwID;

    int iBtnID = 0;

    switch(iChar)
    {
    case _T('a'):
    case _T('A'):
        iBtnID = 1;
        break;
    case _T('b'):
    case _T('B'):
        iBtnID = 17;
        break;
    case _T('c'):
    case _T('C'):
        iBtnID = 11;
        break;
    case _T('d'):
    case _T('D'):
        iBtnID = 7;
        break;
    case _T('e'):
    case _T('E'):
        iBtnID = 6;
        break;
    case _T('f'):
    case _T('F'):
        iBtnID = 10;
        break;
    case _T('g'):
    case _T('G'):
        iBtnID = 13;
        break;
    case _T('h'):
    case _T('H'):
        iBtnID = 16;
        break;
    case _T('i'):
    case _T('I'):
        iBtnID = 21;
        break;
    case _T('j'):
    case _T('J'):
        iBtnID = 19;
        break;
    case _T('k'):
    case _T('K'):
        iBtnID = 22;
        break;
    case _T('l'):
    case _T('L'):
        iBtnID = 25;
        break;
    case _T('m'):
    case _T('M'):
        iBtnID = 23;
        break;
    case _T('n'):
    case _T('N'):
        iBtnID = 20;
        break;
    case _T('o'):
    case _T('O'):
        iBtnID = 24;
        break;
    case _T('p'):
    case _T('P'):
        iBtnID = 26;
        break;
    case _T('q'):
    case _T('Q'):
        iBtnID = 0;
        break;
    case _T('r'):
    case _T('R'):
        iBtnID = 9;
        break;
    case _T('s'):
    case _T('S'):
        iBtnID = 4;
        break;
    case _T('t'):
    case _T('T'):
        iBtnID = 12;
        break;
    case _T('u'):
    case _T('U'):
        iBtnID = 18;
        break;
    case _T('v'):
    case _T('V'):
        iBtnID =14;
        break;
    case _T('w'):
    case _T('W'):
        iBtnID = 3;
        break;
    case _T('x'):
    case _T('X'):
        iBtnID = 8;
        break;
    case _T('y'):
    case _T('Y'):
        iBtnID = 15;
        break;
    case _T('z'):
    case _T('Z'):
        iBtnID = 5;
        break;
    default: 
        return FALSE;
        break;
    }

    dwType = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonType;
    dwID = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonValue;
    oCalc->CalcButtonPress(dwType, dwID);

    return TRUE;

}

BOOL CHandleButtons::OnCharQWERTZ(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    int iChar = LOWORD(wParam);

    //we'll handle the 12 buttons in KEY_DOWN/KEY_UP
    //handle all characters
    DWORD dwType;
    DWORD dwID;
    int iBtnID = 0;

    switch(iChar)
    {
    case _T('y'):
    case _T('Y'):
        iBtnID = 5;
        break;
    case _T('z'):
    case _T('Z'):
        iBtnID = 15;
        break;
    default:
        return OnCharQWERTY(hWnd, wParam, lParam, oCalc, oMng);
        break;
    }

    dwType = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonType;
    dwID = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonValue;
    oCalc->CalcButtonPress(dwType, dwID);

    return TRUE;
}

BOOL CHandleButtons::OnCharAZERTY(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
    int iChar = LOWORD(wParam);

    //we'll handle the 12 buttons in KEY_DOWN/KEY_UP
    //handle all characters
    DWORD dwType;
    DWORD dwID;
    int iBtnID = 0;

    switch(iChar)
    {
    case _T('a'):
    case _T('A'):
        iBtnID = 0;
        break;
    case _T('q'):
    case _T('Q'):
        iBtnID = 1;
        break;
    case _T('z'):
    case _T('Z'):
        iBtnID = 3;
        break;
    case _T('w'):
    case _T('W'):
        iBtnID = 5;
        break;
    //M gets moved but I don't think I'll remap ... just move the button ... easy enough
    default:
        return OnCharQWERTY(hWnd, wParam, lParam, oCalc, oMng);
        break;
    }

    dwType = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonType;
    dwID = oMng->GetCurrentLayout(NULL)->sKeyboardButtons[iBtnID]->iButtonValue;
    oCalc->CalcButtonPress(dwType, dwID);

    return TRUE;
}

BOOL CHandleButtons::OnCharPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	//numbers
	int iChar = LOWORD(wParam);

	//we'll handle the 12 buttons in KEY_DOWN/KEY_UP
	//handle all characters
	switch(iChar)
	{
	// if it hasn't been handled already we'll do it 
	case _T('0'):
	case _T('1'):
	case _T('2'):
	case _T('3'):
	case _T('4'):
	case _T('5'):
	case _T('6'):
	case _T('7'):
	case _T('8'):
	case _T('9'):
		oCalc->AddNumber(NumberType(NUM_0+iChar-48));
		break;
	case VK_BACK:
		oCalc->AddClear(CLEAR_BackSpace);
		break;
	case VK_SPACE:
	case VK_RETURN:
		oCalc->AddEquals(EQUALS_Equals);
		break;
	case _T('.'):
    case _T(','):
		oCalc->AddNumber(NUM_Period);
		break;
    //Operators
	case _T('+'):
		oCalc->AddOperator(OPP_Plus);
		break;
	case _T('-'):
		oCalc->AddOperator(OPP_Minus);
		break;
	case _T('*'):
		oCalc->AddOperator(OPP_Times);
		break;
	case _T('/'):
    case _T('\\'):
		oCalc->AddOperator(OPP_Divide);
		break;
		//	case VK_RETURN://we'll use it for the selector otherwise
		//		if(m_bShowSelector == TRUE)
		//			break;
	case _T('='):
    case _T('?'):
		oCalc->AddEquals(EQUALS_Equals);
		break;
	case _T('^'):
		oCalc->AddOperator(OPP_XY);
		break;
		//Functions
	case _T('%'):
		oCalc->AddFunction(FUNCTION_Percentage);
		break;
	case _T('!'):
		oCalc->AddFunction(FUNCTION_Factorial);
		break;
	case _T('('):
		oCalc->AddBrackets(BRACKET_Open);
		break;
	case _T(')'):
		oCalc->AddBrackets(BRACKET_Close);
		break;
    case _T('#'):
        oCalc->AddConstants(CONSTANT_Last_Ans);
        break;
	case _T('"'):
        oCalc->AddFunction(FUNCTION_DRG);
        break;
    case _T('&'):
        oCalc->AddOperator(OPP_AND);
        break;
    case _T('$'):
        oCalc->AddClear(CLEAR_Clear);
        break;
	default:
        {
            switch(oMng->GetKBType())
            {
            case KB_Original:
                OnCharStandard(hWnd, wParam, lParam, oCalc, oMng);
            	break;
            case KB_QWERTY:
                OnCharQWERTY(hWnd, wParam, lParam, oCalc, oMng);
            	break;
            case KB_QWERTZ:
                OnCharQWERTZ(hWnd, wParam, lParam, oCalc, oMng);
                break;
            case KB_AZERTY:
                OnCharAZERTY(hWnd, wParam, lParam, oCalc, oMng);
                break;
            default:
                return FALSE;
                break;
            }
        }
		break;
	}
	m_eLastHandled = LAST_Char;
    InvalidateRect(hWnd, NULL, FALSE);
//	m_oRep.Reset();//if we do anything we reset
	return TRUE;
}

BOOL CHandleButtons::OnCharSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	return TRUE;
}

BOOL CHandleButtons::OnCharSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng)
{
	if(LOWORD(wParam) == VK_RETURN)
		return UNHANDLED;

	return OnCharPPC(hWnd, wParam, lParam, oCalc, oMng);
}