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

#include "slur.h"
#include "slurpoint.h"
#include "page.h"
#include "staff.h"
#include "chordorrest.h"
#include <math.h>

#define X_POS_PAGE_REL(sp, p) ((sp->m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)

#define COMPUTE_MID_POINT_RIGHT(x0, y0, x1, y1, x2, y2, midx, midy, zmidx, zmidy) midx2 = (x0) + ((x1) - (x0)) / 2.0;\
midy2 = (y0) + ((y1) - (y0)) / 2.0;\
zmidx = (x1) + ((x2) - (x1)) / 2.0;\
zmidy = (y1) + ((y2) - (y1)) / 2.0;\
midx = midx2 + (zmidx - midx2) / 2.0;\
midy = midy2 + (zmidy - midy2) / 2.0

#define COMPUTE_MID_POINT_LEFT(x0, y0, x1, y1, x2, y2, midx, midy, zmidx, zmidy) zmidx = (x0) + ((x1) - (x0)) / 2.0;\
zmidy = (y0) + ((y1) - (y0)) / 2.0;\
midx2 = (x1) + ((x2) - (x1)) / 2.0;\
midy2 = (y1) + ((y2) - (y1)) / 2.0;\
midx = zmidx + (midx2 - zmidx) / 2.0;\
midy = zmidy + (midy2 - zmidy) / 2.0


NedSlur::NedSlur(NedSlurPoint *points[3]) {
	m_slur_points[0] = points[0];
	m_slur_points[1] = points[1];
	m_slur_points[2] = points[2];
	m_slur_points[0]->setSlur(this);
	m_slur_points[1]->setSlur(this);
	m_slur_points[2]->setSlur(this);
	computeParams();
}

bool NedSlur::isActive() {
	return m_slur_points[0]->isActive() || m_slur_points[1]->isActive() || m_slur_points[2]->isActive();
}


void NedSlur::computeParams() {
	double len;
	double xoffs, yoffs;
	double xdist, ydist;
	double sinx, cosx;
	double staff0xend, staff2xstart;
	double y1_0, y2_0, y0_1, y1_1;
	double midxl, midyl;
	double midxr, midyr;
	double midx2, midy2;
	NedStaff *staff0, *staff1, *staff2;
	NedSlurPoint *tmp_point;

#define SLUR_THICK2 0.05
#define SLUR_THICK1 0.10
#define XCORREC2 0.25
#define XCORREC1 0.05
#define YCORREC 0.0

	m_slur_points[0]->getXYPosAndStaff(&m_xpos0, &m_ypos0, &staff0);
	m_slur_points[1]->getXYPosAndStaff(&m_xpos1, &m_ypos1, &staff1);
	m_slur_points[2]->getXYPosAndStaff(&m_xpos2, &m_ypos2, &staff2);
	if (m_slur_points[2]->less_than(m_slur_points[0])) {
		tmp_point = m_slur_points[0];
		m_slur_points[0] = m_slur_points[2];
		m_slur_points[2] = tmp_point;
		m_slur_points[2]->m_point_number = 2;
		m_slur_points[0]->m_point_number = 0;
		m_slur_points[0]->getXYPosAndStaff(&m_xpos0, &m_ypos0, &staff0);
		m_slur_points[1]->getXYPosAndStaff(&m_xpos1, &m_ypos1, &staff1);
		m_slur_points[2]->getXYPosAndStaff(&m_xpos2, &m_ypos2, &staff2);
	}
	if (m_slur_points[1]->less_than(m_slur_points[0])) {
		tmp_point = m_slur_points[1];
		m_slur_points[1] = m_slur_points[0];
		m_slur_points[0] = tmp_point;
		m_slur_points[1]->m_point_number = 1;
		m_slur_points[0]->m_point_number = 0;
		m_slur_points[0]->getXYPosAndStaff(&m_xpos0, &m_ypos0, &staff0);
		m_slur_points[1]->getXYPosAndStaff(&m_xpos1, &m_ypos1, &staff1);
		m_slur_points[2]->getXYPosAndStaff(&m_xpos2, &m_ypos2, &staff2);
	}
	if (m_slur_points[2]->less_than(m_slur_points[1])) {
		tmp_point = m_slur_points[1];
		m_slur_points[1] = m_slur_points[2];
		m_slur_points[2] = tmp_point;
		m_slur_points[1]->m_point_number = 1;
		m_slur_points[2]->m_point_number = 2;
		m_slur_points[0]->getXYPosAndStaff(&m_xpos0, &m_ypos0, &staff0);
		m_slur_points[1]->getXYPosAndStaff(&m_xpos1, &m_ypos1, &staff1);
		m_slur_points[2]->getXYPosAndStaff(&m_xpos2, &m_ypos2, &staff2);
	}
	if (staff0 == staff2) {
		m_distributed = false;
		xdist = m_xpos2 - m_xpos0;
		ydist = m_ypos2 - m_ypos0;
		len = sqrt(xdist * xdist + ydist * ydist);
		sinx = xdist / len;
		cosx = ydist / len;
		yoffs = ((cosx < 0.0) ? SLUR_THICK2 : SLUR_THICK1) * sinx;
		xoffs = SLUR_THICK1 * cosx;
		m_xpos11 = m_xpos1 - xoffs;
		m_xpos12 = m_xpos1 + xoffs + ((cosx < 0.0) ? XCORREC2 : XCORREC1) *  sinx;
		m_ypos11 = m_ypos1 - yoffs;
		m_ypos12 = m_ypos1 + yoffs + YCORREC * cosx;
	}
	else {
		m_distributed = true;
		staff0xend = staff0->getSystemEnd();
		staff2xstart = staff2->getSystemStart();
		y1_0 = m_slur_points[1]->computeYposRelative(m_slur_points[0]->getStaff());
		y2_0 = m_slur_points[2]->computeYposRelative(m_slur_points[0]->getStaff());
		COMPUTE_MID_POINT_LEFT(m_xpos0, m_ypos0, staff0xend, y1_0, staff0xend + (m_xpos2 - staff2xstart), y2_0, m_midx1_0, m_midy1_0, midxl, midyl);

		xdist = m_midx1_0 - m_xpos0;
		ydist = m_midy1_0 - m_ypos0;
		len = sqrt(xdist * xdist + ydist * ydist);
		sinx = xdist / len;
		cosx = ydist / len;
		yoffs = ((cosx < 0.0) ? SLUR_THICK2 : SLUR_THICK1) * sinx;
		xoffs = SLUR_THICK1 * cosx;
		m_xpos11_l = midxl - xoffs;
		m_xpos12_l = midxl + xoffs + ((cosx < 0.0) ? XCORREC2 : XCORREC1) *  sinx;
		m_ypos11_l = midyl - yoffs;
		m_ypos12_l = midyl + yoffs + YCORREC * cosx;

		/* ------------------------------------- */
		y0_1 = m_slur_points[0]->computeYposRelative(m_slur_points[2]->getStaff());
		y1_1 = m_slur_points[1]->computeYposRelative(m_slur_points[2]->getStaff());
		COMPUTE_MID_POINT_RIGHT(staff2xstart - (staff0xend - m_xpos0), y0_1, staff2xstart, y1_1, m_xpos2, m_ypos2,
				m_midx1_1, m_midy1_1, midxr, midyr);
		xdist = m_xpos2 - m_midx1_1;
		ydist = m_ypos2 - m_midy1_1;
		len = sqrt(xdist * xdist + ydist * ydist);
		sinx = xdist / len;
		cosx = ydist / len;
		yoffs = ((cosx < 0.0) ? SLUR_THICK1 : SLUR_THICK2) * sinx;
		xoffs = SLUR_THICK1 * cosx;
		m_xpos11_r = midxr - xoffs;
		m_xpos12_r = midxr + xoffs + ((cosx < 0.0) ? XCORREC1 : XCORREC2) *  sinx;
		m_ypos11_r = midyr - yoffs;
		m_ypos12_r = midyr + yoffs + YCORREC * cosx;
	}
}

void NedSlur::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) {
	if (m_distributed) {
		cairo_new_path(cr);
		cairo_move_to(cr, X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0),
			X_POS_PAGE_REL(m_slur_points[0], m_xpos11_l), Y_POS_PAGE_REL(m_ypos11_l),
			X_POS_PAGE_REL(m_slur_points[0], m_midx1_0), Y_POS_PAGE_REL(m_midy1_0));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[0], m_midx1_0), Y_POS_PAGE_REL(m_midy1_0),
			X_POS_PAGE_REL(m_slur_points[0], m_xpos12_l), Y_POS_PAGE_REL(m_ypos12_l),
			X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0));
		cairo_close_path(cr);
		//cairo_stroke(cr);
		cairo_fill(cr);
		/* --------------------------------------------------- */
		cairo_new_path(cr);
		cairo_move_to(cr, X_POS_PAGE_REL(m_slur_points[2], m_midx1_1), Y_POS_PAGE_REL(m_midy1_1));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[2], m_midx1_1), Y_POS_PAGE_REL(m_midy1_1),
			X_POS_PAGE_REL(m_slur_points[2], m_xpos11_r), Y_POS_PAGE_REL(m_ypos11_r),
			X_POS_PAGE_REL(m_slur_points[2], m_xpos2), Y_POS_PAGE_REL(m_ypos2));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[2], m_xpos2), Y_POS_PAGE_REL(m_ypos2),
			X_POS_PAGE_REL(m_slur_points[2], m_xpos12_r), Y_POS_PAGE_REL(m_ypos12_r),
			X_POS_PAGE_REL(m_slur_points[2], m_midx1_1), Y_POS_PAGE_REL(m_midy1_1));
		cairo_close_path(cr);
		//cairo_stroke(cr);
		cairo_fill(cr);
	}
	else {
		cairo_new_path(cr);
		cairo_move_to(cr, X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0),
			X_POS_PAGE_REL(m_slur_points[1], m_xpos11), Y_POS_PAGE_REL(m_ypos11),
			X_POS_PAGE_REL(m_slur_points[2], m_xpos2), Y_POS_PAGE_REL(m_ypos2));
		cairo_curve_to(cr, X_POS_PAGE_REL(m_slur_points[2], m_xpos2), Y_POS_PAGE_REL(m_ypos2),
			X_POS_PAGE_REL(m_slur_points[1], m_xpos12), Y_POS_PAGE_REL(m_ypos12),
			X_POS_PAGE_REL(m_slur_points[0], m_xpos0), Y_POS_PAGE_REL(m_ypos0));
		cairo_close_path(cr);
		//cairo_stroke(cr);
		cairo_fill(cr);
	}
}



