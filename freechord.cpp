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

#include "freechord.h"
#include "chordorrest.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "chordpainter.h"
#include "chordstruct.h"
#include "mainwindow.h"
#include "chorddialog.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)
#define X_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + leftx) / zoom_factor - m_element->getPage()->getContentXpos()) 
#define Y_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + topy - (TOP_BOTTOM_BORDER + DEFAULT_BORDER)) / zoom_factor)

NedFreeChord::NedFreeChord(char *code, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_main_window(main_window), m_chordname(chordname), m_status(status) {
	m_chordptr = NedChordPainter::searchCordByCode(code);
	if (m_chordptr == NULL) {
		NedResource::Abort("Chord code not found");
	}
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	computeBbox();
}

NedFreeChord::NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window) :
		NedFreeReplaceable(), m_chordptr(chordptr), m_main_window(main_window), m_chordname(chordname), m_status(status) {
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	computeBbox();
}
NedFreeChord::NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element) :
	NedFreeReplaceable(element), m_chordptr(m_chordptr), m_main_window(main_window), m_chordname(chordname), m_status(status) {
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	computeBbox();
}
NedFreeChord::NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element, double x, double y, bool relative) :
	NedFreeReplaceable(element, x, y, relative), m_chordptr(m_chordptr), m_main_window(main_window), m_chordname(chordname), m_status(status) {
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	computeBbox();
}

NedFreeReplaceable *NedFreeChord::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */, NedMainWindow *main_window /* = NULL */) {
	if (main_window == NULL) {
		NedResource::Abort("NedFreeChord::clone main_window == NULL");
	}
	NedFreeChord *newFreeChord = new NedFreeChord(m_chordptr, m_chordname, m_status, main_window, newelement, m_x, m_y, true);
	return newFreeChord;
}

void NedFreeChord::adjust_pointers(struct addr_ref_str *slurlist) {
}


void NedFreeChord::computeBbox() {
#define CHORD_ZOOM 0.008
#define FEELED_TEXT_HEIGHT 0.6

	if (m_status & GUITAR_CHORD_DRAW_DIAGRAM) {
		m_bbox.width = GUITAR_WIDTH * CHORD_ZOOM;
		m_bbox.height = F_DIST * F_COUNT * CHORD_ZOOM;
		return;
	}
	m_bbox.width = GUITAR_WIDTH * CHORD_ZOOM;
	m_bbox.height = FEELED_TEXT_HEIGHT;
}

bool NedFreeChord::trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy) {
	if (m_element == NULL) return false;
	double xpos, ypos;
	double x = X_POS_PAGE_REL_INVERSE(xl);
	double y = Y_POS_PAGE_REL_INVERSE(yl);

	getXYPos(&xpos, &ypos);
	if (x < xpos - m_bbox.width / 2.0 || x > xpos + m_bbox.width / 2.0 || y < ypos -  m_bbox.height / 2.0 || y > ypos + m_bbox.height / 2.0) return false;
	setThis();
	return true;
}

void NedFreeChord::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;

	double xpos, ypos;

	getXYPos(&xpos, &ypos);
#define CHORD_ZOOM_Z (CHORD_ZOOM * zoom_factor) 
	
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}

	if (m_status & GUITAR_CHORD_DRAW_DIAGRAM) {
		NedChordPainter::draw_chord(cr, m_chordptr, m_chord_name, m_status, true, X_POS_PAGE_REL(xpos - m_bbox.width / 2.0 ),
			Y_POS_PAGE_REL(ypos - m_bbox.height / 2.0), CHORD_ZOOM_Z, m_main_window->getCurrentZoomLevel());
	}
	else {
		NedChordPainter::draw_chord(cr, m_chordptr, m_chord_name, m_status, true, X_POS_PAGE_REL(xpos - m_bbox.width / 2.0 ), Y_POS_PAGE_REL(ypos + 0.8 *m_bbox.height), CHORD_ZOOM_Z, m_main_window->getCurrentZoomLevel());
	}

	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_is_abs) {
		drawConnection(cr, xpos, ypos, leftx, topy, zoom_factor);
	}
}

const char *NedFreeChord::getCode() {
	return m_chordptr->id;
}

void NedFreeChord::startContextDialog(GtkWidget *ref) {
	struct chord_str *new_chord_ptr;
	int new_chord_name;
	bool state;
	unsigned int new_status;
	NedChordDialog *dialog = new NedChordDialog(GTK_WINDOW(ref), m_main_window, m_chordptr, m_status);
	dialog->getValues(&state, &new_chord_ptr, &new_chord_name, &new_status);
	if (!state) return;
	m_chordptr = new_chord_ptr;
	m_chordname = new_chord_name;
	m_status = new_status;
	strcpy(m_chord_name, NedChordPainter::roots[(m_status & ROOT_MASK)]);
	strcat(m_chord_name, NedChordPainter::modies2[((m_status >> ROOT_BITS) & ((1 << MODI_BITS) - 1))]);
	computeBbox();
}
