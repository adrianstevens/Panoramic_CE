#pragma once

#include "stdafx.h"

class CBJMetrics
{
public:
    CBJMetrics(void);
    ~CBJMetrics(void);

    int         GetTextHeight();
    int         GetCountTextHeight();
    int         GetTextIndent();

    int         GetCardHeight();

    int         GetCountSize();
    int         GetCountWidth();

    int         GetChipSize();


/*    POINT       GetPlayerHandLocation();
    POINT       GetDealHandLocation();
    POINT       GetSplitHandLocation();
    POINT       GetPlayerwSplitLocation();*/

//    POINT       GetChipsLocation();


private:
};
