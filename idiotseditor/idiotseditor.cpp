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

#include <gdk/gdkx.h>
#include "idiotseditor.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "temposign.h"
#include "volumesign.h"
#include "idcommandhistory.h"
#include "changeidnotecommand.h"
#include "idcommandlist.h"
#include "note.h"
#include <locale.h>
#include <cairo.h>
#include "localization.h"
#include <dynarray.h>

#define STEP_DIVISION 10.0
#define ID_ZOOM_STEP (8.0 / 3.0)
#define ID_MIN_ZOOM 0.25
#define ID_MAX_ZOOM 8.0
#define ID_DEFAULT_ZOOM 1.0
#define PIANO_PART_WIDTH 50
#define TUNE_NAME_X_START 10

#define TIME_SIG_TRACK_HEIGHT 8.0
#define TEMPO_TRACK_HEIGHT 8.0
#define SIGN_TRACK_HEIGHT 8.0
#define VOLUME_TRACK_HEIGHT 8.0
#define SPEC_TRACKS_HEIGHT (TIME_SIG_TRACK_HEIGHT + TEMPO_TRACK_HEIGHT + SIGN_TRACK_HEIGHT + VOLUME_TRACK_HEIGHT)
#define NOTE_BORDER_LINE_THICK 4.0
#define NOTE_BORDER_LINE_THICK_2 (NOTE_BORDER_LINE_THICK / 2.0)
#define SEP_LINE_THICK 2.0
#define BAR_LINE_THICK 3.0
#define NOTE_LINE_THICK 0.8
#define SEP_LINE_DIST  20.0
#define NOTE_SEP_THICK 4
#define NOTE_SEP_THICK_2 (NOTE_SEP_THICK / 2)
#define SEP_LINE_BORDER (0.2 * SEP_LINE_DIST)
#define ID_HEIGHT (120 - 4 + 1) * SEP_LINE_DIST
#define START_HEIGHT 800
#define START_WIDTH 1000

#define MIN_VERTICAL_LINES_DIST 12.0

#define TREBLE_CLEF_UPPER_PITCH 88
#define BASS_CLEF_UPPER_PITCH 59
#define BASS_CLEF_LOWER_PITCH 33
#define PART_RELEVANT 10 /* % */

#define ALLOW1 4
#define ALLOW2 2 
#define ALLOW3 4

#define DEFAULT_FREE_X_DIST -0.3
#define DEFAULT_FREE_Y_DIST -1.3
#define VOLUME_SIGN_Y_DIST 1.3

#define MOTION_MODE_NONE  0
#define MOTION_MODE_MOVE  1
#define MOTION_NODE_LEFT  2
#define MOTION_NODE_RIGHT 3

#define CHOOSE_HEAD(line) ((m_channel == 9) ? (((line + OFFSET_TO_BE_USED_DURING_MIDI_IMPORT) % COUNT_OF_DRUMS_TO_CHOOSE_DURING_MIDI_IMPORT) + 1) : NORMAL_NOTE ) 

#define X_POS(p) (PIANO_PART_WIDTH + (p) / FACTOR * zoom_value - xleft)
#define PITCH_TO_YPOS(pi) ((116 - ((pi) - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT)
#define POS_TO_PITCH(y) (((116 + 4) * SEP_LINE_DIST - (y) - SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT) / SEP_LINE_DIST + 0.5)

#define WITH_SHOWING_ID_EDITOR

const char *NedIdiotsEditor::guiDescription =
"<ui>"
"  <menubar name='IdMainMenu'>"
"    <menu action='IdFileMenu'>"
"      <menuitem action='IdNew'/>"
"      <menuitem action='IdOpen'/>"
"      <separator/>\n"
"      <menuitem action='IdExit'/>"
"    </menu>"
"    <menu action='IdEditMenu'>"
"      <menuitem action='Idundo'/>"
"      <menuitem action='Idredo'/>"
"    </menu>"
"    <menu action='IdViewMenu'>"
"      <menuitem action='IdzoomIn'/>"
"      <menuitem action='IdzoomOut'/>"
"    </menu>"
"  </menubar>"
"  <toolbar name='IdToolBar'>\n"
"    <toolitem action='IdzoomIn'/>\n"
"    <toolitem action='IdzoomOut'/>\n"
"    <toolitem action='Idundo'/>\n"
"    <toolitem action='Idredo'/>\n"
"  </toolbar>\n"
"  <accelerator name='<control>plus' action='IdzoomIn'/>\n"
"  <accelerator name='<control>minus' action='IdzoomOut'/>\n"
"  <accelerator name='<control>z' action='Idundo'/>\n"
"  <accelerator name='<control>y' action='Idredo'/>\n"
"</ui>";


const GtkActionEntry NedIdiotsEditor::file_entries[] = {
  { "IdFileMenu", NULL, _("_File")},
  { "IdViewMenu", NULL, _("_View")},
  { "IdEditMenu", NULL, _("_Edit") },
  { "IdNew", GTK_STOCK_NEW, _("New"), NULL, _("New File"), NULL /* G_CALLBACK(new_file) */ },
  { "IdOpen", GTK_STOCK_OPEN, _("Open  ..."), "<control>O", _("Open a file"), NULL /* G_CALLBACK(restore_score) */ },
  { "IdExit", GTK_STOCK_QUIT, _("E_xit"), "<control>Q", _("Exit the program"), G_CALLBACK(close_i_edit)},
  {"IdzoomIn", GTK_STOCK_ZOOM_IN, "in", "<control>plus", _("Zoom in Ctrl + +"),  G_CALLBACK(zoom_in)},
  {"IdzoomOut", GTK_STOCK_ZOOM_OUT, "out",  "<control>minus", _("Zoom out Ctrl + -"), G_CALLBACK(zoom_out)},
  {"Idundo", GTK_STOCK_UNDO, _("undo"), "<control>z", _("Undo Ctrl + z"), G_CALLBACK(do_undo)},
  {"Idredo", GTK_STOCK_REDO, _("redo"), "<control>y", _("Redo Ctrl + y"), G_CALLBACK(do_redo)}
};

const char *NedIdiotsEditor::pitch_tab[] = { "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C",
       "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"};

NedIdNote::~NedIdNote() {
	GList *lptr;

	if (m_chordnotes != NULL) {
		for (lptr = g_list_first(m_chordnotes); lptr; lptr = g_list_next(lptr)) {
			delete (pitch_info *) lptr->data;
		}
		g_list_free(m_chordnotes);
		m_chordnotes = NULL;
	}
	for (int i = 0; i < 3; i++) {
		if (m_tripletnotes[i] != NULL) {
			delete m_tripletnotes[i];
			m_tripletnotes[i] = NULL;
		}
	}
}

void NedIdNote::addPitch(int pitch, bool tiebackward) {
	GList *lptr;
	pitch_info *pi;
	int p;
	if (pitch == m_pitch) return;

	if (pitch < m_pitch) {
		p = pitch;
		pitch = m_pitch;
		m_pitch = p;
	}

	for (lptr = g_list_first(m_chordnotes); lptr; lptr = g_list_next(lptr)) {
		if (pitch == ((pitch_info* ) lptr->data)->m_pitch) return;
		if (pitch < ((pitch_info *) lptr->data)->m_pitch) {
			pi = new pitch_info();
			pi->m_pitch = pitch;
			pi->m_tiebackward = tiebackward;
			m_chordnotes =  g_list_insert_before(m_chordnotes, lptr, pi);
			return;
		}
	}
	pi = new pitch_info();
	pi->m_pitch = pitch;
	pi->m_tiebackward = tiebackward;
	m_chordnotes = g_list_append(m_chordnotes, pi);
}

void NedIdNote::determineLines(int clef, int octave_shift, int key, char *offs_array) {
	pitch_info *pi;
	GList *lptr;
	int offs;

	m_line = NedNote::pitchToLine(m_pitch, clef, key, octave_shift, &offs);
	m_state = NedNote::determineState(m_pitch, m_line, offs, offs_array, key);
	for (lptr = g_list_first(m_chordnotes); lptr; lptr = g_list_next(lptr)) {
		pi = (pitch_info *) lptr->data;
		pi->m_line = NedNote::pitchToLine(pi->m_pitch, clef, key, octave_shift, &offs);
		pi->m_state = NedNote::determineState(pi->m_pitch, pi->m_line, offs, offs_array, key);
	}
}

NedIdNote *NedIdNote::cloneWithDifferentTime(unsigned long long sta, unsigned long long sto) {
	pitch_info *pi;
	GList *lptr;
	NedIdNote *cl = new NedIdNote(m_pitch, m_volume, sta, sto);
	cl->m_line = m_line;
	cl->m_state = m_state;
	cl->m_tiedbackward = true;

	for (lptr = g_list_first(m_chordnotes); lptr; lptr = g_list_next(lptr)) {
		pi = new pitch_info();
		pi->m_pitch = ((pitch_info *) lptr->data)->m_pitch;
		pi->m_line = ((pitch_info *) lptr->data)->m_line;
		pi->m_state = ((pitch_info *) lptr->data)->m_state;
		cl->m_chordnotes = g_list_append(cl->m_chordnotes, pi);
	}
	return cl;
}

bool NedIdNote::isInRect(double x, double y, double zoom_value, double xleft, double ytop) {
	double startpos = X_POS(m_midi_start);
	double endpos = X_POS(m_midi_stop);
	double ypos = PITCH_TO_YPOS(m_pitch);

	if (x < startpos) return false;
	if (x > endpos) return false;
	if (y < ypos) return false;
	if (y > ypos + SEP_LINE_DIST - 2 * SEP_LINE_BORDER) return false;
	return true;
}

int NedIdiotsInstrument::m_mid_lines[4] = { 38, 50, 67, 79 };


NedIdiotsInstrument::NedIdiotsInstrument(NedIdiotsEditor *id_edit, int channel, int orig_channel, int midi_nr, int volume_change_density) :
	m_id_edit(id_edit), m_specials(NULL), m_selected_note(NULL), m_volume_change_density(volume_change_density),
	m_midi_nr(midi_nr), m_channel(channel), m_orig_channel(orig_channel), m_volume(64) {m_inst_name[0] = '\0'; for (int i = 0; i < VOICE_COUNT; m_notes[i++] = NULL);}


NedIdiotsInstrument::~NedIdiotsInstrument() {
	GList *lptr;
	int voi;

	for (voi = 0; voi < VOICE_COUNT; voi++) {
		if (m_notes[voi] != NULL) {
			for (lptr = m_notes[voi]; lptr; lptr = g_list_next(lptr)) {
				delete (NedIdNote *) lptr->data;
			}
			g_list_free(m_notes[voi]);
			m_notes[voi] = NULL;
		}
	}

	if (m_specials != NULL) {
		for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
			switch (((NedIdSpecial *) lptr->data)->m_type) {
				case ID_SPEC_VOLUME_CHANGE: delete (NedIdVolumeChange *) lptr->data;
						    	break;
			}
		}
		g_list_free(m_specials);
		m_specials = NULL;
	}
}


void NedIdiotsInstrument::draw(Display *dpy, Drawable d, GC xgc, GC x_border_gc, double zoom_value, double xleft, double ytop, double visible_start_pos, double visible_end_pos) {
	GList *lptr;
	NedIdNote *note;
	int voice;

	XSetLineAttributes(dpy, x_border_gc, NOTE_BORDER_LINE_THICK, LineSolid, CapRound, JoinMiter);
	XSetForeground(dpy, x_border_gc, 0x000000);

	//for (voice = 0; voice < VOICE_COUNT; voice++) {
		for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (!note->m_valid) continue;
			if (note->m_midi_start / FACTOR * zoom_value > visible_end_pos) continue;
			if (note->m_midi_stop / FACTOR * zoom_value < visible_start_pos) continue;
			if (note == m_selected_note && m_id_edit->motion_mode != MOTION_MODE_NONE) {
				XSetForeground(dpy, xgc, 0x00ff00);
				switch(m_id_edit->motion_mode) {
					case MOTION_MODE_MOVE:
					XFillRectangle(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX(),
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT + m_id_edit->getOffY(),
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value, SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					XDrawRectangle(dpy, d, x_border_gc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft +
					 	NOTE_BORDER_LINE_THICK_2 + m_id_edit->getOffX(),
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT + m_id_edit->getOffY(),
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value - NOTE_BORDER_LINE_THICK_2,
				 		SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					break;
					case MOTION_NODE_LEFT:
					XFillRectangle(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX(),
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value - m_id_edit->getOffX(), SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					XDrawRectangle(dpy, d, x_border_gc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft +
					 	NOTE_BORDER_LINE_THICK_2 + m_id_edit->getOffX(),
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value - NOTE_BORDER_LINE_THICK_2 - m_id_edit->getOffX(),
				 		SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					break;
					case MOTION_NODE_RIGHT:
					XFillRectangle(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft,
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value + m_id_edit->getOffX(),
						 SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					XDrawRectangle(dpy, d, x_border_gc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft +
					 	NOTE_BORDER_LINE_THICK_2,
						(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
						(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value - NOTE_BORDER_LINE_THICK_2 + m_id_edit->getOffX(),
				 		SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
					break;
				}
			}
			else {
				XSetForeground(dpy, xgc, 0xff0000);
				XFillRectangle(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft,
					(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
					(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value, SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
				XDrawRectangle(dpy, d, x_border_gc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + NOTE_BORDER_LINE_THICK_2,
					(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
					(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value - NOTE_BORDER_LINE_THICK_2,
				 	SEP_LINE_DIST - 2 * SEP_LINE_BORDER);
			}
			/*
			XDrawLine(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value + NOTE_BORDER_LINE_THICK / 2.0 - xleft,
				(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
				PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value + NOTE_BORDER_LINE_THICK / 2.0 - xleft,
				(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_DIST - SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT);
			XDrawLine(dpy, d, xgc, PIANO_PART_WIDTH + note->m_midi_stop / FACTOR * zoom_value  - NOTE_BORDER_LINE_THICK / 2.0 - xleft,
				(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT,
				PIANO_PART_WIDTH + note->m_midi_stop / FACTOR * zoom_value  - NOTE_BORDER_LINE_THICK / 2.0 - xleft,
				(116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_DIST - SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT);
			*/
			/*
			cairo_rectangle (cr, note->m_midi_start / FACTOR * zoom_value, (116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER, 
				 	(note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value, SEP_LINE_DIST - 2 * SEP_LINE_BORDER - SPEC_TRACKS_HEIGHT);
				 	*/
		}
	//}
}

void NedIdiotsInstrument::draw_specials(cairo_t *cr, double zoom_value, double xleft, double ytop, double visible_start_pos, double visible_end_pos) {
	GList *lptr;
	NedIdSpecial *spec;
	char Str[128];

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		spec = (NedIdSpecial *) lptr->data;
		if (spec->m_time / FACTOR * zoom_value > visible_end_pos) break;
		if (spec->m_time / FACTOR * zoom_value < visible_start_pos) continue;
		switch (spec->m_type) {
			case ID_SPEC_KEYSIG_CHANGE: sprintf(Str, "%d", ((NedIdKeysigChange *) spec)->m_key);
				cairo_new_path(cr);
				cairo_move_to(cr, PIANO_PART_WIDTH + spec->m_time / FACTOR * zoom_value - xleft, TIME_SIG_TRACK_HEIGHT + TEMPO_TRACK_HEIGHT + SIGN_TRACK_HEIGHT);
				cairo_show_text(cr, Str);
				cairo_stroke(cr);
				break;
			case ID_SPEC_VOLUME_CHANGE: sprintf(Str, "%d", ((NedIdVolumeChange *) spec)->m_volume);
				cairo_new_path(cr);
				cairo_move_to(cr, PIANO_PART_WIDTH + spec->m_time / FACTOR * zoom_value - xleft, TIME_SIG_TRACK_HEIGHT + TEMPO_TRACK_HEIGHT + SIGN_TRACK_HEIGHT + VOLUME_TRACK_HEIGHT);
				cairo_show_text(cr, Str);
				cairo_stroke(cr);
				break;
			default: NedResource::Abort("draw_specials");
		}
	}
}


void NedIdiotsEditor::getMeasureNumber(unsigned long long start_time, int *num, int *denom) {
	GList *lptr;
	NedIdTimesigChange *found = NULL, *newfound;
	int numerator = 4, denominator = 4;
	unsigned long long duration = WHOLE_NOTE;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time > start_time) break;
		if (((NedIdSpecial *) lptr->data)->m_type == ID_SPEC_TIME_SIG_CHANGE) {
			newfound = (NedIdTimesigChange *) lptr->data;
			numerator = newfound->m_numerator;
			denominator = newfound->m_denominator;
			if (found != NULL) {
				if (((newfound->m_time - found->m_time) % duration) != 0) {
					printf("start_time = %llu(%llu), %llu(%llu) - %llu(%llu) = %llu(%llu), duration = %llu(%llu) mod = %llu(%llu)\n",
						start_time, start_time/ NOTE_8, 
						newfound->m_time, newfound->m_time / NOTE_8, found->m_time, found->m_time / NOTE_8,
						newfound->m_time - found->m_time, (newfound->m_time - found->m_time) / NOTE_8,
						duration, duration / NOTE_8, 
						(newfound->m_time - found->m_time) % duration, 
						((newfound->m_time - found->m_time) % duration) / NOTE_8); fflush(stdout);
					NedResource::Abort("NedIdiotsEditor::getMeasureNumber");
				}
			}
			found = newfound;
			duration = numerator * WHOLE_NOTE / denominator;
		}
	}
	*num = numerator;
	*denom = denominator;
}
unsigned long long NedIdiotsEditor::getMeasureStart(unsigned long long sample_time, bool endtime) {
	GList *lptr;
	double divide_d;
	int divide;
	NedIdTimesigChange *found = NULL, *newfound;
	int numerator = 4, denominator = 4;
	unsigned long long duration = WHOLE_NOTE;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (!endtime && ((NedIdSpecial *) lptr->data)->m_time > sample_time) break;
		if (endtime && ((NedIdSpecial *) lptr->data)->m_time >= sample_time) break;
		if (((NedIdSpecial *) lptr->data)->m_type == ID_SPEC_TIME_SIG_CHANGE) {
			newfound = (NedIdTimesigChange *) lptr->data;
			numerator = newfound->m_numerator;
			denominator = newfound->m_denominator;
			if (found != NULL) {
				if (((newfound->m_time - found->m_time) % duration) != 0) {
					printf("sample_time = %llu(%llu), %llu(%llu) - %llu(%llu) = %llu(%llu), duration = %llu(%llu) mod = %llu(%llu)\n",
						sample_time, sample_time/ NOTE_8, 
						newfound->m_time, newfound->m_time / NOTE_8, found->m_time, found->m_time / NOTE_8,
						newfound->m_time - found->m_time, (newfound->m_time - found->m_time) / NOTE_8,
						duration, duration / NOTE_8, 
						(newfound->m_time - found->m_time) % duration, 
						((newfound->m_time - found->m_time) % duration) / NOTE_8); fflush(stdout);
					NedResource::Abort("NedIdiotsEditor::getMeasureStart(1)");
				}
			}
			found = newfound;
			duration = numerator * WHOLE_NOTE / denominator;
		}
	}
	if (found == NULL) {
		divide_d = (double) sample_time / (double) duration;
		divide = (int) divide_d;
		return (unsigned long long) divide * (unsigned long long) duration;
	}
	if (sample_time < found->m_time) {
		NedResource::Abort("NedIdiotsEditor::getMeasureStart(2)");
	}
	divide_d = (double) (sample_time - found->m_time) / (double) duration;
	divide = (int) divide_d;
	return found->m_time + (unsigned long long) divide * duration;
}

GList* NedIdiotsInstrument::trySplitInstrument(bool force_piano, bool dont_split) {
	NedIdNote *note;
	int i, idx, dist, mindist;
	GList *lptr;
	GList *deleted_note_ptrs = NULL;
	int sum = 1;
	int max_idx, max_sum;
	int part_sums[4];
	NedIdiotsInstrument *newinstruments[4];
	GList *instruments_list = NULL;
	bool first;

	for (i = 0; i < 4; i++) {
		part_sums[i] = 0;
		newinstruments[i] = NULL;
	}

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		sum++;
		if (note->m_pitch > TREBLE_CLEF_UPPER_PITCH) part_sums[3]++;
		else if (note->m_pitch > BASS_CLEF_UPPER_PITCH) part_sums[2]++;
		else if (note->m_pitch > BASS_CLEF_LOWER_PITCH) part_sums[1]++;
		else part_sums[0]++;
	}

	if (force_piano || m_channel == 9) {
		part_sums[2] += part_sums[3];
		part_sums[3] = 0;
		part_sums[1] += part_sums[0];
		part_sums[0] = 0;
	}


	if (!dont_split) {
		first = true;
		for (i = 3; i >= 0; i--) {
			if (part_sums[i] * 100 / sum > PART_RELEVANT) {
				if (first) {
					first = false;
					newinstruments[i] = this;
				}
				else {
					newinstruments[i] = new NedIdiotsInstrument(m_id_edit, m_channel, m_orig_channel, m_midi_nr, m_volume_change_density);
					if (m_inst_name[0] != '\0') {
						strcpy(newinstruments[i]->m_inst_name, m_inst_name);
					}
					newinstruments[i]->m_volume = m_volume;
					newinstruments[i]->m_inst_number = m_inst_number;
					newinstruments[i]->m_specials = clone_specials();
				}
				switch(i) {
					case 3: newinstruments[i]->m_clef = TREBLE_CLEF;
						newinstruments[i]->m_octave_shift = 12;
						break;
					case 2: newinstruments[i]->m_clef = (m_channel == 9) ? NEUTRAL_CLEF1 : TREBLE_CLEF;
						newinstruments[i]->m_octave_shift = 0;
						break;
					case 1: newinstruments[i]->m_clef = (m_channel == 9) ? NEUTRAL_CLEF2 : BASS_CLEF;
						newinstruments[i]->m_octave_shift = 0;
						break;
					default: newinstruments[i]->m_clef = BASS_CLEF;
						newinstruments[i]->m_octave_shift = -12;
						break;
				}
			}
		}
	}
	if (newinstruments[0] == NULL && newinstruments[1] == NULL && newinstruments[2] == NULL && newinstruments[3] == NULL) {
		max_idx = 0;
		max_sum = part_sums[0];

		for (i = 1; i < 4; i++) {
			if (part_sums[i] > max_sum) {
				max_sum = part_sums[i];
				max_idx = i;
			}
		}
		switch(max_idx) {
			case 3: m_clef = TREBLE_CLEF;
				m_octave_shift = 12;
				break;
			case 2: m_clef = (m_channel == 9) ? NEUTRAL_CLEF1 : TREBLE_CLEF;
				m_octave_shift = 0;
				break;
			case 1: m_clef = (m_channel == 9) ? NEUTRAL_CLEF2 : BASS_CLEF;
				m_octave_shift = 0;
				break;
			default: m_clef = BASS_CLEF;
				m_octave_shift = -12;
				break;
		}
		return NULL;
	}


	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		mindist = 1000;
		idx = -1;
		for (i = 0; i < 4; i++) {
			if (newinstruments[i] != NULL) {
				dist = note->m_pitch - m_mid_lines[i];
				if (dist < 0) dist = -dist;
				if (dist < mindist) {
					mindist = dist;
					idx = i;
				}
			}
		}
		if (idx < 0) {
			NedResource::Abort("NedIdiotsInstrument::trySplitInstrument");
		}

		if (newinstruments[idx] != this) {
			newinstruments[idx]->addNote(note);
			deleted_note_ptrs = g_list_append(deleted_note_ptrs, lptr);
		}
	}

	for (lptr = g_list_first(deleted_note_ptrs); lptr; lptr = g_list_next(lptr)) {
		m_notes[0] = g_list_delete_link(m_notes[0], (GList *) lptr->data);
	}
	g_list_free(deleted_note_ptrs);

	for (i = 3; i >= 0; i--) {
		if (newinstruments[i] == NULL) continue;
		if (newinstruments[i] == this) continue;
		instruments_list = g_list_append(instruments_list, newinstruments[i]);
	}

	return instruments_list;
}

void NedIdiotsInstrument::determineLines(NedIdiotsEditor *id) {
	char offs_array[115];
	NedIdNote *note;
	bool first;
	int key;
	unsigned long long start_time = 0, end_time, measure_width;
	int numerator, denominator;
	id->getMeasureNumber(start_time, &numerator, &denominator);
	m_notes[0] = g_list_sort(m_notes[0], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
	GList *lptr = g_list_first(m_notes[0]);

	do {
		memset(offs_array, 0, sizeof(offs_array));
		measure_width = numerator * WHOLE_NOTE / denominator;
		end_time = start_time + measure_width;
		for (; lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (!note->m_valid) continue;
			if (note->m_midi_start < start_time) {
				NedResource::Abort("NedIdiotsInstrument::determineLines");
			}
			if (note->m_midi_start >= end_time) break;
			if (note->m_is_triplet_note) {
				first = true;
				for (int i = 0; i < 3; i++) {
					if (note->m_tripletnotes[i] != NULL) {
						key = id->determine_key(note->m_tripletnotes[i]->m_midi_start, m_channel);
						note->m_tripletnotes[i]->determineLines(m_clef, m_octave_shift, key, offs_array);
						if (first) {
							first = false;
							note->m_line = note->m_tripletnotes[i]->m_line;
							note->m_state = note->m_tripletnotes[i]->m_state;
							note->m_pitch = note->m_tripletnotes[i]->m_pitch;
						}
					}
				}
			}
			else {
				key = id->determine_key(note->m_midi_start, m_channel);
				note->determineLines(m_clef, m_octave_shift, key, offs_array);
			}
		}
		id->getMeasureNumber(end_time, &numerator, &denominator);
		start_time = end_time;
	}
	while (lptr != NULL);
}


void NedIdiotsInstrument::splitNotesAtMeasureStart(NedIdiotsEditor *id) {
	char offs_array[115];
	GList *lptr;
	NedIdNote * note;
	unsigned long long duration, start_time, end_time, split_time;
	int dist;
	int pitch;
	int pos;
	int voi;
	int key;

	for (voi = 0; voi < VOICE_COUNT; voi++) {
		for (lptr = g_list_first(m_notes[voi]); lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (!note->m_valid) continue;
			pos = g_list_position(m_notes[voi], lptr);
			duration = note->m_midi_stop - note->m_midi_start;
			if (duration > WHOLE_NOTE) duration = WHOLE_NOTE;
			start_time = note->m_midi_start;
			while ((split_time = id->getFirstSplitTime(start_time, note->m_midi_stop)) != 0) {
				if (note->m_is_triplet_note) {
					NedResource::Abort("NedIdiotsInstrument::splitNotesAtMeasureStart(1)");
				}
				dist = (int) (split_time - note->m_midi_start);
				if ((unsigned int ) dist < duration / ALLOW1) {
					note->m_midi_start = split_time;
					if (note->m_max_stop > 0 && note->m_max_stop <= note->m_midi_start) {
						note->m_max_stop = 0;
					}
					note->fix_start = true;
					memset(offs_array, 0, sizeof(offs_array));
					key = id->determine_key(note->m_midi_start, m_channel);
					note->determineLines(m_clef, m_octave_shift, key, offs_array); // because the start is now behind the new measure
					start_time = split_time;
					continue;
				}
				dist = (int) (note->m_midi_stop - start_time);
				if ((unsigned int) dist < duration / ALLOW1) {
					note->m_midi_stop = split_time;
					if (note->m_max_stop > 0 && note->m_max_stop <= note->m_midi_start) {
						NedResource::Abort("NedIdiotsInstrument::splitNotesAtMeasureStart(2)");
					}
					note->fix_end = true;
					break;
				}
				if (note->m_midi_stop <= note->m_midi_start) {
					note->m_valid = false;
					break;
				}
				dist = note->m_midi_stop - note->m_midi_start;
				if ((unsigned int) dist < duration / ALLOW1) {
					note->m_valid = false;
					break;
				}
				end_time = note->m_midi_stop;
				pitch = note->m_pitch;
				note->m_midi_stop = split_time;
				if (note->m_max_stop > 0 && note->m_max_stop <= note->m_midi_start) {
					NedResource::Abort("NedIdiotsInstrument::splitNotesAtMeasureStart(3)");
				}
				note->fix_end = true;
				if (end_time - split_time < NOTE_16) break;
				note = note->cloneWithDifferentTime(split_time, end_time);
				note->fix_start = true;
				if (note->m_max_stop > 0 && note->m_max_stop <= note->m_midi_start) {
					NedResource::Abort("NedIdiotsInstrument::splitNotesAtMeasureStart(4)");
				}
				m_notes[voi] = g_list_insert(m_notes[voi], note, ++pos);
				memset(offs_array, 0, sizeof(offs_array));
				key = id->determine_key(note->m_midi_start, m_channel);
				note->determineLines(m_clef, m_octave_shift, key, offs_array); // because the start is now behind the new measure
				start_time = split_time;
				if ((lptr = g_list_find(m_notes[voi], note)) == NULL) {
					NedResource::Abort("NedIdiotsInstrument::splitNotesAtMeasureStart(5)");
				}
			}
	
		}
		m_notes[voi] = g_list_sort(m_notes[voi], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
	}
}

void NedIdiotsInstrument::snapStartOfNotes(NedIdiotsEditor *id) {
	unsigned long long start_time, snap, divide;
	double divide_d;
	int duration;
	GList *lptr;
	NedIdNote *note;
	unsigned long long pow2dist, pow23dist;

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		start_time = id->getMeasureStart(note->m_midi_start, false);
		duration = (int) (note->m_midi_stop - note->m_midi_start);
		if (duration > NOTE_8) duration = NOTE_8;
		for (snap = NOTE_64; snap < NOTE_4 && snap < (unsigned int) duration / ALLOW2; snap <<= 1);
		divide_d = (double) (note->m_midi_start - start_time) / (double) snap;
		divide = (unsigned long long) (divide_d + 0.5);
		note->m_midi_start = start_time + divide * snap;
		if (note->m_midi_stop < note->m_midi_start) {
			note->m_valid = false;
		}
	}

	m_notes[0] = g_list_sort(m_notes[0], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		start_time = id->getMeasureStart(note->m_midi_start, false);
		pow2dist = (note->m_ori_start > note->m_midi_start) ? (note->m_ori_start - note->m_midi_start) : (note->m_midi_start - note->m_ori_start);
		snap = NOTE_4 / 3;
		divide_d = (double) (note->m_ori_start - start_time) / (double) snap;
		divide = (unsigned long long) (divide_d + 0.5);
		//if (divide % 3 != 0) continue;
		note->m_tri_start = start_time + divide * snap;
		pow23dist = (note->m_tri_start > note->m_ori_start) ? ( note->m_tri_start - note->m_ori_start ) : (note->m_ori_start - note->m_tri_start);
		if ((int) divide >= 0 && (int) divide < 40) {
			note->m_tri_start_numerator = (pow23dist < pow2dist) ?  ((int) divide) : 0;
		}
			
	}
}

void NedIdiotsInstrument::findChords(bool all_voices) {
	GList *lptr1, *lptr2;
	NedIdNote *note1, *note2;
	int dur1, dur2, dist, durmax;
	int i, max_voice = all_voices ? 4 : 1;
	for (i = 0; i < max_voice; i++) {
		for (lptr1 = g_list_first(m_notes[i]); lptr1; lptr1 = g_list_next(lptr1)) {
			note1 = (NedIdNote *) lptr1->data;
			if (!note1->m_valid) continue;
			if (note1->m_is_triplet_note) continue;
			for (lptr2 = g_list_next(lptr1); lptr2; lptr2 = g_list_next(lptr2)) {
				note2 = (NedIdNote *) lptr2->data;
				if (!note2->m_valid) continue;
				if (note2->m_is_triplet_note) continue;
				if (note2->m_midi_start >= note1->m_midi_stop) break;
				if (note1->m_midi_start != note2->m_midi_start) continue;
				dur1 = (int) (note1->m_midi_stop - note1->m_midi_start);
				dur2 = (int) (note2->m_midi_stop - note2->m_midi_start);
				durmax = (dur1 > dur2) ? dur1 : dur2;
				dist = (dur1 > dur2) ? (dur1 - dur2) : (dur2 - dur1);
				if (dist < durmax / 2) {
			    		note1->addPitch(note2->m_pitch, note2->m_tiedbackward);
			    		note2->m_valid = false;
				}
			}
		}
	}
}

bool NedIdiotsInstrument::newVolumeNeeded(unsigned long long time, int *new_volume) {
	GList *lptr;

	for (lptr = g_list_last(m_specials); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_VOLUME_CHANGE) continue;
		if (((NedIdSpecial *) lptr->data)->m_time > time) continue;
		if (((NedIdVolumeChange *) lptr->data)->m_taken) return false;
		*new_volume = ((NedIdVolumeChange *) lptr->data)->m_volume;
		((NedIdVolumeChange *) lptr->data)->m_taken = true;
		return true;
	}

	return false;
}


#define WEIGHT_PAUSE 4
#define WEIGHT_DOTTED_PAUSE 6
#define WEIGHT_DOTTED_NOTE 2
#define WEIGHT_NOTE 1

int NedIdiotsInstrument::compute_weight(unsigned long long sta, unsigned long long sto, bool is_rest) {
	int ndotcount = 0, rdotcount = 0;
	int ncount = 0, rcount = 0;
	int dotcount;
	unsigned int len2;
	if (sto < sta) {
		NedResource::Abort("NedIdiotsInstrument::compute_weight");
	}
	NedResource::setLengthField((int) sto - sta);

	len2 = NedResource::getPartLength(&dotcount);
	while(len2) {
		if (is_rest) {
			if (dotcount != 0) {
				rdotcount++;
			}
			else {
				rcount++;
			}
		}
		else {
			if (dotcount != 0) {
				ndotcount++;
			}
			else {
				ncount++;
			}
		}
		len2 = NedResource::getPartLength(&dotcount);
	}

	return rcount * WEIGHT_PAUSE + ncount * WEIGHT_NOTE + rdotcount * WEIGHT_DOTTED_PAUSE + ndotcount * WEIGHT_DOTTED_NOTE;
}

unsigned long long NedIdiotsInstrument::findEndTime(unsigned long long sta, unsigned long long sto, unsigned long long max_stop, unsigned long long rest_end, unsigned long long measure_start) {
	unsigned long long duration, snap, i, minsnap = 0, mini = 0;
	int weight, minweight = 10000;
	if (rest_end < max_stop) {
		NedResource::Abort("NedIdiotsInstrument::findEndTime");
	}

	duration = sto - sta;
	sto = sta + (duration >> 1);

	for (snap = NOTE_2; snap >= NOTE_64; snap >>= 1) {
		i = 0;
		while (true) {
			if (measure_start + i * snap <= sto) {i++; continue;}
			if (measure_start + i * snap > max_stop) break;
			weight = compute_weight(sta, measure_start + i * snap, false);
			weight += compute_weight(measure_start + i * snap, rest_end, true);
			if (weight < minweight) {
				minweight = weight;
				mini = i;
				minsnap = snap;
			}
			i++;
		}
	}
	if (minsnap != 0) {
		return measure_start + mini * minsnap;
	}
	return sto;
}

void NedIdiotsInstrument::enlargeNotesWithUnusualLength(NedIdiotsEditor *id) {
	NedIdNote *note, *note2;
	unsigned long long note_duration, new_end_time, max_end_time, start_time, end_time, measure_width;
	/* unsigned long long snap, minsnap, maxsnap, dist, mindist; */
	int numerator, denominator;
	GList *lptr, *lptr2;

	m_notes[0] = g_list_sort(m_notes[0], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
	start_time = 0;
	id->getMeasureNumber(start_time, &numerator, &denominator);
	lptr = g_list_first(m_notes[0]);

	do {
		measure_width = numerator * WHOLE_NOTE / denominator;
		end_time = start_time + measure_width;
		for (; lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (note->m_is_triplet_note) continue;
			if (!note->m_valid) continue;
			if (note->m_midi_start >= end_time) break;
			for (lptr2 = g_list_next(lptr); lptr2; lptr2 = g_list_next(lptr2)) {
				note2 = (NedIdNote *) lptr2->data;
				if (note2->m_midi_start == note->m_midi_start) continue;
				if (note2->m_valid) break;
			}
			if (lptr2 == NULL) {
				new_end_time = max_end_time = end_time;
				if (new_end_time <= note->m_midi_start) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesWithUnusualLength(1)");
				}
			}
			else {
				if (note2->m_midi_start > end_time) {
					new_end_time = max_end_time = end_time;
					if (new_end_time <= note->m_midi_start) {
						NedResource::Abort("NedIdiotsInstrument::enlargeNotesWithUnusualLength(2)");
					}
				}
				else {
					new_end_time = max_end_time = note2->m_midi_start;
					note_duration = note->m_midi_stop - note->m_midi_start;
					if (max_end_time - note->m_midi_start < 3 * note_duration / 5) {
						new_end_time = max_end_time = end_time;
					}
					if (new_end_time <= note->m_midi_start) {
						NedResource::Abort("NedIdiotsInstrument::enlargeNotesWithUnusualLength(3)");
					}
				}
			}
			note_duration = note->m_midi_stop - note->m_midi_start;
			if (note_duration < NOTE_8) {
				if (new_end_time - note->m_midi_start > (note_duration << 2)) {
					new_end_time = note->m_midi_start + (note_duration << 2);
				}
			}
			else {
				if (new_end_time - note->m_midi_start > (note_duration  + (note_duration >> 1))) {
					new_end_time = note->m_midi_start + (note_duration + (note_duration >> 1));
				}
			}
			note->m_max_stop = new_end_time;

			if (note->m_max_stop > 0 && note->m_max_stop <= note->m_midi_start) {
				//NedResource::Abort("NedIdiotsInstrument::enlargeNotesWithUnusualLength(4)");
				note->m_valid = false;
				continue;
			}

			note->m_midi_stop = findEndTime(note->m_midi_start, note->m_midi_stop, new_end_time, max_end_time, start_time);

			/*
			note_duration = new_end_time - note->m_midi_start;
			mindist = WHOLE_NOTE;
			minsnap = 0;
			for (snap = WHOLE_NOTE; snap >= NOTE_64; snap >>= 1) {
				dist = (note_duration > snap ) ? (note_duration - snap) : (snap - note_duration);
				if (dist < mindist && note->m_midi_start + snap <= new_end_time) {
					mindist = dist;
					minsnap = snap;
				}
			}
			for (snap = NOTE_2 + NOTE_4; snap > NOTE_64; snap >>= 1) {
				dist = (note_duration > snap ) ? (note_duration - snap) : (snap - note_duration);
					if (dist < mindist && note->m_midi_start + snap <= new_end_time) {
					mindist = dist;
					minsnap = snap;
				}
			}
			if (minsnap > 0) {
				note->m_midi_stop = note->m_midi_start + minsnap;
			}
			*/
		}
		id->getMeasureNumber(end_time, &numerator, &denominator);
		start_time = end_time;
	}
	while (lptr != NULL);
}

void NedIdiotsInstrument::notesAus(char *s, int i) {
	GList *lptr;
	NedIdNote *note;
	printf("%s, voi %d:\n", s, i);
	for (lptr = g_list_first(m_notes[i]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		printf("\tnote(0x%p): %llu(%llu, %llu) - %llu(%llu,%llu), m_pitch = %d, m_line = %d, m_is_triplet_note = %d, m_valid = %d\n",
			note, LAUS(note->m_midi_start), LAUS(note->m_midi_stop),
			note->m_pitch, note->m_line, note->m_is_triplet_note, note->m_valid);
			if (note->m_is_triplet_note) {
				for (int i = 0; i <3; i++) {		
					printf("\t\tnote(0x%p): %llu(%llu, %llu) - %llu(%llu,%llu), m_pitch = %d, m_line = %d, m_triplet_type = %d, m_valid = %d\n",
						note->m_tripletnotes[i], LAUS(note->m_tripletnotes[i]->m_midi_start), LAUS(note->m_tripletnotes[i]->m_midi_stop),
						note->m_tripletnotes[i]->m_pitch, note->m_tripletnotes[i]->m_line, note->m_tripletnotes[i]->m_is_triplet_note, note->m_tripletnotes[i]->m_valid);
				}
			}
	}
	fflush(stdout);
}

NedIdNote *NedIdiotsInstrument::findNote(GList *start, unsigned long long start_time, unsigned long long end_time, NedIdNote *not_note1, NedIdNote *not_note2)  {
	NedIdNote *note;
	unsigned long long diff, dur;

	for (; start; start = g_list_next(start)) {
		note = (NedIdNote *) start->data;
		if (!note->m_valid) continue;
		if (note == not_note1) continue;
		if (note == not_note2) continue;
		if (note->m_ori_start > end_time) return NULL;
		diff = (start_time > note->m_ori_start) ? (start_time - note->m_ori_start) : ( note->m_ori_start - start_time);
		if (diff > NOTE_8) continue;
		dur = note->m_ori_stop - note->m_ori_start;
		diff = (dur > NOTE_4 / 3) ? (dur - NOTE_4 / 3) : (NOTE_4 / 3 - dur);
		if (diff > NOTE_8) continue;
		if (note->m_ori_stop - note->m_ori_start <= NOTE_32) continue;
		return note;
	}
	return NULL;
}

void NedIdiotsInstrument::recognizeTriplets(NedIdiotsEditor *id) {
	NedIdNote *note = NULL, *tri_note, *tri_array[40][3]; // worst case: 20/2 that is 40 quarter triplets
	unsigned long long start_time, end_time, measure_width;
	int numerator, denominator, i, j;
	GList *lptr_start, *lptr, *lptr2;
	GList *tri_notes = NULL;


	m_notes[0] = g_list_sort(m_notes[0], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
	start_time = 0;
	id->getMeasureNumber(start_time, &numerator, &denominator);
	lptr = g_list_first(m_notes[0]);

	do {
		for (i = 0; i < 40; i++)  {
			for (j = 0; j < 3; tri_array[i][j++] = NULL);
		}
		
		measure_width = numerator * WHOLE_NOTE / denominator;
		end_time = start_time + measure_width;
		lptr_start = lptr;
		for (; lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (!note->m_valid) continue;
			if (note->m_midi_start >= end_time) break;
			if (note->m_tri_start_numerator) {
				tri_array[note->m_tri_start_numerator / 3][note->m_tri_start_numerator % 3] = note;
			}
		}
		if (measure_width >= NOTE_4) {
			for (i = 0; i < 40; i++) {
				if (tri_array[i][1] && tri_array[i][1]->m_valid && start_time + (i + 1) * NOTE_4 <= end_time) {
					if ((tri_array[i][0] = findNote(lptr_start, start_time + i * NOTE_4, start_time + i * NOTE_4 + NOTE_8, tri_array[i][1], tri_array[i][2]))) {
						tri_note = new NedIdNote(tri_array[i][0]->m_pitch, tri_array[i][0]->m_volume, start_time + i * NOTE_4, start_time + (i + 1) *  NOTE_4);
						tri_note->m_line = tri_array[i][0]->m_line;
						tri_note->m_state = tri_array[i][0]->m_state;
						tri_note->m_is_triplet_note = true;
						tri_notes = g_list_append(tri_notes, tri_note);
						for (j = 0; j < 2; j++) {
							tri_note->m_tripletnotes[j] = tri_array[i][j];
							tri_array[i][j]->m_valid = false; // avoid normal treatment of these notes
						}
						if (tri_array[i][2] && tri_array[i][2]->m_valid) {
							tri_note->m_tripletnotes[2] = tri_array[i][2];
							tri_array[i][2]->m_valid = false; // avoid normal treatment of these notes
						}
					}
					else if (tri_array[i][2] && tri_array[i][2]->m_valid) {
						tri_note = new NedIdNote(tri_array[i][1]->m_pitch, tri_array[i][1]->m_volume, start_time + i * NOTE_4, start_time + (i + 1) *  NOTE_4);
						tri_note->m_is_triplet_note = true;
						tri_note->m_line = tri_array[i][1]->m_line;
						tri_note->m_state = tri_array[i][1]->m_state;
						tri_note->m_is_triplet_note = true;
						tri_notes = g_list_append(tri_notes, tri_note);
						for (j = 1; j < 3; j++) { // only 2 notes --> first ist a break
							tri_note->m_tripletnotes[j] = tri_array[i][j];
							tri_array[i][j]->m_valid = false; // avoid normal treatment of these notes
						}
					}
				}
			}

		}
		id->getMeasureNumber(end_time, &numerator, &denominator);
		start_time = end_time;
	}
	while (lptr != NULL);
	for (lptr = g_list_first(tri_notes); lptr; lptr = g_list_next(lptr)) {
		tri_note = (NedIdNote *) lptr->data;
		for (i = 0; i < 3; i++) {
			if (tri_note->m_tripletnotes[i] != NULL) {
				if ((lptr2 = g_list_find(m_notes[0], tri_note->m_tripletnotes[i])) == NULL) {
					NedResource::Abort("NedIdiotsInstrument::recognizeTriplets(1)");
				}
				m_notes[0] = g_list_delete_link(m_notes[0], lptr2);
				note->m_valid = true;
			}
		}
	}
	for (lptr = g_list_first(tri_notes); lptr; lptr = g_list_next(lptr)) {
		m_notes[0] = g_list_insert_sorted(m_notes[0], lptr->data, (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
	}
	g_list_free(tri_notes);
}

void NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(NedIdiotsEditor *id) {
	NedIdNote *note, *note2;
	unsigned long long note_duration, max_end_time, new_end_time, start_time, end_time, measure_width;
	/* unsigned long long snap, minsnap, maxsnap, dist, mindist; */
	int numerator, denominator;
	GList *lptr, *lptr2;
	int voi;

	for (voi = 0; voi < VOICE_COUNT; voi++) {
		m_notes[voi] = g_list_sort(m_notes[voi], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);
		start_time = 0;
		id->getMeasureNumber(start_time, &numerator, &denominator);
		lptr = g_list_first(m_notes[voi]);

		do {
			measure_width = numerator * WHOLE_NOTE / denominator;
			end_time = start_time + measure_width;
			for (; lptr; lptr = g_list_next(lptr)) {
				note = (NedIdNote *) lptr->data;
				if (!note->m_valid) continue;
				if (note->m_is_triplet_note) continue;
				if (note->fix_end) continue;
				if (note->m_midi_start >= end_time) break;
				for (lptr2 = g_list_next(lptr); lptr2; lptr2 = g_list_next(lptr2)) {
					note2 = (NedIdNote *) lptr2->data;
					if (note2->m_midi_start == note->m_midi_start) continue;
					if (note2->m_valid) break;
				}
				if (lptr2 == NULL || note2->m_midi_start > end_time) {
					new_end_time = max_end_time = end_time;
				}
				else {
					new_end_time = max_end_time = note2->m_midi_start;
				}
				if (new_end_time > end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(1)");
				}
				if (max_end_time > end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(2)");
				}
				/*
				if (note->m_max_stop > 0 && max_end_time > note->m_max_stop) {
					if (note->m_max_stop <= note->m_midi_start) {
						NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(3)");
					}
					new_end_time = max_end_time = note->m_max_stop;
				}
				*/
				if (new_end_time > end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(4)");
				}
				note_duration = note->m_midi_stop - note->m_midi_start;
				if (note_duration < NOTE_16) {
					if (new_end_time - note->m_midi_start > (note_duration << 2)) {
						new_end_time = note->m_midi_start + (note_duration << 2);
					}
				}
				else {
					if (new_end_time - note->m_midi_start > (note_duration  + (note_duration >> 1))) {
						new_end_time = note->m_midi_start + (note_duration + (note_duration >> 1));
					}
				}
				if (new_end_time > end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(5)");
				}
				if (new_end_time > max_end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(6)");
				}
				note->m_midi_stop = findEndTime(note->m_midi_start, note->m_midi_stop, new_end_time, max_end_time, start_time);
				if (note->m_midi_stop > end_time) {
					NedResource::Abort("NedIdiotsInstrument::enlargeNotesToAvoidSmallRests(7)");
				}

				/*
				note_duration = new_end_time - note->m_midi_start;
				mindist = WHOLE_NOTE;
				minsnap = 0;
				for (snap = WHOLE_NOTE; snap >= NOTE_64; snap >>= 1) {
					dist = (note_duration > snap ) ? (note_duration - snap) : (snap - note_duration);
					if (dist < mindist && note->m_midi_start + snap <= new_end_time) {
						mindist = dist;
						minsnap = snap;
					}
				}
				for (snap = NOTE_2 + NOTE_4; snap > NOTE_64; snap >>= 1) {
					dist = (note_duration > snap ) ? (note_duration - snap) : (snap - note_duration);
					if (dist < mindist && note->m_midi_start + snap <= new_end_time) {
						mindist = dist;
						minsnap = snap;
					}
				}
				if (minsnap > 0) {
					note->m_midi_stop = note->m_midi_start + minsnap;
				}
				*/
			}
			id->getMeasureNumber(end_time, &numerator, &denominator);
			start_time = end_time;
		}
		while (lptr != NULL);
	}
}



void NedIdiotsInstrument::computeVolume() {
	GList *lptr;
	NedIdNote *note;

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		m_volume = note->m_volume;
		return;
	}
}


void NedIdiotsInstrument::findConflictingNotes() {
	GList *lptr1, *lptr2;
	NedIdNote *note1, *note2;
	GList *conflict_list_ptrs = NULL;
	GList *conflict_list_nts = NULL;
	int voi;

	for (voi = 0; voi < VOICE_COUNT; voi++) {
		m_notes[voi] = g_list_sort(m_notes[voi], (gint (*)(const void*, const void*)) NedIdNote::compare_note_start);

		for (lptr1 = g_list_first(m_notes[voi]); lptr1; lptr1 = g_list_next(lptr1)) {
			note1 = (NedIdNote *) lptr1->data;
			if (!note1->m_valid) continue;
			if (conflict_list_ptrs != NULL) {
				g_list_free(conflict_list_ptrs);
			}
			conflict_list_ptrs = NULL;
			if (conflict_list_nts != NULL) {
				g_list_free(conflict_list_nts);
			}
			conflict_list_nts = NULL;
			for (lptr2 = g_list_next(lptr1); lptr2; lptr2 = g_list_next(lptr2)) {
				note2 = (NedIdNote *) lptr2->data;
				if (!note2->m_valid) continue;
				if (note2->m_midi_start >= note1->m_midi_stop) break;
				if (note2->m_midi_start < note1->m_midi_stop) {
					conflict_list_ptrs = g_list_append(conflict_list_ptrs, lptr2);
					conflict_list_nts = g_list_append(conflict_list_nts, lptr2->data);
				}
			}
			for (lptr2 = g_list_first(conflict_list_ptrs); lptr2; lptr2 = g_list_next(lptr2)) {
				m_notes[voi] = g_list_delete_link(m_notes[voi], (GList *) (lptr2->data));
			}
			if (voi < VOICE_COUNT - 1) {
				for (lptr2 = g_list_first(conflict_list_nts); lptr2; lptr2 = g_list_next(lptr2)) {
					m_notes[voi + 1] = g_list_append(m_notes[voi + 1], lptr2->data);
				}
			}
		}
		if (conflict_list_ptrs != NULL) {
			g_list_free(conflict_list_ptrs);
			conflict_list_ptrs = NULL;
		}
		if (conflict_list_nts != NULL) {
			g_list_free(conflict_list_nts);
			conflict_list_nts = NULL;
		}
	}
	for (voi = 0; voi < VOICE_COUNT; voi++) {
		for (lptr1 = g_list_first(m_notes[voi]); lptr1; lptr1 = g_list_next(lptr1)) {
			note1 = (NedIdNote *) lptr1->data;
			if (!note1->m_valid) continue;
			if (note1->m_max_stop > 0 && note1->m_max_stop <= note1->m_midi_start) {
				NedResource::Abort("NedIdiotsInstrument::findConflictingNotes");
			}
		}
	}
}

void NedIdiotsInstrument::addSpecial(NedIdSpecial *spec) {
	m_specials = g_list_insert_sorted(m_specials, spec, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
}

GList *NedIdiotsInstrument::clone_specials() {
	GList *cl = NULL;
	GList *lptr;
	NedIdVolumeChange *vc0, *vc1;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		switch (((NedIdSpecial *) lptr->data)->m_type) {
			case ID_SPEC_VOLUME_CHANGE: vc0 = (NedIdVolumeChange *) lptr->data;
						vc1 = new NedIdVolumeChange(vc0->m_time, 0, vc0->m_volume);
						cl = g_list_insert_sorted(cl, vc1, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
						break;
		}
	}
	return cl;
}

int NedIdiotsInstrument::specCount() {
	return g_list_length(m_specials);
}

void NedIdiotsInstrument::generateVolumeSigns(int volume_change_density) {
	GList *lptr;
	int current_volume = -1000;
	int vol_dist;
	NedIdNote *note;
	NedIdVolumeChange *vc;

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (!note->m_valid) continue;
		vol_dist = note->m_volume - current_volume;
		if (vol_dist < 0) vol_dist = -vol_dist;
		if ((int) ((double) (vol_dist * 100) / /* 127.0 */ (double) (current_volume + 1) ) > volume_change_density) {
			vc = new NedIdVolumeChange(note->m_midi_start, 0, note->m_volume);
			m_specials = g_list_insert_sorted(m_specials, vc, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
			current_volume = note->m_volume;
		}
	}
}

GList * NedIdiotsInstrument::convertToNtEdNotes(NedIdiotsEditor *id, bool place_tempo_sigs, bool place_volume_or_key_signs, int voice_nr, unsigned long long last_time) {
	/* int j; */
	pitch_info *pi;
	GList *notes = NULL;
	NedChordOrRest *chord_or_rest = NULL, *last_chord = NULL, *last_splitted_chord = NULL;
	int measure_nr = 0;
	int dotcount, len2;
	unsigned long long start_time = 0, end_time, current_time;
	unsigned long long measure_width;
	unsigned long long last_tempo_test = 0, last_key_test = 0;
	int numerator, denominator;
	int current_tempo = -1, newtempo, newvolume = 1000, newkey;
	int last_key = 0;
	NedVolumeSign *volsign;
	GList *lptr2;
	NedIdNote *note, *tri_note;
	NedNote *ned_note;
	bool first, tie_required;
	bool volume_set = false;

	if (voice_nr == 0) {
		generateVolumeSigns(id->getVolumeChangeDensity());
		splitNotesAtMeasureStart(id);
		findChords(false);
		determineLines(id);
		enlargeNotesWithUnusualLength(id);
		findConflictingNotes();
		enlargeNotesToAvoidSmallRests(id);
		//findChords(true); // after "splitNotesAtMeasureStart" new chords are possible
	}
	id->getMeasureNumber(start_time, &numerator, &denominator);

	GList *lptr = g_list_first(m_notes[voice_nr]);
	do {
		measure_nr++;
		current_time = start_time;
		measure_width = numerator * WHOLE_NOTE / denominator;
		end_time = start_time + measure_width;
		for (; lptr; lptr = g_list_next(lptr)) {
			note = (NedIdNote *) lptr->data;
			if (!note->m_valid) continue;
			if (note->m_midi_start < start_time) {
				NedResource::Abort("NedIdiotsInstrument::convertToNtEdNotes(1)");
			}
			if (note->m_midi_start >= end_time) {
				NedResource::setLengthField(end_time - current_time);
			}
			else {
				NedResource::setLengthField(note->m_midi_start - current_time);
			}
			if (place_volume_or_key_signs && m_channel != 9 && id->newKeyNeeded(current_time, &newkey, &last_key_test)) {
				if (current_time != 0) {
					chord_or_rest = new NedChordOrRest(NULL, TYPE_KEYSIG, newkey, last_key, 0);
					notes = g_list_append(notes, chord_or_rest);
				}
				last_key = newkey;
			}
			len2 = NedResource::getPartLength(&dotcount, true, current_time == start_time && measure_width <= NOTE_4, measure_width);
			while (len2) {
				last_chord = NULL;
				chord_or_rest = new NedChordOrRest(NULL, TYPE_REST, false, 3 /* dummy */, dotcount, len2, NORMAL_NOTE, 0, 0);
				if (place_tempo_sigs && m_channel != 9 && id->newTempoNeeded(current_time, &newtempo, current_tempo, &last_tempo_test)) {
					new NedTempoSign(NOTE_4, newtempo, chord_or_rest, DEFAULT_FREE_X_DIST, DEFAULT_FREE_Y_DIST, true);
					current_tempo = newtempo;
				}
				current_time += chord_or_rest->getDuration(measure_width);
				notes = g_list_append(notes, chord_or_rest);
				len2 = NedResource::getPartLength(&dotcount, true, current_time == start_time, measure_width);
			}
			if (note->m_midi_start >= end_time) break;
			if (end_time < current_time + note->m_midi_stop - note->m_midi_start) {
				NedResource::Abort("NedIdiotsInstrument::convertToNtEdNotes(2a)");
			}
			if (note->m_midi_stop < note->m_midi_start) {
				NedResource::Abort("NedIdiotsInstrument::convertToNtEdNotes(2)");
			}
			if (place_volume_or_key_signs && m_channel != 9 && id->newKeyNeeded(current_time, &newkey, &last_key_test)) {
				if (current_time != 0) {
					chord_or_rest = new NedChordOrRest(NULL, TYPE_KEYSIG, newkey, last_key, 0);
					notes = g_list_append(notes, chord_or_rest);
				}
				last_key = newkey;
			}
			chord_or_rest = NULL;
			if (note->m_is_triplet_note) {
				for (int i = 0; i < 3; i++) {
					if ((tri_note = note->m_tripletnotes[i]) != NULL) {
						chord_or_rest = new NedChordOrRest(NULL, TYPE_CHORD, false, tri_note->m_line, 0, NOTE_8, CHOOSE_HEAD(tri_note->m_line), tri_note->m_state, 0);
						for (lptr2 = g_list_first(tri_note->m_chordnotes); lptr2; lptr2 = g_list_next(lptr2)) {
							pi = (pitch_info *) lptr2->data;
							ned_note = new NedNote(chord_or_rest, pi->m_line, CHOOSE_HEAD(pi->m_line), pi->m_state);
							chord_or_rest->insertNoteAt(ned_note);
						}
					}
					else {
						chord_or_rest = new NedChordOrRest(NULL, TYPE_REST, false, 3 /* dummy */, 0, NOTE_8, NORMAL_NOTE, 0, 0);
					}
					chord_or_rest->setTupletVal(3);
					notes = g_list_append(notes, chord_or_rest);
				}
				chord_or_rest->setLastTupletFlag(true);
				current_time += NOTE_4;
				last_chord = NULL;
			}
			else {
				first = true;
				last_splitted_chord = NULL;
#define SPLIT2
#ifdef SPLIT2
				NedResource::setLengthField2(start_time, note->m_midi_start, note->m_midi_stop - note->m_midi_start);
				len2 = NedResource::getPartLength2(&dotcount, false, true, measure_width);
#else
				NedResource::setLengthField(note->m_midi_stop - note->m_midi_start);
				len2 = NedResource::getPartLength(&dotcount, false, true, measure_width);
#endif
				while (len2) {
					chord_or_rest = new NedChordOrRest(NULL, TYPE_CHORD, false, note->m_line, dotcount, len2, CHOOSE_HEAD(note->m_line), note->m_state, 0);
					if (place_tempo_sigs && id->newTempoNeeded(current_time, &newtempo, current_tempo, &last_tempo_test)) {
						new NedTempoSign(NOTE_4, newtempo, chord_or_rest, DEFAULT_FREE_X_DIST, DEFAULT_FREE_Y_DIST, true);
						current_tempo = newtempo;
					}
					if (first && place_volume_or_key_signs && newVolumeNeeded(current_time, &newvolume)) {
						if (volume_set) {
							volsign = new NedVolumeSign(NedVolumeSign::determineKind(newvolume), chord_or_rest, DEFAULT_FREE_X_DIST, VOLUME_SIGN_Y_DIST, true);
							volsign->m_midi_volume = newvolume;
						}
						else {
							//m_volume = newvolume;
						}
						volume_set = true;
					}
					tie_required = (first && note->m_tiedbackward);
					for (lptr2 = g_list_first(note->m_chordnotes); lptr2; lptr2 = g_list_next(lptr2)) {
						pi = (pitch_info *) lptr2->data;
						ned_note = new NedNote(chord_or_rest, pi->m_line, CHOOSE_HEAD(pi->m_line), pi->m_state);
						tie_required = (tie_required || (first && pi->m_tiebackward));
						chord_or_rest->insertNoteAt(ned_note);
					}
					if (tie_required) {
						if (last_chord == NULL) {
							fprintf(stderr, "cannot tie\n"); fflush(stderr);
						}
						else {
							last_chord->tieCompleteTo(chord_or_rest, false);
							last_chord = NULL;
						}
					}
					if (last_splitted_chord != NULL) {
						last_splitted_chord->tieCompleteTo(chord_or_rest, false);
					}
					last_splitted_chord = chord_or_rest;
					current_time += chord_or_rest->getDuration(measure_width);
					notes = g_list_append(notes, chord_or_rest);
#ifdef SPLIT2
					len2 = NedResource::getPartLength2(&dotcount, false, true, measure_width);
#else
					len2 = NedResource::getPartLength(&dotcount, false, true, measure_width);
#endif
					first = false;
					last_chord = chord_or_rest;
				}
			}
		}
		if (end_time < current_time) {
			printf("Crash: end_time = %llu(%llu, %llu), current_time = %llu(%llu, %llu)\n",
					end_time, end_time / NOTE_64, end_time / NOTE_4, current_time, current_time / NOTE_64, current_time / NOTE_4); fflush(stdout);

			NedResource::Abort("NedIdiotsInstrument::convertToNtEdNotes(3)");
		}
		NedResource::setLengthField(end_time - current_time);
		if (place_volume_or_key_signs && id->newKeyNeeded(current_time, &newkey, &last_key_test)) {
			if (current_time != 0) {
				chord_or_rest = new NedChordOrRest(NULL, TYPE_KEYSIG, newkey, last_key, 0);
				notes = g_list_append(notes, chord_or_rest);
			}
			last_key = newkey;
		}
		len2 = NedResource::getPartLength(&dotcount, true, current_time == start_time && measure_width <= NOTE_4, measure_width);
		while (len2) {
			last_chord = NULL;
			chord_or_rest = new NedChordOrRest(NULL, TYPE_REST, false, 3 /* dummy */, dotcount, len2, NORMAL_NOTE, 0, 0);
			if (place_tempo_sigs && id->newTempoNeeded(current_time, &newtempo, current_tempo, &last_tempo_test)) {
				new NedTempoSign(NOTE_4, newtempo, chord_or_rest, DEFAULT_FREE_X_DIST, DEFAULT_FREE_Y_DIST, true);
				current_tempo = newtempo;
			}
			current_time += chord_or_rest->getDuration(measure_width);
			notes = g_list_append(notes, chord_or_rest);
			len2 = NedResource::getPartLength(&dotcount, true, current_time == start_time, measure_width);
		}
		if (end_time != current_time) {
			printf("Crash: end_time = %llu(%llu, %llu), current_time = %llu(%llu, %llu)\n",
					end_time, end_time / NOTE_64, end_time / NOTE_4, current_time, current_time / NOTE_64, current_time / NOTE_4); fflush(stdout);

			NedResource::Abort("NedIdiotsInstrument::convertToNtEdNotes(4)");
		}
		id->getMeasureNumber(end_time, &numerator, &denominator);
		start_time = end_time;
	}
	while(end_time < last_time);
	/*
	}
	else {
		for (j = 0; j < measure_nr; j++) {
			chord_or_rest = new NedChordOrRest(NULL, TYPE_REST, true, 3, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0);
			notes = g_list_append(notes, chord_or_rest);
		}
	}
	*/
	return notes;
}

void NedIdiotsInstrument::addNote(int pitch, int volume, unsigned long long  start, int midi_nr) {
	m_notes[0] = g_list_append(m_notes[0], new NedIdNote(pitch, volume, start));
}

void NedIdiotsInstrument::addNote(NedIdNote *note) {
	m_notes[0] = g_list_append(m_notes[0], note);
}

void NedIdiotsInstrument::setInstrumentName(char *name) {
	strcpy(m_inst_name, name);
}



void NedIdiotsInstrument::closeNote(int pitch, unsigned long long end_time, int midi_nr) {
	GList *lptr;

	for (lptr = g_list_last(m_notes[0]); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdNote *) lptr->data)->m_pitch == pitch &&
			!((NedIdNote *) lptr->data)->m_valid && ((NedIdNote *) lptr->data)->m_midi_start < end_time) {
			((NedIdNote *) lptr->data)->m_midi_stop =
			((NedIdNote *) lptr->data)->m_ori_stop = end_time;
			((NedIdNote *) lptr->data)->m_valid = true;
			return;
		}
	}
	printf("cannot close note, inst = %d, pitch = %d\n", m_midi_nr, pitch); fflush(stdout);
}

#define MAX_BORDER_CLICK_DIST 5.4

void NedIdiotsInstrument::handle_button_press (GdkEventButton *event, double xleft, double ytop, double visible_start_pos, double visible_end_pos) {
	NedIdNote *note, *prev_selected;
	GList * lptr;
	double dist;
	double zoom_value = m_id_edit->getZoomValue();
	
	prev_selected = m_selected_note;
	m_selected_note = NULL;

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (note->m_midi_start / FACTOR * zoom_value > visible_end_pos) continue;
		if (note->m_midi_stop / FACTOR * zoom_value < visible_start_pos) continue;
		double startpos = X_POS(note->m_midi_start);
		double endpos = X_POS(note->m_midi_stop);
		double ypos = PITCH_TO_YPOS(note->m_pitch);
		if (note->isInRect(event->x, event->y, zoom_value, xleft, ytop)) {
			m_selected_note = note;
			dist = event->x - startpos;
			if (dist < 0) dist = -dist;
			if (dist < MAX_BORDER_CLICK_DIST) {
				//printf("move start\n"); fflush(stdout);
				gdk_window_set_cursor (GDK_WINDOW(m_id_edit->getDrawingArea()->window), m_id_edit->getDoubldArrowCursor());
				m_id_edit->motion_mode = MOTION_NODE_LEFT;
			}
			else {
				dist = event->x - endpos;
				if (dist < 0) dist = -dist;
				if (dist < MAX_BORDER_CLICK_DIST) {
					//printf("move end\n"); fflush(stdout);
					m_id_edit->motion_mode = MOTION_NODE_RIGHT;
					gdk_window_set_cursor (GDK_WINDOW(m_id_edit->getDrawingArea()->window), m_id_edit->getDoubldArrowCursor());
				}
				else {
					//printf("move mid\n"); fflush(stdout);
					m_id_edit->motion_mode = MOTION_MODE_MOVE;
					gdk_window_set_cursor (GDK_WINDOW(m_id_edit->getDrawingArea()->window), m_id_edit->getFleurCursor());
				}
			}
		}
	}
	if (m_selected_note == prev_selected) return;
	m_id_edit->repaint();

}




NedIdiotsEditor::NedIdiotsEditor(int tempo_change_density, int volume_change_density, bool force_piano, bool sort_instruments, bool dont_split, bool X11Version /* = false */) :
			m_tempo_inverse(START_TEMPO_INVERSE), motion_mode(MOTION_MODE_NONE),
			m_zoom_value(ID_DEFAULT_ZOOM), m_force_piano(force_piano), 
			m_tempo_change_density(tempo_change_density), m_volume_change_density(volume_change_density),
		m_current_instrument_number(0), m_channel_nr(0), m_last_inserted(NULL),
		 m_sort_according_instrument(sort_instruments), m_dont_split(dont_split),
		 m_X11version(X11Version), m_min_denom(1), m_instruments(NULL), m_specials(NULL), 
		m_mouse_x(0.0), m_mouse_y(0.0), m_dpy(NULL), m_off_x(0.0), m_off_y(0.0) {

	m_pending_inst_name[0] = '\0';
	GtkWidget *menubar;
	if (m_X11version) {
	GtkWidget *drawing_hbox;
	GdkColor bgcolor;
	GtkWidget *main_vbox;
	GtkWidget *toolbar;
	GtkWidget *hscrollbar;
	GtkWidget *vscrollbar;
	GtkAccelGroup *accel_group;
#ifdef ORIORI
	GtkActionGroup *instrument_group;
#endif
	m_command_history = new NedIdCommandHistory(this);

	GError *error;
    	double min, max;

	m_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (m_main_window), _("Idiot's editor"));
	gtk_window_set_default_size (GTK_WINDOW (m_main_window), 600, 400);

	m_menu_action_group = gtk_action_group_new ("IdMenuActions");
	gtk_action_group_set_translation_domain(m_menu_action_group, NULL);
	gtk_action_group_add_actions (m_menu_action_group, file_entries, G_N_ELEMENTS (file_entries), (void *) this);

	m_ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (m_ui_manager, m_menu_action_group, 0);
	accel_group = gtk_ui_manager_get_accel_group (m_ui_manager);
	gtk_window_add_accel_group (GTK_WINDOW (m_main_window), accel_group);

	error = NULL;
	if (!gtk_ui_manager_add_ui_from_string (m_ui_manager, guiDescription, -1, &error)) {
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
		NedResource::Abort("error");
	}

	menubar = gtk_ui_manager_get_widget (m_ui_manager, "/IdMainMenu");
	
	toolbar = gtk_ui_manager_get_widget (m_ui_manager, "/IdToolBar");
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	//table = gtk_notebook_new();
	m_instrument_toolbar = gtk_toolbar_new();
	m_instr_bu = gtk_radio_button_new_with_label(NULL, "Inst 1");
	g_signal_connect (GTK_BUTTON(m_instr_bu), "pressed", G_CALLBACK (select_instrument), (void *) this);
	gtk_toolbar_append_widget(GTK_TOOLBAR(m_instrument_toolbar), m_instr_bu , NULL, NULL);

    m_drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered(m_drawing_area, FALSE);
    //gtk_widget_set_size_request(m_drawing_area, 16 * m_zoom_value , (int) ID_HEIGHT);
    gtk_widget_set_size_request(m_drawing_area, START_WIDTH, START_HEIGHT);
    m_hadjust = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 100, 1, 100, START_WIDTH));
    m_vadjust = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, ID_HEIGHT, 1, 10, START_HEIGHT));
    hscrollbar = gtk_hscrollbar_new(m_hadjust);
    vscrollbar = gtk_vscrollbar_new(m_vadjust);


    bgcolor.pixel = 0;
    bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
    gtk_widget_modify_bg(GTK_WIDGET(m_drawing_area), GTK_STATE_NORMAL, &bgcolor);


/*
	 GTK_WIDGET_SET_FLAGS(m_drawing_area, GTK_CAN_FOCUS);
	 GTK_WIDGET_SET_FLAGS(m_main_window, GTK_CAN_FOCUS);
	 GTK_WIDGET_SET_FLAGS(m_drawing_area, GTK_CAN_FOCUS);
	 GTK_WIDGET_SET_FLAGS(m_main_window, GTK_CAN_FOCUS);
	gtk_widget_add_events(m_main_window, GDK_KEY_PRESS_MASK |  GDK_KEY_RELEASE_MASK );
	gtk_widget_add_events(m_drawing_area, GDK_KEY_PRESS_MASK |  GDK_KEY_RELEASE_MASK );
	gtk_widget_add_events(m_main_window, GDK_KEY_RELEASE_MASK );
	gtk_widget_add_events(m_drawing_area, GDK_KEY_RELEASE_MASK );
*/
	/*
	gtk_widget_add_events(m_drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
			GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
			*/
	m_double_arrow = gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);
	m_pointer = gdk_cursor_new(GDK_LEFT_PTR);
	m_fleur = gdk_cursor_new(GDK_FLEUR);

gtk_widget_add_events(m_drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK );
    g_signal_connect (m_drawing_area, "expose-event",
		      G_CALLBACK (handle_expose), (void *) this);
    g_signal_connect (m_hadjust, "value-changed", 
		      G_CALLBACK (handle_scroll), (void *) this);
    g_signal_connect (m_vadjust, "value-changed", 
		      G_CALLBACK (handle_scroll), (void *) this);
    g_signal_connect (m_drawing_area, "size-allocate",
		      G_CALLBACK (size_change_handler), (void *) this);
	
    g_signal_connect (m_drawing_area, "button-press-event",
		      G_CALLBACK (handle_button_press), (void *) this);
    g_signal_connect (m_drawing_area, "motion-notify-event",
		      G_CALLBACK (handle_motion), (void *) this);
    g_signal_connect (m_drawing_area, "button-release-event",
		      G_CALLBACK (handle_button_release), (void *) this);
/*
    g_signal_connect (m_drawing_area, "motion-notify-event",
		      G_CALLBACK (handle_motion), (void *) this);
    g_signal_connect (m_drawing_area, "key-press-event",
		      G_CALLBACK (key_press_handler), (void *) this);
    g_signal_connect (m_drawing_area, "key-release-event",
		      G_CALLBACK (key_release_handler), (void *) this);
    g_signal_connect (m_drawing_area, "enter-notify-event",
		      G_CALLBACK (window_enter_handler), (void *) this);
    g_signal_connect (m_drawing_area, "leave-notify-event",
		      G_CALLBACK (window_leave_handler), (void *) this);
    g_signal_connect (m_drawing_area, "scroll-event",
		      G_CALLBACK (scroll_event_handler), (void *) this);
    g_signal_connect (m_main_window, "delete-event",
		      G_CALLBACK (handle_delete), (void *) this);
    g_signal_connect (m_page_selector, "value-changed",
		      G_CALLBACK (handle_page_request), (void *) this);
		      */

	drawing_hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (drawing_hbox), m_drawing_area, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (drawing_hbox), vscrollbar, FALSE, FALSE, 0);
	main_vbox = gtk_vbox_new (FALSE, 0);
	
	gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), m_instrument_toolbar, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (main_vbox), drawing_hbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), hscrollbar, FALSE, FALSE, 0);
    gtk_container_add (GTK_CONTAINER (m_main_window), main_vbox);


    gtk_widget_show_all (m_main_window);
    g_object_get (m_vadjust, "lower", &min, "upper", &max, NULL);
     gtk_adjustment_set_value(GTK_ADJUSTMENT(m_vadjust), min + (max - min) / 3.0);
     } /* if (x11version) */
        m_instruments = g_list_append(m_instruments, new NedIdiotsInstrument(this, m_channel_nr++, -2,  -2, m_volume_change_density));

	setLastTime(4 * NOTE_4);


}

NedIdiotsEditor::~NedIdiotsEditor() {
	GList *lptr;

	if (m_dpy != NULL)  {
		XFreePixmap(m_dpy, m_back_image);
	}
	if (m_X11version) {
		gtk_widget_destroy(m_main_window);
	}

	if (m_instruments != NULL) {
		for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
			delete (NedIdiotsInstrument *) lptr->data;
		}
		g_list_free(m_instruments);
		m_instruments = NULL;
	}
	if (m_specials != NULL) {
		for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
			delete (NedIdSpecial *) lptr->data;
		}
		g_list_free(m_specials);
		m_specials = NULL;
	}
}

void  NedIdiotsEditor::initX11() {
	char *image_data;
	unsigned int i;
	bool ok = false;
	XGCValues vals;
	const char* font_names [] = {
	"-misc-*-bold-r-normal--13-*-*-*-*-*-*-*",
	"-*-*-bold-r-normal--13-*-*-*-*-*-*-*",
	"-*-*-bold-*-normal--13-*-*-*-*-*-*-*",
	"-*-*-bold-*-*--13-*-*-*-*-*-*-*",
	"-*-*-*-*-*--13-*-*-*-*-*-*-*"};
	XFontStruct *font_info;

	if (m_dpy != NULL) return;

	GTK_WIDGET_UNSET_FLAGS (m_drawing_area, GTK_DOUBLE_BUFFERED);
	m_dpy = gdk_x11_drawable_get_xdisplay(m_drawing_area->window);
	m_win = gdk_x11_drawable_get_xid(m_drawing_area->window);
	m_screen = DefaultScreen(m_dpy);
	m_xgc = DefaultGC(m_dpy, m_screen);
	for (i = 0; i < sizeof(font_names) / sizeof(const char*); i++) {
		font_info = XLoadQueryFont(m_dpy, font_names[i]);
		if (font_info) {
			ok = true;
			break;
		}
	}
	if (ok) {
		XSetFont(m_dpy, m_xgc, font_info->fid);
	}
	else {
		fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", font_names[(sizeof(font_names) / sizeof(const char*)) - 1]);
	}
	m_x_border_gc = XCreateGC(m_dpy, m_win, 0, &vals);
	if ((image_data = (char *) g_try_malloc(4 * m_drawing_area->allocation.width * m_drawing_area->allocation.height)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::handle_map_event");
	}
	m_back_image = XCreatePixmap(m_dpy, m_win, m_drawing_area->allocation.width, m_drawing_area->allocation.height,
				DefaultDepth(m_dpy, m_screen));
}


gboolean NedIdiotsEditor::handle_button_press (GtkWidget *widget, GdkEventButton *event, gpointer data) {
	if (event->button != 1) return FALSE;
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	double visible_start_pos, visible_end_pos;
	double xleft = gtk_adjustment_get_value(id_editor->m_hadjust);
	double ytop = gtk_adjustment_get_value(id_editor->m_vadjust);
	int width = id_editor->m_drawing_area->allocation.width;
	int height = id_editor->m_drawing_area->allocation.height;

	visible_start_pos = xleft;
	visible_end_pos = xleft + width;

	id_editor->m_mouse_x = event->x;
	id_editor->m_mouse_y = event->y;

	NedIdiotsInstrument *inst = (NedIdiotsInstrument *) g_list_first(id_editor->m_instruments)->data;

	inst->handle_button_press(event, xleft, ytop, visible_start_pos, visible_end_pos);
	return FALSE;

}

gboolean NedIdiotsEditor::handle_motion (GtkWidget *widget, GdkEventMotion  *event, gpointer data)
{

	if ((event->state & GDK_BUTTON1_MASK) == 0) return FALSE;
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	if (id_editor->motion_mode == MOTION_MODE_NONE) return FALSE;
	NedIdiotsInstrument *inst = (NedIdiotsInstrument *) g_list_first(id_editor->m_instruments)->data;
	id_editor->m_off_x = event->x - id_editor->m_mouse_x;
	id_editor->m_off_y = event->y - id_editor->m_mouse_y;
	id_editor->repaint();
	return FALSE;
}

gboolean NedIdiotsEditor::handle_button_release (GtkWidget *widget, GdkEventButton  *event, gpointer data)
{
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	double xleft = gtk_adjustment_get_value(id_editor->m_hadjust);
	double ytop = gtk_adjustment_get_value(id_editor->m_vadjust);
	double zoom_value = id_editor->getZoomValue();

	gdk_window_set_cursor(GDK_WINDOW(id_editor->m_drawing_area->window), id_editor->m_pointer);
	NedIdiotsInstrument *inst = (NedIdiotsInstrument *) g_list_first(id_editor->m_instruments)->data;
	inst->recomputeNotes(zoom_value, xleft, ytop);
	id_editor->motion_mode = MOTION_MODE_NONE;
	id_editor->m_off_x = id_editor->m_off_y = 0.0;
	id_editor->repaint();
}

void NedIdiotsInstrument::recomputeNotes(double zoom_value, double xleft, double ytop) {
	if (!m_selected_note) return;
	GList *lptr;
	NedIdNote *note;
	unsigned int f, snap, xpitch;
	double xstart, xend, ypos;
	unsigned int newstart, newstop;
	int newpitch;
	NedChangeIdNoteCommand *nidchange;
	NedIdCommandList *nidchange_cl;

	for (lptr = g_list_first(m_notes[0]); lptr; lptr = g_list_next(lptr)) {
		note = (NedIdNote *) lptr->data;
		if (note != m_selected_note) continue;
		switch(m_id_edit->motion_mode) {
			case MOTION_MODE_MOVE:
				ypos = (116 - (note->m_pitch - 4)) * SEP_LINE_DIST + SEP_LINE_BORDER - ytop - SPEC_TRACKS_HEIGHT;
				newpitch = POS_TO_PITCH(ypos + m_id_edit->getOffY());
				xstart = PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX();
				xend = PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX() + (note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value;
				newstart = (xstart - PIANO_PART_WIDTH + xleft) * FACTOR / zoom_value;
				newstop  = (xend - PIANO_PART_WIDTH + xleft) * FACTOR / zoom_value;
				snap = WHOLE_NOTE / m_id_edit->getMinDenom();
				f = (newstart + snap / 2) / snap;
				newstart = snap * f;
				f = (newstop + snap / 2) / snap;
				newstop = snap * f;
				nidchange = new NedChangeIdNoteCommand(note, newstart, newstop, newpitch);
				nidchange_cl = new NedIdCommandList(m_id_edit);
				nidchange_cl->addCommand(nidchange);
				nidchange->execute();
				m_id_edit->getCommandHistory()->addCommandList(nidchange_cl);
				m_selected_note = NULL;
				return;
			case MOTION_NODE_LEFT:
				xstart = PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX();
				newstart = (xstart - PIANO_PART_WIDTH + xleft) * FACTOR / zoom_value;
				snap = WHOLE_NOTE / m_id_edit->getMinDenom();
				f = (newstart + snap / 2) / snap;
				newstart = snap * f;
				nidchange = new NedChangeIdNoteCommand(note, newstart, note->m_midi_stop, note->m_pitch);
				nidchange_cl = new NedIdCommandList(m_id_edit);
				nidchange_cl->addCommand(nidchange);
				nidchange->execute();
				m_id_edit->getCommandHistory()->addCommandList(nidchange_cl);
				m_selected_note = NULL;
				return;
			case MOTION_NODE_RIGHT:
				xend = PIANO_PART_WIDTH + note->m_midi_start / FACTOR * zoom_value - xleft + m_id_edit->getOffX() + (note->m_midi_stop - note->m_midi_start) / FACTOR * zoom_value;
				newstop = (xend - PIANO_PART_WIDTH + xleft) * FACTOR / zoom_value;
				snap = WHOLE_NOTE / m_id_edit->getMinDenom();
				f = (newstop + snap / 2) / snap;
				newstop = snap * f;
				m_selected_note = NULL;
				nidchange = new NedChangeIdNoteCommand(note, note->m_midi_start, newstop, note->m_pitch);
				nidchange_cl = new NedIdCommandList(m_id_edit);
				nidchange_cl->addCommand(nidchange);
				nidchange->execute();
				m_id_edit->getCommandHistory()->addCommandList(nidchange_cl);
				return;
			default: return;
		}
	}
}


void NedIdiotsEditor::close_i_edit(GtkWidget  *widget, void *data) {
	/* GList *lptr; */
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;

/*
	for (i = 0, lptr = g_list_first(id_editor->m_instruments); lptr; lptr = g_list_next(lptr), i++) {
		((NedIdiotsInstrument *) lptr->data)->convertToNotes(id_editor, i, id_editor->m_last_time);
	}

	gtk_widget_destroy(id_editor->m_main_window);
*/
	delete id_editor;
}


void NedIdiotsEditor::addTimeSigChange(unsigned long long time, int numerator, int denominator) {
	NedIdTimesigChange *time_sig_change;
	bool deleted;
	time_sig_change = new NedIdTimesigChange(time, 0, numerator, denominator);
	m_specials = g_list_insert_sorted(m_specials, time_sig_change, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
	do {
		correct_times_of_specials();
		deleted = delete_double_specials();
	}
	while (deleted);
}

void NedIdiotsEditor::addTempoChange(unsigned long long time, int tempo) {
	NedIdTempoChange *tempo_change;
	bool deleted;
	tempo_change = new NedIdTempoChange(time, 0, tempo);
	m_specials = g_list_insert_sorted(m_specials, tempo_change, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
	do {
		deleted = delete_double_specials();
	}
	while (deleted);

}

bool NedIdiotsEditor::newTempoNeeded(unsigned long long current_time, int *newtempo, int current_tempo, unsigned long long  *last_tempo_test) {
	GList *lptr;
	int tempo = -1;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time <= *last_tempo_test) continue;
		if (((NedIdSpecial *) lptr->data)->m_time > current_time) break;
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TEMPO_CHANGE) continue;
		if (((NedIdTempoChange *) lptr->data)->m_used) continue;
		tempo = ((NedIdTempoChange *) lptr->data)->m_tempo;
		((NedIdTempoChange *) lptr->data)->m_used = true;
	}

	if (tempo >= 0) {
		if (tempo == current_tempo) return false;
		*newtempo = tempo;
		*last_tempo_test = current_time;
		return true;
	}
	return false;
}

bool NedIdiotsEditor::newKeyNeeded(unsigned long long current_time, int *newkey, unsigned long long  *last_key_test) {
	GList *lptr;
	int key = -100;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time <= *last_key_test) continue;
		if (((NedIdSpecial *) lptr->data)->m_time > current_time) break;
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_KEYSIG_CHANGE) continue;
		key = ((NedIdKeysigChange *) lptr->data)->m_key;
	}

	if (key > -100) {
		*newkey = key;
		*last_key_test = current_time;
		return true;
	}
	return false;
}

	

void NedIdiotsEditor::correct_times_of_specials() {
	unsigned long long last_measure_change_time = 0;
	unsigned long long time;
	unsigned long current_measure_length = WHOLE_NOTE;
	double measure_dist_d;
	unsigned int measure_dist;
	GList *lptr;
	bool time_changed;

	m_specials = g_list_sort(m_specials, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);

	do {
		time_changed = false;
		for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
			if (last_measure_change_time > ((NedIdSpecial *)lptr->data)->m_time) {
				//time_changed = true;
				continue;
			}
			measure_dist_d = (double)(((NedIdSpecial *)lptr->data)->m_time - last_measure_change_time) / (double) current_measure_length;
			measure_dist = (unsigned int) measure_dist_d;
			time = last_measure_change_time + (unsigned long long) measure_dist * (unsigned long long) current_measure_length;
			if (time != ((NedIdSpecial *)lptr->data)->m_time) {
				time_changed = true;
				((NedIdSpecial *)lptr->data)->m_time = time;
			}
			//((NedIdSpecial *)lptr->data)->m_time = time;
			//((NedIdSpecial *)lptr->data)->m_measure_number = last_measure_change_number + measure_dist;
			if (((NedIdSpecial *)lptr->data)->m_type == ID_SPEC_TIME_SIG_CHANGE) {
				current_measure_length = ((NedIdTimesigChange *)lptr->data)->m_numerator * WHOLE_NOTE / ((NedIdTimesigChange *)lptr->data)->m_denominator;
				last_measure_change_time = ((NedIdSpecial *)lptr->data)->m_time;
			}
		}
		if (time_changed) {
			m_specials = g_list_sort(m_specials, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
		}
	}
	while (time_changed);
}

bool NedIdiotsEditor::delete_double_specials() {
	int type = ID_SPEC_TIME_SIG_CHANGE;
	GList *lptr, *lptr2;
	bool del;
	bool deleted = false;
	NedIdSpecial *spec;
	unsigned long long time;

	for (type = ID_SPEC_TIME_SIG_CHANGE; type <= ID_SPEC_TEMPO_CHANGE; type++) {
		for(lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
			if (((NedIdSpecial *)lptr->data)->m_type != type) continue;
			time = ((NedIdSpecial *)lptr->data)->m_time;
			del= false;
			for (lptr2 = g_list_first(m_specials); lptr2; lptr2 = g_list_next(lptr2)) {
				if (lptr2 == lptr) continue;
				if (((NedIdSpecial *)lptr2->data)->m_type != type) continue;
				if (((NedIdSpecial *)lptr2->data)->m_time == time) {
					del = true;
					deleted = true;
					spec = (NedIdSpecial *) lptr2->data;
					m_specials = g_list_delete_link(m_specials, lptr2);
					switch (spec->m_type) {
						case ID_SPEC_TIME_SIG_CHANGE: delete (NedIdTimesigChange *) spec; break;
						case ID_SPEC_TEMPO_CHANGE: delete (NedIdTempoChange *) spec; break;
						default: NedResource::Abort("delete_double_specials(1)"); break;
					}
					lptr2 = g_list_first(m_specials);
				}
			}
			if (del) {
				if ((lptr = g_list_find(m_specials, lptr->data)) == NULL) {
					NedResource::Abort("delete_double_specials(2)");
				}
			}
		}
	}
	return deleted;
}

void NedIdiotsEditor::thin_out_temposigs() {
	GList *lptr;
	GList *del_list_ptr = NULL, *del_list_specials = NULL;
	int last_tempo = -1, tempo_dist, proz_dist;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *)lptr->data)->m_type != ID_SPEC_TEMPO_CHANGE) continue;
		tempo_dist = ((NedIdTempoChange *) lptr->data)->m_tempo - last_tempo;
		if (tempo_dist < 0) tempo_dist = -tempo_dist;
		proz_dist = (int) ((double) (tempo_dist * 100) / /*127.0*/ (double) (last_tempo + 1));
		if (last_tempo > 0 && proz_dist < m_tempo_change_density) {
			del_list_ptr = g_list_append(del_list_ptr, lptr);
			del_list_specials = g_list_append(del_list_specials, lptr->data);
		}
		else {
			last_tempo = ((NedIdTempoChange *) lptr->data)->m_tempo;
		}
	}
	for (lptr = g_list_first(del_list_ptr); lptr; lptr = g_list_next(lptr)) {
		m_specials = g_list_delete_link(m_specials, (GList *) lptr->data);
	}
	for (lptr = g_list_first(del_list_specials); lptr; lptr = g_list_next(lptr)) {
		delete (NedIdTempoChange *) lptr->data;
	}
	g_list_free(del_list_ptr);
	g_list_free(del_list_specials);
}

int NedIdiotsEditor::determine_key(unsigned long long time, int channel) {
	GList *lptr;

	if (channel == 9) return 0;

	for (lptr = g_list_last(m_specials); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_KEYSIG_CHANGE) continue;
		if (((NedIdSpecial *) lptr->data)->m_time <= time) return ((NedIdKeysigChange *) lptr->data)->m_key;
	}
	return 0;
}



void NedIdiotsEditor::getNumDenom(unsigned long long time, unsigned int *num, unsigned int *denom) {
	GList *lptr;

	*num = *denom = 4;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *)lptr->data)->m_time > time) break;
		if (((NedIdSpecial *)lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		*num = ((NedIdTimesigChange *)lptr->data)->m_numerator;
		*denom = ((NedIdTimesigChange *)lptr->data)->m_denominator;
	}
}

char *NedIdiotsInstrument::getInstrumentName() {
	static char inst_name[128];

	if (m_inst_name[0] != '\0') return m_inst_name;
	sprintf(inst_name, "Inst %d", m_inst_number + 1);
	return inst_name;
}



void NedIdiotsEditor::addNote(int pitch, int volume, unsigned long long  start, int midi_nr, int chan) {
	GList *lptr;
	NedIdiotsInstrument *inst = NULL;


	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdiotsInstrument *) lptr->data)->getMidiNr() == -2) {
			inst = (NedIdiotsInstrument *) lptr->data;
			inst->setMidiNr(midi_nr);
			inst->setOrigChannel(chan);
			inst->setChannel(chan);
			break;
		}
		if (m_sort_according_instrument) {
			if (((NedIdiotsInstrument *) lptr->data)->getMidiNr() == midi_nr) {
				inst = (NedIdiotsInstrument *) lptr->data;
				inst->setOrigChannel(chan);
				break;
			}
		}
		else {
			if (((NedIdiotsInstrument *) lptr->data)->getOrigChannel() == chan) {
				inst = (NedIdiotsInstrument *) lptr->data;
				break;
			}
		}
	}

	if (inst == NULL) {
		if (midi_nr == -1) {
			inst = new NedIdiotsInstrument(this, 9, chan, midi_nr, m_volume_change_density);
		}
		else {
			while (++m_channel_nr == 9);
        		inst = new NedIdiotsInstrument(this, m_channel_nr, chan, midi_nr, m_volume_change_density);
		}
		m_instruments = g_list_append(m_instruments, inst);
	}
	inst->addNote(pitch, volume, start, midi_nr);
	m_last_inserted = inst;
	if (m_pending_inst_name[0] != '\0') {
		inst->setInstrumentName(m_pending_inst_name);
		m_pending_inst_name[0] = '\0';
	}
}

void NedIdiotsEditor::addKeySig(unsigned long long time, int measure_number, int key) {
	NedIdKeysigChange* kc = new NedIdKeysigChange(time, measure_number, key);
	m_specials = g_list_insert_sorted(m_specials, kc, (gint (*)(const void*, const void*)) NedIdSpecial::compare_specials);
}

void NedIdiotsEditor::closeNote(int pitch, unsigned long long end_time, int midi_nr, int chan) {
	GList *lptr;
	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		if (m_sort_according_instrument) {
			if (((NedIdiotsInstrument *) lptr->data)->getMidiNr() != midi_nr) continue;
		}
		else {
			if (((NedIdiotsInstrument *) lptr->data)->getOrigChannel() != chan) continue;
		}
		((NedIdiotsInstrument *) lptr->data)->closeNote(pitch, end_time, midi_nr);
		return;
	}
	fprintf(stderr, "cannot close note(1), midi_nr = %d\n", midi_nr); fflush(stderr);
}

gboolean NedIdiotsEditor::handle_expose (GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
#ifdef ORIORI
	if (id_editor->m_X11version) {
#endif
		id_editor->draw_new();
#ifdef ORIORI
	}
	else {
		id_editor->draw();
	}
#endif
	
	return FALSE;
}

void NedIdiotsEditor::repaint() {
	if (!m_X11version) return;
	gdk_window_invalidate_rect (m_main_window->window, NULL, FALSE);
	gdk_window_invalidate_rect (m_drawing_area->window, NULL, FALSE);
}

void NedIdiotsEditor::handle_scroll(GtkAdjustment *adjustment, gpointer data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	if (!id_editor->m_X11version) return;
	id_editor->repaint();
}

void NedIdiotsEditor::setHadjust() {
	if (!m_X11version) return;
	m_hadjust->upper = m_paper_len * m_zoom_value;
	m_hadjust->page_size = m_drawing_area->allocation.width - PIANO_PART_WIDTH;
	m_hadjust->page_increment = (m_drawing_area->allocation.width - PIANO_PART_WIDTH) / STEP_DIVISION;
}

void NedIdiotsEditor::setVadjust() {
	if (!m_X11version) return;
	m_vadjust->upper = ID_HEIGHT - SPEC_TRACKS_HEIGHT;
	m_vadjust->page_size = m_drawing_area->allocation.height;
	m_vadjust->page_increment = m_drawing_area->allocation.height / STEP_DIVISION;
}

void NedIdiotsEditor::setLastTime(unsigned long long last_time) {
	GList *lptr;
	GtkWidget *instr_bu;
	int inst_num = 0;

	m_paper_len = last_time / FACTOR;
	m_last_time = last_time;
    	//gtk_widget_set_size_request(m_drawing_area, (int) (m_paper_len * m_zoom_value) , (int) ID_HEIGHT);
	
	setHadjust();
	/*
	printf("A:up = %f (l = %f, zv = %f), ps = %f, pi = %f\n", m_hadjust->upper,
		m_paper_len, m_zoom_value, 
		m_hadjust->page_size, m_hadjust->page_increment);
		*/
	lptr = g_list_first(m_instruments);
	((NedIdiotsInstrument *) lptr->data)->setInstNumber(inst_num);
	if (m_X11version) {
	gtk_button_set_label(GTK_BUTTON(m_instr_bu), ((NedIdiotsInstrument *) lptr->data)->getInstrumentName());
	}
	for (inst_num++,lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr), inst_num++) {
		((NedIdiotsInstrument *) lptr->data)->setInstNumber(inst_num);
	if (m_X11version) {
		instr_bu = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_instr_bu), ((NedIdiotsInstrument *) lptr->data)->getInstrumentName());
		g_signal_connect (GTK_BUTTON(instr_bu), "pressed", G_CALLBACK (select_instrument), (void *) this);
		gtk_toolbar_append_widget(GTK_TOOLBAR(m_instrument_toolbar), instr_bu , NULL, NULL);
	}
	}
	if (m_X11version) {
    	gtk_widget_hide (m_main_window);
    	gtk_widget_show_all (m_main_window);
	}
}

#ifdef ORIORI
void NedIdiotsEditor::draw () {
	int i;
	double x, y;
	int width, height;
	cairo_t *cr;
	unsigned int measure_width;
	unsigned long long current_time, time_dist;
	double d, measure_dist_d;
	int measure_dist;
	double start_pos_measure, end_pos_measure;
	double visible_start_pos, visible_end_pos;
	unsigned int  current_dist, nominator_dist;
	int current_color;
	unsigned int curr_dnom;
	GList *lptr;
	unsigned int numerator = 4;
	unsigned int denominator = 4;



	x = gtk_adjustment_get_value(m_hadjust);
	y = gtk_adjustment_get_value(m_vadjust);
	width = m_drawing_area->allocation.width;
	height = m_drawing_area->allocation.height;

	cr = gdk_cairo_create (m_drawing_area->window);
	//cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

#define BAR_COLOR 0.0, 0.0, 0.0
#define NOMINATOR_COLOR 0.0, 0.0, 0.8
#define HALF_COLOR 0.0, 0.1, 0.6
#define QUARTER_COLOR  0.0, 0.2, 0.5
#define EIGHTS_COLOR 0.5, 0.3, 0.4
#define COLOR_16 0.0, 0.4, 0.3
#define COLOR32  0.5, 0.5, 0.2
#define COLOR64 0.9, 0.6, 0.1

#ifdef ORIORI
	static double color_array[8][3] = {
		{BAR_COLOR},
		{NOMINATOR_COLOR},
		{HALF_COLOR},
		{QUARTER_COLOR},
		{EIGHTS_COLOR},
		{COLOR_16},
		{COLOR32},
		{COLOR64}};
#endif



	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  PIANO_PART_WIDTH,  SPEC_TRACKS_HEIGHT, width - PIANO_PART_WIDTH, height - SPEC_TRACKS_HEIGHT);
	cairo_fill(cr);
	cairo_set_line_width(cr, SEP_LINE_THICK);
	for (i = 4; i < 4 + 117; i++) {
		cairo_new_path(cr);
		if (strlen(pitch_tab[i-4]) > 1) {
			cairo_new_path(cr);
			cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
			cairo_rectangle (cr, PIANO_PART_WIDTH, (i - 4) * SEP_LINE_DIST - y - SPEC_TRACKS_HEIGHT, width - PIANO_PART_WIDTH, SEP_LINE_DIST);
			cairo_fill(cr);
		}
		cairo_new_path(cr);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		if (y < (i - 3) * SEP_LINE_DIST  && y + height > (i - 4) * SEP_LINE_DIST) {
			cairo_move_to(cr, PIANO_PART_WIDTH, (i - 4) * SEP_LINE_DIST - y - SPEC_TRACKS_HEIGHT);
			cairo_line_to(cr, width, (i - 4) * SEP_LINE_DIST - y - SPEC_TRACKS_HEIGHT);
		}
		cairo_stroke(cr);
	}


	visible_start_pos = x;
	/*
	current_time = 0;
	getNumDenom(current_time, &numerator, &denominator);
	*/
	measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
	start_pos_measure = 0;
	visible_end_pos = x + width;

	end_pos_measure = start_pos_measure + m_zoom_value * measure_width;


/*
	while (end_pos_measure < visible_start_pos) {
		current_time += numerator * WHOLE_NOTE / denominator;
		getNumDenom(current_time, &numerator, &denominator);
		start_pos_measure = end_pos_measure;
		measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
		end_pos_measure = start_pos_measure + m_zoom_value * measure_width;
	}
	*/

	current_time = (unsigned long long) ((visible_start_pos * (double) FACTOR) / m_zoom_value);

	for (lptr = g_list_last(m_specials); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		measure_width = ((NedIdTimesigChange *) lptr->data)->m_numerator * WHOLE_NOTE / FACTOR / ((NedIdTimesigChange *) lptr->data)->m_denominator;
		if (((NedIdSpecial *) lptr->data)->m_time + measure_width * FACTOR <= current_time) break;
	}

	if (lptr != NULL) {
		time_dist = current_time - (((NedIdSpecial *) lptr->data)->m_time + measure_width * FACTOR);
		numerator = ((NedIdTimesigChange *) lptr->data)->m_numerator;
		denominator = ((NedIdTimesigChange *) lptr->data)->m_denominator;
		current_time = ((NedIdSpecial *) lptr->data)->m_time;
		measure_dist_d = (double) time_dist / (double) (measure_width * FACTOR);
		measure_dist = (int) measure_dist_d;
		unsigned long long  add = (unsigned long long) measure_dist * (unsigned long long) measure_width * (unsigned long long) FACTOR;
		current_time += add;
		//printf("current_time = %llu(%llu), add = %llu(%llu)\n",  current_time, current_time / NOTE_8, add, add / NOTE_8); fflush(stdout);
		//current_time += measure_dist * measure_width * FACTOR;
		start_pos_measure = current_time / FACTOR  * m_zoom_value;
	}
	else {
		current_time = 0;
		measure_width = WHOLE_NOTE / FACTOR;
		start_pos_measure = 0;
		numerator = denominator = 4;
	}

	end_pos_measure = start_pos_measure + m_zoom_value * measure_width;


	while (start_pos_measure < visible_end_pos) {
		nominator_dist = measure_width /  numerator;
		current_dist = nominator_dist / (64 / denominator);
		cairo_set_line_width(cr, NOTE_LINE_THICK);
		curr_dnom = 64;
		current_color = 6;
		cairo_stroke(cr);
		while (curr_dnom > denominator) {
			if (current_dist * m_zoom_value > MIN_ZOOM_DIST) {
				cairo_new_path(cr);
				cairo_set_source_rgb (cr, color_array[current_color][0], color_array[current_color][1],color_array[current_color][2]);
				i = 0;
				do {
					d = PIANO_PART_WIDTH + start_pos_measure + ((double) i * (double) current_dist) * m_zoom_value - x;
					cairo_move_to(cr, d, SPEC_TRACKS_HEIGHT);
					cairo_line_to(cr, d, height - SPEC_TRACKS_HEIGHT);
					cairo_stroke(cr);
					i++;
				}
				while (d < end_pos_measure + PIANO_PART_WIDTH);
				cairo_stroke(cr);
			}
			current_color--;
			curr_dnom /= 2;
			current_dist *= 2;
		}
		current_color = 1;
		cairo_new_path(cr);
		cairo_set_line_width(cr, NOTE_LINE_THICK);
		cairo_set_source_rgb (cr, color_array[current_color][0], color_array[current_color][1],color_array[current_color][2]);
		i = 0;
		do {
			d = PIANO_PART_WIDTH + start_pos_measure + ((double) i * (double) nominator_dist) * m_zoom_value - x;
			cairo_move_to(cr, d, SPEC_TRACKS_HEIGHT);
			cairo_line_to(cr, d, height - SPEC_TRACKS_HEIGHT);
			cairo_stroke(cr);
			i++;
		}
		while (d < end_pos_measure + PIANO_PART_WIDTH);
	
		current_color = 0;
		cairo_new_path(cr);
		cairo_set_line_width(cr, BAR_LINE_THICK);
		cairo_set_source_rgb (cr, color_array[current_color][0], color_array[current_color][1],color_array[current_color][2]);
		d = PIANO_PART_WIDTH + start_pos_measure - x;
		cairo_move_to(cr, d, SPEC_TRACKS_HEIGHT);
		cairo_line_to(cr, d, height - SPEC_TRACKS_HEIGHT);
		cairo_stroke(cr);

		current_time += numerator * WHOLE_NOTE / denominator;
		getNumDenom(current_time, &numerator, &denominator);
		start_pos_measure = end_pos_measure;
		measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
		end_pos_measure = start_pos_measure + m_zoom_value * measure_width;
	}
	lptr = g_list_nth(m_instruments, m_current_instrument_number);
	((NedIdiotsInstrument *) lptr->data)->draw(cr, m_zoom_value, x, y, visible_start_pos, visible_end_pos);

	draw_specials(cr, m_zoom_value, x, y, width, visible_start_pos, visible_end_pos);
	((NedIdiotsInstrument *) lptr->data)->draw_specials(cr, m_zoom_value, x, y, visible_start_pos, visible_end_pos);
	draw_piano(cr, y, height);

	cairo_destroy (cr);
}
#endif


void NedIdiotsEditor::draw_new () {
	int i;
	double xleft, ytop;
	int width, height;
	unsigned int measure_width;
	unsigned long long current_time, time_dist;
	double d, measure_dist_d;
	int measure_dist;
	double start_pos_measure, end_pos_measure;
	double visible_start_pos, visible_end_pos;
	unsigned int  current_dist, nominator_dist;
	int current_color;
	unsigned int curr_dnom;
	GList *lptr;
	unsigned int numerator = 4;
	unsigned int denominator = 4;

#define MIN_ZOOM_DIST 8.0

	if (m_dpy == NULL) initX11();


	xleft = gtk_adjustment_get_value(m_hadjust);
	ytop = gtk_adjustment_get_value(m_vadjust);
	width = m_drawing_area->allocation.width;
	height = m_drawing_area->allocation.height;


#define BAR_COLOR 0, 0, 0
#define NOMINATOR_COLOR 0, 0, 204
#define HALF_COLOR 0.0, 26, 153
#define QUARTER_COLOR  0, 51, 127
#define EIGHTS_COLOR 127, 77, 102
#define COLOR_16 0, 102, 77
#define COLOR32  127, 127, 51
#define COLOR64 230, 153,25 

	static unsigned int color_array[8][3] = {
		{BAR_COLOR},
		{NOMINATOR_COLOR},
		{HALF_COLOR},
		{QUARTER_COLOR},
		{EIGHTS_COLOR},
		{COLOR_16},
		{COLOR32},
		{COLOR64}};





	XSetForeground(m_dpy, m_xgc, 0xeeeeee);
	XFillRectangle(m_dpy, m_back_image, m_xgc, 0, 0, width, height);
	XSetForeground(m_dpy, m_xgc, 0x0);
	XSetLineAttributes(m_dpy, m_xgc, SEP_LINE_THICK, LineSolid, CapRound, JoinMiter);
	for (i = 4; i < 4 + 117; i++) {
		if (strlen(pitch_tab[i-4]) > 1) {
			XSetForeground(m_dpy, m_xgc, 0x999999);
			XFillRectangle(m_dpy, m_back_image, m_xgc, PIANO_PART_WIDTH, (i - 4) * SEP_LINE_DIST - ytop - SPEC_TRACKS_HEIGHT, width - PIANO_PART_WIDTH, SEP_LINE_DIST);
		}
		XSetForeground(m_dpy, m_xgc, 0x0);
		if (ytop < (i - 3) * SEP_LINE_DIST  && ytop + height > (i - 4) * SEP_LINE_DIST) {
			XDrawLine(m_dpy, m_back_image, m_xgc, PIANO_PART_WIDTH, (i - 4) * SEP_LINE_DIST - ytop - SPEC_TRACKS_HEIGHT,
				width, (i - 4) * SEP_LINE_DIST - ytop - SPEC_TRACKS_HEIGHT);

		}
	}

	visible_start_pos = xleft;
	/*
	current_time = 0;
	getNumDenom(current_time, &numerator, &denominator);
	*/
	measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
	start_pos_measure = 0;
	visible_end_pos = xleft + width;

	end_pos_measure = start_pos_measure + m_zoom_value * measure_width;


/*
	while (end_pos_measure < visible_start_pos) {
		current_time += numerator * WHOLE_NOTE / denominator;
		getNumDenom(current_time, &numerator, &denominator);
		start_pos_measure = end_pos_measure;
		measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
		end_pos_measure = start_pos_measure + m_zoom_value * measure_width;
	}
	*/

	current_time = (unsigned long long) ((visible_start_pos * (double) FACTOR) / m_zoom_value);

	for (lptr = g_list_last(m_specials); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		measure_width = ((NedIdTimesigChange *) lptr->data)->m_numerator * WHOLE_NOTE / FACTOR / ((NedIdTimesigChange *) lptr->data)->m_denominator;
		if (((NedIdSpecial *) lptr->data)->m_time + measure_width * FACTOR <= current_time) break;
	}

	if (lptr != NULL) {
		time_dist = current_time - (((NedIdSpecial *) lptr->data)->m_time + measure_width * FACTOR);
		numerator = ((NedIdTimesigChange *) lptr->data)->m_numerator;
		denominator = ((NedIdTimesigChange *) lptr->data)->m_denominator;
		current_time = ((NedIdSpecial *) lptr->data)->m_time;
		measure_dist_d = (double) time_dist / (double) (measure_width * FACTOR);
		measure_dist = (int) measure_dist_d;
		unsigned long long  add = (unsigned long long) measure_dist * (unsigned long long) measure_width * (unsigned long long) FACTOR;
		current_time += add;
		//printf("current_time = %llu(%llu), add = %llu(%llu)\n",  current_time, current_time / NOTE_8, add, add / NOTE_8); fflush(stdout);
		//current_time += measure_dist * measure_width * FACTOR;
		start_pos_measure = current_time / FACTOR  * m_zoom_value;
	}
	else {
		current_time = 0;
		measure_width = WHOLE_NOTE / FACTOR;
		start_pos_measure = 0;
		numerator = denominator = 4;
	}

	end_pos_measure = start_pos_measure + m_zoom_value * measure_width;
	m_min_denom = 1;


	while (start_pos_measure < visible_end_pos) {
		nominator_dist = measure_width /  numerator;
		current_dist = nominator_dist / (64 / denominator);
		XSetLineAttributes(m_dpy, m_xgc, SEP_LINE_THICK, LineSolid, CapRound, JoinMiter);
		curr_dnom = 64;
		current_color = 6;
		while (curr_dnom > denominator) {
			if (current_dist * m_zoom_value > MIN_VERTICAL_LINES_DIST) {
				if (m_min_denom == 1) {
					m_min_denom = curr_dnom;
				}
				XSetForeground(m_dpy, m_xgc, (color_array[current_color][0] << 16 ) | (color_array[current_color][1] << 8) | color_array[current_color][2]);
				i = 0;
				do {
					d = PIANO_PART_WIDTH + start_pos_measure + ((double) i * (double) current_dist) * m_zoom_value - xleft;
					XDrawLine(m_dpy, m_back_image, m_xgc, d, SPEC_TRACKS_HEIGHT, d, height - SPEC_TRACKS_HEIGHT);
					i++;
				}
				while (d < end_pos_measure + PIANO_PART_WIDTH);
			}
			current_color--;
			curr_dnom /= 2;
			current_dist *= 2;
		}
		current_color = 1;
		XSetLineAttributes(m_dpy, m_xgc, NOTE_LINE_THICK, LineSolid, CapRound, JoinMiter);
		XSetForeground(m_dpy, m_xgc, (color_array[current_color][0] << 16 ) | (color_array[current_color][1] << 8) | color_array[current_color][2]);
		i = 0;
		do {
			d = PIANO_PART_WIDTH + start_pos_measure + ((double) i * (double) nominator_dist) * m_zoom_value - xleft;
			XDrawLine(m_dpy, m_back_image, m_xgc, d, SPEC_TRACKS_HEIGHT, d, height - SPEC_TRACKS_HEIGHT);
			i++;
		}
		while (d < end_pos_measure + PIANO_PART_WIDTH);
	
		current_color = 0;
		XSetLineAttributes(m_dpy, m_xgc, BAR_LINE_THICK, LineSolid, CapRound, JoinMiter);
		XSetForeground(m_dpy, m_xgc, (color_array[current_color][0] << 16 ) | (color_array[current_color][1] << 8) | color_array[current_color][2]);
		d = PIANO_PART_WIDTH + start_pos_measure - xleft;
		XDrawLine(m_dpy, m_back_image, m_xgc, d, SPEC_TRACKS_HEIGHT, d, height - SPEC_TRACKS_HEIGHT);

		current_time += numerator * WHOLE_NOTE / denominator;
		getNumDenom(current_time, &numerator, &denominator);
		start_pos_measure = end_pos_measure;
		measure_width = numerator * WHOLE_NOTE / FACTOR / denominator;
		end_pos_measure = start_pos_measure + m_zoom_value * measure_width;
	}
	lptr = g_list_nth(m_instruments, m_current_instrument_number);
	((NedIdiotsInstrument *) lptr->data)->draw(m_dpy, m_back_image, m_xgc, m_x_border_gc, m_zoom_value, xleft, ytop, visible_start_pos, visible_end_pos);

	XSetForeground(m_dpy, m_xgc, 0xffffff);
	XFillRectangle(m_dpy, m_back_image, m_xgc, 0, 0, PIANO_PART_WIDTH, height);

	XSetForeground(m_dpy, m_xgc, 0x0);
	for (i = 4; i < 4 + 117; i++) {
		/*
		if (strlen(pitch_tab[i-4]) > 1) {
			XSetForeground(m_dpy, m_xgc, 0x999999);
			XFillRectangle(m_dpy, m_back_image, m_xgc, PIANO_PART_WIDTH, (i - 4) * SEP_LINE_DIST - ytop - SPEC_TRACKS_HEIGHT, width - PIANO_PART_WIDTH, SEP_LINE_DIST);
		}
*/
		if (ytop < (i - 3) * SEP_LINE_DIST  && ytop + height > (i - 4) * SEP_LINE_DIST) {
			 XDrawString(m_dpy, m_back_image, m_xgc, TUNE_NAME_X_START, (i - 4) * SEP_LINE_DIST + SEP_LINE_DIST / 2 - ytop - SPEC_TRACKS_HEIGHT,
			   "ABC", strlen("ABC"));

			
			


		}
	}
	XSetForeground(m_dpy, m_xgc, 0xffffff);
	XFillRectangle(m_dpy, m_back_image, m_xgc, 0, 0, width, SPEC_TRACKS_HEIGHT);
	XFillRectangle(m_dpy, m_back_image, m_xgc, 0, height - SPEC_TRACKS_HEIGHT, width, height);

#ifdef XXX
	draw_specials(cr, m_zoom_value, xleft, ytop, width, visible_start_pos, visible_end_pos);
	((NedIdiotsInstrument *) lptr->data)->draw_specials(cr, m_zoom_value, xleft, ytop, visible_start_pos, visible_end_pos);
	draw_piano(cr, ytop, height);
#endif
	XCopyArea(m_dpy, m_back_image, m_win, m_xgc, 0, 0, width, height, 0, 0);
	XFlush(m_dpy);

}

void NedIdiotsEditor::do_undo(GtkWidget  *widget, void *data) {
	//if (NedResource::isPlaying()) return;
	//NedResource::m_avoid_immadiate_play = TRUE;
	NedIdiotsEditor *id_editor = (NedIdiotsEditor *) data;
	//id_editor->m_selected_note = NULL;
	id_editor->m_command_history->unexecute();
	id_editor->repaint();
	//NedResource::m_avoid_immadiate_play = FALSE;
}

void NedIdiotsEditor::do_redo(GtkWidget  *widget, void *data) {
	//if (NedResource::isPlaying()) return;
	//NedResource::m_avoid_immadiate_play = TRUE;
	NedIdiotsEditor *id_editor = (NedIdiotsEditor *) data;
	//id_editor->m_selected_note = NULL;
	id_editor->m_command_history->execute();
	id_editor->repaint();
	//NedResource::m_avoid_immadiate_play = FALSE;
}



unsigned long long NedIdiotsEditor::getFirstSplitTime(unsigned long long start_time, unsigned long long end_time) {
	GList *lptr;
	unsigned long long time_dist, current_time;
	int numerator, denominator, measure_width, measure_dist;
 
	for (lptr = g_list_last(m_specials); lptr; lptr = g_list_previous(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		if (((NedIdSpecial *) lptr->data)->m_time <= start_time) break;
	}

	if (lptr != NULL) {
		time_dist = start_time - ((NedIdSpecial *) lptr->data)->m_time;
		numerator = ((NedIdTimesigChange *) lptr->data)->m_numerator;
		denominator = ((NedIdTimesigChange *) lptr->data)->m_denominator;
		measure_width = numerator * WHOLE_NOTE / denominator;
		current_time = ((NedIdSpecial *) lptr->data)->m_time;
		measure_dist = (time_dist / measure_width) + 1;
		current_time += (unsigned long long) measure_dist * (unsigned long long) measure_width;
	}
	else {
		time_dist = start_time;
		current_time = 0;
		measure_width = WHOLE_NOTE;
		numerator = denominator = 4;
		measure_dist = (time_dist / measure_width) + 1;
		current_time += (unsigned long long) measure_dist * (unsigned long long) measure_width;
	}
	if (current_time > start_time && current_time < end_time) return current_time;
	return 0;
}

void NedIdiotsEditor::draw_specials (cairo_t *cr, double zoom_value, double xleft, double ytop, int width, double visible_start_pos, double visible_end_pos) {
	GList *lptr;
	NedIdSpecial *spec;
	char Str[128];

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.9, 0.9, 0.0);
	cairo_rectangle (cr, PIANO_PART_WIDTH,  0, width - PIANO_PART_WIDTH, TIME_SIG_TRACK_HEIGHT);
	cairo_fill(cr);

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.8, 0.8, 0.4);
	cairo_rectangle (cr, PIANO_PART_WIDTH,  TIME_SIG_TRACK_HEIGHT, width - PIANO_PART_WIDTH, TEMPO_TRACK_HEIGHT);
	cairo_fill(cr);

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.9, 0.9, 0.2);
	cairo_rectangle (cr, PIANO_PART_WIDTH,  TIME_SIG_TRACK_HEIGHT + SIGN_TRACK_HEIGHT, width - PIANO_PART_WIDTH, SIGN_TRACK_HEIGHT);
	cairo_fill(cr);

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.4, 0.9, 0.4);
	cairo_rectangle (cr, PIANO_PART_WIDTH,  TIME_SIG_TRACK_HEIGHT + SIGN_TRACK_HEIGHT + VOLUME_TRACK_HEIGHT, width - PIANO_PART_WIDTH, VOLUME_TRACK_HEIGHT);
	cairo_fill(cr);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		spec = (NedIdSpecial *) lptr->data;
		if (spec->m_time / FACTOR * zoom_value > visible_end_pos) break;
		if (spec->m_time / FACTOR * zoom_value < visible_start_pos) continue;
		switch (spec->m_type) {
			case ID_SPEC_TIME_SIG_CHANGE: sprintf(Str, "%d/%d", ((NedIdTimesigChange *) spec)->m_numerator,
				((NedIdTimesigChange *) spec)->m_denominator);
				cairo_new_path(cr);
				cairo_move_to(cr, PIANO_PART_WIDTH + spec->m_time / FACTOR * zoom_value - xleft, TIME_SIG_TRACK_HEIGHT);
				cairo_show_text(cr, Str);
				cairo_stroke(cr);
				break;
			case ID_SPEC_TEMPO_CHANGE: sprintf(Str, "%d", ((NedIdTempoChange *) spec)->m_tempo);
				cairo_new_path(cr);
				cairo_move_to(cr, PIANO_PART_WIDTH + spec->m_time / FACTOR * zoom_value - xleft, TIME_SIG_TRACK_HEIGHT + TEMPO_TRACK_HEIGHT);
				cairo_show_text(cr, Str);
				cairo_stroke(cr);
				break;
			case ID_SPEC_KEYSIG_CHANGE: /* TODO: draw */break;
			default: NedResource::Abort("draw_specials");
		}
	}

}

void NedIdiotsEditor::draw_piano (cairo_t *cr, double ytop, int height) {
	int i;

#define T_X_OFFS 2.0


	//cr = gdk_cairo_create (m_drawing_area->window);
	cairo_set_source_rgb (cr, 1.0, 2.0, 1.0);
	cairo_rectangle (cr,  0,  0, PIANO_PART_WIDTH, m_drawing_area->allocation.height);
	cairo_fill(cr);
	cairo_select_font_face (cr, "SANS", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 8.0);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	for (i = 4; i < 4 + 117; i++) {
		if (ytop < (116 - (i - 4)) * SEP_LINE_DIST  && ytop + height > (116 - (i - 5)) * SEP_LINE_DIST) {
			if (strlen(pitch_tab[i-4]) > 1) {
				cairo_new_path(cr);
				cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
				cairo_rectangle (cr, 0, (116 - (i - 4)) * SEP_LINE_DIST - ytop - TIME_SIG_TRACK_HEIGHT, PIANO_PART_WIDTH, SEP_LINE_DIST);
				cairo_fill(cr);
			}
		}
		cairo_new_path(cr);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		if (ytop < (116 - (i - 4)) * SEP_LINE_DIST  && ytop + height > (116 - (i - 5)) * SEP_LINE_DIST) {
			cairo_move_to(cr, T_X_OFFS, (116 - (i - 4)) * SEP_LINE_DIST - ytop + SEP_LINE_DIST / 1.2 - TIME_SIG_TRACK_HEIGHT);
			cairo_show_text(cr, pitch_tab[i-4]);
		}
		cairo_stroke(cr);
	}
}

void NedIdiotsEditor::zoom_in(GtkWidget  *widget, void *data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	id_editor->m_zoom_value *= ID_ZOOM_STEP;
	if (id_editor->m_zoom_value > ID_MAX_ZOOM) {
		id_editor->m_zoom_value = ID_MAX_ZOOM;
	}
	id_editor->setHadjust();
	id_editor->repaint();
}

void NedIdiotsEditor::zoom_out(GtkWidget  *widget, void *data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	id_editor->m_zoom_value /= ID_ZOOM_STEP;
	if (id_editor->m_zoom_value < ID_MIN_ZOOM) {
		id_editor->m_zoom_value = ID_MIN_ZOOM;
	}
	id_editor->setHadjust();
	id_editor->repaint();
}

void NedIdiotsEditor::size_change_handler(GtkWidget *widget, GtkRequisition *requisition, gpointer data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;

	if (id_editor->m_dpy == NULL) return;

	XFreePixmap(id_editor->m_dpy, id_editor->m_back_image);

	id_editor->m_back_image = XCreatePixmap(id_editor->m_dpy, id_editor->m_win,
			 id_editor->m_drawing_area->allocation.width, id_editor->m_drawing_area->allocation.height,
				DefaultDepth(id_editor->m_dpy, id_editor->m_screen));

	id_editor->setHadjust();
	id_editor->setVadjust();
}

void NedIdiotsEditor::setInstrumentName(char *name) {
	name[MAX_INST_NAME - 1] = '\0';
	strcpy(m_pending_inst_name, name);
}

int NedIdiotsEditor::getPartCount() {
	return g_list_length(m_instruments);
}

char *NedIdiotsEditor::getInstrumentName(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getInstrumentName");
	}
	return ((NedIdiotsInstrument *) lptr->data)->getInstrumentName();
}

int NedIdiotsEditor::getNumerator() {
	GList *lptr;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time > 0) break;
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		return ((NedIdTimesigChange *) lptr->data)->m_numerator;
	}

	return 4;
}

int NedIdiotsEditor::getDenominator() {
	GList *lptr;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time > 0) break;
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		return ((NedIdTimesigChange *) lptr->data)->m_denominator;
	}
	return 4;
}

void NedIdiotsEditor::computeTimeSigMeasureNumbers(NedMainWindow *main_window) {
	unsigned int num = 0;
	unsigned long long last_timesig_time = 0;
	int dummy;
	unsigned int dummy2;
	GList *lptr;
	unsigned int duration = WHOLE_NOTE;
	NedIdTimesigChange *timesig;

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TIME_SIG_CHANGE) continue;
		timesig = (NedIdTimesigChange *) lptr->data;
		num = timesig->m_measure_number = num + (timesig->m_time - last_timesig_time) / duration;
		if (timesig->m_time > 0) {
			main_window->setSpecialTimesig(num + 1, timesig->m_numerator, timesig->m_denominator, TIME_SYMBOL_NONE, &dummy, &dummy, &dummy2, false);
		}
		duration = timesig->m_numerator * WHOLE_NOTE / timesig->m_denominator;
		last_timesig_time = timesig->m_time;
	}
}

void NedIdiotsEditor::trySplitInstruments() {
	GList *lptr, *lptr2;
	GList *new_instruments;
	int pos;

	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		new_instruments = ((NedIdiotsInstrument *) lptr->data)->trySplitInstrument(m_force_piano, m_dont_split);
		pos = g_list_position(m_instruments, lptr);
		for (lptr2 = g_list_first(new_instruments); lptr2; lptr2 = g_list_next(lptr2)) {
			m_instruments = g_list_insert(m_instruments, lptr2->data, ++pos);
			if ((lptr = g_list_find(m_instruments, lptr2->data)) == NULL) {
				NedResource::Abort("NedIdiotsEditor::trySplitInstruments");
			}
		}
		g_list_free(new_instruments);
	}
}

void NedIdiotsEditor::computeVolumes() {
	GList *lptr;

	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		((NedIdiotsInstrument *) lptr->data)->computeVolume();
	}
}

void NedIdiotsEditor::snapStartOfNotes() {
	GList *lptr;

	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		((NedIdiotsInstrument *) lptr->data)->snapStartOfNotes(this);
	}
}

void NedIdiotsEditor::recognizeTriplets() {
	GList *lptr;

	for (lptr = g_list_first(m_instruments); lptr; lptr = g_list_next(lptr)) {
		((NedIdiotsInstrument *) lptr->data)->recognizeTriplets(this);
	}
}

void NedIdiotsEditor::computeTempo() {

	GList *lptr;
	GList *del_list_ptr = NULL, *del_list_specs = NULL;

	thin_out_temposigs();

	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_time > 0) break;
		if (((NedIdSpecial *) lptr->data)->m_type != ID_SPEC_TEMPO_CHANGE) continue;
		m_tempo_inverse = 60000.0 / (double) ((NedIdTempoChange *) lptr->data)->m_tempo;
		del_list_ptr = g_list_append(del_list_ptr, lptr);
		del_list_specs = g_list_append(del_list_specs, lptr->data);
	}

	for (lptr = g_list_first(del_list_ptr); lptr; lptr = g_list_next(lptr)) {
		m_specials = g_list_delete_link(m_specials, (GList *) lptr->data);
	}
	for (lptr = g_list_first(del_list_specs); lptr; lptr = g_list_next(lptr)) {
		delete (NedIdTempoChange *) lptr->data;
	}
	g_list_free(del_list_ptr);
	g_list_free(del_list_specs);
}

GList *NedIdiotsEditor::convertToNtEdNotes(int staff_nr, int voice_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::convertToNtEdNotes");
	}
	return ((NedIdiotsInstrument *) lptr->data)->convertToNtEdNotes(this, /* staff_nr == 0 && */ voice_nr == 0, voice_nr == 0, voice_nr, m_last_time);
}

int NedIdiotsEditor::getClef(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getClef");
	}
	return ((NedIdiotsInstrument *) lptr->data)->getClef();
}

int NedIdiotsEditor::getOctaveShift(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getOctaveShift");
	}
	return ((NedIdiotsInstrument *) lptr->data)->getOctaveShift();
}

int NedIdiotsEditor::getMidiPgm(int staff_nr) {
	GList *lptr;
	int midi_pgm;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getMidiPgm");
	}
	midi_pgm = ((NedIdiotsInstrument *) lptr->data)->getMidiNr();
	if (midi_pgm < 0) {
		midi_pgm = 0;
	}
	return midi_pgm;
}

int NedIdiotsEditor::getChannel(int staff_nr) {
	GList *lptr;
	int midi_pgm;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getMidiPgm");
	}
	midi_pgm = ((NedIdiotsInstrument *) lptr->data)->getMidiNr();
	if (midi_pgm < 0) {
		return 9;
	}
	return ((NedIdiotsInstrument *) lptr->data)->getChannel();
}

int NedIdiotsEditor::getVolume(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::getVolume");
	}
	return ((NedIdiotsInstrument *) lptr->data)->getVolume();
}

int NedIdiotsEditor::getKeySig(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
	}

	if (((NedIdiotsInstrument *) lptr->data)->getChannel() == 9) return 0;


	for (lptr = g_list_first(m_specials); lptr; lptr = g_list_next(lptr)) {
		if (((NedIdSpecial *) lptr->data)->m_type == ID_SPEC_KEYSIG_CHANGE) {
			if (((NedIdSpecial *) lptr->data)->m_time > 0) return 0;
			return ((NedIdKeysigChange *) lptr->data)->m_key;
		}
	}
	return 0;
}

void NedIdiotsEditor::changeChannelAndPgm(int staff_nr, int chan, int pgm) {
	GList *lptr;

	if ((lptr = g_list_nth(m_instruments, staff_nr)) == NULL) {
		NedResource::Abort("NedIdiotsEditor::changeChannelAndPgm");
	}
	((NedIdiotsInstrument *) lptr->data)->setChannel(chan);
	((NedIdiotsInstrument *) lptr->data)->setMidiNr(pgm);
}

void NedIdiotsEditor::select_instrument(GtkButton *button, gpointer data) {
	NedIdiotsEditor *id_editor = (NedIdiotsEditor*) data;
	GSList *l, *ll;
	int i;

	l = gtk_radio_button_get_group(GTK_RADIO_BUTTON(id_editor->m_instr_bu));
	ll = gtk_radio_button_get_group(GTK_RADIO_BUTTON(id_editor->m_instr_bu));
	for (; ll ; ll = g_slist_next(ll)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ll->data))) {
			i = g_slist_position(l, ll);
			if (i  > 0) {
				i = g_slist_length(l) - i;
			}
			id_editor->m_current_instrument_number = i;
			id_editor->repaint();
			return;
		}
	}
}
