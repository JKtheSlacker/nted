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

#include <math.h>
#include "temposign.h"
#include "chordorrest.h"
#include "tempodialog.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedTempoSign::NedTempoSign(unsigned int kind, unsigned int tempo) : NedFreeReplaceable(), m_kind(kind), m_tempo(tempo), m_number_len(0), m_dot(0)  {computeGlypths();}
NedTempoSign::NedTempoSign(unsigned int kind, unsigned int tempo, NedChordOrRest *element) : NedFreeReplaceable(element), m_kind(kind), m_tempo(tempo), m_number_len(0), m_dot(0)   {computeGlypths();}
NedTempoSign::NedTempoSign(unsigned int kind, unsigned int tempo, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_kind(kind), m_tempo(tempo), m_number_len(0), m_dot(0)  {computeGlypths();}


NedFreeReplaceable *NedTempoSign::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */, NedMainWindow *main_window /* = NULL */) {
	NedTempoSign *newVolSig = new NedTempoSign(m_kind, m_tempo, newelement, m_x, m_y, true);
	return newVolSig;
}

void NedTempoSign::adjust_pointers(struct addr_ref_str *slurlist) {
}

void NedTempoSign::setTempo(unsigned int kind, unsigned int tempo) {
	m_kind = kind;
	m_tempo = tempo;
	computeGlypths();
}

double NedTempoSign::getTempoInverse() {
	if (m_dot) {
		return (double) NOTE_4 / (double) (3 * m_kind / 2) * 60000.0 / (double) m_tempo;
	}
	return (double) NOTE_4 / (double) m_kind * 60000.0 / (double) m_tempo;
}

void NedTempoSign::computeGlypths() {
	switch (m_kind) {
		case WHOLE_NOTE: m_dot = 0; break;
		case NOTE_2: m_dot = 0; break;
		case 3 * NOTE_2 / 2: m_dot = 1; m_kind = NOTE_2; break;
		case NOTE_4: m_dot = 0; break;
		case 3 * NOTE_4 / 2: m_dot = 1; m_kind = NOTE_4; break;
		case NOTE_8: m_dot = 0; break;
		case 3 * NOTE_8 / 2: m_dot = 1; m_kind = NOTE_8; break;
		case NOTE_16: m_dot = 0; break;
		case 3 * NOTE_16 / 2: m_dot = 1; m_kind = NOTE_16; break;
		default: NedResource::Abort("NedTempoSign::computeGlypths"); break;
	}
	if (m_tempo < 10) {
		m_number_len = 1;
		m_glyphs[0].index = m_tempo + NUMBERBASE;
		return;
	}
	if (m_tempo < 100) {
		m_number_len = 2;
		m_glyphs[0].index = (m_tempo / 10) + NUMBERBASE;
		m_glyphs[1].index = (m_tempo % 10) + NUMBERBASE;
		return;
	}
	m_number_len = 3;
	m_glyphs[0].index = (m_tempo / 100) + NUMBERBASE;
	m_glyphs[1].index = ((m_tempo % 100) / 10) + NUMBERBASE;
	m_glyphs[2].index = (m_tempo % 10) + NUMBERBASE;
}

void NedTempoSign::startContextDialog(GtkWidget *ref) {
	bool state;
	unsigned int newtempo, newkind;
	bool newdot;

	NedTempoDialog *tempo_dialog = new NedTempoDialog(GTK_WINDOW(ref), false, m_kind, m_dot, m_tempo);

	tempo_dialog->getValues(&state, &newkind, &newdot, &newtempo);
	delete tempo_dialog;

	if (state) {
		m_kind = newdot ? 3 * newkind / 2 : newkind;
		m_tempo = newtempo;
		computeGlypths();
	}
}

void NedTempoSign::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;
	double xpos, ypos;
	int i;
#define EXTRA_OFFS 0.15
	double extra_offs = 0.0;
	cairo_glyph_t glyph;
	cairo_matrix_t m;
	double extra_x_offs = 0.0;

	getXYPos(&xpos, &ypos);
	
	cairo_new_path(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}

	switch(m_kind) {
		case WHOLE_NOTE: glyph.index = BASE + 6; break;
		case NOTE_2: glyph.index = BASE + 7; break;
		case NOTE_4: glyph.index = BASE + 4; break;
		default: glyph.index = BASE + 4; 
			extra_offs = EXTRA_OFFS; break;
	}
	glyph.x = 0.0;
	glyph.y = 0.0;
	cairo_get_matrix(cr, &m);
#define X_SPACE 0.4
	cairo_translate(cr, X_POS_PAGE_REL(xpos - X_SPACE), Y_POS_PAGE_REL(ypos));
#define SCALE 0.6
	cairo_scale(cr, SCALE, SCALE);
	cairo_show_glyphs(cr, &glyph, 1);
#define EQU_XOFFS 0.3
#define EQU_LEN 0.3
#define QE_1_POS 0.0
#define QE_2_POS 0.1

#define NUMBER_DIST 0.55
#define DIGIT_DIST 0.3
#define NUMBER_Y_POS 0.2
#define TEMPO_DOT_DIST (1.3 * LINE_DIST)
#define TEMPO_DOT_RAD (0.2 * LINE_DIST)
#define TEMPO_DOT_EXTRA_X_OFFS (0.4 * LINE_DIST)
	if (m_dot) {
		extra_x_offs = TEMPO_DOT_EXTRA_X_OFFS;
		cairo_new_path(cr);
		cairo_arc(cr, zoom_factor * TEMPO_DOT_DIST, 0.0, zoom_factor * TEMPO_DOT_RAD, 0.9, 2.0 * M_PI);
		cairo_fill(cr);
	}
	glyph.index = BASE + 5;
	if (m_kind < NOTE_4) {
		glyph.x = zoom_factor * (extra_x_offs + HEAD_THICK_HALF - 0.15);
		glyph.y = -zoom_factor * 0.8 * STEM_DEFAULT_HEIGHT;
		cairo_show_glyphs(cr, &glyph, 1);
	}
	if (m_kind < NOTE_8) {
		glyph.x = zoom_factor * (extra_x_offs + HEAD_THICK_HALF - 0.15);
		glyph.y = -zoom_factor * 0.6 * STEM_DEFAULT_HEIGHT;
		cairo_show_glyphs(cr, &glyph, 1);
	}
	for (i = 0; i < 3;i++) {
		m_glyphs[i].x = zoom_factor * (extra_x_offs + NUMBER_DIST + extra_offs + i * DIGIT_DIST);
		m_glyphs[i].y = zoom_factor * NUMBER_Y_POS;
	}
	cairo_show_glyphs(cr, m_glyphs, m_number_len);
	cairo_stroke(cr);
	cairo_set_line_width(cr, zoom_factor * 1.4 * STEM_THICK);
	if (m_kind < WHOLE_NOTE) {
		cairo_new_path(cr);
		cairo_move_to(cr, zoom_factor * (HEAD_THICK_HALF - 0.15), 0.0);
		cairo_line_to(cr, zoom_factor * (HEAD_THICK_HALF - 0.15), - zoom_factor * 0.8 * STEM_DEFAULT_HEIGHT);
		cairo_stroke(cr);
	}
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * STEM_THICK);
	cairo_move_to(cr, zoom_factor * (extra_x_offs + EQU_XOFFS + EQU_LEN + extra_offs), - zoom_factor * QE_1_POS);
	cairo_line_to(cr, zoom_factor * (extra_x_offs + EQU_XOFFS + extra_offs), - zoom_factor * QE_1_POS);
	cairo_move_to(cr, zoom_factor * (extra_x_offs + EQU_XOFFS + EQU_LEN + extra_offs), - zoom_factor * QE_2_POS);
	cairo_line_to(cr, zoom_factor * (extra_x_offs + EQU_XOFFS + extra_offs), - zoom_factor * QE_2_POS);
	cairo_stroke(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	cairo_set_matrix(cr, &m);
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}
;
