#pragma once
#include "wndinfoedit.h"

#define MAX_STORAGECARDS 5

class CWndStorage : public CWndInfoEdit
{
public:
	CWndStorage(void);
	~CWndStorage(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();

private:
    int         FindStorageCards();

private:
    //shouldn't ever exceed 5
    TCHAR*			m_szStorageCards[MAX_STORAGECARDS];
};
