#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define POSITION_SCORE 2
#define SPAN_SCORE 12
#define TSPAN_SCORE 3
#define OPENS_SCORE 10
#define ROOT_SCORE 50
#define ADJ_SCORE 9
#define MAXSPAN 4
#define CHORDSTACK 50


#define NUMSCORETYPES 7

#define SCORE_TOTAL             0
#define SCORE_SPAN              1
#define SCORE_TOTALSPAN         2
#define SCORE_OPENSTRINGS       3
#define SCORE_FIRSTROOT         4
#define SCORE_LOWONNECK         5
#define SCORE_ADJNOTES          6

// Just in case someone wants to port this to banjo or mandolin or something
#define NUM_STRINGS             6


/* A   A#  B   C   C#  D   D#  E   F   F#  G   G# */
/* A   Bb  B   C   Db  D   Eb  E   F   Gb  G   Ab */
/* 0   1   2   3   4   5   6   7   8   9   10  11 */

/* STRINGS ARE: E=0, A=1, D=2, G=3, B=4, E=5 IN ORDER */

// This can be reset for alternate tunings, this is EADGBE tuning
// here:
int openStrings[NUM_STRINGS] = { 7, 0, 5, 10, 2, 7 };

static char *strNotes[] = { "A ", "A#", "B ", "C ", "C#", "D ",
"D#", "E ", "F ", "F#", "G ", "G#" };




class CChordGenerator
{
public:
	CChordGenerator(void);
	~CChordGenerator(void);


private:
/*	void		clear();		// Clears last chord
	
	int			findChord(char *s);	//  Decodes input string into notes[] array, return error string
	void		printError();// Print last error message from chord parse
	void		print(char *chordName);// Prints out notes[] array to screen
		
	int			inChord(int note);// Is the note note in the chord
	int			covered(int *noteArray);// Is this chord covered by the array of notes (ie: Does the array contain all the notes of the chord
	int			getRoot() { return notes[0]; }// Return the root note of the chord
	int			get_base(char *s);// Given a chord string, return the note that starts it (and recognize sharps and flats (b and #)
	int			note_offset(int base, int offset) { return ((base + offset) % 12); }// Get a note offset from another...
*/



public:
	char		errorStr[64];  // String set to last error

	int			notes[12]; // Notes that are in the chord -1 should be ignored
	int			optional[12]; // NOtes that are optional have a 1, otherwise a zero

};
