#pragma once
#include "IssString.h"
#include "dance.h"
#include "SudokuGlobals.h"


struct ElementLocation
{
	int indexI;
	int indexJ;
};



class CSudokuPuzzle
{
public:
	CSudokuPuzzle(void);
	~CSudokuPuzzle(void);

	void GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst);
	

protected:
    void GetNewHardPuzzle(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst);

	BOOL IsBoardValid();
	BOOL PopulateCell(int iCellIndex);
	BOOL IsColumnValid(int iColumnIndex);
	BOOL IsRowValid(int iRowIndex);
	BOOL IsSectionValid(int iRowIndex, int iColumnIndex);
	BOOL IsSectionValid(int iSectionIndex);
	void RandomizeArray(int iArray[], int iSize);
	void GeneratePuzzle();
	void ArrayCopy(int iFrom[NUM_ROWS][NUM_COLUMNS], int iTo[NUM_ROWS][NUM_COLUMNS]);
	int	Cardinality(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iRow, int iColumn);
	void MyQSort(int iSortArray[81], int iCompanionArray[81], int iFirst, int iLast);

    void FixNonSolvable(int iPuzzleArray[][9], int iSolutionArray[][9]);

    void SwapNumbers(int iPuzzleArray[][9], int iSolutionArray[][9], int iNumSwaps = 3);
    void ManipulatePuzzle(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void Rotate270(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void Rotate180(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void Rotate90(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void MirrorXY(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void MirrorHorizontal(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void MirrorVertical(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void SwapColumns(int iPuzzleArray[][9], int iColumn1, int iColumn2);
    void SwapRows(int iPuzzleArray[][9], int iRow1, int iRow2);
    void SwapColumnSections(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void SwapRowSections(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void SwapColumnSingle(int iPuzzleArray[][9], int iSolutionArray[][9]);
    void SwapRowSingle(int iPuzzleArray[][9], int iSolutionArray[][9]);
    


protected:
	CIssString*		m_oStr;
	int				m_iGeneratedPuzzle[NUM_ROWS][NUM_COLUMNS];
	ElementLocation	m_oGeneratedOrder[NUM_ROWS*NUM_COLUMNS];
	DLX				m_oSolver;
	BOOL			m_bNumbers[NUM_DIGITS];
	BOOL			m_bTruelyRandom;							//if we remove values randomly or force removals from each section

};