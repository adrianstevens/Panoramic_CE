#include "StdAfx.h"
#include "IssPokerHandEval.h"

CIssPokerHandEval::CIssPokerHandEval(void)
{
}

CIssPokerHandEval::~CIssPokerHandEval(void)
{

}


EnumPokerResult CIssPokerHandEval::EvaluateHand(EnumPokerType eType)
{
	int m		=	0;//loop counters
	int n		=	0;
	int tempvalue;
	int tempsuit;
	CIssCard* oCard;

#define NUMBER_OF_CARDS 5

	//Comment out all scoring code...we'll do it in a separate function

	//YEHAW...reusing old code
	int iPlayerCard[5][2];
	for(m = 0; m < NUMBER_OF_CARDS; m++)
	{
		oCard = m_oCard[m];

		if(!oCard)
        {
            ASSERT(0); 
            return VPRESULT_Lose;
        }


		iPlayerCard[m][0] = (int)oCard->m_eRank;
		iPlayerCard[m][1] = (int)oCard->m_eSuit;
	}

	//First We'll put the cards in descending order....
	//Its kinda brute force but its only 16 comparisons so I just don't care....
	//Cause it works
	for(m=0; m < 4; m++)
	{
		for(n=0; n<4; n++)
		{
			if(iPlayerCard[n][0]<iPlayerCard[n+1][0])
			{
				tempvalue=iPlayerCard[n+1][0];
				tempsuit =iPlayerCard[n+1][1];

				iPlayerCard[n+1][0]=iPlayerCard[n][0];
				iPlayerCard[n+1][1]=iPlayerCard[n][1];

				iPlayerCard[n][0]=tempvalue;
				iPlayerCard[n][1]=tempsuit;
			}
		}
	}
	//Now we start comparing the cards
	switch(eType)
	{
	default:
	case PTYPE_Straight:
		return EvaluateStraight(iPlayerCard);
		break;
	case PTYPE_10sOrBetter:
		return Evaluate10sOrBetter(iPlayerCard);
		break;
	case PTYPE_JacksOrBetter:
		return EvaluateJacksOrBetter(iPlayerCard);
		break;
	case PTYPE_AcesAndFaces:
		return EvaluateAcesAndFaces(iPlayerCard);
		break;
	case PTYPE_DoubleBonus:
		return EvaluateDoubleBonus(iPlayerCard);
		break;
    case PTYPE_BonusDeluxe://or double double
        return EvaluateBonusDeluxe(iPlayerCard);
        break;
    case PTYPE_StudVP:
        return EvaluateStud(iPlayerCard);
        break;
	}
}

EnumPokerResult CIssPokerHandEval::EvaluateStud(int iCards[5][2])
{
    //Pretty simple ... we just check em in order of payout
    if(RoyalFlush(iCards))
        return VPRESULT_RoyalFlush;
    if(StraightFlush(iCards))
        return VPRESULT_StraightFlush;
    if(Flush(iCards))
        return VPRESULT_Flush;
    if(Straight(iCards))
        return VPRESULT_Straight;
    if(FourOfaKind(iCards))
        return VPRESULT_4OfaKind;
    if(FullHouse(iCards))
        return VPRESULT_FullHouse;
    if(ThreeOfaKind(iCards))
        return VPRESULT_3OfaKind;
    if(TwoPair(iCards))
        return VPRESULT_2Pairs;
    if(JacksOrBetter(iCards))
        return VPRESULT_JacksOrBetter;
    if(SixesOrBetter(iCards))
        return VPRESULT_6sOrBetter;
    return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::Evaluate10sOrBetter(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKind(iCards))
		return VPRESULT_4OfaKind;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(TensOrBetter(iCards))
		return VPRESULT_10sOrBetter;

	return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::EvaluateJacksOrBetter(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKind(iCards))
		return VPRESULT_4OfaKind;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(JacksOrBetter(iCards))
		return VPRESULT_JacksOrBetter;

	return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::EvaluateBonusDeluxe(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKind(iCards))
		return VPRESULT_4OfaKind;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(JacksOrBetter(iCards))
		return VPRESULT_JacksOrBetter;

	return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::EvaluateStraight(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKind(iCards))
		return VPRESULT_4OfaKind;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(Pair(iCards))
		return VPRESULT_Pair;

	return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::EvaluateDoubleBonus(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKindAces(iCards))
		return VPRESULT_4OfaKindAces;
	if(FourOfaKind2s4s(iCards))
		return VPRESULT_4OfaKind2s4s;
	if(FourOfaKind5sKs(iCards))
		return VPRESULT_4OfaKind5sKs;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(JacksOrBetter(iCards))
		return VPRESULT_JacksOrBetter;

	return VPRESULT_Lose;
}

EnumPokerResult CIssPokerHandEval::EvaluateDoubleDouble(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKindAcesw234(iCards))
		return VPRESULT_4OfaKindAcesw234;
	if(FourOfaKindAces(iCards))
		return VPRESULT_4OfaKindAces;
	if(FourOfaKind2s4swA(iCards))
		return VPRESULT_4OfaKind2s4swA;
	if(FourOfaKind2s4s(iCards))
		return VPRESULT_4OfaKind2s4s;
	if(FourOfaKind5sKs(iCards))
		return VPRESULT_4OfaKind5sKs;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(JacksOrBetter(iCards))
		return VPRESULT_JacksOrBetter;

	return VPRESULT_Lose;
}


EnumPokerResult CIssPokerHandEval::EvaluateAcesAndFaces(int iCards[5][2])
{
	//Pretty simple ... we just check em in order of payout
	if(RoyalFlush(iCards))
		return VPRESULT_RoyalFlush;
	if(StraightFlush(iCards))
		return VPRESULT_StraightFlush;
	if(Flush(iCards))
		return VPRESULT_Flush;
	if(Straight(iCards))
		return VPRESULT_Straight;
	if(FourOfaKindAces(iCards))
		return VPRESULT_4OfaKindAces;
	if(FourOfaKind2s10s(iCards))
		return VPRESULT_4OfaKind2s10s;
	if(FourOfaKindJsKs(iCards))
		return VPRESULT_4OfaKindJsKs;
	if(FullHouse(iCards))
		return VPRESULT_FullHouse;
	if(ThreeOfaKind(iCards))
		return VPRESULT_3OfaKind;
	if(TwoPair(iCards))
		return VPRESULT_2Pairs;
	if(JacksOrBetter(iCards))
		return VPRESULT_JacksOrBetter;

	return VPRESULT_Lose;
}


BOOL CIssPokerHandEval::Flush(int iCards[5][2])
{
	if( (iCards[0][1]==iCards[1][1]) &&
		(iCards[0][1]==iCards[2][1]) &&
		(iCards[0][1]==iCards[3][1]) &&
		(iCards[0][1]==iCards[4][1]) )
		return TRUE;
	return FALSE;
}


BOOL CIssPokerHandEval::Straight(int iCards[5][2])
{
	if( iCards[0][0]==iCards[1][0]+1 &&
		iCards[0][0]==iCards[2][0]+2 &&
		iCards[0][0]==iCards[3][0]+3 &&
		iCards[0][0]==iCards[4][0]+4)
		return TRUE;
	if(iCards[0][0]==13 && iCards[1][0]==12 && iCards[2][0]==11 && iCards[3][0]==10 && iCards[4][0]==1)
		return TRUE;
	return FALSE;
}

BOOL CIssPokerHandEval::StraightFlush(int iCards[5][2])
{
	if(Flush(iCards))
	{
		return Straight(iCards);
	}
	return FALSE;
}

BOOL CIssPokerHandEval::RoyalFlush(int iCards[5][2])
{
	if(Flush(iCards))
	{
		if(iCards[0][0]==13 && iCards[1][0]==12 && iCards[2][0]==11 && iCards[3][0]==10 && iCards[4][0]==1)
			return TRUE;
	}
	return FALSE;
}


BOOL CIssPokerHandEval::FourOfaKind(int iCards[5][2])
{
	if((iCards[1][0]==iCards[2][0]) &&
		(iCards[1][0]==iCards[3][0]))
	{
		if(iCards[0][0] == iCards[1][0] ||
			iCards[4][0] == iCards[1][0])
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKind2s10s(int iCards[5][2])
{
	if(FourOfaKind(iCards))
	{
		if(iCards[2][0] > 1 && iCards[2][0] < 11)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKindJsKs(int iCards[5][2])
{
	if(FourOfaKind(iCards))
	{
		if(iCards[2][0] > 10 && iCards[2][0] < 14)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKind5sKs(int iCards[5][2])
{
	if(FourOfaKind(iCards))
	{
		if(iCards[2][0] > 4 && iCards[2][0] < 14)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKind2s4s(int iCards[5][2])
{
	if(FourOfaKind(iCards))
	{
		if(iCards[2][0] > 1 && iCards[2][0] < 5)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKind2s4swA(int iCards[5][2])
{
	if(FourOfaKind2s4s(iCards))
	{
		//is the last card an Ace?
		if(iCards[4][0] == 1)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKindAces(int iCards[5][2])
{
	if(FourOfaKind(iCards))
	{
		if(iCards[2][0] == 1)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::FourOfaKindAcesw234(int iCards[5][2])
{
	if(FourOfaKindAces(iCards))
	{
		//is the first card a 2, 3, or 4?
		if(iCards[0][0] == 2 || iCards[0][0] == 3 || iCards[0][0] == 4)
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::ThreeOfaKind(int iCards[5][2])
{
	if((iCards[0][0]==iCards[1][0]) &&
		(iCards[0][0]==iCards[2][0]))
		return TRUE;
	else if((iCards[1][0]==iCards[2][0]) &&
		(iCards[1][0]==iCards[3][0]) )
		return TRUE;
	else if((iCards[2][0]==iCards[3][0]) &&
		(iCards[2][0]==iCards[4][0]))
		return TRUE;
	return FALSE;
}

BOOL CIssPokerHandEval::FullHouse(int iCards[5][2])
{
	if(ThreeOfaKind(iCards))
	{
		if(iCards[0][0]==iCards[1][0] &&
			iCards[0][0]!=iCards[2][0])
			return TRUE;
		if(iCards[3][0]==iCards[4][0] &&
			iCards[2][0]!=iCards[4][0])
			return TRUE;
	}
	return FALSE;
}

BOOL CIssPokerHandEval::Pair(int iCards[5][2])
{
	if(iCards[0][0]==iCards[1][0] ||
		iCards[1][0]==iCards[2][0] ||
		iCards[2][0]==iCards[3][0] ||
		iCards[3][0]==iCards[4][0])
		return TRUE;
	return FALSE;
}

BOOL CIssPokerHandEval::SixesOrBetter(int iCards[5][2])
{
    if(	(iCards[0][0]==iCards[1][0]	&& (iCards[0][0] > 5 || iCards[0][0]==1	))	||
        (iCards[1][0]==iCards[2][0]	&& (iCards[1][0] > 5 || iCards[1][0]==1	))	||
        (iCards[2][0]==iCards[3][0]	&& (iCards[2][0] > 5 || iCards[2][0]==1	))	||
        (iCards[3][0]==iCards[4][0]	&& (iCards[3][0] > 5 || iCards[3][0]==1	)))
        return TRUE;
    return FALSE;

}

BOOL CIssPokerHandEval::TensOrBetter(int iCards[5][2])
{
	if(	(iCards[0][0]==iCards[1][0]	&& (iCards[0][0] > 9 || iCards[0][0]==1	))	||
		(iCards[1][0]==iCards[2][0]	&& (iCards[1][0] > 9 || iCards[1][0]==1	))	||
		(iCards[2][0]==iCards[3][0]	&& (iCards[2][0] > 9 || iCards[2][0]==1	))	||
		(iCards[3][0]==iCards[4][0]	&& (iCards[3][0] > 9 || iCards[3][0]==1	)))
		return TRUE;
	return FALSE;

}

BOOL CIssPokerHandEval::JacksOrBetter(int iCards[5][2])
{
	if(	(iCards[0][0]==iCards[1][0]	&& (iCards[0][0] > 10 || iCards[0][0]==1 ))	||
		(iCards[1][0]==iCards[2][0]	&& (iCards[1][0] > 10 || iCards[1][0]==1 ))	||
		(iCards[2][0]==iCards[3][0]	&& (iCards[2][0] > 10 || iCards[2][0]==1 ))	||
		(iCards[3][0]==iCards[4][0]	&& (iCards[3][0] > 10 || iCards[3][0]==1 )))
		return TRUE;
	return FALSE;

}

BOOL CIssPokerHandEval::TwoPair(int iCards[5][2])
{
	if(iCards[0][0]==iCards[1][0] && 
		iCards[2][0]==iCards[3][0])
		return TRUE;
	if(iCards[0][0]==iCards[1][0] && 
		iCards[3][0]==iCards[4][0])
		return TRUE;
	if(iCards[1][0]==iCards[2][0] && 
		iCards[3][0]==iCards[4][0])
		return TRUE;
	return FALSE;
}



BOOL CIssPokerHandEval::SetPokerHand(CIssVPHand oHand)
{
	for(int i = 0; i < 5; i++)
	{
		m_oCard[i] = oHand.GetCard(i);
		if(m_oCard[i]==NULL)
			return FALSE;
	}
	return TRUE;

}

BOOL CIssPokerHandEval::SetCard(CIssCard* oCard, int i)
{
	if(i < 0 || i > 4)
		return FALSE;

	m_oCard[i] = oCard;

	if(m_oCard == NULL)
		return FALSE;


	return TRUE;
}

int CIssPokerHandEval::GetResultsCount(EnumPokerType eType)
{
    //pair, 2 pair, 3of, 4of, FH, straight, flush, SF, RF
    switch(eType)
    {
    case PTYPE_AcesAndFaces:
        return 11;
        break;
    case PTYPE_DoubleBonus:
        return 11;
        break;
    case PTYPE_BonusDeluxe:
        return 13;
        break;
    case PTYPE_StudVP:
        return 10;
        break;
    default://10s,Jacks,Deluxe
        return 9;
        break;
    }
    return 0;
}

//for video poker really
BOOL CIssPokerHandEval::IsResultAval(EnumPokerType eType, EnumPokerResult eResult)
{
    //these are always good
    switch(eResult)
    {
    case VPRESULT_2Pairs:
    case VPRESULT_3OfaKind:
    case VPRESULT_FullHouse:
    case VPRESULT_Straight:
    case VPRESULT_Flush:
    case VPRESULT_StraightFlush:
    case VPRESULT_RoyalFlush:
        return TRUE;
        break;
    case VPRESULT_5OfaKind:
    case VPRESULT_RoyalFlushNatural:
    case VPRESULT_HighCard:
        return FALSE;
        break;
    }

    //10s or better
    if(eType == PTYPE_10sOrBetter)
    {
        if(eResult == VPRESULT_4OfaKind || eResult == VPRESULT_10sOrBetter)
            return TRUE;
    }
    //jacks or better and bonus deluxe
    else if(eType == PTYPE_JacksOrBetter || eType == PTYPE_BonusDeluxe)
    {
        if(eResult == VPRESULT_4OfaKind || eResult == VPRESULT_JacksOrBetter)
            return TRUE;
    }
    //aces and faces
    else if(eType == PTYPE_AcesAndFaces)
    {
        if(eResult == VPRESULT_4OfaKindAces ||
            eResult == VPRESULT_4OfaKind2s10s || 
            eResult == VPRESULT_4OfaKindJsKs)
            return TRUE;
    }
    //double bonus
    else if(eType == PTYPE_DoubleBonus)
    {
        if(eResult == VPRESULT_4OfaKindAces ||
            eResult == VPRESULT_4OfaKind2s4s || 
            eResult == VPRESULT_4OfaKind5sKs)
            return TRUE;
    }
    else if(eType == PTYPE_StudVP)
    {
        if(eResult == VPRESULT_6sOrBetter ||
            eResult == VPRESULT_JacksOrBetter)
            return TRUE;
    }

    return FALSE;
}


