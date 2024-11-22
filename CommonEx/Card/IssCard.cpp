// IssCard.cpp: implementation of the CIssCard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IssCard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssCard::CIssCard():
 m_eSuit(SUIT_Spade)
,m_eRank(RANK_Ace)
,m_iIndexInDeck(-1)
{


}

CIssCard::~CIssCard()
{

}

int CIssCard::GetCardValue()
{
	switch(m_eRank)
	{
	case RANK_Joker:
	case RANK_Ace:
	case RANK_Two:
	case RANK_Three:
	case RANK_Four:
	case RANK_Five:
	case RANK_Six:
	case RANK_Seven:
	case RANK_Eight:
	case RANK_Nine:
	case RANK_Ten:
		return (int)m_eRank;
	case RANK_Jack:
	case RANK_Queen:
	case RANK_King:
		return 10;
	}

	return 0;
}
