#include "ObjMRUList.h"
#include "IssDebug.h"
#include "IssRegistry.h"
#include "ContactsGuiDefines.h"

#define REG_Name    _T("n%d")
#define REG_Count   _T("d%d")

TypeMRU::TypeMRU()
:szLnk(NULL)
,iUsageCnt(0)
,iRegIndex(0)
,iIconIndex(0)
{}

TypeMRU::~TypeMRU()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szLnk);
}

CObjMRUList::CObjMRUList(void)
:m_oStr(CIssString::Instance())
{
}

CObjMRUList::~CObjMRUList(void)
{
    Destroy();
}

void CObjMRUList::Destroy()
{
    DeleteLinkArray();
}

void CObjMRUList::LoadRegistry()
{
    DeleteLinkArray();

    HRESULT hr = S_OK;
    TCHAR szText[STRING_MAX];
    TCHAR szTemp[STRING_MAX];
    DWORD dwTemp;
    int iRegIndex = 0;
    TypeMRU* sItem = NULL;
    while(TRUE)
    {
        m_oStr->Format(szText, REG_Name, iRegIndex);
        hr = GetKey(REG_MRU, szText, szTemp, STRING_MAX);
        if(hr != S_OK) 
        {
            hr = S_OK;  // we're still good
            break;
        }

        m_oStr->Format(szText, REG_Count, iRegIndex);
        hr = GetKey(REG_MRU, szText, dwTemp);
        if(hr != S_OK)
        {
            hr = S_OK;  // we're still good
            break;
        }

        // create a new item for our list
        sItem = new TypeMRU;
        CPHR(sItem, _T("sItem"));
        sItem->szLnk    = m_oStr->CreateAndCopy(szTemp);
        CPHR(sItem->szLnk, _T("sItem->szLnk"));
        sItem->iUsageCnt= dwTemp;
        sItem->iRegIndex= iRegIndex;

        SHFILEINFO sfi = {0};
        // get the icon index
        if (SHGetFileInfo(szTemp, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
            sItem->iIconIndex = sfi.iIcon;

        hr = m_arrLnk.AddElement(sItem);
        CHR(hr, _T("m_arrLnk.AddElement"));

        iRegIndex++;
    }

    // sort the list now
    m_arrLnk.Sort(CompareLinks);

Error:
    if(sItem && hr != S_OK)
        delete sItem;    
}

void CObjMRUList::SaveRegistry(int iRegIndex, TypeMRU* sItem)
{
    TCHAR szText[STRING_MAX];
    m_oStr->Format(szText, REG_Name, iRegIndex);
    SetKey(REG_MRU, szText, sItem->szLnk, m_oStr->GetLength(sItem->szLnk));
    m_oStr->Format(szText, REG_Count, iRegIndex);
    SetKey(REG_MRU, szText, sItem->iUsageCnt);
}

HRESULT CObjMRUList::AddLink(TCHAR* szLnk)
{
    HRESULT hr = S_OK;
    TypeMRU* sItem = NULL;
    CBARG(!m_oStr->IsEmpty(szLnk), _T(""));

    sItem = FindItem(szLnk);
    if(sItem)
    {
        // update the count and save to registry
        sItem->iUsageCnt++;
        SaveRegistry(sItem->iRegIndex, sItem);
        sItem = NULL;
        hr = m_arrLnk.Sort(CompareLinks);
        CHR(hr, _T("m_arrLnk.Sort"));
    }
    else
    {
        // create a new entry
        sItem = new TypeMRU;
        CPHR(sItem, _T("sItem"));

        sItem->szLnk = m_oStr->CreateAndCopy(szLnk);
        CPHR(sItem->szLnk, _T("sItem->szLnk"));
        sItem->iRegIndex    = m_arrLnk.GetSize();

        SHFILEINFO sfi = {0};
        // get the icon index
        if (SHGetFileInfo(szLnk, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
            sItem->iIconIndex = sfi.iIcon;

        hr = m_arrLnk.AddElement(sItem);

        // save to registry
        SaveRegistry(sItem->iRegIndex, sItem);

        // no need to sort because it's a new one
    }

Error:
    if(sItem && hr != S_OK)
        delete sItem;
    return hr;
}

TCHAR* CObjMRUList::GetLink(int iIndex)
{
    TypeMRU* sItem = m_arrLnk[iIndex];
    if(sItem)
        return sItem->szLnk;
    return NULL;
}

int CObjMRUList::GetLinkIconIndex(int iIndex)
{
    TypeMRU* sItem = m_arrLnk[iIndex];
    if(sItem)
        return sItem->iIconIndex;
    return 0;
}

void CObjMRUList::DeleteLinkArray()
{
    for(int i=0; i<m_arrLnk.GetSize(); i++)
    {
        TypeMRU* sItem = m_arrLnk[i];
        if(sItem)
            delete sItem;
    }
    m_arrLnk.RemoveAll();
}

TypeMRU* CObjMRUList::FindItem(TCHAR* szLnk)
{
    if(m_oStr->IsEmpty(szLnk))
        return NULL;

    for(int i=0; i<m_arrLnk.GetSize(); i++)
    {
        TypeMRU* sItem = m_arrLnk[i];
        if(sItem && 0 == m_oStr->Compare(szLnk, sItem->szLnk))
            return sItem;
    }
    return NULL;
}

int CObjMRUList::CompareLinks( const void *arg1, const void *arg2 )
{
    TypeMRU& sListItem1 = **(TypeMRU **)arg1;
    TypeMRU& sListItem2 = **(TypeMRU **)arg2;
    return sListItem2.iUsageCnt - sListItem1.iUsageCnt;
}
