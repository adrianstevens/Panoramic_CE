#pragma once

#include "stdafx.h"
#define CARD_BLANK 53

class CIssPyramid
{
public:
    CIssPyramid(void);
    ~CIssPyramid(void);

    void        NewGame();

private:
    BOOL        IsCardFree(int iRow, int iIndex);

private:
    byte        m_btCards[7][7];//not actually using em all

    byte        m_bt7[7];
    byte        m_bt6[6];
    byte        m_bt5[5];
    byte        m_bt4[4];
    byte        m_bt3[3];
    byte        m_bt2[2];
    byte        m_bt1[1];



    byte		m_btCardsRemaining;
    int			m_iGameNumber;

    int			m_iGamesWon;
    int			m_iGamesLost;
    int			m_iCurrentStreak;
    int			m_iLongestStreak;

    DWORD		m_dwTickCount; //we'll use this for the double click time out


};
