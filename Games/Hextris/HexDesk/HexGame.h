#pragma once

#include "CurrentPiece.h"
#include "IssVector.h"

#define GAME_WIDTH 10
#define MAX_GAME_HEIGHT 40 //we'll see about this

#define WM_LINE_TOBE_CLEARED	WM_USER + 50 //wParam is the line, lParam is 1 if its the "alt" line
#define WM_LINE_CLEARED			WM_USER + 51
#define WM_LEVEL_CHANGE			WM_USER + 52 //wParam is level, lParam is the suggested timing to drop
#define WM_TIME_UP				WM_USER + 53 //for timed games only
#define WM_NO_MOVES				WM_USER + 54 //we've hit the top
#define WM_40LINES_CLEARED		WM_USER + 55
#define WM_PIECE_PLACED			WM_USER + 56 //for the sound
#define WM_NEW_HISCORE			WM_USER + 57 //sound effects 
#define WM_PIECE_SWITCHED		WM_USER + 58 //sound effects

#define NUM_PIECE_PREVIEWS		5

#define MAX_LINES				8

enum EnumGameState
{
	GS_Ingame,
	GS_GameOver,
	GS_Paused,
	GS_Menu,
};//we'll probably change this

enum EnumGameType
{
	GT_Classic,		//progress through the levels
	GT_Challenge,	//garbage at start of each level, must get 10 lines to progress then the level resets with new garbage 
	GT_Patterns,	//Pre-defined preset block patterns ... 50ish??
	//	GT_Speed,		//like classic but garbage comes up from the bottom of the play field
	//	GT_SuperHex,	//special pieces and the like ... we'll work on this later
	GT_Ultra,		//accumulate as many points as possible in 3 minutes
	GT_40,			//clear 40 lines as quickly as possible
	GT_NumberOfTypes,

	/* VS
	Head to head single player but when you get 2 lines the opponent gets 1 line of crap
	3 gives 2
	And 4 gives 4
	*/
};

struct ClearedType
{
	byte	btRow;
	bool	bAlt;
};

class CHexGame
{
public:
	CHexGame(void);
	~CHexGame(void);

	void			Init(HWND hWnd){m_hWndDlg = hWnd;};

	void			SetNumRows(int iRows);
	int				GetNumRows(){return m_iRows;};

	EnumGameState	GetGameState(){return m_eState;};
	EnumGameType	GetGameType(){return m_eGameType;};
	BOOL			SetGameType(EnumGameType eGameType);

	CurrentPiece*	GetCurrentPiece(){return m_oCurPiece[0];};
	CurrentPiece*	GetNextPiece(int i){return m_oCurPiece[i];};
	CurrentPiece*	GetSavedPiece(){return m_oSavedPiece;};
	CurrentPiece*	GetGhost(){return m_oGhost;};

	void			NewGame();

	void			OnTimer();//pump the piece down
	void			OnClock();//pumped every second for various stuff
	int				GetTime(){return m_iTime;};

	//interface
	BOOL			OnLeft();//{m_oCurPiece.MovePieceLeft();};
	BOOL			OnRight();//{m_oCurPiece.MovePieceRight();};
	BOOL			OnDown();//{m_oCurPiece.MovePieceDown();};
	void			OnUp();//{m_oCurPiece.MovePieceUp();};
	BOOL			OnRotate();
	void			OnRotateAlt(){m_oCurPiece[0]->Rotate();};
	void			OnDrop();
	void			OnSwitchPiece();//saved with current

	int				GetScore(){return m_iScore;};
	int				GetHighScore(){return m_iHighScore[m_eGameType];};
	int				GetLevel(){return m_iLevel + 1;};
	int				GetRowsCleared(){return m_iRowsCleared;};
	int				GetLevelTiming();

	GameHexagon*	GetBoardHex(int iX, int iY){return &m_oGameField[iX][iY];};
	GameHexagon*	GetPreDropHex(int iX, int iY){return &m_oPreDropField[iX][iY];};
	GameHexagon*	GetPrevHex(int iX, int iY){return &m_oPrevField[iX][iY];};

private:
	void			LoadRegistry();
	void			SaveRegistry();

	void			Reset();
	void			ClearGameBoard();
	void			AddGarageBlocks();
	void			NewPiece();
	BOOL			CheckCollision(CurrentPiece* oPiece);
	void			CheckForEndGameState();
	void			SetCurPieceToBoard();
	void			CheckForCompleteLines();
	void			ScoreClearedLine();//we'll handle multiple lines later



	void			SetLineToCleared(int iY, BOOL bAlt);
	void			DropPieces();

	void			SetPattern();//for GT_Patterns (need a better name I suppose...)

	void			CalcGhostLocation();
	
private:
	GameHexagon		m_oGameField[GAME_WIDTH][MAX_GAME_HEIGHT];//our game field
	GameHexagon		m_oPrevField[GAME_WIDTH][MAX_GAME_HEIGHT];//before a line remove
	GameHexagon		m_oPreDropField[GAME_WIDTH][MAX_GAME_HEIGHT];//lines erased without dropping
	CurrentPiece*	m_oCurPiece[NUM_PIECE_PREVIEWS];

	CurrentPiece*	m_oSavedPiece;
	CurrentPiece*	m_oGhost;

	ClearedType		m_sClearedLines[MAX_LINES];
	int				m_iClearedLines;

	EnumGameState	m_eState;
	EnumGameType	m_eGameType;

	HWND			m_hWndDlg;

	int				m_iRows; // number of rows in the game

	int				m_iRowsCleared;
	int				m_iScore;
	int				m_iLevel;
	int				m_iStartingLevel; //in case its been changed in the options

	int				m_iTime;//for games with any sort of timer

	//statistics
	int				m_iStats[NUMBER_OF_PIECES];
	int				m_iHighScore[GT_NumberOfTypes];

	BOOL			m_bNewHighScore;

};
