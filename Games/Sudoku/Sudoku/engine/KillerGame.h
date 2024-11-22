#pragma once
#include "SudokuGame.h"


class CKillerGame : public CSudokuGame
{
public:
    CKillerGame(void);
    ~CKillerGame(void);

    void            NewGame(EnumDifficulty eDifficulty = DIFF_Medium);
    DWORD           GetOutline(int iX, int iY);
    int             GetTotal(int iX, int iY);
    int             GetSamuIndex(int iX, int iY){return m_iSamuIndex[iX][iY];};
    int             GetSamuValue(int iX, int iY){return m_iSamuValue[iX][iY];};

    BOOL            DrawTotal(int iX, int iY);

    
    virtual BOOL	IsPencilMarkValid(int iX, int iY, int iValue);
protected:
    int             GetGuessPossibilities(int iNumCells, int iTotal);//right out of iSS Killer
    BOOL            IsGuessValid(int iGuess, int iNumberToCheck);
    int             GetCellsInGroup(int iX, int iY);

    virtual void    LoadRegistry();
    virtual void    SaveRegistry();

    virtual void    Destroy();
    
protected:
    int				m_iSamuIndex[NUM_ROWS][NUM_COLUMNS];//index for the colors
    int				m_iSamuValue[NUM_ROWS][NUM_COLUMNS];//total within the Samu (yeah I'm making up words)
    BOOL			m_bDrawSamuValue[NUM_ROWS*NUM_COLUMNS];	//bool to control if we've drawn the Samu value yet or not

    BOOL            m_bSetSingleValues;//set the values for any number in a group of 1

};
