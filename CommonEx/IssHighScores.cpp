#include "IssHighScores.h"
#include "IssRegistry.h"

#define		HSREG_Name		_T("HSN%d")
#define		HSREG_Score		_T("HSS%d")
#define		HSREG_ScoreSize	_T("HSSS%d")
#define		HSREG_Time		_T("HST%d")
#define		HSREG_Value		_T("HSV%d")

CIssHighScores::CIssHighScores(void)
:m_oStr(CIssString::Instance())
,m_iMaxScoreSaves(-1)
,m_iIndexOfLastAdded(NOT_IN_HISCORE_TABLE)
{
}

CIssHighScores::~CIssHighScores(void)
{
	DeleteScoreVector();
}

void CIssHighScores::DeleteScoreItem(int iIndex)
{
	TypeScore* sScore = m_arrScores[iIndex];
	if(sScore)
	{
		m_oStr->Delete(&sScore->szName);
		delete [] sScore->lpScore;
		delete sScore;
	}
}

void CIssHighScores::DeleteScoreVector()
{
	for(int i=0; i<m_arrScores.GetSize(); i++)
	{
		DeleteScoreItem(i);
	}
	m_arrScores.RemoveAll();
}


BOOL CIssHighScores::Init(int iMaxScoreSaves)
{
	m_iMaxScoreSaves = iMaxScoreSaves;
	return TRUE;
}

//adding a score ... determine if its a new high score
int CIssHighScores::AddHighScore(int iScoreValue, TCHAR* szName, LPVOID lpScore, UINT uiScoreSize)
{
	TypeScore* sScore = new TypeScore;
	ZeroMemory(sScore, sizeof(TypeScore));
	int iRet = -1;

	if(!m_oStr->IsEmpty(szName))
		sScore->szName = m_oStr->CreateAndCopy(szName);

	sScore->iScoreValue		= iScoreValue;
	
	GetLocalTime(&sScore->sysTime);
	
	if(lpScore && uiScoreSize > 0)
	{
		sScore->lpScore	= new BYTE[uiScoreSize];
		memcpy(sScore->lpScore, lpScore, sizeof(BYTE)*uiScoreSize);
		sScore->uiScoreSize = uiScoreSize;
	}

	m_arrScores.AddElement(sScore);

	// now put it in order
	for(iRet=0; iRet < m_arrScores.GetSize()-1; iRet++)
	{
		TypeScore* sNew = m_arrScores[iRet];
		if(!sNew)
			break;

		if(sScore->iScoreValue > sNew->iScoreValue)
		{
			m_arrScores.Move(m_arrScores.GetSize()-1, iRet);
			break;
		}
	}

	// if we have a max set up then lets take off the end ones
	if(m_iMaxScoreSaves > 0)
	{
		for(int i=m_arrScores.GetSize()-1; i>0; i--)
		{
			if(i>m_iMaxScoreSaves-1)
			{
				DeleteScoreItem(i);
				m_arrScores.RemoveElementAt(i);
			}
			else
				break;
		}
	}

	//this score didn't make it into the high score table (loser)
	if(iRet >= m_iMaxScoreSaves)
		iRet = NOT_IN_HISCORE_TABLE;

	m_iIndexOfLastAdded = iRet;//save the index of the last added score

	return iRet;
}

BOOL CIssHighScores::SaveRegistry(TCHAR* szKey)
{
	if(m_oStr->IsEmpty(szKey))
		return FALSE;

	TCHAR szRegKey[STRING_MAX];
	TCHAR szValue[STRING_MAX];

	m_oStr->StringCopy(szRegKey, szKey);
	m_oStr->Concatenate(szRegKey, _T("\\Scores"));

	for(int i=0; i<m_arrScores.GetSize(); i++)
	{
		TypeScore* sScore = m_arrScores[i];
		if(!sScore)
			continue;

		if(!m_oStr->IsEmpty(sScore->szName))
		{
			m_oStr->Format(szValue, HSREG_Name, i);
			SetKey(szRegKey, szValue, sScore->szName, m_oStr->GetLength(sScore->szName));
		}

		if(sScore->lpScore && sScore->uiScoreSize)
		{
			m_oStr->Format(szValue, HSREG_ScoreSize, i);
			SetKey(szRegKey, szValue, (DWORD)sScore->uiScoreSize);

			m_oStr->Format(szValue, HSREG_Score, i);
			SetKey(szRegKey, szValue, (LPBYTE)sScore->lpScore, (DWORD)sScore->uiScoreSize);
		}

		m_oStr->Format(szValue, HSREG_Value, i);
		SetKey(szRegKey, szValue, (DWORD)sScore->iScoreValue);

		m_oStr->Format(szValue, HSREG_Time, i);
		SetKey(szRegKey, szValue, (LPBYTE)&sScore->sysTime, sizeof(SYSTEMTIME));
	}

	return TRUE;
}

BOOL CIssHighScores::LoadRegistry(TCHAR* szKey)
{
	if(m_oStr->IsEmpty(szKey))
		return FALSE;

	// we start over
	DeleteScoreVector();

	TCHAR szRegKey[STRING_MAX];
	TCHAR szValue[STRING_MAX];

	m_oStr->StringCopy(szRegKey, szKey);
	m_oStr->Concatenate(szRegKey, _T("\\Scores"));

	TCHAR szName[STRING_MAX];
	SYSTEMTIME sysTime;
	DWORD	dwScoreValue, dwScoreSize;
	int		iCount = 0;
	LPBYTE lpScore = NULL;

	while(TRUE)
	{
		m_oStr->Format(szValue, HSREG_Value, iCount);
		if(FAILED(GetKey(szRegKey, szValue, (DWORD)dwScoreValue)))
			break;

		m_oStr->Format(szValue, HSREG_Time, iCount);
		dwScoreSize	= sizeof(SYSTEMTIME);
		if(FAILED(GetKey(szRegKey, szValue, (LPBYTE)&sysTime, dwScoreSize)))
			break;

		m_oStr->Format(szValue, HSREG_Name, iCount);
		m_oStr->Empty(szName);
		GetKey(szRegKey, szValue, szName, STRING_MAX);
		

		lpScore		= NULL;
		dwScoreSize = 0;
		m_oStr->Format(szValue, HSREG_ScoreSize, iCount);
		if(SUCCEEDED(GetKey(szRegKey, szValue, (DWORD)dwScoreSize) && dwScoreSize > 0))
		{
			lpScore = new BYTE[dwScoreSize];
			ZeroMemory(lpScore, sizeof(BYTE)*dwScoreSize);
			m_oStr->Format(szValue, HSREG_Score, iCount);
			if(FAILED(GetKey(szRegKey, szValue, (LPBYTE)lpScore, dwScoreSize)))
			{
				delete [] lpScore;
				lpScore		= NULL;
				dwScoreSize = 0;
			}
		}

		TypeScore* sScore = new TypeScore;
		ZeroMemory(sScore, sizeof(TypeScore));

		if(!m_oStr->IsEmpty(szName))
			sScore->szName		= m_oStr->CreateAndCopy(szName);

		sScore->iScoreValue		= dwScoreValue;
		sScore->sysTime			= sysTime;
		sScore->lpScore			= lpScore;
		sScore->uiScoreSize		= dwScoreSize;

		m_arrScores.AddElement(sScore);


		iCount++;
	}

	return TRUE;
}

TypeScore* CIssHighScores::GetScore(int iIndex)
{
	return m_arrScores[iIndex];
}


int CIssHighScores::GetHiScore()
{
	if(m_arrScores.GetSize() == 0)
		return 0;

	return m_arrScores[0]->iScoreValue;



}