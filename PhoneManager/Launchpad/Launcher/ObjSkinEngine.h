#pragma once

#include "windows.h"




extern EnumColorScheme g_eColorScheme;

UINT FindSkin(UINT uiSkin);
COLORREF FindSelectedColor();

#define SKIN(UI) FindSkin(UI)
#define COLOR_TEXT_SELECTED   FindSelectedColor()


