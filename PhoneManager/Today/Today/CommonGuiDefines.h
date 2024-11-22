#pragma once

#include "IssGDIEx.h"

#define REG_KEY_ISS_PATH			_T("SOFTWARE\\Pano\\PhoneGenius")
#define REG_TodayItems_Path			_T("SOFTWARE\\Pano\\PhoneGenius\\Today\\Items")
#define REG_TodayContacts_Path		_T("SOFTWARE\\Pano\\PhoneGenius\\Today\\Contacts")
#define REG_PhoneProfile			_T("PhoneProfile")

#define UND							-1

#define MENU_BTNS_Grad1		RGB(195,195,195)
#define MENU_BTNS_Grad2		RGB(92,93,92)
#define MENU_BTNS_Outline1	RGB(18,19,18)
#define MENU_BTNS_Outline2	RGB(138,140,138)
#define MENU_BTNS_TextColor	RGB(255,255,255)

#define TEXT_HEIGHT			        GetSystemMetrics(SM_CXICON)/2

// utilities
#define CENTERX(rc)                 (rc.left + (rc.right - rc.left)/2)
#define CENTERY(rc)                 (rc.top + (rc.bottom - rc.top)/2)

static void SnapRectToGrid(RECT& rc, int iGridX, int iGridY)
{
	int w = WIDTH(rc);
	int h = HEIGHT(rc);

	if(rc.left % iGridX > iGridX / 2)
		rc.left += iGridX;

	if(rc.top % iGridY > iGridY / 2)
		rc.top += iGridY;

	rc.left		= rc.left/iGridX*iGridX;
	rc.top		= rc.top/iGridY*iGridY;

	rc.right	= rc.left +  w;
	rc.bottom	= rc.top + h;
}

static BOOL IsOverlappedRect(RECT& rc1, RECT& rc2)
{
	int loX = max(rc1.left, rc2.left);
	int hiX = min(rc1.right, rc2.right);
	int loY = max(rc1.top, rc2.top);
	int hiY = min(rc1.bottom, rc2.bottom);

	if(loX >= hiX || loY >= hiY)
		return FALSE;
	else
		return TRUE;
	return FALSE;
}

static BOOL IsContainedIn(RECT& rcBig, RECT& rcSmall)
{
	if(rcSmall.left < rcBig.left)
		return FALSE;
	else if(rcSmall.right > rcBig.right)
		return FALSE;
	else if(rcSmall.top < rcBig.top)
		return FALSE;
	else if(rcSmall.bottom > rcBig.bottom)
		return FALSE;
	else
		return TRUE;
}
