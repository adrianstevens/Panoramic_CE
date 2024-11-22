#pragma once

#include "windows.h"
#include "IssVector.h"
#include "IssString.h"

#define MAX_Stars       4
#define MAX_CONST_Favs  12 //seems like a good number

struct TypeConstant
{
    TCHAR*  szName;
    TCHAR*  szConstant;
    BOOL    bIsGroup;
    int     iStartIndex;
    int     iEndIndex;

    TypeConstant();
    ~TypeConstant();
};

struct TypeFavorite
{
    TCHAR* szName;
    TCHAR* szGroup;
    TCHAR* szConstant;
    TypeFavorite();
    ~TypeFavorite();
};

class CObjConstants
{
public:
    CObjConstants(void);
    ~CObjConstants(void);

    void    Initialize();

    void    Destroy();
    HRESULT LoadConstants(HINSTANCE hInst);
    
    int     GetGroupCount(int iLevel);
    TCHAR*  GetGroupName(int iLevel, int iIndex);
    TCHAR*  GetGroupConstant(int iLevel, int iIndex);
    BOOL    IsGroupAConstant(int iLevel, int iIndex);

    TCHAR*  GetSelectedSection(int iLevel);

    BOOL    AddToFavorites();
    int     GetFavoriteCount(){return m_arrFavorites.GetSize();};
    TCHAR*  GetFavoriteName(int iIndex);
    TCHAR*  GetFavoriteGroup(int iIndex);
    TCHAR*  GetFavoriteConstant(int iIndex);

    BOOL    IsInitialized(){if(m_szData) return TRUE; return FALSE;};

    BOOL    SetGroup(int iLevel, int iIndex);

    void    SetFavString(TCHAR* szText);

private:
    HRESULT PopulateGroup(int iLevel, int iPreviousGroupIndex);
    HRESULT PopulateGroupFavs();
    HRESULT ReadLine(int& iDataIndex, char* szData, DWORD& dwDataSize, TCHAR** szLine);
    BOOL    IsGroup(TCHAR* szLine, int& iStarCount);
    void    DeleteArrays(int iStart);
    void    DeleteFavoriteArray();
    void    LoadFavorites();
    void    SaveFavorites();
    void    SetFavoriteDefaults();

    BOOL    IsFavorite(int iLevel);

private:    
    CIssString*     m_oStr;

    CIssVector<TypeConstant>  m_arrConstant[MAX_Stars];
    CIssVector<TypeFavorite>  m_arrFavorites;

    TCHAR*          m_szFavorites;//so we can localize

    TCHAR*          m_szTemp;
    DWORD           m_dwFullDataSize;
    char*           m_szData;
    int             m_iLastLevel;
    int             m_iLastIndex;

    int             m_iIndex[MAX_Stars];
};
