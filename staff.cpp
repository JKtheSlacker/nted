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

#include <string.h>
#include "staff.h"
#include "voice.h"
#include "chordorrest.h"
#include "system.h"
#include "page.h"
#include "system.h"
#include "mainwindow.h"
#include "clipboard.h"
#include "pangocairotext.h"

#define DEFAULT_STAFF_DIST (4 * LINE_DIST)

#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_SYS_REL(p) (((p) + getSystem()->getYPos())  * zoom_factor - topy)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_SYS_REL(p) ((height - ((p) + getSystem()->getYPos())) * PS_ZOOM)

#define Y_POS_INVERS_STAFF_REL(p) ((topy + (p) / current_scale) / zoom_factor - (getSystem()->getYPos() + getBottomPos()))
#define X_POS_INVERS_STAFF_REL(p) (((p) / current_scale + leftx) / zoom_factor - (getPage()->getContentXpos()))
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getBottomPos()) * zoom_factor - topy)

const char NedStaff::m_sharpPos[7][7] = {
	{8, 5, 9, 6, 3, 7, 4}, {6, 3, 7, 4, 1, 5, 2}, {7, 4, 8, 5, 2, 6, 3}, {3, 0, 4, 1, 5, 2, 6},  {9, 6, 3, 7, 4, 8, 5}, {8, 5, 9, 6, 3, 7, 4}, {6, 3, 7, 4, 1, 5, 2} };
const char NedStaff::m_flatPos[7][7] = {
	{4, 7, 3, 6, 2, 5, 1}, {2, 5, 1, 4, 0, 3, -1}, {3, 6, 2, 5, 1, 4, 0}, {6, 2, 5, 1, 4, 0, 3}, {5, 8, 4, 7, 3, 6, 2}, {4, 7, 3, 6, 2, 5, 1}, {2, 5, 1, 4, 0, 3, -1} };

NedStaff::NedStaff(NedSystem *system, double ypos, double width, int nr, bool start) :
m_staffelems(NULL), m_ypos(ypos), m_width(width), m_top_y_border(0), m_lyrics_lines(0), m_bottom_y_border(4 * LINE_DIST), m_staff_number(nr), m_clef_type(TREBLE_CLEF),
m_clef_octave_shift(0), m_keysig(0), m_volume_changes(NULL), m_system(system) {
	int i;

	if (start) {
		for (i = 0; i < VOICE_COUNT; i++) {
			m_voices[i] = new NedVoice(this, i, TRUE);	
		}
#ifdef TTT
		if (nr == 0 && getSystem()->getSystemNumber() == 0) {
			m_staffelems = g_list_append(m_staffelems, new NedChordOrRest(m_voices[0], TYPE_KEYSIG, -3, (6 * 4 + 1) * NOTE_4));
			m_staffelems = g_list_append(m_staffelems, new NedChordOrRest(m_voices[0], TYPE_KEYSIG,  3,      8 * 4 * NOTE_4));
		}
#endif
	}
}

NedStaff::~NedStaff() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		delete m_voices[i];
	}
}

NedStaff *NedStaff::clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedSystem *p_system, int staff_number) {
	GList * lptr;
	int i;
	NedStaff *staff = new NedStaff(p_system, m_ypos, m_width, staff_number, false);

	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		staff->m_staffelems = g_list_append(staff->m_staffelems, ((NedChordOrRest *) lptr->data)->clone(NULL, NULL, m_voices[0]));
	}

	for (i = 0; i < VOICE_COUNT; i++) {
		staff->m_voices[i] = m_voices[i]->clone(addrlist, slurlist, staff);
	}

	return staff;
}

void NedStaff::adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->adjust_pointers(addrlist, slurlist);
	}
}


void NedStaff::appendWholeRest(NedCommandList *command_list /* = NULL */) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->appendWholeRest(command_list);
	}
}


void NedStaff::setUpbeatRests(unsigned int upbeat_inverse) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->setUpbeatRests(upbeat_inverse);
	}
}


void NedStaff::appendAppropriateWholes(int start_measure) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->appendAppropriateWholes(start_measure);
	}
}

void NedStaff::setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, bool first) {
	GList *lptr;
	m_clef_type = clef_and_key_array[3 * m_staff_number + 0];
	m_clef_octave_shift = clef_and_key_array[3 * m_staff_number + 1];
		
	m_keysig = clef_and_key_array[3 * m_staff_number + 2];
	double staff_start;
	int k;
	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_CLEF) {
			if (((NedChordOrRest *) lptr->data)->getMidiTime() == 0) {
				m_clef_type = ((NedChordOrRest *) lptr->data)->getLength();
				m_clef_octave_shift = ((NedChordOrRest *) lptr->data)->getDotCount();
			}
			clef_and_key_array[3 * m_staff_number + 0] = ((NedChordOrRest *) lptr->data)->getLength(); // clef number
			clef_and_key_array[3 * m_staff_number + 1] = ((NedChordOrRest *) lptr->data)->getDotCount(); // octave shift
		}
		else if (((NedChordOrRest *) lptr->data)->getType() == TYPE_KEYSIG) {
			((NedChordOrRest *) lptr->data)->setDotCount(clef_and_key_array[3 * m_staff_number + 2]);
			((NedChordOrRest *) lptr->data)->computeBbox();
			clef_and_key_array[3 * m_staff_number + 2] = ((NedChordOrRest *) lptr->data)->getLength(); // keysig value 
		}
	}
	k = (m_keysig < 0) ? -m_keysig : m_keysig;
	staff_start = CLEF_SPACE + k * SIG_X_SHIFT + 2 * EXTRA_KEY_OFF;
	if (first) {
		staff_start += TIME_SIGNATURE_SPACE;
	}
	if (m_system->m_system_start < staff_start) {
		m_system->m_system_start = staff_start;
	}
}

void NedStaff::determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->determineTempoInverse(element, till, tempoinverse, found);
	}
}


GList *NedStaff::getFirstChordOrRest(int voice_nr, int lyrics_line, bool lyrics_required, bool note_required) {
	if (voice_nr < 0 || voice_nr >= VOICE_COUNT) {
		NedResource::Abort("NedStaff::getFirstChordOrRest");
	}
	return m_voices[voice_nr]->getFirstChordOrRest(lyrics_line, lyrics_required, note_required);
}

GList *NedStaff::getLastChordOrRest(int voice_nr, int lyrics_line, bool lyrics_required, bool note_required) {
	if (voice_nr < 0 || voice_nr >= VOICE_COUNT) {
		NedResource::Abort("NedStaff::getLastChordOrRest");
	}
	return m_voices[voice_nr]->getLastChordOrRest(lyrics_line, lyrics_required, note_required);
}



void NedStaff::collectSelectionRectangleElements(double xp, double yp, NedBbox *sel_rect, GList **sel_group,
		bool is_first_selected, bool is_last_selected) {
	int i;
	if (is_first_selected && m_ypos + 4 * LINE_DIST < yp) return;
	if (is_last_selected && yp + sel_rect->height < m_ypos)  return;
	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->collectSelectionRectangleElements(xp, yp, sel_rect, sel_group, is_first_selected, is_last_selected);
	}
}

bool NedStaff::hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, unsigned int long long midi_time) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (m_voices[i]->hasTupletConflict(meas_duration, elements, from_staff, midi_time)) {
		 	return TRUE;
		}
	}
	return FALSE;
}

void NedStaff::pasteElements(NedCommandList *command_list, GList **elements, int from_staff, unsigned long long start_midi_time) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		 m_voices[i]->pasteElements(command_list, elements, from_staff, start_midi_time);
	}
}

bool NedStaff::findFromTo(GList *clipboard) {
	int i;
	bool found = FALSE;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (m_voices[i]->findFromTo(clipboard)) {
		 	found = TRUE;
		}
	}
	return found;
}

bool NedStaff::findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi) {
	int i;
	bool found = FALSE, fnd;
	for (i = 0; i < VOICE_COUNT; i++) {
		fnd = m_voices[i]->findStartMeasureLimits(clipboard, start_midi);
		found = found || fnd;
	}
	return found;
}

bool NedStaff::findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi) {
	int i;
	bool found = FALSE, fnd;
	for (i = 0; i < VOICE_COUNT; i++) {
		fnd = m_voices[i]->findEndMeasureLimits(clipboard, end_midi);
		found = found || fnd;
	}
	return found;
}

void NedStaff::deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->deleteItemsFromTo(command_list, is_first, is_last, start_midi, end_midi);
	}
}

void NedStaff::removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->removeNotesFromTo(command_list, items, is_first, is_last);
	}
}

void NedStaff::insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->insertBlocks(command_list, blockcount, midi_time);
	}
}

void NedStaff::testForPageBackwardTies(NedCommandList *command_list) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->testForPageBackwardTies(command_list);
	}
}

void NedStaff::checkForElementsToSplit(NedCommandList *command_list) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->checkForElementsToSplit(command_list);
	}
}

void NedStaff::findSelectedStaffs(double yp, NedBbox *sel_rect, int *number_of_first_selected_staff, int *number_of_last_selected_staff) {

	if (m_ypos < yp || m_ypos + 4 * LINE_DIST > yp + sel_rect->height) return;
	if (*number_of_first_selected_staff < 0) {
		*number_of_first_selected_staff = *number_of_last_selected_staff = m_staff_number;
/*
		printf("num = %d, sys_m_ypos = %f m_ypos = %f, m_ypos - 4 * LINE_DIST = %f, sys_m_ypos + m_ypos = %f yp = %f, yp + 4 * LINE_DIST = %f\n",
			m_staff_number, m_system->getYPos(),m_ypos, m_ypos - 4 * LINE_DIST, m_system->getYPos() + m_ypos,  yp, yp + 4 * LINE_DIST); fflush(stdout);
*/
		return;
	}
	*number_of_last_selected_staff = m_staff_number;
}


NedMainWindow *NedStaff::getMainWindow() {return m_system->getPage()->getMainWindow();}
NedPage *NedStaff::getPage() {return m_system->getPage();}

double NedStaff::getSystemEnd() {
	return getPage()->getContentWidth() + TIE_X_RIGHT_OVER;
}

double NedStaff::getSystemStart() {
	return getSystem()->m_system_start - TIE_X_OFFS;
}

GList *NedStaff::getStaffElements(unsigned long long midi_time) {
	GList *lptr, *elems = NULL;

	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() == midi_time) {
			elems = g_list_append(elems, lptr->data);
		}
	}

	return elems;
}


void NedStaff::draw(cairo_t *cr, double botpos, bool first_page, double indent, bool *freetexts_or_lyrics_present) {
	GList *lptr;
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double clef_y_offs = 0.0;
	bool oct_shift = false;
	double oct_shift_x_offs = 0.0, oct_shift_y_offs = 0.0;
	int zoom_level = getMainWindow()->getCurrentZoomLevel();
	double sig_xpos;
	bool dummy = false;
	int i;

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, zoom_factor *LINE_THICK);
	for (i = 0; i < 5; i++) {
		cairo_move_to(cr, X_POS_PAGE_REL(indent), Y_POS_SYS_REL(m_ypos + i*LINE_DIST));
		cairo_line_to(cr, X_POS_PAGE_REL(m_width), Y_POS_SYS_REL(m_ypos + i*LINE_DIST));
	}
	cairo_stroke(cr);
//#define DRAW_TOP_BOTTOM_BORDERS
#ifdef DRAW_TOP_BOTTOM_BORDERS
	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	//NedResource::DbgMsg(DBG_TESTING, "(4)topy ist %f == %d, boty ist %f == %d\n", m_top_y_border, (int) ( m_top_y_border / LINE_DIST), m_bottom_y_border,  (int) (m_bottom_y_border / LINE_DIST));
	cairo_move_to(cr, X_POS_PAGE_REL(0), Y_POS_SYS_REL(m_ypos + m_top_y_border));
	cairo_line_to(cr, X_POS_PAGE_REL(m_width), Y_POS_SYS_REL(m_ypos + m_top_y_border));
	cairo_stroke(cr);
	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
	cairo_move_to(cr, X_POS_PAGE_REL(0), Y_POS_SYS_REL(m_ypos + m_bottom_y_border));
	cairo_line_to(cr, X_POS_PAGE_REL(m_width), Y_POS_SYS_REL(m_ypos + m_bottom_y_border));
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
#endif
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getBottomPos()) * zoom_factor - topy)
	cairo_glyph_t glyph[2];
	cairo_new_path(cr);
	switch (m_clef_type) {
		case TREBLE_CLEF: glyph[0].index = BASE + 2;
				  switch (m_clef_octave_shift) {
				  	case  12: oct_shift_y_offs = OCTUP_Y_TREBLE_DIST_UP; oct_shift = true; break;
				  	case -12: oct_shift_y_offs = OCTUP_Y_TREBLE_DIST_DOWN; oct_shift = true; break;
				  }
				break;
		case ALTO_CLEF: glyph[0].index = BASE + 1;
				  switch (m_clef_octave_shift) {
				  	case  12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_UP;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  	case -12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_DOWN;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  }
				break;
		case SOPRAN_CLEF: glyph[0].index = BASE + 1;
				  switch (m_clef_octave_shift) {
				  	case  12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_UP;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  	case -12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_DOWN;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  }
				  clef_y_offs = 2 * LINE_DIST;
				break;
		case TENOR_CLEF: glyph[0].index = BASE + 1;
				  switch (m_clef_octave_shift) {
				  	case  12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_UP;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  	case -12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_DOWN;
						 oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 oct_shift = true; break;
				  }
				clef_y_offs = -LINE_DIST;break;
		case NEUTRAL_CLEF1: 
		case NEUTRAL_CLEF2: 
		case NEUTRAL_CLEF3: glyph[0].index = BASE + 27;
				break;
		default: glyph[0].index = BASE + 3;
				  switch (m_clef_octave_shift) {
				  	case  12: oct_shift_y_offs = OCTUP_Y_BASS_DIST_UP; oct_shift = true; break;
				  	case -12: oct_shift_y_offs = OCTUP_Y_BASS_DIST_DOWN; oct_shift = true; break;
				  }
				break;
	}
	glyph[0].x = X_POS_PAGE_REL(CLEF_POS + indent);
	glyph[0].y = Y_POS_STAFF_REL(clef_y_offs + -3 * LINE_DIST/2.0);
	cairo_show_glyphs(cr, glyph, 1);
	if (oct_shift) {
		glyph[0].index = BASE + 28;
		glyph[0].x = X_POS_PAGE_REL(CLEF_POS + indent + oct_shift_x_offs);
		glyph[0].y = Y_POS_STAFF_REL(-3 * LINE_DIST/2.0 + clef_y_offs + oct_shift_y_offs);
		cairo_show_glyphs(cr, glyph, 1);
	}
	cairo_stroke(cr);
	sig_xpos = CLEF_SPACE + indent;
	if (m_keysig > 0) {
		cairo_new_path(cr);
		glyph[0].index = BASE - 1;
		for (i = 0; i < m_keysig; i++) {
			glyph[0].x = X_POS_PAGE_REL(sig_xpos);
			glyph[0].y = Y_POS_STAFF_REL(-m_sharpPos[m_clef_type][i] * LINE_DIST/2.0);
			sig_xpos += SIG_X_SHIFT;
			cairo_show_glyphs(cr, glyph, 1);
		}
		cairo_stroke(cr);
	}
	else if (m_keysig < 0) {
		cairo_new_path(cr);
		glyph[0].index = BASE + 16;
		for (i = 0; i > m_keysig; i--) {
			glyph[0].x = X_POS_PAGE_REL(sig_xpos);
			glyph[0].y = Y_POS_STAFF_REL(-m_flatPos[m_clef_type][-i] * LINE_DIST/2.0);
			sig_xpos += SIG_X_SHIFT;
			cairo_show_glyphs(cr, glyph, 1);
		}
		cairo_stroke(cr);
	}
	if (first_page || getMainWindow()->isTimsigChangingMeasure(getSystem()->getNumberOfFirstMeasure())) {
		switch (getMainWindow()->getTimesigSymbol(getSystem()->getNumberOfFirstMeasure())) {
			case TIME_SYMBOL_COMMON_TIME:
				glyph[0].index = BASE + 56;
				glyph[0].x = X_POS_PAGE_REL(sig_xpos);
				glyph[0].y = Y_POS_STAFF_REL(-3 * LINE_DIST/2.0);
				cairo_show_glyphs(cr, glyph, 1);
				cairo_stroke(cr);
				break;
			case TIME_SYMBOL_CUT_TIME:
				glyph[0].index = BASE + 57;
				glyph[0].x = X_POS_PAGE_REL(sig_xpos);
				glyph[0].y = Y_POS_STAFF_REL(-3 * LINE_DIST/2.0);
				cairo_show_glyphs(cr, glyph, 1);
				cairo_stroke(cr);
				break;
			default:
				int n = getMainWindow()->getNumerator(getSystem()->getNumberOfFirstMeasure());
				if (n > 9) {
					glyph[0].index = NUMBERBASE + n / 10;
					glyph[0].x =  X_POS_PAGE_REL(sig_xpos - SIG_HALF);
					glyph[0].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS);
					glyph[1].index = NUMBERBASE + n % 10;
					glyph[1].x =  X_POS_PAGE_REL(sig_xpos + SIG_HALF);
					glyph[1].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS);
					cairo_show_glyphs(cr, glyph, 2);
				}
				else {
			
					glyph[0].index = NUMBERBASE + n;
					glyph[0].x =  X_POS_PAGE_REL(sig_xpos);
					glyph[0].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS);
					cairo_show_glyphs(cr, glyph, 1);
				}
				n = getMainWindow()->getDenominator(getSystem()->getNumberOfFirstMeasure());
				if (n > 9) {
					glyph[0].index = NUMBERBASE + n / 10;
					glyph[0].x =  X_POS_PAGE_REL(sig_xpos - SIG_HALF);
					glyph[0].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS);
					glyph[1].index = NUMBERBASE + n % 10;
					glyph[1].x =  X_POS_PAGE_REL(sig_xpos + SIG_HALF);
					glyph[1].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS);
					cairo_show_glyphs(cr, glyph, 2);
				}
				else {
					glyph[0].index = NUMBERBASE + n;
					glyph[0].x =  X_POS_PAGE_REL(sig_xpos);
					glyph[0].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS);
					cairo_show_glyphs(cr, glyph, 1);
				}
				cairo_stroke(cr);
				break;
	    	} // switch
	}
	
	m_freetexts_or_lyrics_present = false;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->draw(cr, &m_freetexts_or_lyrics_present);

	}
	int mcount = getSystem()->getFullMeasureCount();
	bool rep_line_needed = getMainWindow()->needsARepLine(getSystem()->getMeasureNr(0)->getMeasureNumber());
	getSystem()->getMeasureNr(0)->draw(cr, SPECIAL_POSITION_START, m_system->m_system_start + REP_THICK_LINE_DIST - indent, getPage()->getContentXpos() + indent, getSystem()->getYPos(), m_ypos - LINE_THICK / 2.0, botpos + LINE_THICK / 2.0, getBottomPos(), leftx, topy, m_top_y_border, zoom_factor, zoom_level, rep_line_needed, getSystem()->getSpecialMeasuresAtEnd(), false);
	for (i = 1; i <= mcount; i++) {
		rep_line_needed = getMainWindow()->needsARepLine(getSystem()->getMeasureNr(i)->getMeasureNumber());
		getSystem()->getMeasureNr(i)->draw(cr, (i == mcount) ? SPECIAL_POSITION_END : SPECIAL_POSITION_NONE, m_width, getPage()->getContentXpos(),  getSystem()->getYPos(), m_ypos - LINE_THICK / 2.0, botpos + LINE_THICK / 2.0, getBottomPos(), leftx, topy, m_top_y_border, zoom_factor, zoom_level, rep_line_needed, getSystem()->getSpecialMeasuresAtEnd(), false);
	}
	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() == 0 && ((NedChordOrRest *) lptr->data)->getType() == TYPE_CLEF) continue;
		((NedChordOrRest *) lptr->data)->draw(cr, &dummy);
	}
	if (getMainWindow()->m_staff_contexts[m_staff_number].m_staff_name != NULL ||
		getMainWindow()->m_staff_contexts[m_staff_number].m_staff_short_name != NULL) {
			m_freetexts_or_lyrics_present = true;
	}
	if (m_freetexts_or_lyrics_present) *freetexts_or_lyrics_present = true;
#ifdef SHOW_MASURE_LENGTH
	int measure_count = getSystem()->getMeasureCount();
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	for (i = 0; i < getSystem()->getMeasureCount(); i++) {
		cairo_move_to(cr, X_POS_PAGE_REL(getSystem()->getMeasureNr(i)->start), Y_POS_SYS_REL(getTopPos() + 3 * LINE_DIST + 0.05));
		cairo_line_to(cr, X_POS_PAGE_REL(getSystem()->getMeasureNr(i)->start + getSystem()->getMeasureInfo(i)->width), Y_POS_SYS_REL(getTopPos() + 3 * LINE_DIST + 0.05));
	}
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
#endif
};

void NedStaff::drawTexts(cairo_t *cr, bool first_page, double scale) {
	int i;

	if (!m_freetexts_or_lyrics_present) return;

	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();

	if (first_page) {
		if (getMainWindow()->m_staff_contexts[m_staff_number].m_staff_name != NULL) {
		getMainWindow()->m_staff_contexts[m_staff_number].m_staff_name->draw(cr,
		((getPage()->getContentXpos()) * zoom_factor - leftx) * scale +
		(getMainWindow()->getNettoIndent() - getMainWindow()->m_staff_contexts[m_staff_number].m_staff_name->getInternalWidth()),
		 Y_POS_SYS_REL(m_ypos + 2 * LINE_DIST) * scale, zoom_factor, scale);
		}
	}
	else if (getMainWindow()->m_staff_contexts[m_staff_number].m_staff_short_name != NULL) {
		getMainWindow()->m_staff_contexts[m_staff_number].m_staff_short_name->draw(cr,
		((getPage()->getContentXpos()) * zoom_factor  - leftx) * scale +
		(getMainWindow()->get2ndNettoIndent() - getMainWindow()->m_staff_contexts[m_staff_number].m_staff_short_name->getInternalWidth()),
		 Y_POS_SYS_REL(m_ypos + 2 * LINE_DIST) * scale, zoom_factor, scale);
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->drawTexts(cr, scale);
	}
};

void NedStaff::convertStaffRel(double x, double y, double *x_trans, double *y_trans) {
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double current_scale = getMainWindow()->getCurrentScale();
	*y_trans = Y_POS_INVERS_STAFF_REL(y);
	*x_trans = X_POS_INVERS_STAFF_REL(x);
}
	
double NedStaff::getBottomPos() {
	return m_ypos + 4 * LINE_DIST;
}

double NedStaff::getMidPos() {
	return m_ypos + 2 * LINE_DIST;
}

double NedStaff::getHeight() {
	return 4 * LINE_DIST;
}

double NedStaff::getRealYPosOfLine(int line) {
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double topy = getMainWindow()->getTopY();

	return getMainWindow()->getCurrentScale() * Y_POS_SYS_REL(m_ypos + 4 * LINE_DIST + line * LINE_DIST / 2.0);
}

void NedStaff::empty() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->empty();
	}
}

bool NedStaff::hasOnlyRests() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (!m_voices[i]->hasOnlyRests()) return false;
	}
	return true;
}



void NedStaff::cutEmptyVoices() {
	int i, j;
	bool has_only_rests[VOICE_COUNT];
	NedVoice *tmp_voice;
	bool tmp_b;

	for (i = 0; i < VOICE_COUNT; i++) {
		has_only_rests[i] = m_voices[i]->hasOnlyRests();
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		if (has_only_rests[i]) {
			m_voices[i]->empty();
			m_voices[i]->appendAppropriateWholes(0);
		}
	}
		
	for (i = 0; i < VOICE_COUNT - 1; i++) {
		if (has_only_rests[i]) {
			for (j = i + 1; j < VOICE_COUNT; j++) {
				if (!has_only_rests[j]) {
					tmp_voice = m_voices[i];
					m_voices[i] = m_voices[j];
					m_voices[j] = tmp_voice;
					tmp_b = has_only_rests[i];
					has_only_rests[i] = has_only_rests[j];
					has_only_rests[j] = tmp_b;
					break;
				}
			}
		}
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->setVoiceNumber(i);
	}
}

void NedStaff::handleEmptyMeasures() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->handleEmptyMeasures();
	}
}

void NedStaff::zoomFreeReplaceables(double zoom, double scale) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->zoomFreeReplaceables(zoom, scale);
	}
}

void NedStaff::recomputeFreeReplaceables() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->recomputeFreeReplaceables();
	}
}

void NedStaff::testTies() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->testTies();
	}
}

bool NedStaff::trySelect(double x, double y, bool only_free_placeables) {
	int i;
	GList *lptr;
	for (lptr = g_list_first(m_staffelems); !only_free_placeables && lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->trySelect(x, y, false)) {
			return TRUE;
		}
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		if (m_voices[i]->trySelect(x, y, only_free_placeables)) {
			return TRUE;
		}
	}
	return FALSE;
}

NedChordOrRest *NedStaff::findNearestElement(double x, double y, NedStaff *oldstaff, double *mindist, double *ydist) {
	NedChordOrRest *nearestELement = NULL, *element;
#define HYSTERESIS 0.4
#ifdef ALL_NEAREST
	for (i = 0; i < VOICE_COUNT; i++) {
		if ((element = m_voices[i]->findNearestElement(x, y, mindist, ydist)) != NULL) {
			nearestELement = element;
		}
	}
#endif
	if ((element = m_voices[0]->findNearestElement(x, y, (oldstaff == this) ? HYSTERESIS : 1.0, mindist, ydist)) != NULL) {
		nearestELement = element;
	}
	return nearestELement;
}

bool NedStaff::tryChangeLength(NedChordOrRest *chord_or_rest) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (m_voices[i]->tryChangeLength(chord_or_rest)) {
			return TRUE;
		}
	}
	return FALSE;
}

void NedStaff::prepareForImport() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->prepareForImport();
	}
}

void NedStaff::removeLastImported() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->removeLastImported();
	}
}

bool NedStaff::truncateAtStart(NedCommandList *command_list, unsigned long long midi_time) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (!m_voices[i]->truncateAtStart(command_list, midi_time)) return false;
	}
	return true;
}

bool NedStaff::handleImportedTuplets() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (!m_voices[i]->handleImportedTuplets()) return false;
	}
	return true;
}

void NedStaff::handleStaffElements() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->handleStaffElements();
	}
}

void NedStaff::appendElementsOfMeasureLength(part *part_ptr, unsigned int meas_duration, int meas_num) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->appendElementsOfMeasureLength(part_ptr, meas_duration, meas_num);
	}
}

void NedStaff::copyDataOfWholeStaff() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->copyDataOfWholeVoice();
	}
}

bool NedStaff::tryInsertOrErease(double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj, bool force_rest) {
	return m_voices[getMainWindow()->getCurrentVoice()]->tryInsertOrErease(x, y, num_midi_input_notes, midi_input_chord, newObj, force_rest);
}

bool NedStaff::findLine(double x, double y, double *ypos, int *line, double *bottom) {
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	double yl = Y_POS_INVERS_STAFF_REL(y);
	double yoffs = (yl >= 0.0) ? LINE_DIST / 4.0 : -LINE_DIST / 4.0;
	*line = (int) ((yl + yoffs) / (LINE_DIST/2.0));
	if (*line < -(MAX_OVER_UNDER_LINES + 8) || *line > MAX_OVER_UNDER_LINES) return FALSE;
	*ypos = (Y_POS_STAFF_REL(*line * LINE_DIST/2.0) + yoffs);
	*bottom =  Y_POS_STAFF_REL(0);
	return TRUE;
}

bool NedStaff::findFirstElementeGreaterThan(double x, NedChordOrRest **element) {
	return m_voices[0]->findFirstElementeGreaterThan(x, element);
}

bool NedStaff::findLastElementeLessThan(double x, NedChordOrRest **element) {
	return m_voices[0]->findLastElementeLessThan(x, element);
}

bool NedStaff::hasStaffElem(unsigned long long miditime) {
	GList *lptr;

	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() == miditime) {
			return true;
		}
	}
	return false;
}

void NedStaff::computeBeams() {
	int i;
	//bool topFix = false, botFix = false;
	m_top_y_border = 0; m_bottom_y_border = 4 * LINE_DIST;
	m_lyrics_lines = 0;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->computeBeams(&m_lyrics_lines, &m_top_y_border, &m_bottom_y_border, /* &topFix, &botFix, */ m_staff_number * VOICE_COUNT + i);
		//if (topFix && botFix) break;
	}
}

void NedStaff::computeTies() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->computeTies();
	}
}

void NedStaff::computeTuplets(bool has_repeat_lines) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->computeTuplets(&m_top_y_border, &m_bottom_y_border);
	}
	if (has_repeat_lines) {
		if (m_top_y_border > -MIN_SEC_DIST) {
			m_top_y_border = -MIN_SEC_DIST;
		}
		m_top_y_border -= SEC_DIST_BELOW + SEC_DIST_ABOVE;
	}
}

double NedStaff::placeStaff(double staffpos) {
#define STAFF_MIN_DIST 2.0
#ifdef PLACE_OUT_DEBUG
	if (getSystem()->getSystemNumber()  < 2) {
	NedResource::DbgMsg(DBG_TESTING, "system %d staff %d: (1)staffpos = %f(%d)\n", getSystem()->getSystemNumber(), m_staff_number, staffpos, (int) (staffpos / (LINE_DIST / 2)));
	}
#endif
	if (m_top_y_border < 0) {
#ifdef PLACE_OUT_DEBUG2
		if (getSystem()->getSystemNumber()  == 0 && m_staff_number == 0) {
			NedResource::DbgMsg(DBG_TESTING, "erhoehe von %f auf %f\n", staffpos, staffpos -m_top_y_border);
			
		}
#endif
		staffpos += -m_top_y_border;
	}
	m_ypos = staffpos;
#ifdef PLACE_OUT_DEBUG
	if (getSystem()->getSystemNumber()  < 2) {
	NedResource::DbgMsg(DBG_TESTING, "(2)m_bottom_y_border = %f(%d), STAFF_MIN_DIST = %f(%d)\n", m_bottom_y_border, (int) (m_bottom_y_border / (LINE_DIST / 2)), STAFF_MIN_DIST, (int) (STAFF_MIN_DIST / (LINE_DIST / 2)));
	}
#endif
	double yy = (m_lyrics_lines == 0) ? m_bottom_y_border : (m_bottom_y_border + 5 * LINE_DIST +  TEXT_Y_OFFS + m_lyrics_lines * LYRICS_HEIGHT);
	if (yy < STAFF_MIN_DIST) {
		staffpos += STAFF_MIN_DIST;
	}
	else {
		staffpos += yy;
	}
#ifdef PLACE_OUT_DEBUG
	if (getSystem()->getSystemNumber()  < 2) {
	NedResource::DbgMsg(DBG_TESTING, "(3)staffpos = %f(%d)\n", staffpos, (int) (staffpos / (LINE_DIST / 2)));
	}
#endif

	return staffpos;
}

void NedStaff::insertStaffElement(NedChordOrRest *elem, bool redraw /* = true */) {
	m_staffelems = g_list_insert_sorted(m_staffelems, elem, (gint (*)(const void*, const void*)) NedChordOrRest::compareMidiTimesAndTypes);
	if (redraw) {
		getMainWindow()->setAndUpdateClefTypeAndKeySig();
		getMainWindow()->reposit(NULL, NULL, NULL);
		getMainWindow()->repaint();
	}
}

void NedStaff::removeStaffElement(NedChordOrRest *elem) {
	GList *lptr;

	if ((lptr = g_list_find(m_staffelems, elem)) == NULL) {
		NedResource::Abort("NedStaff::removeStaffElement");
	}
	m_staffelems = g_list_delete_link(m_staffelems, lptr);
	getMainWindow()->setAndUpdateClefTypeAndKeySig();
	getMainWindow()->reposit(NULL, NULL, NULL);
	getMainWindow()->repaint();
}

void NedStaff::assignMidiTimes() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->assignMidiTimes();
	}
}

void NedStaff::deleteStaffElements(bool unexecute, GList *elems, unsigned long long reftime, unsigned long duration) {
	GList *lptr1, *lptr2;

	for (lptr1 = g_list_first(elems); lptr1; lptr1 = g_list_next(lptr1)) {
		if ((lptr2 = g_list_find(m_staffelems, lptr1->data)) == NULL) {
			NedResource::Abort("NedStaff::deleteStaffElements");
		}
		if (!unexecute) {
		/*
			measure = m_system->getMeasure(((NedChordOrRest *) lptr2->data)->getMidiTime());
			measure = ((NedChordOrRest *) lptr2->data)->m_measure;
			if (measure == NULL) {
				NedResource::DbgMsg(DBG_TESTING, "measure = 0 fuer time %llu (%llu)\n", ((NedChordOrRest *) lptr2->data)->getMidiTime(),
					((NedChordOrRest *) lptr2->data)->getMidiTime() / NOTE_4); 
			}
			((NedChordOrRest *) lptr2->data)->decrMidiTime(measure->midi_start);
			*/
			((NedChordOrRest *) lptr2->data)->decrMidiTime(reftime);
		}
		m_staffelems = g_list_delete_link(m_staffelems, lptr2);
	}
	if (unexecute) { 
		for (lptr1 = g_list_first(m_staffelems); lptr1; lptr1 = g_list_next(lptr1)) {
			((NedChordOrRest *) lptr1->data)->decrMidiTime(duration);
		}
	}
			
}

void NedStaff::insertIntoStaff(GList *elems, unsigned long long duration) {
	GList *lptr;
	int pos = 0;
	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->incrMidiTime(duration);
	}
	for (lptr = g_list_first(elems); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setVoice(m_voices[0]);
		m_staffelems = g_list_insert(m_staffelems, lptr->data, pos++);
	}
}

void NedStaff::appendAtStaff(GList *elems) {
	unsigned long long dur_of_system;
	GList *lptr;
	if (elems == NULL) return;
	dur_of_system = m_system->getSystemEndTime();

	for (lptr = g_list_first(elems); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->incrMidiTime(dur_of_system);
		((NedChordOrRest *) lptr->data)->setVoice(m_voices[0]);
		m_staffelems = g_list_append(m_staffelems, lptr->data);
	}
}


void NedStaff::searchForBeamGroups(unsigned int midi_start_time) {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->searchForBeamGroups(midi_start_time);
	}
}

void NedStaff::collectChordsAndRests(NedClipBoard *board, unsigned long long midi_start, unsigned long long midi_end) {
	int i;
	GList *lptr;
	NedClipBoardElement *element = new NedClipBoardElement();
	element->from_staff = this;
	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() <= midi_start) continue;
		if (((NedChordOrRest *) lptr->data)->getMidiTime() > midi_end) break;
		element->chords_or_rests_to_move = g_list_append(element->chords_or_rests_to_move, lptr->data);
	}
	board->m_elements = g_list_append(board->m_elements, element);

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->collectChordsAndRests(board, midi_start, midi_end);
	}
}

void NedStaff::collectDestinationVoices(NedClipBoard *board) {
	int i;
	NedClipBoardElement *element;
	if (board->element_counter == NULL) {
		NedResource::Abort("NedStaff::collectDestinationVoices(1)");
	}
	element = ((NedClipBoardElement *) board->element_counter->data);
	if (element->from_staff == NULL) {
		NedResource::Abort("NedStaff::collectDestinationVoices(2)");
	}
	element->to_staff = this;
	board->element_counter = g_list_next(board->element_counter);
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->collectDestinationVoices(board);
	}
}

void NedStaff::getCurrentClefAndKeysig(unsigned long long miditime, int *clef, int *keysig, int *octave_shift) {
	GList *lptr;
	*clef = m_clef_type;
	*keysig = m_keysig;
	*octave_shift = m_clef_octave_shift;

	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() > miditime) break;
		if (((((NedChordOrRest *) lptr->data)->getType() & TYPE_CLEF) != 0))  {
			*clef = ((NedChordOrRest *) lptr->data)->getLength(); // clef number
			*octave_shift = ((NedChordOrRest *) lptr->data)->getDotCount(); //octave shift
		}
		else if (((((NedChordOrRest *) lptr->data)->getType() & TYPE_KEYSIG) != 0))  {
			*keysig = ((NedChordOrRest *) lptr->data)->getLength();
		}
	}
}


void NedStaff::resetActiveFlags() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->resetActiveFlags();
	}
}

void NedStaff::prepareReplay(bool with_keysig) {
	int i, j;
	GList *lptr;

	for (i = 0; i < VOICE_COUNT; i++) {
		if (getMainWindow()->m_staff_contexts[m_staff_number].m_muted[i]) {
			m_voices[i]->prepareMutedReplay();
		}
		else {
			m_voices[i]->prepareReplay(getMainWindow()->m_staff_contexts[m_staff_number].m_different_voices ? i : 0);
		}
	}

	if (with_keysig) {
		for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
			if ((((NedChordOrRest *) lptr->data)->getType() & TYPE_KEYSIG) == 0) continue;
			NedResource::addKeysigEntry(((NedChordOrRest *) lptr->data)->getMidiTime(), 
				getMainWindow()->m_staff_contexts[m_staff_number].m_midi_channel,
				((NedChordOrRest *) lptr->data)->getLength() /* keysig */);
		}
	}

}

void NedStaff::findAccidentals(char offs_array[115], NedMeasure *meas_info, unsigned long long  midi_time, bool including) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->findAccidentals(offs_array, meas_info, midi_time, including);
	}
}

void NedStaff::setInternalPitches() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->setInternalPitches();
	}
}

void NedStaff::adjustAccidentals() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->adjustAccidentals();
	}
}

void NedStaff::changeAccidentals(NedCommandList *command_list, int preferred_offs, GList *selected_group) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->setInternalPitch(0, selected_group);
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->changeAccidentals(command_list, preferred_offs, selected_group);
	}
	removeUnneededAccidentals(selected_group);
}


void NedStaff::transpose(int pitchdist, GList *selected_group) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->setInternalPitch(pitchdist, selected_group);
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->adjustAccidentals(pitchdist, selected_group);
	}
	removeUnneededAccidentals(selected_group);
}

void NedStaff::hideRests(NedCommandList *command_list, bool unhide, int voice_nr) {
	if (voice_nr < 0  || voice_nr >= VOICE_COUNT) {
		NedResource::Abort("NedStaff::hideRests");
	}

	m_voices[voice_nr]->hideRests(command_list, unhide);
}

bool NedStaff::shiftNotes(unsigned long long start_time, int linedist) {
	int i;
	GList *lptr;
	unsigned long long stop_time = 0;
	bool stop_at_clef = false;

	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() != TYPE_CLEF) continue;
		if (((NedChordOrRest *) lptr->data)->getMidiTime() <= start_time) continue;
		stop_time = ((NedChordOrRest *) lptr->data)->getMidiTime();
		stop_at_clef = true;
		break;
	}

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->shiftNotes(start_time, linedist, stop_time, stop_at_clef);
	}

	return !stop_at_clef;
}

void NedStaff::removeUnneededAccidentals(GList *selected_group /* = NULL */) {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i]->removeUnneededAccidentals(selected_group);
	}
}

bool NedStaff::detectVoices(unsigned int *voice_mask, unsigned long long *e_time) {
	int i;
	bool not_empty = false, ne;

	for (i = 0; i < VOICE_COUNT; i++) {
		ne = m_voices[i]->detectVoices(voice_mask, e_time);
		not_empty = not_empty || ne;
	}
	return not_empty;
}

void NedStaff::exportLilyPond(FILE *fp, int voice_nr, int *last_line, unsigned int *midi_len,
		bool last_system, unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur,
		unsigned int *lyrics_map, bool with_break, bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams) {
	if (voice_nr < 0 || voice_nr >= VOICE_COUNT) {
		NedResource::Abort("NedStaff::exportLilyPond");
	}
	m_voices[voice_nr]->exportLilyPond(fp, last_line, midi_len, last_system, end_time, in_alternative, lily_slur, lyrics_map, with_break, guitar_chordnames, chordnames, breath_script, keep_beams);
}

void NedStaff::exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len,
		bool last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	m_voices[0]->exportLilyGuitarChordnames(fp, last_line, midi_len, last_system, end_time, in_alternative, with_break);
}

void NedStaff::exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len,
		bool last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	m_voices[0]->exportLilyFreeChordName(fp, last_line, midi_len, last_system, end_time, in_alternative, with_break);
}

void NedStaff::exportLilyLyrics(FILE *fp, bool last_system, int voice_nr, int line_nr, unsigned long long end_time, int *sil_count) {
	if (voice_nr < 0 || voice_nr >= VOICE_COUNT) {
		NedResource::Abort("NedStaff::exportLilyLyrics");
	}
	m_voices[voice_nr]->exportLilyLyrics(fp, last_system, line_nr, end_time, sil_count);
}
	
void NedStaff::collectLyrics(NedLyricsEditor *leditor) {
	m_voices[0]->collectLyrics(leditor);
}

void NedStaff::setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor) {
	m_voices[0]->setLyrics(command_list, leditor);
}
	

int NedStaff::assignElementsToMeasures(NedMeasure *measures, int staff_offs, bool use_upbeat, int *special_measure_at_end, bool out /* = false */) {
	int mesure_count = 0, mc;
	NedMeasure *measure;
	NedChordOrRest *element;
	GList *lptr;
	int curr_measure = 0;
	bool last_measure_has_keysig_or_timesig = false;
	NedChordOrRest *last_keysig = NULL;
	for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		switch (element->getType()) {
			case TYPE_KEYSIG:  while (((NedChordOrRest *) lptr->data)->getMidiTime() >= measures[curr_measure].midi_end) {
							curr_measure++; // KEYSIG after bar
							last_keysig = (NedChordOrRest *) lptr->data;
							if (((NedChordOrRest *) lptr->data)->getMidiTime() == measures[curr_measure].midi_start) {
#define KEYSIG_EXTRA_WIDTH 0.2
								measures[curr_measure].m_staff_elements_width = ((NedChordOrRest *) lptr->data)->getBBox()->width + KEYSIG_EXTRA_WIDTH;
							}
				
				  }
				  break;
			default: while (((NedChordOrRest *) lptr->data)->getMidiTime() > measures[curr_measure].midi_end) {
					curr_measure++; // CLEF before bar
				  }
				  break;
		}
		measure = measures + curr_measure;
		//if (((NedChordOrRest *) lptr->data)->getMidiTime() > measures[curr_measure].midi_end) continue;
		//if (((NedChordOrRest *) lptr->data)->getMidiTime() == 0 && ((NedChordOrRest *) lptr->data)->getType() == TYPE_CLEF) continue;
		//if (((NedChordOrRest *) lptr->data)->getMidiTime() == 0 && (((NedChordOrRest *) lptr->data)->getType() & (TYPE_CLEF | TYPE_KEYSIG)) != 0) continue;
		measure->m_position_array.insertStaffElement(
			element, staff_offs / VOICE_COUNT, measure->getMeasureNumber());
		element->m_measure = measure;
		if (curr_measure > mesure_count) mesure_count = curr_measure;
	}
	if (last_keysig && last_keysig->getMidiTime() == measures[curr_measure].midi_start) {
		last_measure_has_keysig_or_timesig = true;
	}
	for (int i = 0; i < VOICE_COUNT; i++) {
		mc = m_voices[i]->assignElementsToMeasures(measures, staff_offs + i, use_upbeat);
		if (mc > mesure_count) {
			mesure_count = mc;
		}
		if (mc >= mesure_count) {
			last_measure_has_keysig_or_timesig = false;
		}
	}
	if (last_measure_has_keysig_or_timesig) { // the last measure isn't regulaer
		mesure_count--;
		*special_measure_at_end = 1;
	}
	else if ((measures[mesure_count+1].getSpecial() & TIMESIG_TYPE_MASK) == TIMESIG) {
		*special_measure_at_end = 1;
		last_measure_has_keysig_or_timesig = true;
	}
	return mesure_count;
}

double NedStaff::computeMidDist(double y) {
	double d;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();

	y /= getMainWindow()->getCurrentScale();

	d = Y_POS_SYS_REL(m_ypos + 3 * LINE_DIST / 2.0) - y;
	if (d < 0) return -d;
	return d;
}

void NedStaff::saveStaff(FILE *fp) {
	int i, ll;
	GList *lptr;
	fprintf(fp, "STAFF %d\n", m_staff_number);
	if (g_list_length(m_staffelems) > 0) {
		fprintf(fp, "STAFF_ELEMENTS : ");
		for (lptr = g_list_first(m_staffelems); lptr; lptr = g_list_next(lptr)) {
			fprintf(fp, "(");
			switch (((NedChordOrRest *) lptr->data)->getType()) {
				case TYPE_CLEF:
					switch (((NedChordOrRest *) lptr->data)->getLength()) {
						case TREBLE_CLEF: fprintf(fp, "TREBLE_CLEF, "); break;
						case BASS_CLEF: fprintf(fp, "BASS_CLEF, "); break;
						case ALTO_CLEF: fprintf(fp, "ALTO_CLEF, "); break;
						case SOPRAN_CLEF: fprintf(fp, "SOPRAN_CLEF, "); break;
						case TENOR_CLEF: fprintf(fp, "TENOR_CLEF, "); break;
						case NEUTRAL_CLEF1: fprintf(fp, "NEUTRAL_CLEF1, "); break;
						case NEUTRAL_CLEF2: fprintf(fp, "NEUTRAL_CLEF2, "); break;
						case NEUTRAL_CLEF3: fprintf(fp, "NEUTRAL_CLEF3, "); break;
					}
					fprintf(fp, " %d, ", ((NedChordOrRest *) lptr->data)->getDotCount()); // octave shift
					break;
				case TYPE_KEYSIG:
					ll = ((NedChordOrRest *) lptr->data)->getLength();
					fprintf(fp, "KEYSIG, %d, ", ll); 
					break;
				case TYPE_STAFF_SIGN:
					fprintf(fp, "STAFF_SIGN, %d, ", ((NedChordOrRest *) lptr->data)->getDotCount()); // sub_type
					break;
			}
			fprintf(fp, "%llu ) ", ((NedChordOrRest *) lptr->data)->getMidiTime());
		}
		putc('\n', fp);
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		fprintf(fp, "VOICE %d\n", i);
		m_voices[i]->saveVoice(fp);
	}
}

void NedStaff::restoreStaff(FILE *fp) {
	int i, voicenum;
	bool was_clef;
	int octave_shift;
	char buffer[128];
	int staff_elem_type, keysignum, staff_sign_sub_type;
	unsigned long long miditime;
	NedChordOrRest *staffelem;
	
	if (!NedResource::readWord(fp, buffer)) {
		NedResource::m_error_message = "VOICE or STAFF_ELEMENTS expected";
		return;
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_voices[i] = new NedVoice(this, i, FALSE);	
	}
	if (!strcmp(buffer, "STAFF_ELEMENTS")) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
			NedResource::m_error_message = ": expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "( or VOICE expected";
			return;
		}
		while (!strcmp(buffer, "(")) {
			if (!NedResource::readWordWithNum(fp, buffer)) {
				NedResource::m_error_message = "TREBLE_CLEF, BASS_CLEF, ALTO_CLEF, SOPRAN_CLEF, TENOR_CLEF, NEUTRAL_CLEF1, NEUTRAL_CLEF2, NEUTRAL_CLEF3, KEYSIG, or STAFF_SIGN  expected";
				return;
			}
			was_clef = false;
			octave_shift = 0;
			if (!strcmp(buffer, "TREBLE_CLEF")) {
				staff_elem_type = TREBLE_CLEF;
				was_clef = true;
			}
			else if (!strcmp(buffer, "BASS_CLEF")) {
				staff_elem_type = BASS_CLEF;
				was_clef = true;
			}
			else if (!strcmp(buffer, "ALTO_CLEF")) {
				staff_elem_type = ALTO_CLEF;
				was_clef = true;
			}
			else if (!strcmp(buffer, "SOPRAN_CLEF")) {
				staff_elem_type = SOPRAN_CLEF;
				was_clef = true;
			}
			else if (!strcmp(buffer, "TENOR_CLEF")) {
				staff_elem_type = TENOR_CLEF;
				was_clef = true;
			}
			else if (!strcmp(buffer, "NEUTRAL_CLEF1")) {
				staff_elem_type = NEUTRAL_CLEF1;
				was_clef = true;
			}
			else if (!strcmp(buffer, "NEUTRAL_CLEF2")) {
				staff_elem_type = NEUTRAL_CLEF2;
				was_clef = true;
			}
			else if (!strcmp(buffer, "NEUTRAL_CLEF3")) {
				staff_elem_type = NEUTRAL_CLEF3;
				was_clef = true;
			}
			else if (!strcmp(buffer, "KEYSIG")) {
				staff_elem_type = TYPE_KEYSIG;
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ",")) {
					NedResource::m_error_message = ", expected";
					return;
				}
				if (!NedResource::readInt(fp, &keysignum)) {
					NedResource::m_error_message = "keysig number expected";
					return;
				}
			}
			else if (!strcmp(buffer, "STAFF_SIGN")) {
				staff_elem_type = TYPE_STAFF_SIGN;
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ",")) {
					NedResource::m_error_message = ", expected";
					return;
				}
				if (!NedResource::readInt(fp, &staff_sign_sub_type)) {
					NedResource::m_error_message = "staff sign number expected";
					return;
				}
			}
			else {
				printf("buffer = %s\n", buffer); fflush(stdout);
				NedResource::m_error_message = "TREBLE_CLEF, BASS_CLEF,  ALTO_CLEF, SOPRAN_CLEF, TENOR_CLEF, NEUTRAL_CLEF1, NEUTRAL_CLEF2, NEUTRAL_CLEF3, KEYSIG, or STAFF_SIGN expected";
				return;
			}
			if (was_clef && getMainWindow()->getFileVersion() > 4) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ",")) {
					NedResource::m_error_message = ", expected";
					return;
				}
				if (!NedResource::readInt(fp, &octave_shift) || (octave_shift != 0  && octave_shift != -12 && octave_shift != 12)) {
					NedResource::m_error_message = "octave_shift expected";
					return;
				}
			}
				
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ",")) {
				NedResource::m_error_message = ", expected";
				return;
			}
			if (!NedResource::readLong(fp, &miditime)) {
				NedResource::m_error_message = "miditime expected";
				return;
			}
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ")")) {
				NedResource::m_error_message = ") expected";
				return;
			}
			if (staff_elem_type == TYPE_KEYSIG) {
				staffelem = new NedChordOrRest(m_voices[0], TYPE_KEYSIG, keysignum, 0, miditime);
			}
			else if (staff_elem_type >= TREBLE_CLEF && staff_elem_type <= NEUTRAL_CLEF3) {
				staffelem = new NedChordOrRest(m_voices[0], TYPE_CLEF, staff_elem_type, octave_shift, miditime, true);
			}
			else {
				staffelem = new NedChordOrRest(m_voices[0], TYPE_STAFF_SIGN, staff_sign_sub_type, miditime);
			}
			m_staffelems = g_list_insert_sorted(m_staffelems, staffelem, (gint (*)(const void*, const void*)) NedChordOrRest::compareMidiTimesAndTypes);
			if (!NedResource::readWord(fp, buffer)) {
				NedResource::m_error_message = "( or VOICE expected";
				return;
			}
		}
	}
	NedResource::unreadWord(buffer);
	for (i = 0; i < VOICE_COUNT && NedResource::m_error_message == NULL; i++) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "VOICE")) {
			NedResource::m_error_message = "VOICE expected";
			return;
		}
		if (!NedResource::readInt(fp, &voicenum) || voicenum != i) {
			NedResource::m_error_message = "VOICE number exoected";
			return;
		}
		m_voices[i]->restoreVoice(fp);
	}
}
