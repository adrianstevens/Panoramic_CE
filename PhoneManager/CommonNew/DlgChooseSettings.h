#pragma once

#include "DlgChoosePrograms.h"

class CDlgChooseSettings : public CDlgChoosePrograms
{
public:
	CDlgChooseSettings(CIssVector<TypeLaunchItem>* arrExcludeItems = 0);
	~CDlgChooseSettings(void);

    static void     DestroyAllContent();
    
protected:
    BOOL            OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:		// functions
	void			PopulateList();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, BOOL bIsHighlighted, TypeLaunchItem* sItem);
    HRESULT         SearchCPLItems();
    void            MyCheckProgramsList();

private:		// variables
    static HIMAGELIST   m_hImageListCPL;
};
