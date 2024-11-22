#pragma once

#include "DlgPerContactBase.h"
#include "DlgContactDetails.h"
#include "SMSMessaging.h"

class CDlgPerContactSMS:public CDlgPerContactBase
{
public:
    CDlgPerContactSMS(BOOL bEnableDetails, CDlgContactDetails* dlg = NULL);
    ~CDlgPerContactSMS(void);

    void        SetSMSMessage(TypeSMSMessage* sSMS);


protected:
    void        DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void        PopulateList();
    void        OnSelectPopupMenuItem(int iIndex, TypeItems* sItem);
    void        OnSelectMenuItem(int iIndex, TypeItems* sItem);
    void        OnMenu();
    void        DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);
    static void         DeleteMyItem(LPVOID lpItem);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc);

private: // variables
    TypeSMSMessage          m_sSMS;
    BOOL                    m_bEnableDetails;
    CDlgContactDetails*     m_dlgDetails;
};
