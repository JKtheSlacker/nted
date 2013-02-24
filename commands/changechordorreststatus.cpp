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

#include "changechordorreststatus.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedChangeChordOrRestStatusCommand::NedChangeChordOrRestStatusCommand(NedChordOrRest *chord_or_rest, unsigned int newstatus) :
m_chord_or_rest(chord_or_rest), m_newstatus(newstatus) {
	m_oldstatus = chord_or_rest->getStatus();
}

void NedChangeChordOrRestStatusCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeChordOrRestStatusCommand::execute\n");
#endif
	m_chord_or_rest->setStatus(m_newstatus);
	
	if (m_chord_or_rest->getType() != TYPE_REST) m_chord_or_rest->xPositNotes();
	m_chord_or_rest->computeBbox();
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}


void NedChangeChordOrRestStatusCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeChordOrRestStatusCommand::unexecute\n");
#endif
	m_chord_or_rest->setStatus(m_oldstatus);
	if (m_chord_or_rest->getType() != TYPE_REST) m_chord_or_rest->xPositNotes();
	m_chord_or_rest->computeBbox();
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}
