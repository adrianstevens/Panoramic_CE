#pragma once

#include "windows.h"


enum EnumColorScheme
{
    COLOR_Green,
    COLOR_Blue,
    COLOR_Orange,
    COLOR_Pink,
    COLOR_Purple,
    COLOR_Red,
};

extern EnumColorScheme g_eColorScheme;

UINT FindSkin(UINT uiSkin);
COLORREF FindSelectedColor();

#define SKIN(UI) FindSkin(UI)
#define COLOR_TEXT_SELECTED   FindSelectedColor()


