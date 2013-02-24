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

#ifndef SYSTEM_H

#define SYSTEM_H
#include "config.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include "measure.h"

class NedMainWindow;
class NedPage;
class NedStaff;
class NedClipBoard;
class NedCommandList;
class NedChordOrRest;
class NedLyricsEditor;
class NedBbox;
class NedSlur;
class part;

#define MAX_POSITIONS 4096
#define MAX_MEASURES 160
//#define WITH_WEIGHTS


class NedSystem {
	public:
		NedSystem(NedPage *page, double ypos, double width, int nr, unsigned int start_measure_number, bool start);
		~NedSystem();
		NedSystem *clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedPage *p_page, bool *staves);
		void adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist);
		void draw(cairo_t *cr, bool first_page, bool show_measure_numbers, bool *freetexts_or_lyrics_present);
		void drawTexts(cairo_t *cr, bool first_page, double scale);
		NedMainWindow *getMainWindow();
		NedPage *getPage() {return m_page;}
		void assignMidiTimes();
		void prepareForImport();
		void setInternalPitches();
		bool testLineOverflow();
		void appendElementsOfMeasureLength(part *parts, unsigned int meas_duration, int meas_num);
		void copyDataOfWholeStaff(int staff_nr);
		void removeLastImported();
		bool handleImportedTuplets();
		void handleStaffElements();
		void transpose(int pitchdist, bool *staff_list, GList *selected_group);
		void hideRests(NedCommandList *command_list, bool unhide, int staff_nr, int voice_nr);
		bool shiftNotes(unsigned long long start_time, int linedist, int staff_number);
		void adjustAccidentals(int staff_nr);
		void changeAccidentals(NedCommandList *command_list, int preferred_offs, bool *staff_list, GList *selected_group);
		void testBeams(char *s);
		void removeUnneededAccidentals(int staff_nr);
		bool trySelect (double x, double y, bool only_free_placeables);
		NedChordOrRest *findNearestElement(int staff_nr, double x, double y, NedStaff *oldstaff, double *mindist, double *ydist);
		NedChordOrRest *getFirstObjectInNextSystem(int staff_nr);
		bool tryInsertOrErease (double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj, bool force_rest);
		bool findLine(double x, double y, double *ypos, int *line, double *bottom, NedStaff **staff);
		bool findElement(guint keyval, double x, double y, NedChordOrRest **element, NedStaff **staff);
		double getHeight();
		double getYPos() {return m_ypos;}
		double getBottomPos();
		int getSystemNumber() {return m_system_number;}
		void setSystemNumber(int number) {m_system_number = number;}
		void changePageInfo(NedPage *page) {m_page = page;}
		int getNumberOfStaffs();
		void appendStaff(NedCommandList *command_list, int p_staff_nr = -1);
		void cutEmptyVoices();
		void handleEmptyMeasures();
		void zoomFreeReplaceables(double zoom, double scale);
		void recomputeFreeReplaceables();
		void testTies();
		void removeLastStaff();
		void deleteStaff(int staff_number);
		void restoreStaff(int staff_number);
		void shiftStaff(int staff_number, unsigned int position);
		bool reposit(int measure_number, NedCommandList *command_list = NULL, NedSystem **next_system  = NULL);
		void computeTuplets();
		void resetActiveFlags();
		void renumberMeasures(int *measure_number, GList *special_measures, bool force);
		void setNewMeasureSpread(double spread);
		int getNumberOfFirstMeasure();
		int getNumberOfLastMeasure();
		unsigned long long getSystemEndTime();
		void checkForElementsToSplit(NedCommandList *command_list, int *measure_number);
		void collectDestinationVoices(NedClipBoard *board);
		bool collectFirstMeasure(NedClipBoard *board, unsigned long long length_of_first_measure, unsigned long long length_of_previous_system);
		void findSelectedSystems(NedBbox *sel_rect, int *number_of_first_selected_staff, 
			int *number_of_last_selected_staff, NedSystem **first_selected_system, NedSystem **last_selected_system);
		void collectSelectionRectangleElements(double xp, NedBbox *sel_rect, GList **sel_group,
			NedSystem *first_selected_system, NedSystem *last_selected_system, bool is_first_selected_page,
			bool is_last_selected_page);
		void pasteElements(NedCommandList *command_list, GList **elements, int from_staff, int to_staff, unsigned long long start_midi_time);
		bool hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, int to_staff, unsigned long long start_midi_time);
		bool findFromTo(GList *clipboard, NedSystem **min_sys, NedSystem **max_sys);
		bool truncateAtStart(NedCommandList *command_list, unsigned long long midi_time);
		bool findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi);
		bool findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi);
		void deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi);
		void removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last);
		void insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time);
		void testForPageBackwardTies(NedCommandList *command_list);
		double computeMidDist(double y);
		NedMeasure *getMeasure(unsigned long long  midi_time);
		NedMeasure *getMeasureNr(int m_number) {return &(m_measures[m_number]);}
		NedMeasure *getLastMeasure() {return &(m_measures[m_measure_count]);}
		bool findTimeOfMeasure(int meas_num, unsigned long long *meas_time);
		int getMeasureCount() {return m_measure_count;}
		int getFullMeasureCount() {return m_measure_count + m_special_measure_at_end;}
		int getSpecialMeasuresAtEnd() {return m_special_measure_at_end;}
		bool tryChangeLength(NedChordOrRest *chord_or_rest);
		double placeStaffs(double staffpos);
		NedStaff *findStaff(double x, double y, NedMeasure **measure);
		void empty();
		bool hasOnlyRests();
		void shiftY(double offs) {m_ypos += offs;}
		void saveSystem(FILE *fp);
		void restoreSystem(FILE *fp);
		void prepareReplay(bool with_keysig);
		unsigned int getWholeMidiLength();
		void setWidth(double w);
		void do_remove_staff(NedStaff *staff);
		void do_append_staff(NedStaff *staff);
		void setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, double indent, bool first);
		void fill_up(NedCommandList *command_list);
		GList *getFirstChordOrRest(int staff_nr, int voice_nr, int lyrics_line, bool lyrics_required, bool note_required);
		GList *getLastChordOrRest(int staff_nr, int voice_nr, int lyrics_line, bool lyrics_required, bool note_required);
		NedStaff *getStaff(int nr);
		void determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found);
		void detectVoices(int staff_nr, unsigned int *voice_mask, NedSystem **last_system, unsigned long long *end_time);
		void exportLilyPond(FILE *fp, int staff_nr, int voice_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur,
				unsigned int *lyrics_map, bool with_break, bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams);
		void exportLilyGuitarChordnames(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyFreeChordName(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyLyrics(FILE *fp, bool last_system, int staff_nr, int voice_nr, int line_nr, unsigned long long end_time, int *sil_count);
		double m_system_start;
		void collectLyrics(NedLyricsEditor *leditor, int staff_nr);
		void setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor, int staff_nr);
		bool m_is_positioned;
	private:
		void drawBrace (cairo_t *cr, double leftx, double indent, double topy, double zoom_factor, double height, double toppos);
		void drawBracket (cairo_t *cr, double leftx, double indent, double topy, double zoom_factor, double height, double toppos);
		bool m_tempUntouched;
		bool m_check_line_compression;
		void do_reposit(bool use_upbeat);
		void compute_extra_space(bool use_upbeat);
		GList *m_staffs;
		GList *m_deleted_staffs;
		double m_ypos;
		double m_width;
		int m_system_number;
		bool m_has_repeat_lines;
		NedPage *m_page;
		NedMeasure m_measures[MAX_MEASURES];
		int m_measure_count;
		double m_ori,m_netto;
		double m_extra_space;
		bool m_only_whole_elements;
		NedMeasure *m_endMeasure;
		bool m_freetexts_or_lyrics_present;
		int m_special_measure_at_end; // last "measure" aith only keysig and/or tinmesig ?
};

#endif /* SYSTEM_H */
