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

#include "tuplet.h"
#include "chordorrest.h"
#include "page.h"
#include "system.h"
#include "voice.h"
#include "mainwindow.h"
#include "staff.h"
#include "beam.h"

#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getStaff()->getBottomPos()) * zoom_factor - topy)
#define Y_POS_SYS_REL(p) (((p) + getSystem()->getYPos()) * zoom_factor - topy)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_STAFF_REL(p) ((height - ((p) + getSystem()->getYPos() + getStaff()->getBottomPos())) * PS_ZOOM)

NedTuplet::NedTuplet(int tuplet_val, GList *chords_list, double *topy, double *boty) :
m_chords(chords_list), m_tuplet_val(tuplet_val) {
	GList *lptr;

	for (lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setTupletPtr(this);
	}

	computeTuplet(topy, boty);
}

NedMainWindow *NedTuplet::getMainWindow() {return ((NedChordOrRest *) g_list_first(m_chords)->data)->getStaff()->getSystem()->getPage()->getMainWindow();}

NedPage *NedTuplet::getPage() {return ((NedChordOrRest *) g_list_first(m_chords)->data)->getStaff()->getSystem()->getPage();}
NedSystem *NedTuplet::getSystem() {return ((NedChordOrRest *) g_list_first(m_chords)->data)->getStaff()->getSystem();}
NedStaff *NedTuplet::getStaff() {return ((NedChordOrRest *) g_list_first(m_chords)->data)->getStaff();}


void NedTuplet::computeTuplet(/* int stemdir */  double *topy, double *boty) {
	GList *lptr;
	NedBeam *b;
	double marker_pos;
	int ups = 0, chords = 0;
	NedChordOrRest *first = (NedChordOrRest *) g_list_first(m_chords)->data;
	NedChordOrRest *last = (NedChordOrRest *) g_list_last(m_chords)->data;
	m_start_x = first->getXPos() + first->getBBox()->x - TUPLET_XFRONTOVER;
	m_end_x = last->getXPos() + last->getBBox()->x + last->getBBox()->width /*+ TUPLET_XOVER */;
	switch(last->getDotCount()) {
	case 1:
	  m_end_x += DOT1POS;
	  break;
	case 2:
	  m_end_x += DOT2POS;
	  break;
	default:
	  break;
	}
	m_up = TRUE;
	m_bracket_needed = FALSE;
	for(lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->isUp()) {
			ups++;
		}
		if (!((NedChordOrRest *) lptr->data)->isRest()) {
			chords++;
		}
	}
	m_up = ups > chords / 2;
	if (m_up) {
		m_start_y = m_end_y = -4 * LINE_DIST;
	}
	else {
		m_start_y = m_end_y = 0;
	}
	for(lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		if (!((NedChordOrRest *) lptr->data)->isRest()) {
			if (m_up) {
				if (((NedChordOrRest *) lptr->data)->getTopOfChordOrRest() <  m_start_y) {
					m_start_y = m_end_y = ((NedChordOrRest *) lptr->data)->getTopOfChordOrRest();
				}
			}
			else {
				if (((NedChordOrRest *) lptr->data)->getBottomOfChordOrRest() > m_start_y) {
					m_start_y = m_end_y = ((NedChordOrRest *) lptr->data)->getBottomOfChordOrRest();
				}
			}
		}
		if (((NedChordOrRest *) lptr->data)->isRest() || ((NedChordOrRest *) lptr->data)->getBeam() == NULL) {
			m_bracket_needed = TRUE;
		}
	}
	if (m_bracket_needed) {
		if (m_up) {
			m_start_x += TUPLET_XOVER;
			m_end_x += TUPLET_XOVER;
			marker_pos = m_start_y - TUPLET_MARKER_DIST_UP  -  TUPLET_BRACKET_HEIGHT - NUMNBER_GLYPH_HEIGHT;
			if (marker_pos + 4 * LINE_DIST - TUPLET_MARKER_HEIGHT < *topy) *topy = marker_pos + 4 * LINE_DIST - TUPLET_MARKER_HEIGHT;
		}
		else {
			marker_pos = m_start_y + TUPLET_MARKER_DIST_DOWN + 0.3 + TUPLET_BRACKET_HEIGHT +  NUMNBER_GLYPH_HEIGHT;
			if (marker_pos + TUPLET_MARKER_HEIGHT  > *boty) *boty = marker_pos + TUPLET_MARKER_HEIGHT;
		}
	}
	else {
		for(lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
			b = ((NedChordOrRest *) lptr->data)->getBeam();
			b->setTupletMarker(m_tuplet_val, topy, boty);
		}
	}
}

NedTuplet::~NedTuplet() {
	GList *lptr;

	for (lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setTupletPtr(NULL);
	}
}

unsigned long long NedTuplet::getEndTime() {
	NedChordOrRest *last = (NedChordOrRest *) g_list_last(m_chords)->data;
	return last->getMidiTime() + last->getDuration();
}

void NedTuplet::reconfigureLast() {
	GList *lptr, *maxmidichordorrest;
	unsigned int maxmiditime;

	((NedChordOrRest *) g_list_first(m_chords)->data)->getVoice()->assignMidiTimes(); //recompute MIDI Times
	maxmidichordorrest = lptr = g_list_first(m_chords);
	maxmiditime = ((NedChordOrRest *) lptr->data)->getMidiTime();
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() > maxmiditime) {
			maxmiditime = ((NedChordOrRest *) lptr->data)->getMidiTime();
			maxmidichordorrest = lptr;
		}
	}
	for (lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setLastTupletFlag(((NedChordOrRest *) lptr->data)->getMidiTime() == maxmiditime);
	}
}

void NedTuplet::draw(cairo_t *cr) {
	if (!m_bracket_needed) return;
	int val = m_tuplet_val;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double leftx = getMainWindow()->getLeftX();
	cairo_glyph_t glyph;
	if (m_up) {
		glyph.y = Y_POS_STAFF_REL(m_start_y - TUPLET_MARKER_DIST_UP);
	}
	else {
		glyph.y = Y_POS_STAFF_REL(m_start_y + TUPLET_MARKER_DIST_DOWN);
	}
	if (m_tuplet_val > 9) {
		glyph.index = NUMBERBASE + 1;
		glyph.x = X_POS_PAGE_REL(m_start_x + (m_end_x - m_start_x) / 2.0 - TRIPLET_VAL_SHIFT_X - FIRST_TUPLET_DIGIT_DIST);
		cairo_show_glyphs(cr, &glyph, 1);
		val -= 10;
	}
	glyph.index = NUMBERBASE + val;
	glyph.x = X_POS_PAGE_REL(m_start_x + (m_end_x - m_start_x) / 2.0 - TRIPLET_VAL_SHIFT_X);
	cairo_show_glyphs(cr, &glyph, 1);
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * TUPLET_LINE_THICK);
	if (m_up) {
		cairo_move_to(cr, X_POS_PAGE_REL(m_start_x), Y_POS_STAFF_REL(m_start_y));
		cairo_line_to(cr, X_POS_PAGE_REL(m_start_x), Y_POS_STAFF_REL(m_start_y - TUPLET_BRACKET_HEIGHT));
		cairo_line_to(cr, X_POS_PAGE_REL(m_end_x), Y_POS_STAFF_REL(m_end_y - TUPLET_BRACKET_HEIGHT));
		cairo_line_to(cr, X_POS_PAGE_REL(m_end_x), Y_POS_STAFF_REL(m_end_y));
	}
	else {
		cairo_move_to(cr, X_POS_PAGE_REL(m_start_x), Y_POS_STAFF_REL(m_start_y));
		cairo_line_to(cr, X_POS_PAGE_REL(m_start_x), Y_POS_STAFF_REL(m_start_y + TUPLET_BRACKET_HEIGHT));
		cairo_line_to(cr, X_POS_PAGE_REL(m_end_x), Y_POS_STAFF_REL(m_end_y + TUPLET_BRACKET_HEIGHT));
		cairo_line_to(cr, X_POS_PAGE_REL(m_end_x), Y_POS_STAFF_REL(m_end_y));
	}
	cairo_stroke(cr);
}

unsigned long long NedTuplet::getMidiStartTime() {
	return ((NedChordOrRest *) g_list_first(m_chords)->data)->getMidiTime();
}

NedChordOrRest *NedTuplet::getFirstElement() {
	return (NedChordOrRest *) g_list_first(m_chords)->data;
}

NedChordOrRest *NedTuplet::getLastElement() {
	return (NedChordOrRest *) g_list_last(m_chords)->data;
}

unsigned int NedTuplet::getDuration() {
	return (((NedChordOrRest *) g_list_last(m_chords)->data)->getMidiTime() + ((NedChordOrRest *) g_list_last(m_chords)->data)->getDuration() -
		((NedChordOrRest *) g_list_first(m_chords)->data)->getMidiTime());
}

void NedTuplet::saveTuplet(FILE *fp) {
	GList *lptr;
	int marker;

	fprintf(fp, "( %d ;", m_tuplet_val);
	for(lptr = g_list_first(m_chords); lptr; lptr = g_list_next(lptr)) {
		marker = NedResource::getMarkerOfAddress(lptr->data);
		fprintf(fp, "%d ", marker);
		if (lptr != g_list_last(m_chords)) {
			fprintf(fp, " , ");
		}
	}
	fprintf(fp, ")\n");
}
