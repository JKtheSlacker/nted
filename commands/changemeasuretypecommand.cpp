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

#include "changemeasuretypecommand.h"
#include "mainwindow.h"
#include "resource.h"


NedChangeMeasureTypeCommand::NedChangeMeasureTypeCommand(NedMainWindow *main_window, int measnum, int mtype, bool hide_following) :
m_main_window(main_window), m_measnum(measnum), m_mtype(mtype), m_hide_following(hide_following) {}


void NedChangeMeasureTypeCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeMeasureTypeCommand::execute\n");
#endif
	m_old_mtype = m_main_window->setSpecial(m_measnum, m_mtype, m_hide_following, &m_old_hide_following, true); // !! friend
}


void NedChangeMeasureTypeCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\ttNedChangeMeasureTypeCommand::unexecute\n");
#endif
	m_main_window->setSpecial(m_measnum, m_old_mtype, m_old_hide_following, NULL, true); // !! friend
}
