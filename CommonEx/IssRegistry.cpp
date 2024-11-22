/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssRegistry.cpp
// Abstract: Class to encapsulate common routines around the Registry
//  
/***************************************************************************************************************/ 

#include "IssString.h"
#include "IssRegistry.h"
#include "IssDebug.h"

#ifdef UNDER_CE
#define ISS_KEY_READ    0
#define ISS_KEY_WRITE    0
#else
#define ISS_KEY_READ    KEY_READ
#define ISS_KEY_WRITE    KEY_WRITE
#endif

/********************************************************************

    Function:    GetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                lpData - The Key Value to get

    Outputs:    szText - Text to Get out                

    Returns:    BOOL - TRUE if successfull

    Comments:    This function will retrieve a string from a registry key

*********************************************************************/
HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               LPBYTE lpData, 
               DWORD& dwSize,
               HKEY hKeyStart)  
{  
     HKEY hKey = NULL;  
     DWORD rc;  
     HRESULT hr = S_OK;
       
     //open the Key  
     rc = RegOpenKeyEx(hKeyStart, szSubKey, 0, ISS_KEY_READ, &hKey);  
     CBARG(rc == ERROR_SUCCESS, _T("RegOpenKeyEx"));
       
     //get the string from the key  
     rc = RegQueryValueEx(hKey, szValue, NULL, NULL, lpData, &dwSize);  
     CBARG(rc == ERROR_SUCCESS, _T("RegQueryValueEx failed"));
  
Error:
     if(hKey)
         RegCloseKey(hKey);
     return hr; 
}   

/********************************************************************

    Function:    GetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                szValue - The Key Value to get

    Outputs:    szText - Text to Get out
            
    Returns:    BOOL - TRUE if successful

    Comments:    This function will retrieve a string from a registry key

*********************************************************************/
HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               TCHAR* szText, 
               DWORD dwLength,
               HKEY hKeyStart)
{
    HKEY hKey = NULL;
    DWORD rc;
    HRESULT hr = S_OK;
    
    //open the Key
    rc = RegOpenKeyEx(hKeyStart, szSubKey, 0, ISS_KEY_READ, &hKey);
    CBARG(rc == ERROR_SUCCESS, _T("RegOpenKeyEx failed"));
    
    //get the string from the key
    dwLength *= sizeof(TCHAR);
    rc = RegQueryValueEx(hKey, szValue, NULL, NULL, (LPBYTE)szText, &dwLength);
    CBARG(rc == ERROR_SUCCESS, _T("RegQueryValueEx"));

Error:
    if(hKey)
        RegCloseKey(hKey);
    return hr;
}

/********************************************************************

    Function:    GetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                szValue - The Key Value to get

    Outputs:    dwNumber - DWORD to get out
                
    Returns:    BOOL - TRUE if successful

    Comments:    This function will retrieve an integer from a registry key

*********************************************************************/
HRESULT GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               DWORD& dwNumber,
               HKEY hKeyStart)
{
    HKEY hKey = NULL;
    DWORD rc;
    HRESULT hr = S_OK;
    
    //open the Key
    rc = RegOpenKeyEx(hKeyStart, szSubKey, 0, ISS_KEY_READ, &hKey);
    CBARG(rc == ERROR_SUCCESS, _T("RegOpenKeyEx failed"));
    
    //get the string from the key
    DWORD len = sizeof(dwNumber);
    rc = RegQueryValueEx(hKey, szValue, NULL, NULL, (LPBYTE)&dwNumber, &len);
    CBARG(rc == ERROR_SUCCESS, _T("RegQueryValueEx"));

Error:
    if(hKey)
        RegCloseKey(hKey);
    return hr;
}

/********************************************************************

    Function:    DeleteValue

    Inputs:        szSubKey - Registry Directory where the stuff is stored

    Outputs:    None            

    Returns:    BOOL - TRUE if successful

    Comments:    This function will Delete a key from the registry

*********************************************************************/
HRESULT DeleteValue(TCHAR* szSubKey, 
                    TCHAR* szValue)
{
    HKEY hKey = NULL;
    DWORD rc;
    HRESULT hr = S_OK;
    
    //open the Key
    rc = RegOpenKeyEx(HKEY_CURRENT_USER, szSubKey, 0, 0, &hKey);
    CBARG(rc == ERROR_SUCCESS, _T("RegOpenKeyEx failed"));
    
    //get the string from the key
    rc = RegDeleteValue(hKey, szValue);
    CBARG(rc == ERROR_SUCCESS, _T("RegDeleteValue failed"));

Error:
    if(hKey)
    {
        //close the key
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
    return hr;
}


/********************************************************************

    Function:    SetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                szValue - The Key Value to get
                szText - Text to Set

    Outputs:    None
                

    Returns:    BOOL - TRUE if successful

    Comments:    This function will Set a String to a registry key

*********************************************************************/
HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               LPBYTE lpData, 
               DWORD dwSize,
               HKEY hKeyStart)
{
    HKEY hKey = NULL;
    DWORD rc;
    TCHAR buf[255]; //Note that the Text Cannot be more then 255 characters
    HRESULT hr = S_OK;

    CIssString* oStr = CIssString::Instance();

    //copy the text into the char
    oStr->StringCopy(buf, szValue);
    
    //open the Key or create it if it's not there
    rc = RegCreateKeyEx(hKeyStart, szSubKey, 0, NULL, 0, ISS_KEY_READ | ISS_KEY_WRITE, NULL, &hKey, NULL);
    CBARG(rc == ERROR_SUCCESS, _T("RegCreateKeyEx failed"));
    
    //set the string to the key
    rc = RegSetValueEx(hKey, szValue, 0, REG_BINARY, (LPBYTE)lpData, (dwSize));
    CBARG(rc == ERROR_SUCCESS, _T("RegSetValueEx failed"));

Error:
    if(hKey)
    {
        //close the key
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
    return hr;
}
/********************************************************************

    Function:    SetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                szValue - The Key Value to get
                szText - Text to Set

    Outputs:    None
                

    Returns:    BOOL - TRUE if successful

    Comments:    This function will Set a String to a registry key

*********************************************************************/
HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               TCHAR* szText, 
               DWORD dwLength,
               HKEY hKeyStart)
{
    HKEY hKey = NULL;
    DWORD rc;
    TCHAR buf[255]; //Note that the Text Cannot be more then 255 characters
    HRESULT hr = S_OK;

    CIssString* oStr = CIssString::Instance();

    //copy the text into the char
    oStr->StringCopy(buf, szText);
    
    //open the Key or create it if it's not there
    rc = RegCreateKeyEx(hKeyStart, szSubKey, 0, NULL, 0, ISS_KEY_READ | ISS_KEY_WRITE, NULL, &hKey, NULL);
    CBARG(rc == ERROR_SUCCESS, _T("RegCreateKeyEx failed"));

    
    //set the string to the key
    DWORD len = oStr->GetLength(buf);
    rc = RegSetValueEx(hKey, szValue, 0, REG_SZ, (LPBYTE)buf, (len+1)*sizeof(TCHAR));
    CBARG(rc == ERROR_SUCCESS, _T("RegSetValueEx failed"));

Error:
    if(hKey)
    {
        //close the key
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
    return hr;
}
    
/********************************************************************

    Function:    SetKey

    Inputs:        szSubKey - Registry Directory where the stuff is stored
                iLength - Maximum Length of the String
                szValue - The Key Value to get
                dwNumber - dword to set out

    Outputs:    None                

    Returns:    BOOL - TRUE if successful

    Comments:    This function will Set a dword to a registry key

*********************************************************************/
HRESULT SetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               DWORD dwNumber,
               HKEY hKeyStart)
{
    HKEY hKey = NULL;
    DWORD rc;
    HRESULT hr = S_OK;
    
    //open the Key or create it if it's not there
    rc = RegCreateKeyEx(HKEY_CURRENT_USER, szSubKey, 0, NULL, 0, ISS_KEY_READ | ISS_KEY_WRITE, NULL, &hKey, NULL);
    CBARG(rc == ERROR_SUCCESS, _T("RegCreateKeyEx failed"));
    
    //set the dword to the key
    rc = RegSetValueEx(hKey, szValue, 0, REG_DWORD, (LPBYTE)&dwNumber, sizeof(dwNumber));
    CBARG(rc == ERROR_SUCCESS, _T("RegSetValueEx"));

Error:
    if(hKey)
    {
        //close the key
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
    return hr;
}

/********************************************************************

Function:    RegCopyKey

Inputs:        hKeySrc        - Key to copy from
            hKeyDest    - Key to copy to
            szKeyDestName - name of the key to copy to

Outputs:    None                

Returns:    BOOL - TRUE if successful

Comments:    Copy a full key branch from one given key to another

*********************************************************************/
HRESULT RegCopyKey(    HKEY    hKeySrc, 
                    HKEY    hKeyDest, 
                    TCHAR*    szKeyDestName
                    )
{

    HKEY        hKeySubSrc;
    HKEY        hKeySubDest;
    TCHAR        szValueName[MAX_PATH+1];
    TCHAR        szKeyName[MAX_PATH+1];
    DWORD        dwSize;    
    DWORD        dwVarType;
    DWORD        dwBuffSize;
    LPBYTE        lpBuff            = NULL;
    int            iValEnumIndex    = 0;
    int            iKeyEnumIndex    = 0;
    DWORD        dwDisposition    = 0;
    LONG        Err;
    
    // create target key
    if (RegCreateKeyEx(hKeyDest,
                       szKeyDestName,
                       NULL,NULL,
                       0L,
                       0,NULL,
                       &hKeySubDest,
                       &dwDisposition) != ERROR_SUCCESS)
        return GetLastError();
    
    do 
    {
        do 
        {
            // read value from source key
            Err = ERROR_NOT_ENOUGH_MEMORY;
            dwBuffSize = 1024;
            do 
            {      
                if(lpBuff)
                {
                    delete [] lpBuff;
                    lpBuff = NULL;
                }
                lpBuff = new byte[dwBuffSize];
                dwSize=MAX_PATH+1;
                Err = RegEnumValue(hKeySrc,
                                   iValEnumIndex,
                                   szValueName,
                                   &dwSize,
                                   NULL,
                                   &dwVarType,
                                   lpBuff,
                                   &dwBuffSize);
                
                if ((Err != ERROR_SUCCESS) && 
                    (Err != ERROR_NO_MORE_ITEMS))
                    Err = GetLastError();
            } while (Err == ERROR_NOT_ENOUGH_MEMORY);
            
            // done copying this key
            if (Err == ERROR_NO_MORE_ITEMS)
                break;
            
            // unknown error return
            if (Err != ERROR_SUCCESS)
                goto quit_err;
            
            // write value to target key
            if (RegSetValueEx(hKeySubDest,
                              szValueName,
                              NULL,
                              dwVarType,
                              lpBuff,
                              dwBuffSize) != ERROR_SUCCESS)
                goto quit_get_err;
            
            // read next value
            iValEnumIndex++;
        } while (true);
        
        // free buffer
        if(lpBuff)
        {
            delete [] lpBuff;
            lpBuff = NULL;
        }
        
        // if copying under the same 
        // key avoid endless recursions
        do 
        {
            // enum sub keys
            dwSize=MAX_PATH+1;
            Err = RegEnumKeyEx(hKeySrc,
                               iKeyEnumIndex++,
                               szKeyName,
                               &dwSize,
                               NULL,NULL,
                               NULL,
                               NULL);
        } while ((hKeySrc == hKeyDest)                                  && 
                 !_tcsnicmp(szKeyName,szKeyDestName,_tcsclen(szKeyName)) && 
                 (Err == ERROR_SUCCESS));
        
        // done copying this key        
        if (Err == ERROR_NO_MORE_ITEMS)
            break;
        
        // unknown error return
        if (Err != ERROR_SUCCESS)
            goto quit_get_err;
        
        // open the source subkey
        if (RegOpenKeyEx(hKeySrc,
                         szKeyName,
                         NULL,
                         0,
                         &hKeySubSrc) != ERROR_SUCCESS)
            goto quit_get_err;
        
        // recurs with the subkey
        if ((Err = RegCopyKey(hKeySubSrc, 
                              hKeySubDest,
                              szKeyName)) != ERROR_SUCCESS)
            break;
        
        if (RegCloseKey(hKeySubSrc) != ERROR_SUCCESS)
            goto quit_get_err;
    } while (true);
    
    // normal quit
quit_err:
    if(lpBuff)
    {
        delete [] lpBuff;
        lpBuff = NULL;
    }
    RegFlushKey(hKeySubDest);
    RegCloseKey(hKeySubDest);
    if (Err == ERROR_NO_MORE_ITEMS)
        return ERROR_SUCCESS;    
    else
        return Err;
    
    // abnormal quit
quit_get_err:
    if(lpBuff)
    {
        delete [] lpBuff;
        lpBuff = NULL;
    }
    RegFlushKey(hKeySubDest);
    RegCloseKey(hKeySubDest);
    return GetLastError();
}

/********************************************************************

Function:    RegDeleteKeyNT

Inputs:        hStartKey    - base key to work off of
            szKeyName    - Key to start deleting            

Outputs:    None                

Returns:    DWORD - ERROR_SUCCESS if it worked

Comments:    this is equivalent to the desktop version.  It will recursively go and 
            delete a full key branch
*********************************************************************/
HRESULT RegDeleteKeyNT(HKEY hStartKey, 
                       TCHAR* szKeyName)
{
    DWORD   dwSubKeyLength;
    LPTSTR  pSubKey = NULL;
    TCHAR   szSubKey[MAX_PATH+1]; // (256) this should be dynamic.
    HKEY    hKey = NULL;
    HRESULT hr = S_OK;

    CBARG(hStartKey && szKeyName, _T("RegDeleteKeyNT bad values"));
    
    if( (hr=RegOpenKeyEx(hStartKey,
                            szKeyName,
                            0, 0, 
                            &hKey )) == ERROR_SUCCESS)
    {
        while (hr == ERROR_SUCCESS )
        {
            dwSubKeyLength = MAX_PATH+1;
            hr=RegEnumKeyEx(    hKey,
                                0,       // always index zero
                                szSubKey,
                                &dwSubKeyLength,
                                NULL,
                                NULL,
                                NULL,
                                NULL);
            
            if(hr == (HRESULT)ERROR_NO_MORE_ITEMS)
            {
                // we have to close the pointer to the key first before we can delete it
                if(hKey)
                {
                    RegFlushKey(hKey);
                    RegCloseKey(hKey);
                    hKey = NULL;
                }
                // now delete the key
                hr = RegDeleteKey(hStartKey, szKeyName);
                break;
            }
            else if(hr == ERROR_SUCCESS)
                hr=RegDeleteKeyNT(hKey, szSubKey);
        }
        if(hKey)
        {
            RegFlushKey(hKey);
            RegCloseKey(hKey);
            hKey = NULL;
        }
        
        // Do not save return code because error
        // has already occurred
    }
    
Error:
    if(hKey)
    {
        RegFlushKey(hKey);
        RegCloseKey(hKey);
        hKey = NULL;
    }

    return hr;
}

/********************************************************************

Function:    RegRenameKey

Inputs:        hKeySrc        - Source base key
            szKeySrc    - Source key name
            hKeyDest    - Destination base key
            szKeyDest    - Destination key name

Outputs:    None                

Returns:    DWORD - ERROR_SUCCESS if it worked

Comments:    Rename a full key branch
*********************************************************************/
HRESULT RegRenameKey( HKEY        hKeySrc,
                      TCHAR*    szKeySrc,
                      HKEY        hKeyDest,
                      TCHAR*    szKeyDest
                      )
{
    DWORD    dwRtn;
    HKEY    hKeySubSrc = NULL;
    HRESULT hr = S_OK;

    // quick check to see if the keys are valid
    CBARG(hKeySrc && hKeyDest && szKeySrc && szKeyDest, _T("RegRenameKey arguments"));

    // open a key to the source
    dwRtn    = RegOpenKeyEx(hKeySrc, szKeySrc, 0, 0, &hKeySubSrc);
    CBARG(dwRtn == ERROR_SUCCESS, _T("RegOpenKeyEx failed"));

    // first delete the destination key if it's there
    hr = RegDeleteKeyNT(hKeyDest, szKeyDest);
    CHR(hr, _T("RegDeleteKeyNT failed"));
    
    // copy the contents of the key to the new location
    hr = RegCopyKey(hKeySubSrc, hKeyDest, szKeyDest);
    CHR(hr, _T("RegCopyKey failed"));

    // delete the old key
    RegCloseKey(hKeySubSrc);
    hKeySubSrc = NULL;

    hr = RegDeleteKeyNT(hKeySrc, szKeySrc);
    CHR(hr, _T("RegDeleteKeyNT failed"));

Error:
    if(hKeySubSrc)
        RegCloseKey(hKeySubSrc);
    return hr;
}


