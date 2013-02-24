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
#include "freespacer.h"
#include "chordorrest.h"
#include "page.h"
#include "mainwindow.h"
#include "resource.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)


NedFreeSpacer::NedFreeSpacer(NedMainWindow *main_window) :
		NedFreeReplaceable(),  m_main_window(main_window) {}

NedFreeSpacer::NedFreeSpacer(NedMainWindow *main_window, NedChordOrRest *element) :
	NedFreeReplaceable(element), m_main_window(main_window) {}

NedFreeSpacer::NedFreeSpacer(NedMainWindow *main_window, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_main_window(main_window) {}

void NedFreeSpacer::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;
	if (!m_main_window->getShowHidden()) return;

	double xpos, ypos;

	getXYPos(&xpos, &ypos);

#define FREE_SPACER_RAD 0.5
	
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}
	else {
		cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	}

	cairo_new_path(cr);
	cairo_arc(cr, X_POS_PAGE_REL(xpos), Y_POS_PAGE_REL(ypos), FREE_SPACER_RAD * zoom_level, 0.0, 2.0 * M_PI);
	cairo_fill(cr);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}

bool NedFreeSpacer::trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy) {
	if (!m_main_window->getShowHidden()) return false;
	return NedFreeReplaceable::trySelect(xl, yl, zoom_factor, current_scale, leftx, topy);
}

