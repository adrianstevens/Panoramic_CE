// IssBJHand.h: interface for the CIssBJHand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISSBJHAND_H__B19FA889_51B6_4A5F_A21B_5AAAB4374F29__INCLUDED_)
#define AFX_ISSBJHAND_H__B19FA889_51B6_4A5F_A21B_5AAAB4374F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IssCard.h"
#include "IssVector.h"

#define MAX_CARDS_IN_HAND 11 //absolute max

class CIssBJHand  
{
public:
	CIssBJHand();
	virtual ~CIssBJHand();

	int			GetNumberOfCards();
	CIssCard*	GetCard(int iCardIndex);
	BOOL		GetScore(int &iLowScore, int &iHighScore);
	void		AddCard(CIssCard *oCard);
	void		RemoveCard(int iCardIndex);
	void		ClearHand();

	

private:	// variables
	CIssVector <CIssCard>	m_oCards;


};

#endif // !defined(AFX_ISSBJHAND_H__B19FA889_51B6_4A5F_A21B_5AAAB4374F29__INCLUDED_)
