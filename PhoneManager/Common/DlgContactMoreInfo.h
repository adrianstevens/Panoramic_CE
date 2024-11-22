#pragma once

#include "DlgPerContactBase.h"
#include "PoomContacts.h"

class CDlgContactMoreInfo:public CDlgPerContactBase
{
public:
    CDlgContactMoreInfo(void);
    ~CDlgContactMoreInfo(void);
  
private:    // functions
    void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                PopulateList();
    void                DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);
    static void         DeleteMyItem(LPVOID lpItem);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumContactInfo* eInfo);
    void                Destroy();
    void                SetMenuItemHeight(int iTextHeight);
	void				OnSelectMenuItem(int iIndex, TypeItems* sItem);
	BOOL				SendTextToClipboard(TCHAR* szSource, HWND hWnd);

    int                 GetLinesOfText(TCHAR* szSource, HFONT hFont, RECT rc);

    void                CleanNotesBuffer(TCHAR* szNotes);

private:    // variables

    TCHAR*              m_szTitle;
    TCHAR*              m_szBirthday;
    TCHAR*              m_szDepartment;
    TCHAR*              m_szCategory;
    TCHAR*              m_szWebpage;
    TCHAR*              m_szHomeAddress;
    TCHAR*              m_szBusinessAddress;
	TCHAR*				m_szNotes;

    HFONT               m_hFontSmall;
};
