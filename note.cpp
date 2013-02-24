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
#include "note.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "voice.h"
#include "system.h"
#include "mainwindow.h"
#include "commandlist.h"
#include "commandhistory.h"
#include "tienotescommand.h"
#include "untieforwardcommand.h"
#include "changeaccidentalcmd.h"
#include "druminfo.h"


#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getStaff()->getBottomPos()) * zoom_factor - topy)
#define X_POS_INVERS_PAGE_REL(p) ((leftx + (p) / current_scale) / zoom_factor - getPage()->getContentXpos())
#define Y_POS_INVERS_STAFF_REL(p) ((topy + (p) / current_scale) / zoom_factor - (getSystem()->getYPos() + getStaff()->getBottomPos()))

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_STAFF_REL(p) ((height - ((p) + getSystem()->getYPos() + getStaff()->getBottomPos())) * PS_ZOOM)

#define TREBLE_PITCH_OFFS 35
#define BASS_PITCH_OFFS   23
#define ALTO_PITCH_OFFS   29
#define TENOR_PITCH_OFFS  27
#define SOPRAN_PITCH_OFFS 33

#define TREBLE_LINE_OFFS  0
#define BASS_LINE_OFFS   12
#define ALTO_LINE_OFFS    6
#define TENOR_LINE_OFFS   8
#define SOPRAN_LINE_OFFS  2

struct pitch_descr {
	char pitch, min_sharp, min_flat;
};

pitch_descr NedNote::lineToPitch[] = {
                                          {  4 /* E */, 6, 2}, {  5 /* F */, 1, 100}, {  7 /* G */, 3, 5}, {  9 /* A */, 5, 3}, { 11 /* B */, 100, 1},
{ 12 /* C */, 2, 6}, { 14 /* D */, 4, 4}, { 16 /* E */, 6, 2}, { 17 /* F */, 1, 100}, { 19 /* G */, 3, 5}, { 21 /* A */, 5, 3}, { 23 /* B */, 100, 1},
{ 24 /* C */, 2, 6}, { 26 /* D */, 4, 4}, { 28 /* E */, 6, 2}, { 29 /* F */, 1, 100}, { 31 /* G */, 3, 5}, { 33 /* A */, 5, 3}, { 35 /* B */, 100, 1},
{ 36 /* C */, 2, 6}, { 38 /* D */, 4, 4}, { 40 /* E */, 6, 2}, { 41 /* F */, 1, 100}, { 43 /* G */, 3, 5}, { 45 /* A */, 5, 3}, { 47 /* B */, 100, 1},
{ 48 /* C */, 2, 6}, { 50 /* D */, 4, 4}, { 52 /* E */, 6, 2}, { 53 /* F */, 1, 100}, { 55 /* G */, 3, 5}, { 57 /* A */, 5, 3}, { 59 /* B */, 100, 1}, 
{ 60 /* C */, 2, 6}, { 62 /* D */, 4, 4}, { 64 /* E */, 6, 2}, { 65 /* F */, 1, 100}, { 67 /* G */, 3, 5}, { 69 /* A */, 5, 3}, { 71 /* B */, 100, 1},
{ 72 /* C */, 2, 6}, { 74 /* D */, 4, 4}, { 76 /* E */, 6, 2}, { 77 /* F */, 1, 100}, { 79 /* G */, 3, 5}, { 81 /* A */, 5, 3}, { 83 /* B */, 100, 1},
{ 84 /* C */, 2, 6}, { 86 /* D */, 4, 4}, { 88 /* E */, 6, 2}, { 89 /* F */, 1, 100}, { 91 /* G */, 3, 5}, { 93 /* A */, 5, 3}, { 95 /* B */, 100, 1},
{ 96 /* C */, 2, 6}, { 98 /* D */, 4, 4}, {100 /* E */, 6, 2}, {101 /* F */, 1, 100}, {103 /* G */, 3, 5}, {105 /* A */, 5, 3}, {107 /* B */, 100, 1},
{108 /* C */, 2, 6}, {110 /* D */, 4, 4}, {112 /* E */, 6, 2}, {114 /* F */, 1, 100}, {115 /* G */, 3, 5}, {117 /* A */, 5, 3}, {119 /* B */, 100, 1}};

int NedNote::signToBase[][2] = {
	{-6, 66}, {-5, 61}, {-4, 68}, {-3, 63}, {-2, 70 }, {-1, 65}, {0, 60}, {1, 67}, {2, 62}, {3, 69}, {4, 64}, {5, 71}, {6, 66}
};

int NedNote::diaDists[] = {-7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19};

NedNote::NedNote(NedChordOrRest *chord_or_rest, int line, int head, unsigned int status) :
m_active(NOTE_ACTIVE_STATE_NONE), m_xpos(0.0), m_ypos(0.0), m_tmp_ypos(0.0), m_acc_dist(0.0), m_line(line), m_tmp_line(UNKNOWN_LINE), m_status(status), m_head(head), m_chord_or_rest(chord_or_rest),
m_tie_forward(NULL), m_tie_backward(NULL), m_tie_y_offs1(0.0), m_tie_y_offs2(0.0), m_tie_xpos0(0.0), m_tie_xpos1(0.0), m_tie_back_xpos0(0.0),
m_tie_back_xpos1(0.0), m_tie_offs(0.0) {
	m_ypos = - line * LINE_DIST/2.0;
	/*
	NedResource::DbgMsg(DBG_TESTING, "keysig = 0, pitch = 62, pitchup = %d, pitchdown = %d\n", findPitchUpDown(0, 62, true),  findPitchUpDown(0, 62, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = 0, pitch = 64, pitchup = %d, pitchdown = %d\n", findPitchUpDown(0, 64, true),  findPitchUpDown(0, 64, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = 0, pitch = 65, pitchup = %d, pitchdown = %d\n", findPitchUpDown(0, 65, true),  findPitchUpDown(0, 65, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = 2, pitch = 64, pitchup = %d, pitchdown = %d\n", findPitchUpDown(2, 64, true),  findPitchUpDown(2, 64, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = 2, pitch = 66, pitchup = %d, pitchdown = %d\n", findPitchUpDown(2, 66, true),  findPitchUpDown(2, 66, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = 2, pitch = 67, pitchup = %d, pitchdown = %d\n", findPitchUpDown(2, 67, true),  findPitchUpDown(2, 67, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = -4, pitch = 68, pitchup = %d, pitchdown = %d\n", findPitchUpDown(-4, 68, true),  findPitchUpDown(-4, 68, false));
	NedResource::DbgMsg(DBG_TESTING, "keysig = -4, pitch = 67, pitchup = %d, pitchdown = %d\n", findPitchUpDown(-4, 67, true),  findPitchUpDown(-4, 67, false));
	*/
}

NedMainWindow *NedNote::getMainWindow() {return m_chord_or_rest->getStaff()->getSystem()->getPage()->getMainWindow();}
NedPage *NedNote::getPage() {return m_chord_or_rest->getStaff()->getSystem()->getPage();}
NedSystem *NedNote::getSystem() {return m_chord_or_rest->getStaff()->getSystem();}
NedStaff *NedNote::getStaff() {return m_chord_or_rest->getStaff();}
NedVoice *NedNote::getVoice() {return m_chord_or_rest->getVoice();}

void NedNote::draw(cairo_t *cr) {
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	cairo_glyph_t glyph;
	double yl;
	double yy = (m_tmp_line == UNKNOWN_LINE) ? m_ypos : m_tmp_ypos;
	int line;
	/*
	extern  bool xxx;
	if (xxx) {
		getMainWindow()->drawVisibleRectangle(cr, this);
		xxx = false;
	}
	*/

	if (getMainWindow()->m_selected_note == this || m_active == NOTE_ACTIVE_STATE_ON) {
		if (getMainWindow()->doPaintColored()) {
			switch(getVoice()->getVoiceNumber()) {
				case 0: cairo_set_source_rgb (cr, DSCOLOR(V1RED), DSCOLOR(V1GREEN), DSCOLOR(V1BLUE)); break;
				case 1: cairo_set_source_rgb (cr, DSCOLOR(V2RED), DSCOLOR(V2GREEN), DSCOLOR(V2BLUE)); break;
				case 2: cairo_set_source_rgb (cr, DSCOLOR(V3RED), DSCOLOR(V3GREEN), DSCOLOR(V3BLUE)); break;
				default: cairo_set_source_rgb (cr, DSCOLOR(V4RED), DSCOLOR(V4GREEN), DSCOLOR(V4BLUE)); break;
			}
		}
		else {
			cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
		}
	}
	else if (getMainWindow()->doPaintColored()) {
		switch(getVoice()->getVoiceNumber()) {
			case 0: cairo_set_source_rgb (cr, DCOLOR(V1RED), DCOLOR(V1GREEN), DCOLOR(V1BLUE)); break;
			case 1: cairo_set_source_rgb (cr, DCOLOR(V2RED), DCOLOR(V2GREEN), DCOLOR(V2BLUE)); break;
			case 2: cairo_set_source_rgb (cr, DCOLOR(V3RED), DCOLOR(V3GREEN), DCOLOR(V3BLUE)); break;
			default: cairo_set_source_rgb (cr, DCOLOR(V4RED), DCOLOR(V4GREEN), DCOLOR(V4BLUE)); break;
		}
	}
	switch (m_head) {
		case NORMAL_NOTE:
			switch(m_chord_or_rest->getLength()) {
				case WHOLE_NOTE:
					glyph.index = BASE + 6; break;
				case NOTE_2:
					glyph.index = BASE + 7; break;
				default:
					glyph.index = BASE + 4; break;
			}
			break;
		case CROSS_NOTE1:
			glyph.index = BASE + 21; break;
		case CROSS_NOTE2:
			glyph.index = BASE + 22; break;
		case RECT_NOTE1:
			glyph.index = BASE + 23; break;
		case RECT_NOTE2:
			glyph.index = BASE + 24; break;
		case TRIAG_NOTE1:
			glyph.index = BASE + 25; break;
		case TRIAG_NOTE2:
			glyph.index = BASE + 26; break;
		case OPEN_HIGH_HAT:
		case CLOSED_HIGH_HAT:
		case CROSS_NOTE3:
			glyph.index = BASE + 49; break;
		case CROSS_NOTE4:
			glyph.index = BASE + 50; break;
		case TRIAG_NOTE3:
			glyph.index = BASE + 51; break;
		case QUAD_NOTE2:
			glyph.index = BASE + 52; break;
		case GUITAR_NOTE_STEM:
			if (m_chord_or_rest->getLength() > NOTE_4) {
				glyph.index = BASE + 54; break;
			}
			glyph.index = BASE + 53; break;
		case GUITAR_NOTE_NO_STEM:
			glyph.index = BASE + 53; break;
	}
	glyph.x = X_POS_PAGE_REL(m_xpos + m_chord_or_rest->getXPosAndMicroshifts());
	glyph.y = Y_POS_STAFF_REL(yy);



	cairo_show_glyphs(cr, &glyph, 1);
	if (m_active) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		if (m_active == NOTE_ACTIVE_STATE_OFF) {
			m_active = NOTE_ACTIVE_STATE_NONE;
		}
		return;
	}

	switch (m_status & ACCIDENTAL_MASK) {
		case STAT_DFLAT: glyph.index = BASE + 29;
				glyph.x = X_POS_PAGE_REL(m_chord_or_rest->getXPos() - m_acc_dist /*+ ((m_line < -1 || m_line > 9) ? AUX_LINE_OVER: 0.0) */);
				cairo_show_glyphs(cr, &glyph, 1);
				break;
		case STAT_FLAT: glyph.index = BASE + 16;
				glyph.x = X_POS_PAGE_REL(m_chord_or_rest->getXPos() - m_acc_dist /*+ ((m_line < -1 || m_line > 9) ? AUX_LINE_OVER: 0.0) */);
				cairo_show_glyphs(cr, &glyph, 1);
				break;
		case STAT_SHARP: glyph.index = BASE - 1;
				glyph.x = X_POS_PAGE_REL(m_chord_or_rest->getXPos() - m_acc_dist /*+ ((m_line < -1 || m_line > 9) ? AUX_LINE_OVER: 0.0) */);
				cairo_show_glyphs(cr, &glyph, 1);
				break;
		case STAT_DSHARP: glyph.index = BASE + 30;
				glyph.x = X_POS_PAGE_REL(m_chord_or_rest->getXPos() - m_acc_dist /*+ ((m_line < -1 || m_line > 9) ? AUX_LINE_OVER: 0.0) */);
				cairo_show_glyphs(cr, &glyph, 1);
				break;
		case STAT_NATURAL: glyph.index = BASE + 17;
				glyph.x = X_POS_PAGE_REL(m_chord_or_rest->getXPos() - m_acc_dist /*+ ((m_line < -1 || m_line > 9) ? AUX_LINE_OVER: 0.0) */);
				cairo_show_glyphs(cr, &glyph, 1);
				break;
	}

	if (m_line < -1) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, zoom_factor * AUX_LINE_THICK);
		line = m_line / 2;
		line *= 2;
		for (; line < -1; line += 2) {
			yl = - line * LINE_DIST/2.0;
			cairo_move_to(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPosAndMicroshifts() + m_xpos - HEAD_THICK_2 - AUX_LINE_OVER),
				Y_POS_STAFF_REL(yl));
			cairo_line_to(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPosAndMicroshifts() + m_xpos + HEAD_THICK_2 + AUX_LINE_OVER),
				Y_POS_STAFF_REL(yl));
		}
		cairo_stroke(cr);
	}
	if (m_line > 9) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, zoom_factor * AUX_LINE_THICK);
		line = m_line / 2;
		line *= 2;
		for (; line > 9; line -= 2) {
			yl = - line * LINE_DIST/2.0;
			cairo_move_to(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPosAndMicroshifts() + m_xpos - HEAD_THICK_2 - AUX_LINE_OVER),
				Y_POS_STAFF_REL(yl));
			cairo_line_to(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPosAndMicroshifts() + m_xpos + HEAD_THICK_2 + AUX_LINE_OVER),
				Y_POS_STAFF_REL(yl));
		}
		cairo_stroke(cr);
	}

	if (m_chord_or_rest->getDotCount() > 0) {
		cairo_new_path(cr);
		if ((m_line % 2) == 0 && m_line >= 0 && m_line <= 8) {
			cairo_arc(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPos() + m_chord_or_rest->getBBox()->x + m_chord_or_rest->getBBox()->width_netto - 2 * DOT_RAD), Y_POS_STAFF_REL(yy - LINE_DIST / 2.0),
				zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
		}
		else {
			cairo_arc(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPos() + m_chord_or_rest->getBBox()->x + m_chord_or_rest->getBBox()->width_netto - 2 * DOT_RAD), Y_POS_STAFF_REL(yy),
				zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
		}
		cairo_fill(cr);
		if (m_chord_or_rest->getDotCount() > 1) {
			cairo_new_path(cr);
			if ((m_line % 2) == 0 && m_line >= 0 && m_line <= 8) {
				cairo_arc(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPos() + m_chord_or_rest->getBBox()->x + m_chord_or_rest->getBBox()->width_netto - 2 * DOT_RAD - (DOT2POS - DOT1POS)), Y_POS_STAFF_REL(yy - LINE_DIST / 2.0),
					zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
			}
			else {
				cairo_arc(cr, X_POS_PAGE_REL(m_chord_or_rest->getXPos() + m_chord_or_rest->getBBox()->x + m_chord_or_rest->getBBox()->width_netto - 2 * DOT_RAD - (DOT2POS - DOT1POS)), Y_POS_STAFF_REL(yy),
					zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
			}
			cairo_fill(cr);
		}
			
	}
	if (getMainWindow()->m_selected_note == this || getMainWindow()->doPaintColored()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_tie_forward != NULL) {
		if (m_chord_or_rest->getVoice() == m_tie_forward->getVoice()) {
			if (getMainWindow()->m_selected_tie_forward == this) {
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
			}
			cairo_new_path(cr);
			cairo_move_to(cr, X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos));
			cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos),
						X_POS_PAGE_REL(m_tie_xpos0 + (m_tie_xpos1 - m_tie_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + m_tie_y_offs1),
						X_POS_PAGE_REL(m_tie_xpos1), Y_POS_STAFF_REL(m_ypos));
			cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_xpos1), Y_POS_STAFF_REL(m_ypos),
						X_POS_PAGE_REL(m_tie_xpos0 + (m_tie_xpos1 - m_tie_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + m_tie_y_offs2),
						X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos));
			cairo_fill(cr);
			if (getMainWindow()->m_selected_tie_forward == this) {
				cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			}
		}
		else {
			cairo_new_path(cr);
			cairo_move_to(cr, X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos));
			cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos),
						X_POS_PAGE_REL(m_tie_xpos0 + (m_tie_xpos1 - m_tie_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + TIE_Y_OFFS1),
						X_POS_PAGE_REL(m_tie_xpos1), Y_POS_STAFF_REL(m_ypos));
			cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_xpos1), Y_POS_STAFF_REL(m_ypos),
						X_POS_PAGE_REL(m_tie_xpos0 + (m_tie_xpos1 - m_tie_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + TIE_Y_OFFS2),
						X_POS_PAGE_REL(m_tie_xpos0), Y_POS_STAFF_REL(m_ypos));
			cairo_fill(cr);
		}
			
	}
	if (m_tie_backward != NULL && m_chord_or_rest->getVoice() != m_tie_backward->getVoice()) {
		cairo_new_path(cr);
		cairo_move_to(cr, X_POS_PAGE_REL(m_tie_back_xpos0), Y_POS_STAFF_REL(m_ypos));
		cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_back_xpos0), Y_POS_STAFF_REL(m_ypos),
					X_POS_PAGE_REL(m_tie_back_xpos0 + (m_tie_back_xpos1 - m_tie_back_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + TIE_Y_OFFS1),
					X_POS_PAGE_REL(m_tie_back_xpos1), Y_POS_STAFF_REL(m_ypos));
		cairo_curve_to(cr,  X_POS_PAGE_REL(m_tie_back_xpos1), Y_POS_STAFF_REL(m_ypos),
					X_POS_PAGE_REL(m_tie_back_xpos0 + (m_tie_back_xpos1 - m_tie_back_xpos0)/ 2.0), Y_POS_STAFF_REL(m_ypos + TIE_Y_OFFS2),
					X_POS_PAGE_REL(m_tie_back_xpos0), Y_POS_STAFF_REL(m_ypos));
		cairo_fill(cr);
	}
};

bool NedNote::isFirstNote() {
	return m_chord_or_rest->getFirstNote() == this;
}


int NedNote::findPitchUpDown(int key, int pitch, int dist) {
	int base = 60;
	int i;

	for (i = 0; i < 12; i++) {
		if (signToBase[i][0] == key) {
			base = signToBase[i][1];
			break;
		}
	}
	while (pitch > base + 12) base += 12;
	while (pitch < base) base -= 12;
	for (i = 1; i < 9; i++) {
		if (base + diaDists[i] == pitch) {
			return base + diaDists[i+dist];
		}
	}
	return pitch + dist;
}

bool NedNote::testForTiesToDelete(NedCommandList *command_list, unsigned int dir /* = BREAK_TIE_BACKWARD */, bool execute /* = false */) {
	bool deleted_ties = FALSE;
	NedUnTieForwardCommand *untie_cmd;
	if (m_tie_backward != NULL && ((dir & BREAK_TIE_BACKWARD) != 0)) {
		deleted_ties = TRUE;
		untie_cmd = new NedUnTieForwardCommand(m_tie_backward, this);
		if (execute) {
			untie_cmd->execute();
		}
		command_list->addCommand(untie_cmd);
	}
	if (m_tie_forward != NULL && ((dir & BREAK_TIE_FORWARD) != 0)) {
		deleted_ties = TRUE;
		untie_cmd = new NedUnTieForwardCommand(this, m_tie_forward);
		if (execute) {
			untie_cmd->execute();
		}
		command_list->addCommand(untie_cmd);
	}
	return deleted_ties;
}
double NedNote::getYpos() {
#define GUITAR_YTOP_DISTANCE ( 1.6 * LINE_DIST )
	if (m_head == GUITAR_NOTE_STEM || m_head == GUITAR_NOTE_NO_STEM) {
		return m_ypos + GUITAR_YTOP_DISTANCE;
	}
	return (m_tmp_line == UNKNOWN_LINE) ? m_ypos : m_tmp_ypos;
}

void NedNote::correctTieForward() {
	if (m_tie_forward != NULL) {
		m_tie_forward->m_tie_backward = this;
	}
}

void NedNote::setOffset(char offs_array[OFFS_ARRAY_SIZE]) {
	if (offs_array[m_line + OFFS_ARRAY_HALF] != 0) return;
	switch (m_status & ACCIDENTAL_MASK) {
		case STAT_DFLAT: offs_array[m_line + OFFS_ARRAY_HALF] = -2;
				break;
		case STAT_FLAT: offs_array[m_line + OFFS_ARRAY_HALF] = -1;
				break;
		case STAT_SHARP: offs_array[m_line + OFFS_ARRAY_HALF] = 1;
				break;
		case STAT_DSHARP: offs_array[m_line + OFFS_ARRAY_HALF] = 2;
				break;
		case STAT_NATURAL: offs_array[m_line + OFFS_ARRAY_HALF] = VIRTUAL_NATURAL_OFFS;
				break;
	}
}

int NedNote::pitchToLine(int pitch, int clef, int keysig, int octave_shift, int *offs, int preferred_offs /* = 0 */) {
	int oct = 0;
	int line = 0;
	int retline = 0;
	int x_oct_shift= 0;
	*offs = 0;
	int clef_offs = 0;
	switch (octave_shift) {
		case  12: x_oct_shift =  7; break;
		case -12: x_oct_shift = -7; break;
		case   1: x_oct_shift =  7; break;
		case  -1: x_oct_shift = -7; break;
		case   0: x_oct_shift =  0; break;
		default: printf("octave_shift = %d\n", octave_shift);
			NedResource::Abort("NedNote::pitchToLine");
			break;
	}

	while (pitch < 60) {
		pitch += 12; oct--;
	}
	while (pitch >= 72) {
		pitch-= 12; oct++;
	}
	switch (clef) {
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF3:
		case TREBLE_CLEF: clef_offs = TREBLE_LINE_OFFS; break;
		case NEUTRAL_CLEF2:
		case BASS_CLEF: clef_offs = BASS_LINE_OFFS; break;
		case ALTO_CLEF: clef_offs = ALTO_LINE_OFFS; break;
		case TENOR_CLEF: clef_offs = TENOR_LINE_OFFS; break;
		case SOPRAN_CLEF: clef_offs = SOPRAN_LINE_OFFS; break;
	}

	switch (preferred_offs) {
		case -1:
			switch (pitch) {
				case 60: line = -2; break;
				case 61: line = -1; *offs = -1; break;
				case 62: line = -1; break;
				case 63: line =  0; *offs = -1; break;
				case 64: line =  0; break;
				case 65: line =  1; break;
				case 66: line =  2; *offs = -1; break;
				case 67: line =  2; break;
				case 68: line =  3; *offs = -1; break;
				case 69: line =  3; break;
				case 70: line =  4; *offs = -1; break;
				case 71: line =  4; break;
			}
			break;
		case 1:
			switch (pitch) {
				case 60: line = -2; break;
				case 61: line = -2; *offs = 1; break;
				case 62: line = -1; break;
				case 63: line = -1; *offs = 1; break;
				case 64: line =  0; break;
				case 65: line =  1; break;
				case 66: line =  1; *offs = 1; break;
				case 67: line =  2; break;
				case 68: line =  2; *offs = 1; break;
				case 69: line =  3; break;
				case 70: line =  3; *offs = 1; break;
				case 71: line =  4; break;
			}
			break;
		default:
			if (keysig < 0) {
				switch (pitch) {
					case 60: line = -2; break;
					case 61: line = -1; *offs = -1; break;
					case 62: line = -1; break;
					case 63: line =  0; *offs = -1; break;
					case 64: line =  0; break;
					case 65: line =  1; break;
					case 66: line =  2; *offs = -1; break;
					case 67: line =  2; break;
					case 68: line =  3; *offs = -1; break;
					case 69: line =  3; break;
					case 70: line =  4; *offs = -1; break;
					case 71: line =  4; break;
				}
			}
			else {
				switch (pitch) {
					case 60: line = -2; break;
					case 61: line = -2; *offs = 1; break;
					case 62: line = -1; break;
					case 63: line = -1; *offs = 1; break;
					case 64: line =  0; break;
					case 65: line =  1; break;
					case 66: line =  1; *offs = 1; break;
					case 67: line =  2; break;
					case 68: line =  2; *offs = 1; break;
					case 69: line =  3; break;
					case 70: line =  3; *offs = 1; break;
					case 71: line =  4; break;
				}
			}
	}
	retline = line + 7 * oct - x_oct_shift + clef_offs;
	while (retline < MIN_LINE) retline += 7;
	while (retline > MAX_LINE) retline -= 7;
	return retline;
}

int NedNote::getPitch() {
	int clef, keysig, octave_shift;
	bool dummy;
	getStaff()->getCurrentClefAndKeysig(m_chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
	return getPitch(clef, keysig, octave_shift, 0, &dummy, true);
}

int NedNote::getPitch(int clef, int keysig, int octave_shift, int pitch_offs, bool *flat, bool use_transpose) {
	if (clef == NEUTRAL_CLEF3) return getDrumPitch();
	int p;
	int offs = 0;
	char offs_array[OFFS_ARRAY_SIZE];
	*flat = false;

	if (m_tie_backward != 0) {
		return m_tie_backward->getPitch(clef, keysig, octave_shift, pitch_offs, flat, use_transpose);
	}

	switch (clef) {
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF3:
		case TREBLE_CLEF: offs = TREBLE_PITCH_OFFS; break;
		case NEUTRAL_CLEF2:
		case BASS_CLEF: offs = BASS_PITCH_OFFS; break;
		case ALTO_CLEF: offs = ALTO_PITCH_OFFS; break;
		case TENOR_CLEF: offs = TENOR_PITCH_OFFS; break;
		case SOPRAN_CLEF: offs = SOPRAN_PITCH_OFFS; break;
	}
	p = lineToPitch[m_line+offs].pitch; 
	memset(offs_array, 0, sizeof(offs_array));
	//getStaff()->findAccidentals(offs_array, getSystem()->getMeasure(m_chord_or_rest->getMidiTime()), m_chord_or_rest->getMidiTime());
	getStaff()->findAccidentals(offs_array, m_chord_or_rest->m_measure, m_chord_or_rest->getMidiTime(), true);
	
	switch (m_status & ACCIDENTAL_MASK) {
		case STAT_DFLAT: p -= 2; *flat = true;
				break;
		case STAT_FLAT: p--; *flat = true;
				break;
		case STAT_SHARP: p++;
				break;
		case STAT_DSHARP: p += 2;
				break;
		case STAT_NATURAL: break;
		default:
			if (offs_array[m_line + OFFS_ARRAY_HALF] == VIRTUAL_NATURAL_OFFS) { //natural
				break;
			}
			if (keysig < 0) {
				if (lineToPitch[m_line+offs].min_flat <= -keysig && offs_array[m_line+OFFS_ARRAY_HALF] == 0) {
					p--;
					*flat = true;
					break;
				}
			}
			if (lineToPitch[m_line+offs].min_sharp <= keysig && offs_array[m_line+OFFS_ARRAY_HALF] == 0) {
				p++;
				break;
			}
			p += offs_array[m_line + OFFS_ARRAY_HALF];
			if (offs_array[m_line + OFFS_ARRAY_HALF] < 0) *flat = true;
			break;
	} 
	if (use_transpose) {
		return p + octave_shift + pitch_offs + getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_play_transposed;
	}
	return p + octave_shift + pitch_offs;
}

int NedNote::getDrumPitch() {
	struct d_info_str *d_inf_ptr;

	for (d_inf_ptr = current_dinfo; d_inf_ptr->name; d_inf_ptr++) {
		if (m_line == d_inf_ptr->line && m_head == d_inf_ptr->note_head) {
			return d_inf_ptr->pitch;
		}
	}
	return 60;
}

	
void NedNote::removeUnneededAccidental(int clef, int keysig, int octave_shift) {
	int p;
	int offs = 0;
	char offs_array[OFFS_ARRAY_SIZE];

	switch (clef) {
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF3:
		case TREBLE_CLEF: offs = TREBLE_PITCH_OFFS; break;
		case NEUTRAL_CLEF2:
		case BASS_CLEF: offs = BASS_PITCH_OFFS; break;
		case ALTO_CLEF: offs = ALTO_PITCH_OFFS; break;
		case TENOR_CLEF: offs = TENOR_PITCH_OFFS; break;
		case SOPRAN_CLEF: offs = SOPRAN_PITCH_OFFS; break;
	}
	p = lineToPitch[m_line+offs].pitch + octave_shift; 
	memset(offs_array, 0, sizeof(offs_array));
	getStaff()->findAccidentals(offs_array, m_chord_or_rest->m_measure, m_chord_or_rest->getMidiTime(), false);
	switch (m_status & ACCIDENTAL_MASK) {
		case STAT_FLAT: if (offs_array[m_line+OFFS_ARRAY_HALF] == -1) {
					 m_status &= (~(ACCIDENTAL_MASK));
					 break;
				}
				if (offs_array[m_line+OFFS_ARRAY_HALF] != VIRTUAL_NATURAL_OFFS) {
					if (keysig < 0) {
						if (lineToPitch[m_line+offs].min_flat <= -keysig && offs_array[m_line+OFFS_ARRAY_HALF] != 2 /* natural */) {
							m_status &= (~(ACCIDENTAL_MASK));
						}
					}
				}
				break;
		case STAT_SHARP: if (offs_array[m_line+OFFS_ARRAY_HALF] == 1) {
					m_status &= (~(ACCIDENTAL_MASK));
					break;
				 }
				if (offs_array[m_line+OFFS_ARRAY_HALF] != VIRTUAL_NATURAL_OFFS) {
					if (lineToPitch[m_line+offs].min_sharp <= keysig && offs_array[m_line+OFFS_ARRAY_HALF] != 2 /* natural */) {
						m_status &= (~(ACCIDENTAL_MASK));
					}
				}
				break;
		case STAT_NATURAL: 
				if (offs_array[m_line+OFFS_ARRAY_HALF] == 2) { //natural
					m_status &= (~(ACCIDENTAL_MASK));
					break;
				}
				if (offs_array[m_line+OFFS_ARRAY_HALF] != 0) break;
				if (keysig < 0) {
					if (lineToPitch[m_line+offs].min_flat > -keysig) {
						m_status &= (~(ACCIDENTAL_MASK));
					}
					break;
				}
				if (lineToPitch[m_line+offs].min_sharp > keysig) {
					m_status &= (~(ACCIDENTAL_MASK));
				}
				break;
	} 
}


unsigned int NedNote::determineState(int pitch, int line, int half_offs, char offs_array[117], int keysig) {
	int i;

	switch (half_offs) {
		case -1: if (offs_array[line + 115 / 2] == -1) return 0;
			 if (offs_array[line + 115 / 2] != VIRTUAL_NATURAL_OFFS) {
			 	if (keysig < 0) {
			 		for (i = sizeof(lineToPitch) / sizeof(pitch_descr) - 1; i >= 0; i--) {
						if (lineToPitch[i].pitch < pitch) {
							i++; break;
						}
					}
			 		if (lineToPitch[i].min_flat <= -keysig) return 0;
			 	}
			 }
			 offs_array[line + 115 / 2] = -1;
			 return STAT_FLAT;
		case  1: if (offs_array[line + 115 / 2] == 1) return 0;
			 if (offs_array[line + 115 / 2] != VIRTUAL_NATURAL_OFFS) {
			 	if (keysig > 0) {
				  for ( i = 0; i < (signed int)(sizeof(lineToPitch) / sizeof(pitch_descr)); i++) {
						if (lineToPitch[i].pitch > pitch) {
							i--; break;
						}
					}
			 		if (lineToPitch[i].min_sharp <= keysig) return 0;
			 	}
			 }
			 offs_array[line + 115 / 2] =  1;
			 return STAT_SHARP;
		default: if (offs_array[line + 115 / 2] == VIRTUAL_NATURAL_OFFS) return 0;
			 if (offs_array[line + 115 / 2] == 1 || offs_array[line + 115 / 2] == -1) {
			 	offs_array[line + 115 / 2] = VIRTUAL_NATURAL_OFFS;
				return STAT_NATURAL;
			 }
			 if (keysig < 0) {
			 	for (i = sizeof(lineToPitch) / sizeof(pitch_descr) - 1; i >= 0; i--) {
					if (lineToPitch[i].pitch < pitch) {
						i++; break;
					}
				}
			 	if (lineToPitch[i].min_flat <= -keysig) {
					offs_array[line + 115 / 2] = VIRTUAL_NATURAL_OFFS;
					return STAT_NATURAL;
				}
			 }
			 if (keysig > 0) {
			 	for ( i = 0; i < (signed int)(sizeof(lineToPitch) / sizeof(pitch_descr)); i++) {
					if (lineToPitch[i].pitch > pitch) {
						i--; break;
					}
				}
			 	if (lineToPitch[i].min_sharp <= keysig) {
					offs_array[line + 115 / 2] = VIRTUAL_NATURAL_OFFS; // neutral
					return STAT_NATURAL;
				}
			}
			break;
	} 
	return 0;
}

void NedNote::setInternalPitch(int pitchdist, int clef, int keysig, int octave_shift) {
	bool dummy;

	m_pitch = getPitch(clef, keysig, octave_shift, 0, &dummy, false) + pitchdist;
}

void NedNote::adjustAccidental(int clef, int keysig, int octave_shift) {
	int offs;

	m_line = pitchToLine(m_pitch, clef, keysig, octave_shift, &offs);
	m_ypos = - m_line * LINE_DIST/2.0;

	m_status &= (~(ACCIDENTAL_MASK));

	switch (offs) {
		case -1: m_status |= STAT_FLAT; break;
		case  1: m_status |= STAT_SHARP; break;
		default: m_status |= STAT_NATURAL; break;
	}
}

void NedNote::changeAccidental(NedCommandList *command_list, int preferred_offs, int clef, int keysig, int octave_shift) {
	int offs;
	int line;
	unsigned int status;
	NedChangeAccidentalCommand *ch_acc_cmd;


	line = pitchToLine(m_pitch, clef, keysig, octave_shift, &offs, preferred_offs);
	status = (m_status & (~(ACCIDENTAL_MASK)));

	switch (offs) {
		case -1: status |= STAT_FLAT; break;
		case  1: status |= STAT_SHARP; break;
		default: status |= STAT_NATURAL; break;
	}

	if (status != m_status || line != m_line) {
		ch_acc_cmd = new NedChangeAccidentalCommand(this, status, line);
		ch_acc_cmd->execute();
		command_list->addCommand(ch_acc_cmd);
	}
}


void NedNote::prepareReplay(unsigned long long midi_start_time, int clef, int keysig, int octave_shift, int pitch_offs, int voice_idx, 
		 unsigned int grace_time, bool is_grace, bool is_stacc, unsigned short segno_sign) {
	int pitches[4], tt;
	unsigned int i, part_duration;
	bool dummy;

	pitches[0] = getPitch(clef, keysig, octave_shift, pitch_offs, &dummy, true);
	if (is_grace) {
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time - grace_time, SND_SEQ_EVENT_NOTEON, pitches[0],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time - grace_time + GRACE_DURATION, SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
	}
	else if (is_stacc && m_chord_or_rest->getLength() > NOTE_32 && grace_time == 0) {
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time, SND_SEQ_EVENT_NOTEON, pitches[0],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			m_chord_or_rest->getMidiTime() + m_chord_or_rest->getDuration() / 2, SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
	}
	else if (grace_time == 0 && (m_chord_or_rest->getStatus() & STAT_TRILL) != 0 && (m_chord_or_rest->getStatus() & (STAT_STACC | STAT_STACCATISSIMO)) == 0) {
		pitches[1] = findPitchUpDown(keysig, pitches[0], 1);
		for (tt = 0, i = 0; i < m_chord_or_rest->getDuration() / NOTE_32; i++, tt = 1 - tt) {
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time + i * NOTE_32, SND_SEQ_EVENT_NOTEON, tt ? pitches[0] : pitches[1],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			m_chord_or_rest->getMidiTime() + (i + 1) * NOTE_32, SND_SEQ_EVENT_NOTEOFF, tt ? pitches[1] : pitches[0], 0, segno_sign, this);
		}
	}
	else if (m_chord_or_rest->getDuration() >= NOTE_8 && grace_time == 0 && (m_chord_or_rest->getStatus() & STAT_TURN) != 0 && (m_chord_or_rest->getStatus() & (STAT_STACC | STAT_STACCATISSIMO)) == 0) {
		pitches[1] =  pitches[0];
		pitches[0] = findPitchUpDown(keysig, pitches[1],  1);
		pitches[2] = findPitchUpDown(keysig, pitches[1],  -1);
		pitches[3] = pitches[1];
		part_duration = m_chord_or_rest->getDuration() / 4;
		for (i = 0; i < 4; i++) {
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time + i * part_duration, SND_SEQ_EVENT_NOTEON, pitches[i],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			m_chord_or_rest->getMidiTime() + (i + 1) * part_duration, SND_SEQ_EVENT_NOTEOFF, pitches[i], 0, segno_sign, this);
		}
	}
	else if (m_chord_or_rest->getDuration() >= NOTE_8 && grace_time == 0 && (m_chord_or_rest->getStatus() & STAT_REV_TURN) != 0 && (m_chord_or_rest->getStatus() & (STAT_STACC | STAT_STACCATISSIMO)) == 0) {
		pitches[1] =  pitches[0];
		pitches[0] = findPitchUpDown(keysig, pitches[1],  -1);
		pitches[2] = findPitchUpDown(keysig, pitches[1],  1);
		pitches[3] = pitches[1];
		part_duration = m_chord_or_rest->getDuration() / 4;
		for (i = 0; i < 4; i++) {
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time + i * part_duration, SND_SEQ_EVENT_NOTEON, pitches[i],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
			NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			m_chord_or_rest->getMidiTime() + (i + 1) * part_duration, SND_SEQ_EVENT_NOTEOFF, pitches[i], 0, segno_sign, this);
		}
	}
	else if (grace_time == 0 && (m_chord_or_rest->getStatus() & STAT_PRALL) != 0 && (m_chord_or_rest->getStatus() & (STAT_STACC | STAT_STACCATISSIMO)) == 0) {
		pitches[1] = findPitchUpDown(keysig, pitches[0], 1);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time,  SND_SEQ_EVENT_NOTEON, pitches[0],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() +  NOTE_32, SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time + NOTE_32,  SND_SEQ_EVENT_NOTEON, pitches[1],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program,
		segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() + 2 * NOTE_32, SND_SEQ_EVENT_NOTEOFF, pitches[1], 0, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time + 2 * NOTE_32,  SND_SEQ_EVENT_NOTEON, pitches[0],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() +  m_chord_or_rest->getDuration(), SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
	}
	else if (grace_time == 0 && (m_chord_or_rest->getStatus() & STAT_MORDENT) != 0 && (m_chord_or_rest->getStatus() & (STAT_STACC | STAT_STACCATISSIMO)) == 0) {
		pitches[1] = findPitchUpDown(keysig, pitches[0], -1);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time,  SND_SEQ_EVENT_NOTEON, pitches[0],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program,
		segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() +  NOTE_32, SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time + NOTE_32,  SND_SEQ_EVENT_NOTEON, pitches[1],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() + 2 * NOTE_32, SND_SEQ_EVENT_NOTEOFF, pitches[1], 0, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		midi_start_time + 2 * NOTE_32,  SND_SEQ_EVENT_NOTEON, pitches[0],
		getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
		m_chord_or_rest->getMidiTime() +  m_chord_or_rest->getDuration(), SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
	}
	else {
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			midi_start_time, SND_SEQ_EVENT_NOTEON, pitches[0],
			getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[voice_idx].m_current_midi_program, segno_sign, this);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
			voice_idx,
			m_chord_or_rest->getMidiTime() + m_chord_or_rest->getDuration() - grace_time, SND_SEQ_EVENT_NOTEOFF, pitches[0], 0, segno_sign, this);
	}
}

void NedNote::setTieSimple(NedNote *other_note) {
	m_tie_forward = other_note;
	other_note->m_tie_backward = this;
}

void NedNote::setTieIfSamePitch(NedNote *other_note, bool check_pitch) {
	if (m_line != other_note->m_line) return;
	if (check_pitch) {
		if (!isTheSame(other_note)) return;
	}
	m_tie_forward = other_note;
	other_note->m_tie_backward = this;
}

bool NedNote::isTheSame(NedNote *other_note) {
	if (m_line != other_note->m_line) return false;
	return (getPitch() == other_note->getPitch());
}
	
void NedNote::removeBackwardTie() {
	if (m_tie_backward == NULL) return;
	m_tie_backward->m_tie_forward = NULL;
	m_tie_backward = NULL;
}
	
void NedNote::removeForwardTie() {
	if (m_tie_forward == NULL) return;
	m_tie_forward->m_tie_backward = NULL;
	m_tie_forward = NULL;
}
	

void NedNote::do_tie(NedNote *other_note) {
	int sorting;
	NedNote *n0, *n1;
	if (getChord()->getType() == TYPE_GRACE || other_note->getChord()->getType() == TYPE_GRACE) return;
	if (other_note == this) return;
	if (!isTheSame(other_note)) return;
	sorting = m_chord_or_rest->getSorting(other_note->m_chord_or_rest);
	switch (sorting) {
		case SORTING_NONE: return;
		case SORTING_LESS: n0 = other_note; n1 = this; break;
		default: n0 = this; n1 = other_note; break;
	}
	if (n0->m_tie_forward != NULL || n1->m_tie_backward != NULL) return;

	NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
	command_list->addCommand(new NedTieNotesCommand(n0, n1));
	command_list->execute();
	getMainWindow()->getCommandHistory()->addCommandList(command_list);
}

	
void NedNote::setTies(NedNote *n1, NedNote *n2) {
	n1->m_tie_forward = n2;
	n2->m_tie_backward = n1;
}

void NedNote::tryTieBackward() {
	NedChordOrRest *previousChord = m_chord_or_rest->getPreviousChordOrRest();
	if (previousChord == NULL) return;
	if (previousChord->getType() != TYPE_CHORD) return;
	previousChord->tieAllNotes(m_chord_or_rest);
}

bool NedNote::noConflict() {
	return m_chord_or_rest->noConflict(m_tmp_line);
}


bool NedNote::trySelect(double x, double y) {
	int zoom_level = getMainWindow()->getCurrentZoomLevel();
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	cairo_text_extents_t *extention = &(NedResource::fontextentions[zoom_level][4]);

	double xl = X_POS_INVERS_PAGE_REL(x);
	double yl = Y_POS_INVERS_STAFF_REL(y);
	double xnote = m_xpos + m_chord_or_rest->getXPosAndMicroshifts();

	if (m_ypos + extention->y_bearing / zoom_factor < yl &&
	    m_ypos + (extention->y_bearing  +  extention->height) / zoom_factor > yl &&
	    xnote + extention->x_bearing / zoom_factor < xl &&
	    xnote + (extention->x_bearing  +  extention->width) / zoom_factor > xl) {
		getMainWindow()->m_selected_note = this;
		return TRUE;
	}
	if (m_tie_forward != NULL) {
		if (xl > m_tie_xpos0 && xl < m_tie_xpos1) {
			if (m_tie_y_offs1 > 0.0) {
				if (yl > m_ypos && yl < m_ypos +  m_tie_y_offs1) {
					getMainWindow()->m_selected_tie_forward = this;
				}
			}
			else {
				if (yl < m_ypos && yl > m_ypos + m_tie_y_offs1) {
					getMainWindow()->m_selected_tie_forward = this;
				}
			}
		}
	}
	return FALSE;
}

bool NedNote::testYShift(double y) {
	if (m_head == GUITAR_NOTE_STEM  || m_head == GUITAR_NOTE_NO_STEM) return false;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();

	double yl = Y_POS_INVERS_STAFF_REL(y);
	int old_line = m_tmp_line;
	m_tmp_line = (int) (m_line + (m_ypos - yl) / (LINE_DIST/2.0));
	
	if (m_tmp_line != old_line) {
		m_tmp_ypos =  - m_tmp_line * LINE_DIST/2.0;
		return TRUE;
	}
	return FALSE;
}

void NedNote::setStatus(unsigned int status) {
	int voice_idx;
	int clef, keysig, octave_shift;
	int pitch;
	bool dummy;
	staff_context_str *context;
	m_status = status;
	context = &(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()]);
	getStaff()->getCurrentClefAndKeysig(m_chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
	if (getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_different_voices) {
		voice_idx = m_chord_or_rest->getVoice()->getVoiceNumber();
	}
	else {
		voice_idx = 0;
	}
	if (NedResource::m_midi_echo) {
		pitch = getPitch(clef, keysig, octave_shift, 0, &dummy, true);
		NedResource::playImmediately(context->m_midi_channel, context->voices[voice_idx].m_midi_program, 1, &pitch, context->voices[voice_idx].m_midi_volume);
	}
}
void NedNote::moveUpDown(int line) {
	staff_context_str *context;
	int voice_idx;
	bool dummy;
	int clef, keysig, octave_shift;
	int pitch;
	getStaff()->getCurrentClefAndKeysig(m_chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
	m_line = line;
	m_ypos = - m_line * LINE_DIST/2.0;
	m_chord_or_rest->sortNotes();
	if (getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_different_voices) {
		voice_idx = m_chord_or_rest->getVoice()->getVoiceNumber();
	}
	else {
		voice_idx = 0;
	}
	if (NedResource::m_midi_echo) {
		context = &(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()]);
		pitch = getPitch(clef, keysig, octave_shift, 0, &dummy, true);
		NedResource::playImmediately(context->m_midi_channel, context->voices[voice_idx].m_midi_program, 1, &pitch, context->voices[voice_idx].m_midi_volume);
	}
	m_tmp_line = UNKNOWN_LINE;
}

void NedNote::shiftNoteLine(int linedist) {
	m_line += linedist;
	m_ypos = - m_line * LINE_DIST/2.0;
}


bool NedNote::testRelativeMove(int dist) {
	return m_chord_or_rest->noConflict(m_line + dist);
}

bool NedNote::test_tie_increment(double incr) {
	if (m_tie_forward == NULL) return false;
	if (m_tie_offs + incr > MAX_TIE_INCR) return false;
	if (m_tie_offs + incr < MIN_TIE_INCR) return false;
	return true;
}



void NedNote::shiftNote(bool shift) {
	switch(m_chord_or_rest->getLength()) {
		case WHOLE_NOTE:
			m_xpos = -HEAD_THICK_HALF_2; break;
		case NOTE_2:
			m_xpos = -HEAD_THICK_HALF_2; break;
		default:
			m_xpos = -HEAD_THICK_2; break;
	}
	if (shift) {
		m_xpos = -m_xpos;
	}
}

void NedNote::computeTie() {
	if (m_tie_forward != NULL) {
		if (m_chord_or_rest->getVoice() == m_tie_forward->getVoice()) {
			if (m_chord_or_rest->getStemDir() == STEM_DIR_NONE) {
				if (!m_chord_or_rest->hasUpDir() && !m_tie_forward->getChord()->hasUpDir()) {
					m_tie_y_offs1 = -TIE_Y_OFFS1 + m_tie_offs;
					m_tie_y_offs2 = -TIE_Y_OFFS2 + m_tie_offs;
				}
				else  {
					m_tie_y_offs1 = TIE_Y_OFFS1 + m_tie_offs;
					m_tie_y_offs2 = TIE_Y_OFFS2 + m_tie_offs;
				}
			}
			else {
				if (m_chord_or_rest->getStemDir() == STEM_DIR_UP) {
					m_tie_y_offs1 = -TIE_Y_OFFS1 + m_tie_offs;
					m_tie_y_offs2 = -TIE_Y_OFFS2 + m_tie_offs;
				}
				else  {
					m_tie_y_offs1 = TIE_Y_OFFS1 + m_tie_offs;
					m_tie_y_offs2 = TIE_Y_OFFS2 + m_tie_offs;
				}
			}
			m_tie_xpos0 = m_chord_or_rest->getXPosAndMicroshifts() + m_xpos + TIE_X_START;
			m_tie_xpos1 = m_tie_forward->m_chord_or_rest->getXPosAndMicroshifts() + m_tie_forward->m_xpos - TIE_X_START;
			if (m_tie_xpos1 - m_tie_xpos0 > MIN_TIE_LEN) {
				m_tie_xpos0 += TIE_X_START_LARGE;
				m_tie_xpos1 -= TIE_X_START_LARGE;
			}
			else if (m_chord_or_rest->getDotCount()) {
				m_tie_xpos0 += DOT2POS;
			}
		}
		else {
			m_tie_xpos1 = getPage()->getContentWidth() + TIE_X_RIGHT_OVER;
			m_tie_xpos0 = m_chord_or_rest->getXPosAndMicroshifts() + m_xpos + TIE_X_START;
		}
			
	}
	if (m_tie_backward != NULL && m_chord_or_rest->getVoice() != m_tie_backward->getVoice()) {
		m_tie_back_xpos0 = getSystem()->m_system_start - TIE_X_OFFS;
		m_tie_back_xpos1 = m_chord_or_rest->getXPosAndMicroshifts() + m_xpos - TIE_X_START;
	}
}


void NedNote::collectNotesWithAccidentals(unsigned int *num_notes_with_accidentals, NedNote **note_field) {
	if (*num_notes_with_accidentals  >= MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF) return;
	if ((m_status & ACCIDENTAL_MASK) == 0) return;
	note_field[*num_notes_with_accidentals] = this;
	(*num_notes_with_accidentals)++;
}

int NedNote::compare_note_lines(NedNote **n1, NedNote **n2) {
	if ((*n1)->m_line < (*n2)->m_line) return -1;
	return 1;
}

double NedNote::placeAccidental(bool all_shifted, int acc_places[4], bool has_arpeggio, bool out) {
	bool placed;
	int  i;
	if ((m_status & ACCIDENTAL_MASK) != 0) {
		placed = false;
		i = (has_arpeggio ?  1 : 0);
		for (;!placed && i < 4; i++) {
			if (m_line - acc_places[i] > 4) {
				placed = true;
				m_acc_dist = ACCIDENTAL_BASE_DIST + i * NedResource::m_acc_bbox.width + HEAD_THICK;
				acc_places[i] = m_line;
			}
		}
		if (!placed) {
			NedResource::Warning("Cannot place accidental");
		}
		/*
		if (m_line < -1 || m_line > 9) {
			m_acc_dist += 2* AUX_LINE_OVER;
		}
		*/
	}
	return m_acc_dist;
}

void NedNote::computeBounds(int len, double *minx, double *miny, double *maxx, double *maxy) {
	double yy0, xx0, xx1, yy1;

	switch(len) {
		case WHOLE_NOTE:
			xx0 = m_xpos - HEAD_THICK_2;
			yy0 = m_ypos - HEAD_HIGHT_2;
			xx1 = m_xpos + HEAD_THICK_2;
			yy1 = m_ypos + HEAD_HIGHT_2;
			break;
		case NOTE_2:
			xx0 = m_xpos - HEAD_THICK_HALF_2;
			yy0 = m_ypos - HEAD_HIGHT_HALF_2;
			xx1 = m_xpos + HEAD_THICK_HALF_2;
			yy1 = m_ypos + HEAD_HIGHT_HALF_2;
			break;
		default:
			xx0 = m_xpos - HEAD_THICK_2;
			yy0 = m_ypos - HEAD_HIGHT_2;
			xx1 = m_xpos + HEAD_THICK_2;
			yy1 = m_ypos + HEAD_HIGHT_2;
			break;
	}

	if (m_line < -1 || m_line > 9) {
		xx0 -= AUX_LINE_OVER;
		xx1 += AUX_LINE_OVER;
	}

#ifdef XXX
	if ((m_status & ACCIDENTAL_MASK) != 0) {
		if (-m_acc_dist /* + NedResource::m_acc_bbox.x */ < xx0) xx0 = -m_acc_dist /* + NedResource::m_acc_bbox.x */;
		if (m_ypos + NedResource::m_acc_bbox.y < yy0) yy0 = m_ypos + NedResource::m_acc_bbox.y;
		if ((m_status & ACCIDENTAL_MASK) != STAT_FLAT && (m_status & ACCIDENTAL_MASK) != STAT_DFLAT) {
			if (yy0 + NedResource::m_acc_bbox.height > yy1) yy1 = yy0 + NedResource::m_acc_bbox.height;
		}
	}
#endif
	switch (m_chord_or_rest->getDotCount()) {
		case 1: if (xx1 < m_xpos + HEAD_THICK + DOT1POS) {xx1 =  m_xpos + HEAD_THICK + DOT1POS + 2 * DOT_RAD;} break;
		case 2: if (xx1 < m_xpos + HEAD_THICK + DOT2POS) {xx1 =  m_xpos + HEAD_THICK + DOT2POS + 2 * DOT_RAD;} break;
	}
	if (xx0 < *minx)  *minx = xx0;
	if (yy0 < *miny)  *miny = yy0;
	if (xx1 > *maxx)  *maxx = xx1;
	if (yy1 > *maxy)  *maxy = yy1;
}

void NedNote::saveNote(FILE *fp) {
	int marker;

	if (m_tie_forward || m_tie_backward) {
		marker = NedResource::addAddr(this);
		fprintf(fp, "|%d| ", marker);
	}
	switch (m_status & ACCIDENTAL_MASK) {
		case STAT_DFLAT: fprintf(fp, "p");
				break;
		case STAT_FLAT: fprintf(fp, "b");
				break;
		case STAT_SHARP: fprintf(fp, "#");
				break;
		case STAT_DSHARP: fprintf(fp, "c");
				break;
		case STAT_NATURAL:fprintf(fp, "=");
				break;
		default: fprintf(fp, "_");
			break;
	}
	fprintf(fp, "%d ", m_line);
	if (m_head > NORMAL_NOTE) {
		fprintf(fp, " [  head :  %d  ]", m_head);
	}
}

void NedNote::saveTies(FILE *fp, bool *ties_written) {
	int marker1, marker2;
	if (m_tie_backward != NULL) {
		if (!(*ties_written)) {
			fprintf(fp, "TIES\n");
			*ties_written = TRUE;
		}
		marker1 = NedResource::getMarkerOfAddress(m_tie_backward);
		marker2 = NedResource::getMarkerOfAddress(this);
		fprintf(fp, "( %d, %d, %f )\n", marker1, marker2, m_tie_backward->m_tie_offs);
	}
}

gint NedNote::compare(NedNote *a, NedNote *b) {
	if (a->m_line < b->m_line) return -1;
	if (a->m_line > b->m_line) return  1;
	return 0;
}

void NedNote::adjust_pointers(struct addr_ref_str *addrlist) {
	struct addr_ref_str *addr_ptr;
	void *ptr;
	if (m_tie_forward != NULL) {
		ptr = NULL;
		for (addr_ptr = addrlist; addr_ptr; addr_ptr = addr_ptr->next) {
			if (addr_ptr->ori == m_tie_forward) {
				ptr = addr_ptr->copy;
				break;
			}
		}
		m_tie_forward = (NedNote *) ptr;
	}
	if (m_tie_backward != NULL) {
		ptr = NULL;
		for (addr_ptr = addrlist; addr_ptr; addr_ptr = addr_ptr->next) {
			if (addr_ptr->ori == m_tie_backward) {
				ptr = addr_ptr->copy;
				break;
			}
		}
		m_tie_backward = (NedNote *) ptr;
	}
}
		
void NedNote::exportLilyPond(FILE *fp, int *last_line, bool *tie) {
	int clef, keysig, octave_shift;
	int pitch;
	int line;
	bool flat;
	staff_context_str *context;
	context = &(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()]);
	getStaff()->getCurrentClefAndKeysig(m_chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
	if (clef == NEUTRAL_CLEF3) {
		exportLilyPondDrums(fp);
		return;
	}
	if (clef == NEUTRAL_CLEF1 || clef == NEUTRAL_CLEF2 || clef == NEUTRAL_CLEF3) clef = ALTO_CLEF;
	pitch = getPitch(clef, keysig, octave_shift, 0, &flat, false);

	while (pitch < 60) pitch += 12;
	while (pitch > 71) pitch -= 12;

	switch (pitch) {
		case 60: fprintf(fp, " c"); break;
		case 61: fprintf(fp, " %s", flat ? "des" : "cis"); break;
		case 62: fprintf(fp, " d"); break;
		case 63: fprintf(fp, " %s", flat ? "es" : "dis"); break;
		case 64: fprintf(fp, " e"); break;
		case 65: fprintf(fp, " %s", (keysig > 5) ? "eis" : "f"); break;
		case 66: fprintf(fp, " %s", flat ? "ges" : "fis"); break;
		case 67: fprintf(fp, " g"); break;
		case 68: fprintf(fp, " %s", flat ? "as" : "gis"); break;
		case 69: fprintf(fp, " a"); break;
		case 70: fprintf(fp, " %s", flat ? "bes" : "ais"); break;
		case 71: fprintf(fp, " %s", (keysig < -5) ? "ces" : "b"); break;
	}
	line = m_line;
	while (line > *last_line + 3) {
		fprintf(fp, "'");
		line -= 7;
	}
	while (line < *last_line - 3) {
		fprintf(fp, ",");
		line += 7;
	}
	if (m_tie_forward != NULL) {
		*tie = true;
	}
	*last_line = m_line;
}

void NedNote::exportLilyPondDrums(FILE *fp) {
	struct d_info_str *d_inf_ptr;

	for (d_inf_ptr = current_dinfo; d_inf_ptr->name; d_inf_ptr++) {
		if (m_line == d_inf_ptr->line && m_head == d_inf_ptr->note_head) {
			fprintf(fp, " %s", d_inf_ptr->lily_short_name);
			return;
		}
	}
	fprintf(fp, " r");
}
