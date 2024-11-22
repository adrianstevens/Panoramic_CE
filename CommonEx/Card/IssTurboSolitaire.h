#pragma once

#include "stdafx.h"
#include "IssCardSet.h"
#include "IssString.h"

#define CARD_COLUMNS		7
#define CARDS_PER_COLUMN	5
#define COST_PER_GAME		5000 //for Vegas Scoring

#define	BASE_SCORE			100
#define SCORE_INCRIMENT		20
#define TIME_BONUS			50 //per second

#define WM_AniColumn		WM_USER + 200
#define WM_AniDeal			WM_USER + 201
#define WM_AniWinner		WM_USER + 202
#define WM_AniLoser			WM_USER + 203

enum EnumScoringType
{
	SCORE_Practice,
	SCORE_Normal,
	SCORE_Vegas,
};

enum EnumLastMove
{
	MOVE_None,
	MOVE_Deal,
	MOVE_MoveCard,
	MOVE_Undo,
};

enum EnumGameState
{
	GS_NoGame,
	GS_EndGame,
	GS_WinGame,
	GS_NewGame,
	GS_InPlay,
};

class CIssTurboSolitaire
{
public:
	CIssTurboSolitaire(void);
	~CIssTurboSolitaire(void);

	EnumGameState	GetGameState(){return m_eGameState;};
	EnumScoringType	GetGameMode(){return m_eScoreType;};
	void		SetGameMode(EnumScoringType eScoringType){m_eScoreType = eScoringType; m_iScore = 0;};
	BOOL		Init(HWND hWnd);
	BOOL		NewGame();
	BOOL		ClearGame();
	void		OnDeal();
	BOOL		OnDragCard(int iColumn);//so it moves the card pointers appropriately
	BOOL		OnCancelDrag();	//released outside of the deck...so canceled
	BOOL		OnReleaseOverDeck();//release the dragged card on the deck
	BOOL		Undo();
	BOOL		OnColumn(int iColumn);
	TCHAR*		GetScore();
	TCHAR*		GetHighScore();
	TCHAR*		GetTime();
	void		OnTimer();//call every second
	void		ResetHighScore(){m_iHighScore = 0;};

	BOOL		SaveGameState(TCHAR* szRegLocation);
	BOOL		LoadGameState(TCHAR* szRegLocation);

	BOOL		AniColumnCardStart();
	BOOL		AniColumnCardEnd();
//	BOOL		AniDealStart();
//	BOOL		AniDealEnd();


	CIssCard*   GetDeckCard(){return m_oNextCard;};
	CIssCard*	GetCard(int iColumn, int iRow);
	CIssCard*	GetTopCard(int iColumn);
	CIssCard*	GetMovingCard(){return m_oMovingCard;};
	int			GetNumCardsInColumn(int iColumn);
	int			GetCardsLeftInDeck();	

private:
	BOOL		SetTopCard(int iColumn, CIssCard* sCard);
	BOOL		CheckCard(CIssCard* oCard);
	void		RemoveTopCard(int iColumn);
	BOOL		CheckForValidMoves();
	void		IsWinner();
	

private:
	HWND		m_hWnd;
	CIssString*	m_oStr;
	CIssCardSet m_oDeck;
	CIssCard*	m_oCards[CARD_COLUMNS][CARDS_PER_COLUMN];
	CIssCard*	m_oNextCard;	
	CIssCard*	m_oMovingCard;

	TCHAR		m_szScore[STRING_NORMAL];
	TCHAR		m_szHighScore[STRING_NORMAL];
	TCHAR		m_szTime[STRING_NORMAL];

	EnumLastMove m_eLastMove;
	EnumGameState m_eGameState;
	EnumScoringType m_eScoreType;

	int			m_iLastColumn;
	int			m_iLastScore;
	int			m_iDragColumn;
	int			m_iHighScore;//only for standard
	CIssCard*	m_oPrevDeckCard;

	int			m_iScore;  //for the score or the dollar amount
	int			m_iLastAddition;
	int			m_iIncrimentCounter;
	int			m_iLastIncriment;
	int			m_iTime;
};
