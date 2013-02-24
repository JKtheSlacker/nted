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

#include "deletestaffcommand.h"
#include "page.h"
#include "resource.h"

NedDeleteStaffCommand::NedDeleteStaffCommand(NedMainWindow *main_window, int staff_number) :
m_main_window(main_window), m_staff_number(staff_number), m_staff_flags(NULL) {
	m_staff_context = main_window->m_staff_contexts[staff_number];
	m_num_staffs = main_window->m_staff_count; // friend !!
	if ((m_staff_flags = (unsigned int *) g_try_malloc(m_num_staffs * sizeof(unsigned int))) == NULL) {
		NedResource::Abort("NedDeleteStaffCommand::NedDeleteStaffCommand");
	}
	for (int i = 0; i < m_num_staffs; i++) {
		m_staff_flags[i] = main_window->m_staff_contexts[i].m_flags;
	}
}

NedDeleteStaffCommand::~NedDeleteStaffCommand() {
	g_free(m_staff_flags);
}


void NedDeleteStaffCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedDeleteStaffCommand::execute\n");
#endif
	for (int i = 0; i < m_num_staffs; i++) {
		m_main_window->m_staff_contexts[i].m_flags = 0; // !friend
	}
	m_main_window->deleteStaff(m_staff_number);
}


void NedDeleteStaffCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedDeleteStaffCommand::unexecute\n");
#endif
	m_main_window->restoreStaff(m_staff_number, &m_staff_context);
	for (int i = 0; i < m_num_staffs; i++) {
		m_main_window->m_staff_contexts[i].m_flags = m_staff_flags[i]; // !friend
	}
}
