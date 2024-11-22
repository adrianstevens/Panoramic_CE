#include "MyGenerator.h"
#include "chord-generator.h"

TCHAR g_szFrets[18][5] = 
{
	_T(""),    _T(""), _T("III"), _T(""), _T("V"),    _T(""),
	_T("VII"), _T(""), _T("IX"),  _T(""), _T(""),     _T("XII"),
	_T(""),    _T(""), _T("XV"),  _T(""), _T("XVII"), _T("")
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
//	_T("6-9"), 
	_T("7sus4"), _T("7b5"), _T("7b9"), _T("9sus4"), 
	_T("add9"), _T("aug9")
};

TCHAR g_szTunings[(int)TUNING_Count][STRING_NORMAL] = 
{
	_T("Norm"), _T("DropD"), _T("DropD2"),
	_T("DModal"),  _T("Drop"),   _T("Forths"),
	_T("ShiftE"), _T("OpenD"), _T("Lute"),
};

TCHAR g_szTuningsLong[(int)TUNING_Count][STRING_NORMAL] = 
{
	_T("Normal - E A D G B E"), _T("Drop D - D A D G B E"), _T("Drop D Alt - D A D G B D"),
	_T("D Modal - D A D G A D"),  _T("Drop - D#G#C#F#A#D#"),   _T("Forths - E A D G C F"),
	_T("Shifted E - E G#C#F#B E"), _T("Open D - E G#C#F#B E"), _T("Lute - E A D F#B E"),
};

CMyGenerator::CMyGenerator(void)
{
	m_oStr = CIssString::Instance();

	m_eTuning = TUNING_Standard;
	m_eType = CHORD_maj;
	m_eRoot = ROOT_C;
	m_iChordIndex = 0;
	m_bIsLeftHanded = FALSE;
	m_bNumberFrets = FALSE;


	SetTuning(m_eTuning);
	GenerateChords(m_eRoot, m_eType);
}

CMyGenerator::~CMyGenerator(void)
{
}

BOOL CMyGenerator::GenerateChords(EnumChordRoot eRoot, EnumChordType eType)
{
	TCHAR szTemp[STRING_NORMAL];
	
	m_oStr->Format(szTemp, _T("%s%s"), g_szRoots[eRoot], g_szChords[eType]);

	return GenerateChords(szTemp);
}

BOOL CMyGenerator::GenerateChords(TCHAR* szName)
{
	m_iChordIndex = 0;

	char buf[256], buf2[256];

	strcpy(buf, "A");
	strcpy(buf2, buf);

	// Allocate it for DOS/WINDOWS, to avoid stack overflow (weak)
//	Fretboard fb;
//	Chord chord;
//	chord.findChord(buf);
//	chord.print(buf2);
//	fb.iterate(chord);
//	fb.printStack();
//	printf("Enter Chord: ");

	//convert to char
	CHAR szTemp[STRING_NORMAL];
	m_oStr->StringCopy(szTemp, szName);

	m_oChord.findChord(szTemp);
//	m_oChord.print(szTemp);
	m_oFretBoard.Reset();
	m_oFretBoard.iterate(m_oChord);
//	m_oFretBoard.printStack();

	return TRUE;
}

void CMyGenerator::SetChord(EnumChordType eType, BOOL bReload)
{
	if(m_eType == eType)
		return;

	m_eType = eType;

	if(bReload)
		GenerateChords(m_eRoot, m_eType);
}

void CMyGenerator::SetRoot(EnumChordRoot eRoot, BOOL bReload)
{
	if(m_eRoot == eRoot)
		return;

	m_eRoot = eRoot;

	if(bReload)
		GenerateChords(m_eRoot, m_eType);

}

int	CMyGenerator::GetFret(int iString)
{
	return m_oFretBoard.bestFrets[m_iChordIndex][iString];
}

TCHAR* CMyGenerator::GetNote(int iString)
{
	return g_szRoots[m_oFretBoard.bestNotes[m_iChordIndex][iString]];
}

int CMyGenerator::GetNoteValue(int iString)
{
	if(GetFret(iString) == -1)
		return -1;//not played

	return m_oFretBoard.bestNotes[m_iChordIndex][iString];//otherwise return the note
}

int CMyGenerator::GetStartingFret()
{
	int iRet = 100;
	int iMax = 0;
	
	for(int i = 0; i < 6; i++)
	{
		if(m_oFretBoard.bestFrets[m_iChordIndex][i] > 0)
			iRet = min(iRet, m_oFretBoard.bestFrets[m_iChordIndex][i]);
		iMax = max(iMax, m_oFretBoard.bestFrets[m_iChordIndex][i]);
	}

	if(iRet == 2)
		iRet = 1;

	if(iRet == 3 && iMax <= 5)
		iRet = 1;

	return iRet;

}

void CMyGenerator::SetLeftHanded(BOOL bLeftHanded)
{
	m_bIsLeftHanded = bLeftHanded;
}

void CMyGenerator::SetTuning(EnumTuning eTuning, BOOL bReload)
{
	m_eTuning = eTuning;

	switch(m_eTuning)
	{
	case TUNING_Standard:
		m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7);
		break;
	case TUNING_DropD:
		m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7);
		break;
	case TUNING_DropDAlt:
	    m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 5);
		break;
	case TUNING_DModal:
	    m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 0, 5);
		break;
	case TUNING_Drop:
		m_oFretBoard.SetOpenStrings( 6, 11, 4, 9, 1, 6);
		break;
	case TUNING_Fourths:
		m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 3, 8);
		break;
	case TUNING_ShiftedE:
	    m_oFretBoard.SetOpenStrings( 7, 11, 4, 9, 1, 7);
		break;
	case TUNING_OpenD:
	    m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7);
		break;
	case TUNING_Lute:
		m_oFretBoard.SetOpenStrings( 7, 11, 4, 9, 2, 7);
		break;
	default:
		return;
	    break;
	}

	if(bReload)
		GenerateChords(m_eRoot, m_eType);
}

int CMyGenerator::GetOpenNote(int iString)
{
	if(iString < 0 || iString > 5)
		return -1;

	return m_oFretBoard.GetOpenString(iString);
}

