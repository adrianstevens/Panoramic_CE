/**********************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (c) Microsoft Corporation. All Rights Reserved.

This code is sample code and is included for illustrative purposes only.
It may not have been designed by the core product team and has not been 
tested to the same standards as product code.  

MODULE:
  PhoneResources.h

ABSTRACT:
  Resource definitions
**********************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
// Phone resources
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef TAPRES_CPROG_BASE
CASSERT(TAPRES_CPROG_BASE == 21000);
#else
#define TAPRES_CPROG_BASE 21000
#endif

#ifndef IDC_STATIC
#define IDC_STATIC -1
#endif

// ID to offset things that are shared between TPC and PPC
#define TAPRES_CPROGEX_SHARED   22500 // TAPRES_CPROGEX_BASE + 0

// ID to offset platform specific stuff from
#define TAPRES_CPROGEX_PLATFORM 22550 // TAPRES_CPROGEX_BASE + 50


// Dialog resources
#define IDD_CPROG_PORT                  22500   // (TAPRES_CPROGEX_SHARED + 0)
#define IDM_PROGRESSVIEW_MENU           22501   // (TAPRES_CPROGEX_SHARED + 1)
#define IDM_CPROG_CALLHISTORY           22502   // (TAPRES_CPROGEX_SHARED + 2)
#define IDM_CPROG_SPEEDDIAL             22503   // (TAPRES_CPROGEX_SHARED + 3)
#define IDM_CPROGPPC_CREATENOTE         22504   // (TAPRES_CPROGEX_SHARED + 4)
#define IDM_CPROG_CONTACTS              22505   // (TAPRES_CPROGEX_SHARED + 5) 
#define IDD_CALERT                      22506   // (TAPRES_CPROGEX_SHARED + 6)
#define IDD_CPROG_LAND                  22507   // (TAPRES_CPROGEX_SHARED + 7)
#define IDD_CPROG_SQRE                  22508   // (TAPRES_CPROGEX_SHARED + 8)
#define IDD_CDIALER_PORT                22509   // (TAPRES_CPROGEX_SHARED + 9)
#define IDD_CDIALER_LAND                22510   // (TAPRES_CPROGEX_SHARED + 10)
#define IDD_CDIALER_SQRE                22511   // (TAPRES_CPROGEX_SHARED + 11)
#define IDD_CPROG_PORT_CDMA             22512   // (TAPRES_CPROGEX_SHARED + 12)
#define IDD_CPROG_LAND_CDMA             22513   // (TAPRES_CPROGEX_SHARED + 13) 
#define IDD_CPROG_SQRE_CDMA             22514   // (TAPRES_CPROGEX_SHARED + 14)
#define IDD_CDIALER_PORT_CDMA           22515   // (TAPRES_CPROGEX_SHARED + 15)
#define IDD_CDIALER_LAND_CDMA           22516   // (TAPRES_CPROGEX_SHARED + 16)
#define IDD_CDIALER_SQRE_CDMA           22517   // (TAPRES_CPROGEX_SHARED + 17)
#define IDM_SK1                         22518   // (TAPRES_CPROGEX_SHARED + 18)
#define IDD_CPROG_PRIVCALL_LAND         22519   // (TAPRES_CPROGEX_SHARED + 19)
#define IDD_CPROG_PRIVCALL_PORT         22520   // (TAPRES_CPROGEX_SHARED + 20)
#define IDD_CPROG_PRIVCALL_SQRE         22521   // (TAPRES_CPROGEX_SHARED + 21)
#define IDM_PRIVCALL_MENU               22522   // (TAPRES_CPROGEX_SHARED + 22)
#define IDD_CALERT_LAND                 22523   // (TAPRES_CPROGEX_SHARED + 23)
#define IDM_CPROG_SAVETOCONTACTS        22524   // (TAPRES_CPROGEX_SHARED + 24)
#define IDD_CPROG_MANUALNET_PORT        22525   // (TAPRES_CPROGEX_SHARED + 25)
#define IDD_CPROG_MANUALNET_LAND        22526   // (TAPRES_CPROGEX_SHARED + 26)
#define IDD_CPROG_MANUALNET_SQRE        22527   // (TAPRES_CPROGEX_SHARED + 27)

#define IDC_ACCUMULATOR    (TAPRES_CPROG_BASE + 2002)
#define IDC_STATUSAREA     (TAPRES_CPROG_BASE + 2003)
#define IDC_CALLALERT      (TAPRES_CPROG_BASE + 2004)

#define IDC_CALLFUNC       (TAPRES_CPROG_BASE + 2006)
#define IDC_ELAPSEDTIME    (TAPRES_CPROG_BASE + 2007)
#define IDC_CONTACTPICTURE (TAPRES_CPROG_BASE + 2008)
#define IDC_STATUSICONS    (TAPRES_CPROG_BASE + 2009)
#define IDC_BRANDING       (TAPRES_CPROG_BASE + 2010)
#define IDC_LINEUI         (TAPRES_CPROG_BASE + 2021)
#define IDC_CALLSTATE      (TAPRES_CPROG_BASE + 2022)
#define IDC_CALERTCONTACT  (TAPRES_CPROG_BASE + 2025)
#define IDC_CALLERLIST     (TAPRES_CPROG_BASE + 2026)
// identifiers in this range are used at phcanvas.h
#define IDC_PROGRESSACCUMULATOR     (TAPRES_CPROG_BASE + 2033)
#define IDC_NETWORK_LIST      (TAPRES_CPROG_BASE + 2034)
#define IDC_PRIVATE        (TAPRES_CPROG_BASE + 2035)
#define IDC_REGISTER       IDC_PRIVATE
#define IDC_HEADINGTEXT    (TAPRES_CPROG_BASE + 2036)
#define IDC_OTHERTEXT      (TAPRES_CPROG_BASE + 2037)
#define IDC_LAYOUTHELPER   (TAPRES_CPROG_BASE + 2038)
#define IDC_CALERTLAYOUTHELPER      (TAPRES_CPROG_BASE + 2039)
#define IDC_CALERTCONTACTPICTURE    (TAPRES_CPROG_BASE + 2040)
// identifiers in this range are used at phcanvas.h
#define IDC_ACCUMLAYOUTHELPER   (TAPRES_CPROG_BASE + 2053)
#define IDC_MANUALNET_HEADINGTEXT  (TAPRES_CPROG_BASE + 2054)
#define IDC_MANUALNET_OTHERTEXT    (TAPRES_CPROG_BASE + 2055)
#define IDC_INVALIDPHONEAREA       (TAPRES_CPROG_BASE + 2056)
#define IDC_INVALIDPHONELAYOUTHELPER (TAPRES_CPROG_BASE + 2057)
#define IDC_PHONELINE_TEXT          (TAPRES_CPROG_BASE + 2058)
#define IDC_INCOMINGCALL_TEXT       (TAPRES_CPROG_BASE + 2059)
#define IDC_ICONSLAYOUTHELPER       (TAPRES_CPROG_BASE + 2060)


