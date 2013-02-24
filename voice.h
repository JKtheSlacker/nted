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

#ifndef VOICE_H

#define VOICE_H
#include "config.h"

#include <gtk/gtk.h>
#include <cairo.h>

class NedMainWindow;
class NedPage;
class NedSystem;
class NedStaff;
class NedChordOrRest;
class NedCommandList;
class NedBeam;
class NedClipBoard;
class NedLyricsEditor;
class NedVoice;
class NedBbox;
class NedMeasure;
class NedSlur;
class SpecialMeasure;
class part;

struct VoiceInfoContainer {
	NedVoice *voice;
	NedStaff *staff;
	NedChordOrRest *element;
};

#define START_MEASURE_COUNT 4


class NedVoice {
	public:
		NedVoice(NedStaff *staff, int nr, bool start);
		~NedVoice();
		NedVoice *clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedStaff *p_staff);
		void adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist);
		void draw(cairo_t *cr, bool *freetexts_or_lyrics_present);
		void drawTexts(cairo_t *cr, double scale);
		int getBeamCount();
		int getPosition(NedChordOrRest *chord_or_rest);
		int getVoiceNumber() {return m_voice_nr;}
		void setVoiceNumber(int nr) {m_voice_nr = nr;}
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem();
		NedStaff *getStaff() {return m_staff;}
		int getNumElements();
		void prepareForImport() {m_start_of_last_imported = m_end_of_last_imported = NULL;}
		void appendElementsOfMeasureLength(part *part_ptr, unsigned int meas_duration, int meas_num);
		void copyDataOfWholeVoice();
		bool handleImportedTuplets();
		void handleEmptyMeasures();
		void zoomFreeReplaceables(double zoom, double scale);
		void recomputeFreeReplaceables();
		void removeLastImported();
		void handleStaffElements();
		bool trySelect (double x, double y, bool only_free_placeables);
		NedChordOrRest *findNearestElement(double x, double y, double factor, double *mindist, double *ydist);
		bool tryInsertOrErease (double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj, bool force_rest);
		bool tryConvertToTuplet(int method, int tuplet_val, NedChordOrRest *templ);
		double findTimePos(unsigned int time);
		void addBeam(NedBeam *beam);
		void removeBeam(NedBeam *beam);
		NedChordOrRest *getNextChordOrRest(NedChordOrRest *element);
		NedChordOrRest *getPreviousChordOrRest(NedChordOrRest *element);
		void selectNextChord(NedChordOrRest *chord, int line, bool lyrics_required);
		void selectPreviousChord(NedChordOrRest *chord, int line, bool lyrics_required);
		void computeBeams(int *lyrics_lines, double *topy, double *boty /*, bool *topFix, bool *botFix */, int staff_voice_offs);
		void computeTuplets(double *topy, double *boty);
		void computeTies();
		void removeTuplet(NedChordOrRest *element, NedCommandList *cmd_list = NULL);
		void insertIntoSystem(GList *chords_or_rests_to_move);
		void deleteNoteGroup(GList *chords_or_rests_to_move);
		void appendAtSystem(GList *chords_or_rests_to_move);
		bool truncateAtStart(NedCommandList *command_list, unsigned long long midi_time);
		int getSorting(NedChordOrRest *chord0, NedChordOrRest *chord1);
		bool tryChangeLength(NedChordOrRest *chord_or_rest);
		void empty();
		bool hasOnlyRests();
		bool isFirst(NedChordOrRest *chord);
		bool isLast(NedChordOrRest *chord);
		bool setNewXpos(NedChordOrRest *element, double newpos);
		void saveVoice(FILE *fp);
		void restoreVoice(FILE *fp);
		void restoreBeams(FILE *fp);
		void restoreTies(FILE *fp);
		void restoreTuplets(FILE *fp);
		void restoreFreePlaceables(FILE *fp);
		void prepareMutedReplay();
		void resetActiveFlags();
		void prepareReplay(int voice_idx);
		void findAccidentals(char offs_array[115], NedMeasure *meas_info, unsigned long long midi_time, bool including);
		void removeUnneededAccidentals(GList *selected_group);
		void setInternalPitches();
		void hideRests(NedCommandList *command_list, bool unhide);
		void adjustAccidentals();
		void changeAccidentals(NedCommandList *command_list, int preferred_offs, GList *selected_group);
		void setInternalPitch(int pitchdist, GList *selected_group);
		void adjustAccidentals(int pitchdist, GList *selected_group);
		void shiftNotes(unsigned long long start_time, int linedist, unsigned long long stop_time, bool stop_at);
		void assignMidiTimes();
		int assignElementsToMeasures(NedMeasure *measures, int offs, bool use_upbeat);
		void searchForBeamGroups(unsigned int midi_start_time, NedChordOrRest *new_chord_or_rest = NULL);
		bool findFirstElementeGreaterThan(double x, NedChordOrRest **element);
		bool findLastElementeLessThan(double x, NedChordOrRest **element);
		void collectChordsAndRests(NedClipBoard *board, unsigned long long midi_start, unsigned long long midi_end);
		void collectDestinationVoices(NedClipBoard *board);
		void collectSelectionRectangleElements(double xp, double yp, NedBbox *sel_rect, GList **sel_group, 
			bool is_first_selected, bool is_last_selected);
		void appendWholeRest(NedCommandList *command_list);
		void appendAppropriateWholes(int start_measure);
		void setUpbeatRests(unsigned int upbeat_inverse);
		void pasteElements(NedCommandList *command_list, GList **elements, int from_staff, unsigned int long long midi_time);
		bool hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, unsigned int long long midi_time);
		bool findFromTo(GList *clipboard);
		bool findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi);
		bool findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi);
		void deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi);
		void removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last);
		void testTies();
		void insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time);
		void testForPageBackwardTies(NedCommandList *command_list);
		void checkForElementsToSplit(NedCommandList *command_list);
		GList *getFirstChordOrRest(int lyrics_line, bool lyrics_required, bool note_required);
		GList *getLastChordOrRest(int lyrics_line, bool lyrics_required, bool note_required);
		void setBeamFlagsOf(NedCommandList *command_list, GList *selected_group, NedChordOrRest *start_elem);
		void isolate(NedCommandList *command_list, NedChordOrRest *element);
		void determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found);
		bool detectVoices(unsigned int *voice_mask, unsigned long long *e_time);
		void exportLilyPond(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
				unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur,
				unsigned int *lyrics_map, bool with_break, bool *guitar_chordnames, bool *chordname, int *breath_script, bool keep_beams);
		void exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
				unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
			unsigned long long end_time, bool *in_alternative, bool with_break);
		void exportLilyLyrics(FILE *fp, bool last_system, int line_nr, unsigned long long end_time, int *sil_count);
		void collectLyrics(NedLyricsEditor *leditor);
		void setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor);
	private:
		bool beamEndRequired(GList *beamlist_so_far, unsigned int beats, NedMeasure *measure);
		NedChordOrRest *handleGapAfterInsertion(NedCommandList *command_list, int *pos /* in/out */, int sum_of_chords_and_rests_to_delete,
			NedChordOrRest *chord_or_rest_to_distribute, int tuplet_val,  bool force_wholes, bool do_execute);
		NedChordOrRest *handleGapAfterInsertion2(NedCommandList *command_list, unsigned long long offs, int *pos /* in/out */,
			int sum_of_chords_and_rests_to_delete, NedChordOrRest *chord_or_rest_to_distribute,
			int tuplet_val, bool force_wholes, bool do_execute);

		bool searchForBreakableTuplet(NedCommandList *command_list, unsigned long long critical_time);
		void searchForBreakableTies(NedCommandList *command_list, unsigned int dir, unsigned long long start_time, unsigned long long end_time);
		void testForTupletEnd(unsigned long long start_time, unsigned int duration, unsigned long long *end_time);
		void testAllMeasuresForFillup(NedCommandList *command_list);
		void exportLilypondSpecMeasure(FILE *fp, unsigned int special, SpecialMeasure *spec_measure, bool *in_alternative, int measure_number);
		bool insertGrace(double x, double y, int pitch /* given if insert from midikeyboard */);
		GList *m_chord_or_rests;
		GList *m_beam_list;
		GList *m_tuplet_list;
		int m_voice_nr;
		NedStaff *m_staff;
		GList *m_start_of_last_imported, *m_end_of_last_imported;
		bool m_free_texts_or_lyrics_present;

	friend class NedInsertChordOrRestCommand;
	friend class NedEreaseChordOrRestCommand;
	friend class NedDeleteChordOrRestGroupCommand;
	friend class NedInsertChordOrRestGroupCommand;
	friend class NedInsertTiedElementCommand;
};

#endif /* VOICE_H */
