#pragma once

#include "stdafx.h"
#include "IssString.h"
#include "IssVector.h"

#define MAX_BPM 240
#define MIN_BPM  20

#define MAX_METER 12
#define MIN_METER 0 

#define MAX_SAVE_LOCATIONS 25

struct TypeMetronome
{
    int         iTempo;
    int         iMeter;
    TCHAR       szSongName[STRING_LARGE]; //we'll see
};

enum EnumClickSound
{
    CLICK_Loud,
    CLICK_Normal,
    CLICK_Soft, //???
};

class CMetronome
{
public:
    CMetronome(void);
    ~CMetronome(void);

    void        Start();
    void        Stop();

    //add more reference pitches ... also good for tuning I suppose ...
    void        A440(); //440hz tone (continuous ... ???)  (A and Bb). 
    BOOL        OnTempoUp();
    BOOL        OnTempoDown();

    BOOL        OnMeterUp();
    BOOL        OnMeterDown();

    BOOL        OnTap();//to set tempo

    BOOL        SetBPM(int iBPM);
    int         GetBPM();
    int         GetTimerTiming();//in MS

    BOOL        SetMeter(int iMeter);
    int         GetMeter();

    BOOL        AddToSetList(int iIndex);


    void        SetOnOff(BOOL bOn){m_bOn = bOn;};
    BOOL        GetOnOff(){return m_bOn;};
    void        ToggleOnOff(){m_bOn = !m_bOn;};

private:


private:
    CIssVector<TypeMetronome>   m_arrSetList;

    HWND        m_hWnd;//pretty sure we'll need this for the timer

    int         m_iBPM;
    int         m_iMeter;//might be an enum ...

    BOOL        m_bOn; //placeholder


};


//http://www.metronomeonline.com/
//http://www.ab-archive.com/graphics/screenshots/metronome-30209.jpg
//http://www.merrylandsmusic.com.au/images/DM33%20metronome.jpg

//look this one up
//http://www.woodbrass.com/images/woodbrass/METRONOME+BOSS+DR+BEAT+DB-60.JPG 



//WOOD BLOCK CLICK

//accented beats??

//40-208 BPM tempo range ... some go to 240

//multiple LEDs to show you where you are in the time ... ie sweep





//BIG LIST of Metronomes
//http://www.free-scores.com/boutique/accessories_603_shop_600.htm




/*

Dr Beat seems to be the top of the line
has pre-programmed beats so I'm guessing the tempo isn't constant

save settings in the form of a "Set List" (love it)

Korg MA30 looks like a great entry level ... match these features first
similiar to my tuner ... probably should buy


Tap Tempo (mode to tap and discover the tempo ... love it)

Beat accent 

Beat division (to hear quarter, triples, halfs, etc)

3 useful sound sets

display italian names

*/