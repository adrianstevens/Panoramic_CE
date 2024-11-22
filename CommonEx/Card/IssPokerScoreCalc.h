#pragma once

#include "stdafx.h"
#include "IssPokerHandEval.h"

class CIssPokerScoreCalc
{
public:
	CIssPokerScoreCalc(void);
	~CIssPokerScoreCalc(void);

	int			GetPayoutRatio(EnumPokerType eType, EnumPokerResult eResult, BOOL bMaxBet = TRUE);

private:
	int			Get10sOrBetter(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetJacksOrBetter(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetAcesAndFaces(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetDoubleBonus(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetAmerican(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetEnglish(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetBonusDeluxe(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
	int			GetDoubleDouble(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
    int         GetStudVP(EnumPokerResult eResult, BOOL bMaxBet = TRUE);
};
