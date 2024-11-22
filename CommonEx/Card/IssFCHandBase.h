#pragma once

#include "IssCard.h"

#define MAX_CARDS 17

class CIssFCHandBase
{
public:
	CIssFCHandBase(void);
public:
	~CIssFCHandBase(void);


public:
	BOOL				ClearHand();
	virtual	BOOL		AddCard();
	CIssCard*			RemoveTopCard();		//clears the top card
	CIssCard*			GetTopCard();			//gets a pointer to the top card

private:


private:
	CIssCard*			m_oCards[MAX_CARDS];

	int					m_iNumberOfCards;


};
