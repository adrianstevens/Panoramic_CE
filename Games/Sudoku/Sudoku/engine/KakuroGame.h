#pragma once

#include "windows.h"
#include "IssVector.h"
#include "IssString.h"
#include "SudokuGlobals.h"

enum EnumTilePiece
{
	TP_Blank,
	TP_Kakuro,
	TP_PencilMark,
	TP_Guess,
};

class CKakuroGame
{
public:
	CKakuroGame(void);
	~CKakuroGame(void);

	BOOL				Destroy();
	BOOL				Initialize(HINSTANCE hInst, HWND hWnd);
	BOOL				NewGame();
	int					GetBoardSize(){return m_iBoardSize;};
	void				TogglePencil(int iNum, int iX, int iY);
	void				ClearPencil(int iX, int iY);
	void				SetGuessPiece(int iNum, int iX, int iY);
	double				GetAccuracy();//decimal percentage
	BOOL				ClearAllPencilMarks();
	BOOL				Hint();
    BOOL                Undo();

    int					GetNumFreeCells(){return GetFreeCells();};

	int					GetFreeCells(){return m_iNumFreeCells;};
	int					GetNumSeconds(){return m_iNumSeconds;};
	void				IncrementTimer();

	BOOL				IsGameInPlay(){return m_bIsGameInPlay;};
	EnumLevel			GetLevel(){return m_eLevel;};
//	EnumSkins			GetSkin(){return m_eSkin;};
	EnumSize			GetSize(){return m_eSize;};
	BOOL				GetAutoPencil(){return m_bAutoPencilMarks;};

	BOOL				GetShowErrors(){return m_bShowErrors;};


	void				StopCurrentGame(){m_bIsGameInPlay = FALSE;};
	void				SetLevel(EnumLevel eLevel){m_eLevel = eLevel;};
	void				SetSize(EnumSize eSize){m_eSize = eSize;};
	void				SetAutoPencilMarks(BOOL bAuto){m_bAutoPencilMarks = bAuto; if(bAuto)AutoFillPencilMarks();};
    void                SetAutoRemovePencilMarks(BOOL bAuto){m_bAutoRemovePencilMarks = bAuto;};
	void				SetShowErrors(BOOL bShowErrors){m_bShowErrors = bShowErrors;};
	
	EnumTilePiece		GetTileType(int iX, int iY);
	int					GetGuessPiece(int iX, int iY);
	int					GetKakuroPiece(int iX, int iY);

/*******************************************/
	int					GetType(int d);
	int					GetPencil(int d);
	int					GetGuess(int d);
	int					GetSolution(int d);
	int					GetHorzTotal(int d);
	int					GetVertTotal(int d);
	void				SetType(int& iCurrent, int iType);
	void				SetPencil(int& iCurrent, int iPencil);
	void				SetGuess(int& iCurrent, int iGuess);
	void				SetSolution(int& iCurrent, int iSolution);
	void				SetHorzTotal(int& iCurrent, int iHorzTotal);
	void				SetVertTotal(int& iCurrent, int iVertTotal);
/*******************************************/



	BOOL				IsPencilMarkValid(int iX, int iY, int iValue);  //makes more sense to do this in the engine imo
	BOOL				IsPencilMarkPossible(int iX, int iY, int iValue);  //if we're not doing the smart pencil marks
	BOOL				IsGameComplete();

    POINT               GetLastHint(){return m_ptLastHint;};
    POINT				GetLastCorrectGuess();

	void				RestartGame();

    void                SolveGame();//easy enough 

private:	// functions
	void				LoadRegistry();
	void				SaveRegistry();

	void				RandomizeBoard(TCHAR* szGame);//for new games ... flip, rotate, etc
	void				Rotate90(TCHAR* szGame);
	void				Rotate180(TCHAR* szGame);
	void				Rotate270(TCHAR* szGame);
	void				MirrorXY(TCHAR* szGame);

	TCHAR				GetValueFromTCHAR(int iX, int iY, TCHAR* szGame);
	
	int					AddKakuroPiece(int iSumVertical, int iSumHorizontal);
	int					GetGuessPiece(int iIndex);
	int					GetKakuroPiece(int iIndex);
	void				CalcFreeCells();

	BOOL				IsTileValid(int iX, int iY);

	int					FindVertTotal(int iX, int iY, BOOL bCheat = TRUE);
	int					FindHorzTotal(int iX, int iY, BOOL bCheat = TRUE);
	int					FindVertCWSize(int iX, int iY, BOOL bCheat = TRUE);
	int					FindHorzCWSize(int iX, int iY, BOOL bCheat = TRUE);
	int					GetVertGuesses(int iX, int iY);//binary
	int					GetHorzGuesses(int iX, int iY);//binary
	int					IsPencilValueValid(int iValue, int iTotal, int iGroupSize);  //private function to determine if a number can be used to form this total

	void				AutoFillPencilMarks();
	void				RecalcPencilMarks(int iX, int iY);

	TCHAR*				GetPuzzle(EnumLevel eLevel, EnumSize eSize);

	BOOL				IsGuessValid(int iGuess, int iNumberToCheck);
	int					GetGuessPossibilities(int iNumCells, int iTotal);

    

private:	// variables
    TypeLastMove m_sLastMove;
	CIssString* m_oStr;
	HINSTANCE	m_hInst;
    HWND        m_hWnd;

	int			m_iTile[BOARD_SIZE][BOARD_SIZE];		// our 10x10 array of the board
	int			m_iBoardSize;
	BOOL		m_bIsGameInPlay;						// is a game going on right now
	BOOL		m_bAutoPencilMarks;		
	BOOL		m_bShowErrors;
    BOOL        m_bAutoRemovePencilMarks;

	EnumLevel	m_eLevel;								// current level
	EnumSize	m_eSize;								// current size (ie..puzzle dimension)
	int			m_iNumFreeCells;						// number of free cells
	int			m_iMistakeCounter;						// number of mistakes ... we'll give an accuracy rating at the end 
	int			m_iTotalPlayableCells;					// number of cells the user must solve ... used for the accuracy percentage
	int			m_iNumSeconds;

	POINT       m_ptLastCorrectGuess;					// we'll use this for the correct guess animation
    POINT       m_ptLastHint;
};
