#pragma once

#include "IssGDIEx.h"
#include "IssImageSliced.h"
#include "PoomContacts.h"

#define IDT_LoadPic     1000
#define WM_DrawContactPic WM_USER + 300

struct TypePicLoad
{
	long		lOid;
	CIssGDIEx*	gdiImage;
	TypePicLoad();
	~TypePicLoad();
	void Destroy();
};

class CObjContactImgLoader
{
public:
    CObjContactImgLoader(void);
    ~CObjContactImgLoader(void);

    void        Destroy();
    void        Initialize(int iWidth, int iHeight, HWND hWndParent, HINSTANCE hInst);
    void        SetContactOid(LONG lOid);
    void        DrawContactPic(CIssGDIEx& gdi, long lOid, int iX, int iY);
    HRESULT     FadeIn(long lOid, HDC hDC, int iX, int iY, DWORD dwTime);

private:    // functions
    static DWORD ThreadLoadImage(LPVOID lpVoid);
    DWORD       LoadContactImage();
    BOOL        ShouldAbort(HWND hWnd);


private:
    LONG            m_lDesiredOid;
    CPoomContacts*  m_pPoom;
	TypePicLoad		m_sPic;					// our image to load
    
    int             m_iHeight;
    int             m_iWidth;
    HWND            m_hWndParent;
    DWORD           m_dwLastTick;

    CIssImageSliced  m_imgMask;              // picture's mask
    CIssImageSliced  m_imgBorder;            // picture's border

	// thread stuff
    HANDLE           m_hEvent;
    CRITICAL_SECTION m_cr;
    HANDLE			 m_hThread;
	BOOL			 m_bKillThread;
};
