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

#include "crescendo.h"
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


NedCrescendo::NedCrescendo(NedLinePoint *start, NedLinePoint *end, bool decrescendo) :
	NedLine(start, end), m_decrescendo(decrescendo) {
	m_volume_diff = decrescendo ? -40 : 40;
}

NedCrescendo::NedCrescendo(NedLinePoint *start, NedLinePoint *end, bool decrescendo, int volume_diff) :
	NedLine(start, end), m_decrescendo(decrescendo), m_volume_diff(volume_diff) {}

#define CRESCENDO_FAC 0.04
#define MAX_CRESCENDO_OPEN 0.1
#define CRESCENDO_THICK 0.04

void NedCrescendo::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) {
	double open, ypos1;
	NedStaff *staff;
	GList *lptr, *last;
	if (!m_allow_drawing) return;

	if (m_distributed)  {
		cairo_set_line_width(cr, zoom_factor * CRESCENDO_THICK);
		if ((lptr = g_list_first(m_intermediate_staves)) == NULL) {
			NedResource::Abort("NedCrescendo::draw: error 1");
		}
		last = g_list_last(m_intermediate_staves);
		staff = (NedStaff *) lptr->data;
		if (m_decrescendo) {
			open = CRESCENDO_FAC * (staff->getSystemEnd() - m_start_x);
			if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
			cairo_new_path(cr);
			cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_start_x), Y_POS_PAGE_REL(m_start_y + open));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(m_start_y));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_start_x), Y_POS_PAGE_REL(m_start_y - open));
			cairo_stroke(cr);
			for (lptr = g_list_next(lptr); lptr != last; lptr = g_list_next(lptr)) {
				staff = (NedStaff *) lptr->data;
				ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
				open = CRESCENDO_FAC * (staff->getSystemEnd() - staff->getSystemStart());
				if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
				cairo_new_path(cr);
				cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1 + open));
				cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(ypos1));
				cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1 - open));
				cairo_stroke(cr);
			}
			staff = (NedStaff *) lptr->data;
			ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
			open = CRESCENDO_FAC * (m_end_x - staff->getSystemStart());
			if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
			cairo_new_path(cr);
			cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1 + open));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_end_x), Y_POS_PAGE_REL(ypos1));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1 - open));
			cairo_stroke(cr);
		}
		else {
			open = CRESCENDO_FAC * (staff->getSystemEnd() - m_start_x);
			if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
			cairo_new_path(cr);
			cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(m_start_y + open));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_start_x), Y_POS_PAGE_REL(m_start_y));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(m_start_y - open));
			cairo_stroke(cr);
			for (lptr = g_list_next(lptr); lptr != last; lptr = g_list_next(lptr)) {
				staff = (NedStaff *) lptr->data;
				ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
				open = CRESCENDO_FAC * (staff->getSystemEnd() - staff->getSystemStart());
				if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
				cairo_new_path(cr);
				cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(ypos1 + open));
				cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1));
				cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemEnd()), Y_POS_PAGE_REL(ypos1 - open));
				cairo_stroke(cr);
			}
			staff = (NedStaff *) lptr->data;
			ypos1 = staff->getSystem()->getYPos() + staff->getMidPos() + m_line_start_point->getYposRelative();
			open = CRESCENDO_FAC * (m_end_x - staff->getSystemStart());
			if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
			cairo_new_path(cr);
			cairo_move_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_end_x), Y_POS_PAGE_REL(ypos1 + open));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), staff->getSystemStart()), Y_POS_PAGE_REL(ypos1));
			cairo_line_to(cr, X_PAGE_POS_PAGE_REL(staff->getPage(), m_end_x), Y_POS_PAGE_REL(ypos1 - open));
			cairo_stroke(cr);
		}
	}
	else {
		open = CRESCENDO_FAC * (m_end_x - m_start_x);
		if (open > MAX_CRESCENDO_OPEN) open = MAX_CRESCENDO_OPEN;
		if (m_decrescendo) {
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * CRESCENDO_THICK);
			cairo_move_to(cr, X_POS_PAGE_REL(m_line_start_point, m_start_x), Y_POS_PAGE_REL(m_start_y + open));
			cairo_line_to(cr, X_POS_PAGE_REL(m_line_end_point, m_end_x), Y_POS_PAGE_REL(m_start_y));
			cairo_line_to(cr, X_POS_PAGE_REL(m_line_start_point, m_start_x), Y_POS_PAGE_REL(m_start_y - open));
			cairo_stroke(cr);
		}
		else {
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * CRESCENDO_THICK);
			cairo_move_to(cr, X_POS_PAGE_REL(m_line_end_point, m_end_x), Y_POS_PAGE_REL(m_start_y + open));
			cairo_line_to(cr, X_POS_PAGE_REL(m_line_start_point, m_start_x), Y_POS_PAGE_REL(m_start_y));
			cairo_line_to(cr, X_POS_PAGE_REL(m_line_end_point, m_end_x), Y_POS_PAGE_REL(m_start_y - open));
			cairo_stroke(cr);
		}
	}

}

void NedCrescendo::startContextDialog(GtkWidget *ref) {
	int newvoldiff;
	bool state;
	NedVolumeDialog *voldialog = new NedVolumeDialog(GTK_WINDOW(ref), m_volume_diff, true);
	voldialog->getValues(&state, &newvoldiff);
	if (state) {
		m_volume_diff = newvoldiff;
	}
}
