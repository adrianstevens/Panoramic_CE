#include "IssImgFont.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"

TypeValue::TypeValue()
:iWidth(0)
,iIndex(0)
{}

TypeValue::~TypeValue()
{}

CIssImgFont::CIssImgFont(void)
:m_oStr(CIssString::Instance())
{
}

CIssImgFont::~CIssImgFont(void)
{
	Destroy();
}

BOOL CIssImgFont::Destroy()
{
	for(int i=0; i<m_arrValues.GetSize(); i++)
	{
		TypeValue* sItem = m_arrValues[i];
		if(sItem)
			delete sItem;
	}
	m_arrValues.RemoveAll();

	m_gdiFont.Destroy();
	return TRUE;
}

BOOL CIssImgFont::Initialize(UINT uiFont, HWND hWndParent, HINSTANCE hInst, int iHeight/* = -1*/)
{
	Destroy();

	CIssGDIEx gdiFont;

    if(FAILED(gdiFont.LoadImage(uiFont, hWndParent, hInst, TRUE)))
        return FALSE;

	if(gdiFont.GetDC() == NULL || 
	   gdiFont.GetWidth() == 0 || 
	   gdiFont.GetHeight() == 0 || 
       gdiFont.GetBits() == NULL)
		return FALSE;

    int iImageHeight = gdiFont.GetHeight();
	int iLenCount = 0;
	int iStart = 0;
	DWORD dwWidthInBytes	= gdiFont.GetBitmapInfo().bmWidthBytes;
	WORD* lpFont			= (WORD *)((LPBYTE)gdiFont.GetBits() + dwWidthInBytes*(gdiFont.GetHeight() - 1));
    BYTE* btAlpha           = gdiFont.GetAlpha();
	for(int i=0; i<gdiFont.GetWidth(); i++)
	{
        if((gdiFont.GetAlpha() && gdiFont.GetAlphaValue(i, 0) != 0 && lpFont[i] == 0) || (lpFont[i] == 0 && gdiFont.GetAlpha() == NULL)) // if it's Black (with full alpha) we keep adding the length
		{
			// this tells us we're starting a new letter
			if(iLenCount == 0)
				iStart = i;

			iLenCount++;
		}
		else if(iLenCount > 0)
		{
			// add the new length paramter
			TypeValue* sNewItem = new TypeValue;
			sNewItem->iWidth = iLenCount;
			sNewItem->iIndex = iStart;	
			m_arrValues.AddElement(sNewItem);
			iLenCount = 0;
		}
	}

	// for the last one
	if(iLenCount > 0)
	{
		// add the new length paramter
		TypeValue* sNewItem = new TypeValue;
		sNewItem->iWidth = iLenCount;
		sNewItem->iIndex = iStart;
		m_arrValues.AddElement(sNewItem);
	}

	m_gdiFont.Create(gdiFont.GetDC(), gdiFont.GetWidth(), gdiFont.GetHeight()-1, FALSE, TRUE);
    if(gdiFont.GetAlpha())
        m_gdiFont.InitAlpha(TRUE);

    ::Draw(m_gdiFont,
		   0,0,
		   m_gdiFont.GetWidth(), m_gdiFont.GetHeight(),
		   gdiFont,
		   0,1,
		   ALPHA_Copy);

	// if we don't have to scale at all
	if(iHeight == -1 || m_gdiFont.GetHeight() == iHeight)
		return TRUE;

	CIssGDIEx gdiTemp1;

	SIZE sizeText;
	sizeText.cy		= iHeight;
	sizeText.cx		= gdiFont.GetWidth()*iHeight/gdiFont.GetHeight();

	// scale the font to the proper height
	ScaleImage(m_gdiFont, gdiTemp1, sizeText, FALSE, 0);

	// recreate the font and now use the proper heght
	m_gdiFont.Create(gdiTemp1.GetDC(), gdiTemp1.GetWidth(), gdiTemp1.GetHeight(), FALSE, TRUE);

    if(gdiTemp1.GetAlpha())
        m_gdiFont.InitAlpha(TRUE);

    ::Draw(m_gdiFont,
        0,0,
        m_gdiFont.GetWidth(), m_gdiFont.GetHeight(),
        gdiTemp1,
        0,0,
        ALPHA_Copy);

	// go through and reset all the font width's
	double dbItem;
	for(int i=0; i<m_arrValues.GetSize(); i++)
	{
		TypeValue* sItem = m_arrValues[i];
		if(sItem)
		{
			dbItem			= (double)sItem->iWidth;
			dbItem			= dbItem*(double)iHeight/(double)iImageHeight;
			//dbItem			+= 0.5; // we round up one
			sItem->iWidth	= (int)dbItem;	


			dbItem			= (double)sItem->iIndex;
			dbItem			= dbItem*(double)iHeight/(double)iImageHeight;
			dbItem			+= 0.5;
			sItem->iIndex	= (int)dbItem;// we round up one
		}
	}

	return TRUE;
}

BOOL CIssImgFont::IsLoaded()
{
	return (m_gdiFont.GetDC()?TRUE:FALSE);
}

void CIssImgFont::DrawText(CIssGDIEx& gdiDest, TCHAR* szText, RECT& rc, UINT uiFormat, int iAlpha)
{
	if(!IsLoaded() || m_oStr->IsEmpty(szText) || gdiDest.GetBits() == NULL)
		return;

	int iFullLen = 0;
	int iChar;
	for(int i=0; i<m_oStr->GetLength(szText); i++)
	{
		iChar	= (int)szText[i] - 32;
		TypeValue* sItem = m_arrValues[iChar];
		if(!sItem)
			continue;
		iFullLen += sItem->iWidth;
	}

	// if we couldn't find any letter lengths
	if(iFullLen == 0)
		return;

	RECT rcDraw = rc;

	if(uiFormat == DT_LEFT)
	{
		rcDraw.right	= rcDraw.left + iFullLen;
		rcDraw.bottom	= rcDraw.top + m_gdiFont.GetHeight();
	}
	else if(uiFormat & DT_CENTER)
	{
		int iCenter		= rc.left + (rc.right-rc.left)/2;
		rcDraw.left		= iCenter-iFullLen/2;
		rcDraw.right	= iCenter+iFullLen/2;
		rcDraw.bottom	= rcDraw.top + m_gdiFont.GetHeight();
	}
	else if(uiFormat & DT_RIGHT)
	{
		rcDraw.left		= rcDraw.right - iFullLen;
		rcDraw.bottom	= rcDraw.top + m_gdiFont.GetHeight();
	}
	if(uiFormat & DT_VCENTER)
	{
		rcDraw.top		= rcDraw.top + (HEIGHT(rc) - m_gdiFont.GetHeight())/2;
		rcDraw.bottom	= rcDraw.top + m_gdiFont.GetHeight();
	}

	RECT rcLetter;
	int iLenDone = 0;
	for(int i=0; i<m_oStr->GetLength(szText); i++)
	{
		iChar	= (int)szText[i] - 32;
		TypeValue* sItem = m_arrValues[iChar];
		if(!sItem)
			continue;

		rcLetter		= rcDraw;
		rcLetter.left	= rcDraw.left + iLenDone;
		rcLetter.right	= rcLetter.left + sItem->iWidth;
		
		// if we're outside the bounds of draw
		if(rcLetter.right < rc.left)
			continue;

		if(rcLetter.left < rc.left)
			rcLetter.left = rc.left;

		iLenDone		+= WIDTH(rcLetter);

        ::Draw(gdiDest,
             rcLetter.left, rcLetter.top,
             WIDTH(rcLetter), HEIGHT(rcLetter),
             m_gdiFont,
             sItem->iIndex, 0,
             ALPHA_Normal,
             iAlpha);
	}
}
