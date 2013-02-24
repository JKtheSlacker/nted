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
#include "slurpoint.h"
#include "slur.h"
#include "chordorrest.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedSlurPoint::NedSlurPoint(NedSlur *slur, int p_nr) : NedFreeReplaceable(), m_point_number(p_nr), m_slur(slur) {}
NedSlurPoint::NedSlurPoint(NedSlur *slur, int p_nr, NedChordOrRest *element) : NedFreeReplaceable(element), m_point_number(p_nr), m_slur(slur) {}
NedSlurPoint::NedSlurPoint(NedSlur *slur, int p_nr, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_point_number(p_nr), m_slur(slur) {}


NedFreeReplaceable *NedSlurPoint::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */, NedMainWindow *main_window /* = NULL */) {
	struct addr_ref_str *adr_ptr;
	if (slurlist == NULL) return NULL;
	NedSlurPoint *slurpoint = new NedSlurPoint(m_slur, m_point_number, newelement, m_x, m_y, true);
	if ((adr_ptr = (struct addr_ref_str *) g_try_malloc(sizeof(struct addr_ref_str))) == NULL) {
		NedResource::Abort("NedChordOrRest::clone");
	}
	adr_ptr->ori = m_slur;
	adr_ptr->copy = (void *) slurpoint;
	adr_ptr->next = *slurlist;
	*slurlist = adr_ptr;
	return slurpoint;
}

void NedSlurPoint::adjust_pointers(struct addr_ref_str *slurlist) {
	NedSlurPoint *p[3];
	struct addr_ref_str *adr_ptr;
	if (m_point_number != 2) return;

	memset(p, 0, sizeof(p));

	for (adr_ptr = slurlist; adr_ptr; adr_ptr = adr_ptr->next) {
		if (adr_ptr->ori != (void *) m_slur) continue;
		switch (((NedSlurPoint *) adr_ptr->copy)->m_point_number) {
			case 0: p[0] = (NedSlurPoint *) adr_ptr->copy; break;
			case 1: p[1] = (NedSlurPoint *) adr_ptr->copy; break;
			case 2: p[2] = (NedSlurPoint *) adr_ptr->copy; break;
			default: NedResource::Abort("NedSlurPoint::adjust_pointers(1)");
		}
	}
	if (p[0] == NULL || p[1] == NULL || p[2] == NULL) {
		NedResource::Abort("NedSlurPoint::adjust_pointers(2)");
	}
	new NedSlur(p);
}


void NedSlurPoint::getXYPosAndStaff(double *xpos, double *ypos, NedStaff **staff) {
	getXYPos(xpos, ypos);
	*staff = m_element->getStaff();
}
void NedSlurPoint::startContextDialog(GtkWidget *ref) {}

NedStaff *NedSlurPoint::getStaff() {
	return m_element->getStaff();
}

NedSlurPoint **NedSlurPoint::getSlurPoints() {
	return m_slur->getSlurPoints();
}

void NedSlurPoint::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	double xpos, ypos;
	if (m_element == NULL) return;

	
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}
#define RECT_D 0.1
	if (m_slur == NULL || m_slur->isActive()) {
		getXYPos(&xpos, &ypos);
		cairo_new_path(cr);
		cairo_set_line_width(cr, zoom_factor * LINE_THICK);
		cairo_rectangle(cr, X_POS_PAGE_REL(xpos - RECT_D / 2.0) , Y_POS_PAGE_REL(ypos -RECT_D / 2.0) , zoom_factor * RECT_D, zoom_factor * RECT_D); 
		cairo_stroke(cr);
	}
	if (m_point_number == 2 && m_slur != NULL) {
		m_slur->draw(cr, leftx, topy, zoom_factor, zoom_level);
	}

	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}


void NedSlurPoint::recompute() {
	if (m_slur) {
		m_slur->computeParams();
	}
}
	

void NedSlurPoint::shift(double x, double y, double zoom_factor, double m_current_scale, double leftx, double topy) {
	NedFreeReplaceable::shift(x, y, zoom_factor, m_current_scale, leftx, topy);
	if (m_slur) {
		m_slur->computeParams();
	}
}

double NedSlurPoint::computeYposRelative(NedStaff *staff) {
	if (m_is_abs) {
		return m_y - (m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos()) +
			staff->getSystem()->getYPos() + staff->getMidPos();
	}
	return m_y + staff->getSystem()->getYPos() + staff->getMidPos();
}
