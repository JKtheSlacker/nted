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

#include "restmovecommand.h"
#include "chordorrest.h"
#include "mainwindow.h"

NedRestMoveCommand::NedRestMoveCommand(NedChordOrRest *rest, int from, int to) :
m_rest(rest), m_from(from), m_to(to) {}

void NedRestMoveCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedRestMoveCommand::execute\n");
#endif
	m_rest->moveToLine(m_to);
	m_rest->computeBbox();
	if (adjust) {
		m_rest->getMainWindow()->setVisible(m_rest);
	}
}

void NedRestMoveCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedRestMoveCommand::unexecute\n");
#endif
	m_rest->moveToLine(m_from);
	m_rest->computeBbox();
	if (adjust) {
		m_rest->getMainWindow()->setVisible(m_rest);
	}
}

	

