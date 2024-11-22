#include "GuiGuitarTuner.h"
#include "IssLocalisation.h"

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

#define IDT_TIMER_IDLE 1 //idle timer animations ... just for FUN
#define IDT_ANI_TIMER 2
#define IDT_START_ANI 3

#define IDLE_TIME 3000
#define ANI_START 5000

#define INDENT (IsVGA()?10:5)

extern TCHAR g_szRoots[12][8];


void CGuiGuitarTuner::Init(HWND hWnd, HINSTANCE hInst)
{
    CGuiBase::Init(hWnd, hInst);
 
    m_oFreq.InitTuner(hWnd, hInst);
 //   m_oFreq.StartTuner(FQ_SAMP_High);
    m_oStr->Empty(m_szFreq);
    m_oStr->Empty(m_szNote);
    m_oStr->Empty(m_szOctave);
}

CGuiGuitarTuner::CGuiGuitarTuner(void)
{
    m_hFontFreq = CIssGDIEx::CreateFont(IsVGA()?52:26, FW_NORMAL, TRUE);
    m_hFontNote = CIssGDIEx::CreateFont(IsVGA()?80:40, FW_BOLD, TRUE);    
 	m_eGUI = GUI_Tuner;
}

CGuiGuitarTuner::~CGuiGuitarTuner(void)
{
    CIssGDIEx::DeleteFont(m_hFontNote);
    CIssGDIEx::DeleteFont(m_hFontFreq);
    m_oFreq.StopTuner();
}

BOOL CGuiGuitarTuner::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL)
        return FALSE;

    if(m_gdiBackground.GetDC() == NULL)
        InitBackground();


    BitBlt(g_gui->GetGDI()->GetDC(), rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), m_gdiBackground.GetDC(), rcClip.left, rcClip.top, SRCCOPY);
    DrawText(*g_gui->GetGDI(), rcClip);
    DrawPitchBar(*g_gui->GetGDI(), rcClip, m_iPcntTune);
    DrawQualityBar(*g_gui->GetGDI(), rcClip, m_iSigQual*20);//seems to vary between 0 & 5
    DrawGraph(*g_gui->GetGDI(), rcClip);
    g_gui->DrawMenu(rcClip, g_gui->GetGDI());
//    g_gui->DrawTopButtons(rcClip, g_gui->GetGDI());
    


    return TRUE;
}

BOOL CGuiGuitarTuner::DrawQualityBar(CIssGDIEx& gdi, RECT& rc, int iPcnt)
{
#ifdef DEBUG
 //   iPcnt = 100;
#endif

    if(iPcnt < 1)
        return TRUE;

    static int iLastPcnt = 0;
    
    if(iPcnt != iLastPcnt)
    {
        iLastPcnt = iPcnt;
        if(m_imgSigQual.IsLoaded() == FALSE)
            m_imgSigQual.Initialize(m_hWnd, m_hInst, (IsVGA()?IDB_PNG_BarInside_HR:IDB_PNG_BarInside));

        m_imgSigQual.SetSize(WIDTH(m_rcSigQual)*iPcnt/100-(IsVGA()?2:1), HEIGHT(m_rcSigQual));
    }

    static POINT ptDraw = {m_rcSigQual.left, m_rcSigQual.top};
    m_imgSigQual.DrawImage(gdi, ptDraw.x, ptDraw.y);
    
    return TRUE;
}

CIssGDIEx* CGuiGuitarTuner::GetPitchImage() 
{
    if(m_hInst == NULL || m_hWnd == NULL)
        return NULL;

    if(m_gdiBars.GetDC() == NULL)
    {
        m_gdiBars.LoadImage(IsVGA()?IDB_PNG_PitchBars_HR:IDB_PNG_PitchBars, m_hWnd, m_hInst);
    }

    return &m_gdiBars;
}

CIssGDIEx* CGuiGuitarTuner::GetFlatArrows()
{
    if(m_gdiFlatArrows.GetDC() == NULL)
    {
        m_gdiFlatArrows.LoadImage(IsVGA()?IDB_PNG_PitchArrows_HR:IDB_PNG_PitchArrows, m_hWnd, m_hInst);
    }

    return &m_gdiFlatArrows;
}

CIssGDIEx* CGuiGuitarTuner::GetSharpArrows()
{
    if(m_gdiSharpArrows.GetDC() == NULL)
    {
        m_gdiSharpArrows.LoadImage(IsVGA()?IDB_PNG_PitchArrows_HR:IDB_PNG_PitchArrows, m_hWnd, m_hInst);
        m_gdiSharpArrows.FlipVertical();//and flip of course
    }

    return &m_gdiSharpArrows;
}

CIssGDIEx* CGuiGuitarTuner::GetInTuneArrow()
{
    if(m_gdiInTunerArrow.GetDC() == NULL)
    {
        m_gdiInTunerArrow.LoadImage(IsVGA()?IDB_PNG_PitchArrow_HR:IDB_PNG_PitchArrow, m_hWnd, m_hInst);
    }
    return &m_gdiInTunerArrow;
}

BOOL CGuiGuitarTuner::DrawPitchBar(CIssGDIEx& gdi, RECT& rc, int iPitch)
{
    //find the pitch location
    if(iPitch <= -50)
        return FALSE;
    if(iPitch > 50)
        iPitch = 50;

    iPitch += 50; //now its between 0 & 100
    int iPcnt = iPitch; //for the bars

    iPitch = iPitch*m_iNumBars/100;

    int iTemp;
    EnumPitchIndicators ePitch;
    int iGap = IsVGA()?2:1;

    //now fill in the bars at iPitch
    for(int i = 0; i < 7; i++)
    {
        iTemp = iPitch - 3 + i;
        if(iTemp < 0)
            continue;
        if(iTemp >= m_iNumBars)
            break;

        switch(i)
        {
        case 0:
        case 6:
            ePitch = PITCH_Faint;
        	break;
        case 1:
        case 5:
            ePitch = PITCH_Med;
        	break;
        case 2:
        case 4:
            ePitch = PITCH_Bright;
            break;
        default:
            ePitch = PITCH_Full;
            break;
        }

        if(iTemp == m_iCenter)
            ePitch = EnumPitchIndicators(ePitch + (int)PITCH_GOff);
        else if(iTemp == m_iCenter - 1 && i < 3)
            ePitch = EnumPitchIndicators(ePitch + (int)PITCH_LOff);
        else if(iTemp == m_iCenter + 1 && i > 3)//over half way
        {
            if(ePitch == PITCH_Bright)
                ePitch = PITCH_RMed;
            else if(ePitch == PITCH_Med)
                ePitch = PITCH_RFaint;
            else if(ePitch == PITCH_Faint)
                ePitch = PITCH_ROff;
        }
       
        Draw(gdi, m_rcPitch.left + iTemp*(iGap+m_gdiBars.GetWidth()/PITCH_Count), m_rcPitch.top,
            m_gdiBars.GetWidth()/PITCH_Count, m_gdiBars.GetHeight(), m_gdiBars, (int)ePitch*m_gdiBars.GetWidth()/PITCH_Count, 0);//the off state is currently the first one in the image
    }

    //draw the yellow arrows
    //flat first
    if(iPcnt < 18)
        iPitch = 0;//full
    else if(iPcnt < 36)
        iPitch = 1;
    else if(iPcnt < 49)
        iPitch = 2;
    else 
        iPitch = 3;
    Draw(gdi, m_rcFlat, *GetFlatArrows(), iPitch*(GetFlatArrows()->GetWidth()/4), 0);

    //sharp
    if(iPcnt > 82)
        iPitch = 3;//full
    else if(iPcnt > 54)
        iPitch = 2;
    else if(iPcnt > 51)
        iPitch = 1;
    else 
        iPitch = 0;
    Draw(gdi, m_rcSharp, *GetSharpArrows(), iPitch*(GetSharpArrows()->GetWidth()/4), 0);

    //and finally ... draw the in tune arrow
    if(iPcnt == 50)
    {
        Draw(gdi, m_rcInTune, *GetInTuneArrow());
    }

    return TRUE;
}

BOOL CGuiGuitarTuner::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    m_rcTitle = rcClient;
    m_rcTitle.bottom = rcClient.top + BUTTONS_HEIGHT;

    int iIndent = INDENT; 
    int iTextHeight = IsVGA()?32:16;
    int iPitchHeight = IsVGA()?71:36;
    int iPitchWidth = IsVGA()?10:5;
    int iArrowWidth = IsVGA()?51:25;
    int iArrowHeight = IsVGA()?28:14;
    int iSigQualHeight = IsVGA()?30:16;
    int iNoteSize = IsVGA()?110:55;
    int iFreqHeight = IsVGA()?50:25;

    int iInTuneW = IsVGA()?18:9;
    int iInTuneH = IsVGA()?10:5;

    SetRect(&m_rcPitchText, rcClient.left + iIndent, rcClient.top + iIndent, rcClient.right - iIndent, rcClient.top + iTextHeight);
    SetRect(&m_rcPitch, rcClient.left + iIndent, m_rcPitchText.bottom + iIndent/2, rcClient.right - iIndent, m_rcPitchText.bottom + iPitchHeight);
    SetRect(&m_rcInTune, rcClient.left + (WIDTH(rcClient) - iInTuneW)/2, m_rcPitch.top - iInTuneH,   rcClient.left + (WIDTH(rcClient) + iInTuneW)/2, m_rcPitch.top);
    SetRect(&m_rcFlat, rcClient.left + iIndent, m_rcPitch.bottom + iIndent, rcClient.left + iArrowWidth + iIndent, m_rcPitch.bottom + iIndent + iArrowHeight);
    SetRect(&m_rcSharp, rcClient.right - iArrowWidth - iIndent, m_rcPitch.bottom + iIndent, rcClient.right - iIndent, m_rcPitch.bottom + iIndent + iArrowHeight);
    SetRect(&m_rcSigQualText, rcClient.left + iIndent, m_rcFlat.bottom, rcClient.right, m_rcFlat.bottom + iIndent + iTextHeight);
    SetRect(&m_rcSigQual, rcClient.left + iIndent, m_rcSigQualText.bottom + iIndent/2, rcClient.right - iNoteSize - 4*iIndent, m_rcSigQualText.bottom + iIndent + iSigQualHeight);
    SetRect(&m_rcNote, rcClient.right - iIndent - iNoteSize, m_rcFlat.bottom + iIndent, rcClient.right - iIndent, m_rcFlat.bottom + iIndent + iNoteSize);
    SetRect(&m_rcGraphText, rcClient.left + iIndent, max(m_rcNote.bottom, m_rcSigQual.bottom) - iIndent/2, rcClient.right - iIndent, max(m_rcNote.bottom, m_rcSigQual.bottom) + iIndent + iTextHeight);
    SetRect(&m_rcGraph, rcClient.left + iIndent, m_rcGraphText.bottom, rcClient.right - iIndent, rcClient.bottom - iIndent - MENU_HEIGHT);
    SetRect(&m_rcFreq, m_rcGraph.left + iIndent, m_rcGraph.bottom - iIndent - iFreqHeight, m_rcGraph.right - 2*iIndent, m_rcGraph.bottom - iIndent);
    
    m_iNumBars = WIDTH(m_rcPitch)/iPitchWidth;
    m_iCenter = m_iNumBars/2;

//    int iGap = IsVGA()?2:1;
//    int iTemp = m_rcPitch.left + m_iCenter*(iGap+m_gdiBars.GetWidth()/PITCH_Count) + m_gdiBars.GetWidth()/2 - iInTuneW/2;
//    SetRect(&m_rcInTune, iTemp, m_rcPitch.top - iInTuneH, iTemp + iInTuneW, m_rcPitch.top);*/


    SetRect(&m_rcUpdateRgn, rcClient.left + iIndent, m_rcInTune.top, rcClient.right - iIndent, m_rcGraph.bottom);

    return TRUE;

}

BOOL CGuiGuitarTuner::OnLButtonUp(HWND hWnd, POINT& pt)
{
    
    return UNHANDLED;
}

BOOL CGuiGuitarTuner::OnLButtonDown(HWND hWnd, POINT& pt)
{
    KillTimer(m_hWnd, IDT_TIMER_IDLE);
    SetTimer(m_hWnd, IDT_TIMER_IDLE, IDLE_TIME, NULL);

    return UNHANDLED;
}

BOOL CGuiGuitarTuner::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_LEFT:
        m_iPcntTune--;
        if(m_iPcntTune < -50)
            m_iPcntTune = 50;
        break;
    case VK_RIGHT:
        m_iPcntTune++;
        if(m_iPcntTune > 50)
            m_iPcntTune = -50;
        break;    
    default:
        return UNHANDLED;
        break;
    }
    InvalidateRect(m_hWnd, &m_rcPitch, FALSE);
    UpdateWindow(m_hWnd);
    return TRUE;
}


BOOL CGuiGuitarTuner::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDT_TIMER_IDLE:
        KillTimer(m_hWnd, IDT_TIMER_IDLE);
        ClearValues();
        InvalidateRect(m_hWnd, &m_rcUpdateRgn, FALSE);
        UpdateWindow(m_hWnd);
    	break;
    default:
        break;
    }

    return UNHANDLED;
}

BOOL CGuiGuitarTuner::DrawGraph(CIssGDIEx& gdi, RECT& rc)
{
    if(m_szFreq[0] == _T('\0'))
    {
        ::Line(gdi.GetDC(), m_rcGraph.left + INDENT, m_rcGraph.top + HEIGHT(m_rcGraph)/2, 
            m_rcGraph.right - INDENT, m_rcGraph.top + HEIGHT(m_rcGraph)/2, 0x009900);
        return TRUE;//don't need to draw it
    }

    unsigned char* data = m_oFreq.GetGraphData();

    if(data == NULL)
        return FALSE;

    POINT* pt= NULL;
    pt = new POINT[WIDTH(m_rcGraph)];//slightly more than we need but that's fine
    if(pt == NULL)
        return FALSE;


    //so the data buffer size is WF_BUFFER_SIZE
    //lets try a little brute force first

    int iScale = IsVGA()?72:36;

    if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
        iScale = GetSystemMetrics(SM_CXSMICON)*4/3;

    int iXOffSet = m_rcGraph.left + INDENT;
    int iWidth = WIDTH(m_rcGraph) - 2*INDENT;

  //  for(int i = m_rcGraph.left + INDENT; i < m_rcGraph.right - INDENT; i++)
    for(int i = 0; i < iWidth; i++)
    {
        pt[i].x = iXOffSet + i;
        pt[i].y = ((data[i] - 0x80) * iScale) / m_oFreq.GetMaxDispLev() + m_rcGraph.top + HEIGHT(m_rcGraph)/2;
    }

    HPEN hPenNew = CIssGDIEx::CreatePen(RGB(0, 255, 0));
    HPEN hPenOld = (HPEN)SelectObject(gdi.GetDC(), hPenNew);
    
    Polyline(gdi.GetDC(), pt, iWidth);

    if(pt)
        delete [] pt;

    SelectObject(gdi.GetDC(), hPenOld);
    DeleteObject(hPenNew);

    return TRUE;
}

BOOL CGuiGuitarTuner::DrawText(CIssGDIEx& gdi, RECT& rc)
{
    ::DrawText(gdi.GetDC(), m_szNote, m_rcNote, DT_CENTER | DT_VCENTER, m_hFontNote, 0xCCCCCC);
    RECT rcTemp = m_rcNote;
    rcTemp.right -= IsVGA()?10:5;
    ::DrawText(gdi.GetDC(), m_szOctave, rcTemp, DT_RIGHT | DT_BOTTOM, g_gui->GetFont(FONT_Button), 0xCCCCCC);
    ::DrawText(gdi.GetDC(), m_szFreq, m_rcFreq, DT_RIGHT | DT_BOTTOM, m_hFontFreq, 0x0000FF);
    return TRUE;
}

BOOL CGuiGuitarTuner::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_FREQ_UPDATE)
    {
    //    TCHAR szOutPut[STRING_MAX];
    //    m_oStr->Format(szOutPut, _T("Freq: %f Quality: %i Sig Stren: %i Tune: %i"),
    //            m_oFreq.GetFrequency(), m_oFreq.GetSampleQuality(), m_oFreq.GetSignalStrength(), m_oFreq.GetPcntFromTuned());

    //  DebugOut(szOutPut);

    static double dbFreq;
    dbFreq = m_oFreq.GetFrequency();

        if(dbFreq > 0)
        {
            m_iSigQual = m_oFreq.GetSampleQuality();
            m_iPcntTune = m_oFreq.GetPcntFromTuned();
            m_oStr->Format(m_szFreq, _T("%.1f"), dbFreq);
        
            int iNote = m_oFreq.GetNote();
            int iNoteIndex = iNote % 12;
            int iOctaveIndex = (iNote - 3) / 12;


            m_oStr->StringCopy(m_szNote, g_szRoots[iNoteIndex]);
            m_oStr->IntToString(m_szOctave, iOctaveIndex);

            InvalidateRect(m_hWnd, &m_rcUpdateRgn, FALSE);
            UpdateWindow(m_hWnd);
            KillTimer(m_hWnd, IDT_TIMER_IDLE);
            SetTimer(m_hWnd, IDT_TIMER_IDLE, IDLE_TIME, NULL);
        }
                
        return TRUE;
    }
    return UNHANDLED;
}

void CGuiGuitarTuner::ClearValues()
{
    m_iSigQual = 0;
    m_iPcntTune = -50;
    m_oStr->Empty(m_szNote);
    m_oStr->Empty(m_szFreq);
    m_oStr->Empty(m_szOctave);
}

void CGuiGuitarTuner::OnSetFocus()
{
    m_oFreq.StartTuner(FQ_SAMP_Med);
}

void CGuiGuitarTuner::OnLostFocus()
{
    m_oFreq.StopTuner();

    KillTimer(m_hWnd, IDT_TIMER_IDLE);
    KillTimer(m_hWnd, IDT_ANI_TIMER);
    KillTimer(m_hWnd, IDT_START_ANI);
}

void CGuiGuitarTuner::InitBackground()
{
    if(g_gui == NULL || m_hWnd == NULL)
        return;

    if(GetPitchImage() == NULL)
        return;

   RECT rcClient;
   GetClientRect(m_hWnd, &rcClient);
   HDC dc = GetDC(m_hWnd);
   m_gdiBackground.Create(dc, rcClient);
   ReleaseDC(m_hWnd, dc);
   
   RECT rcTemp = rcClient;
   rcTemp.bottom = rcTemp.top + m_gdiBars.GetHeight()*3/2;

   FillRect(m_gdiBackground, rcTemp, 0);
   rcTemp.top = rcTemp.bottom;
   rcTemp.bottom = rcClient.bottom;
   
    //gradient background
    GradientFillRect(m_gdiBackground, rcTemp, 0, 0x434343, FALSE);

    //draw the basic text
    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_Pitch), m_hInst), m_rcPitchText, DT_LEFT | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xCCCCCC);
    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_SignalQuality), m_hInst), m_rcSigQualText, DT_LEFT | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xCCCCCC);
    ::DrawText(m_gdiBackground.GetDC(), m_oStr->GetText(ID(IDS_INFO_Frequency), m_hInst), m_rcGraphText, DT_LEFT | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xCCCCCC);

    //lets do the outlines now
    CIssImageSliced gdiSlice;
        
    gdiSlice.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_PitchBG_HR:IDB_PNG_PitchBG);
    rcTemp = m_rcPitch;
    InflateRect(&rcTemp, IsVGA()?4:2, IsVGA()?4:2);
    gdiSlice.SetSize(WIDTH(rcTemp), HEIGHT(rcTemp));
    gdiSlice.DrawImage(m_gdiBackground, rcTemp.left, rcTemp.top);

    CIssGDIEx gdiGraph; //we're going to borrow this for the upside down slice
    gdiGraph.LoadImage(IsVGA()?IDB_PNG_PitchBG_HR:IDB_PNG_PitchBG, m_hWnd, m_hInst);
    gdiGraph.FlipHorizontal();
    gdiSlice.Initialize(gdiGraph);
    gdiSlice.SetSize(WIDTH(m_rcNote), HEIGHT(m_rcNote));
    gdiSlice.DrawImage(m_gdiBackground, m_rcNote.left, m_rcNote.top);

    //start graph
    gdiGraph.Create(m_gdiBackground.GetDC(), m_rcGraph);

    int iW = WIDTH(m_rcGraph);
    int iH = HEIGHT(m_rcGraph);

    SetRect(&rcTemp, 0, 0, iW, iH);
    //and the graph background
    GradientFillRect(gdiGraph, rcTemp, 0x434043, 0, FALSE);

    //and the grid
    int iNumVert = 20;
    int iNumHorz = iNumVert*HEIGHT(rcTemp)/WIDTH(rcTemp);//that's better

    for(int i = 0; i < iNumVert; i++)
    {
        ::Line(gdiGraph.GetDC(), i*WIDTH(rcTemp)/iNumVert, rcTemp.top, i*WIDTH(rcTemp)/iNumVert, rcTemp.bottom, 0);
    }

    for(int i = 0; i < iNumHorz; i++)
    {
        ::Line(gdiGraph.GetDC(), rcTemp.left, i*HEIGHT(rcTemp)/iNumHorz, rcTemp.right, i*HEIGHT(rcTemp)/iNumHorz, 0);
    }

    gdiGraph.SetAlphaMask(g_gui->GetAlpha(iW, iH)->GetImage());
    g_gui->GetImgOutline(iW, iH)->DrawImage(gdiGraph, 0, 0, ALPHA_AddValue);
    Draw(m_gdiBackground, m_rcGraph, gdiGraph);

    //signal background
    gdiSlice.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_BarBack_HR:IDB_PNG_BarBack);
    gdiSlice.SetSize(WIDTH(m_rcSigQual), HEIGHT(m_rcSigQual));
    gdiSlice.DrawImage(m_gdiBackground, m_rcSigQual.left, m_rcSigQual.top);

    //pitch unlit
    int iGap = IsVGA()?2:1;
    for(int i = 0; i < m_iNumBars; i++)
    {
        if(i == m_iCenter)
            Draw(m_gdiBackground, m_rcPitch.left + i*(iGap+m_gdiBars.GetWidth()/PITCH_Count), m_rcPitch.top, 
            m_gdiBars.GetWidth()/PITCH_Count, m_gdiBars.GetHeight(), m_gdiBars, PITCH_GOff*m_gdiBars.GetWidth()/PITCH_Count, 0);//the off state is currently the first one in the image
        else
            Draw(m_gdiBackground, m_rcPitch.left + i*(iGap+m_gdiBars.GetWidth()/PITCH_Count), m_rcPitch.top, 
                m_gdiBars.GetWidth()/PITCH_Count, m_gdiBars.GetHeight(), m_gdiBars, 0, 0);//the off state is currently the first one in the image
    }
}

BOOL CGuiGuitarTuner::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
    {
        m_oFreq.SetTrigger(g_options->GetiTrigger());
        return TRUE;
    }
    return UNHANDLED;
}

void CGuiGuitarTuner::ResetGDI()
{   //for lanuage change
    m_gdiBackground.Destroy();
}