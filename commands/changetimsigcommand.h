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

#ifndef  CHANGE_TIMESIG_COMMAND_H

#define  CHANGE_TIMESIG_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedMainWindow;

class NedChangeTimeSigCommand : public NedCommand {
	public:	
		NedChangeTimeSigCommand(NedMainWindow *main_window, int newnum, int newdenom, unsigned int newsymbol);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedMainWindow *m_main_window;
		int m_new_num, m_new_denom;
		int m_old_num, m_old_denom;
		unsigned int m_old_symbol, m_new_symbol;
		int m_new_upbeat_inverse, m_old_upbeat_inverse;
		
		
};

#endif /* CHANGE_TIMESIG_COMMAND_H */
