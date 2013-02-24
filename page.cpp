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
#include <math.h>
#include <gdk/gdkkeysyms.h>
#include "page.h"
#include "system.h"
#include "mainwindow.h"
#include "clipboard.h"
#include "staff.h"
#include "deletesystemcommand.h"
#include "appendsystemcommand.h"
#include "movesystemcommand.h"
#include "removepagecommand.h"
#include "insertfreeplaceablecommand.h"
#include "getsystemfromnextpagecmmand.h"
#include "commandhistory.h"
#include "commandlist.h"
#include "chordorrest.h"
#include "volumesign.h"
#include "temposign.h"
#include "scoreinfodialog.h"
#include "slurpoint.h"
#include "slur.h"
#include "linepoint.h"
#include "crescendo.h"
#include "linesdialog.h"
#include "octavation.h"
#include "acceleration.h"
#include "freesign.h"
#include "freechord.h"
#include "freespacer.h"
#include "pangocairotext.h"
#include "freechordname.h"


#define X_POS_INVERS(p) ((leftx + (p) / current_scale) / zoom_factor - m_xpos)
#define Y_POS_INVERS(p) ((topy + (p) / current_scale) / zoom_factor)
#define X_POS_INVERS_PAGE_REL(p) (((leftx + (p) / current_scale) / zoom_factor - getContentXpos()))

#define X_POS(p) (((p) + m_xpos) * zoom_factor - leftx)
#define Y_POS(p) ((p) * zoom_factor - topy)

#define X_POS_PAGE_REL(p) (((p) + getContentXpos()) * zoom_factor - leftx)

#define DEFAULT_SYSTEM_DIST (2 * 5 * LINE_DIST)
NedPage::NedPage(NedMainWindow *main_window, double width, double height, int nr, unsigned int start_measure_number, bool start) :
m_systems(NULL), m_xpos(nr * (width + DEFAULT_BORDER)),
m_width(width), m_height(height), default_border(DEFAULT_BORDER), m_page_number(nr), m_main_window(main_window)
{
	int i;
	double system_pos = TOP_BOTTOM_BORDER;
	NedSystem *system;


	if (start) {
		i = 0;
		m_system_diff = 100.0;
		m_systems = g_list_append(m_systems, system = new NedSystem(this, system_pos,
			m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER), i, start_measure_number, TRUE));
		placeStaffs(0);
		/*
		do {
			m_systems = g_list_append(m_systems, system = new NedSystem(this, system_pos,
				m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER), i, start_measure_number, TRUE));
			system_pos += system->getHeight() + DEFAULT_SYSTEM_DIST;
			i++;
			placeStaffs(0);
		}
		while (m_system_diff > MIN_SYSTEM_Y_DIST);
		if (g_list_length(m_systems) > 1) {
			lptr = g_list_last(m_systems);
			delete ((NedSystem *) lptr->data);
			m_systems = g_list_delete_link(m_systems, lptr);
			placeStaffs(0);
		}
		*/
	}
}

NedPage::~NedPage() {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		delete ((NedSystem *) lptr->data);
	}
	g_list_free(m_systems);
	m_systems = NULL;
}

NedPage *NedPage::clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedMainWindow *p_main_window, bool *staves) {
	GList *lptr;
	NedPage *page = new NedPage(p_main_window, m_width, m_height, m_page_number, ((NedSystem *)g_list_first(m_systems)->data)->getNumberOfFirstMeasure(), false);

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		page->m_systems = g_list_append(page->m_systems, ((NedSystem *) lptr->data)->clone(addrlist, slurlist, page, staves));
	}

	return page;
}

void NedPage::adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->adjust_pointers(addrlist, slurlist);
	}
}

#ifdef XXX
void NedPage::removeAllBeams() {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->removeAllBeams();
	}
}
#endif


double NedPage::getTopPos() {return (2 * default_border +  m_height) * m_main_window->getCurrentZoomFactor();}
double NedPage::getLeftPos() {return (( m_xpos + 2*default_border +  m_width)) * m_main_window->getCurrentZoomFactor();}

NedSystem *NedPage::appendSystem() {
	int system_nr;
	NedSystem *system;

	system_nr = g_list_length(m_systems);
	m_systems = g_list_append(m_systems, system = new NedSystem(this, 0,
	                                m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER), system_nr, 1, TRUE));

	placeStaffs(0);

	return system;
}

void NedPage::insertSystem(NedSystem *system) {
	
	m_systems = g_list_prepend(m_systems, system);
	system->changePageInfo(this);
}


void NedPage::removeSystem(NedSystem *system) {
	GList *lptr;
	int system_nr;

	if ((lptr = g_list_find(m_systems, system)) == NULL) {
		NedResource::Abort("NedPage::removeSystem");
	}
	m_systems = g_list_delete_link(m_systems, lptr);

	for (system_nr = 0, lptr = g_list_first(m_systems); lptr; system_nr++, lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->setSystemNumber(system_nr);
	}
}

void NedPage::appendSystem(NedCommandList *command_list) {
	NedAppendSystemCommand *append_sys_cmd = new NedAppendSystemCommand(this);
	append_sys_cmd->execute();
	command_list->addCommand(append_sys_cmd);
}

bool NedPage::isPageOverflow() {
	if (g_list_length(m_systems) < 2) return false;
	do_place_staffs(0.0);
	return m_system_diff <= MIN_SYSTEM_Y_DIST;
}


unsigned int NedPage::getNumberOfLastMeasure() {
	GList *lptr;
	lptr = g_list_last(m_systems);
	if (lptr == NULL) {
		NedResource::Abort("NedPage::getNumberOfLastMeasure");
	}
	return ((NedSystem *) g_list_last(m_systems)->data)->getNumberOfLastMeasure();
}

int NedPage::getSorting(NedStaff *this_staff, NedSystem *this_system, NedStaff *other_staff, NedSystem *other_system) {
	int pos0, pos1;
	if (this_staff->getStaffNumber() != other_staff->getStaffNumber()) {
		return SORTING_NONE;
	}
	if ((pos0 = g_list_index(m_systems, this_system)) < 0) {
		NedResource::Warning("NedPage::getSorting: didn't foind system 0");
		return SORTING_NONE;
	}
	if ((pos1 = g_list_index(m_systems, other_system)) < 0) {
		NedResource::Warning("dNedPage::getSorting: idn't foind system 1");
		return SORTING_NONE;
	}
	if (pos0 + 1 == pos1) {
		return SORTING_GREAT;
	}
	if (pos1 + 1 == pos0) {
		return SORTING_LESS;
	}
	return SORTING_NONE;
}

bool NedPage::isFirst(NedSystem *system) {
	GList *lptr;
	if ((lptr = g_list_find(m_systems, system)) == NULL) {
		 NedResource::Abort("NedPage::isFirst");
	}
	return (lptr == g_list_first(m_systems));
}

bool NedPage::isLast(NedSystem *system) {
	GList *lptr;
	if ((lptr = g_list_find(m_systems, system)) == NULL) {
		 NedResource::Abort("NedPage::isLast");
	}
	return (lptr == g_list_last(m_systems));
}

NedSystem *NedPage::getLastSystem() {
	return (NedSystem *) g_list_last(m_systems)->data;
}


bool NedPage::find_staff_and_line(int x, int y, NedStaff **last_staff, int *last_line) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	double bottom;

	double xl = X_POS_INVERS(x);
	double yl2;


	if (xl < 0.0 || xl > m_width) return false;


	return findLine(x, y, &yl2, last_line, &bottom, last_staff);
}



void NedPage::draw(cairo_t *cr, bool show_measure_numbers, double main_width, double main_height) {
	double leftx = m_main_window->getLeftX();
	double topy = m_main_window->getTopY();
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double scale = m_main_window->getCurrentScale();
	ScoreInfo *score_info = NULL;
	double xp, yp;
	bool first = true;
	bool freetext_or_lyrics_present;
	cairo_scale(cr, m_main_window->getCurrentScale(), m_main_window->getCurrentScale());

	if (!m_main_window->doDrawPostscript()) {
		if ((m_xpos * zoom_factor - leftx) * scale > main_width) {cairo_identity_matrix(cr); return;}
		if (((m_xpos  + (DEFAULT_BORDER + m_width)) * zoom_factor - leftx)  * scale < 0) {cairo_identity_matrix(cr); return;}
	
	
		cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);
		cairo_rectangle (cr,  m_xpos * zoom_factor - leftx, -topy,
			(2 * DEFAULT_BORDER + m_width) * zoom_factor,
			(2 * DEFAULT_BORDER + m_height) * zoom_factor);
	
		cairo_fill(cr);
		cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
		cairo_rectangle (cr,  (m_xpos + DEFAULT_BORDER) * zoom_factor -leftx,
			DEFAULT_BORDER * zoom_factor - topy,
		 	m_width * zoom_factor, m_height * zoom_factor);
		cairo_fill(cr);
	

		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		xp = m_xpos;
	}
	else {
		cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
		cairo_rectangle (cr,  (DEFAULT_BORDER) * zoom_factor,
			DEFAULT_BORDER * zoom_factor,
		 	m_width * zoom_factor, m_height * zoom_factor);
		cairo_fill(cr);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		xp = 0.0;
	}
	if (m_page_number == 0) {
		cairo_identity_matrix(cr);
		yp = 0.0;
		score_info = getMainWindow()->getScoreInfo();
		if (m_main_window->getScoreInfo()->title != NULL) {
			score_info->title->draw(cr, ((xp + DEFAULT_BORDER + (m_width - score_info->title_extends.width) / 2.0) * zoom_factor -leftx ) * scale,
				(STAFF_TOP_DIST + score_info->title_extends.height) * zoom_factor - topy, zoom_factor, scale);
			yp += score_info->title_extends.height + 4.0 * SCORE_INFO_EXTRA_DIST;
		}
		if (m_main_window->getScoreInfo()->subject != NULL) {
			score_info->subject->draw(cr, ((xp + DEFAULT_BORDER + (m_width - score_info->subject_extends.width) / 2.0) * zoom_factor -leftx) * scale,
				(STAFF_TOP_DIST + yp + score_info->subject_extends.height) * zoom_factor - topy, zoom_factor, scale);
			yp += score_info->subject_extends.height;
		}
		if (m_main_window->getScoreInfo()->composer != NULL) {
			score_info->composer->draw(cr, ((xp + DEFAULT_BORDER + (m_width - score_info->composer_extends.width - DEFAULT_BORDER - LEFT_RIGHT_BORDER)) * zoom_factor -leftx) * scale,
				(STAFF_TOP_DIST + yp + score_info->composer_extends.height) * zoom_factor - topy, zoom_factor, scale);
			yp += score_info->composer_extends.height + SCORE_INFO_EXTRA_DIST;
		}
		if (m_main_window->getScoreInfo()->arranger != NULL) {
			score_info->arranger->draw(cr, ((xp + DEFAULT_BORDER + (m_width - score_info->arranger_extends.width - DEFAULT_BORDER - LEFT_RIGHT_BORDER)) * zoom_factor -leftx) * scale,
				(STAFF_TOP_DIST + yp + score_info->arranger_extends.height) * zoom_factor - topy, zoom_factor, scale);
			yp += score_info->arranger_extends.height + SCORE_INFO_EXTRA_DIST;
		}
		if (m_main_window->getScoreInfo()->copyright != NULL) {
			score_info->copyright->draw(cr, ((xp + DEFAULT_BORDER + (m_width - score_info->copyright_extends.width - DEFAULT_BORDER - LEFT_RIGHT_BORDER)) * zoom_factor -leftx) * scale,
				(STAFF_TOP_DIST + yp + score_info->copyright_extends.height) * zoom_factor - topy, zoom_factor, scale);
		}
		cairo_scale(cr, m_main_window->getCurrentScale(), m_main_window->getCurrentScale());
	}
	GList *lptr;
	freetext_or_lyrics_present = false;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->draw(cr, m_page_number == 0 && first, show_measure_numbers, &freetext_or_lyrics_present);
		first = false;
	}
	cairo_identity_matrix(cr);
	if (freetext_or_lyrics_present) {
		first = true;
		for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
			((NedSystem *) lptr->data)->drawTexts(cr, m_page_number == 0 && first, scale);
			first = false;
		}
	}
	cairo_show_page(cr);
};


int NedPage::getNumberOfStaffs() {
	return ((NedSystem *) g_list_first(m_systems)->data)->getNumberOfStaffs();
}

double  NedPage::getContentXpos() {
	if (m_main_window->doDrawPostscript()) {
		return LEFT_RIGHT_BORDER;
	}
	return m_xpos + LEFT_RIGHT_BORDER;
}

bool NedPage::isLastPage() {
	return m_main_window->getLastPage() == this;
}

bool NedPage::isLastSystem(NedSystem *system) {
	GList *lptr;

	if (m_main_window->getLastPage() != this) return false;
	if ((lptr = g_list_last(m_systems)) == NULL) {
		NedResource::Abort("NedPage::isLastSystem");
	}
	return (((NedSystem *) lptr->data) == system);
}

void NedPage::setAndUpdateClefTypeAndKeySig(int *clef_and_key_array, bool first) {
	GList *lptr;
	double indent = first ? m_main_window->getFirstSystemIndent() : m_main_window->get2ndSystemIndent();

	if ((lptr = g_list_first(m_systems)) == NULL) return;
	((NedSystem *) lptr->data)->setAndUpdateClefTypeAndKeySig(clef_and_key_array, indent, first);

	indent = m_main_window->get2ndSystemIndent();

	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->setAndUpdateClefTypeAndKeySig(clef_and_key_array, indent, false);
	}
}

void NedPage::determineTempoInverse(NedChordOrRest *element, NedSystem* system, double *tempoinverse, bool *found) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if ((NedSystem *) lptr->data == system) {
			((NedSystem *) lptr->data)->determineTempoInverse(element, element->getMidiTime(), tempoinverse, found);
			break;
		}
		((NedSystem *) lptr->data)->determineTempoInverse(NULL, element->getMidiTime(), tempoinverse, found);
	}
}

void NedPage::detectVoices(int staff_nr, unsigned int *voice_mask, NedSystem **last_system,  unsigned long long *end_time) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->detectVoices(staff_nr, voice_mask, last_system, end_time);
	}
}

void NedPage::cutEmptyVoices() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->cutEmptyVoices();
	}
}

void NedPage::handleEmptyMeasures() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->handleEmptyMeasures();
	}
}

bool NedPage::hasOnlyRests() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if (!((NedSystem *) lptr->data)->hasOnlyRests()) return false;
	}
	return true;
}

void NedPage::setAllUnpositioned() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->m_is_positioned = false;
	}
}

void NedPage::zoomFreeReplaceables(double zoom, double scale) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->zoomFreeReplaceables(zoom, scale);
	}
}

void NedPage::recomputeFreeReplaceables() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->recomputeFreeReplaceables();
	}
}


void NedPage::testTies() {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->testTies();
	}
}

bool NedPage::exportLilyPond(FILE *fp, int staff_nr, int voice_nr, int *last_line, unsigned int *midi_len,
	NedSystem *last_system, unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur, unsigned int *lyrics_map,
		bool with_break, bool *guitar_chordnames, bool *chordnames, int *breath_script, bool keep_beams) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->exportLilyPond(fp, staff_nr, voice_nr, last_line, midi_len, last_system,
			end_time, in_alternative, lily_slur, lyrics_map, with_break, guitar_chordnames, chordnames, breath_script, keep_beams);
		if ((NedSystem *) lptr->data == last_system) return false;
	}
	return true;
}

bool NedPage::exportLilyGuitarChordnames(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
	NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->exportLilyGuitarChordnames(fp, staff_nr, last_line, midi_len, last_system, end_time, in_alternative, with_break);
		if ((NedSystem *) lptr->data == last_system) return false;
	}
	return true;
}

bool NedPage::exportLilyFreeChordName(FILE *fp, int staff_nr, int *last_line, unsigned int *midi_len,
	NedSystem *last_system, unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->exportLilyFreeChordName(fp, staff_nr, last_line, midi_len, last_system, end_time, in_alternative, with_break);
		if ((NedSystem *) lptr->data == last_system) return false;
	}
	return true;
}

bool NedPage::exportLilyLyrics(FILE *fp, int staff_nr, int voice_nr, int line_nr, NedSystem *last_system, unsigned long long end_time, int *sil_count) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->exportLilyLyrics(fp, ((NedSystem *) lptr->data) == last_system, staff_nr, voice_nr, line_nr, end_time, sil_count);
		if (((NedSystem *) lptr->data) == last_system) return false;
	}
	return true;
}

void NedPage::collectLyrics(NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->collectLyrics(leditor, staff_nr);
	}
}

void NedPage::setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->setLyrics(command_list, leditor, staff_nr);
	}
}

void NedPage::copyDataOfWholeStaff(int staff_nr) {
	GList *lptr;
	
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->copyDataOfWholeStaff(staff_nr);
	}
}

void NedPage::handleStaffElements() {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->handleStaffElements();
	}
}

void NedPage::setInternalPitches() {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->setInternalPitches();
	}
}

void NedPage::adjustAccidentals(int staff_nr) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->adjustAccidentals(staff_nr);
	}
}

void NedPage::changeAccidentals(NedCommandList *command_list, int preferred_offs, bool *staff_list, GList *selected_group) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->changeAccidentals(command_list, preferred_offs, staff_list, selected_group);
	}
}

void NedPage::transpose(int pitchdist, bool *staff_list, GList *selected_group) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->transpose(pitchdist, staff_list, selected_group);
	}
}

void NedPage::hideRests(NedCommandList *command_list, bool unhide, int staff_nr, int voice_nr) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->hideRests(command_list, unhide, staff_nr, voice_nr);
	}
}

bool NedPage::shiftNotes(unsigned long long start_time, int linedist, NedSystem *start_system, int staff_number) {
	GList *lptr;

	if (start_system == 0) {
		lptr = g_list_first(m_systems);
	}
	else {
		if ((lptr = g_list_find(m_systems, start_system)) == NULL) {
			NedResource::Abort("NedPage::shiftNotes");
		}
	}
	for (; lptr; lptr = g_list_next(lptr)) {
		if (!((NedSystem *) lptr->data)->shiftNotes(start_time, linedist, staff_number)) {
			return false;
		}
	}
	return true;
}

void NedPage::removeUnneededAccidentals(int staff_nr /* = -1 */) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->removeUnneededAccidentals(staff_nr);
	}
}


void NedPage::prepareReplay(bool with_keysig /* = false */) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->prepareReplay(with_keysig);
		NedResource::increaseSystemStartTime(((NedSystem *) lptr->data)->getSystemEndTime());

	}
}

void NedPage::reconfig_paper(double width, double height) {
	GList *lptr;

	m_xpos = m_page_number * (width + DEFAULT_BORDER) * m_main_window->getCurrentScale();
	m_width = width; m_height = height;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->setWidth(m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER));
	}

}

void NedPage::getLinePoints(NedStaff *start_staff, NedStaff *end_staff, GList **staves) {
	GList *lptr;
	NedStaff *staff;
	NedPage *next_page;
	int staff_nr = start_staff->getStaffNumber();
	int i;


	if (start_staff->getPage() != this) {
		if ((next_page = m_main_window->getNextPage(this)) == NULL) {
			NedResource::Abort("NedPage::getLinePoints: error 1");
		}
		if ((lptr = g_list_find(next_page->m_systems, start_staff->getSystem())) == NULL) {
			NedResource::Abort("NedPage::getLinePoints: error 2");
		}
	}
	else {
		if ((lptr = g_list_find(m_systems, start_staff->getSystem())) == NULL) {
			NedResource::Abort("NedPage::getLinePoints: error 3");
		}
	}


	for (i = 0; i < 2; i++) {
		for (; lptr; lptr = g_list_next(lptr)) {
			if ((staff = ((NedSystem *) lptr->data)->getStaff(staff_nr)) == NULL) {
				NedResource::Abort("NedPage::getLinePoints: error 4");
			}
			*staves = g_list_append(*staves, staff);
			if (staff == end_staff) return;
		}
	
	
		if ((next_page = end_staff->getPage()) == NULL) {
			NedResource::Abort("NedPage::getLinePoints: error 6");
		}
		if ((lptr = g_list_first(next_page->m_systems)) == NULL) {
			NedResource::Abort("NedPage::getLinePoints: error 7");
		}
	}

	NedResource::DbgMsg(DBG_CRITICAL, "gesucht wurde in staff %d\n", staff_nr);

	NedResource::Abort("NedPage::getLinePoints: error 8");
}


		




void NedPage::savePage(FILE *fp) {
	GList *lptr;
	fprintf(fp, "PAGE %d\n", m_page_number);
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->saveSystem(fp);
	}
}

void NedPage::restorePage(FILE *fp) {
	int i, system_nr;
	char buffer[128];
	NedSystem *system;
	double system_pos = TOP_BOTTOM_BORDER;

	i = 0;
	do {
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) return;
			NedResource::m_error_message = "(1)SYSTEM or PAGE expected";
			return;
		}
		if (strcmp(buffer, "SYSTEM")) {
			if (!strcmp(buffer, "PAGE")) {
				NedResource::unreadWord(buffer);
				return;
			}
			NedResource::DbgMsg(DBG_CRITICAL, "buffer = %s\n", buffer);
			NedResource::m_error_message = "(2)SYSTEM or PAGE expected";
			return;
		}
	
		if (!NedResource::readInt(fp, &system_nr)) {
			NedResource::m_error_message = "SYSTEM number expected";
			return;
		}
		/*
		if (system_nr != i) {
			NedResource::m_error_message = "bad system_nr";
			return;
		}
		*/
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
			NedResource::m_error_message = ": expected";
			return;
		}

		system = new NedSystem(this, system_pos, m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER), system_nr, 0 /* dummy is reset during "renumberMeasures()" at the end of "reposit()" */, FALSE);
		m_systems = g_list_append(m_systems, system);

		system->restoreSystem(fp);

		i++;
		system_pos += system->getHeight() + DEFAULT_SYSTEM_DIST;

	}
	while (NedResource::m_error_message == NULL);
}



bool NedPage::tryChangeLength(NedChordOrRest *chord_or_rest) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if (((NedSystem *) lptr->data)->tryChangeLength(chord_or_rest)) {
			return TRUE;
		}
	}
	return FALSE;
}


bool NedPage::trySelect(double x, double y, bool only_free_placeables /* = false */) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double leftx = m_main_window->getLeftX();
	double current_scale = m_main_window->getCurrentScale();

	double xl = X_POS_INVERS(x);


	if (xl < 0.0 || xl > m_width) return false;

	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if (((NedSystem *) lptr->data)->trySelect(x, y, only_free_placeables)) {
			return true;
		}
	}
	return false;
}

NedChordOrRest *NedPage::findNearestElement(int staff_nr, double x, double y, NedStaff *oldstaff, double *ydist) {
	double mindist = 10000000.0;
	NedChordOrRest *nearestElement = NULL, *element;

	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if ((element = ((NedSystem *) lptr->data)->findNearestElement(staff_nr, x, y, oldstaff, &mindist, ydist)) != NULL) {
			nearestElement = element;
		}
	}
	return nearestElement;
}

bool NedPage::findTimeOfMeasure(int meas_num, unsigned long long *meas_time, unsigned long long *system_offs) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if (((NedSystem *) lptr->data)->findTimeOfMeasure(meas_num, meas_time)) {
			return true;
		}
		*system_offs += ((NedSystem *) lptr->data)->getSystemEndTime();
	}
	return false;
}

void NedPage::collectSelectionRectangleElements(NedBbox *sel_rect, GList **sel_group, NedSystem *first_selected_system, NedSystem *last_selected_system,
	NedPage *first_selected_page, NedPage *last_selected_page) {
	double xp = sel_rect->x - m_xpos;
	if (xp + sel_rect->width < 0.0 || m_width < xp) return;
	GList *lptr;
	xp -= LEFT_RIGHT_BORDER;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->collectSelectionRectangleElements(xp, sel_rect, sel_group, first_selected_system, last_selected_system,
		first_selected_page == this, last_selected_page == this);
	}
}

void NedPage::findSelectedFirstsLasts(NedBbox *sel_rect, int *number_of_first_selected_staff, int *number_of_last_selected_staff,
		NedSystem **first_selected_system, NedSystem **last_selected_system, NedPage **first_selected_page, NedPage **last_selected_page) {
	double xp = sel_rect->x - m_xpos;
	if (xp +  sel_rect->width < 0.0 || m_width < xp) return;
	GList *lptr;
	xp -= LEFT_RIGHT_BORDER;
	if (*first_selected_page == NULL) {
		*first_selected_page = *last_selected_page = this;
	}
	else {
		*last_selected_page = this;
	}
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->findSelectedSystems(sel_rect, number_of_first_selected_staff, number_of_last_selected_staff, first_selected_system, last_selected_system);
	}
}

void NedPage::findFromTo(GList *clipboard, NedPage **min_page, NedPage **max_page, NedSystem **min_sys, NedSystem **max_sys) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if (((NedSystem *) lptr->data)->findFromTo(clipboard, min_sys, max_sys)) {
			if (*min_page == NULL) {
				*min_page = *max_page = this;
			}
			else {
				*max_page = this;
			}
		}
	}
}

void NedPage::deleteItemsFromTo(NedCommandList *command_list, bool is_first_page, bool is_last_page,
		NedSystem *min_sys, NedSystem *max_sys, unsigned long long start_midi, unsigned long long end_midi) {
	NedSystem *system;
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		system = (NedSystem *) lptr->data;
		if (is_first_page && system->getSystemNumber() < min_sys->getSystemNumber()) continue;
		if (is_last_page && system->getSystemNumber() > max_sys->getSystemNumber()) break;
		system->deleteItemsFromTo(command_list, system == min_sys, system == max_sys, start_midi, end_midi);
	}
}

void NedPage::removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first_page, bool is_last_page,
			NedSystem *min_sys, NedSystem *max_sys) {
	GList *lptr;
	NedSystem *system;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		system = (NedSystem *) lptr->data;
		if (is_first_page && system->getSystemNumber() < min_sys->getSystemNumber()) continue;
		if (is_last_page && system->getSystemNumber() > max_sys->getSystemNumber()) break;
		system->removeNotesFromTo(command_list, items, system == min_sys, system == max_sys);
	}
}

void NedPage::testForPageBackwardTies(NedCommandList *command_list) {
	((NedSystem *) g_list_first(m_systems)->data)->testForPageBackwardTies(command_list);
}

void NedPage::checkForElementsToSplit(NedCommandList *command_list, int *measure_number) {
	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->checkForElementsToSplit(command_list, measure_number);
	}
}
	


NedStaff *NedPage::findStaff(double x, double y, NedMeasure **measure) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	NedStaff *staff;

	double xl = X_POS_INVERS(x);


	if (xl < 0.0 || xl > m_width) return NULL;

	GList *lptr;
	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		if ((staff = ((NedSystem *) lptr->data)->findStaff(xl + m_xpos, y, measure)) != NULL) {
			return staff;
		}
	}
	return NULL;
}

double NedPage::convertX(double xpos) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double leftx = m_main_window->getLeftX();

	return m_main_window->getCurrentScale() * X_POS_PAGE_REL(xpos);
}

bool NedPage::isXOnPage(double x) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	double xl = X_POS_INVERS(x);
	if (xl < 0.0 || xl > m_width) return false;
	return true;
}
	

bool NedPage::tryInsertOrErease(double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */, NedChordOrRest **newObj /*  = NULL */, bool force_rest /* = false */) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	NedChordOrRest *element;
	NedChordName *chordname;
	double dummy, dummy0;
	double topy, xx, yy;
	double xx2, yy2;
	unsigned int kind, tempo;
	NedCommandList *command_list;
	NedLinePoint *lp0, *lp1, *lp2;
	if (newObj != NULL) {
		*newObj = NULL;
	}

	double xl = X_POS_INVERS(x);


#define LINE_START_X_DIST 2.0
#define SLUR_START_X_DIST 1.0
#define SLUR_START_Y_DIST ( 2 * LINE_DIST )

	if (xl < 0.0 || xl > m_width) return FALSE;

	GList *lptr;
	double mindist, d;

	if (midi_input_chord == NULL) { // x, y given
		switch(m_main_window->getSpecialType()) {
		case TYPE_DYNAMIC:
		case TYPE_TEMPO:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list = new NedCommandList(m_main_window);
			if (m_main_window->getSpecialType() == TYPE_DYNAMIC) {
				command_list->addCommand(new NedInsertFreePlaceableCommand(new NedVolumeSign(m_main_window->getSpecialSubTypeInt()), element, xx, yy));
			}
			else {
				kind = (m_main_window->getSpecialSubTypeInt() & 0xffff) * FACTOR;
				tempo = ((m_main_window->getSpecialSubTypeInt() >> 16) & 0xffff);
				command_list->addCommand(new NedInsertFreePlaceableCommand(new NedTempoSign(kind, tempo), element, xx, yy));
			}
			command_list->execute();
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_SIGN:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list = new NedCommandList(m_main_window);
			command_list->addCommand(new NedInsertFreePlaceableCommand(new NedFreeSign(m_main_window->getSpecialSubTypeInt()), element, xx, yy));
			command_list->execute();
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_GUITAR_CHORD:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
#define CHOR_NAME_Y_DIST 0.4
#define CHOR_NAME_X_DIST 0.4
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			xx2 = element->getXPos() - CHOR_NAME_X_DIST;
			yy2 = element->getSystem()->getYPos() + element->getStaff()->getTopPos() - CHOR_NAME_Y_DIST;
			command_list = new NedCommandList(m_main_window);
			command_list->addCommand(new NedInsertFreePlaceableCommand(new NedFreeChord(m_main_window->getSpecialSubTypeChordInfo().chord_ptr,
					m_main_window->getSpecialSubTypeChordInfo().chord_name_num, 
					m_main_window->getSpecialSubTypeChordInfo().status, m_main_window), element, xx2, yy2));
			command_list->execute();
			m_main_window->setAllUnpositioned();
			m_main_window->reposit(command_list);
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_SPACER:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list = new NedCommandList(m_main_window);
			command_list->addCommand(new NedInsertFreePlaceableCommand(new NedFreeSpacer(m_main_window), element, xx, yy));
			command_list->execute();
			m_main_window->setAllUnpositioned();
			m_main_window->reposit(command_list);
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_CHORDNAME:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			xx2 = element->getXPos() - CHOR_NAME_X_DIST;
			yy2 = element->getSystem()->getYPos() +  element->getStaff()->getTopPos() - m_main_window->getSpecialSubTypeChordNameInfo().ydist;
			command_list = new NedCommandList(m_main_window);
			command_list->addCommand(new NedInsertFreePlaceableCommand(chordname = new NedChordName(m_main_window->getDrawingArea(),m_main_window->getSpecialSubTypeChordNameInfo().root_name,
						m_main_window->getSpecialSubTypeChordNameInfo().up_name,m_main_window->getSpecialSubTypeChordNameInfo().down_name, m_main_window->getSpecialSubTypeChordNameInfo().fontsize), element, xx2, yy2));

			chordname->setZoom(zoom_factor, current_scale);
			command_list->execute();
			m_main_window->setAllUnpositioned();
			m_main_window->reposit(command_list);
			m_main_window->getCommandHistory()->addCommandList(command_list);
			//m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_FREE_TEXT:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			yy = Y_POS_INVERS(y);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list = new NedCommandList(m_main_window);
			command_list->addCommand(new NedInsertFreePlaceableCommand(m_main_window->m_freetext, element, xx, yy));
			m_main_window->m_freetext->setZoom(zoom_factor, current_scale);
			command_list->execute();
			m_main_window->m_freetext = NULL;
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			return TRUE;
		case TYPE_LINE:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			if (xx > m_width / 2.0) xx -= 2 * SLUR_START_X_DIST;
			yy = Y_POS_INVERS(y);
			command_list = new NedCommandList(m_main_window);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list->addCommand(new NedInsertFreePlaceableCommand(lp0 = new NedLinePoint(NULL, 0), element, xx, yy));
			dummy0 = 10000000.0;
			element = element->getStaff()->findNearestElement(xx + LINE_START_X_DIST, yy, NULL, &dummy0, &dummy);
			command_list->addCommand(new NedInsertFreePlaceableCommand(lp1 = new NedLinePoint(NULL, 1), element, xx + LINE_START_X_DIST, yy));
			command_list->execute();
			switch (m_main_window->getSpecialSubTypeInt()) {
				case LINE_CRESCENDO:
				case LINE_DECRESCENDO:
						new NedCrescendo(lp0, lp1, m_main_window->getSpecialSubTypeInt() == LINE_DECRESCENDO);
						break;
				case LINE_OCTAVATION1:
				case LINE_OCTAVATION_1:
				case LINE_OCTAVATION2:
				case LINE_OCTAVATION_2:
						new NedOctavation(lp0, lp1, m_main_window->getSpecialSubTypeInt());
						break;
						
			}
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			m_main_window->m_selected_free_replaceable = lp0;
			return TRUE;
		case TYPE_LINE3:
			topy = m_main_window->getTopY();
			xx = X_POS_INVERS_PAGE_REL(x);
			//if (xx > m_width / 2.0) xx -= 3 * SLUR_START_X_DIST;
			yy = Y_POS_INVERS(y);
			command_list = new NedCommandList(m_main_window);
			element = findNearestElement(-1, xx, yy, NULL, &dummy);
			command_list->addCommand(new NedInsertFreePlaceableCommand(lp0 = new NedLinePoint(NULL, 0), element, xx, yy));
			dummy0 = 10000000.0;
			element = element->getStaff()->findNearestElement(xx + LINE_START_X_DIST, yy, NULL, &dummy0, &dummy);
			command_list->addCommand(new NedInsertFreePlaceableCommand(lp1 = new NedLinePoint(NULL, 1), element, xx + LINE_START_X_DIST, yy));
			dummy0 = 10000000.0;
			element = element->getStaff()->findNearestElement(xx + 2 * LINE_START_X_DIST, yy, NULL, &dummy0, &dummy);
			command_list->addCommand(new NedInsertFreePlaceableCommand(lp2 = new NedLinePoint(NULL, 1), element, xx + 2 * LINE_START_X_DIST, yy));
			command_list->execute();
			new NedAcceleration(lp0, lp1, lp2, m_main_window->getSpecialSubTypeInt() == LINE_RITARDANDO);
			m_main_window->getCommandHistory()->addCommandList(command_list);
			m_main_window->resetSpecialType();
			m_main_window->m_selected_free_replaceable = lp0;
			return TRUE;
		}
	}
	lptr = g_list_first(m_systems);
	NedSystem *nearest_system = (NedSystem *) lptr->data;
	mindist = nearest_system->computeMidDist(y);
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if ((d = ((NedSystem *) lptr->data)->computeMidDist(y)) < mindist) {
			nearest_system = (NedSystem *) lptr->data;
			mindist = d;
		}
	}
	if (nearest_system->tryInsertOrErease(x, y, num_midi_input_notes, midi_input_chord, newObj, force_rest)) {
		return true;
	}
	return false;
}

void NedPage::insertSlur(GdkRectangle *selection_rect) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	double topy, xx, yy, yyy;
	double xx0, yy0, xx1, yy1, xx2, yy2;
	double xdist, ydist, dist;
	double dummy, dummy0;
	int dir, l, mid;
	NedSlurPoint *sp[3];
	GList *lptr;
	NedChordOrRest *element, *element0, *element1, *element2;
	NedCommandList *command_list = new NedCommandList(m_main_window);

	if (NedResource::m_main_clip_board == NULL) {
		//printf("Paramater vom Rect\n"); fflush(stdout);
		topy = m_main_window->getTopY();
		xx = X_POS_INVERS_PAGE_REL(selection_rect->x);
		if (xx > m_width / 2.0) xx -= (3 * SLUR_START_X_DIST) / zoom_factor;
		yy = Y_POS_INVERS(selection_rect->y);
		command_list = new NedCommandList(m_main_window);
		element = findNearestElement(-1, xx, yy, NULL, &dummy);
		yyy = element->getSystem()->getYPos() + element->getStaff()->getMidPos();
		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[0] = new NedSlurPoint(NULL, 0), element, xx, yyy));
		dummy0 = 10000000.0;
		element1 = element->getStaff()->findNearestElement(xx + SLUR_START_X_DIST, yyy - SLUR_START_Y_DIST, NULL, &dummy0, &dummy);
		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[1] = new NedSlurPoint(NULL, 1), element1, xx + SLUR_START_X_DIST, yyy - SLUR_START_Y_DIST));
		dummy0 = 10000000.0;
		element2 = element->getStaff()->findNearestElement(xx + 2 * SLUR_START_X_DIST, yyy - SLUR_START_Y_DIST / 2, NULL, &dummy0, &dummy);
		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[2] = new NedSlurPoint(NULL, 2), element2, xx + 2 * SLUR_START_X_DIST, yyy - SLUR_START_Y_DIST / 2));
	}
	else {
		//printf("Paramater von Elementen\n"); fflush(stdout);
		dir = 0;
		l = g_list_length(NedResource::m_main_clip_board);
		if (l < 2) {
			NedResource::Abort("l > 2");
		}
		mid = l / 2;

		lptr = g_list_last(NedResource::m_main_clip_board);
		dir += ((NedChordOrRest *) lptr->data)->hasUpDir() ? 1 : -1;
		element2 = (NedChordOrRest *) lptr->data;

		lptr = g_list_first(NedResource::m_main_clip_board);
		dir += ((NedChordOrRest *) lptr->data)->hasUpDir() ? 1 : -1;
		element0 = (NedChordOrRest *) lptr->data;
#define SLUR_X_DIST_PROLONG (LINE_DIST * 0.5)

		xx2 = element2->getXPos() + element2->getBBox()->width_netto + element2->getBBox()->x + SLUR_X_DIST_PROLONG;
		yy2 = element2->getStaff()->getTopPos() + element2->getSystem()->getYPos() +  element2->getStemBottom();

		lptr = g_list_nth(NedResource::m_main_clip_board, mid);
		dir += ((NedChordOrRest *) lptr->data)->hasUpDir() ? 1 : -1;
		element1 = (NedChordOrRest *) lptr->data;
/*
		xx1 = element1->getXPos();
		yy1 = element1->getStaff()->getTopPos() + element1->getSystem()->getYPos() +  element1->getStemBottom();
*/

		dir = (dir > 0.0) ? 1.0 : -1.0;
#define SLUR_MIDDLE_DIST (LINE_DIST * 1.8)
#define SLUR_Y_DIST_UP (LINE_DIST * 1.0)
#define SLUR_Y_DIST_DOWN (LINE_DIST * 2.0)
	
		xx0 = element0->getXPos() + element2->getBBox()->x - SLUR_X_DIST_PROLONG;
		yy0 = element0->getStaff()->getTopPos() + element0->getSystem()->getYPos() +  element0->getStemBottom();

		yy2 += dir * ((dir > 0) ? SLUR_Y_DIST_DOWN : SLUR_Y_DIST_UP);
		yy0 += dir * ((dir > 0) ? SLUR_Y_DIST_DOWN : SLUR_Y_DIST_UP);

		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[0] = new NedSlurPoint(NULL, 0), element2, xx2, yy2));
		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[2] = new NedSlurPoint(NULL, 0), element0, xx0, yy0));


		xdist = xx2 - xx0;
		ydist = yy2 - yy0;
		xx1 = xx0 + xdist / 2.0;
		yy1 = yy0 + ydist / 2.0;
		dist = sqrt(xdist * xdist + ydist * ydist);
		xdist /= dist;
		ydist /= dist;
		xx1 -= ydist * dir * SLUR_MIDDLE_DIST;
		yy1 += xdist * dir * SLUR_MIDDLE_DIST;
		command_list->addCommand(new NedInsertFreePlaceableCommand(sp[1] = new NedSlurPoint(NULL, 0), element1, xx1, yy1));
		
	}
	
	command_list->execute();
	new NedSlur(sp);
	m_main_window->getCommandHistory()->addCommandList(command_list);
	m_main_window->resetSpecialType();
	m_main_window->m_selected_free_replaceable = sp[2];
}

bool NedPage::findLine(double x, double y, double *ypos, int *line, double *bottom, NedStaff **staff) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double leftx = m_main_window->getLeftX();
	double current_scale = m_main_window->getCurrentScale();

	double xl = X_POS_INVERS(x);

	if (xl < 0.0 || xl > m_width) return FALSE;
	GList *lptr;
	double mindist, d;
	lptr = g_list_first(m_systems);
	if (lptr == NULL) {
		return FALSE; // don't know in which case this happens; concerns mainly MIDI import
	}
	NedSystem *nearest_system = (NedSystem *) lptr->data;
	mindist = nearest_system->computeMidDist(y);
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if ((d = ((NedSystem *) lptr->data)->computeMidDist(y)) < mindist) {
			nearest_system = (NedSystem *) lptr->data;
			mindist = d;
		}
	}
	if (nearest_system->findLine(x, y, ypos, line, bottom, staff)) {
		return TRUE;
	}
	return FALSE;
}

bool NedPage::findXposInOtherMeasure(guint keyval, double x, double y, double *newx, double *newy) {
	double zoom_factor = m_main_window->getCurrentZoomFactor();
	double current_scale = m_main_window->getCurrentScale();
	double leftx = m_main_window->getLeftX();
	NedChordOrRest *element;
	NedStaff *staff;
	NedSystem *system;

	double xl = X_POS_INVERS(x);
	*newy = y;

	if (xl < 0.0 || xl > m_width) return false;
	GList *lptr;
	double mindist, d;
	lptr = g_list_first(m_systems);
	NedSystem *nearest_system = (NedSystem *) lptr->data;
	mindist = nearest_system->computeMidDist(y);
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if ((d = ((NedSystem *) lptr->data)->computeMidDist(y)) < mindist) {
			nearest_system = (NedSystem *) lptr->data;
			mindist = d;
		}
	}
	if (nearest_system->findElement(keyval, x, y, &element, &staff)) {
		m_main_window->setVisible(element);
		leftx = m_main_window->getLeftX();
		*newx = X_POS_PAGE_REL(element->getXPos());
		return true;
	}
	else {
		if ((lptr = g_list_find(m_systems, nearest_system)) == NULL) {
			NedResource::Abort("NedPage::findXposInOtherMeasure");
		}
		switch (keyval) {
			case GDK_Right: lptr = g_list_next(lptr);
					if (lptr == NULL) return false;
					system = (NedSystem *) lptr->data;
					lptr = system->getFirstChordOrRest(staff->getStaffNumber(), 0, 0, false, false);
					if (lptr == NULL) return false;
					element = (NedChordOrRest *) lptr->data;
					m_main_window->setVisible(element);
					*newx = X_POS_PAGE_REL(element->getXPos());
					*newy = element->getStaff()->getRealYPosOfLine(-3);
					return true;
			case GDK_Left:  lptr = g_list_previous(lptr);
					if (lptr == NULL) return false;
					system = (NedSystem *) lptr->data;
					lptr = system->getLastChordOrRest(staff->getStaffNumber(), 0, 0, false, false);
					if (lptr == NULL) return false;
					element = (NedChordOrRest *) lptr->data;
					m_main_window->setVisible(element);
					*newx = X_POS_PAGE_REL(element->getXPos());
					*newy = element->getStaff()->getRealYPosOfLine(-3);
					return true;
		}
	}
	return false;
}

void NedPage::renumberSystems() {
	int i;
	GList *lptr;

	for (i = 0, lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr), i++) {
		((NedSystem *) lptr->data)->setSystemNumber(i);
	}
}

void NedPage::resetActiveFlags() {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->resetActiveFlags();
	}
}

NedPage *NedPage::getNextPage() {
	return m_main_window->getNextPage(this);
}

NedPage *NedPage::getPreviousPage() {
	return m_main_window->getPreviousPage(this);
}



bool NedPage::placeStaffs(int pass, NedCommandList *command_list /* = NULL */) {
	GList *lptr;
	NedPage *other_page;
	double offs = STAFF_TOP_DIST;
	do_place_staffs(offs);
	bool changed = FALSE;

	if (m_page_number == 0) {
		offs += getMainWindow()->getFirstPageYOffs() / getMainWindow()->getCurrentScale();
	}

	if (pass == 0) m_ping_pong = false;
	do_place_staffs(offs);
	if (pass == 0 && m_system_diff < 0) {
		if (command_list != NULL && g_list_length(m_systems) > 1) {
			lptr = g_list_last(m_systems);
			if (m_main_window->getLastPage() && hasOnlyRests())  {
				NedDeleteSystemCommand *del_system_command = new NedDeleteSystemCommand((NedSystem *) lptr->data);
				del_system_command->execute();
				command_list->addCommand(del_system_command);
			}
			else {
				other_page = m_main_window->getNextPage(this, command_list);
				NedMoveSystemCommand *move_system_command = new NedMoveSystemCommand(this, (NedSystem *) lptr->data, other_page);
				move_system_command->execute(); // the commandlist is not executed
				command_list->addCommand(move_system_command);
			}
			changed = TRUE;
			do_place_staffs(offs);
		}
	}
#define MAX_Y_BORDER 1.0
	if (pass == 1 && !isEmpty() && !m_ping_pong && command_list != NULL && m_system_diff > MAX_Y_BORDER) {
		other_page = m_main_window->getNextPage(this);
		if (other_page == NULL) return FALSE;
		if ((lptr = g_list_first(other_page->m_systems)) == NULL) {
			NedRemovePageCommand *rem_page_command = new NedRemovePageCommand(m_main_window, other_page);
			rem_page_command->execute();
			command_list->addCommand(rem_page_command);
			return FALSE;
		}
		NedGetSystemFromNextPageCommand *get_system_from_next_page_command =
			new NedGetSystemFromNextPageCommand(lptr, this, other_page);
		get_system_from_next_page_command->execute(); // the commandlist is not executed; performs placeStaffs()
		if (m_system_diff < MIN_SYSTEM_Y_DIST) {
			m_ping_pong = TRUE;
			get_system_from_next_page_command->unexecute();
			delete get_system_from_next_page_command;
		}
		else {
			command_list->addCommand(get_system_from_next_page_command);
			changed = TRUE;
		}
		if (!isEmpty()) {
			do_place_staffs(offs);
		}
	}
	return changed;
}

void  NedPage::computeTuplets() {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->computeTuplets();
	}
}

int NedPage::getSystemCount() {
	return g_list_length(m_systems);
}

void NedPage::do_place_staffs(double offs) {
	GList *lptr;
	double staffpos = TOP_BOTTOM_BORDER + offs;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		staffpos = ((NedSystem *) lptr->data)->placeStaffs(staffpos);
	}
	m_system_diff = m_height - staffpos;
}

NedSystem *NedPage::getNextSystem(NedSystem *system, NedCommandList *command_list /* = NULL */) {
	GList *lptr;
	NedAppendSystemCommand *app_sys_cmd;
	NedSystem *system_on_next_page;
	bool append_system_if_necessary = ((m_main_window->getLastPage() == this) && (system == ((NedSystem *) g_list_last(m_systems)->data)));


	if (isEmpty()) return NULL;

	if ((lptr = g_list_find(m_systems, system)) == NULL) {
		printf("gesucht war: 0x%p\n", system); fflush(stdout);
		NedResource::Abort("NedPage::getNextSystem");
	}

	if ((lptr = g_list_next(lptr)) == NULL) {
		system_on_next_page = m_main_window->getNextSystem(this, NULL);
		if (system_on_next_page == NULL) {
			if (command_list != NULL && append_system_if_necessary) {
				app_sys_cmd = new NedAppendSystemCommand(this);
				app_sys_cmd->execute();
				command_list->addCommand(app_sys_cmd);
				return app_sys_cmd->getSystem();
			}
			return NULL;
		}
		return system_on_next_page;
	}

	return (NedSystem *) lptr->data;
}

NedSystem *NedPage::getPreviousSystem(NedSystem *system) {
	GList *lptr;
	if ((lptr = g_list_find(m_systems, system)) == NULL) {
		NedResource::Abort("NedPage::getPreviousSystem");
	}
	if ((lptr = g_list_previous(lptr)) == NULL) {
		return NULL;
	}
	return (NedSystem *) lptr->data;
}

NedSystem *NedPage::getFirstSystem() {
	GList *lptr;

	if ((lptr = g_list_first(m_systems)) == NULL) {
		NedResource::Abort("NedPage::getFirstSystem");
	}

	return (NedSystem *) lptr->data;
}


void NedPage::appendStaff(NedCommandList *command_list, int p_staff_nr /* = -1 */) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->appendStaff(command_list, p_staff_nr);
	}
}

void NedPage::removeLastStaff() {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->removeLastStaff();
	}
}

void NedPage::deleteStaff(int staff_number) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->deleteStaff(staff_number);
	}
}

void NedPage::restoreStaff(int staff_number) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->restoreStaff(staff_number);
	}
}

void NedPage::shiftStaff(int staff_number, int position) {
	GList *lptr;

	for (lptr = g_list_first(m_systems); lptr; lptr = g_list_next(lptr)) {
		((NedSystem *) lptr->data)->shiftStaff(staff_number, position);
	}
}
