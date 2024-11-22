#include "DlgBaseOptions.h"
#include "ObjSkinEngine.h"
#include "Resource.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "IssRegistry.h"

CIssGDIEx CDlgBaseOptions::m_gdi;
CIssGDIEx CDlgBaseOptions::m_gdiBackArrow;
CIssGDIEx CDlgBaseOptions::m_gdiForwardArrow;
CGuiBackground CDlgBaseOptions::m_guiBG;
CIssKineticList CDlgBaseOptions::m_oMenu;
CIssWndTouchMenu CDlgBaseOptions::m_wndMenu;
CIssImageSliced CDlgBaseOptions::m_imgBtn;
CIssImageSliced CDlgBaseOptions::m_imgBtnSelected;

#define REG_Save                            _T("SOFTWARE\\Pano\\PhoneGenius\\Contacts")

CDlgBaseOptions::CDlgBaseOptions(void)
:m_bAnimate(FALSE)
{
    DWORD dwItem;
    if(S_OK == GetKey(REG_Save, _T("ColorScheme"), dwItem))
        g_eColorScheme = (EnumColorScheme)dwItem;
}

CDlgBaseOptions::~CDlgBaseOptions(void)
{
}

BOOL CDlgBaseOptions::PreloadImages(HWND hWndParent, HINSTANCE hInst)
{
    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.PreloadImages(hWndParent, hInst);

    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.PreloadImages(hWndParent, hInst);

    m_imgBtnSelected.Initialize(hWndParent, hInst, SKIN(IDR_PNG_DetailsBtnGreen));
    //m_imgBtnSelected.Initialize(hWndParent, hInst, IsVGA()?IDR_PNG_DetailsBtnGrnVGA:IDR_PNG_DetailsBtnGrn);
    m_imgBtn.Initialize(hWndParent, hInst, IsVGA()?IDR_PNG_DetailsBtnVGA:IDR_PNG_DetailsBtn);

    m_gdiBackArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWndParent, hInst, TRUE);
    m_gdiForwardArrow.LoadImage(IsVGA()?IDR_PNG_BackVGA:IDR_PNG_Back, hWndParent, hInst, TRUE);
    m_gdiForwardArrow.FlipVertical();

    m_guiBG.Init(GetDesktopWindow(), hInst);
    m_guiBG.PreloadImages();

    return TRUE;
}

void CDlgBaseOptions::OnColorSchemeChange(HWND hWndParent, HINSTANCE hInst)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.PreloadImages(hWndParent, hInst);

    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.PreloadImages(hWndParent, hInst);

    m_imgBtnSelected.Initialize(hWndParent, hInst, SKIN(IDR_PNG_DetailsBtnGreen));

    m_gdiBackArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWndParent, hInst, TRUE);
    //m_gdiForwardArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWndParent, hInst, TRUE);
    //m_gdiForwardArrow.FlipVertical();

    ::SetCursor(hCursor);
}

BOOL CDlgBaseOptions::Create(TCHAR* szWindowName, TCHAR* szClassName, HINSTANCE hInst)
{
    CDlgBase::Init(&m_gdi, &m_guiBG, TRUE, TRUE);

    if(CIssWnd::Create(szWindowName, NULL, hInst, szClassName, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
    {
        ShowWindow(m_hWnd, SW_SHOW);

        MSG msg;
        while( GetMessage( &msg,NULL,0,0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }
        return (BOOL)( msg.wParam );
    }
    return TRUE;
}

BOOL CDlgBaseOptions::Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate)
{
    CDlgBase::Init(&m_gdi, &m_guiBG, TRUE, TRUE);

    BOOL bRet = CDlgBase::Launch(hWndParent, hInst, bAnimate);
    return bRet;
}

void CDlgBaseOptions::DrawButton(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc)
{
    if(bIsHighlighted)
    {
        if(WIDTH(rc) != m_imgBtnSelected.GetWidth() || HEIGHT(rc) != m_imgBtnSelected.GetHeight())
            m_imgBtnSelected.SetSize(WIDTH(rc), HEIGHT(rc));

        m_imgBtnSelected.DrawImage(gdi, rc.left, rc.top);
    }
    else
    {
        if(WIDTH(rc) != m_imgBtn.GetWidth() || HEIGHT(rc) != m_imgBtn.GetHeight())
            m_imgBtn.SetSize(WIDTH(rc), HEIGHT(rc));

        m_imgBtn.DrawImage(gdi, rc.left, rc.top);
    }
}

BOOL CDlgBaseOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;


    if(m_oMenu.OnLButtonDown(pt))
        return TRUE;

    return FALSE;
}

BOOL CDlgBaseOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{

    // if the popup menu is going
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
        return TRUE;
    else if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < WIDTH(m_rcBottomBar)/2)
            OnMenuLeft();
        else
            OnMenuRight();
    }


    return FALSE;
}

BOOL CDlgBaseOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
        return TRUE;

    return FALSE;
}

BOOL CDlgBaseOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgBaseOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}
