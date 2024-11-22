/********************************************************************
	created:	2004/03/29

	filename: 	IssXmlBuilder.cpp

	author:		Patrick Abadi
				Copyright Implicit Software Solutions 2003 - 2004
				www.implicitsoftware.com
	
	purpose:	Implementation for creating XML files using MSXML 3.0
*********************************************************************/



#include "stdafx.h"
#include "IssXmlBuilder.h"



CIssXmlBuilder::CIssXmlBuilder()
{
	m_pDoc = NULL;
	m_pCurrNode = NULL;
	m_pConstructionNode = NULL;
	m_pXmlAscii = NULL;
}


CIssXmlBuilder::~CIssXmlBuilder()
{
#ifdef UNDER_CE
	ASSERT(NULL == m_pDoc.p);
	ASSERT(NULL == m_pCurrNode.p);
	ASSERT(NULL == m_pConstructionNode.p);
	ASSERT(NULL == m_pXmlAscii);
#endif
}

/********************************************************************
	Function:	Open
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Opens an empty document (ie. one with no elements yet).
				Subsequent builder methods will add elements and attributes
				to this document.
*********************************************************************/
BOOL CIssXmlBuilder::Open()
{
	HRESULT hr;

	// Use apartment-threaded because the client, Watcher, is an MFC application
	// which is most likely to be using OLE controls. To use these, Watcher
	// must call OleInitialize(). This translates into a call 
	// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED). Our code runs in the same 
	// thread as this call. It has no need to be multithreaded. Hence we make the
	// identical call otherwise the call will fail with hr = 0x8001016 indicating a 
	// threading model conflict for the thread.
	//
	// The call now will return with hr = 0x00000001 (S_FALSE). This is acceptable and means
	// COM has already been initialized to the model we want. Note that we still 
	// need to match this call with a call to CoUninitialize().
	// 
#ifdef UNDER_CE
	hr = CoInitializeEx(NULL,
#ifdef UNDER_CE
		COINIT_MULTITHREADED);
#else
		COINIT_APARTMENTTHREADED);
#endif
#else
	hr = CoInitialize(NULL);
#endif

	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}

	// Create an instance of the Builder
	hr = CoCreateInstance (CLSID_DOMDocument, 
		NULL, 
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
		IID_IXMLDOMDocument, 
		(LPVOID *)&m_pDoc);
	if (FAILED(hr) || (m_pDoc.p == NULL)) 
	{
		return FALSE;
	}

	// Pocket PC workaround:
	// Remove document safety options.
	// See http://www.microsoft.com/mobile/developer/technicalarticles/xmlparser.asp.
	// Causes no problems on win32 either so no need for conditional compile.

	CComPtr<IObjectSafety> pSafety;
	DWORD dwSupported, dwEnabled;
	m_pDoc->QueryInterface( IID_IObjectSafety, (void**)&pSafety);
	pSafety->GetInterfaceSafetyOptions(
		IID_IXMLDOMDocument, 
		&dwSupported, 
		&dwEnabled);
 	pSafety->SetInterfaceSafetyOptions(
		IID_IXMLDOMDocument,
		dwSupported, 
		0);

	// End of workaround.
	
	m_pCurrNode = m_pDoc;
	
	m_pXmlAscii = NULL;

	return TRUE;
}

/********************************************************************
	Function:	Close
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Deallocates resources and closes the builder
*********************************************************************/
void CIssXmlBuilder::Close()
{
	if (m_pXmlAscii)
	{
		delete m_pXmlAscii;
		m_pXmlAscii = NULL;
	}
	
	m_pDoc           = NULL;
	m_pCurrNode      = NULL;
	m_pConstructionNode = NULL;

	CoUninitialize();
}

/********************************************************************
	Function:	SetCurrentElement
	
	Argument:	szPath - node path descriptor
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Sets the document's current element to point to the node
				identified by a string
*********************************************************************/
BOOL CIssXmlBuilder::SetCurrentElement(LPCTSTR szPath)
{
	HRESULT hr;
	CComBSTR bstrPath(szPath);

	m_pCurrNode = NULL;
	hr = m_pDoc->selectSingleNode(bstrPath, &m_pCurrNode);
	if (FAILED(hr) || (m_pCurrNode.p == NULL))
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************
	Function:	SetCurrentElement
	
	Argument:	szPath - node path descriptor
				iIndex - which one in the list to select
				szExtraPath - if we need to go down further into the document
			
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Sets the document's current element to point to the node
				identified by a string
*********************************************************************/
BOOL CIssXmlBuilder::SetCurrentElementinList(LPCTSTR szPath, int iIndex, LPCTSTR szExtraPath)
{
	HRESULT hr;
	CComBSTR bstrPath(szPath);
	CComPtr<IXMLDOMNodeList> m_pNodeList = NULL;
	CComPtr<IXMLDOMNode> m_pNewNode = NULL;
	long lNode;

	m_pCurrNode = NULL;
	// get the list of nodes
	hr = m_pDoc->selectNodes(bstrPath, &m_pNodeList);
	if (FAILED(hr))
	{
		return FALSE;
	}

	hr = m_pNodeList->get_item(iIndex, &m_pCurrNode);
	if (FAILED(hr) || (m_pCurrNode.p == NULL))
	{
		return FALSE;
	}

	// if there is more searching to do
	if(NULL != szExtraPath)
	{
		m_pNodeList = NULL;
		CComBSTR bstrNewPath(szExtraPath);
		hr = m_pCurrNode->selectNodes(bstrNewPath, &m_pNodeList);
		if (FAILED(hr))
		{
			return FALSE;
		}

		hr = m_pNodeList->get_length(&lNode);
		if(FAILED(hr))
		{
			return FALSE;
		}

		lNode --;
		m_pCurrNode = NULL;
		hr = m_pNodeList->get_item(lNode, &m_pCurrNode);
		if (FAILED(hr) || (m_pCurrNode.p == NULL))
		{
			return FALSE;
		}
	}


	return TRUE;
}


/********************************************************************
	Function:	CreateElement
	
	Argument:	szName - tag name for new element
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Creates and element with the given tag name.  This element
				becomes the builder's current construction node.  The element
				is standalone and must be explicitly attached to 
				the document using InsertChileElement()
*********************************************************************/
BOOL CIssXmlBuilder::CreateElement(LPCTSTR szName)
{
	HRESULT hr;
	CComBSTR bstrName(szName);

	// It is the doc which creates nodes.
    m_pConstructionNode = NULL;
    hr = m_pDoc->createNode(CComVariant(NODE_ELEMENT), 
		bstrName, 
		NULL, 
		&m_pConstructionNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}



/********************************************************************
	Function:	CreateTextElement
	
	Argument:	szName - tag name for new element
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Creates an element with the given tag name.  
				This element becomes the builder's current construction node.  The element
				is standalone and must be explicitly attached to 
				the document using InsertChileElement()
*********************************************************************/
BOOL CIssXmlBuilder::CreateTextElement(LPCTSTR szName)
{
	HRESULT hr;
	CComBSTR bstrName(_T("BaseLable"));
	CComBSTR bstrText(szName);

	// It is the doc which creates nodes.
	// Note: the bstrName is not used because it's of type
	//		 NODE_TEXT.
    hr = m_pDoc->createNode(CComVariant(NODE_TEXT), 
		bstrText, 
		NULL, 
		&m_pConstructionNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// add the Text now
	hr = m_pConstructionNode->put_text(bstrText);
	if (FAILED(hr))
	{
		return FALSE;
	}


	return TRUE;
}

/********************************************************************
	Function:	AddAttribute
	
	Argument:	szName - name of attribute
				szValue - value string
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Adds an attribute to the element which is currently being 
				constructed.
*********************************************************************/
BOOL CIssXmlBuilder::AddAttribute(LPCTSTR szName, TCHAR* szValue)
{
	HRESULT hr;
	CComBSTR bstrName(szName);
	CComVariant varValue(szValue);
	
	if (m_pConstructionNode == NULL)
	{
		return FALSE;
	}
	
	// Add the attribute. Note we do this through the IXMLDOMElement
	// interface, so we need to do the QI(). This is a bit like casting a base pointer
	// to a derived type so we can use extra functionality.
	CComQIPtr<IXMLDOMElement> pElement;
	pElement = m_pConstructionNode;

	hr = pElement->setAttribute(bstrName, varValue);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}


/********************************************************************
	Function:	AddAttribute
	
	Argument:	szName - name of attribute
				iInt   -  int to save
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Adds an attribute to the element which is currently being 
				constructed.
*********************************************************************/
BOOL CIssXmlBuilder::AddAttribute(LPCTSTR szName,  int& iInt)
{
	HRESULT hr;
	CComBSTR bstrName(szName);
	TCHAR szValue[255];
	wsprintf(szValue, _T("%i"), iInt);
	CComVariant varValue(szValue);
	
	if (m_pConstructionNode == NULL)
	{
		return FALSE;
	}
	
	// Add the attribute. Note we do this through the IXMLDOMElement
	// interface, so we need to do the QI(). This is a bit like casting a base pointer
	// to a derived type so we can use extra functionality.
	CComQIPtr<IXMLDOMElement> pElement;
	pElement = m_pConstructionNode;

	hr = pElement->setAttribute(bstrName, varValue);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CIssXmlBuilder::AddAttribute(LPCTSTR szName, LPVOID lpData, int iLen)
{
	TCHAR* szOutput;
	TCHAR szHex[STRING_SMALL];
	CIssString* oStr = CIssString::Instance();

	szOutput = new TCHAR[2*iLen + 1];

	oStr->Empty(szOutput);
	oStr->Empty(szHex);

	memset(szOutput, 0, sizeof(TCHAR)*(2*iLen+1));

	byte* lpBuf = (byte*)lpData;

	int iCount = 0;
	while(iCount<iLen)
	{
		oStr->Format(szHex, _T("%.2X"), lpBuf[iCount]);
		szHex[2] = _T('\0');
		oStr->Concatenate(szOutput, szHex);
		iCount++;
	}

	BOOL bReturn = AddAttribute(szName, szOutput);

	oStr->Delete(&szOutput);

	return TRUE;
}


/********************************************************************
	Function:	InsertChileElement
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Inserts the element under construction into the document as a
				child of the document's current element
*********************************************************************/
BOOL CIssXmlBuilder::InsertChildElement()
{
	HRESULT hr;

	if (m_pConstructionNode == NULL)
	{
		return FALSE;
	}

	CComPtr<IXMLDOMNode> pInsertedNode;

	// If appendChild succeeds, it's two parameters end up being equal.
	hr = m_pCurrNode->appendChild(m_pConstructionNode, &pInsertedNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// The element now belongs to the tree. 
	m_pConstructionNode = NULL;

	return TRUE;
}

/********************************************************************
	Function:	InsertChileElement
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Inserts the element under construction into the document as a
				child of the document's current element
*********************************************************************/
BOOL CIssXmlBuilder::InsertChildElement(CComPtr<IXMLDOMNode> pNodeToInsert)
{
	HRESULT hr;

	CComPtr<IXMLDOMNode> pInsertedNode;

	// If appendChild succeeds, it's two parameters end up being equal.
	hr = m_pCurrNode->appendChild(pNodeToInsert, &pInsertedNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

CComPtr<IXMLDOMNode> CIssXmlBuilder::GetCurrentNode()
{
	return m_pCurrNode;
}


/********************************************************************
	Function:	SaveToFile
	
	Argument:	szFileName - filename to save to
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Saves the document to a file
*********************************************************************/
BOOL CIssXmlBuilder::SaveToFile(LPCTSTR szFileName)
{
	// first try and delete the file if it's there
	DeleteFile(szFileName);

	HRESULT hr;
	CComVariant varFileName(szFileName);

	if (m_pDoc == NULL)
	{
		return FALSE;
	}

	hr = m_pDoc->save(varFileName);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************
	Function:	InsertProcessingInstruction
	
	Argument:	szTarget - instruction target eg. _T("target")
				szInstruction - instruction content eg. _T("value=\"x.y.y\"")
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Inserts an XML processing instruction to the document.  
				eg. used to insert the header <?target value="x.y.z"?>
*********************************************************************/
BOOL CIssXmlBuilder::InsertProcessingInstruction(LPCTSTR szTarget, LPCTSTR szInstruction)
{
	// This code works on Win32. It works on WinCE only if szTarget is not "xml".
	// Therfore we use the alternative code below.

	HRESULT hr;
	CComBSTR bstrTarget(szTarget);
	CComBSTR bstrInstruction(szInstruction);
	CComPtr<IXMLDOMProcessingInstruction> pProcessingInstruction;

	hr = m_pDoc->createProcessingInstruction(bstrTarget, 
		bstrInstruction,
		&pProcessingInstruction);
	if (FAILED(hr))
	{
		return FALSE;
	}

	CComPtr<IXMLDOMNode> pInsertedNode;
	// If appendChild succeeds, it's two parameters end up being equal.
	hr = m_pCurrNode->appendChild(pProcessingInstruction, &pInsertedNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************
	Function:	InsertXmlHeader
	
	Argument:					
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Function that will insert and XML header with the version
				e.g. "?xml version=1.0?"
*********************************************************************/
BOOL CIssXmlBuilder::InsertXmlHeader()
{
	HRESULT hr;
	CComPtr<IXMLDOMNode> pNode;

	// It is the doc which creates nodes.
    hr = m_pDoc->createNode(CComVariant(NODE_PROCESSING_INSTRUCTION), 
		CComBSTR(_T("xml")), 
		NULL, 
		&pNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// The attribute "version="1.0"" is appended by default, we don't need to do it. 
	
	CComPtr<IXMLDOMNode> pInsertedNode;
	// If appendChild succeeds, it's two parameters end up being equal.
	hr = m_pCurrNode->appendChild(pNode, &pInsertedNode);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CIssXmlBuilder::Open(TCHAR* szXMLFile)
{
	HRESULT hr;

	// Use apartment-threaded for Win32 because the client, Watcher, is an MFC application
	// which is most likely to be using OLE controls. To use these, Watcher
	// must call OleInitialize(). This translates into a call in Win32 
	// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED). Our code runs in the same 
	// thread as this call. It has no need to be multithreaded. Hence we make the
	// identical call otherwise the call will fail with hr = 0x8001016 indicating a 
	// threading model conflict for the thread.
	//
	// The call now will return with hr = 0x00000001 (S_FALSE). This is acceptable and means
	// COM has already been initialized to the model we want. Note that we still 
	// need to match this call with a call to CoUninitialize().
	// 
#ifdef UNDER_CE
	hr = CoInitializeEx(NULL,
#ifdef UNDER_CE
		COINIT_MULTITHREADED);
#else
		COINIT_APARTMENTTHREADED);
#endif
#else
	hr = CoInitialize(NULL);
#endif
	
	if(!SUCCEEDED(hr))
	{
	  return FALSE;
	}

	// Create an instance of the parser
	hr = CoCreateInstance (CLSID_DOMDocument, 
	  NULL, 
	  CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
	  IID_IXMLDOMDocument, 
	  (LPVOID *)&m_pDoc);
	if (FAILED(hr) || (m_pDoc.p == NULL)) 
	{
	  return FALSE;
	}

	// Pocket PC workaround:
	// Remove document safety options.
	// See http://www.microsoft.com/mobile/developer/technicalarticles/xmlparser.asp.
	// Causes no problems on win32 either so no need for conditional compile.

	CComPtr<IObjectSafety> pSafety;
	DWORD dwSupported, dwEnabled;

	m_pDoc->QueryInterface( IID_IObjectSafety, (void**)&pSafety );
	pSafety->GetInterfaceSafetyOptions(
	  IID_IXMLDOMDocument, 
	  &dwSupported, 
	  &dwEnabled);
	pSafety->SetInterfaceSafetyOptions(
	  IID_IXMLDOMDocument,
	  dwSupported, 
	  0);

	// End of workaround.

	// Do we want to validate against DOCTYPE.

	#pragma warning(disable: 4310) // cast truncates constant value
	VARIANT_BOOL bValidate = VARIANT_TRUE;
	#pragma warning(default: 4310) // cast truncates constant value

	hr = m_pDoc->put_validateOnParse(bValidate);
	if (FAILED(hr))
	{
	  return FALSE;
	}

	// Do we want to resolve external namespace/doc type references.

	#pragma warning(disable: 4310) // cast truncates constant value
	VARIANT_BOOL bResolveExternals = VARIANT_TRUE;
	#pragma warning(default: 4310) // cast truncates constant value

	hr = m_pDoc->put_resolveExternals(bResolveExternals);
	if (FAILED(hr))
	{
	  return FALSE;
	}

	// Load data from appropriate source. 

	VARIANT_BOOL bSuccess = FALSE;

  hr = m_pDoc->loadXML(CComBSTR(szXMLFile) , &bSuccess);
  if (FAILED(hr) ||!bSuccess)
  {
	return FALSE;
  }

	m_pCurrNode = m_pDoc;

	return TRUE;
}

