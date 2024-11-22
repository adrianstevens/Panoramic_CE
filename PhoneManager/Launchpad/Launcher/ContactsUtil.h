#pragma once


// is the small rect contained fully within the big rect?
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

