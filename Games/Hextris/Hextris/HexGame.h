#pragma once

#include "CurrentPiece.h"
#include "IssVector.h"
#include "IssHighScores.h"
#include "IssString.h"

#define GAME_WIDTH 10
#define MAX_GAME_HEIGHT 24 //we'll see about this

#define WM_LINE_TOBE_CLEARED	WM_USER + 50 //wParam is the line, lParam is 1 if its the "alt" line
#define WM_LINE_CLEARED			WM_USER + 51
#define WM_LEVEL_CHANGE			WM_USER + 52 //wParam is level, lParam is the suggested timing to drop
#define WM_TIME_UP				WM_USER + 53 //for timed games only
#define WM_NO_MOVES				WM_USER + 54 //we've hit the top
#define WM_40LINES_CLEARED		WM_USER + 55
#define WM_PIECE_PLACED			WM_USER + 56 //for the sound
#define WM_NEW_HISCORE			WM_USER + 57 //sound effects 
#define WM_PIECE_SWITCHED		WM_USER + 58 //sound effects
#define WM_BONUS_IN_QUE			WM_USER + 59 //for sound effects ... wParem is the que count, lParam is the number of saved gems (you never know)
#define WM_BONUS_SAVED			WM_USER + 60 //effects
#define WM_BONUS_SCORE			WM_USER + 61 //wParam will be the score 
#define WM_ALL_ORBS				WM_USER + 62 //when we max out the orbs
#define WM_DOUBLE_ORB			WM_USER + 63

#define NUM_PIECE_PREVIEWS		2 //this also includes the current piece so this must be at least 1

#define MAX_LINES				8	//max lines you can clear at once ... this "might" be too low
#define MAX_CLEARED_ORBS		10  //really shouldn't exceed this

#define NUM_HIGHSCORES			10 //arbitrary

#define MAX_SAVED_GEMS			4 //for now

#define LINES_PER_LEVEL			10

#define LEVEL_BONUS				1000
#define POINTS_PER_LINE			50
#define FOUR_ORB_BONUS			25000

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
	// GT_Patterns,	//Pre-defined preset block patterns ... 50ish??
	//	GT_Speed,		//like classic but garbage comes up from the bottom of the play field
	//	GT_SuperHex,	//special pieces and the like ... we'll work on this later
	GT_Ultra,		//accumulate as many points as possible in 3 minutes
	//GT_40,			//clear 40 lines as quickly as possible
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

//could add names here too
struct TypeHighScore
{
	int			iScore;
	int			iLevel;
	EnumGameType eGameType;
};

class CHexGame
{
public:
	CHexGame(void);
	~CHexGame(void);

	void			Init(HWND hWnd){m_hWndDlg = hWnd;};
	void			Reset();
	void			LoadRegistry();
	void			SaveRegistry();
	BOOL			IsGameInPlay(){return (m_eState==GS_Ingame?TRUE:FALSE);};

	void			SetNumRows(int iRows);
	int				GetNumRows(){return m_iRows;};
	int             GetGameHeight(){return GetNumRows();};//yes its the same as above

	EnumGameState	GetGameState(){return m_eState;};
	EnumGameType	GetGameType(){return m_eGameType;};
	BOOL			SetGameType(EnumGameType eGameType);

	CurrentPiece*	GetCurrentPiece(){return m_oCurPiece[0];};
	CurrentPiece*	GetNextPiece(int i){return m_oCurPiece[i];};
	CurrentPiece*	GetSavedPiece(){return m_oSavedPiece;};
	CurrentPiece*	GetGhost(){return m_oGhost;};

	void			SetShowFPS(BOOL bShowFPS){m_bShowFPS = bShowFPS;};
	BOOL			GetShowFPS(){return m_bShowFPS;};

	void			SetPowerSaving(BOOL bPower){m_bPowerSaving = bPower;};
	BOOL			GetPowerSaving(){return m_bPowerSaving;};

	
	int				GetMusicLevel(){return m_iMusicLevel;};
	void			SetMusicLevel(int iLevel){m_iMusicLevel = iLevel;};
	int				GetSFXLevel(){return m_iSFXLevel;};
	void			SetSFXLevel(int iLevel){m_iSFXLevel = iLevel;};
	int				GetStartLevel(){return m_iStartingLevel;};
	void			SetStartLevel(int iStartLevel){m_iStartingLevel = iStartLevel;};

	CIssHighScores* GetHighScoreTable(){return &m_oHighScores[m_eGameType];};

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
	BOOL			OnUseGem();

	int				GetScore(){return m_iScore;};
	int				GetLastScoreInc(){return m_iLastScoreIncrease;};
	int				GetLevel(){return m_iLevel + 1;};
	int				GetLevelComp();
	double			GetLevelPercent(){return m_dbLevelPercent;};
	int				GetRowsCleared(){return m_iRowsCleared;};
	int				GetLevelTiming();
	int				GetSavedGems(){return m_iSavedGems;};
	GameHexagon*	GetBoardHex(int iX, int iY){return &m_oGameField[iX][iY];};
	GameHexagon*	GetPreDropHex(int iX, int iY){return &m_oPreDropField[iX][iY];};
	GameHexagon*	GetPrevHex(int iX, int iY){return &m_oPrevField[iX][iY];};

	BOOL			IsGamePiece(EnumHexPeice ePiece);

	BOOL			IsHighScore(){return m_bNewHighScore;};

	//pumped by the interface for now ... this kinda sucks but oh well
	void			FourOrbClear();

	//for the double ORB
	POINT			GetDoubleExOrb1(){return m_ptOrb1;};
	POINT			GetDoubleExOrb2(){return m_ptOrb2;};

	
private:
	void			ClearGameBoard();
	void			AddGarageBlocks();
	void			NewPiece();
	BOOL			CheckCollision(CurrentPiece* oPiece);
	BOOL			CheckForDoubleOrb(CurrentPiece* oPiece);
	void			CheckForEndGameState();
	void			SetCurPieceToBoard();
	void			CheckForCompleteLines();
	BOOL			CheckForDoubleOrb(POINT &pt1, POINT &pt2);//must be before in play piece is locked to board
	void			ScoreClearedLine();//we'll handle multiple lines later



	void			SetLineToCleared(int iY, BOOL bAlt);
	void			DropPieces();

	void			CalcGhostLocation();

	void			AddGemToQue();
	void			AddGemToSaved();

	void			SingleExplosion(POINT ptLocation, BOOL bScore = TRUE);
	void			SingleExplosion(int iX, int iY);
	void			DoubleExplosion(POINT ptLocation);

	EnumHexPeice	GetHexAtLocation(int iX, int iY);

	
	
	


private:
	CIssString*		m_oStr;
	CIssHighScores	m_oHighScores[GT_NumberOfTypes];
	GameHexagon		m_oGameField[GAME_WIDTH][MAX_GAME_HEIGHT+1];//our game field
	GameHexagon		m_oPrevField[GAME_WIDTH][MAX_GAME_HEIGHT+1];//before a line remove
	GameHexagon		m_oPreDropField[GAME_WIDTH][MAX_GAME_HEIGHT+1];//lines erased without dropping
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
	double			m_dbLevelPercent;
	int				m_iStartingLevel; //in case its been changed in the options

	int				m_iTime;		//for games with any sort of timer

	int				m_iBonusQue;	//que to go into new pieces
	int				m_iSavedGems;	//number of gems we have saved to play

	int				m_iMusicLevel;	// music level 0 - 64
	int				m_iSFXLevel;	// sound effects level 0 - 64

	int				m_iLastScoreIncrease;

	BOOL			m_bShowFPS;		// should we show frames per second
	BOOL			m_bPowerSaving; // bool for power savings 

	//statistics
	int				m_iStats[NUMBER_OF_PIECES];

	BOOL			m_bNewHighScore;


	//save the double orb explosion locations
	POINT			m_ptOrb1;
	POINT			m_ptOrb2;
};
