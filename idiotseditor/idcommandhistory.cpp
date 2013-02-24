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

#include "idcommandhistory.h"
#include "idcommandlist.h"
#include "idiotseditor.h"

NedIdCommandHistory::NedIdCommandHistory(NedIdiotsEditor *id_edit) :
 m_command_lists(NULL), m_current_command_list(NULL), m_id_edit(id_edit)  {
	
}

void NedIdCommandHistory::reset() {
	/* TODO: free commdand lists */
	m_command_lists = NULL;
	m_current_command_list = NULL;
}

void NedIdCommandHistory::execute() {
	GList *lptr;
	if (m_current_command_list == NULL) {
		if (m_command_lists == NULL) {
			return;
		}
		m_current_command_list = g_list_first(m_command_lists);
		((NedIdCommandList *) m_current_command_list->data)->execute(true);
		return;
	}
	lptr = g_list_next(m_current_command_list);
	if (lptr == NULL) {
		return;
	}
	m_current_command_list = lptr;
	((NedIdCommandList *) m_current_command_list->data)->execute(true);
}

void NedIdCommandHistory::unexecute() {
	if (m_current_command_list == NULL) {
		return;
	}
	((NedIdCommandList *) m_current_command_list->data)->unexecute(true);
	m_current_command_list = g_list_previous(m_current_command_list);
}

void NedIdCommandHistory::addCommandList(NedIdCommandList *command_list) {
	GList *lptr;

	if (m_current_command_list == NULL) {
		// TODO free old command list
		m_command_lists = NULL;
	}
	if (m_command_lists != NULL) {
		if (m_current_command_list == NULL) {
			lptr = m_command_lists;
		}
		else {
			lptr = g_list_next(m_current_command_list);
		}
		while (lptr != NULL) {
			delete ((NedIdCommandList *) lptr->data);
			m_command_lists = g_list_delete_link(m_command_lists, lptr);
			lptr = g_list_next(m_current_command_list);
		}
	}
	m_command_lists = g_list_append(m_command_lists, command_list);
	m_current_command_list = g_list_last(m_command_lists);
}

bool NedIdCommandHistory::unexecute_possible() {
	return m_current_command_list != NULL;
}

bool NedIdCommandHistory::execute_possible() {
	if (m_current_command_list == NULL) {
		if (m_command_lists == NULL) {
			return FALSE;
		}
		return TRUE;
	}
	return g_list_next(m_current_command_list) != NULL;
}
	
