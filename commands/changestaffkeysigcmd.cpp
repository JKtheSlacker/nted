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

#include "changestaffkeysigcmd.h"
#include "mainwindow.h"
#include "resource.h"


NedChangeStaffKeysigCmd::NedChangeStaffKeysigCmd(NedMainWindow *main_window, int staff_number, int newkey, bool do_adjust) :
m_main_window(main_window), m_newkey(newkey), m_staff_number(staff_number), m_do_adjust(do_adjust) {
	m_oldkey = main_window->m_staff_contexts[staff_number].m_key_signature_number;
}


void NedChangeStaffKeysigCmd::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeStaffKeysigCmd::execute\n");
#endif
	if (m_do_adjust) {
		m_main_window->setInternalPitches();
	}
	m_main_window->m_staff_contexts[m_staff_number].m_key_signature_number = m_newkey;
	if (m_do_adjust) {
		m_main_window->adjustAccidentals(m_staff_number);
	}
}


void NedChangeStaffKeysigCmd::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\ttNedChangeStaffKeysigCmd::unexecute\n");
#endif
	if (m_do_adjust) {
		m_main_window->setInternalPitches();
	}
	m_main_window->m_staff_contexts[m_staff_number].m_key_signature_number = m_oldkey;
	if (m_do_adjust) {
		m_main_window->adjustAccidentals(m_staff_number);
	}
}
