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

#include "changespacement.h"
#include "mainwindow.h"

NedChangeSpacementCommand::NedChangeSpacementCommand(NedMainWindow *main_window, double newspacement) :
m_main_window(main_window), m_new_spacement(newspacement) {
	m_old_spacement = main_window->m_global_spacement_fac; // !! friend
}

void NedChangeSpacementCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeSpacementCommand::execute\n");
#endif
	m_main_window->realizeNewSpacement(m_new_spacement); // !! friend
}


void NedChangeSpacementCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeSpacementCommand::unexecute\n");
#endif
	m_main_window->realizeNewSpacement(m_old_spacement); // !! friend
}
