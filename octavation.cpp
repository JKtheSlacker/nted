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

#include "octavation.h"
#include "linepoint.h"
#include "page.h"
#include "staff.h"
#include "chordorrest.h"
#include "system.h"
#include "volumedialog.h"
#include <math.h>

#define X_POS_PAGE_REL(sp, p) ((sp->m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define X_PAGE_POS_PAGE_REL(page, p) (((page)->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

static double dash_array[2] = {1.0, 3.0};


NedOctavation::NedOctavation(NedLinePoint *start, NedLinePoint *end, int sub_type) :
	NedLine(start, end), m_octave_dist(1) {
	switch(sub_type) {
		case LINE_OCTAVATION_1: m_octave_dist = -1; break;
		case LINE_OCTAVATION2: m_octave_dist = 2; break;
		case LINE_OCTAVATION_2: m_octave_dist = -2; break;
	}

}

NedOctavation::NedOctavation(NedLinePoint *start, NedLinePoint *end, NedLinePoint *dummy, int octave_shift) :
	NedLine(start, end), m_octave_dist(1) {
	switch(octave_shift) {
		case -12: m_octave_dist = -1; break;
		case 24: m_octave_dist = 2; break;
		case -24: m_octave_dist = -2; break;
	}

}

#define OCTAVIATION_THICK 0.03
#define OCT_SIGN_LENGTH1 0.5
#define OCT_SIGN_LENGTH2 0.8

void NedOctavation::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) {
	double ypos1;
	NedStaff *staff;
	GList *lptr, *last;
	cairo_glyph_t glyph;
	double sign_length = OCT_SIGN_LENGTH1;
	if (!m_allow_drawing) return;

	cairo_new_path(cr);
	switch (m_octave_dist) {
		case 1: glyph.index = BASE + 41; break;
		case -1: glyph.index = BASE + 42; break;
		case 2: glyph.index = BASE + 43; 
			sign_length = OCT_SIGN_LENGTH2;
			 break;
		case -2: glyph.index = BASE + 44;
			sign_length = OCT_SIGN_LENGTH2;
			 break;
	}
	glyph.x = X_POS_PAGE_REL(m_line_start_point, m_start_x);
	glyph.y = Y_POS_PAGE_REL(m_start_y);
	cairo_show_glyphs(cr, &glyph, 1);
	cairo_new_path(cr);
	cairo_stroke(cr);
	cairo_set_line_width(cr, zoom_factor * OCTAVIATION_THICK);
	cairo_set_dash(cr, dash_array, 2, 0.0);

	if (m_distributed)  {
		if ((lptr = g_list_first(m_intermediate_staves)) == NULL) {
			NedResource::Abort("NedOctavation::draw: error 1");
		}
		last = g_list_last(m_intermediate_staves);
		staff = (NedStaff *) lptr->data;

		cairo_new_path(cr);
		cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_start_x), Y_POS_PAGE_REL(m_start_y));
		cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(m_start_y));
		cairo_stroke(cr);
		for (lptr = g_list_next(lptr); lptr != last; lptr = g_list_next(lptr)) {
			staff = (NedStaff *) lptr->data;
			ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
			cairo_new_path(cr);
			cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(ypos1));
			cairo_stroke(cr);
		}
		staff = (NedStaff *) lptr->data;
		ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
		cairo_new_path(cr);
		cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1));
		cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_end_x), Y_POS_PAGE_REL(ypos1));
		cairo_stroke(cr);
	}
	else {

		cairo_move_to(cr, X_POS_PAGE_REL(m_line_start_point, m_start_x + sign_length), Y_POS_PAGE_REL(m_start_y));
		cairo_line_to(cr, X_POS_PAGE_REL(m_line_end_point, m_end_x), Y_POS_PAGE_REL(m_start_y));
		cairo_stroke(cr);
	}
	cairo_set_dash(cr, dash_array, 0, 0.0);

}

int NedOctavation::getLineType() {
	switch (m_octave_dist) {
		case 1: return LINE_OCTAVATION1;
		case -1: return LINE_OCTAVATION_1;
		case 2: return LINE_OCTAVATION2;
	}

	return LINE_OCTAVATION_2;
}

int NedOctavation::getPitchOffset() {
	switch (m_octave_dist) {
		case 1: return 12;
		case -1: return -12;
		case 2: return 24;
	}
	return -24;
}
