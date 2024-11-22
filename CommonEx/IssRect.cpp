/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssRect.cpp
// Abstract: Class to encapsulate common routines around RECT structure
//  
/***************************************************************************************************************/ 
#include "IssRect.h"

CIssRect::CIssRect(void)
:left(0)
,right(0)
,top(0)
,bottom(0)
{
}

CIssRect::CIssRect(CIssRect& rc)
{
    Set(rc);
}

CIssRect::CIssRect(RECT& rc)
{
    Set(rc);
}

CIssRect::~CIssRect(void)
{}

CIssRect::operator RECT()
{
    RECT rc = {left,top,right,bottom};
    return rc;
}

BOOL CIssRect::operator==(RECT& rc)
{
    if(left        == rc.left    &&
       right    == rc.right &&
       top        == rc.top    &&
       bottom    == rc.bottom)
       return TRUE;
    else
        return FALSE; 
}

BOOL CIssRect::operator==(CIssRect& rc)
{
    if (left    == rc.left  &&
        right    == rc.right &&
        top        == rc.top    &&
        bottom    == rc.bottom)
        return TRUE;
    else
        return FALSE; 
}

/********************************************************************
Function:    Set

Arguments:        

Returns:    

Comments:    Set our RECT values
*********************************************************************/
HRESULT CIssRect::Set(int iLeft, 
                      int iTop, 
                      int iRight, 
                      int iBottom)
{
    left    = iLeft;
    right    = iRight;
    top        = iTop;
    bottom    = iBottom;
    return S_OK;
}

/********************************************************************
Function:    Set

Arguments:        

Returns:    

Comments:    Set our RECT values
*********************************************************************/
HRESULT CIssRect::Set(CIssRect& rc)
{
    left    = rc.left;
    right    = rc.right;
    top        = rc.top;
    bottom    = rc.bottom;
    return S_OK;
}

/********************************************************************
Function:    Set

Arguments:        

Returns:    

Comments:    Set our RECT values
*********************************************************************/
HRESULT CIssRect::Set(RECT& rc)
{
    left    = rc.left;
    right    = rc.right;
    top        = rc.top;
    bottom    = rc.bottom;
    return S_OK;
}

/********************************************************************
Function:    Get

Arguments:        

Returns:    RECT structure

Comments:    Get the RECT out of the class
*********************************************************************/
RECT CIssRect::Get()
{
    RECT rc = {left,top,right,bottom};
    return rc;
}

/********************************************************************
Function:    Stretch

Arguments:    iStretch - value to stretch by

Returns:    

Comments:    Stretch our rect by given amount on all sides. NOTE: can be negative too
*********************************************************************/
HRESULT CIssRect::Stretch(int iStretch)
{
    left        -= iStretch;
    right        += iStretch;
    top            -= iStretch;
    bottom        += iStretch;
    return S_OK;
}

/********************************************************************
Function:    Translate

Arguments:    iX    - translate in x direction
            iY    - translate in y direction

Returns:    

Comments:    translate the RECT by given amount.  NOTE negative numbers accepted
*********************************************************************/
HRESULT CIssRect::Translate(int iX, 
                            int iY)
{
    left        += iX;
    right        += iX;
    top            += iY;
    bottom        += iY;
    return S_OK;
}

/********************************************************************
Function:    Concatenate

Arguments:    rc    - RECT to add to our own rect

Returns:    

Comments:    join the current RECT with this new given rect
*********************************************************************/
HRESULT CIssRect::Concatenate(RECT rc)
{
    left     = min(rc.left, left); 
    top      = min(rc.top, top); 
    right    = max(rc.right, right); 
    bottom   = max(rc.bottom, bottom); 
    return S_OK;
}

/********************************************************************
Function:    ZeroBase

Arguments:        

Returns:    

Comments:    Zero base the rect
*********************************************************************/
HRESULT CIssRect::ZeroBase()
{
    right  -= left;
    bottom -= top;
    left    = top    = 0;
    return S_OK;
}

/********************************************************************
Function:    PtInRect

Arguments:        

Returns:    TRUE if POINT is in RECT

Comments:    Is this POINT in our RECT?
*********************************************************************/
BOOL CIssRect::PtInRect(POINT& pt)
{
    RECT rc = Get();
    return ::PtInRect(&rc, pt);
}

/********************************************************************
Function:    RectInRect

Arguments:        

Returns:    TRUE if RECT is in RECT

Comments:    Is this RECT in our RECT?
*********************************************************************/
BOOL CIssRect::RectInRect(RECT& rc)
{
    if( rc.left     >= rc.left    &&
        rc.top        >= rc.top    &&
        rc.right    <= rc.right    &&
        rc.bottom    <= rc.bottom    )
        return TRUE;
    else
        return FALSE;
}

/********************************************************************
Function:    RectInRect

Arguments:        

Returns:    TRUE if RECT is in RECT

Comments:    Is this RECT in our RECT?
*********************************************************************/
BOOL CIssRect::RectInRect(CIssRect& rc)
{
    if(rc.left        >= rc.left    &&
       rc.top        >= rc.top        &&
       rc.right        <= rc.right    &&
       rc.bottom    <= rc.bottom    )
       return TRUE;
    else
        return FALSE;
}

