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

#include "importer.h"

part::part() {
	init();
}
void part::init() {
	int i;
	id[0] = '\0';
	name[0] = '\0';
	divisions = 1;
	current_clef = clef = TREBLE_CLEF;
	clef_octave = current_clef_octave = 0;
	first_clef = true;
	volume = 64;
	current_key_signature = key_signature = 0;
	first_key_signature = true;
	numerator = current_numerator = denominator = current_denominator = 4;
	first_time_signature = true;
	midi_pgm = 0;
	midi_channel = 0;
	for (i = 0; i < VOICE_COUNT; i++) {
 		voices[i] = NULL;
		start_of_last_taken[i] = NULL;
		end_of_last_taken[i] = NULL;
		last_chord[i] = NULL;
		eof[i] = false;
		last_keysig[i] = 0;
	}
}

bool part::is_eof() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (!eof[i]) return false;
	}
	return true;
}

bool part::isEmpty() {
	int i;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (g_list_length(voices[i]) > 0 ) return false;
	}
	return true;
}

void part::copy_from(part *other_part) {
	int i;
	*this = *other_part;
	strcpy(id, other_part->id);
	strcpy(name, other_part->name);
	for (i = 0; i < VOICE_COUNT; i++) {
		voices[i] = other_part->voices[i];
		start_of_last_taken[i] = other_part->start_of_last_taken[i];
		end_of_last_taken[i] = other_part->end_of_last_taken[i];
		eof[i] = other_part->eof[i];
	}
}

void part::resetTaken() {
	int i;

	for (i = 0; i < VOICE_COUNT; i++) {
		end_of_last_taken[i] = g_list_previous(start_of_last_taken[i]);
	}
}

NedImporter::NedImporter() :  m_partcount(0), m_upbeat_inverse(0), m_tempo_inverse(START_TEMPO_INVERSE), m_main_window(NULL) {}

void NedImporter::resetTaken() {
	int i;

	for (i = 0; i < m_partcount; i++) {
		m_parts[i].resetTaken();
	}
}

bool NedImporter::is_eof() {
	int i;

	for (i = 0; i < m_partcount; i++) {
		if (!m_parts[i].is_eof()) return false;
	}
	return true;
}
