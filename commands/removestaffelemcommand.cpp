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

#include "removestaffelemcommand.h"
#include "staff.h"
#include "chordorrest.h"
#include "resource.h"
#include "mainwindow.h"

NedRemoveStaffElemCommand::NedRemoveStaffElemCommand(NedMainWindow *main_window, int staff_nr, NedChordOrRest *staffelem, bool do_adjust) :
m_staff_nr(staff_nr), m_staff_elem(staffelem), m_main_window(main_window), m_do_adjust(do_adjust) {
	m_staff = staffelem->getStaff();
}


void NedRemoveStaffElemCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedRemoveStaffElemCommand::execute\n");
#endif
	if (m_do_adjust && (m_staff_elem->getType() & (TYPE_KEYSIG | TYPE_CLEF)) != 0) {
		m_main_window->setInternalPitches();
	}
	m_staff->removeStaffElement(m_staff_elem);
	if (m_do_adjust && (m_staff_elem->getType() & (TYPE_KEYSIG | TYPE_CLEF)) != 0) {
		m_main_window->adjustAccidentals(m_staff_nr);
	}
}


void NedRemoveStaffElemCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedRemoveStaffElemCommand::unexecute\n");
#endif
	if (m_do_adjust && (m_staff_elem->getType() & (TYPE_KEYSIG | TYPE_CLEF)) != 0) {
		m_main_window->setInternalPitches();
	}
	m_staff->insertStaffElement(m_staff_elem);
	if (m_do_adjust && (m_staff_elem->getType() & (TYPE_KEYSIG | TYPE_CLEF)) != 0) {
		m_main_window->adjustAccidentals(m_staff_nr);
	}
}
