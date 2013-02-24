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

#include "shiftstaffcommand.h"
#include "page.h"
#include "mainwindow.h"
#include "resource.h"

NedShiftStaffCommand::NedShiftStaffCommand(NedMainWindow *main_window, int staff_number, int position) :
m_main_window(main_window), m_staff_number(staff_number), m_position(position) {
	int i;
	if ((m_old_flags = (int *)  g_try_malloc(main_window->m_staff_count * sizeof(int))) == NULL) {
		NedResource::Abort("NedShiftStaffCommand");
	}
	for (i = 0; i < main_window->m_staff_count; i++) {
		m_old_flags[i] = main_window->m_staff_contexts[i].m_flags;
	}

}

NedShiftStaffCommand::~NedShiftStaffCommand() {
	g_free(m_old_flags);
}


void NedShiftStaffCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedShiftStaffCommand::execute\n");
#endif
	for (int i = 0; i < m_main_window->m_staff_count; i++) {
		m_main_window->m_staff_contexts[i].m_flags = 0;
	}
	m_main_window->shiftStaff(m_staff_number, m_position);
}


void NedShiftStaffCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedShiftStaffCommand::unexecute\n");
#endif
	m_main_window->shiftStaff(m_position, m_staff_number);
	for (int i = 0; i < m_main_window->m_staff_count; i++) {
		m_main_window->m_staff_contexts[i].m_flags = m_old_flags[i];
	}
}
