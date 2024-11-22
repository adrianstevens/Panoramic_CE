#include "GuiBase.h"


CGuiBase::CGuiBase(void)
:m_hWnd(NULL)
,m_hInst(NULL)
,m_bPlaySounds(TRUE)
{
    m_oStr = CIssString::Instance();
}

CGuiBase::~CGuiBase(void)
{
	m_oSoundFX.Destroy();
}

void CGuiBase::PlaySounds(TCHAR* szWave)
{
	
    BOOL bRet = FALSE;
    if(m_bPlaySounds)
        bRet = PlaySound(szWave, m_hInst, SND_RESOURCE | SND_SYNC | SND_NODEFAULT);
    //bRet = PlaySound(szWave, m_hInst, SND_RESOURCE);

    DebugOut(_T("PlaySounds\r\n"));
}

BOOL CGuiBase::DrawBackground(CIssGDIEx& gdi, RECT& rc)
{
    if(m_gdiBackground.GetDC() == NULL)
    {
        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);
        m_gdiBackground.Create(gdi.GetDC(), rcClient, FALSE);

        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IDB_PNG_Wood, m_hWnd, m_hInst);

        CIssGDIEx gdiNote;
        gdiNote.LoadImage(IDB_PNG_Note, m_hWnd, m_hInst);

        ::TileBackGround(m_gdiBackground, rcClient, gdiTemp, FALSE);

        //draw the note
        RECT rcDest = rcClient;
        rcDest.left = rcClient.right - gdiNote.GetWidth() - GetSystemMetrics(SM_CXSMICON);
        rcDest.top = rcClient.bottom - gdiNote.GetHeight() - GetSystemMetrics(SM_CXSMICON);
        Draw(m_gdiBackground, rcDest, gdiNote);
    }

    // draw the background
    BitBlt(gdi.GetDC(),
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_gdiBackground.GetDC(),
        rc.left, rc.top,
        SRCCOPY);


    return TRUE;
}