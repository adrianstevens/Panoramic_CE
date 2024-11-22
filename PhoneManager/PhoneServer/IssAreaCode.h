#pragma once

#include "IssVector.h"
#include "IssString.h"

#define NO_TIME_ZONE 99

struct AreaCodeType
{
	int		iAreaCode;
	TCHAR	szRegion[3];
	int		iTimeZoneOffset; //relative to central time
	TCHAR*	szDescription;
};

class CIssAreaCode
{
public:
	CIssAreaCode(void);
	~CIssAreaCode(void);

	void			Init(HINSTANCE hInst){m_hInst = hInst;};
	BOOL			LoadAreaCodes();
	void			SetAreaCode(int iAreaCode);//input the area code as an int - ie 604
	SYSTEMTIME		GetLocalTime();//get the local time of the incoming call
	AreaCodeType*	GetAreaCodeInfo();

private:
	void			DelAreaCodeVector();
	
private:
	CIssString*					m_oStr;
	
	CIssVector<AreaCodeType>	m_arrAreaCodes;

	CRITICAL_SECTION			m_cs;

	HINSTANCE					m_hInst;

	int							m_iAreaCodeIndex;//index in the vector for the area code


};
