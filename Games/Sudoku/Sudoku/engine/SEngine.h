#pragma once
#include "stdafx.h"
#include "SudokuGame.h"
#include "KillerGame.h"
#include "KakuroGame.h"
#include "KenKenGame.h"
#include "GreaterGame.h"

#define DEFAULT_TIME 3599

enum EnumSudokuType
{
    STYPE_Sudoku,
    STYPE_Killer,
    STYPE_Kakuro,
    STYPE_KenKen,
    STYPE_Greater,
    STYPE_Count,
};

class CSEngine
{
public:
    CSEngine(void);
    ~CSEngine(void);

    void                Initialize(HINSTANCE hInst, HWND hWnd);

    EnumSudokuType      GetGameType(){return m_eGameType;};
    void                SetGameType(EnumSudokuType eType){m_eGameType = eType;};

    BOOL			    NewGame();
    BOOL			    IsGameComplete(int iGameTime = DEFAULT_TIME);
    void			    RestartGame();

    int                 GetCellsFree();

    void				OnSetGuess(int iX, int iY, int iGuess);//public
    void                OnSetPencilMark(int iX, int iY, int iPencil);
    POINT				GetLastCorrectGuess();
    POINT				GetLastHint();
    POINT				GetLastWrongGuess();

    int                 GetGridSize();

    BOOL				IsGiven(int iX, int iY);
    BOOL                IsLocked(int iX, int iY);
    int                 GetSolution(int iX, int iY);
    int					GetGuess(int iX, int iY);

    int                 GetPencilMarks(int iX, int iY); 
    
    BOOL                SetDifficulty(EnumDifficulty eDiff);
    EnumDifficulty      GetDifficulty(){return m_eDiff;};

    BOOL                Undo();
    BOOL                Hint();
    BOOL                Solve();

    BOOL                GetAutoPencilMarks();
    void                SetAutoPencilMarks(BOOL bAuto);
    BOOL                GetAutoRemovePencil();
    void                SetAutoRemovePencil(BOOL bAuto);

    void                SetShowLocked(BOOL bShowLocked);
    void                SetShowErrors(BOOL bShow);//not needed

    void                ClearPencilMarks();

    BOOL                GetShowHints(){return m_bShowHints;};
    void                SetShowHints(BOOL bShow){m_bShowHints = bShow;};

    void                CheckTime(int iGameTime);

    int                 GetBestTime(EnumSudokuType eType, EnumDifficulty eDiff){return m_iBestTimes[eType][eDiff];};
    int                 GetGameCount(EnumSudokuType eType, EnumDifficulty eDiff){return m_iGameCounts[eType][eDiff];};
    int                 GetAvgTime(EnumSudokuType eType, EnumDifficulty eDiff){return m_iAvgTimes[eType][eDiff];};

    //Killer Specific
    DWORD               GetOutline(int iX, int iY);
    int                 GetTotal(int iX, int iY);
    int                 GetSamuIndex(int iX, int iY);
    int                 GetSamuValue(int iX, int iY);
    BOOL                DrawTotal(int iX, int iY);

    //Kakuro Specific
    int                 GetHorzTotal(int iX, int iY);
    int                 GetVertTotal(int iX, int iY);
    BOOL                IsBlank(int iX, int iY);  //for the non playable pieces
    BOOL                IsKakuro(int iX, int iY); //the pieces with the totals 
    BOOL                SetBoardSize(EnumSize eSize);

    BOOL                IsPencilMarkValid(int iValue, int iX, int iY);

    //KenKen Specific
    int                 GetKenKenAnswer(int iX, int iY);
    EnumOperator        GetKenKenOp(int iX, int iY);

    //Greater Specific
    EnumGreaterDir      GetGVertical(int iX, int iY);
    EnumGreaterDir      GetGHorizontal(int iX, int iY);
    
private:

private:
    CSudokuGame m_oSudoku;
    CKakuroGame m_oKakuro;
    CKillerGame m_oKiller; 
    CKenKenGame m_oKenKen;
    CGreaterGame m_oGreater;

    EnumSudokuType m_eGameType;

    EnumDifficulty m_eDiff; 
    BOOL        m_bShowHints;
    
    int         m_iAvgTimes[STYPE_Count][DIFF_UnKnown];//unknown is the same as the count
    int         m_iBestTimes[STYPE_Count][DIFF_UnKnown];//unknown is the same as the count
    int         m_iGameCounts[STYPE_Count][DIFF_UnKnown];//count of games played per type

};
