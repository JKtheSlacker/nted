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

#ifndef STAFF_H

#define STAFF_H
#include "config.h"
#include "resource.h"

#include <gtk/gtk.h>
#include <cairo.h>

class NedMainWindow;
class NedPage;
class NedSystem;
class NedVoice;
class NedChordOrRest;
class NedClipBoard;
class NedCommandList;
class NedLyricsEditor;
struct VoiceInfoContainer;
class NedMeasure;
class NedSlur;
class part;

class NedStaff {
	public:
		NedStaff(NedSystem *system, double ypos, double width, int nr, bool start);
		~NedStaff();
		NedStaff *clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedSystem *p_system, int staff_number);
		void adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist);
		void draw(cairo_t *cr, double botpos, bool first_page, double indent, bool *freetexts_or_lyrics_present);
		void drawTexts(cairo_t *cr, bool first_page, double scale);
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem() {return m_system;}
		int getStaffNumber() {return m_staff_number;}
		bool truncateAtStart(NedCommandList *command_list, unsigned long long midi_time);
		void setStaffNumber(int nr) {m_staff_number = nr;}
		void prepareForImport();
		void appendElementsOfMeasureLength(part *part_ptr, unsigned int meas_duration, int meas_num);
		void copyDataOfWholeStaff();
		void removeLastImported();
		bool handleImportedTuplets();
		void handleStaffElements();
		//bool find_new_cursor_pos(guint keyval, int *x, int *y, double *dist);
		bool trySelect (double x, double y, bool only_free_placeables);
		NedChordOrRest *findNearestElement(double x, double y, NedStaff *oldstaff, double *mindist, double *ydist);
		bool tryInsertOrErease (double x, double y, int num_midi_input_notes, int *midi_input_chord/* given if insert from midikeyboard */, NedChordOrRest **newObj, bool force_rest);
		bool tryErease (double x, double y);
		bool findLine(double x, double y, double *ypos, int *line, double *bottom);
		bool findFirstElementeGreaterThan(double x, NedChordOrRest **element);
		bool findLastElementeLessThan(double x, NedChordOrRest **element);
		double getBottomPos();
		double getMidPos();
		double getTopPos() {return m_ypos;}
		double getWidth() {return m_width;}
		double getRealYPosOfLine(int line);
		void setWidth(double w) {m_width = w;}
		double getHeight();
		void empty();
		bool hasOnlyRests();
		void cutEmptyVoices();
		void handleEmptyMeasures();
		void zoomFreeReplaceables(double zoom, double scale);
		void recomputeFreeReplaceables();
		void computeBeams();
		void computeTuplets(bool has_repeat_lines);
		void computeTies();
		unsigned int getStaffDurationTotal();
		void assignMidiTimes();
		double computeMidDist(double y);
		void collectChordsAndRests(NedClipBoard *board, unsigned long long midi_start, unsigned long long midi_end);
		void collectDestinationVoices(NedClipBoard *board);
		int assignElementsToMeasures(NedMeasure *measures, int staff_offs, bool use_upbeat, int *special_measure_at_end, bool out = false);
		bool tryChangeLength(NedChordOrRest *chord_or_rest);
		void searchForBeamGroups(unsigned int midi_start_time);
		double getTopYBorder() {return m_top_y_border;}
		double getBottomYBorder() {return m_bottom_y_border;}
		void convertStaffRel(double x, double y, double *x_trans, double *y_trans);
		double placeStaff(double staffpos);
		int getNumberOfVoices(int staff_number);
		void saveStaff(FILE *fp);
		void restoreStaff(FILE *fp);
		void resetActiveFlags();
		void prepareReplay(bool with_keysig);
		void findAccidentals(char offs_array[115], NedMeasure *meas_info, unsigned long long midi_time, bool including);
		void setInternalPitches();
		void adjustAccidentals();
		void changeAccidentals(NedCommandList *command_list, int preferred_offs, GList *selected_group);
		void transpose(int pitchdist, GList *selected_group);
		void hideRests(NedCommandList *command_list, bool unhide, int voice_nr);
		bool shiftNotes(unsigned long long start_time, int linedist);
		void removeUnneededAccidentals(GList *selected_group = NULL);
		void appendWholeRest(NedCommandList *command_list = NULL);
		void getCurrentClefAndKeysig(unsigned long long miditime, int *clef, int *keysig, int *octave_shift);
		void appendAppropriateWholes(int start_measure);
		void setUpbeatRests(unsigned int upbeat_inverse);
		void testTies();
		void collectSelectionRectangleElements(double xp, double yp, NedBbox *sel_rect, GList **sel_group,
			bool is_first_selected, bool is_last_selected);
		void findSelectedStaffs(double yp, NedBbox *sel_rect, int *number_of_first_selected_staff, int *number_of_last_selected_staff);
		void pasteElements(NedCommandList *command_list, GList **elements, int from_staff, unsigned long long start_midi_time);
		bool hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, unsigned int long long midi_time);
		bool findFromTo(GList *clipboard);
		bool findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi);
		bool findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi);
		void deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi);
		void removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last);
		void insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time);
		void testForPageBackwardTies(NedCommandList *command_list);
		void checkForElementsToSplit(NedCommandList *command_list);
		void insertStaffElement(NedChordOrRest *elem, bool redraw = true);
		void removeStaffElement(NedChordOrRest *elem);
		void deleteStaffElements(bool unexecute, GList *elems, unsigned long long reftime, unsigned long duration);
		bool hasStaffElem(unsigned long long miditime);
		void insertIntoStaff(GList *elems, unsigned long long duration);
		void appendAtStaff(GList *elems);
		void setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, bool first);
		double getSystemEnd();
		double getSystemStart();
		GList *getFirstChordOrRest(int voice_nr, int lyrics_line, bool lyrics_required, bool note_required);
		GList *getLastChordOrRest(int voice_nr, int lyrics_line, bool lyrics_required, bool note_required);
		void determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found);
		bool detectVoices(unsigned int *voice_mask, unsigned long long *e_time);
		void exportLilyPond(FILE *fp, int voice_nr, int *last_line, unsigned int *midi_len,
				bool last_system, unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur, unsigned int *lyrics_map, bool with_break,
					 bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams);
		void exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len,
				bool last_system, unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len,
				bool last_system, unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyLyrics(FILE *fp, bool last_system, int voice_nr, int line_nr, unsigned long long end_time, int *sil_count);
		void collectLyrics(NedLyricsEditor *leditor);
		void setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor);
		GList *getStaffElements(unsigned long long midi_time);
		static const char m_sharpPos[7][7];
		static const char m_flatPos[7][7];
	private:
		GList *m_staffelems;
		NedVoice *m_voices[VOICE_COUNT];
		double m_ypos;
		double m_width;
		double m_top_y_border;
		int m_lyrics_lines;
		double m_bottom_y_border;
		int m_staff_number;
		int m_clef_type;
		int m_clef_octave_shift;
		int m_keysig;
		bool m_freetexts_or_lyrics_present;
		GList *m_volume_changes;
		NedSystem *m_system;
};

#endif /* STAFF_H */
