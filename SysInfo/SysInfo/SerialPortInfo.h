#pragma once


// CSerialPortInfo
//
//		Contains information about a serial port
//
class CSerialPortInfo
{
private:
	CString	m_strPortName,			// Serial port name (COM1:)
			m_strDescription;		// Serial port description if available

public:

	CSerialPortInfo(LPCTSTR pszPortName, LPCTSTR pszDescription)
		:	m_strPortName		(pszPortName),
			m_strDescription	(pszDescription)
	{
	}

	LPCTSTR GetPortName		() const	{ return m_strPortName;		}
	LPCTSTR	GetDescription	() const	{ return m_strDescription;	}
};
