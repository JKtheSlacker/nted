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

#include "freesign.h"
#include "chordorrest.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedFreeSign::NedFreeSign(int kind) : NedFreeReplaceable(), m_kind(kind) {}
NedFreeSign::NedFreeSign(int kind, NedChordOrRest *element) : NedFreeReplaceable(element), m_kind(kind)  {}
NedFreeSign::NedFreeSign(int kind, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_kind(kind) {}

NedFreeReplaceable *NedFreeSign::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */,
			NedMainWindow *main_window /* = NULL */) {
	NedFreeSign *newFreeSig = new NedFreeSign(m_kind, newelement, m_x, m_y, true);
	return newFreeSig;
}

void NedFreeSign::adjust_pointers(struct addr_ref_str *slurlist) {
}


void NedFreeSign::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;

	double xpos, ypos;

	getXYPos(&xpos, &ypos);
	cairo_glyph_t glyph[3];
	int len = 0;
	double yoffs = 0.0;
#define X_SIGN_SPACE 0.2
	
	cairo_new_path(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}

	switch(m_kind) {
		case SIGN_SEGNO:
			glyph[0].index = BASE + 48; len = 1; break;
		case SIGN_CODA:
			glyph[0].index = BASE + 47; len = 1; break;
	}
	glyph[0].y = Y_POS_PAGE_REL(ypos + yoffs); glyph[1].y = glyph[2].y = Y_POS_PAGE_REL(ypos);

	for (int i = 0; i < len; i++) {
		glyph[i].x = X_POS_PAGE_REL(xpos - (len / 2 - i) * X_SIGN_SPACE);
	}
	cairo_show_glyphs(cr, glyph, len);
	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	cairo_stroke(cr);
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}

void NedFreeSign::startContextDialog(GtkWidget *ref) {
}
