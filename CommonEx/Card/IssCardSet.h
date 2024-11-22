// IssCardSet.h: interface for the CIssCardSet class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "IssCard.h"

#define MAX_DECKS		8
#define NUM_SHUFFLES	127	// card swaps per deck

class CIssCardSet  
{
public:
	CIssCardSet(BOOL bUseJokers = FALSE);
	virtual ~CIssCardSet();
	
	BOOL			SaveDeck(TCHAR* szRegLocation);
	BOOL			LoadDeck(TCHAR* szRegLocation);
	void			Shuffle();
	void			ChangeNumDecks(int iNumDecks);
	int				GetNumDecks(){return m_iNumDecks;};
	CIssCard*		GetNextCard(); //BOOL bNoShuffle = FALSE);
	CIssCard*		GetCardFromIndex(int iIndex);
	void			UndoNextCard();
	int				GetCardsLeft(){return m_iNumCards - m_iCardIndex;};
    BOOL            MoveToEndOfDeck(EnumCardRank eRank, EnumCardSuit eSuit);

private:

	void			Reset();
	void			InitializeDeck();

private:
	BOOL			m_bUseJokers;
	CIssCard		*m_oCards;
	int				m_iNumCards;
	int				m_iNumDecks;
	int				m_iCardIndex;

    int             m_iSwapIndex; //a little dirty but it works
	
	

};
