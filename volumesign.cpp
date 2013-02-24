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
#include "volumesign.h"
#include "chordorrest.h"
#include "volumedialog.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedVolumeSign::NedVolumeSign(int kind) : NedFreeReplaceable(), m_kind(kind) {setMidiVolume();}
NedVolumeSign::NedVolumeSign(int kind, NedChordOrRest *element) : NedFreeReplaceable(element), m_kind(kind)  {setMidiVolume();}
NedVolumeSign::NedVolumeSign(int kind, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_kind(kind) {setMidiVolume();}

void NedVolumeSign::setMidiVolume() {
	switch (m_kind) {
		case VOL_PPP: m_midi_volume = 18; break;
		case VOL_PP: m_midi_volume = 36; break;
		case VOL_P: m_midi_volume = 54; break;
		case VOL_MP: 
		case VOL_SP: 
		case VOL_MF: 
		case VOL_SF: m_midi_volume = 72; break;
		case VOL_F: m_midi_volume = 91; break;
		case VOL_FF: m_midi_volume = 108; break;
		case VOL_FFF: m_midi_volume = 127; break;
	}
}

int NedVolumeSign::determineKind(int volume) {
	if (volume <= 18) return VOL_PPP;
	if (volume <= 36) return VOL_PP;
	if (volume <= 54) return VOL_P;
	if (volume <= 72) return VOL_MF;
	if (volume <= 91) return VOL_F;
	if (volume <= 108) return VOL_FF;
	return VOL_FFF;
}
	

NedFreeReplaceable *NedVolumeSign::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */, 
	NedMainWindow *main_window /* = NULL */) {
	NedVolumeSign *newVolSig = new NedVolumeSign(m_kind, newelement, m_x, m_y, true);
	return newVolSig;
}

void NedVolumeSign::adjust_pointers(struct addr_ref_str *slurlist) {
}

void NedVolumeSign::startContextDialog(GtkWidget *ref) {
	bool state;
	int newvolume;

	NedVolumeDialog *volume_dialog = new NedVolumeDialog(GTK_WINDOW(ref), m_midi_volume, false);

	volume_dialog->getValues(&state, &newvolume);
	delete volume_dialog;

	if (state) {
		m_midi_volume = newvolume;
	}
}

void NedVolumeSign::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;

	double xpos, ypos;

	getXYPos(&xpos, &ypos);
	cairo_glyph_t glyph[3];
	int len = 0;
	double yoffs = 0.0;
#define X_SPACE 0.2
#define FORTE_Y_OFFS 0.05
	
	cairo_new_path(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}

	switch(m_kind) {
		case VOL_P:
			glyph[0].index = BASE + 40; len = 1; break;
		case VOL_PP:
			glyph[0].index = glyph[1].index = BASE + 40; len = 2; break;
		case VOL_PPP:
			glyph[0].index = glyph[1].index = glyph[2].index = BASE + 40; len = 3; break;
		case VOL_MF:
			glyph[0].index = BASE + 39; glyph[1].index = BASE + 37; len = 2; yoffs = -FORTE_Y_OFFS; break;
		case VOL_SF:
			glyph[0].index = BASE + 38; glyph[1].index = BASE + 37; len = 2; yoffs = -FORTE_Y_OFFS; break;
		case VOL_MP:
			glyph[0].index = BASE + 39; glyph[1].index = BASE + 40; len = 2; break;
		case VOL_SP:
			glyph[0].index = BASE + 38; glyph[1].index = BASE + 40; len = 2; break;
		case VOL_F:
			glyph[0].index = BASE + 37; len = 1; break;
		case VOL_FF:
			glyph[0].index = glyph[1].index = BASE + 37; len = 2; break;
		case VOL_FFF:
			glyph[0].index = glyph[1].index = glyph[2].index = BASE + 37; len = 3; break;
	}
	glyph[0].y = Y_POS_PAGE_REL(ypos + yoffs); glyph[1].y = glyph[2].y = Y_POS_PAGE_REL(ypos);

	for (int i = 0; i < len; i++) {
		glyph[i].x = X_POS_PAGE_REL(xpos - (len / 2 - i) * X_SPACE);
	}
	cairo_show_glyphs(cr, glyph, len);
	/*
	cairo_move_to(cr, X_POS_PAGE_REL(xpos - 1.0), Y_POS_PAGE_REL(ypos - 1.0)) ;
	cairo_line_to(cr, X_POS_PAGE_REL(xpos + 1.0), Y_POS_PAGE_REL(ypos + 1.0)) ;
	cairo_move_to(cr, X_POS_PAGE_REL(xpos + 1.0), Y_POS_PAGE_REL(ypos - 1.0)) ;
	cairo_line_to(cr, X_POS_PAGE_REL(xpos - 1.0), Y_POS_PAGE_REL(ypos + 1.0)) ;
	*/
	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	cairo_stroke(cr);
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}

