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

#include "commandlist.h"
#include "command.h"
#include "system.h"
#include "mainwindow.h"

NedCommandList::NedCommandList(NedMainWindow *main_window, NedSystem *system_for_reposit /* = NULL */) :
m_commands(NULL), m_break(NULL), m_systems_for_reposit(NULL), m_full_reposit(false), m_main_window(main_window),
m_start_system_for_renumbering(NULL), m_do_renumber(false), m_do_renumber_pages(false) {
	if (system_for_reposit != NULL) {
		m_systems_for_reposit = g_list_append(m_systems_for_reposit, system_for_reposit);
	}
}

NedCommandList::~NedCommandList() {
	GList *lptr;

	lptr = g_list_first(m_commands);
	while (lptr != NULL) {
		delete ((NedCommand*) lptr->data);
		m_commands = g_list_delete_link(m_commands, lptr);
		lptr = g_list_first(m_commands);
	}
	lptr = g_list_first(m_systems_for_reposit);
	while (lptr != NULL) {
		m_systems_for_reposit = g_list_delete_link(m_systems_for_reposit, lptr);
		lptr = g_list_first(m_systems_for_reposit);
	}
}

void NedCommandList::addSystemForRepositioning(NedSystem *system_for_reposit) {
	if (g_list_find(m_systems_for_reposit, system_for_reposit) != NULL) return;
	m_systems_for_reposit = g_list_append(m_systems_for_reposit, system_for_reposit);
	
}
void NedCommandList::setRenumber(NedSystem *m_start_system_for_renumbering /* = NULL */, int start_measure_number /* = 0 */, bool force /* = false */) {
	m_start_system_for_renumbering = m_start_system_for_renumbering;
	m_do_renumber = true;
	m_force = force;
	if (m_start_system_for_renumbering != NULL) {
		m_old_measure_start_number = m_start_system_for_renumbering->getMeasure(0)->getMeasureNumber();
		m_new_measure_start_number = start_measure_number;
	}
}
	

void NedCommandList::execute(bool adjust /* = false */) {
	GList *lptr;
#ifdef EXECUTE_DEBUG
	printf("NedCommandList::execute\n");
#endif
	for (lptr = g_list_first(m_commands); lptr; lptr = g_list_next(lptr)) {
		((NedCommand*) lptr->data)->execute(adjust);
	}
	if (m_full_reposit) {
		m_main_window->setAllUnpositioned();
		m_main_window->reposit();
	}
	else {
		for (lptr = g_list_first(m_systems_for_reposit); lptr; lptr = g_list_next(lptr)) {
			((NedSystem *) lptr->data)->m_is_positioned = false;
			m_main_window->reposit(NULL, ((NedSystem *) lptr->data)->getPage(), (NedSystem *) lptr->data);
		}
	}
	if (m_do_renumber) {
		if (m_start_system_for_renumbering != NULL) {
			m_main_window->renumberMeasures(m_start_system_for_renumbering, m_new_measure_start_number, m_force);
		}
		else {
			m_main_window->renumberMeasures(NULL, 1, m_force);
		}
	}
	if (m_do_renumber_pages) {
		m_main_window->renumberPages();
	}
	m_main_window->repaint();
#ifdef EXECUTE_DEBUG
	printf("NedCommandList::execute END\n");
#endif
}

void NedCommandList::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("NedCommandList::unexecute\n");
#endif
	GList *lptr;
	for (lptr = g_list_last(m_commands); lptr; lptr = g_list_previous(lptr)) {
		((NedCommand*) lptr->data)->unexecute(adjust);
	}
	if (m_do_renumber) {
		if (m_start_system_for_renumbering != NULL) {
			m_main_window->renumberMeasures(m_start_system_for_renumbering, m_old_measure_start_number, m_force);
		}
		else {
			m_main_window->renumberMeasures(NULL, 1, m_force);
		}
	}
	if (m_full_reposit) {
		m_main_window->setAllUnpositioned();
		m_main_window->reposit();
	}
	else {
		for (lptr = g_list_first(m_systems_for_reposit); lptr; lptr = g_list_next(lptr)) {
			((NedSystem *) lptr->data)->m_is_positioned = false;
			m_main_window->reposit(NULL, ((NedSystem *) lptr->data)->getPage(), (NedSystem *) lptr->data);
		}
	}
	if (m_do_renumber_pages) {
		m_main_window->renumberPages();
	}
	m_main_window->repaint();
#ifdef UNEXECUTE_DEBUG
	printf("NedCommandList::unexecute END\n");
#endif
}

void NedCommandList::executeAndSetBreak() {
	execute();
	m_break =  g_list_last(m_commands);
}

void NedCommandList::executeAfterBreakAndSetBreak() {
	GList *lptr;
	if (m_break == NULL) {
		execute();
		m_break = g_list_last(m_commands);
		return;
	}
	for (lptr = g_list_next(m_break); lptr; lptr = g_list_next(lptr)) {
		((NedCommand*) lptr->data)->execute();
	}
	m_break = g_list_last(m_commands);
}

void NedCommandList::setBreak() {
	m_break = g_list_last(m_commands);
}

void NedCommandList::executeAfterBreak() {
	GList *lptr;
	if (m_break == NULL) {
		execute(); return;
	}
	for (lptr = g_list_next(m_break); lptr; lptr = g_list_next(lptr)) {
		((NedCommand*) lptr->data)->execute();
	}
}


void NedCommandList::addCommand(NedCommand *command) {
	m_commands = g_list_append(m_commands, command);
}

int NedCommandList::getNumberOfCommands() {
	return g_list_length(m_commands);
}

void NedCommandList::addCommandAndSetBreak(NedCommand *command) {
	m_commands = g_list_append(m_commands, command);
	m_break = g_list_last(m_commands);
}
	

