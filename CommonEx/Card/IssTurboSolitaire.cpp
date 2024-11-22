#include "IssTurboSolitaire.h"
#include "IssRegistry.h"

struct TypeGameState
{
	int				iScore;
	int				iLastScore;
	int				iTime;
	EnumLastMove	eLastMove;
	EnumScoringType eScoreType;
	int				iLastColumn;
	int				iDragColumn;
	int				iMoveCardIndex;
	int				iNextCardIndex;
	int				iPrevDeckCardIndex;
	int				iLastAddition;
	int				iIncrementCounter;
	int				iCardIndexes[CARD_COLUMNS][CARDS_PER_COLUMN];
};

#define REG_HighScore	_T("HiScore")
#define REG_GameState	_T("GameState")
#define REG_GameValues	_T("GameValues")

CIssTurboSolitaire::CIssTurboSolitaire(void)
:m_iScore(0)
,m_iHighScore(0)
,m_iLastScore(0)
,m_iTime(120)
,m_eLastMove(MOVE_None)
,m_eScoreType(SCORE_Normal)
,m_iLastColumn(-1)
,m_iDragColumn(-1)
,m_oStr(CIssString::Instance())
,m_oMovingCard(NULL)
,m_oNextCard(NULL)
,m_oPrevDeckCard(NULL)
,m_hWnd(NULL)
,m_eGameState(GS_NoGame)
,m_iLastAddition(0)
,m_iIncrimentCounter(0)
,m_iLastIncriment(0)
{
	for(int i=0; i < CARD_COLUMNS; i++)
	{	
		for(int j=0; j<CARDS_PER_COLUMN;j++)
		{		
			m_oCards[i][j] = NULL;
		}
	}
}

CIssTurboSolitaire::~CIssTurboSolitaire(void)
{

}

BOOL CIssTurboSolitaire::SaveGameState(TCHAR* szRegLocation)
{
	if(!szRegLocation)
		return FALSE;

	//save the highscore regardless
	DWORD dwValue = (DWORD)m_iHighScore;
	CIssRegistry::SetKey(szRegLocation, REG_HighScore, dwValue);

	// if we're not currently in a game then don't bother saving any game data
	if(m_eGameState != GS_InPlay)
	{
		CIssRegistry::DeleteValue(szRegLocation, REG_GameState);
		CIssRegistry::DeleteValue(szRegLocation, REG_GameValues);
		return FALSE;
	}

	TypeGameState sState;

	// save all the game state values
	sState.eLastMove			= m_eLastMove;
	sState.eScoreType			= m_eScoreType;
	sState.iDragColumn			= m_iDragColumn;
	sState.iIncrementCounter	= m_iIncrimentCounter;
	sState.iLastAddition		= m_iLastAddition;
	sState.iLastColumn			= m_iLastColumn;
	sState.iLastScore			= m_iLastScore;
	sState.iMoveCardIndex		= (m_oMovingCard?m_oMovingCard->GetIndexInDeck():-1);
	sState.iNextCardIndex		= (m_oNextCard?m_oNextCard->GetIndexInDeck():-1);
	sState.iPrevDeckCardIndex	= (m_oPrevDeckCard?m_oPrevDeckCard->GetIndexInDeck():-1);
	sState.iScore				= m_iScore;
	sState.iTime				= m_iTime;

	for(int i=0; i < CARD_COLUMNS; i++)
		for(int j=0; j<CARDS_PER_COLUMN;j++)
			sState.iCardIndexes[i][j] = (m_oCards[i][j]?m_oCards[i][j]->GetIndexInDeck():-1);

	dwValue = (DWORD)m_eGameState;
	CIssRegistry::SetKey(szRegLocation, REG_GameState, dwValue);

	dwValue = sizeof(TypeGameState);
	CIssRegistry::SetKey(szRegLocation, REG_GameValues, (LPBYTE)&sState, dwValue);

	// also have to save the deck
	m_oDeck.SaveDeck(szRegLocation);

	return TRUE;
}

BOOL CIssTurboSolitaire::LoadGameState(TCHAR* szRegLocation)
{
	if(!szRegLocation)
		return FALSE;

	DWORD dwValue;
	if(!CIssRegistry::GetKey(szRegLocation, REG_HighScore, dwValue))
		m_iHighScore = 0;
	else
		m_iHighScore = (int)dwValue;

	if(!CIssRegistry::GetKey(szRegLocation, REG_GameState, dwValue))
		return FALSE;

	m_eGameState = (EnumGameState)dwValue;
	if(m_eGameState != GS_InPlay)
		return FALSE;

	// read in the deck
	if(!m_oDeck.LoadDeck(szRegLocation))
		return FALSE;

	TypeGameState sState;
	dwValue = sizeof(TypeGameState);
	if(!CIssRegistry::GetKey(szRegLocation, REG_GameValues, (LPBYTE)&sState, dwValue))
		return FALSE;

	m_eLastMove					= sState.eLastMove;			
	m_eScoreType				= sState.eScoreType;			
	m_iDragColumn				= sState.iDragColumn;			
	m_iIncrimentCounter			= sState.iIncrementCounter;	
	m_iLastAddition				= sState.iLastAddition;		
	m_iLastColumn				= sState.iLastColumn;			
	m_iLastScore				= sState.iLastScore;	
	m_oMovingCard				= (sState.iMoveCardIndex == -1?NULL:m_oDeck.GetCardFromIndex(sState.iMoveCardIndex));		
	m_oNextCard					= (sState.iNextCardIndex == -1?NULL:m_oDeck.GetCardFromIndex(sState.iNextCardIndex));
	m_oPrevDeckCard				= (sState.iPrevDeckCardIndex == -1?NULL:m_oDeck.GetCardFromIndex(sState.iPrevDeckCardIndex));
	m_iScore					= sState.iScore;				
	m_iTime						= sState.iTime;	

	for(int i=0; i < CARD_COLUMNS; i++)
		for(int j=0; j<CARDS_PER_COLUMN;j++)
			m_oCards[i][j]	= (sState.iCardIndexes[i][j] == -1?NULL:m_oDeck.GetCardFromIndex(sState.iCardIndexes[i][j]));

	return TRUE;
}


BOOL CIssTurboSolitaire::Init(HWND hWnd)
{

	m_hWnd = hWnd;
	return TRUE;
}


BOOL CIssTurboSolitaire::ClearGame()
{
	for(int i=0; i < CARD_COLUMNS; i++)
	{	
		for(int j=0; j<CARDS_PER_COLUMN;j++)
		{		
			m_oCards[i][j] = NULL;
		}
	}

	m_oMovingCard	= NULL;
	m_oNextCard		= NULL;

	if(m_eGameState != GS_WinGame && m_eScoreType != SCORE_Vegas)
		m_iScore = 0;

	m_iTime = 120;
	m_eGameState = GS_NoGame;

	m_oDeck.Shuffle();

	return TRUE;
}

//New Game
BOOL CIssTurboSolitaire::NewGame()
{
	m_eLastMove			= MOVE_None;
	m_eGameState		= GS_NewGame;
	m_iLastAddition		= 0;
	m_iLastIncriment	= 0;
	m_iIncrimentCounter = 0;

	m_oDeck.Shuffle();

	for(int i=0; i < CARD_COLUMNS; i++)
	{	
		for(int j=0; j<CARDS_PER_COLUMN;j++)
		{		
			m_oCards[i][j] = m_oDeck.GetNextCard();
		}
	}
	m_oNextCard = m_oDeck.GetNextCard();
	
	if(m_eScoreType == SCORE_Vegas)
		m_iScore	-= COST_PER_GAME;
	m_iTime			= 120;
	m_eGameState	= GS_InPlay;
	return TRUE;
}

BOOL CIssTurboSolitaire::Undo()
{
	if(m_eGameState != GS_InPlay)
		return FALSE;

	if(m_eLastMove == MOVE_None)
		return FALSE;	
	if(m_eLastMove == MOVE_Undo)
		return FALSE;
	if(m_eLastMove == MOVE_Deal)
	{
		m_oDeck.UndoNextCard();
		m_oNextCard		= m_oPrevDeckCard;
		m_eLastMove		= MOVE_Undo;
		m_iIncrimentCounter = m_iLastIncriment;
		m_iLastIncriment = 0;
		return TRUE;
	}
	if(m_eLastMove == MOVE_MoveCard)
	{
		//and put the card back in the column
		SetTopCard(m_iLastColumn, m_oNextCard);
		m_oNextCard		= m_oPrevDeckCard;
		m_eLastMove		= MOVE_Undo;
		m_iLastColumn	= -1;//not nessesary but safe
		m_iScore		= m_iLastScore;
		m_iLastScore	= 0;
		m_iIncrimentCounter --;
		m_iLastIncriment = 0;
		return TRUE;
	}


	return FALSE;
}

CIssCard* CIssTurboSolitaire::GetCard(int iColumn, int iRow)
{
	if(iColumn >= CARD_COLUMNS || iColumn < 0)
		return NULL;
	if(iRow >= CARDS_PER_COLUMN || iRow < 0 )
		return NULL;

	return m_oCards[iColumn][iRow];
}

CIssCard* CIssTurboSolitaire::GetTopCard(int iColumn)
{
	int i=GetNumCardsInColumn(iColumn);

	if(i > 0)
		return m_oCards[iColumn][i-1]; //this should work
	else
		return NULL;
}

BOOL CIssTurboSolitaire::OnDragCard(int iColumn)
{
	if(m_oMovingCard != NULL)
	{
		ASSERT(0);
		return FALSE;
	}
	m_oMovingCard = GetTopCard(iColumn);
	RemoveTopCard(iColumn);
	if(m_oMovingCard == NULL)
		return FALSE;
	m_iDragColumn = iColumn;
	return TRUE;
}

BOOL CIssTurboSolitaire::OnCancelDrag()
{
	if(m_oMovingCard == NULL)
		return FALSE;
	if(m_iDragColumn == -1)
		ASSERT(0);//means we lost a card....

	if(SetTopCard(m_iDragColumn, m_oMovingCard) == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}
	m_iDragColumn = -1;
	m_oMovingCard = NULL;
	return TRUE;
}

BOOL CIssTurboSolitaire::SetTopCard(int iColumn, CIssCard* sCard)
{
	if(iColumn >= CARD_COLUMNS || iColumn < 0)
		return FALSE;
	
	for(int i = 0; i < CARDS_PER_COLUMN; i++)
	{
		if(m_oCards[iColumn][i] == NULL)
		{
			m_oCards[iColumn][i] = sCard;
			return TRUE;
		}
	}

	return FALSE;

}

int CIssTurboSolitaire::GetCardsLeftInDeck()
{
	return m_oDeck.GetCardsLeft();
}

int CIssTurboSolitaire::GetNumCardsInColumn(int iColumn)
{
	if(iColumn >= CARD_COLUMNS || iColumn < 0)
		return 0;

	int i=CARDS_PER_COLUMN;
	while(m_oCards[iColumn][i-1] == NULL)
	{
		i--;
		if(i==0)
			return 0;//null
	}
	return i; //m_oCards[iColumn][i-1]; //this should work



}

void CIssTurboSolitaire::RemoveTopCard(int iColumn)
{
	if(iColumn >= CARD_COLUMNS || iColumn < 0)
		return;

	int i=CARDS_PER_COLUMN;
	while(m_oCards[iColumn][i-1] == NULL)
	{
		i--;
		if(i==0)
			return;
	}

	m_oCards[iColumn][i-1] = NULL;
}

void CIssTurboSolitaire::OnDeal()
{
	if(m_eGameState != GS_InPlay)
		return;

	if(m_oDeck.GetCardsLeft() > 0)
	{
		PostMessage(m_hWnd, WM_AniDeal, 0,0);
		m_oPrevDeckCard		= m_oNextCard;
		m_oNextCard			= m_oDeck.GetNextCard();
		m_eLastMove			= MOVE_Deal;
		m_eGameState		= GS_InPlay;
		m_iIncrimentCounter = 0;
		IsWinner();
	}
}

/*
BOOL CIssTurboSolitaire::AniDealStart()
{
	m_oPrevDeckCard = m_oNextCard;
	m_oNextCard = m_oDeck.GetNextCard();
	m_eLastMove = MOVE_Deal;

	return TRUE;
}*/


BOOL CIssTurboSolitaire::OnColumn(int iColumn)
{
	if(iColumn >= CARD_COLUMNS || iColumn < 0 || m_eGameState != GS_InPlay)
		return NULL;
	OnCancelDrag();//just in case
	CIssCard* oCardTemp = GetTopCard(iColumn);

	if(oCardTemp==NULL)
		return FALSE;

	if(CheckCard(oCardTemp))
	{
		m_iLastColumn		= iColumn;
		m_oMovingCard		= oCardTemp;
		RemoveTopCard(iColumn);
		PostMessage(m_hWnd, WM_AniColumn, iColumn, 0);
		m_eGameState		= GS_InPlay;
		return TRUE;
	}
	return FALSE;	
}

BOOL CIssTurboSolitaire::AniColumnCardStart()
{
	return TRUE;
}

BOOL CIssTurboSolitaire::AniColumnCardEnd()
{
	m_oPrevDeckCard = m_oNextCard;
	m_oNextCard		= m_oMovingCard;
	m_eLastMove		= MOVE_MoveCard;
	m_oMovingCard	= NULL;
	m_iLastScore	= m_iScore;
	m_iScore		+= BASE_SCORE + m_iIncrimentCounter*SCORE_INCRIMENT;
	m_iIncrimentCounter++;
	
	IsWinner();
	return TRUE;
}

BOOL CIssTurboSolitaire::OnReleaseOverDeck()
{
	if(m_iDragColumn == -1 || m_oMovingCard == NULL)
		return FALSE;

	if(CheckCard(m_oMovingCard))
	{
		m_oPrevDeckCard = m_oNextCard;
		m_oNextCard		= m_oMovingCard;
		m_eLastMove		= MOVE_MoveCard;
		m_iLastColumn	= m_iDragColumn;
		m_iDragColumn	= -1;
		m_oMovingCard	= NULL;
		return TRUE;
	}
	else
		return OnCancelDrag();



}


BOOL CIssTurboSolitaire::CheckCard(CIssCard* oCard)
{	//fail safes are always good
	if(oCard == NULL)
		return FALSE;

	int iTemp1 = oCard->m_eRank;
	int iTemp2 = m_oNextCard->m_eRank;

	if(iTemp1 == iTemp2 + 1)
		return TRUE;
	if(iTemp2 == iTemp1 + 1)
		return TRUE;
	if(iTemp1 == 1 && iTemp2 == 13)
		return TRUE;
	if(iTemp2 == 1 && iTemp1 == 13)
		return TRUE;

	return FALSE;
}

BOOL CIssTurboSolitaire::CheckForValidMoves()
{	//means we have more cards in the deck
	if(m_oDeck.GetCardsLeft() > 0)
		return TRUE;
	
	for(int i = 0; i < CARD_COLUMNS; i++)
	{
		if(CheckCard(GetTopCard(i)))
			return TRUE;
	}
	m_eGameState		= GS_EndGame;
	return FALSE;

}

TCHAR* CIssTurboSolitaire::GetScore()
{	
	m_oStr->IntToString(m_szScore, m_iScore);
	
	if(m_eScoreType == SCORE_Vegas)
	{	//Format it to look like Currency
		m_oStr->Insert(m_szScore, _T("$"), 0);
		m_oStr->Concatenate(m_szScore, _T(".00"));
	}

	m_oStr->Insert(m_szScore, _T("Score: "));

	return m_szScore;
}

TCHAR* CIssTurboSolitaire::GetHighScore()
{	
	if(m_eScoreType != SCORE_Normal)
		return NULL;

	m_oStr->IntToString(m_szScore, m_iHighScore);
	m_oStr->Insert(m_szScore, _T("High Score: "));

	return m_szScore;
}

TCHAR* CIssTurboSolitaire::GetTime()
{
	if(m_eScoreType == SCORE_Practice)
		return NULL;

	m_oStr->IntToString(m_szTime, m_iTime);
	m_oStr->Insert(m_szTime, _T("Time: "));
	return m_szTime;
}

void CIssTurboSolitaire::IsWinner()
{
	if(m_eGameState != GS_InPlay)
		return;

	if(m_eScoreType == SCORE_Normal)
		if(m_iHighScore < m_iScore)
			m_iHighScore = m_iScore;//save it yo

	BOOL bFoundCard = FALSE;
	for(int i = 0; i < CARD_COLUMNS; i++)
	{
		CIssCard* oCard = GetTopCard(i);
		if(oCard)
		{
			// there are cards up top left
			bFoundCard = TRUE;
			if(CheckCard(oCard))
				return;		// still some moves that are left
		}
	}

	// No more moves left or no more cards
	if(bFoundCard && m_oDeck.GetCardsLeft() == 0)
	{
		m_eGameState = GS_EndGame;
		PostMessage(m_hWnd, WM_AniLoser, 0,0);
		//ClearGame();
	}
	else if(bFoundCard == FALSE)
	{
		m_eGameState = GS_WinGame;
		if(m_eScoreType != SCORE_Practice)//no bonus in practice mode
			m_iScore += m_iTime*TIME_BONUS;
		if(m_eScoreType == SCORE_Vegas)
			m_iScore += COST_PER_GAME;
		PostMessage(m_hWnd, WM_AniWinner, 0,0);
		//ClearGame();
	}
	else
	{
		//we're still in play
		m_eGameState = GS_InPlay;

	}

}

void CIssTurboSolitaire::OnTimer()
{
	if(m_eGameState != GS_InPlay)
		return;

	if(m_eScoreType == SCORE_Practice)
		return;

	if(m_iTime > 0)
		m_iTime--;
	else
	{
		PostMessage(m_hWnd, WM_AniLoser, 0,0);
		m_eGameState = GS_EndGame;
	}
	
}