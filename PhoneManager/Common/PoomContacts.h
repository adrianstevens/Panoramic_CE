#pragma once

#include "windows.h"
#include "IssGDIEx.h"
#include "IssString.h"
#include "pimstore.h"
#include "IssVector.h"
#include "imaging.h"
#include "PimStoreEx.h"
#include <soundfile.h>

#define FIX_ANNOYING_POOM_ONEOFF_ERROR

enum EnumContactInfo
{
    CI_FirstName = 0,
	CI_MiddleName,
    CI_LastName,
    CI_MobileNumber,
    CI_HomeNumber,
    CI_HomeNumber2,//new
    CI_HomeFax,//new
    CI_SMS,
    CI_WorkNumber,
    CI_WorkNumber2,//new
    CI_WorkFax,//new
    CI_Pager,//new
    CI_Car,//new
    CI_Email,
    CI_Email2,//new
    CI_Email3,//new
    CI_Company,//new
    CI_FileAs,

    CI_Birthday,
    CI_Anniversary,
    CI_Department,
    CI_JobTitle,
    CI_Category,
    CI_Webpage,
    CI_HomeAddress,
    CI_BusinessAddress,
	CI_Notes,
    CI_Count,
};

struct TypeContact
{
	TCHAR*	szFirstName;
	TCHAR*	szMiddleName;
	TCHAR*	szLastName;
	TCHAR*	szHomeNumber;
    TCHAR*  szHomeNumber2;
    TCHAR*  szHomeFax;
	TCHAR*	szMobileNumber;
	TCHAR*	szWorkNumber;
    TCHAR*  szWorkNumber2;
    TCHAR*  szWorkFax;
    TCHAR*  szPager;
    TCHAR*  szCar;
	TCHAR*	szEmailAddr;
    TCHAR*	szEmailAddr2;
    TCHAR*	szEmailAddr3;
	TCHAR*	szCompany;
	CIssGDIEx* gdiPicture;
	LONG	lOid;						// unique identifier for this contact

	TypeContact(void);
	~TypeContact(void);
	void Clear();						// clear out so we start with a blank slate
};


class CPoomContacts
{
public:
	CPoomContacts(void);
	CPoomContacts(HWND hWndApp, BOOL bShowSimContacts);
	~CPoomContacts(void);

	static CPoomContacts*	Instance();
	static void				DeleteInstance();

	BOOL 					Initialize(HWND hWndApp = NULL, BOOL bShowSimContacts = TRUE);
	BOOL 					Destroy();

	BOOL					GetUserName(TCHAR* szPhoneNum, TCHAR* szName);
	BOOL 					LaunchEditContactScreen(HWND hWnd, LONG lOid);
	BOOL					LaunchEditContactScreen(HWND hWnd, int iIndex);
    int                     FindContactFromName(TCHAR* szNameGiven);
	int						FindContactFromNumber(TCHAR* szNumber);
	IContact* 				GetContactFromIndex(int iIndex);
	IContact* 				GetContactFromOID(LONG lOid);
	BOOL 					SetContact(TypeContact& sContact);
    HRESULT                 CreateNew(HWND hWnd, TCHAR* szPhoneNum = NULL);
    HRESULT                 DeleteContact(LONG lOid);
    HRESULT                 AddPictureToContact(LONG lOid, LPTSTR pszPictureFile);

	int	 					GetNumContacts();

	//all of these TCHAR* returning functions create memory that needs to be handled externally
    TCHAR*      GetUserData(EnumContactInfo eInfo, int iIndex);
    TCHAR*      GetUserData(EnumContactInfo eInfo, IContact* pContact);
    TCHAR*      GetUserData(EnumContactInfo eInfo, LONG lOid);

    TCHAR*		GetUserFirstName(int iIndex){return GetUserData(CI_FirstName, iIndex);}; 
	TCHAR*		GetUserLastName(int iIndex){return GetUserData(CI_LastName, iIndex);};
	TCHAR*		GetUserName(int iIndex);
	TCHAR*		GetUserEmail(int iIndex){return GetUserData(CI_Email, iIndex);};
    TCHAR*		GetUserEmail2(int iIndex){return GetUserData(CI_Email2, iIndex);};
    TCHAR*		GetUserEmail3(int iIndex){return GetUserData(CI_Email3, iIndex);};
	TCHAR*		GetUserHomeNum(int iIndex){return GetUserData(CI_HomeNumber, iIndex);}; 
	TCHAR*		GetUserMobileNum(int iIndex){return GetUserData(CI_MobileNumber, iIndex);};
    TCHAR*		GetUserWorkNum(int iIndex){return GetUserData(CI_WorkNumber, iIndex);};
	LONG		GetUserOID(int iIndex);
	BOOL		GetUserOID(TCHAR* szPhoneNum, long* pOid);
	CIssGDIEx*	GetUserPicture(int iIndex, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp = FALSE);

    TCHAR*		GetUserFirstName(IContact* pContact){return GetUserData(CI_FirstName, pContact);}; 
    TCHAR*		GetUserLastName(IContact* pContact){return GetUserData(CI_LastName, pContact);};
    TCHAR*		GetUserName(IContact* pContact);
    TCHAR*		GetUserEmail(IContact* pContact){return GetUserData(CI_Email, pContact);};
    TCHAR*		GetUserEmail2(IContact* pContact){return GetUserData(CI_Email2, pContact);};
    TCHAR*		GetUserEmail3(IContact* pContact){return GetUserData(CI_Email3, pContact);};
    TCHAR*		GetUserHomeNum(IContact* pContact){return GetUserData(CI_HomeNumber, pContact);}; 
    TCHAR*		GetUserMobileNum(IContact* pContact){return GetUserData(CI_MobileNumber, pContact);};
    TCHAR*		GetUserWorkNum(IContact* pContact){return GetUserData(CI_WorkNumber, pContact);};
	CIssGDIEx*	GetUserPicture(IContact* pContact, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp = FALSE);

    TCHAR*		GetUserFirstName(LONG lOid){return GetUserData(CI_FirstName, lOid);}; 
    TCHAR*		GetUserLastName(LONG lOid){return GetUserData(CI_LastName, lOid);};
    TCHAR*		GetUserName(LONG lOid);
    TCHAR*		GetUserEmail(LONG lOid){return GetUserData(CI_Email, lOid);};
    TCHAR*		GetUserEmail2(LONG lOid){return GetUserData(CI_Email2, lOid);};
    TCHAR*		GetUserEmail3(LONG lOid){return GetUserData(CI_Email3, lOid);};
    TCHAR*		GetUserHomeNum(LONG lOid){return GetUserData(CI_HomeNumber, lOid);}; 
    TCHAR*		GetUserMobileNum(LONG lOid){return GetUserData(CI_MobileNumber, lOid);};
    TCHAR*		GetUserWorkNum(LONG lOid){return GetUserData(CI_WorkNumber, lOid);};
	CIssGDIEx*	GetUserPicture(LONG lOid, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp = FALSE);

	int			GetIndexFromOID(LONG lOid);

	BOOL		GetContactInfo(	IContact* pContact, 
								TypeContact& sContact, 
								BOOL bRetrievePicture = FALSE,
								int iDesiredPicWidth = 0,
								int iDesiredPicHeight = 0,
								COLORREF crBackground = 0x000000/*TRANSPARENT_COLOR*/);

	BOOL		GetContactCategories(TCHAR* pszCategories);				// all categories for all contacts
	BOOL		GetContactCategories(TCHAR* pszCategories, long lOid);	// all categories for one contact
	BOOL		SetContactRingtone(IContact* pContact, SNDFILEINFO* pInfo);
	BOOL		GetContactRingtone(IContact* pContact, SNDFILEINFO* pInfo);
    BOOL        RemoveContactRingtone(IContact* pContact);
    BOOL        SetContactPicture(LONG lOid);
    BOOL        GetFormatedNameText(TCHAR* szName, LONG lOid, BOOL bLastNameFirst = TRUE);
    BOOL        GetFormatedNameText(TCHAR* szName, IContact* pContact, BOOL bLastNameFirst = TRUE);
	BOOL		GetFormatedNameText(TCHAR* szName, TCHAR* szGivenFirstName, TCHAR* szGivenMiddleName, TCHAR* szGivenLastName, TCHAR* szBusiness, TCHAR* szFileAs, BOOL bLastNameFirst);
	

private:	// functions
	
	TCHAR*		MakeStrictPhoneNumber(TCHAR* szPhoneNumber);
	BOOL		MakeStrictPhoneNumber(TCHAR* szPhoneNumber, TCHAR* szStrict);
    HRESULT     SelectPicture(TCHAR* szPictureFile);
    HRESULT     SaveFileToStream(LPTSTR pszFileName, IStream *pStream, BOOL bResize);
    
	BOOL		DoesUserOwnNumber(IContact* pContact, TCHAR* szPhoneNum);
	BOOL		GetStreamSize(IStream* pStream, ULONG* pulSize);
	CIssGDIEx*	ISSGDIFromImage(IN IImage * pImage, IN COLORREF crBackColor = 0x00000000/*TRANSPARENT_COLOR*/);
	CIssGDIEx*	GetBitmapFromStream(IStream* pStream, int iDesiredWidth, int iDesiredHeight, COLORREF crBackground, BOOL bScaleUp = FALSE);
	void		ScaleProportional(UINT uFitToWidth, UINT uFitToHeight, UINT *puWidthToScale, UINT *puHeightToScale, BOOL bScaleUp = FALSE);
	

private:	// variables
	CIssString*					m_oStr;
	IPOutlookApp2*				m_polApp;			// outlook app
	IFolder*					m_pFolder;			// we focus on the Contacts folder
	IPOutlookItemCollection*	m_pContacts;		// our collection of contacts
	IPOlItems3*					m_pItem;			// one item
	//int							m_iNumContacts;		// number of contacts found Let's not cache this-it can change
	BOOL						m_bInitialized;		// quick hack for now..
	static CPoomContacts*		m_Instance;

};
