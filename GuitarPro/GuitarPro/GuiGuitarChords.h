#pragma once
#include "guibase.h"
#include "MyGenerator.h"

class CGuiGuitarChords : public CGuiBase
{
public:
    CGuiGuitarChords(void);
    ~CGuiGuitarChords(void);

    void                Init(HWND hWnd, HINSTANCE hInst);
    EnumGUI             GetGUI(){return GUI_Chords;};

    BOOL                OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL                OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnMouseMove(HWND, POINT& pt);
    BOOL                OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL                DrawText(CIssGDIEx& gdi, RECT& rc);
    BOOL                DrawGrid(CIssGDIEx& gdi, RECT& rc);
	BOOL				DrawBridge(CIssGDIEx& gdi, RECT& rc);
    BOOL                DrawStar(CIssGDIEx& gdi, RECT& rc);
    
    BOOL                OnChord(); //Chord Button 
    BOOL                OnRoot();
    BOOL                OnVariation();
	BOOL				OnTuning();
    BOOL                OnFavorite();

	UINT                GetNoteRes(int iString);//we'll us this one primarily
	UINT			    GetNoteRes(int iString, int iFret);

	void				PlayChord();

private:
    HPEN                m_hPenNormal;
    HPEN                m_hPenThick;
    
    RECT                m_rcSave; //????
    
    RECT                m_rcRoot;
    RECT                m_rcChord;
    RECT                m_rcVariation;
    RECT                m_rcLeftRight;
    RECT                m_rcGrid;
    RECT                m_rcFretNums[MAX_FRETS];

    RECT                m_rcFingers[6][MAX_FRETS];
    RECT                m_rcStringInidcators[6];

    RECT                m_rcFav;

    RECT                m_rcNotes[6];//rects of the actual notes for hit detection

    POINT               m_ptLastMove;//for mouse move

    
   
};
