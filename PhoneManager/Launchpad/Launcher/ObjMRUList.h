#pragma once

#include "IssVector.h"
#include "IssString.h"

struct TypeMRU
{
    TCHAR*  szLnk;              // link name
    int     iUsageCnt;          // usage count
    int     iRegIndex;          // index in the registry
    int     iIconIndex;         // icon in the image list
    TypeMRU();
    ~TypeMRU();
};

class CObjMRUList
{
public:
    CObjMRUList(void);
    ~CObjMRUList(void);

    void            Destroy();
    void            LoadRegistry();
    HRESULT         AddLink(TCHAR* szLnk);
    int             GetLinkCount(){return m_arrLnk.GetSize();};
    TCHAR*          GetLink(int iIndex);
    int             GetLinkIconIndex(int iIndex);

private:    // functions
    void            SaveRegistry(int iRegIndex, TypeMRU* sItem);
    void            DeleteLinkArray();
    TypeMRU*        FindItem(TCHAR* szLnk);
    static int      CompareLinks( const void *arg1, const void *arg2 );


private:    // variables
    CIssString*         m_oStr;
    CIssVector<TypeMRU> m_arrLnk;
};
