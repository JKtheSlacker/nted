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

#include "changemeasuretimsigcommand.h"
#include "mainwindow.h"
#include "resource.h"


NedChangeMeasureTimeSignatureCommand::NedChangeMeasureTimeSignatureCommand(NedMainWindow *main_window, int measnum, int newnum, int newdenom, unsigned int newsym) :
m_main_window(main_window), m_measnum(measnum), m_new_num(newnum), m_new_denom(newdenom), m_new_symbol(newsym) {}


void NedChangeMeasureTimeSignatureCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeMeasureTimeSignatureCommand::execute\n");
#endif
	m_main_window->setSpecialTimesig(m_measnum, m_new_num, m_new_denom, m_new_symbol, &m_old_num, &m_old_denom, &m_old_symbol); // !! friend
}


void NedChangeMeasureTimeSignatureCommand::unexecute(bool adjust /* = false */) {
	int dummy;
	unsigned int dummy2;
#ifdef UNEXECUTE_DEBUG
	printf("\ttNedChangeMeasureTimeSignatureCommand::unexecute\n");
#endif
	m_main_window->setSpecialTimesig(m_measnum, m_old_num, m_old_denom, m_old_symbol, &dummy, &dummy, &dummy2); // !! friend
}
