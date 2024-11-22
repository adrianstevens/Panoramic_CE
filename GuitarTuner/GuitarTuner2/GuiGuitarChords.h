#pragma once
#include "guibase.h"

#include "MyGenerator.h"

class CGuiGuitarChords : public CGuiBase
{
public:
    CGuiGuitarChords(void);
    ~CGuiGuitarChords(void);

    void                Init(HWND hWnd, HINSTANCE hInst);

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL				IsLeft(){return m_oChords.IsLeftHanded();};
	BOOL				ShowFretNum(){return m_oChords.NumberFrets();};
	void				SetLeft(BOOL bLeft){m_oChords.SetLeftHanded(bLeft);};
	void				SetShowFretNum(BOOL bShowFretNum){m_oChords.SetNumberFrets(bShowFretNum);};

private:
    BOOL                DrawText(CIssGDIEx& gdi, RECT& rc);
    BOOL                DrawGrid(CIssGDIEx& gdi, RECT& rc);
	BOOL				DrawBridge(CIssGDIEx& gdi, RECT& rc);
    
    BOOL                OnChord(); //Chord Button .... could make em all buttons ... hmmm
    BOOL                OnRoot();
    BOOL                OnVariation();
	BOOL				OnTuning();
    //BOOL                OnLeftRight();

    BOOL                LoadImages(HWND hWnd, HINSTANCE hInst);

	void				LoadRegistry();
	void				SaveRegistry();

	UINT                GetNoteRes(int iString);//we'll us this one primarily
	UINT				GetNoteRes(int iString, int iFret);

	void				PlayChord();
	void				LoadSounds();


private:
	CMyGenerator		m_oChords;

	CIssImageSliced     m_imgBlue;
    CIssImageSliced     m_imgRed;
    CIssImageSliced     m_imgGrey;

	CIssGDIEx			m_gdiFinger; //our sexy circles

    HFONT               m_hFontSmall;
    HFONT               m_hFontNormal;
    HFONT               m_hFontLarge;

    HPEN                m_hPenNormal;
    HPEN                m_hPenThick;
    
    
    RECT                m_rcSave; //????
    
    RECT                m_rcRoot;
    RECT                m_rcChord;
    RECT                m_rcVariation;
    RECT                m_rcLeftRight;
    RECT                m_rcPlayType[6];//text above grid
    RECT                m_rcGrid;
    RECT                m_rcFretNums[5];
    RECT                m_rcNote[6];//notes below grid

    RECT                m_rcFingers[6][5];//these of course need to be square
   
};
