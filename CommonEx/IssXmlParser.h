
#ifndef ISS_XML_PARSER_H
#define ISS_XML_PARSER_H

#include "stdafx.h"
#include <objsafe.h>
#include <msxml.h>
#include <ocidl.h>
#include <atlbase.h>


class CIssXmlParser
{
public:

	CIssXmlParser();
	CIssXmlParser(IXMLDOMNode *);
	virtual ~CIssXmlParser();

	BOOL Open(TCHAR* szData,
			  BOOL isFile, 
			  BOOL validate = TRUE,
			  BOOL resolveExternals = TRUE);

	void Close();

	BOOL SetCurrentElement(TCHAR* szPath);

	BOOL GetElementAttribute(TCHAR* szAttribute, 
							 TCHAR* szValue);

	BOOL GetElementAttribute(TCHAR* szAttribute,  
							 int&	iInt);

	BOOL GetElementAttribute(TCHAR* szAttribute,  
							 LPVOID lpData,
							 int	iLen);

	BOOL GetElementAttribute(TCHAR* szAttribute, TCHAR* szValue, CComPtr<IXMLDOMNode> node);
	BOOL GetElementAttribute(TCHAR* szAttribute, int& iInt, CComPtr<IXMLDOMNode> node);

	BOOL SetElementList(TCHAR* szElement, unsigned long *pNumOfElements);
	BOOL NextElement();

	BOOL GetChildren(int* iNumberOfChildren);
	BOOL GetChildIndexName(int iIndex, TCHAR* szElementName);
    BOOL GetChildrenNodes(TCHAR* szNode, int* iNumberOfChildren);

	BOOL SaveChildList();
	BOOL SetSavedChildList();


	BOOL GetXMLText(TCHAR* szXMLFile, int iBufferSize);
	
	CComPtr<IXMLDOMNode> GetCurrentNode(){return m_pCurrNode;};
    CComPtr<IXMLDOMNodeList> GetCurrentNodeList(){return m_pCurrNodeList;};
	CComPtr<IXMLDOMNodeList> GetCurrentChildNode(){return m_pCurrChildNodeList;};
	VOID SetCurrentNode(CComPtr<IXMLDOMNode> node){m_pCurrNode = node;};
	VOID SetCurrentChildNode(CComPtr<IXMLDOMNodeList> node){m_pCurrChildNodeList = node;};
    VOID SetCurrentNodeList(CComPtr<IXMLDOMNodeList> node){m_pCurrNodeList = node;};

    static BOOL SetElementList(TCHAR* szElement, ULONG& ulNumElements, CComPtr<IXMLDOMNode>& pCurNode, CComPtr<IXMLDOMNodeList>& pNodeList);

protected:

	CComPtr<IXMLDOMDocument> m_pDoc;		// Document which contains provision data.
	CComPtr<IXMLDOMNode> m_pCurrNode;		// Current node being accessed in DOM doc.
	// This will be of NODE_ELEMENT type.
	CComPtr<IXMLDOMNodeList> m_pCurrNodeList;// Current list of nodes.
	CComPtr<IXMLDOMNodeList> m_pCurrChildNodeList; //list of Children in a node
	CComPtr<IXMLDOMNodeList> m_pSavedChildNodeList; //list of Children in a node
};

#endif
