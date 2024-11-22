#include "StdAfx.h"
#include "IssFCHandBase.h"

CIssFCHandBase::CIssFCHandBase(void):
 m_iNumberOfCards(0)
{
	ClearHand();
}

CIssFCHandBase::~CIssFCHandBase(void)
{
}


BOOL CIssFCHandBase::ClearHand()
{
	for(int i = 0; i < MAX_CARDS; i++)
		m_oCards[i] = NULL;

	return TRUE;
}

BOOL CIssFCHandBase::AddCard()
{

	return TRUE;
}

CIssCard* CIssFCHandBase::GetTopCard()
{
	if(m_iNumberOfCards == 0 ||
		m_iNumberOfCards > MAX_CARDS)
		return NULL;


	return m_oCards[m_iNumberOfCards-1]; // minus one because the array is zero based but we count starting at 1
}

CIssCard* CIssFCHandBase::RemoveTopCard()
{
	CIssCard* oCardTemp = GetTopCard();

	if(oCardTemp != NULL)
	{
		m_iNumberOfCards--;
		m_oCards[m_iNumberOfCards-1]=NULL;
	}
	return oCardTemp;
}


