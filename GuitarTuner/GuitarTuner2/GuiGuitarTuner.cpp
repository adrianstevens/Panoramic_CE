#include "GuiGuitarTuner.h"

#define IDT_TIMER_HOLD      1
#define IDT_REPEAT_TIMER    2

void CGuiGuitarTuner::Init(HWND hWnd, HINSTANCE hInst)
{
    CGuiBase::Init(hWnd, hInst);

    LoadImages(m_hWnd, m_hInst);
    LoadTuning();

	ReloadSounds();
}

CGuiGuitarTuner::CGuiGuitarTuner(void)
{
    m_hfButtonText	= m_gdiMem.CreateFont(GetSystemMetrics(SM_CXICON)*3/5, FW_BOLD, TRUE);
    m_hfScreenText	= m_gdiMem.CreateFont(GetSystemMetrics(SM_CXICON)*3/4, FW_BOLD, TRUE);

    for(int i = 0; i < 6; i++)
        m_eButtonState[i] = BSTATE_Off;

    m_iTuning = 0;

	m_eGUI = GUI_Tuner;
}

CGuiGuitarTuner::~CGuiGuitarTuner(void)
{
    CIssGDIEx::DeleteFont(m_hfButtonText);
    CIssGDIEx::DeleteFont(m_hfScreenText);
}

BOOL CGuiGuitarTuner::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(m_gdiMem.GetDC() == NULL)
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

    DrawBackground(m_gdiMem, rcClip);
    DrawText(m_gdiMem, rcClip);
    DrawButtons(m_gdiMem, rcClip);


    BitBlt(hDC,
        rcClip.left, rcClip.top, 
        rcClip.right - rcClip.left,
        rcClip.bottom - rcClip.top,
        m_gdiMem.GetDC(),
        rcClip.left,
        rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CGuiGuitarTuner::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    m_gdiMem.Destroy();
    m_gdiBackground.Destroy();

    m_rcTopText = rcClient;
    m_rcTopText.bottom = m_rcTopText.top + GetSystemMetrics(SM_CXICON);

    //and button time
    int iIndent = 0; //GetSystemMetrics(SM_CXICON)/32;//seems good
    int iWidth  = GetSystemMetrics(SM_CXICON)*13/5;
    int iHeight = min(GetSystemMetrics(SM_CXICON)*7/5, (rcClient.bottom - m_rcTopText.bottom)/6);



    m_rcBtns[0].top     = m_rcTopText.bottom;
    m_rcBtns[0].bottom  = m_rcBtns[0].top + iHeight;
    m_rcBtns[0].left    = rcClient.left + 2*iIndent;
    m_rcBtns[0].right   = m_rcBtns[0].left + iWidth;

    m_rcBtns[1]         = m_rcBtns[0];
    m_rcBtns[1].top     = m_rcBtns[0].bottom;
    m_rcBtns[1].bottom  = m_rcBtns[1].top + iHeight;

    m_rcBtns[2]         = m_rcBtns[1];
    m_rcBtns[2].top     = m_rcBtns[1].bottom;
    m_rcBtns[2].bottom  = m_rcBtns[2].top + iHeight;

    m_rcBtns[3]         = m_rcBtns[2];
    m_rcBtns[3].top     = m_rcBtns[2].bottom;
    m_rcBtns[3].bottom  = m_rcBtns[3].top + iHeight;

    m_rcBtns[4]         = m_rcBtns[3];
    m_rcBtns[4].top     = m_rcBtns[3].bottom;
    m_rcBtns[4].bottom  = m_rcBtns[4].top + iHeight;

    m_rcBtns[5]         = m_rcBtns[4];
    m_rcBtns[5].top     = m_rcBtns[4].bottom;
    m_rcBtns[5].bottom  = m_rcBtns[5].top + iHeight;

    m_rcBottomText      = rcClient;
    m_rcBottomText.left  = m_rcBtns[0].right;

    m_imgBtn.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));
    m_imgBtnRed.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));
    m_imgBtnGreen.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));

    return TRUE;
}

BOOL CGuiGuitarTuner::OnLButtonUp(HWND hWnd, POINT& pt)
{
    KillTimer(m_hWnd, IDT_TIMER_HOLD);

    for(int i = 0; i < 6; i++)
    {
        if(PtInRect(&m_rcBtns[i], pt))
        {
            if(m_eButtonState[i] == BSTATE_Repeat)
                return UNHANDLED;

            m_eButtonState[i] = BSTATE_Off;

            switch(i)
            {
            case 0:
                PlaySounds(m_sTuning[m_iTuning].szWave1);
                break;
            case 1:
                PlaySounds(m_sTuning[m_iTuning].szWave2);
                break;
            case 2:
                PlaySounds(m_sTuning[m_iTuning].szWave3);
                break;
            case 3:
                PlaySounds(m_sTuning[m_iTuning].szWave4);
                break;
            case 4:
                PlaySounds(m_sTuning[m_iTuning].szWave5);
                break;
            case 5:
                PlaySounds(m_sTuning[m_iTuning].szWave6);
                break;
            default:
                break;
            }

            InvalidateRect(m_hWnd, &m_rcBtns[i], FALSE);
            UpdateWindow(m_hWnd);
            return TRUE;
        }
    }

    if(PtInRect(&m_rcBottomText, pt))
    {
        m_iTuning++;
        if(m_iTuning > 8)
            m_iTuning = 0;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }

    return UNHANDLED;
}

BOOL CGuiGuitarTuner::OnLButtonDown(HWND hWnd, POINT& pt)
{
    KillTimer(m_hWnd, IDT_REPEAT_TIMER);
    InvalidateRect(m_hWnd, NULL, FALSE);

    for(int i = 0; i < 6; i++)
        m_eButtonState[i] = BSTATE_Off;

    for(int i = 0; i < 6; i++)
    {
        if(PtInRect(&m_rcBtns[i], pt))
        {
            m_eButtonState[i] = BSTATE_On;
            m_iSelected = i;
            SetTimer(m_hWnd, IDT_TIMER_HOLD, 400, NULL);
            UpdateWindow(m_hWnd);
            return TRUE;
        }
    }

    return UNHANDLED;
}

BOOL CGuiGuitarTuner::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDT_TIMER_HOLD:
        KillTimer(m_hWnd, IDT_TIMER_HOLD);
        m_eButtonState[m_iSelected] = BSTATE_Repeat;
        InvalidateRect(m_hWnd, &m_rcBtns[m_iSelected], FALSE);
        //      m_iSelected = -1;
        UpdateWindow(m_hWnd);
        SetTimer(m_hWnd, IDT_REPEAT_TIMER, 1500, NULL);
    case IDT_REPEAT_TIMER:
        switch(m_iSelected)
        {
        case 0:
            PlaySounds(m_sTuning[m_iTuning].szWave1);
            break;
        case 1:
            PlaySounds(m_sTuning[m_iTuning].szWave2);
            break;
        case 2:
            PlaySounds(m_sTuning[m_iTuning].szWave3);
            break;
        case 3:
            PlaySounds(m_sTuning[m_iTuning].szWave4);
            break;
        case 4:
            PlaySounds(m_sTuning[m_iTuning].szWave5);
            break;
        case 5:
            PlaySounds(m_sTuning[m_iTuning].szWave6);
            break;      
        }
        break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

void CGuiGuitarTuner::LoadImages(HWND hWnd, HINSTANCE hInst)
{
    //btn slice time
    if(!m_imgBtn.IsLoaded())
        m_imgBtn.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnVGA:IDB_PNG_Btn);

    if(!m_imgBtnRed.IsLoaded())
        m_imgBtnRed.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnRedVGA:IDB_PNG_BtnRed);

    if(!m_imgBtnGreen.IsLoaded())
        m_imgBtnGreen.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnGreenVGA:IDB_PNG_BtnGreen);

    m_imgBtn.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));
    m_imgBtnRed.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));
    m_imgBtnGreen.SetSize(WIDTH(m_rcBtns[0]), HEIGHT(m_rcBtns[0]));
}

void CGuiGuitarTuner::LoadTuning()
{
    //Populate Tuning Structure Array - holds info for tuning name, string and wav file to play
    m_oStr->StringCopy(m_sTuning[0].szName, _T("Standard"));
    m_oStr->StringCopy(m_sTuning[0].szString1, _T("E"));
    m_oStr->StringCopy(m_sTuning[0].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[0].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[0].szString4, _T("G"));
    m_oStr->StringCopy(m_sTuning[0].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[0].szString6, _T("E"));
    m_oStr->StringCopy(m_sTuning[0].szWave1, _T("IDR_WAV_LOW_E"));
    m_oStr->StringCopy(m_sTuning[0].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[0].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[0].szWave4, _T("IDR_WAV_MID_G"));
    m_oStr->StringCopy(m_sTuning[0].szWave5, _T("IDR_WAV_HIGH_B"));
    m_oStr->StringCopy(m_sTuning[0].szWave6, _T("IDR_WAV_HIGH_E"));

    m_oStr->StringCopy(m_sTuning[1].szName,  _T("Drop D"));
    m_oStr->StringCopy(m_sTuning[1].szString1, _T("D"));
    m_oStr->StringCopy(m_sTuning[1].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[1].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[1].szString4, _T("G"));
    m_oStr->StringCopy(m_sTuning[1].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[1].szString6, _T("E"));
    m_oStr->StringCopy(m_sTuning[1].szWave1, _T("IDR_WAV_LOW_D"));
    m_oStr->StringCopy(m_sTuning[1].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[1].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[1].szWave4, _T("IDR_WAV_MID_G"));
    m_oStr->StringCopy(m_sTuning[1].szWave5, _T("IDR_WAV_HIGH_B"));
    m_oStr->StringCopy(m_sTuning[1].szWave6, _T("IDR_WAV_HIGH_E"));

    m_oStr->StringCopy(m_sTuning[2].szName,  _T("Drop D Alternate"));
    m_oStr->StringCopy(m_sTuning[2].szString1, _T("D"));
    m_oStr->StringCopy(m_sTuning[2].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[2].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[2].szString4, _T("G"));
    m_oStr->StringCopy(m_sTuning[2].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[2].szString6, _T("D"));
    m_oStr->StringCopy(m_sTuning[2].szWave1, _T("IDR_WAV_LOW_D"));
    m_oStr->StringCopy(m_sTuning[2].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[2].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[2].szWave4, _T("IDR_WAV_MID_G"));
    m_oStr->StringCopy(m_sTuning[2].szWave5, _T("IDR_WAV_HIGH_B"));
    m_oStr->StringCopy(m_sTuning[2].szWave6, _T("IDR_WAV_HIGH_D"));

    m_oStr->StringCopy(m_sTuning[3].szName,  _T("D Modal"));
    m_oStr->StringCopy(m_sTuning[3].szString1, _T("D"));
    m_oStr->StringCopy(m_sTuning[3].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[3].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[3].szString4, _T("G"));
    m_oStr->StringCopy(m_sTuning[3].szString5, _T("A"));
    m_oStr->StringCopy(m_sTuning[3].szString6, _T("D"));
    m_oStr->StringCopy(m_sTuning[3].szWave1, _T("IDR_WAV_LOW_D"));
    m_oStr->StringCopy(m_sTuning[3].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[3].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[3].szWave4, _T("IDR_WAV_MID_G"));
    m_oStr->StringCopy(m_sTuning[3].szWave5, _T("IDR_WAV_HIGH_A"));
    m_oStr->StringCopy(m_sTuning[3].szWave6, _T("IDR_WAV_HIGH_D"));

    m_oStr->StringCopy(m_sTuning[4].szName,  _T("Fourths"));
    m_oStr->StringCopy(m_sTuning[4].szString1, _T("E"));
    m_oStr->StringCopy(m_sTuning[4].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[4].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[4].szString4, _T("G"));
    m_oStr->StringCopy(m_sTuning[4].szString5, _T("C"));
    m_oStr->StringCopy(m_sTuning[4].szString6, _T("F"));
    m_oStr->StringCopy(m_sTuning[4].szWave1, _T("IDR_WAV_LOW_E"));
    m_oStr->StringCopy(m_sTuning[4].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[4].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[4].szWave4, _T("IDR_WAV_MID_G"));
    m_oStr->StringCopy(m_sTuning[4].szWave5, _T("IDR_WAV_HIGH_C"));
    m_oStr->StringCopy(m_sTuning[4].szWave6, _T("IDR_WAV_HIGH_F"));

    m_oStr->StringCopy(m_sTuning[5].szName,  _T("Lute"));
    m_oStr->StringCopy(m_sTuning[5].szString1, _T("E"));
    m_oStr->StringCopy(m_sTuning[5].szString2, _T("A"));
    m_oStr->StringCopy(m_sTuning[5].szString3, _T("D"));
    m_oStr->StringCopy(m_sTuning[5].szString4, _T("F#"));
    m_oStr->StringCopy(m_sTuning[5].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[5].szString6, _T("E"));
    m_oStr->StringCopy(m_sTuning[5].szWave1, _T("IDR_WAV_LOW_E"));
    m_oStr->StringCopy(m_sTuning[5].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[5].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[5].szWave4, _T("IDR_WAV_MID_Fs"));
    m_oStr->StringCopy(m_sTuning[5].szWave5, _T("IDR_WAV_HIGH_B"));
    m_oStr->StringCopy(m_sTuning[5].szWave6, _T("IDR_WAV_HIGH_E"));

    m_oStr->StringCopy(m_sTuning[6].szName,  _T("Drop Tuning"));
    m_oStr->StringCopy(m_sTuning[6].szString1, _T("D#"));
    m_oStr->StringCopy(m_sTuning[6].szString2, _T("G#"));
    m_oStr->StringCopy(m_sTuning[6].szString3, _T("C#"));
    m_oStr->StringCopy(m_sTuning[6].szString4, _T("F#"));
    m_oStr->StringCopy(m_sTuning[6].szString5, _T("A#"));
    m_oStr->StringCopy(m_sTuning[6].szString6, _T("D#"));
    m_oStr->StringCopy(m_sTuning[6].szWave1, _T("IDR_WAV_LOW_Ds"));
    m_oStr->StringCopy(m_sTuning[6].szWave2, _T("IDR_WAV_LOW_Gs"));
    m_oStr->StringCopy(m_sTuning[6].szWave3, _T("IDR_WAV_MID_Cs"));
    m_oStr->StringCopy(m_sTuning[6].szWave4, _T("IDR_WAV_MID_Ds"));
    m_oStr->StringCopy(m_sTuning[6].szWave5, _T("IDR_WAV_HIGH_As"));
    m_oStr->StringCopy(m_sTuning[6].szWave6, _T("IDR_WAV_HIGH_Ds"));

    m_oStr->StringCopy(m_sTuning[7].szName,  _T("Shifted E"));
    m_oStr->StringCopy(m_sTuning[7].szString1, _T("E"));
    m_oStr->StringCopy(m_sTuning[7].szString2, _T("G#"));
    m_oStr->StringCopy(m_sTuning[7].szString3, _T("C#"));
    m_oStr->StringCopy(m_sTuning[7].szString4, _T("F#"));
    m_oStr->StringCopy(m_sTuning[7].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[7].szString6, _T("E"));
    m_oStr->StringCopy(m_sTuning[7].szWave1, _T("IDR_WAV_LOW_E"));
    m_oStr->StringCopy(m_sTuning[7].szWave2, _T("IDR_WAV_LOW_Gs"));
    m_oStr->StringCopy(m_sTuning[7].szWave3, _T("IDR_WAV_MID_Cs"));
    m_oStr->StringCopy(m_sTuning[7].szWave4, _T("IDR_WAV_MID_Fs"));
    m_oStr->StringCopy(m_sTuning[7].szWave5, _T("IDR_WAV_HIGH_B"));
    m_oStr->StringCopy(m_sTuning[7].szWave6, _T("IDR_WAV_HIGH_E"));

    m_oStr->StringCopy(m_sTuning[8].szName,  _T("Open D"));
    m_oStr->StringCopy(m_sTuning[8].szString1, _T("E"));
    m_oStr->StringCopy(m_sTuning[8].szString2, _T("G#"));
    m_oStr->StringCopy(m_sTuning[8].szString3, _T("C#"));
    m_oStr->StringCopy(m_sTuning[8].szString4, _T("F#"));
    m_oStr->StringCopy(m_sTuning[8].szString5, _T("B"));
    m_oStr->StringCopy(m_sTuning[8].szString6, _T("E"));
    m_oStr->StringCopy(m_sTuning[8].szWave1, _T("IDR_WAV_LOW_D"));
    m_oStr->StringCopy(m_sTuning[8].szWave2, _T("IDR_WAV_MID_A"));
    m_oStr->StringCopy(m_sTuning[8].szWave3, _T("IDR_WAV_MID_D"));
    m_oStr->StringCopy(m_sTuning[8].szWave4, _T("IDR_WAV_MID_Fs"));
    m_oStr->StringCopy(m_sTuning[8].szWave5, _T("IDR_WAV_HIGH_A"));
    m_oStr->StringCopy(m_sTuning[8].szWave6, _T("IDR_WAV_HIGH_D"));
}

BOOL CGuiGuitarTuner::DrawText(CIssGDIEx& gdi, RECT& rc)
{
    RECT rcTemp = m_rcTopText;
    OffsetRect(&rcTemp, 2, 2);
    ::DrawText(gdi.GetDC(), _T("Guitar Tuner"), rcTemp, DT_CENTER | DT_VCENTER, m_hfScreenText, 0);
    ::DrawText(gdi.GetDC(), _T("Guitar Tuner"), m_rcTopText, DT_CENTER | DT_VCENTER, m_hfScreenText, 0xFFFFFF);


    rcTemp = m_rcBottomText;
    OffsetRect(&rcTemp, 2, 2);
    ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szName, rcTemp, DT_CENTER | DT_VCENTER, m_hfButtonText, 0);
    ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szName, m_rcBottomText, DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);

    return TRUE;
}


BOOL CGuiGuitarTuner::DrawButtons(CIssGDIEx& gdi, RECT& rc)
{
    for(int i = 0; i < 6; i++)
    {
        if(IsRectInRect(rc, m_rcBtns[i]))
        {
            switch(m_eButtonState[i])
            {
            default:
            case BSTATE_Off:
                m_imgBtn.DrawImage(gdi, m_rcBtns[i].left, m_rcBtns[i].top);
                break;
            case BSTATE_On:
                m_imgBtnGreen.DrawImage(gdi, m_rcBtns[i].left, m_rcBtns[i].top);
                break;
            case BSTATE_Repeat:
                m_imgBtnRed.DrawImage(gdi, m_rcBtns[i].left, m_rcBtns[i].top);
                break;
            }
            //draw label
            switch(i)
            {
            case 0:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString1, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            case 1:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString2, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            case 2:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString3, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            case 3:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString4, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            case 4:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString5, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            case 5:
                ::DrawText(gdi.GetDC(), m_sTuning[m_iTuning].szString6, m_rcBtns[i], DT_CENTER | DT_VCENTER, m_hfButtonText, 0xFFFFFF);
                break;
            }
        }
    }
    return TRUE;
}



void CGuiGuitarTuner::ReloadSounds()
{
	if(m_oSoundFX.IsLoaded())
		return;

	int iRet = 0;

	iRet = m_oSoundFX.Initialize(m_hInst);

	m_oSoundFX.LoadSFX(IDR_WAV_GLow);
	m_oSoundFX.LoadSFX(IDR_WAV_BMid);
	m_oSoundFX.LoadSFX(IDR_WAV_DMid);
	m_oSoundFX.LoadSFX(IDR_WAV_GMid);
	m_oSoundFX.LoadSFX(IDR_WAV_DHi);
	m_oSoundFX.LoadSFX(IDR_WAV_GHi);

	m_oSoundFX.SetVolumeSFX(63);
}