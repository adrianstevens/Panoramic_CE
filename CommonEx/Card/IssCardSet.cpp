// IssCardSet.cpp: implementation of the CIssCardSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IssCardSet.h"
#include "IssRegistry.h"

#define REG_CardSetState	_T("CardSetState")
#define REG_Deck			_T("Deck")

struct TypeCardSetState
{
	BOOL	bUseJokers;
	int		iNumCards;
	int		iNumDecks;
	int		iCardIndex;
};

struct TypeCards
{
	BYTE	btSuit;
	BYTE	btRank;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssCardSet::CIssCardSet(BOOL	bUseJokers)
{
	m_bUseJokers		= bUseJokers;
	m_oCards			= NULL;

	Reset();
	ChangeNumDecks(1);
}

CIssCardSet::~CIssCardSet()
{
	Reset();
}

BOOL CIssCardSet::SaveDeck(TCHAR* szRegLocation)
{
	if(!m_oCards || m_iNumCards == 0 || !szRegLocation)
		return FALSE;

	DWORD dwSize = sizeof(TypeCards)*m_iNumCards;
	TypeCards* sCards = new TypeCards[m_iNumCards];
	for(int i=0; i<m_iNumCards; i++)
	{
		sCards[i].btSuit	= (BYTE)m_oCards[i].m_eSuit;
		sCards[i].btRank	= (BYTE)m_oCards[i].m_eRank;
	}

	// save the cards
	SetKey(szRegLocation, REG_Deck, (LPBYTE)sCards, dwSize);	
	delete [] sCards;

	TypeCardSetState sState;
	sState.bUseJokers		= m_bUseJokers;
	sState.iCardIndex		= m_iCardIndex;
	sState.iNumCards		= m_iNumCards;
	sState.iNumDecks		= m_iNumDecks;
	dwSize					= sizeof(TypeCardSetState);
	// save the deck state
	SetKey(szRegLocation, REG_CardSetState, (LPBYTE)&sState, dwSize);	
	return TRUE;
}

BOOL CIssCardSet::LoadDeck(TCHAR* szRegLocation)
{
	Reset();

	if(!szRegLocation)
		return FALSE;

	TypeCardSetState sState;
	DWORD dwSize = sizeof(TypeCardSetState);

	if(S_OK == GetKey(szRegLocation, REG_CardSetState, (LPBYTE)&sState, dwSize))
		return FALSE;

	// copy the values over
	m_bUseJokers	= sState.bUseJokers;
	m_iCardIndex	= sState.iCardIndex;
	m_iNumCards		= sState.iNumCards;
	m_iNumDecks		= sState.iNumDecks;

	if(m_iNumCards == 0)
		return FALSE;

	TypeCards* sCards = new TypeCards[m_iNumCards];
	ZeroMemory(sCards, sizeof(TypeCards)*m_iNumCards);
	dwSize = sizeof(TypeCards)*m_iNumCards;
	if(S_OK == GetKey(szRegLocation, REG_Deck, (LPBYTE)sCards, dwSize))
	{
		Reset();
		delete [] sCards;
		return FALSE;
	}

	m_oCards = new CIssCard[m_iNumCards];
	for(int i=0;i<m_iNumCards; i++)
	{
		m_oCards[i].m_eRank		= (EnumCardRank)sCards[i].btRank;
		m_oCards[i].m_eSuit		= (EnumCardSuit)sCards[i].btSuit;
		m_oCards[i].SetIndexInDeck(i);
	}

	delete [] sCards;

	return TRUE;
}



void CIssCardSet::Reset()
{
	if(m_oCards != NULL)
	{
		delete [] m_oCards;
		m_oCards = NULL;
	}

	m_iCardIndex		= 0;
	m_iNumCards			= -1;
	m_iNumDecks			= 0;

}

void CIssCardSet::ChangeNumDecks(int iNumDecks)
{
	if(	iNumDecks < 1  ||
		iNumDecks > MAX_DECKS || 
		iNumDecks == m_iNumDecks)
	{
		return;
	}

	Reset();

	m_iNumDecks = iNumDecks;
	//set the number of cards
	m_iNumCards = iNumDecks * 52;
	
	if(m_bUseJokers)
		m_iNumCards = iNumDecks * 54;

	m_oCards = new CIssCard[m_iNumCards];
	
	// make sure to set where in the deck these cards are
	for(int i=0; i<m_iNumCards; i++)
		m_oCards[i].SetIndexInDeck(i);

	//Always Initialize the deck(s) 
	InitializeDeck();
	Shuffle();
}

void CIssCardSet::Shuffle()
{
	// pick two random cards and swap them, repeat
	if (m_oCards == NULL)
		return;

    m_iSwapIndex     = 0;
	m_iCardIndex	 = 0; //KINDA need this....

#ifdef DEBUG
	// Used for Debugging purposes line up cards with smallest number first
/*	for(int i=0; i<m_iNumCards; i++)
	{
		m_oCards[i].m_eRank = RANK_Three;
		m_oCards[i].m_eSuit = SUIT_Club;
	}

	return;*/
#endif

	//Seed the randomness with the seed of randomness
	/*SYSTEMTIME time;
	GetLocalTime(&time);
    srand(time.wSecond);*/
    DWORD dwTick = GetTickCount();
    srand(dwTick);
	

	CIssCard oTempCard;
	
	int			iNumSwaps = m_iNumDecks * NUM_SHUFFLES;
	int			iSwap1, iSwap2;
	
	for (int i = 0; i < iNumSwaps; i++)
	{
		iSwap1 = rand()%m_iNumCards;
		iSwap2 = rand()%m_iNumCards;

		//simple swap
		oTempCard.m_eRank			= m_oCards[iSwap1].m_eRank;
		oTempCard.m_eSuit			= m_oCards[iSwap1].m_eSuit;

		m_oCards[iSwap1].m_eRank	= m_oCards[iSwap2].m_eRank;
		m_oCards[iSwap1].m_eSuit	= m_oCards[iSwap2].m_eSuit;

		m_oCards[iSwap2].m_eRank	= oTempCard.m_eRank;
		m_oCards[iSwap2].m_eSuit	= oTempCard.m_eSuit;
	}
}

void CIssCardSet::InitializeDeck()
{
	int iCardsInDeck = 52;
	m_iCardIndex	 = 0;
    m_iSwapIndex     = 0;

	if (m_bUseJokers == TRUE)
	{
		iCardsInDeck = 54;
	}
	// go through number of decks
	for (int i = 0; i < m_iNumDecks; i++)
	{
		// go through suits
		for (int j = 0; j < NUM_SUITS; j++)
		{
			// go through card ranks
			for (int k = 0; k < NUM_RANKS; k++)
			{
				m_oCards[k + j*NUM_RANKS + i*iCardsInDeck].m_eSuit = (EnumCardSuit)j;
				m_oCards[k + j*NUM_RANKS + i*iCardsInDeck].m_eRank = (EnumCardRank)(k+1);	// correct for joker in enum
			}
		}
		// add jokers at the end of each additional deck
		if (m_bUseJokers == TRUE)
		{
			m_oCards[(i+1)*iCardsInDeck - 2].m_eRank = RANK_Joker;
			m_oCards[(i+1)*iCardsInDeck - 1].m_eRank = RANK_Joker;
		}
	}
}

CIssCard* CIssCardSet::GetCardFromIndex(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_iNumCards)
		return NULL;

	return &m_oCards[iIndex];
}

CIssCard* CIssCardSet::GetNextCard() //(BOOL bNoShuffle /* = FALSE */)
{

	/*
#define LIMIT_OF_CARDS 20
	if(m_iCardIndex + LIMIT_OF_CARDS >= m_iNumCards && bNoShuffle)
	{	//we're out of cards 
		//re-init
		Shuffle();
		m_iCardIndex = 0;
	}*/

	if(m_iCardIndex >= m_iNumCards + 1)
		return NULL;

	m_iCardIndex++;
	return &m_oCards[m_iCardIndex-1];

}

void CIssCardSet::UndoNextCard()
{
	if(m_iCardIndex > 0)
		m_iCardIndex--;
}

BOOL CIssCardSet::MoveToEndOfDeck(EnumCardRank eRank, EnumCardSuit eSuit)
{
    int iTemp = 0;

    for(int i = 0; i < m_iNumCards; i++)
    {
        if(m_oCards[i].m_eRank == eRank &&
            m_oCards[i].m_eSuit == eSuit)
        {
            iTemp = m_iNumCards - m_iSwapIndex -1;
            m_oCards[i].m_eRank = m_oCards[iTemp].m_eRank;
            m_oCards[i].m_eSuit = m_oCards[iTemp].m_eSuit;
            m_iSwapIndex++;
            m_oCards[iTemp].m_eRank = eRank;
            m_oCards[iTemp].m_eSuit = eSuit;
            break;
        }
    }

    if(m_iSwapIndex > m_iNumCards/2)
        m_iSwapIndex = 0;//just to be safe


    return TRUE;
}