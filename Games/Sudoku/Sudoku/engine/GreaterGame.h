#pragma once
#include "sudokugame.h"
#include "GreaterPuzzle.h"

class CGreaterGame : public CSudokuGame
{
public:
    CGreaterGame(void);
    ~CGreaterGame(void);

    EnumGreaterDir      GetGVertical(int iX, int iY);
    EnumGreaterDir      GetGHorizontal(int iX, int iY);

    BOOL                NewGame();

private:

private:
};
