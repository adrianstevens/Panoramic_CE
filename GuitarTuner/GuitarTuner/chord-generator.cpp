#include "Chord-generator.h"

//
// I have been messing around with this program now for a couple weeks
// and thought it might be useful to someone.  Given a chord by name,
// it generates all possible ways to play that chord on a guitar and
// scores them based on things like finger span, root as bass note,
// number of open strings, etc (see below).  Then it prints the top
// 50 it found (or more, you can set it).  The program is pretty
// configurable if you can stand to read the stuff at the top where
// all the #define's are.  Currently it prints out the score, and the
// pieces that made up the score...you may want to get rid of the
// extra info, but I found it interesting.
//
// It generates some wild chords and some interesting versions that
// I have NEVER seen before.  Probably a great program to find inversions
// and stuff (especially if you lower the score given for having a root
// note 1st).  The program could easily be modified to do Drop D tuning
// or any other alternate tuning, and I think it could do other instruments
// without much change, but I have not tried.
//
// I have been having fun playing with this, if you make a changes or
// fixes I would appreciate a copy.  Other than that, this is public
// domain, have a party.  Oh yeah, the code is C++...
//
// Thanks,
// Rick Eesley
// re@warren.mentorg.com
//
//
//
// Chords can be built like: | = seperate fields, [ ... ] = range of values
//                           <> = nothing
//
//                          [<--optional--->]
//                          [<--repeating-->]
// [A...G]  | <>    | <>    | <>    | #     |
//          | #     | min   | maj   |       |
//          | b     | m     | add   |       |
//          |       | sus2  | aug   |       |
//          |       | sus4  | dim   |       |
//          |       | dim   | +     |       |
//          |       | aug   | -     |       |
//          |       |       | #     |
//          |       |       | b     |
//          |       |       | /     |
//
// Legal chord names: A A7 Amaj7 Amaj9 Ammaj7 Aadd4 Asus2Add4 E7b13b11 ...
// Does not do: C/G which is a c chord with a g root, just find a c
// chord and pick out a g root you like for those...
// Does not do: E5 (that's not a chord, just 2 notes)
//
//
#include "stdafx.h"



#undef DEBUG

//////////////////////////////////////////////////////////////////////
// Scoring a chord is influenced by these multipliers, change them around
// to your own preferences
//////////////////////////////////////////////////////////////////////

//
// Score for where this lands on the fretboard (Lower is better)
// score += (15 - AverageFret) * POSITION_SCORE;
#define POSITION_SCORE 2

// Score for minimal span (ie: chord covering from fret 2 to fret 4 has
// a span of 2.  Open strings do NOT count in span, this is a measure
// of whether a chord is playable
// The maximum span allowed is set by the #define MAXSPAN
// score +=  (MAXSPAN - span) * SPAN_SCORE;
#define SPAN_SCORE 12

// This score is for total Span (ie: includes open strings, which span
// does not.
// score += (15 - tspan) * TSPAN_SCORE;
#define TSPAN_SCORE 3

// This score is for number of open strings (I like them).
// score += numberOfOpens * OPENS_SCORE
#define OPENS_SCORE 10

// This score multiplier is the score to add if the most bass string
// is the root note
// score += ROOT_SCORE
#define ROOT_SCORE 50

// This score multipier is used to penalize for adjacent notes
// score += ADJ_SCORE * (5-adjNotes)
// THe five is the max number of adjacent strings possible (duh...)
#define ADJ_SCORE 9

// This is the maximum finger reach, if this is bumped up to five or
// more you get a zillion more fingers (mainly 'cause 5 takes you to
// the notes on the next string...so leave it probably
#define MAXSPAN 4

//
// These are different types of scores, NUMSCORETYPES MUST be set to the
// number of these types, so if any are added change it TOO!
// Add any new scores onto the end, the total MUST be the 0th element
//
#define NUMSCORETYPES 7

#define SCORE_TOTAL             0
#define SCORE_SPAN              1
#define SCORE_TOTALSPAN         2
#define SCORE_OPENSTRINGS       3
#define SCORE_FIRSTROOT         4
#define SCORE_LOWONNECK         5
#define SCORE_ADJNOTES          6


/* A   A#  B   C   C#  D   D#  E   F   F#  G   G# */
/* A   Bb  B   C   Db  D   Eb  E   F   Gb  G   Ab */
/* 0   1   2   3   4   5   6   7   8   9   10  11 */

/* STRINGS ARE: E=0, A=1, D=2, G=3, B=4, E=5 IN ORDER */

// This can be reset for alternate tunings, this is EADGBE tuning
// here:
int openStrings[NUM_STRINGS] = { 7, 0, 5, 10, 2, 7 };

static char *strNotes[] = { "A ", "A#", "B ", "C ", "C#", "D ",
                            "D#", "E ", "F ", "F#", "G ", "G#" };


//
// Before building a chord, clear the notes and optional arrays
//
void Chord::clear()
{
	for (int i = 0; i < 12; i++)
	{
		notes[i] = -1;
		optional[i] = 0;
	}
}

//
// Print out the last error string
//
void
Chord::printError()
{
	printf("Error: %s\n", errorStr);
}

//
// I dunno, our C++ compiler at work did not have strupr, so heres mine
//
void myStrupr(char *s)
{
	while (*s)
	{
		if (islower(*s))
			*s = toupper(*s);
		s++;
	}
}

//
// Decodes input string into notes[] array, return error string
// always: 0 = root, 1 = 3rd, 2 = 5th
// Also sets the optional array in parallel with the notes[]
//
int Chord::findChord(char *s)
{
	clear();

	// up case the string
	myStrupr(s);

	// DECODE ROOT NOTE : A - G
	notes[0] = get_base(s);
	s++;

	// CHECK FOR SHARP OR FLAT ROOT NOTE
	if (*s == '#') s++;
	if (*s == 'B') s++;

	// MODIFY THE ROOT BY M, MIN, SUS2, SUS4, or diminished
	if (!strncmp(s, "MIN", 3 ))
	{
		notes[1] = note_offset(notes[0], 3);
		s += 3;
		optional[2] = 1;
	}
	else if (!strncmp(s, "MAJ", 3))
	{
		// Do nothing, but stops program from seeing the
		// first m in maj as a minor (see next line)...so give a normal 3rd
		notes[1] = note_offset(notes[0], 4);
		optional[2] = 1;
	}
	else if (!strncmp(s, "M", 1))
	{
		notes[1] = note_offset(notes[0], 3);
		s += 1;
		optional[2] = 1;
	}
	else if (!strncmp(s, "SUS", 1))
	{
		s += 3;   // go past sus
		if (*s == '2')
			notes[1] = note_offset(notes[0], 2);
		else if (*s == '4')
			notes[1] = note_offset(notes[0], 5);
		else
		{
			strcpy(errorStr, "sus must be followed by 2 or 4");
			return 1;
		}
		s++;  // Go past 2 or 4
		optional[2] = 1;
	}
	else if ((!strncmp(s, "DIM", 3 )) && (!isdigit(s[3])))
	{
		// If it is diminished, just return (no other stuff allowed)/
		notes[1] = note_offset(notes[0], 3);
		notes[2] = note_offset(notes[0], 6);
		notes[3] = note_offset(notes[0], 9);
		return 0;
	}
	else if ((!strncmp(s, "AUG", 3 )) && (!isdigit(s[3])))
	{
		// If it is diminished, just return (no other stuff allowed)/
		notes[1] = note_offset(notes[0], 4);
		notes[2] = note_offset(notes[0], 8);
		return 0;
	}
	else
	{
		notes[1] = note_offset(notes[0], 4);
		//  optional[1] = 1;
		//  optional[2] = 1;
	}

	notes[2] = note_offset(notes[0], 7);


	// At this point, the 1,3,5 triad or variant is built, now add onto
	//    it until the string end is reached...
	// Next note to add is index = 3...
	int index = 3;
	enum homeboy { NORMAL, MAJ, ADD, AUG, DIM } mtype ;
	char lbuf[10];

	while (*s)
	{
		// FIrst, check the mtype of modifier, ie: Aug, Maj, etc...
		mtype = NORMAL;
		if (!strncmp(s, "MAJ", 3))
		{
			mtype = MAJ;
			s += 3;
		}
		else if (!strncmp(s, "ADD", 3))
		{
			mtype = ADD;
			s += 3;
		}
		else if (!strncmp(s, "AUG", 3))
		{
			mtype = AUG ;
			s += 3;
		}
		else if (!strncmp(s, "DIM", 3))
		{
			mtype = DIM;
			s += 3;
		}
		else if ( *s == '+' )
		{
			mtype = AUG;
			s += 1;
		}
		else if ( *s == '-' )
		{
			mtype = DIM;
			s += 1;
		}
		else if ( *s == '#' )
		{
			mtype = AUG;
			s += 1;
		}
		else if ( *s == 'B' )
		{
			mtype = DIM;
			s += 1;
		}
		else if ( *s == '/' )
		{
			mtype = ADD;
			s += 1;
		}
		// Now find the number...
		if (isdigit(*s))
		{
			lbuf[0] = *s++;
			lbuf[1] = '\0';
		}
		else
		{
			sprintf(errorStr, "Expecting number, got %s", s);
			return 1;
		}
		// 2nd digit?
		if (isdigit(*s))
		{
			lbuf[1] = *s++;
			lbuf[2] = '\0';
		}

		int number = atoi(lbuf);

		switch (number)
		{
				case 7 :
				notes[index] = note_offset(notes[0], 10);
				break;
				case 9 :
				notes[index] = note_offset(notes[0], 2);

				// put the 7th in 2nd so it can be maj'ed if need be...
				if ((mtype == NORMAL) || (mtype == MAJ))
				{
					index++;
					notes[index] = note_offset(notes[0], 10);
					optional[index] = 1;  // 7th is optional, unless it is maj!
				}

				break;
				case 11 :
				notes[index] = note_offset(notes[0], 5);

				// put the 7th in 2nd so it can be maj'ed if need be...
				if ((mtype == NORMAL) || (mtype == MAJ))
				{
					index++;
					notes[index] = note_offset(notes[0], 10);
					optional[index] = 1;  // 7th is optional, unless it is  maj!
				}

				break;
				case 13 :
				notes[index] = note_offset(notes[0], 9);
				index++;
				notes[index] = note_offset(notes[0], 5);
				optional[index] = 1;  // 7th is optional, unless it is  maj!
				index++;
				notes[index] = note_offset(notes[0], 2);
				optional[index] = 1;  // 7th is optional, unless it is  maj!

				// put the 7th in 2nd so it can be maj'ed if need be...
				if ((mtype == NORMAL) || (mtype == MAJ))
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
				sprintf(errorStr, "Cannot do number: %d\n", number);
				return 1;
		}

		switch (mtype)
		{
				case DIM:
				notes[index] = note_offset(notes[index], -1);
				break;
				case MAJ:
				// It is a major, so not optional
				optional[index] = 0;
				case AUG :
				notes[index] = note_offset(notes[index], 1);
				break;
				case NORMAL:
				case ADD:
				break;
				default:
				break;
		}

		index++;
	}
	return 0;
}

//
// Print out chord by name
//
void Chord::print(char *cname)
{
	printf("Chord %s : ", cname);
	for (int i = 0; i < 12; i++)
	{
		if (notes[i] != -1)
			printf("%s ", strNotes[notes[i]]);
	}
	printf("\n");
}

//
// Are all the notes in this chord covered by the notes in the
// noteArray, it is not necessary to cover the notes in the optional
// array of the chord
//
int Chord::covered(int *noteArray)
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


//
// Is the given note in the chord
//
int Chord::inChord(int note)
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

//
// Given a chord string, pick off the root (either C or C# or Cb)...and
// return that integer value (A = 0)
//
int Chord::get_base(char *s)
{

	static int halfsteps[] = { 0, 2, 3, 5, 7, 8, 10 };

	if ((*s < 'A') || (*s > 'G'))
		return 0;

	if (s[1] == '#')
		return ( note_offset(halfsteps[s[0] - 'A'], 1));
	else if (s[1] == 'B')
		return ( note_offset(halfsteps[s[0] - 'A'], -1));
	else
		return ( halfsteps[s[0] - 'A']);
}


//
// Print out the current fretboard
//
void Fretboard::print()
{
	printf("SCORE: %3d ", score[SCORE_TOTAL]);
	printf(
	    " SPN: %2d TSPN: %2d OS: %2d ROOT: %2d LOW %2d ADJ %2d",
	    score[SCORE_SPAN], score[SCORE_TOTALSPAN], score[SCORE_OPENSTRINGS],
	    score[SCORE_FIRSTROOT], score[SCORE_LOWONNECK],
	    score[SCORE_ADJNOTES]);

	printf(" FB: ");
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		if (fretboard[i] != -1)
			printf(" %2d", fretboard[i]);
		else
			printf("  X");
	}

	printf(" NT: ");
	for (int i = 0; i < NUM_STRINGS; i++)
		if (notes[i] != -1)
			printf(" %s", strNotes[notes[i]]);
		else
			printf(" X ");
	printf("\n");
}

//
// Construct a fretboard -- reset to the openStrings, clear the stack
// and reset all the bestScores to -1
//
Fretboard::Fretboard()
{
	Reset();
}

//
// Get the span of this chord, don't count open strings
//
int Fretboard::getSpan()
{
	int min = 100, max = 0;
	for (int i = 0; i < NUM_STRINGS; i++)
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
int Fretboard::getTotalSpan()
{
	int min = 100, max = 0;
	for (int i = 0; i < NUM_STRINGS; i++)
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
void Fretboard::addToBest()
{
	// CHORDSTACK is the sDepth of keepers...
#ifdef DEBUG
	printf("ATB: ");
	this->print();
#endif

	int i;
	
	// NOTE: at the start, bestScores is full of -1's, so any reall
	// real score will be better (worst score is 0)
	for (i = 0; i < sDepth; i++)
	{
		if (score[0] > bestScores[i][0])
			break;
	}

	// If score was not better than any in the stack just return
	if (i >= CHORDSTACK)
		return ;
	// MOve down old guys to make room for the new guy
	for (int j = CHORDSTACK - 1; j >= i; j--)
	{
		for (int q = 0; q < NUM_STRINGS; q++)
		{
			bestFrets[j][q] = bestFrets[j - 1][q];
			bestNotes[j][q] = bestNotes[j - 1][q];
		}
		for (int q = 0; q < NUMSCORETYPES; q++)
			bestScores[j][q] = bestScores[j - 1][q];
	}

	for (int q = 0; q < NUM_STRINGS; q++)
	{
		bestFrets[i][q] = fretboard[q];
		bestNotes[i][q] = notes[q];
	}
	for (int q = 0; q < NUMSCORETYPES; q++)
		bestScores[i][q] = score[q];

	sDepth++;
	if (sDepth > CHORDSTACK)
		sDepth--;
}

//
// Print out the stack to the screen
//
void Fretboard::printStack()
{
	static char *strNotes[] = { "A ", "A#", "B ", "C ", "C#", "D ",
	                            "D#", "E ", "F ", "F#", "G ", "G#" };

	printf(" TOT SPN TSN OPN ROT LOW ADJ");
	printf("           CHORD               NOTES\n");
	printf("============================");
	printf("==========================================\n");

//	for (int f = 0; f < sDepth; f++)
	for (int f = 0; f < 4; f++)
	{

		printf("%4d %3d %3d %3d %3d %3d %3d", bestScores[f][0],
		       bestScores[f][SCORE_SPAN], bestScores[f][SCORE_TOTALSPAN],
		       bestScores[f][SCORE_OPENSTRINGS], bestScores[f][SCORE_FIRSTROOT],
		       bestScores[f][SCORE_LOWONNECK], bestScores[f][SCORE_ADJNOTES]);

		printf("   ");
		for (int i = 0; i < NUM_STRINGS; i++)
		{
			if (bestFrets[f][i] != -1)
				printf(" %2d", bestFrets[f][i]);
			else
				printf("  X");
		}

		printf("   ");
		for (int i = 0; i < NUM_STRINGS; i++)
			if (bestNotes[f][i] != -1)
				printf(" %s", strNotes[bestNotes[f][i]]);
			else
				printf(" X ");
		printf("\n");
	}
}


//
// Get the score for this chord
//
void Fretboard::getScore(Chord &chord)
{
	// First, points for small span (excluding opens)
	score[SCORE_SPAN] = (MAXSPAN - getSpan()) * SPAN_SCORE;

	// Then, points for small total span
	score[SCORE_TOTALSPAN] = (15 - getTotalSpan()) * 3;

	score[SCORE_OPENSTRINGS] = 0;
	// Points for open strings
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		if (fretboard[i] == 0)
		{
			score[SCORE_OPENSTRINGS] += OPENS_SCORE;
		}
	}

	// Points for first string being the root ...
	score[SCORE_FIRSTROOT] = 0;
	int i;
	for (i = 0; (fretboard[i] == -1) && (i < NUM_STRINGS) ; i++)
		;
	if (notes[i] == chord.getRoot())
	{
		score[SCORE_FIRSTROOT] = ROOT_SCORE;
	}

	// Points for being low on the neck...
	int sum = 0, cnt = 0;
	for (i = 0; i < NUM_STRINGS; i++)
	{
		// Don't count X strings or open strings
		if (fretboard[i] > 0)
		{
			sum += fretboard[i];
			cnt++;
		}
	}
	if (cnt)
		score[SCORE_LOWONNECK] = (int)(15 - ((double) sum / (double) cnt))
		                         * POSITION_SCORE;
	else
		score[SCORE_LOWONNECK] = 15 * POSITION_SCORE;


	int adjNotes = 0;
	for (i = 0; i < 5; i++)
	{
		if ((notes[i] != -1) && (notes[i] == notes[i + 1]))
			adjNotes++;
	}
	score[SCORE_ADJNOTES] = (ADJ_SCORE * (5 - adjNotes));

	// FInally, total up the score
	score[SCORE_TOTAL] = 0;
	for (i = 1; i < NUMSCORETYPES; i++)
		score[SCORE_TOTAL] += score[i];
}


void Fretboard::Reset()
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
		bestScores[i][0] = -1;
	}
}

//
// Iterate over all fretboard config's for this chord, call addToBest
// with any good ones (ie: < span, etc etc...)
//
void Fretboard::iterate(Chord &chord)
{
	int string = 0;

	// Start notes setup, increment up the neck for each string until
	// you find a note that is in this chord (may be an open note)
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		while (! chord.inChord(notes[i]))
		{
			fretboard[i]++;
			notes[i] = ( notes[i] + 1 ) % 12;
		}
	}

	// Back up the first note one...so the loop will work
	fretboard[0] = -1;

	// While we are still on the fretboard!
	while (string < NUM_STRINGS)
	{

		// increment the current string
		fretboard[string]++;
		if (fretboard[string] == 0)
			notes[string] = openStrings[string];
		else
			notes[string] = ( notes[string] + 1 ) % 12;

		while (! chord.inChord(notes[string]))
		{
			fretboard[string]++;
			notes[string] = ( notes[string] + 1 ) % 12;
		}

		if (fretboard[string] > 15)
		{

			// Before turning over the 3rd string from the bass, try
			// to make a chord with the bass string, and 2nd from
			// bass string X'ed out...(ie: set to -1)
			if (string == 0)
			{

				notes[0] = fretboard[0] = -1;
				int span = getSpan();
				if ((span < MAXSPAN) && chord.covered(notes))
				{
					getScore(chord);
					addToBest();
				}
			}

			if (string == 1)
			{
				int store = notes[0];
				int fstore = fretboard[0];
				notes[1] = fretboard[1] = -1;
				notes[0] = fretboard[0] = -1;
				int span = getSpan();
				if ((span < MAXSPAN) && chord.covered(notes))
				{
					getScore(chord);
					addToBest();
				}
				// Restore the notes you X'ed out
				notes[0] = store;
				fretboard[0] = fstore;
			}

			fretboard[string] = 0;
			notes[string] = openStrings[string];
			while (! chord.inChord(notes[string]))
			{
				fretboard[string]++;
				notes[string] = chord.note_offset(notes[string], 1);
			}
			string++;
			continue;
		}

#ifdef DEBUG
		printf("TRY: "); this->print();
#endif

		string = 0;
		int span = getSpan();
		if (span >= MAXSPAN)
		{
#ifdef DEBUG
			printf("Rejected for span\n");
#endif
			continue;
		}
		if (!chord.covered(notes))
		{
#ifdef DEBUG
			printf("Rejected for coverage\n");
#endif
			continue;
		}

		getScore(chord);

		addToBest();
	}
}


//
// uh, main
//
int main()
{
	char buf[256], buf2[256];

	printf("Enter Chord: ");
	while (gets(buf) != NULL)
	{
		// Allocate it for DOS/WINDOWS, to avoid stack overflow (weak)
		Fretboard *fb = new Fretboard; ;
		Chord chord;

		if ( !strcmp("quit", buf) || !strcmp("QUIT", buf))
			break;

		// findChord upppercases the input string, so save a copy
		strcpy(buf2, buf);

		if (chord.findChord(buf))
		{
			chord.printError();
			printf("Enter Chord: ");
			continue;
		}
		chord.print(buf2);
		fb->iterate(chord);
		fb->printStack();
		printf("Enter Chord: ");
		delete fb;
	}
	return 0;
}

void Fretboard::SetOpenStrings(int i1, int i2, int i3, int i4, int i5, int i6)
{
	openStrings[0] = i1;
	openStrings[1] = i2;
	openStrings[2] = i3;
	openStrings[3] = i4;
	openStrings[4] = i5;
	openStrings[5] = i6;

	Reset();
}

int Fretboard::GetOpenString(int iString)
{
	if(iString < 0 || iString >= NUM_STRINGS)
		return -1;
	return openStrings[iString];
}


/*
 while (string < NUM_STRINGS) {

        // increment the current string
        fretboard[string]++;
        if (fretboard[string] == 0)
            notes[string] = openStrings[string];
        else 
            notes[string] = ( notes[string] + 1 ) % 12;

        while (! chord.inChord(notes[string])) {
            fretboard[string]++;
            notes[string] = ( notes[string] + 1 ) % 12;
        }

        if (fretboard[string] > 15) {

            // Before turning over the 3rd string from the bass, try
            // to make a chord with the bass string, and 2nd from 
            // bass string X'ed out...(ie: set to -1)
            if (string == 0) {
                
                notes[0] = fretboard[0] = -1;
                int span = getSpan();
                if ((span < MAXSPAN) && chord.covered(notes)) {
                    getScore(chord);
                    addToBest();
                }
            }

            if (string == 1) {
                int store = notes[0];
                int fstore = fretboard[0];
                notes[1] = fretboard[1] = -1;
                notes[0] = fretboard[0] = -1;
                int span = getSpan();
                if ((span < MAXSPAN) && chord.covered(notes)) {
                    getScore(chord);
                    addToBest();
                }
                // Restore the notes you X'ed out
                notes[0] = store;
                fretboard[0] = fstore;
            }

            fretboard[string] = 0;
            notes[string] = openStrings[string];
            while (! chord.inChord(notes[string])) {
               fretboard[string]++;
               notes[string] = chord.note_offset(notes[string], 1);
            }
            string++;
            continue;
        }
        
#ifdef DEBUG
        printf("TRY: "); this->print();
#endif

        string = 0;     
        int span = getSpan();
        if (span >= MAXSPAN)  {
#ifdef DEBUG
            printf("Rejected for span\n");
#endif
            continue;
        }
        if (!chord.covered(notes)) {
#ifdef DEBUG
            printf("Rejected for coverage\n");
#endif
            continue;
        }
        
        getScore(chord);
        
        addToBest();
    }
}*/


// 
// uh, main
// 

/*
void main()
{
    char buf[256], buf2[256];

    printf("Enter Chord: ");
    while (gets(buf) != NULL) {
        // Allocate it for DOS/WINDOWS, to avoid stack overflow (weak)
        Fretboard *fb = new Fretboard;;
        Chord chord;
        
        if ( !strcmp("quit", buf) || !strcmp("QUIT", buf))
            break;

        // findChord upppercases the input string, so save a copy
        strcpy(buf2, buf);

        if (chord.findChord(buf)) {
            chord.printError();
            printf("Enter Chord: ");
            continue;
        }
        chord.print(buf2);
        fb->iterate(chord);
        fb->printStack();
        printf("Enter Chord: ");
        delete fb;
    }
    return 0;
}

*/