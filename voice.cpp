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
#include "voice.h"
#include "beaming.h"
#include "staff.h"
#include "chordorrest.h"
#include "system.h"
#include "page.h"
#include "resource.h"
#include "system.h"
#include "mainwindow.h"
#include "commandlist.h"
#include "clipboard.h"
#include "beam.h"
#include "tuplet.h"
#include "note.h"
#include "commandhistory.h"
#include "insertchordorrestcommand.h"
#include "ereasechordorrestcommand.h"
#include "changechordorrestlength.h"
#include "addnoteatcommand.h"
#include "deletechordorrestgroupcommand.h"
#include "insertchordorrestgroupcommand.h"
#include "changechordorreststatus.h"
#include "deletechordcommand.h"
#include "changexpositioncommand.h"
#include "inserttiedelementcommand.h"
#include "settotupletcommand.h"
#include "insertstaffelemcommand.h"
#include "musicxmlimport.h"
#include "volumesign.h"
#include "temposign.h"
#include "freesign.h"
#include "slur.h"
#include "slurpoint.h"
#include "linepoint.h"
#include "crescendo.h"
#include "octavation.h"
#include "acceleration.h"
#include "freetext.h"
#include "freechord.h"
#include "freechordname.h"
#include "freespacer.h"



#define DEFAULT_STAFF_DIST (4 * LINE_DIST)

#define X_POS_INVERS_PAGE_REL(p) ((leftx + (p) / current_scale) / zoom_factor - getPage()->getContentXpos())
#define Y_POS_INVERS_STAFF_REL(p) ((topy + (p) / current_scale) / zoom_factor - (getSystem()->getYPos() + getStaff()->getBottomPos()))

#define X_POS_PAGE_REL(p) ((getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_STAFF_REL(p) (((p) + getSystem()->getYPos() + getStaff()->getBottomPos()) * zoom_factor - topy)

#define X_PS_POS(p) ((DEFAULT_BORDER + LEFT_RIGHT_BORDER + (p)) * PS_ZOOM)
#define Y_PS_POS_STAFF_REL(p) ((height - ((p) + getSystem()->getYPos() + getStaff()->getBottomPos())) * PS_ZOOM)


NedVoice::NedVoice(NedStaff *staff, int nr, bool start) :
m_chord_or_rests(NULL), m_beam_list(NULL), m_tuplet_list(NULL), m_voice_nr(nr), m_staff(staff), m_start_of_last_imported(NULL) {
#ifdef XXX
	unsigned int rest_time = 0;
	int i;

	if (start) {
		for (i = 0; i < START_MEASURE_COUNT; i++) {
			NedChordOrRest *rest = new NedChordOrRest(this, TYPE_REST, nr > 0, 3, 0, WHOLE_NOTE, getMainWindow->getCurrentNoteHead(), 0, rest_time);
			m_chord_or_rests = g_list_append(m_chord_or_rests, rest);
			rest_time = rest->getStopTime();
		}
	}
#endif
}

NedVoice *NedVoice::clone(struct addr_ref_str **addrlist, struct addr_ref_str **slurlist, NedStaff *p_staff) {
	GList *lptr;
	NedVoice *voice = new NedVoice(p_staff, m_voice_nr, false);

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		voice->m_chord_or_rests = g_list_append(voice->m_chord_or_rests, ((NedChordOrRest *) lptr->data)->clone(addrlist, slurlist, voice));
	}

	return voice;
}

void NedVoice::adjust_pointers(struct addr_ref_str *addrlist, struct addr_ref_str *slurlist) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->adjust_pointers(addrlist, slurlist);
	}
}


void NedVoice::appendWholeRest(NedCommandList *command_list) {

	NedChordOrRest *rest = new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0);
	if (command_list == NULL) {
		m_chord_or_rests = g_list_append(m_chord_or_rests, rest);
		return;
	}
	NedInsertChordOrRestCommand *command = new NedInsertChordOrRestCommand(this, -1, rest);
	command->resetAdjustable(); // avoid ajjustment to filling wholes if undo/redo
	command->execute();
	command_list->addCommand(command);
}

void NedVoice::testTies() {
	GList *lptr1;

	for (lptr1 = g_list_first(m_chord_or_rests); lptr1; lptr1 = g_list_next(lptr1)) {
		((NedChordOrRest *) lptr1->data)->testTies();
	}
}

void NedVoice::setUpbeatRests(unsigned int upbeat_inverse) {
	unsigned int len2;
	int dotcount;

	NedResource::setLengthField(upbeat_inverse);
	len2 = NedResource::getPartLength(&dotcount);
	while (len2) {
		NedChordOrRest *rest = new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3, dotcount, len2, NORMAL_NOTE, 0, 0);
		m_chord_or_rests = g_list_append(m_chord_or_rests, rest);
		len2 = NedResource::getPartLength(&dotcount);
	}
}

void NedVoice::appendAppropriateWholes(int start_measure) {
	int meas_num = getSystem()->getMeasureCount();
	for (; start_measure < meas_num; start_measure++) {
		NedChordOrRest *rest = new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0);
		m_chord_or_rests = g_list_append(m_chord_or_rests, rest);
	}
}

int NedVoice::getPosition(NedChordOrRest *chord_or_rest) {
	return g_list_index(m_chord_or_rests, chord_or_rest);
}


NedVoice::~NedVoice() {
	GList *lptr;

	for(lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		// important: must be first
		delete ((NedBeam *) lptr->data);
	}
	g_list_free(m_beam_list);
	m_beam_list = NULL;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		delete ((NedChordOrRest *) lptr->data);
	}

	g_list_free(m_chord_or_rests);
	m_chord_or_rests = NULL;
}

int NedVoice::getNumElements() {
	return g_list_length(m_chord_or_rests);
}

void NedVoice::empty() {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		delete (NedChordOrRest *) lptr->data;
	}
	g_list_free(m_chord_or_rests);
	m_chord_or_rests = NULL;
}


NedMainWindow *NedVoice::getMainWindow() {return m_staff->getSystem()->getPage()->getMainWindow();}
NedPage *NedVoice::getPage() {return m_staff->getSystem()->getPage();}
NedSystem *NedVoice::getSystem() {return m_staff->getSystem();}

void NedVoice::draw(cairo_t *cr, bool *freetexts_or_lyrics_present) {
	GList *lptr;
	m_free_texts_or_lyrics_present = false;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->draw(cr, &m_free_texts_or_lyrics_present);
	}
	if (m_free_texts_or_lyrics_present) *freetexts_or_lyrics_present = true;
	for(lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		((NedBeam *) lptr->data)->draw(cr);
	}
	for(lptr = g_list_first(m_tuplet_list); lptr; lptr = g_list_next(lptr)) {
		((NedTuplet *) lptr->data)->draw(cr);
	}
	/*
	if (true || lyrics_present) {
		for (lptr = g_list_last(m_chord_or_rests); lptr; lptr = g_list_previous(lptr)) {
			((NedChordOrRest *) lptr->data)->drawlyrics(cr, last_left_bbox_edge);
			last_left_bbox_edge = ((NedChordOrRest *) lptr->data)->getLeftBboxEdge();
		}
	}
	*/
}

void NedVoice::drawTexts(cairo_t *cr, double scale) {
	double last_left_bbox_edge = -1.0;
	GList *lptr;
	bool last = true;
	if (!m_free_texts_or_lyrics_present) return;

	for (lptr = g_list_last(m_chord_or_rests); lptr; lptr = g_list_previous(lptr), last = false) {
		((NedChordOrRest *) lptr->data)->drawTexts(cr, scale, last_left_bbox_edge, last);
		last_left_bbox_edge = ((NedChordOrRest *) lptr->data)->getLeftBboxEdge();
	}
}

bool NedVoice::hasOnlyRests() {
	GList *lptr;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() != TYPE_REST) return false;
	}
	return true;
}

bool NedVoice::trySelect(double x, double y, bool only_free_placeables) {
	GList *lptr;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->trySelect(x, y, only_free_placeables)) {
			return TRUE;
		}
	}
	return FALSE;
}

NedChordOrRest *NedVoice::findNearestElement(double x, double y, double factor, double *mindist, double *ydist) {
	GList *lptr;
	double dist, yd;
	NedChordOrRest *nearestElement = NULL;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->isHidden()) continue;
		dist = ((NedChordOrRest *) lptr->data)->computeDistFrom(x, y, factor, &yd);
		if (dist < *mindist) {
			*mindist = dist;
			*ydist = yd < 0 ? -yd : yd;
			nearestElement = ((NedChordOrRest *) lptr->data);
		}
	}
	return nearestElement;
}

void NedVoice::appendElementsOfMeasureLength(part *part_ptr, unsigned int meas_duration, int meas_num) {
	GList *lptr;
	NedChordOrRest *chord_or_rest;
	NedBeam *beam;
	int pos = 0, dotcount;
	unsigned int len2;
	bool first = true;
	unsigned int duration = 0;

	if (part_ptr->eof[m_voice_nr]) {
		chord_or_rest = new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0);
		m_chord_or_rests = g_list_append(m_chord_or_rests, chord_or_rest);
		m_start_of_last_imported = g_list_find(m_chord_or_rests, chord_or_rest);
		return;
	}

	if (part_ptr->start_of_last_taken[m_voice_nr] == NULL) {
		lptr = g_list_first(part_ptr->voices[m_voice_nr]);
	}
	else {
		lptr = g_list_next(part_ptr->end_of_last_taken[m_voice_nr]);
	}
	if (lptr == NULL) {
		part_ptr->eof[m_voice_nr] = true;
		chord_or_rest = new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0);
		m_chord_or_rests = g_list_append(m_chord_or_rests, chord_or_rest);
		m_start_of_last_imported = g_list_find(m_chord_or_rests, chord_or_rest);
		return;
	}

	if (m_start_of_last_imported == NULL) {
		pos = 0;
	}
	else {
		if ((pos = g_list_position(m_chord_or_rests, m_end_of_last_imported)) < 0) {
			NedResource::Abort("NedVoice::appendElementsOfMeasureLength(1)");
		}
		pos++;
	}
	
	for (;duration < meas_duration && lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		m_chord_or_rests = g_list_insert(m_chord_or_rests, chord_or_rest, pos++);
		if (first) {
			first = false;
			if ((m_start_of_last_imported = g_list_nth(m_chord_or_rests, pos - 1)) == NULL) {
				NedResource::Abort("NedVoice::appendElementsOfMeasureLength(4)");
			}
			part_ptr->start_of_last_taken[m_voice_nr] = lptr;
		}
		part_ptr->end_of_last_taken[m_voice_nr] = lptr;
		if ((beam = chord_or_rest->getBeam()) != NULL) {
			delete beam; // the beam removes itself from voice and
			             // sets the beam pointer of all chords to NULL
		}
		chord_or_rest->setVoice(this);
		if ((chord_or_rest->getType() & TYPE_CHORD) != 0) {
			chord_or_rest->xPositNotes();
		}
		chord_or_rest->computeBbox();
		if ((chord_or_rest->getType() & (TYPE_CHORD | TYPE_REST)) != 0) {
			duration += chord_or_rest->getDuration(meas_duration);

		}
		chord_or_rest->computeBbox();
		chord_or_rest->reConfigure();
	}
	if (duration > meas_duration) {
		NedResource::DbgMsg(DBG_CRITICAL, "Overflow\n"); 
	}
	if (duration < meas_duration) {
		NedResource::setLengthField(meas_duration - duration);
		while ((len2 = NedResource::getPartLength(&dotcount)) > 0) {
			m_chord_or_rests = g_list_insert(m_chord_or_rests, new NedChordOrRest(this, TYPE_REST, len2 == WHOLE_NOTE && m_voice_nr > 0, 3,
					dotcount, len2, NORMAL_NOTE, 0, 0), pos++);
		}
	}
	if ((m_end_of_last_imported = g_list_nth(m_chord_or_rests, pos - 1)) == NULL) {
		NedResource::Abort("NedVoice::appendElementsOfMeasureLength(5)");
	}
	if (first) {
		m_start_of_last_imported = m_end_of_last_imported;
	}
}

void NedVoice::copyDataOfWholeVoice() {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		NedResource::m_main_clip_board = g_list_append(NedResource::m_main_clip_board, lptr->data);
	}
}

void NedVoice::zoomFreeReplaceables(double zoom, double scale) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->zoomFreeReplaceables(zoom, scale);
	}
}

void NedVoice::recomputeFreeReplaceables() {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->recomputeFreeReplaceables();
	}
}

void NedVoice::handleEmptyMeasures() {
	NedMeasure *measure = NULL;
	GList *lptr, *lptr2, *newmeasure = NULL, *newlist = NULL;
	bool only_rests = false;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->m_measure != measure) {
			if (only_rests) {
				newlist = g_list_append(newlist, new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3 /* dummy */, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0 /* dummy */));
			}
			else {
				for (lptr2 = g_list_first(newmeasure); lptr2; lptr2 = g_list_next(lptr2)) {
					newlist = g_list_append(newlist, lptr2->data);
				}
			}
			g_list_free(newmeasure);
			newmeasure = NULL;
			only_rests = true;
			measure = ((NedChordOrRest *) lptr->data)->m_measure;
		}
		if (((NedChordOrRest *) lptr->data)->getType() != TYPE_REST) only_rests = false;
		newmeasure = g_list_append(newmeasure, lptr->data);
	}
	if (only_rests) {
		newlist = g_list_append(newlist, new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3 /* dummy */, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0 /* dummy */));
	}
	else {
		for (lptr2 = g_list_first(newmeasure); lptr2; lptr2 = g_list_next(lptr2)) {
			newlist = g_list_append(newlist, lptr2->data);
		}
	}
	g_list_free(newmeasure);
	g_list_free(m_chord_or_rests);
	m_chord_or_rests = newlist;
	assignMidiTimes();
}
	

void NedVoice::removeLastImported() {
	GList *lptr;
	GList *dellist = NULL;
	NedResource::DbgMsg(DBG_TESTING, "removeLastImported\n");

	if (m_start_of_last_imported == NULL) {
		NedResource::DbgMsg(DBG_CRITICAL, "staff = %d, voi = %d\n", m_staff->getStaffNumber(), m_voice_nr); 
		NedResource::Abort("NedVoice::removeLastImported");
	}
	for (lptr = m_start_of_last_imported; lptr; lptr = g_list_next(lptr)) {
		dellist = g_list_append(dellist, lptr->data);
	}
	for (lptr = g_list_first(dellist); lptr; lptr = g_list_next(lptr)) {
		m_chord_or_rests = g_list_remove(m_chord_or_rests, lptr->data);
	}
	g_list_free(dellist);
}

bool NedVoice::handleImportedTuplets() {
	GList *lptr;
	unsigned int  duration = 0;
	NedChordOrRest *chord_or_rest, *last_tupletted_chord_or_rest = NULL;
	int tupletval = -1, tupval;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		tupval = chord_or_rest->getTupletVal();
		if (tupval != 0) {
			duration += chord_or_rest->getDuration();
			if (tupval != 0) {
				if (tupletval < 0) {
					tupletval = tupval;
				}
				else if (tupletval != tupval) {
					return false;
				}
				if (NedResource::fittingDuration(duration)) {
					chord_or_rest->setLastTupletFlag(true);
					duration = 0;
					tupletval = -1;
				}
				last_tupletted_chord_or_rest = chord_or_rest;
			}
		}
		else if (tupletval > 0) {
			if (!NedResource::fittingDuration(duration)) {NedResource::DbgMsg(DBG_TESTING, "Stelle 1: tupletval = %d, duration = %u NOTE_4 = %u\n", tupletval, duration, NOTE_4); return false;}
			last_tupletted_chord_or_rest->setLastTupletFlag(true);
			tupletval = -1;
			duration = 0;
			last_tupletted_chord_or_rest = NULL;
		}
	}
	if (tupletval > 0) {
		if (!NedResource::fittingDuration(duration)) {NedResource::DbgMsg(DBG_TESTING, "Stelle 2: duration = %u NOTE_4 = %u\n", duration, NOTE_4); return false;}
		last_tupletted_chord_or_rest->setLastTupletFlag(true);
	}
	return true;
}

bool NedVoice::truncateAtStart(NedCommandList *command_list, unsigned long long midi_time) {
	unsigned int duration = 0;
	GList *list_of_elems_to_delete = NULL;
	NedDeleteChordOrRestGroupCommand *del_cmd;
	GList *lptr;
	NedChordOrRest *element;
	int pos;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		if ((element->getType() & (TYPE_CHORD | TYPE_REST)) == 0) continue;
		if (element->getTupletVal() != 0) return false;
		duration += element->getDuration();
		list_of_elems_to_delete = g_list_append(list_of_elems_to_delete, element);
		if (duration >= midi_time) {
			element->testForTiesToDelete(command_list, BREAK_TIE_FORWARD);
			del_cmd = new NedDeleteChordOrRestGroupCommand(this, list_of_elems_to_delete);
			command_list->addCommand(del_cmd);
			pos = 0;
			if (duration > midi_time) {
				handleGapAfterInsertion(command_list, &pos, duration - midi_time, element, 0, false, false);
			}

			return true;
		}
	}
	return false;
}

void NedVoice::determineTempoInverse(NedChordOrRest *element, unsigned long long till, double *tempoinverse, bool *found) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data) == element) {
			*found = true;
			return;
		}
		if (element && ((NedChordOrRest *) lptr->data)->getMidiTime() >= till) {
			return;
		}
		((NedChordOrRest *) lptr->data)->determineTempoInverse(tempoinverse);
	}
}

void NedVoice::handleStaffElements() {
	GList *lptr, *lptr2;
	GList *del_objs = NULL;
	NedChordOrRest *chord_or_rest;

	assignMidiTimes();

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		switch (chord_or_rest->getType()) {
			case TYPE_CLEF:
			case TYPE_STAFF_SIGN:
			case TYPE_KEYSIG: m_staff->insertStaffElement(chord_or_rest, false);
					  //m_chord_or_rests = g_list_delete_link(m_chord_or_rests, lptr);
					  //changed = true;
					  del_objs = g_list_append(del_objs, chord_or_rest);
					  break;
		}
	}

	for (lptr = g_list_first(del_objs); lptr; lptr = g_list_next(lptr)) {
		if ((lptr2 = g_list_find(m_chord_or_rests, lptr->data)) == NULL) {
			NedResource::Abort("NedVoice::handleStaffElements");
		}
		m_chord_or_rests = g_list_delete_link(m_chord_or_rests, lptr2);
	}

	g_list_free(del_objs);
}

bool NedVoice::tryInsertOrErease(double x, double y, int num_midi_input_notes, int *midi_input_chord /* given if insert from midikeyboard */,  NedChordOrRest **newObj, bool force_rest) {
	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	int i, npitch;
	GList *lptr, *lptr2;
	NedChordOrRest *chord;
	NedChordOrRest *last_tupleted_chord_or_rest;
	NedTuplet *tuplet_ptr;
	bool removed = FALSE;
	staff_context_str *context;
	int tuplet_val, tuplet_val_raw;
	int line = 0;
	NedChordOrRest *chord_or_rest = NULL;
	unsigned long long end_time;
	unsigned long long new_chords_midi_end_time;
	char offs_array[115];

	if (NedResource::getCurrentLength() < NOTE_64 && getMainWindow()->getSpecialType() < 0) {
		return insertGrace(x, y, midi_input_chord != NULL ? *midi_input_chord : -1);
	}

	if (midi_input_chord == NULL) { // line given
		for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
			chord = (NedChordOrRest *) lptr->data;
			if (chord->tryErease(x, y, &removed)) {
				NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
				chord->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
				command_list->addCommand(new NedDeleteChordCommand(chord));
				command_list->execute();
				getMainWindow()->getCommandHistory()->addCommandList(command_list);
				return TRUE;
			}
			if (removed) {
				return TRUE;
			}
		}
	}

	double current_scale = getMainWindow()->getCurrentScale();
	if (midi_input_chord == NULL) { // line given
		double yl = Y_POS_INVERS_STAFF_REL(y);
		if (MAX_OVER_UNDER_LINES * LINE_DIST/2.0 < yl ||  -((MAX_OVER_UNDER_LINES + 8) * LINE_DIST/2.0) > yl) return FALSE;
		double yoffs = (yl >= 0.0) ? LINE_DIST / 4.0 : -LINE_DIST / 4.0;
		line = (int) ((- yl - yoffs) / (LINE_DIST/2.0));
	}
	double leftx = getMainWindow()->getLeftX();
	double xl = X_POS_INVERS_PAGE_REL(x);
	double min = 1000000.0;
	double diff;
	unsigned int v, sv, state;
	int minpos = -1;
	unsigned long long offs;
	int half_offset;
	int num_elements_to_delete = 1;
	int clef = TREBLE_CLEF, keysig_so_far, octave_shift;
	bool greater_than_last;
	GList *min_pos_ptr = NULL;
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		diff = ((NedChordOrRest *) lptr->data)->getXPos() - xl;
		if (diff < 0) diff = -diff;
		if (diff < min) {
			min = diff;
			min_pos_ptr = lptr;
			minpos = g_list_position(m_chord_or_rests, lptr);
		}
	}
	if (minpos < 0) {
		return FALSE;
	}
	state = NedResource::getStatus();

	if (midi_input_chord != NULL) { // pitch given
		m_staff->getCurrentClefAndKeysig(((NedChordOrRest *) min_pos_ptr->data)->getMidiTime(),
				&clef, &keysig_so_far, &octave_shift);
		line = NedNote::pitchToLine(*midi_input_chord, clef, keysig_so_far, octave_shift, &half_offset);
		memset(offs_array, 0, sizeof(offs_array));
		findAccidentals(offs_array, ((NedChordOrRest *) min_pos_ptr->data)->m_measure, ((NedChordOrRest *) min_pos_ptr->data)->getMidiTime(), false);
		state &= (~(ACCIDENTAL_MASK));
		state |= (NedNote::determineState(*midi_input_chord, line, half_offset, offs_array, keysig_so_far) &  ACCIDENTAL_MASK);


	}
	else {
		state = NedResource::getStatus();
	}

	tuplet_val = ((NedChordOrRest *) min_pos_ptr->data)->getTupletVal();
	tuplet_val_raw = ((NedChordOrRest *) min_pos_ptr->data)->getTupletValRaw();
	if (getMainWindow()->getSpecialType() < 0 && !((NedChordOrRest *) min_pos_ptr->data)->isRest() && !(NedResource::getRestMode() || force_rest)) {
		if (((NedChordOrRest *) min_pos_ptr->data)->noConflict(line)) {
			NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
			NedAddNoteAtCommand *add_note_at_command = new NedAddNoteAtCommand(this, ((NedChordOrRest *) min_pos_ptr->data), line, NedResource::getCurrentNoteHead(), state);
			command_list->addCommand(add_note_at_command);
			getMainWindow()->getCommandHistory()->addCommandList(command_list);
			getMainWindow()->setSelected((NedChordOrRest *) min_pos_ptr->data, add_note_at_command->getNote());
			command_list->execute();
			return TRUE;
		}
		return FALSE;
	}
/*
	if (((NedChordOrRest *) min_pos_ptr->data)->isRest() && (((NedChordOrRest *) min_pos_ptr->data)->isHidden())) {
		return FALSE;
	}
*/
	NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
	if (midi_input_chord == NULL && getMainWindow()->getSpecialType() >= 0) {
		for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
			if (((NedChordOrRest *) lptr->data)->getXPos() > xl) break;
		}
		if (lptr) {
			greater_than_last = false;
			if (lptr == g_list_first(m_chord_or_rests)) { // avoid inserting clefs and keys at begin of staff
				getMainWindow()->resetSpecialType();
				return TRUE;
			}
		}
		else { // behind last element
			lptr = g_list_last(m_chord_or_rests);
			greater_than_last = true;
		}
		switch (getMainWindow()->getSpecialType() & 0xffff) {
			case TYPE_CLEF:
				getStaff()->getCurrentClefAndKeysig(((NedChordOrRest *) lptr->data)->getMidiTime(), &clef, &keysig_so_far, &octave_shift);
				v = getMainWindow()->getSpecialSubTypeInt();
				sv = (v >> 16) & 0xffff;
				sv -= 100;
				chord_or_rest = new NedChordOrRest(this, getMainWindow()->getSpecialType() & 0xffff, v & 0xffff, 
					sv, greater_than_last ? ((NedChordOrRest *) lptr->data)->getMidiTime() + ((NedChordOrRest *) lptr->data)->getDuration() :
						((NedChordOrRest *) lptr->data)->getMidiTime(), true);
				break;
			case TYPE_KEYSIG:
				getStaff()->getCurrentClefAndKeysig(((NedChordOrRest *) lptr->data)->getMidiTime(), &clef, &keysig_so_far, &octave_shift);
				if (keysig_so_far == getMainWindow()->getSpecialSubTypeInt()) {
					getMainWindow()->resetSpecialType();
					return TRUE;
				}
				chord_or_rest = new NedChordOrRest(this, getMainWindow()->getSpecialType() & 0xffff, getMainWindow()->getSpecialSubTypeInt(), 
						keysig_so_far, greater_than_last ? ((NedChordOrRest *) lptr->data)->getMidiTime() + ((NedChordOrRest *) lptr->data)->getDuration() :
						((NedChordOrRest *) lptr->data)->getMidiTime());
				break;
			case TYPE_STAFF_SIGN:
				chord_or_rest = new NedChordOrRest(this, getMainWindow()->getSpecialType() & 0xffff, getMainWindow()->getSpecialSubTypeInt(), 
						greater_than_last ? ((NedChordOrRest *) lptr->data)->getMidiTime() + ((NedChordOrRest *) lptr->data)->getDuration() :
						((NedChordOrRest *) lptr->data)->getMidiTime());
				break;
		}
		NedInsertStaffElemCommand *ins_staff_elem_cmd = new NedInsertStaffElemCommand(chord_or_rest, clef, octave_shift, m_staff,
					getMainWindow()->getDoAdjust());
		getMainWindow()->resetSpecialType();
		ins_staff_elem_cmd->execute();
		command_list->setFullReposit();
		command_list->addCommand(ins_staff_elem_cmd);
		getMainWindow()->setAllUnpositioned();
		getMainWindow()->getCommandHistory()->addCommandList(command_list);
		getMainWindow()->reposit(command_list);
		return TRUE;
	}
	else {
		chord_or_rest = new NedChordOrRest(this, (midi_input_chord == NULL && (NedResource::getRestMode() || force_rest))  ? TYPE_REST : TYPE_CHORD, FALSE, line,
		NedResource::getDotCount(), NedResource::getCurrentLength(), NedResource::getCurrentNoteHead(), state, 0);
		chord_or_rest->setTupletVal(tuplet_val);
		if (newObj != NULL) {
			*newObj = chord_or_rest;
		}
		if (midi_input_chord != NULL && num_midi_input_notes > 1) {
			for (i = 1; i < num_midi_input_notes; i++) {
				state = NedResource::getStatus();
				state &= (~(ACCIDENTAL_MASK));
				line = NedNote::pitchToLine(midi_input_chord[i], clef, keysig_so_far, octave_shift, &half_offset);
				state |= (NedNote::determineState(midi_input_chord[i], line, half_offset, offs_array, keysig_so_far) &  ACCIDENTAL_MASK);
				NedNote *note = new NedNote(chord_or_rest, line, NedResource::getCurrentNoteHead(), state);
				chord_or_rest->insertNoteAt(note);

			}
			chord_or_rest->xPositNotes();
			chord_or_rest->computeBbox();
		}
	}
	if (!chord_or_rest->isRest()) {
		getMainWindow()->setSelected(chord_or_rest, chord_or_rest->getFirstNote());
	}
	unsigned long long min_pos_midi_time = ((NedChordOrRest *) min_pos_ptr->data)->getMidiTime();
	if ((tuplet_ptr = ((NedChordOrRest *) min_pos_ptr->data)->getTupletPtr()) != NULL)  {
		end_time = tuplet_ptr->getEndTime();
	}
	else {
		//NedMeasure *measure = getSystem()->getMeasure(((NedChordOrRest *) min_pos_ptr->data)->getMidiTime());
		NedMeasure *measure = ((NedChordOrRest *) min_pos_ptr->data)->m_measure;
		end_time = measure->midi_end;
		testForTupletEnd(min_pos_midi_time, chord_or_rest->getDuration(), &end_time);
	}
	new_chords_midi_end_time = min_pos_midi_time + chord_or_rest->getDuration();
	if (end_time > new_chords_midi_end_time) end_time = new_chords_midi_end_time;
	unsigned long long sum_of_chords_and_rests_to_delete = 0;
	GList *chords_and_rests_to_delete = NULL;
	GList *already_deleted_free_repleceables = NULL;
	for (lptr2 = min_pos_ptr; lptr2 && ((NedChordOrRest *) lptr2->data)->getMidiTime() < end_time;
		lptr2 = g_list_next(lptr2)) {
		chords_and_rests_to_delete = g_list_append(chords_and_rests_to_delete, lptr2->data);
		if (g_list_find(already_deleted_free_repleceables, lptr2->data) == NULL) {
			((NedChordOrRest *) lptr2->data)->testForFreeReplaceablesToDelete(command_list, &already_deleted_free_repleceables);
		}
		((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
		sum_of_chords_and_rests_to_delete += ((NedChordOrRest *) lptr2->data)->getDuration();
	}
	g_list_free(already_deleted_free_repleceables);
	if (sum_of_chords_and_rests_to_delete < chord_or_rest->getDuration()) {
		chord_or_rest->changeDuration(sum_of_chords_and_rests_to_delete, tuplet_val);	
	}
	if (tuplet_ptr != NULL) {
		if (tuplet_ptr->getMidiStartTime() == min_pos_midi_time && chord_or_rest->getDuration() == tuplet_ptr->getDuration()) {
			delete command_list;
			return FALSE;
		}
	}
	sum_of_chords_and_rests_to_delete -= chord_or_rest->getDuration();
	if (chords_and_rests_to_delete != NULL) {
		command_list->addCommand(new NedDeleteChordOrRestGroupCommand(this, chords_and_rests_to_delete));
		num_elements_to_delete = g_list_length(chords_and_rests_to_delete);
	}
	command_list->addCommand(new NedInsertChordOrRestCommand(this, minpos++, chord_or_rest));
	last_tupleted_chord_or_rest = chord_or_rest;
	offs = ((NedChordOrRest *) min_pos_ptr->data)->getMidiTime() - (((NedChordOrRest *) min_pos_ptr->data)->m_measure)->midi_start +
		chord_or_rest->getDuration();
	if  (sum_of_chords_and_rests_to_delete > 0) {
		NedChordOrRest *chord_or_rest_to_distribute = (NedChordOrRest *) (g_list_last(chords_and_rests_to_delete)->data);
		last_tupleted_chord_or_rest = handleGapAfterInsertion2(command_list, offs, &minpos, sum_of_chords_and_rests_to_delete, chord_or_rest_to_distribute, tuplet_val, false, false);
	}
	if (tuplet_val_raw & LAST_TUPLET_FLAG) {
		last_tupleted_chord_or_rest->setLastTupletFlag(TRUE);
	}
	getMainWindow()->getCommandHistory()->addCommandList(command_list);
	command_list->execute();
	getMainWindow()->reposit(command_list, getPage(), getSystem());
	context = &(getMainWindow()->m_staff_contexts[getStaff()->getStaffNumber()]);
	if (midi_input_chord == NULL /* otherwise done in main */ && !chord_or_rest->isRest() && NedResource::m_midi_echo) {
		npitch = chord_or_rest->getPitchOfFirstNote();
		NedResource::playImmediately(context->m_midi_channel, context->voices[0].m_midi_program, 1, &npitch, context->voices[0].m_midi_volume);
	}
	return TRUE;
}


bool NedVoice::insertGrace(double x, double y, int pitch /* given if insert from midikeyboard */) {
	GList *lptr;
	NedChordOrRest *chord_or_rest;
	int line = 0;
	int clef = TREBLE_CLEF, keysig_so_far, octave_shift;
	int half_offset;

	double topy = getMainWindow()->getTopY();
	double zoom_factor = getMainWindow()->getCurrentZoomFactor();
	double current_scale = getMainWindow()->getCurrentScale();
	if (pitch < 0) { // line given
		double yl = Y_POS_INVERS_STAFF_REL(y);
		if (MAX_OVER_UNDER_LINES * LINE_DIST/2.0 < yl ||  -((MAX_OVER_UNDER_LINES + 8) * LINE_DIST/2.0) > yl) return FALSE;
		line = (int) ((- yl) / (LINE_DIST/2.0) + 0.5);
	}
	double leftx = getMainWindow()->getLeftX();
	double xl = X_POS_INVERS_PAGE_REL(x);
	int minpos = -1;
	GList *min_pos_ptr = NULL;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getXPos() < xl) continue;
		min_pos_ptr = lptr;
		minpos = g_list_position(m_chord_or_rests, lptr);
		break;
	}
	if (minpos < 0) {
		return FALSE;
	}
	if (pitch >= 0) { // pitch given
		m_staff->getCurrentClefAndKeysig(((NedChordOrRest *) min_pos_ptr->data)->getMidiTime(), &clef, &keysig_so_far, &octave_shift);
		line = NedNote::pitchToLine(pitch, clef, keysig_so_far, octave_shift, &half_offset);
	}

	chord_or_rest = new NedChordOrRest(this, TYPE_CHORD, FALSE, line,
		0, NedResource::getCurrentLength(), NORMAL_NOTE, 0, 0);
	NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
	command_list->addCommand(new NedInsertChordOrRestCommand(this, minpos, chord_or_rest));
	getMainWindow()->getCommandHistory()->addCommandList(command_list);
	getSystem()->m_is_positioned = false;
	command_list->execute();
	getMainWindow()->reposit(command_list, getPage(), getSystem());
	getSystem()->m_is_positioned = false;
	return TRUE;
}


void NedVoice::collectSelectionRectangleElements(double xp, double yp, NedBbox *sel_rect, GList **sel_group,
		bool is_first_selected, bool is_last_selected) {
	GList *lptr;
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (is_first_selected && ((NedChordOrRest *) lptr->data)->getXPos() < xp) continue;
		if (is_last_selected && ((NedChordOrRest *) lptr->data)->getXPos() > xp + sel_rect->width) continue;
		*sel_group = g_list_append(*sel_group, lptr->data);
	}
}

bool NedVoice::hasTupletConflict(unsigned int meas_duration, GList **elements, int from_staff, unsigned int long long midi_time) {
	GList *lptr, *lptr2;
	GList *tested_elements = NULL;
	NedChordOrRest *element;
	NedTuplet *tuplet;
	unsigned long long current_midi_time;

	for (lptr2 = g_list_first(m_chord_or_rests); lptr2; lptr2 = g_list_next(lptr2)) {
		if (((NedChordOrRest *) lptr2->data)->getMidiTime() >= midi_time) {
			break;
		}
	}
	if (lptr2 == NULL) return FALSE;
	current_midi_time = ((NedChordOrRest *) lptr2->data)->getMidiTime();
	for (lptr = g_list_first(*elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		if (element->getStaff()->getStaffNumber() != from_staff) continue;
		if (element->getVoice()->m_voice_nr != m_voice_nr) continue;
		tested_elements = g_list_append(tested_elements, element);
		current_midi_time += element->getDuration();
		if (element->getTupletVal() != 0) {
			if ((tuplet = element->getTupletPtr()) == NULL) {
				NedResource::Abort("NedVoice::hasTupletConflict(1)");
			}
			if (element->hasLastTupletFlag()) {
				/* -1 is very tricky but if it ends exactly at next measure it still belongs to this measure */
				if ((current_midi_time - tuplet->getDuration()) / meas_duration != (current_midi_time - 1) / meas_duration) {
					/*
					NedResource::DbgMsg(DBG_TESTING, "current_midi_time = %llu, tuplet->getDuration() = %u, diff = %llu, meas_duration = %u, current_midi_time = %llu, d1 = %llu, f2 = %llu\n",
					current_midi_time, tuplet->getDuration(), current_midi_time - tuplet->getDuration(), meas_duration, current_midi_time,
					(current_midi_time - tuplet->getDuration()) / meas_duration,  current_midi_time / meas_duration);
					*/
					return TRUE;
				}
			}
		}
	}
	for (lptr = g_list_first(tested_elements); lptr; lptr = g_list_next(lptr)) {
		if ((lptr2 = g_list_find(*elements, lptr->data)) == NULL) {
			NedResource::Abort("NedVoice::hasTupletConflict(2)");
		}
		*elements = g_list_delete_link(*elements, lptr2);
	}
	g_list_free(tested_elements);
	return FALSE;
}

void NedVoice::pasteElements(NedCommandList *command_list, GList **elements, int from_staff, unsigned int long long midi_time) {
	GList *lptr, *lptr2;
	GList *copied_elements = NULL;
	GList *chordsorrests_to_delete = NULL;
	GList *chordsorrests_to_insert = NULL;
	NedChordOrRest *element;
	int idx, diff, pos;
	unsigned long long duration = 0, deleted_duration;
	unsigned long long current_midi_time;
	unsigned int last_duration = 0, first_duration = -1;

	for (lptr = g_list_first(*elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		if (element->getStaff()->getStaffNumber() != from_staff) continue;
		if (element->getVoice()->m_voice_nr != m_voice_nr) continue;
		last_duration = element->getDuration();
		if (first_duration < 0) {
			first_duration =  element->getDuration();
		}
		duration += last_duration;
	}
	do {
		for (lptr2 = g_list_first(m_chord_or_rests); lptr2; lptr2 = g_list_next(lptr2)) {
			if (((NedChordOrRest *) lptr2->data)->getMidiTime() >= midi_time) {
				break;
			}
		}
		if (lptr2 == NULL || duration == 0) return;
		if ((idx = g_list_position(m_chord_or_rests, lptr2)) < 0) {
			NedResource::Abort("NedVoice::pasteElements(1)");
		}
		element = (NedChordOrRest *) lptr2->data;
	}
	while (searchForBreakableTuplet(command_list, element->getMidiTime()));
	searchForBreakableTies(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD, element->getMidiTime(), element->getMidiTime() + element->getDuration());
	current_midi_time = ((NedChordOrRest *) lptr2->data)->getMidiTime();
	searchForBreakableTuplet(command_list, current_midi_time + duration);
	searchForBreakableTies(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD, current_midi_time + duration - last_duration, current_midi_time + duration);
	assignMidiTimes(); // recompute midi times
	for (lptr2 = g_list_first(m_chord_or_rests); lptr2; lptr2 = g_list_next(lptr2)) { // test again with removed tuplets
		if (((NedChordOrRest *) lptr2->data)->getMidiTime() >= midi_time) {
			if (((NedChordOrRest *) lptr2->data)->getMidiTime() > midi_time) {
				if ((lptr2 = g_list_previous(lptr2)) == NULL) {
					NedResource::Abort("NedVoice::pasteElements(2)");
				}
				if ((pos = g_list_position(m_chord_or_rests, lptr2)) < 0) {
					NedResource::Abort("NedVoice::pasteElements(3)");
				}
				NedChordOrRest *chord_or_rest = ((NedChordOrRest *) lptr2->data);
				int duration = chord_or_rest->getDuration();
				unsigned long long start_midi_time = chord_or_rest->getMidiTime();
				GList *list = NULL;
				list = g_list_append(list, chord_or_rest);
				NedDeleteChordOrRestGroupCommand *delete_chord_command = new NedDeleteChordOrRestGroupCommand(this, list);
				delete_chord_command->execute();
				int dur1 = midi_time - start_midi_time;
				command_list->addCommand(delete_chord_command);
				handleGapAfterInsertion(command_list, &pos, dur1, chord_or_rest, 0 , false, true);
				int dur2 = duration - dur1;
				handleGapAfterInsertion(command_list, &pos, dur2, NULL, 0, false, true);
				command_list->setBreak();
				idx = pos;
				if ((lptr2 = g_list_nth(m_chord_or_rests, idx)) == NULL) {
					NedResource::Abort("NedVoice::pasteElements(4)");
				}
				assignMidiTimes(); // recompute midi times
				current_midi_time = ((NedChordOrRest *) lptr2->data)->getMidiTime();
			}
			break;
		}
	}
	deleted_duration = 0;
	while (deleted_duration < duration && lptr2) {
		deleted_duration += ((NedChordOrRest *) lptr2->data)->getDuration();
		chordsorrests_to_delete = g_list_append(chordsorrests_to_delete, lptr2->data);
		lptr2 = g_list_next(lptr2);
	}
	diff = deleted_duration - duration;
	for (lptr = g_list_first(*elements); deleted_duration > 0 && lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		if (element->getStaff()->getStaffNumber() != from_staff) continue;
		if (element->getVoice()->m_voice_nr != m_voice_nr) continue;
		unsigned long long midi_end_time = current_midi_time + element->getDuration();
		NedMeasure *measure = getSystem()->getMeasure(current_midi_time);
		if (current_midi_time < measure->midi_end && midi_end_time > measure->midi_end) {
			unsigned long long t1, t2;
			t1 = measure->midi_end - current_midi_time; t2 = midi_end_time - measure->midi_end;
			NedResource::split_element(elements, lptr, t1, t2);
			current_midi_time += t1;
		}
		else {
			current_midi_time = midi_end_time;
		}
		deleted_duration -= element->getDuration();
		chordsorrests_to_insert = g_list_append(chordsorrests_to_insert, element);
		copied_elements = g_list_append(copied_elements, element);
	}

	NedDeleteChordOrRestGroupCommand *delete_command = new NedDeleteChordOrRestGroupCommand(this, chordsorrests_to_delete);
	delete_command->execute();
	command_list->addCommand(delete_command);
	NedInsertChordOrRestGroupCommand *insert_command = new NedInsertChordOrRestGroupCommand(this, chordsorrests_to_insert, idx);
	insert_command->execute();
	command_list->addCommandAndSetBreak(insert_command);
	if (diff > 0) {
		NedChordOrRest *chord_or_rest_to_distribute = (NedChordOrRest *) g_list_last(chordsorrests_to_delete)->data;
		if ((pos = g_list_index(m_chord_or_rests, g_list_last(chordsorrests_to_insert)->data)) < 0) {
			NedResource::Abort("NedVoice::pasteElements(8)");
		}
		pos++;
		handleGapAfterInsertion(command_list, &pos, diff, chord_or_rest_to_distribute, 0, false, true);
	}

	for (lptr = g_list_first(copied_elements); lptr; lptr = g_list_next(lptr)) {
		if ((lptr2 = g_list_find(*elements, lptr->data)) == NULL) {
			NedResource::Abort("NedVoice::pasteElements(9)");
		}
		*elements = g_list_delete_link(*elements, lptr2);
	}
	g_list_free(copied_elements);
	assignMidiTimes();
}

bool NedVoice::findFromTo(GList *clipboard) {
	GList *lptr;
	NedSystem *the_system = getSystem();
	NedPage *the_page = getPage();

	for (lptr = g_list_first(clipboard); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getPage() != the_page) continue;
		if (((NedChordOrRest *) lptr->data)->getSystem() != the_system) continue;
		if (((NedChordOrRest *) lptr->data)->getStaff() != m_staff) continue;
		if (((NedChordOrRest *) lptr->data)->getVoice() != this) continue;
		return TRUE;
	}
	return FALSE;
}

bool NedVoice::findStartMeasureLimits(GList *clipboard, unsigned long long *start_midi) {
	GList *start_ptr;
	NedChordOrRest *element;
	NedSystem *the_system = getSystem();
	NedPage *the_page = getPage();
	NedMeasure *measure;

	for (start_ptr = g_list_first(clipboard); start_ptr; start_ptr = g_list_next(start_ptr)) {
		element = (NedChordOrRest *) start_ptr->data;
		if (element->isRest() && element->isHidden()) continue;
		if (element->getPage() != the_page) continue;
		if (element->getSystem() != the_system) continue;
		if (element->getStaff() != m_staff) continue;
		if (element->getVoice() != this) continue;
		//measure = getSystem()->getMeasure(element->getMidiTime());
		measure = element->m_measure;
		if (element->getMidiTime() != measure->midi_start) continue;
		if (*start_midi > element->getMidiTime()) {
			*start_midi = element->getMidiTime();
			return TRUE;
		}
	}
	return FALSE;
}

bool NedVoice::findEndMeasureLimits(GList *clipboard, unsigned long long *end_midi) {
	GList *end_ptr = NULL;
	NedChordOrRest *element;
	NedSystem *the_system = getSystem();
	NedPage *the_page = getPage();
	NedMeasure *measure;

	for (end_ptr = g_list_last(clipboard); end_ptr; end_ptr = g_list_previous(end_ptr)) {
		element = (NedChordOrRest *) end_ptr->data;
		if (element->isRest() && element->isHidden()) continue;
		if (element->getPage() != the_page) continue;
		if (element->getSystem() != the_system) continue;
		if (element->getStaff() != m_staff) continue;
		if (element->getVoice() != this) continue;
		//measure = getSystem()->getMeasure(element->getMidiTime());
		measure = element->m_measure;
		if (element->getMidiTime() + element->getDuration() != measure->midi_end) continue;
		if (*end_midi < measure->midi_end) {
			*end_midi = measure->midi_end;
			return TRUE;
		}
	}
	return FALSE;
}

void NedVoice::deleteItemsFromTo(NedCommandList *command_list, bool is_first, bool is_last, unsigned long long start_midi, unsigned long long end_midi) {
	GList *elements_to_delete = NULL;
	GList *start_ptr, *end_ptr = NULL;
	NedChordOrRest *element;

	start_ptr = g_list_first(m_chord_or_rests);

	if (is_first) {
		for (; start_ptr; start_ptr = g_list_next(start_ptr)) {
			if (((NedChordOrRest *) start_ptr->data)->getMidiTime() < start_midi) continue;
			break;
		}
	}
	if (start_ptr == NULL) return;
	if (is_last) {
		for (end_ptr = g_list_last(m_chord_or_rests); end_ptr; end_ptr = g_list_previous(end_ptr)) {
			element = (NedChordOrRest *) end_ptr->data;
			if (element->getMidiTime() + element->getDuration() > end_midi) continue;
			break;
		}
	}
	if (end_ptr == NULL) return;
	for (; start_ptr; start_ptr = g_list_next(start_ptr)) {
		elements_to_delete = g_list_append(elements_to_delete, start_ptr->data);
		if (start_ptr == end_ptr) break;
	}
	if (g_list_length(elements_to_delete) > 0) {
		((NedChordOrRest *) g_list_first(elements_to_delete)->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, true);
		((NedChordOrRest *) g_list_last(elements_to_delete)->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD, true);
		NedDeleteChordOrRestGroupCommand *delete_cmd = new NedDeleteChordOrRestGroupCommand(this, elements_to_delete);
		delete_cmd->execute();
		command_list->addCommand(delete_cmd);
	}
}

void NedVoice::removeNotesFromTo(NedCommandList *command_list, GList *items, bool is_first, bool is_last) {
	GList *elements_to_delete = NULL;
	GList *all_elements_to_delete = NULL;
	GList *start_ptr, *end_ptr = NULL;
	NedChordOrRest *element, *start_element;
	NedSystem *the_system = getSystem();
	NedPage *the_page = getPage();
	int pos;
	unsigned int duration;
	NedDeleteChordOrRestGroupCommand *command;
	NedMeasure *measure = NULL;
	bool only_rests = true;
	bool measure_start_seen = false;

	start_ptr = g_list_first(items);

	if (is_first) {
		for (start_ptr = g_list_first(items); start_ptr; start_ptr = g_list_next(start_ptr)) {
			element = (NedChordOrRest *) start_ptr->data;
			if (element->isRest() && element->isHidden()) continue;
			if (element->getPage() != the_page) continue;
			if (element->getSystem() != the_system) continue;
			if (element->getStaff() != m_staff) continue;
			if (element->getVoice() != this) continue;
			break;
		}
	}
	if (start_ptr == NULL) return;
	if (is_last) {
		for (end_ptr = g_list_last(items); end_ptr; end_ptr = g_list_previous(end_ptr)) {
			element = (NedChordOrRest *) end_ptr->data;
			if (element->isRest() && element->isHidden()) continue;
			if (element->getPage() != the_page) continue;
			if (element->getSystem() != the_system) continue;
			if (element->getStaff() != m_staff) continue;
			if (element->getVoice() != this) continue;
			break;
		}
		if (end_ptr == NULL) return;
	}
	duration = 0;
	/*
	//measure = getSystem()->getMeasure(((NedChordOrRest *) start_ptr->data)->getMidiTime());
	measure = ((NedChordOrRest *) start_ptr->data)->m_measure;
	if (((NedChordOrRest *) start_ptr->data)->getMidiTime() == measure->midi_start) {
		measure_start_seen = true;
	}
	*/
	for (;start_ptr; start_ptr = g_list_next(start_ptr)) {
		element = (NedChordOrRest *) start_ptr->data;
		if (element->isRest() && element->isHidden()) continue;
		if (element->getPage() != the_page) continue;
		if (element->getSystem() != the_system) continue;
		if (element->getStaff() != m_staff) continue;
		if (element->getVoice() != this) continue;
		if (!element->isRest())  only_rests = false;
		//measure = getSystem()->getMeasure(element->getMidiTime());
		measure = element->m_measure;
		if (element->getMidiTime() == measure->midi_start) {
			if (g_list_length(elements_to_delete) > 0) {
				if ((pos = g_list_index(m_chord_or_rests, g_list_first(elements_to_delete)->data)) < 0) {
					NedResource::Abort("NedVoice::removeNotesFromTo(1)");
				}
				command = new NedDeleteChordOrRestGroupCommand(this, elements_to_delete);
				command->execute();
				command_list->addCommand(command);
				handleGapAfterInsertion(command_list, &pos, duration, NULL, 0, measure_start_seen, true);
			}
			elements_to_delete = NULL;
			duration = 0;
			measure_start_seen = true;
		}
		duration += element->getDuration();
		elements_to_delete = g_list_append(elements_to_delete, element);
		all_elements_to_delete = g_list_append(all_elements_to_delete, element);
		if (start_ptr == end_ptr) break;
	}
	if (g_list_length(elements_to_delete) > 0) {
		if ((pos = g_list_index(m_chord_or_rests, g_list_first(elements_to_delete)->data)) < 0) {
			NedResource::Abort("NedVoice::removeNotesFromTo(2)");
		}
		start_element = (NedChordOrRest *) g_list_first(elements_to_delete)->data;
		element = (NedChordOrRest *) g_list_last(elements_to_delete)->data;
		measure_start_seen = element->getMidiTime() + element->getDuration() == measure->midi_end &&
					start_element->getMidiTime() == measure->midi_start;
		command = new NedDeleteChordOrRestGroupCommand(this, elements_to_delete);
		command->execute();
		command_list->addCommand(command);
		handleGapAfterInsertion(command_list, &pos, duration, NULL, 0,  measure_start_seen, true);
	}
	if (g_list_length(elements_to_delete) > 0) {
		((NedChordOrRest *) g_list_first(all_elements_to_delete)->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, true);
		((NedChordOrRest *) g_list_last(all_elements_to_delete)->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD, true);
	}
	g_list_free(all_elements_to_delete);
}

void NedVoice::insertBlocks(NedCommandList *command_list, int blockcount, unsigned long long midi_time) {
	int i;
	GList *lptr;
	GList *items_to_insert = NULL;
	int pos;

	for (lptr = g_list_first(m_chord_or_rests); lptr && ((NedChordOrRest *) lptr->data)->getMidiTime() < midi_time; lptr = g_list_next(lptr));
	if (lptr) {
		if ((pos = g_list_position(m_chord_or_rests, lptr)) < 0) {
			NedResource::Abort("NedVoice::insertBlocks");
		}
		((NedChordOrRest *) lptr->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, true);
	}
	else {
		((NedChordOrRest *) g_list_last(m_chord_or_rests)->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, true);
		pos = g_list_length(m_chord_or_rests);
	}

	for (i = 0; i < blockcount; i++) {
		items_to_insert = g_list_append(items_to_insert, new NedChordOrRest(this, TYPE_REST, m_voice_nr != 0, 3 /*dummy*/, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0 /* dummy */));
	}
	NedInsertChordOrRestGroupCommand *command = new NedInsertChordOrRestGroupCommand(this, items_to_insert, pos);
	command->execute();
	command_list->addCommand(command);
	assignMidiTimes();
}

void NedVoice::testForPageBackwardTies(NedCommandList *command_list) {
	((NedChordOrRest *) g_list_first(m_chord_or_rests)->data)->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD);
}


bool NedVoice::tryChangeLength(NedChordOrRest *chord_or_rest) {
	GList *lptr, *lptr2;
	NedChordOrRest *chord_or_rest_to_distribute;
	NedChordOrRest *last_tupleted_chord_or_rest;
	unsigned long long new_duration;
	unsigned long long change_chords_midi_end_time;
	unsigned long long end_time;
	unsigned long long offs;
	bool was_last_in_tuplet;
	NedTuplet *tuplet_ptr;
	int tuplet_val, tuplet_val_raw;
	int new_dot_count = NedResource::getDotCount();
	lptr = g_list_find(m_chord_or_rests, chord_or_rest);
	if (lptr == NULL) {
		return FALSE;
	}
	tuplet_val = chord_or_rest->getTupletVal();
	tuplet_val_raw = chord_or_rest->getTupletValRaw();
	was_last_in_tuplet = chord_or_rest->hasLastTupletFlag();
	chord_or_rest->setLastTupletFlag(FALSE);
	new_duration = NedChordOrRest::computeDuration(NedResource::getCurrentLength(), NedResource::getDotCount(), chord_or_rest->getTupletVal());
	if (chord_or_rest->getDuration() == new_duration) return TRUE;
	int pos = g_list_position(m_chord_or_rests, lptr);
	unsigned long long change_pos_midi_time = ((NedChordOrRest *) lptr->data)->getMidiTime();
	if ((tuplet_ptr = chord_or_rest->getTupletPtr()) != NULL)  {
		end_time = tuplet_ptr->getEndTime();
		change_chords_midi_end_time = change_pos_midi_time + new_duration;
	}
	else {
		NedMeasure *measure = ((NedChordOrRest *) lptr->data)->m_measure;
		change_chords_midi_end_time = change_pos_midi_time + new_duration;
		end_time = measure->midi_end;
		if (NedResource::getCurrentLength() == WHOLE_NOTE && NedResource::getDotCount() == 0 && chord_or_rest->getMidiTime() == change_pos_midi_time) {
			new_duration = chord_or_rest->m_measure->getMeasureDuration();
		}
	}
	change_chords_midi_end_time = change_pos_midi_time + new_duration;
	if (end_time > change_chords_midi_end_time) end_time = change_chords_midi_end_time;
	if (tuplet_ptr != NULL && tuplet_ptr->getMidiStartTime() == change_pos_midi_time &&  change_chords_midi_end_time - change_pos_midi_time == tuplet_ptr->getDuration()) {
		return FALSE;
	}
	unsigned int sum_of_chords_and_rests_to_delete = 0;
	GList *chords_and_rests_to_delete = NULL;
	NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
	if (tuplet_ptr == NULL) {
		searchForBreakableTuplet(command_list, end_time);
	}
	((NedChordOrRest *) lptr->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD);
	for (lptr2 = g_list_next( lptr ); lptr2 && ((NedChordOrRest *) lptr2->data)->getMidiTime() < end_time;
		lptr2 = g_list_next(lptr2)) {
		chords_and_rests_to_delete = g_list_append(chords_and_rests_to_delete, lptr2->data);
		((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list, BREAK_TIE_FORWARD);
		sum_of_chords_and_rests_to_delete += ((NedChordOrRest *) lptr2->data)->getDuration();
	}
	if (chords_and_rests_to_delete != NULL) {
		command_list->addCommand(new NedDeleteChordOrRestGroupCommand(this, chords_and_rests_to_delete));
	}
	unsigned int length = NedResource::getCurrentLength();
	if (new_duration > chord_or_rest->getDuration()) {
		if (new_duration - chord_or_rest->getDuration() > sum_of_chords_and_rests_to_delete) {
			if (tuplet_val > 0) {
				unsigned int d = sum_of_chords_and_rests_to_delete + chord_or_rest->getDuration();
				d *= tuplet_val;
				d /= NedResource::m_tuplet_tab[tuplet_val];
				NedChordOrRest::compute_fitting_duration(d, &length, &new_dot_count);
			}
			else {
				NedChordOrRest::compute_fitting_duration(sum_of_chords_and_rests_to_delete + chord_or_rest->getDuration(), &length, &new_dot_count);
			}
			new_duration = NedChordOrRest::computeDuration(length, new_dot_count, chord_or_rest->getTupletVal());
		}
	}
	else {
		sum_of_chords_and_rests_to_delete = chord_or_rest->getDuration() - new_duration;
	}
	if (new_duration > chord_or_rest->getDuration()) {
		sum_of_chords_and_rests_to_delete -= new_duration - chord_or_rest->getDuration();
	}

	last_tupleted_chord_or_rest = chord_or_rest;
	command_list->addCommand(new NedChangeChordOrRestLengthCommand(chord_or_rest, length, new_dot_count));
	if (chords_and_rests_to_delete == NULL) {
		chord_or_rest_to_distribute = NULL;
	}
	else {
		chord_or_rest_to_distribute = (NedChordOrRest *) (g_list_last(chords_and_rests_to_delete)->data);
	}
	if (sum_of_chords_and_rests_to_delete > 0) {
		pos++;
		offs = length;
		switch (new_dot_count) {
			case 1: offs *= 3; offs /= 2; break;
			case 2: offs *= 7; offs /= 4; break;
		}
		offs += chord_or_rest->getMidiTime();
		handleGapAfterInsertion2(command_list, offs, &pos, sum_of_chords_and_rests_to_delete, chord_or_rest_to_distribute, tuplet_val, false, false);
	}
	if (was_last_in_tuplet) {
		last_tupleted_chord_or_rest->setLastTupletFlag(TRUE);
	}
	getMainWindow()->getCommandHistory()->addCommandList(command_list);
	command_list->executeAfterBreak(); // because of searchForBreakableTuplet; don't repeat actions before break
	getMainWindow()->reposit(command_list, getPage(), getSystem());
	return TRUE;
}


bool NedVoice::tryConvertToTuplet(int method, int tuplet_val, NedChordOrRest *templ) {
	GList *lptr2;
	NedChordOrRest *chord_or_rest = NULL, *last_tupleted_chord_or_rest;
	int minpos;
	GList *min_pos_ptr;
	int num_elements_to_delete;
	unsigned long long new_chords_midi_end_time;
	unsigned long long  ref_duration;
	int num_additional_rests;


	if (templ->getTupletVal() != 0 || templ->getLength() < NOTE_32) {
		return FALSE;
	}
	if ((method == 1 && ((templ->getDuration() % 2) != 0)) || tuplet_val < 2 || (method > 0 && tuplet_val < 3)) {
		return FALSE;
	}
	ref_duration = (method == 1) ? templ->getDuration() / 2 : templ->getDuration();
	if ((min_pos_ptr = g_list_find(m_chord_or_rests, templ)) < 0) {
		NedResource::Abort("NedVoice::tryConvertToTuplet(1)");
	}
	if ((minpos = g_list_index(m_chord_or_rests, templ)) < 0) {
		NedResource::Abort("NedVoice::tryConvertToTuplet(2)");
	}
	if (((NedChordOrRest *) min_pos_ptr->data)->getTupletVal() != 0) {
		return false;
	}
	minpos++;
	//NedMeasure *measure = getSystem()->getMeasure(templ->getMidiTime());
	NedMeasure *measure = templ->m_measure;
	unsigned long long min_pos_midi_time = templ->getMidiTime();
	new_chords_midi_end_time = min_pos_midi_time + NedResource::m_tuplet_tab[tuplet_val] * ref_duration;
	unsigned int end_time = measure->midi_end;
	if (new_chords_midi_end_time > end_time) {
		return FALSE;
	}
	unsigned int sum_of_chords_and_rests_to_delete = 0;
	GList *chords_and_rests_to_delete = NULL;
	NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
	for (lptr2 = g_list_next(min_pos_ptr); lptr2 && ((NedChordOrRest *) lptr2->data)->getMidiTime() < new_chords_midi_end_time;
		lptr2 = g_list_next(lptr2)) {
		chords_and_rests_to_delete = g_list_append(chords_and_rests_to_delete, lptr2->data);
		((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list);
		sum_of_chords_and_rests_to_delete += ((NedChordOrRest *) lptr2->data)->getDuration();
	}
	/*
	if (sum_of_chords_and_rests_to_delete <  templ->getDuration()) {
		return FALSE;
	}
	*/
	if ((NedResource::m_tuplet_tab[tuplet_val] - 1) * ref_duration <= sum_of_chords_and_rests_to_delete) {
		sum_of_chords_and_rests_to_delete -= (NedResource::m_tuplet_tab[tuplet_val] - 1) * ref_duration;
	}
	if (chords_and_rests_to_delete != NULL) {
		command_list->addCommand(new NedDeleteChordOrRestGroupCommand(this, chords_and_rests_to_delete));
		num_elements_to_delete = g_list_length(chords_and_rests_to_delete);
	}
	command_list->addCommand(new NedSetToTupletCommand(templ, tuplet_val));
	num_additional_rests = (method == 1) ? tuplet_val - 2 : tuplet_val - 1;
	for (int i = 0; i < num_additional_rests; i++) {
		chord_or_rest = new NedChordOrRest(this, TYPE_REST, FALSE, 0, 0, ref_duration, NORMAL_NOTE, 0, 0);
		chord_or_rest->setTupletVal(tuplet_val);
		command_list->addCommand(new NedInsertChordOrRestCommand(this, minpos++, chord_or_rest));
	}
	last_tupleted_chord_or_rest = chord_or_rest;
	if (sum_of_chords_and_rests_to_delete > 0) {
		NedChordOrRest *chord_or_rest_to_distribute = (NedChordOrRest *) (g_list_last(chords_and_rests_to_delete)->data);
		handleGapAfterInsertion(command_list, &minpos, sum_of_chords_and_rests_to_delete, chord_or_rest_to_distribute, 0, false, false); 
	}

	last_tupleted_chord_or_rest->setLastTupletFlag(TRUE);
	getMainWindow()->getCommandHistory()->addCommandList(command_list);
	command_list->execute();
	getMainWindow()->reposit(command_list, getPage(), getSystem());
	getSystem()->m_is_positioned = false;
	return TRUE;
}

void NedVoice::removeTuplet(NedChordOrRest *element, NedCommandList *cmd_list) {
	NedTuplet *tuplet = element->getTupletPtr();
	NedCommandList *command_list;
	NedDeleteChordOrRestGroupCommand *del_chord;
	unsigned int tuplet_time;
	int pos;

	if (tuplet == NULL) {
		NedResource::Abort("NedVoice::removeTuplet(1)");
	}
	tuplet_time = tuplet->getDuration();
	pos = g_list_index(m_chord_or_rests, tuplet->getFirstElement());
	if (pos < 0) {
		NedResource::Abort("NedVoice::removeTuplet(2)");
	}
	if (cmd_list == NULL) {
		command_list = new NedCommandList(getMainWindow(), getSystem());
	}
	else {
		command_list = cmd_list;
	}
	tuplet->getFirstElement()->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD, cmd_list != NULL);
	tuplet->getLastElement()->testForTiesToDelete(command_list, BREAK_TIE_FORWARD, cmd_list != NULL);
	command_list->addCommand(del_chord = new NedDeleteChordOrRestGroupCommand(this, tuplet->getElementList()));
	if (cmd_list != NULL) {
		del_chord->execute();
	}
	if (tuplet_time > 0) {
		handleGapAfterInsertion(command_list, &pos, tuplet_time, NULL, 0, false, cmd_list != NULL);
	}

	if (cmd_list == NULL) {
		getMainWindow()->getCommandHistory()->addCommandList(command_list);
		command_list->execute();
		getMainWindow()->reposit(command_list, getPage(), getSystem());
	}
	getSystem()->m_is_positioned = false;
}

void NedVoice::checkForElementsToSplit(NedCommandList *command_list) {
	GList *lptr, *lptr2;
	GList *lptr3;
	int n2;
	GList *elements_to_delete = NULL;
	unsigned long long  t1;
	unsigned int len;
	bool in_upbeat;
	NedMeasure *measure;
	NedTuplet *tuplet;
	NedChordOrRest *element_to_distribute = NULL;
	unsigned int overflow = 0;
	NedChordOrRest *element;
	NedChangeChordOrRestLengthCommand *change_length_command;
	NedInsertTiedElementCommand *insert_element_command;
	NedDeleteChordOrRestGroupCommand *del_group;
	int pos = 0, dotcount;
	int n;


	for (n = 0, lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr), n++) {
		element = (NedChordOrRest *) lptr->data;
		measure = getSystem()->getMeasure(element->getMidiTime());
		//measure = element->m_measure;
		if (measure == NULL) {
			int idx =  g_list_position(m_chord_or_rests, lptr);
			int ll = g_list_length(m_chord_or_rests);
			for (n2 = 0, lptr3 = g_list_first(m_chord_or_rests); lptr3; lptr3 = g_list_next(lptr3), n2++) {
				printf("%d: ", n2);
				if (((NedChordOrRest *) lptr3->data)->isRest()) {
					printf("r");
				}
				else {
					printf("n");
				}
				printf("%u(%u, %u) ", LAUS(((NedChordOrRest *) lptr3->data)->getLength()));
				if (((NedChordOrRest *) lptr3->data)->getDotCount()) {
					printf("D%d ", ((NedChordOrRest *) lptr3->data)->getDotCount());
				}
				if (!((NedChordOrRest *) lptr3->data)->isRest()) {
					printf(" %d ", ((NedChordOrRest *) lptr3->data)->getLineOfFirstNote());
				}
				printf("\n"); fflush(stdout);
			}

			printf("pag: %d, sys: %d, sta: %d, voi: %d, idx = %d n = %d, len = %d, element->getMidiTime() = %llu(%llu, %llu)\n",
				getPage()->getPageNumber(), getSystem()->getSystemNumber(), m_staff->getStaffNumber(), m_voice_nr,
				idx, n, ll, LAUS(element->getMidiTime())); fflush(stdout);
			NedResource::Abort("NedVoice::checkForElementsToSplit(0)");
		}
		if (element->getTupletVal() != 0) {
			tuplet = element->getTupletPtr();
			if (tuplet->getMidiStartTime() < measure->midi_start && 
				tuplet->getMidiStartTime() + tuplet->getDuration() > measure->midi_start) {
				removeTuplet(element, command_list);
				lptr = g_list_first(m_chord_or_rests); 
				assignMidiTimes(); // recompute midi times
			}
		}
	}
	in_upbeat = getPage()->getPageNumber() == 0 && getMainWindow()->getUpBeatInverse() > 0 &&
			getSystem()->getSystemNumber() == 0;


	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		element = (NedChordOrRest *) lptr->data;
		measure = getSystem()->getMeasure(element->getMidiTime());
		//measure = element->m_measure;
		if (measure == NULL) {
			elements_to_delete = g_list_append(elements_to_delete, element);
			continue;
		}
		if (overflow == 0 && element->isRest() && element->getLength() == WHOLE_NOTE && element->getMidiTime() == measure->midi_start) {
			overflow = 0;
			assignMidiTimes();
			continue;
		}
			
		if ((!(in_upbeat && measure->getMeasureNumber() == 0) && overflow == 0) || (element_to_distribute != NULL && element_to_distribute->isRest())) {
			if (element->getLength() == WHOLE_NOTE && element->isRest() && element->getMidiTime() == measure->midi_start /*  && 
					element->getMidiTime() + element->getDuration() == measure->midi_end */) {
				overflow = 0;
				assignMidiTimes();
				continue;
			}
		}
		if (overflow > measure->getMeasureDuration()) {
			overflow = 0;
		}
		if (overflow > 0) {
			NedResource::setLengthField(overflow);
			len = NedResource::getPartLength(&dotcount);
			while (len > 0) {
				insert_element_command = new NedInsertTiedElementCommand(this, element_to_distribute, len, dotcount);
				insert_element_command->execute();
				command_list->addCommand(insert_element_command);
				pos++;
				if ((lptr2 = g_list_nth(m_chord_or_rests, pos)) == NULL) {
					NedResource::Abort("checkForElementsToSplit(1)");
				}
				element_to_distribute = (NedChordOrRest *) lptr2->data;
				len = NedResource::getPartLength(&dotcount);
			}
			overflow = 0;
			element_to_distribute = NULL;
			if ((lptr = g_list_nth(m_chord_or_rests, pos)) == NULL) {
				NedResource::Abort("checkForElementsToSplit(2)");
			}
			assignMidiTimes();
		}
		overflow = 0;
		if ((pos = g_list_position(m_chord_or_rests, lptr)) < 0) {
			NedResource::Abort("checkForElementsToSplit(3)");
		}

		if (element->isRest() && element->getLength() == WHOLE_NOTE) { // i.e. was overflow into a measure with a whole rest
			t1 = measure->midi_end - element->getMidiTime();
			handleGapAfterInsertion(command_list, &pos, t1, NULL, 0, false, true);
			assignMidiTimes();
			if ((lptr = g_list_nth(m_chord_or_rests, pos)) == NULL) {
				NedResource::Abort("checkForElementsToSplit(5)");
			}
			continue;
		}
				
		if (element->getMidiTime() < measure->midi_end && element->getMidiTime() + element->getDuration() > measure->midi_end) {
			t1 = measure->midi_end - element->getMidiTime();
			/*
			if (!(in_upbeat && measure->getMeasureNumber() == 0) || !(element->isRest() && element->getLength() == WHOLE_NOTE)) {
				overflow = element->getMidiTime() + element->getDuration() - measure->midi_end;
			}
			*/
			overflow = element->getMidiTime() + element->getDuration() - measure->midi_end;
			NedResource::setLengthField(t1);
			len = NedResource::getPartLength(&dotcount);
			change_length_command = new NedChangeChordOrRestLengthCommand(element, len, dotcount);
			change_length_command->execute();
			command_list->addCommand(change_length_command);
			len = NedResource::getPartLength(&dotcount);
			element_to_distribute = element;
			while (len > 0) {
				insert_element_command = new NedInsertTiedElementCommand(this, element_to_distribute, len, dotcount);
				insert_element_command->execute();
				command_list->addCommand(insert_element_command);
				pos++;
				if ((lptr2 = g_list_nth(m_chord_or_rests, pos)) == NULL) {
					NedResource::Abort("checkForElementsToSplit(4)");
				}
				element_to_distribute = (NedChordOrRest *) lptr2->data;
				len = NedResource::getPartLength(&dotcount);
			}
			if ((lptr = g_list_nth(m_chord_or_rests, pos > 0 ? pos-1 : pos)) == NULL) {
				NedResource::Abort("checkForElementsToSplit(6)");
			}
		}
	}
	if (elements_to_delete != NULL) {
		del_group = new NedDeleteChordOrRestGroupCommand(this, elements_to_delete);
		del_group->execute();
		command_list->addCommand(del_group);
	}
	testAllMeasuresForFillup(command_list);
}

GList *NedVoice::getFirstChordOrRest(int lyrics_line, bool lyrics_required, bool note_required) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (note_required && ((NedChordOrRest *) lptr->data)->getType() != TYPE_CHORD) continue;
		if ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) == 0) continue;
		if (lyrics_required && ((NedChordOrRest *) lptr->data)->getType() != TYPE_CHORD) continue;
		if (!lyrics_required) return lptr;
		if (((NedChordOrRest *) lptr->data)->getLyrics(lyrics_line) != NULL) {
			return lptr;
		}
	}
	return NULL;
}

GList *NedVoice::getLastChordOrRest(int lyrics_line, bool lyrics_required, bool note_required) {
	GList *lptr;

	for (lptr = g_list_last(m_chord_or_rests); lptr; lptr = g_list_previous(lptr)) {
		if (note_required && ((NedChordOrRest *) lptr->data)->getType() != TYPE_CHORD) continue;
		if ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) == 0) continue;
		if (lyrics_required && ((NedChordOrRest *) lptr->data)->getType() != TYPE_CHORD) continue;
		if (!lyrics_required) return lptr;
		if (((NedChordOrRest *) lptr->data)->getLyrics(lyrics_line) != NULL) {
			return lptr;
		}
	}
	return NULL;
}


void NedVoice::testAllMeasuresForFillup(NedCommandList *command_list) {
	int last_measure_number;
	NedChordOrRest *last_element;
	GList *appended_wholes = NULL;
	NedMeasure *measure;
	int meas_number;
	unsigned int diff;
	int pos;

	if (m_chord_or_rests == NULL) return;
	NedInsertChordOrRestGroupCommand *insert_chord_or_rest_group_command;

	last_measure_number = getSystem()->getNumberOfLastMeasure();
	last_element = (NedChordOrRest *) g_list_last(m_chord_or_rests)->data;
	//measure = getSystem()->getMeasure(last_element->getMidiTime());
	measure = last_element->m_measure;
	if (measure == NULL) return;
	if (measure->midi_end > last_element->getMidiTime() + last_element->getDuration()) {
		diff = measure->midi_end - (last_element->getMidiTime() + last_element->getDuration());
		if ((pos = g_list_index(m_chord_or_rests, last_element)) < 0) {
			NedResource::Abort("NedVoice::testAllMeasuresForFillup");
		}
		pos++;
		handleGapAfterInsertion(command_list, &pos, diff, NULL, 0, false, true);
	}
	else {
		pos = g_list_length(m_chord_or_rests);
	}
	for (meas_number = measure->getMeasureNumber(); meas_number < last_measure_number; meas_number++) {
		appended_wholes = g_list_append(appended_wholes, new NedChordOrRest(this, TYPE_REST, m_voice_nr > 0, 3 /* dummy */, 0, WHOLE_NOTE, NORMAL_NOTE, 0, 0));
	}
	if (appended_wholes != NULL) {
		insert_chord_or_rest_group_command = new NedInsertChordOrRestGroupCommand(this, appended_wholes, pos);
		insert_chord_or_rest_group_command->execute();
		command_list->addCommand(insert_chord_or_rest_group_command);
	}
}

		



NedChordOrRest *NedVoice::handleGapAfterInsertion(NedCommandList *command_list, int *pos /* in/out */,
			int sum_of_chords_and_rests_to_delete, NedChordOrRest *chord_or_rest_to_distribute,
			int tuplet_val, bool force_wholes, bool do_execute) {
	unsigned int len2 = 0;
	int dotcount = 0;
	NedInsertChordOrRestCommand *insert_command;
	NedChordOrRest *chord_or_rest, *last_tupleted_chord_or_rest = NULL, *distributed_new_sym;
	if (tuplet_val > 0) {
		sum_of_chords_and_rests_to_delete *= tuplet_val;
		sum_of_chords_and_rests_to_delete /= NedResource::m_tuplet_tab[tuplet_val];
	}
	/*
	if (sum_of_chords_and_rests_to_delete >= WHOLE_NOTE) {
		len2 = WHOLE_NOTE;
	}
	else */
	if (force_wholes) {
		len2 = WHOLE_NOTE;
		last_tupleted_chord_or_rest = chord_or_rest = new NedChordOrRest(this, TYPE_REST, FALSE, 3, dotcount, len2, NORMAL_NOTE, 0, 0);
		insert_command = new NedInsertChordOrRestCommand(this, *pos, chord_or_rest);
		command_list->addCommand(insert_command);
		if (do_execute) {
			insert_command->execute();
		}
	}
	else {
		NedResource::setLengthField(sum_of_chords_and_rests_to_delete);
		len2 = NedResource::getPartLength(&dotcount);
		while (len2 > 0) {
			if (chord_or_rest_to_distribute == NULL) {
				last_tupleted_chord_or_rest = chord_or_rest = new NedChordOrRest(this, TYPE_REST, FALSE, 3, dotcount, len2, NORMAL_NOTE, 0, 0);
				chord_or_rest->setTupletVal(tuplet_val);
				insert_command = new NedInsertChordOrRestCommand(this, *pos, chord_or_rest);
				command_list->addCommand(insert_command);
			}
			else {
				last_tupleted_chord_or_rest = distributed_new_sym = chord_or_rest_to_distribute->cloneWithDifferentLength(len2, dotcount);
				distributed_new_sym->setTupletVal(tuplet_val);
				insert_command = new NedInsertChordOrRestCommand(this, *pos, distributed_new_sym);
				command_list->addCommand(insert_command);
			}
			if (do_execute) {
				insert_command->execute();
			}
			len2 = NedResource::getPartLength(&dotcount);
			(*pos)++;
		}
	}
	assignMidiTimes(); // the new element(s) has no MIDI time, but needed in reposit (collect FirstMeasure)
	return last_tupleted_chord_or_rest;
}

NedChordOrRest *NedVoice::handleGapAfterInsertion2(NedCommandList *command_list, unsigned long long offs, int *pos /* in/out */,
			int sum_of_chords_and_rests_to_delete, NedChordOrRest *chord_or_rest_to_distribute,
			int tuplet_val, bool force_wholes, bool do_execute) {
	unsigned int len2 = 0;
	int dotcount = 0;
	NedInsertChordOrRestCommand *insert_command;
	NedChordOrRest *chord_or_rest, *last_tupleted_chord_or_rest = NULL, *distributed_new_sym;
	if (tuplet_val > 0) {
		sum_of_chords_and_rests_to_delete *= tuplet_val;
		sum_of_chords_and_rests_to_delete /= NedResource::m_tuplet_tab[tuplet_val];
		offs *= tuplet_val;
		offs /= NedResource::m_tuplet_tab[tuplet_val];
	}
	/*
	if (sum_of_chords_and_rests_to_delete >= WHOLE_NOTE) {
		len2 = WHOLE_NOTE;
	}
	else */
	if (force_wholes) {
		len2 = WHOLE_NOTE;
		last_tupleted_chord_or_rest = chord_or_rest = new NedChordOrRest(this, TYPE_REST, FALSE, 3, dotcount, len2, NORMAL_NOTE, 0, 0);
		insert_command = new NedInsertChordOrRestCommand(this, *pos, chord_or_rest);
		command_list->addCommand(insert_command);
		if (do_execute) {
			insert_command->execute();
		}
	}
	else {
		NedResource::setLengthField2(0, offs, sum_of_chords_and_rests_to_delete);
		len2 = NedResource::getPartLength2(&dotcount);
		while (len2 > 0) {
			if (chord_or_rest_to_distribute == NULL) {
				last_tupleted_chord_or_rest = chord_or_rest = new NedChordOrRest(this, TYPE_REST, FALSE, 3, dotcount, len2, NORMAL_NOTE, 0, 0);
				chord_or_rest->setTupletVal(tuplet_val);
				insert_command = new NedInsertChordOrRestCommand(this, *pos, chord_or_rest);
				command_list->addCommand(insert_command);
			}
			else {
				last_tupleted_chord_or_rest = distributed_new_sym = chord_or_rest_to_distribute->cloneWithDifferentLength(len2, dotcount);
				distributed_new_sym->setTupletVal(tuplet_val);
				insert_command = new NedInsertChordOrRestCommand(this, *pos, distributed_new_sym);
				command_list->addCommand(insert_command);
			}
			if (do_execute) {
				insert_command->execute();
			}
			len2 = NedResource::getPartLength2(&dotcount);
			(*pos)++;
		}
	}
	assignMidiTimes(); // the new element(s) has no MIDI time, but needed in reposit (collect FirstMeasure)
	return last_tupleted_chord_or_rest;
}

bool NedVoice::searchForBreakableTuplet(NedCommandList *command_list, unsigned long long critical_time) {
	GList *lptr;
	bool tuplet_broken;
	bool was_broken = FALSE;
	NedTuplet *tuplet;
	NedChordOrRest *chord_or_rest;
	int pos;
	int i;


	do {
		tuplet_broken = FALSE;
		for (i = 0, lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr), i++) {
			chord_or_rest = (NedChordOrRest *) lptr->data;
			if (chord_or_rest->getTupletVal() == 0) continue;
			tuplet = chord_or_rest->getTupletPtr();
			if (tuplet == NULL) continue; // can happen if the reposition gets tuplets from next page
			/*
				printf("is rest = %d, type = %d(0x%x), i = %d, pa = %d, sys = %d, sta = %d, voi = %d\n",
					chord_or_rest->isRest(), chord_or_rest->getType(), chord_or_rest->getType(),i, getPage()->getPageNumber(), getSystem()->getSystemNumber(), m_staff->getStaffNumber(), m_voice_nr); fflush(stdout);
				NedResource::Abort("NedVoice::searchForBreakableTuplet(1)");
			}
			*/
			if (tuplet->getMidiStartTime() < critical_time && tuplet->getEndTime() > critical_time) {
				tuplet_broken = TRUE;
				pos = g_list_index(m_chord_or_rests, tuplet->getFirstElement());
				if (pos < 0) {
					NedResource::Abort("NedVoice::searchForBreakableTuplet(2)");
				}
				command_list->addCommand(new NedDeleteChordOrRestGroupCommand(this, tuplet->getElementList()));
				pos++;
				handleGapAfterInsertion(command_list, &pos, tuplet->getDuration(), NULL, 0, false, false);
				command_list->executeAfterBreakAndSetBreak();
				was_broken = TRUE;
				assignMidiTimes();
				break;
			}
		}
	}
	while (tuplet_broken);
	return was_broken;
}

void NedVoice::searchForBreakableTies(NedCommandList *command_list, unsigned int dir, unsigned long long start_time, unsigned long long end_time) {
	GList *lptr;
	NedChordOrRest *chord_or_rest;
	unsigned long long chord_start_time;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		chord_start_time = chord_or_rest->getMidiTime();
		if (chord_start_time + chord_or_rest->getDuration() <= start_time) continue;
		if (chord_start_time >= end_time) break;
		if (chord_or_rest->testForTiesToDelete(command_list, dir)) {
			command_list->executeAfterBreakAndSetBreak();
		}
	}
}

void NedVoice::testForTupletEnd(unsigned long long start_time, unsigned int duration, unsigned long long *end_time) {
	GList *lptr;
	NedChordOrRest *chord_or_rest;
	unsigned long long chord_start_time;
	unsigned long long chord_end_time;
	NedTuplet *tuplet;

	chord_end_time = start_time + duration;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		chord_start_time = chord_or_rest->getMidiTime();
		if (chord_start_time + chord_or_rest->getDuration() <= start_time) continue;
		if (chord_start_time >= *end_time) break;
		if (chord_or_rest->getTupletVal() != 0) {
			tuplet = chord_or_rest->getTupletPtr();
			if (tuplet == NULL) {
				NedResource::Abort("NedVoice::testForTupletEnd");
			}
			if (chord_end_time <= tuplet->getMidiStartTime()) continue;
			if (chord_end_time >= tuplet->getMidiStartTime() + tuplet->getDuration()) continue;
			*end_time = tuplet->getMidiStartTime();
			break;
		}

	}
}


bool NedVoice::setNewXpos(NedChordOrRest *element, double newpos) {
	GList *lptr1, *lptr2;
	NedTuplet *tuplet_ptr1, *tuplet_ptr2;
	int tuplet_val;


	lptr1 = g_list_find(m_chord_or_rests, element);
	if (lptr1 == NULL) {
		NedResource::Abort("NedVoice::setNewXpos");
	}
	tuplet_val = ((NedChordOrRest *) lptr1->data)->getTupletVal();
	if (tuplet_val == 0) {
	  //NedMeasure *meas_info1 = getSystem()->getMeasure(((NedChordOrRest *) lptr1->data)->getMidiTime());
	  NedMeasure *meas_info1 = ((NedChordOrRest *) lptr1->data)->m_measure;
	  for (lptr2 = g_list_first(m_chord_or_rests); lptr2; lptr2 = g_list_next(lptr2)) {
		if (((NedChordOrRest *) lptr2->data)->getRealXpos() > newpos) {
			//NedMeasure *meas_info2 =  getSystem()->getMeasure(((NedChordOrRest *) lptr2->data)->getMidiTime());
			NedMeasure *meas_info2 = ((NedChordOrRest *) lptr2->data)->m_measure;
			if (meas_info1 != meas_info2) {
				if (meas_info2 - meas_info1 == 1) {
					break;
				}
				return FALSE;
			}
			if (lptr1 == lptr2) return FALSE;
			NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
			((NedChordOrRest *) lptr1->data)->testForTiesToDelete(command_list);
			((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list);
			command_list->addCommand(new NedChangeXPositionCommand(&m_chord_or_rests, (NedChordOrRest *) lptr2->data, (NedChordOrRest *) lptr1->data, TRUE));
			command_list->execute();
			getMainWindow()->getCommandHistory()->addCommandList(command_list);
			return TRUE;
		}
	  }
	  for (lptr2 = g_list_last(m_chord_or_rests); lptr2; lptr2 = g_list_previous(lptr2)) {
		if (((NedChordOrRest *) lptr2->data)->getRealXpos() < newpos) {
			//NedMeasure *meas_info2 =  getSystem()->getMeasure(((NedChordOrRest *) lptr2->data)->getMidiTime());
			NedMeasure *meas_info2 = ((NedChordOrRest *) lptr2->data)->m_measure;
			if (meas_info1 != meas_info2) return FALSE;
			if (lptr1 == lptr2) return FALSE;
			NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
			((NedChordOrRest *) lptr1->data)->testForTiesToDelete(command_list);
			((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list);
			command_list->addCommand(new NedChangeXPositionCommand(&m_chord_or_rests, (NedChordOrRest *) lptr2->data, (NedChordOrRest *) lptr1->data, FALSE));
			command_list->execute();
			getMainWindow()->getCommandHistory()->addCommandList(command_list);
			return TRUE;
		}
	  }
	}
	else {
	  tuplet_ptr1 = ((NedChordOrRest *) lptr1->data)->getTupletPtr();
	  for (lptr2 = g_list_first(m_chord_or_rests); lptr2; lptr2 = g_list_next(lptr2)) {
		if (((NedChordOrRest *) lptr2->data)->getRealXpos() > newpos) {
			tuplet_ptr2 = ((NedChordOrRest *) lptr2->data)->getTupletPtr();
			if (lptr1 == lptr2 || tuplet_ptr1 != tuplet_ptr2) {
				break;
			}
			NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
			((NedChordOrRest *) lptr1->data)->testForTiesToDelete(command_list);
			((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list);
			command_list->addCommand(new NedChangeXPositionCommand(&m_chord_or_rests, (NedChordOrRest *) lptr2->data, (NedChordOrRest *) lptr1->data, TRUE));
			command_list->execute();
			getMainWindow()->getCommandHistory()->addCommandList(command_list);
			return TRUE;
		}
	  }
	  for (lptr2 = g_list_last(m_chord_or_rests); lptr2; lptr2 = g_list_previous(lptr2)) {
		if (((NedChordOrRest *) lptr2->data)->getRealXpos() < newpos) {
			NedCommandList *command_list = new NedCommandList(getMainWindow(), getSystem());
			tuplet_ptr2 = ((NedChordOrRest *) lptr2->data)->getTupletPtr();
			if (lptr1 == lptr2 || tuplet_ptr1 != tuplet_ptr2) {
				return FALSE;
			}
			((NedChordOrRest *) lptr1->data)->testForTiesToDelete(command_list);
			((NedChordOrRest *) lptr2->data)->testForTiesToDelete(command_list);
			command_list->addCommand(new NedChangeXPositionCommand(&m_chord_or_rests, (NedChordOrRest *) lptr2->data, (NedChordOrRest *) lptr1->data, FALSE));
			command_list->execute();
			getMainWindow()->getCommandHistory()->addCommandList(command_list);
			return TRUE;
		}
	  }
	}
	return FALSE;
}

void NedVoice::searchForBeamGroups(unsigned int midi_start_time, NedChordOrRest *new_chord_or_rest /* = NULL */) {
	GList *lptr, *beamlist = NULL, *lptr3 = NULL;
	unsigned int beats;
	bool split = false;
	NedMeasure *last_measure = NULL, *measure = NULL;
	bool grace_mode = false;
	//unsigned int meas_duration;

	while ((lptr =  g_list_first(m_beam_list)) != NULL) {
		// the beam removes itself from m_beam_list if deleted
		// therfore we must restart from list begin in every loop
		delete ((NedBeam *) lptr->data);
	}
	g_list_free(m_beam_list);
	m_beam_list = NULL;

	/*
	lptr = g_list_first(m_beam_list);
	while (lptr) {
		NedBeam *b = (NedBeam *) lptr->data;
		delete b; // includes NULL pointer in chords and removal from m_beam_list
		lptr = g_list_first(m_beam_list);
	}

	for (lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		((NedBeam *) lptr->data)->m_needed = FALSE;
	}
	meas_duration = getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
	*/
	for(lptr = g_list_first(m_chord_or_rests), lptr3 = NULL; lptr; lptr3 = lptr, lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_KEYSIG) continue; // can happen during do_import_from_other (testLineOverflow)
		NedChordOrRest *chord_or_rest = (NedChordOrRest *) lptr->data;
		last_measure = measure;
		//measure = getSystem()->getMeasure(chord_or_rest->getMidiTime());
		measure = chord_or_rest->m_measure;
		split = m_staff->hasStaffElem(chord_or_rest->getMidiTime());
		if (measure == NULL) {
			NedResource::Abort("NedVoice::searchForBeamGroups");
		}
		beats = chord_or_rest->getMidiTime() - measure->midi_start;
		if (last_measure != measure) {
			 split = true;
		}
		/*
		if (meas_duration <= beats) {
			split = true;
		}
		if (!split) {
			split = beamEndRequired(beamlist, beats, measure) || ((NedChordOrRest *) lptr->data)->hasLastTupletFlag();
		}
		*/
		if (!split) {
			if ((chord_or_rest->getStatus() & STAT_FORCE_BEAM) == 0) {
				split = beamEndRequired(beamlist, beats, measure);
			}
		}
		if (!split && lptr3 != NULL) {
			split = (chord_or_rest->getTupletVal() != ((NedChordOrRest *) lptr3->data)->getTupletVal()) || ((NedChordOrRest *) lptr3->data)->hasLastTupletFlag();
		}
		/*
		if (lptr3 != NULL) {
			split = split || (chord_or_rest->getTupletVal() != ((NedChordOrRest *) lptr3->data)->getTupletVal());
		}
		*/
		if (chord_or_rest->getType() == TYPE_GRACE) {
			if (grace_mode) {
				split = false;
			}
			else {
				split = true;
			}
			grace_mode = true;
		}
		else if (grace_mode) {
			grace_mode = false;
			split = true;
		}

		if ((split || (chord_or_rest->getStatus() & STAT_BREAK_BEAM) || (chord_or_rest->getType() & (TYPE_CHORD | TYPE_GRACE)) == 0 ||
			chord_or_rest->getLength() >=  NOTE_4 || chord_or_rest->getFirstNote()->getNoteHead() == GUITAR_NOTE_NO_STEM) && beamlist != NULL) {
			if (g_list_length(beamlist) > 1) {
			/*
				inserted = FALSE;
				for (zz = 0, lptr2 = g_list_first(m_beam_list); lptr2; lptr2 = g_list_next(lptr2), zz++) {
					beam = (NedBeam *) lptr2->data;
					beam->m_needed = beam->isEqual(beamlist, new_chord_or_rest, &newpos);
					NedResource::DbgMsg(DBG_TESTING, "Beam %d --> %d\n", zz, beam->m_needed); 
					if (new_chord_or_rest != NULL && beam->m_needed) {
						beam->insertChord(new_chord_or_rest, newpos);
						inserted = TRUE;
					}
					else if (beam->m_needed) {
						inserted = TRUE;
					}
				}
				if (inserted) {
					g_list_free(beamlist);
				}
				else {
				*/
					NedBeam *bb = new NedBeam(beamlist);
					m_beam_list = g_list_append(m_beam_list, bb);
					/*
				}
				*/
			}
			else {
				g_list_free(beamlist);
			}
			beamlist = NULL;
		}
		if ((chord_or_rest->getType() & (TYPE_CHORD | TYPE_GRACE)) != 0 && chord_or_rest->getLength() != STROKEN_GRACE && chord_or_rest->getLength() < NOTE_4) {
			beamlist = g_list_append(beamlist, chord_or_rest);
		}
		//measure = getSystem()->getMeasure(chord_or_rest->getMidiTime());
		measure = chord_or_rest->m_measure;
		/*
		if (measure == NULL) {
			meas_duration = getMainWindow()->getNumerator() * WHOLE_NOTE / getMainWindow()->getDenominator();
		}
		else {
			meas_duration = getMainWindow()->getMeasureDuration(measure->getMeasureNumber());
		}
		*/
	}
	if (beamlist != NULL) {
		if (g_list_length(beamlist) > 1) {
			NedBeam *bb = new NedBeam(beamlist);
			m_beam_list = g_list_append(m_beam_list, bb);
		}
		else {
			g_list_free(beamlist);
			beamlist = NULL;
		}
	}
	/*
	lptr = g_list_first(m_beam_list);
	while (lptr) {
		NedBeam *b = (NedBeam *) lptr->data;
		if (b->getChordCount() < 1 || !b->m_needed)  {
			delete b; // includes NULL pointer in chords/rests and removal from m_beam_list
			lptr = g_list_first(m_beam_list);
		}
		else {
			lptr = g_list_next(lptr);
		}
	}
	*/
}

void NedVoice::setBeamFlagsOf(NedCommandList *command_list, GList *selected_group, NedChordOrRest *start_elem) {
	GList *sel_group_ptr, *voice_list_ptr;
	void *last_touched = NULL;
	NedMeasure *measure, *measure1;
	unsigned int status;
	NedChangeChordOrRestStatusCommand *change_status;

	//measure = getSystem()->getMeasure(start_elem->getMidiTime());
	measure = start_elem->m_measure;


	for (sel_group_ptr = g_list_first(selected_group); sel_group_ptr; sel_group_ptr = g_list_next(sel_group_ptr)) {
		if (((NedChordOrRest *) sel_group_ptr->data)->getVoice() != this) continue;
		if (((NedChordOrRest *) sel_group_ptr->data)->getType() != TYPE_CHORD) continue;
		if (((NedChordOrRest *) sel_group_ptr->data)->getLength() >= NOTE_4) continue;
		measure1 = ((NedChordOrRest *) sel_group_ptr->data)->m_measure;
		if (measure != measure1) break;
		status = ((NedChordOrRest *) sel_group_ptr->data)->getStatus();
		if( (status & STAT_BREAK_BEAM) || !(status & STAT_FORCE_BEAM) ){
			status &= (~(STAT_BREAK_BEAM));
			status |= STAT_FORCE_BEAM;
			NedChangeChordOrRestStatusCommand *change_status = new NedChangeChordOrRestStatusCommand((NedChordOrRest *) sel_group_ptr->data, status);
			command_list->addCommand(change_status);
			change_status->execute();
		}
		last_touched = sel_group_ptr->data;
	}

	status = start_elem->getStatus();
	if( (status & STAT_FORCE_BEAM) || !(status & STAT_BREAK_BEAM) ){
		status &= (~(STAT_FORCE_BEAM));
		status |= STAT_BREAK_BEAM;
		change_status = new NedChangeChordOrRestStatusCommand(start_elem, status);
		command_list->addCommand(change_status);
		change_status->execute();
	}

	if (last_touched == NULL) return;

	if ((voice_list_ptr = g_list_find(m_chord_or_rests, last_touched)) == NULL) {
		NedResource::Abort("NedVoice::setBeamFlagsOf");
	}
	voice_list_ptr = g_list_next(voice_list_ptr);
	if (voice_list_ptr == NULL) return;

	measure1 = ((NedChordOrRest *) voice_list_ptr->data)->m_measure;
	if (measure != measure1) return;

	if (((NedChordOrRest *) voice_list_ptr->data)->getType() != TYPE_CHORD) return;
	if (((NedChordOrRest *) voice_list_ptr->data)->getLength() >= NOTE_4) return;

	status = ((NedChordOrRest *) voice_list_ptr->data)->getStatus();
	if( (status & STAT_FORCE_BEAM) || !(status & STAT_BREAK_BEAM) ){
		status &= (~(STAT_FORCE_BEAM));
		status |= STAT_BREAK_BEAM;
		change_status = new NedChangeChordOrRestStatusCommand((NedChordOrRest *) voice_list_ptr->data, status);
		command_list->addCommand(change_status);
		change_status->execute();
	}
}

void NedVoice::isolate(NedCommandList *command_list, NedChordOrRest *element) {
	GList *lptr;
	NedBeam *beam;
	unsigned int status;
	NedChangeChordOrRestStatusCommand *change_status;

	if (element->getBeam() == NULL) return;

	if ((lptr = g_list_find(m_chord_or_rests, element)) == NULL) {
		NedResource::Abort("NedVoice::isolate");
	}
	
	if ((beam = element->getBeam()) == NULL) return;
	lptr = g_list_next(lptr);
	if (lptr == NULL) return;

	if (((NedChordOrRest *) lptr->data)->getBeam() != beam) return;

	status = ((NedChordOrRest *) lptr->data)->getStatus();
	status &= (~(STAT_FORCE_BEAM));
	status |= STAT_BREAK_BEAM;
	change_status = new NedChangeChordOrRestStatusCommand((NedChordOrRest *) lptr->data, status);
	command_list->addCommand(change_status);
	change_status->execute();

	status = element->getStatus();
	status &= (~(STAT_FORCE_BEAM));
	status |= STAT_BREAK_BEAM;
	change_status = new NedChangeChordOrRestStatusCommand(element, status);
	command_list->addCommand(change_status);
	change_status->execute();
}


bool NedVoice::findFirstElementeGreaterThan(double x, NedChordOrRest **element) {
	GList *lptr;

	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getXPos() > x) {
			*element = (NedChordOrRest *) lptr->data;
			return TRUE;
		}
	}

	return FALSE;
}

bool NedVoice::findLastElementeLessThan(double x, NedChordOrRest **element) {
	GList *lptr;

	for(lptr = g_list_last(m_chord_or_rests); lptr; lptr = g_list_previous(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getXPos() < x) {
			*element = (NedChordOrRest *) lptr->data;
			return TRUE;
		}
	}

	return FALSE;
}

void NedVoice::collectChordsAndRests(NedClipBoard *board, unsigned long long midi_start, unsigned long long  midi_end) {
	GList *lptr;
	NedClipBoardElement *element = new NedClipBoardElement();
	element->from_voice = this;
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() < midi_start  || ((NedChordOrRest *) lptr->data)->getMidiTime() >= midi_end) continue;
		element->chords_or_rests_to_move = g_list_append(element->chords_or_rests_to_move, lptr->data);
	}
	board->m_elements = g_list_append(board->m_elements, element);
}

void NedVoice::collectDestinationVoices(NedClipBoard *board) {
	NedClipBoardElement *element;
	if (board->element_counter == NULL) {
		NedResource::Abort("NedVoice::collectDestinationVoices(1)");
	}
	element = ((NedClipBoardElement *) board->element_counter->data);
	if (element->from_voice == NULL) {
		NedResource::Abort("NedVoice::collectDestinationVoices(2)");
	}
	element->to_voice = this;
	board->element_counter = g_list_next(board->element_counter);
}

	

bool NedVoice::beamEndRequired(GList *beamlist_so_far, unsigned int beats, NedMeasure *measure) {
	int shortestNote = WHOLE_NOTE, dur;
	GList *lptr;
	struct rule_str *wild_ptr = NULL, *rule_ptr = NULL, *ptr;
	int best_match = -1;
	int num, denom;

	for (lptr = g_list_first(beamlist_so_far); lptr; lptr = g_list_next(lptr)) {
		dur = ((NedChordOrRest *) lptr->data)->getDuration();
		if (dur < shortestNote) shortestNote = dur;
	}
	if (measure == NULL) {
		num = getMainWindow()->getNumerator(); 
		denom = getMainWindow()->getDenominator();
	}
	else {
		num = getMainWindow()->getNumerator(measure->getMeasureNumber());
		denom = getMainWindow()->getDenominator(measure->getMeasureNumber());
	}
	for (ptr = beam_rules_tab__; ptr->function != END_OF_TABLE; ptr++) {
		if (ptr->function!= FUNC_END) continue;
		if (num == ptr->time_num && denom == ptr->time_denom) {
			if (ptr->notelen < 0) {
				wild_ptr = ptr;
			}
			else if (ptr->notelen >= shortestNote) {
				if (best_match < 0) {
					best_match = ptr->notelen;
					rule_ptr = ptr;
				}
				else if (best_match > ptr->notelen) {
					best_match = ptr->notelen;
					rule_ptr = ptr;
				}
			}
		}
	}
	if ((best_match != shortestNote || rule_ptr == NULL) && wild_ptr != NULL) {
		rule_ptr = wild_ptr;
	}
	if (rule_ptr == NULL) {
		return false;
	}
	return ((beats % rule_ptr->duration) == 0);
}

int NedVoice::getSorting(NedChordOrRest *chord0, NedChordOrRest *chord1) {
	int pos0, pos1;

	if ((pos0 = g_list_index(m_chord_or_rests, chord0)) < 0) {
		NedResource::Abort("NedVoice::getSorting 1");
	}
	if ((pos1 = g_list_index(m_chord_or_rests, chord1)) < 0) {
		NedResource::Abort("NedVoice::getSorting 2");
	}
	if (pos0 == pos1) {
		NedResource::Abort("NedVoice::getSorting 3");
	}
	if (pos0 + 1 == pos1) {
		return SORTING_GREAT;
	}
	if (pos1 + 1 == pos0) {
		return SORTING_LESS;
	}
	return SORTING_NONE;
}

bool NedVoice::isFirst(NedChordOrRest *chord) {
	GList *lptr;
	if ((lptr = g_list_find(m_chord_or_rests, chord)) == NULL) {
		 NedResource::Abort("NedVoice::isFirst");
	}
	return (lptr == g_list_first(m_chord_or_rests));
}

bool NedVoice::isLast(NedChordOrRest *chord) {
	GList *lptr;
	if ((lptr = g_list_find(m_chord_or_rests, chord)) == NULL) {
		 NedResource::Abort("NedVoice::isLast");
	}
	return (lptr == g_list_last(m_chord_or_rests));
}

int NedVoice::getBeamCount() {
	return g_list_length(m_beam_list);
}

NedChordOrRest *NedVoice::getNextChordOrRest(NedChordOrRest *element) {
	GList *lptr;

	lptr = g_list_find(m_chord_or_rests, element);
	if ((lptr = g_list_next(lptr)) == NULL) {
		return NULL;
	}
	return (NedChordOrRest *) lptr->data;
}

void NedVoice::selectNextChord(NedChordOrRest *chord, int line, bool lyrics_required) {
	GList *lptr;
	NedSystem *other_system;

	getMainWindow()->m_selected_chord_or_rest = NULL;
	getMainWindow()->m_selected_note = NULL;
	if ((lptr = g_list_find(m_chord_or_rests, chord)) == NULL) {
		NedResource::Abort("NedVoice::selectNextChord");
	}
	for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_CHORD) {
			if ((!lyrics_required && !((NedChordOrRest *) lptr->data)->isTied()) || ((NedChordOrRest *) lptr->data)->getLyrics(line) != NULL) {
				break;
			}
		}
	}
	if (lptr == NULL) {
		other_system = getPage()->getNextSystem(getSystem());
		if (other_system != NULL) {
			lptr = other_system->getFirstChordOrRest(m_staff->getStaffNumber(), m_voice_nr, line, lyrics_required, true);
		}
	}
	if (lptr == NULL) {
		return;
	}
	getMainWindow()->m_selected_chord_or_rest = (NedChordOrRest *) lptr->data;
	getMainWindow()->m_selected_note = ((NedChordOrRest *) lptr->data)->getFirstNote();
}

void NedVoice::selectPreviousChord(NedChordOrRest *chord, int line, bool lyrics_required) {
	GList *lptr;
	NedSystem *other_system;

	getMainWindow()->m_selected_chord_or_rest = NULL;
	getMainWindow()->m_selected_note = NULL;
	if ((lptr = g_list_find(m_chord_or_rests, chord)) == NULL) {
		NedResource::Abort("NedVoice::selectPreviousChord");
	}
	for (lptr = g_list_previous(lptr); lptr; lptr = g_list_previous(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_CHORD) {
			if (!lyrics_required || ((NedChordOrRest *) lptr->data)->getLyrics(line) != NULL) {
				break;
			}
		}
	}
	if (lptr == NULL) {
		other_system = getPage()->getPreviousSystem(getSystem());
		if (other_system != NULL) {
			lptr = other_system->getLastChordOrRest(m_staff->getStaffNumber(), m_voice_nr, line, lyrics_required, true);
		}
	}
	if (lptr == NULL) {
		return;
	}
	getMainWindow()->m_selected_chord_or_rest = (NedChordOrRest *) lptr->data;
	getMainWindow()->m_selected_note = ((NedChordOrRest *) lptr->data)->getFirstNote();
}

NedChordOrRest *NedVoice::getPreviousChordOrRest(NedChordOrRest *element) {
	GList *lptr;

	lptr = g_list_find(m_chord_or_rests, element);
	if ((lptr = g_list_previous(lptr)) == NULL) {
		return NULL;
	}
	return (NedChordOrRest *) lptr->data;
}

void NedVoice::removeBeam(NedBeam *beam) {
	GList *lptr;
	if ((lptr = g_list_find(m_beam_list, beam)) == NULL) {
		NedResource::Abort("NedVoice::removeBeam");
	}
	m_beam_list = g_list_delete_link(m_beam_list, lptr);
}

void NedVoice::addBeam(NedBeam *beam) {
	m_beam_list = g_list_append(m_beam_list, beam);
}

double NedVoice::findTimePos(unsigned int time) {
	GList *lptr;
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getTime() == time) {
			return ((NedChordOrRest *) lptr->data)->getXPos();
		}
		else if (((NedChordOrRest *) lptr->data)->getTime() > time) {
			lptr = g_list_previous(lptr);
			if (lptr != NULL) {
				return ((NedChordOrRest *) lptr->data)->getXPos();
			}
			else {
				return 0;
			}
		}
	}
	return -1;
}

void NedVoice::assignMidiTimes() {
	GList *lptr;
	unsigned long long midi_time = (getPage()->getPageNumber() == 0 && getSystem()->getSystemNumber() == 0) ? getMainWindow()->getUpBeatInverse() : 0;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setMidiTime(midi_time);
		midi_time += ((NedChordOrRest *) lptr->data)->getDuration();
		if (!((NedChordOrRest *) lptr->data)->isRest()) continue;
		((NedChordOrRest *) lptr->data)->resetYPos();
	}
}
		
int NedVoice::assignElementsToMeasures(NedMeasure *measures, int offs, bool use_upbeat) {
	GList *lptr, *lptr2;
	int curr_measure = 0;
	bool first_measure_has_only_rests = true;

	if (use_upbeat && m_voice_nr > 0) { // special handling of rests of first measure
		for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
			if (((NedChordOrRest *) lptr->data)->getMidiTime() >= measures[0].midi_end) {
				if (first_measure_has_only_rests) {
					for (lptr2 = g_list_first(m_chord_or_rests); lptr2 != lptr; lptr2 = g_list_next(lptr2)) {
						if ((((NedChordOrRest *) lptr2->data)->getType() & TYPE_REST) != 0) {
							((NedChordOrRest *) lptr2->data)->setHidden();
						}
					}
				}
				break;
			}
			if ((((NedChordOrRest *) lptr->data)->getType() & TYPE_REST) == 0) {
				first_measure_has_only_rests = false;
			}
		}
	}

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_KEYSIG) continue; // can happen during do_import_from_other (testLineOverflow)
		while (((NedChordOrRest *) lptr->data)->getMidiTime() >= measures[curr_measure].midi_end) {
			curr_measure++;
		}
		if (((NedChordOrRest *) lptr->data)->getMidiTime() >= measures[curr_measure].midi_end) continue;
		if (use_upbeat && m_voice_nr > 0 && curr_measure == 0) {
			if ((((NedChordOrRest *) lptr->data)->getType() & TYPE_REST) == 0) {
				first_measure_has_only_rests = false;
			}
		}
		if ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_CHORD | TYPE_REST)) != 0) {
			measures[curr_measure].m_position_array.insertElement(
				(NedChordOrRest *) lptr->data, offs, measures[curr_measure].getMeasureNumber());
		}
		else if ((((NedChordOrRest *) lptr->data)->getType() & TYPE_GRACE) != 0) {
			measures[curr_measure].m_position_array.insertGraceElement(
				(NedChordOrRest *) lptr->data, offs, measures[curr_measure].getMeasureNumber());
		}
		((NedChordOrRest *) lptr->data)->m_measure = &(measures[curr_measure]);
	}
	return curr_measure;
}

void NedVoice::insertIntoSystem(GList *chords_or_rests_to_move) {
	GList *lptr;
	NedBeam *b;
	int pos = 0;
	for (lptr = g_list_first(chords_or_rests_to_move); lptr; lptr = g_list_next(lptr)) {
		if ((b = ((NedChordOrRest *) lptr->data)->getBeam()) != NULL) {
			delete b;
		}
		((NedChordOrRest *) lptr->data)->setVoice(this);
		m_chord_or_rests = g_list_insert(m_chord_or_rests, lptr->data, pos++);
	}
}

void NedVoice::appendAtSystem(GList *chords_or_rests_to_move) {
	GList *lptr;
	NedBeam *b;
	for (lptr = g_list_first(chords_or_rests_to_move); lptr; lptr = g_list_next(lptr)) {
		if ((b = ((NedChordOrRest *) lptr->data)->getBeam()) != NULL) {
			delete b;
		}
		((NedChordOrRest *) lptr->data)->setVoice(this);
		m_chord_or_rests = g_list_append(m_chord_or_rests, lptr->data);
	}
}


void NedVoice::deleteNoteGroup(GList *chords_or_rests_to_move) {
	GList *lptr, *lptr2;
	for(lptr = g_list_first(chords_or_rests_to_move); lptr; lptr = g_list_next(lptr)) {
		if ((lptr2 = g_list_find(m_chord_or_rests, lptr->data)) == NULL) {
			NedResource::Abort("NedVoice::deleteNoteGroup");
		}
		m_chord_or_rests = g_list_delete_link(m_chord_or_rests, lptr2);
	}
}


#ifdef AAA
void NedVoice::computeBeams(int *lyrics_lines, double *topy, double *boty) {
	GList *lptr;
	double ty, by;
	for(lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		((NedBeam *) lptr->data)->computeBeam(m_stem_direction);
		
	}
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->getTopBotY(lyrics_lines, &ty, &by);
		if (ty < *topy) *topy = ty;
		if (by > *boty) *boty = by;
	}
}

#else

void NedVoice::computeBeams(int *lyrics_lines, double *topy, double *boty/* , bool *topFix, bool *botFix */, int staff_voice_offs) {
	GList *lptr;
	double ty, by;
	for(lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		((NedBeam *) lptr->data)->computeBeam(staff_voice_offs);
		
	}
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_KEYSIG) continue; // can happen during do_import_from_other (testLineOverflow)
		((NedChordOrRest *) lptr->data)->getTopBotY(lyrics_lines, &ty, &by /*, topFix, botFix*/);
		if (ty < *topy) *topy = ty;
		if (by > *boty) *boty = by;
	}
}
#endif

void NedVoice::computeTies() {
	GList *lptr;
		
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->computeTies();
	}
}
void NedVoice::computeTuplets(double *topy, double *boty) {
	GList *lptr;
	GList *tuplet_list = NULL;
	int tuplet_val = 0;
	NedTuplet *tuplet;
	//double ty, by;
	for(lptr = g_list_first(m_tuplet_list); lptr; lptr = g_list_next(lptr)) {
		delete ((NedTuplet *) lptr->data);
	}
	g_list_free(m_tuplet_list);
	m_tuplet_list = NULL;
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getTupletVal() > 0) {
			if (tuplet_val == 0) {
				tuplet_val = ((NedChordOrRest *) lptr->data)->getTupletVal();
				tuplet_list = g_list_append(tuplet_list, lptr->data);
			}
			else if (((NedChordOrRest *) lptr->data)->getTupletVal() == tuplet_val) {
				tuplet_list = g_list_append(tuplet_list, lptr->data);
				if (((NedChordOrRest *) lptr->data)->hasLastTupletFlag()) {
					tuplet = new NedTuplet(tuplet_val, tuplet_list, topy, boty);
					 m_tuplet_list = g_list_append(m_tuplet_list, tuplet);
					 tuplet_val = 0;
					 tuplet_list = NULL;
				}
			}
			else {
				tuplet = new NedTuplet(tuplet_val, tuplet_list, topy, boty);
				m_tuplet_list = g_list_append(m_tuplet_list, tuplet);
				tuplet_val = 0;
				tuplet_list = NULL;
			}
		}
		else if (tuplet_val > 0) {
			tuplet = new NedTuplet(tuplet_val, tuplet_list, topy, boty);
			m_tuplet_list = g_list_append(m_tuplet_list, tuplet);
			tuplet_val = 0;
			tuplet_list = NULL;
		}
	}
			
	/*
	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->getTopBotY(&ty, &by);
		if (ty < *topy) *topy = ty;
		if (by > *boty) *boty = by;
	}
	*/
}

	
void NedVoice::prepareMutedReplay() {
	GList *lptr;
	NedChordOrRest *chord_or_rest;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;

		if (chord_or_rest->getType() & (TYPE_CHORD | TYPE_REST)) {
			chord_or_rest->prepareMutedReplay();
		}
	}
}

void NedVoice::resetActiveFlags() {
	GList *lptr;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (!(((NedChordOrRest *) lptr->data)->getType() & (TYPE_CHORD | TYPE_GRACE))) continue;
		((NedChordOrRest *) lptr->data)->resetActiveFlags();
	}
}

void NedVoice::prepareReplay(int voice_idx) {
	GList *lptr, *lptr2;
	int clef, keysig, octave_shift;
	unsigned int grace_time = 0;
	NedChordOrRest *chord_or_rest;
	int measurenumber = -1;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		//NedMeasure *measure = getSystem()->getMeasure(chord_or_rest->getMidiTime());
		NedMeasure *measure = chord_or_rest->m_measure;
		if (measure->getMeasureNumber() != measurenumber) {
			measurenumber = measure->getMeasureNumber();
			NedResource::addMeasureEntry(measure);
		}

		if (chord_or_rest->getType() == TYPE_CHORD) {
			m_staff->getCurrentClefAndKeysig(chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
			grace_time = 0;
			for (lptr2 = g_list_next(lptr); lptr2; lptr2 = g_list_next(lptr2)) {
				if (((NedChordOrRest *) lptr2->data)->getType() == TYPE_GRACE) {
					grace_time += GRACE_DURATION;
					continue;
				}
				break;
			}
			if (grace_time > chord_or_rest->getDuration() / 2) {
				grace_time = chord_or_rest->getDuration() / 2;
			}
			chord_or_rest->prepareReplay(clef, keysig, octave_shift, voice_idx, grace_time);
		}
		else if (chord_or_rest->getType() == TYPE_GRACE) {
			if (grace_time == 0) {
				continue;
			}
			m_staff->getCurrentClefAndKeysig(chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
			chord_or_rest->prepareReplay(clef, keysig, octave_shift, voice_idx, grace_time);
			if (grace_time > GRACE_DURATION) {
				grace_time -= GRACE_DURATION;
			}
			else {
				grace_time = 0;
			}
		}
		if (chord_or_rest->getType() == TYPE_REST) {
			grace_time = 0;
			for (lptr2 = g_list_next(lptr); lptr2; lptr2 = g_list_next(lptr2)) {
				if (((NedChordOrRest *) lptr2->data)->getType() == TYPE_GRACE) {
					grace_time += ((NedChordOrRest *) lptr2->data)->getLength();
					continue;
				}
				break;
			}
			if (grace_time > chord_or_rest->getDuration() / 2) {
				grace_time = chord_or_rest->getDuration() / 2;
			}
			chord_or_rest->prepareReplay(clef, keysig, octave_shift, voice_idx, grace_time);
		}
	}
}

void NedVoice::setInternalPitches() {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		((NedChordOrRest *) lptr->data)->setInternalPitch(0);
	}
}

void NedVoice::adjustAccidentals() {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		((NedChordOrRest *) lptr->data)->adjustAccidentals();
	}
}

void NedVoice::setInternalPitch(int pitchdist, GList *selected_group) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		if (selected_group) {
			if (g_list_find(selected_group, lptr->data) == NULL) continue;
		}
		((NedChordOrRest *) lptr->data)->setInternalPitch(pitchdist);
	}
}

void NedVoice::adjustAccidentals(int pitchdist, GList *selected_group) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		if (selected_group) {
			if (g_list_find(selected_group, lptr->data) == NULL) continue;
		}
		((NedChordOrRest *) lptr->data)->adjustAccidentals();
	}
}

void NedVoice::changeAccidentals(NedCommandList *command_list, int preferred_offs, GList *selected_group) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		if (selected_group) {
			if (g_list_find(selected_group, lptr->data) == NULL) continue;
		}
		((NedChordOrRest *) lptr->data)->changeAccidentals(command_list, preferred_offs);
	}
}


void NedVoice::hideRests(NedCommandList *command_list, bool unhide) {
	GList *lptr;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->hideRest(command_list, unhide);
	}
}

void NedVoice::shiftNotes(unsigned long long start_time, int linedist, unsigned long long stop_time, bool stop_at) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (stop_at) {
			if (((NedChordOrRest *) lptr->data)->getMidiTime() >= stop_time) return;
		}
		if (((NedChordOrRest *) lptr->data)->getMidiTime() < start_time) continue;
		if (((NedChordOrRest *) lptr->data)->getType() == TYPE_REST) continue;
		((NedChordOrRest *) lptr->data)->shiftNotes(linedist);
	}
}
	

void NedVoice::removeUnneededAccidentals(GList *selected_group) {
	GList *lptr;
	int clef, keysig, octave_shift;
	NedChordOrRest *chord_or_rest;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if (selected_group) {
			if (g_list_find(selected_group, lptr->data) == NULL) continue;
		}
		chord_or_rest = (NedChordOrRest *) lptr->data;
		if (chord_or_rest->getType() == TYPE_CHORD) {
			m_staff->getCurrentClefAndKeysig(chord_or_rest->getMidiTime(), &clef, &keysig, &octave_shift);
			chord_or_rest->removeUnneededAccidentals(clef, keysig, octave_shift);
		}
	}
}

void NedVoice::findAccidentals(char offs_array[115], NedMeasure *meas_info, unsigned long long midi_time, bool including) {
	GList *lptr;

	for (lptr = g_list_last(m_chord_or_rests); lptr; lptr = g_list_previous(lptr)) {
		if (((NedChordOrRest *) lptr->data)->getMidiTime() < meas_info->midi_start) break;
		if (including) {
			if (((NedChordOrRest *) lptr->data)->getMidiTime() > midi_time) continue;
		}
		else {
			if (((NedChordOrRest *) lptr->data)->getMidiTime() >= midi_time) continue;
		}
		((NedChordOrRest *) lptr->data)->setOffset(offs_array);
	}
}

void NedVoice::saveVoice(FILE *fp) {
	GList *lptr;
	bool BEAMS_written = FALSE;
	bool TIES_written = FALSE;
	bool TUPLETS_written = FALSE;
	bool FREE_PLACEABLES_written = FALSE;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->saveChordOrRest(fp);
	}
	fprintf(fp, "\n");
	for(lptr = g_list_first(m_beam_list); lptr; lptr = g_list_next(lptr)) {
		if (!BEAMS_written) {
			fprintf(fp, "BEAMS\n");
			BEAMS_written = TRUE;
		}
		((NedBeam *) lptr->data)->saveBeam(fp);
	}
	int xx;
	for(xx = 0, lptr = g_list_first(m_tuplet_list); lptr; lptr = g_list_next(lptr), xx++) {
		if (!TUPLETS_written) {
			fprintf(fp, "TUPLETS\n");
			TUPLETS_written = TRUE;
		}
		((NedTuplet *) lptr->data)->saveTuplet(fp);
	}
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->saveTies(fp, &TIES_written);
	}
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->saveFreePlaceables(fp, &FREE_PLACEABLES_written);
	}
	if (FREE_PLACEABLES_written) {
		putc('\n', fp);
	}
}

void NedVoice::restoreVoice(FILE *fp) {
	char buffer[128];
	bool beams_read = FALSE;
	bool ties_read = FALSE;
	bool tuplets_read = FALSE;
	bool free_placeables_read = FALSE;
	NedChordOrRest *chord_or_rest;

	while (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) return;
			NedResource::m_error_message = "( TUPLETS, BEAMS, TIES, FREE_PLACEABLES, VOICE, STAFF SYSTEM or PAGE expected";
			return;
		}
		if (!strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE")) {
			NedResource::unreadWord(buffer);
			return;
		}
		if (!strcmp(buffer, "BEAMS")) {
			if (beams_read) {
				NedResource::m_error_message = "BEAMS twice";
				return;
			}
			if (ties_read) {
				NedResource::m_error_message = "BEAMS after TIES";
				return;
			}
			beams_read = TRUE;
			restoreBeams(fp);
			if (NedResource::m_error_message != NULL) {
				return;
			}
			continue;
		}
		if (!strcmp(buffer, "TIES")) {
			if (ties_read) {
				NedResource::m_error_message = "TIES twice";
				return;
			}
			ties_read = TRUE;
			restoreTies(fp);
			if (NedResource::m_error_message != NULL) {
				return;
			}
			continue;
		}
		if (!strcmp(buffer, "TUPLETS")) {
			if (tuplets_read) {
				NedResource::m_error_message = "TUPLETS twice";
				return;
			}
			tuplets_read = TRUE;
			restoreTuplets(fp);
			if (NedResource::m_error_message != NULL) {
				return;
			}
			continue;
		}
		if (!strcmp(buffer, "FREE_PLACEABLES")) {
			if (free_placeables_read) {
				NedResource::m_error_message = "FREE_PLACEABLES twice";
				return;
			}
			free_placeables_read = TRUE;
			restoreFreePlaceables(fp);
			if (NedResource::m_error_message != NULL) {
				return;
			}
			continue;
		}
		if (!strcmp(buffer, "(")) {
			if (beams_read) {
				NedResource::m_error_message = "Notes after BEAMS";
				return;
			}
			if (ties_read) {
				NedResource::m_error_message = "Notes after TIES";
				return;
			}
			if ((chord_or_rest = NedChordOrRest::restoreChordOrRest(fp, this)) != NULL) {
				m_chord_or_rests = g_list_append(m_chord_or_rests, chord_or_rest);
			}
			else {
				return;
			}
		}
		else {
			NedResource::m_error_message = "( BEAMS, TIES, VOICE, STAFF SYSTEM or PAGE expected";
			return;
		}
	}
}
				
void NedVoice::restoreBeams(FILE *fp) {
	char buffer[128];
	int marker;

	if (!NedResource::readWord(fp, buffer)) {
		if (feof(fp)) return;
		NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM, FREE_PLACEABLES,  or PAGE expected";
		return;
	}
	if (!strcmp(buffer, "TUPLETS") || !strcmp(buffer, "TIES") || !strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE") ||
		!strcmp(buffer, "FREE_PLACEABLES")) {
		NedResource::unreadWord(buffer);
		return;
	}
	if (strcmp(buffer, "(")) {
		NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM or PAGE expected(1)";
		return;
	}
	while (!strcmp(buffer, "(")) {
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "u or d expected";
			return;
		}
		if (!NedResource::readInt(fp, &marker)) {
			NedResource::m_error_message = "marker expected(1)";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
			NedResource::m_error_message = ", or ) expected";
			return;
		}
		while (strcmp(buffer, ")")) {
			if (buffer[0] != ',') {
				NedResource::m_error_message = ", or ) expected";
				return;
			}
			if (!NedResource::readInt(fp, &marker)) {
				NedResource::m_error_message = "marker expected(2)";
				return;
			}
			if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
				NedResource::m_error_message = ", or ) expected";
				return;
			}
		}
		if (strcmp(buffer, ")")) {
			NedResource::m_error_message = ", or ) expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) return;
			NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM or PAGE expected(2)";
			return;
		}
		if (!strcmp(buffer, "TUPLETS") || !strcmp(buffer, "TIES") || !strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") ||
			!strcmp(buffer, "FREE_PLACEABLES") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE")) {
			NedResource::unreadWord(buffer);
			return;
		}
		else if (strcmp(buffer, "(")) {
			NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM or PAGE expected(3)";
			return;
		}
	}
}

void NedVoice::restoreTuplets(FILE *fp) {
	char buffer[128];
	int marker;
	int tuplet_val;
	NedChordOrRest *chord_or_rest;

	if (!NedResource::readWord(fp, buffer)) {
		if (feof(fp)) return;
		NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM, FREE_PLACEABLES or PAGE expected";
		return;
	}
	if (!strcmp(buffer, "TIES") || !strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE") ||
		!strcmp(buffer, "FREE_PLACEABLES")) {
		NedResource::unreadWord(buffer);
		return;
	}
	if (strcmp(buffer, "(")) {
		NedResource::m_error_message = "( TIES VOICE STAFF SYSTEM or PAGE expected(4)";
		return;
	}

	while (!strcmp(buffer, "(")) {
		if (!NedResource::readInt(fp, &tuplet_val) || tuplet_val < 2 || tuplet_val > 13) {
			NedResource::m_error_message = "tuplet val expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[0] != ';' || buffer[1] != '\0') {
			NedResource::m_error_message = "; expected";
			return;
		}
		if (!NedResource::readInt(fp, &marker)) {
			NedResource::m_error_message = "marker expected(1)";
			return;
		}
		chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
		chord_or_rest->setTupletVal(tuplet_val);
		if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
			NedResource::m_error_message = ", or ) expected";
			return;
		}
		while (strcmp(buffer, ")")) {
			if (buffer[0] != ',') {
				NedResource::m_error_message = ", or ) expected";
				return;
			}
			if (!NedResource::readInt(fp, &marker)) {
				NedResource::m_error_message = "marker expected(2)";
				return;
			}
			chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
			chord_or_rest->setTupletVal(tuplet_val);
			if (!NedResource::readWord(fp, buffer) || buffer[1] != '\0') {
				NedResource::m_error_message = ", or ) expected";
				return;
			}
		}
		chord_or_rest->setLastTupletFlag(TRUE);
		if (strcmp(buffer, ")")) {
			NedResource::m_error_message = ", or ) expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) return;
			NedResource::m_error_message = "( FREE_PLACEABLES TIES TUPLETS VOICE STAFF SYSTEM or PAGE expected";
			return;
		}
		if (!strcmp(buffer, "TIES") || !strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE") || !strcmp(buffer, "FREE_PLACEABLES")) {
			NedResource::unreadWord(buffer);
			return;
		}
		else if (strcmp(buffer, "(")) {

			NedResource::m_error_message = "( FREE_PLACEABLES TIES VOICE STAFF SYSTEM or PAGE expected";
			return;
		}
	}
}

void NedVoice::restoreTies(FILE *fp) {
	char buffer[128];
	int marker1, marker2;
	NedNote *note1, *note2;
	double tie_offs = 0.0;

	if (!NedResource::readWord(fp, buffer)) {
		if (feof(fp)) return;
		NedResource::m_error_message = "( VOICE STAFF SYSTEM, FREE_PLACEABLES, or PAGE expected";
		return;
	}
	if (!strcmp(buffer, "TUPLETS") ||!strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE") ||
	 !strcmp(buffer, "FREE_PLACEABLES")) {
		NedResource::unreadWord(buffer);
		return;
	}
	if (strcmp(buffer, "(")) {
		NedResource::m_error_message = "( VOICE STAFF SYSTEM or PAGE expected";
		return;
	}
	while (!strcmp(buffer, "(")) {
		if (!NedResource::readInt(fp, &marker1)) {
			NedResource::m_error_message = "address expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || buffer[0] != ',' || buffer[1] != '\0') {
			NedResource::m_error_message = ", expected";
			return;
		}
		if (!NedResource::readInt(fp, &marker2)) {
			NedResource::m_error_message = "address expected";
			return;
		}
		if (getMainWindow()->getFileVersion() > 8) {
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ',' || buffer[1] != '\0') {
				NedResource::m_error_message = ", expected";
				return;
			}
			if (!NedResource::readFloat(fp, &tie_offs)) {
				NedResource::m_error_message = "tie offset expected";
				return;
			}
		}
		if (!NedResource::readWord(fp, buffer) || buffer[0] != ')' || buffer[1] != '\0') {
			NedResource::m_error_message = ") expected";
			return;
		}
		note1 = (NedNote *) NedResource::getAdressOfMarker(marker1);
		note2 = (NedNote *) NedResource::getAdressOfMarker(marker2);
		NedNote::setTies(note1, note2);
		note1->setTieOffset(tie_offs);
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) return;
			NedResource::m_error_message = "( VOICE STAFF SYSTEM or PAGE expected";
			return;
		}
		if (!strcmp(buffer, "VOICE") || !strcmp(buffer, "STAFF") || !strcmp(buffer, "SYSTEM") || !strcmp(buffer, "PAGE") || !strcmp(buffer, "TUPLETS") || !strcmp(buffer, "FREE_PLACEABLES") ) {
			NedResource::unreadWord(buffer);
			return;
		}
		if (strcmp(buffer, "(")) {
			NedResource::m_error_message = "( FREE_PLACEABLES TUPLETS VOICE STAFF SYSTEM or PAGE expected";
			return;
		}
	}
}

void NedVoice::restoreFreePlaceables(FILE *fp) {
	char buffer[128];
	int marker, i;
	double x, y;
	unsigned int kind, tempo, midi_vol;
	int sign_kind, midi_pgm;
	char chordcode[128];
	unsigned int channel;
	int en_chn;
	int chordname;
	int dot = 0;
	int chordstatus;
	NedVolumeSign *volsign;
	NedChordOrRest *chord_or_rest;
	NedSlurPoint *sp[3];
	NedLinePoint *startp, *endp, *midp = NULL;
	int vol_diff, octave_shift, tempo_diff;
	char ffamily[1024], xtext[1024], *xfamily;
	char root_text[1024], down_text[1024], up_text[1024];
	double fsize;
	double fontsize = 0.5;
	int en_vol, en_tpo;
	int slant, weight;
	int segno_sign = 0, anchor = 0;
	PangoStyle cslant;
	PangoWeight cweight;

	if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "(")) {
		NedResource::m_error_message = "( expected";
		return;
	}
	while (!strcmp(buffer, "(")) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "<")) {
			NedResource::m_error_message = "< expected";
			return;
		}
		if (!NedResource::readInt(fp, &marker)) {
			NedResource::m_error_message = "address expected";
			return;
		}
		chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
		if (!NedResource::readFloat(fp, &x)) {
			NedResource::m_error_message = "x value expected";
			return;
		}
		if (!NedResource::readFloat(fp, &y)) {
			NedResource::m_error_message = "y value expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ">")) {
			NedResource::m_error_message = "> expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "Free Placeable Spec expected";
			return;
		}
		if (!strcmp(buffer, "VOLUME_SIGN")) {
			if (!NedResource::readWord(fp, buffer)) {
				NedResource::m_error_message = "Volume spec expected";
				return;
			}
			if (!strcmp(buffer, "VOL_PPP")) {
				volsign = new NedVolumeSign(VOL_PPP, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_PP")) {
				volsign = new NedVolumeSign(VOL_PP, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_P")) {
				volsign = new NedVolumeSign(VOL_P, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_MP")) {
				volsign = new NedVolumeSign(VOL_MP, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_SP")) {
				volsign = new NedVolumeSign(VOL_SP, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_MF")) {
				volsign = new NedVolumeSign(VOL_MF, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_SF")) {
				volsign = new NedVolumeSign(VOL_SF, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_F")) {
				volsign = new NedVolumeSign(VOL_F, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_FF")) {
				volsign = new NedVolumeSign(VOL_FF, chord_or_rest, x, y, true);
			}
			else if (!strcmp(buffer, "VOL_FFF")) {
				volsign = new NedVolumeSign(VOL_FFF, chord_or_rest, x, y, true);
			}
			else {
				NedResource::m_error_message = "Wrong volume sig";
				return;
			}
			if (getMainWindow()->getFileVersion() > 6) {
				if (!NedResource::readInt(fp, (int *) &midi_vol) || midi_vol < 0 || midi_vol > 127) {
					NedResource::m_error_message = "Midi volume expected";
					return;
				}
				volsign->m_midi_volume = midi_vol;
			}
				
		}
		else if (!strcmp(buffer, "TEMPO_SIGN")) {
			if (!NedResource::readUnsignedInt(fp, &kind) || (kind != WHOLE_NOTE && kind != NOTE_2 &&
					kind != NOTE_4 && kind != NOTE_8 && kind != NOTE_16) ) {
				NedResource::m_error_message = "TEMPO kind expected";
				return;
			}
			if (getMainWindow()->getFileVersion() > 21) {
				if (!NedResource::readInt(fp, &dot) || dot < 0 || dot > 1) { 
					NedResource::m_error_message = "dot information expected";
					return;
				}
			}
			if (!NedResource::readUnsignedInt(fp, &tempo) || tempo < 10 || tempo > 999) {
				NedResource::m_error_message = "TEMPO val expected";
				return;
			}
			if (dot) {
				kind = 3 * kind / 2;
			}
			new NedTempoSign(kind, tempo, chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "FREE_SPACER")) {
			new NedFreeSpacer(getMainWindow(), chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "FREE_CHORD")) {
			if (!NedResource::readWordOfLength(fp, chordcode)) {
				NedResource::m_error_message = "CHORD_CODE kind expected";
				return;
			}
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ',' || buffer[1] != '\0') {

				NedResource::m_error_message = ", expected";
				return;
			}
			if (!NedResource::readInt(fp, &chordname) || chordname < 0 || chordname > 1) {
				NedResource::m_error_message = "chordname val expected";
				return;
			}
			if (!NedResource::readWord(fp, buffer) || buffer[0] != ',' || buffer[1] != '\0') {
				NedResource::m_error_message = "expected";
				return;
			}
			if (!NedResource::readHex(fp, &chordstatus)) {
				NedResource::m_error_message = "chordstatus val expected";
				return;
			}
			new NedFreeChord(chordcode, chordname, chordstatus, getMainWindow(), chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "FREE_SIGN")) {
			if (!NedResource::readInt(fp, &sign_kind) || sign_kind < SIGN_SEGNO || sign_kind > SIGN_CODA) {
				NedResource::m_error_message = "SIGN kind expected";
				return;
			}
			new NedFreeSign(sign_kind, chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "FREE_CHORD_NAME")) {
			if (!NedResource::readWordOfLength(fp, root_text)) {
				NedResource::m_error_message = "root text expected";
				return;
			}
			if (!NedResource::readWordOfLength(fp, down_text)) {
				NedResource::m_error_message = "down text expected";
				return;
			}
			if (!NedResource::readWordOfLength(fp, up_text)) {
				NedResource::m_error_message = "up text expected";
				return;
			}
			fontsize = 0.5;
			if (getMainWindow()->getFileVersion() > 17) {
				if (!NedResource::readFloat(fp, &fontsize)) {
					NedResource::m_error_message = "font size expected(2)";
					return;
				}
			}
			new NedChordName(getMainWindow()->getDrawingArea(), root_text, down_text, up_text, fontsize,
				chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "FREE_TEXT")) {
			if (!NedResource::readWordOfLength(fp, xtext)) {
				NedResource::m_error_message = "text expected";
				return;
			}
			if (!NedResource::readWordOfLength(fp, ffamily)) {
				NedResource::m_error_message = "font family expected";
				return;
			}
			if ((xfamily = (char *) g_try_malloc(strlen(ffamily) + 1)) == NULL) {
				NedResource::Abort("NedVoice::restoreFreePlaceables: allocation failed");
			}
			strcpy(xfamily, ffamily);
			if (!NedResource::readFloat(fp, &fsize)) {
				NedResource::m_error_message = "font size expected";
				return;
			}
			if (!NedResource::readInt(fp, &en_vol)) {
				NedResource::m_error_message = "enable volume expected";
				return;
			}
			if (!NedResource::readUnsignedInt(fp, &midi_vol)) {
				NedResource::m_error_message = "volume expected";
				return;
			}
			if (!NedResource::readInt(fp, &en_tpo)) {
				NedResource::m_error_message = "enable tempo expected";
				return;
			}
			if (!NedResource::readUnsignedInt(fp, &tempo)) {
				NedResource::m_error_message = "tempo expected";
				return;
			}
			channel = 1;
			en_chn = false;
			segno_sign = 0;
			if (getMainWindow()->getFileVersion() > 11) {
				if (!NedResource::readHex(fp, &segno_sign)) {
					NedResource::m_error_message = "segno_sign expected";
					return;
				}
				if (getMainWindow()->getFileVersion() > 20) {
					if (!NedResource::readInt(fp, &midi_pgm)) {
						NedResource::m_error_message = "midi_pgm expected";
						return;
					}
				}
				else midi_pgm = -1;
				if (getMainWindow()->getFileVersion() > 22) {
					if (!NedResource::readInt(fp, &en_chn)) {
						NedResource::m_error_message = "enable channel expected";
						return;
					}
					if (!NedResource::readUnsignedInt(fp, &channel)) {
						NedResource::m_error_message = "channel expected";
						return;
					}
				}
				if (!NedResource::readInt(fp, &anchor)) {
					NedResource::m_error_message = "anchor expected";
					return;
				}
			}
			
			if (!NedResource::readInt(fp, &slant)) {
				NedResource::m_error_message = "slant expected";
				return;
			}
			switch (slant) {
				case WRITTEN_SLANT_NORMAL: cslant = PANGO_STYLE_NORMAL; break;
				case WRITTEN_SLANT_ITALIC: cslant = PANGO_STYLE_ITALIC; break;
				case WRITTEN_SLANT_OBLIQUE: cslant = PANGO_STYLE_OBLIQUE; break;
				default: NedResource::m_error_message = "unknown slant"; return;
			}
			if (!NedResource::readInt(fp, &weight)) {
				NedResource::m_error_message = "weight expected";
				return;
			}
			switch (weight) {
				case WRITTEN_WEIGHT_NORMAL: cweight = PANGO_WEIGHT_NORMAL; break;
				case WRITTEN_WEIGHT_BOLD: cweight = PANGO_WEIGHT_BOLD; break;
				default: NedResource::m_error_message = "unknown weight"; return;
			}
			new NedFreeText(xtext, getMainWindow()->getDrawingArea(), anchor, tempo, en_tpo != 0, midi_vol, en_vol != 0, segno_sign, 
				midi_pgm, channel, en_chn != 0, xfamily, fsize, cslant, cweight,
				chord_or_rest, x, y, true);
		}
		else if (!strcmp(buffer, "SLUR")) {
			sp[0] = new NedSlurPoint(NULL, 0, chord_or_rest, x, y, true);
			for (i = 1; i < 3; i++) {
				if (!NedResource::readInt(fp, &marker)) {
					NedResource::m_error_message = "address expected";
					return;
				}
				chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
				if (!NedResource::readFloat(fp, &x)) {
					NedResource::m_error_message = "x value expected";
					return;
				}
				if (!NedResource::readFloat(fp, &y)) {
					NedResource::m_error_message = "y value expected";
					return;
				}
				sp[i] = new NedSlurPoint(NULL, i, chord_or_rest, x, y, true);
			}
			new NedSlur(sp);
		}
		else if (!strncmp(buffer, "LINE_", strlen("LINE_"))) {
			startp = new NedLinePoint(NULL, 0, chord_or_rest, x, y, true);
			if (!strcmp(buffer, "LINE_CRESCENDO") || !strcmp(buffer, "LINE_DECRESCENDO")) {
				if (!NedResource::readInt(fp, &vol_diff)) {
					NedResource::m_error_message = "vol_diff expected";
					return;
				}
			}
			if (!strcmp(buffer, "LINE_ACCELERATO") || !strcmp(buffer, "LINE_RITARDANDO")) {
				if (!NedResource::readInt(fp, &tempo_diff)) {
					NedResource::m_error_message = "tempo_diff expected";
					return;
				}
			}
			if (!strcmp(buffer, "LINE_OCTAVATION")) {
				if (!NedResource::readInt(fp, &octave_shift)) {
					NedResource::m_error_message = "octave_shift expected";
					return;
				}
			}
			if (!strcmp(buffer, "LINE_ACCELERATO") || !strcmp(buffer, "LINE_RITARDANDO")) {
				if (!NedResource::readInt(fp, &marker)) {
					NedResource::m_error_message = "address expected";
					return;
				}
				chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
				if (!NedResource::readFloat(fp, &x)) {
					NedResource::m_error_message = "x value expected";
					return;
				}
				if (!NedResource::readFloat(fp, &y)) {
					NedResource::m_error_message = "y value expected";
					return;
				}
				midp = new NedLinePoint(NULL, 1, chord_or_rest, x, y, true);
			}
			if (!NedResource::readInt(fp, &marker)) {
				NedResource::m_error_message = "address expected";
				return;
			}
			chord_or_rest = (NedChordOrRest *) NedResource::getAdressOfMarker(marker);
			if (!NedResource::readFloat(fp, &x)) {
				NedResource::m_error_message = "x value expected";
				return;
			}
			if (!NedResource::readFloat(fp, &y)) {
				NedResource::m_error_message = "y value expected";
				return;
			}
			endp = new NedLinePoint(NULL, 1, chord_or_rest, x, y, true);
			if (!strcmp(buffer, "LINE_CRESCENDO")) {
				new NedCrescendo(startp, endp, false, vol_diff);
			}
			else if (!strcmp(buffer, "LINE_DECRESCENDO")) {
				new NedCrescendo(startp, endp, true, vol_diff);
			}
			else if (!strcmp(buffer, "LINE_OCTAVATION")) {
				new NedOctavation(startp, endp, NULL /* dummy */, octave_shift);
			}
			else if (!strcmp(buffer, "LINE_ACCELERATO")) {
				new NedAcceleration(startp, midp, endp, false, tempo_diff);
			}
			else if (!strcmp(buffer, "LINE_RITARDANDO")) {
				new NedAcceleration(startp, midp, endp, true, tempo_diff);
			}
			else {
				NedResource::m_error_message = "LINE_CRESCENDO, LINE_DECRESCENDO, LINE_ACCELERATO, LINE_RITARDANDO, or LINE_OCTAVATION expected";
				return;
			}
		}
		else {
			NedResource::m_error_message = "VOLUME_SIG, SLUR, TEMPO_SIGN, FREE_SIGN,  FREE_TEXT, FREE_SPACER, or FREE_CHORD  expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ")")) {
			NedResource::m_error_message = ") expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "( TUPLETS, BEAMS, TIES, FREE_PLACEABLES, VOICE, STAFF SYSTEM or PAGE expected";
		}
	}
	NedResource::unreadWord(buffer);
}

bool NedVoice::detectVoices(unsigned int *voice_mask, unsigned long long *e_time) {
	GList *lptr;
	bool not_empty = false;
	NedMeasure *measure;

	for(lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		if ((((NedChordOrRest *) lptr->data)->getType() == TYPE_REST && !((NedChordOrRest *) lptr->data)->isHidden()) || ((NedChordOrRest *) lptr->data)->getType() == TYPE_CHORD ||
			((NedChordOrRest *) lptr->data)->getType() == TYPE_GRACE) {
				*voice_mask |= (1 << m_voice_nr);
				//measure = getSystem()->getMeasure(((NedChordOrRest *) lptr->data)->getMidiTime());
				measure = ((NedChordOrRest *) lptr->data)->m_measure;
				if (((NedChordOrRest *) lptr->data)->getType() == TYPE_CHORD || ((NedChordOrRest *) lptr->data)->getType() == TYPE_GRACE) {
					not_empty = true;
					if (measure->midi_end > *e_time) {
						*e_time = measure->midi_end;
					}
				}

		}
	}
	return not_empty;
}

void NedVoice::exportLilypondSpecMeasure(FILE *fp, unsigned int special, SpecialMeasure *spec_measure, bool *in_alternative, int measure_number) {
	bool spec_written;

	spec_written = false;
	switch (special & START_TYPE_MASK) {
		case REP1START: fprintf(fp, "\n\t}\n\t\\alternative {\n\t{\n\t"), *in_alternative = true; spec_written = true; break;
	}
	switch (special & END_TYPE_MASK) {
		case REP2END: fprintf(fp, "\n\t}}\n\t"); *in_alternative = false; spec_written = true;break;
	}
	switch (special & REP_TYPE_MASK) {
		case REPEAT_CLOSE: fprintf(fp, "\n\t}\n\t"); spec_written = true;
					if ((special & START_TYPE_MASK) == REP2START) {
						fprintf(fp, "\n\t{\n\t");
					}
					else if (*in_alternative) {
						fprintf(fp, "\n\t {} }\n\t");
						*in_alternative = false;
					}
				break;
		case REPEAT_OPEN: fprintf(fp, "\n\t\\repeat volta 2 {\n\t");  spec_written = true; break;
		case REPEAT_OPEN_CLOSE: fprintf(fp, "\n\t}\n\t");
					if (*in_alternative) {
						fprintf(fp, "\n\t {} }\n\t");
						*in_alternative = false;
					}
				fprintf(fp, "\n\t\\repeat volta 2 {\n\t");  spec_written = true;
				break;
		case END_BAR:   fprintf(fp, "\n\t\\bar \"|.\"\n\t"); spec_written = true; break;
		case DOUBLE_BAR:   fprintf(fp, "\n\t\\bar \"||\"\n\t"); spec_written = true; break;
	}
	switch (special & TIMESIG) {
		case TIMESIG: fprintf(fp, " \\time %d/%d ", spec_measure->numerator, spec_measure->denominator);
				spec_written = true;
				break;
	}
	if (!spec_written) {
		fprintf(fp, "| %% %d\n\t", measure_number);
	}
}

void NedVoice::exportLilyGuitarChordnames(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
		unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr, *lptr2, *staff_elem_list;
	bool dummy4;
	NedMeasure *old_measure = NULL, *measure;
	SpecialMeasure *spec_measure;
	unsigned int special;
	bool in_tuplet = false;
	unsigned int dummy3;
	NedSlur *dummy2;
	int dummy5;
	bool first = true;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr), first = false) {
		//measure = getSystem()->getMeasure(((NedChordOrRest *) lptr->data)->getMidiTime());
		measure = ((NedChordOrRest *) lptr->data)->m_measure;
		if (measure != old_measure && measure->getMeasureNumber() > 1) {
			spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
			if (spec_measure != NULL && !first ) {
				special = spec_measure->type;
				exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
			}
			else {
				fprintf(fp, "| %% %d\n\t", measure->getMeasureNumber());
			}
		}
		if (last_system && ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) != 0) && ((NedChordOrRest *) lptr->data)->getMidiTime() >= end_time) {
			break;
		}
		if (m_voice_nr == 0) {
			staff_elem_list = m_staff->getStaffElements(((NedChordOrRest *) lptr->data)->getMidiTime());
			if (staff_elem_list != NULL) {
				for (lptr2 = g_list_first(staff_elem_list); lptr2; lptr2 = g_list_next(staff_elem_list)) {
					 ((NedChordOrRest *) lptr2->data)->exportLilyPond(fp, last_line, midi_len, &dummy2, &dummy3, &dummy4, &dummy4, &dummy5, false);
				}
				g_list_free(staff_elem_list);
			}
		}
		old_measure = measure;
		if (((NedChordOrRest *) lptr->data)->getTupletVal() && !in_tuplet) {
			fprintf(fp, " \\times %d/%d { ",  NedResource::m_tuplet_tab[((NedChordOrRest *) lptr->data)->getTupletVal()],
					((NedChordOrRest *) lptr->data)->getTupletVal());
			in_tuplet = true;
		}

		((NedChordOrRest *) lptr->data)->exportLilyGuitarChordnames(fp, last_line, midi_len);
		if (((NedChordOrRest *) lptr->data)->hasLastTupletFlag()) {
			fprintf(fp, " } ");
			in_tuplet = false;
		}
	}
	if (with_break) {
		fprintf(fp, "  \\break  ");
	}
	measure = getSystem()->getLastMeasure();
	spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
	if (spec_measure != NULL) {
		special = spec_measure->type;
		exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
	}
}

void NedVoice::exportLilyFreeChordName(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
		unsigned long long end_time, bool *in_alternative, bool with_break) {
	GList *lptr, *lptr2, *staff_elem_list;
	bool dummy4;
	NedMeasure *old_measure = NULL, *measure;
	SpecialMeasure *spec_measure;
	unsigned int special;
	bool in_tuplet = false;
	unsigned int dummy3;
	NedSlur *dummy2;
	int dummy5;
	bool first = true;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr), first = false) {
		//measure = getSystem()->getMeasure(((NedChordOrRest *) lptr->data)->getMidiTime());
		measure = ((NedChordOrRest *) lptr->data)->m_measure;
		if (measure != old_measure && measure->getMeasureNumber() > 1) {
			spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
			if (spec_measure != NULL && !first ) {
				special = spec_measure->type;
				exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
			}
			else {
				fprintf(fp, "| %% %d\n\t", measure->getMeasureNumber());
			}
		}
		if (last_system && ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) != 0) && ((NedChordOrRest *) lptr->data)->getMidiTime() >= end_time) {
			break;
		}
		if (m_voice_nr == 0) {
			staff_elem_list = m_staff->getStaffElements(((NedChordOrRest *) lptr->data)->getMidiTime());
			if (staff_elem_list != NULL) {
				for (lptr2 = g_list_first(staff_elem_list); lptr2; lptr2 = g_list_next(staff_elem_list)) {
					 ((NedChordOrRest *) lptr2->data)->exportLilyPond(fp, last_line, midi_len, &dummy2, &dummy3, &dummy4, &dummy4, &dummy5, false);
				}
				g_list_free(staff_elem_list);
			}
		}
		old_measure = measure;
		if (((NedChordOrRest *) lptr->data)->getTupletVal() && !in_tuplet) {
			fprintf(fp, " \\times %d/%d { ",  NedResource::m_tuplet_tab[((NedChordOrRest *) lptr->data)->getTupletVal()],
					((NedChordOrRest *) lptr->data)->getTupletVal());
			in_tuplet = true;
		}

		((NedChordOrRest *) lptr->data)->exportLilyFreeChordName(fp, last_line, midi_len);
		if (((NedChordOrRest *) lptr->data)->hasLastTupletFlag()) {
			fprintf(fp, " } ");
			in_tuplet = false;
		}
	}
	if (with_break) {
		fprintf(fp, "  \\break  ");
	}
	measure = getSystem()->getLastMeasure();
	spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
	if (spec_measure != NULL) {
		special = spec_measure->type;
		exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
	}
}

void NedVoice::exportLilyPond(FILE *fp, int *last_line, unsigned int *midi_len, bool last_system,
		unsigned long long end_time, bool *in_alternative, NedSlur **lily_slur, unsigned int *lyrics_map,
			bool with_break, bool *guitar_chordnames, bool *chordname, int *breath_script, bool keep_beams) {
	GList *lptr, *lptr2, *staff_elem_list;
	bool dummy4;
	NedMeasure *old_measure = NULL, *measure;
	SpecialMeasure *spec_measure;
	unsigned int special;
	bool in_tuplet = false;
	unsigned int dummy3;
	NedSlur *dummy2;
	int dummy5;
	bool first = true;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr), first = false) {
		//measure = getSystem()->getMeasure(((NedChordOrRest *) lptr->data)->getMidiTime());
		measure = ((NedChordOrRest *) lptr->data)->m_measure;
		if (measure != old_measure && measure->getMeasureNumber() > 1) {
			spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
			if (spec_measure != NULL && !first ) {
				special = spec_measure->type;
				exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
			}
			else {
				fprintf(fp, "| %% %d\n\t", measure->getMeasureNumber());
			}
		}
		if (last_system && ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) != 0) && ((NedChordOrRest *) lptr->data)->getMidiTime() >= end_time) {
			break;
		}
		if (m_voice_nr == 0) {
			staff_elem_list = m_staff->getStaffElements(((NedChordOrRest *) lptr->data)->getMidiTime());
			if (staff_elem_list != NULL) {
				for (lptr2 = g_list_first(staff_elem_list); lptr2; lptr2 = g_list_next(lptr2)) {
					 ((NedChordOrRest *) lptr2->data)->exportLilyPond(fp, last_line, midi_len, &dummy2, &dummy3, &dummy4, &dummy4, breath_script, keep_beams);
				}
				g_list_free(staff_elem_list);
			}
		}
		old_measure = measure;
		if (((NedChordOrRest *) lptr->data)->getTupletVal() && !in_tuplet) {
			fprintf(fp, " \\times %d/%d { ",  NedResource::m_tuplet_tab[((NedChordOrRest *) lptr->data)->getTupletVal()],
					((NedChordOrRest *) lptr->data)->getTupletVal());
			in_tuplet = true;
		}

		((NedChordOrRest *) lptr->data)->exportLilyPond(fp, last_line, midi_len, lily_slur, lyrics_map, guitar_chordnames, chordname, &dummy5, keep_beams);
		if (((NedChordOrRest *) lptr->data)->hasLastTupletFlag()) {
			fprintf(fp, " } ");
			in_tuplet = false;
		}
	}
	if (m_voice_nr == 0) {
		staff_elem_list = m_staff->getStaffElements(getSystem()->getSystemEndTime());
		if (staff_elem_list != NULL) {
			for (lptr2 = g_list_first(staff_elem_list); lptr2; lptr2 = g_list_next(lptr2)) {
				 ((NedChordOrRest *) lptr2->data)->exportLilyPond(fp, last_line, midi_len, &dummy2, &dummy3, &dummy4, &dummy4, breath_script, keep_beams);
			}
			g_list_free(staff_elem_list);
		}
	}
	if (with_break) {
		fprintf(fp, "  \\break  ");
	}
	measure = getSystem()->getLastMeasure();
	spec_measure = getMainWindow()->getSpecialMeasure(measure->getMeasureNumber());
	if (spec_measure != NULL) {
		special = spec_measure->type;
		exportLilypondSpecMeasure(fp, special, spec_measure, in_alternative, measure->getMeasureNumber());
	}
}
		
void NedVoice::exportLilyLyrics(FILE *fp, bool last_system, int line_nr, unsigned long long end_time, int *sil_count) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->exportLilyLyrics(fp, line_nr, sil_count);
		if (last_system && ((((NedChordOrRest *) lptr->data)->getType() & (TYPE_REST | TYPE_CHORD)) != 0) && ((NedChordOrRest *) lptr->data)->getMidiTime() >= end_time) {
			break;
		}
	}
}


void NedVoice::collectLyrics(NedLyricsEditor *leditor) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->collectLyrics(leditor);
	}
}

void NedVoice::setLyrics(NedCommandList *command_list, NedLyricsEditor *leditor) {
	GList *lptr;

	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->setLyrics(command_list, leditor);
	}
}

	
