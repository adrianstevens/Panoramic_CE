/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: CIssMD5.h 
// Abstract: Windows CE implimentation of MD5 encryption/decryption
// 
/***************************************************************************************************************/ 

#pragma once

#include "windows.h"

class CIssMD5 
{
public:
    CIssMD5();
    ~CIssMD5();
    HRESULT Reset();
    HRESULT Update(const void *data, size_t sizeBytes);
    HRESULT GetDigest(byte btDigest[16]);
    HRESULT GetDigest(TCHAR* szDigest);

private:
    static void Process(unsigned __int32 iState[4], const byte btBlock[64]);

private:
    mutable byte            m_btBuf[64];
    size_t                  m_sizeBufUsed;
    unsigned __int64        m_iNbits;
    unsigned __int32        m_iState[4];
};

