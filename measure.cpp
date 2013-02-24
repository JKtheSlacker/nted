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
#include "measure.h"
#include "resource.h"
#include "system.h"
#include "staff.h"
#include "mainwindow.h"

#define X_POS_PAGE_REL(p) ((content_x_pos + (p)) * zoom_factor - leftx)
#define Y_POS_SYS_REL(p) (((p) + sys_y_pos)  * zoom_factor - topy)
#define Y_POS_STAFF_REL(p) (((p) + sys_y_pos + top) * zoom_factor - topy)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_SYS_REL(p) ((page_height - ((p) + sys_y_pos)) * PS_ZOOM)

NedMeasure::NedMeasure() : m_staff_elements_width(0.0), m_measure_has_only_staff_members(false), m_spread_fac(1.0), m_special_spread(false), m_hide_following(false), m_measure_number(1), m_special_descr(NULL) {
	m_measure_number_str[0] = '1';
	m_measure_number_str[1] = '\0';
}

void NedMeasure::setMeasureNumber(int num, GList *special_measures, bool force /* = false */) {
	GList *lptr;


	if (m_measure_number == num && !force) return;

	m_special_descr = NULL;
	m_measure_number = num;
	sprintf(m_measure_number_str, "%d", num);
	for (lptr = g_list_first(special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == m_measure_number) {
			m_special_descr = (SpecialMeasure *) lptr->data;
			//NedResource::DbgMsg(DBG_TESTING, "0x%x, type = 0x%x --> c = %d (0x%x)\n", m_special_descr, m_special_descr->type, m_measure_number, &(m_special_descr->measure)); 
			m_hide_following = ((SpecialMeasure *) lptr->data)->hide_following;
			m_special_descr->measure = this;
		}
	}
}

int NedMeasure::getSpecial() {
	if (m_special_descr == NULL) return 0;
	return m_special_descr->type;
}

bool NedMeasure::barRequiresHideFollowing() {
	if (m_special_descr == NULL) return false;
	return (((m_special_descr->type & REP_TYPE_MASK) == END_BAR || (m_special_descr->type & REP_TYPE_MASK) == REPEAT_CLOSE) && m_hide_following);
}

bool NedMeasure::isNearStart(double x) {
	double dist = x - start;
	if (dist < 0.0) dist = -dist;
	return dist < MIN_NEAR_DIST;
	/*
	ret = dist < MIN_NEAR_DIST;
	if (ret) {
		NedResource::DbgMsg(DBG_TESTING, "measure %d, x = %f , start = %f, midi_start = %llu, midi_end = %llu\n",
			m_measure_number, x, start, midi_start / NOTE_4, midi_end / NOTE_4); 
	}
	return ret;
	*/
}

double NedMeasure::shiftXpos(double extra_space_divided, double shift, bool out) {
	start += shift;
	shift = m_position_array.shiftXpos(extra_space_divided, shift, m_measure_number, out);
	shift += extra_space_divided;
	return shift;
}

double NedMeasure::computeAndSetPositions(double start_position, int *num_elements, double *extra_space_for_accidentals, bool *measure_has_only_staff_members /* means keysig */, bool out) {
	
	double timesig_space = (m_special_descr && (m_special_descr->type & TIMESIG_TYPE_MASK)) ? TIME_SIG_WIDTH : 0.0;
	start_position += getNeededSpaceAfter();
	start = start_position;
	start_position += getNeededSpaceBefore();
	start_position = m_position_array.computeAndSetPositions(start_position, num_elements, extra_space_for_accidentals, measure_has_only_staff_members, timesig_space, m_measure_number, m_spread_fac, out);
	m_measure_has_only_staff_members = *measure_has_only_staff_members;
	end = start_position;
	return start_position;
}


bool NedMeasure::nearRepeatElement(double x, double y) {
	int i = 0;
	bool hit = false;
	double staff_border_dist, xx, yy;
	NedStaff *staff;

	for (i = 0; !hit &&(staff = m_system->getStaff(i)) != NULL; i++) {
		staff->convertStaffRel(x, y, &xx, &yy);
		double xdist = xx - start;
		if (xdist < 0.0) xdist = -xdist;
		if (xdist > MIN_NEAR_DIST) continue;
		staff_border_dist = staff->getTopYBorder();
		double repyoffs = staff_border_dist;
		if (staff_border_dist > -MIN_STAFF_BORDER_DIST) repyoffs = -MIN_STAFF_BORDER_DIST;
		repyoffs += SEC_DIST_ABOVE;
		double ydist = yy - (repyoffs + REP_NUM_YPOS - 4 * LINE_DIST);
		if (ydist < 0.0) ydist = - ydist;
		if (ydist > MIN_NEAR_DIST) continue;
		hit = true;
	}
	return hit;
}
	

bool NedMeasure::isNearEnd(double x) {
	double dist = x - end;
	if (dist < 0.0) dist = -dist;
	return dist < MIN_NEAR_DIST;
}

double NedMeasure::getNeededSpaceBefore() {
	if (m_special_descr == NULL) return MEAS_BAR_SPACE;
	switch (m_special_descr->type & TIMESIG_TYPE_MASK) {
		case TIMESIG: return TIME_SIGNATURE_SPACE;
	}
	switch (m_special_descr->type & REP_TYPE_MASK) {
		case REPEAT_OPEN_CLOSE: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST + REP_DOTS_X_EXTRA_DIST;
		case REPEAT_OPEN: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST;
		case REPEAT_CLOSE: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST;
		case DOUBLE_BAR: return REP_DOUBLE_LINE_DIST;
		case END_BAR: return 4 * REP_DOUBLE_LINE_DIST;
	}
	return MEAS_BAR_SPACE;
}

double NedMeasure::getNeededSpaceAfter() {
	if (m_special_descr == NULL) return MEAS_BAR_SPACE;

	switch (m_special_descr->type & REP_TYPE_MASK) {
		case REPEAT_OPEN_CLOSE: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST + REP_DOTS_X_EXTRA_DIST;
		case REPEAT_OPEN: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST + REP_DOTS_X_EXTRA_DIST;
		case REPEAT_CLOSE: return REP_THICK_LINE_DIST + REP_DOTS_X_DIST + REP_DOTS_X_EXTRA_DIST;
		case DOUBLE_BAR: return 3 * REP_DOUBLE_LINE_DIST;
		case END_BAR: return 4 * REP_DOUBLE_LINE_DIST;
	}
	return MEAS_BAR_SPACE;
}


void NedMeasure::draw(cairo_t *cr, int special_position, double xpos, double content_x_pos, double sys_y_pos, double top, double bottom, double bottom_ori, double leftx, double topy,
			double staff_border_dist, double zoom_factor, int zoom_level, bool rep_line_needed, int spec_measure_at_end, bool out) {
	double line_xpos, normal_line_pos = start;
	double repyoffs;
	double line_bottom = bottom;
	bool draw_default_line = true;
	int realtype = 0;
	double sig_xpos = start;

	if (m_special_descr != NULL) {
		realtype = m_special_descr->type & (REP_TYPE_MASK | TIMESIG_TYPE_MASK);
	}

	if (rep_line_needed && special_position != SPECIAL_POSITION_END) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
		repyoffs = staff_border_dist;
		if (staff_border_dist > -MIN_STAFF_BORDER_DIST) repyoffs = -MIN_STAFF_BORDER_DIST;
		repyoffs += SEC_DIST_ABOVE;
		if (special_position == SPECIAL_POSITION_START) {
			cairo_move_to(cr, X_POS_PAGE_REL(xpos), Y_POS_STAFF_REL(repyoffs));
		}
		else {
			cairo_move_to(cr, X_POS_PAGE_REL(start), Y_POS_STAFF_REL(repyoffs));
		}
		cairo_line_to(cr, X_POS_PAGE_REL(end), Y_POS_STAFF_REL(repyoffs));
		cairo_stroke(cr);
	}

	if (special_position == SPECIAL_POSITION_START) {
		if (m_special_descr == NULL) return;
		if ((m_special_descr->type & REP_TYPE_MASK) == REPEAT_OPEN_CLOSE) {
			realtype = REPEAT_OPEN;
		}
	}
	if (special_position == SPECIAL_POSITION_END) {
		normal_line_pos = xpos;
		if (m_special_descr != NULL) {
			//if ((m_special_descr->type & REP_TYPE_MASK) == REPEAT_OPEN) return;
			if ((m_special_descr->type & REP_TYPE_MASK) == REPEAT_OPEN_CLOSE) {
				realtype = REPEAT_CLOSE;
			}
		}
	}
	if (m_special_descr != NULL) {
		switch (m_special_descr->type & START_TYPE_MASK) {
			case REP1START: if (special_position == SPECIAL_POSITION_END) break;
					line_xpos = (special_position == SPECIAL_POSITION_START) ? xpos : start;
					repyoffs = staff_border_dist;
					if (staff_border_dist > -MIN_STAFF_BORDER_DIST) repyoffs = -MIN_STAFF_BORDER_DIST;
					repyoffs += SEC_DIST_ABOVE;
					cairo_new_path(cr);
					cairo_set_line_width(cr, zoom_factor *  MEASURE_LINE_THICK);
					cairo_move_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_STAFF_REL(0));
					cairo_line_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_STAFF_REL(repyoffs));
					if (special_position != SPECIAL_POSITION_END) {
						if (m_special_descr == m_system->getMainWindow()->m_selected_spec_measure) {
							cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
						}
						cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(cr, MEASURE_NUMBER_SIZE * zoom_factor);
						cairo_move_to(cr, X_POS_PAGE_REL(line_xpos + REP_NUM_XPOS), Y_POS_STAFF_REL(repyoffs + REP_NUM_YPOS));
						cairo_show_text(cr, "1.");
#ifdef HAS_SET_SCALED_FONT
						cairo_set_scaled_font (cr, scaled_font);
#else
						cairo_set_font_face(cr, NedResource::getFontFace());
						cairo_set_font_matrix(cr, NedResource::getFontMatrix(zoom_level));
#endif
						if (m_special_descr == m_system->getMainWindow()->m_selected_spec_measure) {
							cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
						}
					}
					cairo_stroke(cr);
					line_bottom = bottom_ori;
					break;
			case REP2START: if (special_position == SPECIAL_POSITION_END) break;
					line_xpos = (special_position == SPECIAL_POSITION_START) ? xpos : start;
					repyoffs = staff_border_dist;
					if (staff_border_dist > -MIN_STAFF_BORDER_DIST) repyoffs = -MIN_STAFF_BORDER_DIST;
					repyoffs += SEC_DIST_ABOVE;
					cairo_new_path(cr);
					cairo_set_line_width(cr, zoom_factor *  MEASURE_LINE_THICK);
					cairo_move_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_STAFF_REL(0));
					cairo_line_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_STAFF_REL(repyoffs));
					if (special_position != SPECIAL_POSITION_END) {
						if (m_special_descr == m_system->getMainWindow()->m_selected_spec_measure) {
							cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
						}
						cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(cr, MEASURE_NUMBER_SIZE * zoom_factor);
						cairo_move_to(cr, X_POS_PAGE_REL(line_xpos + REP_NUM_XPOS), Y_POS_STAFF_REL(repyoffs + REP_NUM_YPOS));
						cairo_show_text(cr, "2.");
#ifdef HAS_SET_SCALED_FONT
						cairo_set_scaled_font (cr, scaled_font);
#else
						cairo_set_font_face(cr, NedResource::getFontFace());
						cairo_set_font_matrix(cr, NedResource::getFontMatrix(zoom_level));
#endif
						if (m_special_descr == m_system->getMainWindow()->m_selected_spec_measure) {
							cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
						}
					}
					cairo_stroke(cr);
					break;
		}
		switch (m_special_descr->type & END_TYPE_MASK) {
			case REP1END:   if (special_position == SPECIAL_POSITION_START) break;
					repyoffs = staff_border_dist;
					if (staff_border_dist > -MIN_STAFF_BORDER_DIST) repyoffs = -MIN_STAFF_BORDER_DIST;
					repyoffs += SEC_DIST_ABOVE;
					cairo_new_path(cr);
					cairo_set_line_width(cr, zoom_factor *  MEASURE_LINE_THICK);
					cairo_move_to(cr, X_POS_PAGE_REL(start), Y_POS_STAFF_REL(0));
					cairo_line_to(cr, X_POS_PAGE_REL(start), Y_POS_STAFF_REL(repyoffs));
					cairo_stroke(cr);
					break;
			case REP2END:   line_bottom = bottom_ori;
					break;
		}
		if ( special_position == SPECIAL_POSITION_START && ((m_special_descr->type & (START_TYPE_MASK | END_TYPE_MASK)) != 0) && realtype == 0) return;
		if ( special_position == SPECIAL_POSITION_START && ((m_special_descr->type & REP_TYPE_MASK) == REPEAT_CLOSE)) return;
		switch (realtype & REP_TYPE_MASK) {
#define REP_OPEN_START_LINE_DIST 0.2
			case REPEAT_OPEN:
				if (special_position == SPECIAL_POSITION_END) break;
				line_xpos = (special_position == SPECIAL_POSITION_START) ? xpos + REP_OPEN_START_LINE_DIST : start;
				if (special_position == SPECIAL_POSITION_START) {
					normal_line_pos = line_xpos;
				}
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_arc(cr, X_POS_PAGE_REL(line_xpos + REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y1DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_arc(cr, X_POS_PAGE_REL(line_xpos + REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y2DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				sig_xpos += REP_DOTS_X_DIST;
				break;
			case REPEAT_CLOSE:
				if (special_position == SPECIAL_POSITION_END && spec_measure_at_end) break;
				line_xpos = (special_position == SPECIAL_POSITION_END) ? xpos : start;
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_arc(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST - REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y1DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_arc(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST - REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y2DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				draw_default_line = false;
				break;
			case END_BAR:
				if (special_position == SPECIAL_POSITION_END && spec_measure_at_end) break;
				if (special_position == SPECIAL_POSITION_START) return;
				line_xpos = (special_position == SPECIAL_POSITION_END) ? xpos : start;
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos - REP_THICK_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				draw_default_line = false;
				break;
			case DOUBLE_BAR:
				if (special_position == SPECIAL_POSITION_END && spec_measure_at_end) break;
				if (special_position == SPECIAL_POSITION_START) return;
				line_xpos = (special_position == SPECIAL_POSITION_END) ? xpos : start;
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(line_xpos - REP_DOUBLE_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(line_xpos - REP_DOUBLE_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				break;
			case REPEAT_OPEN_CLOSE:
				if (special_position == SPECIAL_POSITION_END && spec_measure_at_end) break;
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(start), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(start), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
				cairo_move_to(cr, X_POS_PAGE_REL(start + REP_THICK_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(start + REP_THICK_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_move_to(cr, X_POS_PAGE_REL(start - REP_THICK_LINE_DIST), Y_POS_SYS_REL(top));
				cairo_line_to(cr, X_POS_PAGE_REL(start - REP_THICK_LINE_DIST), Y_POS_SYS_REL(bottom));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, X_POS_PAGE_REL(start + REP_THICK_LINE_DIST + REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y1DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_arc(cr, X_POS_PAGE_REL(start + REP_THICK_LINE_DIST + REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y2DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				cairo_new_path(cr);
				cairo_arc(cr, X_POS_PAGE_REL(start - REP_THICK_LINE_DIST - REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y1DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_arc(cr, X_POS_PAGE_REL(start - REP_THICK_LINE_DIST - REP_DOTS_X_DIST), Y_POS_SYS_REL(bottom_ori - REP_DOTS_Y2DIST), zoom_factor * REP_DOTS_RADIUS, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				sig_xpos += 2 * REP_DOTS_X_DIST;
				//draw_default_line = false;
				break;
		}
		switch (realtype & TIMESIG_TYPE_MASK) {
			case TIMESIG: 
				if (special_position == SPECIAL_POSITION_START) {
					if ((realtype & REP_TYPE_MASK) == 0) return;
					break;
				}
				if (special_position == SPECIAL_POSITION_END && spec_measure_at_end) return;
				cairo_glyph_t glyph[2];
				int n;
				switch (m_special_descr->symbol) {
					case TIME_SYMBOL_COMMON_TIME:
						glyph[0].index = BASE + 56;
						glyph[0].x = X_POS_PAGE_REL(sig_xpos + m_staff_elements_width);
						glyph[0].y = Y_POS_STAFF_REL(5 * LINE_DIST/2.0);
						cairo_show_glyphs(cr, glyph, 1);
						cairo_stroke(cr);
						break;
					case TIME_SYMBOL_CUT_TIME:
						glyph[0].index = BASE + 57;
						glyph[0].x = X_POS_PAGE_REL(sig_xpos + m_staff_elements_width);
						glyph[0].y = Y_POS_STAFF_REL(5 * LINE_DIST/2.0);
						cairo_show_glyphs(cr, glyph, 1);
						cairo_stroke(cr);
						break;
					default:
						cairo_new_path(cr);
						n = m_special_descr->numerator;
						if (n > 9) {
							glyph[0].index = NUMBERBASE + n / 10;
							glyph[0].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width - SIG_HALF);
							glyph[0].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS + 4 * LINE_DIST);
							glyph[1].index = NUMBERBASE + n % 10;
							glyph[1].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width + SIG_HALF);
							glyph[1].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS + 4 * LINE_DIST);
							cairo_show_glyphs(cr, glyph, 2);
						}
						else {
					
							glyph[0].index = NUMBERBASE + n;
							glyph[0].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width);
							glyph[0].y = Y_POS_STAFF_REL(Y_NUMRATOR_POS + 4 * LINE_DIST);
							cairo_show_glyphs(cr, glyph, 1);
						}
						n = m_special_descr->denominator;
						if (n > 9) {
							glyph[0].index = NUMBERBASE + n / 10;
							glyph[0].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width - SIG_HALF);
							glyph[0].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS + 4 * LINE_DIST);
							glyph[1].index = NUMBERBASE + n % 10;
							glyph[1].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width + SIG_HALF);
							glyph[1].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS + 4 * LINE_DIST);
							cairo_show_glyphs(cr, glyph, 2);
						}
						else {
							glyph[0].index = NUMBERBASE + n;
							glyph[0].x =  X_POS_PAGE_REL(sig_xpos + m_staff_elements_width);
							glyph[0].y = Y_POS_STAFF_REL(Y_DENOMINATOR_POS + 4 * LINE_DIST);
							cairo_show_glyphs(cr, glyph, 1);
						}
						cairo_stroke(cr);
						break;
					}
				break;
		}
	}
	if ((special_position == SPECIAL_POSITION_END && spec_measure_at_end) || !draw_default_line) return;
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * ((special_position == SPECIAL_POSITION_END) ? 2 * MEASURE_LINE_THICK : MEASURE_LINE_THICK));
	cairo_move_to(cr, X_POS_PAGE_REL(normal_line_pos), Y_POS_SYS_REL(top));
	cairo_line_to(cr, X_POS_PAGE_REL(normal_line_pos), Y_POS_SYS_REL(line_bottom));
//#define SHOW_MEAS_NUM
#ifdef SHOW_MEAS_NUM
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, MEASURE_NUMBER_SIZE * zoom_factor);
	cairo_move_to(cr, X_POS_PAGE_REL(normal_line_pos - MEASURE_NUMBER_X_DIST), Y_POS_SYS_REL(top - MEASURE_NUMBER_Y_DIST));
	cairo_show_text(cr, m_measure_number_str);
#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(zoom_level));
#endif
#endif
	cairo_stroke(cr);
}

unsigned int NedMeasure::getStoredStaffCount() {return m_position_array.getStoredStaffCount();}

void NedMeasure::setNewStaffCount(int newstaffcount) {
	m_position_array.setNewStaffCount(newstaffcount);
}
