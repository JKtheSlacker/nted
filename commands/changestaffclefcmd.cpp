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

#include "changestaffclefcmd.h"
#include "mainwindow.h"
#include "resource.h"


NedChangeStaffClefCmd::NedChangeStaffClefCmd(NedMainWindow *main_window, int staff_number, int newclef, int new_clef_octave_shift, bool do_adjust) :
m_main_window(main_window), m_newclef(newclef), m_staff_number(staff_number), m_new_clef_octave_shift(new_clef_octave_shift), m_do_adjust(do_adjust) {
	m_oldclef = main_window->m_staff_contexts[staff_number].m_clef_number;
	m_old_clef_octave_shift = main_window->m_staff_contexts[staff_number].m_clef_octave_shift;
	m_new_clef_octave_shift = new_clef_octave_shift;
}


void NedChangeStaffClefCmd::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeStaffClefCmd::execute\n");
#endif
	if (m_do_adjust) {
		m_main_window->shiftNotes(0, NedResource::getClefDist(m_main_window->m_staff_contexts[m_staff_number].m_clef_number,
			m_main_window->m_staff_contexts[m_staff_number].m_clef_octave_shift, m_newclef, m_new_clef_octave_shift), NULL, NULL, m_staff_number);
	}
	m_main_window->m_staff_contexts[m_staff_number].m_clef_number = m_newclef;
	m_main_window->m_staff_contexts[m_staff_number].m_clef_octave_shift = m_new_clef_octave_shift;
	m_main_window->setAndUpdateClefTypeAndKeySig();
}


void NedChangeStaffClefCmd::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\ttNedChangeStaffClefCmd::unexecute\n");
#endif
	if (m_do_adjust) {
		m_main_window->shiftNotes(0, NedResource::getClefDist(m_main_window->m_staff_contexts[m_staff_number].m_clef_number,
			m_main_window->m_staff_contexts[m_staff_number].m_clef_octave_shift, m_oldclef, m_old_clef_octave_shift), NULL, NULL, m_staff_number);
	}
	m_main_window->m_staff_contexts[m_staff_number].m_clef_number = m_oldclef;
	m_main_window->m_staff_contexts[m_staff_number].m_clef_octave_shift = m_old_clef_octave_shift;
	m_main_window->setAndUpdateClefTypeAndKeySig();
}
