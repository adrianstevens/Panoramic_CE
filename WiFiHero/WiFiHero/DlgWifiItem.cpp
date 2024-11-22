#include "StdAfx.h"
#include "DlgWifiItem.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "resource.h"
#include "IssCommon.h"


#define APP_TITLE                       _T("WiFi Hero 0.5b")
#define IDT_TIMER                       161
#define TXT_DISPLAY_TIME                5000 

#define FILE_XmlSaved                   _T("wifi-saved.xml")
#define FILE_XmlDevice                  _T("wifi-device.xml")
#define FILE_XmlDiff                    _T("wifi-diff.xml")

#define COLOR_TEXT_NORMAL               (COLORREF)0xCFCFCF
#define COLOR_TEXT_SELECTED             (COLORREF)0x77AF39
#define HEIGHT_Text                     (GetSystemMetrics(SM_CXICON)*4/9)
#define BUTTON_Height                   (GetSystemMetrics(SM_CXICON))
#define BUTTON_HeightSelected           (GetSystemMetrics(SM_CXICON)*3/2)

enum EnumItems
{
    EI_Adhoc,
    EI_Authentication,
    EI_DestId,
    EI_EAPType,
    EI_Encryption,
    EI_KeyIndex,
    EI_KeyProvided,
    EI_NetworkKey,
    EI_Use8021x,
};

TypeWifi::TypeWifi()
:bAccessPoint(TRUE)
,szName(NULL)
,bOnDevice(TRUE)
{}

TypeWifi::~TypeWifi()
{
    CIssString* oStr = CIssString::Instance();
    bAccessPoint = FALSE;
    oStr->Delete(&szName);

    for(int i=0; i<arrParmNames.GetSize(); i++)
    {
        TCHAR* szParmName = arrParmNames[i];
        oStr->Delete(&szParmName);
    }
    arrParmNames.RemoveAll();

    for(int i=0; i<arrParmValues.GetSize(); i++)
    {
        TCHAR* szParmValues = arrParmValues[i];
        oStr->Delete(&szParmValues);
    }
    arrParmValues.RemoveAll();
}

void TypeWifi::Clone(TypeWifi* sWifi)
{
    if(!sWifi)
        return;

    CIssString* oStr = CIssString::Instance();
    bAccessPoint    = sWifi->bAccessPoint;
    bOnDevice       = sWifi->bOnDevice;
    szName          = oStr->CreateAndCopy(sWifi->szName);

    for(int i=0; i<sWifi->arrParmNames.GetSize(); i++)
    {
        TCHAR* szParmName = sWifi->arrParmNames[i];
        TCHAR* szNew = oStr->CreateAndCopy(szParmName);
        arrParmNames.AddElement(szNew);
    }

    for(int i=0; i<sWifi->arrParmValues.GetSize(); i++)
    {
        TCHAR* szParmValues = sWifi->arrParmValues[i];
        TCHAR* szNew = oStr->CreateAndCopy(szParmValues);
        arrParmValues.AddElement(szNew);
    }
}

CDlgWifiItem::CDlgWifiItem(void)
{
}

CDlgWifiItem::~CDlgWifiItem(void)
{
}

void CDlgWifiItem::Init(CIssGDIEx* gdiMem, 
                     CIssGDIEx* gdiBackground, 
                     CIssImageSliced* imgButton,
                     CIssImageSliced* imgButtonSel,
                     HFONT      hFontLabel,
                     TypeWifi*  sWifiItem)
{
    m_gdiMem        = gdiMem;
    m_gdiBack       = gdiBackground;
    m_imgButton     = imgButton;
    m_imgButtonSel  = imgButtonSel;
    m_hFontBtnLabel = hFontLabel;
    m_sWifiItem     = sWifiItem;
}

BOOL CDlgWifiItem::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;
	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= m_hWnd;
	mbi.nToolBarId	= IDR_MENU_Back; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

    m_oList.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_DrawScrollBar|OPTION_DrawScrollArrows|OPTION_Bounce);
    
    m_oList.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oList.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_oList.PreloadImages(hWnd, m_hInst);
    
    m_oList.SetCustomDrawFunc(DrawButtonItem, this);
    m_oList.SetDeleteItemFunc(DeleteMenuItem);

    for(int i=0; i<m_sWifiItem->arrParmNames.GetSize(); i++)
    {
        int* iValue = new int;
        *iValue = i;
        m_oList.AddItem((LPVOID)iValue, 5000);
    }
    
    m_oList.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oList.SetSelectedItemIndex(0, TRUE);

    m_bFirstTime = TRUE;
    
	return TRUE;
}

BOOL CDlgWifiItem::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    DrawBackground(*m_gdiMem, rcClip);

    if(IsRectInRect(rcClip, m_rcTitle))
    {
        DrawTextShadow(*m_gdiMem, m_sWifiItem->szName, m_rcTitle, DT_CENTER|DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), 0);
    }

    m_oList.OnDraw(*m_gdiMem, rcClient, rcClip);
 
    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
	
	return TRUE;
}

BOOL CDlgWifiItem::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

    if(GetSystemMetrics(SM_CXSCREEN) != m_gdiMem->GetWidth() && WIDTH(rc) == GetSystemMetrics(SM_CXSCREEN) && m_gdiMem->GetWidth() != 0)
    {
        // close the window if we go from portrait to landscape
        SafeCloseWindow(IDOK);
        return TRUE;
    }

    SetRect(&m_rcTitle, rc.left, rc.top, rc.right, rc.top + GetSystemMetrics(SM_CXICON));
    SetRect(&m_rcList, rc.left, m_rcTitle.bottom, rc.right, rc.bottom);

    m_oList.OnSize(m_rcList.left,m_rcList.top, WIDTH(m_rcList), HEIGHT(m_rcList));


	return TRUE;
}

BOOL CDlgWifiItem::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{
		::SetWindowText(GetWnd(), _T("Wifi Item"));
	}

	return UNHANDLED;
}

BOOL CDlgWifiItem::OnLButtonDown(HWND hWnd, POINT& pt)
{

    if(m_oList.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiItem::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oList.OnLButtonUp(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiItem::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oList.OnMouseMove(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiItem::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oList.OnKeyDown(wParam, lParam);
}

BOOL CDlgWifiItem::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgWifiItem::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
	case IDMENU_Back:
        SafeCloseWindow(IDOK);
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgWifiItem::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case _T('a'):
    case _T('A'):
        break;
    default:
        return UNHANDLED;
        break;

	}

	return TRUE;
}

BOOL CDlgWifiItem::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

void CDlgWifiItem::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgWifiItem* pThis = (CDlgWifiItem*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    int* iIndex = (int*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, iIndex);
}

BOOL CDlgWifiItem::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, int* iIndex)
{
    if(bIsHighlighted)
    {
        m_imgButtonSel->DrawImage(gdi, rc.left, rc.top);
    }
    else
    {
        m_imgButton->DrawImage(gdi, rc.left, rc.top);
    }

    TCHAR* szName = m_sWifiItem->arrParmNames[*iIndex];
    TCHAR* szValue= m_sWifiItem->arrParmValues[*iIndex];
    if(!szName || !szValue || !GetItemInfo(szName, szValue))
        return FALSE;

    RECT rcText;
    rcText			= rc;
    rcText.left		+= GetSystemMetrics(SM_CXSMICON)/2;
    rcText.right    -= GetSystemMetrics(SM_CXSMICON);

    
    DrawTextShadow(gdi, m_szName, rcText, DT_LEFT | DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), RGB(0,0,0));
    DrawTextShadow(gdi, m_szValue, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), RGB(0,0,0));


    return TRUE;
}

BOOL CDlgWifiItem::DrawBackground(CIssGDIEx& gdiMem, RECT& rcClip)
{
    Draw(gdiMem, rcClip, *m_gdiBack, rcClip.left, rcClip.top);

    return TRUE;
}

BOOL CDlgWifiItem::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oList.OnTimer(wParam, lParam))
        return TRUE;

    return UNHANDLED;
}

BOOL CDlgWifiItem::GetItemInfo(TCHAR* szName, TCHAR* szValue)
{
    m_oStr->Empty(m_szName);
    m_oStr->Empty(m_szValue);

    if(0 == m_oStr->Compare(_T("DestId"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Connects to:"));
        if(0 == m_oStr->Compare(_T("{436EF144-B4FB-4863-A041-8F905A62C572}"), szValue))
            m_oStr->StringCopy(m_szValue, _T("The Internet"));
        else if(0 == m_oStr->Compare(_T("{A1182988-0D73-439e-87AD-2A5B369F808B}"), szValue))
            m_oStr->StringCopy(m_szValue, _T("Work"));
        else
            m_oStr->StringCopy(m_szValue, _T("Unknown"));
    }
    else if(0 == m_oStr->Compare(_T("AdHoc"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Network:"));
        int i = m_oStr->StringToInt(szValue);
        if(i <= 0)
            m_oStr->StringCopy(m_szValue, _T("Access-Point"));
        else
            m_oStr->StringCopy(m_szValue, _T("Ad-Hoc"));
    }
    else if(0 == m_oStr->Compare(_T("Hidden"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Hidden:"));
        int i = m_oStr->StringToInt(szValue);
        if(i <= 0)
            m_oStr->StringCopy(m_szValue, _T("No"));
        else
            m_oStr->StringCopy(m_szValue, _T("Yes"));
    }
    else if(0 == m_oStr->Compare(_T("Authentication"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Authentication:"));
        int i = m_oStr->StringToInt(szValue);
        if(i == 1)
            m_oStr->StringCopy(m_szValue, _T("Shared"));
        else if(i == 3)
            m_oStr->StringCopy(m_szValue, _T("WPA"));
        else if(i == 4)
            m_oStr->StringCopy(m_szValue, _T("WPA-PSK"));
        else if(i == 5)
            m_oStr->StringCopy(m_szValue, _T("WPA-NONE"));
        else
            m_oStr->StringCopy(m_szValue, _T("Open"));
    }
    else if(0 == m_oStr->Compare(_T("Encryption"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Encryption:"));
        int i = m_oStr->StringToInt(szValue);
        if(i == 1)
            m_oStr->StringCopy(m_szValue, _T("None"));
        else if(i == 4)
            m_oStr->StringCopy(m_szValue, _T("TKIP"));
        else
            m_oStr->StringCopy(m_szValue, _T("WEP key"));
    }
    else if(0 == m_oStr->Compare(_T("KeyProvided"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Key Provided:"));
        int i = m_oStr->StringToInt(szValue);
        if(i <= 0)
            m_oStr->StringCopy(m_szValue, _T("No"));
        else
            m_oStr->StringCopy(m_szValue, _T("Yes"));
    }
    else if(0 == m_oStr->Compare(_T("NetworkKey"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Password:"));
        m_oStr->StringCopy(m_szValue, szValue);        
    }
    else if(0 == m_oStr->Compare(_T("KeyIndex"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("WEP Key Index:"));
        m_oStr->StringCopy(m_szValue, szValue);  
    }
    else if(0 == m_oStr->Compare(_T("Use8021x"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("Use 802.1x Network:"));
        int i = m_oStr->StringToInt(szValue);
        if(i <= 0)
            m_oStr->StringCopy(m_szValue, _T("No"));
        else
            m_oStr->StringCopy(m_szValue, _T("Yes"));
    }
    else if(0 == m_oStr->Compare(_T("EAPType"), szName))
    {
        m_oStr->StringCopy(m_szName, _T("EAP Protocol:"));
        int i = m_oStr->StringToInt(szValue);
        if(i == 13)
            m_oStr->StringCopy(m_szValue, _T("EAP-TLS"));
        else
            m_oStr->StringCopy(m_szValue, _T("PEAP"));
    }
    else
        return FALSE;

    return TRUE;
}

void CDlgWifiItem::DeleteMenuItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    int* iTemp = (int*)lpItem;

    if(iTemp)
    {
        delete iTemp;
        iTemp = NULL;
    }
}
