#pragma once

#include "stdafx.h"
#include "IssCardSet.h"
#include "IssPokerHandEval.h"
#include "IssPokerScoreCalc.h"

enum EnumPokerSolGameType
{
	PSOLTYPE_Comumns,
	PSOLTYPE_Shuffle,
	PSOLTYPE_Square,
};

class CIssPokerSolitaire
{
public:
	CIssPokerSolitaire(void);
	~CIssPokerSolitaire(void);

	void			Reset();
	void			Init();
	CIssCard*		GetCard(int iRow, int iColum); //not inline to make it safe
	CIssCard*		GetNextCard(){return m_oNextCard;};	
	BOOL			SetCard(int iRow, int iColum);
	void			SetGameMode(EnumPokerSolGameType eGameMode);
	EnumPokerSolGameType GetGameMode(){return m_eGameMode;};
	void			SetScoringMode(EnumPokerType ePokerType){m_ePokerType = ePokerType;};
	EnumPokerType	GetScoringMode(){return m_ePokerType;};
	BOOL			PlaceCard(int iRow, int iColum);
	BOOL			SwitchCards(int iRow1, int iColum1, int iRow2, int iColum2);
	int				GetScores(int iIndex);
	int				GetNumberOfDealtCards(){return m_iNumberOfDealtCards;};

	int				GetNumberOfScores(){return m_iNumberOfScores;};
	void			SetNumberOfScores(int iScores){m_iNumberOfScores = iScores;};

private:
	void			InitPreDealGame();
	void			CalculateScores();



private:
	CIssPokerScoreCalc			m_oScoreCalc;
	CIssPokerHandEval			m_oHandEval;
	CIssCardSet					m_oCardSet;
	CIssCard*					m_oCards[5][5];  //we'll skip creating a "hand" class for this game...so 5 5 card hands
	CIssCard*					m_oNextCard;	 //we'll grab and store the ondeck card since the deck class isn't really designed that wasy

	EnumPokerSolGameType		m_eGameMode;
	EnumPokerType				m_ePokerType;
	int							m_iLineScores[10];//current score per line
	int							m_iNumberOfDealtCards;//for the first two game modes
	int							m_iTotalScore;
	int							m_iNumberOfScores;		// a counter so we can play a sound


	
};
