#pragma once

#define MENU_BTNS_Grad1		RGB(195,195,195)
#define MENU_BTNS_Grad2		RGB(92,93,92)
#define MENU_BTNS_Outline1	RGB(18,19,18)
#define MENU_BTNS_Outline2	RGB(138,140,138)
#define MENU_BTNS_TextColor	RGB(255,255,255)

#define MENU_BTNS_Style     BTN_Shine3

/* //Baby Blue
#define MENU_BTNS_Grad1		RGB(204,225,255)
#define MENU_BTNS_Grad2		RGB(145,173,209)
#define MENU_BTNS_Outline1	RGB(13,50,90)
#define MENU_BTNS_Outline2	RGB(255,255,255)
#define MENU_BTNS_TextColor	RGB(13,50,90)*/

#define TEXT_HEIGHT			GetSystemMetrics(SM_CXICON)/2

#define WM_InvalTitle	WM_USER + 300


static COLORREF GetListBGColor(int iIndex)
{
	if(iIndex > 1)
		return RGB(255,255,255);

	COLORREF rgbColor = GetSysColor(COLOR_BTNFACE);

	int R = (GetRValue(rgbColor) + 255)/2;
	int G = (GetGValue(rgbColor) + 255)/2;
	int B = (GetBValue(rgbColor) + 255)/2;

	if(iIndex == 1)
		return RGB(R,G,B);		

	return RGB(R*15/16,G*15/16,B*15/16);

}
