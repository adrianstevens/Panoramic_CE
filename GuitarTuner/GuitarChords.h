#pragma once
#include "IssVector.h"
#include "IssString.h"
#include "IssIniEx.h"

//since we'll never have anything this high
#define GUITAR_BLANK 255


//we'll keep this small
struct ChordType
{
    byte        btStartingFret;
    byte        btStringPlayed[6]; //open, not played, or fingered
    byte        btFingerPos[6]; //10 column indicates relative fret, 1 column indicates finger
    byte        btBridge1[3]; //fret, string1, string2
    byte        btBridge2[3];
    TCHAR       szNote[6][3];

    //total count 
    //1 + 6 + 6 + 3 + 3 + 6*3*2 (=36)
    // = 55 bytes per chord ... 
    // won't bother byte aligning since if figure the vectors through it off anyways
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
	CHORD_6over9,
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

extern TCHAR g_szFrets[12][4];
extern TCHAR g_szRoots[12][8];
extern TCHAR g_szChords[(int)CHORD_Count][STRING_NORMAL];


class CGuitarChords
{
public:
    CGuitarChords(void);
    ~CGuitarChords(void);

    EnumChordRoot       GetChordRoot(){return m_eChordRoot;};
    void                SetChordRoot(EnumChordRoot eRoot);

    EnumChordType       GetChordType(){return m_eChordType;};
    void                SetChordType(EnumChordType eType){m_eChordType = eType;};

    ChordType*          GetCurrentCord();
    int                 GetChordCount(EnumChordRoot eRoot, EnumChordType eType);

    void                NextChord();
    void                PrevChord();

    void                SetLeftHanded(BOOL bLeft){m_bLeft = bLeft;};
    BOOL                IsLeftHanded(){return m_bLeft;};

    int                 GetCurrentChordIndex(); //index per variation
    int                 GetNumVariations(); //variations of the same chord

private:
    BOOL                LoadChords(EnumChordRoot eRoot);
	void				Destroy();
    
private:
    
    //array of size of ROOTS of this array
    //heck why not???
    CIssVector<ChordType>  m_arrChords[ROOT_Count][CHORD_Count];

    CIssString*         m_oStr;

    CIssIniEx           m_oIni;

//    ChordType           m_oTestChord;

    EnumChordType       m_eChordType;
    EnumChordRoot       m_eChordRoot;

    int                 m_iChordIndex;

    BOOL                m_bLeft;

    bool                m_bInitialized[ROOT_Count];
};
