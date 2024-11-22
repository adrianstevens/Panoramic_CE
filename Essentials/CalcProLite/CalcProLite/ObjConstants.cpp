#include "ObjConstants.h"
#include "issdebug.h"
#include "IssRegistry.h"
#include "Globals.h"

#define REG_Constants   _T("\\Constants")
#define CONST_Name      _T("CName%d")
#define CONST_Const     _T("CConst%d")

TypeConstant::TypeConstant()
:szName(NULL)
,szConstant(NULL)
,bIsGroup(FALSE)
,iStartIndex(-1)
,iEndIndex(-1)
{}

TypeConstant::~TypeConstant()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szName);
    oStr->Delete(&szConstant);
    bIsGroup = FALSE;
    iStartIndex = -1;
    iEndIndex = -1;    
}

TypeFavorite::TypeFavorite()
:szName(NULL)
,szGroup(NULL)
,szConstant(NULL)
{}

TypeFavorite::~TypeFavorite()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szName);
    oStr->Delete(&szConstant);
    oStr->Delete(&szGroup);
}

CObjConstants::CObjConstants(void)
:m_oStr(CIssString::Instance())
,m_szTemp(NULL)
,m_iLastLevel(-1)
,m_iLastIndex(-1)
{
    for(int i = 0; i < MAX_Stars; i++)
        m_iIndex[i] = -1;

    LoadFavorites();
}

CObjConstants::~CObjConstants(void)
{
    Destroy();
    DeleteFavoriteArray();
}

void CObjConstants::Destroy()
{
    DeleteArrays(0);    
    m_oStr->Delete(&m_szTemp);
}

void CObjConstants::DeleteFavoriteArray()
{
    for(int i=0; i<m_arrFavorites.GetSize(); i++)
    {
        TypeFavorite* sConst = m_arrFavorites[i];
        if(sConst)
            delete sConst;
    }
    m_arrFavorites.RemoveAll();
}

void CObjConstants::LoadFavorites()
{
    DeleteFavoriteArray();

    TCHAR szKey[STRING_MAX];
    TCHAR szName[STRING_MAX];
    TCHAR szConstant[STRING_MAX];
    TCHAR szTemp[STRING_MAX];

    m_oStr->StringCopy(szKey, REG_KEY);
    m_oStr->Concatenate(szKey, REG_Constants);

    for(int i=0; i<MAX_CONST_Favs; i++)
    {
        m_oStr->Format(szTemp, CONST_Name, i);
        if(S_OK != GetKey(szKey, szTemp, szName, STRING_MAX))
            break;

        m_oStr->Format(szTemp, CONST_Const, i);
        if(S_OK != GetKey(szKey, szTemp, szConstant, STRING_MAX))
            break;

        TypeFavorite* sConst = new TypeFavorite;
        if(!sConst)
            break;

        int iIndex = m_oStr->FindLastOccurance(szName, _T("->"));
        if(iIndex == -1)
        {
            delete sConst;
            break;
        }
        sConst->szName      = m_oStr->CreateAndCopy(szName, iIndex + 2, m_oStr->GetLength(szName) - iIndex - 2);
        sConst->szGroup     = m_oStr->CreateAndCopy(szName, 0, iIndex);
        sConst->szConstant  = m_oStr->CreateAndCopy(szConstant);
        m_arrFavorites.AddElement(sConst);
    }  

    // put something in there
    if(m_arrFavorites.GetSize() == 0)
        SetFavoriteDefaults();
}

void CObjConstants::SaveFavorites()
{
    TCHAR szKey[STRING_MAX];
    TCHAR szTemp[STRING_MAX];
    TCHAR szName[STRING_MAX];

    m_oStr->StringCopy(szKey, REG_KEY);
    m_oStr->Concatenate(szKey, REG_Constants);

    // blow away the old values first
    RegDeleteKeyNT(HKEY_CURRENT_USER, szKey);

    // delete the old keys first
    for(int i=0; i<m_arrFavorites.GetSize(); i++)
    {
        TypeFavorite* sConst = m_arrFavorites[i];
        if(!sConst)
            break;

        m_oStr->StringCopy(szName, sConst->szGroup);
        m_oStr->Concatenate(szName, _T("->"));
        m_oStr->Concatenate(szName, sConst->szName);
        m_oStr->Format(szTemp, CONST_Name, i);
        if(S_OK != SetKey(szKey, szTemp, szName, m_oStr->GetLength(szName)))
            break;

        m_oStr->Format(szTemp, CONST_Const, i);
        if(S_OK != SetKey(szKey, szTemp, sConst->szConstant, m_oStr->GetLength(sConst->szConstant)))
            break;
    }
}

void CObjConstants::SetFavoriteDefaults()
{
    TypeFavorite* sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Elementary charge (C)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Electromagnetic"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("1.60217648740e-019"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Electron mass (kg)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Atomic and Nuclear"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("9.1093821545e-031"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Proton mass (kg)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Atomic and Nuclear"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("1.67262163783e-027"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Neutron mass (kg)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Atomic and Nuclear"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("1.6749286e-027"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Standard gravity (m/s²)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Common Values"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("9.80665"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Speed of sound (m/s)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Common Values"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("343"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Speed of light (m/s)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Universal"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("299792458"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Gravitation (m³/kg·s²)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Universal"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("6.6742867e-011"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Planck (J·s)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Universal"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("6.6260689633e-034"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Gas constant R (J/°K mol)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Physico-chemical"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("8.31447215"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Boltzmann k (J/K)"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Physico-chemical"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("1.3806505e-023"));
    m_arrFavorites.AddElement(sConst);

    sConst = new TypeFavorite;
    sConst->szName      = m_oStr->CreateAndCopy(_T("Avagadro's number"));
    sConst->szGroup     = m_oStr->CreateAndCopy(_T("Physics->Physico-chemical"));
    sConst->szConstant  = m_oStr->CreateAndCopy(_T("6.0221367e+023"));
    m_arrFavorites.AddElement(sConst);

    SaveFavorites();
}

void CObjConstants::DeleteArrays(int iStart)
{
    for(int i=iStart; i<MAX_Stars; i++)
    {
        for(int j=0; j<m_arrConstant[i].GetSize(); j++)
        {
            TypeConstant* sConst = m_arrConstant[i][j];
            if(sConst)
                delete sConst;
        }
        m_arrConstant[i].RemoveAll();
    }
}

HRESULT CObjConstants::LoadConstants(HINSTANCE hInst)
{
    Destroy();

    HRESULT hr          = S_OK;
    HRSRC hrsrc         = NULL;
    HGLOBAL hResource   = NULL;

    // try and load the resource, NOTE: all resource images have to be kept in the "GIF" section
    hrsrc = FindResource(hInst, _T("IDR_TXT_Constants"), TEXT("TXT"));
    CPHR(hrsrc, _T("FindResource"));

    hResource = LoadResource(hInst, hrsrc);
    CPHR(hResource, _T("LoadResource"));

    // Get a pointer to the data
    m_szData = (char *)LockResource(hResource);
    CPHR(m_szData, _T("szData"));

    m_dwFullDataSize = SizeofResource(hInst, hrsrc);
    CBARG(m_dwFullDataSize>0, _T("SizeofResource"));

    m_szTemp = new TCHAR[STRING_MAX];
    CPHR(m_szTemp, _T("m_szTemp"));

Error:
    if(hr != S_OK)
        Destroy();
    return hr;
}

HRESULT CObjConstants::PopulateGroup(int iLevel, int iPreviousGroupIndex)
{
    HRESULT hr = S_OK;
    TCHAR* szLine = NULL;
    CBARG(iLevel >= 0 && iLevel < MAX_Stars, _T(""));

    m_iLastLevel    = -1;
    m_iLastIndex    = -1;

     // start a new
    DeleteArrays(iLevel);

    int iStartIndex = 0;
    int iEndIndex = m_dwFullDataSize;

    // make sure that the group previously actually has something
    if(iLevel > 0)
    {
        CBARG(m_arrConstant[iLevel-1].GetSize() > 0, _T(""));
        CBARG(iPreviousGroupIndex>=0 && iPreviousGroupIndex<m_arrConstant[iLevel-1].GetSize(), _T(""));

        TypeConstant* sConst = m_arrConstant[iLevel-1][iPreviousGroupIndex];
        CPHR(sConst, _T("sConst"));
        iStartIndex = sConst->iStartIndex;
        iEndIndex   = sConst->iEndIndex;
    }

    char szGroups[STRING_LARGE];
    m_oStr->StringCopy(m_szTemp, _T("GROUP"));
    for(int i=0; i<iLevel+1; i++)
    {
        m_oStr->Insert(m_szTemp, _T("*"));
        m_oStr->Concatenate(m_szTemp, _T("*"));
    }
    m_oStr->Concatenate(m_szTemp, _T("\r\n"));
    m_oStr->StringCopy(szGroups, m_szTemp);

    int iCurrentIndex = m_oStr->Find(m_szData, szGroups, iStartIndex);
    if(iCurrentIndex == -1 || iCurrentIndex >= iEndIndex)
    {
        // try and read some actual constants
        iCurrentIndex = iStartIndex;
        while(S_OK == ReadLine(iCurrentIndex, m_szData, m_dwFullDataSize, &szLine))
        {
            int iNewStarCount = 0;
            // we don't want to read groups here
            if(IsGroup(szLine, iNewStarCount))
                break;
            
            // we are an item so read two lines worth of data
            TCHAR* szConstant = NULL;
            hr = ReadLine(iCurrentIndex, m_szData, m_dwFullDataSize, &szConstant);
            CHR(hr, _T("ReadLine for szConstant"));
            TypeConstant* sNewItem = new TypeConstant;
            if(!sNewItem)
            {
                m_oStr->Delete(&szConstant);
                CPHR(sNewItem, _T("sNewItem"));
            }
            sNewItem->szName    = szLine;
            sNewItem->szConstant= szConstant;
            m_arrConstant[iLevel].AddElement(sNewItem);

        }
    }
    else
    {
        // read in some groups
        while(iCurrentIndex != -1 && iCurrentIndex < iEndIndex)
        {
            TCHAR* szGroupName = NULL;
            iCurrentIndex   = iCurrentIndex + 5 + 2*(iLevel + 1) + 2;
            hr = ReadLine(iCurrentIndex, m_szData, m_dwFullDataSize, &szGroupName);
            CHR(hr, _T("ReadLine for szGroupName"));

            TypeConstant* sNewGroup = new TypeConstant;
            if(!sNewGroup)
            {
                m_oStr->Delete(&szGroupName);
                CPHR(sNewGroup, _T("sNewGroup"));
            }
            sNewGroup->szName       = szGroupName;
            sNewGroup->bIsGroup     = TRUE;
            sNewGroup->iStartIndex  = iCurrentIndex;
            sNewGroup->iEndIndex    = iEndIndex;
            m_arrConstant[iLevel].AddElement(sNewGroup);

            if(m_arrConstant[iLevel].GetSize() > 1)
            {
                // set the proper size for the previous index
                TypeConstant* sPrevious = m_arrConstant[iLevel][m_arrConstant[iLevel].GetSize()-2];
                CPHR(sPrevious, _T("sPrevious"));
                sPrevious->iEndIndex    = iCurrentIndex - 1;
            }

            iCurrentIndex = m_oStr->Find(m_szData, szGroups, iCurrentIndex);
        }
    }

Error:
    if(hr != S_OK && szLine)
        m_oStr->Delete(&szLine);

    return hr;
}

int CObjConstants::GetGroupCount(int iLevel)
{
    if(iLevel < 0 || iLevel >= MAX_Stars)
        return 0;

    if(iLevel == 0)
    {   //kick it up when we need it
        if(m_arrConstant[iLevel].GetSize() == 0)
           PopulateGroup(0,0);
    }

    return m_arrConstant[iLevel].GetSize();
}

TCHAR* CObjConstants::GetGroupName(int iLevel, int iIndex)
{
    if(iLevel < 0 || iLevel >= MAX_Stars)
        return NULL;

    TypeConstant* sConst = m_arrConstant[iLevel][iIndex];
    if(!sConst)
        return NULL;
    return sConst->szName;
}

TCHAR* CObjConstants::GetGroupConstant(int iLevel, int iIndex)
{
    if(iLevel < 0 || iLevel >= MAX_Stars)
        return NULL;

    TypeConstant* sConst = m_arrConstant[iLevel][iIndex];
    if(!sConst || sConst->szConstant == NULL)
        return NULL;

    // save the last entry so we can add it to our favorites after
    m_iLastIndex    = iIndex;
    m_iLastLevel    = iLevel;

    return sConst->szConstant;
}

TCHAR* CObjConstants::GetSelectedSection(int iLevel)
{
    if(iLevel < 0 || iLevel > 2)
        return NULL;

    TypeConstant* sConst = m_arrConstant[iLevel][m_iIndex[iLevel]];

    if(sConst == NULL || sConst->szName == NULL)
        return NULL;

    return sConst->szName;
}

BOOL CObjConstants::IsGroupAConstant(int iLevel, int iIndex)
{
    if(iLevel < 0 || iLevel >= MAX_Stars)
        return FALSE;

    TypeConstant* sConst = m_arrConstant[iLevel][iIndex];
    if(!sConst)
        return FALSE;
    return !sConst->bIsGroup;
}

HRESULT CObjConstants::ReadLine(int& iDataIndex, char* szData, DWORD& dwDataSize, TCHAR** szLine)
{
    HRESULT hr = S_OK;

    if(iDataIndex == dwDataSize)
        return E_FAIL;

    int iIndex = -1;
    // so we skip multiple \n
    while(TRUE)
    {
        iIndex = m_oStr->Find(szData, "\n", iDataIndex);
        if(iIndex == -1)
            return E_FAIL;

        if(iIndex != iDataIndex)
            break;

        iDataIndex++;
        if(iDataIndex >= (int)dwDataSize)
            return E_FAIL;
    }

    m_oStr->StringCopy(m_szTemp, szData, iDataIndex, iIndex-iDataIndex);
    m_oStr->Trim(m_szTemp);
    *szLine  = m_oStr->CreateAndCopy(m_szTemp);
    CPHR(szLine, _T("szLine"));

    iDataIndex = iIndex + 1;

Error:
    return hr;
}

BOOL CObjConstants::IsGroup(TCHAR* szLine, int& iStarCount)
{
    if(!szLine)
        return FALSE;

    BOOL bRet = FALSE;
    if(m_oStr->GetFirst(szLine) == _T('*') && m_oStr->GetLast(szLine) == _T('*'))
    {
        bRet = TRUE;
        iStarCount = 0;
        for(int i=0; i<m_oStr->GetLength(szLine); i++)
        {
            if(szLine[i] != _T('*'))
                break;
            iStarCount++;
        }
    }
    return bRet;
}

BOOL CObjConstants::AddToFavorites()
{
    if(m_iLastLevel == -1 || m_iLastIndex == -1 || !IsGroupAConstant(m_iLastLevel, m_iLastIndex))
        return FALSE;

    TypeConstant* sFavorite = m_arrConstant[m_iLastLevel][m_iLastIndex];
    if(!sFavorite|| m_oStr->IsEmpty(sFavorite->szName))
        return FALSE;

    TCHAR szGroup[STRING_MAX] = _T("");

    // build the name here
    for(int i=0; i<m_iLastLevel; i++)
    {
        TypeConstant* sName = m_arrConstant[i][m_iIndex[i]];
        if(!sName || m_oStr->IsEmpty(sName->szName))
            return FALSE;

        if(i!=0)
            m_oStr->Concatenate(szGroup, _T("->"));

        m_oStr->Concatenate(szGroup, sName->szName);
        
    }

    // first check if the favorite is already in the list)
    for(int i=0; i<m_arrFavorites.GetSize(); i++)
    {
        TypeFavorite* sConst = m_arrFavorites[i];
        if(!sConst)
            continue;

        if(m_oStr->Compare(sConst->szName, sFavorite->szName) == 0 && m_oStr->Compare(sConst->szGroup, szGroup) == 0)
            return TRUE;
    }

    TypeFavorite* sNew = new TypeFavorite;
    sNew->szName    = m_oStr->CreateAndCopy(sFavorite->szName);
    sNew->szGroup   = m_oStr->CreateAndCopy(szGroup);
    sNew->szConstant= m_oStr->CreateAndCopy(sFavorite->szConstant);
    m_arrFavorites.AddElement(sNew);
    m_arrFavorites.Move(m_arrFavorites.GetSize()-1, 0);

    if(m_arrFavorites.GetSize() > MAX_CONST_Favs)
    {
        TypeFavorite* sDel = m_arrFavorites[MAX_CONST_Favs];
        delete sDel;
        m_arrFavorites.RemoveElementAt(MAX_CONST_Favs);
    }

    // save to registry
    SaveFavorites();
    return TRUE;
}

TCHAR* CObjConstants::GetFavoriteName(int iIndex)
{
    TypeFavorite* sConst = m_arrFavorites[iIndex];
    if(!sConst)
        return NULL;

    return sConst->szName;
}

TCHAR* CObjConstants::GetFavoriteGroup(int iIndex)
{
    TypeFavorite* sConst = m_arrFavorites[iIndex];
    if(!sConst)
        return NULL;

    return sConst->szGroup;
}

TCHAR* CObjConstants::GetFavoriteConstant(int iIndex)
{
    TypeFavorite* sConst = m_arrFavorites[iIndex];
    if(!sConst)
        return NULL;

    return sConst->szConstant;
}

BOOL CObjConstants::SetGroup(int iLevel, int iIndex)
{
    //some basic bounds checking
    if(iLevel < 0 || iLevel >= MAX_Stars)
        return FALSE;
    if(iIndex < 0)
        return FALSE;

    //make sure our previous array has values
    if(iLevel > 0 && (m_iIndex[iLevel - 1] == -1 || m_arrConstant[iLevel-1].GetSize() == 0))
        return FALSE;


    for(int i=iLevel; i<MAX_Stars; i++)
        m_iIndex[i] = -1;

    m_iIndex[iLevel] = iIndex;

    // populate the next box
    if(iLevel < MAX_Stars + 1)
        PopulateGroup(iLevel + 1, iIndex);

    return TRUE;

}