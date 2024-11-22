#include "WndBenchmark.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"

#define DRAWLOOP_TIMER      1

#define POINTS_PER_TEST     500

#define NUMBER_OF_TEST_FRAMES   500

CWndBenchmark::CWndBenchmark(void)
{
    m_uiIntTime = 0;
    m_uiMemoryTime = 0;
    m_uiFileIOTime = 0;
    m_uiFloatTime = 0;
    m_uiMemoryTime = 0;
    m_uiFractalTime = 0;
    m_dbFrameRate = 0.0;

    m_iScore    = 0;
    m_iHiScore  = 0;

    m_bTesting = FALSE;
}

CWndBenchmark::~CWndBenchmark(void)
{
}

BOOL CWndBenchmark::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CWndInfoEdit::OnInitDialog(hWnd, wParam, lParam);
    m_oStr->StringCopy(m_szName, _T("Benchmark"));

    m_oTank.Initialize(2);//tank I think
    m_oTank.SetPosition(0,0,10);

    TCHAR szTemp[STRING_SMALL];
    m_oStr->IntToString(szTemp, m_iHiScore);
    OutputString(_T("Best Score:"), szTemp);

    return TRUE;
}

BOOL CWndBenchmark::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    CWndInfoEdit::OnPaint(hWnd, hDC, rcClient);

    RECT rcTemp = rcClient;
    rcTemp.bottom -= GetSystemMetrics(SM_CXSMICON)/2;

    //draw bottom text
    DrawTextShadow(hDC, _T("Press 'Refresh' to start benchmark"), rcTemp, DT_CENTER|DT_BOTTOM, m_hFontBtnLabel, RGB(255,255,255), 0);

    return TRUE;
}

void CWndBenchmark::Refresh()
{
    Clear();
    
    HDC dc = GetDC(m_hWnd);
    RECT rcClient;
    
    GetClientRect(m_hWnd, &rcClient);

    FillRect(dc, rcClient, 0xFFFFFF);

    rcClient.top += HEIGHT(rcClient)/3;

    DrawText(dc, _T("Benchmark in progress\r\n\r\nPlease be patient\r\n\r\nAverage completion time is ~20s"), rcClient, DT_CENTER | DT_TOP);


    ReleaseDC(m_hWnd, dc);

  //  FractalBM();
    IntegerBM();
    FloatingPointBM();
    MemoryBM();
    FileIOBM();
    IntegerDraw();
    FillRateBM();
    

    TCHAR szTemp[STRING_MAX];

    m_oStr->Format(szTemp, _T("%ums"), m_uiIntTime);
    OutputString(_T("Integer:"), szTemp);
    m_oStr->Format(szTemp, _T("%ums"), m_uiFloatTime);
    OutputString(_T("Floating Point:"), szTemp);
    m_oStr->Format(szTemp, _T("%ums"), m_uiMemoryTime);
    OutputString(_T("Memory:"), szTemp);
//    m_oStr->Format(szTemp, _T("File IO: %u"), m_uiFileIOTime);
//    OutputString(szTemp);
//    m_oStr->Format(szTemp, _T("Fill Rate: %u"), m_uiFillRateTime);
//    OutputString(szTemp);
    m_oStr->Format(szTemp, _T("%0.2ffps"), m_dbFrameRate);
    OutputString(_T("Framerate:"), szTemp);
    double dbTemp = m_uiFillRate/1000000.0;
    m_oStr->Format(szTemp, _T("%.2f Mpix/s"), dbTemp);
    OutputString(_T("Fillrate:"), szTemp);

    m_iScore = 0;
    m_iScore += POINTS_PER_TEST*400/m_uiIntTime;
    m_iScore += POINTS_PER_TEST*600/m_uiFloatTime;
    m_iScore += POINTS_PER_TEST*1400/m_uiMemoryTime;
    m_iScore += POINTS_PER_TEST*(int)(m_dbFrameRate/100.0);
    m_iScore += POINTS_PER_TEST*m_uiFillRate/10000000;

    m_oStr->Format(szTemp, _T("%u"), m_iScore);
    OutputString(_T("Score: "), szTemp);

    InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CWndBenchmark::IntegerBM()
{
#define INT_LOOPS 2
    
    m_uiIntTime = GetTickCount();

    DWORD dwTemp = 0;

    for(int i = 0; i < 5000000; i++)
    {
        dwTemp++;
        m_dwMath = dwTemp * dwTemp;
        m_dwMath = m_dwMath + dwTemp;
        m_dwMath = dwTemp / dwTemp;
        m_dwMath = m_dwMath - dwTemp;
    }

    m_uiIntTime = GetTickCount() - m_uiIntTime;
    return TRUE;
}

BOOL CWndBenchmark::FloatingPointBM()
{
    m_uiFloatTime = GetTickCount();

    double dbTemp = 0;

    for(int i = 0; i < 500000; i++)
    {
        dbTemp += 1.0;
        m_dbMath = dbTemp * dbTemp;
        m_dbMath = m_dbMath + dbTemp;
        m_dbMath = dbTemp / dbTemp;
        m_dbMath = m_dbMath - dbTemp;
    }

    m_uiFloatTime = GetTickCount() - m_uiFloatTime;
    return TRUE;
}

BOOL CWndBenchmark::MemoryBM()
{
#define MEMBUFFER_SIZE 100000

    m_uiMemoryTime = GetTickCount();

    TCHAR szCopyTo[MEMBUFFER_SIZE];

    for(int i = 0; i < 200; i++)
    {
        TCHAR* szTemp = NULL;

        szTemp = new TCHAR[MEMBUFFER_SIZE];
        memset(szTemp, 0, MEMBUFFER_SIZE*sizeof(TCHAR));
        memcpy(szCopyTo, szTemp, MEMBUFFER_SIZE*sizeof(TCHAR));

        m_oStr->Delete(&szTemp);

        szTemp = NULL;
    }

    m_uiMemoryTime = GetTickCount() - m_uiMemoryTime;
    return TRUE;
}

BOOL CWndBenchmark::FileIOBM()
{
    m_uiFileIOTime = GetTickCount();


    m_uiFileIOTime = GetTickCount() - m_uiFileIOTime;
    return TRUE;
}

BOOL CWndBenchmark::FractalBM()
{
    CFractal oFractal;

    HDC dc = GetDC(m_hWnd);
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    CIssGDIEx   gdiGradient;
    CIssGDIEx   gdiMem;

    gdiGradient.Create(dc, rcClient);
    gdiMem.Create(dc, rcClient);

    GradientFillRect(gdiGradient, rcClient, 0x00DD00, 0x223333);

    int iX = 0;
    int iY = 0;
    int iColor = 0;


    BitBlt(gdiMem.GetDC(), 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiGradient.GetDC(), 0, 0, SRCCOPY);
    oFractal.Render(gdiMem, FRAC_SeaHorse);

    BitBlt(dc, 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiMem.GetDC(), 0, 0, SRCCOPY);

    ReleaseDC(m_hWnd, dc);

    Sleep(500);

    return TRUE;
}

BOOL CWndBenchmark::FillRateBM()
{
  //  m_bTesting = TRUE;
  //  SetTimer(m_hWnd, DRAWLOOP_TIMER, 5, NULL);
    
  //  return TRUE;

    HDC dc = GetDC(m_hWnd);
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    CIssGDIEx   gdiGradient;
    CIssGDIEx   gdiMem;

    RECT rcGrad;
    
    rcGrad = rcClient;
    rcGrad.top = 0;
    rcGrad.bottom = HEIGHT(rcClient)*3;//who cares

    gdiGradient.Create(dc, rcGrad);
    gdiMem.Create(dc, rcClient);

    rcGrad.bottom = HEIGHT(rcClient)/2;
    GradientFillRect(gdiGradient, rcGrad, 0xFF0000, 0x00FF00);
    rcGrad.top = rcGrad.bottom;
    rcGrad.bottom +=HEIGHT(rcClient)/2;
    GradientFillRect(gdiGradient, rcGrad, 0x00FF00, 0x00FFFF);
    rcGrad.top = rcGrad.bottom;
    rcGrad.bottom +=HEIGHT(rcClient)/2;
    GradientFillRect(gdiGradient, rcGrad, 0x00FFFF, 0x007FFF);
    rcGrad.top = rcGrad.bottom;
    rcGrad.bottom +=HEIGHT(rcClient)/2;
    GradientFillRect(gdiGradient, rcGrad, 0x007FFF, 0x0000FF);
    rcGrad.top = rcGrad.bottom;
    rcGrad.bottom +=HEIGHT(rcClient)/2;
    GradientFillRect(gdiGradient, rcGrad, 0x0000FF, 0xFF007F);
    rcGrad.top = rcGrad.bottom;
    rcGrad.bottom  =HEIGHT(rcClient)*3;
    GradientFillRect(gdiGradient, rcGrad, 0xFF007F, 0xFF0000);
    rcGrad.top = rcGrad.bottom;

    BitBlt(dc, 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiGradient.GetDC(), 0, 0, SRCCOPY);
    
    m_uiFillRateTime = GetTickCount();

    int iX = 0;
    int iY = 0;
    int iColor = 0;
    int iStart = 0;
    int iHeight;

    for(int i = 0; i < NUMBER_OF_TEST_FRAMES; i++)
    {
        ReleaseDC(m_hWnd, dc);
        dc = GetDC(m_hWnd);

        iHeight = gdiGradient.GetHeight() - iStart;
        if(iHeight > HEIGHT(rcClient))
            iHeight = HEIGHT(rcClient);

        BitBlt(gdiMem.GetDC(), 0, 0, WIDTH(rcClient), iHeight, gdiGradient.GetDC(), 0, iStart, SRCCOPY);
        if(iHeight < HEIGHT(rcClient))
        {
            int iRem = HEIGHT(rcClient) - iHeight;

            BitBlt(gdiMem.GetDC(), 0, iHeight, WIDTH(rcClient), iRem, gdiGradient.GetDC(), 0, 0, SRCCOPY);
        }

        if(iStart >= gdiGradient.GetHeight())
            iStart = 0;

        if(BitBlt(dc, 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiMem.GetDC(), 0, 0, SRCCOPY))
            iStart++;
     }
    
    m_uiFillRateTime = GetTickCount() - m_uiFillRateTime;

    m_dbFrameRate = NUMBER_OF_TEST_FRAMES*1000.0/(double)m_uiFillRateTime;

    m_uiFillRate = UINT(WIDTH(rcClient)*HEIGHT(rcClient)*NUMBER_OF_TEST_FRAMES*m_dbFrameRate);

    ReleaseDC(m_hWnd, dc);
    return TRUE;
}



BOOL CWndBenchmark::IntegerDraw()
{

    HDC dc = GetDC(m_hWnd);
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    CIssGDIEx   gdiGradient;
    CIssGDIEx   gdiMem;

    gdiGradient.Create(dc, rcClient);
    gdiMem.Create(dc, rcClient);

    GradientFillRect(gdiGradient, rcClient, 0x00DD00, 0x223333);


    m_uiFillRateTime = GetTickCount();

    int iX = 0;
    int iY = 0;
    int iColor = 0;
    int iStart = 0;

    for(int i = 0; i < NUMBER_OF_TEST_FRAMES; i++)
    {
        ReleaseDC(m_hWnd, dc);
        dc = GetDC(m_hWnd);
    
        BitBlt(gdiMem.GetDC(), 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiGradient.GetDC(), 0, 0, SRCCOPY);

        //draw the tank
        m_oTank.Spin(1);
        m_oTank.Draw(gdiMem.GetDC(), rcClient);


        if(BitBlt(dc, 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiMem.GetDC(), 0, 0, SRCCOPY))
            iStart++;
    }

    m_uiFillRateTime = GetTickCount() - m_uiFillRateTime;

    m_dbFrameRate = NUMBER_OF_TEST_FRAMES*1000.0/(double)m_uiFillRateTime;

 //   m_uiFillRate = UINT(WIDTH(rcClient)*HEIGHT(rcClient)*NUMBER_OF_TEST_FRAMES*m_dbFrameRate);

    ReleaseDC(m_hWnd, dc);


    return TRUE;
}


