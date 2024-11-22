#include "ObjSkinEngine.h"
#include "Resource.h"
#include "IssCommon.h"

EnumColorScheme g_eColorScheme = COLOR_Green;

struct TypeSkin
{
    UINT uiGreen;
    UINT uiGreenVGA;
    UINT uiBlue;
    UINT uiBlueVGA;
    UINT uiOrange;
    UINT uiOrangeVGA;
    UINT uiPink;
    UINT uiPinkVGA;
    UINT uiPurple;
    UINT uiPurpleVGA;
    UINT uiRed;
    UINT uiRedVGA;
};

TypeSkin g_sSkin[] = 
{
    {IDR_PNG_Arrow_Lan, IDR_PNG_Arrow_LanVGA, IDR_PNG_B_Arrow_Lan, IDR_PNG_B_Arrow_LanVGA, IDR_PNG_O_Arrow_Lan, IDR_PNG_O_Arrow_LanVGA,IDR_PNG_PI_Arrow_Lan, IDR_PNG_PI_Arrow_LanVGA,IDR_PNG_P_Arrow_Lan, IDR_PNG_P_Arrow_LanVGA,IDR_PNG_R_Arrow_Lan, IDR_PNG_R_Arrow_LanVGA},
    {IDR_PNG_Arrow, IDR_PNG_ArrowVGA, IDR_PNG_B_Arrow, IDR_PNG_B_ArrowVGA, IDR_PNG_O_Arrow, IDR_PNG_O_ArrowVGA,IDR_PNG_PI_Arrow, IDR_PNG_PI_ArrowVGA,IDR_PNG_P_Arrow, IDR_PNG_P_ArrowVGA,IDR_PNG_R_Arrow, IDR_PNG_R_ArrowVGA},
    {IDR_PNG_FavoritesImgSelect, IDR_PNG_FavoritesImgSelectVGA, IDR_PNG_B_FavSelect, IDR_PNG_B_FavSelectVGA, IDR_PNG_O_FavSelect, IDR_PNG_O_FavSelectVGA, IDR_PNG_PI_FavSelect, IDR_PNG_PI_FavSelectVGA, IDR_PNG_P_FavSelect, IDR_PNG_P_FavSelectVGA, IDR_PNG_R_FavSelect, IDR_PNG_R_FavSelectVGA},
    {IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_B_Glow, IDR_PNG_B_GlowVGA, IDR_PNG_O_Glow, IDR_PNG_O_GlowVGA, IDR_PNG_PI_Glow, IDR_PNG_PI_GlowVGA, IDR_PNG_P_Glow, IDR_PNG_P_GlowVGA, IDR_PNG_R_Glow, IDR_PNG_R_GlowVGA},
    {IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA, IDR_PNG_B_DetailsBtnGreen, IDR_PNG_B_DetailsBtnGreenVGA,IDR_PNG_O_DetailsBtnGreen, IDR_PNG_O_DetailsBtnGreenVGA,IDR_PNG_PI_DetailsBtnGreen, IDR_PNG_PI_DetailsBtnGreenVGA,IDR_PNG_P_DetailsBtnGreen, IDR_PNG_P_DetailsBtnGreenVGA,IDR_PNG_R_DetailsBtnGreen, IDR_PNG_R_DetailsBtnGreenVGA},
    {IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_B_DetailsBackGreen, IDR_PNG_B_DetailsBackGreenVGA, IDR_PNG_O_DetailsBackGreen, IDR_PNG_O_DetailsBackGreenVGA, IDR_PNG_PI_DetailsBackGreen, IDR_PNG_PI_DetailsBackGreenVGA, IDR_PNG_P_DetailsBackGreen, IDR_PNG_P_DetailsBackGreenVGA, IDR_PNG_R_DetailsBackGreen, IDR_PNG_R_DetailsBackGreenVGA},
    {IDR_PNG_Selector, IDR_PNG_Selector, IDR_PNG_B_Selector, IDR_PNG_B_Selector, IDR_PNG_O_Selector, IDR_PNG_O_Selector,IDR_PNG_PI_Selector, IDR_PNG_PI_Selector,IDR_PNG_P_Selector, IDR_PNG_P_Selector,IDR_PNG_R_Selector, IDR_PNG_R_Selector},
    {IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_B_MenuArray, IDR_PNG_B_MenuArrayVGA, IDR_PNG_O_MenuArray, IDR_PNG_O_MenuArrayVGA, IDR_PNG_PI_MenuArray, IDR_PNG_PI_MenuArrayVGA, IDR_PNG_P_MenuArray, IDR_PNG_P_MenuArrayVGA, IDR_PNG_R_MenuArray, IDR_PNG_R_MenuArrayVGA,},
    {IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_Glow, IDR_PNG_GlowVGA, IDR_PNG_Glow, IDR_PNG_GlowVGA},
};

UINT FindSkin(UINT uiSkin)
{
    for(int i=0; i<sizeof(g_sSkin)/sizeof(TypeSkin); i++)
    {
        if(uiSkin == g_sSkin[i].uiGreen)
        {
            switch(g_eColorScheme)
            {
            case COLOR_Green:
                return (IsVGA()?g_sSkin[i].uiGreenVGA:g_sSkin[i].uiGreen);
                break;
            case COLOR_Blue:
                return (IsVGA()?g_sSkin[i].uiBlueVGA:g_sSkin[i].uiBlue);
                break;
            case COLOR_Orange:
                return (IsVGA()?g_sSkin[i].uiOrangeVGA:g_sSkin[i].uiOrange);
                break;
            case COLOR_Pink:
                return (IsVGA()?g_sSkin[i].uiPinkVGA:g_sSkin[i].uiPink);
                break;
            case COLOR_Purple:
                return (IsVGA()?g_sSkin[i].uiPurpleVGA:g_sSkin[i].uiPurple);
                break;
            case COLOR_Red:
                return (IsVGA()?g_sSkin[i].uiRedVGA:g_sSkin[i].uiRed);
                break;
            default:
                break;  // unknown
            }
        }
    }

    // just return what we got
    return uiSkin;
}

COLORREF FindSelectedColor()
{
    switch(g_eColorScheme)
    {
        case COLOR_Blue:
            return RGB(34,96,139);
            break;
        case COLOR_Orange:
            return RGB(142,94,31);
            break;
        case COLOR_Pink:
            return RGB(142,31,88);
            break;
        case COLOR_Purple:
            return RGB(106,34,139);
            break;
        case COLOR_Red:
            return RGB(160,13,13);
            break;
        default:
            return (COLORREF)0x39AF77;
    }
}
