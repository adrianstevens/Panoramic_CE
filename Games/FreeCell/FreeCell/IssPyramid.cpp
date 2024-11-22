#include "IssPyramid.h"

CIssPyramid::CIssPyramid(void)
{
}

CIssPyramid::~CIssPyramid(void)
{
}

void CIssPyramid::NewGame()
{

}

BOOL CIssPyramid::IsCardFree(int iRow, int iIndex)
{   //zero based
    if(iRow < 0 || iRow > 6)
        return FALSE;

    if(iRow == 1)
        return TRUE;
     
    if(iIndex > 1)
        if(m_btCards[iRow-1][iIndex-1] != CARD_BLANK)
            return FALSE;
        





    return FALSE;
}