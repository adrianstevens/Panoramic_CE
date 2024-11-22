

#ifndef __ISS_XML_BUILDER_H__
#define __ISS_XML_BUILDER_H__

#include <objsafe.h>
#include <msxml.h>
#include <ocidl.h>
#include <atlbase.h>
#include "IssString.h"

#if !defined(TBD_DEBUG)
	#define TBD_DEBUG
	#define TBD_OUTPUT 0

	enum TBD_CLASS_TYPE { TBDCT_NULL = 0, TBDCT_ALL, TBDCT_ALL_FUNCTIONS, TBDCT_ALL_MEMORY, TBDCT_INFO, TBDCT_READ, TBDCT_LAST };
	// Valid tracing levels
	enum TBD_OUTPUT_LEVEL { TBDOL_TRACE = 0, TBDOL_INFORMATION, TBDOL_ERROR, TBDOL_NONE };
	// Class-type used to display output regardless of other settings
	#define TBDCT_ALWAYS TBDCT_LAST
#endif


class CIssXmlBuilder
{
public:
	CIssXmlBuilder();
	virtual ~CIssXmlBuilder();

	BOOL Open();
	BOOL Open(TCHAR* szXMLFile);
	void Close();
	BOOL InsertXmlHeader();
	BOOL SetCurrentElementinList(LPCTSTR szPath, int iIndex, LPCTSTR szExtraPath = NULL);
	BOOL SetCurrentElement(LPCTSTR szPath);
	BOOL CreateElement(LPCTSTR szName);
	BOOL CreateTextElement(LPCTSTR szName);
	BOOL AddAttribute(LPCTSTR szName, TCHAR* szValue);
	BOOL AddAttribute(LPCTSTR szName, LPVOID lpData, int iLen);
	BOOL AddAttribute(LPCTSTR szName,  int& iInt);
	BOOL InsertChildElement();
	BOOL InsertChildElement(CComPtr<IXMLDOMNode> pNodeToInsert);
	BOOL SaveToFile(LPCTSTR szFileName);
	BOOL InsertProcessingInstruction(LPCTSTR szTarget, LPCTSTR szInstruction);

	CComPtr<IXMLDOMNode> GetCurrentNode();
    CComPtr<IXMLDOMNode> GetConstructionNode(){return m_pConstructionNode;};
    CComPtr<IXMLDOMDocument> GetDocument(){return m_pDoc;};
     VOID SetConstructionNode(CComPtr<IXMLDOMNode> node){m_pConstructionNode = node;};
	

protected:

	CComPtr<IXMLDOMDocument> m_pDoc;		// Document which will contain data.
	CComPtr<IXMLDOMNode> m_pCurrNode;		// Current node being accessed in DOM doc.
											// This will be of NODE_ELEMENT type.
	CComPtr<IXMLDOMNode> m_pConstructionNode;// Current element under construction.
											// This will be of NODE_ELEMENT type.
	char* m_pXmlAscii;
};

#endif //__ISS_XML_BUILDER_H__