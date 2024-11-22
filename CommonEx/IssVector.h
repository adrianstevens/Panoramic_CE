/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssVector.h
// Abstract: Abstract vector array class 
//  
/***************************************************************************************************************/ 

#pragma once

#ifdef _AFXDLL
#include "stdafx.h"
#else
#include "windows.h"
#endif


/************************************************************************
CIssVectorBuffer is used so I can test out the class.  If it was all in the 
template class I would never know if it works until i used the given function
/************************************************************************/
class CIssVectorBuffer 
{
public:    // functions
    CIssVectorBuffer(int iInitial=0,int iIncrement=8);
    ~CIssVectorBuffer();

    HRESULT     SetSize(int iSize);    
    int         GetSize(){return m_iDataCount;};
    int         GetCapacity(){return m_iDataCapacity;};
    int         IndexOf(LPVOID ptr,int iStart=0)          const;
    BOOL        Contains(LPVOID ptr){return (IndexOf(ptr)>=0?TRUE:FALSE);}
    BOOL        IsEmpty(){return (m_iDataCount == 0 ? TRUE : FALSE);};
    
    LPVOID      GetElement(int i){return ((0 <= i && i < m_iDataCapacity)?m_pPtr[i]:NULL);};
    HRESULT     AddElement(LPVOID ptr);
    HRESULT     RemoveElement(LPVOID ptr);
    HRESULT     RemoveElementAt(int i);
    HRESULT     RemoveAll();
    HRESULT     Move(int iFrom, int iTo);
    
    HRESULT     Sort(int (*compare)(const void* , const void*));
    LPVOID      SearchSortedArray(LPVOID ptr, int (*compare)(const void* , const void*));
    int         SearchSortedArrayIndex(LPVOID ptrKey, int (*compare)(const void* , const void*));
    HRESULT     AddSortedElement(LPVOID ptr, int (*compare)(const void* , const void*));
    
private:    // functions
    HRESULT     EnsureCapacity(int iElements);
    int         BinarySearch (int iLow, int iHigh, LPVOID ptr, int (*compare)(const void* , const void*));


protected:    // variables
    LPVOID*     m_pPtr;
    int         m_iDataCount;
    int         m_iDataCapacity;
    int         m_iDataIncrement;
};



template<class T> class CIssVector : public CIssVectorBuffer
{
public:
  T*       ElementAt(int i)       { return (T*)GetElement(i);           }
  T*       operator [] (int i)    { return (T*)GetElement(i);           }
  T*       FirstElement()         { return (T*)GetElement(0);           }
  T*       LastElement()          { return (T*)GetElement(m_iDataCount-1);}
  void     SetElementAt(T * t,int i)    { m_pPtr[i] = t;     }
};


