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

#include "mainwindow.h"
#include "volumesign.h"
#include "chordorrest.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "freereplaceable.h"

#define X_POS_PAGE_REL(p) (((p) + m_element->getPage()->getContentXpos()) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy + (TOP_BOTTOM_BORDER + DEFAULT_BORDER))
#define X_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + leftx) / zoom_factor - m_element->getPage()->getContentXpos()) 
//#define Y_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + topy) / zoom_factor - (TOP_BOTTOM_BORDER + DEFAULT_BORDER))
#define Y_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + topy - (TOP_BOTTOM_BORDER + DEFAULT_BORDER)) / zoom_factor)

NedFreeReplaceable::NedFreeReplaceable() : m_abs(false), m_element(NULL), m_is_abs(false)  {}
NedFreeReplaceable::NedFreeReplaceable(NedChordOrRest *element) :  m_abs(false), m_element(element), m_is_abs(false), m_element_at_move_start(element) {
	m_element->addFreeDislaceable(this);
	if (m_is_abs) {
		toRel();
	}
}
NedFreeReplaceable::NedFreeReplaceable(NedChordOrRest *element, double x, double y, bool relative) :
m_x(x), m_y(y), m_abs(false), m_element(element), m_is_abs(!relative), m_element_at_move_start(element) {
	m_element->addFreeDislaceable(this);
	if (m_is_abs) {
		toRel();
	}
	m_x_at_start = m_x;
	m_y_at_start = m_y;
	m_yrel = m_y;
}

void NedFreeReplaceable::setElement(NedChordOrRest *element) {
	m_element = element;
	m_element->addFreeDislaceable(this); // friend !!
	m_element_at_move_start = element;
}

void NedFreeReplaceable::freeFromElement() {
	if (m_element == NULL) return;
	m_element->removeFreeDisplaceable(this); // friend !!
}

void NedFreeReplaceable::setXY(double x, double y, bool relative) {
	m_x = x; m_y = y;
	m_is_abs = !relative;
	if (m_is_abs) {
		toRel();
	}
	m_x_at_start = m_x;
	m_y_at_start = m_y;
	m_yrel = m_y;
	computeBbox();
}

double NedFreeReplaceable::getAbsoluteX() {
	if (m_is_abs) {
		return m_x;
	}
	return m_x + m_element->m_xpos;
}

double NedFreeReplaceable::getAbsoluteY() {
	if (m_is_abs) {
		return m_y;
	}
	return m_y + m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
}

void NedFreeReplaceable::setThis() {
	if (m_element == NULL) return;
	m_element->getMainWindow()->m_selected_free_replaceable = this;
}

bool NedFreeReplaceable::isActive() {
	if (m_element == NULL) return false;
	return m_element->getMainWindow()->m_selected_free_replaceable == this;
}

void NedFreeReplaceable::getXYPos(double *xpos, double *ypos) {
	if (m_is_abs) {
		*xpos = m_x;
		*ypos = m_y;
	}
	else {
		*xpos = m_x + m_element->m_xpos;
		*ypos = m_y + m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
	}
}

void NedFreeReplaceable::toAbs() {
	m_x_at_start = m_x;
	m_y_at_start = m_y;
	m_element_at_move_start = m_element;
	if (m_is_abs) return;
	m_x += m_element->m_xpos;
	m_y += m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
	m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
	m_last_ref_x = m_x;
	m_last_ref_y = m_y;
	m_is_abs = true;
}

void NedFreeReplaceable::toRel() {
	if (!m_is_abs) return;
	m_x -= m_element->m_xpos;
	m_y -= m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
	m_yrel = m_y;
	m_is_abs = false;
	computeBbox();
}


void NedFreeReplaceable::shift(double x, double y, double zoom_factor, double current_scale, double leftx, double topy) {
	NedChordOrRest *newelement;
	NedPage *nextpage;
	double ydist, yd;
	if (!m_is_abs) {
		toAbs();
	}
#define CRITICAL_DIST 0.4
	m_x = X_POS_PAGE_REL_INVERSE(x);
	m_y = Y_POS_PAGE_REL_INVERSE(y);
	if (m_x < 0.0) {
		if ((nextpage = m_element->getPage()->getPreviousPage()) == NULL) {
			m_x = 0.0;
		}
		else {
			newelement = nextpage->findNearestElement(-1, m_x, m_y, m_element ? m_element->getStaff() : NULL, &ydist);
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
			return;
		}
	}
	if (m_x > m_element->getPage()->getWidth()) {
		if ((nextpage = m_element->getPage()->getNextPage()) == NULL)  {
			m_x = m_element->getPage()->getWidth() - LEFT_RIGHT_BORDER;
		}
		else {
			newelement = nextpage->findNearestElement(-1, m_x, m_y, m_element ? m_element->getStaff() : NULL, &ydist);
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
			return;
		}
	}
	if (m_y < TOP_BOTTOM_BORDER) m_y = TOP_BOTTOM_BORDER;
	if (m_y >  m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER) m_y = m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER;
	if ((m_x - m_last_ref_x) * (m_x - m_last_ref_x) + (m_y - m_last_ref_y) * (m_y - m_last_ref_y) > CRITICAL_DIST) {
		newelement = m_element->getPage()->findNearestElement(-1, m_x, m_y, m_element ? m_element->getStaff() : NULL, &ydist);
		yd = m_y - m_last_elements_y;
		if (yd < 0.0) yd = -yd;
		if (m_element != newelement) {
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
		}
	}
}

void NedFreeReplaceable::shiftOnlyX(double x, double y, double zoom_factor, double current_scale, double leftx, double topy) {
	NedChordOrRest *newelement;
	NedPage *nextpage;
	int staff_nr;
	double ydist, yd, test_y;
	if (!m_is_abs) {
		toAbs();
	}
#define CRITICAL_DIST 0.4
	m_x = X_POS_PAGE_REL_INVERSE(x);
	test_y = Y_POS_PAGE_REL_INVERSE(y);
	staff_nr = m_element->getStaff()->getStaffNumber();
	if (m_x < 0.0) {
		if ((nextpage = m_element->getPage()->getPreviousPage()) == NULL) {
			m_x = 0.0;
		}
		else {
			newelement = nextpage->findNearestElement(staff_nr, m_x, test_y, m_element ? m_element->getStaff() : NULL, &ydist);
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
			return;
		}
	}
	if (m_x > m_element->getPage()->getWidth()) {
		if ((nextpage = m_element->getPage()->getNextPage()) == NULL)  {
			m_x = m_element->getPage()->getWidth() - LEFT_RIGHT_BORDER;
		}
		else {
			newelement = nextpage->findNearestElement(staff_nr, m_x, test_y, m_element ? m_element->getStaff() : NULL, &ydist);
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
			return;
		}
	}
	if (test_y < TOP_BOTTOM_BORDER) test_y = TOP_BOTTOM_BORDER;
	if (test_y >  m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER) test_y = m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER;
	if ((m_x - m_last_ref_x) * (m_x - m_last_ref_x) + (test_y - m_last_ref_y) * (test_y - m_last_ref_y) > CRITICAL_DIST) {
		newelement = m_element->getPage()->findNearestElement(staff_nr, m_x, test_y, m_element ? m_element->getStaff() : NULL, &ydist);
		yd = test_y - m_last_elements_y;
		if (yd < 0.0) yd = -yd;
		if (m_element != newelement) {
			if (m_element->getStaff() != newelement->getStaff()) {
				m_y += newelement->getSystem()->getYPos() + newelement->getStaff()->getMidPos() - (
					m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos());
			}
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
		}
	}
}


void NedFreeReplaceable::shiftOnlyY(int staff_nr, double y, double zoom_factor, double current_scale, double leftx, double topy) {
	NedChordOrRest *newelement;
	double dummy0 = 10000000.0, dummy;
	if (!m_is_abs) {
		toAbs();
	}
#define CRITICAL_DIST 0.4
	m_y = Y_POS_PAGE_REL_INVERSE(y);
	if (m_y < TOP_BOTTOM_BORDER) m_y = TOP_BOTTOM_BORDER;
	if (m_y >  m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER) m_y = m_element->getPage()->getHeight() - TOP_BOTTOM_BORDER;
	if (m_element->getStaff()->getStaffNumber() != staff_nr) {
		newelement = m_element->getSystem()->findNearestElement(staff_nr, m_x, m_y, NULL, &dummy0, &dummy);
		if (m_element != newelement) {
			m_element->removeFreeDisplaceable(this);
			newelement->addFreeDislaceable(this);
			m_element = newelement;
			m_last_elements_y = m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos();
		}
	}
}

bool NedFreeReplaceable::trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy) {
	bool ret;
	double x = X_POS_PAGE_REL_INVERSE(xl);
	double y = Y_POS_PAGE_REL_INVERSE(yl);


	double xx, yy;
	getXYPos(&xx, &yy);

	ret = ((xx - x) * (xx - x) + (yy - y) * (yy - y) < 0.15);
	if (ret) {
		setThis();
	}
	return ret;
}

void NedFreeReplaceable::drawConnection(cairo_t *cr, double xpos, double ypos, double leftx, double topy, double zoom_factor) {
	cairo_new_path(cr);
#define THIN_LINE_THICK 0.02
	cairo_set_line_width(cr, zoom_factor * THIN_LINE_THICK);
	cairo_set_source_rgb (cr, 0.0, 0.8, 0.8);
	cairo_move_to(cr, X_POS_PAGE_REL(xpos), Y_POS_PAGE_REL(ypos)) ;
	cairo_line_to(cr, X_POS_PAGE_REL(m_element->getRealXpos()), Y_POS_PAGE_REL(m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos()));
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
}

void NedFreeReplaceable::drawConnectionInFreeText(cairo_t *cr, double xpos, double ypos, double scale, double leftx, double topy, double zoom_factor) {
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * THIN_LINE_THICK);
	cairo_set_source_rgb (cr, 0.0, 0.8, 0.8);
	cairo_move_to(cr, scale * X_POS_PAGE_REL(xpos), scale * Y_POS_PAGE_REL(ypos)) ;
	cairo_line_to(cr, scale * X_POS_PAGE_REL(m_element->getRealXpos()), scale * Y_POS_PAGE_REL(m_element->getSystem()->getYPos() + m_element->getStaff()->getMidPos()));
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
}

bool NedFreeReplaceable::less_than(NedFreeReplaceable *other) {
	NedPage *my_page, *other_page;
	NedStaff *my_staff, *other_staff;
	my_staff = m_element->getStaff();
	other_staff = other->m_element->getStaff();
	if (my_staff == other_staff) {
		return getAbsoluteX() < other->getAbsoluteX();
	}
	my_page = my_staff->getPage();
	other_page = other_staff->getPage();
	if (my_page == other_page) {
		return my_staff->getSystem()->getSystemNumber() < other_staff->getSystem()->getSystemNumber();
	}
	return my_page->getPageNumber() < other_page->getPageNumber();
}

