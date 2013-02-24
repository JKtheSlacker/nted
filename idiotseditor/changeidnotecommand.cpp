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

#include "changeidnotecommand.h"
#include "idiotseditor.h"

NedChangeIdNoteCommand::NedChangeIdNoteCommand(NedIdNote *id_note, unsigned long long new_start, unsigned long long new_stop, int new_pitch) :
m_id_note(id_note), m_new_midi_start(new_start), m_new_midi_stop(new_stop), m_new_pitch(new_pitch) {
	m_old_valid = m_new_valid = id_note->m_valid;
	m_old_midi_start = id_note->m_midi_start;
	m_old_midi_stop = id_note->m_midi_stop;
	m_old_pitch = id_note->m_pitch;
}

void NedChangeIdNoteCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeIdNoteCommand::execute\n");
#endif
	m_id_note->m_midi_start = m_new_midi_start;
	m_id_note->m_midi_stop = m_new_midi_stop;
	m_id_note->m_pitch = m_new_pitch;
	m_id_note->m_valid = m_new_valid;
}


void NedChangeIdNoteCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeIdNoteCommand::unexecute\n");
#endif
	m_id_note->m_midi_start = m_old_midi_start;
	m_id_note->m_midi_stop = m_old_midi_stop;
	m_id_note->m_pitch = m_old_pitch;
	m_id_note->m_valid = m_old_valid;
}
