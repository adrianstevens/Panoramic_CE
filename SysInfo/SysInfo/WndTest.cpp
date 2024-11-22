#include "WndTest.h"
#include "IssGDIEx.h"

//HKCU\Software\Microsoft\Shell\HasKeyboard

CWndTest::CWndTest(void)
{
    m_iMouseCount = 0;
    m_iMouseIndex = 0;

    m_dwCharWParam = 0;
    m_dwCharLParam = 0;

    m_dwKeyDownWParam = 0;
    m_dwKeyDownLParam = 0;

    m_dwKeyUpWParam = 0;
    m_dwKeyUpLParam = 0;

    m_ptMouseArray[0].x = -1;
    m_ptMouseArray[0].y = -1;

    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
}

CWndTest::~CWndTest(void)
{
    CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CWndTest::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Test Hardware"));
    return TRUE;
}

BOOL CWndTest::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    DrawBackground(*m_gdiMem, rcClip);

    if(IsRectInRect(rcClip, m_rcTitle))
    {
        DrawTextShadow(*m_gdiMem, m_szName, m_rcTitle, DT_CENTER|DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), 0);
    }

    TCHAR szTemp[STRING_LARGE];
    TCHAR szSmall[STRING_SMALL];
    RECT rcTemp = rcClient;

    rcTemp.left += GetSystemMetrics(SM_CXSMICON);
    rcTemp.right -= GetSystemMetrics(SM_CXSMICON);



    rcTemp.top += GetSystemMetrics(SM_CXICON);
    m_oStr->StringCopy(szTemp, _T("WM_KEYDOWN"));
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    m_oStr->Format(szTemp, _T("wparam: %u"), m_dwKeyDownWParam, szSmall);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);
    
    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    m_oStr->Format(szTemp, _T("lparam: %u"), m_dwKeyDownLParam);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    
    rcTemp.top += GetSystemMetrics(SM_CXICON);
    m_oStr->StringCopy(szTemp, _T("WM_KEYUP"));
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    m_oStr->Format(szTemp, _T("wparam: %u"), m_dwKeyUpWParam, szSmall);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    m_oStr->Format(szTemp, _T("lparam: %u"), m_dwKeyUpLParam);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);


    rcTemp.top = GetSystemMetrics(SM_CXICON) + rcClient.top;
    rcTemp.left = rcClient.left + WIDTH(rcClient)/2;
    m_oStr->StringCopy(szTemp, _T("WM_CHAR"));
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    szSmall[0] = (TCHAR)m_dwCharWParam;
    szSmall[1] = _T('\0');
    m_oStr->Format(szTemp, _T("wparam: %u (%s)"), m_dwCharWParam, szSmall);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    rcTemp.top += GetSystemMetrics(SM_CXSMICON);
    m_oStr->Format(szTemp, _T("lparam: %u"), m_dwCharLParam);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);



    int iX, iY;
    if(m_iMouseCount == 0)
    {
        iX = -1;
        iY = -1;
    }
    else if(m_iMouseIndex == 0)
    {
        iX = m_ptMouseArray[MARRAY_SIZE - 1].x;
        iY = m_ptMouseArray[MARRAY_SIZE - 1].y;
    }
    else
    {
        iX = m_ptMouseArray[m_iMouseIndex-1].x;
        iY = m_ptMouseArray[m_iMouseIndex-1].y;
    }

    rcTemp.top += GetSystemMetrics(SM_CXICON);
    
    m_oStr->Format(szTemp, _T("(%i, %i)"), iX, iY);
    DrawText(m_gdiMem->GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hFont, 0xFFFFFF);

    DrawMouseTrail(m_gdiMem->GetDC());

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CWndTest::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_dwCharWParam = wParam;
    m_dwCharLParam = lParam;
    return TRUE;
}

BOOL CWndTest::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_dwKeyDownWParam = wParam;
    m_dwKeyDownLParam = lParam;
    return TRUE;
}

BOOL CWndTest::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_dwKeyUpWParam = wParam;
    m_dwKeyUpLParam = lParam;
    
    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

BOOL CWndTest::OnMouseMove(HWND hWnd, POINT& pt)
{
    m_ptMouseArray[m_iMouseIndex] = pt;
    m_iMouseIndex++;
    if(m_iMouseIndex >= MARRAY_SIZE)
        m_iMouseIndex = 0;

    if(m_iMouseCount < MARRAY_SIZE)
        m_iMouseCount++;

    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

BOOL CWndTest::DrawMouseTrail(HDC hDC)
{
    HPEN hPenOld;
    HPEN hPenNew = CreatePen(0, 0, 0xFF7777);
    hPenOld = (HPEN)SelectObject(hDC, hPenNew);

    if(m_iMouseCount < MARRAY_SIZE)
    {
        MoveToEx(hDC, m_ptMouseArray[0].x, m_ptMouseArray[0].y, NULL);

        for(int i = 1; i < m_iMouseCount; i++)
            LineTo(hDC, m_ptMouseArray[i].x, m_ptMouseArray[i].y);
        
    }
    else //array is full so loop around
    {
        //always the oldest point
        MoveToEx(hDC, m_ptMouseArray[m_iMouseIndex].x, m_ptMouseArray[m_iMouseIndex].y, NULL);

        for(int i = m_iMouseIndex + 1; i < MARRAY_SIZE; i++)
            LineTo(hDC, m_ptMouseArray[i].x, m_ptMouseArray[i].y);
        
        for(int i = 0; i < m_iMouseIndex; i++)
            LineTo(hDC, m_ptMouseArray[i].x, m_ptMouseArray[i].y);

        

    }

    SelectObject(hDC, hPenOld);
    DeleteObject(hPenNew);

    return TRUE;
}