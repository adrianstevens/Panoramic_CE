#pragma once

#include "stdafx.h"
#include "IssString.h"

//http://www.metronomeonline.com/
//http://www.ab-archive.com/graphics/screenshots/metronome-30209.jpg
//http://www.merrylandsmusic.com.au/images/DM33%20metronome.jpg

//look this one up
//http://www.woodbrass.com/images/woodbrass/METRONOME+BOSS+DR+BEAT+DB-60.JPG 



//WOOD BLOCK CLICK

//accented beats??

//40-208 BPM tempo range

//multiple LEDs to show you where you are in the time ... ie sweep



//BIG LIST of Metronomes
//http://www.free-scores.com/boutique/accessories_603_shop_600.htm

enum EnumClickSound
{
    CLICK_Loud,
    CLICK_Normal,
    CLICK_Soft //???
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
    void        Up();
    void        Down();


private:


private:
    HWND        m_hWnd;//pretty sure we'll need this for the timer

    BOOL        m_bPlaySound; //otherwise flash only
    BOOL        m_bClassic; //classic settings or full???? ... maybe GUI thing
    BOOL        m_bAccentedBeats; ///gotta look this up

    int         m_iTempo;
    int         m_iBeat; // ... maybe ...


};
