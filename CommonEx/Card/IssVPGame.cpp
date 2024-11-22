#include "StdAfx.h"
#include "IssVPGame.h"
#include "IssRegistry.h"
#include "IssCommon.h"

#define STARTING_BANK (m_bBankBroken?50000:10000)

CIssVPGame::CIssVPGame(void)
:m_iNumDecks(1)//should only ever be one
,m_iNumMultiplay(1)
,m_eGameType(PTYPE_DoubleBonus)
,m_bBonusGame(TRUE)
,m_eMultiplay(MPLAY_Off)
,m_bAutoHold(TRUE)
,m_bBankBroken(FALSE)
{
	m_oStr = CIssString::Instance();
	ResetStats();
    Reset();
}

CIssVPGame::~CIssVPGame(void)
{
	
}
void CIssVPGame::Reset()
{
    //lets just check some enums here too
    if(m_eBetAmounts < 0 || BA_50 > m_eBetAmounts)
        BA_1;
    if(m_eGameType < 0 || m_eGameType > PTYPE_StudVP)
        m_eGameType = PTYPE_JacksOrBetter;

	m_iBank			= STARTING_BANK;
	m_iOldBank		= STARTING_BANK;
    m_iBetAmount	= GetAmount(m_eBetAmounts);
    m_iNumCredits   = 1;
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

void CIssVPGame::SetBetAmount(EnumBetAmounts eBet)
{
    //reset and bet once
    m_eBetAmounts = eBet;    //set anyways and we'll catch up on the fresh deal
    if(m_eGameState == VPSTATE_Bet || m_eGameState == VPSTATE_EndGame)
    {
        ClearBet();
        OnBetOne();
    }
}


BOOL CIssVPGame::SetMultiplay(EnumMultiPlay eMulti)
{
    m_eMultiplay = eMulti;

    if(m_eGameState == VPSTATE_EndGame || m_eGameState == VPSTATE_Bet)
    {
        if(m_eMultiplay == MPLAY_Off)
            m_iNumMultiplay = 1;
        else if(m_eMultiplay == MPLAY_3)
            m_iNumMultiplay = 3;
        else 
            m_iNumMultiplay = 5;
    }

    return TRUE;
}

void CIssVPGame::NewHand()
{	
	m_eGameState		= VPSTATE_Bet;	

}
BOOL CIssVPGame::Hold(int iButton)
{
	if(m_eGameState != VPSTATE_Hold)
		return FALSE;
    if(m_eGameType == PTYPE_StudVP)
        return FALSE;

	if(iButton < 0 || iButton >= DEFAULT_NUM_CARDS)
		return FALSE;

	m_bHold[iButton]= !m_bHold[iButton];
	return TRUE;
}

BOOL CIssVPGame::AddBet(int iBet)
{
	if(m_eGameState != VPSTATE_Bet)
		return FALSE;

	if(iBet+m_iBetAmount > m_iBank)
		return FALSE;

	m_iBetAmount	+= iBet;
//	m_iBank			-= iBet;
//	m_iOldBank		= m_iBank;

	return TRUE;
}

BOOL CIssVPGame::ClearBet()
{
    if(m_iBetAmount == 0)
        return FALSE;

    m_iOldBank = m_iBank;

    m_iBetAmount = 0;
    m_iNumCredits = 0;

    return TRUE;
}

BOOL CIssVPGame::DealBonusGame()
{
	return TRUE;
}

BOOL CIssVPGame::Deal()
{
    //set the multiplay so we don't break hands mid game (kinda clever)
    switch(m_eMultiplay)
    {
    default:
    case MPLAY_Off:
        m_iNumMultiplay = 1;
    	break;
    case MPLAY_3:
        m_iNumMultiplay = 3;
    	break;
    case MPLAY_5:
        m_iNumMultiplay = 5;
        break;
    }

    if(m_eGameType == PTYPE_StudVP)
        return DealStud();

	switch(m_eGameState)
	{
	case VPSTATE_Bet:
	case VPSTATE_EndGame:
		{
			m_eGameState = VPSTATE_Deal;
            for(int i = 0; i < m_iNumMultiplay; i++)
            {
                m_oHand[i].ClearHand();
                m_oCardSet[i].Shuffle();
            }
			for(int i = 0; i < DEFAULT_NUM_CARDS; i++)
			{
				m_bHold[i]		= FALSE;
				m_bShowing[0][i]	= FALSE;
		
				// get a new card
				m_oHand[0].AddCard(m_oCardSet[0].GetNextCard());
			}
		}
		break;
	case VPSTATE_Deal://its dealing 
		return FALSE;
		break;
	case VPSTATE_Hold:
		{
            m_eGameState = VPSTATE_Deal2;

            //and setup the multiplay hands
            SetMultiplayHands();
			
			for(int i = 0; i < DEFAULT_NUM_CARDS; i ++)
			{
				if(m_bHold[i] == FALSE)
				{
					m_bHold[i] = TRUE;
					m_bShowing[0][i] = FALSE;
					m_oHand[0].RemoveCard(i);
					m_oHand[0].AddCard(m_oCardSet[0].GetNextCard());
				}
			}
		}
		break;
	case VPSTATE_Deal2://its dealing ... relax
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CIssVPGame::DealStud()
{
    switch(m_eGameState)
    {
    case VPSTATE_Bet:
    case VPSTATE_EndGame:
        {
            m_bDouble = FALSE;//perfect
            m_eGameState = VPSTATE_Deal;
            m_oHand[0].ClearHand();
            for(int i = 0; i < m_iNumMultiplay; i++)
            {
                m_oHand[i].ClearHand();
                m_oCardSet[i].Shuffle();
            }
            for(int i = 0; i < DEFAULT_NUM_CARDS; i++)
            {
                m_bHold[i]		= FALSE;
                m_bShowing[0][i]	= FALSE;

                // get a new card
                if(i < 4) //only 4
                    m_oHand[0].AddCard(m_oCardSet[0].GetNextCard());
            }
        }
        break;
    case VPSTATE_Deal://its dealing 
        return FALSE;
        break;
    case VPSTATE_Hold:
        {
            //Deal the 4th Card
            m_eGameState = VPSTATE_Deal2;
            //and setup the multiplay hands
            SetMultiplayStud();
            m_bHold[4]		= FALSE;
            m_bShowing[0][4]	= FALSE;
            m_oHand[0].AddCard(m_oCardSet[0].GetNextCard());
        }
        break;
    case VPSTATE_Deal2://its dealing ... relax
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL CIssVPGame::DealNextCard()
{
	int i;

    int jCount = 1;
    if(m_eGameState == VPSTATE_Deal2)
        jCount = m_iNumMultiplay;

    for(int j = jCount; j > 0; j--)
    {
        for(i=0; i<DEFAULT_NUM_CARDS; i++)
	    {
            if(!m_bShowing[j-1][i])
            {
                m_bShowing[j-1][i] = TRUE;
                goto out; // kinda lazy but its fine
            }
        }
	}

out:
	
    //only deal 4 cards initially for stud
    if(m_eGameType == PTYPE_StudVP && m_eGameState == VPSTATE_Deal && i == 4)
        m_eGameState = (m_eGameState==VPSTATE_Deal2?VPSTATE_EndGame:VPSTATE_Hold);

    // change the game state if all the cards are showing
	if(i >= DEFAULT_NUM_CARDS)
		m_eGameState = (m_eGameState==VPSTATE_Deal2?VPSTATE_EndGame:VPSTATE_Hold);

    if(m_eGameState == VPSTATE_Hold && m_eGameType != PTYPE_StudVP)
        AutoHold();


	// here we should calculate our payout if we win or lose
	if(m_eGameState == VPSTATE_EndGame)
	{
		UpdateBank();
	}

	return TRUE;
}

CIssCard* CIssVPGame::GetCard(int iIndex, int iSet)
{
	if(iIndex < 0 || iIndex >= DEFAULT_NUM_CARDS)
		return NULL;
    if(iSet < 0 || iSet >= m_iNumMultiplay)
        return NULL;

	// is the card being shown
	if(!m_bShowing[iSet][iIndex])
		return NULL;

	return m_oHand[iSet].GetCard(iIndex);
};






EnumPokerResult CIssVPGame::EvaluateHand(int iHand)
{
    if(iHand < 0 || iHand > m_iNumMultiplay - 1)
        return VPRESULT_Lose;

	m_oHandEval.SetPokerHand(m_oHand[iHand]);
	m_eResult[iHand] = m_oHandEval.EvaluateHand(m_eGameType);
    return m_eResult[iHand];
}

int CIssVPGame::GetBank(TCHAR* szBank)
{
    int iTemp = m_iBank;
 //   if(m_eGameState == VPSTATE_EndGame || m_eGameState == VPSTATE_Bet)
 //       iTemp = m_iBetAmount + m_iBank;

	if(szBank)
		m_oStr->IntToString(szBank, iTemp);

	return iTemp;
}

BOOL CIssVPGame::IsBonusAval()
{
    if(m_bBonusGame == FALSE)
        return FALSE;

    if(m_eGameState != VPSTATE_EndGame)
        return FALSE;

    if(m_eResult[0] == VPRESULT_JacksOrBetter ||
        m_eResult[0] == VPRESULT_10sOrBetter)
        return TRUE;

    return TRUE;
}

void CIssVPGame::SetNumHands(int iMulti)
{
    if(iMulti < 1)
        return;
    if(iMulti >= MAX_MUTLIPLAY)
        return;
    m_iNumMultiplay = iMulti;
}

void CIssVPGame::SetMultiplayHands()
{
    if(m_iNumMultiplay < 2)
        return;//nothing to do
    if(m_iNumMultiplay > MAX_MUTLIPLAY)
        return; 

    if(m_eGameType == PTYPE_StudVP)
    {
        SetMultiplayStud();
        return;
    }

    CIssCard* oCard = NULL;

    //first we'll adjust the multiplay decks ... this should be quick
    for(int i = 0; i < 5; i++)
    {
        if(m_bHold[i])
        {   //first hand is good
            oCard = m_oHand[0].GetCard(i);

            for(int j = 1; j < m_iNumMultiplay; j++)
                if(oCard)
                    m_oCardSet[j].MoveToEndOfDeck(oCard->m_eRank, oCard->m_eSuit);
        }
    }

    //ok we're going to just pass the pointer in from the original card set
    //our hand evaluator won't care
    for(int i = 0; i < 5; i++)
    {
        for(int j = 1; j < m_iNumMultiplay; j++)
        {
            if(m_bHold[i])
            {
                m_oHand[j].AddCard(m_oHand[0].GetCard(i), i);
                m_bShowing[j][i] = TRUE;
            }
            else
            {
                m_oHand[j].AddCard(m_oCardSet[j].GetNextCard(), i);
                m_bShowing[j][i] = FALSE;
            }
        }
    }
}


void CIssVPGame::SetMultiplayStud()
{
    if(m_iNumMultiplay < 2)
        return;//nothing to do
    if(m_iNumMultiplay > MAX_MUTLIPLAY)
        return; 

    CIssCard* oCard = NULL;

    //set the first 4 cards to the end of the decks
    for(int i = 0; i < 4; i++)
    {
        oCard = m_oHand[0].GetCard(i);

        for(int j = 1; j < m_iNumMultiplay; j++)
            if(oCard)
                m_oCardSet[j].MoveToEndOfDeck(oCard->m_eRank, oCard->m_eSuit);

    }

    //ok we're going to just pass the pointer in from the original card set
    //our hand evaluator won't care
    for(int i = 0; i < 5; i++)
    {
        for(int j = 1; j < m_iNumMultiplay; j++)
        {
            if(i < 4)
            {
                m_oHand[j].AddCard(m_oHand[0].GetCard(i), i);
                m_bShowing[j][i] = TRUE;
            }
            else
            {
                m_oHand[j].AddCard(m_oCardSet[j].GetNextCard(), i);
                m_bShowing[j][i] = FALSE;
            }
        }
    }
}

BOOL CIssVPGame::IsWinningResult(EnumPokerResult eResult)
{
    for(int i = 0; i < m_iNumMultiplay; i++)
    {
        if(eResult == m_eResult[i])
            return TRUE;
    }
    return FALSE;
}


//we'll loop around for now
BOOL CIssVPGame::OnBetOne()
{
    if(m_eGameState != VPSTATE_Bet && m_eGameState != VPSTATE_EndGame)
        return FALSE;

    if(m_iBank < GetAmount(m_eBetAmounts))
        CheckBet(FALSE);

    if(m_iBank > GetAmount(m_eBetAmounts)*(m_iNumCredits) || m_iNumCredits == 5)
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

BOOL CIssVPGame::OnBetMax()
{
    if(m_eGameState != VPSTATE_Bet &&
        m_eGameState != VPSTATE_EndGame)
        return FALSE;

    if(m_iNumCredits == 5)
        CheckBet(FALSE);
    else
        for(int i = m_iNumCredits; i < 5; i++)
            OnBetOne();//just try

    return OnDeal();
}

BOOL CIssVPGame::OnDeal()
{
    if(m_eGameState == VPSTATE_Bet || m_eGameState == VPSTATE_EndGame)
        CheckBet(TRUE);
    if(m_iBetAmount == 0 || m_iNumCredits == 0 || m_iBank < 0)//we've already reduced the bank
        return FALSE;

    return CIssVPGame::Deal();
}

BOOL CIssVPGame::IsHeld(int iCard)
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

BOOL CIssVPGame::IsWinner(int iCard, int iHand)
{
    if(iCard < 0 || iCard > 4)
        return FALSE;
    if(iHand < 0 || iHand >= m_iNumMultiplay)
        return FALSE;

//    if(m_eGameState != VPSTATE_EndGame)
//        return FALSE;
    if(m_oHand[iHand].GetNumberOfCards() < 5)
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

int CIssVPGame::GetPayoutCount()
{
    return m_oHandEval.GetResultsCount(m_eGameType);
}

BOOL CIssVPGame::OnDouble()
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

void CIssVPGame::UpdateBank()
{
    m_iOldBank = m_iBank;
    m_eBestResult = VPRESULT_Lose;
    m_iAmountWon = 0;
    int iTempAmnt = 0;

    for(int i = 0; i < m_iNumMultiplay; i++)
    {
        iTempAmnt = m_iBetAmount*(m_oScoreCalc.GetPayoutRatio(m_eGameType, EvaluateHand(i))); 

        //update the biggest single hand win
        if(iTempAmnt > m_iBiggestWin)
            m_iBiggestWin = iTempAmnt;
        
        m_iAmountWon += iTempAmnt;
        if(m_eResult[i] < m_eBestResult)
            m_eBestResult = m_eResult[i];
    }

    m_iBank += m_iAmountWon;

    if(m_iBank >= MAX_BANK)
        m_bBankBroken = TRUE;

    //update the other stats
    UpdateStats();
}

int CIssVPGame::GetAmount(EnumBetAmounts eAmt)
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

EnumBetAmounts CIssVPGame::GetBetAmount()
{
    return m_eBetAmounts;
}



int CIssVPGame::GetBet(TCHAR* szBet)
{
    if(m_eGameState == VPSTATE_Bet && m_bDouble)
        CheckBet(FALSE);

    if(szBet)
        m_oStr->IntToString(szBet, m_iBetAmount);

    return m_iBetAmount; 
}



//set it to the correct amount and then reduce the bank
void CIssVPGame::CheckBet(BOOL bUpdateBank /* = TRUE */)
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

    if(m_iNumCredits == 0)
        m_iNumCredits = 1;

    //then the credit multiplier
    while(m_iBank < GetAmount(m_eBetAmounts) * iMultiplier * m_iNumCredits && 
        m_eBetAmounts > BA_1)
    {
        m_eBetAmounts = (EnumBetAmounts)(m_eBetAmounts - 1);
    }

    //are we still over?
    if(m_iBank < GetAmount(m_eBetAmounts) * iMultiplier * m_iNumCredits)
    {
        m_iBetAmount = 0;
        m_iNumCredits = 0;
        return; //we're done
    }

    //set everything to the way it should be
    m_iBetAmount = m_iNumCredits*GetAmount(m_eBetAmounts);
    if(bUpdateBank)
        m_iBank -= m_iBetAmount*iMultiplier; //reduce the bank by the correct amount
}

void CIssVPGame::AutoHold()
{
    if(m_bAutoHold == FALSE)
        return;

    //we need a result for this
    m_eResult[0] = EvaluateHand(0);

    for(int i = 0; i < 5; i++)
    {
        if(IsWinner(i,0))
            Hold(i);
    }
}

//nice and easy ... the state machine will take care of the rest
BOOL CIssVPGame::IsBroke()
{
    if(m_iBank < m_iNumMultiplay)
        return TRUE;
    return FALSE;
}

BOOL CIssVPGame::IsGameInProgress()
{
    //good enough ...
    if(m_iBank == 0 && 
        (m_eGameState == VPSTATE_EndGame || m_eGameState == VPSTATE_Bet))
        return FALSE;
    if(m_iBank == STARTING_BANK && m_iOldBank == STARTING_BANK && m_eGameState == VPSTATE_Bet)
        return FALSE;

    return TRUE;
}

void CIssVPGame::UpdateStats()
{
    int i = (int)m_eGameType;
    if(i < 0 || i > 5)
        return;

    if(m_iBank > m_iHighestBank)
        m_iHighestBank = m_iBank;

    //now for the actual stats
    double dbTotal = m_iNumHandsPlayed[i]*m_dbPayout[i];
    //now add in the new payout ratio
    dbTotal += ((double)m_iAmountWon/((double)m_iBetAmount));

    //number of hands played
    m_iNumHandsPlayed[i] += m_iNumMultiplay;

    m_dbPayout[i] = dbTotal/(double)m_iNumHandsPlayed[i];
   
    DebugOut(_T("Hands played: %i"), m_iNumHandsPlayed[i]);
    DebugOut(_T("Payout %%: %f"), m_dbPayout[i]*100.0);

}

void CIssVPGame::ResetStats()
{
    m_iBiggestWin   = 0;
    m_iHighestBank  = 10000;

    for(int i = 0; i < 6; i++)
    {
        m_iNumHandsPlayed[i]    = 0;
        m_dbPayout[i]           = 0.0;
    }
}

void CIssVPGame::SetGameType(EnumPokerType eType)
{
    if(eType < 0 || eType > PTYPE_StudVP)
        return;//stay safe

    m_eGameType = eType;
}
