#pragma once
#include "stdafx.h"
#include "issvpgame.h"

enum EnumBetAmounts
{
    BA_1,
    BA_5,
    BA_10,
    BA_20,
    BA_50,
    BA_100,
 //   BA_500,
 //   BA_1000,
    BA_Count,
};

class CPanoVPGame : public CIssVPGame
{
public:
    CPanoVPGame(void);
    ~CPanoVPGame(void);

    EnumBetAmounts      GetBetAmount();
    void                SetBetAmount(EnumBetAmounts eBet);


    BOOL                OnBetOne();
    BOOL                OnBetMax();
    BOOL                OnDeal();

    BOOL                OnDouble();//for stud poker
    BOOL                IsDouble(){if(m_eGameType == PTYPE_StudVP && m_bDouble) return TRUE; return FALSE;};

    BOOL				ClearBet();

    BOOL                IsHeld(int iCard);
    void                Reset();
    BOOL                IsWinner(int iCard, int iHand = 0);

    int                 GetPayoutCount();//number of payout types
    int                 GetBet(TCHAR* szBet  = NULL);
    int                 GetAmountWon(){return m_iAmountWon;};

    BOOL                IsMaxBet(){if(m_iNumCredits == 5) return TRUE; return FALSE;};

private:
	void				UpdateBank();

	int					GetAmount(EnumBetAmounts eAmt);

    void                CheckBet();

private:



};
