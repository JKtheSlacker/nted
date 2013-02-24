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

#include "changetimsigcommand.h"
#include "mainwindow.h"
#include "resource.h"


NedChangeTimeSigCommand::NedChangeTimeSigCommand(NedMainWindow *main_window, int newnum, int newdenom, unsigned int newsymbol) :
m_main_window(main_window), m_new_num(newnum), m_new_denom(newdenom), m_new_symbol(newsymbol) {
	unsigned int duration = m_main_window->m_numerator * WHOLE_NOTE / m_main_window->m_denominator;
	if (duration == m_new_num * WHOLE_NOTE / m_new_denom) {
		m_new_upbeat_inverse = m_main_window->m_upbeat_inverse;
	}
	else {
		m_new_upbeat_inverse = 0;
	}
}


void NedChangeTimeSigCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeTimeSigCommand::execute\n");
#endif
	m_old_num = m_main_window->m_numerator;
	m_old_denom = m_main_window->m_denominator;
	m_old_symbol = m_main_window->m_timesig_symbol;
	m_old_upbeat_inverse = m_main_window->m_upbeat_inverse; // friend !!
	m_main_window->m_numerator = m_new_num; // friend !!
	m_main_window->m_denominator = m_new_denom; // friend !!
	m_main_window->m_timesig_symbol = m_new_symbol; // friend !!
	m_main_window->m_upbeat_inverse = m_new_upbeat_inverse; // friend !!
}


void NedChangeTimeSigCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\ttNedChangeTimeSigCommand::unexecute\n");
#endif
	m_main_window->m_numerator = m_old_num; // friend !!
	m_main_window->m_denominator = m_old_denom; // friend !!
	m_main_window->m_timesig_symbol = m_old_symbol; // friend !!
	m_main_window->m_upbeat_inverse = m_old_upbeat_inverse; // friend !!
}
