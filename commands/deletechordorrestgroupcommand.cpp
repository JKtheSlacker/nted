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

#include "deletechordorrestgroupcommand.h"
#include "voice.h"
#include "resource.h"

NedDeleteChordOrRestGroupCommand::NedDeleteChordOrRestGroupCommand(NedVoice *voice, GList *group) :
m_voice(voice), m_group(group) {
	GList *lptr2 = g_list_find(voice->m_chord_or_rests, g_list_first(group)->data); // friend !!
	if (lptr2 == NULL) {
		NedResource::Abort("NedDeleteChordOrRestGroupCommand:1");
	}
	if ((m_pos = g_list_position(voice->m_chord_or_rests, lptr2)) < 0) { // friend !!
		printf("m_pos = %d\n", m_pos);
		NedResource::Abort("NedDeleteChordOrRestGroupCommand:2");
	}
}


void NedDeleteChordOrRestGroupCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedDeleteChordOrRestGroupCommand::execute\n");
#endif
	GList *lptr, *lptr2;
	for (lptr = g_list_first(m_group); lptr; lptr = g_list_next(lptr)) {
		lptr2 = g_list_find(m_voice->m_chord_or_rests, lptr->data); // friend !!
		if (lptr2 == NULL) {
			NedResource::Abort("NedDeleteChordOrRestGroupCommand::execute:1");
		}
		GList *inserted = g_list_find(m_voice->m_chord_or_rests, lptr2->data); // friend
		if (inserted == NULL) {
			NedResource::Abort("NedDeleteChordOrRestGroupCommand::execute:2");
		}
		m_voice->m_chord_or_rests = g_list_delete_link(m_voice->m_chord_or_rests, inserted); // friend
	}
}


void NedDeleteChordOrRestGroupCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedDeleteChordOrRestGroupCommand::unexecute\n");
#endif
	GList *lptr;
	int pos = m_pos;
	for (lptr = g_list_first(m_group); lptr; lptr = g_list_next(lptr)) {
		m_voice->m_chord_or_rests = g_list_insert(m_voice->m_chord_or_rests, lptr->data, pos++); // friend !!
	}
}
