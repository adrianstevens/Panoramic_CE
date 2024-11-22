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

#include "idlist.h"
#include "malloc.h"

HRESULT SHGetMalloc(LPMALLOC * ppMalloc)
{
	ASSERT(ppMalloc);
	if (!ppMalloc)
		return E_INVALIDARG;

	// Should we just use CoGetMalloc instead?
	*ppMalloc = (LPMALLOC) new CShellMalloc();

	return ((*ppMalloc) ? NOERROR : E_OUTOFMEMORY);
}

HRESULT StrRetToBuf(STRRET * psr, LPCITEMIDLIST pidl, LPWSTR pszBuf, UINT cchBuf)
{
	ASSERT(psr);
	ASSERT(pszBuf);
	if (!psr || !pszBuf)
		return E_INVALIDARG;

	HRESULT hr = E_FAIL;
	ASSERT(STRRET_OFFSET != psr->uType);

	switch (psr->uType)
	{
	case STRRET_WSTR:
		{
			if (psr->pOleStr)
			{
				IMalloc * pShellMalloc = NULL;

				hr = SHGetMalloc(&pShellMalloc);

				if (SUCCEEDED(hr))
				{
					hr = StringCchCopy(pszBuf, cchBuf, psr->pOleStr);

					pShellMalloc->Free(psr->pOleStr);
					pShellMalloc->Release();

					// Make sure no one thinks things are allocated still
					psr->uType = STRRET_CSTR;
					psr->cStr[0] = 0;
				}
			}
		}
		break;

	case STRRET_CSTR:
		{
			mbstowcs(pszBuf, psr->cStr, cchBuf);
			hr = NOERROR;
		}
		break;
	}

	if (FAILED(hr) && cchBuf)
		*pszBuf = 0;

	return hr;
}

HRESULT SHBindToParent(LPCITEMIDLIST pidl, REFIID riid, VOID ** ppv, LPCITEMIDLIST * ppidlLast)
{
	ASSERT(pidl);
	ASSERT(ppv);
	if (!pidl || !ppv)
		return E_INVALIDARG;
	*ppv = NULL;

	HRESULT hr = NOERROR;
	IShellFolder * pDesktop = NULL;
	IShellFolder * pParent = NULL;
	UINT uCount = 0;

	uCount = ILCount(pidl);
	if (0 == uCount)
		return E_FAIL;

	hr = SHGetDesktopFolder(&pDesktop);
	if (FAILED(hr))
		return hr;

	if (1 == uCount)
	{
		hr = pDesktop->QueryInterface(IID_IShellFolder, (VOID **) &pParent);
	}
	else
	{
		LPCITEMIDLIST pidlParent = ILCopy(pidl, uCount-1);
		if (pidlParent)
		{
			hr = pDesktop->BindToObject(pidlParent, NULL, IID_IShellFolder, (VOID **) &pParent);
			ILFree(pidlParent);
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}

	pDesktop->Release();

	if (FAILED(hr))
		return hr;

	ASSERT(pParent);
	hr = pParent->QueryInterface(riid, ppv);
	pParent->Release();

	if (SUCCEEDED(hr) && ppidlLast)
	{
		*ppidlLast = ILCopy(ILFindLast(pidl), 1);
		if (!*ppidlLast)
		{
			((IUnknown *)(*ppv))->Release();
			*ppv = NULL;
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

HRESULT SHBindToObject (IShellFolder *psf, REFIID riid, LPCITEMIDLIST pidl, void **ppvOut)
{
	HRESULT hr = E_FAIL;
	IShellFolder *psfRelease = NULL;

	if (!psf)
	{
		hr = SHGetDesktopFolder(&psf);
		if (SUCCEEDED(hr) && psf)
		{
			psfRelease = psf;
		}
		else
		{
			ASSERT(psf);
			return hr;
		}
	}
	else
	{
		psfRelease = NULL;
	}

	if (ILIsEmpty(pidl))
	{
		hr = psf->QueryInterface(riid, ppvOut);
	}
	else
	{
		hr = psf->BindToObject(pidl, NULL, riid, ppvOut);
	}

	if (psfRelease)
	{
		psfRelease->Release();
	}

	if (SUCCEEDED(hr) && (*ppvOut == NULL))
	{
		//Some lame shell extensions (eg WS_FTP on desktop) will return success and a null out pointer
		//DEBUGMSG(ZONE_WARNING, (_T("SHBindToObject: BindToObject succeeded but returned null ppvOut!!")));
	}

	return hr;
}

LPITEMIDLIST ILConcatenate(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
   LPITEMIDLIST pidlCat = NULL;

   if (pidl1 && pidl2)
   {
      size_t cb1 = ILSize(pidl1, IL_ALL)-sizeof(pidl1->mkid.cb);
      size_t cb2 = ILSize(pidl2, IL_ALL)-sizeof(pidl1->mkid.cb);

      pidlCat = (LPITEMIDLIST) LocalAlloc(LPTR, cb1+cb2+sizeof(pidl1->mkid.cb));
      if (pidlCat)
      {
         memcpy(pidlCat, pidl1, cb1);
         memcpy(((LPBYTE) pidlCat)+cb1, pidl2, cb2);
      }
   }

   return pidlCat;
}

LPITEMIDLIST ILCopy(LPCITEMIDLIST pidl, UINT cbItems)
{
   LPITEMIDLIST pidlCopy = NULL;

   if (pidl)
   {
      size_t cb = ILSize(pidl, cbItems);
      pidlCopy = (LPITEMIDLIST) LocalAlloc(LPTR, cb);
      if (pidlCopy)
         memcpy(pidlCopy, pidl, cb-sizeof(pidl->mkid.cb));
   }

   return pidlCopy;
}

DWORD ILCount(LPCITEMIDLIST pidl)
{
   DWORD dw = 0;

   while (!ILIsEmpty(pidl))
   {
      dw++;
      pidl = ILNext(pidl);
   }

   return dw;
}

LPCWSTR ILDisplayName(LPCITEMIDLIST pidl)
{
   if (pidl)
      return (LPCWSTR) IL_DISPLAYNAME(pidl);
   else
      return NULL;
}

LPITEMIDLIST ILFindChild(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild)
{
   if (ILIsParent(pidlParent, pidlChild, FALSE))
   {
      while (!ILIsEmpty(pidlParent))
      {
         pidlChild = ILNext(pidlChild);
         pidlParent = ILNext(pidlParent);
      }

      return (LPITEMIDLIST) pidlChild;
   }

   return NULL;
}

LPITEMIDLIST ILFindLast(LPCITEMIDLIST pidl)
{
   LPITEMIDLIST pidlLast = (LPITEMIDLIST) pidl;
   LPITEMIDLIST pidlNext = (LPITEMIDLIST) pidl;

   while (!ILIsEmpty(pidlNext))
   {
      pidlLast = pidlNext;
      pidlNext = ILNext(pidlLast);
   }

   return pidlLast;
}

void ILFree(LPCITEMIDLIST pidl)
{
   if (pidl)
      LocalFree((void*)pidl);
}

BOOL ILGetDateCached(LPCITEMIDLIST pidl, FILETIME* ftDateOut)
{
   if (pidl && (CEIDLIST_FS_DATECACHED & IL_GET_FLAGS(pidl)))
   {
      memcpy(ftDateOut, IL_DATECACHED(pidl), sizeof(FILETIME));
      return TRUE;
   }
   return FALSE;
}
BOOL ILGetSizeCached(LPCITEMIDLIST pidl, ULARGE_INTEGER* uliSizeOut)
{
   if (pidl && (CEIDLIST_FS_SIZECACHED & IL_GET_FLAGS(pidl)))
   {
      memcpy(uliSizeOut, IL_SIZECACHED(pidl), sizeof(ULARGE_INTEGER));
      return TRUE;
   }
   return FALSE;
}

LPCWSTR ILGetTypeCached(LPCITEMIDLIST pidl)
{
   if (pidl && (CEIDLIST_FS_TYPECACHED & IL_GET_FLAGS(pidl)))
      return (LPCWSTR) IL_TYPECACHED(pidl);
   else
      return NULL;
}

BOOL ILHasDateCached(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   return (CEIDLIST_FS_DATECACHED & IL_GET_FLAGS(pidl));
}

BOOL ILHasSizeCached(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   return (CEIDLIST_FS_SIZECACHED & IL_GET_FLAGS(pidl));
}

BOOL ILHasTypeCached(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   return (CEIDLIST_FS_TYPECACHED & IL_GET_FLAGS(pidl));
}

BOOL ILIsEmpty(LPCITEMIDLIST pidl)
{
   if(!pidl)
      return TRUE;

   return (0 == (pidl)->mkid.cb);
}

BOOL ILIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
   BOOL fEqual = FALSE;
   IShellFolder * psfDesktop = NULL;

   if (SUCCEEDED(SHGetDesktopFolder(&psfDesktop)))
   {
      fEqual = (0 == psfDesktop->CompareIDs(0, pidl1, pidl2));
      psfDesktop->Release();
   }

   return fEqual;
}

BOOL ILIsEqualParsingNames(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
   if (ILIsEmpty(pidl1) || ILIsEmpty(pidl2))
      return FALSE;

   return (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                                    IL_PARSINGNAME(pidl1), -1,
                                    IL_PARSINGNAME(pidl2), -1));
}

BOOL ILIsFileSystemPidl(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   return (CEIDLIST_FILESYSTEM & IL_GET_FLAGS(pidl));
}

BOOL ILIsFileSystemDirectoryPidl(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   WORD flags = IL_GET_FLAGS(pidl);
   return ((CEIDLIST_FILESYSTEM & flags) && (CEIDLIST_FS_DIRECTORY & flags));
}

BOOL ILIsFileSystemLinkPidl(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   WORD flags = IL_GET_FLAGS(pidl);
   return ((CEIDLIST_FILESYSTEM & flags) && (CEIDLIST_FS_LINK & flags));
}


BOOL ILIsNameSpacePidl(LPCITEMIDLIST pidl)
{
   if (ILIsEmpty(pidl))
      return FALSE;

   return (CEIDLIST_NAMESPACE & IL_GET_FLAGS(pidl));
}

BOOL ILIsNameSpace(LPCITEMIDLIST pidl, REFCLSID rclsid)
{
   CLSID clsid = {0};
   LPITEMIDLIST pidlLast = ILFindLast(pidl);

   if (pidlLast && ILIsNameSpacePidl(pidlLast) &&
       SUCCEEDED(CLSIDFromString(IL_PARSINGNAME(pidlLast), &clsid)) &&
       IsEqualGUID(clsid, rclsid))
   {
      return TRUE;
   }

   return FALSE;
}

BOOL ILIsParent(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild, BOOL fImmediate)
{
   LPITEMIDLIST pidlParentTemp = (LPITEMIDLIST) pidlParent;
   LPITEMIDLIST pidlChildTemp = (LPITEMIDLIST) pidlChild;

   if (!pidlParent || !pidlChild)
      return FALSE;

   while (!ILIsEmpty(pidlParentTemp))
   {
      // If pidlChild is shorter than pidlParent, pidlParent can't be its parent.
      if (ILIsEmpty(pidlChildTemp))
         return FALSE;

      pidlParentTemp = ILNext(pidlParentTemp);
      pidlChildTemp = ILNext(pidlChildTemp);
   }

   if (fImmediate)
   {
      // If fImmediate is TRUE, pidlChildTemp should contain exactly one ID.
      if (ILIsEmpty(pidlChildTemp) || !ILNext(pidlChildTemp) || !ILIsEmpty(ILNext(pidlChildTemp)))
         return FALSE;
   }

   // Create a new IDList from a portion of pidlChild, which contains the
   // same number of IDs as pidlParent.
   size_t cb = (size_t)pidlChildTemp-(size_t)pidlChild;
   pidlChildTemp = (LPITEMIDLIST) LocalAlloc(LPTR, cb+sizeof(pidlChild->mkid.cb));
   if (pidlChildTemp)
   {
      BOOL fRet;

      memcpy(pidlChildTemp, pidlChild, cb);
      fRet = ILIsEqual(pidlParent, pidlChildTemp);

      ILFree(pidlChildTemp);
      return fRet;
   }

   return FALSE;
}

LPITEMIDLIST ILNext(LPCITEMIDLIST pidl)
{
   LPITEMIDLIST pidlNext = NULL;

   if (pidl)
   {
      pidlNext = (LPITEMIDLIST) (((LPBYTE) pidl) + pidl->mkid.cb);
      if (ILIsEmpty(pidlNext))
         pidlNext = NULL;
   }

   return pidlNext;
}

LPCWSTR ILParsingName(LPCITEMIDLIST pidl)
{
   if (pidl)
      return (LPCWSTR) IL_PARSINGNAME(pidl);
   else
      return NULL;
}

BOOL ILRemoveLast(LPITEMIDLIST pidl)
{
   BOOL fRemoved = FALSE;

   LPITEMIDLIST pidlLast = ILFindLast(pidl);
   if (pidlLast)
   {
      // Remove the last one, null-terminator note that this doesn't clean up the memory
      pidlLast->mkid.cb = 0;
      fRemoved = TRUE;
   }

   return fRemoved;
}

size_t ILSize(LPCITEMIDLIST pidl, UINT cbItems)
{
   if (!pidl)
      return 0;

   size_t cb = 0;

   while (pidl && cbItems--)
   {
      cb += pidl->mkid.cb;
      pidl = ILNext(pidl);
   }

   return (cb + sizeof(pidl->mkid.cb));
}

UINT ILGetSize(LPITEMIDLIST pidl) 
{
	UINT cbTotal = 0;
	if (pidl) 
	{
		cbTotal += sizeof(pidl->mkid.cb); // Null terminator
		while (pidl && pidl->mkid.cb) 
		{
			cbTotal += pidl->mkid.cb;
			pidl = ILNext(pidl);
		}
	}
	return cbTotal;
}

LPITEMIDLIST ILCreate(LPCWSTR pwszParsingName, LPCWSTR pwszDisplayName, WORD flags)
{
	return ILCreateWithCache(pwszParsingName, pwszDisplayName, flags, NULL, NULL);
}

LPITEMIDLIST ILCreateWithCache(LPCWSTR pwszParsingName, LPCWSTR pwszDisplayName, WORD flags,
							   LPCWSTR pwszFilePath, WIN32_FIND_DATA * pfd)
{
	LPITEMIDLIST pidl = NULL;
	size_t cchParsing = 0;
	size_t cchDisplay = 0;

	if (SUCCEEDED(StringCchLength(pwszParsingName, MAX_PATH, &cchParsing)))
	{
		size_t cb = 0;
		size_t offset = 0;
		size_t cacheOffset = 0;

		if ((pwszParsingName == pwszDisplayName) ||
			FAILED(StringCchLength(pwszDisplayName, MAX_PATH, &cchDisplay)))
		{
			pwszDisplayName = NULL;
		} 

		cb += sizeof(pidl->mkid.cb);
		cb += sizeof(WORD); // flags
		ASSERT(0 == cb%4); // Verify that the address starts DWORD aligned
		cb += sizeof(WORD); // offset of DisplayName from the beginning of the pidl
		cb += sizeof(WORD); // offset of Cache from the beginning of the pidl
		ASSERT(0 == cb%4); // Verify that the string starts DWORD aligned
		offset = cb;
		cb += ((cchParsing+1)*sizeof(WCHAR)); // The ParsingName
		cb += ((0 < cb%4) ? (4-(cb%4)) : 0); // DWORD align the DisplayName
		ASSERT(0 == cb%4); // Verify that the string starts DWORD aligned
		if (pwszDisplayName)
		{
			offset = cb;
			cb += ((cchDisplay+1)*sizeof(WCHAR)); // The DisplayName
			cb += ((0 < cb%4) ? (4-(cb%4)) : 0); // DWORD align the DisplayName
			ASSERT(0 == cb%4); // Verify that the item ends DWORD aligned
		}
		cacheOffset = cb;

		SHFILEINFO shfi = {0};               
		size_t cchType = 0;
		if(pfd)
		{
			cb += sizeof(FILETIME); // The Date
			ASSERT(0 == cb%4); // Verify that the address is DWORD aligned
			if (!((CEIDLIST_FILESYSTEM & flags) && (CEIDLIST_FS_DIRECTORY & flags)))
			{
				cb += sizeof(ULARGE_INTEGER); // The Size
				ASSERT(0 == cb%4); // Verify that the string starts DWORD aligned
				if (pwszFilePath && SHGetFileInfo(pwszFilePath, 0, &shfi, sizeof(shfi), SHGFI_TYPENAME))
				{
					if (SUCCEEDED(StringCchLength(shfi.szTypeName, lengthof(shfi.szTypeName), &cchType)))
					{
						cb += ((cchType+1)*sizeof(WCHAR)); // The Type
						cb += ((0 < cb%4) ? (4-(cb%4)) : 0); // DWORD align the Type
						ASSERT(0 == cb%4); // Verify that the item ends DWORD aligned
					}
				}
			}
		}

		LPBYTE p = (LPBYTE) LocalAlloc(LPTR, cb+2); // 2 byte NULL ends the pidl
		if (p)
		{
			flags &= ~CEIDLIST_FS_CACHEMASK;

			IL_CB(p) = (USHORT) cb;
			IL_SET_DISPLAYNAME_OFFSET(p,offset);
			IL_SET_CACHE_OFFSET(p,cacheOffset);

			wcscpy(IL_PARSINGNAME(p), pwszParsingName);
			if (pwszDisplayName)
				wcscpy(IL_DISPLAYNAME(p), pwszDisplayName);

			if(pfd)
			{
				memcpy(IL_DATECACHED(p), &pfd->ftLastWriteTime, sizeof(FILETIME));
				flags |= CEIDLIST_FS_DATECACHED;

				if (!((CEIDLIST_FILESYSTEM & flags) && (CEIDLIST_FS_DIRECTORY & flags)))
				{
					ULARGE_INTEGER uliSize ;
					uliSize.HighPart = pfd->nFileSizeHigh;
					uliSize.LowPart = pfd->nFileSizeLow;
					memcpy(IL_SIZECACHED(p), &uliSize, sizeof(ULARGE_INTEGER));
					flags |= CEIDLIST_FS_SIZECACHED;

					if(cchType)
					{
						wcscpy(IL_TYPECACHED(p), shfi.szTypeName);
						flags |= CEIDLIST_FS_TYPECACHED;
					}
				}
			}
			IL_SET_FLAGS(p,flags);

			pidl = (LPITEMIDLIST) p;
		}
	}

	return pidl;
}

HRESULT CreateFileSystemPidl(LPCWSTR pwszPathPart, LPITEMIDLIST * ppidl)  
{  
	ASSERT(pwszPathPart);  
	ASSERT(ppidl);  
	if (!pwszPathPart || !ppidl)  
		return E_INVALIDARG;  

	HRESULT hr = NOERROR;  

	//LPITEMIDLIST p = ILCreate(pwszPathPart, NULL, CEIDLIST_FILESYSTEM);  
	*ppidl = (LPITEMIDLIST)ILCreate(pwszPathPart, NULL, CEIDLIST_FILESYSTEM);  
	if (!*ppidl)  
		hr = E_OUTOFMEMORY;  

	return hr;  
}