#pragma once

#include "IssGDIDraw.h"

#ifndef UNDER_CE 
#define VK_TSOFT1 VK_F1 
#define VK_TSOFT2 VK_F2 

#define VK_TTALK VK_F3
#define VK_TEND VK_F4

#define BTN_EDGE_INDENT			(GetButtonIndent())
#define BTN_TOP_INDENT			(GetSystemMetrics(SM_CXICON)/16) //2
#define BTN_SPACING				(GetButtonSpacing())
#define WINDOW_IDENT			(GetSystemMetrics(SM_CXICON)/10) //3

#elif WIN32_PLATFORM_WFSP
#define BTN_EDGE_INDENT			GetSystemMetrics(SM_CXICON)/14 //3
#define BTN_TOP_INDENT			GetSystemMetrics(SM_CXICON)/22 //2
#define BTN_SPACING				GetSystemMetrics(SM_CXICON)/44 //1
#define WINDOW_IDENT			GetSystemMetrics(SM_CXICON)/14 //3

#else
#define BTN_EDGE_INDENT			(GetButtonIndent())
#define BTN_TOP_INDENT			(GetSystemMetrics(SM_CXICON)/16) //2
#define BTN_SPACING				(GetButtonSpacing())
#define WINDOW_IDENT			(GetSystemMetrics(SM_CXICON)/10) //3
#endif

#ifdef UNDER_CE
#define DISPLAY_TEXT_SIZE       (GetSystemMetrics(SM_CXICON)*5/6)
#define EXPONENT_TEXT_SIZE      (GetSystemMetrics(SM_CXICON)*7/10)
#define DISPLAY_TOP_TEXT        (GetSystemMetrics(SM_CXICON)*4/9)
#define DISPLAY_TOP_TEXT_SM     (GetSystemMetrics(SM_CXICON)/3+1)

#define TEXT_INDENT				(GetSystemMetrics(SM_CXICON)/4) //4iish

#define DISPLAY_TALL        GetSystemMetrics(SM_CXICON)*9/5
#define DISPLAY_HEIGHT		GetSystemMetrics(SM_CXICON)*7/5 //I think we'll use this size for the buttons too ... just make em square or slightly wider than high
#define DISPLAY_INDENT		GetSystemMetrics(SM_CXICON)/16

//I'm sure these will be used on currency and time too
#define BTN_WIDTH_RATIO		(5/4)
#define UNIT_BTN_HEIGHT		DISPLAY_HEIGHT
#define UNIT_BTN_WIDTH		(DISPLAY_HEIGHT*BTN_WIDTH_RATIO)
#define FUNC_BTN_WIDTH      (DISPLAY_HEIGHT*BTN_WIDTH_RATIO)

#else //PC baby
#define WINDOW_WIDTH            500     
#define WINDOW_HEIGHT           320


#define DISPLAY_TEXT_SIZE       (WINDOW_HEIGHT/10)
#define EXPONENT_TEXT_SIZE      (WINDOW_HEIGHT/12)
#define DISPLAY_TOP_TEXT        (WINDOW_HEIGHT/16)
#define DISPLAY_TOP_TEXT_SM     (WINDOW_HEIGHT/20)

#define TEXT_INDENT				(WINDOW_HEIGHT/20) //4iish

#define DISPLAY_TALL        (WINDOW_HEIGHT/7)
#define DISPLAY_HEIGHT		(WINDOW_HEIGHT/4)
#define DISPLAY_INDENT		(WINDOW_HEIGHT/64)

//I'm sure these will be used on currency and time too
#define BTN_WIDTH_RATIO		(5/4)
#define UNIT_BTN_HEIGHT		DISPLAY_HEIGHT
#define UNIT_BTN_WIDTH		(DISPLAY_HEIGHT*BTN_WIDTH_RATIO)
#define FUNC_BTN_WIDTH      (DISPLAY_HEIGHT*BTN_WIDTH_RATIO)




#endif




#ifdef WIN32_PLATFORM_WFSP
#define UNIT_BTN_WIDTH_LRG (3*UNIT_BTN_WIDTH/2)
#else
#define UNIT_BTN_WIDTH_LRG (2*UNIT_BTN_WIDTH)
#endif


#define REG_KEY _T("SOFTWARE\\Pano\\CalcProLite") 

//#define TOP_TEXT_COLOR_1 RGB(170,109,82)
//#define TOP_TEXT_COLOR_2 RGB(82,113,170)
//#define TOP_TEXT_COLOR_3 RGB(82,170,94)

#define TEXT_HEIGHT_RATIO		5/7
#define TEXT_LONG_RATIO			4/7
#define TEXT_LONG_RATIO_LAND    1/2 //yuck

//damn square devices
#define TEXT_HEIGHT_RATIO_SQ	8/9
#define TEXT_LONG_RATIO_SQ		6/9

#define WM_CALCENGINE WM_USER + 123

#define MAX_MAIN_BUTTONS 25


#define IDMENU_Unit_From		1000
#define IDMENU_Unit_To			1100
#define IDMENU_Unit_Conv		1200

#define IDMENU_Cur_From			2000
#define IDMENU_Cur_To			2200

#define IDMENU_Graph_Func       2500

#define IDMENU_Const1           2600
#define IDMENU_Const2           2800
#define IDMENU_Const3           3000
#define IDMENU_Const4           3200
#define IDMENU_ConstEnd         3400

#define IDMENU_ConstFav         4000

extern BOOL                 gbGraphPoints;
extern BOOL                 gbGraphFast;
extern BOOL                 gbTVMEndPay;



static int GetButtonSpacing()
{
    switch(GetSystemMetrics(SM_CXSCREEN))
    {
    case 320:
        if(GetSystemMetrics(SM_CYSCREEN) == 320)
            return 2;
    case 240:
    case 400:    
        return 1;
        break;
    case 480:
    case 640:
    case 800:
        return 2;
    default:
        return GetSystemMetrics(SM_CXICON)/32;
        break;
    }

}

static int GetButtonIndent()
{
    switch(GetSystemMetrics(SM_CXSCREEN))
    {
    case 320:
        if(GetSystemMetrics(SM_CYSCREEN) == 320)
            return 1;
    case 400:
    case 240:
        return 3;
    	break;
    case 480:
    case 640:
    case 800:
        return 6;
    default:
        return GetSystemMetrics(SM_CXICON)/10;
        break;
    }
}
