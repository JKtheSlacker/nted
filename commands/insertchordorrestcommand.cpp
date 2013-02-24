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

#include "insertchordorrestcommand.h"
#include "voice.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"
#include "system.h"


NedInsertChordOrRestCommand::NedInsertChordOrRestCommand(NedVoice *voice, int pos, NedChordOrRest *chord_or_rest) :
m_voice(voice), m_pos(pos), m_adjustable(true), m_chord_or_rest(chord_or_rest) {}


void NedInsertChordOrRestCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertChordOrRestCommand::execute\n");
#endif
	if (m_pos < 0) {
		m_voice->m_chord_or_rests = g_list_append(m_voice->m_chord_or_rests, m_chord_or_rest); // friend !!
	}
	else {
		m_voice->m_chord_or_rests = g_list_insert(m_voice->m_chord_or_rests, m_chord_or_rest, m_pos); // friend !!
	}
	m_chord_or_rest->computeBbox();
	if (adjust && m_adjustable) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}


void NedInsertChordOrRestCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertChordOrRestCommand::unexecute\n");
#endif
	GList *inserted = g_list_find(m_voice->m_chord_or_rests, m_chord_or_rest);
	//GList *inserted = g_list_find(m_chord_or_rest->getVoice()->m_chord_or_rests, m_chord_or_rest);
	if (inserted == NULL) {
	  printf("voice %d (0x%lx) gesucht: 0x%lx\n", m_voice->getVoiceNumber(), (unsigned long)m_voice, (unsigned long)m_chord_or_rest);
		NedResource::Abort("NedInsertChordOrRestCommand::unexecute");
	}
	m_voice->m_chord_or_rests = g_list_delete_link(m_voice->m_chord_or_rests, inserted); // friend !!
	if (adjust && m_adjustable) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}
