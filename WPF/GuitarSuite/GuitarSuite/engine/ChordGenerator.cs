using System;
using System.Collections.Generic;
using System.Text;

    public class ChordGenerator
    {
        enum EnumChords { NORMAL, MAJ, ADD, AUG, DIM };

        //defines
        public static int NUMSTRINGS_SCORE = 9;
        public static int LOW_SCOUNT_PENALTY = -50;
        public static int NUM_STRINGS = 6;

        public int[] openStrings = new int[6];// = { 7, 0, 5, 10, 2, 7 };
        public string[] strNotes = { "A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#" };

        //public
        public string[] errorStr = new string[64];
        public int[] notes = new int[12];
        public int[] optional = new int[12];


        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }

        void clear()
        {
            for (int i = 0; i < 12; i++)
            {
                notes[i] = -1;
                optional[i] = 0;
            }
        }

        void printError()
        {
            //might be useful so we'll leave the function in 
        }

        void myStrupr(ref string s)
        {
            //this just converts the string to upper case ... we don't need to do this
        }

        public int findChord(ref string s)
        {
            clear();

            myStrupr(ref s);


			//bool bFlat = false; //for 7b5, 7b9 etc .... see 2nd digit handling
            int iS = 0;
            int iLen = s.Length;

            // DECODE ROOT NOTE : A - G
            notes[0] = get_base(s);
            iS++;

            // CHECK FOR SHARP OR FLAT ROOT NOTE
            if (s[iS] == '#') iS++;
            if (s[iS] == 'b') iS++;
		
			if (s[iS] == '♯') iS++;
            if (s[iS] == '♭') iS++;


            // MODIFY THE ROOT BY M, MIN, SUS2, SUS4, or diminished
            //if (!strncmp(s, "MIN", 3))
            if (s.Length > iS + 2 &&
                s[iS] == 'm' &&
                s[iS + 1] == 'i' &&
                s[iS + 2] == 'n')
            {
                notes[1] = note_offset(notes[0], 3);
                iS += 3;
                optional[2] = 1;
            }
            //else if (!strncmp(s, "MAJ", 3))
            else if (s.Length > iS + 2 && 
                s[iS] == 'm' &&
                s[iS + 1] == 'a' &&
                s[iS + 2] == 'j')
            {
                // Do nothing, but stops program from seeing the
                // first m in maj as a minor (see next line)...so give a normal 3rd
                notes[1] = note_offset(notes[0], 4);
                optional[2] = 1;
            }
            else if (s[iS] == 'm')
            {
                notes[1] = note_offset(notes[0], 3);
                iS += 1;
                optional[2] = 1;
            }//here's the sus
            else if (s[iS] == 's' &&
                    s[iS + 1] == 'u' &&
                    s[iS + 2] == 's')
            {
                iS += 3;   // go past sus
                if (s[iS] == '2')
                    notes[1] = note_offset(notes[0], 2);
                else if (s[iS] == '4')
                    notes[1] = note_offset(notes[0], 5);
                else
                {
                    //strcpy(errorStr, "sus must be followed by 2 or 4");
                    return 1;
                }
                iS++;  // Go past 2 or 4
                optional[2] = 1;
            }
            //else if ((!strncmp(s, "DIM", 3)) && (!isdigit(s[3])))
            //supposed to check for another number but I'm too lazy ...
            else if (s[iS] == 'd' &&
                s[iS + 1] == 'i' &&
                s[iS + 2] == 'm')
            {
                // If it is diminished, just return (no other stuff allowed)/
                notes[1] = note_offset(notes[0], 3);
                notes[2] = note_offset(notes[0], 6);
                notes[3] = note_offset(notes[0], 9);
                return 0;
            }
            //else if ((!strncmp(s, "AUG", 3)) && (!isdigit(s[3])))
            else if (s[iS] == 'a' &&
                    s[iS + 1] == 'u' &&
                    s[iS + 2] == 'g')
            {
                // If it is augmented, just return (no other stuff allowed)/
                notes[1] = note_offset(notes[0], 4);
                notes[2] = note_offset(notes[0], 8);
                return 0;
            }
			else if(s[iS] == '5')
			{
				notes[1] = note_offset(notes[0], 7); //5th only
				return 0;
			}
            else
            {
                notes[1] = note_offset(notes[0], 4);
                //  optional[1] = 1;
                //  optional[2] = 1; .. commented out in the c++ code ... just leave it 
            }

            notes[2] = note_offset(notes[0], 7);

            // At this point, the 1,3,5 triad or variant is built, now add onto
	        //    it until the string end is reached...
	        // Next note to add is index = 3...
	        int index = 3;
            EnumChords eType;
            string lbuf = "";

            while(iS < iLen)
            {
                eType = EnumChords.NORMAL;

                if (s[iS] == 'm' &&
                s[iS + 1] == 'a' &&
                s[iS + 2] == 'j')
                {
                    eType = EnumChords.MAJ;
                    iS += 3;
                }
                else if (s[iS] == 'a' &&
                    s[iS + 1] == 'd' &&
                    s[iS + 2] == 'd')
                {
                    eType = EnumChords.ADD;
                    iS += 3;
                }
                else if (s[iS] == 'a' &&
                      s[iS + 1] == 'u' &&
                      s[iS + 2] == 'g')
                {
                    eType = EnumChords.AUG;
                    iS += 3;
                }
                else if (s[iS] == 'd' &&
                      s[iS + 1] == 'i' &&
                      s[iS + 2] == 'm')
                {
                    eType = EnumChords.DIM;
                    iS += 3;
                }
                else if (s[iS] == '+')
                {
                    eType = EnumChords.AUG;
                    iS++;
                }
                else if (s[iS] == '-')
                {
                    eType = EnumChords.DIM;
                    iS++;
                }
                else if (s[iS] == '#' || s[iS] == '♯')
                {
                    eType = EnumChords.AUG;
                    iS++;
                }
                else if (s[iS] == 'b' || s[iS] == '♭')
                {
                    eType = EnumChords.DIM;
                    iS++;
                }
                else if (s[iS] == '/')
                {
                    eType = EnumChords.ADD;
                    iS++;
                }

                // Now find the number...
				// 11th chord
				/*if(iS < s.Length - 1 &&
			  	 	s[iS] == '1' && 
			   		s[iS] == '1')
				{
					
					iS += 2;
					notes[3] = note_offset(notes[0], 10); //m7th 
					notes[4] = note_offset(notes[0], 2); //9th (2nd)
					notes[5] = note_offset(notes[0], 5); //11th (4th)
					return 0;
				}

    		    else */

				if(iS < s.Length && s[iS] >= '0' && s[iS] <= '9')
                {
                    lbuf += s[iS];
                    iS++;
                }
                else
                {
                    return 1;
                }
    
			   // 2nd digit?
			    if (iS < s.Length && s[iS] >= '0' && s[iS] <= '9')
                {
                    lbuf += s[iS];
                    iS++;
                }
			     
				int number = int.Parse(lbuf);

                switch (number)
                {
                    case 7:
						if(eType == EnumChords.MAJ)
							notes[index] = note_offset(notes[0], 11);//major 7
						else
                        	notes[index] = note_offset(notes[0], 10);//minor 7
						index++;
                        break;
                    case 9:
                        notes[index] = note_offset(notes[0], 2);

                        // put the 7th in 2nd so it can be maj'ed if need be...
                        if ((eType == EnumChords.NORMAL) || (eType == EnumChords.MAJ))
                        {
                            index++;
                            notes[index] = note_offset(notes[0], 10);
                            optional[index] = 1;  // 7th is optional, unless it is maj!
                        }

                        break;
                    case 11:
                        notes[index] = note_offset(notes[0], 5);

                        // put the 7th in 2nd so it can be maj'ed if need be...
                        if ((eType == EnumChords.NORMAL) || (eType == EnumChords.MAJ))
                        {
                            index++;
                            notes[index] = note_offset(notes[0], 10);
                            optional[index] = 1;  // 7th is optional, unless it is  maj!
                        }

                        break;
                    case 13:
                        notes[index] = note_offset(notes[0], 9);
                        index++;
                        notes[index] = note_offset(notes[0], 5);
                        optional[index] = 1;  // 7th is optional, unless it is  maj!
                        index++;
                        notes[index] = note_offset(notes[0], 2);
                        optional[index] = 1;  // 7th is optional, unless it is  maj!

                        // put the 7th in 2nd so it can be maj'ed if need be...
                        if ((eType == EnumChords.NORMAL) || (eType == EnumChords.MAJ))
                        {
                            index++;
                            notes[index] = note_offset(notes[0], 10);
                            optional[index] = 1;  // 7th is optional, unless it is maj!
                        }

                        break;
                    case 2:
                        notes[index] = note_offset(notes[0], 2);
                        break;
                    case 4:
                        notes[index] = note_offset(notes[0], 5);
                        break;
                    case 6:
                        notes[index] = note_offset(notes[0], 9);
                        break;
                    case 5:
				        notes[index] = note_offset(notes[0], 7);
                        break;
                    default:
                        //sprintf(errorStr, "Cannot do number: %d\n", number);
                        return 1;
                }

				//and a little hack shit for the b5 and b9
				//I think this could be fixed properly above 
				if(iS < s.Length &&
			   		(s[iS] == 'b' || s[iS] == '♭') )
				{	//only handling 5 & 9 for now but could easily add more
					iS++;
					if(iS < s.Length)
					{
						if(s[iS] == '5')
							notes[2] = note_offset(notes[0], 6);//flatted 5th ... overwrite the perfect 5th
						else if(s[iS] == '9')
						notes[index] = note_offset(notes[0], 1);//flatted 9th
					}
					iS++;//doesn't really matter now ....
				}
				else if(iS < s.Length &&
				         (s[iS] == '#' || s[iS] == '♯') )
				{	//only handling 5 & 9 for now but could easily add more
					iS++;
					if(iS < s.Length)
					{
						if(s[iS] == '5')
							notes[2] = note_offset(notes[0], 8);//sharped 5th ... overwrite the perfect 5th
						else if(s[iS] == '9')
							notes[index] = note_offset(notes[0], 3);//sharped 9th ... don't think its used
					}
					iS++;//doesn't really matter now ....
				}



                switch (eType)
                {
                    case EnumChords.DIM:
                        notes[index] = note_offset(notes[index], -1);
                        break;
                    case EnumChords.MAJ:
                        // It is a major, so not optional
                        optional[index] = 0;
                        notes[index] = note_offset(notes[index], 1);//so lame .. no fall through
                        break;
                    case EnumChords.AUG:
                        notes[index] = note_offset(notes[index], 1);
                        break;
                    case EnumChords.NORMAL:
                        break;
                    case EnumChords.ADD:
                        break;
                    default:
                        break;
                }
                index++;
            }
            return 0;
        }

        void print(string[] cname)
        {

        }

        public int covered(ref int[] noteArray)
        {
	        // noteArray is an array of notes this chord has, it is NUM_STRINGS notes
	        // long (like a guitar fretboard dude...unused notes may be set
	        // to -1 (which wont compare since -1 is tossed...
	        for (int i = 0; i < 12; i++)
	        {
		        if (notes[i] != -1)
		        {
			        int gotIt = 0;
			        for (int j = 0; j < NUM_STRINGS; j++)
			        {
				        if (noteArray[j] == notes[i])
				        {
					        gotIt = 1;
					        break;
				        }
			        }
			        // If it was not found, and it is NOT optional, then it is
			        // not covered
			        if ((gotIt == 0) && (optional[i] == 0))
				        return 0;
		        }
	        }
	        return 1;
        }

        public int inChord(int note)
        {
            for (int i = 0; i < 12; i++)
            {
                // Check if we are off the end of the notes set
                if (notes[i] == -1)
                    return 0;
                // Check if the note was found
                if (note == notes[i])
                    return 1;
            }
            // Did not find out, return 0
            return 0;
        }

        public int get_base(string s)
        {
            int[] halfsteps = { 0, 2, 3, 5, 7, 8, 10 };

           	if ( (s[0] < 'A') || (s[0] > 'G') )
		        return 0;

	        if (s[1] == '#' || s[1] == '♯')
		        return ( note_offset(halfsteps[s[0] - 'A'], 1));
	        else if (s[1] == 'B' || s[1] == 'b' || s[1] == '♭')
		        return ( note_offset(halfsteps[s[0] - 'A'], -1));
	        else
		        return ( halfsteps[s[0] - 'A']);
        }

        // Get a note offset from another...
	    public int note_offset(int iBase, int offset) 
        {
            return ((iBase + offset) % 12); 
        }

        // Return the root note of the chord
        public int getRoot() { return notes[0]; }
    }

