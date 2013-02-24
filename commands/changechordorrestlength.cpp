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

#include "changechordorrestlength.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedChangeChordOrRestLengthCommand::NedChangeChordOrRestLengthCommand(NedChordOrRest *chord_or_rest, unsigned int newlength, int newdotcount) :
m_chord_or_rest(chord_or_rest), m_new_dot_count(newdotcount), m_newlength(newlength) {
	m_oldlength = chord_or_rest->getLength();
	m_old_dot_count = chord_or_rest->getDotCount();

}

void NedChangeChordOrRestLengthCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeChordOrRestLengthCommand::execute\n");
#endif
	m_chord_or_rest->setLength(m_newlength);
	m_chord_or_rest->setDotCount(m_new_dot_count);
	if (!m_chord_or_rest->isRest()) {
		m_chord_or_rest->reConfigure();
		m_chord_or_rest->xPositNotes();
	}
	m_chord_or_rest->computeBbox();
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}


void NedChangeChordOrRestLengthCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeChordOrRestLengthCommand::unexecute\n");
#endif
	m_chord_or_rest->setLength(m_oldlength);
	m_chord_or_rest->setDotCount(m_old_dot_count);
	if (!m_chord_or_rest->isRest()) {
		m_chord_or_rest->reConfigure();
		m_chord_or_rest->xPositNotes();
	}
	m_chord_or_rest->computeBbox();
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}
