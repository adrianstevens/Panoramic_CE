/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: CIssBase64.cpp 
// Abstract: encodes and decodes text to from a data buffer.  Hides the text and ensure proper transport over HTTP
// 
/***************************************************************************************************************/ 

#include "IssBase64.h"

//our 64 values for encoding
static char charTab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

CIssBase64::CIssBase64(void):
 m_oStr(CIssString::Instance())
{
}

CIssBase64::~CIssBase64(void)
{
}

/********************************************************************
Function:       Encode

Arguments:      szBuffer - char source
                        iStart - start position in szBuffer
                        iLen - length to encode
                        btData - our destination

Returns:        HRESULT S_OK is successful 

Comments:       Encode a char buffer into a byte array
*********************************************************************/
HRESULT CIssBase64::Encode(byte* btData, 
                           int iStart, 
                           int& iLen, 
                           char* szBuffer)
{
    HRESULT hr = S_OK;

    if(szBuffer == NULL)
    {
        szBuffer = new char[iLen*4/3];
        m_oStr->Empty(szBuffer);
    }

    int iEnd = iLen - 3;

    int i = iStart;
    int iTemp;
    int iCurrentIndex = iStart;
    
    while (i <= iEnd)
    {
            iTemp = ((((int) btData[i]) & 0x0ff) << 16)
                            | ((((int) btData[i + 1]) & 0x0ff) << 8)
                            | (((int) btData[i + 2]) & 0x0ff);

            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 18) & 63];
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 12) & 63];
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 6) & 63];
            szBuffer[iCurrentIndex++] = charTab[iTemp & 63];
            i+=3;
    }
    
    if(i == (iStart + iLen - 2))
    {
            iTemp = ((((int) btData[i]) & 0x0ff) << 16)
                      | ((((int) btData[i + 1]) & 255) << 8);


            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 18) & 63];
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 12) & 63];
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 6) & 63];
            szBuffer[iCurrentIndex++] = '=';
    }
    else if(i == (iStart + iLen -1))
    {
            iTemp = (((int) btData[i]) & 0x0ff) << 16;
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 18) & 63];
            szBuffer[iCurrentIndex++] = charTab[(iTemp >> 12) & 63];
            szBuffer[iCurrentIndex++] = '=';
            szBuffer[iCurrentIndex++] = '=';
    }

    szBuffer[iCurrentIndex] = '\0';

    iLen = iCurrentIndex;

    return hr;
}

/********************************************************************
Function:       Decode

Arguments:      int value of character ... -1 if its invalid

Returns:        HRESULT S_OK if successful

Comments:       Decodes szValue from base 64 to normal text
                        Destination is still szValue
*********************************************************************/
int CIssBase64::Decode(TCHAR szValue)
{
    if(szValue >= _T('A') && szValue <= _T('Z'))
        return (int)(szValue - _T('A'));
    else if (szValue >= _T('a') && szValue <= _T('z'))
        return (int)(szValue - _T('a') + 26);
    else if (szValue >= _T('0') && szValue <= _T('9'))
        return (int)(szValue - _T('0') + 26 + 26);
    else
    {
        switch (szValue) 
        {
            case _T('+') :
                    return 62;
                    break;
            case _T('/') :
                    return 63;
                    break;                          
            case _T('=') :
                    return 0;
                    break;
            default :
                    ASSERT(0);
                    return -1; //error return
                    break;
        }
    }
}

/********************************************************************
Function:       Decode

Arguments:      szString - our destination
                        btDataOut - Base64 byte array
                
Returns:        S_OK if succesful 

Comments:       Decodes a byte array to a TCHAR
*********************************************************************/
HRESULT CIssBase64::Decode(TCHAR* szString, 
                           byte* btDataOut, 
                           int iByteLen)
{
    HRESULT hr = S_OK;
    
    if(btDataOut == NULL)
    {
         hr = E_INVALIDARG;
         goto Error;
    }

    int i = 0;
    int iByte = 0;
    int iLen = m_oStr->GetLength(szString);

    while(TRUE)
    {
            while((i < iLen) &&  //memory careful please
                    szString[i] <= _T(' ')) //avoid blanks
                    i++;

            if(i == iLen)
                    break;

            int iTri = (Decode(szString[i]) << 18)
                             + (Decode(szString[i + 1]) << 12)
                             + (Decode(szString[i + 2]) << 6)
                             + (Decode(szString[i + 3]));

            btDataOut[iByte++] = ((iTri>>16) & 255);
            
            if(szString[i+2]==_T('='))
                    break;
            if(iByteLen<iByte)
            {
                hr = E_INVALIDARG;
                goto Error;
            }
            btDataOut[iByte++] = ((iTri>>8) & 255);
            
            if(szString[i+3]==_T('='))
                    break;
            if(iByteLen<iByte)
            {
                    hr = E_INVALIDARG;
         goto Error;
            }
            btDataOut[iByte++] = (iTri & 255);

            i += 4;
    }
Error:
    return hr;
}

/********************************************************************
Function:       Decode

Arguments:      szString - our destination
                btDataOut - Base64 byte array

Returns:        TRUE is successful 

Comments:       Decodes a byte array to a TCHAR
*********************************************************************/
int CIssBase64::Decode(char* szString, 
                       byte* btDataOut,
                       int iByteLen)
{
    if(btDataOut == NULL)
            return 0;

    int i = 0;
    int iByte = 0;
    int iLen = m_oStr->GetLength(szString);

    while(TRUE)
    {
            while((i < iLen) &&  //memory careful please
                    szString[i] <= (' '))   //avoid blanks
                    i++;

            if(i == iLen)
                    break;

            int iTri = (Decode((TCHAR)szString[i]) << 18)
                    + (Decode((TCHAR)szString[i + 1]) << 12)
                    + (Decode((TCHAR)szString[i + 2]) << 6)
                    + (Decode((TCHAR)szString[i + 3]));

            btDataOut[iByte++] = ((iTri>>16) & 255);

            if(szString[i+2]==('='))
                    break;
            if(iByteLen<iByte)
                    return FALSE;
            btDataOut[iByte++] = ((iTri>>8) & 255);

            if(szString[i+3]==('='))
                    break;
            if(iByteLen<iByte)
                    return FALSE;
            btDataOut[iByte++] = (iTri & 255);

            i += 4;
    }
    return i;
}
