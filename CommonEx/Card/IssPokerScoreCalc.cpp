#include "IssPokerScoreCalc.h"

CIssPokerScoreCalc::CIssPokerScoreCalc(void)
{
}

CIssPokerScoreCalc::~CIssPokerScoreCalc(void)
{
}

int CIssPokerScoreCalc::GetPayoutRatio(EnumPokerType eType, EnumPokerResult eResult, BOOL bMaxBet)
{
	switch(eType)
	{
	case PTYPE_10sOrBetter:
		return Get10sOrBetter(eResult, bMaxBet);
		break;
	case PTYPE_JacksOrBetter:
		return GetJacksOrBetter(eResult, bMaxBet);
		break;
	case PTYPE_AcesAndFaces:
		return GetAcesAndFaces(eResult, bMaxBet);
		break;
	case PTYPE_DoubleBonus:
		return GetDoubleBonus(eResult, bMaxBet);
		break;
	case PTYPE_American:
		return GetAmerican(eResult, bMaxBet);
		break;
	case PTYPE_English:
		return GetEnglish(eResult, bMaxBet);
		break;
	case PTYPE_BonusDeluxe:
		return GetBonusDeluxe(eResult, bMaxBet);
		break;
    case PTYPE_StudVP:
        return GetStudVP(eResult, bMaxBet);
        break;
	default:
		return 0;
	}
}

int CIssPokerScoreCalc::GetAmerican(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_Pair:
		iReturn = 2;
			break;
	case VPRESULT_2Pairs:
		iReturn = 5;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 10;
		break;
	case VPRESULT_Straight:
		iReturn = 15;
		break;
	case VPRESULT_Flush:
		iReturn = 20;
		break;
	case VPRESULT_FullHouse:
		iReturn = 25;
		break;
	case VPRESULT_4OfaKind:
		iReturn = 50;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 75;
		break;
	case VPRESULT_RoyalFlush:
		iReturn = 100;
		break;
	}
	return iReturn;
}

int CIssPokerScoreCalc::GetEnglish(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_Pair:
		iReturn = 2;
		break;
	case VPRESULT_2Pairs:
		iReturn = 6;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 12;
		break;
	case VPRESULT_Straight:
		iReturn = 24;
		break;
	case VPRESULT_Flush:
		iReturn = 10;
		break;
	case VPRESULT_FullHouse:
		iReturn = 20;
		break;
	case VPRESULT_4OfaKind:
		iReturn = 32;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 60;
		break;
	case VPRESULT_RoyalFlush:
		iReturn = 60;
		break;
	}
	return iReturn;
}

int CIssPokerScoreCalc::GetStudVP(EnumPokerResult eResult, BOOL bMaxBet)
{
    int iReturn = 0;

    switch(eResult)
    {
    default:
    case VPRESULT_Lose:
        iReturn = 0;
        break;
    case VPRESULT_6sOrBetter:
        iReturn = 1;
        break;
    case VPRESULT_JacksOrBetter:
        iReturn = 2;
        break;
    case VPRESULT_2Pairs:
        iReturn = 3;
        break;
    case VPRESULT_3OfaKind:
        iReturn = 4;
        break;
    case VPRESULT_Straight:
        iReturn = 6;
        break;
    case VPRESULT_Flush:
        iReturn = 9;
        break;
    case VPRESULT_FullHouse:
        iReturn = 12;
        break;
    case VPRESULT_4OfaKind:
        iReturn = 50;
        break;
    case VPRESULT_StraightFlush:
        iReturn = 200;
        break;
    case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 2000;
        else
            iReturn = 1000;
        break;
    }
    return iReturn;
}

int CIssPokerScoreCalc::Get10sOrBetter(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_10sOrBetter:
		iReturn = 1;
		break;
	case VPRESULT_2Pairs:
		iReturn = 2;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 3;
		break;
	case VPRESULT_Straight:
		iReturn = 4;
		break;
	case VPRESULT_Flush:
		iReturn = 5;
		break;
	case VPRESULT_FullHouse:
		iReturn = 6;
		break;
	case VPRESULT_4OfaKind:
		iReturn = 25;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
		    iReturn = 800;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}

int CIssPokerScoreCalc::GetJacksOrBetter(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_JacksOrBetter:
		iReturn = 1;
			break;
	case VPRESULT_2Pairs:
		iReturn = 2;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 3;
		break;
	case VPRESULT_Straight:
		iReturn = 4;
		break;
	case VPRESULT_Flush:
		iReturn = 6;
		break;
	case VPRESULT_FullHouse:
		iReturn = 9;
		break;
	case VPRESULT_4OfaKind:
		iReturn = 25;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 800;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}
//9/6 payout table
int CIssPokerScoreCalc::GetBonusDeluxe(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_JacksOrBetter:
		iReturn = 1;
		break;
	case VPRESULT_2Pairs:
		iReturn = 1;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 3;
		break;
	case VPRESULT_Straight:
		iReturn = 4;
		break;
	case VPRESULT_Flush:
		iReturn = 6;
		break;
	case VPRESULT_FullHouse:
		iReturn = 9;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_4OfaKind:
		iReturn = 80;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 800;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}

int CIssPokerScoreCalc::GetDoubleDouble(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_JacksOrBetter:
		iReturn = 1;
		break;
	case VPRESULT_2Pairs:
		iReturn = 1;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 3;
		break;
	case VPRESULT_Straight:
		iReturn = 5;
		break;
	case VPRESULT_Flush:
		iReturn = 6;
		break;
	case VPRESULT_FullHouse:
		iReturn = 10;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_4OfaKind5sKs:
		iReturn = 50;
		break;
	case VPRESULT_4OfaKind2s4s:
		iReturn = 80;
		break;
	case VPRESULT_4OfaKindAces:
		iReturn = 160;
		break;
	case VPRESULT_4OfaKind2s4swA:
		iReturn = 160;
		break;
	case VPRESULT_4OfaKindAcesw234:
		iReturn = 400;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 800;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}



int CIssPokerScoreCalc::GetAcesAndFaces(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_JacksOrBetter:
		iReturn = 1;
			break;
	case VPRESULT_2Pairs:
		iReturn = 2;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 3;
		break;
	case VPRESULT_Straight:
		iReturn = 4;
		break;
	case VPRESULT_Flush:
		iReturn = 5;
		break;
	case VPRESULT_FullHouse:
		iReturn = 8;
		break;
	case VPRESULT_4OfaKind2s10s:
		iReturn = 25;
		break;
	case VPRESULT_4OfaKindJsKs:
		iReturn = 40;
		break;
	case VPRESULT_4OfaKindAces:
		iReturn = 80;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 800;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}

int CIssPokerScoreCalc::GetDoubleBonus(EnumPokerResult eResult, BOOL bMaxBet)
{
	int iReturn = 0;

	switch(eResult)
	{
	default:
	case VPRESULT_Lose:
		iReturn = 0;
		break;
	case VPRESULT_JacksOrBetter:
		iReturn = 1;
			break;
	case VPRESULT_2Pairs:
		iReturn = 2;
		break;
	case VPRESULT_3OfaKind:
		iReturn = 2;
		break;
	case VPRESULT_Straight:
		iReturn = 4;
		break;
	case VPRESULT_Flush:
		iReturn = 5;
		break;
	case VPRESULT_FullHouse:
		iReturn = 8;
		break;
	case VPRESULT_4OfaKind5sKs:
		iReturn = 50;
		break;
	case VPRESULT_4OfaKind2s4s:
		iReturn = 90;
		break;
	case VPRESULT_4OfaKindAces:
		iReturn = 160;
		break;
	case VPRESULT_StraightFlush:
		iReturn = 50;
		break;
	case VPRESULT_RoyalFlush:
        if(bMaxBet)
            iReturn = 840;
        else 
            iReturn = 250;
		break;
	}
	return iReturn;
}
