#include "StdAfx.h"
#include "IssVPHand.h"

CIssVPHand::CIssVPHand(void)
{
	ClearHand();
	m_iNumCards = DEFAULT_NUM_CARDS;//default
}

CIssVPHand::~CIssVPHand(void)
{	
	ClearHand();
}

CIssCard* CIssVPHand::GetCard(int iCardIndex)
{
	if(iCardIndex < 0 ||
		iCardIndex >= m_iNumCards)
		return NULL;
	
	return m_oCards[iCardIndex];
}

BOOL CIssVPHand::AddCard(CIssCard *oCard, int iIndex)
{
    if(oCard == NULL)
        return FALSE;
    if(iIndex < 0 || iIndex >= MAX_NUM_CARDS)
        return FALSE;
    if(m_oCards[iIndex] != NULL)
        return FALSE;
    m_oCards[iIndex] = oCard;
    return TRUE;
}

BOOL CIssVPHand::IsCardSet(int iIndex)
{
    if(iIndex < 0 || iIndex >= MAX_NUM_CARDS)
        return FALSE;
    if(m_oCards[iIndex] == NULL)
        return FALSE;
    return TRUE;
}

BOOL CIssVPHand::AddCard(CIssCard *oCard)
{
	int i = 0;
	while(i < m_iNumCards)
	{
		if(m_oCards[i] == NULL)
		{
			m_oCards[i] = oCard;
			return TRUE;
			break;
		}
		i++;
	}
	return FALSE;
}

BOOL CIssVPHand::RemoveCard(int iCardIndex)
{
	if(iCardIndex < 0 ||
		iCardIndex >= m_iNumCards)
		return FALSE;
	m_oCards[iCardIndex] = NULL;

	return TRUE;
}

void CIssVPHand::ClearHand()
{
	for(int i = 0; i < MAX_NUM_CARDS; i++)
		m_oCards[i] = NULL;
}

void CIssVPHand::SetNumberOfCards(int iNumCards)
{
	if(iNumCards < 1 || iNumCards > MAX_NUM_CARDS)
		return;

	ClearHand();
	m_iNumCards = iNumCards;
}

int CIssVPHand::GetNumberOfCards()
{
	int iRet = 0;
	for(int i = 0; i < m_iNumCards; i++)
	{
		if(m_oCards[i] != NULL)
			iRet++;
		else
			break;
	}

	return iRet;
}