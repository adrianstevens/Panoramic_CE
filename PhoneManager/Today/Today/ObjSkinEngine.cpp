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
    {IDR_PNG_Selector, IDR_PNG_Selector, IDR_PNG_Selector, IDR_PNG_Selector, IDR_PNG_Selector, IDR_PNG_Selector,IDR_PNG_Selector, IDR_PNG_Selector,IDR_PNG_Selector, IDR_PNG_Selector,IDR_PNG_Selector, IDR_PNG_Selector},
    {IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA, IDR_PNG_MenuArray, IDR_PNG_MenuArrayVGA},
    {IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA, IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA,IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA,IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA,IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA,IDR_PNG_DetailsBtnGreen, IDR_PNG_DetailsBtnGreenVGA},
    {IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA, IDR_PNG_DetailsBackGreen, IDR_PNG_DetailsBackGreenVGA},

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
