#include "StdAfx.h"
#include "DlgWifiHero.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "resource.h"
#include "IssXmlBuilder.h"
#include "IssXmlParser.h"
#include "IssCommon.h"
#include "Cfgmgrapi.h"
#include "DlgHelp.h"
#include "DlgAddPassword.h"
#include "IssRegistry.h"

#define REG_KEY			_T("SOFTWARE\\Pano\\WiFiHero")

#define TXT_APP_TITLE                       _T("WiFi Hero 1.3")
#define TXT_DEFAULT_MESSAGE                 _T("Press \"Update\" to restore settings")

#define IDT_TIMER                       161
#define TXT_DISPLAY_TIME                5000 

#define FILE_XmlSaved                   _T("wifi-saved.xml")
#define FILE_XmlDevice                  _T("wifi-device.xml")
#define FILE_XmlDiff                    _T("wifi-diff.xml")
#define FILE_XmlRestore                 _T("wifi-restore.xml")

#define COLOR_TEXT_NORMAL               (COLORREF)0xFFFFFF
#define COLOR_TEXT_SELECTED             COLOR_TEXT_NORMAL//(COLORREF)0x77AF39
#define HEIGHT_Text                     (GetSystemMetrics(SM_CXICON)*4/9)
#define BUTTON_Height                   (GetSystemMetrics(SM_CXICON))
#define BUTTON_HeightSelected           (GetSystemMetrics(SM_CXICON)*3/2)
#define IDMENU_List                     5000
#define IDMENU_MoveTop                  6000
#define IDMENU_MoveUp                   6001
#define IDMENU_MoveDown                 6002
#define IDMENU_MoveBottom               6003
#define IDMENU_DeleteProfile            6004
#define IDMENU_SaveProfile              6005
#define IDMENU_RefreshList              6006
#define IDMENU_Exit                     6007
#define IDMENU_About                    6008
#define IDMENU_Help                     6009
#define IDMENU_DeleteSelected           6010
#define IDMENU_RestoreSelected          6011
#define IDMENU_ResetPasswords           6012
#define IDMENU_ShowAllProfiles          6013
#define WM_RefreshItems                 WM_USER+200

LPCWSTR g_wszFavoriteXml = 
L"<wap-provisioningdoc>"
L"   <characteristic-query type=\"Wi-Fi\" recursive=\"true\"/>"
L"</wap-provisioningdoc>";

LPCWSTR g_wszDeleteFaked = 
L"<wap-provisiongdoc>"
L"<characteristic type=\"Wi-Fi\">"
L"<characteristic type=\"access-point\">"
L"<nocharacteristic type=\"Faked\"/>"
L"</characteristic>"
L"</characteristic>"
L"</wap-provisioningdoc>";


LPCWSTR g_wszAddFraud = 
L"<wap-provisioningdoc>"
L"<characteristic type=\"Wi-Fi\">"
L"<characteristic type=\"access-point\">"
L"<characteristic type=\"Fraud%i\"/>"
L"</characteristic>"
L"</characteristic>"
L"</wap-provisioningdoc>";

LPCWSTR g_wszDeleteAll = 
L"<wap-provisioningdoc>"
L"  <nocharacteristic type=\"Wi-Fi\"/>"
L"</wap-provisioningdoc>";


CDlgWifiHero::CDlgWifiHero(void)
:m_bShowAllProfiles(TRUE)
{
    m_hFontBtnLabel = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_oStr->StringCopy(m_szTitle, TXT_APP_TITLE);

    LoadRegistry();
}

CDlgWifiHero::~CDlgWifiHero(void)
{
    SaveRegistry();

    CIssGDIEx::DeleteFont(m_hFontBtnLabel);

    DeletePassArray();

	m_oList.ResetContent();
	m_wndMenu.ResetContent();

	CIssKineticList::DeleteAllContent();
	CIssString::DeleteInstance();
}

void CDlgWifiHero::DeletePassArray()
{
    PasswordType* sPass = NULL;
    for(int i = 0; i < m_arrPasswords.GetSize(); i++)
    {
        sPass = m_arrPasswords[i];
        if(sPass)
            delete sPass;
    }
    m_arrPasswords.RemoveAll();
}

void CDlgWifiHero::LoadRegistry()
{
    DWORD dwTemp = 0;
    DWORD dwSize = sizeof(PasswordType);
    PasswordType* sPass;

    GetKey(REG_KEY, _T("Count"), dwTemp);

    TCHAR szTemp[STRING_LARGE];

    for(int i = 0; i < (int)dwTemp; i++)
    {
        m_oStr->Format(szTemp, _T("%i"), i);
        sPass = new PasswordType;    

        GetKey(REG_KEY, szTemp, (LPBYTE)sPass, dwSize);
    
        m_arrPasswords.AddElement(sPass);
        sPass = NULL;
    }

    if(S_OK == GetKey(REG_KEY, _T("ShowAll"), dwTemp))
        m_bShowAllProfiles = (BOOL)dwTemp;
}

void CDlgWifiHero::SaveRegistry()
{
    DWORD dwTemp = m_arrPasswords.GetSize();

    //Save the number of passwords ... should be encrypted
    SetKey(REG_KEY, _T("Count"), dwTemp);
    SetKey(REG_KEY, _T("ShowAll"), m_bShowAllProfiles);

    TCHAR szTemp[STRING_LARGE];
    PasswordType* sPass;
    dwTemp = sizeof(PasswordType);

    for(int i = 0; i < m_arrPasswords.GetSize(); i++)
    {
        sPass = m_arrPasswords[i];
        if(sPass == NULL)
            continue;
        m_oStr->Format(szTemp, _T("%i"), i);
        SetKey(REG_KEY, szTemp, (LPBYTE)sPass, dwTemp);
    }
}

BOOL CDlgWifiHero::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;
	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= m_hWnd;
	mbi.nToolBarId	= IDR_MENU_MenuUpdate; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

    m_oList.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oList.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_oList.PreloadImages(hWnd, m_hInst);
    m_oList.SetCustomDrawFunc(DrawButtonItem, this);
    m_oList.SetDeleteItemFunc(DeleteMenuItem);

    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_wndMenu.SetSelected(IDR_PNG_Selector);
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.PreloadImages(hWnd, m_hInst);

    UpdateTitle(NULL);

    m_bFirstTime = TRUE;

/*    CDlgAddPassword dlgPass;
    dlgPass.SetName(_T("ISSINC"), TRUE);
    dlgPass.DoModal(hWnd, m_hInst, IDD_DLG_Basic);*/
    
	return TRUE;
}

BOOL CDlgWifiHero::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || m_gdiMem.GetWidth() != WIDTH(rcClient) || m_gdiMem.GetHeight() != HEIGHT(rcClient))
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    DrawBackground(m_gdiMem, rcClient, rcClip);

    if(m_bFirstTime)
    {
        m_oStr->StringCopy(m_szTitle, _T("Loading WiFi settings"));
        DrawText(m_gdiMem, rcClip);
        
        BitBlt(hDC,
            rcClip.left,rcClip.top,
            WIDTH(rcClip), HEIGHT(rcClip),
            m_gdiMem.GetDC(),
            rcClip.left,rcClip.top,
            SRCCOPY);
        
        PostMessage(m_hWnd, WM_RefreshItems, 0,0);
        m_bFirstTime = FALSE;
    }
    else if(m_oList.GetItemCount() != 0)
    {
        m_oList.OnDraw(m_gdiMem, rcClient, rcClip);
        DrawText(m_gdiMem, rcClip);
    }
    else
    {
        DrawText(m_gdiMem, rcClip);
    }

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
	
	return TRUE;
}

BOOL CDlgWifiHero::DrawText(CIssGDIEx& m_gdiMem, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcTitle) == FALSE)
        return TRUE;

    DrawTextShadow(m_gdiMem, m_szTitle, m_rcTitle, DT_CENTER|DT_VCENTER, m_hFontBtnLabel, RGB(255,255,255), 0);
    
    return TRUE;
}

BOOL CDlgWifiHero::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

    if(GetSystemMetrics(SM_CXSCREEN) != m_gdiMem.GetWidth())
        m_gdiMem.Destroy();

    SetRect(&m_rcTitle, rc.left, rc.top, rc.right, rc.top + GetSystemMetrics(SM_CXICON));
    SetRect(&m_rcList, rc.left, m_rcTitle.bottom, rc.right, rc.bottom);

    m_oList.OnSize(m_rcList.left,m_rcList.top, WIDTH(m_rcList), HEIGHT(m_rcList));


	return TRUE;
}

BOOL CDlgWifiHero::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{
		::SetWindowText(GetWnd(), _T("Wifi Hero"));
	}

	return UNHANDLED;
}

BOOL CDlgWifiHero::OnLButtonDown(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oList.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiHero::OnLButtonUp(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oList.OnLButtonUp(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiHero::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oList.OnMouseMove(pt))
    {}

	return TRUE;
}

BOOL CDlgWifiHero::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oList.OnKeyDown(wParam, lParam);
}

BOOL CDlgWifiHero::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgWifiHero::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDMENU_List)
    {
        int iIndex = m_oList.GetSelectedItemIndex();
        LaunchWifiItem(iIndex);
        return TRUE;
    }

	switch(LOWORD(wParam))
	{
	case IDMENU_Menu:
        OnMenu();
        break;
	case IDOK:
    case IDMENU_Exit:
		PostQuitMessage(0);
		break;
    case IDMENU_Update:
        OnUpdate();
        break;
    case IDMENU_ShowAllProfiles:
        m_bShowAllProfiles = !m_bShowAllProfiles;
        RefreshValues();
        break;
    case IDMENU_RefreshList:
        RefreshValues();
        break;
    case IDMENU_SaveProfile:
        OnSaveProfile();
        break;
    case IDMENU_DeleteProfile:
        OnDeleteProfile();
        break;
    case IDMENU_About:
        MessageBox(m_hWnd, _T("WiFi Hero 1.2.1\r\n\r\nPanoramic Software Inc\r\nwww.panoramicsoft.com\r\nCopyright 2009"), _T("About WiFi Hero"), MB_OK);
        break;
    case IDMENU_Help:
      //  MessageBox(m_hWnd, _T("WiFi Hero will automatically load and save all WiFi settings on your device.  If WiFi Hero finds that any WiFi access points have been deleted they will be displayed.  You can restore or delete saved access points individually or press \"Update\" on the menu to restore all.  WiFi Hero will automatically save new access points when its launched"), _T("WiFi Hero Help"), MB_OK);
        {
            CDlgHelp dlgHelp;
            dlgHelp.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        }
        break;
    case IDMENU_DeleteSelected:
        DeleteSelectedEntry();
        break;
    case IDMENU_RestoreSelected:
        RestoreSelectedEntry();
        break;
    case IDMENU_ResetPasswords:
        if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to delete all saved passwords?"), _T("WiFi Hero"), MB_YESNO | MB_ICONEXCLAMATION))
        {
            DeletePassArray();
            UpdateTitle(_T("All passwords deleted"));
        }
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgWifiHero::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case _T('a'):
    case _T('A'):
        //add a fake access-point
        {   
            UpdateTitle(_T("Adding fake access to point"));
            TCHAR szTemp[STRING_MAX];
            m_oStr->Format(szTemp, (TCHAR*)g_wszAddFraud, rand()%100);
            TCHAR* szOutput = NULL;
            DMProcessConfigXML(szTemp, CFGFLAG_PROCESS, &szOutput);
            //m_oStr->Delete(&szOutput);
			if(szOutput)
				delete [] szOutput;
            UpdateWindow(m_hWnd);
            RefreshValues();
        }
        break;
    default:
        return UNHANDLED;
        break;

	}

	return TRUE;
}

BOOL CDlgWifiHero::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_RefreshItems)
        RefreshValues();

	return UNHANDLED;
}

BOOL CDlgWifiHero::IsFileExists(TCHAR* szPath)
{
    return (UINT)GetFileAttributes(szPath) != (UINT)-1;
}

void CDlgWifiHero::RefreshValues(BOOL bUpdateMsg /* = TRUE */)
{
    if(bUpdateMsg)
        UpdateTitle(_T("Loading WiFi settings"));

    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    TCHAR szError[STRING_MAX] = _T("");
    BOOL bReturn = FALSE;

    m_oList.ResetContent();
    m_oList.SetDeleteItemFunc(DeleteMenuItem);
    m_oList.SetCustomDrawFunc(DrawButtonItem, this);

    CIssVector<TypeWifi> arrSaved, arrDevice, arrDiff;
    TCHAR szFileName[STRING_MAX];

    // wifi has to be off
    /*if(m_oWireless.IsWifiPowerOn())
        m_oWireless.TurnWifiPowerOff();*/

    //////////////////////////////////////////////////////////////////////////
    // read the saved info
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlSaved);
    ReadFileValues(szFileName, arrSaved); // don't care about result

    //////////////////////////////////////////////////////////////////////////
    // read the device info
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlDevice);
    DeleteFile(szFileName);

    TCHAR* szOutput = NULL;
    HRESULT hr;

    // Process the XML.
    hr = DMProcessConfigXML(g_wszFavoriteXml, CFGFLAG_PROCESS, &szOutput);
    if(hr != S_OK || m_oStr->IsEmpty(szOutput))
    {
        m_oStr->StringCopy(szError, _T("Unable to read device Wifi information"));
        UpdateTitle(szError);
        goto Error;
    }
        
    if(!SaveFileInfo(szFileName, szOutput))
    {
        m_oStr->StringCopy(szError, _T("Unable to save device Wifi information"));
        UpdateTitle(szError);
        goto Error;
    }

    //m_oStr->Delete(&szOutput);
    if(!ReadFileValues(szFileName, arrDevice))
    {
        m_oStr->StringCopy(szError, _T("Unable to process device Wifi information"));
        UpdateTitle(szError);
        goto Error;
    }

    //////////////////////////////////////////////////////////////////////////
    // Synchronize the two lists
    SynchronizeWifiProfiles(arrDevice, arrSaved, arrDiff);

    //////////////////////////////////////////////////////////////////////////
    // save the new data and order in an XML file

    //save the difference ... just makes things easy
    if(arrDiff.GetSize() > 0)
    {
        GetExeDirectory(szFileName);
        m_oStr->Concatenate(szFileName, FILE_XmlDiff);
        SaveFileValues(szFileName, arrDiff);
    }

    //save the new master list ... arrDevice now has the new values appended so its our new saved
    if(arrDevice.GetSize() > 0)
    {
        GetExeDirectory(szFileName);
        m_oStr->Concatenate(szFileName, FILE_XmlSaved);
        SaveFileValues(szFileName, arrDevice);
    }

    //////////////////////////////////////////////////////////////////////////
    // Populate the list menu
    if(m_bShowAllProfiles)
    {
        m_oList.AddCategory(_T("Saved Profiles"));
        for(int i=0; i<arrDevice.GetSize(); i++)
        {
            TypeWifi* sWifi = arrDevice[i];
            if(!sWifi)
                continue;
            if(sWifi->bOnDevice == FALSE)
                m_oList.AddItem((LPVOID)sWifi, IDMENU_List);
        }
        if(m_oList.GetItemCount() == 1)
            m_oList.AddSeparator();

        m_oList.AddCategory(_T("On-Device Profiles"));
        for(int i=0; i<arrDevice.GetSize(); i++)
        {
            TypeWifi* sWifi = arrDevice[i];
            if(!sWifi)
                continue;
            if(sWifi->bOnDevice == TRUE)
                m_oList.AddItem((LPVOID)sWifi, IDMENU_List);
        }
        arrDevice.RemoveAll();
    }
    else
    {
        // just add the diff list
        for(int i=0; i<arrDiff.GetSize(); i++)
        {
            TypeWifi* sWifi = arrDiff[i];
            if(!sWifi)
                continue;

            m_oList.AddItem((LPVOID)sWifi, IDMENU_List);
        }
        // no need to delete because the kinetic list now owns all the data
        arrDiff.RemoveAll();
    }    

    // we only want a scrollbar if there are more then twenty profiles
 //   if(m_oList.GetItemCount() > 20)
        m_oList.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_DrawScrollBar|OPTION_DrawScrollArrows|OPTION_Bounce, TRUE);
  //  else
  //      m_oList.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList, TRUE);

    m_oList.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oList.SetSelectedItemIndex(0, TRUE);

    bReturn = TRUE;

Error:

	if(szOutput)
		delete [] szOutput;

    // delete the vectors right now
    DeleteProfile(arrDiff);
    DeleteProfile(arrSaved);
    DeleteProfile(arrDevice);

    // restore cursor
    SetCursor(hCurs);

    //check for passwords ... as the function states
    CheckForPasswords();

    InvalidateRect(m_hWnd, NULL, FALSE);

    if(!bReturn)
    {   //messages set in the function
       // MessageBox(m_hWnd, szError, _T("Error"), MB_OK);
    }
}

void CDlgWifiHero::DeleteProfile(CIssVector<TypeWifi>& arrItems)
{
    for(int i=0; i<arrItems.GetSize(); i++)
    {
        TypeWifi* sItem = arrItems[i];
        delete sItem;
    }
    arrItems.RemoveAll();
}

void CDlgWifiHero::SynchronizeWifiProfiles(CIssVector<TypeWifi>& arrDevice, CIssVector<TypeWifi>& arrSaved, CIssVector<TypeWifi>& arrDiff)
{
    BOOL bFound = FALSE;
    int i, j;

    int iOriginalSize = arrDevice.GetSize();

    // first set all the items in arrDevice bool to bOnDevice = TRUE
    for(i=0; i<arrDevice.GetSize(); i++)
    {
        TypeWifi* sDevice = arrDevice[i];
        if(!sDevice)
            continue;
        sDevice->bOnDevice = TRUE;
    }

    for(i=0; i<arrSaved.GetSize(); i++)
    {
        bFound = FALSE;
        TypeWifi* sSaved = arrSaved[i];
        if(!sSaved)
            continue;

        for(j=0; j<iOriginalSize; j++)
        {
            TypeWifi* sDevice = arrDevice[j];
            if(!sDevice)
                continue;

            // see if we have a match
            if(0 == m_oStr->Compare(sDevice->szName, sSaved->szName) && sDevice->bAccessPoint == sSaved->bAccessPoint)
            {
                bFound = TRUE;
                break;
            }
        }

        // if we didn't find it in the list then add it to end of the device list
        if(!bFound)
        {
            TypeWifi* sNew = new TypeWifi;
            sNew->Clone(sSaved);
            sNew->bOnDevice = FALSE;
            arrDiff.AddElement(sNew);

            sNew = NULL;
            sNew = new TypeWifi;
            sNew->Clone(sSaved);
            sNew->bOnDevice = FALSE;    // we're adding a saved one to 
            arrDevice.AddElement(sNew);
        }
    }
}

BOOL CDlgWifiHero::ReadFileValues(TCHAR* szFileName, CIssVector<TypeWifi>& arrItems)
{
    if(!IsFileExists(szFileName))
      return FALSE;

    CIssXmlParser oXml;
    TypeWifi* sWifi = NULL;
    TCHAR szTemp[STRING_MAX];
    TCHAR szValue[STRING_MAX];
    HRESULT hr = S_OK;
    BOOL bReturn = FALSE;

    if(!oXml.Open(szFileName, TRUE))
        return FALSE;

    BOOL bAccessPoint;
    CComPtr<IXMLDOMNode> pDoc          = oXml.GetCurrentNode();
    CComPtr<IXMLDOMNode> pWap = NULL;
    CComPtr<IXMLDOMNode> pChar = NULL;
    CComPtr<IXMLDOMNode> pCharItem = NULL;
    CComPtr<IXMLDOMNode> pProfileItem = NULL;
    CComPtr<IXMLDOMNode> pProfileParmItem= NULL;
    CComPtr<IXMLDOMNodeList> pCharList = NULL;
    CComPtr<IXMLDOMNodeList> pProfileList = NULL;
    CComPtr<IXMLDOMNodeList> pProfileParmList = NULL;

    hr = pDoc->get_firstChild(&pWap);
    if(FAILED(hr) || pWap == NULL)
        goto Error;    

    hr = pWap->get_firstChild(&pChar);
    if(FAILED(hr) || pChar == NULL)
        goto Error; 

    hr = pChar->get_childNodes(&pCharList);
    if(FAILED(hr) || pCharList == NULL)
        goto Error;

    long lCharElements = 0;
    long lProfileElements = 0;
    long lProfileParmElements = 0;
    hr = pCharList->get_length(&lCharElements);
    if(FAILED(hr))
        goto Error;

    // loop through all the wifi types
    for(int i=0; i<lCharElements; i++)
    {
        pCharItem = NULL;
        hr = pCharList->get_item(i, &pCharItem);
        if(FAILED(hr) || pCharItem == NULL)
            goto Error; 

        // this is allow to fail
        if(!oXml.GetElementAttribute(_T("type"), szTemp, pCharItem))
            continue;

        // are we looking through access points?
        if(0 == m_oStr->Compare(_T("access-point"), szTemp))
            bAccessPoint = TRUE;
        else
            bAccessPoint = FALSE;

        pProfileList = NULL;
        hr = pCharItem->get_childNodes(&pProfileList);
        if(FAILED(hr) || pProfileList == NULL)
            continue;

        lProfileElements = 0;
        hr = pProfileList->get_length(&lProfileElements);
        if(FAILED(hr))
            continue;

        
        // loop through all the profiles
        for(int j=0; j<lProfileElements; j++)
        {
            pProfileItem = NULL;
            hr = pProfileList->get_item(j, &pProfileItem);
            if(FAILED(hr) || pProfileItem == NULL)
                goto Error; 

            // this is allow to fail
            if(!oXml.GetElementAttribute(_T("type"), szTemp, pProfileItem))
                continue;

            pProfileParmList = NULL;
            hr = pProfileItem->get_childNodes(&pProfileParmList);
            if(FAILED(hr) || pProfileList == NULL)
                continue;

            lProfileParmElements = 0;
            hr = pProfileParmList->get_length(&lProfileParmElements);
            if(FAILED(hr))
                continue;

            if(sWifi)
            {
                delete sWifi;
                sWifi = NULL;
            }
            sWifi = new TypeWifi;

            sWifi->bAccessPoint = bAccessPoint;
            sWifi->szName       = m_oStr->CreateAndCopy(szTemp);

            // loop through all the profiles parameters
            for(int k=0; k<lProfileParmElements; k++)
            {
                pProfileParmItem = NULL;
                hr = pProfileParmList->get_item(k, &pProfileParmItem);
                if(FAILED(hr) || pProfileParmItem == NULL)
                    goto Error; 

                // read the name value pair
                if(oXml.GetElementAttribute(_T("name"), szTemp, pProfileParmItem))
                {
                    if(oXml.GetElementAttribute(_T("value"), szValue, pProfileParmItem))
                    {
                        TCHAR* szName = m_oStr->CreateAndCopy(szTemp);
                        TCHAR* szVal  = m_oStr->CreateAndCopy(szValue);
                        sWifi->arrParmNames.AddElement(szName);
                        sWifi->arrParmValues.AddElement(szVal);
                    }
                }
            }

            arrItems.AddElement(sWifi);
            sWifi = NULL;
        }

    }

    bReturn = TRUE;
 
Error:
    pWap = NULL;
    pChar = NULL;
    pCharItem = NULL;
    pProfileItem = NULL;
    pProfileParmItem= NULL;
    pCharList = NULL;
    pProfileList = NULL;
    pProfileParmList = NULL;

    oXml.Close();

    if(sWifi)
      delete sWifi;

    return bReturn;
}


BOOL CDlgWifiHero::SaveFileInfo(TCHAR* szFileName, TCHAR* szContents)
{
    FILE	*fp;
    CHAR*	sText = NULL;
    int		iTextLength;

    //open the file
    if((fp = _tfopen(szFileName, _T("wb"))) == NULL)
    {
        //the file was not able to be open
        return FALSE;
    }

    iTextLength = m_oStr->GetLength(szContents);

    sText = m_oStr->CreateAndCopyChar(szContents);

    //write to the file
    fwrite(sText, sizeof(CHAR), iTextLength, fp);


    //close the file
    fclose(fp);

    m_oStr->Delete(&sText);
    sText = NULL;

    return TRUE;
}

void CDlgWifiHero::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgWifiHero* pThis = (CDlgWifiHero*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, sWifi);
}

BOOL CDlgWifiHero::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, TypeWifi* sWifi)
{
    if(bIsHighlighted)
    {
        if(!m_imgButtonSel.IsLoaded() || WIDTH(rc) != m_imgButtonSel.GetWidth() || HEIGHT(rc) != m_imgButtonSel.GetHeight())
        {
            m_imgButtonSel.Initialize(m_hWnd, m_hInst, IDR_PNG_DetailsBtnGreen);
            m_imgButtonSel.SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButtonSel.DrawImage(gdi, rc.left, rc.top);
    }
    else
    {
        if(!m_imgButton.IsLoaded() || WIDTH(rc) != m_imgButton.GetWidth() || HEIGHT(rc) != m_imgButton.GetHeight())
        {
            m_imgButton.Initialize(m_hWnd, m_hInst, IDR_PNG_DetailsBtn);
            m_imgButton.SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButton.DrawImage(gdi, rc.left, rc.top);
    }

    if(!sWifi)
        return FALSE;

    RECT rcText;
    rcText			= rc;
//    if(bIsHighlighted)
//        rcText.bottom	-= HEIGHT(rc)/2;
    rcText.left		+= GetSystemMetrics(SM_CXSMICON)/2;
    rcText.right    -= GetSystemMetrics(SM_CXSMICON)/2;
    DrawTextShadow(gdi, sWifi->szName, rcText, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontBtnLabel, RGB(255,255,255), RGB(0,0,0));


    return TRUE;
}

void CDlgWifiHero::OnSaveProfile()
{
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    BOOL bSuccess = FALSE;

    TCHAR szFileName[STRING_MAX];
    TCHAR szError[STRING_MAX];

    //////////////////////////////////////////////////////////////////////////
    // save the new data and order in an XML file
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlSaved);
    if(!SaveFileValues(szFileName))
    {
        m_oStr->StringCopy(szError, _T("Unable to save new wifi profile"));
        UpdateTitle(szError);
        goto Error;
    }
    bSuccess = TRUE;


Error:
    SetCursor(hCurs);

    if(bSuccess)
        UpdateTitle(_T("Profile restored to device"));
}

void CDlgWifiHero::OnDeleteProfile()
{
    if(IDYES != MessageBox(m_hWnd, _T("Are you sure you want to delete all saved WiFi profiles?"), TXT_APP_TITLE, MB_ICONEXCLAMATION | MB_YESNO))
        return;

    UpdateTitle(_T("Deleting saved profiles"));

    TCHAR szFileName[STRING_MAX];
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlSaved);
    DeleteFile(szFileName);

    UpdateWindow(m_hWnd);
    RefreshValues();
    UpdateTitle(_T("All saved profiles deleted"));
}

void CDlgWifiHero::OnMenu()
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return;

    m_wndMenu.ResetContent();

    int iCount = m_oList.GetItemCount();
    int iCurrent = m_oList.GetSelectedItemIndex();
    BOOL bGrayed = TRUE;
    TypeItems* sItem = m_oList.GetSelectedItem();
    if(sItem && sItem->lpItem)
    {
        TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
        bGrayed = sWifi->bOnDevice;
    }

 /*   m_wndMenu.AddItem(_T("Move Top"), IDMENU_MoveTop, (iCurrent==0||iCount==0?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Move Up"), IDMENU_MoveUp, (iCurrent==0||iCount==0?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Move Down"), IDMENU_MoveDown, (iCurrent==iCount-1||iCount==0?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Move Bottom"), IDMENU_MoveBottom, (iCurrent==iCount-1||iCount==0?FLAG_Grayed:NULL));
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Save Profile"), IDMENU_SaveProfile, (iCount==0?FLAG_Grayed:NULL));*/
    m_wndMenu.AddItem(_T("Show All Profiles"), IDMENU_ShowAllProfiles, (m_bShowAllProfiles?FLAG_Check:NULL));
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Restore Selected"), IDMENU_RestoreSelected, (iCount==0||bGrayed?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Delete Selected"), IDMENU_DeleteSelected, (iCount==0||bGrayed?FLAG_Grayed:NULL));
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Delete Saved Profiles"), IDMENU_DeleteProfile, (iCount==0?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Reset Passwords"), IDMENU_ResetPasswords, (m_arrPasswords.GetSize()==0?FLAG_Grayed:NULL));
    m_wndMenu.AddItem(_T("Refresh List"), IDMENU_RefreshList);
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Help"), IDMENU_Help);
    m_wndMenu.AddItem(_T("About"), IDMENU_About);
    m_wndMenu.AddItem(_T("Exit"), IDMENU_Exit);
    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.left,rcClient.top,
        GetSystemMetrics(SM_CXSCREEN)*3/4, HEIGHT(rcClient),
        0, 0, 10,10, ADJUST_Bottom);
}

void CDlgWifiHero::OnUpdate()
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return;

    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    // wifi has to be off
    /*if(m_oWireless.IsWifiPowerOn())
        m_oWireless.TurnWifiPowerOff();*/

    BOOL bRet = TRUE;

    int iCount = 0;
    int iStart = 0;
    int iTotalCount = m_oList.GetItemCount();
    if(m_bShowAllProfiles)
    {
        while(iCount+iStart < iTotalCount &&
            m_oList.GetItem(iCount+iStart)->eType == ITEM_Unknown)
            iCount++;
    }
    else
    {
        iCount = m_oList.GetItemCount();
    }

    if(m_oList.GetItemCount() == 0)
    {
        UpdateTitle(_T("Device is already updated"));
    }
    else
    {
        for(int i = iStart + iCount; i > iStart; i--)
        {
            if(!RestoreEntry(i-1))
                bRet = FALSE;
        }
        if(bRet)
            UpdateTitle(_T("Profiles applied succesfully"));
        else
            UpdateTitle(_T("Unable to restore profiles"));
        InvalidateRect(m_hWnd, NULL, FALSE);
        UpdateWindow(m_hWnd);
        RefreshValues(FALSE);
    }

    SetCursor(hCurs);
/*

    //now we can just be lazy and apply the diff XML
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    BOOL bSuccess = FALSE;
    TCHAR szError[STRING_MAX] = _T(""); 
    HRESULT hr = S_OK;

    TCHAR szFileName[STRING_MAX];
  
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlDiff);

    bSuccess = UpdateDevice(szFileName);


//Error:
    SetCursor(hCurs);

    if(!bSuccess)
        //MessageBox(m_hWnd, _T("Device is already updated"), _T("WiFi Hero"), MB_OK);
        UpdateTitle(_T("Device is already updated"));
    else
    {
        UpdateTitle(_T("Profiles applied succesfully"));
        UpdateWindow(m_hWnd);
        RefreshValues(FALSE);
    }*/
}

/*
void CDlgWifiHero::OnUpdate()
{
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    BOOL bSuccess = FALSE;
    TCHAR szError[STRING_MAX] = _T("");

    if(m_oList.GetItemCount() == 0)
    {
        m_oStr->StringCopy(szError, _T("No Wifi profile entries to update"));
        goto Error;
    }

    HRESULT hr = S_OK;

    //////////////////////////////////////////////////////////////////////////
    // delete what is in there now
    
    TCHAR* szOutput = NULL;
    hr = DMProcessConfigXML(g_wszAddFraud, CFGFLAG_PROCESS, &szOutput);
    m_oStr->Delete(&szOutput);

    if(hr != S_OK)
    {
        m_oStr->StringCopy(szError, _T("Unable to clear device wifi profiles.  Insufficient security privileges"));
        goto Error;
    }

    TCHAR szFileName[STRING_MAX];

    //////////////////////////////////////////////////////////////////////////
    // save the new data and order in an XML file
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlSaved);
    if(!SaveFileValues(szFileName))
    {
        m_oStr->StringCopy(szError, _T("Unable to save new wifi profile"));
        goto Error;
    }

    //////////////////////////////////////////////////////////////////////////
    // update the device profiles now
    if(!UpdateDevice(szFileName))
    {
        m_oStr->StringCopy(szError, _T("Unable to update device wifi profiles.  Insufficient security priviledges"));
        goto Error;
    }

    bSuccess = TRUE;

Error:
    SetCursor(hCurs);

    if(!bSuccess)
        MessageBox(m_hWnd, szError, _T("Error"), MB_OK);
}*/

BOOL CDlgWifiHero::SaveFileValues(TCHAR* szFileName, CIssVector<TypeWifi>& arrItems)
{
    CIssXmlBuilder oXml;
    BOOL bSuccess = FALSE;
    HRESULT hr;

    // nothing to save
    if(arrItems.GetSize() == 0)
    {
        DeleteFile(szFileName);
        return TRUE;
    }

    if(!oXml.Open())
        return FALSE;

    CComPtr<IXMLDOMNode> pNodeCharacteristic = NULL;
    CComPtr<IXMLDOMNode> pNodeProfile = NULL;
    CComPtr<IXMLDOMNode> pNodeParm = NULL;
    CComPtr<IXMLDOMNode> pNodeInserted = NULL;

    if(!oXml.CreateElement(_T("wap-provisioningdoc")))
        goto Error;
    if(!oXml.InsertChildElement())
        goto Error;
    if(!oXml.SetCurrentElement(_T("//wap-provisioningdoc")))
        goto Error;
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("Wi-Fi")))
        goto Error;
    if(!oXml.InsertChildElement())
        goto Error;
    if(!oXml.SetCurrentElement(_T("//wap-provisioningdoc//characteristic")))
        goto Error;

    //////////////////////////////////////////////////////////////////////////
    // Access point branch
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("access-point")))
        goto Error;
    pNodeCharacteristic = oXml.GetConstructionNode();

    for(int i=0; i < arrItems.GetSize(); i++)
    {
        TypeWifi* sWifi = (TypeWifi*)arrItems[i];
        if(!sWifi || !sWifi->bAccessPoint)// only looking for access points right now
            continue;

        pNodeProfile = NULL;
        if(!oXml.CreateElement(_T("characteristic")))
            goto Error;
        if(!oXml.AddAttribute(_T("type"), sWifi->szName))
            goto Error;
        pNodeProfile = oXml.GetConstructionNode();
        // add the parameters
        for(int j=0; j<sWifi->arrParmValues.GetSize(); j++)
        {
            pNodeParm = NULL;
            TCHAR* szValue  = sWifi->arrParmValues[j];
            TCHAR* szName   = sWifi->arrParmNames[j];
            if(!szValue || !szName)
                continue;

            if(!oXml.CreateElement(_T("parm")))
                goto Error;
            if(!oXml.AddAttribute(_T("name"), szName))
                goto Error;
            if(!oXml.AddAttribute(_T("value"), szValue))
                goto Error;
            pNodeParm = oXml.GetConstructionNode();
            pNodeInserted = NULL;
            hr = pNodeProfile->appendChild(pNodeParm, &pNodeInserted);
            if (FAILED(hr))
                goto Error;
        }

        // now add the profile
        pNodeInserted = NULL;
        hr = pNodeCharacteristic->appendChild(pNodeProfile, &pNodeInserted);
        if (FAILED(hr))
            goto Error;
    }
    if(!oXml.InsertChildElement(pNodeCharacteristic))
        goto Error;

    //////////////////////////////////////////////////////////////////////////
    // ad hoc branch
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("ad-hoc")))
        goto Error;
    pNodeCharacteristic = oXml.GetConstructionNode();

    for(int i=0; i< arrItems.GetSize(); i++)
    {
        TypeWifi* sWifi = (TypeWifi*)arrItems[i];
        if(!sWifi || sWifi->bAccessPoint)// only looking for ad hocs right now
            continue;

        pNodeProfile = NULL;
        if(!oXml.CreateElement(_T("characteristic")))
            goto Error;
        if(!oXml.AddAttribute(_T("type"), sWifi->szName))
            goto Error;
        pNodeProfile = oXml.GetConstructionNode();
        // add the parameters
        for(int j=0; j<sWifi->arrParmValues.GetSize(); j++)
        {
            pNodeParm = NULL;
            TCHAR* szValue  = sWifi->arrParmValues[j];
            TCHAR* szName   = sWifi->arrParmNames[j];
            if(!szValue || !szName)
                continue;

            if(!oXml.CreateElement(_T("parm")))
                goto Error;
            if(!oXml.AddAttribute(_T("name"), szName))
                goto Error;
            if(!oXml.AddAttribute(_T("value"), szValue))
                goto Error;
            pNodeParm = oXml.GetConstructionNode();
            pNodeInserted = NULL;
            hr = pNodeProfile->appendChild(pNodeParm, &pNodeInserted);
            if (FAILED(hr))
                goto Error;
        }

        // now add the profile
        pNodeInserted = NULL;
        hr = pNodeCharacteristic->appendChild(pNodeProfile, &pNodeInserted);
        if (FAILED(hr))
            goto Error;
    }
    if(!oXml.InsertChildElement(pNodeCharacteristic))
        goto Error;


    DeleteFile(szFileName);
    if(!oXml.SaveToFile(szFileName))
        goto Error;

    bSuccess = TRUE;

Error:
    pNodeCharacteristic = NULL;
    pNodeProfile = NULL;
    pNodeParm = NULL;
    pNodeInserted = NULL;

    oXml.Close();

    return bSuccess;
}



BOOL CDlgWifiHero::SaveFileValues(TCHAR* szFileName)
{
    CIssXmlBuilder oXml;
    BOOL bSuccess = FALSE;
    HRESULT hr;

    // nothing to save
    if(m_oList.GetItemCount() == 0)
    {
        DeleteFile(szFileName);
        return TRUE;
    }

    if(!oXml.Open())
        return FALSE;

    CComPtr<IXMLDOMNode> pNodeCharacteristic = NULL;
    CComPtr<IXMLDOMNode> pNodeProfile = NULL;
    CComPtr<IXMLDOMNode> pNodeParm = NULL;
    CComPtr<IXMLDOMNode> pNodeInserted = NULL;

    if(!oXml.CreateElement(_T("wap-provisioningdoc")))
        goto Error;
    if(!oXml.InsertChildElement())
        goto Error;
    if(!oXml.SetCurrentElement(_T("//wap-provisioningdoc")))
        goto Error;
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("Wi-Fi")))
        goto Error;
    if(!oXml.InsertChildElement())
        goto Error;
    if(!oXml.SetCurrentElement(_T("//wap-provisioningdoc//characteristic")))
        goto Error;

    //////////////////////////////////////////////////////////////////////////
    // Access point branch
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("access-point")))
        goto Error;
    pNodeCharacteristic = oXml.GetConstructionNode();

    for(int i=0; i < m_oList.GetItemCount(); i++)
    {
        TypeItems* sItem = m_oList.GetItem(i);
        if(!sItem)
            continue;
        TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
        if(!sWifi || !sWifi->bAccessPoint)// only looking for access points right now
            continue;

        pNodeProfile = NULL;
        if(!oXml.CreateElement(_T("characteristic")))
            goto Error;
        if(!oXml.AddAttribute(_T("type"), sWifi->szName))
            goto Error;
        pNodeProfile = oXml.GetConstructionNode();
        // add the parameters
        for(int j=0; j<sWifi->arrParmValues.GetSize(); j++)
        {
            pNodeParm = NULL;
            TCHAR* szValue  = sWifi->arrParmValues[j];
            TCHAR* szName   = sWifi->arrParmNames[j];
            if(!szValue || !szName)
                continue;

            if(!oXml.CreateElement(_T("parm")))
                goto Error;
            if(!oXml.AddAttribute(_T("name"), szName))
                goto Error;
            if(!oXml.AddAttribute(_T("value"), szValue))
                goto Error;
            pNodeParm = oXml.GetConstructionNode();
            pNodeInserted = NULL;
            hr = pNodeProfile->appendChild(pNodeParm, &pNodeInserted);
            if (FAILED(hr))
                goto Error;
        }

        // now add the profile
        pNodeInserted = NULL;
        hr = pNodeCharacteristic->appendChild(pNodeProfile, &pNodeInserted);
        if (FAILED(hr))
            goto Error;
    }
    if(!oXml.InsertChildElement(pNodeCharacteristic))
        goto Error;

    //////////////////////////////////////////////////////////////////////////
    // ad hoc branch
    if(!oXml.CreateElement(_T("characteristic")))
        goto Error;
    if(!oXml.AddAttribute(_T("type"), _T("ad-hoc")))
        goto Error;
    pNodeCharacteristic = oXml.GetConstructionNode();

    for(int i=0; i<m_oList.GetItemCount(); i++)
    {
        TypeItems* sItem = m_oList.GetItem(i);
        if(!sItem)
            continue;
        TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
        if(!sWifi || sWifi->bAccessPoint)// only looking for ad hocs right now
            continue;

        pNodeProfile = NULL;
        if(!oXml.CreateElement(_T("characteristic")))
            goto Error;
        if(!oXml.AddAttribute(_T("type"), sWifi->szName))
            goto Error;
        pNodeProfile = oXml.GetConstructionNode();
        // add the parameters
        for(int j=0; j<sWifi->arrParmValues.GetSize(); j++)
        {
            pNodeParm = NULL;
            TCHAR* szValue  = sWifi->arrParmValues[j];
            TCHAR* szName   = sWifi->arrParmNames[j];
            if(!szValue || !szName)
                continue;

            if(!oXml.CreateElement(_T("parm")))
                goto Error;
            if(!oXml.AddAttribute(_T("name"), szName))
                goto Error;
            if(!oXml.AddAttribute(_T("value"), szValue))
                goto Error;
            pNodeParm = oXml.GetConstructionNode();
            pNodeInserted = NULL;
            hr = pNodeProfile->appendChild(pNodeParm, &pNodeInserted);
            if (FAILED(hr))
                goto Error;
        }

        // now add the profile
        pNodeInserted = NULL;
        hr = pNodeCharacteristic->appendChild(pNodeProfile, &pNodeInserted);
        if (FAILED(hr))
            goto Error;
    }
    if(!oXml.InsertChildElement(pNodeCharacteristic))
        goto Error;

    DeleteFile(szFileName);
    if(!oXml.SaveToFile(szFileName))
        goto Error;

    bSuccess = TRUE;

Error:
    pNodeCharacteristic = NULL;
    pNodeProfile = NULL;
    pNodeParm = NULL;
    pNodeInserted = NULL;

    oXml.Close();

    return bSuccess;
}

BOOL CDlgWifiHero::UpdateDevice(TCHAR* szFileName)
{
    FILE *fp = NULL;
    char *szFileBuffer = NULL;
    TCHAR* szText = NULL;
    BOOL bSuccess = FALSE;

    //open the file
    if((fp = _tfopen(szFileName, _T("rb"))) == NULL)
        return FALSE;

    //read file stuff
    DWORD dwFileSize = 0;
    fseek(fp, 0, SEEK_END);
    dwFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //the File was open so read in the contents in a buffer
    szFileBuffer = new char[dwFileSize+1];
    if(szFileBuffer == NULL)
        goto Error;

    ZeroMemory(szFileBuffer, sizeof(CHAR)*(dwFileSize+1));

    fread(szFileBuffer, sizeof(char), dwFileSize, fp);
    //end file reading

    LPTSTR		lpBuf = (LPTSTR)szFileBuffer;//just easier

    if( dwFileSize == 0 || lpBuf == NULL)
        goto Error;

    HRESULT hr         = E_FAIL;
    LPWSTR wszOutput   = NULL;

    szText = m_oStr->CreateAndCopy(szFileBuffer);
    if(!szText)
        goto Error;

    // Process the XML.
    hr = DMProcessConfigXML(szText, CFGFLAG_PROCESS, &wszOutput);
    delete [] wszOutput;
    if(hr != S_OK)
        goto Error;

    bSuccess = TRUE;

Error:
    m_oStr->Delete(&szFileBuffer);
    m_oStr->Delete(&szText);

    if(fp)
        fclose(fp);

    return bSuccess;
}

BOOL CDlgWifiHero::DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip)
{
    if(m_gdiBack.GetDC() == NULL || WIDTH(rcClient) != m_gdiBack.GetWidth() || HEIGHT(rcClient) != m_gdiBack.GetHeight())
    {
        m_gdiBack.Create(gdiMem.GetDC(), rcClient, FALSE, TRUE);

        GradientFillRect(m_gdiBack, rcClient, RGB(70,70,70), 0, FALSE);

        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IDR_PNG_Background, m_hWnd, m_hInst, TRUE);

        CIssGDIEx gdiScale;
        SIZE szSize;

        if(gdiTemp.GetWidth() > WIDTH(rcClient) || 
            gdiTemp.GetHeight() > HEIGHT(rcClient))
        {
            szSize.cx = gdiTemp.GetWidth()/2;
            szSize.cy = gdiTemp.GetHeight()/2;

            ScaleImage(gdiTemp, gdiScale, szSize, FALSE, 0);

            //centered        
            // int iX = WIDTH(rcClient)/2 - szSize.cx/2;
            // int iY = HEIGHT(rcClient)/2 - szSize.cy/2;

            //bottom right
            int iX = WIDTH(rcClient) - szSize.cx;
            int iY = HEIGHT(rcClient) - szSize.cy - GetSystemMetrics(SM_CXSMICON);

            Draw(m_gdiBack, iX, iY, szSize.cx, szSize.cy, gdiScale);
        }
        else
        {
            //centered
            //int iX = WIDTH(rcClient)/2 - gdiTemp.GetWidth()/2;
            //int iY = HEIGHT(rcClient)/2 - gdiTemp.GetHeight()/2;
            //bottom right
            int iX = WIDTH(rcClient) - gdiTemp.GetWidth();
            int iY = HEIGHT(rcClient) - gdiTemp.GetHeight() - GetSystemMetrics(SM_CXSMICON);

            Draw(m_gdiBack, iX, iY, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp);
        }
    }
    Draw(gdiMem, rcClip, m_gdiBack, rcClip.left, rcClip.top);

    return TRUE;
}

BOOL CDlgWifiHero::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIMER)
    {
        UpdateTitle(NULL);
        KillTimer(m_hWnd, IDT_TIMER);
        return TRUE;
    }

    if(m_oList.OnTimer(wParam, lParam))
        return TRUE;

    return UNHANDLED;
}

BOOL CDlgWifiHero::DeleteSelectedEntry()
{
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    int iIndex = m_oList.GetSelectedItemIndex();

    DeleteEntry(iIndex);

    // restore cursor
    SetCursor(hCurs);
    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateTitle(_T("Profile deleted"));

    return TRUE;
}

BOOL CDlgWifiHero::RestoreSelectedEntry()
{
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    
    int iIndex = m_oList.GetSelectedItemIndex();

    if(RestoreEntry(iIndex))
    {
        UpdateTitle(_T("Profile restored"));
    }
    else
    {
        UpdateTitle(_T("Unable to restore profile"));
    }

    // restore cursor
    SetCursor(hCurs);
    InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}

BOOL CDlgWifiHero::DeleteEntry(int iIndex)
{
    BOOL bRet = FALSE;
    TypeWifi* oWifi = NULL;
    TCHAR szSSID[STRING_MAX];
    TCHAR szFileName[STRING_MAX];
    

    CIssVector<TypeWifi> arrDiff, arrSaved;

    if(iIndex < 0 || iIndex >= m_oList.GetItemCount())
        goto Error;

    //we need to delete from the list
    if(S_OK != m_oList.RemoveItem(iIndex))
        goto Error;

    //delete from the diff XML
    //load file
      
    // read the saved info
    if(S_OK != GetExeDirectory(szFileName))
        goto Error;
    m_oStr->Concatenate(szFileName, FILE_XmlDiff);
    
    if(!ReadFileValues(szFileName, arrDiff)) 
        goto Error;

    //delete from Vector
    //correct the index for the title 
    if(m_bShowAllProfiles)
        iIndex --;
    if(iIndex < 0)
        goto Error;
    
    oWifi = arrDiff[iIndex];
    if(oWifi == NULL)
        goto Error;

    //save the SSID for the next XML file
    m_oStr->StringCopy(szSSID, oWifi->szName);

    arrDiff.RemoveElementAt(iIndex);
    delete oWifi;
    oWifi = NULL;

    //save file
    SaveFileValues(szFileName, arrDiff);

    //delete array
    DeleteProfile(arrDiff);

    //delete from the saved XML
    // read the saved info
    if(S_OK != GetExeDirectory(szFileName))
        goto Error;
    m_oStr->Concatenate(szFileName, FILE_XmlSaved);

    if(!ReadFileValues(szFileName, arrSaved)) 
        goto Error;

    //find the correct index
    int iSavedIndex = -1;

    TCHAR* szName;

    for(int i = 0; i < arrSaved.GetSize(); i++)
    {
        if(arrSaved[i] == NULL)
            break;
        szName = arrSaved[i]->szName;

        if(m_oStr->Compare(szName, szSSID) == 0)
        {
            iSavedIndex = i;
            break;
        }
    }

    if(iSavedIndex == -1)
        goto Error;

    //delete the entry from the vector
    oWifi = arrSaved[iSavedIndex];
    if(oWifi == NULL)
        goto Error;

    arrSaved.RemoveElementAt(iSavedIndex);
    delete oWifi;
    oWifi = NULL;

    //save the file
    SaveFileValues(szFileName, arrSaved);

    //done

    bRet = TRUE;

Error:
    if(arrSaved.GetSize())
        DeleteProfile(arrSaved);
    if(arrDiff.GetSize())
        DeleteProfile(arrDiff);

    // redraw the screen
    InvalidateRect(m_hWnd, NULL, FALSE);

    return bRet;
}

BOOL CDlgWifiHero::RestoreEntry(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_oList.GetItemCount())
        return FALSE;

    TCHAR szFileName[STRING_MAX];
    BOOL bRet = FALSE;

    CIssVector<TypeWifi> arrEntry;

    TypeWifi* sWifiEntry = new TypeWifi;

    TypeItems* sItem = m_oList.GetItem(iIndex);
    if(!sItem)
        goto Error;
    TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
    TCHAR* szName;
    TCHAR* szValue;

    TCHAR* szPassword;
    TCHAR* szPassName;

    BOOL bSwap = FALSE;

    sWifiEntry->Clone(sWifi);

    arrEntry.AddElement(sWifiEntry);

    //now lets see if we have a password saved for this location
    for(int i = 0; i < m_arrPasswords.GetSize(); i++)
    {
        if(m_arrPasswords[i] == NULL)
            continue;

        szPassName = m_arrPasswords[i]->szName;
        szPassword = m_arrPasswords[i]->szPassword;

        if(m_oStr->Compare(szPassName, sWifiEntry->szName) == 0)
        {
            //now we have to find the password entry
            for(int j = 0; j < sWifiEntry->arrParmNames.GetSize(); j++)
            {
                szName = sWifiEntry->arrParmNames[j];

                if(szName == NULL)
                    continue;

                if(m_oStr->Compare(szName, _T("NetworkKey")) == 0)
                {
                    //finally ... lets swap it in!
                    szValue = sWifiEntry->arrParmValues[j];
                    if(szValue == NULL)
                    {
                        ASSERT(0);
                    }
                    m_oStr->StringCopy(szValue, szPassword);
                    bSwap = TRUE;
                }

                if(bSwap)
                    break;
            }
            break;
        }

        if(bSwap)
            break;
    }

    if(S_OK != GetExeDirectory(szFileName))
        goto Error;
    m_oStr->Concatenate(szFileName, FILE_XmlRestore);

    if(!SaveFileValues(szFileName, arrEntry))
        goto Error;

    if(UpdateDevice(szFileName))
    {

        DeleteProfile(arrEntry);
#ifndef DEBUG
        DeleteFile(szFileName);
#endif

        if(!DeleteEntry(iIndex))
            goto Error;
    }
Error:
    return bRet;
}

void CDlgWifiHero::LaunchWifiItem(int iIndex)
{
    TypeItems* sItem = m_oList.GetItem(iIndex);
    if(!sItem)
        return;
    TypeWifi* sWifi = (TypeWifi*)sItem->lpItem;
    if(!sWifi)
        return;

    CDlgWifiItem dlgWifiItem;

    dlgWifiItem.Init(&m_gdiMem, &m_gdiBack, &m_imgButton, &m_imgButtonSel, m_hFontBtnLabel, sWifi);

    dlgWifiItem.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
}

BOOL CDlgWifiHero::UpdateTitle(TCHAR* szMsg)
{
    if(szMsg == NULL)
    {
        if(m_oList.GetItemCount() == 0)
            m_oStr->StringCopy(m_szTitle, TXT_APP_TITLE);
        else
            m_oStr->StringCopy(m_szTitle, TXT_DEFAULT_MESSAGE);
    }
    else
    {
        m_oStr->StringCopy(m_szTitle, szMsg, STRING_MAX);
    }


    KillTimer(m_hWnd, IDT_TIMER);
    
    

    //force an update
    InvalidateRect(m_hWnd, &m_rcTitle, FALSE);
    UpdateWindow(m_hWnd);

    SetTimer(m_hWnd, IDT_TIMER, TXT_DISPLAY_TIME, NULL);


    return TRUE;
}

void CDlgWifiHero::CheckForPasswords()
{
    //crack open the device XML

    CIssVector<TypeWifi> arrDevice;
    TCHAR szFileName[STRING_MAX];

    //////////////////////////////////////////////////////////////////////////
    // read the saved info
    GetExeDirectory(szFileName);
    m_oStr->Concatenate(szFileName, FILE_XmlDevice);
    ReadFileValues(szFileName, arrDevice); // don't care about result

    TypeWifi* sWiFi = NULL;
    TCHAR* szName;
    TCHAR* szValue;

    for(int i = 0; arrDevice.GetSize(); i++)
    {
        sWiFi = arrDevice[i];

        if(sWiFi == NULL)
            return;

        //search for ****
        for(int j = 0; j < sWiFi->arrParmNames.GetSize(); j++)
        {
            szName = sWiFi->arrParmNames[j];
            if(szName == NULL)
                continue;

            if(m_oStr->Compare(szName, _T("NetworkKey")) == 0)
            {
                szValue = sWiFi->arrParmValues[j];
                if(szValue == NULL)
                    continue;

                //now look for a star
                if(szValue[0] == _T('*'))
                {   //ask for password
                    CDlgAddPassword dlgPass;
                    dlgPass.SetName(sWiFi->szName, sWiFi->bAccessPoint);

                    BOOL bFound = FALSE;

                    //compare to known values
                    for(int k = 0; k < m_arrPasswords.GetSize(); k++)
                    {
                        if(m_arrPasswords[k] == NULL)
                            break;
                        if(m_oStr->Compare(m_arrPasswords[k]->szName, sWiFi->szName) == 0)
                        {
                            bFound = TRUE;
                            break;
                        }
                    }
            
                    //prompt user for password
                    if(bFound == FALSE && IDOK == dlgPass.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
                    {
                        PasswordType* sPass = new PasswordType;
                        TCHAR szTemp[STRING_LARGE];

                        //save to array
                        if(dlgPass.GetKey(szTemp))
                        {
                            m_oStr->StringCopy(sPass->szName, sWiFi->szName);
                            m_oStr->StringCopy(sPass->szPassword, szTemp);
                            m_arrPasswords.AddElement(sPass);
                            sPass = NULL;
                        }
                    }
                }
            }
        }
    }
}

void CDlgWifiHero::DeleteMenuItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    TypeWifi* sWifi = (TypeWifi*)lpItem;

    if(sWifi)
    {
        delete sWifi;
        sWifi = NULL;
    }
}