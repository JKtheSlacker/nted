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

#include "fixmovedfreeplaceablecommand.h"
#include "chordorrest.h"
#include "freereplaceable.h"
#include "chordorrest.h"
#include "slurpoint.h"
#include "resource.h"


NedFixMovedFreePlaceableCommand::NedFixMovedFreePlaceableCommand(NedFreeReplaceable *freeplaceable) : 
m_freereplaceable(freeplaceable) {
	m_old_element = freeplaceable->m_element_at_move_start; // friend !!
	m_new_element = freeplaceable->m_element; // friend !!
	m_oldx = freeplaceable->m_x_at_start; // friend !!
	m_oldy = freeplaceable->m_y_at_start; // friend !!
	m_newx = freeplaceable->m_x; // friend !!
	m_newy = freeplaceable->m_y; // friend !!
}

void NedFixMovedFreePlaceableCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedFixMovedFreePlaceableCommand::execute\n");
#endif
	m_freereplaceable->freeFromElement();
	m_freereplaceable->setElement(m_new_element);
	m_freereplaceable->setXY(m_newx, m_newy, true);
	if (m_freereplaceable->getType() == FREE_PLACEABLE_SLUR_POINT) {
		((NedSlurPoint *) m_freereplaceable)->recompute();
	}
}


void NedFixMovedFreePlaceableCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedFixMovedFreePlaceableCommand::unexecute\n");
#endif
	m_freereplaceable->freeFromElement();
	m_freereplaceable->setElement(m_old_element);
	m_freereplaceable->setXY(m_oldx, m_oldy, true);
	if (m_freereplaceable->getType() == FREE_PLACEABLE_SLUR_POINT) {
		((NedSlurPoint *) m_freereplaceable)->recompute();
	}
}
