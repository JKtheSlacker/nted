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

#ifndef COMMAND_LIST_H

#define  COMMAND_LIST_H
#include <gtk/gtk.h>
#include "config.h"

class NedCommand;
class NedSystem;
class NedMainWindow;

class NedCommandList {
	public:
		NedCommandList(NedMainWindow *main_window, NedSystem *system_for_reposit = NULL);
		~NedCommandList();
		void addCommand(NedCommand *command);
		void addCommandAndSetBreak(NedCommand *command);
		int getNumberOfCommands();
		void execute(bool adjust = false);
		void unexecute(bool adjust = false);
		void executeAndSetBreak();
		void executeAfterBreakAndSetBreak();
		void executeAfterBreak();
		void setBreak();
		void addSystemForRepositioning(NedSystem *system_for_reposit);
		void setFullReposit() {m_full_reposit = true;}
		void setRenumber(NedSystem *m_start_system_for_renumbering = NULL, int start_measure_number = 0, bool force = false);
		void setRenumberPages() {m_do_renumber_pages = true;}
	private:
		GList *m_commands;
		GList *m_break;
		GList *m_systems_for_reposit;
		bool m_full_reposit;
		NedMainWindow *m_main_window;
		int m_old_measure_start_number;
		int m_new_measure_start_number;
		NedSystem *m_start_system_for_renumbering;
		bool m_do_renumber;
		bool m_do_renumber_pages;
		bool m_force;
};

#endif /* COMMAND_LIST_H */
