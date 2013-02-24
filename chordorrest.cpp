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
#include <string.h>
#include "chordorrest.h"
#include "voice.h"
#include "note.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "tuplet.h"
#include "system.h"
#include "mainwindow.h"
#include "commandlist.h"
#include "commandhistory.h"
#include "ereasechordorrestcommand.h"
#include "changechordorreststatus.h"
#include "ereasenotecommand.h"
#include "deletechordcommand.h"
#include "flipvaluescommand.h"
#include "removeconstraintscommand.h"
#include "removefreeplaceablecommand.h"
#include "changelyricscommand.h"
#include "newlyricscommand.h"
#include "volumesign.h"
#include "temposign.h"
#include "deletelyricscommand.h"
#include "tienotescommand.h"
#include "slurpoint.h"
#include "slur.h"
#include "linepoint.h"
#include "line.h"
#include "crescendo.h"
#include "octavation.h"
#include "acceleration.h"
#include "freetext.h"
#include "freesign.h"
#include "freechord.h"
#include "chordpainter.h"
#include "chordstruct.h"
#include "freespacer.h"
#include "freechordname.h"
#include "pangocairotext.h"
#include "scoreinfodialog.h"
#include "lyricseditor.h"


#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getStaff()->getBottomPos()) * zoom_factor - topy)
#define X_POS_INVERS_PAGE_REL(p) ((leftx + (p) / current_scale) / zoom_factor - getPage()->getContentXpos())
#define Y_POS_INVERS_ABS(p) ((topy + (p) / current_scale) / zoom_factor)
#define Y_POS_INVERSE_STAFF_REL(p) (((p) / current_scale + topy) / zoom_factor - getSystem()->getYPos() - getStaff()->getBottomPos())



NedChordOrRest::NedChordOrRest(NedVoice *voice, int type, bool is_hidden, int line, int dot_count, unsigned int length, int head, unsigned int status, unsigned long long time) :
m_measure(NULL), m_notes(NULL), m_beam(NULL), m_tuplet_ptr(NULL), m_tuplet_val(0), m_type(type), m_is_hidden(is_hidden), m_length(length), m_xpos(0.0), m_ypos(0.0), m_time(time), m_dot_count(dot_count), m_status(status), m_line(line), m_voice(voice), m_midi_time(time), 
m_tmp_xpos(-1.0), m_all_shifted(FALSE), m_up_art_count(0), m_down_art_count(0),
m_freedisp(NULL), m_active(false), m_stem_end_xpos(0.0), m_stem_end_ypos(0.0),
m_stem_height(STEM_DEFAULT_HEIGHT), m_stemheight_copy(UNSET_STEM_HEIGHT - 10.0), m_beam_y_offs(0.0), m_slope_offs(0),
m_beam_y_offs_copy(UNSET_STEM_HEIGHT - 10.0), m_micro_shift(0.0), m_slope_offs_copy(UNSET_SLOPE_OFFS),
m_has_open_highhat(false), m_has_closed_highhat(false)
{
	m_bbox.width = -0.1;
	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		m_lyrics[i] = NULL;
	}
	if (m_length < NOTE_64) {
		m_type = TYPE_GRACE;
		m_dot_count = 0;
		m_status = 0;
	}
	if (m_type == TYPE_REST) {
		m_status &= (STAT_FERMATA | STAT_USER_HIDDEN_REST | STAT_USER_PLACED_REST);
		m_line = 3;
		m_ypos = -m_line * LINE_DIST/2.0;
		if (m_length == WHOLE_NOTE) {
			if (m_voice != NULL && m_voice->getVoiceNumber() > 0) {
				m_is_hidden = TRUE;
			}
		}
	}
	else {
		if (head == GUITAR_NOTE_STEM || head == GUITAR_NOTE_NO_STEM) {
			m_line = 5;
		}
		NedNote *note = new NedNote(this, m_line, head, m_status);
		m_notes = g_list_append(m_notes, note);
//#define OOO
#ifdef OOO
#define TTT
#ifdef TTT
		static int zz;
		zz++;
		if (m_type == TYPE_CHORD && (zz % 3) == 0) {
			//createLyrics(0, "Хěвел-");
			createLyrics(0, "aaa");
		}
		else if (m_type == TYPE_CHORD && (zz % 3) == 1) {
			//createLyrics(0, "ухăç");
			createLyrics(0, "aaa");
		}
		else if (m_type == TYPE_CHORD) {
			//createLyrics(0, "округě");
			//createLyrics(0, "Ärger");
			createLyrics(0, "aaa");
		}
#else
		static int zz;
		zz++;
		if ((zz % 4) == 0) {
			createLyrics(0, "תדמיתה");
		}
		else if ((zz % 4) == 1) {
			createLyrics(0, "פעילות");
		}
		else if ((zz % 4) == 2) {
			createLyrics(0, "Хěвел-");
		}
		else {
			//createLyrics(0, "округě");
			createLyrics(0, "Ärger");
		}
#endif
#endif
		if (m_voice != NULL) {
			reConfigure();
		}
		xPositNotes();
		m_ypos = 0.0;
	}
	if (m_voice != NULL) {
		computeBbox();
	}

//#define VVV
#ifdef VVV
static int zz = 2;
static NedSlurPoint *slur_points[3];
	if (zz >= 0 && m_type == TYPE_CHORD) {
		NedSlur *slur;
		//m_freedisp = g_list_append(m_freedisp, new NedVolumeSign(zz + 1, this, 0.8, 0.0, true));
		slur_points[2-zz] = new NedSlurPoint(NULL, 2 - zz, this, 0.8, 0.0, true);
		m_freedisp = g_list_append(m_freedisp, slur_points[2-zz]);
		if (zz == 0) {
			slur = new NedSlur(slur_points);
			slur_points[0]->setSlur(slur);
			slur_points[1]->setSlur(slur);
			slur_points[2]->setSlur(slur);
		}
		zz--;

	}
#endif
}

NedChordOrRest::NedChordOrRest(NedVoice *voice, int type, unsigned int clef_number, int octave_shift, unsigned long long time, bool dummy) : // clef
m_measure(NULL), m_notes(NULL), m_beam(NULL), m_tuplet_ptr(NULL), m_tuplet_val(0), m_type(type), m_is_hidden(false), m_length(clef_number), m_xpos(0.0), m_ypos(0.0), m_time(time), m_dot_count(octave_shift), m_status(0), m_line(3), m_voice(voice), m_midi_time(time),
m_tmp_xpos(-1.0), m_all_shifted(FALSE), m_freedisp(NULL), m_stem_end_xpos(0.0), m_stem_end_ypos(0.0),
m_stem_height(STEM_DEFAULT_HEIGHT), m_stemheight_copy(UNSET_STEM_HEIGHT - 10.0), m_beam_y_offs(0.0), m_slope_offs(0),
m_beam_y_offs_copy(UNSET_STEM_HEIGHT - 10.0), m_micro_shift(0.0), m_slope_offs_copy(UNSET_SLOPE_OFFS),
m_has_open_highhat(false), m_has_closed_highhat(false)
{
	m_bbox.width = -0.1;

	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		m_lyrics[i] = NULL;
	}
	if (m_voice != NULL) {
		computeBbox();
	}
}

NedChordOrRest::NedChordOrRest(NedVoice *voice, int type, int keysig, int last_keysig, unsigned long long time) : // keysig
m_measure(NULL), m_notes(NULL), m_beam(NULL), m_tuplet_ptr(NULL), m_tuplet_val(0), m_type(type), m_is_hidden(false), m_length(keysig), m_xpos(0.0), m_ypos(0.0), m_time(time), m_dot_count(last_keysig), m_status(0), m_line(3), m_voice(voice), m_midi_time(time),
m_tmp_xpos(-1.0), m_all_shifted(FALSE), m_freedisp(NULL), m_stem_end_xpos(0.0), m_stem_end_ypos(0.0),
m_stem_height(STEM_DEFAULT_HEIGHT), m_stemheight_copy(UNSET_STEM_HEIGHT - 10.0), m_beam_y_offs(0.0), m_slope_offs(0),
m_beam_y_offs_copy(UNSET_STEM_HEIGHT - 10.0), m_micro_shift(0.0), m_slope_offs_copy(UNSET_SLOPE_OFFS),
m_has_open_highhat(false), m_has_closed_highhat(false)
{
	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		m_lyrics[i] = NULL;
	}
	if (m_voice != NULL) {
		computeBbox();
	}
}

NedChordOrRest::NedChordOrRest(NedVoice *voice, int type, int sub_type, unsigned long long time) : //staff sign
m_measure(NULL), m_notes(NULL), m_beam(NULL), m_tuplet_ptr(NULL), m_tuplet_val(0), m_type(type), m_is_hidden(false), m_length(0), m_ypos(0.0), m_time(time), m_dot_count(sub_type), m_status(0), m_line(3), m_voice(voice), m_midi_time(time),
m_tmp_xpos(-1.0), m_all_shifted(FALSE), m_freedisp(NULL), m_stem_end_xpos(0.0), m_stem_end_ypos(0.0),
m_stem_height(STEM_DEFAULT_HEIGHT), m_stemheight_copy(UNSET_STEM_HEIGHT - 10.0), m_beam_y_offs(0.0), m_slope_offs(0),
m_beam_y_offs_copy(UNSET_STEM_HEIGHT - 10.0), m_micro_shift(0.0), m_slope_offs_copy(UNSET_SLOPE_OFFS)
{
	m_bbox.width = -0.1;
	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		m_lyrics[i] = NULL;
	}
	if (m_voice != NULL) {
		computeBbox();
	}
}


NedChordOrRest::~NedChordOrRest() {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		delete ((NedNote *) lptr->data);
	}

	g_list_free(m_notes);
	m_notes = NULL;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		delete (NedFreeReplaceable *) lptr->data;
	}

	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			delete m_lyrics[i];
		}
	}

	g_list_free(m_freedisp);
	m_freedisp = NULL;
}

NedChordOrRest *NedChordOrRest::clone(struct addr_ref_str **addrlist, bool keep_beam /* = false */) {
	GList *lptr;
	NedChordOrRest *cl = new NedChordOrRest(*this);
	NedNote *note;
	int i;
	struct addr_ref_str *adr_ptr;

	cl->m_notes = NULL;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		note = new NedNote(*((NedNote *) lptr->data));
		note->setChord(cl);
		if ((adr_ptr = (struct addr_ref_str *) g_try_malloc(sizeof(struct addr_ref_str))) == NULL) {
			NedResource::Abort("NedChordOrRest::clone");
		}
		adr_ptr->ori = lptr->data;
		adr_ptr->copy = (void *) note;
		adr_ptr->next = *addrlist;
		*addrlist = adr_ptr;
		cl->m_notes = g_list_append(cl->m_notes, note);
		if (!keep_beam) {
			cl->m_beam = NULL;
		}
	}
	for (i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			cl->m_lyrics[i] = m_lyrics[i]->clone();
		}
	}
	cl->m_freedisp = NULL;
	/*
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEXT) {
			((NedFreeReplaceable *) lptr->data)->clone(slurlist, cl, getMainWindow()->getDrawingArea(), getMainWindow());
			continue;
		}
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_SLUR_POINT | FREE_PLACEABLE_LINE_POINT)) == 0) continue;
		((NedFreeReplaceable *) lptr->data)->clone(slurlist, cl);
	}
	*/

	return cl;
}

int NedChordOrRest::getPosition() {
	return m_voice->getPosition(this);
}

NedChordOrRest *NedChordOrRest::clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedVoice *p_voice) {
	GList *lptr;
	NedChordOrRest *cl = new NedChordOrRest(*this);
	NedNote *note;
	struct addr_ref_str *adr_ptr;
	int i;

	cl->m_notes = NULL;
	cl->m_voice = p_voice;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		note = new NedNote(*((NedNote *) lptr->data));
		note->setChord(cl);
		if ((adr_ptr = (struct addr_ref_str *) g_try_malloc(sizeof(struct addr_ref_str))) == NULL) {
			NedResource::Abort("NedChordOrRest::clone");
		}
		adr_ptr->ori = lptr->data;
		adr_ptr->copy = (void *) note;
		adr_ptr->next = *addrlist;
		*addrlist = adr_ptr;
		cl->m_notes = g_list_append(cl->m_notes, note);
		cl->m_beam = NULL;
	}
	for (i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			cl->m_lyrics[i] = m_lyrics[i]->clone();
		}
	}
	cl->m_freedisp = NULL;
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEXT) {
			((NedFreeReplaceable *) lptr->data)->clone(slurlist, cl, getMainWindow()->getDrawingArea(), getMainWindow());
			continue;
		}
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_SLUR_POINT | FREE_PLACEABLE_LINE_POINT)) == 0) continue;
		((NedFreeReplaceable *) lptr->data)->clone(slurlist, cl);
	}

	return cl;
}

char * NedChordOrRest::getLyrics(int line) {
	if (m_lyrics[line] == NULL) return NULL;
	return m_lyrics[line]->getText();
}

void NedChordOrRest::adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist) {
	GList *lptr;
	GList *newlist = NULL;
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (lptr->data == NULL) continue;
		newlist = g_list_append(newlist, lptr->data);
	}
	g_list_free(m_freedisp);
	m_freedisp = newlist;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_SLUR_POINT)) == 0) continue;
		((NedFreeReplaceable *) lptr->data)->adjust_pointers(slurlist);
	}

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->adjust_pointers(addrlist);
	}
}

void NedChordOrRest::setLyrics(int line, char *text) {
	if (text == 0 || strlen(text) < 1) return;
	if (g_utf8_strlen(text, -1) > MAX_TEXT_LENGTH) return;
	m_lyrics[line] = new NedPangoCairoText(getMainWindow()->getDrawingArea()->window, text,
		LYRICS_FONT, LYRICS_FONT_SLANT, LYRICS_FONT_WEIGHT, LYRICS_FONT_SIZE,
			getMainWindow()->getCurrentZoomFactor(), getMainWindow()->getCurrentScale(), true);
}

void NedChordOrRest::setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor) {
	GList *lptr;
	char *text;
	bool has_tie;
	int line;
	NedPangoCairoText *new_pango_text;
	NedChangeLyricsCommand *change_lyrics_cmd;

	if (m_type != TYPE_CHORD) return;
	has_tie = false;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getTieBackward() != NULL) {
			has_tie = true;
		}
	}
	if (has_tie) {
		return;
	}
	for (line = 0; line < MAX_LYRICS_LINES; line++) {
		new_pango_text = NULL;
		if (leditor->m_newchar[line]) {
			text = leditor->getNextWord(line);
			if (text != NULL) {
				if (*text != '.' || *(text + 1) != '\0')  {
					if (g_utf8_strlen(text, -1) <= MAX_TEXT_LENGTH) {
						if (m_lyrics[line] != NULL && !strcmp(m_lyrics[line]->getText(), text)) continue;
						new_pango_text = new NedPangoCairoText(getMainWindow()->getDrawingArea()->window, text,
										LYRICS_FONT, LYRICS_FONT_SLANT, LYRICS_FONT_WEIGHT, LYRICS_FONT_SIZE,
										getMainWindow()->getCurrentZoomFactor(), getMainWindow()->getCurrentScale(), true);
					}
				}
			}
		}
		if (m_lyrics[line] == NULL && new_pango_text == NULL) continue;
		change_lyrics_cmd = new NedChangeLyricsCommand(this, line, new_pango_text);
		change_lyrics_cmd->execute();
		command_list->addCommand(change_lyrics_cmd);

	}
}

void NedChordOrRest::setLyricsFromOldContext(NedMainWindow *main_window, int line, char *text) {
	if (text == 0 || strlen(text) < 1) return;
	if (g_utf8_strlen(text, -1) > MAX_TEXT_LENGTH) return;
	m_lyrics[line] = new NedPangoCairoText(main_window->getDrawingArea()->window, text,
		LYRICS_FONT, LYRICS_FONT_SLANT, LYRICS_FONT_WEIGHT, LYRICS_FONT_SIZE,
			main_window->getCurrentZoomFactor(), main_window->getCurrentScale(), true);
}


void NedChordOrRest::createLyrics(int line, char *text) {
	if (line < 0 || line >= MAX_LYRICS_LINES) {
		NedResource::Abort("NedChordOrRest::createLyrics(1)");
	}
	if (m_type != TYPE_CHORD) return;
	if (m_voice->getVoiceNumber() > 0) return;
	if (text == 0 || strlen(text) < 1) return;
	m_lyrics[line] = new NedPangoCairoText(getMainWindow()->getDrawingArea()->window, text,
		LYRICS_FONT, LYRICS_FONT_SLANT, LYRICS_FONT_WEIGHT, LYRICS_FONT_SIZE,
			getMainWindow()->getCurrentZoomFactor(), getMainWindow()->getCurrentScale(), true);
}

void NedChordOrRest::appendToLyrics(NedCommandList *command_list, int line, const char *text) {
	if (line < 0 || line >= MAX_LYRICS_LINES) {
		NedResource::Abort("NedChordOrRest::appendToLyrics(1)");
	}
//#define RTL_TEST2
#ifdef RTL_TEST2
	printf("appendToLyrics(a): string = %s : ", text);
	for (unsigned int i = 0; i < strlen(text); i++) {
		printf("s[%u] = 0x%x, ", i, 0xff & text[i]);
	}
	putchar('\n'); fflush(stdout);
#endif
	
	if (m_voice->getVoiceNumber() > 0) return;
#ifdef RTL_TEST2
	printf("appendToLyrics(b): string = %s : ", text);
	for (unsigned int i = 0; i < strlen(text); i++) {
		printf("s[%u] = 0x%x, ", i, 0xff & text[i]);
	}
	putchar('\n'); fflush(stdout);
#endif
	if (m_type != TYPE_CHORD) return;
#ifdef RTL_TEST2
	printf("appendToLyrics(c): string = %s : ", text);
	for (unsigned int i = 0; i < strlen(text); i++) {
		printf("s[%u] = 0x%x, ", i, 0xff & text[i]);
	}
	putchar('\n'); fflush(stdout);
#endif
	if (text == NULL || strlen(text) < 1) return;
#ifdef RTL_TEST2
	printf("appendToLyrics(d): string = %s : ", text);
	for (unsigned int i = 0; i < strlen(text); i++) {
		printf("s[%u] = 0x%x, ", i, 0xff & text[i]);
	}
	putchar('\n'); fflush(stdout);
#endif
//#define RTL_TEST
#ifdef RTL_TEST
	static int zz = 0;
	if (text[0] != '-') {
		switch (zz++ % 6) {
			case 0: text = "פ"; break;
			case 1: text = "ע"; break;
			case 2: text = "י"; break;
			case 3: text = "ל"; break;
			case 4: text = "ו"; break;
			case 5: text = "ת"; break;
		}
	}
#endif
	if (m_lyrics[line] == NULL) {
		if (text == NULL || strlen(text) < 1) return;
		m_lyrics[line] = new NedPangoCairoText(getMainWindow()->getDrawingArea()->window,  text,
			LYRICS_FONT, LYRICS_FONT_SLANT, LYRICS_FONT_WEIGHT, LYRICS_FONT_SIZE,
			getMainWindow()->getCurrentZoomFactor(), getMainWindow()->getCurrentScale(), true);
		command_list->addCommand(new NedNewLyricsCommand(this, line));
	}
	else {
		m_lyrics[line]->insertAtCursorPosition( text, getMainWindow()->getCurrentZoomFactor());
	}
	computeBbox();
}

bool NedChordOrRest::deleteLyricsLetter(NedCommandList *command_list, int line) {
	if (line < 0 || line >= MAX_LYRICS_LINES) {
		NedResource::Abort("NedChordOrRest::deleteLyricsLetter(1)");
	}
	if (m_type != TYPE_CHORD || m_lyrics[line] == NULL) return TRUE;
	if (!m_lyrics[line]->removeAtCursorPosition(getMainWindow()->getCurrentZoomFactor())) {
		delete m_lyrics[line];
		m_lyrics[line] = NULL;
		return false;
	}
	computeBbox();
	return true;
}

bool NedChordOrRest::setLyricsCursor(int line, int pos) {
	if (line < 0 || line >= MAX_LYRICS_LINES) {
		NedResource::Abort("NedChordOrRest::setLyricsCursor");
	}
	switch(pos) {
		case LYR_CURSOR_RIGHT:
			if (m_lyrics[line] == NULL) return false;
			return m_lyrics[line]->moveCursor(1);
			break;
		case LYR_CURSOR_LEFT: 
			if (m_lyrics[line] == NULL) return false;
			return m_lyrics[line]->moveCursor(-1);
			break;
		case LYR_CURSOR_END:
			if (m_lyrics[line] == NULL) return false;
			m_lyrics[line]->setCursorToLastPos();
			break;
		case LYR_CURSOR_START: 
			if (m_lyrics[line] == NULL) return false;
			m_lyrics[line]->setCursorToFirstPos();
			break;
	}
	return true;
}

void NedChordOrRest::setXPos2(double pos) {
	m_xpos = pos;
	computeBbox();
}




void NedChordOrRest::selectNextChord(int line, bool lyrics_required) {
	return m_voice->selectNextChord(this, line, lyrics_required);
}

void NedChordOrRest::selectPreviousChord(int line, bool lyrics_required) {
	return m_voice->selectPreviousChord(this, line, lyrics_required);
}


NedMainWindow *NedChordOrRest::getMainWindow() {return m_voice->getStaff()->getSystem()->getPage()->getMainWindow();}
NedPage *NedChordOrRest::getPage() {return m_voice->getStaff()->getSystem()->getPage();}
NedSystem *NedChordOrRest::getSystem() {return m_voice->getStaff()->getSystem();}
NedStaff *NedChordOrRest::getStaff() {return m_voice->getStaff();}

void NedChordOrRest::setToRest(bool toRest) {
	if (m_type != TYPE_REST && toRest) {
		if (m_beam != NULL) {
			delete m_beam; // includes unbeaming other notes and unregister on voice
		}
	}
	if (toRest) {
		m_type = TYPE_REST;
	}
	else {
		m_type = TYPE_CHORD;
	}
	
	if (m_type == TYPE_REST) {
		m_ypos = - 3 * LINE_DIST/2.0;
	}
	else {
		m_ypos = 0.0;
		reConfigure();
		xPositNotes();
	}
	computeBbox();
}

void NedChordOrRest::reposit(unsigned int pos) {
	m_xpos = 1.0 + 1.0 * pos;
}

int NedChordOrRest::getPitchOfFirstNote() {
	int clef, keysig, octave_shift;
	bool dummy;
	getStaff()->getCurrentClefAndKeysig(m_midi_time, &clef, &keysig, &octave_shift);
	return ((NedNote *) g_list_first(m_notes)->data)->getPitch(clef, keysig, octave_shift, 0, &dummy, true);
}

void NedChordOrRest::setInternalPitch(int pitchdist) {
	int clef, keysig, octave_shift;
	GList *lptr;

	getStaff()->getCurrentClefAndKeysig(m_midi_time, &clef, &keysig, &octave_shift);
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->setInternalPitch(pitchdist, clef, keysig, octave_shift);
	}
}

void NedChordOrRest::hideRest(NedCommandList *command_list, bool unhide) {
	if (m_type != TYPE_REST) return;
	if (m_is_hidden) return;
	NedChangeChordOrRestStatusCommand *chordstatus;
	unsigned int newstatus = m_status;

	if (unhide) {
		newstatus = m_status & (~(STAT_USER_HIDDEN_REST));
	}
	else {
		newstatus = m_status | STAT_USER_HIDDEN_REST;
	}
	chordstatus = new NedChangeChordOrRestStatusCommand(this, newstatus);
	command_list->addCommand(chordstatus);
	chordstatus->execute();
}


void NedChordOrRest::shiftNotes(int linedist) {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->shiftNoteLine(linedist);
	}
	reConfigure();
	xPositNotes();
	computeBbox();
}

void NedChordOrRest::adjustAccidentals() {
	int clef, keysig, octave_shift;
	GList *lptr;

	getStaff()->getCurrentClefAndKeysig(m_midi_time, &clef, &keysig, &octave_shift);
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->adjustAccidental(clef, keysig, octave_shift);
	}
	reConfigure();
	xPositNotes();
	computeBbox();
}

void NedChordOrRest::changeAccidentals(NedCommandList *command_list, int preferred_offs) {
	int clef, keysig, octave_shift;
	GList *lptr;

	getStaff()->getCurrentClefAndKeysig(m_midi_time, &clef, &keysig, &octave_shift);
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->changeAccidental(command_list, preferred_offs, clef, keysig, octave_shift);
	}
	reConfigure();
	xPositNotes();
	computeBbox();
}

void NedChordOrRest::insertNoteAt(NedNote *note, bool echo /* = FALSE */) {
	staff_context_str *context;
	bool dummy;
	int clef, keysig, octave_shift;
	int pitch;
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->isTheSame(note)) return;
	}

	note->setChord(this);
	if (m_voice != NULL) {
		context = &(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()]);
		getStaff()->getCurrentClefAndKeysig(m_midi_time, &clef, &keysig, &octave_shift);

		if (echo && NedResource::m_midi_echo) {
			pitch = note->getPitch(clef, keysig, octave_shift, 0, &dummy, true);
			NedResource::playImmediately(context->m_midi_channel, context->voices[0].m_midi_program, 1, &pitch, context->voices[0].m_midi_volume);
		}
		xPositNotes();
	}
	m_notes = g_list_insert_sorted(m_notes, note, (gint (*)(const void*, const void*)) NedNote::compare);
}



bool NedChordOrRest::removeNoteAt(int line) {
	GList *lptr;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getLine() == line) {
			m_notes = g_list_delete_link(m_notes, lptr);
			xPositNotes();
			return g_list_length(m_notes) > 0;
		}
	}
	NedResource::Abort("NedChordOrRest::removeNoteAt");
	return FALSE;
}

NedChordOrRest *NedChordOrRest::cloneWithDifferentLength(unsigned int newlen, int dotcount) {
	int line, head;
	GList *lptr;

	if (m_type == TYPE_REST) {
		line = 3;
	}
	else {
		NedNote *note = (NedNote *) g_list_first(m_notes)->data;
		line = note->getLine();
	}
	if (m_notes != NULL) {
		head = ((NedNote *) g_list_first(m_notes)->data)->getNoteHead();
	}
	else {
		head = NORMAL_NOTE;
	}
	NedChordOrRest *newChord = new NedChordOrRest(m_voice, m_type, FALSE /* m_is_hidden */ , line, dotcount, newlen, head, m_status, 0);
	if (m_type != TYPE_REST) {
		lptr = g_list_first(m_notes);
		for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
			newChord->m_notes = g_list_append(newChord->m_notes, new NedNote(newChord, ((NedNote *) lptr->data)->getLine(), ((NedNote *) lptr->data)->getNoteHead(), 0));
		}
	}
	/* not correct, because it will be a new NedChordOrRest 
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_SLUR_POINT | FREE_PLACEABLE_LINE_POINT)) != 0) continue;
		((NedFreeReplaceable *) lptr->data)->clone(NULL, newChord);
	}
	*/
	return newChord;
}

void NedChordOrRest::print() {
	GList *lptr;
	
	NedResource::DbgMsg(DBG_TESTING, "len = %u(%u) time = %llu(%llu) lines = ", m_length, m_length / NOTE_64, m_midi_time, m_midi_time /NOTE_64);
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		NedResource::DbgMsg(DBG_TESTING, " %d, ", ((NedNote *) lptr->data)->getLine());
	}
}


bool NedChordOrRest::isTied() {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getTieBackward() != NULL) {
			return true;
		}
	}

	return false;
}

void NedChordOrRest::testTies() {
	GList *lptr;
	NedNote *n, *c;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		n = (NedNote *) lptr->data;
		c = n->getTieBackward();
		if (c != NULL) {
			if (c->getVoice()->getVoiceNumber() != m_voice->getVoiceNumber()) {
				n->removeBackwardTie();
			}
		}
		c = n->getTieForward();
		if (c != NULL ) {
			if (c->getVoice()->getVoiceNumber() != m_voice->getVoiceNumber()) {
				n->removeForwardTie();
			}
		}
	}
}
	

void NedChordOrRest::removeAllBackwardTies() {
	GList *lptr;
	if (m_type == TYPE_REST) return;

	for (lptr= g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->removeBackwardTie();
	}
}

bool NedChordOrRest::tieBackward(NedChordOrRest *prev, NedNote *note) {
	GList *lptr;
	int line = note->getLine();

	if (g_list_find(m_notes, note) == NULL) {
		NedResource::Abort("NedChordOrRest::tieBackward");
	}

	for (lptr = g_list_first(prev->m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getLine() == line) {
			((NedNote *) lptr->data)->setTies((NedNote *) lptr->data, note);
			return true;
		}
	}
	return false;
}

void NedChordOrRest::handleOpenTies(NedCommandList *command_list, GList *pool) {
	GList *chord_ptr1, *chord_ptr2;
	GList *note_ptr1, *note_ptr2;
	bool check;
	
	for (chord_ptr1 = g_list_first(pool); chord_ptr1; chord_ptr1 = g_list_next(chord_ptr1)) {
		if (((NedChordOrRest *) chord_ptr1->data)->getType() != TYPE_CHORD) continue;
		for (note_ptr1 = g_list_first(((NedChordOrRest *) chord_ptr1->data)->m_notes); note_ptr1; note_ptr1 = g_list_next(note_ptr1)) {
			if (((NedNote *) note_ptr1->data)->getTieForward() != NULL) {
				check = false;
				for (chord_ptr2 = g_list_first(pool); chord_ptr2; chord_ptr2 = g_list_next(chord_ptr2)) {
					if (chord_ptr1->data == chord_ptr2->data) continue;
					if (((NedChordOrRest *) chord_ptr2->data)->getType() != TYPE_CHORD) continue;
					for (note_ptr2 = g_list_first(((NedChordOrRest *) chord_ptr2->data)->m_notes); note_ptr2; note_ptr2 = g_list_next(note_ptr2)) {
						if (((NedNote *) note_ptr1->data)->getTieForward() == (NedNote *) note_ptr2) {
							check = true;
							break;
						}
					}
				}
				if (!check) {
					((NedNote *) note_ptr1->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD, true);
				}
			}
			if (((NedNote *) note_ptr1->data)->getTieBackward() != NULL) {
				check = false;
				for (chord_ptr2 = g_list_first(pool); chord_ptr2; chord_ptr2 = g_list_next(chord_ptr2)) {
					if (chord_ptr1->data == chord_ptr2->data) continue;
					if (((NedChordOrRest *) chord_ptr2->data)->getType() != TYPE_CHORD) continue;
					for (note_ptr2 = g_list_first(((NedChordOrRest *) chord_ptr2->data)->m_notes); note_ptr2; note_ptr2 = g_list_next(note_ptr2)) {
						if (((NedNote *) note_ptr1->data)->getTieBackward() == (NedNote *) note_ptr2) {
							check = true;
							break;
						}
					}
				}
				if (!check) {
					((NedNote *) note_ptr1->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, true);
				}
			}
		}
	}
}



bool NedChordOrRest::testForTiesToDelete(NedCommandList *command_list, unsigned int dir /* = BREAK_TIE_BACKWARD */, bool execute /* = false */) {
	bool deleted_ties = FALSE, delties;
	for (GList *lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		delties = ((NedNote *) lptr->data)->testForTiesToDelete(command_list, dir, execute);
		deleted_ties = deleted_ties || delties;
	}
	return deleted_ties;
}

void NedChordOrRest::testForFreeReplaceablesToDelete(NedCommandList *command_list, GList **already_deleted, bool execute /* = false */) {
	NedSlurPoint **sp;
	NedLinePoint *startp, *endp, *midp;
	GList *lptr;
	NedFreeReplaceable * freereplaceable;
	NedRemoveFreePlaceableCommand *rem_displaceable;
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		switch (((NedFreeReplaceable *) lptr->data)->getType()) {
			case FREE_PLACEABLE_SLUR_POINT:
				sp = ((NedSlurPoint *) lptr->data)->getSlurPoints();
				if (g_list_find(*already_deleted, sp[0]) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(sp[0]);
					*already_deleted = g_list_append(*already_deleted, sp[0]);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				if (g_list_find(*already_deleted, sp[1]) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(sp[1]);
					*already_deleted = g_list_append(*already_deleted, sp[1]);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				if (g_list_find(*already_deleted, sp[2]) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(sp[2]);
					*already_deleted = g_list_append(*already_deleted, sp[2]);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				break;
			case FREE_PLACEABLE_LINE_POINT:
				startp = ((NedLinePoint *) lptr->data)->getLineStartPoint();
				endp = ((NedLinePoint *) lptr->data)->getLineEndPoint();
				if (g_list_find(*already_deleted, startp) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(startp);
					*already_deleted = g_list_append(*already_deleted, startp);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				if (g_list_find(*already_deleted, endp) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(endp);
					*already_deleted = g_list_append(*already_deleted, endp);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				if ((((NedLinePoint *) lptr->data)->getLine()->getLineType() & LINE_TEMPO_CHANGE) != 0) {
					midp = ((NedLine3 *) ((NedLinePoint *) lptr->data)->getLine())->getLineMidPoint();
					if (g_list_find(*already_deleted, midp) == NULL) {
                                		rem_displaceable = new NedRemoveFreePlaceableCommand(midp);
						*already_deleted = g_list_append(*already_deleted, midp);
                                		command_list->addCommand(rem_displaceable);
                                		if (execute) rem_displaceable->execute();
					}
				}
				break;
			default: 
				freereplaceable = (NedFreeReplaceable *) lptr->data;
				if (g_list_find(*already_deleted, freereplaceable) == NULL) {
                                	rem_displaceable = new NedRemoveFreePlaceableCommand(freereplaceable);
					*already_deleted = g_list_append(*already_deleted, freereplaceable);
                                	command_list->addCommand(rem_displaceable);
                                	if (execute) rem_displaceable->execute();
				}
				break;
		}
	}
}

void NedChordOrRest::testForDeleteableConstraints(NedCommandList *command_list) {
	bool constraint = false;
	NedRemoveConstraintsCommand *rem_constr_cmd;
	NedChordOrRest *other_chord = NULL;

	constraint = ((m_status & STAT_USER_STEMDIR) != 0);
	constraint = constraint || (m_stem_height != STEM_DEFAULT_HEIGHT);
	constraint = constraint || (m_beam_y_offs != 0.0);
	constraint = constraint || (m_slope_offs != 0);

	if (constraint) {
		rem_constr_cmd = new NedRemoveConstraintsCommand(this);
		rem_constr_cmd->execute();
		command_list->addCommand(rem_constr_cmd);
	}
	if (m_beam == NULL) return;
	if (m_beam->isFirstInBeam(this)) {
		other_chord = m_beam->getLast();
	}
	else if (m_beam->isLastInBeam(this)) {
		other_chord = m_beam->getFirst();
	}
	if (other_chord == NULL) return;
	constraint = ((other_chord->m_status & STAT_USER_STEMDIR) != 0);
	constraint = constraint || (other_chord->m_stem_height != STEM_DEFAULT_HEIGHT);
	constraint = constraint || (other_chord->m_beam_y_offs != 0.0);
	constraint = constraint || (other_chord->m_slope_offs != 0);

	if (constraint) {
		rem_constr_cmd = new NedRemoveConstraintsCommand(other_chord);
		rem_constr_cmd->execute();
		command_list->addCommand(rem_constr_cmd);
	}
}

void NedChordOrRest::tryTieBackward() {
	NedChordOrRest *previousChord = getPreviousChordOrRest();
	if (previousChord == NULL) return;
	if (previousChord->getType() != TYPE_CHORD) return;
	previousChord->tieAllNotes(this);
}
		

void NedChordOrRest::correctTiesForward() {
	GList *lptr;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->correctTieForward();
	}
}

void NedChordOrRest::tieAllNotes(NedChordOrRest *other_chord) {
	NedCommandList *command_list = NULL;
	GList *lptr1, *lptr2;

	for (lptr1 = g_list_first(m_notes); lptr1; lptr1 = g_list_next(lptr1)) {
		for (lptr2 = g_list_first(other_chord->m_notes); lptr2; lptr2 = g_list_next(lptr2)) {
			if (((NedNote *) lptr1->data)->isTheSame((NedNote *) lptr2->data)) {
				NedTieNotesCommand *cmd = new NedTieNotesCommand((NedNote *) lptr1->data, (NedNote *) lptr2->data);
				if (command_list == NULL) {
					command_list = new NedCommandList(getMainWindow(), getSystem());
				}
				command_list->addCommand(cmd);
			}
		}
	}
	if (command_list != NULL) {
		command_list->execute();
		getMainWindow()->getCommandHistory()->addCommandList(command_list);
	}
}

void NedChordOrRest::tieCompleteTo(NedChordOrRest *other_chord, bool check_pitch) {
	GList *lptr1, *lptr2;
	for (lptr1 = g_list_first(m_notes); lptr1; lptr1 = g_list_next(lptr1)) {
		for (lptr2 = g_list_first(other_chord->m_notes); lptr2; lptr2 = g_list_next(lptr2)) {
			((NedNote *) lptr1->data)->setTieIfSamePitch((NedNote *) lptr2->data, check_pitch);
		}
	}
}

void NedChordOrRest::computeTies() {
	GList *lptr;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->computeTie();
	}
}


bool NedChordOrRest::noConflict(int line) {
	if (m_type == TYPE_REST) {
		NedResource::Abort("NedChordOrRest::noConflict");
	}
	if (((NedNote *) g_list_first(m_notes)->data)->getNoteHead() == GUITAR_NOTE_STEM) return false;
	if (((NedNote *) g_list_first(m_notes)->data)->getNoteHead() == GUITAR_NOTE_NO_STEM) return false;
	GList *lptr;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getLine() == line) {
			return false;
		}
	}
	return true;
}

int NedChordOrRest::getSorting(NedChordOrRest *other_chord) {
	NedVoice *other_voice;
	NedChordOrRest *c0, *c1;
	int sorting;
	if (other_chord == this) return SORTING_NONE;

	if (getPage() != other_chord->getPage()) {
		sorting = getMainWindow()->getSorting(getPage(), getStaff(), other_chord->getPage(), other_chord->getStaff());
		switch (sorting) {
			case SORTING_NONE: return SORTING_NONE;
			case SORTING_LESS: c0 = other_chord; c1 = this; break;
			default: c0 = this; c1 = other_chord; break;
		}
		if (!c0->getPage()->isLast(c0->getSystem())) return SORTING_NONE;
		if (!c1->getPage()->isFirst(c1->getSystem())) return SORTING_NONE;

		if (!c0->m_voice->isLast(c0)) return SORTING_NONE;
		if (!c1->m_voice->isFirst(c1)) return SORTING_NONE;
		return sorting;
	}

	other_voice = other_chord->m_voice;
	if (m_voice != other_voice) {
		sorting = getPage()->getSorting(getStaff(), getSystem(), other_chord->getStaff(), other_chord->getSystem());
		switch (sorting) {
			case SORTING_NONE: return SORTING_NONE;
			case SORTING_LESS: c0 = other_chord; c1 = this; break;
			default: c0 = this; c1 = other_chord; break;
		}

		if (!c0->m_voice->isLast(c0)) return SORTING_NONE;
		if (!c1->m_voice->isFirst(c1)) return SORTING_NONE;
		return sorting;
	}
	return m_voice->getSorting(this, other_chord);
}


void NedChordOrRest::reConfigure() {
	int line_sum = 0;
	unsigned int oldstemdir;
	if (m_type == TYPE_REST) {
		if (getStemDir() == STEM_DIR_NONE && !(m_status & STAT_USER_PLACED_REST)) {
			m_ypos = - 3 * LINE_DIST/2.0;
		}
		return;
	}

	m_notes = g_list_sort(m_notes,  (gint (*)(const void*, const void*)) NedNote::compare);
	GList *lptr;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		line_sum += ((NedNote *) lptr->data)->getLine();
	}
	if ((m_status & STAT_USER_STEMDIR) == 0) {
		oldstemdir = (m_status & STAT_UP);
		switch (getStemDir()) {
			case STEM_DIR_UP: m_status |= STAT_UP; if (!oldstemdir) {xPositNotes(); computeBbox();} break;
			case STEM_DIR_DOWN: m_status &= (~(STAT_UP)); if (oldstemdir) {xPositNotes(); computeBbox();} break;
			default:
			int c = 4 * g_list_length(m_notes);
			if (line_sum < c || m_length ==  WHOLE_NOTE) {
				m_status |= STAT_UP;
				if (!oldstemdir) {
					xPositNotes();
					computeBbox();
				}
			}
			else  {
				m_status &= (~(STAT_UP));
				if (oldstemdir) {
					xPositNotes();
					computeBbox();
				}
			}
			break;
		}
	}
	if (m_beam != NULL) {
		if (m_length >= NOTE_4) {
			m_beam->removeChord(this);
			m_beam = NULL;
		}
		else {
			m_beam->computeBeam(getStemDir());
		}
	}
}

void NedChordOrRest::flipStem(NedCommandList *command_list) {
	NedChangeChordOrRestStatusCommand *notestatus;
	NedFlipValuesCommand *flipbeamvaluecmd1, *flipbeamvaluecmd2;
	NedChordOrRest *other_chord;
	unsigned int newstatus;

	if (m_beam == NULL) {
		newstatus = m_status;
		newstatus |= STAT_USER_STEMDIR;
		if ((m_status & STAT_UP) == 0) {
			newstatus |= STAT_UP;
		}
		else {
			newstatus &= (~(STAT_UP));
		}
		notestatus = new NedChangeChordOrRestStatusCommand(this, newstatus);
		command_list->addCommand(notestatus);
		notestatus->execute();
	}
	else if (m_beam->isFirstInBeam(this)) {
		newstatus = m_status;
		newstatus |= STAT_USER_STEMDIR;
		if ((m_status & STAT_UP) == 0) {
			newstatus |= STAT_UP;
		}
		else {
			newstatus &= (~(STAT_UP));
		}
		notestatus = new NedChangeChordOrRestStatusCommand(this, newstatus);
		command_list->addCommand(notestatus);
		other_chord = m_beam->getLast();
		if ((((newstatus & STAT_UP) == 0) && m_beam->isBeamUp()) || (((newstatus & STAT_UP) != 0) && !m_beam->isBeamUp())) {
			flipbeamvaluecmd1 = new NedFlipValuesCommand(this);
			command_list->addCommand(flipbeamvaluecmd1);
			flipbeamvaluecmd2 = new NedFlipValuesCommand(other_chord);
			command_list->addCommand(flipbeamvaluecmd2);
			notestatus->execute();
			flipbeamvaluecmd1->execute();
			flipbeamvaluecmd2->execute();
		}
		else {
			notestatus->execute();
		}
			
	}
	else if (m_beam->isLastInBeam(this)) {
		other_chord = m_beam->getFirst();
		newstatus = other_chord->m_status;
		newstatus |= STAT_USER_STEMDIR;
		if ((newstatus & STAT_UP) == 0) {
			newstatus |= STAT_UP;
		}
		else {
			newstatus &= (~(STAT_UP));
		}
		notestatus = new NedChangeChordOrRestStatusCommand(other_chord, newstatus);
		command_list->addCommand(notestatus);
		if ((((newstatus & STAT_UP) == 0) && other_chord->m_beam->isBeamUp()) || (((newstatus & STAT_UP) != 0) && !other_chord->m_beam->isBeamUp())) {
			flipbeamvaluecmd1 = new NedFlipValuesCommand(this);
			command_list->addCommand(flipbeamvaluecmd1);
			flipbeamvaluecmd2 = new NedFlipValuesCommand(other_chord);
			command_list->addCommand(flipbeamvaluecmd2);
			notestatus->execute();
			flipbeamvaluecmd1->execute();
			flipbeamvaluecmd2->execute();
		}
		else {
			notestatus->execute();
		}
	}
		
}

void NedChordOrRest::setLength(int length) {
	m_length = length;
	unsigned int upbeat;
	unsigned int measure_length;
	if (m_length == WHOLE_NOTE) {
		if (m_voice->getVoiceNumber() > 0) {
			m_is_hidden = TRUE;
		}
	}
	else if (m_midi_time == 0 && getMainWindow()->getUpBeatInverse() > 0) {
		measure_length = getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		upbeat = measure_length - getMainWindow()->getUpBeatInverse();
		if (m_voice->getVoiceNumber() > 0 && getDuration() == measure_length) {
			m_is_hidden = TRUE;
		}
	}
		
}

bool NedChordOrRest::ReleaseTmpValue() {
	if (m_tmp_xpos < 0.0) return FALSE;
	bool ret = m_voice->setNewXpos(this, m_tmp_xpos);
	m_tmp_xpos = -1.0;
	if (!ret) {
		getMainWindow()->reposit(NULL, getPage());
	}
	return ret;
}

void NedChordOrRest::changeDuration(unsigned int new_duration, int tuplet_val) {
	if (m_tuplet_val != 0) {
		new_duration *= tuplet_val;
		new_duration /= (tuplet_val - 1);
	}
	compute_fitting_duration(new_duration, &m_length, &m_dot_count);
}

void NedChordOrRest::compute_fitting_duration(unsigned int sum, unsigned int *length, int *dot_count) {
	unsigned int dur = NOTE_64;
	while (dur <= sum && dur <= WHOLE_NOTE) {
		dur <<= 1;
	}
	dur >>= 1;
	*length = dur;
	*dot_count = (*length / 2 * 3 <= sum) ? 1 : 0;
}

bool NedChordOrRest::testXShift(double x) {
	double leftx = getMainWindow()->getLeftX();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	double xx = X_POS_INVERS_PAGE_REL(x);
	double xxdiff = xx - m_xpos;

	if (xxdiff < 0.0) xxdiff = -xxdiff;
	if (xxdiff > MIN_SHIFT_X_DIFF) {
		m_tmp_xpos = xx;
		return TRUE;
	}
	return FALSE;
}

void NedChordOrRest::moveToLine(int line) {
	if (m_type != TYPE_REST) return;
	m_line = line;
	m_ypos = -m_line * LINE_DIST/2.0;
	m_status |= STAT_USER_PLACED_REST;
}



void NedChordOrRest::computeBbox() {
	double minx = 10000.0, miny = 10000.0;
	double maxx = -10000.0, maxy = -10000.0;
	double zoom_factor;
	int zoom_level;
	cairo_text_extents_t *extention = NULL;
	NedNote *stem_start_note;
	int dir, i;
	double yy;
	int fac, ll, lll;
	double extry_y_offs = 0, off1;
	double maxlyricslength;

	switch (m_type) {
	case TYPE_CLEF:
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		extention = &(NedResource::fontextentions[zoom_level][3]);
		m_bbox.x = extention->x_bearing / zoom_factor;
		m_bbox.y = (extention->y_bearing) / zoom_factor + (m_ypos -3 * LINE_DIST/2.0);
		m_bbox.width_netto = m_bbox.width =  extention->width / zoom_factor;
		m_bbox.height = extention->height / zoom_factor;
		break;
	case TYPE_KEYSIG:
		lll = m_dot_count; // keysig so far
		if (lll < 0) lll = -lll;
		ll = (int) m_length; // "new" keysig
		off1 = (ll > 0) ? (0.5 * LINE_DIST) : 0.0;
		if (ll < 0) ll = -ll;
		m_bbox.x = 0.0;
		m_bbox.y = - 6 * LINE_DIST;
		m_bbox.width_netto = m_bbox.width = (ll + lll) * (SIG_X_SHIFT /* + EXTRA_KEY_OFF */ ) + off1;
		m_bbox.height = 7 * LINE_DIST;
		break;
	case TYPE_STAFF_SIGN:
#define BREATH_WIDTH 0.4
#define BREATH_HEIGHT (2.0 * LINE_DIST)
#define STAFF_SIGN_Y_OFFS (5.0 * LINE_DIST)
		m_bbox.x = -BREATH_WIDTH / 2.0;
		m_bbox.y = -STAFF_SIGN_Y_OFFS - 0.5 * BREATH_HEIGHT;
		m_bbox.width_netto = m_bbox.width = BREATH_WIDTH;
		m_bbox.height = BREATH_HEIGHT;
		break;
	case TYPE_REST:
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		switch(m_length) {
		case WHOLE_NOTE:
			extry_y_offs = WHOLE_NOTE_Y_OFFS;
			extention = &(NedResource::fontextentions[zoom_level][9]); break;
			break;
		case NOTE_2:
			extry_y_offs = HALF_NOTE_Y_OFFS;
			extention = &(NedResource::fontextentions[zoom_level][10]); 
			break;
		case NOTE_4:
			extention = &(NedResource::fontextentions[zoom_level][11]); break;
		case NOTE_8:
			extention = &(NedResource::fontextentions[zoom_level][12]); break;
		case NOTE_16:
			extention = &(NedResource::fontextentions[zoom_level][13]); break;
		case NOTE_32:
			extention = &(NedResource::fontextentions[zoom_level][14]); break;
		case NOTE_64:
			extention = &(NedResource::fontextentions[zoom_level][15]); break;
		}
		switch(m_length) {
		case NOTE_2:
			m_bbox.x = (extention->x_bearing - 3.2) / zoom_factor; // half rests are too close to others
			m_bbox.width_netto = m_bbox.width = (extention->width + 6.4) / zoom_factor;
			break;
		default:
			m_bbox.x = extention->x_bearing / zoom_factor;
			m_bbox.width_netto = m_bbox.width =  extention->width / zoom_factor;
			break;
		}
		m_bbox.y = (extention->y_bearing) / zoom_factor + m_ypos;
		m_bbox.height = extention->height / zoom_factor;
		switch (m_dot_count) {
			case 1: m_bbox.width_netto = (m_bbox.width += m_dot_count * HEAD_THICK + DOT1POS + extry_y_offs); break;
			case 2: m_bbox.width_netto = (m_bbox.width += m_dot_count * HEAD_THICK + DOT2POS + extry_y_offs); break;
		}
		break;
	case TYPE_CHORD:
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		GList *lptr;
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
			((NedNote *) lptr->data)->computeBounds(m_length, &minx, &miny, &maxx, &maxy);
		}
		if (m_length < NOTE_4 && m_beam == NULL && (m_status & STAT_UP)) {
			if (NedResource::getFlagWidth() > maxx) maxx = NedResource::getFlagWidth();
		}
		if (m_beam != NULL) {
			if (m_beam->isBeamUp()) {
				dir = -1;
				stem_start_note = (NedNote *) g_list_last(m_notes)->data;
			}
			else {
				dir = 1;
				stem_start_note = (NedNote *) g_list_first(m_notes)->data;
			}
			fac = 0;
		}
		else {
			dir = (m_status & STAT_UP) ? -1 : 1;
			if (m_status & STAT_UP) {
				stem_start_note = (NedNote *) g_list_last(m_notes)->data;
			}
			else {
				stem_start_note = (NedNote *) g_list_first(m_notes)->data;
			}
		  	switch(m_length) {
		  	case NOTE_32:
		  		fac = 1; break;
		  	case NOTE_64:
		  		fac = 2; break;
			default: fac = 0; break;
		  	}
		}
#define ARPEGGIO_X_DIST -0.4
#define ARPEGGIO_THICK 0.4
#define ARPEGGO_Y_SEGMENT_LENGTH 0.2
#define ARPEGG_EXTRA 0.2
#define ARPEGGIO_MIDI_DIST NOTE_32
		if (m_length < WHOLE_NOTE) {
			yy = stem_start_note->getYpos() + dir * (m_stem_height + fac * FLAG_DIST);
			if (yy > maxy) maxy = yy;
			if (yy < miny) miny = yy;
		}
		if (m_status & STAT_ARPEGGIO) { 
			minx += ARPEGGIO_X_DIST /* + (m_all_shifted ? HEAD_THICK : 0.0) */ + 0.1;
		}
#define TREMOLO_WIDTH (LINE_DIST)
#define REAL_TREMOLO_WIDTH ( 0.7 * LINE_DIST)
#define TREMOLO_SHIFT (1.2 * LINE_DIST)
#define TREMOLO_BEAM_SHIFT (0.4 * LINE_DIST)
#define TREMOLO_THICK (0.5 * LINE_DIST)
		if (m_length < WHOLE_NOTE && (m_status & TREMOLO_MASK)) {
			if (dir < 0) {
				if (maxx < TREMOLO_WIDTH) maxx = TREMOLO_WIDTH;
				if (minx > -TREMOLO_WIDTH) minx = -TREMOLO_WIDTH;
			}
			else {
				if (minx >  -2 * TREMOLO_WIDTH) minx = -2 * TREMOLO_WIDTH;
			}
		}
		m_bbox.x = minx; m_bbox.y = miny;
		m_bbox.width_netto /* width without lyrics */ = m_bbox.width = maxx - minx;
		maxlyricslength = 0.0;
		for (i = 0; i < MAX_LYRICS_LINES; i++) {
			if (m_lyrics[i] != NULL) {
				if (m_lyrics[i]->getWidth() / getMainWindow()->getCurrentZoomFactor() / getMainWindow()->getCurrentScale() > maxlyricslength) {
					maxlyricslength = m_lyrics[i]->getWidth() / getMainWindow()->getCurrentZoomFactor() / getMainWindow()->getCurrentScale();
				}
			}
		}
#define HYPHEN_LONG 0.2
		if (maxlyricslength > 0.0) {
			if (maxlyricslength + HYPHEN_LONG > m_bbox.width) {
				m_bbox.width = maxlyricslength + HYPHEN_LONG;
				m_bbox.x = -m_bbox.width / 2.0;
				m_bbox.width_netto += m_bbox.width / 2.0 - HYPHEN_LONG;
			}
		}
		m_bbox.height = maxy - miny;
		break;
	case TYPE_GRACE:
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		extention = &(NedResource::fontextentions[zoom_level][18]); 
		m_bbox.x = extention->x_bearing / zoom_factor;
		m_bbox.y = (extention->y_bearing) / zoom_factor - ((NedNote *) g_list_first(m_notes)->data)->getLine() * LINE_DIST / 2.0;
		m_bbox.width_netto = m_bbox.width =  extention->width / zoom_factor;
		m_bbox.height = extention->height / zoom_factor;
		break;
	}
}


double NedChordOrRest::compute_microshift(NedChordOrRest *right_chord, int mnum, int pos) {
	//int line1, line2, dir1, dir2;
	GList *lptr1, *lptr2;
	double new_micro_shift;
	int linedist;
	bool no_conflict;
	right_chord->m_micro_shift = m_micro_shift;

	/* ignore the beams because they can be wrong at this moment */
/*
	if (m_status & STAT_UP) {
		dir1 = -1;
		line1 = ((NedNote *) g_list_first(m_notes)->data)->getLine();
	}
	else {
		dir1 = 1;
		line1 = ((NedNote *) g_list_last(m_notes)->data)->getLine();
	}
	if (right_chord->m_status & STAT_UP) {
		dir2 = -1;
		line2 = ((NedNote *) g_list_first(right_chord->m_notes)->data)->getLine();
	}
	else {
		dir2 = 1;
		line2 = ((NedNote *) g_list_last(right_chord->m_notes)->data)->getLine();
	}
	if (dir1 * dir2 < 0 && line1 == line2) return m_micro_shift;
	if (dir1 < 0 && dir2 > 0) {
		if (line1 - line2 > 1) return m_micro_shift;
	}
	if (dir1 > 0 && dir2 < 0) {
		if (line2 - line1 > 1) return m_micro_shift;
	}
	if (right_chord->m_bbox.y > m_bbox.y + m_bbox.height) return m_micro_shift;
	if (m_bbox.y > right_chord->m_bbox.y + right_chord->m_bbox.height) return m_micro_shift;
	if (right_chord->m_xpos + right_chord->m_bbox.x > m_xpos + m_bbox.x + m_bbox.width) return m_micro_shift;
	if (right_chord->m_xpos + right_chord->m_bbox.x + right_chord->m_bbox.width < m_xpos + m_bbox.x) return m_micro_shift;
*/

	no_conflict = true;
	for (lptr1 = g_list_first(right_chord->m_notes); lptr1 && no_conflict; lptr1 = g_list_next(lptr1)) {
		for (lptr2 = g_list_first(m_notes); lptr2 && no_conflict; lptr2 = g_list_next(lptr2)) {
			linedist = ((NedNote *) lptr1->data)->getLine() - ((NedNote *) lptr2->data)->getLine();
			if (linedist < 0) linedist = -linedist;
			if (linedist > 0 && linedist < 2) no_conflict = false;
		}
	}
	if (no_conflict) return m_micro_shift;
				
	
	new_micro_shift = m_micro_shift + m_bbox.x + m_bbox.width_netto /* width without lyrics */ - (right_chord->m_bbox.x);
	if (new_micro_shift > right_chord->m_micro_shift) {
		right_chord->m_micro_shift = new_micro_shift;
	}
	return right_chord->m_micro_shift;
}

double NedChordOrRest::compute_y_shift(NedChordOrRest *other_chord, int dir) {
	if (other_chord->m_bbox.y > MIN_REST_Y_VOICE_DIST + m_bbox.y +  m_bbox.height) return -1.0;
	if (m_bbox.y > MIN_REST_Y_VOICE_DIST + other_chord->m_bbox.y +  other_chord->m_bbox.height) return -1.0;
	if (other_chord->m_xpos + other_chord->m_bbox.x > m_xpos + m_bbox.x + m_bbox.width) return -1.0;
	if (other_chord->m_xpos + other_chord->m_bbox.x + other_chord->m_bbox.width < m_xpos + m_bbox.x) return -1.0;
	if (dir == STEM_DIR_DOWN) {
		return m_bbox.y + m_bbox.height - other_chord->m_bbox.y + MIN_REST_Y_VOICE_DIST;
	}
	return other_chord->m_bbox.y + other_chord->m_bbox.height - m_bbox.y + MIN_REST_Y_VOICE_DIST;
}

void NedChordOrRest::sortNotes() {
	m_notes = g_list_sort(m_notes, (gint (*)(const void*, const void*)) NedNote::compare);
}



void NedChordOrRest::setMidiTime(unsigned long long time) {
	m_midi_time = time;
	if (m_type == TYPE_REST) {
		computeBbox();
	}
}

void NedChordOrRest::resetYPos() {
	if (m_status & STAT_USER_PLACED_REST) return;
	m_ypos = - 3 * LINE_DIST/2.0;
	computeBbox();
}

bool NedChordOrRest::hasUpDir() {
	if (m_beam != NULL) {
		return m_beam->isBeamUp();
	}
	return ((m_status & STAT_UP) != 0);
}

int NedChordOrRest::lengthForFlagCountComputation() {
	unsigned int effective_length = m_length;
	if (m_length < WHOLE_NOTE && (m_status & TREMOLO_MASK)) {
		switch (m_status & TREMOLO_MASK) {
			case STAT_TREMOLO1: if (m_length > NOTE_8) effective_length = NOTE_8; break;
			case STAT_TREMOLO2: if (m_length > NOTE_16) effective_length = NOTE_16;break;
			case STAT_TREMOLO3: if (m_length > NOTE_32) effective_length = NOTE_32;break;
			case STAT_TREMOLO4: if (m_length > NOTE_64) effective_length = NOTE_64;break;
		}
	}
	return effective_length;
}

int NedChordOrRest::numberOfFlagSigns() {
	switch(m_length) {
		case NOTE_8: return 1;
		case NOTE_16: return 2;
		case NOTE_32: return 3;
		case NOTE_64: return 4;
	}
	return 0;
}

int NedChordOrRest::numberOfTremoloSigns() {
	switch (m_status & TREMOLO_MASK) {
		case STAT_TREMOLO1: return 1;
		case STAT_TREMOLO2: return 2;
		case STAT_TREMOLO3: return 3;
		case STAT_TREMOLO4: return 4;
	}
	return 0;
}

void NedChordOrRest::draw(cairo_t *cr, bool *freetexts_or_lyrics_present) {
	cairo_glyph_t glyph;
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	int zoom_level = getMainWindow()->getCurrentZoomLevel();
	int dir, i, ll;
	double sig_xpos, yl;
	int flag_glyph = 0, tremolo_glyph = 0, clef_type, keysig_type, octave_shift;
	double real_xpos = m_tmp_xpos < 0.0 ? m_xpos : m_tmp_xpos;
	double y_start, y_stem_start, ydist;
	double oct_shift_y_offs = 0.0, oct_shift_x_offs = 0.0;
	double clef_y_offs = 0.0;
	bool oct_shift = false;
	int up_art_nr = 0, down_art_nr = 0;
	double art_y_pos;
	int noteline, line;
	bool user_hidden_rest;
	int arpeg_parts;
//#define SHOW_HIDDEN
#ifdef SHOW_HIDDEN
	bool is_hidden = false;
#endif
	GList *lptr;
	real_xpos += m_micro_shift;
	m_free_texts_present = m_lyrics_present = false;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_TEXT|FREE_PLACEABLE_CHORDNAME)) != 0) {m_free_texts_present = true; continue;}
		((NedFreeReplaceable *) lptr->data)->draw(cr, leftx, topy, zoom_factor, zoom_level, getMainWindow()->getCurrentScale());
	}
	if (m_free_texts_present) *freetexts_or_lyrics_present = true;
	switch (m_type) {
	case TYPE_CLEF:
		switch (m_length) { // kind
			case TREBLE_CLEF: glyph.index = BASE + 2;
				  	switch (m_dot_count) { // octave shift
				  		case  12: oct_shift_y_offs = OCTUP_Y_TREBLE_DIST_UP; oct_shift = true; break;
				  		case -12: oct_shift_y_offs = OCTUP_Y_TREBLE_DIST_DOWN; oct_shift = true; break;
				  	}
					break;
			case ALTO_CLEF: glyph.index = BASE + 1;
				  	switch (m_dot_count) {
				  		case  12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_UP;
						 	oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 	oct_shift = true; break;
				  		case -12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_DOWN;
						 	oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 	oct_shift = true; break;
				  	}
					break;
			case SOPRAN_CLEF: glyph.index = BASE + 1;
				  	switch (m_dot_count) {
				  		case  12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_UP;
						 	oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 	oct_shift = true; break;
				  		case -12: oct_shift_y_offs = OCTUP_Y_ALTO_DIST_DOWN;
						 	oct_shift_x_offs = OCTUP_X_ALTO_DIST;
						 	oct_shift = true; break;
				  	}
				  	clef_y_offs = 2 * LINE_DIST;
					break;
			case TENOR_CLEF: glyph.index = BASE + 1;
				  	switch (m_dot_count) {
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
			case NEUTRAL_CLEF3: glyph.index = BASE + 27;
					break;
			default: glyph.index = BASE + 3;
				  	switch (m_dot_count) {
				  		case  12: oct_shift_y_offs = OCTUP_Y_BASS_DIST_UP; oct_shift = true; break;
				  		case -12: oct_shift_y_offs = OCTUP_Y_BASS_DIST_DOWN; oct_shift = true; break;
				  	}
					break;
		}
#define CLEF_ADDITIONAL_OFFS 0.1
		glyph.x = X_POS_PAGE_REL(real_xpos + CLEF_ADDITIONAL_OFFS);
		glyph.y = Y_POS_STAFF_REL(m_ypos + clef_y_offs + -3 * LINE_DIST/2.0);
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
		}
		cairo_show_glyphs(cr, &glyph, 1);
		if (oct_shift) {
			glyph.index = BASE + 28;
			glyph.x = X_POS_PAGE_REL(real_xpos + oct_shift_x_offs + CLEF_ADDITIONAL_OFFS);
			glyph.y = Y_POS_STAFF_REL(m_ypos -3 * LINE_DIST/2.0 + clef_y_offs + oct_shift_y_offs);
			cairo_show_glyphs(cr, &glyph, 1);
		}
		cairo_stroke(cr);
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
		break;
	case TYPE_STAFF_SIGN:
		switch (m_dot_count) { // sub type
			case SIGN_BREATH1: glyph.index = BASE + 58; break;
			case SIGN_BREATH2: glyph.index = BASE + 59; break;
		}
		glyph.x = X_POS_PAGE_REL(real_xpos);
		glyph.y = Y_POS_STAFF_REL(m_ypos-STAFF_SIGN_Y_OFFS);
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
		}
		cairo_show_glyphs(cr, &glyph, 1);
		cairo_stroke(cr);
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
		break;
	case TYPE_KEYSIG:
#define KEYSIG_ADITIONAL_X_OFFS -0.2
		ll = m_dot_count;
		getStaff()->getCurrentClefAndKeysig(m_time, &clef_type, &keysig_type, &octave_shift);
		sig_xpos = real_xpos + KEYSIG_ADITIONAL_X_OFFS;
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
		}
		if (ll > 0) { // keysig type so far
			cairo_new_path(cr);
			glyph.index = BASE + 17;
			for (i = 0; i < ll; i++) {
				glyph.x = X_POS_PAGE_REL(sig_xpos);
				glyph.y = Y_POS_STAFF_REL(-NedStaff::m_sharpPos[clef_type][i] * LINE_DIST/2.0);
				sig_xpos += SIG_X_SHIFT;
				cairo_show_glyphs(cr, &glyph, 1);
			}
			cairo_stroke(cr);
		}
		else if (ll < 0) {
			cairo_new_path(cr);
			glyph.index = BASE + 17;
			for (i = 0; i > ll; i--) {
				glyph.x = X_POS_PAGE_REL(sig_xpos);
				glyph.y = Y_POS_STAFF_REL(-NedStaff::m_flatPos[clef_type][-i] * LINE_DIST/2.0);
				sig_xpos += SIG_X_SHIFT;
				cairo_show_glyphs(cr, &glyph, 1);
			}
			cairo_stroke(cr);
		}
		ll = (int) m_length;
		if (ll > 0) { // keysig type
			cairo_new_path(cr);
			glyph.index = BASE - 1;
			for (i = 0; i < ll; i++) {
				glyph.x = X_POS_PAGE_REL(sig_xpos);
				glyph.y = Y_POS_STAFF_REL(-NedStaff::m_sharpPos[clef_type][i] * LINE_DIST/2.0);
				sig_xpos += SIG_X_SHIFT;
				cairo_show_glyphs(cr, &glyph, 1);
			}
			cairo_stroke(cr);
		}
		else if (ll < 0) {
			cairo_new_path(cr);
			glyph.index = BASE + 16;
			for (i = 0; i > ll; i--) {
				glyph.x = X_POS_PAGE_REL(sig_xpos);
				glyph.y = Y_POS_STAFF_REL(-NedStaff::m_flatPos[clef_type][-i] * LINE_DIST/2.0);
				sig_xpos += SIG_X_SHIFT;
				cairo_show_glyphs(cr, &glyph, 1);
			}
			cairo_stroke(cr);
		}
		if (getMainWindow()->m_selected_chord_or_rest == this) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
		break;
	case TYPE_REST:
#ifndef SHOW_HIDDEN
		if (m_is_hidden) return;
#endif
		user_hidden_rest = false;
		if (m_status & STAT_USER_HIDDEN_REST) {
			if (getMainWindow()->getShowHidden()) {
				user_hidden_rest = true;
				cairo_set_source_rgb (cr, HIDD_RED, HIDD_GREEN, HIDD_BLUE);
			}
			else break;
		}
		glyph.x = X_POS_PAGE_REL(real_xpos);
		glyph.y = Y_POS_STAFF_REL(m_ypos);
#ifdef SHOW_HIDDEN
		if (m_is_hidden) {
			m_is_hidden = true;
			glyph.y = Y_POS_STAFF_REL(m_ypos + LINE_DIST / 2 * m_voice->getVoiceNumber());
		}
#endif
		if (m_active) {
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
		}
#ifdef SHOW_HIDDEN
		/*
		else if (m_is_hidden) {
				 cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
		}
		*/
#endif
		switch(m_length) {
		case WHOLE_NOTE:
			glyph.index = BASE +  9; break;
		case NOTE_2:
			glyph.index = BASE + 10; break;
		case NOTE_4:
			glyph.index = BASE + 11; break;
		case NOTE_8:
			glyph.index = BASE + 12; break;
		case NOTE_16:
			glyph.index = BASE + 13; break;
		case NOTE_32:
			glyph.index = BASE + 14; break;
		case NOTE_64:
			glyph.index = BASE + 15; break;
		}
		if (getMainWindow()->m_selected_chord_or_rest == this) {
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
		else if (!user_hidden_rest && getMainWindow()->doPaintColored()) {
			switch(getVoice()->getVoiceNumber()) {
			case 0: cairo_set_source_rgb (cr, DCOLOR(V1RED), DCOLOR(V1GREEN), DCOLOR(V1BLUE)); break;
			case 1: cairo_set_source_rgb (cr, DCOLOR(V2RED), DCOLOR(V2GREEN), DCOLOR(V2BLUE)); break;
			case 2: cairo_set_source_rgb (cr, DCOLOR(V3RED), DCOLOR(V3GREEN), DCOLOR(V3BLUE)); break;
			default: cairo_set_source_rgb (cr, DCOLOR(V4RED), DCOLOR(V4GREEN), DCOLOR(V4BLUE)); break;
			}
		}
		cairo_show_glyphs(cr, &glyph, 1);
		if (m_dot_count > 0) {
			double extra_x_offs, extry_y_offs;
			switch (m_length) {
				case WHOLE_NOTE: extra_x_offs = WHOLE_NOTE_X_OFFS; 
						 extry_y_offs = WHOLE_NOTE_Y_OFFS;
						 break;
				case NOTE_2:  extra_x_offs = HALF_NOTE_X_OFFS; 
						 extry_y_offs = HALF_NOTE_Y_OFFS;
						 break;
				default:	 extra_x_offs = 0;
						 extry_y_offs = 0;
						 break;
			}
				
			cairo_new_path(cr);
			cairo_arc(cr, X_POS_PAGE_REL(real_xpos + DOT1_REST_POS +  extra_x_offs), Y_POS_STAFF_REL(m_ypos + extry_y_offs),
					zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
			cairo_fill(cr);
			if (m_dot_count > 1) {
				cairo_arc(cr, X_POS_PAGE_REL(real_xpos + DOT2_REST_POS +  extra_x_offs), Y_POS_STAFF_REL(m_ypos + extry_y_offs),
						zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
				cairo_fill(cr);
			}
		}
		if (m_status & STAT_FERMATA) {
#define REST_FERMATA_DIST -0.2
			glyph.index = BASE + 45;
			glyph.x = X_POS_PAGE_REL(real_xpos + REST_FERMATA_DIST);
			glyph.y = Y_POS_STAFF_REL(-5 * LINE_DIST);
			cairo_show_glyphs(cr, &glyph, 1);

		}
			
		if (m_active || getMainWindow()->m_selected_chord_or_rest == this || getMainWindow()->doPaintColored()) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
		if (user_hidden_rest) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
#ifdef SHOW_HIDDEN
		if (m_is_hidden) {
				 cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
#endif
		break;
	case TYPE_CHORD:
		NedNote *start_note, *stem_start_note;
	
	
		if (m_beam != NULL) {
			if (m_beam->isBeamUp()) {
				dir = -1;
				start_note = (NedNote *) g_list_first(m_notes)->data;
				stem_start_note = (NedNote *) g_list_last(m_notes)->data;
			}
			else {
				dir = 1;
				if (m_all_shifted) {
					real_xpos -= HEAD_THICK;
				}
				stem_start_note = (NedNote *) g_list_first(m_notes)->data;
				start_note = (NedNote *) g_list_last(m_notes)->data;
			}
		}
		else {
			tremolo_glyph = BASE + 63;
			dir = (m_status & STAT_UP) ? -1 : 1;
			flag_glyph = (m_status & STAT_UP) ? BASE + 5 : BASE + 8;
			if (m_status & STAT_UP) {
				start_note = (NedNote *) g_list_first(m_notes)->data;
				stem_start_note = (NedNote *) g_list_last(m_notes)->data;
			}
			else {
				if (m_all_shifted) {
					real_xpos -= HEAD_THICK;
				}
				stem_start_note = (NedNote *) g_list_first(m_notes)->data;
				start_note = (NedNote *) g_list_last(m_notes)->data;
			}
		}
		y_start = start_note->getYpos();
		y_stem_start = stem_start_note->getYpos();
		if (((NedNote *) g_list_first(m_notes)->data)->getNoteHead() != GUITAR_NOTE_NO_STEM) {
		   if (m_beam == NULL) {
			switch(lengthForFlagCountComputation()) {
			case NOTE_2:
			case NOTE_4:
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_new_path(cr);
			   cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			   cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = (y_stem_start + dir * m_stem_height)));
			   cairo_stroke(cr);
			   break;
			case NOTE_8:
			   glyph.index = (m_length > NOTE_8) ? tremolo_glyph : flag_glyph;
			   glyph.x = X_POS_PAGE_REL(real_xpos);
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height - ((m_length > NOTE_8) ? TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_new_path(cr);
			   cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			   cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = (y_stem_start + dir * m_stem_height)));
			   cairo_stroke(cr);
			   break;
			case NOTE_16:
			   glyph.index = (m_length > NOTE_8) ? tremolo_glyph : flag_glyph;
			   glyph.x = X_POS_PAGE_REL(real_xpos);
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height - ((m_length > NOTE_8) ? TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.index = (m_length > NOTE_16) ? tremolo_glyph : flag_glyph;
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height - FLAG_DIST + ((m_length > NOTE_16) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_new_path(cr);
			   cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			   cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = (y_stem_start + dir * m_stem_height)));
			   cairo_stroke(cr);
			   break;
			case NOTE_32:
			   glyph.index = (m_length > NOTE_8) ? tremolo_glyph : flag_glyph;
			   glyph.x = X_POS_PAGE_REL(real_xpos);
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height + FLAG_DIST + ((m_length > NOTE_8) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.index = (m_length > NOTE_16) ? tremolo_glyph : flag_glyph;
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height + ((m_length > NOTE_16) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.index = (m_length > NOTE_32) ? tremolo_glyph : flag_glyph;
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height - FLAG_DIST + ((m_length > NOTE_32) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_new_path(cr);
			   cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			   cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = (y_stem_start + dir * (m_stem_height + FLAG_DIST))));
			   cairo_stroke(cr);
			   break;
			case NOTE_64:
			   glyph.index = (m_length > NOTE_8) ? tremolo_glyph : flag_glyph;
			   glyph.x = X_POS_PAGE_REL(real_xpos);
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height + 2 * FLAG_DIST + ((m_length > NOTE_8) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.x = X_POS_PAGE_REL(real_xpos);
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height + FLAG_DIST + ((m_length > NOTE_16) ? -TREMOLO_SHIFT : 0)));
			   glyph.index = (m_length > NOTE_16) ? tremolo_glyph : flag_glyph;
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.index = (m_length > NOTE_32) ? tremolo_glyph : flag_glyph;
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height + ((m_length > NOTE_32) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   glyph.index = (m_length > NOTE_64) ? tremolo_glyph : flag_glyph;
			   glyph.y = Y_POS_STAFF_REL(y_stem_start + dir * (m_stem_height - FLAG_DIST + ((m_length > NOTE_64) ? -TREMOLO_SHIFT : 0)));
			   cairo_show_glyphs(cr, &glyph, 1);
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_new_path(cr);
			   cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			   cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = (y_stem_start + dir * (m_stem_height + 2 * FLAG_DIST))));
			   cairo_stroke(cr);
			   break;
			}
			if (getMainWindow()->m_selected_chord_or_rest == this && ((getMainWindow()->getMainWindowStatus() & INSERT_EREASE_MODE) == 0)) {
		  	   if (getMainWindow()->m_selected_stem == this) {
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
			   }
			   else {
				cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
			   }
			   cairo_new_path(cr);
			   cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			   cairo_rectangle (cr, X_POS_PAGE_REL(m_stem_end_xpos - STEM_TOP_DIST), Y_POS_STAFF_REL(m_stem_end_ypos - STEM_TOP_DIST),
				zoom_factor * 2 * STEM_TOP_DIST, zoom_factor * 2 * STEM_TOP_DIST);
			   cairo_stroke(cr);
			   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			}

		   }
		   else {
			cairo_set_line_width(cr, zoom_factor * STEM_THICK);
			cairo_new_path(cr);
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos), Y_POS_STAFF_REL(y_start));
			cairo_line_to(cr, X_POS_PAGE_REL(m_stem_end_xpos = real_xpos), Y_POS_STAFF_REL(m_stem_end_ypos = m_beam->getYPosAt(real_xpos)));
			cairo_stroke(cr);
			if (m_status & TREMOLO_MASK) {
				ydist =  REAL_TREMOLO_WIDTH * m_beam->getSlope();
			   	cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor *  TREMOLO_THICK);
				for (i = numberOfFlagSigns(); i < numberOfTremoloSigns(); i++) {
			   		cairo_move_to(cr, X_POS_PAGE_REL(real_xpos - REAL_TREMOLO_WIDTH), Y_POS_STAFF_REL(m_stem_end_ypos + ydist - dir * (TREMOLO_BEAM_SHIFT + i * FLAG_DIST)));
			   		cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + REAL_TREMOLO_WIDTH), Y_POS_STAFF_REL(m_stem_end_ypos - ydist - dir * (TREMOLO_BEAM_SHIFT + i * FLAG_DIST)));
				}
			   	cairo_stroke(cr);
			}
		  	if (getMainWindow()->m_selected_chord_or_rest == this && ((getMainWindow()->getMainWindowStatus() & INSERT_EREASE_MODE) == 0) &&
				(m_beam->isFirstInBeam(this) || m_beam->isLastInBeam(this))) {
		  			if (getMainWindow()->m_selected_stem == this) {
						cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
					}
					else {
						cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
					}
					cairo_new_path(cr);
					cairo_set_line_width(cr, zoom_factor * STEM_THICK);
					cairo_rectangle (cr, X_POS_PAGE_REL(m_stem_end_xpos - STEM_TOP_DIST), Y_POS_STAFF_REL(m_stem_end_ypos - STEM_TOP_DIST),
					zoom_factor * 2 * STEM_TOP_DIST, zoom_factor * 2 * STEM_TOP_DIST);
					cairo_stroke(cr);
					cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		  		}
		   }
		} // "notehead" != GUITAR_NOTE_NO_STEM 
		if (m_status & STAT_ARPEGGIO) {
			cairo_new_path(cr);
			glyph.index = BASE + 46;
			glyph.x = X_POS_PAGE_REL(m_xpos  + m_bbox.x /* + (m_all_shifted ? HEAD_THICK : 0.0) */ + 0.15);
			glyph.y = Y_POS_STAFF_REL(y_start);
			arpeg_parts = (int) ( (dir * (y_stem_start - y_start) + ARPEGG_EXTRA) / ARPEGGO_Y_SEGMENT_LENGTH);
			cairo_show_glyphs(cr, &glyph, 1);
			for (i = 1; i < arpeg_parts; i++) {
				glyph.y = Y_POS_STAFF_REL(y_start + dir *  i * ARPEGGO_Y_SEGMENT_LENGTH);
				cairo_show_glyphs(cr, &glyph, 1);
			}
			cairo_stroke(cr);
		}
#define STACC_DOT_X_DIST 0.13
		if (m_status & STAT_STACC) {
			cairo_new_path(cr);
			art_y_pos = (dir < 0) ? m_art_down_y_pos[down_art_nr++] : m_art_up_y_pos[up_art_nr++];
			cairo_arc(cr, X_POS_PAGE_REL(real_xpos + dir * STACC_DOT_X_DIST), Y_POS_STAFF_REL(art_y_pos),
					zoom_factor * DOT_RAD, 0, 2.0 * M_PI);
			cairo_fill(cr);
		}
		if (m_status & STAT_STACCATISSIMO) {
			cairo_new_path(cr);
			art_y_pos = (dir < 0) ? m_art_down_y_pos[down_art_nr++] : m_art_up_y_pos[up_art_nr++];
#define STR_STACC_X_ARC_OFFS 0.13
#define STR_STACC_X_ARC_DIST 0.052
#define STR_STACC_Y_ARC_OFFS 0.08
#define STR_STACC_X_CONE_DIST 0.016
#define STR_STACC_HEIGHT 0.24
			art_y_pos -= dir * STR_STACC_HEIGHT;
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS - STR_STACC_X_ARC_DIST),
			 	Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_Y_ARC_OFFS));
			cairo_curve_to(cr, X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS - STR_STACC_X_ARC_DIST),
			 	Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_Y_ARC_OFFS),
				X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS), Y_POS_STAFF_REL(art_y_pos),
				X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS + STR_STACC_X_ARC_DIST),
				Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_Y_ARC_OFFS));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS + STR_STACC_X_CONE_DIST), Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_HEIGHT));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS - STR_STACC_X_CONE_DIST), Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_HEIGHT));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * STR_STACC_X_ARC_OFFS - STR_STACC_X_ARC_DIST),
			 	Y_POS_STAFF_REL(art_y_pos + dir * STR_STACC_Y_ARC_OFFS));
		        cairo_fill(cr);
		}
#define TENUTO_X_DIST 0.13
#define TENUTO_LONG 0.2
#define TENUTO_LONG_2 (TENUTO_LONG / 2.0)
#define TENUTO_THICK 0.04
		if (m_status & STAT_TENUTO) {
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * TENUTO_THICK);
			art_y_pos = (dir < 0) ? m_art_down_y_pos[down_art_nr++] : m_art_up_y_pos[up_art_nr++];
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * TENUTO_X_DIST - TENUTO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * TENUTO_X_DIST + TENUTO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
			cairo_stroke(cr);
		}
		if (m_status & STAT_SFORZANDO) {
#define SFORZANDO_X_DIST 0.13
#define SFORZANDO_LONG 0.2
#define SFORZANDO_LONG_2 (SFORZANDO_LONG / 2.0)
#define SFORZANDO_HIGHT 0.15
#define SFORZANDO_HIGHT_2 (SFORZANDO_HIGHT / 2.0)
#define SFORZANDO_THICK 0.04
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * SFORZANDO_THICK);
			art_y_pos = (dir < 0) ? m_art_down_y_pos[down_art_nr++] : m_art_up_y_pos[up_art_nr++];
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZANDO_X_DIST - SFORZANDO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZANDO_X_DIST + SFORZANDO_LONG_2), Y_POS_STAFF_REL(art_y_pos - dir * SFORZANDO_HIGHT_2));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZANDO_X_DIST - SFORZANDO_LONG_2), Y_POS_STAFF_REL(art_y_pos - dir * SFORZANDO_HIGHT));
			cairo_stroke(cr);
		}
		if (m_status & STAT_SFORZATO) {
#define SFORZATO_X_DIST 0.13
#define SFORZATO_LONG 0.15
#define SFORZATO_LONG_2 (SFORZATO_LONG / 2.0)
#define SFORZATO_HIGHT 0.2
#define SFORZATO_THICK1 0.08
#define SFORZATO_THICK2 0.04
			cairo_new_path(cr);
			if (dir > 0) {
				cairo_set_line_width(cr, zoom_factor * SFORZATO_THICK1);
				art_y_pos = m_art_up_y_pos[up_art_nr++];
				cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST - SFORZATO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
				cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST), Y_POS_STAFF_REL(art_y_pos - SFORZATO_HIGHT));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * SFORZATO_THICK2);
				cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST), Y_POS_STAFF_REL(art_y_pos - SFORZATO_HIGHT));
				cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST + SFORZATO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
				cairo_stroke(cr);
			}
			else {
				cairo_set_line_width(cr, zoom_factor * SFORZATO_THICK2);
				art_y_pos = m_art_down_y_pos[down_art_nr++];
				cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST - SFORZATO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
				cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST), Y_POS_STAFF_REL(art_y_pos + SFORZATO_HIGHT));
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, zoom_factor * SFORZATO_THICK1);
				cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST), Y_POS_STAFF_REL(art_y_pos + SFORZATO_HIGHT));
				cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * SFORZATO_X_DIST + SFORZATO_LONG_2), Y_POS_STAFF_REL(art_y_pos));
				cairo_stroke(cr);
			}
		}
		if (m_status & STAT_BOW_UP) {
#define BOW_UP_X_DIST 0.13
#define BOW_UP_LONG 0.2
#define BOW_UP_HIGHT 0.3
#define BOW_UP_LONG_2 (BOW_UP_LONG / 2.0)
#define BOW_UP_THICK 0.04
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * BOW_UP_THICK);
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_UP_X_DIST - BOW_UP_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] - BOW_UP_HIGHT));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_UP_X_DIST), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr]));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_UP_X_DIST + BOW_UP_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++] - BOW_UP_HIGHT));
			cairo_stroke(cr);
		}
		if (m_status & STAT_BOW_DOWN) {
#define BOW_DOWN_X_DIST 0.13
#define BOW_DOWN_LONG 0.25
#define BOW_DOWN_HIGHT 0.2
#define BOW_DOWN_UNDER 0.07
#define BOW_DOWN_LONG_2 (BOW_DOWN_LONG / 2.0)
#define BOW_DOWN_THICK1 0.04
#define BOW_DOWN_THICK2 0.08
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * BOW_DOWN_THICK1);
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST - BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] + BOW_DOWN_UNDER));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST - BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] - BOW_DOWN_HIGHT + BOW_DOWN_UNDER));
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST + BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] + BOW_DOWN_UNDER));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST + BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] - BOW_DOWN_HIGHT + BOW_DOWN_UNDER));
			cairo_stroke(cr);
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * BOW_DOWN_THICK2);
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST - BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr] - BOW_DOWN_HIGHT + BOW_DOWN_UNDER));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + dir * BOW_DOWN_X_DIST + BOW_DOWN_LONG_2), Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++] - BOW_DOWN_HIGHT + BOW_DOWN_UNDER));
			cairo_stroke(cr);
		}
		if (m_status & STAT_TRILL) {
#define PEDAL_OFF_X_DIST 0.13
#define PEDAL_ON_X_DIST 0.13
			glyph.index = BASE + 33;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_has_open_highhat) {
			glyph.index = BASE + 34;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_has_closed_highhat) {
			glyph.index = BASE + 60;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_PRALL) {
			glyph.index = BASE + 35;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_MORDENT) {
			glyph.index = BASE + 36;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_TURN) {
			glyph.index = BASE + 61;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_REV_TURN) {
			glyph.index = BASE + 62;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_FERMATA) {
			glyph.index = BASE + 45;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_up_y_pos[up_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_PED_ON) {
			glyph.index = BASE + 31;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_ON_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_down_y_pos[down_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		if (m_status & STAT_PED_OFF) {
			glyph.index = BASE + 32;
			glyph.x = X_POS_PAGE_REL(real_xpos + dir * PEDAL_OFF_X_DIST);
			glyph.y = Y_POS_STAFF_REL(m_art_down_y_pos[down_art_nr++]);
			cairo_show_glyphs(cr, &glyph, 1);

		}
		GList *lptr;
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
			((NedNote *) lptr->data)->draw(cr);
		}
		for (i = 0; !(m_lyrics_present) && i  < MAX_LYRICS_LINES; i++) {
			if (m_lyrics[i] != NULL) m_lyrics_present = true;
		}
		if (m_lyrics_present) *freetexts_or_lyrics_present = true;
		break;
	case TYPE_GRACE:
		if (getMainWindow()->m_selected_chord_or_rest == this) {
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
		stem_start_note = (NedNote *) g_list_first(m_notes)->data;
		y_stem_start = stem_start_note->getYpos();
		switch(m_length) {
			case STROKEN_GRACE: glyph.index =  BASE + 18; break;
			case GRACE_8:
			case GRACE_16: glyph.index =  BASE + 19; break;
		}
		glyph.x = X_POS_PAGE_REL(real_xpos);
		glyph.y = Y_POS_STAFF_REL(y_stem_start);
		cairo_show_glyphs(cr, &glyph, 1);
		if (m_beam == NULL) {
			switch(m_length) {
				case GRACE_16: glyph.index =  BASE + 20;
					glyph.y = Y_POS_STAFF_REL(y_stem_start + GRACE_HEIGHT2);
					cairo_show_glyphs(cr, &glyph, 1);
				case GRACE_8: glyph.index =  BASE + 20;
					glyph.y = Y_POS_STAFF_REL(y_stem_start + GRACE_HEIGHT1);
					cairo_show_glyphs(cr, &glyph, 1);
					cairo_new_path(cr);
					cairo_set_line_width(cr, zoom_factor * GRACE_STEM_THICK);
					cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + GRACE_HEAD_THICK_HALF), Y_POS_STAFF_REL(y_stem_start));
					cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + GRACE_HEAD_THICK_HALF), Y_POS_STAFF_REL(y_stem_start - GRACE_HEIGHT3));
					cairo_stroke(cr);
					break;
			}
		}
		else {
			cairo_set_line_width(cr, zoom_factor * GRACE_STEM_THICK);
			cairo_new_path(cr);
			cairo_move_to(cr, X_POS_PAGE_REL(real_xpos + GRACE_HEAD_THICK_HALF), Y_POS_STAFF_REL(y_stem_start));
			cairo_line_to(cr, X_POS_PAGE_REL(real_xpos + GRACE_HEAD_THICK_HALF), Y_POS_STAFF_REL(m_beam->getYPosAt(real_xpos + GRACE_HEAD_THICK_HALF)));
			cairo_stroke(cr);
		}
		if (getMainWindow()->m_selected_chord_or_rest == this || getMainWindow()->doPaintColored()) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
		noteline = ((NedNote *) g_list_first(m_notes)->data)->getLine();
		if (noteline < -1) {
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * AUX_LINE_THICK);
			line = noteline / 2;
			line *= 2;
			for (; line < -1; line += 2) {
				yl = - line * LINE_DIST/2.0;
				cairo_move_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift - HEAD_THICK_2 / 2.0 - AUX_LINE_OVER),
					Y_POS_STAFF_REL(yl));
				cairo_line_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift + HEAD_THICK_2 / 2.0 + AUX_LINE_OVER),
					Y_POS_STAFF_REL(yl));
			}
			cairo_stroke(cr);
		}
		if (noteline > 9) {
			cairo_new_path(cr);
			cairo_set_line_width(cr, zoom_factor * AUX_LINE_THICK);
			line = noteline / 2;
			line *= 2;
			for (; line > 9; line -= 2) {
				yl = - line * LINE_DIST/2.0;
				cairo_move_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift - HEAD_THICK_2 / 2.0 - AUX_LINE_OVER),
					Y_POS_STAFF_REL(yl));
				cairo_line_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift + HEAD_THICK_2 / 2.0 + AUX_LINE_OVER),
					Y_POS_STAFF_REL(yl));
			}
			cairo_stroke(cr);
		}
		break;
	}
//#define SHOW_BBOX
#ifdef SHOW_BBOX
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	cairo_new_path(cr);
	cairo_set_line_width(cr, zoom_factor * STEM_THICK);
	cairo_rectangle (cr, X_POS_PAGE_REL(m_xpos + m_micro_shift + m_bbox.x), Y_POS_STAFF_REL(m_bbox.y), zoom_factor * (m_bbox.width), zoom_factor * m_bbox.height);
	cairo_stroke(cr);
	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
	cairo_move_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift), Y_POS_STAFF_REL(m_bbox.y));
	cairo_line_to(cr, X_POS_PAGE_REL(m_xpos + m_micro_shift), Y_POS_STAFF_REL(m_bbox.y + m_bbox.height));
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
#endif
}

void NedChordOrRest::drawTexts(cairo_t *cr, double scale, double last_left_bbox_edge, bool last) {
	GList *lptr;
	int line, i;
	double dist;
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();

	if (m_free_texts_present) {
		for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
			if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_TEXT|FREE_PLACEABLE_CHORDNAME)) == 0) continue;
			((NedFreeReplaceable *) lptr->data)->draw(cr, leftx, topy, zoom_factor, getMainWindow()->getCurrentZoomLevel(), getMainWindow()->getCurrentScale());
		}
	}
	if (m_type != TYPE_CHORD) return;
	if (!m_lyrics_present) return;

	for (i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			m_lyrics_present = true;
			if (last_left_bbox_edge >= 0.0 && m_lyrics[i]->hasHiddenHyphen()) {
				dist = (last_left_bbox_edge - (m_xpos + m_bbox.x + m_bbox.width - 1.5 * HYPHEN_LONG)) / 2.0;
				if (dist < 0.0) dist = 0.0;

				cairo_new_path(cr);
#define HYPHEN_WIDTH 0.10
#define HYPHEN_HEIGHT 0.03
#define HYPHEN_Y_OFFS (TEXT_Y_OFFS + 0.02)
				cairo_rectangle(cr, X_POS_PAGE_REL(last_left_bbox_edge - dist - 0.25 * HYPHEN_LONG) * scale, Y_POS_STAFF_REL(getStaff()->getBottomYBorder() +
					HYPHEN_Y_OFFS + i * LYRICS_HEIGHT) * scale,
				HYPHEN_WIDTH * zoom_factor * scale, HYPHEN_HEIGHT * zoom_factor * scale);
				cairo_fill(cr);
			}
			else if (last && m_lyrics[i]->hasHiddenHyphen()) {
				cairo_new_path(cr);
				cairo_rectangle(cr, X_POS_PAGE_REL(m_xpos + m_bbox.x + m_bbox.width) * scale, Y_POS_STAFF_REL(getStaff()->getBottomYBorder() +
					HYPHEN_Y_OFFS + i * LYRICS_HEIGHT) * scale,
				HYPHEN_WIDTH * zoom_factor * scale, HYPHEN_HEIGHT * zoom_factor * scale);
				cairo_fill(cr);
			}
			m_lyrics[i]->draw(cr, X_POS_PAGE_REL(m_xpos + m_bbox.x) * scale, Y_POS_STAFF_REL(getStaff()->getBottomYBorder() + TEXT_Y_OFFS + i * LYRICS_HEIGHT) * scale, zoom_factor, scale);
		}
	}
	if (!getMainWindow()->doDrawPostscript() && m_lyrics_present && ((line = getMainWindow()->getLyricsMode()) != -1) && getMainWindow()->m_selected_chord_or_rest == this) {
		if (m_lyrics[line] != NULL) {
			cairo_stroke(cr);
			m_lyrics[line]->drawCursor(cr, scale * X_POS_PAGE_REL(m_xpos - m_bbox.width / 2.0),
			 scale * Y_POS_STAFF_REL(getStaff()->getBottomYBorder() + TEXT_Y_OFFS + line * LYRICS_HEIGHT), zoom_factor);
		}
	}
}

void NedChordOrRest::xPositNotes() {
	int line_num, last_line_num;
	bool shifted = false;
	GList *lptr;
	int dir;
	double y_stem_end, up_art_offs = 0.0, down_art_offs = 0.0;
	int art_line;
	int i = 0, j;
	m_up_art_count = m_down_art_count = 0;
	NedNote *start_note, *stem_start_note;
	static unsigned int freePosArticulations[] = { STAT_STACC, STAT_STACCATISSIMO, STAT_TENUTO, STAT_SFORZANDO, STAT_SFORZATO, 0};
	static unsigned int upPosArticulations[] = { STAT_BOW_UP, STAT_BOW_DOWN, STAT_TRILL, STAT_OPEN, STAT_PRALL, STAT_MORDENT, STAT_TURN, STAT_REV_TURN, STAT_FERMATA, 0 };
	static unsigned int downPosArticulations[] = { STAT_PED_ON, STAT_PED_OFF, 0 };
	if (m_beam) {
		dir = m_beam->isBeamUp() ? -1 : 1;
	}
	else {
		dir = ((m_status & STAT_UP) != 0) ? -1 : 1;
	}
	m_has_open_highhat = m_has_closed_highhat = false;
	if (dir < 0) {
		last_line_num = -100;
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr), i++) {
			if (((NedNote *) lptr->data)->getNoteHead() == OPEN_HIGH_HAT) {
				m_has_open_highhat = true;
			}
			if (((NedNote *) lptr->data)->getNoteHead() == CLOSED_HIGH_HAT) {
				m_has_closed_highhat = true;
			}
			line_num = ((NedNote *) lptr->data)->getLine();
			if (!shifted && line_num - last_line_num < 2) {
				((NedNote *) lptr->data)->shiftNote(true);
				shifted = true;
			}
			else {
				((NedNote *) lptr->data)->shiftNote(false);
				m_all_shifted = false;
				shifted = false;
			}
			last_line_num = line_num;
		}
		start_note = (NedNote *) g_list_first(m_notes)->data;
		stem_start_note = (NedNote *) g_list_last(m_notes)->data;
	}
	else {
		m_all_shifted = true;
		last_line_num = 100;
		i =  g_list_length(m_notes) - 1;
		for (lptr = g_list_last(m_notes); lptr; lptr = g_list_previous(lptr), i--) {
			if (((NedNote *) lptr->data)->getNoteHead() == OPEN_HIGH_HAT) {
				m_has_open_highhat = true;
			}
			if (((NedNote *) lptr->data)->getNoteHead() == CLOSED_HIGH_HAT) {
				m_has_closed_highhat = true;
			}
			line_num = ((NedNote *) lptr->data)->getLine();
			if (!shifted && last_line_num - line_num < 2) {
				((NedNote *) lptr->data)->shiftNote(false);
				shifted = true;
				m_all_shifted = false;
			}
			else {
				((NedNote *) lptr->data)->shiftNote(true);
				shifted = false;
			}
			last_line_num = line_num;
		}
		if (m_all_shifted) {
			for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
				((NedNote *) lptr->data)->shiftNote(false);
			}
		}
		stem_start_note = (NedNote *) g_list_first(m_notes)->data;
		start_note = (NedNote *) g_list_last(m_notes)->data;
			
	}
#ifdef XXX
	acc_places[0] = acc_places[1] = acc_places[2] = acc_places[3] = -10000;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->placeAccidental(FALSE /*all_shifted*/, acc_places);
	}
#endif
	if (m_beam) {
		y_stem_end = m_beam->getYPosAt(m_xpos + m_micro_shift);
	}
	else if (m_length == WHOLE_NOTE || m_type == TYPE_REST) {
		y_stem_end = stem_start_note->getYpos();
	}
	else {
		y_stem_end = stem_start_note->getYpos() + dir * m_stem_height;
	}
	for (j = 0; freePosArticulations[j]; j++) {
		if (m_status & freePosArticulations[j]) {
			art_line = start_note->getLine() + 2 * dir;
			switch (art_line) {
				case 0: case 2: case 4: case 6: case 8: art_line += dir; break;
			}
			if (dir < 0) {
				m_art_down_y_pos[m_down_art_count++] = -art_line * LINE_DIST/2.0 + down_art_offs;
				down_art_offs += LINE_DIST;
			}
			else {
				m_art_up_y_pos[m_up_art_count++] = -art_line * LINE_DIST/2.0 - up_art_offs;
				up_art_offs += LINE_DIST;
			}
		}
	}
	for (j = 0; upPosArticulations[j]; j++) {
		if (j == 3 && (m_has_open_highhat || m_has_closed_highhat)) {
			if (dir < 0) {
				m_art_up_y_pos[m_up_art_count] = y_stem_end - LINE_DIST;
			}
			else {
				art_line = start_note->getLine() + 2 * dir;
				m_art_up_y_pos[m_up_art_count] = -art_line * LINE_DIST/2.0;
			}
			if (m_art_up_y_pos[m_up_art_count] > -5 * LINE_DIST) m_art_up_y_pos[m_up_art_count] = -5 * LINE_DIST;
			m_up_art_count++;
			continue;
		}
		if (m_status & upPosArticulations[j]) {
			if (dir < 0) {
				m_art_up_y_pos[m_up_art_count] = y_stem_end - LINE_DIST;
			}
			else {
				art_line = start_note->getLine() + 2 * dir;
				m_art_up_y_pos[m_up_art_count] = -art_line * LINE_DIST/2.0;
			}
			if (m_art_up_y_pos[m_up_art_count] > -5 * LINE_DIST) m_art_up_y_pos[m_up_art_count] = -5 * LINE_DIST;
			m_art_up_y_pos[m_up_art_count] -= up_art_offs;
			if (upPosArticulations[j] & (STAT_BOW_UP | STAT_BOW_DOWN)) {
				up_art_offs += 2.5*LINE_DIST;
			}
			else if (upPosArticulations[j] & (STAT_FERMATA)) {
				up_art_offs += 2.8*LINE_DIST;
			}
			else {
				up_art_offs += 2 * LINE_DIST;
			}
			m_up_art_count++;
		}
	}
	for (j = 0; downPosArticulations[j]; j++) {
		if (m_status & downPosArticulations[j]) {
			if (dir < 0) {
				art_line = start_note->getLine() + 2 * dir;
				m_art_down_y_pos[m_down_art_count] = -art_line * LINE_DIST/2.0;
			}
			else {
				m_art_down_y_pos[m_down_art_count] = y_stem_end + LINE_DIST;
			}
			if (m_art_down_y_pos[m_down_art_count] < LINE_DIST) m_art_down_y_pos[m_down_art_count] = LINE_DIST;
			m_art_down_y_pos[m_down_art_count] += down_art_offs;
			down_art_offs += 2 * LINE_DIST;
			m_down_art_count++;
		}
	}
}

bool NedChordOrRest::testMeasureSpread(int dir) {
	if (m_measure == NULL) return false;
	if (m_measure->m_spread_fac + dir * MEASURE_SPREAD_STEP > MEASURE_MAX_SPREAD) return false;
	if (m_measure->m_spread_fac + dir * MEASURE_SPREAD_STEP < MEASURE_MIN_SPREAD) return false;
	return true;
}


void NedChordOrRest::collectNotesWithAccidentals(unsigned int *num_notes_with_accidentals, NedNote **note_field) {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->collectNotesWithAccidentals(num_notes_with_accidentals, note_field);
	}
}

double NedChordOrRest::getBeamXPos() {
	if (m_type == TYPE_GRACE) return m_xpos + m_micro_shift + GRACE_HEAD_THICK_HALF;
	if (m_all_shifted) return m_xpos + m_micro_shift - HEAD_THICK;
	return m_xpos + m_micro_shift;
}


double NedChordOrRest::getStemTop() {
	NedNote *stem_start_note;
	stem_start_note = (NedNote *) g_list_last(m_notes)->data;
	double y_stem_start = stem_start_note->getYpos();
	if (m_type == TYPE_GRACE) {
		return y_stem_start - GRACE_HEIGHT3;
	}
	switch(lengthForFlagCountComputation()) {
		case NOTE_32:
			return y_stem_start - m_stem_height - FLAG_DIST;
		case NOTE_64:
			return y_stem_start - m_stem_height - 2 * FLAG_DIST;
	}
	return y_stem_start - m_stem_height;
}

double NedChordOrRest::getTopOfChordOrRest() {
	if (m_type == TYPE_REST) {
		return -3 * LINE_DIST / 2.0;
	}
	NedNote *stem_start_note;
	stem_start_note = (NedNote *) g_list_last(m_notes)->data;
	double y_stem_start = stem_start_note->getYpos();
	if (!(m_status & STAT_UP)) {
		return y_stem_start - 0.1;
	}
	if (m_beam != NULL) {
		return m_beam->getTopOfBeam();
	}
	switch(lengthForFlagCountComputation()) {
		case NOTE_32:
			return y_stem_start - m_stem_height - FLAG_DIST;
		case NOTE_64:
			return y_stem_start - m_stem_height - 2 * FLAG_DIST;
	}
	return y_stem_start - m_stem_height;
}

double NedChordOrRest::getBottomOfChordOrRest() {
	if (m_type == TYPE_REST) {
		return -3 * LINE_DIST / 2.0;
	}
	NedNote *stem_start_note;
	stem_start_note = (NedNote *) g_list_first(m_notes)->data;
	double y_stem_start = stem_start_note->getYpos();
	if (m_status & STAT_UP) {
		return y_stem_start + 0.1;
	}
	if (m_beam != NULL) {
		return m_beam->getBottomOfBeam();
	}
	switch(lengthForFlagCountComputation()) {
		case NOTE_32:
			return y_stem_start + m_stem_height + FLAG_DIST;
		case NOTE_64:
			return y_stem_start + m_stem_height + 2 * FLAG_DIST;
	}
	return y_stem_start + m_stem_height;
}

double NedChordOrRest::getStemBottom() {
	NedNote *stem_start_note;
	stem_start_note = (NedNote *) g_list_first(m_notes)->data;
	double y_stem_start = stem_start_note->getYpos();
	switch(lengthForFlagCountComputation()) {
		case NOTE_32:
			return y_stem_start + m_stem_height + FLAG_DIST;
		case NOTE_64:
			return y_stem_start + m_stem_height + 2 * FLAG_DIST;
	}
	return y_stem_start + m_stem_height;
}

int NedChordOrRest::getStemDir() {
	if (m_measure == NULL) return STEM_DIR_NONE;
	return m_measure->getStemDirectionOfTheMeasure(
		getStaff()->getStaffNumber() * VOICE_COUNT + m_voice->getVoiceNumber());
}

/*
double getTopPos() {
	if (m_status & STAT_UP) {
		return getStemTop();
	}
	return getStemBottom();
}
*/

double NedChordOrRest::getStemYStart() {
	NedNote *start_note;
	if (m_status & STAT_UP) {
		start_note = (NedNote *) g_list_first(m_notes)->data;
	}
	else {
		start_note = (NedNote *) g_list_last(m_notes)->data;
	}
	return start_note->getYpos();
}

void NedChordOrRest::getTopBotY(int *lyrics_lines, double *topy, double *boty/* , bool *topFix, bool *botFix*/) {
	int dir;
	NedNote *start_note, *stem_start_note;
	double endposoffs = 0.0;
	double y_start;
	double freetop;
	int i;
	GList *lptr;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if ((((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_CHORD | FREE_PLACEABLE_SPACER | FREE_PLACEABLE_CHORDNAME)) == 0) continue;
		freetop = ((NedFreeReplaceable *) lptr->data)->getTop();
		//if (!*topFix) {
			if (freetop < *topy) {
				*topy = freetop;
				//if (((NedFreeReplaceable *) lptr->data)->getType() & FREE_PLACEABLE_SPACER) {
					//*topFix = true;
				//}
			}
		//}
		if (((NedFreeReplaceable *) lptr->data)->getType() & FREE_PLACEABLE_CHORD) return;
		//if (!*botFix) {
			if (freetop + 4 * LINE_DIST > *boty) {
				*boty = freetop + 4 * LINE_DIST;
				//*botFix = true;
			}
		//}
		return;
	}

	if (m_type == TYPE_REST) {
		if (m_is_hidden) {
			*topy = *boty = 3 * LINE_DIST / 2.0;
		}
		else {
			*topy = m_bbox.y + 4 * LINE_DIST;
			*boty = m_bbox.y + m_bbox.height + 4 * LINE_DIST;
		}
		return;
	}
	for (i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL && *lyrics_lines < (i + 1)) {
			*lyrics_lines = (i + 1);
		}
	}
	if (m_beam != NULL) {
		if (m_beam->isBeamUp()) {
			dir = -1;
		}
		else {
			dir = 1;
		}
	}
	else {
		dir = (m_status & STAT_UP) ? -1 : 1;
	}
	if (dir == -1) {
		start_note = (NedNote *) g_list_first(m_notes)->data;
		stem_start_note = (NedNote *) g_list_last(m_notes)->data;
		y_start = start_note->getYpos() + HEAD_THICK_HALF;
	}
	else {
		stem_start_note = (NedNote *) g_list_first(m_notes)->data;
		start_note = (NedNote *) g_list_last(m_notes)->data;
		y_start = start_note->getYpos() - HEAD_THICK_HALF;
	}
	if (m_length > NOTE_2) {
		*boty = 4 * LINE_DIST + y_start;
		*topy = 4 * LINE_DIST + stem_start_note->getYpos();
	}
	else if (m_beam != NULL) {
		if (dir == -1) {
			*boty = 4 * LINE_DIST + y_start;
			*topy = 4 * LINE_DIST + m_beam->getYPosAt(m_xpos + m_micro_shift);
		}
		else {
			*topy = 4 * LINE_DIST + y_start;
			*boty = 4 * LINE_DIST + m_beam->getYPosAt(m_xpos + m_micro_shift);
		}
/*
		*topy = 4 * LINE_DIST + m_beam->getTopOfBeam();
		*boty = 4 * LINE_DIST + m_beam->getBottomOfBeam();
*/
	}
	else {
		switch(lengthForFlagCountComputation()) {
		  	case NOTE_2:
		  	case NOTE_4:
		  	case NOTE_8:
		  	case NOTE_16:
				endposoffs = m_stem_height;
				break;
		  	case NOTE_32:
				endposoffs = m_stem_height + FLAG_DIST;
				break;
		  	case NOTE_64:
				endposoffs = m_stem_height + 2 * FLAG_DIST;
				break;
		}
	}
	double y_stem_start = stem_start_note->getYpos();
	if (dir == -1) {
		if (m_beam == NULL) {
			*topy = 4 * LINE_DIST + y_stem_start - endposoffs;
			*boty = 4 * LINE_DIST + y_start;
		}
		if (m_up_art_count > 0 && m_art_up_y_pos[m_up_art_count-1] + 3 * LINE_DIST < *topy) *topy  = m_art_up_y_pos[m_up_art_count-1] + 3 * LINE_DIST;
		if (m_down_art_count > 0 &&  m_art_down_y_pos[m_down_art_count-1] + 5 * LINE_DIST > *boty) *boty = m_art_down_y_pos[m_down_art_count-1] + 5 * LINE_DIST ;
	}
	else {
		if (m_beam == NULL) {
			*boty = 4 * LINE_DIST + (y_stem_start + endposoffs);
			*topy = 4 * LINE_DIST + y_start;
		}
		if (m_up_art_count > 0 && m_art_up_y_pos[m_up_art_count-1] + 3 * LINE_DIST < *topy)
			 *topy = m_art_up_y_pos[m_up_art_count-1] + 3 * LINE_DIST;
		if (m_down_art_count > 0 &&  m_art_down_y_pos[m_down_art_count-1] + 5 * LINE_DIST > *boty) *boty = m_art_down_y_pos[m_down_art_count-1] + 5 * LINE_DIST ;
	}
}

bool NedChordOrRest::isUp() {
	if (m_beam != NULL) {
		return m_beam->isBeamUp();
	}
	return (m_status & STAT_UP);
}


double NedChordOrRest::computeStemDist(bool thisup, NedChordOrRest *other_chord, bool otherup) {
	double d, dist = 0.0;
	int dir;
	double thisypos, otherypos;
	NedNote *this_stem_start_note, *other_stem_start_note;

	if (m_type == TYPE_REST || other_chord->m_type == TYPE_REST) return 0.0;

	dir = thisup ? 1 : -1;
	if (dir > 0) {
		this_stem_start_note = (NedNote *) g_list_first(m_notes)->data;
	}
	else {
		this_stem_start_note = (NedNote *) g_list_last(m_notes)->data;
	}
	thisypos = this_stem_start_note->getYpos() + dir * m_stem_height;

	dir = otherup ? 1 : -1;
	if (dir > 0) {
		other_stem_start_note = (NedNote *) g_list_first(other_chord->m_notes)->data;
	}
	else {
		other_stem_start_note = (NedNote *) g_list_last(other_chord->m_notes)->data;
	}
	otherypos = other_stem_start_note->getYpos() + dir * m_stem_height;
	d = otherypos - this_stem_start_note->getYpos();
	if (d < 0.0) d = -d;
	dist += d;
	d = thisypos - other_stem_start_note->getYpos();
	if (d < 0.0) d = -d;
	dist += d;
	return dist;
}

int NedChordOrRest::getLineOfLowestNote() {
	if ((m_type & TYPE_CHORD) == 0) return 0;
	return ((NedNote *) g_list_first(m_notes)->data)->getLine();
}

int NedChordOrRest::getLineOfFirstNote() {
	if ((m_type & TYPE_CHORD) == 0) return 0;

	if (m_beam) {
		if (m_beam->isBeamUp()) {
			return ((NedNote *) g_list_first(m_notes)->data)->getLine();
		}
		return ((NedNote *) g_list_last(m_notes)->data)->getLine();
	}
	if (m_status & STAT_UP) {
		return ((NedNote *) g_list_first(m_notes)->data)->getLine();
	}
	return ((NedNote *) g_list_last(m_notes)->data)->getLine();
}

int NedChordOrRest::compareMidiTimes(NedChordOrRest *c1, NedChordOrRest *c2) {
	if (c1->m_midi_time < c2->m_midi_time) return -1;
	if (c1->m_midi_time == c2->m_midi_time) return 0;
	return 1;
}

int NedChordOrRest::compareMidiTimesAndTypes(NedChordOrRest *c1, NedChordOrRest *c2) {
	if (c1->m_midi_time < c2->m_midi_time) return -1;
	if (c1->m_midi_time > c2->m_midi_time) return 1;
	if (c1->getType() == c2->getType()) return 0;
	if (c1->getType() <  c2->getType()) return -1; // TYPE_CLEF < TYPE_KEYSIG
	return 1;
}

void NedChordOrRest::shiftY(double y) {
	m_ypos += y;
	computeBbox();
}

void NedChordOrRest::setRestsY(double y) {
	if (m_status & STAT_USER_PLACED_REST) return;
	m_ypos = - 3 * LINE_DIST/2.0 + y;
	computeBbox();
}

void NedChordOrRest::setXPos(double pos) {
	m_xpos = pos;

}

void NedChordOrRest::shiftX(double x) {
	m_xpos += x;
}

void NedChordOrRest::recomputeFreeReplaceables() {
	GList *lptr;
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		((NedFreeReplaceable *) lptr->data)->recompute();
	}
}


void NedChordOrRest::changeStemDir(int dir) {
	unsigned int upbeat;
	unsigned int measure_length;
	if (m_type == TYPE_REST) {
		if (m_length != WHOLE_NOTE) {
			if (getSystem()->getSystemNumber() > 0) {
				m_is_hidden = FALSE;
				return;
			}
			if (getMainWindow()->getUpBeatInverse() == 0) {
				m_is_hidden = FALSE;
				return;
			}
			if (m_midi_time  != getMainWindow()->getUpBeatInverse()) {
				m_is_hidden = FALSE;
				return;
			}
			measure_length = getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
			upbeat = measure_length - getMainWindow()->getUpBeatInverse();
			if (getDuration() != upbeat) {
				m_is_hidden = FALSE;
			}
		}
		return;
	}
	if (m_status & STAT_USER_STEMDIR) return;
	bool changed = FALSE;
	switch(dir) {
		case STEM_DIR_UP: if (m_status & STAT_UP) break; changed = TRUE; m_status |= STAT_UP; break;
		case STEM_DIR_DOWN: if (!(m_status & STAT_UP)) break; changed = TRUE; m_status &= (~(STAT_UP)); break;
		default: reConfigure(); changed = TRUE; break;
	}
	if (changed) {
		xPositNotes();
		computeBbox();
	}
}

void NedChordOrRest::setStatus(unsigned int status) {
	if (m_type == TYPE_REST) {
		status &= (STAT_FERMATA | STAT_USER_HIDDEN_REST | STAT_USER_PLACED_REST);
		m_status = status;
	}
	else if (m_type != TYPE_CHORD) return;
	m_status = status;
}

bool NedChordOrRest::trySelect(double x, double y, bool only_free_placeables) {
	int zoom_level;
	double leftx, topy;
	double zoom_factor;
	double current_scale = getMainWindow()->getCurrentScale();
	double xl, yl;
	GList *lptr;
	cairo_text_extents_t *extention = NULL;
	if (m_freedisp) {
		leftx = getMainWindow()->getLeftX();
		topy = getMainWindow()->getTopY();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
			if (((NedFreeReplaceable *) lptr->data)->trySelect(x, y, zoom_factor, current_scale, leftx, topy)) {
				return TRUE;
			}
		}
	}
	if (only_free_placeables) return FALSE;
	switch (m_type) {
	case TYPE_CLEF:
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		leftx = getMainWindow()->getLeftX();
		topy = getMainWindow()->getTopY();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		xl = X_POS_INVERS_PAGE_REL(x);
		yl = Y_POS_INVERS_ABS(y);
		switch (m_length) {
			case TREBLE_CLEF:
				extention = &(NedResource::fontextentions[zoom_level][2]); break;
			case BASS_CLEF:
				extention = &(NedResource::fontextentions[zoom_level][3]); break;
			case ALTO_CLEF:
			case SOPRAN_CLEF:
			case TENOR_CLEF:
				extention = &(NedResource::fontextentions[zoom_level][1]); break;
			case NEUTRAL_CLEF1:
			case NEUTRAL_CLEF2:
			case NEUTRAL_CLEF3:
				extention = &(NedResource::fontextentions[zoom_level][27]); break;
		}
		if (getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + extention->y_bearing / zoom_factor < yl &&
			getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + (extention->y_bearing  +  extention->height) / zoom_factor > yl &&
		    	m_xpos + extention->x_bearing / zoom_factor < xl &&
		    	m_xpos + (extention->x_bearing  +  extention->width) / zoom_factor > xl) {
				getMainWindow()->m_selected_chord_or_rest = this;
				return TRUE;
		}
		return FALSE;
	case TYPE_STAFF_SIGN:
		leftx = getMainWindow()->getLeftX();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		topy = getMainWindow()->getTopY();
		xl = X_POS_INVERS_PAGE_REL(x);
		yl = Y_POS_INVERS_ABS(y);
		if (getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + m_bbox.y < yl &&
			getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + (m_bbox.y  +  m_bbox.height) > yl &&
		    	m_xpos + m_bbox.x  < xl &&
		    	m_xpos + m_bbox.x +  m_bbox.width  > xl) {
				getMainWindow()->m_selected_chord_or_rest = this;
				return TRUE;
		}
		return FALSE;
	case TYPE_KEYSIG:
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		leftx = getMainWindow()->getLeftX();
		topy = getMainWindow()->getTopY();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		xl = X_POS_INVERS_PAGE_REL(x);
		yl = Y_POS_INVERS_ABS(y);
		if (getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos - m_bbox.height < yl && getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos > yl &&
			m_xpos < xl && m_xpos + m_bbox.width > xl) {
				getMainWindow()->m_selected_chord_or_rest = this;
				return TRUE;
		}
		return FALSE;
	case TYPE_REST:
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		leftx = getMainWindow()->getLeftX();
		topy = getMainWindow()->getTopY();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		xl = X_POS_INVERS_PAGE_REL(x);
		yl = Y_POS_INVERS_ABS(y);
		switch(m_length) {
		case WHOLE_NOTE:
			extention = &(NedResource::fontextentions[zoom_level][9]); break;
		case NOTE_2:
			extention = &(NedResource::fontextentions[zoom_level][10]); break;
		case NOTE_4:
			extention = &(NedResource::fontextentions[zoom_level][11]); break;
		case NOTE_8:
			extention = &(NedResource::fontextentions[zoom_level][12]); break;
		case NOTE_16:
			extention = &(NedResource::fontextentions[zoom_level][13]); break;
		case NOTE_32:
			extention = &(NedResource::fontextentions[zoom_level][14]); break;
		case NOTE_64:
			extention = &(NedResource::fontextentions[zoom_level][15]); break;
		}
		if (getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + extention->y_bearing / zoom_factor < yl &&
		    getSystem()->getYPos() + getStaff()->getBottomPos() + m_ypos + (extention->y_bearing  +  extention->height) / zoom_factor > yl &&
		    m_xpos + extention->x_bearing / zoom_factor < xl &&
		    m_xpos + (extention->x_bearing  +  extention->width) / zoom_factor > xl) {
			getMainWindow()->m_selected_chord_or_rest = this;
			return TRUE;
		}
		return FALSE;
	case TYPE_GRACE:
		zoom_level = getMainWindow()->getCurrentZoomLevel();
		leftx = getMainWindow()->getLeftX();
		topy = getMainWindow()->getTopY();
		zoom_factor = getMainWindow()->getCurrentZoomFactor();
		xl = X_POS_INVERS_PAGE_REL(x);
		yl = Y_POS_INVERS_ABS(y);
		if (getSystem()->getYPos() + getStaff()->getBottomPos() + m_bbox.y < yl && getSystem()->getYPos() + getStaff()->getBottomPos() + m_bbox.y + m_bbox.height > yl &&
			m_xpos + m_bbox.x < xl && m_xpos + m_bbox.x + m_bbox.width > xl) {
				getMainWindow()->m_selected_chord_or_rest = this;
				getMainWindow()->m_selected_note = (NedNote *) g_list_first(m_notes)->data;
				return TRUE;
		}
		return FALSE;
	default:
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
			if (((NedNote *) lptr->data)->trySelect(x, y)) {
				getMainWindow()->m_selected_chord_or_rest = this;
				return TRUE;
			}
		}

		return FALSE;
	}
}

bool NedChordOrRest::hitStemEnd(double x, double y) {
	double xd, yd;
	double leftx = getMainWindow()->getLeftX();
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	double xl = X_POS_INVERS_PAGE_REL(x);
	double yl = Y_POS_INVERSE_STAFF_REL(y);

	if (m_beam == NULL) {
		xd = xl - m_stem_end_xpos;
		yd = yl - m_stem_end_ypos;
		if (xd < 0.0) xd = -xd;
		if (yd < 0.0) yd = -yd;
		if (xd < STEM_TOP_DIST && yd < STEM_TOP_DIST) {
			return true;
		}
	}
	else if (m_beam->isFirstInBeam(this) || m_beam->isLastInBeam(this))  {
		xd = xl - m_stem_end_xpos;
		yd = yl - m_stem_end_ypos;
		if (xd < 0.0) xd = -xd;
		if (yd < 0.0) yd = -yd;
		if (xd < STEM_TOP_DIST && yd < STEM_TOP_DIST) {
			return true;
		}
	}
	return false;
}


bool NedChordOrRest::scaleStem(double y, double old_ypos) {
	double yd;
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	double yl = Y_POS_INVERSE_STAFF_REL(y);
	bool ret = false;
	m_status_copy = m_status;
	if (m_beam == NULL) {
		if (m_stemheight_copy < UNSET_STEM_HEIGHT) {
			m_stemheight_copy = m_stem_height;
		}
		yd = yl - m_stem_end_ypos;
		if (m_status & STAT_UP) {
			m_stem_height -= yd;
		}
		else {
			m_stem_height += yd;
		}
		if (m_stem_height > 3 * STEM_DEFAULT_HEIGHT) m_stem_height = 3 * STEM_DEFAULT_HEIGHT;
		if (m_stem_height < 0.4 * STEM_DEFAULT_HEIGHT) m_stem_height = 0.4 * STEM_DEFAULT_HEIGHT;
	}
	else if (m_beam->isFirstInBeam(this)) {
		m_status |= STAT_USER_STEMDIR;
		if (m_beam->isBeamUp()) {
			m_status |= STAT_UP;
		}
		else {
			m_status &= (~(STAT_UP));
		}
		if (m_beam_y_offs_copy < UNSET_STEM_HEIGHT) {
			m_beam_y_offs_copy = m_beam_y_offs;
		}
		yd = yl - m_stem_end_ypos;
		m_beam_y_offs += yd;
		if (m_beam->isBeamUp()) {
			if (m_beam_y_offs < -2 * STEM_DEFAULT_HEIGHT) m_beam_y_offs = -2 * STEM_DEFAULT_HEIGHT;
			if (m_beam_y_offs > 0.4 * STEM_DEFAULT_HEIGHT) m_beam_y_offs = 0.4 * STEM_DEFAULT_HEIGHT;
		}
		else {
			if (m_beam_y_offs > 2 * STEM_DEFAULT_HEIGHT) m_beam_y_offs = 2 * STEM_DEFAULT_HEIGHT;
			if (m_beam_y_offs < -0.4 * STEM_DEFAULT_HEIGHT) m_beam_y_offs = -0.4 * STEM_DEFAULT_HEIGHT;
		}
		m_beam->computeBeam(getStaff()->getStaffNumber() * VOICE_COUNT + m_voice->getVoiceNumber());
	}
#define MIN_SLOPE_CHANGE_DIST 0.09
	else if (m_beam->isLastInBeam(this)) {
		m_status |= STAT_USER_STEMDIR;
		if (m_beam->isBeamUp()) {
			m_status |= STAT_UP;
		}
		else {
			m_status &= (~(STAT_UP));
		}
		double yl_old = Y_POS_INVERSE_STAFF_REL(old_ypos);
		double motion_dist = yl - yl_old;
		if (m_slope_offs_copy == UNSET_SLOPE_OFFS) {
			m_slope_offs_copy = m_slope_offs;
		}
		if (motion_dist > MIN_SLOPE_CHANGE_DIST * m_beam->getBeamLength()) {
			m_slope_offs--;
			ret = true;
		}
		else if (motion_dist < -MIN_SLOPE_CHANGE_DIST * m_beam->getBeamLength()) {
			m_slope_offs++;
			ret = true;
		}
		if (m_slope_offs < -SLOPE_COUNT) m_slope_offs = -SLOPE_COUNT;
		if (m_slope_offs > SLOPE_COUNT) m_slope_offs = SLOPE_COUNT;
		m_beam->computeBeam(getStaff()->getStaffNumber() * VOICE_COUNT + m_voice->getVoiceNumber());
	}
	return ret;
}

double NedChordOrRest::computeDistFrom(double x, double y, double factor, double *ydist) {
	double xdist;
	switch (m_type) {
	case TYPE_CLEF:
	case TYPE_STAFF_SIGN:
	case TYPE_KEYSIG:
	case TYPE_GRACE:
		return 100000000.0;
	default:
		xdist = m_xpos - x;
		*ydist = getSystem()->getYPos() + getStaff()->getMidPos() - y;
		return xdist * xdist + (*ydist * factor) * (*ydist * factor);
	}
}

void NedChordOrRest::setActive() {
	switch (m_type) {
		case TYPE_CHORD:
			((NedNote *) g_list_first(m_notes)->data)->m_active = true;
			break;
		default:
			m_active = true;
			break;
	}
}


void NedChordOrRest::decrMidiTime(unsigned long long incr) {
	if (incr > m_midi_time) {
		NedResource::DbgMsg(DBG_CRITICAL, "this = 0x%x, m_type = 0x%x, m_midi_time = %llu, incr = %llu\n", this, m_type, m_midi_time, incr) ;
		NedResource::Abort("NedChordOrRest::decrMidiTime, incr > m_midi_time");
	}
	m_midi_time -= incr;
}


int NedChordOrRest::getStemDirectionOfTheMeasure(int staff_voice_offs) {
	//NedMeasure *measure = getSystem()->getMeasure(m_midi_time);
	//return measure->getStemDirectionOfTheMeasure(staff_voice_offs);
	return m_measure->getStemDirectionOfTheMeasure(staff_voice_offs);
}


#ifdef AAA
unsigned int NedChordOrRest::getDuration(unsigned int measure_duration /* = -1 */) {
	int denom;
	if (m_type & (TYPE_STAFF_SIGN | TYPE_CLEF | TYPE_KEYSIG | TYPE_GRACE)) return 0;
	if (m_type == TYPE_REST && m_length == WHOLE_NOTE) {
		if ((signed int) measure_duration != -1) {
			return measure_duration;
		}
		if (m_measure == NULL) {
			return getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		}
		denom = getMainWindow()->getDenominator(m_measure->getMeasureNumber());
		if (denom == 0) {
			return getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		}
		return getMainWindow()->getNumerator(m_measure->getMeasureNumber()) * WHOLE_NOTE / denom;
	}
	return computeDuration(m_length, m_dot_count, m_tuplet_val);
}
#else
unsigned int NedChordOrRest::getDuration(unsigned int measure_duration /* = -1 */) {
	int denom;
	if (m_type & (TYPE_CLEF | TYPE_STAFF_SIGN | TYPE_KEYSIG | TYPE_GRACE | TYPE_KEYSIG /* can happen during import_from_other */)) return 0;
	if (m_type == TYPE_REST && m_length == WHOLE_NOTE) {
		if ((signed int) measure_duration != -1) {
			return measure_duration;
		}
		NedMeasure *measure = getSystem()->getMeasure(m_midi_time);
		if (measure == NULL) {
			return getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		}
		denom = getMainWindow()->getDenominator(measure->getMeasureNumber());
		if (denom == 0) {
			return getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		}
		return getMainWindow()->getNumerator(measure->getMeasureNumber()) * WHOLE_NOTE / denom;
	}
	return computeDuration(m_length, m_dot_count, m_tuplet_val);
}

#endif

unsigned int NedChordOrRest::computeDuration(unsigned int length, int dotcount, int tuplet_val) {
	unsigned int ret;
	if (dotcount == 1) {
		ret = length / 2 * 3;
		if (tuplet_val != 0) {
			ret = ret * NedResource::m_tuplet_tab[tuplet_val & TUPLET_VAR_MASK];
			return ret / (tuplet_val & TUPLET_VAR_MASK);
		}
		return ret;
	}
	if (dotcount == 2) {
		ret = length / 4 * 7;
		if (tuplet_val != 0) {
			ret = ret * NedResource::m_tuplet_tab[tuplet_val & TUPLET_VAR_MASK];
			return ret / (tuplet_val & TUPLET_VAR_MASK);
		}
		return ret;
	}
	if (tuplet_val != 0) {
		ret = length * NedResource::m_tuplet_tab[tuplet_val & TUPLET_VAR_MASK];
		return ret / (tuplet_val & TUPLET_VAR_MASK);
	}
	return length;
}


bool NedChordOrRest::tryErease(double x, double y, bool *removed) {
	NedCommandList *command_list;
	GList *lptr;
	*removed = false;
	if (m_type == TYPE_REST) return FALSE;
	if (g_list_length(m_notes) < 2) {
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
			if (((NedNote *) lptr->data)->trySelect(x, y)) {
				*removed = true;
				return TRUE;
			}
		}
	}
	else {
		for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
			if (((NedNote *) lptr->data)->trySelect(x, y)) {
				command_list = new NedCommandList(getMainWindow(), getSystem());
				((NedNote *) lptr->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
				command_list->addCommand(new NedEreaseNoteCommand(&m_notes, lptr));
				getMainWindow()->getCommandHistory()->addCommandList(command_list);
				command_list->execute();
				*removed = true;
				return FALSE;
			}
		}
		return FALSE;
	}
	return FALSE;
}

void NedChordOrRest::ereaseNote(NedNote *note) {
	GList *lptr;
	NedCommandList *command_list;
	if (m_type == TYPE_REST) return;
	if (g_list_length(m_notes) < 2) {
		NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
		testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
		command_list->addCommand(new NedDeleteChordCommand(this));
		command_list->execute();
		getMainWindow()->getCommandHistory()->addCommandList(command_list);
	}
	else {
		if ((lptr = g_list_find(m_notes, note)) == NULL) {
			NedResource::Abort("NedChordOrRest::ereaseNote");
		}
		command_list = new NedCommandList(getMainWindow(), getSystem());
		note->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
		command_list->addCommand(new NedEreaseNoteCommand(&m_notes, lptr));
		getMainWindow()->getCommandHistory()->addCommandList(command_list);
		command_list->execute();
	}
}

unsigned int NedChordOrRest::getStopTime() {
	return m_time + m_length;
}

double NedChordOrRest::getNeededSpace() {
	return m_bbox.width;
}

void NedChordOrRest::saveChordOrRest(FILE *fp) {
	GList *lptr;
	char Str[1024], *cptr;
	int marker;
	int last_font_line;
	int i;
	
	fprintf(fp, "(");
	if (m_beam != NULL || m_tuplet_ptr != NULL || m_freedisp != NULL) {
		marker = NedResource::addAddr(this);
		fprintf(fp, "<%d>", marker);
	}
	if (m_type == TYPE_REST) {
		fprintf(fp, "r");
		if (m_is_hidden) {
			fprintf(fp, "h");
		}
	}
	else {
		fprintf(fp, "n");
		if ((m_status & STAT_UP) != 0) {
			fprintf(fp, "u");
		}
		else {
			fprintf(fp, "d");
		}
	}
	fprintf(fp, " %d, %x ", m_length / LENGH_FACTOR, m_status & (~(ACCIDENTAL_MASK)));

	if (m_type == TYPE_REST && (m_status & STAT_USER_PLACED_REST)) {
		fprintf(fp, "%d ", m_line);
	}

	if (m_dot_count > 0) {
		fprintf(fp, " D %d ", m_dot_count);
	}

	if (m_type == TYPE_REST) {
		fprintf(fp, ")");
		return;
	}

	fprintf(fp, " { ");
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->saveNote(fp);
		if (lptr != g_list_last(m_notes)) {
			fprintf(fp, " , ");
		}
	}
	fprintf(fp, " } ");
#define MIN_DRIFT 0.01
	if (fabs(m_stem_height - STEM_DEFAULT_HEIGHT) > MIN_DRIFT) {
		fprintf(fp, " STEM_HEIGHT: %f ", m_stem_height);
	}
	if (fabs(m_beam_y_offs) > MIN_DRIFT) {
		fprintf(fp, " BEAM_Y_OFFS: %f ", m_beam_y_offs);
	}
	if (m_slope_offs != 0) {
		fprintf(fp, " BEAM_SLOPE_OFFS: %d ", m_slope_offs);
	}

	last_font_line = -1;
	for (i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			last_font_line = i;
		}
	}
	if (m_type == TYPE_CHORD && last_font_line >= 0) {
		fprintf(fp, "[");
		for (i = 0; i <= last_font_line; i++) {
			if (m_lyrics[i] != NULL) {
				strcpy(Str,  m_lyrics[i]->getText());
				while ((cptr = strchr(Str, ' ')) != NULL) *cptr = '_';
				fprintf(fp, " %s ", Str);
			}
			if (i < last_font_line) {
				fprintf(fp, " , ");
			}
		}
		fprintf(fp, " ] ");
	}
	fprintf(fp, ")");
}

void NedChordOrRest::saveTies(FILE *fp, bool *ties_written) {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->saveTies(fp, ties_written);
	}
}

void NedChordOrRest::saveFreePlaceables(FILE *fp, bool *free_placeables_written) {
	GList *lptr;
	NedVolumeSign *vol_sign;
	NedTempoSign *tempo_sign;
	NedLine *line;
	NedLinePoint *startp,*endp;
	NedSlurPoint **sp;
	NedFreeText *ftext;
	NedFreeSign *fsign;
	NedFreeChord *fchord;
	NedFreeSpacer *fspacer;
	NedChordName *fcn;
	char *roottext, *uptext, *downtext;
	int marker;

	if (m_freedisp) {
		marker = NedResource::getMarkerOfAddress(this);
		for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
			switch (((NedFreeReplaceable *) lptr->data)->getType()) {
				case FREE_PLACEABLE_VOLSIGN: 
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					vol_sign = (NedVolumeSign *) lptr->data;
					fprintf(fp, "( < %d %f %f > VOLUME_SIGN ", marker, vol_sign->getX(), vol_sign->getY());
					switch (vol_sign->getKind()) {
						case VOL_PPP: fprintf(fp, "VOL_PPP " ); break;
						case VOL_PP: fprintf(fp, "VOL_PP " ); break;
						case VOL_P: fprintf(fp, "VOL_P " ); break;
						case VOL_MP: fprintf(fp, "VOL_MP " ); break;
						case VOL_SP: fprintf(fp, "VOL_SP " ); break;
						case VOL_MF: fprintf(fp, "VOL_MF " ); break;
						case VOL_SF: fprintf(fp, "VOL_SF " ); break;
						case VOL_F: fprintf(fp, "VOL_F " ); break;
						case VOL_FF: fprintf(fp, "VOL_FF " ); break;
						case VOL_FFF: fprintf(fp, "VOL_FFF " ); break;
					}
					fprintf(fp,  "%d ) ", vol_sign->m_midi_volume);
					break;
				case FREE_PLACEABLE_TEMPO:
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					tempo_sign = (NedTempoSign *) lptr->data;
					fprintf(fp, "( < %d %f %f > TEMPO_SIGN %u %d %u) ",
						marker, tempo_sign->getX(), tempo_sign->getY(), tempo_sign->getKind(), tempo_sign->getDot(), tempo_sign->getTempo());
					break;
				case FREE_PLACEABLE_CHORDNAME:
					fcn = (NedChordName *) lptr->data;
					roottext = fcn->getRootText();
					uptext = fcn->getUpText();
					downtext = fcn->getDownText();
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					fprintf(fp, "( < %d %f %f > FREE_CHORD_NAME %zu %s %zu %s %zu %s %f) ",
						marker, fcn->getX(), fcn->getY(),
						 roottext == NULL ? 0 : strlen(roottext), roottext == NULL ? "" : roottext,
						 uptext == NULL ? 0 : strlen(uptext), uptext == NULL ? "" : uptext,
						 downtext == NULL ? 0 : strlen(downtext), downtext == NULL ? "" : downtext, fcn->getSize());
					break;
				case FREE_PLACEABLE_SLUR_POINT:
					if (((NedSlurPoint *) lptr->data)->m_point_number != 2) break;
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					sp = ((NedSlurPoint *) lptr->data)->getSlurPoints();
					fprintf(fp, "( < %d %f %f >  SLUR  %d %f %f %d %f %f ) ",
						NedResource::getMarkerOfAddress(sp[0]->getElement()), sp[0]->getX(), sp[0]->getY(),
						NedResource::getMarkerOfAddress(sp[1]->getElement()), sp[1]->getX(), sp[1]->getY(),
						NedResource::getMarkerOfAddress(sp[2]->getElement()), sp[2]->getX(), sp[2]->getY());
					break;
				case FREE_PLACEABLE_LINE_POINT:
					line = ((NedLinePoint *) lptr->data)->getLine();
					startp = line->getLineStartPoint();
					endp = line->getLineEndPoint();
					if (lptr->data != endp) break;
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					fprintf(fp, "( < %d %f %f > ",
						NedResource::getMarkerOfAddress(startp->getElement()), startp->getX(), startp->getY());
					switch (line->getLineType()) {
						case LINE_CRESCENDO: fprintf(fp, "LINE_CRESCENDO %d ", ((NedCrescendo *) line)->m_volume_diff); break;
						case LINE_DECRESCENDO: fprintf(fp, "LINE_DECRESCENDO %d ", ((NedCrescendo *) line)->m_volume_diff) ; break;
						case LINE_OCTAVATION1:
						case LINE_OCTAVATION_1:
						case LINE_OCTAVATION2:
						case LINE_OCTAVATION_2: fprintf(fp, "LINE_OCTAVATION %d ", ((NedOctavation *) line)->getPitchOffset()); break;
						case LINE_ACCELERANDO: fprintf(fp, "LINE_ACCELERATO %d", (int) ((NedAcceleration *) line)->m_tempodiff); break;
						case LINE_RITARDANDO: fprintf(fp, "LINE_RITARDANDO %d", (int) ((NedAcceleration *) line)->m_tempodiff); break;
					}
					if ((line->getLineType() & LINE_TEMPO_CHANGE) != 0) {
						NedLinePoint *midp = ((NedLine3 *) line)->getLineMidPoint();
						fprintf(fp, " %d %f %f %d %f %f ) ",
							NedResource::getMarkerOfAddress(midp->getElement()), midp->getX(), midp->getY(),
							NedResource::getMarkerOfAddress(endp->getElement()), endp->getX(), endp->getY());
					}
					else {
						fprintf(fp, " %d %f %f ) ",
							NedResource::getMarkerOfAddress(endp->getElement()), endp->getX(), endp->getY());
					}
					break;
				case FREE_PLACEABLE_TEXT:
					ftext = (NedFreeText *) lptr->data;
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					tempo_sign = (NedTempoSign *) lptr->data;
					fprintf(fp, "( < %d %f %f > FREE_TEXT %zu %s %zu %s %f %d %d %d %d %x %d %d %d %d ",
						marker, ftext->getX(), ftext->getY(), strlen(ftext->getText()), ftext->getText(),
							strlen(ftext->m_font_family), ftext->m_font_family,
							ftext->m_font_size, ftext->m_enable_volume_change, ftext->m_midi_volume,
							ftext->m_enable_tempo_change, ftext->getTempo(), ftext->m_segno_sign, ftext->m_midi_pgm,
							ftext->m_enable_channel, ftext->m_channel, ftext->m_anchor);
					switch (ftext->m_font_slant) {
						case CAIRO_FONT_SLANT_ITALIC: fprintf(fp, "%d ", WRITTEN_SLANT_ITALIC); break;
						case CAIRO_FONT_SLANT_OBLIQUE: fprintf(fp, "%d ", WRITTEN_SLANT_OBLIQUE); break;
						default: fprintf(fp, "%d ", WRITTEN_SLANT_NORMAL); break;
					}

					switch (ftext->m_font_weight) {
						case CAIRO_FONT_WEIGHT_BOLD: fprintf(fp, "%d ", WRITTEN_WEIGHT_BOLD); break;
						default: fprintf(fp, "%d ", WRITTEN_WEIGHT_NORMAL); break;
					}
					fprintf(fp, " ) ");
					break;
				case FREE_PLACEABLE_SIGN:
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					fsign = (NedFreeSign *) lptr->data;
					fprintf(fp, "( < %d %f %f > FREE_SIGN %d) ",
						marker, fsign->getX(), fsign->getY(), fsign->getKind());
					break;
				case FREE_PLACEABLE_CHORD:
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					fchord = (NedFreeChord *) lptr->data;
					fprintf(fp, "( < %d %f %f > FREE_CHORD %zu %s, %d, %x ) ",
						marker, fchord->getX(), fchord->getY(), strlen(fchord->getCode()), fchord->getCode(), fchord->getChordName(), fchord->getStatus());
					break;
				case FREE_PLACEABLE_SPACER:
					if (!*free_placeables_written) {
						fprintf(fp, "FREE_PLACEABLES ");
						*free_placeables_written = true;
					}
					fspacer = (NedFreeSpacer *) lptr->data;
					fprintf(fp, "( < %d %f %f > FREE_SPACER ) ",
						marker, fspacer->getX(), fspacer->getY());
					break;
			}
		}
	}
}


NedNote* NedChordOrRest::getFirstNote() {
	return (NedNote *)g_list_first(m_notes)->data;
}

NedChordOrRest *NedChordOrRest::getNextChordOrRest() {return m_voice->getNextChordOrRest(this);}
NedChordOrRest *NedChordOrRest::getPreviousChordOrRest() {return m_voice->getPreviousChordOrRest(this);}

NedChordOrRest *NedChordOrRest::getFirstObjectInNextSystem() {
	return getSystem()->getFirstObjectInNextSystem(getStaff()->getStaffNumber());
}

void NedChordOrRest::prepareMutedReplay() {
	GList *lptr;
	int staff_nr;
	struct staff_context_str *contexts;
	staff_nr = getStaff()->getStaffNumber();
	contexts = getMainWindow()->m_staff_contexts;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEMPO) {
			NedResource::addTempoChange(m_midi_time, ((NedTempoSign *) lptr->data)->getTempoInverse());
		}
	}
}

void NedChordOrRest::resetActiveFlags() {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->resetActiveFlag();
	}
}

void NedChordOrRest::prepareReplay(int clef, int keysig, int octave_shift, int voice_idx, unsigned int grace_time) {
	GList *lptr;
	int staff_nr, i;
	unsigned int midi_start_offs;
	NedCrescendo *cres;
	NedAcceleration *accel;
	NedFreeText *ftext;
	bool reset_pitch_offset = false;
	struct staff_context_str *contexts;
	NedLinePoint *startp, *endp;
	NedOctavation *oct;
	staff_nr = getStaff()->getStaffNumber();
	contexts = getMainWindow()->m_staff_contexts;
	unsigned short segno_sign = 0;

	if (m_status & STAT_PED_ON) {
		NedResource::addMidiCtrlEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
				getMidiTime(), MIDI_CTL_SUSTAIN, 100);
	}
	if (m_status & STAT_PED_OFF) {
		NedResource::addMidiCtrlEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
				getMidiTime(), MIDI_CTL_SUSTAIN, 1);
	}
	if (m_status & STAT_FERMATA) {
		NedResource::addFermata(getMidiTime());
	}
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEMPO) {
			NedResource::addTempoChange(m_midi_time, ((NedTempoSign *) lptr->data)->getTempoInverse());
		}
		else if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEXT) {
			ftext = (NedFreeText *) lptr->data;
			if (ftext->m_enable_tempo_change) {
				NedResource::addTempoChange(m_midi_time, ftext->getTempoInverse());
			}
			segno_sign |= ftext->m_segno_sign;
			if (ftext->m_enable_channel) {
				getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel = ftext->m_channel;
			}
			if (ftext->m_midi_pgm >= 0) {
				getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].voices[0].m_current_midi_program = ftext->m_midi_pgm;
			}
			if (ftext->m_enable_volume_change) {
				NedResource::addPseudoEvent2(m_midi_time, PSEUDO_EVENT_VOL_CHANGE, getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
					ftext->m_midi_volume);
			}
		}
		else if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_VOLSIGN) {
			NedResource::addPseudoEvent2(m_midi_time, PSEUDO_EVENT_VOL_CHANGE, getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel,
				((NedVolumeSign *) lptr->data)->m_midi_volume);
			
		}
		else if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_LINE_POINT) {
			if ((((NedLinePoint *) lptr->data)->getLine()->getLineType() & (LINE_CRESCENDO | LINE_DECRESCENDO)) != 0) {
				startp = ((NedLinePoint *) lptr->data)->getLine()->getLineStartPoint();
				endp = ((NedLinePoint *) lptr->data)->getLine()->getLineEndPoint();
				cres = (NedCrescendo *) ((NedLinePoint *) lptr->data)->getLine();
				if (startp == lptr->data) {
					NedResource::addPseudoEvent2((unsigned int) m_midi_time, PSEUDO_EVENT_CRES_START, 
						getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel, cres->m_volume_diff);
				}
				else if (endp == lptr->data) { 
					NedResource::addPseudoEvent2((unsigned int) m_midi_time, PSEUDO_EVENT_CRES_STOP,
						getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel, cres->m_volume_diff);
				}
			}
			else if ((((NedLinePoint *) lptr->data)->getLine()->getLineType() & LINE_TEMPO_CHANGE) != 0) {
				startp = ((NedLinePoint *) lptr->data)->getLine()->getLineStartPoint();
				endp = ((NedLinePoint *) lptr->data)->getLine()->getLineEndPoint();
				accel = (NedAcceleration *) ((NedLinePoint *) lptr->data)->getLine();
				if (startp == lptr->data) {
					NedResource::addPseudoEvent2((unsigned int) m_midi_time, PSEUDO_EVENT_RIT_START,
						0 /*dummy */,  accel->m_tempodiff);
				}
				else if (endp == lptr->data) { 
					NedResource::addPseudoEvent2((unsigned int) m_midi_time, PSEUDO_EVENT_RIT_STOP,
						0 /*dummy */, accel->m_tempodiff);
				}
			}
			else if ((((NedLinePoint *) lptr->data)->getLine()->getLineType() & LINE_OCTAVIATION_MASK) != 0) {
				startp = ((NedLinePoint *) lptr->data)->getLine()->getLineStartPoint();
				endp = ((NedLinePoint *) lptr->data)->getLine()->getLineEndPoint();
				oct = (NedOctavation *) ((NedLinePoint *) lptr->data)->getLine();
				if (startp == lptr->data) {
					contexts[staff_nr].m_pitch_offs = oct->getPitchOffset();
				}
				else if (endp == lptr->data) {
					reset_pitch_offset = true;
				}
			}
		}
		else if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_SIGN) {
			switch (((NedFreeSign *) lptr->data)->getKind()) {
				case SIGN_SEGNO: segno_sign |= SEGNO_SIGN; break;
				case SIGN_CODA:  segno_sign |= SEGNO_CODA; break;
			}
		}
	}

	midi_start_offs = 0;
	if ((m_status & STAT_ARPEGGIO) != 0 && g_list_length(m_notes) * ARPEGGIO_MIDI_DIST < getDuration()) { 
		midi_start_offs = ARPEGGIO_MIDI_DIST;
	}
	if (m_type & (TYPE_CHORD | TYPE_GRACE) ) {
		for (i = 0, lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr), i++) {
			((NedNote *) lptr->data)->prepareReplay(m_midi_time + i * midi_start_offs, clef, keysig, octave_shift, contexts[staff_nr].m_pitch_offs, voice_idx, grace_time, m_type == TYPE_GRACE, (m_status & (STAT_STACC | STAT_STACCATISSIMO)) != 0, segno_sign);
		}
	}
	else if (segno_sign) { // ensure segno staff attached to rests is correctly handled
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel, (unsigned char) m_voice->getVoiceNumber(),
			m_midi_time, SND_SEQ_EVENT_NOTEON | PSEUDO2_FLAG /* keep during replay */ , 60 /* dummy */,
				0 /* dummy */, segno_sign, NULL /* dummy */);
		NedResource::addMidiNoteEvent(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()].m_effective_channel, (unsigned char) m_voice->getVoiceNumber(),
			m_midi_time + getDuration() - grace_time, SND_SEQ_EVENT_NOTEOFF | PSEUDO2_FLAG /* keep during replay */ , 60 /* dummy */, 0, segno_sign, NULL /* dummy */);
	}
	if (reset_pitch_offset) {
		contexts[staff_nr].m_pitch_offs = 0;
	}
}

void NedChordOrRest::exportLilyPond(FILE *fp, int *last_line, unsigned int *midi_len, NedSlur **lily_slur,
		unsigned int *lyrics_map, bool *guitar_chordnames, bool *chordname, int *breath_script, bool keep_beams) {
	GList *lptr;
	bool force_length = false;
	char restchar;
	int dotcount, tremolo;
	unsigned int duration, len2;
	char Str[100], Str2[100];
	int i;
	int clef, keysig, octave_shift;
	int new_line, old_line;
	int clef_number;
	bool tie = false;
	duration = getDuration();

	switch (m_type) {
		case TYPE_REST: 
			exportLilyFreePlaceablesBefore(fp);
			restchar =m_is_hidden ? 's' : 'r';
			if (m_length == WHOLE_NOTE && duration != WHOLE_NOTE) {
				NedResource::setLengthField(duration);
				len2 = NedResource::getPartLength(&dotcount);
				sprintf(Str, "%c%d", restchar, WHOLE_NOTE / len2);
				for (i = 0; i < dotcount; i++) {
					strcat(Str, ".");
				}
				strcat(Str, " ");
				len2 = NedResource::getPartLength(&dotcount);
				while (len2) {
					sprintf(Str2, "%c%d", restchar, WHOLE_NOTE / len2);
					strcat(Str, Str2);
					for (i = 0; i < dotcount; i++) {
						strcat(Str, ".");
					}
					strcat(Str, " ");
					len2 = NedResource::getPartLength(&dotcount);
				}
				fprintf(fp, Str);
				*midi_len = 0;
			}
			else {
				fprintf(fp, "%c%d ", restchar, WHOLE_NOTE / m_length);
				for (i = 0; i < m_dot_count; i++) {
					fprintf(fp, ".");
				}
				*midi_len = getDuration();
			}
			exportLilyAccents(fp);
			exportLilyFreePlaceablesAfter(fp, lily_slur, guitar_chordnames, chordname);
			putc(' ', fp);
			break;
		case TYPE_CHORD:
			exportLilyFreePlaceablesBefore(fp);
			if (g_list_length(m_notes) > 1) {
				fprintf(fp, " <");
			}
			for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
				((NedNote *) lptr->data)->exportLilyPond(fp, last_line, &tie);
			}
			*last_line = ((NedNote *) g_list_first(m_notes)->data)->getLine();
			if (g_list_length(m_notes) > 1) {
				fprintf(fp, " >");
			}
			if (duration != *midi_len || (m_status & TREMOLO_MASK)) {
				if (m_status & TREMOLO_MASK) {
					switch (m_status & TREMOLO_MASK) {
						case STAT_TREMOLO1: tremolo =  8; break;
						case STAT_TREMOLO2: tremolo = 16; break;
						case STAT_TREMOLO3: tremolo = 32; break;
						case STAT_TREMOLO4: tremolo = 64; break;
					}
					sprintf(Str, "%d:%d", WHOLE_NOTE / m_length, tremolo);
				}
				else {
					sprintf(Str, "%d", WHOLE_NOTE / m_length);
				}
				for (i = 0; i < m_dot_count; i++) {
					strcat(Str, ".");
				}
				force_length = true;
			}
			if (force_length) {
				fprintf(fp, Str);
			}
			exportLilyAccents(fp);
			if (keep_beams && m_beam) {
				if (m_beam->getFirst() == this) {
					fprintf(fp, " [ ");
				}
				if (m_beam->getLast() == this) {
					fprintf(fp, " ] ");
				}
			}
			if (tie) {
				fprintf(fp, " ~ ");
			}
			exportLilyFreePlaceablesAfter(fp, lily_slur, guitar_chordnames, chordname);
			putc(' ', fp);
			*midi_len = getDuration();
			for (i = 0; i < MAX_LYRICS_LINES; i++) {
				if (m_lyrics[i] != NULL) {
					(*lyrics_map) |= (1 << i);
				}
			}
			break;
		case TYPE_GRACE:
			switch (m_length) {
				case STROKEN_GRACE: fprintf(fp, "\\acciaccatura "); break;
				default: fprintf(fp, "\\grace{ "); break;
			}
			for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
				((NedNote *) lptr->data)->exportLilyPond(fp, last_line, &tie);
			}
			switch (m_length) {
				case GRACE_8:
				case STROKEN_GRACE: fprintf(fp, "8 "); break;
				case GRACE_16: fprintf(fp, "16 "); break;
			}
			switch (m_length) {
				case STROKEN_GRACE: break;
				default: fprintf(fp, "}"); break;
			}
			*midi_len = 0;
			break;
		case TYPE_CLEF:
			getStaff()->getCurrentClefAndKeysig(m_midi_time - NOTE_64, &clef, &keysig, &octave_shift);
			old_line = NedResource::determineLastLine(-2, clef);
			clef_number = m_length;
			new_line = NedResource::determineLastLine(-2, clef_number);
			fprintf(fp, " \\clef %s ", NedResource::getLilyPondClefName(clef_number));
			*last_line += new_line - old_line;
			break;
		case TYPE_STAFF_SIGN:
			switch (m_dot_count) {
				case SIGN_BREATH1: if (*breath_script != LILY_BREAH_SCRIPT_RCOMMA) {
							fprintf(fp, " \\override BreathingSign #'text = #(make-musicglyph-markup \"scripts.rcomma\") "); 
							*breath_script = LILY_BREAH_SCRIPT_RCOMMA;
						   }
						   fprintf(fp, " \\breathe ");
						   break;
				case SIGN_BREATH2: if (*breath_script != LILY_BREAH_SCRIPT_UPBOW) {
							fprintf(fp, " \\override BreathingSign #'text = #(make-musicglyph-markup \"scripts.upbow\") ");
							*breath_script = LILY_BREAH_SCRIPT_UPBOW;
						   }
						   fprintf(fp, " \\breathe ");
						   break;
			}
			break;
		case TYPE_KEYSIG:
			fprintf(fp, " %s ", NedResource::getLilyPondKeySigName(m_length));
			break;
	}
}

void NedChordOrRest::exportLilyAccents(FILE *fp) {
	if (m_status & STAT_STACC) {
		fprintf(fp, "\\staccato ");
	}
	if (m_status & STAT_TENUTO) {
		fprintf(fp, "\\tenuto ");
	}
	if (m_status & STAT_SFORZATO) {
		fprintf(fp, "\\marcato ");
	}
	if (m_status & STAT_SFORZANDO) {
		fprintf(fp, "\\accent ");
	}
	if (m_status & STAT_BOW_UP) {
		fprintf(fp, "\\upbow ");
	}
	if (m_status & STAT_BOW_DOWN) {
		fprintf(fp, "\\downbow ");
	}
	if (m_status & STAT_PED_ON) {
		fprintf(fp, "\\sustainDown ");
	}
	if (m_status & STAT_PED_OFF) {
		fprintf(fp, "\\sustainUp ");
	}
	if (m_status & STAT_TRILL) {
		fprintf(fp, "\\trill ");
	}
	if (m_status & STAT_PRALL) {
		fprintf(fp, "\\prall ");
	}
	if (m_status & STAT_MORDENT) {
		fprintf(fp, "\\mordent ");
	}
	if (m_status & STAT_TURN) {
		fprintf(fp, "\\turn ");
	}
	if (m_status & STAT_REV_TURN) {
		fprintf(fp, "\\reverseturn ");
	}
	if (m_status & STAT_OPEN) {
		fprintf(fp, "\\open ");
	}
	if (m_status & STAT_FERMATA) {
		fprintf(fp, "\\fermata ");
	}
	if (m_status & STAT_ARPEGGIO) {
		fprintf(fp, "\\arpeggio ");
	}
}

void NedChordOrRest::exportLilyFreePlaceablesBefore(FILE *fp) {
	GList *lptr;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		switch (((NedFreeReplaceable *) lptr->data)->getType()) {
			case FREE_PLACEABLE_TEMPO:
				fprintf(fp, "\\tempo ");
				switch (((NedTempoSign *) lptr->data)->getKind()) {
					case WHOLE_NOTE: fprintf(fp, "1="); break;
					case NOTE_2: fprintf(fp, "2="); break;
					case NOTE_4: fprintf(fp, "4="); break;
					case NOTE_8: fprintf(fp, "8="); break;
					case NOTE_16: fprintf(fp, "16="); break;
				}
				fprintf(fp, "%d ", ((NedTempoSign *) lptr->data)->getTempo());
				break;
		}
	}
}

void NedChordOrRest::exportLilyFreePlaceablesAfter(FILE *fp, NedSlur **lily_slur, bool *guitar_chordnames, bool *chordnames) {
	GList *lptr;
	NedSlur *slur;
	NedSlurPoint **sp;
	char Str[1024];
	int bracenum;
	NedLine *line;
	NedLinePoint *endp;
	NedLinePoint *startp;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		switch (((NedFreeReplaceable *) lptr->data)->getType()) {
			case FREE_PLACEABLE_VOLSIGN: 
				switch (((NedVolumeSign *) lptr->data)->getKind()) {
					case VOL_PPP: fprintf(fp, "\\ppp"); break;
					case VOL_PP: fprintf(fp, "\\pp"); break;
					case VOL_P: fprintf(fp, "\\p"); break;
					case VOL_MF: fprintf(fp, "\\mf"); break;
					case VOL_SF: fprintf(fp, "\\sf"); break;
					case VOL_MP: fprintf(fp, "\\mp"); break;
					case VOL_SP: fprintf(fp, "\\sp"); break;
					case VOL_F: fprintf(fp, "\\f"); break;
					case VOL_FF: fprintf(fp, "\\ff"); break;
					case VOL_FFF: fprintf(fp, "\\fff"); break;
				}
				break;
			case FREE_PLACEABLE_SLUR_POINT:
				slur = ((NedSlurPoint *) lptr->data)->getSlur();
				sp = slur->getSlurPoints();
				if (((NedSlurPoint *) lptr->data) == sp[0]) {
					if (*lily_slur == NULL) {
						fprintf(fp, " ( ");
						*lily_slur = slur;
					}
				}
				else if (((NedSlurPoint *) lptr->data) == sp[2]) {
					if (*lily_slur == slur) {
						fprintf(fp, " ) " );
						*lily_slur = NULL;
					}
				}
				break;
			case FREE_PLACEABLE_LINE_POINT:
				line = ((NedLinePoint *) lptr->data)->getLine();
				startp = line->getLineStartPoint();
				if (lptr->data == startp) {
					switch (line->getLineType()) {
						case LINE_CRESCENDO: fprintf(fp, "\\< "); break;
						case LINE_DECRESCENDO: fprintf(fp, "\\> "); break;
					}
				}
				endp = line->getLineEndPoint();
				if (lptr->data == endp) {
					switch (line->getLineType()) {
						case LINE_CRESCENDO: 
						case LINE_DECRESCENDO: fprintf(fp, "\\! "); break;
					}
				}
				break;
			case FREE_PLACEABLE_TEXT:
				fprintf(fp, "^\\markup {");
				bracenum = 0;
				if (((NedFreeText *) lptr->data)->m_font_slant == PANGO_STYLE_ITALIC) {
					fprintf(fp, " \\italic {"); bracenum++;
				}
				if (((NedFreeText *) lptr->data)->m_font_weight == PANGO_WEIGHT_BOLD) {
					fprintf(fp, " \\bold {"); bracenum++;
				}
				((NedFreeText *) lptr->data)->getLilyText(Str);
				fprintf(fp, "%s }", Str);
				while (bracenum--) {
					fprintf(fp, " } ");
				}
				break;
			case FREE_PLACEABLE_SIGN:
				switch (((NedFreeSign *) lptr->data)->getKind()) {
					case SIGN_SEGNO: fprintf(fp, "\\mark \\markup { \\musicglyph #\"scripts.segno\" } "); break;
					case SIGN_CODA: fprintf(fp, "\\mark \\markup { \\musicglyph #\"scripts.coda\" } "); break;
				}
				break;
			case FREE_PLACEABLE_CHORD:
				if ((((NedFreeChord *) lptr->data)->getStatus() & GUITAR_CHORD_DRAW_TEXT) != 0) {
					*guitar_chordnames = true;
				}
				if ((((NedFreeChord *) lptr->data)->getStatus() & GUITAR_CHORD_DRAW_DIAGRAM) == 0) break;
				NedChordPainter::writeLilyChord(fp, ((NedFreeChord *) lptr->data)->getChordPtr());
				break;
			case FREE_PLACEABLE_CHORDNAME:
				*chordnames = true;
				break;
		}
	}
}

void NedChordOrRest::exportLilyLyrics(FILE *fp, int line_nr, int *sil_count) {
#define MAX_LILY_SIL_COUNT 8
	GList *lptr;
	bool has_tie;
	char buffer[1024];
	if (line_nr < 0 || line_nr >= MAX_LYRICS_LINES) {
		NedResource::Abort("NedChordOrRest::exportLilyLyrics");
	}

	switch (m_type) {
		case TYPE_CHORD:
			has_tie = false;
			for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
				if (((NedNote *) lptr->data)->getTieBackward() != NULL) {
					has_tie = true;
				}
			}
			if (has_tie) {
				break;
			}
			if (m_lyrics[line_nr] == NULL) {
				fprintf(fp, "\" \"" );
			}
			else {
				NedPangoCairoText::makeLilyString(m_lyrics[line_nr]->getText(), buffer);
				fprintf(fp, "%s ", buffer);
			}
			(*sil_count)++;
			if (*sil_count > MAX_LILY_SIL_COUNT) {
				fprintf(fp, "\n\t");
				*sil_count = 0;
			}
			break;
	}
}

void NedChordOrRest::exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len) {
	GList *lptr;
	int dotcount;
	unsigned int duration, len2;
	char Str[100], Str2[100];
	int i;
	bool chord_name_written = false;

	duration = getDuration();

	if ((m_type & (TYPE_CHORD | TYPE_REST)) == 0) return;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_CHORD) {
			if (((((NedFreeChord *) lptr->data)->getStatus() & GUITAR_CHORD_DRAW_TEXT) != 0)) {
				NedChordPainter::writeLilyChordName(fp, (NedFreeChord *) lptr->data, m_length, duration, m_dot_count, midi_len);
				chord_name_written = true;
			}
		}
	}
	if (chord_name_written) return;
		

	if (m_length == WHOLE_NOTE && duration != WHOLE_NOTE) {
		NedResource::setLengthField(duration);
		len2 = NedResource::getPartLength(&dotcount);
		sprintf(Str, "s%d", WHOLE_NOTE / len2);
		for (i = 0; i < dotcount; i++) {
			strcat(Str, ".");
		}
		strcat(Str, " ");
		len2 = NedResource::getPartLength(&dotcount);
		while (len2) {
			sprintf(Str2, "s%d", WHOLE_NOTE / len2);
			strcat(Str, Str2);
			for (i = 0; i < dotcount; i++) {
				strcat(Str, ".");
			}
			strcat(Str, " ");
			len2 = NedResource::getPartLength(&dotcount);
		}
		fprintf(fp, Str);
		*midi_len = 0;
	}
	else {
		if (*midi_len == getDuration()) {
			fprintf(fp, "s");
		}
		else {
			fprintf(fp, "s%d ", WHOLE_NOTE / m_length);
			for (i = 0; i < m_dot_count; i++) {
				fprintf(fp, ".");
			}
			*midi_len = getDuration();
		}
	}
	putc(' ', fp);
}

void NedChordOrRest::exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len) {
	GList *lptr;
	int dotcount;
	unsigned int duration, len2;
	char Str[100], Str2[100];
	int i;
	bool chord_name_written = false;

	duration = getDuration();

	if ((m_type & (TYPE_CHORD | TYPE_REST)) == 0) return;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_CHORDNAME) {
			NedResource::writeLilyFreeChord(fp, (NedChordName *) lptr->data, m_length, duration, m_dot_count, midi_len);
			chord_name_written = true;
		}
	}
	if (chord_name_written) return;
		

	if (m_length == WHOLE_NOTE && duration != WHOLE_NOTE) {
		NedResource::setLengthField(duration);
		len2 = NedResource::getPartLength(&dotcount);
		sprintf(Str, "s%d", WHOLE_NOTE / len2);
		for (i = 0; i < dotcount; i++) {
			strcat(Str, ".");
		}

		len2 = NedResource::getPartLength(&dotcount);
		while (len2) {
			sprintf(Str2, "s%d", WHOLE_NOTE / len2);
			strcat(Str, Str2);
			for (i = 0; i < dotcount; i++) {
				strcat(Str, ".");
			}
			strcat(Str, " ");
			len2 = NedResource::getPartLength(&dotcount);
		}
		fprintf(fp, Str);
		*midi_len = 0;
	}
	else {
		if (*midi_len == getDuration()) {
			fprintf(fp, "s");
		}
		else {
			fprintf(fp, "s%d ", WHOLE_NOTE / m_length);
			for (i = 0; i < m_dot_count; i++) {
				fprintf(fp, ".");
			}
			*midi_len = getDuration();
		}
	}
	putc(' ', fp);
}

void NedChordOrRest::collectLyrics(NedLyricsEditor *leditor) {
	GList *lptr;
	bool has_tie;
	int line_nr;
	if (m_type != TYPE_CHORD) return;

	has_tie = false;
	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		if (((NedNote *) lptr->data)->getTieBackward() != NULL) {
			has_tie = true;
		}
	}
	if (has_tie) {
		return;
	}
	for (line_nr = 0; line_nr < MAX_LYRICS_LINES; line_nr++) {
		if (m_lyrics[line_nr] == NULL) {
			leditor->addDot(line_nr);
		}
		else {
			leditor->appendText(line_nr, m_lyrics[line_nr]->getText());
		}
	}
}
			

void NedChordOrRest::zoomFreeReplaceables(double zoom, double scale) {
	GList *lptr;
	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() & (FREE_PLACEABLE_TEXT|TYPE_CHORDNAME|FREE_PLACEABLE_CHORDNAME)) {
			((NedFreeReplaceable *) lptr->data)->setZoom(zoom, scale);
		}
	}
	for (int i = 0; i < MAX_LYRICS_LINES; i++) {
		if (m_lyrics[i] != NULL) {
			m_lyrics[i]->setZoom(zoom, scale);
		}
	}
}

double NedChordOrRest::determineTempoInverse() {
	return getMainWindow()->determineTempoInverse(this);
}

void NedChordOrRest::determineTempoInverse(double *tempoinverse) {
	GList *lptr;
	double temp;

	for (lptr = g_list_first(m_freedisp); lptr; lptr = g_list_next(lptr)) {
		if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_LINE_POINT) {
			if ((((NedLinePoint *) lptr->data)->getLine()->getLineType() & LINE_TEMPO_CHANGE) != 0) {
				NedLinePoint *startp = ((NedLinePoint *) lptr->data)->getLine()->getLineStartPoint();
				if (startp == lptr->data) {
					temp = 60000.0 / *tempoinverse;
					temp += ((NedAcceleration *) ((NedLinePoint *) lptr->data)->getLine())->m_tempodiff;
					*tempoinverse = 60000.0 / temp;
					if (*tempoinverse < 10.0) *tempoinverse = 10.0;
					if (*tempoinverse > 999.0) *tempoinverse = 999.0;
				}
			}
		}
		else if (((NedFreeReplaceable *) lptr->data)->getType() == FREE_PLACEABLE_TEMPO) {
			*tempoinverse = ((NedTempoSign *) lptr->data)->getTempoInverse();
		}
	}
}

void NedChordOrRest::removeUnneededAccidentals(int clef, int keysig, int octave_shift) {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->removeUnneededAccidental(clef, keysig, octave_shift);
	}
}

void NedChordOrRest::setOffset(char offs_array[115]) {
	GList *lptr;

	for (lptr = g_list_first(m_notes); lptr; lptr = g_list_next(lptr)) {
		((NedNote *) lptr->data)->setOffset(offs_array);
	}
}

NedChordOrRest *NedChordOrRest::restoreChordOrRest(FILE *fp, NedVoice *voice) {
	char buffer[128], note_descr[128], *cptr;
	NedChordOrRest *chord_or_rest;
	NedNote *note;
	int ival;
	double fval;
	int length;
	int dotcount = 0;
	int marker, chord_marker;
	bool marker_read = FALSE, chord_marker_read = FALSE;
	unsigned int status = 0;
	int line, head;
	int rest_line = -1000;

	if (!NedResource::readWord(fp, buffer)) {
		NedResource::m_error_message = "Note or address expected";
		return NULL;
	}
	if (!strcmp(buffer, "<")) {
		if (!NedResource::readInt(fp, &chord_marker)) {
			NedResource::m_error_message = "Hex number expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ">")) {
			NedResource::m_error_message = "> expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "Note expected";
			return NULL;
		}
		chord_marker_read = TRUE;
	}
	strcpy(note_descr, buffer);
	if (!NedResource::readInt(fp, &length)) {
		NedResource::m_error_message = "Note length expected";
		return NULL;
	}
	if (voice->getMainWindow()->getFileVersion() > 5) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ",")) {
			NedResource::m_error_message = ", expected(0)";
			return NULL;
		}
		if (!NedResource::readHex(fp, (int *) &status)) {
			NedResource::m_error_message = "status expected";
			return NULL;
		}
	}
	if (status & STAT_USER_PLACED_REST) {
		if (!NedResource::readInt(fp, &rest_line)) {
			NedResource::m_error_message = "rest line expected(1)";
			return NULL;
		}
	}
	if (!NedResource::readWord(fp, buffer)) {
		NedResource::m_error_message = "{ or D expected";
		return NULL;
	}
	if (!strcmp(buffer, "D")) {
		if (!NedResource::readInt(fp, &dotcount)) {
			NedResource::m_error_message = "dotcount expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "{ expected";
			return NULL;
		}
	}
	if (note_descr[0] == 'r') {
		if (note_descr[1] != '\0') {
			if (note_descr[1] != 'h') {
				NedResource::m_error_message = "rh expected";
				return NULL;
			}
			if (voice->getMainWindow()->getFileVersion() > 9) {
				chord_or_rest = new NedChordOrRest(voice, TYPE_REST, TRUE, 3, dotcount, length * LENGH_FACTOR, NORMAL_NOTE, status, 0);
			}
			else {
				chord_or_rest = new NedChordOrRest(voice, TYPE_REST, TRUE, 3, dotcount, length * FACTOR, NORMAL_NOTE, status, 0);
			}
			if (chord_marker_read) {
				NedResource::addAddr(chord_marker, chord_or_rest);
			}
			if (rest_line > -900) {
				chord_or_rest->moveToLine(rest_line);
			}
			return chord_or_rest;
		}
		if (voice->getMainWindow()->getFileVersion() > 9) {
			chord_or_rest = new NedChordOrRest(voice, TYPE_REST, FALSE, 3, dotcount, length * LENGH_FACTOR, NORMAL_NOTE, status, 0);
		}
		else {
			chord_or_rest = new NedChordOrRest(voice, TYPE_REST, FALSE, 3, dotcount, length * FACTOR, NORMAL_NOTE, status, 0);
		}
		if (chord_marker_read) {
			NedResource::addAddr(chord_marker, chord_or_rest);
		}
			
		if (rest_line > -900) {
			chord_or_rest->moveToLine(rest_line);
		}
		return chord_or_rest;
	}
	if (note_descr[0] != 'n') {
		NedResource::m_error_message = "n or r expected";
		return NULL;
	}
	if (strcmp(buffer, "{")) {
		NedResource::m_error_message = "{ expected";
		return NULL;
	}

	if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
		NedResource::m_error_message = " | _, b , #, p, c, or = expected(1)";
		return NULL;
	}
	if (buffer[0] == '|') {
		if (!NedResource::readInt(fp, &marker)) {
			NedResource::m_error_message = "addr expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[0] != '|' || buffer[1] != '\0') {
			NedResource::m_error_message = " | expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
			NedResource::m_error_message = " _, b , #, p, c, or = expected";
			return NULL;
		}
		marker_read = TRUE;
	}
	switch (buffer[0]) {
		case '_': break;
		case 'b': status |= STAT_FLAT; break;
		case 'p': status |= STAT_DFLAT; break;
		case '#': status |= STAT_SHARP; break;
		case 'c': status |= STAT_DSHARP; break;
		case '=': status |= STAT_NATURAL; break;
		default: NedResource::m_error_message = "_, b , #, p, c, or = expected"; return NULL;
	}
	if (!NedResource::readInt(fp, &line) || line < MIN_LINE || line > MAX_LINE) {
		printf("line = %d\n", line); fflush(stdout);
		NedResource::m_error_message = "line expected(2)";
		return NULL;
	}
	head = NORMAL_NOTE;
	if (!NedResource::readWord(fp, buffer)) {
		NedResource::m_error_message = " [ or } or , expected";
		return NULL;
	} 
	if (strcmp(buffer, "[")) {
		NedResource::unreadWord(buffer);
	}
	else {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "head")) {
			NedResource::m_error_message = "head expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
			NedResource::m_error_message = ": expected";
			return NULL;
		}
		if (!NedResource::readInt(fp, &head) || head < CROSS_NOTE1 || head > CLOSED_HIGH_HAT) {
			NedResource::m_error_message = "head value expected(2)";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "]")) {
			NedResource::m_error_message = "] expected";
			return NULL;
		}
	}

	if (voice->getMainWindow()->getFileVersion() > 9) {
		chord_or_rest = new NedChordOrRest(voice, TYPE_CHORD, FALSE, line, dotcount, length * LENGH_FACTOR, head, status, 0);
	}
	else {
		chord_or_rest = new NedChordOrRest(voice, TYPE_CHORD, FALSE, line, dotcount, length * FACTOR, head, status, 0);
	}
	if (chord_marker_read) {
		NedResource::addAddr(chord_marker, chord_or_rest);
	}
	if (marker_read) {
		NedResource::addAddr(marker, g_list_first(chord_or_rest->m_notes)->data);
	}
			
	if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
		NedResource::m_error_message = " } or ,  expected";
		return NULL;
	}

	while (buffer[0] != '}') {
		marker_read = FALSE;
		if (buffer[0] != ',') {
			NedResource::DbgMsg(DBG_CRITICAL, "gelesen(1): *%s*\n", buffer); 
			NedResource::m_error_message = " ,  expected";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
			NedResource::DbgMsg(DBG_CRITICAL, "buffer = %s\n", buffer);
			NedResource::m_error_message = " | _, b , #, p, c, or = expected(2)";
			return NULL;
		}
		if (buffer[0] == '|') {
			if (!NedResource::readInt(fp, &marker)) {
				NedResource::m_error_message = "addr expected";
				return NULL;
			}
			if (!NedResource::readWord(fp, buffer) || buffer[0] != '|' || buffer[1] != '\0') {
				NedResource::m_error_message = " | expected";
				return NULL;
			}
			if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
				NedResource::m_error_message = " _, b , #, p, c, or = expected";
				return NULL;
			}
			marker_read = TRUE;
		}
		status = 0;
		switch (buffer[0]) {
			case '_': break;
			case 'p': status |= STAT_DFLAT; break;
			case 'b': status |= STAT_FLAT; break;
			case '#': status |= STAT_SHARP; break;
			case 'c': status |= STAT_DSHARP; break;
			case '=': status |= STAT_NATURAL; break;
			default: NedResource::m_error_message = "_, b , #, p, c or = expected"; return NULL;
		}
		if (!NedResource::readInt(fp, &line) || line < MIN_LINE || line > MAX_LINE) {
			printf("line = %d\n", line); fflush(stdout);
			NedResource::m_error_message = "line expected(3)";
			return NULL;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = " }, [ or ,  expected";
			return NULL;
		}
		head = NORMAL_NOTE;
		if (strcmp(buffer, "[")) {
			NedResource::unreadWord(buffer);
		}
		else {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "head")) {
				NedResource::m_error_message = "head expected";
				return NULL;
			}
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": expected";
				return NULL;
			}
			if (!NedResource::readInt(fp, &head) || head < CROSS_NOTE1 || head > CLOSED_HIGH_HAT) {
				NedResource::m_error_message = "head value expected(1)";
				return NULL;
			}
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "]")) {
				NedResource::m_error_message = "] expected";
				return NULL;
			}
		}
		note = new NedNote(chord_or_rest, line, head, status);
		void *addr = (void *) note;
		chord_or_rest->insertNoteAt(note, FALSE);
		if (marker_read) {
			NedResource::addAddr(marker, addr);
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::DbgMsg(DBG_CRITICAL, "gelesen(2): *%s*\n", buffer); 
			NedResource::m_error_message = " } or ,  expected";
			return NULL;
		}
	}
	if (!NedResource::readWord(fp, buffer)) {
		NedResource::m_error_message = " ) or STEM_HEIGHT or BEAM_Y_OFFS or BEAM_SLOPE_OFFS expected";
		return NULL;
	}
	while (!strcmp(buffer, "STEM_HEIGHT")  || !strcmp(buffer, "BEAM_Y_OFFS") || !strcmp(buffer, "BEAM_SLOPE_OFFS")) {
		if (!strcmp(buffer, "STEM_HEIGHT")) {
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ':' || buffer[1] != '\0') {
				NedResource::m_error_message = " : expected";
				return NULL;
			}
			if (!NedResource::readFloat(fp, &fval)) {
				NedResource::m_error_message = "stem height value expected";
				return NULL;
			}
			chord_or_rest->setStemHeight(fval);
		}
		else if (!strcmp(buffer, "BEAM_Y_OFFS")) {
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ':' || buffer[1] != '\0') {
				NedResource::m_error_message = " : expected";
				return NULL;
			}
			if (!NedResource::readFloat(fp, &fval)) {
				NedResource::m_error_message = "beam y offset value expected";
				return NULL;
			}
			chord_or_rest->setBeamYOffset(fval);
		}
		else if (!strcmp(buffer, "BEAM_SLOPE_OFFS")) {
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ':' || buffer[1] != '\0') {
				NedResource::m_error_message = " : expected";
				return NULL;
			}
			if (!NedResource::readInt(fp, &ival)) {
				NedResource::m_error_message = "beam slope offset value expected";
				return NULL;
			}
			chord_or_rest->setBeamSlopeOffset(ival);
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = " ) or STEM_HEIGHT or BEAM_Y_OFFS or BEAM_SLOPE_OFFS expected";
			return NULL;
		}
	}
	if (buffer[0] == '[' && buffer[1] == '\0') {
		line = 0;
		while (buffer[0] != ']') {
			if (!NedResource::readString(fp, buffer)) {
				NedResource::m_error_message = "lyrics expected";
				return NULL;
			}
			if (buffer[0] == ',' && buffer[1] == '\0') {
				line++;
				continue;
			}
			while ((cptr = strchr(buffer, '_')) != NULL) *cptr = ' ';
			chord_or_rest->createLyrics(line, buffer);
			if (!NedResource::readWord(fp, buffer) || (buffer[0] != ']' && buffer[0] != ',') || buffer[1] != '\0') {
				NedResource::m_error_message = ",  or ]  expected";
				return NULL;
			}
			line++;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[0] != ')' || buffer[1] != '\0') {
			NedResource::m_error_message = " )  expected";
			return NULL;
		}
	}
	else if (buffer[0] != ')' || buffer[1] != '\0') {
		NedResource::m_error_message = " )  expected";
		return NULL;
	}
	chord_or_rest->xPositNotes();
	chord_or_rest->computeBbox();
	return chord_or_rest;
}



void NedChordOrRest::removeFreeDisplaceable(NedFreeReplaceable *freedisplaceable) {
	GList *lptr;

	if ((lptr = g_list_find(m_freedisp, freedisplaceable)) == NULL) {
		NedResource::Abort("removeFreeDisplaceable");
	}
	m_freedisp = g_list_delete_link(m_freedisp, lptr);
}

void NedChordOrRest::addFreeDislaceable(NedFreeReplaceable *freedisplaceable)  {
	if (g_list_find(m_freedisp, freedisplaceable)) {
		return;
	}
	m_freedisp = g_list_append(m_freedisp, freedisplaceable);
}


