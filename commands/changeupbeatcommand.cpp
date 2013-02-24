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

#include "changeupbeatcommand.h"
#include "mainwindow.h"

NedChangeUpbeadCommand::NedChangeUpbeadCommand(NedMainWindow *main_window, unsigned int new_upbeat_inverse) :
m_main_window(main_window), m_newupbeat_inverse(new_upbeat_inverse) {
	m_oldupbeat_inverse = main_window->m_upbeat_inverse;
}

void NedChangeUpbeadCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeUpbeadCommand::execute\n");
#endif
	m_main_window->m_upbeat_inverse = m_newupbeat_inverse;
}


void NedChangeUpbeadCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeUpbeadCommand::unexecute\n");
#endif
	m_main_window->m_upbeat_inverse = m_oldupbeat_inverse;
}
