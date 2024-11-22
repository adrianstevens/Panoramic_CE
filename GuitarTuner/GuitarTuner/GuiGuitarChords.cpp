#include "GuiGuitarChords.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "Resource.h"
#include "IssRegistry.h"

extern TCHAR g_szChords[(int)CHORD_Count][STRING_NORMAL]; 
extern TCHAR g_szFrets[18][5];
extern TCHAR g_szRoots[12][8];
extern TCHAR g_szTunings[(int)TUNING_Count][STRING_NORMAL];
extern TCHAR g_szTuningsLong[(int)TUNING_Count][STRING_NORMAL];

#define GRID_COLOR	0xFFFFFF
//#define TEXT_COLOR	0xFFFFFF
#define FINGER_COLOR	0

CGuiGuitarChords::CGuiGuitarChords(void)
{
    //create some fonts
    m_hFontSmall = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_NORMAL, TRUE);
    m_hFontNormal = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*15/32, FW_NORMAL, TRUE);
    m_hFontLarge = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*18/32, FW_NORMAL, TRUE);

    //create some pens
    m_hPenNormal = CreatePen(0, GetSystemMetrics(SM_CXICON)/32, GRID_COLOR); 
    m_hPenThick  = CreatePen(0, GetSystemMetrics(SM_CXICON)/16, GRID_COLOR);

	DWORD dwVal = 0;

	m_eGUI = GUI_Chords;

	LoadRegistry();
}

CGuiGuitarChords::~CGuiGuitarChords(void)
{
    CIssGDIEx::DeleteFont(m_hFontSmall);
    CIssGDIEx::DeleteFont(m_hFontNormal);
    CIssGDIEx::DeleteFont(m_hFontLarge);

    DeleteObject(m_hPenNormal);
    DeleteObject(m_hPenThick);

	SaveRegistry();
}

void CGuiGuitarChords::Init(HWND hWnd, HINSTANCE hInst)
{
    CGuiBase::Init(hWnd, hInst);
	LoadImages(hWnd, hInst);
	LoadSounds();
}

BOOL CGuiGuitarChords::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(m_gdiMem.GetDC() == NULL)
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

 //   FillRect(m_gdiMem, rcClient, 0xFFFFFF);
	DrawBackground(m_gdiMem, rcClient);

    m_imgBlue.DrawImage(m_gdiMem, m_rcRoot.left, m_rcRoot.top);
    m_imgBlue.DrawImage(m_gdiMem, m_rcChord.left, m_rcChord.top);
    m_imgGrey.DrawImage(m_gdiMem, m_rcVariation.left, m_rcVariation.top);
    m_imgRed.DrawImage(m_gdiMem, m_rcLeftRight.left, m_rcLeftRight.top);

    DrawBridge(m_gdiMem, rcClip);
	DrawGrid(m_gdiMem, rcClip);
    DrawText(m_gdiMem, rcClip);
	
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

BOOL CGuiGuitarChords::DrawText(CIssGDIEx& gdi, RECT& rc)
{
    TCHAR szTemp[STRING_LARGE];
    
    //make sure its not null
    ::DrawText(gdi.GetDC(), g_szRoots[(int)m_oChords.GetRoot()], m_rcRoot, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);
    
    ::DrawText(gdi.GetDC(), g_szChords[(int)m_oChords.GetType()], m_rcChord, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);
    
    //draw the chord variation and count
    m_oStr->Format(szTemp, _T("%i / %i"), m_oChords.GetIndex()+1, m_oChords.GetNumVariations());
    ::DrawText(gdi.GetDC(), szTemp, m_rcVariation, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);
    
/*    if(m_oChords.IsLeftHanded())
        ::DrawText(gdi.GetDC(), _T("Left"), m_rcLeftRight, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);
    else
        ::DrawText(gdi.GetDC(), _T("Right"), m_rcLeftRight, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);
*/	
	//Tuning Button
	::DrawText(gdi.GetDC(), g_szTunings[(int)m_oChords.GetTuning()], m_rcLeftRight, DT_CENTER | DT_VCENTER, m_hFontLarge, 0xFFFFFF);


	//Fret labels on right
	int iTemp = m_oChords.GetStartingFret() - 1;

    for(int i = 0; i < 5; i++)
    {
        if(iTemp + i >= 17)
            continue;
		if(m_oChords.NumberFrets())
		{
			m_oStr->IntToString(szTemp, iTemp + i + 1);
			::DrawText(gdi.GetDC(), szTemp, m_rcFretNums[i], DT_CENTER | DT_VCENTER, m_hFontNormal, TEXT_COLOR);
		}
		else
			::DrawText(gdi.GetDC(), g_szFrets[iTemp+i], m_rcFretNums[i], DT_CENTER | DT_VCENTER, m_hFontNormal, TEXT_COLOR);
    }

	//now lets draw some notes
    for(int i = 0; i < 6; i++)
    {
        iTemp = i;
        if(m_oChords.IsLeftHanded())
            iTemp = 5 - i;

		if(m_oChords.GetFret(iTemp) == 0)
     //   if(oChord->btFingerPos[iTemp] == 0)
            ::DrawText(gdi.GetDC(), _T("o"), m_rcPlayType[i], DT_CENTER | DT_VCENTER, m_hFontSmall, TEXT_COLOR);
     //   else if(oChord->btFingerPos[iTemp] == 1)
		else if(m_oChords.GetFret(iTemp) == -1)
            ::DrawText(gdi.GetDC(), _T("x"), m_rcPlayType[i], DT_CENTER | DT_VCENTER, m_hFontSmall, TEXT_COLOR);
        
        //::DrawText(gdi.GetDC(), oChord->szNote[iTemp], m_rcNote[i], DT_CENTER | DT_VCENTER, m_hFontSmall, 0);
		::DrawText(gdi.GetDC(), m_oChords.GetNote(iTemp), m_rcNote[i], DT_CENTER | DT_VCENTER, m_hFontSmall, TEXT_COLOR);
    }

    return TRUE;
}


//no bridging with the new class
BOOL CGuiGuitarChords::DrawBridge(CIssGDIEx& gdi, RECT& rc)
{
/*	ChordType* oChord;
	oChord = m_oChords.GetCurrentCord();

	if(oChord == NULL)
		return FALSE;

	if(oChord->btBridge1[0] == GUITAR_BLANK)
		return TRUE;

	//draw the first bridge
	HPEN penBlue = CreatePen(0, 2, RGB(0,50,255));
	HPEN penOld = (HPEN)SelectObject(gdi.GetDC(), penBlue);
	
	POINT pt[2]; //enough for now
	
	pt[0].x = m_rcFingers[oChord->btBridge1[1]-1][oChord->btBridge1[0]-1].left + WIDTH(m_rcFingers[0][0])/2;
	pt[0].y = m_rcFingers[oChord->btBridge1[1]-1][oChord->btBridge1[0]-1].top + HEIGHT(m_rcFingers[0][0])/2;
	pt[1].x = m_rcFingers[oChord->btBridge1[2]-1][oChord->btBridge1[0]-1].left + WIDTH(m_rcFingers[0][0])/2;
	pt[1].y = pt[0].y;

	Polyline(gdi.GetDC(), pt, 2);


	if(oChord->btBridge2[0] == GUITAR_BLANK)
		goto Error;

	pt[0].x = m_rcFingers[oChord->btBridge2[1]-1][oChord->btBridge2[0]-1].left + WIDTH(m_rcFingers[0][0])/2;
	pt[0].y = m_rcFingers[oChord->btBridge2[1]-1][oChord->btBridge2[0]-1].top + HEIGHT(m_rcFingers[0][0])/2;
	pt[1].x = m_rcFingers[oChord->btBridge2[2]-1][oChord->btBridge2[0]-1].left + WIDTH(m_rcFingers[0][0])/2;
	pt[1].y = pt[0].y;


	Polyline(gdi.GetDC(), pt, 2);


Error:
	SelectObject(gdi.GetDC(), penOld);*/
	return TRUE;
}

BOOL CGuiGuitarChords::DrawGrid(CIssGDIEx& gdi, RECT& rc)
{
	FrameRect(gdi.GetDC(), m_rcGrid, GRID_COLOR, 1);

    POINT pt[2];

    HPEN oldPen = (HPEN)SelectObject(gdi.GetDC(), m_hPenNormal);

    //just the inner lines ... we'll leave the outline
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

    if(m_oChords.GetStartingFret() == 1)
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

    HBRUSH oldBrush = (HBRUSH)SelectObject(gdi.GetDC(), GetStockObject(DKGRAY_BRUSH));

    int y = 0;

//    TCHAR szTemp[4];

    int iTemp = 0;
	int iTemp2 = 0;

	int iFret;

	SelectObject(gdi.GetDC(), GetStockObject(BLACK_PEN));

    for(int x = 0; x < 6; x++)
    {
        iTemp = x;
        if(m_oChords.IsLeftHanded())
            iTemp = 5 - x;

		if(m_oChords.GetFret(iTemp) < 1)
			continue;

		//get the relative fret ...
		iFret = m_oChords.GetFret(iTemp) - m_oChords.GetStartingFret();

    	y = iFret;
   //     Ellipse(gdi.GetDC(), m_rcFingers[x][y].left, m_rcFingers[x][y].top, 
   //         m_rcFingers[x][y].right, m_rcFingers[x][y].bottom);

		Draw(gdi, m_rcFingers[x][y], m_gdiFinger);


    }

    SelectObject(gdi.GetDC(), oldBrush);
    SelectObject(gdi.GetDC(), oldPen);

	return TRUE;
}

BOOL CGuiGuitarChords::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    m_gdiMem.Destroy();

    int iIndent = GetSystemMetrics(SM_CXSMICON)/5;
    int iWidth;
    int iHeight;

    //so lets draw some rects for now as fill
    iWidth = (WIDTH(rcClient) - 5*iIndent)/4;

    m_rcRoot.top = iIndent + rcClient.top;
    m_rcRoot.left = iIndent + rcClient.left;
    m_rcRoot.bottom = m_rcRoot.top + GetSystemMetrics(SM_CXICON);
    m_rcRoot.right = m_rcRoot.left + iWidth;

    m_rcChord = m_rcRoot;
    m_rcChord.left = m_rcRoot.right + iIndent;
    m_rcChord.right = m_rcChord.left + iWidth;

    m_rcVariation = m_rcRoot;
    m_rcVariation.left = m_rcChord.right + iIndent;
    m_rcVariation.right = m_rcVariation.left + iWidth;

    m_rcLeftRight = m_rcRoot;
    m_rcLeftRight.left = m_rcVariation.right + iIndent;
    m_rcLeftRight.right = m_rcLeftRight.left + iWidth;


    //set the top text here ....
    iHeight = 0;
    iHeight = (rcClient.bottom - m_rcChord.bottom - GetSystemMetrics(SM_CXICON))/5;

    m_rcFretNums[0].top    = m_rcChord.bottom + GetSystemMetrics(SM_CXSMICON);
    m_rcFretNums[0].bottom = m_rcFretNums[0].top + iHeight;
    m_rcFretNums[0].right  = rcClient.right - 2*iIndent;
    m_rcFretNums[0].left   = m_rcFretNums[0].right - GetSystemMetrics(SM_CXSMICON) - 2*iIndent;
    
    m_rcFretNums[1].top    = m_rcFretNums[0].bottom;
    m_rcFretNums[1].bottom = m_rcFretNums[1].top + iHeight;
    m_rcFretNums[1].right  = m_rcFretNums[0].right;
    m_rcFretNums[1].left   = m_rcFretNums[0].left;
    
    m_rcFretNums[2].top    = m_rcFretNums[1].bottom;
    m_rcFretNums[2].bottom = m_rcFretNums[2].top + iHeight;
    m_rcFretNums[2].right  = m_rcFretNums[0].right;
    m_rcFretNums[2].left   = m_rcFretNums[0].left;

    m_rcFretNums[3].top    = m_rcFretNums[2].bottom; 
    m_rcFretNums[3].bottom = m_rcFretNums[3].top + iHeight;
    m_rcFretNums[3].right  = m_rcFretNums[0].right;
    m_rcFretNums[3].left   = m_rcFretNums[0].left;

    m_rcFretNums[4].top    = m_rcFretNums[3].bottom;  
    m_rcFretNums[4].bottom = m_rcFretNums[4].top + iHeight; 
    m_rcFretNums[4].right  = m_rcFretNums[0].right;
    m_rcFretNums[4].left   = m_rcFretNums[0].left;

    //and the grid ... just an outline for now
    m_rcGrid.left   = GetSystemMetrics(SM_CXSMICON);
    m_rcGrid.right  = m_rcFretNums[0].left - GetSystemMetrics(SM_CXSMICON);
    m_rcGrid.top    = m_rcFretNums[0].top;
    m_rcGrid.bottom = m_rcFretNums[4].bottom;

    iWidth = WIDTH(m_rcGrid)/5;
    iHeight = HEIGHT(m_rcGrid)/5;//redefined to spacing

    for(int i = 0; i < 6; i++)
    {
        m_rcPlayType[i].left    = m_rcGrid.left - GetSystemMetrics(SM_CXSMICON)/2 + i*iWidth;
        m_rcPlayType[i].right   = m_rcGrid.left + GetSystemMetrics(SM_CXSMICON) + i*iWidth;
        m_rcPlayType[i].bottom  = m_rcGrid.top;
        m_rcPlayType[i].top     = m_rcGrid.top - GetSystemMetrics(SM_CXSMICON);

        m_rcNote[i].left        = m_rcGrid.left - GetSystemMetrics(SM_CXSMICON)/2 + i*iWidth;
        m_rcNote[i].right       = m_rcGrid.left + GetSystemMetrics(SM_CXSMICON) + i*iWidth;
        m_rcNote[i].top         = m_rcGrid.bottom;
        m_rcNote[i].bottom      = m_rcGrid.bottom + GetSystemMetrics(SM_CXSMICON);
    }

    int iSize = min(iWidth-2, iHeight-2);

    //and now the important
    for(int x = 0; x < 6; x++)
    {
        for(int y = 0; y < 5; y++)
        {
        //    m_rcFingers[x][y].left = m_rcGrid.left - iSize/2 + iWidth*x;
        //    m_rcFingers[x][y].right = m_rcFingers[x][y].left + iSize;
        //    m_rcFingers[x][y].top = m_rcGrid.top - iSize/2 + iHeight*y + iHeight/2;
        //    m_rcFingers[x][y].bottom = m_rcFingers[x][y].top + iSize;

			m_rcFingers[x][y].left = m_rcGrid.left - iSize/2 + WIDTH(m_rcGrid)*x/5;
			m_rcFingers[x][y].right = m_rcFingers[x][y].left + iSize;
			m_rcFingers[x][y].top = m_rcGrid.top - iSize/2 + HEIGHT(m_rcGrid)*y/5 + iHeight/2;
			m_rcFingers[x][y].bottom = m_rcFingers[x][y].top + iSize;
        }
    }

	m_gdiFinger.Destroy();

    return TRUE;
}

BOOL CGuiGuitarChords::OnLButtonDown(HWND hWnd, POINT& pt)
{
    
    return TRUE;
}

BOOL CGuiGuitarChords::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcRoot, pt))
    {
        OnRoot();
    }
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
    //    m_oChords.SetLeftHanded(!m_oChords.IsLeftHanded());
    //    InvalidateRect(hWnd, NULL, FALSE);
		OnTuning();
    }
	else
	{
		PlayChord();
	}

    return TRUE;
}

BOOL CGuiGuitarChords::LoadImages(HWND hWnd, HINSTANCE hInst)
{
    //btn slice time
    if(!m_imgBlue.IsLoaded())
        m_imgBlue.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnGreenVGA:IDB_PNG_BtnGreen);

    if(!m_imgRed.IsLoaded())
        m_imgRed.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnRedVGA:IDB_PNG_BtnRed);

    if(!m_imgGrey.IsLoaded())
        m_imgGrey.Initialize(hWnd, hInst, IsVGA()?IDB_PNG_BtnVGA:IDB_PNG_Btn);

    m_imgBlue.SetSize(WIDTH(m_rcRoot), HEIGHT(m_rcRoot));
    m_imgRed.SetSize(WIDTH(m_rcRoot), HEIGHT(m_rcRoot));
    m_imgGrey.SetSize(WIDTH(m_rcRoot), HEIGHT(m_rcRoot));

	if(m_gdiFinger.GetDC() == NULL)
	{
		SIZE size;
		size.cx = WIDTH(m_rcFingers[0][0]);
		size.cy = HEIGHT(m_rcFingers[0][0]);

		m_gdiFinger.Create(m_gdiMem.GetDC(), size.cx, size.cy);

		CIssGDIEx gdiTemp;
		gdiTemp.LoadImage(IDB_PNG_Finger, hWnd, hInst);

		ScaleImage(gdiTemp, m_gdiFinger, size, FALSE, 0xffffff);
	}

    return TRUE;
}

BOOL CGuiGuitarChords::OnChord()
{
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

    ///    TCHAR szText[STRING_MAX];

    for(int i = 0; i < CHORD_Count; i++)
        AppendMenu(hMenu, MF_STRING, IDMENU_ChordType+i, g_szChords[i]);

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    pt.x	= m_rcChord.left;
    pt.y    = m_rcChord.bottom + GetSystemMetrics(SM_CXSMICON);
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

    DestroyMenu(hMenu);
 

    return TRUE;
}

BOOL CGuiGuitarChords::OnTuning()
{
	HMENU hMenu = CreatePopupMenu();
	if(!hMenu)
		return FALSE;

	///    TCHAR szText[STRING_MAX];

	for(int i = 0; i < TUNING_Count; i++)
		AppendMenu(hMenu, MF_STRING, IDMENU_Tuning+i, g_szTuningsLong[i]);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= m_rcLeftRight.right;
	pt.y    = m_rcLeftRight.bottom + GetSystemMetrics(SM_CXSMICON);
	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

	DestroyMenu(hMenu);

	return TRUE;
}

BOOL CGuiGuitarChords::OnRoot()
{
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return FALSE;

///    TCHAR szText[STRING_MAX];

    for(int i = 0; i < ROOT_Count; i++)
        AppendMenu(hMenu, MF_STRING, IDMENU_Root+i, g_szRoots[i]);


    /*   int iChecked = 0;
    if(m_oBtnManger.GetCalcType() == CALC_Standard)
    iChecked = 0;
    else if(m_oBtnManger.GetCalcType() == CALC_Scientific)
    iChecked = 1;
    else if(m_oBtnManger.GetCalcType() == CALC_BaseConversions)
    iChecked = 2;

    MENUITEMINFO sMenu;
    memset(&sMenu, 0, sizeof(MENUITEMINFO));//zero it out
    sMenu.cbSize = sizeof(MENUITEMINFO);//set the size
    sMenu.fMask = MIIM_STATE;//state for selected
    sMenu.fType = MFT_STRING;//we working on a string entry ... might as well set the radio while we're at it
    sMenu.fState = MFS_DEFAULT; // that's what we wanted  - checked for the radio 
    SetMenuItemInfo(hMenu, (UINT)iChecked, TRUE, &sMenu);
    //this should be doable using SetMenuItemInfo but I couldn't figure it out 

    CheckMenuRadioItem(hMenu, 0, 3, iChecked, MF_BYPOSITION);*/

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    pt.x	= m_rcRoot.left;
    pt.y    = m_rcRoot.bottom + GetSystemMetrics(SM_CXSMICON);
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

    DestroyMenu(hMenu);

    return TRUE;
}

BOOL CGuiGuitarChords::OnVariation()
{
//    m_oChords.NextChord();
	m_oChords.NextChord();
    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

BOOL CGuiGuitarChords::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam >= IDMENU_Root && wParam < IDMENU_Root + ROOT_Count)
    {   //handle the roots
		m_oChords.SetRoot(EnumChordRoot(wParam - IDMENU_Root));
		InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
    else if(wParam >= IDMENU_ChordType && wParam < IDMENU_ChordType + CHORD_Count)
    {   
		m_oChords.SetChord(EnumChordType(wParam - IDMENU_ChordType));
		InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
	else if(wParam >= IDMENU_Tuning && wParam < IDMENU_Tuning + TUNING_Count)
	{
		m_oChords.SetTuning(EnumTuning(wParam - IDMENU_Tuning));
		InvalidateRect(hWnd, NULL, FALSE);
		return TRUE;
	}
    return UNHANDLED;
}

void CGuiGuitarChords::LoadRegistry()
{
	DWORD dwTemp;

	//regional settings
	if(S_OK == GetKey(REG_KEY, _T("LeftHanded"), dwTemp))
		m_oChords.SetLeftHanded((BOOL)dwTemp);
	else 
		m_oChords.SetLeftHanded(FALSE);

	if(S_OK == GetKey(REG_KEY, _T("ShowNumbers"), dwTemp))
		m_oChords.SetNumberFrets((BOOL)dwTemp);
	else 
		m_oChords.SetNumberFrets(FALSE);

	if(S_OK == GetKey(REG_KEY, _T("Root"), dwTemp))
		m_oChords.SetRoot((EnumChordRoot)dwTemp, FALSE);

	if(S_OK == GetKey(REG_KEY, _T("Chord"), dwTemp))
		m_oChords.SetChord((EnumChordType)dwTemp, FALSE);

	if(S_OK == GetKey(REG_KEY, _T("Tuning"), dwTemp))
		m_oChords.SetTuning((EnumTuning)dwTemp);
}

void CGuiGuitarChords::SaveRegistry()
{
	DWORD dwValue;

	dwValue = (DWORD)m_oChords.IsLeftHanded();
	SetKey(REG_KEY, _T("leftHanded"), dwValue);

	dwValue = (DWORD)m_oChords.NumberFrets();
	SetKey(REG_KEY, _T("ShowNumbers"), dwValue);

	dwValue = (DWORD)m_oChords.GetRoot();
	SetKey(REG_KEY, _T("Root"), dwValue);

	dwValue = (DWORD)m_oChords.GetType();
	SetKey(REG_KEY, _T("Chord"), dwValue);

	dwValue = (DWORD)m_oChords.GetTuning();
	SetKey(REG_KEY, _T("Tuning"), dwValue);
}

UINT CGuiGuitarChords::GetNoteRes(int iString)
{
	int iNote = m_oChords.GetNoteValue(iString);
	if(iNote == -1)
		return 0;//since its a UINT ....
	
	return GetNoteRes(iString, m_oChords.GetFret(iString));
}

UINT CGuiGuitarChords::GetNoteRes(int iString, int iFret)
{
	UINT uiRet = -1;

	int iNote = m_oChords.GetOpenNote(iString);

	switch(iString)
	{
	case 0:
		uiRet = iNote + iFret + IDR_WAV_AMid - 12;
		break;
	case 1:
	case 2:
		uiRet = iNote + iFret + IDR_WAV_AMid;
		break;
	case 3:
	case 4:	//should be good for both the G & B strings 
		uiRet = iNote + iFret + IDR_WAV_AMid;
		if(iNote < 8)//assume its been tuned up
			uiRet += 12;
	    break;
	case 5:
		uiRet = iNote + iFret + IDR_WAV_AHi;
		break;
	}

	//now check ranges
	while(uiRet < IDR_WAV_CLow)
		uiRet += 12;
	while(uiRet > IDR_WAV_AVHi)
		uiRet -= 12;

	return uiRet;
}

void CGuiGuitarChords::PlayChord()
{
	UINT uiTemp = 0;
	int iSleep = 130;
	for(int i = 0; i < 6; i++)
	{
		uiTemp = GetNoteRes(i);
		if(uiTemp != 0)
		{
			m_oSoundFX.PlaySFX(uiTemp - IDR_WAV_CLow);
//#ifdef DEBUG
//			Sleep(350);
//#else
			Sleep(iSleep);
//#endif
			iSleep -= 15;
		}
	}
}

void CGuiGuitarChords::LoadSounds()
{
	if(m_oSoundFX.IsLoaded())
		return;

	int iRet = 0;

	iRet = m_oSoundFX.Initialize(m_hInst);

	for(UINT i = IDR_WAV_CLow; i <= IDR_WAV_AVHi; i++)
		m_oSoundFX.LoadSFX(i);

	m_oSoundFX.SetVolumeSFX(25);
}