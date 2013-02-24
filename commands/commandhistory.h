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

#ifndef COMMAND_HISTORY_H

#define  COMMAND_HISTORY_H
#include <gtk/gtk.h>
#include "config.h"

class NedCommandList;
class NedMainWindow;

class NedCommandHistory {
	public:
		NedCommandHistory(NedMainWindow *main_window);
		void addCommandList(NedCommandList *command_list);
		void execute();
		void unexecute();
		void reset();
		void setSavePoint() {m_save_point = m_current_command_list;}
		bool unsavedOperations() {return m_save_point != m_current_command_list;}
	private:
		bool execute_possible();
		bool unexecute_possible();
		GList *m_command_lists;
		GList *m_current_command_list;
		GList *m_save_point;
		NedMainWindow *m_main_window;
};

#endif /* COMMAND_HISTORY_H */
