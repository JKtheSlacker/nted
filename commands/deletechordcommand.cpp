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

#include "deletechordcommand.h"
#include "chordorrest.h"
#include "resource.h"
#include "mainwindow.h"


NedDeleteChordCommand::NedDeleteChordCommand(NedChordOrRest *chord) :
m_chord(chord) {
	m_state = m_chord->isRest();
}

void NedDeleteChordCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedDeleteChordCommand::execute\n");
#endif
	m_chord->setToRest(true);
	if (!m_chord->isRest()) {
		m_chord->reConfigure();
		m_chord->xPositNotes();
	}
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}

void NedDeleteChordCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedDeleteChordCommand::unexecute\n");
#endif
	m_chord->setToRest(m_state);
	if (!m_chord->isRest()) {
		m_chord->reConfigure();
		m_chord->xPositNotes();
	}
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}

