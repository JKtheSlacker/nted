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

#include "settotupletcommand.h"
#include "chordorrest.h"
#include "mainwindow.h"

NedSetToTupletCommand::NedSetToTupletCommand(NedChordOrRest *chord_or_rest, int tuplet_val) :
m_chord_or_rest(chord_or_rest), m_tuplet_val(tuplet_val) {}

void NedSetToTupletCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedSetToTupletCommand::execute\n");
#endif
	m_chord_or_rest->setTupletVal(m_tuplet_val);
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}

void NedSetToTupletCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedSetToTupletCommand::unexecute\n");
#endif
	m_chord_or_rest->setTupletVal(0);
	if (adjust) {
		m_chord_or_rest->getMainWindow()->setVisible(m_chord_or_rest);
	}
}

	

