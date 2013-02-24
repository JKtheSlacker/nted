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

#ifndef PAGE_H

#define PAGE_H
#include "config.h"
#include "resource.h"

#include <gtk/gtk.h>
#include <cairo.h>

class NedMainWindow;
class NedClipBoard;
class NedSystem;
class NedStaff;
class NedMeasure;
class NedCommandList;
class NedLyricsEditor;
class NedSlur;
class part;

class NedPage {
	public:
		NedPage(NedMainWindow *main_window, double width, double height, int nr, unsigned int start_measure_number, bool start);
		~NedPage();
		NedPage *clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedMainWindow *p_main_window, bool *staves);
		void adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist);
		NedSystem *appendSystem();
		void testBeams(char *s);
		void testForKeysig(char *s);
		void removeSystem(NedSystem *system);
		void insertSystem(NedSystem *system);
		void appendSystem(NedCommandList *command_list);
		bool isPageOverflow();
		bool isEmpty() {return g_list_length(m_systems) == 0;}
		bool hasOnlyRests();
		bool isXOnPage(double x);
		void draw(cairo_t *cr, bool show_measure_numbers, double main_width, double main_height);
		double getTopPos();
		double getLeftPos();
		double getWidth() {return m_width;}
		double getHeight() {return m_height;}
		double getXPos() {return m_xpos;}
		unsigned int getNumberOfLastMeasure();
		void setPageNumber(int num) {m_page_number = num; m_xpos = num * (m_width + DEFAULT_BORDER);}
		double getContentXpos();
		double getContentWidth() {return m_width - 2 * LEFT_RIGHT_BORDER;}
		NedMainWindow *getMainWindow() {return m_main_window;}
		void copyDataOfWholeStaff(int staff_nr);
		void handleStaffElements();
		void adjustAccidentals(int staff_nr);
		void changeAccidentals(NedCommandList *command_list, int preferred_offs, bool *staff_list, GList *selected_group);
		void setInternalPitches();
		void transpose(int pitchdist, bool *staff_list, GList *selected_group);
		void hideRests(NedCommandList *command_list, bool unhide, int staff_nt, int voice_nr);
		bool shiftNotes(unsigned long long start_time, int linedist, NedSystem *start_system, int staff_number);
		void insertSlur(GdkRectangle *selection_rect);
		void removeUnneededAccidentals(int staff_nr = -1);
		bool trySelect(double x, double y, bool only_free_placeables = false);
		NedChordOrRest *findNearestElement(int staff_nr, double x, double y, NedStaff *oldstaff, double *ydist);
		bool tryInsertOrErease(double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj = NULL, bool force_rest = false);
		void setAllUnpositioned();
		void zoomFreeReplaceables(double zoom, double scale);
		double convertX(double xpos);
		bool findLine(double x, double y, double *ypos, int *line, double *bottom, NedStaff **staff);
		bool findXposInOtherMeasure(guint keyval, double x, double y, double *newx, double *newy);
		int getPageNumber() {return m_page_number;}
		bool findTimeOfMeasure(int meas_num, unsigned long long *meas_time, unsigned long long *system_offs);
		void resetCollision();
		void findSelectedFirstsLasts(NedBbox *sel_rect, int *number_of_first_selected_staff, int *number_of_last_selected_staff, 
			NedSystem **first_selected_system, NedSystem **last_selected_system,
			NedPage **first_selected_page, NedPage **last_selected_page);
		void collectSelectionRectangleElements(NedBbox *sel_rect, GList **sel_group,
			NedSystem *first_selected_system, NedSystem *last_selected_system,
			NedPage *first_selected_page, NedPage *last_selected_page);
		int getSorting(NedStaff *this_staff, NedSystem *this_system, NedStaff *other_staff, NedSystem *other_system);
		NedSystem *getNextSystem(NedSystem *system, NedCommandList *command_list = NULL);
		NedSystem *getPreviousSystem(NedSystem *system);
		NedSystem *getFirstSystem();
		NedPage *getNextPage();
		NedPage *getPreviousPage();
		void renumberSystems();
		void resetActiveFlags();
		bool isFirst(NedSystem *system);
		bool isLastPage();
		bool isLast(NedSystem *system);
		bool isLastSystem(NedSystem *system);
		NedSystem *getLastSystem();
		bool tryChangeLength(NedChordOrRest *chord_or_rest);
		bool placeStaffs(int pass, NedCommandList *command_list = NULL);
		void computeTuplets();
		int getNumberOfStaffs();
		NedStaff *findStaff(double x, double y, NedMeasure **measure);
		void savePage(FILE *fp);
		void restorePage(FILE *fp);
		void prepareReplay(bool with_keysig = false);
		void appendStaff(NedCommandList *command_list, int p_staff_nr = -1);
		void removeLastStaff();
		void deleteStaff(int staff_number);
		void restoreStaff(int staff_number);
		void shiftStaff(int staff_number, int position);
		void getLinePoints(NedStaff *start_staff, NedStaff *end_staff, GList **staves);
		void reconfig_paper(double width, double height);
		void findFromTo(GList *clipboard, NedPage **min_page, NedPage **max_page, NedSystem **min_sys, NedSystem **max_sys);
		void deleteItemsFromTo(NedCommandList *command_list, bool is_first_page, bool is_last_page, NedSystem *min_sys, NedSystem *max_sys,
				unsigned long long start_midi, unsigned long long end_midi);
		void removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first_page, bool is_last_page,
			NedSystem *min_sys, NedSystem *max_sys);
		void testForPageBackwardTies(NedCommandList *command_list);
		void checkForElementsToSplit(NedCommandList *command_list, int *measure_number);
		void setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, bool first);
		GList **getSystemsList() {return &m_systems;}
		bool find_staff_and_line(int x, int y, NedStaff **last_staff, int *last_line);
		void determineTempoInverse(NedChordOrRest *element, NedSystem* system, double *tempoinverse, bool *found);
		void detectVoices(int staff_nr, unsigned int *voice_mask, NedSystem **last_system, unsigned long long *end_time);
		void cutEmptyVoices();
		void handleEmptyMeasures();
		void testTies();
		int getSystemCount();
		bool exportLilyPond(FILE *fp, int staff_nr, int voice_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alernative, NedSlur **lily_slur,
				unsigned int *lyrics_map, bool with_break, bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams);
		bool exportLilyGuitarChordnames(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alernative, bool with_break);
		bool exportLilyFreeChordName(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
			NedSystem *last_system, unsigned long long end_time, bool *in_alernative, bool with_break);
		bool exportLilyLyrics(FILE *fp, int staff_nr, int voice_nr, int line_nr, NedSystem *last_system, unsigned long long end_time, int *sil_count);
		void collectLyrics(NedLyricsEditor *leditor, int staff_nr);
		void setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor, int staff_nr);
		void recomputeFreeReplaceables();
	private:
		void do_place_staffs(double offs);
		GList *m_systems;
		double m_xpos;
		double m_system_diff;
		double m_width, m_height;
		const double default_border;
		int m_page_number;
		bool m_ping_pong;
		NedMainWindow *m_main_window;

	friend class NedDeleteSystemCommand;
	friend class NedAppendSystemCommand;
	friend class NedMoveSystemCommand;
};

#endif /* PAGE_H */
