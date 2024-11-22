#include "PanoVPGame.h"

CPanoVPGame::CPanoVPGame(void)
:m_eBetAmounts(BA_1)
,m_iNumCredits(0)
{
}

CPanoVPGame::~CPanoVPGame(void)
{
}

void CPanoVPGame::Reset()
{
    //lets just check some enums here too
    if(m_eBetAmounts < 0 || BA_50 > m_eBetAmounts)
        BA_1;
    if(m_eGameType < 0 || m_eGameType > PTYPE_StudVP)
        m_eGameType = PTYPE_JacksOrBetter;

    m_iBank			= 10000;
    m_iOldBank		= 10000;
    m_iBetAmount	= GetAmount(m_eBetAmounts);
    m_eGameState	= VPSTATE_Bet;
    

    for(int i = 0; i < DEFAULT_NUM_CARDS; i++)
    {
        m_bHold[i]		= FALSE;
        for(int j = 0; j < MAX_MUTLIPLAY; j++)
            m_bShowing[j][i]	= FALSE;
    }

    for(int i = 0; i < MAX_MUTLIPLAY; i++)
    {
        m_oCardSet[i].ChangeNumDecks(m_iNumDecks);
        m_oCardSet[i].Shuffle();
    }
    NewHand();
}

void CPanoVPGame::SetBetAmount(EnumBetAmounts eBet)
{
    //reset and bet once
    if(m_eGameState == VPSTATE_Bet || m_eGameState == VPSTATE_EndGame)
    {
        OnBetOne();
        ClearBet();
    }
    m_eBetAmounts = eBet;    //set anyways and we'll catch up on the fresh deal
}


BOOL CPanoVPGame::ClearBet()
{
    if(m_iBetAmount == 0)
        return FALSE;

    m_iOldBank = m_iBank;

    m_iBetAmount = 0;
    m_iNumCredits = 0;

    return TRUE;
}


//we'll loop around for now
BOOL CPanoVPGame::OnBetOne()
{
    if(m_eGameState != VPSTATE_Bet && m_eGameState != VPSTATE_EndGame)
        return FALSE;

    if(m_iBank > GetAmount(m_eBetAmounts) || m_iNumCredits == 5)
    {
        m_iNumCredits++;
        if(m_iNumCredits > 5)
        {
            m_iNumCredits = 1;
            m_iBetAmount = GetAmount(m_eBetAmounts);
        }
        else
        {
            m_iBetAmount += GetAmount(m_eBetAmounts);
        }
    }

    return TRUE;
}

BOOL CPanoVPGame::OnBetMax()
{
    if(m_eGameState != VPSTATE_Bet &&
		m_eGameState != VPSTATE_EndGame)
        return FALSE;

    int iIncrease = 0;
    iIncrease = 5 - m_iNumCredits;

    if(iIncrease*(GetAmount(m_eBetAmounts)) > m_iBank)
        iIncrease = m_iBank/GetAmount(m_eBetAmounts);

    m_iNumCredits += iIncrease;
    m_iBetAmount += iIncrease*GetAmount(m_eBetAmounts);

    //off we go
    return OnDeal();
}

BOOL CPanoVPGame::OnDeal()
{
    if(m_eGameState == VPSTATE_Bet)
        CheckBet();
    if(m_iBetAmount == 0 || m_iNumCredits == 0)
        return FALSE;

    return CIssVPGame::Deal();
}

BOOL CPanoVPGame::IsHeld(int iCard)
{
    if(iCard < 0 || iCard > 4)
        return FALSE;

    if(m_bHold[iCard] == FALSE)
        return FALSE;
    if(m_bShowing[0][iCard] == FALSE)
        return FALSE;

    if(m_eGameState != VPSTATE_Hold)
        //m_eGameState != VPSTATE_Deal2)//leave em held for the second deal
        return FALSE;

    return TRUE;
}

BOOL CPanoVPGame::IsWinner(int iCard, int iHand)
{
    if(iCard < 0 || iCard > 4)
        return FALSE;
    if(iHand < 0 || iHand >= m_iNumMultiplay)
        return FALSE;

    if(m_eGameState != VPSTATE_EndGame)
        return FALSE;

    if(m_eResult[iHand] == VPRESULT_Lose)
        return FALSE;

    switch(m_eResult[iHand])
    {
    case VPRESULT_Pair:
    case VPRESULT_6sOrBetter:
    case VPRESULT_10sOrBetter:
    case VPRESULT_JacksOrBetter:
    case VPRESULT_2Pairs:
    case VPRESULT_3OfaKind:    
    case VPRESULT_4OfaKind:
        //we've got a pair ... only highlight the win
        for(int i = 0; i < 5; i++)
        {
            if(i == iCard)
                continue;
            //just gotta find one match
            if(m_oHand[iHand].GetCard(iCard)->GetCardRank() == m_oHand[iHand].GetCard(i)->GetCardRank())
                return TRUE;
        }
        return FALSE;
        break;
    default://all other wins involve 5 cards
        return TRUE;
        break;
    }

    return FALSE;

}

int CPanoVPGame::GetPayoutCount()
{
    return m_oHandEval.GetResultsCount(m_eGameType);
    return 0;
}

BOOL CPanoVPGame::OnDouble()
{
    if(m_eGameType != PTYPE_StudVP)
        return FALSE;

    if(m_eGameState != VPSTATE_Hold)
        return FALSE;

    //double the bet if possible
    m_bDouble = FALSE;
    
    //here we do want to check the bank
    if(m_iBank >= m_iBetAmount*m_iNumMultiplay)
    {
        m_iBank -= m_iBetAmount*m_iNumMultiplay;
        m_iBetAmount += m_iBetAmount;
        m_iNumCredits += m_iNumCredits;
        m_iOldBank = m_iBank; //not sure if this is nesessary
        m_bDouble = TRUE;
    }

    //and deal the final card
    OnDeal();

    return TRUE;
}

void CPanoVPGame::UpdateBank()
{
	m_iOldBank = m_iBank;
    m_eBestResult = VPRESULT_Lose;
    m_iAmountWon = 0;

    for(int i = 0; i < m_iNumMultiplay; i++)
    {
        m_iAmountWon += m_iBetAmount*(m_oScoreCalc.GetPayoutRatio(m_eGameType, EvaluateHand(i))); 
        if(m_eResult[i] < m_eBestResult)
            m_eBestResult = m_eResult[i];

    }

	if(m_bDouble)
	{
        m_iAmountWon *= 2;
	}

    m_iBank += m_iAmountWon;

}

int CPanoVPGame::GetAmount(EnumBetAmounts eAmt)
{
	switch(eAmt)
	{
	default:
	case BA_1:
		return 1;
		break;
	case BA_5:
		return 5;
		break;
	case BA_10:
		return 10;
	    break;
	case BA_20:
		return 20;
	    break;
	case BA_50:
		return 50;
		break;
	case BA_100:
		return 100;
		break;
	}
}

EnumBetAmounts CPanoVPGame::GetBetAmount()
{
    return m_eBetAmounts;
}



int CPanoVPGame::GetBet(TCHAR* szBet)
{
    if(m_eGameState == VPSTATE_Bet && m_bDouble)
        CheckBet();

    if(szBet)
        m_oStr->IntToString(szBet, m_iBetAmount);

     return m_iBetAmount; 
}



//set it to the correct ammount and then reduce the bank
void CPanoVPGame::CheckBet()
{
    //put the bet back in
 //   m_iBank += m_iBetAmount;
 //   m_iBetAmount = 0;

    if(m_bDouble)
    {
        m_iNumCredits /= 2;
        m_iBetAmount /= 2;
        m_bDouble = FALSE;
    }
    
    //check credits & multi play
    int iMultiplier = 1;
    if(m_eMultiplay == MPLAY_5)
        iMultiplier = 5;
    else if(m_eMultiplay == MPLAY_3)
        iMultiplier = 3;
    
    int iCredMulti = GetAmount(m_eBetAmounts);


    //first reduce the credits
    while(m_iBank < iCredMulti * iMultiplier * m_iNumCredits && 
        m_iNumCredits > 1)
    {
        m_iNumCredits--;
    }

    //then the credit multiplier
    while(m_iBank < GetAmount(m_eBetAmounts) * iMultiplier * m_iNumCredits && 
        m_eBetAmounts > BA_1)
    {
        m_eBetAmounts = (EnumBetAmounts)(m_eBetAmounts - 1);
    }

    //are we still over?
    if(m_iBank < GetAmount(m_eBetAmounts) * iMultiplier * m_iNumCredits)
    {
        return; //we're done
    }
   
    //set everything to the way it should be
    m_iBetAmount = m_iNumCredits*GetAmount(m_eBetAmounts);
    m_iBank -= m_iBetAmount*iMultiplier; //reduce the bank by the correct amount
}
