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

#include "transposecommand.h"
#include "resource.h"
#include "mainwindow.h"

NedTransposeCommand::NedTransposeCommand(NedMainWindow *main_window, int pitchdist ,int staff_count, bool *staff_list, GList *selected_group) :
m_main_window(main_window), m_pitch_dist(pitchdist), m_selected_group(NULL) {
	GList *lptr;

	if ((m_staff_list = (bool *) g_try_malloc(staff_count * sizeof(bool))) == NULL) {
		NedResource::Abort("g_try_malloc error\n");
	}
	memcpy(m_staff_list, staff_list, staff_count * sizeof(bool));

	for (lptr = g_list_first(selected_group); lptr; lptr = g_list_next(lptr)) {
		m_selected_group = g_list_append(m_selected_group, lptr->data);
	}
}


void NedTransposeCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedTransposeCommand::execute\n");
#endif
	m_main_window->do_transpose(m_pitch_dist, m_staff_list, m_selected_group);
}


void NedTransposeCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedTransposeCommand::unexecute\n");
#endif
	m_main_window->do_transpose(-m_pitch_dist, m_staff_list, m_selected_group);
}
