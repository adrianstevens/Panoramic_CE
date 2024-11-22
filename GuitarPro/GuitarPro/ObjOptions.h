#pragma once
#include "MyGenerator.h"
#include "FreqTuner.h"

enum EnumFretMarkers
{
    FMARKERS_Roman,
    FMARKERS_Numbered,
    FMARKERS_Count,
};

enum EnumStrum
{
    STRUM_Fast,
    STRUM_Slow,
    STRUM_Off, //so you can strum yourself
    STRUM_Count,
};

enum EnumMetroSounds
{
    METSND_Standard,
    METSND_Wood,
    METSND_Metal,
    METSND_Count,
};


class CObjOptions
{
public:
    CObjOptions(void);
    ~CObjOptions(void);

    void                Init(HINSTANCE hInst){m_hInst = hInst;};
    CMyGenerator*       GetGen();//gen for generator 

    EnumFretMarkers     GetFretMarkers(){return m_eFretmarkers;};
    void                SetFretMarkers(EnumFretMarkers eFret){m_eFretmarkers = eFret;};

//  moved to objgui
//    BOOL                GetPlaySounds(){return m_bPlaySounds;};
//    void                SetPlaySounds(BOOL bPlaySounds);//{m_bPlaySounds = bPlaySounds;};

    BOOL                GetLeftHanded(){return m_bLeftHanded;};
    void                SetLeftHanded(BOOL bLeft){m_bLeftHanded = bLeft;};

    EnumInstrument      GetInstrument(){return m_oChords.GetInstrument();};
    void                SetInstrument(EnumInstrument eInst);

    //Chords
    //yes you can go around this by callng GetGen but we'll do it for completeness
    EnumChordRoot       GetRoot(){return GetGen()->GetRoot();};
    void                SetRoot(EnumChordRoot eRoot, BOOL bReload = TRUE){GetGen()->SetRoot(eRoot, bReload);};

    EnumChordType       GetChordType(){return GetGen()->GetType();};
    void                SetChordType(EnumChordType eType, BOOL bReload = TRUE){GetGen()->SetChord(eType, bReload);};

//    EnumTuning          GetTuning(){return GetGen()->GetTuning();};
//    void                SetTuning(EnumTuning eTuning){GetGen()->SetTuning(eTuning);};
	int					GetTuning(){return GetGen()->GetTuning();};
	void                SetTuning(int iTuning){GetGen()->SetTuning(iTuning);};

    int                 GetTuning(EnumInstrument eInst){return GetGen()->GetTuning(eInst);};
    void                SetTuning(int iTuning, EnumInstrument eInst){GetGen()->SetTuning(iTuning, eInst);};

    //chords
    EnumStrum           GetStrumStyle(){return m_eStrum;};
    void                SetStrumStyle(EnumStrum eStrum){m_eStrum = eStrum;};

	BOOL				GetShowFingers(){return m_bShowFingers;};
	void				SetShowFingers(BOOL bShowFingers){m_bShowFingers = bShowFingers;};

    //play
    EnumScales          GetScale(){return GetGen()->GetScale();};
    void                SetScale(EnumScales eScale){GetGen()->SetScale(eScale);};

    //we're not going to set get the variation for now ....

    //Metronome
    EnumMetroSounds     GetMetroSound(){return m_eMetroSound;};
    void                SetMetroSound(EnumMetroSounds eSnd){m_eMetroSound = eSnd;};

    int                 GetMetroBPM(){return m_iMetroBPM;};
    void                SetMetroBPM(int iBPM){m_iMetroBPM = iBPM;};//we'll let the engine check for valid values

    int                 GetMetroMeter(){return m_iMetroMeter;};
    void                SetMetroMeter(int iMeter){m_iMetroMeter = iMeter;};


    //Digital Tuner
    EnumSampleFreqID    GetSampleFreq(){return m_eSampleFreq;};
    void                SetSampleFreq(EnumSampleFreqID eFreq){m_eSampleFreq = eFreq;};
    int                 GetiTrigger(){return m_iTrigger;};
    void                SetTrigger(int iTrig){m_iTrigger = iTrig;};


private:
    void                SetDefaults();

    //did this all in the dlg ... feel free to move it here 
    void                LoadRegistry();
    void                SaveRegistry();

public:
    

private:
    CMyGenerator		m_oChords;
    HINSTANCE           m_hInst;

    //skin
    //in ObjGUI for now ... not really problem

    //metronome
    EnumMetroSounds     m_eMetroSound;
    int                 m_iMetroBPM;
    int                 m_iMetroMeter;
    //BOOL                m_bPlayOnLostFocus;
    
    //general
    EnumFretMarkers     m_eFretmarkers;
    BOOL                m_bLeftHanded;
  //  BOOL                m_bPlaySounds; //might want to just see chords, use LEDs on Metronome, Tuner, etc

    //chords
    EnumStrum           m_eStrum;
	BOOL				m_bShowFingers;

    //Play

    
    //Tuner ... do these
    EnumSampleFreqID    m_eSampleFreq;
    int                 m_iTrigger; //trigger level ... between 0 & 32 inclusive
    //...tuning tolerance?

    //to get from chord class
    //capo
};
