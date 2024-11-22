#include "GuitarChords.h"
#include "IssCommon.h"


TCHAR g_szFrets[12][4] = 
{
	_T(""), _T(""), _T("III"), _T(""), _T("V"), _T(""),
	_T("VII"), _T(""), _T("IX"), _T(""), _T(""), _T("XII")
};

TCHAR g_szRoots[12][8] = 
{
	_T("A"), _T("A#"), _T("B"), _T("C"), _T("C#"), _T("D"),
	_T("D#"), _T("E"), _T("F"), _T("F#"), _T("G"), _T("G#")
};

TCHAR g_szChords[(int)CHORD_Count][STRING_NORMAL] = 
{
	_T("maj"), _T("5"), _T("6"), _T("7"), _T("maj7"), 
	_T("9"), _T("maj9"), _T("11"), _T("13"), _T("maj13"), 
	_T("m"), _T("m6"), _T("m7"), _T("m9"), _T("m11"), 
	_T("m13"), _T("sus2"), _T("sus4"), _T("dim"), _T("aug"), 
	_T("6-9"), _T("7sus4"), _T("7b5"), _T("7b9"), _T("9sus4"), 
	_T("add9"), _T("aug9")

};

CGuitarChords::CGuitarChords(void)
{
    m_oStr = CIssString::Instance();

    for(int i = 0; i < ROOT_Count; i++)
        m_bInitialized[i] = false;

    //top indicators
/*    m_oTestChord.btFingerPos[0] = 2;
    m_oTestChord.btFingerPos[1] = 0;
    m_oTestChord.btFingerPos[2] = 2;
    m_oTestChord.btFingerPos[3] = 2;
    m_oTestChord.btFingerPos[4] = 2;
    m_oTestChord.btFingerPos[5] = 1;

    m_oTestChord.btStartingFret = 1;

    m_oTestChord.btStringPlayed[0] = 11; //fret then finger ...
    m_oTestChord.btStringPlayed[1] = GUITAR_BLANK;//not 
    m_oTestChord.btStringPlayed[2] = 22;
    m_oTestChord.btStringPlayed[3] = 33;
    m_oTestChord.btStringPlayed[4] = 33;
    m_oTestChord.btStringPlayed[5] = GUITAR_BLANK;//not 

    m_oStr->StringCopy(m_oTestChord.szNote[0], _T("A#"));
    m_oStr->StringCopy(m_oTestChord.szNote[1], _T("B"));
    m_oStr->StringCopy(m_oTestChord.szNote[2], _T("C"));
    m_oStr->StringCopy(m_oTestChord.szNote[3], _T("Db"));
    m_oStr->StringCopy(m_oTestChord.szNote[4], _T("E"));
    m_oStr->StringCopy(m_oTestChord.szNote[5], _T("F#"));

    m_iChordIndex = 1; // for now
    m_oStr->StringCopy(m_szChordName, _T("maj9"));*/

	m_eChordRoot = ROOT_A;
	m_eChordType = CHORD_maj;

    LoadChords(ROOT_A);
}

CGuitarChords::~CGuitarChords(void)
{
	Destroy();
}

ChordType* CGuitarChords::GetCurrentCord()
{
    //make sure its initialized first
    if(m_bInitialized[(int)m_eChordRoot] == false)
        LoadChords(m_eChordRoot);


    //seems about right I guess .... 
    CIssVector<ChordType>* arrChords = NULL;
    arrChords = &m_arrChords[m_eChordRoot][m_eChordType];

    if(arrChords->GetSize() == 0)
        return NULL;

    ChordType* sChord = NULL;

    sChord = (ChordType*)arrChords->GetElement(m_iChordIndex);

    return sChord;
}

int CGuitarChords::GetCurrentChordIndex()
{
	if(m_arrChords[m_eChordRoot][m_eChordType].GetSize() == 0)
		return -1; //works nicely for the interface
    return m_iChordIndex;
}

int CGuitarChords::GetNumVariations()
{
	CIssVector<ChordType>* arrChords = NULL;
	arrChords = &m_arrChords[m_eChordRoot][m_eChordType];

    return arrChords->GetSize();
}

void CGuitarChords::NextChord()
{
    m_iChordIndex++;
    if(m_iChordIndex >= GetNumVariations())
        m_iChordIndex = 0;
}


//I'd like to load them one root at a time instead of per file so I can check if I have chords
//defined for each chord type and dynamically populate the menus
BOOL CGuitarChords::LoadChords(EnumChordRoot eRoot)
{
    TCHAR szIniName[STRING_MAX];

    int iCount = 0;
    int iTemp = 0;
    int iTemp2 = 0;

    TCHAR szTemp[STRING_LARGE];
    
    ChordType* sType = NULL; //&m_oTestChord;
    TCHAR szNum[STRING_NORMAL];//really more like a temp2

	CIssVector<ChordType>* arrChords = NULL;

    int r = (int)eRoot;

//	for(int r = 0; r < ROOT_Count; r++)
//	{
		for(int  c = 0; c < CHORD_Count; c++)
		{
			GetExeDirectory(szIniName);
			m_oStr->Concatenate(szIniName, g_szRoots[r]);
			m_oStr->Concatenate(szIniName, _T("\\"));
			m_oStr->Concatenate(szIniName, g_szChords[c]);
			m_oStr->Concatenate(szIniName, _T(".txt"));
//			m_oStr->Concatenate(szIniName, _T("A\\maj.txt"));

			if(S_OK != m_oIni.Open(szIniName))
				continue;

			iCount = 0;
			m_oIni.GetValue(&iCount, _T("Main"), _T("NumChords"), 0);

			arrChords = &m_arrChords[r][c];

			for(int i = 0; i < iCount; i++)
			{
				sType = new ChordType;
				if(sType == NULL)
					goto Error;

				m_oStr->IntToString(szTemp, i+1);//1 based not 0

				//fret
				m_oIni.GetValue(&iTemp, szTemp, _T("TopFret"), 1);//default this sucker to 1 .. that's fine
				sType->btStartingFret = iTemp;

				m_oIni.GetValue(szNum, szTemp, _T("TopIndicators"), _T("0000000"));//all blank

				for(int j = 0; j < 6; j++)
					sType->btFingerPos[j] = (byte)(szNum[j] - _T('0'));

				for(int j = 0; j < 6; j++)
				{
					m_oStr->Format(szNum, _T("Str%i"), j+1);
					iTemp = GUITAR_BLANK;
					m_oIni.GetValue(&iTemp, szTemp, szNum, GUITAR_BLANK);//all blank
					sType->btStringPlayed[j] = (byte)iTemp;
				}

				//and finally lets break out the note values ... 
				m_oIni.GetValue(szNum, szTemp, _T("Notes"), _T(""));//all blank

				//now since we're at the end we can use Temp1 as well ... a temp variable
				iTemp = 0;//index
				iTemp2 = m_oStr->Find(szNum, _T(","));
				if(iTemp2 > iTemp)
					m_oStr->StringCopy(sType->szNote[0], szNum, iTemp, iTemp2 - iTemp);
				else 
					m_oStr->Empty(sType->szNote[0]);
				for(int j = 1; j < 5; j++)
				{
					iTemp = iTemp2;
					iTemp2 = m_oStr->Find(szNum, _T(","), iTemp + 1);
					if(iTemp2 > iTemp + 1)
						m_oStr->StringCopy(sType->szNote[j], szNum, iTemp+1, iTemp2 - iTemp - 1);
					else 
						m_oStr->Empty(sType->szNote[j]);
				}
				iTemp = iTemp2;
				iTemp2 = m_oStr->GetLength(szNum);
				if(iTemp2 > iTemp)
					m_oStr->StringCopy(sType->szNote[5], szNum, iTemp + 1, iTemp2 - iTemp - 1);

				//bridge time
				m_oStr->Empty(szNum);
				m_oIni.GetValue(szNum, szTemp, _T("Bridge1"), _T(""));//all blank
				if(m_oStr->GetLength(szNum) == 3)
				{
					sType->btBridge1[0] = szNum[0] - _T('0');
					sType->btBridge1[1] = szNum[1] - _T('0');
					sType->btBridge1[2] = szNum[2] - _T('0');
				}
				else
				{
					sType->btBridge1[0] = GUITAR_BLANK;
				}

				m_oStr->Empty(szNum);
				m_oIni.GetValue(szNum, szTemp, _T("Bridge2"), _T(""));//all blank
				if(m_oStr->GetLength(szNum) == 3)
				{
					sType->btBridge2[0] = szNum[0] - _T('0');
					sType->btBridge2[1] = szNum[1] - _T('0');
					sType->btBridge2[2] = szNum[2] - _T('0');
				}
				else
				{
					sType->btBridge2[0] = GUITAR_BLANK;
				}

				arrChords->AddElement(sType);
				sType = NULL;
			}
//		}
	}

    m_bInitialized[(int)m_eChordRoot] = true;

Error:
    if(sType)
        delete sType;



    return TRUE;
}

void CGuitarChords::Destroy()
{
	CIssVector<ChordType>* arrChords = NULL;
	ChordType* sType;

	for(int i = 0; i < CHORD_Count; i++)
	{
		for(int j = 0; j < ROOT_Count; j++)
		{
			arrChords = &m_arrChords[j][i];

			for(int k = 0; k < arrChords->GetSize(); k++)
			{
				sType = (ChordType*)arrChords->GetElement(k);
				if(sType)
				{
					delete sType;
					sType = NULL;
				}
			}

			arrChords->RemoveAll();
			arrChords = NULL;
		}
	}
}

void CGuitarChords::SetChordRoot(EnumChordRoot eRoot)
{
    if(m_bInitialized[(int)eRoot] == false)
        LoadChords(eRoot);

    m_eChordRoot = eRoot;
}

int CGuitarChords::GetChordCount(EnumChordRoot eRoot, EnumChordType eType)
{
    return m_arrChords[eRoot][eType].GetSize();
}