using System;
using System.Collections.Generic;
using System.Net;

namespace GuitarSuite
{
    public class ReverseChord
    {
        private List<int> _arrNotes = new List<int>();

        int iRoot;//first note added
        int iRootAlt;//most common note
        int iRootAltCount;//doesn't really need to be public but it'll be useful for degugging / testing
        int iNumNotes;
		
		bool bInversion;

        EnumChordType eChord = EnumChordType.CHORD_Count;

        public void Reset()
        {
            iNumNotes = 0;
            iRoot = -1;
            iRootAlt = - 1;
            iRootAltCount = 0;
            _arrNotes.Clear();
            _arrNotes.Capacity = 0;
            eChord = EnumChordType.CHORD_Count;
        }

        //for now add lowest to highest and we'll assume the first note is the root ... 
        public void AddNote(int iNote)
        {
            if (iNote > -1)//just makes it easier coming from the gui
            {
                _arrNotes.Capacity++;
                _arrNotes.Add(iNote);
            }
        }

        public bool GetChord(ref string szChord)
        {
            if (eChord == EnumChordType.CHORD_Count)
            {
                CalculateChord();

                if (eChord == EnumChordType.CHORD_Count)
                {
                    szChord = "";
                    return false;
                }
            }

            szChord = Globals.myGen.m_szRoots[iRoot] + " " + Globals.myGen.GetChordString((int)eChord);

            return true;
        }

        bool CalculateChord()
        {
			//string temp = "";

            if (iRoot < 0 && _arrNotes.Count > 0)
                iRoot = _arrNotes[0];
						
			iRootAlt = CalcMostCommon();
			
			int i = 0;

			while(iRootAlt == iRoot && i < _arrNotes.Count)
			{
				i++;
				if(i >= _arrNotes.Count)
					return false;

				iRootAlt = _arrNotes[i];

			}
			
			if(CalculateChord(iRoot) == true && eChord != EnumChordType.CHORD_Count)
				return true;

			if(iRootAlt != -1)
			{
				iRoot = iRootAlt;
				return CalculateChord(iRootAlt);//never gets called
			}

			return false;
        }

        bool CalculateChord(int iInputRoot)
        {
            //for now we'll go a little brute force and see what we can find ....
            //first things first ... find the most common note
            iNumNotes = GetNumUniqueNotes();
              
            if (IsInList((iInputRoot + 4) % 12) && IsInList((iInputRoot + 7) % 12))
            {
                eChord = EnumChordType.CHORD_maj;
                if (iNumNotes == 4)
                    Calc4NoteMaj(iInputRoot);
                else if (iNumNotes > 4)
                    CalcNotesOther(iInputRoot);
            }
            else if (IsInList((iInputRoot + 3) % 12) && IsInList((iInputRoot + 7) % 12))
            {
                 eChord = EnumChordType.CHORD_m;
                 if (iNumNotes == 4)
                     Calc4NoteMin(iInputRoot);
                 else if (iNumNotes > 4)
                     CalcNotesOther(iInputRoot);
            }
            else if (IsInList((iInputRoot + 3) % 12) && IsInList((iInputRoot + 6) % 12))
            {
                eChord = EnumChordType.CHORD_dim;
                if (iNumNotes > 3)
                    CalcNotesOther(iInputRoot);
            }
            else if (IsInList((iInputRoot + 4) % 12) && IsInList((iInputRoot + 8) % 12))
            {
                eChord = EnumChordType.CHORD_aug;
                if (iNumNotes > 4)
                    CalcNotesOther(iInputRoot);
            }
            else
            {
                CalcNotesOther(iInputRoot);
                if(eChord == EnumChordType.CHORD_Count && iInputRoot != iRootAlt && iRootAltCount > 1)
                    return CalculateChord(iRootAlt);
            }

  
            //if we end up using the alt root ... save it (probably doesn't matter really)
            if (iInputRoot == iRootAlt)
                iRoot = iRootAlt;

            return true;

        }

        //4 = 3rd //7 = 5th
        bool Calc4NoteMaj(int iInputRoot)
        {
            //assume we already have a major triad
            if (IsInList((iInputRoot + 9) % 12))
                eChord = EnumChordType.CHORD_6;

            else if (IsInList((iInputRoot + 10) % 12))
                eChord = EnumChordType.CHORD_7;

            else if (IsInList((iInputRoot + 11) % 12))
                eChord = EnumChordType.CHORD_maj7;
            //maj + 9 .. nice and simple
            else if (IsInList((iInputRoot + 2) % 12))
                eChord = EnumChordType.CHORD_add9;



            else
            {
                eChord = EnumChordType.CHORD_Count;
                return false;
            }

            return true;

        }

        
        bool Calc4NoteMin(int iInputRoot)
        {
            //assume we already have a minor triad ...

            if (IsInList((iInputRoot + 9) % 12))
                eChord = EnumChordType.CHORD_m6;

            else if (IsInList((iInputRoot + 10) % 12))
                eChord = EnumChordType.CHORD_m7;

            else if (IsInList((iInputRoot + 14) % 12))
                eChord = EnumChordType.CHORD_m9;

   //         else if (IsInList((iInputRoot + ) % 12))
   //             eChord = EnumChordType.CHORD_m11;

   //         else if (IsInList((iInputRoot + ) % 12))
   //             eChord = EnumChordType.CHORD_m13;

            else
            {
                eChord = EnumChordType.CHORD_Count;
                return false;
            }

            return true;
        }

       

        bool CalcNotesOther(int iInputRoot)
        {
            int r = iInputRoot;

            //maj9 ... maj7 + 9
            if (iNumNotes == 5 && eChord == EnumChordType.CHORD_maj &&
                IsInList((iInputRoot + 11) % 12) && //maj7
                IsInList((iInputRoot + 2) % 12) )//9
                eChord = EnumChordType.CHORD_maj9;

            //maj14 ... maj7 + 13
            else  if (iNumNotes == 5 && eChord == EnumChordType.CHORD_maj &&
                IsInList((iInputRoot + 11) % 12) &&//maj7
                IsInList((iInputRoot + 9) % 12) )//13
                eChord = EnumChordType.CHORD_maj13;

            //dominant 7th + 9  (minor 7th for dominant)
            else if (iNumNotes == 5 && eChord == EnumChordType.CHORD_maj &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 2) % 12) ) //9
                eChord = EnumChordType.CHORD_9;

            //dominant 7th + 11
            else if (iNumNotes == 5 && eChord == EnumChordType.CHORD_maj &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 5) % 12))//11
                eChord = EnumChordType.CHORD_11;

            //dominant 7th + 13
            else if (iNumNotes == 5 && eChord == EnumChordType.CHORD_maj &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 9) % 12))//13
                eChord = EnumChordType.CHORD_13;

            //minor 
            else if (iNumNotes == 5 && eChord == EnumChordType.CHORD_m &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 5) % 12))//11th
                eChord = EnumChordType.CHORD_m11;

            else if (iNumNotes == 5 && eChord == EnumChordType.CHORD_m &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 9) % 12))
                eChord = EnumChordType.CHORD_m13;

            //sus2
            else if (iNumNotes == 3 &&
                IsInList((r + 7) % 12) && //5th
                IsInList((r + 2) % 12)) //2nd
                eChord = EnumChordType.CHORD_sus2;

            //sus4
            else if (iNumNotes == 3 &&
                IsInList((r + 7) % 12) && //5th
                IsInList((r + 5) % 12)) //4th
                eChord = EnumChordType.CHORD_sus4;
                
            //7sus4
            else if (iNumNotes == 3 &&
                IsInList((r + 10) % 12) && //7th
                IsInList((r + 7) % 12) && //5th
                IsInList((r + 5) % 12)) //4th
                eChord = EnumChordType.CHORD_7sus4;

            //9sus4
            else if (iNumNotes == 3 &&
                IsInList((r + 2) % 12) && //9th
                IsInList((r + 7) % 12) && //5th
                IsInList((r + 5) % 12)) //4th
                eChord = EnumChordType.CHORD_9sus4;

            //aug9  ... later

            //7b5

            //7b9 

            //dim ... we'll return true for both the triad and the dim + 7
            else if(iNumNotes == 4 &&
                eChord == EnumChordType.CHORD_dim &&
                IsInList((r + 9)%12 ) )
                eChord = EnumChordType.CHORD_dim;
            else 
                eChord = EnumChordType.CHORD_Count;
            return false;
        }
            

        int CalcMostCommon()
        {
            int iCurCount;
            iRootAlt = -1;
            iRootAltCount = 0;

            for (int i = 0; i < 12; i++)
            {
                iCurCount = 0;

                for (int j = 0; j < _arrNotes.Count; j++)
                {
                    if (_arrNotes[j] == i)
                        iCurCount++;
                }

                if (iCurCount > 1)
                {
                    if (iCurCount > iRootAltCount)
                    {
                        iRootAlt = i;
                        iRootAltCount = iCurCount;
                    }
                }
            }
            return iRootAlt;
        }

        bool IsInList(int iValue)
        {
            if (_arrNotes.Contains(iValue))
                return true;

            return false;
        }

        int GetNumUniqueNotes()
        {
            int iCount = 0;

            for (int i = 0; i < 12; i++)
            {
                if (IsInList(i))
                    iCount++;
            }

            return iCount;
        }
    }
}
