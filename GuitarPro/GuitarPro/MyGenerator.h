#pragma once
#include "stdafx.h"
#include "chord-generator.h"
#include "IssString.h"
#include "ChordFavs.h"

#define SCALE_SIZE 16
#define NOTE_NOT_DEF 255

#define NUM_CHORDS	10 //looks good ... seems cool
/*
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
};*/

class CMyGenerator
{
public:
	CMyGenerator();
	~CMyGenerator(void);

    void            Init(HINSTANCE hInst);
//	void			SetTuning(EnumTuning eTuning, BOOL bReload = TRUE);
//	EnumTuning		GetTuning(){return m_eTuning;};
	void            SetTuning(int iIndex, BOOL bReload = TRUE);
    void            SetTuning(int iIndex, EnumInstrument eInst);
    int				GetTuning(EnumInstrument eInst);
    int				GetTuning();
	static int		GetNumTuning(EnumInstrument eInst);
    int		        GetNumTuning(){return GetNumTuning(GetInstrument());};
	void			SetRoot(EnumChordRoot eRoot, BOOL bReload = TRUE);
	EnumChordRoot	GetRoot(){return m_eRoot;};
	void			SetChord(EnumChordType eType, BOOL bReload = TRUE);
	EnumChordType	GetType(){return m_eType;};
	int				GetFret(int iString);
	TCHAR*			GetNote(int iString);
    TCHAR*			GetNote(int iString, int iFret);
    int             GetNoteValue(int iString, int iFret);//get the 0-11 value
	int				GetNoteValue(int iString);//A = 0 and not played = -1
    EnumInstrument  GetInstrument(){return m_eInst;};
    void            SetInstrument(EnumInstrument eInst, BOOL bReload = TRUE);

	int				GetFinger(int iString);

	void			NextChord(){m_iChordIndex++; if(m_iChordIndex >= NUM_CHORDS) m_iChordIndex = 0;};
    void			PrevChord(){m_iChordIndex--; if(m_iChordIndex < 0) m_iChordIndex = NUM_CHORDS - 1;};

	int				GetNumberofStrings(){return m_oFretBoard.getNumStrings();};//we'll play with this later
	int				GetIndex(){return m_iChordIndex;};
	int				GetNumVariations(){return NUM_CHORDS;};

	BOOL			IsLeftHanded(){return m_bIsLeftHanded;};
	BOOL			NumberFrets(){return m_bNumberFrets;};
	void			SetLeftHanded(BOOL bLeftHanded);
	void			SetNumberFrets(BOOL bNum){m_bNumberFrets = bNum;};

	int				GetStartingFret();

	int				GetOpenNote(int iString);

    EnumScales      GetScale(){return m_eScale;};
    void            SetScale(EnumScales eScale);

    byte            ScaleToStep(int iScaleNote, bool bSharp, bool bFlat);

    byte            GetScaleValue(int iIndex){return m_btCurrentScale[iIndex];};
    int             GetScaleLength();


    static TCHAR*   GetChordText(int iIndex, HINSTANCE hInst);
    static TCHAR*   GetScalesText(int iIndex, HINSTANCE hInst);
    static TCHAR*   GetTuningText(int iIndex, HINSTANCE hInst, EnumInstrument eInst);      
    TCHAR*          GetTuningLongText(int iIndex, HINSTANCE hInst){return GetTuningLongText(iIndex, hInst, GetInstrument());};   
    TCHAR*          GetTuningLongText(int iIndex, HINSTANCE hInst, EnumInstrument eInst);  //was static ... can't remember why .... 

    UINT            GetNoteRes(int iString);//we'll us this one primarily
    UINT            GetNoteRes(int iString, int iFret);

    //favorites 
    BOOL            OnFavorites();
    BOOL            IsStarLit();
    int             GetFavCount(){return m_oFav.GetFavCount();};

    EnumChordRoot   GetFavRoot(int iFav){return m_oFav.GetRoot(iFav);};
    EnumChordType   GetFavType(int iFav){return m_oFav.GetType(iFav);};
    EnumChordRoot   GetFavNoteIndex(int iFav, int iString){return m_oFav.GetNoteIndex(iFav, iString);};
    
    int             GetNumStrings();

private:
    void            SetTuningB4(int iIndex, BOOL bReload = TRUE);
    void            SetTuningB5(int iIndex, BOOL bReload = TRUE);
    void            SetTuningBanjo(int iIndex, BOOL bReload = TRUE);
    void            SetTuningLute(int iIndex, BOOL bReload = TRUE);
	void			SetTuningUkulele(int iIndex, BOOL bReload = TRUE);

    static TCHAR*   GetTuningTextGuitar(int iIndex, HINSTANCE hInst);      
    static TCHAR*   GetTuningLongTextGuitar(int iIndex, HINSTANCE hInst);   

    static TCHAR*   GetTuningTextB4(int iIndex, HINSTANCE hInst);      
    static TCHAR*   GetTuningLongTextB4(int iIndex, HINSTANCE hInst);   

    static TCHAR*   GetTuningTextB5(int iIndex, HINSTANCE hInst);      
    static TCHAR*   GetTuningLongTextB5(int iIndex, HINSTANCE hInst);   

    static TCHAR*   GetTuningTextBanjo(int iIndex, HINSTANCE hInst);      
    static TCHAR*   GetTuningLongTextBanjo(int iIndex, HINSTANCE hInst);   

    static TCHAR*   GetTuningTextLute(int iIndex, HINSTANCE hInst);      
    static TCHAR*   GetTuningLongTextLute(int iIndex, HINSTANCE hInst);  

	static TCHAR*	GetTuningTextUkulele(int iIndex, HINSTANCE hInst);
	static TCHAR*	GetTuningLongTextUkulele(int iIndex, HINSTANCE hInst);

 
	BOOL			GenerateChords(EnumChordRoot eRoot, EnumChordType eType);
	BOOL			GenerateChords(TCHAR* szName);

	BOOL			RaiseNote(int iString);//do we raise by an octive

private:
	CIssString*		m_oStr;
    HINSTANCE       m_hInst;

    CChordFavs      m_oFav;//saves and restores chords 
	Fretboard		m_oFretBoard;
	Chord			m_oChord;

	EnumTuning		m_eTuning;
    EnumTuningBass4 m_eTuningB4;
    EnumTuningBass5 m_eTuningB5;        //not used yet
    EnumTuningBanjo m_eTuningBanjo;     //not used yet
    EnumTuningLute  m_eTuningLute;      
	EnumTuningUkulele m_eTuningUkulele;


	EnumChordRoot	m_eRoot;
	EnumChordType	m_eType;

    EnumInstrument  m_eInst;

    EnumScales      m_eScale;//doesn't effect the other parts but I might as well have it in the generator

	int				m_iChordIndex;

	BOOL			m_bIsLeftHanded;
	BOOL			m_bNumberFrets; 

    byte            m_btCurrentScale[SCALE_SIZE];//WAY bigger than needed 
};
