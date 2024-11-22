/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: CISSCrcCalc.cpp 
// Abstract: Calculate CRC values
// 
/***************************************************************************************************************/ 

#include "windows.h"
#include "ISSCrcCalc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CISSCrcCalc::CISSCrcCalc():
 m_szBuffer(NULL)
{
    CrcGen();
}

CISSCrcCalc::~CISSCrcCalc()
{
    Destroy();
}


/********************************************************************
Function:       Destroy

Arguments:                              

Returns:        TRUE is successful 

Comments:       Clean's up our buffer
*********************************************************************/
HRESULT CISSCrcCalc::Destroy()
{
    if(m_szBuffer)
    {
        delete [] m_szBuffer;
        m_szBuffer = NULL;
    }
    return S_OK;
}

/********************************************************************
Function:       CrcGen

Arguments:                              

Returns:        

Comments:       Generate our CRC value
*********************************************************************/
// fills the CRC Table with valid codes
void CISSCrcCalc::CrcGen()
{
    unsigned long   ulCrc, ulPoly;
    int     i, j;

    ulPoly = 0xEDB88320L;

    for (i=0; i<256; i++)
    {
            ulCrc = i;
    
            for (j=8; j>0; j--) 
            {
                    if (ulCrc & 1) 
                    {
                            ulCrc = (ulCrc >> 1) ^ ulPoly;
                    } 
                    else 
                    {
                            ulCrc >>= 1;
                    }
            }
            
            m_crcTable[i] = ulCrc;
    }
}

/********************************************************************
Function:       GetCRC

Arguments:      szBuffer - to calc our crc value

Returns:        the CRC value based off of szBuffer 

Comments:       pretty standard stuff
*********************************************************************/
unsigned long CISSCrcCalc::GetCRC(TCHAR* szBuffer)
{
    if(!szBuffer)
    {
            // break out, we don't want to get here.
            ASSERT(0);
            return 0;
    }
    
    register unsigned long crc;
    int c;
    
    crc = 0xFFFFFFFF;
    
    Destroy();
    int iSize = wcslen(szBuffer);
    
    
    for(int i = 0; i < iSize; i++)
    {
            c = szBuffer[i];
            crc = ((crc>>8) & 0x00FFFFFF) ^ m_crcTable[ (crc^c) & 0xFF ];
    }
    
    return( crc^0xFFFFFFFF );
}

/********************************************************************
Function:       GetCRC

Arguments:      fp - to calc our crc value

Returns:        the CRC value based off of fp 

Comments:       pretty standard stuff
*********************************************************************/
unsigned long CISSCrcCalc::GetCRC(FILE* fp)
{
    register unsigned long crc;
    int c;

    crc = 0xFFFFFFFF;

    fseek(fp,0L,SEEK_END);
    int iSize = ftell(fp);

    Destroy();
    m_szBuffer = new CHAR[iSize+1];

    if(m_szBuffer == NULL)
    {
        return 0L;
    }

    fseek(fp,0L,SEEK_SET);
    fread(m_szBuffer,sizeof(char),iSize,fp);

    for(int i = 0; i < iSize; i++)
    {
            c = m_szBuffer[i];
            crc = ((crc>>8) & 0x00FFFFFF) ^ m_crcTable[ (crc^c) & 0xFF ];
    }
    
    return( crc^0xFFFFFFFF );
}