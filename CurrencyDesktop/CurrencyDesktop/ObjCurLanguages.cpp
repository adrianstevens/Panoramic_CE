#include "ObjCurLanguages.h"
#include "IssCommon.h"
#include "issdebug.h"

TypeLanItem::TypeLanItem()
:szCode(NULL)
{
	for(int i=0; i<L_Count; i++)
	{
		szText[i] = NULL;
	}
}

TypeLanItem::~TypeLanItem()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szCode);
	for(int i = 0; i<L_Count; i++)
	{
		oStr->Delete(&szText[i]);
	}
}

CObjCurLanguages::CObjCurLanguages(void)
:m_oStr(CIssString::Instance())
{
}

CObjCurLanguages::~CObjCurLanguages(void)
{
	for(int i=0; i<m_arrCodes.GetSize(); i++)
	{
		TypeLanItem* sItem = m_arrCodes[i];
		if(sItem)
			delete sItem;
	}
	m_arrCodes.RemoveAll();
}

int CObjCurLanguages::CompareNameItems(const void* lp1, const void* lp2)
{
	if(!lp1 || !lp2)
		return 0;

	TypeLanItem* sItem1 = (TypeLanItem*)lp1;
	TypeLanItem* sItem2 = (TypeLanItem*)lp2;
	CIssString* oStr = CIssString::Instance();

	return oStr->CompareNoCase(sItem1->szCode, sItem2->szCode);
}

HRESULT CObjCurLanguages::Initialize(HINSTANCE hInst)
{
	HRESULT hr = S_OK;
	TCHAR* szCharFile = NULL;
	//FILE *fp = NULL;
	TypeLanItem* sNew = NULL;

	DBG_OUT((_T("CObjCurLanguages::Initialize()")));

	HRSRC hres = FindResource(hInst, _T("LANGUAGES"), _T("TXT"));
	CPHR(hres, _T("FindResource failed"));

	//If resource is found a handle to the resource is returned
	//now just load the resource
	HGLOBAL    hbytes = LoadResource(hInst, hres);
	CPHR(hbytes, _T("LoadResource failed"));

	// Lock the resource
	LPVOID pdata    = LockResource(hbytes);
	szCharFile  = (TCHAR*)pdata;//unicode now becuase of japanese
	CPHR(szCharFile, _T("LockResource failed"));

	DWORD dwFileSize = m_oStr->GetLength(szCharFile);

	/*fp = _tfopen(_T("LANGUAGES"), _T("r"));
	CPHR(fp, _T("fp = _tfopen(szFileName, _T(\"r\")));"));

	//read file stuff
	DWORD dwFileSize = 0;
	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	CBARG(dwFileSize > 0, _T(""));

	szCharFile = new char[dwFileSize+1];
	CPHR(szCharFile, _T("szCharFile = new char[dwFileSize+1];"));
	ZeroMemory(szCharFile, sizeof(char)*(dwFileSize+1));

	fread(szCharFile, sizeof(char), dwFileSize, fp);*/

	int iIndex = 0;
	int iLineEnd = 0;
	int iComma = 0;
	int iComma2 = 0;

	while (iIndex < (int)dwFileSize)
	{
		iLineEnd = m_oStr->Find(szCharFile, _T("\r\n"), iIndex);
		if(iLineEnd == -1)
			iLineEnd = dwFileSize - 1;

		iComma	= m_oStr->Find(szCharFile, _T(","), iIndex);
		CBARG(iComma > 0 && iComma < iLineEnd, _T(""));

		sNew = new TypeLanItem;
		CPHR(sNew, _T("sNew = new TypeLanItem;"));

		sNew->szCode = m_oStr->CreateAndCopy(szCharFile, iIndex, iComma-iIndex);
		CPHR(sNew->szCode, _T("sNew->szCode = m_oStr->CreateAndCopy(szCharFile, iIndex, iComma-iIndex);"));

		iComma++;

		for(int i=0; i< (int)L_Count; i++)
		{
			iComma2 = m_oStr->Find(szCharFile, _T(","), iComma);
			if(i == (int)L_Count-1)
				iComma2 = iLineEnd;
			CBARG(iComma2 > 0 && iComma2 <= iLineEnd, _T(""));

			sNew->szText[i] = m_oStr->CreateAndCopy(szCharFile, iComma, iComma2-iComma);
			CPHR(sNew->szText[i], _T("sNew->szText[i] = m_oStr->CreateAndCopy(szCharFile, iComma, iComma2-iComma);"));

			iComma = iComma2+1;
		}

		hr = m_arrCodes.AddSortedElement(sNew, CompareNameItems);
		CHR(hr, _T("hr = m_arrCodes.AddElement(sNew);"));

		sNew = NULL;

		iIndex = iLineEnd + 2;
	}


Error:
	/*if(fp)
		fclose(fp);*/

	if(sNew)
		delete sNew;

	DBG_OUT((_T("CObjCurLanguages::Initialize() - Finished, %d items found"), m_arrCodes.GetSize()));

	return hr;
}

TypeLanItem* CObjCurLanguages::FindItem(TCHAR* szCode)
{
	TypeLanItem* sSearch = new TypeLanItem;
	if(!sSearch)
		return NULL;

	sSearch->szCode = m_oStr->CreateAndCopy(szCode);
	if(!sSearch->szCode)
		return NULL;

	TypeLanItem* sFound = (TypeLanItem*)m_arrCodes.SearchSortedArray(sSearch, CompareNameItems);

	delete sSearch;

	return sFound;
}
