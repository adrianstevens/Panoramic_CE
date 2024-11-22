#pragma once
#include "stdafx.h"
#include "SudokuPuzzle.h"
#include "SudokuGlobals.h"

class CSudokuGame
{
public:
	CSudokuGame(void);
	~CSudokuGame(void);

    virtual void        Init(HINSTANCE hInst, HWND hWnd){m_hInst = hInst; m_hWnd = hWnd;};
	virtual BOOL		NewGame();
	virtual BOOL		IsGameComplete();
	virtual void		RestartGame();
	virtual void		SaveRegistry();
	virtual void		LoadRegistry();
	virtual void		StopGame();
	virtual int			GetScore(){return m_iScore;};
	virtual void		OnSetGuess(int iX, int iY, int iGuess);//public
	virtual POINT		GetLastCorrectGuess();
	virtual POINT		GetLastHint();
	virtual POINT		GetLastWrongGuess();

	//Tile Functions
	BOOL				IsGiven(int iX, int iY);
	BOOL				IsLocked(int iX, int iY);
	int					GetSolution(int iX, int iY);
	int					GetGuess(int iX, int iY);
	int					GetPencilMarks(int iX, int iY);
	void				SetIsGiven(int iX, int iY, BOOL bIsGiven);
	void				SetIsLocked(int iX, int iY, BOOL bIsLocked);
	void				SetSolution(int iX, int iY, int iSolution);
	void				SetGuess(int iX, int iY, int iGuess);//not for public use 
	void				SetPencilMarks(int iX, int iY, int iPencilMarks);
	void				TogglePencilMarks(int iX, int iY, int iNumber);
		

	BOOL				ClearAllPencilMarks();
	BOOL				Hint();
	BOOL				GetAutoPencil(){return m_bAutoPencilMarks;};
    BOOL                GetAutoRemovePencil(){return m_bAutoRemovePencilMarks;};
	void				SetAutoPencilMarks(BOOL bAuto){m_bAutoPencilMarks = bAuto; if(bAuto)AutoFillPencilMarks();};
    void                SetAutoRemovePencilMarks(BOOL bAuto){m_bAutoRemovePencilMarks = bAuto;};
	BOOL				GetShowErrors(){return m_bShowErrors;};
	void				SetShowErrors(BOOL bShowErrors){m_bShowErrors = bShowErrors;};
	void				SolveGame();
    BOOL                Undo();

	void		        AutoFillPencilMarks();
	void	            ReCalcPencilMarks();

	// custom dlg stuff
	void				NewBoard();
	void				SaveSolvableGame();
	
	// Game state stuff
	int					GetBoardSize(){return m_iBoardSize;};
	BOOL				IsGameInPlay(){return m_bIsGameInPlay;};
	int					GetNumFreeCells(){return m_iNumFreeCells;};
	int					GetNumSeconds(){return m_iNumSeconds;};
	void				IncrementTimer();
	BOOL				CheckCellSolution(int iX, int iY);
	void				CheckSolution();
	void				ResetLockedItems();

	//pencil mark stuff
	virtual BOOL		IsPencilMarkValid(int iX, int iY, int iValue);
	BOOL				ReCalcCell(int iX, int iY, int iXCell, int iYCell);
	void				RecalcPencilMarks(int iX, int iY);

	// Options stuff
	EnumDifficulty		GetDifficulty(){return m_eDifficulty;};
	BOOL				IsLocked(){return m_bShowLockedState;};
	int					GetHighScore(){return m_iHighScore;};
	void				SetDifficulty(EnumDifficulty eDifficulty){m_eDifficulty = eDifficulty;};
	void				SetLocked(BOOL bLocked);//{m_bShowLockedState = bLocked;};


protected:	// Function
	BOOL				IsValid(int iX, int iY);
	int					GetTile(int iX, int iY);
	void				SetTile(int iX, int iY, int iTile);
	void				CalcFreeCells();
	int					GetSubsection(int iRowColumnIndex);
	BOOL				IsSolved(int iX, int iY);
	BOOL				IsColumnSolved(int iColumnIndex);
	BOOL				IsRowSolved(int iRowIndex);
	BOOL				IsSubsectionSolved(int iRowIndex, int iColumnIndex);
	BOOL				IsGameSolved();

protected:
	EnumDifficulty		m_eDifficulty;				// Difficulty of the Game
    TypeLastMove        m_sLastMove;
    

    HWND                m_hWnd;
    HINSTANCE           m_hInst;

	POINT				m_ptLastCorrectGuess;	
	POINT				m_ptLastWrongGuess;
	POINT				m_ptLastHint;
	
	int					m_iTile[SUDOKU_Size][SUDOKU_Size];	// Tiles [1 bit given][1 bit locked][4 bits solution][4 bits guess][9 bits pencilmarks]
	int					m_iBoardSize;						// board size, for now it's regular 9 by 9
	int					m_iNumFreeCells;					// how many free cells are there currently
	int					m_iNumSeconds;						// Number of Seconds that have passed
	int					m_iHighScore;						// High Score
	int					m_iScore;
	int					m_iMistakeCounter;
    		
	BOOL				m_bShowLockedState;					// Should we show the locked items
	BOOL				m_bIsGameInPlay;					// is there a game currently in progress?
	BOOL				m_bAutoPencilMarks;
	BOOL				m_bShowErrors;
    BOOL                m_bAutoRemovePencilMarks;           // if not auto it'll remove pencil marks as you solve (kinda cool)
};
