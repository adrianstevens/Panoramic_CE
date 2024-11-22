//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#ifndef __pimstoreex_h__
#define __pimstoreex_h__

#include "pimstore.h"
#ifndef __IPOlItems3_INTERFACE_DEFINED__
#define __IPOlItems3_INTERFACE_DEFINED__

// {58ED74D6-D2C4-415f-8179-5B67E4043614}
DEFINE_GUID(IID_IPOlItems3, 0x58ed74d6, 0xd2c4, 0x415f, 0x81, 0x79, 0x5b, 0x67, 0xe4, 0x4, 0x36, 0x14);
#if defined(__cplusplus) && !defined(CINTERFACE)
interface DECLSPEC_UUID("58ed74d6-d2c4-415f-8179-5b67e4043614")
IPOlItems3 : public IPOlItems2
{
public:
    // Configure the collection to return SIM contacts as well as other contact types
    virtual HRESULT STDMETHODCALLTYPE IncludeSimContacts() = 0;
};
#endif // defined(__cplusplus) && !defined(CINTERFACE)

#endif /* __IPOlItems3_INTERFACE_DEFINED__ */

// Returned when SetProps is attempted for a property not supported by SIM contacts.
#define E_NOTSUPPORTED          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 110)

// Type used for creating SIM contacts
// This would be a part of OlItemType enum in pimstore.h if it were going in the SDK
// This type is useful only for creating a SIM contact
#define olSimContactItem 103

// Flag for FindMatchingContact
// This would be with the other FMCF values in pimstore.h if it were going in the SDK
#define FMCF_INCLUDESIM 0x80000000

// Flag for ChooseContact
// This would be with the other CCF values in pimstore.h if it were going in the SDK
#define CCF_INCLUDESIM 0x8000

// HRESULT values associated with SIM data errors.
#define FACILITY_SIMDATA                0x225
#define SIMDATA_E_NUMBERTOOLONG         (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x01))
#define SIMDATA_E_NAMETOOLONG           (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x02))
#define SIMDATA_E_EMAILTOOLONG          (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x03))
#define SIMDATA_E_NEEDNUMBER            (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x04))
#define SIMDATA_E_GENERIC               (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x05))
#define SIMDATA_E_FULL                  (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_SIMDATA, 0x06))

// Extended Error Information.
// When a FACILITY_SIMDATA HRESULT is ambiguous because it refers to an attribute which has
// multiple instances in a SIM contact, eg a phone number, the extended error code returned by
// GetLastError will be equal to the PIM property ID of the attribute causing the error.

#endif // __pimstoreex_h__
