#include "IssConstantsInterface.h"
#include "globals.h"
#include "IssLocalisation.h"


#ifdef WIN32_PLATFORM_WFSP
#define DISPLAY_TEXT_HEIGHT (GetSystemMetrics(SM_CXICON)*1/3)
#else
#define DISPLAY_TEXT_HEIGHT (GetSystemMetrics(SM_CXICON)*2/5)
#endif

#define IDT_CON_TIMER 4000 //seems like a good timer number and duration 
#define IDT_CON_FAVPLUS 4001


CIssConstantsInterface::CIssConstantsInterface(void)
:m_hFontButton(NULL)
,m_hFontDisplay(NULL)
,m_eDisStyle(CD_Normal)
,m_bDrawPlus(FALSE)
,m_szLastConst(NULL)
{
    LoadRegistry();
}

CIssConstantsInterface::~CIssConstantsInterface(void)
{
    SaveRegistry();
    CIssGDIEx::DeleteFont(m_hFontButton);
    CIssGDIEx::DeleteFont(m_hFontDisplay);
}

void CIssConstantsInterface::Init(TypeCalcInit* sInit)
{
    CCalcInterace::Init(sInit);

    //set the favorite constants string
 /*   if(sInit)
    {
        TCHAR szTemp[STRING_MAX];
        m_oStr->Empty(szTemp);
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_ConstantFavorites), STRING_MAX, sInit->hInst);
        sInit->oConstants->SetFavString(szTemp);
    }*/
    
    if((m_oBtnMan && m_oBtnMan->GetCalcType() == CALC_Constants) ||
        (sInit != NULL && m_hWndDlg != NULL && m_hInst != NULL))
    {
        //since in this case we're actually setting the rects in OnSize
        OnSize(m_hWndDlg, NULL, NULL);
	    CreateDisplays();
	    CreateButtons();

        if(m_oConstants->IsInitialized()== FALSE)
            m_oConstants->LoadConstants(m_hInst);
    }
}

void CIssConstantsInterface::SetPosition(HWND hWnd, int iYMain, int iYAlt)
{
    //lets create our rect
    m_rcDrawArea.top = iYAlt;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if(0 == iYAlt)
    {   //we're fat .. I mean wide
        m_rcDrawArea.top = iYAlt;
        m_rcDrawArea.bottom = rcClient.bottom;
        m_rcDrawArea.right = rcClient.right;
        m_rcDrawArea.left = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        m_rcDrawArea = rcClient;
        m_rcDrawArea.top = iYAlt;
        m_rcDrawArea.bottom = iYMain;
    }
}

BOOL CIssConstantsInterface::Draw(HDC hdc, RECT& rc)
{
	DrawDisplays(hdc, rc);
	DrawConstBtns(hdc, rc);
    DrawDisplayText(hdc, rc);

	return TRUE;
}

BOOL CIssConstantsInterface::DrawDisplays(HDC hdc, RECT& rcClip)
{
#ifndef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) <= GetSystemMetrics(SM_CYSCREEN))
#endif
    {
        m_oConstDisplay.Draw(hdc);
    }
	return TRUE;
}

BOOL CIssConstantsInterface::DrawDisplayText(HDC hdc, RECT& rcClip)
{
    RECT rcTemp;

#ifndef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
    {   //landscape
        rcTemp = m_rcConstDisplay;
        rcTemp.top += DISPLAY_INDENT;
        rcTemp.bottom -= DISPLAY_INDENT;

        if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oConstants->GetSelectedSection(0), m_oConstBtns[0].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[0].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oConstants->GetSelectedSection(1), m_oConstBtns[1].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[1].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oConstants->GetSelectedSection(2), m_oConstBtns[2].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[2].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[3].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
    }
    //square
    else if(GetSystemMetrics(SM_CXSCREEN) == GetSystemMetrics(SM_CYSCREEN))
    {
        //draw button labels
        //port & square
        
        rcTemp = m_oConstBtns[0].GetButtonRect();
        rcTemp.top += DISPLAY_INDENT*3;
        if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        rcTemp = m_oConstBtns[1].GetButtonRect();
        rcTemp.top += DISPLAY_INDENT*3;
        if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        rcTemp = m_oConstBtns[2].GetButtonRect();
        rcTemp.top += DISPLAY_INDENT*3;
        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        rcTemp = m_oConstBtns[3].GetButtonRect();
        rcTemp.top += DISPLAY_INDENT*3;
        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), rcTemp, DT_CENTER |  DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_eDisStyle == CD_Normal)
        {
            //port & square
            rcTemp = m_rcConstDisplay;
            //      rcTemp.top += 2*DISPLAY_INDENT;
            rcTemp.bottom -= 2*DISPLAY_INDENT;
            rcTemp.left += 4*DISPLAY_INDENT;
            rcTemp.right -= 4*DISPLAY_INDENT;

            if(m_oConstants->GetSelectedSection(0))
                DrawText(hdc, m_oConstants->GetSelectedSection(0), rcTemp, DT_LEFT | DT_TOP, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);

            if(m_oConstants->GetSelectedSection(1))
                DrawText(hdc, m_oConstants->GetSelectedSection(1), rcTemp, DT_RIGHT | DT_TOP, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);

            if(m_oConstants->GetSelectedSection(2))
                DrawText(hdc, m_oConstants->GetSelectedSection(2), rcTemp, DT_CENTER | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
        }
        else if(m_eDisStyle == CD_LastValue)
        {
            rcTemp = m_rcConstDisplay;
            rcTemp.top += DISPLAY_INDENT;
            rcTemp.bottom -= DISPLAY_INDENT;

            DrawText(hdc, m_szLastConst, m_oConstDisplay.GetPosition(), DT_CENTER |  DT_VCENTER | DT_END_ELLIPSIS, m_hFontButton, m_oBtnMan->GetSkin()->m_crTextTop);
        }
    }
    else
#endif
    {   
        //draw button labels
        //port & square
        rcTemp = m_rcConstDisplay;
        rcTemp.top += DISPLAY_INDENT;
        rcTemp.bottom -= DISPLAY_INDENT;

        if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), m_oConstBtns[0].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[0].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), m_oConstBtns[1].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(0))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[1].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Group), m_hInst), m_oConstBtns[2].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        else if(m_oConstants->GetSelectedSection(1))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[2].GetButtonRect(), DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);

        if(m_oConstants->GetSelectedSection(2))
            DrawText(hdc, m_oStr->GetText(ID(IDS_MENU_Constants), m_hInst), m_oConstBtns[3].GetButtonRect(), DT_CENTER |  DT_VCENTER | DT_END_ELLIPSIS, m_hFontDisplay, m_oBtnMan->GetSkin()->GetButton(0)->TextColor);
        
        if(m_eDisStyle == CD_Normal)
        {
            //port & square
            rcTemp = m_rcConstDisplay;
      //      rcTemp.top += 2*DISPLAY_INDENT;
             rcTemp.bottom -= DISPLAY_INDENT;

            if(m_oConstants->GetSelectedSection(0))
                DrawText(hdc, m_oConstants->GetSelectedSection(0), rcTemp, DT_CENTER | DT_TOP, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
            
            if(m_oConstants->GetSelectedSection(1))
                DrawText(hdc, m_oConstants->GetSelectedSection(1), rcTemp, DT_CENTER | DT_VCENTER, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
            
            if(m_oConstants->GetSelectedSection(2))
                DrawText(hdc, m_oConstants->GetSelectedSection(2), rcTemp, DT_CENTER | DT_BOTTOM, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
        }
        else
        {
            DrawText(hdc, m_szLastConst, m_oConstDisplay.GetPosition(), DT_CENTER |  DT_VCENTER | DT_END_ELLIPSIS, m_hFontButton, m_oBtnMan->GetSkin()->m_crTextTop);
        }
    }

    //draw the plus
    if(m_bDrawPlus)
    {
        rcTemp = m_rcDrawArea;
        rcTemp.left += GetSystemMetrics(SM_CXSMICON)/2;
        rcTemp.top += GetSystemMetrics(SM_CXSMICON)/2;
        DrawText(hdc, _T("+"), rcTemp, DT_LEFT | DT_TOP, m_hFontDisplay, m_oBtnMan->GetSkin()->m_crTextTop);
    }


    return TRUE;
}

BOOL CIssConstantsInterface::DrawConstBtns(HDC hdc, RECT& rcClip)
{
    LayoutCalcType* sLayout = m_oBtnMan->GetCurrentLayout();

    for(int i = 0; i < 4; i++)
    {
        if(m_oConstants->GetGroupCount(i) == 0)
            break;

        m_oConstBtns[i].Draw(NULL, hdc, _T(" "));

        DrawDownArrow(hdc, m_oConstBtns[i].GetButtonRect(), 
                m_oBtnMan->GetSkin()->GetButton(0)->TextColor,
                m_oBtnMan->GetSkin()->GetButton(0)->TextColor2,
                m_oBtnMan->GetSkin()->m_bUseTextShadow);
    }

    m_oConstBtns[CSB_AddFav].Draw(m_hFontButton, hdc, _T("+Fav"));
    return TRUE;
}

BOOL CIssConstantsInterface::OnLButtonDown(HWND hWnd, POINT& pt)
{
    for(int i = 0; i < (int)CSB_Count; i++)
    {
        if(m_oConstBtns[i].OnLButtonDown(pt))
            return TRUE;
    }
    return UNHANDLED;
}

BOOL CIssConstantsInterface::OnLButtonUp(HWND hWnd, POINT& pt)
{
    for(int i = 0; i < (int)CSB_Count; i++)
    {
        if(m_oConstBtns[i].OnLButtonUp(pt))
        {   //I'm lazy and this is easy
            switch(i)
            {
            case CSB_1:
                OnBtn1();
            	break;
            case CSB_2:
                OnBtn2();
            	break;
            case CSB_3:
                OnBtn3();
                break;
            case CSB_4:
                OnBtn4();
                break;
            case CSB_AddFav:
                OnBtnFavorites();
                break;
            }
            return TRUE;
        }
    }
    return UNHANDLED;
}

BOOL CIssConstantsInterface::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int iParam = LOWORD(wParam);

    if(iParam >= IDMENU_Const1 && iParam < IDMENU_Const1 + 200)
    {
        SelectMenuItem(0, iParam - IDMENU_Const1);
    }
    else if(iParam >= IDMENU_Const2 && iParam < IDMENU_Const2 + 200)
    {	
        SelectMenuItem(1, iParam - IDMENU_Const2);
    }
    else if(iParam >= IDMENU_Const3 && iParam < IDMENU_Const3 + 200)
    {
        SelectMenuItem(2, iParam - IDMENU_Const3);
    }
    else if(iParam >= IDMENU_Const4 && iParam < IDMENU_Const4 + 200)
    {
        SelectMenuItem(3, iParam - IDMENU_Const4);
    }
    else
        return FALSE;

    return TRUE;
}

BOOL CIssConstantsInterface::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //set our rects
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iMenuHeight = 0;
    if(m_oMenu)
        iMenuHeight = m_oMenu->GetMenuHeight();


    //landscape

    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
    {
#ifdef WIN32_PLATFORM_WFSP
//really should have the display
        int iBtnWidth = (WIDTH(m_rcDrawArea)-5*DISPLAY_INDENT)/4;

        m_rcConstDisplay.top = m_rcDrawArea.top + DISPLAY_INDENT;
        m_rcConstDisplay.left = m_rcDrawArea.left + DISPLAY_INDENT;
        m_rcConstDisplay.bottom = m_rcConstDisplay.top + HEIGHT(m_rcDrawArea)/2 - DISPLAY_INDENT - iMenuHeight/2;
        m_rcConstDisplay.right = m_rcDrawArea.right - 2*DISPLAY_INDENT - HEIGHT(m_rcDrawArea)*5/8;

        m_rcFavBtns = m_rcConstDisplay;
        m_rcFavBtns.left = m_rcConstDisplay.right + DISPLAY_INDENT;
        m_rcFavBtns.right = m_rcDrawArea.right - DISPLAY_INDENT;


        m_rcBtns[0].top = m_rcConstDisplay.bottom + DISPLAY_INDENT;
        m_rcBtns[0].bottom = m_rcDrawArea.bottom - iMenuHeight - DISPLAY_INDENT;
        m_rcBtns[0].left = m_rcDrawArea.left + DISPLAY_INDENT;
        m_rcBtns[0].right = m_rcBtns[0].left + iBtnWidth;

        m_rcBtns[1] = m_rcBtns[0];
        m_rcBtns[1].left = m_rcBtns[0].right + DISPLAY_INDENT;
        m_rcBtns[1].right = m_rcBtns[1].left + iBtnWidth;

        m_rcBtns[2] = m_rcBtns[0];
        m_rcBtns[2].left = m_rcBtns[1].right + DISPLAY_INDENT;
        m_rcBtns[2].right = m_rcBtns[2].left + iBtnWidth;

        m_rcBtns[3] = m_rcBtns[0];
        m_rcBtns[3].left = m_rcBtns[2].right + DISPLAY_INDENT;
        m_rcBtns[3].right = m_rcBtns[3].left + iBtnWidth;

#else
        //start from the bottom
        int iBtnHeight = (HEIGHT(m_rcDrawArea) - iMenuHeight - 6*DISPLAY_INDENT)/5;

        m_rcFavBtns.left    = m_rcDrawArea.left;
        m_rcFavBtns.right   = m_rcDrawArea.right - DISPLAY_INDENT;
        m_rcFavBtns.top     = m_rcDrawArea.top + DISPLAY_INDENT;
        m_rcFavBtns.bottom  = m_rcFavBtns.top + iBtnHeight;

        m_rcBtns[0] = m_rcFavBtns;
        m_rcBtns[0].top = m_rcFavBtns.bottom + DISPLAY_INDENT;
        m_rcBtns[0].bottom = m_rcBtns[0].top + iBtnHeight;

        m_rcBtns[1] = m_rcBtns[0];
        m_rcBtns[1].top = m_rcBtns[0].bottom + DISPLAY_INDENT;
        m_rcBtns[1].bottom = m_rcBtns[1].top + iBtnHeight;

        m_rcBtns[2] = m_rcBtns[1];
        m_rcBtns[2].top = m_rcBtns[1].bottom + DISPLAY_INDENT;
        m_rcBtns[2].bottom = m_rcBtns[2].top + iBtnHeight;

        m_rcBtns[3] = m_rcBtns[2];
        m_rcBtns[3].top = m_rcBtns[2].bottom + DISPLAY_INDENT;
        m_rcBtns[3].bottom = m_rcBtns[3].top + iBtnHeight;
#endif
    }//portrait
    else

    {
        int iBtnWidth = (WIDTH(m_rcDrawArea)-5*DISPLAY_INDENT)/4;

        m_rcConstDisplay.top = m_rcDrawArea.top + DISPLAY_INDENT;
        m_rcConstDisplay.left = m_rcDrawArea.left + DISPLAY_INDENT;
        m_rcConstDisplay.bottom = m_rcConstDisplay.top + HEIGHT(m_rcDrawArea)/2 - DISPLAY_INDENT;
        m_rcConstDisplay.right = m_rcDrawArea.right - 2*DISPLAY_INDENT - HEIGHT(m_rcDrawArea)*5/8;

        m_rcFavBtns = m_rcConstDisplay;
        m_rcFavBtns.left = m_rcConstDisplay.right + DISPLAY_INDENT;
        m_rcFavBtns.right = m_rcDrawArea.right - DISPLAY_INDENT;
        

        m_rcBtns[0].top = m_rcConstDisplay.bottom + DISPLAY_INDENT;
        m_rcBtns[0].bottom = m_rcDrawArea.bottom;
        m_rcBtns[0].left = m_rcDrawArea.left + DISPLAY_INDENT;
        m_rcBtns[0].right = m_rcBtns[0].left + iBtnWidth;

        m_rcBtns[1] = m_rcBtns[0];
        m_rcBtns[1].left = m_rcBtns[0].right + DISPLAY_INDENT;
        m_rcBtns[1].right = m_rcBtns[1].left + iBtnWidth;

        m_rcBtns[2] = m_rcBtns[0];
        m_rcBtns[2].left = m_rcBtns[1].right + DISPLAY_INDENT;
        m_rcBtns[2].right = m_rcBtns[2].left + iBtnWidth;

        m_rcBtns[3] = m_rcBtns[0];
        m_rcBtns[3].left = m_rcBtns[2].right + DISPLAY_INDENT;
        m_rcBtns[3].right = m_rcBtns[3].left + iBtnWidth;
    }


	if(m_hFontDisplay)
		CIssGDIEx::DeleteFont(m_hFontDisplay);
	if(m_hFontButton)
		CIssGDIEx::DeleteFont(m_hFontButton);

#ifdef WIN32_PLATFORM_WFSP
	m_hFontDisplay = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/14
		, FW_NORMAL, TRUE); 
#else
	m_hFontDisplay = CIssGDIEx::CreateFont(DISPLAY_TEXT_HEIGHT, FW_NORMAL, TRUE);
#endif
	m_hFontButton = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_NORMAL, TRUE);



    return TRUE;
}

BOOL CIssConstantsInterface::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

void CIssConstantsInterface::CreateButtons()   
{
    CalcProSkins* oSkin = m_oBtnMan->GetSkin();
 
    int iStyle = 0;

    CCalcDynBtn* oBtn = NULL;

    for(int i = 0; i < (int)CSB_Count; i++)
    {
        m_oConstBtns[i].Destroy();
        m_oConstBtns[i].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
        m_oConstBtns[i].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
            oSkin->m_typeBtnClr[iStyle].ButtonColor2,
            oSkin->m_typeBtnClr[iStyle].OutlineColor1,
            oSkin->m_typeBtnClr[iStyle].OutlineColor2);

        m_oConstBtns[i].Init(m_rcBtns[i], _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, oBtn);
        oBtn = &m_oConstBtns[0];
    }

    iStyle = 3;

    m_oConstBtns[CSB_AddFav].Destroy();
    m_oConstBtns[CSB_AddFav].SetTextColors(oSkin->m_typeBtnClr[iStyle].TextColor, oSkin->m_typeBtnClr[iStyle].TextColor2, oSkin->m_bUseTextShadow);
    m_oConstBtns[CSB_AddFav].InitAdvanced(oSkin->m_typeBtnClr[iStyle].ButtonColor1,
        oSkin->m_typeBtnClr[iStyle].ButtonColor2,
        oSkin->m_typeBtnClr[iStyle].OutlineColor1,
        oSkin->m_typeBtnClr[iStyle].OutlineColor2);

    m_oConstBtns[CSB_AddFav].Init(m_rcFavBtns, _T(" "), m_hWndDlg, NULL, 0, RGB(255,0,0), oSkin->m_eBtnType, NULL);
}

void CIssConstantsInterface::CreateDisplays()
{
    CalcProSkins* oSkin = m_oBtnMan->GetSkin();//makes for tidier code

    m_oConstDisplay.Init(m_rcConstDisplay, oSkin->m_typeDisplay.ButtonColor1
        , oSkin->m_typeDisplay.ButtonColor2
        , oSkin->m_typeDisplay.OutlineColor1
        , oSkin->m_typeDisplay.OutlineColor2,
        DISPLAY_Grad_DS);
}

void CIssConstantsInterface::LoadRegistry()
{

}

void CIssConstantsInterface::SaveRegistry()
{

}

void CIssConstantsInterface::OnBtnFavorites()
{
    if(m_oConstants->AddToFavorites())
    {
        // BUGBUG: should play a sound to add to favorites
        MessageBeep(MB_OK);
        m_bDrawPlus = TRUE;
        SetTimer(m_hWndDlg, IDT_CON_FAVPLUS, 2000, NULL);
        InvalidateRect(m_hWndDlg, NULL, FALSE);
    }
    else
    {
        // BUGBUG: should play a sound that add to favorite didn't work
        MessageBeep(MB_ICONHAND);
    }
}

void CIssConstantsInterface::OnBtn1()
{
    LaunchMenu(0, IDMENU_Const1);
}

void CIssConstantsInterface::OnBtn2()
{
    LaunchMenu(1, IDMENU_Const2);
}

void CIssConstantsInterface::OnBtn3()
{
    LaunchMenu(2, IDMENU_Const3);
}

void CIssConstantsInterface::OnBtn4()
{
    LaunchMenu(3, IDMENU_Const4);
}

void CIssConstantsInterface::LaunchMenu(int iBtn, UINT uiMenuID)
{
    if(m_oConstants->GetGroupCount(iBtn) == 0)
        return;
    
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu)
        return;

    TCHAR szText[STRING_MAX];

    for(int i=0; i<m_oConstants->GetGroupCount(iBtn); i++)
    {
        m_oStr->StringCopy(szText, m_oConstants->GetGroupName(iBtn,i));
        AppendMenu(hMenu, MF_STRING, i + uiMenuID, szText);
    }

    //and add favorites entry ... little hackish 
    if(iBtn == 0)
    {
        m_oStr->StringCopy(szText, ID(IDS_MENU_ConstantFavorites), STRING_MAX, m_hInst);
        AppendMenu(hMenu, MF_STRING, m_oConstants->GetGroupCount(iBtn) + uiMenuID, szText);
    }

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWndDlg, &rc);
    pt.x	= m_oConstBtns[iBtn].GetButtonRect().right;
    pt.y	= m_oConstBtns[iBtn].GetButtonRect().bottom;

    ClientToScreen(m_hWndDlg, &pt);

    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWndDlg, NULL);

    DestroyMenu(hMenu);
    return;
}

void CIssConstantsInterface::SelectMenuItem(int iBtn, int iIndex)
{
    if(m_oConstants->IsGroupAConstant(iBtn, iIndex))
    {
        // put in the parameters on the screen
        TCHAR* szConstant = m_oConstants->GetGroupConstant(iBtn, iIndex);
        m_szLastConst = m_oConstants->GetGroupName(iBtn, iIndex);
        if(szConstant)
        {
            m_oCalc->AddString(szConstant);
        }
        if(m_szLastConst)
        {
            SetTimer(m_hWndDlg, IDT_CON_TIMER, IDT_CON_TIMER, NULL);
            m_eDisStyle = CD_LastValue;
        }
        InvalidateRect(m_hWndDlg, NULL, FALSE);
        return;
    }

    if(iBtn == 0 && iIndex == m_oConstants->GetGroupCount(0))
    {
        PostMessage(m_hWndDlg, WM_CALCENGINE, (WPARAM)INPUT_CalcState, (LPARAM)CALCSTATE_Constants);     
    }
    else
    {
        m_oConstants->SetGroup(iBtn, iIndex);
        m_iSelector = iBtn+1;
        InvalidateRect(m_hWndDlg, NULL, FALSE);
    }
}

BOOL CIssConstantsInterface::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == IDT_CON_TIMER)
    {
        KillTimer(hWnd, IDT_CON_TIMER);
        m_eDisStyle = CD_Normal;
        InvalidateRect(hWnd, &m_rcDrawArea, FALSE);
        //m_szLastConst = NULL;
        return TRUE;
    }

    if(wParam == IDT_CON_FAVPLUS)
    {
        m_bDrawPlus = FALSE;
        KillTimer(hWnd, IDT_CON_FAVPLUS);
        InvalidateRect(hWnd, NULL, FALSE);

    }
    return UNHANDLED;
}

BOOL CIssConstantsInterface::HandleDPad(int iVKKey)
{
    int iCount = 0;

    for(int i = 0; i < 4; i++)
    {
        if(m_oConstants->GetGroupCount(i) == 0)
            break;
        else
            iCount++;
    }

    switch(iVKKey)
    {
    case VK_LEFT://good
        if(m_iSelector > 0)
            m_iSelector = min(m_iSelector - 1, iCount - 1);
        else 
            m_iSelector = 4;
        InvalidateRect(m_hWndDlg, NULL, FALSE);//its fast enough
    	break;
    case VK_RIGHT:
        if(m_iSelector < 4)
        {
            m_iSelector++;
            if(m_iSelector >= iCount)
                m_iSelector = 4;
        }
        else 
            m_iSelector = 0;
        InvalidateRect(m_hWndDlg, NULL, FALSE);//its fast enough
    	break;
    case VK_DOWN:
    case VK_UP:
        if(m_iSelector < 4)
            m_iSelector = 4;
        else
            m_iSelector = min(iCount-1, 3);
        InvalidateRect(m_hWndDlg, NULL, FALSE);//its fast enough
        break;
    case VK_RETURN:
        {
            switch(m_iSelector)
            {
            case CSB_1:
                OnBtn1();
            	break;
            case CSB_2:
                OnBtn2();
            	break;
            case CSB_3:
                OnBtn3();
                break;
            case CSB_4:
                OnBtn4();
                break;
            case CSB_AddFav:
                OnBtnFavorites();
                break;
            default:
                return FALSE;
                break;
            }
        }
        break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL CIssConstantsInterface::DrawSelector(HDC hdc, RECT& rc)
{
    RECT rcTemp;

    if(m_iSelector < 0)
        return FALSE;

    if(m_iSelector < 4)
        rcTemp = m_rcBtns[m_iSelector];
    else 
        rcTemp = m_rcFavBtns;

    int iRadius = GetSystemMetrics(SM_CXICON)/6;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(WHITE_PEN));
    //	CIssGDI::FrameRect(hDC, rcTemp, 0xFFFFFF, 1);
    RoundRect(hdc, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, iRadius, iRadius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    return TRUE;
}

BOOL CIssConstantsInterface::DrawMainButtons()
{
#ifdef WIN32_PLATFORM_WFSP
    if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN))
        return FALSE;
#endif
    return TRUE;
}