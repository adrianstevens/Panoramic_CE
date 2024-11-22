/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssVector.cpp  
// Abstract: Abstract vector array class.  NOTE: this class will not look after the pointers given to it, this
//             could be added later
//  
/***************************************************************************************************************/ 

#include "IssVector.h"
#include "IssDebug.h"

CIssVectorBuffer::CIssVectorBuffer(int iInitial,int iIncrement)
:m_iDataCount(0)
,m_iDataCapacity(max(0,iInitial))
,m_iDataIncrement(max(8,iIncrement))
,m_pPtr(NULL)
{
    if(m_iDataCapacity)
    {
        m_pPtr = new LPVOID[m_iDataCapacity];
        ASSERT(m_pPtr);
    }
}

CIssVectorBuffer::~CIssVectorBuffer()
{
    SAFE_DELETE_ARRAY(m_pPtr);
}

/********************************************************************
Function:    EnsureCapacity

Arguments:    iElements - number of elements needed in the array    

Returns:    

Comments:    Called as a safety check to make sure there is enough room in the 
            array and also will make the room if there isn't
*********************************************************************/
HRESULT CIssVectorBuffer::EnsureCapacity(int iElements)
{
    HRESULT hr = S_OK;
    LPVOID* lpOld = NULL;

    if (iElements > m_iDataCapacity)
    {
        // save a copy of the old array        
        int iOldSize = m_iDataCapacity;
        if(m_pPtr && m_iDataCapacity)
        {
            lpOld = new LPVOID[m_iDataCapacity];
            CPHR(lpOld, _T("lpOld"));

            memcpy(lpOld, m_pPtr, m_iDataCapacity*sizeof(LPVOID));
        }

        // Determine new storage size
        if (m_iDataIncrement)
            m_iDataCapacity += m_iDataIncrement;
        else
            m_iDataCapacity *= 2;
        if (m_iDataCapacity < iElements)
            m_iDataCapacity = iElements;
        
        // Allocate new storage
        SAFE_DELETE_ARRAY(m_pPtr);

        m_pPtr = new LPVOID[m_iDataCapacity];
        CPHR(m_pPtr, _T("m_pPtr"));

        memset(m_pPtr, 0, sizeof(LPVOID)*m_iDataCapacity);

        if(lpOld && iOldSize)
        {
            memcpy(m_pPtr, lpOld, iOldSize*sizeof(LPVOID));
        }
    }
Error:
    SAFE_DELETE_ARRAY(lpOld);
    return hr;
}

/********************************************************************
Function:    SetSize

Arguments:    iElements - number of elements needed in the array    

Returns:    

Comments:    set the size of the array right now
*********************************************************************/
HRESULT CIssVectorBuffer::SetSize(int iElements)
{
    HRESULT hr = S_OK;
    LPVOID* lpOld = NULL;

    if (0 <= iElements)
    {
        // save a copy of the old array
        if(m_pPtr && m_iDataCapacity)
        {
            lpOld = new LPVOID[m_iDataCapacity];
            CPHR(lpOld, _T("lpOld"));
            memcpy(lpOld, m_pPtr, m_iDataCapacity*sizeof(LPVOID));
        }

        // Allocate new storage
        SAFE_DELETE_ARRAY(m_pPtr);

        if(iElements)
        {
            m_pPtr = new LPVOID[iElements];
            CPHR(m_pPtr, _T("m_pPtr"));
            memset(m_pPtr, 0, sizeof(LPVOID)*iElements);
        }
        m_iDataCapacity = iElements;

        if(lpOld)
        {
            memcpy(m_pPtr, lpOld, m_iDataCapacity*sizeof(LPVOID));
        }
        
        // If array size is increased, then set
        // all the new spaces to NULL (not used)        
        //int i = m_iDataCount;
        //while (i < iElements)
        //    m_pPtr[i++] = NULL;
        
        // If the array size has been reduced, then
        // the array is truncated and the count is
        // equal to the capacity.        
        if (m_iDataCount > m_iDataCapacity)
            m_iDataCount = m_iDataCapacity;
    }
Error:
    SAFE_DELETE_ARRAY(lpOld);
    return hr;
}


/********************************************************************
Function:    IndexOf

Arguments:    ptr        - given an element pointer search the array
            iStart    - starting position to search the array

Returns:    

Comments:    Called as a safety check to make sure there is enough room in the 
array and also will make the room if there isn't
*********************************************************************/
int CIssVectorBuffer::IndexOf(LPVOID ptr,int iStart) const
{
    for (int i=max(0,iStart); i<m_iDataCount; i++)
        if (m_pPtr[i] == ptr)
            return i;
        
        return -1; // not found
}

/********************************************************************
Function:    AddElement

Arguments:    ptr - add an element to the array

Returns:    

Comments:    add an element to the array. goes to end of array
*********************************************************************/
HRESULT CIssVectorBuffer::AddElement(LPVOID ptr)
{
    HRESULT hr = S_OK;
    hr = EnsureCapacity(m_iDataCount+1);
    CHR(hr, _T("EnsureCapacity Failed"));
    m_pPtr[m_iDataCount++] = ptr;
Error:
    return hr;
}
/********************************************************************
Function:    RemoveElement

Arguments:    ptr - element pointer

Returns:    

Comments:    Remove this element by searching the array for it
*********************************************************************/
HRESULT CIssVectorBuffer::RemoveElement(LPVOID ptr)
{
    HRESULT hr = RemoveElementAt(IndexOf(ptr));
    return hr;
}

/********************************************************************
Function:    RemoveElementAt

Arguments:    i - index of which element to remove

Returns:    

Comments:    Remove this element by searching the array for it
*********************************************************************/
HRESULT CIssVectorBuffer::RemoveElementAt(int i)
{
    HRESULT hr = S_OK;
    CBARG((0 <= i && i < m_iDataCount), _T("i is not valid"));

    // bring all the elements down one
    while (++i < m_iDataCount)
        m_pPtr[i-1] = m_pPtr[i];
    m_pPtr[i-1] = NULL;
    
    // set the new size
    hr = SetSize(--m_iDataCount);

Error:
    return hr;
}

/********************************************************************
Function:    RemoveAll

Arguments:    

Returns:    

Comments:    Remove all the elements in the array
*********************************************************************/
HRESULT CIssVectorBuffer::RemoveAll()
{
    HRESULT hr = SetSize(0);
    return hr;
}
/********************************************************************
Function:    Move

Arguments:    

Returns:    

Comments:    Move an element from given index to given index
*********************************************************************/
HRESULT CIssVectorBuffer::Move(int iFrom, int iTo)
{
    HRESULT hr = S_OK;

    if (iFrom == iTo)
        goto Error;
    if (iFrom < 0 || m_iDataCount <= iFrom)
        goto Error;
    if (iTo < 0 || m_iDataCount <= iTo)
        goto Error;
    
    LPVOID   lpTemp = m_pPtr[iFrom], *src, *dst;
    int         iLen;
    
    if (iFrom < iTo)
    {
        dst = m_pPtr + iFrom;
        src = m_pPtr + (iFrom+1);
        iLen = (iTo-iFrom) * sizeof(LPVOID);
    }
    else
    {
        dst = m_pPtr + (iTo+1);
        src = m_pPtr + iTo;
        iLen = (iFrom-iTo) * sizeof(LPVOID);
    }
    
    memmove(dst, src, iLen);
    m_pPtr[iTo] = lpTemp;
    
Error:
    return hr;
}


/********************************************************************
Function:    Sort

Arguments:    compare - given static function to compare two elements in the array

Returns:    

Comments:    uses the quick sort algorithm
*********************************************************************/
HRESULT CIssVectorBuffer::Sort(int (*compare)(const void* , const void* ))
{
    HRESULT hr = S_OK;
    CBARG((m_pPtr && m_iDataCount), _T("m_pPtr && m_iDataCount"));
    qsort(m_pPtr, m_iDataCount, sizeof(LPVOID), compare);

Error:
    return hr;
}

/********************************************************************
Function:    SearchSortedArray

Arguments:    ptrKey    - key to search for
            compare    - compare function to find out what direction the key is in

Returns:    LPVOID - NULL if it didn't find anything

Comments:    Search a sorted array.  This is an optimized way of finding an element in a list.
            NOTE: your array must be sorted before you use this.
*********************************************************************/
LPVOID CIssVectorBuffer::SearchSortedArray(LPVOID ptrKey, int (*compare)(const void* , const void*))
{
    int iLo = 0;
    int iHi = m_iDataCount - 1;
    int iMid;
    int iReturn;

    if(m_iDataCount == 0 || !m_pPtr)
        return NULL;

    while (iLo <= iHi)
    {
        iMid = (iLo + iHi)/2;
        iReturn = (*compare)(ptrKey,GetElement(iMid));
        if(0 == iReturn)
            return GetElement(iMid);
        else if(iReturn < 0)
            iHi = iMid - 1;
        else
            iLo = iMid + 1;

    }

    return NULL;
}

/********************************************************************
Function:    SearchSortedArrayIndex

Arguments:    ptrKey    - key to search for
compare    - compare function to find out what direction the key is in

Returns:    iIndex

Comments:    Search a sorted array.  This is an optimized way of finding an element in a list.
NOTE: your array must be sorted before you use this.
*********************************************************************/
int CIssVectorBuffer::SearchSortedArrayIndex(LPVOID ptrKey, int (*compare)(const void* , const void*))
{
    int iLo = 0;
    int iHi = m_iDataCount - 1;
    int iMid;
    int iReturn;

    if(m_iDataCount == 0 || !m_pPtr)
        return -1;

    while (iLo <= iHi)
    {
        iMid = (iLo + iHi)/2;
        iReturn = (*compare)(ptrKey,GetElement(iMid));
        if(0 == iReturn)
            return iMid;
        else if(iReturn < 0)
            iHi = iMid - 1;
        else
            iLo = iMid + 1;

    }

    return -1;
    
    /*int iLo = 0;
    int iHi = m_iDataCount - 1;
    int iMid;
    int iHalf;
    int iDataCount = m_iDataCount;    
    int        iResult;

    if(m_iDataCount == 0 || !m_pPtr)
        return -1;

    while (iLo <= iHi)
    {
        if (iHalf = iDataCount / 2)
        {
            iMid = iLo + (iDataCount & 1 ? iHalf : (iHalf - 1));
            iResult = (*compare)(ptrKey,GetElement(iMid));
            if (iResult == 0)
                return iMid;
            else if (iResult < 0)
            {
                iHi = iMid - 1;
                iDataCount = iDataCount & 1 ? iHalf : iHalf-1;
            }
            else    
            {
                iLo = iMid + 1;
                iDataCount = iHalf;
            }
        }
        else if (iDataCount)
            return ((*compare)(ptrKey,GetElement(iLo)) ? -1 : iLo);
        else
            break;
    }

    return -1;*/
}

/********************************************************************
Function:    AddSortedElement

Arguments:    ptr        - pointer to add to the list
            compare    - function to see where in the list to add the function

Returns:    

Comments:    Add an element to the list but search for the right spot to put it in first
*********************************************************************/
HRESULT CIssVectorBuffer::AddSortedElement(LPVOID ptr, int (*compare)(const void* , const void*))
{
    HRESULT hr = S_OK;

    if(m_iDataCount == 0)
    {
        return AddElement(ptr);
    }

    /*for (int i=0; i < m_iDataCount; i++)
    {
        LPVOID pTmp = GetElement(i);
        if(!pTmp)
            continue;

        if ((*compare)(ptr, pTmp) < 0)
        {
            hr = AddElement(ptr);
            CHR(hr, _T("AddElement failed"));
            hr = Move(m_iDataCount-1, i);
            CHR(hr, _T("Move failed"));
            return hr;
        }
    }

    hr = AddElement(ptr);*/

    int iIndex = BinarySearch(0, m_iDataCount, ptr, compare);

    hr = AddElement(ptr);
    CHR(hr, _T("CIssVector: AddElement failed"));
    hr = Move(m_iDataCount-1, iIndex);
    CHR(hr, _T("CIssVector: Move failed"));

Error:
    return hr;
}

int CIssVectorBuffer::BinarySearch (int iLow, int iHigh, LPVOID ptr, int (*compare)(const void* , const void*))
{
    int mid;

    if (iLow == iHigh)
        return iLow;

    mid = iLow + ((iHigh - iLow) / 2);

    int iReturn  = (*compare)(ptr, GetElement(mid));

    if (iReturn > 0)
        return BinarySearch (mid + 1, iHigh, ptr, compare);
    else if (iReturn < 0)
        return BinarySearch (iLow, mid, ptr, compare);

    return mid;

}



