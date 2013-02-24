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

#include "idcommandlist.h"
#include "command.h"
#include "idiotseditor.h"

NedIdCommandList::NedIdCommandList(NedIdiotsEditor *id_edit): 
m_commands(NULL), m_id_edit(id_edit) {}

NedIdCommandList::~NedIdCommandList() {
	GList *lptr;

	lptr = g_list_first(m_commands);
	while (lptr != NULL) {
		delete ((NedCommand*) lptr->data);
		m_commands = g_list_delete_link(m_commands, lptr);
		lptr = g_list_first(m_commands);
	}
}

void NedIdCommandList::execute(bool adjust /* = false */) {
	GList *lptr;
#ifdef EXECUTE_DEBUG
	printf("NedIdCommandList::execute\n");
#endif
	for (lptr = g_list_first(m_commands); lptr; lptr = g_list_next(lptr)) {
		((NedCommand*) lptr->data)->execute(adjust);
	}
	m_id_edit->repaint();
#ifdef EXECUTE_DEBUG
	printf("NedIdCommandList::execute END\n");
#endif
}

void NedIdCommandList::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("NedIdCommandList::unexecute\n");
#endif
	GList *lptr;
	for (lptr = g_list_last(m_commands); lptr; lptr = g_list_previous(lptr)) {
		((NedCommand*) lptr->data)->unexecute(adjust);
	}
	m_id_edit->repaint();
#ifdef UNEXECUTE_DEBUG
	printf("NedIdCommandList::unexecute END\n");
#endif
}


void NedIdCommandList::addCommand(NedCommand *command) {
	m_commands = g_list_append(m_commands, command);
}

int NedIdCommandList::getNumberOfCommands() {
	return g_list_length(m_commands);
}


