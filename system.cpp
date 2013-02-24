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
#include <gdk/gdkkeysyms.h>
#include "system.h"
#include "staff.h"
#include "page.h"
#include "chordorrest.h"
#include "resource.h"
#include "mainwindow.h"
#include "clipboard.h"
#include "commandlist.h"
#include "commandhistory.h"
#include "voice.h"
#include "movechordsandrestscommand.h"
#include "movechordsandrestsreversecommand.h"
#include "appendstaffcommand.h"
#include "mainwindow.h"
#include "musicxmlimport.h"
#include "pangocairotext.h"

#define DEFAULT_SYSTEM_DIST (4 * LINE_DIST)

#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)

#define Y_POS(p) ((p) * zoom_factor - topy)
#define Y_POS_INVERSE(p) (((p) / current_scale  + topy) / zoom_factor)
#define X_POS_PAGE_REL_INVERSE(p) (((p) + leftx) / zoom_factor - getPage()->getContentXpos())
#define X_POS_INVERSE(p) (((p) / current_scale + leftx) / zoom_factor)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS(p) ((height - (p)) * PS_ZOOM)

NedSystem::NedSystem(NedPage *page, double ypos, double width, int nr, unsigned int start_measure_number, bool start) :
m_system_start(CLEF_SPACE), m_is_positioned(false), m_staffs(NULL), m_deleted_staffs(NULL),
m_ypos(ypos), m_width(width), m_system_number(nr), m_has_repeat_lines(false),
m_page(page), m_measure_count(0), m_extra_space(0.0), m_only_whole_elements(FALSE), m_endMeasure(NULL), m_special_measure_at_end(0) {
	int i;
	double staff_pos = 0;
	NedStaff *staff;
	GList *lptr;
	if (page->getPageNumber() == 0 &&  nr == 0) {
		m_system_start += TIME_SIGNATURE_SPACE;
	}
	for (i = 0; i < MAX_MEASURES; m_measures[i++].setSystem(this));
	if (start) {
		for (i = 0; i < getMainWindow()->getStaffCount(); i++) {
			m_staffs = g_list_append(m_staffs,
				staff = new NedStaff(this,
			 	staff_pos, m_width, i, TRUE));
			staff_pos += staff->getHeight() + DEFAULT_SYSTEM_DIST;
		}
		do {
			for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
				((NedStaff *) lptr->data)->appendWholeRest();
			}
			//do_reposit();
			reposit(start_measure_number);
		}
		while (m_extra_space > MAXIMUM_EXTRA_SPACE);

	}
}
	
NedSystem::~NedSystem() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		delete ((NedStaff *) (lptr->data));
	}
	g_list_free(m_staffs);
	m_staffs = NULL;

	for (lptr = g_list_first(m_deleted_staffs); lptr; lptr = g_list_next(lptr)) {
		delete ((NedStaff *) (lptr->data));
	}
	g_list_free(m_deleted_staffs);
	m_deleted_staffs = NULL;
}

NedSystem *NedSystem::clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedPage *p_page, bool *staves) {
	GList *lptr;
	int i, j;
	NedSystem *system = new NedSystem(p_page, m_ypos, m_width, m_system_number, getNumberOfFirstMeasure(), false);

	for (i = j = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		if (!staves[i]) continue;
		system->m_staffs = g_list_append(system->m_staffs, ((NedStaff *) lptr->data)->clone(addrlist, slurlist, system, j++));
	}

	return system;
}

void NedSystem::adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->adjust_pointers(addrlist, slurlist);
	}
}


NedStaff *NedSystem::getStaff(int nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, nr)) == NULL) {
		return NULL;
	}
	return (NedStaff *) lptr->data;
}


void NedSystem::fill_up(NedCommandList *command_list) {
	GList *lptr;
	do {
		for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
			((NedStaff *) lptr->data)->appendWholeRest(command_list);
		}
		compute_extra_space(false);
		//do_reposit(false);
	}
	while (m_extra_space > MINIMUM_EXTRA_SPACE);
}

double NedSystem::getHeight() {
	NedStaff *staff;

	staff = (NedStaff *) g_list_last(m_staffs)->data;
	return staff->getBottomPos();
}

double NedSystem::getBottomPos() {
	NedStaff *staff;

	staff = (NedStaff *) g_list_last(m_staffs)->data;
	return m_ypos  + staff->getBottomPos();
}

NedMainWindow *NedSystem::getMainWindow() {return m_page->getMainWindow();}

/*
bool NedSystem::find_new_cursor_pos(guint keyval, int *x, int *y, double *dist) {
	GList *lptr;
	double newx = x, newy = y;
	bool found = false;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (((NedSystem *) lptr->data)->find_new_cursor_pos(keyval, &newx, &newy, dist)) {
			found = true;
			*x = newx; *y = newy;
		}
	}
	return found;
}
*/

void NedSystem::draw(cairo_t *cr, bool first_page, bool show_measure_numbers, bool *freetexts_or_lyrics_present) {
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	NedStaff *firststaff, *laststaff;
	NedStaff *staff;
	double brace_start_y_pos = -1.0;
	double bracket_start_y_pos = -1.0;
	bool inside_connected_staves = false;
	bool inside_bracket = false;
	int i;
	double botp, botp1, botp2;
	double indent = first_page ? getMainWindow()->getFirstSystemIndent() : getMainWindow()->get2ndSystemIndent();

	GList *lptr1, *lptr2;
	lptr1 = g_list_first(m_staffs);
	lptr2 = g_list_next(lptr1);
	m_freetexts_or_lyrics_present = false;
	for (i = 0, lptr2 = g_list_next(lptr1); lptr1; lptr1 = lptr2, lptr2 = g_list_next(lptr2), i++) {
		if ((getMainWindow()->m_staff_contexts[i].m_flags & CON_BAR_START) != 0) {
			inside_connected_staves = true;
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & CON_BAR_END) != 0) {
			inside_connected_staves = false;
		}
		botp1 = botp2 = ((NedStaff *) lptr1->data)->getBottomPos();
		if (lptr2) botp2 = ((NedStaff *) lptr2->data)->getBottomPos();
		if (inside_connected_staves) {
			if (lptr2 == NULL) {
				NedResource::Abort("NedSystem::draw");
			}
			botp = botp2;
		}
		else {
			botp = botp1;
		}

		((NedStaff *) lptr1->data)->draw(cr, botp,  first_page, indent, &m_freetexts_or_lyrics_present);
	}
	firststaff = (NedStaff *) g_list_first(m_staffs)->data;
	laststaff = (NedStaff *) g_list_last(m_staffs)->data;
	if (show_measure_numbers && (m_system_number != 0 || !first_page)) {
		cairo_new_path(cr);
		cairo_move_to(cr, X_POS_PAGE_REL(indent + MEASURE_NUMBER_X_POS), Y_POS(m_ypos + firststaff->getTopPos() + MEASURE_NUMBER_Y_POS));
		cairo_select_font_face(cr, "Sans", MEASURE_NUMBER_SLANT, MEASURE_NUMBER_WEIGHT);
		cairo_set_font_size(cr, MEASNUMBER_SIZE * zoom_factor);
		cairo_show_text(cr, m_measures[0].getNumberString());
		cairo_stroke(cr);
#ifdef HAS_SET_SCALED_FONT
		cairo_set_scaled_font (cr, scaled_font);
#else
		cairo_set_font_face(cr, NedResource::getFontFace());
		cairo_set_font_matrix(cr,  NedResource::getFontMatrix(getMainWindow()->getCurrentZoomLevel()));
#endif
	}
	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
#ifdef BADLINEPOLICY
	cairo_set_line_width(cr, zoom_factor * STAFF_START_LEFT_THICK);
#else
	cairo_set_line_width(cr, zoom_factor * MEASURE_LINE_THICK);
#endif
	if (g_list_length(m_staffs) > 1) {
		cairo_move_to(cr, X_POS_PAGE_REL(indent),
		 	Y_POS(m_ypos + firststaff->getTopPos() - LINE_THICK / 2.0));
		cairo_line_to(cr,  X_POS_PAGE_REL(indent),
		 	Y_POS(m_ypos + laststaff->getBottomPos() + LINE_THICK / 2.0));
	}
#ifdef BADLINEPOLICY
	if (!m_special_measure_at_end) { // i.e there is a key signature at the end
			cairo_move_to(cr, X_POS_PAGE_REL(m_width),
		 	Y_POS(m_ypos + firststaff->getTopPos() - LINE_THICK / 2.0));
		cairo_line_to(cr,  X_POS_PAGE_REL(m_width),
		 	Y_POS(m_ypos + laststaff->getBottomPos() + LINE_THICK / 2.0));
	}
#endif
	cairo_stroke(cr);
//#define SHOW_MEASURES_END
#ifdef SHOW_MEASURES_END
	cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * 4 * STEM_THICK);
	cairo_move_to(cr, X_POS_PAGE_REL(m_measures[m_measure_count].end), Y_POS(m_ypos + firststaff->getTopPos() - LINE_THICK / 2.0));
	cairo_line_to(cr,  X_POS_PAGE_REL(m_measures[m_measure_count].end), Y_POS(m_ypos + laststaff->getBottomPos() + LINE_THICK / 2.0));
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
	cairo_new_path(cr);
	cairo_move_to(cr, X_POS_PAGE_REL(m_measures[m_measure_count].end - m_extra_space), Y_POS(m_ypos + firststaff->getTopPos() + 3 * LINE_THICK));
	cairo_line_to(cr, X_POS_PAGE_REL(m_measures[m_measure_count].end), Y_POS(m_ypos + firststaff->getTopPos() + 3 * LINE_THICK));

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_stroke(cr);
#endif
	for (i = 0; i < getMainWindow()->getStaffCount(); i++) {
		staff = (NedStaff *) g_list_nth(m_staffs, i)->data;
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACE_START) != 0) {
			brace_start_y_pos = staff->getTopPos();
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACE_END) != 0) {
			if (brace_start_y_pos >= 0.0) {
				drawBrace(cr, leftx, indent - (inside_bracket ? 2 * NESTED_BRACE_INDENT : BRACE_SPACE) , topy, zoom_factor, staff->getBottomPos() - brace_start_y_pos, brace_start_y_pos);
				if (first_page) {
					if (getMainWindow()->m_staff_contexts[i].m_group_name != NULL) {
						m_freetexts_or_lyrics_present = true;
					}
				}
				else if (getMainWindow()->m_staff_contexts[i].m_group_short_name != NULL) {
						m_freetexts_or_lyrics_present = true;
				}
				brace_start_y_pos = -1.0;
			}
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACKET_START) != 0) {
			bracket_start_y_pos = staff->getTopPos();
			inside_bracket = true;
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACKET_END) != 0) {
			if (bracket_start_y_pos >= 0.0) {
				drawBracket(cr, leftx, indent, topy, zoom_factor, bracket_start_y_pos, staff->getBottomPos());
				if (first_page) {
					if (getMainWindow()->m_staff_contexts[i].m_group_name != NULL) {
						m_freetexts_or_lyrics_present = true;
					}
				}
				else if (getMainWindow()->m_staff_contexts[i].m_group_short_name != NULL) {
						m_freetexts_or_lyrics_present = true;
				}
				bracket_start_y_pos = -1.0;
			}
			inside_bracket = false;
		}
	}
	if (m_freetexts_or_lyrics_present) *freetexts_or_lyrics_present = true;
	if (m_endMeasure != NULL) {
		cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
		cairo_rectangle (cr,  X_POS_PAGE_REL(m_endMeasure->start + REP_THICK_LINE_DIST / 3.0), Y_POS(m_ypos + firststaff->getTopPos() + firststaff->getTopYBorder() - 2 * REP_THICK_LINE_DIST ),
			(m_width - (m_endMeasure->start) + (LEFT_RIGHT_BORDER - 2 * DEFAULT_BORDER)) * zoom_factor,
			(laststaff->getBottomPos() + laststaff->getBottomYBorder() - (firststaff->getTopPos() + firststaff->getTopYBorder()) + 4 * REP_THICK_LINE_DIST) * zoom_factor);
		cairo_fill(cr);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}

};

void NedSystem::drawTexts(cairo_t *cr, bool first_page, double scale) {
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	NedStaff *staff;
	double brace_start_y_pos = -1.0;
	double bracket_start_y_pos = -1.0;
	int i;
	bool group_written;

	if (!m_freetexts_or_lyrics_present) return;
	for (i = 0; i < getMainWindow()->getStaffCount(); i++) {
		staff = (NedStaff *) g_list_nth(m_staffs, i)->data;
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACE_START) != 0) {
			brace_start_y_pos = staff->getTopPos();
		}
		group_written = false;
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACE_END) != 0) {
			if (brace_start_y_pos >= 0.0) {
				if (first_page) {
					if (getMainWindow()->m_staff_contexts[i].m_group_name != NULL) {
					group_written = true;
					getMainWindow()->m_staff_contexts[i].m_group_name->draw(cr,
					((getPage()->getContentXpos()) * zoom_factor - leftx - BRACE_X_OFFSET) * scale +
					(getMainWindow()->getNettoIndent() - getMainWindow()->m_staff_contexts[i].m_group_name->getInternalWidth()),
		 			Y_POS(m_ypos + brace_start_y_pos + (staff->getTopPos() + 4 * LINE_DIST - brace_start_y_pos - LINE_DIST) / 2.0) * scale, zoom_factor, scale);
					}
				}
				else if (getMainWindow()->m_staff_contexts[i].m_group_short_name != NULL) {
					group_written = true;
					getMainWindow()->m_staff_contexts[i].m_group_short_name->draw(cr,
					((getPage()->getContentXpos()) * zoom_factor  - leftx - BRACE_X_OFFSET) * scale +
					(getMainWindow()->get2ndNettoIndent() - getMainWindow()->m_staff_contexts[i].m_group_short_name->getInternalWidth()),
		 			Y_POS(m_ypos + brace_start_y_pos + (staff->getTopPos() + 4 * LINE_DIST - brace_start_y_pos - LINE_DIST) / 2.0)  * scale, zoom_factor, scale);
				}
			}
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACKET_START) != 0) {
			bracket_start_y_pos = staff->getTopPos();
		}
		if ((getMainWindow()->m_staff_contexts[i].m_flags & BRACKET_END) != 0) {
			if (bracket_start_y_pos >= 0.0) {
				if (!group_written) {
					if (first_page) {
						if (getMainWindow()->m_staff_contexts[i].m_group_name != NULL) {
						getMainWindow()->m_staff_contexts[i].m_group_name->draw(cr,
						((getPage()->getContentXpos()) * zoom_factor - leftx) * scale +
						(getMainWindow()->getNettoIndent() - getMainWindow()->m_staff_contexts[i].m_group_name->getInternalWidth()),
		 				Y_POS(m_ypos + bracket_start_y_pos + (staff->getTopPos() + 4 * LINE_DIST - bracket_start_y_pos - LINE_DIST) / 2.0)  * scale, zoom_factor, scale);
						}
					}
					else if (getMainWindow()->m_staff_contexts[i].m_group_short_name != NULL) {
						getMainWindow()->m_staff_contexts[i].m_group_short_name->draw(cr,
						((getPage()->getContentXpos()) * zoom_factor  - leftx) * scale +
						(getMainWindow()->get2ndNettoIndent() - getMainWindow()->m_staff_contexts[i].m_group_short_name->getInternalWidth()),
		 				Y_POS(m_ypos + bracket_start_y_pos + (staff->getTopPos() + 4 * LINE_DIST - bracket_start_y_pos - LINE_DIST) / 2.0)  * scale, zoom_factor, scale);
					}
					bracket_start_y_pos = -1.0;
				}
			}
		}
		staff->drawTexts(cr, first_page, scale);
	}
};


void NedSystem::drawBrace (cairo_t *cr, double leftx, double indent, double topy, double zoom_factor, double height, double toppos) {
	int i;
	cairo_new_path(cr);
	cairo_set_line_width(cr, 1.0);
#define BRACE_OFFS -0.03
#define BRACE_X_FACTOR 0.004
#define BRACE_Y_FACTOR 0.00106
#define FAKX(a) (BRACE_X_FACTOR * zoom_factor * a)
#define FAKY(a) (BRACE_Y_FACTOR * zoom_factor * a * height)
	cairo_move_to(cr, X_POS_PAGE_REL(indent-0.05), Y_POS(m_ypos + toppos + BRACE_OFFS));
	for (i = 0; i < 16; i++) {
		cairo_rel_curve_to(cr, FAKX(NedResource::m_braceOutline[i][0]), FAKY(NedResource::m_braceOutline[i][1]),
				   FAKX(NedResource::m_braceOutline[i][2]), FAKY(NedResource::m_braceOutline[i][3]),
				   FAKX(NedResource::m_braceOutline[i][4]), FAKY(NedResource::m_braceOutline[i][5]));
	}
	cairo_fill(cr);
}

void NedSystem::drawBracket (cairo_t *cr, double leftx, double indent, double topy, double zoom_factor, double toppos, double botpos) {
	int i;
	cairo_new_path(cr);
	cairo_set_line_width(cr, 1.0);
#define BRACKET_X_OFFS -0.2
#define BRACKET_Y_OFFS 0.1
#define BRACKET_X_FACTOR 0.0008
#define BRACKET_Y_FACTOR 0.0008
#define BRACKET_FAKX(a) (BRACKET_X_FACTOR * zoom_factor * a)
#define BRACKET_FAKY(a) (BRACKET_Y_FACTOR * zoom_factor * a)
#define BRACKET_THICK 0.1
#define BRACKET_LINE_DIST 0.05
	cairo_move_to(cr, X_POS_PAGE_REL(indent+BRACKET_X_OFFS), Y_POS(m_ypos + botpos  + BRACKET_Y_OFFS));
	for (i = 0; i < 5; i++) {
		cairo_rel_curve_to(cr, BRACKET_FAKX(NedResource::m_bracketEndOutline[i][0]), BRACKET_FAKY(NedResource::m_bracketEndOutline[i][1]),
				   BRACKET_FAKX(NedResource::m_bracketEndOutline[i][2]), BRACKET_FAKY(NedResource::m_bracketEndOutline[i][3]),
				   BRACKET_FAKX(NedResource::m_bracketEndOutline[i][4]), BRACKET_FAKY(NedResource::m_bracketEndOutline[i][5]));
	}
	cairo_close_path(cr);
	cairo_fill(cr);
	cairo_new_path(cr);
	cairo_move_to(cr, X_POS_PAGE_REL(indent+BRACKET_X_OFFS), Y_POS(m_ypos + toppos  - BRACKET_Y_OFFS));
	for (i = 0; i < 5; i++) {
		cairo_rel_curve_to(cr, BRACKET_FAKX(NedResource::m_bracketEndOutline[i][0]), BRACKET_FAKY(-NedResource::m_bracketEndOutline[i][1]),
				   BRACKET_FAKX(NedResource::m_bracketEndOutline[i][2]), BRACKET_FAKY(-NedResource::m_bracketEndOutline[i][3]),
				   BRACKET_FAKX(NedResource::m_bracketEndOutline[i][4]), BRACKET_FAKY(-NedResource::m_bracketEndOutline[i][5]));
	}
	cairo_close_path(cr);
	cairo_fill(cr);
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * BRACKET_THICK);
	cairo_move_to(cr, X_POS_PAGE_REL(indent+BRACKET_X_OFFS - BRACKET_LINE_DIST), Y_POS(m_ypos + toppos - BRACKET_Y_OFFS));
	cairo_line_to(cr, X_POS_PAGE_REL(indent+BRACKET_X_OFFS - BRACKET_LINE_DIST), Y_POS(m_ypos + botpos + BRACKET_Y_OFFS));
	cairo_stroke(cr);

}

void NedSystem::empty() {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->empty();
	}
}

bool NedSystem::hasOnlyRests() {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (!((NedStaff *) lptr->data)->hasOnlyRests()) return false;
	}
	return true;
}


bool NedSystem::trySelect(double x, double y, bool only_free_placeables) {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (((NedStaff *) lptr->data)->trySelect(x, y, only_free_placeables)) {
			return TRUE;
		}
	}
	return FALSE;
}

NedChordOrRest *NedSystem::findNearestElement(int staff_nr, double x, double y, NedStaff *oldstaff, double *mindist, double *ydist) {
	NedChordOrRest *nearestElement = NULL, *element;
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (staff_nr >= 0 && ((NedStaff *) lptr->data)->getStaffNumber() != staff_nr) continue;
		if ((element = ((NedStaff *) lptr->data)->findNearestElement(x, y, oldstaff, mindist, ydist)) != NULL) {
			nearestElement = element;
		}
	}
	return nearestElement;
}

bool NedSystem::tryChangeLength(NedChordOrRest *chord_or_rest) {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (((NedStaff *) lptr->data)->tryChangeLength(chord_or_rest)) {
			return TRUE;
		}
	}
	return FALSE;
}

double NedSystem::computeMidDist(double y) {
	double tp, bt;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();

	tp = Y_POS(m_ypos + ((NedStaff *) g_list_first(m_staffs)->data)->getTopPos());
	bt = Y_POS(m_ypos + ((NedStaff *) g_list_last(m_staffs)->data)->getBottomPos());

	y /= getMainWindow()->getCurrentScale();


	if (y < tp) {
		return tp - y;
	}
	if (y > bt) {
		return y - bt;
	}
	return 0.0;
}

bool NedSystem::testLineOverflow() {
	compute_extra_space(false);
	//do_reposit(false);
	return m_extra_space < MINIMUM_EXTRA_SPACE && m_measure_count > 1;
}

void NedSystem::removeLastImported() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->removeLastImported();
	}
	m_measure_count--;
}

bool NedSystem::truncateAtStart(NedCommandList *command_list, unsigned long long midi_time) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (!((NedStaff *) lptr->data)->truncateAtStart(command_list, midi_time)) return false;
	}
	return true;
}

bool NedSystem::handleImportedTuplets() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (!((NedStaff *) lptr->data)->handleImportedTuplets()) return false;
	}
	return true;
}

void NedSystem::prepareForImport() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->prepareForImport();
	}
}

void NedSystem::appendElementsOfMeasureLength(part *parts, unsigned int meas_duration, int meas_num) {
	NedStaff *staff;
	part *part_ptr;
	int i;
	GList *lptr;

	for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		staff = (NedStaff *) lptr->data;
		part_ptr = &(parts[i]);
		staff->appendElementsOfMeasureLength(part_ptr, meas_duration, meas_num);
	}
}

void NedSystem::copyDataOfWholeStaff(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::copyDataOfWholeStaff");
	}

	((NedStaff *) lptr->data)->copyDataOfWholeStaff();
}

void NedSystem::handleStaffElements() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->handleStaffElements();
	}
}

void NedSystem::setInternalPitches() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setInternalPitches();
	}
}

void NedSystem::adjustAccidentals(int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::adjustAccidentals");
	}
	((NedStaff *) lptr->data)->adjustAccidentals();
}

void NedSystem::changeAccidentals(NedCommandList *command_list, int preferred_offs, bool *staff_list, GList *selected_group) {
	GList *lptr;
	int i;

	for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		if (!staff_list[i]) continue;
		((NedStaff *) lptr->data)->changeAccidentals(command_list, preferred_offs, selected_group);
	}
}

void NedSystem::transpose(int pitchdist, bool *staff_list, GList *selected_group) {
	GList *lptr;
	int i;

	for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		if (!staff_list[i]) continue;
		((NedStaff *) lptr->data)->transpose(pitchdist, selected_group);
	}
}

void NedSystem::hideRests(NedCommandList *command_list, bool unhide, int staff_nr, int voice_nr) {
	GList *lptr;

	if (staff_nr < 0) {
		for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
			((NedStaff *) lptr->data)->hideRests(command_list, unhide, voice_nr);
		}
		return;
	}
	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::hideRests");
	}
	((NedStaff *) lptr->data)->hideRests(command_list, unhide, voice_nr);
}

bool NedSystem::shiftNotes(unsigned long long start_time, int linedist, int staff_number) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_number)) == NULL) {
		NedResource::Abort("NedSystem::shiftNotes");
	}
	return ((NedStaff *) lptr->data)->shiftNotes(start_time, linedist);
}

void NedSystem::removeUnneededAccidentals(int staff_nr) {
	GList *lptr;

	if (staff_nr < 0) {
		for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
			((NedStaff *) lptr->data)->removeUnneededAccidentals();
		}
	}
	else {
		if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
			NedResource::Abort("NedSystem::removeUnneededAccidentals");
		}
		((NedStaff *) lptr->data)->removeUnneededAccidentals();
	}
}


bool NedSystem::tryInsertOrErease(double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj, bool force_rest) {
	GList *lptr;
	double mindist, d;
	lptr = g_list_first(m_staffs);
	NedStaff *nearest_staff = (NedStaff *) lptr->data;
	mindist = nearest_staff->computeMidDist(y);
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if ((d = ((NedStaff *) lptr->data)->computeMidDist(y)) < mindist) {
			nearest_staff = (NedStaff *) lptr->data;
			mindist = d;
		}
	}
	if (nearest_staff->tryInsertOrErease(x, y, num_midi_input_notes, midi_input_chord, newObj, force_rest)) {
		return true;
	}
	return false;
}

bool NedSystem::findLine(double x, double y, double *ypos, int *line, double *bottom, NedStaff **staff) {
	GList *lptr;
	double mindist, d;
	lptr = g_list_first(m_staffs);
	NedStaff *nearest_staff = (NedStaff *) lptr->data;
	mindist = nearest_staff->computeMidDist(y);
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if ((d = ((NedStaff *) lptr->data)->computeMidDist(y)) < mindist) {
			nearest_staff = (NedStaff *) lptr->data;
			mindist = d;
		}
	}
	if (nearest_staff->findLine(x, y, ypos, line, bottom)) {
		*staff = nearest_staff;
		return TRUE;
	}
	return FALSE;
}

bool NedSystem::findElement(guint keyval, double x, double y, NedChordOrRest **element, NedStaff **staff) {
	double leftx = getMainWindow()->getLeftX();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	int i;
	double dummy1, dummy3;
	int dummy2;
	double xx;

	if (!findLine(x, y, &dummy1, &dummy2, &dummy3, staff) ) return FALSE;
	xx = X_POS_PAGE_REL_INVERSE(x);
	switch(keyval) {
		case GDK_Right:
			for (i = 0; i < m_measure_count; i++) {
				if (m_measures[i].start <= xx && m_measures[i].end >= xx) {
					return (*staff)->findFirstElementeGreaterThan(m_measures[i].end, element);
				}
			}
			return false;
		case GDK_Left:
			for (i = m_measure_count - 1; i >= 0; i--) {
				if (m_measures[i].start <= xx && m_measures[i].end >= xx) {
					return (*staff)->findLastElementeLessThan(m_measures[i].start, element);
				}
			}
			return false;
	}
	return false;
}

NedStaff *NedSystem::findStaff(double x, double y, NedMeasure **measure) {
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double topy = getMainWindow()->getTopY();
	double current_scale = getMainWindow()->getCurrentScale();
	double yl = Y_POS_INVERSE(y) - m_ypos;
	int i;
	GList *lptr;

	x -= getPage()->getContentXpos();
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (((NedStaff *) lptr->data)->getTopPos() < yl && ((NedStaff *) lptr->data)->getBottomPos() > yl) {
			for (i = 0; i <= m_measure_count; i++) {
				if (m_measures[i].isNearStart(x)) {
					*measure = &(m_measures[i]);
				}
			}
			return (NedStaff *) lptr->data;
		}
	}
	return NULL;
}

void NedSystem::appendStaff(NedCommandList *command_list, int p_staff_nr /* = -1 */) {
	NedStaff *staff;
	int staff_nr;

	if (p_staff_nr < 0) {
		staff_nr = g_list_length(m_staffs);
	}
	else {
		staff_nr = p_staff_nr;
	}
	staff = new NedStaff(this, 10.0 /* dummy */, m_width, staff_nr, TRUE);
	if (getMainWindow()->getUpBeatInverse() > 0 && m_page->getPageNumber() == 0 && m_system_number == 0) {
		staff->setUpbeatRests(getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator() - getMainWindow()->getUpBeatInverse());
		staff->appendAppropriateWholes(1);
	}
	else {
		staff->appendAppropriateWholes(0);
	}

	command_list->addCommand(new NedAppendStaffCommand(this, staff));
}

NedChordOrRest *NedSystem::getFirstObjectInNextSystem(int staff_nr) {
	NedSystem *system;
	GList *lptr;

	if ((system = m_page->getNextSystem(this)) == NULL) return NULL;

	if ((lptr = g_list_nth(system->m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::getFirstObjectInNextSystem");
	}
	lptr = ((NedStaff *) lptr->data)->getFirstChordOrRest(0, 0, false, false);
	if (lptr == NULL) return NULL;
	return (NedChordOrRest *) lptr->data;
}

void NedSystem::do_append_staff(NedStaff *staff) {
	m_staffs = g_list_append(m_staffs, staff);
}

void NedSystem::do_remove_staff(NedStaff *staff) {
	GList *lptr;

	if ((lptr = g_list_find(m_staffs, staff)) == NULL) {
		NedResource::Abort("NedSystem::do_remove_staff");
	}
	m_staffs = g_list_delete_link(m_staffs, lptr);
}

void NedSystem::cutEmptyVoices() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->cutEmptyVoices();
	}
}

void NedSystem::handleEmptyMeasures() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->handleEmptyMeasures();
	}
}

void NedSystem::zoomFreeReplaceables(double zoom, double scale) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->zoomFreeReplaceables(zoom, scale);
	}
}

void NedSystem::recomputeFreeReplaceables() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->recomputeFreeReplaceables();
	}
}

void NedSystem::testTies() {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->testTies();
	}
}

void NedSystem::removeLastStaff() {
	GList *lptr;
	NedStaff *staff;


	if (g_list_length(m_staffs) < 2) {
		NedResource::Abort("NedSystem::removeLastStaff");
	}
	lptr = g_list_last(m_staffs);
	staff = (NedStaff *) lptr->data;
	delete staff;
	m_staffs = g_list_delete_link(m_staffs, lptr);
}

void NedSystem::deleteStaff(int staff_number) {
	GList *lptr;
	NedStaff *staff;
	int i;

	if ((lptr = g_list_nth(m_staffs, staff_number)) == NULL) {
		NedResource::Abort("NedSystem::deleteStaff");
	}
	staff = (NedStaff *) lptr->data;
	m_staffs = g_list_delete_link(m_staffs, lptr);
	m_deleted_staffs = g_list_append(m_deleted_staffs, staff);
	for (i = 0, lptr = g_list_first(m_staffs); lptr; i++, lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setStaffNumber(i);
	}
}

void NedSystem::restoreStaff(int staff_number) {
	GList *lptr;
	NedStaff *staff;
	int pos, i;

	if ((lptr = g_list_last(m_deleted_staffs)) == NULL) {
		NedResource::Abort("NedSystem::restoreStaff");
	}
	staff = (NedStaff *) lptr->data;
	m_deleted_staffs = g_list_delete_link(m_deleted_staffs, lptr);
	pos = staff->getStaffNumber();
	if (pos != staff_number) {
		NedResource::Abort("NedSystem::restoreStaff: pos != staff_number");
	}
	m_staffs = g_list_insert(m_staffs, staff, pos);
	for (i = 0, lptr = g_list_first(m_staffs); lptr; i++, lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setStaffNumber(i);
	}
}


void NedSystem::shiftStaff(int staff_number, unsigned int position) {
	GList *lptr;
	NedStaff *staff;
	int i;

	if (position > g_list_length(m_staffs)) {
		NedResource::Abort("NedSystem::shiftStaff(1)");
	}
	if ((lptr = g_list_nth(m_staffs, staff_number)) == NULL) {
		NedResource::Abort("NedSystem::shiftStaff(2)");
	}
	staff = (NedStaff *) lptr->data;
	m_staffs = g_list_delete_link(m_staffs, lptr);
	m_staffs = g_list_insert(m_staffs, staff, position);
	for (i = 0, lptr = g_list_first(m_staffs); lptr; i++, lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setStaffNumber(i);
	}
}


bool NedSystem::reposit(int measure_number, NedCommandList *command_list /* = NULL */, NedSystem **next_system /* = NULL */) {
	GList *lptr;
	NedSystem *other_system;
	int newnum, i;
	unsigned long long midi_time = 0;
	unsigned int meas_duration;
	bool use_upbeat = (m_system_number == 0 && m_page->getPageNumber() == 0);
	int critical_measure;
	//bool changed = false;
	//bool out = (m_system_number == 6 && m_page->getPageNumber() == 2);
	

	if (next_system != NULL) *next_system = m_page->getNextSystem(this);
	if (m_is_positioned) {
		return false;
	}

	if (measure_number < 0) {
		measure_number = m_measures[0].getMeasureNumber();
	}
	for (i = 0; i < MAX_MEASURES; i++) {
		m_measures[i].midi_start = midi_time;
		meas_duration = getMainWindow()->getMeasureDuration(measure_number);
		m_measures[i].setMeasureNumber(measure_number++, getMainWindow()->m_special_measures /* friend !! */, true);
		if (!m_measures[i].m_special_spread) {
			m_measures[i].m_spread_fac = getMainWindow()->getGlopabSpreadFac();
		}
		midi_time += meas_duration;
		m_measures[i].midi_end = midi_time;
	}
		
	if (command_list == NULL) {
		do_reposit(use_upbeat);
		return false;
	}
	compute_extra_space(use_upbeat);
	//do_reposit(use_upbeat);
	m_check_line_compression = true;

	if (m_extra_space < MINIMUM_EXTRA_SPACE && m_measure_count > 1) {
		//printf("pag: %d sys: %d: m_extra_space(%f) < MINIMUM_EXTRA_SPACE (%f)reposit\n", m_page->getPageNumber(), m_system_number, m_extra_space, MINIMUM_EXTRA_SPACE); fflush(stdout);
		m_check_line_compression = FALSE;
		//critical_measure = m_special_measure_at_end ? m_measure_count-2 : m_measure_count-1;
		critical_measure = m_measure_count-1;
		//int xx = 0;
		NedClipBoard *board = new NedClipBoard();
		if (m_measures[critical_measure].m_measure_has_only_staff_members) critical_measure--;
		board->setRefTimeAndDuration(m_measures[critical_measure].midi_start, m_measures[critical_measure].midi_end - m_measures[critical_measure].midi_start);
		for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
			((NedStaff *) lptr->data)->collectChordsAndRests(board, m_measures[critical_measure].midi_start,  m_measures[critical_measure].midi_end);
		}
		if ((board->onlyWholeRests() && m_page->isLastSystem(this))) {
			board->setDeleteOnly();
			other_system = NULL;
		}
		else {
			other_system = m_page->getNextSystem(this, command_list);
			if (next_system != NULL)  *next_system = other_system;
		}
		//changed = true;
		newnum = m_measures[m_measure_count-1].getMeasureNumber();
		if (other_system != NULL) {
			other_system->collectDestinationVoices(board);
			// Do not add this because if unexecute the syszem is already deleted command_list->addSystemForRepositioning(other_system);
			compute_extra_space(use_upbeat);
			//do_reposit(use_upbeat);
			newnum = m_measures[m_measure_count-1].getMeasureNumber() + 1;
			other_system->m_is_positioned = false;
			if (other_system != NULL && getMainWindow()->m_start_measure_number_for_renumbering > newnum) {
				getMainWindow()->m_start_system_for_renumbering = other_system;
				getMainWindow()->m_start_measure_number_for_renumbering = newnum;
			}
		}
		NedMoveChordsAndRestsCommand *command = new NedMoveChordsAndRestsCommand(board);
		command_list->addCommand(command);
		command->execute();
	}
	if (m_check_line_compression && m_extra_space > MAXIMUM_EXTRA_SPACE) {
		//printf("pag: %d sys: %d : m_extra_space(%f) > MAXIMUM_EXTRA_SPACE (%f)reposit\n", m_page->getPageNumber(), m_system_number, m_extra_space, MAXIMUM_EXTRA_SPACE); fflush(stdout);
		if ((other_system = m_page->getNextSystem(this)) != NULL) {
			NedClipBoard *board = new NedClipBoard();
			if (other_system->collectFirstMeasure(board, getMainWindow()->getMeasureDuration(m_measures[m_measure_count].getMeasureNumber()), getSystemEndTime())) {
				collectDestinationVoices(board);
				if (board->onlyWholeRests() && m_page->isLastSystem(this)) {
					board->setInsertOnly();
				}
				board->execute_reverse();
				compute_extra_space(use_upbeat);
				//do_reposit(use_upbeat);
				if (m_extra_space < MINIMUM_EXTRA_SPACE) {
					board->unexecute_reverse();
					compute_extra_space(use_upbeat);
					//do_reposit(use_upbeat);
					other_system->do_reposit(false); // the ping-pong elements have bad x position
					m_check_line_compression = false;
				}
				else {
					//board->unexecute_reverse();
					NedMoveChordsAndRestsReverseCommand *command = new NedMoveChordsAndRestsReverseCommand(board);
					for (lptr = g_list_first(other_system->m_staffs); lptr; lptr = g_list_next(lptr)) {
						((NedStaff *) lptr->data)->assignMidiTimes();
					}
					//changed = true;
					command_list->addCommand(command);
					//command->execute(); do not execut because is already done
					command_list->addSystemForRepositioning(other_system);
					if (board->onlyWholeRests() && m_page->isLastSystem(this)) {
						board->setInsertOnly();
					}
					compute_extra_space(use_upbeat);
					//do_reposit(use_upbeat);
					newnum = m_measures[0].getMeasureNumber();
					if (other_system != NULL && getMainWindow()->m_start_measure_number_for_renumbering > newnum) {
						getMainWindow()->m_start_system_for_renumbering = this;
						getMainWindow()->m_start_measure_number_for_renumbering = newnum;
					}
				}
			}
			else {
				m_check_line_compression = FALSE;
			}
			other_system->m_is_positioned = false;
		}
		else {
			m_check_line_compression = FALSE;
			if (command_list != NULL) {
				fill_up(command_list);
			}
		}
	}
	do_reposit(use_upbeat);
	m_is_positioned = (m_extra_space >= MINIMUM_EXTRA_SPACE || m_measure_count < 2)  && (!m_check_line_compression || m_extra_space <= MAXIMUM_EXTRA_SPACE);
	/*
	if (out) {
		printf("raus mit %d, m_extra_space >= MINIMUM_EXTRA_SPACE = %d, m_measure_count = %d, m_check_line_compression = %d, m_extra_space <= MAXIMUM_EXTRA_SPACE = %d\n",
		!m_is_positioned, m_extra_space >= MINIMUM_EXTRA_SPACE, m_measure_count, m_check_line_compression , m_extra_space <= MAXIMUM_EXTRA_SPACE ); fflush(stdout);
	}
	*/
	return !m_is_positioned;
}

bool NedSystem::findTimeOfMeasure(int meas_num, unsigned long long *meas_time) {
	int i;

	for (i = 0; i < m_measure_count; i++) {
		if (m_measures[i].getMeasureNumber() == meas_num) {
			*meas_time = m_measures[i].midi_start;
			return true;
		}
	}
	return false;
}

void NedSystem::renumberMeasures(int *measure_number, GList *special_measures, bool force) {
	int i;


	for (i = 0; i <= m_measure_count; i++) {
		m_measures[i].setMeasureNumber((*measure_number)++, special_measures, force);
	}
	(*measure_number)--;
}

void NedSystem::setNewMeasureSpread(double spread) {
	for (int i = 0; i < MAX_MEASURES; i++) {
		m_measures[i].m_spread_fac = spread;
		m_measures[i].m_special_spread = false;
	}
}

int NedSystem::getNumberOfFirstMeasure() {
	return m_measures[0].getMeasureNumber();
}

int NedSystem::getNumberOfLastMeasure() {
	return m_measures[m_measure_count - 1].getMeasureNumber();
}

unsigned long long NedSystem::getSystemEndTime() {
	return m_measures[m_measure_count - 1].midi_end;
}


double NedSystem::placeStaffs(double staffpos) {
#define SYSTEM_MIN_DIST 0.08
	GList *lptr;
	m_ypos = staffpos;

	double yy = 0.0;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		yy = ((NedStaff *) lptr->data)->placeStaff(yy);
	}
	return yy + m_ypos + SYSTEM_MIN_DIST;
}

bool NedSystem::collectFirstMeasure(NedClipBoard *board, unsigned long long length_of_first_measure, unsigned long long length_of_previous_system) {
	GList *lptr;
	if (m_measure_count < 2) return FALSE;
	m_measures[0].midi_start = 0;
	m_measures[0].midi_end = length_of_first_measure; // preliminary times importatnt is the correct duration. Under some circumstances
	                                                  // collectFirstMeasure is called before the measure got their time
	board->setRefTimeAndDuration(length_of_previous_system, m_measures[0].midi_end);
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->collectChordsAndRests(board, m_measures[0].midi_start,  m_measures[0].midi_end);
	}
	return TRUE;
}

void NedSystem::collectSelectionRectangleElements(double xp, NedBbox *sel_rect, GList **sel_group,
		NedSystem *first_selected_system, NedSystem *last_selected_system,
		bool is_first_selected_page, bool is_last_selected_page) {
	GList *lptr;
	NedStaff *firststaff, *laststaff;

	firststaff = (NedStaff *) g_list_first(m_staffs)->data;
	laststaff = (NedStaff *) g_list_last(m_staffs)->data;
	double yp = firststaff->getTopPos() + firststaff->getTopYBorder() + m_ypos;
	if (is_last_selected_page && sel_rect->y + sel_rect->height < yp) return;

	if (is_first_selected_page && m_ypos + laststaff->getBottomPos() < sel_rect->y) return;
	yp = sel_rect->y - m_ypos;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->collectSelectionRectangleElements(xp, yp, sel_rect, sel_group,
		is_first_selected_page && (first_selected_system == this), is_last_selected_page && (last_selected_system == this));
	}
}

void NedSystem::findSelectedSystems(NedBbox *sel_rect, int *number_of_first_selected_staff, 
		int *number_of_last_selected_staff, NedSystem **first_selected_system, NedSystem **last_selected_system) {
	GList *lptr;
	NedStaff *firststaff, *laststaff;

	firststaff = (NedStaff *) g_list_first(m_staffs)->data;
	laststaff = (NedStaff *) g_list_last(m_staffs)->data;
	double yp = firststaff->getTopPos() + m_ypos;
	if (sel_rect->y + sel_rect->height < yp || yp + laststaff->getBottomPos() - firststaff->getTopPos() < sel_rect->y) return;
	if (*first_selected_system == NULL) {
		*first_selected_system = *last_selected_system = this;
	}
	else {
		*last_selected_system = this;
	}
	if (first_selected_system == last_selected_system) {
		*number_of_first_selected_staff = 0;
		*number_of_last_selected_staff = g_list_length(m_staffs) - 1;
	}
	else {
		double yp = sel_rect->y - m_ypos;
/*
		if (m_page->getPageNumber() == 0) {
			printf("yp = %f, m_ypos = %f, m_ypos + 4 * LINE_DIST = %f\n", yp, m_ypos, m_ypos + 4 * LINE_DIST); fflush(stdout);
		}
*/
		for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
			((NedStaff *) lptr->data)->findSelectedStaffs(yp, sel_rect, number_of_first_selected_staff, number_of_last_selected_staff);
		}
	}
}

void NedSystem::setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, double indent, bool first) {
	GList *lptr;
	m_system_start = CLEF_SPACE;

	if (getMainWindow()->isTimsigChangingMeasure(getNumberOfFirstMeasure())) {
		m_system_start += SIG_X_SHIFT;
	}
	if (first) {
		m_system_start += TIME_SIGNATURE_SPACE;
	}
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setAndUpdateClefTypeAndKeySig(clef_and_key_array, first);
	}
	m_system_start += indent; // do this here because otherwise the staves
				  // do not update m_system_start correctly
}

void NedSystem::determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found) {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->determineTempoInverse(element, till, tempoinverse, found);
	}
}

GList *NedSystem::getFirstChordOrRest(int staff_nr, int voice_nr, int lyrics_line, bool lyrics_required, bool note_required) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("getFirstChordOrRest");
	}
	return ((NedStaff *) lptr->data)->getFirstChordOrRest(voice_nr, lyrics_line, lyrics_required, note_required);
}

GList *NedSystem::getLastChordOrRest(int staff_nr, int voice_nr, int lyrics_line, bool lyrics_required, bool note_required) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("getFirstChordOrRest");
	}
	return ((NedStaff *) lptr->data)->getLastChordOrRest(voice_nr, lyrics_line, lyrics_required, note_required);
}

bool NedSystem::hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, int to_staff, unsigned long long start_midi_time) {
	GList *lptr;
	if ((lptr = g_list_nth(m_staffs, to_staff)) == NULL) {
		NedResource::Abort("NedSystem::hasTupletConflict");
	}
	return ((NedStaff *) lptr->data)->hasTupletConflict(meas_duration, elements, from_staff, start_midi_time);
}


void NedSystem::pasteElements(NedCommandList *command_list, GList **elements, int from_staff, int to_staff, unsigned long long start_midi_time) {
	GList *lptr;
	if ((lptr = g_list_nth(m_staffs, to_staff)) == NULL) {
		NedResource::Abort("NedSystem::pasteElements");
	}
	((NedStaff *) lptr->data)->pasteElements(command_list, elements, from_staff, start_midi_time);
}

bool NedSystem::findFromTo(GList *clipboard, NedSystem **min_sys, NedSystem **max_sys) {
	GList *lptr;
	bool found = FALSE;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		if (((NedStaff *) lptr->data)->findFromTo(clipboard)) {
			found = TRUE;
			if (*min_sys == NULL) {
				*min_sys = *max_sys = this;
			}
			else {
				*max_sys = this;
			}
		}
	}
	return found;
}
bool NedSystem::findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi) {
	GList *lptr;
	bool found = FALSE, fnd;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		fnd = ((NedStaff *) lptr->data)->findStartMeasureLimits(clipboard, start_midi);
		found = found || fnd;
	}
	return found;
}

bool NedSystem::findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi) {
	GList *lptr;
	bool found = FALSE, fnd;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		fnd = ((NedStaff *) lptr->data)->findEndMeasureLimits(clipboard, end_midi);
		found = found || fnd;
	}
	return found;
}

void NedSystem::deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->deleteItemsFromTo(command_list, is_first, is_last, start_midi, end_midi);
	}
}

void NedSystem::removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->removeNotesFromTo(command_list, items, is_first, is_last);
	}
}

void NedSystem::insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->insertBlocks(command_list, blockcount, midi_time);
	}
}

void NedSystem::checkForElementsToSplit(NedCommandList *command_list, int *measure_number) {
	GList *lptr;
	int i;
	unsigned int meas_duration;
	unsigned long long midi_time = 0;
	unsigned long long end_time = getSystemEndTime();

	for (i = 0; midi_time < end_time && i < MAX_MEASURES; i++) {
		m_measures[i].midi_start = midi_time;
		meas_duration = getMainWindow()->getMeasureDuration(*measure_number);
		m_measures[i].setMeasureNumber((*measure_number)++, getMainWindow()->m_special_measures /* friend !! */, true);
		midi_time += meas_duration;
		m_measures[i].midi_end = midi_time;
	}

	m_measure_count = i; // otherwise " NedVoice::checkForElementsToSplit" crashes, because it cannot find the appropriate measure
                             // if the measure count increases/decreases
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->checkForElementsToSplit(command_list);
	}
}

void NedSystem::testForPageBackwardTies(NedCommandList *command_list) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->testForPageBackwardTies(command_list);
	}
}

void NedSystem::collectDestinationVoices(NedClipBoard *board) {
	GList *lptr;

	board->element_counter = g_list_first(board->m_elements);
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->collectDestinationVoices(board);
	}

}

void NedSystem::assignMidiTimes() {
	GList *lptr;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->assignMidiTimes();
	}
}


void NedSystem::do_reposit(bool use_upbeat) {
	GList *lptr;
	int num_elements;
	int i;
	double current_position;
	double shift;
	double extra_space_divided;
	double extra_width_at_end;
	int new_measure_count;
	int grace_positions[MAX_POSITIONS];
	double extra_space_for_accidentals;
	bool measure_has_only_staff_members;
	m_special_measure_at_end = new_measure_count = 0;
	if (m_measures[0].getStoredStaffCount() != g_list_length(m_staffs)) {
		for (i = 0; i < MAX_MEASURES; i++) {
			m_measures[i].m_position_array.setNewStaffCount(g_list_length(m_staffs));
		}
	}
	for (i = 0; i < MAX_MEASURES; i++) {
		m_measures[i].m_position_array.empty();
		m_measures[i].m_staff_elements_width = 0.0;
	}
	current_position = m_system_start - m_measures[0].getNeededSpaceAfter();
	if (m_measures[0].getSpecial() == REPEAT_OPEN || m_measures[0].getSpecial() == REPEAT_OPEN_CLOSE) {
		current_position +=  m_measures[0].getNeededSpaceAfter();
	}

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->assignMidiTimes();
	}
	memset(grace_positions, 0, sizeof(grace_positions));
	for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		new_measure_count = ((NedStaff *) lptr->data)->assignElementsToMeasures(m_measures, VOICE_COUNT * i, use_upbeat, &m_special_measure_at_end, m_system_number == 0);
		new_measure_count++;
		m_measure_count = new_measure_count;
	}
	if (m_special_measure_at_end) {
		m_measures[m_measure_count+1].m_staff_elements_width = 0.2;
		m_measures[m_measure_count+1].m_position_array.empty();
	}
	if (((m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == REPEAT_CLOSE) || ((m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == REPEAT_OPEN_CLOSE) ||
		(m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == END_BAR) {
		extra_width_at_end = m_measures[m_measure_count+m_special_measure_at_end].getNeededSpaceBefore();
	}
	else {
		extra_width_at_end = 0.0;
	}
	num_elements = 0;
	extra_space_for_accidentals = 0.0;
	current_position = m_measures[0].computeAndSetPositions(current_position, &num_elements, &extra_space_for_accidentals,  &measure_has_only_staff_members, false);
	if (m_measures[0].getSpecial() & (START_TYPE_MASK | END_TYPE_MASK)) {
	}
	m_endMeasure = NULL;
	m_has_repeat_lines = false;
	for (i = 1; i < m_measure_count+m_special_measure_at_end; i++) {
		measure_has_only_staff_members = true;
		current_position = m_measures[i].computeAndSetPositions(current_position, &num_elements, &extra_space_for_accidentals, &measure_has_only_staff_members, false);
		if (m_measures[i].barRequiresHideFollowing()) {
			m_endMeasure = &(m_measures[i]);
		}
		m_measures[i-1].end = m_measures[i].start;
		if (m_measures[i].getSpecial() & (START_TYPE_MASK | END_TYPE_MASK)) {
			m_has_repeat_lines = true;
		}
	}
	m_measures[i-1].end = m_width - RIGHT_DIST;
	if (m_measures[i].getSpecial() & (START_TYPE_MASK | END_TYPE_MASK)) {
		m_has_repeat_lines = true;
	}
	num_elements += m_measure_count+m_special_measure_at_end;
	m_extra_space = m_width - current_position - extra_space_for_accidentals - m_measures[m_measure_count+m_special_measure_at_end+1].getNeededSpaceBefore() - RIGHT_DIST - extra_width_at_end;
	/*
	if (m_page->getPageNumber() == 0 && m_system_number == 0) {
		printf("m_extra_space(%f) = m_width(%f) - current_position(%f) - extra_space_for_accidentals(%f) - nb(%f) -  RIGHT_DIST(%f)\n",
				m_extra_space, m_width, current_position, extra_space_for_accidentals, 
					m_measures[m_measure_count+m_special_measure_at_end+1].getNeededSpaceBefore(), RIGHT_DIST); fflush(stdout);
	}
	*/
	extra_space_divided = m_extra_space / (num_elements + 2);
	shift = 0;
	if (extra_space_divided > 0.0) {
		for (i = 0; i < m_measure_count+m_special_measure_at_end+1; i++) {
#define BBB
#ifdef BBB
			shift = m_measures[i].shiftXpos(extra_space_divided, shift, m_page->getPageNumber() == 0 && m_system_number == 0);
#endif
			if (i > 0) {
				m_measures[i-1].end = m_measures[i].start;
			}
		}
	}
	m_measures[i-2].end = m_measures[i-1].start = m_width - RIGHT_DIST;
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->searchForBeamGroups(use_upbeat ? getMainWindow()->getUpBeatInverse() : 0);
		((NedStaff *) lptr->data)->computeBeams();
		((NedStaff *) lptr->data)->computeTuplets(m_has_repeat_lines);
		((NedStaff *) lptr->data)->computeTies();
	}
	if (m_special_measure_at_end) { /* means last measure consists of only a key signature */
		/*
		m_measures[i-2].start -= 0.15;
		m_measures[i-1].end = m_measures[i-2].start;
		*/
	}
}

void NedSystem::computeTuplets() {
	GList *lptr;
	int i;
	
	m_has_repeat_lines = false;
	for (i = 0; i < m_measure_count; i++) {
		if (m_measures[i].getSpecial() & (START_TYPE_MASK | END_TYPE_MASK)) {
			m_has_repeat_lines = true;
		}
	}
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->computeBeams();
		((NedStaff *) lptr->data)->computeTuplets(m_has_repeat_lines);
	}
}

void NedSystem::resetActiveFlags() {
	GList *lptr;
	
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->resetActiveFlags();
	}
}

void NedSystem::compute_extra_space(bool use_upbeat) {
	GList *lptr;
	int num_elements;
	int i;
	double current_position;
	double extra_width_at_end;
	int new_measure_count;
	int grace_positions[MAX_POSITIONS];
	double extra_space_for_accidentals;
	bool measure_has_only_staff_members;
	new_measure_count = 0;
	if (m_measures[0].getStoredStaffCount() != g_list_length(m_staffs)) {
		for (i = 0; i < MAX_MEASURES; i++) {
			m_measures[i].m_position_array.setNewStaffCount(g_list_length(m_staffs));
		}
	}
	for (i = 0; i < MAX_MEASURES; i++) {
		m_measures[i].m_position_array.empty();
		m_measures[i].m_staff_elements_width = 0.0;
	}
	current_position = m_system_start - m_measures[0].getNeededSpaceAfter();
	if (m_measures[0].getSpecial() == REPEAT_OPEN || m_measures[0].getSpecial() == REPEAT_OPEN_CLOSE) {
		current_position +=  m_measures[0].getNeededSpaceAfter();
	}

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->assignMidiTimes();
	}
	memset(grace_positions, 0, sizeof(grace_positions));
	for (i = 0, lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr), i++) {
		new_measure_count = ((NedStaff *) lptr->data)->assignElementsToMeasures(m_measures, VOICE_COUNT * i, use_upbeat, &m_special_measure_at_end, m_system_number == 0);
		new_measure_count++;
		m_measure_count = new_measure_count;
	}
	if (m_special_measure_at_end) {
		m_measures[m_measure_count+1].m_staff_elements_width = 0.2;
		m_measures[m_measure_count+1].m_position_array.empty();
	}
	if (((m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == REPEAT_CLOSE) || ((m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == REPEAT_OPEN_CLOSE) ||
		(m_measures[m_measure_count+m_special_measure_at_end].getSpecial() & REP_TYPE_MASK) == END_BAR) {
		extra_width_at_end = m_measures[m_measure_count+m_special_measure_at_end].getNeededSpaceBefore();
	}
	else {
		extra_width_at_end = 0.0;
	}
	num_elements = 0;
	extra_space_for_accidentals = 0.0;
	current_position = m_measures[0].computeAndSetPositions(current_position, &num_elements, &extra_space_for_accidentals,  &measure_has_only_staff_members, false);
	m_endMeasure = NULL;
	for (i = 1; i < m_measure_count+m_special_measure_at_end; i++) {
		measure_has_only_staff_members = true;
		current_position = m_measures[i].computeAndSetPositions(current_position, &num_elements, &extra_space_for_accidentals, &measure_has_only_staff_members, false);
			if (m_measures[i].barRequiresHideFollowing()) {
				m_endMeasure = &(m_measures[i]);
			}
			m_measures[i-1].end = m_measures[i].start;
	}
	m_measures[i-1].end = m_width - RIGHT_DIST;
	num_elements += m_measure_count+m_special_measure_at_end;
	m_extra_space = m_width - current_position - extra_space_for_accidentals - m_measures[m_measure_count+m_special_measure_at_end+1].getNeededSpaceBefore() - RIGHT_DIST - extra_width_at_end;
	/*
	if (m_page->getPageNumber() == 0 && m_system_number == 0) {
		printf("m_extra_space(%f) = m_width(%f) - current_position(%f) - extra_space_for_accidentals(%f) - nb(%f) -  RIGHT_DIST(%f)\n",
				m_extra_space, m_width, current_position, extra_space_for_accidentals, 
					m_measures[m_measure_count+1].getNeededSpaceBefore(), RIGHT_DIST); fflush(stdout);
	}
	*/
	//if (measure_has_only_staff_members) { /* means last measure consists of only a key signature */
		/*
		m_measures[i-2].start -=  0.15;
		m_measures[i].end = m_measures[i-2].start;
		*/
//	}
}

unsigned int NedSystem::getWholeMidiLength() {
	return m_measure_count * getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
}


NedMeasure *NedSystem::getMeasure(unsigned long long  midi_time) {
	int i;
	for (i = 0; i < m_measure_count; i++) {
		if (m_measures[i].midi_start <= midi_time && m_measures[i].midi_end > midi_time) {
			return (&(m_measures[i]));
		}
	}
#ifdef VVV
	NedResource::DbgMsg(DBG_TESTING, "pag: %d, sys. %d, midi_time = %llu(%llu, %llu) , m_measures[m_measure_count(%d)].midi_start = %llu(%llu,%llu), m_measures[m_measure_count(%d)].midi_end = %llu(%llu, %llu)\n",
	m_page->getPageNumber(), m_system_number,
	LAUS(midi_time), m_measure_count, m_measures[m_measure_count].midi_start, m_measure_count, m_measures[m_measure_count].midi_end, NOTE_4); 
	if (m_measure_count >= 0) {
		NedResource::DbgMsg(DBG_TESTING, "m_measures[m_measure_count - 1(%d)].midi_start = %llu,  m_measures[m_measure_count - 1(%d)].midi_end = %llu\n",
		m_measure_count-1, m_measures[m_measure_count-1].midi_start, m_measure_count-1, m_measures[m_measure_count-1].midi_end); 
	}
	NedResource::Abort("NedSystem::getMeasure");
#endif
	return NULL;
}

int NedSystem::getNumberOfStaffs() {
	return g_list_length(m_staffs);
}


void NedSystem::prepareReplay(bool with_keysig) {
	GList *lptr;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->prepareReplay(with_keysig);
	}
}

void NedSystem::setWidth(double w) {
	GList *lptr;

	m_width = w;

	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->setWidth(m_width);
	}
}

void NedSystem::saveSystem(FILE *fp) {
	int i;
	GList *lptr;
	fprintf(fp, "SYSTEM %d: %d MEASURES\n", m_system_number, m_measure_count);
	for (i = 0; i < m_measure_count; i++) {
		fprintf(fp, "MEASURE: %d %f\n", m_measures[i].getMeasureNumber(), m_measures[i].m_spread_fac);
	}
	for (lptr = g_list_first(m_staffs); lptr; lptr = g_list_next(lptr)) {
		((NedStaff *) lptr->data)->saveStaff(fp);
	}
}

void NedSystem::restoreSystem(FILE *fp) {
	char buffer[128];
	int i, measnum, staffnum;
	NedStaff *staff;
	double staff_pos = 0;

	if (!NedResource::readInt(fp, &m_measure_count)) {
		NedResource::m_error_message = "Measure count expected";
		return;
	}
	if (m_measure_count < 0 || m_measure_count >= MAX_MEASURES) {
		NedResource::m_error_message = "Bad measure count";
	}

	if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "MEASURES")) {
		NedResource::m_error_message = "MEASURES expected";
		return;
	}
	for (i = 0; NedResource::m_error_message == NULL && i < m_measure_count; i++) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "MEASURE")) {
			NedResource::m_error_message = "MEASURE expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
			NedResource::m_error_message = ": expected";
			return;
		}
		if (getMainWindow()->getFileVersion() > 14) {
			if (!NedResource::readInt(fp, &measnum)) {
				NedResource::m_error_message = "measure number expected";
				return;
			}
		}
		if (getMainWindow()->getFileVersion() > 9) {
			if (!NedResource::readFloat(fp, &(m_measures[i].m_spread_fac))) {
				NedResource::m_error_message = "spread factor expected";
				return;
			}
		}
		else {
			if (!NedResource::readInt(fp, &measnum)) {
				NedResource::m_error_message = "MeasureNumber expected";
				return;
			}
			if (i != measnum) {
				NedResource::m_error_message = "Bad measure number";
				return;
			}
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": expected";
				return;
			}
			if (!NedResource::readLong(fp, (unsigned long long *) &(m_measures[i].midi_start))) {
				NedResource::m_error_message = "Midi start value expected";
				return;
			}
			if (!NedResource::readLong(fp, (unsigned long long *) &(m_measures[i].midi_end))) {
				NedResource::m_error_message = "Midi end value expected";
				return;
			}
		}
	}
	for (i = 0; NedResource::m_error_message == NULL && i < getMainWindow()->getPreStaffCount(); i++) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "STAFF")) {
			NedResource::m_error_message = "STAFF expected(2)";
			return;
		}
		if (!NedResource::readInt(fp, &staffnum)) {
			NedResource::m_error_message = "STAFF number expected";
			return;
		}
		if (staffnum != i) {
			NedResource::m_error_message = "Bad STAFF number";
			return;
		}
		staff = new NedStaff(this, staff_pos, m_width, i, FALSE);

		m_staffs = g_list_append(m_staffs, staff);
		staff->restoreStaff(fp);

		staff_pos += staff->getHeight() + DEFAULT_SYSTEM_DIST;
	}
}

void NedSystem::detectVoices(int staff_nr, unsigned int *voice_mask, NedSystem **last_system, unsigned long long *end_time) {
	GList *lptr;
	unsigned long long e_time = 0;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::detectVoices");
	}
	if (((NedStaff *) lptr->data)->detectVoices(voice_mask, &e_time)) {
		if (*last_system == NULL) {
			*last_system = this;
			*end_time = e_time;
		}
		else if ((*last_system)->m_page->getPageNumber() < m_page->getPageNumber()) {
			*last_system = this;
			*end_time = e_time;
		}
		else if ((*last_system)->m_page->getPageNumber() == m_page->getPageNumber() && (*last_system)->m_system_number < m_system_number) {
			*last_system = this;
			*end_time = e_time;
		}
		else if ((*last_system)->m_page->getPageNumber() == m_page->getPageNumber() && (*last_system)->m_system_number == m_system_number && *last_system == this) {
			if (e_time > *end_time) {
				*end_time = e_time;
			}
		}
	}
}

void NedSystem::exportLilyPond(FILE *fp, int staff_nr, int voice_nr, int *last_line, unsigned int *midi_len,
		NedSystem *last_system, unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur,
			unsigned int *lyrics_map, bool with_break, bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::exportLilyPond");
	}
	((NedStaff *) lptr->data)->exportLilyPond(fp, voice_nr, last_line, midi_len, last_system == this, end_time, in_alternative,
		lily_slur, lyrics_map, with_break, guitar_chordnames, chordnames, breath_script, keep_beams);
}

void NedSystem::exportLilyGuitarChordnames(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
		NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::exportLilyGuitarChordnames");
	}
	((NedStaff *) lptr->data)->exportLilyGuitarChordnames(fp, last_line, midi_len, last_system == this, end_time, in_alternative, with_break);
}

void NedSystem::exportLilyFreeChordName(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
		NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::exportLilyFreeChordName");
	}
	((NedStaff *) lptr->data)->exportLilyFreeChordName(fp, last_line, midi_len, last_system == this, end_time, in_alternative, with_break);
}

void NedSystem::exportLilyLyrics(FILE *fp, bool last_system, int staff_nr, int voice_nr, int line_nr, unsigned long long end_time, int *sil_count) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::exportLilyLyrics");
	}
	((NedStaff *) lptr->data)->exportLilyLyrics(fp, last_system, voice_nr, line_nr, end_time, sil_count);
}

void NedSystem::collectLyrics(NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::collectLyrics");
	}

	((NedStaff *) lptr->data)->collectLyrics(leditor);
}

void NedSystem::setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;

	if ((lptr = g_list_nth(m_staffs, staff_nr)) == NULL) {
		NedResource::Abort("NedSystem::setLyrics");
	}

	((NedStaff *) lptr->data)->setLyrics(command_list, leditor);
}
