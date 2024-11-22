#pragma once

#include "IssCard.h"

#define MAX_NUM_CARDS		7
#define DEFAULT_NUM_CARDS	5

class CIssVPHand
{
public:
	CIssVPHand(void);
	~CIssVPHand(void);

	CIssCard*		GetCard(int iCardIndex);
	BOOL			AddCard(CIssCard *oCard);
    BOOL			AddCard(CIssCard *oCard, int iIndex);
    BOOL            IsCardSet(int iIndex);
	BOOL			RemoveCard(int iCardIndex);
	void			ClearHand();
	void			SetNumberOfCards(int iNumCards);
	int				GetNumberOfCards();

private:
	CIssCard*		m_oCards[MAX_NUM_CARDS];
	int				m_iNumCards;
};
