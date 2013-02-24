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

#include "appendsystemcommand.h"
#include "system.h"
#include "mainwindow.h"
#include "page.h"


NedAppendSystemCommand::NedAppendSystemCommand(NedPage *page):
m_page(page) { 
	int sys_nr, meas_nr;

	sys_nr = g_list_length(page->m_systems); // friend !!
	meas_nr = page->getNumberOfLastMeasure() + 1;
	m_system = new NedSystem(page, 10.0 /* dummy */, m_page->m_width - 2 * (LEFT_RIGHT_BORDER - DEFAULT_BORDER), sys_nr, meas_nr, TRUE);
}

NedAppendSystemCommand::~NedAppendSystemCommand() {
	delete m_system;
}

void NedAppendSystemCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedAppendSystemCommand::execute\n");
#endif
	m_page->m_systems = g_list_append(m_page->m_systems, m_system);
	m_page->placeStaffs(0);
	m_page->getMainWindow()->setAndUpdateClefTypeAndKeySig();
}

void NedAppendSystemCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedAppendSystemCommand::unexecute\n");
#endif
	m_page->m_systems = g_list_remove(m_page->m_systems, m_system);
	m_page->placeStaffs(0);
}

