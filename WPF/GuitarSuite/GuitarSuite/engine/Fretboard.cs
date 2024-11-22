using System;
using System.Collections.Generic;
using System.Text;


    public class Fretboard
    {
        public static int NUMSCORETYPES = 8;
        public static int NUM_STRINGS = 6;
        public static int CHORDSTACK = 120;

		public static int PENALTY_LOW_SCOUNT = -100; //if we're using less than 3 notes 
		public static int PENALTY_TOOMANYFINGERS = -100;

        public static int  MAXSPAN 					= 4;
		public static int  MAXUNIQUEFRETS 			= 4;

		// should be an enum
        public static int  SCORE_TOTAL              = 0;
        public static int  SCORE_SPAN               = 1;
        public static int  SCORE_TOTALSPAN          = 2;
        public static int  SCORE_OPENSTRINGS        = 3;
        public static int  SCORE_FIRSTROOT          = 4;
        public static int  SCORE_LOWONNECK          = 5;
        public static int  SCORE_ADJNOTES           = 6;
        public static int  SCORE_NUMNOTES           = 7;
		

		// end

		public static int  SCORE_NUMSTRINGS			= 9;

        public int[] openStrings = new int[]{ 7, 0, 5, 10, 2, 7 };
            

        // Fretboard of the current chord, fretboard[0] is the low (bass) E
	    // string.  A fretboard value of 0 is an open string, -1 is an
	    // X'ed out string (not strung)
	    public int[] fretboard = new int[NUM_STRINGS];

	    // Score of current chord, held in an array so we can track the
	    // components of the score (see the SCORE_ defines in the beginning
	    // of this file
	    public int[] score = new int[NUMSCORETYPES];

	    // Notes of the current fretboard fingering for a chord
	    public int[] notes = new int[NUM_STRINGS];

	    // The best fret layouts so far based on score
	    public int[,] bestFrets = new int[CHORDSTACK,NUM_STRINGS];

	    // The best fret layout note sets so far based on score
	    public int[,] bestNotes = new int[CHORDSTACK,NUM_STRINGS];

	    // The best scores
	    public int[,] bestScores = new int[CHORDSTACK,NUMSCORETYPES];

	    // Keep track of stack sDepth to speed it up!
	    public int sDepth;

        //since we're not going to change the define (too much work ... )
        public int iNumStrings;

        //private
        int iScorePos;
        int iScoreSpan;
        //int iScoreTSpan;
        int iScoreOpen;
        int iScoreRoot;
        int iScoreAdjacent;
        //int iScoreUsedStrings;


        void Start()
        {
            Reset();
        }
        
        void print(string cname)
        {

        }

        int getSpan()
        {
            int min = 100, max = 0;
            for (int i = 0; i < getNumStrings(); i++)
            {
                // Don't count X strings or open strings
                if (fretboard[i] <= 0)
                    continue;
                if (fretboard[i] > max) max = fretboard[i];
                if (fretboard[i] < min) min = fretboard[i];
            }
            if (min == 100)
                // All open strings, took awhile to catch this bug
                return 0;
            else
                return (max - min);


        }

        //
        // Get the span of this chord, DO count open strings
        //
        int getTotalSpan()
        {
	        int min = 100, max = 0;
	        for (int i = 0; i < getNumStrings(); i++)
	        {
		        // Don't count X strings
		        if (fretboard[i] < 0)
			        continue;
		        if (fretboard[i] > max) max = fretboard[i];
		        if (fretboard[i] < min) min = fretboard[i];
	        }
	        if (min == -1)
		        min = 0;
	        return (max - min);
        }

        
        //
        // Add this chord to the best (if there is room in the stack)
        //
        void addToBest()
        {
	        // CHORDSTACK is the sDepth of keepers...
            /*
            #ifdef DEBUG
	        printf("ATB: ");
	        this->print();
            #endif
		    */

	        int i;
        	
	        // NOTE: at the start, bestScores is full of -1's, so any reall
	        // real score will be better (worst score is 0)
	        for (i = 0; i < sDepth; i++)
	        {
		        if (score[0] > bestScores[i,0])
			        break;
	        }

	        // If score was not better than any in the stack just return
	        if (i >= CHORDSTACK)
		        return ;
	        // MOve down old guys to make room for the new guy
	        //for (int j = CHORDSTACK - 1; j >= i; j--)
            for (int j = CHORDSTACK - 1; j > i; j--)
	        {
		        for (int q = 0; q < getNumStrings(); q++)
		        {
			        bestFrets[j,q] = bestFrets[j - 1,q];
			        bestNotes[j,q] = bestNotes[j - 1,q];
		        }
		        for (int q = 0; q < NUMSCORETYPES; q++)
			        bestScores[j,q] = bestScores[j - 1,q];
	        }

	        for (int q = 0; q < getNumStrings(); q++)
	        {
		        bestFrets[i,q] = fretboard[q];
		        bestNotes[i,q] = notes[q];
	        }
	        for (int q = 0; q < NUMSCORETYPES; q++)
		        bestScores[i,q] = score[q];

	        sDepth++;
	        if (sDepth > CHORDSTACK)
		        sDepth--;
        }
    

        void printStack()
        {

        }

        public void SetScoreVars(bool bBase)
        {
	        if(bBase)
	        {
		        iScorePos			= 10;
		        iScoreSpan			= 19;
		        //iScoreTSpan			= 3;
		        iScoreOpen			= 12;
		        iScoreRoot			= 50;
		        iScoreAdjacent		= 5;
		       // iScoreUsedStrings	= 0;
	        }
	        else
	        {
		        iScorePos			= 2;
		        iScoreSpan			= 19;
		        //iScoreTSpan			= 3;
		        iScoreOpen			= 10;
		        iScoreRoot			= 50;
		        iScoreAdjacent		= 9;
		        //iScoreUsedStrings	= 0;
	        }
        }



        //
        // Get the score for this chord
        //
        public void getScore(ref ChordGenerator chord)
        {
	        // First, points for small span (excluding opens)
	        score[SCORE_SPAN] = (MAXSPAN - getSpan()) * iScoreSpan;

	        // Then, points for small total span
	        score[SCORE_TOTALSPAN] = (15 - getTotalSpan()) * 3;

	        score[SCORE_OPENSTRINGS] = 0;
	        // Points for open strings
            int i;
            
            for (i = 0; i < getNumStrings(); i++)
	        {
		        if (fretboard[i] == 0)
		        {
			        score[SCORE_OPENSTRINGS] += iScoreOpen;
		        }
	        }

	        // Points for first string being the root ...
	        score[SCORE_FIRSTROOT] = 0;
	        
	        for (i = 0; (fretboard[i] == -1) && (i < getNumStrings()) ; i++)
		        ;
	        if (notes[i] == chord.getRoot())
	        {
		        score[SCORE_FIRSTROOT] = iScoreRoot;
	        }

	        // Points for being low on the neck...
	        int sum = 0, cnt = 0;
	        for (i = 0; i < getNumStrings(); i++)
	        {
		        // Don't count X strings or open strings
		        if (fretboard[i] > 0)
		        {
			        sum += fretboard[i];
			        cnt++;
		        }
	        }
	        if (cnt > 0)
		        score[SCORE_LOWONNECK] = (int)(15 - ((double) sum / (double) cnt))
		                                 * iScorePos;
	        else
                score[SCORE_LOWONNECK] = 15 * iScorePos;


	        int adjNotes = 0;
	        for (i = 0; i < 5; i++)
	        {
		        if ((notes[i] != -1) && (notes[i] == notes[i + 1]))
			        adjNotes++;
	        }
	        score[SCORE_ADJNOTES] = (iScoreAdjacent * (5 - adjNotes));

            //score for num strings
            score[SCORE_NUMNOTES] = 0;

            int iCount = 0;
            for(i = 0; i < getNumStrings(); i++)
            {
                if(notes[i] != -1)
                {
                    iCount++;
                    score[SCORE_NUMNOTES] += SCORE_NUMSTRINGS;
                }
            }

            //penalized
            if(iCount < 3)
			{
				score[SCORE_NUMNOTES] = PENALTY_LOW_SCOUNT;
			}

	        // FInally, total up the score
	        score[SCORE_TOTAL] = 0;
	        for (i = 1; i < NUMSCORETYPES; i++)
			{
				score[SCORE_TOTAL] += score[i];
			}
        }

        public void Reset()
        {
            sDepth = 0;
            score[0] = 0;
            for (int i = 0; i < NUM_STRINGS; i++)
            {
                notes[i] = openStrings[i];
                fretboard[i] = 0;
            }
            for (int i = 0; i < CHORDSTACK; i++)
            {
                bestScores[i,0] = -1;
            }
        }



        //
        // Iterate over all fretboard config's for this chord, call addToBest
        // with any good ones (ie: < span, etc etc...)
        //
        public void iterate(ref ChordGenerator chord)
        {
	        int iString = 0;

            //int iNumStrings = getNumStrings();

	        // Start notes setup, increment up the neck for each string until
	        // you find a note that is in this chord (may be an open note)
	        for (int i = 0; i < getNumStrings(); i++)
	        {
		        while (0 == chord.inChord(notes[i]))
		        {
			        fretboard[i]++;
			        notes[i] = ( notes[i] + 1 ) % 12;
		        }
	        }

	        // Back up the first note one...so the loop will work
	        fretboard[0] = -1;

	        // While we are still on the fretboard!
	        while (iString < getNumStrings())
	        {

		        // increment the current string
		        fretboard[iString]++;
		        if (fretboard[iString] == 0)
			        notes[iString] = openStrings[iString];
		        else
			        notes[iString] = ( notes[iString] + 1 ) % 12;

		        while (0 == chord.inChord(notes[iString]))
		        {
			        fretboard[iString]++;
			        notes[iString] = ( notes[iString] + 1 ) % 12;
		        }

		        if (fretboard[iString] > 15)
		        {

			        // Before turning over the 3rd string from the bass, try
			        // to make a chord with the bass string, and 2nd from
			        // bass string X'ed out...(ie: set to -1)
			        if (iString == 0)
			        {

				        notes[0] = fretboard[0] = -1;
				        int span = getSpan();
				        if ((span < MAXSPAN) && (0 < chord.covered(ref notes)) )
				        {
					        getScore(ref chord);
					        addToBest();
				        }
			        }

			        if (iString == 1)
			        {
				        int store = notes[0];
				        int fstore = fretboard[0];
				        notes[1] = fretboard[1] = -1;
				        notes[0] = fretboard[0] = -1;
				        int span = getSpan();
				        if ((span < MAXSPAN) && (0 < chord.covered(ref notes)) )
				        {
					        getScore(ref chord);
					        addToBest();
				        }
				        // Restore the notes you X'ed out
				        notes[0] = store;
				        fretboard[0] = fstore;
			        }

			        fretboard[iString] = 0;
			        notes[iString] = openStrings[iString];
			        while (0 == chord.inChord(notes[iString]))
			        {
				        fretboard[iString]++;
				        notes[iString] = chord.note_offset(notes[iString], 1);
			        }
			        iString++;
			        continue;
		        }

		        iString = 0;
		        int iSpan = getSpan();
		        if (iSpan >= MAXSPAN)
		        {
			        continue;
		        }
		        if (0 == chord.covered(ref notes))
		        {
			        continue;
		        }

		        getScore(ref chord);

		        addToBest();
	        }
        }

        public void SetOpenStrings(int i1, int i2, int i3, int i4, int i5, int i6, int iStringCount)
        {
            openStrings[0] = i1;
            openStrings[1] = i2;
            openStrings[2] = i3;
            openStrings[3] = i4;
            openStrings[4] = i5;
            openStrings[5] = i6;

            //perfect
            iNumStrings = iStringCount;

            //	SetScoreVars();
            Reset();
        }

        public int GetOpenString(int iString)
        {
            if (iString < 0 || iString >= getNumStrings())
                return -1;
            return openStrings[iString];
        }

        public int getNumStrings()
        {
            return iNumStrings;
        }

		public int getUniqueFrets()
		{
			int count = 0;

		// Don't count X strings or open strings
		if (fretboard[0] > 0)
			count++;
     
		for(int i = 1; i < iNumStrings; i++)
		{
			if (fretboard[i] <= 0)
                continue;

			bool bNew = true;
			for(int j = 0; j < i; j++)
			{
				if(fretboard[j] == fretboard[i])
				{
					bNew = false;
					break;
				}
			}
			if(bNew == true)
				count++;
		}
		return count;
	}

        public int getStringIndent()
        {
            return NUM_STRINGS - iNumStrings;
        }//hackish






    }

        

