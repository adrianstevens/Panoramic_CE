#pragma once
#include "sudokupuzzle.h"

enum EnumGreaterDir
{
    GDIR_Left,
    GDIR_Right,
    GDIR_Up,
    GDIR_Down,
    GDIR_Unknown,
};

class CGreaterPuzzle : public CSudokuPuzzle
{
public:
    CGreaterPuzzle(void);
    ~CGreaterPuzzle(void);

    EnumGreaterDir      GetDirection(int iX, int iY);

    void                GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[][9], int iSolutionArray[][9], HINSTANCE hInst);

private:
    void                CalcGreaters();

private:


};
