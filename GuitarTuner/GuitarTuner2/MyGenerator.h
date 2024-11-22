#pragma once
#include "stdafx.h"
#include "chord-generator.h"
#include "IssString.h"

#define NUM_CHORDS	9 //looks good ... seems cool

enum EnumTuning
{
	TUNING_Standard,
	TUNING_DropD,
	TUNING_DropDAlt,
	TUNING_DModal,
	TUNING_Drop,
	TUNING_Fourths,
	TUNING_ShiftedE,
	TUNING_OpenD,
	TUNING_Lute,
	TUNING_Count,
};

//we'll add to it as we find more chords ...
enum EnumChordType
{
	CHORD_maj,
	CHORD_5,
	CHORD_6,
	CHORD_7,
	CHORD_maj7,
	CHORD_9,
	CHORD_maj9,
	CHORD_11,
	CHORD_13,
	CHORD_maj13,
	CHORD_m,
	CHORD_m6,
	CHORD_m7,
	CHORD_m9,
	CHORD_m11,
	CHORD_m13,
	CHORD_sus2,
	CHORD_sus4,
	CHORD_dim,
	CHORD_aug,
//	CHORD_6over9,
	CHORD_7sus4,
	CHORD_7b5,
	CHORD_7b9,
	CHORD_9sus4,
	CHORD_add9,
	CHORD_aug9,
	CHORD_Count,
};

enum EnumChordRoot
{
	ROOT_A,
	ROOT_As,
	ROOT_B,
	ROOT_C,
	ROOT_Cs,
	ROOT_D,
	ROOT_Ds,
	ROOT_E,
	ROOT_F,
	ROOT_Fs,
	ROOT_G,
	ROOT_Gs,
	ROOT_Count, //12 of course
};



class CMyGenerator
{
public:
	CMyGenerator(void);
	~CMyGenerator(void);

	void			SetTuning(EnumTuning eTuning, BOOL bReload = TRUE);
	EnumTuning		GetTuning(){return m_eTuning;};
	void			SetRoot(EnumChordRoot eRoot, BOOL bReload = TRUE);
	EnumChordRoot	GetRoot(){return m_eRoot;};
	void			SetChord(EnumChordType eType, BOOL bReload = TRUE);
	EnumChordType	GetType(){return m_eType;};
	int				GetFret(int iString);
	TCHAR*			GetNote(int iString);
	int				GetNoteValue(int iString);//A = 0 and not played = -1

	void			NextChord(){m_iChordIndex++; if(m_iChordIndex >= NUM_CHORDS) m_iChordIndex = 0;};

	int				GetNumberofStrings(){return 6;};//we'll play with this later
	int				GetIndex(){return m_iChordIndex;};
	int				GetNumVariations(){return NUM_CHORDS;};

	BOOL			IsLeftHanded(){return m_bIsLeftHanded;};
	BOOL			NumberFrets(){return m_bNumberFrets;};
	void			SetLeftHanded(BOOL bLeftHanded);
	void			SetNumberFrets(BOOL bNum){m_bNumberFrets = bNum;};

	int				GetStartingFret();

	int				GetOpenNote(int iString);


private:
	BOOL			GenerateChords(EnumChordRoot eRoot, EnumChordType eType);
	BOOL			GenerateChords(TCHAR* szName);

private:
	CIssString*		m_oStr;

	Fretboard		m_oFretBoard;
	Chord			m_oChord;

	EnumTuning		m_eTuning;
	EnumChordRoot	m_eRoot;
	EnumChordType	m_eType;

	int				m_iChordIndex;

	BOOL			m_bIsLeftHanded;
	BOOL			m_bNumberFrets; 
};
