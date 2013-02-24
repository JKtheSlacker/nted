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

#ifndef DELETE_STAFF_COMMAND_H

#define DELETE_STAFF_COMMAND_H
#include "config.h"
#include "command.h"
#include "mainwindow.h"
#include <gtk/gtk.h>

class NedPage;

class NedDeleteStaffCommand : public NedCommand {
	public:	
		NedDeleteStaffCommand(NedMainWindow *main_window, int staff_number);
		~NedDeleteStaffCommand();
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedMainWindow *m_main_window;
		struct staff_context_str m_staff_context;
		int m_staff_number;
		unsigned int *m_staff_flags;
		int m_num_staffs;
};

#endif /* DELETE_STAFF_COMMAND_H */
