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
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
#pragma once

// Standard headers
//#define _SHELL32_
//#define _SHLWAPI_
//#define _OLE32_
//#pragma warning (disable:4100) // 'identifier' : unreferenced formal parameter
//#pragma warning (disable:4514) // 'function' : unreferenced inline function has been removed
//#pragma warning (push, 3)
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
//#pragma warning (pop)

// Shell headers
//#include "shobjidl.h"
#include <shlobj.h>

#define SHCIDS_ALLFIELDS        0x80000000L
#define SHCIDS_CANONICALONLY    0x10000000L
#define SHCIDS_BITMASK          0xFFFF0000L
#define SHCIDS_COLUMNMASK       0x0000FFFFL
#define SFGAO_CANCOPY           DROPEFFECT_COPY // Objects can be copied    (0x1)
#define SFGAO_CANMOVE           DROPEFFECT_MOVE // Objects can be moved     (0x2)
#define SFGAO_CANLINK           DROPEFFECT_LINK // Objects can be linked    (0x4)
#define SFGAO_STORAGE           0x00000008L     // supports BindToObject(IID_IStorage)
#define SFGAO_CANRENAME         0x00000010L     // Objects can be renamed
#define SFGAO_CANDELETE         0x00000020L     // Objects can be deleted
#define SFGAO_HASPROPSHEET      0x00000040L     // Objects have property sheets
#define SFGAO_DROPTARGET        0x00000100L     // Objects are drop target
#define SFGAO_CAPABILITYMASK    0x00000177L
#define SFGAO_ENCRYPTED         0x00002000L     // object is encrypted (use alt color)
#define SFGAO_ISSLOW            0x00004000L     // 'slow' object
//#define SFGAO_GHOSTED           0x00008000L     // ghosted icon
#define SFGAO_LINK              0x00010000L     // Shortcut (link)
#define SFGAO_SHARE             0x00020000L     // shared
#define SFGAO_READONLY          0x00040000L     // read-only
#define SFGAO_HIDDEN            0x00080000L     // hidden object
//#define SFGAO_DISPLAYATTRMASK   0x000FC000L
#define SFGAO_FILESYSANCESTOR   0x10000000L     // may contain children with SFGAO_FILESYSTEM
#define SFGAO_FOLDER            0x20000000L     // support BindToObject(IID_IShellFolder)
#define SFGAO_FILESYSTEM        0x40000000L     // is a win32 file system object (file/folder/root)
#define SFGAO_HASSUBFOLDER      0x80000000L     // may contain children with SFGAO_FOLDER
#define SFGAO_CONTENTSMASK      0x80000000L
#define SFGAO_VALIDATE          0x01000000L     // invalidate cached information
#define SFGAO_REMOVABLE         0x02000000L     // is this removeable media?
#define SFGAO_COMPRESSED        0x04000000L     // Object is compressed (use alt color)
#define SFGAO_BROWSABLE         0x08000000L     // supports IShellFolder, but only implements CreateViewObject() (non-folder view)
#define SFGAO_NONENUMERATED     0x00100000L     // is a non-enumerated object
#define SFGAO_NEWCONTENT        0x00200000L     // should show bold in explorer tree
#define SFGAO_CANMONIKER        0x00400000L     // defunct
#define SFGAO_HASSTORAGE        0x00400000L     // defunct
#define SFGAO_STREAM            0x00400000L     // supports BindToObject(IID_IStream)
#define SFGAO_STORAGEANCESTOR   0x00800000L     // may contain children with SFGAO_STORAGE or SFGAO_STREAM
#define SFGAO_STORAGECAPMASK    0x70C50008L     // for determining storage capabilities, ie for open/save semantics
typedef ULONG SFGAOF;

//HRESULT SHGetDesktopFolder(IShellFolder **ppshf);

#include <shlwapi.h>
//#include <shellapi.h>
//#include <shcore.h>
//#include <ole2.h>


