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

#include "insertfreeplaceablecommand.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "freereplaceable.h"
#include "chordorrest.h"
#include "resource.h"


NedInsertFreePlaceableCommand::NedInsertFreePlaceableCommand(NedFreeReplaceable *freereplaceable, NedChordOrRest *chord_or_rest, double x, double y) :
m_freereplaceable(freereplaceable), m_chord_or_rest(chord_or_rest), m_x(x), m_y(y) {}


void NedInsertFreePlaceableCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertFreePlaceableCommand::execute\n");
#endif
	m_freereplaceable->setElement(m_chord_or_rest);
	m_freereplaceable->setXY(m_x, m_y, false);
}


void NedInsertFreePlaceableCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertFreePlaceableCommand::unexecute\n");
#endif
	m_freereplaceable->freeFromElement(); // friend!!
}
