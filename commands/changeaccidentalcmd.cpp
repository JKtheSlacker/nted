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

#include "changeaccidentalcmd.h"
#include "note.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedChangeAccidentalCommand::NedChangeAccidentalCommand(NedNote *note, unsigned int newstatus, int newline) :
m_note(note), m_newstatus(newstatus), m_new_line(newline) {
	m_oldstatus = note->m_status; // friend !!
	m_old_line = note->m_line; // friend !!
}

void NedChangeAccidentalCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeAccidentalCommand::execute\n");
#endif
	m_note->m_status = m_newstatus; // friend !!
	m_note->m_line = m_new_line; // friend !!
	m_note->m_ypos = - m_note->m_line * LINE_DIST/2.0; // friend !!
	m_note->getChord()->computeBbox();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}


void NedChangeAccidentalCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeAccidentalCommand::unexecute\n");
#endif
	m_note->m_status = m_oldstatus; // friend !!
	m_note->m_line = m_old_line; // friend !!
	m_note->m_ypos = - m_note->m_line * LINE_DIST/2.0; // friend !!
	m_note->getChord()->computeBbox();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}
