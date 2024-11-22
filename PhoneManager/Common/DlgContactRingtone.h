#pragma once

#include "DlgPerContactBase.h"
#include "PoomContacts.h"
#include "objringtoneengine.h"
#include "objringtonerulelist.h"
#include "ObjPhoneProfile.h"
#include <soundfile.h>

class CDlgContactRingtone:public CDlgPerContactBase
{
public:
    CDlgContactRingtone(void);
    ~CDlgContactRingtone(void);

protected:
    BOOL                OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    
private:    // functions
    void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                PopulateList();
    void                OnMenuLeft();
    void                OnMenuRight();
    void                DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);
    void                PlaySelected();
    void                StopPlaying();
    TypeRingtoneRule*   FindContactRingRule();


private:    // variables
    SNDFILEINFO         m_sCurrent;
    SNDFILEINFO*		m_sFileList;
    int                 m_iFileCount;

    CObjRingtoneRuleList m_objRuleList;
    CPoomContacts*      m_pPoom;
    int                 m_iLastSelected;
    BOOL                m_bPlaying;

};
