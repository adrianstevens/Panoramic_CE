#include "MyGenerator.h"
#include "chord-generator.h"
#include "IssLocalisation.h"
#include "Resource.h"

//http://www.cyberfret.com/scales/guitar-codex/index.php

char g_szScalesCode[(int)SCALE_Count][STRING_NORMAL] = 
{
    ("1,2,3,4,5,6,7"),
    ("1,2,b3,4,5,b6,7"),
    ("1,2,b3,4,5,6,7"),
    ("1,2,3,4,5,b6,b7"),
    ("1,2,3,5,6"),
    ("1,b3,4,5,b7"),
    ("1,b3,4,b5,b7"),//pent blues
    ("1,2,4,5,b7"),
    ("1,2,b3,4,5,6,b7"),//dorian
    ("1,b2,b3,4,5,b6,b7"),
    ("1,2,3,#4,5,6,7"),
    ("1,2,3,4,5,6,b7"),
    ("1,2,b3,4,5,b6,b7"),
    ("1,b2,b3,4,b5,b6,b7"),
    ("1,b2,b3,3,b5,5,6,b7"),//half diminished
    ("1,2,b3,4,b5,#5,6,7"),
    ("1,2,3,#4,#5,#6"),
    ("1,#2,3,#4,#5,7"),
    ("1,b2,2,b3,3,4,b5,5,#5,6,b7,7"),
    ("1,2,b3,b5,5,6,b7"),
    ("1,b2,3,4,5,#5,b7"),
    ("1,b2,3,4,5,#5,7"), //blues 
    ("1,b2,3,4,b5,#5,7"),
    ("1,b2,b3,3,4,b5,#5,b7"),
    ("1,b2,3,b5,#5,b7,7"),
    ("1,2,3,b5,#5,6,b7"),
    ("1,2,3,#4,#5,6,7"),
    ("1,b2,b3,4,5,6,7"),
    ("1,b2,b3,4,5,b6,b7"),
    ("1,b2,b3,b5,b7,7"),//pelog
    ("1,2,3,b5,6,b7"),
    ("1,b2,3,b5,6,b7"),
    ("1,b2,3,4,#5,6"),
    ("1,b2,b3,3,b5,#5,b7"),
    ("1,2,3,b5,5,b6,b7"),
    ("1,2,b3,b5,5,b6,b7"),
    ("1,2,b3,3,b5,5,#5,6,7"),//nine tone scale 
    ("1,2,b3,4,b5,#5,6,7"),
    ("1,2,3,b5,#5,b7"),
    ("1,b2,b3,3,b5,6,b7"),
    ("1,2,3,4,b5,#5,b7"),
    ("1,2,3,b5,5,6,b7"),
    ("1,b2,b3,3,b5,b6,b7"),
    ("1,2,4,5,6,b7")//scale
};

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

TCHAR* CMyGenerator::GetChordText(int iIndex, HINSTANCE hInst)
{
    if(iIndex < 0 || iIndex >= CHORD_Count || hInst == NULL)
        return NULL;
    CIssString* oStr = CIssString::Instance();
    return oStr->GetText(ID(IDS_CHORD_Maj+iIndex));
}

TCHAR* CMyGenerator::GetTuningLongText(int iIndex, HINSTANCE hInst, EnumInstrument eInst)
{
    UINT uiRes = 0;

    static TCHAR szTemp[STRING_LARGE];
    
    CIssString* oStr = CIssString::Instance();

    oStr->Empty(szTemp);

    if(iIndex < 0 || iIndex >= GetNumTuning(eInst))
        return NULL;

    oStr->StringCopy(szTemp, GetTuningText(iIndex, hInst, eInst));

    oStr->Concatenate(szTemp, _T(" ( "));

    int iTuning = GetTuning();
    SetTuning(iIndex, FALSE);

    for(int i = 0; i < GetNumStrings(); i++)
    {
        switch(GetOpenNote(i))
        {
        case ROOT_A:
        case ROOT_B:
        case ROOT_C:
        case ROOT_D:
        case ROOT_E:
        case ROOT_F:
        case ROOT_G:
            oStr->Concatenate(szTemp, g_szRoots[GetOpenNote(i)]);
            oStr->Concatenate(szTemp, _T(" "));
            break;
        case ROOT_As:
        case ROOT_Cs:
        case ROOT_Ds:
        case ROOT_Fs:
        case ROOT_Gs:
            oStr->Concatenate(szTemp, g_szRoots[GetOpenNote(i)]);
            break;
        default:
            return NULL;
        }
        
    }
    oStr->Concatenate(szTemp, _T(")"));

    SetTuning(iTuning, FALSE);
    return szTemp;

}

TCHAR* CMyGenerator::GetTuningTextBanjo(int iIndex, HINSTANCE hInst)
{
    CIssString* oStr = CIssString::Instance();

    static TCHAR szTemp[STRING_LARGE];
    static TCHAR szTemp2[STRING_LARGE];

    switch(iIndex)
    {
    case TUNINGBANJO_G:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_G]);
        return szTemp;
        break;
    case TUNINGBANJO_GModal:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Modal), hInst));
        oStr->Insert(szTemp, g_szRoots[ROOT_G]);
        return szTemp;
        break;
    case TUNINGBANJO_DoubleC:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Double), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_C]);
        return szTemp;
        break;
    case TUNINGBANJO_C:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_C]);
        return szTemp;
        break;
    case TUNINGBANJO_D:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_D]);
        return szTemp;
        break;
    case TUNINGBANJO_DAlt:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_D]);
        oStr->Concatenate(szTemp, _T("2"));
        return szTemp;
        break;
    case TUNINGBANJO_Guitar:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_OPT_Guitar), hInst));
        return szTemp;
        break;
    }
    return NULL;
}

TCHAR* CMyGenerator::GetTuningTextUkulele(int iIndex, HINSTANCE hInst)
{
	CIssString* oStr = CIssString::Instance();

	static TCHAR szTemp[STRING_LARGE];
    oStr->Empty(szTemp);

	switch(iIndex)
	{
	case TUNINGUKU_RentrantC:
		oStr->StringCopy(szTemp, g_szRoots[ROOT_C]);
        oStr->Concatenate(szTemp, _T(" 'RE'"));
		return szTemp;
		break;
	case TUNINGUKU_C:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_C]);		
        return szTemp;
		break;
	case TUNINGUKU_G:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_G]);
        oStr->Concatenate(szTemp, _T(" 'RE'"));
		return szTemp;
		break;
	case TUNINGUKU_D:
		oStr->StringCopy(szTemp, g_szRoots[ROOT_D]);
		return szTemp;
		break;
	case TUNINGUKU_B:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_B]);
        oStr->Concatenate(szTemp, _T(" 'RE'"));
		return szTemp;
		break;
	case TUNINGUKU_Cs:
        oStr->StringCopy(szTemp, g_szRoots[ROOT_Cs]);
        oStr->Concatenate(szTemp, _T(" 'RE'"));
		return szTemp;
		break;
	case TUNINGUKU_Guitar:
		oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_OPT_Guitar), hInst));
		return szTemp;
		break;
	case TUNINGUKU_SlackKey:
		oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_SlackKey), hInst));
		return szTemp;
		break;
	}
	return NULL;
}

TCHAR* CMyGenerator::GetTuningTextLute(int iIndex, HINSTANCE hInst)
{
    CIssString* oStr = CIssString::Instance();

    static TCHAR szTemp[STRING_LARGE];

    switch(iIndex)
    {
    case TUNINGLUTE_Mandolin:   //G D E A
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Mandolin), hInst));
        return szTemp;
        break;
    case TUNINGLUTE_Mandola:     //C G D A
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Mandola), hInst));
        return szTemp;
        break;
    case TUNINGLUTE_Pipa:        //D A D A
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Pipa), hInst));
        return szTemp;
        break;
    case TUNINGLUTE_Bouzouki:    //C F A D
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Bouzouki), hInst));
        return szTemp;
        break;
    case TUNINGLUTE_Bouzouki2:   //G D A D
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Bouzouki2), hInst));
        return szTemp;
        break;
    case TUNINGLUTE_Bouzouki3:   //A D E A
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_Bouzouki3), hInst));
        return szTemp;
        break;
    }
    return NULL;
}



TCHAR* CMyGenerator::GetTuningTextB4(int iIndex, HINSTANCE hInst)
{
    CIssString* oStr = CIssString::Instance();

    static TCHAR szTemp[STRING_LARGE];
    static TCHAR szTemp2[STRING_NORMAL];

    switch(iIndex)
    {
    case TUNINGB4_Standard:
        return oStr->GetText(ID(IDS_TUN_Standard));
        break;
    case TUNINGB4_DropD:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Drop), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_D]);
        return szTemp;
        break;
    case TUNINGB4_HalfDown:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Half), hInst));
        oStr->StringCopy(szTemp2, oStr->GetText(ID(IDS_TUN_Down), hInst));
        
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, szTemp2);
        return szTemp;
        break;
    case TUNINGB4_LowB:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Low), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_B]);
        return szTemp;
        break;
    case TUNINGB4_DropC:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Drop), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_C]);
        return szTemp;
        break;
    default:
        return NULL;
        break;
    }
}

TCHAR* CMyGenerator::GetTuningTextB5(int iIndex, HINSTANCE hInst)
{
    CIssString* oStr = CIssString::Instance();

    static TCHAR szTemp[STRING_LARGE];

   /* switch(iIndex)
    {
    case TUNINGB5_Standard:
        return oStr->GetText(ID(IDS_TUN_Standard));
        break;
    case TUNINGB5_DropD:
        return oStr->GetText(ID(IDS_TUN_DropD));
        break;
    case TUNINGB5_HiC:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_HighC), hInst));
        return szTemp;
        break;
    case TUNINGB5_HalfDown:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_HalfDown), hInst));
        return szTemp;
        break;
    case TUNINGB5_FullDown:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_FullDown), hInst));
        return szTemp;
        break;
    case TUNINGB5_HalfDownHi:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_HalfDHigh), hInst));
        return szTemp;
        break;
    case TUNINGB5_FullDownHi:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUNE_FullDHigh), hInst));
        return szTemp;
        break;
    default:
        return NULL;
        break;
    }*/

    return NULL;
}

TCHAR* CMyGenerator::GetTuningTextGuitar(int iIndex, HINSTANCE hInst)
{
    CIssString* oStr = CIssString::Instance();

    static TCHAR szTemp[STRING_LARGE];

    switch(iIndex)
    {
    case TUNING_Standard:
        return oStr->GetText(ID(IDS_TUN_Standard));
        break;
    case TUNING_Drop:
        return oStr->GetText(ID(IDS_TUN_Drop));
        break;
    case TUNING_DropD:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Drop), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_D]);
        break;
    case TUNING_DropDAlt:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Drop), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_D]);
        oStr->Concatenate(szTemp, _T("2"));
        break;
    case TUNING_OpenD:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_D]);
        break;
    case TUNING_OpenDm:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" Dm"));//could also pull it from the chords ....
        break;
    case TUNING_DModal:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Modal), hInst));
        oStr->Insert(szTemp, _T(" "));
        oStr->Insert(szTemp, g_szRoots[ROOT_D]);
        break;
    case TUNING_Fourths:
        return oStr->GetText(ID(IDS_TUN_Fourths));
        break;
    case TUNING_OpenE:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_E]);
        break;
    case TUNING_ShiftedE:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Shifted), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_E]);
        break;
    case TUNING_G6:
        oStr->StringCopy(szTemp, _T("G6"));
        break;
    case TUNING_OpenG:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_G]);
        break;
    case TUNING_Gm:
        oStr->StringCopy(szTemp, _T("Gm"));
        break;
    case TUNING_OpenA:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_A]);
        break;
    case TUNING_Am:
        oStr->StringCopy(szTemp, _T("Am"));
        break;
    case TUNING_C6:
        oStr->StringCopy(szTemp, _T("C6"));
        break;
    case TUNING_OpenC:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Open), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_C]);
        break;
    case TUNING_LowC:
        oStr->StringCopy(szTemp, oStr->GetText(ID(IDS_TUN_Low), hInst));
        oStr->Concatenate(szTemp, _T(" "));
        oStr->Concatenate(szTemp, g_szRoots[ROOT_C]);
        break;
    default:
        return NULL;
    }
    return szTemp;
}

TCHAR* CMyGenerator::GetTuningText(int iIndex, HINSTANCE hInst, EnumInstrument eInst)
{
	UINT uiRes = 0;

	if(iIndex < 0 || iIndex >= GetNumTuning(eInst))
		return NULL;
	CIssString* oStr = CIssString::Instance();

	switch(eInst)
	{
	case INST_4StringBass:
		return GetTuningTextB4(iIndex, hInst);
		break;
    case INST_Lute:
        return GetTuningTextLute(iIndex, hInst);
        break;
	case INST_Ukulele:
		return GetTuningTextUkulele(iIndex, hInst);
		break;
//	case INST_5StringBass:
//        return GetTuningTextB5(iIndex, hInst);
//		break;
    case INST_Banjo:
        return GetTuningTextBanjo(iIndex, hInst);
        break;
	case INST_Guitar:
	default:
        return GetTuningTextGuitar(iIndex, hInst);
		break;
	}
}

TCHAR* CMyGenerator::GetScalesText(int iIndex, HINSTANCE hInst)
{
    if(iIndex < 0 || iIndex >= SCALE_Count || hInst == NULL)
        return NULL;
    CIssString* oStr = CIssString::Instance();
    return oStr->GetText(ID(IDS_OPT_Major+iIndex));
}


CMyGenerator::CMyGenerator()
{
    CIssString* oStr = CIssString::Instance();

    m_eTuning       = TUNING_Standard;
    m_eTuningB4     = TUNINGB4_Standard;
    m_eTuningB5     = TUNINGB5_Standard;
    m_eTuningBanjo  = TUNINGBANJO_G;
    m_eTuningLute   = TUNINGLUTE_Mandolin;
	m_eTuningUkulele= TUNINGUKU_RentrantC;

    m_eType = CHORD_maj;
    m_eRoot = ROOT_C;
    m_iChordIndex = 0;
    m_bIsLeftHanded = FALSE;
    m_bNumberFrets = FALSE;
    m_eInst = INST_Guitar;

#ifdef DEBUG
//    m_eInst = INST_5StringBass;
#endif
}

CMyGenerator::~CMyGenerator(void)
{
}

void CMyGenerator::Init(HINSTANCE hInst)
{
    m_hInst = hInst;
    //   m_eScale = SCALE_Major;
    SetScale(SCALE_Major);
    SetTuning(m_eTuning);
    GenerateChords(m_eRoot, m_eType);
}

BOOL CMyGenerator::GenerateChords(EnumChordRoot eRoot, EnumChordType eType)
{
	TCHAR szTemp[STRING_NORMAL];

    CIssString* oStr = CIssString::Instance();
	
	//oStr->Format(szTemp, _T("%s%s"), g_szRoots[eRoot], g_szChords[eType]);
    oStr->Format(szTemp, _T("%s%s"), g_szRoots[eRoot], GetChordText((int)eType, m_hInst));

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
	if(m_eType == eType && bReload == FALSE)
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

    SetScale(m_eScale);//reload cause of root ???

	if(bReload)
		GenerateChords(m_eRoot, m_eType);
}

void CMyGenerator::SetInstrument(EnumInstrument eInst, BOOL bReload)
{
    m_eInst = eInst;

    //scale won't change until we set different tunings for diff instruments
    if(bReload)
    {
        SetTuning(GetTuning(), TRUE);//forces the strings to be reset
        GenerateChords(m_eRoot, m_eType);
    }

}

int	CMyGenerator::GetFret(int iString)
{
	return m_oFretBoard.bestFrets[m_iChordIndex][iString];
}

TCHAR* CMyGenerator::GetNote(int iString)
{
	return g_szRoots[m_oFretBoard.bestNotes[m_iChordIndex][iString]];
}

int CMyGenerator::GetNoteValue(int iString, int iFret)
{
    int iIndex = m_oFretBoard.GetOpenString(iString);
    iIndex += iFret;

    while(iIndex > 11)
        iIndex -= 12;

    return iIndex;
}

TCHAR* CMyGenerator::GetNote(int iString, int iFret)
{
    int iIndex = m_oFretBoard.GetOpenString(iString);
    iIndex += iFret;

    while(iIndex > 11)
        iIndex -= 12;

    return g_szRoots[iIndex];
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
	
	for(int i = 0; i < GetNumStrings(); i++)
	{
		if(m_oFretBoard.bestFrets[m_iChordIndex][i] > 0)
			iRet = min(iRet, m_oFretBoard.bestFrets[m_iChordIndex][i]);
		iMax = max(iMax, m_oFretBoard.bestFrets[m_iChordIndex][i]);
	}

	if(iRet == 2)
		iRet = 1;

	if(iRet == 3 && iMax <= 5)
		iRet = 1;

    if(iRet == 100)
        iRet = 1;//good ol Banjo ....

	return iRet;

}

void CMyGenerator::SetLeftHanded(BOOL bLeftHanded)
{
	m_bIsLeftHanded = bLeftHanded;
}

void CMyGenerator::SetTuningB4(int iTuning, BOOL bReload)
{
	m_oFretBoard.SetScoreVars(TRUE);

    switch(iTuning)
    {
    case TUNINGB4_Standard:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB4_DropD:
        m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB4_HalfDown:
        m_oFretBoard.SetOpenStrings( 6, 11, 4, 9, 1, 6, GetNumStrings());
        break;
    case TUNINGB4_LowB:
        m_oFretBoard.SetOpenStrings( 2, 7, 0, 5, 2, 7, GetNumStrings());
        break;
    case TUNINGB4_DropC:
        m_oFretBoard.SetOpenStrings( 3, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    }
}

//incomplete ... CHECK if you want to use it
//5 string bass typically has a lower B string below the low E ... as of writing I don't have the samples that low
void CMyGenerator::SetTuningB5(int iTuning, BOOL bReload)
{
	m_oFretBoard.SetScoreVars(TRUE);

    switch(iTuning)
    {
    case TUNINGB5_Standard:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB5_DropD:
        m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB5_HiC:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 3, 7, GetNumStrings());
        break;
    case TUNINGB5_HalfDown:
        m_oFretBoard.SetOpenStrings( 1, 6, 11, 4, 9, 7, GetNumStrings());
        break;
    case TUNINGB5_FullDown:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB5_HalfDownHi:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    case TUNINGB5_FullDownHi:
        m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
        break;
    }
}

void CMyGenerator::SetTuningBanjo(int iIndex, BOOL bReload /* = TRUE */)
{
    m_oFretBoard.SetScoreVars(FALSE);

    switch(iIndex)
    {
    case TUNINGBANJO_G:
        m_oFretBoard.SetOpenStrings( 10, 5, 10, 2, 5, 0, GetNumStrings());
        break;
    case TUNINGBANJO_GModal:
        m_oFretBoard.SetOpenStrings( 10, 5, 10, 3, 5, 0, GetNumStrings());
        break;
    case TUNINGBANJO_DoubleC:
        m_oFretBoard.SetOpenStrings( 10, 3, 10, 3, 5, 0, GetNumStrings());
        break;
    case TUNINGBANJO_C:
        m_oFretBoard.SetOpenStrings( 10, 5, 10, 5, 2, 0, GetNumStrings());
        break;
    case TUNINGBANJO_D:
        m_oFretBoard.SetOpenStrings( 5, 0, 9, 0, 5, 0, GetNumStrings());
        break;
    case TUNINGBANJO_DAlt:
        m_oFretBoard.SetOpenStrings( 9, 5, 9, 0, 5, 0, GetNumStrings());
        break;
    case TUNINGBANJO_Guitar:
        m_oFretBoard.SetOpenStrings( 0, 5, 10, 2, 7, 0, GetNumStrings());
        break;
    }
}

void CMyGenerator::SetTuningUkulele(int iIndex, BOOL bReload /* = TRUE */)
{
	m_oFretBoard.SetScoreVars(FALSE);

	switch(iIndex)
	{
	case TUNINGUKU_RentrantC:
		m_oFretBoard.SetOpenStrings(10, 3, 7, 0, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_C:
		m_oFretBoard.SetOpenStrings(10, 3, 7, 0, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_G:
		m_oFretBoard.SetOpenStrings(5, 10, 2, 7, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_D:
		m_oFretBoard.SetOpenStrings(0, 5, 9, 2, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_B:
		m_oFretBoard.SetOpenStrings(9, 2, 6, 11, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_Cs:
		m_oFretBoard.SetOpenStrings(11, 4, 8, 1, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_Guitar:
		m_oFretBoard.SetOpenStrings(5, 10, 2, 7, 0, 0, GetNumStrings());
		break;
	case TUNINGUKU_SlackKey:
		m_oFretBoard.SetOpenStrings(10, 3, 7, 10, 0, 0, GetNumStrings());
		break;
	}
}

void CMyGenerator::SetTuningLute(int iIndex, BOOL bReload /* = TRUE */)
{
	m_oFretBoard.SetScoreVars(FALSE);

    switch(iIndex)
    {
    case TUNINGLUTE_Mandolin:   //G D E A
        m_oFretBoard.SetOpenStrings(10, 5, 7, 0, 2, 7, GetNumStrings());
        break;
    case TUNINGLUTE_Mandola:     //C G D A
        m_oFretBoard.SetOpenStrings( 3, 10, 5, 0, 2, 7, GetNumStrings());
        break;
    case TUNINGLUTE_Pipa:        //D A D A
        m_oFretBoard.SetOpenStrings( 5, 0, 5, 0, 2, 7, GetNumStrings());
        break;
    case TUNINGLUTE_Bouzouki:    //C F A D
        m_oFretBoard.SetOpenStrings( 3, 8, 0, 5, 2, 7, GetNumStrings());
        break;
    case TUNINGLUTE_Bouzouki2:   //G D A D
        m_oFretBoard.SetOpenStrings(10, 5, 0, 5, 2, 7, GetNumStrings());
        break;
    case TUNINGLUTE_Bouzouki3:   //A D E A
        m_oFretBoard.SetOpenStrings( 0, 5, 7, 0, 2, 7, GetNumStrings());
        break;
    }
}

void CMyGenerator::SetTuning(int iIndex, EnumInstrument eInst)
{
    switch(eInst)
    {
    case INST_4StringBass:
        m_eTuningB4 = (EnumTuningBass4)iIndex;
        break;
    case INST_Lute:
        m_eTuningLute = (EnumTuningLute)iIndex;
        break;
    case INST_Guitar:
        m_eTuning = (EnumTuning)iIndex;
        break;
	case INST_Ukulele:
		m_eTuningUkulele = (EnumTuningUkulele)iIndex;
		break;
    case INST_Banjo:
        m_eTuningBanjo = (EnumTuningBanjo)iIndex;
        break;
    }
}





//void CMyGenerator::SetTuning(EnumTuning eTuning, BOOL bReload)
void CMyGenerator::SetTuning(int iTuning, BOOL bReload)
{
    switch(GetInstrument())
    {
    case INST_4StringBass:
        SetTuningB4(iTuning, bReload);
        m_eTuningB4 = (EnumTuningBass4)iTuning;
        if(bReload)
            GenerateChords(m_eRoot, m_eType);
        return;
        break;
//    case INST_5StringBass:
//        SetTuningB5(iTuning, bReload);
//        return;
        break;
    case INST_Banjo:
        SetTuningBanjo(iTuning, bReload);
        m_eTuningBanjo = (EnumTuningBanjo)iTuning;
        if(bReload)
            GenerateChords(m_eRoot, m_eType);
        return;
        break;
    case INST_Lute:
        SetTuningLute(iTuning, bReload);
        m_eTuningLute = (EnumTuningLute)iTuning;
        if(bReload)
            GenerateChords(m_eRoot, m_eType);
        return;
        break;
	case INST_Ukulele:
		SetTuningUkulele(iTuning, bReload);
		m_eTuningUkulele = (EnumTuningUkulele)iTuning;
		if(bReload)
			GenerateChords(m_eRoot, m_eType);
		return;
		break;
    }

	m_oFretBoard.SetScoreVars(FALSE);

	m_eTuning = (EnumTuning)iTuning;

	switch(m_eTuning)
	{
	case TUNING_Standard:
		m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
		break;
    case TUNING_Drop:
        m_oFretBoard.SetOpenStrings( 6, 11, 4, 9, 1, 6, GetNumStrings());
        break;
	case TUNING_DropD:
		m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7, GetNumStrings());
		break;
	case TUNING_DropDAlt:
	    m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 5, GetNumStrings());
		break;
    case TUNING_OpenD:
        m_oFretBoard.SetOpenStrings( 5, 0, 5, 8, 1, 5, GetNumStrings());
        break;

    case TUNING_OpenDm://D A D F A D
        m_oFretBoard.SetOpenStrings( 5, 0, 5, 8, 0, 5, GetNumStrings());
        break;



    case TUNING_DModal:
	    m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 0, 5, GetNumStrings());
		break;
	case TUNING_Fourths:
		m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 3, 8, GetNumStrings());
		break;
    case TUNING_OpenE:
        m_oFretBoard.SetOpenStrings( 7, 2, 7, 11, 2, 7, GetNumStrings());
        break;
    case TUNING_ShiftedE:
	    m_oFretBoard.SetOpenStrings( 7, 11, 4, 9, 2, 7, GetNumStrings());
		break;
   

    case TUNING_G6:
        m_oFretBoard.SetOpenStrings(5,10,5,10,2,7, GetNumStrings());
        break;
    case TUNING_OpenG:
        m_oFretBoard.SetOpenStrings(5,10,5,10,2,5, GetNumStrings());
        break;
    case TUNING_Gm:
        m_oFretBoard.SetOpenStrings(5,10,5,10,1,5, GetNumStrings());
        break;

    case TUNING_OpenA:
        m_oFretBoard.SetOpenStrings(7,0,7,0,4,7, GetNumStrings());
        break;
    case TUNING_Am:
        m_oFretBoard.SetOpenStrings(7,0,7,0,3,7, GetNumStrings());
        break;

    case TUNING_C6:
        m_oFretBoard.SetOpenStrings(3,10,3,10,0,7, GetNumStrings());
        break;
    case TUNING_OpenC:
        m_oFretBoard.SetOpenStrings(3,10,3,10,3,7, GetNumStrings());
        break;
    case TUNING_LowC:
        m_oFretBoard.SetOpenStrings(3,10,5,10,0,5, GetNumStrings());
        break;

//	case TUNING_Lute:
//		m_oFretBoard.SetOpenStrings( 7, 11, 4, 9, 2, 7, GetNumStrings());
//		break;
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

void CMyGenerator::SetScale(EnumScales eScale)
{
	if(eScale < 0 || eScale >= SCALE_Count)
		m_eScale = SCALE_Major;
	else
		m_eScale = eScale;
    
    for(int i = 0; i < SCALE_SIZE; i++)
        m_btCurrentScale[i] = NOTE_NOT_DEF;//max value 

    char* szCode = g_szScalesCode[(int)m_eScale];
    if(szCode == NULL)
        return;
        
    int iIndex = 0;//index for string
    int iNote = 0;//note from array
    int iLen = 0;//index in bt array

    bool bFlat;
    bool bSharp;
  
    //I'm not babysitting ... better be formatted correctly
    while(szCode[iIndex] != '\0')//end of the string
    {
        if(szCode[iIndex] != ',')
        {
            bFlat = false;
            bSharp = false;

            if(szCode[iIndex] == 'b')
            {
                bFlat = true;
                iIndex++;
            }
            else if(szCode[iIndex] == '#')
            {
                bSharp = true;
                iIndex++;
            }
            iNote = szCode[iIndex] - '0';

            //now note to step
            m_btCurrentScale[iLen] = ScaleToStep(iNote, bSharp, bFlat);
            iLen++;
        }
        iIndex++;
    }
}

byte CMyGenerator::ScaleToStep(int iScaleNote, bool bSharp, bool bFlat)
{
    //most are whole steps so double then correct by two zero base
    byte btRet = iScaleNote * 2 - 2;

    //now correct for the half step
    if(iScaleNote > 3)
        btRet--;
    if(bSharp)
        btRet++;
    else if(bFlat)
        btRet--;
 
    return btRet;
}

BOOL CMyGenerator::OnFavorites()
{
    if(m_oFav.AddFavorite(m_eRoot, m_eType, m_eTuning, m_iChordIndex, 
        GetNoteValue(0), GetNoteRes(1), GetNoteRes(2), GetNoteRes(3), GetNoteRes(4), GetNoteRes(5)) == FALSE)
    {   //then we delete it
        m_oFav.DeleteLastFav();
    }
    return TRUE;
}

BOOL CMyGenerator::IsStarLit()
{
    return m_oFav.IsFavorite(m_eRoot, m_eType, m_eTuning, m_iChordIndex);
}

UINT CMyGenerator::GetNoteRes(int iString)
{
    int iNote = GetNoteValue(iString);
    if(iNote == -1)
        return 0;//since its a UINT ....

    return GetNoteRes(iString, GetFret(iString));
}

UINT CMyGenerator::GetNoteRes(int iString, int iFret)
{
    UINT uiRet = -1;

    int iNote = GetOpenNote(iString);

    switch(iString)
    {
    case 0:
        uiRet = iNote + iFret + IDR_WAV_AMid - 12;
        break;
    case 1:
    case 2:
        uiRet = iNote + iFret + IDR_WAV_AMid;
        if(iNote > 9)
            uiRet -= 12;
        break;
    case 3:
    case 4:	//should be good for both the G & B strings 
        uiRet = iNote + iFret + IDR_WAV_AMid;
        if(iNote < 8)//assume its been tuned up
            uiRet += 12;
        break;
    case 5:
        uiRet = iNote + iFret + IDR_WAV_AHi;
        break;
    }

    //drop an octave for bass guitars
    if(m_eInst == INST_4StringBass)// || m_eInst == INST_5StringBass)
        uiRet -= 12;
    else
        uiRet += RaiseNote(iString)*12;

    //now check ranges
    while(uiRet < LOWEST_SOUND_RES)
        uiRet += 12;
    while(uiRet > HIGHEST_SOUND_RES)
        uiRet -= 12;



    return uiRet;
}

BOOL CMyGenerator::RaiseNote(int iString)
{
    switch(m_eInst)
    {
    case INST_Banjo:
        if(iString == 0)
            return 2;
        if(iString == 2)
            return 1;
        break;
    case INST_Ukulele:
        switch(m_eTuningUkulele)
        {
        case TUNINGUKU_C:
        case TUNINGUKU_D:
        case TUNINGUKU_Guitar:
            return FALSE;
            break;
        }
        if(iString == 0)
            return 1;
    default:
        return FALSE;
    }

    return FALSE;

/*

	if(iString > 0)
        return FALSE;
    //simple for now
    if(m_eInst == INST_Banjo)
        return TRUE;
    if(m_eInst != INST_Ukulele)
		return FALSE;
	
	switch(m_eTuningUkulele)
	{
	case TUNINGUKU_C:
	case TUNINGUKU_D:
	case TUNINGUKU_Guitar:
		return FALSE;
		break;
	}
	return TRUE;*/
}

int CMyGenerator::GetScaleLength()
{
    for(int i = 0; i < SCALE_SIZE; i++)
    {
        if(m_btCurrentScale[i] == NOTE_NOT_DEF)
            return i;
    }
    return SCALE_SIZE;
}

int CMyGenerator::GetNumStrings()
{
    switch(m_eInst)
    {
    case INST_Guitar:
        return 6;
        break;
    case INST_4StringBass:
    case INST_Lute:
	case INST_Ukulele:
        return 4;
        break;
//    case INST_5StringBass:
    case INST_Banjo:
        return 5;
        break;
    default: 
        break;
    }
    return 6;
}

int CMyGenerator::GetTuning(EnumInstrument eInst)
{
    switch(eInst)
    {
    case INST_4StringBass:
        return (int)m_eTuningB4;
        break;
    case INST_Lute:
        return (int)m_eTuningLute;
        break;
    case INST_Guitar:
        return (int)m_eTuning;
        break;
	case INST_Ukulele:
		return (int)m_eTuningUkulele;
		break;
    case INST_Banjo:
        return (int)m_eTuningBanjo;
        break;
    }
    return 0;
}

int CMyGenerator::GetTuning()
{
	switch(m_eInst)
	{
	case INST_4StringBass:
        if((int)m_eTuningB4 >= GetNumTuning(m_eInst))
            SetTuning(0, TRUE);
        return (int)m_eTuningB4;
	    break;
    case INST_Lute:
        if((int)m_eTuningLute >= GetNumTuning(m_eInst))
            SetTuning(0, TRUE);
        return (int)m_eTuningLute;
        break;
    case INST_Banjo:
        if((int)m_eTuningB4 >= GetNumTuning(m_eInst))
            SetTuning(0, TRUE);
        return (int)m_eTuningBanjo;
        break;
	case INST_Ukulele:
		if((int)m_eTuningUkulele >= GetNumTuning(m_eInst))
			SetTuning(0, TRUE);
		return (int)m_eTuningUkulele;
		break;
        
//	case INST_5StringBass:
//		break;
    case INST_Guitar:
        return (int)m_eTuning;
		break;
	}

    return 0;


}

int CMyGenerator::GetNumTuning(EnumInstrument eInst)
{
	switch(eInst)
	{
	case INST_4StringBass:
		return (int)TUNINGB4_Count;
		break;
    case INST_Lute:
        return (int)TUNINGLUTE_Count;
        break;
    case INST_Banjo:
        return (int)TUNINGBANJO_Count;
        break;
	case INST_Ukulele:
		return (int)TUNINGUKU_Count;
		break;
//	case INST_5StringBass:
//		return (int)TUNINGB5_Count;
//		break;
	case INST_Guitar:
	default:
		return (int)TUNING_Count;
		break;
	}
}