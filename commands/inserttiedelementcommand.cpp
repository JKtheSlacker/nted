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

#include "inserttiedelementcommand.h"
#include "voice.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedInsertTiedElementCommand::NedInsertTiedElementCommand(NedVoice *voice, NedChordOrRest *chord_or_rest, unsigned int len, int dotcount) :
m_chord_or_rest(chord_or_rest), m_voice(voice) {
	m_new_chord_or_rest = chord_or_rest->cloneWithDifferentLength(len, dotcount);

}

void NedInsertTiedElementCommand::execute(bool adjust /* = false */) {
	int pos;
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertTiedElementCommand::execute\n");
#endif
	if ((pos = g_list_index(m_voice->m_chord_or_rests, m_chord_or_rest)) < 0) { // friend !!
		NedResource::Abort("NedInsertTiedElementCommand::execute");
	}
	m_voice->m_chord_or_rests = g_list_insert(m_voice->m_chord_or_rests, m_new_chord_or_rest, pos + 1); // friend !!
	m_chord_or_rest->tieCompleteTo(m_new_chord_or_rest, true);
	
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}


void NedInsertTiedElementCommand::unexecute(bool adjust /* = false */) {
	GList *lptr;
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertTiedElementCommand::unexecute\n");
#endif

	m_new_chord_or_rest->removeAllBackwardTies();
	if ((lptr = g_list_find(m_voice->m_chord_or_rests, m_new_chord_or_rest)) == NULL) { // friend !!
		NedResource::Abort("NedInsertTiedElementCommand::unexecute");
	}
	m_voice->m_chord_or_rests = g_list_delete_link(m_voice->m_chord_or_rests, lptr); // friend !!
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}
