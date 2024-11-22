#pragma once
#include "stdafx.h"
 
#define MAX_FRETS           7
#define FADE_OUT_SPEED      75
#define MAX_STRINGS         6

#define HIGHEST_SOUND_RES   IDR_WAV_GsVHi
#define LOWEST_SOUND_RES    IDR_WAV_CVLow//IDR_WAV_CLow
#define MIDDLE_A_RES        IDR_WAV_AMid

static int GetMaxFrets()
{
 /*   if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
        return 7;
    if(GetSystemMetrics(SM_CYSCREEN) == 480 && GetSystemMetrics(SM_CXSCREEN) == 320)
        return 7;*/
    return 5;
}