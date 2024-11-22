#include "ObjContactImgLoader.h"
#include "IssGDIDraw.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "commondefines.h"

TypePicLoad::TypePicLoad()
:lOid(0)
,gdiImage(NULL)
{}

TypePicLoad::~TypePicLoad()
{
	Destroy();
}

void TypePicLoad::Destroy()
{
	if(gdiImage)
	{
		delete gdiImage;
		gdiImage = NULL;
	}
	lOid = 0;
}

CObjContactImgLoader::CObjContactImgLoader(void)
:m_hThread(NULL)
,m_hEvent(NULL)
,m_hWndParent(NULL)
,m_lDesiredOid(0)
,m_dwLastTick(0)
,m_bKillThread(FALSE)
{
    InitializeCriticalSection(&m_cr);
}

CObjContactImgLoader::~CObjContactImgLoader(void)
{
    Destroy();
    DeleteCriticalSection(&m_cr);
}

void CObjContactImgLoader::Destroy()
{
    if(m_hThread)
    {
        DWORD dwExitCode = STILL_ACTIVE;
		int iCount = 0;
		// turn the thread off
		m_bKillThread = TRUE;
		PulseEvent(m_hEvent);
        // give it up to 10 seconds to turn itself off
		while(dwExitCode == STILL_ACTIVE && iCount++ < 20)
		{
			if(!GetExitCodeThread(m_hThread, &dwExitCode))
				break;

			if(dwExitCode != STILL_ACTIVE)
				break;

			Sleep(500);
		}
		if(iCount >= 20)
			TerminateThread(m_hThread, 999);
          
        // memory cleanup
        CloseHandle( m_hThread );
        m_hThread = NULL;
    }

    if(m_hEvent)
    {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }

    m_bKillThread = FALSE;
    m_sPic.Destroy();
}

void CObjContactImgLoader::Initialize(int iWidth, int iHeight, HWND hWndParent, HINSTANCE hInst)
{
    Destroy();

    if(!m_imgMask.IsLoaded())
        m_imgMask.Initialize(hWndParent, hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);
    if(iWidth != m_imgMask.GetWidth() || iHeight != m_imgMask.GetHeight())
        m_imgMask.SetSize(iWidth, iHeight);

    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(hWndParent, hInst, IsVGA()?IDR_PNG_FavoritesImgBorderVGA:IDR_PNG_FavoritesImgBorder);
    if(iWidth != m_imgBorder.GetWidth() || iHeight != m_imgBorder.GetHeight())
        m_imgBorder.SetSize(iWidth, iHeight);

    m_iWidth    = iWidth;
    m_iHeight   = iHeight;
    m_hWndParent= hWndParent;
    m_pPoom     = CPoomContacts::Instance();

    m_hEvent    = CreateEvent(NULL, FALSE, FALSE, NULL);

    m_hThread   = CreateThread(NULL, 0, &ThreadLoadImage, (LPVOID)this, CREATE_SUSPENDED, NULL);
    if(m_hThread)
    {
        // make sure it runs in the background
        SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
        ResumeThread(m_hThread);
    }
}

void CObjContactImgLoader::SetContactOid(LONG lOid)
{
    // no need to reload because this will be called a lot during draw time
    if(lOid == m_lDesiredOid || !m_hWndParent)
        return;

    int iInterval = m_dwLastTick + 600 - GetTickCount();

    if(iInterval > 0)
    {
        KillTimer(m_hWndParent, IDT_LoadPic);
        SetTimer(m_hWndParent, IDT_LoadPic, iInterval, NULL);
        return;
    }

    m_dwLastTick    = GetTickCount();
    m_lDesiredOid   = lOid;    

    PulseEvent(m_hEvent);   // get the event going    
}

void CObjContactImgLoader::DrawContactPic(CIssGDIEx& gdi, long lOid, int iX, int iY)
{
    EnterCriticalSection(&m_cr);
    if(lOid != m_sPic.lOid || !m_sPic.gdiImage)
    {
        LeaveCriticalSection(&m_cr);
        SetContactOid(lOid);
        return;
    }

    Draw(gdi, iX, iY, m_iWidth, m_iHeight, *m_sPic.gdiImage, 0, 0);
    LeaveCriticalSection(&m_cr);

    return;
}

DWORD CObjContactImgLoader::ThreadLoadImage(LPVOID lpVoid)
{
    CObjContactImgLoader* pThis = (CObjContactImgLoader*)lpVoid;
    if(!pThis)
        return 0;

    return pThis->LoadContactImage();
}

DWORD CObjContactImgLoader::LoadContactImage()
{
    CPoomContacts* pPoom = CPoomContacts::Instance();
    if(!pPoom)
        return 0;

    long        lOid;
    CIssGDIEx*  gdiTemp = NULL;
    CIssGDIEx*  gdiImage= NULL;
    while(!m_bKillThread)
    {
        WaitForSingleObject(m_hEvent, INFINITE);
        DBG_OUT((_T("m_hEvent triggered")));

        if(m_bKillThread)
            break;

        EnterCriticalSection(&m_cr);
        m_sPic.Destroy();
        LeaveCriticalSection(&m_cr);

        lOid = m_lDesiredOid;
        if(!IsValidOID(lOid))
        {
            DBG_OUT((_T("lOid not valid")));
            continue;
        }

        IContact* pContact = pPoom->GetContactFromOID(lOid);
        if(!pContact)
        {
            DBG_OUT((_T("pContact not valid")));
            continue;
        }
        gdiTemp = pPoom->GetUserPicture(pContact, m_iWidth, m_iHeight, RGB(0,0,0), TRUE);
        pContact->Release();
        pContact = NULL;
        if(!gdiTemp)
            continue;

        gdiImage = new CIssGDIEx;
        if(!gdiImage)
        {
            DBG_OUT((_T("gdiImage not created")));
            delete gdiTemp;
            gdiTemp = NULL;
            continue;
        }

        RECT rc;
        SetRect(&rc, 0,0,m_iWidth, m_iHeight);
        gdiImage->Create(*gdiTemp, rc, FALSE, TRUE);
        FillRect(*gdiImage, rc, RGB(0,0,0));
        // draw it centered
        BitBlt(*gdiImage,
            0,0,
            m_iWidth, m_iHeight,
            *gdiTemp,
            (gdiTemp->GetWidth()-m_iWidth)/2, (gdiTemp->GetHeight()-m_iHeight)/2,
            SRCCOPY);
        gdiImage->SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(*gdiImage, 0,0, ALPHA_AddValue);

        EnterCriticalSection(&m_cr);
        m_sPic.Destroy();
        m_sPic.lOid     = lOid;
        m_sPic.gdiImage = gdiImage;
        LeaveCriticalSection(&m_cr);

        // tell the parent that we have a new image to draw
        PostMessage(m_hWndParent, WM_DrawContactPic, NULL, NULL);

        delete gdiTemp;
        gdiTemp = NULL;
    }

    if(gdiTemp)
    {
        delete gdiTemp;
        gdiTemp = NULL;
    }
  
    return 1;
}

HRESULT CObjContactImgLoader::FadeIn(long lOid, HDC hDC, int iX, int iY, DWORD dwTime)
{
    HRESULT hr = S_OK;
    RECT rc;
    CIssGDIEx gdiBack;
    CIssGDIEx gdiTemp;

    EnterCriticalSection(&m_cr);
    CBARG(IsValidOID(lOid) && m_sPic.lOid==lOid && m_sPic.gdiImage, _T(""));

    
    SetRect(&rc,0, 0, m_sPic.gdiImage->GetWidth(), m_sPic.gdiImage->GetHeight());
    
    gdiBack.Create(hDC, rc, FALSE, TRUE);
    gdiTemp.Create(hDC, rc, FALSE, TRUE);

    BitBlt(gdiBack.GetDC(), 0, 0, m_sPic.gdiImage->GetWidth(), m_sPic.gdiImage->GetHeight(), 
        hDC, iX, iY, SRCCOPY);

    int iAlpha = 0;

    DWORD dwStart = GetTickCount();

    while(iAlpha <= 255)
    {
        Draw(gdiTemp, rc, gdiBack, 0, 0);
        Draw(gdiTemp, rc, *m_sPic.gdiImage, 0, 0, ALPHA_Normal, iAlpha);
        BitBlt(hDC, iX, iY, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp.GetDC(), 0, 0, SRCCOPY);
        iAlpha = (GetTickCount() - dwStart)*255/dwTime;
        if(ShouldAbort(m_hWndParent))
            break;
    }

Error:
    LeaveCriticalSection(&m_cr);
    return hr;
}

BOOL CObjContactImgLoader::ShouldAbort(HWND hWnd)
{
    MSG msg;

    if (MsgWaitForMultipleObjects(0, NULL, FALSE, 5, QS_ALLINPUT) != WAIT_OBJECT_0)
        return FALSE;

    if(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
    {
        switch(LOWORD(msg.message))
        {
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_TIMER:
        case WM_PAINT:
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            return TRUE;
        }

        //	PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
    }
    return FALSE;
}
