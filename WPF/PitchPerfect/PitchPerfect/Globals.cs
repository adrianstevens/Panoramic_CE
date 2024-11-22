using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PitchPerfect
{
    class Globals
    {


        public static string GetWave(int iIndex)
        {
            return "./Assets/Piano/" + waves[iIndex];
        }

        public static int GetNoteRes(int Note, int Octave)
        {
            //lowest note is a C (perfect)
            int AMid = 21; //from Unity Array
            //int AHi = 33;
            int HIGHEST_SOUND_RES = 56;


            int uiRet = -1;

            uiRet = Note + ((Octave - 4)*12)  + AMid;//might have to tweak that octave correction ... 

            //now check ranges
            while (uiRet < 0)
                uiRet += 12;
            while (uiRet > HIGHEST_SOUND_RES)
                uiRet -= 12;
           

            return uiRet;
        }

        public static string[] waves = {"c-vlow.wav", 
                                        "cs-vlow.wav", 
                                        "d-vlow.wav", 
                                        "ds-vlow.wav", 
                                        "e-vlow.wav", 
                                        "f-vlow.wav", 
                                        "fs-vlow.wav", 
                                        "g-vlow.wav", 
                                        "gs-vlow.wav", 

                                        "a-low.wav", 
                                        "as-low.wav", 
                                        "b-low.wav", 
                                        "c-low.wav", 
                                        "cs-low.wav", 
                                        "d-low.wav", 
                                        "ds-low.wav", 
                                        "e-low.wav", 
                                        "f-low.wav", 
                                        "fs-low.wav", 
                                        "g-low.wav", 
                                        "gs-low.wav", 

                                        "a-mid.wav", 
                                        "as-mid.wav", 
                                        "b-mid.wav", 
                                        "c-mid.wav", 
                                        "cs-mid.wav", 
                                        "d-mid.wav", 
                                        "ds-mid.wav", 
                                        "e-mid.wav", 
                                        "f-mid.wav", 
                                        "fs-mid.wav", 
                                        "g-mid.wav", 
                                        "gs-mid.wav",

                                        "a-hi.wav", 
                                        "as-hi.wav", 
                                        "b-hi.wav", 
                                        "c-hi.wav", 
                                        "cs-hi.wav", 
                                        "d-hi.wav", 
                                        "ds-hi.wav", 
                                        "e-hi.wav", 
                                        "f-hi.wav", 
                                        "fs-hi.wav", 
                                        "g-hi.wav", 
                                        "gs-hi.wav",

                                        "a-vhi.wav", 
                                        "as-vhi.wav", 
                                        "b-vhi.wav", 
                                        "c-vhi.wav", 
                                        "cs-vhi.wav", 
                                        "d-vhi.wav", 
                                        "ds-vhi.wav", 
                                        "e-vhi.wav", 
                                        "f-vhi.wav", 
                                        "fs-vhi.wav", 
                                        "g-vhi.wav", 
                                        "gs-vhi.wav",
                                    };
    }
    }
