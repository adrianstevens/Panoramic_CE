#pragma once

enum EnumGameState
{
	GSTATE_InPlay,
	GSTATE_Win,
	GSTATE_Lose, //No Moves
};

enum EnumBackgroundColor
{
	BGCOLOR_Blue,
	BGCOLOR_Green,
	BGCOLOR_Purple,
	BGCOLOR_Red,
    BGCOLOR_Orange,
    BGCOLOR_Count,
};

#include "stdafx.h"

#define DOUBLECLICK_TIMEOUT		800
#define REG_KEY					_T("SOFTWARE\\iSS\\FreeCell")

#define SEED_MAX				2000000 // Two Billion works ... but lets use 2,000,000

#define CARD_COLUMNS			8
#define MAX_CARDS_PER_COLUMN	20
#define NULL_CARD				255
#define CARD_BLANK				53

#define CARD_BLANK_SPADE        54 
#define CARD_BLANK_DIAMOND      55
#define CARD_BLANK_CLUB         56
#define CARD_BLANK_HEART        57





#define WM_INVALID_MOVE		WM_USER+500
#define WM_GAME_WIN			WM_USER+501
#define WM_GAME_LOSE		WM_USER+502 // no moves left
#define WM_PROCESS_MOVE		WM_USER+503 // to pump game animation/update
#define WM_SHOW_UNDO		WM_USER+504 //to show/hide the undo button
#define WM_ANI_COLUMN_ACE   WM_USER+505	//if we auto move a card to the ace rack wParam - Column, lParam - Acerack
#define WM_ANI_FREECELL_ACE WM_USER+506	//if we auto move a card to the ace rack wParam - Column, lParam - Acerack

struct TypeFreeCellUndo
{
	BOOL		bCanUndo;
	byte		btColumns[CARD_COLUMNS][MAX_CARDS_PER_COLUMN];//array to store cards 
	byte		btFreeCells[4];
	byte		btAceRacks[4];
	void		ClearUndo(){bCanUndo = FALSE;};//really all we need
	void		SetUndo(){bCanUndo = TRUE;};//allow the undo
};

enum EnumCardLocations
{
	CL_Column1,
	CL_Column2,
	CL_Column3,
	CL_Column4,
	CL_Column5,
	CL_Column6,
	CL_Column7,
	CL_Column8,
	CL_FreeCell1,
	CL_FreeCell2,
	CL_FreeCell3,
	CL_FreeCell4,
	CL_AceRack1,
	CL_AceRack2,
	CL_AceRack3,
	CL_AceRack4,
	CL_None,
};

enum EnumAutoPlay
{
	AUTOPLAY_Off,
	AUTOPLAY_Safe,
	AUTOPLAY_On,
    AUTOPLAY_Count,
};


class CIssFreeCell
{
public:
	CIssFreeCell(void);
	~CIssFreeCell(void);

public:
	void		Init(HWND hWnd){m_hWnd = hWnd;};
	BOOL		ReloadGame();
	void		NewGame(BOOL bSelNewGame = TRUE);
	BOOL		SelectGame(int iGame);
	BOOL		IsGameInPlay(){return m_bGameInPlay;};
	
	int			GetCardsRemaining(){return (int)m_btCardsRemaining;};
	int			GetGameNumber(){return m_iGameNumber;};
	int			GetSelectedCard(){return (int)m_btSelected;};

	BOOL		OnAceRack(int iAce);
	BOOL		OnFreeCell(int iFreeCell);
	BOOL		OnColumn(int iColumn);
	BOOL		OnTable();//ie ... empty space

	BOOL		OnUndo();
	BOOL		CanUndo(){return m_sUndo.bCanUndo;};

	void		AutoPlay(); //auto play once for people that aren't playing with autoplay


	BOOL		ShowAnimations(){return m_bShowAnimations;};
	void		SetShowAnimations(BOOL bShow){m_bShowAnimations = bShow;};
	EnumAutoPlay		GetAutoPlay(){return m_eAutoPlay;};
	BOOL		GetPlaySounds(){return m_bPlaySounds;};
	EnumBackgroundColor GetBackgroundColor(){return m_eTableColor;};
	void		SetBackgroundColor(EnumBackgroundColor eColor){m_eTableColor = eColor;};
	void		SetAutoPlay(EnumAutoPlay eAutoPlay){m_eAutoPlay = eAutoPlay;};
	void		SetPlaySounds(BOOL bPlaySounds){m_bPlaySounds = bPlaySounds;};
	void		SetTableColor(EnumBackgroundColor eTableColor){m_eTableColor = eTableColor;};

	void		CheckGameState(); //auto move aces and 2s, check for win/lose

	BOOL		IsSolvable(){return m_bIsSolvable;};

	int			GetCurrentStreak(){return m_iCurrentStreak;};
	int			GetLongestStreak(){return m_iLongestStreak;};
	int			GetGamesWon(){return m_iGamesWon;};
	int			GetGamesLost(){return m_iGamesLost;};
	int			GetWinPcnt(){if(m_iGamesLost == 0 && m_iGamesWon == 0) return 0; else if(m_iGamesLost < 1) return 100; return m_iGamesWon*100/(m_iGamesLost + m_iGamesWon);};

	void		ClearStats(){m_iGamesLost = 0; m_iGamesWon = 0; m_iCurrentStreak = 0; m_iLongestStreak = 0;};//otherwise one could cheat!

	int			GetNumCardsInColumn(byte btColumn);

    BOOL        IsCardLocked(byte btColumn, byte btIndex);

    BOOL        MoveCard(byte btFrom, byte btTo, byte btDepth);

private:
	void		LoadRegistry();
	void		SaveRegistry();

	BOOL		IsCardBlack(byte btCard);
	
	void		DealUnsolvable();

	BOOL		ValidateAceRack(byte btCol, byte btCard);
	byte		GetCardNum(byte btCard); // Return the number of the card
    byte        GetCardSuit(byte btCard);
	void		MoveError();//so we know to pop the message box
	void		ValidMove(byte btFrom, byte btTo);//in case we need to do any processing after valid moves
	void		CalcCardsRemaining();//rather than subtracting in code .. this is just easier
	
	int			GetNumEmptyFreeCells();

	BOOL		MoveCardFromColumntoColumn(byte btFrom, byte btTo, BOOL bMakeMove = FALSE);
	BOOL		MoveStackFromColumntoColumn(byte btFrom, byte btTo, BOOL bMakeMove = FALSE);//we'll assume the other has already been called
	BOOL		MoveCardFromColumntoAceRack(byte btFrom, byte btAce, BOOL bMakeMove = FALSE, BOOL bIsAutoMove = FALSE);
	BOOL		MoveCardFromColumntoFreeCell(byte btColumn, byte btFreeCell, BOOL bMakeMove = FALSE);
	BOOL		MoveCardFromFreeCelltoAceRack(byte btFreeCell, byte btAce, BOOL bMakeMove = FALSE, BOOL bIsAutoMove = FALSE);
	BOOL		MoveCardFromFreeCelltoColumn(byte btFreeCell, byte btTo, BOOL bMakeMove = FALSE);
	BOOL		MoveCardFromFreeCelltoFreeCell(byte btFrom, byte btTo, BOOL bMakeMove = FALSE);
	
	
	EnumGameState	CheckForValidMoves();//also win
	BOOL		CheckAceRack();//auto move aces and twos

	void		SaveUndoInfo();

public:
	HWND		m_hWnd;

	byte		m_btColumns[CARD_COLUMNS][MAX_CARDS_PER_COLUMN];//array to store cards 
	byte		m_btFreeCells[4];
	byte		m_btAceRacks[4];
	
	byte		m_btSelected; // -1 for nothing selected
	byte		m_btCardsRemaining;
	int			m_iGameNumber;

	int			m_iGamesWon;
	int			m_iGamesLost;
	int			m_iCurrentStreak;
	int			m_iLongestStreak;

	DWORD		m_dwTickCount; //we'll use this for the double click time out

//	BOOL		m_bAutoPlay;//auto places cards on the "ace rack"
	EnumAutoPlay	m_eAutoPlay;
	BOOL		m_bPlaySounds;
	BOOL		m_bShowAnimations;

	BOOL		m_bGameInPlay;
	BOOL		m_bIsSolvable;

	EnumBackgroundColor m_eTableColor;

	EnumCardLocations m_eLastFrom; 
	EnumCardLocations m_eLastTo;

private:
	TypeFreeCellUndo m_sUndo;
	
};
