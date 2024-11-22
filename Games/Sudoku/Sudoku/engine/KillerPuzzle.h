#pragma once
#include "IssString.h"
#include "dance.h"
#include "SudokuGlobals.h"


class CKillerPuzzle
{
#define MAX_SAMUNAMPURES NUM_COLUMNS*NUM_ROWS

    struct ElementLocation
    {
        int indexI;
        int indexJ;
    };


public:
	CKillerPuzzle(void);
	~CKillerPuzzle(void);

	BOOL	GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst = NULL);
	void	GenerateSamu(EnumDifficulty eDiff);
	int		GetSamuIndex(int iRow, int iColumn){return m_iSamud[iRow][iColumn];};
	int		GetSamuValue(int iRow, int iColumn){return m_iSamunamupures[m_iSamud[iRow][iColumn]];};
	
protected:
	BOOL	IsBoardValid();
	BOOL	PopulateCell(int iCellIndex);
	BOOL	IsColumnValid(int iColumnIndex);
	BOOL	IsRowValid(int iRowIndex);
	BOOL	IsSectionValid(int iRowIndex, int iColumnIndex);
	BOOL	IsSectionValid(int iSectionIndex);
	void	RandomizeArray(int iArray[], int iSize);
	void	GeneratePuzzle();
	void	ArrayCopy(int iFrom[NUM_ROWS][NUM_COLUMNS], int iTo[NUM_ROWS][NUM_COLUMNS]);
	int		Cardinality(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iRow, int iColumn);
	void	MyQSort(int iSortArray[81], int iCompanionArray[81], int iFirst, int iLast);

	BOOL	IsSamuExpansionPossible(int iRow, int iColumn, BOOL *bNumArray);
	BOOL	IsLeftAval(int iRow, int iColumn, BOOL *bNumArray);
	BOOL	IsRightAval(int iRow, int iColumn, BOOL *bNumArray);
	BOOL	IsAboveAval(int iRow, int iColumn, BOOL *bNumArray);
	BOOL	IsBelowAval(int iRow, int iColumn, BOOL *bNumArray);

    BOOL    FixNonSolvable(int iPuzzleArray[][9], int iSolutionArray[][9]);
    
    BOOL    IsNonFixable(int iPuzzleArray[][9], int iSolutionArray[][9]);


    BOOL    FixNonSolvable2(int iPuzzleArray[][9], int iSolutionArray[][9]);

    BOOL    SplitKiller(int iPuzzleArray[][9], int iSolutionArray[][9], POINT pt1, POINT pt2, BOOL bHorizontal);

    virtual void    Reset();
    virtual int     GetSamuSize(EnumDifficulty eDiff);

    void    OutputGrid(TCHAR* szName, int iArray[][9]);
    void    OutputKiller();

public:


protected:
	CIssString*		m_oStr;
	int				m_iGeneratedPuzzle[NUM_ROWS][NUM_COLUMNS];
	ElementLocation	m_oGeneratedOrder[NUM_ROWS*NUM_COLUMNS];
	DLX				m_oSolver;
	BOOL			m_bNumbers[NUM_DIGITS];
	BOOL			m_bTruelyRandom;							//if we remove values randomly or force removals from each section

    int				m_iSamuCounts[MAX_SAMUNAMPURES]; //number of cells in each group
	int				m_iSamunamupures[MAX_SAMUNAMPURES]; //value of each group of numbers
	int				m_iNumSamus;//number of samunamupures
	BOOL			m_iSamud[NUM_ROWS][NUM_COLUMNS];//if the cells have been samud yet

};