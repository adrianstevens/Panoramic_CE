/********************************************************************
	created:	2004/03/30

	filename: 	IssXmlParser.cpp

	author:		Patrick Abadi
				Copyright Implicit Software Solutions 2003 - 2004
				www.implicitsoftware.com
	
	purpose:	Implementation of XML parser using MSXML 3.0
*********************************************************************/

#include "IssXmlParser.h"
#include "IssString.h"



CIssXmlParser::CIssXmlParser()
{
	m_pDoc			= NULL;
	m_pCurrNode		= NULL;
	m_pCurrNodeList = NULL;
	m_pCurrChildNodeList = NULL;
	m_pSavedChildNodeList = NULL;
}

CIssXmlParser::CIssXmlParser(IXMLDOMNode *pNode)
{
	m_pCurrNode		= pNode;
	pNode->get_ownerDocument(&m_pDoc);
	m_pCurrNodeList = NULL;
}



CIssXmlParser::~CIssXmlParser()
{
	// Note CoUninitialize has to be called only AFTER the SMART POINTER is destructed
	// which happens when the class goes out of scope.
	//CoUninitialize();
#ifdef UNDER_CE
	ASSERT(NULL == m_pDoc.p);
	ASSERT(NULL == m_pCurrNode.p);
	ASSERT(NULL == m_pCurrNodeList.p);
	ASSERT(NULL == m_pCurrChildNodeList.p);
	ASSERT(NULL == m_pSavedChildNodeList.p);
#endif
}

/********************************************************************
	Function:	Open
	
	Argument:	szData - data to be parsed
				isFile - set to TRUE to indicate that rData is a file pathname 
  //                from which the XML data will be loaded. FALSE indicates that
  //                rData itself contains the XML data.
  //            validate - TRUE to validate against document type if document
  //                type declaration (!DOCTYPE) exists in data.
  //                resolveExternals - TRUE to resolve external refs to namespaces 
  //                and document types.
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Opens a parser using either a string or a file as its subject data
*********************************************************************/
BOOL CIssXmlParser::Open( TCHAR*	szData,
						  BOOL		isFile, 
						  BOOL		validate, /* = TRUE */
						  BOOL		resolveExternals /* = TRUE */)
{
	HRESULT hr;

	//check if the file exists, otherwise return FALSE
	if(isFile)
	{
		FILE* fp;
		fp = _tfopen(szData, _T("r"));
		if(fp == NULL)
			return FALSE;
		else
			fclose(fp);
	}
	
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
	if (hr != S_OK || (m_pDoc.p == NULL)) 
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
	VARIANT_BOOL bValidate = validate ? VARIANT_TRUE : VARIANT_FALSE;
	#pragma warning(default: 4310) // cast truncates constant value

	hr = m_pDoc->put_validateOnParse(bValidate);
	if (hr != S_OK)
	{
	  return FALSE;
	}

	// Do we want to resolve external namespace/doc type references.

	#pragma warning(disable: 4310) // cast truncates constant value
	VARIANT_BOOL bResolveExternals = resolveExternals ? VARIANT_TRUE : VARIANT_FALSE;
	#pragma warning(default: 4310) // cast truncates constant value

	hr = m_pDoc->put_resolveExternals(bResolveExternals);
	if (hr != S_OK)
	{
	  return FALSE;
	}

	// Load data from appropriate source. 

	VARIANT_BOOL bSuccess = FALSE;

	if (isFile)
	{
	  hr = m_pDoc->load(CComVariant(szData) , &bSuccess);
	  if (hr != S_OK ||!bSuccess)
	  {
		return FALSE;
	  }
	}
	else
	{
	  hr = m_pDoc->loadXML(CComBSTR(szData) , &bSuccess);
	  if (hr != S_OK ||!bSuccess)
	  {
		return FALSE;
	  }
	}


	m_pCurrNode = m_pDoc;

	return TRUE;
}


/********************************************************************
	Function:	Close
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Deallocates resources and closes the parser
*********************************************************************/
void CIssXmlParser::Close()
{
	m_pDoc					= NULL;
	m_pCurrNode				= NULL;
	m_pCurrNodeList			= NULL;
	m_pCurrChildNodeList	= NULL;
	m_pSavedChildNodeList	= NULL;

	
	
}

/********************************************************************
	Function:	SetCurrentElement
	
	Argument:	szPath - node path descriptor, must be an absolute path.
						 eg. _T("//mmc//status")				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Sets the document's current element to point to the node
				identified by a string
*********************************************************************/
BOOL CIssXmlParser::SetCurrentElement(TCHAR* szPath)
{
	HRESULT hr;

	CComBSTR bstrPath(szPath);

	m_pCurrNode = NULL;
	hr = m_pDoc->selectSingleNode(bstrPath, &m_pCurrNode);
	if (hr != S_OK || (m_pCurrNode.p == NULL))
	{
	  return FALSE;
	}

	return TRUE;
}


/********************************************************************
	Function:	GetElementAttribute
	
	Argument:	szAttibute - attribute name whose value must be read
				szValue - text will be written here
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Gets the value of an attribute associated with the 
				current element
*********************************************************************/
BOOL CIssXmlParser::GetElementAttribute(TCHAR* szAttribute, TCHAR* szValue, CComPtr<IXMLDOMNode> node)
{
	HRESULT hr;
	CComVariant varValue(VT_EMPTY);

	CComBSTR bstrSS(szAttribute);

	CComQIPtr<IXMLDOMElement> pElement;
	pElement = node;

	hr = pElement->getAttribute(bstrSS, &varValue);

	pElement.Release();

	if (hr != S_OK)
	{
	  return FALSE;
	}

	if (varValue.vt == VT_BSTR)
	{
	  _tcscpy(szValue, varValue.bstrVal);
	} 
	else 
	{
	  return FALSE;
	}

	return TRUE;
}



/********************************************************************
	Function:	GetElementAttribute
	
	Argument:	szAttibute - attribute name whose value must be read
				szValue - text will be written here
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Gets the value of an attribute associated with the 
				current element
*********************************************************************/
BOOL CIssXmlParser::GetElementAttribute(TCHAR* szAttribute, TCHAR* szValue)
{
	HRESULT hr;
	CComVariant varValue(VT_EMPTY);

	CComBSTR bstrSS(szAttribute);

	CComQIPtr<IXMLDOMElement> pElement;
	pElement = m_pCurrNode;

	hr = pElement->getAttribute(bstrSS, &varValue);

	pElement.Release();

	if (hr != S_OK)
	{
	  return FALSE;
	}

	if (varValue.vt == VT_BSTR)
	{
	  _tcscpy(szValue, varValue.bstrVal);
	} 
	else 
	{
	  return FALSE;
	}

	return TRUE;
}


/********************************************************************
	Function:	GetElementAttribute
	
	Argument:	szAttribute - attribute name whose value must be read
				iInt - integer value will be written to here
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Gets the integer value of an attribute associated with the
				current element
*********************************************************************/
BOOL CIssXmlParser::GetElementAttribute(TCHAR* szAttribute,  int& iInt)
{
	TCHAR szText[255];

	if (TRUE == GetElementAttribute(szAttribute, szText))
	{
		
	  iInt = _ttoi(szText);
	  return TRUE;
	}
	else
	{
	  return FALSE;
	}
}

BOOL CIssXmlParser::GetElementAttribute(TCHAR* szAttribute,  
										LPVOID lpData,
										int	iLen)
{
	TCHAR* szText = new TCHAR[2*iLen + 1];
	memset(szText, 0, sizeof(TCHAR)*(2*iLen+1));

	CIssString* oStr = CIssString::Instance();

	if(!GetElementAttribute(szAttribute, szText))
	{
		oStr->Delete(&szText);
		return FALSE;
	}

	TCHAR szHex[STRING_SMALL];
	TCHAR** szPtr = NULL;
	byte* lpBuf = (byte*)lpData;
	BOOL	bReturn = TRUE;

	int iTextlen = oStr->GetLength(szText);
	for(int i=0; i+1 < iTextlen; i+=2)
	{
		oStr->Empty(szHex);

		// if we've gone over our buffer size
		if(i/2 > iLen)
		{
			bReturn = FALSE;
			break;
		}

		szHex[0]	= szText[i];
		szHex[1]	= szText[i+1];
		szHex[2]	= _T('\0');
		lpBuf[i/2]  = (byte)_tcstoul(szHex, szPtr, 16);
	}

	oStr->Delete(&szText);

	return bReturn;
}



/********************************************************************
	Function:	GetElementAttribute
	
	Argument:	szAttribute - attribute name whose value must be read
				iInt - integer value will be written to here
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Gets the integer value of an attribute associated with the
				current element
*********************************************************************/
BOOL CIssXmlParser::GetElementAttribute(TCHAR* szAttribute, int& iInt, CComPtr<IXMLDOMNode> node)
{
	TCHAR szText[255];

	if (TRUE == GetElementAttribute(szAttribute, szText,node))
	{
		
	  iInt = _ttoi(szText);
	  return TRUE;
	}
	else
	{
	  return FALSE;
	}
}



/********************************************************************
	Function:	SetElementList
	
	Argument:	szNode - element descriptor, must be an absolute path
				pNumOfElements - number of elements in list will be written
								 to this value
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Sets the parser to point to a list of nodes identified by a 
				string.  This becomes the current element list
*********************************************************************/
BOOL CIssXmlParser::SetElementList(TCHAR* szNode, unsigned long *pNumOfNodes)
{
	HRESULT hr;
	LONG numOfNodes;
	CComBSTR bstrSS(szNode);

	m_pCurrNodeList = NULL;
	hr = m_pDoc->selectNodes(bstrSS, &m_pCurrNodeList);
	if (hr != S_OK)
	{
	  return FALSE;
	}

	hr = m_pCurrNodeList->get_length(&numOfNodes);
	if (hr != S_OK)
	{
	  return FALSE;
	}

	*pNumOfNodes = (unsigned long) numOfNodes;

	return TRUE;
}

BOOL CIssXmlParser::SetElementList(TCHAR* szElement, ULONG& ulNumElements, CComPtr<IXMLDOMNode>& pCurNode, CComPtr<IXMLDOMNodeList>& pNodeList)
{
    if(!szElement || !pCurNode)
        return FALSE;

    LONG numOfNodes;
    CComBSTR bstrSS(szElement);

    pNodeList = NULL;
    
    HRESULT hr = pCurNode->selectNodes(bstrSS, &pNodeList);
    if (hr != S_OK || !pNodeList)
        return FALSE;

    hr = pNodeList->get_length(&numOfNodes);
    if (hr != S_OK)
        return FALSE;

    ulNumElements = numOfNodes;

    return TRUE;
}


/********************************************************************
	Function:	NextElement
	
	Argument:	
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	Sets the current element to the next element in the
				current element list.  The first call to NextElement() after
				a successful call to SetElementList() will set the current element to
				the first element in the list.
*********************************************************************/
BOOL CIssXmlParser::NextElement()
{
	HRESULT hr;

	m_pCurrNode = NULL;
	hr = m_pCurrNodeList->nextNode(&m_pCurrNode);
	if (hr != S_OK)
	{
	  return FALSE;
	}

	return TRUE;
}

/********************************************************************
	Function:	GetChildren
	
	Argument:	iNumberOfChildren - return the number of children nodes found
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	This function will search the current node and return how many children
				have been found
*********************************************************************/
BOOL CIssXmlParser::GetChildren(int* iNumberOfChildren)
{
	HRESULT hr;
	long	lReturn;

	m_pCurrChildNodeList = NULL;
	hr = m_pCurrNode->get_childNodes(&m_pCurrChildNodeList);
	if(hr != S_OK)
	{
		return FALSE;
	}

	hr = m_pCurrChildNodeList->get_length(&lReturn);
	if(hr != S_OK)
	{
		return FALSE;
	}

	*iNumberOfChildren = (int)lReturn;


	return TRUE;
}

BOOL CIssXmlParser::GetChildrenNodes(TCHAR* szNode, int* iNumberOfChildren)
{
    HRESULT hr;
    long	lReturn;

    m_pCurrNodeList = NULL;
    CComBSTR bstrNewPath(szNode);
    hr = m_pCurrNode->selectNodes(bstrNewPath, &m_pCurrNodeList);
    if(hr != S_OK)
    {
        return FALSE;
    }

    hr = m_pCurrNodeList->get_length(&lReturn);
    if(hr != S_OK)
    {
        return FALSE;
    }

    *iNumberOfChildren = (int)lReturn;


    return TRUE;
}

/********************************************************************
	Function:	GetChildIndexName
	
	Argument:	iIndex - which child to find
				szElementName - return the Node Name
				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	This function will return the name of the child node from
				the index.  This is used in conjunction with GetChildren.
				Note: You must call GetChildren() first
*********************************************************************/
BOOL CIssXmlParser::GetChildIndexName(int iIndex, TCHAR* szElementName)
{
	long lIndex;
	CComBSTR bstrSS;
	DOMNodeType nodeType;
	HRESULT hr;

	if(NULL == szElementName)
		return FALSE;

	lIndex = (long)iIndex;

	m_pCurrNode = NULL;
	hr = m_pCurrChildNodeList->get_item(lIndex, &m_pCurrNode);
	if(hr != S_OK)
	{
		return FALSE;
	}

	hr = m_pCurrNode->get_nodeType(&nodeType);
	if(hr != S_OK)
	{
		return FALSE;
	}

	if(nodeType == NODE_TEXT)
		hr = m_pCurrNode->get_text(&bstrSS);
	else
		hr = m_pCurrNode->get_nodeName(&bstrSS);
	if(hr != S_OK)
	{
		return FALSE;
	}

	_tcscpy(szElementName, bstrSS);

	return TRUE;
	
}

/********************************************************************
	Function:	SaveChildList
	
	Argument:					
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	This function will save a child list for use later.  This is used
				when you want to make calls to GetChildren() but you need to return to 
				a specific Child Level.  Note:  a call to GetChildren() should be done
				before using this function
*********************************************************************/
BOOL CIssXmlParser::SaveChildList()
{
	m_pSavedChildNodeList = m_pCurrChildNodeList;

	return TRUE;
}

/********************************************************************
	Function:	SetSavedChildList
	
	Argument:				
	
	Author:		Patrick Abadi
				Copyright Implicit Software Solutions
				www.implicitsoftware.com

	Purpose:	This function is used with SaveChildList and will return a 
				Saved Child Node List to the saved one.  Note: SaveChildList() needs
				to be called before this function.
*********************************************************************/	
BOOL CIssXmlParser::SetSavedChildList()
{
	m_pCurrChildNodeList = m_pSavedChildNodeList;

	return TRUE;
}


BOOL CIssXmlParser::GetXMLText(TCHAR* szXMLFile, int iBufferSize)
{
	CComBSTR bstrSS;
	HRESULT hr;

	hr = m_pDoc->get_xml(&bstrSS);

	if(hr != S_OK)
		return FALSE;

	_tcscpy(szXMLFile, bstrSS);


	return TRUE;
}
