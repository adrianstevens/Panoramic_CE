#pragma once

#include "windows.h"

//because I'm too lazy to rename all of my resources
#define IDR_PNG_FavoritesImgAlphaVGA    IDR_PNG_CALL_ImgAlphaVGA
#define IDR_PNG_FavoritesImgAlpha       IDR_PNG_CALL_ImgAlpha
#define IDR_PNG_CallBgVGA               IDR_PNG_CALL_BKVGA
#define IDR_PNG_CallBg                  IDR_PNG_CALL_BK
#define IDR_PNG_FavoritesImgBorderVGA   IDR_PNG_CALL_ImgBorderVGA
#define IDR_PNG_FavoritesImgBorder      IDR_PNG_CALL_ImgBorder
#define IDR_PNG_DetailsBackGreen        -1 //maybe later


enum EnumColorScheme
{
    COLOR_Green,
    COLOR_Blue,
    COLOR_Orange,
    COLOR_Pink,
    COLOR_Purple,
    COLOR_Red,
    COLOR_Count,
};

extern EnumColorScheme g_eColorScheme;

UINT FindSkin(UINT uiSkin);
COLORREF FindSelectedColor();

#define SKIN(UI) FindSkin(UI)
#define COLOR_TEXT_SELECTED   FindSelectedColor()


