#pragma once
#include "killerpuzzle.h"

enum EnumOperator
{
    OP_Plus,
    OP_Minus,
    OP_Times,
    OP_Divide,
    OP_None,
};

class CKenKenPuzzle : public CKillerPuzzle
{
public:
    CKenKenPuzzle(void);
    ~CKenKenPuzzle(void);

    void                GenerateKenKen(EnumDifficulty eDiff);
    BOOL                GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst = NULL);
    void                Reset();

    int                 GetKenKen(int iIndex);
    EnumOperator        GetOp(int iIndex);

private:
    int                 GetSamuSize(EnumDifficulty eDiff);
    

private:

    //int				m_iSamunamupures[MAX_SAMUNAMPURES]; //value of each group of numbers
    //int				    m_iSamuCounts[MAX_SAMUNAMPURES]; //number of cells in each group
    int				    m_iKenKens[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
    EnumOperator        m_eKenKenOps[MAX_SAMUNAMPURES]; //value of each group of numbers
    
};
