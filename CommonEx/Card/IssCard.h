// IssCard.h: interface for the CIssCard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISSCARD_H__339B1C3C_3CF8_4CDF_8FB5_EBFAF26CF5A8__INCLUDED_)
#define AFX_ISSCARD_H__339B1C3C_3CF8_4CDF_8FB5_EBFAF26CF5A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		NUM_SUITS	4
#define		NUM_RANKS	13

enum EnumCardSuit
{
	SUIT_Diamond,
	SUIT_Club,
	SUIT_Heart,
	SUIT_Spade,
};

enum EnumCardRank
{
	RANK_Joker,
	RANK_Ace,
	RANK_Two,
	RANK_Three,
	RANK_Four,
	RANK_Five,
	RANK_Six,
	RANK_Seven,
	RANK_Eight,
	RANK_Nine,
	RANK_Ten,
	RANK_Jack,
	RANK_Queen,
	RANK_King,
};

class CIssCard  
{
public:
	CIssCard();
	virtual ~CIssCard();

	int				GetCardValue();
	int				GetCardRank(){return (int)m_eRank;};
	int				GetCardSuit(){return (int)m_eSuit;};
	int				GetIndexInDeck(){return m_iIndexInDeck;};
	void			SetIndexInDeck(int iIndex){m_iIndexInDeck = iIndex;};


public:
	EnumCardSuit			m_eSuit;
	EnumCardRank			m_eRank;
	int						m_iIndexInDeck;			// so it knows where it's located in the deck

};

#endif // !defined(AFX_ISSCARD_H__339B1C3C_3CF8_4CDF_8FB5_EBFAF26CF5A8__INCLUDED_)
