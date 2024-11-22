#pragma once
#include "globals.h"
#include "IssVector.h"
//#include "MyGenerator.h"


enum EnumScales
{
    SCALE_Major,
    SCALE_HarmonicMinor,
    SCALE_MelodicMinorAscending,
    SCALE_MelodicMinorDescending,
    SCALE_PentatonicMajor,
    SCALE_PentatonicMinor,
    SCALE_PentatonicBlues,
    SCALE_PentatonicNeutral,
    //SCALE_Ionian, - same as major
    SCALE_Dorian,
    SCALE_Phrygian,
    SCALE_Lydian,
    SCALE_Mixolydian,
    SCALE_Aeolian,
    SCALE_Locrian,
    SCALE_HalfDiminished,
    SCALE_WholeDiminished,
    SCALE_Whole,
    SCALE_Augmented,
    SCALE_Chromatic,
    SCALE_RoumanianMinor,
    SCALE_SpanishGypsy,
    SCALE_Blues,
    SCALE_DoubleHarmonic,
    SCALE_EightToneSpanish,
    SCALE_Enigmatic,
    SCALE_LeadingWholeTone,
    SCALE_LydianAugmented,
    SCALE_NeopolitanMajor,
    SCALE_NeopolitanMinor,
    SCALE_Pelog,
    SCALE_Prometheus,
    SCALE_PrometheusNeopolitan,
    SCALE_SixToneSymmetrical,
    SCALE_SuperLocrian,
    SCALE_LydianMinor,
    SCALE_LydianDiminished,
    SCALE_NineToneScale,
    SCALE_AuxiliaryDiminished,
    SCALE_AuxiliaryAugmented,
    SCALE_AuxiliaryDiminishedBlues,
    SCALE_MajorLocrian,
    SCALE_Overtone,
    SCALE_DiminishedWholeTone,
    SCALE_Scale,
    SCALE_Count,
};

enum EnumTuning
{
    TUNING_Standard,
    TUNING_Drop,
    TUNING_DropD,
    TUNING_DropDAlt,
    TUNING_OpenD,
    TUNING_OpenDm,
    TUNING_DModal,
    TUNING_Fourths,
    TUNING_OpenE,
    TUNING_ShiftedE,
    TUNING_G6,
    TUNING_OpenG,
    TUNING_Gm,
    TUNING_OpenA,
    TUNING_Am,
    TUNING_C6,
    TUNING_OpenC,
    TUNING_LowC,
    TUNING_Count,
};


//thinking one tuning enum per instrument
enum EnumTuningBass5
{
	TUNINGB5_Standard,
	TUNINGB5_DropD,
	TUNINGB5_HiC,
	TUNINGB5_HalfDown,
	TUNINGB5_FullDown,
	TUNINGB5_HalfDownHi,
	TUNINGB5_FullDownHi,
	TUNINGB5_Count,
};

enum EnumTuningBass4
{
	TUNINGB4_Standard,  //E A D G
	TUNINGB4_DropD,     //G#D#C#F#
    TUNINGB4_HalfDown,  //EbAbDbGb
	TUNINGB4_LowB,      //B E A D
	TUNINGB4_DropC,     //C A D G
	TUNINGB4_Count,
};

enum EnumTuningBanjo
{
    TUNINGBANJO_G,//GDGBD        
    TUNINGBANJO_GModal,//GDGCD        
    TUNINGBANJO_DoubleC,//GCGCD        
    TUNINGBANJO_C,//GDGDB        
    TUNINGBANJO_D,//DAF#AD
    TUNINGBANJO_DAlt,//F#DF#AD
    TUNINGBANJO_Guitar,//ADGBE        
    TUNINGBANJO_Count,
};

enum EnumTuningLute
{
    TUNINGLUTE_Mandolin,    //G D E A
    TUNINGLUTE_Mandola,     //C G D A
    TUNINGLUTE_Pipa,        //D A D A
    TUNINGLUTE_Bouzouki,    //C F A D
    TUNINGLUTE_Bouzouki2,   //G D A D
    TUNINGLUTE_Bouzouki3,   //A D E A
    TUNINGLUTE_Count,
};

enum EnumTuningUkulele
{
	TUNINGUKU_RentrantC,//gCEA
	TUNINGUKU_C,//GCEA
	TUNINGUKU_G,//dGBA
	TUNINGUKU_D,//ADF#B
	TUNINGUKU_B,
	TUNINGUKU_Cs,
	TUNINGUKU_Guitar,//DGBE
	TUNINGUKU_SlackKey,//gCEG
	TUNINGUKU_Count,
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

enum EnumInstrument
{
    INST_Guitar,
//    INST_5StringBass,
    INST_4StringBass,
	INST_Banjo,
	INST_Ukulele,
	INST_Lute,
    INST_count,
};


struct TypeFavorite
{   //or maybe we save specific information .....
    EnumChordRoot   eRoot;
    EnumChordType   eType;
    EnumTuning      eTuning;
    int             iIndex;
    UINT            uiRes[6];
};

class CChordFavs
{
public:
    CChordFavs(void);
    ~CChordFavs(void);

    int             GetFavCount(){return m_arrFavorites.GetSize();};
    TypeFavorite*   GetFavorite(int iIndex){return m_arrFavorites[iIndex];};

   // BOOL            AddFavorite(EnumChordRoot eRoot, EnumChordType eType, EnumTuning eTuning, int iIndex);
    BOOL            AddFavorite(EnumChordRoot eRoot, EnumChordType eType, EnumTuning eTuning, int iIndex,
                                UINT uiRes1, UINT uiRes2, UINT uiRes3, UINT uiRes4, UINT uiRes5, UINT uiRes6);

    BOOL            IsFavorite(EnumChordRoot eRoot, EnumChordType eType, EnumTuning eTuning, int iIndex);

    BOOL            RemoveFavorite(int iIndex);

    UINT            GetRes(int iFav, int iString);
    EnumChordRoot   GetNoteIndex(int iFav, int iString);
    EnumChordRoot   GetRoot(int iFav);
    EnumChordType   GetType(int iFav);

    BOOL            DeleteLastFav();


private:

private:
    CIssVector<TypeFavorite>        m_arrFavorites;

    int             m_iLastFound;

};
