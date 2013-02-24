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

#include "changetie.h"
#include "mainwindow.h"
#include "note.h"

NedChangeTieCommand::NedChangeTieCommand(NedNote *note, double offs) :
m_note(note), m_offs(offs) {}

void NedChangeTieCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeTieCommand::execute\n");
#endif
	m_note->m_tie_offs += m_offs;
	m_note->computeTie();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}


void NedChangeTieCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeTieCommand::unexecute\n");
#endif
	m_note->m_tie_offs -= m_offs;
	m_note->computeTie();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}
