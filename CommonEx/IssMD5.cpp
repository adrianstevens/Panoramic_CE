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

#include "IssMD5.h"
#include "IssString.h"
#include "IssDebug.h"

#define F1(X, Y, Z) ((Z) ^ ((X) & ((Y) ^ (Z))))
#define F2(X, Y, Z) ((Y) ^ ((Z) & ((X) ^ (Y))))
#define F3(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define F4(X, Y, Z) ((Y) ^ ((X) | ~(Z)))
//yes it actually works
#define ROUND(r, a, b, c, d, F, i, s) { a = b + _rotl(a + F(b, c, d) + reinterpret_cast<const unsigned __int32 *>(m_btBlock)[i] + SIN##r, s); }

//keep things quick
#define SIN1    3614090360ul
#define SIN2    3905402710ul
#define SIN3    606105819ul
#define SIN4    3250441966ul
#define SIN5    4118548399ul
#define SIN6    1200080426ul
#define SIN7    2821735955ul
#define SIN8    4249261313ul
#define SIN9    1770035416ul
#define SIN10   2336552879ul
#define SIN11   4294925233ul
#define SIN12   2304563134ul
#define SIN13   1804603682ul
#define SIN14   4254626195ul
#define SIN15   2792965006ul
#define SIN16   1236535329ul
#define SIN17   4129170786ul
#define SIN18   3225465664ul
#define SIN19   643717713ul
#define SIN20   3921069994ul
#define SIN21   3593408605ul
#define SIN22   38016083ul
#define SIN23   3634488961ul
#define SIN24   3889429448ul
#define SIN25   568446438ul
#define SIN26   3275163606ul
#define SIN27   4107603335ul
#define SIN28   1163531501ul
#define SIN29   2850285829ul
#define SIN30   4243563512ul
#define SIN31   1735328473ul
#define SIN32   2368359562ul
#define SIN33   4294588738ul
#define SIN34   2272392833ul
#define SIN35   1839030562ul
#define SIN36   4259657740ul
#define SIN37   2763975236ul
#define SIN38   1272893353ul
#define SIN39   4139469664ul
#define SIN40   3200236656ul
#define SIN41   681279174ul
#define SIN42   3936430074ul
#define SIN43   3572445317ul
#define SIN44   76029189ul
#define SIN45   3654602809ul
#define SIN46   3873151461ul
#define SIN47   530742520ul
#define SIN48   3299628645ul
#define SIN49   4096336452ul
#define SIN50   1126891415ul
#define SIN51   2878612391ul
#define SIN52   4237533241ul
#define SIN53   1700485571ul
#define SIN54   2399980690ul
#define SIN55   4293915773ul
#define SIN56   2240044497ul
#define SIN57   1873313359ul
#define SIN58   4264355552ul
#define SIN59   2734768916ul
#define SIN60   1309151649ul
#define SIN61   4149444226ul
#define SIN62   3174756917ul
#define SIN63   718787259ul
#define SIN64   3951481745ul

CIssMD5::CIssMD5() 
{
    Reset();
}

CIssMD5::~CIssMD5()
{
}

/********************************************************************
Function:       Process

Arguments:                              

Returns:        void

Comments:       Process the block based on the current state
*********************************************************************/
void CIssMD5::Process(unsigned __int32 iState[4], const byte m_btBlock[64]) 
{
    unsigned __int32 A = iState[0], B = iState[1], C = iState[2], D = iState[3];
    ROUND(1, A, B, C, D, F1, 0, 7); ROUND(2, D, A, B, C, F1, 1, 12);
    ROUND(3, C, D, A, B, F1, 2, 17);ROUND(4, B, C, D, A, F1, 3, 22);
    ROUND(5, A, B, C, D, F1, 4, 7); ROUND(6, D, A, B, C, F1, 5, 12);
    ROUND(7, C, D, A, B, F1, 6, 17);ROUND(8, B, C, D, A, F1, 7, 22);
    ROUND(9, A, B, C, D, F1, 8, 7); ROUND(10, D, A, B, C, F1, 9, 12);
    ROUND(11, C, D, A, B, F1, 10, 17);ROUND(12, B, C, D, A, F1, 11, 22);
    ROUND(13, A, B, C, D, F1, 12, 7);ROUND(14, D, A, B, C, F1, 13, 12);
    ROUND(15, C, D, A, B, F1, 14, 17);ROUND(16, B, C, D, A, F1, 15, 22);
    ROUND(17, A, B, C, D, F2, 1, 5);ROUND(18, D, A, B, C, F2, 6, 9);
    ROUND(19, C, D, A, B, F2, 11, 14);ROUND(20, B, C, D, A, F2, 0, 20);
    ROUND(21, A, B, C, D, F2, 5, 5);ROUND(22, D, A, B, C, F2, 10, 9);
    ROUND(23, C, D, A, B, F2, 15, 14);ROUND(24, B, C, D, A, F2, 4, 20);
    ROUND(25, A, B, C, D, F2, 9, 5);ROUND(26, D, A, B, C, F2, 14, 9);
    ROUND(27, C, D, A, B, F2, 3, 14);ROUND(28, B, C, D, A, F2, 8, 20);
    ROUND(29, A, B, C, D, F2, 13, 5);ROUND(30, D, A, B, C, F2, 2, 9);
    ROUND(31, C, D, A, B, F2, 7, 14);ROUND(32, B, C, D, A, F2, 12, 20);
    ROUND(33, A, B, C, D, F3, 5, 4);ROUND(34, D, A, B, C, F3, 8, 11);
    ROUND(35, C, D, A, B, F3, 11, 16);ROUND(36, B, C, D, A, F3, 14, 23);
    ROUND(37, A, B, C, D, F3, 1, 4);ROUND(38, D, A, B, C, F3, 4, 11);
    ROUND(39, C, D, A, B, F3, 7, 16);ROUND(40, B, C, D, A, F3, 10, 23);
    ROUND(41, A, B, C, D, F3, 13, 4);ROUND(42, D, A, B, C, F3, 0, 11);
    ROUND(43, C, D, A, B, F3, 3, 16);ROUND(44, B, C, D, A, F3, 6, 23);
    ROUND(45, A, B, C, D, F3, 9, 4);ROUND(46, D, A, B, C, F3, 12, 11);
    ROUND(47, C, D, A, B, F3, 15, 16);ROUND(48, B, C, D, A, F3, 2, 23);
    ROUND(49, A, B, C, D, F4, 0, 6);ROUND(50, D, A, B, C, F4, 7, 10);
    ROUND(51, C, D, A, B, F4, 14, 15);ROUND(52, B, C, D, A, F4, 5, 21);
    ROUND(53, A, B, C, D, F4, 12, 6);ROUND(54, D, A, B, C, F4, 3, 10);
    ROUND(55, C, D, A, B, F4, 10, 15);ROUND(56, B, C, D, A, F4, 1, 21);
    ROUND(57, A, B, C, D, F4, 8, 6);ROUND(58, D, A, B, C, F4, 15, 10);
    ROUND(59, C, D, A, B, F4, 6, 15);ROUND(60, B, C, D, A, F4, 13, 21);
    ROUND(61, A, B, C, D, F4, 4, 6);ROUND(62, D, A, B, C, F4, 11, 10);
    ROUND(63, C, D, A, B, F4, 2, 15);ROUND(64, B, C, D, A, F4, 9, 21);
    iState[0] += A; iState[1] += B; iState[2] += C; iState[3] += D;
}

/********************************************************************
Function:       Reset

Arguments:                              

Returns:        void

Comments:       Reset our member variables
*********************************************************************/
HRESULT CIssMD5::Reset() 
{
    m_sizeBufUsed   = 0;
    m_iNbits        = 0;
    return S_OK;
}


HRESULT CIssMD5::Update(const void *data, size_t bytes) 
{
    HRESULT hr = S_OK;
    CBARG(data, _T("data"));
    CBARG(bytes > 0, _T("bytes > 0"));
    if (m_sizeBufUsed != 0) 
    {
        size_t bufremain = 64 - m_sizeBufUsed;
        if (bytes < bufremain) 
        {
            memcpy(m_btBuf + m_sizeBufUsed, data, bytes);
            m_sizeBufUsed += bytes;
            goto Error;
        }
        else 
        {
            memcpy(m_btBuf + m_sizeBufUsed, data, bufremain);
            Process(m_iState, m_btBuf);
            bytes -= bufremain;
            data = reinterpret_cast<const byte *>(data) + bufremain;
        }
    }
    while (bytes >= 64) 
    {
        Process(m_iState, reinterpret_cast<const byte *>(data));
        bytes -= 64;
        data = reinterpret_cast<const byte *>(data) + 64;
        m_iNbits += 512;
    }
    if ((m_sizeBufUsed = bytes) != 0) 
        memcpy(m_btBuf, data, bytes);

Error:
    return hr;
}

//get the MD5 digested value from a byte array
HRESULT CIssMD5::GetDigest(byte digest[16]) 
{
    memcpy(digest, m_iState, 16);
    memset(m_btBuf + m_sizeBufUsed, 0, 64 - m_sizeBufUsed);
    m_btBuf[m_sizeBufUsed] = 0x80;
    if (m_sizeBufUsed < 56) 
    {
        reinterpret_cast<unsigned __int64 *>(m_btBuf)[7] = m_iNbits + m_sizeBufUsed * 8;
        Process(reinterpret_cast<unsigned __int32 *>(digest), m_btBuf);
    }
    else 
    {
        byte extra[64];
        Process(reinterpret_cast<unsigned __int32 *>(digest), m_btBuf);
        memset(extra, 0, 56);
        reinterpret_cast<unsigned __int64 *>(extra)[7] = m_iNbits + m_sizeBufUsed * 8;
        Process(reinterpret_cast<unsigned __int32 *>(digest), extra);
    }
    return S_OK;
}

// note: string has to be of length 33 min
//get the MD5 digested value from a String
HRESULT CIssMD5::GetDigest(TCHAR* szDigest)
{
    HRESULT hr = S_OK;

    byte btDigest[16];
    CIssString* oStr = CIssString::Instance();
    CBARG(szDigest, _T("szDigest"));
    oStr->Empty(szDigest);

    // fill in the bytes here
    hr = GetDigest(btDigest);
    CHR(hr, _T("GetDigest"));

    TCHAR szHex[STRING_SMALL];
    for(int i=0; i<16; i++)
    {
        oStr->Empty(szHex);
        wsprintf(szHex, _T("%.2x"), btDigest[i]);
        szHex[2] = _T('\0');
        oStr->Concatenate(szDigest, szHex);
    }
    szDigest[32] = _T('\0');
Error:
    return hr;
}


