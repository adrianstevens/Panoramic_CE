/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssRegistry.h
// Abstract: Class to encapsulate common routines around the Registry
//  
/***************************************************************************************************************/ 

#pragma once

#include "windows.h"

HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               TCHAR* szText, 
               DWORD dwLength,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               DWORD& dwNumber,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               LPBYTE lpData, 
               DWORD& dwSize,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               TCHAR* szText, 
               DWORD dwLength,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               DWORD dwNumber,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               LPBYTE lpData, 
               DWORD dwSize,
               HKEY hKeyStart = HKEY_CURRENT_USER);

HRESULT DeleteValue(TCHAR* szSubKey, 
                    TCHAR* szValue);    
HRESULT RegDeleteKeyNT(HKEY hStartKey, 
                    TCHAR* szKeyName);
HRESULT RegCopyKey( HKEY    hKeySrc, 
                    HKEY    hKeyDest, 
                    TCHAR*    szKeyDestName
                    );
HRESULT RegRenameKey( HKEY        hKeySrc,
                      TCHAR*    szKeySrc,
                      HKEY        hKeyDest,
                      TCHAR*    szKeyDest
                      );




