#include "IssWndGraph.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "math.h"
#include "globals.h"
#ifdef UNDER_CE
#include "DlgGetFile.h"
#else

#endif

#define GRAPH_COLOR_1 RGB(0,255,0)
#define GRAPH_COLOR_2 RGB(0,128,255)
#define GRAPH_COLOR_3 RGB(236,0,140)
#define GRAPH_COLOR_4 RGB(255,252,0)
#define GRAPH_COLOR_5 RGB(255,63,0)
#define GRAPH_COLOR_6 RGB(127,255,255)
#define GRAPH_COLOR_7 RGB(120,120,120)
#define GRAPH_COLOR_8 RGB(255,0,255)
#define GRAPH_COLOR_9 RGB(0,0,0)
#define GRAPH_COLOR_10 RGB(0,0,0)

//#define GRAPH_TEXT_COLOR RGB(237,28,36)
#define GRAPH_TEXT_COLOR RGB(255,255,255) //(218,218,218)
#define GRAPH_AXIS_COLOR RGB(155,175,193) //(255,255,255)
#define GRAPH_BACKGROUND RGB(0,0,0) //(0,0,0)
#define GRAPH_LINE_COLOR RGB(38,48,52) //(50,50,50)

#define INDENT_TEXT (GetSystemMetrics(SM_CXSMICON)/2)

LPCTSTR lpFilter = 
_T("Bitmaps (*.bmp)\0*.bmp\0")
_T("All Files (*.*)\0*.*\0")
_T("");

CWndGraph::CWndGraph(void)
{
	m_oCalc		= SciCalc::Instance();
	m_oSciUtil	= CIssSciCalcUtil::Instance();

	m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), 400, TRUE);

	for(int i = 0; i < MAX_GRAPHS; i++)
	{
		m_szGraphs[i] = NULL;
		m_iYGraph[i]  = NULL;
	}

	m_bDrawLines	= TRUE;
	m_bDrawGrid		= TRUE;
	m_bMouseMoved	= FALSE;
	m_bLButtonDown	= FALSE;
	m_eGraphMode	= MODE_Translate;
    m_eZoomMode     = ZOOM_Normal;

	m_iGraphSelected= 0;
	m_iNumPoints	= 0;
	m_iAcuracy		= 0;

    for(int i = 0; i < 8; i++)
        m_hPen[i]= CreatePen(PS_SOLID, 1, GetGraphColor(i));

	m_crBackGround	= GRAPH_BACKGROUND;
	m_crText		= GRAPH_TEXT_COLOR;
	m_crLineColor	= GRAPH_LINE_COLOR;
	m_crAxisColor	= GRAPH_AXIS_COLOR;
}

CWndGraph::~CWndGraph(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);

	//should probably clean up those arrays
	for(int i = 0; i < MAX_GRAPHS; i++)
	{
		if(m_iYGraph[i] != NULL)
		{
			delete [] m_iYGraph[i];
			m_iYGraph[i] = NULL;
		}
		if(m_szGraphs[i] != NULL)
			m_oStr->Delete(&m_szGraphs[i]);

        //Woh ... that was a leak ... fixed
        if(m_hPen[i]!=NULL)
		{
			DeleteObject((HPEN)m_hPen[i]);
			m_hPen[i] = NULL;
		}
	}
	if(m_hOldPen!=NULL)
	{
        DeleteObject((HPEN)m_hOldPen);
		m_hOldPen = NULL;
	}
	if(m_hPenGrid!=NULL)
	{
		DeleteObject((HPEN)m_hPenGrid);
		m_hPenGrid = NULL;
	}
	if(m_hPenSelect!=NULL)
	{
		DeleteObject((HPEN)m_hPenSelect);
		m_hPenSelect = NULL;
	}
}

BOOL CWndGraph::CalcGraphPoints()
{
	int i			= 0;

	TCHAR szEq[STR_MAX];	//I really don't wanna leak
	TCHAR szDoubleTemp[STR_NORMAL];			//convert our doubles to string for the ol calc engine

	//first thing is to figure out the start point and step size
	
	double dbStart	= -1.0*m_dbXTranslation;
	double dbStep	= m_dbXScale/((double)m_iScreenWidth/(1+m_iAcuracy));
	double dbAns	= 0;

	int iSize;

	iSize = sizeof(m_iYGraph[0]);

	for(i = 0; i < MAX_GRAPHS; i++)
	{
		if(m_oStr->IsEmpty(m_szGraphs[i]) == FALSE)
		{
			if(m_iYGraph[i] == NULL)
				m_iYGraph[i] = new long int[m_iScreenWidth];
					
			for(int j=0; j < m_iScreenWidth/(1+m_iAcuracy); j++)
			{
				//now screen points are relative to say...the SCREEN..so we don't need an x
				//don't forget the damn Y is upside down...we'll do that later
				//lets build our string to parse
				m_oStr->DoubleToString(szDoubleTemp, dbStart+(j*dbStep));
				m_oStr->Insert(szDoubleTemp, _T("("), 0);
				m_oStr->Concatenate(szDoubleTemp, _T(")"));
				m_oSciUtil->FindAndReplaceString(m_szGraphs[i], _T("X"), szDoubleTemp, szEq);

                if(szEq[0] == _T('-'))
                    m_oStr->Insert(szEq, _T("0"), 0);
				
				dbAns = m_oCalc->Calculate(szEq, szEq);

				//And finally we need to store it as an int into our array....leet math scilz
				//quick translation from real Y to pocket PC y screen location
				m_iYGraph[i][j] = YToScreen(dbAns);
			}
		}
	}

	return TRUE;
}

void CWndGraph::ResetValues()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	//lets set a proper scale based on the width
	m_dbXScale = 10.0;

//	m_dbYScale = 10.0;
	m_dbYScale = m_dbXScale*HEIGHT(rcClient)/WIDTH(rcClient);

	m_dbXTranslation = 5.0;
//	m_dbYTranslation = 5.0; //to center, this should be half of the scale
	m_dbYTranslation = m_dbYScale/2.0;

	CalcGraphPoints();
	InvalidateRect(m_hWnd, NULL, FALSE);
}


BOOL CWndGraph::SetGraph(TCHAR* szGraph, int iGraph)
{
	if(iGraph < 0 || 
		iGraph >= MAX_GRAPHS)
		return FALSE;

	m_oStr->Delete(&m_szGraphs[iGraph]);
	m_szGraphs[iGraph] = m_oStr->CreateAndCopy(szGraph);

    //quick check for brackets
    int iLeftCnt = 0;
    int iRightCnt = 0;
    int iIndex = 0;

    while(TRUE)
    {
        iIndex = m_oStr->Find(szGraph, _T("("), iIndex);

        if(iIndex != -1)
            iIndex++;
        else
            break;

        iLeftCnt++;
    }

    iIndex = 0;

    while(iIndex != -1)
    {
        iIndex = m_oStr->Find(szGraph, _T(")"), iIndex);

        if(iIndex != -1)
            iIndex++;
        else
            break;

        iRightCnt++;
    }

    if(iRightCnt < iLeftCnt)
    {
        TCHAR* szTemp = new TCHAR[m_oStr->GetLength(szGraph) + 1 + iLeftCnt - iRightCnt];
        m_oStr->StringCopy(szTemp, szGraph);
        for(int i = 0; i < iLeftCnt - iRightCnt; i++)
            m_oStr->Concatenate(szTemp, _T(")"));

        m_oStr->Delete(&m_szGraphs[iGraph]);
        m_szGraphs[iGraph] = szTemp;//just easy
        szTemp = NULL;
    }

	return TRUE;
}

void CWndGraph::SetGraphAccuracy(int i)
{
	if(i < 1)
		m_iAcuracy = 0;
	else if(i == 1)
		m_iAcuracy = 3;
	else
		m_iAcuracy = 6;
}


int CWndGraph::XToScreen(double dbX)
{
	return (int)((dbX+m_dbXTranslation)*m_iScreenWidth/m_dbXScale);//the cast should fix any range issues;
}

int CWndGraph::YToScreen(double dbY)
{
	//we start from the height and subtract to invert for Y...note we don't for X
//	int iRet = m_iScreenHeight - (int)((dbY+m_dbYTranslation)*(double)m_iScreenHeight/m_dbYScale);//the cast should fix any range issues
//	double dbret = 0.5 + (double)m_iScreenHeight - (dbY+m_dbYTranslation)*(double)m_iScreenHeight/m_dbYScale; 

	double dbret = 0.5  - (dbY+m_dbYTranslation)*(double)m_iScreenHeight/m_dbYScale; 

	int iCast = (int)dbret;

	return m_iScreenHeight + iCast;
}


//if we click on the graph it moves yo
BOOL CWndGraph::TranslateGraph(POINT pt)
{
#define PERCENT_OF_SCREEN 0.1
#define PERCENT_MOVEMENT  0.1

	BOOL bReturn = FALSE;
	//we'll define the regions as a percentage of the graph size....say 10% per edge
	//we'll check the left edge first
	if(pt.x < m_iScreenWidth*PERCENT_OF_SCREEN)
	{
		m_dbXTranslation += m_dbXScale*PERCENT_MOVEMENT;
		bReturn = TRUE;
	}
	if(pt.x > m_iScreenWidth*(1.0-PERCENT_OF_SCREEN))
	{
		m_dbXTranslation -= m_dbXScale*PERCENT_MOVEMENT;
		bReturn = TRUE;
	}
	if(pt.y < m_iScreenHeight*PERCENT_OF_SCREEN)
	{
		m_dbYTranslation -= m_dbYScale*PERCENT_MOVEMENT;
		bReturn = TRUE;
	}
	if(pt.y > m_iScreenHeight*(1.0 - PERCENT_OF_SCREEN))
	{
		m_dbYTranslation += m_dbYScale*PERCENT_MOVEMENT;
		bReturn = TRUE;
	}

	return bReturn;

}

void CWndGraph::DrawGraphs(HDC dc)
{
	if(m_bDrawLines)//Draw Lines
	{
		m_hOldPen = (HPEN)SelectObject(dc, m_hPen[0]);

		for(int j = 0; j < MAX_GRAPHS; j++)
		{
			if(m_iYGraph[j] == NULL)
				continue;
			MoveToEx(dc, 0, m_iYGraph[j][0], NULL);
			SelectObject(dc, m_hPen[j]);
			for(int i = 1; i < m_iScreenWidth/(1+m_iAcuracy); i++)
			{
		//		if((double)m_iYGraph[j][i] - (double)m_iYGraph[j][i+1] > m_iScreenHeight*2.0)
		//			MoveToEx(dc, i, m_iYGraph[j][i], NULL);
		//		else if((double)m_iYGraph[j][i] - (double)m_iYGraph[j][i+1] < m_iScreenHeight*-2.0)
		//			MoveToEx(dc, i, m_iYGraph[j][i], NULL);
		//		else
				LineTo(dc, i*(1+m_iAcuracy), m_iYGraph[j][i]);	
			}
		}

		SelectObject(dc, m_hOldPen);
	}
	else//Draw Lines with dynamic
	{
		//Create the Point array if it hasn't already been created AND the graph equation is defined
		for(int i = 0; i < m_iScreenWidth/(1+m_iAcuracy); i++)
		{
            for(int g = 0; g < MAX_GRAPHS; g++)
            {
                if(m_iYGraph[g] != NULL)
                    SetPixel(dc, i*(1+m_iAcuracy), m_iYGraph[g][i], GetGraphColor(g));
            }
		}
	}
}


void CWndGraph::DrawAxis(HDC dc)
{
	//ok...translations are exact...not relative so
	//we need to convert the translations and see if the axis are on screen
	//and of course shift in the opposite direction

	HPEN penLine = CreatePen(PS_SOLID, 1, m_crText);
	m_hOldPen = (HPEN)SelectObject(dc,  penLine);
	//m_hOldPen = (HPEN)SelectObject(dc, (HPEN)GetStockObject(WHITE_PEN));


	int	m_iX = (int)(m_dbXTranslation*m_iScreenWidth/m_dbXScale);
	int m_iY = 1+(int)(m_iScreenHeight - m_dbYTranslation*(double)m_iScreenHeight/m_dbYScale);//try and figure out why the correction is needed

	if(m_iX > -1 && m_iX < m_iScreenWidth)
	{
		MoveToEx(dc, m_iX, 0, NULL);
		LineTo(dc, m_iX, m_iScreenHeight);
	}

	if(m_iY > -1 && m_iY < m_iScreenHeight)
	{
		MoveToEx(dc, 0, m_iY, NULL);
		LineTo(dc, m_iScreenWidth, m_iY);
	}

	SelectObject(dc, m_hOldPen);
	DeleteObject(penLine);
}

void CWndGraph::DrawGrid(HDC dc)
{
#define NUM_X_DIVIDERS 9


	RECT rc;
	HBRUSH brBackground;
	brBackground = CreateSolidBrush(m_crBackGround);
	GetClientRect(m_hWnd, &rc);
	FillRect(dc, &rc, brBackground);
	DeleteObject(brBackground);

	if(m_bDrawGrid == FALSE)
		return;

	m_hOldPen	= (HPEN)SelectObject(dc, m_hPenGrid);

	//we'll just draw 4 lines per....making 5 sections per side...10 total
	//relative to the center
	int iXSpacing	= m_iScreenWidth/(NUM_X_DIVIDERS+1);	
	int iYSpacing	= iXSpacing; //m_iScreenHeight/(NUM_DIVIDERS+1);

	//Now translation & scale? correction
	int iXAdjust	= (int)(m_dbXTranslation*m_iScreenWidth/m_dbXScale);  //translation is converted to a fraction of the screen
	int iYAdjust	= (int)(m_dbYTranslation*m_iScreenHeight/m_dbYScale);  //translation is converted to a fraction of the screen

	//now we need it in range
	for(iXAdjust; iXAdjust > -1*iXSpacing; iXAdjust -= iXSpacing)
	{}
	for(iXAdjust; iXAdjust < -1*iXSpacing; iXAdjust += iXSpacing)
	{}

	//now we need it in range
	for(iYAdjust; iYAdjust > 0; iYAdjust -= iYSpacing)
	{}
	for(iYAdjust; iYAdjust < 0; iYAdjust += iYSpacing)
	{}

	iYAdjust = iYSpacing - iYAdjust;

	int iX = 0;
	int iY = 0;
	int i;

	//And lets draw	
//	for(int i =0; i < NUM_X_DIVIDERS+1; i++)
	i = 0;
	while(iY < m_iScreenHeight)
	{	//Y
		iY = (iYSpacing*(1+i)-iYAdjust);
		MoveToEx(dc, 0, m_iScreenHeight - iY, NULL);
		LineTo(dc, m_iScreenWidth, m_iScreenHeight - iY);
		i++;
	}

//	for(int i =0; i < NUM_X_DIVIDERS+1; i++)
	i = 0;
	while(iX < m_iScreenWidth)
	{	//X
		iX = iXAdjust+iXSpacing*(1+i);
		MoveToEx(dc, iX, 0, NULL);
		LineTo(dc, iX, m_iScreenHeight);
		i++;
	}
	SelectObject(dc, m_hOldPen);
}


//draws the bounding box for zoom mode
void CWndGraph::DrawSelect(HDC dc)
{
	if(!m_bLButtonDown)
		return;

	if(m_eGraphMode != MODE_Zoom)
		return;

	m_hOldPen = (HPEN)SelectObject(dc, m_hPenSelect);

	MoveToEx(dc, m_ptStartSelect.x, m_ptStartSelect.y, NULL);
	LineTo(dc,m_ptStartSelect.x, m_ptEndSelect.y);
	LineTo(dc,m_ptEndSelect.x, m_ptEndSelect.y);
	LineTo(dc,m_ptEndSelect.x, m_ptStartSelect.y);
	LineTo(dc,m_ptStartSelect.x, m_ptStartSelect.y);

	SelectObject(dc, m_hOldPen);
}

void CWndGraph::DrawScale(HDC dc)
{
	//we'll just center each one for now
	int iXCenter = m_iScreenWidth/2;
	int iYCenter = m_iScreenHeight/2;
	static TCHAR szTemp[STRING_MAX];

	//Top
	FormatScaleText(szTemp, m_dbYScale - m_dbYTranslation);
	DrawText(dc, szTemp, iXCenter + INDENT_TEXT, INDENT_TEXT/2, DT_LEFT, m_hFontText, GRAPH_TEXT_COLOR);

	//Bottom
	FormatScaleText(szTemp, -1*m_dbYTranslation);
	DrawText(dc, szTemp, iXCenter + INDENT_TEXT, m_iScreenHeight - GetSystemMetrics(SM_CXICON)/2, DT_LEFT, m_hFontText, GRAPH_TEXT_COLOR);

	//Left
	FormatScaleText(szTemp, -1*m_dbXTranslation);
	DrawText(dc, szTemp, INDENT_TEXT, iYCenter + INDENT_TEXT/2, DT_LEFT, m_hFontText, GRAPH_TEXT_COLOR);

	//Right
	FormatScaleText(szTemp, m_dbXScale - m_dbXTranslation);
	DrawText(dc, szTemp, m_iScreenWidth - INDENT_TEXT, iYCenter + INDENT_TEXT/2, DT_RIGHT, m_hFontText, GRAPH_TEXT_COLOR);
}

BOOL CWndGraph::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_oGDI.Destroy();
	m_oGDIBuffer.Destroy();

    //set up the rects for zoom in and zoom out 
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    static int iIndent = GetSystemMetrics(SM_CXSMICON)/2; //don't need much
    static int iSize = GetSystemMetrics(SM_CXSMICON)*2;

    //zoom out at the bottom
    m_rcZoomOut.bottom = rcClient.bottom - iIndent - iSize/2;
    m_rcZoomOut.right = rcClient.right - iIndent;
    m_rcZoomOut.left = m_rcZoomOut.right - iSize;
    m_rcZoomOut.top = m_rcZoomOut.bottom - iSize;

    m_rcZoomIn = m_rcZoomOut;

    if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))
    {
        m_rcZoomIn.bottom = m_rcZoomOut.top - iIndent;
        m_rcZoomIn.top = m_rcZoomIn.bottom - iSize;
    }
    else
    {
        m_rcZoomIn.right = m_rcZoomOut.left - iIndent;
        m_rcZoomIn.left = m_rcZoomIn.right - iSize;
    }

    //load in the button if it hasn't been done already ... I'm not going to
    //resize it anyways 
#ifdef IDR_PNG_Button
    if(m_imgButton.IsLoaded() == FALSE)
    {
        m_imgButton.Initialize(hWnd, m_hInst, IDR_PNG_Button);
        m_imgButton.SetSize(WIDTH(m_rcZoomIn), HEIGHT(m_rcZoomIn));
    }
#endif

	m_iScreenWidth = rcClient.right - rcClient.left;
	m_iScreenHeight = rcClient.bottom - rcClient.top;

	for(int i = 0; i < MAX_GRAPHS; i++)
	{
		if(m_iYGraph[i] != NULL)
		{
			delete [] m_iYGraph[i];
			m_iYGraph[i] = NULL;
		}
	}

	ResetValues();

	return TRUE;
}

BOOL CWndGraph::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rc;
	GetClipBox(hDC, &rc);

	if(m_oGDI.GetDC() == NULL)
	{
		GetClientRect(m_hWnd, &rc);
		m_oGDI.Create(hDC, rc, FALSE, TRUE);
	}


	if(m_bLButtonDown && m_eGraphMode == MODE_Zoom)	//when you draw the selecting box
	{												//from the mem DC, keep the draw speed up
		//Copy from buffer
		BitBlt(m_oGDI.GetDC(),
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			m_oGDIBuffer.GetDC(),
			rc.left,
			rc.top,
			SRCCOPY);
		DrawSelect(m_oGDI.GetDC());
	}
	else if(m_bLButtonDown && m_eGraphMode == MODE_GetLocation)
	{
		//Copy from buffer
		BitBlt(m_oGDI.GetDC(),
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			m_oGDIBuffer.GetDC(),
			rc.left,
			rc.top,
			SRCCOPY);
		DrawPointLocation(m_oGDI.GetDC());
	}

	else//full redraw
	{
		DrawGrid(m_oGDI.GetDC());
		DrawAxis(m_oGDI.GetDC());
		if(!m_bLButtonDown )//this is to save speed on translation
		{
			DrawGraphs(m_oGDI.GetDC());
			DrawSpecPoints(m_oGDI.GetDC());//ie points in linear regression
		}

		DrawScale(m_oGDI.GetDC());

		if(m_oGDIBuffer.GetDC() == NULL)
			m_oGDIBuffer.Create(m_oGDI.GetDC(), rcClient, TRUE);

        DrawZoomButtons(&m_oGDI, rc);

		//Copy to Buffer
		BitBlt(m_oGDIBuffer.GetDC(),
			rcClient.left,
			rcClient.top,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			m_oGDI.GetDC(),
			rcClient.left,
			rcClient.top,
			SRCCOPY);
	}


    DrawOtherText(m_oGDI.GetDC());
	

	///// COPY memDC to SCREEN //////////////////////////////
	BitBlt(hDC,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		m_oGDI.GetDC(),
		rc.left,
		rc.top,
		SRCCOPY);

	return TRUE;
}

void CWndGraph::DrawZoomButtons(CIssGDIEx* gdi, RECT rcClip)
{
    if(m_imgButton.IsLoaded())
        m_imgButton.DrawImage(*gdi, m_rcZoomIn.left, m_rcZoomIn.top);
    else
        ::FrameRect(gdi->GetDC(), m_rcZoomIn, 0xEEEEEE);
    ::DrawText(gdi->GetDC(), _T("+"), m_rcZoomIn, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

    if(m_imgButton.IsLoaded())
        m_imgButton.DrawImage(*gdi, m_rcZoomOut.left, m_rcZoomOut.top);
    else    
        ::FrameRect(gdi->GetDC(), m_rcZoomOut, 0xEEEEEE);
    ::DrawText(gdi->GetDC(), _T("-"), m_rcZoomOut, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);
}

void CWndGraph::DrawOtherText(HDC dc)
{
    //Draw The Graph Mode 
    static TCHAR szTemp[STRING_MAX];

    switch(m_eGraphMode)
    {
    case MODE_Zoom:
        //		m_oStr->StringCopy(szTemp, IDS_GMENU_Zoom, STR_NORMAL, m_hInst);
        m_oStr->StringCopy(szTemp, _T("Zoom"));
        break;
    case MODE_Translate:
        //		m_oStr->StringCopy(szTemp, IDS_GMENU_Translate, STR_NORMAL, m_hInst);
        m_oStr->StringCopy(szTemp, _T("Translate"));
        break;
    case MODE_GetLocation:
        //		m_oStr->StringCopy(szTemp, IDS_GMENU_PointOnGraph, STR_NORMAL, m_hInst);
        m_oStr->StringCopy(szTemp, _T("Point on graph"));
        break;
    default:
        break;
    }


    DrawText(m_oGDI.GetDC(), szTemp, INDENT_TEXT, m_iScreenHeight - GetSystemMetrics(SM_CXICON)/2, DT_LEFT, m_hFontText, GRAPH_TEXT_COLOR);

    //graph 
    static COLORREF crGraph;

    crGraph = GetGraphColor(m_iGraphSelected);

    RECT rcTemp = {INDENT_TEXT/2, INDENT_TEXT/2, GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CXICON)};

    //we're going to fit in the graph equation here too 
    m_oStr->Format(szTemp, _T("f(x)=%s"), m_szGraphs[m_iGraphSelected]);
    DrawText(dc, szTemp, rcTemp, DT_TOP | DT_LEFT | DT_END_ELLIPSIS, m_hFontText, crGraph);
}


BOOL CWndGraph::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
//void CDlgGraph::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	POINT ptHack;
	switch(wParam) 
	{
    case _T('0'):
        FitToData();
        break;
    case _T('5'):
        CenterGraph();
        break;
    case _T('4'):
	case VK_LEFT:
		ptHack.x = 1;
		ptHack.y = m_iScreenHeight/2;
		TranslateGraph(ptHack);
		break;
    case _T('6'):
	case VK_RIGHT:
		ptHack.x = m_iScreenWidth - 1;
		ptHack.y = m_iScreenHeight/2;
		TranslateGraph(ptHack);
		break;
    case _T('2'):
	case VK_DOWN:
		ptHack.x = m_iScreenWidth/2;
		ptHack.y = m_iScreenHeight - 1;
		TranslateGraph(ptHack);
		break;
    case _T('8'):
	case VK_UP:
		ptHack.x = m_iScreenWidth/2;
		ptHack.y = 1;
		TranslateGraph(ptHack);
		break;
    case 120:
	case 107 /*'+'*/:
		ZoomIn();
		return TRUE;
		break;
    case 119:
	case 109 /*'-'*/:
		ZoomOut();
		return TRUE;
		break;
    case VK_ESCAPE:
        SafeCloseWindow(IDOK);
        break;
	default:
		break;
	}
	CalcGraphPoints();
	InvalidateRect(m_hWnd, NULL, FALSE);

	return FALSE;
}

//we gotta set the Scale AND the translation
void CWndGraph::ZoomIn()
{
#define ZOOM_IN_FACTOR 2.0
#define MIN_SCALE	   0.00001

	if(m_dbXScale/ZOOM_IN_FACTOR < MIN_SCALE && m_eZoomMode != ZOOM_Y)
		return;
	if(m_dbYScale/ZOOM_IN_FACTOR < MIN_SCALE && m_eZoomMode != ZOOM_X)
		return;

	if(m_eZoomMode != ZOOM_Y)
    {
        m_dbXScale /= ZOOM_IN_FACTOR;
        //Old Trans        New Trans		  Old Scale                        New Scale
        m_dbXTranslation = m_dbXTranslation - m_dbXScale*ZOOM_IN_FACTOR/2.0 + m_dbXScale/2.0;
    }
    if(m_eZoomMode != ZOOM_X)
    {
	    m_dbYScale /= ZOOM_IN_FACTOR;
        //Old Trans        New Trans		  Old Scale                        New Scale
        m_dbYTranslation = m_dbYTranslation - m_dbYScale*ZOOM_IN_FACTOR/2.0 + m_dbYScale/2.0;
    }

	CalcGraphPoints();
	InvalidateRect(m_hWnd, NULL, FALSE);
}

void CWndGraph::ZoomOut()
{
#define ZOOM_OUT_FACTOR 2
#define MAX_SCALE	1000000

	if(m_dbXScale*ZOOM_IN_FACTOR > MAX_SCALE && m_eZoomMode != ZOOM_Y)
		return;
	if(m_dbYScale*ZOOM_IN_FACTOR > MAX_SCALE && m_eZoomMode != ZOOM_X)
		return;

    if(m_eZoomMode != ZOOM_Y)
    {   //Old Trans        New Trans		  Old Scale                        New Scale
        m_dbXScale *= ZOOM_OUT_FACTOR;
        m_dbXTranslation = m_dbXTranslation - m_dbXScale/ZOOM_OUT_FACTOR/2.0 + m_dbXScale/2.0;
    }

    if(m_eZoomMode != ZOOM_X)
    {
        m_dbYScale *= ZOOM_OUT_FACTOR;
        m_dbYTranslation = m_dbYTranslation - m_dbYScale/ZOOM_OUT_FACTOR/2.0 + m_dbYScale/2.0;
    }

	CalcGraphPoints();
	InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CWndGraph::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	m_hPenGrid		= CreatePen(PS_SOLID, 1, m_crLineColor);	//  RGB(50,50,50));
	m_hPenSelect	= CreatePen(PS_DASH,  1, m_crAxisColor);	//  RGB(255,255,255));
	m_hOldPen		= NULL;

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Graph;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
	{
		int i = 0;
	}

	m_hMenuBar =(HMENU)::SendMessage(mbi.hwndMB,SHCMBM_GETMENU,(WPARAM)0,(LPARAM)0);
	m_hWndMenu = mbi.hwndMB;

/*	TBBUTTON sButtons[3];
	memset(&sButtons, 0, sizeof(TBBUTTON)*3);

	sButtons[0].iBitmap		= 0;
	sButtons[0].idCommand	= ID_RESET;
	sButtons[0].fsState		= TBSTATE_ENABLED;
	sButtons[0].fsStyle		= TBSTYLE_BUTTON;
	sButtons[0].dwData		= 0;
	sButtons[0].iString		= 0;

	sButtons[1].iBitmap		= 1;
	sButtons[1].idCommand	= ID_ZOOMIN;
	sButtons[1].fsState		= TBSTATE_ENABLED;
	sButtons[1].fsStyle		= TBSTYLE_BUTTON;
	sButtons[1].dwData		= 0;
	sButtons[1].iString		= 0;

	sButtons[2].iBitmap		= 2;
	sButtons[2].idCommand	= ID_ZOOMOUT;
	sButtons[2].fsState		= TBSTATE_ENABLED;
	sButtons[2].fsStyle		= TBSTYLE_BUTTON;
	sButtons[2].dwData		= 0;
	sButtons[2].iString		= 0;

static LPTSTR szToolTips[] = {
		TEXT(""),	// Menu
		TEXT(""),	// Menu
		TEXT("Reset"),
		TEXT("Zoom In"),
		TEXT("Zoom Out"),
	};*/
#endif

	

	RECT rc;
	GetClientRect(hWnd, &rc);
	m_iScreenWidth = rc.right - rc.left;
	m_iScreenHeight = rc.bottom - rc.top;

	ResetValues();
	//CalcGraphPoints();

    //and finally ... set the currently selected graph
    for(int i = 0; i < MAX_GRAPHS; i++)
    {
        if(m_oStr->IsEmpty(m_szGraphs[i]) == FALSE)
        {
            m_iGraphSelected = i;
            break;
        }
    }

	return TRUE;
}


void CWndGraph::DrawSpecPoints(HDC dc)
{
#define BOX_SPACING (4*GetSystemMetrics(SM_CXSMICON)/16) //4 pixels scaled
    HPEN hPenNew = CreatePen(0, 1, GetGraphColor(m_iGraphSelected));
	m_hOldPen = (HPEN)SelectObject(dc, hPenNew);	

	int iXtemp;
	int iYtemp;

	for(int i = 0; i < m_iNumPoints; i++)
	{
		iXtemp = XToScreen(m_dbSpecX[i]);
		iYtemp = YToScreen(m_dbSpecY[i]);

		//	SetPixel(dc,iXtemp ,iYtemp , RGB(255,255,255));
		MoveToEx(dc, iXtemp - BOX_SPACING+1, iYtemp, NULL);
		LineTo(dc, iXtemp + BOX_SPACING, iYtemp);
		MoveToEx(dc, iXtemp, iYtemp - BOX_SPACING+1, NULL);
		LineTo(dc, iXtemp, iYtemp + BOX_SPACING);
	}
	SelectObject(dc, m_hOldPen);
    DeleteObject(hPenNew);
}

COLORREF CWndGraph::GetGraphColor(int i)
{
    COLORREF rgb;
    switch(i) 
    {
    case 0:
        rgb = GRAPH_COLOR_1;
        break;
    case 1:
        rgb = GRAPH_COLOR_2;
        break;
    case 2:
        rgb = GRAPH_COLOR_3;
        break;
    case 3:
        rgb = GRAPH_COLOR_4;
        break;
    case 4: 
        rgb = GRAPH_COLOR_5;
        break;
    case 5:
        rgb = GRAPH_COLOR_6;
        break;
    case 6:
        rgb = GRAPH_COLOR_7;
        break;
    case 7:
    default:
        rgb = GRAPH_COLOR_8;
        break;
    }

    return rgb;
}

void CWndGraph::DrawPointLocation(HDC dc)
{
	if(m_iGraphSelected < 0 ||
		m_iGraphSelected > MAX_GRAPHS)
		return;

    COLORREF rgb = GetGraphColor(m_iGraphSelected);

    int iX = m_ptEndSelect.x;

	double dbX = iX/(double)m_iScreenWidth*m_dbXScale -m_dbXTranslation;

	TCHAR szDouble[STR_NORMAL];

	m_oStr->DoubleToString(szDouble, dbX);

	TCHAR szTemp[STR_MAX];
	m_oStr->StringCopy(szTemp, m_szGraphs[m_iGraphSelected]);

	m_oSciUtil->FindAndReplaceString(szTemp, _T("X"), szDouble, szTemp);

	//get the answer 
	double dbY = m_oCalc->Calculate(szTemp, szTemp);

	//now we have 2 doubles and 2 TCHARs...cool eh?..one 1 int though
	int iY = (int)(m_iScreenHeight - (dbY+m_dbYTranslation)*(double)m_iScreenHeight/m_dbYScale);

	//there we go....
	if((iX > 0 && iX < m_iScreenWidth) &&
		iY > 0 && iY < m_iScreenHeight)
	{
		HPEN hPen	= CreatePen(PS_SOLID, 1, rgb);
		m_hOldPen = (HPEN)SelectObject(dc, hPen);

		MoveToEx(dc, 0, iY, NULL);
		LineTo(dc, m_iScreenWidth, iY);
		MoveToEx(dc, iX, 0, NULL);
		LineTo(dc, iX, m_iScreenHeight);

		SelectObject(dc, m_hOldPen);
		DeleteObject((HPEN)hPen);
	}

	//oh hell...lets draw some co-ordinates too
    if(iX > m_iScreenWidth/2)
    {
        DrawText(dc, szDouble, iX-5, iY-GetSystemMetrics(SM_CXICON)/2, DT_RIGHT, m_hFontText, rgb);
        DrawText(dc, szTemp,	iX-5, iY, DT_RIGHT, m_hFontText, rgb);
    }
    else
    { 
	    DrawText(dc, szDouble, 5+iX, iY-GetSystemMetrics(SM_CXICON)/2, DT_LEFT, m_hFontText, rgb);
	    DrawText(dc, szTemp,	5+iX, iY, DT_LEFT, m_hFontText, rgb);
    }
}

BOOL CWndGraph::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDMENU_SaveGraph:
        OnSaveGraph();
        break;
	case IDMENU_Graph:
		OnMenuGraph();
		break;
	case IDMENU_Mode:
		OnMenuMode();
		break;
	case IDMENU_Exit:
	case IDOK:	
		SafeCloseWindow(IDOK);
		return TRUE;
		break;
	case IDMENU_ZoomIn:
		ZoomIn();
		break;
	case IDMENU_ZoomOut:
		ZoomOut();
		break;
    case IDMENU_ZoomNormal:
        m_eZoomMode = ZOOM_Normal;
        break;
    case IDMENU_ZoomX:
        m_eZoomMode = ZOOM_X;
        break;
    case IDMENU_ZoomY:
        m_eZoomMode = ZOOM_Y;
        break;
	case IDMENU_Reset:
		ResetValues();
		break;
	case IDMENU_Translate:
		m_eGraphMode = MODE_Translate;
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_Zoom:
		m_eGraphMode = MODE_Zoom;
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_PointOnGraph:
		m_eGraphMode = MODE_GetLocation;
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_CenterGraph:
		CenterGraph();
		break;
    case IDMENU_FitToData:
        m_bLButtonDown = FALSE;
        FitToData();
        CalcGraphPoints();
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case IDMENU_SelGraph1:
    case IDMENU_SelGraph2:
    case IDMENU_SelGraph3:
    case IDMENU_SelGraph4:
    case IDMENU_SelGraph5:
    case IDMENU_SelGraph6:
    case IDMENU_SelGraph7:
    case IDMENU_SelGraph8:
        m_iGraphSelected = LOWORD(wParam)-IDMENU_SelGraph1;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
	default:
		break;
	}
	return FALSE;

}

void CWndGraph::FormatScaleText(TCHAR* szNum, double dbNum)
{
#define SIG_FIGS 5
	m_oStr->DoubleToString(szNum, dbNum);

	int iLen	= m_oStr->GetLength(szNum);
	if(m_oStr->Find(szNum, _T("e"))!= -1 )
	{

		int iE		= m_oStr->Find(szNum, _T("e"));
		if(iLen > SIG_FIGS)
		{
			for( int i = SIG_FIGS + 5; i < iLen; i++)
			{
				m_oStr->Delete(iE - 1, 1, szNum);
				iE--;
			}
			if(szNum[iE+1]==_T('-'))
				iE++;
			if(szNum[iE+1]==_T('0'))
			{
				m_oStr->Delete(iE+1, 1, szNum);
				if(szNum[iE+1]==_T('0'))
					m_oStr->Delete(iE+1, 1, szNum);
			}
		}
	}
	else if(szNum[1]==_T('.')) //Stupid Normal Numbers....

	{
		if(iLen > SIG_FIGS+1)
			m_oStr->Delete(SIG_FIGS + 1, iLen - SIG_FIGS - 1, szNum);

	}
	else if(szNum[2]==_T('.') &&
		szNum[0]==_T('-')) //Stupid Normal Numbers....

	{
		if(iLen > SIG_FIGS+1)
			m_oStr->Delete(SIG_FIGS + 2, iLen - SIG_FIGS - 2, szNum);
	}
}


BOOL CWndGraph::OnLButtonUp(HWND hWnd, POINT& pt)
{
	m_bLButtonDown= FALSE;

    if(PtInRect(&m_rcZoomIn, pt))
    {
        ZoomIn();
        return TRUE;
    }
    if(PtInRect(&m_rcZoomOut, pt))
    {
        ZoomOut();
        return TRUE;
    }

	if(TranslateGraph(pt))//if we clicked on the edge of the graph
	{
	}
	else if(m_bMouseMoved && m_eGraphMode == MODE_Zoom)
	{
		if(abs(m_ptStartSelect.x - m_ptEndSelect.x) > GetSystemMetrics(SM_CXSMICON) &&
			abs(m_ptStartSelect.y - m_ptEndSelect.y) > GetSystemMetrics(SM_CXSMICON))//avoids accidental small boxes
		{
			double dbTemp1 = (double)((double)m_ptStartSelect.x - (double)m_ptEndSelect.x)/(double)m_iScreenWidth;
			double dbTemp2 = (double)((double)m_ptStartSelect.y - (double)m_ptEndSelect.y)/(double)m_iScreenHeight;

			if(dbTemp1 < 0)dbTemp1*=-1;
			if(dbTemp2 < 0)dbTemp2*=-1; //these are the % of the screen selected


			//think of it as a viewing window
			//translate based off center of new viewing window
			double dbXStart = m_ptStartSelect.x;
			if(m_ptStartSelect.x > m_ptEndSelect.x)
				dbXStart = m_ptEndSelect.x;

			double dbYStart = m_ptStartSelect.y;
			if(m_ptStartSelect.y < m_ptEndSelect.y)
				dbYStart = m_ptEndSelect.y;

			//basically...to re-translate....we need to remove the amount of translation we're cutting off when selecting
			//so ... the distance from the left end to the selection edge and the distance from the bottom the the selector
			m_dbXTranslation -= dbXStart/(double)m_iScreenWidth*m_dbXScale;
			m_dbYTranslation -= ((double)m_iScreenHeight - dbYStart)/(double)m_iScreenWidth*m_dbYScale;


			//time to rescale...old is right
			m_dbXScale *= dbTemp1; //multiply by the % to see how much the scale reduces by...good and easy
			m_dbYScale *= dbTemp2; 
		}
	}
	else if(m_bMouseMoved== FALSE && m_eGraphMode != MODE_GetLocation)
	{
		//Lets Center to the point
		m_dbXTranslation = -1*(double)pt.x/(double)m_iScreenWidth* m_dbXScale              + m_dbXTranslation  + 0.5*m_dbXScale;
		m_dbYTranslation = (double)pt.y/(double)m_iScreenHeight*m_dbYScale - m_dbYScale + m_dbYTranslation + 0.5*m_dbYScale;
	}

	CalcGraphPoints();

	InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}


BOOL CWndGraph::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcZoomIn, pt))
        return TRUE;
    if(PtInRect(&m_rcZoomOut, pt))
        return TRUE;

	m_bMouseMoved = FALSE;
	m_bLButtonDown= TRUE;
	m_ptStartSelect.x = pt.x;
	m_ptStartSelect.y = pt.y;

	return TRUE;
}

BOOL CWndGraph::OnMouseMove(HWND hWnd, POINT& pt)
{
	m_bMouseMoved = TRUE;
	m_ptEndSelect.x = pt.x;
	m_ptEndSelect.y = pt.y;

	if(m_eGraphMode == MODE_Translate && m_bLButtonDown)
	{
		//add some translate code here....finally
		int iX = m_ptEndSelect.x - m_ptStartSelect.x;
		int iY = m_ptEndSelect.y - m_ptStartSelect.y;

		double dbXMove = (double)iX/(double)m_iScreenWidth*m_dbXScale; 
		double dbYMove = (double)iY/(double)m_iScreenHeight*m_dbYScale;

		m_dbXTranslation += dbXMove;
		m_dbYTranslation -= dbYMove;

		m_ptStartSelect.x = pt.x;
		m_ptStartSelect.y = pt.y;

	}
	InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}

BOOL CWndGraph::AddPoint(double dbX, double dbY)
{
	if(m_iNumPoints >= MAX_POINTS)
		return FALSE;

	m_dbSpecX[m_iNumPoints] = dbX;
	m_dbSpecY[m_iNumPoints] = dbY;

	m_iNumPoints++;

	return TRUE;
}

void CWndGraph::OnMenuGraph()
{
	HMENU hMenu = CreatePopupMenu();
    HMENU hMenuGraphs = CreatePopupMenu();
	if(!hMenu)
		return;

	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, IDS_MENU_ZoomIn, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_ZoomIn, szText);

	m_oStr->StringCopy(szText, IDS_MENU_ZoomOut, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_ZoomOut, szText);

	m_oStr->StringCopy(szText, IDS_MENU_Reset, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Reset, szText);

	m_oStr->StringCopy(szText, IDS_MENU_CenterGraph, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_CenterGraph, szText);

    if(m_iNumPoints > 2)
    {
        m_oStr->StringCopy(szText, _T("Fit to data"));
        AppendMenu(hMenu, MF_STRING, IDMENU_FitToData, szText);
    }

    //if we have more than one graph we need to allow selecting which one we're focusing on
    int iCount = 0;

    for(int i = 0; i < MAX_GRAPHS; i++)
    {
        if(m_oStr->IsEmpty(m_szGraphs[i]))
            continue;

        iCount++;

        //add this to the graph menu
        AppendMenu(hMenuGraphs, MF_STRING, IDMENU_SelGraph1+i, m_szGraphs[i]);
    }

    if(iCount > 1)
    {
        m_oStr->StringCopy(szText, _T("Select Graph"));
        //add the menu
        AppendMenu(hMenu, MF_POPUP, UINT_PTR(hMenuGraphs), szText);
    }

    //end graph selection

    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

    m_oStr->StringCopy(szText, IDS_MENU_SaveGraph, STRING_MAX, m_hInst);
    AppendMenu(hMenu, MF_STRING, IDMENU_SaveGraph, szText);

    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	m_oStr->StringCopy(szText, IDS_MENU_Exit, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Exit, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
#ifdef UNDER_CE
	pt.x	= rc.left + GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;
	
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
#else
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	pt.x	= rcClient.left;
	pt.y	= rcClient.top;

	ClientToScreen(m_hWnd, &pt);

	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);


#endif

	DestroyMenu(hMenu);
    DestroyMenu(hMenuGraphs);

}

void CWndGraph::OnMenuMode()
{
	HMENU hMenu = CreatePopupMenu();
    HMENU hMenuZoom = CreatePopupMenu();
	if(!hMenu || !hMenuZoom)
		goto Error;

	TCHAR szText[STRING_MAX];
    int iCheck;

#ifndef WIN32_PLATFORM_WFSP
	m_oStr->StringCopy(szText, IDS_MENU_Zoom, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Zoom, szText);

	m_oStr->StringCopy(szText, IDS_MENU_Translate, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Translate, szText);

	m_oStr->StringCopy(szText, IDS_MENU_PointOnGraph, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_PointOnGraph, szText);

	iCheck = (int)m_eGraphMode;
	CheckMenuRadioItem(hMenu, 0, 2, iCheck, MF_BYPOSITION);

    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

#endif

    //build up the zoom menu
    m_oStr->StringCopy(szText, _T("Normal (XY)"));
    AppendMenu(hMenuZoom, MF_STRING, IDMENU_ZoomNormal, szText);
    m_oStr->StringCopy(szText, _T("X Only"));
    AppendMenu(hMenuZoom, MF_STRING, IDMENU_ZoomX, szText);
    m_oStr->StringCopy(szText, _T("Y Only"));
    AppendMenu(hMenuZoom, MF_STRING, IDMENU_ZoomY, szText);

    iCheck = (int)m_eZoomMode;
    CheckMenuRadioItem(hMenuZoom, 0, 2, iCheck, MF_BYPOSITION);
    //end zoom

    m_oStr->StringCopy(szText, _T("Zoom Mode"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuZoom, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
#ifdef UNDER_CE
	pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);

#else
RECT rcClient;
GetClientRect(m_hWnd, &rcClient);

pt.x	= rcClient.left;
pt.y	= rcClient.top;

ClientToScreen(m_hWnd, &pt);

TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);


#endif

Error:
    if(hMenu)
	    DestroyMenu(hMenu);
    if(hMenuZoom)
        DestroyMenu(hMenuZoom);
}

void CWndGraph::CenterGraph()
{
#define NUMBER_OF_STEPS 15

	//find y location at center of screen
	double dbX = m_dbXScale/2 - m_dbXTranslation;

	TCHAR szEq[STR_MAX];	//I really don't wanna leak
	TCHAR szDoubleTemp[STR_NORMAL];			//convert our doubles to string for the ol calc engine

	//now screen points are relative to say...the SCREEN..so we don't need an x
	//don't forget the damn Y is upside down...we'll do that later
	//lets build our string to parse
	m_oStr->DoubleToString(szDoubleTemp, dbX);
	m_oStr->Insert(szDoubleTemp, _T("("), 0);
	m_oStr->Concatenate(szDoubleTemp, _T(")"));
	
    if(m_szGraphs[m_iGraphSelected] == NULL)
        return;
    
    m_oSciUtil->FindAndReplaceString(m_szGraphs[m_iGraphSelected], _T("X"), szDoubleTemp, szEq);

	double dbAns = m_oCalc->Calculate(szEq, szEq);

	//gotta make sure dbAns isn't an error 
	if(szEq[0] == _T('i') || szEq[0] == _T('s'))
		return;

	double dbTemp = m_dbYTranslation;
	double dbDelta = dbAns + m_dbYTranslation - m_dbYScale/2;

	if(dbDelta == 0)
	{	//we're already centered
		return;
	}

		HDC dc = GetDC(m_hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	//log translation?
	for(int i = 0; i < NUMBER_OF_STEPS; i++)
	{
		m_dbYTranslation = log10((i+1)*10/(double)NUMBER_OF_STEPS)*dbDelta*-1.0 + dbTemp;

		DrawGrid(m_oGDI.GetDC());
		DrawAxis(m_oGDI.GetDC());
		DrawScale(m_oGDI.GetDC());

		BitBlt(dc, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient),
			m_oGDI.GetDC(), rcClient.left, rcClient.top, SRCCOPY);

		Sleep(33);//make it look good
	}

	//lets just set the final location
	m_dbYTranslation = m_dbYScale/2 - dbAns;

	ReleaseDC(m_hWnd, dc);

	CalcGraphPoints();
	InvalidateRect(m_hWnd, NULL, FALSE);

}

void CWndGraph::FitToData()
{
    if(m_iNumPoints < 2)
        return;

    double dbXMin = m_dbSpecX[0];
    double dbXMax = m_dbSpecX[0];
    double dbYMin = m_dbSpecY[0];
    double dbYMax = m_dbSpecY[0];

    for(int i = 1; i < m_iNumPoints; i++)
    {
        dbXMin = min(dbXMin, m_dbSpecX[i]);
        dbXMax = max(dbXMax, m_dbSpecX[i]);
        dbYMin = min(dbYMin, m_dbSpecY[i]);
        dbYMax = max(dbYMax, m_dbSpecY[i]);
    }

    if(dbXMin == dbXMax ||
        dbYMin == dbYMax)
        return;

    double dbXPad = (dbXMax - dbXMin)/10;
    double dbYPad = (dbYMax - dbYMin)/10;

    //set scale and translation
    m_dbXTranslation = 0 - dbXMin + dbXPad;
    m_dbYTranslation = 0 - dbYMin + dbYPad;//gross but just the way it is

    //uhhh ... 
    m_dbXScale       = 12*dbXPad;
    m_dbYScale       = 12*dbYPad;

    //done
}

BOOL CWndGraph::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case _T('+'):
        ZoomIn();
    	break;
    case _T('-'):
        ZoomOut();
    	break;
    case _T('c'):
    case _T('C'):
        CenterGraph();
        break;
    case _T('f'):
    case _T('F'):
        FitToData();
        break;
    default:
        return UNHANDLED;
        break;
    }

    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;

}

BOOL CWndGraph::OnSaveGraph()
{
    OPENFILENAME sOpenFileName;
    TCHAR szFile[MAX_PATH];
    TCHAR szPathFolder[MAX_PATH];

    m_oStr->StringCopy(szPathFolder, _T("\\"));

    m_oStr->Empty(szFile);

    memset( &(sOpenFileName), 0, sizeof(sOpenFileName)); 
    sOpenFileName.lStructSize		= sizeof(sOpenFileName); 
    sOpenFileName.hwndOwner			= m_hWnd; 
    sOpenFileName.hInstance			= m_hInst;
    sOpenFileName.lpstrFilter		= lpFilter; 
    sOpenFileName.lpstrCustomFilter	= NULL;
    sOpenFileName.nMaxCustFilter	= 256; //must be at least 40, ignored if custom filter is NULL
    sOpenFileName.lpstrFile			= szFile; //The opened file name ... empty this puppy first (ie first char is \0)
    sOpenFileName.lpstrInitialDir	= szPathFolder;
    sOpenFileName.nMaxFile			= 256; //max length of szFile
    sOpenFileName.lpstrFileTitle	= NULL; //Pointer to a buffer that receives the file name and extension (without path information) of the selected file. This member can be NULL.
    sOpenFileName.nMaxFileTitle		= 256; //Specifies the size, in TCHARs, of the buffer pointed to by lpstrFileTitle - ignored if above is NULL
    sOpenFileName.lpstrTitle		= NULL; //title for the dialog...NULL will show the default
    //	sOpenFileName.Flags				= 0;	//check documentation...pretty sure I don't need it
    //there's a few more but I'm pretty sure I don't need them

    BOOL bRet = TRUE;
#ifdef UNDER_CE

    CDlgGetFile dlgGet;

    bRet = dlgGet.GetSaveFile(&sOpenFileName);
#else
	bRet = GetSaveFileName(&sOpenFileName);

    if(bRet == FALSE)
        goto Error;

    if(S_OK != m_oGDI.SaveToFile(szFile))
        bRet = FALSE;

Error:



#endif

    if(bRet == FALSE)
        MessageBox(m_hWnd, _T("Unable to save image"), _T("Graph Error"), MB_OK | MB_ICONEXCLAMATION);

    return bRet;
}

int CWndGraph::GetSystemMetrics(__in int nIndex)
{
	return GetSysMets(nIndex);
}