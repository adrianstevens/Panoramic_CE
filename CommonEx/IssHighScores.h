#pragma once

#include "stdafx.h"
#include "issString.h"
#include "IssVector.h"

#define NOT_IN_HISCORE_TABLE -1


struct TypeScore
{
	TCHAR*		szName;				// the name of the lucky high scorer
	int			iScoreValue;		// this is the abstract score value so we can sort properly
	LPBYTE		lpScore;			// will be a static chunk of memory (usually a struct of some sort)
	UINT		uiScoreSize;		// size of this static chunk of memory
	SYSTEMTIME	sysTime;			// the time of this high score
};

class CIssHighScores
{
public:
	CIssHighScores(void);
	~CIssHighScores(void);

	BOOL		Init(int iMaxScoreSaves);	
	int			AddHighScore(int iScoreValue, TCHAR* szName, LPVOID lpScore, UINT uiScoreSize);//return the index - ie position in the highscore table -1 if failed
	BOOL		SaveRegistry(TCHAR* szKey);
	BOOL		LoadRegistry(TCHAR* szKey);
	TypeScore*	GetScore(int iIndex);
	int			GetScoreCount(){return m_arrScores.GetSize();};

	int			GetHiScore();

	int			GetIndexOfLastScore(){return m_iIndexOfLastAdded;};

private:	// functions
	void		DeleteScoreItem(int iIndex);
	void		DeleteScoreVector();


private:	// variables
	CIssString*				m_oStr;
	CIssVector<TypeScore>	m_arrScores;
	int						m_iMaxScoreSaves;		// how many max high scores you want to show
	int						m_iIndexOfLastAdded;

};
