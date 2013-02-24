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

#include "tienotescommand.h"
#include "note.h"
#include "mainwindow.h"
#include "resource.h"

NedTieNotesCommand::NedTieNotesCommand(NedNote *note0, NedNote* note1) :
m_note0(note0), m_note1(note1) {m_tie_offs = m_note0->m_tie_offs;}


void NedTieNotesCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedTieNotesCommand::execute\n");
#endif
	m_note0->setTieForward(m_note1);
	m_note1->setTieBackward(m_note0);
	m_note0->m_tie_offs = 0.0;
	if (adjust) {
		m_note0->getMainWindow()->setVisible(m_note0);
	}
}


void NedTieNotesCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedTieNotesCommand::unexecute\n");
#endif
	m_note0->setTieForward(NULL);
	m_note1->setTieBackward(NULL);
	m_note0->m_tie_offs = m_tie_offs;
	if (adjust) {
		m_note0->getMainWindow()->setVisible(m_note0);
	}

}
