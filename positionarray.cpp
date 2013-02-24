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

#include "positionarray.h"
#include "chordorrest.h"
#include "resource.h"
#include "voice.h"
#include "note.h"

#define DIVISION NOTE_8


NedPosition::NedPosition(NedChordOrRest *element, int offs, unsigned int staff_count) : m_valid(true),
	m_has_notes(false), m_has_unhidden_elements(false), m_has_only_staff_members(true), m_has_only_graces(true), m_max_microshift(0.0), m_step_width(0.0), m_min_duration(8* WHOLE_NOTE), m_midi_time(element->getMidiTime()),
	m_min_width_after(0.0), m_min_width_before(0.0), m_acc_dist(0.0), m_needed_width(0.0), m_min_needed_width(0.0), m_staff_count(staff_count) {
	m_voice_members.setLen(VOICE_COUNT * staff_count, true);
	m_staff_members.setLen(staff_count, true);
	m_count_of_elements_per_staff.setLen(staff_count, true);
	if (element->getType() == TYPE_KEYSIG) {
		return; // TODO: this shouldn't happen (???)
		NedResource::Abort("TYPE_KEYSIG(1)");
	}
	if ((element->getType() & (TYPE_CHORD | TYPE_REST)) != 0) {
		m_voice_members.insert(offs, element);
		m_has_only_staff_members = false;
		m_has_only_graces = false;
		m_element_has_arpeggio = ((element->getStatus() & STAT_ARPEGGIO) != 0);
	}
	else if ((element->getType() & TYPE_GRACE) == 0) {
		m_has_only_graces = false;
		m_staff_members.insert(offs, element);
	}
}


void NedPosition::setNewStaffCount(unsigned int newstaffcount) {
	if (newstaffcount == m_staff_count) return;
	//if (newstaffcount > m_staff_count) {
		m_voice_members.setLen(VOICE_COUNT * newstaffcount, true);
		m_staff_members.setLen(newstaffcount, true);
		m_count_of_elements_per_staff.setLen(newstaffcount, true);
	//}
	m_staff_count = newstaffcount;
}

bool NedPosition::hasOtherStaffElements(int type) {
	NedChordOrRest **ptr;

	for (ptr = m_staff_members.getFirst(); ptr; ptr = m_staff_members.getNext()) {
		if ((*ptr) == NULL) continue;
		if ((*ptr)->getType() != type) return true;
	}

	return false;
}

void NedPosition::insertElement(NedChordOrRest *element, int offs) {
	unsigned char c;
	m_midi_time = element->getMidiTime();
	m_valid = true;
	m_has_only_staff_members = false;
	if (element->getType() == TYPE_KEYSIG) {
		NedResource::Abort("TYPE_KEYSIG(2)");
	}

	if (element->getType() == TYPE_CHORD) {
		m_has_unhidden_elements = m_has_notes = true;
		m_has_only_graces = false;
		m_element_has_arpeggio = m_element_has_arpeggio || ((element->getStatus() & STAT_ARPEGGIO) != 0);
		if (element->getDuration() < m_min_duration) {
			m_min_duration = element->getDuration();
		}
	}
	else if (element->getType() == TYPE_REST) {
		if (!element->isHidden() || (element->getStatus() & STAT_USER_HIDDEN_REST) == 0) {
			m_has_unhidden_elements = true;
		}
		m_has_only_graces = false;
	}
	if (element->getType() != TYPE_REST || (!element->isHidden() && (element->getStatus() & STAT_USER_HIDDEN_REST) == 0)) {
		if (element->getBBox()->width > m_min_needed_width) {
			m_min_needed_width = element->getBBox()->width;
		}
		if (element->getBBox()->width + element->getBBox()->x > m_min_width_after) {
			m_min_width_after = element->getBBox()->width + element->getBBox()->x;
		}
		if (-element->getBBox()->x > m_min_width_before) {
			m_min_width_before = -element->getBBox()->x;
		}
	}
	m_voice_members.insert(offs, element);
	if (element->isRest() && (element->isHidden() || (element->getStatus() & STAT_USER_HIDDEN_REST))) return;
	c = m_count_of_elements_per_staff.getAt(offs / VOICE_COUNT);
	m_count_of_elements_per_staff.insert(offs / VOICE_COUNT, c + 1);
}

bool NedPosition::insertStaffElement(NedChordOrRest *element, int staff_offs) {
	unsigned int c;
	/*
	if (m_valid) {
		if (m_staff_members.getAt(staff_offs) != NULL) return false;
	}
	*/
	m_has_only_graces = false;
	m_midi_time = element->getMidiTime();
	m_has_unhidden_elements =
	m_valid = true;

	if (element->getBBox()->width > m_min_needed_width) {
		m_min_needed_width = element->getBBox()->width;
	}
	if (element->getBBox()->width + element->getBBox()->x > m_min_width_after) {
		m_min_width_after = element->getBBox()->width + element->getBBox()->x;

	}

	if (-element->getBBox()->x > m_min_width_before) {
		m_min_width_before = -element->getBBox()->x;
	}
	m_staff_members.insert(staff_offs, element);
	c = m_count_of_elements_per_staff.getAt(staff_offs);
	m_count_of_elements_per_staff.insert(staff_offs, c + 1);
	return true;
}


bool NedPosition::isUsed(int offs) {
	bool res = m_voice_members.getAt(offs) != NULL;
	return res;
}


void NedPosition::empty() {
	m_has_unhidden_elements = m_has_notes = m_valid = false;
	m_has_only_graces = m_has_only_staff_members = true;
	m_max_microshift = m_min_needed_width = m_needed_width = m_min_width_after = m_min_width_before = 0.0;
	m_min_duration = 8 * WHOLE_NOTE;

	m_count_of_elements_per_staff.zeroArray();

	if (m_voice_members.getLen() > 0) {
		m_voice_members.zeroArray();
	}
	if (m_staff_members.getLen() > 0) {
		m_staff_members.zeroArray();
	}
}


void NedPosition::placeAt(double xpos, int *v_array, struct voice_properties *v_props, int mnum, bool out) {
	NedChordOrRest **ptr;
	int i;

	if (m_has_only_staff_members) {
		for (ptr = m_staff_members.getFirst(); ptr; ptr = m_staff_members.getNext()) {
			if (*ptr == NULL) continue;
			(*ptr)->setXPos2(xpos);
		}
		return;
	}

	if (m_has_only_graces) {
		for (ptr = m_voice_members.getFirst(); ptr; ptr = m_voice_members.getNext()) {
			if (*ptr == NULL) continue;
			(*ptr)->setXPos2(xpos);
		}
		return;
	}

	for (i = 0, ptr = m_voice_members.getFirst(); ptr; ptr = m_voice_members.getNext(), i++) {
		if (*ptr == NULL) continue;
		(*ptr)->setXPos2(xpos);
		if (v_array[i / VOICE_COUNT] > 1) {
			if (!(*ptr)->isRest()) {
				(*ptr)->changeStemDir(v_props[i].m_stem_decision);
			}
		}
	}
}

void NedPosition::computeAccidentalPositions(int mnum, bool out) {
	NedChordOrRest **ptr;
	int acc_places[4];
	unsigned int i, j, offs = 0;
	NedNote **notes_with_accidentals = (NedNote **) alloca(MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF * m_staff_count * sizeof(NedNote *));
	NedNote **nptr;
	unsigned int *num_notes_with_accidentals_per_staff = (unsigned int *) alloca(m_staff_count * sizeof(unsigned int));
	unsigned int accidental_columns;
	double acc_d;

	memset(num_notes_with_accidentals_per_staff, 0, m_staff_count * sizeof(unsigned int));

	m_acc_dist = 0.0;
	for (i = 0, ptr = m_voice_members.getFirst(); /* offs < m_staff_count && */ ptr; ptr = m_voice_members.getNext(), i++) {
		if (*ptr != NULL) {
			(*ptr)->collectNotesWithAccidentals(num_notes_with_accidentals_per_staff + offs, notes_with_accidentals + MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF * offs);
		}
		if (((i + 1) % VOICE_COUNT) == 0) {
			if (num_notes_with_accidentals_per_staff[offs] > 0) {
				qsort(notes_with_accidentals + offs * MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF, num_notes_with_accidentals_per_staff[offs], sizeof(NedNote *),
					(int (*) (const void *, const void *)) NedNote::compare_note_lines);
			}
			offs++;
		}
	}

	accidental_columns = 0;
	for (i = 0; i < m_staff_count; i++) {
		nptr = notes_with_accidentals + MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF * i;
		acc_places[0] = acc_places[1] = acc_places[2] = acc_places[3] = -10000;
		for (j = 0; j < num_notes_with_accidentals_per_staff[i]; j++, nptr++) {
			acc_d = (*nptr)->placeAccidental(FALSE /*all_shifted*/, acc_places, m_element_has_arpeggio, out &&  mnum == 1);
			if (acc_d > m_acc_dist) m_acc_dist = acc_d;
		}
	}
}

void NedPosition::computeMicroShifts(int mnum, int pos, bool out) {
	NedChordOrRest **p_staff, **p_voice1, **p_voice2;
	double microshift;



	for (p_staff = m_voice_members.getFirst(); p_staff; p_staff = m_voice_members.skip(VOICE_COUNT)) {
		for (p_voice1 = m_voice_members.getFirst2(VOICE_COUNT); p_voice1; p_voice1 = m_voice_members.getNext2()) {
			if (*p_voice1 == NULL) continue;
			if ((*p_voice1)->getType() != TYPE_CHORD) continue;
			(*p_voice1)->resetMicroshift();
		}
		for (p_voice1 = m_voice_members.getFirst2(VOICE_COUNT); p_voice1; p_voice1 = m_voice_members.getNext2()) {
			if (*p_voice1 == NULL) continue;
			if ((*p_voice1)->getType() != TYPE_CHORD) continue;
			for (p_voice2 = m_voice_members.getFirst3(); p_voice2; p_voice2 = m_voice_members.getNext3()) {
				if (*p_voice2 == NULL) continue;
				if ((*p_voice2)->getType() != TYPE_CHORD) continue;
				/*
				if (out) {
					printf("mnum = %d, voi1 = %d, voi2 = %d\n", mnum, (*p_voice1)->getVoice()->getVoiceNumber(), (*p_voice2)->getVoice()->getVoiceNumber()); fflush(stdout);
				}
				*/
				microshift = (*p_voice1)->compute_microshift(*p_voice2, mnum, pos);
				if (microshift > m_max_microshift) m_max_microshift = microshift;
			}
		}
	}
}


void NedPosition::shiftXpos(double xshift, int mnum, int pos) {
	NedChordOrRest **ptr;

	if (m_has_only_staff_members) {
		for (ptr = m_staff_members.getFirst(); ptr;  ptr = m_staff_members.getNext()) {
			if (*ptr == NULL) continue;
			(*ptr)->shiftX(xshift);
		}
		return;
	}

	for (ptr = m_voice_members.getFirst(); ptr; ptr = m_voice_members.getNext()) {
		if (*ptr == NULL) continue;
		(*ptr)->shiftX(xshift);
	}

}


bool NedPosition::hasNote() {
	NedChordOrRest **ptr;

	for (ptr = m_voice_members.getFirst(); ptr;  ptr = m_voice_members.getNext()) {
		if (*ptr == NULL) continue;
		if (!(*ptr)->isRest()) return true;
	}
	return false;
}


void NedPosition::placeRests(struct voice_properties *vprops) {
	int i, j;
	NedChordOrRest **st_ptr, **ptr1, **ptr2, **pterm1, **pterm2;
	unsigned int st, ii;
	double shift;
	int *notes_available = (int *) alloca(m_staff_count * sizeof(int));
	memset(notes_available, 0, m_staff_count * sizeof(int));

	for (ii = 0, ptr1 = m_voice_members.getFirst(); ptr1;  ptr1 = m_voice_members.getNext(), ii++) {
		if (*ptr1 == NULL) continue;
		if ((*ptr1)->isRest()) continue;
		notes_available[ii / VOICE_COUNT] = 1;
	}

	for (st = 0, st_ptr = m_voice_members.getFirst();  st_ptr; st_ptr = m_voice_members.skip(VOICE_COUNT), st++) {
		if (m_count_of_elements_per_staff.getAt(st) < 2) continue;
		pterm1 = st_ptr + VOICE_COUNT - 1;
		pterm2 = st_ptr + VOICE_COUNT;
		for (i = 0, ptr1 = st_ptr; ptr1 < pterm1; ptr1++, i++) {
			if (*ptr1 == NULL) continue;
			if ((*ptr1)->isRest() && ((*ptr1)->isHidden() || ((*ptr1)->getStatus() & (STAT_USER_PLACED_REST | STAT_USER_HIDDEN_REST)))) continue;
			for (j = i+1, ptr2 = ptr1 + 1; ptr2 < pterm2; ptr2++, j++) {
				if (*ptr2 == NULL) continue;
				if (!(*ptr2)->isRest()) continue;
				if ((*ptr2)->isHidden()) continue;
				if ((*ptr2)->getStatus() & (STAT_USER_PLACED_REST | STAT_USER_HIDDEN_REST)) continue;
				shift = (*ptr1)->compute_y_shift(*ptr2, vprops[st * VOICE_COUNT + j].m_stem_decision);
				if (shift > 0.0) {
					if (vprops[st * VOICE_COUNT + j].m_stem_decision == STEM_DIR_DOWN) {
						(*ptr2)->shiftY(shift);
					}
					else {
						(*ptr2)->shiftY(-shift);
					}
				}
			}
		}
		for (i = VOICE_COUNT - 1, ptr1 = st_ptr + VOICE_COUNT - 1; ptr1 > st_ptr; ptr1--, i--) {
			if (*ptr1 == NULL) continue;
			if ((*ptr1)->isRest() && ((*ptr1)->isHidden() || ((*ptr1)->getStatus() & (STAT_USER_PLACED_REST | STAT_USER_HIDDEN_REST)))) continue;
			pterm2 = notes_available[st] ? st_ptr : st_ptr + 1;
			for (j = i - 1, ptr2 = ptr1 - 1; ptr2 >= st_ptr; ptr2--, j--) {
				if (*ptr2 == NULL) continue;
				if (!(*ptr2)->isRest()) continue;
				if ((*ptr2)->isHidden()) continue;
				if ((*ptr2)->getStatus() & (STAT_USER_PLACED_REST | STAT_USER_HIDDEN_REST)) continue;
				shift = (*ptr1)->compute_y_shift(*ptr2, vprops[st * VOICE_COUNT + j].m_stem_decision);
				if (shift > 0.0) {
					if (vprops[st * VOICE_COUNT + j].m_stem_decision == STEM_DIR_DOWN) {
						(*ptr2)->shiftY(shift);
					}
					else {
						(*ptr2)->shiftY(-shift);
					}
				}
			}
		}
	}
}

#ifdef UUU

bool NedPosition::print() {
	unsigned int i;
	bool b = false;

	printf("-----------------------\n");
	for (i = 0; i < m_voice_members->len; i++) {
		//if (((NedChordOrRest *) m_members->pdata[i])->getType() != TYPE_CHORD) continue;
		if (((NedChordOrRest *) m_voice_members->pdata[i])->getVoice()->getVoiceNumber() != 0) continue;
		b = true;
		printf("\t%s: TIME: %llu dur: %u pos = %f\n", ((NedChordOrRest *) m_voice_members->pdata[i])->isRest() ? "Rest" : "Note", 
				((NedChordOrRest *) m_voice_members->pdata[i])->getMidiTime() / DIVISION,
				((NedChordOrRest *) m_voice_members->pdata[i])->getDuration() / DIVISION,
				((NedChordOrRest *) m_voice_members->pdata[i])->getXPos());
	}
	return b;
}

#endif

NedPositionArray::NedPositionArray() : m_staff_count(0), m_min_duration(2 * WHOLE_NOTE) {}

NedPositionArray::~NedPositionArray() {
	NedPosition **ptr;

	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		delete (*ptr);
	}
}

int NedPositionArray::compare_positions(NedPosition **pos1, NedPosition **pos2)  {
	if (!(*pos1)->m_valid && !(*pos2)->m_valid) return 0;
	if (!(*pos1)->m_valid) return 1;
	if (!(*pos2)->m_valid) return -1;
	if ((*pos1)->m_midi_time == (*pos2)->m_midi_time) {
		if ((*pos1)->m_has_only_staff_members && (*pos2)->m_has_only_staff_members) return 0;
		if ((*pos1)->m_has_only_staff_members) return -1;
		if ((*pos2)->m_has_only_staff_members) return 1;
		if ((*pos1)->m_has_only_graces && (*pos2)->m_has_only_graces) return -1;
		if ((*pos1)->m_has_only_graces) return -1;
		if ((*pos2)->m_has_only_graces) return 1;
	}
	if ((*pos1)->m_midi_time < (*pos2)->m_midi_time) return -1;
	return 1;
}


void NedPositionArray::insertElement(NedChordOrRest *element, int offs, int mnum) {
	NedPosition *pos_ptr;
	NedPosition **ptr;
	struct voice_properties *np_ptr;

	np_ptr = m_voice_properties.getPointerAt(offs);
	if (element->getType() & TYPE_CHORD) {
		np_ptr->m_note_count++;
		np_ptr->m_line_sum += element->getLineOfLowestNote();
	}
	if ((element->getType() & TYPE_REST) && !element->isHidden() && (element->getStatus() & STAT_USER_HIDDEN_REST) == 0) {
		if (np_ptr == NULL) {
			printf("(element->getType() = 0x%x\n", element->getType()); fflush(stdout);
		}
		np_ptr->m_rests_count++;
	}

	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) continue;
		if ((*ptr)->m_has_only_staff_members) continue;
		if ((*ptr)->m_has_only_graces) continue;
		if ((*ptr)->m_midi_time == element->getMidiTime()) {
			(*ptr)->insertElement(element, offs);
			if (element->getType() == TYPE_CHORD && (*ptr)->m_min_duration < m_min_duration) {
				m_min_duration = (*ptr)->m_min_duration;
			}
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}
	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) {
			(*ptr)->insertElement(element, offs);
			if (element->getType() == TYPE_CHORD && (*ptr)->m_min_duration < m_min_duration) {
				m_min_duration = (*ptr)->m_min_duration;
			}
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}

	pos_ptr = new NedPosition(element, offs, m_staff_count);
	m_positions.append(pos_ptr);
	if (element->getType() == TYPE_CHORD && element->getDuration() < m_min_duration) {
		m_min_duration = element->getDuration();
	}
	//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
}


void NedPositionArray::insertStaffElement(NedChordOrRest *element, int staff_offs, int mnum) {
	NedPosition *pos_ptr;
	NedPosition **ptr;
	int type = element->getType();


	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) continue;
		if ((*ptr)->m_has_only_graces) continue;
		if (!(*ptr)->m_has_only_staff_members) continue;
		if ((*ptr)->hasOtherStaffElements(type)) continue;
		if ((*ptr)->m_midi_time == element->getMidiTime()) {
			if (!(*ptr)->insertStaffElement(element, staff_offs)) break;
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}
	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) {
			(*ptr)->insertStaffElement(element, staff_offs);
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}

	pos_ptr = new NedPosition(element, staff_offs, m_staff_count);
	m_positions.append(pos_ptr);
	//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
}


void NedPositionArray::insertGraceElement(NedChordOrRest *element, int offs, int mnum) {
	NedPosition *pos_ptr;
	NedPosition **ptr;

	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) continue;
		if ((*ptr)->m_has_only_staff_members) continue;
		if (!(*ptr)->m_has_only_graces) continue;
		if ((*ptr)->m_midi_time == element->getMidiTime()) {
			if ((*ptr)->isUsed(offs)) break;
			(*ptr)->insertElement(element, offs);
			(*ptr)->m_has_unhidden_elements = true;
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}
	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		if (!(*ptr)->m_valid) {
			(*ptr)->m_has_unhidden_elements = true;
			(*ptr)->insertElement(element, offs);
			//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
			return;
		}
	}

	pos_ptr = new NedPosition(element, offs, m_staff_count);
	m_positions.append(pos_ptr);
	//m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
}


void NedPositionArray::empty() {
	NedPosition **ptr;
	m_min_duration = 8 * WHOLE_NOTE;

	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		(*ptr)->empty();
	}
	m_voice_properties.zeroArray();
}


void NedPositionArray::setNewStaffCount(unsigned int newstaffcount) {
	NedPosition **ptr;

	if (newstaffcount > m_staff_count) {
		m_voice_properties.setLen(VOICE_COUNT * newstaffcount, false);
	}
	m_staff_count = newstaffcount;

	for (ptr = m_positions.getFirst(); ptr; ptr = m_positions.getNext()) {
		(*ptr)->setNewStaffCount(newstaffcount);
	}
}

double NedPositionArray::shiftXpos(double extra_space_divided, double shift, int mnum, bool out) {
	NedPosition **ptr, **last = 0;
	int i;

	for (i = 0, ptr = m_positions.getFirst(); ptr; last = ptr, ptr = m_positions.getNext(), i++) {
		if (!(*ptr)->m_valid) continue;
		shift += extra_space_divided;
		if (last == NULL) {
			if ((*ptr)->m_acc_dist > (*ptr)->m_step_width) {
				shift += (*ptr)->m_acc_dist - ((*ptr)->m_step_width);
			}
		}
		else {
			if ((*ptr)->m_acc_dist > (*ptr)->m_step_width - (*last)->m_min_width_after) {
				shift += (*ptr)->m_acc_dist - ((*ptr)->m_step_width - (*last)->m_min_width_after);
			}
		}
		//shift += (*ptr)->m_acc_dist;
		(*ptr)->shiftXpos(shift, mnum, i);
	}
	return shift;
}

struct v_params_str{
	char voice_number;
	char valid;
	double av;
};

double NedPositionArray::computeAndSetPositions(double start_position, int *num_elements, double *extra_space_for_accidentals, bool *measure_has_only_staff_members /* means keysig */, double timesig_space, int mnum, double spread_fac, bool out) {
	NedPosition **p_first_valid = NULL, **p_last_valid, **p_next_valid;
	struct voice_properties *p_vp;
	struct v_params_str vparams[VOICE_COUNT];
	double highest;
	int h_idx = 0;
	bool taken;
	double last_width_after, last_micro_shifts = 0.0;
	unsigned int last_duration;
	unsigned int mid;
	unsigned int i, j, k;
	int num_distances = 0;
	unsigned long long dist, mindist = 2 * WHOLE_NOTE;
	double step_width, min_step_width;
	int *num_voices = (int *) alloca(sizeof(int) * m_staff_count);
	int *num_notes = (int *) alloca(sizeof(int) * m_staff_count);

	m_positions.sort((gint (*)(const void*, const void*)) compare_positions);
	memset(num_voices, 0, sizeof(int) * m_staff_count);
	memset(num_notes, 0, sizeof(int) * m_staff_count);

	for (i = 0; i < m_staff_count; i++) {
		for (j = 0; j < VOICE_COUNT; j++) {
			vparams[j].voice_number = j;
			if (m_voice_properties.getAt(VOICE_COUNT * i + j).m_note_count > 0) {
				num_voices[i]++;
				num_notes[i]++;
			}
			else if (m_voice_properties.getAt(VOICE_COUNT * i + j).m_rests_count > 0) {
				num_voices[i]++;
			}
		}
	}
	p_vp = m_voice_properties.getArray();
	for (i = 0; i < m_staff_count; i++, p_vp += VOICE_COUNT) {
		if (num_notes[i] > 1) {
			for (j = 0; j < VOICE_COUNT; j++) {
				vparams[j].voice_number = j;
				if (p_vp[j].m_note_count > 0) {
					vparams[j].av = (double) p_vp[j].m_line_sum / (double) p_vp[j].m_note_count;
					vparams[j].valid = true;
				}
				else {
					vparams[j].av = 100000.0;
					vparams[j].valid = false;
				}
			}
			mid = num_notes[i] / 2;
			k = 0;
			do {
				taken = false;
				highest = -20000.0;
				for (j = 0; j < VOICE_COUNT; j++) {
					if (!vparams[j].valid) continue;
					if (vparams[j].av > highest) {
						h_idx = j;
						highest = vparams[j].av;
						taken = true;
					}
				}
				if (taken) {
					if (h_idx < 0 || h_idx >= VOICE_COUNT) {
						NedResource::Abort("hidx");
					}
					vparams[h_idx].valid = false;
					p_vp[h_idx].m_stem_decision = k++ < mid ? STEM_DIR_UP : STEM_DIR_DOWN;
				}
			}
			while (taken);
		}
	}
	m_min_durations_dist = 0.0;
	last_width_after = 0.0;
	last_duration = 0;

	for (p_last_valid = m_positions.getFirst(); p_last_valid; p_last_valid = m_positions.getNext()) {
		if ((*p_last_valid)->m_valid) {
			(*p_last_valid)->computeAccidentalPositions(mnum, out);
			if ((*p_last_valid)->m_has_notes) {
				last_duration = (*p_last_valid)->m_min_duration;
				last_width_after = (*p_last_valid)->m_min_width_after;
			}
			break;
		}
	}
	p_first_valid = p_last_valid;
	p_next_valid = m_positions.getNext();
	while (p_next_valid) {
		if ((*p_next_valid)->m_valid) {
			(*p_next_valid)->computeAccidentalPositions(mnum, out);
			dist = (*p_next_valid)->m_midi_time - (*p_last_valid)->m_midi_time;
			if (dist < mindist) {
				mindist = dist;
			}
			if (last_duration == m_min_duration) {
				if (last_width_after + (*p_next_valid)->m_min_width_before > m_min_durations_dist)
					 m_min_durations_dist = last_width_after + (*p_next_valid)->m_min_width_before;
			}
			if ((*p_next_valid)->m_has_notes) {
				last_duration = (*p_next_valid)->m_min_duration;
				last_width_after = (*p_next_valid)->m_min_width_after;
			}
/*
			else {
				last_duration = 0;
			}
*/
			p_last_valid = p_next_valid;
			num_distances++;
		}
		p_next_valid = m_positions.getNext();
	}
	if (num_distances == 0 && p_first_valid != NULL && (*p_first_valid) != NULL && /* staff members i.e. keysigs have teir own values */ !(*p_first_valid)->m_has_only_staff_members ) {
		(*p_first_valid)->m_needed_width = 2 * WHOLE_EXTRA_SPACE;
		(*p_first_valid)->m_min_needed_width = 2 * WHOLE_EXTRA_SPACE;
		(*p_first_valid)->m_min_width_after = WHOLE_EXTRA_SPACE;
		(*p_first_valid)->m_min_width_before = WHOLE_EXTRA_SPACE;
		m_min_durations_dist = 2 * WHOLE_EXTRA_SPACE;
	}
	else {
#define MAX_MINDURATIONS_DIST 1.0
		if (m_min_durations_dist > MAX_MINDURATIONS_DIST) m_min_durations_dist = MAX_MINDURATIONS_DIST;
	}
	last_width_after = last_micro_shifts = 0.0;
	bool last_round_had_a_grace = false;
	int pos = 0;
	for (i = 0, p_next_valid = m_positions.getFirst(); p_next_valid; p_next_valid = m_positions.getNext()) {
		if (!(*p_next_valid)->m_valid) continue;
		(*num_elements)++;
		(*p_next_valid)->computeMicroShifts(mnum, pos++, out && mnum == 1);
		min_step_width = last_width_after + last_micro_shifts + (*p_next_valid)->m_min_width_before;
		if (p_next_valid != NULL) {
			if ((*p_next_valid)->m_has_notes) {
				*measure_has_only_staff_members = false;
				if (i == 0) {
					step_width = 0.0; // avoid additional gap behind bar line
				}
				timesig_space = 0.0; // add only if staff element is at first place
				step_width = 1.4 * HEAD_THICK + (double) ((*p_next_valid)->m_min_duration) / (double) (m_min_duration) * 0.2 * HEAD_THICK;
				if (step_width < min_step_width) step_width = min_step_width;
				if (last_round_had_a_grace) {
					step_width = 1.5 * HEAD_THICK;
					last_round_had_a_grace = false;
				}
			}
			else if ((*p_next_valid)->m_has_only_staff_members) {
				if (i == 0) {
					step_width = 0.0; // avoid additional gap behind bar line
				}
				else  {
					step_width = min_step_width;
					timesig_space = 0.0; // add only if staff element is at first place
				}
				last_round_had_a_grace = false;

			}
			else if ((*p_next_valid)->m_has_only_graces) {
				step_width = 0.9 * HEAD_THICK; // rest
				timesig_space = 0.0; // add only if staff element is at first place
				*measure_has_only_staff_members = false;
				last_round_had_a_grace = true;
			}
			else {
				step_width = 1.4 * HEAD_THICK; // rest
				timesig_space = 0.0; // add only if staff element is at first place
				*measure_has_only_staff_members = false;
				if (step_width < min_step_width) step_width = min_step_width;
				last_round_had_a_grace = false;
			}
		}
		else {
			step_width = min_step_width;
			*measure_has_only_staff_members = false;
			timesig_space = 0.0; // add only if staff element is at first place
			last_round_had_a_grace = false;
		}
		step_width *= spread_fac;
		if ((*p_next_valid)->m_has_unhidden_elements) {
			start_position += step_width;
		}
		(*p_next_valid)->placeAt(start_position, num_voices, m_voice_properties.getArray(), mnum, out);
		start_position += timesig_space; // If bar has a timesig then additional space is needed after an eventuall staff element (keysig)

		/*
#define EXTRA_DIST 0.01
		step_width += EXTRA_DIST;
		if (out) {
			printf("mnum = %d, pos = %d, need = %f, step = %f\n",
				mnum, i, (*p_last_valid)->m_min_needed_width, step_width);
			fflush(stderr);
		}
		*/
		(*p_next_valid)->m_step_width = step_width - last_micro_shifts;
		if ((*p_next_valid)->m_acc_dist > step_width - last_micro_shifts -  last_width_after) {
			(*extra_space_for_accidentals) += (*p_next_valid)->m_acc_dist - (step_width - last_micro_shifts -last_width_after);
		}
		/*
		if (out) {
			printf("mnum = %d, pos = %d, m_acc_dist = %f\n",
				mnum, i, (*p_last_valid)->m_acc_dist); fflush(stderr);
		}
		*/
		//(*extra_space_for_accidentals) += (*p_last_valid)->m_acc_dist;
		if ((*p_next_valid)->m_has_only_graces) {
			last_round_had_a_grace = true;
		}
		last_width_after = (*p_next_valid)->m_min_width_after;
		last_micro_shifts = (*p_next_valid)->m_max_microshift;
		i++;
	}
	start_position += last_width_after + last_micro_shifts;


	for (p_last_valid = m_positions.getFirst(); p_last_valid; p_last_valid = m_positions.getNext()) {
		if (!(*p_last_valid)->m_valid) continue;
		(*p_last_valid)->placeRests(m_voice_properties.getArray());
	}
	return start_position;
}

int NedPositionArray::getStemDirectionOfTheMeasure(int m_staff_voice_offs) {
	if (m_voice_properties.getLen() < 1) return STEM_DIR_NONE;
	if (m_voice_properties.getLen() <= m_staff_voice_offs / sizeof(struct voice_properties)) return STEM_DIR_NONE;
	return m_voice_properties.getAt(m_staff_voice_offs).m_stem_decision;
}

#ifdef UUU
void NedPositionArray::print() {
	unsigned int i;
	bool b = false;

	for (i = 0; i < m_positions->len; i++) {
		if (((NedPosition *) m_positions->pdata[i])->m_valid) {
			if (((NedPosition *) m_positions->pdata[i])->print()) {
				b = true;
			}
		}
		if (b) {
			putchar('\n');
		}
	}
	fflush(stdout);
}

#endif
