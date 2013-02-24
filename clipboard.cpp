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

#include "clipboard.h"
#include "system.h"
#include "voice.h"
#include "system.h"
#include "staff.h"
#include "chordorrest.h"
#include "mainwindow.h"

int NedClipBoard::count;

NedClipBoard::NedClipBoard() : m_elements(NULL), element_counter(NULL), 
m_delete_only(FALSE), m_insert_only(FALSE), m_reftime(0), m_duration(0) {}


NedClipBoardElement::NedClipBoardElement() :
chords_or_rests_to_move(NULL), from_voice(NULL), to_voice(NULL), from_staff(NULL), to_staff(NULL) {}

void NedClipBoardElement::convertAllToClones() {
	GList  *lptr;
	GList *newlist = NULL;
	NedChordOrRest *chord_or_rest;

	for (lptr = chords_or_rests_to_move; lptr; lptr = g_list_next(lptr)) {
		chord_or_rest = (NedChordOrRest *) lptr->data;
		if (chord_or_rest->getType() != TYPE_REST || chord_or_rest->getLength() != WHOLE_NOTE) {
			NedResource::Abort("NedClipBoardElement::convertAllToClones");
		}
		newlist = g_list_append(newlist, chord_or_rest->cloneWithDifferentLength(WHOLE_NOTE, 0));
	}
	g_list_free(chords_or_rests_to_move);
	chords_or_rests_to_move = newlist;
}


void NedClipBoard::execute(bool adjust) {
	GList *lptr;
	NedClipBoardElement *element;
	NedVoice *voice_from = NULL, *voice_to = NULL;

	for (lptr = g_list_first(m_elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedClipBoardElement *) lptr->data;
		if (element->from_voice == NULL) {
			element->from_staff->deleteStaffElements(false, element->chords_or_rests_to_move, m_reftime, m_duration);
			if (!m_delete_only) {
				if (element->to_staff == NULL) {
					printf("element = 0x%x\n", element); fflush(stdout);
					NedResource::Abort("element->to_staff == NULL");
				}
				element->to_staff->insertIntoStaff(element->chords_or_rests_to_move, m_duration);
				if (g_list_length(element->chords_or_rests_to_move) > 0) {
					element->from_staff->getMainWindow()->setAndUpdateClefTypeAndKeySig();
				}
			}
		}
		else {
			voice_from = element->from_voice; 
			element->from_voice->deleteNoteGroup(element->chords_or_rests_to_move);
			if (!m_delete_only) {
				voice_to = element->to_voice;
				element->to_voice->insertIntoSystem(element->chords_or_rests_to_move);
			}
		}
	}
	if (adjust) {
		if (voice_from != NULL) {
			voice_from->getMainWindow()->reposit(NULL, voice_from->getPage(),
				voice_from->getSystem());
		}
		if (voice_to != NULL) {
			voice_to->getMainWindow()->reposit(NULL, voice_to->getPage(),
			voice_to->getSystem());
		}
	}
}

void NedClipBoard::unexecute(bool adjust) {
	GList *lptr;
	NedClipBoardElement *element;
	NedVoice *voice_from = NULL, *voice_to = NULL;

	for (lptr = g_list_first(m_elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedClipBoardElement *) lptr->data;
		if (element->from_voice == NULL) {
			if (!m_delete_only) {
				element->to_staff->deleteStaffElements(true, element->chords_or_rests_to_move, m_reftime, m_duration);
			}
			element->from_staff->appendAtStaff(element->chords_or_rests_to_move);
			if (g_list_length(element->chords_or_rests_to_move) > 0) {
				element->from_staff->getMainWindow()->setAndUpdateClefTypeAndKeySig();
			}
		}
		else {
			if (!m_delete_only) {
				voice_to = element->to_voice;
				element->to_voice->deleteNoteGroup(element->chords_or_rests_to_move);
			}
			voice_from = element->from_voice;
			element->from_voice->appendAtSystem(element->chords_or_rests_to_move);
		}
	}
	if (adjust) {
		if (voice_from != NULL) {
			voice_from->getMainWindow()->reposit(NULL, voice_from->getPage(),
			voice_from->getSystem());
		}
		if (voice_to != NULL) {
			voice_to->getMainWindow()->reposit(NULL, voice_to->getPage(),
			voice_to->getSystem());
		}
	}
}

void NedClipBoard::setInsertOnly() {
	GList *lptr;

	m_insert_only = true;
	for (lptr = g_list_first(m_elements); lptr; lptr = g_list_next(lptr)) {
		((NedClipBoardElement *) lptr->data)->convertAllToClones();
	}
}

void NedClipBoard::execute_reverse(bool adjust) {
	GList *lptr;
	NedClipBoardElement *element;
	NedVoice *voice_from = NULL, *voice_to = NULL;


	for (lptr = g_list_first(m_elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedClipBoardElement *) lptr->data;
		if (element->from_voice == NULL) {
			if (!m_insert_only) {
				element->from_staff->deleteStaffElements(true, element->chords_or_rests_to_move, m_reftime, m_duration);
			}
			element->to_staff->appendAtStaff(element->chords_or_rests_to_move);
			if (g_list_length(element->chords_or_rests_to_move) > 0) {
				element->to_staff->getMainWindow()->setAndUpdateClefTypeAndKeySig();
			}
		}
		else {
			if (!m_insert_only) {
				voice_from = element->from_voice;
				element->from_voice->deleteNoteGroup(element->chords_or_rests_to_move);
			}
			voice_to = element->to_voice;
			element->to_voice->appendAtSystem(element->chords_or_rests_to_move);
		}
	}
	if (adjust) {
		if (voice_from != NULL) {
			voice_from->getMainWindow()->reposit(NULL, voice_from->getPage(),
			voice_from->getSystem());
		}
		if (voice_to != NULL) {
			voice_to->getMainWindow()->reposit(NULL, voice_to->getPage(),
			voice_to->getSystem());
		}
	}
}

void NedClipBoard::unexecute_reverse(bool adjust) {
	GList *lptr;
	NedClipBoardElement *element;
	NedVoice *voice_from = NULL, *voice_to = NULL;

	for (lptr = g_list_first(m_elements); lptr; lptr = g_list_next(lptr)) {
		element = (NedClipBoardElement *) lptr->data;
		if (element->from_voice == NULL) {
			element->to_staff->deleteStaffElements(false, element->chords_or_rests_to_move, m_reftime, m_duration);
			if (!m_insert_only) {
				element->from_staff->insertIntoStaff(element->chords_or_rests_to_move, m_duration);
				if (g_list_length(element->chords_or_rests_to_move) > 0) {
					element->from_staff->getMainWindow()->setAndUpdateClefTypeAndKeySig();
				}
			}
		}
		else {
			voice_to = element->to_voice;
			element->to_voice->deleteNoteGroup(element->chords_or_rests_to_move);
			if (!m_insert_only) {
				voice_from = element->from_voice;
				element->from_voice->insertIntoSystem(element->chords_or_rests_to_move);
			}
		}
	}
	if (adjust) {
		if (voice_from != NULL) {
			voice_from->getMainWindow()->reposit(NULL, voice_from->getPage(),
			voice_from->getSystem());
		}
		if (voice_to) {
			voice_to->getMainWindow()->reposit(NULL, voice_to->getPage(),
			voice_to->getSystem());
		}
	}
}

bool NedClipBoard::onlyWholeRests() {
	GList *lptr1, *lptr2;
	NedClipBoardElement *element;
	NedChordOrRest *chord_or_rest;


	for (lptr1 = g_list_first(m_elements); lptr1; lptr1 = g_list_next(lptr1)) {
		element = (NedClipBoardElement *) lptr1->data;
		for (lptr2 = g_list_first(element->chords_or_rests_to_move); lptr2; lptr2 = g_list_next(lptr2)) {
			chord_or_rest = (NedChordOrRest *) lptr2->data;
			if (chord_or_rest->getType() != TYPE_REST || chord_or_rest->getLength() != WHOLE_NOTE) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

bool NedClipBoard::onlyStaffElems() {
	GList *lptr1, *lptr2;
	NedClipBoardElement *element;
	NedChordOrRest *chord_or_rest;


	for (lptr1 = g_list_first(m_elements); lptr1; lptr1 = g_list_next(lptr1)) {
		element = (NedClipBoardElement *) lptr1->data;
		for (lptr2 = g_list_first(element->chords_or_rests_to_move); lptr2; lptr2 = g_list_next(lptr2)) {
			chord_or_rest = (NedChordOrRest *) lptr2->data;
			if (chord_or_rest->getType() & (TYPE_CLEF  | TYPE_KEYSIG | TYPE_STAFF_SIGN)) {
				return false;
			}
		}
	}
	return true;
}
	
