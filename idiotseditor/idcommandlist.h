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

#ifndef ID_COMMAND_LIST_H

#define  ID_COMMAND_LIST_H
#include <gtk/gtk.h>
#include "config.h"

class NedCommand;
class NedIdiotsEditor;

class NedIdCommandList {
	public:
		NedIdCommandList(NedIdiotsEditor *id_edit);
		~NedIdCommandList();
		void addCommand(NedCommand *command);
		int getNumberOfCommands();
		void execute(bool adjust = false);
		void unexecute(bool adjust = false);
	private:
		GList *m_commands;
		NedIdiotsEditor *m_id_edit;
};

#endif /* ID_COMMAND_LIST_H */
