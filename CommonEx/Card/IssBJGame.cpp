// IssBJGame.cpp: implementation of the CIssBJGame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IssBJGame.h"
#include "IssCommon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssBJGame::CIssBJGame()
:m_hWnd(NULL)
,m_bNewGame(FALSE)
,m_eCardCountView(CC_On)
{
	m_oStr = CIssString::Instance();

	m_oCardSet = new CIssCardSet(FALSE);
	m_bPlayEndSound = FALSE;

	SetDifficulty(BJDIFF_Normal);
    m_eCountSystem = COUNT_HiLow;

    Reset();

}

CIssBJGame::~CIssBJGame()
{
	delete m_oCardSet;
}

void CIssBJGame::MatchChipsToBet()
{
    int iTemp = m_iBetAmount;

    //zero and recalculate
    m_iNum100Chips = 0;
    m_iNum50Chips = 0;
    m_iNum25Chips = 0;

    while(iTemp >= 100 && m_iNum100Chips < MAX_CHIPS_PER_STACK)
    {
        m_iNum100Chips++;
        iTemp -= 100;
    }
    while(iTemp >= 50 && m_iNum50Chips < MAX_CHIPS_PER_STACK)
    {
        m_iNum50Chips++;
        iTemp -= 50;
    }
    while(iTemp >= 25 && m_iNum25Chips < MAX_CHIPS_PER_STACK)
    {
        m_iNum25Chips++;
        iTemp -= 25;
    } 

    if(iTemp > 0)
    {
        m_iBank += iTemp;//should never happen
        m_iBetAmount -= iTemp;
    }
}

BOOL CIssBJGame::SetDifficulty(EnumGameDifficulty eDiff)
{
	switch(eDiff)
	{
	case BJDIFF_Easy:
		m_iNumDecks				= 1;
		m_bBurnFirstCard		= FALSE;
		m_bBlackJack2to1		= TRUE;
		m_bDealerHitsOnSoft17	= TRUE;
		m_b5CardsScores21		= TRUE;
		m_bShowScores			= TRUE;
		m_bAllowSurrender		= TRUE;
		m_bShowDealerCard		= TRUE;
		break;
	case BJDIFF_Normal:
		m_iNumDecks				= 2;
		m_bBurnFirstCard		= FALSE;
		m_bBlackJack2to1		= FALSE;
		m_bDealerHitsOnSoft17	= TRUE;
		m_b5CardsScores21		= FALSE;
		m_bShowScores			= TRUE;
		m_bAllowSurrender		= TRUE;
		m_bShowDealerCard		= FALSE;
		break;
	case BJDIFF_Expert:
		m_iNumDecks				= 4;
		m_bBurnFirstCard		= TRUE;
		m_bBlackJack2to1		= FALSE;
		m_bDealerHitsOnSoft17	= FALSE;
		m_b5CardsScores21		= FALSE;
		m_bShowScores			= FALSE;
		m_bAllowSurrender		= FALSE;
		m_bShowDealerCard		= FALSE;
		break;
    case BJDIFF_Learn:
        m_iNumDecks				= 1;
        m_bBurnFirstCard		= FALSE;
        m_bBlackJack2to1		= TRUE;
        m_bDealerHitsOnSoft17	= TRUE;
        m_b5CardsScores21		= FALSE;
        m_bShowScores			= TRUE;
        m_bAllowSurrender		= TRUE;
        m_bShowDealerCard		= FALSE;
        break;
	default:
		return FALSE;
		break;
	}

	m_eGameDiff = eDiff; 

	return TRUE;
}

BOOL CIssBJGame::HasGameStarted()
{
    if(m_iBetAmount==0&&m_iBank==BANK_DEFAULT)
        return FALSE;
    else
        return TRUE;
}

void CIssBJGame::Reset()
{
	NewHandReset();
	m_iBank			= BANK_DEFAULT;
    m_iBankHi       = BANK_DEFAULT;
    m_iPreviousBank = BANK_DEFAULT;
	m_iBetAmount	= 0;
	m_bPlayEndSound = TRUE;
	m_bNewGame		= TRUE;

    m_iNum25Chips = 0;
    m_iNum50Chips = 0;
    m_iNum100Chips = 0;

    m_iCardCount    = 0;

	m_eGameState	= GSTATE_Bet;
	m_oCardSet->ChangeNumDecks(m_iNumDecks);
	m_oCardSet->Shuffle();
}

void CIssBJGame::NewHandReset()
{
	m_bSplit		= FALSE;
	m_bDoubleDown	= FALSE;
	m_bCalcDoubleDown = FALSE;
	m_bSurrender	= FALSE;
	m_bInsurance	= FALSE;

	//Clear the Hands
	m_oDealerHand.ClearHand();
	m_oPlayerHand.ClearHand();
	m_oSplitHand.ClearHand();

	m_iNumDealerCardsDealt = 0;
	m_iNumPlayerCardsDealt = 0;
	m_iNumSplitCardsDealt = 0;

	//and finally check if we need to shuffle the decks
	//maybe we should toss up an indicator??
#define CARD_LIMIT 20
	if(m_oCardSet->GetCardsLeft() <= CARD_LIMIT)
    {
		m_oCardSet->Shuffle();
        m_iCardCount = 0;
        PostMessage(m_hWnd, WM_SHUFFLE, 0, 0);
    }
}


BOOL CIssBJGame::AddBetAmount(int iAmount)
{
	if((m_eGameState != GSTATE_Bet) &&
		(m_eGameState != GSTATE_EndOfGame))
		return FALSE;
	else if(m_eGameState == GSTATE_EndOfGame)
		m_eGameState = GSTATE_Bet;

	if(iAmount < 0 ||
		iAmount > m_iBank)
		return FALSE;

	m_iBetAmount	+= iAmount;
	m_iBank			-= iAmount;

	return TRUE;
}

BOOL CIssBJGame::ResetBetAmount()
{
	if((m_eGameState != GSTATE_Bet) &&
		(m_eGameState != GSTATE_EndOfGame))
		return FALSE;
	else if(m_eGameState == GSTATE_EndOfGame)
		m_eGameState = GSTATE_Bet;

	if(m_iBetAmount < 1)
		return FALSE;

	m_iBank += m_iBetAmount;
	
	m_iBetAmount = 0;

    m_iNum25Chips = 0;
    m_iNum50Chips = 0;
    m_iNum100Chips = 0;

	return TRUE;
}

BOOL CIssBJGame::EvaluateScores()
{
    m_iPreviousBank = m_iBank;

	//Now we figure out if we've won and do the payouts
	int iPlayer, iDealer, iSplit;
	m_oPlayerHand.GetScore(m_iLow, iPlayer);
	m_oDealerHand.GetScore(m_iLow, iDealer);
	m_oSplitHand.GetScore(m_iLow, iSplit);

	if(m_b5CardsScores21)
	{
		if(iPlayer < 22 &&
			m_oPlayerHand.GetNumberOfCards() == 5)
			iPlayer = 21;

		if(iSplit < 22 &&
			m_oSplitHand.GetNumberOfCards() == 5)
			iSplit = 21;

		if(iDealer < 22 &&
			m_oDealerHand.GetNumberOfCards() == 5)
			iDealer = 21;
	}

	m_eResultSplit  = GRESULT_NULL;

	//Special Cases first
	if(m_bSurrender)
	{
		m_bSurrender = FALSE;
		m_eResultPlayer = GRESULT_Surrender;
	}
	else
	{	//Player Hand
		if(iPlayer > 21)
			m_eResultPlayer = GRESULT_Lose;
		else if(iPlayer == iDealer)
        {
            if(iDealer == 21)
            {   //we both have bj ... bj doesn't count if you split
                if (m_oPlayerHand.GetNumberOfCards() == 2 && m_oDealerHand.GetNumberOfCards() == 2 && IsSplitting() == FALSE)
                    m_eResultPlayer = GRESULT_Draw;
                //we have bj ... dealer doesn't ... will never happen 
                else if (m_oPlayerHand.GetNumberOfCards() == 2 && m_oDealerHand.GetNumberOfCards() > 2 && IsSplitting() == FALSE)
                    m_eResultPlayer = GRESULT_BlackJack;
                //we don't have bj but the dealer does               
                else if (m_oDealerHand.GetNumberOfCards() == 2)
                    m_eResultPlayer = GRESULT_Lose;
                else //neither has bj
                    m_eResultPlayer = GRESULT_Draw;
            }
            else //less than 21 .. just a draw
            {
                m_eResultPlayer = GRESULT_Draw;
            }
        }
		else if(iPlayer == 21 &&
			m_oPlayerHand.GetNumberOfCards()==2 &&
            IsSplitting() == FALSE)
			m_eResultPlayer = GRESULT_BlackJack;
		else if(iDealer > 21)
			m_eResultPlayer = GRESULT_Win;
		else if(iDealer > iPlayer)
			m_eResultPlayer = GRESULT_Lose;
		else if(iPlayer > iDealer)
			m_eResultPlayer = GRESULT_Win;

		if(m_bSplit)
		{
			if(iSplit > 21)
				m_eResultSplit = GRESULT_Lose;
			else if(iSplit == iDealer)
            {
                //if the dealer has blackjack we lose
                if (iDealer == 21 &&
                    m_oDealerHand.GetNumberOfCards() == 2)
                    m_eResultSplit = GRESULT_Lose;
                else//otherwise its just a tie
                    m_eResultSplit = GRESULT_Draw;
            }
			else if(iDealer > 21)
				m_eResultSplit = GRESULT_Win;
			else if(iDealer > iSplit)
				m_eResultSplit = GRESULT_Lose;
            else
                m_eResultSplit = GRESULT_Win; //we have a higher score
		}
	}

	//Now lets do the payout...this could be in another function but doesn't really matter
	switch(m_eResultPlayer)
	{
	case GRESULT_Win:
		m_iBank += m_iBetAmount;
		break;
	case GRESULT_BlackJack:
		if(m_bBlackJack2to1)
			m_iBank += 2*m_iBetAmount;
		else
			m_iBank += 3*m_iBetAmount/2;
		break;
	case GRESULT_NULL:
	case GRESULT_Draw:
		//leave it
		break;
	case GRESULT_Lose:
		m_iBank -= m_iBetAmount;
		break;
	case GRESULT_Surrender:
		m_iBank -= m_iBetAmount/2;
		break;
	}

	switch(m_eResultSplit)
	{
	case GRESULT_Win:
		m_iBank += m_iBetAmount;
		break;
	case GRESULT_BlackJack:
		if(m_bBlackJack2to1)
			m_iBank += 2*m_iBetAmount;
		else
			m_iBank += 3*m_iBetAmount/2;
		break;
	case GRESULT_NULL:
	case GRESULT_Draw:
		//leave it
		break;
	case GRESULT_Lose:
		m_iBank -= m_iBetAmount;
		break;
	}

    if(m_bSplit)
        m_iBank += m_iBetAmount;//when we split we take another bet out 
        //after the bet is reduced so we put that money back into the bank

	if(m_bInsurance)
	{
		if(m_oDealerHand.GetNumberOfCards() == 2 && 
			iDealer == 21)
		m_iBank += m_iBetAmount; //Insurance Pays off sucka!
	}

	if(m_bCalcDoubleDown)
	{
		m_iBetAmount /= 2;
		m_iBank += m_iBetAmount;
		m_bCalcDoubleDown = FALSE;
	}

	while(m_iBank < 0)
	{
		m_iBetAmount--;
		m_iBank++;
	}

    if(m_iBank == 0)
        MatchChipsToBet();

    //post the results message
    PostMessage(m_hWnd, WM_BJRESULTS, (DWORD)m_eResultPlayer, (DWORD)m_eResultSplit);

	m_eGameState = GSTATE_EndOfGame; 

	if(m_iBank == 0 && m_iBetAmount == 0)
	{	//the game is over ... send a message to the dlg
		PostMessage(m_hWnd, WM_BJ_ENDGAME, NULL, NULL);
	}

    if((m_iBank + m_iBetAmount) > m_iBankHi)
        m_iBankHi = m_iBank + m_iBetAmount;

    return TRUE;
}

BOOL CIssBJGame::CheckGameStatus()
{
	int iTemp;
	m_oDealerHand.GetScore(m_iLow, m_iHi);
	m_oPlayerHand.GetScore(m_iLow, iTemp);

	//We'll call this constantly to see what's going on....in case of black jacks, busts, etc it will change the game state
	switch(m_eGameState)
	{
	case GSTATE_PlayerFirstMove:
		//check for blackjack
		{
			if(iTemp == 21)
				//blackjack
				EvaluateScores();
		}
		break;
	case GSTATE_PlayerMove:
		{	
			if(m_bSplit)
			{
                if(m_iLow == 21 || iTemp == 21)//check the first two calls .. this is right
                {
                    Stand();
                    CheckGameStatus();
                }
			}
			else
			{
				if(m_iLow>21)//you're done sucka
					EvaluateScores();
				else if(m_iLow == 21 || iTemp == 21)
					PlayDealerHand();
			}
		}
		break;
	case GSTATE_SplitMove:
		{
			m_oSplitHand.GetScore(m_iLow, m_iHi);
			if(m_iLow >= 21)
				PlayDealerHand();
            if(m_iLow == 21 || m_iHi == 21)
                Stand();
		}
		break;
	case GSTATE_DealerMove:
		{
			m_oDealerHand.GetScore(m_iLow, m_iHi);
			if(m_iLow >= 21)
				EvaluateScores(); //now I know what you're thinking what about holding on 17??...dealt with 
		}
	default:
		break;
	
	}
	return TRUE;

}

BOOL CIssBJGame::PlayDealerHand()
{
    CIssCard* oCard = NULL;

	if(m_eGameState == GSTATE_DealerMove)
	{
		//check and see if we even need to play it out
		m_oPlayerHand.GetScore(m_iLow, m_iHi);
		if(m_iLow > 21)
		{
			if(m_iLow > 21)
            {
                if(m_bSplit)
                    m_oSplitHand.GetScore(m_iLow, m_iHi);
                if(m_iLow > 21)//this is fine .. checks twice for one hand
                    //checks each hand for split
				    return TRUE;
            }
		}

		m_oDealerHand.GetScore(m_iLow, m_iHi);
		
//		while(m_iHi < 17) // have to hit
		while(1 == 1)
		{
			if(m_iHi > 16 && m_iHi < 22)
				break; //we're all good
			if(m_iLow > 16)
				break; //also good
            oCard = m_oCardSet->GetNextCard();
            AddCardToCount(oCard);
			m_oDealerHand.AddCard(oCard);
			m_oDealerHand.GetScore(m_iLow, m_iHi);
		}
		
		//check for soft 17
		if(m_bDealerHitsOnSoft17 &&
			m_iLow == 7 && m_iHi == 17) // hit on soft 17 baby
		{
            oCard = m_oCardSet->GetNextCard();
            AddCardToCount(oCard);
			m_oDealerHand.AddCard(oCard);
		}
		
		EvaluateScores();

		return TRUE;
	}

	return FALSE;


}

BOOL CIssBJGame::Deal()
{
    if(ShowDealButton()==FALSE)
        return FALSE;

	if(m_eGameState != GSTATE_Bet &&
		m_eGameState != GSTATE_EndOfGame)
		return FALSE;
	m_eGameState = GSTATE_Deal;
	NewHandReset();
	m_bPlayEndSound = TRUE;
	m_bNewGame = FALSE;

    CIssCard* nextCard = NULL;

	//Deal two cards to Player and Dealer
	//might need more control here....or maybe the interface can worry about the animation

    nextCard = m_oCardSet->GetNextCard();
    AddCardToCount(nextCard);
	m_oPlayerHand.AddCard(nextCard);

    nextCard = m_oCardSet->GetNextCard();
	AddCardToCount(nextCard);
    m_oDealerHand.AddCard(nextCard);
	
    nextCard = m_oCardSet->GetNextCard();
	AddCardToCount(nextCard);
    m_oPlayerHand.AddCard(nextCard);
	
    nextCard = m_oCardSet->GetNextCard();
	AddCardToCount(nextCard);
    m_oDealerHand.AddCard(nextCard);

	m_eGameState = GSTATE_PlayerFirstMove;

	return CheckGameStatus();
}

BOOL CIssBJGame::Bet()
{
    if(m_eGameState != GSTATE_EndOfGame)
        return FALSE;

    m_eGameState = GSTATE_Bet;
    
    return TRUE;
}


BOOL CIssBJGame::Hit()
{
    CIssCard* oCard = NULL;

	//Normal
	if(m_eGameState == GSTATE_PlayerMove ||
		m_eGameState == GSTATE_PlayerFirstMove)
	{
        oCard = m_oCardSet->GetNextCard();
        AddCardToCount(oCard);
		m_oPlayerHand.AddCard(oCard);

		//now check if we've busted for the correct game state
		m_oPlayerHand.GetScore(m_iLow, m_iHi);
		if(m_iLow > 21 || //busted
			(m_oPlayerHand.GetNumberOfCards() == 5 && m_b5CardsScores21)) //done
		{
            if(m_bSplit)
				m_eGameState = GSTATE_SplitMove;
			else
            {
				EvaluateScores();
                SendMessage(m_hWnd, WM_DEALER_FLIP, NULL, NULL);
            }

		}
        else if(m_iLow == 21 || m_iHi == 21)//no reason to hit again 
        {
            return Stand();
        /*    if(m_bSplit)
            {
                m_eGameState = GSTATE_SplitMove;
            }
            else
            {
                m_eGameState = GSTATE_DealerMove;
            }*/
        }
		else
        {
			m_eGameState = GSTATE_PlayerMove;
        }
	}


	//Split
	else if(m_eGameState == GSTATE_SplitMove)
	{
        oCard = m_oCardSet->GetNextCard();
        AddCardToCount(oCard);
        m_oSplitHand.AddCard(oCard);

        int iLow, iHi;
        m_oPlayerHand.GetScore(iLow, iHi);
		m_oSplitHand.GetScore(m_iLow, m_iHi);

        if(m_iLow > 21 && iLow > 21)
        {   //both hands are busted .. we're done
            EvaluateScores();
            SendMessage(m_hWnd, WM_DEALER_FLIP, NULL, NULL);
        }
        else if(m_iLow > 21)
        {   //just the split hand has busted so dealer gets to play
            return Stand(); //lazy but works
        }
        //if both hands have blackjack the dealer doesn't play
        else if(m_iLow == 21 && iLow == 21 && m_oPlayerHand.GetNumberOfCards() == 2 && m_oSplitHand.GetNumberOfCards() == 2)
        {
            EvaluateScores();
            SendMessage(m_hWnd, WM_DEALER_FLIP, NULL, NULL);
        }

        //if we have 21 we auto stand
		else if(m_iLow == 21 || m_iHi == 21 ||
            (m_oSplitHand.GetNumberOfCards() == 5 && m_b5CardsScores21)) //done
        {
            return Stand();
            //dealer knows how to play .. it can flip itself
        }
	}
	else
	{
		return FALSE;
	}
	return CheckGameStatus();
}

BOOL CIssBJGame::Stand()
{
	//Normal
	if(m_eGameState == GSTATE_PlayerFirstMove ||
		m_eGameState == GSTATE_SplitMove)
	{
		m_eGameState = GSTATE_DealerMove;
	}
	else if(m_eGameState == GSTATE_PlayerMove)
	{
		if(m_bSplit)
			m_eGameState = GSTATE_SplitMove;
		else
			m_eGameState = GSTATE_DealerMove;
	}
	else
		return FALSE; 

	if(m_eGameState == GSTATE_DealerMove)
		//Make the dealer play out his hand
        SendMessage(m_hWnd, WM_DEALER_FLIP, NULL, NULL);
		PlayDealerHand();

	return TRUE;
}

BOOL CIssBJGame::Split()
{
    CIssCard* oCard = NULL;

	if(ShowSplitButton())
	{	//we split!
		m_iBank -= m_iBetAmount;

		m_oSplitHand.AddCard(m_oPlayerHand.GetCard(1));//copy the pointer to the card to the Split Hand
		m_oPlayerHand.RemoveCard(1);	//delete the pointer to the Player Hand
		//Deal two more cards

        oCard = m_oCardSet->GetNextCard();
        AddCardToCount(oCard);
        m_oPlayerHand.AddCard(oCard);

        oCard = m_oCardSet->GetNextCard();
        AddCardToCount(oCard);
        m_oSplitHand.AddCard(oCard);

		m_bSplit		= TRUE;
		m_eGameState	= GSTATE_PlayerMove;
		return CheckGameStatus();
	}
	return FALSE;
}


BOOL CIssBJGame::Insurance()
{
	if(ShowInsuranceButton())
	{
		m_iBank -= m_iBetAmount/2;
		m_bInsurance = TRUE;
		m_eGameState = GSTATE_PlayerMove;
	}
	return FALSE;
}

BOOL CIssBJGame::DoubleDown()
{
	if(ShowDoubleButton())
	{
		m_bDoubleDown = TRUE;
		m_bCalcDoubleDown = m_bDoubleDown;
		m_iBank -= m_iBetAmount;
		m_iBetAmount *= 2;

        CIssCard* oCard = NULL;

        oCard = m_oCardSet->GetNextCard();
        AddCardToCount(oCard);
        m_oPlayerHand.AddCard(oCard);

		m_eGameState = GSTATE_DealerMove;
		CheckGameStatus();
		PlayDealerHand();

		//ok this engine is kind of shit
		int iLow, iHigh;
		m_oPlayerHand.GetScore(iLow, iHigh);
		if(iHigh > 21)
			EvaluateScores();
		return TRUE;
	}
	return FALSE;
}

BOOL CIssBJGame::Surrender()
{
	if(ShowSurrenderButton())
	{
		m_bSurrender = TRUE;
        EvaluateScores();
		return TRUE;
	}
	return FALSE;
}

BOOL CIssBJGame::ShowDoubleButton()
{
	if(m_eGameState == GSTATE_PlayerFirstMove &&
		m_iBank >= m_iBetAmount)
		return TRUE;
	else
		return FALSE;
}

BOOL CIssBJGame::ShowInsuranceButton()
{
	if(m_eGameState == GSTATE_PlayerFirstMove &&
        m_iBank >= m_iBetAmount/2)
	{
		CIssCard *oCard;

		oCard = m_oDealerHand.GetCard(0);

		if(oCard->m_eRank == RANK_Ace)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CIssBJGame::ShowSplitButton()
{

	if(m_eGameState == GSTATE_PlayerFirstMove && 
        m_iBank >= m_iBetAmount)
	{
		CIssCard* oTempCard1;
		CIssCard* oTempCard2;

		oTempCard1 = m_oPlayerHand.GetCard(0);
		oTempCard2 = m_oPlayerHand.GetCard(1);
#ifndef DEBUG
		if(oTempCard2->GetCardValue()== oTempCard1->GetCardValue() &&
			m_iBank >= m_iBetAmount)
#endif
			return TRUE;
	}
	return FALSE;
}


BOOL CIssBJGame::ShowSurrenderButton()
{
	if(m_bAllowSurrender == FALSE)
		return FALSE;

	if(m_eGameState == GSTATE_PlayerFirstMove)
	{
		return TRUE;
	}
	return FALSE;
}

void CIssBJGame::GetBank(TCHAR* szBank)
{
	m_oStr->IntToString(szBank, m_iBank);

}

void CIssBJGame::GetBet(TCHAR* szBet)
{
	m_oStr->IntToString(szBet, m_iBetAmount);
}

void CIssBJGame::GetScore(TCHAR* szScore, CIssBJHand* oHand)
{
	m_oStr->Empty(szScore);
	if(m_bShowScores == FALSE)
		return;

	oHand->GetScore(m_iLow, m_iHi);
		
	if(m_iHi < 22 &&
		m_iLow != m_iHi)
	{	//show both scores
		TCHAR sz2ndScore[4];
		m_oStr->IntToString(sz2ndScore, m_iHi);
		m_oStr->IntToString(szScore, m_iLow);

		m_oStr->Concatenate(szScore, _T(" ("));
		m_oStr->Concatenate(szScore, sz2ndScore);
		m_oStr->Concatenate(szScore, _T(")"));
	}
	else
	{
		m_oStr->IntToString(szScore, m_iLow);
	}

	if(m_iLow == 0)
		m_oStr->Empty(szScore);
}

void CIssBJGame::GetDealerScore(TCHAR* szDealerScore)
{
	m_oStr->Empty(szDealerScore);
	if(m_bShowDealerCard == TRUE ||
		m_eGameState == GSTATE_EndOfGame)
		GetScore(szDealerScore, &m_oDealerHand);
}

BOOL CIssBJGame::ShowDealerScore()
{
	if(m_bShowScores == FALSE)
	{
		if(m_eGameState != GSTATE_EndOfGame)
			return FALSE;
	}
	
	if(m_iNumDealerCardsDealt < m_oDealerHand.GetNumberOfCards())
		return FALSE;

	switch(m_eGameState)
	{
	case GSTATE_Bet:
	case GSTATE_PlayerMove:
	case GSTATE_SplitMove:
	case GSTATE_DealerMove:
		return FALSE;
		break;
	case GSTATE_PlayerFirstMove:
	case GSTATE_Deal:
	case GSTATE_EndOfGame:
		return TRUE;
		break;
	}

	return FALSE;
}

BOOL CIssBJGame::ShowPlayerScore()
{
	if(m_bShowScores == FALSE)
	{
		if(m_eGameState != GSTATE_EndOfGame)
			return FALSE;
	}
	
	if(m_iNumPlayerCardsDealt < m_oPlayerHand.GetNumberOfCards())
		return FALSE;

	switch(m_eGameState)
	{
	case GSTATE_PlayerMove:
	case GSTATE_SplitMove:
	case GSTATE_DealerMove:
	case GSTATE_PlayerFirstMove:
	case GSTATE_Deal:
	case GSTATE_EndOfGame:
		return TRUE;
		break;
	default:
	case GSTATE_Bet:
		return FALSE;
		break;
	}

	return FALSE;
}

BOOL CIssBJGame::ShowSplitScore()
{
	if(m_bSplit == FALSE)
		return FALSE;
	
	else if(m_bShowScores == FALSE)
	{
		if(m_eGameState != GSTATE_EndOfGame)
			return FALSE;
	}
	if(m_iNumSplitCardsDealt < m_oSplitHand.GetNumberOfCards())
		return FALSE;

	switch(m_eGameState)
	{
	case GSTATE_Bet:
	case GSTATE_PlayerMove:
	case GSTATE_SplitMove:
	case GSTATE_DealerMove:
//		return FALSE;
//		break;
	case GSTATE_PlayerFirstMove:
	case GSTATE_Deal:
	case GSTATE_EndOfGame:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CIssBJGame::ShowResults()
{
	if(m_eGameState == GSTATE_EndOfGame)
		return TRUE;
	return FALSE;
}

BOOL CIssBJGame::IsFinishedDealing()
{
    if(m_bDoubleDown == TRUE && m_iNumPlayerCardsDealt == 2)
        return FALSE;
    else if(m_iNumDealerCardsDealt < m_oDealerHand.GetNumberOfCards())
        return FALSE;
    else if(m_iNumPlayerCardsDealt < m_oPlayerHand.GetNumberOfCards())
        return FALSE;
    else if(m_iNumSplitCardsDealt < m_oSplitHand.GetNumberOfCards())
        return FALSE;
    return TRUE;

}


//Now we want to be careful with the order of the cards dealt
BOOL CIssBJGame::DealAnotherCard()
{
	if(m_bDoubleDown == TRUE && m_iNumPlayerCardsDealt == 2)
	{
		m_iNumPlayerCardsDealt++;
		return TRUE;
	}
	else if(m_iNumDealerCardsDealt < m_oDealerHand.GetNumberOfCards())
	{
		m_iNumDealerCardsDealt++;
		return TRUE;
	}
	else if(m_iNumPlayerCardsDealt < m_oPlayerHand.GetNumberOfCards())
	{
		m_iNumPlayerCardsDealt++;
		return TRUE;
	}
	else if(m_iNumSplitCardsDealt < m_oSplitHand.GetNumberOfCards())
	{
		m_iNumSplitCardsDealt++;
		return TRUE;
	}
	return FALSE;
}

EnumGameDifficulty CIssBJGame::GetDifficulty()
{
	return m_eGameDiff;
}

BOOL CIssBJGame::Add25ChipBet()
{
    if(Show25() == FALSE)
        return FALSE;


    if(AddBetAmount(25))
    {
        m_iNum25Chips++;
        if(m_iNum25Chips > MAX_CHIPS_PER_STACK)
        {
            m_iNum25Chips -= 2;
            m_iNum50Chips++;

            if(m_iNum50Chips > MAX_CHIPS_PER_STACK)
            {
                m_iNum50Chips -= 2;
                m_iNum100Chips++;
            }
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CIssBJGame::Add50ChipBet()
{
    if(Show50() == FALSE)
        return FALSE;

    if(AddBetAmount(50))
    {
        m_iNum50Chips++;

        if(m_iNum50Chips > MAX_CHIPS_PER_STACK)
        {
            m_iNum50Chips -= 2;
            m_iNum100Chips++;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CIssBJGame::Add100ChipBet()
{
    if(Show100() == FALSE)
        return FALSE;

    if(AddBetAmount(100))
    {
        if(m_iNum100Chips < MAX_CHIPS_PER_STACK)
        {   
            m_iNum100Chips++;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CIssBJGame::Add500ChipBet()
{
    if(Show500() == FALSE)
        return FALSE;

    for(int i = 0; i < 5; i++)
    {
        if(Add100ChipBet()==FALSE)
            return FALSE;
    }
    return TRUE;
}


BOOL CIssBJGame::AddCardToCount(CIssCard* oCard)
{
    if(oCard == NULL)
        return FALSE;

    //subtract by one because 0 rank is Joker
    m_iCardCount += GetCardCountValue(oCard);

    return TRUE;
}

int CIssBJGame::GetCardCountValue(CIssCard* oCard)
{
    static int iCountValues[COUNT_Count][13] =
    {   //Ace 2  3  4  5  6  7  8  9 10  J  Q  K
        {-1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},//wizard
        {-1, 1, 1, 1, 1, 1, 1, 0, 0,-1,-1,-1,-1},//KO
        {-1, 1, 1, 1, 1, 1, 0, 0, 0,-1,-1,-1,-1},//HiLow
        { 0, 0, 1, 1, 1, 1, 0, 0, 0,-1,-1,-1,-1},//HiOpt1
        { 0, 1, 1, 2, 2, 1, 1, 0, 0,-2,-2,-2,-2},//HiOpt2
        {-1, 1, 1, 2, 2, 2, 1, 0, 0,-2,-2,-2,-2},//Zen
        { 0, 1, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2,-2}//OmegaII
    };

    return iCountValues[m_eCountSystem][oCard->GetCardRank()-1];

}

BOOL CIssBJGame::DealerBusts()
{
    int hi, low; 
    
    m_oDealerHand.GetScore(low, hi); 
    
    if(low > 21)
        return TRUE;
    
    return FALSE;
}

void CIssBJGame::SetBetAmount(int iBet)
{
    if(iBet > 0)
        m_iBetAmount = iBet;

    MatchChipsToBet();
}

BOOL CIssBJGame::Show25()
{
    if(m_iBank < 25)
        return FALSE;

    if(m_iNum25Chips >= MAX_CHIPS_PER_STACK)
        return FALSE;

    return TRUE;
}

BOOL CIssBJGame::Show50()
{
    if(m_iBank < 50)
        return FALSE;

    if(m_iNum50Chips >= MAX_CHIPS_PER_STACK)
        return FALSE;

    return TRUE;
}

BOOL CIssBJGame::Show100()
{
    if(m_iBank < 100)
        return FALSE;

    if(m_iNum100Chips >= MAX_CHIPS_PER_STACK)
        return FALSE;

    return TRUE;
}

BOOL CIssBJGame::Show500()
{
    if(m_iBank < 500)
        return FALSE;

    if(m_iNum100Chips >= MAX_CHIPS_PER_STACK - 4)
        return FALSE;

    return TRUE;
}

int CIssBJGame::GetCardCount()
{
    if(m_eGameDiff == BJDIFF_Easy)
        return m_iCardCount;

    //we don't know the dealer card value yet
    if(m_oDealerHand.GetCard(1) && DealerCardFaceUp() == FALSE)
        return m_iCardCount - GetCardCountValue(m_oDealerHand.GetCard(1));

    return m_iCardCount;
}

BOOL CIssBJGame::DealerCardFaceUp()
{
    if(m_eGameDiff == BJDIFF_Easy)
        return TRUE;

    switch(m_eGameState)
    {
    case GSTATE_Bet:
        break;
    case GSTATE_Deal:
        if(m_bShowDealerCard != FALSE)
            return TRUE;
    case GSTATE_PlayerFirstMove:
    case GSTATE_PlayerMove:
    case GSTATE_SplitMove:
        return FALSE;
        break;
    case GSTATE_DealerMove:
    case GSTATE_EndOfGame:
    	break;
    }
    return TRUE;
}


//hard totals, soft totals, and pairs
EnumNextMove CIssBJGame::GetNextMove()
{
    EnumNextMove eMove = NM_Nothing;

    CIssBJHand* oPlayerHand = NULL;

    if(m_eGameState == GSTATE_SplitMove)
        oPlayerHand = &m_oSplitHand;
    else 
        oPlayerHand = &m_oPlayerHand;

    if(oPlayerHand == NULL)
        goto End;

    if(m_oDealerHand.GetNumberOfCards() != 2 || oPlayerHand->GetNumberOfCards() < 2)
        goto End;

    if(GetGameState() != GSTATE_PlayerMove &&
        GetGameState() != GSTATE_PlayerFirstMove &&
        GetGameState() != GSTATE_SplitMove)
        goto End;
    
    static EnumNextMove ePairs[10][10] = 
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp},//a
        {NM_H , NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_H , NM_H , NM_H },//2
        {NM_H , NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_H , NM_H , NM_H },//3
        {NM_H , NM_H , NM_H , NM_H , NM_Sp, NM_Sp, NM_H , NM_H , NM_H , NM_H },//4
        {NM_H , NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_H },//5
        {NM_H , NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_H , NM_H , NM_H , NM_H },//6
        {NM_H , NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_H , NM_H , NM_H },//7
        {NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp},//8
        {NM_S , NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_Sp, NM_S , NM_Sp, NM_Sp, NM_S },//9
        {NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S }//10
    };

    static EnumNextMove eSoft[10][10] = 
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H },//a .. should split but we'll always hit if not
        {NM_H , NM_H , NM_H , NM_H , NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//2
        {NM_H , NM_H , NM_H , NM_H , NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//3
        {NM_H , NM_H , NM_H , NM_DH, NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//4
        {NM_H , NM_H , NM_H , NM_DH, NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//5
        {NM_H , NM_H , NM_DH, NM_DH, NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//6
        {NM_H , NM_S , NM_DS, NM_DS, NM_DS, NM_DS, NM_S , NM_S , NM_S , NM_H },//7
        {NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S },//8
        {NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S },//9
        {NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S }//10 this is blackjack holmes
    };

    //and finally ... based on the total values
    static EnumNextMove eHard[9][10] = 
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S , NM_S },//17-20
        {NM_Su, NM_S , NM_S , NM_S , NM_S , NM_S , NM_H , NM_Su, NM_Su, NM_Su},//16
        {NM_H , NM_S , NM_S , NM_S , NM_S , NM_S , NM_H , NM_H , NM_H , NM_Su},//15
        {NM_H , NM_S , NM_S , NM_S , NM_S , NM_S , NM_H , NM_H , NM_H , NM_H },//13-14
        {NM_H , NM_H , NM_H , NM_S , NM_S , NM_S , NM_H , NM_H , NM_H , NM_H },//12
        {NM_H , NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH},//11
        {NM_H , NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_DH, NM_H },//10
        {NM_H , NM_H , NM_DH, NM_DH, NM_DH, NM_DH, NM_H , NM_H , NM_H , NM_H },//9
        {NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H , NM_H },//5-8
    };

    //we'll do pairs first
    if(oPlayerHand->GetNumberOfCards() == 2 && 
        oPlayerHand->GetCard(0)->GetCardValue() == oPlayerHand->GetCard(1)->GetCardValue())
    {   //0 position is a joker for card value
        if(ShowSplitButton())
        {
            eMove = ePairs[oPlayerHand->GetCard(0)->GetCardValue()-1][m_oDealerHand.GetCard(0)->GetCardValue()-1];
            goto End;
        }
        //otherwise we'll fall through to the hard and soft totals
    }

    //might as well do the soft totals next
    int iHi, iLow;
    oPlayerHand->GetScore(iLow, iHi);
    if(iLow != iHi && iHi < 22)
    {   //we have a valid soft score
        int iNonAce = iLow - 1;
        eMove = eSoft[iNonAce - 1][m_oDealerHand.GetCard(0)->GetCardValue()-1];
        goto End;
    }

    //otherwise we've got some hard totals going on
    int iIndex = 0;
    if(iLow > 16)
        iIndex = 0;
    else if(iLow > 15)
        iIndex = 1;
    else if(iLow > 14)
        iIndex = 2;
    else if(iLow > 12)
        iIndex = 3;
    else if(iLow > 11)
        iIndex = 4;
    else if(iLow > 10)
        iIndex = 5;
    else if(iLow > 9)
        iIndex = 6;
    else if(iLow > 8)
        iIndex = 7;
    else
        iIndex = 8;

    eMove = eHard[iIndex][m_oDealerHand.GetCard(0)->GetCardValue()-1];


End:
    //now we'll correct the choices with two possible moves
    if(eMove == NM_DH && ShowDoubleButton() == FALSE)
        eMove = NM_H;
    if(eMove == NM_DS && ShowDoubleButton() == FALSE)
        eMove = NM_S;
    if(eMove == NM_DS || eMove == NM_DH)
        eMove = NM_D;
    if(eMove == NM_Su && ShowSurrenderButton() == FALSE)
        eMove = NM_H;

    return eMove;
}