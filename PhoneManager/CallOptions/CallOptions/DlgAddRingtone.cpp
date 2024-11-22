#include "StdAfx.h"
#include "DlgAddRingtone.h"
#include "CallOptionsGuiDefines.h"
#include "PoomContacts.h"
#include "DlgCallOptions.h"
#include "DlgChooseContact.h"
#include "IssCommon.h"
#include "Resource.h"
#include "ObjSkinEngine.h"
#include "DlgChooseCategory.h"
#include "DlgChooseContact.h"
#include "IssDebug.h"

#define USE_WMP
#ifdef USE_WMP
#include "ObjMediaPlayer.h"
#endif

#define IDMENU_Selection        5000
#define IDMENU_Popup            5001


CDlgAddRingtone::CDlgAddRingtone(CObjRingtoneRuleList* objRuleList)
:m_sFileList(NULL)
,m_objRuleList(objRuleList)
,m_bEditMode(FALSE)
{
    m_sRule.eRuleType   = NumRingtoneRuleTypes;
    m_sRule.eRingType   = RT_Count;
    m_sRule.iOID        = 0;
    ZeroMemory(&m_sRule.ringToneInfo, sizeof(SNDFILEINFO));
    ZeroMemory(m_sRule.szDesc, sizeof(TCHAR)*STRING_MAX);
	m_hBtnFont	    = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgAddRingtone::~CDlgAddRingtone(void)
{
	CIssGDIEx::DeleteFont(m_hBtnFont);
    CIssGDIEx::DeleteFont(m_hFontBtnText);

#ifdef USE_WMP
	//CObjMediaPlayer::Destroy();
#endif

	if(m_sFileList)
		LocalFree(m_sFileList);
}

BOOL CDlgAddRingtone::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    SndGetSoundFileList(SND_EVENT_RINGTONELINE1 ,SND_LOCATION_ALL, &m_sFileList, &m_iFileCount);

    if(!m_oStr->IsEmpty(m_sRule.ringToneInfo.szDisplayName) && !m_oStr->IsEmpty(m_sRule.ringToneInfo.szPathName))
    {
#ifdef USE_WMP
        //CObjMediaPlayer::GetPlayer().SetFile(m_sRule.ringToneInfo.szPathName);
#else
        HSOUND hSndFile;
        SndOpen(pInfo->szPathName, hSndFile);
#endif
    }

    RefreshList();

    ::SetCursor(hCursor);
	return TRUE;
}

void CDlgAddRingtone::SetEditItem(TypeRingtoneRule* sItem)
{
    if(!sItem)
        return;

    memcpy(&m_sRule, sItem, sizeof(TypeRingtoneRule));

	m_bEditMode = TRUE;
}

void CDlgAddRingtone::CreateButtons(int iTop)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    int iIconSize	= GetSystemMetrics(SM_CXICON);

	RECT rcPlay, rcStop;

    rcPlay.top      = rcStop.top    = iTop;
    rcPlay.bottom   = rcStop.bottom = iTop + iIconSize;
    rcStop.right    = m_rcArea.right;
    rcStop.left     = rcStop.right - 2*iIconSize;
    rcPlay.right    = rcStop.left - INDENT/2;
    rcPlay.left     = rcPlay.right - 2*iIconSize;

    m_btnPlay.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnPlay.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnPlay.Init(rcPlay, _T("> Play"), m_hWnd, WM_Play, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	TCHAR szStop[2] = {_T(' '-1), _T('\0')};

    TCHAR szTemp[STRING_NORMAL];
    m_oStr->Format(szTemp, _T("%s %s"), szStop, _T("Stop"));

	m_btnStop.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnStop.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnStop.Init(rcStop, szTemp, m_hWnd, WM_Stop, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);
}

BOOL CDlgAddRingtone::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
/*#ifdef USE_WMP
	CObjMediaPlayer::GetPlayer().HandleTransportCommand(uiMessage);
#endif*/
	switch(uiMessage)
	{
	case WM_Play:
		OnPlay();
		break;
	case WM_Stop:
		OnStop();
		break;
	default:
		return UNHANDLED;
	}

	return TRUE;
}

void CDlgAddRingtone::OnPlay()
{
	OnStop();

	EnumRingType eRing = m_sRule.eRingType;
	if(eRing == RT_Count)
		eRing = RT_Ring;

	if(m_oStr->IsEmpty(m_sRule.ringToneInfo.szPathName))
	{
		MessageBeep(MB_ICONHAND);
		return;
	}

	m_objRingtoneEngine.PlayRingToneEvent(&m_objPhoneProfile, m_sRule.ringToneInfo.szPathName, eRing, TRUE);
}

void CDlgAddRingtone::OnStop()
{
	m_objRingtoneEngine.StopRingToneEvent();
}

BOOL CDlgAddRingtone::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    TypeItems* sItem = m_oMenu.GetItem(0);
    if(!sItem)
        return TRUE;

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), 3*sItem->iHeight);

    CreateButtons(m_rcArea.top + 3*sItem->iHeight + 1);
    


	return TRUE;
}

BOOL CDlgAddRingtone::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonDown(hWnd, pt))
    {}
    else if(m_btnPlay.OnLButtonDown(pt))
    {}
    else if(m_btnStop.OnLButtonDown(pt))
    {}

	return UNHANDLED;
}

BOOL CDlgAddRingtone::OnLButtonUp(HWND hWnd, POINT& pt)
{

    if(CDlgBaseOptions::OnLButtonUp(hWnd, pt))
    {}
    else if(m_btnPlay.OnLButtonUp(pt))
    {}
    else if(m_btnStop.OnLButtonUp(pt))
    {}

	return UNHANDLED;
}

BOOL CDlgAddRingtone::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Ok:
		break;
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
	case IDMENU_Cancel:
		//EndDialog(hWnd, IDCANCEL);
		break;
    case IDMENU_Selection:
        LaunchPopupMenu();
        break;
    case IDMENU_Popup:
        SetSelection();
        break;
	default:
		return UNHANDLED;
	}

	return TRUE;
}


BOOL CDlgAddRingtone::StoreRingtoneRule(TypeRingtoneRule* pRule)
{
	if(m_objRuleList)
		return m_objRuleList->AddItem(pRule);

	return FALSE;
}

void CDlgAddRingtone::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumAddRingtone* eCall = (EnumAddRingtone*)lpItem;
    delete eCall;
}

void CDlgAddRingtone::RefreshList()
{
    // if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

    EnumAddRingtone* eOption = new EnumAddRingtone;
    *eOption = AR_Rule;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumAddRingtone;
    *eOption = AR_RingType;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumAddRingtone;
    *eOption = AR_RingTone;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

     m_oMenu.SetSelectedItemIndex(0, TRUE);
}

void CDlgAddRingtone::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgAddRingtone* pThis = (CDlgAddRingtone*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumAddRingtone* eRing = (EnumAddRingtone*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eRing);
}

BOOL CDlgAddRingtone::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumAddRingtone* eRing)
{
    if(!(*eRing == AR_Rule && m_bEditMode))
        Draw(gdi,
            rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
            m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
            m_oMenu.GetImageArrayGDI(),
            (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0); 

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eRing, szTitle);

    switch(*eRing)
    {
    case AR_Rule:
        GetRuleText(m_sRule.eRuleType, szInfo);
        break;
    case AR_RingType:
        GetRintTypeText(m_sRule.eRingType, szInfo);
        break;
    case AR_RingTone:
        if(m_oStr->IsEmpty(m_sRule.ringToneInfo.szDisplayName))
            m_oStr->StringCopy(szInfo, _T("Select ringtone..."));
        else
            m_oStr->StringCopy(szInfo, m_sRule.ringToneInfo.szDisplayName);
        break;
    default:
        return TRUE;
    }

    RECT rcText = rc;
    rcText.left += INDENT;
    rcText.right = rc.right - m_oMenu.GetImageArrayGDI().GetHeight();

    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));


    DrawText(gdi,  szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(255,255,255):RGB(167,167,167)));
    return TRUE;
}

void CDlgAddRingtone::GetRintTypeText(EnumRingType eRing, TCHAR* szText)
{
    CIssString* oStr = CIssString::Instance();
    switch(eRing)
    {
    case RT_Ring:
        oStr->StringCopy(szText, _T("Ring"));
        break;
    case RT_IncreasingRing:
        oStr->StringCopy(szText, _T("Increasing ring"));
        break;
    case RT_RingOnce:
        oStr->StringCopy(szText, _T("Ring once"));
        break;
    case RT_Vibrate:
        oStr->StringCopy(szText, _T("Vibrate"));
        break;
    case RT_VibrateAndRing:
        oStr->StringCopy(szText, _T("Vibrate and ring"));
        break;
    case RT_VibrateThenRing:
        oStr->StringCopy(szText, _T("Vibrate then ring"));
        break;
    case RT_None:
        oStr->StringCopy(szText, _T("None"));
        break;
    case RT_Count:
        oStr->StringCopy(szText, _T("Select ring type..."));
        break;
    }
}

void CDlgAddRingtone::GetRuleText(EnumRingtoneRuleType eRule, TCHAR* szText, BOOL bPopupMenu)
{
    switch(eRule)
    {
    case RR_Category:
        m_oStr->StringCopy(szText, bPopupMenu?_T("Categories..."):_T("Category: "));
        if(!bPopupMenu)
            m_oStr->Concatenate(szText, m_sRule.szDesc);
        break;
    case RR_Contact:
        m_oStr->StringCopy(szText, bPopupMenu?_T("Contacts..."):_T("Contact: "));
        if(!bPopupMenu)
            m_oStr->Concatenate(szText, m_sRule.szDesc);
        break;
    case RR_Default:
        m_oStr->StringCopy(szText, _T("Default ring"));
        break;
    case NumRingtoneRuleTypes:
        m_oStr->StringCopy(szText, _T("Select rule..."));
        break;
    }
}

void CDlgAddRingtone::GetItemTitle(EnumAddRingtone eRing, TCHAR* szText)
{
    switch(eRing)
    {
    case AR_Rule:
        m_oStr->StringCopy(szText, _T("Rule:"));
        break;
    case AR_RingType:
        m_oStr->StringCopy(szText, _T("Ring type:"));
        break;
    case AR_RingTone:
        m_oStr->StringCopy(szText, _T("Ringtone:"));
        break;
    }
}

BOOL CDlgAddRingtone::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_oMenu.GetItemCount() == 0)
    {
        RECT rc = m_rcArea;
        rc.top  += INDENT;
        DrawTextShadow(gdi, _T("No entries found"), rc, DT_CENTER|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

    // draw buttons
    m_btnPlay.Draw(m_hBtnFont, gdi);
    m_btnStop.Draw(m_hBtnFont, gdi);

    return TRUE;
}

void CDlgAddRingtone::OnMenuLeft()
{
	OnStop();
    /*if(m_sRule.eRingType == RT_Count || 
       m_sRule.eRuleType == NumRingtoneRuleTypes || 
       (m_sRule.eRuleType == RR_Contact && m_sRule.iOID == 0) || 
       (m_sRule.eRuleType == RR_Category && m_oStr->IsEmpty(m_sRule.szDesc)) ||
       m_oStr->IsEmpty(m_sRule.ringToneInfo.szDisplayName) ||
       m_oStr->IsEmpty(m_sRule.ringToneInfo.szPathName))
        SafeCloseWindow(IDCANCEL);
    else
        SafeCloseWindow(IDOK);*/

    // this is just not allowed
    if(  m_sRule.eRingType == RT_Count || 
         m_sRule.eRuleType == NumRingtoneRuleTypes || 
        (m_sRule.eRuleType == RR_Contact && m_sRule.iOID == 0) || 
        (m_sRule.eRuleType == RR_Category && m_oStr->IsEmpty(m_sRule.szDesc)))
    {
        DBG_OUT((_T("CDlgAddRingtone::OnMenuLeft() - Tried setting an invalid rule")));
        
        MessageBeep(MB_ICONHAND);
        //SafeCloseWindow(IDCANCEL); let's not allow them to close, they have to cancel
        return;
    }

    if(m_oStr->IsEmpty(m_sRule.ringToneInfo.szDisplayName) || m_oStr->IsEmpty(m_sRule.ringToneInfo.szPathName))
    {
        DBG_OUT((_T("CDlgAddRingtone::OnMenuLeft() - no ringtone set so setting ringtone to *none*")));
        m_oStr->StringCopy(m_sRule.ringToneInfo.szDisplayName, _T("None"));
        m_oStr->StringCopy(m_sRule.ringToneInfo.szPathName, _T("*none*"));
    }

    SafeCloseWindow(IDOK);

}

void CDlgAddRingtone::OnMenuRight()
{
	OnStop();
    SafeCloseWindow(IDCANCEL);
}

void CDlgAddRingtone::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, m_bEditMode?_T("Edit Ringtone"):_T("Add Ringtone"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAddRingtone::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(!m_wndMenu.IsWindowUp(FALSE)) 
        DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgAddRingtone::LaunchPopupMenu()
{
    m_wndMenu.ResetContent();
	OnStop();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumAddRingtone* eRing = (EnumAddRingtone*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX] = _T("");
    GetItemTitle(*eRing, szTitle);
    m_wndMenu.AddCategory(szTitle);
    int i;

    switch(*eRing)
    {
    case AR_Rule:
        if(m_sRule.eRuleType == RR_Default || m_bEditMode)
            return; // no popup if in edit mode or the rule is in default mode
        for(i=RR_Category; i<=RR_Contact; i++)
        {
            GetRuleText((EnumRingtoneRuleType)i, szInfo, TRUE);
            m_wndMenu.AddItem(szInfo, IDMENU_Popup, (m_sRule.eRuleType==(EnumRingtoneRuleType)i?FLAG_Radio:NULL), i);
        }
        break;
    case AR_RingType:
        for(i=RT_Ring; i<=RT_None; i++)
        {
            GetRintTypeText((EnumRingType)i, szInfo);
            m_wndMenu.AddItem(szInfo, IDMENU_Popup, (m_sRule.eRingType==(EnumRingType)i?FLAG_Radio:NULL), i);
        }
        break;
    case AR_RingTone:
        if(m_sFileList == NULL || m_iFileCount == 0)
            return; // nothing to show
        for(int i=0; i<m_iFileCount; i++)
        {
            SNDFILEINFO* psfInfo = &m_sFileList[i];
            if(psfInfo && 
               (psfInfo->sstType == SND_SOUNDTYPE_FILE || psfInfo->sstType == SND_SOUNDTYPE_NONE) && 
               !m_oStr->IsEmpty(psfInfo->szDisplayName) && 
               !m_oStr->IsEmpty(psfInfo->szPathName))
            {
                m_wndMenu.AddItem(psfInfo->szDisplayName, IDMENU_Popup, (0==m_oStr->Compare(m_sRule.ringToneInfo.szPathName,psfInfo->szPathName)?FLAG_Radio:NULL), i);
            }
        }
        break;
    default:
        return;
    }
    

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_DrawScrollArrows|OPTION_Bounce,
        rcClient.right/4,m_rcTitle.bottom,
        rcClient.right*3/4, HEIGHT(m_rcArea),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgAddRingtone::SetSelection()
{
	OnStop();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = m_wndMenu.GetSelectedItem();
    if(!sMenuItem || !sItem->lpItem)
        return;

    EnumAddRingtone* eRing = (EnumAddRingtone*)sItem->lpItem;

    switch(*eRing)
    {
    case AR_Rule:
        {
            EnumRingtoneRuleType eRule = (EnumRingtoneRuleType)sMenuItem->lParam;
            if(eRule == RR_Default || m_bEditMode)
                return; // no changing when when in edit mode or default setting
            else if(eRule == RR_Category)
            {
                CDlgChooseCategory dlg;
                dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
                if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
                {
                    m_sRule.eRuleType   = eRule;
                    m_oStr->StringCopy(m_sRule.szDesc, dlg.GetCategory());
                }
            }
            else if(eRule == RR_Contact)
            {
                CDlgChooseContact dlg(m_hWnd);
                dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
                if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
                {
                    CPoomContacts*  pOPoom = CPoomContacts::Instance();
                    if(pOPoom)
                    {
                        LONG lOID = dlg.GetSelectedOID();
                        IContact* pContact = pOPoom->GetContactFromOID(lOID);
                        if(pContact)
                        {
                            // if we have a ringtone set it in outlook too
                            /*if(!m_oStr->IsEmpty(m_sRule.ringToneInfo.szDisplayName) && !m_oStr->IsEmpty(m_sRule.ringToneInfo.szPathName))
                                pOPoom->SetContactRingtone(pContact, &m_sRule.ringToneInfo);*/

                            if(pOPoom->GetFormatedNameText(m_sRule.szDesc, pContact, TRUE))
                            {
                                m_sRule.eRuleType   = eRule;
                                m_sRule.iOID        = (int)lOID;
                            }
                            pContact->Release();
                        }
                    }
                }
            }
            RefreshList();
        }
        break;
    case AR_RingType:
        m_sRule.eRingType   = (EnumRingType)sMenuItem->lParam;
        break;
    case AR_RingTone:
        {
            int iIndex = (int)sMenuItem->lParam;
            if(iIndex < 0 || iIndex >= m_iFileCount)
                return;
            SNDFILEINFO* psfInfo = &m_sFileList[iIndex];
            if(!psfInfo)
                return;
            memcpy(&m_sRule.ringToneInfo, psfInfo, sizeof(SNDFILEINFO));

#ifdef USE_WMP
            //CObjMediaPlayer::GetPlayer().SetFile(m_sRule.ringToneInfo.szPathName);
#else
            HSOUND hSndFile;
            SndOpen(pInfo->szPathName, hSndFile);
#endif

            // if we are changing the default then set the sound profile in the system right now
            if(m_sRule.eRuleType == RR_Default)
            {
                //HRESULT hr = SndSetSound(SND_EVENT_RINGTONELINE1, psfInfo, FALSE);
            }
            else if(m_sRule.eRuleType == RR_Contact && m_sRule.iOID != 0)
            {
                // set it in outlook
                /*CPoomContacts*  pOPoom = CPoomContacts::Instance();
                if(pOPoom)
                {
                    IContact* pContact = pOPoom->GetContactFromOID(m_sRule.iOID);
                    if(pContact)
                    {
                        pOPoom->SetContactRingtone(pContact, &m_sRule.ringToneInfo);
                        pContact->Release();
                    }
                }*/
            }
        }
        break;
    default:
        return;
    }
}
