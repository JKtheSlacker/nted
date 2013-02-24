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

#include "setstaffcontext.h"
#include "resource.h"

NedSetStaffContext::NedSetStaffContext(NedMainWindow *main_window, staff_context_str *staff_context, int *staff_cout) :
m_main_window(main_window), m_staff_count(staff_cout) {
	m_staff_context = *staff_context;
	m_old_context = m_main_window->m_staff_contexts[*m_staff_count];
}


void NedSetStaffContext::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedSetStaffContext::execute\n");
#endif
	m_main_window->m_staff_contexts[*m_staff_count] = m_staff_context;
	(*m_staff_count)++;
}


void NedSetStaffContext::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedSetStaffContext::unexecute\n");
#endif
	(*m_staff_count)--;
	m_main_window->m_staff_contexts[*m_staff_count] = m_old_context;
}
