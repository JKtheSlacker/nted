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

#ifndef MAINWINDOW_H

#define MAINWINDOW_H
#include "config.h"
#include <sys/time.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-features.h>
#include "resource.h"
#include "paths.h"

//#define WITH_BANNER

class NedPage;
class NedNote;
class NedCommandHistory;
class NedCommandList;
class NedChordOrRest;
class NedSystem;
class NedStaff;
class NedFreeReplaceable;
class NedFreeText;
class NedImporter;
class NedMidiRecorder;
class NedLyricsEditor;
class NedMusicXMLImport;
class NedPangoCairoText;
class ScoreInfo;
struct paper_info_struct;

struct staff_context_str {
	NedPangoCairoText *m_staff_name;
	NedPangoCairoText *m_staff_short_name;
	NedPangoCairoText *m_group_name;
	NedPangoCairoText *m_group_short_name;
	int m_key_signature_number;
	int m_clef_number;
	int m_clef_octave_shift;
	bool m_different_voices;
	struct {
		unsigned char m_midi_volume;
		unsigned char m_midi_pan;
		unsigned char m_midi_program;
		unsigned char m_current_midi_program;
	} voices[VOICE_COUNT];
	char m_play_transposed;
	unsigned char m_midi_chorus;
	unsigned char m_midi_reverb;
	unsigned char m_midi_channel;
	unsigned char m_effective_channel;
	unsigned int m_flags;
#define BRACE_START   (1 << 0)
#define BRACE_END     (1 << 1)
#define BRACKET_START (1 << 2)
#define BRACKET_END   (1 << 3)
#define CON_BAR_START (1 << 4)
#define CON_BAR_END   (1 << 5)
	int m_pitch_offs;
	bool m_muted[VOICE_COUNT];

};

class SpecialMeasure {
	public:
		SpecialMeasure() {type = 0; measure = NULL; symbol = TIME_SYMBOL_NONE;}
		void setTimeSignature(unsigned short num, unsigned short denom, unsigned int sym) {
			numerator = num; denominator = denom;
			symbol = sym;
			measure_duration = numerator * WHOLE_NOTE / denominator;
		}
#define FIELD_BITS 8
#define REP_TYPE_START_BIT 0
#define START_TYPE_START_BIT 8
#define END_TYPE_START_BIT 16 
#define TIMESIG_TYPE_START_BIT 24

#define REP_TYPE_MASK (((1 << FIELD_BITS) - 1) << REP_TYPE_START_BIT)
#define START_TYPE_MASK (((1 << FIELD_BITS) - 1) << START_TYPE_START_BIT)
#define END_TYPE_MASK (((1 << FIELD_BITS) - 1) << END_TYPE_START_BIT)
#define TIMESIG_TYPE_MASK (((1 << FIELD_BITS) - 1) << TIMESIG_TYPE_START_BIT)

		unsigned int type;
#define REPEAT_OPEN  (1 << REP_TYPE_START_BIT)
#define REPEAT_CLOSE (2 << REP_TYPE_START_BIT)
#define DOUBLE_BAR   4
#define END_BAR      5
#define REPEAT_OPEN_CLOSE 3
#define REP1START (1 << START_TYPE_START_BIT)
#define REP1END   (1 << END_TYPE_START_BIT)
#define REP2START (2 << START_TYPE_START_BIT)
#define REP2END   (2 << END_TYPE_START_BIT)
#define TIMESIG   (1 << TIMESIG_TYPE_START_BIT)
		int measure_number;
		bool hide_following;
		unsigned int measure_duration;
		NedMeasure *measure;
		unsigned short numerator, denominator;
		unsigned int symbol;
		bool isNear(double x, double y);
		static int compare_spec_measures(const SpecialMeasure *a, const SpecialMeasure *b);
};

#define NO_LYRICS -1 
#define LYRICS1   0
#define LYRICS2   1
#define LYRICS3   2
#define LYRICS4   3
#define LYRICS5   4

#define MAX_STAFFS 100

typedef struct {
	struct chord_str *chord_ptr;
	char chord_name_num;
	unsigned int status;
} chord_info_struct;

typedef struct {
	char *root_name;
	char *up_name;
	char *down_name;
	double fontsize;
	double ydist;
} chord_name_info_struct;


typedef union {
	int m_special_sub_type_int;
	chord_info_struct m_chord_info;
	chord_name_info_struct m_chordname_info;
} t_special_subtype;
	
	

class NedMainWindow {
	public:
		NedMainWindow();
		~NedMainWindow();
		void createLayout(char *fname, char *yelparg);
		double getCurrentZoomFactor() {return m_current_zoom;}
		int getCurrentZoomLevel() {return m_current_zoom_level;}
		double getCurrentScale() {return (double) m_current_scale / (double) SCALE_GRANULARITY;}
		unsigned int getMainWindowStatus();
		bool getShowHidden();
		bool hasFocus();
		int getLyricsMode();
		void collectLyrics(NedLyricsEditor *leditor, int staff_nr);
		void updateLyrics(NedLyricsEditor *leditor, int staff_nr);
		NedCommandHistory *getCommandHistory() {return m_command_history;}
		double getLeftX();
		double getTopY();
		NedNote *m_selected_note;
		NedChordOrRest *m_selected_chord_or_rest;
		NedChordOrRest *m_selected_stem;
		NedFreeReplaceable *m_selected_free_replaceable;
		SpecialMeasure *m_selected_spec_measure;
		NedNote *m_selected_tie_forward;
		GdkCursor *m_hand, *m_pointer, *m_pencil;
		NedSystem *getNextSystem(NedPage *page, NedCommandList *command_list = NULL);
		NedPage *getNextPage(NedPage *page, NedCommandList *command_list = NULL);
		NedPage *getPreviousPage(NedPage *page);
		int getSorting(NedPage *this_page, NedStaff *this_staff, NedPage *other_page, NedStaff *other_staff);
#ifdef WITH_TIME_TEST
		struct timeval m_last_mouse_motion_time;
#endif
		void setUnRedoButtons(bool execute_possible, bool unexecute_possible);
		void repaint();
		void repaintDuringReplay(NedNote *notes[], int num_notes);
		void reposit(NedCommandList *command_list = NULL, NedPage *start_page = NULL, NedSystem *start_system = NULL,
			bool with_progress_bar = false);
		void setNewMeasureSpread(double spread);
		void renumberMeasures(NedSystem *start_system = NULL, int measure_number = 1, bool force = false);
		staff_context_str m_staff_contexts[MAX_STAFFS];
		int getNumerator() {return m_numerator;}
		int getDenominator() {return m_denominator;}
		bool isTimsigChangingMeasure(int meas_num);
		unsigned int getMeasureDuration(int meas_num);
		double getGlopabSpreadFac() {return m_global_spacement_fac;}
		bool findTimeOfMeasure(int meas_num, unsigned long long *meas_time);
		void changeLength();
		void changeState();
		void changeDots();
		void changeNoteHead();
		void changeTieState();
		void changeNoteState();
		void changeTremolo();
		int getNumerator(int meas_num);
		int getDenominator(int meas_num);
		unsigned int getTimesigSymbol(int meas_num);
		unsigned int getTimesigSymbolOfThisMeasure(int meas_num);
		int getNumeratorOfThisMeasure(int meas_num);
		int getDenominatorOfThisMeasure(int meas_num);
		int getPreStaffCount() {return m_pre_staffcount;}
		void stopReplay();
		void setVisibleSystem(NedSystem *system);
		//void setVisiblePage(NedPage *page);
		void setVisible(NedNote *note);
		void setVisible(NedChordOrRest *chor_or_rest);
		void setSelected(NedChordOrRest *chord_or_rest, NedNote *note);
		NedPage *getLastPage();
		ScoreInfo *getScoreInfo() {return m_score_info;}
		double getFirstPageYOffs() {return m_first_page_yoffs;}
		double getMidiTempoInverse() {return m_midi_tempo_inverse;}
		void updatePageCounter();
		void renumberPages();
		int getStaffCount() {return m_staff_count;}
		void do_import_midi(bool use_id_ed);
		void do_remove_last_staff();
		bool do_import_from_other(NedImporter *im);
		void do_lily_export(FILE *fp, bool with_break, bool *selected_staves, bool *kept_beams);
		void do_transpose(int pitchdist, bool *staff_list, GList *selected_group);
		void shiftNotes(unsigned long long start_time, int linedist, NedPage *start_page, NedSystem *start_system, int staff_number);
		int setChannelFromCursor();
		void deleteStaff(int staff_number);
		void shiftStaff(int staff_number, int position);
		void restoreStaff(int staff_number, staff_context_str *staff_context);
		double determineTempoInverse(NedChordOrRest *element);
		void setInternalPitches();
		void adjustAccidentals(int staff_nr);
		void insertNoteWithPitch(int num_midi_input_notes, int *midi_input_chord);
		GtkWidget *getDrawingArea() {return m_drawing_area;}
		GtkWidget *getWindow() {return m_main_window;}
		struct paper_info_struct *getCurrentPaper() {return m_current_paper;}
		bool getPortrait() {return m_portrait;}
		void reconfig_paper(bool do_reposit, struct paper_info_struct *paper_type, bool portrait, int scale);
		void resetPointerLastTouchedSystems() {m_last_touched_system = NULL;}
		int getCurrentVoice();
		bool doPaintColored() {return m_paint_colored;}
		bool doDrawPostscript() {return m_draw_postscript;}
		int m_start_measure_number_for_renumbering;
		NedSystem *m_start_system_for_renumbering;
		bool needsARepLine(int measure_number);
		int getSpecialType() {return m_special_type;}
		int getDoAdjust() {return ((m_special_type & (0xef000000)) >> 24);}
		int getSpecialSubTypeInt() {return m_special_sub_type.m_special_sub_type_int;}
		int getSpecialSubTypeChordname() {return m_special_sub_type.m_special_sub_type_int;}
		chord_info_struct getSpecialSubTypeChordInfo() {return m_special_sub_type.m_chord_info;}
		chord_name_info_struct getSpecialSubTypeChordNameInfo() {return m_special_sub_type.m_chordname_info;}
		unsigned int getSpecial(int meas_num);
		SpecialMeasure *getSpecialMeasure(int meas_num);
		void resetSpecialType() {m_special_type = m_special_sub_type.m_special_sub_type_int = -1;
				m_special_sub_type.m_chord_info.chord_ptr = NULL;}
		void setChorNameInfo(char *rootname, char *uptext, char *downtext, double fontsize, double ydist);
		void setAndUpdateClefTypeAndKeySig();
		double get2ndNettoIndent() {return m_2ndnetto_indent;}
		double getNettoIndent() {return m_netto_indent;}
		double getFirstSystemIndent() {return m_first_system_indent;}
		double get2ndSystemIndent() {return m_2nd_system_indent;}
		unsigned int getUpBeatInverse() {return m_upbeat_inverse;}
		int getFileVersion() {return m_file_version;}
		void turnOffKeyboardCtrlMode() { m_keyboard_ctrl_mode= false; }
		void resetMidiInput();
		void setAllUnpositioned();
		NedFreeReplaceable *m_freetext;
		void drawVisibleRectangle(cairo_t *cr, NedNote *note);
		void resetActiveFlags();
	private: 
		GList *m_special_measures;
		int setSpecial(int measnum, int mtype, bool hide, bool *oldhide, bool overwrite);
		void setSpecialTimesig(int measnum, int num, int denom, unsigned int newsym, int *oldnum, int *olddenom, unsigned int *oldsym, bool do_reposit = true);
		static gint compare_spec_measures_according_measnum(gconstpointer ptr1, gconstpointer ptr2);
		void readSpecMeasures(FILE *fp, GList **new_spec_measures);
		bool conflictWithOhterSpecMeasure(int measnum, int dir);
		void testAlternativeState(NedCommandList *command_list, NedMeasure *measure, bool alter1, bool alter2);
		void checkForElementsToSplit(NedCommandList *command_list);
		bool import_from_other(NedImporter *im, char* filename, bool execute_import);
		void deleteEmptyPagesAtEnd();
		void deletePagesWithoutSystem();
		void removeEmptyInteriourPages();
		void rezoomScoreInfo();
		void adjustAfterZoom();
		void find_new_cursor_pos(guint keyval, int *x, int *y);
		bool find_new_cursor_pos_at_measure(guint keyval, int *x, int *y);
		int computeLilySize();
		void disconnectKeys();
		NedChordOrRest *findAndBeamElementsOfCurrentVoice();
		void zoomFreeReplaceables(double zoom, double scale);
		void resetSomeButtons();
		void resetLyricsMode();
		void moveSpecMeasure(SpecialMeasure *spec, int dir);
		bool deleteEnclosedBras(unsigned int starts, unsigned int ends);
		bool deleteEnclosedConnections();
		void computeSystemIndent();
		void tryTieBackward();
		void modeChange(unsigned int stat, GtkAction *action);
		char *m_start_file_name;
		int m_last_line_during_key_insertion;
		NedStaff *m_last_staff_during_key_insertion;
		double m_current_zoom;
		int m_current_scale; // * 50!!! avoid 1.e-10
		double m_leftx, m_topy;
		double m_leftx0, m_topy0;
		double m_mouse_x, m_mouse_y;
		int m_current_zoom_level;
		int m_special_type;
		t_special_subtype m_special_sub_type;
		GtkWidget *m_drawing_area;
		GtkWidget *m_page_selector;
		GtkIMContext *m_im_context;
		static cairo_status_t writefunc(void *fdv,  const unsigned char *data, unsigned int length);
#define STREAM_FILE_TYPE_PS 1
#define STREAM_FILE_TYPE_PDF 2
		void do_export_to_stream(FILE *psfile, int stream_file_type);
		GList *m_pages;
		void updateRecentFiles();
		int m_recentFileMergeId;
		NedCommandHistory *m_command_history;
		void realizeNewSpacement(double spacement);
		void do_restore(FILE *fp, char *filename);
		void do_staff_config(int staff_number, NedSystem *system);
		void setToPage(int page_nr);
		void computeScoreTextExtends();
		int m_numerator, m_denominator;
		unsigned int m_timesig_symbol;
		int m_staff_count, m_pre_staffcount;
		void replay(bool on);
		bool m_config_changed;
		double m_midi_tempo_inverse;
		NedSystem *m_last_touched_system;
		struct timeval m_last_motion_call;
		ScoreInfo *m_score_info;
		double m_first_page_yoffs;
		unsigned int m_upbeat_inverse;

		GtkUIManager *m_ui_manager;
		GtkWidget *m_customize_beam_button;
		GtkAction *m_undo_action, *m_redo_action;
		GtkToggleAction *m_insert_erease_mode_action;
		GtkToggleAction *m_shift_action, *m_keyboard_insert_action;
		GtkToggleAction *m_midi_input_action, *m_midi_record_action;
		GtkToggleAction *m_show_hidden_action;
		GtkToggleAction *m_replay_action;
		GtkToggleAction *m_show_measure_numbers_action;
		GtkActionGroup *m_menu_action_group;
		GtkActionGroup *m_open_recent_action_group;
		GSList *m_voice_buttons;
		GtkAction *m_recent_actions[MAX_RECENT_FILES];
		GtkAction *m_remove_page_action;
		GtkWidget *m_v1bu, *m_v2bu, *m_v3bu, *m_v4bu;
		GtkWidget *m_main_vbox;

		void draw (cairo_t *cr, int width, int height);
		void setTopLeft(double x, double y);
		void setTopRelative(double y_incr);
		bool setPageRelative(int y_incr);
		void setButtons();
		void resetButtons();
		void prepareInsertion();

		char m_current_filename[4096];

		static const GtkActionEntry main_tools[];
		static const GtkToggleActionEntry accessory_buttons[];
		static const GtkToggleActionEntry toggle_mode_buttons[];
		static const GtkToggleActionEntry insert_erease_button[];
		static const GtkActionEntry file_entries[];
		static const char *guiDescription;
		static gboolean handle_expose (GtkWidget *widget,
			 GdkEventExpose *event, gpointer data);
		static gboolean scroll_event_handler(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		GtkWidget *m_main_window;
		void storeScore(FILE *fp);
		GdkRectangle m_selection_rect;
		GList *m_selected_group;

		static void zoom_in(GtkWidget *widget, void *data);
		static void zoom_out(GtkWidget *widget, void *data);
		static void quit_app(GtkWidget *widget, void *data);
		static void new_file(GtkWidget *widget, void *data);
		static void save_score(GtkWidget *widget, void *data);
		static void save_score_as(GtkWidget *widget, void *data);
		static void restore_score(GtkWidget *widget, void *data);
		static void print_file(GtkWidget *widget, void *data);
		static void import_musicxml(GtkWidget *widget, void *data);
		static void import_midi(GtkWidget *widget, void *data);
		static void import_midi2(GtkWidget *widget, void *data);
		static void export_midi(GtkWidget *widget, void *data);
		static void export_lily(GtkWidget *widget, void *data);
		static void insert_tuplet(GtkWidget *widget, void *data);
		static void set_brace_system_delimiter(GtkWidget *widget, void *data);
		static void set_bracket_system_delimiter(GtkWidget *widget, void *data);
		static void set_connected_bar_line(GtkWidget *widget, void *data);
		static void remove_system_delimiter(GtkWidget *widget, void *data);
		static void remove_tuplet(GtkWidget *widget, void *data);
		static void OnCommit(GtkIMContext *context, const gchar  *str,  void *data);
/*		static void append_page(GtkWidget *widget, void *data); 
		static void insert_page(GtkWidget *widget, void *data); */
		static void insert_keysig(GtkWidget *widget, void *data);
		static void insert_lines(GtkWidget *widget, void *data);
		static void insert_signs(GtkWidget *widget, void *data);
		static void insert_slur(GtkWidget *widget, void *data);
		static void insert_spacer(GtkWidget *widget, void *data);
		static void insert_chord(GtkWidget *widget, void *data);
		static void insert_chord_name(GtkWidget *widget, void *data);
		static void insert_accelerando(GtkWidget *widget, void *data);
		static void insert_ritardando(GtkWidget *widget, void *data);
		static void append_staff(GtkWidget *widget, void *data);
		static void lyrics_editor(GtkWidget *widget, void *data);
		static void insert_ppp(GtkAction *widget, gpointer data);
		static void insert_pp(GtkAction *widget, gpointer data);
		static void insert_p(GtkAction *widget, gpointer data);
		static void insert_mp(GtkAction *widget, gpointer data);
		static void insert_sp(GtkAction *widget, gpointer data);
		static void insert_mf(GtkAction *widget, gpointer data);
		static void insert_sf(GtkAction *widget, gpointer data);
		static void insert_f(GtkAction *widget, gpointer data);
		static void insert_ff(GtkAction *widget, gpointer data);
		static void insert_fff(GtkAction *widget, gpointer data);
		static void insert_tempo(GtkAction *widget, gpointer data);
		static void insert_text(GtkAction *widget, gpointer data);
		static void set_lyrics_mode1(GtkAction *widget, gpointer data);
		static void set_lyrics_mode2(GtkAction *widget, gpointer data);
		static void set_lyrics_mode3(GtkAction *widget, gpointer data);
		static void set_lyrics_mode4(GtkAction *widget, gpointer data);
		static void set_lyrics_mode5(GtkAction *widget, gpointer data);
		static void remove_page(GtkWidget *widget, void *data);
		static void remove_empty_pages(GtkWidget *widget, void *data);
		static void change_spacement(GtkWidget *widget, void *data);
		static void change_scale(GtkWidget *widget, void *data);
		static void config_record(GtkWidget *widget, void *data);
		static void config_drums(GtkWidget *widget, void *data);
		static void popup_tools(GtkWidget *widget, void *data);
		static void set_upbeat_measure(GtkWidget *widget, void *data);
		static void set_upbeat_start(GtkWidget *widget, void *data);
		static void edit_score_info(GtkWidget *widget, void *data);
		static void mute_staves(GtkWidget *widget, void *data);
		static void transpose(GtkWidget *widget, void *data);
		static void set_all_halfs_to_sharp(GtkWidget *widget, void *data);
		static void set_all_halfs_to_flat(GtkWidget *widget, void *data);
		static void hide_rests(GtkWidget *widget, void *data);
		static void unhide_rests(GtkWidget *widget, void *data);
		static void hide_rests_of_all_voices(GtkWidget *widget, void *data);
		static void unhide_rests_of_all_voices(GtkWidget *widget, void *data);
		static void config_midi_out(GtkWidget *widget, void *data);
		static void config_midi_in(GtkWidget *widget, void *data);
		static void config_paper(GtkWidget *widget, void *data);
		static void show_about(GtkWidget *widget, void *data);
#ifdef YELP_PATH
		static void show_docu(GtkWidget *widget, void *data);
#endif
		static void show_license(GtkWidget *widget, void *data);
		static void config_print_cmd(GtkWidget *widget, void *data);
		static void config_meas_num_font(GtkWidget *widget, void *data);
		static void do_undo(GtkWidget *widget, void *data);
		static void do_redo(GtkWidget *widget, void *data);
		static void select_voice(GtkButton *button, gpointer data);
		static void copy_data(GtkWidget *widget, void *data);
		static void paste_data(GtkWidget *widget, void *data);
		static void delete_block(GtkWidget *widget, void *data);
		static void append_system(GtkWidget *widget, void *data);
		static void importRecorded(GtkWidget *widget, void *data);
		static void insert_block(GtkWidget *widget, void *data);
		static void insert_clef(GtkWidget *widget, void *data);
		static void reposit_all(GtkWidget *widget, void *data);
		static void empty_block(GtkWidget *widget, void *data);
		static void toggle_insert_erease_mode(GtkAction *action, gpointer data);
		static void prepare_keyboard_insertion(GtkAction *action, gpointer data);
		static void customize_beam(GtkAction *action, gpointer data);
		static void flip_stem(GtkAction *action, gpointer data);
		static void do_play(GtkAction *action, gpointer data);
		static void initiate_repaint(GtkAction *action, gpointer data);
		static void set_paint_colored(GtkAction *action, gpointer data);
		static void toggle_shift_mode(GtkAction *action, gpointer data);
		static void toggle_show_hidden(GtkAction *action, gpointer data);
		static void toggle_midi_input(GtkAction *action, gpointer data);
		static void toggle_record_midi(GtkAction *action, gpointer data);
		static void size_change_handler(GtkWidget *widget, GtkRequisition *requisition, gpointer data);
		static gboolean handle_delete(GtkWidget *widget, GdkEvent *event, gpointer data);
		static void handle_page_request(GtkSpinButton *spinbutton, gpointer data);

		void closeApplication();
		void adjustView();
		void setCursor();
		int m_staff_numbers;
		bool m_avoid_feedback;
		bool m_avoid_feedback_action;
		int m_lyrics_mode;

		static void OnSetFocus(GtkWindow *window, GtkWidget *widget, gpointer data);  
		static gboolean handle_button_press (GtkWidget *widget,
			 GdkEventButton *event, gpointer data);
		static gboolean handle_button_release (GtkWidget *widget,
			 GdkEventButton *event, gpointer data);
		static gboolean key_press_handler (GtkWidget *widget,  GdkEventKey *event,
                                                        gpointer  data);
		static gboolean key_release_handler (GtkWidget *widget,  GdkEventKey *event,
                                                        gpointer  data);
		static gboolean window_leave_handler (GtkWidget *widget,  GdkEventCrossing *event,
                                                        gpointer  data);
		static gboolean window_enter_handler (GtkWidget *widget,  GdkEventCrossing *event,
                                                        gpointer  data);

		static gboolean handle_motion (GtkWidget *widget,
	       			GdkEventMotion *event, gpointer data);

		static void write_png(GtkWidget *widget, void *data);
#ifdef CAIRO_HAS_SVG_SURFACE
		static void write_svg(GtkWidget *widget, void *data);
#endif
		static void write_ps(GtkWidget  *widget, void *data);
#ifdef CAIRO_HAS_PDF_SURFACE
		static void write_pdf(GtkWidget  *widget, void *data);
#endif
		static void new_window(GtkWidget  *widget, void *data);
		static void create_excerpt(GtkWidget  *widget, void *data);
		static bool close_window2(GtkWidget *widget, GdkEvent  *event, gpointer data);
		static void close_window(GtkWidget  *widget, void *data);
		static void open_recent(GtkAction *action, gpointer user); 
		NedMidiRecorder *m_midi_recorder;
		double m_pointer_xpos, m_pointer_ypos;
		struct paper_info_struct *m_current_paper;
		bool m_paint_colored;
		bool m_portrait;
		bool m_keyboard_ctrl_mode;
		bool m_draw_postscript;
		double m_netto_indent;
		double m_2ndnetto_indent;
		double m_first_system_indent;
		double m_2nd_system_indent;
		double m_global_spacement_fac;
		gint m_last_cursor_x, m_last_cursor_y;
		NedChordOrRest *m_last_obj;
		NedSystem *m_last_y_adjustment;
		int m_record_midi_numerator, m_record_midi_denominator;
		int m_record_midi_pgm, m_record_midi_tempo_inverse, m_record_midi_metro_volume;
		bool m_record_midi_triplet_recognition, m_record_midi_f_piano, m_record_midi_dnt_split;
		int m_record_midi_keysig;
#ifdef YELP_PATH
		char *m_docu;
#endif
		int m_file_version;
#ifdef WITH_BANNER
		NedPangoCairoText *m_banner1;
		NedPangoCairoText *m_banner2;
		NedPangoCairoText *m_banner3;
		NedPangoCairoText *m_banner4;
		NedPangoCairoText *m_banner5;
#endif

	friend class NedAppendNewPageCommand;
	friend class NedInsertNewPageCommand;
	friend class NedChangeMeasureTypeCommand;
	friend class NedChangeMeasureTimeSignatureCommand;
	friend class NedShiftStaffCommand;
	friend class NedMoveSpecMeasureCommand;
	friend class NedRemoveLastPageCommand;
	friend class NedRemovePageCommand;
	friend class NedSystem;
	friend class NedChangeTimeSigCommand;
	friend class NedMidiExport;
	friend class NedMusicXMLImport;
	friend class NedChangeUpbeadCommand;
	friend class NedInstrumentTable;
	friend class NedIdiotsEditor;
	friend class NedChangeSpacementCommand;
	friend class NedDeleteStaffCommand;
};

#endif /* MAINWINDOW_H */
