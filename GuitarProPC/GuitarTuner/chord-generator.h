#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// These are different types of scores, NUMSCORETYPES MUST be set to the
// number of these types, so if any are added change it TOO!
// Add any new scores onto the end, the total MUST be the 0th element
//
#define NUMSCORETYPES 7

// Just in case someone wants to port this to banjo or mandolin or something
#define NUM_STRINGS             6

// This is number of top scores to keep, something < 20 leaves out
// too much, 100 is a lot to look at...but I am going with it...
#define CHORDSTACK 50


//
// This class contains a decode chord, and can take a string and turn
// it into a chord.  A chord is a triad or greater here.  The code is
// decoded from a string into notes in the c array (notes: are A=0, A#=1,
// and so on.  The longest chord you can possibly have is 12 notes which
// would be every note in an octave,  since chords are typical 3 to 5
// notes, remaining notes in the c[] array are set to -1
//
class Chord
{
public:
	// String set to last error
	char errorStr[64];

	// Notes that are in the chord -1 should be ignored
	int notes[12];

	// NOtes that are optional have a 1, otherwise a zero
	int optional[12];

	// ------------------ MEMBER FUNCTIONS ----------------------------

	// Clears last chord
	void clear();

	//  Decodes input string into notes[] array, return error string
	int findChord(char *s);

	// Print last error message from chord parse
	void printError();

	// Prints out notes[] array to screen
	void print(char *chordName);

	// Is the note note in the chord
	int inChord(int note);

	// Is this chord covered by the array of notes (ie: Does the
	//  array contain all the notes of the chord
	int covered(int *noteArray);

	// Return the root note of the chord
	int getRoot() { return notes[0]; }

	// Given a chord string, return the note that starts it (and recognize
	// sharps and flats (b and #)
	int get_base(char *s);

	// Get a note offset from another...
	int note_offset(int base, int offset) { return ((base + offset) % 12); }
} ;

//
// This class holds a fingering for a chord, as well as a score for that
// fingering.  This class also keeps the CHORDSTACK (ie: 100) best version
// of that chord to print out in sorted order (best score 1st).
//
class Fretboard
{
public:

	// ------------------ MEMBER FUNCTIONS ----------------------------

	// Construct one
	Fretboard();

	void SetOpenStrings(int i1, int i2, int i3, int i4, int i5, int i6);
	int  GetOpenString(int iString);

	// Given a chord (and the current fretboard state) score this doggie
	// and leave the score value in score
	void getScore(Chord &chord);

	// Print the current Fretboard state (not the stack)
	void print();

	// Print the fretboard stack
	void printStack();

	// Iterate over different fretboard variations for a chord
	void iterate(Chord &chord);

	// Take the current fretboard state and put it into the stack
	void addToBest();

	// Get the span of the current chord, excluding open string
	int getSpan();

	// Get the span of the current chord, INCLUDING open string
	int getTotalSpan();

	void Reset();//clear it out

private:

//lazy but easy
public:
	// Fretboard of the current chord, fretboard[0] is the low (bass) E
	// string.  A fretboard value of 0 is an open string, -1 is an
	// X'ed out string (not strung)
	int fretboard[NUM_STRINGS];

	// Score of current chord, held in an array so we can track the
	// components of the score (see the SCORE_ defines in the beginning
	// of this file
	int score[NUMSCORETYPES];

	// Notes of the current fretboard fingering for a chord
	int notes[NUM_STRINGS];

	// The best fret layouts so far based on score
	int bestFrets[CHORDSTACK][NUM_STRINGS];

	// The best fret layout note sets so far based on score
	int bestNotes[CHORDSTACK][NUM_STRINGS];

	// The best scores
	int bestScores[CHORDSTACK][NUMSCORETYPES];

	// Keep track of stack sDepth to speed it up!
	int sDepth;
};
