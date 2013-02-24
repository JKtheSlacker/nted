/****************************************************************************************/
/*											*/
/* This program is free software; you can redistribute it and/or modify it under the	*/
/* terms of the GNU General Public License as published by the Free Software		*/
/* Foundation; either version 2 of the License, or (at your option) any later version.	*/
/*											*/
/* This program is distributed in the hope that it will be useful, but WITHOUT ANY	*/
/* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	*/
/* PARTICULAR PURPOSE. See the GNU General Public License for more details.		*/
/*											*/
/* You should have received a copy of the GNU General Public License along with this	*/
/* program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.        */
/*											*/
/*--------------------------------------------------------------------------------------*/
/*											*/
/*  Copyright   Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY           */
/*		ja@informatik.tu-chemnitz.de						*/
/*											*/
/*											*/
/****************************************************************************************/

#ifndef NOTE_H

#define NOTE_H
#include "config.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include "chordorrest.h"
#include "resource.h"

class NedMainWindow;
class NedPage;
class NedSystem;
class NedStaff;
class NedVoice;
class NedChordOrRest;
class NedBbox;
class NedCommandList;

struct pitch_descr;

#define UNKNOWN_LINE -1111111

class NedNote {
	public:
		NedNote(NedChordOrRest *chord_or_rest, int line, int head, unsigned int status);
		void draw(cairo_t *cr);
		static gint compare(NedNote *a, NedNote *b);
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem();
		NedStaff *getStaff();
		NedVoice *getVoice();
		NedChordOrRest *getChord() {return m_chord_or_rest;}
		void setChord(NedChordOrRest *chord) {m_chord_or_rest = chord;}
		void handleRelease() {m_tmp_line = UNKNOWN_LINE;}
		bool noConflict();
		void shiftNote(bool shift);
		double placeAccidental(bool all_shifted, int acc_places[4], bool has_arpeggio, bool out);
		void collectNotesWithAccidentals(unsigned int *num_notes_with_accidentals, NedNote **note_field);
		static int compare_note_lines(NedNote **n1, NedNote **n2);
		bool trySelect (double x, double y);
		double getYpos();
		bool testYShift(double y);
		void moveUpDown(int line);
		bool testRelativeMove(int dist);
		int getTempLine() {return (m_tmp_line == m_line) ?  UNKNOWN_LINE : m_tmp_line;}
		int getLine() {return m_line;}
		void shiftNoteLine(int linedist);
		unsigned int getStatus() {return m_status;}
		void setStatus(unsigned int status);
		void computeBounds(int len, double *minx, double *miny, double *maxx, double *maxy);
		void do_tie(NedNote *other_note);
		void setTieSimple(NedNote *other_note);
		void setTieIfSamePitch(NedNote *other_note, bool check_pitch);
		void computeTie();
		bool isTheSame(NedNote *other_note);
		void tryTieBackward();
		void removeBackwardTie();
		void resetActiveFlag() {m_active = NOTE_ACTIVE_STATE_NONE;}
		void removeForwardTie();
		void setTieForward(NedNote *tie_forward) {m_tie_forward = tie_forward;}
		void setTieBackward(NedNote *tie_backward) {m_tie_backward = tie_backward;}
		bool testForTiesToDelete(NedCommandList *command_list, unsigned int dir = BREAK_TIE_BACKWARD, bool execute = false);
		NedNote *getTieForward() {return m_tie_forward;}
		NedNote *getTieBackward() {return m_tie_backward;}
		void correctTieForward();
		void saveNote(FILE *fp);
		void saveTies(FILE *fp, bool *ties_written);
		bool test_tie_increment(double incr);
		void setInternalPitch(int pitchdist, int clef, int keysig, int octave_shift);
		void adjustAccidental(int clef, int keysig, int octave_shift);
		void changeAccidental(NedCommandList *command_list, int preferred_offs, int clef, int keysig, int octave_shift);
		void prepareReplay(unsigned long long midi_start_time, int clef, int keysig, 
			 int octave_shift,  int pitch_offs, int voice_idx, unsigned int grace_time, bool is_grace, bool is_stacc, unsigned short segno_sign);
		char m_active;
		int getPitch(int clef, int keysig, int octave_shift, int pitch_offs, bool *flat, bool use_transpose);
		int getPitch();
		void removeUnneededAccidental(int clef, int keysig, int octave_shift);
		int getNoteHead() {return m_head;}
		void setNoteHead(int head) {m_head = head;}
		void setOffset(char offs_array[OFFS_ARRAY_SIZE]);
		static void setTies(NedNote *n1, NedNote *n2);
		void adjust_pointers(struct addr_ref_str *addrlist);
		void exportLilyPond(FILE *fp, int *last_line, bool *tie);
		static int determineLineOf(int oldline, int newclef);
		static int pitchToLine(int pitch, int clef, int keysig, int octave_shift, int *offs, int preferred_offs = 0);
		static unsigned int determineState(int pitch, int line, int half_offs, char offs_array[115], int keysig);
		bool isFirstNote();
		void setTieOffset(double offs) {m_tie_offs = offs;}
	private:
		int findPitchUpDown(int key, int pitch, int dist);
		int getDrumPitch();
		void exportLilyPondDrums(FILE *fp);
		static pitch_descr lineToPitch[];
		static int signToBase[][2];
		static int diaDists[];
		double m_xpos, m_ypos;
		double m_tmp_ypos;
		double m_acc_dist;
		int m_line, m_tmp_line;
		unsigned int m_status;
		int m_head;
		NedChordOrRest *m_chord_or_rest;
		NedNote *m_tie_forward, *m_tie_backward;
		double m_tie_y_offs1, m_tie_y_offs2;
		double m_tie_xpos0, m_tie_xpos1;
		double m_tie_back_xpos0, m_tie_back_xpos1;
		double m_tie_offs;
		int m_pitch; // used temporarily

	friend class NedChangeTieCommand;
	friend class NedTieNotesCommand;
	friend class NedMoveNoteRelativeCommand;
	friend class NedChangeAccidentalCommand;
};

#endif /* NOTE_H */
