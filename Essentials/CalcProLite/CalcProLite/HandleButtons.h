#pragma once

#include "IssCalculator.h"
#include "RepeatCounter.h"
#include "CalcBtnManager.h"

enum EnumLastMsg
{
	LAST_KeyDown,
	LAST_KeyUp,
	LAST_Char,
	LAST_None,
};

enum EnumDeviceType
{
	DEVTYPE_PPC,
	DEVTYPE_SP,
	DEVTYPE_SPLand
};


class CHandleButtons
{
public:
	CHandleButtons(void);
	~CHandleButtons(void);

	BOOL				OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);

	void				SetDeviceType(EnumDeviceType eType){m_eDevType = eType;};
	EnumDeviceType		GetDeviceType(){return m_eDevType;};

private:
	BOOL				OnKeyDownPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyDownSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyDownSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);

	BOOL				OnKeyUpPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyUpSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnKeyUpSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);

	BOOL				OnCharPPC(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnCharSP(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);
	BOOL				OnCharSPLand(HWND hWnd, WPARAM wParam, LPARAM lParam, CIssCalculator* oCalc, CCalcBtnManager* oMng);

	

private:
	CRepeatCounter		m_oRep;

	UINT				m_uiMessage;

	EnumLastMsg			m_eLastReceived;
	EnumLastMsg			m_eLastHandled;

	EnumDeviceType		m_eDevType;

	BOOL				m_bBackPressed;
	

};
