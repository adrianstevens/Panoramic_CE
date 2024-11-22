#include "GuiBase.h"


CGuiBase::CGuiBase(void)
:m_hWnd(NULL)
,m_hInst(NULL)
{
    m_oStr = CIssString::Instance();
}

CGuiBase::~CGuiBase(void)
{

}

void CGuiBase::PlaySounds(TCHAR* szWave)
{
  /*  PlaySound(szWave, m_hInst, SND_RESOURCE | SND_SYNC | SND_NODEFAULT);
    DebugOut(_T("PlaySounds\r\n"));*/
}
