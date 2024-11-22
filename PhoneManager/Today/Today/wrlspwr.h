/* 
   Copyright (C) 2004-2006 Teksoft SRL Romania
   
   THIS SOURCE CODE IS PROPERTY OF Teksoft SRL Romania.
   http://www.teksoftco.com
   All rights reserved.

Description:
============
RadioDevices sample offers the possibility of controlling the Radio Devices (Phone,Bluetooth,WiFi),
on devices running Windows CE for Pocket PC 4.0 and Windows CE for Pocket PC 5.0

License:
========
Redistribution and use in source and binary forms, with or without modification, are permitted 
provided that the following conditions are met (1,2,3): 
1) Redistribution of source code is not allowed in any way, nor is allowed selling the source code 
and this library to other parties.
2) Redistribution in binary form must reproduce the above copyright notice, this list of conditions 
and the following disclaimer in the documentation and/or other materials provided with the 
distribution. 
3) Neither the name of Teksoft SRL Romania or the names of contributors may be used to endorse 
or promote products derived from this software without specific prior written permission. 

This software is provided "AS IS," without a warranty of any kind. ALL EXPRESS OR IMPLIED 
CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. Teksoft SRL Romania 
("Teksoft") AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT 
OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL Teksoft OR 
ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL, 
CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF 
LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF Teksoft HAS BEEN 
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
*/

#pragma once

// Types of radio device
typedef enum _RADIODEVTYPE
{
    RADIODEVICES_MANAGED = 1,
    RADIODEVICES_PHONE,
    RADIODEVICES_BLUETOOTH,
} RADIODEVTYPE;

// whether to save before or after changing state
typedef enum _SAVEACTION
{
    RADIODEVICES_DONT_SAVE = 0,
    RADIODEVICES_PRE_SAVE,
    RADIODEVICES_POST_SAVE,
} SAVEACTION;

// Details of radio devices
struct RDD 
{
    RDD() : pszDeviceName(NULL), pNext(NULL), pszDisplayName(NULL) {}
    ~RDD() { LocalFree(pszDeviceName); LocalFree(pszDisplayName); }
    LPTSTR   pszDeviceName;  // Device name for registry setting.
    LPTSTR   pszDisplayName; // Name to show the world
    DWORD    dwState;        // ON/off/[Discoverable for BT]
    DWORD    dwDesired;      // desired state - used for setting registry etc.
    RADIODEVTYPE    DeviceType;         // Managed, phone, BT etc.
    RDD * pNext;    // Next device in list
}; //radio device details



