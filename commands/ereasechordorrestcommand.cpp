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

#include "ereasechordorrestcommand.h"
#include "voice.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedEreaseChordOrRestCommand::NedEreaseChordOrRestCommand(NedVoice *voice, int pos) :
m_voice(voice), m_pos(pos) {
	GList *lptr;
	lptr = g_list_nth(m_voice->m_chord_or_rests, m_pos); // friend !!
	if (lptr == NULL) {
		NedResource::Abort("NedEreaseChordOrRestCommand(1)");
	}
	m_chord_or_rest = ((NedChordOrRest *) lptr->data);
}

NedEreaseChordOrRestCommand::NedEreaseChordOrRestCommand(NedChordOrRest *chord) : //grace note
m_chord_or_rest(chord) {
	m_voice = chord->getVoice();
	if ((m_pos = g_list_index(m_voice->m_chord_or_rests, chord)) < 0) { // friend !!
		NedResource::Abort("NedEreaseChordOrRestCommand(2)");
	}
}

void NedEreaseChordOrRestCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedEreaseChordOrRestCommand::execute\n");
#endif
	GList *lptr = g_list_nth(m_voice->m_chord_or_rests, m_pos); // friend !!
	if (lptr == NULL) {
		NedResource::Abort("NedEreaseChordOrRestCommand::execute");
	}
	m_voice->m_chord_or_rests = g_list_delete_link(m_voice->m_chord_or_rests, lptr); // friend !!
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}

void NedEreaseChordOrRestCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedEreaseChordOrRestCommand::unexecute\n");
#endif
	if (m_pos < 0) {
		m_voice->m_chord_or_rests = g_list_append(m_voice->m_chord_or_rests, m_chord_or_rest); // friend !!
	}
	else {
		m_voice->m_chord_or_rests = g_list_insert(m_voice->m_chord_or_rests, m_chord_or_rest, m_pos); // friend !!
	}
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}

