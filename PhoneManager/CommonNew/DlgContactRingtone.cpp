#include "DlgContactRingtone.h"
#include "resource.h"
#include "ObjSkinEngine.h"
#include "ContactsGuiDefines.h"
#include "ObjMediaPlayer.h"
#include "CommonDefines.h"

#define IDMENU_Items    5000
#define IDMENU_None     5001
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*5/7)

CDlgContactRingtone::CDlgContactRingtone(void)
:m_sFileList(NULL)
,m_iFileCount(0)
,m_pPoom(CPoomContacts::Instance())
,m_iLastSelected(-1)
,m_bPlaying(FALSE)
{
    ZeroMemory(&m_sCurrent, sizeof(SNDFILEINFO));
}

CDlgContactRingtone::~CDlgContactRingtone(void)
{
    if(m_sFileList)
        LocalFree(m_sFileList);

    CObjMediaPlayer::Destroy();
}

BOOL CDlgContactRingtone::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    // get the full list of ringtones
    SndGetSoundFileList(SND_EVENT_RINGTONELINE1 ,SND_LOCATION_ALL, &m_sFileList, &m_iFileCount);

    // initialize the media player
    CObjMediaPlayer::GetPlayer();

    if(IsFileExists(TXT_FileRingtonRulelist))
        m_objRuleList.ReloadList();

    // find the contact ringtone from poom
    IContact* pContact = m_pPoom->GetContactFromIndex(m_iContactIndex);
    if(pContact)
    {
        m_pPoom->GetContactRingtone(pContact, &m_sCurrent);
        pContact->Release();

        // we don't accept the none ringtone
        if(0 == m_oStr->Compare(m_sCurrent.szPathName, _T("*none*")))
            ZeroMemory(&m_sCurrent, sizeof(SNDFILEINFO));
    }

    CDlgPerContactBase::OnInitDialog(hWnd, wParam, lParam);

    ::SetCursor(hCursor);
    return TRUE;
}


void CDlgContactRingtone::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Set Contact Ringtone"), rc, DT_LEFT | DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(51,51,51));

}

void CDlgContactRingtone::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgContactRingtone::OnMenuLeft()
{
    StopPlaying();

	if(!m_oMenu)
		return;

    int iCurrentIndex = m_oMenu->GetSelectedItemIndex();
    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(iCurrentIndex == -1 || !sItem)
    {
        CloseWindow(IDOK);
        return;
    }

    if(IsFileExists(TXT_FileRingtonRulelist))
    {
        TypeRingtoneRule* sRule = FindContactRingRule();
        if(sRule)
            m_objRuleList.DeleteItem(sRule);

        if(iCurrentIndex != 0)
        {
            SNDFILEINFO* psfInfo = &m_sFileList[(int)sItem->lParam];
            TypeRingtoneRule* sNewRule = new TypeRingtoneRule;
            if(sNewRule && psfInfo)
            {
                ZeroMemory(sNewRule, sizeof(TypeRingtoneRule));
                sNewRule->eRingType = RT_VibrateAndRing;
                sNewRule->eRuleType = RR_Contact;
                sNewRule->iOID      = m_sContact.lOid;
                memcpy(&sNewRule->ringToneInfo, psfInfo, sizeof(SNDFILEINFO));

                IContact* pContact = m_pPoom->GetContactFromIndex(m_iContactIndex);
                if(pContact)
                {
                    m_pPoom->GetFormatedNameText(sNewRule->szDesc, pContact, TRUE);
                    pContact->Release();
                }

                if(!m_objRuleList.AddItem(sNewRule))
                    delete sNewRule;
            }
        }
    }
    else
    {
        // just do it in POOM manually
        IContact* pContact = m_pPoom->GetContactFromIndex(m_iContactIndex);
        if(pContact)
        {
            if(iCurrentIndex == 0)
                m_pPoom->RemoveContactRingtone(pContact);
            else
            {
                SNDFILEINFO* psfInfo = &m_sFileList[(int)sItem->lParam];
                m_pPoom->SetContactRingtone(pContact, psfInfo);
            }
            pContact->Release();
        }
    }

    CloseWindow(IDOK);
}

void CDlgContactRingtone::OnMenuRight()
{
    StopPlaying();
    CloseWindow(IDCANCEL);
}

void CDlgContactRingtone::PopulateList()
{
	if(!m_oMenu)
		return;

    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_Bounce);
    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu->PreloadImages(m_hWnd, m_hInst);
    m_oMenu->ResetContent();

    int iCurrentSelection = 0;

    m_oMenu->AddItem(_T("- No Ringtone -"), IDMENU_None);

    int iAdded = 0;
    for(int i=0; i<m_iFileCount; i++)
    {
        SNDFILEINFO* psfInfo = &m_sFileList[i];
        if(psfInfo && 
          (psfInfo->sstType == SND_SOUNDTYPE_FILE || psfInfo->sstType == SND_SOUNDTYPE_NONE) && 
          !m_oStr->IsEmpty(psfInfo->szDisplayName) && 
          !m_oStr->IsEmpty(psfInfo->szPathName) &&
          0 != m_oStr->Compare(psfInfo->szPathName, _T("*none*")))
        {
            m_oMenu->AddItem(psfInfo->szDisplayName, IDMENU_Items, NULL, i);
            iAdded++;

            if(0 == m_oStr->Compare(m_sCurrent.szPathName, psfInfo->szPathName))
                iCurrentSelection = iAdded;
        }
    }

    m_oMenu->SetSelectedItemIndex(iCurrentSelection, TRUE);   
}

void CDlgContactRingtone::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
{
    RECT rc = rcDraw;
    rc.bottom = rc.top + HEIGHT(rcDraw)/2;
    DrawTextShadow(gdi, _T("Current ringtone:"), rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.top  = rc.bottom;
    rc.bottom = rcDraw.bottom;

    if(!m_oStr->IsEmpty(m_sCurrent.szDisplayName) && !m_oStr->IsEmpty(m_sCurrent.szPathName))
        DrawTextShadow(gdi, m_sCurrent.szDisplayName, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    else
        DrawTextShadow(gdi, _T("- No Ringtone -"), rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));


}

void CDlgContactRingtone::PlaySelected()
{
	if(!m_oMenu)
		return;

    int iCurrentIndex = m_oMenu->GetSelectedItemIndex();
    if(m_iLastSelected == iCurrentIndex && m_bPlaying)
    {
        StopPlaying();
        return; // don't do anything
    }

    StopPlaying();    

    m_iLastSelected = iCurrentIndex;

    if(iCurrentIndex <= 0)
        return;

    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || sItem->lParam < 0 || sItem->lParam >= m_iFileCount)
        return;

    // now play the new ringtone
    SNDFILEINFO* psfInfo = &m_sFileList[(int)sItem->lParam];

    if(!psfInfo || m_oStr->IsEmpty(psfInfo->szPathName))
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

    m_bPlaying = TRUE;

    CObjMediaPlayer::GetPlayer().SetFile(psfInfo->szPathName);
    CObjMediaPlayer::GetPlayer().Play();
}

void CDlgContactRingtone::StopPlaying()
{
    m_bPlaying = FALSE;
    CObjMediaPlayer::GetPlayer().Stop();
}

BOOL CDlgContactRingtone::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDMENU_Items:
        PlaySelected();
        break;
    case IDMENU_None:
        StopPlaying();
        break;
    default:
        return CDlgPerContactBase::OnCommand(hWnd, wParam, lParam);
    }
    return TRUE;
}

TypeRingtoneRule* CDlgContactRingtone::FindContactRingRule()
{
    for(int i=0; i<m_objRuleList.GetListCount(); i++)
    {
        TypeRingtoneRule* sRule = m_objRuleList.GetItem(i);
        if(!sRule)
            continue;
        
        if(sRule->eRuleType == RR_Contact && sRule->iOID == m_sContact.lOid)
            return sRule;
    }
    return NULL;
}

