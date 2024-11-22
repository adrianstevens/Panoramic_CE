// IssBJHand.cpp: implementation of the CIssBJHand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IssBJHand.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssBJHand::CIssBJHand()
{

}

CIssBJHand::~CIssBJHand()
{
	/*for(int i=0; i<m_oCards.GetSize(); i++)
	{
		CIssCard* pCard = m_oCards[i];
		if(pCard)
			delete pCard;
		m_oCards.SetElementAt(NULL, i);
	}*/
}


int CIssBJHand::GetNumberOfCards()
{
	return m_oCards.GetSize();
}

CIssCard* CIssBJHand::GetCard(int iCardIndex)
{
	return m_oCards.ElementAt(iCardIndex);
}

BOOL CIssBJHand::GetScore(int &iLowScore, int &iHighScore)
{
	int		iTemp	= 0;
	BOOL	bAce	= FALSE;

	iLowScore = 0;
	iHighScore = 0;

	// calculate low score
	for (int i = 0; i < m_oCards.GetSize(); i++)
	{
		iTemp = (int)m_oCards.ElementAt(i)->m_eRank;
		if (iTemp == 1)
			bAce = TRUE;
		if (iTemp > 10)
			iTemp = 10;
		iLowScore += iTemp;
	}
	if (bAce == TRUE &&
		(iLowScore + 10) < 22)
	{
		iHighScore = iLowScore + 10;
		return TRUE;
	}
	else
	{
		iHighScore = iLowScore;
		return FALSE;
	}

}

void CIssBJHand::AddCard(CIssCard *oCard)
{
	m_oCards.AddElement(oCard);
}

void CIssBJHand::RemoveCard(int iCardIndex)
{
	m_oCards.RemoveElementAt(iCardIndex);
}

void CIssBJHand::ClearHand()
{
	m_oCards.RemoveAll();
}


