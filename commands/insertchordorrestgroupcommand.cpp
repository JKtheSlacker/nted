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

#include "insertchordorrestgroupcommand.h"
#include "voice.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"
#include "system.h"
#include "voice.h"
#include "staff.h"

NedInsertChordOrRestGroupCommand::NedInsertChordOrRestGroupCommand(NedVoice *voice, GList *chords_or_rests, int pos) :
m_voice(voice), m_pos(pos), m_chord_or_rests(chords_or_rests) {}


void NedInsertChordOrRestGroupCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertChordOrRestGroupCommand::execute\n");
#endif
	GList *lptr;
	int pos = m_pos;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) {
		m_voice->m_chord_or_rests = g_list_insert(m_voice->m_chord_or_rests, lptr->data, pos++); // friend !!
		((NedChordOrRest *) lptr->data)->setVoice(m_voice);
	}
}

void NedInsertChordOrRestGroupCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertChordOrRestGroupCommand::unexecute\n");
#endif
	GList *lptr, *lptr2;
	for (lptr = g_list_first(m_chord_or_rests); lptr; lptr = g_list_next(lptr)) { // friend !!
		lptr2 = g_list_find(m_voice->m_chord_or_rests, lptr->data); // friend !!
		if (lptr2 == NULL) {
			NedResource::Abort("NedInsertChordOrRestGroupCommand::execute:1");
		}
		GList *inserted = g_list_find(m_voice->m_chord_or_rests, lptr2->data); // friend !!
		if (inserted == NULL) {
			NedResource::Abort("NedInsertChordOrRestGroupCommand::execute:2");
		}
		m_voice->m_chord_or_rests = g_list_delete_link(m_voice->m_chord_or_rests, inserted); // friend !!
	}
}
