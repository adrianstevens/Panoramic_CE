/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: CIssBase64.h 
// Abstract: encodes and decodes text to from a data buffer.  Hides the text and ensure proper transport over HTTP
// 
/***************************************************************************************************************/ 

#pragma once

#include "IssString.h"
#include "IssDebug.h"

class CIssBase64
{
public:
    CIssBase64(void);
    ~CIssBase64(void);

    HRESULT         Encode(byte* btData, 
                            int iStart, 
                            int& iLen, 
                            char* szBuffer);

    int             Decode(TCHAR szValue);
    int             Decode(char* szStringIn, 
                            byte* btResults, 
                            int iByteLen);
    HRESULT         Decode(TCHAR* szStringIn, 
                            byte* btResults, 
                            int iByteLen);

private:

private://variables
        CIssString*     m_oStr;

};
