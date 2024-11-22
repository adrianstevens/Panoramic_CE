#include "WndStorage.h"

CWndStorage::CWndStorage(void)
{
}

CWndStorage::~CWndStorage(void)
{
    for(int i = 0; i < MAX_STORAGECARDS; i++)
        m_oStr->Delete(&m_szStorageCards[i]);
}

BOOL CWndStorage::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Storage"));
    Refresh();
    return TRUE;
}


void CWndStorage::Refresh()
{
    Clear();
    //here is fine
    ULARGE_INTEGER iUnused, iNumBytes, iNumBytesFree;
    if(!GetDiskFreeSpaceEx(_T("\\"), &iUnused, &iNumBytes, &iNumBytesFree))
        return;

    double dbTotalMem		= (double)iNumBytes.QuadPart;//in bytes
    double dbAvalMem		= (double)iNumBytesFree.QuadPart;

    TCHAR szTemp[STRING_MAX];
    OutputSection(_T("Internal Storage"));
    m_oStr->Format(szTemp, _T("%.0f kb"), dbTotalMem/1024.0);
    OutputString(_T("Total Storage:"), szTemp);
    m_oStr->Format(szTemp, _T("%.0f kb"), dbAvalMem/1024.0);
    OutputString(_T("Total Free:"), szTemp);

    double dbPcnt = dbAvalMem/dbTotalMem*100.0;
    m_oStr->Format(szTemp, _T("%.2f %%"), dbPcnt);
    OutputString( _T("Percent Free:"), szTemp);


    int iCount = FindStorageCards();

    for(int i = 0; i < iCount; i++)
    {
        ULARGE_INTEGER iUnused, iNumBytes, iNumBytesFree;
        if(!GetDiskFreeSpaceEx(m_szStorageCards[i], &iUnused, &iNumBytes, &iNumBytesFree))
        {
            iUnused.QuadPart = 0;
            iNumBytes.QuadPart = 1;
            iNumBytesFree.QuadPart = 0;
        }

        dbTotalMem		= (double)iNumBytes.QuadPart;//in bytes
        dbAvalMem		= (double)iNumBytesFree.QuadPart;

        m_oStr->Format(szTemp, _T("%s"), m_szStorageCards[i]);
        OutputSection(szTemp);

        m_oStr->Format(szTemp, _T("%.0f kb"), dbTotalMem/1024.0);
        OutputString(_T("Total Storage:"), szTemp);
        m_oStr->Format(szTemp, _T("%.0f kb"), dbAvalMem/1024.0);
        OutputString(_T("Total Free:"), szTemp);

        double dbPcnt = dbAvalMem/dbTotalMem*100.0;
        m_oStr->Format(szTemp, _T("%.2f %%"), dbPcnt);
        OutputString( _T("Percent Free:"), szTemp);



    }


}


int CWndStorage::FindStorageCards()
{
#define CF_CARDS_FLAGS (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_TEMPORARY)
    //	ULARGE_INTEGER iUnused, iNumBytes, iNumBytesFree;
    WIN32_FIND_DATA fd;
    HANDLE hCF = FindFirstFile(_T("\\*"),&fd);

    int iStorageCards = 0;

    if (INVALID_HANDLE_VALUE == hCF) 
        return 0;
    do
    {
        if ((fd.dwFileAttributes & CF_CARDS_FLAGS) == CF_CARDS_FLAGS)
        {
            //only add it if we have some valid size information
            if(iStorageCards < MAX_STORAGECARDS)
            {
                ULARGE_INTEGER iUnused, iNumBytes, iNumBytesFree;
                if(!GetDiskFreeSpaceEx(fd.cFileName, &iUnused, &iNumBytes, &iNumBytesFree))
                {
                    iUnused.QuadPart = 0;
                    iNumBytes.QuadPart = 1;
                    iNumBytesFree.QuadPart = 0;
                }

                if(iNumBytes.QuadPart > 1) //make sure it registers some size
                {
                    m_oStr->Delete(&m_szStorageCards[iStorageCards]);
                    m_szStorageCards[iStorageCards] = m_oStr->CreateAndCopy(fd.cFileName);
                    iStorageCards++;
                }
            }
        }
    } while (FindNextFile(hCF,&fd));

    FindClose(hCF);

    return iStorageCards;
}
