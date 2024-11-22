#include "GuiGuitarChords.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "Resource.h"
#include "IssRegistry.h"
#include "ObjGui.h"

extern TCHAR g_szFrets[18][5];
extern TCHAR g_szRoots[12][8];

extern CObjGui*     g_gui;
extern CObjOptions* g_options;

#define GRID_COLOR	0xFFFFFF
//#define TEXT_COLOR	0xFFFFFF
#define FINGER_COLOR	0

CGuiGuitarChords::CGuiGuitarChords(void)
{
    //create some pens
    m_hPenNormal = CreatePen(0, GetSystemMetrics(SM_CXICON)/32, GRID_COLOR); 
    m_hPenThick  = CreatePen(0, GetSystemMetrics(SM_CXICON)/16, GRID_COLOR);

    DWORD dwVal = 0;

	m_eGUI = GUI_Chords;
}

CGuiGuitarChords::~CGuiGuitarChords(void)
{
    DeleteObject(m_hPenNormal);
    DeleteObject(m_hPenThick);
}

void CGuiGuitarChords::Init(HWND hWnd, HINSTANCE hInst)
{
    CGuiBase::Init(hWnd, hInst);
    g_options->GetGen()->Init(m_hInst);
}

BOOL CGuiGuitarChords::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(g_gui == NULL || g_options == NULL)
        return FALSE;

    g_gui->DrawBackground(rcClip);

    if(g_options && g_options->GetGen()->GetStartingFret() == 1)
        g_gui->DrawNut(rcClip);

    int iFade = 0;
    for(int i = 0; i < 6; i++)
    {
        if(g_options->GetGen()->GetFret(i) < 0)
            iFade = iFade | 1<<i;
        else if(i >= g_options->GetGen()->GetNumStrings())        
            iFade = iFade | 1<<i;
    }

    g_gui->DrawStrings(rcClip, iFade);

    DrawBridge(*g_gui->GetGDI(), rcClip);
    DrawGrid(*g_gui->GetGDI(), rcClip);
    DrawText(*g_gui->GetGDI(), rcClip);
#ifdef DEBUG
    DrawStar(*g_gui->GetGDI(), rcClip);
#endif

    return TRUE;
}

BOOL CGuiGuitarChords::DrawText(CIssGDIEx& gdi, RECT& rc)
{
    if(g_options == NULL)
        return FALSE;

    TCHAR szTemp[STRING_LARGE];
    
    //make sure its not null
    ::DrawText(gdi.GetDC(), g_szRoots[(int)g_options->GetRoot()], m_rcRoot, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    
    ::DrawText(gdi.GetDC(), g_options->GetGen()->GetChordText((int)g_options->GetChordType(), m_hInst), m_rcChord, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    
    //draw the chord variation and count
    m_oStr->Format(szTemp, _T("%i / %i"), g_options->GetGen()->GetIndex()+1, g_options->GetGen()->GetNumVariations());
    ::DrawText(gdi.GetDC(), szTemp, m_rcVariation, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Button), 0xFFFFFF);
    
	//Tuning Button
   	::DrawText(gdi.GetDC(), g_options->GetGen()->GetTuningText((int)g_options->GetTuning(), m_hInst, g_options->GetInstrument()), m_rcLeftRight, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, g_gui->GetFont(FONT_Button), 0xFFFFFF);


	//Fret labels on right
	int iTemp = g_options->GetGen()->GetStartingFret() - 1;

    static POINT ptFretOff = {(WIDTH(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetWidth())/2, (HEIGHT(m_rcFretNums[0])-g_gui->GetFingerGrey()->GetHeight())/2};


    //fret markers
    for(int i = 0; i < GetMaxFrets(); i++)
    {
        if(iTemp + i >= 17)
            continue;
		if(g_options->GetFretMarkers() == FMARKERS_Numbered)
		{
			m_oStr->IntToString(szTemp, iTemp + i + 1);
            if(m_oStr->GetLength(szTemp))
            {
                ::Draw(gdi, m_rcFretNums[i].left + ptFretOff.x , m_rcFretNums[i].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
                ::DrawText(gdi.GetDC(), szTemp, m_rcFretNums[i], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);
            }
		}
		else
        {
            if(m_oStr->GetLength(g_szFrets[iTemp+i]))
            {
			    ::Draw(gdi, m_rcFretNums[i].left + ptFretOff.x, m_rcFretNums[i].top + ptFretOff.y, g_gui->GetFingerGrey()->GetWidth(), g_gui->GetFingerGrey()->GetHeight(), *g_gui->GetFingerGrey());
                ::DrawText(gdi.GetDC(), g_szFrets[iTemp+i], m_rcFretNums[i], DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);
            }
        }
    }

    return TRUE;
}


//no bridging with the new class
BOOL CGuiGuitarChords::DrawBridge(CIssGDIEx& gdi, RECT& rc)
{

	return TRUE;
}

BOOL CGuiGuitarChords::DrawStar(CIssGDIEx& gdi, RECT& rc)
{
    if(IsRectInRect(rc, m_rcFav) == FALSE)
        return FALSE;


    if(g_options->GetGen()->IsStarLit())
        Draw(gdi, m_rcFav, 
             *g_gui->GetStar(FALSE));
    else
        Draw(gdi, m_rcFav, 
            *g_gui->GetStar(TRUE));

    return TRUE;
}

BOOL CGuiGuitarChords::DrawGrid(CIssGDIEx& gdi, RECT& rc)
{
#ifdef DRAW_GRID
    POINT pt[2];
    HPEN oldPen = (HPEN)SelectObject(gdi.GetDC(), m_hPenNormal);

    //outline
    FrameRect(gdi.GetDC(), m_rcGrid, GRID_COLOR, 1);
    //inner line
    for(int i = 1; i < 5; i++)
    {
        pt[0].x = m_rcGrid.left;
        pt[0].y = m_rcGrid.top + i*HEIGHT(m_rcGrid)/5;
        pt[1].x = m_rcGrid.right;
        pt[1].y = pt[0].y;

        Polyline(gdi.GetDC(), pt, 2);

        pt[0].x = m_rcGrid.left + i*WIDTH(m_rcGrid)/5;
        pt[1].x = pt[0].x;
        pt[0].y = m_rcGrid.top;
        pt[1].y = m_rcGrid.bottom;

        Polyline(gdi.GetDC(), pt, 2);
    }
    if(g_options->GetGen()->GetStartingFret() == 1)
    {
        SelectObject(gdi.GetDC(), m_hPenThick);

        //thick line at the top
        pt[0].x = m_rcGrid.left;
        pt[0].y = m_rcGrid.top;
        pt[1].x = m_rcGrid.right;
        pt[1].y = m_rcGrid.top;

        Polyline(gdi.GetDC(), pt, 2);

        SelectObject(gdi.GetDC(), m_hPenNormal);
    }
    SelectObject(gdi.GetDC(), oldPen);

#endif
    int y = 0;

	int iFret;

    int iX, iY;

    RECT rcTemp;
    POINT ptTemp;

    static POINT ptOffSet = {(0 - g_gui->GetNoNote()->GetWidth())/2, 
                       (HEIGHT(m_rcStringInidcators[0]) - g_gui->GetNoNote()->GetHeight())/2};

    for(int x = 0; x < g_options->GetGen()->GetNumStrings(); x++)
    {
        SetRect(&m_rcNotes[x], 0,0,0,0);

		if(g_options->GetGen()->GetFret(x) < 1)
		{
            ptTemp.y = m_rcStringInidcators[x].top;

            if(g_options->GetGen()->GetFret(x) == -1)
            {//not played
                ptTemp.x = g_gui->GetStringX(x) - g_gui->GetNoNote()->GetWidth()/2;

                ::Draw(gdi, 
                    ptTemp.x, 
                    ptTemp.y, 
                    g_gui->GetNoNote()->GetWidth(),
                    g_gui->GetNoNote()->GetHeight(),
                    *g_gui->GetNoNote());
            }
            else
            {
                ptTemp.x = g_gui->GetStringX(x) - g_gui->GetFingerGrey()->GetWidth()/2;
                ::Draw(gdi, 
                    ptTemp.x, 
                    ptTemp.y, 
                    g_gui->GetFingerGrey()->GetWidth(),
                    g_gui->GetFingerGrey()->GetHeight(),
                    *g_gui->GetFingerGrey());

                rcTemp.left = ptTemp.x;
                rcTemp.right = rcTemp.left + g_gui->GetFingerGrey()->GetWidth();
                rcTemp.top = ptTemp.y;
                rcTemp.bottom = m_rcStringInidcators[x].bottom;

                ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(x, 0), rcTemp, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Frets), 0);

                m_rcNotes[x] = rcTemp;
            }

            //all good for this loop
            continue;
        }

		//get the relative fret ...
		iFret = g_options->GetGen()->GetFret(x) - g_options->GetGen()->GetStartingFret();

    	y = iFret;

        iX = g_gui->GetStringX(x) - g_gui->GetFinger()->GetWidth()/2;
        iY = m_rcFingers[x][y].top + (HEIGHT(m_rcFingers[x][y]) - g_gui->GetFinger()->GetHeight())/2;//its fine if its negative


        Draw(gdi, iX, iY, g_gui->GetFinger()->GetWidth(), g_gui->GetFinger()->GetHeight(), *g_gui->GetFinger());

        rcTemp.left = iX;
        rcTemp.top = iY;
        rcTemp.bottom = rcTemp.top + g_gui->GetFinger()->GetHeight();
        rcTemp.right = rcTemp.left + g_gui->GetFinger()->GetWidth();
		
        //and draw the note
        ::DrawText(gdi.GetDC(), g_options->GetGen()->GetNote(x), rcTemp, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_Fingers), 0xEEDDDD);

        m_rcNotes[x] = rcTemp;
    }
	return TRUE;
}

BOOL CGuiGuitarChords::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iTopIndent = BUTTONS_HEIGHT + NUT_HEIGHT;
    int iLeftIndent = 84*GetSystemMetrics(SM_CXSCREEN)/480;
    int iWidth;
    int iHeight;

    //so lets draw some rects for now as fill
    iWidth = (WIDTH(rcClient))/4;

    m_rcRoot.top = rcClient.top;
    m_rcRoot.left = rcClient.left;
    m_rcRoot.bottom = m_rcRoot.top + BUTTONS_HEIGHT;
    m_rcRoot.right = m_rcRoot.left + iWidth;

    m_rcChord = m_rcRoot;
    m_rcChord.left = m_rcRoot.right;
    m_rcChord.right = m_rcChord.left + iWidth;

    m_rcVariation = m_rcRoot;
    m_rcVariation.left = m_rcChord.right;
    m_rcVariation.right = m_rcVariation.left + iWidth;

    m_rcLeftRight = m_rcRoot;
    m_rcLeftRight.left = m_rcVariation.right;
    m_rcLeftRight.right = m_rcLeftRight.left + iWidth;

    //and the grid ... just an outline for now
    //iWidth = IsVGA()?62:31;
    iHeight = IsVGA()?94:47;

    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
        iHeight = IsVGA()?62:31;//tweak this
    if(GetSystemMetrics(SM_CYSCREEN) == 480 && GetSystemMetrics(SM_CXSCREEN) == 320)
        iHeight = 58;

    m_rcGrid.left   = iLeftIndent + rcClient.left;
    m_rcGrid.right  = rcClient.right - iLeftIndent;
    m_rcGrid.top    = rcClient.top + iTopIndent;
    m_rcGrid.bottom = m_rcGrid.top + 5*iHeight;

    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
        m_rcGrid.top -= IsVGA()?8:4;


    //and now the important part
    for(int x = 0; x < 6; x++)
    {
        for(int y = 0; y < GetMaxFrets(); y++)
        {
            m_rcFingers[x][y].left = m_rcGrid.left + WIDTH(m_rcGrid)*x/5;
            m_rcFingers[x][y].right = m_rcFingers[x][y].left + WIDTH(m_rcGrid)/5;
            m_rcFingers[x][y].top = m_rcGrid.top + HEIGHT(m_rcGrid)*y/5;
            m_rcFingers[x][y].bottom = m_rcFingers[x][y].top + HEIGHT(m_rcGrid)/5;
        }

        m_rcStringInidcators[x].left = m_rcFingers[x][0].left;
        m_rcStringInidcators[x].right = m_rcFingers[x][0].right;
        m_rcStringInidcators[x].top = rcClient.top + BUTTONS_HEIGHT;// + (IsVGA()?2:1);
        m_rcStringInidcators[x].bottom = m_rcFingers[x][0].top;
    }

    for(int y = 0; y < GetMaxFrets(); y++)
    {
        SetRect(&m_rcFretNums[y], rcClient.right - iLeftIndent/2, m_rcFingers[0][y].top, 
            rcClient.right, m_rcFingers[0][y].bottom);
    }

    m_rcFav.left= IsVGA()?2:1;
    m_rcFav.top = m_rcFretNums[2].top + (HEIGHT(m_rcFretNums[2]) - IsVGA()?50:25)/2;
    m_rcFav.right= m_rcFav.left + (IsVGA()?50:25);
    m_rcFav.bottom = m_rcFav.top + (IsVGA()?50:25);

    return TRUE;
}

BOOL CGuiGuitarChords::OnLButtonDown(HWND hWnd, POINT& pt)
{
    m_ptLastMove = pt;
    return TRUE;
}

BOOL CGuiGuitarChords::OnMouseMove(HWND, POINT& pt)
{
    if(g_options->GetStrumStyle() != STRUM_Off)
        return UNHANDLED;

    UINT uiTemp;

    if(g_gui->GetPlaySounds() == FALSE)
        return UNHANDLED;

    //check each string
    for(int i = 0; i < g_options->GetGen()->GetNumStrings(); i++)
    {
        if(min(pt.x, m_ptLastMove.x) < g_gui->GetStringX(i) &&
            max(pt.x, m_ptLastMove.x) > g_gui->GetStringX(i))
        {   //play the string
            uiTemp = GetNoteRes(i);
            if(uiTemp != 0)
            {
                g_gui->PlaySFX(uiTemp - LOWEST_SOUND_RES);
                Sleep(5);//so we don't overwork the sound engine
            }
        }
    }
    
    m_ptLastMove = pt;

    return TRUE;
}

BOOL CGuiGuitarChords::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_UP:
        g_options->GetGen()->PrevChord();
        g_gui->PlayClick();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case VK_DOWN:
        g_options->GetGen()->NextChord();
        g_gui->PlayClick();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case VK_RETURN:
        PlayChord();
        break;
    default:
        return UNHANDLED;
        break;
    }
    return TRUE;    
}


BOOL CGuiGuitarChords::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcRoot, pt))
    {
        OnRoot();
    }
#ifdef DEBUG
    else if(PtInRect(&m_rcFav, pt))
    {
        OnFavorite();
    }
#endif
    else if(PtInRect(&m_rcChord, pt))
    {
        OnChord();
    }
    else if(PtInRect(&m_rcVariation, pt))
    {
        OnVariation();
    }
    else if(PtInRect(&m_rcLeftRight, pt))
    {
		OnTuning();
    }
	else 
	{   //we'll check the mode in Play Chord
        if(g_options->GetStrumStyle() == STRUM_Off)
        {

        }
        else
        {
            //check for the individual notes ... otherwise strum
            for(int i = 0; i < g_options->GetGen()->GetNumStrings(); i++)
            {
                if(PtInRect(&m_rcNotes[i], pt))
                {
                    //play the note
                    UINT uiTemp = GetNoteRes(i);
                    if(uiTemp != 0)
                        g_gui->PlaySFX(uiTemp - LOWEST_SOUND_RES);

                    return TRUE;
                }
            }


            PlayChord();
        }
	}

    return TRUE;
}

BOOL CGuiGuitarChords::OnChord()
{
    g_gui->m_wndMenu.ResetContent();

    for(int i = 0; i < CHORD_Count; i++)
        g_gui->m_wndMenu.AddItem(g_options->GetGen()->GetChordText(i, m_hInst), IDMENU_ChordType+i, 0);

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetChordType(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_Bounce|OPTION_DrawScrollArrows|OPTION_DrawScrollBar,
        0,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}

BOOL CGuiGuitarChords::OnTuning()
{
    g_gui->m_wndMenu.ResetContent();

   // for(int i = 0; i < TUNING_Count; i++)
   //     g_gui->m_wndMenu.AddItem(g_options->GetGen()->GetTuningLongText(i, m_hInst), IDMENU_Tuning+i, 0);

    for(int i = 0; i < g_options->GetGen()->GetNumTuning(); i++)
    {
        g_gui->m_wndMenu.AddItem(g_options->GetGen()->GetTuningLongText(i, m_hInst), IDMENU_Tuning+i, 0);
    }

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetTuning(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    DWORD dwTemp = OPTION_AlwaysShowSelector|OPTION_Bounce|OPTION_DrawScrollArrows;

    if(g_options->GetGen()->GetNumTuning() > 20)
        dwTemp = dwTemp | OPTION_DrawScrollBar;

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        dwTemp,
        0,rcClient.top,
        rcClient.right, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}

BOOL CGuiGuitarChords::OnRoot()
{
    g_gui->m_wndMenu.ResetContent();

    for(int i = 0; i < ROOT_Count; i++)
        g_gui->m_wndMenu.AddItem(g_szRoots[i], IDMENU_Root+i, 0);

    g_gui->m_wndMenu.SetSelectedItemIndex((int)g_options->GetRoot(), TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector | OPTION_Bounce|OPTION_DrawScrollArrows|OPTION_DrawScrollBar,
        0,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        0,0,0,0, ADJUST_Bottom);


    return TRUE;
}

BOOL CGuiGuitarChords::OnVariation()
{
    g_options->GetGen()->NextChord();
    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

BOOL CGuiGuitarChords::OnFavorite()
{
    g_options->GetGen()->OnFavorites();

    InvalidateRect(m_hWnd, &m_rcFav, FALSE);
    return TRUE;
}

BOOL CGuiGuitarChords::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam >= IDMENU_Root && wParam < IDMENU_Root + ROOT_Count)
    {   //handle the roots
		g_options->SetRoot(EnumChordRoot(wParam - IDMENU_Root));
		InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
    else if(wParam >= IDMENU_ChordType && wParam < IDMENU_ChordType + CHORD_Count)
    {   
		g_options->SetChordType(EnumChordType(wParam - IDMENU_ChordType));
		InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
	else if(wParam >= IDMENU_Tuning && wParam < IDMENU_Tuning + TUNING_Count)
	{
		//g_options->SetTuning(EnumTuning(wParam - IDMENU_Tuning));
		g_options->SetTuning(wParam - IDMENU_Tuning);
		InvalidateRect(hWnd, NULL, FALSE);
		return TRUE;
	}
    return UNHANDLED;
}

UINT CGuiGuitarChords::GetNoteRes(int iString)
{
	int iNote = g_options->GetGen()->GetNoteValue(iString);
	if(iNote == -1)
		return 0;//since its a UINT ....
	
	return GetNoteRes(iString, g_options->GetGen()->GetFret(iString));
}


UINT CGuiGuitarChords::GetNoteRes(int iString, int iFret)
{
    return g_options->GetGen()->GetNoteRes(iString, iFret);
}

void CGuiGuitarChords::PlayChord()
{
    if(g_options && g_gui->GetPlaySounds() == FALSE)
        return;

    UINT uiTemp = 0;

    int iSleep = 130;
    if(g_options->GetStrumStyle() == STRUM_Fast)
        iSleep = 0;
    
	g_gui->StopSFX();

	for(int i = 0; i < g_options->GetGen()->GetNumStrings(); i++)
	{
		uiTemp = GetNoteRes(i);
		if(uiTemp != 0)
		{
			g_gui->PlaySFX(uiTemp - LOWEST_SOUND_RES);

			iSleep = max(iSleep, 35);
            Sleep(iSleep);
			iSleep -= 15;
		}
	}
}
