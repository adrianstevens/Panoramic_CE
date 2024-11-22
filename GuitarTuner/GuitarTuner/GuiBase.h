#pragma once

#include "stdafx.h"
#include "ISSGDIEx.h"
#include "IssGDIDraw.h"
#include "IssString.h"
#include "IssImageSliced.h"
#include "IssCommon.h"
#include "Resource.h"
#include "IssSoundFX.h"

#define TEXT_COLOR RGB(255,255,255)
#define TEXT_SHADOW 0

#define REG_KEY		        _T("SOFTWARE\\Pano\\GuitarTuner")

enum EnumGUI
{
	GUI_Chords,
	GUI_Tuner,
	GUI_Count,
};

class CGuiBase
{
public:
    CGuiBase(void);
    ~CGuiBase(void);

    virtual void        Init(HWND hWnd, HINSTANCE hInst){m_hWnd = hWnd; m_hInst = hInst;};

    virtual BOOL	    OnPaint(HWND hWnd, HDC hDC, RECT& rcClient){return TRUE;};
    virtual BOOL	    OnLButtonDown(HWND hWnd, POINT& pt){return TRUE;};
    virtual BOOL	    OnLButtonUp(HWND hWnd, POINT& pt){return TRUE;};
    virtual BOOL        OnMouseMove(HWND hWnd, POINT& pt){return TRUE;};
    virtual BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    virtual BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    virtual BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

	EnumGUI				GetGUI(){return m_eGUI;};

protected:
    void                PlaySounds(TCHAR* szWave);
    BOOL                DrawBackground(CIssGDIEx& gdi, RECT& rc);

protected:
	CIssSoundFX			m_oSoundFX;

    HINSTANCE           m_hInst;
    HWND                m_hWnd;

    CIssString*			m_oStr;
    CIssGDIEx    		m_gdiMem;
    CIssGDIEx			m_gdiBackground;

	EnumGUI				m_eGUI;

    BOOL        m_bPlaySounds;
};
