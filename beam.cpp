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

#include "beam.h"
#include "staff.h"
#include "system.h"
#include "mainwindow.h"
#include "page.h"
#include "system.h"
#include "voice.h"
#include "chordorrest.h"
#include "note.h"

#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getStaff()->getBottomPos()) * zoom_factor - topy)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_STAFF_REL(p) ((height - ((p) + getSystem()->getYPos() + getStaff()->getBottomPos())) * PS_ZOOM)

double NedBeam::m_slopes[SLOPE_COUNT]  = {-0.5, -0.3420104, -0.1736431084, 0.0, 0.1736431084, 0.3420104, 0.5};

NedBeam::NedBeam(GList *beamed_chords_list) :
m_needed(TRUE), m_beamed_chords(beamed_chords_list), m_slope(0.0), m_start_y(10000.0), m_tuplet_marker(0), m_beam_count(0) {
	reconnect();
}

void NedBeam::freeChords() {
	GList *lptr;
	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setBeam(NULL);
	}
}

void NedBeam::reconnect() {
	GList *lptr;
	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setBeam(this);
	}
}

void NedBeam::insertChord(NedChordOrRest *chord, int newpos) {
	m_beamed_chords = g_list_insert(m_beamed_chords, chord, newpos);
	chord->setBeam(this);
	//computeBeam();
}

int NedBeam::getPosition(NedChordOrRest *chord) {
	int pos;
	if ((pos = g_list_index(m_beamed_chords, chord)) < 0) {
		NedResource::Abort("NedBeam::getPosition");
	}
	return pos;
}

void NedBeam::removeChord(NedChordOrRest *chord) {
	GList *lptr = g_list_find(m_beamed_chords, chord);
	if (lptr == NULL) {
		NedResource::Abort("NedBeam::removeChord");
	}
	m_beamed_chords = g_list_delete_link(m_beamed_chords, lptr);
}

int NedBeam::getChordCount() {
	return g_list_length(m_beamed_chords);
}


bool NedBeam::isEqual(GList *beamlist, NedChordOrRest *except, int *diff_pos) {
	return NedResource::equalContent(m_beamed_chords, beamlist, except, diff_pos);
}

bool NedBeam::isFirstInBeam(NedChordOrRest *chord) {
	return (g_list_first(m_beamed_chords)->data == chord);
}

bool NedBeam::isLastInBeam(NedChordOrRest *chord) {
	return (g_list_last(m_beamed_chords)->data == chord);
}

NedChordOrRest *NedBeam::getFirst() {
	return (NedChordOrRest *) g_list_first(m_beamed_chords)->data;
}

NedChordOrRest *NedBeam::getLast() {
	return (NedChordOrRest *) g_list_last(m_beamed_chords)->data;
}

	

void NedBeam::computeBeam(int staff_voice_offs) {
	GList *lptr;
	int i;
	int slope_idx = 0, slope_down_idx = 0;
	double h, x;
	double sumdist, dist;
	double summ_mid_dist_down, summ_mid_dist_up = 0.0, middist;
	double endy_down = 0.0, slope_down = 0.0, starty_down = 0.0, startx_down = 0.0, endx_down = 0.0, last_sum_dist_down = 100000.0;
	double slope, starty, last_sum_dist = 100000.0;
	bool user_stem = false;
	bool predefined_up = false;
	int stem_dir;


	NedChordOrRest *first = (NedChordOrRest *) g_list_first(m_beamed_chords)->data;
	NedChordOrRest *last = (NedChordOrRest *) g_list_last(m_beamed_chords)->data;

	stem_dir = first->getStemDirectionOfTheMeasure(staff_voice_offs);

	user_stem = ((first->getStatus() & STAT_USER_STEMDIR) != 0);
	if (user_stem) {
		predefined_up = ((first->getStatus() & STAT_UP) != 0);
	}
	if (first->getFirstNote()->getNoteHead() == GUITAR_NOTE_STEM) {
			user_stem = true; predefined_up = false;
	}

	if (first->getType() == TYPE_GRACE) {
		switch (first->getLength()) {
			case GRACE_16: m_beam_count = 2; break;
			default: m_beam_count = 1; break;
		}
		computeGraceBeam(first, last);
		return;
	}
	m_start_x = first->getBeamXPos() - STEM_THICK / 2.0;
	m_end_x = last->getBeamXPos() + STEM_THICK / 2.0;

	m_up = TRUE;
	if (!user_stem || predefined_up) {
		for (i = 0; i < SLOPE_COUNT; i++) {
			starty = ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStemTop();
			slope = m_slopes[i];
			for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
				x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
				h = ((NedChordOrRest *) lptr->data)->getStemTop();
				if (h < starty - (x - m_start_x) * slope) {
					starty -= (starty - (x - m_start_x) * slope) - h;
				}
			}
			sumdist = 0.0;
			for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
				x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
				h = ((NedChordOrRest *) lptr->data)->getStemTop();
				dist = h - (starty - (x - m_start_x) * slope);
				if (dist < 0) dist = -dist;
#define QUAD
#ifdef QUAD
				sumdist += dist * dist;
#else
				sumdist += dist;
#endif
			}
			if (last_sum_dist > sumdist) {
				last_sum_dist = sumdist;
				m_start_y = starty;
				m_slope = slope;
				slope_idx = i;
			}
		}
		slope_idx += ((NedChordOrRest *) g_list_last(m_beamed_chords)->data)->getSlopeOffset();
		if (slope_idx < 0) slope_idx = 0;
		if (slope_idx >= SLOPE_COUNT) slope_idx = SLOPE_COUNT-1;
		m_slope = m_slopes[slope_idx];
	
		m_end_y = m_start_y - (m_end_x - m_start_x) * m_slope;
	}

	if (!user_stem || !predefined_up) {
		for (i = 0; i < SLOPE_COUNT; i++) {
			starty = ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStemBottom();
			slope = m_slopes[i];
			for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
				x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
				h = ((NedChordOrRest *) lptr->data)->getStemBottom();
				if (h > starty - (x - m_start_x) * slope) {
					starty += h - (starty - (x - m_start_x) * slope);
				}
			}
			sumdist = 0.0;
			for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
				x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
				h = ((NedChordOrRest *) lptr->data)->getStemBottom();
				dist = (starty - (x - m_start_x) * slope) - h;
				if (dist < 0)  dist = -dist;
#ifdef QUAD
				sumdist += dist * dist;
#else
				sumdist += dist;
#endif
			}
			if (last_sum_dist_down > sumdist) {
				last_sum_dist_down = sumdist;
				starty_down = starty;
				slope_down_idx = i;
			}
		}
		slope_down_idx += last->getSlopeOffset();
		if (slope_down_idx < 0) slope_down_idx = 0;
		if (slope_down_idx >= SLOPE_COUNT) slope_down_idx = SLOPE_COUNT-1;
		slope_down = m_slopes[slope_down_idx];
	}

	if (!user_stem || predefined_up) {

		summ_mid_dist_up = 0.0;
		middist = m_start_y - (-2* LINE_DIST);
		if (middist < 0.0) middist = -middist;
		summ_mid_dist_up += middist;

		middist = m_end_y - (-2* LINE_DIST);
		if (middist < 0.0) middist = -middist;
		summ_mid_dist_up += middist;
	}
	if (!user_stem || !predefined_up) {
		startx_down = first->getBeamXPos();
		endx_down = last->getBeamXPos();

		endy_down = starty_down - (endx_down - startx_down) * slope_down;

		summ_mid_dist_down = 0.0;
		middist = starty_down - (-2* LINE_DIST);
		if (middist < 0.0) middist = -middist;
		summ_mid_dist_down += middist;
	
		middist = endy_down - (-2* LINE_DIST);
		if (middist < 0.0) middist = -middist;
		summ_mid_dist_down += middist;
	}

	if (user_stem) {
		stem_dir = predefined_up ? STEM_DIR_UP : STEM_DIR_DOWN;
	}
		

	if ((stem_dir == STEM_DIR_NONE && summ_mid_dist_down < summ_mid_dist_up) || stem_dir == STEM_DIR_DOWN) {
		m_up = FALSE;
		m_start_y = starty_down;
		m_end_y = endy_down;
		m_slope = slope_down;
		m_start_x = startx_down;
		m_end_x = endx_down;
	}
	m_start_y += first->getBeamOffset();
	m_end_y += first->getBeamOffset();
	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->xPositNotes();
		((NedChordOrRest *) lptr->data)->computeBbox();
	}
	m_tuplet_x = m_start_x + (m_end_x - m_start_x) / 2.0 - NUMNBER_GLYPH_WIDTH /2.0 - TRIPLET_VAL_SHIFT_X;
	m_tuplet_y = m_start_y + (m_end_y - m_start_y) / 2.0;
}

double NedBeam::getBeamLength() {
	return m_end_x - m_start_x;
}

double NedBeam::getTopOfBeam() {
	return m_start_y < m_end_y ? m_start_y : m_end_y;
}

double NedBeam::getBottomOfBeam() {
	return m_start_y < m_end_y ? m_end_y : m_start_y;
}

void NedBeam::computeGraceBeam(NedChordOrRest *first, NedChordOrRest *last) {
	GList *lptr;
	int i;
	double h, x;
	double sumdist, dist;
	double slope, starty, last_sum_dist = 100000.0;


	m_start_x = first->getBeamXPos() - STEM_THICK / 2.0;
	m_end_x = last->getBeamXPos() + STEM_THICK / 2.0;

	m_up = TRUE;
	for (i = 0; i < SLOPE_COUNT; i++) {
		starty = ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStemTop();
		slope = m_slopes[i];
		for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
			x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
			h = ((NedChordOrRest *) lptr->data)->getStemTop();
			if (h < starty - (x - m_start_x) * slope) {
				starty -= (starty - (x - m_start_x) * slope) - h;
			}
		}
		sumdist = 0.0;
		for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
			x = ((NedChordOrRest *) lptr->data)->getBeamXPos();
			h = ((NedChordOrRest *) lptr->data)->getStemTop();
			dist = h - (starty - (x - m_start_x) * slope);
			if (dist < 0) dist = -dist;
#define QUAD
#ifdef QUAD
			sumdist += dist * dist;
#else
			sumdist += dist;
#endif
		}
		if (last_sum_dist > sumdist) {
			last_sum_dist = sumdist;
			m_start_y = starty;
			m_slope = slope;
		}
	}

	m_end_y = m_start_y - (m_end_x - m_start_x) * m_slope;

	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->xPositNotes();
		((NedChordOrRest *) lptr->data)->computeBbox();
	}
}

NedMainWindow *NedBeam::getMainWindow() {return ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStaff()->getSystem()->getPage()->getMainWindow();}

NedPage *NedBeam::getPage() {return ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStaff()->getSystem()->getPage();}
NedSystem *NedBeam::getSystem() {return ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStaff()->getSystem();}
NedStaff *NedBeam::getStaff() {return ((NedChordOrRest *) g_list_first(m_beamed_chords)->data)->getStaff();}

NedBeam::~NedBeam() {
	GList *lptr;
	NedChordOrRest *chord = (NedChordOrRest *) g_list_first(m_beamed_chords)->data;
	chord->getVoice()->removeBeam(this);
	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getBeam() == this) {
			((NedChordOrRest *) lptr->data)->setBeam(NULL);
			((NedChordOrRest *) lptr->data)->xPositNotes();
		}
	}
	g_list_free(m_beamed_chords);
}

void NedBeam::changeStemDir(int dir) {
	computeBeam(dir);
}

void NedBeam::print() {
	GList *lptr;
	for (lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		NedResource::DbgMsg(DBG_TESTING, "B: 0x%x\n", lptr->data);
	}
	NedResource::DbgMsg(DBG_TESTING, "############\n");
}

double NedBeam::getYPosAt(double xpos) {
	return  m_start_y - (xpos - m_start_x) * m_slope;
}

void NedBeam::setTupletMarker(int tuplet_marker, double *topy, double *boty) {
	double ypos;

	m_tuplet_marker = tuplet_marker;
	ypos = m_tuplet_y + (m_up ? -(NUMNBER_GLYPH_HEIGHT + TUPLET_MARKER_DIST) : (NUMNBER_GLYPH_HEIGHT_DOWN + TUPLET_MARKER_DIST));
	if (ypos < *topy) *topy = ypos;
	if (ypos > *boty) *boty = ypos;
}

void NedBeam::draw(cairo_t *cr) {
	if (m_beam_count > 0) {
		drawGraceBeam(cr);
		return;
	}
	if (getMainWindow()->doDrawPostscript()) {
		drawPostscript(cr);
		return;
	}
	int dir;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double leftx = getMainWindow()->getLeftX();
	GList *lptr1, *lptr2, *lptr0 = NULL, *last;
	NedChordOrRest *chord0= NULL, *chord1, *chord2, *lastchord;
	double xpos1, xpos2, ypos1, ypos2;
	double mid_xpos, mid_ypos;
	double lastxpos, lastypos;
	int tval;
	bool draw_half;
	cairo_glyph_t glyph;



	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * BEAM_THICK);
	dir = m_up ? 1 : -1;
	lptr1 = g_list_first(m_beamed_chords);
	last = g_list_last(m_beamed_chords);
	chord1 = (NedChordOrRest *) lptr1->data;
	lastchord = (NedChordOrRest *) last->data;
	xpos1 = chord1->getBeamXPos() - STEM_THICK / 2.0;
	ypos1 = m_start_y - (xpos1 - m_start_x) * m_slope;
	lastxpos = lastchord->getBeamXPos() + STEM_THICK / 2.0;
	lastypos = m_start_y - (lastxpos - m_start_x) * m_slope;
	cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1));
	cairo_line_to(cr, X_POS_PAGE_REL(lastxpos), Y_POS_STAFF_REL(lastypos));
	for (lptr2 = g_list_next(lptr1); lptr2; lptr0 = lptr1, lptr1 = lptr2, lptr2 = g_list_next(lptr2), chord0 = chord1, chord1 = chord2, xpos1 = xpos2, ypos1 = ypos2) {
		chord2 = (NedChordOrRest *) lptr2->data;
		xpos2 = m_up ? chord2->getBeamXPos() : chord2->getBeamXPos();
		ypos2 = m_start_y - (xpos2 - m_start_x) * m_slope;
		mid_xpos = xpos1 + (xpos2 - xpos1) / 2.0;
		mid_ypos = ypos1 + (ypos2 - ypos1) / 2.0;
		/*
		printf("chord0: %u, chord1: %u, chord2: %u\n", lptr0 ? chord0->getLength() / NOTE_32: 1111111,
			chord1->getLength() / NOTE_32, chord2->getLength() / NOTE_32); fflush(stdout);
			*/

		draw_half = (lptr0 == NULL || (chord0->getLength() != chord1->getLength() && !((chord0->getDotCount() == 1 && chord0->getLength() == (chord1->getLength() << 1)))));
		//if (lptr0 == NULL || chord0->getLength() != chord1->getLength() || chord1->getLength() >= chord2->getLength()) {
		  switch (chord1->getLength()) {
		    case NOTE_64:
			cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 3 * dir * FLAG_DIST));
			cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST));
		    case NOTE_32:
		    	if (draw_half || chord2->getLength() <= NOTE_32) {
				cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 2 * dir * FLAG_DIST));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST));
			}
		    case NOTE_16:
		    	if (draw_half || chord2->getLength() <= NOTE_16) {
				cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + dir * FLAG_DIST));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST));
			}
		  }
		//}
		draw_half = (lptr2 == last || (chord1->getDotCount() == 1 && chord1->getLength() == (chord2->getLength() << 1)));
		//if (lptr2 == last || chord2->getLength() >= chord1->getLength()) {
		   switch (chord2->getLength()) {
		     case NOTE_64:
		     	if (draw_half || chord1->getLength() <= NOTE_64) {
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 3 * dir * FLAG_DIST));
			}
		     case NOTE_32:
		     	if (draw_half || chord1->getLength() <= NOTE_32) {
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 2 * dir * FLAG_DIST));
			}
		     case NOTE_16:
		     	if (draw_half || chord1->getLength() <= NOTE_16) {
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + dir * FLAG_DIST));
			}
		   }
		//}
	}
	cairo_stroke(cr);
	cairo_new_path(cr);
	if (m_tuplet_marker > 0) {
		tval = m_tuplet_marker;
		glyph.y = Y_POS_STAFF_REL(m_tuplet_y + (m_up ? -(NUMNBER_GLYPH_HEIGHT_UP + TUPLET_MARKER_DIST) : (NUMNBER_GLYPH_HEIGHT_DOWN + TUPLET_MARKER_DIST)));
		if (m_tuplet_marker > 9) {
			glyph.index = NUMBERBASE + 1;
			glyph.x = X_POS_PAGE_REL(m_tuplet_x - FIRST_TUPLET_DIGIT_DIST);
			cairo_show_glyphs(cr, &glyph, 1);
			tval -= 10;
		}
		glyph.index = NUMBERBASE + tval;
		glyph.x = X_POS_PAGE_REL(m_tuplet_x);
		cairo_show_glyphs(cr, &glyph, 1);
	}
	cairo_stroke(cr);
}

void NedBeam::drawGraceBeam(cairo_t *cr) {
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double leftx = getMainWindow()->getLeftX();
	GList *lptr1, *last;
	NedChordOrRest *chord1, *lastchord;
	double xpos1, ypos1;
	double lastxpos, lastypos;
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * GRACE_BEAM_THICK);
	lptr1 = g_list_first(m_beamed_chords);
	last = g_list_last(m_beamed_chords);
	chord1 = (NedChordOrRest *) lptr1->data;
	lastchord = (NedChordOrRest *) last->data;
	xpos1 = chord1->getBeamXPos() - STEM_THICK / 2.0;
	ypos1 = m_start_y - (xpos1 - m_start_x) * m_slope;
	lastxpos = lastchord->getBeamXPos() + STEM_THICK / 2.0;
	lastypos = m_start_y - (lastxpos - m_start_x) * m_slope;
	cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1));
	cairo_line_to(cr, X_POS_PAGE_REL(lastxpos), Y_POS_STAFF_REL(lastypos));
	if (m_beam_count > 1) {
		ypos1 += GRACE_BEAM_DIST;
		lastypos += GRACE_BEAM_DIST;
		cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1));
		cairo_line_to(cr, X_POS_PAGE_REL(lastxpos), Y_POS_STAFF_REL(lastypos));
	}
	cairo_stroke(cr);
}

void NedBeam::drawPostscript(cairo_t *cr) {
	int dir;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double leftx = getMainWindow()->getLeftX();
	GList *lptr1, *lptr2, *lptr0 = NULL, *last;
	NedChordOrRest *chord0 = NULL, *chord1, *chord2, *lastchord;
	double xpos1, xpos2, ypos1, ypos2;
	double mid_xpos, mid_ypos;
	double lastxpos, lastypos;
	int tval;
	bool draw_half;
	cairo_glyph_t glyph;
	double h_2 = BEAM_THICK / 2.0;



	cairo_new_path(cr);
	cairo_set_line_width(cr, 0.1);
	//cairo_set_line_width(cr, zoom_factor * BEAM_THICK);
	dir = m_up ? 1 : -1;
	lptr1 = g_list_first(m_beamed_chords);
	last = g_list_last(m_beamed_chords);
	chord1 = (NedChordOrRest *) lptr1->data;
	lastchord = (NedChordOrRest *) last->data;
	xpos1 = chord1->getBeamXPos() - STEM_THICK / 2.0;
	ypos1 = m_start_y - (xpos1 - m_start_x) * m_slope;
	lastxpos = lastchord->getBeamXPos() + STEM_THICK / 2.0;
	lastypos = m_start_y - (lastxpos - m_start_x) * m_slope;
	cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 - h_2));
	cairo_line_to(cr, X_POS_PAGE_REL(lastxpos), Y_POS_STAFF_REL(lastypos - h_2));
	cairo_line_to(cr, X_POS_PAGE_REL(lastxpos), Y_POS_STAFF_REL(lastypos + h_2));
	cairo_line_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + h_2));
	cairo_close_path(cr);
	cairo_fill(cr);
	for (lptr2 = g_list_next(lptr1); lptr2; lptr0 = lptr1, lptr1 = lptr2, lptr2 = g_list_next(lptr2), chord0 = chord1, chord1 = chord2, xpos1 = xpos2, ypos1 = ypos2) {
		chord2 = (NedChordOrRest *) lptr2->data;
		xpos2 = m_up ? chord2->getBeamXPos() : chord2->getBeamXPos();
		ypos2 = m_start_y - (xpos2 - m_start_x) * m_slope;
		mid_xpos = xpos1 + (xpos2 - xpos1) / 2.0;
		mid_ypos = ypos1 + (ypos2 - ypos1) / 2.0;
		draw_half = (lptr0 == NULL || (chord0->getLength() != chord1->getLength() && !((chord0->getDotCount() == 1 && chord0->getLength() == (chord1->getLength() << 1)))));
		switch (chord1->getLength()) {
		  case NOTE_64:
			cairo_new_path(cr);
			cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 3 * dir * FLAG_DIST - h_2));
			cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST - h_2));
			cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST + h_2));
			cairo_line_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 3 * dir * FLAG_DIST + h_2));
			cairo_close_path(cr);
			cairo_fill(cr);
		  case NOTE_32:
			if (draw_half || chord2->getLength() <= NOTE_32) {
				cairo_new_path(cr);
				cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 2 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST + h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + 2 * dir * FLAG_DIST + h_2));
				cairo_close_path(cr);
				cairo_fill(cr);
			}
		  case NOTE_16:
			if (draw_half || chord2->getLength() <= NOTE_16) {
				cairo_new_path(cr);
				cairo_move_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST + h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos1), Y_POS_STAFF_REL(ypos1 + dir * FLAG_DIST + h_2));
				cairo_close_path(cr);
				cairo_fill(cr);
			}
		}
		draw_half = (lptr2 == last || (chord1->getDotCount() == 1 && chord1->getLength() == (chord2->getLength() << 1)));
		//if (lptr2 == last || chord2->getLength() >= chord1->getLength()) {
		   switch (chord2->getLength()) {
		     case NOTE_64:
			if (draw_half || chord1->getLength() <= NOTE_64) {
				cairo_new_path(cr);
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 3 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 3 * dir * FLAG_DIST + h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 3 * dir * FLAG_DIST + h_2));
				cairo_close_path(cr);
				cairo_fill(cr);
			}
		     case NOTE_32:
			if (draw_half || chord1->getLength() <= NOTE_32) {
				cairo_new_path(cr);
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 2 * dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + 2 * dir * FLAG_DIST + h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + 2 * dir * FLAG_DIST + h_2));
				cairo_close_path(cr);
				cairo_fill(cr);
			}
		     case NOTE_16:
			if (draw_half || chord1->getLength() <= NOTE_16) {
				cairo_new_path(cr);
				cairo_move_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + dir * FLAG_DIST - h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(xpos2), Y_POS_STAFF_REL(ypos2 + dir * FLAG_DIST + h_2));
				cairo_line_to(cr, X_POS_PAGE_REL(mid_xpos), Y_POS_STAFF_REL(mid_ypos + dir * FLAG_DIST + h_2));
				cairo_close_path(cr);
				cairo_fill(cr);
			}
		   //}
		}
	}
	cairo_stroke(cr);
	cairo_new_path(cr);
	if (m_tuplet_marker > 0) {
		tval = m_tuplet_marker;
		glyph.y = Y_POS_STAFF_REL(m_tuplet_y + (m_up ? -(NUMNBER_GLYPH_HEIGHT_UP + TUPLET_MARKER_DIST) : (NUMNBER_GLYPH_HEIGHT_DOWN + TUPLET_MARKER_DIST)));
		if (m_tuplet_marker > 9) {
			glyph.index = NUMBERBASE + 1;
			glyph.x = X_POS_PAGE_REL(m_tuplet_x - FIRST_TUPLET_DIGIT_DIST);
			cairo_show_glyphs(cr, &glyph, 1);
			tval -= 10;
		}
		glyph.index = NUMBERBASE + tval;
		glyph.x = X_POS_PAGE_REL(m_tuplet_x);
		cairo_show_glyphs(cr, &glyph, 1);
	}
	cairo_stroke(cr);
}

void NedBeam::saveBeam(FILE *fp) {
	GList *lptr;
	int marker;

	fprintf(fp, "(");
	if (m_up) {
		fprintf(fp, "u ");
	}
	else {
		fprintf(fp, "d ");
	}
	for(lptr = g_list_first(m_beamed_chords); lptr; lptr = g_list_next(lptr)) {
		marker = NedResource::getMarkerOfAddress(lptr->data);
		fprintf(fp, "%d ", marker);
		if (lptr != g_list_last(m_beamed_chords)) {
			fprintf(fp, " , ");
		}
	}
	fprintf(fp, ")\n");
}

