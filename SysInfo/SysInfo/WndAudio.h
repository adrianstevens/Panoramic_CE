#pragma once
#include "wndinfoedit.h"
#include "IssSound.h"

class CWndAudio : public CWndInfoEdit
{
public:
    CWndAudio(void);
    ~CWndAudio(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();

private:
    CIssSound   m_oSound;
};
