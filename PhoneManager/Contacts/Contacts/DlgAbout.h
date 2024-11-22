#pragma once

#include "DlgBase.h"

class CDlgAbout:public CDlgBase
{
public:
    CDlgAbout(void);
    ~CDlgAbout(void);

protected:
    void    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void    OnMenuLeft();
    void    OnMenuRight();
};
