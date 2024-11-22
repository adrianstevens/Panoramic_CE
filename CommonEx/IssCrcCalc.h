/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: CISSCrcCalc.h 
// Abstract: Calculate CRC values
// 
/***************************************************************************************************************/ 

#pragma once

#include <stdio.h>
#include "issdebug.h"

class CISSCrcCalc  
{
public:
    CISSCrcCalc();
    virtual ~CISSCrcCalc();
    
    HRESULT         Destroy();
    
    unsigned long   GetCRC(FILE* fp);
    unsigned long   GetCRC(TCHAR* szBuffer);

private:
    void            CrcGen();

private:
    unsigned long   m_crcTable[256];

    CHAR*           m_szBuffer;
};

