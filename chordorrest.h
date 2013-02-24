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

#ifndef CHORD_OR_REST_H

#define CHORD_OR_REST_H
#include "config.h"
#include "resource.h"
#include "beam.h"

#include <gtk/gtk.h>
#include <cairo.h>

#define LAST_TUPLET_FLAG (1 << 24)
#define TUPLET_VAR_MASK (0xfff)

#define BREAK_TIE_FORWARD (1 << 0)
#define BREAK_TIE_BACKWARD (1 << 1)

#define TYPE_REST (1 << 0)
#define TYPE_CHORD (1 << 1)
#define TYPE_CLEF (1 << 2)
#define TYPE_KEYSIG (1 << 3)
#define TYPE_STAFF_SIGN (1 << 5) // otherwise conflic with CLEFS : -((
#define TYPE_GRACE (1 << 4)
#define TYPE_DYNAMIC (1 << 6)
#define TYPE_TEMPO (1 << 7)
#define TYPE_SLUR (1 << 8)
#define TYPE_LINE (1 << 9)
#define TYPE_LINE3 (1 << 10)
#define TYPE_FREE_TEXT (1 << 11)
#define TYPE_SIGN (1 << 12)
#define TYPE_GUITAR_CHORD (1 << 13)
#define TYPE_SPACER (1 << 14)
#define TYPE_CHORDNAME (1 << 15)


#define LYR_CURSOR_LEFT -1
#define LYR_CURSOR_RIGHT 1
#define LYR_CURSOR_START -2
#define LYR_CURSOR_END 2

#define MAX_ARTICULATIONS 10

#define WRITTEN_SLANT_NORMAL 0
#define WRITTEN_SLANT_ITALIC 1
#define WRITTEN_SLANT_OBLIQUE 2

#define WRITTEN_WEIGHT_NORMAL 0
#define WRITTEN_WEIGHT_BOLD 1

#define UNSET_STEM_HEIGHT -10000.0
#define UNSET_SLOPE_OFFS -1000


class NedMainWindow;
class NedPage;
class NedSystem;
class NedStaff;
class NedVoice;
class NedTuplet;
class BBox;
class NedCommandList;
class NedNote;
class NedFreeReplaceable;
class NedLyricsEditor;
class NedSlur;
class NedMeasure;
class NedPangoCairoText;

class VolumeChange {
	public:
		VolumeChange(int vol, unsigned long long time) : m_volume(vol), m_midi_time(time) {}
		int m_volume;
		unsigned long long m_midi_time;
		static int compare(VolumeChange *v1, VolumeChange *v2) {
			if (v1->m_midi_time < v2->m_midi_time) return -1;
			if (v1->m_midi_time > v2->m_midi_time) return  1;
			return 0;
		}
};




class NedChordOrRest {
	public:
		NedChordOrRest(NedVoice *voice, int type, bool is_hidden, int line, int dot_count, unsigned int length, int head, unsigned int status, unsigned long long time);
		NedChordOrRest(NedVoice *voice, int type, unsigned int clef_number, int octave_shift, unsigned long long time, bool dummy); // clef
		NedChordOrRest(NedVoice *voice, int type, int keysig, int last_keysig, unsigned long long time); // key signature
		NedChordOrRest(NedVoice *voice, int type, int sub_type, unsigned long long time); // sign 
		~NedChordOrRest();
		NedChordOrRest *clone(struct addr_ref_str **addrlist, bool keep_beam = false);
		NedChordOrRest *clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedVoice *p_voice);
		void setActive();
		void setForceBeamFlag() {m_status &= (~(STAT_BREAK_BEAM)); m_status |= STAT_FORCE_BEAM;}
		void setBreakBeamFlag() {m_status &= (~(STAT_FORCE_BEAM)); m_status |= STAT_BREAK_BEAM;}
		bool hasUpDir();
		void flipStem(NedCommandList *command_list);
		void print();
		void draw(cairo_t *cr, bool *freetexts_or_lyrics_present);
		void drawTexts(cairo_t *cr, double scale, double last_left_bbox_edge, bool last);
		double getRealXpos() {return m_xpos;}
		double getXPos() {/* if (m_tmp_xpos < 0.0) */return m_xpos; /* return m_tmp_xpos; */}
		double getLeftBboxEdge() {return m_xpos + m_bbox.x;}
		double getXPosAndMicroshifts() {if (m_tmp_xpos < 0.0) return m_xpos + m_micro_shift; return m_tmp_xpos + m_micro_shift;}
		void insertNoteAt(NedNote *note, bool echo = FALSE);
		bool removeNoteAt(int line);
		bool noConflict(int line);
		void testTies();
		void tryTieBackward();
		void reConfigure();
		NedMeasure *m_measure;
		NedNote *getFirstNote();
		int getLineOfLowestNote();
		int getLineOfFirstNote();
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem();
		NedStaff *getStaff();
		int getPosition();
		int lengthForFlagCountComputation();
		int numberOfFlagSigns();
		int numberOfTremoloSigns();
		NedVoice *getVoice() {return m_voice;}
		void createLyrics(int line, char *text);
		void setLyrics(int line, char *text);
		void setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor);
		void setLyricsFromOldContext(NedMainWindow *main_window, int line, char *text);
		void appendToLyrics(NedCommandList *command_list, int line, const char *text); 
		bool deleteLyricsLetter(NedCommandList *command_list, int line);
		bool setLyricsCursor(int line, int pos);
		void selectNextChord(int line, bool lyrics_required);
		void selectPreviousChord(int line, bool lyrics_required);
		void setVoice(NedVoice *voice) {m_voice = voice;}
		void removeUnneededAccidentals(int clef, int keysig, int octave_shift);
		void setToRest(bool toRest);
		int getType() {return m_type;}
		bool isRest() {return (m_type == TYPE_REST);}
		bool isHidden() {return m_is_hidden || (m_status & STAT_USER_HIDDEN_REST);}
		void setHidden() {m_is_hidden = true;}
		void setTime(unsigned int time)  {m_time = time;}
		unsigned int getStatus() {return m_status;}
		void setStatus(unsigned int status);
		unsigned int getLength() {return m_length;}
		void setLength(int length);
		bool trySelect (double x, double y, bool only_free_placeables);
		bool hitStemEnd(double x, double y);
		double computeDistFrom(double x, double y, double factor, double *ydist);
		bool tryErease (double x, double y, bool *removed);
		void ereaseNote(NedNote *note);
		bool testXShift(double x);
		void moveToLine(int line);
		bool scaleStem(double y, double old_ypos);
		void fixIntermediateValues();
		double getCurrentStemLen() {return m_stem_height;}
		double getOldStemLen() {return m_stemheight_copy;}
		bool ReleaseTmpValue();
		static int compareMidiTimes(NedChordOrRest *c1, NedChordOrRest *c2);
		static int compareMidiTimesAndTypes(NedChordOrRest *c1, NedChordOrRest *c2);
		unsigned int getTime() {return m_time;}
		unsigned int getDuration(unsigned int measure_duration = -1);
		double determineTempoInverse();
		void recomputeFreeReplaceables();
		void zoomFreeReplaceables(double zoom, double scale);
		void determineTempoInverse(double *tempoinverse);
		void setXPos(double pos);
		void setXPos2(double pos);
		void shiftX(double x);
		void shiftY(double y);
		void setRestsY(double y);
		void setXPosAccordingBBox(double pos) {m_xpos = pos - m_bbox.x;}
		unsigned int getStopTime();
		void setBeam(NedBeam *beam) {m_beam = beam;}
		NedBeam *getBeam() {return m_beam;}
		void reposit(unsigned int pos);
		void xPositNotes();
		void collectNotesWithAccidentals(unsigned int *num_notes_with_accidentals, NedNote **note_field);
		void sortNotes();
		double getBeamXPos();
		double getStemTop();
		double getStemBottom();
		int getStemDir();
		void resetYPos();
		double getStemYStart();
		void getTopBotY(int *lyrics_lines, double *topy , double *boty/* , bool *topFix, bool *botFix */);
		double getTopOfChordOrRest();
		double getBottomOfChordOrRest();
		bool testForTiesToDelete(NedCommandList *command_list, unsigned int dir = BREAK_TIE_BACKWARD, bool execute = false);
		void testForFreeReplaceablesToDelete(NedCommandList *command_list, GList **already_deleted, bool execute = false);
		void testForDeleteableConstraints(NedCommandList *command_list);
		void tieAllNotes(NedChordOrRest *other_chord);
		static void handleOpenTies(NedCommandList *command_list, GList *pool);
		bool isTied();
		void tieCompleteTo(NedChordOrRest *other_chord, bool check_pitch);
		int getDotCount() {return m_dot_count;}
		void setDotCount(int dot_count) {m_dot_count = dot_count;}
		NedBbox *getBBox() {return &m_bbox;}
		void computeBbox();
		void computeTies();
		double getNeededSpace();
		bool testMeasureSpread(int dir);
		void setMidiTime(unsigned long long time);
		void incrMidiTime(unsigned long long incr) {m_midi_time += incr;}
		void decrMidiTime(unsigned long long incr); // {m_midi_time -= incr;}
		unsigned long long getMidiTime() {return m_midi_time;}
		bool isDown() {return m_beam ? !m_beam->isBeamUp() : !(m_status & STAT_UP);}
		int getSorting(NedChordOrRest *other_chord);
		void changeDuration(unsigned int new_duration, int tuplet_val);
		NedChordOrRest *cloneWithDifferentLength(unsigned int newlen, int dotcount);
		static unsigned int computeDuration(unsigned int length, int dotcount, int tuplet_val);
		static void compute_fitting_duration(unsigned int sum, unsigned int *length, int *dot_count);
		static NedChordOrRest *restoreChordOrRest(FILE *fp, NedVoice *voice);
		NedChordOrRest *getNextChordOrRest();
		NedChordOrRest *getPreviousChordOrRest();
		NedChordOrRest *getFirstObjectInNextSystem();
		int m_position;
		int getStemDirectionOfTheMeasure(int staff_voice_offs);
		double computeStemDist(bool thisup, NedChordOrRest *other_chord, bool otherup);
		double getBeamOffset() {return m_beam_y_offs;} 
		int getSlopeOffset() {return m_slope_offs;}
		void setStemHeight(double stem_height) {m_stem_height = stem_height;}
		void setBeamYOffset(double beam_offset) {m_beam_y_offs = beam_offset;}
		void setBeamSlopeOffset(int beam_slope_offs) {m_slope_offs = beam_slope_offs;}
		void saveChordOrRest(FILE *fp);
		void changeStemDir(int dir);
		void saveTies(FILE *fp, bool *ties_written);
		void saveFreePlaceables(FILE *fp, bool *free_placeables_written);
		void prepareMutedReplay();
		void resetActiveFlags();
		void prepareReplay(int clef, int keysig, int octave_shift, int voice_idx, unsigned int grace_time);
		void setOffset(char offs_array[115]);
		int getPitchOfFirstNote();
		void setInternalPitch(int pitchdist);
		void hideRest(NedCommandList *command_list, bool unhide);
		void shiftNotes(int linedist);
		void adjustAccidentals();
		void changeAccidentals(NedCommandList *command_list, int preferred_offs);
		void resetMicroshift() {m_micro_shift = 0.0;}
		double compute_microshift(NedChordOrRest *right_chord, int mnum, int pos);
		double compute_y_shift(NedChordOrRest *other_chord, int dir);
		bool hasLastTupletFlag() {return (m_tuplet_val & LAST_TUPLET_FLAG);}
		void setLastTupletFlag(bool on) {if (on) m_tuplet_val |= LAST_TUPLET_FLAG; else m_tuplet_val &= (~(LAST_TUPLET_FLAG));}
		void correctTiesForward();
		void removeAllBackwardTies();
		bool tieBackward(NedChordOrRest *prev, NedNote *note);
		void setTupletVal(int tuplet_val)  {m_tuplet_val = tuplet_val;}
		void setTupletPtr(NedTuplet *tuplet_ptr) {m_tuplet_ptr = tuplet_ptr;}
		NedTuplet *getTupletPtr() {return m_tuplet_ptr;}
		int getTupletVal() {return m_tuplet_val & TUPLET_VAR_MASK;}
		int getTupletValRaw() {return m_tuplet_val;}
		bool isUp();
		int getLine() {return m_line;}
		void adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist);
		char *getLyrics(int line);
		void exportLilyPond(FILE *fp, int *last_line, unsigned int *midi_len, NedSlur **lily_slur, unsigned int *lyrics_map,
			bool *guitar_chordnames, bool *chordname, int *breath_script, bool keep_beams);
		void exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len);
		void exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len);
		void exportLilyLyrics(FILE *fp, int line_nr, int *sil_count);
		void collectLyrics(NedLyricsEditor *leditor);
	private:
		void removeFreeDisplaceable(NedFreeReplaceable *freedisplaceable);
		void addFreeDislaceable(NedFreeReplaceable *freedisplaceable);
		void exportLilyAccents(FILE *fp);
		void exportLilyFreePlaceablesBefore(FILE *fp);
		void exportLilyFreePlaceablesAfter(FILE *fp, NedSlur **lily_slur, bool *guitar_chordnames, bool *chordnames);
		GList *m_notes;
		NedBeam *m_beam;
		NedTuplet *m_tuplet_ptr;
		int m_tuplet_val;
		int m_type;
		bool m_is_hidden;
		unsigned int m_length;
		double m_xpos, m_ypos;
		unsigned int m_time;
		int m_dot_count;
		unsigned int m_status;
		int m_line;
		NedVoice *m_voice;
		NedBbox m_bbox;
		unsigned long long m_midi_time;
		double m_tmp_xpos;
		bool m_all_shifted;
		double m_art_up_y_pos[MAX_ARTICULATIONS];
		double m_art_down_y_pos[MAX_ARTICULATIONS];
		int m_up_art_count, m_down_art_count;
		NedPangoCairoText *m_lyrics[MAX_LYRICS_LINES];
		GList *m_freedisp;
		bool m_active;
		double m_stem_end_xpos, m_stem_end_ypos;
		double m_stem_height, m_stemheight_copy;
		double m_beam_y_offs;
		int m_slope_offs;
		double m_beam_y_offs_copy;
		double m_micro_shift;
		int m_slope_offs_copy;
		unsigned int m_status_copy;
		bool m_free_texts_present, m_lyrics_present;
		bool m_has_open_highhat, m_has_closed_highhat;
	friend class NedNewLyricsCommand;
	friend class NedDeleteLyricsCommand;
	friend class NedFreeReplaceable;
	friend class NedFixIntermediateCommand;
	friend class NedMoveNoteRelativeCommand;
	friend class NedFlipValuesCommand;
	friend class NedRemoveConstraintsCommand;
	friend class NedChangeLyricsCommand;
};

#endif /* CHORD_OR_REST_H */
