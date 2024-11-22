#pragma once

#include "IssVector.h"

#define NULL_CARD				255
#define CARD_BLANK				53

#define CARD_BLANK_SPADE        54 
#define CARD_BLANK_DIAMOND      55
#define CARD_BLANK_CLUB         56
#define CARD_BLANK_HEART        57

enum EnumBackgroundColor
{
    BGCOLOR_Blue,
    BGCOLOR_Green,
    BGCOLOR_Purple,
    BGCOLOR_Red,
    BGCOLOR_Orange,
    BGCOLOR_Pink,
    BGCOLOR_Random,
    BGCOLOR_Count,
};

class CIssCribbage
{
public:
	CIssCribbage(void);
	~CIssCribbage(void);
	
public:
    BOOL                    IsGameInPlay(){return FALSE;};
    void                    Init(HWND hWnd){m_hWnd = hWnd;};
    EnumBackgroundColor     GetBackgroundColor(){return BGCOLOR_Blue;};



private:

private:
    HWND                    m_hWnd;
	
};