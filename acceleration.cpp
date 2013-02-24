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

#include "acceleration.h"
#include "linepoint.h"
#include "page.h"
#include "staff.h"
#include "chordorrest.h"
#include "system.h"
#include "tempodialog.h"
#include <math.h>

#define X_POS_PAGE_REL(sp, p) ((sp->m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define X_PAGE_POS_PAGE_REL(page, p) (((page)->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedAcceleration::NedAcceleration(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end, bool ritardando) :
	NedLine3(start, mid, end), m_tempodiff(ritardando ? -20.0 : 20.0), m_ritardando(ritardando) {
	computeParams();
}

NedAcceleration::NedAcceleration(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end, bool ritardando, double tempodiff) :
	NedLine3(start, mid, end), m_tempodiff(tempodiff), m_ritardando(ritardando) {
	computeParams();
}


#define RITARDANDO_THICK 0.03

#define ACCELERANDO_LEN_2 0.6
#define ACCELERANDO_X_OFFS -0.5
#define ACCELERANDO_Y_OFFS 0.1
#define ACCELERANDO_SLANT CAIRO_FONT_SLANT_ITALIC
#define ACCELERANDO_FONT "Serif"
#define ACCELERANDO_WEIGHT CAIRO_FONT_WEIGHT_BOLD
#define ACCELERANDO_FONT_SIZE 0.4

void NedAcceleration::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) {
	double ypos1;
	NedStaff *staff;
	GList *lptr, *last;
	if (!m_allow_drawing) return;

	cairo_new_path(cr);
	cairo_select_font_face(cr, ACCELERANDO_FONT, ACCELERANDO_SLANT, ACCELERANDO_WEIGHT);
	cairo_set_font_size(cr, ACCELERANDO_FONT_SIZE * zoom_factor);
	cairo_move_to(cr, X_POS_PAGE_REL(m_line_mid_point, m_mid_x + ACCELERANDO_X_OFFS), Y_POS_PAGE_REL(m_mid_y + ACCELERANDO_Y_OFFS));
	if (m_ritardando) {
		cairo_show_text(cr, "ritard.");
	}
	else {
		cairo_show_text(cr, "accel.");
	}
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr, NedResource::getFontMatrix(zoom_level));
	cairo_stroke(cr);

	if (!isActive()) return;
	cairo_set_line_width(cr, zoom_factor * RITARDANDO_THICK);
	cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	if (m_distributed1)  {
		if ((lptr = g_list_first(m_intermediate_staves1)) == NULL) {
			NedResource::Abort("NedAcceleration::draw: error 1");
		}
		last = g_list_last(m_intermediate_staves1);
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
		cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_mid_x), Y_POS_PAGE_REL(ypos1));
		cairo_stroke(cr);
	}
	else {
		cairo_move_to(cr, X_POS_PAGE_REL(m_line_start_point, m_start_x), Y_POS_PAGE_REL(m_mid_y));
		cairo_line_to(cr, X_POS_PAGE_REL(m_line_mid_point, m_mid_x - ACCELERANDO_LEN_2), Y_POS_PAGE_REL(m_mid_y));
		cairo_stroke(cr);
	}
	if (m_distributed2)  {
		if ((lptr = g_list_first(m_intermediate_staves2)) == NULL) {
			NedResource::Abort("NedAcceleration::draw: error 2");
		}
		last = g_list_last(m_intermediate_staves2);
		staff = (NedStaff *) lptr->data;
		ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();

		cairo_new_path(cr);
		cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_mid_x), Y_POS_PAGE_REL(ypos1));
		cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(ypos1));
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
		cairo_move_to(cr, X_POS_PAGE_REL(m_line_mid_point, m_mid_x + ACCELERANDO_LEN_2), Y_POS_PAGE_REL(m_mid_y));
		cairo_line_to(cr, X_POS_PAGE_REL(m_line_end_point, m_end_x), Y_POS_PAGE_REL(m_mid_y));
		cairo_stroke(cr);
	}
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

}

void NedAcceleration::startContextDialog(GtkWidget *ref) {
	unsigned int newtempo_diff;
	unsigned int kind;
	bool dot;
	bool state;
	NedTempoDialog *tempodialog = new NedTempoDialog(GTK_WINDOW(ref), true, NOTE_4, false, m_tempodiff);
	tempodialog->getValues(&state, &kind, &dot, &newtempo_diff);
	if (state) {
		if (dot) {
			m_tempodiff *= 3 * NOTE_4 / 2 / kind;
		}
		else {
			m_tempodiff *= NOTE_4 / kind;
		}
		if (newtempo_diff  < -999) newtempo_diff  = -999;
		if (newtempo_diff  > 999) newtempo_diff  = 999;
	}
}
