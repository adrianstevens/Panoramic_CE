#pragma once
#include "KillerGame.h"
#include "KenKenPuzzle.h"

class CKenKenGame: public CKillerGame
{
public:
    CKenKenGame(void);
    ~CKenKenGame(void);

    void            NewGame(EnumDifficulty eDifficulty);

    int             GetMathAnswer(int iX, int iY);

    EnumOperator    GetOp(int iX, int iY);  

    BOOL            IsPencilMarkValid(int iX, int iY, int iValue);

    void            LoadRegistry();
    void            SaveRegistry();

    void            Destroy();
    

private:
    int             GetDividePossibilities(int iSize, int iTotal);
    int             GetSubtractPossibilities(int iSize, int iTotal);
    int             GetMultiplyPossibilities(int iSize, int iTotal);
    int             GetAddPossibilities(int iSize, int iTotal);

    
private:
    int				    m_iKenKens[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
    EnumOperator        m_eKenKenOps[MAX_SAMUNAMPURES]; //value of each group of numbers

};
