#include "IssPokerSolitaire.h"

CIssPokerSolitaire::CIssPokerSolitaire(void)
:m_eGameMode(PSOLTYPE_Square)
,m_ePokerType(PTYPE_American)
{
	Reset();
}

CIssPokerSolitaire::~CIssPokerSolitaire(void)
{
}

void CIssPokerSolitaire::Reset()
{
	m_iNumberOfDealtCards	= 0;
	m_iTotalScore			= 0;
	m_iNumberOfScores		= 0;
	m_oNextCard				= NULL;
	for(int i = 0; i < 5; i++)
	{
		for(int j=0; j<5; j++)
			m_oCards[i][j] = NULL;
		m_iLineScores[i]	= 0;
		m_iLineScores[i+5]	= 0;//slightly hackish I know
	}

	//Reset the deck, shuffle, and get the first card "on deck"
	m_oCardSet.Shuffle();
	m_oNextCard = m_oCardSet.GetNextCard();

	switch(m_eGameMode)
	{
		case PSOLTYPE_Shuffle:
			{
				InitPreDealGame();
				CalculateScores();
			}
			break;
		default:
			break;
	}
}

void CIssPokerSolitaire::Init()
{
	m_oCardSet.ChangeNumDecks(1);
	m_oCardSet.Shuffle();
}

void CIssPokerSolitaire::InitPreDealGame()
{
	for(int i=0; i < 5; i++)
	{
		for(int j=0; j<5; j++)
			m_oCards[i][j] = m_oCardSet.GetNextCard(); //Populate the board
	}
	m_oNextCard = NULL;
}

BOOL CIssPokerSolitaire::PlaceCard(int iRow, int iColum)
{
	if(m_iNumberOfDealtCards > 24)
		return FALSE;
	if(m_eGameMode == PSOLTYPE_Shuffle)
		return FALSE;
	if(m_oCards[iRow][iColum]!=NULL)
		return FALSE;
	if(iRow > 4 || iRow < 0 || iColum > 4 || iColum < 0)
		return FALSE;

	//Set the Card and populate the next card
	if(m_eGameMode==PSOLTYPE_Comumns)
	{	
		int iHeight =4;
		while(m_oCards[iRow][iHeight] != NULL &&
			iHeight > -1)
			iHeight--;
		if(iHeight == -1)
			return FALSE;
		else
			m_oCards[iRow][iHeight] = m_oNextCard;
	}
	else
	{
		m_oCards[iRow][iColum] = m_oNextCard;
	}
	m_oNextCard = m_oCardSet.GetNextCard();
	m_iNumberOfDealtCards++;
	CalculateScores();
	return TRUE;
}

BOOL CIssPokerSolitaire::SwitchCards(int iRow1, int iColum1, int iRow2, int iColum2)
{
	if(m_eGameMode != PSOLTYPE_Shuffle)
		return FALSE;
	if(iRow1 > 4 || iRow1 < 0 || iColum1 > 4 || iColum1 < 0)
		return FALSE;
	if(iRow2 > 4 || iRow2 < 0 || iColum2 > 4 || iColum2 < 0)
		return FALSE;
	if(m_oCards[iRow1][iColum1] == NULL ||
		m_oCards[iRow2][iColum2] == NULL)
		return FALSE;

	//The Big switch...ah yeah
	CIssCard* oCardTemp;
	oCardTemp					= m_oCards[iRow1][iColum1];
	m_oCards[iRow1][iColum1]	= m_oCards[iRow2][iColum2];
	m_oCards[iRow2][iColum2]	= oCardTemp;	

	CalculateScores();
	return TRUE;
}


CIssCard* CIssPokerSolitaire::GetCard(int iRow, int iColum)
{	//Keep it in bounds
	if(iRow < 0 || iRow > 4 || iColum < 0 || iColum > 4)
		return NULL;
	
	return m_oCards[iRow][iColum];
}

void CIssPokerSolitaire::SetGameMode(EnumPokerSolGameType eGameMode)
{
	m_eGameMode = eGameMode;
}

void CIssPokerSolitaire::CalculateScores()
{
	for(int i=0; i<5; i++)
	{	// calculate rows
		if( m_oCards[0][i] != NULL &&
			m_oCards[1][i] != NULL &&
			m_oCards[2][i] != NULL &&
			m_oCards[3][i] != NULL &&
			m_oCards[4][i] != NULL)
		{	//we have all of the cards placed
			for(int j = 0; j < 5; j++)
			{
				m_oHandEval.SetCard(m_oCards[j][i], j);
			}
			m_iLineScores[i] = m_oScoreCalc.GetPayoutRatio(m_ePokerType, m_oHandEval.EvaluateHand(m_ePokerType));
		}
	
		// calculate colum scores
		if( m_oCards[i][0] != NULL &&
			m_oCards[i][1] != NULL &&
			m_oCards[i][2] != NULL &&
			m_oCards[i][3] != NULL &&
			m_oCards[i][4] != NULL)
		{	//we have all of the cards placed
			for(int j = 0; j < 5; j++)
			{
				m_oHandEval.SetCard(m_oCards[i][j], j);
			}
			m_iLineScores[i+5] = m_oScoreCalc.GetPayoutRatio(m_ePokerType, m_oHandEval.EvaluateHand(m_ePokerType));
		}
	}
	//and finally , the totals
	m_iTotalScore = 0;
	for(int i = 0; i < 10; i++)
	{
		m_iTotalScore += m_iLineScores[i];

	}
}

int CIssPokerSolitaire::GetScores(int iIndex)
{
	if(iIndex < 0 || iIndex > 10) //11th is the total
		return 0;

	if(iIndex == 10)
		return m_iTotalScore;

	return m_iLineScores[iIndex];
}