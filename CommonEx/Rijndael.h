/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: Rijndael.cpp 
// Abstract: Rijndael Encription/Decryption.  This implimentation is working 100% under Windows CE and has 
// been tested to give identical results of other standard implemintations.
// 
// Rijndael (pronounced Reindaal) is a block cipher, designed by Joan Daemen and Vincent Rijmen as a candidate algorithm for the AES.
// The cipher has a variable block length and key length. The authors currently specify how to use keys with a length
// of 128, 192, or 256 bits to encrypt blocks with al length of 128, 192 or 256 bits (all nine combinations of
// key length and block length are possible). Both block length and key length can be extended very easily to
// multiples of 32 bits.
/***************************************************************************************************************/ 

#pragma once

#include "windows.h"

#define AES_BLOCK_SIZE  16
#define MAX_SIZE        256

enum EncryptionMode
{ 
    ECB=0, 
    CBC=1, 
    CFB=2, 
};

#define DEFAULT_BLOCK_SIZE  16
#define MAX_BLOCK_SIZE      32
#define MAX_ROUNDS          14
#define MAX_KC              8
#define MAX_BC              8

class CRijndael
{
public:

private:


public:
    CRijndael();
    virtual ~CRijndael();

    HRESULT     MakeKey(char const* szKey, char const* szChain, int iKeyLen = DEFAULT_BLOCK_SIZE, int iBlockSize = DEFAULT_BLOCK_SIZE);
    HRESULT     EncryptBlock(char const* szIn, char* szResult);
    HRESULT     DecryptBlock(char const* szIn, char* szResult);
    HRESULT     Encrypt(char const* szIn, char* szResult, size_t szSize, EncryptionMode eMode = ECB);
    HRESULT     Decrypt(char const* szIn, char* szResult, size_t szSize, EncryptionMode eMode = ECB);
    int         GetKeyLength(); // Get Key Length
    int         GetBlockSize(); // Block Size
    int         GetRounds();    // Number of Rounds
    HRESULT     ResetChain();

private:
    static int  Mul(int iA, int iB);
    static int  Mul4(int iA, char szB[]);
    void        Xor(char* szBuff, char const* szChain);

    void        DefEncryptBlock(char const* szIn, char* szResult);
    void        DefDecryptBlock(char const* szIn, char* szResult);


public:
    static char const* sm_chain0;

private:
    static const int  sm_alog[MAX_SIZE];
    static const int  sm_log[MAX_SIZE];
    static const char sm_S[MAX_SIZE];
    static const char sm_Si[MAX_SIZE];
    static const int  sm_T1[MAX_SIZE];
    static const int  sm_T2[MAX_SIZE];
    static const int  sm_T3[MAX_SIZE];
    static const int  sm_T4[MAX_SIZE];
    static const int  sm_T5[MAX_SIZE];
    static const int  sm_T6[MAX_SIZE];
    static const int  sm_T7[MAX_SIZE];
    static const int  sm_T8[MAX_SIZE];
    static const int  sm_U1[MAX_SIZE];
    static const int  sm_U2[MAX_SIZE];
    static const int  sm_U3[MAX_SIZE];
    static const int  sm_U4[MAX_SIZE];
    static const char sm_rcon[30];
    static const int  sm_shifts[3][4][2];

    //Error Messages
    static char const* sm_szErrorMsg1;
    static char const* sm_szErrorMsg2;

    //Key Initialization Flag
    BOOL m_bKeyInit;

    //Encryption (m_Ke) round key
    int m_Ke[MAX_ROUNDS+1][MAX_BC];

    //Decryption (m_Kd) round key

    int m_iKd[MAX_ROUNDS+1][MAX_BC];

    int m_iKeyLen;
    int     m_iBlockSize;
    int m_iRounds;

    //Chain Block
    char m_chain0[MAX_BLOCK_SIZE];
    char m_chain[MAX_BLOCK_SIZE];

    int m_itk[MAX_KC];
    int m_iA[MAX_BC];
    int m_iT[MAX_BC];
};