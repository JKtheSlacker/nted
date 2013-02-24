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

#include "commandhistory.h"
#include "commandlist.h"
#include "mainwindow.h"

NedCommandHistory::NedCommandHistory(NedMainWindow *main_window) :
 m_command_lists(NULL), m_current_command_list(NULL), m_save_point(NULL), m_main_window(main_window)  {
	
}

void NedCommandHistory::reset() {
	/* TODO: free commdand lists */
	m_command_lists = NULL;
	m_current_command_list = NULL;
	m_save_point = NULL;
	m_main_window->setUnRedoButtons(execute_possible(),unexecute_possible());
}

void NedCommandHistory::execute() {
	GList *lptr;
	if (m_current_command_list == NULL) {
		if (m_command_lists == NULL) {
			return;
		}
		m_current_command_list = g_list_first(m_command_lists);
		((NedCommandList *) m_current_command_list->data)->execute(true);
		m_main_window->setUnRedoButtons(execute_possible(),unexecute_possible());
		return;
	}
	lptr = g_list_next(m_current_command_list);
	if (lptr == NULL) {
		return;
	}
	m_current_command_list = lptr;
	((NedCommandList *) m_current_command_list->data)->execute(true);
	m_main_window->setUnRedoButtons(execute_possible(),unexecute_possible());
}

void NedCommandHistory::unexecute() {
	if (m_current_command_list == NULL) {
		return;
	}
	((NedCommandList *) m_current_command_list->data)->unexecute(true);
	m_current_command_list = g_list_previous(m_current_command_list);
	m_main_window->setUnRedoButtons(execute_possible(),unexecute_possible());
	
}

void NedCommandHistory::addCommandList(NedCommandList *command_list) {
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
			delete ((NedCommandList *) lptr->data);
			m_command_lists = g_list_delete_link(m_command_lists, lptr);
			lptr = g_list_next(m_current_command_list);
		}
	}
	m_command_lists = g_list_append(m_command_lists, command_list);
	m_current_command_list = g_list_last(m_command_lists);
	m_main_window->setUnRedoButtons(execute_possible(),unexecute_possible());
}

bool NedCommandHistory::unexecute_possible() {
	return m_current_command_list != NULL;
}

bool NedCommandHistory::execute_possible() {
	if (m_current_command_list == NULL) {
		if (m_command_lists == NULL) {
			return FALSE;
		}
		return TRUE;
	}
	return g_list_next(m_current_command_list) != NULL;
}
	

