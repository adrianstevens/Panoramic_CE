#pragma once

#include "IssVPHand.h"

enum EnumPokerType
{
	PTYPE_10sOrBetter,		//Video Poker
	PTYPE_JacksOrBetter,	//Video Poker
    PTYPE_BonusDeluxe,		//Video Poker
    PTYPE_AcesAndFaces,		//Video Poker
	PTYPE_DoubleBonus,		//Video Poker
	PTYPE_StudVP,           //Video Poker style Stud
    PTYPE_DucesWild,		//Video Poker
    PTYPE_Straight,			//Poker
	PTYPE_American,			//Poker Solitaire
	PTYPE_English,			//Poker Solitaire
    
    //double double not in enum but in engine
};

//in order of value for most games
enum EnumPokerResult
{
    VPRESULT_5OfaKind,
    VPRESULT_RoyalFlushNatural,
    VPRESULT_RoyalFlush,
    VPRESULT_4OfaKindAcesw234,
    VPRESULT_4OfaKindAces,
    VPRESULT_4OfaKind2s4swA,
    VPRESULT_4OfaKind2s4s,
    VPRESULT_4OfaKindJsKs,
    VPRESULT_4OfaKind5sKs,
    VPRESULT_4OfaKind2s10s,
    VPRESULT_4OfaKind2s,
    VPRESULT_4OfaKind,
    VPRESULT_StraightFlush,
    VPRESULT_FullHouse,
    VPRESULT_Flush,
    VPRESULT_Straight,
    VPRESULT_3OfaKind,
    VPRESULT_2Pairs,
    VPRESULT_JacksOrBetter,
    VPRESULT_10sOrBetter,
    VPRESULT_6sOrBetter,    //stud
    VPRESULT_Pair,
    VPRESULT_HighCard,
    VPRESULT_Lose,
};

/*
enum EnumPokerResult
{
	VPRESULT_Lose,
	VPRESULT_JacksOrBetter,
	VPRESULT_2Pairs,
	VPRESULT_3OfaKind,
	VPRESULT_4OfaKind,
	VPRESULT_FullHouse,
	VPRESULT_Straight,
	VPRESULT_Flush,
	VPRESULT_StraightFlush,
	VPRESULT_RoyalFlush,

	VPRESULT_Pair,
    VPRESULT_6sOrBetter,    //stud
	VPRESULT_10sOrBetter,
	VPRESULT_4OfaKind2s4s,
	VPRESULT_4OfaKind2s4swA,
	VPRESULT_4OfaKind5sKs,
	VPRESULT_4OfaKind2s10s,
	VPRESULT_4OfaKindJsKs,
	VPRESULT_4OfaKindAces,
	VPRESULT_4OfaKindAcesw234,
	VPRESULT_4OfaKind2s,
	VPRESULT_5OfaKind,
	VPRESULT_RoyalFlushNatural,
	VPRESULT_HighCard,
};*/

class CIssPokerHandEval
{
public:
	CIssPokerHandEval(void);
public:
	~CIssPokerHandEval(void);


	BOOL						SetPokerHand(CIssVPHand oHand);
	BOOL						SetCard(CIssCard* oCard, int i);
	EnumPokerResult				EvaluateHand(EnumPokerType eType);

    int                         GetResultsCount(EnumPokerType eType);
    BOOL                        IsResultAval(EnumPokerType eType, EnumPokerResult eResult);

private:

	//So the easiest way to do this is to separate everything out (easier to bug check too)
	EnumPokerResult				EvaluateJacksOrBetter(int iCards[5][2]);
	EnumPokerResult				Evaluate10sOrBetter(int iCards[5][2]);
	EnumPokerResult				EvaluateAcesAndFaces(int iCards[5][2]);
	EnumPokerResult				EvaluateStraight(int iCards[5][2]);
	EnumPokerResult				EvaluateDoubleBonus(int iCards[5][2]);
	EnumPokerResult				EvaluateDoubleDouble(int iCards[5][2]);
	EnumPokerResult				EvaluateBonusDeluxe(int iCards[5][2]);
    EnumPokerResult             EvaluateStud(int iCards[5][2]);
	

	//Now we break down every hand we want to check for
	//These functions assume the cards are in order 
	BOOL						RoyalFlush(int iCards[5][2]);
	BOOL						StraightFlush(int iCards[5][2]);
	BOOL						Straight(int iCards[5][2]);
	BOOL						Flush(int iCards[5][2]);
	BOOL						FourOfaKind(int iCards[5][2]);
	BOOL						FullHouse(int iCards[5][2]);
	BOOL						ThreeOfaKind(int iCards[5][2]);
	BOOL						TwoPair(int iCards[5][2]);
	BOOL						Pair(int iCards[5][2]);


	//Video Poker Specifics
    BOOL						SixesOrBetter(int iCards[5][2]);
	BOOL						TensOrBetter(int iCards[5][2]);
	BOOL						JacksOrBetter(int iCards[5][2]);

	//We'll assume we've already confirmed 4 of a Kind
	BOOL						FourOfaKind2s4s(int iCards[5][2]);
	BOOL						FourOfaKind5sKs(int iCards[5][2]);
	BOOL						FourOfaKind2s10s(int iCards[5][2]);
	BOOL						FourOfaKindJsKs(int iCards[5][2]);
	BOOL						FourOfaKindAces(int iCards[5][2]);
	BOOL						FourOfaKindAcesw234(int iCards[5][2]);//double double
	BOOL						FourOfaKind2s4swA(int iCards[5][2]);//double double


private:
	CIssCard					*m_oCard[5];

	



};