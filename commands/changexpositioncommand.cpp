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

#include "changexpositioncommand.h"
#include "chordorrest.h"
#include "system.h"
#include "mainwindow.h"
#include "tuplet.h"
#include "resource.h"

NedChangeXPositionCommand::NedChangeXPositionCommand(GList **chord_an_rest_list, NedChordOrRest *sibling_chord_or_rest, NedChordOrRest *chord_or_rest, bool before) :
m_chord_an_rest_list(chord_an_rest_list), m_sibling_chord_or_rest(sibling_chord_or_rest), m_chord_or_rest(chord_or_rest), m_before(before) {
	m_tuplet = chord_or_rest->getTupletPtr();
}


void NedChangeXPositionCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeXPositionCommand::execute\n");
#endif
	GList *lptr, *sibling;

	if ((lptr = g_list_find(*m_chord_an_rest_list, m_chord_or_rest)) == NULL) {
		NedResource::Abort("NedChangeXPositionCommand::execute(1)");
	}
	if ((m_from_pos = g_list_index(*m_chord_an_rest_list, m_chord_or_rest)) < 0) {
		NedResource::Abort("NedChangeXPositionCommand::execute(2)");
	}
	if ((sibling = g_list_find(*m_chord_an_rest_list, m_sibling_chord_or_rest)) == NULL) {
		NedResource::Abort("NedChangeXPositionCommand::execute(3)");
	}
	*m_chord_an_rest_list = g_list_delete_link(*m_chord_an_rest_list, lptr);
	if (m_before) {
		*m_chord_an_rest_list = g_list_insert_before(*m_chord_an_rest_list, sibling, m_chord_or_rest);
	}
	else {
		int idx =  g_list_index(*m_chord_an_rest_list, m_sibling_chord_or_rest);
		if (idx < 0) {
			NedResource::Abort("NedChangeXPositionCommand::execute(4)");
		}
		*m_chord_an_rest_list = g_list_insert(*m_chord_an_rest_list, m_chord_or_rest, idx + 1);
	}
	if (m_tuplet != NULL) {
		m_tuplet->reconfigureLast();
	}

	m_chord_or_rest->getMainWindow()->reposit(NULL, m_chord_or_rest->getPage(), m_chord_or_rest->getSystem());
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}

void NedChangeXPositionCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeXPositionCommand::unexecute\n");
#endif
	GList *lptr;

	if ((lptr = g_list_find(*m_chord_an_rest_list, m_chord_or_rest)) == NULL) {
		NedResource::Abort("NedChangeXPositionCommand::unexecute(1)");
	}
	*m_chord_an_rest_list = g_list_delete_link(*m_chord_an_rest_list, lptr);
	*m_chord_an_rest_list = g_list_insert(*m_chord_an_rest_list, m_chord_or_rest, m_from_pos);
	if (m_tuplet != NULL) {
		m_tuplet->reconfigureLast();
	}
	m_chord_or_rest->getMainWindow()->reposit(NULL, m_chord_or_rest->getPage(), m_chord_or_rest->getSystem());
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}
