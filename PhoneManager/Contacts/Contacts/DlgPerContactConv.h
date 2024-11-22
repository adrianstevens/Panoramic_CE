#pragma once

#include "DlgPerContactBase.h"
#include "DlgContactDetails.h"
#include "SMSMessaging.h"
#include "GuiCallLog.h"

class CDlgPerContactConv:public CDlgPerContactBase
{
public:
    CDlgPerContactConv(BOOL bEnableDetails, CDlgContactDetails* dlg = NULL);
    
    ~CDlgPerContactConv(void);

	void SetContact(CObjHistoryItem* sItem, TypeOptions* sOptions, CIssGDIEx* gdiIconArray);
	void SetContact(TypeContact* sItem, TypeOptions* sOptions, CIssGDIEx* gdiIconArray);

    virtual BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);


    BOOL            NeedMoreHistoryItems(int iCount);
    BOOL            CheckFilters(CALLLOGENTRY& ce);
    BOOL            CheckCallDate(CALLLOGENTRY& ce);
    void            DestroyCallEntry(CALLLOGENTRY& ce);
    void            CollectCallItems(void);
    void            CollectSMSMessages(void);
    void            OnSelectSMS();
    void            OnSelectCall();


protected:
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            PopulateList();
    void            OnSelectPopupMenuItem(int iIndex, TypeItems* sItem);
    void            OnSelectMenuItem(int iIndex, TypeItems* sItem);
    void            OnMenu();
    void            DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);
    static void     DeleteMyItem(LPVOID lpItem);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL			DrawListItem(CIssGDIEx& gdi, TypeItems* sItem, BOOL bIsHighlighted, RECT& rc);

private: // functions
    static int		CompareItems( const void *arg1, const void *arg2 );

private: // variables
    BOOL                    m_bEnableDetails;
    CDlgContactDetails*     m_dlgDetails;
    TCHAR*                  m_szDefaultName;
    TCHAR*                  m_szDefaultNumber;
    CIssGDIEx*              m_gdiIconArray;
    TypeOptions*            m_sOptions;
	int						m_iLastListIndex;
    HANDLE	                m_hCallLog;			// Handle to the call log session
};
