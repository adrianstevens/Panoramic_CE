/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssDebug.h
// Abstract: Common macros for debugging and error handling
/***************************************************************************************************************/

#pragma once

#include "IssCommon.h"

#ifndef ASSERT
#define ASSERT(x)
#endif

#define __WFUNCTION__				 _T("")
#define DELETE_ARRAY(array)           delete [] array
#define BAD_HANDLE_VALUE        NULL
#define BAD_FILE_HANDLE_VALUE   INVALID_HANDLE_VALUE

#define VALID_HANDLE(handle)            \
    (BAD_HANDLE_VALUE != (handle))

#define VALID_FILE_HANDLE(handle)           \
    (BAD_FILE_HANDLE_VALUE != (handle))

#define SAFE_DELETE(ptr)\
	if (NULL!= (ptr))   \
{                   \
	delete (ptr);   \
	(ptr) = NULL;   \
}

#define SAFE_DELETE_ARRAY(ptr)\
	if (NULL!= (ptr))         \
{                         \
	DELETE_ARRAY(ptr);    \
	(ptr) = NULL;         \
}

#ifdef DEBUG

#define DBG_OUT(printf_exp)    DebugOut printf_exp;

	// Check an HRESULT
	#define CHR(hr, desc)   \
		{ \
		HRESULT ___hr___ = hr; \
		if (___hr___ != S_OK)   \
			{                   \
			DebugOut(L".%s: %s failed (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, ___hr___); \
			goto Error;     \
			} \
		}

	// Check a boolean success result and generate an HRESULT if there was an error
	#define CBHR(b, desc)   \
		if (!(b))           \
			{                   \
            hr = E_FAIL; \
			DebugOut(L".%s: %s failed (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;     \
			}

	// Check an rc and generate an HRESULT if there was an error (i.e., rc != 0)
	#define CRHR(rc, desc)  \
			{ \
			DWORD __rc__ = (DWORD) rc;    \
			if (__rc__)             \
			{                   \
			hr = E_FAIL; \
			DebugOut(L".%s: %s failed (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;     \
			} \
			} 

	// Check a returned pointer and generate an HRESULT if it is null
	#define CPHR(ptr, desc) \
		if (!(ptr))         \
			{                   \
			DebugOut(L".%s: %s - alloc failure\r\n", __WFUNCTION__, desc); \
			hr = E_OUTOFMEMORY; \
			goto Error;     \
			}

	// Check a returned handle and generate an HRESULT if it is invalid
	#define CHHR(h, desc)       \
		if (!VALID_HANDLE(h))   \
			{                       \
			hr = E_FAIL; \
			DebugOut(L".%s: %s - open/create failure (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;         \
			}

	// Check a returned file handle and generate an HRESULT if it is invalid
	#define CFHHR(h, desc)          \
		if (!VALID_FILE_HANDLE(h))  \
			{                           \
			hr = E_FAIL; \
			DebugOut(L".%s: %s - open/create failure (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;             \
			}

	// Check an HRESULT and supply a specific HRESULT if there was an error
	#define CHRE(hr, desc, hrErr)   \
		if (hr != S_OK)   \
			{                   \
			DebugOut(L".%s: %s (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			hr = hrErr;     \
			goto Error;     \
			}

	// Check a boolean success result and generate a specific HRESULT if there was an error
	#define CBHRE(b, desc, hrErr)   \
		if (!(b))           \
			{                   \
			hr = hrErr;     \
			DebugOut(L".%s: %s (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;     \
			}

	// Check an rc and generate a specific HRESULT if there was an error (i.e., rc != 0)
	#define CRHRE(rc, desc, hrErr)  \
		if (rc)             \
			{                   \
			hr = hrErr;     \
			DebugOut(L".%s: %s (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;     \
			}

	// Check a returned pointer and generate a specific HRESULT if it is null
	#define CPHRE(ptr, desc, hrErr) \
		if (!(ptr))         \
			{                   \
			hr = hrErr;     \
			DebugOut(L".%s: %s - pointer invalid\r\n", __WFUNCTION__, desc); \
			goto Error;     \
			}

	// Check a returned handle and generate a specific HRESULT if it is invalid
	#define CHHRE(h, desc, hrErr)       \
		if (!VALID_HANDLE(h))   \
			{                       \
			hr = hrErr;         \
			DebugOut(L".%s: %s - open/create failure (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;         \
			}

	// Check a returned file handle and generate a specific HRESULT if it is invalid
	#define CFHHRE(h, desc, hrErr)          \
		if (!VALID_FILE_HANDLE(h))  \
			{                           \
			hr = hrErr;             \
			DebugOut(L".%s: %s - open/create failure (hr=0x%08.8X)\r\n", __WFUNCTION__, desc, hr); \
			goto Error;             \
			}

	// Check a returned HRESULT and associated pointer and generate an HRESULT if it FAILED or the pointer is null
	#define CPRHR(hret, ptrret, desc)      \
		{                                      \
		CHR(hret, desc);                   \
		CPHRE(ptrret, desc, E_UNEXPECTED); \
		}

	//
	// Parameter check macros (similar to the above, but different error strings)
	//

	//
	// Parameter check macros (similar to the above, but different error strings)
	//

	// Check a supplied expression and generate an HRESULT if it is not true
#define CBARG(exp, desc)        \
	if (!(exp))                 \
		{                           \
		DebugOut(L".%s: %s - Invalid argument: Expression (%S) is not true\r\n", __WFUNCTION__, desc, #exp); \
		hr = E_INVALIDARG;      \
		goto Error;             \
		}

// Check a supplied handle and generate an HRESULT if it is invalid
#define CHARG(handle, desc)     \
	if (!VALID_HANDLE(handle))  \
		{                           \
		DebugOut(L".%s: %s - Bad handle argument: %S\r\n", __WFUNCTION__, desc, #handle); \
		hr = E_INVALIDARG;      \
		goto Error;             \
		}

// Check a supplied file handle and generate an HRESULT if it is invalid
#define CFHARG(handle, desc)        \
	if (!VALID_FILE_HANDLE(handle)) \
		{                               \
		DebugOut(L".%s: %s - Bad file handle argument: %S\r\n", __WFUNCTION__, desc, #handle); \
		hr = E_INVALIDARG;          \
		goto Error;                 \
		}

	// Check a supplied pointer and generate an HRESULT if it is null
	#define CPARG(ptr)    \
		if (NULL == (ptr))      \
	{                       \
		DebugOut(L".%s: NULL pointer argument: %S\r\n", __WFUNCTION__, #ptr); \
		hr = E_POINTER;  \
		goto Error;         \
	}

	// Check IOCTL input buffer parameters
	#define CINBUF(SizeNeeded, desc)        \
		if ((0 < (SizeNeeded)) &&           \
		((dwLenIn < (SizeNeeded)) ||    \
		(NULL == pBufIn)))              \
	{                                   \
		DebugOut(L".%s: %s - Missing/undersized input buffer\r\n", __WFUNCTION__, desc); \
		hr = E_INVALIDARG;              \
		goto Error;                     \
	}

	// Check IOCTL output buffer parameters
	#define COUTBUF(SizeNeeded, desc)       \
		if ((0 < (SizeNeeded))&&            \
		((dwLenOut < (SizeNeeded)) ||   \
		(NULL == pBufOut)))             \
	{                                   \
		DebugOut(L".%s: %s - Missing/undersized output buffer\r\n", __WFUNCTION__, desc); \
		hr = E_INVALIDARG;              \
		goto Error;                     \
	}

	// Check an invariant expression
	#define CBEXPR(expr)       \
		if(!(expr))            \
	{                      \
		DebugOut(L".%s: Invariant %s is FALSE\r\n", __WFUNCTION__, L#expr); \
		DBG_ASSERT(FALSE); \
		hr = E_UNEXPECTED; \
		goto Error;        \
	}

#else  // DEBUG
	// Retail versions of the above, which suppress the error message:

#define DBG_OUT(printf_exp)    (void)0

	#define CHR(hr, desc)   \
		if (hr != S_OK)   \
	{                   \
		goto Error;     \
	}

	#define CBHR(b, desc)   \
		if (!(b))           \
	{                   \
		hr = E_FAIL; \
		goto Error;     \
	}

	#define CRHR(rc, desc)  \
		if (rc)             \
	{                   \
		hr = E_FAIL; \
		goto Error;     \
	}

	#define CPHR(ptr, desc) \
		if (!(ptr))         \
	{                   \
		hr = E_OUTOFMEMORY; \
		goto Error;     \
	}

	#define CHHR(h, desc)   \
		if (!VALID_HANDLE(h))           \
	{                   \
		hr = E_FAIL; \
		goto Error;     \
	}

	#define CFHHR(h, desc)  \
		if (INVALID_HANDLE_VALUE == (h)) \
	{                   \
		hr = E_FAIL; \
		goto Error;     \
	}

	// Check an HRESULT and supply a specific HRESULT if there was an error
	#define CHRE(hr, desc, hrErr)   \
		if (hr != S_OK)   \
	{                   \
		hr = hrErr;     \
		goto Error;     \
	}

	// Check a boolean success result and generate a specific HRESULT if there was an error
	#define CBHRE(b, desc, hrErr)   \
		if (!(b))           \
	{                   \
		hr = hrErr;     \
		goto Error;     \
	}

	// Check an rc and generate a specific HRESULT if there was an error (i.e., rc != 0)
	#define CRHRE(rc, desc, hrErr)  \
		if (rc)             \
	{                   \
		hr = hrErr;     \
		goto Error;     \
	}

	// Check a returned pointer and generate a specific HRESULT if it is null
	#define CPHRE(ptr, desc, hrErr) \
		if (!(ptr))         \
	{                   \
		hr = hrErr;     \
		goto Error;     \
	}

	// Check a returned handle and generate a specific HRESULT if it is invalid
	#define CHHRE(h, desc, hrErr)       \
		if (!VALID_HANDLE(h))   \
	{                       \
		hr = hrErr;         \
		goto Error;         \
	}

	// Check a returned file handle and generate a specific HRESULT if it is invalid
	#define CFHHRE(h, desc, hrErr)          \
		if (!VALID_FILE_HANDLE(h))  \
	{                           \
		hr = hrErr;             \
		goto Error;             \
	}

	// Check a returned HRESULT and associated pointer and generate an HRESULT if it FAILED or the pointer is null
	#define CPRHR(hret, ptrret, desc)      \
	{                                      \
		CHR(hret, desc);                   \
		CPHRE(ptrret, desc, E_UNEXPECTED); \
	}

	//
	// Parameter check macros (similar to the above, but different error strings)
	//

	#define CBARG(exp, desc)        \
		if (!(exp))                 \
	{                           \
		hr = E_INVALIDARG;      \
		goto Error;             \
	}

	#define CHARG(handle, desc)     \
		if (!VALID_HANDLE(handle))  \
	{                           \
		hr = E_INVALIDARG;      \
		goto Error;             \
	}

	#define CFHARG(handle, desc)        \
		if (!VALID_FILE_HANDLE(handle)) \
	{                               \
		hr = E_INVALIDARG;          \
		goto Error;                 \
	}

	#define CPARG(ptr)    \
		if (NULL == (ptr))      \
	{                       \
		hr = E_POINTER;  \
		goto Error;         \
	}

	// Check IOCTL input buffer parameters
	#define CINBUF(SizeNeeded, desc)        \
		if ((0 < (SizeNeeded)) &&           \
		((dwLenIn < (SizeNeeded)) ||    \
		(NULL == pBufIn)))              \
	{                                   \
		hr = E_INVALIDARG;              \
		goto Error;                     \
	}

	// Check IOCTL output buffer parameters
	#define COUTBUF(SizeNeeded, desc)       \
		if ((0 < (SizeNeeded))&&            \
		((dwLenOut < (SizeNeeded)) ||   \
		(NULL == pBufOut)))             \
	{                                   \
		hr = E_INVALIDARG;              \
		goto Error;                     \
	}

	// Check an invariant expression
	#define CBEXPR(expr)       \
		if(!(expr))            \
	{                      \
		hr = E_UNEXPECTED; \
		goto Error;        \
	}
#endif  // DEBUG...else

#if defined(DEBUG)
#define DBG_PROFILE(exp, b) DebugProfile(exp, b)
#else
#define DBG_PROFILE(exp, b) (void)0
#endif





