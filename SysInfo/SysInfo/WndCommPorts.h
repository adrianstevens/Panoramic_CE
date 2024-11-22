#pragma once
#include "WndInfoEdit.h"
#include "IssVector.h"
#include "IssString.h"

struct SerialPortType
{
	TCHAR szPortName[STRING_LARGE];
	TCHAR szDescription[STRING_LARGE]; //who cares ... its temporary anyways .. makes cleanup easy
};

class CWndCommPorts : public CWndInfoEdit
{
public:
	CWndCommPorts(void);
	~CWndCommPorts(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		GetCommPortInfo();
	void		EnumeratePorts();
	void		EnumerateBTPorts();//don't forget the BT ports
	HRESULT		GetActiveDriverKey(TCHAR* szKeyName, HKEY* hKey);
	HRESULT		GetKeyValue(HKEY hKey, TCHAR* szValueName, TCHAR& szValue);

	void		DeleteVector();

private:
	CIssVector<SerialPortType>	m_arrSerialPorts;
	CIssString*	m_oStr;

};
