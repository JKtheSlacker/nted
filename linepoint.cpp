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
#include "linepoint.h"
#include "line.h"
#include "chordorrest.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "line3.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

NedLinePoint::NedLinePoint(NedLine *line, int p_nr) : NedFreeReplaceable(), m_point_number(p_nr), m_line(line) {}
NedLinePoint::NedLinePoint(NedLine *line, int p_nr, NedChordOrRest *element) : NedFreeReplaceable(element), m_point_number(p_nr), m_line(line) {}
NedLinePoint::NedLinePoint(NedLine *line, int p_nr, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_point_number(p_nr), m_line(line) {}


NedFreeReplaceable *NedLinePoint::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /*= NULL */, NedMainWindow *main_window /* = NULL */) {
	return NULL;
}

void NedLinePoint::adjust_pointers(struct addr_ref_str *slurlist) {
}

void NedLinePoint::getXYPosAndStaff(double *xpos, double *ypos, NedStaff **staff) {
	getXYPos(xpos, ypos);
	*staff = m_element->getStaff();
}
void NedLinePoint::startContextDialog(GtkWidget *ref) {
	if (m_line != NULL) {
		m_line->startContextDialog(ref);
	}
}

NedStaff *NedLinePoint::getStaff() {
	return m_element->getStaff();
}

NedLinePoint *NedLinePoint::getLineStartPoint() {
	return m_line->getLineStartPoint();
}

NedLinePoint *NedLinePoint::getLineEndPoint() {
	return m_line->getLineEndPoint();
}

void NedLinePoint::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	double xpos, ypos;
	if (m_element == NULL) return;

	
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}
#define RECT_D 0.1
	if (m_line == NULL || m_line->isActive()) {
		getXYPos(&xpos, &ypos);
		cairo_new_path(cr);
		cairo_set_line_width(cr, zoom_factor * LINE_THICK);
		cairo_rectangle(cr, X_POS_PAGE_REL(xpos - RECT_D / 2.0) , Y_POS_PAGE_REL(ypos -RECT_D / 2.0) , zoom_factor * RECT_D, zoom_factor * RECT_D); 
		cairo_stroke(cr);
	}
	if (m_point_number == 1 && m_line != NULL) {
		m_line->draw(cr, leftx, topy, zoom_factor, zoom_level);
	}

	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}


void NedLinePoint::recompute() {
	if (m_line) {
		m_line->computeParams();
	}
}

void NedLinePoint::toRel() {
	if (m_line) {
		m_line->setAllPointsToRel(this);
	}
	NedFreeReplaceable::toRel();
}

void NedLinePoint::toRel2() {
	NedFreeReplaceable::toRel();
}
	

void NedLinePoint::shift(double x, double y, double zoom_factor, double current_scale, double leftx, double topy) {
	NedLinePoint *startp, *endp, *midp;
	double staffdist;
	if (m_line) {
		startp = m_line->getLineStartPoint();
		endp = m_line->getLineEndPoint();
		if (m_line->isLine3()) {
			midp = ((NedLine3 *) m_line)->getLineMidPoint();
			if (endp == this || startp == this) {
				NedFreeReplaceable::shiftOnlyX(x, y, zoom_factor, current_scale, leftx, topy);
			}
			else if (midp == this) {
				NedFreeReplaceable::shift(x, y, zoom_factor, current_scale, leftx, topy);
				staffdist = startp->m_element->getSystem()->getYPos() + startp->m_element->getStaff()->getMidPos() - (
					midp->m_element->getSystem()->getYPos() + midp->m_element->getStaff()->getMidPos());
				startp->shiftOnlyY(m_element->getStaff()->getStaffNumber(), y + staffdist * zoom_factor, zoom_factor, current_scale, leftx, topy);
				staffdist = endp->m_element->getSystem()->getYPos() + endp->m_element->getStaff()->getMidPos() - (
					midp->m_element->getSystem()->getYPos() + midp->m_element->getStaff()->getMidPos());
				endp->shiftOnlyY(m_element->getStaff()->getStaffNumber(), y + staffdist * zoom_factor, zoom_factor, current_scale, leftx, topy);
			}
		}
		else {
			if (endp == this) {
				NedFreeReplaceable::shiftOnlyX(x, y, zoom_factor, current_scale, leftx, topy);
			}
			else if (startp == this) {
				NedFreeReplaceable::shift(x, y, zoom_factor, current_scale, leftx, topy);
				staffdist = endp->m_element->getSystem()->getYPos() + endp->m_element->getStaff()->getMidPos() - (
					startp->m_element->getSystem()->getYPos() + startp->m_element->getStaff()->getMidPos());
				endp->shiftOnlyY(m_element->getStaff()->getStaffNumber(), y + staffdist * zoom_factor, zoom_factor, current_scale, leftx, topy);
			}
		}
		m_line->computeParams();
		return;
	}
	NedFreeReplaceable::shift(x, y, zoom_factor, current_scale, leftx, topy);
	if (m_line) {
		m_line->computeParams();
	}
}

