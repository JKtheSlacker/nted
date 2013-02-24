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

#include "insertstaffelemcommand.h"
#include "staff.h"
#include "chordorrest.h"
#include "resource.h"
#include "mainwindow.h"

NedInsertStaffElemCommand::NedInsertStaffElemCommand(NedChordOrRest *staffelem, int clef_number_so_far, int octave_shift_so_far, NedStaff *staff, bool do_adjust) :
m_staff(staff), m_staff_elem(staffelem), m_line_dist(0), m_do_adjust(do_adjust) {
	m_staff_nr = staff->getStaffNumber();
	if (m_staff_elem->getType() & (TYPE_CLEF)) {
		m_line_dist = NedResource::getClefDist(clef_number_so_far, octave_shift_so_far,
				m_staff_elem->getLength() /* clef_number */, m_staff_elem->getDotCount() /* octave_shift */);
	}
}
		


void NedInsertStaffElemCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertStaffElemCommand::execute\n");
#endif
	if (m_do_adjust && m_staff_elem->getType() == TYPE_KEYSIG) {
		m_staff->getMainWindow()->setInternalPitches();
	}
	m_staff->insertStaffElement(m_staff_elem);
	if (m_do_adjust && m_staff_elem->getType() == TYPE_KEYSIG) {
		m_staff->getMainWindow()->adjustAccidentals(m_staff_nr);
	}
	else if (m_do_adjust && (m_staff_elem->getType() & (TYPE_CLEF | TYPE_STAFF_SIGN))) {
			
		m_staff->getMainWindow()->shiftNotes(m_staff_elem->getMidiTime(), m_line_dist, m_staff->getPage(), m_staff->getSystem(), m_staff->getStaffNumber()); 
	}
}


void NedInsertStaffElemCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertStaffElemCommand::unexecute\n");
#endif
	if (m_do_adjust && m_staff_elem->getType() == TYPE_KEYSIG) {
		m_staff->getMainWindow()->setInternalPitches();
	}
	m_staff->removeStaffElement(m_staff_elem);
	if (m_do_adjust && m_staff_elem->getType() == TYPE_KEYSIG) {
		m_staff->getMainWindow()->adjustAccidentals(m_staff_nr);
	}
	else if (m_do_adjust && (m_staff_elem->getType() & (TYPE_CLEF | TYPE_STAFF_SIGN))) {
		m_staff->getMainWindow()->shiftNotes(m_staff_elem->getMidiTime(), -m_line_dist, m_staff->getPage(), m_staff->getSystem(), m_staff->getStaffNumber()); 
	}
}
