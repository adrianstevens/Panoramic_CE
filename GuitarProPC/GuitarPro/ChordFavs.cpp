#include "ChordFavs.h"
#include "Resource.h"

CChordFavs::CChordFavs(void)
:m_iLastFound(-1)
{
}


CChordFavs::~CChordFavs(void)
{
}


BOOL CChordFavs::AddFavorite(EnumChordRoot eRoot, EnumChordType eType, EnumTuning eTuning, int iIndex,
                             UINT uiRes1, UINT uiRes2, UINT uiRes3, UINT uiRes4, UINT uiRes5, UINT uiRes6)
{
    //make sure its not already added
    if(IsFavorite(eRoot, eType, eTuning, iIndex))
        return FALSE;

    TypeFavorite* sFav = new TypeFavorite;

    sFav->eRoot = eRoot;
    sFav->eTuning = eTuning;
    sFav->eType = eType;
    sFav->iIndex = iIndex;
    sFav->uiRes[0] = uiRes1;
    sFav->uiRes[1] = uiRes2;
    sFav->uiRes[2] = uiRes3;
    sFav->uiRes[3] = uiRes4;
    sFav->uiRes[4] = uiRes5;
    sFav->uiRes[5] = uiRes6;

    m_arrFavorites.AddElement(sFav);
    return TRUE;
}

BOOL CChordFavs::IsFavorite(EnumChordRoot eRoot, EnumChordType eType, EnumTuning eTuning, int iIndex)
{
    TypeFavorite* sFav = NULL;
    for(int i = 0; i < m_arrFavorites.GetSize(); i++)
    {
        sFav = m_arrFavorites[i];

        if(sFav == NULL)
            continue;

        if(sFav->eRoot == eRoot &&
            sFav->eTuning == eTuning &&
            sFav->eType == eType &&
            sFav->iIndex == iIndex)
        {
            m_iLastFound = i;
            return TRUE;
        }
    }
    return FALSE;
}

UINT CChordFavs::GetRes(int iFav, int iString)
{
    TypeFavorite* oFav = NULL;
    oFav = m_arrFavorites[iFav];
    if(oFav == NULL)
        return 0;

    return oFav->uiRes[iString];
}

BOOL CChordFavs::DeleteLastFav()
{
    TypeFavorite* oFav = NULL;
    oFav = m_arrFavorites[m_iLastFound];

    if(oFav == NULL)
        return FALSE;

    delete oFav;
    oFav = NULL;

    m_arrFavorites.RemoveElementAt(m_iLastFound);

    return TRUE;
}

EnumChordRoot CChordFavs::GetRoot(int iFav)
{
    TypeFavorite* oFav = NULL;
    oFav = m_arrFavorites[iFav];

    if(oFav == NULL)
        return ROOT_Count;

    return oFav->eRoot;
}

EnumChordType CChordFavs::GetType(int iFav)
{
    TypeFavorite* oFav = NULL;
    oFav = m_arrFavorites[iFav];

    if(oFav == NULL)
        return CHORD_Count;

    return oFav->eType;
}

EnumChordRoot CChordFavs::GetNoteIndex(int iFav, int iString)
{
    TypeFavorite* oFav = NULL;
    oFav = m_arrFavorites[iFav];

    if(oFav == NULL)
        return ROOT_Count;

    UINT ui = GetRes(iFav, iString);
    
    if(ui == -1)
        return ROOT_Count;


    ui -= LOWEST_SOUND_RES + 3;

    while(ui > 11)
        ui -= 12;

    return (EnumChordRoot)ui;
}