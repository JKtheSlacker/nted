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

#ifndef IDIOTSEDITOR_H

#define IDIOTSEDITOR_H
#include "config.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-features.h>
#include "resource.h"
#include "paths.h"

class NedIdiotsEditor;
class NedIdCommandHistory;

class pitch_info {
	public:
		int m_pitch, m_line;
		bool m_tiebackward;
		unsigned int m_state;
};

class NedIdNote {
	public:
		NedIdNote(int f_pitch, int volume, unsigned long long sta, unsigned long long sto) :
			m_midi_start(sta), m_midi_stop(sto), m_max_stop(0), 
			m_ori_start(sta), m_ori_stop(sto),
			m_tri_start_numerator(0), m_chordnotes(NULL),
				m_pitch(f_pitch), m_valid(true), m_is_triplet_note(false), fix_start(false), fix_end(false), m_volume(volume), m_line(3 /* dummy */), m_tiedbackward(false) {
					m_tripletnotes[0] = m_tripletnotes[1] = m_tripletnotes[2] = NULL;
				}
			
		NedIdNote(int f_pitch, int volume, unsigned long long sta) :
			m_midi_start(sta), m_midi_stop(sta), m_max_stop(0),
			m_ori_start(sta), m_tri_start_numerator(0), m_chordnotes(NULL),
				m_pitch(f_pitch), m_valid(false), m_is_triplet_note(false), fix_start(false), fix_end(false), m_volume(volume), m_line(3 /* dummy */), m_tiedbackward(false) {
					m_tripletnotes[0] = m_tripletnotes[1] = m_tripletnotes[2] = NULL;
			}

		NedIdNote *cloneWithDifferentTime(unsigned long long sta, unsigned long long sto);
		bool isInRect(double x, double y, double zoom_value, double xleft, double ytop);
		void determineLines(int clef, int octave_shift, int key, char *offs_array);

		~NedIdNote();

		void addPitch(int pitch, bool tiebackward);

		void closeNote(unsigned long long  sto) {
			if (sto > m_midi_start) {
				m_midi_stop = sto;
				m_valid = true;
			}
		}

		static int compare_note_start(NedIdNote *n1, NedIdNote *n2) {
			if (n1->m_midi_start < n2->m_midi_start) return -1;
			if (n1->m_midi_start < n2->m_midi_start) return 0;
			return 1;
		}
		unsigned long long m_midi_start, m_midi_stop, m_max_stop;
		unsigned long long m_ori_start, m_ori_stop;
		unsigned long long m_tri_start;
		int m_tri_start_numerator;
		GList *m_chordnotes;
		NedIdNote *m_tripletnotes[3];
		int m_pitch;
		bool m_valid;
		bool  m_is_triplet_note;
		bool fix_start, fix_end;
		unsigned int m_state;
		int m_volume;
		int m_line;
		bool m_tiedbackward;
};

class NedIdSpecial {
	public:
		NedIdSpecial(int type, unsigned long long time, int measure_number) : 
			m_type(type), m_time(time), m_measure_number(measure_number) {}
		int m_type;
#define ID_SPEC_TIME_SIG_CHANGE 1
#define ID_SPEC_TEMPO_CHANGE    2
#define ID_SPEC_VOLUME_CHANGE   3
#define ID_SPEC_KEYSIG_CHANGE   4
		unsigned long long m_time;
		int m_measure_number;
		static int compare_specials(NedIdSpecial *m1, NedIdSpecial *m2) {
			if (m1->m_time == m2->m_time) return 0;
			if (m1->m_time < m2->m_time) return -1;
			return 1;
		}
};

class NedIdTimesigChange : public NedIdSpecial {
	public:
		NedIdTimesigChange(unsigned long long time, int measure_number, int numerator, int denominator) :
			NedIdSpecial(ID_SPEC_TIME_SIG_CHANGE, time, measure_number), m_numerator(numerator), m_denominator(denominator) {}
		int m_numerator, m_denominator;
};

class NedIdTempoChange : public NedIdSpecial {
	public:
		NedIdTempoChange(unsigned long long time, int measure_number, int tempo) :
			NedIdSpecial(ID_SPEC_TEMPO_CHANGE, time, measure_number), m_tempo(tempo), m_used(false) {}
		int m_tempo;
		bool m_used;
};

class NedIdVolumeChange : public NedIdSpecial {
	public:
		NedIdVolumeChange(unsigned long long time, int measure_number, int volume) :
			NedIdSpecial(ID_SPEC_VOLUME_CHANGE, time, measure_number), m_volume(volume), m_taken(false) {}
		int m_volume;
		bool m_taken;
};

class NedIdKeysigChange : public NedIdSpecial {
	public:
		NedIdKeysigChange(unsigned long long time, int measure_number, int key) :
			NedIdSpecial(ID_SPEC_KEYSIG_CHANGE, time, measure_number), m_key(key) {}
		int m_key;
};

#define MAX_INST_NAME 12
#define MAX_64TH 640

class NedIdiotsInstrument {
	public:
		NedIdiotsInstrument(NedIdiotsEditor *id_edit,  int channel, int orig_channel, int midi_nr, int volume_change_density);
		~NedIdiotsInstrument();
		GList* trySplitInstrument(bool force_piano, bool dont_split);
		void setInstNumber(int nr)  {m_inst_number = nr;}
		void draw(Display *dpy, Window win, GC xgc, GC border_gc, double zoom_value, double xleft, double ytop, double visible_start_pos, double visible_end_pos);
		void draw_specials(cairo_t *cr, double zoom_value, double xleft, double ytop, double visible_start_pos, double visible_end_pos);
		void closeNote(int pitch, unsigned long long end_time, int midi_nr);
		int specCount();
		void addNote(int pitch, int volume, unsigned long long  start, int midi_nr);
		void addNote(NedIdNote *note);
		void addKeySig(NedIdKeysigChange *kc);
		void handle_button_press (GdkEventButton *event, double xleft, double ytop, double visible_start_pos, double visible_end_pos);
		void enlargeNotesWithUnusualLength(NedIdiotsEditor *id);
		void recognizeTriplets(NedIdiotsEditor *id);
		void enlargeNotesToAvoidSmallRests(NedIdiotsEditor *id);
		void generateVolumeSigns(int volume_change_density);
		int getMidiNr() {return m_midi_nr;}
		void setMidiNr(int midi_nr) {m_midi_nr = midi_nr;}
		void setInstrumentName(char *name);
		void snapStartOfNotes(NedIdiotsEditor *id);
		void computeVolume();
		void recomputeNotes(double zoom_value, double xleft, double ytop);
		int getChannel() {return m_channel;}
		int getClef() {return m_clef;}
		int getOctaveShift() {return m_octave_shift;}
		int getVolume() {return m_volume;}
		int getOrigChannel() {return m_orig_channel;}
		void setOrigChannel(int chan) {m_orig_channel = chan;}
		void setChannel(int chan) {m_channel = chan;}
		void determineLines(NedIdiotsEditor *id);
		void findConflictingNotes();
		void findChords(bool all_voices);
		void addSpecial(NedIdSpecial *spec);
		GList *clone_specials();
		GList *convertToNtEdNotes(NedIdiotsEditor *id, bool place_tempo_sigs, bool place_volume_or_key_signs,
				int voice_nr, unsigned long long last_time);
		char *getInstrumentName();
	private:
		void splitNotesAtMeasureStart(NedIdiotsEditor *id);
		int compute_weight(unsigned long long sta, unsigned long long sto, bool is_rest);
		unsigned long long findEndTime(unsigned long long sta, unsigned long long sto, unsigned long long max_stop, unsigned long long rest_end, unsigned long long measure_start);
		bool newVolumeNeeded(unsigned long long time, int *new_volume);
		NedIdNote *findNote(GList *start, unsigned long long start_time, unsigned long long end_time, NedIdNote *not_note1, NedIdNote *not_note2);
                void notesAus(char *s, int i);
		NedIdiotsEditor *m_id_edit;
		int m_clef, m_octave_shift;
		GList *m_notes[VOICE_COUNT];
		GList *m_specials;
		NedIdNote *m_selected_note;
		int m_inst_number;
		int m_volume_change_density;
		int m_midi_nr;
		int m_channel;
		int m_orig_channel;
		int m_volume;
		char m_inst_name[MAX_INST_NAME];
		char m_dist_array[4][MAX_64TH];
		static int m_mid_lines[4];
};
			


class NedIdiotsEditor {
	public:
		NedIdiotsEditor(int tempo_change_density, int volume_change_density, bool force_piano, bool sort_instruments, bool dont_split, bool X11Version = false);
		~NedIdiotsEditor();
		void setLastTime(unsigned long long last_time);
		void closeNote(int pitch, unsigned long long end_time, int midi_nr, int chan);
		void addNote(int pitch, int volume, unsigned long long  start, int midi_nr, int chan);
		void addTimeSigChange(unsigned long long time, int numerator, int denominator);
		void addTempoChange(unsigned long long time, int tempo);
		void addKeySig(unsigned long long time, int measure_number, int key);
		void setInstrumentName(char *name);
		void computeTimeSigMeasureNumbers(NedMainWindow *main_window);
		int getVolumeChangeDensity() {return m_volume_change_density;}
		void computeTempo();
		bool newTempoNeeded(unsigned long long current_time, int *newtempo, int current_tempo, unsigned long long  *last_tempo_test);
		bool newVolumeNeeded(int *current_volume, int volume); 
		bool newKeyNeeded(unsigned long long current_time, int *newkey, unsigned long long  *last_key_test);
		void printMeasures();
		void getMeasureNumber(unsigned long long start_time, int *num, int *denom);
		void computeVolumes();
		void changeChannelAndPgm(int staff_nr, int chan, int pgm);
		unsigned long long getMeasureStart(unsigned long long sample_time, bool endtime);
		unsigned long long getFirstSplitTime(unsigned long long start_time, unsigned long long end_time);
		char* getInstrumentName(int staff_nr);
		int getNumerator();
		int getDenominator();
		int getPartCount();
		int determine_key(unsigned long long time, int channel);
		void trySplitInstruments();
		void snapStartOfNotes();
		void recognizeTriplets();
		GList *convertToNtEdNotes(int staff_nr, int voice_nr);
		GList *colone_specials();
		int getClef(int staff_nr);
		int getOctaveShift(int staff_nr);
		int getMidiPgm(int staff_nr);
		int getChannel(int staff_nr);
		int getVolume(int staff_nr);
		int getKeySig(int staff_nr);
		GtkWidget *getDrawingArea() {return m_drawing_area;}
		GdkCursor *getPointCursor() {return m_pointer;}
		GdkCursor *getFleurCursor() {return m_fleur;}
		GdkCursor *getDoubldArrowCursor() {return m_double_arrow;}
		NedIdCommandHistory *getCommandHistory() {return m_command_history;}
		double getOffX() {return m_off_x;}
		double getOffY() {return m_off_y;}
		double getZoomValue() {return m_zoom_value;}
		unsigned int getMinDenom() {return m_min_denom;}
		
		void repaint();
		double m_tempo_inverse;
		int motion_mode;
	private:
		void correct_times_of_specials();
		bool delete_double_specials();
		void initX11();
		GdkCursor *m_double_arrow, *m_pointer, *m_fleur;
		static const char *guiDescription;
		static const GtkActionEntry file_entries[];
		static gboolean handle_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		void  draw_piano(cairo_t *cr, double ytop, int height);
		static void select_instrument(GtkButton *button, gpointer data);
		static void zoom_in(GtkWidget  *widget, void *data);
		static void zoom_out(GtkWidget  *widget, void *data);
		static void size_change_handler(GtkWidget *widget, GtkRequisition *requisition, gpointer data);
		static void handle_scroll(GtkAdjustment *adjustment, gpointer data);
		static gboolean handle_button_press (GtkWidget *widget,
			 GdkEventButton *event, gpointer data);
		static gboolean handle_motion (GtkWidget *widget, GdkEventMotion *event, gpointer data);
		static gboolean handle_button_release (GtkWidget *widget,
			 GdkEventButton *event, gpointer data);
		static void do_undo(GtkWidget  *widget, void *data);
		static void do_redo(GtkWidget  *widget, void *data);
		void getNumDenom(unsigned long long time, unsigned int *num, unsigned int *denom);
		static const char *pitch_tab[];
#ifdef ORIORI
		void draw();
#endif
		void draw_new();
		void draw_specials (cairo_t *cr, double zoom_value, double xleft, double ytop, int width, double visible_start_pos, double visible_end_pos);
		void thin_out_temposigs();
		void setHadjust();
		void setVadjust();
		double m_zoom_value;
		GtkUIManager *m_ui_manager;
		GtkActionGroup *m_menu_action_group;
		GtkWidget *m_main_window;
		GtkWidget *m_drawing_area;
		GtkWidget *m_instrument_toolbar;
		GtkAdjustment *m_hadjust;
		GtkAdjustment *m_vadjust;
		GtkWidget *m_instr_bu;
		double m_paper_len;
		double m_paper_height;
		bool m_force_piano, m_force_one_staff;
		int m_tempo_change_density, m_volume_change_density;
		int m_current_instrument_number;
		unsigned long long m_last_time;
		char m_pending_inst_name[MAX_INST_NAME];
		int m_channel_nr;
		NedIdiotsInstrument *m_last_inserted;
		bool m_sort_according_instrument;
		bool m_dont_split;
		bool m_X11version;
		unsigned int m_min_denom;
		NedIdCommandHistory *m_command_history;

		GList *m_instruments;
		GList *m_specials;

		double m_mouse_x, m_mouse_y;
		double m_off_x, m_off_y;

		Display *m_dpy;
		Window m_win;
		GC m_xgc, m_x_border_gc;
		Pixmap m_back_image;
		int m_screen;

		static void close_i_edit(GtkWidget  *widget, void *data);
};

#endif /* IDIOTSEDITOR_H */
