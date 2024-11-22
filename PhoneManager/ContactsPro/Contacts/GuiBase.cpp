#include "StdAfx.h"
#include "GuiBase.h"

CGuiBase::CGuiBase(void)
:m_oStr(CIssString::Instance())
,m_gdiGlowImg(NULL)
,m_gdiMem(NULL)
,m_sOptions(NULL)
{
}

CGuiBase::~CGuiBase(void)
{
}

BOOL CGuiBase::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions)
{
	m_hWndParent	= hWndParent;
	m_hInst			= hInst;
	m_gdiMem		= gdiMem;
	m_gdiBg	        = gdiBg;
    m_sOptions      = sOptions;
    m_guiBackground = guiBackground;

    return TRUE;
}

BOOL CGuiBase::MoveGui(RECT rcLocation)
{
	m_rcLocation	= rcLocation;
	return TRUE;
}

/***************************************
* GetVKey() generic version
* individual GUI's may need to implement
****************************************/
UINT CGuiBase::GetVKey(EnumKeyType eWhich)
{
    if(IsLandscape())
    {
        switch(eWhich)
        {
        case KEY_Up:        return VK_LEFT;
        case KEY_Down:      return VK_RIGHT;
        case KEY_Left:      return VK_DOWN;
        case KEY_Right:     return VK_UP;
        case KEY_TakeFocus: return VK_UP;
        case KEY_LoseFocus: return VK_DOWN;
        }
    }
    else
    {
        switch(eWhich)
        {
        case KEY_Up:        return VK_UP;
        case KEY_Down:      return VK_DOWN;
        case KEY_Left:      return VK_LEFT;
        case KEY_Right:     return VK_RIGHT;
        case KEY_TakeFocus: return VK_UP;
        case KEY_LoseFocus: return VK_DOWN;
        }
    }
    return 0x0;
}

BOOL CGuiBase::IsLandscape()
{
    return (GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN));
}

void CGuiBase::SetDirty(RECT& rc)
{
    if(IsRectEmpty(&m_rcDirty))
        m_rcDirty = rc;
    else
        UnionRect(&m_rcDirty, &m_rcDirty, &rc);

    // constrain 
    m_rcDirty.left = max(m_rcDirty.left, m_rcLocation.left);
    m_rcDirty.right = min(m_rcDirty.right, m_rcLocation.right);
    m_rcDirty.top = max(m_rcDirty.top, m_rcLocation.top);
    m_rcDirty.bottom = min(m_rcDirty.bottom, m_rcLocation.bottom);
}
